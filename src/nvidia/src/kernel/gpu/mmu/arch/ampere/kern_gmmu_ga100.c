/*
 * SPDX-FileCopyrightText: Copyright (c) 2018-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#define NVOC_KERN_GMMU_H_PRIVATE_ACCESS_ALLOWED

#include "gpu/mmu/kern_gmmu.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/mem_utils.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/nvlink/kernel_nvlink.h"
#include "nverror.h"

#include "published/ampere/ga100/dev_fault.h"
#include "published/ampere/ga100/dev_vm.h"


/*!
 * @brief   Sets the Invalidation scope field in the register
 *
 * @param[in]       pGpu
 * @param[in]       pKernelGmmu
 * @param[in]       flags
 * @param[in/out]   TLB_INVALIDATE_PARAMS pointer
 *
 * @returns NV_ERR_INVALID_ARGUMENT on input validation
 *          NV_OK on success
 */
NV_STATUS
kgmmuSetTlbInvalidationScope_GA100
(
    OBJGPU                *pGpu,
    KernelGmmu            *pKernelGmmu,
    NvU32                  flags,
    TLB_INVALIDATE_PARAMS *pParams
)
{
    switch(flags)
    {
        case NV_GMMU_INVAL_SCOPE_ALL_TLBS:
            pParams->regVal = FLD_SET_DRF(_VIRTUAL_FUNCTION_PRIV, _MMU_INVALIDATE, _INVAL_SCOPE,
                                          _ALL_TLBS, pParams->regVal);
            break;
        case NV_GMMU_INVAL_SCOPE_LINK_TLBS:
            pParams->regVal = FLD_SET_DRF(_VIRTUAL_FUNCTION_PRIV, _MMU_INVALIDATE, _INVAL_SCOPE,
                                          _LINK_TLBS, pParams->regVal);
            break;
        case NV_GMMU_INVAL_SCOPE_NON_LINK_TLBS:
            pParams->regVal = FLD_SET_DRF(_VIRTUAL_FUNCTION_PRIV, _MMU_INVALIDATE, _INVAL_SCOPE,
                                          _NON_LINK_TLBS, pParams->regVal);
            break;
        default:
            return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

/*!
 * @brief   Validates fabric base address.
 *
 * @param   pKernelGmmu
 * @param   fabricBaseAddr
 *
 * @returns On success, NV_OK.
 *          On failure, returns NV_ERR_XXX.
 */
NV_STATUS
kgmmuValidateFabricBaseAddress_GA100
(
    KernelGmmu *pKernelGmmu,
    NvU64       fabricBaseAddr
)
{
    OBJGPU        *pGpu = ENG_GET_GPU(pKernelGmmu);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvU64 fbSizeBytes;
    NvU64 fbUpperLimit;

    fbSizeBytes = pMemoryManager->Ram.fbTotalMemSizeMb << 20;

    //
    // Ampere SKUs will be paired with NVSwitches (Limerock) supporting 2K
    // mapslots that can cover 64GB each. Make sure that the fabric base
    // address being used is valid to cover whole frame buffer.
    //

    // Check if fabric address is aligned to mapslot size.
    if (fabricBaseAddr & (NVBIT64(36) - 1))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Align fbSize to mapslot size.
    fbSizeBytes = RM_ALIGN_UP(fbSizeBytes, NVBIT64(36));

    // Check for integer overflow
    if (!portSafeAddU64(fabricBaseAddr, fbSizeBytes, &fbUpperLimit))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Make sure the address range doesn't go beyond the limit, (2K * 64GB).
    if (fbUpperLimit > NVBIT64(47))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    return NV_OK;
}

static NV_STATUS
kgmmuSetupWarForBug2720120FmtFamily_GA100
(
    KernelGmmu      *pKernelGmmu,
    GMMU_FMT_FAMILY *pFam
)
{
    NV_STATUS            status      = NV_OK;
    OBJGPU              *pGpu        = ENG_GET_GPU(pKernelGmmu);
    KernelBus           *pKernelBus  = GPU_GET_KERNEL_BUS(pGpu);
    MemoryManager       *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    const GMMU_FMT      *pFmt        = kgmmuFmtGet(pKernelGmmu, GMMU_FMT_VERSION_DEFAULT, 0);
    const MMU_FMT_LEVEL *pPageDir1   = mmuFmtFindLevelWithPageShift(pFmt->pRoot, 29);
    const MMU_FMT_LEVEL *pPageDir0   = mmuFmtFindLevelWithPageShift(pFmt->pRoot, 21);
    const MMU_FMT_LEVEL *pSmallPT    = mmuFmtFindLevelWithPageShift(pFmt->pRoot, 12);
    const GMMU_FMT_PDE  *pPde0Fmt    = gmmuFmtGetPde(pFmt, pPageDir0, 1);
    const GMMU_FMT_PDE  *pPde1Fmt    = gmmuFmtGetPde(pFmt, pPageDir1, 0);
    NvU32                entryIndex;
    NvU32                entryIndexHi;

    //
    // BAR2 is not yet initialized. Thus use either the BAR0 window or
    // memmap to initialize the given surface.
    //
    NV_ASSERT(pKernelBus->virtualBar2[GPU_GFID_PF].pCpuMapping == NULL);

    // Initialize the memdescs to NULL before use
    pKernelGmmu->pWarSmallPageTable = NULL;
    pKernelGmmu->pWarPageDirectory0 = NULL;

    // Bug 2720120: Allocate a small page table consisting of all invalid entries
    NV_ASSERT_OK_OR_RETURN(memdescCreate(&pKernelGmmu->pWarSmallPageTable, pGpu,
                                         mmuFmtLevelSize(pSmallPT),
                                         RM_PAGE_SIZE, NV_TRUE,
                                         kgmmuGetPTEAperture(pKernelGmmu),
                                         kgmmuGetPTEAttr(pKernelGmmu), 0));

    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_WAR_PT, 
                    pKernelGmmu->pWarSmallPageTable);
    NV_ASSERT_OK_OR_GOTO(status, status, failed);

    NV_ASSERT_OK_OR_GOTO(status,
        memmgrMemDescMemSet(pMemoryManager, pKernelGmmu->pWarSmallPageTable, 0, TRANSFER_FLAGS_NONE),
        failed);

    // The WAR PDE0 points to the small page table allocated above
    {
        const GMMU_APERTURE aperture = kgmmuGetMemAperture(pKernelGmmu, pKernelGmmu->pWarSmallPageTable);

        nvFieldSetBool(&pPde0Fmt->fldVolatile,
                       memdescGetVolatility(pKernelGmmu->pWarSmallPageTable),
                       pFam->bug2720120WarPde0.v8);
        gmmuFieldSetAperture(&pPde0Fmt->fldAperture, aperture,
                             pFam->bug2720120WarPde0.v8);
        gmmuFieldSetAddress(gmmuFmtPdePhysAddrFld(pPde0Fmt, aperture),
                             kgmmuEncodePhysAddr(pKernelGmmu, aperture,
                                     memdescGetPhysAddr(pKernelGmmu->pWarSmallPageTable,
                                                        AT_GPU, 0),
                                     NVLINK_INVALID_FABRIC_ADDR),
                             pFam->bug2720120WarPde0.v8);
    }

    //
    // Bug 2720120: Allocate a PD0 instance all of whose entries point to
    // the small page table allocated above
    //
    NV_ASSERT_OK_OR_GOTO(status, memdescCreate(&pKernelGmmu->pWarPageDirectory0,
                                               pGpu, mmuFmtLevelSize(pPageDir0),
                                               RM_PAGE_SIZE, NV_TRUE,
                                               kgmmuGetPTEAperture(pKernelGmmu),
                                               kgmmuGetPTEAttr(pKernelGmmu), 0), failed);

    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_WAR_PD, 
                    pKernelGmmu->pWarPageDirectory0);
    NV_ASSERT_OK_OR_GOTO(status, status, failed);

    entryIndexHi = mmuFmtLevelEntryCount(pPageDir0) - 1;
    for (entryIndex = 0; entryIndex <= entryIndexHi; entryIndex++)
    {
        TRANSFER_SURFACE pageDirEntry =
            { .pMemDesc = pKernelGmmu->pWarPageDirectory0, .offset = entryIndex * pPageDir0->entrySize };

        NV_ASSERT_OK_OR_GOTO(status, memmgrMemWrite(pMemoryManager, &pageDirEntry, pFam->bug2720120WarPde0.v8,
                                                    pPageDir0->entrySize, TRANSFER_FLAGS_NONE),
            failed);
    }

    // The WAR PDE1 points to the PD0 instance allocated above
    {
        const GMMU_APERTURE aperture = kgmmuGetMemAperture(pKernelGmmu, pKernelGmmu->pWarPageDirectory0);

        nvFieldSetBool(&pPde1Fmt->fldVolatile,
                       memdescGetVolatility(pKernelGmmu->pWarPageDirectory0),
                       pFam->bug2720120WarPde1.v8);
        gmmuFieldSetAperture(&pPde1Fmt->fldAperture, aperture,
                             pFam->bug2720120WarPde1.v8);
        gmmuFieldSetAddress(gmmuFmtPdePhysAddrFld(pPde1Fmt, aperture),
                             kgmmuEncodePhysAddr(pKernelGmmu, aperture,
                                     memdescGetPhysAddr(pKernelGmmu->pWarPageDirectory0,
                                                        AT_GPU, 0),
                                     NVLINK_INVALID_FABRIC_ADDR),
                             pFam->bug2720120WarPde1.v8);
    }

failed:
    if (status != NV_OK)
    {
        if (pKernelGmmu->pWarSmallPageTable != NULL)
        {
            memdescFree(pKernelGmmu->pWarSmallPageTable);
            memdescDestroy(pKernelGmmu->pWarSmallPageTable);
            pKernelGmmu->pWarSmallPageTable = NULL;
        }
        if (pKernelGmmu->pWarPageDirectory0 != NULL)
        {
            memdescFree(pKernelGmmu->pWarPageDirectory0);
            memdescDestroy(pKernelGmmu->pWarPageDirectory0);
            pKernelGmmu->pWarPageDirectory0 = NULL;
        }
    }
    return status;
}

NV_STATUS
kgmmuSetupWarForBug2720120_GA100
(
    KernelGmmu      *pKernelGmmu
)
{
    NvU32 v;
    GMMU_FMT_FAMILY *pFam;

    if (pKernelGmmu->pWarPageDirectory0 != NULL)
        return NV_OK;

    for (v = 0; v < GMMU_FMT_MAX_VERSION_COUNT; ++v)
    {
        pFam = pKernelGmmu->pFmtFamilies[v];
        if (NULL != pFam)
        {
            if (kgmmuIsBug2720120WarEnabled(pKernelGmmu))
            {
                NV_ASSERT_OK_OR_RETURN(kgmmuSetupWarForBug2720120FmtFamily_GA100(pKernelGmmu, pFam));
            }
        }
    }

    return NV_OK;
}

/*!
 * @brief Get the engine ID associated with the max CE
 *
 * @param[in] pGpu         GPU object
 * @param[in] pKernelGmmu  KernelGmmu object
 *
 * return engine ID of the max CE
 */
NvU32
kgmmuGetMaxCeEngineId_GA100
(
    OBJGPU     *pGpu,
    KernelGmmu *pKernelGmmu
)
{
    return NV_PFAULT_MMU_ENG_ID_CE9;
}

/**
 * @brief   Determine if the fault is P2P unbound Instance Fault
 *
 * @param[in]     pKernelGmmu     KernelGmmu pointer
 * @param[in]     faultType       NvU32 identifier for Fault
 * @param[in]     faultClientId   NvU32 identifier for Client Id
 *
 * @returns  True, if it satisfies the following conditions
 *           -- FLA engine is not bound
             -- Destination side GFID mismatch in P2P/FLA incoming NvLink packets
 * False, otherwise
 */
NvBool
kgmmuIsP2PUnboundInstFault_GA100
(
    KernelGmmu *pKernelGmmu,
    NvU32       faultType,
    NvU32       faultClientId
)
{
    if ((faultType ==  NV_PFAULT_FAULT_TYPE_UNBOUND_INST_BLOCK) &&
       ((faultClientId >= NV_PFAULT_CLIENT_HUB_PTP_X0 && faultClientId <= NV_PFAULT_CLIENT_HUB_PTP_X7)))
    {
        return NV_TRUE;
    }

    return NV_FALSE;
}

/**
 * @brief handles an engine or PBDMA MMU fault
 *
 * "engine" is defined as an engine that is downstream of host (graphics, ce,
 * etc...).
 *
 * @param[in] pGpu                   OBJGPU pointer
 * @param[in] pKernelGmmu            KernelGmmu pointer
 * @param[in] pParsedFaultEntry      Parsed Fault entry
 * @param[in] pMmuExceptionData      FIFO exception packet used
 *                                   for printing fault info.
 *
 * @returns
 */
NV_STATUS
kgmmuServiceMmuFault_GA100
(
    OBJGPU                  *pGpu,
    KernelGmmu              *pKernelGmmu,
    NvP64                    pParsedFaultInfo,
    FIFO_MMU_EXCEPTION_DATA *pMmuExceptionData
)
{
    MMU_FAULT_BUFFER_ENTRY *pParsedFaultEntry = KERNEL_POINTER_FROM_NvP64(MMU_FAULT_BUFFER_ENTRY *, pParsedFaultInfo);

    //  If FLA fault do not reset channel
    if (pParsedFaultEntry->mmuFaultEngineId == NV_PFAULT_MMU_ENG_ID_FLA)
    {
        if (pKernelGmmu->bReportFlaTranslationXid)
        {
            nvErrorLog_va((void *)pGpu,
                DESTINATION_FLA_TRANSLATION_ERROR,
                "FLA Fault: inst:0x%x dev:0x%x subdev:0x%x, faulted @ 0x%x_%08x. Fault is of type %s %s",
                gpuGetInstance(pGpu),
                gpuGetDeviceInstance(pGpu),
                pGpu->subdeviceInstance,
                pMmuExceptionData->addrHi,
                pMmuExceptionData->addrLo,
                kgmmuGetFaultTypeString_HAL(pKernelGmmu, pMmuExceptionData->faultType),
                kfifoGetFaultAccessTypeString_HAL(pGpu, GPU_GET_KERNEL_FIFO(pGpu),
                    pMmuExceptionData->accessType));
        }

        return NV_OK;
    }
    else
    {
        return kgmmuServiceMmuFault_GV100(pGpu, pKernelGmmu, pParsedFaultInfo, pMmuExceptionData);
    }
}
