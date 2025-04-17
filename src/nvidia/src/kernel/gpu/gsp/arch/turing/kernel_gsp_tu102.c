/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * Provides TU102+ specific KernelGsp HAL implementations.
 */

#include "gpu/gsp/kernel_gsp.h"

#include "gpu/disp/kern_disp.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "vgpu/rpc.h"
#include "rmgspseq.h"
#include "core/thread_state.h"
#include "os/os.h"
#include "nverror.h"
#include "nvrm_registry.h"
#include "crashcat/crashcat_report.h"

#include "published/turing/tu102/dev_gsp.h"
#include "published/turing/tu102/dev_gsp_addendum.h"
#include "published/turing/tu102/dev_riscv_pri.h"
#include "published/turing/tu102/dev_fbif_v4.h"
#include "published/turing/tu102/dev_falcon_v4.h"
#include "published/turing/tu102/dev_fb.h"  // for NV_PFB_PRI_MMU_WPR2_ADDR_HI
#include "published/turing/tu102/dev_fuse.h"
#include "published/turing/tu102/dev_ram.h"
#include "published/turing/tu102/dev_gc6_island.h"
#include "published/turing/tu102/dev_gc6_island_addendum.h"

#include "gpu/sec2/kernel_sec2.h"

#include "g_all_dcl_pb.h"
#include "lib/protobuf/prb.h"

void
kgspConfigureFalcon_TU102
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
    falconConfig.bBootFromHs        = NV_FALSE;
    falconConfig.pmcEnableMask      = 0;
    falconConfig.bIsPmcDeviceEngine = NV_FALSE;
    falconConfig.physEngDesc        = ENG_GSP;

    // Enable CrashCat monitoring
    falconConfig.crashcatEngConfig.bEnable = NV_TRUE;
    falconConfig.crashcatEngConfig.pName = MAKE_NV_PRINTF_STR("GSP");
    falconConfig.crashcatEngConfig.errorId = GSP_ERROR;

    kflcnConfigureEngine(pGpu, staticCast(pKernelGsp, KernelFalcon), &falconConfig);
}

/*!
 * Check if the GSP is in debug mode
 *
 * @return whether the GSP is in debug mode or not
 */
NvBool
kgspIsDebugModeEnabled_TU102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    NvU32 data;

    data = GPU_REG_RD32(pGpu, NV_FUSE_OPT_SECURE_GSP_DEBUG_DIS);

    return FLD_TEST_DRF(_FUSE, _OPT_SECURE_GSP_DEBUG_DIS, _DATA, _NO, data);
}

NV_STATUS
kgspAllocBootArgs_TU102
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

    // Allocate WPR meta data
    NV_ASSERT_OK_OR_GOTO(nvStatus,
                         memdescCreate(&pKernelGsp->pWprMetaDescriptor,
                                       pGpu, 0x1000, 0x1000,
                                       NV_TRUE, ADDR_SYSMEM, NV_MEMORY_CACHED,
                                       flags),
                        _kgspAllocBootArgs_exit_cleanup);

    memdescTagAlloc(nvStatus, NV_FB_ALLOC_RM_INTERNAL_OWNER_WPR_METADATA,
                    pKernelGsp->pWprMetaDescriptor);
    NV_ASSERT_OK_OR_GOTO(nvStatus, nvStatus,
                         _kgspAllocBootArgs_exit_cleanup);

    NV_ASSERT_OK_OR_GOTO(nvStatus,
                         memdescMap(pKernelGsp->pWprMetaDescriptor, 0,
                                    memdescGetSize(pKernelGsp->pWprMetaDescriptor),
                                    NV_TRUE, NV_PROTECT_READ_WRITE,
                                    &pVa, &pPriv),
                         _kgspAllocBootArgs_exit_cleanup);

    pKernelGsp->pWprMeta = (GspFwWprMeta *)NvP64_VALUE(pVa);
    pKernelGsp->pWprMetaMappingPriv = pPriv;

    portMemSet(pKernelGsp->pWprMeta, 0, sizeof(*pKernelGsp->pWprMeta));

    //
    // Setup libos arguments memory
    //
    NV_ASSERT_OK_OR_GOTO(nvStatus,
                         memdescCreate(&pKernelGsp->pLibosInitArgumentsDescriptor,
                                       pGpu,
                                       LIBOS_INIT_ARGUMENTS_SIZE,
                                       LIBOS_INIT_ARGUMENTS_SIZE,
                                       NV_TRUE, ADDR_SYSMEM, NV_MEMORY_UNCACHED,
                                       flags),
                         _kgspAllocBootArgs_exit_cleanup);

    memdescTagAlloc(nvStatus, NV_FB_ALLOC_RM_INTERNAL_OWNER_LIBOS_ARGS,
                    pKernelGsp->pLibosInitArgumentsDescriptor);
    NV_ASSERT_OK_OR_GOTO(nvStatus, nvStatus,
                         _kgspAllocBootArgs_exit_cleanup);

    NV_ASSERT_OK_OR_GOTO(nvStatus,
                         memdescMap(pKernelGsp->pLibosInitArgumentsDescriptor, 0,
                                    memdescGetSize(pKernelGsp->pLibosInitArgumentsDescriptor),
                                    NV_TRUE, NV_PROTECT_READ_WRITE,
                                     &pVa, &pPriv),
                         _kgspAllocBootArgs_exit_cleanup);

    pKernelGsp->pLibosInitArgumentsCached = (LibosMemoryRegionInitArgument *)NvP64_VALUE(pVa);
    pKernelGsp->pLibosInitArgumentsMappingPriv = pPriv;

    portMemSet(pKernelGsp->pLibosInitArgumentsCached, 0, LIBOS_INIT_ARGUMENTS_SIZE);

    // Setup bootloader arguments memory.
    NV_ASSERT(sizeof(GSP_ARGUMENTS_CACHED) <= 0x1000);

    NV_ASSERT_OK_OR_GOTO(nvStatus,
                         memdescCreate(&pKernelGsp->pGspArgumentsDescriptor,
                                       pGpu, 0x1000, 0x1000,
                                       NV_TRUE, ADDR_SYSMEM, NV_MEMORY_CACHED,
                                       flags),
                         _kgspAllocBootArgs_exit_cleanup);

    memdescTagAlloc(nvStatus, NV_FB_ALLOC_RM_INTERNAL_OWNER_BOOTLOADER_ARGS,
                    pKernelGsp->pGspArgumentsDescriptor);
    NV_ASSERT_OK_OR_GOTO(nvStatus, nvStatus,
                         _kgspAllocBootArgs_exit_cleanup);

    NV_ASSERT_OK_OR_GOTO(nvStatus,
                         memdescMap(pKernelGsp->pGspArgumentsDescriptor, 0,
                                    memdescGetSize(pKernelGsp->pGspArgumentsDescriptor),
                                    NV_TRUE, NV_PROTECT_READ_WRITE,
                                    &pVa, &pPriv),
                         _kgspAllocBootArgs_exit_cleanup);

    pKernelGsp->pGspArgumentsCached = (GSP_ARGUMENTS_CACHED *)NvP64_VALUE(pVa);
    pKernelGsp->pGspArgumentsMappingPriv = pPriv;

    portMemSet(pKernelGsp->pGspArgumentsCached, 0, sizeof(*pKernelGsp->pGspArgumentsCached));

    if (pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB))
    {
        NvU32 heapSizeMB = 0;
        // Get the sysmem heap size override from the registry, or use default
        if (osReadRegistryDword(pGpu, NV_REG_STR_GSP_SYSMEM_HEAP_SIZE_MB, &heapSizeMB) != NV_OK)
        {
            heapSizeMB = NV_REG_STR_GSP_SYSMEM_HEAP_SIZE_MB_DEFAULT;
        }

        NV_ASSERT_OK_OR_GOTO(nvStatus,
                             memdescCreate(&pKernelGsp->pSysmemHeapDescriptor,
                                            pGpu, (NvU64)heapSizeMB << 20, 0,
                                            NV_FALSE, ADDR_SYSMEM, NV_MEMORY_UNCACHED,
                                            flags),
                             _kgspAllocBootArgs_exit_cleanup);

        memdescTagAlloc(nvStatus, NV_FB_ALLOC_RM_INTERNAL_OWNER_BOOTLOADER_ARGS,
                        pKernelGsp->pSysmemHeapDescriptor);
        NV_ASSERT_OK_OR_GOTO(nvStatus, nvStatus,
                            _kgspAllocBootArgs_exit_cleanup);

        NV_ASSERT_OR_GOTO(memdescCheckContiguity(pKernelGsp->pSysmemHeapDescriptor, AT_GPU),
                          _kgspAllocBootArgs_exit_cleanup);
    }

    return nvStatus;

_kgspAllocBootArgs_exit_cleanup:
    kgspFreeBootArgs_HAL(pGpu, pKernelGsp);
    return nvStatus;
}

void
kgspFreeBootArgs_TU102
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    // release wpr meta data resources
    if (pKernelGsp->pWprMeta != NULL)
    {
        memdescUnmap(pKernelGsp->pWprMetaDescriptor,
                     NV_TRUE, osGetCurrentProcess(),
                     (void *)pKernelGsp->pWprMeta,
                     pKernelGsp->pWprMetaMappingPriv);
        pKernelGsp->pWprMeta = NULL;
        pKernelGsp->pWprMetaMappingPriv = NULL;
    }
    if (pKernelGsp->pWprMetaDescriptor != NULL)
    {
        memdescFree(pKernelGsp->pWprMetaDescriptor);
        memdescDestroy(pKernelGsp->pWprMetaDescriptor);
        pKernelGsp->pWprMetaDescriptor = NULL;
    }

    // release libos init argument resources
    if (pKernelGsp->pLibosInitArgumentsCached != NULL)
    {
        memdescUnmap(pKernelGsp->pLibosInitArgumentsDescriptor,
                     NV_TRUE, osGetCurrentProcess(),
                     (void *)pKernelGsp->pLibosInitArgumentsCached,
                     pKernelGsp->pLibosInitArgumentsMappingPriv);
        pKernelGsp->pLibosInitArgumentsCached = NULL;
        pKernelGsp->pLibosInitArgumentsMappingPriv = NULL;
    }
    if (pKernelGsp->pLibosInitArgumentsDescriptor != NULL)
    {
        memdescFree(pKernelGsp->pLibosInitArgumentsDescriptor);
        memdescDestroy(pKernelGsp->pLibosInitArgumentsDescriptor);
        pKernelGsp->pLibosInitArgumentsDescriptor = NULL;
    }

    // release init argument page resources
    if (pKernelGsp->pGspArgumentsCached != NULL)
    {
        memdescUnmap(pKernelGsp->pGspArgumentsDescriptor,
                     NV_TRUE, osGetCurrentProcess(),
                     (void *)pKernelGsp->pGspArgumentsCached,
                     pKernelGsp->pGspArgumentsMappingPriv);
        pKernelGsp->pGspArgumentsCached = NULL;
        pKernelGsp->pGspArgumentsMappingPriv = NULL;
    }
    if (pKernelGsp->pGspArgumentsDescriptor != NULL)
    {
        memdescFree(pKernelGsp->pGspArgumentsDescriptor);
        memdescDestroy(pKernelGsp->pGspArgumentsDescriptor);
        pKernelGsp->pGspArgumentsDescriptor = NULL;
    }

    // Release radix3 version of GSP-RM ucode
    if (pKernelGsp->pGspUCodeRadix3Descriptor != NULL)
    {
        memdescFree(pKernelGsp->pGspUCodeRadix3Descriptor);
        memdescDestroy(pKernelGsp->pGspUCodeRadix3Descriptor);
        pKernelGsp->pGspUCodeRadix3Descriptor = NULL;
    }

    // Release signature memory
    if (pKernelGsp->pSignatureMemdesc != NULL)
    {
        memdescFree(pKernelGsp->pSignatureMemdesc);
        memdescDestroy(pKernelGsp->pSignatureMemdesc);
        pKernelGsp->pSignatureMemdesc = NULL;
    }

    // Release sysmem heap memory
    if (pKernelGsp->pSysmemHeapDescriptor != NULL)
    {
        memdescFree(pKernelGsp->pSysmemHeapDescriptor);
        memdescDestroy(pKernelGsp->pSysmemHeapDescriptor);
        pKernelGsp->pSysmemHeapDescriptor = NULL;
    }
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
 * Set command queue head for CPU to GSP message queue
 *
 * @param[in]   pGpu            GPU object pointer
 * @param[in]   pKernelGsp      KernelGsp object pointer (not used)
 * @param[in]   queueIdx        index
 * @param[in]   value           value to set command queue head to.
 *
 * @return NV_OK if the operation was successful.
 */
NV_STATUS
kgspSetCmdQueueHead_TU102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    NvU32   queueIdx,
    NvU32   value
)
{
    NV_ASSERT_OR_RETURN(queueIdx < NV_PGSP_QUEUE_HEAD__SIZE_1, NV_ERR_INVALID_ARGUMENT);

    // Write the value to the correct queue head.
    GPU_REG_WR32(pGpu, NV_PGSP_QUEUE_HEAD(queueIdx), value);

    return NV_OK;
}

/*!
 * Load entrypoint address of boot binary into mailbox regs.
 */
void
kgspProgramLibosBootArgsAddr_TU102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    NvU64 addr =
        memdescGetPhysAddr(pKernelGsp->pLibosInitArgumentsDescriptor, AT_GPU, 0);

    GPU_REG_WR32(pGpu, NV_PGSP_FALCON_MAILBOX0, NvU64_LO32(addr));
    GPU_REG_WR32(pGpu, NV_PGSP_FALCON_MAILBOX1, NvU64_HI32(addr));
}

/*!
 * Prepare to boot GSP-RM
 *
 * This routine handles the prerequisites to booting GSP-RM that requires the API LOCK:
 *   - prepares boot binary image
 *   - prepares RISCV core to run GSP-RM
 *
 * Note that boot binary and GSP-RM images have already been placed
 * in the appropriate places by kgspPopulateWprMeta_HAL().
 *
 * Note that this routine is based on flcnBootstrapRiscvOS_GA102().
 *
 * @param[in]   pGpu            GPU object pointer
 * @param[in]   pKernelGsp      GSP object pointer
 * @param[in]   bootMode        GSP boot mode
 *
 * @return NV_OK if GSP-RM RISCV boot was successful.
 *         Appropriate NV_ERR_xxx value otherwise.
 */
NV_STATUS
kgspPrepareForBootstrap_TU102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    KernelGspBootMode bootMode
)
{
    NV_STATUS     status;
    KernelFalcon *pKernelFalcon = staticCast(pKernelGsp, KernelFalcon);

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

    //
    // Prepare to execute FWSEC to setup FRTS if we have a FRTS region
    // Note: for resume and GC6 exit, FRTS is restored by Booter not FWSEC
    //
    if ((bootMode == KGSP_BOOT_MODE_NORMAL) &&
        (kgspGetFrtsSize_HAL(pGpu, pKernelGsp) > 0))
    {
        pKernelGsp->pPreparedFwsecCmd = portMemAllocNonPaged(sizeof(KernelGspPreparedFwsecCmd));
        status = kgspPrepareForFwsecFrts_HAL(pGpu, pKernelGsp,
                                             pKernelGsp->pFwsecUcode,
                                             pKernelGsp->pWprMeta->frtsOffset,
                                             pKernelGsp->pPreparedFwsecCmd);
        if (status != NV_OK)
        {
            portMemFree(pKernelGsp->pPreparedFwsecCmd);
            pKernelGsp->pPreparedFwsecCmd = NULL;
            return status;
        }
    }

    return NV_OK;
}

/*!
 * Obtain sysmem addr or arguments to be consumed by Booter Load.
 * Booter expects different arguments for normal boot, resume, and GC6 exit.
 *
 * @param[in]  bootMode  GSP boot mode
 */
static inline NvU64
_kgspGetBooterLoadArgs
(
    KernelGsp *pKernelGsp,
    KernelGspBootMode bootMode
)
{
    switch (bootMode)
    {
        case KGSP_BOOT_MODE_NORMAL:
            return memdescGetPhysAddr(pKernelGsp->pWprMetaDescriptor, AT_GPU, 0);
        case KGSP_BOOT_MODE_SR_RESUME:
            return memdescGetPhysAddr(pKernelGsp->pSRMetaDescriptor, AT_GPU, 0);
        case KGSP_BOOT_MODE_GC6_EXIT:
            return 0;
    }

    // unreachable
    NV_ASSERT_FAILED("unexpected GSP boot mode");
    return 0;
}

/*!
 * Boot GSP-RM.
 *
 * This routine handles the following:
 *   - starts the RISCV core and passes control to boot binary image
 *   - waits for GSP-RM to complete initialization
 *
 * Note that boot binary and GSP-RM images have already been placed
 * in the appropriate places by kgspPopulateWprMeta_HAL().
 *
 * Note that this routine is based on flcnBootstrapRiscvOS_GA102().
 *
 * Note that this routine can be called without the API lock for
 * parllel initialization.
 *
 * @param[in]   pGpu            GPU object pointer
 * @param[in]   pKernelGsp      GSP object pointer
 * @param[in]   bootMode        GSP boot mode
 *
 * @return NV_OK if GSP-RM RISCV boot was successful.
 *         Appropriate NV_ERR_xxx value otherwise.
 */
NV_STATUS
kgspBootstrap_TU102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    KernelGspBootMode bootMode
)
{
    NV_STATUS status;
    KernelFalcon *pKernelFalcon = staticCast(pKernelGsp, KernelFalcon);

    //
    // For normal boot, additional setup is necessary.
    // Note: for resume or GC6 exit, Booter and/or GSP-RM will restore these.
    //
    if (bootMode == KGSP_BOOT_MODE_NORMAL)
    {
        // Execute Scrubber if needed
        if (pKernelGsp->pScrubberUcode != NULL)
        {
            NV_ASSERT_OK_OR_RETURN(kgspExecuteScrubberIfNeeded_HAL(pGpu, pKernelGsp));
        }

        // Execute FWSEC to setup FRTS if we have a FRTS region.
        if (kgspGetFrtsSize_HAL(pGpu, pKernelGsp) > 0)
        {
            NV_ASSERT_OR_RETURN(pKernelGsp->pPreparedFwsecCmd != NULL, NV_ERR_INVALID_STATE);

            NV_ASSERT_OK_OR_RETURN(kflcnReset_HAL(pGpu, pKernelFalcon));

            status = kgspExecuteFwsec_HAL(pGpu, pKernelGsp, pKernelGsp->pPreparedFwsecCmd);
            portMemFree(pKernelGsp->pPreparedFwsecCmd);
            pKernelGsp->pPreparedFwsecCmd = NULL;

            NV_ASSERT_OK_OR_RETURN(status);
        }

        NV_ASSERT_OK_OR_RETURN(kflcnResetIntoRiscv_HAL(pGpu, pKernelFalcon));

        // Load init args into mailbox regs
        kgspProgramLibosBootArgsAddr_HAL(pGpu, pKernelGsp);
    }

    // Execute Booter Load
    status = kgspExecuteBooterLoad_HAL(pGpu, pKernelGsp,
                                       _kgspGetBooterLoadArgs(pKernelGsp, bootMode));
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to execute Booter Load (ucode for initial boot): 0x%x\n", status);
        return status;
    }

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
        NV_PRINTF(LEVEL_ERROR, "Failed to boot GSP.\n");

        return NV_ERR_NOT_READY;
    }

    NV_PRINTF(LEVEL_INFO, "Waiting for GSP fw RM to be ready...\n");

    //
    // For normal boot, link the status queue.
    // Note: for resume or GC6 exit, GSP-RM will restore queue state.
    //
    if (bootMode == KGSP_BOOT_MODE_NORMAL)
    {
        NV_ASSERT_OK_OR_RETURN(GspStatusQueueInit(pGpu, &pKernelGsp->pRpc->pMessageQueueInfo));
    }

    NV_ASSERT_OK_OR_RETURN(kgspWaitForRmInitDone(pGpu, pKernelGsp));

    NV_PRINTF(LEVEL_INFO, "GSP FW RM ready.\n");

    return NV_OK;
}

/*!
 * Obtain sysmem addr or arguments to be consumed by Booter Unload.
 * Booter expects different arguments for normal unload, suspend, and GC6 enter.
 *
 * @param[in]  unloadMode  GSP unload mode
 */
static inline NvU64
_kgspGetBooterUnloadArgs
(
    KernelGsp *pKernelGsp,
    KernelGspUnloadMode unloadMode
)
{
    switch (unloadMode)
    {
        case KGSP_UNLOAD_MODE_NORMAL:
            return 0;
        case KGSP_UNLOAD_MODE_SR_SUSPEND:
            return memdescGetPhysAddr(pKernelGsp->pSRMetaDescriptor, AT_GPU, 0);
        case KGSP_UNLOAD_MODE_GC6_ENTER:
            return 0;
    }

    // unreachable
    NV_ASSERT_FAILED("unexpected GSP unload mode");
    return 0;
}

/*!
 * Teardown remaining GSP state after GSP-RM unloads.
 *
 * For pre-Hopper, this involves running FWSEC-SB to put back pre-OS apps and
 * Booter Unload to teardown WPR2.
 *
 * @param[in]   pGpu            GPU object pointer
 * @param[in]   pKernelGsp      GSP object pointer
 * @param[in]   unloadMode      GSP unload mode
 */
NV_STATUS
kgspTeardown_TU102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    KernelGspUnloadMode unloadMode
)
{
    NV_STATUS status;

    //
    // Avoid cascading timeouts when attempting to invoke the below ucodes if
    // we are unloading due to a GSP-RM timeout.
    //
    threadStateResetTimeout(pGpu);

    if (unloadMode != KGSP_UNLOAD_MODE_GC6_ENTER)
    {
        KernelGspPreparedFwsecCmd preparedCmd;

        // Reset GSP so we can load FWSEC-SB
        status = kflcnReset_HAL(pGpu, staticCast(pKernelGsp, KernelFalcon));
        NV_ASSERT((status == NV_OK) || (status == NV_ERR_GPU_IN_FULLCHIP_RESET));

        // Invoke FWSEC-SB to put back PreOsApps during driver unload
        status = kgspPrepareForFwsecSb_HAL(pGpu, pKernelGsp, pKernelGsp->pFwsecUcode, &preparedCmd);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "failed to prepare for FWSEC-SB for PreOsApps during driver unload: 0x%x\n", status);
            NV_ASSERT_FAILED("FWSEC-SB prep failed");
        }
        else
        {
            status = kgspExecuteFwsec_HAL(pGpu, pKernelGsp, &preparedCmd);
            if ((status != NV_OK) && (status != NV_ERR_GPU_IN_FULLCHIP_RESET))
            {
                NV_PRINTF(LEVEL_ERROR, "failed to execute FWSEC-SB for PreOsApps during driver unload: 0x%x\n", status);
                NV_ASSERT_FAILED("FWSEC-SB failed");
            }
        }
    }

    // Execute Booter Unload
    status = kgspExecuteBooterUnloadIfNeeded_HAL(pGpu, pKernelGsp,
                                                 _kgspGetBooterUnloadArgs(pKernelGsp, unloadMode));
    return status;
}

void
kgspGetGspRmBootUcodeStorage_TU102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    BINDATA_STORAGE **ppBinStorageImage,
    BINDATA_STORAGE **ppBinStorageDesc
)
{
    const BINDATA_ARCHIVE *pBinArchive = kgspGetBinArchiveGspRmBoot_HAL(pKernelGsp);

    *ppBinStorageImage = (BINDATA_STORAGE *) bindataArchiveGetStorage(pBinArchive, BINDATA_LABEL_UCODE_IMAGE);
    *ppBinStorageDesc  = (BINDATA_STORAGE *) bindataArchiveGetStorage(pBinArchive, BINDATA_LABEL_UCODE_DESC);
}

/*!
 * Populate WPR meta structure.
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
 *   | BOOT BIN (e.g. SK + BL)  |
 *   ---------------------------- <- bootBinOffset
 *   | GSP FW ELF               |
 *   ---------------------------- <- gspFwOffset
 *   | GSP FW (WPR) HEAP        |
 *   ---------------------------- <- gspFwHeapOffset**
 *   | Booter-placed metadata   |
 *   | (struct GspFwWprMeta)    |
 *   ---------------------------- <- gspFwWprStart (128K aligned)
 *   | GSP FW (non-WPR) HEAP    |
 *   ---------------------------- <- nonWprHeapOffset, gspFwRsvdStart
 *
 *  gspFwHeapOffset** contains the entire WPR heap region, which can be subdivided
 *  for various GSP FW components.
 *
 * @param       pGpu          GPU object pointer
 * @param       pKernelGsp    KernelGsp object pointer
 * @param       pGspFw        Pointer to GSP-RM fw image.
 */
NV_STATUS
kgspPopulateWprMeta_TU102
(
    OBJGPU         *pGpu,
    KernelGsp      *pKernelGsp,
    GSP_FIRMWARE   *pGspFw
)
{
    KernelMemorySystem  *pKernelMemorySystem  = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    KernelDisplay       *pKernelDisplay = GPU_GET_KERNEL_DISPLAY(pGpu);
    MemoryManager       *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    GspFwWprMeta        *pWprMeta = pKernelGsp->pWprMeta;
    RM_RISCV_UCODE_DESC *pRiscvDesc = pKernelGsp->pGspRmBootUcodeDesc;
    NvU64                vbiosReservedOffset;
    NvU64                mmuLockLo, mmuLockHi;
    NvBool               bIsMmuLockValid;
    NvU32                data;

    ct_assert(sizeof(*pWprMeta) == 256);

    NV_ASSERT_OR_RETURN(IS_GSP_CLIENT(pGpu), NV_ERR_NOT_SUPPORTED);

    NV_ASSERT_OR_RETURN(pKernelGsp->pGspRmBootUcodeImage != NULL, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pKernelGsp->gspRmBootUcodeSize != 0, NV_ERR_INVALID_STATE);
    NV_ASSERT_OR_RETURN(pRiscvDesc != NULL, NV_ERR_INVALID_STATE);

    NV_ASSERT_OK_OR_RETURN(kmemsysGetUsableFbSize_HAL(pGpu, pKernelMemorySystem, &pWprMeta->fbSize));

    //
    // Start layout calculations at the top and work down.
    // Figure out where VGA workspace is located.  We do not have to adjust
    // it ourselves (see vgaRelocateWorkspaceBase_HAL()).
    //
    if (gpuFuseSupportsDisplay_HAL(pGpu) &&
        kdispGetVgaWorkspaceBase(pGpu, pKernelDisplay, &pWprMeta->vgaWorkspaceOffset))
    {
        if (pWprMeta->vgaWorkspaceOffset < (pWprMeta->fbSize - DRF_SIZE(NV_PRAMIN)))
        {
            const NvU32 VBIOS_WORKSPACE_SIZE = 0x20000;

            // Point NV_PDISP_VGA_WORKSPACE_BASE to end-of-FB
            pWprMeta->vgaWorkspaceOffset = (pWprMeta->fbSize - VBIOS_WORKSPACE_SIZE);
        }
    }
    else
    {
        pWprMeta->vgaWorkspaceOffset = (pWprMeta->fbSize - DRF_SIZE(NV_PRAMIN));
    }
    pWprMeta->vgaWorkspaceSize = pWprMeta->fbSize - pWprMeta->vgaWorkspaceOffset;

    // Check for MMU locked region (locked by VBIOS)
    NV_ASSERT_OK_OR_RETURN(
        memmgrReadMmuLock_HAL(pGpu, pMemoryManager, &bIsMmuLockValid, &mmuLockLo, &mmuLockHi));

    if (bIsMmuLockValid)
        vbiosReservedOffset = NV_MIN(mmuLockLo, pWprMeta->vgaWorkspaceOffset);
    else
        vbiosReservedOffset = pWprMeta->vgaWorkspaceOffset;

    // Set the size of the GSP FW ahead of kgspGetWprEndMargin()
    pWprMeta->sizeOfRadix3Elf = pGspFw->imageSize;

    // End of WPR region (128KB aligned), shifted for any WPR end margin
    pWprMeta->gspFwWprEnd = NV_ALIGN_DOWN64(vbiosReservedOffset - kgspGetWprEndMargin(pGpu, pKernelGsp), 0x20000);

    pWprMeta->frtsSize = kgspGetFrtsSize(pGpu, pKernelGsp);
    pWprMeta->frtsOffset = pWprMeta->gspFwWprEnd - pWprMeta->frtsSize;

    // Offset of boot binary image (4K aligned)
    pWprMeta->sizeOfBootloader = pKernelGsp->gspRmBootUcodeSize;
    pWprMeta->bootBinOffset = NV_ALIGN_DOWN64(pWprMeta->frtsOffset - pWprMeta->sizeOfBootloader, 0x1000);

    //
    // Compute the start of the ELF.  Align to 64K to avoid issues with
    // inherent alignment constraints.
    //
    pWprMeta->gspFwOffset = NV_ALIGN_DOWN64(pWprMeta->bootBinOffset - pWprMeta->sizeOfRadix3Elf, 0x10000);

    //
    // The maximum size of the GSP-FW heap depends on the statically-sized regions before and after
    // it in the pre-scrubbed region of FB.
    //
    const NvU64 MB = (1ULL << 20);
    const NvU64 nonWprHeapSize = NV_ALIGN_UP64(kgspGetNonWprHeapSize(pGpu, pKernelGsp), MB);
    const NvU64 wprMetaSize = NV_ALIGN_UP64(sizeof(*pWprMeta), MB);
    const NvU64 preWprHeapSize = wprMetaSize + nonWprHeapSize;
    const NvU64 postWprHeapSize = NV_ALIGN_UP64(pWprMeta->fbSize - pWprMeta->gspFwOffset, MB);
    const NvU64 wprHeapSize = kgspGetFwHeapSize(pGpu, pKernelGsp, preWprHeapSize, postWprHeapSize);

    // GSP-RM heap in WPR, align to 1MB
    pWprMeta->gspFwHeapOffset = NV_ALIGN_DOWN64(pWprMeta->gspFwOffset - wprHeapSize, MB);
    pWprMeta->gspFwHeapSize = NV_ALIGN_DOWN64(pWprMeta->gspFwOffset - pWprMeta->gspFwHeapOffset, MB);

    // Number of VF partitions allocating sub-heaps from the WPR heap
    pWprMeta->gspFwHeapVfPartitionCount = pGpu->bVgpuGspPluginOffloadEnabled ? MAX_PARTITIONS_WITH_GFID : 0;

    //
    // Start of WPR region (128K alignment requirement, but 1MB aligned so that
    // the extra padding sits in WPR instead of in between the end of the
    // non-WPR heap and the start of WPR).
    //
    pWprMeta->gspFwWprStart = pWprMeta->gspFwHeapOffset - wprMetaSize;

    // Non WPR heap (1MB aligned)
    pWprMeta->nonWprHeapSize = nonWprHeapSize;
    pWprMeta->nonWprHeapOffset = pWprMeta->gspFwWprStart - pWprMeta->nonWprHeapSize;

    pWprMeta->gspFwRsvdStart = pWprMeta->nonWprHeapOffset;

    // Physical address of GSP-RM firmware in system memory.
    pWprMeta->sysmemAddrOfRadix3Elf =
        memdescGetPhysAddr(pKernelGsp->pGspUCodeRadix3Descriptor, AT_GPU, 0);

    // Physical address of boot loader firmware in system memory.
    pWprMeta->sysmemAddrOfBootloader =
        memdescGetPhysAddr(pKernelGsp->pGspRmBootUcodeMemdesc, AT_GPU, 0);

    // Set necessary info from bootloader desc
    pWprMeta->bootloaderCodeOffset = pRiscvDesc->monitorCodeOffset;
    pWprMeta->bootloaderDataOffset = pRiscvDesc->monitorDataOffset;
    pWprMeta->bootloaderManifestOffset = pRiscvDesc->manifestOffset;

    if (pKernelGsp->pSignatureMemdesc != NULL)
    {
        pWprMeta->sysmemAddrOfSignature = memdescGetPhysAddr(pKernelGsp->pSignatureMemdesc, AT_GPU, 0);
        pWprMeta->sizeOfSignature = memdescGetSize(pKernelGsp->pSignatureMemdesc);
    }

    // CrashCat queue (if allocated in sysmem)
    KernelCrashCatEngine *pKernelCrashCatEng = staticCast(pKernelGsp, KernelCrashCatEngine);
    MEMORY_DESCRIPTOR *pCrashCatQueueMemDesc = kcrashcatEngineGetQueueMemDesc(pKernelCrashCatEng);
    if (pCrashCatQueueMemDesc != NULL)
    {
        NV_ASSERT_CHECKED(memdescGetAddressSpace(pCrashCatQueueMemDesc) == ADDR_SYSMEM);
        pWprMeta->sysmemAddrOfCrashReportQueue = memdescGetPhysAddr(pCrashCatQueueMemDesc, AT_GPU, 0);
        pWprMeta->sizeOfCrashReportQueue = (NvU32)memdescGetSize(pCrashCatQueueMemDesc);
    }

    if ((osReadRegistryDword(pGpu, NV_REG_STR_RM_BOOT_GSPRM_WITH_BOOST_CLOCKS, &data) == NV_OK) &&
        (data == NV_REG_STR_RM_BOOT_GSPRM_WITH_BOOST_CLOCKS_DISABLED))
    {
        pKernelGsp->bBootGspRmWithBoostClocks = NV_FALSE;
    }

    pWprMeta->bootCount = 0;
    pWprMeta->verified = 0;
    pWprMeta->revision = GSP_FW_WPR_META_REVISION;
    pWprMeta->magic = GSP_FW_WPR_META_MAGIC;

    if (pKernelGsp->bBootGspRmWithBoostClocks)
    {
        pWprMeta->flags |= GSP_FW_FLAGS_CLOCK_BOOST;
    }

#if 0
    NV_PRINTF(LEVEL_ERROR, "WPR meta data offset:     0x%016llx\n", pWprMeta->gspFwWprStart);
    NV_PRINTF(LEVEL_ERROR, "  magic:                  0x%016llx\n", pWprMeta->magic);
    NV_PRINTF(LEVEL_ERROR, "  revision:               0x%016llx\n", pWprMeta->revision);
    NV_PRINTF(LEVEL_ERROR, "  sysmemAddrOfRadix3Elf:  0x%016llx\n", pWprMeta->sysmemAddrOfRadix3Elf);
    NV_PRINTF(LEVEL_ERROR, "  sizeOfRadix3Elf:        0x%016llx\n", pWprMeta->sizeOfRadix3Elf);
    NV_PRINTF(LEVEL_ERROR, "  sysmemAddrOfBootloader: 0x%016llx\n", pWprMeta->sysmemAddrOfBootloader);
    NV_PRINTF(LEVEL_ERROR, "  sizeOfBootloader:       0x%016llx\n", pWprMeta->sizeOfBootloader);
    NV_PRINTF(LEVEL_ERROR, "  sysmemAddrOfSignature:  0x%016llx\n", pWprMeta->sysmemAddrOfSignature);
    NV_PRINTF(LEVEL_ERROR, "  sizeOfSignature:        0x%016llx\n", pWprMeta->sizeOfSignature);
    NV_PRINTF(LEVEL_ERROR, "  gspFwRsvdStart:         0x%016llx\n", pWprMeta->gspFwRsvdStart);
    NV_PRINTF(LEVEL_ERROR, "  nonWprHeap:             0x%016llx - 0x%016llx (0x%016llx)\n", pWprMeta->nonWprHeapOffset, pWprMeta->nonWprHeapOffset + pWprMeta->nonWprHeapSize - 1, pWprMeta->nonWprHeapSize);
    NV_PRINTF(LEVEL_ERROR, "  gspFwWprStart:          0x%016llx\n", pWprMeta->gspFwWprStart);
    NV_PRINTF(LEVEL_ERROR, "  gspFwHeap:              0x%016llx - 0x%016llx (0x%016llx)\n", pWprMeta->gspFwHeapOffset, pWprMeta->gspFwHeapOffset + pWprMeta->gspFwHeapSize - 1, pWprMeta->gspFwHeapSize);
    NV_PRINTF(LEVEL_ERROR, "  gspFwOffset:            0x%016llx - 0x%016llx (0x%016llx)\n", pWprMeta->gspFwOffset, pWprMeta->gspFwOffset + pWprMeta->sizeOfRadix3Elf - 1, pWprMeta->sizeOfRadix3Elf);
    NV_PRINTF(LEVEL_ERROR, "  bootBinOffset:          0x%016llx - 0x%016llx (0x%016llx)\n", pWprMeta->bootBinOffset, pWprMeta->bootBinOffset + pWprMeta->sizeOfBootloader - 1, pWprMeta->sizeOfBootloader);
    NV_PRINTF(LEVEL_ERROR, "  frtsOffset:             0x%016llx - 0x%016llx (0x%016llx)\n", pWprMeta->frtsOffset, pWprMeta->frtsOffset + pWprMeta->frtsSize - 1, pWprMeta->frtsSize);
    NV_PRINTF(LEVEL_ERROR, "  gspFwWprEnd:            0x%016llx\n", pWprMeta->gspFwWprEnd);
    NV_PRINTF(LEVEL_ERROR, "  fbSize:                 0x%016llx\n", pWprMeta->fbSize);
    NV_PRINTF(LEVEL_ERROR, "  vgaWorkspaceOffset:     0x%016llx - 0x%016llx (0x%016llx)\n", pWprMeta->vgaWorkspaceOffset, pWprMeta->vgaWorkspaceOffset + pWprMeta->vgaWorkspaceSize - 1, pWprMeta->vgaWorkspaceSize);
    NV_PRINTF(LEVEL_ERROR, "  bootCount:              0x%016llx\n", pWprMeta->bootCount);
    NV_PRINTF(LEVEL_ERROR, "  verified:               0x%016llx\n", pWprMeta->verified);
#endif

    return NV_OK;
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
kgspExecuteSequencerCommand_TU102
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
            KernelFalcon *pKernelSec2Falcon = staticCast(GPU_GET_KERNEL_SEC2(pGpu), KernelFalcon);

            NV_ASSERT_OK_OR_RETURN(kflcnResetIntoRiscv_HAL(pGpu, pKernelFalcon));
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

/*!
 * Reset the GSP HW
 *
 * @return NV_OK if the GSP HW was properly reset
 */
NV_STATUS
kgspResetHw_TU102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
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

    return NV_OK;
}

static NvBool kgspCrashCatReportImpactsGspRm(CrashCatReport *pReport)
{
    NV_CRASHCAT_CONTAINMENT containment;

    containment = crashcatReportSourceContainment_HAL(pReport);
    switch (containment)
    {
       case NV_CRASHCAT_CONTAINMENT_RISCV_MODE_M:
       case NV_CRASHCAT_CONTAINMENT_RISCV_HART:
       case NV_CRASHCAT_CONTAINMENT_UNCONTAINED:
           return NV_TRUE;
       default:
           return NV_FALSE;
    }
}

NvBool
kgspHealthCheck_TU102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp
)
{
    NvBool bHealthy = NV_TRUE;
    char buildIdString[64];
    LibosElfNoteHeader *pBuildIdNoteHeader = pKernelGsp->pBuildIdSection;

    // CrashCat is the primary reporting interface for GSP issues
    KernelCrashCatEngine *pKernelCrashCatEng = staticCast(pKernelGsp, KernelCrashCatEngine);
    if (kcrashcatEngineConfigured(pKernelCrashCatEng))
    {
        CrashCatEngine *pCrashCatEng = staticCast(pKernelCrashCatEng, CrashCatEngine);
        CrashCatReport *pReport;

        while ((pReport = crashcatEngineGetNextCrashReport(pCrashCatEng)) != NULL)
        {
            if (kgspCrashCatReportImpactsGspRm(pReport))
                bHealthy = NV_FALSE;

            NV_PRINTF(LEVEL_ERROR,
                "****************************** GSP-CrashCat Report *******************************\n");
            crashcatReportLog(pReport);

            kgspInitNocatData(pGpu, pKernelGsp, GSP_NOCAT_CRASHCAT_REPORT);

            // Build id string can be used by offline decoder to decode crashcat data/addresses to symbols
            if (pKernelGsp->pBuildIdSection != NULL)
            {
              portStringBufferToHex(buildIdString, 64, pBuildIdNoteHeader->data + pBuildIdNoteHeader->namesz, pBuildIdNoteHeader->descsz);

              prbEncAddString(&pKernelGsp->nocatData.nocatBuffer,
                              GSP_XIDREPORT_BUILDID,
                              &buildIdString[0]);
            }

            // ErrorCode of nocat event is used for categorizing GSP crash data collected from the field via nocat
            // Since lowest bit of ra is always empty, we use bit 0 to store the sign bit, for
            // differentiating task crash vs libos crash
            // signbit of ra - 1 bit, 0
            //	ra           - (28 - 1) bits, 27:1
            //	scause       - 4 bits, 31:28
            //	stval        - 32 bits, 63:32
            pKernelGsp->nocatData.errorCode |= (crashcatReportRa_HAL(pReport) >> 63) & 1;
            pKernelGsp->nocatData.errorCode |= crashcatReportRa_HAL(pReport) & 0xFFFFFFE;
            pKernelGsp->nocatData.errorCode |= (crashcatReportXcause_HAL(pReport) & 0xF) << 28;
            pKernelGsp->nocatData.errorCode |= (crashcatReportXtval_HAL(pReport) & 0xFFFFFFFF) << 32;

            prbEncAddUInt32(&pKernelGsp->nocatData.nocatBuffer, GSP_XIDREPORT_XID, 120);
            prbEncAddUInt32(&pKernelGsp->nocatData.nocatBuffer, GSP_XIDREPORT_GPUINSTANCE, gpuGetInstance(pGpu));
            crashcatReportLogToProtobuf_HAL(pReport, &pKernelGsp->nocatData.nocatBuffer);
            kgspPostNocatData(pGpu, pKernelGsp, osGetTimestamp());

            objDelete(pReport);
        }
    }

    if (!bHealthy)
    {
        NvBool bFirstFatal = !pKernelGsp->bFatalError;

        pKernelGsp->bFatalError = NV_TRUE;

        if (pKernelGsp->pRpc)
        {
            // Ideally we could have crashcat report and RPC history in the same NOCAT event. But for each NOCAT event
            // there is a size limit of 1k per event, and crashcat data/ rpc history each takes up like 700 bytes, so we have to create 2 events.
            // Technically both event are associated with the xid 120 report
            // Since any non-terminating NOCAT event after the first terminating event will be dropped,
            // we need to set a earlier time here than the Crashcat Nocat event for RPC history to be preserved in NOCAT
            kgspInitNocatData(pGpu, pKernelGsp, GSP_NOCAT_GSP_RPC_HISTORY);
            prbEncAddUInt32(&pKernelGsp->nocatData.nocatBuffer, GSP_XIDREPORT_XID, 120);
            kgspLogRpcDebugInfoToProtobuf(pGpu, pKernelGsp->pRpc, pKernelGsp, &pKernelGsp->nocatData.nocatBuffer);
            kgspPostNocatData(pGpu, pKernelGsp, pKernelGsp->pRpc->rpcHistory[pKernelGsp->pRpc->rpcHistoryCurrent].ts_start);

            kgspLogRpcDebugInfo(pGpu, pKernelGsp->pRpc, GSP_ERROR, pKernelGsp->bPollingForRpcResponse);
        }

        if (bFirstFatal)
        {
            kgspRcAndNotifyAllChannels(pGpu, pKernelGsp, GSP_ERROR, NV_TRUE);
            gpuMarkDeviceForReset(pGpu);
        }

        gpuCheckEccCounts_HAL(pGpu);

        NV_PRINTF(LEVEL_ERROR,
                  "**********************************************************************************\n");

        if (pGpu->getProperty(pGpu, PDB_PROP_GPU_SUPPORTS_TDR_EVENT))
        {
            NV_ASSERT_FAILED("GSP timed out. Triggering TDR.");
            gpuNotifySubDeviceEvent(pGpu, NV2080_NOTIFIERS_UCODE_RESET, NULL, 0, 0, 0);
        }
    }
    return bHealthy;
}

/*!
 * GSP Interrupt Service Routine
 *
 * @return 32-bit interrupt status AFTER all known interrupt-sources were
 *         serviced.
 */
NvU32
kgspService_TU102
(
    OBJGPU     *pGpu,
    KernelGsp  *pKernelGsp
)
{
    NvU32         intrStatus;
    KernelFalcon *pKernelFalcon = staticCast(pKernelGsp, KernelFalcon);

    // Get the IRQ status for sources routed to host
    intrStatus = kflcnGetPendingHostInterrupts(pGpu, pKernelFalcon);

    // Exit immediately if there is nothing to do
    if (intrStatus == 0)
    {
        NV_ASSERT_FAILED("KGSP service called when no KGSP interrupt pending\n");
        return 0;
    }

    if (!API_GPU_ATTACHED_SANITY_CHECK(pGpu))
    {
        NV_PRINTF(LEVEL_ERROR, "GPU is detached, bailing!\n");
        return 0;
    }

    if (intrStatus & DRF_DEF(_PFALCON, _FALCON_IRQSTAT, _HALT, _TRUE))
    {
        //
        // The _HALT is triggered by ucode as part of the CrashCat protocol to
        // signal the host that some handling is required. Clear the interrupt
        // before handling, so that once the GSP code continues, we won't miss
        // a second _HALT interrupt for the next step.
        //
        kflcnRegWrite_HAL(pGpu, pKernelFalcon, NV_PFALCON_FALCON_IRQSCLR,
            DRF_DEF(_PFALCON, _FALCON_IRQSCLR, _HALT, _SET));

        kgspDumpGspLogs(pKernelGsp, NV_FALSE);
        (void)kgspHealthCheck_HAL(pGpu, pKernelGsp);
#if defined(DEBUG)
        NV_PRINTF(LEVEL_ERROR, "GSP-RM entered into ICD\n");
        DBG_BREAKPOINT();
#endif
    }
    if (intrStatus & DRF_DEF(_PFALCON, _FALCON_IRQSTAT, _SWGEN0, _TRUE))
    {
        //
        // Clear edge triggered interrupt BEFORE (and never after)
        // servicing it to avoid race conditions.
        //
        kflcnRegWrite_HAL(pGpu, pKernelFalcon, NV_PFALCON_FALCON_IRQSCLR,
            DRF_DEF(_PFALCON, _FALCON_IRQSCLR, _SWGEN0, _SET));

        kgspRpcRecvEvents(pGpu, pKernelGsp);

        //
        // If lockdown has been engaged (as notified by an RPC event),
        // we shouldn't access any more GSP registers.
        //
        NV_CHECK_OR_RETURN(LEVEL_SILENT, !pKernelGsp->bInLockdown, 0);
    }

    kgspServiceFatalHwError_HAL(pGpu, pKernelGsp, intrStatus);

    if (intrStatus & kflcnGetEccInterruptMask_HAL(pGpu, pKernelFalcon))
    {
        kgspEccServiceEvent_HAL(pGpu, pKernelGsp);
    }

    //
    // Don't retrigger for fatal errors since they can't be cleared without an
    // engine reset, which results in an interrupt storm until reset
    //
    if (!pKernelGsp->bFatalError)
    {
        kflcnIntrRetrigger_HAL(pGpu, pKernelFalcon);
    }

    return kflcnGetPendingHostInterrupts(pGpu, pKernelFalcon);
}

static NvBool
_kgspIsProcessorSuspended
(
    OBJGPU  *pGpu,
    void    *pVoid
)
{
    KernelGsp *pKernelGsp = reinterpretCast(pVoid, KernelGsp *);
    NvU32 mailbox;

    // Check for LIBOS_INTERRUPT_PROCESSOR_SUSPENDED in mailbox
    mailbox = kflcnRegRead_HAL(pGpu, staticCast(pKernelGsp, KernelFalcon),
                               NV_PFALCON_FALCON_MAILBOX0);
    return (mailbox == 0x80000000);
}

NV_STATUS
kgspWaitForProcessorSuspend_TU102
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    return gpuTimeoutCondWait(pGpu, _kgspIsProcessorSuspended, pKernelGsp, NULL);
}

NvBool
kgspIsWpr2Up_TU102
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    NvU32 data = GPU_REG_RD32(pGpu, NV_PFB_PRI_MMU_WPR2_ADDR_HI);
    NvU32 wpr2HiVal = DRF_VAL(_PFB, _PRI_MMU_WPR2_ADDR_HI, _VAL, data);
    return (wpr2HiVal != 0);
}

NV_STATUS
kgspWaitForGfwBootOk_TU102
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    NV_STATUS status = NV_OK;

    status = gpuWaitForGfwBootComplete_HAL(pGpu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to wait for GFW boot complete: 0x%x VBIOS version %s\n",
                  status, pKernelGsp->vbiosVersionStr);
        NV_PRINTF(LEVEL_ERROR, "(the GPU may be in a bad state and may need to be reset)\n");
    }

    return status;
}

void
kgspFreeSuspendResumeData_TU102
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    // release sr meta data resources
    if (pKernelGsp->pSRMetaDescriptor != NULL)
    {
        memdescFree(pKernelGsp->pSRMetaDescriptor);
        memdescDestroy(pKernelGsp->pSRMetaDescriptor);
        pKernelGsp->pSRMetaDescriptor = NULL;
    }

    // release sr meta data resources
    if (pKernelGsp->pSRRadix3Descriptor != NULL)
    {
        memdescFree(pKernelGsp->pSRRadix3Descriptor);
        memdescDestroy(pKernelGsp->pSRRadix3Descriptor);
        pKernelGsp->pSRRadix3Descriptor = NULL;
    }
}

NV_STATUS
kgspPrepareSuspendResumeData_TU102
(
    OBJGPU    *pGpu,
    KernelGsp *pKernelGsp
)
{
    GspFwSRMeta gspfwSRMeta;
    NvP64 pVa = NvP64_NULL;
    NvP64 pPriv = NvP64_NULL;
    NV_STATUS nvStatus = NV_OK;

    // Fill in GspFwSRMeta structure
    portMemSet(&gspfwSRMeta, 0, sizeof(gspfwSRMeta));
    gspfwSRMeta.magic                   = GSP_FW_SR_META_MAGIC;
    gspfwSRMeta.revision                = GSP_FW_SR_META_REVISION;
    // Region to be saved is from start of WPR2 till end of frts.
    gspfwSRMeta.sizeOfSuspendResumeData = (pKernelGsp->pWprMeta->frtsOffset + pKernelGsp->pWprMeta->frtsSize) -
                                          (pKernelGsp->pWprMeta->nonWprHeapOffset + pKernelGsp->pWprMeta->nonWprHeapSize);
    gspfwSRMeta.flags                   = pKernelGsp->pWprMeta->flags;

    NV_ASSERT_OK_OR_GOTO(nvStatus,
                         kgspCreateRadix3(pGpu,
                                          pKernelGsp,
                                          &pKernelGsp->pSRRadix3Descriptor,
                                          NULL,
                                          NULL,
                                          gspfwSRMeta.sizeOfSuspendResumeData),
                         exit_fail_cleanup);

    gspfwSRMeta.sysmemAddrOfSuspendResumeData = memdescGetPhysAddr(pKernelGsp->pSRRadix3Descriptor, AT_GPU, 0);

    // Create SR Metadata Area
    NV_ASSERT_OK_OR_GOTO(nvStatus,
                         memdescCreate(&pKernelGsp->pSRMetaDescriptor,
                                       pGpu,
                                       sizeof(GspFwSRMeta),
                                       256,
                                       NV_TRUE,
                                       ADDR_SYSMEM,
                                       NV_MEMORY_UNCACHED,
                                       MEMDESC_FLAGS_NONE),
                         exit_fail_cleanup);

    memdescTagAlloc(nvStatus, NV_FB_ALLOC_RM_INTERNAL_OWNER_SR_METADATA,
                    pKernelGsp->pSRMetaDescriptor);
    NV_ASSERT_OK_OR_GOTO(nvStatus, nvStatus,
                         exit_fail_cleanup);

    // Copy SR Metadata Structure
    NV_ASSERT_OK_OR_GOTO(nvStatus,
                         memdescMap(pKernelGsp->pSRMetaDescriptor,
                                    0,
                                    memdescGetSize(pKernelGsp->pSRMetaDescriptor),
                                    NV_TRUE,
                                    NV_PROTECT_WRITEABLE,
                                    &pVa,
                                    &pPriv),
                         exit_fail_cleanup);

    portMemCopy(pVa, sizeof(gspfwSRMeta), &gspfwSRMeta, sizeof(gspfwSRMeta));

    memdescUnmap(pKernelGsp->pSRMetaDescriptor,
                 NV_TRUE, osGetCurrentProcess(),
                 pVa, pPriv);

    return nvStatus;

exit_fail_cleanup:
    kgspFreeSuspendResumeData_HAL(pGpu, pKernelGsp);
    return nvStatus;
}

void
kgspReadEmem_TU102
(
    KernelGsp *pKernelGsp,
    NvU64      offset,
    NvU64      size,
    void      *pBuf
)
{
    NvU32 ememMask = DRF_SHIFTMASK(NV_PGSP_EMEMC_OFFS) | DRF_SHIFTMASK(NV_PGSP_EMEMC_BLK);
    OBJGPU *pGpu = ENG_GET_GPU(pKernelGsp);
    NvU32 limit = size - NVBIT(DRF_SHIFT(NV_PGSP_EMEMC_OFFS));
    NvU32 *pBuffer = pBuf;

    portMemSet(pBuf, 0, size);

#if defined(DEBUG) || defined(DEVELOP)
    NV_ASSERT_OR_RETURN_VOID((offset & ~ememMask) == 0);
    NV_ASSERT_OR_RETURN_VOID(limit <= ememMask);
    NV_ASSERT_OR_RETURN_VOID(offset + limit <= ememMask);
#else
    NV_CHECK_OR_RETURN_VOID(LEVEL_SILENT, (offset & ~ememMask) == 0);
    NV_CHECK_OR_RETURN_VOID(LEVEL_SILENT, limit <= ememMask);
    NV_CHECK_OR_RETURN_VOID(LEVEL_SILENT, offset + limit <= ememMask);
#endif

    GPU_REG_WR32(pGpu, NV_PGSP_EMEMC(pKernelGsp->ememPort),
                 offset | DRF_DEF(_PGSP, _EMEMC, _AINCR, _TRUE));

    for (NvU32 idx = 0; idx < size / sizeof(NvU32); idx++)
        pBuffer[idx] = GPU_REG_RD32(pGpu, NV_PGSP_EMEMD(pKernelGsp->ememPort));
}
