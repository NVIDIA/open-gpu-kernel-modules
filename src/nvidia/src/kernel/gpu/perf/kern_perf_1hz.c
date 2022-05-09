/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/* ------------------------ Includes --------------------------------------- */
#include "gpu/perf/kern_perf.h"
#include "gpu/perf/kern_perf_1hz.h"
#include "objtmr.h"

/* ------------------------ Global Variables ------------------------------- */
/* ------------------------ Static Function Prototypes --------------------- */
/* ------------------------ Macros ----------------------------------------- */
/* ------------------------ Public Functions ------------------------------- */
/*!
 * @copydoc kperfTimerProc
 */
NV_STATUS
kperfTimerProc(OBJGPU *pGpu, OBJTMR *pTmr, void *ptr)
{
    KernelPerf  *pKernelPerf = GPU_GET_KERNEL_PERF(pGpu);

    // Request next callback.  call with Flags = TMR_FLAG_RECUR, since self-rescheduling
    if (ptr == TMR_POBJECT_KERNEL_PERF_1HZ)
    {
        pKernelPerf->timer1HzCallback.b1HzTimerCallback = NV_TRUE;
        tmrScheduleCallbackRelSec(pTmr, kperfTimerProc, TMR_POBJECT_KERNEL_PERF_1HZ, 1, TMR_FLAG_RECUR, 0);
    }

    kperfTimer1HzCallback(pGpu, pKernelPerf);

    return NV_OK;
}

/*!
 * @copydoc kperfTimer1HzCallback
 */
void
kperfTimer1HzCallback_IMPL(OBJGPU *pGpu, KernelPerf *pKernelPerf)
{
    NvBool bBcState = gpumgrGetBcEnabledStatus(pGpu);
    gpumgrSetBcEnabledStatus(pGpu, NV_FALSE);

    //
    // If GPU is in full power, timer updates are enabled and we are not breaking the 
    // reentrancy rules.
    //
    if ((gpuIsGpuFullPower(pGpu)) &&
        (pKernelPerf->timer1HzCallback.bEnableTimerUpdates) &&
        (kperfReentrancy(pGpu, pKernelPerf, KERNEL_PERF_REENTRANCY_TIMER_1HZ_CALLBACK, NV_TRUE) == NV_OK))
    {
        // If this function is called as a result of a 1HZ callback, do the Boost Hint callback
        if (pKernelPerf->timer1HzCallback.b1HzTimerCallback)
        {
            pKernelPerf->timer1HzCallback.b1HzTimerCallback = NV_FALSE;
        }

        // Release the reentrancy flag for this specific routine
        kperfReentrancy(pGpu, pKernelPerf, KERNEL_PERF_REENTRANCY_TIMER_1HZ_CALLBACK, NV_FALSE);
    }

    gpumgrSetBcEnabledStatus(pGpu, bBcState);

}

/* ------------------------- Private Functions ------------------------------ */
