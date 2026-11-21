#include "cli.h"
#include "kernel_bridge.h"
#include "runtime_signals.h"
#include "scheduler.h"
#include "timesharing.h"
#include "worker.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <time.h>

#define CLEANUP_GRACE_MS 200U

/* Catatan hasil satu proses selama simulasi. */
typedef struct {
    worker_handle worker;
    unsigned int slices;
    double granted_ms;
    double finish_ms;
} process_record;

static double monotonic_ms(void)
{
    struct timespec ts;

    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0.0;
    }
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1000000.0;
}

static process_record *find_record(process_record *records, unsigned int count,
                                   pid_t pid)
{
    for (unsigned int i = 0U; i < count; ++i) {
        if (records[i].worker.pid == pid) {
            return &records[i];
        }
    }
    return NULL;
}

/* Mengumpulkan proses anak yang sudah selesai tanpa menunggu. */
static void collect_finished(process_record *records, unsigned int count,
                             double start_ms)
{
    for (unsigned int i = 0U; i < count; ++i) {
        if (records[i].worker.finished) {
            continue;
        }
        if (worker_collect(&records[i].worker)) {
            records[i].finish_ms = monotonic_ms() - start_ms;
        }
    }
}

static unsigned int spawn_all(process_record *records,
                              const timesharing_config *config,
                              round_robin *rr)
{
    unsigned int spawned = 0U;

    for (unsigned int i = 0U; i < config->process_count; ++i) {
        process_record *record = &records[i];

        record->slices = 0U;
        record->granted_ms = 0.0;
        record->finish_ms = -1.0;

        if (!worker_spawn(&record->worker, i + 1U, config->workload_ms)) {
            break;
        }
        /* Proses ditahan sampai gilirannya tiba. */
        runtime_stop(record->worker.pid);
        if (!rr_admit(rr, record->worker.pid, config->workload_ms)) {
            fprintf(stderr, "Antrean penuh, proses %u tidak dijadwalkan.\n",
                    i + 1U);
            break;
        }
        spawned += 1U;
    }
    return spawned;
}

static void run_simulation(process_record *records, unsigned int count,
                           round_robin *rr, double start_ms)
{
    while (rr_has_work(rr) && !runtime_interrupted()) {
        timesharing_process current;

        if (!rr_next(rr, &current)) {
            break;
        }

        process_record *record = find_record(records, count, current.pid);
        if (record == NULL || record->worker.finished) {
            continue;
        }

        const unsigned int slice = rr_slice_ms(rr, &current);

        printf("Giliran %u: proses %u (pid %ld) jatah %u ms, sisa %u ms\n",
               rr->switch_count, record->worker.index,
               (long)current.pid, slice, current.remaining_ms);
        fflush(stdout);

        const double slice_start = monotonic_ms();

        (void)kernel_bridge_mark(current.pid, slice);
        runtime_resume(current.pid);
        if (!runtime_timer_start(slice)) {
            fprintf(stderr, "Timer quantum gagal dijalankan.\n");
            runtime_stop(current.pid);
            break;
        }

        const runtime_event event = runtime_wait_event();

        runtime_timer_stop();
        runtime_stop(current.pid);

        const double used = monotonic_ms() - slice_start;

        collect_finished(records, count, start_ms);
        runtime_clear_child_event();

        record->slices += 1U;
        record->granted_ms += used;
        rr_charge(&current, (unsigned int)used);

        if (record->worker.finished) {
            rr_complete(rr, current.pid);
            printf("  proses %u selesai setelah %u giliran\n",
                   record->worker.index, record->slices);
        } else if (event == RUNTIME_EVENT_INTERRUPT) {
            break;
        } else {
            rr_requeue(rr, current);
        }
        fflush(stdout);
    }
}

static unsigned int count_failed(const process_record *records,
                                 unsigned int count)
{
    unsigned int failed = 0U;

    for (unsigned int i = 0U; i < count; ++i) {
        if (records[i].worker.failed) {
            failed += 1U;
        }
    }
    return failed;
}

/*
 * Menutup semua proses anak. Saat dihentikan lewat signal, proses diminta
 * keluar; pada akhir normal proses dilanjutkan sampai bebannya habis.
 */
static void cleanup_workers(process_record *records, unsigned int count,
                            bool interrupted, double start_ms)
{
    for (unsigned int i = 0U; i < count; ++i) {
        process_record *record = &records[i];

        if (record->worker.finished) {
            continue;
        }
        if (interrupted) {
            printf("Menghentikan proses %u (pid %ld)\n",
                   record->worker.index, (long)record->worker.pid);
            (void)worker_terminate(&record->worker, CLEANUP_GRACE_MS);
        } else {
            runtime_resume(record->worker.pid);
            worker_join(&record->worker);
        }
        record->finish_ms = monotonic_ms() - start_ms;
    }

    const unsigned int orphans = worker_reap_orphans();

    if (orphans > 0U) {
        printf("%u proses anak sisa ikut dibersihkan.\n", orphans);
    }
}

static void print_report(const process_record *records, unsigned int count,
                         const round_robin *rr, double total_ms)
{
    puts("");
    puts("Laporan hasil simulasi");
    puts("proses  pid      giliran  waktu(ms)  selesai(ms)  status");

    for (unsigned int i = 0U; i < count; ++i) {
        const process_record *record = &records[i];
        char finish_text[16];

        if (record->finish_ms >= 0.0) {
            snprintf(finish_text, sizeof finish_text, "%.0f",
                     record->finish_ms);
        } else {
            snprintf(finish_text, sizeof finish_text, "-");
        }
        printf("%-7u %-8ld %-8u %-10.0f %-12s %s\n",
               record->worker.index, (long)record->worker.pid,
               record->slices, record->granted_ms, finish_text,
               worker_state_text(&record->worker));
    }

    printf("Total pergantian giliran: %u\n", rr->switch_count);
    printf("Proses selesai: %u dari %u\n", rr->completed, count);
    printf("Proses gagal: %u\n", count_failed(records, count));
    printf("Giliran tercatat kernel: %lu\n", kernel_bridge_marks());
    printf("Durasi simulasi: %.0f ms\n", total_ms);
}

int main(int argc, char *argv[])
{
    timesharing_config config = {
        .process_count = TIMESHARING_DEFAULT_PROCESS_COUNT,
        .quantum_ms = TIMESHARING_DEFAULT_QUANTUM_MS,
        .workload_ms = TIMESHARING_DEFAULT_WORKLOAD_MS
    };
    process_record records[TIMESHARING_MAX_PROCESSES];
    round_robin rr;

    printf("Time Sharing System v%s\n", TIMESHARING_VERSION);

    switch (cli_read_config(argc, argv, &config)) {
    case CLI_HELP:
        cli_print_usage(argv[0]);
        return 0;
    case CLI_VERSION:
        return 0;
    case CLI_ERROR:
        return 2;
    case CLI_OK:
        break;
    }

    printf("Konfigurasi: %u proses, quantum %u ms, beban %u ms per proses.\n",
           config.process_count, config.quantum_ms, config.workload_ms);

    if (!runtime_signals_install()) {
        fprintf(stderr, "Penyiapan signal dan timer gagal.\n");
        return 1;
    }

    const bool kernel_logging = kernel_bridge_probe();

    printf("Fitur kernel: %s\n", kernel_bridge_status());
    if (!kernel_logging) {
        puts("Pencatatan ke kernel dilewati, simulasi tetap berjalan.");
    }

    rr_init(&rr, config.quantum_ms);

    const double start_ms = monotonic_ms();
    const unsigned int spawned = spawn_all(records, &config, &rr);

    if (spawned == 0U) {
        fprintf(stderr, "Tidak ada proses yang berhasil dibuat.\n");
        runtime_signals_restore();
        return 1;
    }
    if (spawned < config.process_count) {
        fprintf(stderr, "Peringatan: hanya %u dari %u proses dibuat.\n",
                spawned, config.process_count);
    }
    printf("%u proses anak dibuat, penjadwalan Round Robin dimulai.\n",
           spawned);
    puts("Tekan Ctrl+C untuk menghentikan simulasi lebih awal.");

    run_simulation(records, spawned, &rr, start_ms);

    const bool interrupted = runtime_interrupted();

    cleanup_workers(records, spawned, interrupted, start_ms);
    print_report(records, spawned, &rr, monotonic_ms() - start_ms);
    runtime_signals_restore();

    if (interrupted) {
        puts("Simulasi dihentikan lewat signal, proses anak sudah dibersihkan.");
        return 130;
    }
    if (count_failed(records, spawned) > 0U) {
        puts("Sebagian proses berakhir tidak normal.");
        return 1;
    }
    return 0;
}
