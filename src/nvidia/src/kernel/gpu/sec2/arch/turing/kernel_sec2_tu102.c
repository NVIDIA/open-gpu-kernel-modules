/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "core/bin_data.h"
#include "core/core.h"
#include "gpu/falcon/kernel_falcon.h"
#include "gpu/gpu.h"
#include "os/nv_memory_type.h"

#include "published/turing/tu102/dev_sec_pri.h"
#include "published/turing/tu102/dev_sec_addendum.h"

#include "rmflcnbl.h"

NV_STATUS
ksec2ConfigureFalcon_TU102
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

NV_STATUS
ksec2ResetHw_TU102
(
    OBJGPU *pGpu,
    KernelSec2 *pKernelSec2
)
{
    GPU_FLD_WR_DRF_DEF(pGpu, _PSEC, _FALCON_ENGINE, _RESET, _TRUE);

    // Reg read cycles needed for signal propagation.
    for (NvU32 i = 0; i < FLCN_RESET_PROPAGATION_DELAY_COUNT; i++)
    {
        GPU_REG_RD32(pGpu, NV_PSEC_FALCON_ENGINE);
    }

    GPU_FLD_WR_DRF_DEF(pGpu, _PSEC, _FALCON_ENGINE, _RESET, _FALSE);

    // Reg read cycles needed for signal propagation.
    for (NvU32 i = 0; i < FLCN_RESET_PROPAGATION_DELAY_COUNT; i++)
    {
        GPU_REG_RD32(pGpu, NV_PSEC_FALCON_ENGINE);
    }

    return NV_OK;
}

static NV_STATUS
s_allocateGenericBlUcode
(
    OBJGPU *pGpu,
    KernelSec2 *pKernelSec2,
    const RM_FLCN_BL_DESC **ppDesc,
    const NvU8 **ppImg
)
{
    NV_STATUS status = NV_OK;

    const BINDATA_ARCHIVE *pBinArchive;
    const BINDATA_STORAGE *pBinDesc;
    const BINDATA_STORAGE *pBinImg;
    NvLength descSizeAligned;
    NvLength imgSizeAligned;

    RM_FLCN_BL_DESC *pGenericBlUcodeDesc = NULL;
    NvU8 *pGenericBlUcodeImg = NULL;

    NV_ASSERT_OR_RETURN(ppDesc != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(ppImg != NULL, NV_ERR_INVALID_ARGUMENT);

    pBinArchive = ksec2GetBinArchiveBlUcode_HAL(pGpu, pKernelSec2);
    NV_ASSERT_OR_GOTO(pBinArchive != NULL, out);

    // allocate desc
    pBinDesc = bindataArchiveGetStorage(pBinArchive, "ucode_desc");
    NV_ASSERT_OR_GOTO(pBinDesc != NULL, out);

    descSizeAligned = RM_ALIGN_UP(bindataGetBufferSize(pBinDesc), FLCN_BLK_ALIGNMENT);
    pGenericBlUcodeDesc = portMemAllocNonPaged(descSizeAligned);
    if (pGenericBlUcodeDesc == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto out;
    }

    NV_ASSERT_OK_OR_GOTO(status,
        bindataWriteToBuffer(pBinDesc, (NvU8 *) pGenericBlUcodeDesc, descSizeAligned), out);

    // allocate img
    pBinImg = bindataArchiveGetStorage(pBinArchive, "ucode_image");
    imgSizeAligned = RM_ALIGN_UP(bindataGetBufferSize(pBinImg), FLCN_BLK_ALIGNMENT);

    if (pGenericBlUcodeDesc->blImgHeader.blCodeSize > imgSizeAligned)
    {
        status = NV_ERR_INVALID_DATA;
        goto out;
    }

    pGenericBlUcodeImg = portMemAllocNonPaged(imgSizeAligned);
    if (pGenericBlUcodeImg == NULL)
    {
        status = NV_ERR_NO_MEMORY;
        goto out;
    }

    NV_ASSERT_OK_OR_GOTO(status,
        bindataWriteToBuffer(pBinImg, pGenericBlUcodeImg, imgSizeAligned), out);

    *ppDesc = pGenericBlUcodeDesc;
    *ppImg = pGenericBlUcodeImg;

    return status;

out:
    portMemFree(pGenericBlUcodeDesc);
    portMemFree(pGenericBlUcodeImg);
    return status;
}

/*!
 * Get the generic falcon bootloader ucode descriptor and image
 *
 * Note: this bootloader works for both SEC2 and GSP
 * (though it is defined only on KernelSec2)
 *
 * @param[in]  pGpu         OBJGPU pointer
 * @param[in]  pKernelSec2  KernelSec2 pointer
 * @param[out] ppDesc       pointer to ucode descriptor
 * @param[out] ppImg        pointer to ucode image
 */
NV_STATUS
ksec2GetGenericBlUcode_TU102
(
    OBJGPU *pGpu,
    KernelSec2 *pKernelSec2,
    const RM_FLCN_BL_DESC **ppDesc,
    const NvU8 **ppImg
)
{
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(ppDesc != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(ppImg != NULL, NV_ERR_INVALID_ARGUMENT);

    if (pKernelSec2->pGenericBlUcodeDesc == NULL)
    {
        NV_ASSERT_OR_RETURN(pKernelSec2->pGenericBlUcodeImg == NULL, NV_ERR_INVALID_STATE);
        NV_ASSERT_OK_OR_RETURN(
            s_allocateGenericBlUcode(pGpu, pKernelSec2,
                                     &pKernelSec2->pGenericBlUcodeDesc,
                                     &pKernelSec2->pGenericBlUcodeImg));
    }

    NV_ASSERT_OR_RETURN(pKernelSec2->pGenericBlUcodeDesc != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelSec2->pGenericBlUcodeImg != NULL, NV_ERR_INVALID_STATE);

    *ppDesc = pKernelSec2->pGenericBlUcodeDesc;
    *ppImg = pKernelSec2->pGenericBlUcodeImg;

    return status;
}
