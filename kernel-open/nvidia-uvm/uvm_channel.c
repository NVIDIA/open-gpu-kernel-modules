/*******************************************************************************
    Copyright (c) 2015-2022 NVIDIA Corporation

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
#include "uvm_global.h"
#include "uvm_hal.h"
#include "uvm_procfs.h"
#include "uvm_push.h"
#include "uvm_gpu_semaphore.h"
#include "uvm_lock.h"
#include "uvm_kvmalloc.h"

#include "nv_uvm_interface.h"
#include "clb06f.h"

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

    uvm_spin_lock(&channel->pool->lock);

    // Completed value should never exceed the queued value
    UVM_ASSERT_MSG_RELEASE(completed_value <= channel->tracking_sem.queued_value,
                           "GPU %s channel %s unexpected completed_value 0x%llx > queued_value 0x%llx\n",
                           channel->pool->manager->gpu->parent->name, channel->name, completed_value,
                           channel->tracking_sem.queued_value);

    cpu_put = channel->cpu_put;
    gpu_get = channel->gpu_get;

    while (gpu_get != cpu_put && completed_count < max_to_complete) {
        uvm_gpfifo_entry_t *entry = &channel->gpfifo_entries[gpu_get];

        if (mode == UVM_CHANNEL_UPDATE_MODE_COMPLETED && entry->tracking_semaphore_value > completed_value)
            break;

        uvm_pushbuffer_mark_completed(channel->pool->manager->pushbuffer, entry);
        list_add_tail(&entry->push_info->available_list_node, &channel->available_push_infos);
        gpu_get = (gpu_get + 1) % channel->num_gpfifo_entries;
        ++completed_count;
    }

    channel->gpu_get = gpu_get;

    uvm_spin_unlock(&channel->pool->lock);

    if (cpu_put >= gpu_get)
        pending_gpfifos = cpu_put - gpu_get;
    else
        pending_gpfifos = channel->num_gpfifo_entries - gpu_get + cpu_put;

    return pending_gpfifos;
}

NvU32 uvm_channel_update_progress(uvm_channel_t *channel)
{
    // By default, don't complete too many entries at a time to spread the cost
    // of doing so across callers and avoid holding a spin lock for too long.
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

static bool channel_is_available(uvm_channel_t *channel)
{
    NvU32 next_put;

    uvm_assert_spinlock_locked(&channel->pool->lock);

    next_put = (channel->cpu_put + channel->current_pushes_count + 1) % channel->num_gpfifo_entries;

    return (next_put != channel->gpu_get);
}

static bool try_claim_channel(uvm_channel_t *channel)
{
    bool claimed = false;

    uvm_spin_lock(&channel->pool->lock);

    if (channel_is_available(channel)) {
        ++channel->current_pushes_count;
        claimed = true;
    }

    uvm_spin_unlock(&channel->pool->lock);

    return claimed;
}

static void lock_push(uvm_channel_t *channel)
{




}

static void unlock_push(uvm_channel_t *channel)
{




}

static bool trylock_push(uvm_channel_t *channel)
{




    return true;
}

// Reserve a channel in the specified pool
static NV_STATUS channel_reserve_in_pool(uvm_channel_pool_t *pool, uvm_channel_t **channel_out)
{
    uvm_channel_t *channel;
    uvm_spin_loop_t spin;

    UVM_ASSERT(pool);

    uvm_for_each_channel_in_pool(channel, pool) {
        // TODO: Bug 1764953: Prefer idle/less busy channels





        if (trylock_push(channel)) {
            if (try_claim_channel(channel)) {
                *channel_out = channel;
                return NV_OK;
            }
            else {
                unlock_push(channel);
            }
        }
    }

    uvm_spin_loop_init(&spin);
    while (1) {
        uvm_for_each_channel_in_pool(channel, pool) {
            NV_STATUS status;

            uvm_channel_update_progress(channel);

            if (try_claim_channel(channel)) {
                lock_push(channel);
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
	UVM_ASSERT(type < UVM_CHANNEL_TYPE_COUNT);
    return channel_reserve_in_pool(manager->pool_to_use.default_for_type[type], channel_out);
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

    uvm_spin_lock(&channel->pool->lock);

    push_info = list_first_entry_or_null(&channel->available_push_infos, uvm_push_info_t, available_list_node);
    UVM_ASSERT(push_info != NULL);
    UVM_ASSERT(push_info->on_complete == NULL && push_info->on_complete_data == NULL);
    list_del(&push_info->available_list_node);

    uvm_spin_unlock(&channel->pool->lock);

    return push_info - channel->push_infos;
}

NV_STATUS uvm_channel_begin_push(uvm_channel_t *channel, uvm_push_t *push)
{
    NV_STATUS status;
    uvm_channel_manager_t *manager;

    UVM_ASSERT(channel);
    UVM_ASSERT(push);

    manager = channel->pool->manager;








    status = uvm_pushbuffer_begin_push(manager->pushbuffer, push);
    if (status != NV_OK)
        return status;

    push->channel = channel;
    push->channel_tracking_value = 0;
    push->push_info_index = channel_get_available_push_info_index(channel);

    return NV_OK;
}

static void internal_channel_submit_work(uvm_push_t *push, NvU32 push_size, NvU32 new_gpu_put)
{
    NvU64 *gpfifo_entry;
    NvU64 pushbuffer_va;
    uvm_channel_t *channel = push->channel;
    uvm_channel_manager_t *channel_manager = channel->pool->manager;
    uvm_pushbuffer_t *pushbuffer = channel_manager->pushbuffer;
    uvm_gpu_t *gpu = channel_manager->gpu;

    BUILD_BUG_ON(sizeof(*gpfifo_entry) != NVB06F_GP_ENTRY__SIZE);
    UVM_ASSERT(!uvm_channel_is_proxy(channel));

    gpfifo_entry = (NvU64*)channel->channel_info.gpFifoEntries + channel->cpu_put;
    pushbuffer_va = uvm_pushbuffer_get_gpu_va_for_push(pushbuffer, push);

    gpu->parent->host_hal->set_gpfifo_entry(gpfifo_entry, pushbuffer_va, push_size);

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

    // nvUvmInterfacePagingChannelPushStream should not sleep, because a
    // spinlock is currently held.
    uvm_assert_spinlock_locked(&channel->pool->lock);

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

static void uvm_channel_semaphore_release(uvm_push_t *push, NvU64 semaphore_va, NvU32 new_payload)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    // We used to skip the membar or use membar GPU for the semaphore release
    // for a few pushes, but that doesn't provide sufficient ordering guarantees
    // in some cases (e.g. ga100 with an LCE with PCEs from both HSHUBs) for the
    // semaphore writes. To be safe, just always uses a membar sys for now.
    // TODO bug 3770539: Optimize membars used by end of push semaphore releases
    (void)uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_GPU);
    (void)uvm_push_get_and_reset_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);

    if (uvm_channel_is_ce(push->channel))
        gpu->parent->ce_hal->semaphore_release(push, semaphore_va, new_payload);




    else
        UVM_ASSERT_MSG(0, "Semaphore release on an unsupported channel.\n");
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

    uvm_spin_lock(&channel->pool->lock);

    new_tracking_value = ++channel->tracking_sem.queued_value;
    new_payload = (NvU32)new_tracking_value;

    semaphore_va = uvm_channel_tracking_semaphore_get_gpu_va(channel);
    uvm_channel_semaphore_release(push, semaphore_va, new_payload);

    push_size = uvm_push_get_size(push);
    UVM_ASSERT_MSG(push_size <= UVM_MAX_PUSH_SIZE, "push size %u\n", push_size);

    cpu_put = channel->cpu_put;
    new_cpu_put = (cpu_put + 1) % channel->num_gpfifo_entries;

    entry = &channel->gpfifo_entries[cpu_put];
    entry->tracking_semaphore_value = new_tracking_value;
    entry->pushbuffer_offset = uvm_pushbuffer_get_offset_for_push(pushbuffer, push);
    entry->pushbuffer_size = push_size;
    entry->push_info = &channel->push_infos[push->push_info_index];

    UVM_ASSERT(channel->current_pushes_count > 0);
    --channel->current_pushes_count;

    if (uvm_channel_is_proxy(channel))
        proxy_channel_submit_work(push, push_size);
    else
        internal_channel_submit_work(push, push_size, new_cpu_put);

    channel->cpu_put = new_cpu_put;

    uvm_pushbuffer_end_push(pushbuffer, push, entry);

    // The moment the channel is unlocked uvm_channel_update_progress_with_max()
    // may notice the GPU work to be completed and hence all state tracking the
    // push must be updated before that. Notably uvm_pushbuffer_end_push() has
    // to be called first.
    uvm_spin_unlock(&channel->pool->lock);
    unlock_push(channel);

    // This memory barrier is borrowed from CUDA, as it supposedly fixes perf
    // issues on some systems. Comment from CUDA: "fixes throughput-related
    // performance problems, e.g. bugs 626179, 593841. This may be related to
    // bug 124888, which GL works around by doing a clflush"
    wmb();

    push->push_info_index = channel->num_gpfifo_entries;
    push->channel_tracking_value = new_tracking_value;
}

NV_STATUS uvm_channel_reserve(uvm_channel_t *channel)
{
    NV_STATUS status = NV_OK;
    uvm_spin_loop_t spin;

    if (try_claim_channel(channel))
        goto out;

    uvm_channel_update_progress(channel);

    uvm_spin_loop_init(&spin);
    while (!try_claim_channel(channel) && status == NV_OK) {
        UVM_SPIN_LOOP(&spin);
        status = uvm_channel_check_errors(channel);
        uvm_channel_update_progress(channel);
    }

out:
    if (status == NV_OK)
        lock_push(channel);

    return status;
}

// Get the first pending GPFIFO entry, if any.
// This doesn't stop the entry from being reused.
static uvm_gpfifo_entry_t *uvm_channel_get_first_pending_entry(uvm_channel_t *channel)
{
    uvm_gpfifo_entry_t *entry = NULL;
    NvU32 pending_count = channel_update_progress_all(channel, UVM_CHANNEL_UPDATE_MODE_COMPLETED);

    if (pending_count == 0)
        return NULL;

    uvm_spin_lock(&channel->pool->lock);

    if (channel->gpu_get != channel->cpu_put)
        entry = &channel->gpfifo_entries[channel->gpu_get];

    uvm_spin_unlock(&channel->pool->lock);

    return entry;
}

NV_STATUS uvm_channel_get_status(uvm_channel_t *channel)
{
    uvm_gpu_t *gpu;
    NvNotification *errorNotifier;

    if (uvm_channel_is_proxy(channel))
        errorNotifier = channel->proxy.channel_info.shadowErrorNotifier;
    else
        errorNotifier = channel->channel_info.errorNotifier;

    if (errorNotifier->status == 0)
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
        uvm_push_info_t *push_info = fatal_entry->push_info;
        UVM_ERR_PRINT("Channel error likely caused by push '%s' started at %s:%d in %s()\n",
                push_info->description, push_info->filename, push_info->line, push_info->function);
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

    uvm_procfs_destroy_entry(channel->procfs.pushes);
    uvm_procfs_destroy_entry(channel->procfs.info);
    uvm_procfs_destroy_entry(channel->procfs.dir);

    uvm_kvfree(channel->push_acquire_infos);
    uvm_kvfree(channel->push_infos);

    uvm_kvfree(channel->gpfifo_entries);





    if (uvm_channel_is_proxy(channel))
        uvm_rm_locked_call_void(nvUvmInterfacePagingChannelDestroy(channel->proxy.handle));
    else
        uvm_rm_locked_call_void(nvUvmInterfaceChannelDestroy(channel->handle));

    uvm_gpu_tracking_semaphore_free(&channel->tracking_sem);

    UVM_ASSERT(list_empty(&channel->tools.channel_list_node));
    UVM_ASSERT(channel->tools.pending_event_count == 0);

    pool->num_channels--;
}

static NV_STATUS internal_channel_create(uvm_channel_t *channel, unsigned engine_index)
{
    NV_STATUS status;
    UvmGpuChannelAllocParams channel_alloc_params;
    UvmGpuChannelInfo *channel_info = &channel->channel_info;
    uvm_channel_manager_t *manager = channel->pool->manager;
    uvm_gpu_t *gpu = manager->gpu;

    if (uvm_channel_is_ce(channel)) {
        UVM_ASSERT(channel->pool->pool_type == UVM_CHANNEL_POOL_TYPE_CE);





    }

    memset(&channel_alloc_params, 0, sizeof(channel_alloc_params));
    channel_alloc_params.numGpFifoEntries = manager->conf.num_gpfifo_entries;
    channel_alloc_params.gpFifoLoc = manager->conf.gpfifo_loc;
    channel_alloc_params.gpPutLoc = manager->conf.gpput_loc;
    channel_alloc_params.engineIndex = engine_index;

    if (uvm_channel_is_ce(channel))
        channel_alloc_params.engineType = UVM_GPU_CHANNEL_ENGINE_TYPE_CE;





    status = uvm_rm_locked_call(nvUvmInterfaceChannelAllocate(gpu->rm_address_space,
                                                              &channel_alloc_params,
                                                              &channel->handle,
                                                              channel_info));
    if (status != NV_OK) {
        UVM_ERR_PRINT("nvUvmInterfaceChannelAllocate() failed: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_gpu_name(gpu));
        return status;
    }

    snprintf(channel->name,
             sizeof(channel->name),
             "ID %u:%u (0x%x:0x%x) %s %u",
             channel_info->hwRunlistId,
             channel_info->hwChannelId,
             channel_info->hwRunlistId,
             channel_info->hwChannelId,



             "CE",

             engine_index);

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
    unsigned int i;

    UVM_ASSERT(channel != NULL);

    channel->pool = pool;
    pool->num_channels++;
    INIT_LIST_HEAD(&channel->available_push_infos);
    channel->tools.pending_event_count = 0;
    INIT_LIST_HEAD(&channel->tools.channel_list_node);

    status = uvm_gpu_tracking_semaphore_alloc(gpu->semaphore_pool, &channel->tracking_sem);
    if (status != NV_OK) {
        UVM_ERR_PRINT("uvm_gpu_tracking_semaphore_alloc() failed: %s, GPU %s\n",
                      nvstatusToString(status),
                      uvm_gpu_name(gpu));
        goto error;
    }

     if (uvm_channel_is_proxy(channel))
         status = proxy_channel_create(channel, pool->engine_index);
     else
         status = internal_channel_create(channel, pool->engine_index);

     if (status != NV_OK)
         goto error;







    channel->num_gpfifo_entries = manager->conf.num_gpfifo_entries;
    channel->gpfifo_entries = uvm_kvmalloc_zero(sizeof(*channel->gpfifo_entries) * channel->num_gpfifo_entries);
    if (channel->gpfifo_entries == NULL) {
        status = NV_ERR_NO_MEMORY;
        goto error;
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

static NV_STATUS init_channel(uvm_channel_t *channel)
{
    uvm_push_t push;
    uvm_gpu_t *gpu = uvm_channel_get_gpu(channel);
    NV_STATUS status = uvm_push_begin_on_channel(channel, &push, "Init channel");

    if (status != NV_OK) {
        UVM_ERR_PRINT("Failed to begin push on channel: %s, GPU %s\n", nvstatusToString(status), uvm_gpu_name(gpu));
        return status;
    }

    if (uvm_channel_is_ce(channel))
        gpu->parent->ce_hal->init(&push);





    gpu->parent->host_hal->init(&push);

    status = uvm_push_end_and_wait(&push);
    if (status != NV_OK)
        UVM_ERR_PRINT("Channel init failed: %s, GPU %s\n", nvstatusToString(status), uvm_gpu_name(gpu));

    return status;
}

static bool channel_manager_uses_proxy_pool(uvm_channel_manager_t *manager)
{
    return uvm_gpu_is_virt_mode_sriov_heavy(manager->gpu);
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

    return 2;
}

static void channel_pool_destroy(uvm_channel_pool_t *pool)
{
    UVM_ASSERT(pool->manager->num_channel_pools > 0);

    while (pool->num_channels > 0)
        channel_destroy(pool, pool->channels + pool->num_channels - 1);

    uvm_kvfree(pool->channels);
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
    uvm_channel_pool_t *pool;

    pool = channel_manager->channel_pools + channel_manager->num_channel_pools;
    pool->manager = channel_manager;
    pool->engine_index = engine_index;
    pool->pool_type = pool_type;

    uvm_spin_lock_init(&pool->lock, UVM_LOCK_ORDER_CHANNEL);

    num_channels = channel_pool_type_num_channels(pool_type);

    pool->channels = uvm_kvmalloc_zero(sizeof(*pool->channels) * num_channels);
    if (!pool->channels)
        return NV_ERR_NO_MEMORY;

    channel_manager->num_channel_pools++;

    for (i = 0; i < num_channels; i++) {
        uvm_channel_t *channel = pool->channels + i;

        status = channel_create(pool, channel);
        if (status != NV_OK)
            goto error;

        status = init_channel(channel);
        if (status != NV_OK)
            goto error;
    }

    *pool_out = pool;
    return NV_OK;

 error:
    channel_pool_destroy(pool);
    return status;
}

static bool ce_usable_for_channel_type(uvm_channel_type_t type, const UvmGpuCopyEngineCaps *cap)
{
    if (!cap->supported || cap->grce)
        return false;

    switch (type) {
        case UVM_CHANNEL_TYPE_CPU_TO_GPU:
        case UVM_CHANNEL_TYPE_GPU_TO_CPU:
            return cap->sysmem;
        case UVM_CHANNEL_TYPE_GPU_INTERNAL:
        case UVM_CHANNEL_TYPE_MEMOPS:
            return true;
        case UVM_CHANNEL_TYPE_GPU_TO_GPU:
            return cap->p2p;
        default:
            UVM_ASSERT_MSG(false, "Unexpected channel type 0x%x\n", type);
            return false;
    }
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

    UVM_ASSERT(ce_usable_for_channel_type(type, cap0));
    UVM_ASSERT(ce_usable_for_channel_type(type, cap1));
    UVM_ASSERT(ce_index0 < UVM_COPY_ENGINE_COUNT_MAX);
    UVM_ASSERT(ce_index1 < UVM_COPY_ENGINE_COUNT_MAX);
    UVM_ASSERT(ce_index0 != ce_index1);

    switch (type) {
        case UVM_CHANNEL_TYPE_CPU_TO_GPU:
            // For CPU to GPU fast sysmem read is the most important
            if (cap0->sysmemRead != cap1->sysmemRead)
                return cap1->sysmemRead - cap0->sysmemRead;

            // Prefer not to take up the CEs for nvlink P2P
            if (cap0->nvlinkP2p != cap1->nvlinkP2p)
                return cap0->nvlinkP2p - cap1->nvlinkP2p;

            break;

        case UVM_CHANNEL_TYPE_GPU_TO_CPU:
            // For GPU to CPU fast sysmem write is the most important
            if (cap0->sysmemWrite != cap1->sysmemWrite)
                return cap1->sysmemWrite - cap0->sysmemWrite;

            // Prefer not to take up the CEs for nvlink P2P
            if (cap0->nvlinkP2p != cap1->nvlinkP2p)
                return cap0->nvlinkP2p - cap1->nvlinkP2p;

            break;

        case UVM_CHANNEL_TYPE_GPU_TO_GPU:
            // Prefer the LCE with the most PCEs
            {
                int pce_diff = (int)hweight32(cap1->cePceMask) - (int)hweight32(cap0->cePceMask);

                if (pce_diff != 0)
                    return pce_diff;
            }

            break;

        case UVM_CHANNEL_TYPE_GPU_INTERNAL:
            // We want the max possible bandwidth for CEs used for GPU_INTERNAL,
            // for now assume that the number of PCEs is a good measure.
            // TODO: Bug 1735254: Add a direct CE query for local FB bandwidth
            {
                int pce_diff = (int)hweight32(cap1->cePceMask) - (int)hweight32(cap0->cePceMask);

                if (pce_diff != 0)
                    return pce_diff;
            }

            // Leave P2P CEs to the GPU_TO_GPU channel type, when possible
            if (cap0->nvlinkP2p != cap1->nvlinkP2p)
                return cap0->nvlinkP2p - cap1->nvlinkP2p;

            break;

        case UVM_CHANNEL_TYPE_MEMOPS:
            // For MEMOPS we mostly care about latency which should be better
            // with less used CEs (although we only know about our own usage and
            // not system-wide) so just break out to get the default ordering
            // which prioritizes usage count.
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

// Identify usable CEs, and select the preferred CE for a given channel type.
static NV_STATUS pick_ce_for_channel_type(uvm_channel_manager_t *manager,
                                          const UvmGpuCopyEngineCaps *ce_caps,
                                          uvm_channel_type_t type,
                                          unsigned *preferred_ce)
{
    NvU32 i;
    NvU32 best_ce = UVM_COPY_ENGINE_COUNT_MAX;

    UVM_ASSERT(type < UVM_CHANNEL_TYPE_CE_COUNT);

    for (i = 0; i < UVM_COPY_ENGINE_COUNT_MAX; ++i) {
        const UvmGpuCopyEngineCaps *cap = ce_caps + i;

        if (!ce_usable_for_channel_type(type, cap))
            continue;

        __set_bit(i, manager->ce_mask);

        if (best_ce == UVM_COPY_ENGINE_COUNT_MAX) {
            best_ce = i;
            continue;
        }

        if (compare_ce_for_channel_type(ce_caps, type, i, best_ce, preferred_ce) < 0)
            best_ce = i;
    }

    if (best_ce == UVM_COPY_ENGINE_COUNT_MAX) {
        UVM_ERR_PRINT("Failed to find a suitable CE for channel type %s\n", uvm_channel_type_to_string(type));
        return NV_ERR_NOT_SUPPORTED;
    }

    preferred_ce[type] = best_ce;
    return NV_OK;
}

static NV_STATUS channel_manager_pick_copy_engines(uvm_channel_manager_t *manager, unsigned *preferred_ce)
{
    NV_STATUS status;
    unsigned i;
    UvmGpuCopyEnginesCaps ces_caps;
    uvm_channel_type_t types[] = {UVM_CHANNEL_TYPE_CPU_TO_GPU,
                                  UVM_CHANNEL_TYPE_GPU_TO_CPU,
                                  UVM_CHANNEL_TYPE_GPU_INTERNAL,
                                  UVM_CHANNEL_TYPE_GPU_TO_GPU,
                                  UVM_CHANNEL_TYPE_MEMOPS};

    memset(&ces_caps, 0, sizeof(ces_caps));
    status = uvm_rm_locked_call(nvUvmInterfaceQueryCopyEnginesCaps(uvm_gpu_device_handle(manager->gpu), &ces_caps));
    if (status != NV_OK)
        return status;

   // The order of picking CEs for each type matters as it's affected by the
   // usage count of each CE and it increases every time a CE is selected.
   // MEMOPS has the least priority as it only cares about low usage of the
   // CE to improve latency
    for (i = 0; i < ARRAY_SIZE(types); ++i) {
        status = pick_ce_for_channel_type(manager, ces_caps.copyEngineCaps, types[i], preferred_ce);
        if (status != NV_OK)
            return status;
    }

    return NV_OK;
}

// Return the pool corresponding to the given CE index
//
// This function cannot be used to access the proxy pool in SR-IOV heavy.
static uvm_channel_pool_t *channel_manager_ce_pool(uvm_channel_manager_t *manager, NvU32 ce)
{
    uvm_channel_pool_t *pool;

    UVM_ASSERT(test_bit(ce, manager->ce_mask));

    // The index of the pool associated with 'ce' is the number of usable CEs
    // in [0, ce)
    pool = manager->channel_pools + bitmap_weight(manager->ce_mask, ce);

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

    UVM_ASSERT_MSG(false, "Invalid buffer locationvalue %d\n", loc);
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

    // By default we place GPFIFO and GPPUT on vidmem as it potentially has
    // lower latency.
    manager->conf.gpfifo_loc = UVM_BUFFER_LOCATION_VID;
    manager->conf.gpput_loc = UVM_BUFFER_LOCATION_VID;

    // TODO: Bug 1766129: However, this will likely be different on P9 systems.
    // Leaving GPFIFO on sysmem for now. GPPut on sysmem is not supported in
    // production, so we keep it on vidmem, too.
    if (gpu->parent->sysmem_link >= UVM_GPU_LINK_NVLINK_2)
        manager->conf.gpfifo_loc = UVM_BUFFER_LOCATION_SYS;

    // Override defaults
    if (string_to_buffer_location(gpfifo_loc_value) != UVM_BUFFER_LOCATION_DEFAULT)
        manager->conf.gpfifo_loc = string_to_buffer_location(gpfifo_loc_value);

    if (string_to_buffer_location(gpput_loc_value) != UVM_BUFFER_LOCATION_DEFAULT)
        manager->conf.gpput_loc = string_to_buffer_location(gpput_loc_value);
}

// A pool is created for each usable CE, even if it has not been selected as the
// preferred CE for any type, because as more information is discovered (for
// example, a pair of peer GPUs is added) we may start using the previously idle
// channels.
static NV_STATUS channel_manager_create_pools(uvm_channel_manager_t *manager)
{
    NV_STATUS status;
    unsigned ce, type;
    unsigned num_channel_pools;
    unsigned preferred_ce[UVM_CHANNEL_TYPE_CE_COUNT];
    uvm_channel_pool_t *pool = NULL;

    for (type = 0; type < ARRAY_SIZE(preferred_ce); type++)
        preferred_ce[type] = UVM_COPY_ENGINE_COUNT_MAX;

    status = channel_manager_pick_copy_engines(manager, preferred_ce);
    if (status != NV_OK)
        return status;

    // CE channel pools
    num_channel_pools = bitmap_weight(manager->ce_mask, UVM_COPY_ENGINE_COUNT_MAX);

    // CE proxy channel pool.
    if (uvm_gpu_uses_proxy_channel_pool(manager->gpu))
        num_channel_pools++;







    manager->channel_pools = uvm_kvmalloc_zero(sizeof(*manager->channel_pools) * num_channel_pools);
    if (!manager->channel_pools)
        return NV_ERR_NO_MEMORY;

    for_each_set_bit(ce, manager->ce_mask, UVM_COPY_ENGINE_COUNT_MAX) {
        status = channel_pool_add(manager, UVM_CHANNEL_POOL_TYPE_CE, ce, &pool);
        if (status != NV_OK)
            return status;
    }

    // Assign channel types to pools
    for (type = 0; type < ARRAY_SIZE(preferred_ce); type++) {
        unsigned ce = preferred_ce[type];

        UVM_ASSERT(test_bit(ce, manager->ce_mask));

        manager->pool_to_use.default_for_type[type] = channel_manager_ce_pool(manager, ce);
    }

    // In SR-IOV heavy, add an additional, single-channel, pool that is
    // dedicated to the MEMOPS type.
    if (uvm_gpu_uses_proxy_channel_pool(manager->gpu)) {
        uvm_channel_type_t channel_type = uvm_channel_proxy_channel_type();
        status = channel_pool_add(manager, UVM_CHANNEL_POOL_TYPE_CE_PROXY, preferred_ce[channel_type], &pool);
        if (status != NV_OK)
            return status;

        manager->pool_to_use.default_for_type[channel_type] = pool;
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
    while (manager->num_channel_pools > 0)
        channel_pool_destroy(manager->channel_pools + manager->num_channel_pools - 1);

    uvm_kvfree(manager->channel_pools);
}

void uvm_channel_manager_destroy(uvm_channel_manager_t *channel_manager)
{
    if (channel_manager == NULL)
        return;

    uvm_procfs_destroy_entry(channel_manager->procfs.pending_pushes);

    channel_manager_destroy_pools(channel_manager);

    uvm_procfs_destroy_entry(channel_manager->procfs.channels_dir);

    uvm_pushbuffer_destroy(channel_manager->pushbuffer);

    uvm_kvfree(channel_manager);
}

bool uvm_channel_is_privileged(uvm_channel_t *channel)
{
    if (uvm_gpu_is_virt_mode_sriov_heavy(uvm_channel_get_gpu(channel)))
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



    BUILD_BUG_ON(UVM_CHANNEL_TYPE_COUNT != 5);


    switch (channel_type) {
        UVM_ENUM_STRING_CASE(UVM_CHANNEL_TYPE_CPU_TO_GPU);
        UVM_ENUM_STRING_CASE(UVM_CHANNEL_TYPE_GPU_TO_CPU);
        UVM_ENUM_STRING_CASE(UVM_CHANNEL_TYPE_GPU_INTERNAL);
        UVM_ENUM_STRING_CASE(UVM_CHANNEL_TYPE_MEMOPS);
        UVM_ENUM_STRING_CASE(UVM_CHANNEL_TYPE_GPU_TO_GPU);



        UVM_ENUM_STRING_DEFAULT();
    }
}

const char *uvm_channel_pool_type_to_string(uvm_channel_pool_type_t channel_pool_type)
{



    BUILD_BUG_ON(UVM_CHANNEL_POOL_TYPE_COUNT != 2);


    switch (channel_pool_type) {
        UVM_ENUM_STRING_CASE(UVM_CHANNEL_POOL_TYPE_CE);
        UVM_ENUM_STRING_CASE(UVM_CHANNEL_POOL_TYPE_CE_PROXY);



        UVM_ENUM_STRING_DEFAULT();
    }
}

static void uvm_channel_print_info(uvm_channel_t *channel, struct seq_file *s)
{
    uvm_channel_manager_t *manager = channel->pool->manager;
    UVM_SEQ_OR_DBG_PRINT(s, "Channel %s\n", channel->name);

    uvm_spin_lock(&channel->pool->lock);

    UVM_SEQ_OR_DBG_PRINT(s, "completed          %llu\n", uvm_channel_update_completed_value(channel));
    UVM_SEQ_OR_DBG_PRINT(s, "queued             %llu\n", channel->tracking_sem.queued_value);
    UVM_SEQ_OR_DBG_PRINT(s, "GPFIFO count       %u\n", channel->num_gpfifo_entries);
    UVM_SEQ_OR_DBG_PRINT(s, "GPFIFO location    %s\n", buffer_location_to_string(manager->conf.gpfifo_loc));
    UVM_SEQ_OR_DBG_PRINT(s, "GPPUT location     %s\n", buffer_location_to_string(manager->conf.gpput_loc));
    UVM_SEQ_OR_DBG_PRINT(s, "get                %u\n", channel->gpu_get);
    UVM_SEQ_OR_DBG_PRINT(s, "put                %u\n", channel->cpu_put);
    UVM_SEQ_OR_DBG_PRINT(s, "Semaphore GPU VA   0x%llx\n", uvm_channel_tracking_semaphore_get_gpu_va(channel));
    UVM_SEQ_OR_DBG_PRINT(s, "Semaphore CPU VA   0x%llx\n", (NvU64)(uintptr_t)channel->tracking_sem.semaphore.payload);

    uvm_spin_unlock(&channel->pool->lock);
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

    uvm_spin_lock(&channel->pool->lock);

    cpu_put = channel->cpu_put;

    for (gpu_get = channel->gpu_get; gpu_get != cpu_put; gpu_get = (gpu_get + 1) % channel->num_gpfifo_entries) {
        uvm_gpfifo_entry_t *entry = &channel->gpfifo_entries[gpu_get];
        uvm_push_info_t *push_info = entry->push_info;
        uvm_push_acquire_info_t *push_acquire_info = NULL;

        if (entry->tracking_semaphore_value + finished_pushes_count <= completed_value)
            continue;

        // Obtain the value acquire tracking information from the push_info index
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
                             !push_acquire_info || push_acquire_info->num_values == 0? "\n" : "");

        if (push_acquire_info)
            channel_print_push_acquires(push_acquire_info, seq);
    }
    uvm_spin_unlock(&channel->pool->lock);
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
