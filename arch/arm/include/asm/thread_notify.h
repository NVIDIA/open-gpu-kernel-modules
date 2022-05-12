/* SPDX-License-Identifier: GPL-2.0-only */
/*
 *  arch/arm/include/asm/thread_notify.h
 *
 *  Copyright (C) 2006 Russell King.
 */
#ifndef ASMARM_THREAD_NOTIFY_H
#define ASMARM_THREAD_NOTIFY_H

#ifdef __KERNEL__

#ifndef __ASSEMBLY__

#include <linux/notifier.h>
#include <asm/thread_info.h>

static inline int thread_register_notifier(struct notifier_block *n)
{
	extern struct atomic_notifier_head thread_notify_head;
	return atomic_notifier_chain_register(&thread_notify_head, n);
}

static inline void thread_unregister_notifier(struct notifier_block *n)
{
	extern struct atomic_notifier_head thread_notify_head;
	atomic_notifier_chain_unregister(&thread_notify_head, n);
}

static inline void thread_notify(unsigned long rc, struct thread_info *thread)
{
	extern struct atomic_notifier_head thread_notify_head;
	atomic_notifier_call_chain(&thread_notify_head, rc, thread);
}

#endif

/*
 * These are the reason codes for the thread notifier.
 */
#define THREAD_NOTIFY_FLUSH	0
#define THREAD_NOTIFY_EXIT	1
#define THREAD_NOTIFY_SWITCH	2
#define THREAD_NOTIFY_COPY	3

#endif
#endif
