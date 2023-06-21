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

#include "uvm_tools.h"
#include "uvm_va_range.h"
#include "uvm_ats_faults.h"
#include "uvm_migrate_pageable.h"

// TODO: Bug 2103669: Implement a real prefetching policy and remove or adapt
// these experimental parameters. These are intended to help guide that policy.
static unsigned int uvm_exp_perf_prefetch_ats_order_replayable = 0;
module_param(uvm_exp_perf_prefetch_ats_order_replayable, uint, 0644);
MODULE_PARM_DESC(uvm_exp_perf_prefetch_ats_order_replayable,
                 "Max order of pages (2^N) to prefetch on replayable ATS faults");

static unsigned int uvm_exp_perf_prefetch_ats_order_non_replayable = 0;
module_param(uvm_exp_perf_prefetch_ats_order_non_replayable, uint, 0644);
MODULE_PARM_DESC(uvm_exp_perf_prefetch_ats_order_non_replayable,
                 "Max order of pages (2^N) to prefetch on non-replayable ATS faults");

// Expand the fault region to the naturally-aligned region with order given by
// the module parameters, clamped to the vma containing fault_addr (if any).
// Note that this means the region contains fault_addr but may not begin at
// fault_addr.
static void expand_fault_region(struct vm_area_struct *vma,
                                NvU64 start,
                                size_t length,
                                uvm_fault_client_type_t client_type,
                                unsigned long *migrate_start,
                                unsigned long *migrate_length)
{
    unsigned int order;
    unsigned long outer, aligned_start, aligned_size;

    *migrate_start = start;
    *migrate_length = length;

    if (client_type == UVM_FAULT_CLIENT_TYPE_HUB)
        order = uvm_exp_perf_prefetch_ats_order_non_replayable;
    else
        order = uvm_exp_perf_prefetch_ats_order_replayable;

    if (order == 0)
        return;

    UVM_ASSERT(vma);
    UVM_ASSERT(order < BITS_PER_LONG - PAGE_SHIFT);

    aligned_size = (1UL << order) * PAGE_SIZE;

    aligned_start = start & ~(aligned_size - 1);

    *migrate_start = max(vma->vm_start, aligned_start);
    outer = min(vma->vm_end, aligned_start + aligned_size);
    *migrate_length = outer - *migrate_start;
}

static NV_STATUS service_ats_faults(uvm_gpu_va_space_t *gpu_va_space,
                                    struct vm_area_struct *vma,
                                    NvU64 start,
                                    size_t length,
                                    uvm_fault_access_type_t access_type,
                                    uvm_fault_client_type_t client_type)
{
    uvm_va_space_t *va_space = gpu_va_space->va_space;
    struct mm_struct *mm = va_space->va_space_mm.mm;
    bool write = (access_type >= UVM_FAULT_ACCESS_TYPE_WRITE);
    NV_STATUS status;
    NvU64 user_space_start;
    NvU64 user_space_length;

    // Request uvm_migrate_pageable() to touch the corresponding page after
    // population.
    // Under virtualization ATS provides two translations:
    // 1) guest virtual -> guest physical
    // 2) guest physical -> host physical
    //
    // The overall ATS translation will fault if either of those translations is
    // invalid. The get_user_pages() call above handles translation #1, but not
    // #2. We don't know if we're running as a guest, but in case we are we can
    // force that translation to be valid by touching the guest physical address
    // from the CPU. If the translation is not valid then the access will cause
    // a hypervisor fault. Note that dma_map_page() can't establish mappings
    // used by GPU ATS SVA translations. GPU accesses to host physical addresses
    // obtained as a result of the address translation request uses the CPU
    // address space instead of the IOMMU address space since the translated
    // host physical address isn't necessarily an IOMMU address. The only way to
    // establish guest physical to host physical mapping in the CPU address
    // space is to touch the page from the CPU.
    //
    // We assume that the hypervisor mappings are all VM_PFNMAP, VM_SHARED, and
    // VM_WRITE, meaning that the mappings are all granted write access on any
    // fault and that the kernel will never revoke them.
    // drivers/vfio/pci/vfio_pci_nvlink2.c enforces this. Thus we can assume
    // that a read fault is always sufficient to also enable write access on the
    // guest translation.

    uvm_migrate_args_t uvm_migrate_args =
    {
        .va_space               = va_space,
        .mm                     = mm,
        .dst_id                 = gpu_va_space->gpu->parent->id,
        .dst_node_id            = -1,
        .populate_permissions   = write ? UVM_POPULATE_PERMISSIONS_WRITE : UVM_POPULATE_PERMISSIONS_ANY,
        .touch                  = true,
        .skip_mapped            = true,
        .user_space_start       = &user_space_start,
        .user_space_length      = &user_space_length,
    };

    UVM_ASSERT(uvm_ats_can_service_faults(gpu_va_space, mm));

    expand_fault_region(vma, start, length, client_type, &uvm_migrate_args.start, &uvm_migrate_args.length);

    // We are trying to use migrate_vma API in the kernel (if it exists) to
    // populate and map the faulting region on the GPU. We want to do this only
    // on the first touch. That is, pages which are not already mapped. So, we
    // set skip_mapped to true. For pages already mapped, this will only handle
    // PTE upgrades if needed.
    status = uvm_migrate_pageable(&uvm_migrate_args);
    if (status == NV_WARN_NOTHING_TO_DO)
        status = NV_OK;

    UVM_ASSERT(status != NV_ERR_MORE_PROCESSING_REQUIRED);

    return status;
}

static void flush_tlb_write_faults(uvm_gpu_va_space_t *gpu_va_space,
                                   NvU64 addr,
                                   size_t size,
                                   uvm_fault_client_type_t client_type)
{
    uvm_ats_fault_invalidate_t *ats_invalidate;

    if (client_type == UVM_FAULT_CLIENT_TYPE_GPC)
        ats_invalidate = &gpu_va_space->gpu->parent->fault_buffer_info.replayable.ats_invalidate;
    else
        ats_invalidate = &gpu_va_space->gpu->parent->fault_buffer_info.non_replayable.ats_invalidate;

    if (!ats_invalidate->write_faults_in_batch) {
        uvm_tlb_batch_begin(&gpu_va_space->page_tables, &ats_invalidate->write_faults_tlb_batch);
        ats_invalidate->write_faults_in_batch = true;
    }

    uvm_tlb_batch_invalidate(&ats_invalidate->write_faults_tlb_batch, addr, size, PAGE_SIZE, UVM_MEMBAR_NONE);
}

NV_STATUS uvm_ats_service_faults(uvm_gpu_va_space_t *gpu_va_space,
                                 struct vm_area_struct *vma,
                                 NvU64 base,
                                 uvm_ats_fault_context_t *ats_context)
{
    NV_STATUS status = NV_OK;
    uvm_va_block_region_t subregion;
    uvm_va_block_region_t region = uvm_va_block_region(0, PAGES_PER_UVM_VA_BLOCK);
    uvm_page_mask_t *read_fault_mask = &ats_context->read_fault_mask;
    uvm_page_mask_t *write_fault_mask = &ats_context->write_fault_mask;
    uvm_page_mask_t *faults_serviced_mask = &ats_context->faults_serviced_mask;
    uvm_page_mask_t *reads_serviced_mask = &ats_context->reads_serviced_mask;
    uvm_fault_client_type_t client_type = ats_context->client_type;

    UVM_ASSERT(vma);
    UVM_ASSERT(IS_ALIGNED(base, UVM_VA_BLOCK_SIZE));
    UVM_ASSERT(g_uvm_global.ats.enabled);
    UVM_ASSERT(gpu_va_space);
    UVM_ASSERT(gpu_va_space->ats.enabled);
    UVM_ASSERT(uvm_gpu_va_space_state(gpu_va_space) == UVM_GPU_VA_SPACE_STATE_ACTIVE);

    uvm_page_mask_zero(faults_serviced_mask);
    uvm_page_mask_zero(reads_serviced_mask);

    if (!(vma->vm_flags & VM_READ))
        return status;

    if (!(vma->vm_flags & VM_WRITE)) {
        // If VMA doesn't have write permissions, all write faults are fatal.
        // Try servicing such faults for read iff they are also present in
        // read_fault_mask. This is because for replayable faults, if there are
        // pending read accesses on the same page, we have to service them
        // before we can cancel the write/atomic faults. So we try with read
        // fault access type even though these write faults are fatal.
        if (ats_context->client_type == UVM_FAULT_CLIENT_TYPE_GPC)
            uvm_page_mask_and(write_fault_mask, write_fault_mask, read_fault_mask);
        else
            uvm_page_mask_zero(write_fault_mask);
    }

    for_each_va_block_subregion_in_mask(subregion, write_fault_mask, region) {
        NvU64 start = base + (subregion.first * PAGE_SIZE);
        size_t length = uvm_va_block_region_num_pages(subregion) * PAGE_SIZE;
        uvm_fault_access_type_t access_type = (vma->vm_flags & VM_WRITE) ?
                                                                          UVM_FAULT_ACCESS_TYPE_WRITE :
                                                                          UVM_FAULT_ACCESS_TYPE_READ;

        UVM_ASSERT(start >= vma->vm_start);
        UVM_ASSERT((start + length) <= vma->vm_end);

        status = service_ats_faults(gpu_va_space, vma, start, length, access_type, client_type);
        if (status != NV_OK)
            return status;

        if (vma->vm_flags & VM_WRITE) {
            uvm_page_mask_region_fill(faults_serviced_mask, subregion);

            // The Linux kernel never invalidates TLB entries on mapping
            // permission upgrade. This is a problem if the GPU has cached
            // entries with the old permission. The GPU will re-fetch the entry
            // if the PTE is invalid and page size is not 4K (this is the case
            // on P9). However, if a page gets upgraded from R/O to R/W and GPU
            // has the PTEs cached with R/O permissions we will enter an
            // infinite loop because we just forward the fault to the Linux
            // kernel and it will see that the permissions in the page table are
            // correct. Therefore, we flush TLB entries on ATS write faults.
            flush_tlb_write_faults(gpu_va_space, start, length, client_type);
        }
        else {
            uvm_page_mask_region_fill(reads_serviced_mask, subregion);
        }
    }

    // Remove write faults from read_fault_mask
    uvm_page_mask_andnot(read_fault_mask, read_fault_mask, write_fault_mask);

    for_each_va_block_subregion_in_mask(subregion, read_fault_mask, region) {
        NvU64 start = base + (subregion.first * PAGE_SIZE);
        size_t length = uvm_va_block_region_num_pages(subregion) * PAGE_SIZE;

        UVM_ASSERT(start >= vma->vm_start);
        UVM_ASSERT((start + length) <= vma->vm_end);

        status = service_ats_faults(gpu_va_space, vma, start, length, UVM_FAULT_ACCESS_TYPE_READ, client_type);
        if (status != NV_OK)
            return status;

        uvm_page_mask_region_fill(faults_serviced_mask, subregion);
    }

    return status;
}

bool uvm_ats_check_in_gmmu_region(uvm_va_space_t *va_space, NvU64 address, uvm_va_range_t *next)
{
    uvm_va_range_t *prev;
    NvU64 gmmu_region_base = UVM_ALIGN_DOWN(address, UVM_GMMU_ATS_GRANULARITY);

    UVM_ASSERT(va_space);

    if (next) {
        if (next->node.start <= gmmu_region_base + UVM_GMMU_ATS_GRANULARITY - 1)
            return true;

        prev = uvm_va_range_container(uvm_range_tree_prev(&va_space->va_range_tree, &next->node));
    }
    else {
        // No VA range exists after address, so check the last VA range in the
        // tree.
        prev = uvm_va_range_container(uvm_range_tree_last(&va_space->va_range_tree));
    }

    return prev && (prev->node.end >= gmmu_region_base);
}

NV_STATUS uvm_ats_invalidate_tlbs(uvm_gpu_va_space_t *gpu_va_space,
                                  uvm_ats_fault_invalidate_t *ats_invalidate,
                                  uvm_tracker_t *out_tracker)
{
    NV_STATUS status;
    uvm_push_t push;

    if (!ats_invalidate->write_faults_in_batch)
        return NV_OK;

    UVM_ASSERT(gpu_va_space);
    UVM_ASSERT(gpu_va_space->ats.enabled);

    status = uvm_push_begin(gpu_va_space->gpu->channel_manager,
                            UVM_CHANNEL_TYPE_MEMOPS,
                            &push,
                            "Invalidate ATS entries");

    if (status == NV_OK) {
        uvm_tlb_batch_end(&ats_invalidate->write_faults_tlb_batch, &push, UVM_MEMBAR_NONE);
        uvm_push_end(&push);

        // Add this push to the GPU's tracker so that fault replays/clears can
        // wait on it
        status = uvm_tracker_add_push_safe(out_tracker, &push);
    }

    ats_invalidate->write_faults_in_batch = false;

    return status;
}

