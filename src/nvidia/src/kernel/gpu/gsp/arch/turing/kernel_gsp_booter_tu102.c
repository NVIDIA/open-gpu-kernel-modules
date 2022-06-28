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

#include "gpu/gsp/kernel_gsp.h"

#include "gpu/gpu.h"
#include "gpu/falcon/kernel_falcon.h"
#include "gpu/sec2/kernel_sec2.h"

#include "published/turing/tu102/dev_fb.h"  // for NV_PFB_PRI_MMU_WPR2_ADDR_HI
#include "published/turing/tu102/dev_falcon_v4.h"


static NV_STATUS
s_executeBooterUcode_TU102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    KernelGspFlcnUcode *pBooterUcode,
    KernelFalcon *pKernelFlcn,
    const NvU32 mailbox0Arg,
    const NvU32 mailbox1Arg
)
{
    NV_STATUS status;
    NvU32 mailbox0, mailbox1;

    NV_ASSERT_OR_RETURN(pBooterUcode != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pKernelFlcn != NULL, NV_ERR_INVALID_STATE);

    mailbox0 = kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_MAILBOX0);
    mailbox1 = kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_MAILBOX1);

    NV_PRINTF(LEVEL_INFO, "before Booter mailbox0 0x%08x, mailbox1 0x%08x\n", mailbox0, mailbox1);

    mailbox0 = mailbox0Arg;
    mailbox1 = mailbox1Arg;

    NV_PRINTF(LEVEL_INFO, "starting Booter with mailbox0 0x%08x, mailbox1 0x%08x\n", mailbox0, mailbox1);

    status = kgspExecuteHsFalcon_HAL(pGpu, pKernelGsp,
                                     pBooterUcode, pKernelFlcn,
                                     &mailbox0, &mailbox1);

    NV_PRINTF(LEVEL_INFO, "after Booter mailbox0 0x%08x, mailbox1 0x%08x\n", mailbox0, mailbox1);

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to execute Booter: status 0x%x, mailbox 0x%x\n", status, mailbox0);
        return status;
    }

    if (mailbox0 != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "Booter failed with non-zero error code: 0x%x\n", mailbox0);
        return NV_ERR_GENERIC;
    }

    return status;
}

NV_STATUS
kgspExecuteBooterLoad_TU102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    const NvU64 gspFwWprMetaOffset
)
{
    NV_STATUS status;
    NvU32 mailbox0, mailbox1;

    KernelSec2 *pKernelSec2 = GPU_GET_KERNEL_SEC2(pGpu);

    NV_ASSERT_OR_RETURN(pKernelGsp->pBooterLoadUcode != NULL, NV_ERR_INVALID_STATE);

    // Provide gspFwWprMetaOffset in falcon SEC mailboxes 0 (low 32 bits) and 1 (high 32 bits)
    mailbox0 = NvU64_LO32(gspFwWprMetaOffset);
    mailbox1 = NvU64_HI32(gspFwWprMetaOffset);

    NV_PRINTF(LEVEL_INFO, "executing Booter Load, gspFwWprMetaOffset 0x%llx\n",
              gspFwWprMetaOffset);

    kflcnReset_HAL(pGpu, staticCast(pKernelSec2, KernelFalcon));

    status = s_executeBooterUcode_TU102(pGpu, pKernelGsp,
                                        pKernelGsp->pBooterLoadUcode,
                                        staticCast(pKernelSec2, KernelFalcon),
                                        mailbox0, mailbox1);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to execute Booter Load: 0x%x\n", status);
        return status;
    }

    return status;
}

NV_STATUS
kgspExecuteBooterUnloadIfNeeded_TU102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    NV_STATUS status;
    KernelSec2 *pKernelSec2 = GPU_GET_KERNEL_SEC2(pGpu);

    // skip actually executing Booter Unload if WPR2 is not up
    NvU32 data = GPU_REG_RD32(pGpu, NV_PFB_PRI_MMU_WPR2_ADDR_HI);
    NvU32 wpr2HiVal = DRF_VAL(_PFB, _PRI_MMU_WPR2_ADDR_HI, _VAL, data);
    if (wpr2HiVal == 0)
    {
        NV_PRINTF(LEVEL_INFO, "skipping executing Booter Unload as WPR is not up\n");
        return NV_OK;
    }

    NV_PRINTF(LEVEL_INFO, "executing Booter Unload\n");
    NV_ASSERT_OR_RETURN(pKernelGsp->pBooterUnloadUcode != NULL, NV_ERR_INVALID_STATE);

    kflcnReset_HAL(pGpu, staticCast(pKernelSec2, KernelFalcon));
    status = s_executeBooterUcode_TU102(pGpu, pKernelGsp,
                                        pKernelGsp->pBooterUnloadUcode,
                                        staticCast(pKernelSec2, KernelFalcon),
                                        0xFF, 0xFF);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to execute Booter Unload: 0x%x\n", status);
        return status;
    }

    data = GPU_REG_RD32(pGpu, NV_PFB_PRI_MMU_WPR2_ADDR_HI);
    wpr2HiVal = DRF_VAL(_PFB, _PRI_MMU_WPR2_ADDR_HI, _VAL, data);
    if (wpr2HiVal > 0)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to execute Booter Unload: WPR2 is still up\n");
        return NV_ERR_GENERIC;
    }

    return status;
}
