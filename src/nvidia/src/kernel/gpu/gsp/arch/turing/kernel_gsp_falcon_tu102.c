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
 * Provides TU102+ specific KernelGsp HAL implementations related to
 * execution of Falcon cores.
 */

#include "gpu/gsp/kernel_gsp.h"

#include "gpu/gpu.h"
#include "gpu/falcon/kernel_falcon.h"
#include "gpu/sec2/kernel_sec2.h"
#include "rmflcnbl.h"
#include "rmgspseq.h"

#include "published/turing/tu102/dev_falcon_v4.h"
#include "published/turing/tu102/dev_fbif_v4.h"
#include "published/turing/tu102/dev_gc6_island.h"
#include "published/turing/tu102/dev_gc6_island_addendum.h"

/*!
 * Copy sizeBytes from pSrc to DMEM offset dmemDest using DMEM access port 0.
 *
 * @param[in] pGpu          GPU object pointer
 * @param[in] pKernelFlcn   KernelFalcon pointer
 * @param[in] dmemDest      Destination in DMEM
 * @param[in] pSrc          Desired DMEM contents
 * @param[in] sizeInBytes   Number of bytes to copy
 */
static NV_STATUS
s_dmemCopyTo_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU32 dmemDest,
    const NvU8 *pSrc,
    NvU32 sizeBytes
)
{
    NvU32 numWords;
    NvU32 wordIdx;
    NvU32 *pSrcWords;
    NvU32 reg32;

    if (sizeBytes == 0)
    {
        return NV_OK;
    }

    NV_ASSERT_OR_RETURN(RM_IS_ALIGNED(dmemDest, FLCN_DMEM_ACCESS_ALIGNMENT), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pSrc != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(RM_IS_ALIGNED(sizeBytes, FLCN_DMEM_ACCESS_ALIGNMENT), NV_ERR_INVALID_ARGUMENT);

    numWords = sizeBytes >> 2;
    pSrcWords = (NvU32 *) pSrc;

    // Prepare DMEMC register
    reg32 = kflcnMaskDmemAddr_HAL(pGpu, pKernelFlcn, dmemDest);
    reg32 = FLD_SET_DRF_NUM(_PFALCON, _FALCON_DMEMC, _AINCW, 0x1, reg32);
    kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_DMEMC(0), reg32);

    for (wordIdx = 0; wordIdx < numWords; wordIdx++)
    {
        // Write DMEM data
        kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_DMEMD(0), pSrcWords[wordIdx]);
    }

    return NV_OK;
}

/*!
 * Copy sizeBytes from pSrc to IMEM offset imemDest using IMEM access port 0.
 *
 * @param[in] pGpu          GPU object pointer
 * @param[in] pKernelFlcn   KernelFalcon pointer
 * @param[in] imemDest      Destination in IMEM
 * @param[in] pSrc          Desired IMEM contents
 * @param[in] sizeInBytes   Number of bytes to copy
 * @param[in] bSecure       Whether IMEM copied should be marked secure
 * @param[in] tag           Desired IMEM tag
 */
static NV_STATUS
s_imemCopyTo_TU102
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU32 imemDest,
    const NvU8 *pSrc,
    NvU32 sizeBytes,
    NvBool bSecure,
    NvU32 tag
)
{
    NvU32 numWords;
    NvU32 wordIdx;
    NvU32 *pSrcWords;
    NvU32 reg32;

    if (sizeBytes == 0)
    {
        return NV_OK;
    }

    // Require block alignment on IMEM addr (due to tagging at block granularity)
    NV_ASSERT_OR_RETURN(RM_IS_ALIGNED(imemDest, FLCN_BLK_ALIGNMENT), NV_ERR_INVALID_ARGUMENT);

    NV_ASSERT_OR_RETURN(pSrc != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(RM_IS_ALIGNED(sizeBytes, FLCN_IMEM_ACCESS_ALIGNMENT), NV_ERR_INVALID_ARGUMENT);

    numWords = sizeBytes >> 2;
    pSrcWords = (NvU32 *) pSrc;

    // Prepare IMEMC register
    reg32 = kflcnMaskImemAddr_HAL(pGpu, pKernelFlcn, imemDest);
    reg32 = FLD_SET_DRF_NUM(_PFALCON_FALCON, _IMEMC, _AINCW, 0x1, reg32);
    reg32 = FLD_SET_DRF_NUM(_PFALCON_FALCON, _IMEMC, _SECURE, bSecure, reg32);
    kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_IMEMC(0), reg32);

    tag = tag >> 8;
    for (wordIdx = 0; wordIdx < numWords; wordIdx++)
    {
        // Tag blocks (at block granularity)
        if ((wordIdx & ((1u << (FALCON_IMEM_BLKSIZE2 - 2)) - 1)) == 0)
        {
            kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_IMEMT(0),
                              DRF_NUM(_PFALCON_FALCON, _IMEMT, _TAG, tag));
            tag++;
        }

        // Write IMEM data
        kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_IMEMD(0),
                          DRF_NUM(_PFALCON_FALCON, _IMEMD, _DATA, pSrcWords[wordIdx]));
    }

    return NV_OK;
}

static NV_STATUS
s_prepareHsFalconDirect
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    KernelGspFlcnUcodeBootDirect *pUcode
)
{
    NV_STATUS status = NV_OK;

    NV_ASSERT_OR_RETURN(pUcode->pImage != NULL, NV_ERR_INVALID_ARGUMENT);

    kflcnDisableCtxReq_HAL(pGpu, pKernelFlcn);

    // Copy non-secure IMEM code
    NV_ASSERT_OK_OR_RETURN(
        s_imemCopyTo_TU102(pGpu, pKernelFlcn,
                           0,
                           pUcode->pImage + pUcode->imemNsPa,
                           pUcode->imemNsSize,
                           NV_FALSE,
                           pUcode->imemNsPa));

    // Copy secure IMEM code after non-secure block
    NV_ASSERT_OK_OR_RETURN(
        s_imemCopyTo_TU102(pGpu, pKernelFlcn,
                           NV_ALIGN_UP(pUcode->imemNsSize, FLCN_BLK_ALIGNMENT),
                           pUcode->pImage + pUcode->imemSecPa,
                           pUcode->imemSecSize,
                           NV_TRUE,
                           pUcode->imemSecPa));

    // Load DMEM (note: signatures must already be patched)
    NV_ASSERT_OK_OR_RETURN(
        s_dmemCopyTo_TU102(pGpu, pKernelFlcn,
                           pUcode->dmemPa,
                           pUcode->pImage + pUcode->dataOffset,
                           pUcode->dmemSize));

    // Set BOOTVEC to start of non-secure code
    kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_BOOTVEC, 0);

    return status;
}

/*!
 * Creates a memory aperture that the falcon may use to access memory in a
 * specific address-space. Once the aperture is established, it may only be
 * used to access this one specific address-space (FBMEM/SYSMEM(COH)/
 * SYSMEM(NONCOH)). The aperture is identified using a unique-index that will
 * correspond to a single dmaidx in the PMU framebuffer-interface.
 *
 * @param[in]  pGpu            GPU object pointer
 * @param[in]  dmaIdx          The DMA/aperture index to setup
 * @param[in]  addrSpace       @see _def_memory_descriptor::AddressSpace
 * @param[in]  cpuCacheAddrib  @see _def_memory_descriptor::CpuCacheAttrib
 *
 * @return 'NV_OK' upon successful setup of the aperture
 */
static NV_STATUS
s_setupLoaderAperture
(
    OBJGPU  *pGpu,
    KernelFalcon *pKernelFlcn,
    NvU32    dmaIdx,
    NvU32    addrSpace,
    NvU32    cpuCacheAttrib
)
{
    NvU32 target;
    NvU32 data;

    if (addrSpace == ADDR_FBMEM)
    {
        target = NV_PFALCON_FBIF_TRANSCFG_TARGET_LOCAL_FB;
    }
    else if ((addrSpace == ADDR_SYSMEM) &&
            (cpuCacheAttrib == NV_MEMORY_CACHED))
    {
        target = NV_PFALCON_FBIF_TRANSCFG_TARGET_COHERENT_SYSMEM;
    }
    else if ((addrSpace == ADDR_SYSMEM) &&
            (cpuCacheAttrib == NV_MEMORY_UNCACHED))
    {
        target = NV_PFALCON_FBIF_TRANSCFG_TARGET_NONCOHERENT_SYSMEM;
    }
    else
    {
        NV_ASSERT_FAILED("Unsupported address space/cache attribute combination");
        return NV_ERR_INVALID_ARGUMENT;
    }

    data = GPU_REG_RD32(pGpu, pKernelFlcn->fbifBase + NV_PFALCON_FBIF_TRANSCFG(dmaIdx));
    data = FLD_SET_DRF_NUM(_PFALCON, _FBIF_TRANSCFG, _TARGET, target, data);
    data = FLD_SET_DRF(_PFALCON, _FBIF_TRANSCFG, _MEM_TYPE, _PHYSICAL, data);
    GPU_REG_WR32(pGpu, pKernelFlcn->fbifBase + NV_PFALCON_FBIF_TRANSCFG(dmaIdx), data);

    return NV_OK;
}

static NV_STATUS
s_setupLoader
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    const RM_FLCN_BL_DMEM_DESC *pBlDesc,
    NvU32 addrSpace,
    NvU32 cpuCacheAttrib
)
{
    KernelSec2 *pKernelSec2 = GPU_GET_KERNEL_SEC2(pGpu);
    NV_STATUS status = NV_OK;

    const RM_FLCN_BL_DESC *pBlUcDesc = NULL;
    const NvU8 *pBlImg = NULL;

    NvU32 imemDstBlk;
    NvU32 virtAddr;
    NvU32 blSize;

    NV_ASSERT_OR_RETURN(pBlDesc != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pKernelSec2 != NULL, NV_ERR_INVALID_STATE);

    // Get the generic BL image and descriptor from SEC2
    NV_ASSERT_OK_OR_RETURN(ksec2GetGenericBlUcode_HAL(pGpu, pKernelSec2, &pBlUcDesc, &pBlImg));
    blSize = NV_ALIGN_UP(pBlUcDesc->blImgHeader.blCodeSize, FLCN_BLK_ALIGNMENT);

    kflcnDisableCtxReq_HAL(pGpu, pKernelFlcn);

    // Program TRANSCFG to fetch the DMA data
    NV_ASSERT_OK_OR_RETURN(s_setupLoaderAperture(pGpu, pKernelFlcn, pBlDesc->ctxDma, addrSpace, cpuCacheAttrib));

    // Copy dmem desc to DMEM offset 0
    NV_ASSERT_OK_OR_RETURN(
        s_dmemCopyTo_TU102(pGpu, pKernelFlcn,
                           0,
                           (NvU8 *) pBlDesc,
                           sizeof(RM_FLCN_BL_DMEM_DESC)));

    // Compute location of bootloader at top of IMEM
    {
        NvU32 imemSizeBlk = DRF_VAL(_PFALCON, _FALCON_HWCFG, _IMEM_SIZE,
                                    kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_HWCFG));
        imemDstBlk = imemSizeBlk - blSize / (1u << FALCON_IMEM_BLKSIZE2);
    }

    // Copy bootloader to top of IMEM
    virtAddr = pBlUcDesc->blStartTag << 8;
    NV_ASSERT_OK_OR_RETURN(
        s_imemCopyTo_TU102(pGpu, pKernelFlcn,
                           imemDstBlk << FALCON_IMEM_BLKSIZE2,
                           pBlImg,
                           blSize,
                           NV_FALSE,
                           virtAddr));

    // Set BOOTVEC to location of bootloader
    kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_BOOTVEC, virtAddr);

    return status;
}

static NV_STATUS
s_prepareHsFalconWithLoader
(
    OBJGPU *pGpu,
    KernelFalcon *pKernelFlcn,
    KernelGspFlcnUcodeBootWithLoader *pUcode
)
{
    RM_FLCN_BL_DMEM_DESC blDmemDesc;
    NvU64 ucodePACode;
    NvU64 ucodePAData;

    NV_ASSERT_OR_RETURN(pUcode->pCodeMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pUcode->pDataMemDesc != NULL, NV_ERR_INVALID_ARGUMENT);

    // Note: adapted from _vbiosFwseclicCmdOffloadToFlcn
    ucodePACode = memdescGetPhysAddr(pUcode->pCodeMemDesc, AT_GPU, 0);
    ucodePAData = memdescGetPhysAddr(pUcode->pDataMemDesc, AT_GPU, 0);

    blDmemDesc.signature[0] = 0;
    blDmemDesc.signature[1] = 0;
    blDmemDesc.signature[2] = 0;
    blDmemDesc.signature[3] = 0;
    blDmemDesc.ctxDma = 4; // dmaIdx for PHYS_SYS_NCOH, consumed by the generic falcon boot loader
    RM_FLCN_U64_PACK(&blDmemDesc.codeDmaBase, &ucodePACode);

    blDmemDesc.nonSecureCodeOff = pUcode->imemNsPa;
    blDmemDesc.nonSecureCodeSize = pUcode->imemNsSize;

    blDmemDesc.secureCodeOff = pUcode->imemSecPa;
    blDmemDesc.secureCodeSize = pUcode->imemSecSize;

    blDmemDesc.codeEntryPoint = 0;

    RM_FLCN_U64_PACK(&blDmemDesc.dataDmaBase, &ucodePAData);
    blDmemDesc.dataSize = pUcode->dmemSize;

    NV_ASSERT_OK_OR_RETURN(s_setupLoader(pGpu, pKernelFlcn, &blDmemDesc, ADDR_SYSMEM, NV_MEMORY_CACHED));

    return NV_OK;
}

/*!
 * Execute the HS falcon ucode provided in pFlcnUcode on the falcon engine
 * represented by pKernelFlcn and wait for its completion.
 *
 * For _TU102, pFlcnUcode must be of the DIRECT or WITH_LOADER variant.
 *
 * Note: callers are expected to reset pKernelFlcn before calling this
 * function.

 * @param[in]     pGpu            GPU object pointer
 * @param[in]     pKernelGsp      KernelGsp object pointer
 * @param[in]     pFlcnUcode      Falcon ucode to execute
 * @param[in]     pKernelFlcn     KernelFalcon engine to execute on
 * @param[inout]  pMailbox0       Pointer to value of MAILBOX0 to provide/read (or NULL)
 * @param[inout]  pMailbox0       Pointer to value of MAILBOX1 to provide/read (or NULL)
 */
NV_STATUS
kgspExecuteHsFalcon_TU102
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    KernelGspFlcnUcode *pFlcnUcode,
    KernelFalcon *pKernelFlcn,
    NvU32 *pMailbox0,
    NvU32 *pMailbox1
)
{
    NV_STATUS status;

    NV_ASSERT_OR_RETURN(pFlcnUcode != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pKernelFlcn != NULL, NV_ERR_INVALID_STATE);

    NV_ASSERT_OR_RETURN(!pKernelFlcn->bBootFromHs, NV_ERR_NOT_SUPPORTED);

    // Prepare IMEM, DMEM, BOOTVEC, etc. as appropriate for boot type
    if (pFlcnUcode->bootType == KGSP_FLCN_UCODE_BOOT_WITH_LOADER)
    {
        status = s_prepareHsFalconWithLoader(pGpu, pKernelFlcn, &pFlcnUcode->ucodeBootWithLoader);
    }
    else if (pFlcnUcode->bootType == KGSP_FLCN_UCODE_BOOT_DIRECT)
    {
        status = s_prepareHsFalconDirect(pGpu, pKernelFlcn, &pFlcnUcode->ucodeBootDirect);
    }
    else
    {
        status = NV_ERR_NOT_SUPPORTED;
    }

    if (status != NV_OK)
    {
        return status;
    }

    // Write mailboxes if requested
    if (pMailbox0 != NULL)
        kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_MAILBOX0, *pMailbox0);
    if (pMailbox1 != NULL)
        kflcnRegWrite_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_MAILBOX1, *pMailbox1);

    // Start CPU now
    kflcnStartCpu_HAL(pGpu, pKernelFlcn);

    // Wait for completion
    status = kflcnWaitForHalt_HAL(pGpu, pKernelFlcn, GPU_TIMEOUT_DEFAULT, 0);

    // Read mailboxes if requested
    if (pMailbox0 != NULL)
        *pMailbox0 = kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_MAILBOX0);
    if (pMailbox1 != NULL)
        *pMailbox1 = kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_MAILBOX1);

    return status;
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

NV_STATUS
kgspExecuteCoreResume_TU102(OBJGPU *pGpu, KernelGsp *pKernelGsp)
{
    KernelFalcon   *pKernelFalcon = staticCast(pKernelGsp, KernelFalcon);
    RM_RISCV_UCODE_DESC *pRiscvDesc = pKernelGsp->pGspRmBootUcodeDesc;
    KernelFalcon *pKernelSec2Falcon = staticCast(GPU_GET_KERNEL_SEC2(pGpu), KernelFalcon);
    NV_STATUS status;
    NvU32 secMailbox0;

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
        NV_PRINTF(LEVEL_ERROR, "Timeout waiting for SEC2-RTOS to resume GSP-RM. SEC2 Mailbox0 is : 0x%x\n",
            secMailbox0);
        return NV_ERR_TIMEOUT;
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

    return status;
}

/*!
 * Load and execute the generic bootloader on GSP
 *
 * @param[in]      pGpu             GPU object pointer
 * @param[in]      pKernelGsp       KernelGsp object pointer
 * @param[in]      pParams          Generic bootloader execution parameters from GSP
 *
 * @return NV_OK if the GSP boot loader was executed successfully
 */
 NV_STATUS
 kgspLoadAndExecuteGenericBootloader_TU102
 (
     OBJGPU    *pGpu,
     KernelGsp *pKernelGsp,
     GspLoadExecGenericBootloaderParams *pParams
 )
 {
    RM_FLCN_BL_DMEM_DESC  *pBlDesc = &pParams->dmemDesc;
    KernelFalcon          *pKernelFlcn = staticCast(pKernelGsp, KernelFalcon);
    NvU32                  valueToBeRestored;
    NV_STATUS              status;

    // Assert that a supported DMA index is being used.
    NV_ASSERT_OR_RETURN(pBlDesc->ctxDma < NV_PFALCON_FBIF_TRANSCFG__SIZE_1, NV_ERR_INVALID_ARGUMENT);

    // For now we expect our size for RM_FLCN_BL_DMEM_DESC to match GSP's size.
    NV_ASSERT_OR_RETURN(sizeof(RM_FLCN_BL_DMEM_DESC) == pParams->dmemDescSize, NV_ERR_INVALID_ARGUMENT);

    // Wait for the GSP processor suspend to complete.
    status = kgspWaitForProcessorSuspend_HAL(pGpu, pKernelGsp);
    if (status != NV_OK)
    {
        NvU32 mailbox0 = kflcnRegRead_HAL(pGpu, pKernelFlcn, NV_PFALCON_FALCON_MAILBOX0);
        NV_PRINTF(LEVEL_ERROR, "Timeout waiting for falcon to suspend. mailbox0: 0x%x\n", mailbox0);
        return status;
    }

    // Reset GSP processor
    NV_ASSERT_OK_OR_RETURN(kflcnReset_HAL(pGpu, pKernelFlcn));
    kflcnDisableCtxReq_HAL(pGpu, pKernelFlcn);

    // Setup aperture for FBMEM
    valueToBeRestored = GPU_REG_RD32(pGpu, pKernelFlcn->fbifBase + NV_PFALCON_FBIF_TRANSCFG(pBlDesc->ctxDma));
    NV_ASSERT_OK_OR_RETURN(s_setupLoader(pGpu, pKernelFlcn, pBlDesc, pParams->addrSpace, pParams->cpuCacheAttrib));

    //
    // Write FLCN_ERR_BINARY_NOT_STARTED in falcon mailbox register
    // For binaries like ACR, which reads MAILBOX0 register to propagate error code, 0 is considered as success
    // If binary fails to start, ACR thinks it has succeeded even it is not. Writing NOT_STARTED help to identify
    // such cases.
    kflcnRegWrite_HAL(pGpu, pKernelFlcn,
        NV_PFALCON_FALCON_MAILBOX0, FLCN_ERR_BINARY_NOT_STARTED);

    // Start CPU
    kflcnStartCpu_HAL(pGpu, pKernelFlcn);

    // Wait for the bootloader to complete execution.
    NV_ASSERT_OK_OR_RETURN(kflcnWaitForHalt_HAL(pGpu, pKernelFlcn, GPU_TIMEOUT_DEFAULT, 0));

    // Restore the FBIF register
    GPU_REG_WR32(pGpu, pKernelFlcn->fbifBase + NV_PFALCON_FBIF_TRANSCFG(pBlDesc->ctxDma), valueToBeRestored);

    // Perform RISCV resume
    NV_ASSERT_OK_OR_RETURN(kgspExecuteCoreResume_HAL(pGpu, pKernelGsp));

    return NV_OK;
}
