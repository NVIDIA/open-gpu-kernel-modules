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

/*!
 * Provides GH100+ specific KernelGsp HAL implementations.
 */

#include "rmconfig.h"
#include "gpu/fsp/kern_fsp.h"
#include "gpu/gsp/kernel_gsp.h"
#include "gsp/gspifpub.h"
#include "vgpu/rpc.h"

#include "published/hopper/gh100/dev_falcon_v4.h"
#include "published/hopper/gh100/dev_gsp.h"
#include "published/hopper/gh100/dev_riscv_pri.h"

#define RISCV_BR_ADDR_ALIGNMENT                 (8)

const char*
kgspGetSignatureSectionNamePrefix_GH100
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    return GSP_SIGNATURE_SECTION_NAME_PREFIX;
}


/*!
 * Helper Function for kgspResetHw_GH100
 *
 * @param[in]  pGpu    OBJGPU pointer
 * @param[in]  pVoid   unused void pointer
 *
 * @return NvBool NV_TRUE if reset FSM reports ASSERTED
 */
static NvBool
_kgspWaitForAsserted
(
    OBJGPU *pGpu,
    void   *pVoid
)
{
    return GPU_FLD_TEST_DRF_DEF(pGpu, _PGSP, _FALCON_ENGINE, _RESET_STATUS,
                                _ASSERTED);
}


/*!
 * Helper Function for kgspResetHw_GH100
 *
 * @param[in]  pGpu    OBJGPU pointer
 * @param[in]  pVoid   unused void pointer
 *
 * @return NvBool NV_TRUE if reset FSM reports DEASSERTED
 */
static NvBool
_kgspWaitForDeasserted
(
    OBJGPU *pGpu,
    void   *pVoid
)
{
    return GPU_FLD_TEST_DRF_DEF(pGpu, _PGSP, _FALCON_ENGINE, _RESET_STATUS,
                                _DEASSERTED);
}

/*!
 * Reset the GSP HW
 *
 * @return NV_OK if the GSP HW was properly reset
 */
NV_STATUS
kgspResetHw_GH100
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    NV_STATUS status = NV_OK;
    RMTIMEOUT timeout;

    //
    // Add a delay for 10us. This is a worst case estimate.
    // See bug 200636529 comment 20
    // Use PTIMER instead of the default ostimer since it is much faster than the
    // former and hence does not cause sufficient delay.
    //
    gpuSetTimeout(pGpu, 10, &timeout, GPU_TIMEOUT_FLAGS_TMR);

    GPU_FLD_WR_DRF_DEF(pGpu, _PGSP, _FALCON_ENGINE, _RESET, _ASSERT);
    status = gpuTimeoutCondWait(pGpu, _kgspWaitForAsserted, NULL, &timeout);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Timed out waiting for GSP falcon reset to assert\n");
        DBG_BREAKPOINT();
    }

    // Reset timeout
    gpuSetTimeout(pGpu, 10, &timeout, GPU_TIMEOUT_FLAGS_TMR);

    GPU_FLD_WR_DRF_DEF(pGpu, _PGSP, _FALCON_ENGINE, _RESET, _DEASSERT);
    status = gpuTimeoutCondWait(pGpu, _kgspWaitForDeasserted, NULL, &timeout);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Timed out waiting for GSP falcon reset to deassert\n");
        DBG_BREAKPOINT();
    }

    return status;
}

NV_STATUS
kgspAllocBootArgs_GH100
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    NvP64 pVa = NvP64_NULL;
    NvP64 pPriv = NvP64_NULL;
    NV_STATUS nvStatus = NV_OK;

    // Allocate GSP-FMC arguments
    NV_ASSERT_OK_OR_GOTO(nvStatus,
                          memdescCreate(&pKernelGsp->pGspFmcArgumentsDescriptor,
                                        pGpu, sizeof(GSP_FMC_BOOT_PARAMS), 0x1000,
                                        NV_TRUE, ADDR_SYSMEM, NV_MEMORY_CACHED,
                                        MEMDESC_FLAGS_NONE),
                          _kgspAllocBootArgs_exit_cleanup);

    NV_ASSERT_OK_OR_GOTO(nvStatus,
                          memdescAlloc(pKernelGsp->pGspFmcArgumentsDescriptor),
                          _kgspAllocBootArgs_exit_cleanup);

    NV_ASSERT_OK_OR_GOTO(nvStatus,
                          memdescMap(pKernelGsp->pGspFmcArgumentsDescriptor, 0,
                                     memdescGetSize(pKernelGsp->pGspFmcArgumentsDescriptor),
                                     NV_TRUE, NV_PROTECT_READ_WRITE,
                                     &pVa, &pPriv),
                          _kgspAllocBootArgs_exit_cleanup);

    pKernelGsp->pGspFmcArgumentsCached = (GSP_FMC_BOOT_PARAMS *)NvP64_VALUE(pVa);
    pKernelGsp->pGspFmcArgumentsMappingPriv = pPriv;

    return kgspAllocBootArgs_TU102(pGpu, pKernelGsp);

_kgspAllocBootArgs_exit_cleanup:
    kgspFreeBootArgs_HAL(pGpu, pKernelGsp);
    return nvStatus;
}

void
kgspFreeBootArgs_GH100
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    kgspFreeBootArgs_TU102(pGpu, pKernelGsp);

    // release wpr meta data resources
    if (pKernelGsp->pGspFmcArgumentsCached != NULL)
    {
        memdescUnmap(pKernelGsp->pGspFmcArgumentsDescriptor,
                     NV_TRUE, osGetCurrentProcess(),
                     (void *)pKernelGsp->pGspFmcArgumentsCached,
                     pKernelGsp->pGspFmcArgumentsMappingPriv);
        pKernelGsp->pGspFmcArgumentsCached = NULL;
        pKernelGsp->pGspFmcArgumentsMappingPriv = NULL;
    }
    if (pKernelGsp->pGspFmcArgumentsDescriptor != NULL)
    {
        memdescFree(pKernelGsp->pGspFmcArgumentsDescriptor);
        memdescDestroy(pKernelGsp->pGspFmcArgumentsDescriptor);
        pKernelGsp->pGspFmcArgumentsDescriptor = NULL;
    }
}

NV_STATUS
kgspWaitForGfwBootOk_GH100
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    KernelFsp *pKernelFsp = GPU_GET_KERNEL_FSP(pGpu);
    return kfspWaitForSecureBoot_HAL(pGpu, pKernelFsp);
}

/*!
 * Calculate the FB layout. On Hopper, this consists of determining the
 * minimum sizes of various regions that Client RM provides as inputs.
 * The actual offsets are determined by secure ACR ucode and patched
 * into the GspFwWprMeta structure provided to GSP-RM, so most of the
 * structure is 0-initialized here.
 *
 * Firmware scrubs the last 256mb of FB, no memory outside of this region
 * may be used until the FW RM has scrubbed the remainder of memory.
 *
 *   ---------------------------- <- fbSize (end of FB, 1M aligned)
 *   | VGA WORKSPACE            |
 *   ---------------------------- <- vbiosReservedOffset  (64K? aligned)
 *   | (potential align. gap)   |
 *   ---------------------------- <- gspFwWprEnd (128K aligned)
 *   | FRTS data                |    (frtsSize is 0 on GA100)
 *   | ------------------------ | <- frtsOffset
 *   | BOOT BIN (e.g. GSP-FMC)  |
 *   ---------------------------- <- bootBinOffset
 *   | GSP FW ELF               |
 *   ---------------------------- <- gspFwOffset
 *   | GSP FW (WPR) HEAP        |
 *   ---------------------------- <- gspFwHeapOffset
 *   | ACR-placed metadata      |
 *   | (struct GspFwWprMeta)    |
 *   ---------------------------- <- gspFwWprStart (128K aligned)
 *   | GSP FW (non-WPR) HEAP    |
 *   ---------------------------- <- nonWprHeapOffset, gspFwRsvdStart
 *                                   (GSP_CARVEOUT_SIZE bytes from end of FB)
 *
 * @param       pGpu          GPU object pointer
 * @param       pKernelGsp    KernelGsp object pointer
 * @param       pGspFw        Pointer to GSP-RM fw image.
 * @param       pFbRegionInfo Pointer to fb region table to fill in.
 */
NV_STATUS
kgspCalculateFbLayout_GH100
(
    OBJGPU         *pGpu,
    KernelGsp      *pKernelGsp,
    GSP_FIRMWARE   *pGspFw
)
{
    GspFwWprMeta        *pWprMeta = pKernelGsp->pWprMeta;
    RM_RISCV_UCODE_DESC *pRiscvDesc = pKernelGsp->pGspRmBootUcodeDesc;

    ct_assert(sizeof(*pWprMeta) == 256);

    NV_ASSERT_OR_RETURN(IS_GSP_CLIENT(pGpu), NV_ERR_NOT_SUPPORTED);

    NV_ASSERT_OR_RETURN(pKernelGsp->pGspRmBootUcodeImage != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelGsp->gspRmBootUcodeSize != 0, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pRiscvDesc != NULL, NV_ERR_INVALID_STATE);

    portMemSet(pWprMeta, 0, sizeof *pWprMeta);

    //
    // We send this to FSP as the size to reserve above FRTS.
    // The actual offset gets filled in by ACR ucode when it sets up WPR2.
    //
    pWprMeta->vgaWorkspaceSize = 128 * 1024;

    // Physical address and size of GSP-FMC ucode in system memory
    pWprMeta->sizeOfBootloader = pKernelGsp->gspRmBootUcodeSize;
    pWprMeta->sysmemAddrOfBootloader =
        memdescGetPhysAddr(pKernelGsp->pGspRmBootUcodeMemdesc, AT_GPU, 0);

    // Physical address and size of GSP-RM firmware in system memory
    pWprMeta->sizeOfRadix3Elf = pGspFw->imageSize;
    pWprMeta->sysmemAddrOfRadix3Elf =
        memdescGetPhysAddr(pKernelGsp->pGspUCodeRadix3Descriptor, AT_GPU, 0);

    // Set necessary info from the GSP-FMC desc
    pWprMeta->bootloaderCodeOffset = pRiscvDesc->monitorCodeOffset;
    pWprMeta->bootloaderDataOffset = pRiscvDesc->monitorDataOffset;
    pWprMeta->bootloaderManifestOffset = pRiscvDesc->manifestOffset;

    if (pKernelGsp->pSignatureMemdesc != NULL)
    {
        pWprMeta->sysmemAddrOfSignature = memdescGetPhysAddr(pKernelGsp->pSignatureMemdesc, AT_GPU, 0);
        pWprMeta->sizeOfSignature = memdescGetSize(pKernelGsp->pSignatureMemdesc);
    }

    // The non-WPR heap lives in front of the WPR region
    pWprMeta->nonWprHeapSize = kgspGetNonWprHeapSize(pGpu, pKernelGsp);

    //
    // The WPR heap size (gspFwHeapSize) is variable to also get any padding needed
    // in the carveout to align the WPR start. We specify a minimum size here.
    //
    pWprMeta->gspFwHeapSize = kgspGetWprHeapSize(pGpu, pKernelGsp);

    // Fill in the meta-metadata
    pWprMeta->revision = GSP_FW_WPR_META_REVISION;
    pWprMeta->magic = GSP_FW_WPR_META_MAGIC;

    return NV_OK;
}

static GSP_DMA_TARGET _kgspMemdescToDmaTarget
(
    MEMORY_DESCRIPTOR *pMemDesc
)
{
    switch (memdescGetAddressSpace(pMemDesc))
    {
        case ADDR_SYSMEM:
            return (NV_MEMORY_CACHED == memdescGetCpuCacheAttrib(pMemDesc)) ? GSP_DMA_TARGET_COHERENT_SYSTEM
                                                                            : GSP_DMA_TARGET_NONCOHERENT_SYSTEM;
        case ADDR_FBMEM:
            return GSP_DMA_TARGET_LOCAL_FB;
        default:
            return GSP_DMA_TARGET_COUNT;
    }
}

/*!
 * @brief Creates and populates the GSP-FMC/ACR arguments for booting GSP-RM.
 */
NV_STATUS
kgspSetupGspFmcArgs_GH100
(
    OBJGPU       *pGpu,
    KernelGsp    *pKernelGsp,
    GSP_FIRMWARE *pGspFw
)
{
    NV_ASSERT_OR_RETURN(IS_GSP_CLIENT(pGpu), NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(pKernelGsp->pGspFmcArgumentsCached != NULL, NV_ERR_INVALID_STATE);

    GSP_FMC_BOOT_PARAMS *pGspFmcBootParams = pKernelGsp->pGspFmcArgumentsCached;

    pGspFmcBootParams->bootGspRmParams.gspRmDescOffset = memdescGetPhysAddr(pKernelGsp->pWprMetaDescriptor, AT_GPU, 0);
    pGspFmcBootParams->bootGspRmParams.gspRmDescSize = sizeof(*pKernelGsp->pWprMeta);
    pGspFmcBootParams->bootGspRmParams.target = _kgspMemdescToDmaTarget(pKernelGsp->pWprMetaDescriptor);
    pGspFmcBootParams->bootGspRmParams.bIsGspRmBoot = NV_TRUE;

    pGspFmcBootParams->gspRmParams.bootArgsOffset = memdescGetPhysAddr(pKernelGsp->pLibosInitArgumentsDescriptor, AT_GPU, 0);
    pGspFmcBootParams->gspRmParams.target = _kgspMemdescToDmaTarget(pKernelGsp->pLibosInitArgumentsDescriptor);

    return NV_OK;
}

/*!
 * Determine if lockdown is released.
 */
static NvBool
_kgspIsLockdownReleased
(
    OBJGPU  *pGpu,
    void    *pVoid
)
{
    KernelGsp *pKernelGsp = reinterpretCast(pVoid, KernelGsp *);
    NvU32 reg;

    reg = kflcnRegRead_HAL(pGpu, staticCast(pKernelGsp, KernelFalcon), NV_PFALCON_FALCON_HWCFG2);

    return FLD_TEST_DRF(_PFALCON, _FALCON_HWCFG2, _RISCV_BR_PRIV_LOCKDOWN,
                        _UNLOCK, reg);
}

/*!
 * Determine if PRIV target mask is unlocked for GSP and BAR0 Decoupler allows GSP access.
 *
 * This is temporary WAR for the PRIV target mask bug 3640831 until we have notification
 * protocol in place (there is no HW mechanism for CPU to check if GSP is open other than
 * reading 0xBADF41YY code).
 *
 * Until the programmed BAR0 decoupler settings are cleared, GSP access is blocked from
 * the CPU so all reads will return 0.
 */
static NvBool
_kgspIsTargetMaskReleased
(
    OBJGPU  *pGpu,
    void    *pVoid
)
{
    KernelGsp *pKernelGsp               = reinterpretCast(pVoid, KernelGsp *);
    KernelFalcon *pKernelFalcon         = staticCast(pKernelGsp, KernelFalcon);
    const NvU32 privErrTargetLocked     = 0xBADF4100U;
    const NvU32 privErrTargetLockedMask = 0xFFFFFF00U; // Ignore LSB - it has extra error information
    NvU32 reg;

    //
    // This register is read with the raw OS read to avoid the 0xbadf sanity checking
    // done by the usual register read utilities.
    //
    reg = osDevReadReg032(pGpu, gpuGetDeviceMapping(pGpu, DEVICE_INDEX_GPU, 0),
                          pKernelFalcon->registerBase + NV_PFALCON_FALCON_HWCFG2);

    return ((reg != 0) && ((reg & privErrTargetLockedMask) != privErrTargetLocked));
}


static void
_kgspBootstrapGspFmc_GH100
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    KernelFalcon *pKernelFalcon = staticCast(pKernelGsp, KernelFalcon);
    RmPhysAddr physAddr;

    // Reset the GSP to prepare for RISCV bootstrap
    kgspResetHw_HAL(pGpu, pKernelGsp);

    // Stuff the GSP-FMC arguments into the mailbox regs
    physAddr = memdescGetPhysAddr(pKernelGsp->pGspFmcArgumentsDescriptor, AT_GPU, 0);
    kflcnRegWrite_HAL(pGpu, pKernelFalcon, NV_PFALCON_FALCON_MAILBOX0, NvU64_LO32(physAddr));
    kflcnRegWrite_HAL(pGpu, pKernelFalcon, NV_PFALCON_FALCON_MAILBOX1, NvU64_HI32(physAddr));

    // Bootstrap the GSP-FMC by pointing the GSP's BootROM at it
    RM_RISCV_UCODE_DESC *pRiscvDesc = pKernelGsp->pGspRmBootUcodeDesc;
    RmPhysAddr fmcPhysAddr = memdescGetPhysAddr(pKernelGsp->pGspRmBootUcodeMemdesc, AT_GPU, 0);

    // Program FMC code DMAADDR
    physAddr = (fmcPhysAddr + pRiscvDesc->monitorCodeOffset) >> RISCV_BR_ADDR_ALIGNMENT;
    kflcnRiscvRegWrite_HAL(pGpu, pKernelFalcon, NV_PRISCV_RISCV_BCR_DMAADDR_FMCCODE_LO,
                           NvU64_LO32(physAddr));
    kflcnRiscvRegWrite_HAL(pGpu, pKernelFalcon, NV_PRISCV_RISCV_BCR_DMAADDR_FMCCODE_HI,
                           NvU64_HI32(physAddr));

    // Program FMC Data DMAADDR
    physAddr = (fmcPhysAddr + pRiscvDesc->monitorDataOffset) >> RISCV_BR_ADDR_ALIGNMENT;
    kflcnRiscvRegWrite_HAL(pGpu, pKernelFalcon, NV_PRISCV_RISCV_BCR_DMAADDR_FMCDATA_LO,
                           NvU64_LO32(physAddr));
    kflcnRiscvRegWrite_HAL(pGpu, pKernelFalcon, NV_PRISCV_RISCV_BCR_DMAADDR_FMCDATA_HI,
                           NvU64_HI32(physAddr));

    // Program manifest DMAADDR
    physAddr = (fmcPhysAddr + pRiscvDesc->manifestOffset) >> RISCV_BR_ADDR_ALIGNMENT;
    kflcnRiscvRegWrite_HAL(pGpu, pKernelFalcon, NV_PRISCV_RISCV_BCR_DMAADDR_PKCPARAM_LO,
                           NvU64_LO32(physAddr));
    kflcnRiscvRegWrite_HAL(pGpu, pKernelFalcon, NV_PRISCV_RISCV_BCR_DMAADDR_PKCPARAM_HI,
                           NvU64_HI32(physAddr));

    NvU32 target = NV_PRISCV_RISCV_BCR_DMACFG_TARGET_COHERENT_SYSMEM;
    NvU32 dmaCfg = DRF_NUM(_PRISCV_RISCV, _BCR_DMACFG, _TARGET, target) |
                   DRF_DEF(_PRISCV_RISCV, _BCR_DMACFG, _LOCK,   _LOCKED);

    kflcnRiscvRegWrite_HAL(pGpu, pKernelFalcon, NV_PRISCV_RISCV_BCR_DMACFG, dmaCfg);

    kflcnRiscvProgramBcr_HAL(pGpu, pKernelFalcon, NV_TRUE);

    // Start it
    kflcnRiscvRegWrite_HAL(pGpu, pKernelFalcon, NV_PRISCV_RISCV_CPUCTL,
                           DRF_DEF(_PRISCV_RISCV, _CPUCTL, _STARTCPU, _TRUE));
}

/*!
 * Boot GSP-RM.
 *
 * This routine handles the following:
 *   - prepares RISCV core to run GSP-RM
 *   - prepares libos initialization args
 *   - prepares GSP-RM initialization message
 *   - starts the RISCV core and passes control to boot binary image
 *   - waits for GSP-RM to complete initialization
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
kgspBootstrapRiscvOSEarly_GH100
(
    OBJGPU         *pGpu,
    KernelGsp      *pKernelGsp,
    GSP_FIRMWARE   *pGspFw
)
{
    KernelFalcon *pKernelFalcon = staticCast(pKernelGsp, KernelFalcon);
    KernelFsp *pKernelFsp = GPU_GET_KERNEL_FSP(pGpu);
    NV_STATUS     status        = NV_OK;

    // Only for GSP client builds
    if (!IS_GSP_CLIENT(pGpu))
    {
        NV_PRINTF(LEVEL_ERROR, "IS_GSP_CLIENT is not set.\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    // Setup the descriptors that GSP-FMC needs to boot GSP-RM
    NV_ASSERT_OK_OR_RETURN(kgspSetupGspFmcArgs_HAL(pGpu, pKernelGsp, pGspFw));

    kgspSetupLibosInitArgs(pGpu, pKernelGsp);

    // Fill in the GSP-RM message queue init parameters
    kgspPopulateGspRmInitArgs(pGpu, pKernelGsp, NULL);

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

    if (pKernelFsp != NULL && !pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_DISABLE_GSPFMC))
    {
        NV_PRINTF(LEVEL_ERROR, "Starting to boot GSP via FSP.\n");
        pKernelFsp->setProperty(pKernelFsp, PDB_PROP_KFSP_GSP_MODE_GSPRM, NV_TRUE);
        NV_ASSERT_OK_OR_RETURN(kfspSendBootCommands_HAL(pGpu, pKernelFsp));
    }
    else
    {
        _kgspBootstrapGspFmc_GH100(pGpu, pKernelGsp);
    }

    // Wait for target mask to be released.
    status = gpuTimeoutCondWait(pGpu, _kgspIsTargetMaskReleased, pKernelGsp, NULL);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Timeout waiting for GSP target mask release. "
                  "This error may be caused by several reasons: Bootrom may have failed, "
                  "GSP init code may have failed or ACR failed to release target mask. "
                  "RM does not have access to information on which of those conditions happened.\n");

        if (pKernelFsp != NULL && pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_GSP_MODE_GSPRM))
        {
            kfspDumpDebugState_HAL(pGpu, pKernelFsp);
        }

        return status;
    }

    // Wait for lockdown to be released.
    status = gpuTimeoutCondWait(pGpu, _kgspIsLockdownReleased, pKernelGsp, NULL);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Timeout waiting for lockdown release. It's also "
                "possible that bootrom may have failed. RM may not have access to "
                "the BR status to be able to say for sure what failed.\n");

        if (pKernelFsp != NULL && pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_GSP_MODE_GSPRM))
        {
            kfspDumpDebugState_HAL(pGpu, pKernelFsp);
        }

        NV_PRINTF(LEVEL_ERROR, "NV_PGSP_FALCON_MAILBOX0 = 0x%x\n",
                  kflcnRegRead_HAL(pGpu, pKernelFalcon, NV_PFALCON_FALCON_MAILBOX0));
        NV_PRINTF(LEVEL_ERROR, "NV_PGSP_FALCON_MAILBOX1 = 0x%x\n",
                  kflcnRegRead_HAL(pGpu, pKernelFalcon, NV_PFALCON_FALCON_MAILBOX1));
        return status;
    }

    // Start polling for libos logs now that lockdown is released
    pKernelGsp->bLibosLogsPollingEnabled = NV_TRUE;

    // Program FALCON_OS
    RM_RISCV_UCODE_DESC *pRiscvDesc = pKernelGsp->pGspRmBootUcodeDesc;
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
kgspGetGspRmBootUcodeStorage_GH100
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    BINDATA_STORAGE **ppBinStorageImage,
    BINDATA_STORAGE **ppBinStorageDesc
)
{
    kgspGetGspRmBootUcodeStorage_GA102(pGpu, pKernelGsp, ppBinStorageImage, ppBinStorageDesc);
}
