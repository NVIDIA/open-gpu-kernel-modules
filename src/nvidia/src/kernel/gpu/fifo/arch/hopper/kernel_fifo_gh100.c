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
#include "published/hopper/gh100/dev_vm.h"
#include "kernel/gpu/mem_mgr/virt_mem_allocator.h"
#include "gpu/mem_mgr/mem_mgr.h"

/*!
 * Checks the USERD and GPFIFO/PushBuffer location attributes
 * to make sure that when USERD is in sysmem, GPFIFO or
 * PushBuffer is not in vidmem.
 *
 * @param[in]   pKernelFifo         KernelFifo pointer
 * @param[in]   userdAddrSpace      USERD address space
 * @param[in]   pushBuffAddrSpace   PushBuffer address space
 * @param[in]   gpFifoAddrSpace     GPFIFO address space
 *
 */
NV_STATUS
kfifoCheckChannelAllocAddrSpaces_GH100
(
    KernelFifo       *pKernelFifo,
    NV_ADDRESS_SPACE  userdAddrSpace,
    NV_ADDRESS_SPACE  pushBuffAddrSpace,
    NV_ADDRESS_SPACE  gpFifoAddrSpace
)
{
    if ((userdAddrSpace == ADDR_SYSMEM) &&
        ((pushBuffAddrSpace == ADDR_FBMEM || gpFifoAddrSpace == ADDR_FBMEM)))
    {
        return NV_ERR_NOT_SUPPORTED;
    }

    return NV_OK;
}

/*
 * Returns VA alloc flags to be passed into dmaMapBuffer.
 *
 * @param[in]   pGpu               OBJGPU pointer
 * @param[in]   pKernelFifo        KernelFifo pointer
 * @param[in]   engine             engine tells us which engine's ctx buffer are we getting the flags for
 * @param[out]  pflags             NvU32* returned alloc flags to pass into dmaMapBuffer.
 */
void
kfifoGetCtxBufferMapFlags_GH100
(
    OBJGPU      *pGpu,
    KernelFifo  *pKernelFifo,
    NvU32        engine,
    NvU32       *pflags
)
{
    if (IS_GR(engine))
        *pflags |= DMA_VA_LIMIT_57B;

    return;
}

/*
 * Allocate Memory Descriptors for BAR1 VF pages
 *
 * @param[in]   pGpu               OBJGPU pointer
 * @param[in]   pKernelFifo        KernelFifo pointer
 */
NV_STATUS
kfifoConstructUsermodeMemdescs_GH100
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU32 attr                    = 0;
    NvU32 attr2                   = 0;
    NvU32 i                       = 0;
    NV_STATUS status              = NV_OK;

    attr = FLD_SET_DRF(OS32, _ATTR,  _PHYSICALITY, _CONTIGUOUS, attr);
    attr = FLD_SET_DRF(OS32, _ATTR,  _COHERENCY, _CACHED, attr);

    attr2 = FLD_SET_DRF(OS32, _ATTR2, _GPU_CACHEABLE, _NO, attr2 );

    for(i = 0; i < 2; i++)
    {
        NvBool bPriv = (i==0);
        NvU64 offset = bPriv ? DRF_BASE(NV_VIRTUAL_FUNCTION_PRIV) :  DRF_BASE(NV_VIRTUAL_FUNCTION);
        NvU64 size   = bPriv ? DRF_SIZE(NV_VIRTUAL_FUNCTION_PRIV) : DRF_SIZE(NV_VIRTUAL_FUNCTION);
        MEMORY_DESCRIPTOR **ppMemDesc = bPriv ? &(pKernelFifo->pBar1PrivVF) : &(pKernelFifo->pBar1VF);
        NV_ASSERT_OK_OR_GOTO(status,
            memCreateMemDesc(pGpu, ppMemDesc, ADDR_SYSMEM, offset, size, attr, attr2),
            err);
        memdescSetPteKind(*ppMemDesc, memmgrGetMessageKind_HAL(pGpu, pMemoryManager));
        memdescSetFlag(*ppMemDesc, MEMDESC_FLAGS_MAP_SYSCOH_OVER_BAR1, NV_TRUE);
        memdescSetPageSize(*ppMemDesc, AT_GPU, RM_PAGE_SIZE);
    }

    NV_ASSERT_OK_OR_GOTO(status,
        kfifoConstructUsermodeMemdescs_GV100(pGpu, pKernelFifo),
        err);
    return NV_OK;
err:
    memdescDestroy(pKernelFifo->pBar1VF);
    memdescDestroy(pKernelFifo->pBar1PrivVF);
    return status;
}
