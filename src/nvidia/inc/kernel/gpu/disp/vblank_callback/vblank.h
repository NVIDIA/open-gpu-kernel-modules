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

#ifndef VBLANK_H
#define VBLANK_H

#include "gpu/gpu.h"
/* ------------------------ Types definitions ------------------------------ */
/*!
 * Callback function prototype
 */
typedef NV_STATUS (*VBLANKCALLBACKPROC)(OBJGPU*, void *, NvU32, NvU32, NV_STATUS);

typedef struct VBLANKCALLBACK
{
    VBLANKCALLBACKPROC Proc;
    void              *pObject;
    NvBool             bObjectIsChannelDescendant;
    NvU32              Param1;
    NvU32              Param2;
    NvU32              VBlankCount;
    NvU32              VBlankOffset;
    NvU64              TimeStamp;
    NvU32              MC_CallbackFlag;
    NvU32              Flags;
    NV_STATUS          Status;
    struct VBLANKCALLBACK *Next;
    NvBool             bImmediateCallback;
    NvBool             bIsVblankNotifyEnable;
}VBLANKCALLBACK;

/* ------------------------ Macros & Defines ------------------------------- */

/*!
 * Callback function registration flags
 */
#define VBLANK_CALLBACK_FLAG_SPECIFIED_VBLANK_COUNT        0x00000001
#define VBLANK_CALLBACK_FLAG_COMPLETE_ON_OBJECT_CLEANUP    0x00000002
#define VBLANK_CALLBACK_FLAG_PERSISTENT                    0x00000004
#define VBLANK_CALLBACK_FLAG_SPECIFIED_TIMESTAMP           0x00000010
#define VBLANK_CALLBACK_FLAG_SPECIFIED_VBLANK_NEXT         0x00000020  // Explicit request for the next vblank.
#define VBLANK_CALLBACK_FLAG_SPECIFIED_VBLANK_OFFSET       0x00000040  // Explicit request for the vblank offset from the current one
#define VBLANK_CALLBACK_FLAG_PROMOTE_TO_FRONT              0x00000080  // Promotes to being 'first', while still honoring VBlankCount
#define VBLANK_CALLBACK_FLAG_RELEASES_SEMAPHORE            0x00000100  // A flag for deadlock detection to check if this callback could release a semaphore
#define VBLANK_CALLBACK_FLAG_GUARANTEE_SAFETY              0x00000200  // This callback absolutely needs to run during vertical blank, even if it runs late as a consequence.
#define VBLANK_CALLBACK_FLAG_LOW_LATENCY__ISR_ONLY         0x08000000  // This means always process during ISR (never DPC.) Be careful!
#define VBLANK_CALLBACK_FLAG_LOW_LATENCY                   0x10000000  // This now means ASAP, which could be ISR or DPC, depending on which happens first
#define VBLANK_CALLBACK_FLAG_MC_EXECUTE_ONCE               0x40000000  // A special flag for MultiChip configurations to have the callback execute only once
#define VBLANK_CALLBACK_FLAG_USER                          0x80000000

/*!
 * A little macro help for the CALLBACK_FLAG_MC_EXECUTE_ONCE flag above
 */
#define VBLANK_CALLBACK_EXECUTE_ONCE(x)    (x & VBLANK_CALLBACK_FLAG_MC_EXECUTE_ONCE)

/*!
 * VBlank Service info gathering keep-alive in seconds. This value is the number of seconds the vblank service will run after a client request vblank info.
 */
#define VBLANK_INFO_GATHER_KEEPALIVE_SECONDS    (5)

/*!
 * VBLANK SERVICE RELATED
 * VBlank Service callback processing flags
 * These two flags describe when to process the queues
 */

#define VBLANK_STATE_PROCESS_NORMAL                  (0x00000000)    // Process the requested queues if associated vblank interrupt is pending
#define VBLANK_STATE_PROCESS_IMMEDIATE               (0x00000001)    // Process the requested queues now, regardless of any vblank interrupt pending state

/*!
 * These three flags describe which queues to process
 */
#define VBLANK_STATE_PROCESS_LOW_LATENCY             (0x00000002)    // Process the low-latency vblank callback queue
#define VBLANK_STATE_PROCESS_NORMAL_LATENCY          (0x00000004)    // Process the normal-latency vblank callback queue

#define VBLANK_STATE_PROCESS_ALL_CALLBACKS           (VBLANK_STATE_PROCESS_LOW_LATENCY|VBLANK_STATE_PROCESS_NORMAL_LATENCY) // Process all callback (high and low latency) queues

#define VBLANK_STATE_PROCESS_CALLBACKS_ONLY          (0x00000008)    // Process only the callback queue(s) and nothing else

/*!
 * set when called from an ISR; if VBlank() is in an ISR and there is
 * more work to do, then VBlank() will not clear the pending bit
 */
#define VBLANK_STATE_PROCESS_CALLED_FROM_ISR         (0x00000010)
#define VBLANK_STATE_PROCESS_CALLED_FROM_DPC         (0x00000020)

/*! Vblank Interrupt state */
#define NV_HEAD_VBLANK_INTR_UNAVAILABLE              (0x00000000)
#define NV_HEAD_VBLANK_INTR_AVAILABLE                (0x00000001)
#define NV_HEAD_VBLANK_INTR_ENABLED                  (0x00000002)

#endif // VBLANK_H
