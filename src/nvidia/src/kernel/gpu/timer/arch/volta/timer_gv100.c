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
/********************* Chip Specific HAL TMR Routines **********************\
*                                                                           *
*   The GV100 specific HAL TMR routines reside in this file.                *
*                                                                           *
\***************************************************************************/
/* ------------------------- Includes --------------------------------------- */
#include "gpu/gpu.h"
#include "gpu/timer/objtmr.h"
#include "published/volta/gv100/dev_timer.h"
#include "published/volta/gv100/dev_usermode.h"
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
tmrSetCurrentTime_GV100
(
    OBJGPU *pGpu,
    OBJTMR *pTmr
)
{
    NV_STATUS status;

    // We can only set the time if level 0 is allowed to write
    if (GPU_FLD_TEST_DRF_DEF(pGpu, _PTIMER, _TIME_PRIV_LEVEL_MASK, _WRITE_PROTECTION_LEVEL0, _ENABLE))
    {
        NvU64 ns;
        NvU32 seconds;
        NvU32 useconds;

        osGetCurrentTime(&seconds, &useconds);

        NV_PRINTF(LEVEL_INFO,
            "osGetCurrentTime returns 0x%x seconds, 0x%x useconds\n",
            seconds, useconds);

        ns = ((NvU64)seconds * 1000000 + useconds) * 1000;

        // Writing TIME_0 is the trigger to load the new time.
        GPU_REG_WR32(pGpu, NV_PTIMER_TIME_1, NvU64_HI32(ns));
        GPU_REG_WR32(pGpu, NV_PTIMER_TIME_0, NvU64_LO32(ns));

        status = NV_OK;
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
                  "ERROR: Write to PTIMER attempted even though Level 0 PLM is disabled.\n");
        NV_ASSERT(0);
        status = NV_ERR_PRIV_SEC_VIOLATION;
    }

    return status;
}

/*!
 * @brief Gets GPU PTIMER offsets
 *
 */
NV_STATUS
tmrGetGpuPtimerOffset_GV100
(
    OBJGPU  *pGpu,
    OBJTMR  *pTmr,
    NvU32   *pGpuTimestampOffsetLo,
    NvU32   *pGpuTimestampOffsetHi
)
{
    extern NV_STATUS tmrGetGpuPtimerOffset_GM107(OBJGPU *pGpu, OBJTMR *pTmr, NvU32 *pGpuTimestampOffsetLo, NvU32 *pGpuTimestampOffsetHi);

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_NV_USERMODE_ENABLED))
    {
        *pGpuTimestampOffsetLo = NV_USERMODE_TIME_0;
        *pGpuTimestampOffsetHi = NV_USERMODE_TIME_1;
    }
    else
        return tmrGetGpuPtimerOffset_GM107(pGpu, pTmr, pGpuTimestampOffsetLo, pGpuTimestampOffsetHi);

    return NV_OK;
}
