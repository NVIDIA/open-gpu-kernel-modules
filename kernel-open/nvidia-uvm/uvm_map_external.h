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

#ifndef __UVM_MAP_EXTERNAL_H__
#define __UVM_MAP_EXTERNAL_H__

#include "uvm_forward_decl.h"
#include "uvm_va_range.h"
#include "uvm_tracker.h"
#include "nv_uvm_types.h"
#include "uvm_types.h"

typedef struct
{
    NvU64 map_offset;
    UvmGpuMappingType mapping_type;
    UvmGpuCachingType caching_type;
    UvmGpuFormatType format_type;
    UvmGpuFormatElementBits element_bits;
    UvmGpuCompressionType compression_type;
} uvm_map_rm_params_t;

static uvm_ext_gpu_range_tree_t *uvm_ext_gpu_range_tree(uvm_va_range_external_t *external_range, uvm_gpu_t *gpu)
{
    return &external_range->gpu_ranges[uvm_id_gpu_index(gpu->id)];
}

// Returns the first external map (if any) in the gpu's range tree.
// The caller must hold the range tree lock.
static uvm_ext_gpu_map_t *uvm_ext_gpu_map_iter_first(uvm_va_range_external_t *external_range,
                                                     uvm_gpu_t *gpu,
                                                     NvU64 start,
                                                     NvU64 end)
{
    uvm_ext_gpu_range_tree_t *range_tree;
    uvm_range_tree_node_t *node;

    UVM_ASSERT(start >= external_range->va_range.node.start);
    UVM_ASSERT(end <= external_range->va_range.node.end);

    range_tree = uvm_ext_gpu_range_tree(external_range, gpu);
    node = uvm_range_tree_iter_first(&range_tree->tree, start, end);
    return uvm_ext_gpu_map_container(node);
}

// Returns the external map following the provided map (if any) in address order from
// the gpu's range tree.
// The caller must hold the range tree lock.
static uvm_ext_gpu_map_t *uvm_ext_gpu_map_iter_next(uvm_va_range_external_t *external_range,
                                                    uvm_ext_gpu_map_t *ext_gpu_map,
                                                    NvU64 end)
{
    uvm_ext_gpu_range_tree_t *range_tree;
    uvm_range_tree_node_t *node;

    if (!ext_gpu_map)
        return NULL;

    UVM_ASSERT(end <= external_range->va_range.node.end);

    range_tree = uvm_ext_gpu_range_tree(external_range, ext_gpu_map->gpu);
    node = uvm_range_tree_iter_next(&range_tree->tree, &ext_gpu_map->node, end);
    return uvm_ext_gpu_map_container(node);
}

// The four iterators below require that the caller hold the gpu's range tree
// lock.
#define uvm_ext_gpu_map_for_each_in(ext_gpu_map, external_range, gpu, start, end)               \
    for ((ext_gpu_map) = uvm_ext_gpu_map_iter_first((external_range), (gpu), (start), (end));   \
         (ext_gpu_map);                                                                         \
         (ext_gpu_map) = uvm_ext_gpu_map_iter_next((external_range), (ext_gpu_map), (end)))

#define uvm_ext_gpu_map_for_each_in_safe(ext_gpu_map, ext_gpu_map_next, external_range, gpu, start, end)    \
    for ((ext_gpu_map) = uvm_ext_gpu_map_iter_first((external_range), (gpu), (start), (end)),               \
             (ext_gpu_map_next) = uvm_ext_gpu_map_iter_next((external_range), (ext_gpu_map), (end));        \
         (ext_gpu_map);                                                                                     \
         (ext_gpu_map) = (ext_gpu_map_next),                                                                \
             (ext_gpu_map_next) = uvm_ext_gpu_map_iter_next((external_range), (ext_gpu_map), (end)))

#define uvm_ext_gpu_map_for_each(ext_gpu_map, external_range, gpu)      \
    uvm_ext_gpu_map_for_each_in(ext_gpu_map,                            \
                                external_range,                         \
                                gpu,                                    \
                                (external_range)->va_range.node.start,  \
                                (external_range)->va_range.node.end)

#define uvm_ext_gpu_map_for_each_safe(ext_gpu_map, ext_gpu_map_next, external_range, gpu)   \
    uvm_ext_gpu_map_for_each_in_safe(ext_gpu_map,                                           \
                                     ext_gpu_map_next,                                      \
                                     external_range,                                        \
                                     gpu,                                                   \
                                     (external_range)->va_range.node.start,                 \
                                     (external_range)->va_range.node.end)

// User-facing APIs (uvm_api_map_external_allocation, uvm_api_free) are declared
// uvm_api.h.

// Queries RM for the PTEs appropriate to the VA range and mem_info, allocates
// page tables for the VA range, and writes the PTEs.
//
// va_range must have type UVM_VA_RANGE_TYPE_EXTERNAL or
// UVM_VA_RANGE_TYPE_CHANNEL. The allocation descriptor given to RM is looked up
// depending on the type of the va_range.
// For va_ranges of type UVM_VA_RANGE_TYPE_CHANNEL, the descriptor is looked up
// from the va_range. In these cases, the ext_gpu_map parameter should be NULL.
// For va_ranges of type UVM_VA_RANGE_TYPE_EXTERNAL, it is looked up from the
// ext_gpu_map parameter.
//
// This does not wait for the PTE writes to complete. The work is added to
// the output tracker.
NV_STATUS uvm_va_range_map_rm_allocation(uvm_va_range_t *va_range,
                                         uvm_gpu_t *mapping_gpu,
                                         const UvmGpuMemoryInfo *mem_info,
                                         const uvm_map_rm_params_t *map_rm_params,
                                         uvm_ext_gpu_map_t *ext_gpu_map,
                                         uvm_tracker_t *out_tracker);

// Removes and frees the external mapping for mapping_gpu from ext_gpu_map
// mapped within va_range. If deferred_free_list is NULL, the RM handle is freed
// immediately by this function. Otherwise the GPU which owns the allocation (if
// any) is retained and the handle is added to the list for later processing by
// uvm_deferred_free_object_list.
//
// If the mapping is a Sparse mapping, the mapping is removed and freed.
// However, since sparse mappings do not have RM handles, nothing is added to
// the deferred_free_list (if not NULL) and the GPU is no retained.
//
// The caller must hold the range tree lock for the mapping gpu and is
// responsible for making sure that mapping gpu is retained across those calls.
void uvm_ext_gpu_map_destroy(uvm_va_range_external_t *external_range,
                             uvm_ext_gpu_map_t *ext_gpu_map,
                             struct list_head *deferred_free_list);

// Deferred free function which frees the RM handle and the object itself.
void uvm_ext_gpu_map_free(uvm_ext_gpu_map_t *ext_gpu_map);

#endif // __UVM_MAP_EXTERNAL_H__
