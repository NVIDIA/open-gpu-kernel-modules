/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

    listInit(&pMulticastFabricDesc->waitingClientsList,
             portMemAllocatorGetGlobalNonPaged());

    listInit(&pMulticastFabricDesc->gpuInfoList,
             portMemAllocatorGetGlobalNonPaged());

    pMulticastFabricDesc->refCount = 1;
    pMulticastFabricDesc->mcTeamStatus = NV_ERR_NOT_READY;
    pMulticastFabricDesc->attachedGpusMask = 0;
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
_memMulticastFabricGpuInfoAddUnderLock
(
    MemoryMulticastFabric          *pMemoryMulticastFabric,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    NV00FD_CTRL_ATTACH_GPU_PARAMS *pAttachParams = pParams->pParams;
    Subdevice *pSubdevice = NULL;
    MEM_MULTICAST_FABRIC_GPU_INFO *pNode;
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc = \
        pMemoryMulticastFabric->pMulticastFabricDesc;
    OBJGPU *pGpu;
    MEM_MULTICAST_FABRIC_GPU_INFO *pNodeItr;

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        subdeviceGetByHandle(RES_GET_CLIENT(pMemoryMulticastFabric),
            pAttachParams->hSubdevice, &pSubdevice));

    pGpu = GPU_RES_GET_GPU(pSubdevice);

    if(!osMatchGpuOsInfo(pGpu, pParams->secInfo.gpuOsInfo))
        return NV_ERR_INVALID_DEVICE;

    for (pNodeItr = listHead(&pMulticastFabricDesc->gpuInfoList);
         pNodeItr != NULL;
         pNodeItr = listNext(&pMulticastFabricDesc->gpuInfoList, pNodeItr))
    {
        if (pNodeItr->pGpu == pGpu)
        {
           NV_PRINTF(LEVEL_ERROR, "GPU %x has already attached\n",
                                   pGpu->gpuInstance);
           return NV_ERR_IN_USE;
        }
    }

    pNode = listAppendNew(&pMulticastFabricDesc->gpuInfoList);
    if (pNode == NULL)
        return NV_ERR_NO_MEMORY;

    pNode->pGpuOsInfo = pParams->secInfo.gpuOsInfo;
    pNode->pGpu = GPU_RES_GET_GPU(pSubdevice);
    pNode->pAttachMemInfoTree = NULL;
    pNode->bMcflaAlloc = NV_FALSE;

    return NV_OK;
}

static void
_memMulticastFabricGpuInfoRemoveUnderLock
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc
)
{
    MEM_MULTICAST_FABRIC_GPU_INFO *pNode = NULL;
    THREAD_STATE_NODE *pThreadNode = NULL;
    THREAD_STATE_FREE_CALLBACK freeCallback;

    NV_ASSERT_OK(threadStateGetCurrent(&pThreadNode, NULL));

    while ((pNode = listHead(&pMulticastFabricDesc->gpuInfoList)) != NULL)
    {
        freeCallback.pCb = osReleaseGpuOsInfo;
        freeCallback.pCbData = (void *)pNode->pGpuOsInfo;

        NV_ASSERT_OK(threadStateEnqueueCallbackOnFree(pThreadNode, &freeCallback));
        listRemove(&pMulticastFabricDesc->gpuInfoList, pNode);
    }

    pMulticastFabricDesc->attachedGpusMask = 0;
    pMulticastFabricDesc->numAttachedGpus = 0;
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
    MEM_MULTICAST_FABRIC_GPU_INFO *pNode;
    NvU64 requestId;
    NvU32 idx = 0;
    NvU32 payloadSize;
    NvU32 sendDataSize;
    NV_STATUS status = NV_OK;

    sendDataParams = \
        (NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS *)
        portMemAllocNonPaged(sizeof(NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS));

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

    for (pNode = listHead(&pMulticastFabricDesc->gpuInfoList);
         pNode != NULL;
         pNode = listNext(&pMulticastFabricDesc->gpuInfoList, pNode))
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

NV_STATUS
_memMulticastFabricSendInbandTeamSetupRequestUnderlock
(
    OBJGPU                          *pGpu,
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc
)
{
    NvU64 fmCaps;
    NV_STATUS status = NV_OK;

    status = gpuFabricProbeGetfmCaps(pGpu->pGpuFabricProbeInfoKernel, &fmCaps);
    if (status != NV_OK)
        return status;

    if (!(fmCaps & NVLINK_INBAND_FM_CAPS_MC_TEAM_SETUP_V1))
        return NV_ERR_NOT_SUPPORTED;

    return _memMulticastFabricSendInbandTeamSetupRequestV1UnderLock(pGpu,
                                                                    pMulticastFabricDesc);
}

NV_STATUS
_memMulticastFabricSendInbandTeamReleaseRequestUnderLock
(
    OBJGPU *pGpu,
    NvU64   mcTeamHandle
)
{
    NvU64 fmCaps;
    NV_STATUS status = NV_OK;

    status = gpuFabricProbeGetfmCaps(pGpu->pGpuFabricProbeInfoKernel, &fmCaps);
    if (status != NV_OK)
        return status;

    if (!(fmCaps & NVLINK_INBAND_FM_CAPS_MC_TEAM_RELEASE_V1))
        return NV_ERR_NOT_SUPPORTED;

    return _memMulticastFabricSendInbandTeamReleaseRequestV1UnderLock(pGpu,
                                                                      mcTeamHandle);
}

NV_STATUS
_memMulticastFabricSendInbandRequestUnderLock
(
    OBJGPU                            *pGpu,
    MEM_MULTICAST_FABRIC_DESCRIPTOR   *pMulticastFabricDesc,
    MEM_MULTICAST_FABRIC_REQUEST_TYPE  requestType
)
{
    NV_STATUS status = NV_OK;

    // If pGpu is NULL, pick the first one attached to the object.
    if (pGpu == NULL)
        pGpu = listHead(&pMulticastFabricDesc->gpuInfoList)->pGpu;

    switch (requestType)
    {
        case MEM_MULTICAST_FABRIC_TEAM_SETUP_REQUEST:
            status = _memMulticastFabricSendInbandTeamSetupRequestUnderlock(pGpu,
                                                                pMulticastFabricDesc);
            break;
        case MEM_MULTICAST_FABRIC_TEAM_RELEASE_REQUEST:
            status = _memMulticastFabricSendInbandTeamReleaseRequestUnderLock(pGpu,
                                                   pMulticastFabricDesc->mcTeamHandle);
            break;
        default:
            status = NV_ERR_NOT_SUPPORTED;
            break;
    }

    return status;
}

static void
_memorymulticastfabricDetachMem
(
    FABRIC_VASPACE     *pFabricVAS,
    MEMORY_DESCRIPTOR  *pFabricMemDesc,
    NODE               *pMemNode
)
{
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    MEMORY_DESCRIPTOR *pPhysMemDesc;
    MEM_MULTICAST_FABRIC_ATTACH_MEM_INFO_NODE *pAttachMemInfoNode;

    pAttachMemInfoNode = \
        (MEM_MULTICAST_FABRIC_ATTACH_MEM_INFO_NODE *)pMemNode->Data;
    pPhysMemDesc = pAttachMemInfoNode->pPhysMemDesc;

    fabricvaspaceUnmapPhysMemdesc(pFabricVAS, pFabricMemDesc,
                                  pMemNode->keyStart,
                                  pPhysMemDesc,
                                  pAttachMemInfoNode->physMapLength);

    NV_ASSERT_OK(pRmApi->Free(pRmApi, pFabricVAS->hClient,
                              pAttachMemInfoNode->hDupedPhysMem));
}

static void
_memorymulticastfabricBatchDetachMem
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc
)
{
    MEMORY_DESCRIPTOR *pFabricMemDesc;
    MEM_MULTICAST_FABRIC_GPU_INFO *pGpuNode;
    NODE *pMemNode;
    FABRIC_VASPACE *pFabricVAS;

    pFabricMemDesc = pMulticastFabricDesc->pMemDesc;
    NV_ASSERT_OR_RETURN_VOID(pFabricMemDesc != NULL);

    for (pGpuNode = listHead(&pMulticastFabricDesc->gpuInfoList);
         pGpuNode != NULL;
         pGpuNode = listNext(&pMulticastFabricDesc->gpuInfoList, pGpuNode))
    {
        pFabricVAS = dynamicCast(pGpuNode->pGpu->pFabricVAS, FABRIC_VASPACE);
        if (pFabricVAS == NULL)
        {
            NV_ASSERT(0);
            continue;
        }

        btreeEnumStart(0, &pMemNode, pGpuNode->pAttachMemInfoTree);
        while (pMemNode != NULL)
        {
            _memorymulticastfabricDetachMem(pFabricVAS, pFabricMemDesc, pMemNode);

            btreeUnlink(pMemNode, &pGpuNode->pAttachMemInfoTree);
            portMemFree(pMemNode->Data);

            btreeEnumStart(0, &pMemNode, pGpuNode->pAttachMemInfoTree);
        }

        // Everything is detached during object cleanup, free MCFLA now.
        if (pGpuNode->bMcflaAlloc)
        {
            fabricvaspaceBatchFree(pFabricVAS, &pFabricMemDesc->_pteArray[0],
                                   1, 1);
            pGpuNode->bMcflaAlloc = NV_FALSE;
        }
    }
}

static void
_memMulticastFabricDescriptorFreeUnderLock
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc
)
{
    if (pMulticastFabricDesc == NULL)
        return;

    pMulticastFabricDesc->refCount--;

    if (pMulticastFabricDesc->refCount == 0)
    {
        if (pMulticastFabricDesc->pMemDesc != NULL)
        {
            NV_ASSERT(pMulticastFabricDesc->bMemdescInstalled);

            _memorymulticastfabricBatchDetachMem(pMulticastFabricDesc);

            _memMulticastFabricSendInbandRequestUnderLock(NULL, pMulticastFabricDesc,
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

        _memMulticastFabricGpuInfoRemoveUnderLock(pMulticastFabricDesc);

        NV_ASSERT(listCount(&pMulticastFabricDesc->gpuInfoList) == 0);
        listDestroy(&pMulticastFabricDesc->gpuInfoList);

        NV_ASSERT(pMulticastFabricDesc->numAttachedGpus == 0);
        NV_ASSERT(pMulticastFabricDesc->attachedGpusMask == 0);

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
    pMulticastFabricDesc->mcTeamStatus = status;

    _memMulticastFabricDescriptorFlushClientsUnderLock(pMulticastFabricDesc);
}

static NV_STATUS
_memorymulticastFabricAllocVasUnderLock
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc,
    MEMORY_DESCRIPTOR               *pFabricMemDesc
)
{
    NV_STATUS status = NV_OK;
    FABRIC_VASPACE *pFabricVAS;
    MEM_MULTICAST_FABRIC_GPU_INFO *pGpuInfo;
    VAS_ALLOC_FLAGS flags = { 0 };
    NvU64 gpuProbeHandle;

    for (pGpuInfo = listHead(&pMulticastFabricDesc->gpuInfoList);
         pGpuInfo != NULL;
         pGpuInfo = listNext(&pMulticastFabricDesc->gpuInfoList, pGpuInfo))
    {
        OBJGPU *pGpu = pGpuInfo->pGpu;

        pFabricVAS = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);
        if (pFabricVAS == NULL)
        {
            status = NV_ERR_INVALID_STATE;
            goto cleanup;
        }

        //
        // The fabric handle might not be available or have changed, if fabric
        // state was ever invalidated while MCFLA allocation was in progress.
        //
        status = gpuFabricProbeGetGpuFabricHandle(pGpu->pGpuFabricProbeInfoKernel,
                                                  &gpuProbeHandle);
        if ((status != NV_OK) || (pGpuInfo->gpuProbeHandle != gpuProbeHandle))
        {
            NV_PRINTF(LEVEL_ERROR, "Attached GPU's probe handle is stale\n");
            status = NV_ERR_INVALID_DEVICE;
            goto cleanup;
        }

        status = fabricvaspaceAllocMulticast(pFabricVAS,
                                    memdescGetPageSize(pFabricMemDesc, AT_GPU),
                                    pMulticastFabricDesc->alignment,
                                    flags, pFabricMemDesc->_pteArray[0],
                                    pMulticastFabricDesc->allocSize);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Fabric VA space alloc failed for GPU %d\n",
                      pGpuInfo->pGpu->gpuInstance);
            goto cleanup;
        }

        pGpuInfo->bMcflaAlloc = NV_TRUE;
    }

    return NV_OK;

cleanup:
    for (pGpuInfo = listHead(&pMulticastFabricDesc->gpuInfoList);
         pGpuInfo != NULL;
         pGpuInfo = listNext(&pMulticastFabricDesc->gpuInfoList, pGpuInfo))
    {
        if (pGpuInfo->bMcflaAlloc)
        {
            pFabricVAS = dynamicCast(pGpuInfo->pGpu->pFabricVAS, FABRIC_VASPACE);

            fabricvaspaceBatchFree(pFabricVAS, &pFabricMemDesc->_pteArray[0],
                                   1, 1);

            pGpuInfo->bMcflaAlloc = NV_FALSE;
        }
    }

    return status;
}

NV_STATUS
_memMulticastFabricAttachGpuPostProcessorUnderLock
(
    OBJGPU                          *pGpu,
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
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to allocate fabric memdesc\n");
        goto installMemDesc;
    }

    status = _memorymulticastFabricAllocVasUnderLock(pMulticastFabricDesc,
                                                     pMemDesc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to allocate fabric VAS\n");
        memdescDestroy(pMemDesc);
        pMemDesc = NULL;
        goto installMemDesc;
    }

installMemDesc:
    _memMulticastFabricInstallMemDescUnderLock(pMulticastFabricDesc,
                                               pMemDesc,
                                               mcTeamHandle,
                                               status);

     if ((status != NV_OK) && (mcTeamStatus == NV_OK))
         _memMulticastFabricSendInbandRequestUnderLock(pGpu, pMulticastFabricDesc,
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

NV_STATUS
memorymulticastfabricTeamSetupResponseCallback
(
    NvU32                                           gpuInstance,
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
    OBJGPU *pGpu;

    NV_ASSERT(pInbandRcvParams != NULL);
    NV_ASSERT(rmGpuLockIsOwner());

    if ((pGpu = gpumgrGetGpu(gpuInstance)) == NULL)
    {
        NV_ASSERT_FAILED("Invalid GPU instance");
        return NV_ERR_INVALID_ARGUMENT;
    }

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

        (void)_memMulticastFabricAttachGpuPostProcessorUnderLock(pGpu,
                                                                 pMulticastFabricDesc,
                                                                 mcTeamStatus,
                                                                 mcTeamHandle,
                                                                 mcAddressBase,
                                                                 mcAddressSize);
    }
    else
    {
        OS_WAIT_QUEUE *pWq;

        if (mcTeamStatus == NV_OK)
            (void)_memMulticastFabricSendInbandTeamReleaseRequestUnderLock(pGpu,
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

    return NV_OK;
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
memorymulticastfabricCtrlAttachGpu_IMPL
(
    MemoryMulticastFabric         *pMemoryMulticastFabric,
    NV00FD_CTRL_ATTACH_GPU_PARAMS *pParams
)
{
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc = \
                                pMemoryMulticastFabric->pMulticastFabricDesc;
    NV_STATUS status = NV_OK;
    Subdevice *pSubdevice;
    OBJGPU *pGpu;
    FABRIC_VASPACE *pFabricVAS;
    NvU64 gpuProbeHandle;
    MEM_MULTICAST_FABRIC_GPU_INFO *pNode = \
                                listTail(&pMulticastFabricDesc->gpuInfoList);

    if (pParams->flags != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "flags passed for attach mem must be zero\n");
        status = NV_ERR_INVALID_ARGUMENT;
        goto fail;
    }

    // Check if the Multicast FLA object has any additional slots for GPUs
    if (pMulticastFabricDesc->numAttachedGpus == pMulticastFabricDesc->numMaxGpus)
    {
        NV_PRINTF(LEVEL_ERROR, "Max no. of GPUs have already attached!\n");
        return NV_ERR_INVALID_OPERATION;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
        subdeviceGetByHandle(RES_GET_CLIENT(pMemoryMulticastFabric),
            pParams->hSubdevice, &pSubdevice));

    pGpu = GPU_RES_GET_GPU(pSubdevice);

    if (RMCFG_FEATURE_PLATFORM_WINDOWS ||
        gpuIsCCFeatureEnabled(pGpu) ||
        IS_VIRTUAL(pGpu))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Multicast attach not supported on Windows/CC/vGPU modes\n");
        status = NV_ERR_NOT_SUPPORTED;
        goto fail;
    }

    status = gpuFabricProbeGetGpuFabricHandle(pGpu->pGpuFabricProbeInfoKernel,
                                              &gpuProbeHandle);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Attaching GPU does not have a valid probe handle\n");
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

    pNode->gpuProbeHandle = gpuProbeHandle;

    if ((pMulticastFabricDesc->numAttachedGpus + 1)  == pMulticastFabricDesc->numMaxGpus)
    {
        status = _memMulticastFabricSendInbandRequestUnderLock(NULL, pMulticastFabricDesc,
                                            MEM_MULTICAST_FABRIC_TEAM_SETUP_REQUEST);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Inband request submission to FM for Multicast Team Setup failed!\n");
            goto fail;
        }
    }

    pMulticastFabricDesc->numAttachedGpus++;
    pMulticastFabricDesc->attachedGpusMask |= NVBIT32(pGpu->gpuInstance);

    return NV_OK;

fail:
    // Remove GPU OS info added in the prologue.
    listRemove(&pMulticastFabricDesc->gpuInfoList, pNode);

    return status;
}

static MEM_MULTICAST_FABRIC_GPU_INFO*
_memorymulticastfabricGetAttchedGpuInfo
(
    MemoryMulticastFabric  *pMemoryMulticastFabric,
    NvHandle                hSubdevice
)
{
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc = \
                                pMemoryMulticastFabric->pMulticastFabricDesc;
    MEM_MULTICAST_FABRIC_GPU_INFO *pNodeItr;
    Subdevice *pSubdevice = NULL;
    NV_STATUS status;

    status = subdeviceGetByHandle(RES_GET_CLIENT(pMemoryMulticastFabric),
                                  hSubdevice, &pSubdevice);
    if (status != NV_OK)
        return NULL;

    for (pNodeItr = listHead(&pMulticastFabricDesc->gpuInfoList);
         pNodeItr != NULL;
         pNodeItr = listNext(&pMulticastFabricDesc->gpuInfoList, pNodeItr))
    {
        if (pNodeItr->pGpu == GPU_RES_GET_GPU(pSubdevice))
            return pNodeItr;
    }

    return NULL;
}

NV_STATUS
memorymulticastfabricCtrlDetachMem_IMPL
(
    MemoryMulticastFabric         *pMemoryMulticastFabric,
    NV00FD_CTRL_DETACH_MEM_PARAMS *pParams
)
{
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc = \
                                pMemoryMulticastFabric->pMulticastFabricDesc;
    MEM_MULTICAST_FABRIC_GPU_INFO *pGpuInfo;
    NODE *pNode;
    MEMORY_DESCRIPTOR *pFabricMemDesc;
    FABRIC_VASPACE *pFabricVAS;
    NV_STATUS status;

    if (pParams->flags != 0)
        return NV_ERR_INVALID_ARGUMENT;

    pGpuInfo = _memorymulticastfabricGetAttchedGpuInfo(pMemoryMulticastFabric,
                                                       pParams->hSubdevice);
    if (pGpuInfo == NULL)
        return NV_ERR_INVALID_DEVICE;

    status = btreeSearch(pParams->offset, &pNode, pGpuInfo->pAttachMemInfoTree);
    if (status != NV_OK)
        return status;

    pFabricMemDesc = pMulticastFabricDesc->pMemDesc;
    NV_ASSERT_OR_RETURN(pFabricMemDesc != NULL, NV_ERR_INVALID_STATE);

    pFabricVAS = dynamicCast(pGpuInfo->pGpu->pFabricVAS, FABRIC_VASPACE);
    NV_ASSERT_OR_RETURN(pFabricVAS != NULL, NV_ERR_INVALID_STATE);

    _memorymulticastfabricDetachMem(pFabricVAS, pFabricMemDesc, pNode);

    btreeUnlink(pNode, &pGpuInfo->pAttachMemInfoTree);
    portMemFree(pNode->Data);

    return NV_OK;
}

static NV_STATUS
_memorymulticastfabricValidatePhysMem
(
    MemoryMulticastFabric *pMemoryMulticastFabric,
    NvHandle               hPhysMem,
    OBJGPU                *pAttachedGpu,
    MEMORY_DESCRIPTOR    **ppPhysMemDesc
)
{
    RsResourceRef *pPhysmemRef;
    MEMORY_DESCRIPTOR *pPhysMemDesc;
    NvU64 physPageSize;
    NV_STATUS status;

    status = serverutilGetResourceRef(RES_GET_CLIENT_HANDLE(pMemoryMulticastFabric),
                                      hPhysMem, &pPhysmemRef);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to get resource in resserv for physmem handle\n");

        return status;
    }

    pPhysMemDesc = (dynamicCast(pPhysmemRef->pResource, Memory))->pMemDesc;

    if (memdescGetAddressSpace(pPhysMemDesc) != ADDR_FBMEM ||
        (pAttachedGpu != pPhysMemDesc->pGpu))
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid physmem handle passed\n");

        return NV_ERR_INVALID_ARGUMENT;
    }

    physPageSize = memdescGetPageSize(pPhysMemDesc, AT_GPU);
    if ((physPageSize != RM_PAGE_SIZE_HUGE) &&
        (physPageSize != RM_PAGE_SIZE_512M))
    {
        NV_PRINTF(LEVEL_ERROR, "Physmem page size should be 2MB\n");

        return NV_ERR_INVALID_ARGUMENT;
    }

    *ppPhysMemDesc = pPhysMemDesc;

    return NV_OK;
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
    MEM_MULTICAST_FABRIC_GPU_INFO *pGpuInfo;
    NV_STATUS status;
    MEMORY_DESCRIPTOR *pPhysMemDesc;
    MEMORY_DESCRIPTOR *pFabricMemDesc;
    NvHandle hDupedPhysMem = 0;
    RM_API *pRmApi = rmapiGetInterface(RMAPI_GPU_LOCK_INTERNAL);
    FABRIC_VASPACE *pFabricVAS;
    MEM_MULTICAST_FABRIC_ATTACH_MEM_INFO_NODE *pNode;

    if (pParams->flags != 0)
        return NV_ERR_INVALID_ARGUMENT;

    pGpuInfo = _memorymulticastfabricGetAttchedGpuInfo(pMemoryMulticastFabric,
                                                       pParams->hSubdevice);
    if (pGpuInfo == NULL)
        return NV_ERR_INVALID_DEVICE;

    status = _memorymulticastfabricValidatePhysMem(pMemoryMulticastFabric,
                                                   pParams->hMemory,
                                                   pGpuInfo->pGpu,
                                                   &pPhysMemDesc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to validate physmem handle\n");
        return status;
    }

    pFabricVAS = dynamicCast(pGpuInfo->pGpu->pFabricVAS, FABRIC_VASPACE);
    NV_ASSERT_OR_RETURN(pFabricVAS != NULL, NV_ERR_INVALID_STATE);

    pFabricMemDesc = pMulticastFabricDesc->pMemDesc;
    NV_ASSERT_OR_RETURN(pFabricMemDesc != NULL, NV_ERR_INVALID_STATE);

    NV_ASSERT_OR_RETURN(pGpuInfo->bMcflaAlloc, NV_ERR_INVALID_STATE);

    status = pRmApi->DupObject(pRmApi, pFabricVAS->hClient,
                               pFabricVAS->hDevice, &hDupedPhysMem,
                               RES_GET_CLIENT_HANDLE(pMemoryMulticastFabric),
                               pParams->hMemory, 0);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to dup physmem handle\n");
        return status;
    }

    status = fabricvaspaceMapPhysMemdesc(pFabricVAS,
                                         pFabricMemDesc,
                                         pParams->offset,
                                         pPhysMemDesc,
                                         pParams->mapOffset,
                                         pParams->mapLength,
                                         0);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to map FLA\n");
        goto freeDupedMem;
    }

    pNode = portMemAllocNonPaged(sizeof(*pNode));
    if (pNode == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto unmapVas;
    }

    portMemSet(pNode, 0, sizeof(*pNode));

    pNode->node.keyStart = pParams->offset;
    pNode->node.keyEnd   = pParams->offset;
    pNode->physMapLength = pParams->mapLength;
    pNode->pPhysMemDesc  = pPhysMemDesc;
    pNode->hDupedPhysMem = hDupedPhysMem;
    pNode->node.Data     = pNode;

    status = btreeInsert(&pNode->node, &pGpuInfo->pAttachMemInfoTree);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to track attach mem info\n");
        goto freeNode;
    }

    return NV_OK;

freeNode:
    portMemFree(pNode);

unmapVas:
    fabricvaspaceUnmapPhysMemdesc(pFabricVAS, pFabricMemDesc, pParams->offset,
                                  pPhysMemDesc, pParams->mapLength);

freeDupedMem:
    NV_ASSERT_OK(pRmApi->Free(pRmApi, pFabricVAS->hClient, hDupedPhysMem));

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

NV_STATUS
memorymulticastfabricCopyConstruct_IMPL
(
    MemoryMulticastFabric        *pMemoryMulticastFabric,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc;
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());

    fabricMulticastFabricOpsMutexAcquire(pFabric);

    MemoryMulticastFabric *pSourceMemoryMulticastFabric =
        dynamicCast(pParams->pSrcRef->pResource, MemoryMulticastFabric);

    pMulticastFabricDesc = pSourceMemoryMulticastFabric->pMulticastFabricDesc;

    pMemoryMulticastFabric->pMulticastFabricDesc = pMulticastFabricDesc;

    pMulticastFabricDesc->refCount++;

    fabricMulticastFabricOpsMutexRelease(pFabric);

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

NV_STATUS
memorymulticastfabricIsReady_IMPL
(
    MemoryMulticastFabric *pMemoryMulticastFabric,
    NvBool                 bCopyConstructorContext
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    Memory *pMemory = staticCast(pMemoryMulticastFabric, Memory);
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc;
    NV_STATUS mcTeamStatus;

    fabricMulticastFabricOpsMutexAcquire(pFabric);

    pMulticastFabricDesc = pMemoryMulticastFabric->pMulticastFabricDesc;
    mcTeamStatus = pMulticastFabricDesc->mcTeamStatus;

    if (bCopyConstructorContext && (mcTeamStatus == NV_ERR_NOT_READY))
    {
        fabricMulticastFabricOpsMutexRelease(pFabric);
        return NV_OK;
    }

    if (pMemory->pMemDesc != pMulticastFabricDesc->pMemDesc)
    {
        // This function only initializes pMemory so it should never fail.
        NV_ASSERT_OK(memConstructCommon(pMemory,
                                        NV_MEMORY_MULTICAST_FABRIC,
                                        0, pMulticastFabricDesc->pMemDesc,
                                        0, NULL, 0, 0, 0, 0,
                                        NVOS32_MEM_TAG_NONE, NULL));
    }

    fabricMulticastFabricOpsMutexRelease(pFabric);

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
    if (pParams->cmd != NV00FD_CTRL_CMD_ATTACH_GPU)
        return rmresControl_Prologue_IMPL(pResource, pCallContext, pParams);

    return _memMulticastFabricGpuInfoAddUnderLock(pMemoryMulticastFabric, pParams);
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

    if (pParams->cmd != NV00FD_CTRL_CMD_ATTACH_GPU)
        status = memorymulticastfabricIsReady(pMemoryMulticastFabric, NV_FALSE);

    //
    // If clients try to register when the multicast object
    // is ready, then there is nothing left to do as the memory
    // descriptor is already installed.
    //
    // If the status is NV_ERR_NOT_READY then we are yet to
    // receive the inband response and we register the event.
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

NvBool
memorymulticastfabricIsGpuMapAllowed_IMPL
(
    MemoryMulticastFabric *pMemoryMulticastFabric,
    OBJGPU                *pGpu
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc;
    NvU32 attachedGpusMask;

    fabricMulticastFabricOpsMutexAcquire(pFabric);

    pMulticastFabricDesc = pMemoryMulticastFabric->pMulticastFabricDesc;

    attachedGpusMask = pMulticastFabricDesc->attachedGpusMask;

    fabricMulticastFabricOpsMutexRelease(pFabric);

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
