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
*       This file contains functions managing DispSfUser class.
*
******************************************************************************/

#define RM_STRICT_CONFIG_EMIT_DISP_ENGINE_DEFINITIONS     0

#include "resserv/resserv.h"

#include "gpu/gpu.h"
#include "gpu/disp/disp_sf_user.h"
#include "gpu/disp/kern_disp.h"

NV_STATUS
dispsfConstruct_IMPL
(
    DispSfUser *pDispSfUser,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDispSfUser);
    KernelDisplay *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);

    if (pParams->pSecInfo->privLevel < RS_PRIV_LEVEL_USER_ROOT)
    {
        NV_PRINTF(LEVEL_ERROR, 
                  "Failure allocating display class 0x%08x: Only root(admin)/kernel clients are allowed\n", 
                  pParams->externalClassId);

        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    NV_CHECK_OR_RETURN(LEVEL_ERROR, pKernelDisplay != NULL, NV_ERR_NOT_SUPPORTED);

    // Set sf user RegBase offset
    kdispGetDisplaySfUserBaseAndSize_HAL(pGpu, pKernelDisplay,
                                         &pDispSfUser->ControlOffset,
                                         &pDispSfUser->ControlLength);

    return NV_OK;
}

NV_STATUS
dispsfGetRegBaseOffsetAndSize_IMPL
(
    DispSfUser *pDispSfUser,
    OBJGPU *pGpu,
    NvU32* pOffset,
    NvU32* pSize
)
{
    if (pOffset)
    {
        *pOffset = pDispSfUser->ControlOffset;
    }

    if (pSize)
    {
        *pSize = pDispSfUser->ControlLength;
    }

    return NV_OK;
}
