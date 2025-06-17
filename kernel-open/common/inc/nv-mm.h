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
 * The 8-argument version of get_user_pages() was deprecated by commit
 * cde70140fed8 ("mm/gup: Overload get_user_pages() functions") in v4.6-rc1.
 * (calling get_user_pages with current and current->mm).
 *
 * Completely moved to the 6 argument version of get_user_pages() by
 * commit c12d2da56d0e ("mm/gup: Remove the macro overload API migration
 * helpers from the get_user*() APIs") in v4.6-rc4.
 *
 * write and force parameters were replaced with gup_flags by
 * commit 768ae309a961 ("mm: replace get_user_pages() write/force parameters
 * with gup_flags") in v4.9.
 *
 * A 7-argument version of get_user_pages was introduced into linux-4.4.y by
 * commit 8e50b8b07f462 ("mm: replace get_user_pages() write/force parameters
 * with gup_flags") which cherry-picked the replacement of the write and
 * force parameters with gup_flags.
 *
 * Removed vmas parameter from get_user_pages() by commit 54d020692b34
 * ("mm/gup: remove unused vmas parameter from get_user_pages()") in v6.5.
 *
 */

#if defined(NV_GET_USER_PAGES_HAS_ARGS_FLAGS)
    #define NV_GET_USER_PAGES get_user_pages
#elif defined(NV_GET_USER_PAGES_HAS_ARGS_FLAGS_VMAS)
    #define NV_GET_USER_PAGES(start, nr_pages, flags, pages) \
        get_user_pages(start, nr_pages, flags, pages, NULL)
#elif defined(NV_GET_USER_PAGES_HAS_ARGS_TSK_FLAGS_VMAS)
    #define NV_GET_USER_PAGES(start, nr_pages, flags, pages) \
        get_user_pages(current, current->mm, start, nr_pages, flags, pages, NULL)
#else
    static inline long NV_GET_USER_PAGES(unsigned long start,
                                         unsigned long nr_pages,
                                         unsigned int flags,
                                         struct page **pages)
    {
        int write = flags & FOLL_WRITE;
        int force = flags & FOLL_FORCE;

    #if defined(NV_GET_USER_PAGES_HAS_ARGS_WRITE_FORCE_VMAS)
        return get_user_pages(start, nr_pages, write, force, pages, NULL);
    #else
        // NV_GET_USER_PAGES_HAS_ARGS_TSK_WRITE_FORCE_VMAS
        return get_user_pages(current, current->mm, start, nr_pages, write,
                              force, pages, NULL);
    #endif // NV_GET_USER_PAGES_HAS_ARGS_WRITE_FORCE_VMAS
    }
#endif // NV_GET_USER_PAGES_HAS_ARGS_FLAGS

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
 * get_user_pages_remote() was added by commit 1e9877902dc7
 * ("mm/gup: Introduce get_user_pages_remote()") in v4.6.
 *
 * Note that get_user_pages_remote() requires the caller to hold a reference on
 * the task_struct (if non-NULL and if this API has tsk argument) and the mm_struct.
 * This will always be true when using current and current->mm. If the kernel passes
 * the driver a vma via driver callback, the kernel holds a reference on vma->vm_mm
 * over that callback.
 *
 * get_user_pages_remote() write/force parameters were replaced
 * with gup_flags by commit 9beae1ea8930 ("mm: replace get_user_pages_remote()
 * write/force parameters with gup_flags") in v4.9.
 *
 * get_user_pages_remote() added 'locked' parameter by commit 5b56d49fc31d
 * ("mm: add locked parameter to get_user_pages_remote()") in v4.10.
 *
 * get_user_pages_remote() removed 'tsk' parameter by
 * commit 64019a2e467a ("mm/gup: remove task_struct pointer for
 * all gup code") in v5.9.
 *
 * Removed vmas parameter from get_user_pages_remote() by commit ca5e863233e8
 * ("mm/gup: remove vmas parameter from get_user_pages_remote()") in v6.5.
 *
 */

#if defined(NV_GET_USER_PAGES_REMOTE_PRESENT)
    #if defined(NV_GET_USER_PAGES_REMOTE_HAS_ARGS_FLAGS_LOCKED)
        #define NV_GET_USER_PAGES_REMOTE get_user_pages_remote

    #elif defined(NV_GET_USER_PAGES_REMOTE_HAS_ARGS_FLAGS_LOCKED_VMAS)
        #define NV_GET_USER_PAGES_REMOTE(mm, start, nr_pages, flags, pages, locked) \
            get_user_pages_remote(mm, start, nr_pages, flags, pages, NULL, locked)

    #elif defined(NV_GET_USER_PAGES_REMOTE_HAS_ARGS_TSK_FLAGS_LOCKED_VMAS)
        #define NV_GET_USER_PAGES_REMOTE(mm, start, nr_pages, flags, pages, locked) \
            get_user_pages_remote(NULL, mm, start, nr_pages, flags, pages, NULL, locked)

    #elif defined(NV_GET_USER_PAGES_REMOTE_HAS_ARGS_TSK_FLAGS_VMAS)
        #define NV_GET_USER_PAGES_REMOTE(mm, start, nr_pages, flags, pages, locked) \
            get_user_pages_remote(NULL, mm, start, nr_pages, flags, pages, NULL)

    #else
        // NV_GET_USER_PAGES_REMOTE_HAS_ARGS_TSK_WRITE_FORCE_VMAS
        static inline long NV_GET_USER_PAGES_REMOTE(struct mm_struct *mm,
                                                    unsigned long start,
                                                    unsigned long nr_pages,
                                                    unsigned int flags,
                                                    struct page **pages,
                                                    int *locked)
        {
            int write = flags & FOLL_WRITE;
            int force = flags & FOLL_FORCE;

            return get_user_pages_remote(NULL, mm, start, nr_pages, write, force,
                                         pages, NULL);
        }
    #endif // NV_GET_USER_PAGES_REMOTE_HAS_ARGS_FLAGS_LOCKED
#else
    #if defined(NV_GET_USER_PAGES_HAS_ARGS_TSK_WRITE_FORCE_VMAS)
        static inline long NV_GET_USER_PAGES_REMOTE(struct mm_struct *mm,
                                                    unsigned long start,
                                                    unsigned long nr_pages,
                                                    unsigned int flags,
                                                    struct page **pages,
                                                    int *locked)
        {
            int write = flags & FOLL_WRITE;
            int force = flags & FOLL_FORCE;

            return get_user_pages(NULL, mm, start, nr_pages, write, force, pages, NULL);
        }

    #else
        #define NV_GET_USER_PAGES_REMOTE(mm, start, nr_pages, flags, pages, locked) \
            get_user_pages(NULL, mm, start, nr_pages, flags, pages, NULL)
    #endif // NV_GET_USER_PAGES_HAS_ARGS_TSK_WRITE_FORCE_VMAS
#endif // NV_GET_USER_PAGES_REMOTE_PRESENT

/*
 * The .virtual_address field was effectively renamed to .address, by these
 * two commits:
 *
 *  struct vm_fault: .address was added by:
 *   2016-12-14  82b0f8c39a3869b6fd2a10e180a862248736ec6f
 *
 *  struct vm_fault: .virtual_address was removed by:
 *   2016-12-14  1a29d85eb0f19b7d8271923d8917d7b4f5540b3e
 */
static inline unsigned long nv_page_fault_va(struct vm_fault *vmf)
{
#if defined(NV_VM_FAULT_HAS_ADDRESS)
    return vmf->address;
#else
    return (unsigned long)(vmf->virtual_address);
#endif
}

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
#endif

static inline void nv_vm_flags_set(struct vm_area_struct *vma, vm_flags_t flags)
{
#if !NV_CAN_CALL_VMA_START_WRITE
    nv_vma_start_write(vma);
    ACCESS_PRIVATE(vma, __vm_flags) |= flags;
#elif defined(NV_VM_AREA_STRUCT_HAS_CONST_VM_FLAGS)
    vm_flags_set(vma, flags);
#else
    vma->vm_flags |= flags;
#endif
}

static inline void nv_vm_flags_clear(struct vm_area_struct *vma, vm_flags_t flags)
{
#if !NV_CAN_CALL_VMA_START_WRITE
    nv_vma_start_write(vma);
    ACCESS_PRIVATE(vma, __vm_flags) &= ~flags;
#elif defined(NV_VM_AREA_STRUCT_HAS_CONST_VM_FLAGS)
    vm_flags_clear(vma, flags);
#else
    vma->vm_flags &= ~flags;
#endif
}

#endif // __NV_MM_H__
