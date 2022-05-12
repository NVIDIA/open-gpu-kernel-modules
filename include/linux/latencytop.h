/* SPDX-License-Identifier: GPL-2.0 */
/*
 * latencytop.h: Infrastructure for displaying latency
 *
 * (C) Copyright 2008 Intel Corporation
 * Author: Arjan van de Ven <arjan@linux.intel.com>
 *
 */

#ifndef _INCLUDE_GUARD_LATENCYTOP_H_
#define _INCLUDE_GUARD_LATENCYTOP_H_

#include <linux/compiler.h>
struct task_struct;

#ifdef CONFIG_LATENCYTOP

#define LT_SAVECOUNT		32
#define LT_BACKTRACEDEPTH	12

struct latency_record {
	unsigned long	backtrace[LT_BACKTRACEDEPTH];
	unsigned int	count;
	unsigned long	time;
	unsigned long	max;
};



extern int latencytop_enabled;
void __account_scheduler_latency(struct task_struct *task, int usecs, int inter);
static inline void
account_scheduler_latency(struct task_struct *task, int usecs, int inter)
{
	if (unlikely(latencytop_enabled))
		__account_scheduler_latency(task, usecs, inter);
}

void clear_tsk_latency_tracing(struct task_struct *p);

int sysctl_latencytop(struct ctl_table *table, int write, void *buffer,
		size_t *lenp, loff_t *ppos);

#else

static inline void
account_scheduler_latency(struct task_struct *task, int usecs, int inter)
{
}

static inline void clear_tsk_latency_tracing(struct task_struct *p)
{
}

#endif

#endif
