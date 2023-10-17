/*******************************************************************************
    Copyright (c) 2021 NVIDIA Corporation

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

#include "uvm_common.h"
#include "uvm_global.h"
#include "uvm_conf_computing.h"
#include "uvm_kvmalloc.h"
#include "uvm_gpu.h"
#include "uvm_hal.h"
#include "uvm_mem.h"
#include "uvm_processors.h"
#include "uvm_tracker.h"
#include "nv_uvm_interface.h"
#include "uvm_va_block.h"


static UvmGpuConfComputeMode uvm_conf_computing_get_mode(const uvm_parent_gpu_t *parent)
{
    return parent->rm_info.gpuConfComputeCaps.mode;
}

bool uvm_conf_computing_mode_enabled_parent(const uvm_parent_gpu_t *parent)
{
    return uvm_conf_computing_get_mode(parent) != UVM_GPU_CONF_COMPUTE_MODE_NONE;
}

bool uvm_conf_computing_mode_enabled(const uvm_gpu_t *gpu)
{
    return uvm_conf_computing_mode_enabled_parent(gpu->parent);
}

bool uvm_conf_computing_mode_is_hcc(const uvm_gpu_t *gpu)
{
    return uvm_conf_computing_get_mode(gpu->parent) == UVM_GPU_CONF_COMPUTE_MODE_HCC;
}

NV_STATUS uvm_conf_computing_init_parent_gpu(const uvm_parent_gpu_t *parent)
{
    UvmGpuConfComputeMode cc, sys_cc;
    uvm_gpu_t *first;

    uvm_assert_mutex_locked(&g_uvm_global.global_lock);

    // TODO: Bug 2844714: since we have no routine to traverse parent GPUs,
    // find first child GPU and get its parent.
    first = uvm_global_processor_mask_find_first_gpu(&g_uvm_global.retained_gpus);
    if (!first)
        return NV_OK;

    sys_cc = uvm_conf_computing_get_mode(first->parent);
    cc = uvm_conf_computing_get_mode(parent);

    return cc == sys_cc ? NV_OK : NV_ERR_NOT_SUPPORTED;
}

static void dma_buffer_destroy_locked(uvm_conf_computing_dma_buffer_pool_t *dma_buffer_pool,
                                      uvm_conf_computing_dma_buffer_t *dma_buffer)
{
    uvm_assert_mutex_locked(&dma_buffer_pool->lock);

    list_del(&dma_buffer->node);
    uvm_tracker_wait_deinit(&dma_buffer->tracker);

    uvm_mem_free(dma_buffer->alloc);
    uvm_mem_free(dma_buffer->auth_tag);
    uvm_kvfree(dma_buffer);
}

static uvm_gpu_t *dma_buffer_pool_to_gpu(uvm_conf_computing_dma_buffer_pool_t *dma_buffer_pool)
{
    return container_of(dma_buffer_pool, uvm_gpu_t, conf_computing.dma_buffer_pool);
}

// Allocate and map a new DMA stage buffer to CPU and GPU (VA)
static NV_STATUS dma_buffer_create(uvm_conf_computing_dma_buffer_pool_t *dma_buffer_pool,
                                   uvm_conf_computing_dma_buffer_t **dma_buffer_out)
{
    uvm_gpu_t *dma_owner;
    uvm_conf_computing_dma_buffer_t *dma_buffer;
    uvm_mem_t *alloc = NULL;
    NV_STATUS status = NV_OK;
    size_t auth_tags_size = (UVM_CONF_COMPUTING_DMA_BUFFER_SIZE / PAGE_SIZE) * UVM_CONF_COMPUTING_AUTH_TAG_SIZE;

    dma_buffer = uvm_kvmalloc_zero(sizeof(*dma_buffer));
    if (!dma_buffer)
        return NV_ERR_NO_MEMORY;

    dma_owner = dma_buffer_pool_to_gpu(dma_buffer_pool);
    uvm_tracker_init(&dma_buffer->tracker);
    INIT_LIST_HEAD(&dma_buffer->node);

    status = uvm_mem_alloc_sysmem_dma_and_map_cpu_kernel(UVM_CONF_COMPUTING_DMA_BUFFER_SIZE, dma_owner, NULL, &alloc);
    if (status != NV_OK)
        goto err;

    dma_buffer->alloc = alloc;

    status = uvm_mem_map_gpu_kernel(alloc, dma_owner);
    if (status != NV_OK)
        goto err;

    status = uvm_mem_alloc_sysmem_dma_and_map_cpu_kernel(auth_tags_size, dma_owner, NULL, &alloc);
    if (status != NV_OK)
        goto err;

    dma_buffer->auth_tag = alloc;

    status = uvm_mem_map_gpu_kernel(alloc, dma_owner);
    if (status != NV_OK)
        goto err;

    *dma_buffer_out = dma_buffer;

    return status;

err:
    dma_buffer_destroy_locked(dma_buffer_pool, dma_buffer);
    return status;
}

void uvm_conf_computing_dma_buffer_pool_sync(uvm_conf_computing_dma_buffer_pool_t *dma_buffer_pool)
{
    uvm_conf_computing_dma_buffer_t *dma_buffer;

    if (dma_buffer_pool->num_dma_buffers == 0)
        return;

    uvm_mutex_lock(&dma_buffer_pool->lock);
    list_for_each_entry(dma_buffer, &dma_buffer_pool->free_dma_buffers, node)
        uvm_tracker_wait(&dma_buffer->tracker);
    uvm_mutex_unlock(&dma_buffer_pool->lock);
}

static void conf_computing_dma_buffer_pool_deinit(uvm_conf_computing_dma_buffer_pool_t *dma_buffer_pool)
{
    uvm_conf_computing_dma_buffer_t *dma_buffer;
    uvm_conf_computing_dma_buffer_t *next_buff;

    if (dma_buffer_pool->num_dma_buffers == 0)
        return;

    // Because the pool is teared down at the same time the GPU is unregistered
    // the lock is required only to quiet assertions not for functional reasons
    // see dma_buffer_destroy_locked()).
    uvm_mutex_lock(&dma_buffer_pool->lock);

    list_for_each_entry_safe(dma_buffer, next_buff, &dma_buffer_pool->free_dma_buffers, node) {
        dma_buffer_destroy_locked(dma_buffer_pool, dma_buffer);
        dma_buffer_pool->num_dma_buffers--;
    }

    UVM_ASSERT(dma_buffer_pool->num_dma_buffers == 0);
    UVM_ASSERT(list_empty(&dma_buffer_pool->free_dma_buffers));
    uvm_mutex_unlock(&dma_buffer_pool->lock);
}

static void dma_buffer_pool_add(uvm_conf_computing_dma_buffer_pool_t *dma_buffer_pool,
                               uvm_conf_computing_dma_buffer_t *dma_buffer)
{
    uvm_assert_mutex_locked(&dma_buffer_pool->lock);
    list_add_tail(&dma_buffer->node, &dma_buffer_pool->free_dma_buffers);
}

static NV_STATUS conf_computing_dma_buffer_pool_init(uvm_conf_computing_dma_buffer_pool_t *dma_buffer_pool)
{
    size_t i;
    uvm_gpu_t *gpu;
    size_t num_dma_buffers = 32;
    NV_STATUS status = NV_OK;

    UVM_ASSERT(dma_buffer_pool->num_dma_buffers == 0);

    gpu = dma_buffer_pool_to_gpu(dma_buffer_pool);

    UVM_ASSERT(uvm_conf_computing_mode_enabled(gpu));

    INIT_LIST_HEAD(&dma_buffer_pool->free_dma_buffers);
    uvm_mutex_init(&dma_buffer_pool->lock, UVM_LOCK_ORDER_CONF_COMPUTING_DMA_BUFFER_POOL);
    dma_buffer_pool->num_dma_buffers = num_dma_buffers;

    uvm_mutex_lock(&dma_buffer_pool->lock);
    for (i = 0; i < num_dma_buffers; i++) {
        uvm_conf_computing_dma_buffer_t *dma_buffer;

        status = dma_buffer_create(dma_buffer_pool, &dma_buffer);
        if (status != NV_OK)
            break;

        dma_buffer_pool_add(dma_buffer_pool, dma_buffer);
    }
    uvm_mutex_unlock(&dma_buffer_pool->lock);

    if (i < num_dma_buffers)
        conf_computing_dma_buffer_pool_deinit(dma_buffer_pool);

    return status;
}

static NV_STATUS dma_buffer_pool_expand_locked(uvm_conf_computing_dma_buffer_pool_t *dma_buffer_pool)
{
    size_t i;
    uvm_gpu_t *gpu;
    size_t nb_to_alloc;
    NV_STATUS status = NV_OK;
    UVM_ASSERT(dma_buffer_pool->num_dma_buffers > 0);

    gpu = dma_buffer_pool_to_gpu(dma_buffer_pool);
    nb_to_alloc = dma_buffer_pool->num_dma_buffers;
    for (i = 0; i < nb_to_alloc; ++i) {
        uvm_conf_computing_dma_buffer_t *dma_buffer;

        status = dma_buffer_create(dma_buffer_pool, &dma_buffer);
        if (status != NV_OK)
            break;

        dma_buffer_pool_add(dma_buffer_pool, dma_buffer);
    }

    dma_buffer_pool->num_dma_buffers += i;

    if (i == 0)
        return status;

    return NV_OK;
}

NV_STATUS uvm_conf_computing_dma_buffer_alloc(uvm_conf_computing_dma_buffer_pool_t *dma_buffer_pool,
                                              uvm_conf_computing_dma_buffer_t **dma_buffer_out,
                                              uvm_tracker_t *out_tracker)
{
    uvm_conf_computing_dma_buffer_t *dma_buffer = NULL;
    NV_STATUS status;

    UVM_ASSERT(dma_buffer_pool->num_dma_buffers > 0);

    // TODO: Bug 3385623: Heuristically expand DMA memory pool
    uvm_mutex_lock(&dma_buffer_pool->lock);
    if (list_empty(&dma_buffer_pool->free_dma_buffers)) {
        status = dma_buffer_pool_expand_locked(dma_buffer_pool);

        if (status != NV_OK) {
            uvm_mutex_unlock(&dma_buffer_pool->lock);
            return status;
        }
    }

    // We're guaranteed that at least one DMA stage buffer is available at this
    // point.
    dma_buffer = list_first_entry(&dma_buffer_pool->free_dma_buffers, uvm_conf_computing_dma_buffer_t, node);
    list_del_init(&dma_buffer->node);
    uvm_mutex_unlock(&dma_buffer_pool->lock);

    status = uvm_tracker_wait_for_other_gpus(&dma_buffer->tracker, dma_buffer->alloc->dma_owner);
    if (status != NV_OK)
        goto error;

    if (out_tracker)
        status = uvm_tracker_add_tracker_safe(out_tracker, &dma_buffer->tracker);
    else
        status = uvm_tracker_wait(&dma_buffer->tracker);

    if (status != NV_OK)
        goto error;

    uvm_page_mask_zero(&dma_buffer->encrypted_page_mask);
    *dma_buffer_out = dma_buffer;

    return status;

error:
    uvm_tracker_deinit(&dma_buffer->tracker);
    uvm_conf_computing_dma_buffer_free(dma_buffer_pool, dma_buffer, NULL);
    return status;
}

void uvm_conf_computing_dma_buffer_free(uvm_conf_computing_dma_buffer_pool_t *dma_buffer_pool,
                                        uvm_conf_computing_dma_buffer_t *dma_buffer,
                                        uvm_tracker_t *tracker)
{

    NV_STATUS status;

    if (!dma_buffer)
        return;

    UVM_ASSERT(dma_buffer_pool->num_dma_buffers > 0);

    uvm_tracker_remove_completed(&dma_buffer->tracker);
    if (tracker) {
        uvm_tracker_remove_completed(tracker);
        status = uvm_tracker_add_tracker_safe(&dma_buffer->tracker, tracker);
        if (status != NV_OK)
            UVM_ASSERT(status == uvm_global_get_status());
    }

    uvm_mutex_lock(&dma_buffer_pool->lock);
    dma_buffer_pool_add(dma_buffer_pool, dma_buffer);
    uvm_mutex_unlock(&dma_buffer_pool->lock);
}

static void dummy_iv_mem_deinit(uvm_gpu_t *gpu)
{
    uvm_mem_free(gpu->conf_computing.iv_mem);
}

static NV_STATUS dummy_iv_mem_init(uvm_gpu_t *gpu)
{
    NV_STATUS status;

    if (!uvm_conf_computing_mode_is_hcc(gpu))
        return NV_OK;

    status = uvm_mem_alloc_sysmem_dma(sizeof(UvmCslIv), gpu, NULL, &gpu->conf_computing.iv_mem);
    if (status != NV_OK)
        return status;

    status = uvm_mem_map_gpu_kernel(gpu->conf_computing.iv_mem, gpu);
    if (status != NV_OK)
        goto error;

    return NV_OK;

error:
    dummy_iv_mem_deinit(gpu);
    return status;
}

NV_STATUS uvm_conf_computing_gpu_init(uvm_gpu_t *gpu)
{
    NV_STATUS status;

    if (!uvm_conf_computing_mode_enabled(gpu))
        return NV_OK;

    status = conf_computing_dma_buffer_pool_init(&gpu->conf_computing.dma_buffer_pool);
    if (status != NV_OK)
        return status;

    status = dummy_iv_mem_init(gpu);
    if (status != NV_OK)
        goto error;

    return NV_OK;

error:
    uvm_conf_computing_gpu_deinit(gpu);
    return status;
}

void uvm_conf_computing_gpu_deinit(uvm_gpu_t *gpu)
{
    dummy_iv_mem_deinit(gpu);
    conf_computing_dma_buffer_pool_deinit(&gpu->conf_computing.dma_buffer_pool);
}

void uvm_conf_computing_log_gpu_encryption(uvm_channel_t *channel, UvmCslIv *iv)
{
    NV_STATUS status;

    uvm_mutex_lock(&channel->csl.ctx_lock);
    status = nvUvmInterfaceCslIncrementIv(&channel->csl.ctx, UVM_CSL_OPERATION_DECRYPT, 1, iv);
    uvm_mutex_unlock(&channel->csl.ctx_lock);

    // TODO: Bug 4014720: If nvUvmInterfaceCslIncrementIv returns with
    // NV_ERR_INSUFFICIENT_RESOURCES then the IV needs to be rotated via
    // nvUvmInterfaceCslRotateIv.
    UVM_ASSERT(status == NV_OK);
}

void uvm_conf_computing_acquire_encryption_iv(uvm_channel_t *channel, UvmCslIv *iv)
{
    NV_STATUS status;

    uvm_mutex_lock(&channel->csl.ctx_lock);
    status = nvUvmInterfaceCslIncrementIv(&channel->csl.ctx, UVM_CSL_OPERATION_ENCRYPT, 1, iv);
    uvm_mutex_unlock(&channel->csl.ctx_lock);

    // TODO: Bug 4014720: If nvUvmInterfaceCslIncrementIv returns with
    // NV_ERR_INSUFFICIENT_RESOURCES then the IV needs to be rotated via
    // nvUvmInterfaceCslRotateIv.
    UVM_ASSERT(status == NV_OK);
}

void uvm_conf_computing_cpu_encrypt(uvm_channel_t *channel,
                                    void *dst_cipher,
                                    const void *src_plain,
                                    UvmCslIv *encrypt_iv,
                                    size_t size,
                                    void *auth_tag_buffer)
{
    NV_STATUS status;

    UVM_ASSERT(size);

    uvm_mutex_lock(&channel->csl.ctx_lock);
    status = nvUvmInterfaceCslEncrypt(&channel->csl.ctx,
                                      size,
                                      (NvU8 const *) src_plain,
                                      encrypt_iv,
                                      (NvU8 *) dst_cipher,
                                      (NvU8 *) auth_tag_buffer);
    uvm_mutex_unlock(&channel->csl.ctx_lock);

    // nvUvmInterfaceCslEncrypt fails when a 64-bit encryption counter
    // overflows. This is not supposed to happen on CC.
    UVM_ASSERT(status == NV_OK);
}

NV_STATUS uvm_conf_computing_cpu_decrypt(uvm_channel_t *channel,
                                         void *dst_plain,
                                         const void *src_cipher,
                                         const UvmCslIv *src_iv,
                                         size_t size,
                                         const void *auth_tag_buffer)
{
    NV_STATUS status;

    uvm_mutex_lock(&channel->csl.ctx_lock);
    status = nvUvmInterfaceCslDecrypt(&channel->csl.ctx,
                                      size,
                                      (const NvU8 *) src_cipher,
                                      src_iv,
                                      (NvU8 *) dst_plain,
                                      NULL,
                                      0,
                                      (const NvU8 *) auth_tag_buffer);
    uvm_mutex_unlock(&channel->csl.ctx_lock);

    return status;
}

NV_STATUS uvm_conf_computing_fault_decrypt(uvm_parent_gpu_t *parent_gpu,
                                           void *dst_plain,
                                           const void *src_cipher,
                                           const void *auth_tag_buffer,
                                           NvU8 valid)
{
    NV_STATUS status;

    // There is no dedicated lock for the CSL context associated with replayable
    // faults. The mutual exclusion required by the RM CSL API is enforced by
    // relying on the GPU replayable service lock (ISR lock), since fault
    // decryption is invoked as part of fault servicing.
    UVM_ASSERT(uvm_sem_is_locked(&parent_gpu->isr.replayable_faults.service_lock));

    UVM_ASSERT(!uvm_parent_gpu_replayable_fault_buffer_is_uvm_owned(parent_gpu));

    status = nvUvmInterfaceCslDecrypt(&parent_gpu->fault_buffer_info.rm_info.replayable.cslCtx,
                                      parent_gpu->fault_buffer_hal->entry_size(parent_gpu),
                                      (const NvU8 *) src_cipher,
                                      NULL,
                                      (NvU8 *) dst_plain,
                                      &valid,
                                      sizeof(valid),
                                      (const NvU8 *) auth_tag_buffer);

    if (status != NV_OK)
        UVM_ERR_PRINT("nvUvmInterfaceCslDecrypt() failed: %s, GPU %s\n", nvstatusToString(status), parent_gpu->name);

    return status;
}

void uvm_conf_computing_fault_increment_decrypt_iv(uvm_parent_gpu_t *parent_gpu, NvU64 increment)
{
    NV_STATUS status;

    // See comment in uvm_conf_computing_fault_decrypt
    UVM_ASSERT(uvm_sem_is_locked(&parent_gpu->isr.replayable_faults.service_lock));

    UVM_ASSERT(!uvm_parent_gpu_replayable_fault_buffer_is_uvm_owned(parent_gpu));

    status = nvUvmInterfaceCslIncrementIv(&parent_gpu->fault_buffer_info.rm_info.replayable.cslCtx,
                                          UVM_CSL_OPERATION_DECRYPT,
                                          increment,
                                          NULL);

    UVM_ASSERT(status == NV_OK);
}
