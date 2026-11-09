
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
 * SPDX-FileCopyrightText: Copyright (c) 2021-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "class/cl00f8.h"
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

typedef struct
{
    NvBool bHandleTranslation : 1;
    NvBool bFixedGattIdx      : 1;
} FABRIC_ALLOC_FLAGS;

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


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__FABRIC_VASPACE;
struct NVOC_METADATA__OBJVASPACE;


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
    NvHandle hClient;
    NvHandle hDevice;
    NvBool bRpcAlloc;
    NvU32 PRIVATE_FIELD(flags);
    NODE *PRIVATE_FIELD(pFabricVaToGpaMap);
    NvU64 PRIVATE_FIELD(ucFabricBase);
    NvU64 PRIVATE_FIELD(ucFabricLimit);
    NvU64 PRIVATE_FIELD(ucFabricInUseSize);
    NvU64 PRIVATE_FIELD(ucFabricFreeSize);
    NvU64 PRIVATE_FIELD(ucEmulatedHandleBase);
    NvU64 PRIVATE_FIELD(ucEmulatedHandleLimit);
    NvU64 PRIVATE_FIELD(ucEmulatedHandleInUseSize);
    NvU64 PRIVATE_FIELD(ucEmulatedHandleFreeSize);
    NvU64 PRIVATE_FIELD(sysmemFabricBase);
    NvU64 PRIVATE_FIELD(sysmemFabricLimit);
    NvU32 PRIVATE_FIELD(gfid);
};


struct FABRIC_VASPACE_PRIVATE {

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
    NvHandle hClient;
    NvHandle hDevice;
    NvBool bRpcAlloc;
    NvU32 flags;
    NODE *pFabricVaToGpaMap;
    NvU64 ucFabricBase;
    NvU64 ucFabricLimit;
    NvU64 ucFabricInUseSize;
    NvU64 ucFabricFreeSize;
    NvU64 ucEmulatedHandleBase;
    NvU64 ucEmulatedHandleLimit;
    NvU64 ucEmulatedHandleInUseSize;
    NvU64 ucEmulatedHandleFreeSize;
    NvU64 sysmemFabricBase;
    NvU64 sysmemFabricLimit;
    NvU32 gfid;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__FABRIC_VASPACE {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJVASPACE metadata__OBJVASPACE;
};

#ifndef __nvoc_class_id_FABRIC_VASPACE
#define __nvoc_class_id_FABRIC_VASPACE 0x8c8f3du
typedef struct FABRIC_VASPACE FABRIC_VASPACE;
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

NV_STATUS __nvoc_objCreateDynamic_FABRIC_VASPACE(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_FABRIC_VASPACE(FABRIC_VASPACE**, Dynamic*, NvU32);
#define __objCreate_FABRIC_VASPACE(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags) \
    __nvoc_objCreate_FABRIC_VASPACE((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags))


// Wrapper macros for implementation functions
void fabricvaspaceDestruct_IMPL(struct FABRIC_VASPACE *pFabricVAS);
#define __nvoc_fabricvaspaceDestruct(pFabricVAS) fabricvaspaceDestruct_IMPL(pFabricVAS)

void fabricvaspaceBatchFree_IMPL(struct FABRIC_VASPACE *pFabricVAS, NvU64 *pAddr, NvU32 numAddr, NvU32 stride);
#ifdef __nvoc_fabric_vaspace_h_disabled
static inline void fabricvaspaceBatchFree(struct FABRIC_VASPACE *pFabricVAS, NvU64 *pAddr, NvU32 numAddr, NvU32 stride) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
}
#else // __nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceBatchFree(pFabricVAS, pAddr, numAddr, stride) fabricvaspaceBatchFree_IMPL(pFabricVAS, pAddr, numAddr, stride)
#endif // __nvoc_fabric_vaspace_h_disabled

NV_STATUS fabricvaspaceGetFreeEmulatedHandleHeap_IMPL(struct FABRIC_VASPACE *pFabricVAS, NvU64 *pFreeSize);
#ifdef __nvoc_fabric_vaspace_h_disabled
static inline NV_STATUS fabricvaspaceGetFreeEmulatedHandleHeap(struct FABRIC_VASPACE *pFabricVAS, NvU64 *pFreeSize) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceGetFreeEmulatedHandleHeap(pFabricVAS, pFreeSize) fabricvaspaceGetFreeEmulatedHandleHeap_IMPL(pFabricVAS, pFreeSize)
#endif // __nvoc_fabric_vaspace_h_disabled

NV_STATUS fabricvaspaceGetGpaMemdesc_IMPL(struct FABRIC_VASPACE *pFabricVAS, MEMORY_DESCRIPTOR *pFabricMemdesc, struct OBJGPU *pMappingGpu, NvBool bForcePhys, MEMORY_DESCRIPTOR **ppAdjustedMemdesc);
#ifdef __nvoc_fabric_vaspace_h_disabled
static inline NV_STATUS fabricvaspaceGetGpaMemdesc(struct FABRIC_VASPACE *pFabricVAS, MEMORY_DESCRIPTOR *pFabricMemdesc, struct OBJGPU *pMappingGpu, NvBool bForcePhys, MEMORY_DESCRIPTOR **ppAdjustedMemdesc) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceGetGpaMemdesc(pFabricVAS, pFabricMemdesc, pMappingGpu, bForcePhys, ppAdjustedMemdesc) fabricvaspaceGetGpaMemdesc_IMPL(pFabricVAS, pFabricMemdesc, pMappingGpu, bForcePhys, ppAdjustedMemdesc)
#endif // __nvoc_fabric_vaspace_h_disabled

void fabricvaspacePutGpaMemdesc_IMPL(struct FABRIC_VASPACE *pFabricVAS, MEMORY_DESCRIPTOR *pMemDesc);
#ifdef __nvoc_fabric_vaspace_h_disabled
static inline void fabricvaspacePutGpaMemdesc(struct FABRIC_VASPACE *pFabricVAS, MEMORY_DESCRIPTOR *pMemDesc) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
}
#else // __nvoc_fabric_vaspace_h_disabled
#define fabricvaspacePutGpaMemdesc(pFabricVAS, pMemDesc) fabricvaspacePutGpaMemdesc_IMPL(pFabricVAS, pMemDesc)
#endif // __nvoc_fabric_vaspace_h_disabled

NV_STATUS fabricvaspaceVaToGpaMapInsert_IMPL(struct FABRIC_VASPACE *pFabricVAS, NvU64 vAddr, MEMORY_DESCRIPTOR *pVidMemDesc, NvU64 offset);
#ifdef __nvoc_fabric_vaspace_h_disabled
static inline NV_STATUS fabricvaspaceVaToGpaMapInsert(struct FABRIC_VASPACE *pFabricVAS, NvU64 vAddr, MEMORY_DESCRIPTOR *pVidMemDesc, NvU64 offset) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceVaToGpaMapInsert(pFabricVAS, vAddr, pVidMemDesc, offset) fabricvaspaceVaToGpaMapInsert_IMPL(pFabricVAS, vAddr, pVidMemDesc, offset)
#endif // __nvoc_fabric_vaspace_h_disabled

void fabricvaspaceVaToGpaMapRemove_IMPL(struct FABRIC_VASPACE *pFabricVAS, NvU64 vAddr);
#ifdef __nvoc_fabric_vaspace_h_disabled
static inline void fabricvaspaceVaToGpaMapRemove(struct FABRIC_VASPACE *pFabricVAS, NvU64 vAddr) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
}
#else // __nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceVaToGpaMapRemove(pFabricVAS, vAddr) fabricvaspaceVaToGpaMapRemove_IMPL(pFabricVAS, vAddr)
#endif // __nvoc_fabric_vaspace_h_disabled

NV_STATUS fabricvaspaceAllocFixed_IMPL(struct FABRIC_VASPACE *pFabricVAS, NvU64 pageSize, NvU64 alignment, VAS_ALLOC_FLAGS flags, NvU64 base, NvU64 size);
#ifdef __nvoc_fabric_vaspace_h_disabled
static inline NV_STATUS fabricvaspaceAllocFixed(struct FABRIC_VASPACE *pFabricVAS, NvU64 pageSize, NvU64 alignment, VAS_ALLOC_FLAGS flags, NvU64 base, NvU64 size) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceAllocFixed(pFabricVAS, pageSize, alignment, flags, base, size) fabricvaspaceAllocFixed_IMPL(pFabricVAS, pageSize, alignment, flags, base, size)
#endif // __nvoc_fabric_vaspace_h_disabled

NV_STATUS fabricvaspaceMapPhysMemdesc_IMPL(struct FABRIC_VASPACE *pFabricVAS, MEMORY_DESCRIPTOR *pFabricMemDesc, NvU64 fabricOffset, MEMORY_DESCRIPTOR *pPhysMemDesc, NvU64 physOffset, NvU64 physMapLength, NvU32 flags);
#ifdef __nvoc_fabric_vaspace_h_disabled
static inline NV_STATUS fabricvaspaceMapPhysMemdesc(struct FABRIC_VASPACE *pFabricVAS, MEMORY_DESCRIPTOR *pFabricMemDesc, NvU64 fabricOffset, MEMORY_DESCRIPTOR *pPhysMemDesc, NvU64 physOffset, NvU64 physMapLength, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceMapPhysMemdesc(pFabricVAS, pFabricMemDesc, fabricOffset, pPhysMemDesc, physOffset, physMapLength, flags) fabricvaspaceMapPhysMemdesc_IMPL(pFabricVAS, pFabricMemDesc, fabricOffset, pPhysMemDesc, physOffset, physMapLength, flags)
#endif // __nvoc_fabric_vaspace_h_disabled

void fabricvaspaceUnmapPhysMemdesc_IMPL(struct FABRIC_VASPACE *pFabricVAS, MEMORY_DESCRIPTOR *pFabricMemDesc, NvU64 fabricOffset, NvU64 physMapLength);
#ifdef __nvoc_fabric_vaspace_h_disabled
static inline void fabricvaspaceUnmapPhysMemdesc(struct FABRIC_VASPACE *pFabricVAS, MEMORY_DESCRIPTOR *pFabricMemDesc, NvU64 fabricOffset, NvU64 physMapLength) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
}
#else // __nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceUnmapPhysMemdesc(pFabricVAS, pFabricMemDesc, fabricOffset, physMapLength) fabricvaspaceUnmapPhysMemdesc_IMPL(pFabricVAS, pFabricMemDesc, fabricOffset, physMapLength)
#endif // __nvoc_fabric_vaspace_h_disabled

NV_STATUS fabricvaspaceInitUCRange_IMPL(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, NvU64 ucFabricBase, NvU64 ucFabricSize);
#ifdef __nvoc_fabric_vaspace_h_disabled
static inline NV_STATUS fabricvaspaceInitUCRange(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, NvU64 ucFabricBase, NvU64 ucFabricSize) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceInitUCRange(pFabricVAS, pGpu, ucFabricBase, ucFabricSize) fabricvaspaceInitUCRange_IMPL(pFabricVAS, pGpu, ucFabricBase, ucFabricSize)
#endif // __nvoc_fabric_vaspace_h_disabled

NV_STATUS fabricvaspaceInitUCEmulatedHandleRange_IMPL(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, NvU64 ucEmulatedHandleBase, NvU64 ucEmulatedHandleSize);
#ifdef __nvoc_fabric_vaspace_h_disabled
static inline NV_STATUS fabricvaspaceInitUCEmulatedHandleRange(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, NvU64 ucEmulatedHandleBase, NvU64 ucEmulatedHandleSize) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceInitUCEmulatedHandleRange(pFabricVAS, pGpu, ucEmulatedHandleBase, ucEmulatedHandleSize) fabricvaspaceInitUCEmulatedHandleRange_IMPL(pFabricVAS, pGpu, ucEmulatedHandleBase, ucEmulatedHandleSize)
#endif // __nvoc_fabric_vaspace_h_disabled

void fabricvaspaceClearUCRanges_IMPL(struct FABRIC_VASPACE *pFabricVAS);
#ifdef __nvoc_fabric_vaspace_h_disabled
static inline void fabricvaspaceClearUCRanges(struct FABRIC_VASPACE *pFabricVAS) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
}
#else // __nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceClearUCRanges(pFabricVAS) fabricvaspaceClearUCRanges_IMPL(pFabricVAS)
#endif // __nvoc_fabric_vaspace_h_disabled

NV_STATUS fabricvaspaceInitSysmemFlaRange_IMPL(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, NvU64 sysmemFabricBase, NvU64 sysmemFabricSize);
#ifdef __nvoc_fabric_vaspace_h_disabled
static inline NV_STATUS fabricvaspaceInitSysmemFlaRange(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, NvU64 sysmemFabricBase, NvU64 sysmemFabricSize) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceInitSysmemFlaRange(pFabricVAS, pGpu, sysmemFabricBase, sysmemFabricSize) fabricvaspaceInitSysmemFlaRange_IMPL(pFabricVAS, pGpu, sysmemFabricBase, sysmemFabricSize)
#endif // __nvoc_fabric_vaspace_h_disabled

void fabricvaspaceClearSysmemFlaRange_IMPL(struct FABRIC_VASPACE *pFabricVAS);
#ifdef __nvoc_fabric_vaspace_h_disabled
static inline void fabricvaspaceClearSysmemFlaRange(struct FABRIC_VASPACE *pFabricVAS) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
}
#else // __nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceClearSysmemFlaRange(pFabricVAS) fabricvaspaceClearSysmemFlaRange_IMPL(pFabricVAS)
#endif // __nvoc_fabric_vaspace_h_disabled

NV_STATUS fabricvaspaceAllocPtrResources_IMPL(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, NvU64 allocSize, NvU64 alignment, NvU64 pageSize, NvU64 fabricBaseOverride, FABRIC_ALLOC_FLAGS fabricFlags, VAS_ALLOC_FLAGS flags, NvU64 **ppAddr, NvU32 *pNumAddr);
#ifdef __nvoc_fabric_vaspace_h_disabled
static inline NV_STATUS fabricvaspaceAllocPtrResources(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, NvU64 allocSize, NvU64 alignment, NvU64 pageSize, NvU64 fabricBaseOverride, FABRIC_ALLOC_FLAGS fabricFlags, VAS_ALLOC_FLAGS flags, NvU64 **ppAddr, NvU32 *pNumAddr) {
    NV_ASSERT_FAILED_PRECOMP("FABRIC_VASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_fabric_vaspace_h_disabled
#define fabricvaspaceAllocPtrResources(pFabricVAS, pGpu, allocSize, alignment, pageSize, fabricBaseOverride, fabricFlags, flags, ppAddr, pNumAddr) fabricvaspaceAllocPtrResources_IMPL(pFabricVAS, pGpu, allocSize, alignment, pageSize, fabricBaseOverride, fabricFlags, flags, ppAddr, pNumAddr)
#endif // __nvoc_fabric_vaspace_h_disabled


// Wrapper macros for halified functions
#define fabricvaspaceConstruct__FNPTR(pFabricVAS) pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceConstruct___
#define fabricvaspaceConstruct_(pFabricVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags) fabricvaspaceConstruct__DISPATCH(pFabricVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags)
#define fabricvaspaceAlloc_FNPTR(pFabricVAS) pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceAlloc__
#define fabricvaspaceAlloc(pFabricVAS, size, align, rangeLo, rangeHi, pageSize, flags, pAddr) fabricvaspaceAlloc_DISPATCH(pFabricVAS, size, align, rangeLo, rangeHi, pageSize, flags, pAddr)
#define fabricvaspaceFree_FNPTR(pFabricVAS) pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceFree__
#define fabricvaspaceFree(pFabricVAS, vAddr) fabricvaspaceFree_DISPATCH(pFabricVAS, vAddr)
#define fabricvaspaceMap_FNPTR(pFabricVAS) pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceMap__
#define fabricvaspaceMap(pFabricVAS, pGpu, vaLo, vaHi, pTarget, flags) fabricvaspaceMap_DISPATCH(pFabricVAS, pGpu, vaLo, vaHi, pTarget, flags)
#define fabricvaspaceUnmap_FNPTR(pFabricVAS) pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceUnmap__
#define fabricvaspaceUnmap(pFabricVAS, pGpu, vaLo, vaHi) fabricvaspaceUnmap_DISPATCH(pFabricVAS, pGpu, vaLo, vaHi)
#define fabricvaspaceApplyDefaultAlignment_FNPTR(pFabricVAS) pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceApplyDefaultAlignment__
#define fabricvaspaceApplyDefaultAlignment(pFabricVAS, pAllocInfo, pAlign, pSize, pPageSizeLockMask) fabricvaspaceApplyDefaultAlignment_DISPATCH(pFabricVAS, pAllocInfo, pAlign, pSize, pPageSizeLockMask)
#define fabricvaspaceGetVasInfo_FNPTR(pFabricVAS) pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetVasInfo__
#define fabricvaspaceGetVasInfo(pFabricVAS, pParams) fabricvaspaceGetVasInfo_DISPATCH(pFabricVAS, pParams)
#define fabricvaspacePinRootPageDir_FNPTR(pFabricVAS) pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspacePinRootPageDir__
#define fabricvaspacePinRootPageDir(pFabricVAS, pGpu) fabricvaspacePinRootPageDir_DISPATCH(pFabricVAS, pGpu)
#define fabricvaspaceUnpinRootPageDir_FNPTR(pFabricVAS) pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceUnpinRootPageDir__
#define fabricvaspaceUnpinRootPageDir(pFabricVAS, pGpu) fabricvaspaceUnpinRootPageDir_DISPATCH(pFabricVAS, pGpu)
#define fabricvaspaceInvalidateTlb_FNPTR(pFabricVAS) pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceInvalidateTlb__
#define fabricvaspaceInvalidateTlb(pFabricVAS, pUnused, type) fabricvaspaceInvalidateTlb_DISPATCH(pFabricVAS, pUnused, type)
#define fabricvaspaceGetFreeHeap_FNPTR(pFabricVAS) pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetFreeHeap__
#define fabricvaspaceGetFreeHeap(pFabricVAS, pFreeSize) fabricvaspaceGetFreeHeap_DISPATCH(pFabricVAS, pFreeSize)
#define fabricvaspaceGetPageLevelInfo_FNPTR(pFabricVAS) pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetPageLevelInfo__
#define fabricvaspaceGetPageLevelInfo(pFabricVAS, pGpu, pParams) fabricvaspaceGetPageLevelInfo_DISPATCH(pFabricVAS, pGpu, pParams)
#define fabricvaspaceIsInUse_FNPTR(pFabricVAS) pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceIsInUse__
#define fabricvaspaceIsInUse(pFabricVAS) fabricvaspaceIsInUse_DISPATCH(pFabricVAS)
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
#define fabricvaspaceGetGmmuFmt_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetGmmuFmt__
#define fabricvaspaceGetGmmuFmt(pVAS, pGpu) fabricvaspaceGetGmmuFmt_DISPATCH(pVAS, pGpu)
#define fabricvaspaceGetMmuWalker_FNPTR(pVAS) pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetMmuWalker__
#define fabricvaspaceGetMmuWalker(pVAS, pGpu, ppWalk) fabricvaspaceGetMmuWalker_DISPATCH(pVAS, pGpu, ppWalk)
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
    return pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceConstruct___(staticCast(pFabricVAS, OBJVASPACE), classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags);
}

static inline NV_STATUS fabricvaspaceAlloc_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSize, VAS_ALLOC_FLAGS flags, NvU64 *pAddr) {
    return pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceAlloc__(staticCast(pFabricVAS, OBJVASPACE), size, align, rangeLo, rangeHi, pageSize, flags, pAddr);
}

static inline NV_STATUS fabricvaspaceFree_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, NvU64 vAddr) {
    return pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceFree__(staticCast(pFabricVAS, OBJVASPACE), vAddr);
}

static inline NV_STATUS fabricvaspaceMap_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const MMU_MAP_TARGET *pTarget, const VAS_MAP_FLAGS flags) {
    return pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceMap__(staticCast(pFabricVAS, OBJVASPACE), pGpu, vaLo, vaHi, pTarget, flags);
}

static inline void fabricvaspaceUnmap_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi) {
    pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceUnmap__(staticCast(pFabricVAS, OBJVASPACE), pGpu, vaLo, vaHi);
}

static inline NV_STATUS fabricvaspaceApplyDefaultAlignment_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pAlign, NvU64 *pSize, NvU64 *pPageSizeLockMask) {
    return pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceApplyDefaultAlignment__(staticCast(pFabricVAS, OBJVASPACE), pAllocInfo, pAlign, pSize, pPageSizeLockMask);
}

static inline NV_STATUS fabricvaspaceGetVasInfo_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams) {
    return pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetVasInfo__(staticCast(pFabricVAS, OBJVASPACE), pParams);
}

static inline NV_STATUS fabricvaspacePinRootPageDir_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu) {
    return pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspacePinRootPageDir__(staticCast(pFabricVAS, OBJVASPACE), pGpu);
}

static inline void fabricvaspaceUnpinRootPageDir_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu) {
    pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceUnpinRootPageDir__(staticCast(pFabricVAS, OBJVASPACE), pGpu);
}

static inline NV_STATUS fabricvaspaceInvalidateTlb_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pUnused, VAS_PTE_UPDATE_TYPE type) {
    return pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceInvalidateTlb__(staticCast(pFabricVAS, OBJVASPACE), pUnused, type);
}

static inline NV_STATUS fabricvaspaceGetFreeHeap_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, NvU64 *pFreeSize) {
    return pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetFreeHeap__(staticCast(pFabricVAS, OBJVASPACE), pFreeSize);
}

static inline NV_STATUS fabricvaspaceGetPageLevelInfo_DISPATCH(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS *pParams) {
    return pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetPageLevelInfo__(staticCast(pFabricVAS, OBJVASPACE), pGpu, pParams);
}

static inline NvBool fabricvaspaceIsInUse_DISPATCH(struct FABRIC_VASPACE *pFabricVAS) {
    return pFabricVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceIsInUse__(staticCast(pFabricVAS, OBJVASPACE));
}

static inline NV_STATUS fabricvaspaceIncAllocRefCnt_DISPATCH(struct FABRIC_VASPACE *pVAS, NvU64 vAddr) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceIncAllocRefCnt__(staticCast(pVAS, OBJVASPACE), vAddr);
}

static inline NvU64 fabricvaspaceGetVaStart_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetVaStart__(staticCast(pVAS, OBJVASPACE));
}

static inline NvU64 fabricvaspaceGetVaLimit_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetVaLimit__(staticCast(pVAS, OBJVASPACE));
}

static inline NvU32 fabricvaspaceGetFlags_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetFlags__(staticCast(pVAS, OBJVASPACE));
}

static inline NV_STATUS fabricvaspaceReserveMempool_DISPATCH(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu, struct Device *pDevice, NvU64 size, NvU64 pageSizeLockMask, NvU32 flags) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceReserveMempool__(staticCast(pVAS, OBJVASPACE), pGpu, pDevice, size, pageSizeLockMask, flags);
}

static inline OBJEHEAP * fabricvaspaceGetHeap_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetHeap__(staticCast(pVAS, OBJVASPACE));
}

static inline NvU64 fabricvaspaceGetMapPageSize_DISPATCH(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu, EMEMBLOCK *pMemBlock) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetMapPageSize__(staticCast(pVAS, OBJVASPACE), pGpu, pMemBlock);
}

static inline NvU64 fabricvaspaceGetBigPageSize_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetBigPageSize__(staticCast(pVAS, OBJVASPACE));
}

static inline NvBool fabricvaspaceIsFaultCapable_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceIsFaultCapable__(staticCast(pVAS, OBJVASPACE));
}

static inline NvBool fabricvaspaceIsExternallyOwned_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceIsExternallyOwned__(staticCast(pVAS, OBJVASPACE));
}

static inline NvBool fabricvaspaceIsAtsEnabled_DISPATCH(struct FABRIC_VASPACE *pVAS) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceIsAtsEnabled__(staticCast(pVAS, OBJVASPACE));
}

static inline NV_STATUS fabricvaspaceGetPasid_DISPATCH(struct FABRIC_VASPACE *pVAS, NvU32 *pPasid) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetPasid__(staticCast(pVAS, OBJVASPACE), pPasid);
}

static inline PMEMORY_DESCRIPTOR fabricvaspaceGetPageDirBase_DISPATCH(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetPageDirBase__(staticCast(pVAS, OBJVASPACE), pGpu);
}

static inline const GMMU_FMT * fabricvaspaceGetGmmuFmt_DISPATCH(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetGmmuFmt__(staticCast(pVAS, OBJVASPACE), pGpu);
}

static inline NV_STATUS fabricvaspaceGetMmuWalker_DISPATCH(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu, MMU_WALK **ppWalk) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetMmuWalker__(staticCast(pVAS, OBJVASPACE), pGpu, ppWalk);
}

static inline NV_STATUS fabricvaspaceGetPageTableInfo_DISPATCH(struct FABRIC_VASPACE *pVAS, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetPageTableInfo__(staticCast(pVAS, OBJVASPACE), pParams);
}

static inline NV_STATUS fabricvaspaceGetPteInfo_DISPATCH(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams, RmPhysAddr *pPhysAddr) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceGetPteInfo__(staticCast(pVAS, OBJVASPACE), pGpu, pParams, pPhysAddr);
}

static inline NV_STATUS fabricvaspaceSetPteInfo_DISPATCH(struct FABRIC_VASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceSetPteInfo__(staticCast(pVAS, OBJVASPACE), pGpu, pParams);
}

static inline NV_STATUS fabricvaspaceFreeV2_DISPATCH(struct FABRIC_VASPACE *pVAS, NvU64 vAddr, NvU64 *pSize) {
    return pVAS->__nvoc_base_OBJVASPACE.__nvoc_metadata_ptr->vtable.__vaspaceFreeV2__(staticCast(pVAS, OBJVASPACE), vAddr, pSize);
}

// Virtual method declarations and/or inline definitions
NV_STATUS fabricvaspaceConstruct__IMPL(struct FABRIC_VASPACE *pFabricVAS, NvU32 classId, NvU32 vaspaceId, NvU64 vaStart, NvU64 vaLimit, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 flags);

NV_STATUS fabricvaspaceAlloc_IMPL(struct FABRIC_VASPACE *pFabricVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSize, VAS_ALLOC_FLAGS flags, NvU64 *pAddr);

NV_STATUS fabricvaspaceFree_IMPL(struct FABRIC_VASPACE *pFabricVAS, NvU64 vAddr);

NV_STATUS fabricvaspaceMap_IMPL(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const MMU_MAP_TARGET *pTarget, const VAS_MAP_FLAGS flags);

void fabricvaspaceUnmap_IMPL(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi);

NV_STATUS fabricvaspaceApplyDefaultAlignment_IMPL(struct FABRIC_VASPACE *pFabricVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pAlign, NvU64 *pSize, NvU64 *pPageSizeLockMask);

NV_STATUS fabricvaspaceGetVasInfo_IMPL(struct FABRIC_VASPACE *pFabricVAS, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams);

NV_STATUS fabricvaspacePinRootPageDir_IMPL(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu);

void fabricvaspaceUnpinRootPageDir_IMPL(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu);

NV_STATUS fabricvaspaceInvalidateTlb_IMPL(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pUnused, VAS_PTE_UPDATE_TYPE type);

NV_STATUS fabricvaspaceGetFreeHeap_IMPL(struct FABRIC_VASPACE *pFabricVAS, NvU64 *pFreeSize);

NV_STATUS fabricvaspaceGetPageLevelInfo_IMPL(struct FABRIC_VASPACE *pFabricVAS, struct OBJGPU *pGpu, NV90F1_CTRL_VASPACE_GET_PAGE_LEVEL_INFO_PARAMS *pParams);

NvBool fabricvaspaceIsInUse_IMPL(struct FABRIC_VASPACE *pFabricVAS);

// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
static inline NvU64 fabricvaspaceGetUCFlaStart(struct FABRIC_VASPACE *pFabricVAS){
    struct FABRIC_VASPACE_PRIVATE *pFabricVAS_PRIVATE = (struct FABRIC_VASPACE_PRIVATE *)pFabricVAS;
    return pFabricVAS_PRIVATE->ucFabricBase;
}

static inline NvU64 fabricvaspaceGetUCFlaLimit(struct FABRIC_VASPACE *pFabricVAS){
    struct FABRIC_VASPACE_PRIVATE *pFabricVAS_PRIVATE = (struct FABRIC_VASPACE_PRIVATE *)pFabricVAS;
    return pFabricVAS_PRIVATE->ucFabricLimit;
}

static inline NvU64 fabricvaspaceGetUCEmulatedHandleFlaStart(struct FABRIC_VASPACE *pFabricVAS){
    struct FABRIC_VASPACE_PRIVATE *pFabricVAS_PRIVATE = (struct FABRIC_VASPACE_PRIVATE *)pFabricVAS;
    return pFabricVAS_PRIVATE->ucEmulatedHandleBase;
}

static inline NvU64 fabricvaspaceGetUCEmulatedHandleFlaLimit(struct FABRIC_VASPACE *pFabricVAS){
    struct FABRIC_VASPACE_PRIVATE *pFabricVAS_PRIVATE = (struct FABRIC_VASPACE_PRIVATE *)pFabricVAS;
    return pFabricVAS_PRIVATE->ucEmulatedHandleLimit;
}

static inline NvU64 fabricvaspaceGetSysmemFlaStart(struct FABRIC_VASPACE *pFabricVAS){
    struct FABRIC_VASPACE_PRIVATE *pFabricVAS_PRIVATE = (struct FABRIC_VASPACE_PRIVATE *)pFabricVAS;
    return pFabricVAS_PRIVATE->sysmemFabricBase;
}

static inline NvU64 fabricvaspaceGetSysmemFlaLimit(struct FABRIC_VASPACE *pFabricVAS){
    struct FABRIC_VASPACE_PRIVATE *pFabricVAS_PRIVATE = (struct FABRIC_VASPACE_PRIVATE *)pFabricVAS;
    return pFabricVAS_PRIVATE->sysmemFabricLimit;
}

#undef PRIVATE_FIELD


#endif // FABRIC_VASPACE_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_FABRIC_VASPACE_NVOC_H_
