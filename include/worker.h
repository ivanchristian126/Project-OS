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

/* Membuat proses anak lewat fork() untuk menjalankan beban terbatas. */
bool worker_spawn(worker_handle *worker, unsigned int index,
                  unsigned int workload_ms);

/* Memeriksa tanpa menunggu apakah proses anak sudah selesai. */
bool worker_collect(worker_handle *worker);

/* Menunggu proses anak sampai selesai. */
void worker_join(worker_handle *worker);

const char *worker_state_text(const worker_handle *worker);

#endif
