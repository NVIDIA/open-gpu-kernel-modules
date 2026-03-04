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

#include "kernel/gpu/device/device.h"
#include "kernel/gpu/falcon/kernel_falcon.h"
#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/gpu/mig_mgr/kernel_mig_manager.h"
#include "kernel/gpu/nvdec/kernel_nvdec_ctx.h"
#include "os/os.h"
#include "vgpu/sdk-structures.h"

#include "ctrl/ctrl0080/ctrl0080bsp.h"

NV_STATUS
nvdecctxConstructHal_KERNEL
(
    NvdecContext                 *pNvdecContext,
    CALL_CONTEXT                 *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    ChannelDescendant *pChannelDescendant = staticCast(pNvdecContext, ChannelDescendant);
    OBJGPU            *pGpu = GPU_RES_GET_GPU(pChannelDescendant);
    KernelFalcon      *pKernelFalcon = kflcnGetKernelFalconForEngine(pGpu, pChannelDescendant->resourceDesc.engDesc);
    KernelChannel     *pKernelChannel = pChannelDescendant->pKernelChannel;

    if (pKernelFalcon == NULL)
        return NV_ERR_INVALID_STATE;

    NV_PRINTF(LEVEL_INFO, "nvdecctxConstruct for 0x%x\n", pChannelDescendant->resourceDesc.engDesc);

    return kflcnAllocContext(pGpu, pKernelFalcon, pKernelChannel, RES_GET_EXT_CLASS_ID(pChannelDescendant));
}

void nvdecctxDestructHal_KERNEL
(
    NvdecContext *pNvdecContext
)
{
    ChannelDescendant *pChannelDescendant = staticCast(pNvdecContext, ChannelDescendant);
    OBJGPU            *pGpu = GPU_RES_GET_GPU(pChannelDescendant);
    KernelFalcon      *pKernelFalcon = kflcnGetKernelFalconForEngine(pGpu, pChannelDescendant->resourceDesc.engDesc);
    KernelChannel     *pKernelChannel = pChannelDescendant->pKernelChannel;

    NV_PRINTF(LEVEL_INFO, "nvdecctxDestruct for 0x%x\n", pChannelDescendant->resourceDesc.engDesc);

    NV_ASSERT_OK(kflcnFreeContext(pGpu, pKernelFalcon, pKernelChannel, RES_GET_EXT_CLASS_ID(pChannelDescendant)));
}

NV_STATUS deviceCtrlCmdBspGetCapsV2_VF
(
    Device *pDevice,
    NV0080_CTRL_BSP_GET_CAPS_PARAMS_V2 *pBspCapParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pDevice);
    VGPU_STATIC_INFO *pVSI = GPU_GET_STATIC_INFO(pGpu);
    NV_ASSERT_OR_RETURN(pVSI != NULL, NV_ERR_INVALID_STATE);

    // Sanity check for correct instance Id of NVDEC
    if (pBspCapParams->instanceId >= GPU_MAX_NVDECS)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Requested NVDEC Id 0x%x is not present. Hence, returning capabilities of NVDEC0\n",
                  pBspCapParams->instanceId);
        // Set default instance Id to zero if it's not set already.
        pBspCapParams->instanceId = 0;
    }

    portMemCopy(pBspCapParams->capsTbl, sizeof(pBspCapParams->capsTbl),
                pVSI->vgpuBspCaps[pBspCapParams->instanceId].capsTbl,
                NV0080_CTRL_BSP_CAPS_TBL_SIZE);
    return NV_OK;
}
