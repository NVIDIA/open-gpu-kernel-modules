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

/******************************************************************************
*
*       Kernel Display Module
*       This file contains functions managing display on CPU RM
*
******************************************************************************/

#define RM_STRICT_CONFIG_EMIT_DISP_ENGINE_DEFINITIONS     0

#include "gpu/disp/vblank_callback/vblank.h"
#include "gpu/disp/kern_disp.h"

void
kheadVsyncNotificationOverRgVblankIntr_v04_04
(
    OBJGPU              *pGpu,
    KernelHead          *pKernelHead
)
{
    KernelDisplay   *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    VBLANKCALLBACK  *pCallback = (VBLANKCALLBACK *)pKernelDisplay->pRgVblankCb;
    if (pKernelDisplay->bIsPanelReplayEnabled)
    {
        if (pCallback != NULL && pCallback->Proc != NULL)
        {
            pCallback->Proc(pGpu, pCallback->pObject, pCallback->Param1,
                            pCallback->Param2,pCallback->Status);
        }
    }

}

