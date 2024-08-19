/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "gpu/conf_compute/conf_compute.h"
#include "spdm/rmspdmtransport.h"
#include "gpu/fsp/kern_fsp.h"
#include "gpu/gsp/kernel_gsp.h"
#include "gpu/pmu/kern_pmu.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gsp/gspifpub.h"
#include "vgpu/rpc.h"
#include "os/os.h"

#include "published/hopper/gh100/dev_falcon_v4.h"
#include "published/hopper/gh100/dev_gsp.h"
#include "published/hopper/gh100/dev_riscv_pri.h"
#include "published/hopper/gh100/dev_vm.h"

#include "gpu/nvlink/kernel_nvlink.h"

#define RISCV_BR_ADDR_ALIGNMENT                 (8)

const char*
kgspGetSignatureSectionNamePrefix_GH100
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    ConfidentialCompute *pCC = GPU_GET_CONF_COMPUTE(pGpu);
    if (pCC != NULL && pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED))
        return GSP_CC_SIGNATURE_SECTION_NAME_PREFIX;
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
    NvU64 flags = MEMDESC_FLAGS_NONE;

    flags |= MEMDESC_FLAGS_ALLOC_IN_UNPROTECTED_MEMORY;

    // Allocate GSP-FMC arguments
    NV_ASSERT_OK_OR_GOTO(nvStatus,
                          memdescCreate(&pKernelGsp->pGspFmcArgumentsDescriptor,
                                        pGpu, sizeof(GSP_FMC_BOOT_PARAMS), 0x1000,
                                        NV_TRUE, ADDR_SYSMEM, NV_MEMORY_CACHED,
                                        flags),
                          _kgspAllocBootArgs_exit_cleanup);

    memdescTagAlloc(nvStatus, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_56,
                    pKernelGsp->pGspFmcArgumentsDescriptor);
    NV_ASSERT_OK_OR_GOTO(nvStatus,
                          nvStatus,
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

NvBool
kgspIsWpr2Up_GH100
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    ConfidentialCompute *pCC = GPU_GET_CONF_COMPUTE(pGpu);
    if (pCC != NULL && pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED))
    {
        //
        // Due to BAR0 decoupler, we may not be able to read WPR2 MMU regs.
        // Assume WPR2 is down.
        //
        return NV_FALSE;
    }

    return kgspIsWpr2Up_TU102(pGpu, pKernelGsp);
}

NV_STATUS
kgspWaitForGfwBootOk_GH100
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    KernelFsp *pKernelFsp = GPU_GET_KERNEL_FSP(pGpu);

    if (pKernelFsp != NULL)
    {
        return kfspWaitForSecureBoot_HAL(pGpu, pKernelFsp);
    }

    return NV_OK;
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
 *   ---------------------------- <- gspFwWprEnd + frtsSize + pmuReservedSize
 *   | PMU mem reservation      |
 *   ---------------------------- <- gspFwWprEnd (128K aligned) + frtsSize
 *   | FRTS data                |
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

    //
    // We send these to FSP as the size to reserve above FRTS.
    // The actual offsets get filled in by ACR ucode when it sets up WPR2.
    //
    pWprMeta->vgaWorkspaceSize = 128 * 1024;
    pWprMeta->pmuReservedSize = kpmuReservedMemorySizeGet(GPU_GET_KERNEL_PMU(pGpu));

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
    // in the carveout to align the WPR start. This is a minimum size request to
    // the GSP-FMC.
    //
    // We won't know the exact size of everything that comes after the heap until
    // after the GSP-FMC lays it all out during boot. At any rate, this value isn't
    // needed on Hopper+, since the GSP-FMC can scrub/unlock anything we would need
    // before GSP-RM boots, so we pass 0 to allow the heap to extend outside the
    // pre-scrubbed area at the end of FB, if needed.
    //
    pWprMeta->gspFwHeapSize = kgspGetFwHeapSize(pGpu, pKernelGsp, 0);

    // Number of VF partitions allocating sub-heaps from the WPR heap
    pWprMeta->gspFwHeapVfPartitionCount =
        pGpu->bVgpuGspPluginOffloadEnabled ? MAX_PARTITIONS_WITH_GFID : 0;

    // CrashCat queue (if allocated in sysmem)
    KernelCrashCatEngine *pKernelCrashCatEng = staticCast(pKernelGsp, KernelCrashCatEngine);
    MEMORY_DESCRIPTOR *pCrashCatQueueMemDesc = kcrashcatEngineGetQueueMemDesc(pKernelCrashCatEng);
    if (pCrashCatQueueMemDesc != NULL)
    {
        NV_ASSERT_CHECKED(memdescGetAddressSpace(pCrashCatQueueMemDesc) == ADDR_SYSMEM);
        pWprMeta->sysmemAddrOfCrashReportQueue = memdescGetPhysAddr(pCrashCatQueueMemDesc, AT_GPU, 0);
        pWprMeta->sizeOfCrashReportQueue = (NvU32)memdescGetSize(pCrashCatQueueMemDesc);
    }

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

    ConfidentialCompute *pCC = GPU_GET_CONF_COMPUTE(pGpu);
    if (pCC != NULL)
    {
        pGspFmcBootParams->initParams.regkeys = pCC->gspProxyRegkeys;
    }

    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pGpu);
    if (pKernelNvlink != NULL)
    {
        pGspFmcBootParams->initParams.regkeys |= pKernelNvlink->gspProxyRegkeys;
    }

    pGspFmcBootParams->bootGspRmParams.gspRmDescOffset = memdescGetPhysAddr(pKernelGsp->pWprMetaDescriptor, AT_GPU, 0);
    pGspFmcBootParams->bootGspRmParams.gspRmDescSize = sizeof(*pKernelGsp->pWprMeta);
    pGspFmcBootParams->bootGspRmParams.target = _kgspMemdescToDmaTarget(pKernelGsp->pWprMetaDescriptor);
    pGspFmcBootParams->bootGspRmParams.bIsGspRmBoot = NV_TRUE;

    pGspFmcBootParams->gspRmParams.bootArgsOffset = memdescGetPhysAddr(pKernelGsp->pLibosInitArgumentsDescriptor, AT_GPU, 0);
    pGspFmcBootParams->gspRmParams.target = _kgspMemdescToDmaTarget(pKernelGsp->pLibosInitArgumentsDescriptor);

    if (pCC != NULL && pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_SPDM_ENABLED))
    {
        NV_STATUS  status = NV_OK;
        Spdm      *pSpdm  = pCC->pSpdm;

        // If SPDM is NULL, we failed to initialize
        if (pCC->pSpdm == NULL)
        {
            return NV_ERR_INVALID_STATE;
        }

        // Perform required pre-GSP-RM boot setup that could not be done on Spdm object creation.
        status = spdmSetupCommunicationBuffers(pGpu, pSpdm);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failure when initializing SPDM messaging infrastructure. Status:0x%x\n", status);
            return status;
        }

        // Target will always be GSP_DMA_TARGET_COHERENT_SYSTEM
        pGspFmcBootParams->gspSpdmParams.target               = GSP_DMA_TARGET_COHERENT_SYSTEM;
        pGspFmcBootParams->gspSpdmParams.payloadBufferOffset  = memdescGetPhysAddr(pSpdm->pPayloadBufferMemDesc,
                                                                                   AT_GPU, 0);
        pGspFmcBootParams->gspSpdmParams.payloadBufferSize    = pSpdm->payloadBufferSize;
    }

    return NV_OK;
}

/*!
 * Determine if PRIV lockdown is released or the FMC has encountered an error.
 */
static NvBool
_kgspLockdownReleasedOrFmcError
(
    OBJGPU  *pGpu,
    void    *pVoid
)
{
    KernelGsp *pKernelGsp = reinterpretCast(pVoid, KernelGsp *);
    KernelFalcon *pKernelFalcon = staticCast(pKernelGsp, KernelFalcon);
    NvU32 hwcfg2, mailbox0;

    //
    // If lockdown has not been released, check NV_PGSP_FALCON_MAILBOX0, where the GSP-FMC
    // (namely ACR) logs error codes during boot. GSP-FMC reported errors are always fatal,
    // so there's no reason to continue polling for lockdown release. This register can be
    // accessed when GSP PRIV is locked down, but not when the GSP PRIV target mask is locked
    // to FSP - so this shouldn't be called until the latter has been lifted.
    //
    // Generally, this mailbox check could fail if the PRIV target mask is *never* locked to FSP,
    // e.g., this isn't a Chain-of-Trust (COT) boot, because we'd be able to read the original
    // boot args addresses stuffed in the mailboxes (before the GSP-FMC has read and cleared
    // them during bootstrap, while the PRIV target mask is typically locked in COT). To avoid
    // premature failure in this case, if NV_PGSP_FALCON_MAILBOX0 is non-zero, check whether the
    // boot args address is still in the mailboxes, and continue to wait, if so.
    //
    mailbox0 = kflcnRegRead_HAL(pGpu, pKernelFalcon, NV_PFALCON_FALCON_MAILBOX0);
    if (mailbox0 != 0)
    {
        //
        // Collision of a real error code with the boot args address should never happen:
        // - the boot args phys address is page-allocated, so at least 4K-aligned, if not more
        // - the GSP-FMC error codes stashed in NV_PGSP_FALCON_MAILBOX0 are effectively 8-bit
        //   and value 0 means "no error"
        //
        NvU64 physAddr = memdescGetPhysAddr(pKernelGsp->pGspFmcArgumentsDescriptor, AT_GPU, 0);
        if ((NvU64_LO32(physAddr) == mailbox0) &&
            (NvU64_HI32(physAddr) == kflcnRegRead_HAL(pGpu, pKernelFalcon, NV_PFALCON_FALCON_MAILBOX1)))
            return NV_FALSE;
    }

    hwcfg2 = kflcnRegRead_HAL(pGpu, pKernelFalcon, NV_PFALCON_FALCON_HWCFG2);

    return (FLD_TEST_DRF(_PFALCON, _FALCON_HWCFG2, _RISCV_BR_PRIV_LOCKDOWN, _UNLOCK, hwcfg2) ||
            (mailbox0 != 0));
}

/*!
 * Determine if SPDM partition has booted or the FMC has encountered an error.
 */
static NvBool
_kgspSpdmBootedOrFmcError
(
    OBJGPU  *pGpu,
    void    *pVoid
)
{
    //
    // The GSP-FMC (namely ACR) logs error codes to NV_PGSP_FALCON_MAILBOX0 during boot.
    // During normal boot, the mailboxes should be 0 (cleared by the GSP-FMC ucode at the
    // start), so consider any non-zero value here a reason to stop polling.
    //
    return (kflcnRegRead_HAL(pGpu, (KernelFalcon *)pVoid, NV_PFALCON_FALCON_MAILBOX0) != 0);
}

static NvBool
_kgspFalconMailbox0Cleared
(
    OBJGPU  *pGpu,
    void    *pVoid
)
{
    return (kflcnRegRead_HAL(pGpu, (KernelFalcon *)pVoid, NV_PFALCON_FALCON_MAILBOX0) == 0);
}

/*!
 * Establish session with SPDM Responder on GSP in Confidential Compute scenario.
 * GSP-RM boot is blocked until session establishment completes.
 *
 * NOTE: This function currently requires the API lock (for at least the async init
 * RPCs and libspdm synchronization) and as such does NOT support parallel init.
 */
static NV_STATUS
_kgspEstablishSpdmSession
(
    OBJGPU              *pGpu,
    KernelGsp           *pKernelGsp,
    ConfidentialCompute *pConfCompute
)
{
    NV_STATUS     status        = NV_OK;
    KernelFalcon *pKernelFalcon = staticCast(pKernelGsp, KernelFalcon);

    // Ensure SPDM Responder has booted before attempting to establish session.
    status = gpuTimeoutCondWait(pGpu, _kgspSpdmBootedOrFmcError, pKernelFalcon, NULL);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Timeout waiting for SPDM Responder to initialize!\n");
        goto exit;
    }

    //
    // Check if the terminating condition of the above wait was that SPDM has booted, or
    // if the FMC produced an error code in the mailbox.
    //
    NvU32 mailbox0 = kflcnRegRead_HAL(pGpu, pKernelFalcon, NV_PFALCON_FALCON_MAILBOX0);
    if (mailbox0 != NV_SPDM_PARTITION_BOOT_SUCCESS)
    {
        NV_PRINTF(LEVEL_ERROR, "GSP-FMC reported an error prior to SPDM boot: 0x%x\n",
                  mailbox0);
        status = NV_ERR_NOT_READY;
        goto exit;
    }

    status = confComputeEstablishSpdmSessionAndKeys_HAL(pGpu, pConfCompute);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "SPDM handshake with Responder failed.\n");
        goto exit;
    }

    //
    // Now we can send the async init messages that will be run before OBJGPU is created.
    // SPDM will not continue with boot until we send ack after sending these RPCs, else
    // we create a race condition where GSP-RM may skip these RPCs if not sent in time.
    //
    status = kgspQueueAsyncInitRpcs(pGpu, pKernelGsp);
    if (status != NV_OK)
    {
        goto exit;
    }

    // Tell SPDM that it can continue with boot
    kflcnRegWrite_HAL(pGpu, pKernelFalcon, NV_PFALCON_FALCON_MAILBOX0, NV_SPDM_REQUESTER_SECRETS_DERIVED);

    //
    // Wait for SPDM to restore the original mailbox value, which should have been 0 prior
    // to SPDM boot starting. This is needed because the subsequent lockdown release check
    // will also consider any non-zero value in mailbox0 to indicate an FMC error code.
    //
    status = gpuTimeoutCondWait(pGpu, _kgspFalconMailbox0Cleared, (void *)pKernelFalcon, NULL);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Timeout waiting for SPDM to proceed with boot!\n");
        goto exit;
    }

exit:
    if (status != NV_OK)
    {
        KernelFsp *pKernelFsp = GPU_GET_KERNEL_FSP(pGpu);

        NV_PRINTF(LEVEL_ERROR, "Failed to establish session with SPDM Responder!\n");
        if (pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_GSP_MODE_GSPRM))
        {
            kfspDumpDebugState_HAL(pGpu, pKernelFsp);
        }
        NV_PRINTF(LEVEL_ERROR, "NV_PGSP_FALCON_MAILBOX0 = 0x%x\n",
            kflcnRegRead_HAL(pGpu, pKernelFalcon, NV_PFALCON_FALCON_MAILBOX0));
        NV_PRINTF(LEVEL_ERROR, "NV_PGSP_FALCON_MAILBOX1 = 0x%x\n",
            kflcnRegRead_HAL(pGpu, pKernelFalcon, NV_PFALCON_FALCON_MAILBOX1));
    }

    return status;
}

/*!
 * Determine if GSP has performed CC partition cleanup.
 * Success or failure will be determined by the value in Mailbox1
 */
static NvBool
_kgspHasCcCleanupFinished
(
    OBJGPU  *pGpu,
    void    *pVoid
)
{
    NvU32         ccCleanupStatus = 0;
    KernelFalcon *pKernelFalcon   = (KernelFalcon *)pVoid;

    ccCleanupStatus = kflcnRegRead_HAL(pGpu, pKernelFalcon, NV_PFALCON_FALCON_MAILBOX1);

    //
    // To avoid a timing issue and issues with pre-existing values in Mailbox1
    // we check for explicit success or failure statuses.
    //
    return (ccCleanupStatus == NV_SPDM_SECRET_TEARDOWN_SUCCESS ||
            ccCleanupStatus == NV_SPDM_SECRET_TEARDOWN_FAILURE);
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

    // CC needs additional "regkeys" stuffed in a separate mailbox for the init partition
    ConfidentialCompute *pConfCompute =  GPU_GET_CONF_COMPUTE(pGpu);
    GPU_REG_WR32(pGpu, NV_PGSP_MAILBOX(0), pConfCompute->gspProxyRegkeys);

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
 * Prepare to boot GSP-RM
 *
 * This routine handles the prequesites to booting GSP-RM that requires the API LOCK:
 *   - Clear ECC errors
 *   - Prepare GSP-FMC arguments
 *   - Prepare FSP boot commands
 *
 * @param[in]   pGpu            GPU object pointer
 * @param[in]   pKernelGsp      GSP object pointer
 * @param[in]   pGspFw          GSP_FIRMWARE image pointer
 *
 * @return NV_OK if GSP-RM RISCV preparation to boot was successful.
 *         Appropriate NV_ERR_xxx value otherwise.
 */
NV_STATUS
kgspPrepareForBootstrap_GH100
(
    OBJGPU         *pGpu,
    KernelGsp      *pKernelGsp,
    GSP_FIRMWARE   *pGspFw
)
{
    KernelFsp *pKernelFsp = GPU_GET_KERNEL_FSP(pGpu);

    // Only for GSP client builds
    if (!IS_GSP_CLIENT(pGpu))
    {
        NV_PRINTF(LEVEL_ERROR, "IS_GSP_CLIENT is not set.\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    // Setup the descriptors that GSP-FMC needs to boot GSP-RM
    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kgspSetupGspFmcArgs_HAL(pGpu, pKernelGsp, pGspFw));

    if (pKernelFsp != NULL && !pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_DISABLE_GSPFMC))
    {
        pKernelFsp->setProperty(pKernelFsp, PDB_PROP_KFSP_GSP_MODE_GSPRM, NV_TRUE);
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR,
            kfspPrepareBootCommands_HAL(pGpu, pKernelFsp));
    }

    return NV_OK;
}

/*!
 * Boot GSP-RM.
 *
 * This routine handles the following:
 *   - sends FSP boot commands
 *   - waits for GSP-RM to complete initialization
 *
 * Note that this routine is based on flcnBootstrapRiscvOS_GA102().
 *
 * Note that this routine can be called without the API lock for
 * parllel initialization.
 *
 * @param[in]   pGpu            GPU object pointer
 * @param[in]   pKernelGsp      GSP object pointer
 * @param[in]   pGspFw          GSP_FIRMWARE image pointer
 *
 * @return NV_OK if GSP-RM RISCV boot was successful.
 *         Appropriate NV_ERR_xxx value otherwise.
 */
NV_STATUS
kgspBootstrap_GH100
(
    OBJGPU         *pGpu,
    KernelGsp      *pKernelGsp,
    GSP_FIRMWARE   *pGspFw
)
{
    KernelFalcon *pKernelFalcon = staticCast(pKernelGsp, KernelFalcon);
    KernelFsp    *pKernelFsp = GPU_GET_KERNEL_FSP(pGpu);
    NV_STATUS     status = NV_OK;
    NvU32         mailbox0;
    ConfidentialCompute *pCC = GPU_GET_CONF_COMPUTE(pGpu);

    if (pKernelFsp != NULL && !pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_DISABLE_GSPFMC))
    {
        NV_PRINTF(LEVEL_NOTICE, "Starting to boot GSP via FSP.\n");
        NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, kfspSendBootCommands_HAL(pGpu, pKernelFsp));
    }
    else
    {
        _kgspBootstrapGspFmc_GH100(pGpu, pKernelGsp);
    }

    // Wait for target mask to be released.
    if (pKernelFsp != NULL)
    {
        status = kfspWaitForGspTargetMaskReleased_HAL(pGpu, pKernelFsp);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Timeout waiting for GSP target mask release. "
                      "This error may be caused by several reasons: Bootrom may have failed, "
                      "GSP init code may have failed or ACR failed to release target mask. "
                      "RM does not have access to information on which of those conditions happened.\n");

            if (pKernelFsp->getProperty(pKernelFsp, PDB_PROP_KFSP_GSP_MODE_GSPRM))
            {
                kfspDumpDebugState_HAL(pGpu, pKernelFsp);
            }

            return status;
        }
    }

    //
    // In Confidential Compute, SPDM session will be established before GSP-RM boots.
    // Wait until after target mask is released by ACR to minimize busy wait time.
    // NOTE: This is incompatible with parallel initialization. See function
    // description for more details.
    //
    if (pCC != NULL && pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED))
    {
        NV_ASSERT_OR_RETURN(rmapiLockIsOwner(), NV_ERR_INVALID_LOCK_STATE);
        NV_ASSERT_OK_OR_RETURN(_kgspEstablishSpdmSession(pGpu, pKernelGsp, pCC));
    }

    // Wait for lockdown to be released or the FMC to report an error
    status = gpuTimeoutCondWait(pGpu, _kgspLockdownReleasedOrFmcError, pKernelGsp, NULL);
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
    else if ((mailbox0 = kflcnRegRead_HAL(pGpu, pKernelFalcon, NV_PFALCON_FALCON_MAILBOX0)) != 0)
    {
        NV_PRINTF(LEVEL_ERROR, "GSP-FMC reported an error while attempting to boot GSP: 0x%x\n",
                  mailbox0);
        return NV_ERR_NOT_READY;
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
        return NV_ERR_NOT_READY;
    }

    NV_PRINTF(LEVEL_INFO, "Waiting for GSP fw RM to be ready...\n");

    // Link the status queue.
    NV_ASSERT_OK_OR_RETURN(GspStatusQueueInit(pGpu, &pKernelGsp->pRpc->pMessageQueueInfo));

    NV_ASSERT_OK_OR_RETURN(kgspWaitForRmInitDone(pGpu, pKernelGsp));

    NV_PRINTF(LEVEL_INFO, "GSP FW RM ready.\n");

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
        ConfidentialCompute *pCC = GPU_GET_CONF_COMPUTE(pGpu);
        if (pCC != NULL && pCC->getProperty(pCC, PDB_PROP_CONFCOMPUTE_CC_FEATURE_ENABLED))
        {
            const BINDATA_ARCHIVE *pBinArchiveConcatenatedFMCDesc = kgspGetBinArchiveConcatenatedFMCDesc_HAL(pKernelGsp);
            const BINDATA_ARCHIVE *pBinArchiveConcatenatedFMC     = kgspGetBinArchiveConcatenatedFMC_HAL(pKernelGsp);

            if (kgspIsDebugModeEnabled(pGpu, pKernelGsp))
            {
                *ppBinStorageImage = (BINDATA_STORAGE *)bindataArchiveGetStorage(pBinArchiveConcatenatedFMC, "ucode_image_dbg");
                *ppBinStorageDesc  = (BINDATA_STORAGE *)bindataArchiveGetStorage(pBinArchiveConcatenatedFMCDesc, "ucode_desc_dbg");
            }
            else
            {
                *ppBinStorageImage = (BINDATA_STORAGE *)bindataArchiveGetStorage(pBinArchiveConcatenatedFMC, "ucode_image_prod");
                *ppBinStorageDesc  = (BINDATA_STORAGE *)bindataArchiveGetStorage(pBinArchiveConcatenatedFMCDesc, "ucode_desc_prod");
            }

            return;
        }

    kgspGetGspRmBootUcodeStorage_GA102(pGpu, pKernelGsp, ppBinStorageImage, ppBinStorageDesc);
}

NV_STATUS
kgspIssueNotifyOp_GH100
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    NvU32 opCode,
    NvU32 *pArgs,
    NvU32 argc
)
{
    NV_STATUS status = NV_OK;
    NotifyOpSharedSurface *pNotifyOpSharedSurface;
    volatile NvU32 *pInUse;
    volatile NvU32 *pSeqAddr;
    volatile NvU32 *pStatusAddr;
    NvU32 *pOpCodeAddr;
    NvU32 *pArgsAddr;
    NvU32 *pArgcAddr;

    NvU32 seqValue;
    NvU32 i;
    RMTIMEOUT timeout;

    // 1. Validate the arguments.
    NV_CHECK_OR_RETURN(LEVEL_ERROR, opCode < GSP_NOTIFY_OP_OPCODE_MAX, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, argc <= GSP_NOTIFY_OP_MAX_ARGUMENT_COUNT, NV_ERR_INVALID_ARGUMENT);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, (argc == 0 || pArgs != NULL), NV_ERR_INVALID_ARGUMENT);

    // 2. Set up ptrs to the shared memory.
    NV_ASSERT_OR_RETURN(pKernelGsp->pNotifyOpSurf != NULL, NV_ERR_INVALID_STATE);

    pNotifyOpSharedSurface = pKernelGsp->pNotifyOpSurf;

    pInUse      = (NvU32*) &(pNotifyOpSharedSurface->inUse);
    pSeqAddr    = (NvU32*) &(pNotifyOpSharedSurface->seqNum);
    pOpCodeAddr = (NvU32*) &(pNotifyOpSharedSurface->opCode);
    pStatusAddr = (NvU32*) &(pNotifyOpSharedSurface->status);
    pArgcAddr   = (NvU32*) &(pNotifyOpSharedSurface->argc);
    pArgsAddr   = (NvU32*) pNotifyOpSharedSurface->args;

    while (!portAtomicCompareAndSwapU32(pInUse, 1, 0))
    {
        // We're not going to sleep, but safe to sleep also means safe to spin..
        NV_ASSERT_OR_RETURN(portSyncExSafeToSleep(), NV_ERR_INVALID_STATE);
        osSpinLoop();
    }

    // 3. Read current sequence counter value.
    seqValue = *pSeqAddr;

    // 4. Populate the opcode and arguments.
    *pOpCodeAddr = opCode;
    *pArgcAddr = argc;
    for (i = 0; i < argc; i++)
    {
        pArgsAddr[i] = pArgs[i];
    }

    //
    // Issue a store fence to ensure that the parameters have made it to memory
    // before the interrupt is triggered.
    //
    portAtomicMemoryFenceStore();

    // 5. Trigger notification interrupt to GSP.
    gpuSetTimeout(pGpu, GPU_TIMEOUT_DEFAULT, &timeout, 0);
    GPU_VREG_WR32(pGpu, NV_VIRTUAL_FUNCTION_PRIV_DOORBELL, NV_DOORBELL_NOTIFY_LEAF_SERVICE_LOCKLESS_OP_HANDLE);

    // 6. Poll on the sequence number to ensure the op completed.
    while (seqValue + 1 != *pSeqAddr)
    {
        status = gpuCheckTimeout(pGpu, &timeout);

        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "gpuCheckTimeout failed, status = 0x%x\n", status);
            goto error_ret;
        }
        osSpinLoop();
    }

    status = *pStatusAddr;

error_ret:
    portAtomicSetU32(pInUse, 0);

    NV_CHECK_OK_OR_RETURN(LEVEL_ERROR, status);
    return status;
}

NV_STATUS
kgspCheckGspRmCcCleanup_GH100
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    KernelFalcon *pKernelFalcon   = staticCast(pKernelGsp, KernelFalcon);
    NvU32         ccCleanupStatus = 0;
    NV_STATUS     status          = NV_OK;

    // Wait for GSP to explicitly state we have completed CC cleanup.
    status = gpuTimeoutCondWait(pGpu, _kgspHasCcCleanupFinished, (void *)pKernelFalcon, NULL);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "CC secret cleanup failed due to timeout!\n");
        goto exit;
    }

    // Now, check whether or not CC cleanup was successful.
    ccCleanupStatus = kflcnRegRead_HAL(pGpu, pKernelFalcon, NV_PFALCON_FALCON_MAILBOX1);
    if (ccCleanupStatus == NV_SPDM_SECRET_TEARDOWN_SUCCESS)
    {
        // FIPS: Make status clear to user.
        NV_PRINTF(LEVEL_ERROR, "CC secret cleanup successful!\n");
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR, "CC secret cleanup failed! Status 0x%x\n",
                  ccCleanupStatus);
    }

exit:

    // Regardless of success or failure, write ack to GSP.
    kflcnRegWrite_HAL(pGpu, pKernelFalcon, NV_PFALCON_FALCON_MAILBOX1, NV_SPDM_SECRET_TEARDOWN_ACK);

    return status;
}
