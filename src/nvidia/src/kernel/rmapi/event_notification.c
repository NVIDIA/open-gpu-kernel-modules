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

/********************************* DMA Manager *****************************\
*                                                                           *
*   Event notifications are handled in this module.  DMA report and OS      *
*   action are dealt with on a per-object basis.                            *
*                                                                           *
****************************************************************************/

#include "core/core.h"
#include "gpu_mgr/gpu_mgr.h"
#include "gpu/gpu.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "class/cl0000.h"
#include "os/os.h"
#include "class/cl0005.h"
#include "gpu/subdevice/subdevice.h"
#include "rmapi/rs_utils.h"
#include "mem_mgr/mem.h"
#include "kernel/gpu/gpu_engine_type.h"
#include "platform/sli/sli.h"
#include "gpu/timer/objtmr.h"

#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"

#include "virtualization/hypervisor/hypervisor.h"

typedef struct
{
    EVENTNOTIFICATION *pEventNotify;
    Memory *pMemory;
    ListNode eventNotificationListNode;

    // Protected by event list spinlock
    ListNode pendingEventNotifyListNode;
    NvBool bInPendingNotifyList;

    //
    // Incremented under event list spinlock when a notification is pending for
    // this event, decremented for each notification sent (not under spinlock).
    //
    volatile NvS32 pendingNotifyCount;
} ENGINE_EVENT_NOTIFICATION;

//
// These lists are intrusive to avoid memory allocation during insertion while
// in a non-preemptible context (holding a spinlock/in an ISR).
//
MAKE_INTRUSIVE_LIST(EngineEventNotificationList, ENGINE_EVENT_NOTIFICATION,
                    eventNotificationListNode);
MAKE_INTRUSIVE_LIST(PendingEventNotifyList, ENGINE_EVENT_NOTIFICATION,
                    pendingEventNotifyListNode);

// Linked list of per engine non-stall event notifications
struct GpuEngineEventNotificationList
{
    PORT_SPINLOCK *pSpinlock;

    // List insertion and removal happens under pSpinlock
    EngineEventNotificationList eventNotificationList;

    // Filled while pSpinlock is held, drained outside of the lock in ISR
    PendingEventNotifyList pendingEventNotifyList;

    //
    // Accessed under pSpinlock, incremented when a thread starts the notify
    // Decremented when the thread finishes notification
    //
    volatile NvU32 activeNotifyThreads;
};

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

NV_STATUS gpuEngineEventNotificationListCreate
(
    OBJGPU *pGpu,
    GpuEngineEventNotificationList **ppEventNotificationList
)
{
    NV_STATUS status = NV_OK;

    PORT_MEM_ALLOCATOR *pAllocator = portMemAllocatorGetGlobalNonPaged();
    GpuEngineEventNotificationList *pEventNotificationList =
        portMemAllocNonPaged(sizeof(*pEventNotificationList));
    NV_ASSERT_OR_RETURN(pEventNotificationList != NULL, NV_ERR_NO_MEMORY);

    portMemSet(pEventNotificationList, 0, sizeof(*pEventNotificationList));

    pEventNotificationList->pSpinlock = portSyncSpinlockCreate(pAllocator);
    NV_ASSERT_OR_ELSE(pEventNotificationList->pSpinlock != NULL,
    {
        status = NV_ERR_INSUFFICIENT_RESOURCES;
        goto exit;
    });

    listInitIntrusive(&pEventNotificationList->eventNotificationList);
    listInitIntrusive(&pEventNotificationList->pendingEventNotifyList);

    pEventNotificationList->activeNotifyThreads = 0;

    *ppEventNotificationList = pEventNotificationList;

exit:
    if (status != NV_OK)
        gpuEngineEventNotificationListDestroy(pGpu, pEventNotificationList);
    return status;
}

void gpuEngineEventNotificationListDestroy
(
    OBJGPU *pGpu,
    GpuEngineEventNotificationList *pEventNotificationList
)
{
    if (pEventNotificationList == NULL)
        return;

    NV_ASSERT(pEventNotificationList->activeNotifyThreads == 0);

    NV_ASSERT(listCount(&pEventNotificationList->pendingEventNotifyList) == 0);
    listDestroy(&pEventNotificationList->pendingEventNotifyList);

    NV_ASSERT(listCount(&pEventNotificationList->eventNotificationList) == 0);
    listDestroy(&pEventNotificationList->eventNotificationList);

    if (pEventNotificationList->pSpinlock != NULL)
        portSyncSpinlockDestroy(pEventNotificationList->pSpinlock);

    portMemFree(pEventNotificationList);
}

static void _gpuEngineEventNotificationListLockPreemptible
(
    GpuEngineEventNotificationList *pEventNotificationList
)
{
    do
    {
        portSyncSpinlockAcquire(pEventNotificationList->pSpinlock);

        //
        // Only return with the lock held once there are no pending
        // notifications to process. No more pending notifications can be queued
        // while the spinlock is held, and we drop the lock to re-enable
        // preemption, to guarantee that _gpuEngineEventNotificationListNotify()
        // can make forward progress to drain the pending notifications list.
        //
        if (pEventNotificationList->activeNotifyThreads == 0)
            return;

        portSyncSpinlockRelease(pEventNotificationList->pSpinlock);

        //
        // Spin waiting for the pending notifications to drain.
        // This can only be done in a preemptible context (i.e., add
        // or remove notification in a thread context).
        //
        while (pEventNotificationList->activeNotifyThreads > 0)
            osSpinLoop();
    } while (NV_TRUE);
}

static inline void _gpuEngineEventNotificationListUnlockPreemptible
(
    GpuEngineEventNotificationList *pEventNotificationList
)
{
    portSyncSpinlockRelease(pEventNotificationList->pSpinlock);
}

static NV_STATUS _gpuEngineEventNotificationInsert
(
    GpuEngineEventNotificationList *pEventNotificationList,
    EVENTNOTIFICATION *pEventNotify,
    Memory *pMemory
)
{
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pEventNotify != NULL,
                       NV_ERR_INVALID_ARGUMENT);

    // Allocate the new node outside of the spinlock
    ENGINE_EVENT_NOTIFICATION *pEngineEventNotification =
        portMemAllocNonPaged(sizeof(*pEngineEventNotification));

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pEngineEventNotification != NULL,
                       NV_ERR_NO_MEMORY);

    portMemSet(pEngineEventNotification, 0, sizeof(*pEngineEventNotification));

    pEngineEventNotification->pEventNotify = pEventNotify;
    pEngineEventNotification->pMemory = pMemory;

    // Take the lock to add the node to the list
    _gpuEngineEventNotificationListLockPreemptible(pEventNotificationList);
    {
        listPrependExisting(&pEventNotificationList->eventNotificationList,
                            pEngineEventNotification);
    }
    _gpuEngineEventNotificationListUnlockPreemptible(pEventNotificationList);

    return NV_OK;
}

static void _gpuEngineEventNotificationRemove
(
    GpuEngineEventNotificationList *pEventNotificationList,
    EVENTNOTIFICATION *pEventNotify
)
{
    ENGINE_EVENT_NOTIFICATION *pEngineEventNotification = NULL;

    _gpuEngineEventNotificationListLockPreemptible(pEventNotificationList);
    {
        EngineEventNotificationListIter it =
            listIterAll(&pEventNotificationList->eventNotificationList);
        while (listIterNext(&it))
        {
            if (it.pValue->pEventNotify == pEventNotify)
            {
                pEngineEventNotification = it.pValue;
                listRemove(&pEventNotificationList->eventNotificationList,
                           pEngineEventNotification);
                break;
            }
        }
    }
    _gpuEngineEventNotificationListUnlockPreemptible(pEventNotificationList);

    NV_ASSERT(pEngineEventNotification != NULL);
    portMemFree(pEngineEventNotification);
}

static NV_STATUS _gpuEngineEventNotificationListNotify
(
    OBJGPU *pGpu,
    GpuEngineEventNotificationList *pEventNotificationList,
    NvHandle hEvent
)
{
    NV_STATUS status = NV_OK;
    PendingEventNotifyList *pPending =
        &pEventNotificationList->pendingEventNotifyList;
    ENGINE_EVENT_NOTIFICATION *pIter, *pTail;

    //
    // Acquire engine list spinlock before traversing the list. Note that this
    // is called without holding locks from ISR for Linux. This spinlock is used
    // to protect the per GPU per engine event node list.
    //
    portSyncSpinlockAcquire(pEventNotificationList->pSpinlock);
    {
        pEventNotificationList->activeNotifyThreads++;

        EngineEventNotificationListIter it =
            listIterAll(&pEventNotificationList->eventNotificationList);
        while (listIterNext(&it))
        {
            ENGINE_EVENT_NOTIFICATION *pEngineEventNotification = it.pValue;
            if (hEvent &&
                pEngineEventNotification->pEventNotify->hEvent != hEvent)
                continue;

            Memory *pSemMemory = pEngineEventNotification->pMemory;
            if (pSemMemory &&
                pSemMemory->vgpuNsIntr.isSemaMemValidationEnabled &&
                pSemMemory->pMemDesc && pSemMemory->pMemDesc->Allocated)
            {
                NvU32 *pTempKernelMapping =
                    (NvU32 *)NvP64_VALUE(
                        memdescGetKernelMapping(pSemMemory->pMemDesc));
                if (pTempKernelMapping == NULL)
                {
                    NV_PRINTF(LEVEL_WARNING,
                        "Per-vGPU semaphore location mapping is NULL."
                        " Skipping the current node.\n");
                    continue;
                }

                NvU32 semValue = MEM_RD32(pTempKernelMapping +
                                          (pSemMemory->vgpuNsIntr.nsSemOffset /
                                           sizeof(NvU32)));

                if (pSemMemory->vgpuNsIntr.nsSemValue == semValue)
                    continue;

                pSemMemory->vgpuNsIntr.nsSemValue = semValue;

                {
                    OBJSYS *pSys = SYS_GET_INSTANCE();
                    OBJHYPERVISOR *pHypervisor = SYS_GET_HYPERVISOR(pSys);

                    if (pHypervisor != NULL)
                    {
                        NV_STATUS intrStatus =
                            hypervisorInjectInterrupt(pHypervisor,
                                                      &pSemMemory->vgpuNsIntr);

                        //
                        // If we have successfully injected MSI into guest,
                        // then we can jump to the next semaphore location;
                        // otherwise, we need to call osNotifyEvent below to
                        // wake up the plugin.
                        //
                        if (intrStatus == NV_OK)
                            continue;
                    }
                }
            }

            portAtomicIncrementS32(&pEngineEventNotification->pendingNotifyCount);

            //
            // Queue up this event notification to be completed outside of the
            // critical section, as the osNotifyEvent implementation may need
            // to be preemptible.
            //
            if (!pEngineEventNotification->bInPendingNotifyList)
            {
                pEngineEventNotification->bInPendingNotifyList = NV_TRUE;
                listAppendExisting(pPending, pEngineEventNotification);
            }
        }

        //
        // We can't use the list iterator, because listIterNext() will assert
        // if a node is appended to the list after the iterator has been
        // initialized. For the loop below, it's safe to iterate over the list,
        // (up to the point of the last node appended above), since nodes can't
        // be removed from the list while the list's `activeNotifyThreads` is
        // non-zero.
        //
        pIter = listHead(pPending);
        pTail = listTail(pPending);
    }
    portSyncSpinlockRelease(pEventNotificationList->pSpinlock);

    //
    // Iterate through the pending notifications and call the OS to send them.
    // pIter and pTail are initialized to the list head and tail respectively,
    // under the spinlock above.
    //
    while (pIter != NULL)
    {
        //
        // Don't miss firing events - latch the pending count.
        // This can race with the `portAtomicIncrementS32()` above.
        //  - If the increment wins, then the next thread to call the
        //    portAtomicCompareAndSwap32 with the incremented value will send
        //    all of the notifications. Example: thread A is preempted by ISR
        //    here, ISR will run through and send all notifications. Thread A
        //    will get pendingNotifyCount = 0 when it resumes and skip this
        //    element.
        //  - If this loop wins, thread A will proceed to send N notifications,
        //    and thread B will increment the pendingNotifyCount from 0 to 1.
        //    Thread B will service its own notification when it reaches this
        //    loop.
        //
        NvS32 pendingNotifyCount;
        do
            pendingNotifyCount = pIter->pendingNotifyCount;
        while ((pendingNotifyCount > 0) &&
                !portAtomicCompareAndSwapS32(&pIter->pendingNotifyCount, 0, pendingNotifyCount));

        while (pendingNotifyCount--)
            NV_ASSERT_OK_OR_CAPTURE_FIRST_ERROR(status,
                osNotifyEvent(pGpu, pIter->pEventNotify, 0, 0, NV_OK));

        if (pIter == pTail)
            break;

        pIter = listNext(pPending, pIter);
    }

    portSyncSpinlockAcquire(pEventNotificationList->pSpinlock);
    {
        //
        // The last active notify thread drains the pending notify list.
        // Otherwise, we could be removing list nodes while it's being
        // iterated over by another thread above, outside of the protection of
        // the spinlock.
        //
        if (--pEventNotificationList->activeNotifyThreads == 0)
        {
            ENGINE_EVENT_NOTIFICATION *pNext;
            for (pIter = listHead(pPending); pIter != NULL; pIter = pNext)
            {
                pNext = listNext(pPending, pIter);

                //
                // There should not be any unsent notifications at this point,
                // since we are the last active thread.
                //
                NV_ASSERT(pIter->pendingNotifyCount == 0);

                pIter->bInPendingNotifyList = NV_FALSE;
                listRemove(pPending, pIter);
            }
        }
    }
    portSyncSpinlockRelease(pEventNotificationList->pSpinlock);

    return status;
}

NV_STATUS
engineNonStallIntrNotify(OBJGPU *pGpu, RM_ENGINE_TYPE rmEngineId)
{
    NV_ASSERT_OR_RETURN(rmEngineId < NV_ARRAY_ELEMENTS(pGpu->engineNonstallIntrEventNotifications),
                        NV_ERR_INVALID_ARGUMENT);
    return _gpuEngineEventNotificationListNotify(pGpu,
        pGpu->engineNonstallIntrEventNotifications[rmEngineId], 0);
}

NV_STATUS
engineNonStallIntrNotifyEvent(OBJGPU *pGpu, RM_ENGINE_TYPE rmEngineId, NvHandle hEvent)
{
    NV_ASSERT_OR_RETURN(rmEngineId < NV_ARRAY_ELEMENTS(pGpu->engineNonstallIntrEventNotifications),
                        NV_ERR_INVALID_ARGUMENT);
    return _gpuEngineEventNotificationListNotify(pGpu,
        pGpu->engineNonstallIntrEventNotifications[rmEngineId], hEvent);
}

static NV_STATUS
eventGetEngineTypeFromSubNotifyIndex
(
    NvU32 notifyIndex,
    RM_ENGINE_TYPE *pRmEngineId
)
{
    NV_ASSERT_OR_RETURN(pRmEngineId, NV_ERR_INVALID_ARGUMENT);

    *pRmEngineId = RM_ENGINE_TYPE_NULL;

    switch (notifyIndex)
    {
        case NV2080_NOTIFIERS_FIFO_EVENT_MTHD:
            *pRmEngineId = RM_ENGINE_TYPE_HOST;
            break;
        case NV2080_NOTIFIERS_CE0:
            *pRmEngineId = RM_ENGINE_TYPE_COPY0;
            break;
        case NV2080_NOTIFIERS_CE1:
            *pRmEngineId = RM_ENGINE_TYPE_COPY1;
            break;
        case NV2080_NOTIFIERS_CE2:
            *pRmEngineId = RM_ENGINE_TYPE_COPY2;
            break;
        case NV2080_NOTIFIERS_CE3:
            *pRmEngineId = RM_ENGINE_TYPE_COPY3;
            break;
        case NV2080_NOTIFIERS_CE4:
            *pRmEngineId = RM_ENGINE_TYPE_COPY4;
            break;
        case NV2080_NOTIFIERS_CE5:
            *pRmEngineId = RM_ENGINE_TYPE_COPY5;
            break;
        case NV2080_NOTIFIERS_CE6:
            *pRmEngineId = RM_ENGINE_TYPE_COPY6;
            break;
        case NV2080_NOTIFIERS_CE7:
            *pRmEngineId = RM_ENGINE_TYPE_COPY7;
            break;
        case NV2080_NOTIFIERS_CE8:
            *pRmEngineId = RM_ENGINE_TYPE_COPY8;
            break;
        case NV2080_NOTIFIERS_CE9:
            *pRmEngineId = RM_ENGINE_TYPE_COPY9;
            break;
        case NV2080_NOTIFIERS_CE10:
            *pRmEngineId = RM_ENGINE_TYPE_COPY10;
            break;
        case NV2080_NOTIFIERS_CE11:
            *pRmEngineId = RM_ENGINE_TYPE_COPY11;
            break;
        case NV2080_NOTIFIERS_CE12:
            *pRmEngineId = RM_ENGINE_TYPE_COPY12;
            break;
        case NV2080_NOTIFIERS_CE13:
            *pRmEngineId = RM_ENGINE_TYPE_COPY13;
            break;
        case NV2080_NOTIFIERS_CE14:
            *pRmEngineId = RM_ENGINE_TYPE_COPY14;
            break;
        case NV2080_NOTIFIERS_CE15:
            *pRmEngineId = RM_ENGINE_TYPE_COPY15;
            break;
        case NV2080_NOTIFIERS_CE16:
            *pRmEngineId = RM_ENGINE_TYPE_COPY16;
            break;
        case NV2080_NOTIFIERS_CE17:
            *pRmEngineId = RM_ENGINE_TYPE_COPY17;
            break;
        case NV2080_NOTIFIERS_CE18:
            *pRmEngineId = RM_ENGINE_TYPE_COPY18;
            break;
        case NV2080_NOTIFIERS_CE19:
            *pRmEngineId = RM_ENGINE_TYPE_COPY19;
            break;
        case NV2080_NOTIFIERS_GR0:
            *pRmEngineId = RM_ENGINE_TYPE_GR0;
            break;
        case NV2080_NOTIFIERS_GR1:
            *pRmEngineId = RM_ENGINE_TYPE_GR1;
            break;
        case NV2080_NOTIFIERS_GR2:
            *pRmEngineId = RM_ENGINE_TYPE_GR2;
            break;
        case NV2080_NOTIFIERS_GR3:
            *pRmEngineId = RM_ENGINE_TYPE_GR3;
            break;
        case NV2080_NOTIFIERS_GR4:
            *pRmEngineId = RM_ENGINE_TYPE_GR4;
            break;
        case NV2080_NOTIFIERS_GR5:
            *pRmEngineId = RM_ENGINE_TYPE_GR5;
            break;
        case NV2080_NOTIFIERS_GR6:
            *pRmEngineId = RM_ENGINE_TYPE_GR6;
            break;
        case NV2080_NOTIFIERS_GR7:
            *pRmEngineId = RM_ENGINE_TYPE_GR7;
            break;
        case NV2080_NOTIFIERS_PPP:
            *pRmEngineId = RM_ENGINE_TYPE_PPP;
            break;
        case NV2080_NOTIFIERS_NVDEC0:
            *pRmEngineId = RM_ENGINE_TYPE_NVDEC0;
            break;
        case NV2080_NOTIFIERS_NVDEC1:
            *pRmEngineId = RM_ENGINE_TYPE_NVDEC1;
            break;
        case NV2080_NOTIFIERS_NVDEC2:
            *pRmEngineId = RM_ENGINE_TYPE_NVDEC2;
            break;
        case NV2080_NOTIFIERS_NVDEC3:
            *pRmEngineId = RM_ENGINE_TYPE_NVDEC3;
            break;
        case NV2080_NOTIFIERS_NVDEC4:
            *pRmEngineId = RM_ENGINE_TYPE_NVDEC4;
            break;
        case NV2080_NOTIFIERS_NVDEC5:
            *pRmEngineId = RM_ENGINE_TYPE_NVDEC5;
            break;
        case NV2080_NOTIFIERS_NVDEC6:
            *pRmEngineId = RM_ENGINE_TYPE_NVDEC6;
            break;
        case NV2080_NOTIFIERS_NVDEC7:
            *pRmEngineId = RM_ENGINE_TYPE_NVDEC7;
            break;
        case NV2080_NOTIFIERS_PDEC:
            *pRmEngineId = RM_ENGINE_TYPE_VP;
            break;
        case NV2080_NOTIFIERS_MSENC:
            NV_ASSERT(NV2080_NOTIFIERS_MSENC   == NV2080_NOTIFIERS_NVENC0);
            NV_ASSERT(RM_ENGINE_TYPE_MSENC == RM_ENGINE_TYPE_NVENC0);
            *pRmEngineId = RM_ENGINE_TYPE_MSENC;
            break;
        case NV2080_NOTIFIERS_NVENC1:
            *pRmEngineId = RM_ENGINE_TYPE_NVENC1;
            break;
        case NV2080_NOTIFIERS_NVENC2:
            *pRmEngineId = RM_ENGINE_TYPE_NVENC2;
            break;
// Bug 4175886 - Use this new value for all chips once GB20X is released
        case NV2080_NOTIFIERS_NVENC3:
            *pRmEngineId = RM_ENGINE_TYPE_NVENC3;
            break;
        case NV2080_NOTIFIERS_SEC2:
            *pRmEngineId = RM_ENGINE_TYPE_SEC2;
            break;
        case NV2080_NOTIFIERS_NVJPEG0:
            *pRmEngineId = RM_ENGINE_TYPE_NVJPEG0;
            break;
        case NV2080_NOTIFIERS_NVJPEG1:
            *pRmEngineId = RM_ENGINE_TYPE_NVJPEG1;
            break;
        case NV2080_NOTIFIERS_NVJPEG2:
            *pRmEngineId = RM_ENGINE_TYPE_NVJPEG2;
            break;
        case NV2080_NOTIFIERS_NVJPEG3:
            *pRmEngineId = RM_ENGINE_TYPE_NVJPEG3;
            break;
        case NV2080_NOTIFIERS_NVJPEG4:
            *pRmEngineId = RM_ENGINE_TYPE_NVJPEG4;
            break;
        case NV2080_NOTIFIERS_NVJPEG5:
            *pRmEngineId = RM_ENGINE_TYPE_NVJPEG5;
            break;
        case NV2080_NOTIFIERS_NVJPEG6:
            *pRmEngineId = RM_ENGINE_TYPE_NVJPEG6;
            break;
        case NV2080_NOTIFIERS_NVJPEG7:
            *pRmEngineId = RM_ENGINE_TYPE_NVJPEG7;
            break;
        case NV2080_NOTIFIERS_OFA0:
            *pRmEngineId = RM_ENGINE_TYPE_OFA0;
            break;
        case NV2080_NOTIFIERS_OFA1:
            *pRmEngineId = RM_ENGINE_TYPE_OFA1;
            break;
        default:
            NV_PRINTF(LEVEL_WARNING,
                      "notifier 0x%x doesn't use the fast non-stall interrupt path!\n",
                      notifyIndex);
            NV_ASSERT(0);
            return NV_ERR_NOT_SUPPORTED;
    }

    return NV_OK;
}

NV_STATUS registerEventNotification
(
    PEVENTNOTIFICATION *ppEventNotification,
    RsClient           *pEventClient,
    NvHandle            hNotifier,
    NvHandle            hEvent,
    NvU32               NotifyIndex,
    NvU32               NotifyType,
    NvP64               Data,
    NvBool              bUserOsEventHandle
)
{
    NvHandle hEventClient = pEventClient->hClient;
    Subdevice *pSubDevice;
    PEVENTNOTIFICATION pTargetEvent = NULL;
    NV_STATUS rmStatus = NV_OK, rmTmpStatus = NV_OK;
    OBJGPU *pGpu;
    NvBool bNonStallIntrEvent = NV_FALSE;
    RM_ENGINE_TYPE rmEngineId;
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
        if ((clientGetResourceRef(pEventClient, hNotifier, &pResourceRef) != NV_OK) ||
            (!dynamicCast(pResourceRef->pResource, Subdevice)))
        {
            rmStatus = NV_ERR_INVALID_ARGUMENT;
            goto free_entry;
        }

        pSubDevice = dynamicCast(pResourceRef->pResource, Subdevice);

        pGpu = GPU_RES_GET_GPU(pSubDevice);

        GPU_RES_SET_THREAD_BC_STATE(pSubDevice);

        rmStatus = eventGetEngineTypeFromSubNotifyIndex(
                        DRF_VAL(0005, _NOTIFY_INDEX, _INDEX, NotifyIndex), &rmEngineId);

        if (rmStatus != NV_OK)
            goto free_entry;

        if (IS_MIG_IN_USE(pGpu))
        {
            KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
            RM_ENGINE_TYPE globalRmEngineId = rmEngineId;
            MIG_INSTANCE_REF ref;

            NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
                kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                                GPU_RES_GET_DEVICE(pSubDevice), &ref),
                free_entry);

            NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
                kmigmgrGetLocalToGlobalEngineType(pGpu, pKernelMIGManager, ref, rmEngineId, &globalRmEngineId),
                free_entry);

            rmEngineId = globalRmEngineId;
        }

        if (pSubDevice->hSemMemory != NV01_NULL_OBJECT)
        {
            NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_SILENT,
                memGetByHandle(RES_GET_CLIENT(pSubDevice),
                               pSubDevice->hSemMemory,
                               &pSemMemory),
                free_entry);
        }

        rmStatus = _gpuEngineEventNotificationInsert(
                        pGpu->engineNonstallIntrEventNotifications[rmEngineId],
                        *ppEventNotification, pSemMemory);

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

    // These fields are set by NV0004_CTRL_CMD_TMR_SET_ALARM_NOTIFY for graceful TMR_EVENT teardown
    EventNotify->pGpu = NULL;
    EventNotify->pTmrEvent = NULL;

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
    RM_ENGINE_TYPE          rmEngineId;
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

        // Fetch pGpu and hDevice, set the threadstate to the pGpu
        pGpu = GPU_RES_GET_GPU(pSubDevice);

        GPU_RES_SET_THREAD_BC_STATE(pSubDevice);

        rmStatus = eventGetEngineTypeFromSubNotifyIndex(pTargetEvent->NotifyIndex, &rmEngineId);

        if (rmStatus != NV_OK)
            goto free_entry;

        if (IS_MIG_IN_USE(pGpu))
        {
            KernelMIGManager *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
            RM_ENGINE_TYPE globalRmEngineId = rmEngineId;
            MIG_INSTANCE_REF ref;

            NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
                kmigmgrGetInstanceRefFromDevice(pGpu, pKernelMIGManager,
                                                GPU_RES_GET_DEVICE(pSubDevice), &ref),
                free_entry);

            NV_CHECK_OK_OR_GOTO(rmStatus, LEVEL_ERROR,
                kmigmgrGetLocalToGlobalEngineType(pGpu, pKernelMIGManager, ref, rmEngineId, &globalRmEngineId),
                free_entry);

            rmEngineId = globalRmEngineId;
        }

        _gpuEngineEventNotificationRemove(
            pGpu->engineNonstallIntrEventNotifications[rmEngineId],
            pTargetEvent);
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
        if (nextEvent->pTmrEvent != NULL)
        {
            NV_ASSERT_OR_RETURN((nextEvent->pGpu != NULL), NV_ERR_INVALID_STATE);

            tmrEventDestroy(GPU_GET_TIMER(nextEvent->pGpu), nextEvent->pTmrEvent);
            nextEvent->pGpu = NULL;
            nextEvent->pTmrEvent = NULL;
        }

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
