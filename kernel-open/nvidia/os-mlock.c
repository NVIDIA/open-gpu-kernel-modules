/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define  __NO_VERSION__

#include "os-interface.h"
#include "nv-linux.h"

#if defined(NVCPU_FAMILY_X86) && defined(NV_FOLL_LONGTERM_PRESENT) && \
    (defined(NV_PIN_USER_PAGES_HAS_ARGS_VMAS) ||                      \
     defined(NV_GET_USER_PAGES_HAS_ARGS_FLAGS_VMAS))
#define NV_NUM_PIN_PAGES_PER_ITERATION 0x80000
#endif

static inline int nv_follow_pfn(struct vm_area_struct *vma,
                                unsigned long address,
                                unsigned long *pfn)
{
#if defined(NV_FOLLOW_PFN_PRESENT)
    return follow_pfn(vma, address, pfn);
#else
#if NV_IS_EXPORT_SYMBOL_PRESENT_follow_pte
    int status = 0;
    spinlock_t *ptl;
    pte_t *ptep;

    if (!(vma->vm_flags & (VM_IO | VM_PFNMAP)))
        return status;

    status = follow_pte(vma, address, &ptep, &ptl);
    if (status)
        return status;
    *pfn = pte_pfn(ptep_get(ptep));

    // The lock is acquired inside follow_pte()
    pte_unmap_unlock(ptep, ptl);
    return 0;
#else // NV_IS_EXPORT_SYMBOL_PRESENT_follow_pte
    return -1;
#endif // NV_IS_EXPORT_SYMBOL_PRESENT_follow_pte
#endif
}

/*!
 * @brief Locates the PFNs for a user IO address range, and converts those to
 *        their associated PTEs.
 *
 * @param[in]     vma VMA that contains the virtual address range given by the
 *                    start and page count parameters.
 * @param[in]     start Beginning of the virtual address range of the IO PTEs.
 * @param[in]     page_count Number of pages containing the IO range being
 *                           mapped.
 * @param[in,out] pte_array Storage array for PTE addresses. Must be large
 *                          enough to contain at least page_count pointers.
 *
 * @return NV_OK if the PTEs were identified successfully, error otherwise.
 */
static NV_STATUS get_io_ptes(struct vm_area_struct *vma,
                             NvUPtr start,
                             NvU64 page_count,
                             NvU64 **pte_array)
{
    NvU64 i;
    unsigned long pfn;

    for (i = 0; i < page_count; i++)
    {
        if (nv_follow_pfn(vma, (start + (i * PAGE_SIZE)), &pfn) < 0)
        {
            return NV_ERR_INVALID_ADDRESS;
        }

        pte_array[i] = (NvU64 *)(pfn << PAGE_SHIFT);

        if (i == 0)
            continue;

        //
        // This interface is to be used for contiguous, uncacheable I/O regions.
        // Internally, osCreateOsDescriptorFromIoMemory() checks the user-provided
        // flags against this, and creates a single memory descriptor with the same
        // attributes. This check ensures the actual mapping supplied matches the
        // user's declaration. Ensure the PFNs represent a contiguous range,
        // error if they do not.
        //
        if ((NvU64)pte_array[i] != (((NvU64)pte_array[i-1]) + PAGE_SIZE))
        {
            return NV_ERR_INVALID_ADDRESS;
        }
    }
    return NV_OK;
}

NV_STATUS NV_API_CALL os_lookup_user_io_memory(
    void   *address,
    NvU64   page_count,
    NvU64 **pte_array
)
{
    NV_STATUS rmStatus;
    struct mm_struct *mm = current->mm;
    struct vm_area_struct *vma;
    unsigned long pfn;
    NvUPtr start = (NvUPtr)address;
    void **result_array;

    if (!NV_MAY_SLEEP())
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: %s(): invalid context!\n", __FUNCTION__);
        return NV_ERR_NOT_SUPPORTED;
    }

    rmStatus = os_alloc_mem((void **)&result_array, (page_count * sizeof(NvP64)));
    if (rmStatus != NV_OK)
    {
        nv_printf(NV_DBG_ERRORS,
                "NVRM: failed to allocate page table!\n");
        return rmStatus;
    }

    nv_mmap_read_lock(mm);

    // find the first VMA which intersects the interval start_addr..end_addr-1,
    vma = find_vma_intersection(mm, start, start+1);

    // Verify that the given address range is contained in a single vma
    if ((vma == NULL) || ((vma->vm_flags & (VM_IO | VM_PFNMAP)) == 0) ||
            !((vma->vm_start <= start) &&
              ((vma->vm_end - start) >> PAGE_SHIFT >= page_count)))
    {
        nv_printf(NV_DBG_ERRORS,
                "Cannot map memory with base addr 0x%llx and size of 0x%llx pages\n",
                start ,page_count);
        rmStatus = NV_ERR_INVALID_ADDRESS;
        goto done;
    }

    if (nv_follow_pfn(vma, start, &pfn) < 0)
    {
        rmStatus = NV_ERR_INVALID_ADDRESS;
        goto done;
    }

    rmStatus = get_io_ptes(vma, start, page_count, (NvU64 **)result_array);
    if (rmStatus == NV_OK)
        *pte_array = (NvU64 *)result_array;

done:
    nv_mmap_read_unlock(mm);

    if (rmStatus != NV_OK)
    {
        os_free_mem(result_array);
    }

    return rmStatus;
}

NV_STATUS NV_API_CALL os_lock_user_pages(
    void   *address,
    NvU64   page_count,
    void  **page_array,
    NvU32   flags
)
{
    NV_STATUS rmStatus;
    struct mm_struct *mm = current->mm;
    struct page **user_pages;
    NvU64 i;
    NvU64 npages = page_count;
    NvU64 pinned = 0;
    unsigned int gup_flags = DRF_VAL(_LOCK_USER_PAGES, _FLAGS, _WRITE, flags) ? FOLL_WRITE : 0;
    long ret;

#if defined(NVCPU_FAMILY_X86) && defined(NV_FOLL_LONGTERM_PRESENT)
    gup_flags |= FOLL_LONGTERM;
#endif

    if (!NV_MAY_SLEEP())
    {
        nv_printf(NV_DBG_ERRORS,
            "NVRM: %s(): invalid context!\n", __FUNCTION__);
        return NV_ERR_NOT_SUPPORTED;
    }

    rmStatus = os_alloc_mem((void **)&user_pages,
            (page_count * sizeof(*user_pages)));
    if (rmStatus != NV_OK)
    {
        nv_printf(NV_DBG_ERRORS,
                "NVRM: failed to allocate page table!\n");
        return rmStatus;
    }

    nv_mmap_read_lock(mm);
    ret = NV_PIN_USER_PAGES((unsigned long)address,
                            npages, gup_flags, user_pages);
    if (ret > 0)
    {
        pinned = ret;
    }
#if defined(NVCPU_FAMILY_X86) && defined(NV_FOLL_LONGTERM_PRESENT) && \
    (defined(NV_PIN_USER_PAGES_HAS_ARGS_VMAS) ||                      \
     defined(NV_GET_USER_PAGES_HAS_ARGS_FLAGS_VMAS))
    //
    // NV_PIN_USER_PAGES() passes in NULL for the vmas parameter (if required)
    // in pin_user_pages() (or get_user_pages() if pin_user_pages() does not
    // exist). For kernels which do not contain the commit 52650c8b466b
    // (mm/gup: remove the vma allocation from gup_longterm_locked()), if
    // FOLL_LONGTERM is passed in, this results in the kernel trying to kcalloc
    // the vmas array, and since the limit for kcalloc is 4 MB, it results in
    // NV_PIN_USER_PAGES() failing with ENOMEM if more than
    // NV_NUM_PIN_PAGES_PER_ITERATION pages are requested on 64-bit systems.
    //
    // As a workaround, if we requested more than
    // NV_NUM_PIN_PAGES_PER_ITERATION pages and failed with ENOMEM, try again
    // with multiple calls of NV_NUM_PIN_PAGES_PER_ITERATION pages at a time.
    //
    else if ((ret == -ENOMEM) &&
             (page_count > NV_NUM_PIN_PAGES_PER_ITERATION))
    {
        for (pinned = 0; pinned < page_count; pinned += ret)
        {
            npages = page_count - pinned;
            if (npages > NV_NUM_PIN_PAGES_PER_ITERATION)
            {
                npages = NV_NUM_PIN_PAGES_PER_ITERATION;
            }

            ret = NV_PIN_USER_PAGES(((unsigned long) address) + (pinned * PAGE_SIZE),
                                    npages, gup_flags, &user_pages[pinned]);
            if (ret <= 0)
            {
                break;
            }
        }
    }
#endif
    nv_mmap_read_unlock(mm);

    if (pinned < page_count)
    {
        for (i = 0; i < pinned; i++)
            NV_UNPIN_USER_PAGE(user_pages[i]);
        os_free_mem(user_pages);
        return NV_ERR_INVALID_ADDRESS;
    }

    *page_array = user_pages;

    return NV_OK;
}

NV_STATUS NV_API_CALL os_unlock_user_pages(
    NvU64  page_count,
    void  *page_array
)
{
    NvBool write = 1;
    struct page **user_pages = page_array;
    NvU32 i;

    for (i = 0; i < page_count; i++)
    {
        if (write)
            set_page_dirty_lock(user_pages[i]);
        NV_UNPIN_USER_PAGE(user_pages[i]);
    }

    os_free_mem(user_pages);

    return NV_OK;
}
