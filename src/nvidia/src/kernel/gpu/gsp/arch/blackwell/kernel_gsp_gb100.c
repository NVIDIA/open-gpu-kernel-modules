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
#include "gpu/gsp/kernel_gsp.h"
#include "gpu/fsp/kern_fsp.h"

#include "published/blackwell/gb100/dev_gsp.h"

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
    NvU32 errorCode = GPU_REG_RD32(pGpu, NV_PGSP_RISCV_FAULT_CONTAINMENT_SRCSTAT);

    if (!FLD_TEST_DRF(_PGSP_FALCON, _IRQSTAT, _FATAL_ERROR, _TRUE, intrStatus))
    {
        return;
    }

    NV_PRINTF(LEVEL_ERROR, "NV_PGSP_FALCON_IRQSTAT_FATAL_ERROR PENDING error_code 0x%x\n", errorCode);
    MODS_ARCH_ERROR_PRINTF("NV_PGSP_FALCON_IRQSTAT_FATAL_ERROR=0x%x\n", errorCode);

    pKernelGsp->bFatalError = NV_TRUE;

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
        nvErrorLog_va((void *)pGpu, ROBUST_CHANNEL_CONTAINED_ERROR, "GSP-RISCV instance 0 fatal error");
        NV_ASSERT_OK(gpuMarkDeviceForReset(pGpu));
    }

    kgspRcAndNotifyAllChannels(pGpu, pKernelGsp, ROBUST_CHANNEL_CONTAINED_ERROR, NV_TRUE);
}
