/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#ifndef __NV_MM_H__
#define __NV_MM_H__

#include "conftest.h"

#if !defined(NV_VM_FAULT_T_IS_PRESENT)
typedef int vm_fault_t;
#endif

/*
 * pin_user_pages()
 *
 * Presence of pin_user_pages() also implies the presence of unpin-user_page().
 * Both were added in the v5.6.
 *
 * pin_user_pages() was added by commit eddb1c228f79
 * ("mm/gup: introduce pin_user_pages*() and FOLL_PIN") in v5.6.
 *
 * Removed vmas parameter from pin_user_pages() by commit 4c630f307455
 * ("mm/gup: remove vmas parameter from pin_user_pages()") in v6.5.
 */

#include <linux/mm.h>
#include <linux/sched.h>

/*
 * FreeBSD's pin_user_pages's conftest breaks since pin_user_pages is an inline
 * function. Because it simply maps to get_user_pages, we can just replace
 * NV_PIN_USER_PAGES with NV_GET_USER_PAGES on FreeBSD
 */
#if defined(NV_PIN_USER_PAGES_PRESENT) && !defined(NV_BSD)
    #if defined(NV_PIN_USER_PAGES_HAS_ARGS_VMAS)
        #define NV_PIN_USER_PAGES(start, nr_pages, gup_flags, pages) \
            pin_user_pages(start, nr_pages, gup_flags, pages, NULL)
    #else
        #define NV_PIN_USER_PAGES pin_user_pages
    #endif // NV_PIN_USER_PAGES_HAS_ARGS_VMAS
    #define NV_UNPIN_USER_PAGE unpin_user_page
#else
    #define NV_PIN_USER_PAGES NV_GET_USER_PAGES
    #define NV_UNPIN_USER_PAGE put_page
#endif // NV_PIN_USER_PAGES_PRESENT

/*
 * get_user_pages()
 *
 * Removed vmas parameter from get_user_pages() by commit 54d020692b34
 * ("mm/gup: remove unused vmas parameter from get_user_pages()") in v6.5.
 *
 */

#if !defined(NV_GET_USER_PAGES_HAS_VMAS_ARG)
    #define NV_GET_USER_PAGES get_user_pages
#else
    #define NV_GET_USER_PAGES(start, nr_pages, flags, pages) \
        get_user_pages(start, nr_pages, flags, pages, NULL)
#endif

/*
 * pin_user_pages_remote()
 *
 * pin_user_pages_remote() was added by commit eddb1c228f79
 * ("mm/gup: introduce pin_user_pages*() and FOLL_PIN") in v5.6.
 *
 * pin_user_pages_remote() removed 'tsk' parameter by commit
 * 64019a2e467a ("mm/gup: remove task_struct pointer for all gup code")
 * in v5.9.
 *
 * Removed unused vmas parameter from pin_user_pages_remote() by commit
 * 0b295316b3a9 ("mm/gup: remove unused vmas parameter from
 * pin_user_pages_remote()") in v6.5.
 *
 */

#if defined(NV_PIN_USER_PAGES_REMOTE_PRESENT)
    #if defined(NV_PIN_USER_PAGES_REMOTE_HAS_ARGS_TSK_VMAS)
        #define NV_PIN_USER_PAGES_REMOTE(mm, start, nr_pages, flags, pages, locked) \
            pin_user_pages_remote(NULL, mm, start, nr_pages, flags, pages, NULL, locked)
    #elif defined(NV_PIN_USER_PAGES_REMOTE_HAS_ARGS_VMAS)
        #define NV_PIN_USER_PAGES_REMOTE(mm, start, nr_pages, flags, pages, locked) \
            pin_user_pages_remote(mm, start, nr_pages, flags, pages, NULL, locked)
    #else
        #define NV_PIN_USER_PAGES_REMOTE pin_user_pages_remote
    #endif // NV_PIN_USER_PAGES_REMOTE_HAS_ARGS_TSK_VMAS
#else
    #define NV_PIN_USER_PAGES_REMOTE NV_GET_USER_PAGES_REMOTE
#endif // NV_PIN_USER_PAGES_REMOTE_PRESENT

/*
 * Note that get_user_pages_remote() requires the caller to hold a reference on
 * the mm_struct.
 * This will always be true when using current and current->mm. If the kernel passes
 * the driver a vma via driver callback, the kernel holds a reference on vma->vm_mm
 * over that callback.
 *
 * get_user_pages_remote() removed 'tsk' parameter by
 * commit 64019a2e467a ("mm/gup: remove task_struct pointer for
 * all gup code") in v5.9.
 *
 * Removed vmas parameter from get_user_pages_remote() by commit ca5e863233e8
 * ("mm/gup: remove vmas parameter from get_user_pages_remote()") in v6.5.
 *
 */

#if defined(NV_GET_USER_PAGES_REMOTE_HAS_ARGS_TSK_VMAS)
    #define NV_GET_USER_PAGES_REMOTE(mm, start, nr_pages, flags, pages, locked) \
        get_user_pages_remote(NULL, mm, start, nr_pages, flags, pages, NULL, locked)
#elif defined(NV_GET_USER_PAGES_REMOTE_HAS_ARGS_TSK)
    #define NV_GET_USER_PAGES_REMOTE(mm, start, nr_pages, flags, pages, locked) \
        get_user_pages_remote(mm, start, nr_pages, flags, pages, NULL, locked)

#else
    #define NV_GET_USER_PAGES_REMOTE get_user_pages_remote
#endif

static inline void nv_mmap_read_lock(struct mm_struct *mm)
{
#if defined(NV_MM_HAS_MMAP_LOCK)
    mmap_read_lock(mm);
#else
    down_read(&mm->mmap_sem);
#endif
}

static inline void nv_mmap_read_unlock(struct mm_struct *mm)
{
#if defined(NV_MM_HAS_MMAP_LOCK)
    mmap_read_unlock(mm);
#else
    up_read(&mm->mmap_sem);
#endif
}

static inline void nv_mmap_write_lock(struct mm_struct *mm)
{
#if defined(NV_MM_HAS_MMAP_LOCK)
    mmap_write_lock(mm);
#else
    down_write(&mm->mmap_sem);
#endif
}

static inline void nv_mmap_write_unlock(struct mm_struct *mm)
{
#if defined(NV_MM_HAS_MMAP_LOCK)
    mmap_write_unlock(mm);
#else
    up_write(&mm->mmap_sem);
#endif
}

static inline int nv_mm_rwsem_is_locked(struct mm_struct *mm)
{
#if defined(NV_MM_HAS_MMAP_LOCK)
    return rwsem_is_locked(&mm->mmap_lock);
#else
    return rwsem_is_locked(&mm->mmap_sem);
#endif
}

static inline struct rw_semaphore *nv_mmap_get_lock(struct mm_struct *mm)
{
#if defined(NV_MM_HAS_MMAP_LOCK)
    return &mm->mmap_lock;
#else
    return &mm->mmap_sem;
#endif
}

#define NV_CAN_CALL_VMA_START_WRITE 1

#if !NV_CAN_CALL_VMA_START_WRITE
/*
 * Commit 45ad9f5290dc updated vma_start_write() to call __vma_start_write().
 */
void nv_vma_start_write(struct vm_area_struct *);

static inline void nv_vma_flags_set_word(struct vm_area_struct *vma, unsigned long flags)
{
    nv_vma_start_write(vma);
#if defined(NV_VMA_FLAGS_SET_WORD_PRESENT)
    vma_flags_set_word(&vma->flags, flags);
#else
    ACCESS_PRIVATE(vma, __vm_flags) |= flags;
#endif
}

static inline void nv_vma_flags_clear_word(struct vm_area_struct *vma, unsigned long flags)
{
    nv_vma_start_write(vma);
#if defined(NV_VMA_FLAGS_SET_WORD_PRESENT)
    vma_flags_clear_word(&vma->flags, flags);
#else
    ACCESS_PRIVATE(vma, __vm_flags) &= ~flags;
#endif
}
#endif // !NV_CAN_CALL_VMA_START_WRITE

static inline void nv_vm_flags_set(struct vm_area_struct *vma, vm_flags_t flags)
{
#if !NV_CAN_CALL_VMA_START_WRITE
    nv_vma_flags_set_word(vma, flags);
#elif defined(NV_VM_FLAGS_SET_PRESENT)
    vm_flags_set(vma, flags);
#else
    vma->vm_flags |= flags;
#endif
}

static inline void nv_vm_flags_clear(struct vm_area_struct *vma, vm_flags_t flags)
{
#if !NV_CAN_CALL_VMA_START_WRITE
    nv_vma_flags_clear_word(vma, flags);
#elif defined(NV_VM_FLAGS_SET_PRESENT)
    vm_flags_clear(vma, flags);
#else
    vma->vm_flags &= ~flags;
#endif
}

#endif // __NV_MM_H__
