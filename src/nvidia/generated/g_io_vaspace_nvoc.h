
#ifndef _G_IO_VASPACE_NVOC_H_
#define _G_IO_VASPACE_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2013-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_io_vaspace_nvoc.h"

#ifndef _IOVASPACE_H_
#define _IOVASPACE_H_

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
* Module: IOVASPACE.H                                                       *
*       Defines and structures used for IOMMU Virtual Address Space Object. *
\***************************************************************************/

#include "mem_mgr/vaspace.h"               // base class object header

#define NV_IOVA_DOMAIN_NONE (~(NvU32)0)

/*!
 * Virtual address space for a system's IOMMU translation.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_IO_VASPACE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__OBJIOVASPACE;
struct NVOC_METADATA__OBJVASPACE;


struct OBJIOVASPACE {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__OBJIOVASPACE *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJVASPACE __nvoc_base_OBJVASPACE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJVASPACE *__nvoc_pbase_OBJVASPACE;    // vaspace super
    struct OBJIOVASPACE *__nvoc_pbase_OBJIOVASPACE;    // iovaspace

    // Data members
    NvU64 mappingCount;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__OBJIOVASPACE {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJVASPACE metadata__OBJVASPACE;
};

#ifndef __nvoc_class_id_OBJIOVASPACE
#define __nvoc_class_id_OBJIOVASPACE 0x28ed9cu
typedef struct OBJIOVASPACE OBJIOVASPACE;
#endif /* __nvoc_class_id_OBJIOVASPACE */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJIOVASPACE;

#define __staticCast_OBJIOVASPACE(pThis) \
    ((pThis)->__nvoc_pbase_OBJIOVASPACE)

#ifdef __nvoc_io_vaspace_h_disabled
#define __dynamicCast_OBJIOVASPACE(pThis) ((OBJIOVASPACE*) NULL)
#else //__nvoc_io_vaspace_h_disabled
#define __dynamicCast_OBJIOVASPACE(pThis) \
    ((OBJIOVASPACE*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJIOVASPACE)))
#endif //__nvoc_io_vaspace_h_disabled

NV_STATUS __nvoc_objCreateDynamic_OBJIOVASPACE(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJIOVASPACE(OBJIOVASPACE**, Dynamic*, NvU32);
#define __objCreate_OBJIOVASPACE(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags) \
    __nvoc_objCreate_OBJIOVASPACE((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags))


// Wrapper macros for implementation functions
void iovaspaceDestruct_IMPL(struct OBJIOVASPACE *pIOVAS);
#define __nvoc_iovaspaceDestruct(pIOVAS) iovaspaceDestruct_IMPL(pIOVAS)

PIOVAMAPPING iovaspaceCreateMapping_IMPL(struct OBJIOVASPACE *pIOVAS, PMEMORY_DESCRIPTOR pMemDesc, PIOVAMAPPING pParentIovaMapping);
#ifdef __nvoc_io_vaspace_h_disabled
static inline PIOVAMAPPING iovaspaceCreateMapping(struct OBJIOVASPACE *pIOVAS, PMEMORY_DESCRIPTOR pMemDesc, PIOVAMAPPING pParentIovaMapping) {
    NV_ASSERT_FAILED_PRECOMP("OBJIOVASPACE was disabled!");
    return NULL;
}
#else // __nvoc_io_vaspace_h_disabled
#define iovaspaceCreateMapping(pIOVAS, pMemDesc, pParentIovaMapping) iovaspaceCreateMapping_IMPL(pIOVAS, pMemDesc, pParentIovaMapping)
#endif // __nvoc_io_vaspace_h_disabled

void iovaspaceDestroyMapping_IMPL(struct OBJIOVASPACE *pIOVAS, PIOVAMAPPING pIovaMapping);
#ifdef __nvoc_io_vaspace_h_disabled
static inline void iovaspaceDestroyMapping(struct OBJIOVASPACE *pIOVAS, PIOVAMAPPING pIovaMapping) {
    NV_ASSERT_FAILED_PRECOMP("OBJIOVASPACE was disabled!");
}
#else // __nvoc_io_vaspace_h_disabled
#define iovaspaceDestroyMapping(pIOVAS, pIovaMapping) iovaspaceDestroyMapping_IMPL(pIOVAS, pIovaMapping)
#endif // __nvoc_io_vaspace_h_disabled


// Wrapper macros for halified functions
#define iovaspaceConstruct__FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceConstruct___
#define iovaspaceConstruct_(pVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags) iovaspaceConstruct__DISPATCH(pVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags)
#define iovaspaceAlloc_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceAlloc__
#define iovaspaceAlloc(pVAS, size, align, rangeLo, rangeHi, pageSizeLockMask, flags, pAddr) iovaspaceAlloc_DISPATCH(pVAS, size, align, rangeLo, rangeHi, pageSizeLockMask, flags, pAddr)
#define iovaspaceFree_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceFree__
#define iovaspaceFree(pVAS, vAddr) iovaspaceFree_DISPATCH(pVAS, vAddr)
#define iovaspaceApplyDefaultAlignment_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceApplyDefaultAlignment__
#define iovaspaceApplyDefaultAlignment(pVAS, pAllocInfo, pAlign, pSize, pPageSizeLockMask) iovaspaceApplyDefaultAlignment_DISPATCH(pVAS, pAllocInfo, pAlign, pSize, pPageSizeLockMask)
#define iovaspaceIncAllocRefCnt_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceIncAllocRefCnt__
#define iovaspaceIncAllocRefCnt(pVAS, vAddr) iovaspaceIncAllocRefCnt_DISPATCH(pVAS, vAddr)
#define iovaspaceGetVaStart_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetVaStart__
#define iovaspaceGetVaStart(pVAS) iovaspaceGetVaStart_DISPATCH(pVAS)
#define iovaspaceGetVaLimit_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetVaLimit__
#define iovaspaceGetVaLimit(pVAS) iovaspaceGetVaLimit_DISPATCH(pVAS)
#define iovaspaceGetVasInfo_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetVasInfo__
#define iovaspaceGetVasInfo(pVAS, pParams) iovaspaceGetVasInfo_DISPATCH(pVAS, pParams)
#define iovaspaceGetFlags_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetFlags__
#define iovaspaceGetFlags(pVAS) iovaspaceGetFlags_DISPATCH(pVAS)
#define iovaspaceMap_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceMap__
#define iovaspaceMap(pVAS, pGpu, vaLo, vaHi, pTarget, flags) iovaspaceMap_DISPATCH(pVAS, pGpu, vaLo, vaHi, pTarget, flags)
#define iovaspaceUnmap_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceUnmap__
#define iovaspaceUnmap(pVAS, pGpu, vaLo, vaHi) iovaspaceUnmap_DISPATCH(pVAS, pGpu, vaLo, vaHi)
#define iovaspaceReserveMempool_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceReserveMempool__
#define iovaspaceReserveMempool(pVAS, pGpu, pDevice, size, pageSizeLockMask, flags) iovaspaceReserveMempool_DISPATCH(pVAS, pGpu, pDevice, size, pageSizeLockMask, flags)
#define iovaspaceGetHeap_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetHeap__
#define iovaspaceGetHeap(pVAS) iovaspaceGetHeap_DISPATCH(pVAS)
#define iovaspaceGetFreeHeap_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetFreeHeap__
#define iovaspaceGetFreeHeap(pVAS, pFreeSize) iovaspaceGetFreeHeap_DISPATCH(pVAS, pFreeSize)
#define iovaspaceGetMapPageSize_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetMapPageSize__
#define iovaspaceGetMapPageSize(pVAS, pGpu, pMemBlock) iovaspaceGetMapPageSize_DISPATCH(pVAS, pGpu, pMemBlock)
#define iovaspaceGetBigPageSize_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetBigPageSize__
#define iovaspaceGetBigPageSize(pVAS) iovaspaceGetBigPageSize_DISPATCH(pVAS)
#define iovaspaceIsFaultCapable_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceIsFaultCapable__
#define iovaspaceIsFaultCapable(pVAS) iovaspaceIsFaultCapable_DISPATCH(pVAS)
#define iovaspaceIsExternallyOwned_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceIsExternallyOwned__
#define iovaspaceIsExternallyOwned(pVAS) iovaspaceIsExternallyOwned_DISPATCH(pVAS)
#define iovaspaceIsAtsEnabled_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceIsAtsEnabled__
#define iovaspaceIsAtsEnabled(pVAS) iovaspaceIsAtsEnabled_DISPATCH(pVAS)
#define iovaspaceGetPasid_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetPasid__
#define iovaspaceGetPasid(pVAS, pPasid) iovaspaceGetPasid_DISPATCH(pVAS, pPasid)
#define iovaspaceGetPageDirBase_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetPageDirBase__
#define iovaspaceGetPageDirBase(pVAS, pGpu) iovaspaceGetPageDirBase_DISPATCH(pVAS, pGpu)
#define iovaspaceGetGmmuFmt_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetGmmuFmt__
#define iovaspaceGetGmmuFmt(pVAS, pGpu) iovaspaceGetGmmuFmt_DISPATCH(pVAS, pGpu)
#define iovaspaceGetMmuWalker_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetMmuWalker__
#define iovaspaceGetMmuWalker(pVAS, pGpu, ppWalk) iovaspaceGetMmuWalker_DISPATCH(pVAS, pGpu, ppWalk)
#define iovaspacePinRootPageDir_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspacePinRootPageDir__
#define iovaspacePinRootPageDir(pVAS, pGpu) iovaspacePinRootPageDir_DISPATCH(pVAS, pGpu)
#define iovaspaceUnpinRootPageDir_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceUnpinRootPageDir__
#define iovaspaceUnpinRootPageDir(pVAS, pGpu) iovaspaceUnpinRootPageDir_DISPATCH(pVAS, pGpu)
#define iovaspaceInvalidateTlb_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceInvalidateTlb__
#define iovaspaceInvalidateTlb(pVAS, pGpu, type) iovaspaceInvalidateTlb_DISPATCH(pVAS, pGpu, type)
#define iovaspaceGetPageLevelInfo_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetPageLevelInfo__
#define iovaspaceGetPageLevelInfo(pVAS, pGpu, pParams) iovaspaceGetPageLevelInfo_DISPATCH(pVAS, pGpu, pParams)
#define iovaspaceIsInUse_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceIsInUse__
#define iovaspaceIsInUse(pVAS) iovaspaceIsInUse_DISPATCH(pVAS)
#define iovaspaceGetPageTableInfo_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetPageTableInfo__
#define iovaspaceGetPageTableInfo(pVAS, pParams) iovaspaceGetPageTableInfo_DISPATCH(pVAS, pParams)
#define iovaspaceGetPteInfo_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetPteInfo__
#define iovaspaceGetPteInfo(pVAS, pGpu, pParams, pPhysAddr) iovaspaceGetPteInfo_DISPATCH(pVAS, pGpu, pParams, pPhysAddr)
#define iovaspaceSetPteInfo_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceSetPteInfo__
#define iovaspaceSetPteInfo(pVAS, pGpu, pParams) iovaspaceSetPteInfo_DISPATCH(pVAS, pGpu, pParams)
#define iovaspaceFreeV2_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceFreeV2__
#define iovaspaceFreeV2(pVAS, vAddr, pSize) iovaspaceFreeV2_DISPATCH(pVAS, vAddr, pSize)

// Dispatch functions
static inline NV_STATUS iovaspaceConstruct__DISPATCH(struct OBJIOVASPACE *pVAS, NvU32 classId, NvU32 vaspaceId, NvU64 vaStart, NvU64 vaLimit, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 flags) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceConstruct___(staticCast(pVAS, OBJVASPACE), classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags);
}

static inline NV_STATUS iovaspaceAlloc_DISPATCH(struct OBJIOVASPACE *pVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSizeLockMask, VAS_ALLOC_FLAGS flags, NvU64 *pAddr) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceAlloc__(staticCast(pVAS, OBJVASPACE), size, align, rangeLo, rangeHi, pageSizeLockMask, flags, pAddr);
}

static inline NV_STATUS iovaspaceFree_DISPATCH(struct OBJIOVASPACE *pVAS, NvU64 vAddr) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceFree__(staticCast(pVAS, OBJVASPACE), vAddr);
}

static inline NV_STATUS iovaspaceApplyDefaultAlignment_DISPATCH(struct OBJIOVASPACE *pVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pAlign, NvU64 *pSize, NvU64 *pPageSizeLockMask) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceApplyDefaultAlignment__(staticCast(pVAS, OBJVASPACE), pAllocInfo, pAlign, pSize, pPageSizeLockMask);
}

static inline NV_STATUS iovaspaceIncAllocRefCnt_DISPATCH(struct OBJIOVASPACE *pVAS, NvU64 vAddr) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceIncAllocRefCnt__(staticCast(pVAS, OBJVASPACE), vAddr);
}

static inline NvU64 iovaspaceGetVaStart_DISPATCH(struct OBJIOVASPACE *pVAS) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetVaStart__(staticCast(pVAS, OBJVASPACE));
}

static inline NvU64 iovaspaceGetVaLimit_DISPATCH(struct OBJIOVASPACE *pVAS) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetVaLimit__(staticCast(pVAS, OBJVASPACE));
}

static inline NV_STATUS iovaspaceGetVasInfo_DISPATCH(struct OBJIOVASPACE *pVAS, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetVasInfo__(staticCast(pVAS, OBJVASPACE), pParams);
}

static inline NvU32 iovaspaceGetFlags_DISPATCH(struct OBJIOVASPACE *pVAS) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetFlags__(staticCast(pVAS, OBJVASPACE));
}

static inline NV_STATUS iovaspaceMap_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const MMU_MAP_TARGET *pTarget, const VAS_MAP_FLAGS flags) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceMap__(staticCast(pVAS, OBJVASPACE), pGpu, vaLo, vaHi, pTarget, flags);
}

static inline void iovaspaceUnmap_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi) {
    pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceUnmap__(staticCast(pVAS, OBJVASPACE), pGpu, vaLo, vaHi);
}

static inline NV_STATUS iovaspaceReserveMempool_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, struct Device *pDevice, NvU64 size, NvU64 pageSizeLockMask, NvU32 flags) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceReserveMempool__(staticCast(pVAS, OBJVASPACE), pGpu, pDevice, size, pageSizeLockMask, flags);
}

static inline OBJEHEAP * iovaspaceGetHeap_DISPATCH(struct OBJIOVASPACE *pVAS) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetHeap__(staticCast(pVAS, OBJVASPACE));
}

static inline NV_STATUS iovaspaceGetFreeHeap_DISPATCH(struct OBJIOVASPACE *pVAS, NvU64 *pFreeSize) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetFreeHeap__(staticCast(pVAS, OBJVASPACE), pFreeSize);
}

static inline NvU64 iovaspaceGetMapPageSize_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, EMEMBLOCK *pMemBlock) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetMapPageSize__(staticCast(pVAS, OBJVASPACE), pGpu, pMemBlock);
}

static inline NvU64 iovaspaceGetBigPageSize_DISPATCH(struct OBJIOVASPACE *pVAS) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetBigPageSize__(staticCast(pVAS, OBJVASPACE));
}

static inline NvBool iovaspaceIsFaultCapable_DISPATCH(struct OBJIOVASPACE *pVAS) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceIsFaultCapable__(staticCast(pVAS, OBJVASPACE));
}

static inline NvBool iovaspaceIsExternallyOwned_DISPATCH(struct OBJIOVASPACE *pVAS) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceIsExternallyOwned__(staticCast(pVAS, OBJVASPACE));
}

static inline NvBool iovaspaceIsAtsEnabled_DISPATCH(struct OBJIOVASPACE *pVAS) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceIsAtsEnabled__(staticCast(pVAS, OBJVASPACE));
}

static inline NV_STATUS iovaspaceGetPasid_DISPATCH(struct OBJIOVASPACE *pVAS, NvU32 *pPasid) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetPasid__(staticCast(pVAS, OBJVASPACE), pPasid);
}

static inline PMEMORY_DESCRIPTOR iovaspaceGetPageDirBase_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetPageDirBase__(staticCast(pVAS, OBJVASPACE), pGpu);
}

static inline const GMMU_FMT * iovaspaceGetGmmuFmt_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetGmmuFmt__(staticCast(pVAS, OBJVASPACE), pGpu);
}

static inline NV_STATUS iovaspaceGetMmuWalker_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, MMU_WALK **ppWalk) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetMmuWalker__(staticCast(pVAS, OBJVASPACE), pGpu, ppWalk);
}

static inline NV_STATUS iovaspacePinRootPageDir_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspacePinRootPageDir__(staticCast(pVAS, OBJVASPACE), pGpu);
}

static inline void iovaspaceUnpinRootPageDir_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu) {
    pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceUnpinRootPageDir__(staticCast(pVAS, OBJVASPACE), pGpu);
}

static inline NV_STATUS iovaspaceInvalidateTlb_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, VAS_PTE_UPDATE_TYPE type) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceInvalidateTlb__(staticCast(pVAS, OBJVASPACE), pGpu, type);
}

static inline NV_STATUS iovaspaceGetPageLevelInfo_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS *pParams) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetPageLevelInfo__(staticCast(pVAS, OBJVASPACE), pGpu, pParams);
}

static inline NvBool iovaspaceIsInUse_DISPATCH(struct OBJIOVASPACE *pVAS) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceIsInUse__(staticCast(pVAS, OBJVASPACE));
}

static inline NV_STATUS iovaspaceGetPageTableInfo_DISPATCH(struct OBJIOVASPACE *pVAS, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetPageTableInfo__(staticCast(pVAS, OBJVASPACE), pParams);
}

static inline NV_STATUS iovaspaceGetPteInfo_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams, RmPhysAddr *pPhysAddr) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetPteInfo__(staticCast(pVAS, OBJVASPACE), pGpu, pParams, pPhysAddr);
}

static inline NV_STATUS iovaspaceSetPteInfo_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceSetPteInfo__(staticCast(pVAS, OBJVASPACE), pGpu, pParams);
}

static inline NV_STATUS iovaspaceFreeV2_DISPATCH(struct OBJIOVASPACE *pVAS, NvU64 vAddr, NvU64 *pSize) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceFreeV2__(staticCast(pVAS, OBJVASPACE), vAddr, pSize);
}

// Virtual method declarations and/or inline definitions
NV_STATUS iovaspaceConstruct__IMPL(struct OBJIOVASPACE *pVAS, NvU32 classId, NvU32 vaspaceId, NvU64 vaStart, NvU64 vaLimit, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 flags);

NV_STATUS iovaspaceAlloc_IMPL(struct OBJIOVASPACE *pVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSizeLockMask, VAS_ALLOC_FLAGS flags, NvU64 *pAddr);

NV_STATUS iovaspaceFree_IMPL(struct OBJIOVASPACE *pVAS, NvU64 vAddr);

NV_STATUS iovaspaceApplyDefaultAlignment_IMPL(struct OBJIOVASPACE *pVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pAlign, NvU64 *pSize, NvU64 *pPageSizeLockMask);

NV_STATUS iovaspaceIncAllocRefCnt_IMPL(struct OBJIOVASPACE *pVAS, NvU64 vAddr);

NvU64 iovaspaceGetVaStart_IMPL(struct OBJIOVASPACE *pVAS);

NvU64 iovaspaceGetVaLimit_IMPL(struct OBJIOVASPACE *pVAS);

NV_STATUS iovaspaceGetVasInfo_IMPL(struct OBJIOVASPACE *pVAS, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams);

// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


struct OBJIOVASPACE* iovaspaceFromId(NvU32 iovaspaceId);
struct OBJIOVASPACE* iovaspaceFromMapping(PIOVAMAPPING pIovaMapping);

#endif // _IOVASPACE_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_IO_VASPACE_NVOC_H_
