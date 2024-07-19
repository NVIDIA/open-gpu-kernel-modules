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
*   The TU102 specific HAL TMR routines reside in this file.                *
*                                                                           *
\***************************************************************************/
/* ------------------------- Includes --------------------------------------- */
#include "gpu/gpu.h"
#include "gpu/timer/objtmr.h"
#include "published/turing/tu102/dev_vm.h"
/* ------------------------- Datatypes -------------------------------------- */
/* ------------------------- Macros ----------------------------------------- */
/* ------------------------- Static Function Prototypes --------------------- */
/* ------------------------- Public Functions  ------------------------------ */


NV_STATUS
tmrSetCountdown_TU102
(
    OBJGPU            *pGpu,
    OBJTMR            *pTmr,
    NvU32              time,
    NvU32              tmrId,
    THREAD_STATE_NODE *pThreadState
)
{
    GPU_VREG_WR32_EX(pGpu, NV_VIRTUAL_FUNCTION_PRIV_TIMER, time, pThreadState);

    return NV_OK;
}

NV_STATUS
tmrSetCountdownIntrEnable_TU102
(
    OBJGPU *pGpu,
    OBJTMR *pTmr
)
{
    //
    // The countdown timer interrupt has moved to NV_CTRL on Turing and hence,
    // we will never enable the interrupt in NV_PTIMER_INTR_EN_0 which controls
    // whether the interrupt is reported into the NV_PMC_INTR tree. We have not
    // assigned a STUB to the HAL to allow pre-GM20X versions to return an
    // error if called on those chips.
    //

    return NV_OK;
}

NV_STATUS
tmrSetCountdownIntrReset_TU102
(
    OBJGPU             *pGpu,
    OBJTMR             *pTmr,
    THREAD_STATE_NODE  *pThreadState
)
{
    //
    // The countdown timer interrupt has moved to NV_CTRL on Turing and will be
    // reset directly in NV_CTRL. We have not assigned a STUB to the HAL to
    // llow pre-GM20X versions to return an error if called on those chips.
    //
    return NV_OK;
}


/*!
 * @brief Gets GPU PTIMER offsets
 *
 */
NV_STATUS
tmrGetGpuPtimerOffset_TU102
(
    OBJGPU  *pGpu,
    OBJTMR  *pTmr,
    NvU32   *pGpuTimestampOffsetLo,
    NvU32   *pGpuTimestampOffsetHi
)
{
    extern NV_STATUS tmrGetGpuPtimerOffset_GV100(OBJGPU *pGpu, OBJTMR *pTmr, NvU32 *pGpuTimestampOffsetLo, NvU32 *pGpuTimestampOffsetHi);
    NvU32 ptimerOffsetLo = 0;
    NvU32 ptimerOffsetHi = 0;

    if (IS_VIRTUAL_WITH_SRIOV(pGpu))
    {
        ptimerOffsetLo = NV_VIRTUAL_FUNCTION_TIME_0;
        ptimerOffsetHi = NV_VIRTUAL_FUNCTION_TIME_1;
    }
    else
    {
        return tmrGetGpuPtimerOffset_GV100(pGpu, pTmr, pGpuTimestampOffsetLo, pGpuTimestampOffsetHi);
    }

    if (pGpuTimestampOffsetLo)
    {
        *pGpuTimestampOffsetLo = ptimerOffsetLo;
    }

    if (pGpuTimestampOffsetHi)
    {
        *pGpuTimestampOffsetHi = ptimerOffsetHi;
    }

    return NV_OK;
}

/*
 * @brief This function returns the PTIMER_TIME_0 register. This function will
 *  work for both Physical function and virtual function in SR-IOV.
 *
 *  @return NvU32
 */
NvU32
tmrReadTimeLoReg_TU102
(
    OBJGPU             *pGpu,
    OBJTMR             *pTmr,
    THREAD_STATE_NODE  *pThreadState
)
{
    return GPU_VREG_RD32_EX(pGpu, NV_VIRTUAL_FUNCTION_TIME_0, pThreadState);
}

/*
 * @brief This function returns the PTIMER_TIME_1 register. This function will
 *  work for both Physical function and virtual function in SR-IOV.
 *
 *  @return NvU32
 */
NvU32
tmrReadTimeHiReg_TU102
(
    OBJGPU             *pGpu,
    OBJTMR             *pTmr,
    THREAD_STATE_NODE  *pThreadState
)
{
    return GPU_VREG_RD32_EX(pGpu, NV_VIRTUAL_FUNCTION_TIME_1, pThreadState);
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
tmrServiceInterrupt_TU102
(
    OBJGPU *pGpu,
    OBJTMR *pTmr,
    IntrServiceServiceInterruptArguments *pParams
)
{
    extern NvU32 tmrServiceInterrupt_GM107(OBJGPU *pGpu, OBJTMR *pTmr, IntrServiceServiceInterruptArguments *pParams);
    NV_ASSERT_OR_RETURN(pParams != NULL, 0);
    MODS_ARCH_REPORT(NV_ARCH_EVENT_PTIMER, "%s", "processing ptimer interrupt\n");

    // Service countdown timer interrupts
    (void)tmrCallExpiredCallbacks(pGpu, pTmr);

    // Call the legacy timer service routine to service alarm interrupts
    return tmrServiceInterrupt_GM107(pGpu, pTmr, pParams);
}
