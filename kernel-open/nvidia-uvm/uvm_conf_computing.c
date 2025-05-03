/*******************************************************************************
    Copyright (c) 2021-2025 NVIDIA Corporation

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

// Amount of encrypted data on a given engine that triggers key rotation. This
// is a UVM internal threshold, different from that of RM, and used only during
// testing.
//
// Key rotation is triggered when the total encryption size, or the total
// decryption size (whatever comes first) reaches this lower threshold on the
// engine.
#define UVM_CONF_COMPUTING_KEY_ROTATION_LOWER_THRESHOLD (UVM_SIZE_1MB * 8)

// The maximum number of secure operations per push is:
// UVM_MAX_PUSH_SIZE / min(CE encryption size, CE decryption size)
// + 1 (tracking semaphore) =  128 * 1024 / 56 + 1 = 2342
#define UVM_CONF_COMPUTING_IV_REMAINING_LIMIT_MIN 2342lu

// Channels use 32-bit counters so the value after rotation is 0xffffffff.
// setting the limit to this value (or higher) will result in rotation
// on every check. However, pre-emptive rotation when submitting control
// GPFIFO entries relies on the fact that multiple successive checks after
// rotation do not trigger more rotations if there was no IV used in between.
#define UVM_CONF_COMPUTING_IV_REMAINING_LIMIT_MAX 0xfffffffelu

// Attempt rotation when two billion IVs are left. IV rotation call can fail if
// the necessary locks are not available, so multiple attempts may be need for
// IV rotation to succeed.
#define UVM_CONF_COMPUTING_IV_REMAINING_LIMIT_DEFAULT (1lu << 31)

// Start rotating after 500 encryption/decryptions when running tests.
#define UVM_CONF_COMPUTING_IV_REMAINING_LIMIT_TESTS ((1lu << 32) - 500lu)
static ulong uvm_conf_computing_channel_iv_rotation_limit = UVM_CONF_COMPUTING_IV_REMAINING_LIMIT_DEFAULT;

module_param(uvm_conf_computing_channel_iv_rotation_limit, ulong, S_IRUGO);

void uvm_conf_computing_check_parent_gpu(const uvm_parent_gpu_t *parent)
{
    uvm_assert_mutex_locked(&g_uvm_global.global_lock);

    // Confidential Computing enablement on the system should match enablement
    // on the GPU.
    UVM_ASSERT(parent->rm_info.gpuConfComputeCaps.bConfComputingEnabled == g_uvm_global.conf_computing_enabled);
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
    size_t num_dma_buffers = 32;
    NV_STATUS status = NV_OK;

    UVM_ASSERT(dma_buffer_pool->num_dma_buffers == 0);
    UVM_ASSERT(g_uvm_global.conf_computing_enabled);

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

// The production key rotation defaults are such that key rotations rarely
// happen. During UVM testing more frequent rotations are triggering by relying
// on internal encryption usage accounting. When key rotations are triggered by
// UVM, the driver does not rely on channel key rotation notifiers.
//
// TODO: Bug 4612912: UVM should be able to programmatically set the rotation
// lower threshold. This function, and all the metadata associated with it
// (per-pool encryption accounting, for example) can be removed at that point.
static bool key_rotation_is_notifier_driven(void)
{
    return !uvm_enable_builtin_tests;
}

NV_STATUS uvm_conf_computing_gpu_init(uvm_gpu_t *gpu)
{
    NV_STATUS status;

    if (!g_uvm_global.conf_computing_enabled)
        return NV_OK;

    status = conf_computing_dma_buffer_pool_init(&gpu->conf_computing.dma_buffer_pool);
    if (status != NV_OK)
        return status;

    status = dummy_iv_mem_init(gpu);
    if (status != NV_OK)
        goto error;

    if (uvm_enable_builtin_tests && uvm_conf_computing_channel_iv_rotation_limit == UVM_CONF_COMPUTING_IV_REMAINING_LIMIT_DEFAULT)
        uvm_conf_computing_channel_iv_rotation_limit = UVM_CONF_COMPUTING_IV_REMAINING_LIMIT_TESTS;

    if (uvm_conf_computing_channel_iv_rotation_limit < UVM_CONF_COMPUTING_IV_REMAINING_LIMIT_MIN ||
        uvm_conf_computing_channel_iv_rotation_limit > UVM_CONF_COMPUTING_IV_REMAINING_LIMIT_MAX) {
        UVM_ERR_PRINT("Value of uvm_conf_computing_channel_iv_rotation_limit: %lu is outside of the safe "
                      "range: <%lu, %lu>. Using the default value instead (%lu)\n",
                      uvm_conf_computing_channel_iv_rotation_limit,
                      UVM_CONF_COMPUTING_IV_REMAINING_LIMIT_MIN,
                      UVM_CONF_COMPUTING_IV_REMAINING_LIMIT_MAX,
                      UVM_CONF_COMPUTING_IV_REMAINING_LIMIT_DEFAULT);
        uvm_conf_computing_channel_iv_rotation_limit = UVM_CONF_COMPUTING_IV_REMAINING_LIMIT_DEFAULT;
    }

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

void uvm_conf_computing_log_gpu_encryption(uvm_channel_t *channel, size_t size, UvmCslIv *iv)
{
    NV_STATUS status;
    uvm_channel_pool_t *pool;

    if (uvm_channel_is_lcic(channel))
        pool = uvm_channel_lcic_get_paired_wlc(channel)->pool;
    else
        pool = channel->pool;

    uvm_mutex_lock(&channel->csl.ctx_lock);

    if (uvm_conf_computing_is_key_rotation_enabled_in_pool(pool)) {
        status = nvUvmInterfaceCslLogEncryption(&channel->csl.ctx, UVM_CSL_OPERATION_DECRYPT, size);

        // Informing RM of an encryption/decryption should not fail
        UVM_ASSERT(status == NV_OK);

        if (!key_rotation_is_notifier_driven())
            atomic64_add(size, &pool->conf_computing.key_rotation.encrypted);
    }

    status = nvUvmInterfaceCslIncrementIv(&channel->csl.ctx, UVM_CSL_OPERATION_DECRYPT, 1, iv);

    // IV rotation is done preemptively as needed, so the above
    // call cannot return failure.
    UVM_ASSERT(status == NV_OK);

    uvm_mutex_unlock(&channel->csl.ctx_lock);
}

void uvm_conf_computing_acquire_encryption_iv(uvm_channel_t *channel, UvmCslIv *iv)
{
    NV_STATUS status;

    uvm_mutex_lock(&channel->csl.ctx_lock);
    status = nvUvmInterfaceCslIncrementIv(&channel->csl.ctx, UVM_CSL_OPERATION_ENCRYPT, 1, iv);
    uvm_mutex_unlock(&channel->csl.ctx_lock);

    // IV rotation is done preemptively as needed, so the above
    // call cannot return failure.
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
    uvm_channel_pool_t *pool;

    UVM_ASSERT(size);

    if (uvm_channel_is_lcic(channel))
        pool = uvm_channel_lcic_get_paired_wlc(channel)->pool;
    else
        pool = channel->pool;

    uvm_mutex_lock(&channel->csl.ctx_lock);

    status = nvUvmInterfaceCslEncrypt(&channel->csl.ctx,
                                      size,
                                      (NvU8 const *) src_plain,
                                      encrypt_iv,
                                      (NvU8 *) dst_cipher,
                                      (NvU8 *) auth_tag_buffer);

    // IV rotation is done preemptively as needed, so the above
    // call cannot return failure.
    UVM_ASSERT(status == NV_OK);

    if (uvm_conf_computing_is_key_rotation_enabled_in_pool(pool)) {
        status = nvUvmInterfaceCslLogEncryption(&channel->csl.ctx, UVM_CSL_OPERATION_ENCRYPT, size);

        // Informing RM of an encryption/decryption should not fail
        UVM_ASSERT(status == NV_OK);

        if (!key_rotation_is_notifier_driven())
            atomic64_add(size, &pool->conf_computing.key_rotation.decrypted);
    }

    uvm_mutex_unlock(&channel->csl.ctx_lock);
}

NV_STATUS uvm_conf_computing_cpu_decrypt(uvm_channel_t *channel,
                                         void *dst_plain,
                                         const void *src_cipher,
                                         const UvmCslIv *src_iv,
                                         NvU32 key_version,
                                         size_t size,
                                         const void *auth_tag_buffer)
{
    NV_STATUS status;

    // The CSL context associated with a channel can be used by multiple
    // threads. The IV sequence is thus guaranteed only while the channel is
    // "locked for push". The channel/push lock is released in
    // "uvm_channel_end_push", and at that time the GPU encryption operations
    // have not executed, yet. Therefore the caller has to use
    // "uvm_conf_computing_log_gpu_encryption" to explicitly store IVs needed
    // to perform CPU decryption and pass those IVs to this function after the
    // push that did the encryption completes.
    UVM_ASSERT(src_iv);

    uvm_mutex_lock(&channel->csl.ctx_lock);
    status = nvUvmInterfaceCslDecrypt(&channel->csl.ctx,
                                      size,
                                      (const NvU8 *) src_cipher,
                                      src_iv,
                                      key_version,
                                      (NvU8 *) dst_plain,
                                      NULL,
                                      0,
                                      (const NvU8 *) auth_tag_buffer);

    if (status != NV_OK) {
        UVM_ERR_PRINT("nvUvmInterfaceCslDecrypt() failed: %s, channel %s, GPU %s\n",
                      nvstatusToString(status),
                      channel->name,
                      uvm_gpu_name(uvm_channel_get_gpu(channel)));
    }

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
    NvU32 fault_entry_size = parent_gpu->fault_buffer_hal->entry_size(parent_gpu);
    UvmCslContext *csl_context = &parent_gpu->fault_buffer.rm_info.replayable.cslCtx;

    // There is no dedicated lock for the CSL context associated with replayable
    // faults. The mutual exclusion required by the RM CSL API is enforced by
    // relying on the GPU replayable service lock (ISR lock), since fault
    // decryption is invoked as part of fault servicing.
    UVM_ASSERT(uvm_sem_is_locked(&parent_gpu->isr.replayable_faults.service_lock));

    UVM_ASSERT(g_uvm_global.conf_computing_enabled);

    status = nvUvmInterfaceCslLogEncryption(csl_context, UVM_CSL_OPERATION_DECRYPT, fault_entry_size);

    // Informing RM of an encryption/decryption should not fail
    UVM_ASSERT(status == NV_OK);

    status = nvUvmInterfaceCslDecrypt(csl_context,
                                      fault_entry_size,
                                      (const NvU8 *) src_cipher,
                                      NULL,
                                      NV_U32_MAX,
                                      (NvU8 *) dst_plain,
                                      &valid,
                                      sizeof(valid),
                                      (const NvU8 *) auth_tag_buffer);

    if (status != NV_OK) {
        UVM_ERR_PRINT("nvUvmInterfaceCslDecrypt() failed: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_parent_gpu_name(parent_gpu));

    }

    return status;
}

void uvm_conf_computing_fault_increment_decrypt_iv(uvm_parent_gpu_t *parent_gpu)
{
    NV_STATUS status;
    NvU32 fault_entry_size = parent_gpu->fault_buffer_hal->entry_size(parent_gpu);
    UvmCslContext *csl_context = &parent_gpu->fault_buffer.rm_info.replayable.cslCtx;

    // See comment in uvm_conf_computing_fault_decrypt
    UVM_ASSERT(uvm_sem_is_locked(&parent_gpu->isr.replayable_faults.service_lock));

    UVM_ASSERT(g_uvm_global.conf_computing_enabled);

    status = nvUvmInterfaceCslLogEncryption(csl_context, UVM_CSL_OPERATION_DECRYPT, fault_entry_size);

    // Informing RM of an encryption/decryption should not fail
    UVM_ASSERT(status == NV_OK);

    status = nvUvmInterfaceCslIncrementIv(csl_context, UVM_CSL_OPERATION_DECRYPT, 1, NULL);

    UVM_ASSERT(status == NV_OK);
}

void uvm_conf_computing_query_message_pools(uvm_channel_t *channel,
                                            NvU64 *remaining_encryptions,
                                            NvU64 *remaining_decryptions)
{
    NV_STATUS status;

    UVM_ASSERT(channel);
    UVM_ASSERT(remaining_encryptions);
    UVM_ASSERT(remaining_decryptions);

    uvm_mutex_lock(&channel->csl.ctx_lock);
    status = nvUvmInterfaceCslQueryMessagePool(&channel->csl.ctx, UVM_CSL_OPERATION_ENCRYPT, remaining_encryptions);
    UVM_ASSERT(status == NV_OK);
    UVM_ASSERT(*remaining_encryptions <= NV_U32_MAX);

    status = nvUvmInterfaceCslQueryMessagePool(&channel->csl.ctx, UVM_CSL_OPERATION_DECRYPT, remaining_decryptions);
    UVM_ASSERT(status == NV_OK);
    UVM_ASSERT(*remaining_decryptions <= NV_U32_MAX);

    // LCIC channels never use CPU encrypt/GPU decrypt
    if (uvm_channel_is_lcic(channel))
        UVM_ASSERT(*remaining_encryptions == NV_U32_MAX);

    uvm_mutex_unlock(&channel->csl.ctx_lock);
}

static NV_STATUS uvm_conf_computing_rotate_channel_ivs_below_limit_internal(uvm_channel_t *channel, NvU64 limit)
{
    NV_STATUS status = NV_OK;
    NvU64 remaining_encryptions, remaining_decryptions;
    bool rotate_encryption_iv, rotate_decryption_iv;

    UVM_ASSERT(uvm_channel_is_locked_for_push(channel) ||
               (uvm_channel_is_lcic(channel) && uvm_channel_manager_is_wlc_ready(channel->pool->manager)));

    uvm_conf_computing_query_message_pools(channel, &remaining_encryptions, &remaining_decryptions);

    // Ignore decryption limit for SEC2, only CE channels support
    // GPU encrypt/CPU decrypt. However, RM reports _some_ decrementing
    // value for SEC2 decryption counter.
    rotate_decryption_iv = (remaining_decryptions <= limit) && uvm_channel_is_ce(channel);
    rotate_encryption_iv = remaining_encryptions <= limit;

    if (!rotate_encryption_iv && !rotate_decryption_iv)
        return NV_OK;

    // Wait for all in-flight pushes. The caller needs to guarantee that there
    // are no concurrent pushes created, e.g. by only calling rotate after
    // a channel is locked_for_push.
    status = uvm_channel_wait(channel);
    if (status != NV_OK)
        return status;

    uvm_mutex_lock(&channel->csl.ctx_lock);

    if (rotate_encryption_iv)
        status = nvUvmInterfaceCslRotateIv(&channel->csl.ctx, UVM_CSL_OPERATION_ENCRYPT);

    if (status == NV_OK && rotate_decryption_iv)
        status = nvUvmInterfaceCslRotateIv(&channel->csl.ctx, UVM_CSL_OPERATION_DECRYPT);

    uvm_mutex_unlock(&channel->csl.ctx_lock);

    // Change the error to out of resources if the available IVs are running
    // too low
    if (status == NV_ERR_STATE_IN_USE &&
        (remaining_encryptions < UVM_CONF_COMPUTING_IV_REMAINING_LIMIT_MIN ||
         remaining_decryptions < UVM_CONF_COMPUTING_IV_REMAINING_LIMIT_MIN))
        return NV_ERR_INSUFFICIENT_RESOURCES;

    return status;
}

NV_STATUS uvm_conf_computing_rotate_channel_ivs_below_limit(uvm_channel_t *channel, NvU64 limit, bool retry_if_busy)
{
    NV_STATUS status;

    do {
        status = uvm_conf_computing_rotate_channel_ivs_below_limit_internal(channel, limit);
    } while (retry_if_busy && status == NV_ERR_STATE_IN_USE);

    // Hide "busy" error. The rotation will be retried at the next opportunity.
    if (!retry_if_busy && status == NV_ERR_STATE_IN_USE)
        status = NV_OK;

    return status;
}

NV_STATUS uvm_conf_computing_maybe_rotate_channel_ivs(uvm_channel_t *channel)
{
    return uvm_conf_computing_rotate_channel_ivs_below_limit(channel, uvm_conf_computing_channel_iv_rotation_limit, false);
}

NV_STATUS uvm_conf_computing_maybe_rotate_channel_ivs_retry_busy(uvm_channel_t *channel)
{
    return uvm_conf_computing_rotate_channel_ivs_below_limit(channel, uvm_conf_computing_channel_iv_rotation_limit, true);
}

void uvm_conf_computing_enable_key_rotation(uvm_gpu_t *gpu)
{
    if (!g_uvm_global.conf_computing_enabled)
        return;

    // Key rotation cannot be enabled on UVM if it is disabled on RM
    if (!gpu->parent->rm_info.gpuConfComputeCaps.bKeyRotationEnabled)
        return;

    gpu->channel_manager->conf_computing.key_rotation_enabled = true;
}

void uvm_conf_computing_disable_key_rotation(uvm_gpu_t *gpu)
{
    if (!g_uvm_global.conf_computing_enabled)
        return;

    gpu->channel_manager->conf_computing.key_rotation_enabled = false;
}

bool uvm_conf_computing_is_key_rotation_enabled(uvm_gpu_t *gpu)
{
    UVM_ASSERT(gpu);

    // If the channel_manager is not set, we're in channel manager destroy
    // path after the pointer was NULL-ed. Chances are that other key rotation
    // infrastructure is not available either. Disallow the key rotation.
    return gpu->channel_manager && gpu->channel_manager->conf_computing.key_rotation_enabled;
}

bool uvm_conf_computing_is_key_rotation_enabled_in_pool(uvm_channel_pool_t *pool)
{
    if (!uvm_conf_computing_is_key_rotation_enabled(pool->manager->gpu))
        return false;

    // TODO: Bug 4586447: key rotation must be disabled in the SEC2 engine,
    // because currently the encryption key is shared between UVM and RM, but
    // UVM is not able to idle SEC2 channels owned by RM.
    if (uvm_channel_pool_is_sec2(pool))
        return false;

    // Key rotation happens as part of channel reservation, and LCIC channels
    // are never reserved directly. Rotation of keys in LCIC channels happens
    // as the result of key rotation in WLC channels.
    //
    // Return false even if there is nothing fundamental prohibiting direct key
    // rotation on LCIC pools
    if (uvm_channel_pool_is_lcic(pool))
        return false;

    return true;
}

static bool conf_computing_is_key_rotation_pending_use_stats(uvm_channel_pool_t *pool)
{
    NvU64 decrypted, encrypted;

    UVM_ASSERT(!key_rotation_is_notifier_driven());

    decrypted = atomic64_read(&pool->conf_computing.key_rotation.decrypted);

    if (decrypted > UVM_CONF_COMPUTING_KEY_ROTATION_LOWER_THRESHOLD)
        return true;

    encrypted = atomic64_read(&pool->conf_computing.key_rotation.encrypted);

    if (encrypted > UVM_CONF_COMPUTING_KEY_ROTATION_LOWER_THRESHOLD)
        return true;

    return false;
}

static bool conf_computing_is_key_rotation_pending_use_notifier(uvm_channel_pool_t *pool)
{
    // If key rotation is pending for the pool's engine, then the key rotation
    // notifier in any of the engine channels can be used by UVM to detect the
    // situation. Note that RM doesn't update all the notifiers in a single
    // atomic operation, so it is possible that the channel read by UVM (the
    // first one in the pool) indicates that a key rotation is pending, but
    // another channel in the pool (temporarily) indicates the opposite, or vice
    // versa.
    uvm_channel_t *first_channel = pool->channels;

    UVM_ASSERT(key_rotation_is_notifier_driven());
    UVM_ASSERT(first_channel != NULL);

    return first_channel->channel_info.keyRotationNotifier->status == UVM_KEY_ROTATION_STATUS_PENDING;
}

bool uvm_conf_computing_is_key_rotation_pending_in_pool(uvm_channel_pool_t *pool)
{
    if (!uvm_conf_computing_is_key_rotation_enabled_in_pool(pool))
        return false;

    if (key_rotation_is_notifier_driven())
        return conf_computing_is_key_rotation_pending_use_notifier(pool);
    else
        return conf_computing_is_key_rotation_pending_use_stats(pool);
}

NV_STATUS uvm_conf_computing_rotate_pool_key(uvm_channel_pool_t *pool)
{
    NV_STATUS status;

    UVM_ASSERT(uvm_conf_computing_is_key_rotation_enabled_in_pool(pool));
    UVM_ASSERT(pool->conf_computing.key_rotation.csl_contexts != NULL);
    UVM_ASSERT(pool->conf_computing.key_rotation.num_csl_contexts > 0);

    // NV_ERR_STATE_IN_USE indicates that RM was not able to acquire the
    // required locks at this time. This status is not interpreted as an error,
    // but as a sign for UVM to try again later. This is the same "protocol"
    // used in IV rotation.
    status = nvUvmInterfaceCslRotateKey(pool->conf_computing.key_rotation.csl_contexts,
                                        pool->conf_computing.key_rotation.num_csl_contexts);

    if (status == NV_OK) {
        pool->conf_computing.key_rotation.version++;

        if (!key_rotation_is_notifier_driven()) {
            atomic64_set(&pool->conf_computing.key_rotation.decrypted, 0);
            atomic64_set(&pool->conf_computing.key_rotation.encrypted, 0);
        }
    }
    else if (status != NV_ERR_STATE_IN_USE) {
        UVM_DBG_PRINT("nvUvmInterfaceCslRotateKey() failed in engine %u: %s\n",
                      pool->engine_index,
                      nvstatusToString(status));
    }

    return status;
}

__attribute__ ((format(printf, 6, 7)))
NV_STATUS uvm_conf_computing_util_memcopy_cpu_to_gpu(uvm_gpu_t *gpu,
                                                     uvm_gpu_address_t dst_gpu_address,
                                                     void *src_plain,
                                                     size_t size,
                                                     uvm_tracker_t *tracker,
                                                     const char *format,
                                                     ...)
{
    NV_STATUS status;
    uvm_push_t push;
    uvm_conf_computing_dma_buffer_t *dma_buffer;
    uvm_gpu_address_t src_gpu_address, auth_tag_gpu_address;
    void *dst_cipher, *auth_tag;
    va_list args;

    UVM_ASSERT(g_uvm_global.conf_computing_enabled);
    UVM_ASSERT(size <= UVM_CONF_COMPUTING_DMA_BUFFER_SIZE);

    status = uvm_conf_computing_dma_buffer_alloc(&gpu->conf_computing.dma_buffer_pool, &dma_buffer, NULL);
    if (status != NV_OK)
        return status;

    va_start(args, format);
    status = uvm_push_begin_acquire(gpu->channel_manager, UVM_CHANNEL_TYPE_CPU_TO_GPU, tracker, &push, format, args);
    va_end(args);

    if (status != NV_OK)
        goto out;

    dst_cipher = uvm_mem_get_cpu_addr_kernel(dma_buffer->alloc);
    auth_tag = uvm_mem_get_cpu_addr_kernel(dma_buffer->auth_tag);
    uvm_conf_computing_cpu_encrypt(push.channel, dst_cipher, src_plain, NULL, size, auth_tag);

    src_gpu_address = uvm_mem_gpu_address_virtual_kernel(dma_buffer->alloc, gpu);
    auth_tag_gpu_address = uvm_mem_gpu_address_virtual_kernel(dma_buffer->auth_tag, gpu);
    gpu->parent->ce_hal->decrypt(&push, dst_gpu_address, src_gpu_address, size, auth_tag_gpu_address);

    status = uvm_push_end_and_wait(&push);

out:
    uvm_conf_computing_dma_buffer_free(&gpu->conf_computing.dma_buffer_pool, dma_buffer, NULL);
    return status;
}

__attribute__ ((format(printf, 6, 7)))
NV_STATUS uvm_conf_computing_util_memcopy_gpu_to_cpu(uvm_gpu_t *gpu,
                                                     void *dst_plain,
                                                     uvm_gpu_address_t src_gpu_address,
                                                     size_t size,
                                                     uvm_tracker_t *tracker,
                                                     const char *format,
                                                     ...)
{
    NV_STATUS status;
    uvm_push_t push;
    uvm_conf_computing_dma_buffer_t *dma_buffer;
    uvm_gpu_address_t dst_gpu_address, auth_tag_gpu_address;
    void *src_cipher, *auth_tag;
    va_list args;

    UVM_ASSERT(g_uvm_global.conf_computing_enabled);
    UVM_ASSERT(size <= UVM_CONF_COMPUTING_DMA_BUFFER_SIZE);

    status = uvm_conf_computing_dma_buffer_alloc(&gpu->conf_computing.dma_buffer_pool, &dma_buffer, NULL);
    if (status != NV_OK)
        return status;

    va_start(args, format);
    status = uvm_push_begin_acquire(gpu->channel_manager, UVM_CHANNEL_TYPE_GPU_TO_CPU, tracker, &push, format, args);
    va_end(args);

    if (status != NV_OK)
        goto out;

    uvm_conf_computing_log_gpu_encryption(push.channel, size, dma_buffer->decrypt_iv);
    dma_buffer->key_version[0] = uvm_channel_pool_key_version(push.channel->pool);

    dst_gpu_address = uvm_mem_gpu_address_virtual_kernel(dma_buffer->alloc, gpu);
    auth_tag_gpu_address = uvm_mem_gpu_address_virtual_kernel(dma_buffer->auth_tag, gpu);
    gpu->parent->ce_hal->encrypt(&push, dst_gpu_address, src_gpu_address, size, auth_tag_gpu_address);

    status = uvm_push_end_and_wait(&push);
    if (status != NV_OK)
        goto out;

    src_cipher = uvm_mem_get_cpu_addr_kernel(dma_buffer->alloc);
    auth_tag = uvm_mem_get_cpu_addr_kernel(dma_buffer->auth_tag);
    status = uvm_conf_computing_cpu_decrypt(push.channel,
                                            dst_plain,
                                            src_cipher,
                                            dma_buffer->decrypt_iv,
                                            dma_buffer->key_version[0],
                                            size,
                                            auth_tag);

 out:
    uvm_conf_computing_dma_buffer_free(&gpu->conf_computing.dma_buffer_pool, dma_buffer, NULL);
    return status;
}
