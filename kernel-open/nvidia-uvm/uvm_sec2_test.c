/*******************************************************************************
    Copyright (c) 2021-2024 NVIDIA Corporation

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
#include "uvm_gpu.h"
#include "uvm_mem.h"
#include "uvm_push.h"
#include "uvm_hal.h"
#include "uvm_test.h"
#include "uvm_va_space.h"
#include "uvm_kvmalloc.h"
#include <linux/string.h>
#include "nv_uvm_interface.h"

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

    TEST_NV_CHECK_RET(uvm_mem_alloc_sysmem_dma(size, gpu, current->mm, &mem));

    TEST_NV_CHECK_GOTO(uvm_mem_map_gpu_kernel(mem, gpu), error);
    gpu_va = uvm_mem_get_gpu_va_kernel(mem, gpu);

    // Use an RM allocation when SEC2 cannot address the semaphore.
    // SEC2 VA width is similar to Host's.
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

// This test is similar to the test_semaphore_release() test in
// uvm_host_test.c, except that this one uses sec2_hal->semaphore_release();
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

    status = uvm_push_begin(gpu->channel_manager, UVM_CHANNEL_TYPE_SEC2, &push, "semaphore_release test");
    TEST_NV_CHECK_GOTO(status, done);

    gpu->parent->sec2_hal->semaphore_release(&push, mem.gpu_va, payload);

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

// This test is similiar to the test_semaphore_timestamp() test in
// uvm_ce_test.c, except that this one uses sec2_hal->semaphore_timestamp();
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

    // TODO: Bug 3804752: SEC2 semaphore timestamp is not implemented in
    // Hopper+ GPUs.
    if (g_uvm_global.conf_computing_enabled)
        return NV_OK;

    status = test_semaphore_alloc_sem(gpu, size, &mem);
    TEST_NV_CHECK_RET(status);

    timestamp = (NvU64 *)mem.cpu_va;
    TEST_CHECK_GOTO(timestamp != NULL, done);
    memset(timestamp, 0, size);

    // Shift the timestamp pointer to where the semaphore timestamp info is.
    timestamp += 1;

    for (i = 0; i < iterations; i++) {
        status = uvm_push_begin(gpu->channel_manager,
                                UVM_CHANNEL_TYPE_SEC2,
                                &push,
                                "semaphore_timestamp test, iter: %u",
                                i);
        TEST_NV_CHECK_GOTO(status, done);

        gpu->parent->sec2_hal->semaphore_timestamp(&push, mem.gpu_va);

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

typedef enum
{
    MEM_ALLOC_TYPE_SYSMEM_DMA,
    MEM_ALLOC_TYPE_SYSMEM_PROTECTED,
    MEM_ALLOC_TYPE_VIDMEM_PROTECTED
} mem_alloc_type_t;

static bool mem_match(uvm_mem_t *mem1, uvm_mem_t *mem2)
{
    void *mem1_addr;
    void *mem2_addr;

    UVM_ASSERT(uvm_mem_is_sysmem(mem1));
    UVM_ASSERT(uvm_mem_is_sysmem(mem2));
    UVM_ASSERT(mem1->size == mem2->size);

    mem1_addr = uvm_mem_get_cpu_addr_kernel(mem1);
    mem2_addr = uvm_mem_get_cpu_addr_kernel(mem2);

    return !memcmp(mem1_addr, mem2_addr, mem1->size);
}

static NV_STATUS ce_memset_gpu(uvm_gpu_t *gpu, uvm_mem_t *mem, size_t size, NvU32 val)
{
    uvm_push_t push;

    TEST_NV_CHECK_RET(uvm_push_begin(gpu->channel_manager,
                                     UVM_CHANNEL_TYPE_GPU_INTERNAL,
                                     &push,
                                     "VPR memset"));

    gpu->parent->ce_hal->memset_4(&push, uvm_mem_gpu_address_virtual_kernel(mem, gpu), val, size);

    TEST_NV_CHECK_RET(uvm_push_end_and_wait(&push));

    return NV_OK;
}

static void write_range_cpu(uvm_mem_t *mem, size_t size, NvU64 base_val)
{
    char *start, *end;

    UVM_ASSERT(uvm_mem_is_sysmem(mem));

    start = uvm_mem_get_cpu_addr_kernel(mem);
    end = start + size;

    for (; start < end; start += sizeof(NvU64))
        *(NvU64 *) start = base_val++;
}

static NV_STATUS alloc_and_init_mem(uvm_gpu_t *gpu, uvm_mem_t **mem, size_t size, mem_alloc_type_t type)
{
    NV_STATUS status = NV_OK;

    UVM_ASSERT(mem);

    *mem = NULL;

    if (type == MEM_ALLOC_TYPE_VIDMEM_PROTECTED) {
        TEST_NV_CHECK_RET(uvm_mem_alloc_vidmem(size, gpu, mem));
        TEST_NV_CHECK_GOTO(uvm_mem_map_gpu_kernel(*mem, gpu), err);
        TEST_NV_CHECK_GOTO(ce_memset_gpu(gpu, *mem, size, 0xdead), err);
    }
    else {
        if (type == MEM_ALLOC_TYPE_SYSMEM_DMA) {
            TEST_NV_CHECK_RET(uvm_mem_alloc_sysmem_dma(size, gpu, NULL, mem));
            TEST_NV_CHECK_GOTO(uvm_mem_map_gpu_kernel(*mem, gpu), err);
        }
        else {
            TEST_NV_CHECK_RET(uvm_mem_alloc_sysmem(size, NULL, mem));
        }

        TEST_NV_CHECK_GOTO(uvm_mem_map_cpu_kernel(*mem), err);
        write_range_cpu(*mem, size, 0xdeaddead);
    }

    return NV_OK;

err:
    uvm_mem_free(*mem);
    return status;
}

static void cpu_encrypt(uvm_channel_t *channel,
                        uvm_mem_t *dst_mem,
                        uvm_mem_t *src_mem,
                        uvm_mem_t *auth_tag_mem,
                        size_t size,
                        size_t copy_size)
{
    size_t i;
    void *src_plain = uvm_mem_get_cpu_addr_kernel(src_mem);
    void *dst_cipher = uvm_mem_get_cpu_addr_kernel(dst_mem);
    void *auth_tag_buffer = uvm_mem_get_cpu_addr_kernel(auth_tag_mem);

    UVM_ASSERT(IS_ALIGNED(size, copy_size));

    for (i = 0; i < size / copy_size; i++) {
        uvm_conf_computing_cpu_encrypt(channel, dst_cipher, src_plain, NULL, copy_size, auth_tag_buffer);

        src_plain = (char *) src_plain + copy_size;
        dst_cipher = (char *) dst_cipher + copy_size;
        auth_tag_buffer = (char *) auth_tag_buffer + UVM_CONF_COMPUTING_AUTH_TAG_SIZE;
    }
}

static NV_STATUS cpu_decrypt(uvm_channel_t *channel,
                             uvm_mem_t *dst_mem,
                             uvm_mem_t *src_mem,
                             UvmCslIv *decrypt_iv,
                             NvU32 key_version,
                             uvm_mem_t *auth_tag_mem,
                             size_t size,
                             size_t copy_size)
{
    size_t i;
    void *src_cipher = uvm_mem_get_cpu_addr_kernel(src_mem);
    void *dst_plain = uvm_mem_get_cpu_addr_kernel(dst_mem);
    void *auth_tag_buffer = uvm_mem_get_cpu_addr_kernel(auth_tag_mem);

    UVM_ASSERT(IS_ALIGNED(size, copy_size));

    for (i = 0; i < size / copy_size; i++) {
        TEST_NV_CHECK_RET(uvm_conf_computing_cpu_decrypt(channel,
                                                         dst_plain,
                                                         src_cipher,
                                                         &decrypt_iv[i],
                                                         key_version,
                                                         copy_size,
                                                         auth_tag_buffer));

        dst_plain = (char *) dst_plain + copy_size;
        src_cipher = (char *) src_cipher + copy_size;
        auth_tag_buffer = (char *) auth_tag_buffer + UVM_CONF_COMPUTING_AUTH_TAG_SIZE;
    }

    return NV_OK;
}

// gpu_encrypt uses the Copy Engine for encryption, instead of SEC2. SEC2 does
// not support encryption. The following function is copied from uvm_ce_test.c
// and adapted to SEC2 tests.
static void gpu_encrypt(uvm_push_t *push,
                        uvm_mem_t *dst_mem,
                        uvm_mem_t *src_mem,
                        UvmCslIv *decrypt_iv,
                        uvm_mem_t *auth_tag_mem,
                        size_t size,
                        size_t copy_size)
{
    size_t i;
    size_t num_iterations = size / copy_size;
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    uvm_gpu_address_t dst_cipher_address = uvm_mem_gpu_address_virtual_kernel(dst_mem, gpu);
    uvm_gpu_address_t src_plain_address = uvm_mem_gpu_address_virtual_kernel(src_mem, gpu);
    uvm_gpu_address_t auth_tag_address = uvm_mem_gpu_address_virtual_kernel(auth_tag_mem, gpu);

    for (i = 0; i < num_iterations; i++) {
        uvm_conf_computing_log_gpu_encryption(push->channel, copy_size, decrypt_iv);

        if (i > 0)
            uvm_push_set_flag(push, UVM_PUSH_FLAG_CE_NEXT_PIPELINED);

        uvm_push_set_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);

        gpu->parent->ce_hal->encrypt(push, dst_cipher_address, src_plain_address, copy_size, auth_tag_address);
        dst_cipher_address.address += copy_size;
        src_plain_address.address += copy_size;
        auth_tag_address.address += UVM_CONF_COMPUTING_AUTH_TAG_SIZE;
        decrypt_iv++;
    }
}

static void gpu_decrypt(uvm_push_t *push,
                        uvm_mem_t *dst_mem,
                        uvm_mem_t *src_mem,
                        uvm_mem_t *auth_tag_mem,
                        size_t size,
                        size_t copy_size)
{
    size_t i;
    size_t num_iterations = size / copy_size;
    uvm_gpu_t *gpu = uvm_push_get_gpu(push);
    uvm_gpu_address_t src_cipher_address = uvm_mem_gpu_address_virtual_kernel(src_mem, gpu);
    uvm_gpu_address_t dst_plain_address = uvm_mem_gpu_address_virtual_kernel(dst_mem, gpu);
    uvm_gpu_address_t auth_tag_gpu_address = uvm_mem_gpu_address_virtual_kernel(auth_tag_mem, gpu);

    UVM_ASSERT(IS_ALIGNED(size, copy_size));

    for (i = 0; i < num_iterations; i++) {
        uvm_push_set_flag(push, UVM_PUSH_FLAG_NEXT_MEMBAR_NONE);
        gpu->parent->sec2_hal->decrypt(push,
                                       dst_plain_address.address,
                                       src_cipher_address.address,
                                       copy_size,
                                       auth_tag_gpu_address.address);

        dst_plain_address.address += copy_size;
        src_cipher_address.address += copy_size;
        auth_tag_gpu_address.address += UVM_CONF_COMPUTING_AUTH_TAG_SIZE;
    }
}

// This test depends on the CE for the encryption, so we assume tests from
// uvm_ce_test.c have successfully passed.
static NV_STATUS test_cpu_to_gpu_roundtrip(uvm_gpu_t *gpu, size_t copy_size, size_t size)
{
    NV_STATUS status = NV_OK;
    uvm_mem_t *src_plain = NULL;
    uvm_mem_t *src_cipher = NULL;
    uvm_mem_t *dst_cipher = NULL;
    uvm_mem_t *dst_plain = NULL;
    uvm_mem_t *dst_plain_cpu = NULL;
    uvm_mem_t *auth_tag_mem = NULL;
    size_t auth_tag_buffer_size = (size / copy_size) * UVM_CONF_COMPUTING_AUTH_TAG_SIZE;
    uvm_push_t push;
    UvmCslIv *decrypt_iv;
    NvU32 key_version;

    decrypt_iv = uvm_kvmalloc_zero((size / copy_size) * sizeof(UvmCslIv));
    if (!decrypt_iv)
        return NV_ERR_NO_MEMORY;

    TEST_NV_CHECK_GOTO(alloc_and_init_mem(gpu, &src_plain, size, MEM_ALLOC_TYPE_SYSMEM_PROTECTED), out);
    TEST_NV_CHECK_GOTO(alloc_and_init_mem(gpu, &src_cipher, size, MEM_ALLOC_TYPE_SYSMEM_DMA), out);
    TEST_NV_CHECK_GOTO(alloc_and_init_mem(gpu, &dst_cipher, size, MEM_ALLOC_TYPE_SYSMEM_DMA), out);
    TEST_NV_CHECK_GOTO(alloc_and_init_mem(gpu, &dst_plain, size, MEM_ALLOC_TYPE_VIDMEM_PROTECTED), out);
    TEST_NV_CHECK_GOTO(alloc_and_init_mem(gpu, &dst_plain_cpu, size, MEM_ALLOC_TYPE_SYSMEM_PROTECTED), out);
    TEST_NV_CHECK_GOTO(alloc_and_init_mem(gpu, &auth_tag_mem, auth_tag_buffer_size, MEM_ALLOC_TYPE_SYSMEM_DMA), out);

    write_range_cpu(src_plain, size, uvm_get_stale_thread_id());

    TEST_NV_CHECK_GOTO(uvm_push_begin(gpu->channel_manager, UVM_CHANNEL_TYPE_SEC2, &push, "enc(cpu)_dec(gpu)"), out);

    cpu_encrypt(push.channel, src_cipher, src_plain, auth_tag_mem, size, copy_size);
    gpu_decrypt(&push, dst_plain, src_cipher, auth_tag_mem, size, copy_size);

    // Wait for SEC2 before launching the CE part.
    // SEC2 is only allowed to release semaphores in unprotected sysmem,
    // and CE can only acquire semaphores in protected vidmem.
    TEST_NV_CHECK_GOTO(uvm_push_end_and_wait(&push), out);

    TEST_NV_CHECK_GOTO(uvm_push_begin(gpu->channel_manager, UVM_CHANNEL_TYPE_GPU_TO_CPU, &push, "enc(gpu)_dec(cpu)"),
                       out);

    gpu_encrypt(&push, dst_cipher, dst_plain, decrypt_iv, auth_tag_mem, size, copy_size);

    // There shouldn't be any key rotation between the end of the push and the
    // CPU decryption(s), but it is more robust against test changes to force
    // decryption to use the saved key.
    key_version = uvm_channel_pool_key_version(push.channel->pool);

    TEST_NV_CHECK_GOTO(uvm_push_end_and_wait(&push), out);

    TEST_CHECK_GOTO(!mem_match(src_plain, src_cipher), out);
    TEST_CHECK_GOTO(!mem_match(dst_cipher, src_plain), out);

    TEST_NV_CHECK_GOTO(cpu_decrypt(push.channel,
                                   dst_plain_cpu,
                                   dst_cipher,
                                   decrypt_iv,
                                   key_version,
                                   auth_tag_mem,
                                   size,
                                   copy_size),
                       out);

    TEST_CHECK_GOTO(mem_match(src_plain, dst_plain_cpu), out);

out:
    uvm_mem_free(auth_tag_mem);
    uvm_mem_free(dst_plain_cpu);
    uvm_mem_free(dst_plain);
    uvm_mem_free(dst_cipher);
    uvm_mem_free(src_cipher);
    uvm_mem_free(src_plain);

    uvm_kvfree(decrypt_iv);

    return status;
}

static NV_STATUS test_encryption_decryption(uvm_gpu_t *gpu)
{
    size_t copy_sizes[] = { 4, 16, 512, 2 * UVM_SIZE_1KB, 4 * UVM_SIZE_1KB, 64 * UVM_SIZE_1KB, 2 * UVM_SIZE_1MB };
    int i;

    for (i = 0; i < ARRAY_SIZE(copy_sizes); i++) {
        // Limit the number of methods in the gpu_encrypt()/gpu_decrypt() work
        // submission.
        size_t size = min(UVM_VA_BLOCK_SIZE, 256ull * copy_sizes[i]);

        // gpu_encrypt() and gpu_decrypt() iterate over a 'size' buffer. If
        // copy_sizes[i] < 16 (SEC2 src and dst alignment requirement is
        // 16-byte), SEC2 and our HAL implementation assert/fail. When
        // copy_sizes[i] < 16, we only perform a single copy_sizes[i] copy.
        if (copy_sizes[i] < 16)
            size = copy_sizes[i];

        UVM_ASSERT(size % copy_sizes[i] == 0);

        TEST_NV_CHECK_RET(test_cpu_to_gpu_roundtrip(gpu, copy_sizes[i], size));
    }

    return NV_OK;
}

static NV_STATUS test_sec2(uvm_va_space_t *va_space)
{
    uvm_gpu_t *gpu;

    for_each_va_space_gpu(gpu, va_space) {
        TEST_NV_CHECK_RET(test_semaphore_release(gpu));
        TEST_NV_CHECK_RET(test_semaphore_timestamp(gpu));
        TEST_NV_CHECK_RET(test_encryption_decryption(gpu));
    }

    return NV_OK;
}

NV_STATUS uvm_test_sec2_sanity(UVM_TEST_SEC2_SANITY_PARAMS *params, struct file *filp)
{
    NV_STATUS status;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    TEST_CHECK_RET(g_uvm_global.conf_computing_enabled);

    uvm_va_space_down_read_rm(va_space);

    status = test_sec2(va_space);

    uvm_va_space_up_read_rm(va_space);

    return status;
}

NV_STATUS uvm_test_sec2_cpu_gpu_roundtrip(UVM_TEST_SEC2_CPU_GPU_ROUNDTRIP_PARAMS *params, struct file *filp)
{
    NV_STATUS status = NV_OK;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);
    uvm_gpu_t *gpu;

    TEST_CHECK_RET(g_uvm_global.conf_computing_enabled);

    uvm_va_space_down_read(va_space);

    for_each_va_space_gpu(gpu, va_space) {
        // To exercise certain SEC2 context save/restore races, do a looped
        // decrypt with smaller copy sizes instead of larger copy sizes since we
        // need SEC2 to context switch with pending work in different channels
        // and smaller copies decrypt increases the probability of exercising
        // SEC2 context switching. A single push of the entire size may not be
        // enough to re-create this scenario since SEC2 doesn't preempt in the
        // middle of the decrypt.
        status = test_cpu_to_gpu_roundtrip(gpu, UVM_PAGE_SIZE_4K, UVM_VA_BLOCK_SIZE);
        if (status != NV_OK)
            goto done;
    }

done:
    uvm_va_space_up_read(va_space);

    return status;
}
