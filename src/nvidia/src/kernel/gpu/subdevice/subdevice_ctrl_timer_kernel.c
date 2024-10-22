/*
 * SPDX-FileCopyrightText: Copyright (c) 2004-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief This module contains the gpu control interfaces for the
 *        subdevice (NV20_SUBDEVICE_0) class. Subdevice-level control calls
 *        are directed unicast to the associated GPU.
 *        File contains ctrls related to TMR engine object
 */

#include "core/core.h"


#include "core/locks.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/timer/objtmr.h"
#include "rmapi/client.h"

#include "kernel/gpu/intr/intr.h"

//
// subdeviceCtrlCmdTimerCancel
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//
NV_STATUS
subdeviceCtrlCmdTimerCancel_IMPL
(
    Subdevice *pSubdevice
)
{
    OBJGPU *pGpu;
    OBJTMR *pTmr;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    if (pSubdevice == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pGpu = GPU_RES_GET_GPU(pSubdevice);
    pTmr = GPU_GET_TIMER(pGpu);

    if (pSubdevice->notifyActions[NV2080_NOTIFIERS_TIMER] != NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE)
    {
        tmrEventCancel(pTmr, pSubdevice->pTimerEvent);

        pSubdevice->notifyActions[NV2080_NOTIFIERS_TIMER] = NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE;
    }
    return NV_OK;
}

static NV_STATUS
gpuControlTimerCallback(OBJGPU *pGpu, OBJTMR *pTmr, TMR_EVENT *pTmrEvent)
{
    Subdevice *pSubDevice = reinterpretCast(pTmrEvent->pUserData, Subdevice *);
    PEVENTNOTIFICATION pNotifyEvent = inotifyGetNotificationList(staticCast(pSubDevice, INotifier));

    if (pSubDevice->notifyActions[NV2080_NOTIFIERS_TIMER] == NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE)
    {
        NV_PRINTF(LEVEL_INFO,
                  "callback is called but the timer is not scheduled\n");
        return NV_ERR_INVALID_STATE;
    }

    // Mark the timer as processed (no self-rescheduling for now)
    pSubDevice->notifyActions[NV2080_NOTIFIERS_TIMER] = NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE;

    // Find timer event
    while ((pNotifyEvent != NULL) && (pNotifyEvent->NotifyIndex != NV2080_NOTIFIERS_TIMER))
    {
        pNotifyEvent = pNotifyEvent->Next;
    }
    if (pNotifyEvent == NULL)
    {
        NV_PRINTF(LEVEL_INFO, "timer event is missing\n");
        return NV_ERR_INVALID_STATE;
    }

    // perform a direct callback to the client
    if (pNotifyEvent->Data != NvP64_NULL)
    {
        NvU64 currentTime = tmrGetTime_HAL(pGpu, pTmr);
        osEventNotification(pGpu, pNotifyEvent, NV2080_NOTIFIERS_TIMER,
                            &currentTime, sizeof(currentTime));
    }
    else
    {
        NV_PRINTF(LEVEL_INFO, "timer callback pointer is missing\n");
        return NV_ERR_INVALID_STATE;
    }
    return NV_OK;
}

static NV_STATUS
timerSchedule
(
    Subdevice *pSubdevice,
    NV2080_CTRL_CMD_TIMER_SCHEDULE_PARAMS *pTimerScheduleParams
)
{
    OBJGPU *pGpu;
    OBJTMR *pTmr;
    PEVENTNOTIFICATION pNotifyEvent;

    if (pSubdevice == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pGpu = GPU_RES_GET_GPU(pSubdevice);
    pTmr = GPU_GET_TIMER(pGpu);

    pNotifyEvent = inotifyGetNotificationList(staticCast(pSubdevice, INotifier));

    if (pSubdevice->notifyActions[NV2080_NOTIFIERS_TIMER] != NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_DISABLE)
    {
        NV_PRINTF(LEVEL_INFO,
                  "gpuControlTimerCallback: the timer is already scheduled for this subdevice\n");
        return NV_ERR_INVALID_STATE;
    }

    // Validate the timer event
    while ((pNotifyEvent != NULL) && (pNotifyEvent->NotifyIndex != NV2080_NOTIFIERS_TIMER))
    {
        pNotifyEvent = pNotifyEvent->Next;
    }
    if (pNotifyEvent == NULL)
    {
        NV_PRINTF(LEVEL_INFO,
                  "gpuControlTimerCallback: timer event is missing\n");
        return NV_ERR_INVALID_STATE;
    }
    if (((pNotifyEvent->NotifyType != NV01_EVENT_KERNEL_CALLBACK) && (pNotifyEvent->NotifyType != NV01_EVENT_KERNEL_CALLBACK_EX)) ||
        (pNotifyEvent->Data == NvP64_NULL))
    {
        NV_PRINTF(LEVEL_INFO,
                  "gpuControlTimer: cmd 0x%x: callback function is missing\n",
                  NV2080_CTRL_CMD_TIMER_SCHEDULE);
        return NV_ERR_INVALID_STATE;

    }

    // Mark the timer as processed (no self-rescheduling for now). Set the flag before calling the timer
    // since callback may be called right away.
    pSubdevice->notifyActions[NV2080_NOTIFIERS_TIMER] = NV2080_CTRL_EVENT_SET_NOTIFICATION_ACTION_SINGLE;

    if (pSubdevice->pTimerEvent != NULL)
    {
        if (tmrEventOnList(pTmr, pSubdevice->pTimerEvent))
        {
            tmrEventCancel(pTmr, pSubdevice->pTimerEvent);
        }
    }
    else
    {
        NV_ASSERT_OK_OR_RETURN(tmrEventCreate(pTmr,
                                &pSubdevice->pTimerEvent,
                                gpuControlTimerCallback,
                                pSubdevice,
                                TMR_FLAGS_NONE));
    }

    if (DRF_VAL(2080, _CTRL_TIMER_SCHEDULE_FLAGS, _TIME, pTimerScheduleParams->flags) == NV2080_CTRL_TIMER_SCHEDULE_FLAGS_TIME_ABS)
    {
        tmrEventScheduleAbs(pTmr, pSubdevice->pTimerEvent, pTimerScheduleParams->time_nsec);
    }
    else
    {
        tmrEventScheduleRel(pTmr, pSubdevice->pTimerEvent, pTimerScheduleParams->time_nsec);
    }

    return NV_OK;
}

//
// subdeviceCtrlCmdTimerSchedule
//
// Lock Requirements:
//      Assert that API lock  and GPUs lock held on entry
//
NV_STATUS
subdeviceCtrlCmdTimerSchedule_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_CMD_TIMER_SCHEDULE_PARAMS *pParams
)
{
    CALL_CONTEXT *pCallContext  = resservGetTlsCallContext();
    RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

    if (pRmCtrlParams->flags & NVOS54_FLAGS_IRQL_RAISED)
    {
        NV_ASSERT_OR_RETURN(rmDeviceGpuLockIsOwner(GPU_RES_GET_GPU(pSubdevice)->gpuInstance),
            NV_ERR_INVALID_LOCK_STATE);
    }
    else
    {
        NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);
    }

    return timerSchedule(pSubdevice, pParams);
}

//
// subdeviceCtrlCmdTimerGetTime
//
// Lock Requirements:
//      Assert that API lock and GPUs lock held on entry
//      Timer callback list accessed in tmrService at DPC
//
NV_STATUS
subdeviceCtrlCmdTimerGetTime_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_TIMER_GET_TIME_PARAMS *pParams
)
{
    CALL_CONTEXT *pCallContext  = resservGetTlsCallContext();
    RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);

    if ((pRmCtrlParams->flags & NVOS54_FLAGS_IRQL_RAISED) &&
        (pRmCtrlParams->flags & NVOS54_FLAGS_LOCK_BYPASS))
    {
        if (pTmr->tmrChangePending)
        {
            return NV_ERR_STATE_IN_USE;
        }
    }
    else
    {
        NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmDeviceGpuLockIsOwner(pGpu->gpuInstance),
            NV_ERR_INVALID_LOCK_STATE);
    }

    tmrGetCurrentTime(pTmr, &pParams->time_nsec);

    return NV_OK;
}

//
// subdeviceCtrlCmdTimerGetRegisterOffset
//
// Lock Requirements:
//      Assert that API lock held on entry
//      No GPUs lock
//
NV_STATUS
subdeviceCtrlCmdTimerGetRegisterOffset_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_TIMER_GET_REGISTER_OFFSET_PARAMS *pTimerRegOffsetParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    return gpuGetRegBaseOffset_HAL(pGpu, NV_REG_BASE_TIMER, &pTimerRegOffsetParams->tmr_offset);
}

/*!
 * @brief Provides correlation information between GPU time and CPU time.
 *
 * @param[in] pSubDevice
 * @param[in] pParams
 *
 * @return NV_OK                     Success
 * @return NV_ERR_INVALID_ARGUMENT   Invalid argument
 * @return NV_ERR_NOT_SUPPORTED      Unsupported CPU clock id
 */
NV_STATUS
subdeviceCtrlCmdTimerGetGpuCpuTimeCorrelationInfo_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_TIMER_GET_GPU_CPU_TIME_CORRELATION_INFO_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    NV_STATUS status = NV_OK;
    NvU8 i;
    NvU32 sec, usec;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner() && rmGpuLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    NV_CHECK_OR_RETURN(LEVEL_SILENT,
        (pParams->sampleCount <= NV2080_CTRL_TIMER_GPU_CPU_TIME_MAX_SAMPLES),
        NV_ERR_INVALID_ARGUMENT);

    if (RMCFG_FEATURE_PLATFORM_GSP)
    {
        NV_ASSERT_OR_RETURN(
            FLD_TEST_DRF(2080, _TIMER_GPU_CPU_TIME_CPU_CLK_ID, _PROCESSOR, _GSP,
                         pParams->cpuClkId),
            NV_ERR_INVALID_ARGUMENT);
    }
    else if (FLD_TEST_DRF(2080, _TIMER_GPU_CPU_TIME_CPU_CLK_ID, _PROCESSOR, _GSP,
                          pParams->cpuClkId))
    {
        //
        // If GSP time is requested, forward the whole request to GSP.
        // This can only be supported in GSP-RM offload mode.
        //
        if (!IS_GSP_CLIENT(pGpu))
            return NV_ERR_NOT_SUPPORTED;

        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

        return pRmApi->Control(pRmApi,
                               pGpu->hInternalClient,
                               pGpu->hInternalSubdevice,
                               NV2080_CTRL_CMD_TIMER_GET_GPU_CPU_TIME_CORRELATION_INFO,
                               pParams, sizeof(*pParams));
    }
    else
    {
        NV_CHECK_OR_RETURN(LEVEL_SILENT,
            FLD_TEST_DRF(2080, _TIMER_GPU_CPU_TIME_CPU_CLK_ID, _PROCESSOR, _CPU,
                         pParams->cpuClkId),
            NV_ERR_INVALID_ARGUMENT);
    }

    switch (DRF_VAL(2080, _TIMER_GPU_CPU_TIME_CPU_CLK_ID, _SOURCE, pParams->cpuClkId))
    {
        case NV2080_TIMER_GPU_CPU_TIME_CPU_CLK_ID_OSTIME:
        {
            for (i = 0; i < pParams->sampleCount; i++)
            {
                osGetCurrentTime(&sec, &usec);
                pParams->samples[i].cpuTime = (((NvU64)sec) * 1000000) + usec;
                status = tmrGetCurrentTime(pTmr,
                    &pParams->samples[i].gpuTime);
                if (status != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "Could not get GPU time. status=0x%08x\n",
                              status);
                    break;
                }
            }
            break;
        }

        case NV2080_TIMER_GPU_CPU_TIME_CPU_CLK_ID_PLATFORM_API:
        {
            //
            // As reading CPU time and GPU time is a serial process we need to
            // have a technique to mitigate the effects of preemption so we read
            // the timestamps in a zipper pattern like c G c G c G c into an
            // array storing all 7 values, find the two c values closest together,
            // and report the sync point as the average of those two c values and
            // the G between them. One complication is that reading a GPU's PTIMER
            // directly from the CPU must be done via two non-adjacent BAR0-mapped
            // memory locations for the low 32 bits and high 32 bits, and there's
            // no way to atomically get both. One way to fix this is to make the
            // read of the GPU time do the high bits, the low bits, and the high
            // bits again, and if the two high values differ, we repeat the process
            // until Ghi1 and Ghi2 match Once Ghi1 and 2 match, we use that as
            // the high bits and the lo bits & CPU time from the zipper.
            //
            const NvU32 numTimerSamples = 3; // We take (hardcoded) 3 gpu timestamps.
            NvU32 gpuTimeLo[3]; // Array to hold num_timer_samples gpu timestamps.
            NvU64 cpuTime[4];   // Array to hold num_timer_samples+1 cpu timestamps.
            NvU64 min;
            NvU32 closestPairBeginIndex;
            NvU32 gpuTimeHiOld;
            NvU32 gpuTimeHiNew;
            NvU32 i;

            gpuTimeHiNew = tmrReadTimeHiReg_HAL(pGpu, pTmr, NULL);

            do
            {
                gpuTimeHiOld = gpuTimeHiNew;
                for (i = 0; i < numTimerSamples; i++)
                {

                    osGetPerformanceCounter(&cpuTime[i]);

                    gpuTimeLo[i] = tmrReadTimeLoReg_HAL(pGpu, pTmr, NULL);
                }

                osGetPerformanceCounter(&cpuTime[i]);

                // Read GPU TIME_1(High) again to detect wrap around.
                gpuTimeHiNew = tmrReadTimeHiReg_HAL(pGpu, pTmr, NULL);
            } while (gpuTimeHiNew != gpuTimeHiOld);

            // find i such that cpuTime[i+1] - cpuTime[i] is minimum
            // i.e. find closest pair of cpuTime.
            min = cpuTime[1] - cpuTime[0];
            closestPairBeginIndex = 0;
            for (i = 0; i < numTimerSamples; i++)
            {
                if ((cpuTime[i+1] - cpuTime[i]) < min)
                {
                    closestPairBeginIndex = i;
                    min = cpuTime[i+1] - cpuTime[i];
                }
            }

            pParams->samples[0].gpuTime = ((((NvU64)gpuTimeHiNew) << 32) |
                                           gpuTimeLo[closestPairBeginIndex]);
            pParams->samples[0].cpuTime = (cpuTime[closestPairBeginIndex] +
                                           cpuTime[closestPairBeginIndex + 1])/2;
            NV_PRINTF(LEVEL_INFO,
                      "GPUTime = %llx  CPUTime = %llx\n",
                      pParams->samples[0].gpuTime, pParams->samples[0].cpuTime);
            break;
        }

        case NV2080_TIMER_GPU_CPU_TIME_CPU_CLK_ID_TSC:
        {
            for (i = 0; i < pParams->sampleCount; i++)
            {
                status = tmrGetGpuAndCpuTimestampPair_HAL(pGpu, pTmr, &pParams->samples[i].gpuTime, &pParams->samples[i].cpuTime);
                if (status != NV_OK)
                {
                    NV_PRINTF(LEVEL_ERROR,
                              "Could not get CPU GPU time. status=0x%08x\n",
                              status);
                    break;
                }
            }
            break;
        }
        default:
        {
            status = NV_ERR_NOT_SUPPORTED;
            break;
        }
    }

    return status;
}

/*!
 * @brief Set the frequency to update GR time stamp to default or max.
 *
 * The GR tick frequency will be restored to default
 * only when no client has a pending request to increase.
 *
 * Lock Requirements:
 *      Assert that API lock held on entry
 *      No GPUs lock
 *
 * @param[in] pSubDevice
 * @param[in] pParams
 *
 * @return NV_OK                     Success
 *         NV_ERR_INVALID_ARGUMENT   Invalid Argument
 *         NV_ERR_NOT_SUPPORTED      Not Supported
 *         Other errors from refcntRequestReference() or refcntReleaseReferences()
 */
NV_STATUS
subdeviceCtrlCmdTimerSetGrTickFreq_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_CMD_TIMER_SET_GR_TICK_FREQ_PARAMS *pParams
)
{
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pSubdevice);
    OBJTMR   *pTmr = GPU_GET_TIMER(pGpu);
    NvHandle  hClient = RES_GET_CLIENT_HANDLE(pSubdevice);
    NV_STATUS status;
    OBJREFCNT *pRefcnt;
    NvHandle hSubDevice;

    NV_ASSERT_OR_RETURN(rmapiLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);

    if (pSubdevice == NULL || pTmr == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    pRefcnt = pTmr->pGrTickFreqRefcnt;

    hSubDevice = RES_GET_HANDLE(pSubdevice);
    if (pParams->bSetMaxFreq)
    {
        status = refcntRequestReference(pRefcnt,
                    NV_REQUESTER_CLIENT_OBJECT(hClient, hSubDevice),
                    REFCNT_STATE_ENABLED, NV_FALSE);
        pSubdevice->bMaxGrTickFreqRequested = NV_TRUE;
    }
    else
    {
        status = refcntReleaseReferences(pRefcnt,
                    NV_REQUESTER_CLIENT_OBJECT(hClient, hSubDevice),
                    NV_TRUE);
        pSubdevice->bMaxGrTickFreqRequested = NV_FALSE;
    }
    return status;
}

