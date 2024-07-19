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
*   The GM200 specific HAL TMR routines reside in this file.                *
*                                                                           *
\***************************************************************************/
/* ------------------------- Includes --------------------------------------- */
#include "gpu/gpu.h"
#include "gpu/timer/objtmr.h"
#include "published/maxwell/gm200/dev_timer.h"
/* ------------------------- Datatypes -------------------------------------- */
/* ------------------------- Macros ----------------------------------------- */
/* ------------------------- Static Function Prototypes --------------------- */
/* ------------------------- Public Functions  ------------------------------ */

NV_STATUS
tmrSetCountdownIntrDisable_GM200
(
    OBJGPU *pGpu,
    OBJTMR *pTmr
)
{
    NvU32 reg = GPU_REG_RD32(pGpu, NV_PTIMER_INTR_EN_0);

    reg = FLD_SET_DRF(_PTIMER, _INTR_EN_0, _TIMER, _DISABLED, reg);
    GPU_REG_WR32_UC(pGpu, NV_PTIMER_INTR_EN_0, reg);

    return NV_OK;
}

NV_STATUS
tmrSetCountdownIntrEnable_GM200
(
    OBJGPU *pGpu,
    OBJTMR *pTmr
)
{
    NvU32 reg = GPU_REG_RD32(pGpu, NV_PTIMER_INTR_EN_0);

    reg = FLD_SET_DRF(_PTIMER, _INTR_EN_0, _TIMER, _ENABLED, reg);
    GPU_REG_WR32_UC(pGpu, NV_PTIMER_INTR_EN_0, reg);

    return NV_OK;
}

NV_STATUS
tmrSetCountdownIntrReset_GM200
(
    OBJGPU             *pGpu,
    OBJTMR             *pTmr,
    THREAD_STATE_NODE  *pThreadState
)
{
    GPU_REG_WR32_EX(pGpu, NV_PTIMER_INTR_0,
                    DRF_DEF(_PTIMER, _INTR_0, _TIMER, _RESET), pThreadState);
    return NV_OK;
}

NvBool
tmrGetCountdownPending_GM200
(
    OBJGPU             *pGpu,
    OBJTMR             *pTmr,
    THREAD_STATE_NODE  *pThreadState
)
{
    NvU32 reg = GPU_REG_RD32_EX(pGpu, NV_PTIMER_INTR_0, pThreadState);
    return FLD_TEST_DRF(_PTIMER, _INTR_0, _TIMER, _PENDING, reg);
}

NV_STATUS
tmrSetCountdown_GM200
(
    OBJGPU             *pGpu,
    OBJTMR             *pTmr,
    NvU32               timer,
    NvU32               tmrId,
    THREAD_STATE_NODE  *pThreadState
)
{
    GPU_REG_WR32_EX(pGpu, NV_PTIMER_TIMER_0, timer, pThreadState);
    return NV_OK;
}

