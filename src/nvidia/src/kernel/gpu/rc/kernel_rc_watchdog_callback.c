/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "kernel/gpu/rc/kernel_rc.h"

#include "kernel/diagnostics/journal.h"
#include "kernel/gpu/disp/head/kernel_head.h"
#include "kernel/gpu/disp/kern_disp.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/gpu.h"
#include "kernel/gpu_mgr/gpu_mgr.h"
#include "platform/sli/sli.h"

#include "kernel/gpu/intr/engine_idx.h"

#include "ctrl/ctrl906f.h"

#include "nverror.h"


// Seconds before watchdog tries to reset itself
#define WATCHDOG_RESET_SECONDS 4


//
// Thwapping simply invokes error recovery on the given channel, faking a parse
// error.  Error recovery isolates the channel, so ignoring the error is not an
// option for clients.
//
static void
_krcThwapChannel
(
    OBJGPU   *pGpu,
    KernelRc *pKernelRc,
    KernelWatchdog *pKernelWatchdog,
    NvU32     chid
)
{
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    KernelWatchdogState *pWatchdogState = ((pKernelWatchdog != NULL) ? &pKernelWatchdog->watchdogState : &pKernelRc->watchdog);

    KernelChannel *pKernelChannel = kfifoChidMgrGetKernelChannel(
        pGpu, pKernelFifo,
        kfifoGetChidMgr(pGpu, pKernelFifo, pWatchdogState->runlistId),
        chid);

    if (pKernelChannel == NULL)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Unable to thwap channel 0x%02x, it's not in use\n", chid);
        return;
    }

    NV_PRINTF(LEVEL_INFO,
              "Thwapping channel " FMT_CHANNEL_DEBUG_TAG ".\n",
              kchannelGetDebugTag(pKernelChannel));


    {
        RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
        NV906F_CTRL_CMD_RESET_CHANNEL_PARAMS params = {0};

        params.resetReason =
            NV906F_CTRL_CMD_INTERNAL_RESET_CHANNEL_REASON_FAKE_ERROR;
        params.subdeviceInstance = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
        params.engineID          = NV2080_ENGINE_TYPE_NULL;

        pRmApi->Control(pRmApi,
                        RES_GET_CLIENT_HANDLE(pKernelChannel),
                        RES_GET_HANDLE(pKernelChannel),
                        NV906F_CTRL_CMD_RESET_CHANNEL,
                        &params,
                        sizeof params);
    }
}


//
// Tests error recovery by causing channel errors of varying severity.  Only
// affects channels in the ThwapChannelMask and StompChannelMask bitmasks, which
// are normally zero.
//
static void
_krcTestChannelRecovery
(
    OBJGPU   *pGpu,
    KernelRc *pKernelRc,
    KernelWatchdog *pKernelWatchdog
)
{
    NvU32 chid;
    KernelWatchdogState *pWatchdogState = ((pKernelWatchdog != NULL) ? &pKernelWatchdog->watchdogState : &pKernelRc->watchdog);

    for (chid = 0; chid < 32; chid++)
    {
        if (pWatchdogState->thwapChannelMask & (1 << chid))
        {

            _krcThwapChannel(pGpu, pKernelRc, pKernelWatchdog, chid);

            // Unless this channel is marked for repeat, clear its thwap bit.
            if (0 == (pWatchdogState->thwapRepeatMask & (1 << chid)))
            {
                pWatchdogState->thwapChannelMask &= ~(1 << chid);
            }
        }
        if (pWatchdogState->stompChannelMask & (1 << chid))
        {
            // Unless this channel is marked for repeat, clear its stomp bit.
            if (0 == (pWatchdogState->stompRepeatMask & (1 << chid)))
            {
                pWatchdogState->stompChannelMask &= ~(1 << chid);
            }
        }
    }
}


void krcWatchdogTimerProc
(
    OBJGPU *pGpu,
    void   *data
)
{
    KernelWatchdog *pKernelWatchdog = (KernelWatchdog *)data;
    
    // TODO: (Bug 4154640) Below functions are not ready to support KernelWatchdog, so bail out if pKernelWatchdog is not NULL
    if (pKernelWatchdog == NULL)
    {
        //
        // These calls shouldn't occur during a hibernate/standby enter or resume
        // sequence or if the GPU is lost, which will cause a system hang.
        //
        if (gpuIsGpuFullPower(pGpu) &&
            !pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_LOST))
        {
            KernelRc *pKernelRc = GPU_GET_KERNEL_RC(pGpu);

            krcWatchdog_HAL(pGpu, pKernelRc, pKernelWatchdog);
            krcWatchdogCallbackVblankRecovery(pGpu, pKernelRc, pKernelWatchdog);
            krcWatchdogCallbackPerf_HAL(pGpu, pKernelRc, pKernelWatchdog);
        }
    }
}


//
// Watchdog callback routine. Monitors GPU and attempts to recover from lockups,
// etc.
//
void
krcWatchdog_IMPL
(
    OBJGPU   *pGpu,
    KernelRc *pKernelRc,
    KernelWatchdog *pKernelWatchdog
)
{
    NvU32 usec, sec;
    NvU64 currentTime;
    NvBool allNotifiersWritten = NV_TRUE;
    NV_STATUS rmStatus;
    KernelWatchdogState *pWatchdogState = ((pKernelWatchdog != NULL) ? &pKernelWatchdog->watchdogState : &pKernelRc->watchdog);
    KernelWatchdogPersistent *pWatchdogPersistent = ((pKernelWatchdog != NULL) ? &pKernelWatchdog->watchdogPersistent : &pKernelRc->watchdogPersistent);

    // Do nothing if robust channels are not enabled
    if (!pKernelRc->bRobustChannelsEnabled)
        return;

    //
    // If the device has been reset then we can skip this sometimes after a
    // reset we can have a reenable make sure that this is a one time event
    //
    if (pWatchdogState->deviceResetRd != pWatchdogState->deviceResetWr)
    {
        if (pWatchdogState->deviceReset[pWatchdogState->deviceResetRd])
            pWatchdogState->flags |= WATCHDOG_FLAGS_DISABLED;

        pWatchdogState->deviceResetRd = (
            (pWatchdogState->deviceResetRd + 1) &
            (WATCHDOG_RESET_QUEUE_SIZE - 1));
    }

    if ((WATCHDOG_FLAGS_DISABLED !=
         (pWatchdogState->flags & WATCHDOG_FLAGS_DISABLED)) &&
        gpuIsGpuFullPower(pGpu))
    {
        //
        // Make sure we're initialized.  If not, initialize and wait for the
        // next watchdog call.
        //
        if (!(pWatchdogState->flags & WATCHDOG_FLAGS_INITIALIZED))
        {
            rmStatus = krcWatchdogInit_HAL(pGpu, pKernelRc, pKernelWatchdog);

            if (rmStatus!= NV_OK)
            {
                NV_PRINTF(LEVEL_INFO, "krcWatchdogInit failed: %d\n", rmStatus);
            }
            return;
        }

        // Count the number of invocations of the callback.
        pWatchdogState->count++;

        // Check if, for some reason, the watchdog triggered an error
        if ((pWatchdogState->errorContext->status & 0xFFFF) != 0)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "RC watchdog: error on our channel (reinitializing).\n");

            // reset allows enough time to restart
            pWatchdogState->errorContext->status = 0;

            // reinit the pushbuffer image and kickoff again
            krcWatchdogInitPushbuffer_HAL(pGpu, pKernelRc, pKernelWatchdog);

            // Run Immediately
            pWatchdogPersistent->nextRunTime = 0;
        }

        // Handle robust channel testing, if necessary.
        if (pWatchdogState->channelTestCountdown != 0)
        {
            pWatchdogState->channelTestCountdown--;
            if (pWatchdogState->channelTestCountdown == 0)
            {
                if ((pWatchdogState->thwapChannelMask != 0) ||
                    (pWatchdogState->stompChannelMask != 0))
                {
                    _krcTestChannelRecovery(pGpu, pKernelRc, pKernelWatchdog);
                }
                pWatchdogState->channelTestCountdown =
                    pWatchdogState->channelTestInterval;
            }
        }

        osGetSystemTime(&sec, &usec);
        currentTime = (((NvU64)sec) * 1000000) + usec;

        //
        // See if all GPUs got around to delivering their notifiers.  If so,
        // they will have set the notifier statuses to 0.
        //
        SLI_LOOP_START(SLI_LOOP_FLAGS_NONE);
        {
            NvU32 subdeviceId = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
            if (pWatchdogState->notifiers[subdeviceId]->status != 0)
                allNotifiersWritten = NV_FALSE;
        }
        SLI_LOOP_END;

        if (!allNotifiersWritten)
        {
            if (currentTime >= pWatchdogPersistent->notifyLimitTime)
            {
                //
                // If the card hasn't gotten around to us for many seconds,
                // something is wrong.
                //
                NV_PRINTF(LEVEL_ERROR,
                    "RC watchdog: GPU is probably locked!  Notify Timeout Seconds: %d\n",
                    pWatchdogPersistent->timeoutSecs);

                // Disable the watchdog for now, and drop the critical section.
                pWatchdogState->flags |= WATCHDOG_FLAGS_DISABLED;

                //
                // Attempt to clean up the mess.
                // This should probably be in an SLI loop when a GSP client
                //
                krcWatchdogRecovery_HAL(pGpu, pKernelRc, pKernelWatchdog);

                // Re-enable.
                pWatchdogState->flags &= ~WATCHDOG_FLAGS_DISABLED;

                SLI_LOOP_START(SLI_LOOP_FLAGS_NONE);
                {
                    NvU32 subdeviceId = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
                    pWatchdogState->notifiers[subdeviceId]->status = 0;
                }
                SLI_LOOP_END;

                pWatchdogPersistent->nextRunTime = 0;

                NV_PRINTF(LEVEL_WARNING, "RC watchdog: Trying to recover.\n");

                return;
            }
            else if (currentTime >=
                     pWatchdogPersistent->resetLimitTime)
            {
                //
                // It's entirely possible that the card is extremely busy and
                // hasn't gotten around to the watchdog channel yet. This can
                // happen during power events and mode sets. Allow for a few
                // failures before we panic.
                //
                NV_PRINTF(LEVEL_WARNING,
                    "RC watchdog: GPU is possibly locked. Attempting to restart watchdog.\n");

                //
                // Fall through and attempt to update the watchdog's position.
                // This may result in a double notify exception, but that's not
                // fatal.
                //
            }
            else
            {
                return;
            }
        }

        if (currentTime >= pWatchdogPersistent->nextRunTime)
        {
            // Stored as microseconds (1000000 of a second)
            pWatchdogPersistent->nextRunTime = currentTime +
                (pWatchdogPersistent->intervalSecs * 1000000);
            pWatchdogPersistent->notifyLimitTime = currentTime +
                (pWatchdogPersistent->timeoutSecs * 1000000);
            pWatchdogPersistent->resetLimitTime = currentTime +
                (WATCHDOG_RESET_SECONDS * 1000000);

            // Reset the status to a known value.
            SLI_LOOP_START(SLI_LOOP_FLAGS_NONE);
            {
                NvU32 subdeviceId = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
                pWatchdogState->notifiers[subdeviceId]->status = 0xFFFF;
            }
            SLI_LOOP_END;

            // Set the put pointer on our buffer.
            krcWatchdogWriteNotifierToGpfifo(pGpu, pKernelRc, pKernelWatchdog);
        }
    }
}


void
krcWatchdogRecovery_KERNEL
(
    OBJGPU   *pGpu,
    KernelRc *pKernelRc,
    KernelWatchdog *pKernelWatchdog
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

    // TODO: (Bug 4154640) To be updated to support KernelWatchdog under MIG mode
    pRmApi->Control(pRmApi,
                    pGpu->hInternalClient,
                    pGpu->hInternalSubdevice,
                    NV2080_CTRL_CMD_INTERNAL_RC_WATCHDOG_TIMEOUT,
                    NULL,
                    0);
}


/*!
 * @brief Recover VBlank callbacks that may have missed due to missing VBlank
 */
void krcWatchdogCallbackVblankRecovery_IMPL
(
    OBJGPU   *pGpu,
    KernelRc *pKernelRc,
    KernelWatchdog *pKernelWatchdog
)
{
    NvU32           head;
    KernelDisplay  *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    MC_ENGINE_BITVECTOR intrDispPending;
    KernelWatchdogState *pWatchdogState = ((pKernelWatchdog != NULL) ? &pKernelWatchdog->watchdogState : &pKernelRc->watchdog);
    KernelWatchdogPersistent *pWatchdogPersistent = ((pKernelWatchdog != NULL) ? &pKernelWatchdog->watchdogPersistent : &pKernelRc->watchdogPersistent);

    if (!pKernelRc->bRobustChannelsEnabled ||
        (pWatchdogState->flags & WATCHDOG_FLAGS_DISABLED) ||
        !gpuIsGpuFullPower(pGpu) || (pKernelDisplay == NULL))
    {
        return;
    }

    //
    // The pending intr would have been cleared from the HW by now
    // if there is a deferred vblank. We don't need a retrigger for
    // MC_ENGINE_IDX_DISP since that is handled in a separate function
    // and doesn't need a clear if a vblank is pending.
    // We do need a retrigger if we have a separate interrupt vector,
    // so do one if we have it.
    //
    bitVectorClrAll(&intrDispPending);

    if (pKernelDisplay->getProperty(pKernelDisplay, PDB_PROP_KDISP_HAS_SEPARATE_LOW_LATENCY_LINE))
    {
        bitVectorSet(&intrDispPending, MC_ENGINE_IDX_DISP_LOW);
    }

    for (head = 0; head < kdispGetNumHeads(pKernelDisplay); head++)
    {
        KernelHead *pKernelHead = KDISP_GET_HEAD(pKernelDisplay, head);

        if (kheadReadVblankIntrState(pGpu, pKernelHead) !=
            NV_HEAD_VBLANK_INTR_ENABLED)
        {
            continue;
        }

        //
        // Sliding windows -- we expect some failures around mode switches
        // since vblank and watchdog are async to each other. This will
        // dispose of these.
        //
        if (pWatchdogState->oldVblank[head] ==
            kheadGetVblankTotalCounter_HAL(pKernelHead))
        {
            // VBlank Failed to Advance
            pWatchdogState->vblankFailureCount[head]++;

            if (pWatchdogState->vblankFailureCount[head] >
                pWatchdogPersistent->timeoutSecs)
            {
                Journal        *pRcDB = SYS_GET_RCDB(SYS_GET_INSTANCE());
                SYS_ERROR_INFO *pSysErrorInfo = &pRcDB->ErrorInfo;

                if (pKernelRc->bLogEvents &&
                    pSysErrorInfo->LogCount < MAX_ERROR_LOG_COUNT)
                {
                    pSysErrorInfo->LogCount++;
                    nvErrorLog_va((void *)pGpu,
                                  ROBUST_CHANNEL_VBLANK_CALLBACK_TIMEOUT,
                                  "Head %08x Count %08x",
                                  head,
                                  pWatchdogState->oldVblank[head]);
                }

                NV_PRINTF(LEVEL_ERROR,
                    "NVRM-RC: RM has detected that %x Seconds without a Vblank Counter Update on head:%c%d\n",
                    pWatchdogPersistent->timeoutSecs,
                    'A' + head,
                    gpuGetInstance(pGpu));

                //
                // Have the VBlank Service run through in IMMEDIATE mode and
                // process all queues
                //
                kdispServiceLowLatencyIntrs_HAL(pGpu, pKernelDisplay,
                                                NVBIT(head),
                                                (VBLANK_STATE_PROCESS_IMMEDIATE |
                                                VBLANK_STATE_PROCESS_ALL_CALLBACKS),
                                                NULL /* threadstate */,
                                                NULL /* vblankIntrServicedHeadMask */,
                                                &intrDispPending);

                pWatchdogState->vblankFailureCount[head] = 0;
            }
        }
        else
        {
            if (pWatchdogState->vblankFailureCount[head] > 0)
            {
                pWatchdogState->vblankFailureCount[head]--;
            }
        }

        pWatchdogState->oldVblank[head] = kheadGetVblankTotalCounter_HAL(
            pKernelHead);
    }
}

