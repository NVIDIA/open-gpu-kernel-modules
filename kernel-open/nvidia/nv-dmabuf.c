/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include <linux/dma-buf.h>
#include "nv-dmabuf.h"

#if defined(CONFIG_DMA_SHARED_BUFFER)

typedef struct nv_dma_buf_mem_handle
{
    // Memory handle, offset and size
    NvHandle                 h_memory;
    NvU64                    offset;
    NvU64                    size;

    // RM memdesc specific data
    void                    *mem_info;

    //
    // Refcount for phys addresses
    // If refcount > 0, phys address ranges in memArea are reused.
    //
    NvU64                    phys_refcount;

    // Scatterlist of all the memory ranges associated with the buf
    MemoryArea               memArea;
} nv_dma_buf_mem_handle_t;

typedef struct nv_dma_buf_file_private
{
    // GPU device state
    nv_state_t              *nv;

    // Client, device, subdevice handles
    NvHandle                 h_client;
    NvHandle                 h_device;
    NvHandle                 h_subdevice;

    // Total number of handles supposed to be attached to this dma-buf
    NvU32                    total_objects;

    //
    // Number of handles actually attached to this dma-buf.
    // This should equal total_objects, or map fails.
    //
    NvU32                    num_objects;

    // Total size of all handles supposed to be attached to this dma-buf
    NvU64                    total_size;

    //
    // Size of all handles actually attached to the dma-buf
    // If all handles are attached, total_size and attached_size must match.
    //
    NvU64                    attached_size;

    // Mutex to lock priv state during dma-buf callbacks
    struct mutex             lock;

    // Handle info: see nv_dma_buf_mem_handle_t
    nv_dma_buf_mem_handle_t *handles;

    // RM-private info for MIG configs
    void                    *mig_info;

    //
    // Flag to indicate if phys addresses are static and can be
    // fetched during dma-buf create/reuse instead of in map.
    //
    NvBool                   static_phys_addrs;

    //
    // Type of mapping requested, one of:
    //   NV_DMABUF_EXPORT_MAPPING_TYPE_DEFAULT
    //   NV_DMABUF_EXPORT_MAPPING_TYPE_FORCE_PCIE
    //
    NvU8                     mapping_type;

    //
    // On some coherent platforms requesting mapping_type FORCE_PCIE,
    // peer-to-peer is expected to bypass the IOMMU due to hardware
    // limitations. On such systems, IOMMU map/unmap will be skipped.
    //
    NvBool                   skip_iommu;

    struct
    {
        // True if the map attributes are cached
        NvBool               cached;

        // Flag to indicate if dma-buf mmap is allowed
        NvBool               can_mmap;

        //
        // Flag to indicate if client/user is allowed dma-buf mmap or not.
        // That way user can enable mmap for testing/specific
        // use cases and not for any all handles.
        //
        NvU64                allow_mmap;

        // RM-private info for cache type settings (cached/uncached/writecombined).
        NvU32                cache_type;

        // Flag to indicate if dma-buf is RO or RW memory.
        NvBool               read_only_mem;

        // Memory type info: see nv_memory_type_t.
        nv_memory_type_t     memory_type;
    } map_attrs;

    //
    // Flag to indicate if all GPU locks to be acquired/released before/after calling
    // rm_dma_buf_dup_mem_handle().
    // nv_dma_buf_dup_mem_handles() acquires GPU lock only for calling pGPU
    // instance. However, it is not sufficient as per DupObject() SYSMEM's design
    // since it expects either all GPU locks to be acquired by the caller or
    // do not take any GPU locks. This flag is set to TRUE only for
    // ZERO_FB chips.
    //
    NvBool                   acquire_release_all_gpu_lock_on_dup;
} nv_dma_buf_file_private_t;

static void
nv_dma_buf_free_file_private(
    nv_dma_buf_file_private_t *priv
)
{
    if (priv == NULL)
    {
        return;
    }

    if (priv->handles != NULL)
    {
        os_free_mem(priv->handles);
        priv->handles = NULL;
    }

    mutex_destroy(&priv->lock);

    NV_KFREE(priv, sizeof(nv_dma_buf_file_private_t));
}

static nv_dma_buf_file_private_t*
nv_dma_buf_alloc_file_private(
    NvU32 num_handles
)
{
    nv_dma_buf_file_private_t *priv = NULL;
    NvU64 handles_size = num_handles * sizeof(priv->handles[0]);
    NV_STATUS status;

    NV_KZALLOC(priv, sizeof(nv_dma_buf_file_private_t));
    if (priv == NULL)
    {
        return NULL;
    }

    mutex_init(&priv->lock);

    status = os_alloc_mem((void **) &priv->handles, handles_size);
    if (status != NV_OK)
    {
        goto failed;
    }
    os_mem_set(priv->handles, 0, handles_size);

    return priv;

failed:
    nv_dma_buf_free_file_private(priv);

    return NULL;
}

static void
nv_reset_phys_refcount(
    nv_dma_buf_file_private_t *priv,
    NvU32 start_index,
    NvU32 handle_count
)
{
    NvU32 i;
    for (i = 0; i < handle_count; i++)
    {
        NvU32 index = start_index + i;
        priv->handles[index].phys_refcount = 0;
    }
}

static NvBool
nv_dec_and_check_zero_phys_refcount(
    nv_dma_buf_file_private_t *priv,
    NvU32 start_index,
    NvU32 handle_count
)
{
    NvU32 i;
    NvBool is_zero = NV_FALSE;

    for (i = 0; i < handle_count; i++)
    {
        NvU32 index = start_index + i;
        priv->handles[index].phys_refcount--;
        if (priv->handles[index].phys_refcount == 0)
        {
            is_zero = NV_TRUE;
        }
    }

    return is_zero;
}

static NvBool
nv_inc_and_check_one_phys_refcount(
    nv_dma_buf_file_private_t *priv,
    NvU32 start_index,
    NvU32 handle_count
)
{
    NvU32 i;
    NvBool is_one = NV_FALSE;

    for (i = 0; i < handle_count; i++)
    {
        NvU32 index = start_index + i;
        priv->handles[index].phys_refcount++;
        if (priv->handles[index].phys_refcount == 1)
        {
            is_one = NV_TRUE;
        }
    }

    return is_one;
}

// Must be called with RMAPI lock and GPU lock taken
static void
nv_dma_buf_undup_mem_handles_unlocked(
    nvidia_stack_t            *sp,
    NvU32                      start_index,
    NvU32                      num_objects,
    nv_dma_buf_file_private_t *priv
)
{
    NvU32 index, i;

    for (i = 0; i < num_objects; i++)
    {
        index = start_index + i;

        if (priv->handles[index].h_memory == 0)
        {
            continue;
        }

        rm_dma_buf_undup_mem_handle(sp, priv->nv, priv->h_client,
                                    priv->handles[index].h_memory);

        priv->attached_size -= priv->handles[index].size;
        priv->handles[index].h_memory = 0;
        priv->handles[index].offset = 0;
        priv->handles[index].size = 0;
        priv->num_objects--;
    }
}

static void
nv_dma_buf_undup_mem_handles(
    nvidia_stack_t            *sp,
    NvU32                      index,
    NvU32                      num_objects,
    nv_dma_buf_file_private_t *priv
)
{
    NV_STATUS status;

    status = rm_acquire_api_lock(sp);
    if (WARN_ON(status != NV_OK))
    {
        return;
    }

    status = rm_acquire_all_gpus_lock(sp);
    if (WARN_ON(status != NV_OK))
    {
        goto unlock_api_lock;
    }

    nv_dma_buf_undup_mem_handles_unlocked(sp, index, num_objects, priv);

    rm_release_all_gpus_lock(sp);

unlock_api_lock:
    rm_release_api_lock(sp);
}

//
// TODO: Temporary work around for SYSMEM Dup issue.
// Take all GPU locks before calling the DupObject().
// DupObject() requires the caller to either acquire all GPU locks beforehand or
// refrain from acquiring any GPU locks before invoking it.
// Otherwise DupObject() will fail for already locked gpu instance with below error print
// for multi gpu instance use case:
// "GPU lock already acquired by this thread" for gpuInst which is already locked during
// nv_dma_buf_dup_mem_handles().
// In TOT, nv_dma_buf_dup_mem_handles() acquires GPU lock only for calling pGPU
// instance. However, it is not sufficient as per DupObject() SYSMEM's design since it expects
// either all GPU locks to be acquired by the caller or do not take any GPU locks.
// gpuarchIsZeroFb chips (iGPU) doesn't have local memory. In this case,
// SYSMEM is used as Device resources. priv->acquire_release_all_gpu_lock_on_dup flag set as
// NV_TRUE only for gpuarchIsZeroFb chips.
//
// Proper Fix (Bug 4866388):
// The RS_FLAGS_ACQUIRE_RELAXED_GPUS_LOCK_ON_DUP flag was introduced to allow an
// RM class to take GPU Group Lock if the source and the destination object
// belongs to the same pGpu. Take all GPUs lock otherwise.
// With above change, we are seeing test failures.
// Until the above proper fix is added, we need to rely on temporary work around.
//
static inline NV_STATUS
nv_dma_buf_acquire_gpu_lock(
    nvidia_stack_t                  *sp,
    nv_dma_buf_file_private_t       *priv
)
{
    return (priv->acquire_release_all_gpu_lock_on_dup ?
            rm_acquire_all_gpus_lock(sp): rm_acquire_gpu_lock(sp, priv->nv));
}

static inline NV_STATUS
nv_dma_buf_release_gpu_lock(
    nvidia_stack_t                  *sp,
    nv_dma_buf_file_private_t       *priv
)
{
    return (priv->acquire_release_all_gpu_lock_on_dup ?
            rm_release_all_gpus_lock(sp): rm_release_gpu_lock(sp, priv->nv));
}

static NV_STATUS
nv_dma_buf_dup_mem_handles(
    nvidia_stack_t                  *sp,
    nv_dma_buf_file_private_t       *priv,
    nv_ioctl_export_to_dma_buf_fd_t *params
)
{
    NV_STATUS status = NV_OK;
    NvU32 index = params->index;
    NvU32 count = 0;
    NvU32 i = 0;

    status = rm_acquire_api_lock(sp);
    if (status != NV_OK)
    {
        return status;
    }

    status = nv_dma_buf_acquire_gpu_lock(sp, priv);
    if (status != NV_OK)
    {
        goto unlock_api_lock;
    }

    for (i = 0; i < params->numObjects; i++)
    {
        NvHandle h_memory_duped = 0;
        void *mem_info = NULL;
        nv_memory_type_t memory_type = NV_MEMORY_TYPE_SYSTEM;
        NvBool can_mmap;
        NvU32 cache_type;
        NvBool read_only_mem;

        if (priv->handles[index].h_memory != 0)
        {
            status = NV_ERR_IN_USE;
            goto failed;
        }

        if (params->sizes[i] > priv->total_size - priv->attached_size)
        {
            status = NV_ERR_INVALID_ARGUMENT;
            goto failed;
        }

        status = rm_dma_buf_dup_mem_handle(sp, priv->nv,
                                           params->hClient,
                                           priv->h_client,
                                           priv->h_device,
                                           priv->h_subdevice,
                                           priv->mig_info,
                                           params->handles[i],
                                           params->offsets[i],
                                           params->sizes[i],
                                           &h_memory_duped,
                                           &mem_info,
                                           &can_mmap,
                                           &cache_type,
                                           &read_only_mem,
                                           &memory_type);
        if (status != NV_OK)
        {
            goto failed;
        }

        if (priv->map_attrs.cached)
        {
            if ((can_mmap      != priv->map_attrs.can_mmap) ||
                (cache_type    != priv->map_attrs.cache_type) ||
                (read_only_mem != priv->map_attrs.read_only_mem) ||
                (memory_type   != priv->map_attrs.memory_type))
            {
                // Creating mixed dma_buf is not supported.
                status = NV_ERR_INVALID_ARGUMENT;
                goto failed;
            }
        }
        else
        {
            // Store the handle's mmap, RO and cache type info.
            priv->map_attrs.can_mmap      = can_mmap;
            priv->map_attrs.cache_type    = cache_type;
            priv->map_attrs.read_only_mem = read_only_mem;
            priv->map_attrs.memory_type   = memory_type;
            priv->map_attrs.cached        = NV_TRUE;
        }

        priv->attached_size += params->sizes[i];
        priv->handles[index].h_memory = h_memory_duped;
        priv->handles[index].offset = params->offsets[i];
        priv->handles[index].size = params->sizes[i];
        priv->handles[index].mem_info = mem_info;
        priv->num_objects++;
        index++;
        count++;
    }

    if ((priv->num_objects == priv->total_objects) &&
        (priv->attached_size != priv->total_size))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        goto failed;
    }

    nv_dma_buf_release_gpu_lock(sp, priv);

    rm_release_api_lock(sp);

    return NV_OK;

failed:
    if (!priv->acquire_release_all_gpu_lock_on_dup)
    {
        //
        // Undup requires taking all-GPUs lock.
        // So if single GPU lock was taken,
        // release it first so all-GPUs lock can be taken in
        // nv_dma_buf_undup_mem_handles().
        //
        nv_dma_buf_release_gpu_lock(sp, priv);

        nv_dma_buf_undup_mem_handles(sp, params->index, count, priv);
    }
    else
    {
        //
        // Here, all-GPUs lock is already taken, so undup the handles under
        // the unlocked version of the function and then release the locks.
        //
        nv_dma_buf_undup_mem_handles_unlocked(sp, params->index, count, priv);

        nv_dma_buf_release_gpu_lock(sp, priv);
    }

unlock_api_lock:
    rm_release_api_lock(sp);

    return status;
}

static void
nv_put_phys_addresses(
    nvidia_stack_t *sp,
    nv_dma_buf_file_private_t *priv,
    NvU32 start_index,
    NvU32 mapped_handle_count
)
{
    NvU32 i;

    for (i = 0; i < mapped_handle_count; i++)
    {
        NvU32 index = start_index + i;

        if (priv->handles[index].phys_refcount > 0)
        {
            continue;
        }

        // Per-handle memArea is freed by RM
        rm_dma_buf_unmap_mem_handle(sp, priv->nv, priv->h_client,
                                    priv->handles[index].h_memory,
                                    priv->mapping_type,
                                    priv->handles[index].mem_info,
                                    priv->static_phys_addrs,
                                    priv->handles[index].memArea);

        priv->handles[index].memArea.numRanges = 0;
    }
}

static void
nv_dma_buf_put_phys_addresses (
    nv_dma_buf_file_private_t *priv,
    NvU32 start_index,
    NvU32 handle_count
)
{
    NV_STATUS status;
    nvidia_stack_t *sp = NULL;
    NvBool api_lock_taken = NV_FALSE;
    NvBool gpu_lock_taken = NV_FALSE;
    int rc = 0;

    if (!nv_dec_and_check_zero_phys_refcount(priv, start_index, handle_count))
    {
        return;
    }

    rc = nv_kmem_cache_alloc_stack(&sp);
    if (WARN_ON(rc != 0))
    {
        return;
    }

    if (!priv->static_phys_addrs)
    {
        status = rm_acquire_api_lock(sp);
        if (WARN_ON(status != NV_OK))
        {
            goto free_sp;
        }
        api_lock_taken = NV_TRUE;

        status = rm_acquire_gpu_lock(sp, priv->nv);
        if (WARN_ON(status != NV_OK))
        {
            goto unlock_api_lock;
        }
        gpu_lock_taken = NV_TRUE;
    }

    nv_put_phys_addresses(sp, priv, start_index, handle_count);

    if (gpu_lock_taken)
    {
        rm_release_gpu_lock(sp, priv->nv);
    }

unlock_api_lock:
    if (api_lock_taken)
    {
        rm_release_api_lock(sp);
    }

free_sp:
    nv_kmem_cache_free_stack(sp);
}

static NV_STATUS
nv_dma_buf_get_phys_addresses (
    nv_dma_buf_file_private_t *priv,
    NvU32 start_index,
    NvU32 handle_count
)
{
    NV_STATUS status = NV_OK;
    nvidia_stack_t *sp = NULL;
    NvBool api_lock_taken = NV_FALSE;
    NvBool gpu_lock_taken = NV_FALSE;
    NvU32 i;
    int rc = 0;

    if (!nv_inc_and_check_one_phys_refcount(priv, start_index, handle_count))
    {
        return NV_OK;
    }

    rc = nv_kmem_cache_alloc_stack(&sp);
    if (rc != 0)
    {
        status = NV_ERR_NO_MEMORY;
        goto failed;
    }

    //
    // Locking is not needed for static phys address configs because the memdesc
    // is not expected to change in this case and we hold the refcount on the
    // owner GPU and memory before referencing it.
    //
    if (!priv->static_phys_addrs)
    {
        status = rm_acquire_api_lock(sp);
        if (status != NV_OK)
        {
            goto free_sp;
        }
        api_lock_taken = NV_TRUE;

        status = rm_acquire_gpu_lock(sp, priv->nv);
        if (status != NV_OK)
        {
            goto unlock_api_lock;
        }
        gpu_lock_taken = NV_TRUE;
    }

    for (i = 0; i < handle_count; i++)
    {
        NvU32 index = start_index + i;

        if (priv->handles[index].phys_refcount > 1)
        {
            continue;
        }

        // Per-handle memArea is allocated by RM
        status = rm_dma_buf_map_mem_handle(sp, priv->nv, priv->h_client,
                                           priv->handles[index].h_memory,
                                           mrangeMake(priv->handles[index].offset,
                                                priv->handles[index].size),
                                           priv->mapping_type,
                                           priv->handles[index].mem_info,
                                           priv->static_phys_addrs,
                                           &priv->handles[index].memArea);
        if (status != NV_OK)
        {
            goto unmap_handles;
        }
    }

    if (gpu_lock_taken)
    {
        rm_release_gpu_lock(sp, priv->nv);
    }

    if (api_lock_taken)
    {
        rm_release_api_lock(sp);
    }

    nv_kmem_cache_free_stack(sp);

    return NV_OK;

unmap_handles:
    nv_put_phys_addresses(sp, priv, start_index, i);

    if (gpu_lock_taken)
    {
        rm_release_gpu_lock(sp, priv->nv);
    }

unlock_api_lock:
    if (api_lock_taken)
    {
        rm_release_api_lock(sp);
    }

free_sp:
    nv_kmem_cache_free_stack(sp);

failed:
    nv_reset_phys_refcount(priv, start_index, handle_count);

    return status;
}

static void
nv_dma_buf_unmap_pages(
    struct device *dev,
    struct sg_table *sgt,
    nv_dma_buf_file_private_t *priv
)
{
    if (priv->skip_iommu)
    {
        return;
    }

    dma_unmap_sg_attrs(dev, sgt->sgl, sgt->nents, DMA_BIDIRECTIONAL, DMA_ATTR_SKIP_CPU_SYNC);
}

static void
nv_dma_buf_unmap_pfns(
    struct device *dev,
    struct sg_table *sgt,
    nv_dma_buf_file_private_t *priv
)
{
    nv_dma_device_t peer_dma_dev = {{ 0 }};
    struct scatterlist *sg = sgt->sgl;
    NvU32 i;

    if (priv->skip_iommu)
    {
        return;
    }

    peer_dma_dev.dev = dev;
    peer_dma_dev.addressable_range.limit = (NvU64)dev->dma_mask;

    for_each_sg(sgt->sgl, sg, sgt->nents, i)
    {
        nv_dma_unmap_peer(&peer_dma_dev,
                          (sg_dma_len(sg) >> PAGE_SHIFT),
                          sg_dma_address(sg));
    }
}

static NvU32
nv_dma_buf_get_sg_count (
    struct device *dev,
    nv_dma_buf_file_private_t *priv,
    NvU32  *max_seg_size
)
{
    NvU32 dma_max_seg_size, i;
    NvU32 nents = 0;

    dma_max_seg_size = NV_ALIGN_DOWN(dma_get_max_seg_size(dev), PAGE_SIZE);
    if (dma_max_seg_size < PAGE_SIZE)
    {
        return 0;
    }

    // Calculate nents needed to allocate sg_table
    for (i = 0; i < priv->num_objects; i++)
    {
        NvU32 range_count = priv->handles[i].memArea.numRanges;
        NvU32 index;

        for (index = 0; index < range_count; index++)
        {
            NvU64 length = priv->handles[i].memArea.pRanges[index].size;
            NvU64 count = length + dma_max_seg_size - 1;
            do_div(count, dma_max_seg_size);
            nents += count;
        }
    }

    *max_seg_size = dma_max_seg_size;

    return nents;
}

static struct sg_table*
nv_dma_buf_map_pages (
    struct device *dev,
    nv_dma_buf_file_private_t *priv
)
{
    struct sg_table *sgt = NULL;
    struct scatterlist *sg;
    NvU32 dma_max_seg_size = 0;
    NvU32 i, nents;
    int rc;

    nents = nv_dma_buf_get_sg_count(dev, priv, &dma_max_seg_size);

    NV_KZALLOC(sgt, sizeof(struct sg_table));
    if (sgt == NULL)
    {
        return NULL;
    }

    rc = sg_alloc_table(sgt, nents, GFP_KERNEL);
    if (rc != 0)
    {
        goto free_sgt;
    }

    sg = sgt->sgl;

    for (i = 0; i < priv->num_objects; i++)
    {
        NvU32 range_count = priv->handles[i].memArea.numRanges;
        NvU32 index = 0;
        for (index = 0; index < range_count; index++)
        {
            NvU64 dma_addr = priv->handles[i].memArea.pRanges[index].start;
            NvU64 dma_len  = priv->handles[i].memArea.pRanges[index].size;

            // Split each range into dma_max_seg_size chunks
            while(dma_len != 0)
            {
                NvU32 sg_len = NV_MIN(dma_len, dma_max_seg_size);
                struct page *page = NV_GET_PAGE_STRUCT(dma_addr);

                if ((page == NULL) || (sg == NULL))
                {
                    goto free_table;
                }

                sg_set_page(sg, page, sg_len, offset_in_page(dma_addr));
                dma_addr += sg_len;
                dma_len -= sg_len;
                sg = sg_next(sg);
            }
        }
    }

    WARN_ON(sg != NULL);

    // DMA map the sg_table
    rc = dma_map_sg_attrs(dev, sgt->sgl, sgt->orig_nents, DMA_BIDIRECTIONAL, DMA_ATTR_SKIP_CPU_SYNC);
    if (rc <= 0)
    {
        goto free_table;
    }
    sgt->nents = rc;

    return sgt;

free_table:
    sg_free_table(sgt);

free_sgt:
    NV_KFREE(sgt, sizeof(struct sg_table));

    return NULL;
}

static struct sg_table*
nv_dma_buf_map_pfns (
    struct device *dev,
    nv_dma_buf_file_private_t *priv
)
{
    NV_STATUS status;
    struct sg_table *sgt = NULL;
    struct scatterlist *sg;
    nv_dma_device_t peer_dma_dev = {{ 0 }};
    NvU32 dma_max_seg_size = 0;
    NvU32 mapped_nents = 0;
    NvU32 i = 0;
    NvU32 nents;
    int rc = 0;

    peer_dma_dev.dev = dev;
    peer_dma_dev.addressable_range.limit = (NvU64)dev->dma_mask;

    nents = nv_dma_buf_get_sg_count(dev, priv, &dma_max_seg_size);

    NV_KZALLOC(sgt, sizeof(struct sg_table));
    if (sgt == NULL)
    {
        return NULL;
    }

    rc = sg_alloc_table(sgt, nents, GFP_KERNEL);
    if (rc != 0)
    {
        goto free_sgt;
    }

    sg = sgt->sgl;
    for (i = 0; i < priv->num_objects; i++)
    {
        NvU32 range_count = priv->handles[i].memArea.numRanges;
        NvU32 index = 0;

        for (index = 0; index < range_count; index++)
        {
            NvU64 phys_addr = priv->handles[i].memArea.pRanges[index].start;
            NvU64 dma_len  = priv->handles[i].memArea.pRanges[index].size;

            // Break the scatterlist into dma_max_seg_size chunks
            while(dma_len != 0)
            {
                NvU64 dma_addr = phys_addr;
                NvU32 sg_len = NV_MIN(dma_len, dma_max_seg_size);

                if (sg == NULL)
                {
                    goto unmap_pfns;
                }

                if (!priv->skip_iommu)
                {
                    if (priv->nv->coherent)
                    {
                        status = nv_dma_map_non_pci_peer(&peer_dma_dev,
                                                         (sg_len >> PAGE_SHIFT),
                                                         &dma_addr);
                    }
                    else
                    {
                        status = nv_dma_map_peer(&peer_dma_dev, priv->nv->dma_dev, 0x1,
                                                 (sg_len >> PAGE_SHIFT), &dma_addr);
                    }
                    if (status != NV_OK)
                    {
                        goto unmap_pfns;
                    }
                }

                sg_set_page(sg, NULL, sg_len, 0);
                sg_dma_address(sg) = (dma_addr_t) dma_addr;
                sg_dma_len(sg) = sg_len;
                phys_addr += sg_len;
                dma_len -= sg_len;
                mapped_nents++;
                sg = sg_next(sg);
            }
        }
    }

    WARN_ON(sg != NULL);

    sgt->nents = mapped_nents;

    WARN_ON(sgt->nents != sgt->orig_nents);

    return sgt;

unmap_pfns:
    sgt->nents = mapped_nents;

    nv_dma_buf_unmap_pfns(dev, sgt, priv);

    sg_free_table(sgt);

free_sgt:
    NV_KFREE(sgt, sizeof(struct sg_table));

    return NULL;
}

static int
nv_dma_buf_attach(
    struct dma_buf *buf,
#if defined(NV_DMA_BUF_OPS_ATTACH_ARG2_DEV)
    struct device *dev,
#endif
    struct dma_buf_attachment *attachment
)
{
    int rc = 0;
    nv_dma_buf_file_private_t *priv = buf->priv;

    mutex_lock(&priv->lock);

    if (priv->mapping_type == NV_DMABUF_EXPORT_MAPPING_TYPE_FORCE_PCIE)
    {
        if(!nv_pci_is_valid_topology_for_direct_pci(priv->nv,
                                                    to_pci_dev(attachment->dev)))
        {
            nv_printf(NV_DBG_ERRORS,
                      "NVRM: dma-buf attach failed: "
                      "topology not supported for mapping type FORCE_PCIE\n");
            rc = -ENOTSUPP;
            goto unlock_priv;
        }

        priv->skip_iommu = NV_TRUE;
    }
    else
    {
        nv_dma_device_t peer_dma_dev = {{ 0 }};

        peer_dma_dev.dev = &to_pci_dev(attachment->dev)->dev;
        peer_dma_dev.addressable_range.limit = to_pci_dev(attachment->dev)->dma_mask;

        if (!nv_grdma_pci_topology_supported(priv->nv, &peer_dma_dev))
        {
            nv_printf(NV_DBG_ERRORS,
                      "NVRM: dma-buf attach failed: "
                      "PCI topology not supported for dma-buf\n");
            rc = -ENOTSUPP;
            goto unlock_priv;
        }
    }

#if defined(NV_DMA_BUF_ATTACHMENT_HAS_PEER2PEER)
    if ((attachment->importer_ops != NULL) &&
        (!attachment->peer2peer) &&
        (!priv->nv->mem_has_struct_page))
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: dma-buf attach failed: "
                  "importer unable to handle MMIO without struct page\n");
        rc = -ENOTSUPP;
        goto unlock_priv;
    }
#endif

unlock_priv:
    mutex_unlock(&priv->lock);

    return rc;
}

static struct sg_table*
nv_dma_buf_map(
    struct dma_buf_attachment *attachment,
    enum dma_data_direction direction
)
{
    NV_STATUS status;
    struct sg_table *sgt = NULL;
    struct dma_buf *buf = attachment->dmabuf;
    nv_dma_buf_file_private_t *priv = buf->priv;

    mutex_lock(&priv->lock);

    if (priv->num_objects != priv->total_objects)
    {
        goto unlock_priv;
    }

    if (!priv->static_phys_addrs)
    {
        status = nv_dma_buf_get_phys_addresses(priv, 0, priv->num_objects);
        if (status != NV_OK)
        {
            goto unlock_priv;
        }
    }

    //
    // For MAPPING_TYPE_FORCE_PCIE on coherent platforms,
    // get the BAR1 PFN scatterlist instead of C2C pages.
    //
    if (priv->nv->mem_has_struct_page &&
        (priv->mapping_type == NV_DMABUF_EXPORT_MAPPING_TYPE_DEFAULT))
    {
        sgt = nv_dma_buf_map_pages(attachment->dev, priv);
    }
    else
    {
        sgt = nv_dma_buf_map_pfns(attachment->dev, priv);
    }
    if (sgt == NULL)
    {
        goto unmap_handles;
    }

    mutex_unlock(&priv->lock);

    return sgt;

unmap_handles:
    if (!priv->static_phys_addrs)
    {
        nv_dma_buf_put_phys_addresses(priv, 0, priv->num_objects);
    }

unlock_priv:
    mutex_unlock(&priv->lock);

    return NULL;
}

static void
nv_dma_buf_unmap(
    struct dma_buf_attachment *attachment,
    struct sg_table *sgt,
    enum dma_data_direction direction
)
{
    struct dma_buf *buf = attachment->dmabuf;
    nv_dma_buf_file_private_t *priv = buf->priv;

    mutex_lock(&priv->lock);

    if (priv->nv->mem_has_struct_page &&
        (priv->mapping_type == NV_DMABUF_EXPORT_MAPPING_TYPE_DEFAULT))
    {
        nv_dma_buf_unmap_pages(attachment->dev, sgt, priv);
    }
    else
    {
        nv_dma_buf_unmap_pfns(attachment->dev, sgt, priv);
    }

    //
    // For static_phys_addrs platforms, this operation is done in release
    // since getting the phys_addrs was done in create/reuse.
    //
    if (!priv->static_phys_addrs)
    {
        nv_dma_buf_put_phys_addresses(priv, 0, priv->num_objects);
    }

    sg_free_table(sgt);

    NV_KFREE(sgt, sizeof(struct sg_table));

    mutex_unlock(&priv->lock);
}

static void
nv_dma_buf_release(
    struct dma_buf *buf
)
{
    int rc = 0;
    NvU32 i;
    nvidia_stack_t *sp = NULL;
    nv_dma_buf_file_private_t *priv = buf->priv;
    nv_state_t *nv;

    if (priv == NULL)
    {
        return;
    }

    nv = priv->nv;

    if (priv->static_phys_addrs)
    {
        nv_dma_buf_put_phys_addresses(priv, 0, priv->num_objects);
    }

    rc = nv_kmem_cache_alloc_stack(&sp);
    if (WARN_ON(rc != 0))
    {
        return;
    }

    // phys_addr refcounts must be zero at this point
    for (i = 0; i < priv->num_objects; i++)
    {
        WARN_ON(priv->handles[i].phys_refcount > 0);
    }

    nv_dma_buf_undup_mem_handles(sp, 0, priv->num_objects, priv);

    rm_dma_buf_put_client_and_device(sp, priv->nv, priv->h_client, priv->h_device,
                                     priv->h_subdevice, priv->mig_info);

    WARN_ON(priv->attached_size > 0);
    WARN_ON(priv->num_objects > 0);

    nv_dma_buf_free_file_private(priv);
    buf->priv = NULL;

    nvidia_dev_put(nv->gpu_id, sp);

    nv_kmem_cache_free_stack(sp);

    return;
}

static int
nv_dma_buf_mmap(
    struct dma_buf *buf,
    struct vm_area_struct *vma
)
{
    int ret = 0;
    NvU32 i = 0;
    nv_dma_buf_file_private_t *priv = buf->priv;
    unsigned long addr = vma->vm_start;
    NvU32 total_skip_size = 0;
    NvU64 total_map_len  = NV_VMA_SIZE(vma);
    NvU64 off_in_range_array = 0;
    NvU32 index;

    if (priv == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: nv_dma_buf_mmap: priv == NULL.\n");
        return -EINVAL;
    }

    mutex_lock(&priv->lock);

    if (!priv->map_attrs.can_mmap)
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: nv_dma_buf_mmap: mmap is not allowed can_mmap[%d] \n",
                  priv->map_attrs.can_mmap);
        ret = -ENOTSUPP;
        goto unlock_priv;
    }

    // Check for offset overflow.
    if ((NV_VMA_OFFSET(vma) + NV_VMA_SIZE(vma)) < NV_VMA_OFFSET(vma))
    {
        ret = -EOVERFLOW;
        goto unlock_priv;
    }

    if ((NV_VMA_OFFSET(vma) + NV_VMA_SIZE(vma)) > priv->total_size)
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: nv_dma_buf_mmap: Vaddr_start[%llx] Vaddr_end[%llx] "
                  "vm_pgoff[%llx] page_offset[%llx] "
                  "page_prot[%x] total_size[%llx] \n",
                  vma->vm_start, vma->vm_end, NV_VMA_PGOFF(vma),
                  NV_VMA_OFFSET(vma), pgprot_val(vma->vm_page_prot),
                  priv->total_size);
        ret = -EINVAL;
        goto unlock_priv;
    }

    nv_printf(NV_DBG_INFO,
              "NVRM: nv_dma_buf_mmap: Vaddr_start[%llx] Vaddr_end[%llx] "
              "os_page_size[%llx] vm_pgoff[%llx] page_offset[%llx] "
              "page_prot[%x] total_size[%llx] total_map_len[%llx] \n",
              vma->vm_start, vma->vm_end, PAGE_SIZE, NV_VMA_PGOFF(vma),
              NV_VMA_OFFSET(vma), pgprot_val(vma->vm_page_prot), priv->total_size,
              total_map_len);

    // Find the first range from which map should start.
    for (i = 0; i < priv->num_objects; i++)
    {
        NvU32 range_count = priv->handles[i].memArea.numRanges;

        for (index = 0; index < range_count; index++)
        {
            NvU64 len = priv->handles[i].memArea.pRanges[index].size;

            total_skip_size += len;
            //
            // Skip memArea.pRanges[index] until to find out the
            // first mapping page start in the memArea range_count.
            // skip pages which lie outside of offset/map length.
            //
            if (NV_VMA_OFFSET(vma) >= total_skip_size)
            {
                continue;
            }
            total_skip_size   -= len;

            //
            // First mapping page start can be anywhere in the specific
            // memArea.pRanges[index]. So adjust off_in_range_array accordingly.
            //
            off_in_range_array = (NV_VMA_OFFSET(vma) - total_skip_size);
            total_skip_size   += off_in_range_array;
            goto found_start_page;
        }
    }

    // Could not find first map page.
    nv_printf(NV_DBG_ERRORS,
              "NVRM: [nv_dma_buf_mmap-failed] Could not find first map page \n");
    ret = -EINVAL;
    goto unlock_priv;

found_start_page:

    // RO and cache type settings
    if (nv_encode_caching(&vma->vm_page_prot,
                          priv->map_attrs.cache_type,
                          priv->map_attrs.memory_type))
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: [nv_dma_buf_mmap-failed] i[%u] cache_type[%llx] memory_type[%d] page_prot[%x] \n",
                  i, priv->map_attrs.cache_type, priv->map_attrs.memory_type, pgprot_val(vma->vm_page_prot));
        ret = -ENXIO;
        goto unlock_priv;
    }

    if (priv->map_attrs.read_only_mem)
    {
        vma->vm_page_prot = NV_PGPROT_READ_ONLY(vma->vm_page_prot);
        nv_vm_flags_clear(vma, VM_WRITE);
        nv_vm_flags_clear(vma, VM_MAYWRITE);
    }

    nv_vm_flags_set(vma, VM_SHARED | VM_DONTEXPAND | VM_DONTDUMP);

    // Create user mapping
    for (; i < (priv->num_objects && (addr < vma->vm_end)); i++)
    {
        NvU32 range_count = priv->handles[i].memArea.numRanges;

        for (; (index < range_count && (addr < vma->vm_end)); index++)
        {
            NvU64 len     = priv->handles[i].memArea.pRanges[index].size;
            NvU64 map_len = 0;
            NvU64 phy_addr;

            phy_addr = (priv->handles[i].memArea.pRanges[index].start + off_in_range_array);
            len     -= off_in_range_array;

            // Reset to 0, after its initial use.
            off_in_range_array = 0;

            map_len = NV_MIN(len, total_map_len);

            //
            // nv_remap_page_range() map a contiguous physical address space
            // into the user virtual space.
            // Use PFN based mapping api to create the mapping for
            // reserved carveout (OS invisible memory, not managed by OS) too.
            // Basically nv_remap_page_range() works for all kind of memory regions.
            // These are the downsides of using nv_remap_page_range()
            // 1. We can't use vm_insert_pages() batching API, so perf overhead to
            //    map every page individually.
            // 2. We can't support use case to call pin_user_pages() on dma-buf's CPU VA.
            // We will revisit this code path in the future if needed.
            //
            ret = nv_remap_page_range(vma, addr, phy_addr, map_len,
                                      vma->vm_page_prot);
            if (ret)
            {
                nv_printf(NV_DBG_ERRORS,
                          "NVRM: nv_dma_buf_mmap: remap_pfn_range - failed\n", ret);
                // Partial mapping is going to be freed by kernel if nv_dma_buf_mmap() fails.
                goto unlock_priv;
            }

            nv_printf(NV_DBG_INFO,
                      "NVRM: nv_dma_buf_mmap: index[%u] range_count[%u] Vaddr[%llx] "
                      "page_prot[%x] phyAddr[%llx] mapLen[%llx] len[%llx] "
                      "total_map_len[%llx] \n",
                      index, range_count, addr, pgprot_val(vma->vm_page_prot), phy_addr,
                      map_len, len, total_map_len);

            total_map_len -= map_len;
            addr          += map_len;
        }
    }

    mutex_unlock(&priv->lock);

    return 0;

unlock_priv:
    mutex_unlock(&priv->lock);

    return ret;
}

#if defined(NV_DMA_BUF_OPS_HAS_MAP)
static void*
nv_dma_buf_map_stub(
    struct dma_buf *buf,
    unsigned long page_num
)
{
    return NULL;
}

static void
nv_dma_buf_unmap_stub(
    struct dma_buf *buf,
    unsigned long page_num,
    void *addr
)
{
    return;
}
#endif

#if defined(NV_DMA_BUF_OPS_HAS_MAP_ATOMIC)
static void*
nv_dma_buf_map_atomic_stub(
    struct dma_buf *buf,
    unsigned long page_num
)
{
    return NULL;
}

static void
nv_dma_buf_unmap_atomic_stub(
    struct dma_buf *buf,
    unsigned long page_num,
    void *addr
)
{
    return;
}
#endif

//
// Note: Some of the dma-buf operations are mandatory in some kernels.
// So stubs are added to prevent dma_buf_export() failure.
// The actual implementations of these interfaces is not really required
// for the export operation to work.
//
static const struct dma_buf_ops nv_dma_buf_ops = {
    .attach        = nv_dma_buf_attach,
    .map_dma_buf   = nv_dma_buf_map,
    .unmap_dma_buf = nv_dma_buf_unmap,
    .release       = nv_dma_buf_release,
    .mmap          = nv_dma_buf_mmap,
#if defined(NV_DMA_BUF_OPS_HAS_MAP)
    .map          = nv_dma_buf_map_stub,
    .unmap        = nv_dma_buf_unmap_stub,
#endif
#if defined(NV_DMA_BUF_OPS_HAS_MAP_ATOMIC)
    .map_atomic   = nv_dma_buf_map_atomic_stub,
    .unmap_atomic = nv_dma_buf_unmap_atomic_stub,
#endif
};

static NV_STATUS
nv_dma_buf_create(
    nv_state_t *nv,
    nv_ioctl_export_to_dma_buf_fd_t *params
)
{
    int rc = 0;
    NV_STATUS status;
    nvidia_stack_t *sp = NULL;
    struct dma_buf *buf = NULL;
    nv_dma_buf_file_private_t *priv = NULL;
    NvU32 gpu_id = nv->gpu_id;

    if (!nv->dma_buf_supported)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (params->index > (params->totalObjects - params->numObjects))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    priv = nv_dma_buf_alloc_file_private(params->totalObjects);
    if (priv == NULL)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to allocate dma-buf private\n");
        return NV_ERR_NO_MEMORY;
    }

    priv->total_objects        = params->totalObjects;
    priv->total_size           = params->totalSize;
    priv->nv                   = nv;
    priv->mapping_type         = params->mappingType;
    priv->skip_iommu           = NV_FALSE;
    priv->map_attrs.allow_mmap = params->bAllowMmap;

    rc = nv_kmem_cache_alloc_stack(&sp);
    if (rc != 0)
    {
        status = NV_ERR_NO_MEMORY;
        goto cleanup_priv;
    }

    rc = nvidia_dev_get(gpu_id, sp);
    if (rc != 0)
    {
        status = NV_ERR_OPERATING_SYSTEM;
        goto cleanup_sp;
    }

    status = rm_dma_buf_get_client_and_device(sp, priv->nv,
                                              params->hClient,
                                              params->handles[0],
                                              priv->mapping_type,
                                              &priv->h_client,
                                              &priv->h_device,
                                              &priv->h_subdevice,
                                              &priv->mig_info,
                                              &priv->static_phys_addrs,
                                              &priv->acquire_release_all_gpu_lock_on_dup);
    if (status != NV_OK)
    {
        goto cleanup_device;
    }

    status = nv_dma_buf_dup_mem_handles(sp, priv, params);
    if (status != NV_OK)
    {
        goto cleanup_client_and_device;
    }

    if (priv->map_attrs.allow_mmap &&
        !priv->map_attrs.can_mmap)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: mmap is not allowed for the specific handles\n");
        status = NV_ERR_NOT_SUPPORTED;
        goto cleanup_handles;
    }

    // User can enable mmap for testing/specific use cases and not for any all handles.
    if (!priv->map_attrs.allow_mmap)
    {
        priv->map_attrs.can_mmap = NV_FALSE;
    }

    // Get CPU static phys addresses if possible to do so at this time.
    if (priv->static_phys_addrs)
    {
        status = nv_dma_buf_get_phys_addresses(priv, params->index,
                                               params->numObjects);
        if (status != NV_OK)
        {
            goto cleanup_handles;
        }
    }

    {
        DEFINE_DMA_BUF_EXPORT_INFO(exp_info);

        exp_info.ops      = &nv_dma_buf_ops;
        exp_info.size     = params->totalSize;
        exp_info.flags    = O_RDWR | O_CLOEXEC;
        exp_info.priv     = priv;
        exp_info.exp_name = "nv_dmabuf";

        buf = dma_buf_export(&exp_info);
    }

    if (IS_ERR(buf))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to create dma-buf\n");

        status = NV_ERR_OPERATING_SYSTEM;

        goto put_phys_addrs;
    }

    nv_kmem_cache_free_stack(sp);

    rc = dma_buf_fd(buf, O_RDWR | O_CLOEXEC);
    if (rc < 0)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to get dma-buf file descriptor\n");

        //
        // If dma-buf is successfully created, the dup'd handles
        // clean-up should be done by the release callback.
        //
        dma_buf_put(buf);

        return NV_ERR_OPERATING_SYSTEM;
    }

    params->fd = rc;

    return NV_OK;

put_phys_addrs:
    if (priv->static_phys_addrs)
    {
        nv_dma_buf_put_phys_addresses(priv, params->index, params->numObjects);
    }

cleanup_handles:
    nv_dma_buf_undup_mem_handles(sp, params->index, params->numObjects, priv);

cleanup_client_and_device:
    rm_dma_buf_put_client_and_device(sp, priv->nv, priv->h_client, priv->h_device,
                                     priv->h_subdevice, priv->mig_info);

cleanup_device:
    nvidia_dev_put(gpu_id, sp);

cleanup_sp:
    nv_kmem_cache_free_stack(sp);

cleanup_priv:
    nv_dma_buf_free_file_private(priv);

    return status;
}

static NV_STATUS
nv_dma_buf_reuse(
    nv_state_t *nv,
    nv_ioctl_export_to_dma_buf_fd_t *params
)
{
    int rc = 0;
    NV_STATUS status = NV_OK;
    nvidia_stack_t *sp = NULL;
    struct dma_buf *buf = NULL;
    nv_dma_buf_file_private_t *priv = NULL;

    buf = dma_buf_get(params->fd);
    if (IS_ERR(buf))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to get dma-buf\n");
        return NV_ERR_OPERATING_SYSTEM;
    }

    if (buf->ops != &nv_dma_buf_ops)
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: Invalid dma-buf fd\n");
        status = NV_ERR_INVALID_ARGUMENT;
        goto cleanup_dmabuf;
    }

    priv = buf->priv;

    if (priv == NULL)
    {
        status = NV_ERR_OPERATING_SYSTEM;
        goto cleanup_dmabuf;
    }

    rc = mutex_lock_interruptible(&priv->lock);
    if (rc != 0)
    {
        status = NV_ERR_OPERATING_SYSTEM;
        goto cleanup_dmabuf;
    }

    if ((priv->total_objects < params->numObjects) ||
        (params->index > (priv->total_objects - params->numObjects)) ||
        (params->mappingType != priv->mapping_type) ||
        (params->bAllowMmap  != priv->map_attrs.allow_mmap))
    {
        status = NV_ERR_INVALID_ARGUMENT;
        goto unlock_priv;
    }

    rc = nv_kmem_cache_alloc_stack(&sp);
    if (rc != 0)
    {
        status = NV_ERR_NO_MEMORY;
        goto unlock_priv;
    }

    status = nv_dma_buf_dup_mem_handles(sp, priv, params);
    if (status != NV_OK)
    {
        goto cleanup_sp;
    }

    // Get CPU static phys addresses if possible to do so at this time.
    if (priv->static_phys_addrs)
    {
        status = nv_dma_buf_get_phys_addresses(priv, params->index,
                                               params->numObjects);
        if (status != NV_OK)
        {
            goto cleanup_handles;
        }
    }

    nv_kmem_cache_free_stack(sp);

    mutex_unlock(&priv->lock);

    dma_buf_put(buf);

    return NV_OK;

cleanup_handles:
    nv_dma_buf_undup_mem_handles(sp, params->index, params->numObjects, priv);

cleanup_sp:
    nv_kmem_cache_free_stack(sp);

unlock_priv:
    mutex_unlock(&priv->lock);

cleanup_dmabuf:
    dma_buf_put(buf);

    return status;
}
#endif // CONFIG_DMA_SHARED_BUFFER

NV_STATUS
nv_dma_buf_export(
    nv_state_t *nv,
    nv_ioctl_export_to_dma_buf_fd_t *params
)
{
#if defined(CONFIG_DMA_SHARED_BUFFER)
    NV_STATUS status;

    if ((params == NULL) ||
        (params->totalSize == 0) ||
        (params->numObjects == 0) ||
        (params->totalObjects == 0) ||
        (params->numObjects > NV_DMABUF_EXPORT_MAX_HANDLES) ||
        (params->numObjects > params->totalObjects))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if ((params->mappingType != NV_DMABUF_EXPORT_MAPPING_TYPE_DEFAULT) &&
        (params->mappingType != NV_DMABUF_EXPORT_MAPPING_TYPE_FORCE_PCIE))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // If fd >= 0, dma-buf already exists with this fd, so get dma-buf from fd.
    // If fd == -1, dma-buf is not created yet, so create it and then store
    // additional handles.
    //
    if (params->fd == -1)
    {
        status = nv_dma_buf_create(nv, params);
    }
    else if (params->fd >= 0)
    {
        status = nv_dma_buf_reuse(nv, params);
    }
    else
    {
        status = NV_ERR_INVALID_ARGUMENT;
    }

    return status;
#else
    return NV_ERR_NOT_SUPPORTED;
#endif // CONFIG_DMA_SHARED_BUFFER
}

NV_STATUS NV_API_CALL nv_dma_import_dma_buf
(
    nv_dma_device_t *dma_dev,
    struct dma_buf *dma_buf,
    NvBool is_ro_device_map,
    NvU32 *size,
    struct sg_table **sgt,
    nv_dma_buf_t **import_priv
)
{
#if defined(CONFIG_DMA_SHARED_BUFFER)
    nv_dma_buf_t *nv_dma_buf = NULL;
    struct dma_buf_attachment *dma_attach = NULL;
    struct sg_table *map_sgt = NULL;
    NV_STATUS status = NV_OK;

    if ((dma_dev == NULL) ||
        (dma_buf == NULL) ||
        (size == NULL) ||
        (sgt  == NULL) ||
        (import_priv == NULL))
    {
        nv_printf(NV_DBG_ERRORS, "Import arguments are NULL!\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = os_alloc_mem((void **)&nv_dma_buf, sizeof(*nv_dma_buf));
    if (status != NV_OK)
    {
        nv_printf(NV_DBG_ERRORS, "Can't allocate mem for nv_buf!\n");
        return status;
    }

    get_dma_buf(dma_buf);

    dma_attach = dma_buf_attach(dma_buf, dma_dev->dev);
    if (IS_ERR_OR_NULL(dma_attach))
    {
        nv_printf(NV_DBG_ERRORS, "Can't attach dma_buf!\n");
        status = NV_ERR_OPERATING_SYSTEM;

        goto dma_buf_attach_fail;
    }

    if (is_ro_device_map)
    {
        // Try RO only dma mapping.
        nv_dma_buf->direction = DMA_TO_DEVICE;
        nv_printf(NV_DBG_INFO,
                  "NVRM: nv_dma_import_dma_buf -Try RO [DMA_TO_DEVICE] only mapping \n");
    }
    else
    {
        nv_dma_buf->direction = DMA_BIDIRECTIONAL;
    }

    map_sgt = dma_buf_map_attachment(dma_attach, nv_dma_buf->direction);
    if (IS_ERR_OR_NULL(map_sgt))
    {
        nv_printf(NV_DBG_ERRORS, "Can't map dma attachment!\n");
        status = NV_ERR_OPERATING_SYSTEM;

        goto dma_buf_map_fail;
    }

    nv_dma_buf->dma_buf = dma_buf;
    nv_dma_buf->dma_attach = dma_attach;
    nv_dma_buf->sgt = map_sgt;

    *size = dma_buf->size;
    *import_priv = nv_dma_buf;
    *sgt = map_sgt;

    return NV_OK;

dma_buf_map_fail:
    dma_buf_detach(dma_buf, dma_attach);
dma_buf_attach_fail:
    os_free_mem(nv_dma_buf);
    dma_buf_put(dma_buf);

    return status;
#else
    return NV_ERR_NOT_SUPPORTED;
#endif // CONFIG_DMA_SHARED_BUFFER
}

NV_STATUS NV_API_CALL nv_dma_import_from_fd
(
    nv_dma_device_t *dma_dev,
    NvS32 fd,
    NvBool is_ro_device_map,
    NvU32 *size,
    struct sg_table **sgt,
    nv_dma_buf_t **import_priv
)
{
#if defined(CONFIG_DMA_SHARED_BUFFER)
    struct dma_buf *dma_buf = dma_buf_get(fd);
    NV_STATUS status;

    if (IS_ERR_OR_NULL(dma_buf))
    {
        nv_printf(NV_DBG_ERRORS, "Can't get dma_buf from fd!\n");
        return NV_ERR_OPERATING_SYSTEM;
    }

    status = nv_dma_import_dma_buf(dma_dev,
                                   dma_buf, is_ro_device_map, size,
                                   sgt, import_priv);
    dma_buf_put(dma_buf);

    return status;
#else
    return NV_ERR_NOT_SUPPORTED;
#endif // CONFIG_DMA_SHARED_BUFFER
}

void NV_API_CALL nv_dma_release_dma_buf
(
    nv_dma_buf_t *import_priv
)
{
#if defined(CONFIG_DMA_SHARED_BUFFER)
    nv_dma_buf_t *nv_dma_buf = NULL;

    if (import_priv == NULL)
    {
        return;
    }

    nv_dma_buf = (nv_dma_buf_t *)import_priv;
    dma_buf_unmap_attachment(nv_dma_buf->dma_attach, nv_dma_buf->sgt,
                             nv_dma_buf->direction);
    dma_buf_detach(nv_dma_buf->dma_buf, nv_dma_buf->dma_attach);
    dma_buf_put(nv_dma_buf->dma_buf);

    os_free_mem(nv_dma_buf);
#endif // CONFIG_DMA_SHARED_BUFFER
}

