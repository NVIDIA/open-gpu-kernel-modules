/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 *
 * Defines for watchdog component of kernel RC.
 *
 * Don't include this file directly, use kernel_rc.h. Watchdog will be split
 * into its own component in future. Keeping defines in a separate file to aid
 * that transition. See CORERM-2297
 */
#ifndef KERNEL_RC_WATCHDOG_PRIVATE_H
#define KERNEL_RC_WATCHDOG_PRIVATE_H 1

#include "kernel/gpu/disp/kern_disp_max.h"

#include "class/cl906f.h" // GF100_CHANNEL_GPFIFO

#include "nvgputypes.h"
#include "nvlimits.h"
#include "nvtypes.h"


#define WATCHDOG_RESET_QUEUE_SIZE (4)

// KernelWatchdog.flags
#define WATCHDOG_FLAGS_INITIALIZED        NVBIT(0) // Fully initialized and ready
#define WATCHDOG_FLAGS_DISABLED           NVBIT(1) // Disabled
#define WATCHDOG_FLAGS_ALLOC_UNCACHED_PCI NVBIT(2) // Alloc cached / uncached pushbuffer

/*! Volatile watchdog state that is destroyed when watchdog is shutdown */
typedef struct {
    NvHandle hClient;
    NvU32 runlistId;
    NvU32 flags;
    NvU32 deviceReset[WATCHDOG_RESET_QUEUE_SIZE];
    /*! Read Pointer for fifoWatchDog */
    NvU32 deviceResetRd;
    // RmResetWatchdog
    NvU32 deviceResetWr;
    /*! Number of watchdog invocations */
    NvU32 count;
    /*! Countdown for running thwap and stomp tests */
    NvU32 channelTestCountdown;
    /*! Reset value for ChannelTestTimer */
    NvU32 channelTestInterval;
    /*! Masks for RC testing */
    NvU32 thwapChannelMask;
    NvU32 thwapRepeatMask;
    NvU32 stompChannelMask;
    NvU32 stompRepeatMask;
    /*! Mask of allocations to fail for testing RC. See nvcm.h */
    NvU32 allocFailMask;
    /*! Array of NvU32 to hold last vblank counter */
    NvU32 oldVblank[OBJ_MAX_HEADS];
    /*! Number of times that Vblank has failed to advance */
    NvU32 vblankFailureCount[OBJ_MAX_HEADS];
    NvNotification *notifiers[NV_MAX_SUBDEVICES];
    NvNotification *errorContext;
    NvNotification *notifierToken;
    NvBool bHandleValid;
} KernelWatchdog;


/*! Persistent watchdog state preserved across watchdog shutdowns */
typedef struct {
    NvS32 enableRequestsRefCount;
    NvS32 disableRequestsRefCount;
    NvS32 softDisableRequestsRefCount;

    /*! How long we wait for the notifier to come back after being run */
    NvU32 timeoutSecs;
    /*! Seconds between when the Watchdog is run */
    NvU32 intervalSecs;
    NvU64 notifyLimitTime;
    NvU64 nextRunTime;
    NvU64 resetLimitTime;
} KernelWatchdogPersistent;


/*! Watchdog channel info */
typedef struct
{
    Nv906fControl *pControlGPFifo[NV_MAX_SUBDEVICES];
    NvU64  pGpuAddr;
    NvU8  *pCpuAddr;
    // Class engine ID needed for SetObject on Fermi+
    NvU32 classEngineID;
    NvU32 gpEntry0[2];
    NvU32 gpEntry1[2];
    NvU32 pbBytes;
    NvU32 class2dSubch;
} KernelWatchdogChannelInfo;

#endif // ifndef KERNEL_RC_WATCHDOG_PRIVATE_H
