/*******************************************************************************
    Copyright (c) 2015-2023 NVIDIA Corporation

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

#include "uvm_channel.h"

#include "uvm_api.h"
#include "uvm_common.h"
#include "uvm_global.h"
#include "uvm_hal.h"
#include "uvm_procfs.h"
#include "uvm_push.h"
#include "uvm_gpu_semaphore.h"
#include "uvm_lock.h"
#include "uvm_kvmalloc.h"

#include "nv_uvm_types.h"
#include "nv_uvm_interface.h"
#include "clb06f.h"
#include "uvm_conf_computing.h"

static unsigned uvm_channel_num_gpfifo_entries = UVM_CHANNEL_NUM_GPFIFO_ENTRIES_DEFAULT;

#define UVM_CHANNEL_GPFIFO_LOC_DEFAULT "auto"

static char *uvm_channel_gpfifo_loc = UVM_CHANNEL_GPFIFO_LOC_DEFAULT;

#define UVM_CHANNEL_GPPUT_LOC_DEFAULT "auto"

static char *uvm_channel_gpput_loc = UVM_CHANNEL_GPPUT_LOC_DEFAULT;

#define UVM_CHANNEL_PUSHBUFFER_LOC_DEFAULT "auto"

static char *uvm_channel_pushbuffer_loc = UVM_CHANNEL_PUSHBUFFER_LOC_DEFAULT;

module_param(uvm_channel_num_gpfifo_entries, uint, S_IRUGO);
module_param(uvm_channel_gpfifo_loc, charp, S_IRUGO);
module_param(uvm_channel_gpput_loc, charp, S_IRUGO);
module_param(uvm_channel_pushbuffer_loc, charp, S_IRUGO);

static NV_STATUS manager_create_procfs_dirs(uvm_channel_manager_t *manager);
static NV_STATUS manager_create_procfs(uvm_channel_manager_t *manager);
static NV_STATUS channel_create_procfs(uvm_channel_t *channel);

typedef enum
{
    // Only remove completed GPFIFO entries from the pushbuffer
    UVM_CHANNEL_UPDATE_MODE_COMPLETED,

    // Remove all remaining GPFIFO entries from the pushbuffer, regardless of
    // whether they're actually done yet.
    UVM_CHANNEL_UPDATE_MODE_FORCE_ALL
} uvm_channel_update_mode_t;

bool uvm_channel_pool_uses_mutex(uvm_channel_pool_t *pool)
{
    // Work submission to proxy channels in SR-IOV heavy entails calling RM API
    // that acquires a mutex, so the proxy channel pool must use a mutex.
    if (uvm_channel_pool_is_proxy(pool))
        return true;

    // When Confidential Computing is enabled push submission requires call to
    // CSL routines which acquire the CSL context mutex lock. Moreover, WLC
    // submission uses UVM_SPIN_LOOP, which can call 'schedule', to wait for
    // LCIC completion. Indirect submission is synchronous, calling
    // uvm_push_wait which again uses UVM_SPIN_LOOP.
    if (g_uvm_global.conf_computing_enabled)
        return true;

    // Unless the mutex is required, the spinlock is preferred when work
    // submission is expected to take little time.
    return false;
}

static void channel_pool_lock_init(uvm_channel_pool_t *pool)
{
    uvm_lock_order_t order = UVM_LOCK_ORDER_CHANNEL;

    if (g_uvm_global.conf_computing_enabled && uvm_channel_pool_is_wlc(pool))
        order = UVM_LOCK_ORDER_WLC_CHANNEL;

    if (uvm_channel_pool_uses_mutex(pool))
        uvm_mutex_init(&pool->mutex, order);
    else
        uvm_spin_lock_init(&pool->spinlock, order);
}

static void channel_pool_lock(uvm_channel_pool_t *pool)
{
    if (uvm_channel_pool_uses_mutex(pool))
        uvm_mutex_lock(&pool->mutex);
    else
        uvm_spin_lock(&pool->spinlock);
}

static void channel_pool_unlock(uvm_channel_pool_t *pool)
{
    if (uvm_channel_pool_uses_mutex(pool))
        uvm_mutex_unlock(&pool->mutex);
    else
        uvm_spin_unlock(&pool->spinlock);
}

// Update channel progress, completing up to max_to_complete entries
static NvU32 uvm_channel_update_progress_with_max(uvm_channel_t *channel,
                                                  NvU32 max_to_complete,
                                                  uvm_channel_update_mode_t mode)
{
    NvU32 gpu_get;
    NvU32 cpu_put;
    NvU32 completed_count = 0;
    NvU32 pending_gpfifos;

    NvU64 completed_value = uvm_channel_update_completed_value(channel);

    channel_pool_lock(channel->pool);

    // Completed value should never exceed the queued value
    UVM_ASSERT_MSG_RELEASE(completed_value <= channel->tracking_sem.queued_value,
                           "GPU %s channel %s unexpected completed_value 0x%llx > queued_value 0x%llx\n",
                           uvm_gpu_name(uvm_channel_get_gpu(channel)),
                           channel->name,
                           completed_value,
                           channel->tracking_sem.queued_value);

    cpu_put = channel->cpu_put;
    gpu_get = channel->gpu_get;

    while (gpu_get != cpu_put && completed_count < max_to_complete) {
        uvm_gpfifo_entry_t *entry = &channel->gpfifo_entries[gpu_get];

        if (mode == UVM_CHANNEL_UPDATE_MODE_COMPLETED && entry->tracking_semaphore_value > completed_value)
            break;

        if (entry->type == UVM_GPFIFO_ENTRY_TYPE_NORMAL) {
            uvm_pushbuffer_mark_completed(channel, entry);
            list_add_tail(&entry->push_info->available_list_node, &channel->available_push_infos);
        }

        gpu_get = (gpu_get + 1) % channel->num_gpfifo_entries;
        ++completed_count;
    }

    channel->gpu_get = gpu_get;

    channel_pool_unlock(channel->pool);

    if (cpu_put >= gpu_get)
        pending_gpfifos = cpu_put - gpu_get;
    else
        pending_gpfifos = channel->num_gpfifo_entries - gpu_get + cpu_put;

    return pending_gpfifos;
}

NvU32 uvm_channel_update_progress(uvm_channel_t *channel)
{
    // By default, don't complete too many entries at a time to spread the cost
    // of doing so across callers and avoid potentially holding a spin lock for
    // too long.
    return uvm_channel_update_progress_with_max(channel, 8, UVM_CHANNEL_UPDATE_MODE_COMPLETED);
}

// Update progress for all pending GPFIFO entries. This might take a longer time
// and should be only used in exceptional circumstances like when a channel
// error is encountered. Otherwise, uvm_chanel_update_progress() should be used.
static NvU32 channel_update_progress_all(uvm_channel_t *channel, uvm_channel_update_mode_t mode)
{
    return uvm_channel_update_progress_with_max(channel, channel->num_gpfifo_entries, mode);
}

NvU32 uvm_channel_update_progress_all(uvm_channel_t *channel)
{
    return channel_update_progress_all(channel, UVM_CHANNEL_UPDATE_MODE_COMPLETED);
}

NvU32 uvm_channel_manager_update_progress(uvm_channel_manager_t *channel_manager)
{
    NvU32 pending_gpfifos = 0;
    uvm_channel_pool_t *pool;

    uvm_for_each_pool(pool, channel_manager) {
        uvm_channel_t *channel;

        uvm_for_each_channel_in_pool(channel, pool)
            pending_gpfifos += uvm_channel_update_progress(channel);
    }

    return pending_gpfifos;
}

static NvU32 channel_get_available_gpfifo_entries(uvm_channel_t *channel)
{
    NvU32 available = channel->num_gpfifo_entries;

    uvm_channel_pool_assert_locked(channel->pool);

    // Remove sentinel entry
    available -= 1;

    // Remove entries of ongoing pushes
    available -= channel->current_gpfifo_count;

    // Remove pending entries
    if (channel->cpu_put >= channel->gpu_get)
        available -= (channel->cpu_put - channel->gpu_get);
    else
        available -= (channel->cpu_put + channel->num_gpfifo_entries - channel->gpu_get);

    UVM_ASSERT(available < channel->num_gpfifo_entries);

    return available;
}

NvU32 uvm_channel_get_available_gpfifo_entries(uvm_channel_t *channel)
{
    NvU32 available;

    channel_pool_lock(channel->pool);
    available = channel_get_available_gpfifo_entries(channel);
    channel_pool_unlock(channel->pool);

    return available;
}

static bool try_claim_channel_locked(uvm_channel_t *channel, NvU32 num_gpfifo_entries)
{
    bool claimed = false;

    UVM_ASSERT(num_gpfifo_entries > 0);
    UVM_ASSERT(num_gpfifo_entries < channel->num_gpfifo_entries);

    uvm_channel_pool_assert_locked(channel->pool);

    if (channel_get_available_gpfifo_entries(channel) >= num_gpfifo_entries) {
        channel->current_gpfifo_count += num_gpfifo_entries;
        claimed = true;
    }

    return claimed;
}

static bool try_claim_channel(uvm_channel_t *channel, NvU32 num_gpfifo_entries)
{
    bool claimed;

    channel_pool_lock(channel->pool);
    claimed = try_claim_channel_locked(channel, num_gpfifo_entries);
    channel_pool_unlock(channel->pool);

    return claimed;
}

static void unlock_channel_for_push(uvm_channel_t *channel)
{
    NvU32 index;

    if (!g_uvm_global.conf_computing_enabled)
        return;

    index = uvm_channel_index_in_pool(channel);

    uvm_channel_pool_assert_locked(channel->pool);
    UVM_ASSERT(test_bit(index, channel->pool->push_locks));

    __clear_bit(index, channel->pool->push_locks);
    uvm_up_out_of_order(&channel->pool->push_sem);
}

bool uvm_channel_is_locked_for_push(uvm_channel_t *channel)
{
    if (g_uvm_global.conf_computing_enabled)
        return test_bit(uvm_channel_index_in_pool(channel), channel->pool->push_locks);

    // For CE and proxy channels, we always return that the channel is locked,
    // which has no functional impact in the UVM channel code-flow, this is only
    // used in UVM_ASSERTs.
    return true;
}

static void lock_channel_for_push(uvm_channel_t *channel)
{
    NvU32 index = uvm_channel_index_in_pool(channel);

    UVM_ASSERT(g_uvm_global.conf_computing_enabled);
    uvm_channel_pool_assert_locked(channel->pool);
    UVM_ASSERT(!test_bit(index, channel->pool->push_locks));

    __set_bit(index, channel->pool->push_locks);
}

static bool test_claim_and_lock_channel(uvm_channel_t *channel, NvU32 num_gpfifo_entries)
{
    NvU32 index = uvm_channel_index_in_pool(channel);

    UVM_ASSERT(g_uvm_global.conf_computing_enabled);
    uvm_channel_pool_assert_locked(channel->pool);

    if (!test_bit(index, channel->pool->push_locks) && try_claim_channel_locked(channel, num_gpfifo_entries)) {
        lock_channel_for_push(channel);
        return true;
    }

    return false;
}

// Reserve a channel in the specified pool. The channel is locked until the push
// ends
static NV_STATUS channel_reserve_and_lock_in_pool(uvm_channel_pool_t *pool, uvm_channel_t **channel_out)
{
    uvm_channel_t *channel;
    uvm_spin_loop_t spin;
    NvU32 index;

    UVM_ASSERT(pool);
    UVM_ASSERT(g_uvm_global.conf_computing_enabled);

    // This semaphore is uvm_up() in unlock_channel_for_push() as part of the
    // uvm_channel_end_push() routine.
    uvm_down(&pool->push_sem);

    // At least one channel is unlocked. We check if any unlocked channel is
    // available, i.e., if it has free GPFIFO entries.

    channel_pool_lock(pool);

    for_each_clear_bit(index, pool->push_locks, pool->num_channels) {
        channel = &pool->channels[index];
        if (try_claim_channel_locked(channel, 1)) {
            lock_channel_for_push(channel);
            goto done;
        }
    }

    channel_pool_unlock(pool);

    // No channels are available. Update and check errors on all channels until
    // one becomes available.
    uvm_spin_loop_init(&spin);
    while (1) {
        uvm_for_each_channel_in_pool(channel, pool) {
            NV_STATUS status;

            uvm_channel_update_progress(channel);

            channel_pool_lock(pool);

            if (test_claim_and_lock_channel(channel, 1))
                goto done;

            channel_pool_unlock(pool);

            status = uvm_channel_check_errors(channel);
            if (status != NV_OK) {
                uvm_up(&pool->push_sem);
                return status;
            }

            UVM_SPIN_LOOP(&spin);
        }
    }

done:
    channel_pool_unlock(pool);
    *channel_out = channel;
    return NV_OK;
}

// Reserve a channel in the specified pool
static NV_STATUS channel_reserve_in_pool(uvm_channel_pool_t *pool, uvm_channel_t **channel_out)
{
    uvm_channel_t *channel;
    uvm_spin_loop_t spin;

    UVM_ASSERT(pool);

    if (g_uvm_global.conf_computing_enabled)
        return channel_reserve_and_lock_in_pool(pool, channel_out);

    uvm_for_each_channel_in_pool(channel, pool) {
        // TODO: Bug 1764953: Prefer idle/less busy channels
        if (try_claim_channel(channel, 1)) {
            *channel_out = channel;
            return NV_OK;
        }
    }

    uvm_spin_loop_init(&spin);
    while (1) {
        uvm_for_each_channel_in_pool(channel, pool) {
            NV_STATUS status;

            uvm_channel_update_progress(channel);

            if (try_claim_channel(channel, 1)) {
                *channel_out = channel;

                return NV_OK;
            }

            status = uvm_channel_check_errors(channel);
            if (status != NV_OK)
                return status;

            UVM_SPIN_LOOP(&spin);
        }
    }

    UVM_ASSERT_MSG(0, "Cannot get here?!\n");

    return NV_ERR_GENERIC;
}

NV_STATUS uvm_channel_reserve_type(uvm_channel_manager_t *manager, uvm_channel_type_t type, uvm_channel_t **channel_out)
{
    uvm_channel_pool_t *pool = manager->pool_to_use.default_for_type[type];

    UVM_ASSERT(pool != NULL);
    UVM_ASSERT(type < UVM_CHANNEL_TYPE_COUNT);

    return channel_reserve_in_pool(pool, channel_out);
}

NV_STATUS uvm_channel_reserve_gpu_to_gpu(uvm_channel_manager_t *manager,
                                         uvm_gpu_t *dst_gpu,
                                         uvm_channel_t **channel_out)
{
    const NvU32 dst_gpu_index = uvm_id_gpu_index(dst_gpu->id);
    uvm_channel_pool_t *pool = manager->pool_to_use.gpu_to_gpu[dst_gpu_index];

    // If there is no recommended pool for the given GPU pair, use default
    if (pool == NULL)
        pool = manager->pool_to_use.default_for_type[UVM_CHANNEL_TYPE_GPU_TO_GPU];

    UVM_ASSERT(pool->pool_type == UVM_CHANNEL_POOL_TYPE_CE);

    return channel_reserve_in_pool(pool, channel_out);
}

NV_STATUS uvm_channel_manager_wait(uvm_channel_manager_t *manager)
{
    NV_STATUS status = NV_OK;
    uvm_spin_loop_t spin;

    if (uvm_channel_manager_update_progress(manager) == 0)
        return uvm_channel_manager_check_errors(manager);

    uvm_spin_loop_init(&spin);
    while (uvm_channel_manager_update_progress(manager) > 0 && status == NV_OK) {
        UVM_SPIN_LOOP(&spin);
        status = uvm_channel_manager_check_errors(manager);
    }

    return status;
}

static NvU32 channel_get_available_push_info_index(uvm_channel_t *channel)
{
    uvm_push_info_t *push_info;

    channel_pool_lock(channel->pool);

    push_info = list_first_entry_or_null(&channel->available_push_infos, uvm_push_info_t, available_list_node);
    UVM_ASSERT(push_info != NULL);
    UVM_ASSERT(push_info->on_complete == NULL && push_info->on_complete_data == NULL);
    list_del(&push_info->available_list_node);

    channel_pool_unlock(channel->pool);

    return push_info - channel->push_infos;
}

static void channel_semaphore_gpu_encrypt_payload(uvm_push_t *push, NvU64 semaphore_va)
{
    NvU32 iv_index;
    uvm_gpu_t *gpu = push->gpu;
    uvm_channel_t *channel = push->channel;
    uvm_gpu_semaphore_t *semaphore = &channel->tracking_sem.semaphore;
    uvm_gpu_address_t notifier_gpu_va = uvm_gpu_semaphore_get_notifier_gpu_va(semaphore);
    uvm_gpu_address_t auth_tag_gpu_va = uvm_gpu_semaphore_get_auth_tag_gpu_va(semaphore);
    uvm_gpu_address_t encrypted_payload_gpu_va = uvm_gpu_semaphore_get_encrypted_payload_gpu_va(semaphore);
    uvm_gpu_address_t semaphore_gpu_va = uvm_gpu_address_virtual(semaphore_va);
    UvmCslIv *iv_cpu_addr = semaphore->conf_computing.ivs;
    NvU32 payload_size = sizeof(*uvm_gpu_semaphore_get_encrypted_payload_cpu_va(semaphore));
    NvU32 *last_pushed_notifier = &semaphore->conf_computing.last_pushed_notifier;

    UVM_ASSERT(g_uvm_global.conf_computing_enabled);
    UVM_ASSERT(uvm_channel_is_ce(channel));

    iv_index = ((*last_pushed_notifier + 2) / 2) % channel->num_gpfifo_entries;

    uvm_conf_computing_log_gpu_encryption(channel, &iv_cpu_addr[iv_index]);

    gpu->parent->ce_hal->memset_4(push, notifier_gpu_va, ++(*last_pushed_notifier), sizeof(*last_pushed_notifier));
    gpu->parent->ce_hal->encrypt(push, encrypted_payload_gpu_va, semaphore_gpu_va, payload_size, auth_tag_gpu_va);
    gpu->parent->ce_hal->memset_4(push, notifier_gpu_va, ++(*last_pushed_notifier), sizeof(*last_pushed_notifier));
}

// Auxiliary buffer only used by SEC2 channels for CPU computation of the method
// stream signature. Note that it is required that this inline pushbuffer buffer
// is reserved at push->begin.
static void push_reserve_csl_sign_buf(uvm_push_t *push)
{
    uvm_gpu_address_t dummy;
    NvU32 *buf;

    buf = uvm_push_get_single_inline_buffer(push, UVM_CONF_COMPUTING_SIGN_BUF_MAX_SIZE, UVM_METHOD_SIZE, &dummy);

    // Offset UVM_METHOD_SIZE from buf due to the NOP method.
    UVM_ASSERT((buf - UVM_METHOD_SIZE / sizeof(*buf)) == push->begin);
}

static uvm_channel_t *get_paired_channel(uvm_channel_t *channel)
{
    unsigned index;
    uvm_channel_pool_t *paired_pool;
    uvm_channel_type_t paired_channel_type;

    UVM_ASSERT(channel);
    UVM_ASSERT(uvm_channel_is_wlc(channel) || uvm_channel_is_lcic(channel));

    index = uvm_channel_index_in_pool(channel);
    paired_channel_type = uvm_channel_is_wlc(channel) ? UVM_CHANNEL_TYPE_LCIC : UVM_CHANNEL_TYPE_WLC;
    paired_pool = channel->pool->manager->pool_to_use.default_for_type[paired_channel_type];
    return paired_pool->channels + index;
}

uvm_channel_t *uvm_channel_lcic_get_paired_wlc(uvm_channel_t *lcic_channel)
{
    UVM_ASSERT(lcic_channel);
    UVM_ASSERT(uvm_channel_is_lcic(lcic_channel));

    return get_paired_channel(lcic_channel);
}

uvm_channel_t *uvm_channel_wlc_get_paired_lcic(uvm_channel_t *wlc_channel)
{
    UVM_ASSERT(wlc_channel);
    UVM_ASSERT(uvm_channel_is_wlc(wlc_channel));

    return get_paired_channel(wlc_channel);
}

static NV_STATUS channel_rotate_and_reserve_launch_channel(uvm_channel_t *channel, uvm_channel_t **launch_channel)
{
    uvm_channel_manager_t *manager = channel->pool->manager;
    NV_STATUS status;

    status = uvm_conf_computing_maybe_rotate_channel_ivs(channel);
    if (status != NV_OK)
        return status;

    // CE channels, other than WLC fix launch schedule setup, need a launch
    // channel that needs to be reserved
    if (uvm_channel_is_ce(channel) &&
        !(uvm_channel_is_wlc(channel) && uvm_channel_manager_is_wlc_ready(manager))) {
        uvm_channel_t *local_launch_channel = NULL;
        uvm_channel_type_t indirect_channel_type = uvm_channel_manager_is_wlc_ready(manager) ?
                                                   UVM_CHANNEL_TYPE_WLC :
                                                   UVM_CHANNEL_TYPE_SEC2;
        status = uvm_channel_reserve_type(manager, indirect_channel_type, &local_launch_channel);
        if (status != NV_OK)
            return status;

        // Indirect launch relies on pre-allocated resources to avoid failure
        // paths. This includes pre-allocating IV space. There's no way to
        // undo the launch channel reservation, so just return an error.
        status = uvm_conf_computing_maybe_rotate_channel_ivs(local_launch_channel);
        if (status != NV_OK) {
            uvm_channel_release(local_launch_channel, 1);
            return status;
        }

        if (uvm_channel_is_wlc(local_launch_channel)) {
            status = uvm_conf_computing_maybe_rotate_channel_ivs(uvm_channel_wlc_get_paired_lcic(local_launch_channel));
            if (status != NV_OK) {
                uvm_channel_release(local_launch_channel, 1);
                return status;
            }
        }
        *launch_channel = local_launch_channel;
    }


    return NV_OK;
}

NV_STATUS uvm_channel_begin_push(uvm_channel_t *channel, uvm_push_t *push)
{
    NV_STATUS status = NV_OK;
    uvm_channel_manager_t *manager;

    UVM_ASSERT(channel);
    UVM_ASSERT(push);

    manager = channel->pool->manager;

    // Only SEC2 and WLC with set up fixed schedule can use direct push
    // submission. All other cases (including WLC pre-schedule) need to
    // reserve a launch channel that will be used to submit this push
    // indirectly.
    if (g_uvm_global.conf_computing_enabled) {
        status = channel_rotate_and_reserve_launch_channel(channel, &push->launch_channel);
        if (status != NV_OK)
            return status;
    }

    // When the Confidential Computing feature is enabled, the channel's lock
    // should have already been acquired in uvm_channel_reserve() or
    // channel_reserve_and_lock_in_pool().
    UVM_ASSERT(uvm_channel_is_locked_for_push(channel));

    push->channel = channel;
    push->channel_tracking_value = 0;

    status = uvm_pushbuffer_begin_push(manager->pushbuffer, push);
    if (status != NV_OK)
        return status;

    push->push_info_index = channel_get_available_push_info_index(channel);

    if (uvm_channel_is_sec2(push->channel))
        push_reserve_csl_sign_buf(push);
    else if (uvm_channel_is_wlc(push->channel) && uvm_channel_manager_is_wlc_ready(manager))
        uvm_conf_computing_acquire_encryption_iv(push->channel, &push->launch_iv);

    return NV_OK;
}

static void internal_channel_submit_work(uvm_push_t *push, NvU32 push_size, NvU32 new_gpu_put)
{
    NvU64 *gpfifo_entry;
    NvU64 pushbuffer_va;
    uvm_channel_t *channel = push->channel;
    uvm_pushbuffer_t *pushbuffer = uvm_channel_get_pushbuffer(channel);
    uvm_gpu_t *gpu = uvm_channel_get_gpu(channel);

    BUILD_BUG_ON(sizeof(*gpfifo_entry) != NVB06F_GP_ENTRY__SIZE);
    UVM_ASSERT(!uvm_channel_is_proxy(channel));

    gpfifo_entry = (NvU64*)channel->channel_info.gpFifoEntries + channel->cpu_put;
    pushbuffer_va = uvm_pushbuffer_get_gpu_va_for_push(pushbuffer, push);

    if (g_uvm_global.conf_computing_enabled) {
        void *unprotected_pb = uvm_pushbuffer_get_unprotected_cpu_va_for_push(pushbuffer, push);
        UVM_ASSERT(uvm_channel_is_sec2(channel));

        // Copy push data to unprotected sysmem, it has already been signed.
        memcpy(unprotected_pb, push->begin, push_size);
    }

    gpu->parent->host_hal->set_gpfifo_entry(gpfifo_entry, pushbuffer_va, push_size, UVM_GPFIFO_SYNC_PROCEED);

    // Need to make sure all the pushbuffer and the GPFIFO entries writes
    // complete before updating GPPUT. We also don't want any reads to be moved
    // after the GPPut write as the GPU might modify the data they read as soon
    // as the GPPut write happens.
    mb();

    gpu->parent->host_hal->write_gpu_put(channel, new_gpu_put);
}

static void proxy_channel_submit_work(uvm_push_t *push, NvU32 push_size)
{
    NV_STATUS status;
    uvm_channel_t *channel = push->channel;

    UVM_ASSERT(uvm_channel_is_proxy(channel));

    status = nvUvmInterfacePagingChannelPushStream(channel->proxy.handle, (char *) push->begin, push_size);

    if (status != NV_OK) {
        uvm_push_info_t *push_info = uvm_push_info_from_push(push);

        // If the RM invocation fails, there is no clean recovery available
        // (for example, the vGPU plugin may have crashed), so swallow the error
        // but print a descriptive message about the failed push.
        UVM_ASSERT_MSG(status == NV_OK,
                       "nvUvmInterfacePagingChannelPushStream() failed: %s, GPU %s, push '%s' started at %s:%d in %s()\n",
                       nvstatusToString(status),
                       uvm_gpu_name(uvm_channel_get_gpu(channel)),
                       push_info->description,
                       push_info->filename,
                       push_info->line,
                       push_info->function);
    }
}

static void do_semaphore_release(uvm_push_t *push, NvU64 semaphore_va, NvU32 new_payload)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    if (uvm_channel_is_ce(push->channel))
        gpu->parent->ce_hal->semaphore_release(push, semaphore_va, new_payload);
    else
        gpu->parent->sec2_hal->semaphore_release(push, semaphore_va, new_payload);
}

static void uvm_channel_tracking_semaphore_release(uvm_push_t *push, NvU64 semaphore_va, NvU32 new_payload)
{
    // We used to skip the membar or use membar GPU for the semaphore release
    // for a few pushes, but that doesn't provide sufficient ordering guarantees
    // in some cases (e.g. ga100 with an LCE with PCEs from both HSHUBs) for the
    // semaphore writes. To be safe, just always uses a membar sys for now.
    // TODO: Bug 3770539: Optimize membars used by end of push semaphore releases
    (void)uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_GPU);
    (void)uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);

    do_semaphore_release(push, semaphore_va, new_payload);

    // When the Confidential Computing feature is enabled, additional work
    // needs to be scheduled to get an encrypted shadow copy in unprotected
    // sysmem. This allows UVM to later decrypt it and observe the new
    // semaphore value.
    if (g_uvm_global.conf_computing_enabled && uvm_channel_is_ce(push->channel))
        channel_semaphore_gpu_encrypt_payload(push, semaphore_va);
}

static void internal_channel_submit_work_wlc(uvm_push_t *push)
{
    uvm_channel_t *wlc_channel = push->channel;
    uvm_channel_t *lcic_channel = uvm_channel_wlc_get_paired_lcic(wlc_channel);
    UvmCslIv *iv_cpu_addr = lcic_channel->tracking_sem.semaphore.conf_computing.ivs;
    NvU32 *last_pushed_notifier;
    NvU32 iv_index;
    uvm_spin_loop_t spin;

    UVM_ASSERT(lcic_channel);

    // Wait for the WLC/LCIC to be primed. This means that PUT == GET + 2
    // and a WLC doorbell ring is enough to start work.
    UVM_SPIN_WHILE(!uvm_gpu_tracking_semaphore_is_completed(&lcic_channel->tracking_sem), &spin);

    // Executing WLC adds an extra job to LCIC
    ++lcic_channel->tracking_sem.queued_value;

    UVM_ASSERT_MSG(uvm_push_get_size(push) == UVM_MAX_WLC_PUSH_SIZE,
                   "WLC push submission size mismatch, expected: %u, got: %u",
                   UVM_MAX_WLC_PUSH_SIZE,
                   uvm_push_get_size(push));

    // Handles the CPU part of the setup for the LCIC to be able to do GPU
    // encryption of its tracking semaphore value. See setup_lcic_schedule().
    last_pushed_notifier  = &lcic_channel->tracking_sem.semaphore.conf_computing.last_pushed_notifier;
    *lcic_channel->conf_computing.static_notifier_entry_unprotected_sysmem_cpu = ++(*last_pushed_notifier);
    *lcic_channel->conf_computing.static_notifier_exit_unprotected_sysmem_cpu = ++(*last_pushed_notifier);
    iv_index = (*last_pushed_notifier / 2) % lcic_channel->num_gpfifo_entries;
    uvm_conf_computing_log_gpu_encryption(lcic_channel, &iv_cpu_addr[iv_index]);

    // Move push data
    uvm_conf_computing_cpu_encrypt(wlc_channel,
                                   wlc_channel->conf_computing.static_pb_unprotected_sysmem_cpu,
                                   push->begin,
                                   &push->launch_iv,
                                   UVM_MAX_WLC_PUSH_SIZE,
                                   wlc_channel->conf_computing.static_pb_unprotected_sysmem_auth_tag_cpu);

    // Make sure all encrypted data is observable before ringing the doorbell.
    wmb();

    // Ring the WLC doorbell to start processing the above push
    UVM_GPU_WRITE_ONCE(*wlc_channel->channel_info.workSubmissionOffset, wlc_channel->channel_info.workSubmissionToken);
}

static void internal_channel_submit_work_indirect_wlc(uvm_push_t *push, NvU32 old_cpu_put, NvU32 new_gpu_put)
{
    uvm_pushbuffer_t *pushbuffer = uvm_channel_get_pushbuffer(push->channel);
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    uvm_push_t indirect_push;
    NV_STATUS status;
    NvU64 gpfifo_entry;

    void *push_enc_cpu = uvm_pushbuffer_get_unprotected_cpu_va_for_push(pushbuffer, push);
    NvU64 push_enc_gpu = uvm_pushbuffer_get_unprotected_gpu_va_for_push(pushbuffer, push);
    void *push_enc_auth_tag;
    uvm_gpu_address_t push_enc_auth_tag_gpu;
    NvU64 gpfifo_gpu_va = push->channel->channel_info.gpFifoGpuVa + old_cpu_put * sizeof(gpfifo_entry);

    UVM_ASSERT(uvm_channel_is_ce(push->channel));
    UVM_ASSERT(uvm_channel_is_wlc(push->launch_channel));

    // WLC submissions are done under channel lock, so there should be no
    // contention to get the right submission order.
    UVM_ASSERT(push->channel->conf_computing.gpu_put == old_cpu_put);

    // This can never stall or return error. WLC launch after WLC channels are
    // initialized uses private static pb space and it neither needs the general
    // PB space, nor it counts towards max concurrent pushes.
    status = uvm_push_begin_on_reserved_channel(push->launch_channel,
                                                &indirect_push,
                                                "Worklaunch to '%s' via '%s'",
                                                push->channel->name,
                                                push->launch_channel->name);
    UVM_ASSERT(status == NV_OK);


    // Move over the pushbuffer data
    // WLC channels use a static preallocated space for launch auth tags
    push_enc_auth_tag = indirect_push.channel->conf_computing.launch_auth_tag_cpu;
    push_enc_auth_tag_gpu = uvm_gpu_address_virtual(indirect_push.channel->conf_computing.launch_auth_tag_gpu_va);

    uvm_conf_computing_cpu_encrypt(indirect_push.channel,
                                   push_enc_cpu,
                                   push->begin,
                                   NULL,
                                   uvm_push_get_size(push),
                                   push_enc_auth_tag);

    uvm_push_set_flag(&indirect_push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);

    gpu->parent->ce_hal->decrypt(&indirect_push,
                                 uvm_gpu_address_virtual(uvm_pushbuffer_get_gpu_va_for_push(pushbuffer, push)),
                                 uvm_gpu_address_virtual(push_enc_gpu),
                                 uvm_push_get_size(push),
                                 push_enc_auth_tag_gpu);

    gpu->parent->host_hal->set_gpfifo_entry(&gpfifo_entry,
                                            uvm_pushbuffer_get_gpu_va_for_push(pushbuffer, push),
                                            uvm_push_get_size(push),
                                            UVM_GPFIFO_SYNC_PROCEED);

    gpu->parent->ce_hal->memset_8(&indirect_push,
                                  uvm_gpu_address_virtual(gpfifo_gpu_va),
                                  gpfifo_entry,
                                  sizeof(gpfifo_entry));

    uvm_push_set_flag(&indirect_push, UVM_PUSH_FLAG_NEXT_MEMBAR_GPU);
    do_semaphore_release(&indirect_push, push->channel->channel_info.gpPutGpuVa, new_gpu_put);

    uvm_push_set_flag(&indirect_push, UVM_PUSH_FLAG_NEXT_MEMBAR_GPU);
    do_semaphore_release(&indirect_push,
                         push->channel->channel_info.workSubmissionOffsetGpuVa,
                         push->channel->channel_info.workSubmissionToken);

    // Ignore return value of push_wait. It can only fail with channel error
    // which will be detected when waiting for the primary push.
    (void)uvm_push_end_and_wait(&indirect_push);

    push->channel->conf_computing.gpu_put = new_gpu_put;
}

static void update_gpput_via_sec2(uvm_push_t *sec2_push, uvm_channel_t *channel, NvU32 new_gpu_put)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(sec2_push);
    void *gpput_auth_tag_cpu, *gpput_enc_cpu;
    uvm_gpu_address_t gpput_auth_tag_gpu, gpput_enc_gpu;
    NvU32 gpput_scratchpad[UVM_CONF_COMPUTING_SEC2_BUF_ALIGNMENT/sizeof(new_gpu_put)];

    UVM_ASSERT(uvm_channel_is_sec2(sec2_push->channel));

    gpput_enc_cpu = uvm_push_get_single_inline_buffer(sec2_push,
                                                      UVM_CONF_COMPUTING_SEC2_BUF_ALIGNMENT,
                                                      UVM_CONF_COMPUTING_SEC2_BUF_ALIGNMENT,
                                                      &gpput_enc_gpu);
    gpput_auth_tag_cpu = uvm_push_get_single_inline_buffer(sec2_push,
                                                           UVM_CONF_COMPUTING_AUTH_TAG_SIZE,
                                                           UVM_CONF_COMPUTING_AUTH_TAG_ALIGNMENT,
                                                           &gpput_auth_tag_gpu);

    // Update GPPUT. The update needs 4B write to specific offset,
    // however we can only do 16B aligned decrypt writes.
    // A poison value is written to all other locations, this is ignored in
    // most locations and overwritten by HW for GPGET location
    memset(gpput_scratchpad, 0, sizeof(gpput_scratchpad));
    UVM_ASSERT(sizeof(*gpput_scratchpad) == sizeof(new_gpu_put));
    gpput_scratchpad[(channel->channel_info.gpPutGpuVa % UVM_CONF_COMPUTING_AUTH_TAG_ALIGNMENT) /
                     sizeof(*gpput_scratchpad)] = new_gpu_put;

    // Set value of GPGET to be the same as GPPUT. It will be overwritten by
    // HW next time GET value changes. UVM never reads GPGET.
    // However, RM does read it when freeing a channel. When this function
    // is called from 'channel_manager_stop_wlc' we set the value of GPGET
    // to the same value as GPPUT. Mismatch between these two values makes
    // RM wait for any "pending" tasks, leading to significant delays in the
    // channel teardown sequence.
    UVM_ASSERT(channel->channel_info.gpPutGpuVa / UVM_CONF_COMPUTING_AUTH_TAG_ALIGNMENT ==
               channel->channel_info.gpGetGpuVa / UVM_CONF_COMPUTING_AUTH_TAG_ALIGNMENT);
    gpput_scratchpad[(channel->channel_info.gpGetGpuVa % UVM_CONF_COMPUTING_AUTH_TAG_ALIGNMENT) /
                     sizeof(*gpput_scratchpad)] = new_gpu_put;

    uvm_conf_computing_cpu_encrypt(sec2_push->channel,
                                   gpput_enc_cpu,
                                   gpput_scratchpad,
                                   NULL,
                                   sizeof(gpput_scratchpad),
                                   gpput_auth_tag_cpu);
    gpu->parent->sec2_hal->decrypt(sec2_push,
                                   UVM_ALIGN_DOWN(channel->channel_info.gpPutGpuVa,
                                                  UVM_CONF_COMPUTING_SEC2_BUF_ALIGNMENT),
                                   gpput_enc_gpu.address,
                                   sizeof(gpput_scratchpad),
                                   gpput_auth_tag_gpu.address);
}

static void set_gpfifo_via_sec2(uvm_push_t *sec2_push, uvm_channel_t *channel, NvU32 put, NvU64 value)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(sec2_push);
    void *gpfifo_auth_tag_cpu, *gpfifo_enc_cpu;
    uvm_gpu_address_t gpfifo_auth_tag_gpu, gpfifo_enc_gpu;
    NvU64 gpfifo_gpu = channel->channel_info.gpFifoGpuVa + put * sizeof(value);
    NvU64 gpfifo_scratchpad[2];

    UVM_ASSERT(uvm_channel_is_sec2(sec2_push->channel));

    gpfifo_enc_cpu = uvm_push_get_single_inline_buffer(sec2_push,
                                                       sizeof(gpfifo_scratchpad),
                                                       UVM_CONF_COMPUTING_SEC2_BUF_ALIGNMENT,
                                                       &gpfifo_enc_gpu);
    gpfifo_auth_tag_cpu = uvm_push_get_single_inline_buffer(sec2_push,
                                                            UVM_CONF_COMPUTING_AUTH_TAG_SIZE,
                                                            UVM_CONF_COMPUTING_AUTH_TAG_ALIGNMENT,
                                                            &gpfifo_auth_tag_gpu);

    if (IS_ALIGNED(gpfifo_gpu, UVM_CONF_COMPUTING_SEC2_BUF_ALIGNMENT)) {
        gpfifo_scratchpad[0] = value;

        // Set the value of the odd entry to noop.
        // It will be overwritten when the next entry is submitted.
        gpu->parent->host_hal->set_gpfifo_noop(&gpfifo_scratchpad[1]);
    }
    else {
        uvm_gpfifo_entry_t *previous_gpfifo;

        UVM_ASSERT(put > 0);

        previous_gpfifo = &channel->gpfifo_entries[put - 1];

        if (previous_gpfifo->type ==  UVM_GPFIFO_ENTRY_TYPE_CONTROL) {
            gpfifo_scratchpad[0] = previous_gpfifo->control_value;
        }
        else {
            uvm_pushbuffer_t *pushbuffer = uvm_channel_get_pushbuffer(channel);
            NvU64 prev_pb_va = uvm_pushbuffer_get_gpu_va_base(pushbuffer) + previous_gpfifo->pushbuffer_offset;

            // Reconstruct the previous GPFIFO entry. UVM_GPFIFO_SYNC_WAIT is
            // used only in static WLC schedule.
            // Overwriting the previous entry with the same value doesn't hurt,
            // whether the previous entry has been processed or not
            gpu->parent->host_hal->set_gpfifo_entry(&gpfifo_scratchpad[0],
                                                    prev_pb_va,
                                                    previous_gpfifo->pushbuffer_size,
                                                    UVM_GPFIFO_SYNC_PROCEED);
        }

        gpfifo_scratchpad[1] = value;
    }

    uvm_conf_computing_cpu_encrypt(sec2_push->channel,
                                   gpfifo_enc_cpu,
                                   gpfifo_scratchpad,
                                   NULL,
                                   sizeof(gpfifo_scratchpad),
                                   gpfifo_auth_tag_cpu);
    gpu->parent->sec2_hal->decrypt(sec2_push,
                                   UVM_ALIGN_DOWN(gpfifo_gpu, UVM_CONF_COMPUTING_SEC2_BUF_ALIGNMENT),
                                   gpfifo_enc_gpu.address,
                                   sizeof(gpfifo_scratchpad),
                                   gpfifo_auth_tag_gpu.address);
}

static NV_STATUS internal_channel_submit_work_indirect_sec2(uvm_push_t *push, NvU32 old_cpu_put, NvU32 new_gpu_put)
{
    uvm_pushbuffer_t *pushbuffer = uvm_channel_get_pushbuffer(push->channel);
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    uvm_push_t indirect_push;
    NV_STATUS status;
    NvU64 gpfifo_entry;

    void *push_enc_cpu = uvm_pushbuffer_get_unprotected_cpu_va_for_push(pushbuffer, push);
    NvU64 push_enc_gpu = uvm_pushbuffer_get_unprotected_gpu_va_for_push(pushbuffer, push);
    void *push_auth_tag_cpu;
    uvm_gpu_address_t push_auth_tag_gpu;
    uvm_spin_loop_t spin;

    UVM_ASSERT(uvm_channel_is_ce(push->channel));
    UVM_ASSERT(uvm_channel_is_sec2(push->launch_channel));

    // If the old_cpu_put is not equal to the last gpu put, other pushes are
    // pending that need to be submitted. That push/es' submission will update
    // the gpu_put pointer to expected value.
    UVM_SPIN_WHILE(push->channel->conf_computing.gpu_put != old_cpu_put, &spin);

    // This can never stall or return error. SEC2 launch used during init has
    // plenty of PB space available before it needs to check for push
    // completion/channel status. WLC launch after WLC channels are initialized
    // uses private static pb space and needs neither general PB space, nor it
    // counts towards max concurrent pushes.
    status = uvm_push_begin_on_reserved_channel(push->launch_channel,
                                                &indirect_push,
                                                "Worklaunch to '%s' via '%s'",
                                                push->channel->name,
                                                push->launch_channel->name);
    if (status != NV_OK)
        return status;


    // Move over the pushbuffer data
    push_auth_tag_cpu = uvm_push_get_single_inline_buffer(&indirect_push,
                                                          UVM_CONF_COMPUTING_AUTH_TAG_SIZE,
                                                          UVM_CONF_COMPUTING_AUTH_TAG_ALIGNMENT,
                                                          &push_auth_tag_gpu);

    uvm_conf_computing_cpu_encrypt(indirect_push.channel,
                                   push_enc_cpu,
                                   push->begin,
                                   NULL,
                                   uvm_push_get_size(push),
                                   push_auth_tag_cpu);

    uvm_push_set_flag(&indirect_push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);

    gpu->parent->sec2_hal->decrypt(&indirect_push,
                                   uvm_pushbuffer_get_gpu_va_for_push(pushbuffer, push),
                                   push_enc_gpu,
                                   uvm_push_get_size(push),
                                   push_auth_tag_gpu.address);

    gpu->parent->host_hal->set_gpfifo_entry(&gpfifo_entry,
                                            uvm_pushbuffer_get_gpu_va_for_push(pushbuffer, push),
                                            uvm_push_get_size(push),
                                            UVM_GPFIFO_SYNC_PROCEED);


    set_gpfifo_via_sec2(&indirect_push, push->channel, old_cpu_put, gpfifo_entry);
    update_gpput_via_sec2(&indirect_push, push->channel, new_gpu_put);

    // Ring the doorbell
    uvm_push_set_flag(&indirect_push, UVM_PUSH_FLAG_NEXT_MEMBAR_GPU);
    do_semaphore_release(&indirect_push,
                         push->channel->channel_info.workSubmissionOffsetGpuVa,
                         push->channel->channel_info.workSubmissionToken);

    status = uvm_push_end_and_wait(&indirect_push);
    if (status != NV_OK)
        return status;

    push->channel->conf_computing.gpu_put = new_gpu_put;

    return status;
}

// When the Confidential Computing feature is enabled, the CPU is unable to
// access and read the pushbuffer. This is because it is located in the CPR of
// vidmem in this configuration. This function allows UVM to retrieve the
// content of the pushbuffer in an encrypted form for later decryption, hence,
// simulating the original access pattern. E.g, reading timestamp semaphores.
// See also: decrypt_push().
static void encrypt_push(uvm_push_t *push)
{
    NvU64 push_protected_gpu_va;
    NvU64 push_unprotected_gpu_va;
    uvm_gpu_address_t auth_tag_gpu_va;
    uvm_channel_t *channel = push->channel;
    uvm_push_crypto_bundle_t *crypto_bundle;
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    NvU32 push_size = uvm_push_get_size(push);
    uvm_push_info_t *push_info = uvm_push_info_from_push(push);
    uvm_pushbuffer_t *pushbuffer = uvm_channel_get_pushbuffer(channel);
    unsigned auth_tag_offset = UVM_CONF_COMPUTING_AUTH_TAG_SIZE * push->push_info_index;

    if (!g_uvm_global.conf_computing_enabled)
        return;

    if (!push_info->on_complete)
        return;

    if (!uvm_channel_is_ce(channel))
        return;

    if (push_size == 0)
        return;

    UVM_ASSERT(!uvm_channel_is_wlc(channel));
    UVM_ASSERT(!uvm_channel_is_lcic(channel));
    UVM_ASSERT(channel->conf_computing.push_crypto_bundles != NULL);

    crypto_bundle = channel->conf_computing.push_crypto_bundles + push->push_info_index;
    auth_tag_gpu_va = uvm_rm_mem_get_gpu_va(channel->conf_computing.push_crypto_bundle_auth_tags, gpu, false);
    auth_tag_gpu_va.address += auth_tag_offset;

    crypto_bundle->push_size = push_size;
    push_protected_gpu_va = uvm_pushbuffer_get_gpu_va_for_push(pushbuffer, push);
    push_unprotected_gpu_va = uvm_pushbuffer_get_unprotected_gpu_va_for_push(pushbuffer, push);

    uvm_conf_computing_log_gpu_encryption(channel, &crypto_bundle->iv);
    gpu->parent->ce_hal->encrypt(push,
                                 uvm_gpu_address_virtual_unprotected(push_unprotected_gpu_va),
                                 uvm_gpu_address_virtual(push_protected_gpu_va),
                                 push_size,
                                 auth_tag_gpu_va);
}

void uvm_channel_end_push(uvm_push_t *push)
{
    uvm_channel_t *channel = push->channel;
    uvm_channel_manager_t *channel_manager = channel->pool->manager;
    uvm_pushbuffer_t *pushbuffer = channel_manager->pushbuffer;
    uvm_gpfifo_entry_t *entry;
    NvU64 semaphore_va;
    NvU64 new_tracking_value;
    NvU32 new_payload;
    NvU32 push_size;
    NvU32 cpu_put;
    NvU32 new_cpu_put;
    uvm_gpu_t *gpu = uvm_channel_get_gpu(channel);
    bool needs_sec2_work_submit = false;

    channel_pool_lock(channel->pool);

    encrypt_push(push);

    new_tracking_value = ++channel->tracking_sem.queued_value;
    new_payload = (NvU32)new_tracking_value;

    semaphore_va = uvm_channel_tracking_semaphore_get_gpu_va(channel);
    uvm_channel_tracking_semaphore_release(push, semaphore_va, new_payload);

    if (uvm_channel_is_wlc(channel) && uvm_channel_manager_is_wlc_ready(channel_manager)) {
        uvm_channel_t *paired_lcic = uvm_channel_wlc_get_paired_lcic(channel);

        gpu->parent->ce_hal->semaphore_reduction_inc(push,
                                                     paired_lcic->channel_info.gpPutGpuVa,
                                                     paired_lcic->num_gpfifo_entries - 1);
        uvm_push_set_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_GPU);
        do_semaphore_release(push,
                             paired_lcic->channel_info.workSubmissionOffsetGpuVa,
                             paired_lcic->channel_info.workSubmissionToken);

        if (uvm_push_get_size(push) < UVM_MAX_WLC_PUSH_SIZE) {
            // The UVM_MAX_WLC_PUSH_SIZE is set to fit indirect work launch
            // pushes. However, direct pushes to WLC can be smaller than this
            // size. This is used e.g. by indirect submission of control
            // GPFIFO entries.
            gpu->parent->host_hal->noop(push, UVM_MAX_WLC_PUSH_SIZE - uvm_push_get_size(push));
        }
    }

    push_size = uvm_push_get_size(push);
    UVM_ASSERT_MSG(push_size <= UVM_MAX_PUSH_SIZE, "push size %u\n", push_size);

    cpu_put = channel->cpu_put;
    new_cpu_put = (cpu_put + 1) % channel->num_gpfifo_entries;

    entry = &channel->gpfifo_entries[cpu_put];
    entry->tracking_semaphore_value = new_tracking_value;
    entry->pushbuffer_offset = uvm_pushbuffer_get_offset_for_push(pushbuffer, push);
    entry->pushbuffer_size = push_size;

    // Indirect submission via SEC2/WLC needs pushes to be aligned for
    // encryption/decryption. The pushbuffer_size of this push
    // influences starting address of the next push.
    if (g_uvm_global.conf_computing_enabled)
        entry->pushbuffer_size = UVM_ALIGN_UP(push_size, UVM_CONF_COMPUTING_BUF_ALIGNMENT);

    entry->push_info = &channel->push_infos[push->push_info_index];
    entry->type = UVM_GPFIFO_ENTRY_TYPE_NORMAL;

    UVM_ASSERT(channel->current_gpfifo_count > 0);
    --channel->current_gpfifo_count;

    if (uvm_channel_is_proxy(channel)) {
        proxy_channel_submit_work(push, push_size);
    }
    else if (uvm_channel_is_wlc(channel) && uvm_channel_manager_is_wlc_ready(channel_manager)) {
        internal_channel_submit_work_wlc(push);
    }
    else if (g_uvm_global.conf_computing_enabled && uvm_channel_is_ce(channel)) {
        if (uvm_channel_manager_is_wlc_ready(channel_manager)) {
            internal_channel_submit_work_indirect_wlc(push, cpu_put, new_cpu_put);
        }
        else {
            // submitting via SEC2 starts a push, postpone until this push is
            // ended
            needs_sec2_work_submit = true;
        }
    }
    else {
        internal_channel_submit_work(push, push_size, new_cpu_put);
    }

    channel->cpu_put = new_cpu_put;

    uvm_pushbuffer_end_push(pushbuffer, push, entry);

    // The moment the channel is unlocked uvm_channel_update_progress_with_max()
    // may notice the GPU work to be completed and hence all state tracking the
    // push must be updated before that. Notably uvm_pushbuffer_end_push() has
    // to be called first.
    unlock_channel_for_push(channel);
    channel_pool_unlock(channel->pool);

    // This memory barrier is borrowed from CUDA, as it supposedly fixes perf
    // issues on some systems. Comment from CUDA: "fixes throughput-related
    // performance problems, e.g. bugs 626179, 593841. This may be related to
    // bug 124888, which GL works around by doing a clflush"
    wmb();

    if (needs_sec2_work_submit) {
        NV_STATUS status = internal_channel_submit_work_indirect_sec2(push, cpu_put, new_cpu_put);

        // This codepath should only be used during initialization and thus
        // NEVER return an error.
        UVM_ASSERT(status == NV_OK);
    }

    push->push_info_index = channel->num_gpfifo_entries;
    push->channel_tracking_value = new_tracking_value;
}

static void submit_ctrl_gpfifo(uvm_channel_t *channel, uvm_gpfifo_entry_t *entry, NvU32 new_cpu_put)
{
    uvm_gpu_t *gpu = uvm_channel_get_gpu(channel);
    NvU32 cpu_put = channel->cpu_put;
    NvU64 *gpfifo_entry;

    UVM_ASSERT(entry == &channel->gpfifo_entries[cpu_put]);

    if (g_uvm_global.conf_computing_enabled && uvm_channel_is_ce(channel))
        return;

    gpfifo_entry = (NvU64*)channel->channel_info.gpFifoEntries + cpu_put;
    *gpfifo_entry = entry->control_value;

    // Need to make sure all the GPFIFO entries writes complete before updating
    // GPPUT. We also don't want any reads to be moved after the GPPut write as
    // the GPU might modify the data they read as soon as the GPPut write
    // happens.
    mb();

    gpu->parent->host_hal->write_gpu_put(channel, new_cpu_put);
}

static NV_STATUS submit_ctrl_gpfifo_indirect(uvm_channel_t *channel,
                                             uvm_gpfifo_entry_t *entry,
                                             NvU32 old_cpu_put,
                                             NvU32 new_gpu_put)
{
    uvm_push_t indirect_push;
    NV_STATUS status = NV_OK;
    uvm_spin_loop_t spin;
    uvm_channel_type_t indirect_channel_type = uvm_channel_manager_is_wlc_ready(channel->pool->manager) ?
                                               UVM_CHANNEL_TYPE_WLC :
                                               UVM_CHANNEL_TYPE_SEC2;

    UVM_ASSERT(uvm_channel_is_ce(channel));

    // If the old_cpu_put is not equal to the last gpu put,
    // Another push(es) is pending that needs to be submitted.
    // That push/es' submission will update the gpu_put pointer
    // to expected value.
    UVM_SPIN_WHILE(channel->conf_computing.gpu_put != old_cpu_put, &spin);

    status = uvm_push_begin(channel->pool->manager,
                            indirect_channel_type,
                            &indirect_push,
                            "GPFIFO submit to '%s' via '%s'",
                            channel->name,
                            uvm_channel_type_to_string(indirect_channel_type));
    if (status != NV_OK)
        return status;

    if (uvm_channel_is_sec2(indirect_push.channel)) {
        set_gpfifo_via_sec2(&indirect_push, channel, old_cpu_put, entry->control_value);
        update_gpput_via_sec2(&indirect_push, channel, new_gpu_put);
    } else {
        uvm_gpu_t *gpu = uvm_push_get_gpu(&indirect_push);
        NvU64 gpfifo_gpu_va = channel->channel_info.gpFifoGpuVa + (old_cpu_put * sizeof(entry->control_value));

        gpu->parent->ce_hal->memset_8(&indirect_push,
                                      uvm_gpu_address_virtual(gpfifo_gpu_va),
                                      entry->control_value,
                                      sizeof(entry->control_value));

        uvm_push_set_flag(&indirect_push, UVM_PUSH_FLAG_NEXT_MEMBAR_GPU);
        do_semaphore_release(&indirect_push, channel->channel_info.gpPutGpuVa, new_gpu_put);
    }

    uvm_push_set_flag(&indirect_push, UVM_PUSH_FLAG_NEXT_MEMBAR_GPU);
    do_semaphore_release(&indirect_push,
                         channel->channel_info.workSubmissionOffsetGpuVa,
                         channel->channel_info.workSubmissionToken);

    status = uvm_push_end_and_wait(&indirect_push);
    if (status != NV_OK)
        return status;

    channel->conf_computing.gpu_put = new_gpu_put;

    return NV_OK;
}

// The caller must submit a normal GPFIFO entry with a semaphore release
// following the control GPFIFO, refer to uvm_channel_write_ctrl_gpfifo() for an
// example.
static void write_ctrl_gpfifo(uvm_channel_t *channel, NvU64 ctrl_fifo_entry_value)
{
    uvm_gpfifo_entry_t *entry;
    NvU32 cpu_put;
    NvU32 new_cpu_put;

    channel_pool_lock(channel->pool);

    cpu_put = channel->cpu_put;
    new_cpu_put = (cpu_put + 1) % channel->num_gpfifo_entries;

    entry = &channel->gpfifo_entries[cpu_put];
    memset(entry, 0, sizeof(*entry));
    entry->type = UVM_GPFIFO_ENTRY_TYPE_CONTROL;
    entry->control_value = ctrl_fifo_entry_value;

    // Control GPFIFO entries are followed by a semaphore_release push in UVM.
    // We add the tracking semaphore value of the next GPFIFO entry,
    // potentially the associated semaphore release push. Even if a different
    // GPFIFO entry sneaks in, the purposes of signaling that this control
    // GPFIFO entry has been processed is accomplished.
    entry->tracking_semaphore_value = channel->tracking_sem.queued_value + 1;

    UVM_ASSERT(channel->current_gpfifo_count > 1);
    --channel->current_gpfifo_count;

    submit_ctrl_gpfifo(channel, entry, new_cpu_put);

    channel->cpu_put = new_cpu_put;

    // The moment the channel is unlocked uvm_channel_update_progress_with_max()
    // may notice the GPU work to be completed and hence all state tracking the
    // push must be updated before that. Note that we do not call
    // unlock_channel_for_push() because a control GPFIFO is followed by a
    // semaphore release, where the channel is unlocked.
    channel_pool_unlock(channel->pool);

    // Trigger indirect submission when needed.
    if (g_uvm_global.conf_computing_enabled && uvm_channel_is_ce(channel)) {
        NV_STATUS status = submit_ctrl_gpfifo_indirect(channel, entry, cpu_put, new_cpu_put);

        // All failures are globally fatal. There's nothing we do to recover.
        UVM_ASSERT_RELEASE(status == NV_OK);
    }

    // This memory barrier is borrowed from CUDA, as it supposedly fixes perf
    // issues on some systems. Comment from CUDA: "fixes throughput-related
    // performance problems, e.g. bugs 626179, 593841. This may be related to
    // bug 124888, which GL works around by doing a clflush"
    wmb();
}

NV_STATUS uvm_channel_write_ctrl_gpfifo(uvm_channel_t *channel, NvU64 ctrl_fifo_entry_value)
{
    NV_STATUS status;
    uvm_push_t push;

    UVM_ASSERT(!uvm_channel_is_proxy(channel));

    // WLC/LCIC channels can only process custom GPFIFO entries before
    // their schedule is set up.
    UVM_ASSERT(!uvm_channel_is_lcic(channel) || !uvm_channel_manager_is_wlc_ready(channel->pool->manager));
    UVM_ASSERT(!uvm_channel_is_wlc(channel) || !uvm_channel_manager_is_wlc_ready(channel->pool->manager));

    // We reserve two GPFIFO entries, i.e., the control GPFIFO entry and the
    // subsequent semaphore_release push. There is, however, a potential case
    // for GPFIFO control submission starvation. This may happen because a
    // GPFIFO control submission requires two available GPFIFO entries. If you
    // have a full GPFIFO ring buffer that frees one entry at a time, while
    // there is another thread consuming this recently released entry at the
    // same rate, a concurrent thread trying to reserve two entries for a GPFIFO
    // control submission may starve. We could handle this by imposing minimal
    // release entries in uvm_channel.c:uvm_channel_update_progress(). Instead,
    // we don't deal with this potential starvation case because:
    // - Control GPFIFO are rarely used.
    // - By default, we release up to 8 GPFIFO entries at a time, except if the
    //   release rate is constrained by lengthy pushbuffers -- another rare
    //   situation.
    // - It would add unnecessary complexity to channel_update_progress().
    status = uvm_channel_reserve(channel, 2);
    if (status != NV_OK)
        return status;

    if (g_uvm_global.conf_computing_enabled) {
        // Rotating IV needs to idle the channel. However, there's no semaphore
        // release after submitting a control entry. It is not possible to wait
        // for in-flight entries after the GPFIFO submission.
        // Instead, check for IV rotation early. Secure channels are locked for
        // pushes after reservation so the IV space gained here can't be used
        // up by concurrent pushes.
        status = uvm_conf_computing_maybe_rotate_channel_ivs_retry_busy(channel);
        if (status != NV_OK) {
            uvm_channel_release(channel, 2);
            return status;
        }
    }

    write_ctrl_gpfifo(channel, ctrl_fifo_entry_value);

    status = uvm_push_begin_on_reserved_channel(channel, &push, "write_ctrl_GPFIFO");
    if (status != NV_OK) {
        uvm_gpu_t *gpu = uvm_channel_get_gpu(channel);

        // One entry was consumed by GPFIFO entry
        uvm_channel_release(channel, 1);
        UVM_ERR_PRINT("Failed to begin push on channel: %s, GPU %s\n", nvstatusToString(status), uvm_gpu_name(gpu));
        return status;
    }

    // This is an empty push, the push's embedded semaphore_release signals that
    // the GPFIFO control entry has been processed.
    uvm_push_end(&push);

    return NV_OK;
}

static NV_STATUS channel_reserve_and_lock(uvm_channel_t *channel, NvU32 num_gpfifo_entries)
{
    uvm_spin_loop_t spin;
    uvm_channel_pool_t *pool = channel->pool;

    // This semaphore is uvm_up() in unlock_channel_for_push() as part of the
    // uvm_channel_end_push() routine. Note that different than in
    // channel_reserve_and_lock_in_pool, we cannot pick an unlocked channel from
    // the pool, even when there is one available and *channel is locked.
    // Not a concern given that uvm_channel_reserve() is not the common-case for
    // channel reservation, and only used for channel initialization, GPFIFO
    // control work submission, and testing.
    uvm_down(&pool->push_sem);

    channel_pool_lock(pool);

    if (test_claim_and_lock_channel(channel, num_gpfifo_entries))
        goto out;

    channel_pool_unlock(pool);

    uvm_spin_loop_init(&spin);
    while (1) {
        NV_STATUS status;

        uvm_channel_update_progress(channel);

        channel_pool_lock(pool);

        if (test_claim_and_lock_channel(channel, num_gpfifo_entries))
            goto out;

        channel_pool_unlock(pool);

        status = uvm_channel_check_errors(channel);
        if (status != NV_OK) {
            uvm_up(&pool->push_sem);
            return status;
        }

        UVM_SPIN_LOOP(&spin);
    }

out:
    channel_pool_unlock(pool);
    return NV_OK;
}

NV_STATUS uvm_channel_reserve(uvm_channel_t *channel, NvU32 num_gpfifo_entries)
{
    NV_STATUS status = NV_OK;
    uvm_spin_loop_t spin;

    if (g_uvm_global.conf_computing_enabled)
        return channel_reserve_and_lock(channel, num_gpfifo_entries);

    if (try_claim_channel(channel, num_gpfifo_entries))
        return NV_OK;

    uvm_channel_update_progress(channel);

    uvm_spin_loop_init(&spin);
    while (!try_claim_channel(channel, num_gpfifo_entries) && status == NV_OK) {
        UVM_SPIN_LOOP(&spin);
        status = uvm_channel_check_errors(channel);
        uvm_channel_update_progress(channel);
    }

    return status;
}

void uvm_channel_release(uvm_channel_t *channel, NvU32 num_gpfifo_entries)
{
    channel_pool_lock(channel->pool);

    UVM_ASSERT(uvm_channel_is_locked_for_push(channel));
    unlock_channel_for_push(channel);

    UVM_ASSERT(channel->current_gpfifo_count >= num_gpfifo_entries);
    channel->current_gpfifo_count -= num_gpfifo_entries;
    channel_pool_unlock(channel->pool);
}

// Get the first pending GPFIFO entry, if any.
// This doesn't stop the entry from being reused.
static uvm_gpfifo_entry_t *uvm_channel_get_first_pending_entry(uvm_channel_t *channel)
{
    uvm_gpfifo_entry_t *entry = NULL;
    NvU32 pending_count = channel_update_progress_all(channel, UVM_CHANNEL_UPDATE_MODE_COMPLETED);

    if (pending_count == 0)
        return NULL;

    channel_pool_lock(channel->pool);

    if (channel->gpu_get != channel->cpu_put)
        entry = &channel->gpfifo_entries[channel->gpu_get];

    channel_pool_unlock(channel->pool);

    return entry;
}

NV_STATUS uvm_channel_get_status(uvm_channel_t *channel)
{
    uvm_gpu_t *gpu;
    NvNotification *error_notifier;

    if (uvm_channel_is_proxy(channel))
        error_notifier = channel->proxy.channel_info.shadowErrorNotifier;
    else
        error_notifier = channel->channel_info.errorNotifier;

    if (error_notifier->status == 0)
        return NV_OK;

    // In case we hit a channel error, check the ECC error notifier as well so
    // that a more precise ECC error can be returned in case there is indeed an
    // ECC error.
    //
    // Notably this might be racy depending on the ordering of the notifications,
    // but we can't always call RM to service interrupts from this context.
    gpu = uvm_channel_get_gpu(channel);
    if (gpu->ecc.enabled && *gpu->ecc.error_notifier)
        return NV_ERR_ECC_ERROR;

    return NV_ERR_RC_ERROR;
}

uvm_gpfifo_entry_t *uvm_channel_get_fatal_entry(uvm_channel_t *channel)
{
    UVM_ASSERT(uvm_channel_get_status(channel) != NV_OK);

    return uvm_channel_get_first_pending_entry(channel);
}

NV_STATUS uvm_channel_check_errors(uvm_channel_t *channel)
{
    uvm_gpfifo_entry_t *fatal_entry;
    NV_STATUS status = uvm_channel_get_status(channel);

    if (status == NV_OK)
        return NV_OK;

    UVM_ERR_PRINT("Detected a channel error, channel %s GPU %s\n",
                  channel->name,
                  uvm_gpu_name(uvm_channel_get_gpu(channel)));

    fatal_entry = uvm_channel_get_fatal_entry(channel);
    if (fatal_entry != NULL) {
        if (fatal_entry->type == UVM_GPFIFO_ENTRY_TYPE_NORMAL) {
            uvm_push_info_t *push_info = fatal_entry->push_info;
            UVM_ERR_PRINT("Channel error likely caused by push '%s' started at %s:%d in %s()\n",
                          push_info->description,
                          push_info->filename,
                          push_info->line,
                          push_info->function);
        }
        else {
            UVM_ASSERT(!uvm_channel_is_proxy(channel));

            UVM_ERR_PRINT("Channel error likely caused by GPFIFO control entry, data: 0x%llx, gpu_get: %d\n",
                          fatal_entry->control_value,
                          channel->gpu_get);
        }
    }

    uvm_global_set_fatal_error(status);

    return status;
}

NV_STATUS uvm_channel_manager_check_errors(uvm_channel_manager_t *channel_manager)
{
    uvm_channel_pool_t *pool;
    NV_STATUS status = uvm_global_get_status();

    if (status != NV_OK)
        return status;

    uvm_for_each_pool(pool, channel_manager) {
        uvm_channel_t *channel;

        uvm_for_each_channel_in_pool(channel, pool) {
            status = uvm_channel_check_errors(channel);
            if (status != NV_OK)
                return status;
        }
    }

    return status;
}

bool uvm_channel_is_value_completed(uvm_channel_t *channel, NvU64 value)
{
    return uvm_gpu_tracking_semaphore_is_value_completed(&channel->tracking_sem, value);
}

NvU64 uvm_channel_update_completed_value(uvm_channel_t *channel)
{
    return uvm_gpu_tracking_semaphore_update_completed_value(&channel->tracking_sem);
}

NV_STATUS uvm_channel_wait(uvm_channel_t *channel)
{
    NV_STATUS status = uvm_global_get_status();
    uvm_spin_loop_t spin;

    if (uvm_channel_update_progress(channel) == 0 && status == NV_OK)
        return uvm_channel_check_errors(channel);

    uvm_spin_loop_init(&spin);
    while (uvm_channel_update_progress(channel) > 0 && status == NV_OK) {
        UVM_SPIN_LOOP(&spin);
        status = uvm_global_get_status();

        if (status == NV_OK)
            status = uvm_channel_check_errors(channel);
    }

    return status;
}

static NV_STATUS csl_init(uvm_channel_t *channel)
{
    NV_STATUS status;

    UVM_ASSERT(g_uvm_global.conf_computing_enabled);

    status = uvm_rm_locked_call(nvUvmInterfaceCslInitContext(&channel->csl.ctx, channel->handle));
    if (status != NV_OK) {
        uvm_gpu_t *gpu = uvm_channel_get_gpu(channel);

        UVM_DBG_PRINT("nvUvmInterfaceCslInitContext() failed: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_gpu_name(gpu));
        return status;
    }

    uvm_mutex_init(&channel->csl.ctx_lock, UVM_LOCK_ORDER_CSL_CTX);
    channel->csl.is_ctx_initialized = true;

    return NV_OK;
}

static void csl_destroy(uvm_channel_t *channel)
{
    if (!channel->csl.is_ctx_initialized)
        return;

    uvm_assert_mutex_unlocked(&channel->csl.ctx_lock);
    UVM_ASSERT(!uvm_channel_is_locked_for_push(channel));

    uvm_rm_locked_call_void(nvUvmInterfaceDeinitCslContext(&channel->csl.ctx));
    channel->csl.is_ctx_initialized = false;
}

static void free_conf_computing_buffers(uvm_channel_t *channel)
{
    UVM_ASSERT(g_uvm_global.conf_computing_enabled);
    UVM_ASSERT(uvm_channel_is_ce(channel));

    uvm_rm_mem_free(channel->conf_computing.static_pb_protected_vidmem);
    uvm_rm_mem_free(channel->conf_computing.static_pb_unprotected_sysmem);
    uvm_rm_mem_free(channel->conf_computing.static_notifier_unprotected_sysmem);
    uvm_rm_mem_free(channel->conf_computing.push_crypto_bundle_auth_tags);
    uvm_kvfree(channel->conf_computing.static_pb_protected_sysmem);
    uvm_kvfree(channel->conf_computing.push_crypto_bundles);
    channel->conf_computing.static_pb_protected_vidmem = NULL;
    channel->conf_computing.static_pb_unprotected_sysmem = NULL;
    channel->conf_computing.static_notifier_unprotected_sysmem = NULL;
    channel->conf_computing.push_crypto_bundle_auth_tags = NULL;
    channel->conf_computing.static_pb_protected_sysmem = NULL;
    channel->conf_computing.push_crypto_bundles = NULL;

    uvm_kvfree(channel->tracking_sem.semaphore.conf_computing.ivs);
    channel->tracking_sem.semaphore.conf_computing.ivs = NULL;
}

static NV_STATUS alloc_conf_computing_buffers_semaphore(uvm_channel_t *channel)
{
    uvm_gpu_semaphore_t *semaphore = &channel->tracking_sem.semaphore;

    UVM_ASSERT(g_uvm_global.conf_computing_enabled);
    UVM_ASSERT(uvm_channel_is_ce(channel));

    semaphore->conf_computing.ivs = uvm_kvmalloc_zero(sizeof(*semaphore->conf_computing.ivs)
                                                      * channel->num_gpfifo_entries);

    if (!semaphore->conf_computing.ivs)
        return NV_ERR_NO_MEMORY;

    return NV_OK;
}

static NV_STATUS alloc_conf_computing_buffers_wlc(uvm_channel_t *channel)
{
    uvm_gpu_t *gpu = uvm_channel_get_gpu(channel);
    size_t aligned_wlc_push_size = UVM_ALIGN_UP(UVM_MAX_WLC_PUSH_SIZE, UVM_CONF_COMPUTING_AUTH_TAG_ALIGNMENT);
    NV_STATUS status = uvm_rm_mem_alloc_and_map_cpu(gpu,
                                                    UVM_RM_MEM_TYPE_SYS,
                                                    aligned_wlc_push_size + UVM_CONF_COMPUTING_AUTH_TAG_SIZE * 2,
                                                    PAGE_SIZE,
                                                    &channel->conf_computing.static_pb_unprotected_sysmem);
    if (status != NV_OK)
        return status;

    // Both pushes will be targets for SEC2 decrypt operations and have to
    // be aligned for SEC2. The first push location will also be a target
    // for CE decrypt operation and has to be aligned for CE decrypt.
    status = uvm_rm_mem_alloc(gpu,
                              UVM_RM_MEM_TYPE_GPU,
                              UVM_ALIGN_UP(UVM_MAX_WLC_PUSH_SIZE, UVM_CONF_COMPUTING_SEC2_BUF_ALIGNMENT) * 2,
                              UVM_CONF_COMPUTING_BUF_ALIGNMENT,
                              &channel->conf_computing.static_pb_protected_vidmem);
    if (status != NV_OK)
        return status;

    channel->conf_computing.static_pb_unprotected_sysmem_cpu =
        uvm_rm_mem_get_cpu_va(channel->conf_computing.static_pb_unprotected_sysmem);
    channel->conf_computing.static_pb_unprotected_sysmem_auth_tag_cpu =
        (char*)channel->conf_computing.static_pb_unprotected_sysmem_cpu + aligned_wlc_push_size;

    // The location below is only used for launch pushes but reuses
    // the same sysmem allocation
    channel->conf_computing.launch_auth_tag_cpu =
        (char*)channel->conf_computing.static_pb_unprotected_sysmem_cpu +
        aligned_wlc_push_size + UVM_CONF_COMPUTING_AUTH_TAG_SIZE;
    channel->conf_computing.launch_auth_tag_gpu_va =
        uvm_rm_mem_get_gpu_uvm_va(channel->conf_computing.static_pb_unprotected_sysmem, gpu) +
        aligned_wlc_push_size + UVM_CONF_COMPUTING_AUTH_TAG_SIZE;

    channel->conf_computing.static_pb_protected_sysmem = uvm_kvmalloc(UVM_MAX_WLC_PUSH_SIZE + UVM_PAGE_SIZE_4K);
    if (!channel->conf_computing.static_pb_protected_sysmem)
        return NV_ERR_NO_MEMORY;

    return status;
}

static NV_STATUS alloc_conf_computing_buffers_lcic(uvm_channel_t *channel)
{
    uvm_gpu_t *gpu = uvm_channel_get_gpu(channel);
    const size_t notifier_size = sizeof(*channel->conf_computing.static_notifier_entry_unprotected_sysmem_cpu);
    NV_STATUS status = uvm_rm_mem_alloc_and_map_cpu(gpu,
                                                    UVM_RM_MEM_TYPE_SYS,
                                                    notifier_size * 2,
                                                    UVM_CONF_COMPUTING_BUF_ALIGNMENT,
                                                    &channel->conf_computing.static_notifier_unprotected_sysmem);
    if (status != NV_OK)
        return status;

    status = uvm_rm_mem_alloc(gpu,
                              UVM_RM_MEM_TYPE_GPU,
                              UVM_LCIC_PUSH_SIZE,
                              UVM_CONF_COMPUTING_BUF_ALIGNMENT,
                              &channel->conf_computing.static_pb_protected_vidmem);
    if (status != NV_OK)
        return status;

    channel->conf_computing.static_notifier_entry_unprotected_sysmem_cpu =
        uvm_rm_mem_get_cpu_va(channel->conf_computing.static_notifier_unprotected_sysmem);
    channel->conf_computing.static_notifier_exit_unprotected_sysmem_cpu =
        channel->conf_computing.static_notifier_entry_unprotected_sysmem_cpu + 1;

    channel->conf_computing.static_notifier_entry_unprotected_sysmem_gpu_va =
        uvm_rm_mem_get_gpu_va(channel->conf_computing.static_notifier_unprotected_sysmem, gpu, false);
    channel->conf_computing.static_notifier_exit_unprotected_sysmem_gpu_va =
        channel->conf_computing.static_notifier_entry_unprotected_sysmem_gpu_va;
    channel->conf_computing.static_notifier_exit_unprotected_sysmem_gpu_va.address += notifier_size;

    return status;
}

static NV_STATUS alloc_conf_computing_buffers(uvm_channel_t *channel)
{
    NV_STATUS status;

    UVM_ASSERT(g_uvm_global.conf_computing_enabled);
    UVM_ASSERT(uvm_channel_is_ce(channel));

    status = alloc_conf_computing_buffers_semaphore(channel);
    if (status != NV_OK)
        return status;

    if (uvm_channel_is_wlc(channel)) {
        status = alloc_conf_computing_buffers_wlc(channel);
    }
    else if (uvm_channel_is_lcic(channel)) {
        status = alloc_conf_computing_buffers_lcic(channel);
    }
    else {
        uvm_gpu_t *gpu = uvm_channel_get_gpu(channel);
        void *push_crypto_bundles = uvm_kvmalloc_zero(sizeof(*channel->conf_computing.push_crypto_bundles) *
                                                      channel->num_gpfifo_entries);

        if (push_crypto_bundles == NULL)
            return NV_ERR_NO_MEMORY;

        channel->conf_computing.push_crypto_bundles = push_crypto_bundles;

        status = uvm_rm_mem_alloc_and_map_cpu(gpu,
                                              UVM_RM_MEM_TYPE_SYS,
                                              channel->num_gpfifo_entries * UVM_CONF_COMPUTING_AUTH_TAG_SIZE,
                                              UVM_CONF_COMPUTING_BUF_ALIGNMENT,
                                              &channel->conf_computing.push_crypto_bundle_auth_tags);
    }

    return status;
}

static void channel_destroy(uvm_channel_pool_t *pool, uvm_channel_t *channel)
{
    UVM_ASSERT(pool->num_channels > 0);

    if (channel->tracking_sem.queued_value > 0) {
        // The channel should have been idled before being destroyed, unless an
        // error was triggered. We need to check both error cases (global and
        // channel) to handle the UVM_TEST_CHANNEL_SANITY unit test.
        if (uvm_global_get_status() == NV_OK && uvm_channel_get_status(channel) == NV_OK)
            UVM_ASSERT(uvm_gpu_tracking_semaphore_is_completed(&channel->tracking_sem));

        // Remove all remaining GPFIFOs from their pushbuffer chunk, since the
        // pushbuffer has a longer lifetime.
        channel_update_progress_all(channel, UVM_CHANNEL_UPDATE_MODE_FORCE_ALL);
    }

    proc_remove(channel->procfs.pushes);
    proc_remove(channel->procfs.info);
    proc_remove(channel->procfs.dir);

    uvm_kvfree(channel->push_acquire_infos);
    uvm_kvfree(channel->push_infos);

    uvm_kvfree(channel->gpfifo_entries);

    if (g_uvm_global.conf_computing_enabled) {
        csl_destroy(channel);

        if (uvm_channel_is_ce(channel))
            free_conf_computing_buffers(channel);
    }

    if (uvm_channel_is_proxy(channel))
        uvm_rm_locked_call_void(nvUvmInterfacePagingChannelDestroy(channel->proxy.handle));
    else
        uvm_rm_locked_call_void(nvUvmInterfaceChannelDestroy(channel->handle));

    uvm_gpu_tracking_semaphore_free(&channel->tracking_sem);

    UVM_ASSERT(list_empty(&channel->tools.channel_list_node));
    UVM_ASSERT(channel->tools.pending_event_count == 0);

    pool->num_channels--;
}

static unsigned channel_pool_type_num_gpfifo_entries(uvm_channel_manager_t *manager, uvm_channel_pool_type_t pool_type)
{
    switch (pool_type) {
        case UVM_CHANNEL_POOL_TYPE_CE:
        case UVM_CHANNEL_POOL_TYPE_CE_PROXY:
            return manager->conf.num_gpfifo_entries;
        case UVM_CHANNEL_POOL_TYPE_SEC2:
            return manager->conf.num_gpfifo_entries;
        case UVM_CHANNEL_POOL_TYPE_WLC: {
            // WLC benefits from larger number of entries since more available
            // entries result in less frequent calls to
            // uvm_channel_update_progress 16 is the maximum size that can
            // re-use static pb preallocated memory when uploading the WLC
            // schedule.
            return 16;
        }
        case UVM_CHANNEL_POOL_TYPE_LCIC: {
            // Every channel needs at least 3 entries; 1 for sentinel and 2 more
            // for submitting GPFIFO control entries. The number also has to be
            // power of 2, as the HW stores the size as log2 value.
            // LCIC does not accept external pushes, uvm_channel_update_progress
            // is not a concern.
            return 4;
        }
        default:
            UVM_ASSERT_MSG(0, "Unhandled pool type: %d", pool_type);
            return 0;
    }
}

// Returns the TSG for a given channel.
static uvmGpuTsgHandle channel_get_tsg(uvm_channel_t *channel)
{
    unsigned tsg_index = 0;
    uvm_channel_pool_t *pool = channel->pool;

    if (uvm_channel_pool_is_wlc(pool) || uvm_channel_pool_is_lcic(pool)) {
        if (uvm_channel_pool_is_lcic(pool)) {
            channel = uvm_channel_lcic_get_paired_wlc(channel);
            pool = channel->pool;
        }

        tsg_index = uvm_channel_index_in_pool(channel);
    }
    UVM_ASSERT(tsg_index < pool->num_tsgs);

    return pool->tsg_handles[tsg_index];
}

static NV_STATUS internal_channel_create(uvm_channel_t *channel)
{
    NV_STATUS status;
    UvmGpuChannelAllocParams channel_alloc_params;
    UvmGpuChannelInfo *channel_info = &channel->channel_info;
    uvm_channel_manager_t *manager = channel->pool->manager;

    memset(&channel_alloc_params, 0, sizeof(channel_alloc_params));
    channel_alloc_params.numGpFifoEntries = channel_pool_type_num_gpfifo_entries(manager, channel->pool->pool_type);
    channel_alloc_params.gpFifoLoc = manager->conf.gpfifo_loc;
    channel_alloc_params.gpPutLoc = manager->conf.gpput_loc;

    if (uvm_channel_is_sec2(channel)) {
        UVM_ASSERT(g_uvm_global.conf_computing_enabled);

        // SEC2 channels' GPPUT and GPFIFO must be allocated in sysmem.
        channel_alloc_params.gpFifoLoc = UVM_BUFFER_LOCATION_SYS;
        channel_alloc_params.gpPutLoc = UVM_BUFFER_LOCATION_SYS;
    }

    status = uvm_rm_locked_call(nvUvmInterfaceChannelAllocate(channel_get_tsg(channel),
                                                              &channel_alloc_params,
                                                              &channel->handle,
                                                              channel_info));
    if (status != NV_OK) {
        UVM_ERR_PRINT("nvUvmInterfaceChannelAllocate() failed: %s, GPU %s, type %s\n",
                      nvstatusToString(status),
                      uvm_gpu_name(manager->gpu),
                      uvm_channel_pool_type_to_string(channel->pool->pool_type));
        return status;
    }

    snprintf(channel->name,
             sizeof(channel->name),
             "ID %u:%u (0x%x:0x%x) %s %u",
             channel_info->hwRunlistId,
             channel_info->hwChannelId,
             channel_info->hwRunlistId,
             channel_info->hwChannelId,
             uvm_channel_is_sec2(channel) ? "SEC2" :
             uvm_channel_is_wlc(channel) ? "WLC" :
             uvm_channel_is_lcic(channel) ? "LCIC" : "CE",
             channel->pool->engine_index);

    return NV_OK;
}

static NV_STATUS proxy_channel_create(uvm_channel_t *channel, unsigned ce_index)
{
    NV_STATUS status;
    unsigned proxy_index;
    UvmGpuPagingChannelAllocParams channel_alloc_params;
    uvm_channel_manager_t *manager = channel->pool->manager;
    uvm_gpu_t *gpu = manager->gpu;

    UVM_ASSERT(uvm_channel_is_proxy(channel));

    memset(&channel_alloc_params, 0, sizeof(channel_alloc_params));
    channel_alloc_params.engineIndex = ce_index;

    status = uvm_rm_locked_call(nvUvmInterfacePagingChannelAllocate(uvm_gpu_device_handle(gpu),
                                                                    &channel_alloc_params,
                                                                    &channel->proxy.handle,
                                                                    &channel->proxy.channel_info));
    if (status != NV_OK) {
        UVM_ERR_PRINT("nvUvmInterfacePagingChannelAllocate() failed: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_gpu_name(gpu));
        return status;
    }

    proxy_index = uvm_channel_index_in_pool(channel);
    snprintf(channel->name, sizeof(channel->name), "Proxy %u CE %u", proxy_index, ce_index);

    return NV_OK;
}

static NV_STATUS channel_create(uvm_channel_pool_t *pool, uvm_channel_t *channel)
{
    NV_STATUS status;
    uvm_channel_manager_t *manager = pool->manager;
    uvm_gpu_t *gpu = manager->gpu;
    uvm_gpu_semaphore_pool_t *semaphore_pool = gpu->semaphore_pool;
    unsigned int i;

    UVM_ASSERT(channel != NULL);

    channel->pool = pool;
    pool->num_channels++;
    INIT_LIST_HEAD(&channel->available_push_infos);
    channel->tools.pending_event_count = 0;
    INIT_LIST_HEAD(&channel->tools.channel_list_node);

    if (g_uvm_global.conf_computing_enabled && uvm_channel_is_ce(channel))
        semaphore_pool = gpu->secure_semaphore_pool;

    status = uvm_gpu_tracking_semaphore_alloc(semaphore_pool, &channel->tracking_sem);
    if (status != NV_OK) {
        UVM_ERR_PRINT("uvm_gpu_tracking_semaphore_alloc() failed: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_gpu_name(gpu));
        goto error;
    }

     if (uvm_channel_is_proxy(channel))
         status = proxy_channel_create(channel, pool->engine_index);
     else
         status = internal_channel_create(channel);

     if (status != NV_OK)
         goto error;

    channel->num_gpfifo_entries = channel_pool_type_num_gpfifo_entries(manager, pool->pool_type);
    channel->gpfifo_entries = uvm_kvmalloc_zero(sizeof(*channel->gpfifo_entries) * channel->num_gpfifo_entries);
    if (channel->gpfifo_entries == NULL) {
        status = NV_ERR_NO_MEMORY;
        goto error;
    }

    if (g_uvm_global.conf_computing_enabled) {
        status = csl_init(channel);
        if (status != NV_OK)
            goto error;

        if (uvm_channel_is_ce(channel)) {
            // Must happen after the channel's num_gpfifo_entries is known, as
            // UVM needs one IV slot per GPFIFO entry.
            status = alloc_conf_computing_buffers(channel);
            if (status != NV_OK)
                goto error;
        }
    }

    channel->push_infos = uvm_kvmalloc_zero(sizeof(*channel->push_infos) * channel->num_gpfifo_entries);
    if (channel->push_infos == NULL) {
        status = NV_ERR_NO_MEMORY;
        goto error;
    }

    if (uvm_push_info_is_tracking_acquires()) {
        channel->push_acquire_infos = uvm_kvmalloc_zero(sizeof(*channel->push_acquire_infos) * channel->num_gpfifo_entries);
        if (channel->push_acquire_infos == NULL) {
            status = NV_ERR_NO_MEMORY;
            goto error;
        }
    }

    for (i = 0; i < channel->num_gpfifo_entries; i++)
        list_add_tail(&channel->push_infos[i].available_list_node, &channel->available_push_infos);

    status = channel_create_procfs(channel);
    if (status != NV_OK)
        goto error;

    return NV_OK;

error:
    channel_destroy(pool, channel);

    return status;
}

NvU64 uvm_channel_tracking_semaphore_get_gpu_va_in_channel(uvm_channel_t *semaphore_channel,
                                                           uvm_channel_t *access_channel)
{
    uvm_gpu_semaphore_t *semaphore = &semaphore_channel->tracking_sem.semaphore;
    uvm_gpu_t *gpu = uvm_channel_get_gpu(access_channel);

    return uvm_gpu_semaphore_get_gpu_va(semaphore, gpu, uvm_channel_is_proxy(access_channel));
}

static NV_STATUS channel_init(uvm_channel_t *channel)
{
    uvm_push_t push;
    uvm_gpu_t *gpu = uvm_channel_get_gpu(channel);
    NV_STATUS status;
    NvU32 num_entries = 1;

    if (uvm_parent_gpu_needs_pushbuffer_segments(gpu->parent))
        num_entries++;

    status = uvm_channel_reserve(channel, num_entries);
    if (status != NV_OK)
        return status;

    if (uvm_parent_gpu_needs_pushbuffer_segments(gpu->parent)) {
        NvU64 gpfifo_entry;
        uvm_pushbuffer_t *pushbuffer = uvm_channel_get_pushbuffer(channel);
        NvU64 pb_base = uvm_pushbuffer_get_gpu_va_base(pushbuffer);

        if (uvm_channel_is_sec2(channel))
            pb_base = uvm_pushbuffer_get_sec2_gpu_va_base(pushbuffer);
        else if (channel->conf_computing.static_pb_protected_vidmem)
            pb_base = uvm_rm_mem_get_gpu_uvm_va(channel->conf_computing.static_pb_protected_vidmem, gpu);

        gpu->parent->host_hal->set_gpfifo_pushbuffer_segment_base(&gpfifo_entry, pb_base);
        write_ctrl_gpfifo(channel, gpfifo_entry);
    }

    status = uvm_push_begin_on_reserved_channel(channel, &push, "Init channel");
    if (status != NV_OK) {

        // One entry was consumed by control GPFIFO entry above, release the
        // second one.
        uvm_channel_release(channel, 1);
        UVM_ERR_PRINT("Failed to begin push on channel: %s, GPU %s\n", nvstatusToString(status), uvm_gpu_name(gpu));
        return status;
    }

    if (uvm_channel_is_ce(channel))
        gpu->parent->ce_hal->init(&push);
    else
        gpu->parent->sec2_hal->init(&push);

    gpu->parent->host_hal->init(&push);

    status = uvm_push_end_and_wait(&push);
    if (status != NV_OK) {
        UVM_ERR_PRINT("Channel '%s' init failed: %s, GPU %s\n",
                      channel->name,
                      nvstatusToString(status),
                      uvm_gpu_name(gpu));
    }

    return status;
}

static bool channel_manager_uses_proxy_pool(uvm_channel_manager_t *manager)
{
    return uvm_parent_gpu_is_virt_mode_sriov_heavy(manager->gpu->parent);
}

// Number of channels to create in a pool of the given type.
//
// TODO: Bug 1764958: Tweak this function after benchmarking real workloads.
static unsigned channel_pool_type_num_channels(uvm_channel_pool_type_t pool_type)
{
    // TODO: Bug 3387454: The vGPU plugin implementation supports a single
    // proxy channel per GPU
    if (pool_type == UVM_CHANNEL_POOL_TYPE_CE_PROXY)
        return 1;

    // Not all GPU architectures support more than 1 channel per TSG. Since SEC2
    // is not in UVM critical path for performance, we conservatively create a
    // pool/TSG with a single channel.
    if (pool_type == UVM_CHANNEL_POOL_TYPE_SEC2)
        return 1;

    if (pool_type == UVM_CHANNEL_POOL_TYPE_WLC || pool_type == UVM_CHANNEL_POOL_TYPE_LCIC)
        return UVM_PUSH_MAX_CONCURRENT_PUSHES;

    return 2;
}

// Number of TSGs to create in a pool of a given type.
static unsigned channel_pool_type_num_tsgs(uvm_channel_pool_type_t pool_type)
{
    // For WLC and LCIC channels, we create one TSG per WLC/LCIC channel pair.
    // The TSG is stored in the WLC pool.
    if (pool_type == UVM_CHANNEL_POOL_TYPE_WLC)
        return channel_pool_type_num_channels(pool_type);
    else if (pool_type == UVM_CHANNEL_POOL_TYPE_LCIC)
        return 0;

    return 1;
}

static UVM_GPU_CHANNEL_ENGINE_TYPE pool_type_to_engine_type(uvm_channel_pool_type_t pool_type)
{
    if (pool_type ==  UVM_CHANNEL_POOL_TYPE_SEC2)
        return UVM_GPU_CHANNEL_ENGINE_TYPE_SEC2;
    return UVM_GPU_CHANNEL_ENGINE_TYPE_CE;
}

static void tsg_destroy(uvm_channel_pool_t *pool, uvmGpuTsgHandle tsg_handle)
{
    UVM_ASSERT(pool->num_tsgs > 0);

    uvm_rm_locked_call_void(nvUvmInterfaceTsgDestroy(tsg_handle));
    pool->num_tsgs--;
}

static NV_STATUS tsg_create(uvm_channel_pool_t *pool, uvmGpuTsgHandle *tsg_handle)
{
    NV_STATUS status;
    UvmGpuTsgAllocParams tsg_alloc_params;
    uvm_gpu_t *gpu = pool->manager->gpu;

    pool->num_tsgs++;

    tsg_alloc_params.engineType = pool_type_to_engine_type(pool->pool_type);
    tsg_alloc_params.engineIndex = pool->engine_index;

    status = uvm_rm_locked_call(nvUvmInterfaceTsgAllocate(gpu->rm_address_space, &tsg_alloc_params, tsg_handle));
    if (status != NV_OK) {
        UVM_ERR_PRINT("nvUvmInterfaceTsgAllocate() failed: %s, GPU %s, type %s\n",
                      nvstatusToString(status),
                      uvm_gpu_name(gpu),
                      uvm_channel_pool_type_to_string(pool->pool_type));
        goto error;
    }

    return NV_OK;

error:
    tsg_destroy(pool, *tsg_handle);

    return status;
}

static void channel_pool_destroy(uvm_channel_pool_t *pool)
{
    UVM_ASSERT(pool->manager->num_channel_pools > 0);

    while (pool->num_channels > 0)
        channel_destroy(pool, pool->channels + pool->num_channels - 1);
    uvm_kvfree(pool->channels);
    pool->channels = NULL;

    while (pool->num_tsgs > 0)
        tsg_destroy(pool, *(pool->tsg_handles + pool->num_tsgs - 1));
    uvm_kvfree(pool->tsg_handles);
    pool->tsg_handles = NULL;

    pool->manager->num_channel_pools--;
}

static NV_STATUS channel_pool_add(uvm_channel_manager_t *channel_manager,
                                  uvm_channel_pool_type_t pool_type,
                                  unsigned engine_index,
                                  uvm_channel_pool_t **pool_out)
{
    NV_STATUS status;
    unsigned i;
    unsigned num_channels;
    unsigned num_tsgs;
    uvm_channel_pool_t *pool;

    UVM_ASSERT(uvm_pool_type_is_valid(pool_type));

    pool = channel_manager->channel_pools + channel_manager->num_channel_pools;
    channel_manager->num_channel_pools++;

    pool->manager = channel_manager;
    pool->engine_index = engine_index;
    pool->pool_type = pool_type;

    num_tsgs = channel_pool_type_num_tsgs(pool_type);
    if (num_tsgs != 0) {
        pool->tsg_handles = uvm_kvmalloc_zero(sizeof(*pool->tsg_handles) * num_tsgs);
        if (!pool->tsg_handles) {
            status = NV_ERR_NO_MEMORY;
            goto error;
        }

        for (i = 0; i < num_tsgs; i++) {
            uvmGpuTsgHandle *tsg_handle = pool->tsg_handles + i;

            status = tsg_create(pool, tsg_handle);
            if (status != NV_OK)
                goto error;
        }
    }

    channel_pool_lock_init(pool);

    num_channels = channel_pool_type_num_channels(pool_type);
    UVM_ASSERT(num_channels <= UVM_CHANNEL_MAX_NUM_CHANNELS_PER_POOL);

    if (g_uvm_global.conf_computing_enabled) {
        // Use different order lock for SEC2 and WLC channels.
        // This allows reserving a SEC2 or WLC channel for indirect work
        // submission while holding a reservation for a channel.
        uvm_lock_order_t order = uvm_channel_pool_is_sec2(pool) ? UVM_LOCK_ORDER_CSL_SEC2_PUSH :
                                 (uvm_channel_pool_is_wlc(pool) ? UVM_LOCK_ORDER_CSL_WLC_PUSH :
                                                                  UVM_LOCK_ORDER_CSL_PUSH);

        uvm_sema_init(&pool->push_sem, num_channels, order);
    }

    pool->channels = uvm_kvmalloc_zero(sizeof(*pool->channels) * num_channels);
    if (!pool->channels) {
        status = NV_ERR_NO_MEMORY;
        goto error;
    }

    for (i = 0; i < num_channels; i++) {
        uvm_channel_t *channel = pool->channels + i;

        status = channel_create(pool, channel);
        if (status != NV_OK)
            goto error;

        status = channel_init(channel);
        if (status != NV_OK)
            goto error;
    }

    *pool_out = pool;
    return NV_OK;

 error:
    channel_pool_destroy(pool);
    return status;
}

static bool ce_is_usable(const UvmGpuCopyEngineCaps *cap)
{
    return cap->supported && !cap->grce;
}

// Check that all asynchronous CEs are usable, and that there is at least one
// such CE.
static NV_STATUS ces_validate(uvm_channel_manager_t *manager, const UvmGpuCopyEngineCaps *ces_caps)
{
    unsigned ce;
    bool found_usable_ce = false;

    for (ce = 0; ce < UVM_COPY_ENGINE_COUNT_MAX; ++ce) {
        const UvmGpuCopyEngineCaps *ce_caps = ces_caps + ce;

        if (!ce_is_usable(ce_caps))
            continue;

        found_usable_ce = true;

        // All channels may need to release their semaphore to sysmem.
        // All CEs are expected to have the sysmem flag set.
        if (!ce_caps->sysmem)
            return NV_ERR_NOT_SUPPORTED;

        // While P2P capabilities are only required for transfers between GPUs,
        // in practice all CEs are expected to have the corresponding flag set.
        if (!ce_caps->p2p)
            return NV_ERR_NOT_SUPPORTED;
    }

    if (!found_usable_ce)
        return NV_ERR_NOT_SUPPORTED;

    return NV_OK;
}

static unsigned ce_usage_count(NvU32 ce, const unsigned *preferred_ce)
{
    unsigned i;
    unsigned count = 0;

    UVM_ASSERT(ce < UVM_COPY_ENGINE_COUNT_MAX);

    for (i = 0; i < UVM_CHANNEL_TYPE_CE_COUNT; i++) {
        if (ce == preferred_ce[i])
            count++;
    }

    return count;
}

// Returns negative if the first CE should be considered better than the second
static int compare_ce_for_channel_type(const UvmGpuCopyEngineCaps *ce_caps,
                                       uvm_channel_type_t type,
                                       NvU32 ce_index0,
                                       NvU32 ce_index1,
                                       NvU32 *preferred_ce)
{
    unsigned ce0_usage, ce1_usage;
    const UvmGpuCopyEngineCaps *cap0 = ce_caps + ce_index0;
    const UvmGpuCopyEngineCaps *cap1 = ce_caps + ce_index1;

    UVM_ASSERT(ce_index0 < UVM_COPY_ENGINE_COUNT_MAX);
    UVM_ASSERT(ce_index1 < UVM_COPY_ENGINE_COUNT_MAX);
    UVM_ASSERT(ce_index0 != ce_index1);

    switch (type) {
        // For CPU to GPU fast sysmem read is the most important
        case UVM_CHANNEL_TYPE_CPU_TO_GPU:
            if (cap0->sysmemRead != cap1->sysmemRead)
                return cap1->sysmemRead - cap0->sysmemRead;

            // Prefer not to take up the CEs for nvlink P2P
            if (cap0->nvlinkP2p != cap1->nvlinkP2p)
                return cap0->nvlinkP2p - cap1->nvlinkP2p;

            break;

        // For GPU to CPU fast sysmem write is the most important
        case UVM_CHANNEL_TYPE_GPU_TO_CPU:
            if (cap0->sysmemWrite != cap1->sysmemWrite)
                return cap1->sysmemWrite - cap0->sysmemWrite;

            // Prefer not to take up the CEs for nvlink P2P
            if (cap0->nvlinkP2p != cap1->nvlinkP2p)
                return cap0->nvlinkP2p - cap1->nvlinkP2p;

            break;

        // For GPU to GPU prefer the LCE with the most PCEs
        case UVM_CHANNEL_TYPE_GPU_TO_GPU:
            {
                int pce_diff = (int)hweight32(cap1->cePceMask) - (int)hweight32(cap0->cePceMask);

                if (pce_diff != 0)
                    return pce_diff;
            }

            break;

        // For GPU_INTERNAL we want the max possible bandwidth for CEs. For now
        // assume that the number of PCEs is a good measure.
        // TODO: Bug 1735254: Add a direct CE query for local FB bandwidth
        case UVM_CHANNEL_TYPE_GPU_INTERNAL:
            {
                int pce_diff = (int)hweight32(cap1->cePceMask) - (int)hweight32(cap0->cePceMask);

                if (pce_diff != 0)
                    return pce_diff;
            }

            // Leave P2P CEs to the GPU_TO_GPU channel type, when possible
            if (cap0->nvlinkP2p != cap1->nvlinkP2p)
                return cap0->nvlinkP2p - cap1->nvlinkP2p;

            break;

        // For MEMOPS we mostly care about latency which should be better with
        // less used CEs (although we only know about our own usage and not
        // system-wide) so just break out to get the default ordering which
        // prioritizes usage count.
        case UVM_CHANNEL_TYPE_MEMOPS:
        // For WLC we only care about using a dedicated CE, which requires
        // knowing the global CE mappings. For now just rely on the default
        // ordering, which results on selecting an unused CE (if available).
        case UVM_CHANNEL_TYPE_WLC:
            break;

        default:
            UVM_ASSERT_MSG(false, "Unexpected channel type 0x%x\n", type);
            return 0;
    }

    // By default, prefer less used CEs (within the UVM driver at least)
    ce0_usage = ce_usage_count(ce_index0, preferred_ce);
    ce1_usage = ce_usage_count(ce_index1, preferred_ce);

    if (ce0_usage != ce1_usage)
        return ce0_usage - ce1_usage;

    // And CEs that don't share PCEs
    if (cap0->shared != cap1->shared)
        return cap0->shared - cap1->shared;

    // Last resort, just order by index
    return ce_index0 - ce_index1;
}

// Select the preferred CE for the given channel types.
static void pick_ces_for_channel_types(uvm_channel_manager_t *manager,
                                       const UvmGpuCopyEngineCaps *ce_caps,
                                       uvm_channel_type_t *channel_types,
                                       unsigned num_channel_types,
                                       unsigned *preferred_ce)
{
    unsigned i;

    // In Confidential Computing, do not mark all usable CEs, only the preferred
    // ones, because non-preferred CE channels are guaranteed to not be used.
    bool mark_all_usable_ces = !g_uvm_global.conf_computing_enabled;

    for (i = 0; i < num_channel_types; ++i) {
        unsigned ce;
        unsigned best_ce = UVM_COPY_ENGINE_COUNT_MAX;
        uvm_channel_type_t type = channel_types[i];

        for (ce = 0; ce < UVM_COPY_ENGINE_COUNT_MAX; ++ce) {
            if (!ce_is_usable(ce_caps + ce))
                continue;

            if (mark_all_usable_ces)
                __set_bit(ce, manager->ce_mask);

            if (best_ce == UVM_COPY_ENGINE_COUNT_MAX) {
                best_ce = ce;
                continue;
            }

            if (compare_ce_for_channel_type(ce_caps, type, ce, best_ce, preferred_ce) < 0)
                best_ce = ce;
        }

        UVM_ASSERT(best_ce != UVM_COPY_ENGINE_COUNT_MAX);

        preferred_ce[type] = best_ce;

        // Preferred CEs are always marked as usable.
        if (type < UVM_CHANNEL_TYPE_CE_COUNT)
            __set_bit(best_ce, manager->ce_mask);
    }
}

static void pick_ces(uvm_channel_manager_t *manager, const UvmGpuCopyEngineCaps *ce_caps, unsigned *preferred_ce)
{
    // The order of picking CEs for each type matters as it's affected by
    // the usage count of each CE and it increases every time a CE
    // is selected. MEMOPS has the least priority as it only cares about
    // low usage of the CE to improve latency
    uvm_channel_type_t types[] = {UVM_CHANNEL_TYPE_CPU_TO_GPU,
                                  UVM_CHANNEL_TYPE_GPU_TO_CPU,
                                  UVM_CHANNEL_TYPE_GPU_INTERNAL,
                                  UVM_CHANNEL_TYPE_GPU_TO_GPU,
                                  UVM_CHANNEL_TYPE_MEMOPS};

    UVM_ASSERT(!g_uvm_global.conf_computing_enabled);

    pick_ces_for_channel_types(manager, ce_caps, types, ARRAY_SIZE(types), preferred_ce);
}

static void pick_ces_conf_computing(uvm_channel_manager_t *manager,
                                    const UvmGpuCopyEngineCaps *ce_caps,
                                    unsigned *preferred_ce)
{
    unsigned best_wlc_ce;

    // The WLC type must go last so an unused CE is chosen, if available
    uvm_channel_type_t types[] = {UVM_CHANNEL_TYPE_CPU_TO_GPU,
                                  UVM_CHANNEL_TYPE_GPU_TO_CPU,
                                  UVM_CHANNEL_TYPE_GPU_INTERNAL,
                                  UVM_CHANNEL_TYPE_MEMOPS,
                                  UVM_CHANNEL_TYPE_WLC};

    UVM_ASSERT(g_uvm_global.conf_computing_enabled);

    pick_ces_for_channel_types(manager, ce_caps, types, ARRAY_SIZE(types), preferred_ce);

    // Direct transfers between GPUs are disallowed in Confidential Computing,
    // but the preferred CE is still set to an arbitrary value for consistency.
    preferred_ce[UVM_CHANNEL_TYPE_GPU_TO_GPU] = preferred_ce[UVM_CHANNEL_TYPE_GPU_TO_CPU];

    best_wlc_ce = preferred_ce[UVM_CHANNEL_TYPE_WLC];

    // TODO: Bug 4576908: in HCC, the WLC type should not share a CE with any
    // channel type other than LCIC. The assertion should be a check instead.
    UVM_ASSERT(ce_usage_count(best_wlc_ce, preferred_ce) == 0);
}

static NV_STATUS channel_manager_pick_ces(uvm_channel_manager_t *manager, unsigned *preferred_ce)
{
    NV_STATUS status;
    UvmGpuCopyEnginesCaps *ces_caps;
    uvm_channel_type_t type;

    for (type = 0; type < UVM_CHANNEL_TYPE_COUNT; type++)
        preferred_ce[type] = UVM_COPY_ENGINE_COUNT_MAX;

    ces_caps = uvm_kvmalloc_zero(sizeof(*ces_caps));
    if (!ces_caps)
        return NV_ERR_NO_MEMORY;

    status = uvm_rm_locked_call(nvUvmInterfaceQueryCopyEnginesCaps(uvm_gpu_device_handle(manager->gpu), ces_caps));
    if (status != NV_OK)
        goto out;

    status = ces_validate(manager, ces_caps->copyEngineCaps);
    if (status != NV_OK)
        goto out;

    if (g_uvm_global.conf_computing_enabled)
        pick_ces_conf_computing(manager, ces_caps->copyEngineCaps, preferred_ce);
    else
        pick_ces(manager, ces_caps->copyEngineCaps, preferred_ce);
out:
    uvm_kvfree(ces_caps);

    return status;
}

// Return the pool corresponding to the given CE index
//
// Used to retrieve pools of type UVM_CHANNEL_POOL_TYPE_CE only.
static uvm_channel_pool_t *channel_manager_ce_pool(uvm_channel_manager_t *manager, NvU32 ce)
{
    uvm_channel_pool_t *pool = uvm_channel_pool_first(manager, UVM_CHANNEL_POOL_TYPE_CE);

    UVM_ASSERT(pool != NULL);
    UVM_ASSERT(test_bit(ce, manager->ce_mask));

    // Pools of type UVM_CHANNEL_POOL_TYPE_CE are stored contiguously. The
    // offset of the pool associated with 'ce' is the number of usable CEs in
    // [0, ce).
    pool += bitmap_weight(manager->ce_mask, ce);

    UVM_ASSERT(pool->pool_type == UVM_CHANNEL_POOL_TYPE_CE);
    UVM_ASSERT(pool->engine_index == ce);

    return pool;
}

void uvm_channel_manager_set_p2p_ce(uvm_channel_manager_t *manager, uvm_gpu_t *peer, NvU32 optimal_ce)
{
    const NvU32 peer_gpu_index = uvm_id_gpu_index(peer->id);

    UVM_ASSERT(manager->gpu != peer);
    UVM_ASSERT(optimal_ce < UVM_COPY_ENGINE_COUNT_MAX);

    manager->pool_to_use.gpu_to_gpu[peer_gpu_index] = channel_manager_ce_pool(manager, optimal_ce);
}

static bool is_string_valid_location(const char *loc)
{
    return strcmp(uvm_channel_gpfifo_loc, "sys") == 0 ||
           strcmp(uvm_channel_gpfifo_loc, "vid") == 0 ||
           strcmp(uvm_channel_gpfifo_loc, "auto") == 0;
}

static UVM_BUFFER_LOCATION string_to_buffer_location(const char *loc)
{
    UVM_ASSERT(is_string_valid_location(loc));

    if (strcmp(loc, "sys") == 0)
        return UVM_BUFFER_LOCATION_SYS;
    else if (strcmp(loc, "vid") == 0)
        return UVM_BUFFER_LOCATION_VID;
    else
        return UVM_BUFFER_LOCATION_DEFAULT;
}

static const char *buffer_location_to_string(UVM_BUFFER_LOCATION loc)
{
    if (loc == UVM_BUFFER_LOCATION_SYS)
        return "sys";
    else if (loc == UVM_BUFFER_LOCATION_VID)
        return "vid";
    else if (loc == UVM_BUFFER_LOCATION_DEFAULT)
        return "auto";

    UVM_ASSERT_MSG(false, "Invalid buffer location value %d\n", loc);
    return NULL;
}

static void init_channel_manager_conf(uvm_channel_manager_t *manager)
{
    const char *gpfifo_loc_value;
    const char *gpput_loc_value;
    const char *pushbuffer_loc_value;

    uvm_gpu_t *gpu = manager->gpu;

    // 1- Number of GPFIFO entries
    manager->conf.num_gpfifo_entries = uvm_channel_num_gpfifo_entries;

    if (uvm_channel_num_gpfifo_entries < UVM_CHANNEL_NUM_GPFIFO_ENTRIES_MIN)
        manager->conf.num_gpfifo_entries = UVM_CHANNEL_NUM_GPFIFO_ENTRIES_MIN;
    else if (uvm_channel_num_gpfifo_entries > UVM_CHANNEL_NUM_GPFIFO_ENTRIES_MAX)
        manager->conf.num_gpfifo_entries = UVM_CHANNEL_NUM_GPFIFO_ENTRIES_MAX;

    if (!is_power_of_2(manager->conf.num_gpfifo_entries))
        manager->conf.num_gpfifo_entries = UVM_CHANNEL_NUM_GPFIFO_ENTRIES_DEFAULT;

    if (manager->conf.num_gpfifo_entries != uvm_channel_num_gpfifo_entries) {
        pr_info("Invalid value for uvm_channel_num_gpfifo_entries = %u, using %u instead\n",
                uvm_channel_num_gpfifo_entries,
                manager->conf.num_gpfifo_entries);
    }

    // 2- Allocation locations

    if (uvm_conf_computing_mode_is_hcc(gpu)) {
        UVM_ASSERT(gpu->mem_info.size > 0);

        // When the Confidential Computing feature is enabled, the GPU is
        // guaranteed to have a frame buffer and the hardware enforces the
        // following apertures for all channel types but SEC2. We create SEC2
        // channels by overriding their GPPUT and GPFIFO locations and ignoring
        // the location from the channel manager. SEC2 channels are used to
        // bootstrap secure work submission. To setup secure work launch, data
        // transfers from unprotected main memory to protected vidmem are
        // performed by the SEC2 engine and the driver must push work to SEC2
        // channels using unprotected locations.
        manager->conf.gpfifo_loc     = UVM_BUFFER_LOCATION_VID;
        manager->conf.gpput_loc      = UVM_BUFFER_LOCATION_VID;

        // Pushbuffer is located in unprotected sysmem. This is the location
        // that gets mapped to both CPU and GPU. It is populated either by
        // signed pushes (SEC2 channels), or encrypted pushes (CE channels).
        manager->conf.pushbuffer_loc = UVM_BUFFER_LOCATION_SYS;
        return;
    }

    // Override if the GPU doesn't have memory
    if (gpu->mem_info.size == 0) {
        manager->conf.pushbuffer_loc = UVM_BUFFER_LOCATION_SYS;
        manager->conf.gpfifo_loc     = UVM_BUFFER_LOCATION_SYS;
        manager->conf.gpput_loc      = UVM_BUFFER_LOCATION_SYS;
        return;
    }

    manager->conf.pushbuffer_loc = UVM_BUFFER_LOCATION_SYS;

    pushbuffer_loc_value = uvm_channel_pushbuffer_loc;
    if (!is_string_valid_location(pushbuffer_loc_value)) {
        pushbuffer_loc_value = UVM_CHANNEL_PUSHBUFFER_LOC_DEFAULT;
        pr_info("Invalid value for uvm_channel_pushbuffer_loc = %s, using %s instead\n",
                uvm_channel_pushbuffer_loc,
                pushbuffer_loc_value);
    }

    // Override the default value if requested by the user
    if (strcmp(pushbuffer_loc_value, "vid") == 0) {
        // aarch64 requires memset_io/memcpy_io instead of memset/memcpy for
        // mapped GPU memory. The existing push paths only use memset/memcpy,
        // so force the location to sys for now.
        // TODO: Bug 2904133: Remove the following "if" after the bug is fixed.
        if (NVCPU_IS_AARCH64) {
            pr_info("uvm_channel_pushbuffer_loc = %s is not supported on AARCH64, using sys instead\n",
                    pushbuffer_loc_value);
            manager->conf.pushbuffer_loc = UVM_BUFFER_LOCATION_SYS;
        }
        else {
            manager->conf.pushbuffer_loc = UVM_BUFFER_LOCATION_VID;
        }
    }

    // 3- GPFIFO/GPPut location
    // Only support the knobs for GPFIFO/GPPut on Volta+
    if (!gpu->parent->gpfifo_in_vidmem_supported) {
        if (manager->conf.gpput_loc == UVM_BUFFER_LOCATION_SYS) {
            pr_info("CAUTION: allocating GPPut in sysmem is NOT supported and may crash the system, using %s instead\n",
                    buffer_location_to_string(UVM_BUFFER_LOCATION_DEFAULT));
        }

        manager->conf.gpfifo_loc = UVM_BUFFER_LOCATION_DEFAULT;
        manager->conf.gpput_loc = UVM_BUFFER_LOCATION_DEFAULT;

        return;
    }

    gpfifo_loc_value = uvm_channel_gpfifo_loc;
    if (!is_string_valid_location(gpfifo_loc_value)) {
        gpfifo_loc_value = UVM_CHANNEL_GPFIFO_LOC_DEFAULT;
        pr_info("Invalid value for uvm_channel_gpfifo_loc = %s, using %s instead\n",
                uvm_channel_gpfifo_loc,
                gpfifo_loc_value);
    }

    gpput_loc_value = uvm_channel_gpput_loc;
    if (!is_string_valid_location(gpput_loc_value)) {
        gpput_loc_value = UVM_CHANNEL_GPPUT_LOC_DEFAULT;
        pr_info("Invalid value for uvm_channel_gpput_loc = %s, using %s instead\n",
                uvm_channel_gpput_loc,
                gpput_loc_value);
    }

    // On coherent platforms where the GPU does not cache sysmem but the CPU
    // caches vidmem (and sysmem), we place GPFIFO and GPPUT on sysmem to avoid
    // cache thrash. The memory access latency is reduced, despite the required
    // access through the bus, because no cache coherence message is exchanged.
    if (uvm_parent_gpu_is_coherent(gpu->parent)) {
        manager->conf.gpfifo_loc = UVM_BUFFER_LOCATION_SYS;

        // On GPUs with limited ESCHED addressing range, e.g., Volta on P9, RM
        // cannot guarantee that USERD/GPPUT physical address is accessible by
        // ESCHED. We set GPPUT location to vidmem where physical addresses are
        // all accessible by ESCHED. We use the max_host_va as a proxy for the
        // PA limitation, since all architectures with 40b VA limits also have
        // 40b PA limits.
        manager->conf.gpput_loc = gpu->parent->max_host_va == (1ull << 40) ? UVM_BUFFER_LOCATION_VID :
                                                                             UVM_BUFFER_LOCATION_SYS;
    }
    else {
        // By default we place GPFIFO and GPPUT on vidmem as it potentially has
        // lower latency.
        manager->conf.gpfifo_loc = UVM_BUFFER_LOCATION_VID;
        manager->conf.gpput_loc = UVM_BUFFER_LOCATION_VID;
    }

    // Override defaults
    if (string_to_buffer_location(gpfifo_loc_value) != UVM_BUFFER_LOCATION_DEFAULT)
        manager->conf.gpfifo_loc = string_to_buffer_location(gpfifo_loc_value);

    if (string_to_buffer_location(gpput_loc_value) != UVM_BUFFER_LOCATION_DEFAULT)
        manager->conf.gpput_loc = string_to_buffer_location(gpput_loc_value);
}

// Returns the maximum number of pools that are needed in the current
// configuration. The implementation may choose to create a smaller number of
// pools.
static unsigned channel_manager_get_max_pools(uvm_channel_manager_t *manager)
{
    unsigned num_channel_pools;

    // Create one CE channel pool per usable CE
    num_channel_pools = bitmap_weight(manager->ce_mask, UVM_COPY_ENGINE_COUNT_MAX);

    // CE proxy channel pool.
    if (uvm_parent_gpu_needs_proxy_channel_pool(manager->gpu->parent))
        num_channel_pools++;

    // SEC2 pool, WLC pool, LCIC pool
    if (g_uvm_global.conf_computing_enabled)
        num_channel_pools += 3;

    return num_channel_pools;
}

static NV_STATUS channel_manager_create_ce_pools(uvm_channel_manager_t *manager, unsigned *preferred_ce)
{
    unsigned ce;
    unsigned type;

    // A pool is created for each usable CE, even if it has not been selected as
    // the preferred CE for any type, because as more information is discovered
    // (for example, a pair of peer GPUs is added) we may start using the
    // previously idle pools. Configurations where non-preferred CEs are
    // guaranteed to remain unused are allowed to avoid marking those engines as
    // usable.
    for_each_set_bit(ce, manager->ce_mask, UVM_COPY_ENGINE_COUNT_MAX) {
        NV_STATUS status;
        uvm_channel_pool_t *pool = NULL;

        status = channel_pool_add(manager, UVM_CHANNEL_POOL_TYPE_CE, ce, &pool);
        if (status != NV_OK)
            return status;
    }

    for (type = 0; type < UVM_CHANNEL_TYPE_CE_COUNT; type++) {
        // Avoid overwriting previously set defaults.
        if (manager->pool_to_use.default_for_type[type] != NULL)
            continue;

        ce = preferred_ce[type];
        manager->pool_to_use.default_for_type[type] = channel_manager_ce_pool(manager, ce);
    }

    return NV_OK;
}

static NV_STATUS setup_wlc_schedule(uvm_channel_t *wlc)
{
    uvm_gpu_t *gpu = uvm_channel_get_gpu(wlc);
    NvU64 protected_vidmem = uvm_rm_mem_get_gpu_uvm_va(wlc->conf_computing.static_pb_protected_vidmem, gpu);
    NvU64 unprotected_sysmem_gpu = uvm_rm_mem_get_gpu_uvm_va(wlc->conf_computing.static_pb_unprotected_sysmem, gpu);
    void *unprotected_sysmem_cpu = wlc->conf_computing.static_pb_unprotected_sysmem_cpu;
    NvU64 tag_offset = (uintptr_t)wlc->conf_computing.static_pb_unprotected_sysmem_auth_tag_cpu -
                       (uintptr_t)wlc->conf_computing.static_pb_unprotected_sysmem_cpu;

    NvU64 *wlc_gpfifo_entries;
    uvm_push_t wlc_decrypt_push, sec2_push;
    NvU32 decrypt_push_size;
    int i;
    NV_STATUS status = NV_OK;

    // "gpfifo" is the representation of GPFIFO copied to gpFifoGpu
    const size_t gpfifo_size = wlc->num_gpfifo_entries * sizeof(*wlc_gpfifo_entries);
    void *gpfifo_unprotected_cpu = unprotected_sysmem_cpu;
    NvU64 gpfifo_unprotected_gpu = unprotected_sysmem_gpu;

    // "run_push" represents mutable push location used by WLC
    uvm_gpu_address_t run_push_protected_gpu = uvm_gpu_address_virtual(protected_vidmem);
    uvm_gpu_address_t run_push_unprotected_gpu = uvm_gpu_address_virtual(unprotected_sysmem_gpu);
    uvm_gpu_address_t run_push_unprotected_auth_tag_gpu = uvm_gpu_address_virtual(unprotected_sysmem_gpu + tag_offset);

    // "decrypt_push" represents WLC decrypt push, constructed using fake_push.
    // Copied to wlc_pb_base + UVM_MAX_WLC_PUSH_SIZE, as the second of the two
    // pushes that make the WLC fixed schedule.
    NvU64 decrypt_push_protected_gpu = UVM_ALIGN_UP(protected_vidmem + UVM_MAX_WLC_PUSH_SIZE, UVM_CONF_COMPUTING_SEC2_BUF_ALIGNMENT);
    NvU64 decrypt_push_unprotected_gpu = unprotected_sysmem_gpu + gpfifo_size;
    void *decrypt_push_unprotected_cpu = (char*)gpfifo_unprotected_cpu + gpfifo_size;

    // Tags for upload via SEC2
    void *decrypt_push_auth_tag, *gpfifo_auth_tag;
    uvm_gpu_address_t decrypt_push_auth_tag_gpu, gpfifo_auth_tag_gpu;

    BUILD_BUG_ON(sizeof(*wlc_gpfifo_entries) != sizeof(*wlc->channel_info.gpFifoEntries));

    UVM_ASSERT(uvm_channel_is_wlc(wlc));
    UVM_ASSERT(tag_offset == UVM_ALIGN_UP(UVM_MAX_WLC_PUSH_SIZE, UVM_CONF_COMPUTING_AUTH_TAG_ALIGNMENT));

    // WLC schedule consists of two parts, the number of entries needs to be even.
    // This also guarantees that the size is 16B aligned
    UVM_ASSERT(IS_ALIGNED(wlc->num_gpfifo_entries, 2));

    wlc_gpfifo_entries = uvm_kvmalloc(gpfifo_size);
    if (!wlc_gpfifo_entries)
        return NV_ERR_NO_MEMORY;

    // WLC can only process one job at a time.
    // Prune any initialization entries and block all but one (+1 for sentinel)
    uvm_channel_update_progress(wlc);
    if (!try_claim_channel(wlc, wlc->num_gpfifo_entries - 2)) {
        status = NV_ERR_INVALID_STATE;
        goto free_gpfifo_entries;
    }

    // WLC schedule has two steps:
    // 1.) Decrypt from gpu_unprotected_base to wlc_pb_base
    //     Increment LCIC PUT
    // 2.) Execute push at wlc_pb_base
    // The first one is a push, the second one is just a GPFIFO entry
    status = uvm_push_begin_fake(gpu, &wlc_decrypt_push);
    if (status != NV_OK)
        goto free_gpfifo_entries;

    // Begin WLC DECRYPT push
    uvm_push_set_flag(&wlc_decrypt_push, UVM_PUSH_FLAG_NEXT_MEMBAR_GPU);
    gpu->parent->ce_hal->decrypt(&wlc_decrypt_push,
                                 run_push_protected_gpu,
                                 run_push_unprotected_gpu,
                                 UVM_MAX_WLC_PUSH_SIZE,
                                 run_push_unprotected_auth_tag_gpu);
    uvm_hal_wfi_membar(&wlc_decrypt_push, UVM_MEMBAR_NONE);
    decrypt_push_size = uvm_push_get_size(&wlc_decrypt_push);

    // The code below reuses static unprotected sysmem buffer as a temporary
    // storage for uploading the schedule. Check that everything fits.
    UVM_ASSERT(gpfifo_size + decrypt_push_size <= UVM_MAX_WLC_PUSH_SIZE);

    // GPFIFO schedule should alternate between the decrypt routine
    // we don't know if initialization used even or odd number of
    // GPFIFO entries so the exact pattern depends on the value of "PUT"
    for (i = 0; i < wlc->num_gpfifo_entries; ++i) {
        if (i % 2 == wlc->cpu_put % 2) {
            gpu->parent->host_hal->set_gpfifo_entry(wlc_gpfifo_entries + i,
                                                    decrypt_push_protected_gpu,
                                                    decrypt_push_size,
                                                    UVM_GPFIFO_SYNC_PROCEED);
        }
        else {
            gpu->parent->host_hal->set_gpfifo_entry(wlc_gpfifo_entries + i,
                                                    run_push_protected_gpu.address,
                                                    UVM_MAX_WLC_PUSH_SIZE,
                                                    UVM_GPFIFO_SYNC_WAIT);
        }
    }

    // The schedule is prepared. Upload to vidmem
    status = uvm_push_begin(wlc->pool->manager,
                            UVM_CHANNEL_TYPE_SEC2,
                            &sec2_push,
                            "Upload WLC schedule for: %s", wlc->name);
    if (status != NV_OK)
        goto end_wlc_dec_push;

    decrypt_push_auth_tag = uvm_push_get_single_inline_buffer(&sec2_push,
                                                              UVM_CONF_COMPUTING_AUTH_TAG_SIZE,
                                                              UVM_CONF_COMPUTING_AUTH_TAG_ALIGNMENT,
                                                              &decrypt_push_auth_tag_gpu);

    gpfifo_auth_tag = uvm_push_get_single_inline_buffer(&sec2_push,
                                                        UVM_CONF_COMPUTING_AUTH_TAG_SIZE,
                                                        UVM_CONF_COMPUTING_AUTH_TAG_ALIGNMENT,
                                                        &gpfifo_auth_tag_gpu);

    // Upload WLC pushbuffer
    uvm_conf_computing_cpu_encrypt(sec2_push.channel,
                                   decrypt_push_unprotected_cpu,
                                   wlc_decrypt_push.begin,
                                   NULL,
                                   decrypt_push_size,
                                   decrypt_push_auth_tag);
    gpu->parent->sec2_hal->decrypt(&sec2_push,
                                   decrypt_push_protected_gpu,
                                   decrypt_push_unprotected_gpu,
                                   decrypt_push_size,
                                   decrypt_push_auth_tag_gpu.address);

    // Upload WLC GPFIFO
    uvm_conf_computing_cpu_encrypt(sec2_push.channel,
                                   gpfifo_unprotected_cpu,
                                   wlc_gpfifo_entries,
                                   NULL,
                                   gpfifo_size,
                                   gpfifo_auth_tag);
    gpu->parent->sec2_hal->decrypt(&sec2_push,
                                   wlc->channel_info.gpFifoGpuVa,
                                   gpfifo_unprotected_gpu,
                                   gpfifo_size,
                                   gpfifo_auth_tag_gpu.address);

    // Prime the WLC by setting "PUT" two steps ahead. Reuse the current
    // cpu_put value that was used during channel initialization.
    // Don't update wlc->cpu_put, it will be used to track submitted pushes
    // as any other channel.
    update_gpput_via_sec2(&sec2_push, wlc, (wlc->cpu_put + 2) % wlc->num_gpfifo_entries);

    status = uvm_push_end_and_wait(&sec2_push);

end_wlc_dec_push:
    uvm_push_end_fake(&wlc_decrypt_push);
free_gpfifo_entries:
    uvm_kvfree(wlc_gpfifo_entries);
    return status;
}

static NV_STATUS setup_lcic_schedule(uvm_channel_t *paired_wlc, uvm_channel_t *lcic)
{
    uvm_gpu_t *gpu = uvm_channel_get_gpu(lcic);
    NvU64 lcic_pb_base = uvm_rm_mem_get_gpu_uvm_va(lcic->conf_computing.static_pb_protected_vidmem, gpu);

    // Reuse WLC sysmem allocation
    NvU64 gpu_unprotected = uvm_rm_mem_get_gpu_uvm_va(paired_wlc->conf_computing.static_pb_unprotected_sysmem, gpu);
    char *cpu_unprotected = paired_wlc->conf_computing.static_pb_unprotected_sysmem_cpu;

    uvm_gpu_semaphore_t *lcic_semaphore = &lcic->tracking_sem.semaphore;
    uvm_gpu_address_t notifier_src_entry_addr = lcic->conf_computing.static_notifier_entry_unprotected_sysmem_gpu_va;
    uvm_gpu_address_t notifier_src_exit_addr = lcic->conf_computing.static_notifier_exit_unprotected_sysmem_gpu_va;
    uvm_gpu_address_t notifier_dst_addr = uvm_gpu_semaphore_get_notifier_gpu_va(lcic_semaphore);
    uvm_gpu_address_t encrypted_payload_gpu_va = uvm_gpu_semaphore_get_encrypted_payload_gpu_va(lcic_semaphore);
    uvm_gpu_address_t auth_tag_gpu_va = uvm_gpu_semaphore_get_auth_tag_gpu_va(lcic_semaphore);
    uvm_gpu_address_t semaphore_gpu_va = uvm_gpu_address_virtual(uvm_channel_tracking_semaphore_get_gpu_va(lcic));
    NvU32 payload_size = sizeof(*uvm_gpu_semaphore_get_encrypted_payload_cpu_va(lcic_semaphore));
    NvU32 notifier_size = sizeof(*lcic->conf_computing.static_notifier_entry_unprotected_sysmem_cpu);

    NvU64 *lcic_gpfifo_entries;
    uvm_push_t lcic_push, sec2_push;
    NvU32 lcic_push_size;
    int i;
    NV_STATUS status;

    const size_t gpfifo_size = lcic->num_gpfifo_entries * sizeof(*lcic_gpfifo_entries);
    char *gpfifo_unprotected_cpu = cpu_unprotected;
    NvU64 gpfifo_unprotected_gpu = gpu_unprotected;
    char *lcic_push_unprotected_cpu = gpfifo_unprotected_cpu + gpfifo_size;
    NvU64 lcic_push_unprotected_gpu = gpfifo_unprotected_gpu + gpfifo_size;
    NvU64 lcic_push_protected_gpu = lcic_pb_base;

    char *lcic_push_enc_tag, *gpfifo_enc_tag;
    uvm_gpu_address_t lcic_push_enc_tag_gpu, gpfifo_enc_tag_gpu;

    BUILD_BUG_ON(sizeof(*lcic_gpfifo_entries) != sizeof(*lcic->channel_info.gpFifoEntries));

    UVM_ASSERT(uvm_channel_is_wlc(paired_wlc));
    UVM_ASSERT(uvm_channel_is_lcic(lcic));

    lcic_gpfifo_entries = uvm_kvmalloc(gpfifo_size);
    if (!lcic_gpfifo_entries)
        return NV_ERR_NO_MEMORY;

    // LCIC can not process outside jobs.
    // Prune any initialization entries and
    // block all gpfifo entries (-1 for sentinel)
    uvm_channel_update_progress(lcic);
    if (!try_claim_channel(lcic, lcic->num_gpfifo_entries - 1)) {
        status = NV_ERR_INVALID_STATE;
        goto free_gpfifo_entries;
    }

    status = uvm_push_begin_fake(gpu, &lcic_push);
    if (status != NV_OK)
        goto free_gpfifo_entries;

    // LCIC schedule is simple:
    // 1.) wait for engine idle
    // 2.) advance the WLC PUT by 2
    // 3.) release driver semaphore
    uvm_hal_wfi_membar(&lcic_push, UVM_MEMBAR_NONE);
    gpu->parent->ce_hal->semaphore_reduction_inc(&lcic_push,
                                                 paired_wlc->channel_info.gpPutGpuVa,
                                                 paired_wlc->num_gpfifo_entries - 1);
    gpu->parent->ce_hal->semaphore_reduction_inc(&lcic_push,
                                                 paired_wlc->channel_info.gpPutGpuVa,
                                                 paired_wlc->num_gpfifo_entries - 1);
    gpu->parent->ce_hal->semaphore_reduction_inc(&lcic_push,
                                                 semaphore_gpu_va.address,
                                                 0xffffffff);

    gpu->parent->ce_hal->memcopy(&lcic_push, notifier_dst_addr, notifier_src_entry_addr, notifier_size);
    gpu->parent->ce_hal->encrypt(&lcic_push, encrypted_payload_gpu_va, semaphore_gpu_va, payload_size, auth_tag_gpu_va);
    gpu->parent->ce_hal->memcopy(&lcic_push, notifier_dst_addr, notifier_src_exit_addr, notifier_size);

    // End LCIC push
    lcic_push_size = uvm_push_get_size(&lcic_push);

    // We're reusing pre-allocated structures from WLC, make sure we fit.
    UVM_ASSERT(lcic_push_size == UVM_LCIC_PUSH_SIZE);
    UVM_ASSERT(lcic_push_size + gpfifo_size <= UVM_MAX_WLC_PUSH_SIZE);

    // Set all entries to execute the above push
    for (i = 0; i < lcic->num_gpfifo_entries; ++i)
        gpu->parent->host_hal->set_gpfifo_entry(lcic_gpfifo_entries + i,
                                                lcic_push_protected_gpu,
                                                lcic_push_size,
                                                UVM_GPFIFO_SYNC_PROCEED);

    // Upload the prepared schedule using SEC2
    status = uvm_push_begin(lcic->pool->manager,
                            UVM_CHANNEL_TYPE_SEC2,
                            &sec2_push,
                            "Upload LCIC schedule for: %s", lcic->name);
    if (status != NV_OK)
        goto end_lcic_push;

    lcic_push_enc_tag = uvm_push_get_single_inline_buffer(&sec2_push,
                                                          UVM_CONF_COMPUTING_AUTH_TAG_SIZE,
                                                          UVM_CONF_COMPUTING_AUTH_TAG_ALIGNMENT,
                                                          &lcic_push_enc_tag_gpu);

    gpfifo_enc_tag = uvm_push_get_single_inline_buffer(&sec2_push,
                                                       UVM_CONF_COMPUTING_AUTH_TAG_SIZE,
                                                       UVM_CONF_COMPUTING_AUTH_TAG_ALIGNMENT,
                                                       &gpfifo_enc_tag_gpu);

    // Upload LCIC pushbuffer
    uvm_conf_computing_cpu_encrypt(sec2_push.channel,
                                   lcic_push_unprotected_cpu,
                                   lcic_push.begin,
                                   NULL,
                                   lcic_push_size,
                                   lcic_push_enc_tag);
    gpu->parent->sec2_hal->decrypt(&sec2_push,
                                   lcic_push_protected_gpu,
                                   lcic_push_unprotected_gpu,
                                   lcic_push_size,
                                   lcic_push_enc_tag_gpu.address);

    // Upload LCIC GPFIFO
    uvm_conf_computing_cpu_encrypt(sec2_push.channel,
                                   gpfifo_unprotected_cpu,
                                   lcic_gpfifo_entries,
                                   NULL,
                                   gpfifo_size,
                                   gpfifo_enc_tag);
    gpu->parent->sec2_hal->decrypt(&sec2_push,
                                   lcic->channel_info.gpFifoGpuVa,
                                   gpfifo_unprotected_gpu,
                                   gpfifo_size,
                                   gpfifo_enc_tag_gpu.address);
    status = uvm_push_end_and_wait(&sec2_push);

end_lcic_push:
    uvm_push_end_fake(&lcic_push);
free_gpfifo_entries:
    uvm_kvfree(lcic_gpfifo_entries);
    return status;
}

static NV_STATUS channel_manager_setup_wlc_lcic(uvm_channel_pool_t *wlc_pool, uvm_channel_pool_t *lcic_pool)
{
    NvU32 i;

    UVM_ASSERT(wlc_pool->manager == lcic_pool->manager);
    UVM_ASSERT(wlc_pool->manager->pool_to_use.default_for_type[UVM_CHANNEL_TYPE_WLC] != NULL);
    UVM_ASSERT(lcic_pool->manager->pool_to_use.default_for_type[UVM_CHANNEL_TYPE_LCIC] == NULL);
    UVM_ASSERT(wlc_pool->num_channels == lcic_pool->num_channels);

    for (i = 0; i < wlc_pool->num_channels; ++i) {
        uvm_channel_t *wlc = wlc_pool->channels + i;
        uvm_channel_t *lcic = lcic_pool->channels + i;
        NV_STATUS status;

        status = setup_wlc_schedule(wlc);
        if (status != NV_OK)
            return status;

        status = setup_lcic_schedule(wlc, lcic);
        if (status != NV_OK)
            return status;
    }

    return NV_OK;
}

static NV_STATUS channel_manager_create_conf_computing_pools(uvm_channel_manager_t *manager, unsigned *preferred_ce)
{
    NV_STATUS status;
    unsigned wlc_lcic_ce_index;
    uvm_channel_pool_t *sec2_pool = NULL;
    uvm_channel_pool_t *wlc_pool = NULL;
    uvm_channel_pool_t *lcic_pool = NULL;

    if (!g_uvm_global.conf_computing_enabled)
        return NV_OK;

    status = uvm_rm_mem_alloc(manager->gpu,
                             UVM_RM_MEM_TYPE_SYS,
                             sizeof(UvmCslIv),
                             UVM_CONF_COMPUTING_BUF_ALIGNMENT,
                             &manager->gpu->conf_computing.iv_rm_mem);
    if (status != NV_OK)
        return status;

    // Create SEC2 pool. This needs to be done first, initialization of
    // other channels needs SEC2.
    status = channel_pool_add(manager, UVM_CHANNEL_POOL_TYPE_SEC2, 0, &sec2_pool);
    if (status != NV_OK)
        return status;

    manager->pool_to_use.default_for_type[UVM_CHANNEL_TYPE_SEC2] = sec2_pool;

    // WLC and LCIC must use the same engine for the fixed schedule to work.
    wlc_lcic_ce_index = preferred_ce[UVM_CHANNEL_TYPE_WLC];

    // Create WLC/LCIC pools. This should be done early, CE channels use
    // them for secure launch. The WLC pool must be created before the LCIC.
    status = channel_pool_add(manager, UVM_CHANNEL_POOL_TYPE_WLC, wlc_lcic_ce_index, &wlc_pool);
    if (status != NV_OK)
        return status;

    manager->pool_to_use.default_for_type[UVM_CHANNEL_TYPE_WLC] = wlc_pool;

    status = channel_pool_add(manager, UVM_CHANNEL_POOL_TYPE_LCIC, wlc_lcic_ce_index, &lcic_pool);
    if (status != NV_OK)
        return status;

    status = channel_manager_setup_wlc_lcic(wlc_pool, lcic_pool);
    if (status != NV_OK)
        return status;

    // The LCIC pool must be assigned after the call to
    // channel_manager_setup_wlc_lcic(). It determines WLC and LCIC channels
    // are ready to be used for secure work submission.
    manager->pool_to_use.default_for_type[UVM_CHANNEL_TYPE_LCIC] = lcic_pool;

    return NV_OK;
}

static NV_STATUS channel_manager_create_pools(uvm_channel_manager_t *manager)
{
    NV_STATUS status;
    unsigned max_channel_pools;
    unsigned preferred_ce[UVM_CHANNEL_TYPE_COUNT];

    status = channel_manager_pick_ces(manager, preferred_ce);
    if (status != NV_OK)
        return status;

    max_channel_pools = channel_manager_get_max_pools(manager);

    manager->channel_pools = uvm_kvmalloc_zero(sizeof(*manager->channel_pools) * max_channel_pools);
    if (!manager->channel_pools)
        return NV_ERR_NO_MEMORY;

    status = channel_manager_create_conf_computing_pools(manager, preferred_ce);
    if (status != NV_OK)
        return status;

    status = channel_manager_create_ce_pools(manager, preferred_ce);
    if (status != NV_OK)
        return status;

    // In SR-IOV heavy, add an additional, single-channel, pool that is
    // dedicated to the MEMOPS type.
    if (uvm_parent_gpu_needs_proxy_channel_pool(manager->gpu->parent)) {
        uvm_channel_pool_t *proxy_pool = NULL;
        uvm_channel_type_t channel_type = uvm_channel_proxy_channel_type();

        status = channel_pool_add(manager, UVM_CHANNEL_POOL_TYPE_CE_PROXY, preferred_ce[channel_type], &proxy_pool);
        if (status != NV_OK)
            return status;

        manager->pool_to_use.default_for_type[channel_type] = proxy_pool;
    }

    return NV_OK;
}

NV_STATUS uvm_channel_manager_create(uvm_gpu_t *gpu, uvm_channel_manager_t **channel_manager_out)
{
    NV_STATUS status = NV_OK;
    uvm_channel_manager_t *channel_manager;

    channel_manager = uvm_kvmalloc_zero(sizeof(*channel_manager));
    if (!channel_manager)
        return NV_ERR_NO_MEMORY;

    channel_manager->gpu = gpu;
    init_channel_manager_conf(channel_manager);
    status = uvm_pushbuffer_create(channel_manager, &channel_manager->pushbuffer);
    if (status != NV_OK)
        goto error;

    status = manager_create_procfs_dirs(channel_manager);
    if (status != NV_OK)
        goto error;

    status = channel_manager_create_pools(channel_manager);
    if (status != NV_OK)
        goto error;

    status = manager_create_procfs(channel_manager);
    if (status != NV_OK)
        goto error;

    *channel_manager_out = channel_manager;

    return status;

error:
    uvm_channel_manager_destroy(channel_manager);
    return status;
}

static void channel_manager_destroy_pools(uvm_channel_manager_t *manager)
{
    uvm_rm_mem_free(manager->gpu->conf_computing.iv_rm_mem);
    manager->gpu->conf_computing.iv_rm_mem = NULL;
    while (manager->num_channel_pools > 0)
        channel_pool_destroy(manager->channel_pools + manager->num_channel_pools - 1);

    uvm_kvfree(manager->channel_pools);
}

// Because the WLC at rest state has PUT = GET + 2, there's always pending work
// This is what enables the driver to submit work just by ringing a doorbell.
// However, this also means that the pending work has to be removed before
// the channel is passed to RM for deallocation.
static void channel_manager_stop_wlc(uvm_channel_manager_t *manager)
{
    uvm_channel_pool_t *wlc_pool = manager->pool_to_use.default_for_type[UVM_CHANNEL_TYPE_WLC];
    uvm_channel_pool_t *lcic_pool = manager->pool_to_use.default_for_type[UVM_CHANNEL_TYPE_LCIC];
    uvm_channel_t *channel;
    uvm_push_t push;
    NV_STATUS status;

    uvm_for_each_channel_in_pool(channel, lcic_pool) {
        uvm_spin_loop_t spin;

        // Wait for the WLC/LCIC to be primed. This means that PUT == GET + 2
        // and a WLC doorbell ring is enough to start work.
        UVM_SPIN_WHILE(!uvm_gpu_tracking_semaphore_is_completed(&channel->tracking_sem), &spin);
    }

    status = uvm_push_begin(manager, UVM_CHANNEL_TYPE_SEC2, &push, "Stop WLC channels");
    if (status != NV_OK) {
        UVM_ERR_PRINT_NV_STATUS("Failed to begin stop push for WLC", status);
        return;
    }

    uvm_for_each_channel_in_pool(channel, wlc_pool) {
        // Every gpfifo entry advances the gpu put of WLC by two so the current
        // value is: (cpu_put * 2) % num_gpfifo_entries and it's ahead of the
        // get pointer by 2.
        update_gpput_via_sec2(&push, channel, (channel->cpu_put * 2 - 2) % channel->num_gpfifo_entries);
    }

    status = uvm_push_end_and_wait(&push);
    if (status != NV_OK)
        UVM_ERR_PRINT_NV_STATUS("Failed to end stop push for WLC", status);

    manager->pool_to_use.default_for_type[UVM_CHANNEL_TYPE_WLC] = NULL;
    manager->pool_to_use.default_for_type[UVM_CHANNEL_TYPE_LCIC] = NULL;
}

void uvm_channel_manager_destroy(uvm_channel_manager_t *channel_manager)
{
    if (channel_manager == NULL)
        return;

    proc_remove(channel_manager->procfs.pending_pushes);

    if (uvm_channel_manager_is_wlc_ready(channel_manager))
        channel_manager_stop_wlc(channel_manager);

    channel_manager_destroy_pools(channel_manager);

    proc_remove(channel_manager->procfs.channels_dir);

    uvm_pushbuffer_destroy(channel_manager->pushbuffer);

    uvm_kvfree(channel_manager);
}

bool uvm_channel_is_privileged(uvm_channel_t *channel)
{
    if (uvm_parent_gpu_is_virt_mode_sriov_heavy(uvm_channel_get_gpu(channel)->parent))
        return uvm_channel_is_proxy(channel);

    return true;
}

// Return the first channel pool of the given type(s) starting at begin_pool
// (included).
//
// The pool type mask must be a non empty mask of uvm_channel_pool_type_t
// values.
static uvm_channel_pool_t *channel_pool_first_from(uvm_channel_manager_t *manager,
                                                   uvm_channel_pool_t *begin_pool,
                                                   NvU32 pool_type_mask)
{
    uvm_channel_pool_t *curr_pool, *end_pool;

    UVM_ASSERT(manager->channel_pools != NULL);
    UVM_ASSERT(begin_pool != NULL);
    UVM_ASSERT(begin_pool >= manager->channel_pools);
    UVM_ASSERT(pool_type_mask > 0);
    UVM_ASSERT(pool_type_mask <= UVM_CHANNEL_POOL_TYPE_MASK);

    end_pool = manager->channel_pools + manager->num_channel_pools;
    UVM_ASSERT(begin_pool <= end_pool);

    for (curr_pool = begin_pool; curr_pool != end_pool; curr_pool++) {
        if (curr_pool->pool_type & pool_type_mask)
            return curr_pool;
    }

    return NULL;
}

uvm_channel_pool_t *uvm_channel_pool_first(uvm_channel_manager_t *manager, NvU32 pool_type_mask)
{
    return channel_pool_first_from(manager, manager->channel_pools, pool_type_mask);
}

uvm_channel_pool_t *uvm_channel_pool_next(uvm_channel_manager_t *manager,
                                          uvm_channel_pool_t *pool,
                                          NvU32 pool_type_mask)
{
    return channel_pool_first_from(manager, pool + 1, pool_type_mask);
}

uvm_channel_t *uvm_channel_any_of_type(uvm_channel_manager_t *manager, NvU32 pool_type_mask)
{
    uvm_channel_pool_t *pool = uvm_channel_pool_first(manager, pool_type_mask);

    if (pool == NULL)
        return NULL;

    UVM_ASSERT(pool->channels);

    return pool->channels;
}

const char *uvm_channel_type_to_string(uvm_channel_type_t channel_type)
{
    BUILD_BUG_ON(UVM_CHANNEL_TYPE_COUNT != 8);

    switch (channel_type) {
        UVM_ENUM_STRING_CASE(UVM_CHANNEL_TYPE_CPU_TO_GPU);
        UVM_ENUM_STRING_CASE(UVM_CHANNEL_TYPE_GPU_TO_CPU);
        UVM_ENUM_STRING_CASE(UVM_CHANNEL_TYPE_GPU_INTERNAL);
        UVM_ENUM_STRING_CASE(UVM_CHANNEL_TYPE_MEMOPS);
        UVM_ENUM_STRING_CASE(UVM_CHANNEL_TYPE_GPU_TO_GPU);
        UVM_ENUM_STRING_CASE(UVM_CHANNEL_TYPE_SEC2);
        UVM_ENUM_STRING_CASE(UVM_CHANNEL_TYPE_WLC);
        UVM_ENUM_STRING_CASE(UVM_CHANNEL_TYPE_LCIC);
        UVM_ENUM_STRING_DEFAULT();
    }
}

const char *uvm_channel_pool_type_to_string(uvm_channel_pool_type_t channel_pool_type)
{
    BUILD_BUG_ON(UVM_CHANNEL_POOL_TYPE_COUNT != 5);

    switch (channel_pool_type) {
        UVM_ENUM_STRING_CASE(UVM_CHANNEL_POOL_TYPE_CE);
        UVM_ENUM_STRING_CASE(UVM_CHANNEL_POOL_TYPE_CE_PROXY);
        UVM_ENUM_STRING_CASE(UVM_CHANNEL_POOL_TYPE_SEC2);
        UVM_ENUM_STRING_CASE(UVM_CHANNEL_POOL_TYPE_WLC);
        UVM_ENUM_STRING_CASE(UVM_CHANNEL_POOL_TYPE_LCIC);
        UVM_ENUM_STRING_DEFAULT();
    }
}

static const char *get_gpfifo_location_string(uvm_channel_t *channel)
{
    // SEC2 channels override the channel manager location for GPFIFO.
    if (uvm_channel_is_sec2(channel))
        return buffer_location_to_string(UVM_BUFFER_LOCATION_SYS);
    return buffer_location_to_string(channel->pool->manager->conf.gpfifo_loc);
}

static const char *get_gpput_location_string(uvm_channel_t *channel)
{
    // SEC2 channels override the channel manager location for GPPUT.
    if (uvm_channel_is_sec2(channel))
        return buffer_location_to_string(UVM_BUFFER_LOCATION_SYS);
    return buffer_location_to_string(channel->pool->manager->conf.gpput_loc);
}

static void uvm_channel_print_info(uvm_channel_t *channel, struct seq_file *s)
{
    UVM_SEQ_OR_DBG_PRINT(s, "Channel %s\n", channel->name);

    channel_pool_lock(channel->pool);

    UVM_SEQ_OR_DBG_PRINT(s, "completed          %llu\n", uvm_channel_update_completed_value(channel));
    UVM_SEQ_OR_DBG_PRINT(s, "queued             %llu\n", channel->tracking_sem.queued_value);
    UVM_SEQ_OR_DBG_PRINT(s, "GPFIFO count       %u\n", channel->num_gpfifo_entries);
    UVM_SEQ_OR_DBG_PRINT(s, "GPFIFO location    %s\n", get_gpfifo_location_string(channel));
    UVM_SEQ_OR_DBG_PRINT(s, "GPPUT location     %s\n", get_gpput_location_string(channel));
    UVM_SEQ_OR_DBG_PRINT(s, "get                %u\n", channel->gpu_get);
    UVM_SEQ_OR_DBG_PRINT(s, "put                %u\n", channel->cpu_put);
    UVM_SEQ_OR_DBG_PRINT(s, "Semaphore GPU VA   0x%llx\n", uvm_channel_tracking_semaphore_get_gpu_va(channel));
    UVM_SEQ_OR_DBG_PRINT(s, "Semaphore CPU VA   0x%llx\n", (NvU64)uvm_gpu_semaphore_get_cpu_va(&channel->tracking_sem.semaphore));

    channel_pool_unlock(channel->pool);
}

static void channel_print_push_acquires(uvm_push_acquire_info_t *push_acquire_info, struct seq_file *seq)
{
    NvU32 i;
    NvU32 valid_entries;

    UVM_ASSERT(uvm_push_info_is_tracking_acquires());
    UVM_ASSERT(push_acquire_info);

    if (push_acquire_info->num_values == 0)
        return;

    valid_entries = min(push_acquire_info->num_values, (NvU32)UVM_PUSH_ACQUIRE_INFO_MAX_ENTRIES);

    for (i = 0; i < valid_entries; ++i) {
        bool is_proxy = push_acquire_info->values[i].is_proxy;

        UVM_SEQ_OR_DBG_PRINT(seq,
                             "%s (gpu %u, channel %d:%u, value %llu)",
                             i == 0? " acquiring values" : "",
                             uvm_id_value(push_acquire_info->values[i].gpu_id),
                             is_proxy? -1 : push_acquire_info->values[i].runlist_id,
                             is_proxy? push_acquire_info->values[i].proxy.pool_index :
                                       push_acquire_info->values[i].channel_id,
                             push_acquire_info->values[i].value);
    }

    if (push_acquire_info->num_values > valid_entries)
        UVM_SEQ_OR_DBG_PRINT(seq, " (missing %u entries)", push_acquire_info->num_values - valid_entries);

    UVM_SEQ_OR_DBG_PRINT(seq, "\n");
}

// Print all pending pushes and up to finished_pushes_count completed if their
// GPFIFO entries haven't been reused yet.
static void channel_print_pushes(uvm_channel_t *channel, NvU32 finished_pushes_count, struct seq_file *seq)
{
    NvU32 gpu_get;
    NvU32 cpu_put;

    NvU64 completed_value = uvm_channel_update_completed_value(channel);

    channel_pool_lock(channel->pool);

    cpu_put = channel->cpu_put;

    for (gpu_get = channel->gpu_get; gpu_get != cpu_put; gpu_get = (gpu_get + 1) % channel->num_gpfifo_entries) {
        uvm_gpfifo_entry_t *entry = &channel->gpfifo_entries[gpu_get];
        uvm_push_info_t *push_info = entry->push_info;
        uvm_push_acquire_info_t *push_acquire_info = NULL;

        if (entry->tracking_semaphore_value + finished_pushes_count <= completed_value)
            continue;

        if (entry->type == UVM_GPFIFO_ENTRY_TYPE_CONTROL) {
            UVM_ASSERT(!uvm_channel_is_proxy(channel));

            UVM_SEQ_OR_DBG_PRINT(seq,
                                 " control GPFIFO entry - data: 0x%llx, gpu_get: %d\n",
                                 entry->control_value,
                                 gpu_get);
        }
        else {

            // Obtain the value acquire tracking information from the push_info
            // index
            if (uvm_push_info_is_tracking_acquires()) {
                NvU32 push_info_index = push_info - channel->push_infos;
                UVM_ASSERT(push_info_index < channel->num_gpfifo_entries);

                push_acquire_info = &channel->push_acquire_infos[push_info_index];
            }

            UVM_SEQ_OR_DBG_PRINT(seq,
                                 " %s push '%s' started at %s:%d in %s() releasing value %llu%s",
                                 entry->tracking_semaphore_value <= completed_value ? "finished" : "pending",
                                 push_info->description,
                                 push_info->filename,
                                 push_info->line,
                                 push_info->function,
                                 entry->tracking_semaphore_value,
                                 !push_acquire_info || push_acquire_info->num_values == 0 ? "\n" : "");

            if (push_acquire_info)
                channel_print_push_acquires(push_acquire_info, seq);
        }
    }
    channel_pool_unlock(channel->pool);
}

void uvm_channel_print_pending_pushes(uvm_channel_t *channel)
{
    channel_print_pushes(channel, 0, NULL);
}

static void channel_manager_print_pending_pushes(uvm_channel_manager_t *manager, struct seq_file *seq)
{
    uvm_channel_pool_t *pool;

    uvm_for_each_pool(pool, manager) {
        uvm_channel_t *channel;

        uvm_for_each_channel_in_pool(channel, pool) {
            UVM_SEQ_OR_DBG_PRINT(seq, "Channel %s, pending pushes:\n", channel->name);

            channel_print_pushes(channel, 0, seq);
        }
    }
}

static NV_STATUS manager_create_procfs_dirs(uvm_channel_manager_t *manager)
{
    uvm_gpu_t *gpu = manager->gpu;

    // The channel manager procfs files are debug only
    if (!uvm_procfs_is_debug_enabled())
        return NV_OK;

    manager->procfs.channels_dir = NV_CREATE_PROC_DIR("channels", gpu->procfs.dir);
    if (manager->procfs.channels_dir == NULL)
        return NV_ERR_OPERATING_SYSTEM;

    return NV_OK;
}

static int nv_procfs_read_manager_pending_pushes(struct seq_file *s, void *v)
{
    uvm_channel_manager_t *manager = (uvm_channel_manager_t *)s->private;

    if (!uvm_down_read_trylock(&g_uvm_global.pm.lock))
        return -EAGAIN;

    channel_manager_print_pending_pushes(manager, s);

    uvm_up_read(&g_uvm_global.pm.lock);

    return 0;
}

static int nv_procfs_read_manager_pending_pushes_entry(struct seq_file *s, void *v)
{
    UVM_ENTRY_RET(nv_procfs_read_manager_pending_pushes(s, v));
}

UVM_DEFINE_SINGLE_PROCFS_FILE(manager_pending_pushes_entry);

static NV_STATUS manager_create_procfs(uvm_channel_manager_t *manager)
{
    uvm_gpu_t *gpu = manager->gpu;

    // The channel manager procfs files are debug only
    if (!uvm_procfs_is_debug_enabled())
        return NV_OK;

    manager->procfs.pending_pushes = NV_CREATE_PROC_FILE("pending_pushes",
                                                         gpu->procfs.dir,
                                                         manager_pending_pushes_entry,
                                                         manager);
    if (manager->procfs.pending_pushes == NULL)
        return NV_ERR_OPERATING_SYSTEM;

    return NV_OK;
}

static int nv_procfs_read_channel_info(struct seq_file *s, void *v)
{
    uvm_channel_t *channel = (uvm_channel_t *)s->private;

    if (!uvm_down_read_trylock(&g_uvm_global.pm.lock))
        return -EAGAIN;

    uvm_channel_print_info(channel, s);

    uvm_up_read(&g_uvm_global.pm.lock);

    return 0;
}

static int nv_procfs_read_channel_info_entry(struct seq_file *s, void *v)
{
    UVM_ENTRY_RET(nv_procfs_read_channel_info(s, v));
}

UVM_DEFINE_SINGLE_PROCFS_FILE(channel_info_entry);

static int nv_procfs_read_channel_pushes(struct seq_file *s, void *v)
{
    uvm_channel_t *channel = (uvm_channel_t *)s->private;

    if (!uvm_down_read_trylock(&g_uvm_global.pm.lock))
        return -EAGAIN;

    // Include up to 5 finished pushes for some context
    channel_print_pushes(channel, 5, s);

    uvm_up_read(&g_uvm_global.pm.lock);

    return 0;
}

static int nv_procfs_read_channel_pushes_entry(struct seq_file *s, void *v)
{
    UVM_ENTRY_RET(nv_procfs_read_channel_pushes(s, v));
}

UVM_DEFINE_SINGLE_PROCFS_FILE(channel_pushes_entry);

static NV_STATUS channel_create_procfs(uvm_channel_t *channel)
{
    char dirname[16];
    uvm_channel_manager_t *manager = channel->pool->manager;

    // The channel procfs files are debug only
    if (!uvm_procfs_is_debug_enabled())
        return NV_OK;

    // For internal channels, the directory name contains the HW IDs. Those are
    // not available for proxy channels, so use -1:<channel index> instead.
    if (uvm_channel_is_proxy(channel))
        snprintf(dirname, sizeof(dirname), "-1:%u", uvm_channel_index_in_pool(channel));
    else
        snprintf(dirname, sizeof(dirname), "%u:%u", channel->channel_info.hwRunlistId, channel->channel_info.hwChannelId);

    channel->procfs.dir = NV_CREATE_PROC_DIR(dirname, manager->procfs.channels_dir);
    if (channel->procfs.dir == NULL)
        return NV_ERR_OPERATING_SYSTEM;

    channel->procfs.info = NV_CREATE_PROC_FILE("info", channel->procfs.dir, channel_info_entry, channel);
    if (channel->procfs.info == NULL)
        return NV_ERR_OPERATING_SYSTEM;

    channel->procfs.pushes = NV_CREATE_PROC_FILE("pushes", channel->procfs.dir, channel_pushes_entry, channel);
    if (channel->procfs.pushes == NULL)
        return NV_ERR_OPERATING_SYSTEM;

    return NV_OK;
}
