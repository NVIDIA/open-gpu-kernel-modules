/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
 *
 *   Description:
 *       This file contains the functions managing the memory multicast fabric
 *
 *****************************************************************************/
#include "os/os.h"
#include "core/locks.h"
#include "nvport/nvport.h"
#include "rmapi/rs_utils.h"
#include "rmapi/rmapi_utils.h"
#include "compute/fabric.h"
#include "gpu/gpu.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/subdevice/subdevice.h"
#include "kernel/gpu/nvlink/kernel_nvlink.h"
#include "mem_mgr/fabric_vaspace.h"
#include "mem_mgr/mem_multicast_fabric.h"
#include "published/hopper/gh100/dev_mmu.h"

#include "gpu/gpu_fabric_probe.h"

static
NV_STATUS
_memMulticastFabricValidateAllocParams
(
    NV00FD_ALLOCATION_PARAMETERS *pAllocParams
)
{
    // Only page size 512MB is supported
    if (pAllocParams->pageSize != NV_MEMORY_MULTICAST_FABRIC_PAGE_SIZE_512M)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Unsupported pageSize: 0x%x. Only 512MB pagesize is supported\n",
                  pAllocParams->pageSize);
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Alignment should be pageSize aligned
    if (!NV_IS_ALIGNED64(pAllocParams->alignment, pAllocParams->pageSize))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Alignment should be pageSize aligned\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    // AllocSize should be page size aligned
    if (!NV_IS_ALIGNED64(pAllocParams->allocSize, pAllocParams->pageSize))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "AllocSize should be pageSize aligned\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pAllocParams->numGpus == 0)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Number of GPUs to attach must be non-zero\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Allocation flags must be zero
    if (pAllocParams->allocFlags != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "allocFlags must be zero\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

static
MEM_MULTICAST_FABRIC_DESCRIPTOR*
_memMulticastFabricDescriptorAllocUnderLock
(
    MemoryMulticastFabric        *pMemoryMulticastFabric,
    NV00FD_ALLOCATION_PARAMETERS *pAllocParams
)
{
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc;

    pMulticastFabricDesc = portMemAllocNonPaged(sizeof(MEM_MULTICAST_FABRIC_DESCRIPTOR));
    if (pMulticastFabricDesc == NULL)
        return NULL;

    portMemSet(pMulticastFabricDesc, 0, sizeof(MEM_MULTICAST_FABRIC_DESCRIPTOR));

    portAtomicExSetU64(&pMulticastFabricDesc->refCount, 1);

    listInit(&pMulticastFabricDesc->waitingClientsList,
             portMemAllocatorGetGlobalNonPaged());

    listInit(&pMulticastFabricDesc->attachMemInfoList,
             portMemAllocatorGetGlobalNonPaged());

    listInit(&pMulticastFabricDesc->gpuOsInfoList,
             portMemAllocatorGetGlobalNonPaged());

    portAtomicSetU32(&pMulticastFabricDesc->mcTeamStatus, NV_ERR_NOT_READY);

    portAtomicSetU32(&pMulticastFabricDesc->attachedGpusMask, 0);

    pMulticastFabricDesc->alignment  = pAllocParams->alignment;
    pMulticastFabricDesc->allocSize  = pAllocParams->allocSize;
    pMulticastFabricDesc->pageSize   = pAllocParams->pageSize;
    pMulticastFabricDesc->allocFlags = pAllocParams->allocFlags;
    pMulticastFabricDesc->numMaxGpus = pAllocParams->numGpus;

    return pMulticastFabricDesc;
}

static void
_memMulticastFabricDescriptorFlushClientsUnderLock
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc
)
{
    MEM_MULTICAST_FABRIC_CLIENT_INFO *pNode;

    while ((pNode = listHead(&pMulticastFabricDesc->waitingClientsList)) != NULL)
    {
        if (pNode->pOsEvent != NULL)
        {
            osSetEvent(NULL, pNode->pOsEvent);
            NV_ASSERT_OK(osDereferenceObjectCount(pNode->pOsEvent));
        }

        listRemove(&pMulticastFabricDesc->waitingClientsList, pNode);
    }

    return;
}

static NV_STATUS
_memMulticastFabricDescriptorEnqueueWaitUnderLock
(
    NvHandle                         hClient,
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc,
    NvP64                            pOsEvent,
    Memory                          *pMemory
)
{
    MEM_MULTICAST_FABRIC_CLIENT_INFO *pNode;
    NvP64                             pValidatedOsEvent = NULL;
    NV_STATUS                         status;

    if (pOsEvent != NULL)
    {
        status = osUserHandleToKernelPtr(hClient, pOsEvent, &pValidatedOsEvent);
        if (status != NV_OK)
            return status;
    }

    pNode = listAppendNew(&pMulticastFabricDesc->waitingClientsList);
    if (pNode == NULL)
    {
        if (pOsEvent != NULL)
            osDereferenceObjectCount(pValidatedOsEvent);

        return NV_ERR_NO_MEMORY;
    }

    pNode->pOsEvent = pValidatedOsEvent;
    pNode->pMemory = pMemory;

    // In case the multicast object's memdesc is ready, unblock clients waiting on it
    if (pMulticastFabricDesc->bMemdescInstalled)
        _memMulticastFabricDescriptorFlushClientsUnderLock(pMulticastFabricDesc);

    return NV_OK;
}

static void
_memMulticastFabricDescriptorDequeueWaitUnderLock
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc,
    Memory                          *pMemory
)
{
    MEM_MULTICAST_FABRIC_CLIENT_INFO *pNode;
    MEM_MULTICAST_FABRIC_CLIENT_INFO *pNodeNext;

    pNode = listHead(&pMulticastFabricDesc->waitingClientsList);

    // There can be multiple events per memory object, so delete all.
    while (pNode != NULL)
    {
        pNodeNext = listNext(&pMulticastFabricDesc->waitingClientsList, pNode);

        if (pNode->pMemory == pMemory)
        {
            if (pNode->pOsEvent != NULL)
                osDereferenceObjectCount(pNode->pOsEvent);

            listRemove(&pMulticastFabricDesc->waitingClientsList, pNode);
        }

        pNode = pNodeNext;
    }
}

NV_STATUS
_memMulticastFabricGpuOsInfoAddUnderLock
(
    MemoryMulticastFabric          *pMemoryMulticastFabric,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    NV00FD_CTRL_ATTACH_MEM_PARAMS *pAttachParams = pParams->pParams;
    Subdevice *pSubdevice = NULL;
    MEM_MULTICAST_FABRIC_GPU_OS_INFO *pNode;
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc = \
        pMemoryMulticastFabric->pMulticastFabricDesc;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        subdeviceGetByHandle(RES_GET_CLIENT(pMemoryMulticastFabric),
            pAttachParams->hSubdevice, &pSubdevice));

    if(!osMatchGpuOsInfo(GPU_RES_GET_GPU(pSubdevice),
                         pParams->secInfo.gpuOsInfo))
    {
        return NV_ERR_INVALID_DEVICE;
    }

    pNode = listAppendNew(&pMulticastFabricDesc->gpuOsInfoList);
    if (pNode == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    pNode->gpuOsInfo = pParams->secInfo.gpuOsInfo;

    return NV_OK;
}

static void
_memMulticastFabricGpuOsInfoRemoveUnderLock
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc
)
{
    MEM_MULTICAST_FABRIC_GPU_OS_INFO *pNode = NULL;
    THREAD_STATE_NODE *pThreadNode = NULL;
    THREAD_STATE_FREE_CALLBACK freeCallback;

    NV_ASSERT_OK(threadStateGetCurrent(&pThreadNode, NULL));

    while ((pNode = listHead(&pMulticastFabricDesc->gpuOsInfoList)) != NULL)
    {
        freeCallback.pCb = osReleaseGpuOsInfo;
        freeCallback.pCbData = (void *)pNode->gpuOsInfo;

        NV_ASSERT_OK(threadStateEnqueueCallbackOnFree(pThreadNode, &freeCallback));
        listRemove(&pMulticastFabricDesc->gpuOsInfoList, pNode);
    }
}

//
// The caller must have acquired the pMulticastFabricOpsMutex lock.
// GPU lock is acquired as needed.
//
static void
_memMulticastFabricRemoveFabricMapping
(
    OBJGPU            *pGpu,
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    FABRIC_VASPACE *pFabricVAS = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);
    NvU64 mcAddressBase = pMemDesc->_pteArray[0];

    NV_ASSERT_OK(rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE,
                                         RM_LOCK_MODULES_MEM_FLA));

    fabricvaspaceBatchFree(pFabricVAS, &mcAddressBase, 1, 1);

    rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);
}

static void
_memMulticastFabricBatchRemoveFabricMappingUnderLock
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR      *pMulticastFabricDesc,
    MEMORY_DESCRIPTOR                    *pMemDesc,
    MEM_MULTICAST_FABRIC_ATTACH_MEM_INFO *pEndNode
)
{
    MEM_MULTICAST_FABRIC_ATTACH_MEM_INFO *pNode = NULL;

    for (pNode = listHead(&pMulticastFabricDesc->attachMemInfoList);
         pNode != NULL;
         pNode = listNext(&pMulticastFabricDesc->attachMemInfoList, pNode))
    {
         if (pNode == pEndNode)
             return;

         _memMulticastFabricRemoveFabricMapping(pNode->pGpu, pMemDesc);
    }
}

static void
_memMulticastFabricAttachMemInfoRemoveUnderLock
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR      *pMulticastFabricDesc,
    MEM_MULTICAST_FABRIC_ATTACH_MEM_INFO *pNode
)
{
    pMulticastFabricDesc->numAttachedGpus--;

    portAtomicAndU32(&pMulticastFabricDesc->attachedGpusMask,
                     ~(NVBIT32(pNode->pGpu->gpuInstance)));

    listRemove(&pMulticastFabricDesc->attachMemInfoList, pNode);
}

static void
_memMulticastFabricDetachMemInfoUnderLock
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR      *pMulticastFabricDesc,
    MEM_MULTICAST_FABRIC_ATTACH_MEM_INFO *pNode
)
{
    RM_API *pRmApi;
    OBJGPU *pGpu = pNode->pGpu;
    FABRIC_VASPACE *pFabricVAS = \
        dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);
    NvHandle hDupedPhysMem;

    pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    hDupedPhysMem = pNode->hDupedPhysMem;

    _memMulticastFabricAttachMemInfoRemoveUnderLock(pMulticastFabricDesc, pNode);

    NV_ASSERT_OK(pRmApi->Free(pRmApi, pFabricVAS->hClient, hDupedPhysMem));
}

//
// The caller must have acquired the pMulticastFabricOpsMutex lock.
// GPU lock is acquired as needed.
//
static void
_memMulticastFabricBatchDetachMemInfo
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc
)
{
    MEM_MULTICAST_FABRIC_ATTACH_MEM_INFO *pNode = NULL;

    NV_ASSERT_OK(rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE,
                                   RM_LOCK_MODULES_MEM_FLA));

    //
    // _memMulticastFabricDetachMemInfoUnderLock internally trims down the
    // attachMemInfoList where pNode is freed up
    //
    while ((pNode = listHead(&pMulticastFabricDesc->attachMemInfoList)) != NULL)
    {
        _memMulticastFabricDetachMemInfoUnderLock(pMulticastFabricDesc, pNode);
    }

    rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);
}

NV_STATUS
_memMulticastFabricSendInbandTeamSetupRequestV1UnderLock
(
    OBJGPU                          *pGpu,
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS *sendDataParams;
    nvlink_inband_mc_team_setup_req_msg_t *pMcTeamSetupReqMsg = NULL;
    nvlink_inband_mc_team_setup_req_t *pMcTeamSetupReq = NULL;
    MEM_MULTICAST_FABRIC_ATTACH_MEM_INFO *pNode;
    NvU64 requestId;
    NvU32 idx = 0;
    NvU32 payloadSize;
    NvU32 sendDataSize;
    NV_STATUS status = NV_OK;

    sendDataParams = \
        (NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS *)portMemAllocNonPaged(sizeof(NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS));

    if (sendDataParams == NULL)
        return NV_ERR_NO_MEMORY;

    pMcTeamSetupReqMsg = \
        (nvlink_inband_mc_team_setup_req_msg_t *)&sendDataParams->buffer[0];

    pMcTeamSetupReq = \
        (nvlink_inband_mc_team_setup_req_t *)&pMcTeamSetupReqMsg->mcTeamSetupReq;

    payloadSize = (NvU32)(sizeof(nvlink_inband_mc_team_setup_req_t) + \
                      (sizeof(pMcTeamSetupReq->gpuHandles[0]) * pMulticastFabricDesc->numMaxGpus));

    sendDataSize = (NvU32)(sizeof(nvlink_inband_msg_header_t) + payloadSize);

    NV_ASSERT((NvU32)sendDataSize <= sizeof(sendDataParams->buffer));

    portMemSet(sendDataParams, 0, sendDataSize);

    pMcTeamSetupReq->mcAllocSize = pMulticastFabricDesc->allocSize;
    pMcTeamSetupReq->numGpuHandles = pMulticastFabricDesc->numMaxGpus;

    for (pNode = listHead(&pMulticastFabricDesc->attachMemInfoList);
         pNode != NULL;
         pNode = listNext(&pMulticastFabricDesc->attachMemInfoList, pNode))
        pMcTeamSetupReq->gpuHandles[idx++] = pNode->gpuProbeHandle;

    NV_ASSERT(idx == pMcTeamSetupReq->numGpuHandles);

    sendDataParams->dataSize = sendDataSize;

    status = fabricInitInbandMsgHdr(&pMcTeamSetupReqMsg->msgHdr,
                                    NVLINK_INBAND_MSG_TYPE_MC_TEAM_SETUP_REQ,
                                    payloadSize);

    if (status != NV_OK)
        goto done;

    requestId = pMcTeamSetupReqMsg->msgHdr.requestId;

    status = fabricMulticastSetupCacheInsertUnderLock_IMPL(pFabric,
                                                           requestId,
                                                           pMulticastFabricDesc);
    if (status != NV_OK)
        goto done;

    status = knvlinkSendInbandData(pGpu, GPU_GET_KERNEL_NVLINK(pGpu), sendDataParams);
    if (status != NV_OK)
    {
        fabricMulticastSetupCacheDeleteUnderLock_IMPL(pFabric, requestId);
        goto done;
    }

    pMulticastFabricDesc->bInbandReqInProgress = NV_TRUE;
    pMulticastFabricDesc->inbandReqId = requestId;

done:
    portMemFree(sendDataParams);

    return status;
}

NV_STATUS
_memMulticastFabricSendInbandTeamReleaseRequestV1UnderLock
(
    OBJGPU *pGpu,
    NvU64   mcTeamHandle
)
{
    NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS *sendDataParams;
    nvlink_inband_mc_team_release_req_msg_t *pMcTeamReleaseReqMsg = NULL;
    nvlink_inband_mc_team_release_req_t *pMcTeamReleaseReq = NULL;
    NvU32 payloadSize;
    NvU32 sendDataSize;
    NV_STATUS status = NV_OK;

    sendDataParams = \
        (NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS *)portMemAllocNonPaged(sizeof(NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS));

    if (sendDataParams == NULL)
        return NV_ERR_NO_MEMORY;

    pMcTeamReleaseReqMsg = \
        (nvlink_inband_mc_team_release_req_msg_t *)&sendDataParams->buffer[0];

    pMcTeamReleaseReq = \
        (nvlink_inband_mc_team_release_req_t *)&pMcTeamReleaseReqMsg->mcTeamReleaseReq;

    payloadSize = (NvU32)(sizeof(nvlink_inband_mc_team_release_req_t));

    sendDataSize = (NvU32)(sizeof(nvlink_inband_msg_header_t) + payloadSize);

    portMemSet(sendDataParams, 0, sendDataSize);

    pMcTeamReleaseReq->mcTeamHandle = mcTeamHandle;

    status = fabricInitInbandMsgHdr(&pMcTeamReleaseReqMsg->msgHdr,
                                    NVLINK_INBAND_MSG_TYPE_MC_TEAM_RELEASE_REQ,
                                    payloadSize);
    if (status != NV_OK)
        goto done;

    sendDataParams->dataSize = sendDataSize;

    status = knvlinkSendInbandData(pGpu, GPU_GET_KERNEL_NVLINK(pGpu), sendDataParams);

done:
    portMemFree(sendDataParams);

    return status;
}

//
// The caller must have acquired the pMulticastFabricOpsMutex lock.
// GPU lock is acquired as needed.
//
NV_STATUS
_memMulticastFabricSendInbandTeamSetupRequest
(
    OBJGPU                          *pGpu,
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc
)
{
    NvU64 fmCaps;
    NV_STATUS status = NV_OK;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE,
            RM_LOCK_MODULES_MEM_FLA));

    status = gpuFabricProbeGetfmCaps(pGpu->pGpuFabricProbeInfo, &fmCaps);
    if (status != NV_OK)
        goto done;

    if (!(fmCaps & NVLINK_INBAND_FM_CAPS_MC_TEAM_SETUP_V1))
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto done;
    }

    status = _memMulticastFabricSendInbandTeamSetupRequestV1UnderLock(pGpu,
                                                                      pMulticastFabricDesc);

done:
    rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);

    return status;
}

NV_STATUS
_memMulticastFabricSendInbandTeamReleaseRequest
(
    OBJGPU *pGpu,
    NvU64   mcTeamHandle
)
{
    NvU64 fmCaps;
    NV_STATUS status = NV_OK;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE,
            RM_LOCK_MODULES_MEM_FLA));

    status = gpuFabricProbeGetfmCaps(pGpu->pGpuFabricProbeInfo, &fmCaps);
    if (status != NV_OK)
        goto done;

    if (!(fmCaps & NVLINK_INBAND_FM_CAPS_MC_TEAM_RELEASE_V1))
    {
        status = NV_ERR_NOT_SUPPORTED;
        goto done;
    }

    status = _memMulticastFabricSendInbandTeamReleaseRequestV1UnderLock(pGpu,
                                                                        mcTeamHandle);

done:
    rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);

    return status;
}

NV_STATUS
_memMulticastFabricSendInbandRequestUnderLock
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR   *pMulticastFabricDesc,
    MEM_MULTICAST_FABRIC_REQUEST_TYPE  requestType
)
{
    NV_STATUS status = NV_OK;
    OBJGPU *pGpu;

    //
    // TODO: Implement the randomization of pGpu to be used for the inband
    // communication. Currently we pick the first pGpu in the attachMemInfoList
    // to send the inband request.
    //

    pGpu = listHead(&pMulticastFabricDesc->attachMemInfoList)->pGpu;

    switch (requestType)
    {
        case MEM_MULTICAST_FABRIC_TEAM_SETUP_REQUEST:
            status = _memMulticastFabricSendInbandTeamSetupRequest(pGpu,
                                                                   pMulticastFabricDesc);
            break;
        case MEM_MULTICAST_FABRIC_TEAM_RELEASE_REQUEST:
            status = _memMulticastFabricSendInbandTeamReleaseRequest(pGpu,
                                                                     pMulticastFabricDesc->mcTeamHandle);
            break;
        default:
            status = NV_ERR_NOT_SUPPORTED;
            break;
    }

    return status;
}

static void
_memMulticastFabricDescriptorFreeUnderLock
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc
)
{
    if (pMulticastFabricDesc == NULL)
        return;

    if (portAtomicExDecrementU64(&pMulticastFabricDesc->refCount) == 0)
    {
        if (pMulticastFabricDesc->pMemDesc != NULL)
        {
            NV_ASSERT(pMulticastFabricDesc->bMemdescInstalled);

            _memMulticastFabricBatchRemoveFabricMappingUnderLock(pMulticastFabricDesc,
                                                                 pMulticastFabricDesc->pMemDesc,
                                                                 NULL);
            _memMulticastFabricSendInbandRequestUnderLock(pMulticastFabricDesc,
                                                          MEM_MULTICAST_FABRIC_TEAM_RELEASE_REQUEST);
        }

        if (pMulticastFabricDesc->bInbandReqInProgress)
        {
            Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
            OS_WAIT_QUEUE *pWq;
            THREAD_STATE_NODE *pThreadNode = NULL;
            THREAD_STATE_FREE_CALLBACK freeCallback;

            fabricMulticastSetupCacheDeleteUnderLock_IMPL(pFabric,
                                                          pMulticastFabricDesc->inbandReqId);

            NV_ASSERT_OK(osAllocWaitQueue(&pWq));

            if (pWq != NULL)
            {
                NV_ASSERT_OK(fabricMulticastCleanupCacheInsertUnderLock_IMPL(pFabric,
                                                                             pMulticastFabricDesc->inbandReqId,
                                                                             pWq));

                NV_ASSERT_OK(threadStateGetCurrent(&pThreadNode, NULL));

                freeCallback.pCb = fabricMulticastWaitOnTeamCleanupCallback;
                freeCallback.pCbData = (void *)pMulticastFabricDesc->inbandReqId;

                NV_ASSERT_OK(threadStateEnqueueCallbackOnFree(pThreadNode, &freeCallback));
            }
        }

        _memMulticastFabricGpuOsInfoRemoveUnderLock(pMulticastFabricDesc);

        NV_ASSERT(listCount(&pMulticastFabricDesc->gpuOsInfoList) == 0);
        listDestroy(&pMulticastFabricDesc->gpuOsInfoList);

        _memMulticastFabricBatchDetachMemInfo(pMulticastFabricDesc);

        NV_ASSERT(pMulticastFabricDesc->numAttachedGpus == 0);
        NV_ASSERT(portAtomicOrU32(&pMulticastFabricDesc->attachedGpusMask, 0) == 0);
        NV_ASSERT(listCount(&pMulticastFabricDesc->attachMemInfoList) == 0);
        listDestroy(&pMulticastFabricDesc->attachMemInfoList);

        NV_ASSERT(listCount(&pMulticastFabricDesc->waitingClientsList) == 0);
        listDestroy(&pMulticastFabricDesc->waitingClientsList);

        memdescDestroy(pMulticastFabricDesc->pMemDesc);

        portMemFree(pMulticastFabricDesc);
    }
}

NV_STATUS
_memMulticastFabricConstructUnderLock
(
    MemoryMulticastFabric        *pMemoryMulticastFabric,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    Memory                           *pMemory       = staticCast(pMemoryMulticastFabric, Memory);
    NV00FD_ALLOCATION_PARAMETERS     *pAllocParams  = pParams->pAllocParams;
    MEM_MULTICAST_FABRIC_DESCRIPTOR  *pMulticastFabricDesc;
    NV_STATUS                         status        = NV_OK;

    pMulticastFabricDesc = _memMulticastFabricDescriptorAllocUnderLock(pMemoryMulticastFabric,
                                                                       pAllocParams);

    if (pMulticastFabricDesc == NULL)
        return NV_ERR_NO_MEMORY;

    status = _memMulticastFabricDescriptorEnqueueWaitUnderLock(pParams->hClient,
                                                               pMulticastFabricDesc,
                                                               pAllocParams->pOsEvent,
                                                               pMemory);
    if (status != NV_OK)
        goto fail;

    pMemoryMulticastFabric->pMulticastFabricDesc = pMulticastFabricDesc;

    return NV_OK;

fail:
    _memMulticastFabricDescriptorFreeUnderLock(pMulticastFabricDesc);

    return status;
}

NV_STATUS
_memMulticastFabricAttachMemInfoAddUnderLock
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR       *pMulticastFabricDesc,
    OBJGPU                                *pGpu,
    NV00FD_CTRL_ATTACH_MEM_PARAMS         *pParams,
    MEMORY_DESCRIPTOR                     *pPhysMemDesc,
    NvHandle                               hDupedPhysMem,
    NvU64                                  gpuProbeHandle,
    MEM_MULTICAST_FABRIC_ATTACH_MEM_INFO **pAttachMemNode
)
{
    MEM_MULTICAST_FABRIC_ATTACH_MEM_INFO *pNode;
    MEM_MULTICAST_FABRIC_ATTACH_MEM_INFO *pNodeItr;

    for (pNodeItr = listHead(&pMulticastFabricDesc->attachMemInfoList);
         pNodeItr != NULL;
         pNodeItr = listNext(&pMulticastFabricDesc->attachMemInfoList, pNodeItr))
    {
        if (pNodeItr->pGpu == pGpu)
        {
           NV_PRINTF(LEVEL_ERROR, "GPU %x has already attached to this multicast object\n",
                                   pGpu->gpuInstance);
           return NV_ERR_IN_USE;
        }
    }

    pNode = listAppendNew(&pMulticastFabricDesc->attachMemInfoList);
    if (pNode == NULL)
        return NV_ERR_NO_MEMORY;

    pNode->offset        = pParams->offset;
    pNode->mapOffset     = pParams->mapOffset;
    pNode->mapLength     = pParams->mapLength;
    pNode->flags         = pParams->flags;
    pNode->pGpu          = pGpu;
    pNode->pPhysMemDesc  = pPhysMemDesc;
    pNode->hDupedPhysMem = hDupedPhysMem;
    pNode->gpuProbeHandle = gpuProbeHandle;

    portAtomicOrU32(&pMulticastFabricDesc->attachedGpusMask,
                    NVBIT32(pGpu->gpuInstance));

    pMulticastFabricDesc->numAttachedGpus++;

    *pAttachMemNode = pNode;

    return NV_OK;
}

//
// The caller must have acquired the pMulticastFabricOpsMutex lock.
// GPU lock is acquired as needed.
//
NV_STATUS
_memMulticastFabricSetupFabricMapping
(
    OBJGPU                               *pGpu,
    MEM_MULTICAST_FABRIC_DESCRIPTOR      *pMulticastFabricDesc,
    MEM_MULTICAST_FABRIC_ATTACH_MEM_INFO *pAttachMemInfo,
    MEMORY_DESCRIPTOR                    *pMemDesc
)
{
    FABRIC_VASPACE *pFabricVAS = NULL;
    VAS_ALLOC_FLAGS flags = {0};
    NV_STATUS status = NV_OK;
    NvU64 physMemPageSize;
    NvU64 mcAddressBase = pMemDesc->_pteArray[0];

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE,
            RM_LOCK_MODULES_MEM_FLA));

    pFabricVAS = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);

    physMemPageSize = memdescGetPageSize(pAttachMemInfo->pPhysMemDesc, AT_GPU);

    status = fabricvaspaceAllocMulticast(pFabricVAS, physMemPageSize,
                                         pMulticastFabricDesc->alignment,
                                         flags,
                                         mcAddressBase,
                                         pMulticastFabricDesc->allocSize);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Fabric VA space alloc failed! for GPU %x\n",
                  pGpu->gpuInstance);
        goto releaseGpuLock;
    }

    //
    // For now MCFLA code assumes zero fabric offset. When a non-zero fabric
    // offset supported, revisit map/unmap paths.
    //
    status = fabricvaspaceMapPhysMemdesc(pFabricVAS,
                                         pMemDesc,
                                         0,
                                         pAttachMemInfo->pPhysMemDesc,
                                         pAttachMemInfo->mapOffset,
                                         pAttachMemInfo->mapLength,
                                         0);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to map MC FLA at the given physical offset for GPU %x\n",
                  pGpu->gpuInstance);
        goto freeFabricVa;
    }

    //
    // TODO: Handle the case to map remaining MC FLA range
    // to dummy pages when physMemSize < mcAllocSize
    //

    rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);

    return NV_OK;

freeFabricVa:
    fabricvaspaceBatchFree(pFabricVAS, &mcAddressBase, 1, 1);

releaseGpuLock:
    rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);

    return status;
}

NV_STATUS
_memMulticastFabricBatchSetupFabricMappingUnderLock
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc,
    MEMORY_DESCRIPTOR               *pMemDesc
)
{
    MEM_MULTICAST_FABRIC_ATTACH_MEM_INFO *pNode = NULL;
    NV_STATUS status = NV_OK;

    for (pNode = listHead(&pMulticastFabricDesc->attachMemInfoList);
         pNode != NULL;
         pNode = listNext(&pMulticastFabricDesc->attachMemInfoList, pNode))
    {
        status = _memMulticastFabricSetupFabricMapping(pNode->pGpu, pMulticastFabricDesc,
                                                       pNode, pMemDesc);
        if (status != NV_OK)
            goto batchRemoveMapping;
    }

    return NV_OK;

batchRemoveMapping:
    _memMulticastFabricBatchRemoveFabricMappingUnderLock(pMulticastFabricDesc,
                                                         pMemDesc, pNode);

    return status;
}

NV_STATUS
_memMulticastFabricCreateMemDescUnderLock
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR  *pMulticastFabricDesc,
    NvU64                             mcAddressBase,
    MEMORY_DESCRIPTOR               **ppMemDesc
)
{
    NV_STATUS status;
    MEMORY_DESCRIPTOR *pTempMemDesc = NULL;

    status = memdescCreate(&pTempMemDesc, NULL, pMulticastFabricDesc->allocSize,
                           0, NV_TRUE, ADDR_FABRIC_MC, NV_MEMORY_UNCACHED,
                           MEMDESC_FLAGS_NONE);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to allocate memory descriptor for multicast object\n");
        return status;
    }

    memdescSetPte(pTempMemDesc, AT_GPU, 0, mcAddressBase);

    memdescSetPageSize(pTempMemDesc, AT_GPU, pMulticastFabricDesc->pageSize);

    pTempMemDesc->_pteKind = NV_MMU_PTE_KIND_SMSKED_MESSAGE;

    memdescSetFlag(pTempMemDesc, MEMDESC_FLAGS_SET_KIND, NV_TRUE);

    memdescSetGpuCacheAttrib(pTempMemDesc, NV_MEMORY_UNCACHED);

    *ppMemDesc = pTempMemDesc;

    return NV_OK;
}

void
_memMulticastFabricInstallMemDescUnderLock
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc,
    MEMORY_DESCRIPTOR               *pMemDesc,
    NvU64                            mcTeamHandle,
    NV_STATUS                        status
)
{
    NV_ASSERT(pMulticastFabricDesc->pMemDesc == NULL);

    pMulticastFabricDesc->pMemDesc = pMemDesc;
    pMulticastFabricDesc->bMemdescInstalled = NV_TRUE;
    pMulticastFabricDesc->mcTeamHandle = mcTeamHandle;

    portAtomicMemoryFenceStore();

    portAtomicSetU32(&pMulticastFabricDesc->mcTeamStatus, status);

    portAtomicMemoryFenceStore();

    _memMulticastFabricDescriptorFlushClientsUnderLock(pMulticastFabricDesc);
}

//
// TODO: This function will be invoked as part of the FM inband
// response handling once inband integration is complete.
//
NV_STATUS
_memMulticastFabricAttachMemPostProcessorUnderLock
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc,
    NV_STATUS                        mcTeamStatus,
    NvU64                            mcTeamHandle,
    NvU64                            mcAddressBase,
    NvU64                            mcAddressSize
)
{
    NV_STATUS status = mcTeamStatus;
    MEMORY_DESCRIPTOR *pMemDesc = NULL;

    //
    // FM is never expected to return NV_ERR_NOT_READY
    // as part of the inband response.
    //
    NV_ASSERT(mcTeamStatus != NV_ERR_NOT_READY);

    if (mcTeamStatus != NV_OK)
        goto installMemDesc;

    if (mcAddressSize < pMulticastFabricDesc->allocSize)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Insufficient mcAddressSize returned from Fabric Manager\n");
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto installMemDesc;
    }

    if (!NV_IS_ALIGNED64(mcAddressBase, NV_MEMORY_MULTICAST_FABRIC_PAGE_SIZE_512M))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Insufficient mcAddressSize returned from Fabric Manager\n");
        status = NV_ERR_INVALID_ADDRESS;
        goto installMemDesc;
    }

    status = _memMulticastFabricCreateMemDescUnderLock(pMulticastFabricDesc,
                                                       mcAddressBase, &pMemDesc);
    if (status != NV_OK)
        goto installMemDesc;

    status = _memMulticastFabricBatchSetupFabricMappingUnderLock(pMulticastFabricDesc,
                                                                 pMemDesc);
    if (status != NV_OK)
        goto cleanupMemdesc;

    goto installMemDesc;

cleanupMemdesc:
    memdescDestroy(pMemDesc);
    pMemDesc = NULL;

installMemDesc:
    _memMulticastFabricInstallMemDescUnderLock(pMulticastFabricDesc,
                                               pMemDesc,
                                               mcTeamHandle,
                                               status);
     if ((status != NV_OK) && (mcTeamStatus == NV_OK))
         _memMulticastFabricSendInbandRequestUnderLock(pMulticastFabricDesc,
                                                       MEM_MULTICAST_FABRIC_TEAM_RELEASE_REQUEST);

    return status;
}

void
_memorymulticastfabricDestructUnderLock
(
    MemoryMulticastFabric *pMemoryMulticastFabric
)
{
    Memory *pMemory = staticCast(pMemoryMulticastFabric, Memory);

    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc = \
        pMemoryMulticastFabric->pMulticastFabricDesc;

    memDestructCommon(pMemory);

    _memMulticastFabricDescriptorDequeueWaitUnderLock(pMulticastFabricDesc, pMemory);

    _memMulticastFabricDescriptorFreeUnderLock(pMulticastFabricDesc);
}

void memorymulticastfabricTeamSetupResponseCallback
(
    OBJGPU                                         *pGpu,
    NV2080_CTRL_NVLINK_INBAND_RECEIVED_DATA_PARAMS *pInbandRcvParams
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    nvlink_inband_mc_team_setup_rsp_msg_t *pMcTeamSetupRspMsg;
    nvlink_inband_mc_team_setup_rsp_t *pMcTeamSetupRsp;
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc;
    NvU64 requestId;
    NV_STATUS mcTeamStatus;
    NvU64 mcTeamHandle = 0;
    NvU64 mcAddressBase = 0;
    NvU64 mcAddressSize = 0;
    NvU8 *pRsvd = NULL;

    NV_ASSERT(pInbandRcvParams != NULL);

    pMcTeamSetupRspMsg = \
        (nvlink_inband_mc_team_setup_rsp_msg_t *)&pInbandRcvParams->data[0];

    pMcTeamSetupRsp = \
        (nvlink_inband_mc_team_setup_rsp_t *)&pMcTeamSetupRspMsg->mcTeamSetupRsp;

    requestId = pMcTeamSetupRspMsg->msgHdr.requestId;

    mcTeamStatus = pMcTeamSetupRspMsg->msgHdr.status;

    if (mcTeamStatus == NV_OK)
    {
        mcTeamHandle = pMcTeamSetupRsp->mcTeamHandle;
        mcAddressBase = pMcTeamSetupRsp->mcAddressBase;
        mcAddressSize = pMcTeamSetupRsp->mcAddressSize;

        // Make sure that the reserved fields are initialized to 0
        pRsvd = &pMcTeamSetupRsp->reserved[0];

        NV_ASSERT((pRsvd[0] == 0) && portMemCmp(pRsvd, pRsvd + 1,
                  (sizeof(pMcTeamSetupRsp->reserved) - 1)) == 0);
    }

    fabricMulticastFabricOpsMutexAcquire(pFabric);

    pMulticastFabricDesc = \
        fabricMulticastSetupCacheGetUnderLock_IMPL(pFabric, requestId);

    if (pMulticastFabricDesc != NULL)
    {
        pMulticastFabricDesc->bInbandReqInProgress = NV_FALSE;

        pMulticastFabricDesc->inbandReqId = 0;

        fabricMulticastSetupCacheDeleteUnderLock_IMPL(pFabric, requestId);

        (void)_memMulticastFabricAttachMemPostProcessorUnderLock(pMulticastFabricDesc,
                                                                 mcTeamStatus,
                                                                 mcTeamHandle,
                                                                 mcAddressBase,
                                                                 mcAddressSize);
    }
    else
    {
        OS_WAIT_QUEUE *pWq;

        if (mcTeamStatus == NV_OK)
            (void)_memMulticastFabricSendInbandTeamReleaseRequest(pGpu,
                                                                  mcTeamHandle);

        //
        // Check if there is any thread waiting for team release and
        // wake it up.
        //
        // The multicast fabric descriptor could have undergone the
        // destruct sequence while an inband team setup request was in
        // progress with FM.
        //
        // In such a scenario the last thread to free the multicast
        // descriptor is put to sleep until the team setup response
        // is received and a subsequent team release request is sent.
        //

        pWq = (OS_WAIT_QUEUE *)fabricMulticastCleanupCacheGetUnderLock_IMPL(pFabric,
                                                                            requestId);

        if (pWq != NULL)
             osWakeUp(pWq);
    }

    fabricMulticastFabricOpsMutexRelease(pFabric);
}

NV_STATUS
memorymulticastfabricConstruct_IMPL
(
    MemoryMulticastFabric        *pMemoryMulticastFabric,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    Fabric                       *pFabric      = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    NV00FD_ALLOCATION_PARAMETERS *pAllocParams = pParams->pAllocParams;
    NV_STATUS                     status       = NV_OK;
    OBJSYS                       *pSys         = SYS_GET_INSTANCE();

    if (!pSys->bMulticastFlaEnabled)
        return NV_ERR_NOT_SUPPORTED;

    if (RS_IS_COPY_CTOR(pParams))
    {
        return memorymulticastfabricCopyConstruct_IMPL(pMemoryMulticastFabric,
                                                       pCallContext,
                                                       pParams);
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, _memMulticastFabricValidateAllocParams(pAllocParams));

    fabricMulticastFabricOpsMutexAcquire(pFabric);

    status = _memMulticastFabricConstructUnderLock(pMemoryMulticastFabric,
                                                   pCallContext,
                                                   pParams);

    fabricMulticastFabricOpsMutexRelease(pFabric);

    return status;
}

NV_STATUS
memorymulticastfabricCtrlAttachMem_IMPL
(
    MemoryMulticastFabric         *pMemoryMulticastFabric,
    NV00FD_CTRL_ATTACH_MEM_PARAMS *pParams
)
{
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc = \
        pMemoryMulticastFabric->pMulticastFabricDesc;
    NV_STATUS status = NV_OK;
    NvHandle hClient = RES_GET_CLIENT_HANDLE(pMemoryMulticastFabric);
    RsResourceRef *pPhysMemRef = NULL;
    MEMORY_DESCRIPTOR *pPhysMemDesc = NULL;
    Subdevice *pSubdevice = NULL;
    OBJGPU *pGpu = NULL;
    FABRIC_VASPACE *pFabricVAS = NULL;
    RM_API *pRmApi;
    NvHandle hDupedPhysMem = 0;
    NvU64 physMemPageSize;
    NvU64 physMemSize;
    MEM_MULTICAST_FABRIC_ATTACH_MEM_INFO *pAttachMemNode = NULL;
    NvU64 gpuProbeHandle = 0;

    // Check if the Multicast FLA object has any additional slots for GPUs
    if (pMulticastFabricDesc->numAttachedGpus == pMulticastFabricDesc->numMaxGpus)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Max no. of GPUs for this multicast object have already attached!\n");
        return NV_ERR_INVALID_OPERATION;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        subdeviceGetByHandle(RES_GET_CLIENT(pMemoryMulticastFabric),
            pParams->hSubdevice, &pSubdevice));

    pGpu = GPU_RES_GET_GPU(pSubdevice);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        rmDeviceGpuLocksAcquire(pGpu, GPUS_LOCK_FLAGS_NONE,
            RM_LOCK_MODULES_MEM_FLA));

    pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);

    if (RMCFG_FEATURE_PLATFORM_WINDOWS ||
        IS_VIRTUAL(pGpu))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Multicast attach not supported on Windows/CC/vGPU modes\n");
        status = NV_ERR_NOT_SUPPORTED;
        goto fail;
    }

    status = gpuFabricProbeGetGpuFabricHandle(pGpu->pGpuFabricProbeInfo, &gpuProbeHandle);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Attaching GPU does not have a valid probe handle\n");
        goto fail;
    }

    status = serverutilGetResourceRef(hClient, pParams->hMemory, &pPhysMemRef);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to get resource in resserv for physmem handle\n");
        goto fail;
    }

    pFabricVAS = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);
    if (pFabricVAS == NULL)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Fabric vaspace object not available for GPU %x\n",
                  pGpu->gpuInstance);
        status = NV_ERR_NOT_SUPPORTED;
        goto fail;
    }

    pPhysMemDesc = (dynamicCast(pPhysMemRef->pResource, Memory))->pMemDesc;

    if ((memdescGetAddressSpace(pPhysMemDesc) != ADDR_FBMEM) ||
        (pGpu != pPhysMemDesc->pGpu))
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid physical physmem handle passed\n");
        status = NV_ERR_INVALID_ARGUMENT;
        goto fail;
    }

    physMemPageSize = memdescGetPageSize(pPhysMemDesc, AT_GPU);

    physMemSize =  memdescGetSize(pPhysMemDesc);

    if (physMemPageSize != RM_PAGE_SIZE_HUGE)
    {
        NV_PRINTF(LEVEL_ERROR, "Physical physmem page size should be 2MB\n");
        status = NV_ERR_INVALID_ARGUMENT;
        goto fail;
    }

    if ((!NV_IS_ALIGNED64(pParams->mapOffset, physMemPageSize)) ||
        (!NV_IS_ALIGNED64(pParams->mapLength, physMemPageSize)) ||
        (pParams->mapOffset >= physMemSize) ||
        (pParams->mapLength > (physMemSize - pParams->mapOffset)))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Invalid mapOffset passed for the physical physmem handle\n");
        status = NV_ERR_INVALID_OFFSET;
        goto fail;
    }

    if (pParams->offset != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "offset passed for attach mem must be zero\n");
        status = NV_ERR_INVALID_OFFSET;
        goto fail;
    }

    if (pParams->mapLength == 0)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid mapLength passed\n");
        status = NV_ERR_INVALID_ARGUMENT;
        goto fail;
    }

    if ((pParams->offset >= pMulticastFabricDesc->allocSize) ||
        (pParams->mapLength > (pMulticastFabricDesc->allocSize - pParams->offset)))
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid multicast offset passed\n");
        status = NV_ERR_INVALID_OFFSET;
        goto fail;
    }

    if (pParams->flags != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "flags passed for attach mem must be zero\n");
        status = NV_ERR_INVALID_ARGUMENT;
        goto fail;
    }

    status = pRmApi->DupObject(pRmApi, pFabricVAS->hClient, pFabricVAS->hDevice,
                               &hDupedPhysMem, hClient, pParams->hMemory, 0);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to dup physical physical memory handle\n");
        goto fail;
    }

    status = _memMulticastFabricAttachMemInfoAddUnderLock(pMulticastFabricDesc,
                 pGpu, pParams, pPhysMemDesc, hDupedPhysMem, gpuProbeHandle,
                 &pAttachMemNode);

    rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);

    if (status != NV_OK)
        goto addInfoFail;

    if (pMulticastFabricDesc->numAttachedGpus == pMulticastFabricDesc->numMaxGpus)
    {
        status = _memMulticastFabricSendInbandRequestUnderLock(pMulticastFabricDesc,
                                                               MEM_MULTICAST_FABRIC_TEAM_SETUP_REQUEST);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Inband request submission to FM for Multicast Team Setup failed!\n");
            goto inbandFail;
        }
    }

    return NV_OK;

goto inbandFail;
inbandFail:
    _memMulticastFabricAttachMemInfoRemoveUnderLock(pMulticastFabricDesc, pAttachMemNode);

addInfoFail:
    // Take all GPU lock. This is enforced by the vidmem class
    NV_ASSERT_OK(rmGpuLocksAcquire(GPUS_LOCK_FLAGS_NONE, RM_LOCK_MODULES_MEM_FLA));
    NV_ASSERT(pRmApi->Free(pRmApi, pFabricVAS->hClient, hDupedPhysMem) == NV_OK);
    rmGpuLocksRelease(GPUS_LOCK_FLAGS_NONE, NULL);

fail:
    if (rmDeviceGpuLockIsOwner(pGpu->gpuInstance))
        rmDeviceGpuLocksRelease(pGpu, GPUS_LOCK_FLAGS_NONE, NULL);

    return status;
}

void
memorymulticastfabricDestruct_IMPL
(
    MemoryMulticastFabric *pMemoryMulticastFabric
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());

    fabricMulticastFabricOpsMutexAcquire(pFabric);

    _memorymulticastfabricDestructUnderLock(pMemoryMulticastFabric);

    fabricMulticastFabricOpsMutexRelease(pFabric);
}

NvBool
memorymulticastfabricCanCopy_IMPL
(
    MemoryMulticastFabric *pMemoryMulticastFabric
)
{
    return NV_TRUE;
}

//
// memorymulticastfabricCopyConstruct_IMPL does not take
// the multicast fabric mutex lock as it can be invoked
// with the GPU locks held which would result in a lock
// inversion. Hence we resort to portAtomics here.
//
NV_STATUS
memorymulticastfabricCopyConstruct_IMPL
(
    MemoryMulticastFabric        *pMemoryMulticastFabric,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc;

    MemoryMulticastFabric *pSourceMemoryMulticastFabric =
        dynamicCast(pParams->pSrcRef->pResource, MemoryMulticastFabric);

    pMulticastFabricDesc = pSourceMemoryMulticastFabric->pMulticastFabricDesc;

    pMemoryMulticastFabric->pMulticastFabricDesc = pMulticastFabricDesc;

    portAtomicExIncrementU64(&pMulticastFabricDesc->refCount);

    return NV_OK;
}

NV_STATUS
memorymulticastfabricCtrlGetInfo_IMPL
(
    MemoryMulticastFabric       *pMemoryMulticastFabric,
    NV00FD_CTRL_GET_INFO_PARAMS *pParams
)
{
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc;

    pMulticastFabricDesc = pMemoryMulticastFabric->pMulticastFabricDesc;

    pParams->alignment       = pMulticastFabricDesc->alignment;
    pParams->allocSize       = pMulticastFabricDesc->allocSize;
    pParams->pageSize        = pMulticastFabricDesc->pageSize;
    pParams->numMaxGpus      = pMulticastFabricDesc->numMaxGpus;
    pParams->numAttachedGpus = pMulticastFabricDesc->numAttachedGpus;

    return NV_OK;
}

//
// memorymulticastfabricIsReady_IMPL does not take
// the multicast fabric mutex lock as it can be invoked
// with the GPU locks held which would result in a lock
// inversion. Hence we resort to portAtomics here.
//
NV_STATUS
memorymulticastfabricIsReady_IMPL
(
    MemoryMulticastFabric *pMemoryMulticastFabric,
    NvBool                 bCopyConstructorContext
)
{
    Memory *pMemory = staticCast(pMemoryMulticastFabric, Memory);
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc;
    NV_STATUS mcTeamStatus;

    pMulticastFabricDesc = pMemoryMulticastFabric->pMulticastFabricDesc;

    mcTeamStatus = portAtomicOrU32(&pMulticastFabricDesc->mcTeamStatus, 0);

    if (bCopyConstructorContext && (mcTeamStatus == NV_ERR_NOT_READY))
        return NV_OK;

    if (pMemory->pMemDesc != pMulticastFabricDesc->pMemDesc)
    {
        // This function only initializes pMemory so it should never fail.
        NV_ASSERT_OK(memConstructCommon(pMemory,
                                        NV_MEMORY_MULTICAST_FABRIC,
                                        0, pMulticastFabricDesc->pMemDesc,
                                        0, NULL, 0, 0, 0, 0,
                                        NVOS32_MEM_TAG_NONE, NULL));
    }

    return mcTeamStatus;
}

NV_STATUS
memorymulticastfabricCtrlRegisterEvent_IMPL
(
    MemoryMulticastFabric             *pMemoryMulticastFabric,
    NV00FD_CTRL_REGISTER_EVENT_PARAMS *pParams
)
{
    Memory    *pMemory = staticCast(pMemoryMulticastFabric, Memory);
    NvHandle   hClient = RES_GET_CLIENT_HANDLE(pMemoryMulticastFabric);

    return _memMulticastFabricDescriptorEnqueueWaitUnderLock(hClient,
           pMemoryMulticastFabric->pMulticastFabricDesc,
           pParams->pOsEvent, pMemory);
}

NV_STATUS
memorymulticastfabricControl_Prologue_IMPL
(
    MemoryMulticastFabric          *pMemoryMulticastFabric,
    CALL_CONTEXT                   *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    RmResource *pResource = staticCast(pMemoryMulticastFabric, RmResource);

    // Other control calls, nothing to be validated.
    if (pParams->cmd != NV00FD_CTRL_CMD_ATTACH_MEM)
        return rmresControl_Prologue_IMPL(pResource, pCallContext, pParams);

    return _memMulticastFabricGpuOsInfoAddUnderLock(pMemoryMulticastFabric, pParams);
}

NV_STATUS
memorymulticastfabricControl_IMPL
(
    MemoryMulticastFabric          *pMemoryMulticastFabric,
    CALL_CONTEXT                   *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    NV_STATUS status = NV_OK;

    if (pParams->cmd != NV00FD_CTRL_CMD_ATTACH_MEM)
    {
        status = memorymulticastfabricIsReady(pMemoryMulticastFabric, NV_FALSE);
    }

    //
    // If clients try to register when the multicast object
    // is ready, then there is nothing left to do as the memory
    // descriptor is already installed.
    //
    // If the status is NV_ERR_NOT_READY then we are yet to
    // receive the inband reponse and we register the event.
    //
    if (pParams->cmd == NV00FD_CTRL_CMD_REGISTER_EVENT)
    {
        if (status == NV_OK)
            return NV_WARN_NOTHING_TO_DO;

        if (status != NV_ERR_NOT_READY)
            return status;
    }
    else
    {
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, status);
    }

    fabricMulticastFabricOpsMutexAcquire(pFabric);

    status = resControl_IMPL(staticCast(pMemoryMulticastFabric, RsResource),
                             pCallContext, pParams);

    fabricMulticastFabricOpsMutexRelease(pFabric);

    return status;
}

//
// memorymulticastfabricIsGpuMapAllowed_IMPL does not take
// the multicast fabric mutex lock as it can be invoked
// with the GPU locks held which would result in a lock
// inversion. Hence we resort to portAtomics here.
//
NvBool
memorymulticastfabricIsGpuMapAllowed_IMPL
(
    MemoryMulticastFabric *pMemoryMulticastFabric,
    OBJGPU                *pGpu
)
{
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc;
    NvU32 attachedGpusMask;

    pMulticastFabricDesc = pMemoryMulticastFabric->pMulticastFabricDesc;

    attachedGpusMask = portAtomicOrU32(&pMulticastFabricDesc->attachedGpusMask, 0);

    return ((attachedGpusMask & NVBIT32(pGpu->gpuInstance)) != 0U);
}

NV_STATUS
memorymulticastfabricGetMapAddrSpace_IMPL
(
    MemoryMulticastFabric *pMemoryMulticastFabric,
    CALL_CONTEXT          *pCallContext,
    NvU32                  mapFlags,
    NV_ADDRESS_SPACE      *pAddrSpace
)
{
    *pAddrSpace = ADDR_FABRIC_MC;
    return NV_OK;
}
