/*******************************************************************************
    Copyright (c) 2015-2019 NVIDIA Corporation

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

#include "uvm_tracker.h"
#include "uvm_push.h"
#include "uvm_channel.h"
#include "uvm_kvmalloc.h"
#include "uvm_gpu.h"
#include "uvm_global.h"
#include "uvm_common.h"
#include "uvm_linux.h"

static bool tracker_is_using_static_entries(uvm_tracker_t *tracker)
{
    return tracker->max_size == ARRAY_SIZE(tracker->static_entries);
}

static void free_entries(uvm_tracker_t *tracker)
{
    if (tracker_is_using_static_entries(tracker))
        return;
    uvm_kvfree(tracker->dynamic_entries);
}

uvm_tracker_entry_t *uvm_tracker_get_entries(uvm_tracker_t *tracker)
{
    if (tracker_is_using_static_entries(tracker)) {
        return tracker->static_entries;
    }
    else {
        UVM_ASSERT(tracker->dynamic_entries != NULL);
        return tracker->dynamic_entries;
    }
}

static uvm_tracker_entry_t *get_new_entry(uvm_tracker_t *tracker)
{
    NV_STATUS status = uvm_tracker_reserve(tracker, 1);
    if (status != NV_OK)
        return NULL;
    UVM_ASSERT(tracker->size < tracker->max_size);

    return &uvm_tracker_get_entries(tracker)[tracker->size++];
}

NV_STATUS uvm_tracker_init_from(uvm_tracker_t *dst, uvm_tracker_t *src)
{
    NV_STATUS status;
    uvm_tracker_init(dst);
    status = uvm_tracker_overwrite(dst, src);
    if (status != NV_OK) {
        uvm_tracker_deinit(dst);
        uvm_tracker_init(dst);
    }
    return status;
}

void uvm_tracker_deinit(uvm_tracker_t *tracker)
{
    free_entries(tracker);
    memset(tracker, 0, sizeof(*tracker));
}

NV_STATUS uvm_tracker_overwrite(uvm_tracker_t *dst, uvm_tracker_t *src)
{
    NV_STATUS status;

    uvm_tracker_clear(dst);

    status = uvm_tracker_reserve(dst, src->size);
    if (status != NV_OK)
        return status;

    dst->size = src->size;
    memcpy(uvm_tracker_get_entries(dst),
           uvm_tracker_get_entries(src),
           src->size * sizeof(*uvm_tracker_get_entries(dst)));

    return NV_OK;
}

NV_STATUS uvm_tracker_reserve(uvm_tracker_t *tracker, NvU32 min_free_entries)
{
    if (tracker->size + min_free_entries > tracker->max_size) {
        // Special case the first resize to jump from 1 all the way to 8.
        // This is based on a guess that if a tracker needs more than 1
        // entry it likely needs much more.
        // TODO: Bug 1764961: Verify that guess.
        NvU32 new_max_size = max((NvU32)8, (NvU32)roundup_pow_of_two(tracker->size + min_free_entries));
        uvm_tracker_entry_t *new_entries;

        if (tracker_is_using_static_entries(tracker)) {
            new_entries = uvm_kvmalloc(sizeof(*new_entries) * new_max_size);
            if (new_entries)
                memcpy(new_entries, tracker->static_entries, sizeof(*new_entries) * tracker->size);
        } else {
            new_entries = uvm_kvrealloc(tracker->dynamic_entries, sizeof(*new_entries) * new_max_size);
        }
        if (!new_entries)
            return NV_ERR_NO_MEMORY;
        tracker->dynamic_entries = new_entries;
        tracker->max_size = new_max_size;
    }
    UVM_ASSERT(tracker->size + min_free_entries <= tracker->max_size);
    return NV_OK;
}

NV_STATUS uvm_tracker_add_push(uvm_tracker_t *tracker, uvm_push_t *push)
{
    uvm_tracker_entry_t entry;

    uvm_push_get_tracker_entry(push, &entry);

    return uvm_tracker_add_entry(tracker, &entry);
}

NV_STATUS uvm_tracker_add_entry(uvm_tracker_t *tracker, uvm_tracker_entry_t *new_entry)
{
    uvm_tracker_entry_t *tracker_entry;

    for_each_tracker_entry(tracker_entry, tracker) {
        if (tracker_entry->channel == new_entry->channel) {
            tracker_entry->value = max(tracker_entry->value, new_entry->value);
            return NV_OK;
        }
    }

    tracker_entry = get_new_entry(tracker);
    if (tracker_entry == NULL)
        return NV_ERR_NO_MEMORY;

    *tracker_entry = *new_entry;

    return NV_OK;
}

void uvm_tracker_overwrite_with_entry(uvm_tracker_t *tracker, uvm_tracker_entry_t *entry)
{
    NV_STATUS status;

    uvm_tracker_clear(tracker);

    // An empty tracker always has space for at least one entry so this cannot
    // fail.
    status = uvm_tracker_add_entry(tracker, entry);
    UVM_ASSERT(status == NV_OK);
}

void uvm_tracker_overwrite_with_push(uvm_tracker_t *tracker, uvm_push_t *push)
{
    uvm_tracker_entry_t entry;

    uvm_push_get_tracker_entry(push, &entry);

    uvm_tracker_overwrite_with_entry(tracker, &entry);
}

static NV_STATUS reserve_for_entries_from_tracker(uvm_tracker_t *dst, uvm_tracker_t *src)
{
    NvU32 needed_free_entries = 0;
    uvm_tracker_entry_t *src_entry, *dst_entry;

    for_each_tracker_entry(src_entry, src) {
        bool found = false;
        for_each_tracker_entry(dst_entry, dst) {
            if (dst_entry->channel == src_entry->channel) {
                found = true;
                break;
            }
        }
        if (!found)
            needed_free_entries++;
    }

    return uvm_tracker_reserve(dst, needed_free_entries);
}

NV_STATUS uvm_tracker_add_tracker(uvm_tracker_t *dst, uvm_tracker_t *src)
{
    NV_STATUS status;
    uvm_tracker_entry_t *src_entry;

    UVM_ASSERT(dst != NULL);

    if (src == NULL)
        return NV_OK;

    if (src == dst)
        return NV_OK;

    if (uvm_tracker_is_empty(src))
        return NV_OK;

    status = uvm_tracker_reserve(dst, src->size);
    if (status == NV_ERR_NO_MEMORY) {
        uvm_tracker_remove_completed(dst);
        uvm_tracker_remove_completed(src);
        status = reserve_for_entries_from_tracker(dst, src);
    }

    if (status != NV_OK)
        return status;

    for_each_tracker_entry(src_entry, src) {
        status = uvm_tracker_add_entry(dst, src_entry);
        UVM_ASSERT_MSG(status == NV_OK, "Expected success with reserved memory but got error %d\n", status);
    }

    return NV_OK;
}

NV_STATUS uvm_tracker_overwrite_safe(uvm_tracker_t *dst, uvm_tracker_t *src)
{
    NV_STATUS status = uvm_tracker_overwrite(dst, src);
    if (status == NV_ERR_NO_MEMORY) {
        UVM_DBG_PRINT_RL("Failed to overwrite tracker, waiting\n");
        status = uvm_tracker_wait(src);
    }
    return status;
}

NV_STATUS uvm_tracker_add_push_safe(uvm_tracker_t *tracker, uvm_push_t *push)
{
    NV_STATUS status = uvm_tracker_add_push(tracker, push);
    if (status == NV_ERR_NO_MEMORY) {
        UVM_DBG_PRINT_RL("Failed to add push to tracker, waiting\n");
        status = uvm_push_wait(push);
    }
    return status;
}

NV_STATUS uvm_tracker_add_entry_safe(uvm_tracker_t *tracker, uvm_tracker_entry_t *new_entry)
{
    NV_STATUS status = uvm_tracker_add_entry(tracker, new_entry);
    if (status == NV_ERR_NO_MEMORY) {
        UVM_DBG_PRINT_RL("Failed to add entry to tracker, waiting\n");
        status = uvm_tracker_wait_for_entry(new_entry);
    }
    return status;
}

NV_STATUS uvm_tracker_add_tracker_safe(uvm_tracker_t *dst, uvm_tracker_t *src)
{
    NV_STATUS status = uvm_tracker_add_tracker(dst, src);
    if (status == NV_ERR_NO_MEMORY) {
        UVM_DBG_PRINT_RL("Failed to add tracker to tracker, waiting\n");
        status = uvm_tracker_wait(src);
    }
    return status;
}

bool uvm_tracker_is_entry_completed(uvm_tracker_entry_t *tracker_entry)
{
    if (!tracker_entry->channel)
        return true;

    return uvm_channel_is_value_completed(tracker_entry->channel, tracker_entry->value);
}

static void uvm_tracker_entry_print_pending_pushes(uvm_tracker_entry_t *entry)
{
    uvm_channel_t *channel = entry->channel;
    uvm_gpu_t *gpu = uvm_channel_get_gpu(channel);

    UVM_DBG_PRINT("Tracker entry for value %llu (sema VA 0x%llx) channel %s GPU %s\n",
                  entry->value,
                  uvm_channel_tracking_semaphore_get_gpu_va(channel),
                  channel->name,
                  uvm_gpu_name(gpu));

    uvm_channel_print_pending_pushes(channel);
}

static void uvm_tracker_print_pending_pushes(uvm_tracker_t *tracker)
{
    uvm_tracker_entry_t *entry;
    for_each_tracker_entry(entry, tracker)
        uvm_tracker_entry_print_pending_pushes(entry);
}

static NV_STATUS wait_for_entry_with_spin(uvm_tracker_entry_t *tracker_entry, uvm_spin_loop_t *spin)
{
    NV_STATUS status = NV_OK;

    while (!uvm_tracker_is_entry_completed(tracker_entry) && status == NV_OK) {
        if (UVM_SPIN_LOOP(spin) == NV_ERR_TIMEOUT_RETRY)
            uvm_tracker_entry_print_pending_pushes(tracker_entry);

        status = uvm_channel_check_errors(tracker_entry->channel);

        if (status == NV_OK)
            status = uvm_global_get_status();
    }

    if (status != NV_OK) {
        UVM_ASSERT(status == uvm_global_get_status());
        tracker_entry->channel = NULL;
        tracker_entry->value = 0;
    }

    return status;
}

NV_STATUS uvm_tracker_wait_for_entry(uvm_tracker_entry_t *tracker_entry)
{
    uvm_spin_loop_t spin;
    uvm_spin_loop_init(&spin);
    return wait_for_entry_with_spin(tracker_entry, &spin);
}

NV_STATUS uvm_tracker_wait(uvm_tracker_t *tracker)
{
    NV_STATUS status = NV_OK;
    uvm_spin_loop_t spin;

    uvm_spin_loop_init(&spin);
    while (!uvm_tracker_is_completed(tracker) && status == NV_OK) {
        if (UVM_SPIN_LOOP(&spin) == NV_ERR_TIMEOUT_RETRY)
            uvm_tracker_print_pending_pushes(tracker);

        status = uvm_tracker_check_errors(tracker);
    }

    if (status != NV_OK) {
        UVM_ASSERT(status == uvm_global_get_status());

        // Just clear the tracker without printing anything extra. If one of the
        // entries from this tracker caused a channel error,
        // uvm_tracker_check_errors() would have already printed it. And if we
        // hit a global error for some other reason, we don't want to spam the
        // log with all other pending entries.
        //
        // See the comment for uvm_tracker_wait() on why the entries are cleared.
        uvm_tracker_clear(tracker);
    }

    return status;
}

NV_STATUS uvm_tracker_wait_for_other_gpus(uvm_tracker_t *tracker, uvm_gpu_t *gpu)
{
    NV_STATUS status = NV_OK;
    uvm_tracker_entry_t *entry;
    uvm_spin_loop_t spin;

    uvm_spin_loop_init(&spin);

    for_each_tracker_entry(entry, tracker) {
        if (uvm_tracker_entry_gpu(entry) == gpu)
            continue;

        status = wait_for_entry_with_spin(entry, &spin);
        if (status != NV_OK)
            break;
    }

    if (status == NV_OK) {
        uvm_tracker_remove_completed(tracker);
    }
    else {
        UVM_ASSERT(status == uvm_global_get_status());
        uvm_tracker_clear(tracker);
    }

    return status;
}

NV_STATUS uvm_tracker_check_errors(uvm_tracker_t *tracker)
{
    uvm_tracker_entry_t *tracker_entry;
    NV_STATUS status = uvm_global_get_status();

    if (status != NV_OK)
        return status;

    for_each_tracker_entry(tracker_entry, tracker) {
        status = uvm_channel_check_errors(tracker_entry->channel);
        if (status != NV_OK)
            return status;
    }

    return NV_OK;
}

NV_STATUS uvm_tracker_query(uvm_tracker_t *tracker)
{
    NV_STATUS status;
    bool completed = uvm_tracker_is_completed(tracker);

    status = uvm_tracker_check_errors(tracker);
    if (status != NV_OK)
        return status;

    return completed ? NV_OK : NV_WARN_MORE_PROCESSING_REQUIRED;
}

void uvm_tracker_remove_completed(uvm_tracker_t *tracker)
{
    NvU32 i = 0;

    uvm_tracker_entry_t *entries = uvm_tracker_get_entries(tracker);

    // Keep removing completed entries until we run out of entries
    while (i < tracker->size) {
        if (uvm_tracker_is_entry_completed(&entries[i])) {
            --tracker->size;
            if (i != tracker->size)
                entries[i] = entries[tracker->size];
        }
        else {
            ++i;
        }
    }
}

bool uvm_tracker_is_completed(uvm_tracker_t *tracker)
{
    uvm_tracker_remove_completed(tracker);

    return tracker->size == 0;
}

uvm_gpu_t *uvm_tracker_entry_gpu(uvm_tracker_entry_t *entry)
{
    return uvm_channel_get_gpu(entry->channel);
}

