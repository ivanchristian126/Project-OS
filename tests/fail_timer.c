/* Hanya ditautkan ke executable pengujian dengan GNU ld --wrap. */
#include "runtime_signals.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

bool __wrap_runtime_timer_start(unsigned int quantum_ms)
{
    (void)quantum_ms;
    return false;
}

void __real_runtime_signals_restore(void);

void __wrap_runtime_signals_restore(void)
{
    errno = 0;
    if (waitpid(-1, NULL, WNOHANG) != -1 || errno != ECHILD) {
        fputs("FAIL masih ada proses anak setelah kegagalan timer\n", stderr);
        exit(99);
    }
    puts("Cleanup setelah kegagalan timer lulus");
    __real_runtime_signals_restore();
}
