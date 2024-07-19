/*******************************************************************************
    Copyright (c) 2023-2024 NVIDIA Corporation

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

#include "uvm_linux.h"
#include "uvm_processors.h"

static struct kmem_cache *g_uvm_processor_mask_cache __read_mostly;
const uvm_processor_mask_t g_uvm_processor_mask_cpu = { .bitmap = { 1 << UVM_PARENT_ID_CPU_VALUE }};
const uvm_processor_mask_t g_uvm_processor_mask_empty = { };

NV_STATUS uvm_processor_mask_cache_init(void)
{
    BUILD_BUG_ON((8 * sizeof(((uvm_sub_processor_mask_t *)0)->bitmap)) < UVM_PARENT_ID_MAX_SUB_PROCESSORS);

    g_uvm_processor_mask_cache = NV_KMEM_CACHE_CREATE("uvm_processor_mask_t", uvm_processor_mask_t);
    if (!g_uvm_processor_mask_cache)
        return NV_ERR_NO_MEMORY;

    return NV_OK;
}

void uvm_processor_mask_cache_exit(void)
{
    kmem_cache_destroy_safe(&g_uvm_processor_mask_cache);
}

uvm_processor_mask_t *uvm_processor_mask_cache_alloc(void)
{
    return kmem_cache_alloc(g_uvm_processor_mask_cache, NV_UVM_GFP_FLAGS);
}

void uvm_processor_mask_cache_free(uvm_processor_mask_t *mask)
{
    if (mask)
        kmem_cache_free(g_uvm_processor_mask_cache, mask);
}

int uvm_find_closest_node_mask(int src, const nodemask_t *mask)
{
    int nid;
    int closest_nid = NUMA_NO_NODE;

    if (node_isset(src, *mask))
        return src;

    for_each_set_bit(nid, mask->bits, MAX_NUMNODES) {
        if (closest_nid == NUMA_NO_NODE || node_distance(src, nid) < node_distance(src, closest_nid))
            closest_nid = nid;
    }

    return closest_nid;
}

// This implementation avoids having to dynamically allocate a temporary
// uvm_processor_mask_t on the stack (due to size) or the heap (due to possible
// allocation failures).
bool uvm_processor_mask_gpu_subset(const uvm_processor_mask_t *subset,
                                   const uvm_processor_mask_t *mask)
{
    DECLARE_BITMAP(first_word, BITS_PER_LONG);

    BUILD_BUG_ON(UVM_ID_CPU_VALUE > BITS_PER_LONG);
    BUILD_BUG_ON(BITS_PER_LONG > UVM_ID_MAX_PROCESSORS);

    bitmap_copy(first_word, subset->bitmap, BITS_PER_LONG);
    __clear_bit(UVM_ID_CPU_VALUE, first_word);

    return bitmap_subset(first_word, mask->bitmap, BITS_PER_LONG) &&
           bitmap_subset(subset->bitmap + 1, mask->bitmap + 1, UVM_ID_MAX_PROCESSORS - BITS_PER_LONG);
}

void uvm_parent_gpus_from_processor_mask(uvm_parent_processor_mask_t *parent_mask,
                                         const uvm_processor_mask_t *mask)
{
    uvm_gpu_id_t gpu_id;

    uvm_parent_processor_mask_zero(parent_mask);

    for_each_gpu_id_in_mask(gpu_id, mask)
        uvm_parent_processor_mask_set(parent_mask, uvm_parent_gpu_id_from_gpu_id(gpu_id));
}

bool uvm_numa_id_eq(int nid0, int nid1)
{
    UVM_ASSERT(nid0 >= NUMA_NO_NODE && nid0 < MAX_NUMNODES);
    UVM_ASSERT(nid1 >= NUMA_NO_NODE && nid1 < MAX_NUMNODES);

    return nid0 == nid1;
}
