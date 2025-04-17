/*
 * SPDX-FileCopyrightText: Copyright (c) 1999-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
            page_ptr = at->page_table[i];
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
            nv_set_contig_memory_type(at->page_table[i], 1, type);
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
            gfp_mask = NV_GFP_DMA32;
        }
    }
#if defined(__GFP_RETRY_MAYFAIL)
    gfp_mask |= __GFP_RETRY_MAYFAIL;
#elif defined(__GFP_NORETRY)
    gfp_mask |= __GFP_NORETRY;
#endif
#if defined(__GFP_ZERO)
    if (at->flags.zeroed)
        gfp_mask |= __GFP_ZERO;
#endif
#if defined(__GFP_THISNODE)
    if (at->flags.node)
        gfp_mask |= __GFP_THISNODE;
#endif
    // Compound pages are required by vm_insert_page for high-order page
    // allocations
    if (at->order > 0)
        gfp_mask |= __GFP_COMP;

    return gfp_mask;
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
        page_ptr = at->page_table[i];

        page_ptr->virt_addr = virt_addr + i * PAGE_SIZE;
        page_ptr->phys_addr = virt_to_phys((void *)page_ptr->virt_addr);
    }

    if (at->cache_type != NV_MEMORY_CACHED)
    {
        nv_set_contig_memory_type(at->page_table[0],
                                  at->num_pages,
                                  NV_MEMORY_UNCACHED);
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

    page_ptr = at->page_table[0];

    if (at->cache_type != NV_MEMORY_CACHED)
    {
        nv_set_contig_memory_type(at->page_table[0],
                                  at->num_pages,
                                  NV_MEMORY_WRITEBACK);
    }

    dma_free_coherent(dev, at->num_pages * PAGE_SIZE,
                      (void *)page_ptr->virt_addr, at->dma_handle);
}

NV_STATUS nv_alloc_contig_pages(
    nv_state_t *nv,
    nv_alloc_t *at
)
{
    NV_STATUS status;
    nvidia_pte_t *page_ptr;
    NvU32 i, j;
    unsigned int gfp_mask;
    unsigned long virt_addr = 0;
    NvU64 phys_addr;

    nv_printf(NV_DBG_MEMINFO,
            "NVRM: VM: %s: %u pages\n", __FUNCTION__, at->num_pages);

    // TODO: This is a temporary WAR, and will be removed after fixing bug 200732409.
    if (os_is_xen_dom0())
        return nv_alloc_coherent_pages(nv, at);

    at->order = get_order(at->num_pages * PAGE_SIZE);
    gfp_mask = nv_compute_gfp_mask(nv, at);

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

        // In CC, NV_GET_FREE_PAGES only allocates protected sysmem.
        // To get unprotected sysmem, this memory is marked as unencrypted.
        nv_set_memory_decrypted_zeroed(at->flags.unencrypted, virt_addr, 1 << at->order,
                                       at->num_pages * PAGE_SIZE);
    }
    if (virt_addr == 0)
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
#if !defined(__GFP_ZERO)
    if (at->flags.zeroed)
        memset((void *)virt_addr, 0, (at->num_pages * PAGE_SIZE));
#endif

    for (i = 0; i < at->num_pages; i++, virt_addr += PAGE_SIZE)
    {
        phys_addr = nv_get_kern_phys_address(virt_addr);
        if (phys_addr == 0)
        {
            nv_printf(NV_DBG_ERRORS,
                "NVRM: VM: %s: failed to look up physical address\n",
                __FUNCTION__);
            status = NV_ERR_OPERATING_SYSTEM;
            goto failed;
        }

        page_ptr = at->page_table[i];
        page_ptr->phys_addr = phys_addr;
        page_ptr->virt_addr = virt_addr;

        NV_MAYBE_RESERVE_PAGE(page_ptr);
    }

    if (at->cache_type != NV_MEMORY_CACHED)
    {
        nv_set_contig_memory_type(at->page_table[0],
                                  at->num_pages,
                                  NV_MEMORY_UNCACHED);
    }

    at->flags.coherent = NV_FALSE;

    return NV_OK;

failed:
    if (i > 0)
    {
        for (j = 0; j < i; j++)
            NV_MAYBE_UNRESERVE_PAGE(at->page_table[j]);
    }

    page_ptr = at->page_table[0];

    // For unprotected sysmem in CC, memory is marked as unencrypted during allocation.
    // NV_FREE_PAGES only deals with protected sysmem. Mark memory as encrypted and protected before free.
    nv_set_memory_encrypted(at->flags.unencrypted, page_ptr->virt_addr, 1 << at->order);

    NV_FREE_PAGES(page_ptr->virt_addr, at->order);

    return status;
}

void nv_free_contig_pages(
    nv_alloc_t *at
)
{
    nvidia_pte_t *page_ptr;
    unsigned int i;

    nv_printf(NV_DBG_MEMINFO,
            "NVRM: VM: %s: %u pages\n", __FUNCTION__, at->num_pages);

    if (at->flags.coherent)
        return nv_free_coherent_pages(at);

    if (at->cache_type != NV_MEMORY_CACHED)
    {
        nv_set_contig_memory_type(at->page_table[0],
                                  at->num_pages,
                                  NV_MEMORY_WRITEBACK);
    }

    for (i = 0; i < at->num_pages; i++)
    {
        page_ptr = at->page_table[i];

        NV_MAYBE_UNRESERVE_PAGE(page_ptr);
    }

    page_ptr = at->page_table[0];

    // For unprotected sysmem in CC, memory is marked as unencrypted during allocation.
    // NV_FREE_PAGES only deals with protected sysmem. Mark memory as encrypted and protected before free.
    nv_set_memory_encrypted(at->flags.unencrypted, page_ptr->virt_addr, 1 << at->order);

    NV_FREE_PAGES(page_ptr->virt_addr, at->order);
}

NV_STATUS nv_alloc_system_pages(
    nv_state_t *nv,
    nv_alloc_t *at
)
{
    NV_STATUS status;
    nvidia_pte_t *page_ptr;
    NvU32 i, j;
    unsigned int gfp_mask;
    unsigned long virt_addr = 0;
    NvU64 phys_addr;

    unsigned int alloc_page_size = PAGE_SIZE << at->order;
    unsigned int alloc_num_pages = NV_CEIL(at->num_pages * PAGE_SIZE, alloc_page_size);

    unsigned int sub_page_idx;
    unsigned int sub_page_offset;
    unsigned int os_pages_in_page = alloc_page_size / PAGE_SIZE;

    nv_printf(NV_DBG_MEMINFO,
            "NVRM: VM: %s: %u order0 pages, %u order\n", __FUNCTION__, at->num_pages, at->order);

    gfp_mask = nv_compute_gfp_mask(nv, at);

    for (i = 0; i < alloc_num_pages; i++)
    {
        if (at->flags.node)
        {
            unsigned long ptr = 0ULL;
            NV_ALLOC_PAGES_NODE(ptr, at->node_id, at->order, gfp_mask);
            if (ptr != 0)
            {
                virt_addr = (unsigned long) page_address((void *)ptr);
            }
            else
            {
                virt_addr = 0;
            }
        }
        else
        {
            NV_GET_FREE_PAGES(virt_addr, at->order, gfp_mask);

            // In CC, NV_GET_FREE_PAGES only allocates protected sysmem.
            // To get unprotected sysmem, this memory is marked as unencrypted.
            nv_set_memory_decrypted_zeroed(at->flags.unencrypted, virt_addr, 1 << at->order,
                                           alloc_page_size);
        }

        if (virt_addr == 0)
        {
            nv_printf(NV_DBG_MEMINFO,
                "NVRM: VM: %s: failed to allocate memory\n", __FUNCTION__);
            status = NV_ERR_NO_MEMORY;
            goto failed;
        }
#if !defined(__GFP_ZERO)
        if (at->flags.zeroed)
            memset((void *)virt_addr, 0, alloc_page_size);
#endif

        sub_page_offset = 0;
        for (sub_page_idx = 0; sub_page_idx < os_pages_in_page; sub_page_idx++)
        {
            unsigned long sub_page_virt_addr = virt_addr + sub_page_offset;
            unsigned int base_page_idx = (i * os_pages_in_page) + sub_page_idx;

            if (base_page_idx >= at->num_pages)
                break;

            phys_addr = nv_get_kern_phys_address(sub_page_virt_addr);
            if (phys_addr == 0)
            {
                nv_printf(NV_DBG_ERRORS,
                    "NVRM: VM: %s: failed to look up physical address\n",
                    __FUNCTION__);
                NV_FREE_PAGES(sub_page_virt_addr, at->order);
                status = NV_ERR_OPERATING_SYSTEM;
                goto failed;
            }

#if defined(_PAGE_NX)
            if (((_PAGE_NX & pgprot_val(PAGE_KERNEL)) != 0) &&
                    (phys_addr < 0x400000))
            {
                nv_printf(NV_DBG_SETUP,
                    "NVRM: VM: %s: discarding page @ 0x%llx\n",
                    __FUNCTION__, phys_addr);
                --i;
                continue;
            }
#endif

            page_ptr = at->page_table[base_page_idx];
            page_ptr->phys_addr = phys_addr;
            page_ptr->virt_addr = sub_page_virt_addr;

            NV_MAYBE_RESERVE_PAGE(page_ptr);
            sub_page_offset += PAGE_SIZE;
        }
    }

    if (at->cache_type != NV_MEMORY_CACHED)
        nv_set_memory_type(at, NV_MEMORY_UNCACHED);

    return NV_OK;

failed:
    if (i > 0)
    {
        for (j = 0; j < i; j++)
        {
            page_ptr = at->page_table[j * os_pages_in_page];
            NV_MAYBE_UNRESERVE_PAGE(page_ptr);

            // For unprotected sysmem in CC, memory is marked as unencrypted during allocation.
            // NV_FREE_PAGES only deals with protected sysmem. Mark memory as encrypted and protected before free.
            nv_set_memory_encrypted(at->flags.unencrypted, page_ptr->virt_addr, 1 << at->order);

            NV_FREE_PAGES(page_ptr->virt_addr, at->order);
        }
    }

    return status;
}

void nv_free_system_pages(
    nv_alloc_t *at
)
{
    nvidia_pte_t *page_ptr;
    unsigned int i;

    unsigned int alloc_page_size = PAGE_SIZE << at->order;
    unsigned int os_pages_in_page = alloc_page_size / PAGE_SIZE;

    nv_printf(NV_DBG_MEMINFO,
            "NVRM: VM: %s: %u pages\n", __FUNCTION__, at->num_pages);

    if (at->cache_type != NV_MEMORY_CACHED)
        nv_set_memory_type(at, NV_MEMORY_WRITEBACK);

    for (i = 0; i < at->num_pages; i++)
    {
        page_ptr = at->page_table[i];

        NV_MAYBE_UNRESERVE_PAGE(page_ptr);
    }

    for (i = 0; i < at->num_pages; i += os_pages_in_page)
    {
        page_ptr = at->page_table[i];

        // For unprotected sysmem in CC, memory is marked as unencrypted during allocation.
        // NV_FREE_PAGES only deals with protected sysmem. Mark memory as encrypted and protected before free.
        nv_set_memory_encrypted(at->flags.unencrypted, page_ptr->virt_addr, 1 << at->order);

        NV_FREE_PAGES(page_ptr->virt_addr, at->order);
    }
}

static NvUPtr nv_vmap(struct page **pages, NvU32 page_count,
                      NvBool cached, NvBool unencrypted)
{
    void *ptr;
    pgprot_t prot = PAGE_KERNEL;
#if defined(NVCPU_X86_64)
#if defined(PAGE_KERNEL_NOENC)
    if (unencrypted)
    {
        prot = cached ? nv_adjust_pgprot(PAGE_KERNEL_NOENC) :
                        nv_adjust_pgprot(NV_PAGE_KERNEL_NOCACHE_NOENC);
    }
    else
#endif
    {
        prot = cached ? PAGE_KERNEL : PAGE_KERNEL_NOCACHE;
    }
#elif defined(NVCPU_AARCH64)
    prot = cached ? PAGE_KERNEL : NV_PGPROT_UNCACHED(PAGE_KERNEL);
#endif
    /* All memory cached in PPC64LE; can't honor 'cached' input. */
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

