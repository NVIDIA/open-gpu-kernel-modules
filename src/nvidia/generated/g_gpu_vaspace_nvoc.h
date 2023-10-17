#ifndef _G_GPU_VASPACE_NVOC_H_
#define _G_GPU_VASPACE_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_gpu_vaspace_nvoc.h"

#ifndef GPU_VASPACE_H
#define GPU_VASPACE_H

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
* Module: gpu_vaspace.h                                                     *
*       Defines and structures used for GPU Virtual Address Space Object.   *
\***************************************************************************/

#include "mmu/mmu_walk.h"
#include "mmu/gmmu_fmt.h"

#include "core/core.h"
#include "mem_mgr/vaspace.h"               // base class object header
#include "ctrl/ctrl90f1.h"

#include "containers/list.h"
#include "containers/map.h"
#include "mem_mgr/pool_alloc.h"

MAKE_MAP(GVAS_CHANGRP_MAP, NvU32);
typedef GVAS_CHANGRP_MAPIter GVAS_CHANGRP_MAP_ITER;

typedef struct OBJGVASPACE *POBJGVASPACE;

#ifndef __NVOC_CLASS_OBJGVASPACE_TYPEDEF__
#define __NVOC_CLASS_OBJGVASPACE_TYPEDEF__
typedef struct OBJGVASPACE OBJGVASPACE;
#endif /* __NVOC_CLASS_OBJGVASPACE_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGVASPACE
#define __nvoc_class_id_OBJGVASPACE 0xba5875
#endif /* __nvoc_class_id_OBJGVASPACE */


typedef struct KernelChannelGroup KernelChannelGroup;

#ifndef __NVOC_CLASS_KernelChannelGroup_TYPEDEF__
#define __NVOC_CLASS_KernelChannelGroup_TYPEDEF__
typedef struct KernelChannelGroup KernelChannelGroup;
#endif /* __NVOC_CLASS_KernelChannelGroup_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelChannelGroup
#define __nvoc_class_id_KernelChannelGroup 0xec6de1
#endif /* __nvoc_class_id_KernelChannelGroup */



/*!
 * Max number of ranges of VA reserved for partial page tables.
 */
#define GVAS_MAX_PARTIAL_PAGE_TABLE_RANGES 5

// On Linux, CUDA apps allocate VA in bottom 4GB and also above 8GB
#define SPLIT_VAS_SERVER_RM_MANAGED_VA_START   0x100000000ULL  // 4GB
#define SPLIT_VAS_SERVER_RM_MANAGED_VA_SIZE     0x20000000ULL  // 512MB

// Size of Host RM managed VA space required when Full SRIOV or GSP-RM are enabled
#define VA_SIZE_FULL_SRIOV_OR_GSP 0x20000000ULL  // 512MB

/*!
 * Tracks ranges currently mapped (per-GPU).
 */
typedef struct
{
    /*!
     * Embedded btree node.
     */
    NODE node;

    /*!
     * Mask of GPUs with this mapping.
     */
    NvU32 gpuMask;
} GVAS_MAPPING;

/*!
 * Virtual Address Space Block - Data tracked per virtual allocation
 */
typedef struct
{
    /*!
     * Tree of current mappings.
     */
    GVAS_MAPPING *pMapTree;
    /*!
     * Mask of page sizes locked down at VA reservation.
     */
    NvU64 pageSizeLockMask;
    /*!
     * Flags passed by user on VA alloc (reservation).
     */
    VAS_ALLOC_FLAGS flags;
    /*!
     * Level of management.
     */
    VA_MANAGEMENT management;
} GVAS_BLOCK, *PGVAS_BLOCK;

/*!
 * Virtual address range.
 */
struct VA_RANGE_GPU
{
    NvU64 vaLo;
    NvU64 vaHi;
};

typedef struct VA_RANGE_GPU VA_RANGE_GPU;

/*!
 * List of virtual address range.
 */
MAKE_LIST(ReservedPageTableEntriesList, VA_RANGE_GPU);

/*!
 * Per-VAS per-GPU state.
 */
typedef struct GVAS_GPU_STATE
{
    /*!
     * GMMU format for this GPU (structured for heterogenous SLI).
     */
    const GMMU_FMT    *pFmt;
    /*!
     * MMU walk library state.
     */
    MMU_WALK          *pWalk;
    /*!
     * Mirrored Root Page Dir for UVM mirroring.
     */
    MMU_WALK_MEMDESC  *pMirroredRoot;
    /*!
     * RM-internal root page directory for shared VAS management.
     */
    MEMORY_DESCRIPTOR *pRootInternal;
    /*!
     * List head of 4K page cache used for suballocating BPTs
     */
    MEMORY_DESCRIPTOR_LIST unpackedMemDescList;
    /*!
     * Reserved page table entries for the GVA space.
     */
    ReservedPageTableEntriesList reservedPageTableEntries;

    /*!
     * FLA Dummy page for short term WAR for FLA Security issue documented in
     * bug: 3059741.
     */
    struct
    {
        /*!
        * Memory handle for Dummy Big page
        */
        NvHandle hMemory;
        /*!
         * PTE Entry for 64K Dummy page size
         */
        GMMU_ENTRY_VALUE pte;
    }flaDummyPage;
} GVAS_GPU_STATE;

/*!
 * GVAS definition of the MMU walker user context.
 */
struct MMU_WALK_USER_CTX
{
    struct OBJGVASPACE       *pGVAS;
    OBJGPU            *pGpu;
    GVAS_GPU_STATE    *pGpuState;
    const GVAS_BLOCK  *pBlock;
    const NvU32       *pChID;
    NvU32             gfid;
};


/*!
 * RM-registered/managed GPU virtual address space.
 */
#ifdef NVOC_GPU_VASPACE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct OBJGVASPACE {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJVASPACE __nvoc_base_OBJVASPACE;
    struct Object *__nvoc_pbase_Object;
    struct OBJVASPACE *__nvoc_pbase_OBJVASPACE;
    struct OBJGVASPACE *__nvoc_pbase_OBJGVASPACE;
    NV_STATUS (*__gvaspaceConstruct___)(struct OBJGVASPACE *, NvU32, NvU32, NvU64, NvU64, NvU64, NvU64, NvU32);
    NV_STATUS (*__gvaspaceReserveMempool__)(struct OBJGVASPACE *, struct OBJGPU *, struct Device *, NvU64, NvU64, NvU32);
    NV_STATUS (*__gvaspaceAlloc__)(struct OBJGVASPACE *, NvU64, NvU64, NvU64, NvU64, NvU64, VAS_ALLOC_FLAGS, NvU64 *);
    NV_STATUS (*__gvaspaceFree__)(struct OBJGVASPACE *, NvU64);
    NV_STATUS (*__gvaspaceApplyDefaultAlignment__)(struct OBJGVASPACE *, const FB_ALLOC_INFO *, NvU64 *, NvU64 *, NvU64 *);
    NV_STATUS (*__gvaspaceIncAllocRefCnt__)(struct OBJGVASPACE *, NvU64);
    NV_STATUS (*__gvaspaceMap__)(struct OBJGVASPACE *, struct OBJGPU *, const NvU64, const NvU64, const MMU_MAP_TARGET *, const VAS_MAP_FLAGS);
    void (*__gvaspaceUnmap__)(struct OBJGVASPACE *, struct OBJGPU *, const NvU64, const NvU64);
    struct OBJEHEAP *(*__gvaspaceGetHeap__)(struct OBJGVASPACE *);
    NvU64 (*__gvaspaceGetMapPageSize__)(struct OBJGVASPACE *, struct OBJGPU *, EMEMBLOCK *);
    NvU64 (*__gvaspaceGetBigPageSize__)(struct OBJGVASPACE *);
    NvU32 (*__gvaspaceGetFlags__)(struct OBJGVASPACE *);
    NvBool (*__gvaspaceIsMirrored__)(struct OBJGVASPACE *);
    NvBool (*__gvaspaceIsFaultCapable__)(struct OBJGVASPACE *);
    NvBool (*__gvaspaceIsExternallyOwned__)(struct OBJGVASPACE *);
    NvBool (*__gvaspaceIsAtsEnabled__)(struct OBJGVASPACE *);
    NV_STATUS (*__gvaspaceGetPasid__)(struct OBJGVASPACE *, NvU32 *);
    PMEMORY_DESCRIPTOR (*__gvaspaceGetPageDirBase__)(struct OBJGVASPACE *, struct OBJGPU *);
    PMEMORY_DESCRIPTOR (*__gvaspaceGetKernelPageDirBase__)(struct OBJGVASPACE *, struct OBJGPU *);
    NV_STATUS (*__gvaspacePinRootPageDir__)(struct OBJGVASPACE *, struct OBJGPU *);
    void (*__gvaspaceUnpinRootPageDir__)(struct OBJGVASPACE *, struct OBJGPU *);
    void (*__gvaspaceInvalidateTlb__)(struct OBJGVASPACE *, struct OBJGPU *, VAS_PTE_UPDATE_TYPE);
    NV_STATUS (*__gvaspaceGetVasInfo__)(struct OBJGVASPACE *, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *);
    NV_STATUS (*__gvaspaceGetPageTableInfo__)(struct OBJGVASPACE *, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *);
    NV_STATUS (*__gvaspaceGetPteInfo__)(struct OBJGVASPACE *, struct OBJGPU *, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *, RmPhysAddr *);
    NV_STATUS (*__gvaspaceSetPteInfo__)(struct OBJGVASPACE *, struct OBJGPU *, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *);
    NV_STATUS (*__gvaspaceFreeV2__)(struct OBJGVASPACE *, NvU64, NvU64 *);
    NvU64 (*__gvaspaceGetVaLimit__)(struct OBJGVASPACE *);
    NvU64 (*__gvaspaceGetVaStart__)(struct OBJGVASPACE *);
    struct OBJEHEAP *pHeap;
    NvU64 bigPageSize;
    NvU64 compPageSize;
    NvU64 extManagedAlign;
    NvU32 flags;
    NvU64 partialPtVaRangeSize;
    NvU64 partialPtVaRangeBase[5];
    NvU32 numPartialPtRanges;
    NvBool bIsMirrored;
    NvBool bIsFaultCapable;
    NvBool bIsExternallyOwned;
    MEMORY_DESCRIPTOR *pExternalPDB;
    NvBool bIsAtsEnabled;
    NvU32 processAddrSpaceId;
    NvU64 vaLimitMax;
    GVAS_GPU_STATE *pGpuStates;
    GVAS_CHANGRP_MAP chanGrpMap;
    NvU64 vaStartInternal;
    NvU64 vaLimitInternal;
    NvBool bRMInternalRestrictedVaRange;
    NvU64 vaStartServerRMOwned;
    NvU64 vaLimitServerRMOwned;
    RM_POOL_ALLOC_MEM_RESERVE_INFO *pPageTableMemPool;
};

#ifndef __NVOC_CLASS_OBJGVASPACE_TYPEDEF__
#define __NVOC_CLASS_OBJGVASPACE_TYPEDEF__
typedef struct OBJGVASPACE OBJGVASPACE;
#endif /* __NVOC_CLASS_OBJGVASPACE_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJGVASPACE
#define __nvoc_class_id_OBJGVASPACE 0xba5875
#endif /* __nvoc_class_id_OBJGVASPACE */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJGVASPACE;

#define __staticCast_OBJGVASPACE(pThis) \
    ((pThis)->__nvoc_pbase_OBJGVASPACE)

#ifdef __nvoc_gpu_vaspace_h_disabled
#define __dynamicCast_OBJGVASPACE(pThis) ((OBJGVASPACE*)NULL)
#else //__nvoc_gpu_vaspace_h_disabled
#define __dynamicCast_OBJGVASPACE(pThis) \
    ((OBJGVASPACE*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJGVASPACE)))
#endif //__nvoc_gpu_vaspace_h_disabled


NV_STATUS __nvoc_objCreateDynamic_OBJGVASPACE(OBJGVASPACE**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJGVASPACE(OBJGVASPACE**, Dynamic*, NvU32);
#define __objCreate_OBJGVASPACE(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJGVASPACE((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define gvaspaceConstruct_(pGVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags) gvaspaceConstruct__DISPATCH(pGVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags)
#define gvaspaceReserveMempool(pGVAS, pGpu, pDevice, size, pageSizeLockMask, flags) gvaspaceReserveMempool_DISPATCH(pGVAS, pGpu, pDevice, size, pageSizeLockMask, flags)
#define gvaspaceAlloc(pVAS, size, align, rangeLo, rangeHi, pageSizeLockMask, flags, pAddr) gvaspaceAlloc_DISPATCH(pVAS, size, align, rangeLo, rangeHi, pageSizeLockMask, flags, pAddr)
#define gvaspaceFree(pVAS, vAddr) gvaspaceFree_DISPATCH(pVAS, vAddr)
#define gvaspaceApplyDefaultAlignment(pVAS, pAllocInfo, pAlign, pSize, pPageSizeLockMask) gvaspaceApplyDefaultAlignment_DISPATCH(pVAS, pAllocInfo, pAlign, pSize, pPageSizeLockMask)
#define gvaspaceIncAllocRefCnt(pVAS, vAddr) gvaspaceIncAllocRefCnt_DISPATCH(pVAS, vAddr)
#define gvaspaceMap(pVAS, pGpu, vaLo, vaHi, pTarget, flags) gvaspaceMap_DISPATCH(pVAS, pGpu, vaLo, vaHi, pTarget, flags)
#define gvaspaceUnmap(pVAS, pGpu, vaLo, vaHi) gvaspaceUnmap_DISPATCH(pVAS, pGpu, vaLo, vaHi)
#define gvaspaceGetHeap(pVAS) gvaspaceGetHeap_DISPATCH(pVAS)
#define gvaspaceGetMapPageSize(pVAS, pGpu, pMemBlock) gvaspaceGetMapPageSize_DISPATCH(pVAS, pGpu, pMemBlock)
#define gvaspaceGetBigPageSize(pVAS) gvaspaceGetBigPageSize_DISPATCH(pVAS)
#define gvaspaceGetFlags(pVAS) gvaspaceGetFlags_DISPATCH(pVAS)
#define gvaspaceIsMirrored(pVAS) gvaspaceIsMirrored_DISPATCH(pVAS)
#define gvaspaceIsFaultCapable(pVAS) gvaspaceIsFaultCapable_DISPATCH(pVAS)
#define gvaspaceIsExternallyOwned(pVAS) gvaspaceIsExternallyOwned_DISPATCH(pVAS)
#define gvaspaceIsAtsEnabled(pVAS) gvaspaceIsAtsEnabled_DISPATCH(pVAS)
#define gvaspaceGetPasid(pVAS, pPasid) gvaspaceGetPasid_DISPATCH(pVAS, pPasid)
#define gvaspaceGetPageDirBase(pVAS, pGpu) gvaspaceGetPageDirBase_DISPATCH(pVAS, pGpu)
#define gvaspaceGetKernelPageDirBase(pVAS, pGpu) gvaspaceGetKernelPageDirBase_DISPATCH(pVAS, pGpu)
#define gvaspacePinRootPageDir(pVAS, pGpu) gvaspacePinRootPageDir_DISPATCH(pVAS, pGpu)
#define gvaspaceUnpinRootPageDir(pVAS, pGpu) gvaspaceUnpinRootPageDir_DISPATCH(pVAS, pGpu)
#define gvaspaceInvalidateTlb(pVAS, pGpu, type) gvaspaceInvalidateTlb_DISPATCH(pVAS, pGpu, type)
#define gvaspaceGetVasInfo(pVAS, pParams) gvaspaceGetVasInfo_DISPATCH(pVAS, pParams)
#define gvaspaceGetPageTableInfo(pVAS, pParams) gvaspaceGetPageTableInfo_DISPATCH(pVAS, pParams)
#define gvaspaceGetPteInfo(pVAS, pGpu, pParams, pPhysAddr) gvaspaceGetPteInfo_DISPATCH(pVAS, pGpu, pParams, pPhysAddr)
#define gvaspaceSetPteInfo(pVAS, pGpu, pParams) gvaspaceSetPteInfo_DISPATCH(pVAS, pGpu, pParams)
#define gvaspaceFreeV2(pGVAS, vAddr, pSize) gvaspaceFreeV2_DISPATCH(pGVAS, vAddr, pSize)
#define gvaspaceGetVaLimit(pVAS) gvaspaceGetVaLimit_DISPATCH(pVAS)
#define gvaspaceGetVaStart(pVAS) gvaspaceGetVaStart_DISPATCH(pVAS)
NV_STATUS gvaspaceConstruct__IMPL(struct OBJGVASPACE *pGVAS, NvU32 classId, NvU32 vaspaceId, NvU64 vaStart, NvU64 vaLimit, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 flags);

static inline NV_STATUS gvaspaceConstruct__DISPATCH(struct OBJGVASPACE *pGVAS, NvU32 classId, NvU32 vaspaceId, NvU64 vaStart, NvU64 vaLimit, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 flags) {
    return pGVAS->__gvaspaceConstruct___(pGVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags);
}

NV_STATUS gvaspaceReserveMempool_IMPL(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu, struct Device *pDevice, NvU64 size, NvU64 pageSizeLockMask, NvU32 flags);

static inline NV_STATUS gvaspaceReserveMempool_DISPATCH(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu, struct Device *pDevice, NvU64 size, NvU64 pageSizeLockMask, NvU32 flags) {
    return pGVAS->__gvaspaceReserveMempool__(pGVAS, pGpu, pDevice, size, pageSizeLockMask, flags);
}

NV_STATUS gvaspaceAlloc_IMPL(struct OBJGVASPACE *pVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSizeLockMask, VAS_ALLOC_FLAGS flags, NvU64 *pAddr);

static inline NV_STATUS gvaspaceAlloc_DISPATCH(struct OBJGVASPACE *pVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSizeLockMask, VAS_ALLOC_FLAGS flags, NvU64 *pAddr) {
    return pVAS->__gvaspaceAlloc__(pVAS, size, align, rangeLo, rangeHi, pageSizeLockMask, flags, pAddr);
}

NV_STATUS gvaspaceFree_IMPL(struct OBJGVASPACE *pVAS, NvU64 vAddr);

static inline NV_STATUS gvaspaceFree_DISPATCH(struct OBJGVASPACE *pVAS, NvU64 vAddr) {
    return pVAS->__gvaspaceFree__(pVAS, vAddr);
}

NV_STATUS gvaspaceApplyDefaultAlignment_IMPL(struct OBJGVASPACE *pVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pAlign, NvU64 *pSize, NvU64 *pPageSizeLockMask);

static inline NV_STATUS gvaspaceApplyDefaultAlignment_DISPATCH(struct OBJGVASPACE *pVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pAlign, NvU64 *pSize, NvU64 *pPageSizeLockMask) {
    return pVAS->__gvaspaceApplyDefaultAlignment__(pVAS, pAllocInfo, pAlign, pSize, pPageSizeLockMask);
}

NV_STATUS gvaspaceIncAllocRefCnt_IMPL(struct OBJGVASPACE *pVAS, NvU64 vAddr);

static inline NV_STATUS gvaspaceIncAllocRefCnt_DISPATCH(struct OBJGVASPACE *pVAS, NvU64 vAddr) {
    return pVAS->__gvaspaceIncAllocRefCnt__(pVAS, vAddr);
}

NV_STATUS gvaspaceMap_IMPL(struct OBJGVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const MMU_MAP_TARGET *pTarget, const VAS_MAP_FLAGS flags);

static inline NV_STATUS gvaspaceMap_DISPATCH(struct OBJGVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const MMU_MAP_TARGET *pTarget, const VAS_MAP_FLAGS flags) {
    return pVAS->__gvaspaceMap__(pVAS, pGpu, vaLo, vaHi, pTarget, flags);
}

void gvaspaceUnmap_IMPL(struct OBJGVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi);

static inline void gvaspaceUnmap_DISPATCH(struct OBJGVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi) {
    pVAS->__gvaspaceUnmap__(pVAS, pGpu, vaLo, vaHi);
}

struct OBJEHEAP *gvaspaceGetHeap_IMPL(struct OBJGVASPACE *pVAS);

static inline struct OBJEHEAP *gvaspaceGetHeap_DISPATCH(struct OBJGVASPACE *pVAS) {
    return pVAS->__gvaspaceGetHeap__(pVAS);
}

NvU64 gvaspaceGetMapPageSize_IMPL(struct OBJGVASPACE *pVAS, struct OBJGPU *pGpu, EMEMBLOCK *pMemBlock);

static inline NvU64 gvaspaceGetMapPageSize_DISPATCH(struct OBJGVASPACE *pVAS, struct OBJGPU *pGpu, EMEMBLOCK *pMemBlock) {
    return pVAS->__gvaspaceGetMapPageSize__(pVAS, pGpu, pMemBlock);
}

NvU64 gvaspaceGetBigPageSize_IMPL(struct OBJGVASPACE *pVAS);

static inline NvU64 gvaspaceGetBigPageSize_DISPATCH(struct OBJGVASPACE *pVAS) {
    return pVAS->__gvaspaceGetBigPageSize__(pVAS);
}

NvU32 gvaspaceGetFlags_IMPL(struct OBJGVASPACE *pVAS);

static inline NvU32 gvaspaceGetFlags_DISPATCH(struct OBJGVASPACE *pVAS) {
    return pVAS->__gvaspaceGetFlags__(pVAS);
}

NvBool gvaspaceIsMirrored_IMPL(struct OBJGVASPACE *pVAS);

static inline NvBool gvaspaceIsMirrored_DISPATCH(struct OBJGVASPACE *pVAS) {
    return pVAS->__gvaspaceIsMirrored__(pVAS);
}

NvBool gvaspaceIsFaultCapable_IMPL(struct OBJGVASPACE *pVAS);

static inline NvBool gvaspaceIsFaultCapable_DISPATCH(struct OBJGVASPACE *pVAS) {
    return pVAS->__gvaspaceIsFaultCapable__(pVAS);
}

NvBool gvaspaceIsExternallyOwned_IMPL(struct OBJGVASPACE *pVAS);

static inline NvBool gvaspaceIsExternallyOwned_DISPATCH(struct OBJGVASPACE *pVAS) {
    return pVAS->__gvaspaceIsExternallyOwned__(pVAS);
}

NvBool gvaspaceIsAtsEnabled_IMPL(struct OBJGVASPACE *pVAS);

static inline NvBool gvaspaceIsAtsEnabled_DISPATCH(struct OBJGVASPACE *pVAS) {
    return pVAS->__gvaspaceIsAtsEnabled__(pVAS);
}

NV_STATUS gvaspaceGetPasid_IMPL(struct OBJGVASPACE *pVAS, NvU32 *pPasid);

static inline NV_STATUS gvaspaceGetPasid_DISPATCH(struct OBJGVASPACE *pVAS, NvU32 *pPasid) {
    return pVAS->__gvaspaceGetPasid__(pVAS, pPasid);
}

PMEMORY_DESCRIPTOR gvaspaceGetPageDirBase_IMPL(struct OBJGVASPACE *pVAS, struct OBJGPU *pGpu);

static inline PMEMORY_DESCRIPTOR gvaspaceGetPageDirBase_DISPATCH(struct OBJGVASPACE *pVAS, struct OBJGPU *pGpu) {
    return pVAS->__gvaspaceGetPageDirBase__(pVAS, pGpu);
}

PMEMORY_DESCRIPTOR gvaspaceGetKernelPageDirBase_IMPL(struct OBJGVASPACE *pVAS, struct OBJGPU *pGpu);

static inline PMEMORY_DESCRIPTOR gvaspaceGetKernelPageDirBase_DISPATCH(struct OBJGVASPACE *pVAS, struct OBJGPU *pGpu) {
    return pVAS->__gvaspaceGetKernelPageDirBase__(pVAS, pGpu);
}

NV_STATUS gvaspacePinRootPageDir_IMPL(struct OBJGVASPACE *pVAS, struct OBJGPU *pGpu);

static inline NV_STATUS gvaspacePinRootPageDir_DISPATCH(struct OBJGVASPACE *pVAS, struct OBJGPU *pGpu) {
    return pVAS->__gvaspacePinRootPageDir__(pVAS, pGpu);
}

void gvaspaceUnpinRootPageDir_IMPL(struct OBJGVASPACE *pVAS, struct OBJGPU *pGpu);

static inline void gvaspaceUnpinRootPageDir_DISPATCH(struct OBJGVASPACE *pVAS, struct OBJGPU *pGpu) {
    pVAS->__gvaspaceUnpinRootPageDir__(pVAS, pGpu);
}

void gvaspaceInvalidateTlb_IMPL(struct OBJGVASPACE *pVAS, struct OBJGPU *pGpu, VAS_PTE_UPDATE_TYPE type);

static inline void gvaspaceInvalidateTlb_DISPATCH(struct OBJGVASPACE *pVAS, struct OBJGPU *pGpu, VAS_PTE_UPDATE_TYPE type) {
    pVAS->__gvaspaceInvalidateTlb__(pVAS, pGpu, type);
}

NV_STATUS gvaspaceGetVasInfo_IMPL(struct OBJGVASPACE *pVAS, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams);

static inline NV_STATUS gvaspaceGetVasInfo_DISPATCH(struct OBJGVASPACE *pVAS, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams) {
    return pVAS->__gvaspaceGetVasInfo__(pVAS, pParams);
}

NV_STATUS gvaspaceGetPageTableInfo_IMPL(struct OBJGVASPACE *pVAS, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams);

static inline NV_STATUS gvaspaceGetPageTableInfo_DISPATCH(struct OBJGVASPACE *pVAS, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams) {
    return pVAS->__gvaspaceGetPageTableInfo__(pVAS, pParams);
}

NV_STATUS gvaspaceGetPteInfo_IMPL(struct OBJGVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams, RmPhysAddr *pPhysAddr);

static inline NV_STATUS gvaspaceGetPteInfo_DISPATCH(struct OBJGVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams, RmPhysAddr *pPhysAddr) {
    return pVAS->__gvaspaceGetPteInfo__(pVAS, pGpu, pParams, pPhysAddr);
}

NV_STATUS gvaspaceSetPteInfo_IMPL(struct OBJGVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams);

static inline NV_STATUS gvaspaceSetPteInfo_DISPATCH(struct OBJGVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams) {
    return pVAS->__gvaspaceSetPteInfo__(pVAS, pGpu, pParams);
}

NV_STATUS gvaspaceFreeV2_IMPL(struct OBJGVASPACE *pGVAS, NvU64 vAddr, NvU64 *pSize);

static inline NV_STATUS gvaspaceFreeV2_DISPATCH(struct OBJGVASPACE *pGVAS, NvU64 vAddr, NvU64 *pSize) {
    return pGVAS->__gvaspaceFreeV2__(pGVAS, vAddr, pSize);
}

static inline NvU64 gvaspaceGetVaLimit_DISPATCH(struct OBJGVASPACE *pVAS) {
    return pVAS->__gvaspaceGetVaLimit__(pVAS);
}

static inline NvU64 gvaspaceGetVaStart_DISPATCH(struct OBJGVASPACE *pVAS) {
    return pVAS->__gvaspaceGetVaStart__(pVAS);
}

static inline NvU32 gvaspaceGetReservedVaspaceBase(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu) {
    if (!((0) && (pGpu))) {
        return 1048576;
    }
    return 67108864;
}

void gvaspaceDestruct_IMPL(struct OBJGVASPACE *pGVAS);

#define __nvoc_gvaspaceDestruct(pGVAS) gvaspaceDestruct_IMPL(pGVAS)
NV_STATUS gvaspaceExternalRootDirCommit_IMPL(struct OBJGVASPACE *pGVAS, NvHandle hClient, struct OBJGPU *pGpu, NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_PARAMS *pParams);

#ifdef __nvoc_gpu_vaspace_h_disabled
static inline NV_STATUS gvaspaceExternalRootDirCommit(struct OBJGVASPACE *pGVAS, NvHandle hClient, struct OBJGPU *pGpu, NV0080_CTRL_DMA_SET_PAGE_DIRECTORY_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("OBJGVASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_vaspace_h_disabled
#define gvaspaceExternalRootDirCommit(pGVAS, hClient, pGpu, pParams) gvaspaceExternalRootDirCommit_IMPL(pGVAS, hClient, pGpu, pParams)
#endif //__nvoc_gpu_vaspace_h_disabled

NV_STATUS gvaspaceExternalRootDirRevoke_IMPL(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS *pParams);

#ifdef __nvoc_gpu_vaspace_h_disabled
static inline NV_STATUS gvaspaceExternalRootDirRevoke(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_UNSET_PAGE_DIRECTORY_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("OBJGVASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_vaspace_h_disabled
#define gvaspaceExternalRootDirRevoke(pGVAS, pGpu, pParams) gvaspaceExternalRootDirRevoke_IMPL(pGVAS, pGpu, pParams)
#endif //__nvoc_gpu_vaspace_h_disabled

NV_STATUS gvaspaceResize_IMPL(struct OBJGVASPACE *pGVAS, NV0080_CTRL_DMA_SET_VA_SPACE_SIZE_PARAMS *pParams);

#ifdef __nvoc_gpu_vaspace_h_disabled
static inline NV_STATUS gvaspaceResize(struct OBJGVASPACE *pGVAS, NV0080_CTRL_DMA_SET_VA_SPACE_SIZE_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("OBJGVASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_vaspace_h_disabled
#define gvaspaceResize(pGVAS, pParams) gvaspaceResize_IMPL(pGVAS, pParams)
#endif //__nvoc_gpu_vaspace_h_disabled

NV_STATUS gvaspaceUpdatePde2_IMPL(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *pParams);

#ifdef __nvoc_gpu_vaspace_h_disabled
static inline NV_STATUS gvaspaceUpdatePde2(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_UPDATE_PDE_2_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("OBJGVASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_vaspace_h_disabled
#define gvaspaceUpdatePde2(pGVAS, pGpu, pParams) gvaspaceUpdatePde2_IMPL(pGVAS, pGpu, pParams)
#endif //__nvoc_gpu_vaspace_h_disabled

const struct GMMU_FMT *gvaspaceGetGmmuFmt_IMPL(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_vaspace_h_disabled
static inline const struct GMMU_FMT *gvaspaceGetGmmuFmt(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGVASPACE was disabled!");
    return NULL;
}
#else //__nvoc_gpu_vaspace_h_disabled
#define gvaspaceGetGmmuFmt(pGVAS, pGpu) gvaspaceGetGmmuFmt_IMPL(pGVAS, pGpu)
#endif //__nvoc_gpu_vaspace_h_disabled

GVAS_GPU_STATE *gvaspaceGetGpuState_IMPL(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_vaspace_h_disabled
static inline GVAS_GPU_STATE *gvaspaceGetGpuState(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGVASPACE was disabled!");
    return NULL;
}
#else //__nvoc_gpu_vaspace_h_disabled
#define gvaspaceGetGpuState(pGVAS, pGpu) gvaspaceGetGpuState_IMPL(pGVAS, pGpu)
#endif //__nvoc_gpu_vaspace_h_disabled

void gvaspaceWalkUserCtxAcquire_IMPL(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu, const GVAS_BLOCK *pVASBlock, struct MMU_WALK_USER_CTX *pUserCtx);

#ifdef __nvoc_gpu_vaspace_h_disabled
static inline void gvaspaceWalkUserCtxAcquire(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu, const GVAS_BLOCK *pVASBlock, struct MMU_WALK_USER_CTX *pUserCtx) {
    NV_ASSERT_FAILED_PRECOMP("OBJGVASPACE was disabled!");
}
#else //__nvoc_gpu_vaspace_h_disabled
#define gvaspaceWalkUserCtxAcquire(pGVAS, pGpu, pVASBlock, pUserCtx) gvaspaceWalkUserCtxAcquire_IMPL(pGVAS, pGpu, pVASBlock, pUserCtx)
#endif //__nvoc_gpu_vaspace_h_disabled

void gvaspaceWalkUserCtxRelease_IMPL(struct OBJGVASPACE *pGVAS, struct MMU_WALK_USER_CTX *pUserCtx);

#ifdef __nvoc_gpu_vaspace_h_disabled
static inline void gvaspaceWalkUserCtxRelease(struct OBJGVASPACE *pGVAS, struct MMU_WALK_USER_CTX *pUserCtx) {
    NV_ASSERT_FAILED_PRECOMP("OBJGVASPACE was disabled!");
}
#else //__nvoc_gpu_vaspace_h_disabled
#define gvaspaceWalkUserCtxRelease(pGVAS, pUserCtx) gvaspaceWalkUserCtxRelease_IMPL(pGVAS, pUserCtx)
#endif //__nvoc_gpu_vaspace_h_disabled

NV_STATUS gvaspaceIncChanGrpRefCnt_IMPL(struct OBJGVASPACE *pGVAS, KernelChannelGroup *pKernelChannelGroup);

#ifdef __nvoc_gpu_vaspace_h_disabled
static inline NV_STATUS gvaspaceIncChanGrpRefCnt(struct OBJGVASPACE *pGVAS, KernelChannelGroup *pKernelChannelGroup) {
    NV_ASSERT_FAILED_PRECOMP("OBJGVASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_vaspace_h_disabled
#define gvaspaceIncChanGrpRefCnt(pGVAS, pKernelChannelGroup) gvaspaceIncChanGrpRefCnt_IMPL(pGVAS, pKernelChannelGroup)
#endif //__nvoc_gpu_vaspace_h_disabled

NV_STATUS gvaspaceDecChanGrpRefCnt_IMPL(struct OBJGVASPACE *pGVAS, KernelChannelGroup *pKernelChannelGroup);

#ifdef __nvoc_gpu_vaspace_h_disabled
static inline NV_STATUS gvaspaceDecChanGrpRefCnt(struct OBJGVASPACE *pGVAS, KernelChannelGroup *pKernelChannelGroup) {
    NV_ASSERT_FAILED_PRECOMP("OBJGVASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_vaspace_h_disabled
#define gvaspaceDecChanGrpRefCnt(pGVAS, pKernelChannelGroup) gvaspaceDecChanGrpRefCnt_IMPL(pGVAS, pKernelChannelGroup)
#endif //__nvoc_gpu_vaspace_h_disabled

NvU32 gvaspaceGetChanGrpRefCnt_IMPL(struct OBJGVASPACE *pGVAS, KernelChannelGroup *pKernelChannelGroup);

#ifdef __nvoc_gpu_vaspace_h_disabled
static inline NvU32 gvaspaceGetChanGrpRefCnt(struct OBJGVASPACE *pGVAS, KernelChannelGroup *pKernelChannelGroup) {
    NV_ASSERT_FAILED_PRECOMP("OBJGVASPACE was disabled!");
    return 0;
}
#else //__nvoc_gpu_vaspace_h_disabled
#define gvaspaceGetChanGrpRefCnt(pGVAS, pKernelChannelGroup) gvaspaceGetChanGrpRefCnt_IMPL(pGVAS, pKernelChannelGroup)
#endif //__nvoc_gpu_vaspace_h_disabled

NV_STATUS gvaspaceCheckChanGrpRefCnt_IMPL(struct OBJGVASPACE *pGVAS, KernelChannelGroup *pKernelChannelGroup);

#ifdef __nvoc_gpu_vaspace_h_disabled
static inline NV_STATUS gvaspaceCheckChanGrpRefCnt(struct OBJGVASPACE *pGVAS, KernelChannelGroup *pKernelChannelGroup) {
    NV_ASSERT_FAILED_PRECOMP("OBJGVASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_vaspace_h_disabled
#define gvaspaceCheckChanGrpRefCnt(pGVAS, pKernelChannelGroup) gvaspaceCheckChanGrpRefCnt_IMPL(pGVAS, pKernelChannelGroup)
#endif //__nvoc_gpu_vaspace_h_disabled

NV_STATUS gvaspaceUnregisterAllChanGrps_IMPL(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_vaspace_h_disabled
static inline NV_STATUS gvaspaceUnregisterAllChanGrps(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGVASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_vaspace_h_disabled
#define gvaspaceUnregisterAllChanGrps(pGVAS, pGpu) gvaspaceUnregisterAllChanGrps_IMPL(pGVAS, pGpu)
#endif //__nvoc_gpu_vaspace_h_disabled

NV_STATUS gvaspaceReservePageTableEntries_IMPL(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const NvU64 pageSizeMask);

#ifdef __nvoc_gpu_vaspace_h_disabled
static inline NV_STATUS gvaspaceReservePageTableEntries(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const NvU64 pageSizeMask) {
    NV_ASSERT_FAILED_PRECOMP("OBJGVASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_vaspace_h_disabled
#define gvaspaceReservePageTableEntries(pGVAS, pGpu, vaLo, vaHi, pageSizeMask) gvaspaceReservePageTableEntries_IMPL(pGVAS, pGpu, vaLo, vaHi, pageSizeMask)
#endif //__nvoc_gpu_vaspace_h_disabled

NV_STATUS gvaspaceReleasePageTableEntries_IMPL(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const NvU64 pageSizeMask);

#ifdef __nvoc_gpu_vaspace_h_disabled
static inline NV_STATUS gvaspaceReleasePageTableEntries(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const NvU64 pageSizeMask) {
    NV_ASSERT_FAILED_PRECOMP("OBJGVASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_vaspace_h_disabled
#define gvaspaceReleasePageTableEntries(pGVAS, pGpu, vaLo, vaHi, pageSizeMask) gvaspaceReleasePageTableEntries_IMPL(pGVAS, pGpu, vaLo, vaHi, pageSizeMask)
#endif //__nvoc_gpu_vaspace_h_disabled

NV_STATUS gvaspaceGetPageLevelInfo_IMPL(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu, NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS *pParams);

#ifdef __nvoc_gpu_vaspace_h_disabled
static inline NV_STATUS gvaspaceGetPageLevelInfo(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu, NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("OBJGVASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_vaspace_h_disabled
#define gvaspaceGetPageLevelInfo(pGVAS, pGpu, pParams) gvaspaceGetPageLevelInfo_IMPL(pGVAS, pGpu, pParams)
#endif //__nvoc_gpu_vaspace_h_disabled

NV_STATUS gvaspaceCopyServerRmReservedPdesToServerRm_IMPL(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_vaspace_h_disabled
static inline NV_STATUS gvaspaceCopyServerRmReservedPdesToServerRm(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGVASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_vaspace_h_disabled
#define gvaspaceCopyServerRmReservedPdesToServerRm(pGVAS, pGpu) gvaspaceCopyServerRmReservedPdesToServerRm_IMPL(pGVAS, pGpu)
#endif //__nvoc_gpu_vaspace_h_disabled

NV_STATUS gvaspaceGetFreeHeap_IMPL(struct OBJGVASPACE *pGVAS, NvU64 *pFreeSize);

#ifdef __nvoc_gpu_vaspace_h_disabled
static inline NV_STATUS gvaspaceGetFreeHeap(struct OBJGVASPACE *pGVAS, NvU64 *pFreeSize) {
    NV_ASSERT_FAILED_PRECOMP("OBJGVASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_vaspace_h_disabled
#define gvaspaceGetFreeHeap(pGVAS, pFreeSize) gvaspaceGetFreeHeap_IMPL(pGVAS, pFreeSize)
#endif //__nvoc_gpu_vaspace_h_disabled

NvBool gvaspaceIsInUse_IMPL(struct OBJGVASPACE *pGVAS);

#ifdef __nvoc_gpu_vaspace_h_disabled
static inline NvBool gvaspaceIsInUse(struct OBJGVASPACE *pGVAS) {
    NV_ASSERT_FAILED_PRECOMP("OBJGVASPACE was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_vaspace_h_disabled
#define gvaspaceIsInUse(pGVAS) gvaspaceIsInUse_IMPL(pGVAS)
#endif //__nvoc_gpu_vaspace_h_disabled

NV_STATUS gvaspaceReserveSplitVaSpace_IMPL(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu);

#ifdef __nvoc_gpu_vaspace_h_disabled
static inline NV_STATUS gvaspaceReserveSplitVaSpace(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu) {
    NV_ASSERT_FAILED_PRECOMP("OBJGVASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_vaspace_h_disabled
#define gvaspaceReserveSplitVaSpace(pGVAS, pGpu) gvaspaceReserveSplitVaSpace_IMPL(pGVAS, pGpu)
#endif //__nvoc_gpu_vaspace_h_disabled

NvBool gvaspaceIsInternalVaRestricted_IMPL(struct OBJGVASPACE *pGVAS);

#ifdef __nvoc_gpu_vaspace_h_disabled
static inline NvBool gvaspaceIsInternalVaRestricted(struct OBJGVASPACE *pGVAS) {
    NV_ASSERT_FAILED_PRECOMP("OBJGVASPACE was disabled!");
    return NV_FALSE;
}
#else //__nvoc_gpu_vaspace_h_disabled
#define gvaspaceIsInternalVaRestricted(pGVAS) gvaspaceIsInternalVaRestricted_IMPL(pGVAS)
#endif //__nvoc_gpu_vaspace_h_disabled

NV_STATUS gvaspaceCopyServerReservedPdes_IMPL(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu, NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS *pCopyServerReservedPdesParams);

#ifdef __nvoc_gpu_vaspace_h_disabled
static inline NV_STATUS gvaspaceCopyServerReservedPdes(struct OBJGVASPACE *pGVAS, struct OBJGPU *pGpu, NV90F1_CTRL_VASPACE_COPY_SERVER_RESERVED_PDES_PARAMS *pCopyServerReservedPdesParams) {
    NV_ASSERT_FAILED_PRECOMP("OBJGVASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_gpu_vaspace_h_disabled
#define gvaspaceCopyServerReservedPdes(pGVAS, pGpu, pCopyServerReservedPdesParams) gvaspaceCopyServerReservedPdes_IMPL(pGVAS, pGpu, pCopyServerReservedPdesParams)
#endif //__nvoc_gpu_vaspace_h_disabled

#undef PRIVATE_FIELD


#endif // GPU_VASPACE_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_GPU_VASPACE_NVOC_H_
