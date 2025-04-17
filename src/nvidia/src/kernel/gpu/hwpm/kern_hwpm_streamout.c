/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @file
 * @brief  Collection of interfaces for HWPM streamout
 */

/* ------------------------ Includes --------------------------------------- */
#include "gpu/hwpm/kern_hwpm.h"
#include "lib/ref_count.h"
#include "gpu/gpu.h"
#include "gpu/mmu/kern_gmmu.h"
#include "mem_mgr/vaspace.h"
#include "mem_mgr/virt_mem_mgr.h"
#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "resserv/rs_client.h"
#include "vgpu/rpc.h"
#include "virtualization/hypervisor/hypervisor.h"
#include "gpu/bus/kern_bus.h"

#include "ctrl/ctrl90cc.h"
#include "ctrl/ctrlb0cc.h"
#include "class/cl90f1.h" //FERMI_VASPACE_A
#include "rmapi/rs_utils.h"
#include "nvrm_registry.h"

/* ------------------------ Macros ----------------------------------------- */
#define MODS_MULTI_VA_MAPPING_BUFFER_SIZE_LIMIT        (32 * 1024 * 1024)

/* ------------------------ Public Functions  ------------------------------ */

static NV_STATUS
_hwpmStreamoutAllocPmaMapping
(
    OBJGPU            *pGpu,
    KernelHwpm        *pKernelHwpm,
    OBJVASPACE        *pPmaVAS,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU64              virtualAddress
)
{
    VirtMemAllocator *pDma = GPU_GET_DMA(pGpu);
    NvU32 flags = DRF_DEF(OS46, _FLAGS, _DMA_UNICAST_REUSE_ALLOC, _TRUE);

    if (RMCFG_FEATURE_PLATFORM_GSP ||
        (memdescGetCpuCacheAttrib(pMemDesc) == NV_MEMORY_CACHED))
    {
        flags |= FLD_SET_DRF(OS46, _FLAGS, _CACHE_SNOOP, _ENABLE, flags);
    }

    // Map it in PMA VA
    return dmaAllocMapping_HAL(pGpu, pDma, pPmaVAS, pMemDesc, &virtualAddress, flags, NULL, KMIGMGR_SWIZZID_INVALID);
}

static NV_STATUS
_hwpmStreamoutAllocCpuMapping
(
    OBJGPU            *pGpu,
    KernelHwpm        *pKernelHwpm,
    OBJVASPACE        *pPmaVAS,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvP64             *ppCpuAddr,
    NvP64             *ppPriv
)
{
    NvP64 pAddr64 = NvP64_NULL;
    NvU8 *pBuf = NULL;
    NV_STATUS status = NV_OK;

    if (memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM)
    {
        pBuf = kbusMapRmAperture_HAL(pGpu, pMemDesc);
        if (pBuf == NULL)
        {
            NV_PRINTF(LEVEL_ERROR, "busMapRmAperture_HAL failed\n");
            status = NV_ERR_GENERIC;
            goto _hwpmStreamoutAllocCpuMapping_fail;
        }
        pAddr64 = NV_PTR_TO_NvP64(pBuf);
    }
    else
    {
        status = memdescMap(pMemDesc, 0, pMemDesc->Size, NV_TRUE, NV_PROTECT_READ_WRITE,
            &pAddr64, ppPriv);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "memdescMap failed: 0x%x\n", status);
            goto _hwpmStreamoutAllocCpuMapping_fail;
        }
    }

    *ppCpuAddr = pAddr64;

    return NV_OK;

_hwpmStreamoutAllocCpuMapping_fail:
    NV_PRINTF(LEVEL_ERROR, "Error: 0x%x\n", status);

    return status;
}

static void
_hwpmStreamoutFreeCpuMapping
(
    OBJGPU            *pGpu,
    KernelHwpm        *pKernelHwpm,
    OBJVASPACE        *pPmaVAS,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvP64              pCpuAddr,
    NvP64              pPriv
)
{
    NvU8 *pCpuAddrTmp = NvP64_VALUE(pCpuAddr);

    if (pCpuAddrTmp != NULL)
    {
        if (memdescGetAddressSpace(pMemDesc) == ADDR_FBMEM)
        {
            kbusUnmapRmAperture_HAL(pGpu, pMemDesc, &pCpuAddrTmp, NV_TRUE);
        }
        else
        {
            memdescUnmap(pMemDesc, NV_TRUE, osGetCurrentProcess(), pCpuAddr, pPriv);
        }
    }
}

/*!
 * @brief: Allocates a PMA stream for HWPM to streamout records.
 *
 * Each PMA stream can have upto 4GB of VA space, and PMA stream can not straddle
 * 4GB boundary.
 * Caller needs to provide two memory buffers, one for streaming records, and other
 * for streaming mem bytes (unread bytes).
 */
NV_STATUS
khwpmStreamoutAllocPmaStream_IMPL
(
    OBJGPU            *pGpu,
    KernelHwpm        *pKernelHwpm,
    NvU64              profilerId,
    MEMORY_DESCRIPTOR *pRecordBufDesc,
    MEMORY_DESCRIPTOR *pNumBytesBufDesc,
    NvU32              pmaChIdx,
    HWPM_PMA_STREAM   *pPmaStream
)
{
    OBJVASPACE       *pPmaVAS;
    NvU64             virtSize;
    NvU64             virtualAddressIter = 0;
    NvU64             virtualAddress = 0;
    NvU64             virtualAddress2 = 0;
    NvU64             vaAlign;
    NvU64             pageSize;
    NvU64             vaSizeRequested;
    VAS_ALLOC_FLAGS   allocFlags = {0};
    NV_STATUS         status = NV_OK;
    NvP64             pCpuAddr = NvP64_NULL;
    NvP64             pPriv = NvP64_NULL;
    NvU32             gfid;
    const NvU32       bpcIdx = pmaChIdx / pKernelHwpm->maxChannelPerCblock;
    OBJREFCNT        *pRefcnt;
    NvBool            bRefCnted = NV_FALSE;
    NvU32             i;
    NvU32             recordMappingCount = 1;

    NV_CHECK_OR_RETURN(LEVEL_ERROR, (bpcIdx < pKernelHwpm->maxCblocks), NV_ERR_INVALID_ARGUMENT);

    pRefcnt = pKernelHwpm->streamoutState[bpcIdx].pPmaVasRefcnt;
    if (pRefcnt == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    NV_ASSERT_OK_OR_RETURN(refcntRequestReference(pRefcnt, profilerId, REFCNT_STATE_ENABLED, NV_FALSE));
    bRefCnted = NV_TRUE;

    pPmaVAS = pKernelHwpm->streamoutState[bpcIdx].pPmaVAS;
    if (pPmaVAS == NULL)
    {
        status = NV_ERR_INVALID_STATE;
        goto hwpmStreamoutAllocPmaStream_fail;
    }

    if (pPmaStream->bValid)
    {
        status = NV_ERR_INVALID_STATE;
        goto hwpmStreamoutAllocPmaStream_fail;
    }

    pageSize = vaspaceGetBigPageSize(pPmaVAS);
    vaSizeRequested = RM_ALIGN_UP(pRecordBufDesc->Size, pageSize) + RM_ALIGN_UP(pNumBytesBufDesc->Size, pageSize);
    if (vaSizeRequested > pKernelHwpm->perCtxSize)
    {
        status = NV_ERR_INVALID_ARGUMENT;
        goto hwpmStreamoutAllocPmaStream_fail;
    }

    allocFlags.bLazy = NV_TRUE;
    virtSize = pKernelHwpm->perCtxSize;
    vaAlign = virtSize;
    status = vaspaceAlloc(pPmaVAS, virtSize, vaAlign, vaspaceGetVaStart(pPmaVAS), vaspaceGetVaLimit(pPmaVAS),
                          0, allocFlags, &virtualAddress);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "vaspaceAlloc failed: 0x%08x\n", status);
        goto hwpmStreamoutAllocPmaStream_fail;
    }

    virtualAddressIter = virtualAddress;
    for (i = 0; i < recordMappingCount; i++)
    {
        status = _hwpmStreamoutAllocPmaMapping(pGpu, pKernelHwpm, pPmaVAS, pRecordBufDesc, virtualAddressIter);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                    "Failed to map records buffer to pma vaspace: 0x%08x\n",
                    status);
            goto hwpmStreamoutAllocPmaStream_fail;
        }
        virtualAddressIter += RM_ALIGN_UP(pRecordBufDesc->Size, pageSize);
    }

    // memBytes va start right after record buffer va.
    virtualAddress2 = virtualAddressIter;
    status = _hwpmStreamoutAllocPmaMapping(pGpu, pKernelHwpm, pPmaVAS, pNumBytesBufDesc, virtualAddress2);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to map available bytes buffer to pma vaspace: 0x%08x\n",
                  status);
        goto hwpmStreamoutAllocPmaStream_fail;
    }

    pPmaStream->flags = 0;

    // CPU mapping of membytes buffer is not required on vGPU host, will be handled on guest
    NV_ASSERT_OK_OR_GOTO(status, vgpuGetCallingContextGfid(pGpu, &gfid),
        hwpmStreamoutAllocPmaStream_fail);
    if (!(hypervisorIsVgxHyper() || (RMCFG_FEATURE_PLATFORM_GSP && IS_GFID_VF(gfid))))
    {
        // If this is a WSL profiler using DMA remapping support, we already have
        // the CPUVA in pNumBytesBufDesc->_pMemData
        if (pNumBytesBufDesc->_pMemData)
        {
            pCpuAddr = pNumBytesBufDesc->_pMemData;
            pPriv = pNumBytesBufDesc->_pMemData;

            // Set the _pMemData fields to NULL. Otherwise, osDestroyMemCreatedFromOsDescriptor()
            // will interpret the _pMemData as a pointer to a SYS_MEM_INFO structure.
            pRecordBufDesc->_pMemData = NvP64_NULL;
            pNumBytesBufDesc->_pMemData = NvP64_NULL;
            pPmaStream->flags |= NV_HWPM_STREAM_FLAGS_CPUVA_EXTERNAL;
            status = NV_OK;
        }
        else
        {
            status = _hwpmStreamoutAllocCpuMapping(pGpu, pKernelHwpm, pPmaVAS, pNumBytesBufDesc, &pCpuAddr, &pPriv);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Failed to map available bytes buffer to cpu vaspace: 0x%08x\n",
                          status);
                goto hwpmStreamoutAllocPmaStream_fail;
            }
        }
    }

    pPmaStream->pRecordBufDesc = pRecordBufDesc;
    pPmaStream->pNumBytesBufDesc = pNumBytesBufDesc;
    pPmaStream->pNumBytesCpuAddr = pCpuAddr;
    pPmaStream->pNumBytesCpuAddrPriv = pPriv;
    pPmaStream->vaddrRecordBuf = virtualAddress;
    pPmaStream->vaddrNumBytesBuf = virtualAddress2;
    pPmaStream->size = pRecordBufDesc->Size;
    pPmaStream->pmaChannelIdx = pmaChIdx;
    pPmaStream->bValid = NV_TRUE;

    return NV_OK;

hwpmStreamoutAllocPmaStream_fail:
    if (pCpuAddr != NvP64_NULL)
    {
        _hwpmStreamoutFreeCpuMapping(pGpu, pKernelHwpm, pPmaVAS, pNumBytesBufDesc, pCpuAddr, pPriv);
    }

    if (virtualAddress != 0)
    {
        // free va allocations and mappings
        vaspaceFree(pPmaVAS, virtualAddress);
    }

    if (bRefCnted)
    {
        status = refcntReleaseReferences(pRefcnt, profilerId, NV_FALSE);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Releasing pPmaVasRefcnt failed on pmChIdx-%d.\n", pmaChIdx);
        }
    }

    return status;
}

/*!
 * @brief: Frees a PMA stream.
 */
NV_STATUS
khwpmStreamoutFreePmaStream_IMPL
(
    OBJGPU          *pGpu,
    KernelHwpm      *pKernelHwpm,
    NvU64            profilerId,
    HWPM_PMA_STREAM *pPmaStream,
    NvU32            pmaChIdx
)
{
    NV_STATUS   status = NV_OK;
    OBJVASPACE *pPmaVAS;
    const NvU32 bpcIdx = pmaChIdx / pKernelHwpm->maxChannelPerCblock;
    OBJREFCNT  *pRefcnt;

    NV_ASSERT_OR_RETURN(bpcIdx < pKernelHwpm->maxCblocks, NV_ERR_INVALID_ARGUMENT);

    pRefcnt = pKernelHwpm->streamoutState[bpcIdx].pPmaVasRefcnt;
    if (pRefcnt == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    pPmaVAS = pKernelHwpm->streamoutState[bpcIdx].pPmaVAS;
    if (pPmaVAS == NULL)
    {
        return NV_ERR_INVALID_STATE;
    }

    if (!pPmaStream->bValid)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // If there is a CPU address and it was not allocated externally, free
    // the internal CPU mapping.
    if ((pPmaStream->pNumBytesCpuAddr != NvP64_NULL) &&
        (!(pPmaStream->flags & NV_HWPM_STREAM_FLAGS_CPUVA_EXTERNAL)))
    {
        _hwpmStreamoutFreeCpuMapping(pGpu, pKernelHwpm, pPmaVAS, pPmaStream->pNumBytesBufDesc, pPmaStream->pNumBytesCpuAddr,
            pPmaStream->pNumBytesCpuAddrPriv);
    }
    pPmaStream->pNumBytesCpuAddr = NvP64_NULL;
    pPmaStream->pNumBytesCpuAddrPriv = NvP64_NULL;

    if (pPmaStream->vaddrRecordBuf != 0)
    {
        vaspaceFree(pPmaVAS, pPmaStream->vaddrRecordBuf);
        pPmaStream->vaddrRecordBuf = 0;
    }
    pPmaStream->size = 0;

    memdescDestroy(pPmaStream->pRecordBufDesc);
    pPmaStream->pRecordBufDesc = NULL;

    memdescDestroy(pPmaStream->pNumBytesBufDesc);
    pPmaStream->pNumBytesBufDesc = NULL;
    pPmaStream->vaddrNumBytesBuf = 0;

    pPmaStream->pmaChannelIdx = INVALID_PMA_CHANNEL_IDX;
    pPmaStream->bValid = NV_FALSE;

    status = refcntReleaseReferences(pRefcnt, profilerId, NV_FALSE);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Releasing pPmaVasRefcnt failed on pmChIdx-%d.\n", pmaChIdx);
    }

    return status;
}

static NV_STATUS
khwpmInstBlkConstruct(OBJGPU *pGpu, KernelHwpm *pKernelHwpm, NvU32 bpcIdx)
{
    MemoryManager     *pMemoryManager  = GPU_GET_MEMORY_MANAGER(pGpu);
    NV_STATUS          status;
    NvU32              allocFlags      = MEMDESC_FLAGS_NONE;
    MEMORY_DESCRIPTOR *pInstBlkMemDesc = NULL;
    NvU32              addrSpace       = pGpu->getProperty(pGpu, PDB_PROP_GPU_ZERO_FB) ? ADDR_SYSMEM : ADDR_FBMEM;
    NvU32              attr            = NV_MEMORY_WRITECOMBINED;

    memdescOverrideInstLoc(DRF_VAL(_REG_STR_RM, _INST_LOC_4, _HWPM_PMA, pGpu->instLocOverrides4),
                           "HWPM PMA instblk", &addrSpace, &attr);

    if ((status = memdescCreate(&pInstBlkMemDesc, pGpu,
                                GF100_BUS_INSTANCEBLOCK_SIZE,
                                GF100_BUS_INSTANCEBLOCK_SIZE,
                                NV_TRUE,
                                addrSpace,
                                attr,
                                allocFlags)) != NV_OK)
    {
        goto constructInstBlkMemDesc_fail;
    }

    if ((memdescGetAddressSpace(pInstBlkMemDesc) == ADDR_SYSMEM) &&
        (gpuIsInstanceMemoryAlwaysCached(pGpu)))
    {
        memdescSetGpuCacheAttrib(pInstBlkMemDesc, NV_MEMORY_CACHED);
    }

    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_59,
                    pInstBlkMemDesc);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "couldn't allocate PERF instblk!\n");
        goto constructInstBlkMemDesc_fail;
    }

    status = memmgrMemDescMemSet(pMemoryManager, pInstBlkMemDesc, 0, TRANSFER_FLAGS_NONE);
    if (NV_OK != status)
    {
        goto constructInstBlkMemDesc_fail;
    }

    pKernelHwpm->streamoutState[bpcIdx].pInstBlkMemDesc = pInstBlkMemDesc;

    return NV_OK;

constructInstBlkMemDesc_fail:
    if (pInstBlkMemDesc != NULL)
    {
        memdescFree(pInstBlkMemDesc);
        memdescDestroy(pInstBlkMemDesc);
    }
    return status;
}

static NV_STATUS
khwpmStreamoutInstBlkDestruct(OBJGPU *pGpu, KernelHwpm *pKernelHwpm, NvU32 bpcIdx)
{
    // Free the Instblk Surface resources
    if (pKernelHwpm->streamoutState[bpcIdx].pInstBlkMemDesc != NULL)
    {
        memdescFree(pKernelHwpm->streamoutState[bpcIdx].pInstBlkMemDesc);
        memdescDestroy(pKernelHwpm->streamoutState[bpcIdx].pInstBlkMemDesc);
        pKernelHwpm->streamoutState[bpcIdx].pInstBlkMemDesc = NULL;
    }

    return NV_OK;
}

/*!
 * @brief: Creates VA Space and Inst block corresponding to a
 * given Channel ID (for full SRIOV guest)
 */
NV_STATUS
khwpmStreamoutCreatePmaVaSpace_IMPL
(
    OBJGPU     *pGpu,
    KernelHwpm *pKernelHwpm,
    NvU32       bpcIdx
)
{
    OBJVMM     *pVmm               = SYS_GET_VMM(SYS_GET_INSTANCE());
    KernelGmmu *pKernelGmmu        = GPU_GET_KERNEL_GMMU(pGpu);
    OBJVASPACE *pVAS               = NULL;
    NvU64       vaSpaceStartAddr   = pKernelHwpm->vaSpaceBase;
    NvU64       vaRangeLimit       = pKernelHwpm->vaSpaceBase + pKernelHwpm->vaSpaceSize - 1;
    NvBool      bRootPageDirPinned = NV_FALSE;
    NvU32       flags;
    NV_STATUS   status;
    INST_BLK_INIT_PARAMS params = {0};

    NV_CHECK_OR_RETURN(LEVEL_ERROR, (bpcIdx < pKernelHwpm->maxCblocks), NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN((pKernelHwpm->streamoutState[bpcIdx].pPmaVAS == NULL), NV_ERR_INVALID_STATE);

    status = khwpmInstBlkConstruct(pGpu, pKernelHwpm, bpcIdx);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Failed to construct PMA Instance block. Status 0x%x\n",
                  status);
        goto khwpmStreamoutCreatePmaVaSpace_fail;
    }

    // Initialize a heap for PERF VASpace
    flags = DRF_DEF(_VASPACE, _FLAGS, _BIG_PAGE_SIZE, _DEFAULT) |
            VASPACE_FLAGS_ENABLE_VMM |
            VASPACE_FLAGS_HWPM;

    status = vmmCreateVaspace(pVmm, FERMI_VASPACE_A, 0, NVBIT(pGpu->gpuInstance),
                              vaSpaceStartAddr, vaRangeLimit, 0, 0, NULL,
                              flags, &pVAS);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Could not construct PMA vaspace object. Status 0x%x\n",
                  status);
        pVAS = NULL;
        goto khwpmStreamoutCreatePmaVaSpace_fail;
    }

    status = vaspacePinRootPageDir(pVAS, pGpu);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "Error Locking down VA space root.\n");
        goto khwpmStreamoutCreatePmaVaSpace_fail;
    }
    bRootPageDirPinned = NV_TRUE;

    status = kgmmuInstBlkInit(pKernelGmmu, pKernelHwpm->streamoutState[bpcIdx].pInstBlkMemDesc,
                              pVAS, FIFO_PDB_IDX_BASE, &params);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Error initializing HWPM PMA Instance Block.\n");
        goto khwpmStreamoutCreatePmaVaSpace_fail;
    }

    pKernelHwpm->streamoutState[bpcIdx].pPmaVAS = pVAS;

    return NV_OK;

khwpmStreamoutCreatePmaVaSpace_fail:
    if (bRootPageDirPinned)
    {
        vaspaceUnpinRootPageDir(pVAS, pGpu);
    }
    if (pVAS != NULL)
    {
        vmmDestroyVaspace(pVmm, pVAS);
    }
    khwpmStreamoutInstBlkDestruct(pGpu, pKernelHwpm, bpcIdx);
    pKernelHwpm->streamoutState[bpcIdx].pPmaVAS = NULL;

    return status;
}

/*!
 * @brief: Frees VA Space and Inst block (for full SRIOV guest)
 */
NV_STATUS
khwpmStreamoutFreePmaVaSpace_IMPL
(
    OBJGPU     *pGpu,
    KernelHwpm *pKernelHwpm,
    NvU32       bpcIdx
)
{
    OBJSYS     *pSys = SYS_GET_INSTANCE();
    OBJVMM     *pVmm = SYS_GET_VMM(pSys);
    OBJVASPACE *pVAS;

    NV_CHECK_OR_RETURN(LEVEL_ERROR, (bpcIdx < pKernelHwpm->maxCblocks), NV_ERR_INVALID_ARGUMENT);

    pVAS = pKernelHwpm->streamoutState[bpcIdx].pPmaVAS;
    if (pVAS != NULL)
    {
        vaspaceUnpinRootPageDir(pVAS, pGpu);
        vmmDestroyVaspace(pVmm, pVAS);
        khwpmStreamoutInstBlkDestruct(pGpu, pKernelHwpm, bpcIdx);
        pKernelHwpm->streamoutState[bpcIdx].pPmaVAS = NULL;
    }

    // Reset GFID in SRIOV mode
    if (gpuIsSriovEnabled(pGpu))
    {
        khwpmPmaStreamSriovSetGfid_HAL(pGpu, pKernelHwpm, bpcIdx, 0);
    }

    return NV_OK;
}
