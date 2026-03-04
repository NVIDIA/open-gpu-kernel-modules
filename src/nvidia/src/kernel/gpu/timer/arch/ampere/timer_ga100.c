/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
*   The GA100 specific HAL TMR routines reside in this file.                *
*                                                                           *
\***************************************************************************/
/* ------------------------- Includes --------------------------------------- */
#include "gpu/gpu.h"
#include "gpu/timer/objtmr.h"
#include "published/ampere/ga100/dev_vm.h"
/* ------------------------- Datatypes -------------------------------------- */
/* ------------------------- Macros ----------------------------------------- */
/* ------------------------- Static Function Prototypes --------------------- */
/* ------------------------- Public Functions  ------------------------------ */

/*!
 * @brief Gets GPU PTIMER offsets
 *
 */
NV_STATUS
tmrGetGpuPtimerOffset_GA100
(
    OBJGPU  *pGpu,
    OBJTMR  *pTmr,
    NvU32   *pGpuTimestampOffsetLo,
    NvU32   *pGpuTimestampOffsetHi
)
{
    if (IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        *pGpuTimestampOffsetLo = NV_VIRTUAL_FUNCTION_TIME_0;
        *pGpuTimestampOffsetHi = NV_VIRTUAL_FUNCTION_TIME_1;
    }
    else
    {
        *pGpuTimestampOffsetLo = GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_TIME_0);
        *pGpuTimestampOffsetHi = GPU_GET_VREG_OFFSET(pGpu, NV_VIRTUAL_FUNCTION_TIME_1);
    }

    return NV_OK;
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
tmrServiceInterrupt_GA100
(
    OBJGPU *pGpu,
    OBJTMR *pTmr,
    IntrServiceServiceInterruptArguments *pParams
)
{
    NV_ASSERT_OR_RETURN(pParams != NULL, 0);

    switch (pParams->engineIdx)
    {
        case MC_ENGINE_IDX_TMR:
        {
            MODS_ARCH_REPORT(NV_ARCH_EVENT_PTIMER, "%s", "processing ptimer interrupt\n");

            // Service countdown timer interrupts
            (void)tmrCallExpiredCallbacks(pGpu, pTmr);

            break;
        }
        case MC_ENGINE_IDX_TMR_SWRL:
        {
            tmrServiceSwrlCallbacks(pGpu, pTmr, NULL);
            break;
        }
        default:
        {
            NV_ASSERT_FAILED("Invalid engineIdx");
            break;
        }
    }

    return 0;
}
