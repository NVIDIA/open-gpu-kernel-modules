/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "os/os.h"
#include "kernel/gpu/falcon/kernel_falcon.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/gpu/ofa/kernel_ofa_ctx.h"

NV_STATUS
ofactxConstructHal_KERNEL
(
    OfaContext                   *pOfaContext,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    ChannelDescendant *pChannelDescendant = staticCast(pOfaContext, ChannelDescendant);
    OBJGPU            *pGpu = GPU_RES_GET_GPU(pChannelDescendant);
    KernelFalcon      *pKernelFalcon = kflcnGetKernelFalconForEngine(pGpu, pChannelDescendant->resourceDesc.engDesc);
    KernelChannel     *pKernelChannel = pChannelDescendant->pKernelChannel;

    if (pKernelFalcon == NULL)
        return NV_ERR_GENERIC;

    NV_PRINTF(LEVEL_INFO, "ofactxConstruct for 0x%x\n", pChannelDescendant->resourceDesc.engDesc);

    return kflcnAllocContext(pGpu, pKernelFalcon, pKernelChannel, RES_GET_EXT_CLASS_ID(pChannelDescendant));
}

void ofactxDestructHal_KERNEL
(
    OfaContext *pOfaContext
)
{
    ChannelDescendant *pChannelDescendant = staticCast(pOfaContext, ChannelDescendant);
    OBJGPU            *pGpu = GPU_RES_GET_GPU(pChannelDescendant);
    KernelFalcon      *pKernelFalcon = kflcnGetKernelFalconForEngine(pGpu, pChannelDescendant->resourceDesc.engDesc);
    KernelChannel     *pKernelChannel = pChannelDescendant->pKernelChannel;

    NV_PRINTF(LEVEL_INFO, "ofactxDestruct for 0x%x\n", pChannelDescendant->resourceDesc.engDesc);

    NV_ASSERT_OK(kflcnFreeContext(pGpu, pKernelFalcon, pKernelChannel, RES_GET_EXT_CLASS_ID(pChannelDescendant)));
}

