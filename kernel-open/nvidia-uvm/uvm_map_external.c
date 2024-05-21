/*******************************************************************************
    Copyright (c) 2016-2024 NVIDIA Corporation

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
#include "uvm_forward_decl.h"
#include "uvm_lock.h"
#include "uvm_mmu.h"
#include "uvm_api.h"
#include "uvm_global.h"
#include "uvm_gpu.h"
#include "uvm_push.h"
#include "uvm_va_space.h"
#include "uvm_va_range.h"
#include "uvm_tracker.h"
#include "uvm_hal.h"
#include "uvm_hal_types.h"
#include "uvm_map_external.h"
#include "uvm_pte_batch.h"
#include "uvm_tlb_batch.h"
#include "nv_uvm_interface.h"
#include "nv_uvm_types.h"

#include "uvm_pushbuffer.h"

// Assume almost all of the push space can be used for PTEs leaving 1K of margin.
#define MAX_COPY_SIZE_PER_PUSH ((size_t)(UVM_MAX_PUSH_SIZE - 1024))

typedef struct
{
    // The VA range the buffer is for
    uvm_va_range_t *va_range;

    // The GPU that's mapping the VA range
    uvm_gpu_t *gpu;

    // Mapping info used for querying PTEs from RM
    UvmGpuExternalMappingInfo mapping_info;

    // Size of the buffer
    size_t buffer_size;

    // Page size in bytes
    NvU64 page_size;

    // Size of a single PTE in bytes
    NvU32 pte_size;

    // Max PTE offset covered by the VA range.
    //
    // Notably the mapping might not start at offset 0 and max PTE offset can be
    // larger than number of PTEs covering the VA range.
    size_t max_pte_offset;

    // Number of PTEs currently in the buffer
    size_t num_ptes;

    // PTE offset at which the currently buffered PTEs start.
    size_t pte_offset;
} uvm_pte_buffer_t;

// Max PTE buffer size is the size of the buffer used for querying PTEs from RM.
// It has to be big enough to amortize the cost of calling into RM, but small
// enough to fit in CPU caches as it's written and read multiple times on the
// CPU before it ends up in the pushbuffer.
// 96K seems to be a sweet spot at least on a Xeon W5580 system. This could use
// some benchmarking on more systems though.
#define MAX_PTE_BUFFER_SIZE ((size_t)96 * 1024)

static NV_STATUS uvm_pte_buffer_init(uvm_va_range_t *va_range,
                                     uvm_gpu_t *gpu,
                                     const uvm_map_rm_params_t *map_rm_params,
                                     NvU64 length,
                                     NvU64 page_size,
                                     uvm_pte_buffer_t *pte_buffer)
{
    uvm_gpu_va_space_t *gpu_va_space = uvm_gpu_va_space_get(va_range->va_space, gpu);
    uvm_page_tree_t *tree = &gpu_va_space->page_tables;
    size_t num_all_ptes;

    memset(pte_buffer, 0, sizeof(*pte_buffer));

    pte_buffer->va_range = va_range;
    pte_buffer->gpu = gpu;
    pte_buffer->mapping_info.cachingType        = (UvmRmGpuCachingType) map_rm_params->caching_type;
    pte_buffer->mapping_info.mappingType        = (UvmRmGpuMappingType) map_rm_params->mapping_type;
    pte_buffer->mapping_info.formatType         = (UvmRmGpuFormatType) map_rm_params->format_type;
    pte_buffer->mapping_info.elementBits        = (UvmRmGpuFormatElementBits) map_rm_params->element_bits;
    pte_buffer->mapping_info.compressionType    = (UvmRmGpuCompressionType) map_rm_params->compression_type;
    if (va_range->type == UVM_VA_RANGE_TYPE_EXTERNAL)
        pte_buffer->mapping_info.mappingPageSize = page_size;

    pte_buffer->page_size = page_size;
    pte_buffer->pte_size = uvm_mmu_pte_size(tree, page_size);
    num_all_ptes = uvm_div_pow2_64(length, page_size);
    pte_buffer->max_pte_offset = uvm_div_pow2_64(map_rm_params->map_offset, page_size) + num_all_ptes;
    pte_buffer->buffer_size = min(MAX_PTE_BUFFER_SIZE, num_all_ptes * pte_buffer->pte_size);

    pte_buffer->mapping_info.pteBuffer = uvm_kvmalloc(pte_buffer->buffer_size);
    if (!pte_buffer->mapping_info.pteBuffer)
        return NV_ERR_NO_MEMORY;

    return NV_OK;
}

static void uvm_pte_buffer_deinit(uvm_pte_buffer_t *pte_buffer)
{
    uvm_kvfree(pte_buffer->mapping_info.pteBuffer);
}

// Get the PTEs for mapping the [map_offset, map_offset + map_size) VA range.
static NV_STATUS uvm_pte_buffer_get(uvm_pte_buffer_t *pte_buffer,
                                    NvHandle mem_handle,
                                    NvU64 map_offset,
                                    NvU64 map_size,
                                    NvU64 **ptes_out)
{
    NV_STATUS status;
    size_t pte_offset;
    size_t num_ptes;
    size_t ptes_left;
    uvm_va_range_t *va_range = pte_buffer->va_range;
    uvm_gpu_va_space_t *gpu_va_space = uvm_gpu_va_space_get(va_range->va_space, pte_buffer->gpu);

    UVM_ASSERT(IS_ALIGNED(map_offset, pte_buffer->page_size));
    UVM_ASSERT(IS_ALIGNED(map_size, pte_buffer->page_size));

    pte_offset = uvm_div_pow2_64(map_offset, pte_buffer->page_size);
    num_ptes = uvm_div_pow2_64(map_size, pte_buffer->page_size);

    UVM_ASSERT(num_ptes <= pte_buffer->buffer_size / pte_buffer->pte_size);

    // If the requested range is already fully cached, just calculate its
    // offset within the buffer and return.
    if (pte_buffer->pte_offset <= pte_offset && pte_buffer->pte_offset + pte_buffer->num_ptes >= pte_offset + num_ptes) {
        pte_offset -= pte_buffer->pte_offset;
        *ptes_out = (NvU64 *)((char *)pte_buffer->mapping_info.pteBuffer + pte_offset * pte_buffer->pte_size);
        return NV_OK;
    }

    // Otherwise get max possible PTEs from RM starting at the requested offset.
    pte_buffer->pte_offset = pte_offset;
    ptes_left = pte_buffer->max_pte_offset - pte_offset;
    pte_buffer->num_ptes = min(pte_buffer->buffer_size / pte_buffer->pte_size, ptes_left);

    UVM_ASSERT_MSG(pte_buffer->num_ptes >= num_ptes, "buffer num ptes %zu < num ptes %zu\n",
            pte_buffer->num_ptes, num_ptes);

    // TODO: Bug 1735291: RM can determine the buffer size from the map_size
    //       parameter.
    pte_buffer->mapping_info.pteBufferSize = pte_buffer->num_ptes * pte_buffer->pte_size;

    if (va_range->type == UVM_VA_RANGE_TYPE_CHANNEL) {
        status = uvm_rm_locked_call(nvUvmInterfaceGetChannelResourcePtes(gpu_va_space->duped_gpu_va_space,
                                                                         va_range->channel.rm_descriptor,
                                                                         map_offset,
                                                                         pte_buffer->num_ptes * pte_buffer->page_size,
                                                                         &pte_buffer->mapping_info));
    }
    else {
        status = uvm_rm_locked_call(nvUvmInterfaceGetExternalAllocPtes(gpu_va_space->duped_gpu_va_space,
                                                                       mem_handle,
                                                                       map_offset,
                                                                       pte_buffer->num_ptes * pte_buffer->page_size,
                                                                       &pte_buffer->mapping_info));
    }

    if (status != NV_OK) {
        if (status != NV_ERR_NOT_READY) {
            UVM_ERR_PRINT("Failed to get %s mappings for VA range [0x%llx, 0x%llx], offset 0x%llx, size 0x%llx: %s\n",
                          va_range->type == UVM_VA_RANGE_TYPE_CHANNEL ? "channel" : "external",
                          va_range->node.start,
                          va_range->node.end,
                          map_offset,
                          map_size,
                          nvstatusToString(status));
        }
        return status;
    }

    *ptes_out = pte_buffer->mapping_info.pteBuffer;

    return NV_OK;
}

// Copies the input ptes buffer to the given physical address, with an optional
// TLB invalidate. The copy acquires the input tracker then updates it.
static NV_STATUS copy_ptes(uvm_page_tree_t *tree,
                           NvU64 page_size,
                           uvm_gpu_phys_address_t pte_addr,
                           NvU64 *ptes,
                           NvU32 num_ptes,
                           bool last_mapping,
                           uvm_range_tree_node_t *range_node,
                           uvm_tracker_t *tracker)
{
    uvm_push_t push;
    NV_STATUS status;
    NvU32 pte_size = uvm_mmu_pte_size(tree, page_size);

    UVM_ASSERT(((NvU64)pte_size) * num_ptes == pte_size * num_ptes);
    UVM_ASSERT(pte_size * num_ptes <= MAX_COPY_SIZE_PER_PUSH);

    status = uvm_push_begin_acquire(tree->gpu->channel_manager,
                                    UVM_CHANNEL_TYPE_MEMOPS,
                                    tracker,
                                    &push,
                                    "Writing %u bytes of PTEs to {%s, 0x%llx}",
                                    pte_size * num_ptes,
                                    uvm_aperture_string(pte_addr.aperture),
                                    pte_addr.address);
    if (status != NV_OK)
        return status;

    uvm_pte_batch_single_write_ptes(&push, pte_addr, ptes, pte_size, num_ptes);

    if (last_mapping) {
        // Do a TLB invalidate if this is the last mapping in the VA range
        // Membar: This is a permissions upgrade, so no post-invalidate membar
        //         is needed.
        uvm_tlb_batch_single_invalidate(tree,
                                        &push,
                                        range_node->start,
                                        uvm_range_tree_node_size(range_node),
                                        page_size,
                                        UVM_MEMBAR_NONE);
    }
    else {
        // For pushes prior to the last one, the PTE batch write has
        // already pushed a membar that's enough to order the PTE writes
        // with the TLB invalidate in the last push and that's all
        // that's needed.
        // If a failure happens before the push for the last mapping, it is
        // still ok as what will follow is more CE writes to unmap the PTEs and
        // those will get ordered by the membar from the PTE batch.
        uvm_push_set_flag(&push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);
    }

    uvm_push_end(&push);

    // The push acquired the tracker so it's ok to just overwrite it with
    // the entry tracking the push.
    uvm_tracker_overwrite_with_push(tracker, &push);

    return NV_OK;
}

// Map all of pt_range, which is contained with the va_range and begins at
// virtual address map_start. The PTE values are queried from RM and the pushed
// writes are added to the input tracker.
//
// If the mapped range ends on range_node->end, a TLB invalidate for upgrade is
// also issued.
static NV_STATUS map_rm_pt_range(uvm_page_tree_t *tree,
                                 uvm_page_table_range_t *pt_range,
                                 uvm_pte_buffer_t *pte_buffer,
                                 uvm_range_tree_node_t *range_node,
                                 NvHandle mem_handle,
                                 NvU64 map_start,
                                 NvU64 map_offset,
                                 uvm_tracker_t *tracker)
{
    uvm_gpu_phys_address_t pte_addr;
    NvU64 page_size = pt_range->page_size;
    NvU32 pte_size = uvm_mmu_pte_size(tree, page_size);
    NvU64 addr, end;
    size_t max_ptes, ptes_left, num_ptes;
    NvU64 map_size;
    bool last_mapping;
    NV_STATUS status = NV_OK;

    end = map_start + uvm_page_table_range_size(pt_range) - 1;

    UVM_ASSERT(map_start >= range_node->start);
    UVM_ASSERT(end <= range_node->end);
    UVM_ASSERT(page_size & tree->hal->page_sizes());
    UVM_ASSERT(IS_ALIGNED(map_start, page_size));
    UVM_ASSERT(IS_ALIGNED(map_offset, page_size));

    pte_addr = uvm_page_table_range_entry_address(tree, pt_range, 0);
    max_ptes = min((size_t)(uvm_mmu_pde_coverage(tree, page_size) / page_size), MAX_COPY_SIZE_PER_PUSH / pte_size);
    max_ptes = min(max_ptes, pte_buffer->buffer_size / pte_size);

    addr = map_start;
    ptes_left = (size_t)uvm_div_pow2_64(uvm_page_table_range_size(pt_range), page_size);
    while (addr < end) {
        NvU64 *pte_bits;

        num_ptes = min(max_ptes, ptes_left);
        map_size = num_ptes * page_size;
        UVM_ASSERT(addr + map_size <= end + 1);

        status = uvm_pte_buffer_get(pte_buffer, mem_handle, map_offset, map_size, &pte_bits);
        if (status != NV_OK)
            return status;

        last_mapping = (addr + map_size - 1 == range_node->end);

        // These copies are technically independent, except for the last one
        // which issues the TLB invalidate and thus must wait for all others.
        // However, since each copy will saturate the bus anyway we force them
        // to serialize to avoid bus contention.
        status = copy_ptes(tree,
                           page_size,
                           pte_addr,
                           pte_bits,
                           num_ptes,
                           last_mapping,
                           range_node,
                           tracker);
        if (status != NV_OK)
            return status;

        ptes_left -= num_ptes;
        pte_addr.address += num_ptes * pte_size;
        addr += map_size;
        map_offset += map_size;
    }

    return NV_OK;
}

// Determine the appropriate membar for downgrades on a VA range with type
// UVM_VA_RANGE_TYPE_EXTERNAL or UVM_VA_RANGE_TYPE_CHANNEL.
static uvm_membar_t va_range_downgrade_membar(uvm_va_range_t *va_range, uvm_ext_gpu_map_t *ext_gpu_map)
{
    if (va_range->type == UVM_VA_RANGE_TYPE_CHANNEL) {
        return uvm_hal_downgrade_membar_type(va_range->channel.gpu_va_space->gpu,
                                             va_range->channel.aperture == UVM_APERTURE_VID);
    }

    // If there is no mem_handle, this is a sparse mapping.
    // UVM_MEMBAR_GPU is sufficient because the debug pages remain allocated
    // until the GPU is torn down. GPU tear down implies that our context has
    // been switched out. In turn, this implies a sysmembar.
    if (!ext_gpu_map->mem_handle)
        return UVM_MEMBAR_GPU;

    // EGM uses the same barriers as sysmem.
    return uvm_hal_downgrade_membar_type(ext_gpu_map->gpu,
                                         !ext_gpu_map->is_sysmem && ext_gpu_map->gpu == ext_gpu_map->owning_gpu);
}

NV_STATUS uvm_va_range_map_rm_allocation(uvm_va_range_t *va_range,
                                         uvm_gpu_t *mapping_gpu,
                                         const UvmGpuMemoryInfo *mem_info,
                                         const uvm_map_rm_params_t *map_rm_params,
                                         uvm_ext_gpu_map_t *ext_gpu_map,
                                         uvm_tracker_t *out_tracker)
{
    uvm_gpu_va_space_t *gpu_va_space = uvm_gpu_va_space_get(va_range->va_space, mapping_gpu);
    uvm_page_tree_t *page_tree;
    uvm_pte_buffer_t pte_buffer;
    uvm_page_table_range_vec_t *pt_range_vec;
    uvm_page_table_range_t *pt_range;
    uvm_range_tree_node_t *node;
    NvU64 addr, size;
    NvU64 map_offset = map_rm_params->map_offset;
    size_t i;
    NV_STATUS status;
    uvm_tracker_t *tracker;

    // Track local pushes in a separate tracker, instead of adding them
    // directly to the output tracker, to avoid false dependencies
    // (serialization) on unrelated work. The local tracker is added to the
    // output tracker before the function returns.
    uvm_tracker_t local_tracker = UVM_TRACKER_INIT();

    // Local tracker is used when this function is called to map allocations
    // other than external allocations. Otherwise, the external allocations
    // use their own tracker.
    if (ext_gpu_map)
        tracker = &ext_gpu_map->tracker;
    else
        tracker = &local_tracker;

    UVM_ASSERT(gpu_va_space);
    UVM_ASSERT(va_range->type == UVM_VA_RANGE_TYPE_EXTERNAL || va_range->type == UVM_VA_RANGE_TYPE_CHANNEL);
    UVM_ASSERT(IS_ALIGNED(mem_info->size, mem_info->pageSize));
    UVM_ASSERT(out_tracker);

    page_tree = &gpu_va_space->page_tables;

    UVM_ASSERT(uvm_mmu_page_size_supported(page_tree, mem_info->pageSize));

    if (va_range->type == UVM_VA_RANGE_TYPE_EXTERNAL) {
        // We should be never called with ext_gpu_map == NULL
        // and UVM_VA_RANGE_TYPE_EXTERNAL
        UVM_ASSERT(ext_gpu_map != NULL);
        node = &ext_gpu_map->node;
        pt_range_vec = &ext_gpu_map->pt_range_vec;
    }
    else {
        node = &va_range->node;
        pt_range_vec = &va_range->channel.pt_range_vec;
    }

    if (map_offset + uvm_range_tree_node_size(node) > mem_info->size)
        return NV_ERR_INVALID_OFFSET;

    UVM_ASSERT(IS_ALIGNED(node->start, mem_info->pageSize) &&
               IS_ALIGNED(node->end + 1, mem_info->pageSize) &&
               IS_ALIGNED(map_offset, mem_info->pageSize));

    status = uvm_pte_buffer_init(va_range,
                                 mapping_gpu,
                                 map_rm_params,
                                 uvm_range_tree_node_size(node),
                                 mem_info->pageSize,
                                 &pte_buffer);
    if (status != NV_OK)
        return status;

    // Allocate all page tables for this VA range.
    //
    // TODO: Bug 1766649: Benchmark to see if we get any performance improvement
    //       from parallelizing page range allocation with writing PTEs for
    //       earlier ranges.
    status = uvm_page_table_range_vec_init(page_tree,
                                           node->start,
                                           uvm_range_tree_node_size(node),
                                           mem_info->pageSize,
                                           UVM_PMM_ALLOC_FLAGS_EVICT,
                                           pt_range_vec);
    if (status != NV_OK)
        goto out;

    addr = node->start;
    for (i = 0; i < pt_range_vec->range_count; i++) {
        pt_range = &pt_range_vec->ranges[i];

        // External allocations track pushes in their own trackers. User channel
        // mappings don't have their own trackers, so for those the local tracker
        // is used.
        status = map_rm_pt_range(page_tree,
                                 pt_range,
                                 &pte_buffer,
                                 node,
                                 ext_gpu_map ? ext_gpu_map->mem_handle->rm_handle : 0,
                                 addr,
                                 map_offset,
                                 tracker);
        if (status != NV_OK)
            goto out;

        size = uvm_page_table_range_size(pt_range);
        addr += size;
        map_offset += size;
    }

    status = uvm_tracker_add_tracker(out_tracker, tracker);

out:
    if (status != NV_OK) {
        // We could have any number of mappings in flight to these page tables,
        // so wait for everything before we clear and free them.
        if (uvm_tracker_wait(tracker) != NV_OK) {
            // System-fatal error. Just leak.
            return status;
        }

        if (pt_range_vec->ranges) {
            uvm_page_table_range_vec_clear_ptes(pt_range_vec, va_range_downgrade_membar(va_range, ext_gpu_map));
            uvm_page_table_range_vec_deinit(pt_range_vec);
        }
    }

    uvm_pte_buffer_deinit(&pte_buffer);
    uvm_tracker_deinit(&local_tracker);
    return status;
}

static bool uvm_api_mapping_type_invalid(UvmGpuMappingType map_type)
{
    BUILD_BUG_ON((int)UvmGpuMappingTypeDefault != (int)UvmRmGpuMappingTypeDefault);
    BUILD_BUG_ON((int)UvmGpuMappingTypeReadWriteAtomic != (int)UvmRmGpuMappingTypeReadWriteAtomic);
    BUILD_BUG_ON((int)UvmGpuMappingTypeReadWrite != (int)UvmRmGpuMappingTypeReadWrite);
    BUILD_BUG_ON((int)UvmGpuMappingTypeReadOnly != (int)UvmRmGpuMappingTypeReadOnly);
    BUILD_BUG_ON((int)UvmGpuMappingTypeCount != (int)UvmRmGpuMappingTypeCount);

    switch (map_type) {
        case UvmGpuMappingTypeDefault:
        case UvmGpuMappingTypeReadWriteAtomic:
        case UvmGpuMappingTypeReadWrite:
        case UvmGpuMappingTypeReadOnly:
            return false;
        default:
            return true;
    }
}

static bool uvm_api_caching_type_invalid(UvmGpuCachingType cache_type)
{
    BUILD_BUG_ON((int)UvmGpuCachingTypeDefault != (int)UvmRmGpuCachingTypeDefault);
    BUILD_BUG_ON((int)UvmGpuCachingTypeForceUncached != (int)UvmRmGpuCachingTypeForceUncached);
    BUILD_BUG_ON((int)UvmGpuCachingTypeForceCached != (int)UvmRmGpuCachingTypeForceCached);
    BUILD_BUG_ON((int)UvmGpuCachingTypeCount != (int)UvmRmGpuCachingTypeCount);

    switch (cache_type) {
        case UvmGpuCachingTypeDefault:
        case UvmGpuCachingTypeForceUncached:
        case UvmGpuCachingTypeForceCached:
            return false;
        default:
            return true;
    }
}

static bool uvm_api_kind_type_invalid(UvmGpuFormatType format_type,
                                      UvmGpuFormatElementBits element_bits,
                                      UvmGpuCompressionType compression_type)
{
    BUILD_BUG_ON((int)UvmGpuFormatTypeDefault != (int)UvmRmGpuFormatTypeDefault);
    BUILD_BUG_ON((int)UvmGpuFormatTypeBlockLinear != (int)UvmRmGpuFormatTypeBlockLinear);
    BUILD_BUG_ON((int)UvmGpuFormatTypeCount != (int)UvmRmGpuFormatTypeCount);

    BUILD_BUG_ON((int)UvmGpuFormatElementBitsDefault != (int)UvmRmGpuFormatElementBitsDefault);
    BUILD_BUG_ON((int)UvmGpuFormatElementBits8 != (int)UvmRmGpuFormatElementBits8);
    BUILD_BUG_ON((int)UvmGpuFormatElementBits16 != (int)UvmRmGpuFormatElementBits16);
    BUILD_BUG_ON((int)UvmGpuFormatElementBits32 != (int)UvmRmGpuFormatElementBits32);
    BUILD_BUG_ON((int)UvmGpuFormatElementBits64 != (int)UvmRmGpuFormatElementBits64);
    BUILD_BUG_ON((int)UvmGpuFormatElementBits128 != (int)UvmRmGpuFormatElementBits128);
    BUILD_BUG_ON((int)UvmGpuFormatElementBitsCount != (int)UvmRmGpuFormatElementBitsCount);

    BUILD_BUG_ON((int)UvmGpuCompressionTypeDefault != (int)UvmRmGpuCompressionTypeDefault);
    BUILD_BUG_ON((int)UvmGpuCompressionTypeEnabledNoPlc != (int)UvmRmGpuCompressionTypeEnabledNoPlc);
    BUILD_BUG_ON((int)UvmGpuCompressionTypeCount != (int)UvmRmGpuCompressionTypeCount);

    if (compression_type >= UvmGpuCompressionTypeCount)
        return true;

    switch (format_type) {
        case UvmGpuFormatTypeDefault:
        case UvmGpuFormatTypeBlockLinear:
            break;
        default:
            return true;
    }

    switch (element_bits) {
        case UvmGpuFormatElementBitsDefault:
        case UvmGpuFormatElementBits8:
        case UvmGpuFormatElementBits16:
        // CUDA does not support 24-bit width
        case UvmGpuFormatElementBits32:
        case UvmGpuFormatElementBits64:
        case UvmGpuFormatElementBits128:
            break;
        default:
            return true;
    }

    if (((format_type != UvmGpuFormatTypeDefault) && (element_bits == UvmGpuFormatElementBitsDefault)) ||
        ((element_bits != UvmGpuFormatElementBitsDefault) && (format_type == UvmGpuFormatTypeDefault)))
        return true;

    return false;
}

static void uvm_release_rm_handle(struct nv_kref *ref)
{
    uvm_ext_gpu_mem_handle *mem_handle = container_of(ref, uvm_ext_gpu_mem_handle, ref_count);

    if (mem_handle->rm_handle) {
        NV_STATUS status;

        status = uvm_rm_locked_call(nvUvmInterfaceFreeDupedHandle(uvm_gpu_device_handle(mem_handle->gpu),
                                                                  mem_handle->rm_handle));
        UVM_ASSERT(status == NV_OK);
    }
    uvm_kvfree(mem_handle);
}

static NV_STATUS uvm_create_external_range(uvm_va_space_t *va_space, UVM_CREATE_EXTERNAL_RANGE_PARAMS *params)
{
    uvm_va_range_t *va_range = NULL;
    struct mm_struct *mm;
    NV_STATUS status = NV_OK;

    // Before we know the page size used by the allocation, we can only enforce
    // 4K alignment as that's the minimum page size used for GPU allocations.
    // Later uvm_map_external_allocation_on_gpu() will enforce alignment to the
    // page size used by the allocation.
    if (uvm_api_range_invalid_4k(params->base, params->length))
        return NV_ERR_INVALID_ADDRESS;

    // The mm needs to be locked in order to remove stale HMM va_blocks.
    mm = uvm_va_space_mm_or_current_retain_lock(va_space);
    uvm_va_space_down_write(va_space);

    // Create the new external VA range.
    // uvm_va_range_create_external handles any collisions when it attempts to
    // insert the new range into the va_space range tree.
    status = uvm_va_range_create_external(va_space, mm, params->base, params->length, &va_range);
    if (status != NV_OK) {
        UVM_DBG_PRINT_RL("Failed to create external VA range [0x%llx, 0x%llx)\n",
                         params->base,
                         params->base + params->length);
    }

    uvm_va_space_up_write(va_space);
    uvm_va_space_mm_or_current_release_unlock(va_space, mm);
    return status;
}

NV_STATUS uvm_api_create_external_range(UVM_CREATE_EXTERNAL_RANGE_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    return uvm_create_external_range(va_space, params);
}

static NV_STATUS set_ext_gpu_map_location(uvm_ext_gpu_map_t *ext_gpu_map,
                                          uvm_va_space_t *va_space,
                                          uvm_gpu_t *mapping_gpu,
                                          const UvmGpuMemoryInfo *mem_info)
{
    uvm_gpu_t *owning_gpu;

    if (mem_info->egm)
        UVM_ASSERT(mem_info->sysmem);

    // !mem_info->deviceDescendant && !mem_info->sysmem imply fabric allocation.
    // !mem_info->deviceDescendant also means that mem_info->uuid is invalid. In
    // this case the owning GPU is NULL, meaning that UVM is oblivious to the
    // topology and relies on RM and/or the fabric manager (FM) for memory
    // lifetime management and GPU ref counting.
    if (!mem_info->deviceDescendant && !mem_info->sysmem) {
        ext_gpu_map->owning_gpu = NULL;
        ext_gpu_map->is_sysmem = false;
        return NV_OK;
    }
    // This is a local or peer allocation, so the owning GPU must have been
    // registered. This also checks for if EGM owning GPU is registered.
    owning_gpu = uvm_va_space_get_gpu_by_uuid(va_space, &mem_info->uuid);
    if (!owning_gpu)
        return NV_ERR_INVALID_DEVICE;

    // Even if the allocation is in sysmem then it still matters which GPU owns
    // it, because our dup is not enough to keep the owning GPU around and that
    // exposes a bug in RM where the memory can outlast the GPU and then cause
    // crashes when it's eventually freed.
    // TODO: Bug 1811006: Bug tracking the RM issue, its fix might change the
    // semantics of sysmem allocations.

    // Check if peer access for peer memory is enabled.
    if (owning_gpu != mapping_gpu && (!mem_info->sysmem || mem_info->egm)) {
        // TODO: Bug 1757136: In SLI, the returned UUID may be different but a
        //       local mapping must be used. We need to query SLI groups to know
        //       that.
        if (!uvm_va_space_peer_enabled(va_space, mapping_gpu, owning_gpu))
            return NV_ERR_INVALID_DEVICE;
    }

    ext_gpu_map->owning_gpu = owning_gpu;
    ext_gpu_map->is_sysmem = mem_info->sysmem;
    ext_gpu_map->is_egm = mem_info->egm;

    return NV_OK;
}

static uvm_ext_gpu_map_t *uvm_va_range_ext_gpu_map(uvm_va_range_t *va_range, uvm_gpu_t *mapping_gpu, NvU64 addr)
{
    uvm_ext_gpu_map_t *ext_gpu_map = NULL;
    uvm_range_tree_node_t *node;
    uvm_ext_gpu_range_tree_t *range_tree;

    UVM_ASSERT(va_range->type == UVM_VA_RANGE_TYPE_EXTERNAL);
    uvm_assert_rwsem_locked(&va_range->va_space->lock);

    range_tree = uvm_ext_gpu_range_tree(va_range, mapping_gpu);

    if (uvm_processor_mask_test(&va_range->external.mapped_gpus, mapping_gpu->id)) {
        UVM_ASSERT(!uvm_range_tree_empty(&range_tree->tree));
        node = uvm_range_tree_find(&range_tree->tree, addr);
        if (node) {
            ext_gpu_map = uvm_ext_gpu_map_container(node);
            UVM_ASSERT(ext_gpu_map->gpu == mapping_gpu);
        }
    }
    else {
        UVM_ASSERT(uvm_range_tree_empty(&range_tree->tree));
    }

    return ext_gpu_map;
}

static NV_STATUS uvm_ext_gpu_map_split(uvm_range_tree_t *tree,
                                       uvm_ext_gpu_map_t *existing_map,
                                       NvU64 new_end,
                                       uvm_ext_gpu_map_t **new_map)
{
    uvm_ext_gpu_map_t *new;
    NV_STATUS status;
    NvU64 new_start = new_end + 1;

    if (!IS_ALIGNED(new_start, existing_map->pt_range_vec.page_size))
        return NV_ERR_INVALID_ADDRESS;

    UVM_ASSERT(new_start >= existing_map->node.start && new_start < existing_map->node.end);

    new = uvm_kvmalloc_zero(sizeof(*new));
    if (!new)
        return NV_ERR_NO_MEMORY;

    RB_CLEAR_NODE(&new->node.rb_node);
    new->mem_handle = existing_map->mem_handle;
    new->gpu = existing_map->gpu;
    new->owning_gpu = existing_map->owning_gpu;
    new->is_sysmem = existing_map->is_sysmem;
    new->is_egm = existing_map->is_egm;

    // Initialize the new ext_gpu_map tracker as a copy of the existing_map tracker.
    // This way, any operations on any of the two ext_gpu_maps will be able to
    // wait for any uncompleted work prior to the split.
    status = uvm_tracker_init_from(&new->tracker, &existing_map->tracker);
    if (status != NV_OK) {
        uvm_kvfree(new);
        return status;
    }

    status = uvm_page_table_range_vec_split_upper(&existing_map->pt_range_vec, new_start - 1, &new->pt_range_vec);
    if (status != NV_OK) {
        uvm_tracker_deinit(&new->tracker);
        uvm_kvfree(new);
        return status;
    }

    new->node.start = new_start;

    // Sparse mappings don't have actual allocations.
    if (new->mem_handle)
        nv_kref_get(&new->mem_handle->ref_count);

    uvm_range_tree_split(tree, &existing_map->node, &new->node);

    if (new_map)
        *new_map = new;

    return NV_OK;
}

static NV_STATUS uvm_unmap_external_in_range(uvm_va_range_t *va_range,
                                             uvm_gpu_t *gpu,
                                             NvU64 start,
                                             NvU64 end,
                                             struct list_head *deferred_list)
{
    uvm_ext_gpu_range_tree_t *range_tree = uvm_ext_gpu_range_tree(va_range, gpu);
    uvm_ext_gpu_map_t *ext_map, *ext_map_next = NULL;
    NV_STATUS status = NV_OK;

    uvm_assert_mutex_locked(&range_tree->lock);

    // If a previously existing sub-range is found (ext_map != NULL), the
    // new sub-range can be overlapping with the existing one in one of the
    // following ways:
    //
    //   1. complete overlap (exact start and end boundary match is special
    //      cases of this):
    //           [---- existing ----]
    //       [----       new        ----]
    //   2. partial overlap at the start (end boundary match is a special case
    //      of this):
    //           [---- existing ----]
    //               [----    new    ----]
    //   3. partial overlap at the end (start boundary match is a special case
    //      of this):
    //           [---- existing ----]
    //       [----   new    ----]
    //   4. completely contained (start of new != start of existing and end of
    //      new != end of existing, otherwise see 1):
    //           [---- existing ----]
    //                [-- new --]
    //
    // The algorithm below is:
    //   1. If the start of the new mapping is greater than the start of the
    //      existing mapping, split the existing mapping at start. The newly
    //      created uvm_ext_gpu_map_t will be inserted into the tree. Note that
    //      the newly created uvm_ext_gpu_map_t is the one that we want to visit
    //      next. When the loop visits the newly created uvm_ext_gpu_map_t and
    //      its boundaries are completely overlapped by the new mapping, it will
    //      cause the algorithm to destroy it.
    //   2. If the end of the new mapping is less than the end of the existing
    //      mapping, split the existing mapping at end. The newly created
    //      uvm_ext_gpu_map_t will be inserted into the tree. The overlapping
    //      portion of the existing mapping will be destroyed.
    //   3. If the existing mapping is completely overlapped by the new mapping,
    //      the existing mapping is destroyed.
    //
    // The loop cannot use any of the existing iterators because:
    //   1. It needs to be able to destroy ext_gpu_map structures. This means it
    //      can't use non-safe iterators.
    //   2. It needs to visit newly created uvm_ext_gpu_map_t, as a result of
    //      splits. This means it can't use safe iterators as they will skip the
    //      newly created uvm_ext_gpu_map_t.
    ext_map = uvm_ext_gpu_map_iter_first(va_range, gpu, start, end);
    while (ext_map) {
        if (start > ext_map->node.start) {
            status = uvm_ext_gpu_map_split(&range_tree->tree, ext_map, start - 1, &ext_map_next);
            if (status != NV_OK)
                break;
        }
        else {
            if (end < ext_map->node.end) {
                status = uvm_ext_gpu_map_split(&range_tree->tree, ext_map, end, NULL);
                if (status != NV_OK)
                    break;
                ext_map_next = NULL;
            }
            else {
                ext_map_next = uvm_ext_gpu_map_iter_next(va_range, ext_map, end);
            }

            uvm_ext_gpu_map_destroy(va_range, ext_map, deferred_list);
        }

        ext_map = ext_map_next;
    }

    return status;
}

static NV_STATUS uvm_map_external_allocation_on_gpu(uvm_va_range_t *va_range,
                                                    uvm_gpu_t *mapping_gpu,
                                                    const uvm_rm_user_object_t *user_rm_mem,
                                                    const uvm_map_rm_params_t *map_rm_params,
                                                    NvU64 base,
                                                    NvU64 length,
                                                    uvm_tracker_t *out_tracker)
{
    uvm_va_space_t *va_space = va_range->va_space;
    uvm_ext_gpu_map_t *ext_gpu_map = NULL;
    uvm_ext_gpu_range_tree_t *range_tree = uvm_ext_gpu_range_tree(va_range, mapping_gpu);
    UvmGpuMemoryInfo mem_info;
    uvm_gpu_va_space_t *gpu_va_space = uvm_gpu_va_space_get(va_space, mapping_gpu);
    NvU64 mapping_page_size;
    NvU64 biggest_mapping_page_size;
    NvU64 alignments;
    NvU64 smallest_alignment;
    NV_STATUS status;

    uvm_assert_rwsem_locked_read(&va_space->lock);

    if ((map_rm_params->compression_type == UvmGpuCompressionTypeEnabledNoPlc) && !mapping_gpu->parent->plc_supported)
        return NV_ERR_INVALID_DEVICE;

    // Check if the GPU can access the VA
    if (!uvm_gpu_can_address(mapping_gpu, base, length))
        return NV_ERR_OUT_OF_RANGE;

    uvm_mutex_lock(&range_tree->lock);

    status = uvm_unmap_external_in_range(va_range, mapping_gpu, base, base + length - 1, NULL);
    if (status != NV_OK)
        goto error;

    ext_gpu_map = uvm_kvmalloc_zero(sizeof(*ext_gpu_map));
    if (!ext_gpu_map) {
        status = NV_ERR_NO_MEMORY;
        goto error;
    }

    // Insert the ext_gpu_map into the VA range immediately since some of the
    // below calls require it to be there.
    ext_gpu_map->node.start = base;
    ext_gpu_map->node.end = base + length - 1;
    RB_CLEAR_NODE(&ext_gpu_map->node.rb_node);
    uvm_tracker_init(&ext_gpu_map->tracker);
    ext_gpu_map->mem_handle = uvm_kvmalloc_zero(sizeof(*ext_gpu_map->mem_handle));
    if (!ext_gpu_map->mem_handle) {
        status = NV_ERR_NO_MEMORY;
        goto error;
    }

    // Due to the fact that any overlapping mappings were already unmapped,
    // adding the new mapping to the tree cannot fail.
    status = uvm_range_tree_add(&range_tree->tree, &ext_gpu_map->node);
    UVM_ASSERT(status == NV_OK);

    uvm_processor_mask_set_atomic(&va_range->external.mapped_gpus, mapping_gpu->id);
    ext_gpu_map->gpu = mapping_gpu;
    ext_gpu_map->mem_handle->gpu = mapping_gpu;
    nv_kref_init(&ext_gpu_map->mem_handle->ref_count);

    // Error paths after this point may call uvm_va_range_ext_gpu_map, so do a
    // sanity check now to make sure it doesn't trigger any asserts.
    UVM_ASSERT(uvm_va_range_ext_gpu_map(va_range, mapping_gpu, base) == ext_gpu_map);

    // Dup the memory. This verifies the input handles, takes a ref count on the
    // physical allocation so it can't go away under us, and returns us the
    // allocation info.
    status = uvm_rm_locked_call(nvUvmInterfaceDupMemory(uvm_gpu_device_handle(mapping_gpu),
                                                        user_rm_mem->user_client,
                                                        user_rm_mem->user_object,
                                                        &ext_gpu_map->mem_handle->rm_handle,
                                                        &mem_info));
    if (status != NV_OK) {
        UVM_DBG_PRINT("Failed to dup memory handle {0x%x, 0x%x}: %s, GPU: %s\n",
                      user_rm_mem->user_client,
                      user_rm_mem->user_object,
                      nvstatusToString(status),
                      uvm_gpu_name(mapping_gpu));
        goto error;
    }

    status = set_ext_gpu_map_location(ext_gpu_map, va_space, mapping_gpu, &mem_info);
    if (status != NV_OK)
        goto error;

    // Determine the proper mapping page size.
    // This will be the largest supported page size less than or equal to the
    // smallest of the base VA address, length, offset, and allocation page size
    // alignments.
    alignments = mem_info.pageSize | base | length | map_rm_params->map_offset;
    smallest_alignment = alignments & ~(alignments - 1);

    // Check that alignment bits did not get truncated.
    UVM_ASSERT(smallest_alignment);

    mapping_page_size = uvm_mmu_biggest_page_size_up_to(&gpu_va_space->page_tables, smallest_alignment);
    if (!mapping_page_size) {
        status = NV_ERR_INVALID_ADDRESS;
        goto error;
    }

    // Check for the maximum page size for the mapping of vidmem allocations,
    // the vMMU segment size may limit the range of page sizes.
    biggest_mapping_page_size = uvm_mmu_biggest_page_size_up_to(&gpu_va_space->page_tables,
                                                                mapping_gpu->mem_info.max_vidmem_page_size);
    if (!ext_gpu_map->is_sysmem && (ext_gpu_map->gpu == ext_gpu_map->owning_gpu) &&
        (mapping_page_size > biggest_mapping_page_size))
        mapping_page_size = biggest_mapping_page_size;

    mem_info.pageSize = mapping_page_size;

    status = uvm_va_range_map_rm_allocation(va_range, mapping_gpu, &mem_info, map_rm_params, ext_gpu_map, out_tracker);
    if (status != NV_OK)
        goto error;

    uvm_mutex_unlock(&range_tree->lock);
    return NV_OK;

error:
    uvm_ext_gpu_map_destroy(va_range, ext_gpu_map, NULL);
    uvm_mutex_unlock(&range_tree->lock);
    return status;
}

// Actual implementation of UvmMapExternalAllocation
static NV_STATUS uvm_map_external_allocation(uvm_va_space_t *va_space, UVM_MAP_EXTERNAL_ALLOCATION_PARAMS *params)
{
    uvm_va_range_t *va_range = NULL;
    uvm_gpu_t *mapping_gpu;
    uvm_processor_mask_t *mapped_gpus;
    NV_STATUS status = NV_OK;
    size_t i;
    uvm_map_rm_params_t map_rm_params;
    uvm_rm_user_object_t user_rm_mem =
    {
        .rm_control_fd = params->rmCtrlFd,
        .user_client   = params->hClient,
        .user_object   = params->hMemory
    };
    uvm_tracker_t tracker = UVM_TRACKER_INIT();

    if (uvm_api_range_invalid_4k(params->base, params->length))
        return NV_ERR_INVALID_ADDRESS;

    if (params->gpuAttributesCount == 0 || params->gpuAttributesCount > UVM_MAX_GPUS)
        return NV_ERR_INVALID_ARGUMENT;

    mapped_gpus = uvm_processor_mask_cache_alloc();
    if (!mapped_gpus)
        return NV_ERR_NO_MEMORY;

    uvm_va_space_down_read_rm(va_space);
    va_range = uvm_va_range_find(va_space, params->base);

    if (!va_range ||
        va_range->type != UVM_VA_RANGE_TYPE_EXTERNAL ||
        va_range->node.end < params->base + params->length - 1) {
        uvm_va_space_up_read_rm(va_space);
        uvm_processor_mask_cache_free(mapped_gpus);
        return NV_ERR_INVALID_ADDRESS;
    }

    uvm_processor_mask_zero(mapped_gpus);
    for (i = 0; i < params->gpuAttributesCount; i++) {
        if (uvm_api_mapping_type_invalid(params->perGpuAttributes[i].gpuMappingType) ||
            uvm_api_caching_type_invalid(params->perGpuAttributes[i].gpuCachingType) ||
            uvm_api_kind_type_invalid(params->perGpuAttributes[i].gpuFormatType,
                                      params->perGpuAttributes[i].gpuElementBits,
                                      params->perGpuAttributes[i].gpuCompressionType)) {
            status = NV_ERR_INVALID_ARGUMENT;
            goto error;
        }

        mapping_gpu = uvm_va_space_get_gpu_by_uuid_with_gpu_va_space(va_space, &params->perGpuAttributes[i].gpuUuid);
        if (!mapping_gpu) {
            status = NV_ERR_INVALID_DEVICE;
            goto error;
        }

        // Use a tracker to get as much parallelization as possible among GPUs,
        // so one GPU can have its PTE writes in flight while we're working on
        // the next one.
        map_rm_params.map_offset = params->offset;
        map_rm_params.mapping_type = params->perGpuAttributes[i].gpuMappingType;
        map_rm_params.caching_type = params->perGpuAttributes[i].gpuCachingType;
        map_rm_params.format_type = params->perGpuAttributes[i].gpuFormatType;
        map_rm_params.element_bits = params->perGpuAttributes[i].gpuElementBits;
        map_rm_params.compression_type = params->perGpuAttributes[i].gpuCompressionType;
        status = uvm_map_external_allocation_on_gpu(va_range,
                                                    mapping_gpu,
                                                    &user_rm_mem,
                                                    &map_rm_params,
                                                    params->base,
                                                    params->length,
                                                    &tracker);
        if (status != NV_OK)
            goto error;

        uvm_processor_mask_set(mapped_gpus, mapping_gpu->id);
    }

    // Wait for outstanding page table operations to finish across all GPUs. We
    // just need to hold the VA space lock to prevent the GPUs on which we're
    // waiting from getting unregistered underneath us.
    status = uvm_tracker_wait_deinit(&tracker);

    uvm_va_space_up_read_rm(va_space);
    uvm_processor_mask_cache_free(mapped_gpus);

    return status;

error:
    // We still have to wait for page table writes to finish, since the teardown
    // could free them.
    (void)uvm_tracker_wait_deinit(&tracker);

    // Tear down only those mappings we created during this call
    for_each_va_space_gpu_in_mask(mapping_gpu, va_space, mapped_gpus) {
        uvm_ext_gpu_range_tree_t *range_tree = uvm_ext_gpu_range_tree(va_range, mapping_gpu);
        uvm_ext_gpu_map_t *ext_map, *ext_map_next;

        uvm_mutex_lock(&range_tree->lock);
        uvm_ext_gpu_map_for_each_in_safe(ext_map,
                                         ext_map_next,
                                         va_range,
                                         mapping_gpu,
                                         params->base,
                                         params->base + params->length - 1)
            uvm_ext_gpu_map_destroy(va_range, ext_map, NULL);
        uvm_mutex_unlock(&range_tree->lock);
    }

    uvm_va_space_up_read_rm(va_space);
    uvm_processor_mask_cache_free(mapped_gpus);

    return status;
}

NV_STATUS uvm_api_map_external_allocation(UVM_MAP_EXTERNAL_ALLOCATION_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    return uvm_map_external_allocation(va_space, params);
}

static NvU64 external_sparse_pte_maker(uvm_page_table_range_vec_t *range_vec, NvU64 offset, void *caller_data)
{
    return range_vec->tree->hal->make_sparse_pte();
}

static NV_STATUS uvm_map_external_sparse_on_gpu(uvm_va_range_t *va_range,
                                                uvm_gpu_t *mapping_gpu,
                                                NvU64 base,
                                                NvU64 length,
                                                struct list_head *deferred_free_list)
{
    uvm_va_space_t *va_space = va_range->va_space;
    uvm_ext_gpu_map_t *ext_gpu_map = NULL;
    uvm_ext_gpu_range_tree_t *range_tree = uvm_ext_gpu_range_tree(va_range, mapping_gpu);
    uvm_gpu_va_space_t *gpu_va_space = uvm_gpu_va_space_get(va_space, mapping_gpu);
    uvm_page_tree_t *page_tree;
    NV_STATUS status;

    uvm_assert_rwsem_locked(&va_space->lock);

    if (!uvm_gpu_can_address(mapping_gpu, base, length))
        return NV_ERR_OUT_OF_RANGE;

    UVM_ASSERT(gpu_va_space);

    page_tree = &gpu_va_space->page_tables;

    uvm_mutex_lock(&range_tree->lock);

    status = uvm_unmap_external_in_range(va_range, mapping_gpu, base, base + length - 1, deferred_free_list);
    if (status != NV_OK)
        goto error;

    ext_gpu_map = uvm_kvmalloc_zero(sizeof(*ext_gpu_map));
    if (!ext_gpu_map) {
        status = NV_ERR_NO_MEMORY;
        goto error;
    }

    ext_gpu_map->node.start = base;
    ext_gpu_map->node.end = base + length - 1;
    RB_CLEAR_NODE(&ext_gpu_map->node.rb_node);
    uvm_tracker_init(&ext_gpu_map->tracker);

    // Due to the fact that any overlapping mappings were already unmapped,
    // adding the new mapping to the tree cannot fail.
    status = uvm_range_tree_add(&range_tree->tree, &ext_gpu_map->node);
    UVM_ASSERT(status == NV_OK);

    uvm_processor_mask_set_atomic(&va_range->external.mapped_gpus, mapping_gpu->id);
    ext_gpu_map->gpu = mapping_gpu;

    UVM_ASSERT(uvm_va_range_ext_gpu_map(va_range, mapping_gpu, base) == ext_gpu_map);

    status = uvm_page_table_range_vec_init(page_tree,
                                           ext_gpu_map->node.start,
                                           uvm_range_tree_node_size(&ext_gpu_map->node),
                                           UVM_PAGE_SIZE_64K,
                                           UVM_PMM_ALLOC_FLAGS_EVICT,
                                           &ext_gpu_map->pt_range_vec);
    if (status != NV_OK)
        goto error;

    status = uvm_page_table_range_vec_write_ptes(&ext_gpu_map->pt_range_vec,
                                                 UVM_MEMBAR_NONE,
                                                 external_sparse_pte_maker,
                                                 NULL);
    if (status != NV_OK)
        goto error;

    uvm_mutex_unlock(&range_tree->lock);
    return NV_OK;

error:
    uvm_ext_gpu_map_destroy(va_range, ext_gpu_map, NULL);
    uvm_mutex_unlock(&range_tree->lock);
    return status;
}

static NV_STATUS uvm_map_external_sparse(uvm_va_space_t *va_space, UVM_MAP_EXTERNAL_SPARSE_PARAMS *params)
{
    uvm_va_range_t *va_range = NULL;
    uvm_gpu_t *mapping_gpu = NULL;
    NV_STATUS status = NV_OK;
    LIST_HEAD(deferred_free_list);

    if (uvm_api_range_invalid_64k(params->base, params->length))
        return NV_ERR_INVALID_ADDRESS;

    uvm_va_space_down_read(va_space);
    va_range = uvm_va_range_find(va_space, params->base);
    if (!va_range ||
        va_range->type != UVM_VA_RANGE_TYPE_EXTERNAL ||
        va_range->node.end < params->base + params->length - 1) {
        status = NV_ERR_INVALID_ADDRESS;
        goto out;
    }

    mapping_gpu = uvm_va_space_get_gpu_by_uuid_with_gpu_va_space(va_space, &params->gpuUuid);
    if (!mapping_gpu) {
        status = NV_ERR_INVALID_DEVICE;
        goto out;
    }

    // Sparse mappings are unsupported on GPUs prior to Pascal.
    if (!mapping_gpu->parent->sparse_mappings_supported) {
        status = NV_ERR_INVALID_DEVICE;
        goto out;
    }

    status = uvm_map_external_sparse_on_gpu(va_range, mapping_gpu, params->base, params->length, &deferred_free_list);

    if (!list_empty(&deferred_free_list))
        uvm_gpu_retain(mapping_gpu);

out:
    uvm_va_space_up_read(va_space);

    if (!list_empty(&deferred_free_list)) {
        uvm_deferred_free_object_list(&deferred_free_list);
        uvm_gpu_release(mapping_gpu);
    }

    return status;
}

NV_STATUS uvm_api_map_external_sparse(UVM_MAP_EXTERNAL_SPARSE_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    return uvm_map_external_sparse(va_space, params);
}

// Version of free which returns but doesn't release the owning GPU
static uvm_gpu_t *uvm_ext_gpu_map_free_internal(uvm_ext_gpu_map_t *ext_gpu_map)
{
    uvm_gpu_t *owning_gpu;

    if (!ext_gpu_map)
        return NULL;

    UVM_ASSERT(!ext_gpu_map->pt_range_vec.ranges);

    if (ext_gpu_map->mem_handle)
        nv_kref_put(&ext_gpu_map->mem_handle->ref_count, uvm_release_rm_handle);

    owning_gpu = ext_gpu_map->owning_gpu;
    uvm_kvfree(ext_gpu_map);

    return owning_gpu;
}

void uvm_ext_gpu_map_free(uvm_ext_gpu_map_t *ext_gpu_map)
{
    uvm_gpu_t *owning_gpu = uvm_ext_gpu_map_free_internal(ext_gpu_map);
    if (owning_gpu)
        uvm_gpu_release(owning_gpu);
}

void uvm_ext_gpu_map_destroy(uvm_va_range_t *va_range,
                             uvm_ext_gpu_map_t *ext_gpu_map,
                             struct list_head *deferred_free_list)
{
    uvm_membar_t membar;
    uvm_ext_gpu_range_tree_t *range_tree;
    uvm_gpu_t *mapped_gpu;

    if (!ext_gpu_map)
        return;

    (void)uvm_tracker_wait_deinit(&ext_gpu_map->tracker);

    // The external map is inserted into the tree prior to the rest of the mapping
    // steps. So, if it has not been inserted yet, there is nothing to clean up. Just
    // free the memory.
    if (RB_EMPTY_NODE(&ext_gpu_map->node.rb_node)) {
        uvm_kvfree(ext_gpu_map->mem_handle);
        uvm_kvfree(ext_gpu_map);
        return;
    }

    mapped_gpu = ext_gpu_map->gpu;

    range_tree = uvm_ext_gpu_range_tree(va_range, mapped_gpu);

    uvm_assert_mutex_locked(&range_tree->lock);
    UVM_ASSERT(uvm_gpu_va_space_get(va_range->va_space, mapped_gpu));

    uvm_range_tree_remove(&range_tree->tree, &ext_gpu_map->node);

    // Unmap the PTEs
    if (ext_gpu_map->pt_range_vec.ranges) {
        membar = va_range_downgrade_membar(va_range, ext_gpu_map);
        uvm_page_table_range_vec_clear_ptes(&ext_gpu_map->pt_range_vec, membar);
        uvm_page_table_range_vec_deinit(&ext_gpu_map->pt_range_vec);
    }

    if (deferred_free_list && ext_gpu_map->mem_handle) {
        // If this is a GPU allocation, we have to prevent that GPU from going
        // away until we've freed the handle.
        if (ext_gpu_map->owning_gpu)
            uvm_gpu_retain(ext_gpu_map->owning_gpu);

        uvm_deferred_free_object_add(deferred_free_list,
                                     &ext_gpu_map->deferred_free,
                                     UVM_DEFERRED_FREE_OBJECT_TYPE_EXTERNAL_ALLOCATION);
    }
    else {
        uvm_ext_gpu_map_free_internal(ext_gpu_map);
    }

    // Check if the sub-range tree is empty. Only then can the GPU be removed from
    // the mapped_gpus bitmap.
    if (uvm_range_tree_empty(&range_tree->tree))
        uvm_processor_mask_clear_atomic(&va_range->external.mapped_gpus, mapped_gpu->id);
}

static NV_STATUS uvm_unmap_external(uvm_va_space_t *va_space,
                                    NvU64 base,
                                    NvU64 length,
                                    const NvProcessorUuid *gpu_uuid)
{
    uvm_va_range_t *va_range;
    uvm_gpu_t *gpu = NULL;
    NV_STATUS status = NV_OK;
    uvm_ext_gpu_range_tree_t *range_tree;
    LIST_HEAD(deferred_free_list);

    if (uvm_api_range_invalid_4k(base, length))
        return NV_ERR_INVALID_ADDRESS;

    uvm_va_space_down_read(va_space);

    va_range = uvm_va_range_find(va_space, base);
    if (!va_range || va_range->type != UVM_VA_RANGE_TYPE_EXTERNAL || base + length - 1 > va_range->node.end) {
        status = NV_ERR_INVALID_ADDRESS;
        goto out;
    }

    gpu = uvm_va_space_get_gpu_by_uuid(va_space, gpu_uuid);
    if (!gpu) {
        status = NV_ERR_INVALID_DEVICE;
        goto out;
    }

    range_tree = uvm_ext_gpu_range_tree(va_range, gpu);
    uvm_mutex_lock(&range_tree->lock);
    status = uvm_unmap_external_in_range(va_range, gpu, base, base + length - 1, &deferred_free_list);
    uvm_mutex_unlock(&range_tree->lock);

    // If the deferred_free_list is not empty, retain the GPU which maps the
    // allocation because it's the parent of dup_handle. The owning GPU (if any)
    // is retained internally by the deferred free layer.
    if (!list_empty(&deferred_free_list))
        uvm_gpu_retain(gpu);

out:
    uvm_va_space_up_read(va_space);

    if (!list_empty(&deferred_free_list)) {
        uvm_deferred_free_object_list(&deferred_free_list);
        uvm_gpu_release(gpu);
    }

    return status;
}

NV_STATUS uvm_api_unmap_external(UVM_UNMAP_EXTERNAL_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    return uvm_unmap_external(va_space, params->base, params->length, &params->gpuUuid);
}

// This destroys VA ranges created by UvmMapExternalAllocation,
// UvmMapDynamicParallelismRegion, and UvmAllocSemaphorePool *only*. VA ranges
// created by UvmMemMap and UvmAlloc go through mmap/munmap.
static NV_STATUS uvm_free(uvm_va_space_t *va_space, NvU64 base, NvU64 length)
{
    uvm_va_range_t *va_range;
    NV_STATUS status = NV_OK;
    uvm_processor_mask_t *retained_mask = NULL;
    LIST_HEAD(deferred_free_list);

    if (uvm_api_range_invalid_4k(base, length))
        return NV_ERR_INVALID_ADDRESS;

    uvm_va_space_down_write(va_space);

    // Non-managed ranges are defined to not require splitting, so a partial
    // free attempt is an error.
    //
    // TODO: Bug 1763676: The length parameter may be needed for MPS. If not, it
    //       should be removed from the ioctl.
    va_range = uvm_va_range_find(va_space, base);
    if (!va_range                                    ||
        (va_range->type != UVM_VA_RANGE_TYPE_EXTERNAL &&
         va_range->type != UVM_VA_RANGE_TYPE_SKED_REFLECTED &&
         va_range->type != UVM_VA_RANGE_TYPE_SEMAPHORE_POOL) ||
        va_range->node.start != base                 ||
        va_range->node.end != base + length - 1) {
        status = NV_ERR_INVALID_ADDRESS;
        goto out;
    }

    if ((va_range->type == UVM_VA_RANGE_TYPE_SEMAPHORE_POOL) &&
        uvm_mem_mapped_on_cpu_user(va_range->semaphore_pool.mem)) {
        // Semaphore pools must be first unmapped from the CPU with munmap to
        // invalidate the vma.
        status = NV_ERR_INVALID_ARGUMENT;
        goto out;
    }

    if (va_range->type == UVM_VA_RANGE_TYPE_EXTERNAL) {
        retained_mask = va_range->external.retained_mask;

        // Set the retained_mask to NULL to prevent
        // uvm_va_range_destroy_external() from freeing the mask.
        va_range->external.retained_mask = NULL;

        UVM_ASSERT(retained_mask);

        // External ranges may have deferred free work, so the GPUs may have to
        // be retained. Construct the mask of all the GPUs that need to be
        // retained.
        uvm_processor_mask_and(retained_mask, &va_range->external.mapped_gpus, &va_space->registered_gpus);
    }

    uvm_va_range_destroy(va_range, &deferred_free_list);

    // If there is deferred work, retain the required GPUs.
    if (!list_empty(&deferred_free_list))
        uvm_global_gpu_retain(retained_mask);

out:
    uvm_va_space_up_write(va_space);

    if (!list_empty(&deferred_free_list)) {
        UVM_ASSERT(status == NV_OK);
        uvm_deferred_free_object_list(&deferred_free_list);
        uvm_global_gpu_release(retained_mask);
    }

    // Free the mask allocated in uvm_va_range_create_external() since
    // uvm_va_range_destroy() won't free this mask.
    uvm_processor_mask_cache_free(retained_mask);

    return status;
}

NV_STATUS uvm_api_free(UVM_FREE_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    return uvm_free(va_space, params->base, params->length);
}
