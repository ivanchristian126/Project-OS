#include "runtime_signals.h"

#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

static volatile sig_atomic_t g_quantum_expired = 0;
static volatile sig_atomic_t g_child_event = 0;
static volatile sig_atomic_t g_interrupted = 0;

static timer_t g_timer;
static bool g_timer_ready = false;
static sigset_t g_unblocked;

static void handle_alarm(int signo)
{
    (void)signo;
    g_quantum_expired = 1;
}

static void handle_child(int signo)
{
    (void)signo;
    g_child_event = 1;
}

static void handle_interrupt(int signo)
{
    (void)signo;
    g_interrupted = 1;
}

static bool install_handler(int signo, void (*handler)(int), int flags)
{
    struct sigaction action;

    memset(&action, 0, sizeof action);
    action.sa_handler = handler;
    action.sa_flags = flags;
    sigemptyset(&action.sa_mask);
    if (sigaction(signo, &action, NULL) != 0) {
        fprintf(stderr, "sigaction(%d) gagal: %s\n", signo, strerror(errno));
        return false;
    }
    return true;
}

static bool create_quantum_timer(void)
{
    struct sigevent event;

    memset(&event, 0, sizeof event);
    event.sigev_notify = SIGEV_SIGNAL;
    event.sigev_signo = SIGALRM;
    if (timer_create(CLOCK_MONOTONIC, &event, &g_timer) != 0) {
        fprintf(stderr, "timer_create gagal: %s\n", strerror(errno));
        return false;
    }
    g_timer_ready = true;
    return true;
}

bool runtime_signals_install(void)
{
    sigset_t blocked;

    /* SA_NOCLDSTOP supaya SIGSTOP dan SIGCONT tidak terbaca sebagai selesai. */
    if (!install_handler(SIGALRM, handle_alarm, SA_RESTART) ||
        !install_handler(SIGCHLD, handle_child, SA_RESTART | SA_NOCLDSTOP) ||
        !install_handler(SIGINT, handle_interrupt, SA_RESTART) ||
        !install_handler(SIGTERM, handle_interrupt, SA_RESTART)) {
        return false;
    }

    sigemptyset(&blocked);
    sigaddset(&blocked, SIGALRM);
    sigaddset(&blocked, SIGCHLD);
    sigaddset(&blocked, SIGINT);
    sigaddset(&blocked, SIGTERM);

    /* Signal diblokir agar tidak hilang sebelum program siap menunggu. */
    if (sigprocmask(SIG_BLOCK, &blocked, &g_unblocked) != 0) {
        fprintf(stderr, "sigprocmask gagal: %s\n", strerror(errno));
        return false;
    }
    return create_quantum_timer();
}

bool runtime_stop(pid_t pid)
{
    return pid > 0 && kill(pid, SIGSTOP) == 0;
}

bool runtime_resume(pid_t pid)
{
    return pid > 0 && kill(pid, SIGCONT) == 0;
}

static bool arm_timer(unsigned int quantum_ms)
{
    struct itimerspec spec;

    memset(&spec, 0, sizeof spec);
    spec.it_value.tv_sec = (time_t)(quantum_ms / 1000U);
    spec.it_value.tv_nsec = (long)(quantum_ms % 1000U) * 1000000L;
    return timer_settime(g_timer, 0, &spec, NULL) == 0;
}

bool runtime_timer_start(unsigned int quantum_ms)
{
    if (!g_timer_ready || quantum_ms == 0U) {
        return false;
    }
    g_quantum_expired = 0;
    if (!arm_timer(quantum_ms)) {
        fprintf(stderr, "timer_settime gagal: %s\n", strerror(errno));
        return false;
    }
    return true;
}

void runtime_timer_stop(void)
{
    if (g_timer_ready) {
        (void)arm_timer(0U);
    }
}

runtime_event runtime_wait_event(void)
{
    while (g_interrupted == 0 && g_child_event == 0 && g_quantum_expired == 0) {
        (void)sigsuspend(&g_unblocked);
    }
    if (g_interrupted != 0) {
        return RUNTIME_EVENT_INTERRUPT;
    }
    if (g_child_event != 0) {
        return RUNTIME_EVENT_CHILD;
    }
    return RUNTIME_EVENT_QUANTUM;
}

bool runtime_interrupted(void)
{
    return g_interrupted != 0;
}

void runtime_clear_child_event(void)
{
    g_child_event = 0;
}

bool runtime_request_exit(pid_t pid)
{
    if (pid <= 0) {
        return false;
    }
    /* SIGCONT dulu, proses yang sedang berhenti tidak memproses SIGTERM. */
    (void)kill(pid, SIGCONT);
    return kill(pid, SIGTERM) == 0;
}

void runtime_signals_restore(void)
{
    if (g_timer_ready) {
        (void)timer_delete(g_timer);
        g_timer_ready = false;
    }
    (void)sigprocmask(SIG_SETMASK, &g_unblocked, NULL);
}
