#include "process_queue.h"

#include <stddef.h>

#define QUEUE_CAPACITY TIMESHARING_MAX_PROCESSES

static unsigned int slot_at(const timesharing_queue *queue,
                            unsigned int offset)
{
    return (queue->head + offset) % QUEUE_CAPACITY;
}

void queue_init(timesharing_queue *queue)
{
    if (queue == NULL) {
        return;
    }
    queue->head = 0U;
    queue->count = 0U;
}

unsigned int queue_size(const timesharing_queue *queue)
{
    return (queue == NULL) ? 0U : queue->count;
}

unsigned int queue_capacity(void)
{
    return QUEUE_CAPACITY;
}

bool queue_is_empty(const timesharing_queue *queue)
{
    return queue_size(queue) == 0U;
}

bool queue_is_full(const timesharing_queue *queue)
{
    return queue_size(queue) >= QUEUE_CAPACITY;
}

bool queue_find(const timesharing_queue *queue, pid_t pid,
                timesharing_process *out)
{
    if (queue == NULL || pid <= 0) {
        return false;
    }
    for (unsigned int i = 0U; i < queue->count; ++i) {
        const timesharing_process *item = &queue->items[slot_at(queue, i)];
        if (item->pid == pid) {
            if (out != NULL) {
                *out = *item;
            }
            return true;
        }
    }
    return false;
}

bool queue_push(timesharing_queue *queue, timesharing_process process)
{
    if (queue == NULL || process.pid <= 0) {
        return false;
    }
    if (queue_is_full(queue) || queue_find(queue, process.pid, NULL)) {
        return false;
    }
    queue->items[slot_at(queue, queue->count)] = process;
    queue->count += 1U;
    return true;
}

bool queue_peek(const timesharing_queue *queue, timesharing_process *out)
{
    if (queue_is_empty(queue)) {
        return false;
    }
    if (out != NULL) {
        *out = queue->items[queue->head];
    }
    return true;
}

bool queue_pop(timesharing_queue *queue, timesharing_process *out)
{
    if (!queue_peek(queue, out)) {
        return false;
    }
    queue->head = (queue->head + 1U) % QUEUE_CAPACITY;
    queue->count -= 1U;
    return true;
}

bool queue_remove(timesharing_queue *queue, pid_t pid)
{
    if (queue == NULL || pid <= 0) {
        return false;
    }

    unsigned int position = 0U;
    bool found = false;
    for (unsigned int i = 0U; i < queue->count; ++i) {
        if (queue->items[slot_at(queue, i)].pid == pid) {
            position = i;
            found = true;
            break;
        }
    }
    if (!found) {
        return false;
    }

    /* Geser entri sesudahnya agar urutan FIFO tetap. */
    for (unsigned int i = position; i + 1U < queue->count; ++i) {
        queue->items[slot_at(queue, i)] = queue->items[slot_at(queue, i + 1U)];
    }
    queue->count -= 1U;
    return true;
}

void queue_clear(timesharing_queue *queue)
{
    queue_init(queue);
}
