/*
 * SPDX-FileCopyrightText: Copyright (c) 2005-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * @brief The FERMI specific HAL VMA routines reside in this file
 *
 * ===========================================================================
 * GLOSSARY OF INCONSISTENCIES
 * ===========================================================================
 *
 * --------
 * LOW, MIN
 * --------
 *  (1) Synonyms for the first address or index in a range.
 *          e.g. In the inclusive range 37 to 509, the "low" or "min" is 37.
 *
 * ---------------------
 * HIGH, MAX, LIMIT, END
 * ---------------------
 *  (1) Synonyms for the last address or index in a range.
 *          e.g. In the inclusive range 37 to 509, the "limit" is 509.
 *  (2) Sometimes "high" or "end" are used for the "limit plus one" - e.g. 510.
 *          Currently this can only be determined by context.
 *  TODO: Decide on consistent terms and clean this up.
 *
 * ---
 * PDB
 * ---
 *  (1) Page Directory Base
 *          The base address of a page directory,
 *          e.g. written to the PD_BASE field of an instance block.
 *  (2) Page Directory Block
 *          The entire physical memory block of a page directory,
 *          e.g. described by a memdesc associated with a VASPACE object.
 *  (3) Property DataBase - e.g. in PDB_PROP_*
 *          The common meaning to the rest of RM - boolean values associated
 *          with an object. Completely unrelated to (1) and (2).
 *
 * ---
 * PDE
 * ---
 *  (1) Page Directory Entry
 *          An *ENTRY* within a page directory, containing the physical
 *          addresses and attributes of a single small/big page table pair.
 *  (2) !!!WRONG!!! The page direcory itself
 *          Somtimes also used in the plural form "PDEs".
 *          Use "page directory" or "PD" instead.
 *
 * --------------------------
 * PDE ENTRY !!!DO NOT USE!!!
 * --------------------------
 *  (1) !!!WRONG!!! Page Directory Entry Entry(?!)
 *          This is redundant - just use "PDE".
 *  (2) Page Dir*E*ctory Entry
 *          Desperate bacronym to justify current usage.
 *
 * --------
 * PDE SIZE
 * --------
 *  (1) Size or index corresponding to the NV_MMU_PDE_SIZE field of a PDE.
 *          This refers to the size of *page tables* that this
 *          PDE points to (1/8, 1/4, 1/2, full), not the size of the PDE itself.
 *          The more accurate term is "PT size" - most code has been cleaned up
 *          to use this instead, but some API params remain.
 *  (2) Size of the PDE itself (8 bytes), defined by the constant NV_MMU_PDE__SIZE.
 *
 * ---
 * PTE
 * ---
 *  (1) Page Table Entry
 *          An *ENTRY* within a page table, containing the physical
 *          address and attributes of a single page (small or big).
 *  (2) !!!WRONG!!! The page table itself
 *          Somtimes also used in the plural form "PTEs".
 *          Use "page table" or "PT" instead.
 *
 * --------------------------
 * PTE ENTRY !!!DO NOT USE!!!
 * --------------------------
 *  (1) !!!WRONG!!! Page Table Entry Entry(?!)
 *          This is redundant - just use "PTE".
 *  (2) Page Tabl*E* Entry
 *          Desperate bacronym to justify current usage.
 *
 */

#include "core/core.h"
#include "gpu/gpu.h"
#include "lib/base_utils.h"
#include "gpu/mem_mgr/heap.h"
#include "os/os.h"
#include "rmapi/client.h"
#include "nvrm_registry.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "gpu/bif/kernel_bif.h"
#include "core/system.h"
#include "core/thread_state.h"
#include "mem_mgr/vaspace.h"
#include "mem_mgr/fabric_vaspace.h"
#include "mem_mgr/virt_mem_mgr.h"
#include "platform/sli/sli.h"
#include "containers/eheap_old.h"

#include "mem_mgr/fla_mem.h"

#include "gpu/mmu/kern_gmmu.h"
#include "gpu/mem_sys/kern_mem_sys.h"
#include "gpu_mgr/gpu_group.h"
#include "mmu/mmu_fmt.h"
#include "gpu/device/device.h"
#include "gpu/nvlink/kernel_nvlink.h"
#include "gpu/bus/kern_bus.h"
#include "gpu/mem_mgr/mem_mgr.h"

#include "gpu/mem_mgr/fermi_dma.h"

#include "published/maxwell/gm107/dev_mmu.h"
#include "published/maxwell/gm107/dev_bus.h"

#include "ctrl/ctrl0002.h"

#include "vgpu/rpc.h"

#define _MMUXLATEVADDR_FLAG_SHOW_INVALID        NVBIT(0)
#define _MMUXLATEVADDR_FLAG_VALIDATE_ONLY       NVBIT(1) // incomplete
#define _MMUXLATEVADDR_FLAG_VALIDATE_TERSELY    NVBIT(2) // incomplete
// no trace output
#define _MMUXLATEVADDR_FLAG_XLATE_ONLY          _MMUXLATEVADDR_FLAG_VALIDATE_TERSELY

static NV_STATUS _dmaGetFabricAddress(OBJGPU *pGpu, NvU32 aperture, NvU32 kind,
                                        NvU64 *fabricAddr);
static NV_STATUS _dmaGetFabricEgmAddress(OBJGPU *pGpu, NvU32 aperture, NvU32 kind,
                                        NvU64 *fabricEgmAddr);

static NV_STATUS
_dmaApplyWarForBug2720120
(
    OBJGVASPACE  *pGVAS,
    OBJGPU       *pGpu,
    const NvU64   vaLo,
    const NvU64   vaHi
);

//
// Virtual Address Space Block - Data tracked per virtual allocation
//
// only used with NV_REG_STR_RESERVE_PTE_SYSMEM_MB.  Protential dead code.
typedef struct VASINFO_MAXWELL
{
    PNODE pMapTree;      // Tree of current mappings.
    NvU32 pageSizeMask;  // Mask of page size indices supported.
                         // See VAS_PAGESIZE_MASK.
    VAS_ALLOC_FLAGS flags;
    VA_MANAGEMENT management; // Level of management.
} VASINFO_MAXWELL, *PVASINFO_MAXWELL;

/*!
 * @brief Allocate virtual memory and map it to physical memory.
 *
 * The virtual memory may already be allocated, in which case it is just
 * initialized (backing page table tables allocated).
 *
 * VMM-TODO: If possible remove overloading - e.g. just map, never allocate.
 *           Definitely move MMU stuff down.
 *
 * @param[in]     pGpu              OBJGPU pointer
 * @param[in]     pDma              VirtMemAllocator pointer
 * @param[in]     pVAS              OBJVASPACE pointer
 * @param[in]     pMemDesc          Physical memory descriptor
 * @param[in/out] pVaddr            Pointer to Virtual memory base address
 * @param[in]     flags             Mapping options
 * @param[in]     pDmaMappingInfo   CLI_DMA_MAPPING_INFO pointer (for RM Client mappings)
 * @param[in]     swizzId           SMC swizzId (Only used for BAR1 mapping)
 *
 * @returns NV_STATUS status = NV_OK on success, or status upon failure.
 */
NV_STATUS
dmaAllocMapping_GM107
(
    OBJGPU             *pGpu,
    VirtMemAllocator   *pDma,
    OBJVASPACE         *pVAS,
    MEMORY_DESCRIPTOR  *pMemDesc,
    NvU64              *pVaddr,
    NvU32               flags,
    CLI_DMA_ALLOC_MAP_INFO *pCliMapInfo,
    NvU32               swizzId
)
{
    NV_STATUS           status            = NV_OK;
    MemoryManager      *pMemoryManager    = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    KernelMIGManager   *pKernelMIGManager = GPU_GET_KERNEL_MIG_MANAGER(pGpu);
    OBJEHEAP           *pVASpaceHeap      = NULL;
    KernelGmmu         *pKernelGmmu       = GPU_GET_KERNEL_GMMU(pGpu);
    KernelBus          *pKernelBus        = GPU_GET_KERNEL_BUS(pGpu);
    FABRIC_VASPACE     *pFabricVAS        = dynamicCast(pGpu->pFabricVAS, FABRIC_VASPACE);
    MEMORY_DESCRIPTOR  *pAdjustedMemDesc  = pMemDesc;
    ADDRESS_TRANSLATION addressTranslation;
    NvU32               gfid;
    NvBool              bCallingContextPlugin;
    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
        kmemsysGetStaticConfig(pGpu, pKernelMemorySystem);
    OBJGVASPACE        *pGVAS             = NULL;

    struct
    {
        NvU32              pteCount;
        NvU32              pageCount;
        NvU32              overMap;
        NvU64              vaLo;
        NvU64              vaHi;
        NvU64              mapLength;
        NvU64              pageOffset;
        NvU64              pageSize;
        NvU64              vaRangeLo;
        NvU64              vaRangeHi;
        NvU32              kind;
        NvU32              priv;
        NvU32              cacheSnoop;
        COMPR_INFO         comprInfo;
        NvU32              aperture;
        NvU32              tlbLock;
        NvU32              p2p;
        NvU32              writeOnly;
        NvU32              readOnly;
        NvU32              subDevIdSrc;
        NvU32              deferInvalidate;
        NODE              *pMapNode;
        NvU32              shaderFlags;
        NvU32              disableEncryption;
        VASINFO_MAXWELL     *pVASInfo;
        OBJGPU            *pSrcGpu;
        NvU32              peerNumber;
        NvBool             bAllocVASpace;
        NvBool             bIsBarOrPerf;
        NvBool             bIsBar1;
        NvBool             bIsMIGMemPartitioningEnabled;
        RmPhysAddr        *pPteArray;
        DMA_PAGE_ARRAY     pageArray;
        NvU64              vaspaceBigPageSize;
        NvBool             bIsMemContiguous;
        NvU64              fabricAddr;
        NvU32              indirectPeer;
        NvBool             bFlaImport;
        NV_RANGE           totalVaRange;
        MEMORY_DESCRIPTOR *pRootMemDesc;
        MEMORY_DESCRIPTOR *pTempMemDesc;
        Memory            *pMemory;
        NvU64              pageArrayGranularity;
        NvU8               pageShift;
        NvU64              physPageSize;
        NvU64              pageArrayFlags;
    } *pLocals = portMemAllocNonPaged(sizeof(*pLocals));
    // Heap Allocate to avoid stack overflow

    if (pLocals == NULL)
        return NV_ERR_NO_MEMORY;

    portMemSet(pLocals, 0, sizeof(*pLocals));
    pLocals->pSrcGpu = pGpu;
    pLocals->peerNumber = BUS_INVALID_PEER;
    pLocals->totalVaRange = NV_RANGE_EMPTY;

    if (pCliMapInfo != NULL)
        pLocals->pMemory = pCliMapInfo->pMemory;

    pLocals->vaspaceBigPageSize = vaspaceGetBigPageSize(pVAS);
    pLocals->bIsBarOrPerf       = (vaspaceGetFlags(pVAS) &
                                  (VASPACE_FLAGS_BAR|VASPACE_FLAGS_PERFMON|VASPACE_FLAGS_HDA)) != 0;
    pLocals->p2p                = DRF_VAL(OS46, _FLAGS, _P2P_ENABLE, flags);
    pLocals->subDevIdSrc        = DRF_VAL(OS46, _FLAGS, _P2P_SUBDEV_ID_SRC, flags);
    pLocals->deferInvalidate    = FLD_TEST_DRF(OS46, _FLAGS, _DEFER_TLB_INVALIDATION, _TRUE, flags) ?
                                               DMA_DEFER_TLB_INVALIDATE : DMA_TLB_INVALIDATE;
    pLocals->bAllocVASpace      = FLD_TEST_DRF(OS46, _FLAGS, _DMA_UNICAST_REUSE_ALLOC, _FALSE, flags);
    pLocals->bIsBar1            = (vaspaceGetFlags(pVAS) & VASPACE_FLAGS_BAR_BAR1) != 0;
    pLocals->bIsMIGMemPartitioningEnabled = (pKernelMIGManager != NULL) && kmigmgrIsMIGMemPartitioningEnabled(pGpu, pKernelMIGManager);

    pLocals->cacheSnoop = (NVOS46_FLAGS_CACHE_SNOOP_ENABLE == DRF_VAL(OS46, _FLAGS, _CACHE_SNOOP, flags));
    pLocals->writeOnly  = (NVOS46_FLAGS_ACCESS_WRITE_ONLY == DRF_VAL(OS46, _FLAGS, _ACCESS, flags));
    pLocals->readOnly   = (NVOS46_FLAGS_ACCESS_READ_ONLY == DRF_VAL(OS46, _FLAGS, _ACCESS, flags)) ?
                           DMA_UPDATE_VASPACE_FLAGS_READ_ONLY : 0;
    pLocals->tlbLock    = (NVOS46_FLAGS_TLB_LOCK_ENABLE == DRF_VAL(OS46, _FLAGS, _TLB_LOCK, flags)) ?
                           DMA_UPDATE_VASPACE_FLAGS_TLB_LOCK : 0;

    switch (DRF_VAL(OS46, _FLAGS, _SHADER_ACCESS, flags))
    {
        default:
        case NVOS46_FLAGS_SHADER_ACCESS_DEFAULT:
            // The default (0) case we pick up the SHADER_ACCESS from ACCESS.
            pLocals->shaderFlags = 0;
            if (pLocals->readOnly)
                pLocals->shaderFlags |= DMA_UPDATE_VASPACE_FLAGS_SHADER_READ_ONLY;
            if (pLocals->writeOnly)
                pLocals->shaderFlags |= DMA_UPDATE_VASPACE_FLAGS_SHADER_WRITE_ONLY;
            break;
        case NVOS46_FLAGS_SHADER_ACCESS_READ_WRITE:
            pLocals->shaderFlags = 0;
            break;
        case NVOS46_FLAGS_SHADER_ACCESS_READ_ONLY:
            pLocals->shaderFlags = DMA_UPDATE_VASPACE_FLAGS_SHADER_READ_ONLY;
            break;
        case NVOS46_FLAGS_SHADER_ACCESS_WRITE_ONLY:
            pLocals->shaderFlags = DMA_UPDATE_VASPACE_FLAGS_SHADER_WRITE_ONLY;
            break;
    }

    addressTranslation = VAS_ADDRESS_TRANSLATION(pVAS);
    // In SRIOV-heavy plugin may map subheap allocations for itself using BAR1
    NV_ASSERT_OK_OR_GOTO(status, vgpuIsCallingContextPlugin(pGpu, &bCallingContextPlugin), cleanup);
    if (bCallingContextPlugin)
        addressTranslation = FORCE_VMMU_TRANSLATION(pMemDesc, addressTranslation);

    if (pFabricVAS != NULL)
    {
        status = fabricvaspaceGetGpaMemdesc(pFabricVAS, pMemDesc, pGpu, &pAdjustedMemDesc);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR, "Failed to get the adjusted memdesc for the fabric memdesc\n");
            goto cleanup;
        }
    }

    // Get pageSize
    pLocals->pTempMemDesc = memdescGetMemDescFromGpu(pAdjustedMemDesc, pGpu);

    // Get physical allocation granularity and page size.
    pLocals->pageArrayGranularity = pLocals->pTempMemDesc->pageArrayGranularity;
    pLocals->physPageSize = memdescGetPageSize(pLocals->pTempMemDesc, addressTranslation);
    pLocals->bIsMemContiguous = memdescGetContiguity(pLocals->pTempMemDesc, addressTranslation);

    // retrieve mapping page size from flags
    switch(DRF_VAL(OS46, _FLAGS, _PAGE_SIZE, flags))
    {
        case NVOS46_FLAGS_PAGE_SIZE_DEFAULT:
        case NVOS46_FLAGS_PAGE_SIZE_BOTH:
            pLocals->pageSize = memdescGetPageSize(pLocals->pTempMemDesc, addressTranslation);
            break;
        case NVOS46_FLAGS_PAGE_SIZE_4KB:
            pLocals->pageSize = RM_PAGE_SIZE;
            break;
        case NVOS46_FLAGS_PAGE_SIZE_BIG:
            // case for arch specific 128K
            pLocals->pageSize = pLocals->vaspaceBigPageSize;
            break;
        case NVOS46_FLAGS_PAGE_SIZE_HUGE:
            pLocals->pageSize = RM_PAGE_SIZE_HUGE;
            break;
        case NVOS46_FLAGS_PAGE_SIZE_512M:
            pLocals->pageSize = RM_PAGE_SIZE_512M;
            break;
        default:
            NV_PRINTF(LEVEL_ERROR, "Unknown page size flag encountered during mapping\n");
            status = NV_ERR_INVALID_ARGUMENT;
            goto cleanup;
    }

    NV_PRINTF(LEVEL_INFO, "Picked Page size based on flags: 0x%llx flagVal: 0x%x\n",
                           pLocals->pageSize, DRF_VAL(OS46, _FLAGS, _PAGE_SIZE, flags));

    if (pLocals->physPageSize < pLocals->pageSize)
    {
        if (!pLocals->bIsMemContiguous ||
            !NV_IS_ALIGNED64(memdescGetPhysAddr(pLocals->pTempMemDesc, addressTranslation, 0), pLocals->pageSize) ||
            !NV_IS_ALIGNED64(pLocals->pTempMemDesc->Size, pLocals->pageSize))
        {
            //
            // Allow contig allocation to be mapped at page size bigger then physical,
            // but only when offset and size are aligned
            //
            NV_PRINTF(LEVEL_WARNING, "Requested mapping at larger page size than the physical granularity "
                                      "PhysPageSize = 0x%llx MapPageSize = 0x%llx. "
                                      "Overriding to physical page granularity...\n",
                                      pLocals->physPageSize, pLocals->pageSize);

            pLocals->pageSize = pLocals->physPageSize;
        }
    }

    if (memdescGetFlag(pLocals->pTempMemDesc, MEMDESC_FLAGS_DEVICE_READ_ONLY))
    {
        NV_ASSERT_OR_ELSE((pLocals->readOnly == DMA_UPDATE_VASPACE_FLAGS_READ_ONLY),
            status = NV_ERR_INVALID_ARGUMENT; goto cleanup);
    }

    //
    // Force BAR1 VA pageSize at bigPageSize only if total BAR1 size is less
    // than threshold(default: 256MB) to not waste BAR1.
    // For large BAR1 SKUs, avoid forcing 64KB size and use the pagesize of
    // the memdesc.
    //
    if (kgmmuIsVaspaceInteropSupported(pKernelGmmu) &&
        pLocals->bIsBar1)
    {
        if ((pLocals->pageSize > pLocals->vaspaceBigPageSize) &&
             kbusIsBar1Force64KBMappingEnabled(pKernelBus))
        {
            pLocals->pageSize = pLocals->vaspaceBigPageSize;

            SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
                memdescSetPageSize(memdescGetMemDescFromGpu(pAdjustedMemDesc, pGpu),
                               addressTranslation, (NvU32)pLocals->pageSize);
            SLI_LOOP_END
        }
    }

    pLocals->pageShift = BIT_IDX_32(pLocals->pageArrayGranularity);

    // Get mapping params on current gpu memdesc
    pLocals->pageOffset   = memdescGetPhysAddr(pLocals->pTempMemDesc, addressTranslation, 0) & (pLocals->pageSize - 1);
    pLocals->mapLength    = RM_ALIGN_UP(pLocals->pageOffset + pLocals->pTempMemDesc->Size, pLocals->pageSize);
    pLocals->pageCount    = NvU64_LO32(pLocals->mapLength >> pLocals->pageShift);

    pLocals->kind                           = NV_MMU_PTE_KIND_PITCH;

    // Get compression/pte pLocals->kind on current gpu memdesc
    status = memmgrGetKindComprFromMemDesc(pMemoryManager,
                                           pLocals->pTempMemDesc,
                                           0,
                                           &pLocals->kind, &pLocals->comprInfo);

    if (NV_OK != status)
        goto cleanup;

    //
    // When compression is enabled mapping at 4K is not supported due to
    // RM allocating one comptagline per 64KB allocation (From Pascal to Turing).
    // See bug 3909010
    //
    // Skipping it for Raw mode, See bug 4036809
    //
    if ((pLocals->pageSize == RM_PAGE_SIZE) &&
        memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, pLocals->kind) &&
        !(pMemorySystemConfig->bUseRawModeComptaglineAllocation))
    {
        NV_PRINTF(LEVEL_WARNING, "Requested 4K mapping on compressible sufrace. Overriding to physical page granularity...\n");
        pLocals->pageSize = pLocals->physPageSize;
    }

#ifdef DEBUG
    // Check for subdevices consistency if broadcast memdesc is passed in
    if (memdescHasSubDeviceMemDescs(pAdjustedMemDesc))
    {
        // Check pageOffset, pageSize consistency across subdevices
        memdescCheckSubDevicePageSizeConsistency(pGpu, pAdjustedMemDesc, pVAS, pLocals->pageSize, pLocals->pageOffset);

        // Check mem contiguity consistency across subdevices
        memdescCheckSubDeviceMemContiguityConsistency(pGpu, pAdjustedMemDesc, pVAS, pLocals->bIsMemContiguous);

        // Check compression/pte pLocals->kind consistency across subdevices
        status = memdescCheckSubDeviceKindComprConsistency(pGpu, pAdjustedMemDesc, pVAS,
                                                           pLocals->kind, &pLocals->comprInfo);
        NV_ASSERT(!status);
    }
#endif

    //
    //             +-- +-- +------------+ --+
    //             |   |   |            |   |==> pageOffset
    // pageSize <==|   |   |   Page 0   | --+
    //             |   |   |            |   |
    //             +-- |   +------------+   |
    //                 |   |            |   |
    //                 |   |   Page 1   |   |
    //                 |   |            |   |
    //    mapLength <==|   +------------+   |==> pMemDesc->Size
    //                 |   |            |   |
    //                 |   |     ...    |   |
    //                 |   |            |   |
    //                 |   +------------+   |
    //                 |   |            |   |
    //                 |   |  Page N-1  | --+
    //                 |   |            |
    //                 +-- +------------+
    //

    if (pLocals->bIsMemContiguous)
    {
        // FIXME: Throwing away physical length information is dangerous.
        pLocals->pteCount = 1;
    }
    else
    {
        // FIXME: This is broken for page size > 4KB and page offset
        //        that crosses a page boundary (can overrun pPteArray).
        // --
        // page count is one more than integral division in case of presence of offset hence being rounded up
        pLocals->pteCount = RM_ALIGN_UP((pLocals->pTempMemDesc->Size + pLocals->pageOffset), pLocals->pageArrayGranularity) >> BIT_IDX_32(pLocals->pageArrayGranularity);
    }

    // Disable PLC Compression for FLA->PA Mapping because of the HW Bug: 3046774
    if (pMemorySystemConfig->bUseRawModeComptaglineAllocation &&
        pKernelMemorySystem->bDisablePlcForCertainOffsetsBug3046774)
    {
        MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

        if (((vaspaceGetFlags(pVAS) & VASPACE_FLAGS_FLA) || (dynamicCast(pVAS, FABRIC_VASPACE) != NULL)) &&
            memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, pLocals->kind) &&
            !memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_DISALLOW_PLC, pLocals->kind))
        {
            memmgrGetDisablePlcKind_HAL(pMemoryManager, &pLocals->kind);
        }
    }

    if (pLocals->bIsBarOrPerf)
    {
        pLocals->totalVaRange = rangeMake(vaspaceGetVaStart(pVAS), vaspaceGetVaLimit(pVAS));

        // !!!! Nasty hack
        //
        // NVOS46_FLAGS_PTE_COALESCE_LEVEL_CAP used to get the encryption info from _busMapAperture_GF100().
        // Since we have no bit fields left in NVOS46_FLAGS_* to specify encryption info.
        // This is applicable to FERMI+ chips.
        //
        // NVOS46_FLAGS_PTE_COALESCE_LEVEL_CAP is _NV50 specific, and is not used in FERMI+.
        // NVOS46_FLAGS_PTE_COALESCE_LEVEL_CAP_DEFAULT means use default encryption status
        // NVOS46_FLAGS_PTE_COALESCE_LEVEL_CAP_1       means disable encryption
        //
        // VMM-TODO: Add meaningful alias defines or just expand flag bits?
        //
        pLocals->disableEncryption = FLD_TEST_DRF(OS46, _FLAGS, _PTE_COALESCE_LEVEL_CAP, _1, flags) ?
            DMA_UPDATE_VASPACE_FLAGS_DISABLE_ENCRYPTION : 0;

        if (pLocals->bIsMemContiguous)
        {
            pLocals->overMap = pLocals->pageCount + NvU64_LO32((pLocals->pageOffset + (pLocals->pageSize - 1)) / pLocals->pageSize);
        }
        else
        {
            pLocals->overMap = pLocals->pageCount;
        }

        NV_ASSERT_OK_OR_GOTO(status, vgpuGetCallingContextGfid(pGpu, &gfid), cleanup);

        // BAR1 VA space is split when MIG mem partitioning is enabled
        if (pLocals->bIsBar1 && pLocals->bIsMIGMemPartitioningEnabled && IS_GFID_PF(gfid))
        {
            KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);

            pLocals->totalVaRange = memmgrGetMIGPartitionableBAR1Range(pGpu, pMemoryManager);
            NV_ASSERT_OK_OR_GOTO(status,
                kmemsysSwizzIdToMIGMemRange(pGpu, pKernelMemorySystem, swizzId, pLocals->totalVaRange, &pLocals->totalVaRange),
                cleanup);
        }

        if (!FLD_TEST_DRF(OS46, _FLAGS, _DMA_OFFSET_FIXED, _TRUE, flags))
        {
            pLocals->vaRangeLo = pLocals->totalVaRange.lo;
            pLocals->vaRangeHi = pLocals->totalVaRange.hi;
        }
    }
    else
    {

        NvU64 targetSpaceLength, targetSpaceBase, targetSpaceLimit;

        NV_ASSERT((pLocals->pageSize == pLocals->vaspaceBigPageSize) ||
                  (pLocals->pageSize == RM_PAGE_SIZE) ||
                  (pLocals->pageSize == RM_PAGE_SIZE_HUGE) ||
                  (pLocals->pageSize == RM_PAGE_SIZE_512M) ||
                  (pLocals->pageSize == RM_PAGE_SIZE_256G));

        pLocals->overMap = 0;

        if (pCliMapInfo != NULL)
        {
            VirtualMemory     *pVirtualMemory = pCliMapInfo->pVirtualMemory;

            virtmemGetAddressAndSize(pVirtualMemory, &targetSpaceBase, &targetSpaceLength);
            targetSpaceLimit = targetSpaceBase + targetSpaceLength - 1;
        }
        else
        {
            // RM internal mappings. Alt to dmaMapBuffer_HAL()
            targetSpaceBase   = vaspaceGetVaStart(pVAS);
            targetSpaceLimit  = vaspaceGetVaLimit(pVAS);
            targetSpaceLength = targetSpaceLimit - targetSpaceBase + 1;
        }

        if (pLocals->pteCount > ((targetSpaceLength + (pLocals->pageArrayGranularity - 1)) / pLocals->pageArrayGranularity))
        {
            NV_ASSERT(0);
            status = NV_ERR_INVALID_ARGUMENT;
            goto cleanup;
        }

        pVASpaceHeap = vaspaceGetHeap(pVAS);

        if (!FLD_TEST_DRF(OS46, _FLAGS, _DMA_OFFSET_FIXED, _TRUE, flags))
        {
            // offset of the context dma passed in when ctxdma allocated
            // Virtual memory don't have any SMMU mapping. It is still OK to use the engine MMU context; it dont have any effect.

            pLocals->vaRangeLo = NV_MAX(targetSpaceBase, vaspaceGetVaStart(pVAS));
            pLocals->vaRangeHi = NV_MIN(targetSpaceLimit, vaspaceGetVaLimit(pVAS));

            //
            // Handle 32bit pointer requests.  32b pointers are forced below 32b
            // on all chips.  Non-32b requests are only forced on some chips,
            // typically kepler, and only if there are no other address hints.
            //
            if (DRF_VAL(OS46, _FLAGS, _32BIT_POINTER, flags) ==
                NVOS46_FLAGS_32BIT_POINTER_ENABLE)
            {
                pLocals->vaRangeHi = NV_MIN(0xffffffff, pLocals->vaRangeHi);
            }
            else if (pDma->getProperty(pDma, PDB_PROP_DMA_ENFORCE_32BIT_POINTER) &&
                     (pVASpaceHeap->free > NVBIT64(32))) // Pressured address spaces are exempt
            {
                pLocals->vaRangeLo = NV_MAX(NVBIT64(32), pLocals->vaRangeLo);
            }
        }
    }

    //
    // Align the virtual address passed in down to the page size.
    //
    // There is no requirement that the physical offset of a mapping
    // be page-aligned, so we need to map the entire page that contains
    // the desired offset. We then add the page offset
    // onto the returned virtual address.
    //
    pLocals->vaLo = RM_ALIGN_DOWN(*pVaddr, pLocals->pageSize);

    if (pLocals->bAllocVASpace)
    {
        //
        // TODO: This flag handling logic should be consolidated with dmaMapBuffer_GM107
        //       when old path removed.
        //
        VAS_ALLOC_FLAGS allocFlags = {0};
        NvU64           compAlign = NVBIT64(pLocals->comprInfo.compPageShift);
        NvU64           vaAlign   = NV_MAX(pLocals->pageSize, compAlign);
        NvU64           vaSize    = RM_ALIGN_UP(pLocals->mapLength, vaAlign);
        NvU64           pageSizeLockMask = 0;
        pGVAS = dynamicCast(pVAS, OBJGVASPACE);

        if (FLD_TEST_DRF(OS46, _FLAGS, _PAGE_SIZE, _BOTH, flags))
        {
            vaAlign = NV_MAX(vaAlign, pLocals->vaspaceBigPageSize);
            vaSize  = RM_ALIGN_UP(pLocals->mapLength, vaAlign);
        }
        //
        // Third party code path, nvidia_p2p_get_pages, expects on BAR1 VA to be
        // always aligned at 64K.
        //
        // Also, RmMapMemory on PPC64LE expects BAR1 VA to be aligned at 64K.
        //
        if (pLocals->bIsBar1)
        {
            vaAlign = NV_MAX(vaAlign, pLocals->vaspaceBigPageSize);
            vaSize  = RM_ALIGN_UP(pLocals->mapLength, vaAlign);
        }
        if (FLD_TEST_DRF(OS46, _FLAGS, _DMA_OFFSET_FIXED, _TRUE, flags))
        {
            pLocals->vaRangeLo = pLocals->vaLo;
            pLocals->vaRangeHi = pLocals->vaLo + vaSize - 1;
            if (pLocals->bIsBar1)
            {
                NV_RANGE requestedRange = rangeMake(pLocals->vaRangeLo, pLocals->vaRangeHi);
                if (!rangeContains(pLocals->totalVaRange, requestedRange))
                {
                    NV_PRINTF(LEVEL_ERROR, "Requested BAR1 VA Lo=0x%llx Hi=0x%llx\n"
                                           "total BAR1 VA range Lo=0x%llx Hi=0x%llx\n",
                                            requestedRange.lo, requestedRange.hi,
                                            pLocals->totalVaRange.lo, pLocals->totalVaRange.hi);
                    status = NV_ERR_INVALID_ARGUMENT;
                    DBG_BREAKPOINT();
                    goto cleanup;
                }
            }
            if (pGVAS != NULL && gvaspaceIsInternalVaRestricted(pGVAS))
            {
                if ((pLocals->vaRangeLo >= pGVAS->vaStartInternal && pLocals->vaRangeLo <= pGVAS->vaLimitInternal) ||
                    (pLocals->vaRangeHi <= pGVAS->vaLimitInternal && pLocals->vaRangeHi >= pGVAS->vaStartInternal))
                {
                    status = NV_ERR_INVALID_PARAMETER;
                    goto cleanup;
                }
            }
        }
        else if (pDma->bDmaRestrictVaRange)
        {
            // See comments in vaspaceFillAllocParams_IMPL.
            pLocals->vaRangeHi = NV_MIN(pLocals->vaRangeHi, NVBIT64(40) - 1);
        }

        if (FLD_TEST_DRF(OS46, _FLAGS, _PAGE_SIZE, _BOTH, flags))
        {
            NV_ASSERT(pLocals->pageSize <= pLocals->vaspaceBigPageSize);
            pageSizeLockMask |= RM_PAGE_SIZE;
            pageSizeLockMask |= pLocals->vaspaceBigPageSize;
        }
        else
        {
            pageSizeLockMask |= pLocals->pageSize;
        }

        allocFlags.bReverse = FLD_TEST_DRF(OS46, _FLAGS, _DMA_OFFSET_GROWS, _DOWN, flags);

        //
        // Feature requested for RM unlinked SLI:
        // Clients can pass an allocation flag to the device or VA space constructor
        // so that mappings and allocations will fail without an explicit address.
        //
        if (pGVAS != NULL)
        {
            if ((pGVAS->flags & VASPACE_FLAGS_REQUIRE_FIXED_OFFSET) &&
                !FLD_TEST_DRF(OS46, _FLAGS, _DMA_OFFSET_FIXED, _TRUE, flags))
            {
                status = NV_ERR_INVALID_ARGUMENT;
                NV_PRINTF(LEVEL_ERROR, "The VA space requires all allocations to specify a fixed address\n");
                goto cleanup;
            }

            //
            // Bug 3610538 clients can allocate GPU VA, during mapping for ctx dma.
            // But if clients enable RM to map internal buffers in a reserved
            // range of VA for unlinked SLI in Linux, we want to tag these
            // allocations as "client allocated", so that it comes outside of
            // RM internal region.
            //
            if (gvaspaceIsInternalVaRestricted(pGVAS))
            {
                allocFlags.bClientAllocation = NV_TRUE;
            }
        }

        status = vaspaceAlloc(pVAS, vaSize, vaAlign, pLocals->vaRangeLo, pLocals->vaRangeHi,
                              pageSizeLockMask, allocFlags, &pLocals->vaLo);
        if (NV_OK != status)
        {
            NV_PRINTF(LEVEL_ERROR, "can't alloc VA space for mapping.\n");
            goto cleanup;
        }
        NV_ASSERT_OR_ELSE(0 == (pLocals->vaLo & (pLocals->pageSize - 1)),
            status = NV_ERR_INVALID_STATE;
            goto cleanup; );
        NV_ASSERT_OR_ELSE(vaSize >= pLocals->mapLength,
            status = NV_ERR_INVALID_STATE;
            goto cleanup; );

        //
        // Handle overmapping for BAR1.
        //
        // BAR1 VA is allocated at big page size granularity
        // regardless of the physical memory size being mapped.
        // Unmapped regions of BAR1 need to be mapped to dummy
        // pages (or sparse) to avoid faults on PCIe prefetch.
        //
        // Overmap solves this by wrapping around the target physical
        // memory for the remainder of the last big page so
        // any left over 4K pages are "scratch invalidated."
        //
        // When this is used, the mapLength must be extended to
        // to the entire VA range and dmaUpdateVASpace
        // takes care of the overMap modulus.
        //
        // TODO: With VMM enabled BAR1 scratch invalidate is handled
        //       transparently with SW (or HW) sparse support.
        //       Removing this special overmap logic should be
        //       possible when the old VAS path is fully
        //       deprecated.
        //
        // See Bug 200090426.
        //
        if (pLocals->overMap != 0)
        {
            pLocals->mapLength = vaSize;
        }
    }
    else
    {
        //
        // We are mapping to an existing virtual memory allocation.
        //
        // The virtual offset passed in may or may not account for
        // the page offset. Check for either the page-aligned case or
        // the adjusted case to ensure clients are not requesting
        // bogus offsets.
        //
        if (((*pVaddr - pLocals->vaLo) != 0) &&
            ((*pVaddr - pLocals->vaLo) != pLocals->pageOffset))
        {
            NV_PRINTF(LEVEL_ERROR,
                      "Virtual address 0x%llX is not compatible with page size 0x%llX or page"
                      " offset 0x%llX.\n", *pVaddr, pLocals->pageSize, pLocals->pageOffset);
            DBG_BREAKPOINT();
            status = NV_ERR_INVALID_OFFSET;
            goto cleanup;
        }
    }

    //
    // Calculate mapping virtual address limit based on
    // mapping length derived from number of physical pages going to map.
    //
    pLocals->vaHi = pLocals->vaLo + pLocals->mapLength - 1;

    if (pLocals->p2p == NVOS46_FLAGS_P2P_ENABLE_NOSLI)
    {
        NV_ASSERT_OR_GOTO(pLocals->pMemory != NULL, fail_post_register);

        FlaMemory *pFlaMemory = dynamicCast(pLocals->pMemory, FlaMemory);
        if (pFlaMemory != NULL)
        {
            pLocals->pSrcGpu        = gpumgrGetGpu(pFlaMemory->peerGpuInst);
            pLocals->bFlaImport     = NV_TRUE;

            if (!pLocals->pSrcGpu)
            {
                NV_PRINTF(LEVEL_ERROR, "Cannot map FLA Memory without a valid srcGpu, failing....\n");
                status = NV_ERR_INVALID_ARGUMENT;
                DBG_BREAKPOINT();
                goto fail_post_register;
            }
        }
        else
        {
            pLocals->pSrcGpu = pLocals->pMemory->pGpu;

            // XXX - is this required here if we disable SLI BC below?
            GPU_RES_SET_THREAD_BC_STATE(pLocals->pMemory->pDevice);
        }

        if (IsSLIEnabled(pLocals->pSrcGpu))
        {
            NvU32 deviceInstance = gpuGetDeviceInstance(pLocals->pSrcGpu);

            pLocals->pSrcGpu = gpumgrGetGpuFromSubDeviceInst(deviceInstance, pLocals->subDevIdSrc);
            gpumgrSetBcEnabledStatus(pLocals->pSrcGpu, NV_FALSE);
        }

        pLocals->peerNumber = kbusGetPeerId_HAL(pGpu, pKernelBus, pLocals->pSrcGpu);

        // only needed pLocals->pSrcGpu for the one line above, swap back now.
        if (IsSLIEnabled(pLocals->pSrcGpu))
        {
            pLocals->pSrcGpu = gpumgrGetParentGPU(pLocals->pSrcGpu);
            gpumgrSetBcEnabledStatus(pLocals->pSrcGpu, NV_TRUE);
        }

        NV_PRINTF(LEVEL_INFO,
                  "P2P LOOPBACK setup with physical vidmem at 0x%llx and virtual address "
                  "at 0x%llx\n",
                  memdescGetPhysAddr(pAdjustedMemDesc, addressTranslation, 0), pLocals->vaLo);
    }
    else if (pLocals->p2p == NVOS46_FLAGS_P2P_ENABLE_SLI)
    {
        //
        // All the peer GPUs will have valid PTEs written as
        // P2P mappings. The local GPU will have this region marked as
        // invalid.
        //
        const NvU32 deviceInst = gpuGetDeviceInstance(pGpu);
        pLocals->pSrcGpu  = gpumgrGetGpuFromSubDeviceInst(deviceInst, pLocals->subDevIdSrc);
    }

    pLocals->pRootMemDesc = memdescGetRootMemDesc(pAdjustedMemDesc, NULL);
    if (memdescGetAddressSpace(pLocals->pRootMemDesc) == ADDR_FBMEM)
    {
        if (gpumgrCheckIndirectPeer(pGpu, pLocals->pRootMemDesc->pGpu))
        {
            pLocals->indirectPeer = DMA_UPDATE_VASPACE_FLAGS_INDIRECT_PEER;
        }
    }

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
    {
        if (pLocals->p2p)
        {
            if (pLocals->bFlaImport)
            {
                pLocals->pTempMemDesc = memdescGetMemDescFromGpu(pAdjustedMemDesc, pGpu);
            }
            else
            {
                pLocals->pTempMemDesc = memdescGetMemDescFromGpu(pAdjustedMemDesc, pLocals->pSrcGpu);
            }
        }
        else
        {
            pLocals->pTempMemDesc = memdescGetMemDescFromGpu(pAdjustedMemDesc, pGpu);
        }

        // Commit the mapping update
        pLocals->pPteArray = memdescGetPteArray(pLocals->pTempMemDesc, addressTranslation);

        dmaPageArrayInitWithFlags(&pLocals->pageArray, pLocals->pPteArray, pLocals->pteCount,
                                  pLocals->pageArrayFlags);

        // Get pLocals->aperture
        if (memdescGetAddressSpace(pLocals->pTempMemDesc) == ADDR_FBMEM)
        {
            if (pLocals->p2p)
            {
                pLocals->aperture = NV_MMU_PTE_APERTURE_PEER_MEMORY;
            }
            else if (pLocals->indirectPeer)
            {
                pLocals->aperture = NV_MMU_PTE_APERTURE_SYSTEM_COHERENT_MEMORY;
            }
            else
            {
                pLocals->aperture = NV_MMU_PTE_APERTURE_VIDEO_MEMORY;
            }
        }
        else if (
                 (memdescGetAddressSpace(pLocals->pTempMemDesc) == ADDR_FABRIC_MC) ||
                 (memdescGetAddressSpace(pLocals->pTempMemDesc) == ADDR_FABRIC_V2))
        {
            OBJGPU *pMappingGpu = pGpu;
            OBJGPU *pPeerGpu;
            pLocals->peerNumber = BUS_INVALID_PEER;

            if (pLocals->pMemory == NULL)
            {
                status = NV_ERR_INVALID_STATE;
                DBG_BREAKPOINT();
                SLI_LOOP_BREAK;
            }

            pPeerGpu = pLocals->pMemory->pGpu;

            if (memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, pLocals->kind))
            {
                NV_PRINTF(LEVEL_ERROR,
                      "Fabric memory should not be compressible.\n");
                status = NV_ERR_INVALID_STATE;
                DBG_BREAKPOINT();
                SLI_LOOP_BREAK;
            }

            pLocals->aperture = NV_MMU_PTE_APERTURE_PEER_MEMORY;

            if (!memIsGpuMapAllowed(pLocals->pMemory, pMappingGpu))
            {
                NV_PRINTF(LEVEL_ERROR,
                          "Mapping Gpu is not attached to the given memory object\n");
                status = NV_ERR_INVALID_STATE;
                DBG_BREAKPOINT();
                SLI_LOOP_BREAK;
            }

            if (pPeerGpu != NULL)
            {
                if (IS_VIRTUAL_WITH_SRIOV(pMappingGpu) &&
                    !gpuIsWarBug200577889SriovHeavyEnabled(pMappingGpu))
                {
                    pLocals->peerNumber = kbusGetNvlinkPeerId_HAL(pMappingGpu,
                                                                GPU_GET_KERNEL_BUS(pMappingGpu),
                                                                pPeerGpu);
                }
                else
                {
                    KernelNvlink *pKernelNvlink = GPU_GET_KERNEL_NVLINK(pMappingGpu);

                    if ((pKernelNvlink != NULL) &&
                        knvlinkIsNvlinkP2pSupported(pMappingGpu, pKernelNvlink, pPeerGpu))
                    {
                        pLocals->peerNumber = kbusGetPeerId_HAL(pMappingGpu, GPU_GET_KERNEL_BUS(pMappingGpu),
                                                            pPeerGpu);
                    }
                }
            }
            else
            {
                pLocals->peerNumber = kbusGetNvSwitchPeerId_HAL(pMappingGpu,
                                                                GPU_GET_KERNEL_BUS(pMappingGpu));
            }

            if (pLocals->peerNumber == BUS_INVALID_PEER)
            {
                status = NV_ERR_INVALID_STATE;
                DBG_BREAKPOINT();
                SLI_LOOP_BREAK;
            }
        }
        else if (memdescIsEgm(pLocals->pTempMemDesc))
        {
            pLocals->aperture = NV_MMU_PTE_APERTURE_PEER_MEMORY;

            if (pLocals->p2p)
            {
                OBJGPU *pMappingGpu = pGpu;
                OBJGPU *pPeerGpu;

                NV_ASSERT_OR_ELSE(pLocals->pMemory != NULL, status = NV_ERR_INVALID_STATE; goto cleanup);

                pPeerGpu = pLocals->pMemory->pGpu;
                pLocals->peerNumber = kbusGetEgmPeerId_HAL(pMappingGpu, GPU_GET_KERNEL_BUS(pMappingGpu), pPeerGpu);
            }
            else
            {
                //
                // Make sure that we receive a mapping request for EGM memory
                // only if local EGM is enabled.
                //
                NV_ASSERT_OR_ELSE(pMemoryManager->bLocalEgmEnabled, status = NV_ERR_INVALID_STATE; goto cleanup);
                pLocals->peerNumber = pMemoryManager->localEgmPeerId;
            }
        }
        else
        {
            // No P2P for system memory
            if (pLocals->p2p)
            {
                status = NV_ERR_INVALID_ARGUMENT;
                NV_PRINTF(LEVEL_ERROR, "No P2P for system memory.\n");
                SLI_LOOP_BREAK;
            }

            if (pLocals->cacheSnoop || memdescGetFlag(pAdjustedMemDesc, MEMDESC_FLAGS_MAP_SYSCOH_OVER_BAR1))
            {
                pLocals->aperture = NV_MMU_PTE_APERTURE_SYSTEM_COHERENT_MEMORY;
            }
            else
            {
                pLocals->aperture = NV_MMU_PTE_APERTURE_SYSTEM_NON_COHERENT_MEMORY;
            }
        }

        if (pLocals->p2p == NVOS46_FLAGS_P2P_ENABLE_SLI)
        {
            if (pLocals->pSrcGpu == pGpu)
            {
                // Leave the local GPU VA range unmapped (invalid).
                SLI_LOOP_CONTINUE;
            }
            else
            {
                pLocals->peerNumber = kbusGetPeerId_HAL(pGpu, GPU_GET_KERNEL_BUS(pGpu), pLocals->pSrcGpu);
            }
        }

        if (pLocals->aperture == NV_MMU_PTE_APERTURE_PEER_MEMORY &&
            pLocals->peerNumber == BUS_INVALID_PEER)
        {
            status = NV_ERR_INVALID_STATE;
            DBG_BREAKPOINT();
            SLI_LOOP_BREAK;
        }

        if ((gpuIsSriovEnabled(pGpu) || IS_VIRTUAL_WITH_SRIOV(pGpu)) &&
            (pLocals->aperture == NV_MMU_PTE_APERTURE_VIDEO_MEMORY) &&
            (pLocals->pageSize > gpuGetVmmuSegmentSize(pGpu)))
        {
            NV_CHECK_FAILED(LEVEL_ERROR, "Vidmem page size is limited by VMMU segment size when GPU is in SRIOV mode");
            status = NV_ERR_INVALID_ARGUMENT;
            SLI_LOOP_BREAK;
        }

        //
        // Fabric memory descriptors are pre-encoded with the fabric base address
        // use NVLINK_INVALID_FABRIC_ADDR to avoid encoding twice
        //
        // Skip fabric base address for Local EGM as it uses peer aperture but
        // doesn't require fabric address
        //
        if (pLocals->bFlaImport ||
            (memdescGetAddressSpace(pLocals->pTempMemDesc) == ADDR_FABRIC_MC) ||
            (memdescGetAddressSpace(pLocals->pTempMemDesc) == ADDR_FABRIC_V2) ||
            (memdescIsEgm(pLocals->pTempMemDesc) && (pGpu == pLocals->pSrcGpu)))
        {
            pLocals->fabricAddr = NVLINK_INVALID_FABRIC_ADDR;
        }
        else if ((GPU_GET_KERNEL_NVLINK(pGpu) != NULL) &&
                 !gpuFabricProbeIsSupported(pGpu) &&
                 (knvlinkGetIPVersion(pGpu, GPU_GET_KERNEL_NVLINK(pGpu)) >= NVLINK_VERSION_50))
        {
            //
            // on NVL5 direct connect systems we need to use the peerId << 42 as
            // the fabric offset
            //
            pLocals->fabricAddr = (((NvU64)pLocals->peerNumber) << NVLINK_NODE_REMAP_OFFSET_SHIFT);
        }
        else
        {
            // Get EGM fabric address for Remote EGM
            if (memdescIsEgm(pLocals->pTempMemDesc))
            {
                status = _dmaGetFabricEgmAddress(pLocals->pSrcGpu, pLocals->aperture,
                                                pLocals->kind, &pLocals->fabricAddr);
            }
            else
            {
                status = _dmaGetFabricAddress(pLocals->pSrcGpu, pLocals->aperture,
                                                pLocals->kind, &pLocals->fabricAddr);
            }

            if (status != NV_OK)
            {
                DBG_BREAKPOINT();
                SLI_LOOP_BREAK;
            }
        }

        pDma = GPU_GET_DMA(pGpu);

        status = dmaUpdateVASpace_HAL(pGpu, pDma,
                                      pVAS,
                                      pLocals->pTempMemDesc,
                                      NULL,
                                      pLocals->vaLo, pLocals->vaHi,
                                      DMA_UPDATE_VASPACE_FLAGS_UPDATE_ALL | pLocals->readOnly | pLocals->priv |
                                          pLocals->tlbLock | pLocals->shaderFlags | pLocals->disableEncryption | pLocals->indirectPeer,
                                     &pLocals->pageArray, pLocals->overMap,
                                     &pLocals->comprInfo,
                                      0,
                                      NV_MMU_PTE_VALID_TRUE,
                                      pLocals->aperture,
                                      pLocals->peerNumber,
                                      pLocals->fabricAddr,
                                      pLocals->deferInvalidate,
                                      NV_FALSE,
                                      pLocals->pageSize);
        if (NV_OK != status)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "can't update VA space for mapping @vaddr=0x%llx\n",
                      pLocals->vaLo);
            DBG_BREAKPOINT();
            SLI_LOOP_BREAK;
        }
    }
    SLI_LOOP_END

    if (NV_OK == status)
    {
        //
        // Fill in the final virtual address of this mapping.
        //
        // This accounts for page offset for all cases, whether or not
        // the input *pVaddr accounted for it.
        //
        *pVaddr = pLocals->vaLo + pLocals->pageOffset;

        // Fill in the final mapping page size for client mappings.
        if (pCliMapInfo != NULL)
        {
            pCliMapInfo->pDmaMappingInfo->mapPageSize = pLocals->pageSize;
        }

        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
        // This is needed for cliDB tracking of the map.
        memdescSetPageSize(memdescGetMemDescFromGpu(pAdjustedMemDesc, pGpu), addressTranslation, pLocals->pageSize);
        SLI_LOOP_END
    }
    else
    {
fail_post_register:
        if (pLocals->pMapNode)
            btreeUnlink(pLocals->pMapNode, &pLocals->pVASInfo->pMapTree);

        portMemFree(pLocals->pMapNode);

        // Only free the VA allocation if we allocated here.
        if (pLocals->bAllocVASpace)
        {
            vaspaceFree(pVAS, pLocals->vaLo);
        }
    }

cleanup:

    if (pAdjustedMemDesc != pMemDesc)
        fabricvaspacePutGpaMemdesc(pFabricVAS, pAdjustedMemDesc);

    portMemFree(pLocals);

    return status;
}

/*!
 * @brief Unmap a virtual allocation.
 *
 * For client allocations, invalidate the page tables, but don't bother freeing.
 * For internal allocations, free the allocation, but don't bother invalidating.
 * Wait, what?
 *
 * VMM-TODO: Split into two APIs - one for clients one for internal?
 *
 * @param[in] pGpu          OBJGPU pointer
 * @param[in] pDma          VirtMemAllocator pointer
 * @param[in] pVAS          OBJVASPACE pointer
 * @param[in] vAddr         Virtual memory base address
 * @param[in] pMemDesc      Physical memory descriptor
 * @param[in] flags         Unmap options
 * @param[in] pCliMapInfo   PCLI_DMA_ALLOC_MAP_INFO pointer (for RM Client mappings)
 *
 * @returns NV_STATUS status = NV_OK on success, or status upon failure.
 */
NV_STATUS
dmaFreeMapping_GM107
(
    OBJGPU             *pGpu,
    VirtMemAllocator   *pDma,
    OBJVASPACE         *pVAS,
    NvU64               vAddr,
    MEMORY_DESCRIPTOR  *pMemDesc,
    NvU32               flags,
    CLI_DMA_ALLOC_MAP_INFO *pCliMapInfo
)
{
    VirtualMemory *pVirtualMemory = NULL;
    NvU32          p2p = NVOS46_FLAGS_P2P_ENABLE_NONE;
    NvU64          vaLo;
    NvU64          vaHi;
    NvU64          mapLength;
    NvU64          pageOffset;
    NvU64          pageSize;
    NvU32          deferInvalidate;
    NvU32          subDevIdSrc;
    OBJGPU        *pLocalGpu = NULL;

    NV_STATUS status = NV_OK;
    MEMORY_DESCRIPTOR *pTempMemDesc    = NULL;

    NV_ASSERT_OR_RETURN(NULL != pMemDesc, NV_ERR_INVALID_ARGUMENT);

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
        // ensure the page size has been set before continuing
        NV_ASSERT(memdescGetPageSize(memdescGetMemDescFromGpu(pMemDesc, pGpu), VAS_ADDRESS_TRANSLATION(pVAS)) != 0);
    SLI_LOOP_END

    if (pCliMapInfo)
    {
        p2p            = DRF_VAL(OS46, _FLAGS, _P2P_ENABLE, pCliMapInfo->pDmaMappingInfo->Flags);
        subDevIdSrc    = DRF_VAL(OS46, _FLAGS, _P2P_SUBDEV_ID_SRC, pCliMapInfo->pDmaMappingInfo->Flags);
        pVirtualMemory = pCliMapInfo->pVirtualMemory;
    }

    if (p2p == NVOS46_FLAGS_P2P_ENABLE_SLI)
    {
        const NvU32 deviceInst = gpuGetDeviceInstance(pGpu);
        pLocalGpu  = gpumgrGetGpuFromSubDeviceInst(deviceInst, subDevIdSrc);
    }

    deferInvalidate = DRF_VAL(OS47, _FLAGS, _DEFER_TLB_INVALIDATION, flags) ? DMA_DEFER_TLB_INVALIDATE : DMA_TLB_INVALIDATE;

    // Handle NV50_MEMORY_VIRTUAL use case
    if ((pVirtualMemory != NULL) && pVirtualMemory->bReserveVaOnAlloc)
    {
        SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
        {
            if (p2p == NVOS46_FLAGS_P2P_ENABLE_SLI)
            {
                if (pLocalGpu == pGpu)
                {
                    SLI_LOOP_CONTINUE;
                }
            }

            pTempMemDesc = memdescGetMemDescFromGpu(pMemDesc, pGpu);

            NV_ASSERT_OR_RETURN(pCliMapInfo != NULL, NV_ERR_INVALID_STATE);
            NV_ASSERT_OR_RETURN(pCliMapInfo->pDmaMappingInfo->mapPageSize != 0, NV_ERR_INVALID_STATE);

            pageSize     = pCliMapInfo->pDmaMappingInfo->mapPageSize;
            pageOffset   = memdescGetPhysAddr(pTempMemDesc, VAS_ADDRESS_TRANSLATION(pVAS), 0) & (pageSize - 1);
            mapLength    = RM_ALIGN_UP(pageOffset + pTempMemDesc->Size, pageSize);
            vaLo         = RM_ALIGN_DOWN(vAddr, pageSize);
            vaHi         = vaLo + mapLength - 1;

            pDma = GPU_GET_DMA(pGpu);
            if (vaspaceGetFlags(pVAS) & VASPACE_FLAGS_BAR_BAR1)
            {
                NV_PRINTF(LEVEL_ERROR, "Using dmaFreeMapping with sparse == False in BAR1 path!\n");
                NV_ASSERT(0);
                return status;
            }

            status = dmaUpdateVASpace_HAL(pGpu, pDma,
                                          pVAS,
                                          pTempMemDesc,
                                          NULL,
                                          vaLo, vaHi,
                                          DMA_UPDATE_VASPACE_FLAGS_UPDATE_VALID, // only change validity
                                          NULL, 0,
                                          NULL, 0,
                                          NV_MMU_PTE_VALID_FALSE,
                                          kgmmuGetHwPteApertureFromMemdesc(GPU_GET_KERNEL_GMMU(pGpu), pTempMemDesc), 0,
                                          NVLINK_INVALID_FABRIC_ADDR,
                                          deferInvalidate,
                                          NV_FALSE,
                                          pageSize);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR, "error updating VA space.\n");
                vaspaceFree(pVAS, vaLo);
                return status;
            }
        }
        SLI_LOOP_END
    }
    else
    {
        vaspaceFree(pVAS, vAddr);
    }

    //
    // invalidate any cached peer data if this memory was mapped p2p cached.
    // for SLI case - kmemsysCacheOp would loop through all GPUs
    // for non-SLI case pGpu is pointing to the P2P mapped GPU would
    // invalidate only on that GPU.
    //
    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY | SLI_LOOP_FLAGS_IGNORE_REENTRANCY)
        if ((memdescGetGpuP2PCacheAttrib(memdescGetMemDescFromGpu(pMemDesc, pGpu)) == NV_MEMORY_CACHED) &&
            (p2p != NVOS46_FLAGS_P2P_ENABLE_NONE))
        {
            KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
            kmemsysCacheOp_HAL(pGpu, pKernelMemorySystem, pMemDesc, FB_CACHE_PEER_MEMORY,
                               FB_CACHE_INVALIDATE);
        }
    SLI_LOOP_END

    return status;
}

/*!
 * Defines the data needed to iterate over the last level duing map VA op.
 * Note: Used only in the new VMM code path.
 */
struct MMU_MAP_ITERATOR
{
    /*!
     * @copydoc GMMU_FMT
     */
    const GMMU_FMT  *pFmt;

    /*!
     * Physical aperture of the pages.
     */
    GMMU_APERTURE    aperture;

    /*!
     * Opaque array of physical memory to map. Always points to 4K sized pages.
     */
    DMA_PAGE_ARRAY  *pPageArray;

    /*!
     * Points to the index in pPageArray that needs to be mapped.
     */
    NvU32            currIdx;

    /*!
     * Base offset in bytes into the logical surface being mapped.
     */
    NvU64            surfaceOffset;

    /*!
     * Physical address of the last page mapped.
     */
    NvU64            physAddr;

    /*!
     * NvLink fabric address. Used for NVSwitch systems only!
     */
    NvU64            fabricAddr;

    /*!
     * @copydoc COMPR_INFO
     */
    COMPR_INFO       comprInfo;


    /*!
     * Non-compressed kind.
     */
    NvU32            kindNoCompr;

    /*!
     * Indicates whether compression is enabled.
     */
    NvBool           bCompr;

    /*!
     * Template used to initialize the actual PTEs. Will have values that do not
     * change across one map operation.
     */
    NvU8             pteTemplate[GMMU_FMT_MAX_ENTRY_SIZE] NV_ALIGN_BYTES(8);

    /*!
     * The addr field that needs to be filled out, based on the
     * aperture.
     */
    const GMMU_FIELD_ADDRESS *pAddrField;

    /*!
     * Indicates after how many indexes in pPageArray, should the
     * map wrap around to the first mapped page.
     */
    NvU32 overMapModulus;

    /*!
     * Indicates to read-modify-write each PTE instead of
     * using the pteTemplate as the base value.
     */
    NvBool bReadPtes;

    /*!
     * Indicates to update physical address field of each PTE.
     */
    NvBool bUpdatePhysAddr;

    /*!
     * Indicates to update comptag line and kind of each PTE
     * that points to a compressed page.
     */
    NvBool bUpdateCompr;

    /*!
     * Indicates that we are writing PDEs for Bug 2720120.
     * Applicable only to GA100
     */
    NvBool bApplyWarForBug2720120;

    /*!
     * Current page table BAR2 aperture mapping (or user buffer).
     */
    NvU8 *pMap;
};

static void
_gmmuWalkCBMapNextEntries_Direct
(
    MMU_WALK_USER_CTX        *pUserCtx,
    const MMU_MAP_TARGET     *pTarget,
    const MMU_WALK_MEMDESC   *pLevelMem,
    const NvU32               entryIndexLo,
    const NvU32               entryIndexHi,
    NvU32                    *pProgress
)
{
    NvU32                i;
    const MMU_FMT_LEVEL *pLevelFmt = pTarget->pLevelFmt;
    MMU_MAP_ITERATOR    *pIter     = pTarget->pIter;
    NvU8                *pMap      = pIter->pMap;
    const NvU64          pageSize  = mmuFmtLevelPageSize(pLevelFmt);
    GMMU_ENTRY_VALUE     entry;

    NV_ASSERT_OR_RETURN_VOID(pMap != NULL);

    //
    // This function will always write the caller supplied buffer
    // at offset 0. The onus of writing the buffer out to the target
    // location in memory at the appropriate offset is on the caller.
    //

    for (i = entryIndexLo; i <= entryIndexHi; ++i)
    {
        NvU32 entryOffset = (i - entryIndexLo) * pLevelFmt->entrySize;

        // Copy out current PTE if we are overwriting (Read-Modify-Write)
        if (pIter->bReadPtes)
        {
            portMemCopy(entry.v8, pLevelFmt->entrySize,
                        &pMap[entryOffset], pLevelFmt->entrySize);
        }
        else
        {
            // Copy the static fields passed in, if we aren't overwriting a subset of fields.
            portMemCopy(entry.v8, pLevelFmt->entrySize,
                        pIter->pteTemplate, pLevelFmt->entrySize);
        }

        if (pIter->bApplyWarForBug2720120)
        {
            // Commit to memory.
            portMemCopy(&pMap[entryOffset], pLevelFmt->entrySize,
                        entry.v8, pLevelFmt->entrySize);
            continue;
        }

        // Calculate the new physical address for the compression check below.
        if (pIter->bUpdatePhysAddr)
        {
            NvU32 currIdxMod = pIter->currIdx;

            // Wrap the curr idx to the start offset for BAR1 overmapping.
            if (0 != pIter->overMapModulus)
            {
                currIdxMod %= pIter->overMapModulus;
            }

            // Extract the physical address of the page to map.
            if (currIdxMod < pIter->pPageArray->count)
            {
                pIter->physAddr = dmaPageArrayGetPhysAddr(pIter->pPageArray, currIdxMod);
                // Hack to WAR submemesc mappings
                pIter->physAddr = NV_ALIGN_DOWN64(pIter->physAddr, pageSize);
            }
            else
            {
                //
                // Physically contiguous just increments physAddr
                // Should not be the first page (currIdxMod == 0) being mapped.
                //
                NV_ASSERT_OR_RETURN_VOID((pIter->pPageArray->count == 1) &&
                                       (currIdxMod > 0));
                pIter->physAddr += pageSize;
            }
        }

        // Init comptag
        if (pIter->bUpdateCompr)
        {
            OBJGPU        *pGpu           = pUserCtx->pGpu;
            MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
            NvBool         bCompressible  = NV_TRUE;

            //
            // Check if the FB physical address lands in a segment that
            // supports compression.
            // On WDDM neither RM or KMD has the physical address
            // information at compression allocation time.
            // On non-WDDM platforms, RM allocates compression before the
            // actual physical allocation. For non-contig allocations, the
            // physical pages can be spread across multiple regions
            // Therefore compression tags are always allocated and compression must
            // be disabled on a per-PTE basis at map time.
            //
            if ((pMemoryManager->Ram.numFBRegions > 1) &&
                (gmmuFieldGetAperture(&pIter->pFmt->pPte->fldAperture, entry.v8) ==
                 GMMU_APERTURE_VIDEO))
            {
                NvU32 iRegion;
                // Find the region in which the candidate block resides
                for (iRegion = 0; iRegion < pMemoryManager->Ram.numFBRegions; iRegion++)
                {
                    // Does the block resides within this region?  If so, then we are done searching.
                    if ((pIter->physAddr >= pMemoryManager->Ram.fbRegion[iRegion].base) &&
                        (pIter->physAddr <= pMemoryManager->Ram.fbRegion[iRegion].limit))
                    {
                        // Check if the region supports compression
                        bCompressible = pMemoryManager->Ram.fbRegion[iRegion].bSupportCompressed;
                        break;
                    }
                }
            }

            //
            // TODO: The flags that enable compression are confusing -
            //       complicated by memsysReleaseReacquireCompr_GF100 usage.
            //       Clean this up when removing old path and simplifying
            //       the primitive "map" interface.
            //
            if (pIter->bCompr && bCompressible)
            {
                //
                // For VF, HW does 1 to 1 FB-comptag mapping. HW manages comptag
                // allocation, hence RM can skip the comptagline assignment to PTE.
                // Just updating the compressed kind is sufficient for VF.
                //
                if (!IS_VIRTUAL_WITH_SRIOV(pGpu) && pIter->pFmt->version <= GMMU_FMT_VERSION_2)
                {
                    NvBool bIsWarApplied = NV_FALSE;
                    NvU32  savedKind = pIter->comprInfo.kind;
                    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
                    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
                        kmemsysGetStaticConfig(pGpu, pKernelMemorySystem);

                    if (pMemorySystemConfig->bUseRawModeComptaglineAllocation &&
                        pKernelMemorySystem->bDisablePlcForCertainOffsetsBug3046774 &&
                        !memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_DISALLOW_PLC, pIter->comprInfo.kind) &&
                        !kmemsysIsPagePLCable_HAL(pGpu, pKernelMemorySystem, (pIter->surfaceOffset + pIter->currIdx * pTarget->pageArrayGranularity), pageSize))
                    {
                        bIsWarApplied = NV_TRUE;
                        memmgrGetDisablePlcKind_HAL(pMemoryManager, &pIter->comprInfo.kind);
                    }
                    kgmmuFieldSetKindCompTags(GPU_GET_KERNEL_GMMU(pGpu), pIter->pFmt, pLevelFmt, &pIter->comprInfo, pIter->physAddr,
                                              pIter->surfaceOffset + pIter->currIdx * pTarget->pageArrayGranularity,
                                              i, entry.v8);
                    //
                    // restore the kind to PLC if changd, since kind is associated with entire surface, and the WAR applies to
                    // individual pages in the surface.
                    //
                    if (bIsWarApplied)
                        pIter->comprInfo.kind = savedKind;
                }
                else
                {
                    nvFieldSet32(&pIter->pFmt->pPte->fldKind, pIter->comprInfo.kind, entry.v8);
                }
            }
            else
            {
                nvFieldSet32(&pIter->pFmt->pPte->fldKind, pIter->kindNoCompr, entry.v8);

                if (pIter->pFmt->version <= GMMU_FMT_VERSION_2)
                {
                    nvFieldSet32(&pIter->pFmt->pPte->fldCompTagLine, 0, entry.v8);
                    if (nvFieldIsValid32(&pIter->pFmt->pPte->fldCompTagSubIndex))
                    {
                        nvFieldSet32(&pIter->pFmt->pPte->fldCompTagSubIndex, 0, entry.v8);
                    }
                }
            }
        }

        // Fill the physical address field.
        if (pIter->bUpdatePhysAddr && (pIter->pAddrField != NULL))
        {
            // Update the pte with the physical address
            gmmuFieldSetAddress(pIter->pAddrField,
                kgmmuEncodePhysAddr(GPU_GET_KERNEL_GMMU(pUserCtx->pGpu), pIter->aperture, pIter->physAddr,
                    pIter->fabricAddr),
                entry.v8);
        }

        // Commit to memory.
        portMemCopy(&pMap[entryOffset], pLevelFmt->entrySize,
                    entry.v8, pLevelFmt->entrySize);

        //
        // pPageArray deals in 4K pages.
        // So increment by the ratio of mapping page size to 4K
        // --
        // The above assumption will be invalid upon implementation of memdesc dynamic page arrays
        //
        pIter->currIdx += (NvU32)(pageSize / pTarget->pageArrayGranularity);
    }

    *pProgress = entryIndexHi - entryIndexLo + 1;
}

static void
_gmmuWalkCBMapNextEntries_RmAperture
(
    MMU_WALK_USER_CTX        *pUserCtx,
    const MMU_MAP_TARGET     *pTarget,
    const MMU_WALK_MEMDESC   *pLevelMem,
    const NvU32               entryIndexLo,
    const NvU32               entryIndexHi,
    NvU32                    *pProgress
)
{
    OBJGPU              *pGpu           = pUserCtx->pGpu;
    MemoryManager       *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    MMU_MAP_ITERATOR    *pIter          = pTarget->pIter;
    MEMORY_DESCRIPTOR   *pMemDesc       = (MEMORY_DESCRIPTOR*)pLevelMem;
    const MMU_FMT_LEVEL *pLevelFmt      = pTarget->pLevelFmt;
    TRANSFER_SURFACE     surf           = {0};
    NvU32                sizeOfEntries;

    NV_PRINTF(LEVEL_INFO, "[GPU%u]: PA 0x%llX, Entries 0x%X-0x%X\n",
              pUserCtx->pGpu->gpuInstance,
              memdescGetPhysAddr(pMemDesc, AT_GPU, 0), entryIndexLo,
              entryIndexHi);

    surf.pMemDesc = pMemDesc;
    surf.offset = entryIndexLo * pLevelFmt->entrySize;

    sizeOfEntries = (entryIndexHi - entryIndexLo + 1 ) * pLevelFmt->entrySize;

    pIter->pMap = memmgrMemBeginTransfer(pMemoryManager, &surf, sizeOfEntries,
                                         TRANSFER_FLAGS_SHADOW_ALLOC |
                                         TRANSFER_FLAGS_SHADOW_INIT_MEM);
    NV_ASSERT_OR_RETURN_VOID(NULL != pIter->pMap);

    _gmmuWalkCBMapNextEntries_Direct(pUserCtx, pTarget, pLevelMem,
                                     entryIndexLo, entryIndexHi, pProgress);

    memmgrMemEndTransfer(pMemoryManager, &surf, sizeOfEntries,
                         TRANSFER_FLAGS_SHADOW_ALLOC |
                         TRANSFER_FLAGS_SHADOW_INIT_MEM);
}

static NV_STATUS _dmaGetFabricAddress
(
    OBJGPU         *pGpu,
    NvU32           aperture,
    NvU32           kind,
    NvU64           *fabricAddr
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelNvlink  *pKernelNvlink  = GPU_GET_KERNEL_NVLINK(pGpu);

    *fabricAddr = NVLINK_INVALID_FABRIC_ADDR;

    if (pKernelNvlink == NULL)
    {
        return NV_OK;
    }

    if (aperture != NV_MMU_PTE_APERTURE_PEER_MEMORY)
    {
        return NV_OK;
    }

    //
    // Fabric address should be available for NVSwitch connected GPUs,
    // otherwise it is a NOP.
    //
    *fabricAddr = knvlinkGetUniqueFabricBaseAddress(pGpu, pKernelNvlink);
    if (*fabricAddr == NVLINK_INVALID_FABRIC_ADDR)
    {
        return NV_OK;
    }

    if (memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, kind))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Nvswitch systems don't support compression.\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    return NV_OK;
}

static NV_STATUS _dmaGetFabricEgmAddress
(
    OBJGPU         *pGpu,
    NvU32           aperture,
    NvU32           kind,
    NvU64           *fabricEgmAddr
)
{
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    KernelNvlink  *pKernelNvlink  = GPU_GET_KERNEL_NVLINK(pGpu);

    *fabricEgmAddr = NVLINK_INVALID_FABRIC_ADDR;

    if (pKernelNvlink == NULL)
    {
        return NV_OK;
    }

    if (aperture != NV_MMU_PTE_APERTURE_PEER_MEMORY)
    {
        return NV_OK;
    }

    //
    // Fabric address should be available for NVSwitch connected GPUs,
    // otherwise it is a NOP.
    //
    *fabricEgmAddr = knvlinkGetUniqueFabricEgmBaseAddress(pGpu, pKernelNvlink);
    if (*fabricEgmAddr == NVLINK_INVALID_FABRIC_ADDR)
    {
        return NV_OK;
    }

    if (memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, kind))
    {
        NV_PRINTF(LEVEL_ERROR,
                  "Nvswitch systems don't support compression.\n");
        return NV_ERR_NOT_SUPPORTED;
    }

    return NV_OK;
}

// VMM-TODO: PL(N) mmuPageLevelUpdate - but major splits
NV_STATUS
dmaUpdateVASpace_GF100
(
    OBJGPU     *pGpu,
    VirtMemAllocator *pDma,
    OBJVASPACE *pVAS,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU8       *pTgtPteMem,                // CPU pointer to PTE memory for Vista updates
    NvU64       vAddr,
    NvU64       vAddrLimit,
    NvU32       flags,
    DMA_PAGE_ARRAY *pPageArray,
    NvU32       overmapPteMod,
    COMPR_INFO *pComprInfo,
    NvU64       surfaceOffset,
    NvU32       valid,
    NvU32       aperture,
    NvU32       peer,
    NvU64       fabricAddr,
    NvU32       deferInvalidate,
    NvBool      bSparse,
    NvU64       pageSize
)
{
    KernelBus     *pKernelBus = GPU_GET_KERNEL_BUS(pGpu);
    MemoryManager *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);
    NvBool      readPte = NV_FALSE;
    NV_STATUS   status = NV_OK;
    NvBool      isVolatile = NV_TRUE;
    NvU32       encrypted = 0;
    NvU32       tlbLock;
    NvU32       readOnly;
    NvU32       priv;
    NvU32       writeDisable;
    NvU32       readDisable;
    NvU64       vaSpaceBigPageSize = 0;
    KernelMemorySystem *pKernelMemorySystem = GPU_GET_KERNEL_MEMORY_SYSTEM(pGpu);
    const MEMORY_SYSTEM_STATIC_CONFIG *pMemorySystemConfig =
        kmemsysGetStaticConfig(pGpu, pKernelMemorySystem);
    NvU32       alignSize = pMemorySystemConfig->comprPageSize;
    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    NvBool      bFillPteMem = !!(flags & DMA_UPDATE_VASPACE_FLAGS_FILL_PTE_MEM);
    NvBool      bUnmap = !bFillPteMem &&
                         (flags & DMA_UPDATE_VASPACE_FLAGS_UPDATE_VALID) &&
                         (SF_VAL(_MMU, _PTE_VALID, valid) == NV_MMU_PTE_VALID_FALSE);
    NvBool      bIsIndirectPeer;
    VAS_PTE_UPDATE_TYPE update_type;

    {
        OBJGVASPACE *pGVAS = dynamicCast(pVAS, OBJGVASPACE);
        if (bFillPteMem &&
            (pGVAS->flags & VASPACE_FLAGS_BAR_BAR1) &&
            (flags & DMA_UPDATE_VASPACE_FLAGS_UPDATE_VALID) &&
            (SF_VAL(_MMU, _PTE_VALID, valid) == NV_MMU_PTE_VALID_FALSE))
        {
            bSparse = NV_TRUE;
        }
    }

    priv = (flags & DMA_UPDATE_VASPACE_FLAGS_PRIV) ? NV_MMU_PTE_PRIVILEGE_TRUE : NV_MMU_PTE_PRIVILEGE_FALSE;
    tlbLock = (flags & DMA_UPDATE_VASPACE_FLAGS_TLB_LOCK) ? NV_MMU_PTE_LOCK_TRUE : NV_MMU_PTE_LOCK_FALSE;
    readOnly = (flags & DMA_UPDATE_VASPACE_FLAGS_READ_ONLY) ? NV_MMU_PTE_READ_ONLY_TRUE : NV_MMU_PTE_READ_ONLY_FALSE;
    writeDisable = !!(flags & DMA_UPDATE_VASPACE_FLAGS_SHADER_READ_ONLY);
    readDisable = !!(flags & DMA_UPDATE_VASPACE_FLAGS_SHADER_WRITE_ONLY);
    bIsIndirectPeer = !!(flags & DMA_UPDATE_VASPACE_FLAGS_INDIRECT_PEER);

    NV_ASSERT_OR_RETURN(pageSize, NV_ERR_INVALID_ARGUMENT);

    vaSpaceBigPageSize = vaspaceGetBigPageSize(pVAS);
    if ((pageSize == RM_PAGE_SIZE_64K) || (pageSize == RM_PAGE_SIZE_128K))
    {
        NV_ASSERT_OR_RETURN(pageSize == vaSpaceBigPageSize, NV_ERR_INVALID_STATE);
    }

    //
    // Determine whether we are invalidating or revoking privileges, so we know
    // whether to flush page accesses or not. ReadDisable and writeDisable have
    // been deprecated Pascal+, and we don't have the capability to guarantee
    // coherency post TLB invalidate on pre-Pascal, so we ignore them here.
    //
    update_type = (bUnmap || (NV_MMU_PTE_LOCK_FALSE == tlbLock)
                    || (NV_MMU_PTE_READ_ONLY_TRUE == readOnly)) ? PTE_DOWNGRADE : PTE_UPGRADE;

    if (pMemDesc == NULL)
    {
        NV_ASSERT(pMemDesc);
        return NV_ERR_INVALID_ARGUMENT;
    }

    switch (aperture)
    {
        case NV_MMU_PTE_APERTURE_PEER_MEMORY:
            isVolatile = (memdescGetGpuP2PCacheAttrib(pMemDesc) == NV_MEMORY_UNCACHED) ? NV_TRUE : NV_FALSE;
                break;
        case NV_MMU_PTE_APERTURE_SYSTEM_COHERENT_MEMORY:
        case NV_MMU_PTE_APERTURE_SYSTEM_NON_COHERENT_MEMORY:
                if (bIsIndirectPeer)
                    isVolatile = (memdescGetGpuP2PCacheAttrib(pMemDesc) == NV_MEMORY_UNCACHED) ? NV_TRUE : NV_FALSE;
                else
                    isVolatile = (memdescGetGpuCacheAttrib(pMemDesc) == NV_MEMORY_UNCACHED) ? NV_TRUE : NV_FALSE;

                break;
        default:
        case NV_MMU_PTE_APERTURE_VIDEO_MEMORY:
                isVolatile = NV_FALSE;
                break;
    }

    encrypted = (flags & DMA_UPDATE_VASPACE_FLAGS_DISABLE_ENCRYPTION) ? 0 :
        memdescGetFlag(pMemDesc, MEMDESC_FLAGS_ENCRYPTED);

    // Check this here so we don't have to in the loop(s) below as necessary.
    if ((flags & DMA_UPDATE_VASPACE_FLAGS_UPDATE_PADDR) && (pPageArray == NULL))
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    //
    // Must get some attrs from existing PTE and Only if PTE is
    // going to be invalidated, no need to read it
    //
    if (DMA_UPDATE_VASPACE_FLAGS_UPDATE_ALL != (flags & DMA_UPDATE_VASPACE_FLAGS_UPDATE_ALL))
    {
        readPte = !((flags & DMA_UPDATE_VASPACE_FLAGS_UPDATE_VALID) &&
                    (SF_VAL(_MMU, _PTE_VALID, valid) == NV_MMU_PTE_VALID_FALSE));
    }

    //
    // Compressed surfaces must be aligned to the compression page size
    // but don't check for LDDM which may pass in unaligned surfaces incrementally.
    // Chips that support full comp tag lines will support the VA being aligned to
    // the big page size. This is because the PA alignement chooses between even/odd pages
    // and SW programs the PA alignment.
    //
    if (pDma->getProperty(pDma, PDB_PROP_DMA_ENABLE_FULL_COMP_TAG_LINE))
    {
        alignSize = vaSpaceBigPageSize;
    }

    //
    // If we have dynamic granularity page arrays enabled we will never
    // encounter a case where a larger page granularity physical surface gets
    // represented by a smaller granularity pageArray.
    //
    if (!pMemoryManager->bEnableDynamicGranularityPageArrays)
    {
        //
        // VMM-TODO: Merge into PL1 traveral.
        //
        // If the pageSize of the mapping != 4K then be sure that the 4k pages
        // making up the big physical page are contiguous. This is currently
        // necessary since pMemDesc->PteArray is always in terms of 4KB pages.
        // Different large pages do not have to be contiguous with each other.
        // This check isn't needed for contig allocations.
        //
        if (pPageArray && (pageSize != RM_PAGE_SIZE) && (pPageArray->count > 1) &&
        !(flags & DMA_UPDATE_VASPACE_FLAGS_SKIP_4K_PTE_CHECK))
        {
            NvU32 i, j;
            RmPhysAddr pageAddr, pagePrevAddr;

            for (i = 0; i < pPageArray->count; i += j)
            {
                for (j = i + 1; j < pPageArray->count; j++)
                {
                    pagePrevAddr = dmaPageArrayGetPhysAddr(pPageArray, j - 1);
                    pageAddr     = dmaPageArrayGetPhysAddr(pPageArray, j);

                    if ((1 + (pagePrevAddr/(RM_PAGE_SIZE))) !=
                             (pageAddr/(RM_PAGE_SIZE)))
                    {
                        NV_PRINTF(LEVEL_ERROR,
                                 "MMU: given non-contig 4KB pages for %lldkB mapping\n",
                                 pageSize / 1024);
                        DBG_BREAKPOINT();
                        return NV_ERR_GENERIC;
                    }

                    // Are we at the pageSize boundary yet?
                    if ((pageAddr + RM_PAGE_SIZE)
                        % pageSize == 0)
                    {
                        j++;
                        break;
                    }
                }
            }
        }
    }

    // Zero peer on non-peer requests to simplify pte construction
    if (aperture != NV_MMU_PTE_APERTURE_PEER_MEMORY)
    {
        peer = 0;
    }

    MMU_MAP_TARGET   mapTarget = {0};
    MMU_MAP_ITERATOR mapIter   = {0};
    OBJGVASPACE     *pGVAS     = dynamicCast(pVAS, OBJGVASPACE);
    const NvU64      vaLo      = NV_ALIGN_DOWN64(vAddr,                 pageSize);
    const NvU64      vaHi      = NV_ALIGN_DOWN64(vAddrLimit + pageSize, pageSize) - 1;
    GVAS_GPU_STATE  *pGpuState = gvaspaceGetGpuState(pGVAS, pGpu);
    const GMMU_FMT  *pFmt      = pGpuState->pFmt;

    // Enforce unicast.
    NV_ASSERT_OR_RETURN(!gpumgrGetBcEnabledStatus(pGpu), NV_ERR_INVALID_STATE);

    if (bUnmap)
    {
         gvaspaceUnmap(pGVAS, pGpu, vaLo, vaHi);
    }
    else
    {
        NvU32       kind = pComprInfo->kind;
        NvU32       kindNoCompression;

        //
        // If the original kind is compressible we need to know what the non-compresible
        // kind is so we can fall back to that if we run out of compression tags.
        //
        if (memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, kind))
        {
            kindNoCompression = memmgrGetUncompressedKind_HAL(pGpu, pMemoryManager, kind, NV_FALSE);
        }
        else
        {
            kindNoCompression = kind;
        }

        if (!RMCFG_FEATURE_PLATFORM_WINDOWS &&
            memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, pComprInfo->kind) &&
            ((vAddr & (alignSize-1)) != 0) &&
            !(flags & DMA_UPDATE_VASPACE_FLAGS_UNALIGNED_COMP))
        {
            return NV_ERR_INVALID_ARGUMENT;
        }

        // MMU_MAP_CTX
        mapTarget.pLevelFmt            = mmuFmtFindLevelWithPageShift(pFmt->pRoot,
                                                                BIT_IDX_64(pageSize));
        mapTarget.pIter                = &mapIter;
        mapTarget.MapNextEntries       = _gmmuWalkCBMapNextEntries_RmAperture;
        mapTarget.pageArrayGranularity = pMemDesc->pageArrayGranularity;

        //MMU_MAP_ITER
        mapIter.pFmt            = pFmt;
        mapIter.pPageArray      = pPageArray;
        mapIter.surfaceOffset   = surfaceOffset;
        mapIter.comprInfo       = *pComprInfo;
        mapIter.overMapModulus  = overmapPteMod;
        mapIter.bReadPtes       = readPte;
        mapIter.kindNoCompr     = kindNoCompression;
        mapIter.bCompr          = memmgrIsKind_HAL(pMemoryManager, FB_IS_KIND_COMPRESSIBLE, pComprInfo->kind);
        mapIter.bUpdatePhysAddr = !!(flags & DMA_UPDATE_VASPACE_FLAGS_UPDATE_PADDR);
        mapIter.bUpdateCompr    = !!(flags & DMA_UPDATE_VASPACE_FLAGS_UPDATE_COMPR);
        mapIter.fabricAddr      = fabricAddr;

        if ((pageSize == RM_PAGE_SIZE_512M) && kgmmuIsBug2720120WarEnabled(pKernelGmmu))
        {
            NV_ASSERT_OK_OR_RETURN(_dmaApplyWarForBug2720120(pGVAS, pGpu,
                                                             vaLo, vaHi));
        }

        if (kmemsysNeedInvalidateGpuCacheOnMap_HAL(pGpu, pKernelMemorySystem, isVolatile, aperture))
        {
            kmemsysCacheOp_HAL(pGpu, pKernelMemorySystem, pMemDesc,
                               (aperture == NV_MMU_PTE_APERTURE_PEER_MEMORY) ? FB_CACHE_PEER_MEMORY : FB_CACHE_SYSTEM_MEMORY,
                               FB_CACHE_EVICT);
        }

        // Build PTE template
        if (pFmt->version == GMMU_FMT_VERSION_3)
        {
            NvU32 ptePcfHw  = 0;
            NvU32 ptePcfSw  = 0;

            // Set the new PTE PCF bits
            if (valid)
            {
                nvFieldSetBool(&pFmt->pPte->fldValid, NV_TRUE,
                               mapIter.pteTemplate);
                nvFieldSet32(&pFmt->pPte->fldAperture._enum.desc,
                             aperture, mapIter.pteTemplate);
                nvFieldSet32(&pFmt->pPte->fldPeerIndex, peer,
                             mapIter.pteTemplate);
                nvFieldSet32(&pFmt->pPte->fldKind, kindNoCompression,
                             mapIter.pteTemplate);
                //
                // Initializing the PTE V3 PCF bits whose default values are as follows:
                // 1.Regular vs Privilege : Regular (controlled by the priv flag)
                // 2.RO vs RW             : RW (controlled by the readOnly flag)
                // 3.Atomic Enabled vs Atomic Disabled : Atomic Enabled
                // 4.Cached vs Uncached   : Cached (controlled by the isVolatile flag)
                // 5.ACE vs ACD           : ACD
                //
                ptePcfSw |= isVolatile ? (1 << SW_MMU_PCF_UNCACHED_IDX) : 0;
                ptePcfSw |= readOnly   ? (1 << SW_MMU_PCF_RO_IDX)       : 0;
                ptePcfSw |= tlbLock    ? (1 << SW_MMU_PCF_NOATOMIC_IDX) : 0;
                ptePcfSw |= !priv      ? (1 << SW_MMU_PCF_REGULAR_IDX)  : 0;
                if ((memdescGetAddressSpace(pMemDesc) == ADDR_FABRIC_MC))
                {
                    ptePcfSw |= (1 << SW_MMU_PCF_ACE_IDX);
                }
            }
            else
            {
                // NV4K and NOMAPPING are not supported right now
                if (bSparse)
                {
                    ptePcfSw |= (1 << SW_MMU_PCF_SPARSE_IDX);
                }
                else
                {
                    ptePcfSw |= (1 << SW_MMU_PCF_INVALID_IDX);
                }
            }
            NV_CHECK_OR_RETURN(LEVEL_ERROR,
                               (kgmmuTranslatePtePcfFromSw_HAL(pKernelGmmu, ptePcfSw, &ptePcfHw) == NV_OK),
                               NV_ERR_INVALID_ARGUMENT);

            nvFieldSet32(&pFmt->pPte->fldPtePcf, ptePcfHw, mapIter.pteTemplate);
        }
        else
        {
            if (bSparse)
            {
                const GMMU_FMT_FAMILY *pFmtFamily  =
                    kgmmuFmtGetFamily(pKernelGmmu, pFmt->version);
                NV_ASSERT_OR_RETURN(NULL != pFmtFamily, NV_ERR_INVALID_DATA);
                portMemCopy(mapIter.pteTemplate,
                            mapTarget.pLevelFmt->entrySize, pFmtFamily->sparsePte.v8,
                            mapTarget.pLevelFmt->entrySize);
            }
            else
            {
                nvFieldSetBool(&pFmt->pPte->fldValid, !!valid,
                               mapIter.pteTemplate);
                nvFieldSet32(&pFmt->pPte->fldAperture._enum.desc,
                             aperture, mapIter.pteTemplate);
                nvFieldSet32(&pFmt->pPte->fldPeerIndex, peer,
                             mapIter.pteTemplate);

                nvFieldSetBool(&pFmt->pPte->fldVolatile, !!isVolatile,
                               mapIter.pteTemplate);
                nvFieldSet32(&pFmt->pPte->fldKind, kindNoCompression,
                             mapIter.pteTemplate);
                nvFieldSetBool(&pFmt->pPte->fldReadOnly, !!readOnly,
                               mapIter.pteTemplate);
                nvFieldSetBool(&pFmt->pPte->fldPrivilege, !!priv,
                               mapIter.pteTemplate);
                nvFieldSetBool(&pFmt->pPte->fldEncrypted, !!encrypted,
                               mapIter.pteTemplate);
                if (nvFieldIsValid32(&pFmt->pPte->fldReadDisable.desc))
                {
                    nvFieldSetBool(&pFmt->pPte->fldReadDisable, !!readDisable,
                                   mapIter.pteTemplate);
                }
                if (nvFieldIsValid32(&pFmt->pPte->fldWriteDisable.desc))
                {
                    nvFieldSetBool(&pFmt->pPte->fldWriteDisable, !!writeDisable,
                                   mapIter.pteTemplate);
                }
                if (nvFieldIsValid32(&pFmt->pPte->fldLocked.desc))
                {
                    nvFieldSetBool(&pFmt->pPte->fldLocked, !!tlbLock,
                                   mapIter.pteTemplate);
                }
                if (nvFieldIsValid32(&pFmt->pPte->fldAtomicDisable.desc))
                {
                    // tlbLock is overridden by atomic_disable
                    nvFieldSetBool(&pFmt->pPte->fldAtomicDisable, !!tlbLock,
                                   mapIter.pteTemplate);
                }
            }
        }

        // Extract the physical address field based on aperture.
        mapIter.aperture =
            gmmuFieldGetAperture(&pFmt->pPte->fldAperture,
                                 mapIter.pteTemplate);
        if (mapIter.aperture != GMMU_APERTURE_INVALID)
        {
            mapIter.pAddrField =
                gmmuFmtPtePhysAddrFld(pFmt->pPte, mapIter.aperture);
        }

        //
        // FillPteMem case must be handled specially as it violates
        // internal VAS alignment and constistency checks.
        //
        if (bFillPteMem)
        {
            // If caller supplies buffer to write PTEs to, use that
            if (NULL != pTgtPteMem)
            {
                MMU_WALK_USER_CTX userCtx = {0};
                NvU32 progress = 0;
                NvU32 entryIndexLo = mmuFmtVirtAddrToEntryIndex(mapTarget.pLevelFmt, vaLo);
                // Calculated to allow cross-page-table-boundary updates.
                NvU32 entryIndexHi = (NvU32)(vaHi >> mapTarget.pLevelFmt->virtAddrBitLo) -
                                     (NvU32)(vaLo >> mapTarget.pLevelFmt->virtAddrBitLo) +
                                     entryIndexLo;

                //
                // Use pTgtPteMem directly as mapping and pass NULL memdesc to
                // indicate buffered mode.
                //
                userCtx.pGpu = pGpu;
                mapIter.pMap = pTgtPteMem;
                _gmmuWalkCBMapNextEntries_Direct(&userCtx, &mapTarget, NULL,
                                                 entryIndexLo, entryIndexHi, &progress);
                NV_ASSERT(progress == entryIndexHi - entryIndexLo + 1);
            }
            // Otherwise use walker directly.
            else
            {
                GVAS_BLOCK         *pVASBlock  = NULL;
                EMEMBLOCK          *pMemBlock  = NULL;
                MMU_WALK_USER_CTX   userCtx   = {0};

                pMemBlock = pGVAS->pHeap->eheapGetBlock(pGVAS->pHeap, vaLo, 0);
                NV_ASSERT_OR_RETURN(NULL != pMemBlock, NV_ERR_INVALID_ARGUMENT);
                pVASBlock = pMemBlock->pData;

                NV_ASSERT_OK_OR_GOTO(status,
                    gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, pVASBlock, &userCtx),
                    done);

                status = mmuWalkMap(userCtx.pGpuState->pWalk, vaLo, vaHi, &mapTarget);
                NV_ASSERT(NV_OK == status);
                gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);
            }
        }
        else
        {
            VAS_MAP_FLAGS mapFlags = {0};
            mapFlags.bRemap = readPte ||
                (flags & DMA_UPDATE_VASPACE_FLAGS_ALLOW_REMAP);
            status = gvaspaceMap(pGVAS, pGpu, vaLo, vaHi, &mapTarget, mapFlags);
            NV_CHECK(LEVEL_ERROR, NV_OK == status);
        }
    }

done:
    // Invalidate VAS TLB entries.
    if ((NULL == pTgtPteMem) && DMA_TLB_INVALIDATE == deferInvalidate)
    {
        kbusFlush_HAL(pGpu, pKernelBus, BUS_FLUSH_VIDEO_MEMORY |
                                        BUS_FLUSH_SYSTEM_MEMORY);
        gvaspaceInvalidateTlb(pGVAS, pGpu, update_type);
    }

#if NV_PRINTF_LEVEL_ENABLED(LEVEL_INFO)
    if (DBG_RMMSG_CHECK(LEVEL_INFO))
    {
        MMU_TRACE_ARG arg      = {0};
        MMU_TRACE_PARAM params = {0};
        params.mode            = MMU_TRACE_MODE_TRACE_VERBOSE;
        params.va              = vAddr;
        params.vaLimit         = vAddrLimit;
        params.pArg            = &arg;

        mmuTrace(pGpu, pVAS, &params);
    }
#endif
    return status;
}

NV_STATUS
dmaInit_GM107(OBJGPU *pGpu, VirtMemAllocator *pDma)
{
    DMAHALINFO_FERMI   *pDHPI = NULL;
    NvU32               data;

    // Allocate and link in an 'info block' for this engine.
    if (NULL == (pDHPI = (PDMAHALINFO_FERMI)addInfoPtr(&pDma->infoList, HAL_IMPL_GF100,
                                                        sizeof(DMAHALINFO_FERMI))))
    {
        return NV_ERR_NO_MEMORY;
    }

    if (IS_VIRTUAL(pGpu) || IS_GSP_CLIENT(pGpu))
    {
        pGpu->optimizeUseCaseOverride =
            NV_REG_STR_RM_OPTIMIZE_COMPUTE_OR_SPARSE_TEX_DEFAULT;
    }

    pDHPI->vasReverse = !(!pDHPI->vasReverse);

    pDHPI->compTagLineMultiplier = 1;

    if (osReadRegistryDword(pGpu, NV_REG_STR_RM_RESTRICT_VA_RANGE, &data)
            == NV_OK)
    {
        if (NV_REG_STR_RM_RESTRICT_VA_RANGE_ON == data)
        {
            pDma->bDmaRestrictVaRange = NV_TRUE;
        }
    }

    return NV_OK;
}

void
dmaDestruct_GM107(VirtMemAllocator *pDma)
{
    deleteInfoPtr(&pDma->infoList, HAL_IMPL_GF100);
}

// Called when IsSLI = NV_TRUE and all linked GPUs are loaded
NV_STATUS
dmaStatePostLoad_GM107(OBJGPU *pGpu, VirtMemAllocator *pDma, NvU32 flags)
{
#ifdef DEBUG
    DMAHALINFO_FERMI *pDHPI = DMA_GET_FERMI_INFOBLK(pDma);
    DMAHALINFO_FERMI *pDHPIPeer;
    VirtMemAllocator *pPeerDma;

    pPeerDma = GPU_GET_DMA(pGpu);
    pDHPIPeer = DMA_GET_FERMI_INFOBLK(pPeerDma);

    //
    // Require these attributes to be symmetric for now. If we need to support
    // heterogeneous SLI across GPUs that don't match here we'll need to implement
    // dma[Get|Set]TunableState.
    //
    NV_ASSERT(pDHPIPeer->vasReverse == pDHPI->vasReverse);
    NV_ASSERT(pDHPIPeer->compTagLineMultiplier == pDHPI->compTagLineMultiplier);
#endif
    return NV_OK;
}

// VMM-TODO: Remove or merge with dmaAllocMapping_GF100.
NV_STATUS
dmaMapBuffer_GM107
(
    OBJGPU     *pGpu,
    VirtMemAllocator *pDma,
    OBJVASPACE *pVAS,
    MEMORY_DESCRIPTOR *pMemDesc,
    NvU64      *pVaddr,
    NvU32       flagsForAlloc,
    NvU32       flagsForUpdate
)
{
    MemoryManager    *pMemoryManager = GPU_GET_MEMORY_MANAGER(pGpu);

    KernelGmmu *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    NvU32       kind;
    COMPR_INFO  comprInfo;
    NvU32       pteCount, aperture;
    NvU64       mapLength;
    NvU64       vaddr;
    NV_STATUS   status = NV_OK;
    NvU64       rangeLo = 0;
    NvU64       rangeHi = 0;
    NvU64       compAlign;
    NvU64       vaSize;
    NvU64       vaAlign;
    OBJEHEAP   *pVASpaceHeap = vaspaceGetHeap(pVAS);
    NvU64       pageSize = 0;
    NvU64       pageSizeSubDev = 0;
    NvU64       pageOffs = 0;
    NvU64       pageOffsSubDev = 0;
    NvU32       flags;

    DMA_PAGE_ARRAY pageArray;
    MEMORY_DESCRIPTOR *pSubDevMemDesc  = NULL;
    VAS_ALLOC_FLAGS allocFlags      = {0};

    NV_ASSERT(pVaddr);
    NV_ASSERT(pVAS);

    //
    // Sets the page size for all subdevice memdescs when present. Since we don't support
    // different page size per subdevice, it asserts when the page size differs.
    //
    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)
    pSubDevMemDesc = memdescGetMemDescFromGpu(pMemDesc, pGpu);
    if (memmgrSetMemDescPageSize_HAL(pGpu, pMemoryManager, pSubDevMemDesc, VAS_ADDRESS_TRANSLATION(pVAS),
                                     RM_ATTR_PAGE_SIZE_DEFAULT) != NV_OK)
    {
        SLI_LOOP_RETURN(NV_ERR_INVALID_ARGUMENT);
    }
    pageSizeSubDev = memdescGetPageSize(pSubDevMemDesc, VAS_ADDRESS_TRANSLATION(pVAS));
    pageOffsSubDev = memdescGetPhysAddr(pSubDevMemDesc, VAS_ADDRESS_TRANSLATION(pVAS), 0) &
                        (pageSizeSubDev - 1);
    if (0 == pageSize)
    {
        pageSize = pageSizeSubDev;
        pageOffs = pageOffsSubDev;
    }
    else
    {
        NV_ASSERT(pageSize == pageSizeSubDev);
        NV_ASSERT(pageOffs == pageOffsSubDev);
    }
    SLI_LOOP_END

    status = memmgrGetKindComprFromMemDesc(pMemoryManager, pMemDesc, 0, &kind, &comprInfo);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "memmgrGetKindComprFromMemDesc failed\n");
        return NV_ERR_GENERIC;
    }

    if (kgmmuIsPerVaspaceBigPageEn(pKernelGmmu) &&
        (pageSize >= RM_PAGE_SIZE_64K))
    {
        NV_ASSERT(pageSize != RM_PAGE_SIZE_HUGE);
        pageSize = vaspaceGetBigPageSize(pVAS);
    }

    mapLength  = RM_ALIGN_UP(pageOffs + memdescGetSize(pMemDesc), pageSize);

    vaddr     = 0;
    compAlign = NVBIT64(comprInfo.compPageShift);
    vaAlign   = NV_MAX(pageSize, compAlign);
    vaSize    = RM_ALIGN_UP(mapLength, vaAlign);

    if (flagsForAlloc & DMA_ALLOC_VASPACE_SIZE_ALIGNED)
    {
        NvU64 temp = vaSize;
        ROUNDUP_POW2_U64(temp);
        vaAlign = NV_MAX(vaAlign, temp);
    }

    // Set this first in case we ignore DMA_ALLOC_VASPACE_USE_RM_INTERNAL_VALIMITS next
    rangeLo = vaspaceGetVaStart(pVAS);
    rangeHi = vaspaceGetVaLimit(pVAS);

    if (flagsForAlloc & DMA_ALLOC_VASPACE_USE_RM_INTERNAL_VALIMITS)
    {
        OBJGVASPACE *pGVAS = dynamicCast(pVAS, OBJGVASPACE);
        if (pGVAS)
        {
            rangeLo = pGVAS->vaStartInternal;
            rangeHi = pGVAS->vaLimitInternal;
        }
    }

    // If trying to conserve 32bit address space, map RM buffers at 4GB+
    if (pDma->getProperty(pDma, PDB_PROP_DMA_ENFORCE_32BIT_POINTER) &&
        (pVASpaceHeap->free > NVBIT64(32)))
    {
        rangeLo = NV_MAX(NVBIT64(32), rangeLo);
    }

    if (flagsForAlloc & DMA_VA_LIMIT_57B)
    {
        rangeHi = NV_MIN(rangeHi, NVBIT64(57) - 1);
    }
    else if (flagsForAlloc & DMA_VA_LIMIT_49B)
    {
        rangeHi = NV_MIN(rangeHi, NVBIT64(49) - 1);
    }
    else if (pDma->bDmaRestrictVaRange)
    {
        // See comments in vaspaceFillAllocParams_IMPL.
        rangeHi = NV_MIN(rangeHi, NVBIT64(40) - 1);
    }

    status = vaspaceAlloc(pVAS, vaSize, vaAlign, rangeLo, rangeHi,
                          pageSize, allocFlags, &vaddr);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "vaspaceAlloc failed\n");
        return status;
    }

    SLI_LOOP_START(SLI_LOOP_FLAGS_BC_ONLY)

    pSubDevMemDesc = memdescGetMemDescFromGpu(pMemDesc, pGpu);

    pteCount  = memdescGetContiguity(pSubDevMemDesc, VAS_ADDRESS_TRANSLATION(pVAS)) ? 1 :
                    (NvU32)(mapLength >> RM_PAGE_SHIFT);

    dmaPageArrayInit(&pageArray,
        memdescGetPteArray(pSubDevMemDesc, VAS_ADDRESS_TRANSLATION(pVAS)),
        pteCount);
    flags = flagsForUpdate;
    flags |= memdescGetFlag(pSubDevMemDesc, MEMDESC_FLAGS_GPU_PRIVILEGED) ?
                                DMA_UPDATE_VASPACE_FLAGS_PRIV : 0;

    if (memdescGetAddressSpace(pSubDevMemDesc) == ADDR_FBMEM)
    {
        aperture = NV_MMU_PTE_APERTURE_VIDEO_MEMORY;
    }
    else if (memdescGetCpuCacheAttrib(pSubDevMemDesc) == NV_MEMORY_CACHED)
    {
        aperture = NV_MMU_PTE_APERTURE_SYSTEM_COHERENT_MEMORY;
    }
    else
    {
        aperture = NV_MMU_PTE_APERTURE_SYSTEM_NON_COHERENT_MEMORY;
    }

    status = dmaUpdateVASpace_HAL(pGpu, pDma, pVAS,
                                  pSubDevMemDesc,
                                  NULL,
                                  vaddr, vaddr + mapLength - 1,
                                  flags | DMA_UPDATE_VASPACE_FLAGS_UPDATE_ALL,
                                 &pageArray, 0, &comprInfo,
                                  0,
                                  NV_MMU_PTE_VALID_TRUE,
                                  aperture, 0,
                                  NVLINK_INVALID_FABRIC_ADDR,
                                  NV_FALSE, NV_FALSE,
                                  pageSize);

    if (status != NV_OK)
    {
        SLI_LOOP_BREAK;
    }

    SLI_LOOP_END

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "dmaUpdateVASpace_GF100 failed\n");
        vaspaceFree(pVAS, vaddr);
        return NV_ERR_GENERIC;
    }

    if (pVaddr)
    {
        *pVaddr = vaddr;
    }

    return NV_OK;
}

void dmaUnmapBuffer_GM107(OBJGPU *pGpu, VirtMemAllocator *pDma, OBJVASPACE *pVAS, NvU64 vaddr)
{
    NV_ASSERT_OR_RETURN_VOID(NULL != pVAS);

    vaspaceFree(pVAS, vaddr);
}

#ifdef DEBUG
/*
 * These routines are not used by the RM proper. They are meant to be used by by
 * external debuggers.  Because of this we do not have have a global prototype.
 */
NvU32 _mmuReadFb32(OBJGPU *pGpu, RmPhysAddr addr, NvU32 aperture);
void _mmuWriteFb32(OBJGPU *pGpu, RmPhysAddr addr, NvU32 data, NvU32 aperture);

NvU32 _mmuReadFb32(OBJGPU *pGpu, RmPhysAddr addr, NvU32 aperture)
{
    MEMORY_DESCRIPTOR   memDesc = {0};
    NvU8                *pOffset = NULL;
    NvU32               data = 0;

    if (aperture == 0)
        aperture = ADDR_FBMEM;
    memdescCreateExisting(&memDesc, pGpu, 4, aperture, NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE);
    memdescDescribe(&memDesc, aperture, addr, 4); // Note that this will probably fail with MODS/sysmem
    pOffset = kbusMapRmAperture_HAL(pGpu, &memDesc);
    if (pOffset == NULL)
    {
        NV_ASSERT(pOffset != NULL);
        goto _mmuReadFb32_failed;
    }

    data = MEM_RD32(pOffset);

    kbusUnmapRmAperture_HAL(pGpu, &memDesc, &pOffset, NV_TRUE);
_mmuReadFb32_failed:
    memdescDestroy(&memDesc);

    return data;
}

void _mmuWriteFb32(OBJGPU *pGpu, RmPhysAddr addr, NvU32 data, NvU32 aperture)
{
    MEMORY_DESCRIPTOR   memDesc = {0};
    NvU8                *pOffset = NULL;

    if (aperture == 0)
        aperture = ADDR_FBMEM;
    memdescCreateExisting(&memDesc, pGpu, 4, aperture, NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE);
    memdescDescribe(&memDesc, aperture, addr, 4); // Note that this will probably fail with MODS/sysmem
    pOffset = kbusMapRmAperture_HAL(pGpu, &memDesc);
    if (pOffset == NULL)
    {
        NV_ASSERT(pOffset != NULL);
        goto _mmuWriteFb32_failed;
    }

    MEM_WR32(pOffset, data);

    kbusUnmapRmAperture_HAL(pGpu, &memDesc, &pOffset, NV_TRUE);
_mmuWriteFb32_failed:
    memdescDestroy(&memDesc);
}

#endif // DEBUG

//--------------------------------------------------------------------------------
//  dmaXlateVAtoPAforChannel_GM107 - this function translates virtual address
//      to physical address through page table walk for a given channel id.
//
//  Returns NV_OK if translation was successful, NV_ERR_GENERIC otherwise.
//
//  Output parameters:
//      pAddr   - physical address
//      memType - memory type where this physical address belongs to
//                (ADDR_SYSMEM or ADDR_FBMEM)
//
//--------------------------------------------------------------------------------
NV_STATUS
dmaXlateVAtoPAforChannel_GM107
(
    OBJGPU           *pGpu,
    VirtMemAllocator *pDma,
    KernelChannel    *pKernelChannel,
    NvU64             vAddr,
    NvU64            *pAddr,
    NvU32            *memType
)
{
    NV_ASSERT_OR_RETURN(pKernelChannel != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pAddr != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(memType != NULL, NV_ERR_INVALID_ARGUMENT);

    MMU_TRACE_ARG arg      = {0};
    MMU_TRACE_PARAM params = {0};
    NV_STATUS status;

    params.mode    = MMU_TRACE_MODE_TRANSLATE;
    params.va      = vAddr;
    params.vaLimit = vAddr;
    params.pArg    = &arg;

    status = mmuTrace(pGpu, pKernelChannel->pVAS, &params);
    if (status == NV_OK)
    {
        *memType = arg.aperture;
        *pAddr = arg.pa;
    }

    return status;
}

static NV_STATUS
_dmaApplyWarForBug2720120
(
    OBJGVASPACE *pGVAS,
    OBJGPU      *pGpu,
    const NvU64  vaLo,
    const NvU64  vaHi
)
{
    KernelGmmu            *pKernelGmmu = GPU_GET_KERNEL_GMMU(pGpu);
    KernelBus             *pKernelBus  = GPU_GET_KERNEL_BUS(pGpu);
    GVAS_GPU_STATE        *pGpuState   = gvaspaceGetGpuState(pGVAS, pGpu);
    const GMMU_FMT        *pFmt        = pGpuState->pFmt;
    const GMMU_FMT_FAMILY *pFmtFamily  = kgmmuFmtGetFamily(pKernelGmmu, pFmt->version);
    GVAS_BLOCK            *pVASBlock   = NULL;
    EMEMBLOCK             *pMemBlock   = NULL;
    MMU_WALK_USER_CTX      userCtx     = {0};
    MMU_MAP_TARGET         mapTarget   = {0};
    MMU_MAP_ITERATOR       mapIter     = {0};

    NV_ASSERT_OK_OR_RETURN(kgmmuSetupWarForBug2720120_HAL(pKernelGmmu));

    // MMU_MAP_CTX
    mapTarget.pLevelFmt      = mmuFmtFindLevelWithPageShift(pFmt->pRoot, 29);
    mapTarget.pIter          = &mapIter;
    mapTarget.MapNextEntries = _gmmuWalkCBMapNextEntries_RmAperture;

    //MMU_MAP_ITER
    mapIter.pFmt                   = pFmt;
    mapIter.bApplyWarForBug2720120 = NV_TRUE;

    // Copy the template
    portMemCopy(mapIter.pteTemplate,
                mapTarget.pLevelFmt->entrySize, pFmtFamily->bug2720120WarPde1.v8,
                mapTarget.pLevelFmt->entrySize);

    pMemBlock = pGVAS->pHeap->eheapGetBlock(pGVAS->pHeap, vaLo, 0);
    NV_ASSERT_OR_RETURN(pMemBlock != NULL, NV_ERR_INVALID_ARGUMENT);
    pVASBlock = pMemBlock->pData;

    NV_ASSERT_OK_OR_RETURN(gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, pVASBlock, &userCtx));
    NV_ASSERT_OK_OR_RETURN(mmuWalkMap(userCtx.pGpuState->pWalk,
                                      vaLo, vaHi, &mapTarget));
    gvaspaceWalkUserCtxRelease(pGVAS, &userCtx);

    // Flush PTE writes to vidmem and issue TLB invalidate
    kbusFlush_HAL(pGpu, pKernelBus, BUS_FLUSH_VIDEO_MEMORY |
                                    BUS_FLUSH_SYSTEM_MEMORY);
    gvaspaceInvalidateTlb(pGVAS, pGpu, PTE_UPGRADE);

    return NV_OK;
}

NV_STATUS
dmaInitGart_GM107(OBJGPU *pGpu, VirtMemAllocator *pDma)
{
    KernelBif *pKernelBif = GPU_GET_KERNEL_BIF(pGpu);
    pDma->gpuGartCaps = DMA_GPU_GART_CAPS_NOSNOOP;

    if ((pKernelBif != NULL) && FLD_TEST_REF(BIF_DMA_CAPS_SNOOP, _CTXDMA,
                                             kbifGetDmaCaps(pGpu, pKernelBif)))
    {
        pDma->gpuGartCaps |= DMA_GPU_GART_CAPS_SNOOP;
    }

    return NV_OK;
}

/*!
 * @brief This function returns the size of a large page
 *
 * @param[in]  pGpu         OBJGPU pointer
 *
 * @returns    The size of GPU PTE
 */
NvU32
dmaGetPTESize_GM107(OBJGPU *pGpu, VirtMemAllocator *pDma)
{
    return NV_MMU_PTE__SIZE;
}
