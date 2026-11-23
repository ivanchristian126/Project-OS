#include "worker.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

/* Linux menyediakan status proses, termasuk T untuk stopped, lewat /proc. */
static bool is_stopped(pid_t pid)
{
    char path[64];
    char line[256];
    snprintf(path, sizeof path, "/proc/%ld/status", (long)pid);
    FILE *status = fopen(path, "r");
    if (status == NULL) {
        return false;
    }
    bool stopped = false;
    while (fgets(line, sizeof line, status) != NULL) {
        if (strncmp(line, "State:", 6U) == 0) {
            char state = '\0';
            stopped = sscanf(line, "State: %c", &state) == 1 && state == 'T';
            break;
        }
    }
    fclose(status);
    return stopped;
}

static bool was_reaped(pid_t pid)
{
    errno = 0;
    return waitpid(pid, NULL, WNOHANG) == -1 && errno == ECHILD;
}

int main(void)
{
    worker_handle worker;
    if (!worker_spawn(&worker, 1U, 50U)) {
        return 1;
    }
    if (!is_stopped(worker.pid) || worker_collect(&worker)) {
        fprintf(stderr, "FAIL worker harus berhenti sebelum giliran pertama\n");
        (void)worker_terminate(&worker, 0U);
        return 1;
    }
    if (kill(worker.pid, SIGCONT) != 0) {
        (void)worker_terminate(&worker, 0U);
        return 1;
    }
    worker_join(&worker);
    if (!worker.finished || worker.failed || !was_reaped(worker.pid)) {
        fprintf(stderr, "FAIL worker harus selesai dan dipanen setelah SIGCONT\n");
        return 1;
    }
    if (!worker_spawn(&worker, 2U, 10000U)) {
        return 1;
    }
    if (!worker_terminate(&worker, 200U) || !was_reaped(worker.pid)) {
        fprintf(stderr, "FAIL cleanup worker yang sedang berhenti\n");
        return 1;
    }
    puts("Pemeriksaan worker lulus");
    return 0;
}
