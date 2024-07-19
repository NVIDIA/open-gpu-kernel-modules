/*******************************************************************************
    Copyright (c) 2018-2024 NVIDIA Corporation

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
#include "uvm_linux.h"
#include "uvm_gpu.h"
#include "uvm_lock.h"
#include "uvm_va_space.h"
#include "uvm_tracker.h"
#include "uvm_api.h"
#include "uvm_push.h"
#include "uvm_hal.h"
#include "uvm_migrate_pageable.h"
#include "uvm_populate_pageable.h"

#ifdef UVM_MIGRATE_VMA_SUPPORTED

static struct kmem_cache *g_uvm_migrate_vma_state_cache __read_mostly;

static const gfp_t g_migrate_vma_gfp_flags = NV_UVM_GFP_FLAGS | GFP_HIGHUSER_MOVABLE | __GFP_THISNODE;

// Compute the address needed for copying_gpu to access the given page,
// resident on resident_id.
static NV_STATUS migrate_vma_page_copy_address(struct page *page,
                                               unsigned long page_index,
                                               uvm_processor_id_t resident_id,
                                               uvm_gpu_t *copying_gpu,
                                               migrate_vma_state_t *state,
                                               uvm_gpu_address_t *gpu_addr)
{
    uvm_va_space_t *va_space = state->uvm_migrate_args->va_space;
    uvm_gpu_t *owning_gpu = UVM_ID_IS_CPU(resident_id)? NULL: uvm_va_space_get_gpu(va_space, resident_id);
    const bool can_copy_from = uvm_processor_mask_test(&va_space->can_copy_from[uvm_id_value(copying_gpu->id)],
                                                       resident_id);

    UVM_ASSERT(page_index < state->num_pages);

    memset(gpu_addr, 0, sizeof(*gpu_addr));

    if (owning_gpu == copying_gpu) {
        // Local vidmem address
        *gpu_addr = uvm_gpu_address_copy(owning_gpu, uvm_gpu_page_to_phys_address(owning_gpu, page));
    }
    else if (owning_gpu && can_copy_from) {
        uvm_gpu_identity_mapping_t *gpu_peer_mappings = uvm_gpu_get_peer_mapping(copying_gpu, owning_gpu->id);
        uvm_gpu_phys_address_t phys_addr = uvm_gpu_page_to_phys_address(owning_gpu, page);

        *gpu_addr = uvm_gpu_address_virtual(gpu_peer_mappings->base + phys_addr.address);
    }
    else {
        NV_STATUS status = uvm_parent_gpu_map_cpu_page(copying_gpu->parent, page, &state->dma.addrs[page_index]);

        if (status != NV_OK)
            return status;

        state->dma.addrs_gpus[page_index] = copying_gpu;

        if (state->dma.num_pages++ == 0)
            bitmap_zero(state->dma.page_mask, state->num_pages);

        UVM_ASSERT(!test_bit(page_index, state->dma.page_mask));

        __set_bit(page_index, state->dma.page_mask);

        *gpu_addr = uvm_gpu_address_copy(copying_gpu,
                                         uvm_gpu_phys_address(UVM_APERTURE_SYS, state->dma.addrs[page_index]));
    }

    return NV_OK;
}

// Create a new push to zero pages on dst_id
static NV_STATUS migrate_vma_zero_begin_push(uvm_va_space_t *va_space,
                                             uvm_processor_id_t dst_id,
                                             uvm_gpu_t *gpu,
                                             unsigned long start,
                                             unsigned long outer,
                                             uvm_push_t *push)
{
    uvm_channel_type_t channel_type;

    if (UVM_ID_IS_CPU(dst_id)) {
        channel_type = UVM_CHANNEL_TYPE_GPU_TO_CPU;
    }
    else {
        UVM_ASSERT(uvm_id_equal(dst_id, gpu->id));
        channel_type = UVM_CHANNEL_TYPE_GPU_INTERNAL;
    }

    return uvm_push_begin(gpu->channel_manager,
                          channel_type,
                          push,
                          "Zero %s from %s VMA region [0x%lx, 0x%lx]",
                          uvm_va_space_processor_name(va_space, dst_id),
                          uvm_va_space_processor_name(va_space, gpu->id),
                          start,
                          outer);
}

// Create a new push to copy pages between src_id and dst_id
static NV_STATUS migrate_vma_copy_begin_push(uvm_va_space_t *va_space,
                                             uvm_processor_id_t dst_id,
                                             uvm_processor_id_t src_id,
                                             unsigned long start,
                                             unsigned long outer,
                                             uvm_push_t *push)
{
    uvm_channel_type_t channel_type;
    uvm_gpu_t *gpu;

    UVM_ASSERT_MSG(!uvm_id_equal(src_id, dst_id),
                   "Unexpected copy to self, processor %s\n",
                   uvm_va_space_processor_name(va_space, src_id));

    if (UVM_ID_IS_CPU(src_id)) {
        gpu = uvm_va_space_get_gpu(va_space, dst_id);
        channel_type = UVM_CHANNEL_TYPE_CPU_TO_GPU;
    }
    else if (UVM_ID_IS_CPU(dst_id)) {
        gpu = uvm_va_space_get_gpu(va_space, src_id);
        channel_type = UVM_CHANNEL_TYPE_GPU_TO_CPU;
    }
    else {
        // For GPU to GPU copies, prefer to "push" the data from the source as
        // that works better
        gpu = uvm_va_space_get_gpu(va_space, src_id);

        channel_type = UVM_CHANNEL_TYPE_GPU_TO_GPU;
    }

    // NUMA-enabled GPUs can copy to any other NUMA node in the system even if
    // P2P access has not been explicitly enabled (ie va_space->can_copy_from
    // is not set).
    if (!gpu->mem_info.numa.enabled) {
        UVM_ASSERT_MSG(uvm_processor_mask_test(&va_space->can_copy_from[uvm_id_value(gpu->id)], dst_id),
                       "GPU %s dst %s src %s\n",
                       uvm_va_space_processor_name(va_space, gpu->id),
                       uvm_va_space_processor_name(va_space, dst_id),
                       uvm_va_space_processor_name(va_space, src_id));
        UVM_ASSERT_MSG(uvm_processor_mask_test(&va_space->can_copy_from[uvm_id_value(gpu->id)], src_id),
                       "GPU %s dst %s src %s\n",
                       uvm_va_space_processor_name(va_space, gpu->id),
                       uvm_va_space_processor_name(va_space, dst_id),
                       uvm_va_space_processor_name(va_space, src_id));
    }

    if (channel_type == UVM_CHANNEL_TYPE_GPU_TO_GPU) {
        uvm_gpu_t *dst_gpu = uvm_va_space_get_gpu(va_space, dst_id);
        return uvm_push_begin_gpu_to_gpu(gpu->channel_manager,
                                         dst_gpu,
                                         push,
                                         "Copy from %s to %s for VMA region [0x%lx, 0x%lx]",
                                         uvm_va_space_processor_name(va_space, src_id),
                                         uvm_va_space_processor_name(va_space, dst_id),
                                         start,
                                         outer);
    }

    return uvm_push_begin(gpu->channel_manager,
                          channel_type,
                          push,
                          "Copy from %s to %s for VMA region [0x%lx, 0x%lx]",
                          uvm_va_space_processor_name(va_space, src_id),
                          uvm_va_space_processor_name(va_space, dst_id),
                          start,
                          outer);
}

static void migrate_vma_compute_masks(struct vm_area_struct *vma, const unsigned long *src, migrate_vma_state_t *state)
{
    unsigned long i;
    const bool is_rw = vma->vm_flags & VM_WRITE;
    uvm_migrate_args_t *uvm_migrate_args = state->uvm_migrate_args;
    uvm_processor_id_t dst_id = uvm_migrate_args->dst_id;

    UVM_ASSERT(vma_is_anonymous(vma));

    bitmap_zero(state->populate_pages_mask, state->num_pages);
    bitmap_zero(state->allocation_failed_mask, state->num_pages);
    bitmap_zero(state->dst_resident_pages_mask, state->num_pages);

    uvm_processor_mask_zero(&state->src_processors);
    state->num_populate_anon_pages = 0;
    state->dma.num_pages = 0;

    for (i = 0; i < state->num_pages; ++i) {
        uvm_processor_id_t src_id;
        struct page *src_page = NULL;
        int src_nid;
        uvm_gpu_t *src_gpu = NULL;

        // Skip pages that cannot be migrated
        if (!(src[i] & MIGRATE_PFN_MIGRATE)) {
            // This can happen in two cases :
            // - Page is populated but can't be migrated.
            // - Page isn't populated
            // In both the above cases, treat the page as failing migration and
            // populate with get_user_pages.
            if (!(src[i] & MIGRATE_PFN_VALID))
                __set_bit(i, state->populate_pages_mask);

            continue;
        }

        src_page = migrate_pfn_to_page(src[i]);
        if (!src_page) {
            if (is_rw) {
                // Populate PROT_WRITE vmas in migrate_vma so we can use the
                // GPU's copy engines
                if (state->num_populate_anon_pages++ == 0)
                    bitmap_zero(state->processors[uvm_id_value(dst_id)].page_mask, state->num_pages);

                __set_bit(i, state->processors[uvm_id_value(dst_id)].page_mask);
            }
            else {
                // PROT_NONE vmas cannot be populated. PROT_READ anonymous vmas
                // are populated using the zero page. In order to match this
                // behavior, we tell the caller to populate using
                // get_user_pages.
                __set_bit(i, state->populate_pages_mask);
            }

            continue;
        }

        // Page is already mapped. Skip migration of this page if requested.
        if (uvm_migrate_args->skip_mapped) {
            __set_bit(i, state->populate_pages_mask);
            continue;
        }

        src_nid = page_to_nid(src_page);

        // Already at destination
        if (src_nid == uvm_migrate_args->dst_node_id) {
            __set_bit(i, state->dst_resident_pages_mask);
            continue;
        }

        // Already resident on a CPU node, don't move
        if (UVM_ID_IS_CPU(dst_id) && node_state(src_nid, N_CPU)) {
            __set_bit(i, state->dst_resident_pages_mask);
            continue;
        }

        src_gpu = uvm_va_space_find_gpu_with_memory_node_id(uvm_migrate_args->va_space, src_nid);

        // Already resident on a node with no CPUs that doesn't belong to a
        // GPU, don't move
        if (UVM_ID_IS_CPU(dst_id) && !src_gpu) {
            __set_bit(i, state->dst_resident_pages_mask);
            continue;
        }

        // TODO: Bug 2449272: Implement non-P2P copies. All systems that hit
        // this path have P2P copy support between all GPUs in the system, but
        // it could change in the future.

        if (src_gpu)
            src_id = src_gpu->id;
        else
            src_id = UVM_ID_CPU;

        if (!uvm_processor_mask_test_and_set(&state->src_processors, src_id))
            bitmap_zero(state->processors[uvm_id_value(src_id)].page_mask, state->num_pages);

        __set_bit(i, state->processors[uvm_id_value(src_id)].page_mask);
    }
}

static struct page *migrate_vma_alloc_page(migrate_vma_state_t *state)
{
    struct page *dst_page;
    uvm_migrate_args_t *uvm_migrate_args = state->uvm_migrate_args;
    uvm_va_space_t *va_space = uvm_migrate_args->va_space;

    if (uvm_enable_builtin_tests && atomic_dec_if_positive(&va_space->test.migrate_vma_allocation_fail_nth) == 0) {
        dst_page = NULL;
    }
    else {
        dst_page = alloc_pages_node(uvm_migrate_args->dst_node_id, g_migrate_vma_gfp_flags, 0);

        // TODO: Bug 2399573: Linux commit
        // 183f6371aac2a5496a8ef2b0b0a68562652c3cdb introduced a bug that makes
        // __GFP_THISNODE not always be honored (this was later fixed in commit
        // 7810e6781e0fcbca78b91cf65053f895bf59e85f). Therefore, we verify
        // whether the flag has been honored and abort the allocation,
        // otherwise. Remove this check when the fix is deployed on all
        // production systems.
        if (dst_page && page_to_nid(dst_page) != uvm_migrate_args->dst_node_id) {
            __free_page(dst_page);
            dst_page = NULL;
        }
    }

    return dst_page;
}

static NV_STATUS migrate_vma_populate_anon_pages(struct vm_area_struct *vma,
                                                 unsigned long *dst,
                                                 unsigned long start,
                                                 unsigned long outer,
                                                 migrate_vma_state_t *state)
{
    NV_STATUS status = NV_OK;
    uvm_migrate_args_t *uvm_migrate_args = state->uvm_migrate_args;
    uvm_processor_id_t dst_id = uvm_migrate_args->dst_id;
    unsigned long *page_mask = state->processors[uvm_id_value(dst_id)].page_mask;
    uvm_gpu_t *copying_gpu = NULL;
    uvm_va_space_t *va_space = uvm_migrate_args->va_space;
    uvm_push_t push;
    unsigned long i;

    // Nothing to do
    if (state->num_populate_anon_pages == 0)
        return NV_OK;

    UVM_ASSERT(state->num_populate_anon_pages == bitmap_weight(page_mask, state->num_pages));

    for_each_set_bit(i, page_mask, state->num_pages) {
        uvm_gpu_address_t dst_address;
        struct page *dst_page;

        dst_page = migrate_vma_alloc_page(state);
        if (!dst_page) {
            __set_bit(i, state->allocation_failed_mask);
            continue;
        }

        if (!copying_gpu) {
            // Try to get a GPU attached to the node being populated. If there
            // is none, use any of the GPUs registered in the VA space.
            if (UVM_ID_IS_CPU(dst_id)) {
                copying_gpu = uvm_va_space_find_first_gpu_attached_to_cpu_node(va_space, uvm_migrate_args->dst_node_id);
                if (!copying_gpu)
                    copying_gpu = uvm_va_space_find_first_gpu(va_space);
            }
            else {
                copying_gpu = uvm_va_space_get_gpu(va_space, dst_id);
            }

            UVM_ASSERT(copying_gpu);

            status = migrate_vma_zero_begin_push(va_space, dst_id, copying_gpu, start, outer - 1, &push);
            if (status != NV_OK) {
                __free_page(dst_page);
                return status;
            }
        }
        else {
            uvm_push_set_flag(&push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED);
        }

        status = migrate_vma_page_copy_address(dst_page, i, dst_id, copying_gpu, state, &dst_address);
        if (status != NV_OK) {
            __free_page(dst_page);
            break;
        }

        lock_page(dst_page);

        // We'll push one membar later for all memsets in this loop
        uvm_push_set_flag(&push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);
        copying_gpu->parent->ce_hal->memset_8(&push, dst_address, 0, PAGE_SIZE);

        dst[i] = migrate_pfn(page_to_pfn(dst_page));
    }

    if (copying_gpu) {
        NV_STATUS tracker_status;

        uvm_push_end(&push);

        tracker_status = uvm_tracker_add_push_safe(&state->tracker, &push);
        if (status == NV_OK)
            status = tracker_status;
    }

    return status;
}

static NV_STATUS migrate_vma_copy_pages_from(struct vm_area_struct *vma,
                                             const unsigned long *src,
                                             unsigned long *dst,
                                             unsigned long start,
                                             unsigned long outer,
                                             uvm_processor_id_t src_id,
                                             migrate_vma_state_t *state)
{
    NV_STATUS status = NV_OK;
    uvm_push_t push;
    unsigned long i;
    uvm_gpu_t *copying_gpu = NULL;
    uvm_migrate_args_t *uvm_migrate_args = state->uvm_migrate_args;
    uvm_processor_id_t dst_id = uvm_migrate_args->dst_id;
    unsigned long *page_mask = state->processors[uvm_id_value(src_id)].page_mask;
    uvm_va_space_t *va_space = uvm_migrate_args->va_space;

    UVM_ASSERT(!bitmap_empty(page_mask, state->num_pages));

    for_each_set_bit(i, page_mask, state->num_pages) {
        uvm_gpu_address_t src_address;
        uvm_gpu_address_t dst_address;
        struct page *src_page = migrate_pfn_to_page(src[i]);
        struct page *dst_page;

        UVM_ASSERT(src[i] & MIGRATE_PFN_VALID);
        UVM_ASSERT(src_page);

        dst_page = migrate_vma_alloc_page(state);
        if (!dst_page) {
            __set_bit(i, state->allocation_failed_mask);
            continue;
        }

        if (!copying_gpu) {
            status = migrate_vma_copy_begin_push(va_space, dst_id, src_id, start, outer - 1, &push);
            if (status != NV_OK) {
                __free_page(dst_page);
                return status;
            }

            copying_gpu = uvm_push_get_gpu(&push);
        }
        else {
            uvm_push_set_flag(&push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED);
        }

        // We don't have a case where both src and dst use the SYS aperture, so
        // the second call can't overwrite a dma addr set up by the first call.
        status = migrate_vma_page_copy_address(src_page, i, src_id, copying_gpu, state, &src_address);
        if (status == NV_OK)
            status = migrate_vma_page_copy_address(dst_page, i, dst_id, copying_gpu, state, &dst_address);

        if (status != NV_OK) {
            __free_page(dst_page);
            break;
        }

        lock_page(dst_page);

        // We'll push one membar later for all copies in this loop
        uvm_push_set_flag(&push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);
        copying_gpu->parent->ce_hal->memcopy(&push, dst_address, src_address, PAGE_SIZE);

        dst[i] = migrate_pfn(page_to_pfn(dst_page));
    }

    // TODO: Bug 1766424: If the destination is a GPU and the copy was done by
    //       that GPU, use a GPU-local membar if no peer nor the CPU can
    //       currently map this page. When peer access gets enabled, do a
    //       MEMBAR_SYS at that point.
    if (copying_gpu) {
        NV_STATUS tracker_status;

        uvm_push_end(&push);

        tracker_status = uvm_tracker_add_push_safe(&state->tracker, &push);
        if (status == NV_OK)
            status = tracker_status;
    }

    return status;
}

static NV_STATUS migrate_vma_copy_pages(struct vm_area_struct *vma,
                                        const unsigned long *src,
                                        unsigned long *dst,
                                        unsigned long start,
                                        unsigned long outer,
                                        migrate_vma_state_t *state)
{
    uvm_processor_id_t src_id;

    for_each_id_in_mask(src_id, &state->src_processors) {
        NV_STATUS status = migrate_vma_copy_pages_from(vma, src, dst, start, outer, src_id, state);
        if (status != NV_OK)
            return status;
    }

    return NV_OK;
}

static void migrate_vma_cleanup_pages(unsigned long *dst, unsigned long npages)
{
    unsigned long i;

    for (i = 0; i < npages; i++) {
        struct page *dst_page = migrate_pfn_to_page(dst[i]);

        if (!dst_page)
            continue;

        unlock_page(dst_page);
        __free_page(dst_page);
        dst[i] = 0;
    }
}

static void migrate_vma_alloc_and_copy(struct migrate_vma *args, migrate_vma_state_t *state)
{
    struct vm_area_struct *vma = args->vma;
    unsigned long start = args->start;
    unsigned long outer = args->end;
    NV_STATUS tracker_status;

    uvm_tracker_init(&state->tracker);

    state->num_pages = (outer - start) / PAGE_SIZE;
    state->status = NV_OK;

    migrate_vma_compute_masks(vma, args->src, state);

    state->status = migrate_vma_populate_anon_pages(vma, args->dst, start, outer, state);

    if (state->status == NV_OK)
        state->status = migrate_vma_copy_pages(vma, args->src, args->dst, start, outer, state);

    // Wait for tracker since all copies must have completed before returning
    tracker_status = uvm_tracker_wait_deinit(&state->tracker);

    if (state->status == NV_OK)
        state->status = tracker_status;

    // Mark all pages as not migrating if we're failing
    if (state->status != NV_OK)
        migrate_vma_cleanup_pages(args->dst, state->num_pages);
}

#if defined(CONFIG_MIGRATE_VMA_HELPER)
static void migrate_vma_alloc_and_copy_helper(struct vm_area_struct *vma,
                                              const unsigned long *src,
                                              unsigned long *dst,
                                              unsigned long start,
                                              unsigned long end,
                                              void *private)
{
    struct migrate_vma args =
    {
        .vma = vma,
        .dst = dst,
        .src = (unsigned long *) src,
        .start = start,
        .end = end,
    };

    migrate_vma_alloc_and_copy(&args, (migrate_vma_state_t *) private);
}
#endif

static void uvm_migrate_vma_finalize_and_map(struct migrate_vma *args, migrate_vma_state_t *state)
{
    unsigned long i;

    for (i = 0; i < state->num_pages; i++) {
        bool needs_touch = false;
        uvm_migrate_args_t *uvm_migrate_args = state->uvm_migrate_args;

        // The page was successfully migrated.
        if (args->src[i] & MIGRATE_PFN_MIGRATE) {
            // Touch if requested since population of these pages won't be tried
            // later.
            needs_touch = true;
        }
        else {
            // The page was not migrated. This can happen for two reasons.
            //
            // 1. Page is already resident at the destination.
            // 2. Page failed migration because the page state could not be
            // migrated by the kernel.
            //
            // So, only set the corresponding populate_pages bit if both the
            // following conditions are true.
            //
            // 1.Trying to populate pages (with gup) which are already resident
            // at the destination is wasteful but usually harmless except in the
            // PROT_NONE case. gup returns NV_ERR_INVALID_ADDRESS for such pages
            // and will incorrectly lead to API migration failures even though
            // migration worked as expected.
            //
            // 2. Migration failure was not because of allocation failure in
            // uvm_migrate_vma_finalize_and_map() since such failures would be
            // indicated in allocation_failed_mask. Failures other than
            // allocation failures likely means that the page is populated
            // somewhere. So, set the corresponding bit in populate_pages_mask.
            if (test_bit(i, state->dst_resident_pages_mask)) {

                // If touch was requested, pages in allocation_failed and
                // populate_pages masks will be touched during population. But pages
                // which are already resident at the destination need to be touched
                // here since population isn't tried later for such pages.
                needs_touch = true;
            }
            else if (!test_bit(i, state->allocation_failed_mask)) {
                __set_bit(i, state->populate_pages_mask);
            }
        }

        // Touch if requested and needed.
        if (uvm_migrate_args->touch && needs_touch) {
            struct page *dst_page;

            UVM_ASSERT(args->dst[i] & MIGRATE_PFN_VALID);

            dst_page = migrate_pfn_to_page(args->dst[i]);
            UVM_ASSERT(dst_page);
            uvm_touch_page(dst_page);
        }
    }

    // Remove the IOMMU mappings created during the copy
    if (state->dma.num_pages > 0) {

        for_each_set_bit(i, state->dma.page_mask, state->num_pages)
            uvm_parent_gpu_unmap_cpu_page(state->dma.addrs_gpus[i]->parent, state->dma.addrs[i]);
    }

    UVM_ASSERT(!bitmap_intersects(state->populate_pages_mask, state->allocation_failed_mask, state->num_pages));
}

#if defined(CONFIG_MIGRATE_VMA_HELPER)
static void migrate_vma_finalize_and_map_helper(struct vm_area_struct *vma,
                                                const unsigned long *src,
                                                const unsigned long *dst,
                                                unsigned long start,
                                                unsigned long end,
                                                void *private)
{
    struct migrate_vma args =
    {
        .vma = vma,
        .dst = (unsigned long *) dst,
        .src = (unsigned long *) src,
        .start = start,
        .end = end,
    };

    uvm_migrate_vma_finalize_and_map(&args, (migrate_vma_state_t *) private);
}
#endif

static NV_STATUS nv_migrate_vma(struct migrate_vma *args, migrate_vma_state_t *state)
{
    int ret;

#if defined(CONFIG_MIGRATE_VMA_HELPER)
    static const struct migrate_vma_ops uvm_migrate_vma_ops =
    {
        .alloc_and_copy = migrate_vma_alloc_and_copy_helper,
        .finalize_and_map = migrate_vma_finalize_and_map_helper,
    };

    ret = migrate_vma(&uvm_migrate_vma_ops, args->vma, args->start, args->end, args->src, args->dst, state);
    if (ret < 0)
        return errno_to_nv_status(ret);
#else // CONFIG_MIGRATE_VMA_HELPER

#if defined(NV_MIGRATE_VMA_FLAGS_PRESENT)
    args->flags = MIGRATE_VMA_SELECT_SYSTEM;
#endif // NV_MIGRATE_VMA_FLAGS_PRESENT

    ret = migrate_vma_setup(args);
    if (ret < 0)
        return errno_to_nv_status(ret);

    migrate_vma_alloc_and_copy(args, state);
    if (state->status == NV_OK) {
        migrate_vma_pages(args);
        uvm_migrate_vma_finalize_and_map(args, state);
    }

    migrate_vma_finalize(args);
#endif // CONFIG_MIGRATE_VMA_HELPER

    return state->status;
}

static NV_STATUS migrate_pageable_vma_populate_mask(struct vm_area_struct *vma,
                                                    unsigned long start,
                                                    unsigned long outer,
                                                    const unsigned long *mask,
                                                    migrate_vma_state_t *state)
{
    const unsigned long num_pages = (outer - start) / PAGE_SIZE;
    unsigned long subregion_first = find_first_bit(mask, num_pages);
    uvm_migrate_args_t *uvm_migrate_args = state->uvm_migrate_args;

    while (subregion_first < num_pages) {
        NV_STATUS status;
        unsigned long subregion_outer = find_next_zero_bit(mask, num_pages, subregion_first + 1);

        status = uvm_populate_pageable_vma(vma,
                                           start + subregion_first * PAGE_SIZE,
                                           (subregion_outer - subregion_first) * PAGE_SIZE,
                                           0,
                                           uvm_migrate_args->touch,
                                           uvm_migrate_args->populate_permissions);
        if (status != NV_OK)
            return status;

        subregion_first = find_next_bit(mask, num_pages, subregion_outer + 1);
    }

    return NV_OK;
}

static NV_STATUS migrate_pageable_vma_migrate_mask(struct vm_area_struct *vma,
                                                   unsigned long start,
                                                   unsigned long outer,
                                                   const unsigned long *mask,
                                                   migrate_vma_state_t *state)
{
    NV_STATUS status;
    const unsigned long num_pages = (outer - start) / PAGE_SIZE;
    unsigned long subregion_first = find_first_bit(mask, num_pages);
    uvm_migrate_args_t *uvm_migrate_args = state->uvm_migrate_args;
    struct migrate_vma args =
    {
        .vma = vma,
        .src = state->src_pfn_array,
        .dst = state->dst_pfn_array,
    };

    UVM_ASSERT(!uvm_migrate_args->skip_mapped);

    while (subregion_first < num_pages) {
        unsigned long subregion_outer = find_next_zero_bit(mask, num_pages, subregion_first + 1);

        args.start = start + subregion_first * PAGE_SIZE;
        args.end = start + subregion_outer * PAGE_SIZE;

        status = nv_migrate_vma(&args, state);
        if (status != NV_OK)
            return status;

        // We ignore allocation failure here as we are just retrying migration,
        // but pages must have already been populated by the caller

        subregion_first = find_next_bit(mask, num_pages, subregion_outer + 1);
    }

    return NV_OK;
}

static NV_STATUS migrate_pageable_vma_region(struct vm_area_struct *vma,
                                             unsigned long start,
                                             unsigned long outer,
                                             migrate_vma_state_t *state,
                                             unsigned long *next_addr)
{
    NV_STATUS status;
    const unsigned long num_pages = (outer - start) / PAGE_SIZE;
    struct mm_struct *mm = vma->vm_mm;
    uvm_migrate_args_t *uvm_migrate_args = state->uvm_migrate_args;
    struct migrate_vma args =
    {
        .vma = vma,
        .src = state->src_pfn_array,
        .dst = state->dst_pfn_array,
        .start = start,
        .end = outer,
    };

    UVM_ASSERT(PAGE_ALIGNED(start));
    UVM_ASSERT(PAGE_ALIGNED(outer));
    UVM_ASSERT(start < outer);
    UVM_ASSERT(start >= vma->vm_start);
    UVM_ASSERT(outer <= vma->vm_end);
    UVM_ASSERT(outer - start <= UVM_MIGRATE_VMA_MAX_SIZE);
    uvm_assert_mmap_lock_locked(mm);
    uvm_assert_rwsem_locked(&uvm_migrate_args->va_space->lock);

    status = nv_migrate_vma(&args, state);
    if (status != NV_OK)
        return status;

    // Save the returned page masks because they can be overwritten by
    // migrate_pageable_vma_migrate_mask().
    bitmap_copy(state->scratch1_mask, state->populate_pages_mask, num_pages);
    bitmap_copy(state->scratch2_mask, state->allocation_failed_mask, num_pages);

    if (!bitmap_empty(state->scratch1_mask, state->num_pages)) {
        // Populate pages using get_user_pages
        status = migrate_pageable_vma_populate_mask(vma, start, outer, state->scratch1_mask, state);
        if (status != NV_OK)
            return status;

        if (!uvm_migrate_args->skip_mapped) {
            status = migrate_pageable_vma_migrate_mask(vma, start, outer, state->scratch1_mask, state);
            if (status != NV_OK)
                return status;
        }
    }

    // There is no need to copy the masks again after the migration is retried.
    // We ignore the allocation_failed, populate_pages and dst_resident_pages
    // masks set by the retried migration.

    if (!bitmap_empty(state->scratch2_mask, state->num_pages)) {
        // If the destination is the CPU, signal user-space to retry with a
        // different node. Otherwise, just try to populate anywhere in the
        // system
        if (UVM_ID_IS_CPU(uvm_migrate_args->dst_id) && !uvm_migrate_args->populate_on_cpu_alloc_failures) {
            *next_addr = start + find_first_bit(state->scratch2_mask, num_pages) * PAGE_SIZE;
            return NV_ERR_MORE_PROCESSING_REQUIRED;
        }
        else {
            status = migrate_pageable_vma_populate_mask(vma, start, outer, state->scratch2_mask, state);
            if (status != NV_OK)
                return status;
        }
    }

    return NV_OK;
}

NV_STATUS uvm_test_skip_migrate_vma(UVM_TEST_SKIP_MIGRATE_VMA_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    uvm_va_space_down_write(va_space);
    va_space->test.skip_migrate_vma = params->skip;
    uvm_va_space_up_write(va_space);

    return NV_OK;
}

static NV_STATUS migrate_pageable_vma(struct vm_area_struct *vma,
                                      unsigned long start,
                                      unsigned long outer,
                                      migrate_vma_state_t *state,
                                      unsigned long *next_addr)
{
    NV_STATUS status = NV_OK;
    struct mm_struct *mm = vma->vm_mm;
    uvm_migrate_args_t *uvm_migrate_args = state->uvm_migrate_args;
    uvm_va_space_t *va_space = uvm_migrate_args->va_space;

    UVM_ASSERT(PAGE_ALIGNED(start));
    UVM_ASSERT(PAGE_ALIGNED(outer));
    UVM_ASSERT(vma->vm_end > start);
    UVM_ASSERT(vma->vm_start < outer);
    uvm_assert_mmap_lock_locked(mm);
    uvm_assert_rwsem_locked(&va_space->lock);

    // Adjust to input range boundaries
    start = max(start, vma->vm_start);
    outer = min(outer, vma->vm_end);

    if (va_space->test.skip_migrate_vma)
        return NV_WARN_NOTHING_TO_DO;

    // TODO: Bug 2419180: support file-backed pages in migrate_vma, when
    //       support for it is added to the Linux kernel
    if (!vma_is_anonymous(vma))
        return NV_WARN_NOTHING_TO_DO;

    if (uvm_processor_mask_empty(&va_space->registered_gpus))
        return NV_WARN_NOTHING_TO_DO;

    while (start < outer) {
        const size_t region_size = min(outer - start, UVM_MIGRATE_VMA_MAX_SIZE);

        status = migrate_pageable_vma_region(vma, start, start + region_size, state, next_addr);
        if (status == NV_ERR_MORE_PROCESSING_REQUIRED) {
            UVM_ASSERT(*next_addr >= start);
            UVM_ASSERT(*next_addr < outer);
        }

        if (status != NV_OK)
            break;

        start += region_size;
    };

    return status;
}

static NV_STATUS migrate_pageable(migrate_vma_state_t *state)
{
    uvm_migrate_args_t *uvm_migrate_args = state->uvm_migrate_args;
    uvm_va_space_t *va_space = uvm_migrate_args->va_space;
    const unsigned long length = uvm_migrate_args->length;
    NvU64 *user_space_start = uvm_migrate_args->user_space_start;
    NvU64 *user_space_length = uvm_migrate_args->user_space_length;
    struct mm_struct *mm = uvm_migrate_args->mm;
    unsigned long start = uvm_migrate_args->start;
    unsigned long outer = start + length;
    unsigned long prev_outer = outer;
    struct vm_area_struct *vma;

    UVM_ASSERT(PAGE_ALIGNED(start));
    UVM_ASSERT(PAGE_ALIGNED(length));
    uvm_assert_mmap_lock_locked(mm);

    vma = find_vma_intersection(mm, start, outer);
    if (!vma || (start < vma->vm_start))
        return NV_ERR_INVALID_ADDRESS;

    // VMAs are validated and migrated one at a time, since migrate_vma works
    // on one vma at a time
    for (; vma->vm_start <= prev_outer; vma = find_vma_intersection(mm, prev_outer, outer)) {
        unsigned long next_addr = 0;
        NV_STATUS status;

        // Callers have already validated the range so the vma should be valid.
        UVM_ASSERT(vma);

        status = migrate_pageable_vma(vma, start, outer, state, &next_addr);
        if (status == NV_WARN_NOTHING_TO_DO) {
            NV_STATUS populate_status = NV_OK;
            bool touch = uvm_migrate_args->touch;
            uvm_populate_permissions_t populate_permissions = uvm_migrate_args->populate_permissions;

            UVM_ASSERT(va_space->test.skip_migrate_vma ||
                       !vma_is_anonymous(vma) ||
                       uvm_processor_mask_empty(&va_space->registered_gpus));

            // We can't use migrate_vma to move the pages as desired. Normally
            // this fallback path is supposed to populate the memory then inform
            // user mode that it should call move_pages, but that move_pages
            // call won't work as expected if the caller is in the wrong
            // process. Make that failure explicit so the caller is aware that
            // move_pages won't behave as expected.
            //
            // If the caller is a kernel thread, such as the GPU BH, continue
            // with population since there's no move_pages fallback.
            if (current->mm != mm && !(current->flags & PF_KTHREAD))
                return NV_ERR_NOT_SUPPORTED;

            // Populate pages with uvm_populate_pageable
            populate_status = uvm_populate_pageable_vma(vma, start, length, 0, touch, populate_permissions);
            if (populate_status == NV_OK) {
                *user_space_start = max(vma->vm_start, start);
                *user_space_length = min(vma->vm_end, outer) - *user_space_start;
            }
            else {
                status = populate_status;
            }
        }
        else if (status == NV_ERR_MORE_PROCESSING_REQUIRED) {
            UVM_ASSERT(next_addr >= start);
            UVM_ASSERT(next_addr < outer);
            UVM_ASSERT(UVM_ID_IS_CPU(uvm_migrate_args->dst_id));

            *user_space_start = next_addr;
        }

        if (status != NV_OK)
            return status;

        if (vma->vm_end >= outer)
            return NV_OK;

        prev_outer = vma->vm_end;
    }

    // Input range not fully covered by VMAs.
    return NV_ERR_INVALID_ADDRESS;
}

NV_STATUS uvm_migrate_pageable(uvm_migrate_args_t *uvm_migrate_args)
{
    migrate_vma_state_t *state = NULL;
    NV_STATUS status;
    uvm_va_space_t *va_space = uvm_migrate_args->va_space;
    uvm_processor_id_t dst_id = uvm_migrate_args->dst_id;

    UVM_ASSERT(PAGE_ALIGNED(uvm_migrate_args->start));
    UVM_ASSERT(PAGE_ALIGNED(uvm_migrate_args->length));
    uvm_assert_mmap_lock_locked(uvm_migrate_args->mm);

    if (UVM_ID_IS_CPU(dst_id)) {
        if (uvm_migrate_args->dst_node_id == -1)
            return NV_ERR_INVALID_ARGUMENT;
    }
    else {
        // Incoming dst_node_id is only valid if dst_id belongs to the CPU. Use
        // dst_node_id as the GPU node id if dst_id doesn't belong to the CPU.
        uvm_migrate_args->dst_node_id = uvm_gpu_numa_node(uvm_va_space_get_gpu(va_space, dst_id));
    }

    state = kmem_cache_alloc(g_uvm_migrate_vma_state_cache, NV_UVM_GFP_FLAGS);
    if (!state)
        return NV_ERR_NO_MEMORY;

    state->uvm_migrate_args = uvm_migrate_args;
    status = migrate_pageable(state);

    kmem_cache_free(g_uvm_migrate_vma_state_cache, state);

    return status;
}

NV_STATUS uvm_migrate_pageable_init(void)
{
    g_uvm_migrate_vma_state_cache = NV_KMEM_CACHE_CREATE("migrate_vma_state_t", migrate_vma_state_t);
    if (!g_uvm_migrate_vma_state_cache)
        return NV_ERR_NO_MEMORY;

    return NV_OK;
}

void uvm_migrate_pageable_exit(void)
{
    kmem_cache_destroy_safe(&g_uvm_migrate_vma_state_cache);
}
#endif
