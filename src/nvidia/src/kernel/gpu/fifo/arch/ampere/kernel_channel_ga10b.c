/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/fifo/kernel_channel.h"
#include "kernel/gpu/mem_mgr/mem_mgr.h"

/*!
 * @brief Creates a memory descriptor to be used for creating a GPU mapped MMIO
 *        region for a given channel.
 */
NV_STATUS
kchannelCreateUserMemDesc_GA10B
(
    OBJGPU        *pGpu,
    KernelChannel *pKernelChannel
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelFifo    *pKernelFifo    = GPU_GET_KERNEL_FIFO(pGpu);
    NV_STATUS status;
    MEMORY_DESCRIPTOR *pSubDevInstMemDesc = NULL;
    MEMORY_DESCRIPTOR **ppMemDesc =
         &pKernelChannel->pInstSubDeviceMemDesc[gpumgrGetSubDeviceInstanceFromGpu(pGpu)];

    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    status = kfifoChannelGetFifoContextMemDesc_HAL(pGpu, pKernelFifo, pKernelChannel,
                                                   FIFO_CTX_INST_BLOCK, &pSubDevInstMemDesc);

    if (status != NV_OK)
        return status;

    NV_ASSERT_OR_RETURN(pSubDevInstMemDesc != NULL, NV_ERR_OBJECT_NOT_FOUND);

    status = memdescCreate(ppMemDesc, pGpu, RM_PAGE_SIZE, 0,
                           memdescGetContiguity(pSubDevInstMemDesc, AT_GPU),
                           memdescGetAddressSpace(pSubDevInstMemDesc),
                           memdescGetCpuCacheAttrib(pSubDevInstMemDesc),
                           MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE);

    if (status != NV_OK)
        return status;

    NV_ASSERT_OR_RETURN(*ppMemDesc != NULL, NV_ERR_NO_MEMORY);

    memdescDescribe(*ppMemDesc, memdescGetAddressSpace(pSubDevInstMemDesc),
                    memdescGetPhysAddr(pSubDevInstMemDesc, AT_GPU, 0), RM_PAGE_SIZE);
    memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager, *ppMemDesc, AT_GPU, RM_ATTR_PAGE_SIZE_4KB);

    return NV_OK;
}
