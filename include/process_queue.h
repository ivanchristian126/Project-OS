#ifndef PROCESS_QUEUE_H
#define PROCESS_QUEUE_H

#include "timesharing.h"

#include <stdbool.h>
#include <sys/types.h>

/* Satu entri proses di dalam antrean. */
typedef struct {
    pid_t pid;                  /* PID proses, harus > 0. */
    unsigned int remaining_ms;  /* Sisa waktu eksekusi yang diminta. */
} timesharing_process;

/*
 * Antrean proses FIFO dengan kapasitas tetap
 * (TIMESHARING_MAX_PROCESSES) berbasis buffer sirkular.
 */
typedef struct {
    timesharing_process items[TIMESHARING_MAX_PROCESSES];
    unsigned int head;  /* Indeks entri terdepan. */
    unsigned int count; /* Jumlah entri yang terpakai. */
} timesharing_queue;

/* Mengosongkan antrean. Wajib dipanggil sebelum operasi lain. */
void queue_init(timesharing_queue *queue);

/* Jumlah proses di dalam antrean. */
unsigned int queue_size(const timesharing_queue *queue);

/* Kapasitas maksimum antrean. */
unsigned int queue_capacity(void);

bool queue_is_empty(const timesharing_queue *queue);
bool queue_is_full(const timesharing_queue *queue);

/*
 * Menambah proses ke ekor antrean.
 * Mengembalikan false jika antrean penuh, PID tidak valid (<= 0),
 * atau PID sudah terdaftar.
 */
bool queue_push(timesharing_queue *queue, timesharing_process process);

/*
 * Mengambil proses terdepan dan menghapusnya dari antrean.
 * Hasil disalin ke *out bila out tidak NULL.
 * Mengembalikan false jika antrean kosong.
 */
bool queue_pop(timesharing_queue *queue, timesharing_process *out);

/*
 * Membaca proses terdepan tanpa menghapusnya.
 * Mengembalikan false jika antrean kosong.
 */
bool queue_peek(const timesharing_queue *queue, timesharing_process *out);

/*
 * Menghapus proses dengan PID tertentu dari posisi mana pun.
 * Urutan proses lain tetap dipertahankan.
 * Mengembalikan false jika PID tidak ditemukan.
 */
bool queue_remove(timesharing_queue *queue, pid_t pid);

/* Mencari proses berdasarkan PID tanpa mengubah antrean. */
bool queue_find(const timesharing_queue *queue, pid_t pid,
                timesharing_process *out);

/* Menghapus seluruh proses dari antrean. */
void queue_clear(timesharing_queue *queue);

#endif
