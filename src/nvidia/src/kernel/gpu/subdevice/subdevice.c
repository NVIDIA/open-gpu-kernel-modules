/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu_mgr/gpu_mgr.h"

#include "vgpu/rpc.h"
#include "core/locks.h"
#include "rmapi/rs_utils.h"
#include "core/thread_state.h"
#include "kernel/gpu/fifo/kernel_fifo.h"

#include "gpu/timer/objtmr.h"
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

//
// subdeviceUnsetDynamicBoostLimit_IMPL
//
// Unset Dynamic Boost limit when nvidia-powerd is terminated
//
NV_STATUS
subdeviceUnsetDynamicBoostLimit_IMPL
(
    Subdevice *pSubdevice
)
{
    if (!pSubdevice->bUpdateTGP)
        return NV_OK;

    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    return pRmApi->Control(pRmApi,
                           pGpu->hInternalClient,
                           pGpu->hInternalSubdevice,
                           NV2080_CTRL_CMD_INTERNAL_PMGR_UNSET_DYNAMIC_BOOST_LIMIT,
                           NULL,
                           0);
}

void
subdevicePreDestruct_IMPL
(
    Subdevice *pSubdevice
)
{
    subdeviceUnsetDynamicBoostLimit(pSubdevice);
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

    // check for any pending client's timer notification for this subdevice
    if ((pSubdevice->notifyActions[NV2080_NOTIFIERS_TIMER] != NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE) ||
        (pSubdevice->pTimerEvent != NULL))
    {
        OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
        tmrEventDestroy(pTmr, pSubdevice->pTimerEvent);
        pSubdevice->pTimerEvent = NULL;
        pSubdevice->notifyActions[NV2080_NOTIFIERS_TIMER] = NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE;
    }

    subdeviceRestoreLockedClock(pSubdevice, pCallContext);

    // Decrement the reference count for VF if previously incremented.
    subdeviceRestoreVF(pSubdevice, pCallContext);

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
    return subdeviceGetByDeviceAndGpu(pClient, NULL, pGpu, ppSubdevice);
}

NV_STATUS
subdeviceGetByDeviceAndGpu_IMPL
(
    RsClient         *pClient,
    Device           *pDevice,
    OBJGPU           *pGpu,
    Subdevice       **ppSubdevice
)
{
    Subdevice    *pSubdevice = NULL;
    RS_ITERATOR   it;
    RS_ITER_TYPE  iterType = RS_ITERATE_DESCENDANTS;
    RsResourceRef *pDeviceRef = NULL;

    *ppSubdevice = NULL;

    if (pDevice != NULL)
    {
        pDeviceRef = RES_GET_REF(pDevice);
        iterType = RS_ITERATE_CHILDREN;
    }

    it = clientRefIter(pClient, pDeviceRef, classId(Subdevice),
                       iterType, NV_TRUE);

    while (clientRefIterNext(pClient, &it))
    {
        pSubdevice = dynamicCast(it.pResourceRef->pResource, Subdevice);

        if (GPU_RES_GET_GPU(pSubdevice) == pGpu)
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

