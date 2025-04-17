/*******************************************************************************
    Copyright (c) 2015-2024 NVIDIA Corporation

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
#include "uvm_types.h"
#include "uvm_api.h"
#include "uvm_global.h"
#include "uvm_hal.h"
#include "uvm_va_range.h"
#include "uvm_va_block.h"
#include "uvm_kvmalloc.h"
#include "uvm_map_external.h"
#include "uvm_perf_thrashing.h"
#include "nv_uvm_interface.h"

static struct kmem_cache *g_uvm_va_range_managed_cache __read_mostly;
static struct kmem_cache *g_uvm_va_range_external_cache __read_mostly;
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
    size_t new_size = new_num_blocks * sizeof(managed_range->va_range.blocks[0]);
    atomic_long_t *new_blocks;

    UVM_ASSERT(managed_range->va_range.blocks);
    UVM_ASSERT(uvm_kvsize(managed_range->va_range.blocks) >= uvm_va_range_num_blocks(managed_range) *
                                                             sizeof(managed_range->va_range.blocks[0]));
    UVM_ASSERT(new_num_blocks);
    UVM_ASSERT(new_num_blocks <= uvm_va_range_num_blocks(managed_range));

    // TODO: Bug 1766579: This could be optimized by only shrinking the array
    //       when the new size is half of the old size or some similar
    //       threshold. Need to profile this on real apps to see if that's worth
    //       doing.

    new_blocks = uvm_kvrealloc(managed_range->va_range.blocks, new_size);
    if (!new_blocks) {
        // If we failed to allocate a smaller array, just leave the old one as-is
        UVM_DBG_PRINT("Failed to shrink range [0x%llx, 0x%llx] from %zu blocks to %zu blocks\n",
                      managed_range->va_range.node.start,
                      managed_range->va_range.node.end,
                      uvm_kvsize(managed_range->va_range.blocks) / sizeof(managed_range->va_range.blocks[0]),
                      new_num_blocks);
        return;
    }

    managed_range->va_range.blocks = new_blocks;
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

    managed_range->va_range.blocks = uvm_kvmalloc_zero(uvm_va_range_num_blocks(managed_range) *
                                                       sizeof(managed_range->va_range.blocks[0]));
    if (!managed_range->va_range.blocks) {
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
    NV_STATUS status;

    if (managed_range->va_range.blocks) {
        // Unmap and drop our ref count on each block
        for_each_va_block_in_va_range_safe(managed_range, block, block_tmp)
            uvm_va_block_kill(block);

        uvm_kvfree(managed_range->va_range.blocks);
    }

    event_data.range_destroy.range = &managed_range->va_range;
    uvm_perf_event_notify(&managed_range->va_range.va_space->perf_events, UVM_PERF_EVENT_RANGE_DESTROY, &event_data);

    status = uvm_range_group_assign_range(managed_range->va_range.va_space,
                                          NULL,
                                          managed_range->va_range.node.start,
                                          managed_range->va_range.node.end);
    UVM_ASSERT(status == NV_OK);
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

static NV_STATUS va_range_add_gpu_va_space_managed(uvm_va_range_managed_t *managed_range,
                                                   uvm_gpu_va_space_t *gpu_va_space,
                                                   struct mm_struct *mm)
{
    uvm_va_space_t *va_space = managed_range->va_range.va_space;
    uvm_gpu_t *gpu = gpu_va_space->gpu;
    NV_STATUS status = NV_OK;
    const bool should_add_remote_mappings =
        uvm_processor_mask_test(&managed_range->policy.accessed_by, gpu->id) ||
        uvm_processor_mask_test(&managed_range->va_range.uvm_lite_gpus, gpu->id);

    // By this time, the gpu is already in the registration mask.
    const bool should_disable_read_duplication =
        managed_range->policy.read_duplication == UVM_READ_DUPLICATION_ENABLED &&
        (uvm_va_space_can_read_duplicate(va_space, NULL) != uvm_va_space_can_read_duplicate(va_space, gpu));

    // Combine conditions to perform a single VA block traversal
    if (gpu_va_space->ats.enabled || should_add_remote_mappings || should_disable_read_duplication) {
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
                // this GPU is a UVM-Lite GPU or has accessed_by set.
                status = uvm_va_block_set_accessed_by(va_block, va_block_context, gpu->id);
                if (status != NV_OK)
                    break;
            }

            if (should_disable_read_duplication) {
                status = uvm_va_block_unset_read_duplication(va_block, va_block_context);
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
    bool should_enable_read_duplicate;
    uvm_va_block_context_t *va_block_context = uvm_va_space_block_context(va_space, mm);

    should_enable_read_duplicate =
        managed_range->policy.read_duplication == UVM_READ_DUPLICATION_ENABLED &&
        uvm_va_space_can_read_duplicate(va_space, NULL) != uvm_va_space_can_read_duplicate(va_space, gpu_va_space->gpu);

    for_each_va_block_in_va_range(managed_range, va_block) {
        uvm_mutex_lock(&va_block->lock);
        uvm_va_block_remove_gpu_va_space(va_block, gpu_va_space, va_block_context);
        uvm_mutex_unlock(&va_block->lock);

        if (should_enable_read_duplicate)
            uvm_va_block_set_read_duplication(va_block, va_block_context);
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
            uvm_va_range_deinit_device_p2p(uvm_va_range_to_device_p2p(va_range), deferred_free_list);
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
        // For UVM-Lite at most one GPU needs to map the peer GPU if it's the
        // preferred location, but it doesn't hurt to just try mapping both.
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
    uvm_gpu_t *uvm_lite_gpu_to_unmap = NULL;

    bool uvm_lite_mode = uvm_processor_mask_test(&managed_range->va_range.uvm_lite_gpus, gpu0->id) &&
                         uvm_processor_mask_test(&managed_range->va_range.uvm_lite_gpus, gpu1->id);

    if (uvm_lite_mode) {
        // In UVM-Lite mode, the UVM-Lite GPUs can only have mappings to the the
        // preferred location. If peer mappings are being disabled to the
        // preferred location, then unmap the other GPU.
        // Nothing to do otherwise.
        if (uvm_va_policy_preferred_location_equal(&managed_range->policy, gpu0->id, NUMA_NO_NODE))
            uvm_lite_gpu_to_unmap = gpu1;
        else if (uvm_va_policy_preferred_location_equal(&managed_range->policy, gpu1->id, NUMA_NO_NODE))
            uvm_lite_gpu_to_unmap = gpu0;
        else
            return;
    }

    for_each_va_block_in_va_range(managed_range, va_block) {
        uvm_mutex_lock(&va_block->lock);
        if (uvm_lite_mode)
            uvm_va_block_unmap_preferred_location_uvm_lite(va_block, uvm_lite_gpu_to_unmap);
        else
            uvm_va_block_disable_peer(va_block, gpu0, gpu1);
        uvm_mutex_unlock(&va_block->lock);
    }

    if (uvm_lite_mode && !uvm_range_group_all_migratable(managed_range->va_range.va_space,
                                                         managed_range->va_range.node.start,
                                                         managed_range->va_range.node.end)) {
        UVM_ASSERT(uvm_lite_gpu_to_unmap);

        // Migration is prevented, but we had to unmap a UVM-Lite GPU. Update
        // the accessed by and UVM-Lite GPUs masks as it cannot be considered a
        // UVM-Lite GPU any more.
        uvm_va_range_unset_accessed_by(managed_range, uvm_lite_gpu_to_unmap->id, NULL);
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
            // All ranges should have been deinited by GPU VA space unregister,
            // which should have already happened.
            UVM_ASSERT(!uvm_va_range_to_device_p2p(va_range)->p2p_mem);
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
            atomic_long_set(&new->va_range.blocks[0], (long)block);
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
    if (existing->va_range.inject_split_error) {
        UVM_ASSERT(!block);
        existing->va_range.inject_split_error = false;

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
    //                                            ^new->va_range.blocks[0]

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
        atomic_long_set(&new->va_range.blocks[new_index], (long)block);
        atomic_long_set(&existing->va_range.blocks[split_index + new_index], (long)NULL);
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
    uvm_processor_mask_copy(&new->va_range.uvm_lite_gpus, &existing_managed_range->va_range.uvm_lite_gpus);

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
        old = (uvm_va_block_t *)atomic_long_cmpxchg(&managed_range->va_range.blocks[index],
                                                    (long)NULL,
                                                    (long)block);
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

static NV_STATUS range_unmap_mask(uvm_va_range_managed_t *managed_range,
                                  const uvm_processor_mask_t *mask,
                                  uvm_tracker_t *out_tracker)
{
    uvm_va_space_t *va_space = managed_range->va_range.va_space;
    uvm_va_block_context_t *block_context = uvm_va_space_block_context(va_space, NULL);
    uvm_va_block_t *block;

    if (uvm_processor_mask_empty(mask))
        return NV_OK;

    for_each_va_block_in_va_range(managed_range, block) {
        NV_STATUS status;
        uvm_va_block_region_t region = uvm_va_block_region_from_block(block);

        uvm_mutex_lock(&block->lock);
        status = uvm_va_block_unmap_mask(block, block_context, mask, region, NULL);
        if (out_tracker)
            uvm_tracker_add_tracker_safe(out_tracker, &block->tracker);

        uvm_mutex_unlock(&block->lock);
        if (status != NV_OK)
            return status;
    }

    return NV_OK;
}

static NV_STATUS range_unmap(uvm_va_range_managed_t *managed_range,
                             uvm_processor_id_t processor,
                             uvm_tracker_t *out_tracker)
{
    uvm_processor_mask_t *mask;
    uvm_va_space_t *va_space = managed_range->va_range.va_space;

    uvm_assert_rwsem_locked_write(&va_space->lock);

    mask = &va_space->unmap_mask;

    uvm_processor_mask_zero(mask);
    uvm_processor_mask_set(mask, processor);

    return range_unmap_mask(managed_range, mask, out_tracker);
}

static NV_STATUS range_map_uvm_lite_gpus(uvm_va_range_managed_t *managed_range, uvm_tracker_t *out_tracker)
{
    NV_STATUS status = NV_OK;
    uvm_va_block_t *va_block;
    uvm_va_block_context_t *va_block_context = uvm_va_space_block_context(managed_range->va_range.va_space, NULL);

    if (uvm_processor_mask_empty(&managed_range->va_range.uvm_lite_gpus))
        return NV_OK;


    for_each_va_block_in_va_range(managed_range, va_block) {
        // UVM-Lite GPUs always map with RWA
        uvm_mutex_lock(&va_block->lock);
        status = UVM_VA_BLOCK_RETRY_LOCKED(va_block, NULL,
                uvm_va_block_map_mask(va_block,
                                      va_block_context,
                                      &managed_range->va_range.uvm_lite_gpus,
                                      uvm_va_block_region_from_block(va_block),
                                      NULL,
                                      UVM_PROT_READ_WRITE_ATOMIC,
                                      UvmEventMapRemoteCauseCoherence));
        if (status == NV_OK && out_tracker)
            status = uvm_tracker_add_tracker(out_tracker, &va_block->tracker);

        uvm_mutex_unlock(&va_block->lock);
        if (status != NV_OK)
            break;
    }

    return status;
}

// Calculate the mask of GPUs that should follow the UVM-Lite behaviour
static void calc_uvm_lite_gpus_mask(uvm_va_space_t *va_space,
                                    uvm_processor_id_t preferred_location,
                                    const uvm_processor_mask_t *accessed_by_mask,
                                    uvm_processor_mask_t *uvm_lite_gpus)
{
    uvm_gpu_id_t gpu_id;

    uvm_assert_rwsem_locked_write(&va_space->lock);

    // Zero out the mask first
    uvm_processor_mask_zero(uvm_lite_gpus);

    // If no preferred location is set then there are no GPUs following the UVM-Lite behavior
    if (UVM_ID_IS_INVALID(preferred_location))
        return;

    // If the preferred location is a faultable GPU, then no GPUs should follow
    // the UVM-Lite behaviour.
    if (UVM_ID_IS_GPU(preferred_location) &&
        uvm_processor_mask_test(&va_space->faultable_processors, preferred_location)) {
        return;
    }

    // Otherwise add all non-faultable GPUs to the UVM-Lite mask that have
    // accessed by set.
    for_each_gpu_id_in_mask(gpu_id, accessed_by_mask) {
        if (!uvm_processor_mask_test(&va_space->faultable_processors, gpu_id))
            uvm_processor_mask_set(uvm_lite_gpus, gpu_id);
    }

    // And the preferred location if it's a GPU
    if (UVM_ID_IS_GPU(preferred_location))
        uvm_processor_mask_set(uvm_lite_gpus, preferred_location);
}

// Update the mask of GPUs that follow the UVM-Lite behaviour
static void range_update_uvm_lite_gpus_mask(uvm_va_range_managed_t *managed_range)
{
    calc_uvm_lite_gpus_mask(managed_range->va_range.va_space,
                            managed_range->policy.preferred_location,
                            &managed_range->policy.accessed_by,
                            &managed_range->va_range.uvm_lite_gpus);
}

NV_STATUS uvm_va_range_set_preferred_location(uvm_va_range_managed_t *managed_range,
                                              uvm_processor_id_t preferred_location,
                                              int preferred_cpu_nid,
                                              struct mm_struct *mm,
                                              uvm_tracker_t *out_tracker)
{
    NV_STATUS status = NV_OK;
    uvm_processor_mask_t *all_uvm_lite_gpus = NULL;
    uvm_processor_mask_t *new_uvm_lite_gpus = NULL;
    uvm_processor_mask_t *set_accessed_by_processors = NULL;
    uvm_range_group_range_iter_t iter;
    uvm_range_group_range_t *rgr = NULL;
    uvm_va_space_t *va_space = managed_range->va_range.va_space;
    uvm_va_block_t *va_block;
    uvm_va_block_context_t *va_block_context;
    uvm_va_policy_t *va_range_policy;
    NvU64 start, end;

    uvm_assert_rwsem_locked_write(&va_space->lock);

    all_uvm_lite_gpus = uvm_processor_mask_cache_alloc();
    if (!all_uvm_lite_gpus) {
        status = NV_ERR_NO_MEMORY;
        goto out;
    }

    new_uvm_lite_gpus = uvm_processor_mask_cache_alloc();
    if (!new_uvm_lite_gpus) {
        status = NV_ERR_NO_MEMORY;
        goto out;
    }

    set_accessed_by_processors = uvm_processor_mask_cache_alloc();
    if (!set_accessed_by_processors) {
        status = NV_ERR_NO_MEMORY;
        goto out;
    }

    va_range_policy = &managed_range->policy;
    if (uvm_va_policy_preferred_location_equal(va_range_policy, preferred_location, preferred_cpu_nid))
        goto out;

    // Mark all range group ranges within this managed range as migrated since
    // the preferred location has changed.
    start = managed_range->va_range.node.start;
    end = managed_range->va_range.node.end;
    uvm_range_group_for_each_range_in(rgr, va_space, start, end) {
        uvm_spin_lock(&rgr->range_group->migrated_ranges_lock);
        if (list_empty(&rgr->range_group_migrated_list_node))
            list_move_tail(&rgr->range_group_migrated_list_node, &rgr->range_group->migrated_ranges);
        uvm_spin_unlock(&rgr->range_group->migrated_ranges_lock);
    }

    // Calculate the new UVM-Lite GPUs mask, but don't update managed_range
    // state so that we can keep block_page_check_mappings() happy while
    // updating the mappings.
    calc_uvm_lite_gpus_mask(va_space, preferred_location, &va_range_policy->accessed_by, new_uvm_lite_gpus);

    // If the range contains non-migratable range groups, check that new
    // UVM-Lite GPUs can all map the new preferred location.
    if (!uvm_range_group_all_migratable(va_space, start, end) &&
        UVM_ID_IS_VALID(preferred_location) &&
        !uvm_processor_mask_subset(new_uvm_lite_gpus, &va_space->accessible_from[uvm_id_value(preferred_location)])) {
        status = NV_ERR_INVALID_DEVICE;
        goto out;
    }

    if (UVM_ID_IS_INVALID(preferred_location)) {
        uvm_range_group_for_each_migratability_in_safe(&iter, va_space, start, end) {
            if (!iter.migratable) {
                // Clear the range group assocation for any unmigratable ranges
                // if there is no preferred location
                status = uvm_range_group_assign_range(va_space, NULL, iter.start, iter.end);
                if (status != NV_OK)
                    goto out;
            }
        }
    }

    // Unmap all old and new UVM-Lite GPUs
    //  - GPUs that stop being UVM-Lite need to be unmapped so that they don't
    //    have stale mappings to the old preferred location.
    //  - GPUs that will continue to be UVM-Lite GPUs or are new UVM-Lite GPUs
    //    need to be unmapped so that the new preferred location can be mapped.
    uvm_processor_mask_or(all_uvm_lite_gpus, &managed_range->va_range.uvm_lite_gpus, new_uvm_lite_gpus);
    status = range_unmap_mask(managed_range, all_uvm_lite_gpus, out_tracker);
    if (status != NV_OK)
        goto out;

    // GPUs that stop being UVM-Lite, but are in the accessed_by mask need to
    // have any possible mappings established.
    uvm_processor_mask_andnot(set_accessed_by_processors, &managed_range->va_range.uvm_lite_gpus, new_uvm_lite_gpus);

    // A GPU which had been in UVM-Lite mode before must still be in UVM-Lite
    // mode if it is the new preferred location. Otherwise we'd have to be more
    // careful below to not establish remote mappings to the new preferred
    // location.
    if (UVM_ID_IS_GPU(preferred_location))
        UVM_ASSERT(!uvm_processor_mask_test(set_accessed_by_processors, preferred_location));

    // The old preferred location should establish new remote mappings if it has
    // accessed-by set.
    if (UVM_ID_IS_VALID(va_range_policy->preferred_location))
        uvm_processor_mask_set(set_accessed_by_processors, va_range_policy->preferred_location);

    uvm_processor_mask_and(set_accessed_by_processors, set_accessed_by_processors, &va_range_policy->accessed_by);

    // Now update the managed_range state
    va_range_policy->preferred_location = preferred_location;
    va_range_policy->preferred_nid = preferred_cpu_nid;
    uvm_processor_mask_copy(&managed_range->va_range.uvm_lite_gpus, new_uvm_lite_gpus);

    va_block_context = uvm_va_space_block_context(va_space, mm);

    for_each_va_block_in_va_range(managed_range, va_block) {
        uvm_processor_id_t id;
        uvm_va_block_region_t region = uvm_va_block_region_from_block(va_block);

        for_each_id_in_mask(id, set_accessed_by_processors) {
            status = uvm_va_block_set_accessed_by(va_block, va_block_context, id);
            if (status != NV_OK)
                goto out;
        }

        // Also, mark CPU pages as dirty and remove remote mappings from the new
        // preferred location
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

    // And lastly map all of the current UVM-Lite GPUs to the resident pages on
    // the new preferred location. Anything that's not resident right now will
    // get mapped on the next PreventMigration().
    status = range_map_uvm_lite_gpus(managed_range, out_tracker);

out:
    uvm_processor_mask_cache_free(set_accessed_by_processors);
    uvm_processor_mask_cache_free(new_uvm_lite_gpus);
    uvm_processor_mask_cache_free(all_uvm_lite_gpus);

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
    uvm_processor_mask_t *new_uvm_lite_gpus;

    // va_block_context->scratch_processor_mask cannot be used since
    // range_unmap() calls uvm_va_space_block_context(), which re-
    // initializes the VA block context structure.
    new_uvm_lite_gpus = uvm_processor_mask_cache_alloc();
    if (!new_uvm_lite_gpus)
        return NV_ERR_NO_MEMORY;

    // If the range belongs to a non-migratable range group and that
    // processor_id is a non-faultable GPU, check it can map the preferred
    // location
    if (!uvm_range_group_all_migratable(va_space,
                                        managed_range->va_range.node.start,
                                        managed_range->va_range.node.end) &&
        UVM_ID_IS_GPU(processor_id) &&
        !uvm_processor_mask_test(&va_space->faultable_processors, processor_id) &&
        !uvm_processor_mask_test(&va_space->accessible_from[uvm_id_value(policy->preferred_location)], processor_id)) {
        status = NV_ERR_INVALID_DEVICE;
        goto out;
    }

    uvm_processor_mask_set(&policy->accessed_by, processor_id);

    // If a GPU is already a UVM-Lite GPU then there is nothing else to do.
    if (uvm_processor_mask_test(&managed_range->va_range.uvm_lite_gpus, processor_id))
        goto out;

    // Calculate the new UVM-Lite GPUs mask, but don't update it in the managed
    // range yet so that we can keep block_page_check_mappings() happy while
    // updating the mappings.
    calc_uvm_lite_gpus_mask(va_space, policy->preferred_location, &policy->accessed_by, new_uvm_lite_gpus);

    if (uvm_processor_mask_test(new_uvm_lite_gpus, processor_id)) {
        // GPUs that become UVM-Lite GPUs need to unmap everything so that they
        // can map the preferred location.
        status = range_unmap(managed_range, processor_id, out_tracker);
        if (status != NV_OK)
            goto out;
    }

    uvm_processor_mask_copy(&managed_range->va_range.uvm_lite_gpus, new_uvm_lite_gpus);

    for_each_va_block_in_va_range(managed_range, va_block) {
        status = uvm_va_block_set_accessed_by(va_block, va_block_context, processor_id);
        if (status != NV_OK)
            goto out;
    }

out:
    uvm_processor_mask_cache_free(new_uvm_lite_gpus);
    return status;
}

void uvm_va_range_unset_accessed_by(uvm_va_range_managed_t *managed_range,
                                    uvm_processor_id_t processor_id,
                                    uvm_tracker_t *out_tracker)
{
    uvm_range_group_range_t *rgr = NULL;

    // Mark all range group ranges within this managed range as migrated. We do
    // this to force uvm_range_group_set_migration_policy to re-check the policy
    // state since we're changing it here.
    uvm_range_group_for_each_range_in(rgr,
                                      managed_range->va_range.va_space,
                                      managed_range->va_range.node.start,
                                      managed_range->va_range.node.end) {
        uvm_spin_lock(&rgr->range_group->migrated_ranges_lock);
        if (list_empty(&rgr->range_group_migrated_list_node))
            list_move_tail(&rgr->range_group_migrated_list_node, &rgr->range_group->migrated_ranges);
        uvm_spin_unlock(&rgr->range_group->migrated_ranges_lock);
    }

    uvm_processor_mask_clear(&managed_range->policy.accessed_by, processor_id);

    // If a UVM-Lite GPU is being removed from the accessed_by mask, it will
    // also stop being a UVM-Lite GPU unless it's also the preferred location.
    if (uvm_processor_mask_test(&managed_range->va_range.uvm_lite_gpus, processor_id) &&
        !uvm_va_policy_preferred_location_equal(&managed_range->policy, processor_id, NUMA_NO_NODE)) {
        range_unmap(managed_range, processor_id, out_tracker);
    }

    range_update_uvm_lite_gpus_mask(managed_range);
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

        va_range->inject_split_error = true;

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

