#ifndef _G_IO_VASPACE_NVOC_H_
#define _G_IO_VASPACE_NVOC_H_
#include "nvoc/runtime.h"

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

typedef struct OBJIOVASPACE *POBJIOVASPACE;

#ifndef __NVOC_CLASS_OBJIOVASPACE_TYPEDEF__
#define __NVOC_CLASS_OBJIOVASPACE_TYPEDEF__
typedef struct OBJIOVASPACE OBJIOVASPACE;
#endif /* __NVOC_CLASS_OBJIOVASPACE_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJIOVASPACE
#define __nvoc_class_id_OBJIOVASPACE 0x28ed9c
#endif /* __nvoc_class_id_OBJIOVASPACE */



typedef struct IOVAMAPPING IOVAMAPPING;
typedef struct IOVAMAPPING *PIOVAMAPPING;

// Opaque pointer for the OS layer to use
typedef struct OS_IOVA_MAPPING_DATA *POS_IOVA_MAPPING_DATA;

struct IOVAMAPPING
{
    NvU32 iovaspaceId;

    //
    // Refcount of the mapping.
    //
    // Each iovaspaceAcquireMapping() call increments the refcount, and each
    // iovaspaceReleaseMapping() call decrements it. Additionally, submappings
    // increment the refcount of their root mapping on creation and only
    // decrement it when they are destroyed.
    //
    // Mappings are destroyed when their refcount reaches 0.
    //
    // Notably a mapping can be destroyed regardless of its refcount with
    // iovaspaceDestroyMapping(). Destroying a root mapping destroys all of its
    // submappings as well.
    //
    NvU32 refcount;

    PMEMORY_DESCRIPTOR pPhysMemDesc;

    //
    // Maintain a hierarchy of IOVA mappings. The "root" mapping will generally
    // be tied to the root memory descriptor. That mapping can have submappings
    // within the same IOVA space that correspond to submemory descriptors of
    // the root memory descriptor.
    //
    // Also, the root memory descriptor may have multiple IOVA mappings (up to
    // one per IOVA space), so those need to be tracked in association directly
    // with the root memory descriptor.
    //
    // The memory descriptor (root or submemory) always points to a single IOVA
    // mapping. For root memory descriptors, that mapping is the head of a list
    // in which each mapping covers a unique IOVA space. For submemory
    // descriptors, there can only be one IOVA mapping, corresponding to the
    // IOVA space of the pGpu associated with the submemory descriptor.
    //
    union
    {
        struct IOVAMAPPING *pParent;
        struct IOVAMAPPING *pChildren;
    } link;

    //
    // For root mappings, this points to the next root mapping for the same
    // parent physical memory descriptor (e.g., a root mapping for a different
    // IOVA space).
    //
    // For submappings, this instead points to the next submapping of the
    // parent root mapping, since a submemory descriptor may only have a single
    // IOVA mapping (which is a submapping of an IOVA mapping on the root
    // memory descriptor).
    //
    struct IOVAMAPPING *pNext;

    // OS data associated with this mapping. Core RM doesn't touch this.
    POS_IOVA_MAPPING_DATA pOsData;

    //
    // If the memory is contiguous, this array consists of one element.
    // If the memory is discontiguous, this array is actually larger and has
    // one entry for each physical page in pPhysMemDesc. As a result, this
    // structure must be allocated from the heap.
    //
    RmPhysAddr iovaArray[1];
    // WARNING: DO NOT place anything behind the IOVA array!
};

/*!
 * Virtual address space for a system's IOMMU translation.
 */
#ifdef NVOC_IO_VASPACE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct OBJIOVASPACE {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJVASPACE __nvoc_base_OBJVASPACE;
    struct Object *__nvoc_pbase_Object;
    struct OBJVASPACE *__nvoc_pbase_OBJVASPACE;
    struct OBJIOVASPACE *__nvoc_pbase_OBJIOVASPACE;
    NV_STATUS (*__iovaspaceConstruct___)(struct OBJIOVASPACE *, NvU32, NvU32, NvU64, NvU64, NvU64, NvU64, NvU32);
    NV_STATUS (*__iovaspaceAlloc__)(struct OBJIOVASPACE *, NvU64, NvU64, NvU64, NvU64, NvU64, VAS_ALLOC_FLAGS, NvU64 *);
    NV_STATUS (*__iovaspaceFree__)(struct OBJIOVASPACE *, NvU64);
    NV_STATUS (*__iovaspaceApplyDefaultAlignment__)(struct OBJIOVASPACE *, const FB_ALLOC_INFO *, NvU64 *, NvU64 *, NvU64 *);
    NV_STATUS (*__iovaspaceIncAllocRefCnt__)(struct OBJIOVASPACE *, NvU64);
    NvU64 (*__iovaspaceGetVaStart__)(struct OBJIOVASPACE *);
    NvU64 (*__iovaspaceGetVaLimit__)(struct OBJIOVASPACE *);
    NV_STATUS (*__iovaspaceGetVasInfo__)(struct OBJIOVASPACE *, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *);
    NvBool (*__iovaspaceIsMirrored__)(struct OBJIOVASPACE *);
    NvBool (*__iovaspaceIsExternallyOwned__)(struct OBJIOVASPACE *);
    NvU32 (*__iovaspaceGetFlags__)(struct OBJIOVASPACE *);
    NvBool (*__iovaspaceIsAtsEnabled__)(struct OBJIOVASPACE *);
    NvU64 (*__iovaspaceGetBigPageSize__)(struct OBJIOVASPACE *);
    NV_STATUS (*__iovaspaceGetPteInfo__)(struct OBJIOVASPACE *, struct OBJGPU *, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *, RmPhysAddr *);
    PMEMORY_DESCRIPTOR (*__iovaspaceGetPageDirBase__)(struct OBJIOVASPACE *, struct OBJGPU *);
    PMEMORY_DESCRIPTOR (*__iovaspaceGetKernelPageDirBase__)(struct OBJIOVASPACE *, struct OBJGPU *);
    NvU64 (*__iovaspaceGetMapPageSize__)(struct OBJIOVASPACE *, struct OBJGPU *, EMEMBLOCK *);
    struct OBJEHEAP *(*__iovaspaceGetHeap__)(struct OBJIOVASPACE *);
    NvBool (*__iovaspaceIsFaultCapable__)(struct OBJIOVASPACE *);
    void (*__iovaspaceUnmap__)(struct OBJIOVASPACE *, struct OBJGPU *, const NvU64, const NvU64);
    void (*__iovaspaceInvalidateTlb__)(struct OBJIOVASPACE *, struct OBJGPU *, VAS_PTE_UPDATE_TYPE);
    NV_STATUS (*__iovaspacePinRootPageDir__)(struct OBJIOVASPACE *, struct OBJGPU *);
    void (*__iovaspaceUnpinRootPageDir__)(struct OBJIOVASPACE *, struct OBJGPU *);
    NV_STATUS (*__iovaspaceSetPteInfo__)(struct OBJIOVASPACE *, struct OBJGPU *, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *);
    NV_STATUS (*__iovaspaceFreeV2__)(struct OBJIOVASPACE *, NvU64, NvU64 *);
    NV_STATUS (*__iovaspaceGetPasid__)(struct OBJIOVASPACE *, NvU32 *);
    NV_STATUS (*__iovaspaceGetPageTableInfo__)(struct OBJIOVASPACE *, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *);
    NV_STATUS (*__iovaspaceReserveMempool__)(struct OBJIOVASPACE *, struct OBJGPU *, struct Device *, NvU64, NvU64, NvU32);
    NV_STATUS (*__iovaspaceMap__)(struct OBJIOVASPACE *, struct OBJGPU *, const NvU64, const NvU64, const MMU_MAP_TARGET *, const VAS_MAP_FLAGS);
    NvU64 mappingCount;
};

#ifndef __NVOC_CLASS_OBJIOVASPACE_TYPEDEF__
#define __NVOC_CLASS_OBJIOVASPACE_TYPEDEF__
typedef struct OBJIOVASPACE OBJIOVASPACE;
#endif /* __NVOC_CLASS_OBJIOVASPACE_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJIOVASPACE
#define __nvoc_class_id_OBJIOVASPACE 0x28ed9c
#endif /* __nvoc_class_id_OBJIOVASPACE */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJIOVASPACE;

#define __staticCast_OBJIOVASPACE(pThis) \
    ((pThis)->__nvoc_pbase_OBJIOVASPACE)

#ifdef __nvoc_io_vaspace_h_disabled
#define __dynamicCast_OBJIOVASPACE(pThis) ((OBJIOVASPACE*)NULL)
#else //__nvoc_io_vaspace_h_disabled
#define __dynamicCast_OBJIOVASPACE(pThis) \
    ((OBJIOVASPACE*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJIOVASPACE)))
#endif //__nvoc_io_vaspace_h_disabled


NV_STATUS __nvoc_objCreateDynamic_OBJIOVASPACE(OBJIOVASPACE**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJIOVASPACE(OBJIOVASPACE**, Dynamic*, NvU32);
#define __objCreate_OBJIOVASPACE(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJIOVASPACE((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define iovaspaceConstruct_(pVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags) iovaspaceConstruct__DISPATCH(pVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags)
#define iovaspaceAlloc(pVAS, size, align, rangeLo, rangeHi, pageSizeLockMask, flags, pAddr) iovaspaceAlloc_DISPATCH(pVAS, size, align, rangeLo, rangeHi, pageSizeLockMask, flags, pAddr)
#define iovaspaceFree(pVAS, vAddr) iovaspaceFree_DISPATCH(pVAS, vAddr)
#define iovaspaceApplyDefaultAlignment(pVAS, pAllocInfo, pAlign, pSize, pPageSizeLockMask) iovaspaceApplyDefaultAlignment_DISPATCH(pVAS, pAllocInfo, pAlign, pSize, pPageSizeLockMask)
#define iovaspaceIncAllocRefCnt(pVAS, vAddr) iovaspaceIncAllocRefCnt_DISPATCH(pVAS, vAddr)
#define iovaspaceGetVaStart(pVAS) iovaspaceGetVaStart_DISPATCH(pVAS)
#define iovaspaceGetVaLimit(pVAS) iovaspaceGetVaLimit_DISPATCH(pVAS)
#define iovaspaceGetVasInfo(pVAS, pParams) iovaspaceGetVasInfo_DISPATCH(pVAS, pParams)
#define iovaspaceIsMirrored(pVAS) iovaspaceIsMirrored_DISPATCH(pVAS)
#define iovaspaceIsExternallyOwned(pVAS) iovaspaceIsExternallyOwned_DISPATCH(pVAS)
#define iovaspaceGetFlags(pVAS) iovaspaceGetFlags_DISPATCH(pVAS)
#define iovaspaceIsAtsEnabled(pVAS) iovaspaceIsAtsEnabled_DISPATCH(pVAS)
#define iovaspaceGetBigPageSize(pVAS) iovaspaceGetBigPageSize_DISPATCH(pVAS)
#define iovaspaceGetPteInfo(pVAS, pGpu, pParams, pPhysAddr) iovaspaceGetPteInfo_DISPATCH(pVAS, pGpu, pParams, pPhysAddr)
#define iovaspaceGetPageDirBase(pVAS, pGpu) iovaspaceGetPageDirBase_DISPATCH(pVAS, pGpu)
#define iovaspaceGetKernelPageDirBase(pVAS, pGpu) iovaspaceGetKernelPageDirBase_DISPATCH(pVAS, pGpu)
#define iovaspaceGetMapPageSize(pVAS, pGpu, pMemBlock) iovaspaceGetMapPageSize_DISPATCH(pVAS, pGpu, pMemBlock)
#define iovaspaceGetHeap(pVAS) iovaspaceGetHeap_DISPATCH(pVAS)
#define iovaspaceIsFaultCapable(pVAS) iovaspaceIsFaultCapable_DISPATCH(pVAS)
#define iovaspaceUnmap(pVAS, pGpu, vaLo, vaHi) iovaspaceUnmap_DISPATCH(pVAS, pGpu, vaLo, vaHi)
#define iovaspaceInvalidateTlb(pVAS, pGpu, type) iovaspaceInvalidateTlb_DISPATCH(pVAS, pGpu, type)
#define iovaspacePinRootPageDir(pVAS, pGpu) iovaspacePinRootPageDir_DISPATCH(pVAS, pGpu)
#define iovaspaceUnpinRootPageDir(pVAS, pGpu) iovaspaceUnpinRootPageDir_DISPATCH(pVAS, pGpu)
#define iovaspaceSetPteInfo(pVAS, pGpu, pParams) iovaspaceSetPteInfo_DISPATCH(pVAS, pGpu, pParams)
#define iovaspaceFreeV2(pVAS, vAddr, pSize) iovaspaceFreeV2_DISPATCH(pVAS, vAddr, pSize)
#define iovaspaceGetPasid(pVAS, pPasid) iovaspaceGetPasid_DISPATCH(pVAS, pPasid)
#define iovaspaceGetPageTableInfo(pVAS, pParams) iovaspaceGetPageTableInfo_DISPATCH(pVAS, pParams)
#define iovaspaceReserveMempool(pVAS, pGpu, pDevice, size, pageSizeLockMask, flags) iovaspaceReserveMempool_DISPATCH(pVAS, pGpu, pDevice, size, pageSizeLockMask, flags)
#define iovaspaceMap(pVAS, pGpu, vaLo, vaHi, pTarget, flags) iovaspaceMap_DISPATCH(pVAS, pGpu, vaLo, vaHi, pTarget, flags)
NV_STATUS iovaspaceConstruct__IMPL(struct OBJIOVASPACE *pVAS, NvU32 classId, NvU32 vaspaceId, NvU64 vaStart, NvU64 vaLimit, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 flags);

static inline NV_STATUS iovaspaceConstruct__DISPATCH(struct OBJIOVASPACE *pVAS, NvU32 classId, NvU32 vaspaceId, NvU64 vaStart, NvU64 vaLimit, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 flags) {
    return pVAS->__iovaspaceConstruct___(pVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags);
}

NV_STATUS iovaspaceAlloc_IMPL(struct OBJIOVASPACE *pVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSizeLockMask, VAS_ALLOC_FLAGS flags, NvU64 *pAddr);

static inline NV_STATUS iovaspaceAlloc_DISPATCH(struct OBJIOVASPACE *pVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSizeLockMask, VAS_ALLOC_FLAGS flags, NvU64 *pAddr) {
    return pVAS->__iovaspaceAlloc__(pVAS, size, align, rangeLo, rangeHi, pageSizeLockMask, flags, pAddr);
}

NV_STATUS iovaspaceFree_IMPL(struct OBJIOVASPACE *pVAS, NvU64 vAddr);

static inline NV_STATUS iovaspaceFree_DISPATCH(struct OBJIOVASPACE *pVAS, NvU64 vAddr) {
    return pVAS->__iovaspaceFree__(pVAS, vAddr);
}

NV_STATUS iovaspaceApplyDefaultAlignment_IMPL(struct OBJIOVASPACE *pVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pAlign, NvU64 *pSize, NvU64 *pPageSizeLockMask);

static inline NV_STATUS iovaspaceApplyDefaultAlignment_DISPATCH(struct OBJIOVASPACE *pVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pAlign, NvU64 *pSize, NvU64 *pPageSizeLockMask) {
    return pVAS->__iovaspaceApplyDefaultAlignment__(pVAS, pAllocInfo, pAlign, pSize, pPageSizeLockMask);
}

NV_STATUS iovaspaceIncAllocRefCnt_IMPL(struct OBJIOVASPACE *pVAS, NvU64 vAddr);

static inline NV_STATUS iovaspaceIncAllocRefCnt_DISPATCH(struct OBJIOVASPACE *pVAS, NvU64 vAddr) {
    return pVAS->__iovaspaceIncAllocRefCnt__(pVAS, vAddr);
}

NvU64 iovaspaceGetVaStart_IMPL(struct OBJIOVASPACE *pVAS);

static inline NvU64 iovaspaceGetVaStart_DISPATCH(struct OBJIOVASPACE *pVAS) {
    return pVAS->__iovaspaceGetVaStart__(pVAS);
}

NvU64 iovaspaceGetVaLimit_IMPL(struct OBJIOVASPACE *pVAS);

static inline NvU64 iovaspaceGetVaLimit_DISPATCH(struct OBJIOVASPACE *pVAS) {
    return pVAS->__iovaspaceGetVaLimit__(pVAS);
}

NV_STATUS iovaspaceGetVasInfo_IMPL(struct OBJIOVASPACE *pVAS, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams);

static inline NV_STATUS iovaspaceGetVasInfo_DISPATCH(struct OBJIOVASPACE *pVAS, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams) {
    return pVAS->__iovaspaceGetVasInfo__(pVAS, pParams);
}

static inline NvBool iovaspaceIsMirrored_DISPATCH(struct OBJIOVASPACE *pVAS) {
    return pVAS->__iovaspaceIsMirrored__(pVAS);
}

static inline NvBool iovaspaceIsExternallyOwned_DISPATCH(struct OBJIOVASPACE *pVAS) {
    return pVAS->__iovaspaceIsExternallyOwned__(pVAS);
}

static inline NvU32 iovaspaceGetFlags_DISPATCH(struct OBJIOVASPACE *pVAS) {
    return pVAS->__iovaspaceGetFlags__(pVAS);
}

static inline NvBool iovaspaceIsAtsEnabled_DISPATCH(struct OBJIOVASPACE *pVAS) {
    return pVAS->__iovaspaceIsAtsEnabled__(pVAS);
}

static inline NvU64 iovaspaceGetBigPageSize_DISPATCH(struct OBJIOVASPACE *pVAS) {
    return pVAS->__iovaspaceGetBigPageSize__(pVAS);
}

static inline NV_STATUS iovaspaceGetPteInfo_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams, RmPhysAddr *pPhysAddr) {
    return pVAS->__iovaspaceGetPteInfo__(pVAS, pGpu, pParams, pPhysAddr);
}

static inline PMEMORY_DESCRIPTOR iovaspaceGetPageDirBase_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu) {
    return pVAS->__iovaspaceGetPageDirBase__(pVAS, pGpu);
}

static inline PMEMORY_DESCRIPTOR iovaspaceGetKernelPageDirBase_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu) {
    return pVAS->__iovaspaceGetKernelPageDirBase__(pVAS, pGpu);
}

static inline NvU64 iovaspaceGetMapPageSize_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, EMEMBLOCK *pMemBlock) {
    return pVAS->__iovaspaceGetMapPageSize__(pVAS, pGpu, pMemBlock);
}

static inline struct OBJEHEAP *iovaspaceGetHeap_DISPATCH(struct OBJIOVASPACE *pVAS) {
    return pVAS->__iovaspaceGetHeap__(pVAS);
}

static inline NvBool iovaspaceIsFaultCapable_DISPATCH(struct OBJIOVASPACE *pVAS) {
    return pVAS->__iovaspaceIsFaultCapable__(pVAS);
}

static inline void iovaspaceUnmap_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi) {
    pVAS->__iovaspaceUnmap__(pVAS, pGpu, vaLo, vaHi);
}

static inline void iovaspaceInvalidateTlb_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, VAS_PTE_UPDATE_TYPE type) {
    pVAS->__iovaspaceInvalidateTlb__(pVAS, pGpu, type);
}

static inline NV_STATUS iovaspacePinRootPageDir_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu) {
    return pVAS->__iovaspacePinRootPageDir__(pVAS, pGpu);
}

static inline void iovaspaceUnpinRootPageDir_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu) {
    pVAS->__iovaspaceUnpinRootPageDir__(pVAS, pGpu);
}

static inline NV_STATUS iovaspaceSetPteInfo_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams) {
    return pVAS->__iovaspaceSetPteInfo__(pVAS, pGpu, pParams);
}

static inline NV_STATUS iovaspaceFreeV2_DISPATCH(struct OBJIOVASPACE *pVAS, NvU64 vAddr, NvU64 *pSize) {
    return pVAS->__iovaspaceFreeV2__(pVAS, vAddr, pSize);
}

static inline NV_STATUS iovaspaceGetPasid_DISPATCH(struct OBJIOVASPACE *pVAS, NvU32 *pPasid) {
    return pVAS->__iovaspaceGetPasid__(pVAS, pPasid);
}

static inline NV_STATUS iovaspaceGetPageTableInfo_DISPATCH(struct OBJIOVASPACE *pVAS, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams) {
    return pVAS->__iovaspaceGetPageTableInfo__(pVAS, pParams);
}

static inline NV_STATUS iovaspaceReserveMempool_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, struct Device *pDevice, NvU64 size, NvU64 pageSizeLockMask, NvU32 flags) {
    return pVAS->__iovaspaceReserveMempool__(pVAS, pGpu, pDevice, size, pageSizeLockMask, flags);
}

static inline NV_STATUS iovaspaceMap_DISPATCH(struct OBJIOVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const MMU_MAP_TARGET *pTarget, const VAS_MAP_FLAGS flags) {
    return pVAS->__iovaspaceMap__(pVAS, pGpu, vaLo, vaHi, pTarget, flags);
}

void iovaspaceDestruct_IMPL(struct OBJIOVASPACE *pIOVAS);

#define __nvoc_iovaspaceDestruct(pIOVAS) iovaspaceDestruct_IMPL(pIOVAS)
NV_STATUS iovaspaceAcquireMapping_IMPL(struct OBJIOVASPACE *pIOVAS, PMEMORY_DESCRIPTOR pIovaMapping);

#ifdef __nvoc_io_vaspace_h_disabled
static inline NV_STATUS iovaspaceAcquireMapping(struct OBJIOVASPACE *pIOVAS, PMEMORY_DESCRIPTOR pIovaMapping) {
    NV_ASSERT_FAILED_PRECOMP("OBJIOVASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_io_vaspace_h_disabled
#define iovaspaceAcquireMapping(pIOVAS, pIovaMapping) iovaspaceAcquireMapping_IMPL(pIOVAS, pIovaMapping)
#endif //__nvoc_io_vaspace_h_disabled

void iovaspaceReleaseMapping_IMPL(struct OBJIOVASPACE *pIOVAS, PIOVAMAPPING pIovaMapping);

#ifdef __nvoc_io_vaspace_h_disabled
static inline void iovaspaceReleaseMapping(struct OBJIOVASPACE *pIOVAS, PIOVAMAPPING pIovaMapping) {
    NV_ASSERT_FAILED_PRECOMP("OBJIOVASPACE was disabled!");
}
#else //__nvoc_io_vaspace_h_disabled
#define iovaspaceReleaseMapping(pIOVAS, pIovaMapping) iovaspaceReleaseMapping_IMPL(pIOVAS, pIovaMapping)
#endif //__nvoc_io_vaspace_h_disabled

void iovaspaceDestroyMapping_IMPL(struct OBJIOVASPACE *pIOVAS, PIOVAMAPPING pIovaMapping);

#ifdef __nvoc_io_vaspace_h_disabled
static inline void iovaspaceDestroyMapping(struct OBJIOVASPACE *pIOVAS, PIOVAMAPPING pIovaMapping) {
    NV_ASSERT_FAILED_PRECOMP("OBJIOVASPACE was disabled!");
}
#else //__nvoc_io_vaspace_h_disabled
#define iovaspaceDestroyMapping(pIOVAS, pIovaMapping) iovaspaceDestroyMapping_IMPL(pIOVAS, pIovaMapping)
#endif //__nvoc_io_vaspace_h_disabled

#undef PRIVATE_FIELD


struct OBJIOVASPACE* iovaspaceFromId(NvU32 iovaspaceId);
struct OBJIOVASPACE* iovaspaceFromMapping(PIOVAMAPPING pIovaMapping);

//
// Helper that looks up the IOVAS from the mapping and then calls
// iovaspaceDestroyMapping().
//
void iovaMappingDestroy(PIOVAMAPPING pIovaMapping);

#endif // _IOVASPACE_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_IO_VASPACE_NVOC_H_
