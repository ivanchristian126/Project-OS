// SPDX-License-Identifier: GPL-2.0
/*
 * Syscall ts_mark: mencatat pergantian giliran dari aplikasi time sharing.
 * File ini dipasang ke kernel/ts_mark.c pada source kernel.
 */

#include <linux/atomic.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/syscalls.h>

#define TS_MARK_MAX_SLICE_MS 10000U

static atomic_long_t ts_mark_total = ATOMIC_LONG_INIT(0);

SYSCALL_DEFINE2(ts_mark, pid_t, pid, unsigned int, slice_ms)
{
	long total;

	if (pid <= 0 || slice_ms == 0 || slice_ms > TS_MARK_MAX_SLICE_MS)
		return -EINVAL;

	total = atomic_long_inc_return(&ts_mark_total);
	pr_info_ratelimited("ts_mark: caller=%d target=%d slice=%u ms total=%ld\n",
			    current->pid, pid, slice_ms, total);

	return total;
}
