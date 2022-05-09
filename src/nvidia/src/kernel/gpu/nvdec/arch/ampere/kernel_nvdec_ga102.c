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

#include "gpu/nvdec/kernel_nvdec.h"

#include "core/core.h"
#include "gpu/falcon/kernel_falcon.h"
#include "gpu/gpu.h"
#include "os/nv_memory_type.h"

#include "published/ampere/ga102/dev_falcon_second_pri.h"
#include "published/ampere/ga100/dev_fuse.h"
#include "published/ampere/ga102/dev_boot.h"
#include "published/ampere/ga102/dev_nvdec_pri.h"
#include "published/ampere/ga102/dev_nvdec_addendum.h"

void
knvdecConfigureFalcon_GA102
(
    OBJGPU *pGpu,
    KernelNvdec *pKernelNvdec
)
{
    KernelFalconEngineConfig falconConfig;

    portMemSet(&falconConfig, 0, sizeof(falconConfig));

    falconConfig.registerBase       = DRF_BASE(NV_PNVDEC(0));
    falconConfig.riscvRegisterBase  = NV_FALCON2_NVDEC0_BASE;
    falconConfig.fbifBase           = NV_PNVDEC_FBIF_BASE(0);
    falconConfig.bBootFromHs        = NV_TRUE;
    falconConfig.pmcEnableMask      = DRF_DEF(_PMC, _ENABLE, _NVDEC, _ENABLED);
    falconConfig.bIsPmcDeviceEngine = NV_TRUE;
    falconConfig.physEngDesc        = ENG_NVDEC(0);
    falconConfig.ctxAttr            = NV_MEMORY_UNCACHED;
    falconConfig.ctxBufferSize      = FLCN_CTX_ENG_BUFFER_SIZE_HW;
    falconConfig.addrSpaceList      = memdescAddrSpaceListToU32(ADDRLIST_FBMEM_PREFERRED);

    kflcnConfigureEngine(pGpu, staticCast(pKernelNvdec, KernelFalcon), &falconConfig);
}

NvBool
knvdecIsEngineDisabled_GA102
(
    OBJGPU *pGpu,
    KernelNvdec *pKernelNvdec
)
{
    NvU32 reg = GPU_REG_RD32(pGpu, NV_FUSE_OPT_NVDEC_DISABLE);
    NvU32 field = DRF_VAL(_FUSE, _OPT_NVDEC_DISABLE, _DATA, reg);
    // highest bit in field will read 1 if NVDEC0 is disabled
    return (NVBIT(DRF_SIZE(NV_FUSE_OPT_NVDEC_DISABLE_DATA) - 1) & field) > 0;
}
