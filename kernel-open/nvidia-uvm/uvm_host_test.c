/*******************************************************************************
    Copyright (c) 2020-2022 NVIDIA Corporation

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
#include "uvm_common.h"
#include "uvm_hal.h"
#include "uvm_push.h"
#include "uvm_test.h"
#include "uvm_va_space.h"
#include "uvm_mem.h"
#include "uvm_rm_mem.h"

typedef struct test_sem_mem_t {
    void *cpu_va;
    NvU64 gpu_va;

    union {
        uvm_mem_t *uvm_mem;
        uvm_rm_mem_t *rm_mem;
    };
} test_sem_mem;

static NV_STATUS test_semaphore_alloc_uvm_rm_mem(uvm_gpu_t *gpu, const size_t size, test_sem_mem *mem_out)
{
    NV_STATUS status;
    uvm_rm_mem_t *mem = NULL;
    NvU64 gpu_va;

    status = uvm_rm_mem_alloc_and_map_cpu(gpu, UVM_RM_MEM_TYPE_SYS, size, 0, &mem);
    TEST_NV_CHECK_RET(status);

    gpu_va = uvm_rm_mem_get_gpu_uvm_va(mem, gpu);
    TEST_CHECK_GOTO(gpu_va < gpu->parent->max_host_va, error);

    mem_out->cpu_va = uvm_rm_mem_get_cpu_va(mem);
    mem_out->gpu_va = gpu_va;
    mem_out->rm_mem = mem;

    return NV_OK;

error:
    uvm_rm_mem_free(mem);
    return status;
}

static NV_STATUS test_semaphore_alloc_sem(uvm_gpu_t *gpu, const size_t size, test_sem_mem *mem_out)
{
    NV_STATUS status = NV_OK;
    uvm_mem_t *mem = NULL;
    NvU64 gpu_va;

    TEST_NV_CHECK_RET(uvm_mem_alloc_sysmem(size, current->mm, &mem));

    TEST_NV_CHECK_GOTO(uvm_mem_map_gpu_kernel(mem, gpu), error);
    gpu_va = uvm_mem_get_gpu_va_kernel(mem, gpu);

    // Use an RM allocation when Host cannot address the semaphore.
    if (gpu_va >= gpu->parent->max_host_va) {
        uvm_mem_free(mem);
        return test_semaphore_alloc_uvm_rm_mem(gpu, size, mem_out);
    }

    // This semaphore resides in the uvm_mem region, i.e., it has the GPU VA
    // MSbit set. The intent is to validate semaphore operations when the
    // semaphore's VA is in the high-end of the GPU effective virtual address
    // space spectrum, i.e., its VA upper-bit is set.
    TEST_CHECK_GOTO(gpu_va & (1ULL << (gpu->address_space_tree.hal->num_va_bits() - 1)), error);

    TEST_NV_CHECK_GOTO(uvm_mem_map_cpu_kernel(mem), error);

    mem_out->cpu_va = uvm_mem_get_cpu_addr_kernel(mem);
    mem_out->gpu_va = gpu_va;
    mem_out->uvm_mem = mem;

    return NV_OK;

error:
    uvm_mem_free(mem);
    return status;
}

static void test_semaphore_free_sem(uvm_gpu_t *gpu, test_sem_mem *mem)
{
    if (mem->gpu_va >= gpu->parent->uvm_mem_va_base)
        uvm_mem_free(mem->uvm_mem);
    else
        uvm_rm_mem_free(mem->rm_mem);
}

// This test is similar to the test_semaphore_release() test in uvm_ce_test.c,
// except that this one uses host_hal->semaphore_release();
static NV_STATUS test_semaphore_release(uvm_gpu_t *gpu)
{
    NV_STATUS status;
    test_sem_mem mem = { 0 };
    uvm_push_t push;
    NvU32 value;
    NvU32 payload = 0xA5A55A5A;
    NvU32 *cpu_ptr;

    // Semaphore release needs 1 word (4 bytes).
    const size_t size = sizeof(NvU32);

    status = test_semaphore_alloc_sem(gpu, size, &mem);
    TEST_NV_CHECK_RET(status);

    // Initialize the payload.
    cpu_ptr = (NvU32 *)mem.cpu_va;
    *cpu_ptr = 0;

    status = uvm_push_begin(gpu->channel_manager, UVM_CHANNEL_TYPE_GPU_INTERNAL, &push, "semaphore_release test");
    TEST_NV_CHECK_GOTO(status, done);

    gpu->parent->host_hal->semaphore_release(&push, mem.gpu_va, payload);

    status = uvm_push_end_and_wait(&push);
    TEST_NV_CHECK_GOTO(status, done);

    value = *cpu_ptr;
    if (value != payload) {
        UVM_TEST_PRINT("Semaphore payload = %u instead of %u, GPU %s\n", value, payload, uvm_gpu_name(gpu));
        status = NV_ERR_INVALID_STATE;
        goto done;
    }

done:
    test_semaphore_free_sem(gpu, &mem);

    return status;
}

static NV_STATUS test_semaphore_acquire(uvm_gpu_t *gpu)
{
    NV_STATUS status;
    test_sem_mem mem = { 0 };
    uvm_push_t push;
    uvm_spin_loop_t spin;
    NvU32 *cpu_ptr, *cpu_sema_A, *cpu_sema_B, *cpu_sema_C;
    NvU64 gpu_sema_va_A, gpu_sema_va_B, gpu_sema_va_C;
    bool check_sema_C;

    // The semaphore is one word long(4 bytes), we use three semaphores.
    const size_t sema_size = 4;
    const size_t size = sema_size * 3;

    status = test_semaphore_alloc_sem(gpu, size, &mem);
    TEST_NV_CHECK_RET(status);

    gpu_sema_va_A = mem.gpu_va;
    gpu_sema_va_B = mem.gpu_va + sema_size;
    gpu_sema_va_C = mem.gpu_va + 2 * sema_size;

    cpu_ptr = (NvU32 *)mem.cpu_va;
    memset(cpu_ptr, 0, size);
    cpu_sema_A = cpu_ptr;
    cpu_sema_B = cpu_ptr + 1;
    cpu_sema_C = cpu_ptr + 2;

    status = uvm_push_begin(gpu->channel_manager, UVM_CHANNEL_TYPE_GPU_INTERNAL, &push, "semaphore_acquire test");
    TEST_NV_CHECK_GOTO(status, done);

    gpu->parent->host_hal->semaphore_release(&push, gpu_sema_va_A, 1);
    gpu->parent->host_hal->semaphore_acquire(&push, gpu_sema_va_B, 1);
    gpu->parent->host_hal->semaphore_release(&push, gpu_sema_va_C, 1);

    uvm_push_end(&push);

    // Wait for sema_A release.
    UVM_SPIN_WHILE(READ_ONCE(*cpu_sema_A) != 1, &spin);

    // Sleep for 10ms, the GPU waits while sema_B is held by us.
    msleep(10);

    check_sema_C = READ_ONCE(*cpu_sema_C) == 0;

    // memory fence/barrier, check comment in
    // uvm_gpu_semaphore.c:uvm_gpu_semaphore_set_payload() for details.
    mb();

    // Release sema_B.
    WRITE_ONCE(*cpu_sema_B, 1);

    // Wait for the GPU to release sema_C, i.e., the end of the push.
    status = uvm_push_wait(&push);
    TEST_CHECK_GOTO(status == NV_OK, done);

    // check_sema_C is validated here to ensure the push has ended and was not
    // interrupted in the middle, had the check failed.
    TEST_CHECK_GOTO(check_sema_C, done);
    TEST_CHECK_GOTO(READ_ONCE(*cpu_sema_C) == 1, done);

done:
    test_semaphore_free_sem(gpu, &mem);

    return status;
}

// This test is similar to the test_semaphore_timestamp() test in
// uvm_ce_test.c, except that this one uses host_hal->semaphore_timestamp();
static NV_STATUS test_semaphore_timestamp(uvm_gpu_t *gpu)
{
    NV_STATUS status;
    test_sem_mem mem = { 0 };
    uvm_push_t push;
    NvU32 i;
    NvU64 *timestamp;
    NvU64 last_timestamp = 0;

    // 2 iterations:
    //   1: compare retrieved timestamp with 0;
    //   2: compare retrieved timestamp with previous timestamp (obtained in 1).
    const NvU32 iterations = 2;

    // The semaphore is 4 words long (16 bytes).
    const size_t size = 16;

    status = test_semaphore_alloc_sem(gpu, size, &mem);
    TEST_NV_CHECK_RET(status);

    timestamp = (NvU64 *)mem.cpu_va;
    TEST_CHECK_GOTO(timestamp != NULL, done);
    memset(timestamp, 0, size);

    // Shift the timestamp pointer to where the semaphore timestamp info is.
    timestamp += 1;

    for (i = 0; i < iterations; i++) {
        status = uvm_push_begin(gpu->channel_manager,
                                UVM_CHANNEL_TYPE_GPU_INTERNAL,
                                &push,
                                "semaphore_timestamp test, iter: %u",
                                i);
        TEST_NV_CHECK_GOTO(status, done);

        gpu->parent->host_hal->semaphore_timestamp(&push, mem.gpu_va);

        status = uvm_push_end_and_wait(&push);
        TEST_NV_CHECK_GOTO(status, done);

        TEST_CHECK_GOTO(*timestamp != 0, done);
        TEST_CHECK_GOTO(*timestamp >= last_timestamp, done);
        last_timestamp = *timestamp;
    }

done:
    test_semaphore_free_sem(gpu, &mem);

    return status;
}

static NV_STATUS test_host(uvm_va_space_t *va_space)
{
    uvm_gpu_t *gpu;

    for_each_va_space_gpu(gpu, va_space) {
        TEST_NV_CHECK_RET(test_semaphore_release(gpu));
        TEST_NV_CHECK_RET(test_semaphore_acquire(gpu));
        TEST_NV_CHECK_RET(test_semaphore_timestamp(gpu));
    }

    return NV_OK;
}

NV_STATUS uvm_test_host_sanity(UVM_TEST_HOST_SANITY_PARAMS *params, struct file *filp)
{
    NV_STATUS status;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    uvm_va_space_down_read_rm(va_space);

    status = test_host(va_space);

    uvm_va_space_up_read_rm(va_space);

    return status;
}
