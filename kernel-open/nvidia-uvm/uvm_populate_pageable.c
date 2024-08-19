/*******************************************************************************
    Copyright (c) 2018-2023 NVIDIA Corporation

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to
    deal in the Software without restriction, including without limitation the
    rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
    sell copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

        The above copyright notice and this permission notice shall be
        included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.

*******************************************************************************/

#include "uvm_common.h"
#include "uvm_ioctl.h"
#include "uvm_linux.h"
#include "uvm_lock.h"
#include "uvm_api.h"
#include "uvm_va_range.h"
#include "uvm_va_space.h"
#include "uvm_populate_pageable.h"

#if defined(NV_HANDLE_MM_FAULT_HAS_MM_ARG)
#define UVM_HANDLE_MM_FAULT(vma, addr, flags)       handle_mm_fault(vma->vm_mm, vma, addr, flags)
#elif defined(NV_HANDLE_MM_FAULT_HAS_PT_REGS_ARG)
#define UVM_HANDLE_MM_FAULT(vma, addr, flags)       handle_mm_fault(vma, addr, flags, NULL)
#else
#define UVM_HANDLE_MM_FAULT(vma, addr, flags)       handle_mm_fault(vma, addr, flags)
#endif

static bool is_write_populate(struct vm_area_struct *vma, uvm_populate_permissions_t populate_permissions)
{
    switch (populate_permissions) {
        case UVM_POPULATE_PERMISSIONS_INHERIT:
            return vma->vm_flags & VM_WRITE;
        case UVM_POPULATE_PERMISSIONS_ANY:
            return false;
        case UVM_POPULATE_PERMISSIONS_WRITE:
            return true;
        default:
            UVM_ASSERT(0);
            return false;
    }
}

static NV_STATUS handle_fault(struct vm_area_struct *vma, unsigned long start, unsigned long vma_num_pages, bool write)
{
    NV_STATUS status = NV_OK;

    unsigned long i;
    unsigned int ret = 0;
    unsigned int fault_flags = write ? FAULT_FLAG_WRITE : 0;

#if defined(NV_MM_HAS_FAULT_FLAG_REMOTE)
    fault_flags |= (FAULT_FLAG_REMOTE);
#endif

    for (i = 0; i < vma_num_pages; i++) {
        ret = UVM_HANDLE_MM_FAULT(vma, start + (i * PAGE_SIZE), fault_flags);
        if (ret & VM_FAULT_ERROR) {
#if defined(NV_VM_FAULT_TO_ERRNO_PRESENT)
            int err = vm_fault_to_errno(ret, fault_flags);
            status = errno_to_nv_status(err);
#else
            status = errno_to_nv_status(-EFAULT);
#endif
            break;
        }
    }

    return status;
}

NV_STATUS uvm_populate_pageable_vma(struct vm_area_struct *vma,
                                    unsigned long start,
                                    unsigned long length,
                                    int min_prot,
                                    bool touch,
                                    uvm_populate_permissions_t populate_permissions)
{
    unsigned long vma_num_pages;
    unsigned long outer = start + length;
    unsigned int gup_flags = is_write_populate(vma, populate_permissions) ? FOLL_WRITE : 0;
    struct mm_struct *mm = vma->vm_mm;
    unsigned long vm_flags = vma->vm_flags;
    bool uvm_managed_vma;
    long ret;
    struct page **pages = NULL;
    NV_STATUS status = NV_OK;

    UVM_ASSERT(PAGE_ALIGNED(start));
    UVM_ASSERT(PAGE_ALIGNED(outer));
    UVM_ASSERT(vma->vm_end > start);
    UVM_ASSERT(vma->vm_start < outer);
    uvm_assert_mmap_lock_locked(mm);

    // On most CPU architectures, write permission implies RW permission.
    if (vm_flags & VM_WRITE)
        vm_flags = vm_flags | VM_READ;

    if ((vm_flags & min_prot) != min_prot)
        return NV_ERR_INVALID_ADDRESS;

    // Adjust to input range boundaries
    start = max(start, vma->vm_start);
    outer = min(outer, vma->vm_end);

    vma_num_pages = (outer - start) / PAGE_SIZE;

    // Please see the comment in uvm_ats_service_fault() regarding the usage of
    // the touch parameter for more details.
    if (touch) {
        pages = uvm_kvmalloc(vma_num_pages * sizeof(pages[0]));
        if (!pages)
            return NV_ERR_NO_MEMORY;
    }

    // If the input vma is managed by UVM, temporarily remove the record
    // associated with the locking of mmap_lock, in order to avoid a "locked
    // twice" validation error triggered when also acquiring mmap_lock in the
    // page fault handler. The page fault is caused by get_user_pages.
    uvm_managed_vma = uvm_file_is_nvidia_uvm(vma->vm_file);
    if (uvm_managed_vma)
        uvm_record_unlock_mmap_lock_read(mm);

    status = handle_fault(vma, start, vma_num_pages, !!(gup_flags & FOLL_WRITE));
    if (status != NV_OK)
        goto out;

    // Kernel v6.6 introduced a bug in set_pte_range() around the handling of AF
    // bit. Instead of setting the AF bit, the bit is incorrectly being cleared
    // in set_pte_range() during first-touch fault handling. Calling
    // handle_mm_fault() again takes a different code path which correctly sets
    // the AF bit.
    status = handle_fault(vma, start, vma_num_pages, !!(gup_flags & FOLL_WRITE));
    if (status != NV_OK)
        goto out;

    if (touch)
        ret = NV_PIN_USER_PAGES_REMOTE(mm, start, vma_num_pages, gup_flags, pages, NULL);
    else
        ret = NV_GET_USER_PAGES_REMOTE(mm, start, vma_num_pages, gup_flags, pages, NULL);

    if (uvm_managed_vma)
        uvm_record_lock_mmap_lock_read(mm);

    if (ret < 0) {
        status = errno_to_nv_status(ret);
        goto out;
    }

    // We couldn't populate all pages, return error
    if (ret < vma_num_pages) {
        if (touch) {
            unsigned long i;

            for (i = 0; i < ret; i++) {
                UVM_ASSERT(pages[i]);
                NV_UNPIN_USER_PAGE(pages[i]);
            }
        }

        status = NV_ERR_NO_MEMORY;
        goto out;
    }

    if (touch) {
        unsigned long i;

        for (i = 0; i < vma_num_pages; i++) {
            uvm_touch_page(pages[i]);
            NV_UNPIN_USER_PAGE(pages[i]);
        }
    }

out:
    uvm_kvfree(pages);
    return status;
}

NV_STATUS uvm_populate_pageable(struct mm_struct *mm,
                                const unsigned long start,
                                const unsigned long length,
                                int min_prot,
                                bool touch,
                                uvm_populate_permissions_t populate_permissions)
{
    struct vm_area_struct *vma;
    const unsigned long end = start + length;
    unsigned long prev_end = end;

    UVM_ASSERT(PAGE_ALIGNED(start));
    UVM_ASSERT(PAGE_ALIGNED(length));
    uvm_assert_mmap_lock_locked(mm);

    vma = find_vma_intersection(mm, start, end);
    if (!vma || (start < vma->vm_start))
         return NV_ERR_INVALID_ADDRESS;

    // VMAs are validated and populated one at a time, since they may have
    // different protection flags
    // Validation of VM_SPECIAL flags is delegated to get_user_pages
    for (; vma && vma->vm_start <= prev_end; vma = find_vma_intersection(mm, prev_end, end)) {
        NV_STATUS status = uvm_populate_pageable_vma(vma, start, end - start, min_prot, touch, populate_permissions);

        if (status != NV_OK)
            return status;

        if (vma->vm_end >= end)
            return NV_OK;

        prev_end = vma->vm_end;
    }

    // Input range not fully covered by VMAs
    return NV_ERR_INVALID_ADDRESS;
}

NV_STATUS uvm_api_populate_pageable(const UVM_POPULATE_PAGEABLE_PARAMS *params, struct file *filp)
{
    NV_STATUS status;
    bool allow_managed;
    bool skip_prot_check;
    int min_prot;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    if (params->flags & ~UVM_POPULATE_PAGEABLE_FLAGS_ALL)
        return NV_ERR_INVALID_ARGUMENT;

    if ((params->flags & UVM_POPULATE_PAGEABLE_FLAGS_TEST_ALL) && !uvm_enable_builtin_tests) {
        UVM_INFO_PRINT("Test flag set for UVM_POPULATE_PAGEABLE. Did you mean to insmod with uvm_enable_builtin_tests=1?\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Population of managed ranges is only allowed for test purposes. The goal
    // is to validate that it is possible to populate pageable ranges backed by
    // VMAs with the VM_MIXEDMAP or VM_DONTEXPAND special flags set. But since
    // there is no portable way to force allocation of such memory from user
    // space, and it is not safe to change the flags of an already created
    // VMA from kernel space, we take advantage of the fact that managed ranges
    // have both special flags set at creation time (see uvm_mmap)
    allow_managed = params->flags & UVM_POPULATE_PAGEABLE_FLAG_ALLOW_MANAGED;

    skip_prot_check = params->flags & UVM_POPULATE_PAGEABLE_FLAG_SKIP_PROT_CHECK;
    if (skip_prot_check)
        min_prot = 0;
    else
        min_prot = VM_READ;

    // Check size, alignment and overflow. VMA validations are performed by
    // populate_pageable
    if (uvm_api_range_invalid(params->base, params->length))
        return NV_ERR_INVALID_ADDRESS;

    // mmap_lock is needed to traverse the vmas in the input range and call
    // into get_user_pages. Unlike most UVM APIs, this one is defined to only
    // work on current->mm, not the mm associated with the VA space (if any).
    uvm_down_read_mmap_lock(current->mm);

    if (allow_managed || uvm_va_space_range_empty(va_space, params->base, params->base + params->length - 1)) {
        status = uvm_populate_pageable(current->mm,
                                       params->base,
                                       params->length,
                                       min_prot,
                                       false,
                                       UVM_POPULATE_PERMISSIONS_INHERIT);
    }
    else {
        status = NV_ERR_INVALID_ADDRESS;
    }

    uvm_up_read_mmap_lock(current->mm);

    return status;
}
