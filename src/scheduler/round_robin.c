#include "scheduler.h"

#include <stddef.h>

void rr_init(round_robin *rr, unsigned int quantum_ms)
{
    if (rr == NULL) {
        return;
    }
    queue_init(&rr->ready);
    rr->quantum_ms = (quantum_ms == 0U)
        ? TIMESHARING_DEFAULT_QUANTUM_MS
        : quantum_ms;
    rr->switch_count = 0U;
    rr->completed = 0U;
}

bool rr_admit(round_robin *rr, pid_t pid, unsigned int remaining_ms)
{
    if (rr == NULL) {
        return false;
    }

    const timesharing_process process = {
        .pid = pid,
        .remaining_ms = remaining_ms
    };

    return queue_push(&rr->ready, process);
}

bool rr_next(round_robin *rr, timesharing_process *out)
{
    if (rr == NULL || !queue_pop(&rr->ready, out)) {
        return false;
    }
    rr->switch_count += 1U;
    return true;
}

bool rr_requeue(round_robin *rr, timesharing_process process)
{
    if (rr == NULL) {
        return false;
    }
    return queue_push(&rr->ready, process);
}

unsigned int rr_slice_ms(const round_robin *rr,
                         const timesharing_process *process)
{
    if (rr == NULL) {
        return 0U;
    }
    if (process != NULL && process->remaining_ms > 0U &&
        process->remaining_ms < rr->quantum_ms) {
        return process->remaining_ms;
    }
    return rr->quantum_ms;
}

void rr_charge(timesharing_process *process, unsigned int used_ms)
{
    if (process == NULL) {
        return;
    }
    process->remaining_ms = (used_ms >= process->remaining_ms)
        ? 0U
        : process->remaining_ms - used_ms;
}

bool rr_complete(round_robin *rr, pid_t pid)
{
    if (rr == NULL || pid <= 0) {
        return false;
    }
    /* Proses yang sedang berjalan sudah keluar dari antrean. */
    (void)queue_remove(&rr->ready, pid);
    rr->completed += 1U;
    return true;
}

unsigned int rr_pending(const round_robin *rr)
{
    return (rr == NULL) ? 0U : queue_size(&rr->ready);
}

bool rr_has_work(const round_robin *rr)
{
    return rr_pending(rr) > 0U;
}
