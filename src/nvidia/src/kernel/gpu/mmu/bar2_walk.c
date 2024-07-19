/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "gpu/gpu.h"
#include "gpu/mmu/kern_gmmu.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/nvlink/kernel_nvlink.h"
#include "gpu/bus/kern_bus.h"
#include "mem_mgr/gpu_vaspace.h"
#include "mmu/mmu_walk.h"
#include "vgpu/vgpu_events.h"

/*!
 * @file
 * @brief struct MMU_WALK_CALLBACKS g_bar2WalkCallbacks and the callback
 *        function implementations.
 */

/*!
 * Implementation of @ref MmuWalkCBUpdatePde for BAR2
 */
static NvBool
_bar2WalkCBUpdatePdb
(
    MMU_WALK_USER_CTX       *pUserCtx,
    const MMU_FMT_LEVEL     *pRootFmt,
    const MMU_WALK_MEMDESC  *pRootMem,
    const NvBool             bIgnoreChannelBusy
)
{
    OBJGPU             *pGpu        = pUserCtx->pGpu;
    KernelBus          *pKernelBus  = GPU_GET_KERNEL_BUS(pGpu);
    KernelGmmu         *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    NV_STATUS           status      = NV_OK;
    NvU32               gfid;
    NvBool              bUseTempMemDesc;

    NV_ASSERT_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid) == NV_OK, NV_FALSE);

    bUseTempMemDesc = pKernelBus->bar2[gfid].bBootstrap &&
                      kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus) &&
                      (NULL != pKernelBus->bar2[gfid].pPDEMemDescForBootstrap);

    if (NULL == pRootMem)
    {
        //
        // Ignoring uncommits for now since kbusInitInstBlk_HAL can't handle
        // NULL memdesc and it doesn't matter functionally.
        //
        goto done;
    }

    switch (pKernelBus->InstBlkAperture)
    {
        // BAR2 will use the default big page size chosen by the system.
        default:
        case ADDR_FBMEM:
            if (pKernelBus->bar2[gfid].bBootstrap)
            {
                status = kbusInitInstBlk_HAL(pGpu, pKernelBus,
                                             NULL /* use BAR0 window */,
                                             (bUseTempMemDesc ?
                                             pKernelBus->bar2[gfid].pPDEMemDescForBootstrap :
                                             pKernelBus->bar2[gfid].pPDEMemDesc),
                                             pKernelBus->bar2[gfid].vaLimit,
                                             kgmmuGetBigPageSize_HAL(pKernelGmmu), NULL);
            }
            else
            {
                status = kbusInitInstBlk_HAL(pGpu, pKernelBus,
                                             pKernelBus->bar2[gfid].pInstBlkMemDesc,
                                             pKernelBus->bar2[gfid].pPDEMemDesc,
                                             pKernelBus->bar2[gfid].vaLimit,
                                             kgmmuGetBigPageSize_HAL(pKernelGmmu), NULL);
            }
            NV_ASSERT_OR_GOTO(NV_OK == status, done);
            break;
        case ADDR_SYSMEM:
            status = kbusInitInstBlk_HAL(pGpu, pKernelBus,
                                        pKernelBus->bar2[gfid].pInstBlkMemDesc,
                                        (bUseTempMemDesc ?
                                        pKernelBus->bar2[gfid].pPDEMemDescForBootstrap :
                                        pKernelBus->bar2[gfid].pPDEMemDesc),
                                        pKernelBus->bar2[gfid].vaLimit,
                                        kgmmuGetBigPageSize_HAL(pKernelGmmu), NULL);
            NV_ASSERT_OR_GOTO(NV_OK == status, done);
            break;
    }

done:
    return NV_OK == status;
}

/*!
 * Implementation of @ref MmuWalkCBUpdatePde for BAR2
 */
static void
_bar2WalkCBFillEntries
(
    MMU_WALK_USER_CTX         *pUserCtx,
    const MMU_FMT_LEVEL       *pLevelFmt,
    const MMU_WALK_MEMDESC    *pLevelMem,
    const NvU32                entryIndexLo,
    const NvU32                entryIndexHi,
    const MMU_WALK_FILL_STATE  fillState,
    NvU32                     *pProgress
)
{
    OBJGPU                *pGpu         = pUserCtx->pGpu;
    KernelBus             *pKernelBus   = GPU_GET_KERNEL_BUS(pGpu);
    KernelGmmu            *pKernelGmmu  = GPU_GET_KERNEL_GMMU(pGpu);
    NvU32                  gfid         = pUserCtx->gfid;
    const GMMU_FMT        *pFmt         = NULL;
    const GMMU_FMT_FAMILY *pFam         = NULL;
    MEMORY_DESCRIPTOR     *pMemDesc     = (MEMORY_DESCRIPTOR*)pLevelMem;
    NvU8                  *pMap         = NULL;
    void                  *pPriv        = NULL;
    NV_STATUS              status       = NV_OK;
    GMMU_ENTRY_VALUE       entryValue;
    ADDRESS_TRANSLATION    addressTranslation = AT_GPU;
    NvU32                  sizeInDWord  = (NvU32)NV_CEIL(pLevelFmt->entrySize, sizeof(NvU32));
    NvU32                  entryIndex;
    NvU32                  entryOffset;
    NvU64                  entryStart;
    NvU32                  i;

    pFmt = pKernelBus->bar2[gfid].pFmt;
    pFam = kgmmuFmtGetFamily(pKernelGmmu, pFmt->version);

    // Determine what entry value to write.
    switch (fillState)
    {
        case MMU_WALK_FILL_INVALID:
            portMemSet(&entryValue, 0, sizeof(entryValue));
            break;
        case MMU_WALK_FILL_SPARSE:
        {
            // Fake sparse entry is needed for GH100 in CC mode for PDE2-PDE4. Ref: Bug 3341692
            NvU8 *pFakeSparse = kgmmuGetFakeSparseEntry_HAL(pGpu, pKernelGmmu, pLevelFmt);

            if (pFakeSparse != NULL)
            {
                portMemCopy(entryValue.v8, pLevelFmt->entrySize, pFakeSparse, pLevelFmt->entrySize);
                break;
            }
            if (pLevelFmt->numSubLevels > 0)
            {
                // Select sparse entry template based on number of sub-levels.
                if (pLevelFmt->numSubLevels > 1)
                {
                    entryValue = pFam->sparsePdeMulti;
                }
                else
                {
                    NV_ASSERT(pLevelFmt->numSubLevels == 1);
                    entryValue = pFam->sparsePde;
                }
            }
            else
            {
                entryValue = pFam->sparsePte;
            }
            break;
        }
        // case MMU_WALK_FILL_NV4K not supported on bar2 gmmu
        default:
            NV_ASSERT(0);
    }

    // Determine how to write the entry value.
    if (memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM)
    {
        if (kbusIsBarAccessBlocked(pKernelBus))
        {
            MemoryManager   *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
            TRANSFER_SURFACE surf           = {0};
            NvU32            sizeOfEntries;

            NV_ASSERT_OR_RETURN_VOID(pKernelBus->virtualBar2[gfid].pPageLevels == NULL);

            surf.pMemDesc = pMemDesc;
            surf.offset = entryIndexLo * pLevelFmt->entrySize;

            sizeOfEntries = (entryIndexHi - entryIndexLo + 1) * pLevelFmt->entrySize;

            pMap = memmgrMemBeginTransfer(pMemoryManager, &surf, sizeOfEntries,
                                          TRANSFER_FLAGS_SHADOW_ALLOC);

            for (entryIndex = entryIndexLo; entryIndex <= entryIndexHi; entryIndex++)
            {
                NvU32 index = (entryIndex - entryIndexLo) * pLevelFmt->entrySize;
                portMemCopy(&pMap[index], pLevelFmt->entrySize,
                            entryValue.v8, pLevelFmt->entrySize);
            }

            memmgrMemEndTransfer(pMemoryManager, &surf, sizeOfEntries,
                                 TRANSFER_FLAGS_SHADOW_ALLOC);
        }
        else if (pKernelBus->bar2[gfid].bBootstrap)
        {
            if (kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus))
            {
                pMap = kbusCpuOffsetInBar2WindowGet(pGpu, pKernelBus, pMemDesc);

                for (entryIndex = entryIndexLo; entryIndex <= entryIndexHi; entryIndex++ )
                {
                    entryOffset = entryIndex * pLevelFmt->entrySize;

                    // Memory write via BAR2's CPU mapping.
                    portMemCopy(pMap + entryOffset,
                                pLevelFmt->entrySize,
                                entryValue.v8,
                                pLevelFmt->entrySize);
                }
            }
            else
            {
                //
                // No CPU mapping to the BAR2 VAS page levels is available yet.
                // Must use the BAR0 window to directly write to the physical
                // addresses where the BAR2 VAS page levels are located in FB.
                //
                NV_ASSERT_OR_RETURN_VOID(pKernelBus->virtualBar2[gfid].pPageLevels == NULL);

                for ( entryIndex = entryIndexLo; entryIndex <= entryIndexHi; entryIndex++ )
                {
                    entryOffset = entryIndex * pLevelFmt->entrySize;
                    entryStart = memdescGetPhysAddr(pMemDesc, FORCE_VMMU_TRANSLATION(pMemDesc, addressTranslation), entryOffset);
                    for (i = 0; i < sizeInDWord; i++)
                    {
                        // BAR0 write.
                        status = kbusMemAccessBar0Window_HAL(pGpu, pKernelBus,
                                              (entryStart + (sizeof(NvU32) * i)),
                                              &entryValue.v32[i],
                                              sizeof(NvU32),
                                              NV_FALSE,
                                              ADDR_FBMEM);
                        NV_ASSERT_OR_RETURN_VOID(NV_OK == status);
                    }
                }
            }
        }
        else
        {
            //
            // Determine the start of the desired page level offsetted from
            // the CPU mapping to the start of the BAR2 VAS page levels.
            //

            NV_ASSERT_OR_RETURN_VOID(pKernelBus->virtualBar2[gfid].pPageLevels != NULL);

            pMap = memdescGetPhysAddr(pMemDesc, addressTranslation, 0) -
                                  pKernelBus->bar2[gfid].pdeBase +
                                  pKernelBus->virtualBar2[gfid].pPageLevels;

            for ( entryIndex = entryIndexLo; entryIndex <= entryIndexHi; entryIndex++ )
            {
                entryOffset = entryIndex * pLevelFmt->entrySize;

                // Memory write via BAR2.
                portMemCopy(pMap + entryOffset,
                            pLevelFmt->entrySize,
                            entryValue.v8,
                            pLevelFmt->entrySize);
            }
        }
    }
    else
    {
        NV_ASSERT(memdescGetAddressSpace(pMemDesc) == ADDR_SYSMEM);

        // Plain old memmap.
        status = memdescMapOld(pMemDesc, 0,
                               pMemDesc->Size,
                               NV_TRUE, // kernel,
                               NV_PROTECT_READ_WRITE,
                               (void **)&pMap,
                               &pPriv);
        NV_ASSERT_OR_RETURN_VOID(NV_OK == status);

        for ( entryIndex = entryIndexLo; entryIndex <= entryIndexHi; entryIndex++ )
        {
            entryOffset = entryIndex * pLevelFmt->entrySize;

            // Memory-mapped write.
            portMemCopy(pMap + entryOffset,
                        pLevelFmt->entrySize,
                        entryValue.v8,
                        pLevelFmt->entrySize);
        }

        memdescUnmapOld(pMemDesc, 1, 0, pMap, pPriv);
    }

    *pProgress = entryIndexHi - entryIndexLo + 1;
}

/*!
 * Implementation of @ref MmuWalkCBUpdatePde for BAR2
 */
static NvBool
_bar2WalkCBUpdatePde
(
    MMU_WALK_USER_CTX       *pUserCtx,
    const MMU_FMT_LEVEL     *pLevelFmt,
    const MMU_WALK_MEMDESC  *pLevelMem,
    const NvU32              entryIndex,
    const MMU_WALK_MEMDESC **pSubLevels
)
{
    OBJGPU             *pGpu        = pUserCtx->pGpu;
    KernelGmmu         *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    KernelBus          *pKernelBus  = GPU_GET_KERNEL_BUS(pGpu);
    NvU32               gfid;
    const GMMU_FMT     *pFmt;
    MEMORY_DESCRIPTOR  *pMemDesc    = (MEMORY_DESCRIPTOR*)pLevelMem;
    NvU8               *pMap        = NULL;
    void               *pPriv       = NULL;
    NV_STATUS           status      = NV_OK;
    GMMU_ENTRY_VALUE    entry;
    NvU32               i;
    NvU32               sizeInDWord;
    NvU32               entryOffset;
    NvU64               entryStart;

    NV_ASSERT_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid) == NV_OK, NV_FALSE);

    pFmt = pKernelBus->bar2[gfid].pFmt;

    // Clear out the temp copy of the PDE
    portMemSet(entry.v8, 0, pLevelFmt->entrySize);

    for (i = 0; i < pLevelFmt->numSubLevels; ++i)
    {
        const GMMU_FMT_PDE *pPde        = gmmuFmtGetPde(pFmt, pLevelFmt, i);
        MEMORY_DESCRIPTOR  *pSubMemDesc = (MEMORY_DESCRIPTOR*)pSubLevels[i];

        if (NULL != pSubMemDesc)
        {
            const GMMU_APERTURE       aperture = kgmmuGetMemAperture(pKernelGmmu, pSubMemDesc);
            const GMMU_FIELD_ADDRESS *pFldAddr = gmmuFmtPdePhysAddrFld(pPde, aperture);
            const NvU64               physAddr = memdescGetPhysAddr(pSubMemDesc, AT_GPU, 0);

            // Set fields within the temp PDE
            if (pFmt->version == GMMU_FMT_VERSION_3)
            {
                NvU32 pdePcfHw  = 0;
                NvU32 pdePcfSw  = 0;

                pdePcfSw |= memdescGetVolatility(pSubMemDesc) ? (1 << SW_MMU_PCF_UNCACHED_IDX) : 0;
                NV_ASSERT_OR_RETURN((kgmmuTranslatePdePcfFromSw_HAL(pKernelGmmu, pdePcfSw, &pdePcfHw) == NV_OK),
                                      NV_ERR_INVALID_ARGUMENT);
                nvFieldSet32(&pPde->fldPdePcf, pdePcfHw, entry.v8);
            }
            else
            {
                nvFieldSetBool(&pPde->fldVolatile, memdescGetVolatility(pSubMemDesc), entry.v8);
            }
            gmmuFieldSetAperture(&pPde->fldAperture, aperture, entry.v8);
            gmmuFieldSetAddress(pFldAddr,
                kgmmuEncodePhysAddr(pKernelGmmu, aperture, physAddr,
                    NVLINK_INVALID_FABRIC_ADDR),
                entry.v8);

            NV_PRINTF(LEVEL_INFO, "    SubLevel %u = PA 0x%llX\n", i,
                      physAddr);
        }
        else
        {
            NV_PRINTF(LEVEL_INFO, "    SubLevel %u = INVALID\n", i);
        }
    }

    entryOffset  = entryIndex * pLevelFmt->entrySize;

    if (pKernelBus->PDEBAR2Aperture == ADDR_FBMEM)
    {
        if (kbusIsBarAccessBlocked(pKernelBus))
        {
            MemoryManager   *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
            TRANSFER_SURFACE surf           = {0};

            NV_ASSERT_OR_RETURN(pKernelBus->virtualBar2[gfid].pPageLevels == NULL,
                                NV_FALSE);

            surf.pMemDesc = pMemDesc;
            surf.offset = entryOffset;

            NV_ASSERT_OR_RETURN(memmgrMemWrite(pMemoryManager, &surf,
                                               entry.v8, pLevelFmt->entrySize,
                                               TRANSFER_FLAGS_NONE) ==  NV_OK,
                                NV_FALSE);
        }
        // If we are setting up BAR2, we need special handling.
        else if (pKernelBus->bar2[gfid].bBootstrap)
        {
            if (kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus))
            {
                pMap = kbusCpuOffsetInBar2WindowGet(pGpu, pKernelBus, pMemDesc);
                portMemCopy(pMap + entryOffset, pLevelFmt->entrySize, entry.v8, pLevelFmt->entrySize);
            }
            else
            {
                entryStart  = memdescGetPhysAddr(pMemDesc, AT_PA, entryOffset);
                sizeInDWord = (NvU32)NV_CEIL(pLevelFmt->entrySize, sizeof(NvU32));

                for (i = 0; i < sizeInDWord; i++)
                {
                    status = kbusMemAccessBar0Window_HAL(pGpu, pKernelBus,
                                          (entryStart + (sizeof(NvU32) * i)),
                                          &entry.v32[i],
                                          sizeof(NvU32),
                                          NV_FALSE,
                                          ADDR_FBMEM);
                    NV_ASSERT_OR_RETURN(NV_OK == status, NV_FALSE);
                }
            }
        }
        else if (pKernelBus->bar2[gfid].bMigrating || IS_GFID_VF(gfid) ||
                 KBUS_BAR0_PRAMIN_DISABLED(pGpu))
        {
            NV_ASSERT(NULL != pKernelBus->virtualBar2[gfid].pPageLevels);

            pMap = memdescGetPhysAddr(pMemDesc, AT_GPU, 0) -
                                  pKernelBus->bar2[gfid].pdeBase +
                                  pKernelBus->virtualBar2[gfid].pPageLevels;
            NV_ASSERT(NULL != pMap);
            portMemCopy(pMap + entryOffset, pLevelFmt->entrySize, entry.v8, pLevelFmt->entrySize);
        }
        else
        {
            NV_ASSERT_OR_RETURN(0, NV_FALSE); // Not yet supported.
        }
    }
    else if (pKernelBus->PDEBAR2Aperture == ADDR_SYSMEM)
    {
        // Plain old memmap.
        status = memdescMapOld(pMemDesc, 0,
                               pMemDesc->Size,
                               NV_TRUE, // kernel,
                               NV_PROTECT_READ_WRITE,
                               (void **)&pMap,
                               &pPriv);
        NV_ASSERT_OR_RETURN(NV_OK == status, NV_FALSE);
        portMemCopy(pMap + entryOffset, pLevelFmt->entrySize, entry.v8, pLevelFmt->entrySize);
        memdescUnmapOld(pMemDesc, 1, 0, pMap, pPriv);
    }
    else
    {
        NV_ASSERT_OR_RETURN(0, NV_FALSE); // only SYSMEM and FBMEM are supported.
    }

    return NV_TRUE;
}

/*!
 * Implementation of @ref MmuWalkCBLevelFree for BAR2
 */
static void
_bar2WalkCBLevelFree
(
    MMU_WALK_USER_CTX   *pUserCtx,
    const MMU_FMT_LEVEL *pLevelFmt,
    const NvU64          vaBase,
    MMU_WALK_MEMDESC    *pOldMem
)
{
    MEMORY_DESCRIPTOR *pMemDesc = (MEMORY_DESCRIPTOR*)pOldMem;

    NV_PRINTF(LEVEL_INFO, "PA 0x%llX for VA 0x%llX-0x%llX\n",
              memdescGetPhysAddr(pMemDesc, AT_GPU, 0),
              mmuFmtLevelVirtAddrLo(pLevelFmt, vaBase),
              mmuFmtLevelVirtAddrHi(pLevelFmt, vaBase));

    memdescFree(pMemDesc);
    memdescDestroy(pMemDesc);
}

/*!
 * Implementation of @ref MmuWalkCBLevelAlloc for BAR2
 */
static NV_STATUS
_bar2WalkCBLevelAlloc
(
    MMU_WALK_USER_CTX       *pUserCtx,
    const MMU_FMT_LEVEL     *pLevelFmt,
    const NvU64              vaBase,
    const NvU64              vaLimit,
    const NvBool             bTarget,
    MMU_WALK_MEMDESC       **ppMemDesc,
    NvU32                   *pMemSize,
    NvBool                  *pBChanged
)
{
    OBJGPU             *pGpu    = pUserCtx->pGpu;
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    KernelBus          *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    NvU32               gfid;
    NvU64               pdeBase = 0;
    NvU64               pteBase = 0;
    NvU32               allocSize;
    NvU32               memOffset;
    MEMORY_DESCRIPTOR   *pMemDesc = NULL;

    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
        kmemsysGetStaticConfig(pGpu, pKernelMemorySystem);
    NvBool bPreFillCache = gpuIsCacheOnlyModeEnabled(pGpu) &&
                                      !pMemorySystemConfig->bL2PreFill;
    NV_STATUS         status = NV_OK;

    NV_ASSERT_OK_OR_RETURN(vgpuGetCallingContextGfid(pGpu, &gfid));

    // Abort early if level is not targeted or already allocated.
    if (!bTarget || (NULL != *ppMemDesc))
    {
        return NV_OK;
    }

    // Specify which Page Level we are initializing.
    if (pKernelBus->bar2[gfid].bBootstrap || pKernelBus->bar2[gfid].bMigrating ||
        IS_GFID_VF(gfid) || KBUS_BAR0_PRAMIN_DISABLED(pGpu) ||
        kbusIsBarAccessBlocked(pKernelBus))
    {
        if (pLevelFmt == pKernelBus->bar2[gfid].pFmt->pRoot)
        {
            pKernelBus->bar2[gfid].pageDirInit = 0;
            pKernelBus->bar2[gfid].pageTblInit = 0;
        }

        NV_ASSERT_OR_RETURN(pKernelBus->bar2[gfid].pageDirInit + pKernelBus->bar2[gfid].pageTblInit <
                            pKernelBus->bar2[gfid].numPageDirs + pKernelBus->bar2[gfid].numPageTbls,
                            NV_ERR_INVALID_STATE);
    }

    if (kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus) &&
        (ADDR_FBMEM == pKernelBus->PDEBAR2Aperture))
    {
        if (pKernelBus->bar2[gfid].bBootstrap)
        {
            pdeBase = pKernelBus->bar2[gfid].pdeBaseForBootstrap;
            pteBase = pKernelBus->bar2[gfid].pteBaseForBootstrap;
        }
        else if (pKernelBus->bar2[gfid].bMigrating)
        {
            pdeBase = pKernelBus->bar2[gfid].pdeBase;
            pteBase = pKernelBus->bar2[gfid].pteBase;
        }
       else
        {
            status = NV_ERR_INVALID_OPERATION;
            NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);
        }
    }
    else
    {
            NV_ASSERT(pKernelBus->bar2[gfid].bBootstrap || IS_GFID_VF(gfid) ||
                      KBUS_BAR0_PRAMIN_DISABLED(pGpu) ||
                      kbusIsCpuVisibleBar2Disabled(pKernelBus) ||
                      kbusIsBarAccessBlocked(pKernelBus));
            pdeBase = pKernelBus->bar2[gfid].pdeBase;
            pteBase = pKernelBus->bar2[gfid].pteBase;
    }

    // Process Page Dirs
    if (0 != pLevelFmt->numSubLevels)
    {
        allocSize = pKernelBus->bar2[gfid].pageDirSize;
        status = memdescCreate(&pMemDesc, pGpu,
                               allocSize,
                               RM_PAGE_SIZE,
                               NV_TRUE,
                               pKernelBus->PDEBAR2Aperture,
                               pKernelBus->PDEBAR2Attr,
                               MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE);
        NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);

        switch (pKernelBus->PDEBAR2Aperture)
        {
            default:
            case ADDR_FBMEM:
                //
                // Reserved FB memory for BAR2 Page Levels is contiiguous, hence
                // we simply offset from page dir base.
                //
                memOffset = pKernelBus->bar2[gfid].pageDirInit * pKernelBus->bar2[gfid].pageDirSize;
                memdescDescribe(pMemDesc,
                                pKernelBus->PDEBAR2Aperture,
                                pdeBase + memOffset,
                                allocSize);

                //
                // Pre-fill cache to prevent FB read accesses if in cache only
                // mode and not doing one time pre-fill.
                //
                if (bPreFillCache)
                {
                    kmemsysPreFillCacheOnlyMemory_HAL(pGpu, pKernelMemorySystem,
                                                      pdeBase + memOffset,
                                                      allocSize);
                }
                break;

            case ADDR_SYSMEM:
                memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_142, 
                                pMemDesc);
                NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);
                break;
        }

        if (pLevelFmt == pKernelBus->bar2[gfid].pFmt->pRoot)
        {
            if (kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus) &&
                (ADDR_FBMEM == pKernelBus->PDEBAR2Aperture))
            {
                if (pKernelBus->bar2[gfid].bBootstrap)
                {
                    // Cache the temporary root Page Dir setup at bottom of FB.
                    pKernelBus->bar2[gfid].pdeBaseForBootstrap = memdescGetPhysAddr(pMemDesc,
                                                              AT_GPU, 0);
                    pKernelBus->bar2[gfid].pPDEMemDescForBootstrap = pMemDesc;
                }
                else if (pKernelBus->bar2[gfid].bMigrating)
                {
                    //
                    // Cache the root Page Dir setup at top of FB.
                    //
                    pKernelBus->bar2[gfid].pdeBase = memdescGetPhysAddr(pMemDesc,
                                                        AT_GPU, 0);
                    pKernelBus->bar2[gfid].pPDEMemDesc = pMemDesc;
                }
                else
                {
                    status = NV_ERR_INVALID_OPERATION;
                    NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);
                }
            }
            else
            {
                NV_ASSERT(pKernelBus->bar2[gfid].bBootstrap || IS_GFID_VF(gfid) ||
                          KBUS_BAR0_PRAMIN_DISABLED(pGpu) ||
                          kbusIsCpuVisibleBar2Disabled(pKernelBus) ||
                          kbusIsBarAccessBlocked(pKernelBus));
                pKernelBus->bar2[gfid].pdeBase = memdescGetPhysAddr(pMemDesc, AT_GPU, 0);
                pKernelBus->bar2[gfid].pPDEMemDesc = pMemDesc;
            }
        }
        if (pKernelBus->bar2[gfid].bBootstrap || pKernelBus->bar2[gfid].bMigrating ||
            IS_GFID_VF(gfid) || KBUS_BAR0_PRAMIN_DISABLED(pGpu) ||
            kbusIsBarAccessBlocked(pKernelBus))
        {
            pKernelBus->bar2[gfid].pageDirInit++;
        }
    }
    else // Alloc Page Table
    {
        allocSize = pKernelBus->bar2[gfid].pageTblSize;
        status = memdescCreate(&pMemDesc, pGpu,
                               allocSize,
                               RM_PAGE_SIZE,
                               NV_TRUE,
                               pKernelBus->PTEBAR2Aperture,
                               pKernelBus->PTEBAR2Attr,
                               MEMDESC_FLAGS_OWNED_BY_CURRENT_DEVICE);
        NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);

        switch (pKernelBus->PTEBAR2Aperture)
        {
            default:
            case ADDR_FBMEM:
                //
                // Reserved FB memory for BAR2 Page Levels is contiiguous, hence
                // we simply offset from the page table base.
                // pageTblInit gives us the page table number we are
                // initializing.
                //
                memOffset = pKernelBus->bar2[gfid].pageTblInit * allocSize;
                memdescDescribe(pMemDesc,
                                pKernelBus->PTEBAR2Aperture,
                                pteBase + memOffset,
                                allocSize);

                //
                // Pre-fill cache to prevent FB read accesses if in cache only mode
                // and not doing one time pre-fill
                //
                if (bPreFillCache)
                {
                    kmemsysPreFillCacheOnlyMemory_HAL(pGpu, pKernelMemorySystem,
                                                      pteBase + memOffset,
                                                      allocSize);
                }
                break;

            case ADDR_SYSMEM:
                memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_143, 
                                pMemDesc);
                NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);
                break;
        }

        if (pKernelBus->bar2[gfid].pageTblInit == 0)
        {
            if (kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus) &&
                (ADDR_FBMEM == pKernelBus->PTEBAR2Aperture))
            {
                // Cache the first Page Table memdesc
                if (pKernelBus->bar2[gfid].bBootstrap)
                {
                    pKernelBus->bar2[gfid].pteBaseForBootstrap = memdescGetPhysAddr(pMemDesc,
                                                                                    AT_GPU, 0);
                }
                else if (pKernelBus->bar2[gfid].bMigrating)
                {
                    pKernelBus->bar2[gfid].pteBase = memdescGetPhysAddr(pMemDesc,
                                                                        AT_GPU, 0);
                }
                else
                {
                    status = NV_ERR_INVALID_OPERATION;
                    NV_ASSERT_OR_GOTO(NV_OK == status, cleanup);
                }
            }
            else
            {
                NV_ASSERT(pKernelBus->bar2[gfid].bBootstrap || IS_GFID_VF(gfid) ||
                          KBUS_BAR0_PRAMIN_DISABLED(pGpu) ||
                          kbusIsCpuVisibleBar2Disabled(pKernelBus) ||
                          kbusIsBarAccessBlocked(pKernelBus));
                pKernelBus->bar2[gfid].pteBase = memdescGetPhysAddr(pMemDesc,
                                                                    AT_GPU, 0);
            }
            pKernelBus->virtualBar2[gfid].pPTEMemDesc = pMemDesc;
        }
        if (pKernelBus->bar2[gfid].bBootstrap || pKernelBus->bar2[gfid].bMigrating ||
            IS_GFID_VF(gfid) || KBUS_BAR0_PRAMIN_DISABLED(pGpu) ||
            kbusIsBarAccessBlocked(pKernelBus))
        {
            pKernelBus->bar2[gfid].pageTblInit++;
        }
    }

    // Return the allocated memdesc
    *ppMemDesc = (MMU_WALK_MEMDESC*)pMemDesc;
    *pMemSize  = allocSize;
    *pBChanged = NV_TRUE;

cleanup:
    if (NV_OK != status)
    {
        memdescFree(pMemDesc);
        memdescDestroy(pMemDesc);
    }
    return status;
}

/*!
 * Implementation of @ref MmuWalkCBWriteBuffer for BAR2
 */
static void
_bar2WalkCBWriteBuffer
(
     MMU_WALK_USER_CTX    *pUserCtx,
     MMU_WALK_MEMDESC     *pStagingBuffer,
     MMU_WALK_MEMDESC     *pLevelBuffer,
     NvU64                 entryIndexLo,
     NvU64                 entryIndexHi,
     NvU64                 tableSize,
     NvU64                 entrySize
)
{
    OBJGPU            *pGpu               = pUserCtx->pGpu;
    KernelBus         *pKernelBus         = GPU_GET_KERNEL_BUS(pGpu);
    NvU32              gfid;
    MEMORY_DESCRIPTOR *pStagingBufferDesc = (MEMORY_DESCRIPTOR*) pStagingBuffer;
    MEMORY_DESCRIPTOR *pOutputBufferDesc  = (MEMORY_DESCRIPTOR*) pLevelBuffer;
    NvBool             bRestore           = NV_FALSE;
    NvU64              firstEntryOffset   = entryIndexLo * entrySize;
    NvU64              entryRangeSize     = (entryIndexHi - entryIndexLo + 1llu) * (entrySize);
    NvU64              oldBar0Mapping     = 0;
    NvU8              *pStagingBufferMapping;
    NvU8              *pStagingDescMapping;
    NvU8              *pOutputBufferMapping;
    void              *pPriv;

    NV_ASSERT_OR_RETURN_VOID(vgpuGetCallingContextGfid(pGpu, &gfid) == NV_OK);

    // TODO: Stash this mapping somewhere permanent to avoid constant remapping
    NV_ASSERT_OR_RETURN_VOID(
        memdescMapOld(pStagingBufferDesc,
                      0,
                      pStagingBufferDesc->Size,
                      NV_TRUE, // kernel,
                      NV_PROTECT_READ_WRITE,
                      (void **)&pStagingDescMapping,
                      &pPriv)
        == NV_OK);

    pStagingBufferMapping = &pStagingDescMapping[firstEntryOffset % tableSize];

    if (kbusIsBarAccessBlocked(pKernelBus))
    {
        MemoryManager   *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
        TRANSFER_SURFACE surf           = {0};

        NV_ASSERT_OR_RETURN_VOID(pKernelBus->virtualBar2[gfid].pPageLevels == NULL);

        surf.pMemDesc = pOutputBufferDesc;
        surf.offset = firstEntryOffset;

        pOutputBufferMapping = memmgrMemBeginTransfer(pMemoryManager, &surf,
                                                      entryRangeSize,
                                                      TRANSFER_FLAGS_SHADOW_ALLOC);

        portMemCopy(pOutputBufferMapping, entryRangeSize,
                    pStagingBufferMapping, entryRangeSize);

        memmgrMemEndTransfer(pMemoryManager, &surf, entryRangeSize,
                             TRANSFER_FLAGS_SHADOW_ALLOC);

        goto unmap_and_exit;
    }
    else if (pKernelBus->bar2[gfid].bBootstrap)
    {
        if (kbusIsPhysicalBar2InitPagetableEnabled(pKernelBus))
        {
            // BAR2 in physical mode, using top of FB
            NvU8 *pOutputDescMapping = kbusCpuOffsetInBar2WindowGet(pGpu, pKernelBus, pOutputBufferDesc);
            pOutputBufferMapping = &pOutputDescMapping[firstEntryOffset];
        }
        else
        {
            // Get the physical address of the memdesc
            NvU64 phys = memdescGetPhysAddr(pOutputBufferDesc,
                                            FORCE_VMMU_TRANSLATION(pOutputBufferDesc, AT_GPU),
                                            firstEntryOffset);
            // Get BAR0 info
            NvU8 *pWindowAddress = pKernelBus->pDefaultBar0Pointer;
            NvU64 windowSize = pKernelBus->physicalBar0WindowSize;

            //
            // Set PRAMIN window offset to the page needed,
            // logic is copied from kbusMemAccessBar0Window_GM107
            //
            NvU64 currentBar0Mapping = kbusGetBAR0WindowVidOffset_HAL(pGpu, pKernelBus);

            //
            // First check if start of window is in range,
            // then check if end of window is in range
            //
            if (phys < currentBar0Mapping ||
                phys + entryRangeSize >= currentBar0Mapping + windowSize)
            {
                kbusSetBAR0WindowVidOffset_HAL(pGpu,
                                               pKernelBus,
                                               (phys & ~(windowSize - 1llu)));
                oldBar0Mapping = currentBar0Mapping;
                currentBar0Mapping = (phys & ~(windowSize - 1llu));
                bRestore = NV_TRUE;
            }

            pOutputBufferMapping = &pWindowAddress[phys - currentBar0Mapping];
        }
    }
    else
    {
        // BAR2 in virtual mode
        pOutputBufferMapping = memdescGetPhysAddr(pOutputBufferDesc,
                                                 FORCE_VMMU_TRANSLATION(pOutputBufferDesc, AT_GPU),
                                                 firstEntryOffset) -
                               pKernelBus->bar2[gfid].pdeBase +
                               pKernelBus->virtualBar2[gfid].pPageLevels;
    }

    portMemCopy(pOutputBufferMapping,
                entryRangeSize,
                pStagingBufferMapping,
                entryRangeSize);

unmap_and_exit:
    memdescUnmapOld(pStagingBufferDesc, NV_TRUE, 0, pStagingDescMapping, pPriv);

    if (bRestore)
    {
        kbusSetBAR0WindowVidOffset_HAL(pGpu, pKernelBus, oldBar0Mapping);
    }
}

const MMU_WALK_CALLBACKS g_bar2WalkCallbacks =
{
    _bar2WalkCBLevelAlloc,
    _bar2WalkCBLevelFree,
    _bar2WalkCBUpdatePdb,
    _bar2WalkCBUpdatePde,
    _bar2WalkCBFillEntries,
    NULL,
    _bar2WalkCBWriteBuffer,
};
