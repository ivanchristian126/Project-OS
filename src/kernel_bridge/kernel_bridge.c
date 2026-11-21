#define _DEFAULT_SOURCE

#include "kernel_bridge.h"
#include "ts_syscall.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

typedef enum {
    BRIDGE_UNKNOWN,
    BRIDGE_READY,
    BRIDGE_MISSING,
    BRIDGE_ERROR
} bridge_state;

static bridge_state g_state = BRIDGE_UNKNOWN;
static unsigned long g_marks;
static char g_status[160];

static long call_ts_mark(pid_t pid, unsigned int slice_ms)
{
    return syscall((long)TS_MARK_SYSCALL_NR, (long)pid, (long)slice_ms);
}

bool kernel_bridge_probe(void)
{
    if (g_state != BRIDGE_UNKNOWN) {
        return g_state == BRIDGE_READY;
    }

    errno = 0;
    const long result = call_ts_mark(getpid(), 1U);

    /* ts_mark selalu mengembalikan penghitung yang lebih besar dari nol. */
    if (result > 0) {
        g_state = BRIDGE_READY;
        snprintf(g_status, sizeof g_status,
                 "syscall ts_mark nomor %d tersedia", TS_MARK_SYSCALL_NR);
        return true;
    }
    if (result < 0 && errno == ENOSYS) {
        g_state = BRIDGE_MISSING;
        snprintf(g_status, sizeof g_status,
                 "syscall ts_mark nomor %d tidak ada pada kernel ini",
                 TS_MARK_SYSCALL_NR);
        return false;
    }
    if (result == 0) {
        g_state = BRIDGE_ERROR;
        snprintf(g_status, sizeof g_status,
                 "nomor syscall %d dipakai fitur lain, pencatatan dimatikan",
                 TS_MARK_SYSCALL_NR);
        return false;
    }

    g_state = BRIDGE_ERROR;
    snprintf(g_status, sizeof g_status,
             "syscall ts_mark nomor %d gagal dipanggil: %s",
             TS_MARK_SYSCALL_NR, strerror(errno));
    return false;
}

bool kernel_bridge_mark(pid_t pid, unsigned int slice_ms)
{
    if (g_state == BRIDGE_UNKNOWN) {
        (void)kernel_bridge_probe();
    }
    if (g_state != BRIDGE_READY) {
        return false;
    }

    errno = 0;
    if (call_ts_mark(pid, slice_ms) < 0) {
        g_state = BRIDGE_ERROR;
        snprintf(g_status, sizeof g_status,
                 "syscall ts_mark berhenti bekerja: %s", strerror(errno));
        return false;
    }
    g_marks += 1UL;
    return true;
}

const char *kernel_bridge_status(void)
{
    return (g_status[0] == '\0')
        ? "status syscall ts_mark belum diperiksa"
        : g_status;
}

unsigned long kernel_bridge_marks(void)
{
    return g_marks;
}
