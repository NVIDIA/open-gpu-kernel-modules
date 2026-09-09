/*******************************************************************************
    Copyright (c) 2015-2025 NVIDIA Corporation

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
#include "uvm_linux.h"
#include "uvm_lock.h"
#include "uvm_types.h"
#include "uvm_api.h"
#include "uvm_processors.h"
#include "uvm_global.h"
#include "uvm_gpu.h"
#include "uvm_hal.h"
#include "uvm_va_space.h"
#include "uvm_va_range.h"
#include "uvm_va_block.h"
#include "uvm_kvmalloc.h"
#include "uvm_map_external.h"
#include "uvm_va_range_dmabuf.h"
#include "uvm_perf_thrashing.h"
#include "nv_uvm_interface.h"

#include <linux/sched.h>

#if UVM_PROVIDES_DMA_BUF_IMPORTER()
#include <linux/dma-buf.h>
#include <linux/dma-resv.h>
#endif

static struct kmem_cache *g_uvm_va_range_managed_cache __read_mostly;
static struct kmem_cache *g_uvm_va_range_external_cache __read_mostly;
#if UVM_PROVIDES_DMA_BUF_IMPORTER()
static struct kmem_cache *g_uvm_va_range_dma_buf_cache __read_mostly;
#endif
static struct kmem_cache *g_uvm_va_range_channel_cache __read_mostly;
static struct kmem_cache *g_uvm_va_range_sked_reflected_cache __read_mostly;
static struct kmem_cache *g_uvm_va_range_semaphore_pool_cache __read_mostly;
static struct kmem_cache *g_uvm_vma_wrapper_cache __read_mostly;

NV_STATUS uvm_va_range_init(void)
{
    NV_STATUS status;

    g_uvm_va_range_managed_cache = NV_KMEM_CACHE_CREATE("uvm_va_range_managed_t", uvm_va_range_managed_t);
    if (!g_uvm_va_range_managed_cache)
        return NV_ERR_NO_MEMORY;

    g_uvm_va_range_external_cache = NV_KMEM_CACHE_CREATE("uvm_va_range_external_t", uvm_va_range_external_t);
    if (!g_uvm_va_range_external_cache)
        return NV_ERR_NO_MEMORY;

#if UVM_PROVIDES_DMA_BUF_IMPORTER()
    g_uvm_va_range_dma_buf_cache = NV_KMEM_CACHE_CREATE("uvm_va_range_dma_buf_t", uvm_va_range_dma_buf_t);
    if (!g_uvm_va_range_dma_buf_cache)
        return NV_ERR_NO_MEMORY;
#endif

    g_uvm_va_range_channel_cache = NV_KMEM_CACHE_CREATE("uvm_va_range_channel_t", uvm_va_range_channel_t);
    if (!g_uvm_va_range_channel_cache)
        return NV_ERR_NO_MEMORY;

    g_uvm_va_range_sked_reflected_cache = NV_KMEM_CACHE_CREATE("uvm_va_range_sked_reflected_t",
                                                               uvm_va_range_sked_reflected_t);
    if (!g_uvm_va_range_sked_reflected_cache)
        return NV_ERR_NO_MEMORY;

    g_uvm_va_range_semaphore_pool_cache = NV_KMEM_CACHE_CREATE("uvm_va_range_semaphore_pool_t",
                                                               uvm_va_range_semaphore_pool_t);
    if (!g_uvm_va_range_semaphore_pool_cache)
        return NV_ERR_NO_MEMORY;

    g_uvm_vma_wrapper_cache = NV_KMEM_CACHE_CREATE("uvm_vma_wrapper_t", uvm_vma_wrapper_t);
    if (!g_uvm_vma_wrapper_cache)
        return NV_ERR_NO_MEMORY;

    status = uvm_va_range_device_p2p_init();
    if (status != NV_OK)
        return status;

    return uvm_va_block_init();
}

void uvm_va_range_exit(void)
{
    uvm_va_block_exit();
    kmem_cache_destroy_safe(&g_uvm_va_range_managed_cache);
    kmem_cache_destroy_safe(&g_uvm_va_range_external_cache);
#if UVM_PROVIDES_DMA_BUF_IMPORTER()
    kmem_cache_destroy_safe(&g_uvm_va_range_dma_buf_cache);
#endif
    kmem_cache_destroy_safe(&g_uvm_va_range_channel_cache);
    kmem_cache_destroy_safe(&g_uvm_va_range_sked_reflected_cache);
    kmem_cache_destroy_safe(&g_uvm_va_range_semaphore_pool_cache);
    kmem_cache_destroy_safe(&g_uvm_vma_wrapper_cache);
    uvm_va_range_device_p2p_exit();
}

static NvU64 block_calc_start(uvm_va_range_managed_t *managed_range, size_t index)
{
    NvU64 range_start = UVM_VA_BLOCK_ALIGN_DOWN(managed_range->va_range.node.start);
    NvU64 block_start = range_start + index * UVM_VA_BLOCK_SIZE;
    NvU64 start = max(managed_range->va_range.node.start, block_start);
    UVM_ASSERT(start < managed_range->va_range.node.end);
    return start;
}

static NvU64 block_calc_end(uvm_va_range_managed_t *managed_range, size_t index)
{
    NvU64 start = block_calc_start(managed_range, index);
    NvU64 block_end = UVM_VA_BLOCK_ALIGN_UP(start + 1) - 1; // Inclusive end
    NvU64 end = min(managed_range->va_range.node.end, block_end);
    UVM_ASSERT(end > managed_range->va_range.node.start);
    return end;
}

// Called before the range's bounds have been adjusted. This may not actually
// shrink the blocks array. For example, if the shrink attempt fails then
// managed_range's old array is left intact. This may waste memory, but it means
// this function cannot fail.
static void blocks_array_shrink(uvm_va_range_managed_t *managed_range, size_t new_num_blocks)
{
    size_t new_size = new_num_blocks * sizeof(managed_range->blocks[0]);
    atomic_long_t *new_blocks;

    UVM_ASSERT(managed_range->blocks);
    UVM_ASSERT(uvm_kvsize(managed_range->blocks) >= uvm_va_range_num_blocks(managed_range) *
                                                    sizeof(managed_range->blocks[0]));
    UVM_ASSERT(new_num_blocks);
    UVM_ASSERT(new_num_blocks <= uvm_va_range_num_blocks(managed_range));

    // TODO: Bug 1766579: This could be optimized by only shrinking the array
    //       when the new size is half of the old size or some similar
    //       threshold. Need to profile this on real apps to see if that's worth
    //       doing.

    new_blocks = uvm_kvrealloc(managed_range->blocks, new_size);
    if (!new_blocks) {
        // If we failed to allocate a smaller array, just leave the old one as-is
        UVM_DBG_PRINT("Failed to shrink range [0x%llx, 0x%llx] from %zu blocks to %zu blocks\n",
                      managed_range->va_range.node.start,
                      managed_range->va_range.node.end,
                      uvm_kvsize(managed_range->blocks) / sizeof(managed_range->blocks[0]),
                      new_num_blocks);
        return;
    }

    managed_range->blocks = new_blocks;
}

// va_range must already be zeroed.
static void uvm_va_range_initialize(uvm_va_range_t *va_range,
                                    uvm_va_range_type_t type,
                                    uvm_va_space_t *va_space,
                                    NvU64 start,
                                    NvU64 end)
{
    uvm_assert_rwsem_locked_write(&va_space->lock);

    va_range->va_space = va_space;
    va_range->node.start = start;
    va_range->node.end = end;

    // The range is inserted into the VA space tree only at the end of creation,
    // so clear the node so the destroy path knows whether to remove it.
    RB_CLEAR_NODE(&va_range->node.rb_node);

    va_range->type = type;
}

NV_STATUS uvm_va_range_initialize_reclaim(uvm_va_range_t *va_range,
                                          struct mm_struct *mm,
                                          uvm_va_range_type_t type,
                                          uvm_va_space_t *va_space,
                                          NvU64 start,
                                          NvU64 end)
{
    NV_STATUS status;

    // Check for no overlap with HMM blocks.
    status = uvm_hmm_va_block_reclaim(va_space, mm, start, end);
    if (status != NV_OK)
        return status;

    uvm_va_range_initialize(va_range, type, va_space, start, end);
    return NV_OK;
}

static uvm_va_range_managed_t *uvm_va_range_alloc_managed(uvm_va_space_t *va_space, NvU64 start, NvU64 end)
{
    uvm_va_range_managed_t *managed_range = NULL;

    managed_range = nv_kmem_cache_zalloc(g_uvm_va_range_managed_cache, NV_UVM_GFP_FLAGS);
    if (!managed_range)
        return NULL;

    uvm_va_range_initialize(&managed_range->va_range, UVM_VA_RANGE_TYPE_MANAGED, va_space, start, end);

    managed_range->policy = uvm_va_policy_default;

    managed_range->blocks = uvm_kvmalloc_zero(uvm_va_range_num_blocks(managed_range) *
                                              sizeof(managed_range->blocks[0]));
    if (!managed_range->blocks) {
        UVM_DBG_PRINT("Failed to allocate %zu blocks\n", uvm_va_range_num_blocks(managed_range));
        uvm_va_range_destroy(&managed_range->va_range, NULL);
        return NULL;
    }

    return managed_range;
}

NV_STATUS uvm_va_range_create_mmap(uvm_va_space_t *va_space,
                                   struct mm_struct *mm,
                                   uvm_vma_wrapper_t *vma_wrapper,
                                   uvm_va_range_managed_t **out_managed_range)
{
    NV_STATUS status;
    struct vm_area_struct *vma = vma_wrapper->vma;
    uvm_va_range_managed_t *managed_range = NULL;

    // Check for no overlap with HMM blocks.
    status = uvm_hmm_va_block_reclaim(va_space, mm, vma->vm_start, vma->vm_end - 1);
    if (status != NV_OK)
        return status;

    // vma->vm_end is exclusive but managed_range end is inclusive
    managed_range = uvm_va_range_alloc_managed(va_space, vma->vm_start, vma->vm_end - 1);
    if (!managed_range)
        return NV_ERR_NO_MEMORY;

    managed_range->vma_wrapper = vma_wrapper;

    status = uvm_range_tree_add(&va_space->va_range_tree, &managed_range->va_range.node);
    if (status != NV_OK)
        goto error;

    if (out_managed_range)
        *out_managed_range = managed_range;

    return NV_OK;

error:
    uvm_va_range_destroy(&managed_range->va_range, NULL);
    return status;
}

NV_STATUS uvm_va_range_create_external(uvm_va_space_t *va_space,
                                       struct mm_struct *mm,
                                       NvU64 start,
                                       NvU64 length,
                                       uvm_va_range_external_t **out_external_range)
{
    NV_STATUS status;
    uvm_va_range_external_t *external_range = NULL;
    uvm_processor_mask_t *retained_mask = NULL;
    NvU32 i;

    external_range = nv_kmem_cache_zalloc(g_uvm_va_range_external_cache, NV_UVM_GFP_FLAGS);
    if (!external_range)
        return NV_ERR_NO_MEMORY;

    status = uvm_va_range_initialize_reclaim(&external_range->va_range,
                                             mm,
                                             UVM_VA_RANGE_TYPE_EXTERNAL,
                                             va_space,
                                             start,
                                             start + length - 1);
    if (status != NV_OK) {
        kmem_cache_free(g_uvm_va_range_external_cache, external_range);
        return status;
    }

    UVM_ASSERT(!external_range->retained_mask);

    retained_mask = uvm_processor_mask_cache_alloc();
    if (!retained_mask) {
        status = NV_ERR_NO_MEMORY;
        goto error;
    }

    external_range->retained_mask = retained_mask;

    for (i = 0; i < ARRAY_SIZE(external_range->gpu_ranges); i++) {
        uvm_mutex_init(&external_range->gpu_ranges[i].lock, UVM_LOCK_ORDER_EXT_RANGE_TREE);
        uvm_range_tree_init(&external_range->gpu_ranges[i].tree);
    }

    status = uvm_range_tree_add(&va_space->va_range_tree, &external_range->va_range.node);
    if (status != NV_OK)
        goto error;

    if (out_external_range)
        *out_external_range = external_range;

    return NV_OK;

error:
    uvm_va_range_destroy(&external_range->va_range, NULL);

    return status;
}

#if UVM_PROVIDES_DMA_BUF_IMPORTER()
NV_STATUS uvm_va_range_create_dma_buf(uvm_va_space_t *va_space,
                                      struct mm_struct *mm,
                                      struct dma_buf *dmabuf,
                                      NvU64 start,
                                      NvU64 length,
                                      uvm_va_range_dma_buf_t **out_dmabuf_range)
{
    NV_STATUS status;
    uvm_va_range_dma_buf_t *dmabuf_range = NULL;
    NvU32 i;

    // Assigns to zero mapped_gpus, revoked_gpus bitmasks.
    dmabuf_range = nv_kmem_cache_zalloc(g_uvm_va_range_dma_buf_cache, NV_UVM_GFP_FLAGS);
    if (!dmabuf_range)
        return NV_ERR_NO_MEMORY;

    get_dma_buf(dmabuf);
    dmabuf_range->dmabuf = dmabuf;

    status = uvm_va_range_initialize_reclaim(&dmabuf_range->va_range,
                                             mm,
                                             UVM_VA_RANGE_TYPE_DMA_BUF,
                                             va_space,
                                             start,
                                             start + length - 1);
    if (status != NV_OK) {
        kmem_cache_free(g_uvm_va_range_dma_buf_cache, dmabuf_range);
        return status;
    }

    dmabuf_range->retained_mask = uvm_processor_mask_cache_alloc();
    if (!dmabuf_range->retained_mask) {
        status = NV_ERR_NO_MEMORY;
        goto error;
    }

    for (i = 0; i < ARRAY_SIZE(dmabuf_range->gpu_ranges); i++) {
        uvm_mutex_init(&dmabuf_range->gpu_ranges[i].base.lock, UVM_LOCK_ORDER_DMA_BUF_RANGE_TREE);
        uvm_range_tree_init(&dmabuf_range->gpu_ranges[i].base.tree);
    }

    status = uvm_range_tree_add(&va_space->va_range_tree, &dmabuf_range->va_range.node);
    if (status != NV_OK)
        goto error;

    if (out_dmabuf_range)
        *out_dmabuf_range = dmabuf_range;

    return NV_OK;

error:
    uvm_va_range_destroy(&dmabuf_range->va_range, NULL);

    return status;
}
#else
NV_STATUS uvm_va_range_create_dma_buf(uvm_va_space_t *va_space,
                                      struct mm_struct *mm,
                                      struct dma_buf *dmabuf,
                                      NvU64 start,
                                      NvU64 length,
                                      uvm_va_range_dma_buf_t **out_dmabuf_range)
{
    return NV_ERR_NOT_SUPPORTED;
}
#endif // UVM_PROVIDES_DMA_BUF_IMPORTER()

NV_STATUS uvm_va_range_create_channel(uvm_va_space_t *va_space,
                                      struct mm_struct *mm,
                                      NvU64 start,
                                      NvU64 end,
                                      uvm_va_range_channel_t **out_channel_range)
{
    NV_STATUS status;
    uvm_va_range_channel_t *channel_range = NULL;

    channel_range = nv_kmem_cache_zalloc(g_uvm_va_range_channel_cache, NV_UVM_GFP_FLAGS);
    if (!channel_range)
        return NV_ERR_NO_MEMORY;

    status = uvm_va_range_initialize_reclaim(&channel_range->va_range,
                                             mm,
                                             UVM_VA_RANGE_TYPE_CHANNEL,
                                             va_space,
                                             start,
                                             end);
    if (status != NV_OK) {
        kmem_cache_free(g_uvm_va_range_channel_cache, channel_range);
        return status;
    }

    INIT_LIST_HEAD(&channel_range->list_node);

    status = uvm_range_tree_add(&va_space->va_range_tree, &channel_range->va_range.node);
    if (status != NV_OK)
        goto error;

    if (out_channel_range)
        *out_channel_range = channel_range;

    return NV_OK;

error:
    uvm_va_range_destroy(&channel_range->va_range, NULL);
    return status;
}

NV_STATUS uvm_va_range_create_sked_reflected(uvm_va_space_t *va_space,
                                             struct mm_struct *mm,
                                             NvU64 start,
                                             NvU64 length,
                                             uvm_va_range_sked_reflected_t **out_sked_reflected_range)
{
    NV_STATUS status;
    uvm_va_range_sked_reflected_t *sked_reflected_range = NULL;

    sked_reflected_range = nv_kmem_cache_zalloc(g_uvm_va_range_sked_reflected_cache, NV_UVM_GFP_FLAGS);
    if (!sked_reflected_range)
        return NV_ERR_NO_MEMORY;

    status = uvm_va_range_initialize_reclaim(&sked_reflected_range->va_range,
                                             mm,
                                             UVM_VA_RANGE_TYPE_SKED_REFLECTED,
                                             va_space,
                                             start,
                                             start + length - 1);
    if (status != NV_OK) {
        kmem_cache_free(g_uvm_va_range_sked_reflected_cache, sked_reflected_range);
        return status;
    }

    status = uvm_range_tree_add(&va_space->va_range_tree, &sked_reflected_range->va_range.node);
    if (status != NV_OK)
        goto error;

    if (out_sked_reflected_range)
        *out_sked_reflected_range = sked_reflected_range;

    return NV_OK;

error:
    uvm_va_range_destroy(&sked_reflected_range->va_range, NULL);
    return status;
}

NV_STATUS uvm_va_range_create_semaphore_pool(uvm_va_space_t *va_space,
                                             struct mm_struct *mm,
                                             NvU64 start,
                                             NvU64 length,
                                             const UvmGpuMappingAttributes *per_gpu_attrs,
                                             NvU32 per_gpu_attrs_count,
                                             uvm_va_range_semaphore_pool_t **out_semaphore_pool_range)
{
    static const uvm_mem_gpu_mapping_attrs_t default_attrs = {
            .protection = UVM_PROT_READ_WRITE_ATOMIC,
            .is_cacheable = false
    };

    NV_STATUS status;
    uvm_va_range_semaphore_pool_t *semaphore_pool_range = NULL;
    uvm_mem_alloc_params_t mem_alloc_params = { 0 };
    NvU32 i;
    uvm_gpu_id_t gpu_id;

    semaphore_pool_range = nv_kmem_cache_zalloc(g_uvm_va_range_semaphore_pool_cache, NV_UVM_GFP_FLAGS);
    if (!semaphore_pool_range)
        return NV_ERR_NO_MEMORY;

    status = uvm_va_range_initialize_reclaim(&semaphore_pool_range->va_range,
                                             mm,
                                             UVM_VA_RANGE_TYPE_SEMAPHORE_POOL,
                                             va_space,
                                             start,
                                             start + length - 1);
    if (status != NV_OK) {
        kmem_cache_free(g_uvm_va_range_semaphore_pool_cache, semaphore_pool_range);
        return status;
    }

    uvm_tracker_init(&semaphore_pool_range->tracker);
    uvm_mutex_init(&semaphore_pool_range->tracker_lock, UVM_LOCK_ORDER_SEMA_POOL_TRACKER);

    status = uvm_range_tree_add(&va_space->va_range_tree, &semaphore_pool_range->va_range.node);
    if (status != NV_OK)
        goto error;

    // The semaphore pool memory is located in sysmem, and must be zeroed upon
    // allocation because it may be mapped on the user VA space.
    mem_alloc_params.page_size = UVM_PAGE_SIZE_DEFAULT;
    mem_alloc_params.size = length;
    mem_alloc_params.zero = true;
    mem_alloc_params.mm = mm;

    semaphore_pool_range->default_gpu_attrs = default_attrs;
    semaphore_pool_range->owner = NULL;

    for_each_gpu_id(gpu_id)
        semaphore_pool_range->gpu_attrs[uvm_id_gpu_index(gpu_id)] = default_attrs;

    for (i = 0; i < per_gpu_attrs_count; i++) {
        uvm_gpu_t *gpu;
        uvm_mem_gpu_mapping_attrs_t attrs = default_attrs;

        status = uvm_mem_translate_gpu_attributes(&per_gpu_attrs[i], va_space, &gpu, &attrs);
        if (status != NV_OK)
            goto error;

        if (i == 0 && g_uvm_global.conf_computing_enabled)
            mem_alloc_params.dma_owner = gpu;

        if (attrs.is_cacheable) {
            // At most 1 GPU can have this memory cached, in which case it is
            // the 'owner' GPU.
            if (semaphore_pool_range->owner != NULL) {
                UVM_DBG_PRINT("Caching of semaphore pool requested on >1 GPU.");
                status = NV_ERR_INVALID_ARGUMENT;
                goto error;
            }

            semaphore_pool_range->owner = gpu;
        }

        semaphore_pool_range->gpu_attrs[uvm_id_gpu_index(gpu->id)] = attrs;
    }

    status = uvm_mem_alloc(&mem_alloc_params, &semaphore_pool_range->mem);
    if (status != NV_OK)
        goto error;

    status = uvm_mem_map_cpu_kernel(semaphore_pool_range->mem);
    if (status != NV_OK)
        goto error;

    if (out_semaphore_pool_range)
        *out_semaphore_pool_range = semaphore_pool_range;

    return NV_OK;

error:
    uvm_va_range_destroy(&semaphore_pool_range->va_range, NULL);
    return status;
}

static void uvm_va_range_destroy_managed(uvm_va_range_managed_t *managed_range)
{
    uvm_va_block_t *block;
    uvm_va_block_t *block_tmp;
    uvm_perf_event_data_t event_data;

    if (managed_range->blocks) {
        // Unmap and drop our ref count on each block
        for_each_va_block_in_va_range_safe(managed_range, block, block_tmp) {
            uvm_va_block_kill(block);

            // During tear down and with CONFIG_PREEMPT_NONE or an equivalent
            // configuration, uvm_va_block_kill() will not yield, and its mutex
            // acquisitions for va_blocks/chunks are unlikely to block due to
            // lack of contention. Conditionally yield per block to avoid
            // soft-lockup.
            cond_resched();
        }

        uvm_kvfree(managed_range->blocks);
    }

    event_data.range_destroy.range = &managed_range->va_range;
    uvm_perf_event_notify(&managed_range->va_range.va_space->perf_events, UVM_PERF_EVENT_RANGE_DESTROY, &event_data);

    kmem_cache_free(g_uvm_va_range_managed_cache, managed_range);
}

static void uvm_va_range_destroy_external(uvm_va_range_external_t *external_range, struct list_head *deferred_free_list)
{
    uvm_gpu_t *gpu;

    uvm_processor_mask_cache_free(external_range->retained_mask);

    if (uvm_processor_mask_empty(&external_range->mapped_gpus))
        goto out;

    UVM_ASSERT(deferred_free_list);

    for_each_va_space_gpu_in_mask(gpu, external_range->va_range.va_space, &external_range->mapped_gpus) {
        uvm_ext_gpu_range_tree_t *range_tree = uvm_ext_gpu_range_tree(external_range, gpu);
        uvm_ext_gpu_map_t *ext_map, *ext_map_next;

        uvm_mutex_lock(&range_tree->lock);
        uvm_ext_gpu_map_for_each_safe(ext_map, ext_map_next, external_range, gpu)
            uvm_ext_gpu_map_destroy(external_range, ext_map, deferred_free_list);
        uvm_mutex_unlock(&range_tree->lock);
    }

    UVM_ASSERT(uvm_processor_mask_empty(&external_range->mapped_gpus));

out:
    kmem_cache_free(g_uvm_va_range_external_cache, external_range);
}

#if UVM_PROVIDES_DMA_BUF_IMPORTER()
static void uvm_va_range_destroy_dma_buf(uvm_va_range_dma_buf_t *dmabuf_range,
                                         struct list_head *deferred_free_list)
{
    uvm_gpu_t *gpu;

    uvm_processor_mask_cache_free(dmabuf_range->retained_mask);

    for_each_va_space_gpu(gpu, dmabuf_range->va_range.va_space)
        uvm_dma_buf_gpu_range_tree_deinit(dmabuf_range, gpu, deferred_free_list);
    UVM_ASSERT(uvm_processor_mask_empty(&dmabuf_range->mapped_gpus));

    dma_buf_put(dmabuf_range->dmabuf);
    kmem_cache_free(g_uvm_va_range_dma_buf_cache, dmabuf_range);
}
#else
static void uvm_va_range_destroy_dma_buf(uvm_va_range_dma_buf_t *dmabuf_range,
                                         struct list_head *deferred_free_list) {}
#endif // UVM_PROVIDES_DMA_BUF_IMPORTER()

static void uvm_va_range_destroy_channel(uvm_va_range_channel_t *channel_range)
{
    uvm_gpu_va_space_t *gpu_va_space = channel_range->gpu_va_space;
    uvm_membar_t membar;

    UVM_ASSERT(channel_range->ref_count == 0);

    // Unmap the buffer
    if (gpu_va_space && channel_range->pt_range_vec.ranges) {
        membar = uvm_hal_downgrade_membar_type(gpu_va_space->gpu, channel_range->aperture == UVM_APERTURE_VID);
        uvm_page_table_range_vec_clear_ptes(&channel_range->pt_range_vec, membar);
        uvm_page_table_range_vec_deinit(&channel_range->pt_range_vec);
    }

    list_del(&channel_range->list_node);

    // Channel unregister handles releasing this descriptor back to RM
    channel_range->rm_descriptor = 0;
    kmem_cache_free(g_uvm_va_range_channel_cache, channel_range);
}

static void uvm_va_range_deinit_sked_reflected(uvm_va_range_sked_reflected_t *sked_reflected_range)
{
    uvm_gpu_va_space_t *gpu_va_space = sked_reflected_range->gpu_va_space;

    if (!gpu_va_space || !sked_reflected_range->pt_range_vec.ranges)
        return;

    // The SKED reflected mapping has no physical backing and hence no physical
    // accesses can be pending to it and no membar is needed.
    uvm_page_table_range_vec_clear_ptes(&sked_reflected_range->pt_range_vec, UVM_MEMBAR_NONE);
    uvm_page_table_range_vec_deinit(&sked_reflected_range->pt_range_vec);

    sked_reflected_range->gpu_va_space = NULL;

}

static void uvm_va_range_destroy_sked_reflected(uvm_va_range_sked_reflected_t *sked_reflected_range)
{
    uvm_va_range_deinit_sked_reflected(sked_reflected_range);
    kmem_cache_free(g_uvm_va_range_sked_reflected_cache, sked_reflected_range);
}

static void uvm_va_range_destroy_semaphore_pool(uvm_va_range_semaphore_pool_t *semaphore_pool_range)
{
    NV_STATUS status = uvm_tracker_wait_deinit(&semaphore_pool_range->tracker);
    if (status != NV_OK) {
        UVM_ASSERT_MSG(status == uvm_global_get_status(),
                       "uvm_tracker_wait() returned %d (%s) in uvm_va_range_destroy_semaphore_pool()\n",
                       status,
                       nvstatusToString(status));
    }
    uvm_mem_free(semaphore_pool_range->mem);
    semaphore_pool_range->mem = NULL;
    kmem_cache_free(g_uvm_va_range_semaphore_pool_cache, semaphore_pool_range);
}

void uvm_va_range_destroy(uvm_va_range_t *va_range, struct list_head *deferred_free_list)
{
    UVM_ASSERT(va_range);

    if (!RB_EMPTY_NODE(&va_range->node.rb_node))
        uvm_range_tree_remove(&va_range->va_space->va_range_tree, &va_range->node);

    switch (va_range->type) {
        case UVM_VA_RANGE_TYPE_INVALID:
            // Skip partially-created ranges with unset types
            break;
        case UVM_VA_RANGE_TYPE_MANAGED:
            uvm_va_range_destroy_managed(uvm_va_range_to_managed(va_range));
            return;
        case UVM_VA_RANGE_TYPE_EXTERNAL:
            uvm_va_range_destroy_external(uvm_va_range_to_external(va_range), deferred_free_list);
            return;
        case UVM_VA_RANGE_TYPE_DMA_BUF:
#if !UVM_PROVIDES_DMA_BUF_IMPORTER()
            UVM_ASSERT_MSG(0, "[0x%llx, 0x%llx] unexpected DMA_BUF range\n",
                           va_range->node.start, va_range->node.end);
#endif
            uvm_va_range_destroy_dma_buf(uvm_va_range_to_dma_buf(va_range), deferred_free_list);
            return;
        case UVM_VA_RANGE_TYPE_CHANNEL:
            uvm_va_range_destroy_channel(uvm_va_range_to_channel(va_range));
            return;
        case UVM_VA_RANGE_TYPE_SKED_REFLECTED:
            uvm_va_range_destroy_sked_reflected(uvm_va_range_to_sked_reflected(va_range));
            return;
        case UVM_VA_RANGE_TYPE_SEMAPHORE_POOL:
            uvm_va_range_destroy_semaphore_pool(uvm_va_range_to_semaphore_pool(va_range));
            return;
        case UVM_VA_RANGE_TYPE_DEVICE_P2P:
            unmap_mapping_range(va_range->va_space->mapping, va_range->node.start, uvm_va_range_size(va_range), 1);
            uvm_va_range_destroy_device_p2p(uvm_va_range_to_device_p2p(va_range), deferred_free_list);
            return;
        default:
            UVM_ASSERT_MSG(0, "[0x%llx, 0x%llx] has type %d\n",
                           va_range->node.start, va_range->node.end, va_range->type);
    }
}

void uvm_va_range_zombify(uvm_va_range_managed_t *managed_range)
{
    if (!managed_range)
        return;

    UVM_ASSERT(managed_range->vma_wrapper);

    // Destroy will be done by uvm_destroy_vma_managed
    managed_range->vma_wrapper = NULL;
}

static NV_STATUS uvm_free_semaphore_pool(uvm_va_range_semaphore_pool_t *semaphore_pool_range)
{
    // Semaphore pools must be first unmapped from the CPU with munmap to
    // invalidate the vma.
    if (uvm_mem_mapped_on_cpu_user(semaphore_pool_range->mem))
        return NV_ERR_INVALID_ARGUMENT;

    return NV_OK;
}

static uvm_processor_mask_t *uvm_free_external(uvm_va_range_external_t *external_range)
{
    uvm_processor_mask_t *retained_mask = external_range->retained_mask;
    uvm_va_space_t *va_space = external_range->va_range.va_space;

    // Set the retained_mask to NULL to prevent uvm_va_range_destroy_external()
    // from freeing the mask.
    external_range->retained_mask = NULL;

    UVM_ASSERT(retained_mask);

    // External ranges may have deferred free work, so the GPUs may have to be
    // retained. Construct the mask of all the GPUs that need to be retained.
    uvm_processor_mask_and(retained_mask, &external_range->mapped_gpus, &va_space->registered_gpus);

    return retained_mask;
}

static uvm_processor_mask_t *uvm_free_dma_buf(uvm_va_range_dma_buf_t *dmabuf_range)
{
    uvm_processor_mask_t *retained_mask = dmabuf_range->retained_mask;
    uvm_va_space_t *va_space = dmabuf_range->va_range.va_space;
    uvm_gpu_t *gpu;

    // Set the retained_mask to NULL to prevent uvm_va_range_destroy_dma_buf()
    // from freeing the mask, should no attachments require deferred free.
    dmabuf_range->retained_mask = NULL;

    UVM_ASSERT(retained_mask);
    uvm_processor_mask_zero(retained_mask);

    // DMA-BUF ranges defer free the destruction of attachments, so GPUs must be
    // retained. Construct the mask of all the GPUs that need to be retained.
    uvm_assert_rwsem_locked(&va_space->lock);
    for_each_va_space_gpu(gpu, va_space) {
        if (uvm_dma_buf_gpu_range_tree(dmabuf_range, gpu)->attach)
            uvm_processor_mask_set(retained_mask, gpu->id);
    }

    return retained_mask;
}

// This destroys VA ranges created by ioctl. VA ranges created by mmap, such as
// through UvmMemMap, go through munmap.
static NV_STATUS uvm_free(uvm_va_space_t *va_space, NvU64 base)
{
    uvm_va_range_t *va_range;
    NV_STATUS status = NV_OK;
    uvm_processor_mask_t *retained_mask = NULL;
    uvm_gpu_t *retained_gpu = NULL;
    LIST_HEAD(deferred_free_list);

    uvm_va_space_down_write(va_space);

    va_range = uvm_va_range_find(va_space, base);
    if (!va_range || va_range->node.start != base) {
        status = NV_ERR_INVALID_ADDRESS;
        goto out;
    }

    switch (va_range->type) {
        case UVM_VA_RANGE_TYPE_EXTERNAL:
            retained_mask = uvm_free_external(uvm_va_range_to_external(va_range));
            break;

        case UVM_VA_RANGE_TYPE_DMA_BUF:
            retained_mask = uvm_free_dma_buf(uvm_va_range_to_dma_buf(va_range));
            break;

        case UVM_VA_RANGE_TYPE_SEMAPHORE_POOL:
            status = uvm_free_semaphore_pool(uvm_va_range_to_semaphore_pool(va_range));
            break;

        case UVM_VA_RANGE_TYPE_DEVICE_P2P:
            retained_gpu = uvm_va_range_to_device_p2p(va_range)->gpu;
            break;

        case UVM_VA_RANGE_TYPE_SKED_REFLECTED:
            break;

        default:
            status = NV_ERR_INVALID_ADDRESS;
            break;
    }

    if (status != NV_OK)
        goto out;

    uvm_va_range_destroy(va_range, &deferred_free_list);

    // If there is deferred work, retain the required GPUs.
    if (!list_empty(&deferred_free_list)) {
        if (retained_mask)
            uvm_global_gpu_retain(retained_mask);
        else
            uvm_gpu_retain(retained_gpu);
    }

out:
    uvm_va_space_up_write(va_space);

    if (!list_empty(&deferred_free_list)) {
        UVM_ASSERT(status == NV_OK);
        uvm_deferred_free_object_list(&deferred_free_list);
        if (retained_mask)
            uvm_global_gpu_release(retained_mask);
        else
            uvm_gpu_release(retained_gpu);
    }

    // Free the mask allocated in uvm_va_range_create_external() since
    // uvm_va_range_destroy() won't free this mask.
    uvm_processor_mask_cache_free(retained_mask);

    return status;
}

NV_STATUS uvm_api_free(UVM_FREE_PARAMS *params, struct file *filp)
{
    return uvm_free(uvm_va_space_get(filp), params->base);
}

NV_STATUS uvm_api_clean_up_zombie_resources(UVM_CLEAN_UP_ZOMBIE_RESOURCES_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_va_range_t *va_range, *va_range_next;

    uvm_va_space_down_write(va_space);

    uvm_for_each_va_range_safe(va_range, va_range_next, va_space) {
        if (uvm_va_range_is_managed_zombie(va_range))
            uvm_va_range_destroy(va_range, NULL);
    }

    uvm_va_space_up_write(va_space);

    return NV_OK;
}

NV_STATUS uvm_api_validate_va_range(UVM_VALIDATE_VA_RANGE_PARAMS *params, struct file *filp)
{
    NV_STATUS status = NV_ERR_INVALID_ADDRESS;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_va_range_t *va_range;

    uvm_va_space_down_read(va_space);

    va_range = uvm_va_range_find(va_space, params->base);
    if (va_range && va_range->node.start == params->base && va_range->node.end + 1 == params->base + params->length)
        status = NV_OK;

    uvm_va_space_up_read(va_space);

    return status;
}

// Helper to update residency histogram for a single page
static void query_residency_update_page_residency(uvm_processor_id_t processor,
                                                  NvU32 *processor_histogram,
                                                  int numa_node,
                                                  NvU32 *numa_node_histogram)
{
    UVM_ASSERT(processor_histogram);

    processor_histogram[uvm_id_value(processor)]++;

    // Track NUMA node for CPU pages (skip if NUMA disabled)
    if (numa_node_histogram && UVM_ID_IS_CPU(processor) && numa_node >= 0 && numa_node < nr_node_ids)
        numa_node_histogram[numa_node]++;
}

// Query residency for a single page in a VA block.
// Locking: Caller must hold va_block->lock.
static void query_residency_va_block_page(uvm_va_block_t *va_block,
                                          NvU64 addr,
                                          uvm_processor_mask_t *scratch_mask,
                                          NvU32 *processor_histogram,
                                          NvU32 *numa_node_histogram)
{
    uvm_processor_id_t processor;
    uvm_page_index_t page_index = uvm_va_block_cpu_page_index(va_block, addr);
    int numa_node = NUMA_NO_NODE;

    uvm_processor_mask_zero(scratch_mask);
    uvm_va_block_page_resident_processors(va_block, page_index, scratch_mask);

    if (uvm_processor_mask_empty(scratch_mask))
        return;

    // Prioritize GPUs over CPUs for read-duplicated pages
    if (uvm_processor_mask_get_count(scratch_mask) > 1)
        processor = uvm_processor_mask_find_first_gpu_id(scratch_mask);
    else
        processor = uvm_processor_mask_find_first_id(scratch_mask);

    // For CPU pages, verify the page is actually allocated (has physical backing).
    // If not, it's unpopulated and should not count towards any processor.
    if (UVM_ID_IS_CPU(processor)) {
        struct page *page;

        // Check allocated mask first before accessing the page
        if (!uvm_page_mask_test(&va_block->cpu.allocated, page_index))
            return;

        page = uvm_va_block_get_cpu_page(va_block, page_index);
        if (!page)
            return;

        // Get NUMA node for physically backed CPU pages
        if (numa_node_histogram)
            numa_node = page_to_nid(page);
    }

    query_residency_update_page_residency(processor,
                                          processor_histogram,
                                          numa_node,
                                          numa_node_histogram);
}

// Helper to find and update HMM va_block for residency queries.
// Returns NULL if no HMM block exists or if updating residency info fails.
// The update step is critical: it calls hmm_range_fault() to get the current
// page state from the Linux kernel, ensuring we report fresh data.
static uvm_va_block_t *query_residency_find_hmm_va_block_or_null(uvm_va_space_t *va_space,
                                                                 struct mm_struct *mm,
                                                                 NvU64 addr)
{
    uvm_va_block_t *va_block = NULL;
    NV_STATUS status;

    status = uvm_hmm_va_block_find(va_space, addr, &va_block);
    if (status != NV_OK)
        return NULL;

    if (va_block) {
        // Update the va_block's residency info from the kernel via hmm_range_fault().
        // This ensures we query the actual current state, not stale cached data.
        status = uvm_hmm_va_block_update_residency_info_unlocked(va_block, mm, addr, false);
        if (status != NV_OK)
            va_block = NULL;
    }

    return va_block;
}

// Query residency for a single failed page (for slow path with failed
// move_pages).
static NV_STATUS query_residency_single_page(uvm_va_space_t *va_space,
                                             struct mm_struct *mm,
                                             NvU64 addr,
                                             uvm_processor_mask_t *scratch_mask,
                                             NvU32 *processor_histogram,
                                             NvU32 *numa_node_histogram)
{
    uvm_va_range_t *va_range;
    uvm_va_range_managed_t *managed_range;
    uvm_va_block_t *va_block = NULL;
    size_t block_index;

    // First try managed ranges
    va_range = uvm_va_range_find(va_space, addr);
    managed_range = uvm_va_range_to_managed_or_null(va_range);

    if (managed_range) {
        // Found a managed range - get the VA block
        block_index = uvm_va_range_block_index(managed_range, addr);
        va_block = uvm_va_range_block(managed_range, block_index);
    } else if (!va_range) {
        // No managed range - try HMM
        // Note: This only finds HMM pages that already have va_blocks
        va_block = query_residency_find_hmm_va_block_or_null(va_space, mm, addr);
    } else {
        // Non-managed VA range (e.g., external, semaphore pool), skip
        return NV_OK;
    }

    if (!va_block)
        return NV_OK; // No block allocated, skip

    uvm_mutex_lock(&va_block->lock);
    query_residency_va_block_page(va_block,
                                  addr,
                                  scratch_mask,
                                  processor_histogram,
                                  numa_node_histogram);
    uvm_mutex_unlock(&va_block->lock);

    return NV_OK;
}

// Fast path: query residency for managed memory ranges.
static NV_STATUS query_residency_managed(uvm_va_space_t *va_space,
                                         struct mm_struct *mm,
                                         NvU64 base,
                                         NvU64 length,
                                         NvU32 sampling_stride,
                                         NvU32 *processor_histogram,
                                         NvU32 *numa_node_histogram)
{
    NvU64 addr;
    NvU64 end_addr = base + length;
    uvm_processor_mask_t *scratch_mask;

    scratch_mask = uvm_processor_mask_cache_alloc();
    if (!scratch_mask)
        return NV_ERR_NO_MEMORY;

    // Walk through sampled addresses in the range
    for (addr = base; addr < end_addr; addr += sampling_stride) {
        uvm_va_range_t *va_range = uvm_va_range_find(va_space, addr);
        uvm_va_range_managed_t *managed_range = uvm_va_range_to_managed_or_null(va_range);
        uvm_va_block_t *va_block;
        size_t block_index;

        if (!managed_range)
            continue;

        block_index = uvm_va_range_block_index(managed_range, addr);
        va_block = uvm_va_range_block(managed_range, block_index);
        if (!va_block)
            continue;

        uvm_mutex_lock(&va_block->lock);
        query_residency_va_block_page(va_block,
                                      addr,
                                      scratch_mask,
                                      processor_histogram,
                                      numa_node_histogram);
        uvm_mutex_unlock(&va_block->lock);
    }

    uvm_processor_mask_cache_free(scratch_mask);
    return NV_OK;
}

// Validates that the queried range is homogeneous (all managed or all
// non-managed) and contiguous. Returns NV_OK if valid, NV_ERR_INVALID_ADDRESS
// if mixed, mismatched, or non-contiguous.
static NV_STATUS query_residency_validate_range_homogeneity(uvm_va_space_t *va_space,
                                                            NvU64 base,
                                                            NvU64 length,
                                                            NvBool is_managed_memory)
{
    uvm_va_range_t *va_range;
    uvm_va_range_t *last_range = NULL;
    NvU64 range_end = base + length - 1;

    uvm_assert_rwsem_locked(&va_space->lock);

    // Iterate over contiguous VA ranges only, gaps will cause early
    // termination.
    for (va_range = uvm_va_space_iter_first(va_space, base, range_end);
         va_range != NULL;
         va_range = uvm_va_space_iter_next_contig(va_range, range_end)) {

        NvBool is_range_managed = (va_range->type == UVM_VA_RANGE_TYPE_MANAGED);

        // All ranges must match the expected type
        if (is_range_managed != is_managed_memory)
            return NV_ERR_INVALID_ADDRESS;

        last_range = va_range;
    }

    // No VA ranges found covering the queried region.
    // For managed memory: error (user claims managed but no ranges exist).
    // For non-managed memory: OK (pure system memory handled by move_pages).
    if (!last_range)
        return is_managed_memory ? NV_ERR_INVALID_ADDRESS : NV_OK;

    // Verify the range is fully covered: first range must start at or before
    // base, last range must end at or after range_end
    va_range = uvm_va_space_iter_first(va_space, base, range_end);
    if (!va_range || va_range->node.start > base || last_range->node.end < range_end)
        return NV_ERR_INVALID_ADDRESS;

    return NV_OK;
}

// TODO: Bug 5687193: Investigate performance optimizations for query residency
NV_STATUS uvm_api_query_residency(UVM_QUERY_RESIDENCY_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    NV_STATUS status = NV_OK;
    void **pages = NULL;
    int *pageStatus = NULL;
    NvU64 i;
    NvU32 *processor_histogram = NULL;
    NvU32 *numa_node_histogram = NULL;
    struct mm_struct *mm = NULL;
    uvm_processor_id_t max_processor = UVM_ID_INVALID;
    NvU32 max_count = 0;
    NvU32 max_numa_node = 0;
    NvU32 max_numa_count = 0;

    // Validate input parameters (includes overflow check)
    if (uvm_api_range_invalid(params->base, params->length))
        return NV_ERR_INVALID_ADDRESS;

    if (params->samplingStride == 0)
        return NV_ERR_INVALID_ARGUMENT;

    if (!PAGE_ALIGNED(params->samplingStride))
        return NV_ERR_INVALID_ARGUMENT;

    if (params->numSamples == 0)
        return NV_ERR_INVALID_ARGUMENT;

    // Allocate histograms
    processor_histogram = uvm_kvmalloc_zero(UVM_ID_MAX_PROCESSORS * sizeof(*processor_histogram));
    if (!processor_histogram)
        return NV_ERR_NO_MEMORY;

    // Allocate NUMA histogram for tracking per-node residency
    // nr_node_ids is always >= 1 (even single-node systems have node 0)
    numa_node_histogram = uvm_kvmalloc_zero(nr_node_ids * sizeof(*numa_node_histogram));
    if (!numa_node_histogram) {
        status = NV_ERR_NO_MEMORY;
        goto cleanup;
    }

    // Copy the page arrays from userspace
    pages = uvm_kvmalloc(params->numSamples * sizeof(void *));
    if (!pages) {
        status = NV_ERR_NO_MEMORY;
        goto cleanup;
    }

    pageStatus = uvm_kvmalloc(params->numSamples * sizeof(int));
    if (!pageStatus) {
        status = NV_ERR_NO_MEMORY;
        goto cleanup;
    }

    // Use NV_ERR_INVALID_ARGUMENT for copy_from_user failures to align with
    // userspace UvmErrnoToNvStatus().
    if (copy_from_user(pages, (void *)params->pageAddresses, params->numSamples * sizeof(void *))) {
        status = NV_ERR_INVALID_ARGUMENT;
        goto cleanup;
    }

    if (copy_from_user(pageStatus, (void *)params->pageStatus, params->numSamples * sizeof(int))) {
        status = NV_ERR_INVALID_ARGUMENT;
        goto cleanup;
    }

    mm = uvm_va_space_mm_or_current_retain_lock(va_space);
    uvm_va_space_down_read(va_space);

    // Validate range homogeneity: the entire range must be consistently
    // managed or non-managed to ensure correct residency tracking
    status = query_residency_validate_range_homogeneity(va_space,
                                                        params->base,
                                                        params->length,
                                                        params->isManagedMemory);
    if (status != NV_OK)
        goto unlock;

    if (params->isManagedMemory) {
        // FAST PATH: All pages are invalid from move_pages()
        // This is UVM-managed memory - walk VA blocks directly
        status = query_residency_managed(va_space,
                                         mm,
                                         params->base,
                                         params->length,
                                         params->samplingStride,
                                         processor_histogram,
                                         numa_node_histogram);
        if (status != NV_OK)
            goto unlock;
    } else {
        // SLOW PATH: Build histogram from move_pages() results with 50% optimization
        NvU32 valid_cpu_pages = 0;
        NvU32 total_samples = params->numSamples;
        uvm_processor_mask_t *scratch_mask;

        scratch_mask = uvm_processor_mask_cache_alloc();
        if (!scratch_mask) {
            status = NV_ERR_NO_MEMORY;
            goto unlock;
        }

        // STEP 1: Count pages from move_pages() results
        // On coherent systems with NUMA-enabled GPUs, move_pages() returns
        // the GPU's NUMA node ID for pages in GPU memory. We must check if
        // each node belongs to a GPU to correctly attribute residency.
        for (i = 0; i < total_samples; i++) {
            int nid = pageStatus[i];
            if (nid >= 0) {
                uvm_gpu_t *gpu = uvm_va_space_find_gpu_with_memory_node_id(va_space, nid);

                if (gpu) {
                    // This is a GPU NUMA node - attribute to that GPU
                    processor_histogram[uvm_id_value(gpu->id)]++;
                } else {
                    // This is a CPU NUMA node
                    valid_cpu_pages++;
                    processor_histogram[uvm_id_value(UVM_ID_CPU)]++;
                }

                // Track NUMA histogram regardless of processor type
                if (numa_node_histogram && nid < nr_node_ids)
                    numa_node_histogram[nid]++;
            }
        }

        // STEP 2: Check for 50% optimization (non-coherent systems)
        // If CPU has > 50% of pages, we can skip UVM queries
        if (valid_cpu_pages > (total_samples / 2)) {
            // CPU wins - no need to query UVM for failed pages
            // Histogram already updated above
        } else {
            // CPU has ≤ 50% - must query ALL failed pages to find global
            // maximum
            for (i = 0; i < total_samples; i++) {
                if (pageStatus[i] < 0) {
                    // This page failed in move_pages() - query UVM
                    void *addr = pages[i];

                    status = query_residency_single_page(va_space,
                                                        mm,
                                                        (NvU64)addr,
                                                        scratch_mask,
                                                        processor_histogram,
                                                        numa_node_histogram);
                    if (status != NV_OK) {
                        uvm_processor_mask_cache_free(scratch_mask);
                        goto unlock;
                    }
                }
            }
        }

        uvm_processor_mask_cache_free(scratch_mask);
    }

    // Find processor with most pages. On ties, prefer lowest ordinal GPU over
    // CPU. Check GPUs first (starting from GPU0) so lowest ordinal GPU wins
    // ties.
    for (i = UVM_ID_GPU0_VALUE; i < UVM_ID_MAX_PROCESSORS; i++) {
        if (processor_histogram[i] > max_count) {
            max_count = processor_histogram[i];
            max_processor = uvm_id_from_value(i);
        }
    }

    // Check CPU last, it only wins if it has strictly more pages than any GPU.
    if (processor_histogram[UVM_ID_CPU_VALUE] > max_count) {
        max_count = processor_histogram[UVM_ID_CPU_VALUE];
        max_processor = UVM_ID_CPU;
    }

    // If no processor has any pages (unpopulated memory), return zero UUID
    // This is valid for mapped but unpopulated memory ranges
    if (!UVM_ID_IS_VALID(max_processor)) {
        memset(&params->residency, 0, sizeof(params->residency));
        params->resident_nid = NUMA_NO_NODE;
        goto unlock;
    }

    // Map processor ID to UUID
    uvm_processor_get_uuid(max_processor, &params->residency);

    // Handle NUMA node based on system type and winner
    // Skip NUMA node tracking if histogram wasn't allocated (NUMA disabled)
    if (!numa_node_histogram) {
        params->resident_nid = NUMA_NO_NODE;
    } else if (UVM_ID_IS_CPU(max_processor)) {
        // CPU won - return CPU NUMA node with most pages
        // Skip GPU NUMA nodes (coherent systems have GPU memory as NUMA nodes)
        for (i = 0; i < nr_node_ids; i++) {
            if (numa_node_histogram[i] > max_numa_count &&
                !uvm_va_space_memory_node_is_gpu(va_space, i)) {
                max_numa_count = numa_node_histogram[i];
                max_numa_node = i;
            }
        }

        params->resident_nid = (max_numa_count > 0) ? (NvS32)max_numa_node : NUMA_NO_NODE;
    } else {
        // GPU won, return appropriate NUMA node based on coherency
        uvm_gpu_t *gpu = uvm_gpu_get(max_processor);

        UVM_ASSERT(UVM_ID_IS_GPU(max_processor));
        if (gpu) {
            if (gpu->mem_info.numa.enabled) {
                // Coherent GPU - return GPU's NUMA node
                params->resident_nid = (NvS32)uvm_gpu_numa_node(gpu);
            } else {
                // Non-coherent GPU - return NUMA_NO_NODE
                params->resident_nid = NUMA_NO_NODE;
            }
        } else {
            params->resident_nid = NUMA_NO_NODE;
        }
    }

unlock:
    uvm_va_space_up_read(va_space);
    uvm_va_space_mm_or_current_release_unlock(va_space, mm);

cleanup:
    uvm_kvfree(pages);
    uvm_kvfree(pageStatus);
    uvm_kvfree(processor_histogram);
    uvm_kvfree(numa_node_histogram);

    return status;
}

static NV_STATUS va_range_add_gpu_va_space_managed(uvm_va_range_managed_t *managed_range,
                                                   uvm_gpu_va_space_t *gpu_va_space,
                                                   struct mm_struct *mm)
{
    uvm_va_space_t *va_space = managed_range->va_range.va_space;
    uvm_gpu_t *gpu = gpu_va_space->gpu;
    NV_STATUS status = NV_OK;
    const bool should_add_remote_mappings =
        uvm_processor_mask_test(&managed_range->policy.accessed_by, gpu->id) ||
        gpu->parent->is_integrated_gpu;

    // Combine conditions to perform a single VA block traversal
    if (gpu_va_space->ats.enabled || should_add_remote_mappings) {
        uvm_va_block_t *va_block;
        uvm_va_block_context_t *va_block_context = uvm_va_space_block_context(va_space, mm);


        // TODO: Bug 2090378. Consolidate all per-VA block operations within
        // uvm_va_block_add_gpu_va_space so we only need to take the VA block
        // once.
        for_each_va_block_in_va_range(managed_range, va_block) {
            if (gpu_va_space->ats.enabled) {
                // Notify that a new GPU VA space has been created. This is only
                // currently used for PDE1 pre-population on ATS systems.
                status = UVM_VA_BLOCK_LOCK_RETRY(va_block, NULL, uvm_va_block_add_gpu_va_space(va_block, gpu_va_space));
                if (status != NV_OK)
                    break;
            }

            if (should_add_remote_mappings) {
                // Now that we have a GPU VA space, map any VA ranges for which
                // this GPU has accessed_by set.
                status = uvm_va_block_set_accessed_by(va_block, va_block_context, gpu->id);
                if (status != NV_OK)
                    break;
            }
        }
    }

    return status;
}

static NV_STATUS va_range_add_gpu_va_space_semaphore_pool(uvm_va_range_semaphore_pool_t *semaphore_pool_range,
                                                          uvm_gpu_t *gpu)
{
    uvm_mem_gpu_mapping_attrs_t *attrs;

    UVM_ASSERT(uvm_mem_mapped_on_gpu_kernel(semaphore_pool_range->mem, gpu));

    attrs = &semaphore_pool_range->gpu_attrs[uvm_id_gpu_index(gpu->id)];

    return uvm_mem_map_gpu_user(semaphore_pool_range->mem,
                                gpu,
                                semaphore_pool_range->va_range.va_space,
                                (void *)semaphore_pool_range->va_range.node.start,
                                attrs);
}

NV_STATUS uvm_va_range_add_gpu_va_space(uvm_va_range_t *va_range,
                                        uvm_gpu_va_space_t *gpu_va_space,
                                        struct mm_struct *mm)
{
    UVM_ASSERT(va_range->type < UVM_VA_RANGE_TYPE_MAX);

    if (va_range->inject_add_gpu_va_space_error) {
        va_range->inject_add_gpu_va_space_error = false;
        return NV_ERR_NO_MEMORY;
    }

    switch (va_range->type) {
        case UVM_VA_RANGE_TYPE_MANAGED:
            return va_range_add_gpu_va_space_managed(uvm_va_range_to_managed(va_range), gpu_va_space, mm);
        case UVM_VA_RANGE_TYPE_SEMAPHORE_POOL:
            return va_range_add_gpu_va_space_semaphore_pool(uvm_va_range_to_semaphore_pool(va_range),
                                                            gpu_va_space->gpu);
        default:
            return NV_OK;
    }
}

static void va_range_remove_gpu_va_space_managed(uvm_va_range_managed_t *managed_range,
                                                 uvm_gpu_va_space_t *gpu_va_space,
                                                 struct mm_struct *mm)
{
    uvm_va_block_t *va_block;
    uvm_va_space_t *va_space = managed_range->va_range.va_space;
    uvm_va_block_context_t *va_block_context = uvm_va_space_block_context(va_space, mm);

    for_each_va_block_in_va_range(managed_range, va_block) {
        uvm_mutex_lock(&va_block->lock);
        uvm_va_block_remove_gpu_va_space(va_block, gpu_va_space, va_block_context);
        uvm_mutex_unlock(&va_block->lock);
    }
}

static void va_range_remove_gpu_va_space_external(uvm_va_range_external_t *external_range,
                                                  uvm_gpu_t *gpu,
                                                  struct list_head *deferred_free_list)
{
    uvm_ext_gpu_range_tree_t *range_tree;
    uvm_ext_gpu_map_t *ext_map, *ext_map_next;

    UVM_ASSERT(deferred_free_list);

    range_tree = uvm_ext_gpu_range_tree(external_range, gpu);
    uvm_mutex_lock(&range_tree->lock);

    uvm_ext_gpu_map_for_each_safe(ext_map, ext_map_next, external_range, gpu)
        uvm_ext_gpu_map_destroy(external_range, ext_map, deferred_free_list);

    uvm_mutex_unlock(&range_tree->lock);
}

static void va_range_remove_gpu_va_space_dma_buf(uvm_va_range_dma_buf_t *dmabuf_range,
                                                 uvm_gpu_t *gpu,
                                                 struct list_head *deferred_free_list)
{
    uvm_dma_buf_gpu_range_tree_deinit(dmabuf_range, gpu, deferred_free_list);
}

static void va_range_remove_gpu_va_space_semaphore_pool(uvm_va_range_semaphore_pool_t *semaphore_pool_range,
                                                        uvm_gpu_t *gpu)
{

    if (g_uvm_global.conf_computing_enabled && (semaphore_pool_range->mem->dma_owner == gpu))
        uvm_va_range_destroy(&semaphore_pool_range->va_range, NULL);
    else
        uvm_mem_unmap_gpu_user(semaphore_pool_range->mem, gpu);
}

void uvm_va_range_remove_gpu_va_space(uvm_va_range_t *va_range,
                                      uvm_gpu_va_space_t *gpu_va_space,
                                      struct mm_struct *mm,
                                      struct list_head *deferred_free_list)
{
    switch (va_range->type) {
        case UVM_VA_RANGE_TYPE_MANAGED:
            va_range_remove_gpu_va_space_managed(uvm_va_range_to_managed(va_range), gpu_va_space, mm);
            break;
        case UVM_VA_RANGE_TYPE_EXTERNAL:
            va_range_remove_gpu_va_space_external(uvm_va_range_to_external(va_range),
                                                  gpu_va_space->gpu,
                                                  deferred_free_list);
            break;
        case UVM_VA_RANGE_TYPE_DMA_BUF:
#if !UVM_PROVIDES_DMA_BUF_IMPORTER()
            UVM_ASSERT_MSG(0, "[0x%llx, 0x%llx] unexpected DMA_BUF range\n",
                           va_range->node.start, va_range->node.end);
#endif
            va_range_remove_gpu_va_space_dma_buf(uvm_va_range_to_dma_buf(va_range),
                                                 gpu_va_space->gpu,
                                                 deferred_free_list);
            break;
        case UVM_VA_RANGE_TYPE_CHANNEL:
            // All channels under this GPU VA space should've been removed before
            // removing the GPU VA space.
            UVM_ASSERT(uvm_va_range_to_channel(va_range)->gpu_va_space != gpu_va_space);
            break;
        case UVM_VA_RANGE_TYPE_SKED_REFLECTED:
            if (uvm_va_range_to_sked_reflected(va_range)->gpu_va_space == gpu_va_space)
                uvm_va_range_deinit_sked_reflected(uvm_va_range_to_sked_reflected(va_range));
            break;
        case UVM_VA_RANGE_TYPE_SEMAPHORE_POOL:
            va_range_remove_gpu_va_space_semaphore_pool(uvm_va_range_to_semaphore_pool(va_range),
                                                        gpu_va_space->gpu);
            break;
        case UVM_VA_RANGE_TYPE_DEVICE_P2P:
            // Device P2P ranges are associated with a specific GPU so destroy
            // the range entirely if unregistering the associated GPU.
            if (uvm_va_range_to_device_p2p(va_range)->gpu == gpu_va_space->gpu) {
                unmap_mapping_range(va_range->va_space->mapping, va_range->node.start, uvm_va_range_size(va_range), 1);
                uvm_va_range_deinit_device_p2p(uvm_va_range_to_device_p2p(va_range), deferred_free_list);
            }
            break;
        default:
            UVM_ASSERT_MSG(0, "[0x%llx, 0x%llx] has type %d\n",
                           va_range->node.start, va_range->node.end, va_range->type);
    }
}

static NV_STATUS uvm_va_range_enable_peer_managed(uvm_va_range_managed_t *managed_range,
                                                  uvm_gpu_t *gpu0,
                                                  uvm_gpu_t *gpu1)
{
    NV_STATUS status;
    uvm_va_block_t *va_block;
    bool gpu0_accessed_by = uvm_processor_mask_test(&managed_range->policy.accessed_by, gpu0->id);
    bool gpu1_accessed_by = uvm_processor_mask_test(&managed_range->policy.accessed_by, gpu1->id);
    uvm_va_space_t *va_space = managed_range->va_range.va_space;
    uvm_va_block_context_t *va_block_context = uvm_va_space_block_context(va_space, NULL);


    for_each_va_block_in_va_range(managed_range, va_block) {
        if (gpu0_accessed_by) {
            status = uvm_va_block_set_accessed_by(va_block,
                                                  va_block_context,
                                                  gpu0->id);
            if (status != NV_OK)
                return status;
        }

        if (gpu1_accessed_by) {
            status = uvm_va_block_set_accessed_by(va_block,
                                                  va_block_context,
                                                  gpu1->id);
            if (status != NV_OK)
                return status;
        }
    }

    return NV_OK;
}

NV_STATUS uvm_va_range_enable_peer(uvm_va_range_t *va_range, uvm_gpu_t *gpu0, uvm_gpu_t *gpu1)
{
    switch (va_range->type) {
        case UVM_VA_RANGE_TYPE_MANAGED:
            return uvm_va_range_enable_peer_managed(uvm_va_range_to_managed(va_range), gpu0, gpu1);
        case UVM_VA_RANGE_TYPE_EXTERNAL:
            // UVM_VA_RANGE_TYPE_EXTERNAL doesn't create new mappings when enabling peer access
            return NV_OK;
        case UVM_VA_RANGE_TYPE_DMA_BUF:
            // UVM_VA_RANGE_TYPE_DMA_BUF should never have peer mappings
            return NV_OK;
        case UVM_VA_RANGE_TYPE_CHANNEL:
            // UVM_VA_RANGE_TYPE_CHANNEL should never have peer mappings
            return NV_OK;
        case UVM_VA_RANGE_TYPE_SKED_REFLECTED:
            // UVM_VA_RANGE_TYPE_SKED_REFLECTED should never have peer mappings
            return NV_OK;
        case UVM_VA_RANGE_TYPE_SEMAPHORE_POOL:
            // UVM_VA_RANGE_TYPE_SEMAPHORE_POOL should never have peer mappings
            return NV_OK;
        default:
            UVM_ASSERT_MSG(0, "[0x%llx, 0x%llx] has type %d\n",
                           va_range->node.start, va_range->node.end, va_range->type);
            return NV_ERR_NOT_SUPPORTED;
    }
}

static void uvm_va_range_disable_peer_external(uvm_va_range_external_t *external_range,
                                               uvm_gpu_t *mapping_gpu,
                                               uvm_gpu_t *owning_gpu,
                                               struct list_head *deferred_free_list)
{
    uvm_ext_gpu_range_tree_t *range_tree;
    uvm_ext_gpu_map_t *ext_map, *ext_map_next;

    range_tree = uvm_ext_gpu_range_tree(external_range, mapping_gpu);
    uvm_mutex_lock(&range_tree->lock);
    uvm_ext_gpu_map_for_each_safe(ext_map, ext_map_next, external_range, mapping_gpu) {
        if (ext_map->owning_gpu == owning_gpu && (!ext_map->is_sysmem || ext_map->is_egm)) {
            UVM_ASSERT(deferred_free_list);
            uvm_ext_gpu_map_destroy(external_range, ext_map, deferred_free_list);
        }
    }
    uvm_mutex_unlock(&range_tree->lock);
}

static void uvm_va_range_disable_peer_managed(uvm_va_range_managed_t *managed_range, uvm_gpu_t *gpu0, uvm_gpu_t *gpu1)
{
    uvm_va_block_t *va_block;

    for_each_va_block_in_va_range(managed_range, va_block) {
        uvm_mutex_lock(&va_block->lock);
        uvm_va_block_disable_peer(va_block, gpu0, gpu1);
        uvm_mutex_unlock(&va_block->lock);
    }
}

void uvm_va_range_disable_peer(uvm_va_range_t *va_range,
                               uvm_gpu_t *gpu0,
                               uvm_gpu_t *gpu1,
                               struct list_head *deferred_free_list)
{

    switch (va_range->type) {
        case UVM_VA_RANGE_TYPE_MANAGED:
            uvm_va_range_disable_peer_managed(uvm_va_range_to_managed(va_range), gpu0, gpu1);
            break;
        case UVM_VA_RANGE_TYPE_EXTERNAL:
            // If GPU 0 has a mapping to GPU 1, remove GPU 0's mapping
            uvm_va_range_disable_peer_external(uvm_va_range_to_external(va_range), gpu0, gpu1, deferred_free_list);
            // If GPU 1 has a mapping to GPU 0, remove GPU 1's mapping
            uvm_va_range_disable_peer_external(uvm_va_range_to_external(va_range), gpu1, gpu0, deferred_free_list);
            break;
        case UVM_VA_RANGE_TYPE_DMA_BUF:
            // UVM_VA_RANGE_TYPE_DMA_BUF should never have peer mappings
            break;
        case UVM_VA_RANGE_TYPE_CHANNEL:
            // UVM_VA_RANGE_TYPE_CHANNEL should never have peer mappings
            break;
        case UVM_VA_RANGE_TYPE_SKED_REFLECTED:
            // UVM_VA_RANGE_TYPE_SKED_REFLECTED should never have peer mappings
            break;
        case UVM_VA_RANGE_TYPE_SEMAPHORE_POOL:
            // UVM_VA_RANGE_TYPE_SEMAPHORE_POOL should never have peer mappings
            break;
        case UVM_VA_RANGE_TYPE_DEVICE_P2P:
            // UVM_VA_RANGE_TYPE_DEVICE_P2P should never have peer mappings
            break;
        default:
            UVM_ASSERT_MSG(0, "[0x%llx, 0x%llx] has type %d\n",
                           va_range->node.start, va_range->node.end, va_range->type);
    }
}

static NV_STATUS va_range_register_gpu_semaphore_pool(uvm_va_range_semaphore_pool_t *semaphore_pool_range,
                                                      uvm_gpu_t *gpu)
{
    // TODO: Bug 1812419: pass GPU mapping attributes to uvm_mem_map_gpu_kernel
    // once that function accepts them.
    return uvm_mem_map_gpu_kernel(semaphore_pool_range->mem, gpu);
}

NV_STATUS uvm_va_range_register_gpu(uvm_va_range_t *va_range, uvm_gpu_t *gpu)
{
    UVM_ASSERT(va_range->type < UVM_VA_RANGE_TYPE_MAX);
    uvm_assert_rwsem_locked_write(&va_range->va_space->lock);

    if (va_range->type == UVM_VA_RANGE_TYPE_SEMAPHORE_POOL)
        return va_range_register_gpu_semaphore_pool(uvm_va_range_to_semaphore_pool(va_range), gpu);

    return NV_OK;
}

static void va_range_unregister_gpu_managed(uvm_va_range_managed_t *managed_range, uvm_gpu_t *gpu, struct mm_struct *mm)
{
    uvm_va_block_t *va_block;

    // Reset preferred location and accessed-by of managed ranges if needed
    // Note: ignoring the return code of uvm_va_range_set_preferred_location since this
    // will only return on error when setting a preferred location, not on a reset
    if (uvm_va_policy_preferred_location_equal(&managed_range->policy, gpu->id, NUMA_NO_NODE))
        (void)uvm_va_range_set_preferred_location(managed_range, UVM_ID_INVALID, NUMA_NO_NODE, mm, NULL);

    uvm_va_range_unset_accessed_by(managed_range, gpu->id, NULL);

    // Migrate and free any remaining resident allocations on this GPU
    for_each_va_block_in_va_range(managed_range, va_block)
        uvm_va_block_unregister_gpu(va_block, gpu, mm);
}

// The GPU being unregistered can't have any remaining mappings, since those
// were removed when the corresponding GPU VA space was removed. However, other
// GPUs could still have mappings to memory resident on this GPU, so we have to
// unmap those.
static void va_range_unregister_gpu_external(uvm_va_range_external_t *external_range,
                                             uvm_gpu_t *gpu,
                                             struct list_head *deferred_free_list)
{
    uvm_ext_gpu_map_t *ext_map, *ext_map_next;
    uvm_gpu_t *other_gpu;

    for_each_va_space_gpu_in_mask(other_gpu, external_range->va_range.va_space, &external_range->mapped_gpus) {
        uvm_ext_gpu_range_tree_t *range_tree = uvm_ext_gpu_range_tree(external_range, other_gpu);
        UVM_ASSERT(other_gpu != gpu);

        uvm_mutex_lock(&range_tree->lock);
        uvm_ext_gpu_map_for_each_safe(ext_map, ext_map_next, external_range, other_gpu) {
            if (ext_map->owning_gpu == gpu) {
                UVM_ASSERT(deferred_free_list);
                uvm_ext_gpu_map_destroy(external_range, ext_map, deferred_free_list);
            }
        }
        uvm_mutex_unlock(&range_tree->lock);
    }
}

static void va_range_unregister_gpu_semaphore_pool(uvm_va_range_semaphore_pool_t *semaphore_pool_range,
                                                   uvm_gpu_t *gpu)
{
    NV_STATUS status;

    // Ranges for this GPU should have been previously unmapped from the user VA
    // space during GPU VA space unregister, which should have already happened.
    UVM_ASSERT(!uvm_mem_mapped_on_gpu_user(semaphore_pool_range->mem, gpu));
    UVM_ASSERT(uvm_mem_mapped_on_gpu_kernel(semaphore_pool_range->mem, gpu));

    uvm_mutex_lock(&semaphore_pool_range->tracker_lock);
    status = uvm_tracker_wait(&semaphore_pool_range->tracker);
    uvm_mutex_unlock(&semaphore_pool_range->tracker_lock);
    if (status != NV_OK)
        UVM_ASSERT(status == uvm_global_get_status());

    uvm_mem_unmap_gpu_phys(semaphore_pool_range->mem, gpu);

    semaphore_pool_range->gpu_attrs[uvm_id_gpu_index(gpu->id)] = semaphore_pool_range->default_gpu_attrs;
    if (semaphore_pool_range->owner == gpu)
        semaphore_pool_range->owner = NULL;
}

void uvm_va_range_unregister_gpu(uvm_va_range_t *va_range,
                                 uvm_gpu_t *gpu,
                                 struct mm_struct *mm,
                                 struct list_head *deferred_free_list)
{
    switch (va_range->type) {
        case UVM_VA_RANGE_TYPE_MANAGED:
            va_range_unregister_gpu_managed(uvm_va_range_to_managed(va_range), gpu, mm);
            break;
        case UVM_VA_RANGE_TYPE_EXTERNAL:
            va_range_unregister_gpu_external(uvm_va_range_to_external(va_range), gpu, deferred_free_list);
            break;
        case UVM_VA_RANGE_TYPE_DMA_BUF:
#if !UVM_PROVIDES_DMA_BUF_IMPORTER()
            UVM_ASSERT_MSG(0, "[0x%llx, 0x%llx] unexpected DMA_BUF range\n",
                           va_range->node.start, va_range->node.end);
#endif
            // All ranges for this GPU should have been unmapped by GPU VA space
            // unregister (va_range_remove_gpu_va_space_dma_buf), which should
            // have already happened.
            UVM_ASSERT(!uvm_processor_mask_test(&uvm_va_range_to_dma_buf(va_range)->mapped_gpus, gpu->id));
            UVM_ASSERT(uvm_range_tree_empty(
                    &uvm_dma_buf_gpu_range_tree(uvm_va_range_to_dma_buf(va_range), gpu)->base.tree));
            break;
        case UVM_VA_RANGE_TYPE_CHANNEL:
            // All ranges should have been destroyed by GPU VA space unregister,
            // which should have already happened.
            UVM_ASSERT(uvm_va_range_to_channel(va_range)->gpu_va_space->gpu != gpu);
            break;
        case UVM_VA_RANGE_TYPE_SKED_REFLECTED:
            // All ranges for this GPU should have been unmapped by GPU VA space
            // unregister (uvm_va_range_deinit_sked_reflected), which should
            // have already happened.
            if (uvm_va_range_to_sked_reflected(va_range)->gpu_va_space != NULL)
                UVM_ASSERT(uvm_va_range_to_sked_reflected(va_range)->gpu_va_space->gpu != gpu);
            break;
        case UVM_VA_RANGE_TYPE_SEMAPHORE_POOL:
            va_range_unregister_gpu_semaphore_pool(uvm_va_range_to_semaphore_pool(va_range), gpu);
            break;
        case UVM_VA_RANGE_TYPE_DEVICE_P2P:
            // All ranges for this GPU should have been deinited by GPU VA space
            // unregister, which should have already happened.
            if (uvm_va_range_to_device_p2p(va_range)->p2p_mem != NULL)
                UVM_ASSERT(uvm_va_range_to_device_p2p(va_range)->gpu != gpu);
            break;
        default:
            UVM_ASSERT_MSG(0, "[0x%llx, 0x%llx] has type %d\n",
                           va_range->node.start, va_range->node.end, va_range->type);
    }
}

// Split existing's blocks into new. new's blocks array has already been
// allocated. This is called before existing's range node is split, so it
// overlaps new. new is always in the upper region of existing.
//
// The caller will do the range tree split.
//
// If this fails it leaves existing unchanged.
static NV_STATUS uvm_va_range_split_blocks(uvm_va_range_managed_t *existing, uvm_va_range_managed_t *new)
{
    uvm_va_block_t *old_block, *block = NULL;
    size_t existing_blocks, split_index, new_index = 0;
    NV_STATUS status;

    UVM_ASSERT(new->va_range.node.start >  existing->va_range.node.start);
    UVM_ASSERT(new->va_range.node.end   <= existing->va_range.node.end);

    split_index = uvm_va_range_block_index(existing, new->va_range.node.start);

    // Handle a block spanning the split point
    if (block_calc_start(existing, split_index) != new->va_range.node.start) {
        // If a populated block actually spans the split point, we have to split
        // the block. Otherwise just account for the extra entry in the arrays.
        old_block = uvm_va_range_block(existing, split_index);
        if (old_block) {
            UVM_ASSERT(old_block->start < new->va_range.node.start);
            status = uvm_va_block_split(old_block, new->va_range.node.start - 1, &block, new);
            if (status != NV_OK)
                return status;

            // No memory barrier is needed since we're holding the va_space lock in
            // write mode, so no other thread can access the blocks array.
            atomic_long_set(&new->blocks[0], (long)block);
        }

        new_index = 1;
    }

    // uvm_va_block_split gets first crack at injecting an error. If it did so,
    // we wouldn't be here. However, not all va_range splits will call
    // uvm_va_block_split so we need an extra check here. We can't push this
    // injection later since all paths past this point assume success, so they
    // modify the state of 'existing' range.
    //
    // Even if there was no block split above, there is no guarantee that one
    // of our blocks doesn't have the 'inject_split_error' flag set. We clear
    // that here to prevent multiple errors caused by one
    // 'uvm_test_va_range_inject_split_error' call.
    if (existing->inject_split_error) {
        UVM_ASSERT(!block);
        existing->inject_split_error = false;

        for_each_va_block_in_va_range(existing, block) {
            uvm_va_block_test_t *block_test = uvm_va_block_get_test(block);
            if (block_test)
                block_test->inject_split_error = false;
        }

        return NV_ERR_NO_MEMORY;
    }

    existing_blocks = split_index + new_index;

    // Copy existing's blocks over to the new range, accounting for the explicit
    // assignment above in case we did a block split. There are two general
    // cases:
    //
    // No split:
    //                             split_index
    //                                  v
    //  existing (before) [----- A ----][----- B ----][----- C ----]
    //  existing (after)  [----- A ----]
    //  new                             [----- B ----][----- C ----]
    //
    // Split:
    //                                    split_index
    //                                         v
    //  existing (before) [----- A ----][----- B ----][----- C ----]
    //  existing (after   [----- A ----][- B -]
    //  new                                    [- N -][----- C ----]
    //                                            ^new->blocks[0]

    // Note, if we split the last block of existing, this won't iterate at all.
    for (; new_index < uvm_va_range_num_blocks(new); new_index++) {
        block = uvm_va_range_block(existing, split_index + new_index);
        if (!block) {
            // new's array was cleared at allocation
            UVM_ASSERT(uvm_va_range_block(new, new_index) == NULL);
            continue;
        }

        // As soon as we make this assignment and drop the lock, the reverse
        // mapping code can start looking at new, so new must be ready to go.
        uvm_mutex_lock(&block->lock);
        UVM_ASSERT(block->managed_range == existing);
        block->managed_range = new;
        uvm_mutex_unlock(&block->lock);

        // No memory barrier is needed since we're holding the va_space lock in
        // write mode, so no other thread can access the blocks array.
        atomic_long_set(&new->blocks[new_index], (long)block);
        atomic_long_set(&existing->blocks[split_index + new_index], (long)NULL);
    }

    blocks_array_shrink(existing, existing_blocks);

    return NV_OK;
}

NV_STATUS uvm_va_range_split(uvm_va_range_managed_t *existing_managed_range,
                             NvU64 new_end,
                             uvm_va_range_managed_t **new_managed_range_out)
{
    uvm_va_space_t *va_space = existing_managed_range->va_range.va_space;
    uvm_va_policy_t *new_policy, *existing_policy;
    uvm_va_range_managed_t *new = NULL;
    uvm_perf_event_data_t event_data;
    NV_STATUS status;

    UVM_ASSERT(new_end > existing_managed_range->va_range.node.start);
    UVM_ASSERT(new_end < existing_managed_range->va_range.node.end);
    UVM_ASSERT(PAGE_ALIGNED(new_end + 1));
    uvm_assert_rwsem_locked_write(&va_space->lock);

    new = uvm_va_range_alloc_managed(va_space, new_end + 1, existing_managed_range->va_range.node.end);
    if (!new)
        return NV_ERR_NO_MEMORY;

    // The new managed_range is under the same vma. If this is a uvm_vm_open,
    // the caller takes care of updating existing's vma_wrapper for us.
    new->vma_wrapper = existing_managed_range->vma_wrapper;

    // Copy over state before splitting blocks so any block lookups happening
    // concurrently on the eviction path will see the new range's data.
    new_policy = &new->policy;
    existing_policy = &existing_managed_range->policy;
    new_policy->read_duplication = existing_policy->read_duplication;
    new_policy->preferred_location = existing_policy->preferred_location;
    new_policy->preferred_nid = existing_policy->preferred_nid;
    uvm_processor_mask_copy(&new_policy->accessed_by,
                            &existing_policy->accessed_by);

    status = uvm_va_range_split_blocks(existing_managed_range, new);
    if (status != NV_OK) {
        uvm_va_range_destroy(&new->va_range, NULL);
        return status;
    }

    // Finally, update the VA range tree
    uvm_range_tree_split(&va_space->va_range_tree, &existing_managed_range->va_range.node, &new->va_range.node);

    event_data.range_shrink.range = &new->va_range;
    uvm_perf_event_notify(&va_space->perf_events, UVM_PERF_EVENT_RANGE_SHRINK, &event_data);

    if (new_managed_range_out)
        *new_managed_range_out = new;
    return NV_OK;
}

uvm_va_range_t *uvm_va_range_find(uvm_va_space_t *va_space, NvU64 addr)
{
    uvm_assert_rwsem_locked(&va_space->lock);
    return uvm_va_range_container(uvm_range_tree_find(&va_space->va_range_tree, addr));
}

uvm_va_range_t *uvm_va_space_iter_first(uvm_va_space_t *va_space, NvU64 start, NvU64 end)
{
    uvm_assert_rwsem_locked(&va_space->lock);
    return uvm_va_range_container(uvm_range_tree_iter_first(&va_space->va_range_tree, start, end));
}

uvm_va_range_t *uvm_va_space_iter_gmmu_mappable_first(uvm_va_space_t *va_space, NvU64 start)
{
    uvm_va_range_t *va_range = uvm_va_range_container(uvm_range_tree_iter_first(&va_space->va_range_tree, start, ~0ULL));

    if (va_range && !uvm_va_range_is_gmmu_mappable(va_range))
        va_range = uvm_va_range_gmmu_mappable_next(va_range);

    return va_range;
}

uvm_va_range_t *uvm_va_space_iter_next(uvm_va_range_t *va_range, NvU64 end)
{
    uvm_va_space_t *va_space;

    // Handling a NULL va_range here makes uvm_for_each_va_range_in_safe much
    // less messy
    if (!va_range)
        return NULL;

    va_space = va_range->va_space;
    uvm_assert_rwsem_locked(&va_space->lock);
    return uvm_va_range_container(uvm_range_tree_iter_next(&va_space->va_range_tree, &va_range->node, end));
}

uvm_va_range_t *uvm_va_space_iter_prev(uvm_va_range_t *va_range, NvU64 start)
{
    uvm_va_space_t *va_space;

    UVM_ASSERT(va_range);

    va_space = va_range->va_space;
    uvm_assert_rwsem_locked(&va_space->lock);
    return uvm_va_range_container(uvm_range_tree_iter_prev(&va_space->va_range_tree, &va_range->node, start));
}

size_t uvm_va_range_num_blocks(uvm_va_range_managed_t *managed_range)
{
    NvU64 start = UVM_VA_BLOCK_ALIGN_DOWN(managed_range->va_range.node.start);
    NvU64 end   = UVM_VA_BLOCK_ALIGN_UP(managed_range->va_range.node.end); // End is inclusive
    return (end - start) / UVM_VA_BLOCK_SIZE;
}

size_t uvm_va_range_block_index(uvm_va_range_managed_t *managed_range, NvU64 addr)
{
    size_t addr_index, start_index, index;

    UVM_ASSERT(addr >= managed_range->va_range.node.start);
    UVM_ASSERT(addr <= managed_range->va_range.node.end);

    // Each block will cover as much space as possible within the aligned
    // UVM_VA_BLOCK_SIZE, up to the parent VA range boundaries. In other words,
    // the entire VA space can be broken into UVM_VA_BLOCK_SIZE chunks. Even if
    // there are multiple ranges (and thus multiple blocks) per actual
    // UVM_VA_BLOCK_SIZE chunk, none of those will have more than 1 block unless
    // they span a UVM_VA_BLOCK_SIZE alignment boundary.
    addr_index = (size_t)(addr / UVM_VA_BLOCK_SIZE);
    start_index = (size_t)(managed_range->va_range.node.start / UVM_VA_BLOCK_SIZE);

    index = addr_index - start_index;
    UVM_ASSERT(index < uvm_va_range_num_blocks(managed_range));
    return index;
}

NV_STATUS uvm_va_range_block_create(uvm_va_range_managed_t *managed_range, size_t index, uvm_va_block_t **out_block)
{
    uvm_va_block_t *block, *old;
    NV_STATUS status;

    block = uvm_va_range_block(managed_range, index);
    if (!block) {
        // No block has been created here yet, so allocate one and attempt to
        // insert it. Note that this runs the risk of an out-of-memory error
        // when multiple threads race and all concurrently allocate a block for
        // the same address. This should be extremely rare. There is also
        // precedent in the Linux kernel, which does the same thing for demand-
        // allocation of anonymous pages.
        status = uvm_va_block_create(managed_range,
                                     block_calc_start(managed_range, index),
                                     block_calc_end(managed_range, index),
                                     &block);
        if (status != NV_OK)
            return status;

        // Try to insert it
        old = (uvm_va_block_t *)atomic_long_cmpxchg(&managed_range->blocks[index], (long)NULL, (long)block);
        if (old) {
            // Someone else beat us on the insert
            uvm_va_block_release(block);
            block = old;
        }
    }

    *out_block = block;
    return NV_OK;
}

uvm_va_block_t *uvm_va_range_block_next(uvm_va_range_managed_t *managed_range, uvm_va_block_t *va_block)
{
    uvm_va_space_t *va_space = managed_range->va_range.va_space;
    size_t i = 0;

    uvm_assert_rwsem_locked(&va_space->lock);

    if (va_block)
        i = uvm_va_range_block_index(managed_range, va_block->start) + 1;

    for (; i < uvm_va_range_num_blocks(managed_range); i++) {
        va_block = uvm_va_range_block(managed_range, i);
        if (va_block) {
            UVM_ASSERT(va_block->managed_range == managed_range);
            UVM_ASSERT(uvm_va_range_block_index(managed_range, va_block->start) == i);
            return va_block;
        }
    }

    return NULL;
}

NV_STATUS uvm_va_range_set_preferred_location(uvm_va_range_managed_t *managed_range,
                                              uvm_processor_id_t preferred_location,
                                              int preferred_cpu_nid,
                                              struct mm_struct *mm,
                                              uvm_tracker_t *out_tracker)
{
    NV_STATUS status = NV_OK;
    uvm_processor_mask_t *set_accessed_by_processors = NULL;
    uvm_va_space_t *va_space = managed_range->va_range.va_space;
    uvm_va_block_t *va_block;
    uvm_va_block_context_t *va_block_context;
    uvm_va_policy_t *va_range_policy;

    uvm_assert_rwsem_locked_write(&va_space->lock);

    set_accessed_by_processors = uvm_processor_mask_cache_alloc();
    if (!set_accessed_by_processors) {
        status = NV_ERR_NO_MEMORY;
        goto out;
    }

    va_range_policy = &managed_range->policy;
    if (uvm_va_policy_preferred_location_equal(va_range_policy, preferred_location, preferred_cpu_nid))
        goto out;

    // The old preferred location should establish new remote mappings if it has
    // accessed-by set.
    if (UVM_ID_IS_VALID(va_range_policy->preferred_location))
        uvm_processor_mask_set(set_accessed_by_processors, va_range_policy->preferred_location);

    uvm_processor_mask_and(set_accessed_by_processors, set_accessed_by_processors, &va_range_policy->accessed_by);

    // Now update the managed_range state
    va_range_policy->preferred_location = preferred_location;
    va_range_policy->preferred_nid = preferred_cpu_nid;

    va_block_context = uvm_va_space_block_context(va_space, mm);

    for_each_va_block_in_va_range(managed_range, va_block) {
        uvm_processor_id_t id;
        uvm_va_block_region_t region = uvm_va_block_region_from_block(va_block);

        for_each_id_in_mask(id, set_accessed_by_processors) {
            status = uvm_va_block_set_accessed_by(va_block, va_block_context, id);
            if (status != NV_OK)
                goto out;
        }

        // Also, remove remote mappings from the new preferred location.
        uvm_mutex_lock(&va_block->lock);
        status = UVM_VA_BLOCK_RETRY_LOCKED(va_block,
                                           NULL,
                                           uvm_va_block_set_preferred_location_locked(va_block,
                                                                                      va_block_context,
                                                                                      region));

        if (out_tracker) {
            NV_STATUS tracker_status;

            tracker_status = uvm_tracker_add_tracker_safe(out_tracker, &va_block->tracker);
            if (status == NV_OK)
                status = tracker_status;
        }

        uvm_mutex_unlock(&va_block->lock);

        if (status != NV_OK)
            goto out;
    }

out:
    uvm_processor_mask_cache_free(set_accessed_by_processors);

    return status;
}

NV_STATUS uvm_va_range_set_accessed_by(uvm_va_range_managed_t *managed_range,
                                       uvm_processor_id_t processor_id,
                                       struct mm_struct *mm,
                                       uvm_tracker_t *out_tracker)
{
    NV_STATUS status = NV_OK;
    uvm_va_block_t *va_block;
    uvm_va_space_t *va_space = managed_range->va_range.va_space;
    uvm_va_policy_t *policy = &managed_range->policy;
    uvm_va_block_context_t *va_block_context = uvm_va_space_block_context(va_space, mm);

    uvm_processor_mask_set(&policy->accessed_by, processor_id);

    for_each_va_block_in_va_range(managed_range, va_block) {
        status = uvm_va_block_set_accessed_by(va_block, va_block_context, processor_id);
        if (status != NV_OK)
            break;
    }

    return status;
}

void uvm_va_range_unset_accessed_by(uvm_va_range_managed_t *managed_range,
                                    uvm_processor_id_t processor_id,
                                    uvm_tracker_t *out_tracker)
{
    uvm_processor_mask_clear(&managed_range->policy.accessed_by, processor_id);
}

NV_STATUS uvm_va_range_set_read_duplication(uvm_va_range_managed_t *managed_range, struct mm_struct *mm)
{
    uvm_va_block_t *va_block;
    uvm_va_block_context_t *va_block_context;

    if (managed_range->policy.read_duplication == UVM_READ_DUPLICATION_ENABLED)
        return NV_OK;

    va_block_context = uvm_va_space_block_context(managed_range->va_range.va_space, mm);

    for_each_va_block_in_va_range(managed_range, va_block) {
        NV_STATUS status = uvm_va_block_set_read_duplication(va_block, va_block_context);

        if (status != NV_OK)
            return status;
    }

    return NV_OK;
}

NV_STATUS uvm_va_range_unset_read_duplication(uvm_va_range_managed_t *managed_range, struct mm_struct *mm)
{
    uvm_va_block_t *va_block;
    uvm_va_block_context_t *va_block_context;
    NV_STATUS status;

    if (managed_range->policy.read_duplication == UVM_READ_DUPLICATION_DISABLED)
        return NV_OK;

    va_block_context = uvm_va_space_block_context(managed_range->va_range.va_space, mm);

    for_each_va_block_in_va_range(managed_range, va_block) {
        status = uvm_va_block_unset_read_duplication(va_block, va_block_context);

        if (status != NV_OK)
            return status;
    }

    return NV_OK;
}

NV_STATUS uvm_va_range_discard(uvm_va_range_managed_t *managed_range,
                               uvm_va_block_context_t *va_block_context,
                               NvU64 start,
                               NvU64 end,
                               NvU64 flags)
{
    size_t i;
    const size_t first_block_index = uvm_va_range_block_index(managed_range, start);
    const size_t last_block_index = uvm_va_range_block_index(managed_range, end);
    uvm_va_block_region_t covered_region;

    UVM_ASSERT(managed_range);
    UVM_ASSERT(va_block_context);
    UVM_ASSERT(start >= managed_range->va_range.node.start);
    UVM_ASSERT(end <= managed_range->va_range.node.end);
    uvm_assert_rwsem_locked(&managed_range->va_range.va_space->lock);

    // Iterate over blocks, discarding them if covered by range [start, end]
    for (i = first_block_index; i <= last_block_index; i++) {
        NV_STATUS status;
        uvm_va_block_t *va_block;

        // If the block doesn't exist yet, create it so we can track discarded
        // pages.
        status = uvm_va_range_block_create(managed_range, i, &va_block);
        if (status != NV_OK)
            return status;

        // Unlike preferred_location which is a policy, hence causing block
        // splits, discarded status is transient.
        // As a result, discarded page ranges can span block or cover
        // partial blocks.
        covered_region = uvm_va_block_region_from_start_end(va_block,
                                                            max(start, va_block->start),
                                                            min(end, va_block->end));
        uvm_page_mask_init_from_region(&va_block_context->discard.discarded_pages, covered_region, NULL);

        // The minimum discard unit is a va_block
        status = uvm_va_block_discard(va_block, va_block_context, flags);

        if (status != NV_OK)
            return status;
    }

    return NV_OK;
}

uvm_vma_wrapper_t *uvm_vma_wrapper_alloc(struct vm_area_struct *vma)
{
    uvm_vma_wrapper_t *vma_wrapper = nv_kmem_cache_zalloc(g_uvm_vma_wrapper_cache, NV_UVM_GFP_FLAGS);
    if (!vma_wrapper)
        return NULL;

    vma_wrapper->vma = vma;
    uvm_init_rwsem(&vma_wrapper->lock, UVM_LOCK_ORDER_LEAF);

    return vma_wrapper;
}

void uvm_vma_wrapper_destroy(uvm_vma_wrapper_t *vma_wrapper)
{
    if (!vma_wrapper)
        return;

    uvm_assert_rwsem_unlocked(&vma_wrapper->lock);

    kmem_cache_free(g_uvm_vma_wrapper_cache, vma_wrapper);
}

static NvU64 sked_reflected_pte_maker(uvm_page_table_range_vec_t *range_vec, NvU64 offset, void *caller_data)
{
    (void)caller_data;

    return range_vec->tree->hal->make_sked_reflected_pte();
}

static NV_STATUS uvm_map_sked_reflected_range(uvm_va_space_t *va_space, UVM_MAP_DYNAMIC_PARALLELISM_REGION_PARAMS *params)
{
    NV_STATUS status;
    uvm_va_range_sked_reflected_t *sked_reflected_range = NULL;
    uvm_gpu_t *gpu;
    uvm_gpu_va_space_t *gpu_va_space;
    uvm_page_tree_t *page_tables;
    struct mm_struct *mm;

    if (uvm_api_range_invalid_4k(params->base, params->length))
        return NV_ERR_INVALID_ADDRESS;

    // The mm needs to be locked in order to remove stale HMM va_blocks.
    mm = uvm_va_space_mm_or_current_retain_lock(va_space);
    uvm_va_space_down_write(va_space);

    gpu = uvm_va_space_get_gpu_by_uuid_with_gpu_va_space(va_space, &params->gpuUuid);
    if (!gpu) {
        status = NV_ERR_INVALID_DEVICE;
        goto done;
    }

    // Check if the GPU can access the VA
    if (!uvm_gpu_can_address(gpu, params->base, params->length)) {
        status = NV_ERR_OUT_OF_RANGE;
        goto done;
    }

    gpu_va_space = va_space->gpu_va_spaces[uvm_id_gpu_index(gpu->id)];
    page_tables = &gpu_va_space->page_tables;

    // The VA range must exactly cover one supported GPU page
    if (!is_power_of_2(params->length) ||
        !IS_ALIGNED(params->base, params->length) ||
        !uvm_mmu_page_size_supported(page_tables, params->length)) {
        status = NV_ERR_INVALID_ADDRESS;
        goto done;
    }

    status = uvm_va_range_create_sked_reflected(va_space, mm, params->base, params->length, &sked_reflected_range);
    if (status != NV_OK) {
        UVM_DBG_PRINT_RL("Failed to create sked reflected VA range [0x%llx, 0x%llx)\n",
                params->base, params->base + params->length);
        goto done;
    }

    sked_reflected_range->gpu_va_space = gpu_va_space;

    status = uvm_page_table_range_vec_init(page_tables,
                                           sked_reflected_range->va_range.node.start,
                                           uvm_va_range_size(&sked_reflected_range->va_range),
                                           params->length,
                                           UVM_PMM_ALLOC_FLAGS_EVICT,
                                           &sked_reflected_range->pt_range_vec);
    if (status != NV_OK)
        goto done;

    status = uvm_page_table_range_vec_write_ptes(&sked_reflected_range->pt_range_vec,
            UVM_MEMBAR_NONE, sked_reflected_pte_maker, NULL);

    if (status != NV_OK)
        goto done;

done:
    if (status != NV_OK && sked_reflected_range != NULL)
        uvm_va_range_destroy(&sked_reflected_range->va_range, NULL);

    uvm_va_space_up_write(va_space);
    uvm_va_space_mm_or_current_release_unlock(va_space, mm);

    return status;
}

NV_STATUS uvm_api_map_dynamic_parallelism_region(UVM_MAP_DYNAMIC_PARALLELISM_REGION_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    // Notably the ranges created by the UvmMapDynamicParallelismRegion() API
    // are referred to as "SKED reflected ranges" internally as it's more
    // descriptive.
    return uvm_map_sked_reflected_range(va_space, params);
}

NV_STATUS uvm_api_alloc_semaphore_pool(UVM_ALLOC_SEMAPHORE_POOL_PARAMS *params, struct file *filp)
{
    NV_STATUS status;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_va_range_semaphore_pool_t *semaphore_pool_range = NULL;
    uvm_gpu_t *gpu;
    struct mm_struct *mm;

    if (uvm_api_range_invalid(params->base, params->length))
        return NV_ERR_INVALID_ADDRESS;
    if (params->gpuAttributesCount > UVM_MAX_GPUS)
        return NV_ERR_INVALID_ARGUMENT;

    if (g_uvm_global.conf_computing_enabled && params->gpuAttributesCount == 0)
        return NV_ERR_INVALID_ARGUMENT;

    // The mm needs to be locked in order to remove stale HMM va_blocks.
    mm = uvm_va_space_mm_or_current_retain_lock(va_space);
    uvm_va_space_down_write(va_space);

    status = uvm_va_range_create_semaphore_pool(va_space,
                                                mm,
                                                params->base,
                                                params->length,
                                                params->perGpuAttributes,
                                                params->gpuAttributesCount,
                                                &semaphore_pool_range);
    if (status != NV_OK)
        goto unlock;

    for_each_va_space_gpu(gpu, va_space) {
        status = va_range_register_gpu_semaphore_pool(semaphore_pool_range, gpu);
        if (status != NV_OK)
            goto done;

        if (!uvm_processor_mask_test(&va_space->registered_gpu_va_spaces, gpu->id))
            continue;

        status = va_range_add_gpu_va_space_semaphore_pool(semaphore_pool_range, gpu);
        if (status != NV_OK)
            goto done;
    }

done:
    if (status != NV_OK)
        uvm_va_range_destroy(&semaphore_pool_range->va_range, NULL);

unlock:
    uvm_va_space_up_write(va_space);
    uvm_va_space_mm_or_current_release_unlock(va_space, mm);
    return status;
}

NV_STATUS uvm_test_va_range_info(UVM_TEST_VA_RANGE_INFO_PARAMS *params, struct file *filp)
{
    uvm_va_range_managed_t *managed_range;
    uvm_va_space_t *va_space;
    uvm_va_range_t *va_range;
    uvm_processor_id_t processor_id;
    uvm_va_policy_t *policy;
    struct vm_area_struct *vma;
    NV_STATUS status = NV_OK;
    struct mm_struct *mm;

    va_space = uvm_va_space_get(filp);

    mm = uvm_va_space_mm_or_current_retain_lock(va_space);
    uvm_va_space_down_read(va_space);

    va_range = uvm_va_range_find(va_space, params->lookup_address);
    if (!va_range) {
        status = uvm_hmm_va_range_info(va_space, mm, params);
        goto out;
    }

    params->va_range_start = va_range->node.start;
    params->va_range_end   = va_range->node.end;

    // -Wall implies -Wenum-compare, so cast through int to avoid warnings
    BUILD_BUG_ON((int)UVM_READ_DUPLICATION_UNSET    != (int)UVM_TEST_READ_DUPLICATION_UNSET);
    BUILD_BUG_ON((int)UVM_READ_DUPLICATION_ENABLED  != (int)UVM_TEST_READ_DUPLICATION_ENABLED);
    BUILD_BUG_ON((int)UVM_READ_DUPLICATION_DISABLED != (int)UVM_TEST_READ_DUPLICATION_DISABLED);
    BUILD_BUG_ON((int)UVM_READ_DUPLICATION_MAX      != (int)UVM_TEST_READ_DUPLICATION_MAX);

    // -Wall implies -Wenum-compare, so cast through int to avoid warnings
    BUILD_BUG_ON((int)UVM_TEST_VA_RANGE_TYPE_INVALID        != (int)UVM_VA_RANGE_TYPE_INVALID);
    BUILD_BUG_ON((int)UVM_TEST_VA_RANGE_TYPE_MANAGED        != (int)UVM_VA_RANGE_TYPE_MANAGED);
    BUILD_BUG_ON((int)UVM_TEST_VA_RANGE_TYPE_EXTERNAL       != (int)UVM_VA_RANGE_TYPE_EXTERNAL);
    BUILD_BUG_ON((int)UVM_TEST_VA_RANGE_TYPE_DMA_BUF        != (int)UVM_VA_RANGE_TYPE_DMA_BUF);
    BUILD_BUG_ON((int)UVM_TEST_VA_RANGE_TYPE_CHANNEL        != (int)UVM_VA_RANGE_TYPE_CHANNEL);
    BUILD_BUG_ON((int)UVM_TEST_VA_RANGE_TYPE_SKED_REFLECTED != (int)UVM_VA_RANGE_TYPE_SKED_REFLECTED);
    BUILD_BUG_ON((int)UVM_TEST_VA_RANGE_TYPE_SEMAPHORE_POOL != (int)UVM_VA_RANGE_TYPE_SEMAPHORE_POOL);
    BUILD_BUG_ON((int)UVM_TEST_VA_RANGE_TYPE_MAX            != (int)UVM_VA_RANGE_TYPE_MAX);
    params->type = va_range->type;

    params->read_duplication = 0;
    memset(&params->preferred_location, 0, sizeof(params->preferred_location));
    params->preferred_cpu_nid = NUMA_NO_NODE;
    params->accessed_by_count = 0;

    switch (va_range->type) {
        case UVM_VA_RANGE_TYPE_MANAGED:
            managed_range = uvm_va_range_to_managed(va_range);

            policy = &managed_range->policy;
            params->read_duplication = policy->read_duplication;

            if (UVM_ID_IS_VALID(policy->preferred_location)) {
                uvm_processor_get_uuid(policy->preferred_location, &params->preferred_location);
                params->preferred_cpu_nid = policy->preferred_nid;
            }

            for_each_id_in_mask(processor_id, &policy->accessed_by)
                uvm_processor_get_uuid(processor_id, &params->accessed_by[params->accessed_by_count++]);


            params->managed.subtype = UVM_TEST_RANGE_SUBTYPE_UVM;
            if (!managed_range->vma_wrapper) {
                params->managed.is_zombie = NV_TRUE;
                goto out;
            }
            params->managed.is_zombie = NV_FALSE;
            vma = uvm_va_range_vma_check(managed_range, mm);
            if (!vma) {
                // We aren't in the same mm as the one which owns the vma, and
                // we don't have that mm locked.
                params->managed.owned_by_calling_process = NV_FALSE;
                goto out;
            }
            params->managed.owned_by_calling_process = (mm == current->mm ? NV_TRUE : NV_FALSE);
            params->managed.vma_start = vma->vm_start;
            params->managed.vma_end   = vma->vm_end - 1;
            break;
        default:
            break;
    }

out:
    uvm_va_space_up_read(va_space);
    uvm_va_space_mm_or_current_release_unlock(va_space, mm);
    return status;
}

NV_STATUS uvm_test_va_range_split(UVM_TEST_VA_RANGE_SPLIT_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_va_range_managed_t *managed_range;
    NV_STATUS status = NV_OK;

    if (!PAGE_ALIGNED(params->split_address + 1))
        return NV_ERR_INVALID_ADDRESS;

    uvm_va_space_down_write(va_space);

    managed_range = uvm_va_range_managed_find(va_space, params->split_address);
    if (!managed_range ||
        managed_range->va_range.node.end == params->split_address) {
        status = NV_ERR_INVALID_ADDRESS;
        goto out;
    }

    status = uvm_va_range_split(managed_range, params->split_address, NULL);

out:
    uvm_va_space_up_write(va_space);
    return status;
}

NV_STATUS uvm_test_va_range_inject_split_error(UVM_TEST_VA_RANGE_INJECT_SPLIT_ERROR_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_va_range_managed_t *managed_range;
    uvm_va_range_t *va_range;
    struct mm_struct *mm;
    NV_STATUS status = NV_OK;

    mm = uvm_va_space_mm_or_current_retain_lock(va_space);
    uvm_va_space_down_write(va_space);

    va_range = uvm_va_range_find(va_space, params->lookup_address);
    managed_range = uvm_va_range_to_managed_or_null(va_range);
    if (!va_range) {
        if (!mm)
            status = NV_ERR_INVALID_ADDRESS;
        else
            status = uvm_hmm_test_va_block_inject_split_error(va_space, params->lookup_address);
    }
    else if (!managed_range) {
        status = NV_ERR_INVALID_ADDRESS;
    }
    else {
        uvm_va_block_t *va_block;
        size_t split_index;

        managed_range->inject_split_error = true;

        split_index = uvm_va_range_block_index(managed_range, params->lookup_address);
        va_block = uvm_va_range_block(managed_range, split_index);
        if (va_block) {
            uvm_va_block_test_t *block_test = uvm_va_block_get_test(va_block);

            if (block_test)
                block_test->inject_split_error = true;
        }
    }

    uvm_va_space_up_write(va_space);
    uvm_va_space_mm_or_current_release_unlock(va_space, mm);
    return status;
}

NV_STATUS uvm_test_va_range_inject_add_gpu_va_space_error(UVM_TEST_VA_RANGE_INJECT_ADD_GPU_VA_SPACE_ERROR_PARAMS *params,
                                                          struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_va_range_t *va_range;
    NV_STATUS status = NV_OK;

    uvm_va_space_down_write(va_space);

    va_range = uvm_va_range_find(va_space, params->lookup_address);
    if (!va_range) {
        status = NV_ERR_INVALID_ADDRESS;
        goto out;
    }

    va_range->inject_add_gpu_va_space_error = true;

out:
    uvm_va_space_up_write(va_space);
    return status;
}

