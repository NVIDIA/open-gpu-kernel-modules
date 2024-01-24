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

/*!
 * Provides GA102+ specific KernelGsp HAL implementations.
 */

#include "gpu/gsp/kernel_gsp.h"

#include "gpu/conf_compute/conf_compute.h"
#include "gpu/mem_mgr/rm_page_size.h"
#include "nverror.h"
#include "rmgspseq.h"

#include "published/ampere/ga102/dev_falcon_v4.h"
#include "published/ampere/ga102/dev_riscv_pri.h"
#include "published/ampere/ga102/dev_falcon_second_pri.h"
#include "published/ampere/ga102/dev_gsp.h"
#include "published/ampere/ga102/dev_gsp_addendum.h"
#include "published/ampere/ga102/dev_gc6_island.h"
#include "published/ampere/ga102/dev_gc6_island_addendum.h"
#include "gpu/sec2/kernel_sec2.h"

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
    }

    kflcnConfigureEngine(pGpu, staticCast(pKernelGsp, KernelFalcon), &falconConfig);
}

/*!
 * Determine if GSP reload via SEC2 is completed.
 */
static NvBool
_kgspIsReloadCompleted
(
    OBJGPU  *pGpu,
    void    *pVoid
)
{
    NvU32 reg;

    reg = GPU_REG_RD32(pGpu, NV_PGC6_BSI_SECURE_SCRATCH_14);

    return FLD_TEST_DRF(_PGC6, _BSI_SECURE_SCRATCH_14, _BOOT_STAGE_3_HANDOFF, _VALUE_DONE, reg);
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
        *ppBinStorageImage = (BINDATA_STORAGE *)bindataArchiveGetStorage(pBinArchive, "ucode_image_dbg");
        *ppBinStorageDesc  = (BINDATA_STORAGE *)bindataArchiveGetStorage(pBinArchive, "ucode_desc_dbg");
    }
    else
    {
        *ppBinStorageImage = (BINDATA_STORAGE *)bindataArchiveGetStorage(pBinArchive, "ucode_image_prod");
        *ppBinStorageDesc  = (BINDATA_STORAGE *)bindataArchiveGetStorage(pBinArchive, "ucode_desc_prod");
    }
}

/*!
 * Execute GSP sequencer operation
 *
 * @param[in]   pGpu            GPU object pointer
 * @param[in]   pKernelGsp      KernelGsp object pointer
 * @param[in]   opCode          Sequencer opcode
 * @param[in]   pPayload        Pointer to payload
 * @param[in]   payloadSize     Size of payload in bytes
 *
 * @return NV_OK if the sequencer operation was successful.
 *         Appropriate NV_ERR_xxx value otherwise.
 */
NV_STATUS
kgspExecuteSequencerCommand_GA102
(
    OBJGPU         *pGpu,
    KernelGsp      *pKernelGsp,
    NvU32           opCode,
    NvU32          *pPayload,
    NvU32           payloadSize
)
{
    NV_STATUS       status        = NV_OK;
    KernelFalcon   *pKernelFalcon = staticCast(pKernelGsp, KernelFalcon);
    NvU32           secMailbox0   = 0;

    switch (opCode)
    {
        case GSP_SEQ_BUF_OPCODE_CORE_RESUME:
        {
            RM_RISCV_UCODE_DESC *pRiscvDesc = pKernelGsp->pGspRmBootUcodeDesc;

            NV_ASSERT_OR_RETURN(payloadSize == 0, NV_ERR_INVALID_ARGUMENT);

            KernelFalcon *pKernelSec2Falcon = staticCast(GPU_GET_KERNEL_SEC2(pGpu), KernelFalcon);

            pKernelGsp->bLibosLogsPollingEnabled = NV_FALSE;

            kflcnResetIntoRiscv_HAL(pGpu, pKernelFalcon);
            kgspProgramLibosBootArgsAddr_HAL(pGpu, pKernelGsp);

            NV_PRINTF(LEVEL_INFO, "---------------Starting SEC2 to resume GSP-RM------------\n");
            // Start SEC2 in order to resume GSP-RM
            kflcnStartCpu_HAL(pGpu, pKernelSec2Falcon);

            // Wait for reload to be completed.
            status = gpuTimeoutCondWait(pGpu, _kgspIsReloadCompleted, NULL, NULL);

            // Check SEC mailbox.
            secMailbox0 = kflcnRegRead_HAL(pGpu, pKernelSec2Falcon, NV_PFALCON_FALCON_MAILBOX0);

            if ((status != NV_OK) || (secMailbox0 != NV_OK))
            {
                NV_PRINTF(LEVEL_ERROR, "Timeout waiting for SEC2-RTOS to resume GSP-RM. SEC2 Mailbox0 is : 0x%x\n", secMailbox0);
                DBG_BREAKPOINT();
                return NV_ERR_TIMEOUT;
            }

            pKernelGsp->bLibosLogsPollingEnabled = NV_TRUE;

            // Program FALCON_OS
            kflcnRegWrite_HAL(pGpu, pKernelFalcon, NV_PFALCON_FALCON_OS, pRiscvDesc->appVersion);

            // Ensure the CPU is started
            if (kflcnIsRiscvActive_HAL(pGpu, pKernelFalcon))
            {
                NV_PRINTF(LEVEL_INFO, "GSP ucode loaded and RISCV started.\n");
            }
            else
            {
                NV_ASSERT_FAILED("Failed to boot GSP");
                status = NV_ERR_NOT_READY;
            }
            break;
        }
        default:
        {
            status = NV_ERR_INVALID_ARGUMENT;
            break;
        }
    }

    return status;
}
