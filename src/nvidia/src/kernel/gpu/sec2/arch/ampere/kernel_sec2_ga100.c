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

#include "gpu/sec2/kernel_sec2.h"

#include "core/core.h"
#include "gpu/falcon/kernel_falcon.h"
#include "gpu/gpu.h"
#include "os/nv_memory_type.h"

#include "published/ampere/ga100/dev_fuse.h"
#include "published/ampere/ga100/dev_sec_pri.h"
#include "published/ampere/ga100/dev_sec_addendum.h"

NV_STATUS
ksec2ConfigureFalcon_GA100
(
    OBJGPU *pGpu,
    KernelSec2 *pKernelSec2
)
{
    KernelFalconEngineConfig falconConfig;

    portMemSet(&falconConfig, 0, sizeof(falconConfig));

    falconConfig.registerBase       = DRF_BASE(NV_PSEC);
    falconConfig.riscvRegisterBase  = 0;  // RISC-V unused or unsupported
    falconConfig.fbifBase           = NV_PSEC_FBIF_BASE;
    falconConfig.bBootFromHs        = NV_FALSE;
    falconConfig.pmcEnableMask      = 0;
    falconConfig.bIsPmcDeviceEngine = NV_FALSE;
    falconConfig.physEngDesc        = ENG_SEC2;
    falconConfig.ctxAttr            = NV_MEMORY_UNCACHED;
    falconConfig.ctxBufferSize      = FLCN_CTX_ENG_BUFFER_SIZE_HW << 4;
    falconConfig.addrSpaceList      = memdescAddrSpaceListToU32(ADDRLIST_FBMEM_PREFERRED);

    kflcnConfigureEngine(pGpu, staticCast(pKernelSec2, KernelFalcon), &falconConfig);
    return NV_OK;
}

/*!
 * Returns the SEC2 fuse version of the provided ucode id (1-indexed)
 *
 * @param      pGpu         OBJGPU pointer
 * @param      pKernelSec2  KernelSec2 pointer
 * @param[in]  ucodeId      Ucode Id (1-indexed) to read fuse for
 */
NvU32
ksec2ReadUcodeFuseVersion_GA100
(
    OBJGPU *pGpu,
    KernelSec2 *pKernelSec2,
    const NvU32 ucodeId
)
{
    NvU32 fuseVal = 0;
    NvU32 index = ucodeId - 1;  // adjust to 0-indexed

    // TODO: Bug 3519329: switch to indexed register once available
    // if (index < NV_FUSE_OPT_FPF_SEC2_UCODE_VERSION__SIZE_1)
    if (index < 16)
    {
        // fuseVal = GPU_REG_IDX_RD_DRF(pGpu, _FUSE, _OPT_FPF_SEC2_UCODE_VERSION, index, _DATA);
        fuseVal = GPU_REG_RD32(pGpu, NV_FUSE_OPT_FPF_SEC2_UCODE1_VERSION + (4 * index));

        if (fuseVal)
        {
            HIGHESTBITIDX_32(fuseVal);
            fuseVal = fuseVal + 1;
        }
    }

    return fuseVal;
}
