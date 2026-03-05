/*
 * SPDX-FileCopyrightText: Copyright (c) 2012-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/system.h"
#include "os/os.h"
#include "rmapi/control.h"
#include "griddisplayless/objgriddisplayless.h"
#include "griddisplayless/griddisplayless.h"

NV_STATUS
griddisplaylessCtrlCmdVirtualDisplayGetNumHeads_IMPL
(
    Griddisplayless *pGridDisplaylessApi,
    NVA083_CTRL_VIRTUAL_DISPLAY_GET_NUM_HEADS_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pGridDisplaylessApi);
    OBJGRIDDISPLAYLESS *pGridDisplayless = GPU_GET_GRIDDISPLAYLESS(pGpu);

    pParams->numHeads       = griddisplaylessGetNumHeads(pGridDisplayless);
    pParams->maxNumHeads    = griddisplaylessGetMaxNumHeads(pGridDisplayless);

    return NV_OK;
}

NV_STATUS
griddisplaylessCtrlCmdVirtualDisplayGetMaxResolution_IMPL
(
    Griddisplayless *pGridDisplaylessApi,
    NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_RESOLUTION_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pGridDisplaylessApi);
    OBJGRIDDISPLAYLESS *pGridDisplayless = GPU_GET_GRIDDISPLAYLESS(pGpu);
    NV_STATUS rmStatus = NV_OK;

    if (pParams->headIndex >= griddisplaylessGetNumHeads(pGridDisplayless))
    {
        rmStatus = NV_ERR_INVALID_ARGUMENT;
        goto done;
    }

    pParams->maxHResolution = griddisplaylessGetMaxHResolution(pGridDisplayless);
    pParams->maxVResolution = griddisplaylessGetMaxVResolution(pGridDisplayless);

done:
    return rmStatus;
}

NV_STATUS
griddisplaylessCtrlCmdVirtualDisplayGetDefaultEdid_IMPL
(
    Griddisplayless *pGridDisplaylessApi,
    NVA083_CTRL_VIRTUAL_DISPLAY_GET_DEFAULT_EDID_PARAMS *pEdidParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pGridDisplaylessApi);
    OBJGRIDDISPLAYLESS *pGridDisplayless = GPU_GET_GRIDDISPLAYLESS(pGpu);
    NvU8 *pOutBuffer = NvP64_VALUE(pEdidParams->pEdidBuffer);

    return (griddisplaylessGetDefaultEDID(pGridDisplayless, pEdidParams->connectorType, &(pEdidParams->edidSize), pOutBuffer));
}

NV_STATUS
griddisplaylessCtrlCmdVirtualDisplayIsActive_IMPL
(
    Griddisplayless *pGridDisplaylessApi,
    NVA083_CTRL_VIRTUAL_DISPLAY_IS_ACTIVE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pGridDisplaylessApi);
    OBJGRIDDISPLAYLESS *pGridDisplayless = GPU_GET_GRIDDISPLAYLESS(pGpu);

    pParams->isDisplayActive = griddisplaylessVirtualDisplayIsActive(pGridDisplayless);

    return NV_OK;
}

NV_STATUS
griddisplaylessCtrlCmdVirtualDisplayIsConnected_IMPL
(
    Griddisplayless *pGridDisplaylessApi,
    NVA083_CTRL_VIRTUAL_DISPLAY_IS_CONNECTED_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pGridDisplaylessApi);
    OBJGRIDDISPLAYLESS *pGridDisplayless = GPU_GET_GRIDDISPLAYLESS(pGpu);

    pParams->isDisplayConnected = griddisplaylessVirtualDisplayIsConnected(pGridDisplayless);

    return NV_OK;
}

NV_STATUS
griddisplaylessCtrlCmdVirtualDisplayGetMaxPixels_IMPL
(
    Griddisplayless *pGridDisplaylessApi,
    NVA083_CTRL_VIRTUAL_DISPLAY_GET_MAX_PIXELS_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pGridDisplaylessApi);
    OBJGRIDDISPLAYLESS *pGridDisplayless = GPU_GET_GRIDDISPLAYLESS(pGpu);

    pParams->maxPixels  = griddisplaylessGetMaxPixels(pGridDisplayless);

    return NV_OK;
}


