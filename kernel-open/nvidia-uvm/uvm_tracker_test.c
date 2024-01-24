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

#include "uvm_channel.h"
#include "uvm_global.h"
#include "uvm_hal.h"
#include "uvm_push.h"
#include "uvm_test.h"
#include "uvm_tracker.h"
#include "uvm_va_space.h"

static NV_STATUS assert_tracker_is_completed(uvm_tracker_t *tracker)
{
    TEST_NV_CHECK_RET(uvm_tracker_query(tracker));
    TEST_CHECK_RET(uvm_tracker_is_completed(tracker));
    TEST_NV_CHECK_RET(uvm_tracker_wait(tracker));
    TEST_NV_CHECK_RET(uvm_tracker_check_errors(tracker));
    TEST_CHECK_RET(tracker->size == 0);
    uvm_tracker_remove_completed(tracker);
    uvm_tracker_clear(tracker);

    return NV_OK;
}

static NV_STATUS assert_tracker_is_not_completed(uvm_tracker_t *tracker)
{
    uvm_tracker_remove_completed(tracker);
    TEST_CHECK_RET(uvm_tracker_query(tracker) == NV_WARN_MORE_PROCESSING_REQUIRED);
    TEST_CHECK_RET(!uvm_tracker_is_completed(tracker));
    TEST_NV_CHECK_RET(uvm_tracker_check_errors(tracker));
    TEST_CHECK_RET(tracker->size != 0);

    return NV_OK;
}

// This test schedules some GPU work behind a semaphore and then allows the GPU
// to progress one tracker entry at a time verifying that the tracker entries
// are completed as expected.
static NV_STATUS test_tracker_completion(uvm_va_space_t *va_space)
{
    uvm_gpu_t *gpu;
    uvm_tracker_t tracker;
    uvm_gpu_semaphore_t sema;
    NvU32 count = 0;
    NvU32 payload;
    NV_STATUS status = NV_OK;
    uvm_spin_loop_t spin;

    // TODO: Bug 4008734: [UVM][HCC] Extend secure tracking semaphore mechanism
    //                     to all semaphore
    // This test allocates semaphore in vidmem and then releases it from the CPU
    // SEC2 channels cannot target semaphores in vidmem. Moreover, CPU cannot
    // directly release values to vidmem for CE channels.
    if (g_uvm_global.conf_computing_enabled)
        return NV_OK;

    gpu = uvm_va_space_find_first_gpu(va_space);
    TEST_CHECK_RET(gpu != NULL);

    TEST_NV_CHECK_RET(uvm_gpu_semaphore_alloc(gpu->semaphore_pool, &sema));

    uvm_tracker_init(&tracker);
    TEST_NV_CHECK_GOTO(assert_tracker_is_completed(&tracker), done);

    // The following assumes that it's possible to begin a small push that won't
    // be able to finish (it's behind a semaphore that will be released from the
    // CPU later) for each channel on a each GPU.
    for_each_va_space_gpu(gpu, va_space) {
        uvm_channel_pool_t *pool;

        uvm_for_each_pool(pool, gpu->channel_manager) {
            uvm_channel_t *channel;

            // Skip WLC channels as they are used for secure work launch
            if (uvm_channel_pool_is_wlc(pool))
                continue;

            // Skip LCIC channels as those can't accept pushes
            if (uvm_channel_pool_is_lcic(pool))
                continue;
            uvm_for_each_channel_in_pool(channel, pool) {
                uvm_push_t push;
                NvU64 semaphore_gpu_va;

                ++count;
                TEST_NV_CHECK_GOTO(uvm_push_begin_on_channel(channel, &push, "Test push"), done);

                semaphore_gpu_va = uvm_gpu_semaphore_get_gpu_va(&sema, gpu, uvm_channel_is_proxy(channel));

                // Acquire increasing semaphore payloads on all channels so that they can be completed one by one
                gpu->parent->host_hal->semaphore_acquire(&push, semaphore_gpu_va, count);

                uvm_push_end(&push);

                if (count & 1)
                    TEST_NV_CHECK_GOTO(uvm_tracker_add_push_safe(&tracker, &push), done);
                else
                    TEST_NV_CHECK_GOTO(uvm_tracker_add_push(&tracker, &push), done);
            }
        }
    }

    TEST_NV_CHECK_GOTO(assert_tracker_is_not_completed(&tracker), done);

    for (payload = 0; payload < count; ++payload) {
        TEST_CHECK_GOTO(tracker.size == count - payload, done);
        TEST_NV_CHECK_GOTO(assert_tracker_is_not_completed(&tracker), done);

        // Release the next payload allowing a single channel to complete
        uvm_gpu_semaphore_set_payload(&sema, payload + 1);

        uvm_spin_loop_init(&spin);
        while (tracker.size == count - payload) {
            UVM_SPIN_LOOP(&spin);
            uvm_tracker_remove_completed(&tracker);
        }

        TEST_CHECK_GOTO(tracker.size == count - payload - 1, done);
    }

    TEST_NV_CHECK_GOTO(assert_tracker_is_completed(&tracker), done);

done:
    uvm_gpu_semaphore_free(&sema);
    uvm_tracker_wait_deinit(&tracker);
    return status;
}

static NV_STATUS test_tracker_basic(uvm_va_space_t *va_space)
{
    uvm_gpu_t *gpu;
    uvm_channel_t *channel;
    uvm_tracker_t tracker;
    uvm_tracker_entry_t entry;
    NvU32 count = 0;
    NV_STATUS status = NV_OK;

    gpu = uvm_va_space_find_first_gpu(va_space);
    if (gpu == NULL)
        return NV_ERR_INVALID_STATE;

    channel = uvm_channel_any(gpu->channel_manager);
    if (channel == NULL)
        return NV_ERR_INVALID_STATE;

    uvm_tracker_init(&tracker);
    TEST_CHECK_GOTO(assert_tracker_is_completed(&tracker) == NV_OK, done);

    // Some channel
    entry.channel = channel;
    entry.value = 1;

    status = uvm_tracker_add_entry(&tracker, &entry);
    TEST_CHECK_GOTO(status == NV_OK, done);
    TEST_CHECK_RET(tracker.size == 1);

    status = uvm_tracker_add_entry(&tracker, &entry);
    TEST_CHECK_GOTO(status == NV_OK, done);
    TEST_CHECK_RET(tracker.size == 1);
    TEST_CHECK_RET(uvm_tracker_get_entries(&tracker)[0].channel == entry.channel);
    TEST_CHECK_RET(uvm_tracker_get_entries(&tracker)[0].value == entry.value);

    entry.value = 10;
    status = uvm_tracker_add_entry(&tracker, &entry);
    TEST_CHECK_GOTO(status == NV_OK, done);
    TEST_CHECK_RET(tracker.size == 1);
    TEST_CHECK_RET(uvm_tracker_get_entries(&tracker)[0].channel == entry.channel);
    TEST_CHECK_RET(uvm_tracker_get_entries(&tracker)[0].value == entry.value);

    // Adding an older value for the same channel should have no effect
    entry.value = 5;
    status = uvm_tracker_add_entry(&tracker, &entry);
    TEST_CHECK_GOTO(status == NV_OK, done);
    TEST_CHECK_RET(tracker.size == 1);
    TEST_CHECK_RET(uvm_tracker_get_entries(&tracker)[0].value == 10);

    uvm_tracker_clear(&tracker);

    TEST_CHECK_GOTO(assert_tracker_is_completed(&tracker) == NV_OK, done);

    for_each_va_space_gpu(gpu, va_space) {
        uvm_channel_pool_t *pool;

        uvm_for_each_pool(pool, gpu->channel_manager) {
            uvm_channel_t *channel;

            uvm_for_each_channel_in_pool(channel, pool) {
                entry.channel = channel;
                entry.value = uvm_channel_update_completed_value(channel);
                if (count & 1)
                    status = uvm_tracker_add_entry_safe(&tracker, &entry);
                else
                    status = uvm_tracker_add_entry(&tracker, &entry);
                TEST_CHECK_GOTO(status == NV_OK, done);
                ++count;
            }
        }
    }

    TEST_CHECK_GOTO(tracker.size == count, done);

    // All the entries that we added are already completed
    TEST_CHECK_GOTO(assert_tracker_is_completed(&tracker) == NV_OK, done);

    count = 0;
    for_each_va_space_gpu(gpu, va_space) {
        uvm_channel_pool_t *pool;

        uvm_for_each_pool(pool, gpu->channel_manager) {
            uvm_channel_t *channel;

            // Skip LCIC channels as those can't accept pushes
            if (uvm_channel_pool_is_lcic(pool))
                continue;
            uvm_for_each_channel_in_pool(channel, pool) {
                uvm_push_t push;
                status = uvm_push_begin_on_channel(channel, &push, "Test push");
                TEST_CHECK_GOTO(status == NV_OK, done);

                uvm_push_end(&push);

                TEST_NV_CHECK_GOTO(uvm_tracker_add_push(&tracker, &push), done);
                ++count;
            }
        }
    }

    TEST_CHECK_GOTO(tracker.size == count, done);
    TEST_CHECK_GOTO(uvm_tracker_wait(&tracker) == NV_OK, done);
    // After a wait, the tracker should be complete
    TEST_CHECK_GOTO(assert_tracker_is_completed(&tracker) == NV_OK, done);

done:
    uvm_tracker_deinit(&tracker);
    return status;
}

static NV_STATUS test_tracker_overwrite(uvm_va_space_t *va_space)
{
    uvm_gpu_t *gpu;
    uvm_channel_t *channel;
    uvm_tracker_t tracker, dup_tracker;
    uvm_tracker_entry_t entry;
    uvm_tracker_entry_t *entry_iter, *dup_entry_iter;
    NV_STATUS status = NV_OK;
    bool dup_tracker_init = false;
    NvU32 count = 0;

    gpu = uvm_va_space_find_first_gpu(va_space);
    if (gpu == NULL)
        return NV_ERR_INVALID_STATE;

    channel = uvm_channel_any(gpu->channel_manager);
    if (channel == NULL)
        return NV_ERR_INVALID_STATE;

    uvm_tracker_init(&tracker);
    TEST_CHECK_GOTO(assert_tracker_is_completed(&tracker) == NV_OK, done);

    // Some channel
    entry.channel = channel;
    entry.value = 1;

    status = uvm_tracker_add_entry(&tracker, &entry);
    TEST_CHECK_GOTO(status == NV_OK, done);
    TEST_CHECK_RET(tracker.size == 1);
    TEST_CHECK_RET(uvm_tracker_get_entries(&tracker)[0].channel == entry.channel);
    TEST_CHECK_RET(uvm_tracker_get_entries(&tracker)[0].value == entry.value);

    status = uvm_tracker_init_from(&dup_tracker, &tracker);
    TEST_CHECK_GOTO(status == NV_OK, done);
    dup_tracker_init = true;
    TEST_CHECK_RET(dup_tracker.size == 1);
    TEST_CHECK_RET(uvm_tracker_get_entries(&dup_tracker)[0].channel == entry.channel);
    TEST_CHECK_RET(uvm_tracker_get_entries(&dup_tracker)[0].value == entry.value);

    entry.value = 2;

    uvm_tracker_overwrite_with_entry(&dup_tracker, &entry);
    TEST_CHECK_RET(dup_tracker.size == 1);
    TEST_CHECK_RET(uvm_tracker_get_entries(&dup_tracker)[0].channel == entry.channel);
    TEST_CHECK_RET(uvm_tracker_get_entries(&dup_tracker)[0].value == entry.value);

    for_each_va_space_gpu(gpu, va_space) {
        uvm_channel_pool_t *pool;

        uvm_for_each_pool(pool, gpu->channel_manager) {
            uvm_channel_t *channel;

            uvm_for_each_channel_in_pool(channel, pool) {
                entry.channel = channel;
                entry.value = uvm_channel_update_completed_value(channel);
                status = uvm_tracker_add_entry(&tracker, &entry);
                TEST_CHECK_GOTO(status == NV_OK, done);
                ++count;
            }
        }
    }
    TEST_CHECK_GOTO(tracker.size == count, done);

    status = uvm_tracker_overwrite(&dup_tracker, &tracker);
    TEST_CHECK_GOTO(dup_tracker.size == count, done);
    for_each_tracker_entry(dup_entry_iter, &dup_tracker) {
        bool found = false;
        for_each_tracker_entry(entry_iter, &tracker) {
            if (entry_iter->channel == dup_entry_iter->channel && entry_iter->value == dup_entry_iter->value) {
                found = true;
                break;
            }
        }
        TEST_CHECK_RET(found);
    }
    for_each_tracker_entry(entry_iter, &tracker) {
        bool found = false;
        for_each_tracker_entry(dup_entry_iter, &dup_tracker) {
            if (entry_iter->channel == dup_entry_iter->channel && entry_iter->value == dup_entry_iter->value) {
                found = true;
                break;
            }
        }
        TEST_CHECK_RET(found);
    }

done:
    uvm_tracker_deinit(&tracker);
    if (dup_tracker_init)
        uvm_tracker_deinit(&dup_tracker);
    return status;
}

static NV_STATUS test_tracker_add_tracker(uvm_va_space_t *va_space)
{
    uvm_gpu_t *gpu;
    uvm_channel_t *channel;
    uvm_tracker_t tracker, dup_tracker;
    uvm_tracker_entry_t entry;
    uvm_tracker_entry_t *entry_iter, *dup_entry_iter;
    NV_STATUS status = NV_OK;
    NvU32 count = 0;

    gpu = uvm_va_space_find_first_gpu(va_space);
    if (gpu == NULL)
        return NV_ERR_INVALID_STATE;

    channel = uvm_channel_any(gpu->channel_manager);
    if (channel == NULL)
        return NV_ERR_INVALID_STATE;

    uvm_tracker_init(&tracker);
    uvm_tracker_init(&dup_tracker);
    TEST_CHECK_GOTO(assert_tracker_is_completed(&tracker) == NV_OK, done);

    // Some channel
    entry.channel = channel;
    entry.value = 1;

    status = uvm_tracker_add_entry(&tracker, &entry);
    TEST_CHECK_GOTO(status == NV_OK, done);
    TEST_CHECK_RET(tracker.size == 1);
    TEST_CHECK_RET(uvm_tracker_get_entries(&tracker)[0].channel == entry.channel);
    TEST_CHECK_RET(uvm_tracker_get_entries(&tracker)[0].value == entry.value);

    status = uvm_tracker_add_tracker(&dup_tracker, &tracker);
    TEST_CHECK_GOTO(status == NV_OK, done);
    TEST_CHECK_RET(dup_tracker.size == 1);
    TEST_CHECK_RET(uvm_tracker_get_entries(&dup_tracker)[0].channel == entry.channel);
    TEST_CHECK_RET(uvm_tracker_get_entries(&dup_tracker)[0].value == entry.value);

    for_each_va_space_gpu(gpu, va_space) {
        uvm_channel_pool_t *pool;

        uvm_for_each_pool(pool, gpu->channel_manager) {
            uvm_channel_t *channel;

            uvm_for_each_channel_in_pool(channel, pool) {
                entry.channel = channel;
                entry.value = uvm_channel_update_completed_value(channel);
                status = uvm_tracker_add_entry(&tracker, &entry);
                TEST_CHECK_GOTO(status == NV_OK, done);
                ++count;
            }
        }
    }
    TEST_CHECK_GOTO(tracker.size == count, done);

    status = uvm_tracker_add_tracker_safe(&dup_tracker, &tracker);
    TEST_CHECK_GOTO(dup_tracker.size == count, done);
    for_each_tracker_entry(dup_entry_iter, &dup_tracker) {
        bool found = false;
        for_each_tracker_entry(entry_iter, &tracker) {
            if (entry_iter->channel == dup_entry_iter->channel && entry_iter->value == dup_entry_iter->value) {
                found = true;
                break;
            }
        }
        TEST_CHECK_RET(found);
    }
    for_each_tracker_entry(entry_iter, &tracker) {
        bool found = false;
        for_each_tracker_entry(dup_entry_iter, &dup_tracker) {
            if (entry_iter->channel == dup_entry_iter->channel && entry_iter->value == dup_entry_iter->value) {
                found = true;
                break;
            }
        }
        TEST_CHECK_RET(found);
    }

done:
    uvm_tracker_deinit(&tracker);
    uvm_tracker_deinit(&dup_tracker);
    return status;
}

NV_STATUS uvm_test_tracker_sanity(UVM_TEST_TRACKER_SANITY_PARAMS *params, struct file *filp)
{
    NV_STATUS status;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    uvm_va_space_down_read_rm(va_space);

    status = test_tracker_basic(va_space);
    if (status != NV_OK)
        goto done;

    status = test_tracker_completion(va_space);
    if (status != NV_OK)
        goto done;

    status = test_tracker_overwrite(va_space);
    if (status != NV_OK)
        goto done;

    status = test_tracker_add_tracker(va_space);
    if (status != NV_OK)
        goto done;

done:
    uvm_va_space_up_read_rm(va_space);

    return status;
}
