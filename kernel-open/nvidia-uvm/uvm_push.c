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

#include "uvm_extern_decl.h"
#include "uvm_forward_decl.h"
#include "uvm_push.h"
#include "uvm_channel.h"
#include "uvm_global.h"
#include "uvm_hal.h"
#include "uvm_kvmalloc.h"
#include "uvm_linux.h"
#include "nv_stdarg.h"

// This parameter enables push description tracking in push info. It's enabled
// by default for debug and develop builds and disabled for release builds.
static unsigned uvm_debug_enable_push_desc = UVM_IS_DEBUG() || UVM_IS_DEVELOP();
module_param(uvm_debug_enable_push_desc, uint, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(uvm_debug_enable_push_desc, "Enable push description tracking");

static unsigned uvm_debug_enable_push_acquire_info = 0;
module_param(uvm_debug_enable_push_acquire_info, uint, S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(uvm_debug_enable_push_acquire_info, "Enable push acquire information tracking");

static uvm_push_acquire_info_t *push_acquire_info_from_push(uvm_push_t *push)
{
    uvm_channel_t *channel = push->channel;

    UVM_ASSERT(channel != NULL);
    UVM_ASSERT(push->channel_tracking_value == 0);

    UVM_ASSERT_MSG(push->push_info_index < channel->num_gpfifo_entries, "index %u\n", push->push_info_index);

    if (!uvm_debug_enable_push_acquire_info)
        return NULL;

    return &channel->push_acquire_infos[push->push_info_index];
}

bool uvm_push_allow_dependencies_across_gpus(void)
{
    // In Confidential Computing a GPU semaphore release cannot be waited on
    // (acquired by) any other GPU, due to a mix of HW and SW constraints.
    return !g_uvm_global.conf_computing_enabled;
}

// Acquire a single tracker entry. Subsequently pushed GPU work will not start
// before the work tracked by tracker entry is complete.
static void push_acquire_tracker_entry(uvm_push_t *push,
                                       uvm_tracker_entry_t *tracker_entry,
                                       uvm_push_acquire_info_t *push_acquire_info)
{
    uvm_channel_t *entry_channel;
    uvm_channel_t *channel;
    NvU64 semaphore_va;
    uvm_gpu_t *gpu;

    UVM_ASSERT(push != NULL);
    UVM_ASSERT(tracker_entry != NULL);

    entry_channel = tracker_entry->channel;
    if (entry_channel == NULL)
        return;

    channel = push->channel;
    if (channel == entry_channel)
        return;

    gpu = uvm_channel_get_gpu(channel);

    // If dependencies across GPUs are disallowed, the caller is required to
    // previously wait on such dependencies.
    if (gpu != uvm_tracker_entry_gpu(tracker_entry))
        UVM_ASSERT(uvm_push_allow_dependencies_across_gpus());

    semaphore_va = uvm_channel_tracking_semaphore_get_gpu_va_in_channel(entry_channel, channel);
    gpu->parent->host_hal->semaphore_acquire(push, semaphore_va, (NvU32)tracker_entry->value);

    if (push_acquire_info) {
        const NvU32 num_values = push_acquire_info->num_values;

        UVM_ASSERT(uvm_debug_enable_push_acquire_info);

        if (num_values < UVM_PUSH_ACQUIRE_INFO_MAX_ENTRIES) {
            push_acquire_info->values[num_values].value = tracker_entry->value;
            push_acquire_info->values[num_values].gpu_id = uvm_channel_get_gpu(entry_channel)->id;
            push_acquire_info->values[num_values].is_proxy = uvm_channel_is_proxy(channel);

            if (uvm_channel_is_proxy(channel)) {
                push_acquire_info->values[num_values].proxy.pool_index = uvm_channel_index_in_pool(channel);
            }
            else {
                push_acquire_info->values[num_values].runlist_id = entry_channel->channel_info.hwRunlistId;
                push_acquire_info->values[num_values].channel_id = entry_channel->channel_info.hwChannelId;
            }
        }
        ++push_acquire_info->num_values;
    }
}

void uvm_push_acquire_tracker(uvm_push_t *push, uvm_tracker_t *tracker)
{
    uvm_tracker_entry_t *entry;
    uvm_push_acquire_info_t *push_acquire_info;

    UVM_ASSERT(push != NULL);

    if (tracker == NULL)
        return;

    uvm_tracker_remove_completed(tracker);

    push_acquire_info = push_acquire_info_from_push(push);

    for_each_tracker_entry(entry, tracker)
        push_acquire_tracker_entry(push, entry, push_acquire_info);
}

static NV_STATUS push_reserve_channel(uvm_channel_manager_t *manager,
                                      uvm_channel_type_t channel_type,
                                      uvm_gpu_t *dst_gpu,
                                      uvm_channel_t **channel)
{
    NV_STATUS status;

    // Pick a channel and reserve a GPFIFO entry
    // TODO: Bug 1764953: use the dependencies in the tracker to pick a channel
    //       in a smarter way.
    if (dst_gpu == NULL)
        status = uvm_channel_reserve_type(manager, channel_type, channel);
    else
        status = uvm_channel_reserve_gpu_to_gpu(manager, dst_gpu, channel);

    if (status == NV_OK)
        UVM_ASSERT(*channel);

    return status;
}

static void push_set_description(uvm_push_t *push, const char *format, va_list args)
{
    uvm_push_info_t *push_info;

    UVM_ASSERT(uvm_push_info_is_tracking_descriptions());

    push_info = uvm_push_info_from_push(push);
    vsnprintf(push_info->description, sizeof(push_info->description), format, args);
}

void uvm_push_set_description(uvm_push_t *push, const char *format, ...)
{
    va_list va;

    if (!uvm_push_info_is_tracking_descriptions())
        return;

    va_start(va, format);
    push_set_description(push, format, va);
    va_end(va);
}

// Internal helper to fill info push info as part of beginning a push.
static void push_fill_info(uvm_push_t *push,
                           const char *filename,
                           const char *function,
                           int line,
                           const char *format,
                           va_list args)
{
    uvm_push_acquire_info_t *push_acquire_info;
    uvm_push_info_t *push_info = uvm_push_info_from_push(push);

    push_info->filename = kbasename(filename);
    push_info->function = function;
    push_info->line = line;

    push_acquire_info = push_acquire_info_from_push(push);
    if (push_acquire_info)
        push_acquire_info->num_values = 0;

    if (uvm_push_info_is_tracking_descriptions())
        push_set_description(push, format, args);
}

static NV_STATUS wait_for_other_gpus_if_needed(uvm_tracker_t *tracker, uvm_gpu_t *gpu)
{
    if (tracker == NULL)
        return NV_OK;

    if (uvm_push_allow_dependencies_across_gpus())
        return NV_OK;

    return uvm_tracker_wait_for_other_gpus(tracker, gpu);
}

static NV_STATUS push_begin_acquire_with_info(uvm_channel_t *channel,
                                              uvm_tracker_t *tracker,
                                              uvm_push_t *push,
                                              const char *filename,
                                              const char *function,
                                              int line,
                                              const char *format,
                                              va_list args)
{
    NV_STATUS status;

    memset(push, 0, sizeof(*push));

    push->gpu = uvm_channel_get_gpu(channel);

    status = uvm_channel_begin_push(channel, push);
    if (status != NV_OK)
        return status;

    push_fill_info(push, filename, function, line, format, args);

    uvm_push_acquire_tracker(push, tracker);

    return NV_OK;
}

__attribute__ ((format(printf, 9, 10)))
NV_STATUS __uvm_push_begin_acquire_with_info(uvm_channel_manager_t *manager,
                                             uvm_channel_type_t type,
                                             uvm_gpu_t *dst_gpu,
                                             uvm_tracker_t *tracker,
                                             uvm_push_t *push,
                                             const char *filename,
                                             const char *function,
                                             int line,
                                             const char *format, ...)
{
    va_list args;
    NV_STATUS status;
    uvm_channel_t *channel;

    if (dst_gpu != NULL) {
        UVM_ASSERT(type == UVM_CHANNEL_TYPE_GPU_TO_GPU);
        UVM_ASSERT(dst_gpu != manager->gpu);
    }

    status = wait_for_other_gpus_if_needed(tracker, manager->gpu);
    if (status != NV_OK)
        return status;

    status = push_reserve_channel(manager, type, dst_gpu, &channel);
    if (status != NV_OK)
        return status;

    UVM_ASSERT(channel);

    va_start(args, format);
    status = push_begin_acquire_with_info(channel, tracker, push, filename, function, line, format, args);
    va_end(args);

    if (status != NV_OK)
        uvm_channel_release(channel, 1);

    return status;
}

__attribute__ ((format(printf, 7, 8)))
NV_STATUS __uvm_push_begin_acquire_on_channel_with_info(uvm_channel_t *channel,
                                                        uvm_tracker_t *tracker,
                                                        uvm_push_t *push,
                                                        const char *filename,
                                                        const char *function,
                                                        int line,
                                                        const char *format, ...)
{
    va_list args;
    NV_STATUS status;

    status = wait_for_other_gpus_if_needed(tracker, uvm_channel_get_gpu(channel));
    if (status != NV_OK)
        return status;

    status = uvm_channel_reserve(channel, 1);
    if (status != NV_OK)
        return status;

    va_start(args, format);
    status = push_begin_acquire_with_info(channel, tracker, push, filename, function, line, format, args);
    va_end(args);

    if (status != NV_OK)
        uvm_channel_release(channel, 1);

    return status;
}

__attribute__ ((format(printf, 6, 7)))
NV_STATUS __uvm_push_begin_on_reserved_channel_with_info(uvm_channel_t *channel,
                                                         uvm_push_t *push,
                                                         const char *filename,
                                                         const char *function,
                                                         int line,
                                                         const char *format, ...)
{
    va_list args;
    NV_STATUS status;

    va_start(args, format);
    status = push_begin_acquire_with_info(channel, NULL, push, filename, function, line, format, args);
    va_end(args);

    return status;
}

bool uvm_push_info_is_tracking_descriptions(void)
{
    return uvm_debug_enable_push_desc != 0;
}

bool uvm_push_info_is_tracking_acquires(void)
{
    return uvm_debug_enable_push_acquire_info != 0;
}

void uvm_push_end(uvm_push_t *push)
{
    uvm_push_flag_t flag;

    uvm_channel_end_push(push);

    flag = find_first_bit(push->flags, UVM_PUSH_FLAG_COUNT);

    // All flags should be reset by the end of the push
    UVM_ASSERT_MSG(flag == UVM_PUSH_FLAG_COUNT, "first flag set %d\n", flag);
}

NV_STATUS uvm_push_wait(uvm_push_t *push)
{
    uvm_tracker_entry_t entry;

    uvm_push_get_tracker_entry(push, &entry);

    return uvm_tracker_wait_for_entry(&entry);
}

NV_STATUS uvm_push_end_and_wait(uvm_push_t *push)
{
    uvm_push_end(push);

    return uvm_push_wait(push);
}

NV_STATUS uvm_push_begin_fake(uvm_gpu_t *gpu, uvm_push_t *push)
{
    memset(push, 0, sizeof(*push));
    push->begin = (NvU32 *)uvm_kvmalloc(UVM_MAX_PUSH_SIZE);
    if (!push->begin)
        return NV_ERR_NO_MEMORY;

    push->next = push->begin;
    push->gpu = gpu;

    return NV_OK;
}

void uvm_push_end_fake(uvm_push_t *push)
{
    uvm_kvfree(push->begin);
    push->begin = NULL;
}

void *uvm_push_inline_data_get(uvm_push_inline_data_t *data, size_t size)
{
    void *buffer = data->next_data;

    UVM_ASSERT(!uvm_global_is_suspended());

    UVM_ASSERT_MSG(uvm_push_get_size(data->push) + uvm_push_inline_data_size(data) + UVM_METHOD_SIZE + size <= UVM_MAX_PUSH_SIZE,
                   "push size %u inline data size %zu new data size %zu max push %u\n",
                   uvm_push_get_size(data->push), uvm_push_inline_data_size(data), size, UVM_MAX_PUSH_SIZE);
    UVM_ASSERT_MSG(uvm_push_inline_data_size(data) + size <= UVM_PUSH_INLINE_DATA_MAX_SIZE,
                   "inline data size %zu new data size %zu max %u\n",
                   uvm_push_inline_data_size(data), size, UVM_PUSH_INLINE_DATA_MAX_SIZE);

    data->next_data += size;

    return buffer;
}

void *uvm_push_inline_data_get_aligned(uvm_push_inline_data_t *data, size_t size, size_t alignment)
{
    NvU64 next_ptr = (NvU64)(uintptr_t)data->next_data;
    size_t offset = 0;
    char *buffer;

    UVM_ASSERT(alignment <= UVM_PAGE_SIZE_4K);
    UVM_ASSERT_MSG(IS_ALIGNED(alignment, UVM_METHOD_SIZE), "alignment %zu\n", alignment);

    offset = UVM_ALIGN_UP(next_ptr, alignment) - next_ptr;

    buffer = (char *)uvm_push_inline_data_get(data, size + offset);
    return buffer + offset;
}

uvm_gpu_address_t uvm_push_inline_data_end(uvm_push_inline_data_t *data)
{
    NvU64 inline_data_address;
    uvm_push_t *push = data->push;
    uvm_channel_t *channel = push->channel;

    // Round up the inline data size to the method size
    size_t noop_size = roundup(uvm_push_inline_data_size(data), UVM_METHOD_SIZE);

    if (channel == NULL) {
        // Fake push, just return the CPU address.
        inline_data_address = (NvU64) (uintptr_t)(push->next + 1);
    }
    else {
        uvm_pushbuffer_t *pushbuffer = uvm_channel_get_pushbuffer(channel);

        // Offset of the inlined data within the push.
        inline_data_address = (push->next - push->begin + 1) * UVM_METHOD_SIZE;

        // Add GPU VA of the push begin
        inline_data_address += uvm_pushbuffer_get_gpu_va_for_push(pushbuffer, push);
    }

    // This will place a noop right before the inline data that was written.
    // Plus UVM_METHOD_SIZE for the noop method itself.
    uvm_push_get_gpu(push)->parent->host_hal->noop(push, noop_size + UVM_METHOD_SIZE);

    return uvm_gpu_address_virtual(inline_data_address);
}

void *uvm_push_get_single_inline_buffer(uvm_push_t *push,
                                        size_t size,
                                        size_t alignment,
                                        uvm_gpu_address_t *gpu_address)
{
    uvm_push_inline_data_t data;
    void *buffer;

    UVM_ASSERT(IS_ALIGNED(alignment, UVM_METHOD_SIZE));

    uvm_push_inline_data_begin(push, &data);
    buffer = uvm_push_inline_data_get_aligned(&data, size, alignment);
    *gpu_address = uvm_push_inline_data_end(&data);

    gpu_address->address = UVM_ALIGN_UP(gpu_address->address, alignment);

    return buffer;
}

NvU64 *uvm_push_timestamp(uvm_push_t *push)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    const size_t timestamp_size = 16;
    NvU64 *timestamp;
    uvm_gpu_address_t address;

    timestamp = (NvU64 *)uvm_push_get_single_inline_buffer(push, timestamp_size, timestamp_size, &address);

    // Timestamp is in the second half of the 16 byte semaphore release
    timestamp += 1;

    if (uvm_channel_is_ce(push->channel))
        gpu->parent->ce_hal->semaphore_timestamp(push, address.address);
    else
        gpu->parent->sec2_hal->semaphore_timestamp(push, address.address);

    return timestamp;
}

bool uvm_push_method_is_valid(uvm_push_t *push, NvU8 subch, NvU32 method_address, NvU32 method_data)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);

    if (subch == UVM_SUBCHANNEL_CE)
        return gpu->parent->ce_hal->method_is_valid(push, method_address, method_data);
    else if (subch == UVM_SUBCHANNEL_HOST)
        return gpu->parent->host_hal->method_is_valid(push, method_address, method_data);
    else if (subch == UVM_SW_OBJ_SUBCHANNEL)
        return gpu->parent->host_hal->sw_method_is_valid(push, method_address, method_data);
    else if (subch == UVM_SUBCHANNEL_SEC2)
        return true;

    UVM_ERR_PRINT("Unsupported subchannel 0x%x\n", subch);
    return false;
}
