/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/falcon/kernel_falcon.h"
#include "gpu/nvlink/kernel_nvlink.h"
#include "gpu/fifo/kernel_fifo.h"
#include "gpu/fifo/kernel_channel.h"
#include "gpu/fifo/kernel_channel_group.h"
#include "gpu/fifo/kernel_channel_group_api.h"
#include "gpu/subdevice/subdevice.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "vgpu/rpc.h"
#include "ctrl/ctrl2080/ctrl2080flcn.h"

NV_STATUS subdeviceCtrlCmdFlcnGetCtxBufferInfo_IMPL
(   Subdevice *pSubdevice,
    NV2080_CTRL_FLCN_GET_CTX_BUFFER_INFO_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    PMEMORY_DESCRIPTOR pMemDesc, pRootMemDesc;
    RsClient *pUserClient;
    KernelChannel *pKernelChannel = NULL;
    NV_STATUS status = NV_OK;
    NvU64 pageSize;
    NvU8 *pUuid = NULL;

    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
    // do an RPC to the host to get context buffers information.
    //
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL_WITH_SRIOV(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
        return status;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
        serverGetClientUnderLock(&g_resServ, pParams->hUserClient, &pUserClient));

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
        CliGetKernelChannel(pUserClient, pParams->hChannel, &pKernelChannel));
    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_CHANNEL);

    switch (kchannelGetEngineType(pKernelChannel))
    {
        case RM_ENGINE_TYPE_SEC2:
        {
            break;
        }
        default:
        {
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    NV_ASSERT_OK_OR_RETURN(kchangrpGetEngineContextMemDesc(pGpu, pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup, &pMemDesc));
    pRootMemDesc = memdescGetRootMemDesc(pMemDesc, NULL);

    pParams->bufferHandle = NV_PTR_TO_NvP64(pMemDesc);
    pParams->bIsContigous = memdescGetContiguity(pMemDesc, AT_GPU);
    pParams->aperture = memdescGetAddressSpace(pMemDesc);
    pParams->pageCount = pMemDesc->PageCount;
    pParams->kind = memdescGetPteKindForGpu(pMemDesc, pMemDesc->pGpu);

    {
        NvU64 physAddr;
        GMMU_APERTURE aperture = kgmmuGetExternalAllocAperture(pParams->aperture);

        memdescGetPhysAddrsForGpu(pMemDesc, pMemDesc->pGpu,
                                  AT_GPU, 0, 0, 1,
                                  &physAddr);

        pParams->physAddr =
            kgmmuEncodePhysAddr(GPU_GET_KERNEL_GMMU(pMemDesc->pGpu), aperture, physAddr, NVLINK_INVALID_FABRIC_ADDR);
    }

    pageSize = memdescGetPageSize(pMemDesc, AT_GPU);
    if (pageSize == 0)
    {
        status = memmgrSetMemDescPageSize_HAL(pMemDesc->pGpu,
                                              GPU_GET_MEMORY_MANAGER(pMemDesc->pGpu),
                                              pMemDesc,
                                              AT_GPU,
                                              RM_ATTR_PAGE_SIZE_DEFAULT);
        if (status != NV_OK)
            return status;

        pageSize = memdescGetPageSize(pMemDesc, AT_GPU);
        NV_ASSERT(pageSize != 0);
    }

    //
    // Alignment is used to adjust the mapping VA. Hence, we need to make sure
    // that it is at least pageSize to make mapping calculation work correctly.
    //
    pParams->alignment = (pMemDesc->Alignment != 0) ?
        NV_ALIGN_UP(pMemDesc->Alignment, pageSize) : pageSize;

    pParams->size = pMemDesc->ActualSize;

    NV_ASSERT_OR_RETURN(pageSize <= NV_U32_MAX, NV_ERR_INVALID_STATE);
    pParams->pageSize = (NvU32)pageSize;

    pParams->bDeviceDescendant = pRootMemDesc->pGpu != NULL;

    if (pParams->bDeviceDescendant)
    {
        NvU32 flags = DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _TYPE, _SHA1) |
            DRF_DEF(2080_GPU_CMD, _GPU_GET_GID_FLAGS, _FORMAT, _BINARY);
        NvU32 uuidLength;
        // allocates memory for pUuid on success
        NV_ASSERT_OK_OR_RETURN(gpuGetGidInfo(pGpu, &pUuid, &uuidLength, flags));
        if (uuidLength == sizeof(pParams->uuid))
            portMemCopy(pParams->uuid, uuidLength, pUuid, uuidLength);
        else
        {
            status = NV_ERR_INVALID_ARGUMENT;
            goto done;
        }
    }

done:
    portMemFree(pUuid);
    return status;
}

NV_STATUS
subdeviceCtrlCmdFlcnGetCtxBufferSize_IMPL
(
    Subdevice *pSubdevice,
    NV2080_CTRL_FLCN_GET_CTX_BUFFER_SIZE_PARAMS *pParams
)
{
    OBJGPU *pGpu = GPU_RES_GET_GPU(pSubdevice);
    PMEMORY_DESCRIPTOR pMemDesc;
    KernelChannel *pKernelChannel = NULL;
    NV_STATUS status = NV_OK;
    NvU64 alignment;
    NvU64 pageSize;
    NvU64 totalBufferSize;

    //
    // vGPU:
    //
    // Since vGPU does all real hardware management in the
    // host, if we are in guest OS (where IS_VIRTUAL(pGpu) is true),
    // do an RPC to the host to fetch the engine context buffer size.
    //
    if (IS_VIRTUAL_WITHOUT_SRIOV(pGpu) ||
        (IS_VIRTUAL_WITH_SRIOV(pGpu) && gpuIsWarBug200577889SriovHeavyEnabled(pGpu)))
    {
        CALL_CONTEXT *pCallContext = resservGetTlsCallContext();
        RmCtrlParams *pRmCtrlParams = pCallContext->pControlParams;

        NV_RM_RPC_CONTROL(pGpu,
                          pRmCtrlParams->hClient,
                          pRmCtrlParams->hObject,
                          pRmCtrlParams->cmd,
                          pRmCtrlParams->pParams,
                          pRmCtrlParams->paramsSize,
                          status);
        return status;
    }

    NV_CHECK_OK_OR_RETURN(LEVEL_INFO,
        CliGetKernelChannel(RES_GET_CLIENT(pSubdevice), pParams->hChannel,
            &pKernelChannel));
    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_CHANNEL);

    switch (kchannelGetEngineType(pKernelChannel))
    {
        case RM_ENGINE_TYPE_SEC2:
        {
            break;
        }
        default:
        {
            return NV_ERR_NOT_SUPPORTED;
        }
    }

    NV_ASSERT_OK_OR_RETURN(kchangrpGetEngineContextMemDesc(pGpu, pKernelChannel->pKernelChannelGroupApi->pKernelChannelGroup, &pMemDesc));

    pageSize = memdescGetPageSize(pMemDesc, AT_GPU);
    if (pageSize == 0)
    {
        status = memmgrSetMemDescPageSize_HAL(pMemDesc->pGpu,
                                              GPU_GET_MEMORY_MANAGER(pMemDesc->pGpu),
                                              pMemDesc,
                                              AT_GPU,
                                              RM_ATTR_PAGE_SIZE_DEFAULT);
        if (status != NV_OK)
            return status;

        pageSize = memdescGetPageSize(pMemDesc, AT_GPU);
        NV_ASSERT(pageSize != 0);
    }

    //
    // Adjust the total size by adding the alignment so that the mapping VA can
    // be adjusted.
    //
    alignment = (pMemDesc->Alignment != 0) ? NV_ALIGN_UP(pMemDesc->Alignment, pageSize) : pageSize;
    totalBufferSize = 0;
    totalBufferSize += alignment;
    totalBufferSize += (alignment != 0) ? NV_ALIGN_UP(pMemDesc->ActualSize, alignment) : pMemDesc->ActualSize;
    pParams->totalBufferSize = totalBufferSize;

    return status;
}
