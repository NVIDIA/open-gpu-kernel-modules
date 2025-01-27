/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************************************************************
*
*       Kernel Display Module
*       This file contains functions managing display on CPU RM
*
******************************************************************************/

#define RM_STRICT_CONFIG_EMIT_DISP_ENGINE_DEFINITIONS     0

#include "os/os.h"
#include "gpu/gpu.h"
#include "gpu/disp/kern_disp.h"
#include "disp/v04_01/dev_disp.h"
#include "platform/sli/sli.h"
#include "class/clc370.h"

NvBool
kdispReadPendingWinSemIntr_v04_01
(
    OBJGPU *pGpu,
    KernelDisplay *pKernelDisplay,
    THREAD_STATE_NODE *pThreadState,
    NvU32 *pCachedIntr
)
{
    NvU32 intr = pCachedIntr ? *pCachedIntr :
                 GPU_REG_RD32_EX(pGpu, NV_PDISP_FE_RM_INTR_STAT_CTRL_DISP, pThreadState);
    return FLD_TEST_DRF(_PDISP, _FE_RM_INTR_STAT_CTRL_DISP, _WIN_SEM, _PENDING, intr);
}

void
kdispHandleWinSemEvt_v04_01
(
    OBJGPU *pGpu,
    KernelDisplay *pKernelDisplay,
    THREAD_STATE_NODE *pThreadState
)
{
    NvU32 data32 = 0; 
    NvU32 winSemEvtMask = 0;
    NvU32 win;
 
    if (!kdispReadPendingWinSemIntr_HAL(pGpu, pKernelDisplay, pThreadState, NULL))
        return;

    data32 = GPU_REG_RD32_EX(pGpu, NV_PDISP_FE_EVT_DISPATCH, pThreadState); 
    if (FLD_TEST_DRF(_PDISP, _FE_EVT_DISPATCH, _SEM_WIN, _NOT_PENDING, data32))
        return;

    data32 = GPU_REG_RD32_EX(pGpu, NV_PDISP_FE_EVT_STAT_SEM_WIN, pThreadState);

    for (win = 0; win < NV_PDISP_FE_EVT_STAT_SEM_WIN_CH__SIZE_1; win++)
    {
        if (FLD_IDX_TEST_DRF(_PDISP, _FE_EVT_STAT_SEM_WIN, _CH, win, _PENDING, data32))
        {
            winSemEvtMask |= NVBIT(win);
        }
    }

    //Clear Event
    osDispService(NV_PDISP_FE_EVT_STAT_SEM_WIN, winSemEvtMask);
    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY);
        GPU_REG_WR32_EX(pGpu, NV_PDISP_FE_EVT_STAT_SEM_WIN, winSemEvtMask, pThreadState);
    SLI_LOOP_END;

    //TODO: Notify DD about the event,
    kdispNotifyEvent(pGpu, pKernelDisplay, NVC370_NOTIFIERS_WIN_SEM_NOTIFICATION, &winSemEvtMask, sizeof(winSemEvtMask), 0, 0);
}
