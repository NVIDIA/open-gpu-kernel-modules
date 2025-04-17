
#ifndef _G_FABRIC_VASPACE_NVOC_H_
#define _G_FABRIC_VASPACE_NVOC_H_

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 2

#include "nvoc/runtime.h"
#include "nvoc/rtti.h"

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

#pragma once
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

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_FABRIC_VASPACE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__FABRIC_VASPACE;
struct NVOC_METADATA__OBJVASPACE;
struct NVOC_VTABLE__FABRIC_VASPACE;


struct FABRIC_VASPACE {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__FABRIC_VASPACE *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJVASPACE __nvoc_base_OBJVASPACE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJVASPACE *__nvoc_pbase_OBJVASPACE;    // vaspace super
    struct FABRIC_VASPACE *__nvoc_pbase_FABRIC_VASPACE;    // fabricvaspace

    // Data members
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


// Vtable with 29 per-class function pointers
struct NVOC_VTABLE__FABRIC_VASPACE {
    NV_STATUS (*__fabricvaspaceConstruct___)(struct FABRIC_VASPACE * /*this*/, NvU32, NvU32, NvU64, NvU64, NvU64, NvU64, NvU32);  // virtual override (vaspace) base (vaspace)
    NV_STATUS (*__fabricvaspaceAlloc__)(struct FABRIC_VASPACE * /*this*/, NvU64, NvU64, NvU64, NvU64, NvU64, VAS_ALLOC_FLAGS, NvU64 *);  // virtual override (vaspace) base (vaspace)
    NV_STATUS (*__fabricvaspaceFree__)(struct FABRIC_VASPACE * /*this*/, NvU64);  // virtual override (vaspace) base (vaspace)
    NV_STATUS (*__fabricvaspaceMap__)(struct FABRIC_VASPACE * /*this*/, struct OBJGPU *, const NvU64, const NvU64, const MMU_MAP_TARGET *, const VAS_MAP_FLAGS);  // virtual override (vaspace) base (vaspace)
    void (*__fabricvaspaceUnmap__)(struct FABRIC_VASPACE * /*this*/, struct OBJGPU *, const NvU64, const NvU64);  // virtual override (vaspace) base (vaspace)
    NV_STATUS (*__fabricvaspaceApplyDefaultAlignment__)(struct FABRIC_VASPACE * /*this*/, const FB_ALLOC_INFO *, NvU64 *, NvU64 *, NvU64 *);  // virtual override (vaspace) base (vaspace)
    NV_STATUS (*__fabricvaspaceGetVasInfo__)(struct FABRIC_VASPACE * /*this*/, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *);  // virtual override (vaspace) base (vaspace)
    NV_STATUS (*__fabricvaspacePinRootPageDir__)(struct FABRIC_VASPACE * /*this*/, struct OBJGPU *);  // virtual override (vaspace) base (vaspace)
    void (*__fabricvaspaceUnpinRootPageDir__)(struct FABRIC_VASPACE * /*this*/, struct OBJGPU *);  // virtual override (vaspace) base (vaspace)
    void (*__fabricvaspaceInvalidateTlb__)(struct FABRIC_VASPACE * /*this*/, struct OBJGPU *, VAS_PTE_UPDATE_TYPE);  // virtual override (vaspace) base (vaspace)
    NV_STATUS (*__fabricvaspaceIncAllocRefCnt__)(struct FABRIC_VASPACE * /*this*/, NvU64);  // inline virtual inherited (vaspace) base (vaspace) body
    NvU64 (*__fabricvaspaceGetVaStart__)(struct FABRIC_VASPACE * /*this*/);  // virtual inherited (vaspace) base (vaspace)
    NvU64 (*__fabricvaspaceGetVaLimit__)(struct FABRIC_VASPACE * /*this*/);  // virtual inherited (vaspace) base (vaspace)
    NvU32 (*__fabricvaspaceGetFlags__)(struct FABRIC_VASPACE * /*this*/);  // inline virtual inherited (vaspace) base (vaspace) body
    NV_STATUS (*__fabricvaspaceReserveMempool__)(struct FABRIC_VASPACE * /*this*/, struct OBJGPU *, struct Device *, NvU64, NvU64, NvU32);  // inline virtual inherited (vaspace) base (vaspace) body
    OBJEHEAP * (*__fabricvaspaceGetHeap__)(struct FABRIC_VASPACE * /*this*/);  // inline virtual inherited (vaspace) base (vaspace) body
    NvU64 (*__fabricvaspaceGetMapPageSize__)(struct FABRIC_VASPACE * /*this*/, struct OBJGPU *, EMEMBLOCK *);  // inline virtual inherited (vaspace) base (vaspace) body
    NvU64 (*__fabricvaspaceGetBigPageSize__)(struct FABRIC_VASPACE * /*this*/);  // inline virtual inherited (vaspace) base (vaspace) body
    NvBool (*__fabricvaspaceIsMirrored__)(struct FABRIC_VASPACE * /*this*/);  // inline virtual inherited (vaspace) base (vaspace) body
    NvBool (*__fabricvaspaceIsFaultCapable__)(struct FABRIC_VASPACE * /*this*/);  // inline virtual inherited (vaspace) base (vaspace) body
    NvBool (*__fabricvaspaceIsExternallyOwned__)(struct FABRIC_VASPACE * /*this*/);  // inline virtual inherited (vaspace) base (vaspace) body
    NvBool (*__fabricvaspaceIsAtsEnabled__)(struct FABRIC_VASPACE * /*this*/);  // inline virtual inherited (vaspace) base (vaspace) body
    NV_STATUS (*__fabricvaspaceGetPasid__)(struct FABRIC_VASPACE * /*this*/, NvU32 *);  // inline virtual inherited (vaspace) base (vaspace) body
    PMEMORY_DESCRIPTOR (*__fabricvaspaceGetPageDirBase__)(struct FABRIC_VASPACE * /*this*/, struct OBJGPU *);  // inline virtual inherited (vaspace) base (vaspace) body
    PMEMORY_DESCRIPTOR (*__fabricvaspaceGetKernelPageDirBase__)(struct FABRIC_VASPACE * /*this*/, struct OBJGPU *);  // inline virtual inherited (vaspace) base (vaspace) body
    NV_STATUS (*__fabricvaspaceGetPageTableInfo__)(struct FABRIC_VASPACE * /*this*/, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *);  // inline virtual inherited (vaspace) base (vaspace) body
    NV_STATUS (*__fabricvaspaceGetPteInfo__)(struct FABRIC_VASPACE * /*this*/, struct OBJGPU *, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *, RmPhysAddr *);  // inline virtual inherited (vaspace) base (vaspace) body
    NV_STATUS (*__fabricvaspaceSetPteInfo__)(struct FABRIC_VASPACE * /*this*/, struct OBJGPU *, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *);  // inline virtual inherited (vaspace) base (vaspace) body
    NV_STATUS (*__fabricvaspaceFreeV2__)(struct FABRIC_VASPACE * /*this*/, NvU64, NvU64 *);  // inline virtual inherited (vaspace) base (vaspace) body
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__FABRIC_VASPACE {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJVASPACE metadata__OBJVASPACE;
    const struct NVOC_VTABLE__FABRIC_VASPACE vtable;
};

#ifndef __NVOC_CLASS_FABRIC_VASPACE_TYPEDEF__
#define __NVOC_CLASS_FABRIC_VASPACE_TYPEDEF__
typedef struct FABRIC_VASPACE FABRIC_VASPACE;
#endif /* __NVOC_CLASS_FABRIC_VASPACE_TYPEDEF__ */

#ifndef __nvoc_class_id_FABRIC_VASPACE
#define __nvoc_class_id_FABRIC_VASPACE 0x8c8f3d
#endif /* __nvoc_class_id_FABRIC_VASPACE */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_FABRIC_VASPACE;

#define __staticCast_FABRIC_VASPACE(pThis) \
    ((pThis)->__nvoc_pbase_FABRIC_VASPACE)

#ifdef __nvoc_fabric_vaspace_h_disabled
#define __dynamicCast_FABRIC_VASPACE(pThis) ((FABRIC_VASPACE*) NULL)
#else //__nvoc_fabric_vaspace_h_disabled
#define __dynamicCast_FABRIC_VASPACE(pThis) \
    ((FABRIC_VASPACE*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(FABRIC_VASPACE)))
#endif //__nvoc_fabric_vaspace_h_disabled

NV_STATUS __nvoc_objCreateDynamic_FABRIC_VASPACE(FABRIC_VASPACE**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_FABRIC_VASPACE(FABRIC_VASPACE**, Dynamic*, NvU32);
#define __objCreate_FABRIC_VASPACE(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_FABRIC_VASPACE((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define fabricvaspaceConstruct__FNPTR(pFabricVAS) pFabricVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceConstruct___
#define fabricvaspaceConstruct_(pFabricVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags) fabricvaspaceConstruct__DISPATCH(pFabricVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags)
#define fabricvaspaceAlloc_FNPTR(pFabricVAS) pFabricVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceAlloc__
#define fabricvaspaceAlloc(pFabricVAS, size, align, rangeLo, rangeHi, pageSize, flags, pAddr) fabricvaspaceAlloc_DISPATCH(pFabricVAS, size, align, rangeLo, rangeHi, pageSize, flags, pAddr)
#define fabricvaspaceFree_FNPTR(pFabricVAS) pFabricVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceFree__
#define fabricvaspaceFree(pFabricVAS, vAddr) fabricvaspaceFree_DISPATCH(pFabricVAS, vAddr)
#define fabricvaspaceMap_FNPTR(pFabricVAS) pFabricVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceMap__
#define fabricvaspaceMap(pFabricVAS, pGpu, vaLo, vaHi, pTarget, flags) fabricvaspaceMap_DISPATCH(pFabricVAS, pGpu, vaLo, vaHi, pTarget, flags)
#define fabricvaspaceUnmap_FNPTR(pFabricVAS) pFabricVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceUnmap__
#define fabricvaspaceUnmap(pFabricVAS, pGpu, vaLo, vaHi) fabricvaspaceUnmap_DISPATCH(pFabricVAS, pGpu, vaLo, vaHi)
#define fabricvaspaceApplyDefaultAlignment_FNPTR(pFabricVAS) pFabricVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceApplyDefaultAlignment__
#define fabricvaspaceApplyDefaultAlignment(pFabricVAS, pAllocInfo, pAlign, pSize, pPageSizeLockMask) fabricvaspaceApplyDefaultAlignment_DISPATCH(pFabricVAS, pAllocInfo, pAlign, pSize, pPageSizeLockMask)
#define fabricvaspaceGetVasInfo_FNPTR(pFabricVAS) pFabricVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceGetVasInfo__
#define fabricvaspaceGetVasInfo(pFabricVAS, pParams) fabricvaspaceGetVasInfo_DISPATCH(pFabricVAS, pParams)
#define fabricvaspacePinRootPageDir_FNPTR(pFabricVAS) pFabricVAS->__nvoc_metadata_ptr->vtable.__fabricvaspacePinRootPageDir__
#define fabricvaspacePinRootPageDir(pFabricVAS, pGpu) fabricvaspacePinRootPageDir_DISPATCH(pFabricVAS, pGpu)
#define fabricvaspaceUnpinRootPageDir_FNPTR(pFabricVAS) pFabricVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceUnpinRootPageDir__
#define fabricvaspaceUnpinRootPageDir(pFabricVAS, pGpu) fabricvaspaceUnpinRootPageDir_DISPATCH(pFabricVAS, pGpu)
#define fabricvaspaceInvalidateTlb_FNPTR(pFabricVAS) pFabricVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceInvalidateTlb__
#define fabricvaspaceInvalidateTlb(pFabricVAS, pGpu, type) fabricvaspaceInvalidateTlb_DISPATCH(pFabricVAS, pGpu, type)
#define fabricvaspaceIncAllocRefCnt_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceIncAllocRefCnt__
#define fabricvaspaceIncAllocRefCnt(pVAS, vAddr) fabricvaspaceIncAllocRefCnt_DISPATCH(pVAS, vAddr)
#define fabricvaspaceGetVaStart_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetVaStart__
#define fabricvaspaceGetVaStart(pVAS) fabricvaspaceGetVaStart_DISPATCH(pVAS)
#define fabricvaspaceGetVaLimit_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetVaLimit__
#define fabricvaspaceGetVaLimit(pVAS) fabricvaspaceGetVaLimit_DISPATCH(pVAS)
#define fabricvaspaceGetFlags_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetFlags__
#define fabricvaspaceGetFlags(pVAS) fabricvaspaceGetFlags_DISPATCH(pVAS)
#define fabricvaspaceReserveMempool_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceReserveMempool__
#define fabricvaspaceReserveMempool(pVAS, pGpu, pDevice, size, pageSizeLockMask, flags) fabricvaspaceReserveMempool_DISPATCH(pVAS, pGpu, pDevice, size, pageSizeLockMask, flags)
#define fabricvaspaceGetHeap_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetHeap__
#define fabricvaspaceGetHeap(pVAS) fabricvaspaceGetHeap_DISPATCH(pVAS)
#define fabricvaspaceGetMapPageSize_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetMapPageSize__
#define fabricvaspaceGetMapPageSize(pVAS, pGpu, pMemBlock) fabricvaspaceGetMapPageSize_DISPATCH(pVAS, pGpu, pMemBlock)
#define fabricvaspaceGetBigPageSize_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetBigPageSize__
#define fabricvaspaceGetBigPageSize(pVAS) fabricvaspaceGetBigPageSize_DISPATCH(pVAS)
#define fabricvaspaceIsMirrored_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceIsMirrored__
#define fabricvaspaceIsMirrored(pVAS) fabricvaspaceIsMirrored_DISPATCH(pVAS)
#define fabricvaspaceIsFaultCapable_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceIsFaultCapable__
#define fabricvaspaceIsFaultCapable(pVAS) fabricvaspaceIsFaultCapable_DISPATCH(pVAS)
#define fabricvaspaceIsExternallyOwned_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceIsExternallyOwned__
#define fabricvaspaceIsExternallyOwned(pVAS) fabricvaspaceIsExternallyOwned_DISPATCH(pVAS)
#define fabricvaspaceIsAtsEnabled_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceIsAtsEnabled__
#define fabricvaspaceIsAtsEnabled(pVAS) fabricvaspaceIsAtsEnabled_DISPATCH(pVAS)
#define fabricvaspaceGetPasid_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetPasid__
#define fabricvaspaceGetPasid(pVAS, pPasid) fabricvaspaceGetPasid_DISPATCH(pVAS, pPasid)
#define fabricvaspaceGetPageDirBase_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetPageDirBase__
#define fabricvaspaceGetPageDirBase(pVAS, pGpu) fabricvaspaceGetPageDirBase_DISPATCH(pVAS, pGpu)
#define fabricvaspaceGetKernelPageDirBase_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetKernelPageDirBase__
#define fabricvaspaceGetKernelPageDirBase(pVAS, pGpu) fabricvaspaceGetKernelPageDirBase_DISPATCH(pVAS, pGpu)
#define fabricvaspaceGetPageTableInfo_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetPageTableInfo__
#define fabricvaspaceGetPageTableInfo(pVAS, pParams) fabricvaspaceGetPageTableInfo_DISPATCH(pVAS, pParams)
#define fabricvaspaceGetPteInfo_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetPteInfo__
#define fabricvaspaceGetPteInfo(pVAS, pGpu, pParams, pPhysAddr) fabricvaspaceGetPteInfo_DISPATCH(pVAS, pGpu, pParams, pPhysAddr)
#define fabricvaspaceSetPteInfo_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceSetPteInfo__
#define fabricvaspaceSetPteInfo(pVAS, pGpu, pParams) fabricvaspaceSetPteInfo_DISPATCH(pVAS, pGpu, pParams)
#define fabricvaspaceFreeV2_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceFreeV2__
#define fabricvaspaceFreeV2(pVAS, vAddr, pSize) fabricvaspaceFreeV2_DISPATCH(pVAS, vAddr, pSize)

// Dispatch functions
static inline NV_STATUS fabricvaspaceConstruct__DISPATCH(struct FABRIC_VASPACE *pFabricVAS, NvU32 classId, NvU32 vaspaceId, NvU64 vaStart, NvU64 vaLimit, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 flags) {
    return pFabricVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceConstruct___(pFabricVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags);
}

static inline NV_STATUS fabricvaspaceAlloc_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSize, VAS_ALLOC_FLAGS flags, NvU64 *pAddr) {
    return pFabricVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceAlloc__(pFabricVAS, size, align, rangeLo, rangeHi, pageSize, flags, pAddr);
}

static inline NV_STATUS fabricvaspaceFree_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, NvU64 vAddr) {
    return pFabricVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceFree__(pFabricVAS, vAddr);
}

static inline NV_STATUS fabricvaspaceMap_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const MMU_MAP_TARGET *pTarget, const VAS_MAP_FLAGS flags) {
    return pFabricVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceMap__(pFabricVAS, pGpu, vaLo, vaHi, pTarget, flags);
}

static inline void fabricvaspaceUnmap_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi) {
    pFabricVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceUnmap__(pFabricVAS, pGpu, vaLo, vaHi);
}

static inline NV_STATUS fabricvaspaceApplyDefaultAlignment_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pAlign, NvU64 *pSize, NvU64 *pPageSizeLockMask) {
    return pFabricVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceApplyDefaultAlignment__(pFabricVAS, pAllocInfo, pAlign, pSize, pPageSizeLockMask);
}

static inline NV_STATUS fabricvaspaceGetVasInfo_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams) {
    return pFabricVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceGetVasInfo__(pFabricVAS, pParams);
}

static inline NV_STATUS fabricvaspacePinRootPageDir_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu) {
    return pFabricVAS->__nvoc_metadata_ptr->vtable.__fabricvaspacePinRootPageDir__(pFabricVAS, pGpu);
}

static inline void fabricvaspaceUnpinRootPageDir_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu) {
    pFabricVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceUnpinRootPageDir__(pFabricVAS, pGpu);
}

static inline void fabricvaspaceInvalidateTlb_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, VAS_PTE_UPDATE_TYPE type) {
    pFabricVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceInvalidateTlb__(pFabricVAS, pGpu, type);
}

static inline NV_STATUS fabricvaspaceIncAllocRefCnt_DISPATCH(struct FABRIC_VASPACE *pVAS, NvU64 vAddr) {
    return pVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceIncAllocRefCnt__(pVAS, vAddr);
}

static inline NvU64 fabricvaspaceGetVaStart_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceGetVaStart__(pVAS);
}

static inline NvU64 fabricvaspaceGetVaLimit_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceGetVaLimit__(pVAS);
}

static inline NvU32 fabricvaspaceGetFlags_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceGetFlags__(pVAS);
}

static inline NV_STATUS fabricvaspaceReserveMempool_DISPATCH(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu, struct Device *pDevice, NvU64 size, NvU64 pageSizeLockMask, NvU32 flags) {
    return pVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceReserveMempool__(pVAS, pGpu, pDevice, size, pageSizeLockMask, flags);
}

static inline OBJEHEAP * fabricvaspaceGetHeap_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceGetHeap__(pVAS);
}

static inline NvU64 fabricvaspaceGetMapPageSize_DISPATCH(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu, EMEMBLOCK *pMemBlock) {
    return pVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceGetMapPageSize__(pVAS, pGpu, pMemBlock);
}

static inline NvU64 fabricvaspaceGetBigPageSize_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceGetBigPageSize__(pVAS);
}

static inline NvBool fabricvaspaceIsMirrored_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceIsMirrored__(pVAS);
}

static inline NvBool fabricvaspaceIsFaultCapable_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceIsFaultCapable__(pVAS);
}

static inline NvBool fabricvaspaceIsExternallyOwned_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceIsExternallyOwned__(pVAS);
}

static inline NvBool fabricvaspaceIsAtsEnabled_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceIsAtsEnabled__(pVAS);
}

static inline NV_STATUS fabricvaspaceGetPasid_DISPATCH(struct FABRIC_VASPACE *pVAS, NvU32 *pPasid) {
    return pVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceGetPasid__(pVAS, pPasid);
}

static inline PMEMORY_DESCRIPTOR fabricvaspaceGetPageDirBase_DISPATCH(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu) {
    return pVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceGetPageDirBase__(pVAS, pGpu);
}

static inline PMEMORY_DESCRIPTOR fabricvaspaceGetKernelPageDirBase_DISPATCH(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu) {
    return pVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceGetKernelPageDirBase__(pVAS, pGpu);
}

static inline NV_STATUS fabricvaspaceGetPageTableInfo_DISPATCH(struct FABRIC_VASPACE *pVAS, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams) {
    return pVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceGetPageTableInfo__(pVAS, pParams);
}

static inline NV_STATUS fabricvaspaceGetPteInfo_DISPATCH(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams, RmPhysAddr *pPhysAddr) {
    return pVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceGetPteInfo__(pVAS, pGpu, pParams, pPhysAddr);
}

static inline NV_STATUS fabricvaspaceSetPteInfo_DISPATCH(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams) {
    return pVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceSetPteInfo__(pVAS, pGpu, pParams);
}

static inline NV_STATUS fabricvaspaceFreeV2_DISPATCH(struct FABRIC_VASPACE *pVAS, NvU64 vAddr, NvU64 *pSize) {
    return pVAS->__nvoc_metadata_ptr->vtable.__fabricvaspaceFreeV2__(pVAS, vAddr, pSize);
}

NV_STATUS fabricvaspaceConstruct__IMPL(struct FABRIC_VASPACE *pFabricVAS, NvU32 classId, NvU32 vaspaceId, NvU64 vaStart, NvU64 vaLimit, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 flags);

NV_STATUS fabricvaspaceAlloc_IMPL(struct FABRIC_VASPACE *pFabricVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSize, VAS_ALLOC_FLAGS flags, NvU64 *pAddr);

NV_STATUS fabricvaspaceFree_IMPL(struct FABRIC_VASPACE *pFabricVAS, NvU64 vAddr);

NV_STATUS fabricvaspaceMap_IMPL(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const MMU_MAP_TARGET *pTarget, const VAS_MAP_FLAGS flags);

void fabricvaspaceUnmap_IMPL(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi);

NV_STATUS fabricvaspaceApplyDefaultAlignment_IMPL(struct FABRIC_VASPACE *pFabricVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pAlign, NvU64 *pSize, NvU64 *pPageSizeLockMask);

NV_STATUS fabricvaspaceGetVasInfo_IMPL(struct FABRIC_VASPACE *pFabricVAS, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams);

NV_STATUS fabricvaspacePinRootPageDir_IMPL(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu);

void fabricvaspaceUnpinRootPageDir_IMPL(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu);

void fabricvaspaceInvalidateTlb_IMPL(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, VAS_PTE_UPDATE_TYPE type);

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
