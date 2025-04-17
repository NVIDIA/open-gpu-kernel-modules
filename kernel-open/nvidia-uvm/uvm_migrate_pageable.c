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
#include "uvm_forward_decl.h"
#include "uvm_hal_types.h"
#include "uvm_linux.h"
#include "uvm_gpu.h"
#include "uvm_lock.h"
#include "uvm_processors.h"
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

static bool uvm_dma_mapping_required_on_copying_gpu(const uvm_va_space_t *va_space,
                                                    uvm_processor_id_t resident_id,
                                                    const uvm_gpu_t *copying_gpu)
{
    uvm_gpu_t *owning_gpu = UVM_ID_IS_CPU(resident_id) ? NULL : uvm_gpu_get(resident_id);

    // Local vidmem
    if (owning_gpu == copying_gpu)
        return false;

    // Peer GPU vidmem
    if (owning_gpu && uvm_processor_mask_test(&va_space->can_copy_from[uvm_id_value(copying_gpu->id)], resident_id))
        return false;

    return true;
}

// Compute the address needed for copying_gpu to access the given page,
// mapped at the address provided by dma_addr.
static uvm_gpu_address_t uvm_migrate_vma_dma_page_copy_address(uvm_gpu_t *copying_gpu, dma_addr_t dma_addr)
{
    NvU64 gpu_dma_addr = uvm_parent_gpu_dma_addr_to_gpu_addr(copying_gpu->parent, dma_addr);

    return uvm_gpu_address_copy(copying_gpu, uvm_gpu_phys_address(UVM_APERTURE_SYS, gpu_dma_addr));
}

// Compute the address needed for copying_gpu to access the given page,
// resident on resident_id when DMA mappings are not required.
static uvm_gpu_address_t uvm_migrate_vma_page_copy_address(struct page *page,
                                                           unsigned long page_index,
                                                           uvm_processor_id_t resident_id,
                                                           uvm_gpu_t *copying_gpu,
                                                           migrate_vma_state_t *state)
{
    uvm_va_space_t *va_space = state->uvm_migrate_args->va_space;
    uvm_gpu_t *owning_gpu = uvm_gpu_get(resident_id);
    uvm_gpu_phys_address_t phys_addr = uvm_gpu_page_to_phys_address(owning_gpu, page);

    UVM_ASSERT(page_index < state->num_pages);
    UVM_ASSERT(owning_gpu);

    if (owning_gpu == copying_gpu) {
        // Local vidmem address
        return uvm_gpu_address_copy(owning_gpu, phys_addr);
    }
    else {
        // Peer vidmem address
        UVM_ASSERT(uvm_processor_mask_test(&va_space->can_copy_from[uvm_id_value(copying_gpu->id)], resident_id));
        return uvm_gpu_peer_copy_address(owning_gpu, phys_addr.address, copying_gpu);
    }
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
                          uvm_processor_get_name(dst_id),
                          uvm_processor_get_name(gpu->id),
                          start,
                          outer);
}

// Create a new push to copy pages between src_id and dst_id
static NV_STATUS migrate_vma_copy_begin_push(uvm_va_space_t *va_space,
                                             uvm_gpu_t *copying_gpu,
                                             uvm_channel_type_t channel_type,
                                             uvm_processor_id_t dst_id,
                                             uvm_processor_id_t src_id,
                                             unsigned long start,
                                             unsigned long outer,
                                             uvm_push_t *push)
{
    UVM_ASSERT_MSG(!uvm_id_equal(src_id, dst_id),
                   "Unexpected copy to self, processor %s\n",
                   uvm_processor_get_name(src_id));

    // NUMA-enabled GPUs can copy to any other NUMA node in the system even if
    // P2P access has not been explicitly enabled (ie va_space->can_copy_from
    // is not set).
    if (!copying_gpu->mem_info.numa.enabled) {
        UVM_ASSERT_MSG(uvm_processor_mask_test(&va_space->can_copy_from[uvm_id_value(copying_gpu->id)], dst_id),
                       "GPU %s dst %s src %s\n",
                       uvm_processor_get_name(copying_gpu->id),
                       uvm_processor_get_name(dst_id),
                       uvm_processor_get_name(src_id));
        UVM_ASSERT_MSG(uvm_processor_mask_test(&va_space->can_copy_from[uvm_id_value(copying_gpu->id)], src_id),
                       "GPU %s dst %s src %s\n",
                       uvm_processor_get_name(copying_gpu->id),
                       uvm_processor_get_name(dst_id),
                       uvm_processor_get_name(src_id));
    }

    if (channel_type == UVM_CHANNEL_TYPE_GPU_TO_GPU) {
        uvm_gpu_t *dst_gpu = uvm_gpu_get(dst_id);
        return uvm_push_begin_gpu_to_gpu(copying_gpu->channel_manager,
                                         dst_gpu,
                                         push,
                                         "Copy from %s to %s for VMA region [0x%lx, 0x%lx]",
                                         uvm_processor_get_name(src_id),
                                         uvm_processor_get_name(dst_id),
                                         start,
                                         outer);
    }

    return uvm_push_begin(copying_gpu->channel_manager,
                          channel_type,
                          push,
                          "Copy from %s to %s for VMA region [0x%lx, 0x%lx]",
                          uvm_processor_get_name(src_id),
                          uvm_processor_get_name(dst_id),
                          start,
                          outer);
}

static void uvm_migrate_vma_state_compute_masks(struct vm_area_struct *vma,
                                                const unsigned long *src,
                                                migrate_vma_state_t *state)
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

static struct page *uvm_migrate_vma_alloc_page(migrate_vma_state_t *state)
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

static void zero_dma_mapped_pages(uvm_push_t *push,
                                  migrate_vma_state_t *state)
{
    struct sg_dma_page_iter dma_iter;
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    struct sg_table *sgt = &state->dma.sgt_anon;

    for_each_sgtable_dma_page(sgt, &dma_iter, 0) {
        dma_addr_t dma_addr = uvm_sg_page_iter_dma_address(&dma_iter);
        uvm_gpu_address_t dst_address = uvm_migrate_vma_dma_page_copy_address(gpu, dma_addr);

        uvm_push_set_flag(push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED);
        uvm_push_set_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);
        gpu->parent->ce_hal->memset_8(push, dst_address, 0, PAGE_SIZE);
    }
}

static void zero_non_failed_pages_in_mask(uvm_push_t *push,
                                          const unsigned long *pfns,
                                          unsigned long *page_mask,
                                          unsigned long mask_size,
                                          migrate_vma_state_t *state)
{
    unsigned long i;
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    uvm_migrate_args_t *uvm_migrate_args = state->uvm_migrate_args;
    uvm_processor_id_t dst_id = uvm_migrate_args->dst_id;
    const unsigned long *alloc_failed_mask = state->allocation_failed_mask;

    for_each_set_bit(i, page_mask, mask_size) {
        struct page *page;
        uvm_gpu_address_t dst_address;

        if (test_bit(i, alloc_failed_mask))
            continue;

        page = migrate_pfn_to_page(pfns[i]);
        dst_address = uvm_migrate_vma_page_copy_address(page, i, dst_id, gpu, state);

        uvm_push_set_flag(push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED);
        uvm_push_set_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);
        gpu->parent->ce_hal->memset_8(push, dst_address, 0, PAGE_SIZE);
    }
}

static NV_STATUS dma_map_non_failed_pages_in_mask(uvm_gpu_t *gpu,
                                                  struct sg_table *sgt,
                                                  const unsigned long *pfns,
                                                  const unsigned long *page_mask,
                                                  unsigned long mask_size,
                                                  migrate_vma_state_t *state)
{
    int sg_nent;
    unsigned long i;
    unsigned long page_count = 0;
    struct scatterlist *sg = sgt->sgl;
    const unsigned long *alloc_failed_mask = state->allocation_failed_mask;

    UVM_ASSERT(!bitmap_empty(page_mask, mask_size));

    for_each_set_bit(i, page_mask, mask_size) {
        struct page *page;

        if (test_bit(i, alloc_failed_mask))
            continue;

        page = migrate_pfn_to_page(pfns[i]);

        sg_set_page(sg, page, PAGE_SIZE, 0);
        sg = sg_next(sg);

        if (!sg && i != mask_size - 1)
            return NV_ERR_INVALID_STATE;

        page_count++;
    }

    if (page_count < sgt->orig_nents)
        sg_mark_end(sg);

    if (page_count) {
        sg_nent = dma_map_sg(&gpu->parent->pci_dev->dev,
                             sgt->sgl,
                             page_count,
                             DMA_BIDIRECTIONAL);

        sgt->nents = sg_nent;

        if (sg_nent == 0)
            return NV_ERR_NO_MEMORY;
    }

    state->dma.num_pages = page_count;
    return NV_OK;
}

static NV_STATUS uvm_migrate_vma_populate_anon_pages(struct vm_area_struct *vma,
                                                     unsigned long *dst,
                                                     unsigned long start,
                                                     unsigned long outer,
                                                     migrate_vma_state_t *state)
{
    uvm_push_t push;
    unsigned long i;
    uvm_gpu_t *copying_gpu;
    NV_STATUS status = NV_OK;
    uvm_migrate_args_t *uvm_migrate_args = state->uvm_migrate_args;
    uvm_processor_id_t dst_id = uvm_migrate_args->dst_id;
    uvm_va_space_t *va_space = uvm_migrate_args->va_space;
    unsigned long *page_mask = state->processors[uvm_id_value(dst_id)].page_mask;

    // Nothing to do
    if (state->num_populate_anon_pages == 0)
        return NV_OK;

    UVM_ASSERT(state->num_populate_anon_pages == bitmap_weight(page_mask, state->num_pages));

    // Try to get a GPU attached to the node being populated. If there
    // is none, use any of the GPUs registered in the VA space.
    if (UVM_ID_IS_CPU(dst_id)) {
        copying_gpu = uvm_va_space_find_first_gpu_attached_to_cpu_node(va_space, uvm_migrate_args->dst_node_id);
        if (!copying_gpu)
            copying_gpu = uvm_va_space_find_first_gpu(va_space);
    }
    else {
        copying_gpu = uvm_gpu_get(dst_id);
    }

    UVM_ASSERT(copying_gpu);

    state->dma.num_pages = 0;

    // Pre-allocate the dst pages and mark the ones that failed
    for_each_set_bit(i, page_mask, state->num_pages) {
        struct page *dst_page = uvm_migrate_vma_alloc_page(state);
        if (!dst_page) {
            __set_bit(i, state->allocation_failed_mask);
            continue;
        }

        lock_page(dst_page);
        dst[i] = migrate_pfn(page_to_pfn(dst_page));
    }

    if (uvm_dma_mapping_required_on_copying_gpu(va_space, dst_id, copying_gpu)) {
        status = dma_map_non_failed_pages_in_mask(copying_gpu,
                                                  &state->dma.sgt_anon,
                                                  dst,
                                                  page_mask,
                                                  state->num_pages,
                                                  state);
    }

    if (status != NV_OK)
        return status;

    state->dma.sgt_anon_gpu = copying_gpu;

    status = migrate_vma_zero_begin_push(va_space, dst_id, copying_gpu, start, outer - 1, &push);
    if (status != NV_OK)
        return status;

    // DMA mappings were required
    if (state->dma.num_pages)
        zero_dma_mapped_pages(&push, state);
    else
        zero_non_failed_pages_in_mask(&push, dst, page_mask, state->num_pages, state);

    uvm_push_end(&push);

    return uvm_tracker_add_push_safe(&state->tracker, &push);
}

static int find_next_valid_page_index(const unsigned long *page_mask,
                                      const unsigned long *allocation_failed_mask,
                                      unsigned long i,
                                      unsigned long size)
{
    if (i < size && test_bit(i, page_mask) && !test_bit(i, allocation_failed_mask))
        return i;

    while (i < size) {
        i = find_next_bit(page_mask, size, i + 1);

        if (i >= size || !test_bit(i, allocation_failed_mask))
            break;
    }

    return i;
}

static void copy_dma_mapped_pages(uvm_push_t *push,
                                  const unsigned long *src,
                                  const unsigned long *dst,
                                  bool src_has_dma_mappings,
                                  const unsigned long *page_mask,
                                  uvm_processor_id_t src_id,
                                  migrate_vma_state_t *state)
{
    struct sg_dma_page_iter dma_iter;
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    unsigned long i = find_first_bit(page_mask, state->num_pages);
    uvm_migrate_args_t *uvm_migrate_args = state->uvm_migrate_args;
    struct sg_table *sgt = &state->dma.sgt_from[uvm_id_value(src_id)];
    const unsigned long *allocation_failed_mask = state->allocation_failed_mask;

    UVM_ASSERT(state->dma.num_pages);

    // Align first valid page to the first DMA mapped page.
    i = find_next_valid_page_index(page_mask, allocation_failed_mask, i, state->num_pages);

    // All pages failed allocation, nothing to do.
    if (unlikely(i >= state->num_pages))
        return;

    // We are able to reconstruct the relationship between an entry in the
    // scatterlist and a page in the page_mask only because the chosen
    // for_each_ helper is iterating on a PAGE_SIZE basis.
    // Typically, there is no guarantee on the size of the mapped scatterlist,
    // in other words, an entry could represent multiple pages because they
    // are contiguous.
    for_each_sgtable_dma_page(sgt, &dma_iter, 0) {
        uvm_gpu_address_t gpu_addr;
        struct page *page;
        dma_addr_t dma_addr = uvm_sg_page_iter_dma_address(&dma_iter);
        uvm_gpu_address_t gpu_dma_addr = uvm_migrate_vma_dma_page_copy_address(gpu, dma_addr);

        uvm_push_set_flag(push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED);
        uvm_push_set_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);

        if (src_has_dma_mappings) {
            uvm_processor_id_t dst_id = uvm_migrate_args->dst_id;

            UVM_ASSERT(UVM_ID_IS_GPU(dst_id));
            UVM_ASSERT(dst[i] & MIGRATE_PFN_VALID);
            page = migrate_pfn_to_page(dst[i]);
            gpu_addr = uvm_migrate_vma_page_copy_address(page, i, dst_id, gpu, state);
            gpu->parent->ce_hal->memcopy(push, gpu_addr, gpu_dma_addr, PAGE_SIZE);
        }
        else {
            UVM_ASSERT(UVM_ID_IS_GPU(src_id));
            UVM_ASSERT(src[i] & MIGRATE_PFN_VALID);
            page = migrate_pfn_to_page(src[i]);
            gpu_addr = uvm_migrate_vma_page_copy_address(page, i, src_id, gpu, state);
            gpu->parent->ce_hal->memcopy(push, gpu_dma_addr, gpu_addr, PAGE_SIZE);
        }

        // If one or more consecutive page allocation failed, re-alignment with
        // the sgtable is necessary.
        i = find_next_valid_page_index(page_mask, allocation_failed_mask, i + 1, state->num_pages);

        // We should always have enough valid pages for each sgtable entries
        UVM_ASSERT(i < state->num_pages);
    }
}

static void copy_pages_in_mask(uvm_push_t *push,
                               const unsigned long *src,
                               const unsigned long *dst,
                               const unsigned long *page_mask,
                               uvm_processor_id_t src_id,
                               migrate_vma_state_t *state)
{
    unsigned long i;
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    uvm_migrate_args_t *uvm_migrate_args = state->uvm_migrate_args;
    uvm_processor_id_t dst_id = uvm_migrate_args->dst_id;

    for_each_set_bit(i, page_mask, state->num_pages) {
        uvm_gpu_address_t src_address;
        uvm_gpu_address_t dst_address;
        struct page *src_page = migrate_pfn_to_page(src[i]);
        struct page *dst_page = migrate_pfn_to_page(dst[i]);

        UVM_ASSERT(src[i] & MIGRATE_PFN_VALID);
        UVM_ASSERT(src_page);

        if (test_bit(i, state->allocation_failed_mask))
            continue;

        UVM_ASSERT(dst[i] & MIGRATE_PFN_VALID);
        UVM_ASSERT(dst_page);

        src_address = uvm_migrate_vma_page_copy_address(src_page, i, src_id, gpu, state);
        dst_address = uvm_migrate_vma_page_copy_address(dst_page, i, dst_id, gpu, state);

        // We'll push one membar later for all copies in this loop
        uvm_push_set_flag(push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED);
        uvm_push_set_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);
        gpu->parent->ce_hal->memcopy(push, dst_address, src_address, PAGE_SIZE);
    }
}

static uvm_gpu_t *select_gpu_for_vma_copy_push(uvm_processor_id_t dst_id,
                                               uvm_processor_id_t src_id,
                                               uvm_channel_type_t *out_channel_type)
{
    uvm_channel_type_t channel_type;
    uvm_gpu_t *gpu;

    if (UVM_ID_IS_CPU(src_id)) {
        gpu = uvm_gpu_get(dst_id);
        channel_type = UVM_CHANNEL_TYPE_CPU_TO_GPU;
    } else if (UVM_ID_IS_CPU(dst_id)) {
        gpu = uvm_gpu_get(src_id);
        channel_type = UVM_CHANNEL_TYPE_GPU_TO_CPU;
    } else {
        // Prefer to "push" the data from the source for GPU to GPU copies
        gpu = uvm_gpu_get(src_id);
        channel_type = UVM_CHANNEL_TYPE_GPU_TO_GPU;
    }

    if (out_channel_type)
        *out_channel_type = channel_type;

    return gpu;
}

static NV_STATUS uvm_uvm_migrate_vma_copy_pages_from(struct vm_area_struct *vma,
                                                     const unsigned long *src,
                                                     unsigned long *dst,
                                                     unsigned long start,
                                                     unsigned long outer,
                                                     uvm_processor_id_t src_id,
                                                     migrate_vma_state_t *state)
{
    uvm_push_t push;
    unsigned long i;
    uvm_channel_type_t channel_type;
    NV_STATUS status = NV_OK;
    uvm_gpu_t *copying_gpu = NULL;
    bool src_has_dma_mappings = false;
    bool dst_has_dma_mappings = false;
    uvm_migrate_args_t *uvm_migrate_args = state->uvm_migrate_args;
    uvm_processor_id_t dst_id = uvm_migrate_args->dst_id;
    uvm_va_space_t *va_space = uvm_migrate_args->va_space;
    unsigned long *page_mask = state->processors[uvm_id_value(src_id)].page_mask;
    uvm_tracker_t zero_tracker = UVM_TRACKER_INIT();

    UVM_ASSERT(!bitmap_empty(page_mask, state->num_pages));

    copying_gpu = select_gpu_for_vma_copy_push(dst_id, src_id, &channel_type);

    state->dma.num_pages = 0;

    // Pre-allocate the dst pages and mark the ones that failed
    for_each_set_bit(i, page_mask, state->num_pages) {
        struct page *dst_page = uvm_migrate_vma_alloc_page(state);
        if (!dst_page) {
            __set_bit(i, state->allocation_failed_mask);
            continue;
        }

        lock_page(dst_page);
        dst[i] = migrate_pfn(page_to_pfn(dst_page));
    }

    // Zero destination pages in case of NVLINK copy that can hit STO or XC,
    // or in case of injected unresolved NVLINK error.
    // TODO: Bug 4922701: [uvm] Re-evaluate STO handling for ATS migrations
    //       This can be removed if the false-positive rate of STO
    //       fast-path is low enough to prefer failing the copy when an STO
    //       fast-path error is detected.
    if (UVM_ID_IS_GPU(src_id) &&
        UVM_ID_IS_GPU(dst_id) &&
        ((copying_gpu->nvlink_status.enabled &&
        (uvm_parent_gpu_peer_link_type(copying_gpu->parent, uvm_gpu_get(dst_id)->parent) >= UVM_GPU_LINK_NVLINK_5)) ||
        uvm_gpu_get_injected_nvlink_error(copying_gpu) == NV_WARN_MORE_PROCESSING_REQUIRED)) {

        uvm_gpu_t *dst_gpu = uvm_gpu_get(dst_id);
        uvm_push_t zero_push;

        UVM_ASSERT(uvm_id_equal(copying_gpu->id, src_id));
        status = migrate_vma_zero_begin_push(va_space, dst_id, dst_gpu, start, outer - 1, &zero_push);
        if (status != NV_OK)
            return status;

        zero_non_failed_pages_in_mask(&zero_push, dst, page_mask, state->num_pages, state);

        uvm_push_end(&zero_push);
        status = uvm_tracker_add_push_safe(&zero_tracker, &zero_push);
        if (status != NV_OK)
            return status;
    }

    // We don't have a case where both src and dst use the SYS aperture.
    // In other word, only one mapping for page index i is allowed.
    // In both cases, we're using the source processor scatterlist to host
    // the pages because we cannot reuse the destination scatterlist among
    // the different source processors.
    if (uvm_dma_mapping_required_on_copying_gpu(va_space, src_id, copying_gpu)) {
        status = dma_map_non_failed_pages_in_mask(copying_gpu,
                                                  &state->dma.sgt_from[uvm_id_value(src_id)],
                                                  src,
                                                  page_mask,
                                                  state->num_pages,
                                                  state);
        src_has_dma_mappings = true;
    }
    else if (uvm_dma_mapping_required_on_copying_gpu(va_space, dst_id, copying_gpu)) {
        status = dma_map_non_failed_pages_in_mask(copying_gpu,
                                                  &state->dma.sgt_from[uvm_id_value(src_id)],
                                                  dst,
                                                  page_mask,
                                                  state->num_pages,
                                                  state);
        dst_has_dma_mappings = true;
    }

    if (status != NV_OK)
        return status;

    state->dma.sgt_from_gpus[uvm_id_value(src_id)] = copying_gpu;

    status = migrate_vma_copy_begin_push(va_space, copying_gpu, channel_type, dst_id, src_id, start, outer - 1, &push);
    if (status != NV_OK)
        return status;

    // The zero tracker will be empty if zeroing is not necessary
    uvm_push_acquire_tracker(&push, &zero_tracker);
    uvm_tracker_deinit(&zero_tracker);

    if (!(src_has_dma_mappings || dst_has_dma_mappings)) {
        copy_pages_in_mask(&push, src, dst, page_mask, src_id, state);
    }
    else {
        copy_dma_mapped_pages(&push,
                              src,
                              dst,
                              src_has_dma_mappings,
                              page_mask,
                              src_id,
                              state);
    }

    // TODO: Bug 1766424: If the destination is a GPU and the copy was done by
    //       that GPU, use a GPU-local membar if no peer nor the CPU can
    //       currently map this page. When peer access gets enabled, do a
    //       MEMBAR_SYS at that point.
    uvm_push_end(&push);

    return uvm_tracker_add_push_safe(&state->tracker, &push);
}

static NV_STATUS uvm_migrate_vma_copy_pages(struct vm_area_struct *vma,
                                            const unsigned long *src,
                                            unsigned long *dst,
                                            unsigned long start,
                                            unsigned long outer,
                                            migrate_vma_state_t *state)
{
    uvm_processor_id_t src_id;

    for_each_id_in_mask(src_id, &state->src_processors) {
        NV_STATUS status = uvm_uvm_migrate_vma_copy_pages_from(vma, src, dst, start, outer, src_id, state);
        if (status != NV_OK)
            return status;
    }

    return NV_OK;
}

static void uvm_migrate_vma_cleanup_pages(unsigned long *dst, unsigned long npages)
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

static NV_STATUS uvm_migrate_vma_state_init_sgt(migrate_vma_state_t *state)
{
    uvm_processor_id_t src_id;

    if (sg_alloc_table(&state->dma.sgt_anon, state->num_pages, NV_UVM_GFP_FLAGS))
        return NV_ERR_NO_MEMORY;

    state->dma.sgt_anon_gpu = NULL;

    for_each_id_in_mask(src_id, &state->src_processors) {
        if (sg_alloc_table(&state->dma.sgt_from[uvm_id_value(src_id)], state->num_pages, NV_UVM_GFP_FLAGS))
            return NV_ERR_NO_MEMORY;

        state->dma.sgt_from_gpus[uvm_id_value(src_id)] = NULL;
    }

    return NV_OK;
}

static void uvm_migrate_vma_state_deinit_sgt(migrate_vma_state_t *state)
{
    uvm_processor_id_t src_id;
    uvm_gpu_t *gpu;

    gpu = state->dma.sgt_anon_gpu;
    if (gpu) {
        dma_unmap_sg(&gpu->parent->pci_dev->dev,
                     state->dma.sgt_anon.sgl,
                     state->dma.num_pages,
                     DMA_BIDIRECTIONAL);
    }
    state->dma.sgt_anon_gpu = NULL;
    sg_free_table(&state->dma.sgt_anon);

    for_each_id_in_mask(src_id, &state->src_processors) {
        gpu = state->dma.sgt_from_gpus[uvm_id_value(src_id)];

        if (gpu) {
            dma_unmap_sg(&gpu->parent->pci_dev->dev,
                         state->dma.sgt_from[uvm_id_value(src_id)].sgl,
                         state->dma.num_pages,
                         DMA_BIDIRECTIONAL);
        }
        state->dma.sgt_from_gpus[uvm_id_value(src_id)] = NULL;
        sg_free_table(&state->dma.sgt_from[uvm_id_value(src_id)]);
    }
}

static void uvm_migrate_vma_alloc_and_copy(struct migrate_vma *args, migrate_vma_state_t *state)
{
    struct vm_area_struct *vma = args->vma;
    unsigned long start = args->start;
    unsigned long outer = args->end;
    NV_STATUS tracker_status;
    uvm_migrate_args_t *uvm_migrate_args = state->uvm_migrate_args;

    uvm_tracker_init(&state->tracker);

    state->num_pages = (outer - start) / PAGE_SIZE;
    state->status = NV_OK;

    uvm_migrate_vma_state_compute_masks(vma, args->src, state);

    state->status = uvm_migrate_vma_state_init_sgt(state);

    if (state->status != NV_OK)
        return;

    state->status = uvm_migrate_vma_populate_anon_pages(vma, args->dst, start, outer, state);
    if (state->status == NV_OK)
        state->status = uvm_migrate_vma_copy_pages(vma, args->src, args->dst, start, outer, state);

    // Wait for tracker since all copies must have completed before returning
    tracker_status = uvm_tracker_wait_deinit(&state->tracker);

    if (state->status == NV_OK)
        state->status = tracker_status;

    // Check if the copy might have been impacted by NVLINK errors.
    if (state->status == NV_OK) {
        uvm_processor_id_t src_id;

        for_each_id_in_mask(src_id, &state->src_processors) {
            NV_STATUS status;

            // Skip CPU source, even if for some reason the operation went over
            // NVLINK, it'd be a read and hit poison.
            if (UVM_ID_IS_CPU(src_id))
                continue;

            UVM_ASSERT(UVM_ID_IS_GPU(src_id));
            status = uvm_gpu_check_nvlink_error_no_rm(uvm_gpu_get(src_id));

            // Set state->status to the first error if there's an NVLINK error.
            // Do not report NV_WARN_MORE_PROCESSING_REQUIRED. The call to the
            // uvm_migrate_vma_populate_anon_pages above zeroed the destination.
            // Thus in case of real STO error zeroed pages will be mapped.
            if (state->status == NV_OK && status != NV_WARN_MORE_PROCESSING_REQUIRED)
                state->status = status;

            // Record unresolved GPU errors if the caller can use the information
            if (status == NV_WARN_MORE_PROCESSING_REQUIRED) {
                if (uvm_migrate_args->gpus_to_check_for_nvlink_errors)
                    uvm_processor_mask_set(uvm_migrate_args->gpus_to_check_for_nvlink_errors, src_id);

                // fail the copy if requested by the caller
                if (uvm_migrate_args->fail_on_unresolved_sto_errors && state->status == NV_OK)
                    state->status = NV_ERR_BUSY_RETRY;
            }
        }
    }

    // Mark all pages as not migrating if we're failing
    if (state->status != NV_OK) {
        uvm_migrate_vma_cleanup_pages(args->dst, state->num_pages);
        uvm_migrate_vma_state_deinit_sgt(state);
    }
}

#if defined(CONFIG_MIGRATE_VMA_HELPER)
static void uvm_migrate_vma_alloc_and_copy_helper(struct vm_area_struct *vma,
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

    uvm_migrate_vma_alloc_and_copy(&args, (migrate_vma_state_t *) private);
}
#endif

static void uvm_migrate_vma_finalize_and_map(struct migrate_vma *args, migrate_vma_state_t *state)
{
    unsigned long i;

    for (i = 0; i < state->num_pages; i++) {
        // There are two reasons a page might not have been migrated.
        //
        // 1. Page is already resident at the destination.
        // 2. Page failed migration because the page state could not be migrated
        //    by the kernel.
        //
        // So, only set the corresponding populate_pages bit if both the
        // following conditions are true.
        //
        // 1.Trying to populate pages (with gup) which are already resident at
        // the destination is wasteful but usually harmless except in the
        // PROT_NONE case. gup returns NV_ERR_INVALID_ADDRESS for such pages and
        // will incorrectly lead to API migration failures even though migration
        // worked as expected.
        //
        // 2. Migration failure was not because of allocation failure in
        // uvm_migrate_vma_finalize_and_map() since such failures would be
        // indicated in allocation_failed_mask. Failures other than allocation
        // failures likely means that the page is populated somewhere. So, set
        // the corresponding bit in populate_pages_mask.
        if (!(args->src[i] & MIGRATE_PFN_MIGRATE) &&
            !test_bit(i, state->dst_resident_pages_mask) &&
            !test_bit(i, state->allocation_failed_mask))
            __set_bit(i, state->populate_pages_mask);
    }

    UVM_ASSERT(!bitmap_intersects(state->populate_pages_mask, state->allocation_failed_mask, state->num_pages));
    uvm_migrate_vma_state_deinit_sgt(state);
}

#if defined(CONFIG_MIGRATE_VMA_HELPER)
static void uvm_migrate_vma_finalize_and_map_helper(struct vm_area_struct *vma,
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
        .alloc_and_copy = uvm_migrate_vma_alloc_and_copy_helper,
        .finalize_and_map = uvm_migrate_vma_finalize_and_map_helper,
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

    uvm_migrate_vma_alloc_and_copy(args, state);
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
                                           uvm_migrate_args->populate_permissions,
                                           uvm_migrate_args->populate_flags);
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

    // This isn't the right path for a UVM-owned vma. In most cases the callers
    // will take the correct (managed) path, but we can get here if invoked on a
    // disabled vma (see uvm_disable_vma()) that has no VA range but still has a
    // vma. This could cause locking issues if the caller has the VA space
    // locked and we invoke a UVM fault handler, so avoid it entirely.
    if (uvm_file_is_nvidia_uvm_va_space(vma->vm_file))
        return NV_ERR_INVALID_ADDRESS;

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
            NV_STATUS populate_status;

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

            // Populate pages with uvm_populate_pageable if requested.
            if (uvm_migrate_args->populate_on_migrate_vma_failures) {
                populate_status = uvm_populate_pageable_vma(vma,
                                                            start,
                                                            length,
                                                            uvm_migrate_args->populate_permissions,
                                                            uvm_migrate_args->populate_flags);
            }
            else {
                *user_space_start = start;
                *user_space_length = outer - start;
                populate_status = NV_WARN_NOTHING_TO_DO;
            }

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
    uvm_processor_id_t dst_id = uvm_migrate_args->dst_id;

    UVM_ASSERT(PAGE_ALIGNED(uvm_migrate_args->start));
    UVM_ASSERT(PAGE_ALIGNED(uvm_migrate_args->length));
    uvm_assert_mmap_lock_locked(uvm_migrate_args->mm);

    if (UVM_ID_IS_CPU(dst_id)) {
        if (uvm_migrate_args->dst_node_id == -1)
            return NV_ERR_INVALID_ARGUMENT;
    }
    else {
        uvm_gpu_t *gpu = uvm_gpu_get(dst_id);

        // Incoming dst_node_id is only valid if dst_id belongs to the CPU. Use
        // dst_node_id as the GPU node id if dst_id doesn't belong to the CPU.
        UVM_ASSERT(gpu->mem_info.numa.enabled);
        uvm_migrate_args->dst_node_id = uvm_gpu_numa_node(gpu);
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
