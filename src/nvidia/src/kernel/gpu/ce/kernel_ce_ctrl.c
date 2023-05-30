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

#include "core/locks.h"
#include "gpu/ce/kernel_ce.h"
#include "gpu/ce/kernel_ce_private.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu_mgr/gpu_mgr.h"
#include "vgpu/rpc.h"

//
// CE RM Device Controls
//

//
// NV2080_CTRL_CMD_CE_GET_CAPS passes userspace pointer for copyin/out.
// NV2080_CTRL_CMD_CE_GET_CAPS_V2 stores data inline.
//

//
// Lock Requirements:
//      Assert that API lock held on entry
//
NV_STATUS
subdeviceCtrlCmdCeGetCaps_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_CE_GET_CAPS_PARAMS *pCeCapsParams
)
{
    OBJGPU     *pGpu = GPU_RES_GET_GPU(pSubdevice);
    KernelCE    *pKCe;
    NvU32       ceNumber;
    NV_STATUS   status = NV_OK;
    RM_ENGINE_TYPE rmEngineType;

    LOCK_ASSERT_AND_RETURN(rmapiLockIsOwner());

    // sanity check array size
    if (pCeCapsParams->capsTblSize != NV2080_CTRL_CE_CAPS_TBL_SIZE)
    {
        NV_PRINTF(LEVEL_ERROR, "size mismatch: client 0x%x rm 0x%x\n",
                  pCeCapsParams->capsTblSize, NV2080_CTRL_CE_CAPS_TBL_SIZE);
        return NV_ERR_INVALID_ARGUMENT;
    }

    rmEngineType = gpuGetRmEngineType(pCeCapsParams->ceEngineType);

    if (!RM_ENGINE_TYPE_IS_COPY(rmEngineType))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
    // do an RPC to the host to get blacklist information from host RM
    //
    if (IS_VIRTUAL(pGpu))
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;
        NV2080_CTRL_CE_GET_CAPS_V2_PARAMS ceCapsv2Params = { 0 };

        ceCapsv2Params.ceEngineType = pCeCapsParams->ceEngineType;

        NV_RM_RPC_CONTROL(pGpu, pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          NV2080_CTRL_CMD_CE_GET_CAPS_V2,
                          &ceCapsv2Params,
                          sizeof(ceCapsv2Params),
                          status);

        if (status == NV_OK)
        {
            portMemCopy(NvP64_VALUE(pCeCapsParams->capsTbl),
                        (sizeof(NvU8) * NV2080_CTRL_CE_CAPS_TBL_SIZE),
                        ceCapsv2Params.capsTbl,
                        (sizeof(NvU8) * NV2080_CTRL_CE_CAPS_TBL_SIZE));
        }
        return status;
    }

    NV_ASSERT_OK_OR_RETURN(ceIndexFromType(pGpu, GPU_RES_GET_DEVICE(pSubdevice),
                                           rmEngineType, &ceNumber));

    pKCe = GPU_GET_KCE(pGpu, ceNumber);

    // Return an unsupported error for not present or stubbed CEs as they are
    // not supposed to be user visible and cannot be allocated anyway.
    if (!pKCe)
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    // now fill in caps for this CE
    return kceGetDeviceCaps(pGpu, pKCe, rmEngineType, NvP64_VALUE(pCeCapsParams->capsTbl));
}
