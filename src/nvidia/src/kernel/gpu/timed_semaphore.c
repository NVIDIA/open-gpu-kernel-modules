/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************* Video Manager *******************************\
*                                                                             *
*   This module implements the GF100_TIMED_SEMAPHORE_SW object class and *
*   its corresponding methods.                                                *
*                                                                             *
******************************************************************************/

#include "class/cl0000.h"
#include "kernel/gpu/timed_sema.h"
#include "gpu/timer/objtmr.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "gpu_mgr/gpu_mgr.h"
#include "rmapi/control.h"
#include "kernel/gpu/fifo/kernel_channel.h"

#include "class/cl9074.h"

#define F_NOTIFIER_HI_VALID         (NVBIT(0))
#define F_NOTIFIER_LO_VALID         (NVBIT(1))
#define F_SEMAPHORE_HI_VALID        (NVBIT(2))
#define F_SEMAPHORE_LO_VALID        (NVBIT(3))
#define F_WAIT_TIMESTAMP_HI_VALID   (NVBIT(4))
#define F_WAIT_TIMESTAMP_LO_VALID   (NVBIT(5))
#define F_RELEASE_VALUE_VALID       (NVBIT(6))
#define F_FLUSHING                  (NVBIT(8))

#define F_ALL_VALID (           \
    F_NOTIFIER_HI_VALID       | \
    F_NOTIFIER_LO_VALID       | \
    F_SEMAPHORE_HI_VALID      | \
    F_SEMAPHORE_LO_VALID      | \
    F_WAIT_TIMESTAMP_HI_VALID | \
    F_WAIT_TIMESTAMP_LO_VALID | \
    F_RELEASE_VALUE_VALID       \
)

//---------------------------------------------------------------------------
//
//  Internal prototypes
//
//---------------------------------------------------------------------------

static NV_STATUS _class9074TimerCallback
(
    OBJGPU  *pGpu,
    OBJTMR  *pTmr,
    TMR_EVENT *pTmrEvent
);

//---------------------------------------------------------------------------
//
//  Internal functions
//
//---------------------------------------------------------------------------

static NvBool
_9074TimedSemReleaseNow
(
    PGF100_TIMED_SEM_SW_OBJECT  pTimedSemSw,
    NvU64                       waitTimestamp,
    NvU64                       currentTime,
    NvU16                      *notifierStatus
)
{
    if (waitTimestamp <= currentTime)
    {
        *notifierStatus = NV9074_NOTIFICATION_STATUS_DONE;
        return NV_TRUE;
    }
    else if ((pTimedSemSw->Flags & F_FLUSHING) &&
             (waitTimestamp >= pTimedSemSw->FlushLimitTimestamp))
    {
        *notifierStatus = NV9074_NOTIFICATION_STATUS_DONE_FLUSHED;
        return NV_TRUE;
    }
    return NV_FALSE;
}

static NV_STATUS
_9074TimedSemRelease
(
    OBJGPU     *pGpu,
    ChannelDescendant *pObject,
    Device     *pDevice,
    NvU64       notifierGPUVA,
    NvU64       semaphoreGPUVA,
    NvU64       time,
    NvU32       releaseValue,
    NvU16       notifierStatus,
    NvU32       notifyAction
)
{
    NV_STATUS status;
    NV_STATUS overallStatus = NV_OK;

    status = tsemaRelease_HAL(pGpu,
                              semaphoreGPUVA,
                              notifierGPUVA,
                              pObject->pKernelChannel->hVASpace,
                              releaseValue,
                              notifierStatus,
                              pDevice);

    // timedSemaphoreRelease_HAL will print errors on its own
    if (status != NV_OK)
    {
        if (overallStatus == NV_OK)
            overallStatus = status;
    }

    if (notifyAction)
    {
        PEVENTNOTIFICATION pEventNotifications = inotifyGetNotificationList(staticCast(pObject, INotifier));
        status = notifyEvents(pGpu,
                              pEventNotifications,
                              0 /* Notifier 'Index' */,
                              0 /* Method */,
                              0 /* Data */,
                              NV_OK,
                              notifyAction);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Event notify failed, error 0x%x\n",
                      status);

            if (overallStatus == NV_OK)
                overallStatus = status;
        }
    }

    return overallStatus;
}

static NV_STATUS
_9074TimedSemRequest
(
    OBJGPU     *pGpu,
    ChannelDescendant *pObject,
    NvU64       notifierGPUVA,
    NvU64       semaphoreGPUVA,
    NvU64       waitTimestamp,
    NvU32       releaseValue,
    NvU32       notifyAction
)
{
    OBJTMR                    *pTmr           = GPU_GET_TIMER(pGpu);
    PGF100_TIMED_SEM_SW_OBJECT pTimedSemSw    = dynamicCast(pObject, TimedSemaSwObject);
    NvU64                      currentTime;
    NvU16                      notifierStatus = NV9074_NOTIFICATION_STATUS_PENDING;
    PGF100_TIMED_SEM_ENTRY     pTimedSemEntry = NULL;
    NV_STATUS                  status         = NV_OK;

    // Is it possible to release this semaphore immediately?
    if (listCount(&pTimedSemSw->entryList) == 0)
    {
        tmrGetCurrentTime(pTmr, &currentTime);
        if (IS_VIRTUAL(pGpu) ||
            _9074TimedSemReleaseNow(pTimedSemSw, waitTimestamp, currentTime,
                                    &notifierStatus))
        {
            status = _9074TimedSemRelease(pGpu,
                        pObject,
                        GPU_RES_GET_DEVICE(pTimedSemSw),
                        notifierGPUVA,
                        semaphoreGPUVA,
                        currentTime,
                        releaseValue,
                        notifierStatus,
                        notifyAction);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Timed sem release failed, error 0x%x\n", status);
            }
            return status;
        }
    }

    // Queue the semaphore release entry.
    pTimedSemEntry = listAppendNew(&pTimedSemSw->entryList);
    NV_CHECK_OR_RETURN(LEVEL_INFO, pTimedSemEntry != NULL, NV_ERR_NO_MEMORY);

    pTimedSemEntry->NotifierGPUVA  = notifierGPUVA;
    pTimedSemEntry->SemaphoreGPUVA = semaphoreGPUVA;
    pTimedSemEntry->WaitTimestamp  = waitTimestamp;
    pTimedSemEntry->ReleaseValue   = releaseValue;
    pTimedSemEntry->NotifyAction   = notifyAction;

    // Schedule the callback when entry was added to an empty list.
    if (listCount(&pTimedSemSw->entryList) == 1)
    {
        tmrEventScheduleAbs(pTmr,
            pTimedSemSw->pTmrEvent,
            pTimedSemEntry->WaitTimestamp);
    }

    return status;
}

//---------------------------------------------------------------------------
//
//  Class object creation and destruction
//
//---------------------------------------------------------------------------

NV_STATUS
tsemaConstruct_IMPL
(
    TimedSemaSwObject            *pTimedSemSw,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    ChannelDescendant *pChannelDescendant = staticCast(pTimedSemSw, ChannelDescendant);
    OBJTMR            *pTmr = GPU_GET_TIMER(GPU_RES_GET_GPU(pChannelDescendant));

    tmrEventCreate(pTmr, &pTimedSemSw->pTmrEvent, _class9074TimerCallback, pChannelDescendant, TMR_FLAG_RECUR);

    listInit(&pTimedSemSw->entryList, portMemAllocatorGetGlobalNonPaged());

    return NV_OK;
}

void
tsemaDestruct_IMPL
(
    TimedSemaSwObject *pTimedSemSw
)
{
    ChannelDescendant      *pChannelDescendant = staticCast(pTimedSemSw, ChannelDescendant);
    OBJTMR                 *pTmr = GPU_GET_TIMER(GPU_RES_GET_GPU(pChannelDescendant));

    tmrEventDestroy(pTmr, pTimedSemSw->pTmrEvent);
    pTimedSemSw->pTmrEvent = NULL;

    chandesIsolateOnDestruct(pChannelDescendant);

    // Remove all the entries from the queue.
    listDestroy(&pTimedSemSw->entryList);
}

//---------------------------------------------------------------------------
//
//  Class object control.
//
//---------------------------------------------------------------------------

NV_STATUS
tsemaCtrlCmdFlush_IMPL
(
    TimedSemaSwObject *pTimedSemaSwObject,
    NV9074_CTRL_CMD_FLUSH_PARAMS *pFlushParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pTimedSemaSwObject);

    if (pFlushParams->isFlushing) {
        pTimedSemaSwObject->Flags |= F_FLUSHING;
    }
    else {
        pTimedSemaSwObject->Flags &= ~F_FLUSHING;
    }

    if (pTimedSemaSwObject->Flags & F_FLUSHING)
    {
        OBJTMR *pTmr = GPU_GET_TIMER(pGpu);

        tmrGetCurrentTime(pTmr, &pTimedSemaSwObject->FlushLimitTimestamp);
        pTimedSemaSwObject->FlushLimitTimestamp += pFlushParams->maxFlushTime;

        tmrEventCancel(pTmr, pTimedSemaSwObject->pTmrEvent);
        _class9074TimerCallback(pGpu, pTmr, pTimedSemaSwObject->pTmrEvent);
    }

    return NV_OK;
} // end of tsemaCtrlCmdFlush_IMPL

NV_STATUS
tsemaCtrlCmdGetTime_IMPL
(
    TimedSemaSwObject *pTimedSemaSwObject,
    NV9074_CTRL_CMD_GET_TIME_PARAMS *pGetTimeParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pTimedSemaSwObject);
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);

    tmrGetCurrentTime(pTmr, &pGetTimeParams->currentTime);

    return NV_OK;
} // end of tsemaCtrlCmdGetTime_IMPL

NV_STATUS
tsemaCtrlCmdRelease_IMPL
(
    TimedSemaSwObject *pTimedSemaSwObject,
    NV9074_CTRL_CMD_RELEASE_PARAMS *pReleaseParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pTimedSemaSwObject);
    ChannelDescendant *pObject = staticCast(pTimedSemaSwObject, ChannelDescendant);

    NV_PRINTF(LEVEL_INFO, "\n");

    return _9074TimedSemRequest(pGpu,
                pObject,
                pReleaseParams->notifierGPUVA,
                pReleaseParams->semaphoreGPUVA,
                pReleaseParams->waitTimestamp,
                pReleaseParams->releaseValue,
                DRF_VAL(9074, _CTRL_CMD_RELEASE_FLAGS, _NOTIFY, pReleaseParams->releaseFlags));
} // end of tsemaCtrlCmdRelease_IMPL

//---------------------------------------------------------------------------
//
//  Class method routines.
//
//---------------------------------------------------------------------------

static NV_STATUS _class9074SetNotifierHi
(
    OBJGPU *pGpu,
    ChannelDescendant *pObject,
    NvU32   Offset,
    NvU32   Data
)
{
    PGF100_TIMED_SEM_SW_OBJECT pTimedSemSw = dynamicCast(pObject, TimedSemaSwObject);

    NV_PRINTF(LEVEL_INFO, "\n");

    pTimedSemSw->NotifierHi = DRF_VAL(9074, _SET_NOTIFIER_HI, _V, Data);
    pTimedSemSw->Flags |= F_NOTIFIER_HI_VALID;

    return NV_OK;
} // end of _class9074SetNotifierHi

static NV_STATUS _class9074SetNotifierLo
(
    OBJGPU *pGpu,
    ChannelDescendant *pObject,
    NvU32   Offset,
    NvU32   Data
)
{
    PGF100_TIMED_SEM_SW_OBJECT pTimedSemSw = dynamicCast(pObject, TimedSemaSwObject);
    NvU64                      notifier;

    NV_PRINTF(LEVEL_INFO, "\n");

    if (!(pTimedSemSw->Flags & F_NOTIFIER_HI_VALID))
    {
        NV_PRINTF(LEVEL_ERROR, "NOTIFIER_HI not set\n");
        return NV_ERR_INVALID_STATE;
    }
    if (Data & 0x3)
    {
        NV_PRINTF(LEVEL_ERROR, "Mis-aligned address\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    notifier = pTimedSemSw->NotifierHi;
    notifier <<= 32;
    notifier |= Data;

    pTimedSemSw->NotifierLo = Data;
    pTimedSemSw->NotifierGPUVA = notifier;
    pTimedSemSw->Flags |= F_NOTIFIER_LO_VALID;

    return NV_OK;
} // end of _class9074SetNotifierLo(

static NV_STATUS _class9074SetSemaphoreHi
(
    OBJGPU *pGpu,
    ChannelDescendant *pObject,
    NvU32   Offset,
    NvU32   Data
)
{
    PGF100_TIMED_SEM_SW_OBJECT pTimedSemSw = dynamicCast(pObject, TimedSemaSwObject);

    NV_PRINTF(LEVEL_INFO, "\n");

    pTimedSemSw->SemaphoreHi = DRF_VAL(9074, _SET_SEMAPHORE_HI, _V, Data);
    pTimedSemSw->Flags |= F_SEMAPHORE_HI_VALID;

    return NV_OK;
} // end of _class9074SetSemaphoreHi

static NV_STATUS _class9074SetSemaphoreLo
(
    OBJGPU *pGpu,
    ChannelDescendant *pObject,
    NvU32   Offset,
    NvU32   Data
)
{
    PGF100_TIMED_SEM_SW_OBJECT pTimedSemSw = dynamicCast(pObject, TimedSemaSwObject);
    NvU64                      semaphore;

    NV_PRINTF(LEVEL_INFO, "\n");

    if (!(pTimedSemSw->Flags & F_SEMAPHORE_HI_VALID))
    {
        NV_PRINTF(LEVEL_ERROR, "SEMAPHORE_HI not set\n");
        return NV_ERR_INVALID_STATE;
    }
    if (Data & 0x3)
    {
        NV_PRINTF(LEVEL_ERROR, "Mis-aligned address\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    semaphore = pTimedSemSw->SemaphoreHi;
    semaphore <<= 32;
    semaphore |= Data;

    pTimedSemSw->SemaphoreLo = Data;
    pTimedSemSw->SemaphoreGPUVA = semaphore;
    pTimedSemSw->Flags |= F_SEMAPHORE_LO_VALID;

    return NV_OK;
} // end of _class9074SetSemaphoreLo

static NV_STATUS _class9074SetWaitTimestampHi
(
    OBJGPU *pGpu,
    ChannelDescendant *pObject,
    NvU32   Offset,
    NvU32   Data
)
{
    PGF100_TIMED_SEM_SW_OBJECT pTimedSemSw = dynamicCast(pObject, TimedSemaSwObject);

    NV_PRINTF(LEVEL_INFO, "\n");

    pTimedSemSw->WaitTimestampHi = Data;
    pTimedSemSw->Flags |= F_WAIT_TIMESTAMP_HI_VALID;

    return NV_OK;
} // end of _class9074SetWaitTimestampHi

static NV_STATUS _class9074SetWaitTimestampLo
(
    OBJGPU *pGpu,
    ChannelDescendant *pObject,
    NvU32   Offset,
    NvU32   Data
)
{
    PGF100_TIMED_SEM_SW_OBJECT pTimedSemSw = dynamicCast(pObject, TimedSemaSwObject);

    NV_PRINTF(LEVEL_INFO, "\n");

    if (!(pTimedSemSw->Flags & F_WAIT_TIMESTAMP_HI_VALID))
    {
        NV_PRINTF(LEVEL_ERROR, "WAIT_TIMESTAMP_HI not set\n");
        return NV_ERR_INVALID_STATE;
    }

    pTimedSemSw->WaitTimestampLo = Data;
    pTimedSemSw->Flags |= F_WAIT_TIMESTAMP_LO_VALID;

    pTimedSemSw->WaitTimestamp = pTimedSemSw->WaitTimestampHi;
    pTimedSemSw->WaitTimestamp <<= 32;
    pTimedSemSw->WaitTimestamp |= pTimedSemSw->WaitTimestampLo;

    return NV_OK;
} // end of _class9074SetWaitTimestampLo

static NV_STATUS _class9074SetSemaphoreReleaseValue
(
    OBJGPU *pGpu,
    ChannelDescendant *pObject,
    NvU32   Offset,
    NvU32   Data
)
{
    PGF100_TIMED_SEM_SW_OBJECT pTimedSemSw = dynamicCast(pObject, TimedSemaSwObject);

    NV_PRINTF(LEVEL_INFO, "\n");

    pTimedSemSw->ReleaseValue = Data;
    pTimedSemSw->Flags |= F_RELEASE_VALUE_VALID;

    return NV_OK;
} // end of class9074SetSemaphoreRelease

static NV_STATUS _class9074ScheduleSemaphoreRelease
(
    OBJGPU *pGpu,
    ChannelDescendant *pObject,
    NvU32   Offset,
    NvU32   Data
)
{
    PGF100_TIMED_SEM_SW_OBJECT pTimedSemSw = dynamicCast(pObject, TimedSemaSwObject);

    NV_PRINTF(LEVEL_INFO, "\n");

    if ((pTimedSemSw->Flags & F_ALL_VALID) != F_ALL_VALID)
    {
        NV_PRINTF(LEVEL_ERROR, "Required methods were not written\n");
        return NV_ERR_INVALID_STATE;
    }

    pTimedSemSw->Flags &= ~F_ALL_VALID;

    return _9074TimedSemRequest(pGpu,
                pObject,
                pTimedSemSw->NotifierGPUVA,
                pTimedSemSw->SemaphoreGPUVA,
                pTimedSemSw->WaitTimestamp,
                pTimedSemSw->ReleaseValue,
                DRF_VAL(9074, _SCHEDULE_SEMAPHORE_RELEASE, _NOTIFY, Data));
} // end of _class9074ScheduleSemaphoreRelease

static NV_STATUS _class9074TimerCallback
(
    OBJGPU  *pGpu,
    OBJTMR  *pTmr,
    TMR_EVENT *pTmrEvent
)
{
    ChannelDescendant *pObject = pTmrEvent->pUserData;
    PGF100_TIMED_SEM_SW_OBJECT pTimedSemSw = dynamicCast(pObject, TimedSemaSwObject);
    PGF100_TIMED_SEM_ENTRY     pTimedSemEntry = NULL;
    PGF100_TIMED_SEM_ENTRY     pTimedSemEntryNext = NULL;
    NvU64                      currentTime;
    NvU16                      notifierStatus;
    NV_STATUS                  status = NV_OK;

    NV_PRINTF(LEVEL_INFO, "\n");

    tmrGetCurrentTime(pTmr, &currentTime);

    // Process entries at the head of the queue that can be released now.
    for (pTimedSemEntry = listHead(&pTimedSemSw->entryList);
         pTimedSemEntry != NULL;
         pTimedSemEntry = pTimedSemEntryNext)
    {
        pTimedSemEntryNext = listNext(&pTimedSemSw->entryList, pTimedSemEntry);

        if (!_9074TimedSemReleaseNow(pTimedSemSw, pTimedSemEntry->WaitTimestamp,
                                     currentTime, &notifierStatus))
        {
            break;
        }

        status = _9074TimedSemRelease(pGpu,
                    pObject,
                    GPU_RES_GET_DEVICE(pTimedSemSw),
                    pTimedSemEntry->NotifierGPUVA,
                    pTimedSemEntry->SemaphoreGPUVA,
                    currentTime,
                    pTimedSemEntry->ReleaseValue,
                    notifierStatus,
                    pTimedSemEntry->NotifyAction);

        listRemove(&pTimedSemSw->entryList, pTimedSemEntry);
    }

    // Schedule the callback for entry at the head of the queue.
    if (pTimedSemEntry != NULL)
    {
        tmrEventScheduleAbs(pTmr,
            pTimedSemSw->pTmrEvent,
            pTimedSemEntry->WaitTimestamp);
    }

    return status;
} // end of _class9074TimerCallback

// GF100_TIMED_SEMAPHORE_SW
static const METHOD GF100TimedSemSwMethods[] =
{
    { mthdNoOperation,                    0x0100, 0x0103 },
    { _class9074SetNotifierHi,            0x0140, 0x0143 },
    { _class9074SetNotifierLo,            0x0144, 0x0147 },
    { _class9074SetSemaphoreHi,           0x0148, 0x014b },
    { _class9074SetSemaphoreLo,           0x014c, 0x014f },
    { _class9074SetWaitTimestampHi,       0x0150, 0x0153 },
    { _class9074SetWaitTimestampLo,       0x0154, 0x0157 },
    { _class9074SetSemaphoreReleaseValue, 0x0158, 0x015b },
    { _class9074ScheduleSemaphoreRelease, 0x015c, 0x015f }
};

NV_STATUS tsemaGetSwMethods_IMPL
(
    TimedSemaSwObject *pTimedSemSw,
    const METHOD     **ppMethods,
    NvU32             *pNumMethods
)
{
    *ppMethods = GF100TimedSemSwMethods;
    *pNumMethods = NV_ARRAY_ELEMENTS(GF100TimedSemSwMethods);
    return NV_OK;
}

NvBool
tsemaCheckCallbackReleaseSem_IMPL
(
    TimedSemaSwObject *pTimedSemSw
)
{
    OBJTMR *pTmr = GPU_GET_TIMER(GPU_RES_GET_GPU(pTimedSemSw));
    return tmrEventOnList(pTmr, pTimedSemSw->pTmrEvent);
}

NV_STATUS
tsemaRelease_KERNEL
(
    OBJGPU *pGpu,
    NvU64 semaphoreVA,
    NvU64 notifierVA,
    NvU32 hVASpace,
    NvU32 releaseValue,
    NvU32 completionStatus,
    Device *pDevice
)
{
    OBJTMR   *pTmr = GPU_GET_TIMER(pGpu);
    NvU64     currentTime;
    NV_STATUS status;
    NV_STATUS overallStatus = NV_OK;

    tmrGetCurrentTime(pTmr, &currentTime);

    status = semaphoreFillGPUVATimestamp(pGpu,
                                         pDevice,
                                         hVASpace,
                                         semaphoreVA,
                                         releaseValue,
                                         0, /* Index */
                                         NV_TRUE,
                                         currentTime);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Semaphore fill failed, error 0x%x\n", status);

        if (overallStatus == NV_OK)
            overallStatus = status;
    }

    status = notifyFillNotifierGPUVATimestamp(pGpu,
                                              pDevice,
                                              hVASpace,
                                              notifierVA,
                                              0, /* Info32 */
                                              0, /* Info16 */
                                              completionStatus,
                                              0, /* Index */
                                              currentTime);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Notifier fill failed, error 0x%x\n", status);

        if (overallStatus == NV_OK)
            overallStatus = status;
    }

    return overallStatus;
}
