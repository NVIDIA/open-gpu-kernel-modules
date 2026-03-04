/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
*   Description:
*       This file contains functions managing DispCapabilities class.
*
******************************************************************************/

#define RM_STRICT_CONFIG_EMIT_DISP_ENGINE_DEFINITIONS     0

#include "resserv/resserv.h"

#include "gpu/gpu.h"
#include "gpu/disp/disp_capabilities.h"
#include "gpu/disp/kern_disp.h"

NV_STATUS
dispcapConstruct_IMPL
(
    DispCapabilities             *pDispCapabilities,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDispCapabilities);
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);

    // Set display caps RegBase offsets
    kdispGetDisplayCapsBaseAndSize_HAL(pGpu, pKernelDisplay,
                                       &pDispCapabilities->ControlOffset,
                                       &pDispCapabilities->ControlLength);

    return NV_OK;
}

NV_STATUS
dispcapGetRegBaseOffsetAndSize_IMPL
(
    DispCapabilities *pDispCapabilities,
    OBJGPU *pGpu,
    NvU32 *pOffset,
    NvU32 *pSize
)
{
    if (pOffset)
    {
        *pOffset = pDispCapabilities->ControlOffset;
    }
    if (pSize)
    {
        *pSize = pDispCapabilities->ControlLength;
    }

    return NV_OK;
}
