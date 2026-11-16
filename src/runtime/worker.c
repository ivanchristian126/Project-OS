#include "worker.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define PROGRESS_STEPS 5U
#define WORK_CHUNK 20000U

/* Waktu CPU proses ini sendiri; tidak bertambah saat proses di-SIGSTOP. */
static double cpu_time_ms(void)
{
    struct timespec ts;

    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) != 0) {
        return 0.0;
    }
    return (double)ts.tv_sec * 1000.0 + (double)ts.tv_nsec / 1000000.0;
}

/* Beban sintetis yang dibatasi oleh kuota waktu CPU. */
static void run_workload(unsigned int index, unsigned int workload_ms)
{
    const double target = (double)workload_ms;
    const unsigned int span = 100U / PROGRESS_STEPS;
    unsigned int reported = 0U;
    volatile double sink = 0.0;

    for (;;) {
        for (unsigned int i = 0U; i < WORK_CHUNK; ++i) {
            sink += (double)i * 0.5;
        }

        const double used = cpu_time_ms();
        const unsigned int percent = (used >= target)
            ? 100U
            : (unsigned int)((used / target) * 100.0);
        const unsigned int step = percent / span;

        if (step > reported) {
            reported = step;
            printf("    [worker %u] pid %ld progres %u%%\n",
                   index, (long)getpid(), step * span);
            fflush(stdout);
        }
        if (used >= target) {
            break;
        }
    }
}

static void reset_inherited_handlers(void)
{
    static const int signals[] = { SIGINT, SIGTERM, SIGALRM, SIGCHLD };
    struct sigaction action;
    sigset_t empty;

    memset(&action, 0, sizeof action);
    action.sa_handler = SIG_DFL;
    sigemptyset(&action.sa_mask);
    for (size_t i = 0U; i < sizeof signals / sizeof signals[0]; ++i) {
        sigaction(signals[i], &action, NULL);
    }
    sigemptyset(&empty);
    sigprocmask(SIG_SETMASK, &empty, NULL);
}

static void child_main(unsigned int index, unsigned int workload_ms)
{
    reset_inherited_handlers();
    printf("    [worker %u] pid %ld mulai, beban %u ms\n",
           index, (long)getpid(), workload_ms);
    fflush(stdout);
    run_workload(index, workload_ms);
    printf("    [worker %u] pid %ld beban selesai\n", index, (long)getpid());
    fflush(stdout);
    _exit(0);
}

bool worker_spawn(worker_handle *worker, unsigned int index,
                  unsigned int workload_ms)
{
    if (worker == NULL || workload_ms == 0U) {
        return false;
    }

    worker->pid = -1;
    worker->index = index;
    worker->workload_ms = workload_ms;
    worker->finished = false;
    worker->failed = false;
    worker->exit_status = 0;

    fflush(stdout);
    fflush(stderr);

    const pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork() gagal untuk proses %u: %s\n",
                index, strerror(errno));
        return false;
    }
    if (pid == 0) {
        child_main(index, workload_ms);
    }

    worker->pid = pid;
    return true;
}

static void record_status(worker_handle *worker, int status)
{
    if (WIFEXITED(status)) {
        worker->finished = true;
        worker->exit_status = WEXITSTATUS(status);
        worker->failed = (worker->exit_status != 0);
    } else if (WIFSIGNALED(status)) {
        worker->finished = true;
        worker->exit_status = -WTERMSIG(status);
        worker->failed = true;
    }
}

bool worker_collect(worker_handle *worker)
{
    if (worker == NULL) {
        return false;
    }
    if (worker->finished || worker->pid <= 0) {
        return worker->finished;
    }

    int status = 0;
    const pid_t result = waitpid(worker->pid, &status, WNOHANG);

    if (result == 0) {
        return false;
    }
    if (result < 0) {
        if (errno == ECHILD) {
            worker->finished = true;
            worker->failed = true;
        }
        return worker->finished;
    }
    record_status(worker, status);
    return worker->finished;
}

void worker_join(worker_handle *worker)
{
    if (worker == NULL || worker->pid <= 0 || worker->finished) {
        return;
    }

    for (;;) {
        int status = 0;
        const pid_t result = waitpid(worker->pid, &status, 0);

        if (result < 0) {
            if (errno == EINTR) {
                continue;
            }
            worker->finished = true;
            worker->failed = true;
            return;
        }
        record_status(worker, status);
        if (worker->finished) {
            return;
        }
    }
}

const char *worker_state_text(const worker_handle *worker)
{
    if (worker == NULL) {
        return "tidak ada";
    }
    if (!worker->finished) {
        return "berjalan";
    }
    return worker->failed ? "gagal" : "selesai";
}
