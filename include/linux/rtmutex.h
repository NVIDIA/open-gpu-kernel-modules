/* SPDX-License-Identifier: GPL-2.0 */
/*
 * RT Mutexes: blocking mutual exclusion locks with PI support
 *
 * started by Ingo Molnar and Thomas Gleixner:
 *
 *  Copyright (C) 2004-2006 Red Hat, Inc., Ingo Molnar <mingo@redhat.com>
 *  Copyright (C) 2006, Timesys Corp., Thomas Gleixner <tglx@timesys.com>
 *
 * This file contains the public data structure and API definitions.
 */

#ifndef __LINUX_RT_MUTEX_H
#define __LINUX_RT_MUTEX_H

#include <linux/linkage.h>
#include <linux/rbtree.h>
#include <linux/spinlock_types.h>

extern int max_lock_depth; /* for sysctl */

/**
 * The rt_mutex structure
 *
 * @wait_lock:	spinlock to protect the structure
 * @waiters:	rbtree root to enqueue waiters in priority order;
 *              caches top-waiter (leftmost node).
 * @owner:	the mutex owner
 */
struct rt_mutex {
	raw_spinlock_t		wait_lock;
	struct rb_root_cached   waiters;
	struct task_struct	*owner;
#ifdef CONFIG_DEBUG_LOCK_ALLOC
	struct lockdep_map	dep_map;
#endif
};

struct rt_mutex_waiter;
struct hrtimer_sleeper;

#ifdef CONFIG_DEBUG_RT_MUTEXES
extern void rt_mutex_debug_task_free(struct task_struct *tsk);
#else
static inline void rt_mutex_debug_task_free(struct task_struct *tsk) { }
#endif

#define rt_mutex_init(mutex) \
do { \
	static struct lock_class_key __key; \
	__rt_mutex_init(mutex, __func__, &__key); \
} while (0)

#ifdef CONFIG_DEBUG_LOCK_ALLOC
#define __DEP_MAP_RT_MUTEX_INITIALIZER(mutexname) \
	, .dep_map = { .name = #mutexname }
#else
#define __DEP_MAP_RT_MUTEX_INITIALIZER(mutexname)
#endif

#define __RT_MUTEX_INITIALIZER(mutexname) \
	{ .wait_lock = __RAW_SPIN_LOCK_UNLOCKED(mutexname.wait_lock) \
	, .waiters = RB_ROOT_CACHED \
	, .owner = NULL \
	__DEP_MAP_RT_MUTEX_INITIALIZER(mutexname)}

#define DEFINE_RT_MUTEX(mutexname) \
	struct rt_mutex mutexname = __RT_MUTEX_INITIALIZER(mutexname)

/**
 * rt_mutex_is_locked - is the mutex locked
 * @lock: the mutex to be queried
 *
 * Returns 1 if the mutex is locked, 0 if unlocked.
 */
static inline int rt_mutex_is_locked(struct rt_mutex *lock)
{
	return lock->owner != NULL;
}

extern void __rt_mutex_init(struct rt_mutex *lock, const char *name, struct lock_class_key *key);

#ifdef CONFIG_DEBUG_LOCK_ALLOC
extern void rt_mutex_lock_nested(struct rt_mutex *lock, unsigned int subclass);
#define rt_mutex_lock(lock) rt_mutex_lock_nested(lock, 0)
#else
extern void rt_mutex_lock(struct rt_mutex *lock);
#define rt_mutex_lock_nested(lock, subclass) rt_mutex_lock(lock)
#endif

extern int rt_mutex_lock_interruptible(struct rt_mutex *lock);
extern int rt_mutex_trylock(struct rt_mutex *lock);

extern void rt_mutex_unlock(struct rt_mutex *lock);

#endif
