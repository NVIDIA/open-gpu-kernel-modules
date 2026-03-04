/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
*   The PTimer based routines reside in this file.                          *
*                                                                           *
\***************************************************************************/

/* ------------------------- Includes --------------------------------------- */
#include "gpu/gpu.h"
#include "gpu/timer/objtmr.h"

#include "published/turing/tu104/dev_timer.h"

/* ------------------------- Datatypes -------------------------------------- */
/* ------------------------- Macros ----------------------------------------- */
/* ------------------------- Static Function Prototypes --------------------- */
/* ------------------------- Public Functions  ------------------------------ */
//
// NV_PTIMER_TIME_0 rolls over approx every 4 secs. For delays
// less than 1/4 of that time just compare against TIME_0
//
NV_STATUS tmrDelay_PTIMER
(
     OBJTMR    *pTmr,
     NvU32      nsec
)
{
    NvU64   Time;
    NvU32   start = 0;
    OBJGPU *pGpu = ENG_GET_GPU(pTmr);
    NvU32   count = 0;

    // If GPU is gone, we can't use timer at all. Delay also doesn't make any sense.
    if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
    {
        return NV_ERR_GPU_IS_LOST;
    }
    if (API_GPU_IN_RESET_SANITY_CHECK(pGpu))
    {
        return NV_ERR_GPU_IN_FULLCHIP_RESET;
    }

    if (gpuIsReplayableTraceEnabled(pGpu)) {
        NV_PRINTF(LEVEL_ERROR, "Entered tmrDelay  - %d\n", nsec);
    }

    //
    // On NV50 fmodel, a large delay (1 ms) will cause the simulator
    // to abort due to lack of activity.
    if (IS_SIMULATION(pGpu) && (nsec > 100000))
    {
        nsec = 100000;
    }

    if (nsec == 0)
        return NV_OK;

    if (nsec > 50000000)    // 50 ms.
    {
        //
        // Provide control back to os scheduler so as not to
        // starve other processes/tasks in the system, etc...
        //
        NV_PRINTF(LEVEL_ERROR,
                  "Too long delay w/o yield, use osDelay instead.\n");
    }

    //
    // Check to make sure the numerator / denominator have been programmed up.
    // The power on reset values for both are 0
    //
    // for short delays (delays less than 12 us, known to be the smallest delay
    // tmrDelay known to provide - bug  478366), we call tmrGetTimeLo instead of
    // tmrGetTime as it's sufficient to check within one round of 4.29 s time frame.
    //
    if (nsec < 12000) // 12 us
    {
        // start with the current time, reading only low half
        start = tmrGetTimeLo_HAL(pGpu, pTmr);
        while (tmrGetTimeLo_HAL(pGpu, pTmr) - start < nsec)
        {
            //
            // Determine if PTIMER is not moving at all. The math below assumes that register accesses
            // take at least 1/16 usec from CPU (which is very conservative compared to reality).
            // Note that tmrGetTimeLo reads at least one register.
            //
            if ((count >> 4) > ((nsec + 999) / 1000))
            {
                NV_PRINTF(LEVEL_INFO,
                          "PTIMER may be stuck. Already at %d iterations for a delay of %d nsec\n",
                          count, nsec);
            }
            else if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
            {
                return NV_ERR_GPU_IS_LOST;
            }
            else
            {
                count++;
            }
        }
    }
    else
    {
        // Start with the current time.
        Time = tmrGetTime_HAL(pGpu, pTmr);

        // Add nanosecond delay.
        Time += nsec;

        while (Time > tmrGetTime_HAL(pGpu, pTmr))
        {
            //
            // Determine if PTIMER is not moving at all. The math below assumes that register accesses
            // take at least 1/16 usec from CPU (which is very conservative compared to reality).
            // Note that tmrGetTime reads at least two registers.
            //
            if ((count >> 3) > ((nsec + 999) / 1000))
            {
                NV_PRINTF(LEVEL_INFO,
                          "PTIMER may be stuck. Already at %d iterations for a delay of %d nsec\n",
                          count, nsec);
            }
            else if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
            {
                return NV_ERR_GPU_IS_LOST;
            }
            else
            {
                count++;
            }
            osSpinLoop();
        }
    }

    if (gpuIsReplayableTraceEnabled(pGpu)) {
        NV_PRINTF(LEVEL_ERROR, "Exiting tmrDelay\n");
    }

    return NV_OK;
}

/*!
 * @brief Returns the BAR0 offset and size of the PTIMER range.
 *
 * @param[in]  pGpu
 * @param[in]  pTmr
 * @param[out] pBar0MapOffset
 * @param[out] pBar0MapSize
 *
 * @return NV_STATUS
 */
NV_STATUS
tmrGetTimerBar0MapInfo_PTIMER
(
    OBJGPU  *pGpu,
    OBJTMR  *pTmr,
    NvU64   *pBar0MapOffset,
    NvU32   *pBar0MapSize
)
{
    *pBar0MapOffset = DRF_BASE(NV_PTIMER);
    *pBar0MapSize   = DRF_SIZE(NV_PTIMER);

    return NV_OK;
}

