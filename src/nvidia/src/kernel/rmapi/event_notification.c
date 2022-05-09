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

/********************************* DMA Manager *****************************\
*                                                                           *
*   Event notifications are handled in this module.  DMA report and OS      *
*   action are dealt with on a per-object basis.                            *
*                                                                           *
****************************************************************************/

#include "core/core.h"
#include "gpu/gpu.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "class/cl0000.h"
#include "os/os.h"
#include "class/cl0005.h"
#include "gpu/subdevice/subdevice.h"
#include "rmapi/rs_utils.h"
#include "mem_mgr/mem.h"

#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"

static NV_STATUS _insertEventNotification
(
    PEVENTNOTIFICATION *ppEventNotification,
    NvHandle            hEventClient,
    NvHandle            hEvent,
    NvU32               NotifyIndex,
    NvU32               NotifyType,
    NvP64               Data,
    NvBool              bUserOsEventHandle
);

static NV_STATUS _removeEventNotification
(
    PEVENTNOTIFICATION *ppEventNotification,
    NvHandle            hEventClient,
    NvHandle            hEvent,
    NvBool              bMatchData,
    NvP64               Data,
    PEVENTNOTIFICATION *ppOldEvent
);

//---------------------------------------------------------------------------
//
//  Event support.
//
//---------------------------------------------------------------------------

static NV_STATUS engineNonStallEventOp
(
    OBJGPU *pGpu,
    NvU32 engineId,
    PEVENTNOTIFICATION pEventNotify,
    Memory *pMemory,
    NvBool bInsert
)
{
    ENGINE_EVENT_NODE *pTempNode;
    NvBool bFound = NV_FALSE;

    if (bInsert)
    {
        pTempNode = portMemAllocNonPaged(sizeof(ENGINE_EVENT_NODE));

        if (pTempNode == NULL)
            return NV_ERR_NO_MEMORY;

        // Acquire engine list spinlock before adding to engine event list
        portSyncSpinlockAcquire(pGpu->engineNonstallIntr[engineId].pSpinlock);
        pTempNode->pNext = pGpu->engineNonstallIntr[engineId].pEventNode;
        pTempNode->pEventNotify = pEventNotify;
        pTempNode->pMemory = pMemory;

        pGpu->engineNonstallIntr[engineId].pEventNode = pTempNode;

        // Release engine list spinlock
        portSyncSpinlockRelease(pGpu->engineNonstallIntr[engineId].pSpinlock);
    }
    else
    {
        ENGINE_EVENT_NODE *pEngNode, *pPrevNode = NULL;

        // Acquire engine list spinlock before traversing engine event list
        portSyncSpinlockAcquire(pGpu->engineNonstallIntr[engineId].pSpinlock);

        pEngNode = pGpu->engineNonstallIntr[engineId].pEventNode;
        while (pEngNode)
        {
            if (pEngNode->pEventNotify == pEventNotify)
            {
                if (pPrevNode == NULL)
                    pGpu->engineNonstallIntr[engineId].pEventNode = pEngNode->pNext;
                else
                    pPrevNode->pNext = pEngNode->pNext;

                pTempNode = pEngNode;
                bFound = NV_TRUE;
                break;
            }
            else
            {
                pPrevNode = pEngNode;
            }
            pEngNode = pEngNode->pNext;
        }

        // Release engine list spinlock
        portSyncSpinlockRelease(pGpu->engineNonstallIntr[engineId].pSpinlock);

        if (bFound)
        {
            portMemFree(pTempNode);
        }
        else
        {
            NV_ASSERT_FAILED("failed to find non-stall event!");
            return NV_ERR_INVALID_STATE;
        }
    }

    return NV_OK;
}

static NV_STATUS _engineNonStallIntrNotifyImpl(OBJGPU *pGpu, NvU32 engineId, NvHandle hEvent)
{
    ENGINE_EVENT_NODE *pTempHead;
    Memory *pSemMemory;
    NvU32 semValue;
    NvU32 *pTempKernelMapping = NULL;
    NV_STATUS rmStatus = NV_OK;

    //
    // Acquire engine list spinlock before traversing the list. Note that this
    // is called without holding locks from ISR for Linux. This spinlock is used
    // to protect per GPU per engine event node list.
    //
    portSyncSpinlockAcquire(pGpu->engineNonstallIntr[engineId].pSpinlock);

    pTempHead = pGpu->engineNonstallIntr[engineId].pEventNode;
    while (pTempHead)
    {
        if (!pTempHead->pEventNotify)
        {
            rmStatus = NV_ERR_INVALID_STATE;
            break;
        }

        if (hEvent && pTempHead->pEventNotify->hEvent != hEvent)
            goto nextEvent;

        pSemMemory = pTempHead->pMemory;

        if (pSemMemory && pSemMemory->vgpuNsIntr.isSemaMemValidationEnabled &&
            pSemMemory->pMemDesc && pSemMemory->pMemDesc->Allocated)
        {
            pTempKernelMapping = (NvU32 *)NvP64_VALUE(memdescGetKernelMapping(pSemMemory->pMemDesc));
            if (pTempKernelMapping == NULL)
            {
                NV_PRINTF(LEVEL_WARNING, "Per-vGPU semaphore location mapping is NULL. Skipping the current node.\n");
                pTempHead = pTempHead->pNext;
                continue;
            }
            semValue = MEM_RD32(pTempKernelMapping + (pSemMemory->vgpuNsIntr.nsSemOffset / sizeof(NvU32)));

            if (pSemMemory->vgpuNsIntr.nsSemValue == semValue)
            {
                pTempHead = pTempHead->pNext;
                continue;
            }

            pSemMemory->vgpuNsIntr.nsSemValue = semValue;

        }

        if (osNotifyEvent(pGpu, pTempHead->pEventNotify, 0, 0, NV_OK) != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to notify event for engine 0x%x\n",
                      engineId);
            NV_ASSERT(0);
            rmStatus = NV_ERR_INVALID_STATE;
            break;
        }

    nextEvent:
        pTempHead = pTempHead->pNext;
    }

    portSyncSpinlockRelease(pGpu->engineNonstallIntr[engineId].pSpinlock);
    return rmStatus;
}

NV_STATUS
engineNonStallIntrNotify(OBJGPU *pGpu, NvU32 engineId)
{
    return _engineNonStallIntrNotifyImpl(pGpu, engineId, 0);
}

NV_STATUS
engineNonStallIntrNotifyEvent(OBJGPU *pGpu, NvU32 engineId, NvHandle hEvent)
{
    return _engineNonStallIntrNotifyImpl(pGpu, engineId, hEvent);
}

static NV_STATUS
eventGetEngineTypeFromSubNotifyIndex
(
    NvU32 notifyIndex,
    NvU32 *engineIdx
)
{
    NV_ASSERT_OR_RETURN(engineIdx, NV_ERR_INVALID_ARGUMENT);

    *engineIdx = NV2080_ENGINE_TYPE_NULL;

    switch (notifyIndex)
    {
        case NV2080_NOTIFIERS_FIFO_EVENT_MTHD:
            *engineIdx = NV2080_ENGINE_TYPE_HOST;
            break;
        case NV2080_NOTIFIERS_CE0:
            *engineIdx = NV2080_ENGINE_TYPE_COPY0;
            break;
        case NV2080_NOTIFIERS_CE1:
            *engineIdx = NV2080_ENGINE_TYPE_COPY1;
            break;
        case NV2080_NOTIFIERS_CE2:
            *engineIdx = NV2080_ENGINE_TYPE_COPY2;
            break;
        case NV2080_NOTIFIERS_CE3:
            *engineIdx = NV2080_ENGINE_TYPE_COPY3;
            break;
        case NV2080_NOTIFIERS_CE4:
            *engineIdx = NV2080_ENGINE_TYPE_COPY4;
            break;
        case NV2080_NOTIFIERS_CE5:
            *engineIdx = NV2080_ENGINE_TYPE_COPY5;
            break;
        case NV2080_NOTIFIERS_CE6:
            *engineIdx = NV2080_ENGINE_TYPE_COPY6;
            break;
        case NV2080_NOTIFIERS_CE7:
            *engineIdx = NV2080_ENGINE_TYPE_COPY7;
            break;
        case NV2080_NOTIFIERS_CE8:
            *engineIdx = NV2080_ENGINE_TYPE_COPY8;
            break;
        case NV2080_NOTIFIERS_CE9:
            *engineIdx = NV2080_ENGINE_TYPE_COPY9;
            break;
        case NV2080_NOTIFIERS_GR0:
            *engineIdx = NV2080_ENGINE_TYPE_GR0;
            break;
        case NV2080_NOTIFIERS_GR1:
            *engineIdx = NV2080_ENGINE_TYPE_GR1;
            break;
        case NV2080_NOTIFIERS_GR2:
            *engineIdx = NV2080_ENGINE_TYPE_GR2;
            break;
        case NV2080_NOTIFIERS_GR3:
            *engineIdx = NV2080_ENGINE_TYPE_GR3;
            break;
        case NV2080_NOTIFIERS_GR4:
            *engineIdx = NV2080_ENGINE_TYPE_GR4;
            break;
        case NV2080_NOTIFIERS_GR5:
            *engineIdx = NV2080_ENGINE_TYPE_GR5;
            break;
        case NV2080_NOTIFIERS_GR6:
            *engineIdx = NV2080_ENGINE_TYPE_GR6;
            break;
        case NV2080_NOTIFIERS_GR7:
            *engineIdx = NV2080_ENGINE_TYPE_GR7;
            break;
        case NV2080_NOTIFIERS_PPP:
            *engineIdx = NV2080_ENGINE_TYPE_PPP;
            break;
        case NV2080_NOTIFIERS_NVDEC0:
            *engineIdx = NV2080_ENGINE_TYPE_NVDEC0;
            break;
        case NV2080_NOTIFIERS_NVDEC1:
            *engineIdx = NV2080_ENGINE_TYPE_NVDEC1;
            break;
        case NV2080_NOTIFIERS_NVDEC2:
            *engineIdx = NV2080_ENGINE_TYPE_NVDEC2;
            break;
        case NV2080_NOTIFIERS_NVDEC3:
            *engineIdx = NV2080_ENGINE_TYPE_NVDEC3;
            break;
        case NV2080_NOTIFIERS_NVDEC4:
            *engineIdx = NV2080_ENGINE_TYPE_NVDEC4;
            break;
        case NV2080_NOTIFIERS_PDEC:
            *engineIdx = NV2080_ENGINE_TYPE_VP;
            break;
        case NV2080_NOTIFIERS_MSENC:
            NV_ASSERT(NV2080_NOTIFIERS_MSENC   == NV2080_NOTIFIERS_NVENC0);
            NV_ASSERT(NV2080_ENGINE_TYPE_MSENC == NV2080_ENGINE_TYPE_NVENC0);
            *engineIdx = NV2080_ENGINE_TYPE_MSENC;
            break;
        case NV2080_NOTIFIERS_NVENC1:
            *engineIdx = NV2080_ENGINE_TYPE_NVENC1;
            break;
        case NV2080_NOTIFIERS_NVENC2:
            *engineIdx = NV2080_ENGINE_TYPE_NVENC2;
            break;
        case NV2080_NOTIFIERS_SEC2:
            *engineIdx = NV2080_ENGINE_TYPE_SEC2;
            break;
        case NV2080_NOTIFIERS_NVJPEG0:
            *engineIdx = NV2080_ENGINE_TYPE_NVJPEG0;
            break;
        case NV2080_NOTIFIERS_OFA:
            *engineIdx = NV2080_ENGINE_TYPE_OFA;
            break;
        default:
            NV_PRINTF(LEVEL_WARNING,
                      "engine 0x%x doesn't use the fast non-stall interrupt path!\n",
                      notifyIndex);
            NV_ASSERT(0);
            return NV_ERR_NOT_SUPPORTED;
    }

    return NV_OK;
}

NV_STATUS registerEventNotification
(
    PEVENTNOTIFICATION *ppEventNotification,
    NvHandle            hEventClient,
    NvHandle            hNotifier,
    NvHandle            hEvent,
    NvU32               NotifyIndex,
    NvU32               NotifyType,
    NvP64               Data,
    NvBool              bUserOsEventHandle
)
{
    Subdevice *pSubDevice;
    PEVENTNOTIFICATION pTargetEvent = NULL;
    NV_STATUS rmStatus = NV_OK, rmTmpStatus = NV_OK;
    OBJGPU *pGpu;
    NvBool bNonStallIntrEvent = NV_FALSE;
    NvU32 engineId;
    NvHandle hDevice;
    RsResourceRef *pResourceRef;
    Memory *pSemMemory = NULL;

    rmStatus = _insertEventNotification(ppEventNotification, hEventClient,
                                hEvent, NotifyIndex, NotifyType, Data, bUserOsEventHandle);

    if (rmStatus != NV_OK)
        goto failed_insert;

    bNonStallIntrEvent = ((NotifyIndex & NV01_EVENT_NONSTALL_INTR) ?  NV_TRUE : NV_FALSE);

    if (bNonStallIntrEvent)
    {
        //
        // For non-stall interrupt, the event parent type is NV20_SUBDEVICE, so we can locate
        // the correct OBJGPU and attach to its per-engine non-stall event list.
        //
        if ((serverutilGetResourceRef(hEventClient, hNotifier, &pResourceRef) != NV_OK) ||
            (!dynamicCast(pResourceRef->pResource, Subdevice)))
        {
            rmStatus = NV_ERR_INVALID_ARGUMENT;
            goto free_entry;
        }

        pSubDevice = dynamicCast(pResourceRef->pResource, Subdevice);
        hDevice = RES_GET_PARENT_HANDLE(pSubDevice);

        if (CliSetSubDeviceContext(hEventClient, RES_GET_HANDLE(pSubDevice), &hDevice, &pGpu) != NV_OK)
        {
            rmStatus = NV_ERR_INVALID_ARGUMENT;
            goto free_entry;
        }

        rmStatus = eventGetEngineTypeFromSubNotifyIndex(
                        DRF_VAL(0005, _NOTIFY_INDEX, _INDEX, NotifyIndex), &engineId);

        if (rmStatus != NV_OK)
            goto free_entry;

        if (IS_MIG_IN_USE(pGpu))
        {
            KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
            NvU32 globalEngineId = engineId;
            MIG_INSTANCE_REF ref;

            NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
                kmigmgrGetInstanceRefFromClient(pGpu, pKernelMIGManager, hEventClient, &ref),
                free_entry);

            NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
                kmigmgrGetLocalToGlobalEngineType(pGpu, pKernelMIGManager, ref, engineId, &globalEngineId),
                free_entry);

            engineId = globalEngineId;
        }

        if (pSubDevice->hSemMemory != NV01_NULL_OBJECT)
        {
            NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_SILENT,
                memGetByHandle(RES_GET_CLIENT(pSubDevice),
                               pSubDevice->hSemMemory,
                               &pSemMemory),
                free_entry);
        }

        rmStatus = engineNonStallEventOp(pGpu, engineId,
                        *ppEventNotification, pSemMemory, NV_TRUE);

        if (rmStatus != NV_OK)
            goto free_entry;

        return rmStatus;
    }

free_entry:
    if (rmStatus != NV_OK)
    {
        rmTmpStatus = _removeEventNotification(ppEventNotification, hEventClient,
                hEvent, NV_TRUE, Data, &pTargetEvent);

        if (rmTmpStatus == NV_OK)
            portMemFree(pTargetEvent);
    }

failed_insert:
    NV_ASSERT(rmStatus == NV_OK);
    return rmStatus;
}

static NV_STATUS _insertEventNotification
(
    PEVENTNOTIFICATION *ppEventNotification,
    NvHandle            hEventClient,
    NvHandle            hEvent,
    NvU32               NotifyIndex,
    NvU32               NotifyType,
    NvP64               Data,
    NvBool              bUserOsEventHandle

)
{
    PEVENTNOTIFICATION EventNotify;

    //
    // Create the event notification object
    //
    EventNotify = portMemAllocNonPaged(sizeof(EVENTNOTIFICATION));
    if (EventNotify == NULL)
        return NV_ERR_NO_MEMORY;

    //
    // Fill in the fields
    //
    if (NotifyIndex & NV01_EVENT_BROADCAST)
    {
        EventNotify->bBroadcastEvent = NV_TRUE;
    }
    else
    {
        EventNotify->bBroadcastEvent = NV_FALSE;
    }

    if (NotifyIndex & NV01_EVENT_SUBDEVICE_SPECIFIC)
    {
        EventNotify->bSubdeviceSpecificEvent = NV_TRUE;
        EventNotify->SubdeviceSpecificValue =
            DRF_VAL(0005, _NOTIFY_INDEX, _SUBDEVICE, NotifyIndex);
    }
    else
    {
        EventNotify->bSubdeviceSpecificEvent = NV_FALSE;
        EventNotify->SubdeviceSpecificValue = 0;
    }

    if (NotifyIndex & NV01_EVENT_WITHOUT_EVENT_DATA)
    {
        EventNotify->bEventDataRequired = NV_FALSE;
    }
    else
    {
        EventNotify->bEventDataRequired = NV_TRUE;
    }

    if (NotifyIndex & NV01_EVENT_CLIENT_RM)
    {
        EventNotify->bClientRM = NV_TRUE;
    }
    else
    {
        EventNotify->bClientRM = NV_FALSE;
    }

    EventNotify->bNonStallIntrEvent =
        ((NotifyIndex & NV01_EVENT_NONSTALL_INTR) ?  NV_TRUE : NV_FALSE);

    // strip the upper bits as they are actually flags
    NotifyIndex = DRF_VAL(0005, _NOTIFY_INDEX, _INDEX, NotifyIndex);

    EventNotify->hEventClient       = hEventClient;
    EventNotify->hEvent             = hEvent;
    EventNotify->subdeviceInst      = 0;
    EventNotify->NotifyIndex        = NotifyIndex;
    EventNotify->NotifyType         = NotifyType;
    EventNotify->Data               = Data;
    EventNotify->NotifyTriggerCount = 0;
    EventNotify->bUserOsEventHandle = bUserOsEventHandle;

    //
    // Now insert the event into the event chain of this object.
    // Order doesn't really matter.
    //
    EventNotify->Next = *ppEventNotification;
    *ppEventNotification = EventNotify;

    return (NV_OK);
}

//---------------------------------------------------------------------------
//
//  Event Notification support.
//
//---------------------------------------------------------------------------

NV_STATUS unregisterEventNotification
(
    PEVENTNOTIFICATION *ppEventNotification,
    NvHandle            hEventClient,
    NvHandle            hNotifier,
    NvHandle            hEvent
)
{
    return unregisterEventNotificationWithData(ppEventNotification,
                                               hEventClient,
                                               hNotifier,
                                               hEvent,
                                               NV_FALSE,
                                               NvP64_NULL);
}

NV_STATUS unregisterEventNotificationWithData
(
    PEVENTNOTIFICATION *ppEventNotification,
    NvHandle            hEventClient,
    NvHandle            hNotifier,
    NvHandle            hEvent,
    NvBool              bMatchData,
    NvP64               Data
)
{
    NV_STATUS               rmStatus        = NV_OK;
    PEVENTNOTIFICATION      pTargetEvent    = NULL;
    Subdevice              *pSubDevice;
    RsResourceRef          *pResourceRef;
    NvHandle                hDevice;
    NvU32                   engineId;
    OBJGPU                 *pGpu;

    rmStatus = _removeEventNotification(ppEventNotification, hEventClient,
                    hEvent, bMatchData, Data, &pTargetEvent);

    if (rmStatus != NV_OK)
        goto error;

    if (pTargetEvent->bNonStallIntrEvent)
    {
        //
        // For non-stall interrupt, the event parent type is NV20_SUBDEVICE, so we can locate
        // the correct OBJGPU and attach to its per-engine non-stall event list.
        //
        if ((serverutilGetResourceRef(hEventClient, hNotifier, &pResourceRef) != NV_OK) ||
            (!dynamicCast(pResourceRef->pResource, Subdevice)))
        {
            rmStatus = NV_ERR_INVALID_ARGUMENT;
            goto free_entry;
        }

        pSubDevice = dynamicCast(pResourceRef->pResource, Subdevice);
        hDevice = RES_GET_PARENT_HANDLE(pSubDevice);

        if (CliSetSubDeviceContext(hEventClient, RES_GET_HANDLE(pSubDevice), &hDevice, &pGpu) != NV_OK)
        {
            rmStatus = NV_ERR_INVALID_ARGUMENT;
            goto free_entry;
        }

        rmStatus = eventGetEngineTypeFromSubNotifyIndex(pTargetEvent->NotifyIndex, &engineId);

        if (rmStatus != NV_OK)
            goto free_entry;

        if (IS_MIG_IN_USE(pGpu))
        {
            KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
            NvU32 globalEngineId = engineId;
            MIG_INSTANCE_REF ref;

            NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
                kmigmgrGetInstanceRefFromClient(pGpu, pKernelMIGManager, hEventClient, &ref),
                free_entry);

            NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
                kmigmgrGetLocalToGlobalEngineType(pGpu, pKernelMIGManager, ref, engineId, &globalEngineId),
                free_entry);

            engineId = globalEngineId;
        }

        rmStatus = engineNonStallEventOp(pGpu, engineId,
                        pTargetEvent, NULL, NV_FALSE);
    }

free_entry:
    portMemFree(pTargetEvent);

error:
    NV_ASSERT(rmStatus == NV_OK);
    return rmStatus;
}

static NV_STATUS _removeEventNotification
(
    PEVENTNOTIFICATION *ppEventNotification,
    NvHandle            hEventClient,
    NvHandle            hEvent,
    NvBool              bMatchData,
    NvP64               Data,
    PEVENTNOTIFICATION *ppOldEvent
)
{
    PEVENTNOTIFICATION nextEvent, lastEvent;
    NvBool found = NV_FALSE;

    // check for null list
    nextEvent = NULL;

    if (*ppEventNotification != NULL)
    {
        // check for head of list
        nextEvent = lastEvent = *ppEventNotification;
        if ((nextEvent->hEventClient == hEventClient) &&
            (nextEvent->hEvent == hEvent) &&
            (!bMatchData || (nextEvent->Data == Data)))
        {
            *ppEventNotification = nextEvent->Next;
            found = NV_TRUE;
        }
        else
        {
            // check for internal nodes
            nextEvent = nextEvent->Next;
            while (nextEvent)
            {
                if ((nextEvent->hEventClient == hEventClient) &&
                    (nextEvent->hEvent == hEvent) &&
                    (!bMatchData || (nextEvent->Data == Data)))
                {
                    lastEvent->Next = nextEvent->Next;
                    found = NV_TRUE;
                    break;
                }
                lastEvent = nextEvent;
                nextEvent = nextEvent->Next;
            }
        }
    }

    // delete the event if it was found
    if (found)
    {
        if (nextEvent->bUserOsEventHandle)
            osDereferenceObjectCount(NvP64_VALUE(nextEvent->Data));

        *ppOldEvent = nextEvent;
    }

    return (found) ? NV_OK : NV_ERR_GENERIC;

} // end of unregisterEventNotificationEventNotify()

NV_STATUS notifyEvents
(
    OBJGPU   *pGpu,
    PEVENTNOTIFICATION pEventNotification,
    NvU32     Notifier,
    NvU32     Method,
    NvU32     Data,
    NV_STATUS Status,
    NvU32     Action
)
{
    NV_STATUS rmStatus = NV_OK;
    PEVENTNOTIFICATION NotifyEvent;

    NV_PRINTF(LEVEL_INFO, "  Method = 0x%x\n", Method);
    NV_PRINTF(LEVEL_INFO, "  Data   = 0x%x\n", Data);
    NV_PRINTF(LEVEL_INFO, "  Status = 0x%x\n", Status);
    NV_PRINTF(LEVEL_INFO, "  Action = 0x%x\n", Action);

    // perform the type of action
    switch (Action)
    {
        case NV_OS_WRITE_THEN_AWAKEN:

            // walk this object's event list and find any matches for this specific notify
            for (NotifyEvent = pEventNotification; NotifyEvent; NotifyEvent = NotifyEvent->Next)
            {
                if (NotifyEvent->bSubdeviceSpecificEvent)
                {
                    if (gpumgrGetSubDeviceInstanceFromGpu(pGpu) != NotifyEvent->SubdeviceSpecificValue)
                    {
                        continue;
                    }
                }

                if (NotifyEvent->NotifyIndex == Notifier)
                {
                    // Do any OS specified action related to this notification.
                    if (NotifyEvent->bBroadcastEvent)
                    {
                        //
                        // Only do the OS notify when all sub devices under
                        // a BC device have seen the event.
                        //
                        if (++NotifyEvent->NotifyTriggerCount == NumSubDevices(pGpu))
                        {
                            rmStatus = osNotifyEvent(pGpu, NotifyEvent, Method, Data, Status);
                            NotifyEvent->NotifyTriggerCount = 0x0;
                        }
                    }
                    else
                    {
                        rmStatus = osNotifyEvent(pGpu, NotifyEvent, Method, Data, Status);
                    }
                }
            }
            break;

        default:
            // any other actions are legacy channel-based notifies
            rmStatus = NV_ERR_INVALID_EVENT;
            break;
    }

    return rmStatus;
}

//
// bindEventNotificationToSubdevice
//
// This routine walks the given EVENTNOTIFICATION list and sets
// the designated subdevice instance value for any that are associated
// with the specific NV01_EVENT handle hEvent.
//
NV_STATUS
bindEventNotificationToSubdevice
(
    PEVENTNOTIFICATION pEventNotificationList,
    NvHandle           hEvent,
    NvU32              subdeviceInst
)
{
    PEVENTNOTIFICATION pEventNotify;
    NvU32 count = 0;

    if (pEventNotificationList == NULL)
        return NV_ERR_INVALID_STATE;

    pEventNotify = pEventNotificationList;
    while (pEventNotify)
    {
        if (pEventNotify->hEvent == hEvent)
        {
            pEventNotify->subdeviceInst = subdeviceInst;
            count++;
        }
        pEventNotify = pEventNotify->Next;
    }

    if (count == 0)
        return NV_ERR_INVALID_STATE;

    return NV_OK;
}

NV_STATUS
inotifyConstruct_IMPL(INotifier *pNotifier, CALL_CONTEXT *pCallContext)
{
    if (dynamicCast(pNotifier, RsResource) == NULL)
        return NV_ERR_INVALID_OBJECT;

    return NV_OK;
}

void inotifyDestruct_IMPL(INotifier* pNotifier)
{
    return;
}

PEVENTNOTIFICATION
inotifyGetNotificationList_IMPL
(
    INotifier *pNotifier
)
{
    PEVENTNOTIFICATION *ppEventNotifications = inotifyGetNotificationListPtr(pNotifier);
    if (ppEventNotifications != NULL)
        return *ppEventNotifications;

    return NULL;
}

NV_STATUS
notifyConstruct_IMPL(Notifier *pNotifier, CALL_CONTEXT *pCallContext)
{
    return NV_OK;
}

void notifyDestruct_IMPL(Notifier* pNotifier)
{
    NotifShare *pNotifierShare = inotifyGetNotificationShare(staticCast(pNotifier, INotifier));
    if (pNotifierShare != NULL)
    {
        pNotifierShare->pNotifier = NULL;
        serverFreeShare(&g_resServ, staticCast(pNotifierShare, RsShared));
    }
}

PEVENTNOTIFICATION
*notifyGetNotificationListPtr_IMPL
(
    Notifier *pNotifier
)
{
    NotifShare *pNotifierShare = pNotifier->pNotifierShare;
    if (pNotifierShare == NULL)
        return NULL;

    return &pNotifierShare->pEventList;
}

NotifShare
*notifyGetNotificationShare_IMPL
(
    Notifier *pNotifier
)
{
    return pNotifier->pNotifierShare;
}

void
notifySetNotificationShare_IMPL
(
    Notifier *pNotifier,
    NotifShare *pNotifierShare
)
{
    pNotifier->pNotifierShare = pNotifierShare;
}

NV_STATUS
shrnotifConstruct_IMPL
(
    NotifShare *pNotifShare
)
{
    return NV_OK;
}

void
shrnotifDestruct_IMPL
(
    NotifShare *pNotifShare
)
{
}
