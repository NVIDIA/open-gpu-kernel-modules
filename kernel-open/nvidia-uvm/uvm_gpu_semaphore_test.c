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

#include "uvm_global.h"
#include "uvm_gpu_semaphore.h"
#include "uvm_test.h"
#include "uvm_va_space.h"
#include "uvm_kvmalloc.h"

static NV_STATUS set_and_test(uvm_gpu_tracking_semaphore_t *tracking_sem, NvU64 new_value)
{
    uvm_gpu_semaphore_set_payload(&tracking_sem->semaphore, (NvU32)new_value);
    TEST_CHECK_RET(uvm_gpu_tracking_semaphore_update_completed_value(tracking_sem) == new_value);
    TEST_CHECK_RET(uvm_gpu_tracking_semaphore_is_value_completed(tracking_sem, new_value));
    TEST_CHECK_RET(uvm_gpu_tracking_semaphore_is_value_completed(tracking_sem, new_value - 1));
    TEST_CHECK_RET(!uvm_gpu_tracking_semaphore_is_value_completed(tracking_sem, new_value + 1));
    TEST_CHECK_RET(uvm_gpu_tracking_semaphore_is_completed(tracking_sem));

    return NV_OK;
}

static NV_STATUS add_and_test(uvm_gpu_tracking_semaphore_t *tracking_sem, NvU32 increment_by)
{
    NvU64 new_value;
    NvU64 completed = uvm_gpu_tracking_semaphore_update_completed_value(tracking_sem);
    new_value = completed + increment_by;
    tracking_sem->queued_value = new_value;

    TEST_CHECK_RET(uvm_gpu_tracking_semaphore_update_completed_value(tracking_sem) == completed);
    TEST_CHECK_RET(uvm_gpu_tracking_semaphore_is_value_completed(tracking_sem, 0));
    if (completed > 0)
        TEST_CHECK_RET(uvm_gpu_tracking_semaphore_is_value_completed(tracking_sem, completed - 1));
    TEST_CHECK_RET(uvm_gpu_tracking_semaphore_is_value_completed(tracking_sem, completed));
    TEST_CHECK_RET(!uvm_gpu_tracking_semaphore_is_value_completed(tracking_sem, completed + 1));
    TEST_CHECK_RET(!uvm_gpu_tracking_semaphore_is_value_completed(tracking_sem, new_value));
    TEST_CHECK_RET(!uvm_gpu_tracking_semaphore_is_completed(tracking_sem));

    TEST_NV_CHECK_RET(set_and_test(tracking_sem, new_value));
    TEST_CHECK_RET(uvm_gpu_tracking_semaphore_is_value_completed(tracking_sem, completed));

    return NV_OK;
}

// Set the current state of the sema, avoiding UVM_GPU_SEMAPHORE_MAX_JUMP
// detection.
static void manual_set(uvm_gpu_tracking_semaphore_t *tracking_sem, NvU64 value)
{
    uvm_gpu_semaphore_set_payload(&tracking_sem->semaphore, (NvU32)value);
    atomic64_set(&tracking_sem->completed_value, value);
    tracking_sem->queued_value = value;
}

// Set the starting value and payload and expect a global error
static NV_STATUS set_and_expect_error(uvm_gpu_tracking_semaphore_t *tracking_sem, NvU64 starting_value, NvU32 payload)
{
    manual_set(tracking_sem, starting_value);
    uvm_gpu_semaphore_set_payload(&tracking_sem->semaphore, payload);

    TEST_CHECK_RET(uvm_global_get_status() == NV_OK);
    uvm_gpu_tracking_semaphore_update_completed_value(tracking_sem);
    TEST_CHECK_RET(uvm_global_reset_fatal_error() == NV_ERR_INVALID_STATE);

    return NV_OK;
}

static NV_STATUS test_invalid_jumps(uvm_gpu_tracking_semaphore_t *tracking_sem)
{
    int i;
    for (i = 0; i < 10; ++i) {
        NvU64 base = (1ULL<<32) * i;
        TEST_NV_CHECK_RET(set_and_expect_error(tracking_sem, base, UVM_GPU_SEMAPHORE_MAX_JUMP + 1));
        TEST_NV_CHECK_RET(set_and_expect_error(tracking_sem, base, UINT_MAX));
        TEST_NV_CHECK_RET(set_and_expect_error(tracking_sem, base + i + 1, i));
        TEST_NV_CHECK_RET(set_and_expect_error(tracking_sem, base + UINT_MAX / 2, UINT_MAX / 2 + UVM_GPU_SEMAPHORE_MAX_JUMP + 1));
        TEST_NV_CHECK_RET(set_and_expect_error(tracking_sem, base + UINT_MAX / 2, UINT_MAX / 2 - i - 1));
    }

    return NV_OK;
}

static NV_STATUS test_tracking(uvm_va_space_t *va_space)
{
    NV_STATUS status;
    uvm_gpu_tracking_semaphore_t tracking_sem;
    int i;
    uvm_gpu_t *gpu = uvm_va_space_find_first_gpu(va_space);

    if (gpu == NULL)
        return NV_ERR_INVALID_STATE;

    status = uvm_gpu_tracking_semaphore_alloc(gpu->semaphore_pool, &tracking_sem);
    if (status != NV_OK)
        return status;

    status = add_and_test(&tracking_sem, 1);
    if (status != NV_OK)
        goto done;

    for (i = 0; i < 100; ++i) {
        status = add_and_test(&tracking_sem, UVM_GPU_SEMAPHORE_MAX_JUMP - i);
        if (status != NV_OK)
            goto done;
    }

    // Test wrap-around cases
    for (i = 0; i < 100; ++i) {
        // Start with a value right before wrap-around
        NvU64 starting_value = (1ULL<<32) * (i + 1) - i - 1;
        manual_set(&tracking_sem, starting_value);

        // And set payload to after wrap-around
        status = set_and_test(&tracking_sem, (1ULL<<32) * (i + 1) + i);
        if (status != NV_OK)
            goto done;
    }

    g_uvm_global.disable_fatal_error_assert = true;
    uvm_release_asserts_set_global_error_for_tests = true;
    status = test_invalid_jumps(&tracking_sem);
    uvm_release_asserts_set_global_error_for_tests = false;
    g_uvm_global.disable_fatal_error_assert = false;
    if (status != NV_OK)
        goto done;

done:
    uvm_gpu_tracking_semaphore_free(&tracking_sem);
    return status;
}

#define NUM_SEMAPHORES_PER_GPU 4096

static NV_STATUS test_alloc(uvm_va_space_t *va_space)
{
    NV_STATUS status = NV_OK;
    uvm_gpu_t *gpu;
    uvm_gpu_semaphore_t *semaphores;
    int i;
    NvU32 semaphore_count;
    NvU32 gpu_count = uvm_processor_mask_get_gpu_count(&va_space->registered_gpus);
    NvU32 current_semaphore = 0;

    if (gpu_count == 0)
        return NV_ERR_INVALID_STATE;

    semaphore_count = gpu_count * NUM_SEMAPHORES_PER_GPU;

    semaphores = uvm_kvmalloc_zero(semaphore_count * sizeof(*semaphores));
    if (semaphores == NULL)
        return NV_ERR_NO_MEMORY;

    for (i = 0; i < NUM_SEMAPHORES_PER_GPU; ++i) {
        for_each_va_space_gpu(gpu, va_space) {
            status = uvm_gpu_semaphore_alloc(gpu->semaphore_pool, &semaphores[current_semaphore++]);
            if (status != NV_OK)
                goto done;
        }
    }

    for (i = 0; i < current_semaphore; ++i) {
        for_each_va_space_gpu(gpu, va_space) {
            NvU64 gpu_va;

            gpu_va = uvm_gpu_semaphore_get_gpu_uvm_va(&semaphores[i], gpu);
            TEST_CHECK_GOTO(gpu_va != 0, done);

            // In SR-IOV heavy, there should be a mapping in the proxy VA space
            // too.
            if (uvm_parent_gpu_needs_proxy_channel_pool(gpu->parent)) {
                gpu_va = uvm_gpu_semaphore_get_gpu_proxy_va(&semaphores[i], gpu);
                TEST_CHECK_GOTO(gpu_va != 0, done);
            }

            uvm_gpu_semaphore_set_payload(&semaphores[i], 1);
            TEST_CHECK_GOTO(uvm_gpu_semaphore_get_payload(&semaphores[i]) == 1, done);
        }
    }

done:
    for (i = 0; i < current_semaphore; ++i)
        uvm_gpu_semaphore_free(&semaphores[i]);

    uvm_kvfree(semaphores);

    return status;
}


NV_STATUS uvm_test_gpu_semaphore_sanity(UVM_TEST_GPU_SEMAPHORE_SANITY_PARAMS *params, struct file *filp)
{
    NV_STATUS status;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    uvm_mutex_lock(&g_uvm_global.global_lock);
    uvm_va_space_down_read_rm(va_space);

    status = test_alloc(va_space);
    if (status != NV_OK)
        goto done;

    status = test_tracking(va_space);
    if (status != NV_OK)
        goto done;

done:
    uvm_va_space_up_read_rm(va_space);
    uvm_mutex_unlock(&g_uvm_global.global_lock);

    return status;
}
