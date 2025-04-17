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
#include "core/core.h"
#include "os/os.h"
#include "rmapi/event.h"
#include "rmapi/resource_fwd_decls.h"
#include "vgpu/rpc.h"
#include "gpu/device/device.h"
#include "core/locks.h"
#include "rmapi/rs_utils.h"

#include "virtualization/kernel_hostvgpudeviceapi.h"

#include "gpu/external_device/gsync_api.h"

#include "resserv/rs_client.h"
#include "class/cl0005.h"

#include "ctrl/ctrl0000/ctrl0000event.h" // NV0000_CTRL_EVENT_SET_NOTIFICATION_ACTION_*

static NV_STATUS _eventRpcForType(NvHandle hClient, NvHandle hObject);
static void eventSystemDequeueEventLatest(SystemEventQueueList *pQueue);

NV_STATUS
eventConstruct_IMPL
(
    Event *pEvent,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV0005_ALLOC_PARAMETERS *pNv0050AllocParams = pParams->pAllocParams;
    RsClient *pRsClient = pCallContext->pClient;
    RsResourceRef *pClientRef;
    RsResourceRef *pResourceRef = pCallContext->pResourceRef;
    NV_STATUS rmStatus = NV_OK;
    PEVENTNOTIFICATION *ppEventNotification;
    NvHandle hChannel = 0x0;
    OBJGPU *pGpu = NULL;
    RS_PRIV_LEVEL privLevel = pParams->pSecInfo->privLevel;
    NvBool bUserOsEventHandle = NV_FALSE;
    NvHandle hParentClient = pNv0050AllocParams->hParentClient;

    //
    // Allow hParentClient being zero to imply the allocating client should be
    // the parent client of this event.
    //
    if (hParentClient == NV01_NULL_OBJECT)
    {
        hParentClient = pRsClient->hClient;
    }

    // never allow user mode/non-root clients to create ring0 callbacks as
    // we can not trust the function pointer (encoded in data).
    if ((NV01_EVENT_KERNEL_CALLBACK == pResourceRef->externalClassId) ||
        (NV01_EVENT_KERNEL_CALLBACK_EX == pResourceRef->externalClassId))
    {
        if (privLevel < RS_PRIV_LEVEL_KERNEL)
        {
            // sometimes it is nice to hook up callbacks for debug purposes
            // -- but disable the override for release builds!
#if defined(DEBUG) || defined(DEVELOP)
            if (!(pNv0050AllocParams->notifyIndex & NV01_EVENT_PERMIT_NON_ROOT_EVENT_KERNEL_CALLBACK_CREATION))
#endif
            {
                return NV_ERR_ILLEGAL_ACTION;
            }
        }
    }

    if (_eventRpcForType(hParentClient, pNv0050AllocParams->hSrcResource))
    {
        RsResourceRef *pSrcRef;
        NV_STATUS tmpStatus;

        tmpStatus = serverutilGetResourceRef(hParentClient,
                                             pNv0050AllocParams->hSrcResource,
                                             &pSrcRef);

        if (tmpStatus == NV_OK)
        {
            hChannel = pSrcRef->pParentRef ? pSrcRef->pParentRef->hResource : 0;
            pGpu = CliGetGpuFromContext(pSrcRef, NULL);

            if (pGpu == NULL)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "RmAllocEvent could not set pGpu. hClient=0x%x, hObject=0x%x\n",
                          pRsClient->hClient, pResourceRef->hResource);
            }
        }
    }

    NV_ASSERT_OK_OR_RETURN(clientGetResourceRef(pRsClient, pRsClient->hClient, &pClientRef));

    // add event to client and parent object
    rmStatus = eventInit(pEvent,
                        pCallContext,
                        hParentClient,
                        pNv0050AllocParams->hSrcResource,
                        &ppEventNotification);
    if (rmStatus == NV_OK)
    {
        //
        // vGPU:
        //
        // Since vGPU does all real hardware management in the
        // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
        // do an RPC to the host to do the hardware update.
        //
        // In RM-offload, we don't allocate ContextDma in GSP-RM unless there
        // is any necessity to use it (e.g. display channel binding time). So
        // GSP-RM will find no valid object if the event is associated with
        // ContextDma object. So we are ignoring the event allocation here if
        // the event is associated with ContextDma object.
        //
        if (pGpu != NULL)
        {
            RsResourceRef *pSourceRef = NULL;

            if (IS_GSP_CLIENT(pGpu))
            {
                NV_ASSERT_OK_OR_RETURN(
                    serverutilGetResourceRef(hParentClient,
                                             pNv0050AllocParams->hSrcResource,
                                             &pSourceRef));
            }

            if (
                !(IS_GSP_CLIENT(pGpu) && (pSourceRef->internalClassId == classId(KernelHostVgpuDeviceApi))) &&
                (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
                (IS_GSP_CLIENT(pGpu) && pSourceRef->internalClassId != classId(ContextDma)) ||
                (IS_VIRTUAL_WITH_SRIOV(pGpu) && !(pNv0050AllocParams->notifyIndex & NV01_EVENT_NONSTALL_INTR))))
            {
                //
                // In SR-IOV enabled systems, nonstall events can be registered
                // directly with guest RM since guest RM is capable of
                // receiving and handling nonstall interrupts itself. In
                // paravirtualized systems, we always need to use the RPC to
                // host RM.
                //
                NV_RM_RPC_ALLOC_EVENT(pGpu,
                                      pRsClient->hClient,
                                      pEvent->hNotifierClient,
                                      hChannel,
                                      pEvent->hNotifierResource,
                                      pResourceRef->hResource,
                                      pResourceRef->externalClassId,
                                      pNv0050AllocParams->notifyIndex,
                                      rmStatus);
            }
        }

        if (NV01_EVENT_OS_EVENT == pResourceRef->externalClassId)
        {
            // convert a user event handle to its kernel equivalent.
            if (privLevel <= RS_PRIV_LEVEL_USER_ROOT)
            {
                rmStatus = osUserHandleToKernelPtr(pRsClient->hClient,
                                                   pNv0050AllocParams->data,
                                                   &pNv0050AllocParams->data);
                bUserOsEventHandle = NV_TRUE;
            }
        }

        if (rmStatus == NV_OK)
            rmStatus = registerEventNotification(ppEventNotification,
                                                 pRsClient,
                                                 pEvent->hNotifierResource,
                                                 pResourceRef->hResource,
                                                 pNv0050AllocParams->notifyIndex,
                                                 pResourceRef->externalClassId,
                                                 pNv0050AllocParams->data,
                                                 bUserOsEventHandle);
    }

    if (rmStatus != NV_OK)
        goto cleanup;

    return NV_OK;

cleanup:
    eventDestruct_IMPL(pEvent);
    return rmStatus;
}

void eventDestruct_IMPL
(
    Event *pEvent
)
{
    CALL_CONTEXT *pCallContext;
    RS_RES_FREE_PARAMS_INTERNAL *pParams;

    RsClient* pRsClient;
    NvHandle hEventClient;
    NV_STATUS status = NV_OK;
    NvHandle hEvent;
    NotifShare *pNotifierShare;

    resGetFreeParams(staticCast(pEvent, RsResource), &pCallContext, &pParams);
    pRsClient = pCallContext->pClient;
    hEventClient = pRsClient->hClient;
    hEvent = pCallContext->pResourceRef->hResource;

    LOCK_METER_DATA(FREE_EVENT, 0, 0, 0);

    pNotifierShare = pEvent->pNotifierShare;
    if (pNotifierShare != NULL)
    {
        if (pNotifierShare->pNotifier != NULL)
        {
            status = inotifyUnregisterEvent(pNotifierShare->pNotifier,
                    pNotifierShare->hNotifierClient,
                    pNotifierShare->hNotifierResource,
                    hEventClient,
                    hEvent);
        }
        serverFreeShare(&g_resServ, staticCast(pEvent->pNotifierShare, RsShared));
    }

    if (pParams != NULL)
        pParams->status = status;
}

NV_STATUS notifyUnregisterEvent_IMPL
(
    Notifier           *pNotifier,
    NvHandle            hNotifierClient,
    NvHandle            hNotifierResource,
    NvHandle            hEventClient,
    NvHandle            hEvent
)
{
    NV_STATUS status = NV_OK;
    PEVENTNOTIFICATION *ppEventNotification;

    ppEventNotification = inotifyGetNotificationListPtr(staticCast(pNotifier, INotifier));

    // delete the event from the parent object and client
    if (*ppEventNotification != NULL)
    {

        if (_eventRpcForType(hNotifierClient, hNotifierResource))
        {
            OBJGPU *pGpu = CliGetGpuFromHandle(hNotifierClient, hNotifierResource, NULL);

            if (pGpu != NULL)
            {
                RsResourceRef *pNotifierRef = NULL;

                if (IS_GSP_CLIENT(pGpu))
                {
                    NV_ASSERT_OK_OR_RETURN(serverutilGetResourceRef(hNotifierClient, hNotifierResource, &pNotifierRef));
                }

                //
                // vGPU:
                //
                // Since vGPU does all real hardware management in the
                // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
                // do an RPC to the host to do the hardware update.
                //
                if (
                    !(IS_GSP_CLIENT(pGpu) && (pNotifierRef->internalClassId == classId(KernelHostVgpuDeviceApi))) &&
                    (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
                    (IS_GSP_CLIENT(pGpu) && pNotifierRef->internalClassId != classId(ContextDma)) ||
                    (IS_VIRTUAL_WITH_SRIOV(pGpu) && !((*ppEventNotification)->bNonStallIntrEvent))))
                {
                    //
                    // In SR-IOV enabled systems, nonstall events are registered
                    // directly with guest RM since guest RM is capable of
                    // receiving and handling nonstall interrupts itself. We skip
                    // the allocation, so here, we skip the free too. In
                    // paravirtualized systems, we always need to use the RPC to
                    // host RM.
                    //
                    NV_RM_RPC_FREE(pGpu, hEventClient, hEventClient, hEvent, status);
                }
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR,
                          "RmFreeEvent could not set pGpu. hClient=0x%x, hObject=0x%x\n",
                          hNotifierClient, hNotifierResource);
            }
        }

        unregisterEventNotification(ppEventNotification,
                          hEventClient,
                          hNotifierResource,
                          hEvent);

    }

    // Gsync needs special event handling (ugh).
    //
    GSyncApi *pGSyncApi = dynamicCast(pNotifier, GSyncApi);
    if (pGSyncApi != NULL)
    {
        NvU32          eventNum;

        // check all events bound to this gsync object
        for (eventNum = 0; eventNum < NV30F1_CTRL_GSYNC_EVENT_TYPES; eventNum++)
        {
            if ((pGSyncApi->pEventByType[eventNum]) &&
                (pGSyncApi->pEventByType[eventNum]->hEventClient == hEventClient) &&
                (pGSyncApi->pEventByType[eventNum]->hEvent == hEvent))
            {
                unregisterEventNotification(&pGSyncApi->pEventByType[eventNum],
                        hEventClient,
                        hNotifierResource,
                        hEvent);
            }
        }
    }

    return status;
}

NV_STATUS
eventInit_IMPL
(
    Event *pEvent,
    CALL_CONTEXT *pCallContext,
    NvHandle hNotifierClient,
    NvHandle hNotifierResource,
    PEVENTNOTIFICATION **pppEventNotification
)
{
    NV_STATUS        rmStatus = NV_OK;
    RsClient        *pRsClient = pCallContext->pClient;
    RsClient        *pNotifierClient;
    RsResourceRef   *pResourceRef = pCallContext->pResourceRef;
    NotifShare      *pNotifierShare = NULL;

    // validate event class
    switch (pResourceRef->externalClassId)
    {
        case NV01_EVENT_KERNEL_CALLBACK:
        case NV01_EVENT_KERNEL_CALLBACK_EX:
        case NV01_EVENT_OS_EVENT:
            break;

        default:
            return NV_ERR_INVALID_CLASS;
    }

    // RS-TODO remove support for this after per-client locking is enabled
    if (pRsClient->hClient != hNotifierClient)
    {
        rmStatus = serverGetClientUnderLock(&g_resServ, hNotifierClient, &pNotifierClient);
        if (rmStatus != NV_OK)
            return rmStatus;
    }
    else
    {
        pNotifierClient = pRsClient;
    }

    if (pNotifierClient != NULL)
    {
        RsResourceRef *pNotifierRef;
        INotifier *pNotifier;
        if (clientGetResourceRef(pNotifierClient, hNotifierResource, &pNotifierRef) != NV_OK)
            return NV_ERR_INVALID_OBJECT;

        pNotifier = dynamicCast(pNotifierRef->pResource, INotifier);
        if (pNotifier == NULL)
            return NV_ERR_INVALID_OBJECT;

        rmStatus = inotifyGetOrAllocNotifShare(pNotifier, hNotifierClient, hNotifierResource, &pNotifierShare);
        if (rmStatus != NV_OK)
            return rmStatus;

        *pppEventNotification = inotifyGetNotificationListPtr(pNotifierShare->pNotifier);
    }

    serverRefShare(&g_resServ, staticCast(pNotifierShare, RsShared));
    pEvent->pNotifierShare = pNotifierShare;

    // RS-TODO these can be looked up from share
    pEvent->hNotifierClient    = hNotifierClient;
    pEvent->hNotifierResource  = hNotifierResource;
    pEvent->hEvent = pCallContext->pResourceRef->hResource;

    return rmStatus;
}

NV_STATUS
notifyGetOrAllocNotifShare_IMPL
(
    Notifier    *pNotifier,
    NvHandle     hNotifierClient,
    NvHandle     hNotifierResource,
    NotifShare **ppNotifierShare
)
{
    NV_STATUS status;
    NotifShare *pNotifierShare;

    //
    // Most objects that are notifiers will never have any events to notify so
    // notifier shares are allocated as needed (i.e., when an event
    // registers itself with the notifier.)
    //
    pNotifierShare = inotifyGetNotificationShare(staticCast(pNotifier, INotifier));
    if (pNotifierShare == NULL)
    {
        RsShared *pShare;
        status = serverAllocShare(&g_resServ, classInfo(NotifShare), &pShare);
        if (status != NV_OK)
            return status;

        pNotifierShare = dynamicCast(pShare, NotifShare);
        pNotifierShare->pNotifier = staticCast(pNotifier, INotifier);
        pNotifierShare->hNotifierClient = hNotifierClient;
        pNotifierShare->hNotifierResource = hNotifierResource;
        inotifySetNotificationShare(staticCast(pNotifier, INotifier), pNotifierShare);
    }

    if (ppNotifierShare)
        *ppNotifierShare = pNotifierShare;

    return NV_OK;
}

NV_STATUS
CliGetEventNotificationList
(
    NvHandle             hClient,
    NvHandle             hObject,
    INotifier          **ppNotifier,
    PEVENTNOTIFICATION **pppEventNotification
)
{
    NV_STATUS                     status = NV_OK;
    RsResourceRef                *pResourceRef;
    RsClient                     *pRsClient;
    INotifier                    *pNotifier;

    *pppEventNotification = NULL;

    // Populate Resource Server information
    status = serverGetClientUnderLock(&g_resServ, hClient, &pRsClient);
    if (status != NV_OK)
        return status;

    status = clientGetResourceRef(pRsClient, hObject, &pResourceRef);
    if (status != NV_OK)
        return status;

    pNotifier = dynamicCast(pResourceRef->pResource, INotifier);
    if (pNotifier != NULL)
        *pppEventNotification = inotifyGetNotificationListPtr(pNotifier);

    if (*pppEventNotification == NULL)
        return NV_ERR_INVALID_OBJECT;

    if (ppNotifier != NULL)
        *ppNotifier = pNotifier;

    return NV_OK;
}

NvBool
CliGetEventInfo
(
    NvHandle         hClient,
    NvHandle         hEvent,
    Event          **ppEvent
)
{
    RsClient       *pRsClient;
    RsResourceRef  *pResourceRef;
    RmClient       *pClient = serverutilGetClientUnderLock(hClient);

    if (pClient == NULL)
        return NV_FALSE;

    pRsClient = staticCast(pClient, RsClient);
    if (clientGetResourceRefByType(pRsClient, hEvent, classId(Event), &pResourceRef) != NV_OK)
        return NV_FALSE;

    if (pResourceRef->pResource != NULL)
    {
        *ppEvent = dynamicCast(pResourceRef->pResource, Event);
        return NV_TRUE;
    }

    return NV_FALSE;

}

void
CliDelObjectEvents
(
    RsResourceRef *pResourceRef
)
{
    NotifShare    *pNotifierShare;
    INotifier     *pNotifier;

    if (pResourceRef == NULL)
        return;

    // If not a notifier object, there aren't any events to free
    pNotifier = dynamicCast(pResourceRef->pResource, INotifier);

    if (pNotifier == NULL)
        return;

    pNotifierShare = inotifyGetNotificationShare(pNotifier);
    if (pNotifierShare != NULL)
    {
        while(pNotifierShare->pEventList != NULL)
        {
            PEVENTNOTIFICATION pEventNotif = pNotifierShare->pEventList;
            inotifyUnregisterEvent(pNotifier,
                    pNotifierShare->hNotifierClient,
                    pNotifierShare->hNotifierResource,
                    pEventNotif->hEventClient,
                    pEventNotif->hEvent);
        }
        pNotifierShare->pNotifier = NULL;
    }
} // end of CliDelObjectEvents()

// ****************************************************************************
//                                  System events
// ****************************************************************************

void CliAddSystemEvent(
    NvU32 event,
    void *pEventData,
    NvBool *isEventNotified
)
{
    PEVENTNOTIFICATION pEventNotification = NULL;
    RmClient **ppClient;
    RmClient  *pClient;
    RsClient  *pRsClient;
    RsResourceRef *pCliResRef;
    NV_STATUS      rmStatus = NV_OK;
    Notifier  *pNotifier;

    if (isEventNotified != NULL)
        *isEventNotified = NV_FALSE;

    for (ppClient = serverutilGetFirstClientUnderLock();
         ppClient;
         ppClient = serverutilGetNextClientUnderLock(ppClient))
    {
        pClient = *ppClient;
        pRsClient = staticCast(pClient, RsClient);

        if (pClient->CliSysEventInfo.notifyActions[event] == NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE)
        {
            continue;
        }

        rmStatus = clientGetResourceRef(staticCast(pClient, RsClient), pRsClient->hClient, &pCliResRef);
        if (rmStatus != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Failed to look up resource reference handle: 0x%x\n",
                      pRsClient->hClient);
            return;
        }

        pNotifier = dynamicCast(pCliResRef->pResource, Notifier);
        if (pNotifier != NULL)
            pEventNotification = inotifyGetNotificationList(staticCast(pNotifier, INotifier));

        if (pEventNotification != NULL)
        {
            while (pEventNotification)
            {
                if (pEventNotification->NotifyIndex == event)
                {
                    // only log the system event that has data
                    if (pEventData != NULL)
                    {
                        if (eventSystemEnqueueEvent(&pClient->CliSysEventInfo.eventQueue,
                                                    event, pEventData) != NV_OK)
                        {
                            NV_PRINTF(LEVEL_ERROR, "fails to add event=%d\n", event);
                            return;
                        }
                    }

                    if (osNotifyEvent(NULL, pEventNotification, 0, 0, 0) != NV_OK)
                    {
                        if (pEventData != NULL)
                            eventSystemDequeueEventLatest(&pClient->CliSysEventInfo.eventQueue);

                        NV_PRINTF(LEVEL_ERROR, "failed to deliver event 0x%x",
                                  event);
                    }
                    else
                    {
                        if (isEventNotified != NULL)
                            *isEventNotified = NV_TRUE;
                    }
                }
                pEventNotification = pEventNotification->Next;
            }

            if (pClient->CliSysEventInfo.notifyActions[event] == NV0000_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE)
            {
                pClient->CliSysEventInfo.notifyActions[event] = NV0000_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE;
            }
        }
    }

    return;
}

static NV_STATUS
_eventRpcForType(NvHandle hClient, NvHandle hObject)
{
    NV_STATUS status;
    RsResourceRef *pResourceRef;

    status = serverutilGetResourceRef(hClient, hObject, &pResourceRef);

    if (status != NV_OK)
    {
        return NV_FALSE;
    }

    if (objDynamicCastById(pResourceRef->pResource, classId(Subdevice)) ||
        objDynamicCastById(pResourceRef->pResource, classId(KernelHostVgpuDeviceApi)) ||
        objDynamicCastById(pResourceRef->pResource, classId(ChannelDescendant)) ||
        objDynamicCastById(pResourceRef->pResource, classId(ContextDma)) ||
        objDynamicCastById(pResourceRef->pResource, classId(DispChannel)) ||
        objDynamicCastById(pResourceRef->pResource, classId(DispCommon)) ||        
        objDynamicCastById(pResourceRef->pResource, classId(TimerApi)) ||
        objDynamicCastById(pResourceRef->pResource, classId(KernelSMDebuggerSession)))
    {
        return NV_TRUE;
    }

    return NV_FALSE;
}

NV_STATUS
eventGetByHandle_IMPL
(
    RsClient *pClient,
    NvHandle hEvent,
    NvU32 *pNotifyIndex
)
{
    RsResourceRef  *pEventResourceRef;
    NV_STATUS status;
    Event *pEvent;
    NotifShare *pNotifierShare;
    PEVENTNOTIFICATION  pEventNotification;

    *pNotifyIndex = NV2080_NOTIFIERS_MAXCOUNT;

    status = clientGetResourceRef(pClient, hEvent, &pEventResourceRef);
    if (status != NV_OK)
        return status;

    pEvent = dynamicCast(pEventResourceRef->pResource, Event);
    if (pEvent == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Event is null \n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    // TODO: Check existing notifiers in that event
    pNotifierShare = pEvent->pNotifierShare;
    if ((pNotifierShare == NULL) || (pNotifierShare->pNotifier == NULL))
    {
        NV_PRINTF(LEVEL_ERROR, "pNotifierShare or pNotifier is NULL \n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    pEventNotification = inotifyGetNotificationList(pNotifierShare->pNotifier);
    if (pEventNotification == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "pEventNotification is NULL \n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    *pNotifyIndex = pEventNotification->NotifyIndex;

    return status;
}

void eventSystemInitEventQueue(SystemEventQueueList *pQueue)
{
    listInit(pQueue, portMemAllocatorGetGlobalNonPaged());
}

NV_STATUS eventSystemEnqueueEvent(SystemEventQueueList *pQueue, NvU32 event, void *pEventData)
{
    NV0000_CTRL_GET_SYSTEM_EVENT_DATA_PARAMS newNode = { 0 };

    newNode.event = event;
    switch (event)
    {
        case NV0000_NOTIFIERS_DISPLAY_CHANGE:
            newNode.data.display = *((NV0000_CTRL_SYSTEM_EVENT_DATA_DISPLAY_CHANGE *)pEventData);
            break;

        case NV0000_NOTIFIERS_VGPU_UNBIND_EVENT:
            newNode.data.vgpuUnbind = *((NV0000_CTRL_SYSTEM_EVENT_DATA_VGPU_UNBIND *)pEventData);
            break;

        case NV0000_NOTIFIERS_VGPU_BIND_EVENT:
            newNode.data.vgpuBind = *((NV0000_CTRL_SYSTEM_EVENT_DATA_VGPU_BIND *)pEventData);
            break;

        case NV0000_NOTIFIERS_GPU_BIND_UNBIND_EVENT:
            newNode.data.gpuBindUnbind = *((NV0000_CTRL_SYSTEM_EVENT_DATA_GPU_BIND_UNBIND *)pEventData);
            break;

        default:
            return NV_ERR_INVALID_EVENT;
    }

    if (listAppendValue(pQueue, &newNode) == NULL)
        return NV_ERR_NO_MEMORY;

    return NV_OK;
}

static void eventSystemDequeueEventLatest(SystemEventQueueList *pQueue)
{
    NV0000_CTRL_GET_SYSTEM_EVENT_DATA_PARAMS *pLastNode = listTail(pQueue);

    if (pLastNode == NULL)
        return;

    listRemove(pQueue, pLastNode);
}

NV_STATUS eventSystemDequeueEvent(SystemEventQueueList *pQueue, NV0000_CTRL_GET_SYSTEM_EVENT_DATA_PARAMS *pEvent)
{
    NV0000_CTRL_GET_SYSTEM_EVENT_DATA_PARAMS *pFirstNode = listHead(pQueue);

    if (pFirstNode == NULL)
    {
        // Queue is empty
        return NV_ERR_OBJECT_NOT_FOUND;
    }

    *pEvent = *pFirstNode;

    listRemove(pQueue, pFirstNode);

    return NV_OK;
}

void eventSystemClearEventQueue(SystemEventQueueList *pQueue)
{
    listClear(pQueue);
}
