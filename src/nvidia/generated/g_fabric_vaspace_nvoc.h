#ifndef _G_FABRIC_VASPACE_NVOC_H_
#define _G_FABRIC_VASPACE_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_fabric_vaspace_nvoc.h"

#ifndef FABRIC_VASPACE_H
#define FABRIC_VASPACE_H

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
* Module: fabric_vaspace.h                                                  *
*    Defines and structures used for Fabric Virtual Address Space Object.   *
\***************************************************************************/

#include "mmu/mmu_walk.h"
#include "mmu/gmmu_fmt.h"
#include "core/core.h"
#include "mem_mgr/vaspace.h"
#include "mem_mgr/gpu_vaspace.h"
#include "gpu/mem_mgr/heap.h"
#include "gpu/mem_mgr/virt_mem_allocator.h"
#include "ctrl/ctrl0080/ctrl0080dma.h"
#include "ctrl/ctrl90f1.h"
#include "gpu/mem_mgr/mem_desc.h"

#include "containers/list.h"
#include "containers/map.h"
#include "mem_mgr/pool_alloc.h"

typedef struct
{
    NODE               Node;
    MEMORY_DESCRIPTOR *pVidMemDesc;
    NvU64              offset;
} FABRIC_VA_TO_GPA_MAP_NODE;

#define FABRIC_VASPACE_MAP_FLAGS_READ_ONLY    NVBIT(0)

/*!
 * RM-registered/managed Fabric virtual address space.
 */
#ifdef NVOC_FABRIC_VASPACE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct FABRIC_VASPACE {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJVASPACE __nvoc_base_OBJVASPACE;
    struct Object *__nvoc_pbase_Object;
    struct OBJVASPACE *__nvoc_pbase_OBJVASPACE;
    struct FABRIC_VASPACE *__nvoc_pbase_FABRIC_VASPACE;
    NV_STATUS (*__fabricvaspaceConstruct___)(struct FABRIC_VASPACE *, NvU32, NvU32, NvU64, NvU64, NvU64, NvU64, NvU32);
    NV_STATUS (*__fabricvaspaceAlloc__)(struct FABRIC_VASPACE *, NvU64, NvU64, NvU64, NvU64, NvU64, VAS_ALLOC_FLAGS, NvU64 *);
    NV_STATUS (*__fabricvaspaceFree__)(struct FABRIC_VASPACE *, NvU64);
    NV_STATUS (*__fabricvaspaceMap__)(struct FABRIC_VASPACE *, struct OBJGPU *, const NvU64, const NvU64, const MMU_MAP_TARGET *, const VAS_MAP_FLAGS);
    void (*__fabricvaspaceUnmap__)(struct FABRIC_VASPACE *, struct OBJGPU *, const NvU64, const NvU64);
    NV_STATUS (*__fabricvaspaceApplyDefaultAlignment__)(struct FABRIC_VASPACE *, const FB_ALLOC_INFO *, NvU64 *, NvU64 *, NvU64 *);
    NV_STATUS (*__fabricvaspaceGetVasInfo__)(struct FABRIC_VASPACE *, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *);
    NV_STATUS (*__fabricvaspacePinRootPageDir__)(struct FABRIC_VASPACE *, struct OBJGPU *);
    void (*__fabricvaspaceUnpinRootPageDir__)(struct FABRIC_VASPACE *, struct OBJGPU *);
    void (*__fabricvaspaceInvalidateTlb__)(struct FABRIC_VASPACE *, struct OBJGPU *, VAS_PTE_UPDATE_TYPE);
    NvBool (*__fabricvaspaceIsMirrored__)(struct FABRIC_VASPACE *);
    NvBool (*__fabricvaspaceIsExternallyOwned__)(struct FABRIC_VASPACE *);
    NvBool (*__fabricvaspaceIsInternalVaRestricted__)(struct FABRIC_VASPACE *);
    NvU32 (*__fabricvaspaceGetFlags__)(struct FABRIC_VASPACE *);
    NvBool (*__fabricvaspaceIsAtsEnabled__)(struct FABRIC_VASPACE *);
    NvU64 (*__fabricvaspaceGetBigPageSize__)(struct FABRIC_VASPACE *);
    NV_STATUS (*__fabricvaspaceGetPteInfo__)(struct FABRIC_VASPACE *, struct OBJGPU *, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *, RmPhysAddr *);
    NvU64 (*__fabricvaspaceGetVaLimit__)(struct FABRIC_VASPACE *);
    PMEMORY_DESCRIPTOR (*__fabricvaspaceGetPageDirBase__)(struct FABRIC_VASPACE *, struct OBJGPU *);
    PMEMORY_DESCRIPTOR (*__fabricvaspaceGetKernelPageDirBase__)(struct FABRIC_VASPACE *, struct OBJGPU *);
    NvU64 (*__fabricvaspaceGetMapPageSize__)(struct FABRIC_VASPACE *, struct OBJGPU *, EMEMBLOCK *);
    struct OBJEHEAP *(*__fabricvaspaceGetHeap__)(struct FABRIC_VASPACE *);
    NvBool (*__fabricvaspaceIsFaultCapable__)(struct FABRIC_VASPACE *);
    NvU64 (*__fabricvaspaceGetVaStart__)(struct FABRIC_VASPACE *);
    NV_STATUS (*__fabricvaspaceIncAllocRefCnt__)(struct FABRIC_VASPACE *, NvU64);
    NV_STATUS (*__fabricvaspaceSetPteInfo__)(struct FABRIC_VASPACE *, struct OBJGPU *, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *);
    NV_STATUS (*__fabricvaspaceFreeV2__)(struct FABRIC_VASPACE *, NvU64, NvU64 *);
    NV_STATUS (*__fabricvaspaceGetPasid__)(struct FABRIC_VASPACE *, NvU32 *);
    NV_STATUS (*__fabricvaspaceGetPageTableInfo__)(struct FABRIC_VASPACE *, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *);
    NV_STATUS (*__fabricvaspaceReserveMempool__)(struct FABRIC_VASPACE *, struct OBJGPU *, NvHandle, NvU64, NvU64, NvU32);
    struct OBJVASPACE *pGVAS;
    NvU32 flags;
    NvHandle hClient;
    NvHandle hDevice;
    NODE *pFabricVaToGpaMap;
    NvU64 ucFabricBase;
    NvU64 ucFabricLimit;
    NvU64 ucFabricInUseSize;
    NvU64 ucFabricFreeSize;
    NvU32 gfid;
    NvBool bRpcAlloc;
};

#ifndef __NVOC_CLASS_FABRIC_VASPACE_TYPEDEF__
#define __NVOC_CLASS_FABRIC_VASPACE_TYPEDEF__
typedef struct FABRIC_VASPACE FABRIC_VASPACE;
#endif /* __NVOC_CLASS_FABRIC_VASPACE_TYPEDEF__ */

#ifndef __nvoc_class_id_FABRIC_VASPACE
#define __nvoc_class_id_FABRIC_VASPACE 0x8c8f3d
#endif /* __nvoc_class_id_FABRIC_VASPACE */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_FABRIC_VASPACE;

#define __staticCast_FABRIC_VASPACE(pThis) \
    ((pThis)->__nvoc_pbase_FABRIC_VASPACE)

#ifdef __nvoc_fabric_vaspace_h_disabled
#define __dynamicCast_FABRIC_VASPACE(pThis) ((FABRIC_VASPACE*)NULL)
#else //__nvoc_fabric_vaspace_h_disabled
#define __dynamicCast_FABRIC_VASPACE(pThis) \
    ((FABRIC_VASPACE*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(FABRIC_VASPACE)))
#endif //__nvoc_fabric_vaspace_h_disabled


NV_STATUS __nvoc_objCreateDynamic_FABRIC_VASPACE(FABRIC_VASPACE**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_FABRIC_VASPACE(FABRIC_VASPACE**, Dynamic*, NvU32);
#define __objCreate_FABRIC_VASPACE(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_FABRIC_VASPACE((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define fabricvaspaceConstruct_(pFabricVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags) fabricvaspaceConstruct__DISPATCH(pFabricVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags)
#define fabricvaspaceAlloc(pFabricVAS, size, align, rangeLo, rangeHi, pageSize, flags, pAddr) fabricvaspaceAlloc_DISPATCH(pFabricVAS, size, align, rangeLo, rangeHi, pageSize, flags, pAddr)
#define fabricvaspaceFree(pFabricVAS, vAddr) fabricvaspaceFree_DISPATCH(pFabricVAS, vAddr)
#define fabricvaspaceMap(pFabricVAS, pGpu, vaLo, vaHi, pTarget, flags) fabricvaspaceMap_DISPATCH(pFabricVAS, pGpu, vaLo, vaHi, pTarget, flags)
#define fabricvaspaceUnmap(pFabricVAS, pGpu, vaLo, vaHi) fabricvaspaceUnmap_DISPATCH(pFabricVAS, pGpu, vaLo, vaHi)
#define fabricvaspaceApplyDefaultAlignment(pFabricVAS, pAllocInfo, pAlign, pSize, pPageSizeLockMask) fabricvaspaceApplyDefaultAlignment_DISPATCH(pFabricVAS, pAllocInfo, pAlign, pSize, pPageSizeLockMask)
#define fabricvaspaceGetVasInfo(pFabricVAS, pParams) fabricvaspaceGetVasInfo_DISPATCH(pFabricVAS, pParams)
#define fabricvaspacePinRootPageDir(pFabricVAS, pGpu) fabricvaspacePinRootPageDir_DISPATCH(pFabricVAS, pGpu)
#define fabricvaspaceUnpinRootPageDir(pFabricVAS, pGpu) fabricvaspaceUnpinRootPageDir_DISPATCH(pFabricVAS, pGpu)
#define fabricvaspaceInvalidateTlb(pFabricVAS, pGpu, type) fabricvaspaceInvalidateTlb_DISPATCH(pFabricVAS, pGpu, type)
#define fabricvaspaceIsMirrored(pVAS) fabricvaspaceIsMirrored_DISPATCH(pVAS)
#define fabricvaspaceIsExternallyOwned(pVAS) fabricvaspaceIsExternallyOwned_DISPATCH(pVAS)
#define fabricvaspaceIsInternalVaRestricted(pVAS) fabricvaspaceIsInternalVaRestricted_DISPATCH(pVAS)
#define fabricvaspaceGetFlags(pVAS) fabricvaspaceGetFlags_DISPATCH(pVAS)
#define fabricvaspaceIsAtsEnabled(pVAS) fabricvaspaceIsAtsEnabled_DISPATCH(pVAS)
#define fabricvaspaceGetBigPageSize(pVAS) fabricvaspaceGetBigPageSize_DISPATCH(pVAS)
#define fabricvaspaceGetPteInfo(pVAS, pGpu, pParams, pPhysAddr) fabricvaspaceGetPteInfo_DISPATCH(pVAS, pGpu, pParams, pPhysAddr)
#define fabricvaspaceGetVaLimit(pVAS) fabricvaspaceGetVaLimit_DISPATCH(pVAS)
#define fabricvaspaceGetPageDirBase(pVAS, pGpu) fabricvaspaceGetPageDirBase_DISPATCH(pVAS, pGpu)
#define fabricvaspaceGetKernelPageDirBase(pVAS, pGpu) fabricvaspaceGetKernelPageDirBase_DISPATCH(pVAS, pGpu)
#define fabricvaspaceGetMapPageSize(pVAS, pGpu, pMemBlock) fabricvaspaceGetMapPageSize_DISPATCH(pVAS, pGpu, pMemBlock)
#define fabricvaspaceGetHeap(pVAS) fabricvaspaceGetHeap_DISPATCH(pVAS)
#define fabricvaspaceIsFaultCapable(pVAS) fabricvaspaceIsFaultCapable_DISPATCH(pVAS)
#define fabricvaspaceGetVaStart(pVAS) fabricvaspaceGetVaStart_DISPATCH(pVAS)
#define fabricvaspaceIncAllocRefCnt(pVAS, vAddr) fabricvaspaceIncAllocRefCnt_DISPATCH(pVAS, vAddr)
#define fabricvaspaceSetPteInfo(pVAS, pGpu, pParams) fabricvaspaceSetPteInfo_DISPATCH(pVAS, pGpu, pParams)
#define fabricvaspaceFreeV2(pVAS, vAddr, pSize) fabricvaspaceFreeV2_DISPATCH(pVAS, vAddr, pSize)
#define fabricvaspaceGetPasid(pVAS, pPasid) fabricvaspaceGetPasid_DISPATCH(pVAS, pPasid)
#define fabricvaspaceGetPageTableInfo(pVAS, pParams) fabricvaspaceGetPageTableInfo_DISPATCH(pVAS, pParams)
#define fabricvaspaceReserveMempool(pVAS, pGpu, hClient, size, pageSizeLockMask, flags) fabricvaspaceReserveMempool_DISPATCH(pVAS, pGpu, hClient, size, pageSizeLockMask, flags)
NV_STATUS fabricvaspaceConstruct__IMPL(struct FABRIC_VASPACE *pFabricVAS, NvU32 classId, NvU32 vaspaceId, NvU64 vaStart, NvU64 vaLimit, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 flags);

static inline NV_STATUS fabricvaspaceConstruct__DISPATCH(struct FABRIC_VASPACE *pFabricVAS, NvU32 classId, NvU32 vaspaceId, NvU64 vaStart, NvU64 vaLimit, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 flags) {
    return pFabricVAS->__fabricvaspaceConstruct___(pFabricVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags);
}

NV_STATUS fabricvaspaceAlloc_IMPL(struct FABRIC_VASPACE *pFabricVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSize, VAS_ALLOC_FLAGS flags, NvU64 *pAddr);

static inline NV_STATUS fabricvaspaceAlloc_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSize, VAS_ALLOC_FLAGS flags, NvU64 *pAddr) {
    return pFabricVAS->__fabricvaspaceAlloc__(pFabricVAS, size, align, rangeLo, rangeHi, pageSize, flags, pAddr);
}

NV_STATUS fabricvaspaceFree_IMPL(struct FABRIC_VASPACE *pFabricVAS, NvU64 vAddr);

static inline NV_STATUS fabricvaspaceFree_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, NvU64 vAddr) {
    return pFabricVAS->__fabricvaspaceFree__(pFabricVAS, vAddr);
}

NV_STATUS fabricvaspaceMap_IMPL(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const MMU_MAP_TARGET *pTarget, const VAS_MAP_FLAGS flags);

static inline NV_STATUS fabricvaspaceMap_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const MMU_MAP_TARGET *pTarget, const VAS_MAP_FLAGS flags) {
    return pFabricVAS->__fabricvaspaceMap__(pFabricVAS, pGpu, vaLo, vaHi, pTarget, flags);
}

void fabricvaspaceUnmap_IMPL(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi);

static inline void fabricvaspaceUnmap_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi) {
    pFabricVAS->__fabricvaspaceUnmap__(pFabricVAS, pGpu, vaLo, vaHi);
}

NV_STATUS fabricvaspaceApplyDefaultAlignment_IMPL(struct FABRIC_VASPACE *pFabricVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pAlign, NvU64 *pSize, NvU64 *pPageSizeLockMask);

static inline NV_STATUS fabricvaspaceApplyDefaultAlignment_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pAlign, NvU64 *pSize, NvU64 *pPageSizeLockMask) {
    return pFabricVAS->__fabricvaspaceApplyDefaultAlignment__(pFabricVAS, pAllocInfo, pAlign, pSize, pPageSizeLockMask);
}

NV_STATUS fabricvaspaceGetVasInfo_IMPL(struct FABRIC_VASPACE *pFabricVAS, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams);

static inline NV_STATUS fabricvaspaceGetVasInfo_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams) {
    return pFabricVAS->__fabricvaspaceGetVasInfo__(pFabricVAS, pParams);
}

NV_STATUS fabricvaspacePinRootPageDir_IMPL(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu);

static inline NV_STATUS fabricvaspacePinRootPageDir_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu) {
    return pFabricVAS->__fabricvaspacePinRootPageDir__(pFabricVAS, pGpu);
}

void fabricvaspaceUnpinRootPageDir_IMPL(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu);

static inline void fabricvaspaceUnpinRootPageDir_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu) {
    pFabricVAS->__fabricvaspaceUnpinRootPageDir__(pFabricVAS, pGpu);
}

void fabricvaspaceInvalidateTlb_IMPL(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, VAS_PTE_UPDATE_TYPE type);

static inline void fabricvaspaceInvalidateTlb_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, VAS_PTE_UPDATE_TYPE type) {
    pFabricVAS->__fabricvaspaceInvalidateTlb__(pFabricVAS, pGpu, type);
}

static inline NvBool fabricvaspaceIsMirrored_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__fabricvaspaceIsMirrored__(pVAS);
}

static inline NvBool fabricvaspaceIsExternallyOwned_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__fabricvaspaceIsExternallyOwned__(pVAS);
}

static inline NvBool fabricvaspaceIsInternalVaRestricted_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__fabricvaspaceIsInternalVaRestricted__(pVAS);
}

static inline NvU32 fabricvaspaceGetFlags_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__fabricvaspaceGetFlags__(pVAS);
}

static inline NvBool fabricvaspaceIsAtsEnabled_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__fabricvaspaceIsAtsEnabled__(pVAS);
}

static inline NvU64 fabricvaspaceGetBigPageSize_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__fabricvaspaceGetBigPageSize__(pVAS);
}

static inline NV_STATUS fabricvaspaceGetPteInfo_DISPATCH(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams, RmPhysAddr *pPhysAddr) {
    return pVAS->__fabricvaspaceGetPteInfo__(pVAS, pGpu, pParams, pPhysAddr);
}

static inline NvU64 fabricvaspaceGetVaLimit_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__fabricvaspaceGetVaLimit__(pVAS);
}

static inline PMEMORY_DESCRIPTOR fabricvaspaceGetPageDirBase_DISPATCH(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu) {
    return pVAS->__fabricvaspaceGetPageDirBase__(pVAS, pGpu);
}

static inline PMEMORY_DESCRIPTOR fabricvaspaceGetKernelPageDirBase_DISPATCH(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu) {
    return pVAS->__fabricvaspaceGetKernelPageDirBase__(pVAS, pGpu);
}

static inline NvU64 fabricvaspaceGetMapPageSize_DISPATCH(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu, EMEMBLOCK *pMemBlock) {
    return pVAS->__fabricvaspaceGetMapPageSize__(pVAS, pGpu, pMemBlock);
}

static inline struct OBJEHEAP *fabricvaspaceGetHeap_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__fabricvaspaceGetHeap__(pVAS);
}

static inline NvBool fabricvaspaceIsFaultCapable_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__fabricvaspaceIsFaultCapable__(pVAS);
}

static inline NvU64 fabricvaspaceGetVaStart_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__fabricvaspaceGetVaStart__(pVAS);
}

static inline NV_STATUS fabricvaspaceIncAllocRefCnt_DISPATCH(struct FABRIC_VASPACE *pVAS, NvU64 vAddr) {
    return pVAS->__fabricvaspaceIncAllocRefCnt__(pVAS, vAddr);
}

static inline NV_STATUS fabricvaspaceSetPteInfo_DISPATCH(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams) {
    return pVAS->__fabricvaspaceSetPteInfo__(pVAS, pGpu, pParams);
}

static inline NV_STATUS fabricvaspaceFreeV2_DISPATCH(struct FABRIC_VASPACE *pVAS, NvU64 vAddr, NvU64 *pSize) {
    return pVAS->__fabricvaspaceFreeV2__(pVAS, vAddr, pSize);
}

static inline NV_STATUS fabricvaspaceGetPasid_DISPATCH(struct FABRIC_VASPACE *pVAS, NvU32 *pPasid) {
    return pVAS->__fabricvaspaceGetPasid__(pVAS, pPasid);
}

static inline NV_STATUS fabricvaspaceGetPageTableInfo_DISPATCH(struct FABRIC_VASPACE *pVAS, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams) {
    return pVAS->__fabricvaspaceGetPageTableInfo__(pVAS, pParams);
}

static inline NV_STATUS fabricvaspaceReserveMempool_DISPATCH(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu, NvHandle hClient, NvU64 size, NvU64 pageSizeLockMask, NvU32 flags) {
    return pVAS->__fabricvaspaceReserveMempool__(pVAS, pGpu, hClient, size, pageSizeLockMask, flags);
}

static inline NvU64 fabricvaspaceGetUCFlaStart(struct FABRIC_VASPACE *pFabricVAS) {
    return pFabricVAS->ucFabricBase;
}

static inline NvU64 fabricvaspaceGetUCFlaLimit(struct FABRIC_VASPACE *pFabricVAS) {
    return pFabricVAS->ucFabricLimit;
}

void fabricvaspaceDestruct_IMPL(struct FABRIC_VASPACE *pFabricVAS);

#define __nvoc_fabricvaspaceDestruct(pFabricVAS) fabricvaspaceDestruct_IMPL(pFabricVAS)
NV_STATUS fabricvaspaceAllocNonContiguous_IMPL(struct FABRIC_VASPACE *pFabricVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSize, VAS_ALLOC_FLAGS flags, NvU64 **ppAddr, NvU32 *pNumAddr);

#ifdef __nvoc_fabric_vaspace_h_disabled
static inline NV_STATUS fabricvaspaceAllocNonContiguous(struct FABRIC_VASPACE *pFabricVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSize, VAS_ALLOC_FLAGS flags, NvU64 **ppAddr, NvU32 *pNumAddr) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceAllocNonContiguous(pFabricVAS, size, align, rangeLo, rangeHi, pageSize, flags, ppAddr, pNumAddr) fabricvaspaceAllocNonContiguous_IMPL(pFabricVAS, size, align, rangeLo, rangeHi, pageSize, flags, ppAddr, pNumAddr)
#endif //__nvoc_fabric_vaspace_h_disabled

void fabricvaspaceBatchFree_IMPL(struct FABRIC_VASPACE *pFabricVAS, NvU64 *pAddr, NvU32 numAddr, NvU32 stride);

#ifdef __nvoc_fabric_vaspace_h_disabled
static inline void fabricvaspaceBatchFree(struct FABRIC_VASPACE *pFabricVAS, NvU64 *pAddr, NvU32 numAddr, NvU32 stride) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
}
#else //__nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceBatchFree(pFabricVAS, pAddr, numAddr, stride) fabricvaspaceBatchFree_IMPL(pFabricVAS, pAddr, numAddr, stride)
#endif //__nvoc_fabric_vaspace_h_disabled

NV_STATUS fabricvaspaceGetFreeHeap_IMPL(struct FABRIC_VASPACE *pFabricVAS, NvU64 *pFreeSize);

#ifdef __nvoc_fabric_vaspace_h_disabled
static inline NV_STATUS fabricvaspaceGetFreeHeap(struct FABRIC_VASPACE *pFabricVAS, NvU64 *pFreeSize) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceGetFreeHeap(pFabricVAS, pFreeSize) fabricvaspaceGetFreeHeap_IMPL(pFabricVAS, pFreeSize)
#endif //__nvoc_fabric_vaspace_h_disabled

NV_STATUS fabricvaspaceGetGpaMemdesc_IMPL(struct FABRIC_VASPACE *pFabricVAS, MEMORY_DESCRIPTOR *pFabricMemdesc, struct OBJGPU *pMappingGpu, MEMORY_DESCRIPTOR **ppAdjustedMemdesc);

#ifdef __nvoc_fabric_vaspace_h_disabled
static inline NV_STATUS fabricvaspaceGetGpaMemdesc(struct FABRIC_VASPACE *pFabricVAS, MEMORY_DESCRIPTOR *pFabricMemdesc, struct OBJGPU *pMappingGpu, MEMORY_DESCRIPTOR **ppAdjustedMemdesc) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceGetGpaMemdesc(pFabricVAS, pFabricMemdesc, pMappingGpu, ppAdjustedMemdesc) fabricvaspaceGetGpaMemdesc_IMPL(pFabricVAS, pFabricMemdesc, pMappingGpu, ppAdjustedMemdesc)
#endif //__nvoc_fabric_vaspace_h_disabled

void fabricvaspacePutGpaMemdesc_IMPL(struct FABRIC_VASPACE *pFabricVAS, MEMORY_DESCRIPTOR *pMemDesc);

#ifdef __nvoc_fabric_vaspace_h_disabled
static inline void fabricvaspacePutGpaMemdesc(struct FABRIC_VASPACE *pFabricVAS, MEMORY_DESCRIPTOR *pMemDesc) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
}
#else //__nvoc_fabric_vaspace_h_disabled
#define fabricvaspacePutGpaMemdesc(pFabricVAS, pMemDesc) fabricvaspacePutGpaMemdesc_IMPL(pFabricVAS, pMemDesc)
#endif //__nvoc_fabric_vaspace_h_disabled

NV_STATUS fabricvaspaceVaToGpaMapInsert_IMPL(struct FABRIC_VASPACE *pFabricVAS, NvU64 vAddr, MEMORY_DESCRIPTOR *pVidMemDesc, NvU64 offset);

#ifdef __nvoc_fabric_vaspace_h_disabled
static inline NV_STATUS fabricvaspaceVaToGpaMapInsert(struct FABRIC_VASPACE *pFabricVAS, NvU64 vAddr, MEMORY_DESCRIPTOR *pVidMemDesc, NvU64 offset) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceVaToGpaMapInsert(pFabricVAS, vAddr, pVidMemDesc, offset) fabricvaspaceVaToGpaMapInsert_IMPL(pFabricVAS, vAddr, pVidMemDesc, offset)
#endif //__nvoc_fabric_vaspace_h_disabled

void fabricvaspaceVaToGpaMapRemove_IMPL(struct FABRIC_VASPACE *pFabricVAS, NvU64 vAddr);

#ifdef __nvoc_fabric_vaspace_h_disabled
static inline void fabricvaspaceVaToGpaMapRemove(struct FABRIC_VASPACE *pFabricVAS, NvU64 vAddr) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
}
#else //__nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceVaToGpaMapRemove(pFabricVAS, vAddr) fabricvaspaceVaToGpaMapRemove_IMPL(pFabricVAS, vAddr)
#endif //__nvoc_fabric_vaspace_h_disabled

NV_STATUS fabricvaspaceAllocMulticast_IMPL(struct FABRIC_VASPACE *pFabricVAS, NvU64 pageSize, NvU64 alignment, VAS_ALLOC_FLAGS flags, NvU64 base, NvU64 size);

#ifdef __nvoc_fabric_vaspace_h_disabled
static inline NV_STATUS fabricvaspaceAllocMulticast(struct FABRIC_VASPACE *pFabricVAS, NvU64 pageSize, NvU64 alignment, VAS_ALLOC_FLAGS flags, NvU64 base, NvU64 size) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceAllocMulticast(pFabricVAS, pageSize, alignment, flags, base, size) fabricvaspaceAllocMulticast_IMPL(pFabricVAS, pageSize, alignment, flags, base, size)
#endif //__nvoc_fabric_vaspace_h_disabled

NV_STATUS fabricvaspaceMapPhysMemdesc_IMPL(struct FABRIC_VASPACE *pFabricVAS, MEMORY_DESCRIPTOR *pFabricMemDesc, NvU64 fabricOffset, MEMORY_DESCRIPTOR *pPhysMemDesc, NvU64 physOffset, NvU64 physMapLength, NvU32 flags);

#ifdef __nvoc_fabric_vaspace_h_disabled
static inline NV_STATUS fabricvaspaceMapPhysMemdesc(struct FABRIC_VASPACE *pFabricVAS, MEMORY_DESCRIPTOR *pFabricMemDesc, NvU64 fabricOffset, MEMORY_DESCRIPTOR *pPhysMemDesc, NvU64 physOffset, NvU64 physMapLength, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceMapPhysMemdesc(pFabricVAS, pFabricMemDesc, fabricOffset, pPhysMemDesc, physOffset, physMapLength, flags) fabricvaspaceMapPhysMemdesc_IMPL(pFabricVAS, pFabricMemDesc, fabricOffset, pPhysMemDesc, physOffset, physMapLength, flags)
#endif //__nvoc_fabric_vaspace_h_disabled

void fabricvaspaceUnmapPhysMemdesc_IMPL(struct FABRIC_VASPACE *pFabricVAS, MEMORY_DESCRIPTOR *pFabricMemDesc, NvU64 fabricOffset, MEMORY_DESCRIPTOR *pPhysMemDesc, NvU64 physMapLength);

#ifdef __nvoc_fabric_vaspace_h_disabled
static inline void fabricvaspaceUnmapPhysMemdesc(struct FABRIC_VASPACE *pFabricVAS, MEMORY_DESCRIPTOR *pFabricMemDesc, NvU64 fabricOffset, MEMORY_DESCRIPTOR *pPhysMemDesc, NvU64 physMapLength) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
}
#else //__nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceUnmapPhysMemdesc(pFabricVAS, pFabricMemDesc, fabricOffset, pPhysMemDesc, physMapLength) fabricvaspaceUnmapPhysMemdesc_IMPL(pFabricVAS, pFabricMemDesc, fabricOffset, pPhysMemDesc, physMapLength)
#endif //__nvoc_fabric_vaspace_h_disabled

NV_STATUS fabricvaspaceInitUCRange_IMPL(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, NvU64 ucFabricBase, NvU64 ucFabricSize);

#ifdef __nvoc_fabric_vaspace_h_disabled
static inline NV_STATUS fabricvaspaceInitUCRange(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, NvU64 ucFabricBase, NvU64 ucFabricSize) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceInitUCRange(pFabricVAS, pGpu, ucFabricBase, ucFabricSize) fabricvaspaceInitUCRange_IMPL(pFabricVAS, pGpu, ucFabricBase, ucFabricSize)
#endif //__nvoc_fabric_vaspace_h_disabled

void fabricvaspaceClearUCRange_IMPL(struct FABRIC_VASPACE *pFabricVAS);

#ifdef __nvoc_fabric_vaspace_h_disabled
static inline void fabricvaspaceClearUCRange(struct FABRIC_VASPACE *pFabricVAS) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
}
#else //__nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceClearUCRange(pFabricVAS) fabricvaspaceClearUCRange_IMPL(pFabricVAS)
#endif //__nvoc_fabric_vaspace_h_disabled

NV_STATUS fabricvaspaceGetPageLevelInfo_IMPL(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS *pParams);

#ifdef __nvoc_fabric_vaspace_h_disabled
static inline NV_STATUS fabricvaspaceGetPageLevelInfo(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceGetPageLevelInfo(pFabricVAS, pGpu, pParams) fabricvaspaceGetPageLevelInfo_IMPL(pFabricVAS, pGpu, pParams)
#endif //__nvoc_fabric_vaspace_h_disabled

NvBool fabricvaspaceIsInUse_IMPL(struct FABRIC_VASPACE *pFabricVAS);

#ifdef __nvoc_fabric_vaspace_h_disabled
static inline NvBool fabricvaspaceIsInUse(struct FABRIC_VASPACE *pFabricVAS) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
    return NV_FALSE;
}
#else //__nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceIsInUse(pFabricVAS) fabricvaspaceIsInUse_IMPL(pFabricVAS)
#endif //__nvoc_fabric_vaspace_h_disabled

#undef PRIVATE_FIELD


#endif // FABRIC_VASPACE_H

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_FABRIC_VASPACE_NVOC_H_
