/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#define NVOC_OBJTMR_H_PRIVATE_ACCESS_ALLOWED
/* ------------------------- Includes --------------------------------------- */
#include "gpu/gpu.h"
#include "gpu/timer/objtmr.h"

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
NV_STATUS tmrSetCurrentTime_GB10B
(
    OBJGPU *pGpu,
    OBJTMR *pTmr
)
{
    NvU64 osTimeNs, gpuTimerNs, sysTimerOffsetNs;
    NvU32 gpuTimerLo, gpuTimerHi, gpuTimerHi2;
    NvU32 seconds;
    NvU32 useconds;

    osGetSystemTime(&seconds, &useconds);

    osTimeNs = ((NvU64)seconds * 1000000 + useconds) * 1000;

    //
    // Get the current secure timer value to calculate the offset to apply
    // Use hi-lo-hi reading to ensure a consistent value.
    //
    gpuTimerHi2 = tmrReadTimeHiReg_HAL(pGpu, pTmr, NULL);
    do
    {
        gpuTimerHi  = gpuTimerHi2;
        gpuTimerLo  = tmrReadTimeLoReg_HAL(pGpu, pTmr, NULL);
        gpuTimerHi2 = tmrReadTimeHiReg_HAL(pGpu, pTmr, NULL);
    } while (gpuTimerHi != gpuTimerHi2);
    gpuTimerNs = gpuTimerLo | (((NvU64)gpuTimerHi) << 32);

    NV_ASSERT_OR_RETURN(gpuTimerNs < osTimeNs, NV_ERR_INVALID_STATE);
    sysTimerOffsetNs = osTimeNs - gpuTimerNs;

    pTmr->sysTimerOffsetNs = sysTimerOffsetNs;

    return NV_OK;
}

NvU64
tmrGetPtimerOffsetNs_GB10B
(
    OBJGPU *pGpu,
    OBJTMR *pTmr
)
{
    return pTmr->sysTimerOffsetNs;
}
