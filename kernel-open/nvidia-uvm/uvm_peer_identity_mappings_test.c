/*******************************************************************************
    Copyright (c) 2016-2024 NVIDIA Corporation

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

#include "uvm_test.h"
#include "uvm_va_space.h"
#include "uvm_mem.h"
#include "uvm_push.h"
#include "uvm_hal.h"

#define MEM_ALLOCATION_SIZE (4 * 1024 * 1024)

static NV_STATUS try_peer_access_remote_gpu_memory(uvm_gpu_t *local_gpu, uvm_gpu_t *peer_gpu)
{
    NV_STATUS status = NV_OK;
    uvm_mem_t *vidmem = NULL;
    uvm_mem_t *sysmem = NULL;
    uvm_push_t push;
    uvm_gpu_address_t local_gpu_sysmem = {0};
    uvm_gpu_address_t peer_gpu_sysmem = {0};
    uvm_gpu_address_t peer_gpu_vidmem = {0};
    void *cpu_va = NULL;
    volatile NvU32 *cpu_array;
    NvU32 i;

    // allocate CPU memory
    status = uvm_mem_alloc_sysmem_and_map_cpu_kernel(MEM_ALLOCATION_SIZE, current->mm, &sysmem);
    TEST_NV_CHECK_GOTO(status, cleanup);

    // get CPU address
    cpu_va = uvm_mem_get_cpu_addr_kernel(sysmem);
    TEST_CHECK_GOTO(cpu_va != 0, cleanup);
    cpu_array = (volatile NvU32 *)cpu_va;

    // map sysmem to both GPUs
    status = uvm_mem_map_gpu_kernel(sysmem, local_gpu);
    TEST_NV_CHECK_GOTO(status, cleanup);

    status = uvm_mem_map_gpu_kernel(sysmem, peer_gpu);
    TEST_NV_CHECK_GOTO(status, cleanup);

     // get local GPU address for the sysmem
    local_gpu_sysmem = uvm_mem_gpu_address_virtual_kernel(sysmem, local_gpu);
    TEST_CHECK_GOTO(local_gpu_sysmem.address != 0, cleanup);

    peer_gpu_sysmem = uvm_mem_gpu_address_virtual_kernel(sysmem, peer_gpu);
    TEST_CHECK_GOTO(peer_gpu_sysmem.address != 0, cleanup);

    // allocate vidmem on remote GPU
    status = uvm_mem_alloc_vidmem(MEM_ALLOCATION_SIZE, peer_gpu, &vidmem);
    TEST_NV_CHECK_GOTO(status, cleanup);
    TEST_CHECK_GOTO(IS_ALIGNED(MEM_ALLOCATION_SIZE, vidmem->chunk_size), cleanup);

    // map onto GPU
    status = uvm_mem_map_gpu_kernel(vidmem, peer_gpu);
    TEST_NV_CHECK_GOTO(status, cleanup);

    // get remote GPU virtual address for its vidmem
    peer_gpu_vidmem = uvm_mem_gpu_address_virtual_kernel(vidmem, peer_gpu);

    // initialize memory using CPU
    for (i = 0; i < MEM_ALLOCATION_SIZE / sizeof(NvU32); i++)
        cpu_array[i] = i;

    // use the peer mapping to copy data from sysmem to peer's vidmem,
    // since the writes are to peer memory, use GPU_TO_GPU type
    status = uvm_push_begin(local_gpu->channel_manager,
                            UVM_CHANNEL_TYPE_GPU_TO_GPU,
                            &push,
                            "peer identity mapping test write to peer");
    TEST_NV_CHECK_GOTO(status, cleanup);

    for (i = 0; i < MEM_ALLOCATION_SIZE / vidmem->chunk_size; i++) {
        uvm_gpu_address_t local_gpu_peer_vidmem = uvm_mem_gpu_address_copy(vidmem,
                                                                           local_gpu,
                                                                           vidmem->chunk_size * i,
                                                                           vidmem->chunk_size);
        uvm_gpu_address_t local_gpu_sysmem_offset = local_gpu_sysmem;
        local_gpu_sysmem_offset.address += vidmem->chunk_size * i;
        local_gpu->parent->ce_hal->memcopy(&push, local_gpu_peer_vidmem, local_gpu_sysmem_offset, vidmem->chunk_size);
    }
    status = uvm_push_end_and_wait(&push);
    TEST_NV_CHECK_GOTO(status, cleanup);

    // set the sysmem back to zero
    memset((void *)cpu_array, 0, MEM_ALLOCATION_SIZE);

    // copy peer vidmem to sysmem
    status = uvm_push_begin(peer_gpu->channel_manager,
                            UVM_CHANNEL_TYPE_GPU_TO_CPU,
                            &push,
                            "peer identity mapping test read local to check");
    TEST_NV_CHECK_GOTO(status, cleanup);

    peer_gpu->parent->ce_hal->memcopy(&push, peer_gpu_sysmem, peer_gpu_vidmem, MEM_ALLOCATION_SIZE);
    status = uvm_push_end_and_wait(&push);
    TEST_NV_CHECK_GOTO(status, cleanup);

    for (i = 0; i < MEM_ALLOCATION_SIZE / sizeof(NvU32); i++) {
        if (cpu_array[i] != i) {
            UVM_TEST_PRINT("Expected %u at offset %u but got %u\n", i, i, cpu_array[i]);
            status = NV_ERR_INVALID_STATE;
        }
    }

cleanup:
    uvm_mem_free(vidmem);
    uvm_mem_free(sysmem);
    return status;
}

NV_STATUS uvm_test_peer_identity_mappings(UVM_TEST_PEER_IDENTITY_MAPPINGS_PARAMS *params, struct file *filp)
{
    NV_STATUS status;
    uvm_gpu_t *gpu_a;
    uvm_gpu_t *gpu_b;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    uvm_va_space_down_read(va_space);
    gpu_a = uvm_va_space_get_gpu_by_uuid(va_space, &params->gpuA);
    gpu_b = uvm_va_space_get_gpu_by_uuid(va_space, &params->gpuB);

    if (gpu_a == NULL || gpu_b == NULL) {
        status = NV_ERR_INVALID_DEVICE;
        goto done;
    }

    if (gpu_a->parent->peer_copy_mode != gpu_b->parent->peer_copy_mode) {
        status = NV_ERR_INVALID_DEVICE;
        goto done;
    }

    if (gpu_a->parent->peer_copy_mode != UVM_GPU_PEER_COPY_MODE_VIRTUAL) {
        status = NV_WARN_NOTHING_TO_DO;
        goto done;
    }

    if (!uvm_processor_mask_test(&va_space->can_access[uvm_id_value(gpu_a->id)], gpu_b->id)) {
        status = NV_ERR_INVALID_DEVICE;
        goto done;
    }

    status = try_peer_access_remote_gpu_memory(gpu_a, gpu_b);
    if (status != NV_OK)
        goto done;

    status = try_peer_access_remote_gpu_memory(gpu_b, gpu_a);
    if (status != NV_OK)
        goto done;
done:
    uvm_va_space_up_read(va_space);
    return status;
}
