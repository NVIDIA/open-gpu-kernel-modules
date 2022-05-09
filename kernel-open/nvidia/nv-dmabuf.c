/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
    NvHandle h_memory;
    NvU64    offset;
    NvU64    size;
    NvU64    bar1_va;
} nv_dma_buf_mem_handle_t;

typedef struct nv_dma_buf_file_private
{
    nv_state_t              *nv;
    NvHandle                 h_client;
    NvHandle                 h_device;
    NvHandle                 h_subdevice;
    NvU32                    total_objects;
    NvU32                    num_objects;
    NvU64                    total_size;
    NvU64                    attached_size;
    struct mutex             lock;
    nv_dma_buf_mem_handle_t *handles;
    NvU64                    bar1_va_ref_count;
    void                    *mig_info;
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
        NV_KFREE(priv->handles, priv->total_objects * sizeof(priv->handles[0]));
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

    NV_KMALLOC(priv, sizeof(nv_dma_buf_file_private_t));
    if (priv == NULL)
    {
        return NULL;
    }

    memset(priv, 0, sizeof(nv_dma_buf_file_private_t));

    mutex_init(&priv->lock);

    NV_KMALLOC(priv->handles, num_handles * sizeof(priv->handles[0]));
    if (priv->handles == NULL)
    {
        goto failed;
    }

    memset(priv->handles, 0, num_handles * sizeof(priv->handles[0]));

    return priv;

failed:
    nv_dma_buf_free_file_private(priv);

    return NULL;
}

// Must be called with RMAPI lock and GPU lock taken
static void
nv_dma_buf_undup_mem_handles_unlocked(
    nvidia_stack_t            *sp,
    NvU32                      index,
    NvU32                      num_objects,
    nv_dma_buf_file_private_t *priv
)
{
    NvU32 i = 0;

    for (i = index; i < num_objects; i++)
    {
        if (priv->handles[i].h_memory == 0)
        {
            continue;
        }

        rm_dma_buf_undup_mem_handle(sp, priv->nv, priv->h_client,
                                    priv->handles[i].h_memory);

        priv->attached_size -= priv->handles[i].size;
        priv->handles[i].h_memory = 0;
        priv->handles[i].offset = 0;
        priv->handles[i].size = 0;
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

    status = rm_acquire_gpu_lock(sp, priv->nv);
    if (status != NV_OK)
    {
        goto unlock_api_lock;
    }

    for (i = 0; i < params->numObjects; i++)
    {
        NvHandle h_memory_duped = 0;

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
                                           &h_memory_duped);
        if (status != NV_OK)
        {
            goto failed;
        }

        priv->attached_size += params->sizes[i];
        priv->handles[index].h_memory = h_memory_duped;
        priv->handles[index].offset = params->offsets[i];
        priv->handles[index].size = params->sizes[i];
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

    rm_release_gpu_lock(sp, priv->nv);

    rm_release_api_lock(sp);

    return NV_OK;

failed:
    nv_dma_buf_undup_mem_handles_unlocked(sp, params->index, count, priv);

    rm_release_gpu_lock(sp, priv->nv);

unlock_api_lock:
    rm_release_api_lock(sp);

    return status;
}

// Must be called with RMAPI lock and GPU lock taken
static void
nv_dma_buf_unmap_unlocked(
    nvidia_stack_t *sp,
    nv_dma_device_t *peer_dma_dev,
    nv_dma_buf_file_private_t *priv,
    struct sg_table *sgt,
    NvU32 count
)
{
    NV_STATUS status;
    NvU32 i;
    NvU64 dma_len;
    NvU64 dma_addr;
    NvU64 bar1_va;
    NvBool bar1_unmap_needed;
    struct scatterlist *sg = NULL;

    bar1_unmap_needed = (priv->bar1_va_ref_count == 0);

    for_each_sg(sgt->sgl, sg, count, i)
    {
        dma_addr = sg_dma_address(sg);
        dma_len  = priv->handles[i].size;
        bar1_va  = priv->handles[i].bar1_va;

        WARN_ON(sg_dma_len(sg) != priv->handles[i].size);

        nv_dma_unmap_peer(peer_dma_dev, (dma_len / os_page_size), dma_addr);

        if (bar1_unmap_needed)
        {
            status = rm_dma_buf_unmap_mem_handle(sp, priv->nv, priv->h_client,
                                                 priv->handles[i].h_memory,
                                                 priv->handles[i].size,
                                                 priv->handles[i].bar1_va);
            WARN_ON(status != NV_OK);
        }
    }
}

static struct sg_table*
nv_dma_buf_map(
    struct dma_buf_attachment *attachment,
    enum dma_data_direction direction
)
{
    NV_STATUS status;
    nvidia_stack_t *sp = NULL;
    struct scatterlist *sg = NULL;
    struct sg_table *sgt = NULL;
    struct dma_buf *buf = attachment->dmabuf;
    struct device *dev = attachment->dev;
    nv_dma_buf_file_private_t *priv = buf->priv;
    nv_dma_device_t peer_dma_dev = {{ 0 }};
    NvBool bar1_map_needed;
    NvBool bar1_unmap_needed;
    NvU32 count = 0;
    NvU32 i = 0;
    int rc = 0;

    //
    // We support importers that are able to handle MMIO resources
    // not backed by struct page. This will need to be revisited
    // when dma-buf support for P9 will be added.
    //
#if defined(NV_DMA_BUF_HAS_DYNAMIC_ATTACHMENT) && \
    defined(NV_DMA_BUF_ATTACHMENT_HAS_PEER2PEER)
    if (dma_buf_attachment_is_dynamic(attachment) &&
        !attachment->peer2peer)
    {
        nv_printf(NV_DBG_ERRORS,
                  "NVRM: failed to map dynamic attachment with no P2P support\n");
        return NULL;
    }
#endif

    mutex_lock(&priv->lock);

    if (priv->num_objects != priv->total_objects)
    {
        goto unlock_priv;
    }

    rc = nv_kmem_cache_alloc_stack(&sp);
    if (rc != 0)
    {
        goto unlock_priv;
    }

    status = rm_acquire_api_lock(sp);
    if (status != NV_OK)
    {
        goto free_sp;
    }

    status = rm_acquire_gpu_lock(sp, priv->nv);
    if (status != NV_OK)
    {
        goto unlock_api_lock;
    }

    NV_KMALLOC(sgt, sizeof(struct sg_table));
    if (sgt == NULL)
    {
        goto unlock_gpu_lock;
    }

    memset(sgt, 0, sizeof(struct sg_table));

    //
    // RM currently returns contiguous BAR1, so we create as many
    // sg entries as the number of handles being mapped.
    // When RM can alloc discontiguous BAR1, this code will need to be revisited.
    //
    rc = sg_alloc_table(sgt, priv->num_objects, GFP_KERNEL);
    if (rc != 0)
    {
        goto free_sgt;
    }

    peer_dma_dev.dev = dev;
    peer_dma_dev.addressable_range.limit = (NvU64)dev->dma_mask;
    bar1_map_needed = bar1_unmap_needed = (priv->bar1_va_ref_count == 0);

    for_each_sg(sgt->sgl, sg, priv->num_objects, i)
    {
        NvU64 dma_addr;
        NvU64 dma_len;

        if (bar1_map_needed)
        {
            status = rm_dma_buf_map_mem_handle(sp, priv->nv, priv->h_client,
                                               priv->handles[i].h_memory,
                                               priv->handles[i].offset,
                                               priv->handles[i].size,
                                               &priv->handles[i].bar1_va);
            if (status != NV_OK)
            {
                goto unmap_handles;
            }
        }

        dma_addr = priv->handles[i].bar1_va;
        dma_len  = priv->handles[i].size;

        status = nv_dma_map_peer(&peer_dma_dev, priv->nv->dma_dev,
                                 0x1, (dma_len / os_page_size), &dma_addr);
        if (status != NV_OK)
        {
            if (bar1_unmap_needed)
            {
                // Unmap the recently mapped memory handle
                (void) rm_dma_buf_unmap_mem_handle(sp, priv->nv, priv->h_client,
                                                   priv->handles[i].h_memory,
                                                   priv->handles[i].size,
                                                   priv->handles[i].bar1_va);
            }

            // Unmap remaining memory handles
            goto unmap_handles;
        }

        sg_set_page(sg, NULL, dma_len, 0);
        sg_dma_address(sg) = (dma_addr_t)dma_addr;
        sg_dma_len(sg) = dma_len;
        count++;
    }

    priv->bar1_va_ref_count++;

    rm_release_gpu_lock(sp, priv->nv);

    rm_release_api_lock(sp);

    nv_kmem_cache_free_stack(sp);

    mutex_unlock(&priv->lock);

    return sgt;

unmap_handles:
    nv_dma_buf_unmap_unlocked(sp, &peer_dma_dev, priv, sgt, count);

    sg_free_table(sgt);

free_sgt:
    NV_KFREE(sgt, sizeof(struct sg_table));

unlock_gpu_lock:
    rm_release_gpu_lock(sp, priv->nv);

unlock_api_lock:
    rm_release_api_lock(sp);

free_sp:
    nv_kmem_cache_free_stack(sp);

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
    NV_STATUS status;
    struct dma_buf *buf = attachment->dmabuf;
    struct device *dev = attachment->dev;
    nvidia_stack_t *sp = NULL;
    nv_dma_buf_file_private_t *priv = buf->priv;
    nv_dma_device_t peer_dma_dev = {{ 0 }};
    int rc = 0;

    mutex_lock(&priv->lock);

    if (priv->num_objects != priv->total_objects)
    {
        goto unlock_priv;
    }

    rc = nv_kmem_cache_alloc_stack(&sp);
    if (WARN_ON(rc != 0))
    {
        goto unlock_priv;
    }

    status = rm_acquire_api_lock(sp);
    if (WARN_ON(status != NV_OK))
    {
        goto free_sp;
    }

    status = rm_acquire_gpu_lock(sp, priv->nv);
    if (WARN_ON(status != NV_OK))
    {
        goto unlock_api_lock;
    }

    peer_dma_dev.dev = dev;
    peer_dma_dev.addressable_range.limit = (NvU64)dev->dma_mask;

    priv->bar1_va_ref_count--;

    nv_dma_buf_unmap_unlocked(sp, &peer_dma_dev, priv, sgt, priv->num_objects);

    sg_free_table(sgt);

    NV_KFREE(sgt, sizeof(struct sg_table));

    rm_release_gpu_lock(sp, priv->nv);

unlock_api_lock:
    rm_release_api_lock(sp);

free_sp:
    nv_kmem_cache_free_stack(sp);

unlock_priv:
    mutex_unlock(&priv->lock);
}

static void
nv_dma_buf_release(
    struct dma_buf *buf
)
{
    int rc = 0;
    nvidia_stack_t *sp = NULL;
    nv_dma_buf_file_private_t *priv = buf->priv;
    nv_state_t *nv;

    if (priv == NULL)
    {
        return;
    }

    nv = priv->nv;

    rc = nv_kmem_cache_alloc_stack(&sp);
    if (WARN_ON(rc != 0))
    {
        return;
    }

    nv_dma_buf_undup_mem_handles(sp, 0, priv->num_objects, priv);

    rm_dma_buf_put_client_and_device(sp, priv->nv, priv->h_client, priv->h_device,
                                     priv->h_subdevice, priv->mig_info);

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
    return -ENOTSUPP;
}

#if defined(NV_DMA_BUF_OPS_HAS_KMAP) || \
    defined(NV_DMA_BUF_OPS_HAS_MAP)
static void*
nv_dma_buf_kmap_stub(
    struct dma_buf *buf,
    unsigned long page_num
)
{
    return NULL;
}

static void
nv_dma_buf_kunmap_stub(
    struct dma_buf *buf,
    unsigned long page_num,
    void *addr
)
{
    return;
}
#endif

#if defined(NV_DMA_BUF_OPS_HAS_KMAP_ATOMIC) || \
    defined(NV_DMA_BUF_OPS_HAS_MAP_ATOMIC)
static void*
nv_dma_buf_kmap_atomic_stub(
    struct dma_buf *buf,
    unsigned long page_num
)
{
    return NULL;
}

static void
nv_dma_buf_kunmap_atomic_stub(
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
// Same functions are used for kmap*/map* because of this commit:
// f9b67f0014cb: dma-buf: Rename dma-ops to prevent conflict with kunmap_atomic
//
static const struct dma_buf_ops nv_dma_buf_ops = {
    .map_dma_buf   = nv_dma_buf_map,
    .unmap_dma_buf = nv_dma_buf_unmap,
    .release       = nv_dma_buf_release,
    .mmap          = nv_dma_buf_mmap,
#if defined(NV_DMA_BUF_OPS_HAS_KMAP)
    .kmap          = nv_dma_buf_kmap_stub,
    .kunmap        = nv_dma_buf_kunmap_stub,
#endif
#if defined(NV_DMA_BUF_OPS_HAS_KMAP_ATOMIC)
    .kmap_atomic   = nv_dma_buf_kmap_atomic_stub,
    .kunmap_atomic = nv_dma_buf_kunmap_atomic_stub,
#endif
#if defined(NV_DMA_BUF_OPS_HAS_MAP)
    .map          = nv_dma_buf_kmap_stub,
    .unmap        = nv_dma_buf_kunmap_stub,
#endif
#if defined(NV_DMA_BUF_OPS_HAS_MAP_ATOMIC)
    .map_atomic   = nv_dma_buf_kmap_atomic_stub,
    .unmap_atomic = nv_dma_buf_kunmap_atomic_stub,
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

    priv->total_objects = params->totalObjects;
    priv->total_size    = params->totalSize;
    priv->nv            = nv;

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
                                              &priv->h_client,
                                              &priv->h_device,
                                              &priv->h_subdevice,
                                              &priv->mig_info);
    if (status != NV_OK)
    {
        goto cleanup_device;
    }

    status = nv_dma_buf_dup_mem_handles(sp, priv, params);
    if (status != NV_OK)
    {
        goto cleanup_client_and_device;
    }

#if (NV_DMA_BUF_EXPORT_ARGUMENT_COUNT == 1)
    {
        DEFINE_DMA_BUF_EXPORT_INFO(exp_info);

        exp_info.ops   = &nv_dma_buf_ops;
        exp_info.size  = params->totalSize;
        exp_info.flags = O_RDWR | O_CLOEXEC;
        exp_info.priv  = priv;

        buf = dma_buf_export(&exp_info);
    }
#elif (NV_DMA_BUF_EXPORT_ARGUMENT_COUNT == 4)
    buf = dma_buf_export(priv, &nv_dma_buf_ops,
                         params->totalSize, O_RDWR | O_CLOEXEC);
#elif (NV_DMA_BUF_EXPORT_ARGUMENT_COUNT == 5)
    buf = dma_buf_export(priv, &nv_dma_buf_ops,
                         params->totalSize, O_RDWR | O_CLOEXEC, NULL);
#endif

    if (IS_ERR(buf))
    {
        nv_printf(NV_DBG_ERRORS, "NVRM: failed to create dma-buf\n");

        status = NV_ERR_OPERATING_SYSTEM;

        goto cleanup_handles;
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

cleanup_handles:
    nv_dma_buf_undup_mem_handles(sp, 0, priv->num_objects, priv);

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

    if (params->index > (priv->total_objects - params->numObjects))
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













