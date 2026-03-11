/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * Provides GA102+ specific KernelGsp HAL implementations.
 */

#include "gpu/gsp/kernel_gsp.h"

#include "gpu/conf_compute/conf_compute.h"
#include "gpu/mem_mgr/rm_page_size.h"
#include "nverror.h"

#include "published/ampere/ga102/dev_falcon_v4.h"
#include "published/ampere/ga102/dev_riscv_pri.h"
#include "published/ampere/ga102/dev_falcon_second_pri.h"
#include "published/ampere/ga102/dev_gsp.h"
#include "published/ampere/ga102/dev_gsp_addendum.h"

#define RISCV_BR_ADDR_ALIGNMENT                 (8)

void
kgspConfigureFalcon_GA102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    KernelFalconEngineConfig falconConfig;

    portMemSet(&falconConfig, 0, sizeof(falconConfig));

    falconConfig.registerBase       = DRF_BASE(NV_PGSP);
    falconConfig.riscvRegisterBase  = NV_FALCON2_GSP_BASE;
    falconConfig.fbifBase           = NV_PGSP_FBIF_BASE;
    falconConfig.bBootFromHs        = NV_TRUE;
    falconConfig.pmcEnableMask      = 0;
    falconConfig.bIsPmcDeviceEngine = NV_FALSE;
    falconConfig.physEngDesc        = ENG_GSP;

    ConfidentialCompute *pCC = GPU_GET_CONF_COMPUTE(pGpu);

    //
    // No CrashCat queue when CC is enabled, as it's not encrypted.
    // Don't bother enabling the host-side decoding either.
    //
    if (pCC == NULL || !pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED))
    {
        // Enable CrashCat monitoring
        falconConfig.crashcatEngConfig.bEnable = NV_TRUE;
        falconConfig.crashcatEngConfig.pName = MAKE_NV_PRINTF_STR("GSP");
        falconConfig.crashcatEngConfig.errorId = GSP_ERROR;
        falconConfig.crashcatEngConfig.allocQueueSize = kgspGetCrashcatSysmemBufferSize(pKernelGsp);
    }

    kflcnConfigureEngine(pGpu, staticCast(pKernelGsp, KernelFalcon), &falconConfig);
}

void
kgspGetGspRmBootUcodeStorage_GA102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    BINDATA_STORAGE **ppBinStorageImage,
    BINDATA_STORAGE **ppBinStorageDesc
)
{
    const BINDATA_ARCHIVE *pBinArchive = kgspGetBinArchiveGspRmBoot_HAL(pKernelGsp);

    if (kgspIsDebugModeEnabled(pGpu, pKernelGsp))
    {
        *ppBinStorageImage = (BINDATA_STORAGE *)bindataArchiveGetStorage(pBinArchive, BINDATA_LABEL_UCODE_IMAGE_DBG);
        *ppBinStorageDesc  = (BINDATA_STORAGE *)bindataArchiveGetStorage(pBinArchive, BINDATA_LABEL_UCODE_DESC_DBG);
    }
    else
    {
        *ppBinStorageImage = (BINDATA_STORAGE *)bindataArchiveGetStorage(pBinArchive, BINDATA_LABEL_UCODE_IMAGE_PROD);
        *ppBinStorageDesc  = (BINDATA_STORAGE *)bindataArchiveGetStorage(pBinArchive, BINDATA_LABEL_UCODE_DESC_PROD);
    }
}
