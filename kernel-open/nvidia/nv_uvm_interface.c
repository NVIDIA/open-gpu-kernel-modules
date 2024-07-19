/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

/*
 * This file sets up the communication between the UVM driver and RM. RM will
 * call the UVM driver providing to it the set of OPS it supports.  UVM will
 * then return by filling out the structure with the callbacks it supports.
 */

#define  __NO_VERSION__

#include "os-interface.h"
#include "nv-linux.h"

#if defined(NV_UVM_ENABLE)

#include "nv_uvm_interface.h"
#include "nv_gpu_ops.h"
#include "rm-gpu-ops.h"

// This is really a struct UvmOpsUvmEvents *. It needs to be an atomic because
// it can be read outside of the g_pNvUvmEventsLock. Use getUvmEvents and
// setUvmEvents to access it.
static atomic_long_t g_pNvUvmEvents;
static struct semaphore g_pNvUvmEventsLock;

static struct UvmOpsUvmEvents *getUvmEvents(void)
{
    return (struct UvmOpsUvmEvents *)atomic_long_read(&g_pNvUvmEvents);
}

static void setUvmEvents(struct UvmOpsUvmEvents *newEvents)
{
    atomic_long_set(&g_pNvUvmEvents, (long)newEvents);
}

static nvidia_stack_t *g_sp;
static struct semaphore g_spLock;

// Use these to test g_sp usage. When DEBUG_GLOBAL_STACK, one out of every
// DEBUG_GLOBAL_STACK_THRESHOLD calls to nvUvmGetSafeStack will use g_sp.
#define DEBUG_GLOBAL_STACK 0
#define DEBUG_GLOBAL_STACK_THRESHOLD 2

static atomic_t g_debugGlobalStackCount = ATOMIC_INIT(0);

// Called at module load, not by an external client
int nv_uvm_init(void)
{
    int rc = nv_kmem_cache_alloc_stack(&g_sp);
    if (rc != 0)
        return rc;

    NV_INIT_MUTEX(&g_spLock);
    NV_INIT_MUTEX(&g_pNvUvmEventsLock);
    return 0;
}

void nv_uvm_exit(void)
{
    // If this fires, the dependent driver never unregistered its callbacks with
    // us before going away, leaving us potentially making callbacks to garbage
    // memory.
    WARN_ON(getUvmEvents() != NULL);

    nv_kmem_cache_free_stack(g_sp);
}


// Testing code to force use of the global stack every now and then
static NvBool forceGlobalStack(void)
{
    // Make sure that we do not try to allocate memory in interrupt or atomic
    // context
    if (DEBUG_GLOBAL_STACK || !NV_MAY_SLEEP())
    {
        if ((atomic_inc_return(&g_debugGlobalStackCount) %
             DEBUG_GLOBAL_STACK_THRESHOLD) == 0)
            return NV_TRUE;
    }
    return NV_FALSE;
}

// Guaranteed to always return a valid stack. It first attempts to allocate one
// from the pool. If that fails, it falls back to the global pre-allocated
// stack. This fallback will serialize.
//
// This is required so paths that free resources do not themselves require
// allocation of resources.
static nvidia_stack_t *nvUvmGetSafeStack(void)
{
    nvidia_stack_t *sp;
    if (forceGlobalStack() || nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        sp = g_sp;
        down(&g_spLock);
    }
    return sp;
}

static void nvUvmFreeSafeStack(nvidia_stack_t *sp)
{
    if (sp == g_sp)
        up(&g_spLock);
    else
        nv_kmem_cache_free_stack(sp);
}

static NV_STATUS nvUvmDestroyFaultInfoAndStacks(nvidia_stack_t *sp,
                                                uvmGpuDeviceHandle device,
                                                UvmGpuFaultInfo *pFaultInfo)
{
    nv_kmem_cache_free_stack(pFaultInfo->replayable.cslCtx.nvidia_stack);
    nv_kmem_cache_free_stack(pFaultInfo->nonReplayable.isr_bh_sp);
    nv_kmem_cache_free_stack(pFaultInfo->nonReplayable.isr_sp);

    return rm_gpu_ops_destroy_fault_info(sp,
                                         (gpuDeviceHandle)device,
                                         pFaultInfo);
}

NV_STATUS nvUvmInterfaceRegisterGpu(const NvProcessorUuid *gpuUuid, UvmGpuPlatformInfo *gpuInfo)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;
    int rc;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
        return NV_ERR_NO_MEMORY;

    rc = nvidia_dev_get_uuid(gpuUuid->uuid, sp);
    if (rc == 0)
    {
        rc = nvidia_dev_get_pci_info(gpuUuid->uuid,
                                     &gpuInfo->pci_dev,
                                     &gpuInfo->dma_addressable_start,
                                     &gpuInfo->dma_addressable_limit);

        // Block GPU from entering GC6 while used by UVM.
        if (rc == 0)
            rc = nvidia_dev_block_gc6(gpuUuid->uuid, sp);

        // Avoid leaking reference on GPU if we failed.
        if (rc != 0)
            nvidia_dev_put_uuid(gpuUuid->uuid, sp);
    }

    switch (rc)
    {
        case 0:
            status = NV_OK;
            break;
        case -ENOMEM:
            status = NV_ERR_NO_MEMORY;
            break;
        case -ENODEV:
            status = NV_ERR_GPU_UUID_NOT_FOUND;
            break;
        default:
            status = NV_ERR_GENERIC;
            break;
    }

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceRegisterGpu);

void nvUvmInterfaceUnregisterGpu(const NvProcessorUuid *gpuUuid)
{
    nvidia_stack_t *sp = nvUvmGetSafeStack();
    nvidia_dev_unblock_gc6(gpuUuid->uuid, sp);
    nvidia_dev_put_uuid(gpuUuid->uuid, sp);
    nvUvmFreeSafeStack(sp);
}
EXPORT_SYMBOL(nvUvmInterfaceUnregisterGpu);

NV_STATUS nvUvmInterfaceSessionCreate(uvmGpuSessionHandle *session,
                                      UvmPlatformInfo *platformInfo)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    memset(platformInfo, 0, sizeof(*platformInfo));
    platformInfo->atsSupported = nv_ats_supported;

    platformInfo->confComputingEnabled = os_cc_enabled;

    status = rm_gpu_ops_create_session(sp, (gpuSessionHandle *)session);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceSessionCreate);

NV_STATUS nvUvmInterfaceSessionDestroy(uvmGpuSessionHandle session)
{
    nvidia_stack_t *sp = nvUvmGetSafeStack();
    NV_STATUS status;

    status = rm_gpu_ops_destroy_session(sp, (gpuSessionHandle)session);

    nvUvmFreeSafeStack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceSessionDestroy);

NV_STATUS nvUvmInterfaceDeviceCreate(uvmGpuSessionHandle session,
                                     const UvmGpuInfo *pGpuInfo,
                                     const NvProcessorUuid *gpuUuid,
                                     uvmGpuDeviceHandle *device,
                                     NvBool bCreateSmcPartition)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_device_create(sp,
                                      (gpuSessionHandle)session,
                                      (const gpuInfo *)pGpuInfo,
                                      gpuUuid,
                                      (gpuDeviceHandle *)device,
                                      bCreateSmcPartition);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceDeviceCreate);

void nvUvmInterfaceDeviceDestroy(uvmGpuDeviceHandle device)
{
    nvidia_stack_t *sp = nvUvmGetSafeStack();

    rm_gpu_ops_device_destroy(sp, (gpuDeviceHandle)device);

    nvUvmFreeSafeStack(sp);
}
EXPORT_SYMBOL(nvUvmInterfaceDeviceDestroy);

NV_STATUS nvUvmInterfaceDupAddressSpace(uvmGpuDeviceHandle device,
                                        NvHandle hUserClient,
                                        NvHandle hUserVASpace,
                                        uvmGpuAddressSpaceHandle *vaSpace,
                                        UvmGpuAddressSpaceInfo *vaSpaceInfo)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_dup_address_space(sp,
                                          (gpuDeviceHandle)device,
                                          hUserClient,
                                          hUserVASpace,
                                          (gpuAddressSpaceHandle *)vaSpace,
                                          vaSpaceInfo);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceDupAddressSpace);

NV_STATUS nvUvmInterfaceAddressSpaceCreate(uvmGpuDeviceHandle device,
                                           unsigned long long vaBase,
                                           unsigned long long vaSize,
                                           NvBool enableAts,
                                           uvmGpuAddressSpaceHandle *vaSpace,
                                           UvmGpuAddressSpaceInfo *vaSpaceInfo)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_address_space_create(sp,
                                             (gpuDeviceHandle)device,
                                             vaBase,
                                             vaSize,
                                             enableAts,
                                             (gpuAddressSpaceHandle *)vaSpace,
                                             vaSpaceInfo);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceAddressSpaceCreate);

void nvUvmInterfaceAddressSpaceDestroy(uvmGpuAddressSpaceHandle vaSpace)
{
    nvidia_stack_t *sp = nvUvmGetSafeStack();

    rm_gpu_ops_address_space_destroy(
        sp, (gpuAddressSpaceHandle)vaSpace);

    nvUvmFreeSafeStack(sp);
}
EXPORT_SYMBOL(nvUvmInterfaceAddressSpaceDestroy);

NV_STATUS nvUvmInterfaceMemoryAllocFB(uvmGpuAddressSpaceHandle vaSpace,
                    NvLength length, UvmGpuPointer * gpuPointer,
                    UvmGpuAllocInfo * allocInfo)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_memory_alloc_fb(
             sp, (gpuAddressSpaceHandle)vaSpace,
             length, (NvU64 *) gpuPointer,
             allocInfo);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceMemoryAllocFB);

NV_STATUS nvUvmInterfaceMemoryAllocSys(uvmGpuAddressSpaceHandle vaSpace,
                    NvLength length, UvmGpuPointer * gpuPointer,
                    UvmGpuAllocInfo * allocInfo)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_memory_alloc_sys(
             sp, (gpuAddressSpaceHandle)vaSpace,
             length, (NvU64 *) gpuPointer,
             allocInfo);

    nv_kmem_cache_free_stack(sp);
    return status;
}

EXPORT_SYMBOL(nvUvmInterfaceMemoryAllocSys);

NV_STATUS nvUvmInterfaceGetP2PCaps(uvmGpuDeviceHandle device1,
                                   uvmGpuDeviceHandle device2,
                                   UvmGpuP2PCapsParams * p2pCapsParams)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_get_p2p_caps(sp,
                                     (gpuDeviceHandle)device1,
                                     (gpuDeviceHandle)device2,
                                     p2pCapsParams);
    nv_kmem_cache_free_stack(sp);
    return status;
}

EXPORT_SYMBOL(nvUvmInterfaceGetP2PCaps);

NV_STATUS nvUvmInterfaceGetPmaObject(uvmGpuDeviceHandle device,
                                     void **pPma,
                                     const UvmPmaStatistics **pPmaPubStats)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_get_pma_object(sp, (gpuDeviceHandle)device, pPma, (const nvgpuPmaStatistics_t *)pPmaPubStats);

    nv_kmem_cache_free_stack(sp);
    return status;
}

EXPORT_SYMBOL(nvUvmInterfaceGetPmaObject);

NV_STATUS nvUvmInterfacePmaRegisterEvictionCallbacks(void *pPma,
                                                     uvmPmaEvictPagesCallback evictPages,
                                                     uvmPmaEvictRangeCallback evictRange,
                                                     void *callbackData)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_pma_register_callbacks(sp, pPma, evictPages, evictRange, callbackData);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfacePmaRegisterEvictionCallbacks);

void nvUvmInterfacePmaUnregisterEvictionCallbacks(void *pPma)
{
    nvidia_stack_t *sp = nvUvmGetSafeStack();

    rm_gpu_ops_pma_unregister_callbacks(sp, pPma);

    nvUvmFreeSafeStack(sp);
}
EXPORT_SYMBOL(nvUvmInterfacePmaUnregisterEvictionCallbacks);

NV_STATUS nvUvmInterfacePmaAllocPages(void *pPma,
                                      NvLength pageCount,
                                      NvU64 pageSize,
                                      UvmPmaAllocationOptions *pPmaAllocOptions,
                                      NvU64 *pPages)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_pma_alloc_pages(
             sp, pPma,
             pageCount,
             pageSize,
             (nvgpuPmaAllocationOptions_t)pPmaAllocOptions,
             pPages);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfacePmaAllocPages);

NV_STATUS nvUvmInterfacePmaPinPages(void *pPma,
                                    NvU64 *pPages,
                                    NvLength pageCount,
                                    NvU64 pageSize,
                                    NvU32 flags)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_pma_pin_pages(sp, pPma, pPages, pageCount, pageSize, flags);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfacePmaPinPages);

void nvUvmInterfaceMemoryFree(uvmGpuAddressSpaceHandle vaSpace,
                    UvmGpuPointer gpuPointer)
{
    nvidia_stack_t *sp = nvUvmGetSafeStack();

    rm_gpu_ops_memory_free(
    sp, (gpuAddressSpaceHandle)vaSpace,
    (NvU64) gpuPointer);

    nvUvmFreeSafeStack(sp);
}
EXPORT_SYMBOL(nvUvmInterfaceMemoryFree);

void nvUvmInterfacePmaFreePages(void *pPma,
                                NvU64 *pPages,
                                NvLength pageCount,
                                NvU64 pageSize,
                                NvU32 flags)
{
    nvidia_stack_t *sp = nvUvmGetSafeStack();

    rm_gpu_ops_pma_free_pages(sp, pPma, pPages, pageCount, pageSize, flags);

    nvUvmFreeSafeStack(sp);
}
EXPORT_SYMBOL(nvUvmInterfacePmaFreePages);

NV_STATUS nvUvmInterfaceMemoryCpuMap(uvmGpuAddressSpaceHandle vaSpace,
           UvmGpuPointer gpuPointer, NvLength length, void **cpuPtr,
           NvU64 pageSize)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_memory_cpu_map(
             sp, (gpuAddressSpaceHandle)vaSpace,
             (NvU64) gpuPointer, length, cpuPtr, pageSize);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceMemoryCpuMap);

void nvUvmInterfaceMemoryCpuUnMap(uvmGpuAddressSpaceHandle vaSpace,
                                  void *cpuPtr)
{
    nvidia_stack_t *sp = nvUvmGetSafeStack();
    rm_gpu_ops_memory_cpu_ummap(sp, (gpuAddressSpaceHandle)vaSpace, cpuPtr);
    nvUvmFreeSafeStack(sp);
}
EXPORT_SYMBOL(nvUvmInterfaceMemoryCpuUnMap);

NV_STATUS nvUvmInterfaceTsgAllocate(uvmGpuAddressSpaceHandle vaSpace,
                                    const UvmGpuTsgAllocParams *allocParams,
                                    uvmGpuTsgHandle *tsg)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_tsg_allocate(sp,
                                     (gpuAddressSpaceHandle)vaSpace,
                                     allocParams,
                                     (gpuTsgHandle *)tsg);

    nv_kmem_cache_free_stack(sp);

    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceTsgAllocate);

void nvUvmInterfaceTsgDestroy(uvmGpuTsgHandle tsg)
{
    nvidia_stack_t *sp = nvUvmGetSafeStack();
    rm_gpu_ops_tsg_destroy(sp, (gpuTsgHandle)tsg);
    nvUvmFreeSafeStack(sp);
}
EXPORT_SYMBOL(nvUvmInterfaceTsgDestroy);


NV_STATUS nvUvmInterfaceChannelAllocate(const uvmGpuTsgHandle tsg,
                                        const UvmGpuChannelAllocParams *allocParams,
                                        uvmGpuChannelHandle *channel,
                                        UvmGpuChannelInfo *channelInfo)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_channel_allocate(sp,
                                         (gpuTsgHandle)tsg,
                                         allocParams,
                                         (gpuChannelHandle *)channel,
                                         channelInfo);

    nv_kmem_cache_free_stack(sp);

    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceChannelAllocate);

void nvUvmInterfaceChannelDestroy(uvmGpuChannelHandle channel)
{
    nvidia_stack_t *sp = nvUvmGetSafeStack();
    rm_gpu_ops_channel_destroy(sp, (gpuChannelHandle)channel);
    nvUvmFreeSafeStack(sp);
}
EXPORT_SYMBOL(nvUvmInterfaceChannelDestroy);

NV_STATUS nvUvmInterfaceQueryCaps(uvmGpuDeviceHandle device,
                                  UvmGpuCaps * caps)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_query_caps(sp, (gpuDeviceHandle)device, caps);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceQueryCaps);

NV_STATUS nvUvmInterfaceQueryCopyEnginesCaps(uvmGpuDeviceHandle device,
                                             UvmGpuCopyEnginesCaps *caps)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_query_ces_caps(sp, (gpuDeviceHandle)device, caps);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceQueryCopyEnginesCaps);

NV_STATUS nvUvmInterfaceGetGpuInfo(const NvProcessorUuid *gpuUuid,
                                   const UvmGpuClientInfo *pGpuClientInfo,
                                   UvmGpuInfo *pGpuInfo)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_get_gpu_info(sp, gpuUuid, pGpuClientInfo, pGpuInfo);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceGetGpuInfo);

NV_STATUS nvUvmInterfaceServiceDeviceInterruptsRM(uvmGpuDeviceHandle device)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_service_device_interrupts_rm(sp,
                                                    (gpuDeviceHandle)device);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceServiceDeviceInterruptsRM);

NV_STATUS nvUvmInterfaceSetPageDirectory(uvmGpuAddressSpaceHandle vaSpace,
                                         NvU64 physAddress, unsigned numEntries,
                                         NvBool bVidMemAperture, NvU32 pasid)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_set_page_directory(sp, (gpuAddressSpaceHandle)vaSpace,
                                    physAddress, numEntries, bVidMemAperture, pasid);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceSetPageDirectory);

NV_STATUS nvUvmInterfaceUnsetPageDirectory(uvmGpuAddressSpaceHandle vaSpace)
{
    nvidia_stack_t *sp = nvUvmGetSafeStack();
    NV_STATUS status;

    status =
           rm_gpu_ops_unset_page_directory(sp, (gpuAddressSpaceHandle)vaSpace);
    nvUvmFreeSafeStack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceUnsetPageDirectory);

NV_STATUS nvUvmInterfaceDupAllocation(uvmGpuAddressSpaceHandle srcVaSpace,
                                      NvU64 srcAddress,
                                      uvmGpuAddressSpaceHandle dstVaSpace,
                                      NvU64 dstVaAlignment,
                                      NvU64 *dstAddress)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_dup_allocation(sp,
                                      (gpuAddressSpaceHandle)srcVaSpace,
                                      srcAddress,
                                      (gpuAddressSpaceHandle)dstVaSpace,
                                      dstVaAlignment,
                                      dstAddress);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceDupAllocation);

NV_STATUS nvUvmInterfaceDupMemory(uvmGpuDeviceHandle device,
                                  NvHandle hClient,
                                  NvHandle hPhysMemory,
                                  NvHandle *hDupMemory,
                                  UvmGpuMemoryInfo *pGpuMemoryInfo)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_dup_memory(sp,
                                   (gpuDeviceHandle)device,
                                   hClient,
                                   hPhysMemory,
                                   hDupMemory,
                                   pGpuMemoryInfo);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceDupMemory);


NV_STATUS nvUvmInterfaceFreeDupedHandle(uvmGpuDeviceHandle device,
                                        NvHandle hPhysHandle)
{
    nvidia_stack_t *sp = nvUvmGetSafeStack();
    NV_STATUS status;

    status = rm_gpu_ops_free_duped_handle(sp,
                                         (gpuDeviceHandle)device,
                                         hPhysHandle);

    nvUvmFreeSafeStack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceFreeDupedHandle);

NV_STATUS nvUvmInterfaceGetFbInfo(uvmGpuDeviceHandle device,
                                  UvmGpuFbInfo * fbInfo)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_get_fb_info(sp, (gpuDeviceHandle)device, fbInfo);

    nv_kmem_cache_free_stack(sp);

    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceGetFbInfo);

NV_STATUS nvUvmInterfaceGetEccInfo(uvmGpuDeviceHandle device,
                                   UvmGpuEccInfo * eccInfo)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_get_ecc_info(sp, (gpuDeviceHandle)device, eccInfo);

    nv_kmem_cache_free_stack(sp);

    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceGetEccInfo);

NV_STATUS nvUvmInterfaceOwnPageFaultIntr(uvmGpuDeviceHandle device, NvBool bOwnInterrupts)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_own_page_fault_intr(sp, (gpuDeviceHandle)device, bOwnInterrupts);
    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceOwnPageFaultIntr);


NV_STATUS nvUvmInterfaceInitFaultInfo(uvmGpuDeviceHandle device,
                                      UvmGpuFaultInfo *pFaultInfo)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;
    int err;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_init_fault_info(sp,
                                       (gpuDeviceHandle)device,
                                       pFaultInfo);
    if (status != NV_OK)
    {
        goto done;
    }

    // Preallocate a stack for functions called from ISR top half
    pFaultInfo->nonReplayable.isr_sp = NULL;
    pFaultInfo->nonReplayable.isr_bh_sp = NULL;
    pFaultInfo->replayable.cslCtx.nvidia_stack = NULL;

    // NOTE: nv_kmem_cache_alloc_stack does not allocate a stack on PPC.
    // Therefore, the pointer can be NULL on success. Always use the
    // returned error code to determine if the operation was successful.
    err = nv_kmem_cache_alloc_stack((nvidia_stack_t **)&pFaultInfo->nonReplayable.isr_sp);
    if (err)
    {
        goto error;
    }

    err = nv_kmem_cache_alloc_stack((nvidia_stack_t **)&pFaultInfo->nonReplayable.isr_bh_sp);
    if (err)
    {
        goto error;
    }

    // The cslCtx.ctx pointer is not NULL only when ConfidentialComputing is enabled.
    if (pFaultInfo->replayable.cslCtx.ctx != NULL)
    {
        err = nv_kmem_cache_alloc_stack((nvidia_stack_t **)&pFaultInfo->replayable.cslCtx.nvidia_stack);
        if (err)
        {
            goto error;
        }
    }
    goto done;

error:
    nvUvmDestroyFaultInfoAndStacks(sp,
                                   device,
                                   pFaultInfo);
    status = NV_ERR_NO_MEMORY;
done:
    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceInitFaultInfo);

NV_STATUS nvUvmInterfaceInitAccessCntrInfo(uvmGpuDeviceHandle device,
                                           UvmGpuAccessCntrInfo *pAccessCntrInfo,
                                           NvU32 accessCntrIndex)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_init_access_cntr_info(sp,
                                              (gpuDeviceHandle)device,
                                              pAccessCntrInfo,
                                              accessCntrIndex);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceInitAccessCntrInfo);

NV_STATUS nvUvmInterfaceEnableAccessCntr(uvmGpuDeviceHandle device,
                                         UvmGpuAccessCntrInfo *pAccessCntrInfo,
                                         UvmGpuAccessCntrConfig *pAccessCntrConfig)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_enable_access_cntr (sp,
                                            (gpuDeviceHandle)device,
                                            pAccessCntrInfo,
                                            pAccessCntrConfig);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceEnableAccessCntr);

NV_STATUS nvUvmInterfaceDestroyFaultInfo(uvmGpuDeviceHandle device,
                                         UvmGpuFaultInfo *pFaultInfo)
{
    nvidia_stack_t *sp = nvUvmGetSafeStack();
    NV_STATUS status;

    status = nvUvmDestroyFaultInfoAndStacks(sp,
                                            device,
                                            pFaultInfo);
    nvUvmFreeSafeStack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceDestroyFaultInfo);

NV_STATUS nvUvmInterfaceHasPendingNonReplayableFaults(UvmGpuFaultInfo *pFaultInfo,
                                                      NvBool *hasPendingFaults)
{
    return rm_gpu_ops_has_pending_non_replayable_faults(pFaultInfo->nonReplayable.isr_sp,
                                                        pFaultInfo,
                                                        hasPendingFaults);
}
EXPORT_SYMBOL(nvUvmInterfaceHasPendingNonReplayableFaults);

NV_STATUS nvUvmInterfaceGetNonReplayableFaults(UvmGpuFaultInfo *pFaultInfo,
                                               void *pFaultBuffer,
                                               NvU32 *numFaults)
{
    return rm_gpu_ops_get_non_replayable_faults(pFaultInfo->nonReplayable.isr_bh_sp,
                                                pFaultInfo,
                                                pFaultBuffer,
                                                numFaults);
}
EXPORT_SYMBOL(nvUvmInterfaceGetNonReplayableFaults);

NV_STATUS nvUvmInterfaceFlushReplayableFaultBuffer(UvmGpuFaultInfo *pFaultInfo,
                                                   NvBool bCopyAndFlush)
{
    nvidia_stack_t *sp = nvUvmGetSafeStack();
    NV_STATUS status;

    status = rm_gpu_ops_flush_replayable_fault_buffer(sp,
                                                      pFaultInfo,
                                                      bCopyAndFlush);

    nvUvmFreeSafeStack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceFlushReplayableFaultBuffer);

NV_STATUS nvUvmInterfaceTogglePrefetchFaults(UvmGpuFaultInfo *pFaultInfo,
                                             NvBool bEnable)
{
    nvidia_stack_t *sp = nvUvmGetSafeStack();
    NV_STATUS status;

    status = rm_gpu_ops_toggle_prefetch_faults(sp,
                                               pFaultInfo,
                                               bEnable);

    nvUvmFreeSafeStack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceTogglePrefetchFaults);

NV_STATUS nvUvmInterfaceDestroyAccessCntrInfo(uvmGpuDeviceHandle device,
                                              UvmGpuAccessCntrInfo *pAccessCntrInfo)
{
    nvidia_stack_t *sp = nvUvmGetSafeStack();
    NV_STATUS status;

    status = rm_gpu_ops_destroy_access_cntr_info(sp,
                                                 (gpuDeviceHandle)device,
                                                 pAccessCntrInfo);

    nvUvmFreeSafeStack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceDestroyAccessCntrInfo);

NV_STATUS nvUvmInterfaceDisableAccessCntr(uvmGpuDeviceHandle device,
                                          UvmGpuAccessCntrInfo *pAccessCntrInfo)
{
    nvidia_stack_t *sp = nvUvmGetSafeStack();
    NV_STATUS status;

    status = rm_gpu_ops_disable_access_cntr(sp,
                                            (gpuDeviceHandle)device,
                                            pAccessCntrInfo);

    nvUvmFreeSafeStack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceDisableAccessCntr);

// this function is called by the UVM driver to register the ops
NV_STATUS nvUvmInterfaceRegisterUvmCallbacks(struct UvmOpsUvmEvents *importedUvmOps)
{
    NV_STATUS status = NV_OK;

    if (!importedUvmOps)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    down(&g_pNvUvmEventsLock);
    if (getUvmEvents() != NULL)
    {
        status = NV_ERR_IN_USE;
    }
    else
    {
        // Be careful: as soon as the pointer is assigned, top half ISRs can
        // start reading it to make callbacks, even before we drop the lock.
        setUvmEvents(importedUvmOps);
    }
    up(&g_pNvUvmEventsLock);

    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceRegisterUvmCallbacks);

static void flush_top_half(void *info)
{
    // Prior top halves on this core must have completed for this callback to
    // run at all, so we're done.
    return;
}

void nvUvmInterfaceDeRegisterUvmOps(void)
{
    // Taking the lock forces us to wait for non-interrupt callbacks to finish
    // up.
    down(&g_pNvUvmEventsLock);
    setUvmEvents(NULL);
    up(&g_pNvUvmEventsLock);

    // We cleared the pointer so nv_uvm_event_interrupt can't invoke any new
    // top half callbacks, but prior ones could still be executing on other
    // cores. We can wait for them to finish by waiting for a context switch to
    // happen on every core.
    //
    // This is slow, but since nvUvmInterfaceDeRegisterUvmOps is very rare
    // (module unload) it beats having the top half synchronize with a spin lock
    // every time.
    //
    // Note that since we dropped the lock, another set of callbacks could have
    // already been registered. That's ok, since we just need to wait for old
    // ones to finish.
    on_each_cpu(flush_top_half, NULL, 1);
}
EXPORT_SYMBOL(nvUvmInterfaceDeRegisterUvmOps);

NV_STATUS nv_uvm_suspend(void)
{
    NV_STATUS status = NV_OK;
    struct UvmOpsUvmEvents *events;

    // Synchronize callbacks with unregistration
    down(&g_pNvUvmEventsLock);

    // It's not strictly necessary to use a cached local copy of the events
    // pointer here since it can't change under the lock, but we'll do it for
    // consistency.
    events = getUvmEvents();
    if (events && events->suspend)
    {
        status = events->suspend();
    }

    up(&g_pNvUvmEventsLock);

    return status;
}

NV_STATUS nv_uvm_resume(void)
{
    NV_STATUS status = NV_OK;
    struct UvmOpsUvmEvents *events;

    // Synchronize callbacks with unregistration
    down(&g_pNvUvmEventsLock);

    // It's not strictly necessary to use a cached local copy of the events
    // pointer here since it can't change under the lock, but we'll do it for
    // consistency.
    events = getUvmEvents();
    if (events && events->resume)
    {
        status = events->resume();
    }

    up(&g_pNvUvmEventsLock);

    return status;
}

void nv_uvm_notify_start_device(const NvU8 *pUuid)
{
    NvProcessorUuid uvmUuid;
    struct UvmOpsUvmEvents *events;

    memcpy(uvmUuid.uuid, pUuid, UVM_UUID_LEN);

    // Synchronize callbacks with unregistration
    down(&g_pNvUvmEventsLock);

    // It's not strictly necessary to use a cached local copy of the events
    // pointer here since it can't change under the lock, but we'll do it for
    // consistency.
    events = getUvmEvents();
    if(events && events->startDevice)
    {
        events->startDevice(&uvmUuid);
    }
    up(&g_pNvUvmEventsLock);
}

void nv_uvm_notify_stop_device(const NvU8 *pUuid)
{
    NvProcessorUuid uvmUuid;
    struct UvmOpsUvmEvents *events;

    memcpy(uvmUuid.uuid, pUuid, UVM_UUID_LEN);

    // Synchronize callbacks with unregistration
    down(&g_pNvUvmEventsLock);

    // It's not strictly necessary to use a cached local copy of the events
    // pointer here since it can't change under the lock, but we'll do it for
    // consistency.
    events = getUvmEvents();
    if(events && events->stopDevice)
    {
        events->stopDevice(&uvmUuid);
    }
    up(&g_pNvUvmEventsLock);
}

NV_STATUS nv_uvm_event_interrupt(const NvU8 *pUuid)
{
    //
    // This is called from interrupt context, so we can't take
    // g_pNvUvmEventsLock to prevent the callbacks from being unregistered. Even
    // if we could take the lock, we don't want to slow down the ISR more than
    // absolutely necessary.
    //
    // Instead, we allow this function to be called concurrently with
    // nvUvmInterfaceDeRegisterUvmOps. That function will clear the events
    // pointer, then wait for all top halves to finish out. This means the
    // pointer may change out from under us, but the callbacks are still safe to
    // invoke while we're in this function.
    //
    // This requires that we read the pointer exactly once here so neither we
    // nor the compiler make assumptions about the pointer remaining valid while
    // in this function.
    //
    struct UvmOpsUvmEvents *events = getUvmEvents();

    if (events && events->isrTopHalf)
        return events->isrTopHalf((const NvProcessorUuid *)pUuid);

    //
    // NV_OK means that the interrupt was for the UVM driver, so use
    // NV_ERR_NO_INTR_PENDING to tell the caller that we didn't do anything.
    //
    return NV_ERR_NO_INTR_PENDING;
}

NV_STATUS nvUvmInterfaceP2pObjectCreate(uvmGpuDeviceHandle device1,
                                        uvmGpuDeviceHandle device2,
                                        NvHandle *hP2pObject)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;
    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_p2p_object_create(sp,
                                          (gpuDeviceHandle)device1,
                                          (gpuDeviceHandle)device2,
                                          hP2pObject);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceP2pObjectCreate);

void nvUvmInterfaceP2pObjectDestroy(uvmGpuSessionHandle session,
                                         NvHandle hP2pObject)
{
    nvidia_stack_t *sp = nvUvmGetSafeStack();

    rm_gpu_ops_p2p_object_destroy(sp, (gpuSessionHandle)session, hP2pObject);

    nvUvmFreeSafeStack(sp);
}
EXPORT_SYMBOL(nvUvmInterfaceP2pObjectDestroy);

NV_STATUS nvUvmInterfaceGetExternalAllocPtes(uvmGpuAddressSpaceHandle vaSpace,
                                             NvHandle hDupedMemory,
                                             NvU64 offset,
                                             NvU64 size,
                                             UvmGpuExternalMappingInfo *gpuExternalMappingInfo)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_get_external_alloc_ptes(sp,
                                                (gpuAddressSpaceHandle)vaSpace,
                                                hDupedMemory,
                                                offset,
                                                size,
                                                gpuExternalMappingInfo);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceGetExternalAllocPtes);

NV_STATUS nvUvmInterfaceRetainChannel(uvmGpuAddressSpaceHandle vaSpace,
                                      NvHandle hClient,
                                      NvHandle hChannel,
                                      void **retainedChannel,
                                      UvmGpuChannelInstanceInfo *channelInstanceInfo)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_retain_channel(sp,
                                       (gpuAddressSpaceHandle)vaSpace,
                                       hClient,
                                       hChannel,
                                       retainedChannel,
                                       channelInstanceInfo);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceRetainChannel);

NV_STATUS nvUvmInterfaceBindChannelResources(void *retainedChannel,
                                             UvmGpuChannelResourceBindParams *channelResourceBindParams)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_bind_channel_resources(sp,
                                               retainedChannel,
                                               channelResourceBindParams);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceBindChannelResources);

void nvUvmInterfaceReleaseChannel(void *retainedChannel)
{
    nvidia_stack_t *sp = nvUvmGetSafeStack();

    rm_gpu_ops_release_channel(sp, retainedChannel);

    nvUvmFreeSafeStack(sp);
}
EXPORT_SYMBOL(nvUvmInterfaceReleaseChannel);

void nvUvmInterfaceStopChannel(void *retainedChannel, NvBool bImmediate)
{
    nvidia_stack_t *sp = nvUvmGetSafeStack();

    rm_gpu_ops_stop_channel(sp, retainedChannel, bImmediate);

    nvUvmFreeSafeStack(sp);
}
EXPORT_SYMBOL(nvUvmInterfaceStopChannel);

NV_STATUS nvUvmInterfaceGetChannelResourcePtes(uvmGpuAddressSpaceHandle vaSpace,
                                               NvP64 resourceDescriptor,
                                               NvU64 offset,
                                               NvU64 size,
                                               UvmGpuExternalMappingInfo *externalMappingInfo)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_get_channel_resource_ptes(sp,
                                                  (gpuAddressSpaceHandle)vaSpace,
                                                  resourceDescriptor,
                                                  offset,
                                                  size,
                                                  externalMappingInfo);

    nv_kmem_cache_free_stack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceGetChannelResourcePtes);

NV_STATUS nvUvmInterfaceReportNonReplayableFault(uvmGpuDeviceHandle device,
                                                 const void *pFaultPacket)
{
    nvidia_stack_t *sp = nvUvmGetSafeStack();
    NV_STATUS status;

    status = rm_gpu_ops_report_non_replayable_fault(sp, (gpuDeviceHandle)device, pFaultPacket);

    nvUvmFreeSafeStack(sp);
    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceReportNonReplayableFault);

NV_STATUS nvUvmInterfacePagingChannelAllocate(uvmGpuDeviceHandle device,
                                              const UvmGpuPagingChannelAllocParams *allocParams,
                                              UvmGpuPagingChannelHandle *channel,
                                              UvmGpuPagingChannelInfo *channelInfo)
{
    nvidia_stack_t *sp = NULL;
    nvidia_stack_t *pushStreamSp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
        return NV_ERR_NO_MEMORY;

    if (nv_kmem_cache_alloc_stack(&pushStreamSp) != 0)
    {
        nv_kmem_cache_free_stack(sp);
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_paging_channel_allocate(sp,
                                                (gpuDeviceHandle)device,
                                                allocParams,
                                                (gpuPagingChannelHandle *)channel,
                                                channelInfo);

    if (status == NV_OK)
        (*channel)->pushStreamSp = pushStreamSp;
    else
        nv_kmem_cache_free_stack(pushStreamSp);

    nv_kmem_cache_free_stack(sp);

    return status;
}
EXPORT_SYMBOL(nvUvmInterfacePagingChannelAllocate);

void nvUvmInterfacePagingChannelDestroy(UvmGpuPagingChannelHandle channel)
{
    nvidia_stack_t *sp;

    if (channel == NULL)
        return;

    sp = nvUvmGetSafeStack();
    nv_kmem_cache_free_stack(channel->pushStreamSp);
    rm_gpu_ops_paging_channel_destroy(sp, (gpuPagingChannelHandle)channel);
    nvUvmFreeSafeStack(sp);
}
EXPORT_SYMBOL(nvUvmInterfacePagingChannelDestroy);

NV_STATUS nvUvmInterfacePagingChannelsMap(uvmGpuAddressSpaceHandle srcVaSpace,
                                          UvmGpuPointer srcAddress,
                                          uvmGpuDeviceHandle device,
                                          NvU64 *dstAddress)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
        return NV_ERR_NO_MEMORY;

    status = rm_gpu_ops_paging_channels_map(sp,
                                            (gpuAddressSpaceHandle)srcVaSpace,
                                            (NvU64)srcAddress,
                                            (gpuDeviceHandle)device,
                                            dstAddress);

    nv_kmem_cache_free_stack(sp);

    return status;
}
EXPORT_SYMBOL(nvUvmInterfacePagingChannelsMap);

void nvUvmInterfacePagingChannelsUnmap(uvmGpuAddressSpaceHandle srcVaSpace,
                                       UvmGpuPointer srcAddress,
                                       uvmGpuDeviceHandle device)
{
    nvidia_stack_t *sp = nvUvmGetSafeStack();
    rm_gpu_ops_paging_channels_unmap(sp,
                                     (gpuAddressSpaceHandle)srcVaSpace,
                                     (NvU64)srcAddress,
                                     (gpuDeviceHandle)device);
    nvUvmFreeSafeStack(sp);
}
EXPORT_SYMBOL(nvUvmInterfacePagingChannelsUnmap);

NV_STATUS nvUvmInterfacePagingChannelPushStream(UvmGpuPagingChannelHandle channel,
                                                char *methodStream,
                                                NvU32 methodStreamSize)
{
    return rm_gpu_ops_paging_channel_push_stream(channel->pushStreamSp,
                                                 (gpuPagingChannelHandle)channel,
                                                 methodStream,
                                                 methodStreamSize);
}
EXPORT_SYMBOL(nvUvmInterfacePagingChannelPushStream);

NV_STATUS nvUvmInterfaceCslInitContext(UvmCslContext *uvmCslContext,
                                       uvmGpuChannelHandle channel)
{
    nvidia_stack_t *sp = NULL;
    NV_STATUS status;

    if (nv_kmem_cache_alloc_stack(&sp) != 0)
    {
        return NV_ERR_NO_MEMORY;
    }

    status = rm_gpu_ops_ccsl_context_init(sp, &uvmCslContext->ctx, (gpuChannelHandle)channel);

    // Saving the stack in the context allows UVM to safely use the CSL layer
    // in interrupt context without making new allocations. UVM serializes CSL
    // API usage for a given context so the stack pointer does not need
    // additional protection.
    if (status != NV_OK)
    {
        nv_kmem_cache_free_stack(sp);
    }
    else
    {
        uvmCslContext->nvidia_stack = sp;
    }

    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceCslInitContext);

void nvUvmInterfaceDeinitCslContext(UvmCslContext *uvmCslContext)
{
    nvidia_stack_t *sp = uvmCslContext->nvidia_stack;
    rm_gpu_ops_ccsl_context_clear(sp, uvmCslContext->ctx);
    nvUvmFreeSafeStack(sp);
}
EXPORT_SYMBOL(nvUvmInterfaceDeinitCslContext);

NV_STATUS nvUvmInterfaceCslRotateKey(UvmCslContext *contextList[],
                                     NvU32 contextListCount)
{
    NV_STATUS status;
    nvidia_stack_t *sp;

    if ((contextList == NULL) || (contextListCount == 0) || (contextList[0] == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    sp = contextList[0]->nvidia_stack;
    status = rm_gpu_ops_ccsl_rotate_key(sp, contextList, contextListCount);

    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceCslRotateKey);

NV_STATUS nvUvmInterfaceCslRotateIv(UvmCslContext *uvmCslContext,
                                    UvmCslOperation operation)
{
    NV_STATUS status;
    nvidia_stack_t *sp = uvmCslContext->nvidia_stack;

    status = rm_gpu_ops_ccsl_rotate_iv(sp, uvmCslContext->ctx, operation);

    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceCslRotateIv);

NV_STATUS nvUvmInterfaceCslEncrypt(UvmCslContext *uvmCslContext,
                                   NvU32 bufferSize,
                                   NvU8 const *inputBuffer,
                                   UvmCslIv *encryptIv,
                                   NvU8 *outputBuffer,
                                   NvU8 *authTagBuffer)
{
    NV_STATUS status;
    nvidia_stack_t *sp = uvmCslContext->nvidia_stack;

    if (encryptIv != NULL)
        status = rm_gpu_ops_ccsl_encrypt_with_iv(sp, uvmCslContext->ctx, bufferSize, inputBuffer, (NvU8*)encryptIv, outputBuffer, authTagBuffer);
    else
        status = rm_gpu_ops_ccsl_encrypt(sp, uvmCslContext->ctx, bufferSize, inputBuffer, outputBuffer, authTagBuffer);

    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceCslEncrypt);

NV_STATUS nvUvmInterfaceCslDecrypt(UvmCslContext *uvmCslContext,
                                   NvU32 bufferSize,
                                   NvU8 const *inputBuffer,
                                   UvmCslIv const *decryptIv,
                                   NvU32 keyRotationId,
                                   NvU8 *outputBuffer,
                                   NvU8 const *addAuthData,
                                   NvU32 addAuthDataSize,
                                   NvU8 const *authTagBuffer)
{
    NV_STATUS status;
    nvidia_stack_t *sp = uvmCslContext->nvidia_stack;

    status = rm_gpu_ops_ccsl_decrypt(sp,
                                     uvmCslContext->ctx,
                                     bufferSize,
                                     inputBuffer,
                                     (NvU8 *)decryptIv,
                                     keyRotationId,
                                     outputBuffer,
                                     addAuthData,
                                     addAuthDataSize,
                                     authTagBuffer);

    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceCslDecrypt);

NV_STATUS nvUvmInterfaceCslSign(UvmCslContext *uvmCslContext,
                                NvU32 bufferSize,
                                NvU8 const *inputBuffer,
                                NvU8 *authTagBuffer)
{
    NV_STATUS status;
    nvidia_stack_t *sp = uvmCslContext->nvidia_stack;

    status = rm_gpu_ops_ccsl_sign(sp, uvmCslContext->ctx, bufferSize, inputBuffer, authTagBuffer);

    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceCslSign);

NV_STATUS nvUvmInterfaceCslQueryMessagePool(UvmCslContext *uvmCslContext,
                                            UvmCslOperation operation,
                                            NvU64 *messageNum)
{
    NV_STATUS status;
    nvidia_stack_t *sp = uvmCslContext->nvidia_stack;

    status = rm_gpu_ops_ccsl_query_message_pool(sp, uvmCslContext->ctx, operation, messageNum);

    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceCslQueryMessagePool);

NV_STATUS nvUvmInterfaceCslIncrementIv(UvmCslContext *uvmCslContext,
                                       UvmCslOperation operation,
                                       NvU64 increment,
                                       UvmCslIv *iv)
{
    NV_STATUS status;
    nvidia_stack_t *sp = uvmCslContext->nvidia_stack;

    status = rm_gpu_ops_ccsl_increment_iv(sp, uvmCslContext->ctx, operation, increment, (NvU8 *)iv);

    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceCslIncrementIv);

NV_STATUS nvUvmInterfaceCslLogEncryption(UvmCslContext *uvmCslContext,
                                         UvmCslOperation operation,
                                         NvU32 bufferSize)
{
    NV_STATUS status;
    nvidia_stack_t *sp = uvmCslContext->nvidia_stack;

    status = rm_gpu_ops_ccsl_log_encryption(sp, uvmCslContext->ctx, operation, bufferSize);

    return status;
}
EXPORT_SYMBOL(nvUvmInterfaceCslLogEncryption);

#else // NV_UVM_ENABLE

NV_STATUS nv_uvm_suspend(void)
{
    return NV_OK;
}

NV_STATUS nv_uvm_resume(void)
{
    return NV_OK;
}

#endif // NV_UVM_ENABLE
