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

#include "class/cl0040.h" /* NV01_MEMORY_LOCAL_USER */
#include "class/cl84a0.h" /* NV01_MEMORY_LIST_XXX */
#include "class/cl00b1.h" /* NV01_MEMORY_HW_RESOURCES */

#include "nverror.h"

#include "gpu/gpu.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/device/device.h"
#include "gpu/subdevice/subdevice.h"
#include "rmapi/rs_utils.h"
#include "rmapi/rmapi.h"
#include "rmapi/client.h"
#include "rmapi/resource_fwd_decls.h"
#include "core/thread_state.h"
#include "virtualization/kernel_hostvgpudeviceapi.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/fifo/kernel_channel.h"

NV_STATUS
gpuSetExternalKernelClientCount_IMPL(OBJGPU *pGpu, NvBool bIncr)
{
    if (bIncr)
    {
        pGpu->externalKernelClientCount++;
    }
    else
    {
        NV_ASSERT_OR_RETURN(pGpu->externalKernelClientCount > 0, NV_ERR_INVALID_OPERATION);
        pGpu->externalKernelClientCount--;
    }

    return NV_OK;
}

// Get the count of user clients that are using given gpu
static NvU32
_gpuGetUserClientCount
(
    OBJGPU *pGpu,
    NvBool  bCount
)
{
    NvU32      count = 0;
    Device    *pDevice;
    RmClient **ppClient;
    RmClient  *pClient;
    RsClient  *pRsClient;
    NV_STATUS  status;

    // Search list of clients for any that have an InUse ref to the gpu
    for (ppClient = serverutilGetFirstClientUnderLock();
         ppClient;
         ppClient = serverutilGetNextClientUnderLock(ppClient))
    {
        pClient = *ppClient;
        pRsClient = staticCast(pClient, RsClient);

        // Skip internal client
        if (pRsClient->type == CLIENT_TYPE_KERNEL)
            continue;

        status = deviceGetByGpu(pRsClient, pGpu, NV_TRUE /* bAnyInGroup */, &pDevice);

        if (status != NV_OK)
            continue;

        count++;

        if (!bCount)
            break;
    }

    return count;
}

NvBool
gpuIsInUse_IMPL
(
    OBJGPU *pGpu
)
{
    return !!_gpuGetUserClientCount(pGpu, NV_FALSE) ||
           (pGpu->externalKernelClientCount > 0);
}

// Get the count of user clients that are using given gpu
NvU32
gpuGetUserClientCount_IMPL
(
    OBJGPU *pGpu
)
{
    return _gpuGetUserClientCount(pGpu, NV_TRUE);
}

// Get the count of external clients (User+External modules) that are using given gpu
NvU32
gpuGetExternalClientCount_IMPL
(
    OBJGPU *pGpu
)
{
    return _gpuGetUserClientCount(pGpu, NV_TRUE) + pGpu->externalKernelClientCount;
}

/**
 * Find the GPU associated with a resource reference in this order:
 *
 * 1. Directly from the RsResource if the resource is a Device or Subdevice
 * 2. From an ancestor subdevice (if any)
 * 3. From an ancestor device (if any)
 *
 * If the resource your querying is guaranteed to be a GpuResource you should
 * directly call GPU_RES_GET_GPU()
 *
 * @param[out] pbBroadcast True if the found GPU corresponds to a device
 *             [optional]
 */
NV_STATUS
gpuGetByRef
(
    RsResourceRef *pContextRef,
    NvBool        *pbBroadcast,
    OBJGPU       **ppGpu
)
{
    NV_STATUS      status = NV_OK;
    RsResourceRef *pDeviceRef;
    RsResourceRef *pSubdeviceRef;
    GpuResource   *pGpuResource;

    if (ppGpu != NULL)
        *ppGpu = NULL;

    if (pContextRef == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    pGpuResource = dynamicCast(pContextRef->pResource, GpuResource);

    //
    // NULL check on GpuResource::pGpu as this routine is used from within
    // GpuResource::Construct to initialize GpuResource::pGpu
    //
    if ((pGpuResource == NULL) || (pGpuResource->pGpu == NULL))
    {
        status = refFindAncestorOfType(pContextRef, classId(Subdevice), &pSubdeviceRef);
        if (status == NV_OK)
        {
            pGpuResource = dynamicCast(pSubdeviceRef->pResource, GpuResource);
            if ((pGpuResource == NULL) || (pGpuResource->pGpu == NULL))
                status = NV_ERR_OBJECT_NOT_FOUND;
        }

        if (status != NV_OK)
        {
            status = refFindAncestorOfType(pContextRef, classId(Device), &pDeviceRef);
            if (status == NV_OK)
            {
                pGpuResource = dynamicCast(pDeviceRef->pResource, GpuResource);
                if ((pGpuResource == NULL) || (pGpuResource->pGpu == NULL))
                    status = NV_ERR_OBJECT_NOT_FOUND;
            }
        }
    }

    if (status == NV_OK)
    {
        if (pbBroadcast != NULL)
            *pbBroadcast = pGpuResource->bBcResource;

        if (ppGpu != NULL)
            *ppGpu = pGpuResource->pGpu;
    }

    return status;
}

/**
 * Wrapper for gpuGetByRef that takes a pClient + hResource instead of a
 * pResourceRef.
 *
 * Find the GPU associated with a resource;
 */
NV_STATUS
gpuGetByHandle
(
    RsClient      *pClient,
    NvHandle       hResource,
    NvBool        *pbBroadcast,
    OBJGPU       **ppGpu
)
{
    RsResourceRef    *pResourceRef;
    NV_STATUS         status;

    if (ppGpu != NULL)
        *ppGpu = NULL;

    status = clientGetResourceRef(pClient, hResource, &pResourceRef);
    if (status != NV_OK)
        return status;

    return gpuGetByRef(pResourceRef, pbBroadcast, ppGpu);
}

NV_STATUS gpuRegisterSubdevice_IMPL(OBJGPU *pGpu, Subdevice *pSubdevice)
{
    const NvU32 initialSize = 32;
    const NvU32 expansionFactor = 2;

    if (pGpu->numSubdeviceBackReferences == pGpu->maxSubdeviceBackReferences)
    {
        if (pGpu->pSubdeviceBackReferences == NULL)
        {
            pGpu->pSubdeviceBackReferences = portMemAllocNonPaged(initialSize * sizeof(Subdevice*));
            if (pGpu->pSubdeviceBackReferences == NULL)
                return NV_ERR_NO_MEMORY;
            pGpu->maxSubdeviceBackReferences = initialSize;
        }
        else
        {
            const NvU32 newSize = expansionFactor * pGpu->maxSubdeviceBackReferences * sizeof(Subdevice*);
            Subdevice **newArray = portMemAllocNonPaged(newSize);
            if (newArray == NULL)
                return NV_ERR_NO_MEMORY;

            portMemCopy(newArray, newSize, pGpu->pSubdeviceBackReferences, pGpu->maxSubdeviceBackReferences * sizeof(Subdevice*));
            portMemFree(pGpu->pSubdeviceBackReferences);
            pGpu->pSubdeviceBackReferences = newArray;
            pGpu->maxSubdeviceBackReferences *= expansionFactor;
        }
    }
    pGpu->pSubdeviceBackReferences[pGpu->numSubdeviceBackReferences++] = pSubdevice;
    return NV_OK;
}

void gpuUnregisterSubdevice_IMPL(OBJGPU *pGpu, Subdevice *pSubdevice)
{
    NvU32 i;
    for (i = 0; i < pGpu->numSubdeviceBackReferences; i++)
    {
        if (pGpu->pSubdeviceBackReferences[i] == pSubdevice)
        {
            pGpu->numSubdeviceBackReferences--;
            pGpu->pSubdeviceBackReferences[i] = pGpu->pSubdeviceBackReferences[pGpu->numSubdeviceBackReferences];
            pGpu->pSubdeviceBackReferences[pGpu->numSubdeviceBackReferences] = NULL;
            return;
        }
    }
    NV_ASSERT_FAILED("Subdevice not found!");
}

/*!
 * @brief Determine whether the given event should be triggered on the given
 * subdevice based upon MIG attribution, and translate encoded global IDs into
 * instance-local IDs if necessary
 *
 * @param[in]     pSubdevice  Subdevice under consideration for notification
 * @param[in,out] pNotifyType Current notify type which may be overwritten
 * @param[in,out] pInfo32     Current generic 32 bit info which may be
 *                            overwritten
 * @param[in] rcInstanceAttributionId MIG instance attribution valid only if
 *                                    *pNotifyType == NV2080_NOTIFIERS_RC_ERROR
 *
 * @return NV_OK                           if subdevice should be notified.
 *                                         pNotifyType and pInfo32 may be
 *                                         overwritten with new values
 *         NV_ERR_INSUFFICIENT_PERMISSIONS if client is not subscribed and
 *                                         unprivileged, and therefore should
 *                                         not be notified
 *         NV_ERR_OBJECT_NOT_FOUND         if client is subscribed, but the
 *                                         notification references an object
 *                                         which is not part of their instance
 *                                         and therefore should not be notified.
 *         NV_ERR_INVALID_ARGUMENT         if NULL is provided for parameters
 */
static NV_STATUS
_gpuFilterSubDeviceEventInfo
(
    OBJGPU *pGpu,
    Subdevice *pSubdevice,
    NvU32 *pNotifyType,
    NvU32 *pInfo32,
    NvU16  rcInstanceAttributionId
)
{
    MIG_INSTANCE_REF ref;
    Device *pDevice = GPU_RES_GET_DEVICE(pSubdevice);
    NvU32 engineIdx;
    RM_ENGINE_TYPE rmEngineType;
    RM_ENGINE_TYPE localRmEngineType;
    NvU32 localIdx;
    NV_STATUS status = NV_OK;
    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);

    NV_ASSERT_OR_RETURN(pNotifyType != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pInfo32 != NULL, NV_ERR_INVALID_ARGUMENT);

    // No filtering needed if no instances
    if (!IS_MIG_IN_USE(pGpu))
        return NV_OK;

    // Retrieve instance reference for this Device
    status = kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice, &ref);
    if (status != NV_OK)
    {
        RmClient *pRmClient = dynamicCast(RES_GET_CLIENT(pSubdevice), RmClient);
        NV_ASSERT_OR_RETURN(NULL != pRmClient, NV_ERR_INVALID_CLIENT);

        //
        // Privileged or has the mig monitor capability, unsubscribed clients
        // may be notified with no filtering, but other unsubscribed clients
        // must not be notified
        //
        RS_PRIV_LEVEL privLevel = rmclientGetCachedPrivilege(pRmClient);
        NV_CHECK_OR_RETURN(LEVEL_INFO,
                           rmclientIsCapableOrAdmin(pRmClient, NV_RM_CAP_SYS_SMC_MONITOR, privLevel),
                           NV_ERR_INSUFFICIENT_PERMISSIONS);
        return NV_OK;
    }

    // Determine whether we need to convert global IDs to local
    if (*pNotifyType == NV2080_NOTIFIERS_RC_ERROR)
    {
        //
        // If this error was attributed, don't notify unless this client is
        // subscribed to the attributed instance
        //
        if (kmigmgrIsInstanceAttributionIdValid(rcInstanceAttributionId))
        {
            NV_CHECK_OR_RETURN(LEVEL_INFO,
                               kmigmgrGetAttributionIdFromMIGReference(ref) ==
                                   rcInstanceAttributionId,
                               NV_ERR_OBJECT_NOT_FOUND);
        }

        //
        // RC errors encode the global error type in the *pInfo32,
        // these need to be converted to local IDs
        //
        if (ROBUST_CHANNEL_IS_CE_ERROR(*pInfo32))
        {
            engineIdx = ROBUST_CHANNEL_CE_ERROR_IDX(*pInfo32);
            rmEngineType = RM_ENGINE_TYPE_COPY(engineIdx);

            if (!kmigmgrIsEngineInInstance(pGpu, pKernelMIGManager, rmEngineType, ref))
                return NV_ERR_OBJECT_NOT_FOUND;

            NV_ASSERT_OK_OR_RETURN(
                kmigmgrGetGlobalToLocalEngineType(pGpu, pKernelMIGManager, ref,
                                                  rmEngineType,
                                                  &localRmEngineType));
            localIdx = RM_ENGINE_TYPE_COPY_IDX(localRmEngineType);
            *pInfo32 = ROBUST_CHANNEL_CE_ERROR(localIdx);
        }
        else if (ROBUST_CHANNEL_IS_NVDEC_ERROR(*pInfo32))
        {
            engineIdx = ROBUST_CHANNEL_NVDEC_ERROR_IDX(*pInfo32);
            rmEngineType = RM_ENGINE_TYPE_NVDEC(engineIdx);

            if (!kmigmgrIsEngineInInstance(pGpu, pKernelMIGManager, rmEngineType, ref))
                return NV_ERR_OBJECT_NOT_FOUND;

            NV_ASSERT_OK_OR_RETURN(
                kmigmgrGetGlobalToLocalEngineType(pGpu, pKernelMIGManager, ref,
                                                  rmEngineType,
                                                  &localRmEngineType));
            localIdx = RM_ENGINE_TYPE_NVDEC_IDX(localRmEngineType);
            *pInfo32 = ROBUST_CHANNEL_NVDEC_ERROR(localIdx);
        }
        else if (ROBUST_CHANNEL_IS_NVENC_ERROR(*pInfo32))
        {
            engineIdx = ROBUST_CHANNEL_NVENC_ERROR_IDX(*pInfo32);
            rmEngineType = RM_ENGINE_TYPE_NVENC(engineIdx);

            if (!kmigmgrIsEngineInInstance(pGpu, pKernelMIGManager, rmEngineType, ref))
                return NV_ERR_OBJECT_NOT_FOUND;

            NV_ASSERT_OK_OR_RETURN(
                kmigmgrGetGlobalToLocalEngineType(pGpu, pKernelMIGManager, ref,
                                                  rmEngineType,
                                                  &localRmEngineType));
            localIdx = RM_ENGINE_TYPE_NVENC_IDX(localRmEngineType);
            *pInfo32 = ROBUST_CHANNEL_NVENC_ERROR(localIdx);
        }
        else if (ROBUST_CHANNEL_IS_OFA_ERROR(*pInfo32))
        {
            engineIdx = ROBUST_CHANNEL_OFA_ERROR_IDX(*pInfo32);
            rmEngineType = RM_ENGINE_TYPE_OFA(engineIdx);

            if (!kmigmgrIsEngineInInstance(pGpu, pKernelMIGManager, rmEngineType, ref))
                return NV_ERR_OBJECT_NOT_FOUND;

            NV_ASSERT_OK_OR_RETURN(
                kmigmgrGetGlobalToLocalEngineType(pGpu, pKernelMIGManager, ref,
                                                  rmEngineType,
                                                  &localRmEngineType));
            localIdx = RM_ENGINE_TYPE_OFA_IDX(localRmEngineType);
            *pInfo32 = ROBUST_CHANNEL_OFA_ERROR(localIdx);
        }
    }
    else if (NV2080_NOTIFIER_TYPE_IS_GR(*pNotifyType))
    {
        engineIdx = NV2080_NOTIFIERS_GR_IDX(*pNotifyType);
        rmEngineType = RM_ENGINE_TYPE_GR(engineIdx);

        if (!kmigmgrIsEngineInInstance(pGpu, pKernelMIGManager, rmEngineType, ref))
            return NV_ERR_OBJECT_NOT_FOUND;

        NV_ASSERT_OK_OR_RETURN(
            kmigmgrGetGlobalToLocalEngineType(pGpu, pKernelMIGManager, ref,
                                              rmEngineType,
                                              &localRmEngineType));
        localIdx = RM_ENGINE_TYPE_GR_IDX(localRmEngineType);
        *pNotifyType = NV2080_NOTIFIERS_GR(localIdx);
    }
    else if (NV2080_NOTIFIER_TYPE_IS_CE(*pNotifyType))
    {
        engineIdx = NV2080_NOTIFIERS_CE_IDX(*pNotifyType);
        rmEngineType = RM_ENGINE_TYPE_COPY(engineIdx);

        if (!kmigmgrIsEngineInInstance(pGpu, pKernelMIGManager, rmEngineType, ref))
            return NV_ERR_OBJECT_NOT_FOUND;

        NV_ASSERT_OK_OR_RETURN(
            kmigmgrGetGlobalToLocalEngineType(pGpu, pKernelMIGManager, ref,
                                              rmEngineType,
                                              &localRmEngineType));
        localIdx = RM_ENGINE_TYPE_COPY_IDX(localRmEngineType);
        *pNotifyType = NV2080_NOTIFIERS_CE(localIdx);
    }
    else if (NV2080_NOTIFIER_TYPE_IS_NVDEC(*pNotifyType))
    {
        engineIdx = NV2080_NOTIFIERS_NVDEC_IDX(*pNotifyType);
        rmEngineType = RM_ENGINE_TYPE_NVDEC(engineIdx);

        if (!kmigmgrIsEngineInInstance(pGpu, pKernelMIGManager, rmEngineType, ref))
            return NV_ERR_OBJECT_NOT_FOUND;

        NV_ASSERT_OK_OR_RETURN(
            kmigmgrGetGlobalToLocalEngineType(pGpu, pKernelMIGManager, ref,
                                              rmEngineType,
                                              &localRmEngineType));
        localIdx = RM_ENGINE_TYPE_NVDEC_IDX(localRmEngineType);
        *pNotifyType = NV2080_NOTIFIERS_NVDEC(localIdx);
    }
    else if (NV2080_NOTIFIER_TYPE_IS_NVENC(*pNotifyType))
    {
        engineIdx = NV2080_NOTIFIERS_NVENC_IDX(*pNotifyType);
        rmEngineType = RM_ENGINE_TYPE_NVENC(engineIdx);

        if (!kmigmgrIsEngineInInstance(pGpu, pKernelMIGManager, rmEngineType, ref))
            return NV_ERR_OBJECT_NOT_FOUND;

        NV_ASSERT_OK_OR_RETURN(
            kmigmgrGetGlobalToLocalEngineType(pGpu, pKernelMIGManager, ref,
                                              rmEngineType,
                                              &localRmEngineType));
        localIdx = RM_ENGINE_TYPE_NVENC_IDX(localRmEngineType);
        *pNotifyType = NV2080_NOTIFIERS_NVENC(localIdx);
    }
    else if (NV2080_NOTIFIER_TYPE_IS_OFA(*pNotifyType))
    {
        engineIdx = NV2080_NOTIFIERS_OFA_IDX(*pNotifyType);
        rmEngineType = RM_ENGINE_TYPE_OFA(engineIdx);

        if (!kmigmgrIsEngineInInstance(pGpu, pKernelMIGManager, rmEngineType, ref))
            return NV_ERR_OBJECT_NOT_FOUND;

        NV_ASSERT_OK_OR_RETURN(
            kmigmgrGetGlobalToLocalEngineType(pGpu, pKernelMIGManager, ref,
                                              rmEngineType,
                                              &localRmEngineType));
        localIdx = RM_ENGINE_TYPE_OFA_IDX(localRmEngineType);
        *pNotifyType = NV2080_NOTIFIERS_OFAn(localIdx);
    }
    return NV_OK;
}

//
// For a particular gpu, find all the clients waiting for a particular event,
// fill in the notifier if allocated, and raise an event to the client if registered.
//
void
gpuNotifySubDeviceEvent_IMPL
(
    OBJGPU *pGpu,
    NvU32   notifyIndex,
    void   *pNotifyParams,
    NvU32   notifyParamsSize,
    NvV32   info32,
    NvV16   info16
)
{
    PEVENTNOTIFICATION pEventNotification;
    THREAD_STATE_NODE *pCurThread;
    NvU32 localNotifyType;
    NvU32 localInfo32;
    NvU32 i;

    if (NV_OK == threadStateGetCurrent(&pCurThread, pGpu))
    {
        // This function shouldn't be used from lockless ISR.
        // Use engineNonStallIntrNotify() to notify event from lockless ISR.
        NV_ASSERT_OR_RETURN_VOID(!(pCurThread->flags & THREAD_STATE_FLAGS_IS_ISR_LOCKLESS));
    }

    NV_ASSERT(notifyIndex < NV2080_NOTIFIERS_MAXCOUNT);

    // search notifiers with events hooked up for this gpu
    for (i = 0; i < pGpu->numSubdeviceBackReferences; i++)
    {
        Subdevice *pSubdevice = pGpu->pSubdeviceBackReferences[i];

        //
        // We've seen cases where pSubdevice is NULL implying that the
        // pSubdeviceBackReferences[] array is being modified during this loop.
        // Adding a NULL pointer check here is only a stopgap. See bug 3892382.
        //
        NV_ASSERT_OR_RETURN_VOID(pSubdevice != NULL);

        INotifier *pNotifier = staticCast(pSubdevice, INotifier);

        if (inotifyGetNotificationShare(pNotifier) == NULL)
            continue;

        GPU_RES_SET_THREAD_BC_STATE(pSubdevice);

        //
        // For SMC, partitioned engines have partition local IDs and events are
        // registered using partition localId while RM deals with global Ids.
        // Convert global to partition local if necessary
        //
        localNotifyType = notifyIndex;
        localInfo32 = info32;

        {
            NvU16 partitionAttributionId = info16;
            if (_gpuFilterSubDeviceEventInfo(pGpu, pSubdevice,
                                             &localNotifyType,
                                             &localInfo32,
                                             partitionAttributionId) != NV_OK)
            {
                continue;
            }
        }

        if (pSubdevice->notifyActions[localNotifyType] == NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE)
        {
            continue;
        }

        if (pSubdevice->hNotifierMemory != NV01_NULL_OBJECT &&
            pSubdevice->pNotifierMemory != NULL)
        {
            notifyFillNotifierMemory(pGpu, pSubdevice->pNotifierMemory, localInfo32, info16,
                NV2080_SUBDEVICE_NOTIFICATION_STATUS_DONE_SUCCESS, localNotifyType);
        }

        pEventNotification = inotifyGetNotificationList(pNotifier);
        if (pEventNotification != NULL)
        {
            // ping any events on the list of type notifyIndex
            osEventNotificationWithInfo(pGpu, pEventNotification, localNotifyType, localInfo32, info16,
                                        pNotifyParams, notifyParamsSize);
        }

        // reset if single shot notify action
        if (pSubdevice->notifyActions[localNotifyType] == NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE)
        {
            if (notifyIndex == NV2080_NOTIFIERS_FIFO_EVENT_MTHD)
            {
                NV_ASSERT(pGpu->activeFifoEventMthdNotifiers);
                pGpu->activeFifoEventMthdNotifiers--;
            }

            pSubdevice->notifyActions[localNotifyType] = NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE;
        }
    }
}

//
// For a particular gpu, find all the clients waiting for a particular event,
// fill in the notifier if allocated, and raise an event to the client if registered.
//
void
gpuGspPluginTriggeredEvent_IMPL
(
    OBJGPU *pGpu,
    NvU32   gfid,
    NvU32   notifyIndex
)
{
    PEVENTNOTIFICATION pEventNotification;
    RS_SHARE_ITERATOR it = serverutilShareIter(classId(NotifShare));

    NV_ASSERT_OR_RETURN_VOID(notifyIndex < NVA084_NOTIFIERS_MAXCOUNT);

    // search notifiers with events hooked up for this gpu
    while (serverutilShareIterNext(&it))
    {
        RsShared *pShared = it.pShared;
        KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi;
        INotifier *pNotifier;
        NotifShare *pNotifierShare = dynamicCast(pShared, NotifShare);
        KERNEL_HOST_VGPU_DEVICE *pKernelHostVgpuDevice;

        if ((pNotifierShare == NULL) || (pNotifierShare->pNotifier == NULL))
            continue;

        pNotifier = pNotifierShare->pNotifier;
        pKernelHostVgpuDeviceApi = dynamicCast(pNotifier, KernelHostVgpuDeviceApi);

        // Only notify matching GPUs
        if ((pKernelHostVgpuDeviceApi == NULL) || (GPU_RES_GET_GPU(pKernelHostVgpuDeviceApi) != pGpu))
            continue;
        GPU_RES_SET_THREAD_BC_STATE(pKernelHostVgpuDeviceApi);

        pKernelHostVgpuDevice = pKernelHostVgpuDeviceApi->pShared->pDevice;

        // Only notify matching GFID
        if (pKernelHostVgpuDevice == NULL || (gfid != pKernelHostVgpuDevice->gfid))
            continue;

        pEventNotification = inotifyGetNotificationList(pNotifier);
        if (pEventNotification != NULL)
        {
            // ping any events on the list of type notifyIndex
            osEventNotification(pGpu, pEventNotification, notifyIndex,
                                        NULL, 0);
        }
    }
}


//
// Searches the Pid Array to see if the process this client belongs to is already
// in the list.
//
static NvBool
_gpuiIsPidSavedAlready
(
    NvU32  pid,
    NvU32 *pPidArray,
    NvU32  pidCount
)
{
    NvU32 j;

    for (j = 0; j < pidCount; j++)
    {
        if (pid == pPidArray[j])
            return NV_TRUE;
    }
    return NV_FALSE;
}

static NV_STATUS
_gpuConvertPid
(
    RmClient *pClient,
    NvU32    *pNsPid
)
{
    if (pClient->pOsPidInfo != NULL)
        return osFindNsPid(pClient->pOsPidInfo, pNsPid);

    *pNsPid = pClient->ProcID;
    return NV_OK;
}

//
// Searches through clients to find processes with clients that have
// allocated an ElementType of class, defined by elementID. The return values
// are the array containing the PIDs for the processes and the count for the
// array.
// If a valid partitionRef is provided, the scope of search gets limited to a
// partition
//
NV_STATUS
gpuGetProcWithObject_IMPL
(
    OBJGPU *pGpu,
    NvU32 elementID,
    NvU32 internalClassId,
    NvU32 *pPidArray,
    NvU32 *pPidArrayCount,
    MIG_INSTANCE_REF *pRef
)
{
    NvU32         pidcount       = 0;
    NvHandle      hClient;
    Device        *pDevice;
    RmClient      **ppClient;
    RmClient      *pClient;
    RsClient      *pRsClient;
    RsResourceRef *pResourceRef;
    NV_STATUS     status;

    NV_ASSERT_OR_RETURN((pPidArray != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pPidArrayCount != NULL), NV_ERR_INVALID_ARGUMENT);

    for (ppClient = serverutilGetFirstClientUnderLock();
         ppClient;
         ppClient = serverutilGetNextClientUnderLock(ppClient))
    {
        NvBool elementInClient = NV_FALSE;
        RS_ITERATOR  iter;
        RS_PRIV_LEVEL privLevel = rmclientGetCachedPrivilege(*ppClient);

        pClient = *ppClient;
        pRsClient = staticCast(pClient, RsClient);
        hClient = pRsClient->hClient;

        // Skip reporting of kernel mode and internal RM clients
        if ((privLevel >= RS_PRIV_LEVEL_KERNEL) && rmclientIsAdmin(pClient, privLevel))
            continue;

        if (_gpuiIsPidSavedAlready(pClient->ProcID, pPidArray, pidcount))
            continue;

        if (deviceGetByGpu(pRsClient, pGpu, NV_TRUE /* bAnyInGroup */, &pDevice) != NV_OK)
            continue;

        {
            KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
            if ((pKernelMIGManager != NULL) && (pRef->pKernelMIGGpuInstance != NULL))
            {
                MIG_INSTANCE_REF clientRef = kmigmgrMakeNoMIGReference();
                RS_ITERATOR it = clientRefIter(pRsClient, NULL, classId(Device), RS_ITERATE_CHILDREN, NV_TRUE);
                NvBool bClientHasMatchingInstance = NV_FALSE;

                // Find matching MIG instance in devices under this Client
                while (clientRefIterNext(pRsClient, &it))
                {
                    pDevice = dynamicCast(it.pResourceRef->pResource, Device);

                    if ((GPU_RES_GET_GPU(pDevice) != pGpu) || 
                        (kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice,
                                                         &clientRef) != NV_OK))
                    {
                        continue;
                    }

                    // Check partition subscription against requested partition
                    if ((pRef->pKernelMIGGpuInstance != clientRef.pKernelMIGGpuInstance) ||
                        ((pRef->pMIGComputeInstance != NULL) &&
                         (pRef->pMIGComputeInstance != clientRef.pMIGComputeInstance)))
                    {
                        continue;
                    }

                    bClientHasMatchingInstance = NV_TRUE;
                    break;
                }

                if (!bClientHasMatchingInstance)
                    continue;
            }
        }

        iter = serverutilRefIter(hClient, NV01_NULL_OBJECT, 0, RS_ITERATE_DESCENDANTS, NV_TRUE);

        //
        // At this point it has been determined that the client's subdevice
        // is associated with the Gpu of interest, and it is not already
        // included in the pidArray. In the call, objects belonging to the
        // client are returned. If any object in the client belongs to
        // the class being queried, then that process is added to the array.
        //
        while (clientRefIterNext(iter.pClient, &iter))
        {
            pResourceRef = iter.pResourceRef;

            if (!objDynamicCastById(pResourceRef->pResource, internalClassId))
                continue;

            switch (internalClassId)
            {
                case (classId(ChannelDescendant)):
                {
                    ChannelDescendant *Object = dynamicCast(pResourceRef->pResource, ChannelDescendant);
                    if (RES_GET_EXT_CLASS_ID(Object) == elementID)
                    {
                        GpuResource *pGpuResource = (Object->pKernelChannel != NULL)
                            ? dynamicCast(Object->pKernelChannel, GpuResource) : NULL;

                        if ((pGpuResource != NULL) && (GPU_RES_GET_DEVICE(pGpuResource) == pDevice))
                        {
                            elementInClient = NV_TRUE;
                        }
                    }
                    break;
                }

                case (classId(KernelHostVgpuDeviceApi)):
                {
                    KernelHostVgpuDeviceApi *pKernelHostVgpuDeviceApi = dynamicCast(pResourceRef->pResource, KernelHostVgpuDeviceApi);
                    if (pKernelHostVgpuDeviceApi->pShared->pDevice->vgpuGuest)
                    {
                        elementInClient = NV_TRUE;
                    }
                    break;
                }
                case (classId(Device)):
                case (classId(Subdevice)):
                {
                    //
                    // It has been already verified that the client's subdevice
                    // or device is associated with the GPU of interest.
                    // Hence, Just add the client->pid into the list.
                    //
                    elementInClient = NV_TRUE;
                    break;
                }
                case (classId(MpsApi)):
                {
                    elementInClient = NV_TRUE;
                    break;
                }
                default:
                    return NV_ERR_INVALID_ARGUMENT;
            }
            if (elementInClient)
            {
                status = _gpuConvertPid(pClient, &pPidArray[pidcount]);
                if (status == NV_OK)
                {
                    pidcount++;
                }
                else if (status != NV_ERR_OBJECT_NOT_FOUND)
                {
                    return status;
                }

                if (pidcount == NV2080_CTRL_GPU_GET_PIDS_MAX_COUNT)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "Maximum PIDs reached. Returning.\n");

                    goto done;
                }

                break;
            }
        }
    }
done:
    *pPidArrayCount = pidcount;

    return NV_OK;
}

//
// _gpuCollectMemInfo
//
// Retrieves all the FB memory allocated for that client and returned as *pData.
// If the input parameter bIsGuestProcess is true, that means we are on VGX host
// and the caller is trying to find FB memory usage of a process which is
// running inside a VM.
//
static void
_gpuCollectMemInfo
(
    NvHandle                                          hClient,
    NvHandle                                          hDevice,
    Heap                                             *pTargetedHeap,
    NV2080_CTRL_GPU_PID_INFO_VIDEO_MEMORY_USAGE_DATA *pData,
    NvBool                                            bIsGuestProcess,
    NvBool                                            bGlobalInfo
)
{
    RS_ITERATOR      iter;
    Memory          *pMemory = NULL;
    RsResourceRef   *pResourceRef;

    NV_ASSERT_OR_RETURN_VOID(pData != NULL);

    iter = serverutilRefIter(hClient, NV01_NULL_OBJECT, 0, RS_ITERATE_DESCENDANTS, NV_TRUE);

    while (clientRefIterNext(iter.pClient, &iter))
    {
        pResourceRef = iter.pResourceRef;
        pMemory = dynamicCast(pResourceRef->pResource, Memory);

        if (!pMemory)
            continue;

        // In case we are trying to find memory allocated by a process running
        // on a VM - the case where isGuestProcess is true, only consider the
        // memory :
        // 1. which is allocated by the guest VM or by a process running in it.
        // 2. if the memory is not tagged with NVOS32_TYPE_UNUSED type.
        //    Windows KMD and Linux X driver makes dummy allocations which is
        //    done using NV01_MEMORY_LOCAL_USER class with rmAllocMemory()
        //    function.
        //    On VGX, while passing this allocation in RPC, we use the memory
        //    type NVOS32_TYPE_UNUSED. So while calculating the per process FB
        //    usage, only consider the allocation if memory type is not
        //    NVOS32_TYPE_UNUSED.
        if ((pResourceRef->externalClassId == NV01_MEMORY_LOCAL_USER ||
                pResourceRef->externalClassId == NV01_MEMORY_LIST_FBMEM ||
                pResourceRef->externalClassId == NV01_MEMORY_LIST_OBJECT ) &&
            (pMemory->categoryClassId == NV01_MEMORY_LOCAL_USER) &&
            (bGlobalInfo || (pMemory->pHeap == pTargetedHeap)) &&
            (RES_GET_HANDLE(pMemory->pDevice) == hDevice) &&
            (pMemory->pMemDesc != NULL) &&
            ((!bIsGuestProcess && (!memdescGetFlag(pMemory->pMemDesc, MEMDESC_FLAGS_LIST_MEMORY))) ||
             (bIsGuestProcess && (memdescGetFlag(pMemory->pMemDesc, MEMDESC_FLAGS_GUEST_ALLOCATED)) && (pMemory->Type != NVOS32_TYPE_UNUSED))))
        {
            NvBool bIsMemProtected = NV_FALSE;

            bIsMemProtected = gpuIsCCorApmFeatureEnabled(pMemory->pMemDesc->pGpu) &&
                              (pMemory->Flags & NVOS32_ALLOC_FLAGS_PROTECTED);
            if (bIsMemProtected)
            {
                NV_ASSERT(!memdescGetFlag(pMemory->pMemDesc,
                           MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY));
            }

            if (pMemory->pMemDesc->DupCount == 1)
            {
                pData->memPrivate += pMemory->Length;
                if (bIsMemProtected)
                {
                    pData->protectedMemPrivate += pMemory->Length;
                }
            }
            else if (pMemory->isMemDescOwner)
            {
                pData->memSharedOwned += pMemory->Length;
                if (bIsMemProtected)
                {
                    pData->protectedMemSharedOwned += pMemory->Length;
                }
            }
            else
            {
                pData->memSharedDuped += pMemory->Length;
                if (bIsMemProtected)
                {
                    pData->protectedMemSharedDuped += pMemory->Length;
                }
            }
        }
    }
}

static NvBool
_gpuMatchClientPid
(
    RmClient *pClient,
    NvU32 pid,
    NvU32 subPid
)
{
    NvU32 clientNsPid; // pClient PID on current's namespace

    if (_gpuConvertPid(pClient, &clientNsPid) != NV_OK)
        return NV_FALSE;

    return (((subPid == 0) && (clientNsPid == pid)) ||
            ((subPid != 0) && (clientNsPid == pid)  && (pClient->SubProcessID == subPid)));
}

//
// This function takes in the PID for the process of interest, and queries all
// clients for elementType. The 64-bit Data is updated by specific functions
// which handle queries for different elementTypes.
//
NV_STATUS
gpuFindClientInfoWithPidIterator_IMPL
(
    OBJGPU *pGpu,
    NvU32 pid,
    NvU32 subPid,
    NvU32 internalClassId,
    NV2080_CTRL_GPU_PID_INFO_DATA *pData,
    NV2080_CTRL_SMC_SUBSCRIPTION_INFO *pSmcInfo,
    MIG_INSTANCE_REF *pRef,
    NvBool bGlobalInfo
)
{
    NvHandle    hClient;
    Device     *pDevice;
    NvHandle    hDevice;
    RmClient  **ppClient;
    RmClient   *pClient;
    RsClient   *pRsClient;
    Heap       *pHeap               = GPU_GET_HEAP(pGpu);
    NvU32       computeInstanceId   = PARTITIONID_INVALID;
    NvU32       gpuInstanceId       = PARTITIONID_INVALID;

    NV_ASSERT_OR_RETURN(RMCFG_FEATURE_KERNEL_RM, NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN((pid != 0), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pData != NULL), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pSmcInfo != NULL), NV_ERR_INVALID_ARGUMENT);

    for (ppClient = serverutilGetFirstClientUnderLock();
         ppClient;
         ppClient = serverutilGetNextClientUnderLock(ppClient))
    {
        pClient = *ppClient;
        pRsClient = staticCast(pClient, RsClient);

        if (_gpuMatchClientPid(pClient, pid, subPid))
        {
            RS_PRIV_LEVEL privLevel = rmclientGetCachedPrivilege(pClient);
            RS_ITERATOR it;

            hClient = pRsClient->hClient;

            // Skip reporting of kernel mode and internal RM clients
            if ((privLevel >= RS_PRIV_LEVEL_KERNEL) && rmclientIsAdmin(pClient, privLevel))
                continue;

            it = clientRefIter(pRsClient, NULL, classId(Device), RS_ITERATE_CHILDREN, NV_TRUE);

            while (clientRefIterNext(pRsClient, &it))
            {
                pDevice = dynamicCast(it.pResourceRef->pResource, Device);

                if (GPU_RES_GET_GPU(pDevice) != pGpu)
                    continue;

                if (IS_MIG_IN_USE(pGpu))
                {
                    KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
                    MIG_INSTANCE_REF clientRef = kmigmgrMakeNoMIGReference();

                    if (kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager, pDevice,
                                                        &clientRef) != NV_OK)
                    {
                        continue;
                    }

                    // Check instance subscription against requested instance reference
                    if (!bGlobalInfo &&
                        !kmigmgrAreMIGReferencesSame(pRef, &clientRef))
                    {
                        continue;
                    }

                    gpuInstanceId = clientRef.pKernelMIGGpuInstance->swizzId;
                    if (clientRef.pMIGComputeInstance != NULL)
                    {
                        computeInstanceId = clientRef.pMIGComputeInstance->id;
                    }

                    if (kmigmgrIsMIGReferenceValid(pRef))
                    {
                        pHeap = pRef->pKernelMIGGpuInstance->pMemoryPartitionHeap;
                    }
                }

                hDevice = RES_GET_HANDLE(pDevice);

                switch (internalClassId)
                {
                    case (classId(Memory)):
                    {
                        // TODO -
                        // When single process spanning across multiple GI or CI by creating multiple
                        // clients, RM needs to provide the unique list being used by the client
                        _gpuCollectMemInfo(hClient, hDevice, pHeap,
                                           &pData->vidMemUsage, ((subPid != 0) ? NV_TRUE : NV_FALSE),
                                           bGlobalInfo);
                        break;
                    }
                    default:
                        return NV_ERR_INVALID_ARGUMENT;
                }
            }
        }
    }

    pSmcInfo->computeInstanceId = computeInstanceId;
    pSmcInfo->gpuInstanceId = gpuInstanceId;

    return NV_OK;
}
