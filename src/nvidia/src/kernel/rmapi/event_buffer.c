/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "rmapi/event_buffer.h"
#include "os/os.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "core/locks.h"
#include "gpu/gpu.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "rmapi/rs_utils.h"
#include "rmapi/rmapi_utils.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "rmapi/rs_utils.h"
#include "rmapi/rmapi_utils.h"
#include "kernel/gpu/gr/fecs_event_list.h"
#include "gpu/bus/kern_bus.h"
#include "mem_mgr/no_device_mem.h"
#include "class/cl90ce.h"
#include "class/cl0040.h"
#include "gpu/gsp/gsp_trace_rats_macro.h"

static NV_STATUS _allocAndMapMemory(CALL_CONTEXT *pCallContext, NvP64 pAddress, MEMORY_DESCRIPTOR** ppMemDesc, NvU64 size, NvBool bKernel,
    NvP64* pKernelAddr, NvP64* pKernelPriv, NvP64* pUserAddr, NvP64* pUserPriv, Subdevice *pSubdevice);

static void _unmapAndFreeMemory(MEMORY_DESCRIPTOR *pMemDesc, NvBool bKernel, NvP64 kernelAddr,
    NvP64 kernelPriv, NvP64 userAddr, NvP64 userPriv);

NV_STATUS
eventbufferConstruct_IMPL
(
    EventBuffer                  *pEventBuffer,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS                         status;
    NV_EVENT_BUFFER_ALLOC_PARAMETERS *pAllocParams   = pParams->pAllocParams;

    EVENT_BUFFER_MAP_INFO            *pKernelMap     = &pEventBuffer->kernelMapInfo;
    EVENT_BUFFER_MAP_INFO            *pClientMap     = &pEventBuffer->clientMapInfo;

    RmClient                         *pRmClient;
    NvBool                            bKernel;

    NvU32                             recordBufferSize;
    NvP64                             kernelNotificationhandle;
    Subdevice                        *pSubdevice     = NULL;
    NvBool                            bInternalAlloc = (pAllocParams->hBufferHeader == 0);
    NvBool                            bNoDeviceMem   = NV_FALSE;
    NvBool                            bUsingVgpuStagingBuffer = NV_FALSE;
    OBJGPU                           *pGpu           = NULL;
    RsResourceRef                    *pHeaderRef     = NULL;
    RsResourceRef                    *pRecordRef     = NULL;
    RsResourceRef                    *pVardataRef    = NULL;
    NvHandle                          hMapperClient  = 0;
    NvHandle                          hMapperDevice  = 0;

    pRmClient = dynamicCast(pCallContext->pClient, RmClient);
    NV_ASSERT_OR_RETURN(pRmClient != NULL, NV_ERR_INVALID_CLIENT);

    bKernel = (rmclientGetCachedPrivilege(pRmClient) >= RS_PRIV_LEVEL_KERNEL);

    pAllocParams->bufferHeader  = NvP64_NULL;
    pAllocParams->recordBuffer  = NvP64_NULL;
    pAllocParams->vardataBuffer = NvP64_NULL;

    if (bInternalAlloc)
    {
        OBJSYS *pSys = SYS_GET_INSTANCE();
        NvBool bSupported = pSys->getProperty(pSys, PDB_PROP_SYS_INTERNAL_EVENT_BUFFER_ALLOC_ALLOWED);
        NV_ASSERT_OR_RETURN(bSupported, NV_ERR_NOT_SUPPORTED);
    }
    else
    {
        NV_ASSERT_OR_RETURN((pAllocParams->hRecordBuffer != 0), NV_ERR_INVALID_ARGUMENT);
        NV_ASSERT_OR_RETURN(((pAllocParams->vardataBufferSize == 0) ^ (pAllocParams->hVardataBuffer != 0)),
                          NV_ERR_INVALID_ARGUMENT);

        status = clientGetResourceRef(pCallContext->pClient, pAllocParams->hBufferHeader, &pHeaderRef);
        if (status != NV_OK)
            return status;

        status = clientGetResourceRef(pCallContext->pClient, pAllocParams->hRecordBuffer, &pRecordRef);
        if (status != NV_OK)
            return status;

        // Avoid mixing and matching backing-memory
        if (pRecordRef->externalClassId != pHeaderRef->externalClassId)
            return NV_ERR_INVALID_ARGUMENT;

        if (pAllocParams->hVardataBuffer != 0)
        {
            status = clientGetResourceRef(pCallContext->pClient, pAllocParams->hVardataBuffer, &pVardataRef);
            if (status != NV_OK)
                return status;

            if (pVardataRef->externalClassId != pHeaderRef->externalClassId)
                return NV_ERR_INVALID_ARGUMENT;
        }

        bNoDeviceMem = (pRecordRef->externalClassId == NV01_MEMORY_DEVICELESS);

        if (!bNoDeviceMem)
        {
            if (pAllocParams->hSubDevice == 0)
            {
                NV_PRINTF(LEVEL_WARNING, "hSubDevice must be provided.\n");
                return NV_ERR_INVALID_ARGUMENT;
            }
        }
    }

    // bound check inputs and also check for overflow
    if ((pAllocParams->recordSize == 0) || (pAllocParams->recordCount == 0) ||
        (!portSafeMulU32(pAllocParams->recordSize, pAllocParams->recordCount, &recordBufferSize)) ||
        (recordBufferSize / pAllocParams->recordCount != pAllocParams->recordSize) ||
        (pAllocParams->recordsFreeThreshold > pAllocParams->recordCount) ||
        (pAllocParams->vardataFreeThreshold > pAllocParams->vardataBufferSize))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pEventBuffer->hClient = pCallContext->pClient->hClient;
    pEventBuffer->hSubDevice = pAllocParams->hSubDevice;
    if (pEventBuffer->hSubDevice != 0)
    {
        status = subdeviceGetByHandle(pCallContext->pClient, pEventBuffer->hSubDevice, &pSubdevice);
        if (status != NV_OK)
            return NV_ERR_INVALID_OBJECT_HANDLE;

        pEventBuffer->subDeviceInst = pSubdevice->subDeviceInst;
        pGpu = GPU_RES_GET_GPU(pSubdevice);

        if (!bNoDeviceMem)
        {
            if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu))
            {
                // Staging buffer should be mapped as read-only in guest RM
                bUsingVgpuStagingBuffer = NV_TRUE;
            }

            if (!bKernel)
            {
                RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
                status = rmapiutilAllocClientAndDeviceHandles(pRmApi,
                        pGpu,
                        &pEventBuffer->hInternalClient,
                        &pEventBuffer->hInternalDevice,
                        &pEventBuffer->hInternalSubdevice);

                if (status != NV_OK)
                    return status;

                hMapperClient = pEventBuffer->hInternalClient;
                hMapperDevice = pEventBuffer->hInternalDevice;
            }
            else
            {
                hMapperClient = pCallContext->pClient->hClient;
                hMapperDevice = RES_GET_PARENT_HANDLE(pSubdevice);
            }
        }
    }


    //
    // Use goto cleanup on failure below here
    //

    if (!bInternalAlloc)
    {
        Memory *pMemory;
        NvBool bRequireReadOnly = bUsingVgpuStagingBuffer || !bKernel;
        NvU32 flags = 0;

        // Allow the mapping to succeed when HCC is enabled in devtools mode
        if (kbusIsBarAccessBlocked(GPU_GET_KERNEL_BUS(pGpu)) &&
            gpuIsCCDevToolsModeEnabled(pGpu))
        {
            flags = FLD_SET_DRF(OS33, _FLAGS, _ALLOW_MAPPING_ON_HCC, _YES, flags);
        }

        if (bUsingVgpuStagingBuffer)
        {
            flags = FLD_SET_DRF(OS33, _FLAGS, _ACCESS, _READ_ONLY, flags);
        }
        else
        {
            flags = FLD_SET_DRF(OS33, _FLAGS, _ACCESS, _READ_WRITE, flags);
        }

        //
        // Buffer header
        //
        pEventBuffer->pHeader = dynamicCast(pHeaderRef->pResource, Memory);
        pMemory = pEventBuffer->pHeader;
        if ((pMemory == NULL) || (bRequireReadOnly && !memdescGetFlag(pMemory->pMemDesc, MEMDESC_FLAGS_USER_READ_ONLY)))
        {
            status = NV_ERR_INVALID_ARGUMENT;
            goto cleanup;
        }

        if (pMemory->Length < sizeof(NV_EVENT_BUFFER_HEADER))
        {
            status = NV_ERR_INVALID_ARGUMENT;
            goto cleanup;
        }

        if (!bNoDeviceMem)
        {
            RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
            NvHandle hMemory = RES_GET_HANDLE(pMemory);

            // Dup memory object under CPU-RM's hClient
            if (!bKernel)
            {
                status = pRmApi->DupObject(pRmApi,
                                           hMapperClient,
                                           hMapperDevice,
                                           &hMemory,
                                           pCallContext->pClient->hClient,
                                           hMemory, 0);
                if (status != NV_OK)
                {
                    goto cleanup;
                }
            }

            status = pRmApi->MapToCpu(pRmApi,
                                      hMapperClient,
                                      hMapperDevice,
                                      hMemory,
                                      0,
                                      pMemory->Length,
                                      &pKernelMap->headerAddr,
                                      flags);

            if (status != NV_OK)
            {
                goto cleanup;
            }
        }
        else
        {
            status = memCreateKernelMapping(pMemory, NV_PROTECT_READ_WRITE, NV_TRUE);
            if (status != NV_OK)
                goto cleanup;

            pKernelMap->headerAddr = pMemory->KernelVAddr;
        }

        //
        // Record buffer
        //
        pEventBuffer->pRecord = dynamicCast(pRecordRef->pResource, Memory);
        pMemory = pEventBuffer->pRecord;
        if ((pMemory == NULL) || (bRequireReadOnly && !memdescGetFlag(pMemory->pMemDesc, MEMDESC_FLAGS_USER_READ_ONLY)))
        {
            status = NV_ERR_INVALID_ARGUMENT;
            goto cleanup;
        }

        if (pMemory->Length < recordBufferSize)
        {
            status = NV_ERR_INVALID_ARGUMENT;
            goto cleanup;
        }

        if (!bNoDeviceMem)
        {
            RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
            NvHandle hMemory = RES_GET_HANDLE(pMemory);

            // Dup memory object under CPU-RM's hClient
            if (!bKernel)
            {
                status = pRmApi->DupObject(pRmApi,
                                           hMapperClient,
                                           hMapperDevice,
                                           &hMemory,
                                           pCallContext->pClient->hClient,
                                           hMemory, 0);
                if (status != NV_OK)
                {
                    goto cleanup;
                }
            }

            status = pRmApi->MapToCpu(pRmApi,
                                      hMapperClient,
                                      hMapperDevice,
                                      hMemory,
                                      0,
                                      pMemory->Length,
                                      &pKernelMap->recordBuffAddr,
                                      flags);
            if (status != NV_OK)
            {
                goto cleanup;
            }
        }
        else
        {
            status = memCreateKernelMapping(pMemory, NV_PROTECT_READ_WRITE, NV_TRUE);
            pKernelMap->recordBuffAddr = pMemory->KernelVAddr;
            if (status != NV_OK)
                goto cleanup;
        }

        //
        // Vardata buffer [optional]
        //
        if (pAllocParams->hVardataBuffer != 0)
        {
            pEventBuffer->pVardata = dynamicCast(pVardataRef->pResource, Memory);
            pMemory = pEventBuffer->pVardata;
            if ((pMemory == NULL) || (bRequireReadOnly && !memdescGetFlag(pMemory->pMemDesc, MEMDESC_FLAGS_USER_READ_ONLY)))
            {
                status = NV_ERR_INVALID_ARGUMENT;
                goto cleanup;
            }

            if (pMemory->Length < pAllocParams->vardataBufferSize)
            {
                status = NV_ERR_INVALID_ARGUMENT;
                goto cleanup;
            }

            if (!bNoDeviceMem)
            {
                RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
                NvHandle hMemory = RES_GET_HANDLE(pMemory);

                // Dup memory object under CPU-RM's hClient
                if (!bKernel)
                {
                    status = pRmApi->DupObject(pRmApi,
                                               hMapperClient,
                                               hMapperDevice,
                                               &hMemory,
                                               pCallContext->pClient->hClient,
                                               hMemory, 0);
                    if (status != NV_OK)
                    {
                        goto cleanup;
                    }
                }

                status = pRmApi->MapToCpu(pRmApi,
                                          hMapperClient,
                                          hMapperDevice,
                                          hMemory,
                                          0,
                                          pMemory->Length,
                                          &pKernelMap->recordBuffAddr,
                                          flags);
                if (status != NV_OK)
                {
                    goto cleanup;
                }
            }
            else
            {
                status = memCreateKernelMapping(pMemory, NV_PROTECT_READ_WRITE, NV_TRUE);
                if (status != NV_OK)
                    goto cleanup;
            }

            pKernelMap->vardataBuffAddr = pMemory->KernelVAddr;

            refAddDependant(pVardataRef, pCallContext->pResourceRef);
        }

        refAddDependant(pHeaderRef, pCallContext->pResourceRef);
        refAddDependant(pRecordRef, pCallContext->pResourceRef);
    }

    if (bInternalAlloc)
    {
        status = _allocAndMapMemory(pCallContext,
                                    pAllocParams->bufferHeader,
                                    &pEventBuffer->pHeaderDesc,
                                    sizeof(NV_EVENT_BUFFER_HEADER),
                                    bKernel,
                                    &pKernelMap->headerAddr,
                                    &pKernelMap->headerPriv,
                                    &pClientMap->headerAddr,
                                    &pClientMap->headerPriv,
                                    pSubdevice);
        if (status != NV_OK)
            goto cleanup;

        status = _allocAndMapMemory(pCallContext,
                                    pAllocParams->recordBuffer,
                                    &pEventBuffer->pRecordBufDesc,
                                    recordBufferSize,
                                    bKernel,
                                    &pKernelMap->recordBuffAddr,
                                    &pKernelMap->recordBuffPriv,
                                    &pClientMap->recordBuffAddr,
                                    &pClientMap->recordBuffPriv,
                                    pSubdevice);
        if (status != NV_OK)
            goto cleanup;
    }

    eventBufferInitRecordBuffer(&pEventBuffer->producerInfo,
                                KERNEL_POINTER_FROM_NvP64(NV_EVENT_BUFFER_HEADER*, pKernelMap->headerAddr),
                                pKernelMap->recordBuffAddr,
                                pAllocParams->recordSize,
                                pAllocParams->recordCount,
                                recordBufferSize,
                                pAllocParams->recordsFreeThreshold);

    // not needed for all events, such as FECS context switch events
    if (pAllocParams->vardataBufferSize != 0)
    {
        if (bInternalAlloc)
        {
            status = _allocAndMapMemory(pCallContext,
                    pAllocParams->vardataBuffer,
                    &pEventBuffer->pVardataBufDesc,
                    pAllocParams->vardataBufferSize,
                    bKernel,
                    &pKernelMap->vardataBuffAddr,
                    &pKernelMap->vardataBuffPriv,
                    &pClientMap->vardataBuffAddr,
                    &pClientMap->vardataBuffPriv,
                    pSubdevice);

            if (status != NV_OK)
                goto cleanup;
        }

        eventBufferInitVardataBuffer(&pEventBuffer->producerInfo,
                                     pKernelMap->vardataBuffAddr,
                                     pAllocParams->vardataBufferSize,
                                     pAllocParams->vardataFreeThreshold);
    }

    kernelNotificationhandle = (NvP64)pAllocParams->notificationHandle;
    if (bKernel != NV_TRUE)
        status = osUserHandleToKernelPtr(pCallContext->pClient->hClient,
                                         kernelNotificationhandle,
                                         &kernelNotificationhandle);

    eventBufferInitNotificationHandle(&pEventBuffer->producerInfo, kernelNotificationhandle);
    eventBufferSetEnable(&pEventBuffer->producerInfo, NV_FALSE);

    // return user mode mappings
    pAllocParams->bufferHeader = pClientMap->headerAddr;
    pAllocParams->recordBuffer = pClientMap->recordBuffAddr;
    pAllocParams->vardataBuffer = pClientMap->vardataBuffAddr;

    return NV_OK;

cleanup:
    eventbufferDestruct_IMPL(pEventBuffer);
    return status;
}

void
eventbufferDestruct_IMPL
(
    EventBuffer *pEventBuffer
)
{
    CALL_CONTEXT          *pCallContext;
    EVENT_BUFFER_MAP_INFO *pClientMap         = &pEventBuffer->clientMapInfo;
    EVENT_BUFFER_MAP_INFO *pKernelMap         = &pEventBuffer->kernelMapInfo;
    RmClient              *pRmClient          = dynamicCast(RES_GET_CLIENT(pEventBuffer), RmClient);
    NvBool                 bKernel;
    void                  *notificationHandle = NvP64_VALUE(pEventBuffer->producerInfo.notificationHandle);

    NV_ASSERT_OR_RETURN_VOID(pRmClient != NULL);

    bKernel = (rmclientGetCachedPrivilege(pRmClient) >= RS_PRIV_LEVEL_KERNEL);

    resGetFreeParams(staticCast(pEventBuffer, RsResource), &pCallContext, NULL);

    if (notificationHandle != NULL)
    {
        osDereferenceObjectCount(notificationHandle);
    }

    // Clean-up all bind points
    videoRemoveAllBindpoints(pEventBuffer);
    fecsRemoveAllBindpoints(pEventBuffer);
#if KERNEL_GSP_TRACING_RATS_ENABLED
    gspTraceRemoveAllBindpoints(pEventBuffer);
#endif

    _unmapAndFreeMemory(pEventBuffer->pHeaderDesc, bKernel, pKernelMap->headerAddr,
        pKernelMap->headerPriv, pClientMap->headerAddr, pClientMap->headerPriv);

    _unmapAndFreeMemory(pEventBuffer->pRecordBufDesc, bKernel, pKernelMap->recordBuffAddr,
        pKernelMap->recordBuffPriv, pClientMap->recordBuffAddr, pClientMap->recordBuffPriv);

    _unmapAndFreeMemory(pEventBuffer->pVardataBufDesc, bKernel, pKernelMap->vardataBuffAddr,
        pKernelMap->vardataBuffPriv, pClientMap->vardataBuffAddr, pClientMap->vardataBuffPriv);

    if (pEventBuffer->hInternalClient != 0)
    {
        RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
        pRmApi->Free(pRmApi, pEventBuffer->hInternalClient, pEventBuffer->hInternalClient);
    }

}

NV_STATUS
_allocAndMapMemory
(
    CALL_CONTEXT *pCallContext,
    NvP64 pAddress,
    MEMORY_DESCRIPTOR** ppMemDesc,
    NvU64 size,
    NvBool bKernel,
    NvP64* pKernelAddr,
    NvP64* pKernelPriv,
    NvP64* pUserAddr,
    NvP64* pUserPriv,
    Subdevice *pSubdevice
)
{
    NV_STATUS           status;
    MEMORY_DESCRIPTOR*  pMemDesc = NULL;
    OBJGPU*             pGpu     = NULL;

    NV_ASSERT_OR_RETURN(pAddress == NvP64_NULL, NV_ERR_NOT_SUPPORTED);

    if (pSubdevice != NULL)
        pGpu = GPU_RES_GET_GPU(pSubdevice);

    NV_ASSERT_OR_RETURN(pSubdevice != NULL && pGpu != NULL, NV_ERR_INVALID_STATE);
 
    status = memdescCreate(ppMemDesc, pGpu, size, 0, NV_MEMORY_CONTIGUOUS,
            ADDR_SYSMEM, NV_MEMORY_WRITECOMBINED, MEMDESC_FLAGS_CPU_ONLY);
    if (status != NV_OK)
        return status;

    pMemDesc = *ppMemDesc;

    status = osAllocPages(pMemDesc);
    if (status != NV_OK)
        goto cleanup;
    pMemDesc->Allocated = 1;

    // map memory to kernel VA space
    status = memdescMap(pMemDesc, 0, size, NV_TRUE, NV_PROTECT_READ_WRITE,
                        pKernelAddr, pKernelPriv);
    if (status != NV_OK)
        goto cleanup;

    portMemSet(NvP64_VALUE(*pKernelAddr), 0, size);

    // map memory to user VA space
    status = memdescMap(pMemDesc, 0, size, bKernel, NV_PROTECT_READABLE,
            pUserAddr, pUserPriv);

    if (status != NV_OK)
        goto cleanup;

    return NV_OK;

cleanup:
    _unmapAndFreeMemory(pMemDesc, bKernel, *pKernelAddr, *pKernelPriv, *pUserAddr, *pUserPriv);
    return status;
}

static void
_unmapAndFreeMemory
(
    MEMORY_DESCRIPTOR *pMemDesc,
    NvBool             bKernel,
    NvP64              kernelAddr,
    NvP64              kernelPriv,
    NvP64              userAddr,
    NvP64              userPriv
)
{
    if (pMemDesc == NULL)
        return;

    if (userAddr)
        memdescUnmap(pMemDesc, bKernel, osGetCurrentProcess(), userAddr, userPriv);

    if (kernelAddr)
        memdescUnmap(pMemDesc, NV_TRUE, osGetCurrentProcess(), kernelAddr, kernelPriv);

    memdescFree(pMemDesc);
    memdescDestroy(pMemDesc);
}

NV_STATUS
eventbuffertBufferCtrlCmdFlush_IMPL
(
    EventBuffer *pEventBuffer
)
{
    OBJGPU *pGpu;
    NvU32 gpuMask = 0;
    NvU32 gpuIndex = 0;
    gpumgrGetGpuAttachInfo(NULL, &gpuMask);
    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuIndex)) != NULL)
    {
        nvEventBufferFecsCallback(pGpu, NULL);
        nvEventBufferVideoCallback(pGpu, NULL);
    }
    return NV_OK;
}

NV_STATUS
eventbuffertBufferCtrlCmdEnableEvent_IMPL
(
    EventBuffer *pEventBuffer,
    NV_EVENT_BUFFER_CTRL_CMD_ENABLE_EVENTS_PARAMS *pEnableParams
)
{
    GPU_MASK  gpuMask;
    NV_STATUS status          = NV_OK;
    NvBool    updateTelemetry = NV_FALSE;

    if (pEnableParams->flags &
        ~(NV_EVENT_BUFFER_FLAG_OVERFLOW_POLICY_KEEP_NEWEST|NV_EVENT_BUFFER_FLAG_OVERFLOW_POLICY_KEEP_OLDEST))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pEnableParams->enable && !pEventBuffer->producerInfo.isEnabled)
    {
        updateTelemetry = NV_TRUE;
    }

    eventBufferSetEnable(&pEventBuffer->producerInfo, pEnableParams->enable);
    if (pEnableParams->flags & NV_EVENT_BUFFER_FLAG_OVERFLOW_POLICY_KEEP_NEWEST)
        eventBufferSetKeepNewest(&pEventBuffer->producerInfo, NV_TRUE);
    else if (pEnableParams->flags & NV_EVENT_BUFFER_FLAG_OVERFLOW_POLICY_KEEP_OLDEST)
        eventBufferSetKeepNewest(&pEventBuffer->producerInfo, NV_FALSE);

    // NvTelemetry requires a valid subdevice
    if (updateTelemetry && pEventBuffer->hSubDevice)
    {
        Subdevice *pSubDevice;

        status = rmGpuGroupLockAcquire(pEventBuffer->subDeviceInst,
                                       GPU_LOCK_GRP_SUBDEVICE,
                                       GPUS_LOCK_FLAGS_NONE,
                                       RM_LOCK_MODULES_GPU, &gpuMask);
        if (status != NV_OK)
            return status;

        status = subdeviceGetByHandle(RES_GET_CLIENT(pEventBuffer),
                pEventBuffer->hSubDevice, &pSubDevice);
        if (status != NV_OK)
            return status;

        GPU_RES_SET_THREAD_BC_STATE(pSubDevice);

        rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);
    }
    return NV_OK;
}

NV_STATUS
eventbuffertBufferCtrlCmdUpdateGet_IMPL
(
    EventBuffer *pEventBuffer,
    NV_EVENT_BUFFER_CTRL_CMD_UPDATE_GET_PARAMS *pUpdateParams
)
{
    EVENT_BUFFER_PRODUCER_INFO *pProducerInfo = &pEventBuffer->producerInfo;
    NvP64 pVardataBuf = pEventBuffer->kernelMapInfo.vardataBuffAddr;

    if ((pUpdateParams->recordBufferGet >= eventBufferGetRecordBufferCount(pProducerInfo)) ||
        (pVardataBuf == NvP64_NULL && pUpdateParams->varDataBufferGet > 0) ||
        (pVardataBuf != NvP64_NULL && pUpdateParams->varDataBufferGet >= eventBufferGetVardataBufferCount(pProducerInfo)))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    eventBufferUpdateRecordBufferGet(pProducerInfo, pUpdateParams->recordBufferGet);
    if (pVardataBuf)
        eventBufferUpdateVardataBufferGet(pProducerInfo, pUpdateParams->varDataBufferGet);

    pEventBuffer->bNotifyPending = NV_FALSE;

    return NV_OK;
}

/*
 *  eventbuffertBufferCtrlCmdPostTelemetryEvent posts an event to the event buffer for testing purposes.
 *  Note -- in order to post an event, a handle to the buffer is required.  since the handle is
 *  only available to the client that created the buffer, one can only post events to buffers that
 *  it created.  this has been done to limit the ability to post to buffers for testing purposes
 *  only.  if it is determined that we want to open this up to other callers, then this ctrl call
 *  should be moved to the 2080 class & adjustments made for acquiring the pGpu based on the
 *  subdevice handle there.
 */
NV_STATUS
eventbuffertBufferCtrlCmdPostTelemetryEvent_IMPL
(
    EventBuffer *pEventBuffer,
    NV_EVENT_BUFFER_CTRL_CMD_POST_TELEMETRY_EVENT_PARAMS *pPostTelemetryEvent
)
{
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS
eventBufferAdd(EventBuffer* pEventBuffer, void *pEventData, NvU32 recordType, NvBool *pBNotify, NvP64 *pHandle)
{
    EVENT_BUFFER_PRODUCER_DATA *pProducerData = (EVENT_BUFFER_PRODUCER_DATA*)pEventData;
    RECORD_BUFFER_INFO *pRBI;
    NV_EVENT_BUFFER_HEADER *pHeader;

    if (!pEventBuffer->producerInfo.isEnabled)
        return NV_WARN_NOTHING_TO_DO;

    pRBI = &pEventBuffer->producerInfo.recordBuffer;
    pHeader = pEventBuffer->producerInfo.recordBuffer.pHeader;

    NV_ASSERT_OR_RETURN(pHeader->recordPut < pRBI->totalRecordCount, NV_ERR_INVALID_STATE);

    eventBufferProducerAddEvent(&pEventBuffer->producerInfo,
        recordType, 0, pProducerData);

    *pBNotify = (!pEventBuffer->bNotifyPending) &&
                (eventBufferIsNotifyThresholdMet(&pEventBuffer->producerInfo));
    *pHandle  = pEventBuffer->producerInfo.notificationHandle;
    return NV_OK;
}
