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

#define  __NO_VERSION__

#include "os-interface.h"
#include "nv-linux.h"
#include "nv-reg.h"

#define NV_DMA_DEV_PRINTF(debuglevel, dma_dev, format, ... )                \
    nv_printf(debuglevel, "NVRM: %s: " format,                              \
              (((dma_dev) && ((dma_dev)->dev)) ? dev_name((dma_dev)->dev) : \
                                                 NULL),                     \
              ## __VA_ARGS__)

NvU32 nv_dma_remap_peer_mmio = NV_DMA_REMAP_PEER_MMIO_ENABLE;

NV_STATUS   nv_create_dma_map_scatterlist (nv_dma_map_t *dma_map);
void        nv_destroy_dma_map_scatterlist(nv_dma_map_t *dma_map);
NV_STATUS   nv_map_dma_map_scatterlist    (nv_dma_map_t *dma_map);
void        nv_unmap_dma_map_scatterlist  (nv_dma_map_t *dma_map);
static void nv_dma_unmap_contig           (nv_dma_map_t *dma_map);
static void nv_dma_unmap_scatterlist      (nv_dma_map_t *dma_map);

static inline NvBool nv_dma_is_addressable(
    nv_dma_device_t *dma_dev,
    NvU64 start,
    NvU64 size
)
{
    NvU64 limit = start + size - 1;

    return (start >= dma_dev->addressable_range.start) &&
           (limit <= dma_dev->addressable_range.limit) &&
           (limit >= start);
}

static NV_STATUS nv_dma_map_contig(
    nv_dma_device_t *dma_dev,
    nv_dma_map_t *dma_map,
    NvU64 *va
)
{
#if defined(NV_DMA_MAP_PAGE_ATTRS_PRESENT) && defined(NV_DMA_ATTR_SKIP_CPU_SYNC_PRESENT)
    *va = dma_map_page_attrs(dma_map->dev, dma_map->pages[0], 0,
                             dma_map->page_count * PAGE_SIZE,
                             DMA_BIDIRECTIONAL,
                             (dma_map->cache_type == NV_MEMORY_UNCACHED) ?
                              DMA_ATTR_SKIP_CPU_SYNC : 0);
#else
    *va = dma_map_page(dma_map->dev, dma_map->pages[0], 0,
            dma_map->page_count * PAGE_SIZE, DMA_BIDIRECTIONAL);
#endif
    if (dma_mapping_error(dma_map->dev, *va))
    {
        return NV_ERR_OPERATING_SYSTEM;
    }

    dma_map->mapping.contig.dma_addr = *va;

    if (!nv_dma_is_addressable(dma_dev, *va, dma_map->page_count * PAGE_SIZE))
    {
        NV_DMA_DEV_PRINTF(NV_DBG_ERRORS, dma_dev,
                "DMA address not in addressable range of device "
                "(0x%llx-0x%llx, 0x%llx-0x%llx)\n",
                *va, *va + (dma_map->page_count * PAGE_SIZE - 1),
                dma_dev->addressable_range.start,
                dma_dev->addressable_range.limit);
        nv_dma_unmap_contig(dma_map);
        return NV_ERR_INVALID_ADDRESS;
    }

    return NV_OK;
}

static void nv_dma_unmap_contig(nv_dma_map_t *dma_map)
{
#if defined(NV_DMA_MAP_PAGE_ATTRS_PRESENT) && defined(NV_DMA_ATTR_SKIP_CPU_SYNC_PRESENT)
    dma_unmap_page_attrs(dma_map->dev, dma_map->mapping.contig.dma_addr,
                         dma_map->page_count * PAGE_SIZE,
                         DMA_BIDIRECTIONAL,
                         (dma_map->cache_type == NV_MEMORY_UNCACHED) ?
                          DMA_ATTR_SKIP_CPU_SYNC : 0);
#else
    dma_unmap_page(dma_map->dev, dma_map->mapping.contig.dma_addr,
            dma_map->page_count * PAGE_SIZE, DMA_BIDIRECTIONAL);
#endif
}

static void nv_fill_scatterlist
(
    struct scatterlist *sgl,
    struct page **pages,
    unsigned int page_count
)
{
    unsigned int i;
    struct scatterlist *sg;
#if defined(for_each_sg)
    for_each_sg(sgl, sg, page_count, i)
    {
        sg_set_page(sg, pages[i], PAGE_SIZE, 0);
    }
#else
    for (i = 0; i < page_count; i++)
    {
        sg = &(sgl)[i];
        sg->page = pages[i];
        sg->length = PAGE_SIZE;
        sg->offset = 0;
    }
#endif
}

NV_STATUS nv_create_dma_map_scatterlist(nv_dma_map_t *dma_map)
{
    /*
     * We need to split our mapping into at most 4GB - PAGE_SIZE chunks.
     * The Linux kernel stores the length (and offset) of a scatter-gather
     * segment as an unsigned int, so it will overflow if we try to do
     * anything larger.
     */
    NV_STATUS status;
    nv_dma_submap_t *submap;
    NvU32 i;
    NvU64 allocated_size = 0;
    NvU64 num_submaps = dma_map->page_count + NV_DMA_SUBMAP_MAX_PAGES - 1;
    NvU64 total_size = dma_map->page_count << PAGE_SHIFT;

    /*
     * This turns into 64-bit division, which the ARMv7 kernel doesn't provide
     * implicitly. Instead, we need to use the platform's do_div() to perform
     * the division.
     */
    do_div(num_submaps, NV_DMA_SUBMAP_MAX_PAGES);

    WARN_ON(NvU64_HI32(num_submaps) != 0);

    if (dma_map->import_sgt && (num_submaps != 1))
    {
        return -EINVAL;
    }

    dma_map->mapping.discontig.submap_count = NvU64_LO32(num_submaps);

    status = os_alloc_mem((void **)&dma_map->mapping.discontig.submaps,
        sizeof(nv_dma_submap_t) * dma_map->mapping.discontig.submap_count);
    if (status != NV_OK)
    {
        return status;
    }

    os_mem_set((void *)dma_map->mapping.discontig.submaps, 0,
        sizeof(nv_dma_submap_t) * dma_map->mapping.discontig.submap_count);

    /* If we have an imported SGT, just use that directly. */
    if (dma_map->import_sgt)
    {
        dma_map->mapping.discontig.submaps[0].page_count = dma_map->page_count;
        dma_map->mapping.discontig.submaps[0].sgt = *dma_map->import_sgt;
        dma_map->mapping.discontig.submaps[0].imported = NV_TRUE;

        return status;
    }

    NV_FOR_EACH_DMA_SUBMAP(dma_map, submap, i)
    {
        NvU64 submap_size = NV_MIN(NV_DMA_SUBMAP_MAX_PAGES << PAGE_SHIFT,
                                   total_size - allocated_size);

        submap->page_count = (NvU32)(submap_size >> PAGE_SHIFT);

        status = NV_ALLOC_DMA_SUBMAP_SCATTERLIST(dma_map, submap, i);
        if (status != NV_OK)
        {
            submap->page_count = 0;
            break;
        }

#if defined(NV_DOM0_KERNEL_PRESENT)
        {
            NvU64 page_idx = NV_DMA_SUBMAP_IDX_TO_PAGE_IDX(i);
            nv_fill_scatterlist(submap->sgt.sgl,
                &dma_map->pages[page_idx], submap->page_count);
        }
#endif

        allocated_size += submap_size;
    }

    WARN_ON(allocated_size != total_size);

    if (status != NV_OK)
    {
        nv_destroy_dma_map_scatterlist(dma_map);
    }

    return status;
}

NV_STATUS nv_map_dma_map_scatterlist(nv_dma_map_t *dma_map)
{
    NV_STATUS status = NV_OK;
    nv_dma_submap_t *submap;
    NvU64 i;

    NV_FOR_EACH_DMA_SUBMAP(dma_map, submap, i)
    {
        /* Imported SGTs will have already been mapped by the exporter. */
        submap->sg_map_count = submap->imported ?
            submap->sgt.orig_nents :
            dma_map_sg(dma_map->dev,
                       submap->sgt.sgl,
                       submap->sgt.orig_nents,
                       DMA_BIDIRECTIONAL);
        if (submap->sg_map_count == 0)
        {
            status = NV_ERR_OPERATING_SYSTEM;
            break;
        }
    }

    if (status != NV_OK)
    {
        nv_unmap_dma_map_scatterlist(dma_map);
    }

    return status;
}

void nv_unmap_dma_map_scatterlist(nv_dma_map_t *dma_map)
{
    nv_dma_submap_t *submap;
    NvU64 i;

    NV_FOR_EACH_DMA_SUBMAP(dma_map, submap, i)
    {
        if (submap->sg_map_count == 0)
        {
            break;
        }

        if (submap->imported)
        {
            /* Imported SGTs will be unmapped by the exporter. */
            continue;
        }

        dma_unmap_sg(dma_map->dev, submap->sgt.sgl,
                submap->sgt.orig_nents,
                DMA_BIDIRECTIONAL);
    }
}

void nv_destroy_dma_map_scatterlist(nv_dma_map_t *dma_map)
{
    nv_dma_submap_t *submap;
    NvU64 i;

    NV_FOR_EACH_DMA_SUBMAP(dma_map, submap, i)
    {
        if ((submap->page_count == 0) || submap->imported)
        {
            break;
        }

        sg_free_table(&submap->sgt);
    }

    os_free_mem(dma_map->mapping.discontig.submaps);
}

void nv_load_dma_map_scatterlist(
    nv_dma_map_t *dma_map,
    NvU64 *va_array
)
{
    unsigned int i, j;
    struct scatterlist *sg;
    nv_dma_submap_t *submap;
    NvU64 sg_addr, sg_off, sg_len, k, l = 0;

    NV_FOR_EACH_DMA_SUBMAP(dma_map, submap, i)
    {
        for_each_sg(submap->sgt.sgl, sg, submap->sg_map_count, j)
        {
            /*
             * It is possible for pci_map_sg() to merge scatterlist entries, so
             * make sure we account for that here.
             */
            for (sg_addr = sg_dma_address(sg), sg_len = sg_dma_len(sg),
                    sg_off = 0, k = 0;
                 (sg_off < sg_len) && (k < submap->page_count);
                 sg_off += PAGE_SIZE, l++, k++)
            {
                va_array[l] = sg_addr + sg_off;
            }
        }
    }
}

static NV_STATUS nv_dma_map_scatterlist(
    nv_dma_device_t *dma_dev,
    nv_dma_map_t    *dma_map,
    NvU64           *va_array
)
{
    NV_STATUS status;
    NvU64 i;

    status = nv_create_dma_map_scatterlist(dma_map);
    if (status != NV_OK)
    {
        NV_DMA_DEV_PRINTF(NV_DBG_ERRORS, dma_dev,
                "Failed to allocate DMA mapping scatterlist!\n");
        return status;
    }

    status = nv_map_dma_map_scatterlist(dma_map);
    if (status != NV_OK)
    {
        NV_DMA_DEV_PRINTF(NV_DBG_ERRORS, dma_dev,
                "Failed to create a DMA mapping!\n");
        nv_destroy_dma_map_scatterlist(dma_map);
        return status;
    }

    nv_load_dma_map_scatterlist(dma_map, va_array);

    for (i = 0; i < dma_map->page_count; i++)
    {
        if (!nv_dma_is_addressable(dma_dev, va_array[i], PAGE_SIZE))
        {
            NV_DMA_DEV_PRINTF(NV_DBG_ERRORS, dma_dev,
                    "DMA address not in addressable range of device "
                    "(0x%llx, 0x%llx-0x%llx)\n",
                    va_array[i], dma_dev->addressable_range.start,
                    dma_dev->addressable_range.limit);
            nv_dma_unmap_scatterlist(dma_map);
            return NV_ERR_INVALID_ADDRESS;
        }
    }

    return NV_OK;
}

static void nv_dma_unmap_scatterlist(nv_dma_map_t *dma_map)
{
    nv_unmap_dma_map_scatterlist(dma_map);
    nv_destroy_dma_map_scatterlist(dma_map);
}

static void nv_dma_nvlink_addr_compress
(
    nv_dma_device_t *dma_dev,
    NvU64           *va_array,
    NvU64            page_count,
    NvBool           contig
)
{
#if defined(NVCPU_PPC64LE)
    NvU64 addr = 0;
    NvU64 i;

    /*
     * On systems that support NVLink sysmem links, apply the required address
     * compression scheme when links are trained. Otherwise check that PCIe and
     * NVLink DMA mappings are equivalent as per requirements of Bug 1920398.
     */
    if (dma_dev->nvlink)
    {
        for (i = 0; i < (contig ? 1 : page_count); i++)
        {
            va_array[i] = nv_compress_nvlink_addr(va_array[i]);
        }

        return;
    }

    for (i = 0; i < (contig ? 1 : page_count); i++)
    {
        addr = nv_compress_nvlink_addr(va_array[i]);
        if (WARN_ONCE(va_array[i] != addr,
                      "unexpected DMA address compression (0x%llx, 0x%llx)\n",
                      va_array[i], addr))
        {
            break;
        }
    }
#endif
}

static void nv_dma_nvlink_addr_decompress
(
    nv_dma_device_t *dma_dev,
    NvU64           *va_array,
    NvU64            page_count,
    NvBool           contig
)
{
#if defined(NVCPU_PPC64LE)
    NvU64 i;

    if (dma_dev->nvlink)
    {
        for (i = 0; i < (contig ? 1 : page_count); i++)
        {
            va_array[i] = nv_expand_nvlink_addr(va_array[i]);
        }
    }
#endif
}

NV_STATUS NV_API_CALL nv_dma_map_sgt(
    nv_dma_device_t *dma_dev,
    NvU64            page_count,
    NvU64           *va_array,
    NvU32            cache_type,
    void           **priv
)
{
    NV_STATUS status;
    nv_dma_map_t *dma_map = NULL;

    if (priv == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (page_count > NV_NUM_PHYSPAGES)
    {
        NV_DMA_DEV_PRINTF(NV_DBG_ERRORS, dma_dev,
                "DMA mapping request too large!\n");
        return NV_ERR_INVALID_REQUEST;
    }

    status = os_alloc_mem((void **)&dma_map, sizeof(nv_dma_map_t));
    if (status != NV_OK)
    {
        NV_DMA_DEV_PRINTF(NV_DBG_ERRORS, dma_dev,
                "Failed to allocate nv_dma_map_t!\n");
        return status;
    }

    dma_map->dev = dma_dev->dev;
    dma_map->pages = NULL;
    dma_map->import_sgt = (struct sg_table *) *priv;
    dma_map->page_count = page_count;
    dma_map->contiguous = NV_FALSE;
    dma_map->cache_type = cache_type;

    dma_map->mapping.discontig.submap_count = 0;
    status = nv_dma_map_scatterlist(dma_dev, dma_map, va_array);

    if (status != NV_OK)
    {
        os_free_mem(dma_map);
    }
    else
    {
        *priv = dma_map;
        nv_dma_nvlink_addr_compress(dma_dev, va_array, dma_map->page_count,
                                    dma_map->contiguous);
    }

    return status;
}

NV_STATUS NV_API_CALL nv_dma_unmap_sgt(
    nv_dma_device_t *dma_dev,
    void           **priv
)
{
    nv_dma_map_t *dma_map;

    if (priv == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    dma_map = *priv;

    *priv = NULL;

    nv_dma_unmap_scatterlist(dma_map);

    os_free_mem(dma_map);

    return NV_OK;
}

static NV_STATUS NV_API_CALL nv_dma_map_pages(
    nv_dma_device_t *dma_dev,
    NvU64            page_count,
    NvU64           *va_array,
    NvBool           contig,
    NvU32            cache_type,
    void           **priv
)
{
    NV_STATUS status;
    nv_dma_map_t *dma_map = NULL;

    if (priv == NULL)
    {
        /*
         * IOMMU path has not been implemented yet to handle
         * anything except a nv_dma_map_t as the priv argument.
         */
        return NV_ERR_NOT_SUPPORTED;
    }

    if (page_count > NV_NUM_PHYSPAGES)
    {
        NV_DMA_DEV_PRINTF(NV_DBG_ERRORS, dma_dev,
                "DMA mapping request too large!\n");
        return NV_ERR_INVALID_REQUEST;
    }

    status = os_alloc_mem((void **)&dma_map, sizeof(nv_dma_map_t));
    if (status != NV_OK)
    {
        NV_DMA_DEV_PRINTF(NV_DBG_ERRORS, dma_dev,
                "Failed to allocate nv_dma_map_t!\n");
        return status;
    }

    dma_map->dev = dma_dev->dev;
    dma_map->pages = *priv;
    dma_map->import_sgt = NULL;
    dma_map->page_count = page_count;
    dma_map->contiguous = contig;
    dma_map->cache_type = cache_type;

    if (dma_map->page_count > 1 && !dma_map->contiguous)
    {
        dma_map->mapping.discontig.submap_count = 0;
        status = nv_dma_map_scatterlist(dma_dev, dma_map, va_array);
    }
    else
    {
        /*
         * Force single-page mappings to be contiguous to avoid scatterlist
         * overhead.
         */
        dma_map->contiguous = NV_TRUE;

        status = nv_dma_map_contig(dma_dev, dma_map, va_array);
    }

    if (status != NV_OK)
    {
        os_free_mem(dma_map);
    }
    else
    {
        *priv = dma_map;
        nv_dma_nvlink_addr_compress(dma_dev, va_array, dma_map->page_count,
                                    dma_map->contiguous);
    }

    return status;
}

static NV_STATUS NV_API_CALL nv_dma_unmap_pages(
    nv_dma_device_t *dma_dev,
    NvU64            page_count,
    NvU64           *va_array,
    void           **priv
)
{
    nv_dma_map_t *dma_map;

    if (priv == NULL)
    {
        /*
         * IOMMU path has not been implemented yet to handle
         * anything except a nv_dma_map_t as the priv argument.
         */
        return NV_ERR_NOT_SUPPORTED;
    }

    dma_map = *priv;

    if (page_count > NV_NUM_PHYSPAGES)
    {
        NV_DMA_DEV_PRINTF(NV_DBG_ERRORS, dma_dev,
                "DMA unmapping request too large!\n");
        return NV_ERR_INVALID_REQUEST;
    }

    if (page_count != dma_map->page_count)
    {
        NV_DMA_DEV_PRINTF(NV_DBG_WARNINGS, dma_dev,
                "Requested to DMA unmap %llu pages, but there are %llu "
                "in the mapping\n", page_count, dma_map->page_count);
        return NV_ERR_INVALID_REQUEST;
    }

    *priv = dma_map->pages;

    if (dma_map->contiguous)
    {
        nv_dma_unmap_contig(dma_map);
    }
    else
    {
        nv_dma_unmap_scatterlist(dma_map);
    }

    os_free_mem(dma_map);

    return NV_OK;
}

/*
 * Wrappers used for DMA-remapping an nv_alloc_t during transition to more
 * generic interfaces.
 */
NV_STATUS NV_API_CALL nv_dma_map_alloc
(
    nv_dma_device_t *dma_dev,
    NvU64            page_count,
    NvU64           *va_array,
    NvBool           contig,
    void           **priv
)
{
    NV_STATUS status;
    NvU64 i;
    nv_alloc_t *at = *priv;
    struct page **pages = NULL;
    NvU32 cache_type = NV_MEMORY_CACHED;
    NvU64 pages_size = sizeof(struct page *) * (contig ? 1 : page_count);

    /* If we have an imported SGT, just use that directly. */
    if (at && at->import_sgt)
    {
        *priv = at->import_sgt;
        status = nv_dma_map_sgt(dma_dev, page_count, va_array, at->cache_type,
                                priv);
        if (status != NV_OK)
        {
            *priv = at;
        }
        return status;
    }

    /*
     * Convert the nv_alloc_t into a struct page * array for
     * nv_dma_map_pages().
     */
    status = os_alloc_mem((void **)&pages, pages_size);
    if (status != NV_OK)
    {
        NV_DMA_DEV_PRINTF(NV_DBG_ERRORS, dma_dev,
                "Failed to allocate page array for DMA mapping!\n");
        return status;
    }

    os_mem_set(pages, 0, pages_size);

    if (at != NULL)
    {
        WARN_ON(page_count != at->num_pages);

        if (at->flags.user)
        {
            pages[0] = at->user_pages[0];
            if (!contig)
            {
                for (i = 1; i < page_count; i++)
                {
                    pages[i] = at->user_pages[i];
                }
            }
        }
        else if (at->flags.physical && contig)
        {
            /* Supplied pages hold physical address */
            pages[0] = pfn_to_page(PFN_DOWN(va_array[0]));
        }
        cache_type = at->cache_type;
    }

    if (pages[0] == NULL)
    {
        pages[0] = NV_GET_PAGE_STRUCT(va_array[0]);
        if (!contig)
        {
            for (i = 1; i < page_count; i++)
            {
                pages[i] = NV_GET_PAGE_STRUCT(va_array[i]);
            }
        }
    }

    *priv = pages;
    status = nv_dma_map_pages(dma_dev, page_count, va_array, contig, cache_type,
                              priv);
    if (status != NV_OK)
    {
        *priv = at;
        os_free_mem(pages);
    }

    return status;
}

NV_STATUS NV_API_CALL nv_dma_unmap_alloc
(
    nv_dma_device_t *dma_dev,
    NvU64            page_count,
    NvU64           *va_array,
    void           **priv
)
{
    NV_STATUS status = NV_OK;
    nv_dma_map_t *dma_map;

    if (priv == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    dma_map = *priv;

    if (!dma_map->import_sgt)
    {
        status = nv_dma_unmap_pages(dma_dev, page_count, va_array, priv);
        if (status != NV_OK)
        {
            /*
             * If nv_dma_unmap_pages() fails, we hit an assert condition and the
             * priv argument won't be the page array we allocated in
             * nv_dma_map_alloc(), so we skip the free here. But note that since
             * this is an assert condition it really should never happen.
             */
            return status;
        }

        /* Free the struct page * array allocated by nv_dma_map_alloc() */
        os_free_mem(*priv);
    } else {
        status = nv_dma_unmap_sgt(dma_dev, priv);
    }

    return status;
}

static NvBool nv_dma_use_map_resource
(
    nv_dma_device_t *dma_dev
)
{
#if defined(NV_DMA_MAP_RESOURCE_PRESENT)
    const struct dma_map_ops *ops = get_dma_ops(dma_dev->dev);
#endif

    if (nv_dma_remap_peer_mmio == NV_DMA_REMAP_PEER_MMIO_DISABLE)
    {
        return NV_FALSE;
    }

#if defined(NV_DMA_MAP_RESOURCE_PRESENT)
    if (ops == NULL)
    {
        /* On pre-5.0 kernels, if dma_map_resource() is present, then we
         * assume that ops != NULL.  With direct_dma handling swiotlb on 5.0+
         * kernels, ops == NULL.
         */
#if defined(NV_DMA_IS_DIRECT_PRESENT)
        return NV_TRUE;
#else
        return NV_FALSE;
#endif
    }

    return (ops->map_resource != NULL);
#else
    return NV_FALSE;
#endif
}

/* DMA-map a peer PCI device's BAR for peer access. */
NV_STATUS NV_API_CALL nv_dma_map_peer
(
    nv_dma_device_t *dma_dev,
    nv_dma_device_t *peer_dma_dev,
    NvU8             nv_bar_index,
    NvU64            page_count,
    NvU64           *va
)
{
    struct pci_dev *peer_pci_dev = to_pci_dev(peer_dma_dev->dev);
    struct resource *res;
    NvU8 bar_index;
    NV_STATUS status;

    if (peer_pci_dev == NULL)
    {
        NV_DMA_DEV_PRINTF(NV_DBG_ERRORS, peer_dma_dev,
            "Not a PCI device");
        return NV_ERR_INVALID_REQUEST;
    }

    bar_index = nv_bar_index_to_os_bar_index(peer_pci_dev, nv_bar_index);
    res = &peer_pci_dev->resource[bar_index];
    if (res->start == 0)
    {
        NV_DMA_DEV_PRINTF(NV_DBG_ERRORS, peer_dma_dev,
                "Resource %u not valid",
                bar_index);
        return NV_ERR_INVALID_REQUEST;
    }

    if ((*va < res->start) || ((*va + (page_count * PAGE_SIZE)) > res->end))
    {
        NV_DMA_DEV_PRINTF(NV_DBG_ERRORS, peer_dma_dev,
                "Mapping requested (start = 0x%llx, page_count = 0x%llx)"
                " outside of resource bounds (start = 0x%llx, end = 0x%llx)\n",
                *va, page_count, res->start, res->end);
        return NV_ERR_INVALID_REQUEST;
    }

    if (nv_dma_use_map_resource(dma_dev))
    {
        status = nv_dma_map_mmio(dma_dev, page_count, va);
    }
    else
    {
        /*
         * Best effort - can't map through the iommu but at least try to
         * convert to a bus address.
         */
        NvU64 offset = *va - res->start;
        *va = nv_pci_bus_address(peer_pci_dev, bar_index) + offset;
        status = NV_OK;
    }

    return status;
}

void NV_API_CALL nv_dma_unmap_peer
(
    nv_dma_device_t *dma_dev,
    NvU64            page_count,
    NvU64            va
)
{
    if (nv_dma_use_map_resource(dma_dev))
    {
        nv_dma_unmap_mmio(dma_dev, page_count, va);
    }
}

/* DMA-map another anonymous device's MMIO region for peer access. */
NV_STATUS NV_API_CALL nv_dma_map_mmio
(
    nv_dma_device_t *dma_dev,
    NvU64            page_count,
    NvU64           *va
)
{
#if defined(NV_DMA_MAP_RESOURCE_PRESENT)
    BUG_ON(!va);

    if (nv_dma_use_map_resource(dma_dev))
    {
        NvU64 mmio_addr = *va;
        *va = dma_map_resource(dma_dev->dev, mmio_addr, page_count * PAGE_SIZE,
                               DMA_BIDIRECTIONAL, 0);
        if (dma_mapping_error(dma_dev->dev, *va))
        {
            NV_DMA_DEV_PRINTF(NV_DBG_ERRORS, dma_dev,
                    "Failed to DMA map MMIO range [0x%llx-0x%llx]\n",
                    mmio_addr, mmio_addr + page_count * PAGE_SIZE - 1);
            return NV_ERR_OPERATING_SYSTEM;
        }
    }
    else
    {
        /*
         * If dma_map_resource is not available, pass through the source address
         * without failing. Further, adjust it using the DMA start address to
         * keep RM's validation schemes happy.
         */
        *va = *va + dma_dev->addressable_range.start;
    }

    nv_dma_nvlink_addr_compress(dma_dev, va, page_count, NV_TRUE);

    return NV_OK;
#else
    return NV_ERR_NOT_SUPPORTED;
#endif
}

void NV_API_CALL nv_dma_unmap_mmio
(
    nv_dma_device_t *dma_dev,
    NvU64            page_count,
    NvU64            va
)
{
#if defined(NV_DMA_MAP_RESOURCE_PRESENT)
    nv_dma_nvlink_addr_decompress(dma_dev, &va, page_count, NV_TRUE);

    if (nv_dma_use_map_resource(dma_dev))
    {
        dma_unmap_resource(dma_dev->dev, va, page_count * PAGE_SIZE,
                           DMA_BIDIRECTIONAL, 0);
    }
#endif
}

/*
 * Invalidate DMA mapping in CPU caches by "syncing" to the device.
 *
 * This is only implemented for ARM platforms, since other supported
 * platforms are cache coherent and have not required this (we
 * explicitly haven't supported SWIOTLB bounce buffering either where
 * this would be needed).
 */
void NV_API_CALL nv_dma_cache_invalidate
(
    nv_dma_device_t *dma_dev,
    void *priv
)
{
#if defined(NVCPU_AARCH64)
    nv_dma_map_t *dma_map = priv;

    if (dma_map->contiguous)
    {
        dma_sync_single_for_device(dma_dev->dev,
                                   dma_map->mapping.contig.dma_addr,
                                   (size_t) PAGE_SIZE * dma_map->page_count,
                                   DMA_FROM_DEVICE);
    }
    else
    {
        nv_dma_submap_t *submap;
        NvU64 i;

        NV_FOR_EACH_DMA_SUBMAP(dma_map, submap, i)
        {
            dma_sync_sg_for_device(dma_dev->dev,
                                   submap->sgt.sgl,
                                   submap->sgt.orig_nents,
                                   DMA_FROM_DEVICE);
        }
    }
#endif
}

/* Enable DMA-mapping over NVLink */
void NV_API_CALL nv_dma_enable_nvlink
(
    nv_dma_device_t *dma_dev
)
{
    dma_dev->nvlink = NV_TRUE;
}

#if defined(NV_LINUX_DMA_BUF_H_PRESENT) && \
    defined(NV_DRM_AVAILABLE) && defined(NV_DRM_DRM_GEM_H_PRESENT)

/*
 * drm_gem_object_{get/put}() added by commit
 * e6b62714e87c8811d5564b6a0738dcde63a51774 (2017-02-28) and
 * drm_gem_object_{reference/unreference}() removed by commit
 * 3e70fd160cf0b1945225eaa08dd2cb8544f21cb8 (2018-11-15).
 */

static inline void
nv_dma_gem_object_unreference_unlocked(struct drm_gem_object *gem)
{
#if defined(NV_DRM_GEM_OBJECT_GET_PRESENT)

#if defined(NV_DRM_GEM_OBJECT_PUT_UNLOCK_PRESENT)
    drm_gem_object_put_unlocked(gem);
#else
    drm_gem_object_put(gem);
#endif

#else
    drm_gem_object_unreference_unlocked(gem);
#endif
}

static inline void
nv_dma_gem_object_reference(struct drm_gem_object *gem)
{
#if defined(NV_DRM_GEM_OBJECT_GET_PRESENT)
    drm_gem_object_get(gem);
#else
    drm_gem_object_reference(gem);
#endif
}

NV_STATUS NV_API_CALL nv_dma_import_sgt
(
    nv_dma_device_t *dma_dev,
    struct sg_table *sgt,
    struct drm_gem_object *gem
)
{
    if ((dma_dev == NULL) ||
        (sgt == NULL) ||
        (gem == NULL))
    {
        NV_DMA_DEV_PRINTF(NV_DBG_ERRORS, dma_dev,
                "Import arguments are NULL!\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Prevent the kernel module controlling GEM from being unloaded
    if (!try_module_get(gem->dev->driver->fops->owner))
    {
        NV_DMA_DEV_PRINTF(NV_DBG_ERRORS, dma_dev,
                "Couldn't reference the GEM object's owner!\n");
        return NV_ERR_INVALID_DEVICE;
    }

    // Do nothing with SGT, it is already mapped and pinned by the exporter

    nv_dma_gem_object_reference(gem);

    return NV_OK;
}

void NV_API_CALL nv_dma_release_sgt
(
    struct sg_table *sgt,
    struct drm_gem_object *gem
)
{
    if (gem == NULL)
    {
        return;
    }

    // Do nothing with SGT, it will be unmapped and unpinned by the exporter
    WARN_ON(sgt == NULL);

    nv_dma_gem_object_unreference_unlocked(gem);

    module_put(gem->dev->driver->fops->owner);
}

#else

NV_STATUS NV_API_CALL nv_dma_import_sgt
(
    nv_dma_device_t *dma_dev,
    struct sg_table *sgt,
    struct drm_gem_object *gem
)
{
    return NV_ERR_NOT_SUPPORTED;
}

void NV_API_CALL nv_dma_release_sgt
(
    struct sg_table *sgt,
    struct drm_gem_object *gem
)
{
}
#endif /* NV_LINUX_DMA_BUF_H_PRESENT && NV_DRM_AVAILABLE && NV_DRM_DRM_GEM_H_PRESENT */
