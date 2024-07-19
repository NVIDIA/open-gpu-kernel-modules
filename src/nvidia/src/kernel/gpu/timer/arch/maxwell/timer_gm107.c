/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
/************************** Ptimer based Routines **************************\
*                                                                           *
*   The GM107 specific HAL TMR routines reside in this file.                *
*                                                                           *
\***************************************************************************/
/* ------------------------- Includes --------------------------------------- */
#include "gpu/gpu.h"
#include "gpu/timer/objtmr.h"
#include "published/maxwell/gm107/dev_timer.h"
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
 *  @return NV_ERR_PRIV_SEC_VIOLATION - PTIMER_TIME is read-only.
 */
NV_STATUS
tmrSetCurrentTime_GM107
(
    OBJGPU *pGpu,
    OBJTMR *pTmr
)
{
    NvU64   ns;                     // Time since 1970 in ns.
    NvU32   seconds;                // Time since 1970 in seconds
    NvU32   useconds;               //  and uSeconds.

    //
    // Get current time from operating system.
    //
    // We get the time in seconds and microseconds since 1970
    // Note that we don't really need the real time of day
    //
    osGetCurrentTime(&seconds, &useconds);

    NV_PRINTF(LEVEL_INFO,
              "osGetCurrentTime returns 0x%x seconds, 0x%x useconds\n",
              seconds, useconds);

    //
    // Calculate ns since 1970.
    //
    ns = ((NvU64)seconds * 1000000 + useconds) * 1000;

    //
    // TIME_0 must always come 2nd.  On Maxwell and later writing TIME_0 is
    // the trigger to load the new time.
    //
    GPU_REG_WR32(pGpu, NV_PTIMER_TIME_1, NvU64_HI32(ns));
    GPU_REG_WR32(pGpu, NV_PTIMER_TIME_0, NvU64_LO32(ns));

    // Mark that time has been initialized
    pTmr->bInitialized = NV_TRUE;

    return NV_OK;
}

#define NV_NUM_PTIMER_SAMPLES 3

NV_STATUS
tmrGetGpuAndCpuTimestampPair_GM107
(
    OBJGPU  *pGpu,
    OBJTMR  *pTmr,
    NvU64   *pGpuTime,
    NvU64   *pCpuTime
)
{
#if PORT_IS_FUNC_SUPPORTED(portUtilExReadTimestampCounter)
    NvU32 gpuTimeLo[NV_NUM_PTIMER_SAMPLES];
    NvU64 cpuTime[NV_NUM_PTIMER_SAMPLES+1];
    NvU64 min;
    NvU32 closestPairBeginIndex;
    NvU32 gpuTimeHiOld;
    NvU32 gpuTimeHiNew;
    NvU32 i;
    NvU32 gpuTimestampOffsetLo = 0;
    NvU32 gpuTimestampOffsetHi = 0;

    // We take (hardcoded) 3 gpu timestamps.
    ct_assert(NV_NUM_PTIMER_SAMPLES == 3);

    tmrGetGpuPtimerOffset_HAL(pGpu, pTmr, &gpuTimestampOffsetLo, &gpuTimestampOffsetHi);

    gpuTimeHiNew = osGpuReadReg032(pGpu, gpuTimestampOffsetHi);
    do
    {
        portAtomicTimerBarrier();
        gpuTimeHiOld = gpuTimeHiNew;
        cpuTime[0] = portUtilExReadTimestampCounter();
        portAtomicTimerBarrier();
        gpuTimeLo[0] = osGpuReadReg032(pGpu, gpuTimestampOffsetLo);
        portAtomicTimerBarrier();
        cpuTime[1] = portUtilExReadTimestampCounter();
        portAtomicTimerBarrier();
        gpuTimeLo[1] = osGpuReadReg032(pGpu, gpuTimestampOffsetLo);
        portAtomicTimerBarrier();
        cpuTime[2] = portUtilExReadTimestampCounter();
        portAtomicTimerBarrier();
        gpuTimeLo[2] = osGpuReadReg032(pGpu, gpuTimestampOffsetLo);
        portAtomicTimerBarrier();
        cpuTime[3] = portUtilExReadTimestampCounter();
        portAtomicTimerBarrier();
        // Read TIME_1 again to detect wrap around.
        gpuTimeHiNew = osGpuReadReg032(pGpu, gpuTimestampOffsetHi);
    } while (gpuTimeHiNew != gpuTimeHiOld);

    //
    // find i such that cpuTime[i+1] - cpuTime[i] is minimum i.e.
    // the find closest pair of cpuTime.
    //
    min = cpuTime[1] - cpuTime[0];
    closestPairBeginIndex = 0;
    for (i = 0; i < NV_NUM_PTIMER_SAMPLES; i++)
    {
        if ((cpuTime[i+1] - cpuTime[i]) < min)
        {
            closestPairBeginIndex = i;
            min = cpuTime[i+1] - cpuTime[i];
        }
    }

    *pGpuTime = (((NvU64)gpuTimeHiNew) << 32) | gpuTimeLo[closestPairBeginIndex];
    *pCpuTime = (cpuTime[closestPairBeginIndex] + cpuTime[closestPairBeginIndex + 1])/2;
    return NV_OK;
#else
    return NV_ERR_NOT_SUPPORTED;
#endif

}

/*!
 * @brief Gets GPU PTIMER offsets
 *
 */
NV_STATUS
tmrGetGpuPtimerOffset_GM107
(
    OBJGPU  *pGpu,
    OBJTMR  *pTmr,
    NvU32   *pGpuTimestampOffsetLo,
    NvU32   *pGpuTimestampOffsetHi
)
{
    if (pGpuTimestampOffsetLo)
    {
        *pGpuTimestampOffsetLo = NV_PTIMER_TIME_0;
    }
    if (pGpuTimestampOffsetHi)
    {
        *pGpuTimestampOffsetHi = NV_PTIMER_TIME_1;
    }

    return NV_OK;;
}

/*!
 * @brief Change GR Tick frequency to either default or max.
 *
 * @param[in]   OBJGPU    *GPU Object
 * @param[in]   OBJTMR    *Timer Object
 * @param[in]   NvBool     Whether to set freq to Max or not.
 *
 * @returns NV_OK on success.
 */
NV_STATUS
tmrGrTickFreqChange_GM107
(
    OBJGPU  *pGpu,
    OBJTMR  *pTmr,
    NvBool   bSetMax
)
{
    NvU32 grTickFreq;

    grTickFreq = GPU_REG_RD32(pGpu, NV_PTIMER_GR_TICK_FREQ);
    if (bSetMax)
    {
        grTickFreq = FLD_SET_DRF(_PTIMER, _GR_TICK_FREQ, _SELECT, _MAX, grTickFreq);
    }
    else
    {
        grTickFreq = FLD_SET_DRF(_PTIMER, _GR_TICK_FREQ, _SELECT, _DEFAULT, grTickFreq);
    }

    GPU_REG_WR32(pGpu, NV_PTIMER_GR_TICK_FREQ, grTickFreq);

    return NV_OK;
}

//
// From dev_timer.ref
//
// When reading the TIME, TIME_1 should be read first, followed by TIME_0, then
// a second reading of TIME_1 should be done.  If the two readings of TIME_1 do
// not agree, this process should be repeated.
//
NvU64
tmrGetTimeEx_GM107
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
            DBG_BREAKPOINT();

            // count # of times we reset PTIMER
            pTmr->errorCount++;

            // load timer state (this will restore TIME_0 and TIME_1 to OS provided values)
            tmrStateInitLocked(pGpu, pTmr);
            tmrStateLoad(pGpu, pTmr, GPU_STATE_DEFAULT);

            return 0;
        }

        // Read TIME_1 again to detect wrap around.
        TimeHi2 = tmrReadTimeHiReg_HAL(pGpu, pTmr, pThreadState);
    } while (TimeHi != TimeHi2);

    // Convert to 64b
    Time = (((NvU64)TimeHi) << 32) | TimeLo;

    return Time;
}

//
// For functions that only need a short delta of time elapsed (~ 4.29 seconds)
// NOTE: Since it wraps around every 4.29 seconds, for general GetTime purposes,
//       it's better to use tmrGetTime().
//
NvU32
tmrGetTimeLo_GM107
(
    OBJGPU *pGpu,
    OBJTMR *pTmr
)
{
    NvU32 lo = 0;

    // read TIME_0
    lo = tmrReadTimeLoReg_HAL(pGpu, pTmr, NULL);
    //
    // check if it's a stable TIME_0, otherwise, we just call a regular
    // tmrGetTime to handle all error book-keeping, resetting timer, etc.
    //
    if ((lo & ~DRF_SHIFTMASK(NV_PTIMER_TIME_0_NSEC)) != 0)
    {
        // let tmrGetTime() handle all the mess..
        NV_PRINTF(LEVEL_WARNING,
                  "NVRM-RC: Bad TimeLo value %x, Let's see if it happens again.\n",
                  lo);
        DBG_BREAKPOINT();

        return (NvU32) (tmrGetTime_HAL(pGpu, pTmr) & 0xffffffff);
    }
    return lo;
}

NvU64
tmrGetTime_GM107
(
    OBJGPU             *pGpu,
    OBJTMR             *pTmr
)
{
    return tmrGetTimeEx_HAL(pGpu, pTmr, NULL);
}

/**
 * @brief Services the stall interrupt.
 *
 * @param[in] pGpu
 * @param[in] pTmr
 * @param[in] pParams
 *
 * @returns Zero, or any implementation-chosen nonzero value. If the same nonzero value is returned enough
 *          times the interrupt is considered stuck.
 */
NvU32
tmrServiceInterrupt_GM107
(
    OBJGPU *pGpu,
    OBJTMR *pTmr,
    IntrServiceServiceInterruptArguments *pParams
)
{
    NvU32 retVal = 0;
    NV_ASSERT_OR_RETURN(pParams != NULL, 0);

    // Service the countdown timer interrupt.
    tmrServiceSwrlCallbacksPmcTree(pGpu, pTmr, NULL);

    // Service the timer alarm interrupt.
    if (tmrGetCallbackInterruptPending(pGpu, pTmr))
    {
        tmrResetCallbackInterrupt(pGpu, pTmr);

        // service both normal and self-rescheduling lists
        (void)tmrCallExpiredCallbacks(pGpu, pTmr);

        // If there are no remaining callbacks then disable the PTIMER interrupt.
        if (pTmr->pRmActiveEventList == NULL)
        {
            // Last one is gone. Disable PTIMER interrupt.
            tmrRmCallbackIntrDisable(pTmr, pGpu);
        }
    }

    tmrGetIntrStatus_HAL(pGpu, pTmr, &retVal, NULL);

    return retVal;
}
