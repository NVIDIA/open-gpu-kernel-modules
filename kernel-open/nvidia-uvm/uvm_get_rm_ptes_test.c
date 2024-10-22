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
#include "uvm_test_ioctl.h"
#include "uvm_global.h"
#include "uvm_gpu.h"
#include "uvm_hal.h"
#include "uvm_va_space.h"
#include "uvm_mmu.h"
#include "nv_uvm_types.h"
#include "nv_uvm_interface.h"
#include "uvm_common.h"

#define get_rm_ptes(offset, size, ext_map_info)          \
        uvm_rm_locked_call(                              \
        nvUvmInterfaceGetExternalAllocPtes(gpu_va_space, \
                                           duped_memory, \
                                           offset,       \
                                           size,         \
                                           ext_map_info))

static uvm_aperture_t get_aperture(uvm_va_space_t *va_space,
                                   uvm_gpu_t *memory_owning_gpu,
                                   uvm_gpu_t *memory_mapping_gpu,
                                   UvmGpuMemoryInfo *memory_info,
                                   bool sli_supported)
{
    if (memory_info->sysmem) {
        return UVM_APERTURE_SYS;
    }
    else {
        if (memory_mapping_gpu != memory_owning_gpu && !sli_supported)
            return uvm_gpu_peer_aperture(memory_mapping_gpu, memory_owning_gpu);
        return UVM_APERTURE_VID;
    }
}

static bool is_cacheable(UvmGpuExternalMappingInfo *ext_mapping_info, uvm_aperture_t aperture)
{
    if (ext_mapping_info->cachingType == UvmRmGpuCachingTypeForceCached)
        return true;
    else if (ext_mapping_info->cachingType == UvmRmGpuCachingTypeForceUncached)
        return false;
    else if (aperture == UVM_APERTURE_VID)
        return true;

    return false;
}

static NvU32 get_protection(UvmGpuExternalMappingInfo *ext_mapping_info)
{
    if (ext_mapping_info->mappingType == UvmRmGpuMappingTypeReadWriteAtomic ||
        ext_mapping_info->mappingType == UvmRmGpuMappingTypeDefault)
        return UVM_PROT_READ_WRITE_ATOMIC;
    else if (ext_mapping_info->mappingType == UvmRmGpuMappingTypeReadWrite)
        return UVM_PROT_READ_WRITE;
    else
        return UVM_PROT_READ_ONLY;
}

static NV_STATUS verify_mapping_info(uvm_va_space_t *va_space,
                                     uvm_gpu_t *memory_mapping_gpu,
                                     NvU64 mapping_offset,
                                     NvU64 mapping_size,
                                     UvmGpuExternalMappingInfo *ext_mapping_info,
                                     UvmGpuMemoryInfo *memory_info,
                                     bool sli_supported)
{
    NvU32 index = 0, total_pte_count = 0, skip = 0, page_size = 0;
    uvm_aperture_t aperture = 0;
    NvU32 prot;
    NvU64 phys_offset, pte;
    uvm_mmu_mode_hal_t *hal;
    NvU64 pte_flags = UVM_MMU_PTE_FLAGS_ACCESS_COUNTERS_DISABLED;
    uvm_gpu_t *memory_owning_gpu = NULL;

    TEST_CHECK_RET(memory_info->contig);

    hal = uvm_gpu_va_space_get(va_space, memory_mapping_gpu)->page_tables.hal;

    page_size = memory_info->pageSize;

    // Verify that make_pte supports this page size
    TEST_CHECK_RET(page_size & hal->page_sizes());

    total_pte_count = mapping_size ? (mapping_size / page_size) : (memory_info->size / page_size);

    TEST_CHECK_RET(total_pte_count);

    TEST_CHECK_RET(ext_mapping_info->numWrittenPtes <= total_pte_count);

    TEST_CHECK_RET(ext_mapping_info->numRemainingPtes == (total_pte_count - ext_mapping_info->numWrittenPtes));

    skip = ext_mapping_info->pteSize / sizeof(NvU64);

    TEST_CHECK_RET(skip);

    memory_owning_gpu = uvm_va_space_get_gpu_by_mem_info(va_space, memory_info);
    if (!memory_owning_gpu)
        return NV_ERR_INVALID_DEVICE;

    aperture = get_aperture(va_space, memory_owning_gpu, memory_mapping_gpu, memory_info, sli_supported);

    if (is_cacheable(ext_mapping_info, aperture))
        pte_flags |= UVM_MMU_PTE_FLAGS_CACHED;

    prot = get_protection(ext_mapping_info);

    phys_offset = mapping_offset;

    // Add the physical offset for nvswitch connected peer mappings
    if (uvm_aperture_is_peer(aperture) &&
        uvm_parent_gpus_are_nvswitch_connected(memory_mapping_gpu->parent, memory_owning_gpu->parent))
        phys_offset += memory_owning_gpu->parent->nvswitch_info.fabric_memory_window_start;

    for (index = 0; index < ext_mapping_info->numWrittenPtes; index++) {

        pte = hal->make_pte(aperture,
                            memory_info->physAddr + phys_offset,
                            prot,
                            pte_flags);

        TEST_CHECK_RET(pte == ext_mapping_info->pteBuffer[index * skip]);

        phys_offset += page_size;
    }

    return NV_OK;
}

static NV_STATUS test_get_rm_ptes_single_gpu(uvm_va_space_t *va_space, UVM_TEST_GET_RM_PTES_PARAMS *params)
{
    NV_STATUS status = NV_OK;
    NV_STATUS free_status;
    uvm_gpu_t *memory_mapping_gpu;
    NvHandle duped_memory;
    UvmGpuExternalMappingInfo ext_mapping_info;
    UvmGpuMemoryInfo memory_info;
    NvU64 pte_buffer[16] = {0};
    NvU32 size = 0;
    uvmGpuAddressSpaceHandle gpu_va_space;
    uvmGpuDeviceHandle rm_device;
    NvHandle client, memory;

    client = params->hClient;
    memory = params->hMemory;

    // Note: This check is safe as single GPU test does not run on SLI enabled
    // devices.
    memory_mapping_gpu = uvm_va_space_get_gpu_by_uuid_with_gpu_va_space(va_space, &params->gpu_uuid);
    if (!memory_mapping_gpu)
        return NV_ERR_INVALID_DEVICE;

    gpu_va_space = memory_mapping_gpu->rm_address_space;
    rm_device = uvm_gpu_device_handle(memory_mapping_gpu);

    status = uvm_rm_locked_call(nvUvmInterfaceDupMemory(rm_device, client, memory, &duped_memory, &memory_info));
    if (status != NV_OK)
        return status;

    TEST_CHECK_GOTO(uvm_uuid_eq(&memory_info.uuid, &params->gpu_uuid), done);

    TEST_CHECK_GOTO((memory_info.size == params->size), done);

    size = params->size;

    memset(&ext_mapping_info, 0, sizeof(ext_mapping_info));

    ext_mapping_info.pteBuffer = pte_buffer;

    ext_mapping_info.pteBufferSize = 1;

    TEST_CHECK_GOTO((get_rm_ptes(size + 1, 0, &ext_mapping_info) == NV_ERR_INVALID_BASE), done);

    TEST_CHECK_GOTO((get_rm_ptes(0, size + 1, &ext_mapping_info) == NV_ERR_INVALID_LIMIT), done);

    TEST_CHECK_GOTO((get_rm_ptes(1, 0, &ext_mapping_info) == NV_ERR_INVALID_ARGUMENT), done);

    TEST_CHECK_GOTO((get_rm_ptes(0, size - 1, &ext_mapping_info) == NV_ERR_INVALID_ARGUMENT), done);

    TEST_CHECK_GOTO((get_rm_ptes(0, 0, &ext_mapping_info) == NV_ERR_BUFFER_TOO_SMALL), done);

    ext_mapping_info.pteBufferSize = sizeof(pte_buffer);
    TEST_CHECK_GOTO(get_rm_ptes(0, 0, &ext_mapping_info) == NV_OK, done);
    TEST_CHECK_GOTO(verify_mapping_info(va_space,
                                        memory_mapping_gpu,
                                        0,
                                        0,
                                        &ext_mapping_info,
                                        &memory_info,
                                        false) == NV_OK, done);

    TEST_CHECK_GOTO(get_rm_ptes(memory_info.pageSize, 0, &ext_mapping_info) == NV_OK, done);
    TEST_CHECK_GOTO(verify_mapping_info(va_space,
                                        memory_mapping_gpu,
                                        memory_info.pageSize,
                                        0,
                                        &ext_mapping_info,
                                        &memory_info,
                                        false) == NV_OK, done);

    TEST_CHECK_GOTO(get_rm_ptes(0, size - memory_info.pageSize, &ext_mapping_info) == NV_OK, done);
    TEST_CHECK_GOTO(verify_mapping_info(va_space,
                                        memory_mapping_gpu,
                                        0,
                                        size - memory_info.pageSize,
                                        &ext_mapping_info,
                                        &memory_info,
                                        false) == NV_OK, done);

    ext_mapping_info.mappingType = UvmRmGpuMappingTypeReadWrite;
    ext_mapping_info.cachingType = UvmRmGpuCachingTypeForceCached;
    TEST_CHECK_GOTO(get_rm_ptes(memory_info.pageSize, size - memory_info.pageSize, &ext_mapping_info) == NV_OK, done);
    TEST_CHECK_GOTO(verify_mapping_info(va_space,
                                        memory_mapping_gpu,
                                        memory_info.pageSize,
                                        size - memory_info.pageSize,
                                        &ext_mapping_info,
                                        &memory_info,
                                        false) == NV_OK, done);

    ext_mapping_info.mappingType = UvmRmGpuMappingTypeReadOnly;
    ext_mapping_info.cachingType = UvmRmGpuCachingTypeForceUncached;
    TEST_CHECK_GOTO(get_rm_ptes(size - memory_info.pageSize, memory_info.pageSize, &ext_mapping_info) == NV_OK, done);
    TEST_CHECK_GOTO(verify_mapping_info(va_space,
                                        memory_mapping_gpu,
                                        size - memory_info.pageSize,
                                        memory_info.pageSize,
                                        &ext_mapping_info,
                                        &memory_info,
                                        false) == NV_OK, done);

done:
    free_status = uvm_rm_locked_call(nvUvmInterfaceFreeDupedHandle(rm_device, duped_memory));
    if (status == NV_OK)
        status = free_status;

    return status;
}

static NV_STATUS test_get_rm_ptes_multi_gpu(uvm_va_space_t *va_space, UVM_TEST_GET_RM_PTES_PARAMS *params)
{
    NV_STATUS status = NV_OK;
    NV_STATUS free_status;
    uvm_gpu_t *memory_mapping_gpu;
    NvHandle duped_memory;
    UvmGpuExternalMappingInfo ext_mapping_info;
    UvmGpuMemoryInfo memory_info;
    uvmGpuDeviceHandle rm_device;
    NvU64 pte_buffer[16] = {0};
    uvmGpuAddressSpaceHandle gpu_va_space;

   memory_mapping_gpu = uvm_va_space_get_gpu_by_uuid(va_space, &params->gpu_uuid);
   if (!memory_mapping_gpu)
       return NV_ERR_INVALID_DEVICE;

   gpu_va_space = memory_mapping_gpu->rm_address_space;
   rm_device = uvm_gpu_device_handle(memory_mapping_gpu);

   status = uvm_rm_locked_call(nvUvmInterfaceDupMemory(rm_device,
                                                       params->hClient,
                                                       params->hMemory,
                                                       &duped_memory,
                                                       &memory_info));
   if (status != NV_OK)
       return status;

    memset(&ext_mapping_info, 0, sizeof(ext_mapping_info));

    memset(pte_buffer, 0, sizeof(pte_buffer));

    ext_mapping_info.pteBuffer = pte_buffer;

    ext_mapping_info.pteBufferSize = sizeof(pte_buffer);

    switch (params->test_mode) {
        case UVM_TEST_GET_RM_PTES_MULTI_GPU_SUPPORTED:
        case UVM_TEST_GET_RM_PTES_MULTI_GPU_SLI_SUPPORTED:
            TEST_CHECK_GOTO(get_rm_ptes(0, 0, &ext_mapping_info) == NV_OK, done);
            TEST_CHECK_GOTO(verify_mapping_info(va_space,
                                                memory_mapping_gpu,
                                                0,
                                                0,
                                                &ext_mapping_info,
                                                &memory_info,
                                                (params->test_mode ==
                                                        UVM_TEST_GET_RM_PTES_MULTI_GPU_SLI_SUPPORTED)) == NV_OK, done);
            break;

        case UVM_TEST_GET_RM_PTES_MULTI_GPU_NOT_SUPPORTED:
            TEST_CHECK_GOTO(get_rm_ptes(0, 0, &ext_mapping_info) == NV_ERR_NOT_SUPPORTED, done);
            break;

        default:
            status = NV_ERR_INVALID_PARAMETER;
    }

done:
    free_status = uvm_rm_locked_call(nvUvmInterfaceFreeDupedHandle(rm_device, duped_memory));
    if (status == NV_OK)
        status = free_status;

    return status;
}

NV_STATUS uvm_test_get_rm_ptes(UVM_TEST_GET_RM_PTES_PARAMS *params, struct file *filp)
{
    NV_STATUS status;
    uvm_va_space_t *va_space = uvm_va_space_get(filp);

    uvm_va_space_down_read_rm(va_space);

    switch (params->test_mode) {
        case UVM_TEST_GET_RM_PTES_SINGLE_GPU:
            status = test_get_rm_ptes_single_gpu(va_space, params);
            break;

        case UVM_TEST_GET_RM_PTES_MULTI_GPU_SUPPORTED:
        case UVM_TEST_GET_RM_PTES_MULTI_GPU_SLI_SUPPORTED:
        case UVM_TEST_GET_RM_PTES_MULTI_GPU_NOT_SUPPORTED:
            status = test_get_rm_ptes_multi_gpu(va_space, params);
            break;

        default:
            status = NV_ERR_INVALID_PARAMETER;
    }
    uvm_va_space_up_read_rm(va_space);

    return status;
}
