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
#include "disp/v04_01/dev_disp.h"
#include "platform/sli/sli.h"
#include "class/clc370.h"
#include "ctrl/ctrlc370/ctrlc370event.h"
#include "gpu/disp/kern_disp.h"
#include "gpu/disp/head/kernel_head.h"

void
kheadReadPendingRgSemIntr_v04_01
(
    OBJGPU            *pGpu,
    KernelHead        *pKernelHead,
    HEADINTRMASK      *pHeadIntrMask,
    THREAD_STATE_NODE *pThreadState,
    NvU32             *pCachedIntr
)
{
    NvU32 i;
    NvU32 intr = pCachedIntr ? *pCachedIntr :
                 GPU_REG_RD32_EX(pGpu, NV_PDISP_FE_RM_INTR_STAT_HEAD_TIMING(pKernelHead->PublicId), pThreadState);

    for (i = 0; i < headIntr_RgSem__SIZE_1; ++i)
    {
        if (FLD_IDX_TEST_DRF(_PDISP, _FE_EVT_STAT_HEAD_TIMING, _RG_SEM, i, _PENDING, intr))
        {
            *pHeadIntrMask |= headIntr_RgSem(i);
        }
    }
}

void
kheadHandleRgSemIntr_v04_01
(
    OBJGPU            *pGpu,
    KernelHead        *pKernelHead,
    HEADINTRMASK      *pHeadIntrMask,
    THREAD_STATE_NODE *pThreadState
)
{
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    NvU32          rgSemIndex, intr;

    NVC370_RG_SEM_NOTIFICATION_PARAMS params = {0};

    for (rgSemIndex = 0; rgSemIndex < headIntr_RgSem__SIZE_1; ++rgSemIndex)
    {
        if (*pHeadIntrMask & headIntr_RgSem(rgSemIndex))
        {
            intr = DRF_IDX_DEF(_PDISP, _FE_EVT_STAT_HEAD_TIMING, _RG_SEM, rgSemIndex, _RESET);

            osDispService(NV_PDISP_FE_EVT_STAT_HEAD_TIMING(pKernelHead->PublicId), intr);
            SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY);
                GPU_REG_WR32_EX(pGpu, NV_PDISP_FE_EVT_STAT_HEAD_TIMING(pKernelHead->PublicId), intr, pThreadState);
            SLI_LOOP_END;
            
            *pHeadIntrMask &= (~headIntr_RgSem(rgSemIndex));

            params.headId   = pKernelHead->PublicId;
            params.rgSemId  = rgSemIndex;

            kdispNotifyEvent(pGpu, pKernelDisplay, NVC370_NOTIFIERS_RG_SEM_NOTIFICATION, &params, sizeof(params), 0, 0);
        }
    }
}
