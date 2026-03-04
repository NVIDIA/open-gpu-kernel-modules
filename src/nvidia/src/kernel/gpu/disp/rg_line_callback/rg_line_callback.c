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

#define RM_STRICT_CONFIG_EMIT_DISP_ENGINE_DEFINITIONS     0

#include "gpu/disp/rg_line_callback/rg_line_callback.h"
#include "gpu/disp/kern_disp.h"
#include "gpu/disp/disp_objs.h"
#include "rmapi/client.h"
#include "gpu/gpu.h"

static NV_STATUS
_registerRgLineCallback
(
    RgLineCallback *pRgLineCallback,
    NvBool          bEnableRgLineIntr
)
{
    NV_STATUS      status         = NV_OK;
    OBJGPU        *pGpu           = NULL;
    KernelDisplay *pKernelDisplay;
    RM_API        *pRmApi;
    NvU32          hClient;
    NvU32          hSubdevice;
    NV2080_CTRL_INTERNAL_DISPLAY_SETUP_RG_LINE_INTR_PARAMS ctrlParams;

    if (pRgLineCallback->pCallbkFn == NULL)
    {
        NV_PRINTF(LEVEL_ERROR, "Trying to register/un-register a NULL RG line callback\n");
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Get the right pGpu from subdevice instance given by client
    status = dispapiSetUnicastAndSynchronize_HAL(staticCast(pRgLineCallback->pDispCommon, DisplayApi),
                                             DISPAPI_GET_GPUGRP(pRgLineCallback->pDispCommon),
                                             &pGpu,
                                             NULL,
                                             pRgLineCallback->subDeviceInstance);
    if (status != NV_OK)
        return status;

    pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    NV_ASSERT(pKernelDisplay != NULL);
    pRmApi         = GPU_GET_PHYSICAL_RMAPI(pGpu);
    hClient        = pGpu->hInternalClient;
    hSubdevice     = pGpu->hInternalSubdevice;

    ctrlParams.head = pRgLineCallback->head;
    ctrlParams.rgLineNum = pRgLineCallback->rgLineNum;
    ctrlParams.intrLine = pRgLineCallback->rgIntrLine;
    ctrlParams.bEnable = bEnableRgLineIntr;

    status = pRmApi->Control(pRmApi, hClient, hSubdevice,
                             NV2080_CTRL_CMD_INTERNAL_DISPLAY_SETUP_RG_LINE_INTR,
                             &ctrlParams, sizeof(ctrlParams));

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, status);

    if (bEnableRgLineIntr)
    {
        pRgLineCallback->rgIntrLine = ctrlParams.intrLine;
    }

    status = kdispRegisterRgLineCallback(pKernelDisplay, pRgLineCallback,
               pRgLineCallback->head, pRgLineCallback->rgIntrLine, bEnableRgLineIntr);

    return status;
}

NV_STATUS
rglcbConstruct_IMPL
(
    RgLineCallback               *pRgLineCallback,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV0092_RG_LINE_CALLBACK_ALLOCATION_PARAMETERS *pAllocParams = pParams->pAllocParams;

    pRgLineCallback->subDeviceInstance = pAllocParams->subDeviceInstance;
    pRgLineCallback->head              = pAllocParams->head;
    pRgLineCallback->rgLineNum         = pAllocParams->rgLineNum;
    pRgLineCallback->pCallbkFn         = pAllocParams->pCallbkFn;
    pRgLineCallback->pCallbkParams     = pAllocParams->pCallbkParams;
    pRgLineCallback->rgIntrLine        = ~0;

    pRgLineCallback->pDispCommon       = dynamicCast(pCallContext->pResourceRef->pParentRef->pResource, DispCommon);

    return _registerRgLineCallback(pRgLineCallback, NV_TRUE);

}

void
rglcbDestruct_IMPL
(
    RgLineCallback *pRgLineCallback
)
{
    _registerRgLineCallback(pRgLineCallback, NV_FALSE);
}

void
rglcbInvoke_IMPL
(
    RgLineCallback *pRgLineCallback,
    NvBool bIsIrqlIsr
)
{
    (pRgLineCallback->pCallbkFn)(pRgLineCallback->rgIntrLine, pRgLineCallback->pCallbkParams, bIsIrqlIsr);
}
