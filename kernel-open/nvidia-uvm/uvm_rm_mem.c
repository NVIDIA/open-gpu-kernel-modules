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

#include "uvm_rm_mem.h"
#include "uvm_gpu.h"
#include "uvm_global.h"
#include "uvm_kvmalloc.h"
#include "uvm_linux.h"
#include "nv_uvm_interface.h"

bool uvm_rm_mem_mapped_on_gpu(uvm_rm_mem_t *rm_mem, uvm_gpu_t *gpu)
{
    return uvm_processor_mask_test(&rm_mem->mapped_on, gpu->id);
}

bool uvm_rm_mem_mapped_on_gpu_proxy(uvm_rm_mem_t *rm_mem, uvm_gpu_t *gpu)
{
    if (rm_mem->proxy_vas == NULL)
        return false;

    if (rm_mem->proxy_vas[uvm_id_value(gpu->id)] == 0)
        return false;

    UVM_ASSERT(uvm_rm_mem_mapped_on_gpu(rm_mem, gpu));
    UVM_ASSERT(uvm_parent_gpu_needs_proxy_channel_pool(gpu->parent));

    return true;
}

bool uvm_rm_mem_mapped_on_cpu(uvm_rm_mem_t *rm_mem)
{
    return uvm_processor_mask_test(&rm_mem->mapped_on, UVM_ID_CPU);
}

static void rm_mem_set_gpu_va(uvm_rm_mem_t *rm_mem, uvm_gpu_t *gpu, NvU64 va)
{
    rm_mem->vas[uvm_id_value(gpu->id)] = va;
    uvm_processor_mask_set(&rm_mem->mapped_on, gpu->id);
}

static void rm_mem_set_gpu_proxy_va(uvm_rm_mem_t *rm_mem, uvm_gpu_t *gpu, NvU64 va)
{
    rm_mem->proxy_vas[uvm_id_value(gpu->id)] = va;
}

static void rm_mem_set_cpu_va(uvm_rm_mem_t *rm_mem, void *va)
{
    rm_mem->vas[UVM_ID_CPU_VALUE] = (uintptr_t) va;
    uvm_processor_mask_set(&rm_mem->mapped_on, UVM_ID_CPU);
}

static void rm_mem_clear_gpu_va(uvm_rm_mem_t *rm_mem, uvm_gpu_t *gpu)
{
    UVM_ASSERT(!uvm_rm_mem_mapped_on_gpu_proxy(rm_mem, gpu));

    uvm_processor_mask_clear(&rm_mem->mapped_on, gpu->id);
    rm_mem->vas[uvm_id_value(gpu->id)] = 0;
}

static void rm_mem_clear_gpu_proxy_va(uvm_rm_mem_t *rm_mem, uvm_gpu_t *gpu)
{
    rm_mem->proxy_vas[uvm_id_value(gpu->id)] = 0;
}

static void rm_mem_clear_cpu_va(uvm_rm_mem_t *rm_mem)
{
    uvm_processor_mask_clear(&rm_mem->mapped_on, UVM_ID_CPU);
    rm_mem->vas[UVM_ID_CPU_VALUE] = 0;
}

NvU64 uvm_rm_mem_get_gpu_uvm_va(uvm_rm_mem_t *rm_mem, uvm_gpu_t *gpu)
{
    UVM_ASSERT_MSG(uvm_rm_mem_mapped_on_gpu(rm_mem, gpu), "GPU %s\n", uvm_gpu_name(gpu));

    return rm_mem->vas[uvm_id_value(gpu->id)];
}

NvU64 uvm_rm_mem_get_gpu_proxy_va(uvm_rm_mem_t *rm_mem, uvm_gpu_t *gpu)
{
    UVM_ASSERT(uvm_rm_mem_mapped_on_gpu_proxy(rm_mem, gpu));

    return rm_mem->proxy_vas[uvm_id_value(gpu->id)];
}

uvm_gpu_address_t uvm_rm_mem_get_gpu_va(uvm_rm_mem_t *rm_mem, uvm_gpu_t *gpu, bool is_proxy_va_space)
{
    uvm_gpu_address_t gpu_va = {0};

    gpu_va.aperture = UVM_APERTURE_MAX;
    gpu_va.is_virtual = true;

    if (g_uvm_global.conf_computing_enabled && (rm_mem->type == UVM_RM_MEM_TYPE_SYS))
        gpu_va.is_unprotected = true;

    if (is_proxy_va_space)
        gpu_va.address = uvm_rm_mem_get_gpu_proxy_va(rm_mem, gpu);
    else
        gpu_va.address = uvm_rm_mem_get_gpu_uvm_va(rm_mem, gpu);

    return gpu_va;
}

void *uvm_rm_mem_get_cpu_va(uvm_rm_mem_t *rm_mem)
{
    UVM_ASSERT(uvm_rm_mem_mapped_on_cpu(rm_mem));

    return (void *)(uintptr_t)rm_mem->vas[UVM_ID_CPU_VALUE];
}

static NV_STATUS rm_mem_map_gpu_proxy(uvm_rm_mem_t *rm_mem, uvm_gpu_t *gpu)
{
    NV_STATUS status;
    uvm_gpu_t *gpu_owner;
    NvU64 gpu_owner_va;
    NvU64 proxy_va;

    UVM_ASSERT(uvm_rm_mem_mapped_on_gpu(rm_mem, gpu));

    if (!uvm_parent_gpu_needs_proxy_channel_pool(gpu->parent))
        return NV_OK;

    if (uvm_rm_mem_mapped_on_gpu_proxy(rm_mem, gpu))
        return NV_OK;

    if (rm_mem->proxy_vas == NULL) {
        NvU64 *proxy_vas = uvm_kvmalloc_zero(sizeof(rm_mem->vas));
        if (proxy_vas == NULL)
            return NV_ERR_NO_MEMORY;

        rm_mem->proxy_vas = proxy_vas;
    }

    gpu_owner = rm_mem->gpu_owner;
    gpu_owner_va = uvm_rm_mem_get_gpu_uvm_va(rm_mem, gpu_owner);

    status = uvm_rm_locked_call(nvUvmInterfacePagingChannelsMap(gpu_owner->rm_address_space,
                                                                gpu_owner_va,
                                                                uvm_gpu_device_handle(gpu),
                                                                &proxy_va));
    if (status != NV_OK) {
        UVM_ERR_PRINT("nvUvmInterfacePagingChannelsMap() failed: %s, src GPU %s, dst GPU %s\n",
                      nvstatusToString(status),
                      uvm_gpu_name(gpu_owner),
                      uvm_gpu_name(gpu));
        return status;
    }

    rm_mem_set_gpu_proxy_va(rm_mem, gpu, proxy_va);

    return NV_OK;
}

static void rm_mem_unmap_gpu_proxy(uvm_rm_mem_t *rm_mem, uvm_gpu_t *gpu)
{
    uvm_gpu_t *gpu_owner;
    NvU64 gpu_owner_va;

    if (!uvm_rm_mem_mapped_on_gpu_proxy(rm_mem, gpu))
        return;

    gpu_owner = rm_mem->gpu_owner;
    gpu_owner_va = uvm_rm_mem_get_gpu_uvm_va(rm_mem, gpu_owner);


    uvm_rm_locked_call_void(nvUvmInterfacePagingChannelsUnmap(gpu_owner->rm_address_space,
                                                              gpu_owner_va,
                                                              uvm_gpu_device_handle(gpu)));

    rm_mem_clear_gpu_proxy_va(rm_mem, gpu);
}


NV_STATUS uvm_rm_mem_alloc(uvm_gpu_t *gpu,
                           uvm_rm_mem_type_t type,
                           NvLength size,
                           NvU64 gpu_alignment,
                           uvm_rm_mem_t **rm_mem_out)
{
    NV_STATUS status = NV_OK;
    uvm_rm_mem_t *rm_mem;
    UvmGpuAllocInfo alloc_info = { 0 };
    NvU64 gpu_va;

    UVM_ASSERT(gpu);
    UVM_ASSERT((type == UVM_RM_MEM_TYPE_SYS) || (type == UVM_RM_MEM_TYPE_GPU));
    UVM_ASSERT(size != 0);

    rm_mem = uvm_kvmalloc_zero(sizeof(*rm_mem));
    if (rm_mem == NULL)
        return NV_ERR_NO_MEMORY;

    if (!g_uvm_global.conf_computing_enabled || type == UVM_RM_MEM_TYPE_SYS)
        alloc_info.bUnprotected = NV_TRUE;

    alloc_info.alignment = gpu_alignment;

    if (type == UVM_RM_MEM_TYPE_SYS)
        status = uvm_rm_locked_call(nvUvmInterfaceMemoryAllocSys(gpu->rm_address_space, size, &gpu_va, &alloc_info));
    else
        status = uvm_rm_locked_call(nvUvmInterfaceMemoryAllocFB(gpu->rm_address_space, size, &gpu_va, &alloc_info));

    if (status != NV_OK) {
        UVM_ERR_PRINT("nvUvmInterfaceMemoryAlloc%s() failed: %s, GPU %s\n",
                      type == UVM_RM_MEM_TYPE_SYS ? "Sys" : "FB",
                      nvstatusToString(status),
                      uvm_gpu_name(gpu));
        goto error;
    }

    rm_mem->gpu_owner = gpu;
    rm_mem->type = type;
    rm_mem->size = size;
    rm_mem_set_gpu_va(rm_mem, gpu, gpu_va);

    status = rm_mem_map_gpu_proxy(rm_mem, gpu);
    if (status != NV_OK)
        goto error;

    *rm_mem_out = rm_mem;
    return NV_OK;

error:
    uvm_rm_mem_free(rm_mem);
    return status;
}

NV_STATUS uvm_rm_mem_map_cpu(uvm_rm_mem_t *rm_mem)
{
    NV_STATUS status;
    uvm_gpu_t *gpu;
    NvU64 gpu_va;
    void *cpu_va;

    UVM_ASSERT(rm_mem);

    if (uvm_rm_mem_mapped_on_cpu(rm_mem))
        return NV_OK;

    if (g_uvm_global.conf_computing_enabled)
        UVM_ASSERT(rm_mem->type == UVM_RM_MEM_TYPE_SYS);

    gpu = rm_mem->gpu_owner;
    gpu_va = uvm_rm_mem_get_gpu_uvm_va(rm_mem, gpu);

    status = uvm_rm_locked_call(nvUvmInterfaceMemoryCpuMap(gpu->rm_address_space,
                                                           gpu_va,
                                                           rm_mem->size,
                                                           &cpu_va,
                                                           UVM_PAGE_SIZE_DEFAULT));
    if (status != NV_OK) {
        UVM_ERR_PRINT("nvUvmInterfaceMemoryCpuMap() failed: %s, GPU %s\n", nvstatusToString(status), uvm_gpu_name(gpu));
        return status;
    }

    rm_mem_set_cpu_va(rm_mem, cpu_va);

    return NV_OK;
}

void uvm_rm_mem_unmap_cpu(uvm_rm_mem_t *rm_mem)
{
    UVM_ASSERT(rm_mem);

    if (!uvm_rm_mem_mapped_on_cpu(rm_mem))
        return;

    uvm_rm_locked_call_void(nvUvmInterfaceMemoryCpuUnMap(rm_mem->gpu_owner->rm_address_space,
                                                         uvm_rm_mem_get_cpu_va(rm_mem)));

    rm_mem_clear_cpu_va(rm_mem);
}

NV_STATUS uvm_rm_mem_map_gpu(uvm_rm_mem_t *rm_mem, uvm_gpu_t *gpu, NvU64 gpu_alignment)
{
    NV_STATUS status;
    uvm_gpu_t *gpu_owner;
    NvU64 gpu_owner_va;
    NvU64 gpu_va;

    UVM_ASSERT(rm_mem);
    UVM_ASSERT(gpu);

    if (uvm_rm_mem_mapped_on_gpu(rm_mem, gpu))
        return NV_OK;

    // Peer mappings are not supported yet
    UVM_ASSERT(rm_mem->type == UVM_RM_MEM_TYPE_SYS);

    gpu_owner = rm_mem->gpu_owner;
    gpu_owner_va = uvm_rm_mem_get_gpu_uvm_va(rm_mem, gpu_owner);

    status = uvm_rm_locked_call(nvUvmInterfaceDupAllocation(gpu_owner->rm_address_space,
                                                            gpu_owner_va,
                                                            gpu->rm_address_space,
                                                            gpu_alignment,
                                                            &gpu_va));
    if (status != NV_OK) {
        UVM_ERR_PRINT("nvUvmInterfaceDupAllocation() failed: %s, src GPU %s, dest GPU %s\n",
                      nvstatusToString(status),
                      uvm_gpu_name(gpu_owner),
                      uvm_gpu_name(gpu));
        return status;
    }

    rm_mem_set_gpu_va(rm_mem, gpu, gpu_va);

    // Map to proxy VA space, if applicable
    return rm_mem_map_gpu_proxy(rm_mem, gpu);
}

// This internal unmap variant allows the GPU owner to be unmapped, unlike
// uvm_rm_mem_unmap_gpu
static void rm_mem_unmap_gpu(uvm_rm_mem_t *rm_mem, uvm_gpu_t *gpu)
{
    NvU64 va;

    if (!uvm_rm_mem_mapped_on_gpu(rm_mem, gpu))
        return;

    // Remove mappings in proxy address space, if any
    rm_mem_unmap_gpu_proxy(rm_mem, gpu);

    va = uvm_rm_mem_get_gpu_uvm_va(rm_mem, gpu);
    uvm_rm_locked_call_void(nvUvmInterfaceMemoryFree(gpu->rm_address_space, va));
    rm_mem_clear_gpu_va(rm_mem, gpu);
}

void uvm_rm_mem_unmap_gpu(uvm_rm_mem_t *rm_mem, uvm_gpu_t *gpu)
{
    UVM_ASSERT(rm_mem);
    UVM_ASSERT(gpu);

    // The GPU owner mapping remains valid until the memory is freed.
    if (gpu == rm_mem->gpu_owner)
        return;

    rm_mem_unmap_gpu(rm_mem, gpu);
}

void uvm_rm_mem_free(uvm_rm_mem_t *rm_mem)
{
    uvm_gpu_id_t gpu_id;
    uvm_gpu_t *gpu_owner;

    if (rm_mem == NULL)
        return;

    // If the GPU owner is not set, allocation of backing storage by RM failed
    gpu_owner = rm_mem->gpu_owner;
    if (gpu_owner == NULL) {
        uvm_kvfree(rm_mem);
        return;
    }

    uvm_rm_mem_unmap_cpu(rm_mem);

    // Don't use for_each_gpu_in_mask() as the owning GPU might be being
    // destroyed and already removed from the global GPU array causing the
    // iteration to stop prematurely.
    for_each_gpu_id_in_mask(gpu_id, &rm_mem->mapped_on) {
        if (!uvm_id_equal(gpu_id, gpu_owner->id))
            uvm_rm_mem_unmap_gpu(rm_mem, uvm_gpu_get(gpu_id));
    }

    rm_mem_unmap_gpu(rm_mem, gpu_owner);

    UVM_ASSERT_MSG(uvm_processor_mask_empty(&rm_mem->mapped_on),
                   "Left-over %u mappings in rm_mem\n",
                   uvm_processor_mask_get_count(&rm_mem->mapped_on));

    uvm_kvfree(rm_mem->proxy_vas);
    uvm_kvfree(rm_mem);
}

NV_STATUS uvm_rm_mem_alloc_and_map_cpu(uvm_gpu_t *gpu,
                                       uvm_rm_mem_type_t type,
                                       NvLength size,
                                       NvU64 gpu_alignment,
                                       uvm_rm_mem_t **rm_mem_out)
{
    uvm_rm_mem_t *rm_mem;
    NV_STATUS status;

    status = uvm_rm_mem_alloc(gpu, type, size, gpu_alignment, &rm_mem);
    if (status != NV_OK)
        return status;

    status = uvm_rm_mem_map_cpu(rm_mem);
    if (status != NV_OK)
        goto error;

    *rm_mem_out = rm_mem;

    return NV_OK;

error:
    uvm_rm_mem_free(rm_mem);
    return status;
}

NV_STATUS uvm_rm_mem_map_all_gpus(uvm_rm_mem_t *rm_mem, NvU64 gpu_alignment)
{
    uvm_gpu_t *gpu;

    UVM_ASSERT(rm_mem);

    for_each_gpu(gpu) {
        NV_STATUS status = uvm_rm_mem_map_gpu(rm_mem, gpu, gpu_alignment);
        if (status != NV_OK)
            return status;
    }
    return NV_OK;
}

NV_STATUS uvm_rm_mem_alloc_and_map_all(uvm_gpu_t *gpu,
                                       uvm_rm_mem_type_t type,
                                       NvLength size,
                                       NvU64 gpu_alignment,
                                       uvm_rm_mem_t **rm_mem_out)
{
    uvm_rm_mem_t *rm_mem;
    NV_STATUS status;

    UVM_ASSERT(gpu);

    status = uvm_rm_mem_alloc_and_map_cpu(gpu, type, size, gpu_alignment, &rm_mem);
    if (status != NV_OK)
        return status;

    status = uvm_rm_mem_map_all_gpus(rm_mem, gpu_alignment);
    if (status != NV_OK)
        goto error;

    *rm_mem_out = rm_mem;

    return NV_OK;

error:
    uvm_rm_mem_free(rm_mem);
    return status;
}
