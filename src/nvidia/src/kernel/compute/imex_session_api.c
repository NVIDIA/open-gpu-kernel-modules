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


/*!
 * @file
 * @brief This file contains the functions managing the ImexSession
 */

#define NVOC_IMEX_SESSION_API_H_PRIVATE_ACCESS_ALLOWED

#include "core/core.h"
#include "compute/imex_session_api.h"
#include "resserv/rs_client.h"
#include "core/system.h"
#include "rmapi/client.h"
#include "gpu/device/device.h"
#include "os/os.h"
#include "class/cl00f1.h"
#include "compute/fabric.h"
#include "gpu_mgr/gpu_mgr.h"
#include "kernel/gpu/gpu.h"
#include "ctrl/ctrl0080/ctrl0080internal.h"
#include "rmapi/rs_utils.h"
#include "core/locks.h"
#include "resserv/rs_resource.h"
#include "mem_mgr/mem_fabric_import_v2.h"
#include "mem_mgr/mem_export.h"

#include "mem_mgr/mem_multicast_fabric.h"

static void
_performRcAndDisableChannels
(
    OBJGPU *pGpu,
    NV0080_CTRL_INTERNAL_FIFO_RC_AND_PERMANENTLY_DISABLE_CHANNELS_PARAMS *pParams
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    if (pRmApi->Control(pRmApi,
            pGpu->hInternalClient,
            pGpu->hInternalDevice,
            NV0080_CTRL_CMD_INTERNAL_FIFO_RC_AND_PERMANENTLY_DISABLE_CHANNELS,
            pParams,
            sizeof(*pParams)) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                "Failed to disable channels for GPU %d\n", pGpu->gpuInstance);
    }
}

static NvBool
_checkDanglingExports
(
    RsClient *pRsClient
)
{
    RS_ITERATOR it = clientRefIter(pRsClient, 0, classId(MemoryExport),
                                   RS_ITERATE_CHILDREN, NV_TRUE);

    if (clientRefIterNext(pRsClient, &it))
        return NV_TRUE;

    return NV_FALSE;
}

static NvBool
_checkClientDesiredImporterAndClearCache
(
    RsClient *pRsClient,
    NvU16     nodeId
)
{
    RS_ITERATOR it = clientRefIter(pRsClient, 0, classId(MemoryFabricImportV2),
                                   RS_ITERATE_CHILDREN, NV_TRUE);
    NvBool bMatch = NV_FALSE;

    while (clientRefIterNext(pRsClient, &it))
    {
        MemoryFabricImportV2 *pMemoryFabricImportV2 =
            dynamicCast(it.pResourceRef->pResource, MemoryFabricImportV2);

        if (pMemoryFabricImportV2 == NULL)
            continue;

        // Importer of any node or perfect match
        if ((nodeId == NV_FABRIC_INVALID_NODE_ID) ||
            (pMemoryFabricImportV2->expNodeId == nodeId))
        {
            bMatch = NV_TRUE;

            // Clear cache to avoid imports on stale cache.
            memoryfabricimportv2RemoveFromCache(pMemoryFabricImportV2);
        }
    }

    it = clientRefIter(pRsClient, 0, classId(MemoryMulticastFabric),
                       RS_ITERATE_CHILDREN, NV_TRUE);

    while (clientRefIterNext(pRsClient, &it))
    {
        MemoryMulticastFabric *pMemoryMulticastFabric =
            dynamicCast(it.pResourceRef->pResource, MemoryMulticastFabric);

        if ((pMemoryMulticastFabric == NULL) ||
            (!pMemoryMulticastFabric->bImported))
            continue;

        // Importer of any node or perfect match
        if ((nodeId == NV_FABRIC_INVALID_NODE_ID) ||
            (pMemoryMulticastFabric->expNodeId == nodeId))
        {
            bMatch = NV_TRUE;

            // Clear cache to avoid imports on stale cache.
            memorymulticastfabricRemoveFromCache(pMemoryMulticastFabric);
        }
    }

    return bMatch;
}

void
rcAndDisableOutstandingClientsWithImportedMemory
(
    OBJGPU   *pGpu,
    NvU16     nodeId
)
{
    RmClient **ppClient;
    Device *pDevice;
    NV0080_CTRL_INTERNAL_FIFO_RC_AND_PERMANENTLY_DISABLE_CHANNELS_PARAMS params;
    NvU32 gpuMask = 0;
    NvU32 gpuCount = 0;
    NvU32 gpuInstance = 0;

    NV_ASSERT(rmapiLockIsOwner());

    if (pGpu == NULL)
    {
        gpumgrGetGpuAttachInfo(&gpuCount, &gpuMask);
    }
    else
    {
        gpuCount = 1;
        gpuMask = NVBIT(pGpu->gpuInstance);
    }

    if (gpuCount != 0)
    {
        NV_ASSERT(rmGpuLockIsOwner() ||
            rmGpuGroupLockIsOwner(0, GPU_LOCK_GRP_MASK, &gpuMask));
    }

    while ((pGpu = gpumgrGetNextGpu(gpuMask, &gpuInstance)) != NULL)
    {
        params.numClients = 0;

        for (ppClient = serverutilGetFirstClientUnderLock();
             ppClient;
             ppClient = serverutilGetNextClientUnderLock(ppClient))
        {
            RmClient *pClient = *ppClient;
            RsClient *pRsClient = staticCast(pClient, RsClient);

            // Skip kernel clients like UVM or check if the client is IMEX capable
            if ((rmclientGetCachedPrivilege(pClient) >= RS_PRIV_LEVEL_KERNEL) ||
                (rmclientIsCapable(pClient, NV_RM_CAP_SYS_FABRIC_IMEX_MGMT)))
                continue;

            //
            // DO NOT SKIP the clients for any other condition than the
            // privileged trusted clients before this check. We must
            // disable clients even if they have not subscribed to a device
            // but have imported device-less fabric memory.
            //
            if (!_checkClientDesiredImporterAndClearCache(pRsClient, nodeId))
                continue;

            // Disable client from performing new alloc or duping.
            pRsClient->bActive = NV_FALSE;

            // Not subscribed to this device..
            if (deviceGetByGpu(pRsClient, pGpu, NV_TRUE, &pDevice) != NV_OK)
                continue;

            params.clientHandles[params.numClients] = pRsClient->hClient;
            params.numClients++;

            // Flush if the client handle array is full
            if (params.numClients ==
                            NV_FIFO_PERMANENTLY_DISABLE_CHANNELS_MAX_CLIENTS)
            {
                _performRcAndDisableChannels(pGpu, &params);
                params.numClients = 0;
            }
        }

        if (params.numClients != 0)
            _performRcAndDisableChannels(pGpu, &params);
    }
}

NV_STATUS
imexsessionapiConstruct_IMPL
(
    ImexSessionApi               *pImexSessionApi,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV00F1_ALLOCATION_PARAMETERS *pUserParams = pParams->pAllocParams;
    OBJSYS *pSys = SYS_GET_INSTANCE();
    Fabric *pFabric = SYS_GET_FABRIC(pSys);
    NvHandle hClient = pCallContext->pClient->hClient;
    NV_STATUS status;
    NvP64 pImexOsEvent = NvP64_NULL;

    NV_ASSERT_OR_RETURN(RMCFG_FEATURE_KERNEL_RM, NV_ERR_NOT_SUPPORTED);

    if ((pUserParams->flags != 0) ||
        (pUserParams->pOsEvent == NULL) ||
        (pUserParams->nodeId == NV_FABRIC_INVALID_NODE_ID))
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid input value\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    status = osUserHandleToKernelPtr(hClient, pUserParams->pOsEvent,
                                     &pImexOsEvent);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Invalid event handle: 0x%x\n", status);
        return status;
    }

    //
    // This check also ensure that there is only one IMEX instance listening
    // to events.
    //
    status = fabricSetImexEvent(pFabric, pImexOsEvent);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Unable to set event: 0x%x\n", status);
        goto fail_dereference;
    }

    osRmCapInitDescriptor(&pImexSessionApi->dupedCapDescriptor);

    status = osRmCapAcquire(pSys->pOsRmCaps,
                            NV_RM_CAP_SYS_FABRIC_IMEX_MGMT,
                            pUserParams->capDescriptor,
                            &pImexSessionApi->dupedCapDescriptor);
    //
    // On platforms where capability isn't implemented,
    // enforce the admin-only check.
    //
    if (status == NV_ERR_NOT_SUPPORTED)
    {
        if (!rmclientIsAdminByHandle(hClient, pCallContext->secInfo.privLevel))
        {
            NV_PRINTF(LEVEL_ERROR, "insufficient permissions\n");
            status = NV_ERR_INSUFFICIENT_PERMISSIONS;
            goto fail_unset_event;
        }
    }
    else if (status != NV_OK)
    {
         NV_PRINTF(LEVEL_ERROR, "Capability validation failed\n");
         goto fail_unset_event;
    }

    fabricSetNodeId(pFabric, pUserParams->nodeId);
    pImexSessionApi->flags = pUserParams->flags;

    fabricEnableMemAlloc(pFabric);

    return NV_OK;

fail_unset_event:
    NV_ASSERT_OK(fabricSetImexEvent(pFabric, NULL));

fail_dereference:
    osDereferenceObjectCount(pImexOsEvent);

    return status;
}

void
imexsessionapiDestruct_IMPL
(
    ImexSessionApi *pImexSessionApi
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    Fabric *pFabric = SYS_GET_FABRIC(pSys);
    NvP64 pImexOsEvent = fabricGetImexEvent(pFabric);

    //
    // If the IMEX daemon shutdown (crashed) without explicitly cleaning up all
    // its state (e.g. duped exported memory), disable future fabric allocations.
    // The memory is released prematurely due to the IMEX  daemon crash could
    // still be in-use from other nodes and we don't want it to be re-allocated
    // (reused) until all the users (importers) are killed. Thus, until the
    // IMEX daemon restarts after appropriate recovery (admin controlled),
    // disable any future fabric allocations.
    //

    if (_checkDanglingExports(RES_GET_CLIENT(pImexSessionApi)))
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Disabling fabric allocations due to unclean IMEX shutdown!\n");
        fabricDisableMemAlloc(pFabric);
    }

    // Invalidate export cache to block future imports on this node ID.
    memoryexportClearCache(fabricGetNodeId(pFabric));

    // Invalidate IMEX event and cached node ID
    NV_ASSERT_OK(fabricSetImexEvent(pFabric, NULL));
    fabricSetNodeId(pFabric, NV_FABRIC_INVALID_NODE_ID);

    // Release event and IMEX cap
    osDereferenceObjectCount(pImexOsEvent);
    osRmCapRelease(pImexSessionApi->dupedCapDescriptor);

    // Flush pending events and wake up threads waiting on them
    fabricFlushUnhandledEvents(pFabric);
    fabricUnimportCacheIterateAll(pFabric, fabricWakeUpThreadCallback);

    if (!(pImexSessionApi->flags & NV00F1_ALLOC_DISABLE_CHANNEL_RECOVERY))
    {
        rcAndDisableOutstandingClientsWithImportedMemory(NULL, NV_FABRIC_INVALID_NODE_ID);
    }

    NV_PRINTF(LEVEL_WARNING, "IMEX daemon shutdown!\n");
}

NV_STATUS
imexsessionapiCtrlCmdGetFabricEvents_IMPL
(
    ImexSessionApi                          *pImexSessionApi,
    NV00F1_CTRL_GET_FABRIC_EVENTS_PARAMS    *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    Fabric *pFabric = SYS_GET_FABRIC(pSys);

    pParams->numEvents = NV00F1_CTRL_GET_FABRIC_EVENTS_ARRAY_SIZE;
    pParams->bMoreEvents = fabricExtractEventsV2(pFabric,
                                                 pParams->eventArray,
                                                 &pParams->numEvents);

    return NV_OK;
}

NV_STATUS
imexsessionapiCtrlCmdFinishMemUnimport_IMPL
(
    ImexSessionApi                         *pImexSessionApi,
    NV00F1_CTRL_FINISH_MEM_UNIMPORT_PARAMS *pParams
)
{
    OBJSYS *pSys = SYS_GET_INSTANCE();
    Fabric *pFabric = SYS_GET_FABRIC(pSys);
    NV00F1_CTRL_FABRIC_UNIMPORT_TOKEN *pTokenArray = pParams->tokenArray;
    NvU32 i;

    if ((pParams->numTokens == 0) ||
        (pParams->numTokens > NV00F1_CTRL_FINISH_MEM_UNIMPORT_ARRAY_SIZE))
        return NV_ERR_INVALID_ARGUMENT;

    for (i = 0; i < pParams->numTokens; i++)
    {
        //
        // Ignore errors if the unimport event ID is not found. This could
        // happen if thread has already exited.
        //
        (void)fabricUnimportCacheInvokeCallback(pFabric,
                                                pTokenArray[i].unimportEventId,
                                                fabricWakeUpThreadCallback);
    }

    return NV_OK;
}

NV_STATUS
imexsessionapiCtrlCmdDisableImporters_IMPL
(
    ImexSessionApi                       *pImexSessionApi,
    NV00F1_CTRL_DISABLE_IMPORTERS_PARAMS *pParams
)
{

    if (pImexSessionApi->flags & NV00F1_ALLOC_DISABLE_CHANNEL_RECOVERY)
        return NV_ERR_NOT_SUPPORTED;

    rcAndDisableOutstandingClientsWithImportedMemory(NULL, pParams->nodeId);

    return NV_OK;
}
