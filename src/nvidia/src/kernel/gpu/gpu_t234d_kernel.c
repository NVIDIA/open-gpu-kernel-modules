/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @file
 * @brief T234D / DCE client specific kernel stubs
 */

#include "core/core.h"
#include "gpu/gpu.h"

NV_STATUS
gpuGetNameString_T234D
(
    OBJGPU *pGpu,
    NvU32 type,
    void *nameStringBuffer
)
{
    const char name[] = "T234D";
    const NvU32 inputLength = NV2080_GPU_MAX_NAME_STRING_LENGTH;

    if (type == NV2080_CTRL_GPU_GET_NAME_STRING_FLAGS_TYPE_ASCII)
    {
        portStringCopy(nameStringBuffer, inputLength, name, sizeof(name));
    }
    else
    {
        portStringConvertAsciiToUtf16(nameStringBuffer, inputLength, name, sizeof(name));
    }

    return NV_OK;
}

NV_STATUS
gpuGetShortNameString_T234D
(
    OBJGPU *pGpu,
    NvU8 *nameStringBuffer
)
{
    return gpuGetNameString_T234D(pGpu, NV2080_CTRL_GPU_GET_NAME_STRING_FLAGS_TYPE_ASCII, nameStringBuffer);
}

NvU32 gpuGetSimulationModeHal_T234D(OBJGPU *pGpu)
{
    return NV_SIM_MODE_INVALID;
}
