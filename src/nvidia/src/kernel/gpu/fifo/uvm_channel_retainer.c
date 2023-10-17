/*
 * SPDX-FileCopyrightText: Copyright (c) 2016-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


#include "nvtypes.h"
#include "Nvcm.h"
#include "kernel/gpu/fifo/uvm_channel_retainer.h"
#include "class/clc574.h" // UVM_CHANNEL_RETAINER
#include "virtualization/hypervisor/hypervisor.h"
#include "vgpu/vgpu_events.h"
#include "rmapi/client.h"

/*!
 * Construct a new UvmChannelRetainer, which refcounts chId and instance memory.
 *
 * @param[in,out]  pCallContext     The call context
 * @param[in,out]  pParams          UVM_CHANNEL_RETAINER alloc params
 *
 * @returns NV_OK on success, specific error code on failure.
 */
NV_STATUS
uvmchanrtnrConstruct_IMPL
(
    UvmChannelRetainer *pUvmChannelRetainer,
    CALL_CONTEXT *pCallContext,
    RS_RES_ALLOC_PARAMS_INTERNAL *pParams
)
{
    OBJGPU   *pGpu = GPU_RES_GET_GPU(pUvmChannelRetainer);
    KernelFifo  *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    NV_STATUS rmStatus = NV_OK;
    RsClient *pChannelClient;
    KernelChannel *pKernelChannel = NULL;
    NV_UVM_CHANNEL_RETAINER_ALLOC_PARAMS *pUvmChannelRetainerParams = pParams->pAllocParams;
    CHID_MGR *pChidMgr = NULL;

    NV_ASSERT_OK_OR_RETURN(serverGetClientUnderLock(&g_resServ,
        pUvmChannelRetainerParams->hClient, &pChannelClient));

    NV_ASSERT_OK_OR_RETURN(CliGetKernelChannel(pChannelClient, pUvmChannelRetainerParams->hChannel, &pKernelChannel));

    if (!uvmchanrtnrIsAllocationAllowed(pUvmChannelRetainer, pCallContext, pKernelChannel))
    {
        NV_PRINTF(LEVEL_ERROR, "class Id %d can only be allocated by internal kernel clients\n",
            pCallContext->pResourceRef->externalClassId);
        return NV_ERR_INSUFFICIENT_PERMISSIONS;
    }

    pChidMgr = kfifoGetChidMgr(pGpu, pKernelFifo, kchannelGetRunlistId(pKernelChannel));

    // Take a reference on channel Id
    NV_ASSERT_OK_OR_RETURN(kfifoChidMgrRetainChid(pGpu, pKernelFifo, pChidMgr, pKernelChannel->ChID));
    pUvmChannelRetainer->chId = pKernelChannel->ChID;
    pUvmChannelRetainer->runlistId = kchannelGetRunlistId(pKernelChannel);

    // Take a reference on the instance pointer memory
    NV_ASSERT_OK_OR_GOTO(rmStatus,
                         kfifoChannelGetFifoContextMemDesc_HAL(pGpu,
                                                               pKernelFifo,
                                                               pKernelChannel,
                                                               FIFO_CTX_INST_BLOCK,
                                                               &pUvmChannelRetainer->pInstMemDesc),
                         fail);
    if (pUvmChannelRetainer->pInstMemDesc->Allocated > 0)
        pUvmChannelRetainer->pInstMemDesc->Allocated++;

    memdescAddRef(pUvmChannelRetainer->pInstMemDesc);

fail:
    if (rmStatus != NV_OK)
    {
        NV_ASSERT_OK(kfifoChidMgrReleaseChid(pGpu, pKernelFifo, pChidMgr, pKernelChannel->ChID));
    }

    return rmStatus;
}

void
uvmchanrtnrDestruct_IMPL
(
    UvmChannelRetainer *pUvmChannelRetainer
)
{
    OBJGPU     *pGpu = GPU_RES_GET_GPU(pUvmChannelRetainer);
    KernelFifo *pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    CHID_MGR   *pChidMgr = NULL;

    if (pUvmChannelRetainer->pInstMemDesc->Allocated > 0)
        memdescFree(pUvmChannelRetainer->pInstMemDesc);

    memdescDestroy(pUvmChannelRetainer->pInstMemDesc);

    pChidMgr = kfifoGetChidMgr(pGpu, pKernelFifo, pUvmChannelRetainer->runlistId);
    NV_ASSERT_OK(kfifoChidMgrReleaseChid(pGpu, pKernelFifo, pChidMgr, pUvmChannelRetainer->chId));
}

/*!
 * @brief Check if the client allocating this is an internal kernel client on
 * Baremetal, Guest RM and admin client on Host RM.
 *
 * @param[in]  pRmApi
 * @param[in]  hClient
 * @param[in]  pKernelChannel
 *
 * @returns NV_TRUE if allowed.
 */
NvBool
uvmchanrtnrIsAllocationAllowed_IMPL
(
    UvmChannelRetainer *pUvmChannelRetainer,
    CALL_CONTEXT *pCallContext,
    KernelChannel *pKernelChannel
)

{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pUvmChannelRetainer);

    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_FALSE);

    if (gpuIsSriovEnabled(pGpu))
    {
        NvU32 gfid;
        NV_ASSERT_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid) == NV_OK, NV_FALSE);
        if (IS_GFID_VF(gfid))
        {
            return (gfid == kchannelGetGfid(pKernelChannel));
        }
    }

    return NV_TRUE;
}
