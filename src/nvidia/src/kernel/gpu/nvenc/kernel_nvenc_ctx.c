/*
 * SPDX-FileCopyrightText: Copyright (c) 2008-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/nvenc/kernel_nvenc_ctx.h"
#include "kernel/gpu/device/device.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "os/os.h"
#include "gpu/falcon/kernel_falcon.h"

NV_STATUS
msencctxConstructHal_KERNEL
(
    MsencContext                   *pMsencContext,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    ChannelDescendant *pChannelDescendant = staticCast(pMsencContext, ChannelDescendant);
    OBJGPU            *pGpu = GPU_RES_GET_GPU(pChannelDescendant);
    KernelFalcon      *pKernelFalcon = kflcnGetKernelFalconForEngine(pGpu, pChannelDescendant->resourceDesc.engDesc);
    KernelChannel     *pKernelChannel = pChannelDescendant->pKernelChannel;

    NV_PRINTF(LEVEL_INFO, "msencctxConstruct for 0x%x\n", pChannelDescendant->resourceDesc.engDesc);

    return kflcnAllocContext(pGpu, pKernelFalcon, pKernelChannel, RES_GET_EXT_CLASS_ID(pChannelDescendant));
}

void msencctxDestructHal_KERNEL
(
    MsencContext *pMsencContext
)
{
    ChannelDescendant *pChannelDescendant = staticCast(pMsencContext, ChannelDescendant);
    OBJGPU            *pGpu = GPU_RES_GET_GPU(pChannelDescendant);
    KernelFalcon      *pKernelFalcon = kflcnGetKernelFalconForEngine(pGpu, pChannelDescendant->resourceDesc.engDesc);
    KernelChannel     *pKernelChannel = pChannelDescendant->pKernelChannel;

    NV_PRINTF(LEVEL_INFO, "msencctxDestruct for 0x%x\n", pChannelDescendant->resourceDesc.engDesc);

    NV_ASSERT_OK(kflcnFreeContext(pGpu, pKernelFalcon, pKernelChannel, RES_GET_EXT_CLASS_ID(pChannelDescendant)));
}

//
// Query subdevice caps, and return caps for nvenc0
// This version does not support SLI
//
NV_STATUS
deviceCtrlCmdMsencGetCaps_IMPL
(
    Device *pDevice,
    NV0080_CTRL_MSENC_GET_CAPS_PARAMS *pMsencCapsParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDevice);
    NV2080_CTRL_INTERNAL_MSENC_GET_CAPS_PARAMS params;

    // sanity check array size
    if (pMsencCapsParams->capsTblSize != NV0080_CTRL_MSENC_CAPS_TBL_SIZE)
    {
        NV_PRINTF(LEVEL_ERROR, "size mismatch: client 0x%x rm 0x%x\n",
                  pMsencCapsParams->capsTblSize,
                  NV0080_CTRL_MSENC_CAPS_TBL_SIZE);
        return NV_ERR_INVALID_ARGUMENT;
    }

    ct_assert(NV0080_CTRL_MSENC_CAPS_TBL_SIZE ==
        sizeof(params.caps[0].capsTbl) / sizeof(*params.caps[0].capsTbl));

    RM_API *pRmApi = GPU_GET_PHYSICAL_RMAPI(pGpu);
    NV_ASSERT_OK_OR_RETURN(pRmApi->Control(pRmApi,
                            pGpu->hInternalClient,
                            pGpu->hInternalSubdevice,
                            NV2080_CTRL_CMD_INTERNAL_MSENC_GET_CAPS,
                            &params,
                            sizeof(params)));

    NV_ASSERT_OR_RETURN(params.valid[0], NV_ERR_INVALID_STATE);
    portMemCopy(pMsencCapsParams->capsTbl,
        NV0080_CTRL_MSENC_CAPS_TBL_SIZE,
        &params.caps[0].capsTbl,
        sizeof(params.caps[0].capsTbl));

    return NV_OK;
}
