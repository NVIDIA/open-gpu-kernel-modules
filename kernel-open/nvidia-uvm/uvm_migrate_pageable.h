/*******************************************************************************
    Copyright (c) 2018 NVIDIA Corporation

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

#ifndef __UVM_MIGRATE_PAGEABLE_H__
#define __UVM_MIGRATE_PAGEABLE_H__

#include "uvm_common.h"
#include "uvm_linux.h"
#include "uvm_populate_pageable.h"
#include "uvm_forward_decl.h"
#include "uvm_processors.h"

typedef struct
{
    uvm_va_space_t                  *va_space;
    struct mm_struct                *mm;
    const unsigned long             start;
    const unsigned long             length;
    uvm_processor_id_t              dst_id;

    // dst_node_id may be clobbered by uvm_migrate_pageable().
    int                             dst_node_id;
    uvm_populate_permissions_t      populate_permissions;
    bool                            touch : 1;
    bool                            skip_mapped : 1;
    bool                            populate_on_cpu_alloc_failures : 1;
    NvU64                           *user_space_start;
    NvU64                           *user_space_length;
} uvm_migrate_args_t;

#if defined(CONFIG_MIGRATE_VMA_HELPER)
#define UVM_MIGRATE_VMA_SUPPORTED 1
#else
#if NV_IS_EXPORT_SYMBOL_PRESENT_migrate_vma_setup
#define UVM_MIGRATE_VMA_SUPPORTED 1
#endif
#endif

#ifdef UVM_MIGRATE_VMA_SUPPORTED
#include <linux/migrate.h>

// The calls to migrate_vma are capped at 512 pages to set an upper bound on the
// amount of metadata that needs to be allocated for the operation. This number
// was chosen because performance seems to plateau at this size on 64K-pages
// kernels. On kernels with PAGE_SIZE == 4K, 512 pages correspond to 2M VA block,
// which is also a standard size for batch operations.
#define UVM_MIGRATE_VMA_MAX_PAGES (512UL)
#define UVM_MIGRATE_VMA_MAX_SIZE (UVM_MIGRATE_VMA_MAX_PAGES * PAGE_SIZE)

typedef struct
{
    // Input parameters
    uvm_migrate_args_t  *uvm_migrate_args;

    // Output parameters
    //
    // Error code. This only signals errors in internal UVM operations.
    // Pages that failed allocation or could not be populated are communicated
    // using the fields below.
    NV_STATUS           status;

    // Mask of pages that couldn't be made resident on the destination because
    // (a) they are backed with data but pages are not populated (e.g. in swap),
    // (b) pages are not backed with any data yet but were not populated
    // due to the vma not being READ_WRITE, as it would not charge the pages to
    // the process properly.
    // (c) pages are already mapped and such pages were requested to not be
    // migrated via skip_mapped.
    // (d) pages which couldn't be migrated by the kernel.
    DECLARE_BITMAP(populate_pages_mask, UVM_MIGRATE_VMA_MAX_PAGES);

    // Mask of pages that failed allocation on the destination
    DECLARE_BITMAP(allocation_failed_mask, UVM_MIGRATE_VMA_MAX_PAGES);

    // Mask of pages which are already resident at the destination.
    DECLARE_BITMAP(dst_resident_pages_mask, UVM_MIGRATE_VMA_MAX_PAGES);

    // Global state managed by the caller
    //
    // These are scratch masks that can be used by the migrate_vma caller to
    // save output page masks and orchestrate the migrate_vma
    // retries/population calls if needed.
    DECLARE_BITMAP(scratch1_mask, UVM_MIGRATE_VMA_MAX_PAGES);
    DECLARE_BITMAP(scratch2_mask, UVM_MIGRATE_VMA_MAX_PAGES);

    // Arrays used by migrate_vma to store the src/dst pfns
    unsigned long dst_pfn_array[UVM_MIGRATE_VMA_MAX_PAGES];
    unsigned long src_pfn_array[UVM_MIGRATE_VMA_MAX_PAGES];

    // Internal state
    //
    uvm_tracker_t tracker;

    struct {
        // Array of page IOMMU mappings created during allocate_and_copy.
        // Required when using SYS aperture. They are freed in
        // finalize_and_map. Also keep an array with the GPUs for which the
        // mapping was created.
        NvU64              addrs[UVM_MIGRATE_VMA_MAX_PAGES];
        uvm_gpu_t    *addrs_gpus[UVM_MIGRATE_VMA_MAX_PAGES];

        // Mask of pages with entries in the dma address arrays above
        DECLARE_BITMAP(page_mask, UVM_MIGRATE_VMA_MAX_PAGES);

        // Number of pages for which IOMMU mapping were created
        unsigned  long num_pages;
    } dma;

    // Processors where pages are resident before calling migrate_vma
    uvm_processor_mask_t src_processors;

    // Array of per-processor page masks with the pages that are resident
    // before calling migrate_vma.
    struct {
        DECLARE_BITMAP(page_mask, UVM_MIGRATE_VMA_MAX_PAGES);
    } processors[UVM_ID_MAX_PROCESSORS];

    // Number of pages in the migrate_vma call
    unsigned long num_pages;

    // Number of pages that are directly populated on the destination
    unsigned long num_populate_anon_pages;
} migrate_vma_state_t;

#if defined(CONFIG_MIGRATE_VMA_HELPER)
struct migrate_vma {
    struct vm_area_struct   *vma;
    unsigned long           *dst;
    unsigned long           *src;
    unsigned long           start;
    unsigned long           end;
};
#endif // CONFIG_MIGRATE_VMA_HELPER

// Populates the given VA range and tries to migrate all the pages to dst_id. If
// the destination processor is the CPU, the NUMA node in dst_node_id is used.
// The input VA range must be fully backed by VMAs. This function relies on
// migrate_vma, which was added in Linux 4.14. If skip_mapped is set to true,
// then already mapped pages will not be migrated. For kernels that do not
// provide migrate_vma, this function populates the memory using get_user_pages
// and returns NV_WARN_NOTHING_TO_DO to complete the migration in user space for
// API calls made from userspace. Kernel callers are expected to handle this
// error according to their respective usecases. user_space_start and
// user_space_length will contain the full input range. skip_mapped is ignored
// for such kernels. If the destination is the CPU and dst_node_id is full,
// NV_ERR_MORE_PROCESSING_REQUIRED is returned and user-space will call
// UVM_MIGRATE with the next preferred CPU node (if more are available),
// starting at the address specified by user_space_start. If the destination is
// a GPU and a page could not be populated, return NV_ERR_NO_MEMORY. Otherwise,
// return NV_OK. This is fine because UvmMigrate/UvmMigrateAsync only guarantee
// that the memory is populated somewhere in the system, not that pages moved to
// the requested processor.
//
// migrate_vma does not support file-backed vmas yet. If a file-backed vma is
// found, populates the pages corresponding to the vma with get_user_pages() and
// returns NV_WARN_NOTHING_TO_DO. The caller is expected to handle this error.
// API calls will fall back to user-mode to complete the migration. Kernel
// callers are expected to handle this error according to the respective
// usecases. If NV_WARN_NOTHING_TO_DO is returned, user_space_start and
// user_space_length will contain the intersection of the vma address range and
// [start:start + length].
//
// If a user-mode fallback is required but current->mm != uvm_migrate_args->mm,
// NV_ERR_NOT_SUPPORTED is returned since user mode can't perform such a
// migration. If the caller is a kernel thread, such as the GPU BH, this check
// is ignored since the caller is not expected to take such a fallback.
//
// Also, if no GPUs have been registered in the VA space, populates the pages
// corresponding to the first vma in the requested region using get_user_pages()
// and returns NV_WARN_NOTHING_TO_DO to fall back to user space for the
// userspace API callers to complete the whole migration. Kernel callers are
// expected to handle this error according to their respective usecases.
//
// If touch is true, a touch will be attempted on all pages in the requested
// range. All pages are only guaranteed to have been touched if
// NV_WARN_NOTHING_TO_DO or NV_OK is returned.
//
// Locking: mmap_lock must be held in read or write mode
NV_STATUS uvm_migrate_pageable(uvm_migrate_args_t *uvm_migrate_args);

NV_STATUS uvm_migrate_pageable_init(void);

void uvm_migrate_pageable_exit(void);

NV_STATUS uvm_test_skip_migrate_vma(UVM_TEST_SKIP_MIGRATE_VMA_PARAMS *params, struct file *filp);

#else // UVM_MIGRATE_VMA_SUPPORTED

static NV_STATUS uvm_migrate_pageable(uvm_migrate_args_t *uvm_migrate_args)
{
    NV_STATUS status;

    if (current->mm != uvm_migrate_args->mm && !(current->flags & PF_KTHREAD))
        return NV_ERR_NOT_SUPPORTED;

    status = uvm_populate_pageable(uvm_migrate_args->mm,
                                   uvm_migrate_args->start,
                                   uvm_migrate_args->length,
                                   0,
                                   uvm_migrate_args->touch,
                                   uvm_migrate_args->populate_permissions);
    if (status != NV_OK)
        return status;

    *(uvm_migrate_args->user_space_start) = uvm_migrate_args->start;
    *(uvm_migrate_args->user_space_length) = uvm_migrate_args->length;

    return NV_WARN_NOTHING_TO_DO;
}

static NV_STATUS uvm_migrate_pageable_init(void)
{
    return NV_OK;
}

static void uvm_migrate_pageable_exit(void)
{
}

static inline NV_STATUS uvm_test_skip_migrate_vma(UVM_TEST_SKIP_MIGRATE_VMA_PARAMS *params, struct file *filp)
{
    return NV_OK;
}
#endif // UVM_MIGRATE_VMA_SUPPORTED

#endif
