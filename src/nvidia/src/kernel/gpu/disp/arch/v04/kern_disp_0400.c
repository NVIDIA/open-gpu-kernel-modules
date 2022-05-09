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

/******************************************************************************
*
*       Kernel Display Module
*       This file contains functions managing display on CPU RM
*
******************************************************************************/

#define RM_STRICT_CONFIG_EMIT_DISP_ENGINE_DEFINITIONS     0

#include "gpu/gpu.h"
#include "gpu/disp/kern_disp.h"

#include "published/disp/v04_00/dev_disp.h"

/*!
 * @brief Get the VGA workspace base address, if valid.
 *
 */
NvBool
kdispGetVgaWorkspaceBase_v04_00
(
    OBJGPU *pGpu,
    KernelDisplay *pKernelDisplay,
    NvU64 *pOffset
)
{
    NvU32 vgaReg = GPU_REG_RD32(pGpu, NV_PDISP_VGA_WORKSPACE_BASE);

    if (FLD_TEST_DRF(_PDISP, _VGA_WORKSPACE_BASE, _STATUS, _VALID, vgaReg))
    {
        *pOffset = GPU_DRF_VAL(_PDISP, _VGA_WORKSPACE_BASE, _ADDR, vgaReg) << 16;
        return NV_TRUE;
    }

    return NV_FALSE;
}
