/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#define NVOC_MEM_MULTICAST_FABRIC_H_PRIVATE_ACCESS_ALLOWED

/*
 * Lock ordering
 *
 * RMAPI Lock
 * |_Client Lock
 *   |_GPU(s) Lock
 *     |_MCFLA Module Lock
 *       |_MCFLA Descriptor Lock
 */

#include "os/os.h"
#include "core/locks.h"
#include "nvport/nvport.h"
#include "rmapi/rs_utils.h"
#include "rmapi/rmapi_utils.h"
#include "resserv/rs_resource.h"
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

#include "class/cl00f9.h"
#include "mem_mgr/mem_export.h"

#include "gpu/gpu_fabric_probe.h"
#include "gpu_mgr/gpu_mgr.h"

typedef struct mem_multicast_fabric_attach_mem_info_node
{
    MEMORY_DESCRIPTOR *pPhysMemDesc;
    RsInterMapping    *pInterMapping;
    NvU64              physMapLength;
    NODE               node;
} MEM_MULTICAST_FABRIC_ATTACH_MEM_INFO_NODE;

typedef struct mem_multicast_fabric_client_info
{
    void   *pOsEvent;
    Memory *pMemory;
} MEM_MULTICAST_FABRIC_CLIENT_INFO;

typedef struct mem_multicast_fabric_gpu_info
{
    void   *pGpuOsInfo;
    OBJGPU *pGpu;
    NvU64   gpuProbeHandle;
    NvU32   cliqueId;
    NvBool  bMcflaAlloc;

    //
    // Unique import event ID. Valid only if the GPU was remotely attached to
    // the prime MCFLA object
    //
    NvU64 attachEventId;

    //
    // Tracks memory attached using NV00FD_CTRL_CMD_ATTACH_MEM
    //
    // GPU lock must be taken to protect this tree.
    //
    PNODE pAttachMemInfoTree;
} MEM_MULTICAST_FABRIC_GPU_INFO;

typedef struct mem_multicast_fabric_remote_gpu_info
{
    NvU32 cliqueId;
    NvU64 key;
} MEM_MULTICAST_FABRIC_REMOTE_GPU_INFO;

MAKE_MULTIMAP(MemMulticastFabricRemoteGpuInfoMap,
              MEM_MULTICAST_FABRIC_REMOTE_GPU_INFO);

MAKE_LIST(MemMulticastFabricClientInfoList, MEM_MULTICAST_FABRIC_CLIENT_INFO);

MAKE_LIST(MemMulticastFabricGpuInfoList, MEM_MULTICAST_FABRIC_GPU_INFO);

typedef enum
{
    MEM_MULTICAST_FABRIC_TEAM_SETUP_REQUEST = 0,
    MEM_MULTICAST_FABRIC_TEAM_RELEASE_REQUEST,
} MEM_MULTICAST_FABRIC_REQUEST_TYPE;

typedef struct mem_multicast_fabric_descriptor
{
    // Refcount to keep this descriptor alive
    NvU64 refCount;

    // List of clients waiting on this object to be ready
    MemMulticastFabricClientInfoList waitingClientsList;

    // Mask representing the list of attached GPUs
    NvU32 localAttachedGpusMask;

    // List of attached GPU info
    MemMulticastFabricGpuInfoList gpuInfoList;

    // Boolean to be set when pMemDesc is installed
    NvBool bMemdescInstalled;

    // Memory descriptor associated with the multicast object
    MEMORY_DESCRIPTOR *pMemDesc;

    // Unique handle assigned for the multicast team by FM
    NvU64 mcTeamHandle;

    // Status of the multicast team
    NV_STATUS mcTeamStatus;

    //
    // Boolean to be set when an Inband request has been sent to FM
    // and is currently in progress
    //
    NvBool bInbandReqInProgress;

    //
    // Request Id associated with the Inband request in progress when
    // bInbandReqSent is set to true
    //
    NvU64 inbandReqId;

    // Alignment for the multicast FLA allocation
    NvU64 alignment;

    // Multicast FLA allocation size
    NvU64 allocSize;

    // Page size for the multicast FLA
    NvU64 pageSize;

    // Multicast FLA allocation flags
    NvU32 allocFlags;

    // Max. number of unique GPUs associated with the multicast object
    NvU32 numMaxGpus;

    // No. of unique GPUs currently attached to the multicast object
    NvU32 numAttachedGpus;

    // Export object information associated with this import descriptor.
    NvU16   exportNodeId;
    NvU16   index;

    // Same as packet.uuid, but uses NvUuid type.
    NvUuid  expUuid;

    // Import cache key
    NvU64   cacheKey;

    // Map of attached remote GPU info
    MemMulticastFabricRemoteGpuInfoMap remoteGpuInfoMap;

    NvS32 imexChannel;

    //
    // The lock protects MEM_MULTICAST_FABRIC_DESCRIPTOR, the MCFLA descriptor.
    //
    // The lock should be taken only if an MCFLA descriptor is safe
    // to access i.e., holding the module lock or the accessing thread
    // has the MCFLA descriptor refcounted.
    //
    PORT_RWLOCK *pLock;
} MEM_MULTICAST_FABRIC_DESCRIPTOR;

static NvBool
_memMulticastFabricIsPrime
(
    NvU32 allocFlags
)
{
    NvBool bPrime = NV_TRUE;

#ifdef NV_MEMORY_MULTICAST_FABRIC_ALLOC_FLAGS_USE_EXPORT_PACKET
    //
    // If an MCFLA object is allocated using export packet (UUID), then it
    // is a non-prime (imported) object. Such objects are just extension
    // of the prime (exported) MCFLA objects.
    //
    bPrime = !(allocFlags &
               NV_MEMORY_MULTICAST_FABRIC_ALLOC_FLAGS_USE_EXPORT_PACKET);
#endif

    return bPrime;
}

static void
_memMulticastFabricInitAttachEvent
(
    NvU64                        gpuFabricProbeHandle,
    NvU64                        key,
    NvU32                        cliqueId,
    NvU16                        exportNodeId,
    NvU16                        index,
    NvUuid                      *pExportUuid,
    NvS32                        imexChannel,
    NV00F1_CTRL_FABRIC_EVENT    *pEvent
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());

    pEvent->imexChannel = imexChannel;
    pEvent->type = NV00F1_CTRL_FABRIC_EVENT_TYPE_REMOTE_GPU_ATTACH;
    pEvent->id = fabricGenerateEventId(pFabric);

    pEvent->data.attach.gpuFabricProbeHandle = gpuFabricProbeHandle;
    pEvent->data.attach.key = key;
    pEvent->data.attach.cliqueId = cliqueId;
    pEvent->data.attach.index = index;
    pEvent->data.attach.exportNodeId = exportNodeId;
    portMemCopy(pEvent->data.attach.exportUuid, NV_MEM_EXPORT_UUID_LEN,
                pExportUuid->uuid,              NV_MEM_EXPORT_UUID_LEN);
}

static void
_memMulticastFabricInitUnimportEvent
(
    NvU64                        attachEventId,
    NvU16                        exportNodeId,
    NvS32                        imexChannel,
    NV00F1_CTRL_FABRIC_EVENT    *pEvent
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());

    pEvent->imexChannel = imexChannel;
    pEvent->type = NV00F1_CTRL_FABRIC_EVENT_TYPE_MEM_UNIMPORT;
    pEvent->id = fabricGenerateEventId(pFabric);
    pEvent->data.unimport.exportNodeId  = exportNodeId;
    pEvent->data.unimport.importEventId = attachEventId;
}

static
NV_STATUS
_memMulticastFabricValidateAllocParams
(
    NV00FD_ALLOCATION_PARAMETERS *pAllocParams
)
{
    // Nothing to verify in this case..
    if (!_memMulticastFabricIsPrime(pAllocParams->allocFlags))
        return NV_OK;

    // Only page size 512MB is supported
    if (pAllocParams->pageSize != NV_MEMORY_MULTICAST_FABRIC_PAGE_SIZE_512M)
    {
        NV_PRINTF(LEVEL_ERROR,
            "Unsupported pageSize: 0x%x. Only 512MB pagesize is supported\n",
            pAllocParams->pageSize);
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (pAllocParams->alignment != pAllocParams->pageSize)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Alignment must be pageSize for now\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    // AllocSize should be page size aligned
    if (!NV_IS_ALIGNED64(pAllocParams->allocSize, pAllocParams->pageSize))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "AllocSize should be pageSize aligned\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    // NV_U32_MAX is reserved as a special value for non-prime MCFLA objects.
    if ((pAllocParams->numGpus == 0) ||
        (pAllocParams->numGpus == NV_U32_MAX))
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid number of GPUs to attach\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

static void
_memMulticastFabricDescriptorCleanup
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc
)
{
    if (pMulticastFabricDesc == NULL)
        return;

    NV_ASSERT(listCount(&pMulticastFabricDesc->gpuInfoList) == 0);
    listDestroy(&pMulticastFabricDesc->gpuInfoList);

    multimapDestroy(&pMulticastFabricDesc->remoteGpuInfoMap);

    NV_ASSERT(pMulticastFabricDesc->numAttachedGpus == 0);
    NV_ASSERT(pMulticastFabricDesc->localAttachedGpusMask == 0);

    NV_ASSERT(listCount(&pMulticastFabricDesc->waitingClientsList) == 0);
    listDestroy(&pMulticastFabricDesc->waitingClientsList);

    memdescDestroy(pMulticastFabricDesc->pMemDesc);

    if (pMulticastFabricDesc->pLock != NULL)
        portSyncRwLockDestroy(pMulticastFabricDesc->pLock);

    portMemFree(pMulticastFabricDesc);
}

static
MEM_MULTICAST_FABRIC_DESCRIPTOR*
_memMulticastFabricDescriptorAlloc
(
    MemoryMulticastFabric        *pMemoryMulticastFabric,
    NV00FD_ALLOCATION_PARAMETERS *pAllocParams
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc;
    NV_STATUS status;

    pMulticastFabricDesc = portMemAllocNonPaged(
                                sizeof(MEM_MULTICAST_FABRIC_DESCRIPTOR));
    if (pMulticastFabricDesc == NULL)
        return NULL;

    portMemSet(pMulticastFabricDesc, 0,
               sizeof(MEM_MULTICAST_FABRIC_DESCRIPTOR));

    listInit(&pMulticastFabricDesc->waitingClientsList,
             portMemAllocatorGetGlobalNonPaged());

    listInit(&pMulticastFabricDesc->gpuInfoList,
             portMemAllocatorGetGlobalNonPaged());

    pMulticastFabricDesc->refCount = 1;
    pMulticastFabricDesc->mcTeamStatus = NV_ERR_NOT_READY;
    pMulticastFabricDesc->localAttachedGpusMask = 0;
    pMulticastFabricDesc->alignment  = pAllocParams->alignment;
    pMulticastFabricDesc->allocSize  = pAllocParams->allocSize;
    pMulticastFabricDesc->pageSize   = pAllocParams->pageSize;
    pMulticastFabricDesc->allocFlags = pAllocParams->allocFlags;
    pMulticastFabricDesc->numMaxGpus = pAllocParams->numGpus;
    pMulticastFabricDesc->inbandReqId = osGetTimestamp();

    multimapInit(&pMulticastFabricDesc->remoteGpuInfoMap,
                 portMemAllocatorGetGlobalNonPaged());
    pMulticastFabricDesc->exportNodeId = NV_FABRIC_INVALID_NODE_ID;
    pMulticastFabricDesc->imexChannel = -1;

    pMulticastFabricDesc->pLock =
        portSyncRwLockCreate(portMemAllocatorGetGlobalNonPaged());
    if (pMulticastFabricDesc->pLock == NULL)
        goto fail;

    status = fabricMulticastSetupCacheInsert(pFabric,
                                        pMulticastFabricDesc->inbandReqId,
                                        pMulticastFabricDesc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to track memdesc 0x%x", status);
        goto fail;
    }

    return pMulticastFabricDesc;

fail:
    _memMulticastFabricDescriptorCleanup(pMulticastFabricDesc);

    return NULL;
}

static void
_memMulticastFabricDescriptorFlushClients
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc
)
{
    MEM_MULTICAST_FABRIC_CLIENT_INFO *pNode;

    while ((pNode =
            listHead(&pMulticastFabricDesc->waitingClientsList)) != NULL)
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
_memMulticastFabricDescriptorEnqueueWait
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
        status = osUserHandleToKernelPtr(hClient, pOsEvent,
                                         &pValidatedOsEvent);
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

    //
    // In case the multicast object's memdesc is ready, unblock
    // clients waiting on it
    //
    if (pMulticastFabricDesc->bMemdescInstalled)
    {
        _memMulticastFabricDescriptorFlushClients(pMulticastFabricDesc);
    }

    return NV_OK;
}

static void
_memMulticastFabricDescriptorDequeueWait
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

static NV_STATUS
_memMulticastFabricGpuInfoAdd
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
           return NV_ERR_INSERT_DUPLICATE_NAME;
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
_memMulticastFabricGpuInfoRemove
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
        //
        // Enqueue unimport event before the callback to release GPU.
        // This ordering is important.
        //
        if (!_memMulticastFabricIsPrime(pMulticastFabricDesc->allocFlags))
        {
            Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
            NV00F1_CTRL_FABRIC_EVENT unimportEvent;

            NV_ASSERT(pMulticastFabricDesc->imexChannel != -1);

            _memMulticastFabricInitUnimportEvent(pNode->attachEventId,
                                    pMulticastFabricDesc->exportNodeId,
                                    pMulticastFabricDesc->imexChannel,
                                    &unimportEvent);

            NV_CHECK(LEVEL_WARNING,
                fabricPostEventsV2(pFabric, &unimportEvent, 1) == NV_OK);
        }

        freeCallback.pCb = osReleaseGpuOsInfo;
        freeCallback.pCbData = (void *)pNode->pGpuOsInfo;

        NV_ASSERT_OK(threadStateEnqueueCallbackOnFree(pThreadNode,
                                                      &freeCallback));
        listRemove(&pMulticastFabricDesc->gpuInfoList, pNode);
    }

    pMulticastFabricDesc->localAttachedGpusMask = 0;
    pMulticastFabricDesc->numAttachedGpus = 0;
}

NV_STATUS
_memMulticastFabricSendInbandTeamSetupRequestV2
(
    OBJGPU                          *pGpu,
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc
)
{
    NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS *sendDataParams;
    nvlink_inband_mc_team_setup_req_v2_msg_t *pMcTeamSetupReqMsg = NULL;
    nvlink_inband_mc_team_setup_req_v2_t *pMcTeamSetupReq = NULL;
    MEM_MULTICAST_FABRIC_GPU_INFO *pNode;
    NvU32 idx = 0;
    NvU32 payloadSize;
    NvU32 sendDataSize;
    NV_STATUS status = NV_OK;
    NvU16 numKeys = 1;
    MemMulticastFabricRemoteGpuInfoMapSupermapIter smIter;

    sendDataParams =
        (NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS *)
        portMemAllocNonPaged(sizeof(NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS));

    if (sendDataParams == NULL)
        return NV_ERR_NO_MEMORY;

    pMcTeamSetupReqMsg =
        (nvlink_inband_mc_team_setup_req_v2_msg_t *)&sendDataParams->buffer[0];

    pMcTeamSetupReq =
        (nvlink_inband_mc_team_setup_req_v2_t *)&pMcTeamSetupReqMsg->mcTeamSetupReq;

    //
    // Submap of remoteGpuInfoMap represent a node. As there is a key/node,
    // count submap to calculate numKeys.
    //
    numKeys += multimapCountSubmaps(&pMulticastFabricDesc->remoteGpuInfoMap);

    payloadSize = (NvU32)(sizeof(nvlink_inband_mc_team_setup_req_v2_t) +
                         (sizeof(pMcTeamSetupReq->gpuHandlesAndKeys[0]) *
                                 (pMulticastFabricDesc->numMaxGpus + numKeys)));

    sendDataSize = (NvU32)(sizeof(nvlink_inband_msg_header_t) + payloadSize);

    if ((NvU32)sendDataSize > sizeof(sendDataParams->buffer))
    {
        NV_ASSERT(0);
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto done;
    }

    portMemSet(sendDataParams, 0, sendDataSize);

    pMcTeamSetupReq->mcAllocSize = pMulticastFabricDesc->allocSize;
    pMcTeamSetupReq->numGpuHandles = pMulticastFabricDesc->numMaxGpus;
    pMcTeamSetupReq->numKeys = numKeys;

    // Fill local GPUs
    for (pNode = listHead(&pMulticastFabricDesc->gpuInfoList);
         pNode != NULL;
         pNode = listNext(&pMulticastFabricDesc->gpuInfoList, pNode))
        pMcTeamSetupReq->gpuHandlesAndKeys[idx++] = pNode->gpuProbeHandle;

    // Fill remote GPUs probe handles per submap (i.e per node)
    smIter = multimapSubmapIterAll(&pMulticastFabricDesc->remoteGpuInfoMap);

    while (multimapSubmapIterNext(&smIter))
    {
        MemMulticastFabricRemoteGpuInfoMapSubmap *pSubmap = smIter.pValue;
        MemMulticastFabricRemoteGpuInfoMapIter iter;

        iter = multimapSubmapIterItems(&pMulticastFabricDesc->remoteGpuInfoMap,
                                       pSubmap);

        while (multimapItemIterNext(&iter))
        {
            // Item key is the GPU probe handle
            pMcTeamSetupReq->gpuHandlesAndKeys[idx++] =
                    multimapItemKey(&pMulticastFabricDesc->remoteGpuInfoMap,
                                    iter.pValue);
        }
    }

    // Fill local key
    pMcTeamSetupReq->gpuHandlesAndKeys[idx++] = pMulticastFabricDesc->inbandReqId;

    // Fill remote keys per submap (i.e per node)
    smIter = multimapSubmapIterAll(&pMulticastFabricDesc->remoteGpuInfoMap);

    while (multimapSubmapIterNext(&smIter))
    {
        MemMulticastFabricRemoteGpuInfoMapSubmap *pSubmap = smIter.pValue;
        MEM_MULTICAST_FABRIC_REMOTE_GPU_INFO *pRemoteNode = NULL;
        MemMulticastFabricRemoteGpuInfoMapIter iter;

        iter = multimapSubmapIterItems(&pMulticastFabricDesc->remoteGpuInfoMap,
                                       pSubmap);

        if (multimapItemIterNext(&iter))
        {
            pRemoteNode = iter.pValue;
            pMcTeamSetupReq->gpuHandlesAndKeys[idx++] = pRemoteNode->key;
        }
    }

    if (idx != (pMcTeamSetupReq->numGpuHandles + numKeys))
    {
        NV_ASSERT(0);
        status = NV_ERR_INVALID_STATE;
        goto done;
    }

    sendDataParams->dataSize = sendDataSize;

    nvlinkInitInbandMsgHdr(&pMcTeamSetupReqMsg->msgHdr,
                           NVLINK_INBAND_MSG_TYPE_MC_TEAM_SETUP_REQ_V2,
                           payloadSize, pMulticastFabricDesc->inbandReqId);

    status = knvlinkSendInbandData(pGpu, GPU_GET_KERNEL_NVLINK(pGpu),
                                   sendDataParams);
    if (status != NV_OK)
        goto done;

    pMulticastFabricDesc->bInbandReqInProgress = NV_TRUE;

done:
    portMemFree(sendDataParams);

    return status;
}

NV_STATUS
_memMulticastFabricSendInbandTeamSetupRequestV1
(
    OBJGPU                          *pGpu,
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc
)
{
    NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS *sendDataParams;
    nvlink_inband_mc_team_setup_req_msg_t *pMcTeamSetupReqMsg = NULL;
    nvlink_inband_mc_team_setup_req_t *pMcTeamSetupReq = NULL;
    MEM_MULTICAST_FABRIC_GPU_INFO *pNode;
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
                         (sizeof(pMcTeamSetupReq->gpuHandles[0]) * \
                                 pMulticastFabricDesc->numMaxGpus));

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

    nvlinkInitInbandMsgHdr(&pMcTeamSetupReqMsg->msgHdr,
                           NVLINK_INBAND_MSG_TYPE_MC_TEAM_SETUP_REQ,
                           payloadSize, pMulticastFabricDesc->inbandReqId);

    status = knvlinkSendInbandData(pGpu, GPU_GET_KERNEL_NVLINK(pGpu),
                                   sendDataParams);
    if (status != NV_OK)
        goto done;

    pMulticastFabricDesc->bInbandReqInProgress = NV_TRUE;

done:
    portMemFree(sendDataParams);

    return status;
}

NV_STATUS
_memMulticastFabricSendInbandTeamReleaseRequestV1
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
    NV_STATUS status;

    sendDataParams = (NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS *)
        portMemAllocNonPaged(sizeof(NV2080_CTRL_NVLINK_INBAND_SEND_DATA_PARAMS));

    if (sendDataParams == NULL)
        return NV_ERR_NO_MEMORY;

    pMcTeamReleaseReqMsg = (nvlink_inband_mc_team_release_req_msg_t *) \
                           &sendDataParams->buffer[0];

    pMcTeamReleaseReq = (nvlink_inband_mc_team_release_req_t *) \
                        &pMcTeamReleaseReqMsg->mcTeamReleaseReq;

    payloadSize = (NvU32)(sizeof(nvlink_inband_mc_team_release_req_t));

    sendDataSize = (NvU32)(sizeof(nvlink_inband_msg_header_t) + payloadSize);

    portMemSet(sendDataParams, 0, sendDataSize);

    pMcTeamReleaseReq->mcTeamHandle = mcTeamHandle;

    nvlinkInitInbandMsgHdr(&pMcTeamReleaseReqMsg->msgHdr,
                           NVLINK_INBAND_MSG_TYPE_MC_TEAM_RELEASE_REQ,
                           payloadSize, osGetTimestamp());

    sendDataParams->dataSize = sendDataSize;

    status = knvlinkSendInbandData(pGpu, GPU_GET_KERNEL_NVLINK(pGpu),
                                   sendDataParams);

    portMemFree(sendDataParams);

    return status;
}

NV_STATUS
_memMulticastFabricSendInbandTeamSetupRequest
(
    OBJGPU                          *pGpu,
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc
)
{
    NvU64 fmCaps;
    NV_STATUS status = NV_OK;

    status = gpuFabricProbeGetfmCaps(pGpu->pGpuFabricProbeInfoKernel,
                                     &fmCaps);
    if (status != NV_OK)
        return status;

    if (fmCaps & NVLINK_INBAND_FM_CAPS_MC_TEAM_SETUP_V2)
    {
        return _memMulticastFabricSendInbandTeamSetupRequestV2(pGpu,
                                                        pMulticastFabricDesc);
    }
    else if (fmCaps & NVLINK_INBAND_FM_CAPS_MC_TEAM_SETUP_V1)
    {
        return _memMulticastFabricSendInbandTeamSetupRequestV1(pGpu,
                                                        pMulticastFabricDesc);
    }
    else
    {
        return NV_ERR_NOT_SUPPORTED;
    }
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

    // If mcTeamHandle is zero, nothing to do...
    if (mcTeamHandle == 0)
        return NV_OK;

    status = gpuFabricProbeGetfmCaps(pGpu->pGpuFabricProbeInfoKernel,
                                     &fmCaps);
    if (status != NV_OK)
        return status;

    if (!(fmCaps & NVLINK_INBAND_FM_CAPS_MC_TEAM_RELEASE_V1))
        return NV_ERR_NOT_SUPPORTED;

    return _memMulticastFabricSendInbandTeamReleaseRequestV1(pGpu,
                                                             mcTeamHandle);
}

static NV_STATUS
_memMulticastFabricSendInbandRequest
(
    OBJGPU                            *pGpu,
    MEM_MULTICAST_FABRIC_DESCRIPTOR   *pMulticastFabricDesc,
    MEM_MULTICAST_FABRIC_REQUEST_TYPE  requestType
)
{
    NV_STATUS status = NV_OK;
    NvU32 gpuMask;

    if (pGpu == NULL)
        return NV_ERR_NOT_SUPPORTED;

    gpuMask = NVBIT(gpuGetInstance(pGpu));
    if (!rmGpuGroupLockIsOwner(0, GPU_LOCK_GRP_MASK, &gpuMask))
    {
        NV_ASSERT(0);
        return NV_ERR_INVALID_STATE;
    }

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

//
// This function may be called with RO pMulticastFabricDesc->pLock. Don't modify
// pMulticastFabricDesc.
//
static void
_memorymulticastfabricDetachMem
(
    FABRIC_VASPACE     *pFabricVAS,
    MEMORY_DESCRIPTOR  *pFabricMemDesc,
    NODE               *pMemNode
)
{
    MEMORY_DESCRIPTOR *pPhysMemDesc;
    MEM_MULTICAST_FABRIC_ATTACH_MEM_INFO_NODE *pAttachMemInfoNode;

    pAttachMemInfoNode =
        (MEM_MULTICAST_FABRIC_ATTACH_MEM_INFO_NODE *)pMemNode->Data;
    pPhysMemDesc = pAttachMemInfoNode->pPhysMemDesc;

    fabricvaspaceUnmapPhysMemdesc(pFabricVAS, pFabricMemDesc,
                                  pMemNode->keyStart,
                                  pPhysMemDesc,
                                  pAttachMemInfoNode->physMapLength);
}

static void
_memorymulticastfabricBatchFreeVas
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc
)
{
    MEMORY_DESCRIPTOR *pFabricMemDesc;
    MEM_MULTICAST_FABRIC_GPU_INFO *pGpuNode;
    NODE *pMemNode;
    FABRIC_VASPACE *pFabricVAS;
    NvU32 gpuMask;
    OBJGPU *pGpu;

    pFabricMemDesc = pMulticastFabricDesc->pMemDesc;
    NV_ASSERT_OR_RETURN_VOID(pFabricMemDesc != NULL);

    for (pGpuNode = listHead(&pMulticastFabricDesc->gpuInfoList);
         pGpuNode != NULL;
         pGpuNode = listNext(&pMulticastFabricDesc->gpuInfoList, pGpuNode))
    {
        pGpu = pGpuNode->pGpu;
        gpuMask = NVBIT(gpuGetInstance(pGpu));

        NV_ASSERT(rmGpuGroupLockIsOwner(0, GPU_LOCK_GRP_MASK, &gpuMask));

        pFabricVAS = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);
        if (pFabricVAS == NULL)
        {
            NV_ASSERT(0);
            continue;
        }

        // Everything should have been unmapped by now..
        btreeEnumStart(0, &pMemNode, pGpuNode->pAttachMemInfoTree);
        NV_ASSERT(pMemNode == NULL);

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
_memMulticastFabricDescriptorFree
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    NvU32 allocFlags;
    NvU32 gpuMask;
    NvBool bGpuLockTaken = NV_FALSE;

    if (pMulticastFabricDesc == NULL)
        return;

    //
    // Take pMulticastFabricModuleLock to synchronize with
    // memorymulticastfabricTeamSetupResponseCallback() and
    // _memMulticastFabricDescriptorAllocUsingExpPacket().
    // We don't want to delete pMulticastFabricDesc under them.
    //
    portSyncRwLockAcquireWrite(pFabric->pMulticastFabricModuleLock);
    portSyncRwLockAcquireWrite(pMulticastFabricDesc->pLock);

    pMulticastFabricDesc->refCount--;

    if (pMulticastFabricDesc->refCount != 0)
    {
        portSyncRwLockReleaseWrite(pMulticastFabricDesc->pLock);
        portSyncRwLockReleaseWrite(pFabric->pMulticastFabricModuleLock);

        return;
    }

    allocFlags = pMulticastFabricDesc->allocFlags;

    //
    // Empty caches so new calls to
    // memorymulticastfabricTeamSetupResponseCallback() and
    // _memMulticastFabricDescriptorAllocUsingExpPacket() fail.
    //
    if (!_memMulticastFabricIsPrime(allocFlags))
    {
        NV_ASSERT(pMulticastFabricDesc->cacheKey != 0);
        fabricImportCacheDelete(pFabric, &pMulticastFabricDesc->expUuid,
                                pMulticastFabricDesc->cacheKey);
    }

    fabricMulticastSetupCacheDelete(pFabric,
                                    pMulticastFabricDesc->inbandReqId);

    // Now I am the only one holding the pMulticastFabricDesc, drop the locks.
    portSyncRwLockReleaseWrite(pMulticastFabricDesc->pLock);
    portSyncRwLockReleaseWrite(pFabric->pMulticastFabricModuleLock);

    //
    // TODO: Make this a per-GPU lock in future. For simplicity and as we rely
    // on the serverInterUnmap paths, we take the all GPU locks here as well as
    // at the resource level free too.
    //
    if (!rmGpuLockIsOwner())
    {
        gpuMask = GPUS_LOCK_ALL;

        if (rmGpuGroupLockAcquire(0, GPU_LOCK_GRP_MASK, GPUS_LOCK_FLAGS_NONE,
                                  RM_LOCK_MODULES_MEM, &gpuMask) == NV_OK)
        {
            bGpuLockTaken = NV_TRUE;
        }
        else
        {
            NV_ASSERT(0);
        }
    }

    if (pMulticastFabricDesc->pMemDesc != NULL)
    {
        MEM_MULTICAST_FABRIC_GPU_INFO *pNode =
                                listHead(&pMulticastFabricDesc->gpuInfoList);

        NV_ASSERT(pMulticastFabricDesc->bMemdescInstalled);
        NV_ASSERT(pNode != NULL);

        _memorymulticastfabricBatchFreeVas(pMulticastFabricDesc);

        _memMulticastFabricSendInbandRequest(pNode->pGpu, pMulticastFabricDesc,
                                    MEM_MULTICAST_FABRIC_TEAM_RELEASE_REQUEST);
    }

    if (pMulticastFabricDesc->bInbandReqInProgress)
    {
        OS_WAIT_QUEUE *pWq;
        THREAD_STATE_NODE *pThreadNode = NULL;
        THREAD_STATE_FREE_CALLBACK freeCallback;

        NV_ASSERT_OK(osAllocWaitQueue(&pWq));

        if (pWq != NULL)
        {
            NV_ASSERT_OK(fabricMulticastCleanupCacheInsert(pFabric,
                                        pMulticastFabricDesc->inbandReqId,
                                        pWq));

            NV_ASSERT_OK(threadStateGetCurrent(&pThreadNode, NULL));

            freeCallback.pCb = fabricMulticastWaitOnTeamCleanupCallback;
            freeCallback.pCbData = (void *)pMulticastFabricDesc->inbandReqId;

            NV_ASSERT_OK(threadStateEnqueueCallbackOnFree(pThreadNode,
                                                          &freeCallback));
        }
    }

    _memMulticastFabricGpuInfoRemove(pMulticastFabricDesc);

    if (bGpuLockTaken)
    {
        rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);
        bGpuLockTaken = NV_FALSE;
    }

    _memMulticastFabricDescriptorCleanup(pMulticastFabricDesc);
}

static
MEM_MULTICAST_FABRIC_DESCRIPTOR*
_memMulticastFabricDescriptorAllocUsingExpPacket
(
    MemoryMulticastFabric        *pMemoryMulticastFabric,
    NV00FD_ALLOCATION_PARAMETERS *pAllocParams,
    NvS32                         imexChannel
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc;
    NvU64 cacheKey;
    NvUuid expUuid;
    NV_EXPORT_MEM_PACKET *pExportPacket = &pAllocParams->expPacket;
    NV_STATUS status = NV_OK;

    ct_assert(NV_MEM_EXPORT_UUID_LEN == NV_UUID_LEN);
    portMemCopy(expUuid.uuid, NV_UUID_LEN, pExportPacket->uuid,
                NV_MEM_EXPORT_UUID_LEN);

    //
    // To reuse import cache with the UCFLA class (0xf9), we create
    // unique import cache keys for MCFLA using NV00F9_IMPORT_ID_MAX, which
    // are never used by UCFLA.
    //
    cacheKey =
        (NV00F9_IMPORT_ID_MAX << NV00F9_IMPORT_ID_SHIFT) | pAllocParams->index;

    //
    // Take pMulticastFabricModuleLock to synchronize multiple constructors
    // to share the cached pMulticastFabricDesc. Without this lock, two or
    // more constructors may see the import cache empty at the same time.
    //
    // We want the following sequence happen atomically:
    //
    //    pMulticastFabricDesc = fabricImportCacheGet(..);
    //    if (pMulticastFabricDesc == NULL)
    //    {
    //        pMulticastFabricDesc = alloc();
    //        fabricImportCacheInsert(pMulticastFabricDesc);
    //        ...
    //    }
    //
    // Also, it is important to synchronize the constructor with the destructor.
    // The constructor looks up and refcounts pMulticastFabricDesc
    // non-atomically. Thus, pMulticastFabricDesc may be destroyed before
    // it could be refcounted.
    //
    //  pMulticastFabricDesc = fabricImportCacheGet(..);
    //  if (pMulticastFabricDesc != NULL)
    //  {
    //      pMulticastFabricDesc->lock();
    //      pMulticastFabricDesc->refCount++;
    //     ...
    //   }
    //

    portSyncRwLockAcquireWrite(pFabric->pMulticastFabricModuleLock);

    pMulticastFabricDesc = fabricImportCacheGet(pFabric, &expUuid, cacheKey);
    if (pMulticastFabricDesc != NULL)
    {
        portSyncRwLockAcquireWrite(pMulticastFabricDesc->pLock);

        NV_ASSERT(!_memMulticastFabricIsPrime(pMulticastFabricDesc->allocFlags));

        pMulticastFabricDesc->refCount++;

        portSyncRwLockReleaseWrite(pMulticastFabricDesc->pLock);

        goto done;
    }

    pMulticastFabricDesc =
        _memMulticastFabricDescriptorAlloc(pMemoryMulticastFabric,
                                           pAllocParams);

    if (pMulticastFabricDesc == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto done;
    }

    //
    // For the imported object, set numMaxGpus to NV_U32_MAX for two reasons.
    // a. It doesn't track how many GPUs the team should have. That's tracked
    //    by the exporter.
    // b. This object should never send MCFLA team setup request to the FM.
    //    Setting numMaxGpus to NV_U32_MAX, makes sure that implicitly.
    //
    pMulticastFabricDesc->numMaxGpus = NV_U32_MAX;

    //
    // allocSize is set to zero. GFM will provide that in the team setup
    // response.
    //
    pMulticastFabricDesc->allocSize = 0;

    //
    // For now only pageSize support is 512MB. This needs to be revisited
    // in case we support more pagesizes.
    //
    pMulticastFabricDesc->pageSize = NV_MEMORY_MULTICAST_FABRIC_PAGE_SIZE_512M;
    pMulticastFabricDesc->alignment = NV_MEMORY_MULTICAST_FABRIC_PAGE_SIZE_512M;

    pMulticastFabricDesc->exportNodeId = memoryExportGetNodeId(pExportPacket);
    pMulticastFabricDesc->expUuid = expUuid;
    pMulticastFabricDesc->cacheKey = cacheKey;
    pMulticastFabricDesc->index = pAllocParams->index;
    pMulticastFabricDesc->imexChannel = imexChannel;

    // insert into cache once ready...
    status = fabricImportCacheInsert(pFabric, &expUuid, cacheKey,
                                     pMulticastFabricDesc);

done:
    portSyncRwLockReleaseWrite(pFabric->pMulticastFabricModuleLock);

    if (status != NV_OK)
    {
        _memMulticastFabricDescriptorFree(pMulticastFabricDesc);
        pMulticastFabricDesc = NULL;
    }

    return pMulticastFabricDesc;
}

NV_STATUS
_memMulticastFabricConstruct
(
    MemoryMulticastFabric        *pMemoryMulticastFabric,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    Memory *pMemory = staticCast(pMemoryMulticastFabric, Memory);
    NV00FD_ALLOCATION_PARAMETERS *pAllocParams = pParams->pAllocParams;
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc;
    NV_STATUS status = NV_OK;

    RmClient *pRmClient = dynamicCast(RES_GET_CLIENT(pMemoryMulticastFabric), RmClient);

    if (!_memMulticastFabricIsPrime(pAllocParams->allocFlags))
    {
        if (pRmClient->imexChannel == -1)
            return NV_ERR_INSUFFICIENT_PERMISSIONS;

        NV_EXPORT_MEM_PACKET *pExportPacket = &pAllocParams->expPacket;

        // If this a single-node UUID?
        if (memoryExportGetNodeId(pExportPacket) == NV_FABRIC_INVALID_NODE_ID)
            return NV_ERR_NOT_SUPPORTED;

        pMulticastFabricDesc = _memMulticastFabricDescriptorAllocUsingExpPacket(
                                                pMemoryMulticastFabric,
                                                pAllocParams, pRmClient->imexChannel);
    }
    else
    {
        pMulticastFabricDesc = _memMulticastFabricDescriptorAlloc(pMemoryMulticastFabric,
                                                                  pAllocParams);
    }

    if (pMulticastFabricDesc == NULL)
        return NV_ERR_NO_MEMORY;

    //
    // At this point the pMulticastFabricDesc can be fresh or in-use, but will
    // be refcounted for safe access. If in-use, it can get modified before
    // pMulticastFabricDesc->pLock() is acquired.
    //

    portSyncRwLockAcquireWrite(pMulticastFabricDesc->pLock);

    //
    // For imported (non-prime) descriptor, make sure the pre-existing (cached)
    // descriptors can be accessed only if the client is subscribed to the
    // right IMEX channel.
    //
    if (!_memMulticastFabricIsPrime(pAllocParams->allocFlags) &&
        (pMulticastFabricDesc->imexChannel != pRmClient->imexChannel))
    {
        status = NV_ERR_INSUFFICIENT_PERMISSIONS;
        goto fail;
    }

    status = _memMulticastFabricDescriptorEnqueueWait(pParams->hClient,
                                                      pMulticastFabricDesc,
                                                      pAllocParams->pOsEvent,
                                                      pMemory);
    if (status != NV_OK)
        goto fail;

    pMemoryMulticastFabric->pMulticastFabricDesc = pMulticastFabricDesc;

    pMemoryMulticastFabric->expNodeId = pMulticastFabricDesc->exportNodeId;
    pMemoryMulticastFabric->bImported =
               !_memMulticastFabricIsPrime(pMulticastFabricDesc->allocFlags);

    portSyncRwLockReleaseWrite(pMulticastFabricDesc->pLock);

    return NV_OK;

fail:
    portSyncRwLockReleaseWrite(pMulticastFabricDesc->pLock);

    _memMulticastFabricDescriptorFree(pMulticastFabricDesc);

    return status;
}

NV_STATUS
_memMulticastFabricCreateMemDesc
(
    MEM_MULTICAST_FABRIC_DESCRIPTOR  *pMulticastFabricDesc,
    NvU64                             mcAddressBase,
    MEMORY_DESCRIPTOR               **ppMemDesc
)
{
    NV_STATUS status;
    MEMORY_DESCRIPTOR *pTempMemDesc = NULL;

    status = memdescCreate(&pTempMemDesc, NULL,
                           pMulticastFabricDesc->allocSize,
                           0, NV_TRUE, ADDR_FABRIC_MC,
                           NV_MEMORY_UNCACHED,
                           MEMDESC_FLAGS_NONE);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
            "Failed to allocate memory descriptor for multicast object\n");
        return status;
    }

    memdescSetPte(pTempMemDesc, AT_GPU, 0, mcAddressBase);

    memdescSetPageSize(pTempMemDesc, AT_GPU,
                       pMulticastFabricDesc->pageSize);

    pTempMemDesc->_pteKind = NV_MMU_PTE_KIND_SMSKED_MESSAGE;

    memdescSetFlag(pTempMemDesc, MEMDESC_FLAGS_SET_KIND, NV_TRUE);

    memdescSetGpuCacheAttrib(pTempMemDesc, NV_MEMORY_UNCACHED);

    *ppMemDesc = pTempMemDesc;

    return NV_OK;
}

void
_memMulticastFabricInstallMemDesc
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

    _memMulticastFabricDescriptorFlushClients(pMulticastFabricDesc);
}

static NV_STATUS
_memorymulticastFabricAllocVas
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
        NvU32 gpuMask = NVBIT(gpuGetInstance(pGpu));

        if (!rmGpuGroupLockIsOwner(0, GPU_LOCK_GRP_MASK, &gpuMask))
        {
            NV_ASSERT(0);
            status = NV_ERR_INVALID_STATE;
            goto cleanup;
        }

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

static void
_memMulticastFabricAttachGpuPostProcessor
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

    //
    // There is a failure case on multinode systems, where a buggy client
    // may cause an error the prime object (e.g. attaching a GPU even after all
    // the required GPUs were attached) after MCFLA team request is sent to the
    // GFM. This should never happen under normal case, but in case it happens,
    // fail loudly.
    //
    // Initial value is not NV_ERR_NOT_READY, means a failure was observed.
    if (pMulticastFabricDesc->mcTeamStatus != NV_ERR_NOT_READY)
    {
        status = pMulticastFabricDesc->mcTeamStatus;
        NV_ASSERT((status != NV_OK));
        goto installMemDesc;
    }

    if (mcTeamStatus != NV_OK)
        goto installMemDesc;

    if (!_memMulticastFabricIsPrime(pMulticastFabricDesc->allocFlags))
        pMulticastFabricDesc->allocSize = mcAddressSize;

    if (mcAddressSize < pMulticastFabricDesc->allocSize)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Insufficient mcAddressSize returned from Fabric Manager\n");
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto installMemDesc;
    }

    if (!NV_IS_ALIGNED64(mcAddressBase,
                         NV_MEMORY_MULTICAST_FABRIC_PAGE_SIZE_512M))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Insufficient mcAddressSize returned from Fabric Manager\n");
        status = NV_ERR_INVALID_ADDRESS;
        goto installMemDesc;
    }

    status = _memMulticastFabricCreateMemDesc(pMulticastFabricDesc,
                                              mcAddressBase, &pMemDesc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to allocate fabric memdesc\n");
        goto installMemDesc;
    }

    status = _memorymulticastFabricAllocVas(pMulticastFabricDesc, pMemDesc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to allocate fabric VAS\n");
        memdescDestroy(pMemDesc);
        pMemDesc = NULL;
        goto installMemDesc;
    }

installMemDesc:
    _memMulticastFabricInstallMemDesc(pMulticastFabricDesc, pMemDesc,
                                      mcTeamHandle, status);

     if ((status != NV_OK) && (mcTeamStatus == NV_OK))
         _memMulticastFabricSendInbandRequest(pGpu, pMulticastFabricDesc,
                                MEM_MULTICAST_FABRIC_TEAM_RELEASE_REQUEST);
}

NV_STATUS
memorymulticastfabricTeamSetupResponseCallback
(
    NvU32                                           gpuInstance,
    NvU64                                          *pNotifyGfidMask,
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
    OBJGPU *pGpu;

    NV_ASSERT(pInbandRcvParams != NULL);
    NV_ASSERT(rmGpuLockIsOwner());

    if ((pGpu = gpumgrGetGpu(gpuInstance)) == NULL)
    {
        NV_ASSERT_FAILED("Invalid GPU instance");
        return NV_ERR_INVALID_ARGUMENT;
    }

    pMcTeamSetupRspMsg =
        (nvlink_inband_mc_team_setup_rsp_msg_t *)&pInbandRcvParams->data[0];

    pMcTeamSetupRsp =
        (nvlink_inband_mc_team_setup_rsp_t *)&pMcTeamSetupRspMsg->mcTeamSetupRsp;

    requestId = pMcTeamSetupRspMsg->msgHdr.requestId;

    mcTeamStatus = pMcTeamSetupRspMsg->msgHdr.status;

    if (mcTeamStatus == NV_OK)
    {
        mcTeamHandle = pMcTeamSetupRsp->mcTeamHandle;
        mcAddressBase = pMcTeamSetupRsp->mcAddressBase;
        mcAddressSize = pMcTeamSetupRsp->mcAddressSize;

#if defined(DEBUG) || defined(DEVELOP)
        {
            // Make sure that the reserved fields are initialized to 0
            NvU8 *pRsvd = &pMcTeamSetupRsp->reserved[0];

            NV_ASSERT((pRsvd[0] == 0) && portMemCmp(pRsvd, pRsvd + 1,
                      (sizeof(pMcTeamSetupRsp->reserved) - 1)) == 0);
        }
#endif
    }

    //
    // Acquire pMulticastFabricModuleLock here, to make sure
    // pMulticastFabricDesc is not removed underneath us.
    // The thread doesn't hold refcount on pMulticastFabricDesc.
    //
    portSyncRwLockAcquireWrite(pFabric->pMulticastFabricModuleLock);

    pMulticastFabricDesc = fabricMulticastSetupCacheGet(pFabric, requestId);

    if (pMulticastFabricDesc != NULL)
    {
        fabricMulticastSetupCacheDelete(pFabric, requestId);

        //
        // We have now safely acquired pMulticastFabricDesc->lock, which
        // should block the destructor from removing pMulticastFabricDesc
        // under us even if pMulticastFabricModuleLock is dropped.
        //
        portSyncRwLockAcquireWrite(pMulticastFabricDesc->pLock);

        //
        // Drop pMulticastFabricModuleLock here as out of order, now we
        // shouldn't need it ever.
        //
        portSyncRwLockReleaseWrite(pFabric->pMulticastFabricModuleLock);

        pMulticastFabricDesc->bInbandReqInProgress = NV_FALSE;

        _memMulticastFabricAttachGpuPostProcessor(pGpu,
                                                  pMulticastFabricDesc,
                                                  mcTeamStatus,
                                                  mcTeamHandle,
                                                  mcAddressBase,
                                                  mcAddressSize);

        portSyncRwLockReleaseWrite(pMulticastFabricDesc->pLock);
    }
    else
    {
        //
        // As pMulticastFabricDesc is not found, drop the lock right
        // away. GPU locks are already taken to submit the team release
        // request.
        //
        portSyncRwLockReleaseWrite(pFabric->pMulticastFabricModuleLock);

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
        fabricMulticastCleanupCacheInvokeCallback(pFabric, requestId,
                                                  fabricWakeUpThreadCallback);
    }

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
    NV00FD_ALLOCATION_PARAMETERS *pAllocParams = pParams->pAllocParams;

    if (RS_IS_COPY_CTOR(pParams))
    {
        return memorymulticastfabricCopyConstruct(pMemoryMulticastFabric,
                                                  pCallContext, pParams);
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
                _memMulticastFabricValidateAllocParams(pAllocParams));

    return _memMulticastFabricConstruct(pMemoryMulticastFabric,
                                        pCallContext, pParams);
}

//
// Note this function is not always called with the GPU lock. Be careful
// while accessing GPU state.
//
static NV_STATUS
_memorymulticastfabricCtrlAttachGpu
(
    OBJGPU                        *pGpu,
    MemoryMulticastFabric         *pMemoryMulticastFabric,
    NV00FD_CTRL_ATTACH_GPU_PARAMS *pParams
)
{
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc =
                                pMemoryMulticastFabric->pMulticastFabricDesc;
    NV_STATUS status;
    FABRIC_VASPACE *pFabricVAS;
    MEM_MULTICAST_FABRIC_GPU_INFO *pNode;
    MEM_MULTICAST_FABRIC_GPU_INFO *pHead;
    MEM_MULTICAST_FABRIC_REMOTE_GPU_INFO *pRemoteHead;
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();

    if (pParams->flags != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "flags passed for attach mem must be zero\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Check if the Multicast FLA object has any additional slots for GPUs
    if (pMulticastFabricDesc->numAttachedGpus ==
                                pMulticastFabricDesc->numMaxGpus)
    {
        NV_PRINTF(LEVEL_ERROR, "Max no. of GPUs have already attached!\n");
        return NV_ERR_INVALID_OPERATION;
    }

    if (pMulticastFabricDesc->bMemdescInstalled)
    {
        NV_PRINTF(LEVEL_ERROR, "The object is already ready.\n");
        return NV_ERR_STATE_IN_USE;
    }

    if (RMCFG_FEATURE_PLATFORM_WINDOWS
        || gpuIsCCFeatureEnabled(pGpu)
        )
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Multicast attach not supported on Windows/CC modes\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    status = _memMulticastFabricGpuInfoAdd(pMemoryMulticastFabric,
                                           pCallContext->pControlParams);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to populate GPU info\n");
        return status;
    }

    pNode = listTail(&pMulticastFabricDesc->gpuInfoList);

    status = gpuFabricProbeGetGpuFabricHandle(pGpu->pGpuFabricProbeInfoKernel,
                                              &pNode->gpuProbeHandle);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Attaching GPU does not have a valid probe handle\n");
        goto fail;
    }

    status = gpuFabricProbeGetFabricCliqueId(pGpu->pGpuFabricProbeInfoKernel,
                                             &pNode->cliqueId);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Attaching GPU does not have a valid clique ID\n");
        goto fail;
    }

    pHead = listHead(&pMulticastFabricDesc->gpuInfoList);
    pRemoteHead = multimapFirstItem(&pMulticastFabricDesc->remoteGpuInfoMap);

    if ((pHead != NULL) && (pHead->cliqueId != pNode->cliqueId))
    {
        NV_PRINTF(LEVEL_ERROR, "Clique ID mismatch\n");
        status = NV_ERR_INVALID_DEVICE;
        goto fail;
    }

    if ((pRemoteHead != NULL) && (pRemoteHead->cliqueId != pNode->cliqueId))
    {
        NV_PRINTF(LEVEL_ERROR, "Clique ID mismatch\n");
        status = NV_ERR_INVALID_DEVICE;
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

    if ((pMulticastFabricDesc->numAttachedGpus + 1)
                                    == pMulticastFabricDesc->numMaxGpus)
    {
        NV_ASSERT(_memMulticastFabricIsPrime(pMulticastFabricDesc->allocFlags));

        status = _memMulticastFabricSendInbandRequest(pGpu,
                                    pMulticastFabricDesc,
                                    MEM_MULTICAST_FABRIC_TEAM_SETUP_REQUEST);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Inband request Multicast Team Setup failed!\n");
            goto fail;
        }
    }
    // Invoke remote GPU attach event..
    else if (!_memMulticastFabricIsPrime(pMulticastFabricDesc->allocFlags))
    {
        NvU64 fmCaps;
        Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
        NV00F1_CTRL_FABRIC_EVENT event;

        status = gpuFabricProbeGetfmCaps(pGpu->pGpuFabricProbeInfoKernel,
                                         &fmCaps);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to query IMEX FM caps\n");
            goto fail;
        }

        if (!(fmCaps & NVLINK_INBAND_FM_CAPS_MC_TEAM_SETUP_V2))
        {
            NV_PRINTF(LEVEL_ERROR, "Remote attach is supported from V2+\n");
            status = NV_ERR_NOT_SUPPORTED;
            goto fail;
        }

        if (pMulticastFabricDesc->imexChannel == -1)
        {
            NV_PRINTF(LEVEL_ERROR, "IMEX channel subscription is not available\n");
            status = NV_ERR_INVALID_STATE;
            goto fail;
        }

        _memMulticastFabricInitAttachEvent(pNode->gpuProbeHandle,
                                           pMulticastFabricDesc->inbandReqId,
                                           pNode->cliqueId,
                                           pMulticastFabricDesc->exportNodeId,
                                           pMulticastFabricDesc->index,
                                           &pMulticastFabricDesc->expUuid,
                                           pMulticastFabricDesc->imexChannel,
                                           &event);

        pNode->attachEventId = event.id;

        status = fabricPostEventsV2(pFabric, &event, 1);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to notify IMEX daemon of import event\n");
            goto fail;
        }
    }

    //
    // DO NOT FAIL after the call to fabricPostEventsV2 or
    // _memMulticastFabricSendInbandRequest().
    //
    // These functions are used to communicate with the external
    // entities like FM/IMEX daemons. It is recommended that we
    // do not fail the control call after these to avoid
    // complicated cleanups.
    //

    pMulticastFabricDesc->numAttachedGpus++;
    pMulticastFabricDesc->localAttachedGpusMask |= NVBIT32(pGpu->gpuInstance);

    return NV_OK;

fail:
    // Remove GPU OS info added in the prologue.
    listRemove(&pMulticastFabricDesc->gpuInfoList, pNode);

    return status;
}

NV_STATUS
memorymulticastfabricCtrlAttachGpu_IMPL
(
    MemoryMulticastFabric         *pMemoryMulticastFabric,
    NV00FD_CTRL_ATTACH_GPU_PARAMS *pParams
)
{
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc =
                                pMemoryMulticastFabric->pMulticastFabricDesc;
    NV_STATUS status;
    NvBool bLastAttach;
    NvBool bLastAttachRecheck;
    NvBool bGpuLockTaken = NV_FALSE;
    Subdevice *pSubdevice;
    OBJGPU *pGpu;
    NvU32 gpuMask;

    status = subdeviceGetByHandle(RES_GET_CLIENT(pMemoryMulticastFabric),
                                  pParams->hSubdevice, &pSubdevice);
    if (status == NV_ERR_OBJECT_NOT_FOUND)
        status = NV_ERR_INVALID_DEVICE;

    if (status != NV_OK)
        return status;

    pGpu = GPU_RES_GET_GPU(pSubdevice);
    gpuMask = NVBIT(gpuGetInstance(pGpu));

retry:
    //
    // Find if I am the last attach. If yes, take a GPU lock to submit
    // inband request. Otherwise, skip taking GPU lock.
    //
    portSyncRwLockAcquireRead(pMulticastFabricDesc->pLock);

    bLastAttach = ((pMulticastFabricDesc->numMaxGpus -
                    pMulticastFabricDesc->numAttachedGpus) == 1);

    // Drop the lock to avoid lock inversion with the GPU lock...
    portSyncRwLockReleaseRead(pMulticastFabricDesc->pLock);

    if (bLastAttach && (gpuMask != 0))
    {
        status = rmGpuGroupLockAcquire(0, GPU_LOCK_GRP_MASK,
                                       GPUS_LOCK_FLAGS_NONE,
                                       RM_LOCK_MODULES_MEM, &gpuMask);
        if (status != NV_OK)
        {
            NV_ASSERT(0);
            return status;
        }

        bGpuLockTaken = NV_TRUE;
    }

    portSyncRwLockAcquireWrite(pMulticastFabricDesc->pLock);

    //
    // Recheck to avoid TOCTOU, make sure bLastAttach state didn't change..
    //
    // This is a deterministic check as once the GPU is attached to
    // this object it can't be detached unless the object is destroyed.
    //
    bLastAttachRecheck = ((pMulticastFabricDesc->numMaxGpus -
                           pMulticastFabricDesc->numAttachedGpus) == 1);

    if (bLastAttachRecheck != bLastAttach)
    {
        portSyncRwLockReleaseWrite(pMulticastFabricDesc->pLock);

        if (bGpuLockTaken)
        {
            rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);
            bGpuLockTaken = NV_FALSE;
        }

        goto retry;
    }

    status = _memorymulticastfabricCtrlAttachGpu(pGpu, pMemoryMulticastFabric,
                                                 pParams);

    portSyncRwLockReleaseWrite(pMulticastFabricDesc->pLock);

    if (bGpuLockTaken)
        rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);

    return status;
}

#ifdef NV00FD_CTRL_CMD_SET_FAILURE
static NV_STATUS
_memorymulticastfabricCtrlSetFailure
(
    MemoryMulticastFabric          *pMemoryMulticastFabric,
    NV00FD_CTRL_SET_FAILURE_PARAMS *pParams
)
{
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    RmClient *pRmClient = dynamicCast(pCallContext->pClient, RmClient);
    NV_ASSERT_OR_RETURN(pRmClient != NULL, NV_ERR_INVALID_CLIENT);

    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc =
                                pMemoryMulticastFabric->pMulticastFabricDesc;

    if (!rmclientIsCapableOrAdmin(pRmClient, NV_RM_CAP_SYS_FABRIC_IMEX_MGMT, pCallContext->secInfo.privLevel))
        return NV_ERR_INSUFFICIENT_PERMISSIONS;

    if (!_memMulticastFabricIsPrime(pMulticastFabricDesc->allocFlags))
    {
        NV_PRINTF(LEVEL_ERROR, "Only supported on prime MCLFA object\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    if ((pParams->status == NV_OK) || (pParams->status == NV_ERR_NOT_READY))
        return NV_ERR_INVALID_ARGUMENT;

    pMulticastFabricDesc->mcTeamStatus = pParams->status;

    // Notify the callers about the failure.
    _memMulticastFabricDescriptorFlushClients(pMulticastFabricDesc);

    return NV_OK;
}

NV_STATUS memorymulticastfabricCtrlSetFailure_IMPL
(
    MemoryMulticastFabric          *pMemoryMulticastFabric,
    NV00FD_CTRL_SET_FAILURE_PARAMS *pParams
)
{
    NV_STATUS status;
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc =
                                pMemoryMulticastFabric->pMulticastFabricDesc;

    portSyncRwLockAcquireWrite(pMulticastFabricDesc->pLock);

    status = _memorymulticastfabricCtrlSetFailure(pMemoryMulticastFabric,
                                                  pParams);

    portSyncRwLockReleaseWrite(pMulticastFabricDesc->pLock);

    return status;
}
#endif

#ifdef NV00FD_CTRL_CMD_ATTACH_REMOTE_GPU
//
// Note this function is not always called with the GPU lock. Be careful
// while accessing GPU state.
//
static NV_STATUS
_memorymulticastfabricCtrlAttachRemoteGpu
(
    OBJGPU                               *pGpu,
    MemoryMulticastFabric                *pMemoryMulticastFabric,
    NV00FD_CTRL_ATTACH_REMOTE_GPU_PARAMS *pParams
)
{
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc =
                                pMemoryMulticastFabric->pMulticastFabricDesc;
    MEM_MULTICAST_FABRIC_GPU_INFO *pHead =
                        listHead(&pMulticastFabricDesc->gpuInfoList);
    MEM_MULTICAST_FABRIC_REMOTE_GPU_INFO *pRemoteHead =
                    multimapFirstItem(&pMulticastFabricDesc->remoteGpuInfoMap);
    MEM_MULTICAST_FABRIC_REMOTE_GPU_INFO *pNode;
    MEM_MULTICAST_FABRIC_GPU_INFO *pIter;
    MemMulticastFabricRemoteGpuInfoMapSubmap *pSubmap = NULL;
    CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
    RmClient *pRmClient = dynamicCast(pCallContext->pClient, RmClient);
    NV_ASSERT_OR_RETURN(pRmClient != NULL, NV_ERR_INVALID_CLIENT);

    NV00FD_CTRL_SET_FAILURE_PARAMS params;
    NV_STATUS status;

    if (!rmclientIsCapableOrAdmin(pRmClient, NV_RM_CAP_SYS_FABRIC_IMEX_MGMT, pCallContext->secInfo.privLevel))
    {
        // Don't set failure as non-priv client can hit this failure too.
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    if (!_memMulticastFabricIsPrime(pMulticastFabricDesc->allocFlags))
    {
        NV_PRINTF(LEVEL_ERROR, "Only supported on prime MCLFA object\n");
        status = NV_ERR_NOT_SUPPORTED;
        goto fail;
    }

    // Check if the Multicast FLA object has any additional slots for GPUs
    if (pMulticastFabricDesc->numAttachedGpus ==
                                pMulticastFabricDesc->numMaxGpus)
    {
        NV_PRINTF(LEVEL_ERROR, "Max no. of GPUs have already attached!\n");
        status = NV_ERR_INVALID_OPERATION;
        goto fail;
    }

    if ((pHead != NULL) && (pHead->cliqueId != pParams->cliqueId))
    {
        NV_PRINTF(LEVEL_ERROR, "Clique ID mismatch\n");
        status = NV_ERR_INVALID_DEVICE;
        goto fail;
    }

    if ((pRemoteHead != NULL) && (pRemoteHead->cliqueId != pParams->cliqueId))
    {
        NV_PRINTF(LEVEL_ERROR, "Clique ID mismatch\n");
        status = NV_ERR_INVALID_DEVICE;
        goto fail;
    }

    if (pParams->nodeId == NV_FABRIC_INVALID_NODE_ID)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid node ID\n");
        status = NV_ERR_INVALID_ARGUMENT;
        goto fail;
    }

    for (pIter = listHead(&pMulticastFabricDesc->gpuInfoList);
         pIter != NULL;
         pIter = listNext(&pMulticastFabricDesc->gpuInfoList, pIter))
    {
        if (pIter->gpuProbeHandle == pParams->gpuFabricProbeHandle)
        {
           NV_PRINTF(LEVEL_ERROR, "GPU is already attached\n");
           status = NV_ERR_INSERT_DUPLICATE_NAME;
           goto fail;
        }
    }

    if (multimapFindSubmap(&pMulticastFabricDesc->remoteGpuInfoMap,
                           pParams->nodeId) == NULL)
    {
        pSubmap = multimapInsertSubmap(&pMulticastFabricDesc->remoteGpuInfoMap,
                                       pParams->nodeId);
        if (pSubmap == NULL)
        {
            status = NV_ERR_NO_MEMORY;
            goto fail;
        }
    }

    pNode = multimapInsertItemNew(&pMulticastFabricDesc->remoteGpuInfoMap,
                                  pParams->nodeId,
                                  pParams->gpuFabricProbeHandle);
    if (pNode == NULL)
    {
        // This could also fail due to NO_MEMORY error..
        NV_PRINTF(LEVEL_ERROR, "Failed to track remote GPU info\n");
        status = NV_ERR_INSERT_DUPLICATE_NAME;
        goto cleanup_submap;
    }

    portMemSet(pNode, 0, sizeof(*pNode));

    pNode->key = pParams->key;
    pNode->cliqueId = pParams->cliqueId;

    if ((pMulticastFabricDesc->numAttachedGpus + 1)
                                    == pMulticastFabricDesc->numMaxGpus)
    {
        status = _memMulticastFabricSendInbandRequest(pGpu,
                                    pMulticastFabricDesc,
                                    MEM_MULTICAST_FABRIC_TEAM_SETUP_REQUEST);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Inband request Multicast Team Setup failed!\n");
            goto unlink_node;
        }
    }

    //
    // DO NOT FAIL after the call to _memMulticastFabricSendInbandRequest().
    //
    // This function is used to communicate with the external
    // entities like FM daemon. It is recommended that we do not
    // fail the control call after this to avoid complicated state
    // cleanups.
    //

    pMulticastFabricDesc->numAttachedGpus++;

    return NV_OK;

unlink_node:
    multimapRemoveItemByKey(&pMulticastFabricDesc->remoteGpuInfoMap,
                            pParams->nodeId, pParams->gpuFabricProbeHandle);

cleanup_submap:
    if (pSubmap != NULL)
        multimapRemoveSubmap(&pMulticastFabricDesc->remoteGpuInfoMap, pSubmap);

fail:
    //
    // NV_ERR_NOT_READY means the object is not ready, which is the initial
    // state. Thus, in case any API returns NV_ERR_NOT_READY, overwrite it.
    //
    if (status == NV_ERR_NOT_READY)
        status = NV_ERR_GENERIC;

    params.status = status;

    NV_ASSERT_OK(_memorymulticastfabricCtrlSetFailure(pMemoryMulticastFabric,
                                                      &params));

    return status;
}

NV_STATUS
memorymulticastfabricCtrlAttachRemoteGpu_IMPL
(
    MemoryMulticastFabric                *pMemoryMulticastFabric,
    NV00FD_CTRL_ATTACH_REMOTE_GPU_PARAMS *pParams
)
{
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc =
                                pMemoryMulticastFabric->pMulticastFabricDesc;
    NV_STATUS status;
    OBJGPU *pGpu;
    MEM_MULTICAST_FABRIC_GPU_INFO *pNode;
    NvU32 gpuMask;
    NvBool bLastAttach;
    NvBool bLastAttachRecheck;
    NvBool bGpuLockTaken = NV_FALSE;

retry:
    //
    // Find if I am the last attach. If yes, take a GPU lock to submit
    // inband request. Otherwise, skip taking GPU lock.
    //
    portSyncRwLockAcquireRead(pMulticastFabricDesc->pLock);

    bLastAttach = ((pMulticastFabricDesc->numMaxGpus -
                    pMulticastFabricDesc->numAttachedGpus) == 1);

    pNode = listHead(&pMulticastFabricDesc->gpuInfoList);
    pGpu = (pNode != NULL) ? pNode->pGpu : NULL;
    gpuMask = (pGpu != NULL) ? NVBIT(gpuGetInstance(pGpu)) : 0;

    // Drop the lock to avoid lock inversion with the GPU lock...
    portSyncRwLockReleaseRead(pMulticastFabricDesc->pLock);

    if (bLastAttach && (gpuMask != 0))
    {
        status = rmGpuGroupLockAcquire(0, GPU_LOCK_GRP_MASK,
                                       GPUS_LOCK_FLAGS_NONE,
                                       RM_LOCK_MODULES_MEM, &gpuMask);
        if (status != NV_OK)
        {
            NV_ASSERT(0);
            return status;
        }

        bGpuLockTaken = NV_TRUE;
    }

    portSyncRwLockAcquireWrite(pMulticastFabricDesc->pLock);

    //
    // Recheck to avoid TOCTOU, make sure bLastAttach state didn't change..
    //
    // This is a deterministic check as once the GPU is attached to
    // this object it can't be detached unless the object is destroyed.
    //
    bLastAttachRecheck = ((pMulticastFabricDesc->numMaxGpus -
                           pMulticastFabricDesc->numAttachedGpus) == 1);

    if (bLastAttachRecheck != bLastAttach)
    {
        portSyncRwLockReleaseWrite(pMulticastFabricDesc->pLock);

        if (bGpuLockTaken)
        {
            rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);
            bGpuLockTaken = NV_FALSE;
        }

        goto retry;
    }

    status = _memorymulticastfabricCtrlAttachRemoteGpu(pGpu,
                                                       pMemoryMulticastFabric,
                                                       pParams);

    portSyncRwLockReleaseWrite(pMulticastFabricDesc->pLock);

    if (bGpuLockTaken)
        rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);

    return status;
}
#endif

static MEM_MULTICAST_FABRIC_GPU_INFO*
_memorymulticastfabricGetAttchedGpuInfo
(
    MemoryMulticastFabric  *pMemoryMulticastFabric,
    OBJGPU                 *pGpu
)
{
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc =
                                pMemoryMulticastFabric->pMulticastFabricDesc;
    MEM_MULTICAST_FABRIC_GPU_INFO *pNodeItr;

    for (pNodeItr = listHead(&pMulticastFabricDesc->gpuInfoList);
         pNodeItr != NULL;
         pNodeItr = listNext(&pMulticastFabricDesc->gpuInfoList, pNodeItr))
    {
        if (pNodeItr->pGpu == pGpu)
            return pNodeItr;
    }

    return NULL;
}

//
// This function is called with RO pMulticastFabricDesc->pLock. Don't modify
// pMulticastFabricDesc.
//
static NV_STATUS
_memorymulticastfabricCtrlDetachMem
(
    OBJGPU                        *pGpu,
    MemoryMulticastFabric         *pMemoryMulticastFabric,
    NvU64                          offset,
    NvBool                         bRemoveInterMapping
)
{
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc =
                                pMemoryMulticastFabric->pMulticastFabricDesc;
    MEM_MULTICAST_FABRIC_GPU_INFO *pGpuInfo;
    NODE *pNode;
    MEM_MULTICAST_FABRIC_ATTACH_MEM_INFO_NODE *pAttachMemInfoNode;
    MEMORY_DESCRIPTOR *pFabricMemDesc;
    FABRIC_VASPACE *pFabricVAS;
    NV_STATUS status;
    NvU32 gpuMask;

    pGpuInfo = _memorymulticastfabricGetAttchedGpuInfo(pMemoryMulticastFabric,
                                                       pGpu);
    if (pGpuInfo == NULL)
        return NV_ERR_INVALID_DEVICE;

    gpuMask = NVBIT(gpuGetInstance(pGpu));

    if (!rmGpuGroupLockIsOwner(0, GPU_LOCK_GRP_MASK, &gpuMask))
        return NV_ERR_INVALID_LOCK_STATE;

    status = btreeSearch(offset, &pNode, pGpuInfo->pAttachMemInfoTree);
    if (status != NV_OK)
        return status;

    pAttachMemInfoNode = pNode->Data;

    pFabricMemDesc = pMulticastFabricDesc->pMemDesc;
    NV_ASSERT_OR_RETURN(pFabricMemDesc != NULL, NV_ERR_INVALID_STATE);

    pFabricVAS = dynamicCast(pGpuInfo->pGpu->pFabricVAS, FABRIC_VASPACE);
    NV_ASSERT_OR_RETURN(pFabricVAS != NULL, NV_ERR_INVALID_STATE);

    _memorymulticastfabricDetachMem(pFabricVAS, pFabricMemDesc, pNode);

    if (bRemoveInterMapping)
    {
        refRemoveInterMapping(RES_GET_REF(pMemoryMulticastFabric),
                              pAttachMemInfoNode->pInterMapping);
    }

    btreeUnlink(pNode, &pGpuInfo->pAttachMemInfoTree);
    portMemFree(pNode->Data);

    return NV_OK;
}

NV_STATUS
memorymulticastfabricCtrlDetachMem_IMPL
(
    MemoryMulticastFabric         *pMemoryMulticastFabric,
    NV00FD_CTRL_DETACH_MEM_PARAMS *pParams
)
{
    NV_STATUS status;
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc =
                                pMemoryMulticastFabric->pMulticastFabricDesc;
    Subdevice *pSubdevice;
    NvU32 gpuMask;
    OBJGPU *pGpu;

    if (pParams->flags != 0)
        return NV_ERR_INVALID_ARGUMENT;

    status = subdeviceGetByHandle(RES_GET_CLIENT(pMemoryMulticastFabric),
                                  pParams->hSubdevice, &pSubdevice);
    if (status == NV_ERR_OBJECT_NOT_FOUND)
        status = NV_ERR_INVALID_DEVICE;

    if (status != NV_OK)
        return status;

    pGpu = GPU_RES_GET_GPU(pSubdevice);
    gpuMask = GPUS_LOCK_ALL;

    //
    // Take per-GPU lock as we ensure source and destination devices are the
    // same.
    //
    status = rmGpuGroupLockAcquire(0, GPU_LOCK_GRP_MASK, GPUS_LOCK_FLAGS_NONE,
                                   RM_LOCK_MODULES_MEM, &gpuMask);
    if (status != NV_OK)
    {
        NV_ASSERT(0);
        return status;
    }

    //
    // pMulticastFabricDesc->pLock read locking is sufficient as rest of the
    // state in being modified is per-GPU, which is protected by the GPU lock.
    //
    portSyncRwLockAcquireRead(pMulticastFabricDesc->pLock);

    // Remove inter mapping on explicit detach by the control call.
    status = _memorymulticastfabricCtrlDetachMem(pGpu, pMemoryMulticastFabric,
                                                 pParams->offset, NV_TRUE);

    portSyncRwLockReleaseRead(pMulticastFabricDesc->pLock);

    rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);

    return status;
}

static NV_STATUS
_memorymulticastfabricValidatePhysMem
(
    RsClient              *pClient,
    NvHandle               hPhysMem,
    OBJGPU               **ppGpu,
    Memory               **ppPhysMemory
)
{
    MEMORY_DESCRIPTOR *pPhysMemDesc;
    NvU64 physPageSize;
    NV_STATUS status;
    Memory *pMemory;

    status = memGetByHandle(pClient, hPhysMem, &pMemory);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid object handle passed\n");
        return status;
    }

    pPhysMemDesc = pMemory->pMemDesc;

    if (pPhysMemDesc->pGpu == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Device-less memory isn't supported yet\n");

        return NV_ERR_INVALID_ARGUMENT;
    }

    if (!memmgrIsMemDescSupportedByFla_HAL(pPhysMemDesc->pGpu,
                                           GPU_GET_MEMORY_MANAGER(pPhysMemDesc->pGpu),
                                           pPhysMemDesc))
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

    *ppGpu = pPhysMemDesc->pGpu;
    *ppPhysMemory = pMemory;

    return NV_OK;
}

//
// This function is called with RO pMulticastFabricDesc->pLock. Don't modify
// pMulticastFabricDesc.
//
static NV_STATUS
_memorymulticastfabricCtrlAttachMem
(
    OBJGPU                        *pGpu,
    MemoryMulticastFabric         *pMemoryMulticastFabric,
    Memory                        *pPhysMemory,
    NV00FD_CTRL_ATTACH_MEM_PARAMS *pParams
)
{
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc =
                                pMemoryMulticastFabric->pMulticastFabricDesc;
    MEM_MULTICAST_FABRIC_GPU_INFO *pGpuInfo;
    NV_STATUS status;
    MEMORY_DESCRIPTOR *pPhysMemDesc = pPhysMemory->pMemDesc;
    MEMORY_DESCRIPTOR *pFabricMemDesc;
    FABRIC_VASPACE *pFabricVAS;
    MEM_MULTICAST_FABRIC_ATTACH_MEM_INFO_NODE *pNode;
    NvU32 gpuMask = NVBIT(gpuGetInstance(pGpu));
    RsInterMapping *pInterMapping;

    if (!rmGpuGroupLockIsOwner(0, GPU_LOCK_GRP_MASK, &gpuMask))
        return NV_ERR_INVALID_LOCK_STATE;

    pGpuInfo = _memorymulticastfabricGetAttchedGpuInfo(pMemoryMulticastFabric,
                                                       pGpu);
    if (pGpuInfo == NULL)
        return NV_ERR_INVALID_DEVICE;

    pFabricVAS = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);
    NV_ASSERT_OR_RETURN(pFabricVAS != NULL, NV_ERR_INVALID_STATE);

    pFabricMemDesc = pMulticastFabricDesc->pMemDesc;
    NV_ASSERT_OR_RETURN(pFabricMemDesc != NULL, NV_ERR_INVALID_STATE);

    NV_ASSERT_OR_RETURN(pGpuInfo->bMcflaAlloc, NV_ERR_INVALID_STATE);

    status = refAddInterMapping(RES_GET_REF(pMemoryMulticastFabric),
                                RES_GET_REF(pPhysMemory),
                                RES_GET_REF(pPhysMemory)->pParentRef,
                                &pInterMapping);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to setup inter mapping\n");
        return status;
    }

    pInterMapping->flags = pParams->flags;
    pInterMapping->dmaOffset = pParams->offset;
    pInterMapping->pMemDesc = pPhysMemDesc;

    // No partial unmap supported
    pInterMapping->size = 0;

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
        goto removeIntermap;
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
    pNode->pInterMapping = pInterMapping;
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

removeIntermap:
    refRemoveInterMapping(RES_GET_REF(pMemoryMulticastFabric), pInterMapping);

    return status;
}

NV_STATUS
memorymulticastfabricCtrlAttachMem_IMPL
(
    MemoryMulticastFabric         *pMemoryMulticastFabric,
    NV00FD_CTRL_ATTACH_MEM_PARAMS *pParams
)
{
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc =
                                pMemoryMulticastFabric->pMulticastFabricDesc;
    NV_STATUS status;
    NvU32 gpuMask;
    OBJGPU *pGpu;
    Memory *pPhysMemory;

    if (pParams->flags != 0)
        return NV_ERR_INVALID_ARGUMENT;

    //
    // TODO: pParams->hSubdevice will be used when deviceless memory classes
    // will be supported.
    //

    status = _memorymulticastfabricValidatePhysMem(RES_GET_CLIENT(pMemoryMulticastFabric),
                                                   pParams->hMemory, &pGpu, &pPhysMemory);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to validate physmem handle\n");
        return status;
    }

    gpuMask = NVBIT(gpuGetInstance(pGpu));

    //
    // Take per-GPU lock as we ensure source and destination devices are the
    // same.
    //
    status = rmGpuGroupLockAcquire(0, GPU_LOCK_GRP_MASK, GPUS_LOCK_FLAGS_NONE,
                                   RM_LOCK_MODULES_MEM, &gpuMask);
    if (status != NV_OK)
    {
        NV_ASSERT(0);
        return status;
    }

    //
    // pMulticastFabricDesc->pLock read locking is sufficient as rest of the
    // state in being modified is per-GPU, which is protected by the GPU lock.
    //
    portSyncRwLockAcquireRead(pMulticastFabricDesc->pLock);

    status = _memorymulticastfabricCtrlAttachMem(pGpu, pMemoryMulticastFabric,
                                                 pPhysMemory, pParams);

    portSyncRwLockReleaseRead(pMulticastFabricDesc->pLock);

    rmGpuGroupLockRelease(gpuMask, GPUS_LOCK_FLAGS_NONE);

    return status;
}

void
memorymulticastfabricDestruct_IMPL
(
    MemoryMulticastFabric *pMemoryMulticastFabric
)
{
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc =
                                pMemoryMulticastFabric->pMulticastFabricDesc;
    Memory *pMemory = staticCast(pMemoryMulticastFabric, Memory);

    portSyncRwLockAcquireWrite(pMulticastFabricDesc->pLock);

    memDestructCommon(pMemory);

    _memMulticastFabricDescriptorDequeueWait(pMulticastFabricDesc, pMemory);

    portSyncRwLockReleaseWrite(pMulticastFabricDesc->pLock);

    _memMulticastFabricDescriptorFree(pMulticastFabricDesc);
}

NV_STATUS
memorymulticastfabricUnmapFrom_IMPL
(
    MemoryMulticastFabric    *pMemoryMulticastFabric,
    RS_RES_UNMAP_FROM_PARAMS *pParams
)
{
    OBJGPU *pGpu = pParams->pGpu;
    NvU32 gpuMask = NVBIT(gpuGetInstance(pGpu));
    NV_STATUS status;
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc;

    if (!rmGpuGroupLockIsOwner(0, GPU_LOCK_GRP_MASK, &gpuMask))
    {
        NV_ASSERT(0);
        return NV_ERR_INVALID_STATE;
    }

    pMulticastFabricDesc = pMemoryMulticastFabric->pMulticastFabricDesc;

    //
    // pMulticastFabricDesc->pLock read locking is sufficient as rest of the
    // state in being modified is per-GPU, which is protected by the GPU lock.
    //
    portSyncRwLockAcquireRead(pMulticastFabricDesc->pLock);

    status = _memorymulticastfabricCtrlDetachMem(pGpu, pMemoryMulticastFabric,
                                                 pParams->dmaOffset, NV_FALSE);
    NV_ASSERT(status == NV_OK);

    portSyncRwLockReleaseRead(pMulticastFabricDesc->pLock);

    return status;
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

    MemoryMulticastFabric *pSourceMemoryMulticastFabric =
        dynamicCast(pParams->pSrcRef->pResource, MemoryMulticastFabric);

    pMulticastFabricDesc = pSourceMemoryMulticastFabric->pMulticastFabricDesc;

    portSyncRwLockAcquireWrite(pMulticastFabricDesc->pLock);

    pMulticastFabricDesc->refCount++;

    pMemoryMulticastFabric->pMulticastFabricDesc = pMulticastFabricDesc;

    pMemoryMulticastFabric->expNodeId = pMulticastFabricDesc->exportNodeId;
    pMemoryMulticastFabric->bImported =
               !_memMulticastFabricIsPrime(pMulticastFabricDesc->allocFlags);

    portSyncRwLockReleaseWrite(pMulticastFabricDesc->pLock);

    return NV_OK;
}

static NV_STATUS
_memorymulticastfabricCtrlGetInfo
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
memorymulticastfabricCtrlGetInfo_IMPL
(
    MemoryMulticastFabric       *pMemoryMulticastFabric,
    NV00FD_CTRL_GET_INFO_PARAMS *pParams
)
{
    NV_STATUS status = NV_OK;
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc;

    pMulticastFabricDesc = pMemoryMulticastFabric->pMulticastFabricDesc;

    portSyncRwLockAcquireRead(pMulticastFabricDesc->pLock);

    status = _memorymulticastfabricCtrlGetInfo(pMemoryMulticastFabric,
                                               pParams);

    portSyncRwLockReleaseRead(pMulticastFabricDesc->pLock);

    return status;
}

NV_STATUS
memorymulticastfabricIsReady_IMPL
(
    MemoryMulticastFabric *pMemoryMulticastFabric,
    NvBool                 bCopyConstructorContext
)
{
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc =
                                pMemoryMulticastFabric->pMulticastFabricDesc;
    Memory *pMemory = staticCast(pMemoryMulticastFabric, Memory);
    NV_STATUS mcTeamStatus;

    portSyncRwLockAcquireRead(pMulticastFabricDesc->pLock);

    mcTeamStatus = pMulticastFabricDesc->mcTeamStatus;

    if (bCopyConstructorContext && (mcTeamStatus == NV_ERR_NOT_READY))
    {
        portSyncRwLockReleaseRead(pMulticastFabricDesc->pLock);
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

    portSyncRwLockReleaseRead(pMulticastFabricDesc->pLock);

    return mcTeamStatus;
}

static NV_STATUS
_memorymulticastfabricCtrlRegisterEvent
(
    MemoryMulticastFabric             *pMemoryMulticastFabric,
    NV00FD_CTRL_REGISTER_EVENT_PARAMS *pParams
)
{
    Memory    *pMemory = staticCast(pMemoryMulticastFabric, Memory);
    NvHandle   hClient = RES_GET_CLIENT_HANDLE(pMemoryMulticastFabric);

    return _memMulticastFabricDescriptorEnqueueWait(hClient,
                            pMemoryMulticastFabric->pMulticastFabricDesc,
                            pParams->pOsEvent, pMemory);
}


NV_STATUS
memorymulticastfabricCtrlRegisterEvent_IMPL
(
    MemoryMulticastFabric             *pMemoryMulticastFabric,
    NV00FD_CTRL_REGISTER_EVENT_PARAMS *pParams
)
{
    NV_STATUS status;
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc =
                                pMemoryMulticastFabric->pMulticastFabricDesc;

    portSyncRwLockAcquireWrite(pMulticastFabricDesc->pLock);

    status = _memorymulticastfabricCtrlRegisterEvent(pMemoryMulticastFabric,
                                                     pParams);

    portSyncRwLockReleaseWrite(pMulticastFabricDesc->pLock);

    return status;
}

NV_STATUS
memorymulticastfabricControl_IMPL
(
    MemoryMulticastFabric          *pMemoryMulticastFabric,
    CALL_CONTEXT                   *pCallContext,
    RS_RES_CONTROL_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS status = NV_OK;

    if (
#ifdef NV00FD_CTRL_CMD_ATTACH_REMOTE_GPU
        (pParams->cmd != NV00FD_CTRL_CMD_ATTACH_REMOTE_GPU) &&
#endif
#ifdef NV00FD_CTRL_CMD_SET_FAILURE
        (pParams->cmd != NV00FD_CTRL_CMD_SET_FAILURE) &&
#endif
        (pParams->cmd != NV00FD_CTRL_CMD_ATTACH_GPU))
    {
        status = memorymulticastfabricIsReady(pMemoryMulticastFabric,
                                              NV_FALSE);
    }

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
        // Clients may busy-loop on this error status, don't log error.
        if (status == NV_ERR_NOT_READY)
            return status;

        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, status);
    }

    //
    // Note: GPU lock(s) is required for some control calls. Thus, it is
    // incorrect to take the leaf lock here. resControl_IMPL() attempts to
    // acquire the GPU locks before it calls the control call body.
    //
    return resControl_IMPL(staticCast(pMemoryMulticastFabric, RsResource),
                           pCallContext, pParams);
}

NvBool
memorymulticastfabricIsExportAllowed_IMPL
(
    MemoryMulticastFabric *pMemoryMulticastFabric
)
{
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc =
                                pMemoryMulticastFabric->pMulticastFabricDesc;
    NvBool bAllowed;

    portSyncRwLockAcquireRead(pMulticastFabricDesc->pLock);

    bAllowed = _memMulticastFabricIsPrime(pMulticastFabricDesc->allocFlags);

    portSyncRwLockReleaseRead(pMulticastFabricDesc->pLock);

    return bAllowed;
}

NvBool
memorymulticastfabricIsGpuMapAllowed_IMPL
(
    MemoryMulticastFabric *pMemoryMulticastFabric,
    OBJGPU                *pGpu
)
{
    NvU32 localAttachedGpusMask;
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc =
                                pMemoryMulticastFabric->pMulticastFabricDesc;

    portSyncRwLockAcquireRead(pMulticastFabricDesc->pLock);

    localAttachedGpusMask = pMulticastFabricDesc->localAttachedGpusMask;

    portSyncRwLockReleaseRead(pMulticastFabricDesc->pLock);

    return ((localAttachedGpusMask & NVBIT32(pGpu->gpuInstance)) != 0U);
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

void
memorymulticastfabricRemoveFromCache_IMPL
(
    MemoryMulticastFabric *pMemoryMulticastFabric
)
{
    Fabric *pFabric = SYS_GET_FABRIC(SYS_GET_INSTANCE());
    MEM_MULTICAST_FABRIC_DESCRIPTOR *pMulticastFabricDesc =
                                pMemoryMulticastFabric->pMulticastFabricDesc;

    if (!pMemoryMulticastFabric->bImported)
        return;

    portSyncRwLockAcquireRead(pMulticastFabricDesc->pLock);

    fabricImportCacheDelete(pFabric, &pMulticastFabricDesc->expUuid,
                            pMulticastFabricDesc->cacheKey);

    portSyncRwLockReleaseRead(pMulticastFabricDesc->pLock);
}
