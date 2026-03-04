/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "os-interface.h"
#include "nv.h"
#include "nv-linux.h"
#include "nv-reg.h"

extern NvU32 NVreg_EnableSystemMemoryPools;

static inline void nv_set_contig_memory_uc(nvidia_pte_t *page_ptr, NvU32 num_pages)
{
#if defined(NV_SET_MEMORY_UC_PRESENT)
    struct page *page = NV_GET_PAGE_STRUCT(page_ptr->phys_addr);
    unsigned long addr = (unsigned long)page_address(page);
    set_memory_uc(addr, num_pages);
#elif defined(NV_SET_PAGES_UC_PRESENT)
    struct page *page = NV_GET_PAGE_STRUCT(page_ptr->phys_addr);
    set_pages_uc(page, num_pages);
#endif
}

static inline void nv_set_contig_memory_wb(nvidia_pte_t *page_ptr, NvU32 num_pages)
{
#if defined(NV_SET_MEMORY_UC_PRESENT)
    struct page *page = NV_GET_PAGE_STRUCT(page_ptr->phys_addr);
    unsigned long addr = (unsigned long)page_address(page);
    set_memory_wb(addr, num_pages);
#elif defined(NV_SET_PAGES_UC_PRESENT)
    struct page *page = NV_GET_PAGE_STRUCT(page_ptr->phys_addr);
    set_pages_wb(page, num_pages);
#endif
}

static inline int nv_set_memory_array_type_present(NvU32 type)
{
    switch (type)
    {
#if defined(NV_SET_MEMORY_ARRAY_UC_PRESENT)
        case NV_MEMORY_UNCACHED:
            return 1;
        case NV_MEMORY_WRITEBACK:
            return 1;
#endif
        default:
            return 0;
    }
}

static inline int nv_set_pages_array_type_present(NvU32 type)
{
    switch (type)
    {
#if defined(NV_SET_PAGES_ARRAY_UC_PRESENT)
        case NV_MEMORY_UNCACHED:
            return 1;
        case NV_MEMORY_WRITEBACK:
            return 1;
#endif
        default:
            return 0;
    }
}

static inline void nv_set_memory_array_type(
    unsigned long *pages,
    NvU32 num_pages,
    NvU32 type
)
{
    switch (type)
    {
#if defined(NV_SET_MEMORY_ARRAY_UC_PRESENT)
        case NV_MEMORY_UNCACHED:
            set_memory_array_uc(pages, num_pages);
            break;
        case NV_MEMORY_WRITEBACK:
            set_memory_array_wb(pages, num_pages);
            break;
#endif
        default:
            nv_printf(NV_DBG_ERRORS,
                "NVRM: %s(): type %d unimplemented\n",
                __FUNCTION__, type);
            break;
    }
}

static inline void nv_set_pages_array_type(
    struct page **pages,
    NvU32 num_pages,
    NvU32 type
)
{
    switch (type)
    {
#if defined(NV_SET_PAGES_ARRAY_UC_PRESENT)
        case NV_MEMORY_UNCACHED:
            set_pages_array_uc(pages, num_pages);
            break;
        case NV_MEMORY_WRITEBACK:
            set_pages_array_wb(pages, num_pages);
            break;
#endif
        default:
            nv_printf(NV_DBG_ERRORS,
                "NVRM: %s(): type %d unimplemented\n",
                __FUNCTION__, type);
            break;
    }
}

static inline void nv_set_contig_memory_type(
    nvidia_pte_t *page_ptr,
    NvU32 num_pages,
    NvU32 type
)
{
    switch (type)
    {
        case NV_MEMORY_UNCACHED:
            nv_set_contig_memory_uc(page_ptr, num_pages);
            break;
        case NV_MEMORY_WRITEBACK:
            nv_set_contig_memory_wb(page_ptr, num_pages);
            break;
        default:
            nv_printf(NV_DBG_ERRORS,
                "NVRM: %s(): type %d unimplemented\n",
                __FUNCTION__, type);
    }
}

static inline void nv_set_memory_type(nv_alloc_t *at, NvU32 type)
{
    NvU32 i;
    NV_STATUS status = NV_OK;
#if defined(NV_SET_MEMORY_ARRAY_UC_PRESENT)
    unsigned long *pages = NULL;
#elif defined(NV_SET_PAGES_ARRAY_UC_PRESENT)
    struct page **pages = NULL;
#else
    unsigned long *pages = NULL;
#endif

    nvidia_pte_t *page_ptr;
    struct page *page;

    if (at->flags.contig)
    {
        nv_set_contig_memory_type(&at->page_table[0], at->num_pages, type);
        return;
    }

    if (nv_set_memory_array_type_present(type))
    {
        status = os_alloc_mem((void **)&pages,
                at->num_pages * sizeof(unsigned long));

    }
    else if (nv_set_pages_array_type_present(type))
    {
        status = os_alloc_mem((void **)&pages,
                at->num_pages * sizeof(struct page*));
    }

    if (status != NV_OK)
        pages = NULL;

    //
    // If the set_{memory,page}_array_* functions are in the kernel interface,
    // it's faster to use them since they work on non-contiguous memory,
    // whereas the set_{memory,page}_*  functions do not.
    //
    if (pages)
    {
        for (i = 0; i < at->num_pages; i++)
        {
            page_ptr = &at->page_table[i];
            page = NV_GET_PAGE_STRUCT(page_ptr->phys_addr);
#if defined(NV_SET_MEMORY_ARRAY_UC_PRESENT)
            pages[i] = (unsigned long)page_address(page);
#elif defined(NV_SET_PAGES_ARRAY_UC_PRESENT)
            pages[i] = page;
#endif
        }
#if defined(NV_SET_MEMORY_ARRAY_UC_PRESENT)
        nv_set_memory_array_type(pages, at->num_pages, type);
#elif defined(NV_SET_PAGES_ARRAY_UC_PRESENT)
        nv_set_pages_array_type(pages, at->num_pages, type);
#endif
        os_free_mem(pages);
    }

    //
    // If the set_{memory,page}_array_* functions aren't present in the kernel
    // interface, each page has to be set individually, which has been measured
    // to be ~10x slower than using the set_{memory,page}_array_* functions.
    //
    else
    {
        for (i = 0; i < at->num_pages; i++)
            nv_set_contig_memory_type(&at->page_table[i], 1, type);
    }
}

static NvU64 nv_get_max_sysmem_address(void)
{
    NvU64 global_max_pfn = 0ULL;
    int node_id;

    for_each_online_node(node_id)
    {
        global_max_pfn = max(global_max_pfn, (NvU64)node_end_pfn(node_id));
    }

    return ((global_max_pfn + 1) << PAGE_SHIFT) - 1;
}

static unsigned int nv_compute_gfp_mask(
    nv_state_t *nv,
    nv_alloc_t *at
)
{
    unsigned int gfp_mask = NV_GFP_KERNEL;
    struct device *dev = at->dev;

    /*
     * If we know that SWIOTLB is enabled (and therefore we avoid calling the
     * kernel to DMA-remap the pages), or if we are using dma_direct (which may
     * transparently use the SWIOTLB for pages that are unaddressable by the
     * device, in kernel versions 5.0 and later), limit our allocation pool
     * to the first 4GB to avoid allocating pages outside of our device's
     * addressable limit.
     * Also, limit the allocation to the first 4GB if explicitly requested by
     * setting the "nv->force_dma32_alloc" variable.
     */
    if (!nv || !nv_requires_dma_remap(nv) || nv_is_dma_direct(dev) || nv->force_dma32_alloc)
    {
        NvU64 max_sysmem_address = nv_get_max_sysmem_address();
        if ((dev && dev->dma_mask && (*(dev->dma_mask) < max_sysmem_address)) ||
            (nv && nv->force_dma32_alloc))
        {
            gfp_mask = NV_GFP_KERNEL | NV_GFP_DMA32;
        }
    }

    gfp_mask |= __GFP_RETRY_MAYFAIL;

    if (at->flags.zeroed)
        gfp_mask |= __GFP_ZERO;

    if (at->flags.node)
        gfp_mask |= __GFP_THISNODE;

    // Compound pages are required by vm_insert_page for high-order page
    // allocations
    if (at->order > 0)
        gfp_mask |= __GFP_COMP;

    return gfp_mask;
}

// set subpages describing page
static void
nv_alloc_set_page
(
    nv_alloc_t *at,
    unsigned int page_idx,
    unsigned long virt_addr
)
{
    unsigned long phys_addr = nv_get_kern_phys_address(virt_addr);
    unsigned int os_pages_in_page = 1 << at->order;
    unsigned int base_os_page = page_idx * os_pages_in_page;
    unsigned int num_os_pages = NV_MIN(at->num_pages - base_os_page, os_pages_in_page);
    unsigned int i;

    for (i = 0; i < num_os_pages; i++)
    {
        at->page_table[base_os_page + i].virt_addr = virt_addr + i * PAGE_SIZE;
        at->page_table[base_os_page + i].phys_addr = phys_addr + i * PAGE_SIZE;
    }
}

/*
 * This function is needed for allocating contiguous physical memory in xen
 * dom0. Because of the use of xen sw iotlb in xen dom0, memory allocated by
 * NV_GET_FREE_PAGES may not be machine contiguous when size is more than
 * 1 page. nv_alloc_coherent_pages() will give us machine contiguous memory.
 * Even though we get dma_address directly in this function, we will
 * still call pci_map_page() later to get dma address. This is fine as it
 * will return the same machine address.
 */
static NV_STATUS nv_alloc_coherent_pages(
    nv_state_t *nv,
    nv_alloc_t *at
)
{
    nvidia_pte_t *page_ptr;
    NvU32 i;
    unsigned int gfp_mask;
    unsigned long virt_addr = 0;
    nv_linux_state_t *nvl;
    struct device *dev;

    if (!nv)
    {
        nv_printf(NV_DBG_MEMINFO,
            "NVRM: VM: %s: coherent page alloc on nvidiactl not supported\n", __FUNCTION__);
        return NV_ERR_NOT_SUPPORTED;
    }

    nvl = NV_GET_NVL_FROM_NV_STATE(nv);
    dev = nvl->dev;

    gfp_mask = nv_compute_gfp_mask(nv, at);

    virt_addr = (unsigned long)dma_alloc_coherent(dev,
                                                  at->num_pages * PAGE_SIZE,
                                                  &at->dma_handle,
                                                  gfp_mask);
    if (!virt_addr)
    {
        nv_printf(NV_DBG_MEMINFO,
            "NVRM: VM: %s: failed to allocate memory\n", __FUNCTION__);
        return NV_ERR_NO_MEMORY;
    }

    for (i = 0; i < at->num_pages; i++)
    {
        page_ptr = &at->page_table[i];

        page_ptr->virt_addr = virt_addr + i * PAGE_SIZE;
        page_ptr->phys_addr = virt_to_phys((void *)page_ptr->virt_addr);
    }

    if (at->cache_type != NV_MEMORY_CACHED)
    {
        nv_set_memory_type(at, NV_MEMORY_UNCACHED);
    }

    at->flags.coherent = NV_TRUE;
    return NV_OK;
}

static void nv_free_coherent_pages(
    nv_alloc_t *at
)
{
    nvidia_pte_t *page_ptr;
    struct device *dev = at->dev;

    page_ptr = &at->page_table[0];

    if (at->cache_type != NV_MEMORY_CACHED)
    {
        nv_set_memory_type(at, NV_MEMORY_WRITEBACK);
    }

    dma_free_coherent(dev, at->num_pages * PAGE_SIZE,
                      (void *)page_ptr->virt_addr, at->dma_handle);
}

typedef struct
{
    unsigned long virt_addr;
    struct list_head list_node;
} nv_page_pool_entry_t;

#define NV_MEM_POOL_LIST_HEAD(list) list_first_entry_or_null(list, nv_page_pool_entry_t, list_node)

typedef struct nv_page_pool_t
{
    struct list_head clean_list;
    struct list_head dirty_list;
    nv_kthread_q_t scrubber_queue;
    nv_kthread_q_item_t scrubber_queue_item;
    int node_id;
    unsigned int order;
    unsigned long pages_owned;
    void *lock;
    struct shrinker *shrinker;

#ifndef NV_SHRINKER_ALLOC_PRESENT
    struct shrinker _shrinker;
#endif
} nv_page_pool_t;

nv_page_pool_t *sysmem_page_pools[MAX_NUMNODES][NV_MAX_PAGE_ORDER + 1];

#ifdef NV_SHRINKER_ALLOC_PRESENT
static nv_page_pool_t *nv_mem_pool_get_from_shrinker(struct shrinker *shrinker)
{
    return shrinker->private_data;
}

static void nv_mem_pool_shrinker_free(nv_page_pool_t *mem_pool)
{
    if (mem_pool->shrinker != NULL)
    {
        shrinker_free(mem_pool->shrinker);
    }
}

static struct shrinker *nv_mem_pool_shrinker_alloc(nv_page_pool_t *mem_pool)
{
    return shrinker_alloc(SHRINKER_NUMA_AWARE, "nv-sysmem-alloc-node-%d-order-%u", mem_pool->node_id, mem_pool->order);
}

static void nv_mem_pool_shrinker_register(nv_page_pool_t *mem_pool, struct shrinker *shrinker)
{
     shrinker->private_data = mem_pool;
     shrinker_register(shrinker);
}
#else

static nv_page_pool_t *nv_mem_pool_get_from_shrinker(struct shrinker *shrinker)
{
    return container_of(shrinker, nv_page_pool_t, _shrinker);
}

static void nv_mem_pool_shrinker_free(nv_page_pool_t *mem_pool)
{
    if (mem_pool->shrinker != NULL)
    {
        unregister_shrinker(mem_pool->shrinker);
    }
}

static struct shrinker *nv_mem_pool_shrinker_alloc(nv_page_pool_t *mem_pool)
{
    return &mem_pool->_shrinker;
}

static void nv_mem_pool_shrinker_register(nv_page_pool_t *mem_pool, struct shrinker *shrinker)
{
    shrinker->flags |= SHRINKER_NUMA_AWARE;
    register_shrinker(shrinker
#ifdef NV_REGISTER_SHRINKER_HAS_FMT_ARG
        , "nv-sysmem-alloc-node-%d-order-%u", mem_pool->node_id, mem_pool->order
#endif // NV_REGISTER_SHRINKER_HAS_FMT_ARG
    );
}
#endif // NV_SHRINKER_ALLOC_PRESENT

static unsigned long
nv_mem_pool_move_pages
(
    struct list_head *dst_list,
    struct list_head *src_list,
    unsigned long max_entries_to_move
)
{
    while (max_entries_to_move > 0)
    {
        nv_page_pool_entry_t *pool_entry = NV_MEM_POOL_LIST_HEAD(src_list);
        if (pool_entry == NULL)
            break;

        list_del(&pool_entry->list_node);
        list_add(&pool_entry->list_node, dst_list);
        max_entries_to_move--;
    }

    return max_entries_to_move;
}

static void
nv_mem_pool_free_page_list
(
    struct list_head *free_list,
    unsigned int order
)
{
    while (!list_empty(free_list))
    {
        nv_page_pool_entry_t *pool_entry = NV_MEM_POOL_LIST_HEAD(free_list);
        list_del(&pool_entry->list_node);
        NV_FREE_PAGES(pool_entry->virt_addr, order)
        NV_KFREE(pool_entry, sizeof(*pool_entry));
    }
}

static unsigned long
nv_mem_pool_shrinker_count
(
    struct shrinker *shrinker,
    struct shrink_control *sc
)
{
    nv_page_pool_t *mem_pool = nv_mem_pool_get_from_shrinker(shrinker);
    unsigned long pages_owned;

    if (sc->nid != mem_pool->node_id)
    {
        pages_owned = 0;
        goto done;
    }

    if (os_acquire_mutex(mem_pool->lock) != NV_OK)
        return 0;
    // Page that is being scrubbed by worker is also counted
    pages_owned = mem_pool->pages_owned;
    os_release_mutex(mem_pool->lock);

    nv_printf(NV_DBG_MEMINFO, "NVRM: VM: %s: node=%d order=%u: %lu pages in pool\n",
              __FUNCTION__, mem_pool->node_id, mem_pool->order, pages_owned);

done:
#ifdef SHRINK_EMPTY
    return (pages_owned == 0) ? SHRINK_EMPTY : pages_owned;
#else
    return pages_owned;
#endif
}

static unsigned long
nv_mem_pool_shrinker_scan
(
    struct shrinker *shrinker,
    struct shrink_control *sc
)
{
    nv_page_pool_t *mem_pool = nv_mem_pool_get_from_shrinker(shrinker);
    unsigned long pages_remaining;
    unsigned long pages_freed;
    struct list_head reclaim_list;

    if (sc->nid != mem_pool->node_id)
        return SHRINK_STOP;

    INIT_LIST_HEAD(&reclaim_list);

    if (os_acquire_mutex(mem_pool->lock) != NV_OK)
        return SHRINK_STOP;
    pages_remaining = sc->nr_to_scan;
    pages_remaining = nv_mem_pool_move_pages(&reclaim_list, &mem_pool->dirty_list, pages_remaining);
    pages_remaining = nv_mem_pool_move_pages(&reclaim_list, &mem_pool->clean_list, pages_remaining);
    pages_freed = sc->nr_to_scan - pages_remaining;
    mem_pool->pages_owned -= pages_freed;
    os_release_mutex(mem_pool->lock);

    nv_mem_pool_free_page_list(&reclaim_list, mem_pool->order);

    nv_printf(NV_DBG_MEMINFO, "NVRM: VM: %s: node=%d order=%u: %lu/%lu pages freed\n",
              __FUNCTION__, mem_pool->node_id, mem_pool->order, pages_freed, sc->nr_to_scan);

    return (pages_freed == 0) ? SHRINK_STOP : pages_freed;
}

static void
nv_mem_pool_clear_page(unsigned long virt_addr, unsigned int order)
{
    unsigned int os_pages_in_page = 1 << order;
    unsigned int i;

    for (i = 0; i < os_pages_in_page; i++)
    {
        clear_page((void *)(virt_addr + i * PAGE_SIZE));
    }
}

unsigned int
nv_mem_pool_alloc_pages
(
    nv_page_pool_t *mem_pool,
    nv_alloc_t *at
)
{
    unsigned int os_pages_in_page = 1 << at->order;
    unsigned int max_num_pages = NV_CEIL(at->num_pages, os_pages_in_page);
    nv_page_pool_entry_t *pool_entry;
    unsigned int pages_remaining = max_num_pages;
    unsigned int pages_allocated;
    unsigned int pages_allocated_clean;
    unsigned long pages_owned;
    unsigned int i = 0;
    struct list_head alloc_clean_pages;
    struct list_head alloc_dirty_pages;
    NV_STATUS status;

    if (!NV_MAY_SLEEP())
    {
        // can't wait for the mutex
        return 0;
    }

    INIT_LIST_HEAD(&alloc_clean_pages);
    INIT_LIST_HEAD(&alloc_dirty_pages);

    status = os_acquire_mutex(mem_pool->lock);
    WARN_ON(status != NV_OK);
    pages_remaining = nv_mem_pool_move_pages(&alloc_clean_pages, &mem_pool->clean_list, pages_remaining);
    pages_allocated_clean = (max_num_pages - pages_remaining);
    pages_remaining = nv_mem_pool_move_pages(&alloc_dirty_pages, &mem_pool->dirty_list, pages_remaining);
    pages_allocated = (max_num_pages - pages_remaining);
    mem_pool->pages_owned -= pages_allocated;
    pages_owned = mem_pool->pages_owned;
    os_release_mutex(mem_pool->lock);

    while ((pool_entry = NV_MEM_POOL_LIST_HEAD(&alloc_clean_pages)))
    {
        nv_alloc_set_page(at, i, pool_entry->virt_addr);
        list_del(&pool_entry->list_node);
        NV_KFREE(pool_entry, sizeof(*pool_entry));
        i++;
    }

    while ((pool_entry = NV_MEM_POOL_LIST_HEAD(&alloc_dirty_pages)))
    {
        nv_mem_pool_clear_page(pool_entry->virt_addr, mem_pool->order);

        nv_alloc_set_page(at, i, pool_entry->virt_addr);
        list_del(&pool_entry->list_node);
        NV_KFREE(pool_entry, sizeof(*pool_entry));
        i++;
    }

    if (i != pages_allocated)
    {
        os_dbg_breakpoint();
    }

    nv_printf(NV_DBG_MEMINFO,
              "NVRM: VM: %s: node=%d order=%u: %lu/%lu pages allocated (%lu already cleared, %lu left in pool)\n",
              __FUNCTION__, mem_pool->node_id, mem_pool->order, pages_allocated, max_num_pages, pages_allocated_clean,
              pages_owned);

    return pages_allocated;
}

static void
nv_mem_pool_queue_worker(nv_page_pool_t *mem_pool)
{
    nv_kthread_q_schedule_q_item(&mem_pool->scrubber_queue,
                                 &mem_pool->scrubber_queue_item);
}

static void
nv_mem_pool_worker(void *arg)
{
    nv_page_pool_t *mem_pool = arg;
    nv_page_pool_entry_t *pool_entry = NULL;
    NV_STATUS status;

    for (;;)
    {
        status = os_acquire_mutex(mem_pool->lock);
        WARN_ON(status != NV_OK);
        if (pool_entry != NULL)
        {
            // add the entry from the last pass, avoid getting the lock again
            list_add(&pool_entry->list_node, &mem_pool->clean_list);
        }

        pool_entry = NV_MEM_POOL_LIST_HEAD(&mem_pool->dirty_list);
        if (pool_entry == NULL)
        {
            os_release_mutex(mem_pool->lock);
            break;
        }
        list_del(&pool_entry->list_node);
        os_release_mutex(mem_pool->lock);

        nv_mem_pool_clear_page(pool_entry->virt_addr, mem_pool->order);
    }
}

void
nv_mem_pool_destroy(nv_page_pool_t *mem_pool)
{
    NV_STATUS status;

    status = os_acquire_mutex(mem_pool->lock);
    WARN_ON(status != NV_OK);
    nv_mem_pool_free_page_list(&mem_pool->dirty_list, mem_pool->order);
    os_release_mutex(mem_pool->lock);

    // All pages are freed, so scrubber won't attempt to requeue
    nv_kthread_q_stop(&mem_pool->scrubber_queue);

    status = os_acquire_mutex(mem_pool->lock);
    WARN_ON(status != NV_OK);
    // free clean pages after scrubber can't add any new
    nv_mem_pool_free_page_list(&mem_pool->clean_list, mem_pool->order);
    os_release_mutex(mem_pool->lock);

    nv_mem_pool_shrinker_free(mem_pool);

    os_free_mutex(mem_pool->lock);

    NV_KFREE(mem_pool, sizeof(*mem_pool));
}

nv_page_pool_t* nv_mem_pool_init(int node_id, unsigned int order)
{
    struct shrinker *shrinker;
    nv_page_pool_t *mem_pool;

    NV_KZALLOC(mem_pool, sizeof(*mem_pool));
    if (mem_pool == NULL)
    {
        nv_printf(NV_DBG_SETUP, "NVRM: %s: failed allocating memory\n", __FUNCTION__);
        return NULL;
    }

    mem_pool->node_id = node_id;
    mem_pool->order = order;

    INIT_LIST_HEAD(&mem_pool->clean_list);
    INIT_LIST_HEAD(&mem_pool->dirty_list);

    if (os_alloc_mutex(&mem_pool->lock))
    {
        nv_printf(NV_DBG_SETUP, "NVRM: %s: failed allocating mutex for worker thread\n", __FUNCTION__);
        goto failed;
    }

    if (nv_kthread_q_init_on_node(&mem_pool->scrubber_queue, "nv_mem_pool_scrubber_queue", node_id))
    {
        nv_printf(NV_DBG_SETUP, "NVRM: %s: failed allocating worker thread\n", __FUNCTION__);
        goto failed;
    }
    nv_kthread_q_item_init(&mem_pool->scrubber_queue_item, nv_mem_pool_worker, mem_pool);

    shrinker = nv_mem_pool_shrinker_alloc(mem_pool);

    if (shrinker == NULL)
    {
        nv_printf(NV_DBG_SETUP, "NVRM: %s: failed allocating shrinker\n", __FUNCTION__);
        goto failed;
    }

     shrinker->count_objects = nv_mem_pool_shrinker_count;
     shrinker->scan_objects = nv_mem_pool_shrinker_scan;
     shrinker->seeks = 1;

     nv_mem_pool_shrinker_register(mem_pool, shrinker);

     mem_pool->shrinker = shrinker;
     return mem_pool;

failed:
    nv_mem_pool_destroy(mem_pool);
    return NULL;
}

NV_STATUS
nv_mem_pool_free_pages
(
    nv_page_pool_t *mem_pool,
    nv_alloc_t *at
)
{
    unsigned int os_pages_in_page = 1 << at->order;
    unsigned int num_pages = NV_CEIL(at->num_pages, os_pages_in_page);
    NvBool queue_worker;
    nv_page_pool_entry_t *pool_entry;
    struct list_head freed_pages;
    unsigned int num_added_pages = 0;
    unsigned long pages_owned;
    unsigned int i;
    NV_STATUS status;

    if (!NV_MAY_SLEEP())
    {
        // can't wait for the mutex
        return NV_ERR_INVALID_ARGUMENT;
    }

    INIT_LIST_HEAD(&freed_pages);

    for (i = 0; i < num_pages; i++)
    {
        nvidia_pte_t *page_ptr = &at->page_table[i * os_pages_in_page];

        if (page_ptr->virt_addr == 0)
        {
            // alloc failed
            break;
        }

        if (page_to_nid(NV_GET_PAGE_STRUCT(page_ptr->phys_addr)) != mem_pool->node_id)
        {
            // Only accept pages from the right node
            NV_FREE_PAGES(page_ptr->virt_addr, mem_pool->order);
            continue;
        }

        NV_KZALLOC(pool_entry, sizeof(*pool_entry));
        if (pool_entry == NULL)
        {
            NV_FREE_PAGES(page_ptr->virt_addr, mem_pool->order);
            continue;
        }

        pool_entry->virt_addr = page_ptr->virt_addr;
        list_add(&pool_entry->list_node, &freed_pages);
        num_added_pages++;
    }

    if (num_added_pages == 0)
        return NV_OK;

    status = os_acquire_mutex(mem_pool->lock);
    WARN_ON(status != NV_OK);
    // Worker is already queued if list is not empty
    queue_worker = list_empty(&mem_pool->dirty_list);
    list_splice_init(&freed_pages, &mem_pool->dirty_list);
    mem_pool->pages_owned += num_added_pages;
    pages_owned = mem_pool->pages_owned;
    os_release_mutex(mem_pool->lock);

    nv_printf(NV_DBG_MEMINFO, "NVRM: VM: %s: node=%d order=%u: %lu/%lu pages added to pool (%lu now in pool)\n",
              __FUNCTION__, mem_pool->node_id, mem_pool->order, num_added_pages, num_pages, pages_owned);

    if (queue_worker)
    {
        nv_mem_pool_queue_worker(mem_pool);
    }

    return NV_OK;
}

NV_STATUS nv_init_page_pools(void)
{
    int node_id;
    unsigned int order;

    for_each_node(node_id)
    {
        for (order = 0; order <= NV_MAX_PAGE_ORDER; order++)
        {
            unsigned long page_size = PAGE_SIZE << order;

            if (!(NVreg_EnableSystemMemoryPools & (page_size >> NV_ENABLE_SYSTEM_MEMORY_POOLS_SHIFT)))
                continue;

            sysmem_page_pools[node_id][order] = nv_mem_pool_init(node_id, order);

            if (sysmem_page_pools[node_id][order] == NULL)
            {
                return NV_ERR_NO_MEMORY;
            }
        }
    }

    return NV_OK;
}

void nv_destroy_page_pools(void)
{
    int node_id;
    unsigned int order;

    for_each_node(node_id)
    {
        for (order = 0; order <= NV_MAX_PAGE_ORDER; order++)
        {
            if (sysmem_page_pools[node_id][order])
                nv_mem_pool_destroy(sysmem_page_pools[node_id][order]);
        }
    }
}

static nv_page_pool_t *nv_mem_pool_get(int node_id, unsigned int order)
{

    if (node_id >= ARRAY_SIZE(sysmem_page_pools))
        return NULL;

    // get_order() is not limited by NV_MAX_PAGE_ORDER
    if (order >= ARRAY_SIZE(sysmem_page_pools[node_id]))
        return NULL;

    return sysmem_page_pools[node_id][order];
}

void
nv_free_system_pages
(
    nv_alloc_t *at
)
{
    nv_page_pool_t *page_pool = NULL;
    unsigned int os_pages_in_page = 1 << at->order;
    unsigned int num_pages = NV_CEIL(at->num_pages, os_pages_in_page);
    unsigned int i;

    if (at->page_table[0].virt_addr != 0)
    {
        // if low on memory, pages could be allocated from different nodes
        int likely_node_id = page_to_nid(NV_GET_PAGE_STRUCT(at->page_table[0].phys_addr));

        page_pool = nv_mem_pool_get(likely_node_id, at->order);
    }

    if (at->cache_type != NV_MEMORY_CACHED)
    {
        nv_set_memory_type(at, NV_MEMORY_WRITEBACK);
    }

    for (i = 0; i < num_pages; i++)
    {
        nvidia_pte_t *page_ptr = &at->page_table[i * os_pages_in_page];

        if (page_ptr->virt_addr == 0)
        {
            // alloc failed
            break;
        }

        // For unprotected sysmem in CC, memory is marked as unencrypted during allocation.
        // NV_FREE_PAGES only deals with protected sysmem. Mark memory as encrypted and protected before free.
        nv_set_memory_encrypted(at->flags.unencrypted, page_ptr->virt_addr, 1 << at->order);
    }

    if (!at->flags.pool || page_pool == NULL ||
        nv_mem_pool_free_pages(page_pool, at) != NV_OK)
    {
        // nv_mem_pool_free_pages() fails if !NV_MAY_SLEEP()
        for (i = 0; i < num_pages; i++)
        {
            unsigned int base_os_page = i * os_pages_in_page;
            nvidia_pte_t *page_ptr = &at->page_table[base_os_page];

            if (page_ptr->virt_addr == 0)
            {
                // alloc failed
                break;
            }

            NV_FREE_PAGES(page_ptr->virt_addr, at->order);
        }
    }

    nv_printf(NV_DBG_MEMINFO,
            "NVRM: VM: %s: %u/%u order0 pages\n", __FUNCTION__, i * os_pages_in_page, at->num_pages);
}

NV_STATUS
nv_alloc_system_pages
(
    nv_state_t *nv,
    nv_alloc_t *at
)
{
    unsigned int gfp_mask = nv_compute_gfp_mask(nv, at);
    unsigned int i;
    unsigned int num_pool_allocated_pages = 0;
    unsigned int os_pages_in_page = 1 << at->order;
    unsigned int num_pages = NV_CEIL(at->num_pages, os_pages_in_page);
    // OS allocator tries CPU node first by default, mirror that
    int preferred_node_id = at->flags.node ? at->node_id : numa_mem_id();
    nv_page_pool_t *page_pool = nv_mem_pool_get(preferred_node_id, at->order);

    // Remember if pool allocation was attempted and use it on free to avoid hoarding memory
    // Avoid unwanted scrubbing, especially important for onlined FB
    // Cross-node cache invalidation at remap can be dramatically slower if memory is not cached locally
    at->flags.pool = !(gfp_mask & NV_GFP_DMA32) &&
                     at->flags.zeroed &&
                     (at->cache_type == NV_MEMORY_CACHED || num_online_nodes() <= 1);

    nv_printf(NV_DBG_MEMINFO,
            "NVRM: VM: %s: %u order0 pages, %u order\n", __FUNCTION__, at->num_pages, at->order);

    if (page_pool != NULL && at->flags.pool)
    {
        num_pool_allocated_pages = nv_mem_pool_alloc_pages(page_pool, at);
    }

    for (i = num_pool_allocated_pages; i < num_pages; i++)
    {
        unsigned long virt_addr = 0;

        if (at->flags.node)
        {
            unsigned long ptr = 0ULL;
            NV_ALLOC_PAGES_NODE(ptr, at->node_id, at->order, gfp_mask);
            if (ptr != 0)
            {
               virt_addr = (unsigned long) page_address((void *)ptr);
            }
        }
        else
        {
            NV_GET_FREE_PAGES(virt_addr, at->order, gfp_mask);
        }

        if (virt_addr == 0)
        {
            goto failed;
        }

        nv_alloc_set_page(at, i, virt_addr);
    }

    for (i = 0; i < num_pages; i++)
    {
        unsigned int base_os_page = i * os_pages_in_page;
        nvidia_pte_t *page_ptr = &at->page_table[base_os_page];
        unsigned int num_os_pages = NV_MIN(at->num_pages - base_os_page, os_pages_in_page);

        // In CC, NV_GET_FREE_PAGES only allocates protected sysmem.
        // To get unprotected sysmem, this memory is marked as unencrypted.
        nv_set_memory_decrypted_zeroed(at->flags.unencrypted, page_ptr->virt_addr, 1 << at->order,
                                       num_os_pages * PAGE_SIZE);
    }

    if (at->cache_type != NV_MEMORY_CACHED)
    {
        nv_set_memory_type(at, NV_MEMORY_UNCACHED);
    }

    return NV_OK;

failed:
    nv_printf(NV_DBG_MEMINFO,
        "NVRM: VM: %s: failed to allocate memory\n", __FUNCTION__);
    nv_free_system_pages(at);
    return NV_ERR_NO_MEMORY;
}

NV_STATUS nv_alloc_contig_pages(
    nv_state_t *nv,
    nv_alloc_t *at
)
{
    NV_STATUS status;

    nv_printf(NV_DBG_MEMINFO,
            "NVRM: VM: %s: %u pages\n", __FUNCTION__, at->num_pages);

    if (os_is_xen_dom0())
        return nv_alloc_coherent_pages(nv, at);

    at->order = get_order(at->num_pages * PAGE_SIZE);

    status = nv_alloc_system_pages(nv, at);
    if (status != NV_OK)
    {
        if (os_is_vgx_hyper())
        {
            nv_printf(NV_DBG_MEMINFO,
                "NVRM: VM: %s: failed to allocate memory, trying coherent memory \n", __FUNCTION__);

            status = nv_alloc_coherent_pages(nv, at);
            return status;
        }

        nv_printf(NV_DBG_MEMINFO,
            "NVRM: VM: %s: failed to allocate memory\n", __FUNCTION__);
        return NV_ERR_NO_MEMORY;
    }

    return NV_OK;
}

void nv_free_contig_pages(
    nv_alloc_t *at
)
{
    nv_printf(NV_DBG_MEMINFO,
            "NVRM: VM: %s: %u pages\n", __FUNCTION__, at->num_pages);

    if (at->flags.coherent)
        return nv_free_coherent_pages(at);

    nv_free_system_pages(at);
}

static NvUPtr nv_vmap(struct page **pages, NvU32 page_count,
                      NvBool cached, NvBool unencrypted)
{
    void *ptr;
    pgprot_t prot = PAGE_KERNEL;
#if defined(NVCPU_X86_64)
    if (unencrypted)
    {
        prot = cached ? nv_adjust_pgprot(PAGE_KERNEL_NOENC) :
                        nv_adjust_pgprot(NV_PAGE_KERNEL_NOCACHE_NOENC);
    }
    else
    {
        prot = cached ? PAGE_KERNEL : PAGE_KERNEL_NOCACHE;
    }
#elif defined(NVCPU_AARCH64)
    prot = cached ? PAGE_KERNEL : NV_PGPROT_UNCACHED(PAGE_KERNEL);
#endif
    ptr = vmap(pages, page_count, VM_MAP, prot);
    NV_MEMDBG_ADD(ptr, page_count * PAGE_SIZE);

    return (NvUPtr)ptr;
}

static void nv_vunmap(NvUPtr vaddr, NvU32 page_count)
{
    vunmap((void *)vaddr);
    NV_MEMDBG_REMOVE((void *)vaddr, page_count * PAGE_SIZE);
}

NvUPtr nv_vm_map_pages(
    struct page **pages,
    NvU32 count,
    NvBool cached,
    NvBool unencrypted
)
{
    NvUPtr virt_addr = 0;

    if (!NV_MAY_SLEEP())
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: %s: can't map %d pages, invalid context!\n",
                  __FUNCTION__, count);
        os_dbg_breakpoint();
        return virt_addr;
    }

    virt_addr = nv_vmap(pages, count, cached, unencrypted);
    return virt_addr;
}

void nv_vm_unmap_pages(
    NvUPtr virt_addr,
    NvU32 count
)
{
    if (!NV_MAY_SLEEP())
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: %s: can't unmap %d pages at 0x%0llx, "
                  "invalid context!\n", __FUNCTION__, count, virt_addr);
        os_dbg_breakpoint();
        return;
    }

    nv_vunmap(virt_addr, count);
}
