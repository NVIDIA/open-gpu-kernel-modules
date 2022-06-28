/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
*       This is a subdevice resource implementation.
*
******************************************************************************/

#include "resserv/resserv.h"
#include "resserv/rs_server.h"
#include "resserv/rs_client.h"
#include "resserv/rs_resource.h"
#include "gpu/device/device.h"
#include "gpu/subdevice/subdevice.h"

#include "vgpu/rpc.h"
#include "core/locks.h"
#include "rmapi/rs_utils.h"
#include "core/thread_state.h"
#include "kernel/gpu/fifo/kernel_fifo.h"

#include "objtmr.h"
#include "kernel/gpu/rc/kernel_rc.h"
#include "Nvcm.h"
#include "gpu/bus/p2p_api.h"

NV_STATUS
subdeviceConstruct_IMPL
(
    Subdevice *pSubdevice,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV2080_ALLOC_PARAMETERS *pNv2080AllocParams = pParams->pAllocParams;
    OBJGPU                  *pPrimaryGpu;
    OBJGPU                  *pGpu;
    NvU32                    subDeviceInst;
    NV_STATUS                status = NV_OK;
    RsClient                *pRsClient = pCallContext->pClient;
    GpuResource             *pSubdevGpuRes = staticCast(pSubdevice, GpuResource);
    RsResourceRef           *pResourceRef = pCallContext->pResourceRef;
    RsResourceRef           *pParentRef = pResourceRef->pParentRef;
    Device                  *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    NvU32                    i;
    Subdevice               *pSubdeviceTest;

    if (pNv2080AllocParams == NULL)
        subDeviceInst = 0;
    else
        subDeviceInst = pNv2080AllocParams->subDeviceId;

    // validate subdevice instance
    if (gpumgrIsSubDeviceInstanceValid(subDeviceInst) == NV_FALSE)
        return NV_ERR_INVALID_CLASS;

    status = gpuGetByRef(pResourceRef->pParentRef, NULL, &pPrimaryGpu);
    if (status != NV_OK)
         return status;

    // Lookup GPU for subdevice instance
    status = gpugrpGetGpuFromSubDeviceInstance(GPU_RES_GET_GPUGRP(pDevice), subDeviceInst, &pGpu);
    if (status != NV_OK)
        return NV_ERR_INVALID_CLASS;

    // Check if subdevice already allocated
    if (subdeviceGetByInstance(pRsClient, RES_GET_HANDLE(pDevice), subDeviceInst, &pSubdeviceTest) == NV_OK)
        return NV_ERR_INSUFFICIENT_RESOURCES;

    gpuresSetGpu(pSubdevGpuRes, pGpu, NV_FALSE);

    pSubdevice->pDevice = pDevice;
    pSubdevice->deviceInst = pDevice->deviceInst;
    pSubdevice->subDeviceInst = subDeviceInst;
    pSubdevice->bUpdateTGP = NV_FALSE;

    for (i = 0; i < NV2080_NOTIFIERS_MAXCOUNT; i++)
        pSubdevice->notifyActions[i] = NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE;

    pSubdevice->hNotifierMemory = NV01_NULL_OBJECT;
    pSubdevice->hSemMemory = NV01_NULL_OBJECT;

    {
        //
        // If using thwap to generate an allocation failure here, fail the alloc
        // right away
        //
        KernelRc *pKernelRc = GPU_GET_KERNEL_RC(pPrimaryGpu);
        if (pKernelRc != NULL &&
            !krcTestAllowAlloc(pPrimaryGpu, pKernelRc,
                               NV_ROBUST_CHANNEL_ALLOCFAIL_SUBDEVICE))
        {
            return NV_ERR_GENERIC;
        }
    }

    NV_ASSERT_OK_OR_RETURN(gpuRegisterSubdevice(pGpu, pSubdevice));

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        NV_RM_RPC_ALLOC_SUBDEVICE(pPrimaryGpu, pRsClient->hClient, pParentRef->hResource,
                              pResourceRef->hResource, NV20_SUBDEVICE_0,
                              subDeviceInst, status);
        NV_ASSERT_OK_OR_RETURN(status);
    }

    return status;
}

void
subdevicePreDestruct_IMPL
(
    Subdevice *pSubdevice
)
{
    subdeviceResetTGP(pSubdevice);
}

void
subdeviceDestruct_IMPL
(
    Subdevice* pSubdevice
)
{
    CALL_CONTEXT           *pCallContext;
    RsClient               *pRsClient       = RES_GET_CLIENT(pSubdevice);
    RsResourceRef          *pResourceRef    = RES_GET_REF(pSubdevice);
    OBJGPU                 *pGpu            = GPU_RES_GET_GPU(pSubdevice);
    NV_STATUS               status          = NV_OK;

    if (pSubdevice->bGcoffDisallowed)
    {
        osClientGcoffDisallowRefcount(pGpu->pOsGpuInfo, NV_FALSE);
    }

    LOCK_METER_DATA(FREE_SUBDEVICE, 0, 0, 0);

    // TODO - Call context lookup in dtor can likely be phased out now that we have RES_GET_CLIENT
    resGetFreeParams(staticCast(pSubdevice, RsResource), &pCallContext, NULL);

    // free P2P objects associated with this subDevice
    // can't rely on resource server to clean up since object exists in both lists
    if (NULL != pSubdevice->pP2PMappingList)
    {
        CliFreeSubDeviceP2PList(pSubdevice, pCallContext);
    }

    // check for any pending client's timer notification for this subdevice
    if (pSubdevice->notifyActions[NV2080_NOTIFIERS_TIMER] != NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE)
    {
        OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
        tmrCancelCallback(pTmr, pSubdevice);
        pSubdevice->notifyActions[NV2080_NOTIFIERS_TIMER] = NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE;
    }

    subdeviceRestoreLockedClock(pSubdevice, pCallContext);

    // Restore GR tick frequency to default.
    subdeviceRestoreGrTickFreq(pSubdevice, pCallContext);

    // Remove NVLink error injection mode request
    subdeviceReleaseNvlinkErrorInjectionMode(pSubdevice, pCallContext);

    subdeviceReleaseComputeModeReservation(pSubdevice, pCallContext);

#ifdef DEBUG
    NV_ASSERT(pSubdevice->notifyActions[NV2080_NOTIFIERS_TIMER] == NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE);
#endif

    subdeviceUnsetGpuDebugMode(pSubdevice);
    subdeviceRestoreWatchdog(pSubdevice);

    if (pResourceRef != NULL && (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu)))
    {
        NV_RM_RPC_FREE(pGpu, pRsClient->hClient,
                       pResourceRef->pParentRef->hResource,
                       pResourceRef->hResource, status);
    }
    //
    // Restore sched settings
    //
    if (pSubdevice->bSchedPolicySet)
    {
        if (NV_OK == kfifoRestoreSchedPolicy(pGpu, GPU_GET_KERNEL_FIFO(pGpu)))
        {
            pSubdevice->bSchedPolicySet = NV_FALSE;
        }
    }

    gpuUnregisterSubdevice(pGpu, pSubdevice);
}

NV_STATUS
subdeviceInternalControlForward_IMPL
(
    Subdevice *pSubdevice,
    NvU32 command,
    void *pParams,
    NvU32 size
)
{
    return gpuresInternalControlForward_IMPL(staticCast(pSubdevice, GpuResource), command, pParams, size);
}

NV_STATUS
subdeviceControlFilter_IMPL(Subdevice *pSubdevice,
                            CALL_CONTEXT *pCallContext,
                            RS_RES_CONTROL_PARAMS_INTERNAL *pParams)
{
    return NV_OK;
}

NV_STATUS
subdeviceAddP2PApi_IMPL
(
    Subdevice    *pSubdevice,
    P2PApi       *pP2PApi
)
{
    PNODE               pNode;
    NvHandle            hPeerSubDevice;
    NV_STATUS           status;
    PCLI_P2P_INFO_LIST *pP2PInfoList;
    NvHandle          hSubDevice = RES_GET_HANDLE(pSubdevice);

    if (NULL == pP2PApi || NULL == pP2PApi->peer1 || NULL == pP2PApi->peer2)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // in case of loopback, both handles are the same and this will not matter
    // otherwise we need the peer subdevice handle
    //
    hPeerSubDevice = (RES_GET_HANDLE(pP2PApi->peer1) == hSubDevice) ?
                      RES_GET_HANDLE(pP2PApi->peer2) :
                      RES_GET_HANDLE(pP2PApi->peer1);

    if (NV_OK != btreeSearch(hPeerSubDevice, &pNode,
                             pSubdevice->pP2PMappingList))
    {
        pP2PInfoList = portMemAllocNonPaged(sizeof(PCLI_P2P_INFO_LIST));
        if (pP2PInfoList == NULL)
        {
            status = NV_ERR_INSUFFICIENT_RESOURCES;
            goto failed;
        }

        listInit(pP2PInfoList, portMemAllocatorGetGlobalNonPaged());

        pNode = portMemAllocNonPaged(sizeof(NODE));
        if (pNode == NULL)
        {
            status = NV_ERR_INSUFFICIENT_RESOURCES;
            goto failed;
        }

        portMemSet(pNode, 0, sizeof(NODE));
        pNode->keyStart = hPeerSubDevice;
        pNode->keyEnd = hPeerSubDevice;
        pNode->Data = pP2PInfoList;

        status = btreeInsert(pNode, &pSubdevice->pP2PMappingList);
failed:
        if (NV_OK != status)
        {
            portMemFree(pNode);
            portMemFree(pP2PInfoList);
            return status;
        }
    }
    else
    {
        pP2PInfoList = pNode->Data;
    }

    listAppendValue(pP2PInfoList, &pP2PApi);

    return NV_OK;
}

NV_STATUS
subdeviceDelP2PApi_IMPL
(
    Subdevice    *pSubdevice,
    P2PApi       *pP2PApi
)
{
    PCLI_P2P_INFO_LIST *pP2PInfoList;
    PNODE               pNode;
    NV_STATUS           status;
    NvHandle            hPeerSubDevice;
    NvHandle            hSubDevice = RES_GET_HANDLE(pSubdevice);

    //
    // in case of loopback, both handles are the same and this will not matter
    // otherwise we need the peer subdevice handle
    //
    hPeerSubDevice = (RES_GET_HANDLE(pP2PApi->peer1) == hSubDevice) ?
                      RES_GET_HANDLE(pP2PApi->peer2) :
                      RES_GET_HANDLE(pP2PApi->peer1);

    if (NV_OK != (status = btreeSearch(hPeerSubDevice, &pNode, pSubdevice->pP2PMappingList)))
        return status;

    pP2PInfoList = pNode->Data;

    listRemoveFirstByValue(pP2PInfoList, &pP2PApi);
    if (listCount(pP2PInfoList) == 0)
    {
        if (NV_OK != (status = btreeUnlink(pNode, &pSubdevice->pP2PMappingList)))
        {
            return status;
        }

        pNode->Data = NULL;
        portMemFree(pNode);
        pNode = NULL;
        portMemFree(pP2PInfoList);
        pP2PInfoList = NULL;
    }

    return NV_OK;
}

NV_STATUS
subdeviceGetByHandle_IMPL
(
    RsClient         *pClient,
    NvHandle          hSubdevice,
    Subdevice       **ppSubdevice
)
{
    RsResourceRef  *pResourceRef;
    NV_STATUS       status;

    *ppSubdevice = NULL;

    status = clientGetResourceRef(pClient, hSubdevice, &pResourceRef);
    if (status != NV_OK)
        return status;

    *ppSubdevice = dynamicCast(pResourceRef->pResource, Subdevice);

    return (*ppSubdevice) ? NV_OK : NV_ERR_INVALID_OBJECT_HANDLE;
}

NV_STATUS
subdeviceGetByGpu_IMPL
(
    RsClient         *pClient,
    OBJGPU           *pGpu,
    Subdevice       **ppSubdevice
)
{
    Subdevice          *pSubdevice = NULL;
    OBJGPU             *pTmpGpu = NULL;
    RS_ITERATOR         it;
    RsResourceRef      *pResourceRef;

    *ppSubdevice = NULL;

    it = clientRefIter(pClient, NULL, classId(Subdevice), RS_ITERATE_DESCENDANTS, NV_TRUE);
    while (clientRefIterNext(pClient, &it))
    {
        pResourceRef = it.pResourceRef;
        pSubdevice = dynamicCast(pResourceRef->pResource, Subdevice);
        if (pSubdevice == NULL)
            continue;

        pTmpGpu = GPU_RES_GET_GPU(pSubdevice);

        if (pTmpGpu == pGpu)
        {
            *ppSubdevice = pSubdevice;
            return NV_OK;
        }
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

NV_STATUS
subdeviceGetByInstance_IMPL
(
    RsClient         *pClient,
    NvHandle          hDevice,
    NvU32             subDeviceInst,
    Subdevice       **ppSubdevice
)
{
    RsResourceRef      *pDeviceRef;
    Subdevice          *pSubdevice;
    RS_ITERATOR         it;

    *ppSubdevice = NULL;

    if (clientGetResourceRefByType(pClient, hDevice, classId(Device), &pDeviceRef) != NV_OK)
        return NV_ERR_INVALID_ARGUMENT;

    it = clientRefIter(pClient, pDeviceRef, classId(Subdevice), RS_ITERATE_CHILDREN, NV_TRUE);
    while (clientRefIterNext(pClient, &it))
    {
        pSubdevice = dynamicCast(it.pResourceRef->pResource, Subdevice);

        if (pSubdevice && pSubdevice->subDeviceInst == subDeviceInst)
        {
            *ppSubdevice = pSubdevice;
            return NV_OK;
        }
    }

    return NV_ERR_OBJECT_NOT_FOUND;
}

// ****************************************************************************
//                              Helper functions
// ****************************************************************************
void
subdeviceUnsetGpuDebugMode_IMPL
(
    Subdevice *pSubdevice
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);

    if (!pSubdevice->bGpuDebugModeEnabled)
    {
        return;
    }

    GPU_RES_SET_THREAD_BC_STATE(pSubdevice);

    pGpu->bIsDebugModeEnabled = NV_FALSE;
}

void
subdeviceReleaseComputeModeReservation_IMPL
(
    Subdevice       *pSubdevice,
    CALL_CONTEXT    *pCallContext
)
{
    RsClient *pRsClient = pCallContext->pClient;
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pSubdevice);
    GPU_RES_SET_THREAD_BC_STATE(pSubdevice);

    // Release the reservation ONLY IF we had the reservation to begin with. Otherwise,
    // leave it alone, because someone else has acquired it:
    if (pGpu->hComputeModeReservation == pRsClient->hClient)
    {
        pGpu->hComputeModeReservation = NV01_NULL_OBJECT;
    }
}

void
subdeviceRestoreGrTickFreq_IMPL
(
    Subdevice *pSubdevice,
    CALL_CONTEXT *pCallContext
)
{
    if (!pSubdevice->bMaxGrTickFreqRequested)
        return;

    OBJGPU  *pGpu = GPU_RES_GET_GPU(pSubdevice);
    OBJTMR  *pTmr;
    RsClient *pRsClient = pCallContext->pClient;
    RsResourceRef *pSubdeviceRef = pCallContext->pResourceRef;

    GPU_RES_SET_THREAD_BC_STATE(pSubdevice);

    if (pSubdeviceRef)
    {
        pTmr = GPU_GET_TIMER(pGpu);
        refcntReleaseReferences(pTmr->pGrTickFreqRefcnt,
                NV_REQUESTER_CLIENT_OBJECT(pRsClient->hClient, pSubdeviceRef->hResource), NV_TRUE);
        pSubdevice->bMaxGrTickFreqRequested = NV_FALSE;
    }
}

void
subdeviceRestoreWatchdog_IMPL
(
    Subdevice *pSubdevice
)
{
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelRc *pKernelRc;

    GPU_RES_SET_THREAD_BC_STATE(pSubdevice);

    pKernelRc = GPU_GET_KERNEL_RC(pGpu);
    NV_CHECK_OR_RETURN_VOID(LEVEL_INFO, pKernelRc != NULL);
    krcWatchdogChangeState(pKernelRc, pSubdevice, RM_CLIENT_DESTRUCTION);
}

// ****************************************************************************
//                            Deprecated Functions
// ****************************************************************************

/**
 * WARNING: This function is deprecated! Please use subdeviceGetByGpu and
 * GPU_RES_SET_THREAD_BC_STATE (if needed to set thread UC state for SLI)
 */
Subdevice *
CliGetSubDeviceInfoFromGpu
(
    NvHandle hClient,
    OBJGPU  *pGpu
)
{
    RsClient   *pClient;
    NV_STATUS   status;
    Subdevice  *pSubdevice;

    status = serverGetClientUnderLock(&g_resServ, hClient, &pClient);
    if (status != NV_OK)
        return NULL;

    status = subdeviceGetByGpu(pClient, pGpu, &pSubdevice);
    if (status != NV_OK)
        return NULL;

    GPU_RES_SET_THREAD_BC_STATE(pSubdevice);

    return pSubdevice;
}

/**
 * WARNING: This function is deprecated! Please use subdeviceGetByGpu and
 * RES_GET_HANDLE
 */
NV_STATUS
CliGetSubDeviceHandleFromGpu
(
    NvHandle    hClient,
    OBJGPU     *pGpu,
    NvHandle   *phSubDevice
)
{
    Subdevice *pSubdevice;

    if (phSubDevice == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if ((pSubdevice = CliGetSubDeviceInfoFromGpu(hClient, pGpu)) == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    *phSubDevice = RES_GET_HANDLE(pSubdevice);

    return NV_OK;
}

/**
 * WARNING: This function is deprecated and use is *strongly* discouraged
 * (especially for new code!)
 *
 * From the function name (CliSetSubDeviceContext) it appears as a simple
 * accessor but violates expectations by modifying the SLI BC threadstate (calls
 * to GPU_RES_SET_THREAD_BC_STATE). This can be dangerous if not carefully
 * managed by the caller.
 *
 * Instead of using this routine, please use subdeviceGetByHandle then call
 * GPU_RES_GET_GPU, RES_GET_HANDLE, GPU_RES_SET_THREAD_BC_STATE as needed.
 *
 * Note that GPU_RES_GET_GPU supports returning a pGpu for both pDevice,
 * pSubdevice, the base pResource type, and any resource that inherits from
 * GpuResource. That is, instead of using CliSetGpuContext or
 * CliSetSubDeviceContext, please use following pattern to look up the pGpu:
 *
 * OBJGPU *pGpu = GPU_RES_GET_GPU(pResource or pResourceRef->pResource)
 *
 * To set the threadstate, please use:
 *
 * GPU_RES_SET_THREAD_BC_STATE(pResource or pResourceRef->pResource);
 */
NV_STATUS
CliSetSubDeviceContext
(
    NvHandle  hClient,
    NvHandle  hSubdevice,
    NvHandle *phDevice,
    OBJGPU  **ppGpu
)
{
    Subdevice *pSubdevice;
    RsClient  *pClient;
    NV_STATUS  status;

    if (phDevice != NULL)
    {
        *phDevice = 0;
    }
    *ppGpu = NULL;

    status = serverGetClientUnderLock(&g_resServ, hClient, &pClient);
    if (status != NV_OK)
        return status;

    status = subdeviceGetByHandle(pClient, hSubdevice, &pSubdevice);
    if (status != NV_OK)
        return status;

    *ppGpu = GPU_RES_GET_GPU(pSubdevice);
    if (phDevice != NULL)
    {
        *phDevice = RES_GET_HANDLE(pSubdevice->pDevice);
    }

    GPU_RES_SET_THREAD_BC_STATE(pSubdevice);

    return NV_OK;
}
