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
/********************* Chip Specific HAL TMR Routines **********************\
*                                                                           *
*   The GH100 specific HAL TMR routines reside in this file.                *
*                                                                           *
\***************************************************************************/

/* ------------------------- Includes --------------------------------------- */
#include "gpu/gpu.h"
#include "gpu/timer/objtmr.h"
#include "gpu/fsp/kern_fsp.h"
#include "published/hopper/gh100/dev_vm.h"
#include "published/hopper/gh100/dev_timer.h"
#include "published/hopper/gh100/dev_gc6_island.h"
/* ------------------------- Datatypes -------------------------------------- */
/* ------------------------- Macros ----------------------------------------- */
/* ------------------------- Static Function Prototypes --------------------- */
/* ------------------------- Public Functions  ------------------------------ */

/*
 * @brief Sets the GPU time to the current wall-clock time.
 *
 *  @param[in] pGpu- GPU Object pointer
 *  @param[in] pTmr- Timer Object pointer
 *
 *  @return NV_OK
 */
NV_STATUS tmrSetCurrentTime_GH100
(
    OBJGPU *pGpu,
    OBJTMR *pTmr
)
{
    KernelFsp *pKernelFsp = GPU_GET_KERNEL_FSP(pGpu);
    NvU64 osTimeNs, secTimerNs, sysTimerOffsetNs;
    NvU32 secTimerLo, secTimerHi, secTimerHi2;
    NvU32 seconds;
    NvU32 useconds;

    osGetCurrentTime(&seconds, &useconds);

    NV_PRINTF(LEVEL_INFO,
        "osGetCurrentTime returns 0x%x seconds, 0x%x useconds\n",
        seconds, useconds);

    osTimeNs = ((NvU64)seconds * 1000000 + useconds) * 1000;

    //
    // Get the current secure timer value to calculate the offset to apply
    // Use hi-lo-hi reading to ensure a consistent value.
    //
    secTimerHi2 = GPU_REG_RD32(pGpu, NV_PGC6_SCI_SEC_TIMER_TIME_1);
    do
    {
        secTimerHi  = secTimerHi2;
        secTimerLo  = GPU_REG_RD32(pGpu, NV_PGC6_SCI_SEC_TIMER_TIME_0);
        secTimerHi2 = GPU_REG_RD32(pGpu, NV_PGC6_SCI_SEC_TIMER_TIME_1);
    } while (secTimerHi != secTimerHi2);
    secTimerNs = secTimerLo | (((NvU64)secTimerHi) << 32);

    NV_ASSERT_OR_RETURN(secTimerNs < osTimeNs, NV_ERR_INVALID_STATE);
    sysTimerOffsetNs = osTimeNs - secTimerNs;

    if ((pKernelFsp == NULL) || !kfspRequiresBug3957833WAR_HAL(pGpu, pKernelFsp))
    {
        //
        // We can only safely program the timer offset if FSP includes the fix
        // for bug 3957833.
        //
        GPU_REG_WR32(pGpu, NV_PGC6_SCI_SYS_TIMER_OFFSET_1, NvU64_HI32(sysTimerOffsetNs));
        GPU_REG_WR32(pGpu, NV_PGC6_SCI_SYS_TIMER_OFFSET_0, NvU64_LO32(sysTimerOffsetNs) |
                     DRF_DEF(_PGC6, _SCI_SYS_TIMER_OFFSET_0, _UPDATE, _TRIGGER));
    }

    //
    // PTIMER (the system timer) may need to be manually adjusted by the offset
    // everywhere it is supposed to match the host timestamp (for cases where
    // the above writes didn't stick, or where the calling code doesn't have
    // ready access to NV_PTIMER_TIME).
    //
    pTmr->sysTimerOffsetNs = sysTimerOffsetNs;

    return NV_OK;
}

NV_STATUS
tmrSetCountdown_GH100
(
    OBJGPU            *pGpu,
    OBJTMR            *pTmr,
    NvU32              time,
    NvU32              tmrId,
    THREAD_STATE_NODE *pThreadState
)
{
    NV_ASSERT_OR_RETURN(tmrId < NV_VIRTUAL_FUNCTION_PRIV_TIMER__SIZE_1, NV_ERR_NOT_SUPPORTED);

    GPU_VREG_WR32_EX(pGpu, NV_VIRTUAL_FUNCTION_PRIV_TIMER(tmrId), time, pThreadState);
    return NV_OK;
}

NvU64
tmrGetTimeEx_GH100
(
    OBJGPU             *pGpu,
    OBJTMR             *pTmr,
    THREAD_STATE_NODE  *pThreadState
)
{
    NvU32 TimeLo  = 0;
    NvU32 TimeHi  = 0;
    NvU32 TimeHi2 = 0;
    NvU32 i;
    NvU64 Time;

    do
    {
        TimeHi = tmrReadTimeHiReg_HAL(pGpu, pTmr, pThreadState);
        // Get a stable TIME_0
        for (i = 0; i < pTmr->retryTimes; ++i)
        {
            TimeLo = tmrReadTimeLoReg_HAL(pGpu, pTmr, pThreadState);
            if ((TimeLo & ~DRF_SHIFTMASK(NV_PTIMER_TIME_0_NSEC)) == 0)
                break;
        }

        // Couldn't get a good value
        if (i == pTmr->retryTimes)
        {
            // PTIMER returns bad bits after several read attempts
            NV_PRINTF(LEVEL_ERROR,
                      "NVRM-RC: Consistently Bad TimeLo value %x\n", TimeLo);

            //
            // On previous chips we would attempt to reinitialize the timer at this point
            // On Hopper+ RM only handles setting the initial value of the timer at boot
            // and is not allowed to reinitialize the timer afterwards
            //
            DBG_BREAKPOINT();

            return 0;
        }

        // Read TIME_1 again to detect wrap around.
        TimeHi2 = tmrReadTimeHiReg_HAL(pGpu, pTmr, pThreadState);
    } while (TimeHi != TimeHi2);

    // Convert to 64b
    Time = (((NvU64)TimeHi) << 32) | TimeLo;

    return Time;
}
