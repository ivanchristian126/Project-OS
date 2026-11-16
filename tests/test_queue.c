#include "scheduler.h"

#include <stdio.h>

static unsigned int g_total;
static unsigned int g_failed;

static void check(bool condition, const char *name)
{
    g_total += 1U;
    if (condition) {
        printf("PASS %s\n", name);
    } else {
        printf("FAIL %s\n", name);
        g_failed += 1U;
    }
}

static void test_queue_order(void)
{
    timesharing_queue queue;
    timesharing_process item;
    bool order_ok = true;

    queue_init(&queue);
    for (pid_t pid = 1; pid <= 3; ++pid) {
        const timesharing_process process = {
            .pid = pid,
            .remaining_ms = 100U
        };
        order_ok = order_ok && queue_push(&queue, process);
    }
    check(queue_size(&queue) == 3U, "antrean memuat tiga proses");

    for (pid_t pid = 1; pid <= 3; ++pid) {
        if (!queue_pop(&queue, &item) || item.pid != pid) {
            order_ok = false;
        }
    }
    check(order_ok, "proses keluar sesuai urutan masuk");
    check(queue_is_empty(&queue), "antrean kosong setelah semua diambil");
}

static void test_queue_reject(void)
{
    timesharing_queue queue;
    const timesharing_process valid = { .pid = 5, .remaining_ms = 100U };
    const timesharing_process invalid = { .pid = 0, .remaining_ms = 100U };

    queue_init(&queue);
    check(queue_push(&queue, valid), "pid valid diterima");
    check(!queue_push(&queue, valid), "pid duplikat ditolak");
    check(!queue_push(&queue, invalid), "pid nol ditolak");

    for (pid_t pid = 100; queue_size(&queue) < queue_capacity(); ++pid) {
        const timesharing_process filler = {
            .pid = pid,
            .remaining_ms = 10U
        };
        if (!queue_push(&queue, filler)) {
            break;
        }
    }
    check(queue_is_full(&queue), "antrean terisi penuh");

    const timesharing_process overflow = { .pid = 999, .remaining_ms = 10U };
    check(!queue_push(&queue, overflow), "antrean penuh menolak proses baru");
}

static void test_queue_remove(void)
{
    timesharing_queue queue;
    timesharing_process item;

    queue_init(&queue);
    for (pid_t pid = 1; pid <= 3; ++pid) {
        const timesharing_process process = {
            .pid = pid,
            .remaining_ms = 100U
        };
        (void)queue_push(&queue, process);
    }

    check(queue_remove(&queue, 2), "proses di tengah dapat dihapus");
    check(!queue_remove(&queue, 2), "pid yang sudah dihapus tidak ditemukan");
    check(!queue_find(&queue, 2, NULL), "pid terhapus tidak ada di antrean");
    check(queue_size(&queue) == 2U, "ukuran antrean berkurang satu");

    const bool first_ok = queue_pop(&queue, &item) && item.pid == 1;
    const bool second_ok = queue_pop(&queue, &item) && item.pid == 3;

    check(first_ok && second_ok, "urutan proses lain tetap terjaga");
}

static void test_queue_wraparound(void)
{
    timesharing_queue queue;
    timesharing_process item;
    const unsigned int half = queue_capacity() / 2U;
    bool order_ok = true;

    queue_init(&queue);
    for (pid_t pid = 1; (unsigned int)pid <= queue_capacity(); ++pid) {
        const timesharing_process process = {
            .pid = pid,
            .remaining_ms = 50U
        };
        order_ok = order_ok && queue_push(&queue, process);
    }
    for (unsigned int i = 0U; i < half; ++i) {
        order_ok = order_ok && queue_pop(&queue, &item);
    }
    for (unsigned int i = 0U; i < half; ++i) {
        const timesharing_process process = {
            .pid = (pid_t)(queue_capacity() + i + 1U),
            .remaining_ms = 50U
        };
        order_ok = order_ok && queue_push(&queue, process);
    }
    check(order_ok && queue_size(&queue) == queue_capacity(),
          "antrean sirkular terisi kembali setelah berputar");

    const bool head_ok = queue_peek(&queue, &item) &&
                         item.pid == (pid_t)(half + 1U);
    check(head_ok, "entri terdepan benar setelah berputar");
}

static void test_rr_single_process(void)
{
    round_robin rr;
    timesharing_process current;
    unsigned int slices = 0U;

    rr_init(&rr, 100U);
    check(rr_admit(&rr, 7, 250U), "satu proses masuk antrean siap");

    while (rr_next(&rr, &current)) {
        rr_charge(&current, rr_slice_ms(&rr, &current));
        slices += 1U;
        if (current.remaining_ms == 0U) {
            rr_complete(&rr, current.pid);
            break;
        }
        rr_requeue(&rr, current);
    }
    check(slices == 3U, "beban 250 ms dengan quantum 100 ms butuh 3 giliran");
    check(!rr_has_work(&rr), "antrean kosong setelah proses tunggal selesai");
    check(rr.completed == 1U, "satu proses tercatat selesai");
}

static void test_rr_many_processes(void)
{
    round_robin rr;
    bool order_ok = true;

    rr_init(&rr, 100U);
    for (pid_t pid = 1; pid <= 4; ++pid) {
        order_ok = order_ok && rr_admit(&rr, pid, 300U);
    }

    for (unsigned int round = 0U; round < 3U; ++round) {
        for (pid_t pid = 1; pid <= 4; ++pid) {
            timesharing_process current;

            if (!rr_next(&rr, &current) || current.pid != pid) {
                order_ok = false;
                continue;
            }
            rr_charge(&current, 100U);
            rr_requeue(&rr, current);
        }
    }
    check(order_ok, "giliran berputar sesuai urutan antrean");
    check(rr.switch_count == 12U, "dua belas pergantian giliran tercatat");
    check(rr_pending(&rr) == 4U, "empat proses masih menunggu giliran");
}

static void test_rr_early_finish(void)
{
    round_robin rr;
    timesharing_process current;

    rr_init(&rr, 100U);
    (void)rr_admit(&rr, 10, 100U);
    (void)rr_admit(&rr, 11, 100U);
    (void)rr_admit(&rr, 12, 100U);

    (void)rr_next(&rr, &current);
    (void)rr_requeue(&rr, current);

    (void)rr_next(&rr, &current);
    const bool middle_ok = current.pid == 11;
    (void)rr_complete(&rr, current.pid);

    (void)rr_next(&rr, &current);
    const bool third_ok = current.pid == 12;
    (void)rr_requeue(&rr, current);

    check(middle_ok && third_ok, "giliran kedua dan ketiga sesuai urutan");
    check(!queue_find(&rr.ready, 11, NULL),
          "proses yang selesai tidak kembali ke antrean");
    check(rr_pending(&rr) == 2U, "dua proses tersisa di antrean");

    (void)rr_next(&rr, &current);
    check(current.pid == 10, "proses selesai dilewati pada putaran berikutnya");
}

static void test_rr_last_slice(void)
{
    round_robin rr;
    const timesharing_process tail = { .pid = 3, .remaining_ms = 30U };
    const timesharing_process full = { .pid = 4, .remaining_ms = 450U };

    rr_init(&rr, 100U);
    check(rr_slice_ms(&rr, &tail) == 30U, "sisa kecil memakai jatah sisa");
    check(rr_slice_ms(&rr, &full) == 100U, "sisa besar memakai satu quantum");

    timesharing_process process = tail;
    rr_charge(&process, 100U);
    check(process.remaining_ms == 0U, "sisa waktu tidak pernah negatif");
}

int main(void)
{
    puts("Uji antrean proses dan penjadwalan Round Robin");

    test_queue_order();
    test_queue_reject();
    test_queue_remove();
    test_queue_wraparound();
    test_rr_single_process();
    test_rr_many_processes();
    test_rr_early_finish();
    test_rr_last_slice();

    printf("\n%u dari %u pemeriksaan lulus\n", g_total - g_failed, g_total);
    return (g_failed == 0U) ? 0 : 1;
}
