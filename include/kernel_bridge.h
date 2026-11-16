#ifndef KERNEL_BRIDGE_H
#define KERNEL_BRIDGE_H

#include <stdbool.h>
#include <sys/types.h>

/* Memeriksa sekali apakah syscall ts_mark tersedia di kernel berjalan. */
bool kernel_bridge_probe(void);

/* Mencatat satu giliran ke kernel; false bila fitur tidak tersedia. */
bool kernel_bridge_mark(pid_t pid, unsigned int slice_ms);

/* Penjelasan status fitur untuk ditampilkan ke pengguna. */
const char *kernel_bridge_status(void);

/* Jumlah giliran yang berhasil dicatat kernel pada sesi ini. */
unsigned long kernel_bridge_marks(void);

#endif
