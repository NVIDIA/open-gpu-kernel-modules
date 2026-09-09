/*******************************************************************************
    Copyright (c) 2015-2026 NVIDIA Corporation

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
#include "nv-kref.h"
#include "uvm_test.h"
#include "uvm_test_ioctl.h"
#include "uvm_kvmalloc.h"

#if UVM_PROVIDES_DMA_BUF_IMPORTER()

#include <linux/dma-buf.h>
#include <linux/dma-mapping.h>
#include <linux/dma-resv.h>
#include <linux/err.h>
#include <linux/mm.h>
#include <linux/scatterlist.h>
#include <linux/slab.h>

// The struct dma_buf doesn't tracking the backing store.
typedef struct {
    struct page **pages;
    unsigned long num_pages;
    size_t size;
} dma_buf_priv_t;

// The dma_buf_attach() and dma_buf_map_attachment() return but don't
// store the scatter gather table of DMA mappings.
typedef struct {
    struct sg_table *sg_table;
    enum dma_data_direction direction;
} attachment_priv_t;

static int sysmem_attach(struct dma_buf *dmabuf,
                         struct dma_buf_attachment *attach)
{
    attachment_priv_t *attch_priv;

    attch_priv = uvm_kvmalloc_zero(sizeof(*attch_priv));
    if (!attch_priv)
        return -ENOMEM;

    attach->priv = attch_priv;
    return 0;
}

static void sysmem_detach(struct dma_buf *dmabuf,
                          struct dma_buf_attachment *attach)
{
    uvm_kvfree(attach->priv);
    attach->priv = NULL;
}

static struct sg_table *sysmem_map(struct dma_buf_attachment *attach,
                                   enum dma_data_direction direction)
{
    dma_buf_priv_t *dmabuf_priv = attach->dmabuf->priv;
    attachment_priv_t *attach_priv = attach->priv;
    struct sg_table *sg_table;
    int ret;

    sg_table = uvm_kvmalloc_zero(sizeof(*sg_table));
    if (!sg_table)
        return ERR_PTR(-ENOMEM);

    // Merges adjacent pages into fewer sg entries where possible.
    ret = sg_alloc_table_from_pages(sg_table,
                                    dmabuf_priv->pages,
                                    dmabuf_priv->num_pages,
                                    0,
                                    dmabuf_priv->size,
                                    GFP_KERNEL);
    if (ret) {
        uvm_kvfree(sg_table);
        return ERR_PTR(ret);
    }

    ret = dma_map_sgtable(attach->dev,
                          sg_table,
                          direction,
                          0);
    if (ret) {
        sg_free_table(sg_table);
        uvm_kvfree(sg_table);
        return ERR_PTR(ret);
    }

    attach_priv->sg_table = sg_table;
    attach_priv->direction = direction;
    return sg_table;
}

static void sysmem_unmap(struct dma_buf_attachment *attach,
                         struct sg_table *sg_table,
                         enum dma_data_direction direction)
{
    attachment_priv_t *attach_priv = attach->priv;

    attach_priv->sg_table = NULL;
    dma_unmap_sgtable(attach->dev, sg_table, direction, 0);
    sg_free_table(sg_table);
    uvm_kvfree(sg_table);
}

static int sysmem_mmap(struct dma_buf *dmabuf,
                       struct vm_area_struct *vma)
{
    dma_buf_priv_t *dmabuf_priv = dmabuf->priv;
    int i, ret;

    if (vma_pages(vma) > dmabuf_priv->num_pages)
        return -EINVAL;

    for (i = 0; i < vma_pages(vma); i++) {
        ret = vm_insert_page(vma, vma->vm_start + i * PAGE_SIZE,
                             dmabuf_priv->pages[i]);
        if (ret)
            return ret;
    }

    return 0;
}

static int sysmem_begin_cpu_access(struct dma_buf *dmabuf,
                                   enum dma_data_direction direction)
{
    struct dma_buf_attachment *attach;

    dma_resv_lock(dmabuf->resv, NULL);
    list_for_each_entry(attach, &dmabuf->attachments, node) {
        attachment_priv_t *attach_priv = attach->priv;

        if (attach_priv->sg_table)
            dma_sync_sgtable_for_cpu(attach->dev, attach_priv->sg_table, direction);
    }
    dma_resv_unlock(dmabuf->resv);
    return 0;
}

static int sysmem_end_cpu_access(struct dma_buf *dmabuf,
                                 enum dma_data_direction direction)
{
    struct dma_buf_attachment *attach;

    dma_resv_lock(dmabuf->resv, NULL);
    list_for_each_entry(attach, &dmabuf->attachments, node) {
        attachment_priv_t *attach_priv = attach->priv;

        if (attach_priv->sg_table)
            dma_sync_sgtable_for_device(attach->dev, attach_priv->sg_table, direction);
    }
    dma_resv_unlock(dmabuf->resv);
    return 0;
}

static void sysmem_release(struct dma_buf *dmabuf)
{
    int i;
    dma_buf_priv_t *dmabuf_priv = dmabuf->priv;

    if (!dmabuf_priv->pages) {
        uvm_kvfree(dmabuf_priv);
        return;
    }

    for (i = 0; i < dmabuf_priv->num_pages; i++)
        __free_page(dmabuf_priv->pages[i]);

    uvm_kvfree(dmabuf_priv->pages);
    dmabuf_priv->pages = NULL;
    dmabuf_priv->num_pages = 0;

    uvm_kvfree(dmabuf_priv);
}

static const struct dma_buf_ops uvm_sysmem_ops = {
    .attach           = sysmem_attach,
    .detach           = sysmem_detach,
    .map_dma_buf      = sysmem_map,
    .unmap_dma_buf    = sysmem_unmap,
    .mmap             = sysmem_mmap,
    .begin_cpu_access = sysmem_begin_cpu_access,
    .end_cpu_access   = sysmem_end_cpu_access,
    .release          = sysmem_release,
};

NV_STATUS uvm_test_export_dma_buf(UVM_TEST_EXPORT_DMA_BUF_PARAMS *params,
                                  struct file *filp)
{
    DEFINE_DMA_BUF_EXPORT_INFO(exp_info);
    dma_buf_priv_t *dmabuf_priv;
    struct dma_buf *dmabuf;
    unsigned long i;
    int fd;

    if (!params->size || !IS_ALIGNED(params->size, PAGE_SIZE))
        return NV_ERR_INVALID_ARGUMENT;

    dmabuf_priv = uvm_kvmalloc_zero(sizeof(*dmabuf_priv));
    if (!dmabuf_priv)
        return NV_ERR_NO_MEMORY;

    dmabuf_priv->size      = params->size;
    dmabuf_priv->num_pages = params->size >> PAGE_SHIFT;

    dmabuf_priv->pages = uvm_kvmalloc_zero(dmabuf_priv->num_pages * sizeof(*dmabuf_priv->pages));
    if (!dmabuf_priv->pages) {
        uvm_kvfree(dmabuf_priv);
        return NV_ERR_NO_MEMORY;
    }

    for (i = 0; i < dmabuf_priv->num_pages; i++) {
        dmabuf_priv->pages[i] = alloc_page(GFP_KERNEL | __GFP_ZERO);
        if (!dmabuf_priv->pages[i]) {
            while (i--)
                __free_page(dmabuf_priv->pages[i]);
            uvm_kvfree(dmabuf_priv->pages);
            uvm_kvfree(dmabuf_priv);
            return NV_ERR_NO_MEMORY;
        }
    }

    exp_info.ops   = &uvm_sysmem_ops;
    exp_info.size  = dmabuf_priv->size;
    exp_info.flags = O_RDWR | O_CLOEXEC;
    exp_info.priv  = dmabuf_priv;

    dmabuf = dma_buf_export(&exp_info);
    if (IS_ERR(dmabuf)) {
        UVM_ERR_PRINT("dma_buf_export failed: %ld\n", PTR_ERR(dmabuf));
        for (i = 0; i < dmabuf_priv->num_pages; i++)
            __free_page(dmabuf_priv->pages[i]);
        uvm_kvfree(dmabuf_priv->pages);
        uvm_kvfree(dmabuf_priv);
        return NV_ERR_OPERATING_SYSTEM;
    }

    fd = dma_buf_fd(dmabuf, O_CLOEXEC);
    if (fd < 0) {
        UVM_ERR_PRINT("dma_buf_fd failed: %d\n", fd);
        dma_buf_put(dmabuf);
        return NV_ERR_OPERATING_SYSTEM;
    }

    params->fd = fd;
    return NV_OK;
}

NV_STATUS uvm_test_revoke_dma_buf(UVM_TEST_REVOKE_DMA_BUF_PARAMS *params,
                                  struct file *filp)
{
    struct dma_buf *dmabuf;

    dmabuf = dma_buf_get(params->fd);
    if (IS_ERR(dmabuf))
        return NV_ERR_INVALID_ARGUMENT;

    if (dmabuf->ops != &uvm_sysmem_ops) {
        dma_buf_put(dmabuf);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Notify importers that the DMA-BUF is being revoked. The callback,
    // invalidate_mappings() is expected to synchronously provide a new mapping,
    // but in UVM's use case all GMMU mappings will be withdrawn and any
    // further access should produce a fatal fault.
    dma_resv_lock(dmabuf->resv, NULL);
#if defined(NV_DMA_BUF_ATTACH_OPS_HAS_INVALIDATE_MAPPINGS)
    dma_buf_invalidate_mappings(dmabuf);
#else
    dma_buf_move_notify(dmabuf);
#endif
    dma_resv_unlock(dmabuf->resv);

    dma_buf_put(dmabuf);
    return NV_OK;
}

#else

NV_STATUS uvm_test_export_dma_buf(UVM_TEST_EXPORT_DMA_BUF_PARAMS *params,
                                  struct file *filp)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS uvm_test_revoke_dma_buf(UVM_TEST_REVOKE_DMA_BUF_PARAMS *params,
                                  struct file *filp)
{
    return NV_ERR_NOT_SUPPORTED;
}

#endif // UVM_PROVIDES_DMA_BUF_IMPORTER()
