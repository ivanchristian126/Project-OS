#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process_queue.h"

/* Penjadwal Round Robin yang bekerja di atas antrean proses. */
typedef struct {
    timesharing_queue ready;
    unsigned int quantum_ms;
    unsigned int switch_count;
    unsigned int completed;
} round_robin;

void rr_init(round_robin *rr, unsigned int quantum_ms);

/* Memasukkan proses ke antrean siap. */
bool rr_admit(round_robin *rr, pid_t pid, unsigned int remaining_ms);

/* Mengambil proses yang mendapat giliran berikutnya. */
bool rr_next(round_robin *rr, timesharing_process *out);

/* Mengembalikan proses ke ekor antrean setelah quantumnya habis. */
bool rr_requeue(round_robin *rr, timesharing_process process);

/* Jatah waktu giliran ini: quantum, atau sisa waktu bila lebih kecil. */
unsigned int rr_slice_ms(const round_robin *rr,
                         const timesharing_process *process);

/* Mengurangi sisa waktu proses sesuai waktu yang sudah terpakai. */
void rr_charge(timesharing_process *process, unsigned int used_ms);

/* Menandai proses selesai sehingga tidak mendapat giliran lagi. */
bool rr_complete(round_robin *rr, pid_t pid);

unsigned int rr_pending(const round_robin *rr);
bool rr_has_work(const round_robin *rr);

#endif
