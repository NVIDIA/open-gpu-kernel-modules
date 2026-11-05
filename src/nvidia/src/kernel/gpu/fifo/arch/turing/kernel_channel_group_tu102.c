/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "kernel/gpu/fifo/kernel_channel_group.h"

#include "turing/tu102/dev_ram.h"

/**
 * @brief Update the valid subcontext mask of a TSG
 *
 * The channel instance block stores the mask of valid subcontext in a TSG.
 * This is to improve the context switch time. See bug 1795503 for more details.
 */
void
kchangrpUpdateSubcontextMask_TU102
(
    OBJGPU             *pGpu,
    KernelChannelGroup *pKernelChannelGroup,
    NvU32               subctxId,
    NvBool              bValid
)
{
    KernelFifo *pKernelFifo;
    NvU32 index;
    NvU32 subdevInst;

    NV_ASSERT(pKernelChannelGroup != NULL);

    pKernelFifo = GPU_GET_KERNEL_FIFO(pGpu);
    subdevInst = gpumgrGetSubDeviceInstanceFromGpu(pGpu);

    NV_ASSERT(subctxId <
              kfifoChannelGroupGetLocalMaxSubcontext_HAL(pGpu, pKernelFifo,
                                                        pKernelChannelGroup,
                                                        NV_FALSE));

    index = (SF_INDEX(NV_RAMIN_SC_PDB_VALID(subctxId)) - SF_INDEX(NV_RAMIN_SC_PDB_VALID(0)));
    if (index >= SUBCTX_MASK_ARRAY_SIZE)
    {
        NV_ASSERT(index < SUBCTX_MASK_ARRAY_SIZE);
        return;
    }

    pKernelChannelGroup->ppSubctxMask[subdevInst][index] = FLD_SF_IDX_NUM(
        _RAMIN_SC, _PDB_VALID,
        bValid,
        subctxId,
        pKernelChannelGroup->ppSubctxMask[subdevInst][index]);
}
