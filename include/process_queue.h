#ifndef PROCESS_QUEUE_H
#define PROCESS_QUEUE_H

#include "timesharing.h"

#include <stdbool.h>
#include <sys/types.h>

/* Satu proses di dalam antrean. */
typedef struct {
    pid_t pid;
    unsigned int remaining_ms;
} timesharing_process;

/* Antrean FIFO berbasis buffer sirkular dengan kapasitas tetap. */
typedef struct {
    timesharing_process items[TIMESHARING_MAX_PROCESSES];
    unsigned int head;
    unsigned int count;
} timesharing_queue;

void queue_init(timesharing_queue *queue);
unsigned int queue_size(const timesharing_queue *queue);
unsigned int queue_capacity(void);
bool queue_is_empty(const timesharing_queue *queue);
bool queue_is_full(const timesharing_queue *queue);

/* Menambah ke ekor. Gagal bila penuh, pid <= 0, atau pid duplikat. */
bool queue_push(timesharing_queue *queue, timesharing_process process);

/* Mengambil entri terdepan sekaligus mengeluarkannya. */
bool queue_pop(timesharing_queue *queue, timesharing_process *out);

/* Membaca entri terdepan tanpa mengeluarkannya. */
bool queue_peek(const timesharing_queue *queue, timesharing_process *out);

/* Menghapus berdasarkan pid; urutan entri lain tetap terjaga. */
bool queue_remove(timesharing_queue *queue, pid_t pid);

bool queue_find(const timesharing_queue *queue, pid_t pid,
                timesharing_process *out);
void queue_clear(timesharing_queue *queue);

#endif
