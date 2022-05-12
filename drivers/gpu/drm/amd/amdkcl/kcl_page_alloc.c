// SPDX-License-Identifier: GPL-2.0-only
/*
 *  linux/mm/page_alloc.c
 *
 *  Manages the free list, the system allocates free pages here.
 *  Note that kmalloc() lives in slab.c
 *
 *  Copyright (C) 1991, 1992, 1993, 1994  Linus Torvalds
 *  Swap reorganised 29.12.95, Stephen Tweedie
 *  Support of BIGMEM added by Gerhard Wichert, Siemens AG, July 1999
 *  Reshaped it to be a zoned allocator, Ingo Molnar, Red Hat, 1999
 *  Discontiguous memory support, Kanoj Sarcar, SGI, Nov 1999
 *  Zone balancing, Kanoj Sarcar, SGI, Jan 2000
 *  Per cpu hot/cold page lists, bulk allocation, Martin J. Bligh, Sept 2002
 *          (lots of bits borrowed from Ingo Molnar & Andrew Morton)
 */

#include <linux/stddef.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/swap.h>
#include <linux/interrupt.h>
#include <linux/pagemap.h>
#include <linux/jiffies.h>
#include <linux/memblock.h>
#include <linux/compiler.h>
#include <linux/kernel.h>
//#include <linux/kasan.h>
#include <linux/module.h>
#include <linux/suspend.h>
#include <linux/pagevec.h>
#include <linux/blkdev.h>
#include <linux/slab.h>
#include <linux/ratelimit.h>
#include <linux/oom.h>
#include <linux/topology.h>
#include <linux/sysctl.h>
#include <linux/cpu.h>
#include <linux/cpuset.h>
#include <linux/memory_hotplug.h>
#include <linux/nodemask.h>
#include <linux/vmalloc.h>
#include <linux/vmstat.h>
#include <linux/mempolicy.h>
#include <linux/memremap.h>
#include <linux/stop_machine.h>
#include <linux/random.h>
#include <linux/sort.h>
#include <linux/pfn.h>
#include <linux/backing-dev.h>
#include <linux/fault-inject.h>
#include <linux/page-isolation.h>
#include <linux/debugobjects.h>
#include <linux/kmemleak.h>
#include <linux/compaction.h>
#include <trace/events/kmem.h>
#include <trace/events/oom.h>
#include <linux/prefetch.h>
#include <linux/mm_inline.h>
#include <linux/mmu_notifier.h>
#include <linux/migrate.h>
#include <linux/hugetlb.h>
#include <linux/sched/rt.h>
#include <linux/sched/mm.h>
#include <linux/kthread.h>
#include <linux/memcontrol.h>
#include <linux/ftrace.h>
#include <linux/lockdep.h>
#include <linux/nmi.h>
//#include <linux/psi.h>
#include <linux/padata.h>
#include <linux/khugepaged.h>
#include <linux/buffer_head.h>

#include <asm/sections.h>
#include <asm/tlbflush.h>
#include <asm/div64.h>
//#include "internal.h"
//#include "shuffle.h"
//#include "page_reporting.h"

/* Copied from mm/page_allo.c */
#ifndef HAVE_FS_RECLAIM_ACQUIRE
#ifdef CONFIG_LOCKDEP
static struct lockdep_map __fs_reclaim_map =
	STATIC_LOCKDEP_MAP_INIT("fs_reclaim", &__fs_reclaim_map);

static bool __need_reclaim(gfp_t gfp_mask)
{
	/* no reclaim without waiting on it */
	if (!(gfp_mask & __GFP_DIRECT_RECLAIM))
		return false;

	/* this guy won't enter reclaim */
	if (current->flags & PF_MEMALLOC)
		return false;

	if (gfp_mask & __GFP_NOLOCKDEP)
		return false;

	return true;
}

void __fs_reclaim_acquire(void)
{
	lock_map_acquire(&__fs_reclaim_map);
}

void __fs_reclaim_release(void)
{
	lock_map_release(&__fs_reclaim_map);
}

void _kcl_fs_reclaim_acquire(gfp_t gfp_mask)
{
	gfp_mask = current_gfp_context(gfp_mask);

	if (__need_reclaim(gfp_mask)) {
		if (gfp_mask & __GFP_FS)
			__fs_reclaim_acquire();

#ifdef CONFIG_MMU_NOTIFIER
		lock_map_acquire(&__mmu_notifier_invalidate_range_start_map);
		lock_map_release(&__mmu_notifier_invalidate_range_start_map);
#endif

	}
}
EXPORT_SYMBOL_GPL(_kcl_fs_reclaim_acquire);

void _kcl_fs_reclaim_release(gfp_t gfp_mask)
{
	gfp_mask = current_gfp_context(gfp_mask);

	if (__need_reclaim(gfp_mask)) {
		if (gfp_mask & __GFP_FS)
			__fs_reclaim_release();
	}
}
EXPORT_SYMBOL_GPL(_kcl_fs_reclaim_release);
#endif
#endif /* HAVE_FS_RECLAIM_ACQUIRE */
