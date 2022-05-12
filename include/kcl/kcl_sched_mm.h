/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _KCL_KCL_SCHED_MM_H
#define _KCL_KCL_SCHED_MM_H

#include <linux/kernel.h>
#include <linux/atomic.h>
#include <linux/sched.h>
#include <linux/mm_types.h>
#include <linux/gfp.h>

#ifndef SHRINK_EMPTY
#define SHRINK_EMPTY (~0UL - 1)
#define SHRINK_STOP (~0UL)
#endif

#ifndef HAVE_FS_RECLAIM_ACQUIRE
#ifdef CONFIG_LOCKDEP
extern void __fs_reclaim_acquire(void);
extern void __fs_reclaim_release(void);
static inline void fs_reclaim_acquire(gfp_t gfp_mask) {
	return _kcl_fs_reclaim_acquire(gfp_mask);
}
static inline void fs_reclaim_release(gfp_t gfp_mask) {
	return  _kcl_fs_reclaim_release(gfp_mask);
}
#else
static inline void __fs_reclaim_acquire(void) { }
static inline void __fs_reclaim_release(void) { }
static inline void fs_reclaim_acquire(gfp_t gfp_mask) { }
static inline void fs_reclaim_release(gfp_t gfp_mask) { }
#endif /* CONFIG_LOCKDEP */
#endif /* HAVE_FS_RECLAIM_ACQUIRE */

#ifndef HAVE_MEMALLOC_NORECLAIM_SAVE
static inline unsigned int memalloc_noreclaim_save(void)
{
	unsigned int flags = current->flags & PF_MEMALLOC;
	current->flags |= PF_MEMALLOC;
	return flags;
}

static inline void memalloc_noreclaim_restore(unsigned int flags)
{
	current->flags = (current->flags & ~PF_MEMALLOC) | flags;
}
#endif /* HAVE_MEMALLOC_NORECLAIM_SAVE */

#endif
