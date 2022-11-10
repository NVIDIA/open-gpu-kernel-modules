/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/disp/head/kernel_head.h"
#include "gpu/disp/kern_disp.h"
#include "objtmr.h"

// Callback proc for vblank info gathering
static NV_STATUS
_kheadSetVblankGatherInfoCallback
(
    POBJGPU   pGpu,
    void     *Object,
    NvU32     param1,
    NvV32     BuffNum,
    NV_STATUS Status
)
{
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    // We get the head from param1. Beware that this is what we told the vblank support to tell us.
    KernelHead    *pKernelHead = KDISP_GET_HEAD(pKernelDisplay, param1);
    NvU32          Timeout = 0;

    Timeout = kheadTickVblankInfo(pGpu, pKernelHead);

    if (Timeout == 0)
    {
        // Time to kill off our persistence. The vblank service will remove us when we clear the persitent flag durring a callback.
        pKernelHead->Vblank.Callback.gatherInfo.Flags &= ~VBLANK_CALLBACK_FLAG_PERSISTENT;
    }

    return NV_OK;
}

void
kheadSetVblankGatherInfo_IMPL
(
    OBJGPU     *pGpu,
    KernelHead *pKernelHead,
    NvBool      enable
)
{
    if(enable)
    {
        //
        // Update the timeout member to some number of seconds worth of callbacks.
        // Note we're assuming 60 Hz here which doesn't really matter since
        // that's what headAddVblankCallback assumes as well.
        //
        pKernelHead->Vblank.Info.Timeout = 60  * VBLANK_INFO_GATHER_KEEPALIVE_SECONDS;

        //
        // Schedule a persistent vblank callback to handle the updates.
        // This will enable vblank IRQs if not already running.
        //
        pKernelHead->Vblank.Callback.gatherInfo.Proc   = _kheadSetVblankGatherInfoCallback;
        pKernelHead->Vblank.Callback.gatherInfo.pObject = NULL;
        pKernelHead->Vblank.Callback.gatherInfo.bObjectIsChannelDescendant = NV_FALSE;
        pKernelHead->Vblank.Callback.gatherInfo.Param1 = pKernelHead->PublicId;
        pKernelHead->Vblank.Callback.gatherInfo.Param2 = 0;
        pKernelHead->Vblank.Callback.gatherInfo.Status = NV_OK;
        pKernelHead->Vblank.Callback.gatherInfo.Flags  = VBLANK_CALLBACK_FLAG_SPECIFIED_VBLANK_NEXT | VBLANK_CALLBACK_FLAG_PERSISTENT;

        kheadAddVblankCallback(pGpu, pKernelHead, &pKernelHead->Vblank.Callback.gatherInfo);

        NV_PRINTF(LEVEL_INFO, "VBlank Gather Info requested,\n"
                  "       vblank service scheduled on head %d.\n",
                  pKernelHead->PublicId);
    }
    else
    {
        kheadDeleteVblankCallback(pGpu, pKernelHead, &pKernelHead->Vblank.Callback.gatherInfo);
    }
}

NvU32
kheadTickVblankInfo_IMPL
(
    OBJGPU     *pGpu,
    KernelHead *pKernelHead
)
{
    OBJTMR *pTmr = GPU_GET_TIMER(pGpu);
    NvU64   Time;
    NvU32   Delta;
    NvU32   Average;

    // Get current time.
    Time = tmrGetTime_HAL(pGpu, pTmr);

    pKernelHead->Vblank.Info.Time.Current = Time;
    if (pKernelHead->Vblank.Info.Count > 2)
    {
        Delta = NvU64_LO32(pKernelHead->Vblank.Info.Time.Current - pKernelHead->Vblank.Info.Time.Last);

        Average = pKernelHead->Vblank.Info.Time.Average;
        Average = (((Average << 2) - Average) + Delta) >> 2;
        pKernelHead->Vblank.Info.Time.Average = Average;
    }
    pKernelHead->Vblank.Info.Count++;
    pKernelHead->Vblank.Info.Time.Last = Time;

    pKernelHead->Vblank.Info.Timeout--;

    return pKernelHead->Vblank.Info.Timeout;
}
