#ifndef WORKER_H
#define WORKER_H

#include <stdbool.h>
#include <sys/types.h>

/* Status satu proses anak beserta hasil akhirnya. */
typedef struct {
    pid_t pid;
    unsigned int index;
    unsigned int workload_ms;
    bool finished;
    bool failed;
    int exit_status;
} worker_handle;

/* Membuat anak dan menunggu SIGSTOP awal; workload dimulai setelah SIGCONT. */
bool worker_spawn(worker_handle *worker, unsigned int index,
                  unsigned int workload_ms);

/* Memeriksa tanpa menunggu apakah proses anak sudah selesai. */
bool worker_collect(worker_handle *worker);

/* Menunggu proses anak sampai selesai. */
void worker_join(worker_handle *worker);

/*
 * Menghentikan proses anak: SIGTERM, lalu SIGKILL bila masih hidup setelah
 * grace_ms. Proses selalu di-wait sehingga tidak meninggalkan zombie.
 */
bool worker_terminate(worker_handle *worker, unsigned int grace_ms);

/* Memanen proses anak yang masih tersisa, mengembalikan jumlahnya. */
unsigned int worker_reap_orphans(void);

const char *worker_state_text(const worker_handle *worker);

#endif
