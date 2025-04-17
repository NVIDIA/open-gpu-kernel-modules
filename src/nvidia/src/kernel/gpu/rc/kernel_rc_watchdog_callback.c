/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "kernel/gpu/intr/intr.h"

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
    NvU32     chid
)
{
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);

    KernelChannel *pKernelChannel = kfifoChidMgrGetKernelChannel(
        pGpu, pKernelFifo,
        kfifoGetChidMgr(pGpu, pKernelFifo, pKernelRc->watchdog.runlistId),
        chid);

    if (pKernelChannel == NULL)
    {
        NV_PRINTF(LEVEL_WARNING,
                  "Unable to thwap channel 0x%02x, it's not in use\n", chid);
        return;
    }

    NV_PRINTF(LEVEL_INFO, "Thwapping channel 0x%02x.\n",
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
    KernelRc *pKernelRc
)
{
    NvU32 chid;

    for (chid = 0; chid < 32; chid++)
    {
        if (pKernelRc->watchdog.thwapChannelMask & (1 << chid))
        {

            _krcThwapChannel(pGpu, pKernelRc, chid);

            // Unless this channel is marked for repeat, clear its thwap bit.
            if (0 == (pKernelRc->watchdog.thwapRepeatMask & (1 << chid)))
            {
                pKernelRc->watchdog.thwapChannelMask &= ~(1 << chid);
            }
        }
        if (pKernelRc->watchdog.stompChannelMask & (1 << chid))
        {
            // Unless this channel is marked for repeat, clear its stomp bit.
            if (0 == (pKernelRc->watchdog.stompRepeatMask & (1 << chid)))
            {
                pKernelRc->watchdog.stompChannelMask &= ~(1 << chid);
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
    //
    // These calls shouldn't occur during a hibernate/standby enter or resume
    // sequence or if the GPU is lost, which will cause a system hang.
    //
    if (gpuIsGpuFullPower(pGpu) &&
        !pGpu->getProperty(pGpu, PDB_PROP_GPU_IS_LOST))
    {
        KernelRc *pKernelRc = GPU_GET_KERNEL_RC(pGpu);

        krcWatchdog_HAL(pGpu, pKernelRc);
        krcWatchdogCallbackVblankRecovery(pGpu, pKernelRc);
        krcWatchdogCallbackPerf_HAL(pGpu, pKernelRc);
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
    KernelRc *pKernelRc
)
{
    NvU32 usec, sec;
    NvU64 currentTime;
    NvBool allNotifiersWritten = NV_TRUE;
    NV_STATUS rmStatus;

    // Do nothing if robust channels are not enabled
    if (!pKernelRc->bRobustChannelsEnabled)
        return;

    //
    // If the device has been reset then we can skip this sometimes after a
    // reset we can have a reenable make sure that this is a one time event
    //
    if (pKernelRc->watchdog.deviceResetRd != pKernelRc->watchdog.deviceResetWr)
    {
        if (pKernelRc->watchdog.deviceReset[pKernelRc->watchdog.deviceResetRd])
            pKernelRc->watchdog.flags |= WATCHDOG_FLAGS_DISABLED;

        pKernelRc->watchdog.deviceResetRd = (
            (pKernelRc->watchdog.deviceResetRd + 1) &
            (WATCHDOG_RESET_QUEUE_SIZE - 1));
    }

    if ((WATCHDOG_FLAGS_DISABLED !=
         (pKernelRc->watchdog.flags & WATCHDOG_FLAGS_DISABLED)) &&
        gpuIsGpuFullPower(pGpu))
    {
        //
        // Make sure we're initialized.  If not, initialize and wait for the
        // next watchdog call.
        //
        if (!(pKernelRc->watchdog.flags & WATCHDOG_FLAGS_INITIALIZED))
        {
            rmStatus = krcWatchdogInit_HAL(pGpu, pKernelRc);

            if (rmStatus!= NV_OK)
            {
                NV_PRINTF(LEVEL_INFO, "krcWatchdogInit failed: %d\n", rmStatus);
            }
            return;
        }

        // Count the number of invocations of the callback.
        pKernelRc->watchdog.count++;

        // Check if, for some reason, the watchdog triggered an error
        if ((pKernelRc->watchdog.errorContext->status & 0xFFFF) != 0)
        {
            NV_PRINTF(LEVEL_WARNING,
                      "RC watchdog: error on our channel (reinitializing).\n");

            // reset allows enough time to restart
            pKernelRc->watchdog.errorContext->status = 0;

            // reinit the pushbuffer image and kickoff again
            krcWatchdogInitPushbuffer_HAL(pGpu, pKernelRc);

            // Run Immediately
            pKernelRc->watchdogPersistent.nextRunTime = 0;
        }

        // Handle robust channel testing, if necessary.
        if (pKernelRc->watchdog.channelTestCountdown != 0)
        {
            pKernelRc->watchdog.channelTestCountdown--;
            if (pKernelRc->watchdog.channelTestCountdown == 0)
            {
                if ((pKernelRc->watchdog.thwapChannelMask != 0) ||
                    (pKernelRc->watchdog.stompChannelMask != 0))
                {
                    _krcTestChannelRecovery(pGpu, pKernelRc);
                }
                pKernelRc->watchdog.channelTestCountdown =
                    pKernelRc->watchdog.channelTestInterval;
            }
        }

        osGetCurrentTime(&sec, &usec);
        currentTime = (((NvU64)sec) * 1000000) + usec;

        //
        // See if all GPUs got around to delivering their notifiers.  If so,
        // they will have set the notifier statuses to 0.
        //
        SLI_LOOP_START(SLI_LOOP_FLAGS_NONE);
        {
            NvU32 subdeviceId = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
            if (pKernelRc->watchdog.notifiers[subdeviceId]->status != 0)
                allNotifiersWritten = NV_FALSE;
        }
        SLI_LOOP_END;

        if (!allNotifiersWritten)
        {
            if (currentTime >= pKernelRc->watchdogPersistent.notifyLimitTime)
            {
                //
                // If the card hasn't gotten around to us for many seconds,
                // something is wrong.
                //
                NV_PRINTF(LEVEL_ERROR,
                    "RC watchdog: GPU is probably locked!  Notify Timeout Seconds: %d\n",
                    pKernelRc->watchdogPersistent.timeoutSecs);

                // Disable the watchdog for now, and drop the critical section.
                pKernelRc->watchdog.flags |= WATCHDOG_FLAGS_DISABLED;

                //
                // Attempt to clean up the mess.
                // This should probably be in an SLI loop when a GSP client
                //
                krcWatchdogRecovery_HAL(pGpu, pKernelRc);

                // Re-enable.
                pKernelRc->watchdog.flags &= ~WATCHDOG_FLAGS_DISABLED;

                SLI_LOOP_START(SLI_LOOP_FLAGS_NONE);
                {
                    NvU32 subdeviceId = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
                    pKernelRc->watchdog.notifiers[subdeviceId]->status = 0;
                }
                SLI_LOOP_END;

                pKernelRc->watchdogPersistent.nextRunTime = 0;

                NV_PRINTF(LEVEL_WARNING, "RC watchdog: Trying to recover.\n");

                return;
            }
            else if (currentTime >=
                     pKernelRc->watchdogPersistent.resetLimitTime)
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

        if (currentTime >= pKernelRc->watchdogPersistent.nextRunTime)
        {
            // Stored as microseconds (1000000 of a second)
            pKernelRc->watchdogPersistent.nextRunTime = currentTime +
                (pKernelRc->watchdogPersistent.intervalSecs * 1000000);
            pKernelRc->watchdogPersistent.notifyLimitTime = currentTime +
                (pKernelRc->watchdogPersistent.timeoutSecs * 1000000);
            pKernelRc->watchdogPersistent.resetLimitTime = currentTime +
                (WATCHDOG_RESET_SECONDS * 1000000);

            // Reset the status to a known value.
            SLI_LOOP_START(SLI_LOOP_FLAGS_NONE);
            {
                NvU32 subdeviceId = gpumgrGetSubDeviceInstanceFromGpu(pGpu);
                pKernelRc->watchdog.notifiers[subdeviceId]->status = 0xFFFF;
            }
            SLI_LOOP_END;

            // Set the put pointer on our buffer.
            krcWatchdogWriteNotifierToGpfifo(pGpu, pKernelRc);
        }
    }
}


void
krcWatchdogRecovery_KERNEL
(
    OBJGPU   *pGpu,
    KernelRc *pKernelRc
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);

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
    KernelRc *pKernelRc
)
{
    NvU32           head;
    KernelDisplay  *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    Intr           *pIntr = GPU_GET_INTR(pGpu);
    MC_ENGINE_BITVECTOR intrDispPending;

    if (!pKernelRc->bRobustChannelsEnabled ||
        (pKernelRc->watchdog.flags & WATCHDOG_FLAGS_DISABLED) ||
        !gpuIsGpuFullPower(pGpu) || (pKernelDisplay == NULL))
    {
        return;
    }

    //
    // Determine the interrupt type for kdispServiceLowLatencyIntrs_HAL
    // to know what interrupt type it is
    //
    intrGetPendingLowLatencyHwDisplayIntr_HAL(pGpu, pIntr, &intrDispPending, NULL);

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
        if (pKernelRc->watchdog.oldVblank[head] ==
            kheadGetVblankTotalCounter_HAL(pKernelHead))
        {
            // VBlank Failed to Advance
            pKernelRc->watchdog.vblankFailureCount[head]++;

            if (pKernelRc->watchdog.vblankFailureCount[head] >
                pKernelRc->watchdogPersistent.timeoutSecs)
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
                                  pKernelRc->watchdog.oldVblank[head]);
                }

                NV_PRINTF(LEVEL_ERROR,
                    "NVRM-RC: RM has detected that %x Seconds without a Vblank Counter Update on head:%c%d\n",
                    pKernelRc->watchdogPersistent.timeoutSecs,
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

                pKernelRc->watchdog.vblankFailureCount[head] = 0;
            }
        }
        else
        {
            if (pKernelRc->watchdog.vblankFailureCount[head] > 0)
            {
                pKernelRc->watchdog.vblankFailureCount[head]--;
            }
        }

        pKernelRc->watchdog.oldVblank[head] = kheadGetVblankTotalCounter_HAL(
            pKernelHead);
    }
}

