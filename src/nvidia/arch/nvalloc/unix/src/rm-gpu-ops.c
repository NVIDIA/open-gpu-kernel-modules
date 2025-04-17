/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include <nv.h>
#include <os/os.h>
#include <osapi.h>
#include <core/thread_state.h>
#include "rmapi/nv_gpu_ops.h"
#include "gpu/mem_mgr/phys_mem_allocator/phys_mem_allocator.h"

NV_STATUS NV_API_CALL rm_gpu_ops_create_session(
    nvidia_stack_t *sp,
    struct gpuSession **session)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsCreateSession(session);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_destroy_session (
    nvidia_stack_t *sp, gpuSessionHandle session)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsDestroySession(session);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_device_create (
    nvidia_stack_t *sp,
    nvgpuSessionHandle_t session,
    const gpuInfo *pGpuInfo,
    const NvProcessorUuid *gpuUuid,
    nvgpuDeviceHandle_t *device,
    NvBool bCreateSmcPartition)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsDeviceCreate(session, pGpuInfo, gpuUuid, device, bCreateSmcPartition);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_device_destroy (
    nvidia_stack_t *sp,
    gpuDeviceHandle device)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsDeviceDestroy(device);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_address_space_create (
    nvidia_stack_t *sp,
    gpuDeviceHandle device,
    NvU64 vaBase,
    NvU64 vaSize,
    NvU32 enableAts,
    gpuAddressSpaceHandle *vaSpace,
    gpuAddressSpaceInfo *vaSpaceInfo)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsAddressSpaceCreate(device, vaBase, vaSize, enableAts,
                                          vaSpace, vaSpaceInfo);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_dup_address_space(
    nvidia_stack_t *sp,
    gpuDeviceHandle device,
    NvHandle hUserClient,
    NvHandle hUserVASpace,
    gpuAddressSpaceHandle *dupedVaspace,
    gpuAddressSpaceInfo *vaSpaceInfo)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsDupAddressSpace(device, hUserClient, hUserVASpace,
                                       dupedVaspace, vaSpaceInfo);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS NV_API_CALL rm_gpu_ops_address_space_destroy(nvidia_stack_t *sp,
    gpuAddressSpaceHandle vaspace)
{
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    nvGpuOpsAddressSpaceDestroy(vaspace);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return NV_OK;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_memory_alloc_fb(
    nvidia_stack_t *sp, gpuAddressSpaceHandle vaspace,
    NvLength size, NvU64 *gpuOffset, gpuAllocInfo *allocInfo)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsMemoryAllocFb(vaspace, size, gpuOffset, allocInfo);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_get_p2p_caps(nvidia_stack_t *sp,
                                                gpuDeviceHandle device1,
                                                gpuDeviceHandle device2,
                                                getP2PCapsParams *pP2pCapsParams)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsGetP2PCaps(device1, device2, pP2pCapsParams);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_memory_alloc_sys(
    nvidia_stack_t *sp, gpuAddressSpaceHandle vaspace,
    NvLength size, NvU64 *gpuOffset, gpuAllocInfo *allocInfo)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsMemoryAllocSys(vaspace, size, gpuOffset, allocInfo);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_pma_register_callbacks(
    nvidia_stack_t *sp,
    void *pPma,
    pmaEvictPagesCb_t evictPages,
    pmaEvictRangeCb_t evictRange,
    void *callbackData)
{
    THREAD_STATE_NODE threadState;
    NV_STATUS rmStatus;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    rmStatus = pmaRegisterEvictionCb(pPma, evictPages, evictRange, callbackData);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

void  NV_API_CALL  rm_gpu_ops_pma_unregister_callbacks(
    nvidia_stack_t *sp,
    void *pPma)
{
    THREAD_STATE_NODE threadState;
    void *fp;

    NV_ENTER_RM_RUNTIME(sp,fp);
    threadStateInit(&threadState, THREAD_STATE_FLAGS_NONE);

    pmaUnregisterEvictionCb(pPma);

    threadStateFree(&threadState, THREAD_STATE_FLAGS_NONE);
    NV_EXIT_RM_RUNTIME(sp,fp);
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_get_pma_object(
    nvidia_stack_t *sp,
    gpuDeviceHandle device,
    void **pPma,
    const nvgpuPmaStatistics_t *pPmaPubStats)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsGetPmaObject(device, pPma,
                                    (const UvmPmaStatistics **)pPmaPubStats);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_pma_alloc_pages(
    nvidia_stack_t *sp, void *pPma,
    NvLength pageCount, NvU64 pageSize,
    nvgpuPmaAllocationOptions_t pPmaAllocOptions,
    NvU64 *pPages)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsPmaAllocPages(pPma, pageCount, pageSize,
                                     pPmaAllocOptions, pPages);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_pma_pin_pages(
    nvidia_stack_t *sp, void *pPma,
    NvU64 *pPages, NvLength pageCount, NvU64 pageSize, NvU32 flags)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsPmaPinPages(pPma, pPages, pageCount, pageSize, flags);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_memory_cpu_map(
    nvidia_stack_t *sp, gpuAddressSpaceHandle vaspace,
    NvU64 gpuOffset, NvLength length, void **cpuPtr, NvU64 pageSize)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsMemoryCpuMap(vaspace, gpuOffset, length, cpuPtr,
                                    pageSize);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_memory_cpu_ummap(
    nvidia_stack_t *sp, gpuAddressSpaceHandle vaspace, void* cpuPtr)
{
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    nvGpuOpsMemoryCpuUnMap(vaspace, cpuPtr);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return NV_OK;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_tsg_allocate(nvidia_stack_t *sp,
                                                gpuAddressSpaceHandle vaspace,
                                                const gpuTsgAllocParams *allocParams,
                                                gpuTsgHandle *tsg)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsTsgAllocate(vaspace, allocParams, tsg);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS NV_API_CALL rm_gpu_ops_tsg_destroy(nvidia_stack_t * sp,
                                             nvgpuTsgHandle_t tsg)
{
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    nvGpuOpsTsgDestroy(tsg);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return NV_OK;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_channel_allocate(nvidia_stack_t *sp,
                                                    const gpuTsgHandle tsg,
                                                    const gpuChannelAllocParams *allocParams,
                                                    gpuChannelHandle *channel,
                                                    gpuChannelInfo *channelInfo)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsChannelAllocate(tsg, allocParams, channel,
                                       channelInfo);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS NV_API_CALL rm_gpu_ops_channel_destroy(nvidia_stack_t * sp,
                                                 nvgpuChannelHandle_t channel)
{
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    nvGpuOpsChannelDestroy(channel);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return NV_OK;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_pma_free_pages(nvidia_stack_t *sp,
    void *pPma, NvU64 *pPages, NvLength pageCount, NvU64 pageSize, NvU32 flags)
{
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    nvGpuOpsPmaFreePages(pPma, pPages, pageCount, pageSize, flags);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return NV_OK;
}

NV_STATUS  NV_API_CALL rm_gpu_ops_memory_free(
    nvidia_stack_t *sp, gpuAddressSpaceHandle vaspace, NvU64 gpuOffset)
{
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    nvGpuOpsMemoryFree(vaspace, gpuOffset);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return NV_OK;
}

NV_STATUS NV_API_CALL rm_gpu_ops_query_caps(nvidia_stack_t *sp,
                                            gpuDeviceHandle device,
                                            gpuCaps * caps)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsQueryCaps(device, caps);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS NV_API_CALL rm_gpu_ops_query_ces_caps(nvidia_stack_t *sp,
                                                gpuDeviceHandle device,
                                                gpuCesCaps *caps)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsQueryCesCaps(device, caps);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL rm_gpu_ops_get_gpu_info(nvidia_stack_t *sp,
                                               const NvProcessorUuid *pUuid,
                                               const gpuClientInfo *pGpuClientInfo,
                                               gpuInfo *pGpuInfo)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsGetGpuInfo(pUuid, pGpuClientInfo, pGpuInfo);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS NV_API_CALL rm_gpu_ops_service_device_interrupts_rm(nvidia_stack_t *sp,
                                                              gpuDeviceHandle device)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus =  nvGpuOpsServiceDeviceInterruptsRM(device);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_set_page_directory (nvidia_stack_t *sp,
                                         gpuAddressSpaceHandle vaSpace,
                                         NvU64 physAddress, unsigned numEntries,
                                         NvBool bVidMemAperture, NvU32 pasid,
                                         NvU64 *dmaAddress)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsSetPageDirectory(vaSpace, physAddress, numEntries,
                                        bVidMemAperture, pasid, dmaAddress);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_unset_page_directory (nvidia_stack_t *sp,
                                                 gpuAddressSpaceHandle vaSpace)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsUnsetPageDirectory(vaSpace);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_dup_allocation(nvidia_stack_t *sp,
                                                  gpuAddressSpaceHandle srcVaSpace,
                                                  NvU64 srcAddress,
                                                  gpuAddressSpaceHandle dstVaSpace,
                                                  NvU64 dstVaAlignment,
                                                  NvU64 *dstAddress)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsDupAllocation(srcVaSpace, srcAddress, dstVaSpace, dstVaAlignment, dstAddress);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_dup_memory (nvidia_stack_t *sp,
                                               gpuDeviceHandle device,
                                               NvHandle hClient,
                                               NvHandle hPhysMemory,
                                               NvHandle *hDupMemory,
                                               nvgpuMemoryInfo_t gpuMemoryInfo)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsDupMemory(device, hClient, hPhysMemory, hDupMemory, gpuMemoryInfo);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_free_duped_handle (nvidia_stack_t *sp,
                                                gpuDeviceHandle device,
                                                NvHandle hPhysHandle)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsFreeDupedHandle(device, hPhysHandle);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_get_fb_info (nvidia_stack_t *sp,
                                                gpuDeviceHandle device,
                                                gpuFbInfo * fbInfo)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsGetFbInfo(device, fbInfo);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_get_ecc_info (nvidia_stack_t *sp,
                                                 gpuDeviceHandle device,
                                                 gpuEccInfo * eccInfo)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsGetEccInfo(device, eccInfo);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

//
// Please see the comments for nvUvmInterfaceOwnPageFaultIntr(), in
// nv_uvm_interface.h, for the recommended way to use this routine.
//
// How it works:
//
// The rmGpuLocksAcquire call generally saves the current GPU interrupt
// state, then disables interrupt generation for one (or all) GPUs.
// Likewise, the rmGpuLocksRelease call restores (re-enables) those
// interrupts to their previous state. However, the rmGpuLocksRelease
// call does NOT restore interrupts that RM does not own.
//
// This is rather hard to find in the code, so: very approximately, the
// following sequence happens: rmGpuLocksRelease, osEnableInterrupts,
// intrRestoreNonStall_HAL, intrEncodeIntrEn_HAL, and that last one skips
// over any interrupts that RM does not own.
//
// This means that things are a bit asymmetric, because this routine
// actually changes that ownership in between the rmGpuLocksAcquire and
// rmGpuLocksRelease calls. So:
//
// -- If you call this routine with bOwnInterrupts == NV_TRUE (UVM is
//    taking ownership from the RM), then rmGpuLocksAcquire disables all
//    GPU interrupts. Then the ownership is taken away from RM, so the
//    rmGpuLocksRelease call leaves the replayable page fault interrupts
//    disabled. It is then up to UVM (the caller) to enable replayable
//    page fault interrupts when it is ready.
//
// -- If you call this routine with bOwnInterrupts == NV_FALSE (UVM is
//    returning ownership to the RM), then rmGpuLocksAcquire disables
//    all GPU interrupts that RM owns. Then the ownership is returned to
//    RM, so the rmGpuLocksRelease call re-enables replayable page fault
//    interrupts. So, that implies that you need to disable replayable page
//    fault interrupts before calling this routine, in order to hand
//    over a GPU to RM that is not generating interrupts, until RM is
//    ready to handle the interrupts.
//
NV_STATUS NV_API_CALL rm_gpu_ops_own_page_fault_intr(nvidia_stack_t *sp,
                                                     struct gpuDevice *device,
                                                     NvBool bOwnInterrupts)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsOwnPageFaultIntr(device, bOwnInterrupts);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_init_fault_info (nvidia_stack_t *sp,
                                                    gpuDeviceHandle device,
                                                    gpuFaultInfo *pFaultInfo)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsInitFaultInfo(device, pFaultInfo);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_destroy_fault_info (nvidia_stack_t *sp,
                                                       gpuDeviceHandle device,
                                                       gpuFaultInfo *pFaultInfo)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsDestroyFaultInfo(device, pFaultInfo);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

// Functions
//
// - rm_gpu_ops_has_pending_non_replayable_faults
// - rm_gpu_ops_get_non_replayable_faults
//
// Cannot take the GPU/RM lock because it is called during fault servicing.
// This could produce deadlocks if the UVM bottom half gets stuck behind a
// stalling interrupt that cannot be serviced if UVM is holding the lock.
//
// However, these functions can be safely called with no locks because it is
// just accessing the given client shadow fault buffer, which is implemented
// using a lock-free queue. There is a different client shadow fault buffer
// per GPU: RM top-half producer, UVM top/bottom-half consumer.

NV_STATUS  NV_API_CALL  rm_gpu_ops_has_pending_non_replayable_faults(nvidia_stack_t *sp,
                                                                     gpuFaultInfo *pFaultInfo,
                                                                     NvBool *hasPendingFaults)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsHasPendingNonReplayableFaults(pFaultInfo, hasPendingFaults);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_get_non_replayable_faults(nvidia_stack_t *sp,
                                                             gpuFaultInfo *pFaultInfo,
                                                             void *faultBuffer,
                                                             NvU32 *numFaults)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsGetNonReplayableFaults(pFaultInfo, faultBuffer, numFaults);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL rm_gpu_ops_flush_replayable_fault_buffer(nvidia_stack_t *sp,
                                                                gpuFaultInfo *pFaultInfo,
                                                                NvBool bCopyAndFlush)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsFlushReplayableFaultBuffer(pFaultInfo, bCopyAndFlush);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL rm_gpu_ops_toggle_prefetch_faults(nvidia_stack_t *sp,
                                                         gpuFaultInfo *pFaultInfo,
                                                         NvBool bEnable)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsTogglePrefetchFaults(pFaultInfo, bEnable);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_init_access_cntr_info(nvidia_stack_t *sp,
                                                         gpuDeviceHandle device,
                                                         gpuAccessCntrInfo *accessCntrInfo,
                                                         NvU32 accessCntrIndex)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsInitAccessCntrInfo(device, accessCntrInfo, accessCntrIndex);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_destroy_access_cntr_info(nvidia_stack_t *sp,
                                                            gpuDeviceHandle device,
                                                            gpuAccessCntrInfo *accessCntrInfo)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsDestroyAccessCntrInfo(device, accessCntrInfo);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_enable_access_cntr(nvidia_stack_t *sp,
                                                      gpuDeviceHandle device,
                                                      gpuAccessCntrInfo *accessCntrInfo,
                                                      gpuAccessCntrConfig *accessCntrConfig)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsEnableAccessCntr(device, accessCntrInfo, accessCntrConfig);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_disable_access_cntr(nvidia_stack_t *sp,
                                                       gpuDeviceHandle device,
                                                       gpuAccessCntrInfo *accessCntrInfo)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsDisableAccessCntr(device, accessCntrInfo);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL  rm_gpu_ops_get_nvlink_info (nvidia_stack_t *sp,
                                                    gpuDeviceHandle device,
                                                    gpuNvlinkInfo *nvlinkInfo)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsGetNvlinkInfo(device, nvlinkInfo);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS NV_API_CALL
rm_gpu_ops_p2p_object_create(nvidia_stack_t *sp,
                             gpuDeviceHandle device1,
                             gpuDeviceHandle device2,
                             NvHandle *hP2pObject)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp, fp);
    rmStatus = nvGpuOpsP2pObjectCreate(device1, device2, hP2pObject);
    NV_EXIT_RM_RUNTIME(sp, fp);
    return rmStatus;
}

void NV_API_CALL
rm_gpu_ops_p2p_object_destroy(nvidia_stack_t *sp,
                              nvgpuSessionHandle_t session,
                              NvHandle hP2pObject)
{
    void *fp;
    NV_ENTER_RM_RUNTIME(sp, fp);
    nvGpuOpsP2pObjectDestroy(session, hP2pObject);
    NV_EXIT_RM_RUNTIME(sp, fp);
}

NV_STATUS  NV_API_CALL
rm_gpu_ops_get_external_alloc_ptes(nvidia_stack_t* sp,
                                   nvgpuAddressSpaceHandle_t vaSpace,
                                   NvHandle hDupedMemory,
                                   NvU64 offset,
                                   NvU64 size,
                                   nvgpuExternalMappingInfo_t gpuExternalMappingInfo)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp, fp);
    rmStatus = nvGpuOpsGetExternalAllocPtes(vaSpace, hDupedMemory, offset, size,
                                            gpuExternalMappingInfo);
    NV_EXIT_RM_RUNTIME(sp, fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL
rm_gpu_ops_get_external_alloc_phys_addrs(nvidia_stack_t* sp,
                                         nvgpuAddressSpaceHandle_t vaSpace,
                                         NvHandle hDupedMemory,
                                         NvU64 offset,
                                         NvU64 size,
                                         nvgpuExternalPhysAddrInfo_t gpuExternalPhysAddrsInfo)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp, fp);
    rmStatus = nvGpuOpsGetExternalAllocPhysAddrs(vaSpace, hDupedMemory, offset, size,
                                                 gpuExternalPhysAddrsInfo);
    NV_EXIT_RM_RUNTIME(sp, fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL
rm_gpu_ops_retain_channel(nvidia_stack_t* sp,
                          nvgpuAddressSpaceHandle_t vaSpace,
                          NvHandle hClient,
                          NvHandle hChannel,
                          void **retainedChannel,
                          nvgpuChannelInstanceInfo_t channelInstanceInfo)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp, fp);
    rmStatus = nvGpuOpsRetainChannel(vaSpace, hClient, hChannel,
                                     (gpuRetainedChannel **)retainedChannel,
                                     channelInstanceInfo);
    NV_EXIT_RM_RUNTIME(sp, fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL
rm_gpu_ops_bind_channel_resources(nvidia_stack_t* sp,
                                  void *retainedChannel,
                                  nvgpuChannelResourceBindParams_t channelResourceBindParams)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp, fp);
    rmStatus = nvGpuOpsBindChannelResources(retainedChannel,
                                            channelResourceBindParams);
    NV_EXIT_RM_RUNTIME(sp, fp);
    return rmStatus;
}

void NV_API_CALL
rm_gpu_ops_release_channel(nvidia_stack_t *sp, void *retainedChannel)
{
    void *fp;
    NV_ENTER_RM_RUNTIME(sp, fp);
    nvGpuOpsReleaseChannel(retainedChannel);
    NV_EXIT_RM_RUNTIME(sp, fp);
}

void NV_API_CALL
rm_gpu_ops_stop_channel(nvidia_stack_t * sp,
                        void *retainedChannel,
                        NvBool bImmediate)
{
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    nvGpuOpsStopChannel(retainedChannel, bImmediate);
    NV_EXIT_RM_RUNTIME(sp, fp);
}

NV_STATUS  NV_API_CALL
rm_gpu_ops_get_channel_resource_ptes(nvidia_stack_t* sp,
                                     nvgpuAddressSpaceHandle_t vaSpace,
                                     NvP64 resourceDescriptor,
                                     NvU64 offset,
                                     NvU64 size,
                                     nvgpuExternalMappingInfo_t gpuExternalMappingInfo)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp, fp);
    rmStatus = nvGpuOpsGetChannelResourcePtes(vaSpace, resourceDescriptor,
                                              offset, size,
                                              gpuExternalMappingInfo);
    NV_EXIT_RM_RUNTIME(sp, fp);
    return rmStatus;
}

NV_STATUS NV_API_CALL
rm_gpu_ops_report_non_replayable_fault(nvidia_stack_t *sp,
                                       nvgpuDeviceHandle_t device,
                                       const void *pFaultPacket)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsReportNonReplayableFault(device, pFaultPacket);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS NV_API_CALL
rm_gpu_ops_paging_channel_allocate(nvidia_stack_t *sp,
                                   gpuDeviceHandle device,
                                   const gpuPagingChannelAllocParams *allocParams,
                                   gpuPagingChannelHandle *channel,
                                   gpuPagingChannelInfo *channelInfo)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsPagingChannelAllocate(device, allocParams, channel,
                                             channelInfo);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

void NV_API_CALL
rm_gpu_ops_paging_channel_destroy(nvidia_stack_t *sp,
                                  gpuPagingChannelHandle channel)
{
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    nvGpuOpsPagingChannelDestroy(channel);
    NV_EXIT_RM_RUNTIME(sp,fp);
}

NV_STATUS NV_API_CALL
rm_gpu_ops_paging_channels_map(nvidia_stack_t *sp,
                               gpuAddressSpaceHandle srcVaSpace,
                               NvU64 srcAddress,
                               gpuDeviceHandle device,
                               NvU64 *dstAddress)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsPagingChannelsMap(srcVaSpace, srcAddress, device, dstAddress);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

void NV_API_CALL
rm_gpu_ops_paging_channels_unmap(nvidia_stack_t *sp,
                                 gpuAddressSpaceHandle srcVaSpace,
                                 NvU64 srcAddress,
                                 gpuDeviceHandle device)
{
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    nvGpuOpsPagingChannelsUnmap(srcVaSpace, srcAddress, device);
    NV_EXIT_RM_RUNTIME(sp,fp);
}

NV_STATUS NV_API_CALL
rm_gpu_ops_paging_channel_push_stream(nvidia_stack_t *sp,
                                      gpuPagingChannelHandle channel,
                                      char *methodStream,
                                      NvU32 methodStreamSize)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsPagingChannelPushStream(channel, methodStream, methodStreamSize);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

void NV_API_CALL
rm_gpu_ops_report_fatal_error(nvidia_stack_t *sp,
                              NV_STATUS error)
{
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    nvGpuOpsReportFatalError(error);
    NV_EXIT_RM_RUNTIME(sp,fp);
}

NV_STATUS NV_API_CALL rm_gpu_ops_ccsl_context_init(nvidia_stack_t *sp,
                                                   struct ccslContext_t **ctx,
                                                   gpuChannelHandle channel)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsCcslContextInit(ctx, channel);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS NV_API_CALL rm_gpu_ops_ccsl_context_clear(nvidia_stack_t *sp,
                                                    struct ccslContext_t *ctx)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsCcslContextClear(ctx);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS NV_API_CALL rm_gpu_ops_ccsl_rotate_key(nvidia_stack_t *sp,
                                                 UvmCslContext *contextList[],
                                                 NvU32 contextListCount)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsCcslRotateKey(contextList, contextListCount);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS NV_API_CALL rm_gpu_ops_ccsl_rotate_iv(nvidia_stack_t *sp,
                                                struct ccslContext_t *ctx,
                                                NvU8 direction)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsCcslRotateIv(ctx, direction);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS NV_API_CALL rm_gpu_ops_ccsl_encrypt_with_iv(nvidia_stack_t *sp,
                                                      struct ccslContext_t *ctx,
                                                      NvU32 bufferSize,
                                                      NvU8 const *inputBuffer,
                                                      NvU8 *encryptIv,
                                                      NvU8 *outputBuffer,
                                                      NvU8 *authTagData)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsCcslEncryptWithIv(ctx, bufferSize, inputBuffer, encryptIv, outputBuffer, authTagData);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS NV_API_CALL rm_gpu_ops_ccsl_encrypt(nvidia_stack_t *sp,
                                              struct ccslContext_t *ctx,
                                              NvU32 bufferSize,
                                              NvU8 const *inputBuffer,
                                              NvU8 *outputBuffer,
                                              NvU8 *authTagData)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsCcslEncrypt(ctx, bufferSize, inputBuffer, outputBuffer, authTagData);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS NV_API_CALL rm_gpu_ops_ccsl_decrypt(nvidia_stack_t *sp,
                                              struct ccslContext_t *ctx,
                                              NvU32 bufferSize,
                                              NvU8 const *inputBuffer,
                                              NvU8 const *decryptIv,
                                              NvU32 keyRotationId,
                                              NvU8 *outputBuffer,
                                              NvU8 const *addAuthData,
                                              NvU32 addAuthDataSize,
                                              NvU8 const *authTagData)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsCcslDecrypt(ctx, bufferSize, inputBuffer, decryptIv, keyRotationId, outputBuffer,
                                   addAuthData, addAuthDataSize, authTagData);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS NV_API_CALL rm_gpu_ops_ccsl_sign(nvidia_stack_t *sp,
                                           struct ccslContext_t *ctx,
                                           NvU32 bufferSize,
                                           NvU8 const *inputBuffer,
                                           NvU8 *authTagData)

{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsCcslSign(ctx, bufferSize, inputBuffer, authTagData);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL rm_gpu_ops_ccsl_query_message_pool(nvidia_stack_t *sp,
                                                          struct ccslContext_t *ctx,
                                                          NvU8 direction,
                                                          NvU64 *messageNum)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsQueryMessagePool(ctx, direction, messageNum);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL rm_gpu_ops_ccsl_increment_iv(nvidia_stack_t *sp,
                                                    struct ccslContext_t *ctx,
                                                    NvU8 direction,
                                                    NvU64 increment,
                                                    NvU8 *iv)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsIncrementIv(ctx, direction, increment, iv);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}

NV_STATUS  NV_API_CALL rm_gpu_ops_ccsl_log_encryption(nvidia_stack_t *sp,
                                                      struct ccslContext_t *ctx,
                                                      NvU8 direction,
                                                      NvU32 bufferSize)
{
    NV_STATUS rmStatus;
    void *fp;
    NV_ENTER_RM_RUNTIME(sp,fp);
    rmStatus = nvGpuOpsLogEncryption(ctx, direction, bufferSize);
    NV_EXIT_RM_RUNTIME(sp,fp);
    return rmStatus;
}
