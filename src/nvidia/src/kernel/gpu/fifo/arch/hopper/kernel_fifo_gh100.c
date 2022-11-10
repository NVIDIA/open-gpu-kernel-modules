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

/*!
 * @brief Get offset to create GMMU internal MMIO mapping with for access to VF Pages.
 *
 * @param[in]   pGpu              OBJGPU  pointer
 * @param[in]   pKernelFifo       KernelFifo pointer
 * @param[in]   bPriv             Whether PRIV offset or regular VF offset is desired
 */
NvU64
kfifoGetMmioUsermodeOffset_GH100
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvBool      bPriv
)
{
    return bPriv ? DRF_BASE(NV_VIRTUAL_FUNCTION_PRIV) :  DRF_BASE(NV_VIRTUAL_FUNCTION);
}

/*!
 * @brief Get size of either one of the VF pages, for use with GMMU internal MMIO mapping.
 *
 * @param[in]   pGpu              OBJGPU  pointer
 * @param[in]   pKernelFifo       KernelFifo pointer
 * @param[in]   bPriv             Whether PRIV size or regular VF size is desired
 */
NvU64
kfifoGetMmioUsermodeSize_GH100
(
    OBJGPU     *pGpu,
    KernelFifo *pKernelFifo,
    NvBool      bPriv
)
{
    return bPriv ? DRF_SIZE(NV_VIRTUAL_FUNCTION_PRIV) : DRF_SIZE(NV_VIRTUAL_FUNCTION);
}
