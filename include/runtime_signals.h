#ifndef RUNTIME_SIGNALS_H
#define RUNTIME_SIGNALS_H

#include <stdbool.h>
#include <sys/types.h>

/* Kejadian yang mengakhiri satu giliran. */
typedef enum {
    RUNTIME_EVENT_QUANTUM,
    RUNTIME_EVENT_CHILD,
    RUNTIME_EVENT_INTERRUPT
} runtime_event;

/* Memasang handler SIGALRM, SIGCHLD, SIGINT, SIGTERM dan timer quantum. */
bool runtime_signals_install(void);

/* Menghentikan giliran proses anak lewat SIGSTOP. */
bool runtime_stop(pid_t pid);

/* Melanjutkan proses anak lewat SIGCONT. */
bool runtime_resume(pid_t pid);

/* Menyalakan timer satu kali sepanjang quantum. */
bool runtime_timer_start(unsigned int quantum_ms);

void runtime_timer_stop(void);

/* Menunggu quantum habis, proses anak selesai, atau Ctrl+C. */
runtime_event runtime_wait_event(void);

bool runtime_interrupted(void);
void runtime_clear_child_event(void);

/* Melanjutkan lalu meminta proses anak berhenti lewat SIGTERM. */
bool runtime_request_exit(pid_t pid);

/* Mengembalikan mask signal dan membuang timer quantum. */
void runtime_signals_restore(void);

#endif
