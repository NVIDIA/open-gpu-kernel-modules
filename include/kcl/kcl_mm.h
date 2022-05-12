/* SPDX-License-Identifier: GPL-2.0 */
/*
 * linux/ipc/util.c
 * Copyright (C) 1992 Krishna Balasubramanian
 *   For kvmalloc/kvzalloc
 */
#ifndef AMDKCL_MM_H
#define AMDKCL_MM_H

#include <linux/sched/mm.h>
#include <asm/page.h>
#include <linux/mm_types.h>
#include <linux/mm.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <kcl/kcl_mmap_lock.h>
#include <kcl/kcl_mm_types.h>
#include <kcl/kcl_memory.h>

#ifndef untagged_addr
/* Copied from include/linux/mm.h */
#define untagged_addr(addr) (addr)
#endif

#ifndef HAVE_MM_ACCESS
extern struct mm_struct * (*_kcl_mm_access)(struct task_struct *task, unsigned int mode);
#endif

#ifndef HAVE_MMPUT_ASYNC
extern void (*_kcl_mmput_async)(struct mm_struct *mm);
#endif

#ifndef HAVE_GET_MM_EXE_FILE
extern struct file *(*_kcl_get_mm_exe_file)(struct mm_struct *mm);
#endif

#ifndef HAVE_FAULT_FLAG_ALLOW_RETRY_FIRST
static inline bool fault_flag_allow_retry_first(unsigned int flags)
{
	return (flags & FAULT_FLAG_ALLOW_RETRY) &&
	    (!(flags & FAULT_FLAG_TRIED));
}
#endif

#ifndef HAVE_KVFREE
/* Copied from mm/util.c */
static inline void kvfree(const void *addr)
{
	if (is_vmalloc_addr(addr))
		vfree(addr);
	else
		kfree(addr);
}
#endif

#ifndef HAVE_KVZALLOC_KVMALLOC
/* Copied from v4.11-10655-ga7c3e901a46f ipc/util.c */
static inline void *kvmalloc(size_t size, gfp_t flags)
{
	void *out;

	if (size > PAGE_SIZE)
		out = __vmalloc(size, flags, PAGE_KERNEL);
	else
		out = kmalloc(size, flags);
	return out;
}
static inline void *kvzalloc(size_t size, gfp_t flags)
{
	return kvmalloc(size, flags | __GFP_ZERO);
}
#endif /* HAVE_KVZALLOC_KVMALLOC */

#ifndef HAVE_KVMALLOC_ARRAY
/* Copied from v4.11-10661-g752ade68cbd8 include/linux/mm.h */
static inline void *kvmalloc_array(size_t n, size_t size, gfp_t flags)
{
	if (size != 0 && n > SIZE_MAX / size)
		return NULL;

	return kvmalloc(n * size, flags);
}
#endif /* HAVE_KVMALLOC_ARRAY */

#ifndef HAVE_KVCALLOC
/* Copied from v4.17-10379-g1c542f38ab8d include/linux/mm.h */
static inline void *kvcalloc(size_t n, size_t size, gfp_t flags)
{
	return kvmalloc_array(n, size, flags | __GFP_ZERO);
}
#endif /* HAVE_KVCALLOC */

#if !defined(HAVE_MMGRAB)
/* Copied from v4.10-10392-gf1f1007644ff include/linux/sched.h */
static inline void mmgrab(struct mm_struct *mm)
{
	atomic_inc(&mm->mm_count);
}
#endif

#if !defined(HAVE_MEMALLOC_NOFS_SAVE)
static inline unsigned int memalloc_nofs_save(void)
{
	return current->flags;
}

static inline void memalloc_nofs_restore(unsigned int flags)
{
}
#endif

#if !defined(HAVE_ZONE_MANAGED_PAGES)
/* Copied from v4.20-6505-g9705bea5f833 include/linux/mmzone.h and modified for KCL */
static inline unsigned long zone_managed_pages(struct zone *zone)
{
#if defined(HAVE_STRUCT_ZONE_MANAGED_PAGES)
	return (unsigned long)zone->managed_pages;
#else
	/* zone->managed_pages is introduced in v3.7-4152-g9feedc9d831e */
	WARN_ONCE(1, "struct zone->managed_pages don't exist. kernel is a bit old...");
	return 0;
#endif
}
#endif /* HAVE_ZONE_MANAGED_PAGES */

#ifndef HAVE_IS_COW_MAPPING
static inline bool is_cow_mapping(vm_flags_t flags)
{
        return (flags & (VM_SHARED | VM_MAYWRITE)) == VM_MAYWRITE;
}
#endif /* HAVE_IS_COW_MAPPING */

#ifndef HAVE_MMGET
/* Copied fromr include/linux/sched.h */
static inline void mmget(struct mm_struct *mm)
{
        atomic_inc(&mm->mm_users);
}
#endif /*HAVE_MMGET */
#endif /* AMDKCL_MM_H */
