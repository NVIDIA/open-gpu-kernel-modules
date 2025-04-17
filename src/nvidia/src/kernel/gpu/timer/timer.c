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

/**
 * @file  timer.c
 * @brief Timer Object Function Definitions.
 */

/* ------------------------ Includes ---------------------------------------- */
#include "gpu/timer/objtmr.h"
#include "class/cl0004.h" // NV004_NOTIFIERS_SET_ALARM_NOTIFY
#include "gpu/gpu_resource.h"
#include "core/locks.h"
#include "kernel/gpu/intr/engine_idx.h"
#include "kernel/gpu/intr/intr.h"

/* ------------------------ Static Function Prototypes ---------------------- */
static TMR_EVENT_PVT *  _tmrPullCallbackFromHead (OBJTMR *);
static void             _tmrScanCallback(OBJTMR *, void *);
static void             _tmrScanCallbackOSTimer(OBJTMR *, TMR_EVENT_PVT *);
static TMR_EVENT_PVT *  _tmrGetNextFreeCallback(OBJTMR *);
static NV_STATUS        _tmrInsertCallback(OBJTMR *, TMR_EVENT_PVT *, NvU64);
static void             _tmrInsertCallbackInList(OBJGPU *pGpu, OBJTMR *pTmr, TMR_EVENT_PVT *pEvent);
static void             _tmrStateLoadCallbacks(OBJGPU *, OBJTMR *);
static NV_STATUS        _tmrGetNextAlarmTime(OBJTMR *, NvU64 *);
static void             _tmrScheduleCallbackInterrupt(OBJGPU *, OBJTMR *, NvU64);

NV_STATUS
tmrConstructEngine_IMPL
(
    OBJGPU       *pGpu,
    OBJTMR       *pTmr,
    ENGDESCRIPTOR engDesc
)
{
    // Mark that this timer is not yet initialized
    pTmr->bInitialized = NV_FALSE;

    // Create the Granular lock for SWRL Timer callback
    pTmr->pTmrSwrlLock = portSyncSpinlockCreate(portMemAllocatorGetGlobalNonPaged());
    if (pTmr->pTmrSwrlLock == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Alloc spinlock failed\n");
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    tmrInitCallbacks(pTmr);
    osInit1HzCallbacks(pTmr);

    pTmr->retryTimes = 3;

    pTmr->errorCount = 0;

    pTmr->pGrTickFreqRefcnt = NULL;

    return NV_OK;
}

static RefcntStateChangeCallback _tmrGrTimeStampFreqStateChange;
static NV_STATUS _tmrGrTimeStampFreqRefcntInit(OBJGPU *, OBJTMR *);

/*!
 * @brief Initializes the GR timer specific state of OBJTMR.
 *
 * Since this is initialization of client-level structures that won't be
 * touched yet, we can execute without the GPU locks.
 *
 * @param[in] pGpu  - OBJGPU pointer
 * @param[in] pHwpm - OBJTMR pointer
 *
 * @returns NV_OK if the profiler-specific state is successfully initialized
 *          Other errors from _tmrGrTimeStampFreqRefcntInit()
 */
NV_STATUS
tmrStateInitUnlocked_IMPL
(
    OBJGPU  *pGpu,
    OBJTMR  *pTmr
)
{
    NV_STATUS  status;

    status = _tmrGrTimeStampFreqRefcntInit(pGpu, pTmr);

    return status;
}

/*!
 * @brief Creates the reference counters for GR Time Stamp update frequency.
 *
 * @param[in] pGpu  - OBJGPU pointer
 * @param[in] pTmr  - OBJTMR pointer
 *
 * @returns NV_OK if the reference counters are created successfully or the
 *              REFCNT engine is disabled
 *          NV_ERR_INVALID_STATE if one of the reference counters could not
 *              be loaded after creation
 *          Other errors from objCreate() and refcntConstruct()
 */
static NV_STATUS
_tmrGrTimeStampFreqRefcntInit
(
    OBJGPU  *pGpu,
    OBJTMR  *pTmr
)
{
    NV_STATUS  status  = NV_OK;
    status = objCreate(&pTmr->pGrTickFreqRefcnt, pTmr, OBJREFCNT,
                staticCast(pTmr, Dynamic), 0, &_tmrGrTimeStampFreqStateChange, NULL);

    return status;
}

/*!
 * @brief Changes GR engine time stamp update frequency on behalf of the
 *        associated reference counter.
 *
 * @param[in] pRefcnt     - OBJREFCNT pointer
 * @param[in] pDynamic    - OBJTMR pointer
 * @param[in] oldState    - The current state of the setting
 * @param[in] newState    - The next intended state of the setting
 *
 * @returns NV_OK if the power feature state is successfully changed
 *          NV_ERR_INVALID_STATE if the callback context state of the
 *              reference counter cannot be loaded
 *          NV_ERR_NOT_SUPPORTED if the callback is called on an unknown
 *              reference counter
 *          Other errors from tmrGrTimeStampFreqStateChange_HAL(),
 */
static NV_STATUS
_tmrGrTimeStampFreqStateChange
(
    OBJREFCNT   *pRefcnt,
    Dynamic     *pDynamic,
    REFCNT_STATE oldState,
    REFCNT_STATE newState
)
{
    NV_STATUS status        = NV_OK;
    OBJTMR   *pTmr          = dynamicCast(pDynamic, OBJTMR);
    OBJGPU *pGpu            = NULL;
    NvBool bSetMaxFreq      = (newState == REFCNT_STATE_ENABLED);

    if (pRefcnt == NULL)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    if (pTmr == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    pGpu = TMR_GET_GPU(pTmr);
    if (pGpu == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    status = tmrGrTickFreqChange_HAL(pGpu, pTmr, bSetMaxFreq);

    return status;
}

void
tmrDestruct_IMPL(OBJTMR  *pTmr)
{
    // Delete the Granular lock for SWRL Timer callback
    if (pTmr->pTmrSwrlLock != NULL)
    {
        portSyncSpinlockDestroy(pTmr->pTmrSwrlLock);
        pTmr->pTmrSwrlLock = NULL;
    }

    objDelete(pTmr->pGrTickFreqRefcnt);
    pTmr->pGrTickFreqRefcnt = NULL;

    osDestroy1HzCallbacks(pTmr);
}

/*!
 * Simple Utility function, checks if there are any queued callbacks
 */
static NV_INLINE NvBool tmrEventsExist(OBJTMR *pTmr)
{
    return pTmr->pRmActiveEventList != NULL;
}

static NV_INLINE NvBool tmrIsOSTimer(OBJTMR *pTmr, TMR_EVENT *pEventPublic)
{
    return ((pEventPublic != NULL) &&
           (pTmr->getProperty(pTmr, PDB_PROP_TMR_USE_OS_TIMER_FOR_CALLBACKS) ||
           (pEventPublic->flags & TMR_FLAG_USE_OS_TIMER)));
}

/*!
 * Allocates the necessary memory for storing a callback in the timer.
 *
 * @param[out]  ppEvent  A reference to the client's pointer.
 */
NV_STATUS tmrEventCreate_IMPL
(
    OBJTMR     *pTmr,
    TMR_EVENT **ppEventPublic,
    TIMEPROC    Proc,
    void       *pUserData,
    NvU32       flags
)
{
    TMR_EVENT_PVT **ppEvent = (TMR_EVENT_PVT **)ppEventPublic;
    NV_STATUS status = NV_OK;

    *ppEvent = portMemAllocNonPaged(sizeof(TMR_EVENT_PVT));
    if (*ppEvent == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Failed to allocate timer event\n");
        return NV_ERR_NO_MEMORY;
    }
    (*ppEvent)->bLegacy         = NV_FALSE;
    (*ppEvent)->bInUse          = NV_FALSE;
    (*ppEvent)->pNext           = NULL;
    (*ppEventPublic)->pTimeProc = Proc;
    (*ppEventPublic)->pUserData = pUserData;
    (*ppEventPublic)->flags     = flags;

    if (tmrIsOSTimer(pTmr, *ppEventPublic))
    {
        status = tmrEventCreateOSTimer_HAL(pTmr, *ppEventPublic);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to create OS timer \n");
        }
    }
    return status;
}

static void
_tmrScheduleCallbackInterrupt
(
    OBJGPU *pGpu,
    OBJTMR *pTmr,
    NvU64   alarmTime
)
{
    //
    // Don't schedule the interrupt if we are polling.  The interrupt can be
    // routed to a different device, which could get confused.  Also we don't
    // want the extra priv writes.
    //
    if (pTmr->getProperty(pTmr, PDB_PROP_TMR_USE_POLLING_FOR_CALLBACKS))
        return;

    if (pTmr->getProperty(pTmr, PDB_PROP_TMR_USE_COUNTDOWN_TIMER_FOR_RM_CALLBACKS))
    {
        NvU64 currentTime;
        // 
        // Bug: 5071665, 4417666. High spikes of DPC Activity in ARM based system.
        // The issue is caused due to an ARM compiler optimization bug which is doing 
        // 32-bit subtraction instead of 64-bit subtraction casted to 32-bits. 
        // Fix: Set 'volatile' qualifier to' countdownTime' so that it is not optimized.
        //
        volatile NvU32 countdownTime = 0;

        tmrGetCurrentTime(pTmr, &currentTime);
        if (currentTime < alarmTime)
        {
            countdownTime = NvU64_LO32(alarmTime - currentTime);
        }
        tmrSetCountdown_HAL(pGpu, pTmr, countdownTime, 0, NULL);
    }
    else
    {
        tmrSetAlarm_HAL(pGpu, pTmr, alarmTime, NULL);
    }
}

void
tmrResetCallbackInterrupt_IMPL
(
    OBJGPU *pGpu,
    OBJTMR *pTmr
)
{
    if (pTmr->getProperty(pTmr, PDB_PROP_TMR_USE_COUNTDOWN_TIMER_FOR_RM_CALLBACKS))
    {
        tmrSetCountdownIntrReset_HAL(pGpu, pTmr, NULL);
    }
    else
    {
        tmrSetAlarmIntrReset_HAL(pGpu, pTmr, NULL);
    }
}

NvBool
tmrGetCallbackInterruptPending_IMPL
(
    OBJGPU *pGpu,
    OBJTMR *pTmr
)
{
    if (pTmr->getProperty(pTmr, PDB_PROP_TMR_USE_COUNTDOWN_TIMER_FOR_RM_CALLBACKS))
    {
        return tmrGetCountdownPending_HAL(pGpu, pTmr, NULL);
    }
    else
    {
        return tmrGetAlarmPending_HAL(pGpu, pTmr, NULL);
    }
}

/*!
 * Cancels a given callback, marking it invalid and preventing it from being executed.
 * Updates the next alarm time appropriately
 *
 * @param[in]   pEvent   The callback to be cancelled
 */
void tmrEventCancel_IMPL
(
    OBJTMR         *pTmr,
    TMR_EVENT      *pEventPublic
)
{
    NvU64 nextAlarmTime;
    OBJGPU *pGpu = ENG_GET_GPU(pTmr);
    TMR_EVENT_PVT *pEvent = (TMR_EVENT_PVT *)pEventPublic;
    TMR_EVENT_PVT *pChaser = pTmr->pRmActiveEventList;
    NvBool bRemovedHead = pChaser == pEvent;

    if (pEventPublic == NULL)
    {
        return;
    }

    NV_ASSERT(!pEvent->bLegacy);

    pEvent->bInUse = NV_FALSE;

    if (tmrIsOSTimer(pTmr, pEventPublic))
    {
        NV_STATUS status = NV_OK;
        _tmrScanCallbackOSTimer(pTmr, pEvent);
        status = tmrEventCancelOSTimer_HAL(pTmr, pEventPublic);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed in cancel of OS timer callback\n");
        }
        return;
    }

    if (bRemovedHead)
    {
        pTmr->pRmActiveEventList = pEvent->pNext;

        // Need to update the alarm time
        if (NV_OK == _tmrGetNextAlarmTime(pTmr, &nextAlarmTime))
        {
            _tmrScheduleCallbackInterrupt(pGpu, pTmr, nextAlarmTime);
        }
        else
        {
            // List is empty! Disable PTIMER interrupt.
            tmrRmCallbackIntrDisable(pTmr, pGpu);
        }
    }
    else
    {
        while (pChaser != NULL && pChaser->pNext != pEvent)
        {
            pChaser = pChaser->pNext;
        }
        if (pChaser == NULL)
        {
            // The callback wasn't currently scheduled, nothing to change.
            return;
        }
        pChaser->pNext = pEvent->pNext;
    }
}

/*!
 * Frees the memory used for maintaining a given callback in the timer.
 * Currently automatically calls cancel on the event.
 *
 * @param[in]   pEvent   The callback to cancel and free.
 */
void tmrEventDestroy_IMPL
(
    OBJTMR     *pTmr,
    TMR_EVENT  *pEventPublic
)
{
    TMR_EVENT_PVT *pEvent = (TMR_EVENT_PVT *)pEventPublic;

    if (pEvent != NULL)
    {
        NV_ASSERT(!pEvent->bLegacy);
        if (tmrIsOSTimer(pTmr, pEventPublic))
        {
            _tmrScanCallbackOSTimer(pTmr, pEvent);

            // OS timer destroying will cancel the timer
            tmrEventDestroyOSTimer_HAL(pTmr, pEventPublic);
        }
        else
        {
            tmrEventCancel(pTmr, pEventPublic);
        }
        portMemFree(pEvent);
    }
}

/*!
 * Returns time until next callback for a given event
 *
 * @param[in]   pEvent   The event whose remaining time needs to be determined.
 */
NV_STATUS
tmrEventTimeUntilNextCallback_IMPL
(
    OBJTMR     *pTmr,
    TMR_EVENT  *pEventPublic,
    NvU64      *pTimeUntilCallbackNs
)
{
    NvU64 currentTime;
    NvU64 nextAlarmTime;

    TMR_EVENT_PVT *pEvent = (TMR_EVENT_PVT*)pEventPublic;

    if (tmrIsOSTimer(pTmr, pEventPublic))
    {
        currentTime = osGetCurrentTick();
        // timens corresponds to relative time for OS timer
        NV_CHECK_OR_RETURN(LEVEL_ERROR, portSafeAddU64(pEvent->timens, pEvent->startTimeNs, &nextAlarmTime),
                           NV_ERR_INVALID_ARGUMENT);
    }
    else
    {
        NV_ASSERT_OK_OR_RETURN(tmrGetCurrentTime(pTmr, &currentTime));
        // timens corresponds to abs time in case of ptimer
        nextAlarmTime = pEvent->timens;
    }
    if (currentTime > nextAlarmTime)
        return NV_ERR_INVALID_STATE;

    *pTimeUntilCallbackNs = nextAlarmTime - currentTime;
    return NV_OK;
}


/*!
 * Callback invoked by NV0004_CTRL_CMD_TMR_SET_ALARM_NOTIFY
 */
static NV_STATUS
_nv0004CtrlCmdTmrSetAlarmNotifyCallback(OBJGPU *pGpu, OBJTMR *pTmr, TMR_EVENT *pTmrEvent)
{
    PEVENTNOTIFICATION pNotifyEvent = pTmrEvent->pUserData;
    NV_STATUS status = NV_OK;

    // perform a direct callback to the client
    if (NvP64_VALUE(pNotifyEvent->Data) != NULL)
    {
        //one shot signal
        status = osNotifyEvent(pGpu, pNotifyEvent, NV004_NOTIFIERS_SET_ALARM_NOTIFY, 0, NV_OK);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to notify event in callback, status: 0x%08x\n", status);
            return status;
        }
    }

    return status;
}

NV_STATUS
tmrapiCtrlCmdTmrSetAlarmNotify_IMPL
(
    TimerApi *pTimerApi,
    NV0004_CTRL_TMR_SET_ALARM_NOTIFY_PARAMS *pParams
)
{
    NV_STATUS status;
    OBJGPU *pGpu = GPU_RES_GET_GPU(pTimerApi);
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    TMR_EVENT *pTmrEvent = NULL;
    PEVENTNOTIFICATION pNotifyEvent = inotifyGetNotificationList(staticCast(pTimerApi, INotifier));

    // Validate the timer event
    while ((pNotifyEvent != NULL) && (pNotifyEvent->hEvent != pParams->hEvent))
    {
        pNotifyEvent = pNotifyEvent->Next;
    }

    if (pNotifyEvent == NULL)
    {
        NV_PRINTF(LEVEL_INFO, "timer event is missing\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Create an OS timer if not already created
    if (pNotifyEvent->pTmrEvent == NULL)
    {
        status = tmrEventCreate(pTmr,
                                &pTmrEvent,
                                _nv0004CtrlCmdTmrSetAlarmNotifyCallback,
                                (void*)pNotifyEvent,
                                0);
        if (status != NV_OK)
            return status;

        pNotifyEvent->pGpu = pGpu;
        pNotifyEvent->pTmrEvent = pTmrEvent;
    }
    else
    {
        pTmrEvent = pNotifyEvent->pTmrEvent;
    }

    // Schedule the timer
    status = tmrEventScheduleRel(pTmr, pTmrEvent, pParams->alarmTimeNsecs);
    if (status != NV_OK)
        return status;

    return NV_OK;
}

NV_STATUS tmrGetCurrentTimeEx_IMPL
(
    OBJTMR             *pTmr,
    NvU64              *pTime,
    THREAD_STATE_NODE  *pThreadState
)
{
    OBJGPU *pGpu = ENG_GET_GPU(pTmr);

    if (API_GPU_IN_RESET_SANITY_CHECK(pGpu))
    {
        *pTime = (NvU64)(~0);
        return NV_ERR_GPU_IN_FULLCHIP_RESET;
    }

    if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
    {
        *pTime = (NvU64)(~0);
        return NV_ERR_GPU_IS_LOST;
    }

    *pTime = tmrGetTimeEx_HAL(pGpu, pTmr, pThreadState);

    return NV_OK;
}

NV_STATUS tmrGetCurrentTime_IMPL
(
    OBJTMR             *pTmr,
    NvU64              *pTime
)
{
    return tmrGetCurrentTimeEx(pTmr, pTime, NULL);
}

/*!
 * TODO: document
 */
NV_STATUS tmrGetCurrentDiffTime_IMPL
(
    OBJTMR *pTmr,
    NvU64   startTime,
    NvU64  *pDiffTime
)
{
    NvU64 currentTime;
    NV_STATUS rmStatus;

    rmStatus = tmrGetCurrentTime(pTmr, &currentTime);

    *pDiffTime = currentTime - startTime;

    return rmStatus;
}

/*!
 *  Schedule a callback relative to current time specified in units of nanoseconds.
 *  Callbacks should be expected to be late however, this is not an RTOS, and a
 *  scheduling delay has been implemented to fix some race condition bugs.
 *  User has to provide a structure in memory for the timer to use.
 *
 *  @Note: For statically defined events it is recommended to preallocate them all
 *         at the appropriate stage in task life-cycle, and deallocated at the
 *         corresponding end of the life-cycle. For dynamically generated events
 *         consider the affects on fragmentation and potentially deferring deallocation.
 *
 *  @param[in]  pEvent     Callback memory structure, provided by user.
 *  @param[in]  RelTime    Number of nanoseconds from now to call Proc.
 *
 *  @returns               Status
 */
NV_STATUS tmrEventScheduleRel_IMPL
(
    OBJTMR     *pTmr,
    TMR_EVENT  *pEvent,
    NvU64       RelTime
)
{
    NvU64   AbsTime, currentTime;
    NV_STATUS rmStatus;

    if ((pEvent != NULL) && tmrIsOSTimer(pTmr, pEvent))
    {
        TMR_EVENT_PVT *pEventPvt = (TMR_EVENT_PVT *)pEvent;

        NV_CHECK_OK(rmStatus, LEVEL_ERROR,
                    tmrEventScheduleRelOSTimer_HAL(pTmr, pEvent, RelTime));
        //
        // Capture system time here, this will help in scheduling callbacks
        // if there is a state unload before receiving the OS timer callback.
        //
        pEventPvt->startTimeNs = osGetCurrentTick();
        if (!tmrEventOnList(pTmr, pEvent))
        {
            _tmrInsertCallback(pTmr, pEventPvt, RelTime);
        }
        return rmStatus;
    }

    rmStatus = tmrGetCurrentTime(pTmr, &currentTime);
    if (rmStatus != NV_OK)
        return rmStatus;

    NV_CHECK_OR_RETURN(LEVEL_ERROR, portSafeAddU64(currentTime, RelTime, &AbsTime), NV_ERR_INVALID_ARGUMENT);

    return tmrEventScheduleAbs(pTmr, pEvent, AbsTime);
}

/*!
 * Warning! This code is dangerous, it can cause the whole system to crash. It will be
 * removed as soon as possible! Use the new API!
 * It only remains for transitional purposes only.
 */
NV_STATUS tmrScheduleCallbackRel_IMPL
(
    OBJTMR             *pTmr,
    TIMEPROC_OBSOLETE   Proc,
    void               *Object,
    NvU64               RelTime,
    NvU32               Flags,
    NvU32               ChId
)
{
    NvU64   AbsTime, currentTime;
    NV_STATUS rmStatus;

    rmStatus = tmrGetCurrentTime(pTmr, &currentTime);
    if (rmStatus != NV_OK)
        return rmStatus;

    NV_CHECK_OR_RETURN(LEVEL_ERROR, portSafeAddU64(currentTime, RelTime, &AbsTime), NV_ERR_INVALID_ARGUMENT);

    return tmrScheduleCallbackAbs(pTmr, Proc, Object, AbsTime, Flags, ChId);
}

/*!
 * Warning! This code is dangerous, it can cause the whole system to crash. It will be
 * removed as soon as possible! Use the new API!
 * It only remains for transitional purposes only.
 */
NV_STATUS tmrScheduleCallbackRelSec_IMPL
(
    OBJTMR             *pTmr,
    TIMEPROC_OBSOLETE   Proc,
    void               *Object,
    NvU32               RelTimeSec,
    NvU32               Flags,
    NvU32               ChId
)
{
    NvU64 RelTimeNs;

    RelTimeNs = (NvU64)RelTimeSec * 1000000000;

    return tmrScheduleCallbackRel(pTmr, Proc, Object, RelTimeNs, Flags, ChId);
}

/*!
 * Determines if the Callback is actually scheduled currently.
 *
 *  @param[in]  pEvent      The event in question
 */
NvBool tmrEventOnList_IMPL
(
    OBJTMR     *pTmr,
    TMR_EVENT  *pEventPublic
)
{
    TMR_EVENT_PVT *pEvent = (TMR_EVENT_PVT *)pEventPublic;
    TMR_EVENT_PVT *pScan  = tmrIsOSTimer(pTmr, pEventPublic) ?
                            pTmr->pRmActiveOSTimerEventList :
                            pTmr->pRmActiveEventList;

    while (pScan != NULL)
    {
        if (pScan == pEvent)
        {
            NV_ASSERT(pEvent->bInUse);
            return NV_TRUE;
        }
        pScan = pScan->pNext;
    }
    return NV_FALSE;
}

/*!
 * Warning! This code is dangerous, it can cause the whole system to crash. It will be
 * removed as soon as possible! Use the new API!
 * It only remains for transitional purposes only.
 */
NvBool tmrCallbackOnList_IMPL
(
    OBJTMR             *pTmr,
    TIMEPROC_OBSOLETE   Proc,
    void               *Object
)
{
    NvBool onList = NV_FALSE;
    TMR_EVENT_PVT *tmrScan;
    TMR_EVENT_PVT *tmrList;

    tmrList = pTmr->pRmActiveEventList;

    for (tmrScan = tmrList; tmrScan; tmrScan = tmrScan->pNext)
    {
        if ((Proc == tmrScan->pTimeProc_OBSOLETE) &&
            (Object == tmrScan->super.pUserData))
        {
            onList = NV_TRUE;
            break;
        }
    }

    return onList;
}

/*!
 * OBSOLETE: This will be removed very soon!
 */
static TMR_EVENT_PVT *
_tmrGetNextFreeCallback
(
    OBJTMR *pTmr
)
{
    TMR_EVENT_PVT *pEvent = NULL;

    pEvent = pTmr->pRmCallbackFreeList_OBSOLETE;
    if (pEvent != NULL)
    {
        NV_ASSERT(pEvent->bLegacy); // OBSOLETE, remove later
        pTmr->pRmCallbackFreeList_OBSOLETE = pEvent->pNext;
        // just to be sure.
        pEvent->pNext = NULL;
    }

    return pEvent;
}

/*!
 * Creates and inserts a node into the callback list.
 *
 *  @param[in]  pEvent     Callback memory structure, provided by user.
 *  @param[in]  Time       Absolute(for ptimer) or relative (for OS timer) nanoseconds at which to call Proc.
 *
 *  @returns               Status
 */
static NV_STATUS
_tmrInsertCallback
(
    OBJTMR         *pTmr,
    TMR_EVENT_PVT *pEvent,
    NvU64           Time
)
{
    NV_STATUS      returnStatus = NV_ERR_GENERIC; // Indicate that the timer was NOT inserted in the list
    OBJGPU *pGpu = ENG_GET_GPU(pTmr);

    // If this is a free callback
    if (!pEvent->bInUse && !tmrEventOnList(pTmr, (TMR_EVENT *)pEvent))
    {
        pEvent->timens    = Time;

        _tmrInsertCallbackInList(pGpu, pTmr, pEvent);

        returnStatus = NV_OK;
    }
    else
    {
        // Shouldn't get here. Don't call this function unless valid
        NV_ASSERT_OR_RETURN(!"Invalid call to insert, already in use", NV_ERR_INVALID_ARGUMENT);
    }

    return returnStatus;
}

/*!
 * Insert (time sorted) a specific event into the callback queue in case
 * of GPU Timer. Insert a specific event on the top of callback queue in
 * case of OS timer.
 *
 * Handles setting the next alarm time as well as enabling alarm if needed
 *
 *  @param[in]  pEvent      The event to be inserted, must be initialized
 */
static void
_tmrInsertCallbackInList
(
    OBJGPU         *pGpu,
    OBJTMR         *pTmr,
    TMR_EVENT_PVT *pEvent
)
{
    TMR_EVENT_PVT   *pScan;
    NvBool           bAddedAsHead  = NV_TRUE;
    NvU64            nextAlarmTime;

    NV_ASSERT(!pEvent->bInUse);
    pEvent->bInUse = NV_TRUE;

    if (tmrIsOSTimer(pTmr, (TMR_EVENT *)pEvent))
    {
        pEvent->pNext = pTmr->pRmActiveOSTimerEventList;
        pTmr->pRmActiveOSTimerEventList = pEvent;
        return;
    }

    if (pTmr->pRmActiveEventList == NULL)
    {
        // Enable PTIMER interrupt.
        tmrRmCallbackIntrEnable(pTmr, pGpu);

        // insert pEvent as first and only entry.
        pEvent->pNext = NULL;
        pTmr->pRmActiveEventList = pEvent;
    }
    else if (pEvent->timens <= pTmr->pRmActiveEventList->timens)
    {
        // insert pEvent as head entry of the non-empty callback list.
        pEvent->pNext = pTmr->pRmActiveEventList;
        pTmr->pRmActiveEventList = pEvent;
    }
    else
    {
        bAddedAsHead = NV_FALSE;

        pScan = pTmr->pRmActiveEventList;

        while (pScan->pNext != NULL)
        {
            if (pEvent->timens <= pScan->pNext->timens)
            {
                // insert into the middle of the list.
                pEvent->pNext = pScan->pNext;
                pScan->pNext  = pEvent;

                break;
            }
            pScan = pScan->pNext;
        }

        if (pScan->pNext == NULL)
        {
            // insert it at the end of the list.
            pEvent->pNext = NULL;
            pScan->pNext = pEvent;
        }
    }

    if (bAddedAsHead)
    {
        // Find out when the next alarm should be.
        if (NV_OK != _tmrGetNextAlarmTime(pTmr, &nextAlarmTime))
        {
            // if there is no event list, then just use 0.
            nextAlarmTime = 0;
        }

        _tmrScheduleCallbackInterrupt(pGpu, pTmr, nextAlarmTime);
    }
}

/*!
 *  Schedule a callback at the absolute time specified in units of nanoseconds.
 *
 *  Account for bad scheduling times, if the time too close in the future push
 *  it back till a short delay later. This avoids some race conditions. Even though
 *  callbacks may be delayed. However callbacks will not happen early.
 *
 *  @Note: For statically defined events it is recommended to preallocate them all
 *         at the appropriate stage in task life-cycle, and deallocated at the
 *         corresponding end of the life-cycle. For dynamically generated events
 *         consider the affects on fragmentation and potentially deferring deallocation.
 *
 *  @param[in]  pEvent     Callback memory structure, provided by user.
 *  @param[in]  Time       Absolute nanoseconds at which to call Proc.
 *
 *  @returns               Status
 */
NV_STATUS tmrEventScheduleAbs_IMPL
(
    OBJTMR     *pTmr,
    TMR_EVENT  *pEventPublic,
    NvU64       timeAbsNs
)
{
    NV_STATUS      rmStatus = NV_OK;
    TMR_EVENT_PVT *pEvent = (TMR_EVENT_PVT *)pEventPublic;

    if ((pEvent != NULL) && tmrIsOSTimer(pTmr, pEventPublic))
    {
        NvU64 timeRelNs, currentTime;

        rmStatus = tmrGetCurrentTime(pTmr, &currentTime);
        if (rmStatus != NV_OK)
            return rmStatus;

        //
        // If absolute time is less than current time, then timer has already
        // expired. For this case, schedule timer with zero delay to trigger
        // immediate callback.
        //
        timeRelNs = (timeAbsNs > currentTime) ? (timeAbsNs - currentTime) : 0;
        return tmrEventScheduleRel(pTmr, pEventPublic, timeRelNs);
    }

    if ((pEvent == NULL) || (pEventPublic->pTimeProc == NULL &&
                             pEvent->pTimeProc_OBSOLETE == NULL))
    {
        //
        // Bug 372159: Not sure exactly how this is happening, but we are seeing
        // it in OCA.  If you see this during development/testing, please update
        // the bug.
        //
        NV_ASSERT_FAILED(
            "Attempting to schedule callback with NULL procedure.  "
            "Please update Bug 372159 with appropriate information.");
        rmStatus = NV_ERR_INVALID_ARGUMENT;
    }
    else
    {
        //
        // Insert this proc into the callback list.
        //
        // if (Time <= CurrentTime + SCHEDULING_DELAY_MIN):
        //
        // We used to return NV_ERR_CALLBACK_NOT_SCHEDULED here.
        // The next fix called the callback immediately in order to simulate
        // it being "scheduled", however this introduced nasty stack-overflow
        // due self rescheduling tasks.
        //
        // CL 16512758 fixed the stack-overflow issue, and added a case for
        // handling callbacks scheduled to occur within 250 ns.  Later we found
        // out that a 1 us callback could cause the alarm to be set to the past
        // and cause a 4+ second delay due to wrap-around.  To fix this, we
        // removed the 250 ns threshold, so that we will always re-read the
        // current time after setting the alarm to prevent the wrap-around.
        //
        rmStatus = _tmrInsertCallback(pTmr, pEvent, timeAbsNs);
    }

    return rmStatus;
}

/*!
 * Warning! This code is dangerous, it can cause the whole system to crash. It will be
 * removed as soon as possible! Use the new API!
 */
NV_STATUS tmrScheduleCallbackAbs_IMPL
(
    OBJTMR             *pTmr,
    TIMEPROC_OBSOLETE   Proc,
    void               *Object,
    NvU64               Time,
    NvU32               Flags,
    NvU32               ChId
)
{
    TMR_EVENT_PVT *tmrInsert;
    // Get a free callback from the free list.
    if(pTmr->pRmCallbackFreeList_OBSOLETE == NULL)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    if (!tmrCallbackOnList(pTmr, Proc, Object))
    {
        tmrInsert = _tmrGetNextFreeCallback(pTmr);
        if (tmrInsert){
            tmrInsert->pTimeProc_OBSOLETE = Proc;
            tmrInsert->super.pUserData = Object;
            tmrInsert->super.flags = Flags;
            tmrInsert->super.chId = ChId;

            return tmrEventScheduleAbs(pTmr, (TMR_EVENT *)tmrInsert, Time);
        }
        else
        {
            return NV_ERR_INSUFFICIENT_RESOURCES;
        }
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "Proc %p Object %p already on tmrList\n", Proc,
                  Object);
    }
    return NV_OK;
}

/*!
 *  Searches specified lists for TMR_EVENT* associated with Object and
 *  removes it.
 *
 *  @param[in]  Object  Unique identifier based on TMR_POBJECT_BASE (tmr.h)
 *
 *  @returns None
 */
static void _tmrScanCallback
(
    OBJTMR *pTmr,
    void   *pObject
)
{
    TMR_EVENT_PVT *tmrScan;
    TMR_EVENT_PVT *tmrNext;
    TMR_EVENT_PVT *tmrCurrent;

    //
    // Start at the beginning of the callback list.
    //
    // 'current' is either the same as 'scan' or
    // it's the item immediately before 'scan' in
    // the algorithm below.
    //
    tmrScan = tmrCurrent = pTmr->pRmActiveEventList;

    //
    // Loop through the callback list while there are entries.
    //
    while (tmrScan)
    {
        // Point to the next callback so that we
        // can continue our scan through the list.
        tmrNext = tmrScan->pNext;

        //
        // Scan list looking for matches to 'Object'.
        //
        if (tmrScan->super.pUserData == pObject)
        {
            //
            // If the 'current' is not the item to be deleted
            // (It must be the previous item) then link it
            // to the 'next' item
            //
            if (tmrCurrent != tmrScan)
            {
                tmrCurrent->pNext = tmrScan->pNext;
            }
            else
            {
                //
                // If 'current' is the same as the item to be deleted
                // then move it to the next item.
                //
                tmrCurrent = tmrNext;

                //
                // Update the head pointer if removing the head entry.
                // This fixes bug 93812.
                //
                if (pTmr->pRmActiveEventList == tmrScan)
                {
                    pTmr->pRmActiveEventList = tmrScan->pNext;
                }
            }

            if (tmrScan->bLegacy)
            {
                //
                // Tack the object to be deleted onto the head of the
                // callback free list (OBSOLETE)
                //
                tmrScan->pNext = pTmr->pRmCallbackFreeList_OBSOLETE;
                pTmr->pRmCallbackFreeList_OBSOLETE = tmrScan;
            }

            tmrScan->bInUse = NV_FALSE;
        }
        else
        {
            //
            // If we haven't deleted this item, then the 'current'
            // item becomes this item. So 'scan' will advance ONE beyond
            // the item that was NOT deleted, and 'current' becomes
            // the item NOT deleted.
            //
            tmrCurrent = tmrScan;
        }

        // Now point to the 'next' object in the callback list.
        tmrScan = tmrNext;
    }
}

/*!
 * This function removes the timer callback from the OS timer list
 * after it is serviced, cancelled, or destroyed.
 */
static void
_tmrScanCallbackOSTimer
(
    OBJTMR *pTmr,
    TMR_EVENT_PVT *pEvent
)
{
    TMR_EVENT_PVT *pCurrent = pTmr->pRmActiveOSTimerEventList;

    if (pCurrent == pEvent)
    {
        pTmr->pRmActiveOSTimerEventList = pCurrent->pNext;
        pEvent->pNext = NULL;
        pEvent->bInUse = NV_FALSE;
        return;
    }

    while (pCurrent != NULL)
    {
        if (pCurrent->pNext == pEvent)
        {
            pCurrent->pNext = pEvent->pNext;
            pEvent->pNext = NULL;
            pEvent->bInUse = NV_FALSE;
            break;
        }
        pCurrent = pCurrent->pNext;
    }
}

// determine which (if any) callback should determine the next alarm time
static NV_STATUS
_tmrGetNextAlarmTime
(
    OBJTMR *pTmr,
    NvU64  *pNextAlarmTime
)
{
    if (pTmr->pRmActiveEventList == NULL)
    {
        *pNextAlarmTime = 0;
        return NV_ERR_CALLBACK_NOT_SCHEDULED;
    }

    *pNextAlarmTime = pTmr->pRmActiveEventList->timens;

    return NV_OK;
}

/*!
 * Return the very next callback to be scheduled, removing it from the list
 * and marking it as free (only "In Use" when in the list)
 */
static TMR_EVENT_PVT * _tmrPullCallbackFromHead
(
    OBJTMR         *pTmr
)
{
    TMR_EVENT_PVT *tmrDelete = pTmr->pRmActiveEventList;
    if (tmrDelete)
    {
        // remove from callbackList
        pTmr->pRmActiveEventList = tmrDelete->pNext;
        tmrDelete->bInUse = NV_FALSE;

        if(tmrDelete->bLegacy)
        {
            // Might be a race condition, but will be removed so it's OK
            tmrDelete->pNext                    = pTmr->pRmCallbackFreeList_OBSOLETE;
            pTmr->pRmCallbackFreeList_OBSOLETE  = tmrDelete;
        }
    }

    return tmrDelete;
}

/*!
 * Time until next callback expires.
 *
 * Returns NV_ERR_CALLBACK_NOT_SCHEDULED if not callbacks are scheduled.
 */
NV_STATUS
tmrTimeUntilNextCallback_IMPL
(
    OBJGPU *pGpu,
    OBJTMR *pTmr,
    NvU64 *pTimeUntilCallbackNs
)
{
    NvU64           currentTime;
    NvU64           nextAlarmTime;
    NV_STATUS       status;

    *pTimeUntilCallbackNs = 0;

    // Get the time from the first (earliest) entry.
    status = _tmrGetNextAlarmTime(pTmr, &nextAlarmTime);
    if (status != NV_OK)
        return status;

    status = tmrGetCurrentTime(pTmr, &currentTime);
    if (status != NV_OK)
        return status;

    if (currentTime < nextAlarmTime)
        *pTimeUntilCallbackNs = nextAlarmTime - currentTime;

    return NV_OK;
}

/*!
 * Used by tmrService, iteratively checks which callbacks need to be executed.
 */
NvBool
tmrCallExpiredCallbacks_IMPL
(
    OBJGPU *pGpu,
    OBJTMR *pTmr
)
{
    NvU64           currentTime = 0;
    NvU64           nextAlarmTime;
    TMR_EVENT_PVT  *pEvent;
    NV_STATUS       rmStatus;
    NvBool          bProccessedCallback = NV_FALSE;

    // Call all callbacks that have expired
    if (pTmr && (tmrEventsExist(pTmr)))
    {
        // Check for expired time.
        for (;;)
        {
            // Get the time from the first (earliest) entry.
            rmStatus = _tmrGetNextAlarmTime(pTmr, &nextAlarmTime);
            if (rmStatus != NV_OK)
                break;

            if (nextAlarmTime > currentTime)
            {
                rmStatus = tmrGetCurrentTime(pTmr, &currentTime);
                if ((rmStatus != NV_OK) || (nextAlarmTime > currentTime))
                    break;
            }

            // Pull from head of list.
            pEvent = _tmrPullCallbackFromHead(pTmr);

            if (pEvent != NULL)
            {
                // Call callback.  This could insert a new callback into the list.
                if (pEvent->bLegacy && pEvent->pTimeProc_OBSOLETE != NULL)
                {
                    pEvent->pTimeProc_OBSOLETE(pGpu, pTmr, pEvent->super.pUserData);
                    bProccessedCallback = NV_TRUE;
                }
                else if (!pEvent->bLegacy && pEvent->super.pTimeProc != NULL)
                {
                    pEvent->super.pTimeProc(pGpu, pTmr, (TMR_EVENT *)pEvent);
                    bProccessedCallback = NV_TRUE;
                }
                else
                {
                    NV_ASSERT_FAILED("Attempting to execute callback with NULL procedure.");
                }
            }
            else
            {
                NV_ASSERT_FAILED("Attempting to execute callback with NULL timer event.");
            }
        }

        //
        // rmStatus is NV_OK only when there are more events in the list AND
        // the GPU has not fallen off the bus AND the GPU is not in full chip
        // reset.
        //
        // We get this this routine with bInterrupt set to true when we got
        // (and cleared) the timer interrupt.  So, we need to set it again.
        //
        if (rmStatus == NV_OK)
        {
            _tmrScheduleCallbackInterrupt(pGpu, pTmr, nextAlarmTime);
        }
    }

    return bProccessedCallback;
}

/*!
 * TODO: document
 */
static void
_tmrStateLoadCallbacks
(
    OBJGPU *pGpu,
    OBJTMR *pTmr
)
{
    NvU64 nextAlarmTime = 0;
    TMR_EVENT_PVT *pScan = pTmr->pRmActiveOSTimerEventList;

    if (tmrEventsExist(pTmr))
    {
        if (tmrGetCallbackInterruptPending(pGpu, pTmr))
        {
            if (NV_OK == _tmrGetNextAlarmTime(pTmr, &nextAlarmTime))
            {
                _tmrScheduleCallbackInterrupt(pGpu, pTmr, nextAlarmTime);
            }
        }

        //
        // else - we have alarm pending - just proceed to enable interrupts
        // so that it's immediately handled
        //
        if (pTmr->getProperty(pTmr, PDB_PROP_TMR_USE_COUNTDOWN_TIMER_FOR_RM_CALLBACKS))
        {
            tmrSetCountdownIntrEnable_HAL(pGpu, pTmr);
        }
        else
        {
            tmrSetAlarmIntrEnable_HAL(pGpu, pTmr);
        }
    }

    // Schedule the timer callbacks which were paused during state unload
    while (pScan != NULL)
    {
        //
        // Capture system time here, this will help in scheduling callbacks
        // if there is a state unload before receiving the OS timer callback.
        //
        pScan->startTimeNs = osGetCurrentTick();
        tmrEventScheduleRelOSTimer_HAL(pTmr, (TMR_EVENT *)pScan, pScan->timens);
        pScan = pScan->pNext;
    }
}

/*!
 * Wraps HAL functions to enable hardware timer interrupts for the rm callbacks.
 */
void
tmrRmCallbackIntrEnable_IMPL
(
    OBJTMR *pTmr,
    OBJGPU *pGpu
)
{
    tmrResetCallbackInterrupt(pGpu, pTmr);

    if (pTmr->getProperty(pTmr, PDB_PROP_TMR_USE_COUNTDOWN_TIMER_FOR_RM_CALLBACKS))
    {
        tmrSetCountdownIntrEnable_HAL(pGpu, pTmr);
    }
    else
    {
        tmrSetAlarmIntrEnable_HAL(pGpu, pTmr);
    }
}

/*!
 * Wraps HAL functions to disable hardware timer interrupts for the rm callbacks.
 */
void
tmrRmCallbackIntrDisable_IMPL
(
    OBJTMR *pTmr,
    OBJGPU *pGpu
)
{
    if (pTmr->getProperty(pTmr, PDB_PROP_TMR_USE_COUNTDOWN_TIMER_FOR_RM_CALLBACKS))
    {
        tmrSetCountdownIntrDisable_HAL(pGpu, pTmr);
    }
    else
    {
        tmrSetAlarmIntrDisable_HAL(pGpu, pTmr);
    }
}

void
tmrSetCountdownCallback_IMPL
(
    OBJTMR            *pTmr,
    TIMEPROC_COUNTDOWN pSwrlCallback
)
{
    pTmr->pSwrlCallback = pSwrlCallback;
}

/*!
 * TODO: document
 */
void
tmrGetSystemTime_IMPL
(
    OBJTMR         *pTmr,
    PDAYMSECTIME    pTime
)
{
    NvU32   sec;
    NvU32   usec;

    //
    // This function finds out the current time in terms of number of days and
    // milliseconds since 1900. Note that the estimates are really crude since
    // 1 year is treated as 365 days, 1 month as 30 days and so on. Keep these
    // points in mind before using the function.
    //
    if (pTime != NULL)
    {
        // Get the system time and calculate the contents of the returned structure.
        osGetCurrentTime(&sec, &usec);
        pTime->days = sec / (3600 * 24);            // # of days since ref point
        sec = sec % (3600 * 24);                    // seconds since day began
        pTime->msecs = sec * 1000 + (usec / 1000);  // milliseconds since day began
    }
}

/*!
 * This has become obsolete, it should be replaced with userData logic
 */
NvBool
tmrCheckCallbacksReleaseSem_IMPL
(
    OBJTMR *pTmr,
    NvU32   chId
)
{
    TMR_EVENT_PVT *pScan;

    for (pScan = pTmr->pRmActiveEventList; pScan != NULL; pScan = pScan->pNext)
    {
        if ((pScan->super.flags & TMR_FLAG_RELEASE_SEMAPHORE) &&
            (pScan->super.chId == chId))
        {
            break;
        }
    }

    return pScan != NULL;
}

/*!
 * TODO: document
 */
void
tmrInitCallbacks_IMPL
(
    OBJTMR *pTmr
)
{
    NvU32   i;

    // Initialize the timer callback lists.
    pTmr->pRmActiveEventList = NULL;
    pTmr->pRmActiveOSTimerEventList = NULL;

    // Everything below this comment will be removed with new API
    pTmr->pRmCallbackFreeList_OBSOLETE = pTmr->rmCallbackTable_OBSOLETE;

    // Fill in all the forward pointers in the callback table.
    for (i = 0; i < (TMR_NUM_CALLBACKS_RM - 1); i++)
    {
        pTmr->rmCallbackTable_OBSOLETE[i].pNext = &pTmr->rmCallbackTable_OBSOLETE[i+1];
        pTmr->rmCallbackTable_OBSOLETE[i].bInUse = NV_FALSE;
        pTmr->rmCallbackTable_OBSOLETE[i].bLegacy = NV_TRUE;
    }
    pTmr->rmCallbackTable_OBSOLETE[i].pNext = NULL;
    pTmr->rmCallbackTable_OBSOLETE[i].bInUse = NV_FALSE;
    pTmr->rmCallbackTable_OBSOLETE[i].bLegacy = NV_TRUE;
}

/*!
 * Searches for all events associated with an Object and removes them.
 *
 * @param[in,out]   pTmr    TMR object pointer
 * @param[in]       pObject Unique identifier based on TMR_POBJECT_BASE (tmr.h)
 *
 * @returns NV_OK   always succeeds
 */
NV_STATUS
tmrCancelCallback_IMPL
(
    OBJTMR *pTmr,
    void   *pObject
)
{
    OBJGPU *pGpu = ENG_GET_GPU(pTmr);
    NvU64   nextAlarmTime;

    if (tmrEventsExist(pTmr) && pObject != NULL)
    {
        // Pull all objects with the same address from the callback list.
        _tmrScanCallback(pTmr, pObject);

        //
        // If there's anything left then set an alarm for the soonest one.
        // Otherwise, disable the PTIMER interrupt altogether.
        //
        if (NV_OK == _tmrGetNextAlarmTime(pTmr, &nextAlarmTime))
        {
            _tmrScheduleCallbackInterrupt(pGpu, pTmr, nextAlarmTime);
        }
        else
        {
            // List is empty! Disable PTIMER interrupt.
            tmrRmCallbackIntrDisable(pTmr, pGpu);
        }
    }

    return NV_OK;
}

/*!
 * TODO: document
 *
 * This function finds out if the (futureTime - pastTime) > maxCacheTimeInMSec
 */
NvBool
tmrDiffExceedsTime_IMPL
(
    OBJTMR         *pTmr,
    PDAYMSECTIME    pFutureTime,
    PDAYMSECTIME    pPastTime,
    NvU32           time
)
{
    NvU32   msecsInADay = 1000 * 3600 * 24;
    NvBool  bRetVal     = NV_FALSE;

    if ((pFutureTime->days < pPastTime->days) ||
        (((pFutureTime->days == pPastTime->days) &&
          (pFutureTime->msecs < pPastTime->msecs))))
    {
        bRetVal = NV_TRUE;
    }
    else
    {
        // Because of overflow possibility, first check for diff in days
        if ((((pFutureTime->days - pPastTime->days) +
              (pFutureTime->msecs - pPastTime->msecs)/msecsInADay)) > (time/msecsInADay))
        {
            bRetVal = NV_TRUE;
        }
        else
        {
            // Now diff in millisecs
            if ((((pFutureTime->days - pPastTime->days) * msecsInADay) +
                 (pFutureTime->msecs - pPastTime->msecs)) > time)
            {
                bRetVal = NV_TRUE;
            }
        }
    }

    return bRetVal;
}

NV_STATUS
tmrStatePreInitLocked_IMPL
(
    OBJGPU *pGpu,
    OBJTMR *pTmr
)
{

    return NV_OK;
}

NV_STATUS
tmrStateInitLocked_IMPL
(
    OBJGPU *pGpu,
    OBJTMR *pTmr
)
{

    return NV_OK;
}

/*!
 * TODO: document
 */
NV_STATUS
tmrStateLoad_IMPL
(
    OBJGPU *pGpu,
    OBJTMR *pTmr,
    NvU32   flags
)
{
    // Have to restore any pending callbacks' state
    _tmrStateLoadCallbacks(pGpu, pTmr);

    return NV_OK;
}

/*!
 * TODO: document
 */
NV_STATUS
tmrStateUnload_IMPL
(
    OBJGPU *pGpu,
    OBJTMR *pTmr,
    NvU32   flags
)
{
    TMR_EVENT_PVT *pScan = pTmr->pRmActiveOSTimerEventList;
    NvU64 currentSysTime, elapsedTime;

    // Disable Timer interrupt.
    tmrSetAlarmIntrDisable_HAL(pGpu, pTmr);
    tmrSetCountdownIntrDisable_HAL(pGpu, pTmr);

    //
    // Cancel OS timers, and save the time remaining for callback
    // in 'timens', to reschedule after state load.
    //
    while (pScan != NULL)
    {
        currentSysTime = osGetCurrentTick();
        //
        // If somehow any of the time difference is negative,
        // we will use the  original time duration.
        //
        if (currentSysTime >= pScan->startTimeNs)
        {
            elapsedTime = currentSysTime - pScan->startTimeNs;
            if (pScan->timens > elapsedTime)
            {
                pScan->timens -= elapsedTime;
            }
        }

        tmrEventCancelOSTimer_HAL(pTmr, (TMR_EVENT *)pScan);
        pScan = pScan->pNext;
    }
    return NV_OK;
}

/*!
 * TODO: document
 */
void
tmrStateDestroy_IMPL
(
    OBJGPU *pGpu,
    OBJTMR *pTmr
)
{
    objDelete(pTmr->pGrTickFreqRefcnt);
    pTmr->pGrTickFreqRefcnt = NULL;
}

NV_STATUS
tmrapiConstruct_IMPL
(
    TimerApi *pTimerApi,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    return NV_OK;
}

void
tmrapiDestruct_IMPL
(
    TimerApi *pTimerApi
)
{
}

NV_STATUS
tmrapiGetRegBaseOffsetAndSize_IMPL
(
    TimerApi *pTimerApi,
    OBJGPU *pGpu,
    NvU32 *pOffset,
    NvU32 *pSize
)
{
    NV_STATUS status;
    NvU32 offset;

    status = gpuGetRegBaseOffset_HAL(GPU_RES_GET_GPU(pTimerApi), NV_REG_BASE_TIMER, &offset);
    if (status != NV_OK)
        return status;

    if (pOffset)
        *pOffset = offset;

    if (pSize)
        *pSize = sizeof(Nv01TimerMap);

    return NV_OK;
}

void
tmrapiDeregisterEvents_IMPL(TimerApi *pTimerApi)
{
    OBJGPU            *pGpu = GPU_RES_GET_GPU(pTimerApi);
    OBJTMR            *pTmr = GPU_GET_TIMER(pGpu);
    PEVENTNOTIFICATION pNotifyEvent = inotifyGetNotificationList(staticCast(pTimerApi, INotifier));

    // Validate the timer event
    while (pNotifyEvent != NULL)
    {
        //
        // TimerApi events are only set through NV0004_CTRL_CMD_TMR_SET_ALARM_NOTIFY
        // which only schedules TMR_EVENT type callbacks. So only call the new API.
        //
        TMR_EVENT *pTmrEvent = pNotifyEvent->pTmrEvent;
        if (pTmrEvent != NULL)
        {
            tmrEventDestroy(pTmr, pTmrEvent);
            pNotifyEvent->pGpu = NULL;
            pNotifyEvent->pTmrEvent = NULL;
        }

        pNotifyEvent = pNotifyEvent->Next;
    }
}

//---------------------------------------------------------------------------
//
//  NV0004 Control Functions
//
//---------------------------------------------------------------------------

//
// There is some type hacking involved here. The inner callback is called correctly here
// though it is cast to the outer callback type for storage. The timer only sees the
// outer callback type directly so it will call it correctly, and this wrapper hides the
// inner callback and calls it correctly from itself. Hacky but it should work around the
// limitations in the SDK (all RM derived types undefined, so TIMEPROC type is impossible).
//
typedef NvU32 (*TMR_CALLBACK_FUNCTION)(void *pCallbackData);

typedef struct
{
    TMR_CALLBACK_FUNCTION       pTimeProc;
    void                       *pCallbackData;
} wrapperStorage_t;

static NV_STATUS _tmrCallbackWrapperfunction
(
    OBJGPU *pGpu,
    OBJTMR *pTmr,
    TMR_EVENT *pEvent
)
{
    wrapperStorage_t *pObj_Inner = (wrapperStorage_t *)pEvent->pUserData;

    // Backup the wrapper function and data
    TIMEPROC pCallback_Outer    = pEvent->pTimeProc;
    void *pCallbackData_Outer   = pEvent->pUserData;

    // Swap in the inner function and data
    pEvent->pTimeProc = (TIMEPROC) pObj_Inner->pTimeProc; // Intentionally the wrong type!
    pEvent->pUserData = pObj_Inner->pCallbackData;

    // Perform the actual callback the way the user expects it
    pObj_Inner->pTimeProc((void *)pEvent->pUserData);

    // Rewrap whatever changes the user may have made
    pObj_Inner->pTimeProc     = (TMR_CALLBACK_FUNCTION) pEvent->pTimeProc;
    pObj_Inner->pCallbackData = pEvent->pUserData;

    // Restore the wrapper function and data
    pEvent->pTimeProc = pCallback_Outer;
    pEvent->pUserData = pCallbackData_Outer;

    return NV_OK;
}

/*!
 * Creates an event and initializes the wrapper callback data, putting the
 * desired callback inside of it's struct to be swapped in later.
 *
 * @returns NV_STATUS
 */
NV_STATUS
tmrCtrlCmdEventCreate
(
    OBJGPU *pGpu,
    TMR_EVENT_SET_PARAMS *pParams
)
{
    NV_STATUS         rc;
    TMR_EVENT        *pEvent;
    wrapperStorage_t *pWrapper;
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);

    // ALlocate the wrapper's callerdata to store real caller data!
    pWrapper = portMemAllocNonPaged(sizeof(wrapperStorage_t));
    if (pWrapper == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }
    pWrapper->pTimeProc     = (TMR_CALLBACK_FUNCTION)NvP64_VALUE(pParams->pTimeProc);
    pWrapper->pCallbackData = NvP64_VALUE(pParams->pCallbackData);

    rc = tmrEventCreate(pTmr,
                        &pEvent,
                        _tmrCallbackWrapperfunction,
                        pWrapper,
                        pParams->flags);

    *(pParams->ppEvent) = NV_PTR_TO_NvP64(pEvent);

    return rc;
}


/*!
 * Schedules an existing event. Takes in time arguments and a flag to
 * determine if it should be interpreted as absolute or relative time.
 *
 * @returns NV_STATUS
 */
NV_STATUS
tmrCtrlCmdEventSchedule
(
    OBJGPU *pGpu,
    TMR_EVENT_SCHEDULE_PARAMS *pParams
)
{
    NV_STATUS rc;
    OBJTMR    *pTmr   = GPU_GET_TIMER(pGpu);
    TMR_EVENT *pEvent = (TMR_EVENT *)NvP64_VALUE(pParams->pEvent);

    if (pParams->bUseTimeAbs)
    {
        rc = tmrEventScheduleAbs(pTmr, pEvent, pParams->timeNs);
    }
    else
    {
        rc = tmrEventScheduleRel(pTmr, pEvent, pParams->timeNs);
    }

    return rc;
}

/*!
 * Cancels an existing event. NOP on unscheduled event.
 *
 * @returns NV_OK
 */
NV_STATUS
tmrCtrlCmdEventCancel
(
    OBJGPU *pGpu,
    TMR_EVENT_GENERAL_PARAMS *pParams
)
{
    OBJTMR    *pTmr   = GPU_GET_TIMER(pGpu);
    TMR_EVENT *pEvent = (TMR_EVENT *)NvP64_VALUE(pParams->pEvent);
    tmrEventCancel(pTmr, pEvent);

    return NV_OK;
}

/*!
 * Cancel and destroys an existing event. It also cleans up the special
 * wrapper memory used by this API framework.
 *
 * @returns NV_OK
 */
NV_STATUS
tmrCtrlCmdEventDestroy
(
    OBJGPU *pGpu,
    TMR_EVENT_GENERAL_PARAMS *pParams
)
{
    OBJTMR    *pTmr   = GPU_GET_TIMER(pGpu);
    TMR_EVENT *pEvent = (TMR_EVENT *)NvP64_VALUE(pParams->pEvent);

    // Free our temporary wrapper storage
    portMemFree(pEvent->pUserData);

    tmrEventDestroy(pTmr, pEvent);

    return NV_OK;
}

NV_STATUS tmrEventServiceTimer_IMPL
(
    OBJGPU             *pGpu,
    OBJTMR             *pTmr,
    TMR_EVENT          *pPublicEvent
)
{
    TMR_EVENT_PVT *pEvent = (TMR_EVENT_PVT *)pPublicEvent;
    NV_STATUS status      = NV_ERR_INVALID_REQUEST;

    if ((pEvent == NULL) || !tmrIsOSTimer(pTmr, (TMR_EVENT *)pEvent))
    {
        return status;
    }

    if (tmrEventOnList(pTmr, (TMR_EVENT *)pEvent))
    {
        _tmrScanCallbackOSTimer(pTmr, pEvent);
        status = tmrEventServiceOSTimerCallback_HAL(pGpu, pTmr, pPublicEvent);
    }

    return status;
}

/**
 * @brief Provides an opportunity to register some IntrService during intrStateInit.
 *
 * @param[in] pGpu
 * @param[in] pTmr The IntrService object discovered as a GPU child;
 *                         not necessarily the one to be registered.
 * @param[in] pRecords A table of MC_ENGINE_IDX_MAX IntrServiceRecord, to be updated directly.
 */
void
tmrRegisterIntrService_IMPL(OBJGPU *pGpu, OBJTMR *pTmr, IntrServiceRecord pRecords[MC_ENGINE_IDX_MAX])
{
    NV_ASSERT(pRecords[MC_ENGINE_IDX_TMR].pInterruptService == NULL);
    pRecords[MC_ENGINE_IDX_TMR].pInterruptService = staticCast(pTmr, IntrService);

    // NB. There's no harm in registering ourselves for _SWRL even when it's not in the intr table.
    NV_ASSERT(pRecords[MC_ENGINE_IDX_TMR_SWRL].pInterruptService == NULL);
    pRecords[MC_ENGINE_IDX_TMR_SWRL].pInterruptService = staticCast(pTmr, IntrService);
}

/**
 * @brief Clears the stall interrupt leaf vector and return whether to call ServiceStall.
 * @details Normally there's no need to override this function as its default is used by almost all handlers,
 *          but MC_ENGINE_IDX_TMR_SWRL requires a specific sequence to acquire as a semaphore.
 *
 * @param[in] pGpu
 * @param[in] pTmr The IntrService object registered to handle the engineIdx stall interrupt.
 * @param[in] pParams
 *
 * @returns A boolean which is NV_FALSE if the stall interrupt should not actually be handled.
 */
NvBool
tmrClearInterrupt_IMPL(OBJGPU *pGpu, OBJTMR *pTmr, IntrServiceClearInterruptArguments *pParams)
{
    Intr *pIntr = GPU_GET_INTR(pGpu);
    NV_ASSERT_OR_RETURN(pParams != NULL, NV_FALSE);

    switch (pParams->engineIdx) {
        case MC_ENGINE_IDX_TMR:
        {
            intrClearLeafVector_HAL(pGpu, pIntr,
                                       intrGetVectorFromEngineId(pGpu, pIntr, pParams->engineIdx, NV_FALSE),
                                       NULL);
            return NV_TRUE;
        }
        case MC_ENGINE_IDX_TMR_SWRL:
        {
            return tmrClearSwrlCallbacksSemaphore(pGpu, pTmr, NULL);
        }
        default:
        {
            NV_ASSERT_FAILED("invalid EngineIdx");
            return NV_FALSE;
        }
    }
}

