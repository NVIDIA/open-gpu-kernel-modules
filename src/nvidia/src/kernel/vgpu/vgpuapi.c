/*
 * SPDX-FileCopyrightText: Copyright (c) 2014-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/core.h"
#include "os/os.h"

#include "vgpu/vgpuapi.h"
#include "gpu/gpu.h"

NV_STATUS
vgpuapiConstruct_IMPL
(
    VgpuApi *pVgpuApi,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    NV_STATUS rmStatus   = NV_OK;
    OBJGPU   *pGpu       = GPU_RES_GET_GPU(pVgpuApi);
    NvHandle  hDevice    = pParams->hParent;
    NvHandle  hVgpu      = pParams->hResource;

    // KEPLER_DEVICE_VGPU is only supported for virtaul GPU
    if (!IS_VIRTUAL(pGpu))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    pVgpuApi->handle   = hVgpu;
    pVgpuApi->hDevice  = hDevice;

    pVgpuApi->node.Data     = (void *)pVgpuApi;
    pVgpuApi->node.keyStart = hVgpu;
    pVgpuApi->node.keyEnd   = hVgpu;

    return rmStatus;
}

void
vgpuapiDestruct_IMPL
(
    VgpuApi *pResource
)
{
    OBJGPU                      *pGpu = GPU_RES_GET_GPU(pResource);
    CALL_CONTEXT                *pCallContext;
    RS_RES_FREE_PARAMS_INTERNAL *pParams;

    resGetFreeParams(staticCast(pResource, RsResource), &pCallContext, &pParams);

    // KEPLER_DEVICE_VGPU is only supported for virtual GPU
    if (!IS_VIRTUAL(pGpu))
    {
        pParams->status = NV_ERR_NOT_SUPPORTED;
        return;
    }

    pParams->status = NV_OK;
}

