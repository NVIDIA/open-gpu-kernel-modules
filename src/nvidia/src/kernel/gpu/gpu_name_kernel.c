/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/gpu.h"

NV_STATUS
gpuGetNameString_KERNEL
(
    OBJGPU *pGpu,
    NvU32 type,
    void *nameStringBuffer
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_GPU_GET_NAME_STRING_PARAMS params;

    portMemSet(&params, 0, sizeof(params));

    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                       NV2080_CTRL_CMD_GPU_GET_NAME_STRING, &params, sizeof(params)));

    if (type == NV2080_CTRL_GPU_GET_NAME_STRING_FLAGS_TYPE_ASCII)
    {
        portMemCopy(nameStringBuffer, sizeof(params.gpuNameString.ascii),
                    params.gpuNameString.ascii, sizeof(params.gpuNameString.ascii));
    }
    else
    {
        portMemCopy(nameStringBuffer, sizeof(params.gpuNameString.unicode),
                    params.gpuNameString.unicode, sizeof(params.gpuNameString.unicode));
    }

    return NV_OK;
}

NV_STATUS
gpuGetShortNameString_KERNEL
(
    OBJGPU *pGpu,
    NvU8 *nameStringBuffer
)
{
    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV2080_CTRL_GPU_GET_SHORT_NAME_STRING_PARAMS params;

    portMemSet(&params, 0, sizeof(params));

    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi, pGpu->hInternalClient, pGpu->hInternalSubdevice,
                       NV2080_CTRL_CMD_GPU_GET_SHORT_NAME_STRING, &params, sizeof(params)));

    portMemCopy(nameStringBuffer, sizeof(params.gpuShortNameString),
                params.gpuShortNameString, sizeof(params.gpuShortNameString));

     return NV_OK;
}
