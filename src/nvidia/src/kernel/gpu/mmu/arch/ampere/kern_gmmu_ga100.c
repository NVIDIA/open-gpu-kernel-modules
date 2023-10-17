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

NV_STATUS
kgmmuSetupWarForBug2720120_GA100
(
    KernelGmmu      *pKernelGmmu,
    GMMU_FMT_FAMILY *pFam
)
{
    NV_STATUS            status      = NV_OK;
    OBJGPU              *pGpu        = ENG_GET_GPU(pKernelGmmu);
    KernelBus           *pKernelBus  = GPU_GET_KERNEL_BUS(pGpu);
    const GMMU_FMT      *pFmt        = kgmmuFmtGet(pKernelGmmu, GMMU_FMT_VERSION_DEFAULT, 0);
    const MMU_FMT_LEVEL *pPageDir1   = mmuFmtFindLevelWithPageShift(pFmt->pRoot, 29);
    const MMU_FMT_LEVEL *pPageDir0   = mmuFmtFindLevelWithPageShift(pFmt->pRoot, 21);
    const MMU_FMT_LEVEL *pSmallPT    = mmuFmtFindLevelWithPageShift(pFmt->pRoot, 12);
    const GMMU_FMT_PDE  *pPde0Fmt    = gmmuFmtGetPde(pFmt, pPageDir0, 1);
    const GMMU_FMT_PDE  *pPde1Fmt    = gmmuFmtGetPde(pFmt, pPageDir1, 0);
    NvU8                *pMap        = NULL;
    void                *pPriv       = NULL;
    NvU32                sizeOfDWord = sizeof(NvU32);
    RmPhysAddr           physAddr;
    RmPhysAddr           physAddrOrig;
    NvU64                sizeInDWord;
    NvU32                bar0Addr;
    NvU32                entryIndex;
    NvU32                entryIndexHi;
    NvU32                entryOffset;

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

    switch (memdescGetAddressSpace(pKernelGmmu->pWarSmallPageTable))
    {
        case ADDR_FBMEM:
            memUtilsMemSetNoBAR2(pGpu, pKernelGmmu->pWarSmallPageTable, 0);
            break;

        case ADDR_SYSMEM:
            // Plain old memmap.
            NV_ASSERT_OK_OR_GOTO(status, memdescMapOld(pKernelGmmu->pWarSmallPageTable, 0,
                                                       pKernelGmmu->pWarSmallPageTable->Size,
                                                       NV_TRUE, // kernel,
                                                       NV_PROTECT_READ_WRITE,
                                                       (void **)&pMap,
                                                       &pPriv), failed);

            portMemSet(pMap, 0, pKernelGmmu->pWarSmallPageTable->Size);

            memdescUnmapOld(pKernelGmmu->pWarSmallPageTable, 1, 0, pMap, pPriv);
            break;

        default:
            // Should not happen.
            status = NV_ERR_INVALID_ARGUMENT;
            NV_ASSERT_OR_GOTO(status == NV_OK, failed);
            break;
    }

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
    switch (memdescGetAddressSpace(pKernelGmmu->pWarPageDirectory0))
    {
        case ADDR_FBMEM:
            //
            // Set the BAR0 window to encompass the given surface while
            // saving off the location to where the BAR0 window was
            // previously pointing.
            //
            physAddr = memdescGetPhysAddr(pKernelGmmu->pWarPageDirectory0, AT_GPU, 0);
            NV_ASSERT_OR_GOTO(NV_IS_ALIGNED64(physAddr, sizeOfDWord), failed);

            physAddrOrig = kbusGetBAR0WindowVidOffset_HAL(pGpu, pKernelBus);
            NV_ASSERT_OK_OR_GOTO(status,
                                 kbusSetBAR0WindowVidOffset_HAL(pGpu,
                                                                pKernelBus,
                                                                physAddr & ~0xffffULL),
                                 failed);

            bar0Addr = NvU64_LO32(kbusGetBAR0WindowAddress_HAL(pKernelBus) +
                          (physAddr - kbusGetBAR0WindowVidOffset_HAL(pGpu, pKernelBus)));

            //
            // Iterate and initialize the given surface with BAR0
            // writes.
            //
            sizeInDWord = (NvU32)NV_DIV_AND_CEIL(pPageDir0->entrySize, sizeOfDWord);
            for (entryIndex = 0; entryIndex <= entryIndexHi; entryIndex++)
            {
                entryOffset = entryIndex * pPageDir0->entrySize;
                NvU32 i;
                for (i = 0; i < sizeInDWord; i++)
                {
                    GPU_REG_WR32(pGpu,
                                 bar0Addr + entryOffset + (sizeOfDWord * i),
                                 pFam->bug2720120WarPde0.v32[i]);
                }
            }

            // Restore where the BAR0 window was previously pointing to
            NV_ASSERT_OK_OR_GOTO(status,
                                 kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus,
                                                                physAddrOrig),
                                 failed);

            break;

        case ADDR_SYSMEM:
            // Plain old memmap.
            NV_ASSERT_OK_OR_GOTO(status, memdescMapOld(pKernelGmmu->pWarPageDirectory0, 0,
                                                       pKernelGmmu->pWarPageDirectory0->Size,
                                                       NV_TRUE, // kernel,
                                                       NV_PROTECT_READ_WRITE,
                                                       (void **)&pMap,
                                                       &pPriv), failed);

            for (entryIndex = 0; entryIndex <= entryIndexHi; entryIndex++)
            {
                entryOffset = entryIndex * pPageDir0->entrySize;

                // Memory-mapped write.
                portMemCopy(pMap + entryOffset,
                            pPageDir0->entrySize,
                            pFam->bug2720120WarPde0.v8,
                            pPageDir0->entrySize);
            }

            memdescUnmapOld(pKernelGmmu->pWarPageDirectory0, 1, 0, pMap, pPriv);
            break;

        default:
            // Should not happen.
            status = NV_ERR_INVALID_ARGUMENT;
            NV_ASSERT_OR_GOTO(status == NV_OK, failed);
            break;
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
