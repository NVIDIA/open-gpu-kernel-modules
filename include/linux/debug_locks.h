/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_DEBUG_LOCKING_H
#define __LINUX_DEBUG_LOCKING_H

#include <linux/atomic.h>
#include <linux/bug.h>
#include <linux/printk.h>

struct task_struct;

extern int debug_locks __read_mostly;
extern int debug_locks_silent __read_mostly;


static __always_inline int __debug_locks_off(void)
{
	return xchg(&debug_locks, 0);
}

/*
 * Generic 'turn off all lock debugging' function:
 */
extern int debug_locks_off(void);

#define DEBUG_LOCKS_WARN_ON(c)						\
({									\
	int __ret = 0;							\
									\
	if (!oops_in_progress && unlikely(c)) {				\
		instrumentation_begin();				\
		if (debug_locks_off() && !debug_locks_silent)		\
			WARN(1, "DEBUG_LOCKS_WARN_ON(%s)", #c);		\
		instrumentation_end();					\
		__ret = 1;						\
	}								\
	__ret;								\
})

#ifdef CONFIG_SMP
# define SMP_DEBUG_LOCKS_WARN_ON(c)			DEBUG_LOCKS_WARN_ON(c)
#else
# define SMP_DEBUG_LOCKS_WARN_ON(c)			do { } while (0)
#endif

#ifdef CONFIG_DEBUG_LOCKING_API_SELFTESTS
  extern void locking_selftest(void);
#else
# define locking_selftest()	do { } while (0)
#endif

struct task_struct;

#ifdef CONFIG_LOCKDEP
extern void debug_show_all_locks(void);
extern void debug_show_held_locks(struct task_struct *task);
extern void debug_check_no_locks_freed(const void *from, unsigned long len);
extern void debug_check_no_locks_held(void);
#else
static inline void debug_show_all_locks(void)
{
}

static inline void debug_show_held_locks(struct task_struct *task)
{
}

static inline void
debug_check_no_locks_freed(const void *from, unsigned long len)
{
}

static inline void
debug_check_no_locks_held(void)
{
}
#endif

#endif
