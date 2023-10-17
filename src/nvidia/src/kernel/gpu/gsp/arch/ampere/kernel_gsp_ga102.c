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

#include "gpu/bus/kern_bus.h"
#include "gpu/conf_compute/conf_compute.h"
#include "nverror.h"
#include "rmgspseq.h"
#include "vgpu/rpc.h"

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
    // Don't bother enabling the host-side decoding either, as CrashCat
    // currently only supports sysmem queue reporting on GA10x+.
    //
    if (pCC == NULL || !pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED))
    {
        // Enable CrashCat monitoring
        falconConfig.crashcatEngConfig.bEnable = NV_TRUE;
        falconConfig.crashcatEngConfig.pName = MAKE_NV_PRINTF_STR("GSP");
        falconConfig.crashcatEngConfig.errorId = GSP_ERROR;
        falconConfig.crashcatEngConfig.allocQueueSize = RM_PAGE_SIZE;
    }

    kflcnConfigureEngine(pGpu, staticCast(pKernelGsp, KernelFalcon), &falconConfig);
}

/*!
 * Reset RISCV using secure reset
 *
 * @return NV_OK if the RISCV reset was successful.
 *         Appropriate NV_ERR_xxx value otherwise.
 */
static NV_STATUS
_kgspResetIntoRiscv
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    KernelFalcon *pKernelFlcn = staticCast(pKernelGsp, KernelFalcon);
    NV_ASSERT_OK_OR_RETURN(kflcnPreResetWait_HAL(pGpu, pKernelFlcn));

    GPU_FLD_WR_DRF_DEF(pGpu, _PGSP, _FALCON_ENGINE, _RESET, _TRUE);

    // Reg read cycles needed for signal propagation.
    for (NvU32 i = 0; i < FLCN_RESET_PROPAGATION_DELAY_COUNT; i++)
    {
        GPU_REG_RD32(pGpu, NV_PGSP_FALCON_ENGINE);
    }

    GPU_FLD_WR_DRF_DEF(pGpu, _PGSP, _FALCON_ENGINE, _RESET, _FALSE);

    // Reg read cycles needed for signal propagation.
    for (NvU32 i = 0; i < FLCN_RESET_PROPAGATION_DELAY_COUNT; i++)
    {
        GPU_REG_RD32(pGpu, NV_PGSP_FALCON_ENGINE);
    }

    NV_ASSERT_OK_OR_RETURN(kflcnWaitForResetToFinish_HAL(pGpu, pKernelFlcn));

    kflcnRiscvProgramBcr_HAL(pGpu, pKernelFlcn, NV_TRUE);

    return NV_OK;
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

/*!
 * Boot GSP-RM.
 *
 * This routine handles the following:
 *   - prepares boot binary image
 *   - prepares RISCV core to run GSP-RM
 *   - prepares libos initialization args
 *   - prepares GSP-RM initialization message
 *   - starts the RISCV core and passes control to boot binary image
 *   - waits for GSP-RM to complete initialization
 *
 * Note that boot binary and GSP-RM images have already been placed
 * in fbmem by kgspCalculateFbLayout_HAL().
 *
 * Note that this routine is based on flcnBootstrapRiscvOS_GA102().
 *
 * @param[in]   pGpu            GPU object pointer
 * @param[in]   pKernelGsp      GSP object pointer
 * @param[in]   pGspFw          GSP_FIRMWARE image pointer
 *
 * @return NV_OK if GSP-RM RISCV boot was successful.
 *         Appropriate NV_ERR_xxx value otherwise.
 */
NV_STATUS
kgspBootstrapRiscvOSEarly_GA102
(
    OBJGPU         *pGpu,
    KernelGsp      *pKernelGsp,
    GSP_FIRMWARE   *pGspFw
)
{
    KernelFalcon           *pKernelFalcon   = staticCast(pKernelGsp, KernelFalcon);
    NV_STATUS               status          = NV_OK;

    // Only for GSP client builds
    if (!IS_GSP_CLIENT(pGpu))
    {
        NV_PRINTF(LEVEL_ERROR, "IS_GSP_CLIENT is not set.\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    if (!kflcnIsRiscvCpuEnabled_HAL(pGpu, pKernelFalcon))
    {
        NV_PRINTF(LEVEL_ERROR, "RISC-V core is not enabled.\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    kgspPopulateGspRmInitArgs(pGpu, pKernelGsp, NULL);

    {
        // Execute FWSEC to setup FRTS if we have a FRTS region
        if (kgspGetFrtsSize_HAL(pGpu, pKernelGsp) > 0)
        {
            kflcnReset_HAL(pGpu, pKernelFalcon);

            NV_ASSERT_OK_OR_GOTO(status,
                kgspExecuteFwsecFrts_HAL(pGpu, pKernelGsp, pKernelGsp->pFwsecUcode,
                                        pKernelGsp->pWprMeta->frtsOffset), exit);
        }
    }

    NV_ASSERT_OK_OR_RETURN(_kgspResetIntoRiscv(pGpu, pKernelGsp));

    //
    // Stuff the message queue with async init messages that will be run
    // before OBJGPU is created.
    //
    NV_RM_RPC_GSP_SET_SYSTEM_INFO(pGpu, status);
    if (status != NV_OK)
    {
        NV_ASSERT_OK_FAILED("NV_RM_RPC_GSP_SET_SYSTEM_INFO", status);
        goto exit;
    }

    NV_RM_RPC_SET_REGISTRY(pGpu, status);
    if (status != NV_OK)
    {
        NV_ASSERT_OK_FAILED("NV_RM_RPC_SET_REGISTRY", status);
        goto exit;
    }

    // First times setup of libos init args
    kgspSetupLibosInitArgs(pGpu, pKernelGsp);

    // Fb configuration is done so setup libos arg list
    kgspProgramLibosBootArgsAddr_HAL(pGpu, pKernelGsp);

    // Execute Scrubber if needed
    if (pKernelGsp->pScrubberUcode != NULL)
    {
        NV_ASSERT_OK_OR_GOTO(status,
                             kgspExecuteScrubberIfNeeded_HAL(pGpu, pKernelGsp),
                             exit);
    }

    RM_RISCV_UCODE_DESC *pRiscvDesc = pKernelGsp->pGspRmBootUcodeDesc;

    {
        status = kgspExecuteBooterLoad_HAL(pGpu, pKernelGsp,
            memdescGetPhysAddr(pKernelGsp->pWprMetaDescriptor, AT_GPU, 0));
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to execute Booter Load (ucode for initial boot): 0x%x\n", status);
            goto exit;
        }
    }

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
        goto exit;
    }

    NV_PRINTF(LEVEL_INFO, "Waiting for GSP fw RM to be ready...\n");

    // Link the status queue.
    NV_ASSERT_OK_OR_GOTO(status, GspStatusQueueInit(pGpu, &pKernelGsp->pRpc->pMessageQueueInfo),
                          exit);

    NV_ASSERT_OK_OR_GOTO(status, kgspWaitForRmInitDone(pGpu, pKernelGsp),
                          exit);

    NV_PRINTF(LEVEL_INFO, "GSP FW RM ready.\n");

exit:
    NV_ASSERT(status == NV_OK);

    return status;
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

            {
                KernelFalcon *pKernelSec2Falcon = staticCast(GPU_GET_KERNEL_SEC2(pGpu), KernelFalcon);

                pKernelGsp->bLibosLogsPollingEnabled = NV_FALSE;

                NV_ASSERT_OK_OR_RETURN(_kgspResetIntoRiscv(pGpu, pKernelGsp));
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
            }

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
