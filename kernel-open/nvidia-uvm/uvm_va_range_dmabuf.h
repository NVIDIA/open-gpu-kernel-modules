/*******************************************************************************
    Copyright (c) 2026 NVIDIA Corporation

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

//
//     uvm_va_range_dmabuf.h
//
//     This file contains the UVM implementation of the DMA-BUF kernel interface.
//

#ifndef __UVM_VA_RANGE_DMABUF_H__
#define __UVM_VA_RANGE_DMABUF_H__

#include "uvm_linux.h"
#include "uvm_forward_decl.h"
#include "uvm_va_range.h"
#include "uvm_tracker.h"
#include "nv_uvm_types.h"
#include "uvm_types.h"

#if UVM_PROVIDES_DMA_BUF_IMPORTER()
#include <linux/dma-buf.h>
#include <linux/device.h>
#endif

static uvm_dma_buf_gpu_range_tree_t *uvm_dma_buf_gpu_range_tree(uvm_va_range_dma_buf_t *dmabuf_range,
                                                                uvm_gpu_t *gpu)
{
    return &dmabuf_range->gpu_ranges[uvm_id_gpu_index(gpu->id)];
}

// The caller must hold the range tree lock.
static uvm_dma_buf_gpu_map_t *uvm_dma_buf_gpu_map_iter_first(uvm_va_range_dma_buf_t *dmabuf_range,
                                                             uvm_gpu_t *gpu,
                                                             NvU64 start,
                                                             NvU64 end)
{
    UVM_ASSERT(start >= dmabuf_range->va_range.node.start);
    UVM_ASSERT(end <= dmabuf_range->va_range.node.end);
    return uvm_dma_buf_gpu_map_container(
        uvm_gpu_range_tree_iter_first(&uvm_dma_buf_gpu_range_tree(dmabuf_range, gpu)->base, start, end));
}

// The caller must hold the range tree lock.
static uvm_dma_buf_gpu_map_t *uvm_dma_buf_gpu_map_iter_next(uvm_va_range_dma_buf_t *dmabuf_range,
                                                            uvm_dma_buf_gpu_map_t *dmabuf_gpu_map,
                                                            NvU64 end)
{
    if (!dmabuf_gpu_map)
        return NULL;
    UVM_ASSERT(end <= dmabuf_range->va_range.node.end);
    return uvm_dma_buf_gpu_map_container(
        uvm_gpu_range_tree_iter_next(&uvm_dma_buf_gpu_range_tree(dmabuf_range, dmabuf_gpu_map->gpu)->base,
                                     dmabuf_gpu_map, end));
}

// The four iterators below require that the caller hold the gpu's range tree
// lock.
#define uvm_dma_buf_gpu_map_for_each_in(dmabuf_gpu_map, dmabuf_range, gpu, start, end)          \
    uvm_gpu_map_for_each_in(dmabuf_gpu_map, dmabuf_range, gpu, start, end,                      \
                            uvm_dma_buf_gpu_map_iter_first, uvm_dma_buf_gpu_map_iter_next)

#define uvm_dma_buf_gpu_map_for_each_in_safe(dmabuf_gpu_map, dmabuf_gpu_map_next, dmabuf_range, gpu, start, end)    \
    uvm_gpu_map_for_each_in_safe(dmabuf_gpu_map, dmabuf_gpu_map_next, dmabuf_range, gpu, start, end,                \
                                 uvm_dma_buf_gpu_map_iter_first, uvm_dma_buf_gpu_map_iter_next)

#define uvm_dma_buf_gpu_map_for_each(dmabuf_gpu_map, dmabuf_range, gpu)     \
    uvm_dma_buf_gpu_map_for_each_in(dmabuf_gpu_map, dmabuf_range, gpu,      \
                                    (dmabuf_range)->va_range.node.start,    \
                                    (dmabuf_range)->va_range.node.end)

#define uvm_dma_buf_gpu_map_for_each_safe(dmabuf_gpu_map, dmabuf_gpu_map_next, dmabuf_range, gpu)   \
    uvm_dma_buf_gpu_map_for_each_in_safe(dmabuf_gpu_map, dmabuf_gpu_map_next, dmabuf_range, gpu,    \
                                         (dmabuf_range)->va_range.node.start,                       \
                                         (dmabuf_range)->va_range.node.end)

// The release of the attachment must occur after the va_space lock has been
// released at tear-down. This deferred_free object holds the retained gpu as
// well as a reference to the dmabuf which will be put on free.
typedef struct
{
    struct dma_buf *dmabuf;
    struct dma_buf_attachment *attach;
    uvm_gpu_t *gpu;

    uvm_deferred_free_object_t deferred_free;
} uvm_dma_buf_attach_deferred_t;

// Per attachment (per-GPU, per DMA-BUF import) state. Permits late unmap of
// DMA-BUF mappings. Available for use through deferred-free through the
// attachment.
typedef struct
{
    // Reference to the attachment GPU, which is retained until the attachment
    // is destroyed.
    uvm_gpu_id_t gpu_id;

    // Reference to the dma mappings retrieved through dma_buf_map_attachment(),
    // To be revoked either at deferred-free or through the importer
    // .invalidate_mappings callback in keeping with the DMA-BUF API.
    // If this pointer is observed to not be NULL under the dma_resv lock, it
    // can be revoked.
    struct sg_table *sg_table;

    // The va_space and base address of the first mapping are sufficient to
    // identify the DMA-BUF range under the va_space lock. The importer's
    // attachments to the DMA-BUF can outlive its va_range, in which case the
    // sg_table will be torn down directly, as no GMMU mappings will exist.
    uvm_va_space_t *va_space;
    NvU64 base;

    uvm_dma_buf_attach_deferred_t deferred;
} uvm_dma_buf_importer_priv_t;

// Destroy all GPU mapping associated with the VA range. Defer destruction of
// DMA mappings through call to dma_buf_unmap_attachment(), until after the
// va_space lock is dropped.
// LOCKING: The va_space lock must be held in at least read mode.
void uvm_dma_buf_gpu_range_tree_deinit(uvm_va_range_dma_buf_t *dmabuf_range,
                                       uvm_gpu_t *gpu,
                                       struct list_head *deferred_free_list);

// Run a deferred dma_buf_detach() and free the wrapper. Invoked by the
// deferred-free dispatcher in uvm_va_space.c.
void uvm_va_range_dma_buf_attach_deferred_free(uvm_dma_buf_attach_deferred_t *deferred);

#if UVM_PROVIDES_DMA_BUF_IMPORTER()
static inline struct dma_buf_attachment *uvm_dma_buf_attach(struct dma_buf *dmabuf,
                                                            struct device *dev,
                                                            const struct dma_buf_attach_ops *ops,
                                                            void *importer_priv)
{
    struct dma_buf_attachment *ret;

    uvm_record_lock_dma_resv();
    ret = dma_buf_dynamic_attach(dmabuf, dev, ops, importer_priv);
    uvm_record_unlock_dma_resv();

    return ret;
}

static inline void uvm_dma_buf_detach(struct dma_buf *dmabuf,
                                      struct dma_buf_attachment *attach)
{
    uvm_record_lock_dma_resv();
    dma_buf_detach(dmabuf, attach);
    uvm_record_unlock_dma_resv();
}
#endif // UVM_PROVIDES_DMA_BUF_IMPORTER()

#endif // __UVM_VA_RANGE_DMABUF_H__
