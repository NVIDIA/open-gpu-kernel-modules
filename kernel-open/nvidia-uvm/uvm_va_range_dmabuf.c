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

#include "uvm_linux.h"
#include "uvm_common.h"
#include "uvm_api.h"
#include "uvm_forward_decl.h"
#include "uvm_va_range_dmabuf.h"
#include "uvm_gpu.h"
#include "uvm_hal.h"
#include "uvm_hal_types.h"
#include "uvm_lock.h"
#include "uvm_processors.h"
#include "uvm_tracker.h"
#include "uvm_va_space.h"
#include "uvm_va_range.h"
#include "uvm_va_space.h"
#include "uvm_ioctl.h"

#if UVM_PROVIDES_DMA_BUF_IMPORTER()

#include <linux/dma-buf.h>
#include <linux/dma-resv.h>

// Mutable iterator data state passed to uvm_dma_buf_pte_maker()
typedef struct {
    uvm_gpu_t *mapping_gpu;
    struct sg_dma_page_iter dma_iter;
} uvm_dma_buf_pte_maker_data_t;

static void uvm_dma_buf_gpu_map_destroy(uvm_va_range_dma_buf_t *dmabuf_range,
                                        uvm_dma_buf_gpu_map_t *dmabuf_map);

static NV_STATUS uvm_create_dma_buf_range(uvm_va_space_t *va_space,
                                          struct dma_buf *dmabuf,
                                          UVM_IMPORT_DMA_BUF_PARAMS *params)
{
    uvm_va_range_dma_buf_t *dmabuf_range = NULL;
    struct mm_struct *mm;
    NV_STATUS status = NV_OK;

    // The DMA-BUF interface provides a sg_table of DMA mapped regions through
    // dma_buf_map_attachment(), with base addresses and lengths required to be
    // aligned to the system page size. Assert the same here.
    if (uvm_api_range_invalid(params->base, params->length))
        return NV_ERR_INVALID_ADDRESS;

    // Require that the provided length for the va_range matches the full size
    // of the DMA-BUF.
    if (params->length != dmabuf->size)
        return NV_ERR_INVALID_ADDRESS;

    // The mm needs to be locked in order to remove stale HMM va_blocks.
    mm = uvm_va_space_mm_or_current_retain_lock(va_space);
    uvm_va_space_down_write(va_space);

    // Create the new DMA-BUF VA range.
    // uvm_va_range_create_dma_buf handles any collisions when it attempts to
    // insert the new range into the va_space range tree.
    status = uvm_va_range_create_dma_buf(va_space, mm, dmabuf, params->base, dmabuf->size, &dmabuf_range);
    if (status != NV_OK) {
        UVM_DBG_PRINT_RL("Failed to create DMA-BUF VA range [0x%llx, 0x%llx)\n",
                         params->base,
                         params->base + dmabuf->size);
    }

    uvm_va_space_up_write(va_space);
    uvm_va_space_mm_or_current_release_unlock(va_space, mm);
    return status;
}

NV_STATUS uvm_api_import_dma_buf(UVM_IMPORT_DMA_BUF_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    struct dma_buf *dmabuf;
    NV_STATUS status;

    dmabuf = dma_buf_get(params->handleFd);
    if (IS_ERR(dmabuf))
        return NV_ERR_INVALID_ARGUMENT;

    status = uvm_create_dma_buf_range(va_space, dmabuf, params);

    dma_buf_put(dmabuf);
    return status;
}

// Retrieve the per-GPU sub-range mapping object spanning a given virtual
// address or NULL if no such mapping exists.
static uvm_dma_buf_gpu_map_t *uvm_va_range_dma_buf_get_gpu_map_or_null(uvm_va_range_dma_buf_t *dmabuf_range,
                                                                       uvm_gpu_t *mapping_gpu,
                                                                       NvU64 addr)
{
    uvm_dma_buf_gpu_map_t *dmabuf_map = NULL;
    uvm_range_tree_node_t *node;
    uvm_dma_buf_gpu_range_tree_t *range_tree;

    uvm_assert_rwsem_locked(&dmabuf_range->va_range.va_space->lock);

    range_tree = uvm_dma_buf_gpu_range_tree(dmabuf_range, mapping_gpu);

    if (uvm_processor_mask_test(&dmabuf_range->mapped_gpus, mapping_gpu->id)) {
        UVM_ASSERT(!uvm_range_tree_empty(&range_tree->base.tree));
        node = uvm_range_tree_find(&range_tree->base.tree, addr);
        if (node) {
            dmabuf_map = uvm_dma_buf_gpu_map_container(node);
            UVM_ASSERT(dmabuf_map->gpu == mapping_gpu);
        }
    }
    else {
        UVM_ASSERT(uvm_range_tree_empty(&range_tree->base.tree));
    }

    return dmabuf_map;
}

// TODO: Bug 6164216 refactor for code reuse with external mappings
static NV_STATUS uvm_dma_buf_gpu_map_split(uvm_range_tree_t *tree,
                                           uvm_dma_buf_gpu_map_t *existing_map,
                                           NvU64 new_end,
                                           uvm_dma_buf_gpu_map_t **new_map)
{
    uvm_dma_buf_gpu_map_t *new;
    NV_STATUS status;
    NvU64 new_start = new_end + 1;

    if (!IS_ALIGNED(new_start, existing_map->pt_range_vec.page_size))
        return NV_ERR_INVALID_ADDRESS;

    UVM_ASSERT(new_start >= existing_map->node.start && new_start < existing_map->node.end);

    new = uvm_kvmalloc_zero(sizeof(*new));
    if (!new)
        return NV_ERR_NO_MEMORY;

    RB_CLEAR_NODE(&new->node.rb_node);
    new->gpu = existing_map->gpu;

    status = uvm_page_table_range_vec_split_upper(&existing_map->pt_range_vec, new_start - 1, &new->pt_range_vec);
    if (status != NV_OK) {
        uvm_kvfree(new);
        return status;
    }

    new->node.start = new_start;

    uvm_range_tree_split(tree, &existing_map->node, &new->node);

    if (new_map)
        *new_map = new;

    return NV_OK;
}

// See comment in uvm_unmap_external_buf_in_range for description of the
// implementation.
static NV_STATUS uvm_unmap_dma_buf_in_range(uvm_va_range_dma_buf_t *dmabuf_range,
                                            uvm_gpu_t *gpu,
                                            NvU64 start,
                                            NvU64 end)
{
    uvm_dma_buf_gpu_range_tree_t *range_tree = uvm_dma_buf_gpu_range_tree(dmabuf_range, gpu);
    uvm_dma_buf_gpu_map_t *dmabuf_map, *dmabuf_map_next = NULL;
    NV_STATUS status = NV_OK;

    uvm_assert_mutex_locked(&range_tree->base.lock);

    dmabuf_map = uvm_dma_buf_gpu_map_iter_first(dmabuf_range, gpu, start, end);
    while (dmabuf_map) {
        if (start > dmabuf_map->node.start) {
            status = uvm_dma_buf_gpu_map_split(&range_tree->base.tree, dmabuf_map, start - 1, &dmabuf_map_next);
            if (status != NV_OK)
                break;
        }
        else {
            if (end < dmabuf_map->node.end) {
                status = uvm_dma_buf_gpu_map_split(&range_tree->base.tree, dmabuf_map, end, NULL);
                if (status != NV_OK)
                    break;
                dmabuf_map_next = NULL;
            }
            else {
                dmabuf_map_next = uvm_dma_buf_gpu_map_iter_next(dmabuf_range, dmabuf_map, end);
            }

            uvm_dma_buf_gpu_map_destroy(dmabuf_range, dmabuf_map);
        }

        dmabuf_map = dmabuf_map_next;
    }

    return status;
}

static inline void uvm_dma_buf_pte_maker_data_init(uvm_gpu_t *mapping_gpu,
                                                   struct sg_table *sg_table,
                                                   NvU64 start_offset,
                                                   uvm_dma_buf_pte_maker_data_t *out)
{
    memset(out, 0, sizeof(*out));
    out->mapping_gpu = mapping_gpu;

    __sg_page_iter_start(&out->dma_iter.base,
                         sg_table->sgl,
                         sg_table->nents,
                         start_offset >> PAGE_SHIFT);
}

static NvU64 uvm_dma_buf_pte_maker(uvm_page_table_range_vec_t *range_vec,
                                   NvU64 offset,
                                   void *caller_data)
{

    uvm_dma_buf_pte_maker_data_t *data = caller_data;
    dma_addr_t dma_addr;
    NvU64 gpu_addr;

    if (!__sg_page_iter_dma_next(&data->dma_iter)) {
        UVM_ERR_PRINT("DMA-BUF sg_table exhausted before all PTEs were written, GPU %s, offset %llu\n",
                      uvm_gpu_name(data->mapping_gpu),
                      offset);
        return 0;
    }

    dma_addr = uvm_sg_page_iter_dma_address(&data->dma_iter);
    gpu_addr = uvm_parent_gpu_dma_addr_to_gpu_addr(data->mapping_gpu->parent, dma_addr);

    // TODO: Bug 6172651: Revisit unconditional issue of RWA permissions.
    return range_vec->tree->hal->make_pte(UVM_APERTURE_SYS,
                                          gpu_addr,
                                          UVM_PROT_READ_WRITE_ATOMIC,
                                          UVM_MMU_PTE_FLAGS_ACCESS_COUNTERS_DISABLED);
}

// Implementation of the .invalidate_mappings() callback. It is required to
// signal that the memory backing the DMA-BUF is going to move, and so DMA
// mappings must be removed. The following handler is invoked per-GPU prior to
// the exporter revoking accesses to the buffer, and so also signals that
// no new mappings may be established. This callback may be safely issued
// multiple times, though subsequent calls will be no-ops.
static void uvm_dma_buf_invalidate_mappings(struct dma_buf_attachment *attach)
{
    uvm_dma_buf_importer_priv_t *priv = attach->importer_priv;
    uvm_va_space_t *va_space;
    uvm_va_range_dma_buf_t *dmabuf_range;
    uvm_dma_buf_gpu_range_tree_t *range_tree;
    uvm_dma_buf_gpu_map_t *dmabuf_map, *dmabuf_map_next;
    uvm_gpu_t *gpu;

    // The exporter calls this callback with the dma_resv lock held.
    uvm_record_lock_dma_resv();
    dma_resv_assert_held(attach->dmabuf->resv);
    UVM_ASSERT(priv);

    va_space = priv->va_space;
    uvm_va_space_down_read(va_space);

    dmabuf_range = uvm_va_range_dma_buf_find(va_space, priv->base);
    if (!dmabuf_range || dmabuf_range->dmabuf != attach->dmabuf) {
        uvm_va_space_up_read(va_space);
        goto invalidate_no_range;
    }

    // All GMMU mappings will have been destroyed if the GPU is no longer
    // registered but had previously been attached.
    if (!uvm_processor_mask_test(&va_space->registered_gpus, priv->gpu_id)) {
        uvm_va_space_up_read(va_space);
        goto invalidate_no_range;
    }

    gpu = uvm_gpu_get(priv->gpu_id);
    UVM_ASSERT(gpu);

    range_tree = uvm_dma_buf_gpu_range_tree(dmabuf_range, gpu);
    uvm_mutex_lock(&range_tree->base.lock);

    UVM_ASSERT(range_tree->attach == attach);

    if (uvm_processor_mask_test(&dmabuf_range->mapped_gpus, gpu->id)) {
        // Revoke GMMU mappings before calling dma_buf_unmap_attachment(), which is
        // required of the .invalidate_mappings() callback.
        uvm_dma_buf_gpu_map_for_each_safe(dmabuf_map, dmabuf_map_next, dmabuf_range, gpu)
            uvm_dma_buf_gpu_map_destroy(dmabuf_range, dmabuf_map);

        uvm_processor_mask_clear_atomic(&dmabuf_range->mapped_gpus, gpu->id);
    }

    uvm_processor_mask_set_atomic(&dmabuf_range->revoked_gpus, gpu->id);

    uvm_mutex_unlock(&range_tree->base.lock);
    uvm_va_space_up_read(va_space);

invalidate_no_range:

    // Since the invalidate_mappings() callback is exporter-initiated, it may
    // be called even after UVM has ceased to track or accept mappings for this
    // GPU. In such an instance, the importer is still obligated to call
    // dma_buf_unmap_attachment().
    if (priv->sg_table) {
        dma_buf_unmap_attachment(attach, priv->sg_table, DMA_BIDIRECTIONAL);
        priv->sg_table = NULL;
    }

    uvm_record_unlock_dma_resv();
}

static const struct dma_buf_attach_ops uvm_dma_buf_importer_ops = {
    .allow_peer2peer = true,
#if defined(NV_DMA_BUF_ATTACH_OPS_HAS_INVALIDATE_MAPPINGS)
    .invalidate_mappings = uvm_dma_buf_invalidate_mappings,
#else
    .move_notify = uvm_dma_buf_invalidate_mappings,
#endif
};

static NV_STATUS uvm_map_dma_buf_on_gpu(uvm_va_range_dma_buf_t *dmabuf_range,
                                        uvm_gpu_t *mapping_gpu,
                                        NvU64 base,
                                        NvU64 length,
                                        struct dma_buf_attachment *new_attach,
                                        bool *attachment_consumed)
{
    uvm_va_space_t *va_space = dmabuf_range->va_range.va_space;
    uvm_dma_buf_gpu_map_t *dmabuf_map = NULL;
    uvm_dma_buf_gpu_range_tree_t *range_tree = uvm_dma_buf_gpu_range_tree(dmabuf_range, mapping_gpu);
    uvm_gpu_va_space_t *gpu_va_space = uvm_gpu_va_space_get(va_space, mapping_gpu);
    uvm_dma_buf_importer_priv_t *attach_priv;
    uvm_page_tree_t *page_tree;
    uvm_page_table_range_vec_t *pt_range_vec = NULL;
    uvm_dma_buf_pte_maker_data_t pte_maker_data;
    bool previous_mapping;
    NV_STATUS status;

    uvm_assert_rwsem_locked_read(&va_space->lock);

    uvm_mutex_lock(&range_tree->base.lock);
    if (uvm_processor_mask_test(&dmabuf_range->revoked_gpus, mapping_gpu->id)) {
        uvm_mutex_unlock(&range_tree->base.lock);
        return NV_ERR_INVALID_ADDRESS;
    }

    if (!range_tree->attach) {
        range_tree->attach = new_attach;
        *attachment_consumed = true;
    }

    attach_priv = range_tree->attach->importer_priv;

    status = uvm_unmap_dma_buf_in_range(dmabuf_range, mapping_gpu, base, base + length - 1);
    if (status != NV_OK)
        goto error;

    // Create the dma mappings if this is the first mapping on this GPU.
    UVM_ASSERT(attach_priv->sg_table || !uvm_processor_mask_test(&dmabuf_range->mapped_gpus, mapping_gpu->id));
    if (!attach_priv->sg_table) {
        struct sg_table *sg_table;

        dma_resv_assert_held(dmabuf_range->dmabuf->resv);

        sg_table = dma_buf_map_attachment(range_tree->attach,
                                          DMA_BIDIRECTIONAL);
        if (IS_ERR(sg_table)) {
            status = (PTR_ERR(sg_table) == -ENOMEM) ? NV_ERR_NO_MEMORY
                                                    : NV_ERR_INVALID_DEVICE;
            goto error;
        }
        attach_priv->sg_table = sg_table;
    }

    // Now create the GMMU mappings for the requested sub-region, after all
    // per-GPU initialisation steps have completed.
    dmabuf_map = uvm_kvmalloc_zero(sizeof(*dmabuf_map));
    if (!dmabuf_map) {
        status = NV_ERR_NO_MEMORY;
        goto error;
    }

    dmabuf_map->node.start = base;
    dmabuf_map->node.end = base + length - 1;
    RB_CLEAR_NODE(&dmabuf_map->node.rb_node);

    // Due to the fact that any overlapping mappings were already unmapped,
    // adding the new mapping to the tree cannot fail.
    status = uvm_range_tree_add(&range_tree->base.tree, &dmabuf_map->node);
    UVM_ASSERT(status == NV_OK);

    previous_mapping = uvm_processor_mask_test(&dmabuf_range->mapped_gpus, mapping_gpu->id);
    uvm_processor_mask_set_atomic(&dmabuf_range->mapped_gpus, mapping_gpu->id);

    dmabuf_map->gpu = mapping_gpu;
    UVM_ASSERT(uvm_va_range_dma_buf_get_gpu_map_or_null(dmabuf_range, mapping_gpu, base) == dmabuf_map);

    UVM_ASSERT(gpu_va_space);
    page_tree = &gpu_va_space->page_tables;
    pt_range_vec = &dmabuf_map->pt_range_vec;

    // TODO: Bug 6164231: Currently mappings are of uniform size as the system
    // page size. This is due to the fact that the sg_tables returned from
    // dma_buf_map_attachment() are only guaranteed to have system page size
    // alignment. To better suit larger DMA-BUF memories, larger page sizes
    // should be chosen wherever possible.
    status = uvm_page_table_range_vec_init(page_tree,
                                           dmabuf_map->node.start,
                                           uvm_range_tree_node_size(&dmabuf_map->node),
                                           PAGE_SIZE,
                                           UVM_PMM_ALLOC_FLAGS_EVICT,
                                           pt_range_vec);
    if (status != NV_OK) {
        if (!previous_mapping)
            uvm_processor_mask_clear_atomic(&dmabuf_range->mapped_gpus, mapping_gpu->id);
        goto error;
    }

    uvm_dma_buf_pte_maker_data_init(mapping_gpu,
                                    attach_priv->sg_table,
                                    base - dmabuf_range->va_range.node.start,
                                    &pte_maker_data);

    status = uvm_page_table_range_vec_write_ptes(pt_range_vec,
                                                 UVM_MEMBAR_SYS,
                                                 uvm_dma_buf_pte_maker,
                                                 (void *) &pte_maker_data);

    if (status != NV_OK) {
        if (!previous_mapping)
            uvm_processor_mask_clear_atomic(&dmabuf_range->mapped_gpus, mapping_gpu->id);
        goto error;
    }

    uvm_mutex_unlock(&range_tree->base.lock);
    return NV_OK;

error:
    uvm_dma_buf_gpu_map_destroy(dmabuf_range, dmabuf_map);
    uvm_mutex_unlock(&range_tree->base.lock);
    return status;
}

NV_STATUS uvm_api_map_dma_buf(UVM_MAP_DMA_BUF_PARAMS *params, struct file *filp)
{
    uvm_gpu_t *mapping_gpu;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_va_range_dma_buf_t *dmabuf_range;
    struct dma_buf *dmabuf;
    struct dma_buf_attachment *attach;
    uvm_dma_buf_importer_priv_t *importer_priv;
    bool attachment_consumed = false;
    NV_STATUS status;

    if (uvm_api_range_invalid(params->base, params->length))
        return NV_ERR_INVALID_ADDRESS;

    // Phase 1: This may be the first attempt to map to this GPU, and so it is
    // unknown whether this GPU has an attachment. Acquire the read lock on the
    // va_space and retain the GPU and dmabuf as they will be needed to attempt
    // to attach the GPU.
    uvm_va_space_down_read(va_space);

    dmabuf_range = uvm_va_range_dma_buf_find(va_space, params->base);
    if (!dmabuf_range ||
        dmabuf_range->va_range.node.end < params->base + params->length - 1) {
        uvm_va_space_up_read(va_space);
        return NV_ERR_INVALID_ADDRESS;
    }

    mapping_gpu = uvm_va_space_get_gpu_by_uuid_with_gpu_va_space(va_space, &params->gpuUuid);
    if (!mapping_gpu) {
        uvm_va_space_up_read(va_space);
        return NV_ERR_INVALID_DEVICE;
    }

    if (!uvm_gpu_can_address(mapping_gpu, params->base, params->length)) {
        uvm_va_space_up_read(va_space);
        return NV_ERR_OUT_OF_RANGE;
    }

    dmabuf = dmabuf_range->dmabuf;
    get_dma_buf(dmabuf);
    uvm_gpu_retain(mapping_gpu);
    uvm_va_space_up_read(va_space);

    // Phase 2: Creating the attachment will require taking the DMA-BUF's
    // dma_resv lock. This cannot be safely acquired with the va_space lock
    // held, as dma_buf_invalidate_mappings() is called by the DMA-BUF interface
    // with the dma_resv lock, and must itself acquire the va_space lock for
    // read. Eagerly create an attachment, in case this is the first mapping
    // on this GPU.
    importer_priv = uvm_kvmalloc(sizeof(*importer_priv));
    if (!importer_priv) {
        status = NV_ERR_NO_MEMORY;
        goto release;
    }

    importer_priv->gpu_id = mapping_gpu->id;
    importer_priv->sg_table = NULL;
    importer_priv->va_space = va_space;
    importer_priv->base = params->base;

    attach = uvm_dma_buf_attach(dmabuf,
                                &mapping_gpu->parent->pci_dev->dev,
                                &uvm_dma_buf_importer_ops,
                                importer_priv);
    if (IS_ERR(attach)) {
        uvm_kvfree(importer_priv);
        status = (PTR_ERR(attach) == -ENOMEM) ? NV_ERR_NO_MEMORY
                                              : NV_ERR_INVALID_DEVICE;
        goto release;
    }

    // Phase 3: Re-acquire the va_space with read and revalidate - if the
    // DMA-BUF or GPU is no longer in use in this va_space then remove the
    // attachment. Acquire DMA-BUF lock first so that we dma mappings can be
    // established if necessary with dma_buf_map_attachment().
    uvm_dma_resv_lock(dmabuf->resv);
    uvm_va_space_down_read(va_space);

    dmabuf_range = uvm_va_range_dma_buf_find(va_space, params->base);
    if (!dmabuf_range ||
        dmabuf_range->va_range.node.end < params->base + params->length - 1 ||
        dmabuf_range->dmabuf != dmabuf) {
        status = NV_ERR_INVALID_ADDRESS;
        goto unlock_detach;
    }

    if (!uvm_processor_mask_test(&va_space->registered_gpus, mapping_gpu->id)) {
        status = NV_ERR_INVALID_DEVICE;
        goto unlock_detach;
    }

    status = uvm_map_dma_buf_on_gpu(dmabuf_range,
                                    mapping_gpu,
                                    params->base,
                                    params->length,
                                    attach,
                                    &attachment_consumed);

unlock_detach:
    uvm_va_space_up_read(va_space);
    uvm_dma_resv_unlock(dmabuf->resv);

    // On failure, or if otherwise the mapping_gpu was attached by a previous
    // mapping, drop the new attachment after releasing all locks. Attachments
    // are otherwise freed at UvmFree().
    if (!attachment_consumed) {
        uvm_dma_buf_detach(dmabuf, attach);
        uvm_kvfree(importer_priv);
    }

release:
    uvm_gpu_release(mapping_gpu);
    dma_buf_put(dmabuf);
    return status;
}

static void uvm_dma_buf_gpu_map_destroy(uvm_va_range_dma_buf_t *dmabuf_range,
                                        uvm_dma_buf_gpu_map_t *dmabuf_map)
{
    NV_STATUS status;
    uvm_dma_buf_gpu_range_tree_t *range_tree;

    if (!dmabuf_map)
        return;

    UVM_ASSERT(uvm_gpu_va_space_get(dmabuf_range->va_range.va_space, dmabuf_map->gpu));
    range_tree = uvm_dma_buf_gpu_range_tree(dmabuf_range, dmabuf_map->gpu);

    // The DMA-BUF map is inserted into the tree prior to the rest of the
    // mapping steps. So, if it has not been inserted yet, there is nothing to
    // clean up. Just free the memory.
    if (RB_EMPTY_NODE(&dmabuf_map->node.rb_node)) {
        uvm_kvfree(dmabuf_map);
        return;
    }

    uvm_assert_mutex_locked(&range_tree->base.lock);
    uvm_range_tree_remove(&range_tree->base.tree, &dmabuf_map->node);

    // Unmap the PTEs
    if (dmabuf_map->pt_range_vec.ranges) {
        status = uvm_page_table_range_vec_clear_ptes(&dmabuf_map->pt_range_vec, UVM_MEMBAR_SYS);
        if (status != NV_OK)
            UVM_ERR_PRINT("Clearing PTEs failed: %s, GPU %s\n",
                          nvstatusToString(status),
                          uvm_gpu_name(dmabuf_map->gpu));
        UVM_ASSERT(status == NV_OK);

        uvm_page_table_range_vec_deinit(&dmabuf_map->pt_range_vec);
    }

    UVM_ASSERT(!dmabuf_map->pt_range_vec.ranges);
    uvm_kvfree(dmabuf_map);
}

void uvm_dma_buf_gpu_range_tree_deinit(uvm_va_range_dma_buf_t *dmabuf_range,
                                       uvm_gpu_t *gpu,
                                       struct list_head *deferred_free_list)
{
    uvm_dma_buf_gpu_range_tree_t *range_tree = uvm_dma_buf_gpu_range_tree(dmabuf_range, gpu);
    uvm_dma_buf_gpu_map_t *dmabuf_map, *dmabuf_map_next;
    struct dma_buf_attachment *attach = NULL;
    uvm_dma_buf_importer_priv_t *importer_priv;
    uvm_dma_buf_attach_deferred_t *deferred;

    uvm_mutex_lock(&range_tree->base.lock);
    if (uvm_processor_mask_test(&dmabuf_range->mapped_gpus, gpu->id)) {
        uvm_dma_buf_gpu_map_for_each_safe(dmabuf_map, dmabuf_map_next, dmabuf_range, gpu)
            uvm_dma_buf_gpu_map_destroy(dmabuf_range, dmabuf_map);
        uvm_processor_mask_clear_atomic(&dmabuf_range->mapped_gpus, gpu->id);
    }

    // If this GPU ever received mappings, defer detachment to the DMA-BUF until
    // after the va_space lock is released.
    if (range_tree->attach) {
        attach = range_tree->attach;
        range_tree->attach = NULL;

        UVM_ASSERT(deferred_free_list);
    }
    uvm_mutex_unlock(&range_tree->base.lock);

    if (!attach)
        return;

    importer_priv = attach->importer_priv;
    deferred = &importer_priv->deferred;

    // Defer close of the DMA-BUF until the attachment is torn down following
    // release of the va_space lock.
    get_dma_buf(dmabuf_range->dmabuf);

    deferred->dmabuf = dmabuf_range->dmabuf;
    deferred->attach = attach;
    deferred->gpu = gpu;

    uvm_deferred_free_object_add(deferred_free_list,
                                 &deferred->deferred_free,
                                 UVM_DEFERRED_FREE_OBJECT_TYPE_DMA_BUF_ATTACHMENT);
}

void uvm_va_range_dma_buf_attach_deferred_free(uvm_dma_buf_attach_deferred_t *deferred)
{
    uvm_dma_buf_importer_priv_t *importer_priv = deferred->attach->importer_priv;

    // Under the dma_resv lock, check that this is the first time that
    // dma_buf_unmap_attachment() has been called.
    uvm_dma_resv_lock(deferred->dmabuf->resv);
    if (importer_priv->sg_table) {
        dma_buf_unmap_attachment(deferred->attach, importer_priv->sg_table, DMA_BIDIRECTIONAL);
        importer_priv->sg_table = NULL;
    }
    uvm_dma_resv_unlock(deferred->dmabuf->resv);

    uvm_dma_buf_detach(deferred->dmabuf, deferred->attach);

    dma_buf_put(deferred->dmabuf);
    uvm_kvfree(importer_priv);
}

NV_STATUS uvm_api_unmap_dma_buf(UVM_UNMAP_DMA_BUF_PARAMS *params, struct file *filp)
{
    uvm_gpu_t *gpu;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_va_range_dma_buf_t *dmabuf_range;
    uvm_dma_buf_gpu_range_tree_t *range_tree;
    NV_STATUS status = NV_OK;

    // The DMA-BUF interface provides a sg_table of DMA mapped regions through
    // dma_buf_map_attachment(), with base addresses and lengths required to be
    // aligned to the system page size. Assert the same here.
    if (uvm_api_range_invalid(params->base, params->length))
        return NV_ERR_INVALID_ADDRESS;

    uvm_va_space_down_read(va_space);

    dmabuf_range = uvm_va_range_dma_buf_find(va_space, params->base);
    if (!dmabuf_range ||
        dmabuf_range->va_range.node.end < params->base + params->length - 1) {
        uvm_va_space_up_read(va_space);
        return NV_ERR_INVALID_ADDRESS;
    }

    gpu = uvm_va_space_get_gpu_by_uuid(va_space, &params->gpuUuid);
    if (!gpu) {
        uvm_va_space_up_read(va_space);
        return NV_ERR_INVALID_DEVICE;
    }

    range_tree = uvm_dma_buf_gpu_range_tree(dmabuf_range, gpu);

    uvm_mutex_lock(&range_tree->base.lock);
    status = uvm_unmap_dma_buf_in_range(dmabuf_range, gpu, params->base, params->base + params->length - 1);
    uvm_mutex_unlock(&range_tree->base.lock);

    uvm_va_space_up_read(va_space);
    return status;
}

#else

NV_STATUS uvm_api_import_dma_buf(UVM_IMPORT_DMA_BUF_PARAMS *params, struct file *filp)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS uvm_api_map_dma_buf(UVM_MAP_DMA_BUF_PARAMS *params, struct file *filp)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS uvm_api_unmap_dma_buf(UVM_UNMAP_DMA_BUF_PARAMS *params, struct file *filp)
{
    return NV_ERR_NOT_SUPPORTED;
}

void uvm_dma_buf_gpu_range_tree_deinit(uvm_va_range_dma_buf_t *dmabuf_range,
                                       uvm_gpu_t *gpu,
                                       struct list_head *deferred_free_list) {}

void uvm_va_range_dma_buf_attach_deferred_free(uvm_dma_buf_attach_deferred_t *deferred) {}

#endif // UVM_PROVIDES_DMA_BUF_IMPORTER()

NV_STATUS uvm_api_is_dma_buf_import_supported(UVM_IS_DMA_BUF_IMPORT_SUPPORTED_PARAMS *params, struct file *filp)
{
    params->dmaBufImportSupported = UVM_PROVIDES_DMA_BUF_IMPORTER();

    return NV_OK;
}

/*
 * MODULE_IMPORT_NS() is added by commit id 8651ec01daeda
 * ("module: add support for symbol namespaces") in 5.4
 */
#if defined(MODULE_IMPORT_NS)
/*
 * DMA_BUF namespace is added by commit id 16b0314aa746
 * ("dma-buf: move dma-buf symbols into the DMA_BUF module namespace") in 5.16
 */
#if defined(NV_MODULE_IMPORT_NS_TAKES_CONSTANT)
MODULE_IMPORT_NS(DMA_BUF);
#else
MODULE_IMPORT_NS("DMA_BUF");
#endif  // defined(NV_MODULE_IMPORT_NS_TAKES_CONSTANT)
#endif  // defined(MODULE_IMPORT_NS)
