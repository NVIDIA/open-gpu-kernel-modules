/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/core/core.h"
#include "kernel/gpu/mem_mgr/virt_mem_allocator.h"
#include "kernel/gpu/rc/kernel_rc.h"
#include "kernel/os/os.h"

#include "gpu/mem_mgr/mem_desc.h"
#include "nverror.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/bus/kern_bus.h"


NV_STATUS krcReadVirtMem_IMPL
(
    OBJGPU        *pGpu,
    KernelRc      *pKernelRc,
    KernelChannel *pKernelChannel,
    NvU64          virtAddr,
    NvP64          bufPtr,
    NvU32          bufSize
)
{
    VirtMemAllocator  *pDma = GPU_GET_DMA(pGpu);
    MemoryManager     *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    MEMORY_DESCRIPTOR  memDesc;

    NvU32     pageStartOffset;
    NvU32     start4kPage;
    NvU32     end4kPage;
    NvU64     physaddr;
    NvU32     memtype;
    NvU8     *pMem = NULL;
    NvU32     cursize;
    NvU32     cur4kPage;
    NV_STATUS status = NV_OK;
    TRANSFER_SURFACE surf = {0};

    pageStartOffset = NvOffset_LO32(virtAddr) & RM_PAGE_MASK;
    start4kPage = (NvOffset_LO32(virtAddr) >> 12) & 0x1FFFF;
    end4kPage = (NvOffset_LO32(virtAddr + bufSize - 1) >> 12) & 0x1FFFF;

    cursize = RM_PAGE_SIZE - pageStartOffset;
    virtAddr &= ~RM_PAGE_MASK;

    pMem = portMemAllocNonPaged(RM_PAGE_SIZE);
    if (pMem == NULL)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    surf.pMemDesc = &memDesc;
    surf.offset = 0;

    for (cur4kPage = start4kPage; cur4kPage <= end4kPage; ++cur4kPage)
    {
        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            dmaXlateVAtoPAforChannel_HAL(pGpu, pDma, pKernelChannel, virtAddr, &physaddr, &memtype),
            cleanup);

        memdescCreateExisting(&memDesc,
                              pGpu,
                              RM_PAGE_SIZE,
                              memtype,
                              NV_MEMORY_UNCACHED,
                              MEMDESC_FLAGS_NONE);
        memdescDescribe(&memDesc, memtype, physaddr, RM_PAGE_SIZE);

        NV_CHECK_OK_OR_GOTO(status, LEVEL_ERROR,
            memmgrMemRead(pMemoryManager, &surf, pMem, RM_PAGE_SIZE, TRANSFER_FLAGS_NONE),
            cleanup);

        if (cursize > bufSize)
        {
            cursize = bufSize;
        }

        portMemCopy(NvP64_VALUE(bufPtr),
                    cursize,
                    pMem + pageStartOffset,
                    cursize);

        memdescDestroy(&memDesc);

        pageStartOffset = 0;
        bufPtr = NvP64_PLUS_OFFSET(bufPtr,cursize);
        bufSize -= cursize;
        cursize = RM_PAGE_SIZE;
        virtAddr += RM_PAGE_SIZE;
    }

cleanup:
    portMemFree(pMem);

    return status;
}
