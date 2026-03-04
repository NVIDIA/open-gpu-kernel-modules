/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * Provides GB100+ specific KernelGsp HAL implementations.
 */

#include "rmconfig.h"
#include "gpu/falcon/kernel_falcon.h"
#include "gpu/gsp/kernel_gsp.h"
#include "gpu/fsp/kern_fsp.h"
#include "gpu/rc/kernel_rc.h"

#include "published/blackwell/gb100/dev_gsp.h"
#include "published/blackwell/gb100/dev_fuse_zb.h"
#include "published/blackwell/gb100/hwproject.h"

#include "gpu/conf_compute/conf_compute.h"

/*!
 * Helper Function for kgspResetHw_GH100
 *
 * @param[in]  pGpu    OBJGPU pointer
 * @param[in]  pVoid   unused void pointer
 *
 * @return NvBool NV_TRUE if reset PLM reports READ enabled
 */
static NvBool
_kgspWaitForResetAccess
(
    OBJGPU *pGpu,
    void   *pVoid
)
{
    return (GPU_FLD_TEST_DRF_DEF(pGpu, _PGSP, _FALCON_RESET_PRIV_LEVEL_MASK,
                                 _READ_PROTECTION_LEVEL0, _ENABLE) &&
            GPU_FLD_TEST_DRF_DEF(pGpu, _PGSP, _FALCON_RESET_PRIV_LEVEL_MASK,
                                 _WRITE_PROTECTION_LEVEL0, _ENABLE));
}

/*!
 * Reset the GSP HW
 *
 * @return NV_OK if the GSP HW was properly reset
 */
NV_STATUS
kgspResetHw_GB100
(
    OBJGPU      *pGpu,
    KernelGsp   *pKernelGsp
)
{
    NV_STATUS status = NV_OK;
    RMTIMEOUT timeout;

    //
    // Add check to see if RM can read Reset
    // Required to allow GSP to lower PLM to allow RM
    // reset access during suspend/shutdown path
    //
    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, GPU_TIMEOUT_FLAGS_TMR);
    status = gpuTimeoutCondWait(pGpu, _kgspWaitForResetAccess, NULL, &timeout);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Timed out waiting for GSP reset PLM to be lowered\n");
        DBG_BREAKPOINT();
    }

    return kgspResetHw_GH100(pGpu, pKernelGsp);
}

/*!
 * Get signature section prefix
 *
 * @return const char* to the section name prefix
 */
const char*
kgspGetSignatureSectionNamePrefix_GB100
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    ConfidentialCompute *pCC = GPU_GET_CONF_COMPUTE(pGpu);
    if (pCC != NULL && pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED))
    {
        return GSP_CC_SIGNATURE_SECTION_NAME_PREFIX;
    }

    return GSP_SIGNATURE_SECTION_NAME_PREFIX;
}

/*!
 * Get GSP RM Ucode bin
 *
 */
void
kgspGetGspRmBootUcodeStorage_GB100
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    BINDATA_STORAGE **ppBinStorageImage,
    BINDATA_STORAGE **ppBinStorageDesc
)
{
        ConfidentialCompute *pCC = GPU_GET_CONF_COMPUTE(pGpu);
        if (pCC != NULL && pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED))
        {
            const BINDATA_ARCHIVE *pBinArchiveConcatenatedFMCDesc = kgspGetBinArchiveConcatenatedFMCDesc_HAL(pKernelGsp);
            const BINDATA_ARCHIVE *pBinArchiveConcatenatedFMC     = kgspGetBinArchiveConcatenatedFMC_HAL(pKernelGsp);

            if (kgspIsDebugModeEnabled(pGpu, pKernelGsp))
            {
                *ppBinStorageImage = (BINDATA_STORAGE *)bindataArchiveGetStorage(pBinArchiveConcatenatedFMC, BINDATA_LABEL_UCODE_IMAGE_DBG);
                *ppBinStorageDesc  = (BINDATA_STORAGE *)bindataArchiveGetStorage(pBinArchiveConcatenatedFMCDesc, BINDATA_LABEL_UCODE_DESC_DBG);
            }
            else
            {
                *ppBinStorageImage = (BINDATA_STORAGE *)bindataArchiveGetStorage(pBinArchiveConcatenatedFMC, BINDATA_LABEL_UCODE_IMAGE_PROD);
                *ppBinStorageDesc  = (BINDATA_STORAGE *)bindataArchiveGetStorage(pBinArchiveConcatenatedFMCDesc, BINDATA_LABEL_UCODE_DESC_PROD);
            }

            return;
        }

    KernelFsp *pKernelFsp = GPU_GET_KERNEL_FSP(pGpu);
    if (pKernelFsp != NULL)
    {
    }

    kgspGetGspRmBootUcodeStorage_GA102(pGpu, pKernelGsp, ppBinStorageImage, ppBinStorageDesc);
}

/*!
 * Handle GSP Fatal Errors
 */
void
kgspServiceFatalHwError_GB100
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp,
    NvU32      intrStatus
)
{
    KernelFalcon *pKernelFlcn     = staticCast(pKernelGsp, KernelFalcon);
    KernelRc     *pKernelRc       = GPU_GET_KERNEL_RC(pGpu);
    NvU32         errorStatus     = 0;
    NvU32         errorCodeBitIdx = 0;

    if (!FLD_TEST_DRF(_PGSP_FALCON, _IRQSTAT, _FATAL_ERROR, _TRUE, intrStatus))
    {
        return;
    }

    // Must be set before logging any Xids to skip the GSP RPC
    pKernelGsp->bFatalError = NV_TRUE;

    if (kflcnGetFatalHwErrorStatus_HAL(pGpu, pKernelFlcn, &errorStatus) != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "NV_PGSP_FALCON_IRQSTAT_FATAL_ERROR unknown error pending\n");
        MODS_ARCH_ERROR_PRINTF("NV_PGSP_FALCON_IRQSTAT_FATAL_ERROR unknown error pending\n");

        nvErrorLog_va((void *)pGpu, ROBUST_CHANNEL_CONTAINED_ERROR, "GSP-RISCV instance 0 unknown fatal error");

        goto done;
    }

    FOR_EACH_INDEX_IN_MASK(32, errorCodeBitIdx, errorStatus)
    {
        NvU32       errorCode  = NVBIT(errorCodeBitIdx);
        const char *pErrorName = kflcnFatalHwErrorCodeToString_HAL(pGpu, pKernelFlcn, errorCode, NV_FALSE);

#if NV_PRINTF_STRINGS_ALLOWED
        {
            const char *pErrorNameNvPrintf = kflcnFatalHwErrorCodeToString_HAL(pGpu, pKernelFlcn, errorCode, NV_TRUE);

            NV_PRINTF(LEVEL_ERROR, "NV_PGSP_FALCON_IRQSTAT_FATAL_ERROR %s pending (mask: 0x%x)\n",
                      pErrorNameNvPrintf, errorStatus);
            MODS_ARCH_ERROR_PRINTF("NV_PGSP_FALCON_IRQSTAT_FATAL_ERROR=0x%x\n", errorStatus);
        }
#endif // NV_PRINTF_STRINGS_ALLOWED

        // Poison error
        if (FLD_TEST_DRF(_PGSP, _RISCV_FAULT_CONTAINMENT_SRCSTAT, _GLOBAL_MEM, _FAULTED, errorCode))
        {
            NV_ERROR_CONT_LOCATION loc = { 0 };

            loc.locType = NV_ERROR_CONT_LOCATION_TYPE_NONE;

            //
            // Assert since this interrupt can't be cleared without a
            // GPU reset and we shouldn't see this if poison is disabled
            //
            if (!gpuIsGlobalPoisonFuseEnabled(pGpu))
            {
                NV_ASSERT_FAILED("GSP poison pending when poison is disabled");
            }

            NV_ASSERT_OK(gpuUpdateErrorContainmentState_HAL(pGpu, NV_ERROR_CONT_ERR_ID_E24_GSP_POISON, loc, NULL));
        }
        else
        {
            nvErrorLog_va((void *)pGpu, ROBUST_CHANNEL_CONTAINED_ERROR,
                          "GSP-RISCV instance 0 %s fatal error (mask: 0x%x)",
                          pErrorName, errorStatus);
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

done:
    if (pKernelRc != NULL)
    {
        krcRcAndNotifyAllChannels(pGpu, pKernelRc, ROBUST_CHANNEL_CONTAINED_ERROR, NV_TRUE);
    }

    NV_ASSERT_OK(gpuMarkDeviceForReset(pGpu));
}

/*!
 * Check if the GSP is in debug mode
 *
 * @return whether the GSP is in debug mode or not
 */
NvBool
kgspIsDebugModeEnabled_GB100
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    NvU32 data = GPU_REG_RD32(pGpu, NV_FUSE0_PRI_BASE + NV_FUSE_ZB_OPT_SECURE_GSP_DEBUG_DIS);
    return FLD_TEST_DRF(_FUSE_ZB, _OPT_SECURE_GSP_DEBUG_DIS, _DATA, _NO, data);
}

/*!
 * Returns the GSP fuse version of the provided ucode id (1-indexed)
 *
 * @param      pGpu         OBJGPU pointer
 * @param      pKernelGsp   KernelGsp pointer
 * @param[in]  ucodeId      Ucode Id (1-indexed) to read fuse for
 */
NvU32
kgspReadUcodeFuseVersion_GB100
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    const NvU32 ucodeId
)
{
    NvU32 fuseVal = 0;
    NvU32 index = ucodeId - 1;  // adjust to 0-indexed

    if (index < DRF_SIZE(NV_FUSE_ZB_OPT_FPF_GSP_UCODE1_VERSION_DATA))
    {
        fuseVal = GPU_REG_RD32(pGpu, NV_FUSE0_PRI_BASE + NV_FUSE_ZB_OPT_FPF_GSP_UCODE1_VERSION + (4 * index));
        if (fuseVal)
        {
            HIGHESTBITIDX_32(fuseVal);
            fuseVal = fuseVal + 1;
        }
    }

    return fuseVal;
}