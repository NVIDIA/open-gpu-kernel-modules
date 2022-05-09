/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/fifo/kernel_fifo.h"
#include "kernel/gpu/fifo/kernel_channel.h"

#include "gpu/gpu_access.h"
#include "gpu/gpu.h"
#include "vgpu/vgpu_events.h"

#include "published/turing/tu102/dev_vm.h"
#include "published/turing/tu102/dev_ctrl.h"

/*!
 * @brief Update the usermode doorbell register with work submit token to notify
 *        host that work is available on this channel.
 *
 * @param[in] pGpu
 * @param[in] pKernelFifo
 * @param[in] workSubmitToken Token to update the doorbell with
 * @param[in] runlistId       Runlist ID
 */
NV_STATUS
kfifoUpdateUsermodeDoorbell_TU102
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvU32       workSubmitToken,
    NvU32       runlistId
)
{
    NV_PRINTF(LEVEL_INFO, "Poking workSubmitToken 0x%x\n", workSubmitToken);

    GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_DOORBELL, workSubmitToken);

    return NV_OK;
}

/*!
 * @brief Construct the worksubmit token. Caller cannot make assumption about this handle.
 *
 * @param[in]  pGpu
 * @param[in]  pKernelFifo
 * @param[in]  pKernelChannel
 * @param[out] pGeneratedToken Store the generated token
 * @param[in]  bUsedForHost    Used on Host RM
 *
 */
NV_STATUS
kfifoGenerateWorkSubmitToken_TU102
(
    OBJGPU        *pGpu,
    KernelFifo    *pKernelFifo,
    KernelChannel *pKernelChannel,
    NvU32         *pGeneratedToken,
    NvBool         bUsedForHost
)
{
    NvU32          chId;
    NvU32          gfid;
    NvU32          val = 0;

    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_CHANNEL);

    NV_ASSERT_OR_RETURN(pGeneratedToken != NULL, NV_ERR_INVALID_ARGUMENT);

    chId = pKernelChannel->ChID;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    //
    // In case of vGPU with SR-IOV, host RM is currently generating token using
    // virtual chid that was allocated inside the guest. This needs to change
    // once the guest starts managing its own channels. The guest would then
    // generate its own tokens.
    //
    if (!bUsedForHost && IS_GFID_VF(gfid))
    {
        NvU32 vChId;

        NV_ASSERT_OK_OR_RETURN(kfifoGetVChIdForSChId_HAL(pGpu, pKernelFifo,
                                                         chId, gfid,
                                                         kchannelGetEngineType(pKernelChannel),
                                                         &vChId));
        chId = vChId;
    }

    if (!kchannelIsRunlistSet(pGpu, pKernelChannel))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "FAILED Channel 0x%x is not assigned to runlist yet\n",
                  kchannelGetDebugTag(pKernelChannel));
        return NV_ERR_INVALID_STATE;
    }

    // Here we construct token to be a concatenation of runlist id and channel id
    val = FLD_SET_DRF_NUM(_CTRL, _VF_DOORBELL, _RUNLIST_ID, kchannelGetRunlistId(pKernelChannel), val);
    val = FLD_SET_DRF_NUM(_CTRL, _VF_DOORBELL, _VECTOR, chId, val);
    *pGeneratedToken = val;

    NV_PRINTF(LEVEL_INFO,
              "Generated workSubmitToken 0x%x for channel 0x%x runlist 0x%x\n",
              *pGeneratedToken, chId, kchannelGetRunlistId(pKernelChannel));

    return NV_OK;
}
