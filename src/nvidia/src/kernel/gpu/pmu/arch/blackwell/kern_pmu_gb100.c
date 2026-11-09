/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * @file
 * @brief   Contains kernel functionality related to the PMU specific to GB100+.
 */

#include "rmconfig.h"
#include "gpu/gpu.h"
#include "gpu/pmu/kern_pmu.h"
#include "gpu/fsp/kern_fsp.h"
#include "gpu/mem_mgr/mem_mgr.h"

NvU64
kpmuComputeReservedMemoryOffset_GB100
(
    OBJGPU *pGpu,
    KernelPmu *pKernelPmu,
    NvU64 fbSize
)
{
    KernelFsp *pKernelFsp = GPU_GET_KERNEL_FSP(pGpu);

    if ((pKernelFsp != NULL) && !pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_DISABLE_FRTS_VIDMEM))
    {
        // This value should have been populated already
        NV_ASSERT(pKernelFsp->pCotPayload->frtsVidmemOffset > 0U);

        //
        // Note: frtsVidmemOffset is an offset of the end of FRTS from the
        // end of FB. If FRTS is enabled and placed in FB (in WPR2) by FSP,
        // we want to directly use its offset to place PMU, since the offset
        // calculation is not always straightforward.
        // Note that we ensure, in kfspPrepareBootCommands_HAL, that we have
        // enough space to place PMU after FRTS, so this FRTS offset should
        // definitely have that accounted for.
        //
        // Note that we have to add back in the ExtraReservedMemorySize
        // because the FSP allocation may extend beyond where the
        // frtsVidmemOffset claims it stops.
        //
        const NvU64 pmuRsvdOffset = fbSize -
            pKernelFsp->pCotPayload->frtsVidmemOffset +
            kfspGetExtraReservedMemorySize_HAL(pGpu, pKernelFsp);
        return pmuRsvdOffset;
    }
    else
    {
        const NvU64 pmuRsvdOffset = fbSize - VGA_WORKSPACE_SIZE - kpmuReservedMemorySizeGet(pKernelPmu);
        return pmuRsvdOffset;
    }
}
