/*******************************************************************************
    Copyright (c) 2015-2021 NVIDIA Corporation

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

#include "uvm_global.h"
#include "uvm_channel.h"
#include "uvm_hal.h"
#include "uvm_push.h"
#include "uvm_test.h"
#include "uvm_test_rng.h"
#include "uvm_va_space.h"
#include "uvm_tracker.h"
#include "uvm_thread_context.h"
#include "uvm_gpu_semaphore.h"
#include "uvm_kvmalloc.h"

#define TEST_ORDERING_ITERS_PER_CHANNEL_TYPE_PER_GPU     1024
#define TEST_ORDERING_ITERS_PER_CHANNEL_TYPE_PER_GPU_EMU 64

// Schedule pushes one after another on all GPUs and channel types that copy and
// increment a counter into an adjacent memory location in a buffer. And then
// verify that all the values are correct on the CPU.
static NV_STATUS test_ordering(uvm_va_space_t *va_space)
{
    NV_STATUS status;
    uvm_gpu_t *gpu;
    bool exclude_proxy_channel_type;
    NvU32 i, j;
    uvm_rm_mem_t *mem = NULL;
    NvU32 *host_mem;
    uvm_push_t push;
    NvU64 gpu_va;
    uvm_tracker_t tracker = UVM_TRACKER_INIT();
    NvU32 value = 0;
    const NvU32 iters_per_channel_type_per_gpu = g_uvm_global.num_simulated_devices > 0 ?
                                                     TEST_ORDERING_ITERS_PER_CHANNEL_TYPE_PER_GPU_EMU :
                                                     TEST_ORDERING_ITERS_PER_CHANNEL_TYPE_PER_GPU;
    const NvU32 values_count = iters_per_channel_type_per_gpu;
    const size_t buffer_size = sizeof(NvU32) * values_count;

    gpu = uvm_va_space_find_first_gpu(va_space);
    TEST_CHECK_RET(gpu != NULL);

    status = uvm_rm_mem_alloc_and_map_all(gpu, UVM_RM_MEM_TYPE_SYS, buffer_size, &mem);
    TEST_CHECK_GOTO(status == NV_OK, done);

    host_mem = (NvU32*)uvm_rm_mem_get_cpu_va(mem);
    memset(host_mem, 0, buffer_size);

    status = uvm_push_begin(gpu->channel_manager, UVM_CHANNEL_TYPE_GPU_TO_CPU, &push, "Initial memset");
    TEST_CHECK_GOTO(status == NV_OK, done);

    gpu_va = uvm_rm_mem_get_gpu_va(mem, gpu, uvm_channel_is_proxy(push.channel));

    // Semaphore release as part of uvm_push_end() will do the membar
    uvm_push_set_flag(&push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);
    gpu->parent->ce_hal->memset_v_4(&push, gpu_va, 0, buffer_size);

    uvm_push_end(&push);

    TEST_NV_CHECK_GOTO(uvm_tracker_add_push(&tracker, &push), done);

    exclude_proxy_channel_type = uvm_gpu_uses_proxy_channel_pool(gpu);

    for (i = 0; i < iters_per_channel_type_per_gpu; ++i) {
        for (j = 0; j < UVM_CHANNEL_TYPE_CE_COUNT; ++j) {
            uvm_channel_type_t channel_type = j;

            // Proxy channels don't support the virtual memcopies that are about
            // to be pushed, so don't test the proxy channel type in any of the
            // GPUs.
            if (exclude_proxy_channel_type && (channel_type == uvm_channel_proxy_channel_type()))
                continue;

            for_each_va_space_gpu(gpu, va_space) {
                NvU64 gpu_va_base;
                NvU64 gpu_va_src;
                NvU64 gpu_va_dst;

                status = uvm_push_begin_acquire(gpu->channel_manager,
                                                channel_type,
                                                &tracker,
                                                &push,
                                                "memcpy and inc to %u",
                                                value + 1);
                TEST_CHECK_GOTO(status == NV_OK, done);

                gpu_va_base = uvm_rm_mem_get_gpu_va(mem, gpu, uvm_channel_is_proxy(push.channel));
                gpu_va_src = gpu_va_base + (value % values_count) * sizeof(NvU32);
                gpu_va_dst = gpu_va_base + ((value + 1) % values_count) * sizeof(NvU32);

                // The semaphore reduction will do a membar before the reduction
                uvm_push_set_flag(&push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);
                gpu->parent->ce_hal->memcopy_v_to_v(&push, gpu_va_dst, gpu_va_src, sizeof(NvU32));

                // The following reduction is done from the same GPU, but the
                // previous memcpy is to uncached sysmem and that bypasses L2
                // and hence requires a SYSMEMBAR to be ordered.
                gpu->parent->ce_hal->semaphore_reduction_inc(&push, gpu_va_dst, ++value);

                uvm_push_end(&push);

                uvm_tracker_clear(&tracker);
                TEST_NV_CHECK_GOTO(uvm_tracker_add_push(&tracker, &push), done);
            }
        }
    }
    status = uvm_tracker_wait(&tracker);
    TEST_CHECK_GOTO(status == NV_OK, done);

    // At this moment, this should hold:
    // mem[value % values_count] == value
    // mem[(value + 1) % values_count]  == value + 1 - values_count
    // And in general, for i=[0, values_count):
    // mem[(value + 1 + i) % values_count]  == value + 1 - values_count + i
    // Verify that

    for (i = 0; i < values_count; ++i) {
        NvU32 index = (value + 1 + i) % values_count;
        NvU32 expected = (value + 1 + i) - values_count;
        if (host_mem[index] != expected) {
            UVM_TEST_PRINT("Bad value at host_mem[%u] = %u instead of %u\n", index, host_mem[index], expected);
            status = NV_ERR_INVALID_STATE;
            goto done;
        }
    }

done:
    uvm_tracker_wait(&tracker);
    uvm_rm_mem_free(mem);

    return status;
}

static NV_STATUS test_unexpected_completed_values(uvm_va_space_t *va_space)
{
    NV_STATUS status;
    uvm_gpu_t *gpu;

    for_each_va_space_gpu(gpu, va_space) {
        uvm_channel_t *channel;
        NvU64 completed_value;

        // The GPU channel manager is destroyed and then re-created after
        // the test, so this test requires exclusive access to the GPU.
        TEST_CHECK_RET(uvm_gpu_retained_count(gpu) == 1);

        channel = &gpu->channel_manager->channel_pools[0].channels[0];
        completed_value = uvm_channel_update_completed_value(channel);
        uvm_gpu_semaphore_set_payload(&channel->tracking_sem.semaphore, (NvU32)completed_value + 1);

        TEST_CHECK_RET(uvm_global_get_status() == NV_OK);
        uvm_channel_update_progress_all(channel);
        TEST_CHECK_RET(uvm_global_reset_fatal_error() == NV_ERR_INVALID_STATE);

        uvm_channel_manager_destroy(gpu->channel_manager);
        // Destruction will hit the error again, so clear one more time.
        uvm_global_reset_fatal_error();

        TEST_NV_CHECK_RET(uvm_channel_manager_create(gpu, &gpu->channel_manager));
    }

    return NV_OK;
}

static NV_STATUS uvm_test_rc_for_gpu(uvm_gpu_t *gpu)
{
    uvm_push_t push;
    uvm_channel_pool_t *pool;
    uvm_gpfifo_entry_t *fatal_entry;
    uvm_push_info_t *push_info;
    int fatal_line;
    uvm_tracker_entry_t tracker_entry;
    NV_STATUS status;
    uvm_tracker_t tracker = UVM_TRACKER_INIT();
    uvm_channel_manager_t *manager = gpu->channel_manager;

    // Submit a bunch of successful pushes on each channel first so that the
    // fatal one is behind a bunch of work (notably more than
    // uvm_channel_update_progress() completes by default).
    uvm_for_each_pool(pool, manager) {
        uvm_channel_t *channel;

        uvm_for_each_channel_in_pool(channel, pool) {
            NvU32 i;
            for (i = 0; i < 512; ++i) {
                status = uvm_push_begin_on_channel(channel, &push, "Non-faulting push");
                TEST_CHECK_RET(status == NV_OK);

                uvm_push_end(&push);
            }
        }
    }

    // Check RC on a proxy channel (SR-IOV heavy) or internal channel (any other
    // mode). It is not allowed to use a virtual address in a memset pushed to
    // a proxy channel, so we use a physical address instead.
    if (uvm_gpu_uses_proxy_channel_pool(gpu)) {
        uvm_gpu_address_t dst_address;

        // Save the line number the push that's supposed to fail was started on
        fatal_line = __LINE__ + 1;
        TEST_NV_CHECK_RET(uvm_push_begin(manager, uvm_channel_proxy_channel_type(), &push, "Fatal push 0x%X", 0xBAD));

        // Memset targeting a physical address beyond the vidmem size. The
        // passed physical address is not the vidmem size reported by RM
        // because the reported size can be smaller than the actual physical
        // size, such that accessing a GPA at the reported size may be allowed
        // by VMMU.
        //
        // GA100 GPUs have way less than UVM_GPU_MAX_PHYS_MEM vidmem, so using
        // that value as physical address should result on an error
        dst_address = uvm_gpu_address_physical(UVM_APERTURE_VID, UVM_GPU_MAX_PHYS_MEM - 8);
        gpu->parent->ce_hal->memset_8(&push, dst_address, 0, 8);
    }
    else {
        fatal_line = __LINE__ + 1;
        TEST_NV_CHECK_RET(uvm_push_begin(manager, UVM_CHANNEL_TYPE_GPU_TO_CPU, &push, "Fatal push 0x%X", 0xBAD));

        // Memset that should fault on 0xFFFFFFFF
        gpu->parent->ce_hal->memset_v_4(&push, 0xFFFFFFFF, 0, 4);
    }

    uvm_push_end(&push);

    uvm_push_get_tracker_entry(&push, &tracker_entry);
    uvm_tracker_overwrite_with_push(&tracker, &push);

    status = uvm_channel_manager_wait(manager);
    TEST_CHECK_RET(status == NV_ERR_RC_ERROR);

    TEST_CHECK_RET(uvm_channel_get_status(push.channel) == NV_ERR_RC_ERROR);
    fatal_entry = uvm_channel_get_fatal_entry(push.channel);
    TEST_CHECK_RET(fatal_entry != NULL);

    push_info = fatal_entry->push_info;
    TEST_CHECK_RET(push_info != NULL);
    TEST_CHECK_RET(push_info->line == fatal_line);
    TEST_CHECK_RET(strcmp(push_info->function, __FUNCTION__) == 0);
    TEST_CHECK_RET(strcmp(push_info->filename, kbasename(__FILE__)) == 0);
    if (uvm_push_info_is_tracking_descriptions())
        TEST_CHECK_RET(strcmp(push_info->description, "Fatal push 0xBAD") == 0);

    TEST_CHECK_RET(uvm_global_get_status() == NV_ERR_RC_ERROR);

    // Check that waiting for an entry after a global fatal error makes the
    // entry completed.
    TEST_CHECK_RET(!uvm_tracker_is_entry_completed(&tracker_entry));
    TEST_CHECK_RET(uvm_tracker_wait_for_entry(&tracker_entry) == NV_ERR_RC_ERROR);
    TEST_CHECK_RET(uvm_tracker_is_entry_completed(&tracker_entry));

    // Check that waiting for a tracker after a global fatal error, clears all
    // the entries from the tracker.
    TEST_CHECK_RET(!uvm_tracker_is_empty(&tracker));
    TEST_CHECK_RET(uvm_tracker_wait(&tracker) == NV_ERR_RC_ERROR);
    TEST_CHECK_RET(uvm_tracker_is_empty(&tracker));

    TEST_CHECK_RET(uvm_global_reset_fatal_error() == NV_ERR_RC_ERROR);

    return NV_OK;
}

static NV_STATUS test_rc(uvm_va_space_t *va_space)
{
    uvm_gpu_t *gpu;

    uvm_assert_mutex_locked(&g_uvm_global.global_lock);

    for_each_va_space_gpu(gpu, va_space) {
        NV_STATUS test_status, create_status;

        // The GPU channel manager is destroyed and then re-created after
        // testing RC, so this test requires exclusive access to the GPU.
        TEST_CHECK_RET(uvm_gpu_retained_count(gpu) == 1);

        g_uvm_global.disable_fatal_error_assert = true;
        test_status = uvm_test_rc_for_gpu(gpu);
        g_uvm_global.disable_fatal_error_assert = false;

        uvm_channel_manager_destroy(gpu->channel_manager);
        create_status = uvm_channel_manager_create(gpu, &gpu->channel_manager);

        TEST_NV_CHECK_RET(test_status);
        TEST_NV_CHECK_RET(create_status);
    }

    return NV_OK;
}


typedef struct
{
    uvm_push_t push;
    uvm_tracker_t tracker;
    uvm_gpu_semaphore_t semaphore;
    NvU32 queued_counter_value;
    NvU32 queued_counter_repeat;
    uvm_rm_mem_t *counter_mem;
    uvm_rm_mem_t *counter_snapshots_mem;
    uvm_rm_mem_t *other_stream_counter_snapshots_mem;
    NvU32 *counter_snapshots;
    NvU32 *other_stream_counter_snapshots;
    NvU32 *other_stream_counter_expected;
} uvm_test_stream_t;

#define MAX_COUNTER_REPEAT_COUNT 10 * 1024
// For each iter, snapshot the first and last counter value
#define TEST_SNAPSHOT_SIZE(it) (2 * it * sizeof(NvU32))

static void snapshot_counter(uvm_push_t *push,
                             uvm_rm_mem_t *counter_mem,
                             uvm_rm_mem_t *snapshot_mem,
                             NvU32 index,
                             NvU32 counters_count)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    NvU64 counter_gpu_va;
    NvU64 snapshot_gpu_va;
    bool is_proxy_channel;
    NvU32 last_counter_offset = (counters_count - 1) * sizeof(NvU32);

    if (counters_count == 0)
        return;

    is_proxy_channel = uvm_channel_is_proxy(push->channel);
    counter_gpu_va = uvm_rm_mem_get_gpu_va(counter_mem, gpu, is_proxy_channel);
    snapshot_gpu_va = uvm_rm_mem_get_gpu_va(snapshot_mem, gpu, is_proxy_channel) + index * 2 * sizeof(NvU32);

    // Copy the last and first counter to a snapshot for later verification.

    // Membar will be done by uvm_push_end()
    uvm_push_set_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);
    uvm_push_set_flag(push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED);
    gpu->parent->ce_hal->memcopy_v_to_v(push,
                                        snapshot_gpu_va + sizeof(NvU32),
                                        counter_gpu_va + last_counter_offset,
                                        sizeof(NvU32));

    // Membar will be done by uvm_push_end()
    uvm_push_set_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);
    uvm_push_set_flag(push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED);
    gpu->parent->ce_hal->memcopy_v_to_v(push, snapshot_gpu_va, counter_gpu_va, sizeof(NvU32));
}

static void set_counter(uvm_push_t *push, uvm_rm_mem_t *counter_mem, NvU32 value, NvU32 count)
{
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    NvU64 counter_gpu_va;
    bool is_proxy_channel;

    is_proxy_channel = uvm_channel_is_proxy(push->channel);
    counter_gpu_va = uvm_rm_mem_get_gpu_va(counter_mem, gpu, is_proxy_channel);

    gpu->parent->ce_hal->memset_v_4(push, counter_gpu_va, value, count * sizeof(NvU32));
}

static uvm_channel_type_t random_ce_channel_type(uvm_test_rng_t *rng)
{
    return (uvm_channel_type_t)uvm_test_rng_range_32(rng, 0, UVM_CHANNEL_TYPE_CE_COUNT - 1);
}

static uvm_channel_type_t random_ce_channel_type_except(uvm_test_rng_t *rng, uvm_channel_type_t exception)
{
    uvm_channel_type_t channel_type;

    UVM_ASSERT(exception < UVM_CHANNEL_TYPE_CE_COUNT);

    channel_type = (uvm_channel_type_t)uvm_test_rng_range_32(rng, 0, UVM_CHANNEL_TYPE_CE_COUNT - 2);

    if (channel_type >= exception)
        channel_type++;

    UVM_ASSERT(channel_type < UVM_CHANNEL_TYPE_CE_COUNT);

    return channel_type;
}

static uvm_channel_type_t gpu_random_internal_ce_channel_type(uvm_gpu_t *gpu, uvm_test_rng_t *rng)
{
    if (uvm_gpu_uses_proxy_channel_pool(gpu))
        return random_ce_channel_type_except(rng, uvm_channel_proxy_channel_type());

    return random_ce_channel_type(rng);
}

static uvm_gpu_t *random_va_space_gpu(uvm_test_rng_t *rng, uvm_va_space_t *va_space)
{
    uvm_gpu_t *gpu;
    NvU32 gpu_count = uvm_processor_mask_get_gpu_count(&va_space->registered_gpus);
    NvU32 gpu_index = uvm_test_rng_range_32(rng, 0, gpu_count - 1);

    UVM_ASSERT(gpu_count > 0);

    for_each_va_space_gpu(gpu, va_space) {
        if (gpu_index-- == 0)
            return gpu;
    }

    UVM_ASSERT(0);
    return NULL;
}


static void test_memset_rm_mem(uvm_push_t *push, uvm_rm_mem_t *rm_mem, NvU32 value)
{
    uvm_gpu_t *gpu;
    NvU64 gpu_va;

    UVM_ASSERT(rm_mem->size % 4 == 0);

    gpu = uvm_push_get_gpu(push);
    gpu_va = uvm_rm_mem_get_gpu_va(rm_mem, gpu, uvm_channel_is_proxy(push->channel));

    gpu->parent->ce_hal->memset_v_4(push, gpu_va, value, rm_mem->size);
}

// This test schedules a randomly sized memset on a random channel and GPU in a
// "stream" that has operations ordered by acquiring the tracker of the previous
// operation. It also snapshots the memset done by the previous operation in the
// stream to verify it later on the CPU. Each iteration also optionally acquires
// a different stream and snapshots its memset.
// The test ioctl is expected to be called at the same time from multiple
// threads and contains some schedule() calls to help get as many threads
// through the init phase before other threads continue. It also has a random
// schedule() call in the main loop scheduling GPU work.
static NV_STATUS stress_test_all_gpus_in_va(uvm_va_space_t *va_space,
                                            NvU32 num_streams,
                                            NvU32 iterations_per_stream,
                                            NvU32 seed,
                                            NvU32 verbose)
{
    NV_STATUS status = NV_OK;
    uvm_gpu_t *gpu;
    NvU32 i, j;
    uvm_test_stream_t *streams;
    uvm_test_rng_t rng;

    uvm_test_rng_init(&rng, seed);

    gpu = uvm_va_space_find_first_gpu(va_space);
    TEST_CHECK_RET(gpu != NULL);

    streams = uvm_kvmalloc_zero(sizeof(*streams) * num_streams);
    TEST_CHECK_RET(streams != NULL);

    // Initialize all the trackers first so that clean up on error can always
    // wait for them.
    for (i = 0; i < num_streams; ++i) {
        uvm_test_stream_t *stream = &streams[i];
        uvm_tracker_init(&stream->tracker);
    }

    for (i = 0; i < num_streams; ++i) {
        uvm_test_stream_t *stream = &streams[i];

        status = uvm_gpu_semaphore_alloc(gpu->semaphore_pool, &stream->semaphore);
        if (status != NV_OK)
            goto done;

        stream->queued_counter_value = 0;

        status = uvm_rm_mem_alloc_and_map_all(gpu,
                                              UVM_RM_MEM_TYPE_SYS,
                                              MAX_COUNTER_REPEAT_COUNT * sizeof(NvU32),
                                              &stream->counter_mem);
        TEST_CHECK_GOTO(status == NV_OK, done);

        status = uvm_rm_mem_alloc_and_map_all(gpu,
                                              UVM_RM_MEM_TYPE_SYS,
                                              TEST_SNAPSHOT_SIZE(iterations_per_stream),
                                              &stream->counter_snapshots_mem);
        TEST_CHECK_GOTO(status == NV_OK, done);

        stream->counter_snapshots = (NvU32*)uvm_rm_mem_get_cpu_va(stream->counter_snapshots_mem);

        status = uvm_rm_mem_alloc_and_map_all(gpu,
                                              UVM_RM_MEM_TYPE_SYS,
                                              TEST_SNAPSHOT_SIZE(iterations_per_stream),
                                              &stream->other_stream_counter_snapshots_mem);
        TEST_CHECK_GOTO(status == NV_OK, done);

        stream->other_stream_counter_snapshots = (NvU32*)uvm_rm_mem_get_cpu_va(stream->other_stream_counter_snapshots_mem);

        stream->other_stream_counter_expected = uvm_kvmalloc_zero(sizeof(NvU32) * iterations_per_stream);
        if (stream->other_stream_counter_expected == NULL) {
            status = NV_ERR_NO_MEMORY;
            goto done;
        }

        status = uvm_push_begin(gpu->channel_manager, UVM_CHANNEL_TYPE_CPU_TO_GPU, &stream->push, "stream %u init", i);
        TEST_CHECK_GOTO(status == NV_OK, done);

        test_memset_rm_mem(&stream->push, stream->counter_mem, 0);
        test_memset_rm_mem(&stream->push, stream->counter_snapshots_mem, 0);
        test_memset_rm_mem(&stream->push, stream->other_stream_counter_snapshots_mem, 0);

        status = uvm_push_end_and_wait(&stream->push);
        TEST_CHECK_GOTO(status == NV_OK, done);

        if (fatal_signal_pending(current)) {
            status = NV_ERR_SIGNAL_PENDING;
            goto done;
        }

        // Let other threads run
        schedule();
    }

    if (verbose > 0) {
        UVM_TEST_PRINT("Init done, seed %u, GPUs:\n", seed);
        for_each_va_space_gpu(gpu, va_space) {
            UVM_TEST_PRINT(" GPU %s\n", uvm_gpu_name(gpu));
        }
    }

    for (i = 0; i < iterations_per_stream; ++i) {
        for (j = 0; j < num_streams; ++j) {
            uvm_test_stream_t *stream = &streams[j];
            uvm_channel_type_t channel_type;
            gpu = random_va_space_gpu(&rng, va_space);

            if (fatal_signal_pending(current)) {
                status = NV_ERR_SIGNAL_PENDING;
                goto done;
            }

            // Select a random channel type. In SR-IOV heavy the selection has
            // to exclude the type associated with proxy channels, because they
            // do not support the virtual memcopies/memsets pushed by
            // snapshot_counter and set_counter
            channel_type = gpu_random_internal_ce_channel_type(gpu, &rng);

            status = uvm_push_begin_acquire(gpu->channel_manager,
                                            channel_type,
                                            &stream->tracker,
                                            &stream->push,
                                            "stream %u payload %u gid %u channel_type %u",
                                            j,
                                            stream->queued_counter_value,
                                            uvm_id_value(gpu->id),
                                            channel_type);
            TEST_CHECK_GOTO(status == NV_OK, done);

            snapshot_counter(&stream->push,
                             stream->counter_mem,
                             stream->counter_snapshots_mem,
                             i,
                             stream->queued_counter_repeat);
            // Set a random number [2, MAX_COUNTER_REPEAT_COUNT] of counters
            stream->queued_counter_repeat = uvm_test_rng_range_32(&rng, 2, MAX_COUNTER_REPEAT_COUNT);
            set_counter(&stream->push,
                        stream->counter_mem,
                        ++stream->queued_counter_value,
                        stream->queued_counter_repeat);

            if (uvm_test_rng_range_32(&rng, 0, 1) == 0) {
                NvU32 random_stream_index = uvm_test_rng_range_32(&rng, 0, num_streams - 1);
                uvm_test_stream_t *random_stream = &streams[random_stream_index];
                uvm_push_acquire_tracker(&stream->push, &random_stream->tracker);
                snapshot_counter(&stream->push,
                                 random_stream->counter_mem,
                                 stream->other_stream_counter_snapshots_mem,
                                 i,
                                 random_stream->queued_counter_repeat);
            }

            uvm_push_end(&stream->push);
            uvm_tracker_clear(&stream->tracker);
            TEST_NV_CHECK_GOTO(uvm_tracker_add_push(&stream->tracker, &stream->push), done);
        }

        // Randomly schedule other threads
        if (uvm_test_rng_range_32(&rng, 0, 9) == 0)
            schedule();
    }

    if (verbose > 0)
        UVM_TEST_PRINT("All work scheduled\n");

    // Let other threads run
    schedule();

    for (i = 0; i < num_streams; ++i) {
        uvm_test_stream_t *stream = &streams[i];
        status = uvm_tracker_wait(&stream->tracker);
        if (status != NV_OK) {
            UVM_TEST_PRINT("Failed to wait for the tracker for stream %u: %s\n", i, nvstatusToString(status));
            goto done;
        }
        for (j = 0; j < iterations_per_stream; ++j) {
            NvU32 snapshot_last = stream->counter_snapshots[j * 2];
            NvU32 snapshot_first = stream->counter_snapshots[j * 2 + 1];
            if (snapshot_last != j || snapshot_first != j) {
                UVM_TEST_PRINT("Stream %u counter snapshot[%u] = %u,%u instead of %u,%u\n",
                               i,
                               j,
                               snapshot_last,
                               snapshot_first,
                               j,
                               j);
                status = NV_ERR_INVALID_STATE;
                goto done;
            }
        }
        for (j = 0; j < iterations_per_stream; ++j) {
            NvU32 snapshot_last = stream->other_stream_counter_snapshots[j * 2];
            NvU32 snapshot_first = stream->other_stream_counter_snapshots[j * 2 + 1];
            NvU32 expected = stream->other_stream_counter_expected[j];
            if (snapshot_last < expected || snapshot_first < expected) {
                UVM_TEST_PRINT("Stream %u other_counter snapshot[%u] = %u,%u which is < of %u,%u\n",
                               i,
                               j,
                               snapshot_last,
                               snapshot_first,
                               expected,
                               expected);
                status = NV_ERR_INVALID_STATE;
                goto done;
            }
        }
    }

    if (verbose > 0)
        UVM_TEST_PRINT("Verification done\n");

    schedule();

done:
    // Wait for all the trackers first before freeing up memory as streams
    // references each other's buffers.
    for (i = 0; i < num_streams; ++i) {
        uvm_test_stream_t *stream = &streams[i];
        uvm_tracker_wait(&stream->tracker);
    }

    for (i = 0; i < num_streams; ++i) {
        uvm_test_stream_t *stream = &streams[i];
        uvm_gpu_semaphore_free(&stream->semaphore);
        uvm_rm_mem_free(stream->other_stream_counter_snapshots_mem);
        uvm_rm_mem_free(stream->counter_snapshots_mem);
        uvm_rm_mem_free(stream->counter_mem);
        uvm_tracker_deinit(&stream->tracker);
        uvm_kvfree(stream->other_stream_counter_expected);
    }
    uvm_kvfree(streams);

    if (verbose > 0)
        UVM_TEST_PRINT("Cleanup done\n");

    return status;
}

























































NV_STATUS uvm_test_channel_sanity(UVM_TEST_CHANNEL_SANITY_PARAMS *params, struct file *filp)
{
    NV_STATUS status;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    uvm_mutex_lock(&g_uvm_global.global_lock);
    uvm_va_space_down_read_rm(va_space);

    status = test_ordering(va_space);
    if (status != NV_OK)
        goto done;







    g_uvm_global.disable_fatal_error_assert = true;
    uvm_release_asserts_set_global_error_for_tests = true;
    status = test_unexpected_completed_values(va_space);
    uvm_release_asserts_set_global_error_for_tests = false;
    g_uvm_global.disable_fatal_error_assert = false;
    if (status != NV_OK)
        goto done;

    if (g_uvm_global.num_simulated_devices == 0) {
        status = test_rc(va_space);
        if (status != NV_OK)
            goto done;
    }

done:
    uvm_va_space_up_read_rm(va_space);
    uvm_mutex_unlock(&g_uvm_global.global_lock);

    return status;
}

static NV_STATUS uvm_test_channel_stress_stream(uvm_va_space_t *va_space,
                                                const UVM_TEST_CHANNEL_STRESS_PARAMS *params)
{
    NV_STATUS status;

    if (params->iterations == 0 || params->num_streams == 0)
        return NV_ERR_INVALID_PARAMETER;

    // TODO: Bug 1764963: Rework the test to not rely on the global lock as that
    // serializes all the threads calling this at the same time.
    uvm_mutex_lock(&g_uvm_global.global_lock);
    uvm_va_space_down_read_rm(va_space);

    status = stress_test_all_gpus_in_va(va_space,
                                        params->num_streams,
                                        params->iterations,
                                        params->seed,
                                        params->verbose);
    if (status != NV_OK)
        goto done;

done:
    uvm_va_space_up_read_rm(va_space);
    uvm_mutex_unlock(&g_uvm_global.global_lock);

    return status;
}

static NV_STATUS uvm_test_channel_stress_update_channels(uvm_va_space_t *va_space,
                                                         const UVM_TEST_CHANNEL_STRESS_PARAMS *params)
{
    NV_STATUS status = NV_OK;
    uvm_test_rng_t rng;
    NvU32 i;

    uvm_test_rng_init(&rng, params->seed);

    uvm_va_space_down_read(va_space);

    for (i = 0; i < params->iterations; ++i) {
        uvm_gpu_t *gpu = random_va_space_gpu(&rng, va_space);
        uvm_channel_manager_update_progress(gpu->channel_manager);

        if (fatal_signal_pending(current)) {
            status = NV_ERR_SIGNAL_PENDING;
            goto done;
        }
    }

done:
    uvm_va_space_up_read(va_space);

    return status;
}

static NV_STATUS uvm_test_channel_noop_push(uvm_va_space_t *va_space,
                                            const UVM_TEST_CHANNEL_STRESS_PARAMS *params)
{
    NV_STATUS status = NV_OK;
    uvm_push_t push;
    uvm_test_rng_t rng;
    uvm_gpu_t *gpu;
    NvU32 i;

    uvm_test_rng_init(&rng, params->seed);

    uvm_va_space_down_read(va_space);

    for (i = 0; i < params->iterations; ++i) {
        uvm_channel_type_t channel_type = random_ce_channel_type(&rng);
        gpu = random_va_space_gpu(&rng, va_space);

        status = uvm_push_begin(gpu->channel_manager, channel_type, &push, "noop push");
        if (status != NV_OK)
            goto done;

        // Push an actual noop method so that the push doesn't get optimized
        // away if we ever detect empty pushes.
        gpu->parent->host_hal->noop(&push, UVM_METHOD_SIZE);

        uvm_push_end(&push);

        if (fatal_signal_pending(current)) {
            status = NV_ERR_SIGNAL_PENDING;
            goto done;
        }
    }
    if (params->verbose > 0)
        UVM_TEST_PRINT("Noop pushes: completed %u pushes seed: %u\n", i, params->seed);

    for_each_va_space_gpu_in_mask(gpu, va_space, &va_space->registered_gpu_va_spaces) {
        NV_STATUS wait_status = uvm_channel_manager_wait(gpu->channel_manager);
        if (status == NV_OK)
            status = wait_status;
    }

done:
    uvm_va_space_up_read(va_space);

    return status;
}

NV_STATUS uvm_test_channel_stress(UVM_TEST_CHANNEL_STRESS_PARAMS *params, struct file *filp)
{
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    switch (params->mode) {
        case UVM_TEST_CHANNEL_STRESS_MODE_STREAM:
            return uvm_test_channel_stress_stream(va_space, params);
        case UVM_TEST_CHANNEL_STRESS_MODE_UPDATE_CHANNELS:
            return uvm_test_channel_stress_update_channels(va_space, params);
        case UVM_TEST_CHANNEL_STRESS_MODE_NOOP_PUSH:
            return uvm_test_channel_noop_push(va_space, params);
        default:
            return NV_ERR_INVALID_PARAMETER;
    }
}
