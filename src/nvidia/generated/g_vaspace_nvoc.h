
#ifndef _G_VASPACE_NVOC_H_
#define _G_VASPACE_NVOC_H_

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
#pragma once
#include "g_vaspace_nvoc.h"

#ifndef _VASPACE_H_
#define _VASPACE_H_

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
* Module: VASPACE.H                                                         *
*       Defines and structures used for Virtual Address Space Object.       *
\***************************************************************************/

#include "ctrl/ctrl0080/ctrl0080dma.h"

#include "core/core.h"
#include "resserv/rs_client.h"
#include "containers/btree.h"
#include "gpu/mem_mgr/heap_base.h"
#include "gpu/mem_mgr/mem_desc.h"

typedef struct OBJEHEAP OBJEHEAP;
typedef struct EMEMBLOCK EMEMBLOCK;
typedef struct OBJVASPACE *POBJVASPACE;
typedef struct VASPACE VASPACE, *PVASPACE;

struct VirtMemAllocator;

#ifndef __NVOC_CLASS_VirtMemAllocator_TYPEDEF__
#define __NVOC_CLASS_VirtMemAllocator_TYPEDEF__
typedef struct VirtMemAllocator VirtMemAllocator;
#endif /* __NVOC_CLASS_VirtMemAllocator_TYPEDEF__ */

#ifndef __nvoc_class_id_VirtMemAllocator
#define __nvoc_class_id_VirtMemAllocator 0x899e48
#endif /* __nvoc_class_id_VirtMemAllocator */


typedef struct MMU_MAP_TARGET       MMU_MAP_TARGET;
typedef struct NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS;

struct Device;

#ifndef __NVOC_CLASS_Device_TYPEDEF__
#define __NVOC_CLASS_Device_TYPEDEF__
typedef struct Device Device;
#endif /* __NVOC_CLASS_Device_TYPEDEF__ */

#ifndef __nvoc_class_id_Device
#define __nvoc_class_id_Device 0xe0ac20
#endif /* __nvoc_class_id_Device */



typedef struct
{
    NvBool bReverse : 1;
    NvBool bPreferSysmemPageTables : 1;
    NvBool bExternallyManaged : 1;
    NvBool bLazy : 1;
    NvBool bSparse : 1;
    NvBool bPrivileged : 1;
    NvBool bClientAllocation : 1;
    NvBool bFixedAddressRange : 1;
    NvBool bFixedAddressAllocate : 1;
    NvBool bForceContig : 1;
    NvBool bForceNonContig : 1;

    //
    // Using this flag may have security implications. So. use it only when
    // you are sure about its usage.
    //
    NvBool bSkipTlbInvalidateOnFree : 1;
} VAS_ALLOC_FLAGS;

#define VAS_EHEAP_OWNER_NVRM NvU32_BUILD('n','v','r','m')
#define VAS_EHEAP_OWNER_RSVD NvU32_BUILD('r','s','v','d')

typedef struct
{
    NvBool bRemap : 1; //<! Indicates to override existing mappings.
} VAS_MAP_FLAGS;

typedef enum
{
    PTE_UPGRADE,
    PTE_DOWNGRADE
} VAS_PTE_UPDATE_TYPE;

//
// OBJVASPACE creation flags
//
// BAR                        Used by one of the BAR apertures
// SCRATCH_INVAL              Use scratch page instead of invalid bit
// PARTIAL_INVAL              Do not fully invalidate when validating PTE
// MINIMIZE_PTETABLE_SIZE     Size PTE arrays minimally to
// RETRY_PTE_ALLOC_IN_SYS     PTE allocation falls back to sysmem
// FULL_PTE                   Initialize a full PTE
// BAR_BAR1                   Used for BAR1
// BAR_BAR2                   Used for BAR2 (unused)
// BAR_IFB                    Used for IFB
// PERFMON                    Used for Perfmon
// PMU                        Used for PMU
// SET_MIRRORED               <DEPRECATED.........>
//                            This flag will create a privileged PDB as part of this vaspace
//                            This new PDB will mirror all of the allocations made in the
//                            original PDB. The first PDE is considered privileged for this
//                            address space.
// SHARED_MANAGEMENT          Enables mode where only a portion of the VAS is managed
//                            and the page directory may be allocated/set externally.
// ALLOW_ZERO_ADDRESS         Explicitly allows the base VAS address to start at 0.
//                            Normally 0 is reserved to distinguish NULL pointers.
//
// BIG_PAGE_SIZE              Field that specifies the big page size to be used.
//                            DEFAULT is used till GM10X. GM20X and later, uses
//                            custom value for big page size.
//   SIZE_DEFAULT             Lets RM pick the default value
//   SIZE_64K                 Uses 64K as big page size for this VA space
//   SIZE_128K                Uses 128K as big page size for this VA space
//
// MMU_FMT_VA_BITS            Selects the MMU format of the VA space by the number
//                            of VA bits supported.
//   DEFAULT                  RM picks the default for the underlying MMU HW.
//   40                       Fermi+ 40-bit (2-level) format.
//   49                       Pascal+ 49-bit (5-level) format.
//
// ENABLE_VMM                 <DEPRECATED.........>
//                            Temp flag to enable new VMM code path on select
//                            VA spaces (e.g. client but not BAR1/PMU VAS).
//
// ZERO_OLD_STRUCT            Deprecated.
//
// ENABLE_FAULTING            This address space is participating in UVM.
//                            RM will enable page faulting for all channels that will be
//                            associated with this address space.
//
// IS_UVM_MANAGED             This flag will replace the SET_MIRRORED flag. It is used to
//                            denote that this VASpace is participating in UVM.
//
// ENABLE_ATS                 This address space has ATS enabled.
//
//
// ALLOW_PAGES_IN_PHYS_MEM_SUBALLOCATOR This flag when set will allow page table allocations
//                                      to be routed to suballocator of the current process
//                                      requesting mapping. If no suballocator, allocations
//                                      will fallback to global heap.
//
// VASPACE_FLAGS_INVALIDATE_SCOPE_NVLINK_TLB  This flag must be used by the VASs which use
//                                            the NVLink MMU.
//
#define VASPACE_FLAGS_NONE                                        0
#define VASPACE_FLAGS_BAR                                         NVBIT(0)
#define VASPACE_FLAGS_SCRATCH_INVAL                               NVBIT(1)
#define VASPACE_FLAGS_ENABLE_ATS                                  NVBIT(2)
#define VASPACE_FLAGS_RESTRICTED_RM_INTERNAL_VALIMITS             NVBIT(3)
#define VASPACE_FLAGS_MINIMIZE_PTETABLE_SIZE                      NVBIT(4)
#define VASPACE_FLAGS_RETRY_PTE_ALLOC_IN_SYS                      NVBIT(5)
#define VASPACE_FLAGS_REQUIRE_FIXED_OFFSET                        NVBIT(6)
#define VASPACE_FLAGS_BAR_BAR1                                    NVBIT(7)
#define VASPACE_FLAGS_BAR_BAR2                                    NVBIT(8)
#define VASPACE_FLAGS_BAR_IFB                                     NVBIT(9)
#define VASPACE_FLAGS_PERFMON                                     NVBIT(10)
#define VASPACE_FLAGS_PMU                                         NVBIT(11)
#define VASPACE_FLAGS_PTETABLE_PMA_MANAGED                        NVBIT(14)
#define VASPACE_FLAGS_INVALIDATE_SCOPE_NVLINK_TLB                 NVBIT(15)
#define VASPACE_FLAGS_DISABLE_SPLIT_VAS                           NVBIT(16)
#define VASPACE_FLAGS_SET_MIRRORED                                NVBIT(17)
#define VASPACE_FLAGS_SHARED_MANAGEMENT                           NVBIT(18)
#define VASPACE_FLAGS_ALLOW_ZERO_ADDRESS                          NVBIT(19)
#define VASPACE_FLAGS_SKIP_SCRUB_MEMPOOL                          NVBIT(20)
#define NV_VASPACE_FLAGS_BIG_PAGE_SIZE                            22:21
#define NV_VASPACE_FLAGS_BIG_PAGE_SIZE_DEFAULT                    0x00000000
#define NV_VASPACE_FLAGS_BIG_PAGE_SIZE_64K                        0x00000001
#define NV_VASPACE_FLAGS_BIG_PAGE_SIZE_128K                       0x00000002
#define VASPACE_FLAGS_HDA                                         NVBIT(23)
#define VASPACE_FLAGS_FLA                                         NVBIT(24) // Soon to be deprecated and removed.
                                                                            // Used by legacy FLA implementation.
#define VASPACE_FLAGS_HWPM                                        NVBIT(25)
#define VASPACE_FLAGS_ENABLE_VMM                                  NVBIT(26)
#define VASPACE_FLAGS_OPTIMIZE_PTETABLE_MEMPOOL_USAGE             NVBIT(27)
#define VASPACE_FLAGS_REVERSE                                     NVBIT(28)
#define VASPACE_FLAGS_ENABLE_FAULTING                             NVBIT(29)
#define VASPACE_FLAGS_IS_EXTERNALLY_OWNED                         NVBIT(30)
#define VASPACE_FLAGS_ALLOW_PAGES_IN_PHYS_MEM_SUBALLOCATOR        NVBIT(31)

/*!
 * Flags for page table memory pools.
 *
 * VASPACE_RESERVE_FLAGS_ALLOC_UPTO_TARGET_LEVEL_ONLY
 *           Only allocate levels from the top to the specified level only.
 *           Anything below the specified level is not allocated.
 */
#define VASPACE_RESERVE_FLAGS_NONE                                 (0)
#define VASPACE_RESERVE_FLAGS_ALLOC_UPTO_TARGET_LEVEL_ONLY  NVBIT32(0)

/*!
 * Level of RM-management for a given VA range.
 *
 * FULL
 *      RM manages everything (e.g. PDEs, PTEs).
 * PDES_ONLY
 *      RM only manages PDEs (through non-buffer version of UpdatePde2).
 *      Buffer versions of FillPteMem and UpdatePde2 are still allowed.
 * NONE
 *      RM does not manage anything.
 *      Buffer versions of FillPteMem and UpdatePde2 are still allowed.
 */
typedef enum
{
    VA_MANAGEMENT_FULL = 0,
    VA_MANAGEMENT_PDES_ONLY,
    VA_MANAGEMENT_NONE,
} VA_MANAGEMENT;

/*!
 * Abstract base class of an RM-managed virtual address space.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_VASPACE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__OBJVASPACE;
struct NVOC_METADATA__Object;
struct NVOC_VTABLE__OBJVASPACE;


struct OBJVASPACE {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__OBJVASPACE *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct OBJVASPACE *__nvoc_pbase_OBJVASPACE;    // vaspace

    // Data members
    NvU32 gpuMask;
    ADDRESS_TRANSLATION addressTranslation;
    NvU32 refCnt;
    NvU32 vaspaceId;
    NvU64 vasStart;
    NvU64 vasLimit;
    NvU32 vasUniqueId;
};


// Vtable with 29 per-class function pointers
struct NVOC_VTABLE__OBJVASPACE {
    NV_STATUS (*__vaspaceConstruct___)(struct OBJVASPACE * /*this*/, NvU32, NvU32, NvU64, NvU64, NvU64, NvU64, NvU32);  // pure virtual
    NV_STATUS (*__vaspaceAlloc__)(struct OBJVASPACE * /*this*/, NvU64, NvU64, NvU64, NvU64, NvU64, VAS_ALLOC_FLAGS, NvU64 *);  // pure virtual
    NV_STATUS (*__vaspaceFree__)(struct OBJVASPACE * /*this*/, NvU64);  // pure virtual
    NV_STATUS (*__vaspaceApplyDefaultAlignment__)(struct OBJVASPACE * /*this*/, const FB_ALLOC_INFO *, NvU64 *, NvU64 *, NvU64 *);  // pure virtual
    NV_STATUS (*__vaspaceIncAllocRefCnt__)(struct OBJVASPACE * /*this*/, NvU64);  // inline virtual body
    NvU64 (*__vaspaceGetVaStart__)(struct OBJVASPACE * /*this*/);  // virtual
    NvU64 (*__vaspaceGetVaLimit__)(struct OBJVASPACE * /*this*/);  // virtual
    NV_STATUS (*__vaspaceGetVasInfo__)(struct OBJVASPACE * /*this*/, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *);  // pure virtual
    NvU32 (*__vaspaceGetFlags__)(struct OBJVASPACE * /*this*/);  // inline virtual body
    NV_STATUS (*__vaspaceMap__)(struct OBJVASPACE * /*this*/, struct OBJGPU *, const NvU64, const NvU64, const MMU_MAP_TARGET *, const VAS_MAP_FLAGS);  // inline virtual body
    void (*__vaspaceUnmap__)(struct OBJVASPACE * /*this*/, struct OBJGPU *, const NvU64, const NvU64);  // inline virtual body
    NV_STATUS (*__vaspaceReserveMempool__)(struct OBJVASPACE * /*this*/, struct OBJGPU *, struct Device *, NvU64, NvU64, NvU32);  // inline virtual body
    OBJEHEAP * (*__vaspaceGetHeap__)(struct OBJVASPACE * /*this*/);  // inline virtual body
    NvU64 (*__vaspaceGetMapPageSize__)(struct OBJVASPACE * /*this*/, struct OBJGPU *, EMEMBLOCK *);  // inline virtual body
    NvU64 (*__vaspaceGetBigPageSize__)(struct OBJVASPACE * /*this*/);  // inline virtual body
    NvBool (*__vaspaceIsMirrored__)(struct OBJVASPACE * /*this*/);  // inline virtual body
    NvBool (*__vaspaceIsFaultCapable__)(struct OBJVASPACE * /*this*/);  // inline virtual body
    NvBool (*__vaspaceIsExternallyOwned__)(struct OBJVASPACE * /*this*/);  // inline virtual body
    NvBool (*__vaspaceIsAtsEnabled__)(struct OBJVASPACE * /*this*/);  // inline virtual body
    NV_STATUS (*__vaspaceGetPasid__)(struct OBJVASPACE * /*this*/, NvU32 *);  // inline virtual body
    PMEMORY_DESCRIPTOR (*__vaspaceGetPageDirBase__)(struct OBJVASPACE * /*this*/, struct OBJGPU *);  // inline virtual body
    PMEMORY_DESCRIPTOR (*__vaspaceGetKernelPageDirBase__)(struct OBJVASPACE * /*this*/, struct OBJGPU *);  // inline virtual body
    NV_STATUS (*__vaspacePinRootPageDir__)(struct OBJVASPACE * /*this*/, struct OBJGPU *);  // inline virtual body
    void (*__vaspaceUnpinRootPageDir__)(struct OBJVASPACE * /*this*/, struct OBJGPU *);  // inline virtual body
    void (*__vaspaceInvalidateTlb__)(struct OBJVASPACE * /*this*/, struct OBJGPU *, VAS_PTE_UPDATE_TYPE);  // virtual
    NV_STATUS (*__vaspaceGetPageTableInfo__)(struct OBJVASPACE * /*this*/, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *);  // inline virtual body
    NV_STATUS (*__vaspaceGetPteInfo__)(struct OBJVASPACE * /*this*/, struct OBJGPU *, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *, RmPhysAddr *);  // inline virtual body
    NV_STATUS (*__vaspaceSetPteInfo__)(struct OBJVASPACE * /*this*/, struct OBJGPU *, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *);  // inline virtual body
    NV_STATUS (*__vaspaceFreeV2__)(struct OBJVASPACE * /*this*/, NvU64, NvU64 *);  // inline virtual body
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__OBJVASPACE {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
    const struct NVOC_VTABLE__OBJVASPACE vtable;
};

#ifndef __NVOC_CLASS_OBJVASPACE_TYPEDEF__
#define __NVOC_CLASS_OBJVASPACE_TYPEDEF__
typedef struct OBJVASPACE OBJVASPACE;
#endif /* __NVOC_CLASS_OBJVASPACE_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJVASPACE
#define __nvoc_class_id_OBJVASPACE 0x6c347f
#endif /* __nvoc_class_id_OBJVASPACE */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJVASPACE;

#define __staticCast_OBJVASPACE(pThis) \
    ((pThis)->__nvoc_pbase_OBJVASPACE)

#ifdef __nvoc_vaspace_h_disabled
#define __dynamicCast_OBJVASPACE(pThis) ((OBJVASPACE*) NULL)
#else //__nvoc_vaspace_h_disabled
#define __dynamicCast_OBJVASPACE(pThis) \
    ((OBJVASPACE*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJVASPACE)))
#endif //__nvoc_vaspace_h_disabled

NV_STATUS __nvoc_objCreateDynamic_OBJVASPACE(OBJVASPACE**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJVASPACE(OBJVASPACE**, Dynamic*, NvU32);
#define __objCreate_OBJVASPACE(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJVASPACE((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define vaspaceConstruct__FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceConstruct___
#define vaspaceConstruct_(pVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags) vaspaceConstruct__DISPATCH(pVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags)
#define vaspaceAlloc_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceAlloc__
#define vaspaceAlloc(pVAS, size, align, rangeLo, rangeHi, pageSizeLockMask, flags, pAddr) vaspaceAlloc_DISPATCH(pVAS, size, align, rangeLo, rangeHi, pageSizeLockMask, flags, pAddr)
#define vaspaceFree_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceFree__
#define vaspaceFree(pVAS, vAddr) vaspaceFree_DISPATCH(pVAS, vAddr)
#define vaspaceApplyDefaultAlignment_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceApplyDefaultAlignment__
#define vaspaceApplyDefaultAlignment(pVAS, pAllocInfo, pAlign, pSize, pPageSizeLockMask) vaspaceApplyDefaultAlignment_DISPATCH(pVAS, pAllocInfo, pAlign, pSize, pPageSizeLockMask)
#define vaspaceIncAllocRefCnt_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceIncAllocRefCnt__
#define vaspaceIncAllocRefCnt(pVAS, vAddr) vaspaceIncAllocRefCnt_DISPATCH(pVAS, vAddr)
#define vaspaceGetVaStart_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetVaStart__
#define vaspaceGetVaStart(pVAS) vaspaceGetVaStart_DISPATCH(pVAS)
#define vaspaceGetVaLimit_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetVaLimit__
#define vaspaceGetVaLimit(pVAS) vaspaceGetVaLimit_DISPATCH(pVAS)
#define vaspaceGetVasInfo_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetVasInfo__
#define vaspaceGetVasInfo(pVAS, pParams) vaspaceGetVasInfo_DISPATCH(pVAS, pParams)
#define vaspaceGetFlags_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetFlags__
#define vaspaceGetFlags(pVAS) vaspaceGetFlags_DISPATCH(pVAS)
#define vaspaceMap_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceMap__
#define vaspaceMap(pVAS, pGpu, vaLo, vaHi, pTarget, flags) vaspaceMap_DISPATCH(pVAS, pGpu, vaLo, vaHi, pTarget, flags)
#define vaspaceUnmap_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceUnmap__
#define vaspaceUnmap(pVAS, pGpu, vaLo, vaHi) vaspaceUnmap_DISPATCH(pVAS, pGpu, vaLo, vaHi)
#define vaspaceReserveMempool_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceReserveMempool__
#define vaspaceReserveMempool(pVAS, pGpu, pDevice, size, pageSizeLockMask, flags) vaspaceReserveMempool_DISPATCH(pVAS, pGpu, pDevice, size, pageSizeLockMask, flags)
#define vaspaceGetHeap_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetHeap__
#define vaspaceGetHeap(pVAS) vaspaceGetHeap_DISPATCH(pVAS)
#define vaspaceGetMapPageSize_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetMapPageSize__
#define vaspaceGetMapPageSize(pVAS, pGpu, pMemBlock) vaspaceGetMapPageSize_DISPATCH(pVAS, pGpu, pMemBlock)
#define vaspaceGetBigPageSize_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetBigPageSize__
#define vaspaceGetBigPageSize(pVAS) vaspaceGetBigPageSize_DISPATCH(pVAS)
#define vaspaceIsMirrored_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceIsMirrored__
#define vaspaceIsMirrored(pVAS) vaspaceIsMirrored_DISPATCH(pVAS)
#define vaspaceIsFaultCapable_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceIsFaultCapable__
#define vaspaceIsFaultCapable(pVAS) vaspaceIsFaultCapable_DISPATCH(pVAS)
#define vaspaceIsExternallyOwned_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceIsExternallyOwned__
#define vaspaceIsExternallyOwned(pVAS) vaspaceIsExternallyOwned_DISPATCH(pVAS)
#define vaspaceIsAtsEnabled_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceIsAtsEnabled__
#define vaspaceIsAtsEnabled(pVAS) vaspaceIsAtsEnabled_DISPATCH(pVAS)
#define vaspaceGetPasid_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetPasid__
#define vaspaceGetPasid(pVAS, pPasid) vaspaceGetPasid_DISPATCH(pVAS, pPasid)
#define vaspaceGetPageDirBase_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetPageDirBase__
#define vaspaceGetPageDirBase(pVAS, pGpu) vaspaceGetPageDirBase_DISPATCH(pVAS, pGpu)
#define vaspaceGetKernelPageDirBase_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetKernelPageDirBase__
#define vaspaceGetKernelPageDirBase(pVAS, pGpu) vaspaceGetKernelPageDirBase_DISPATCH(pVAS, pGpu)
#define vaspacePinRootPageDir_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspacePinRootPageDir__
#define vaspacePinRootPageDir(pVAS, pGpu) vaspacePinRootPageDir_DISPATCH(pVAS, pGpu)
#define vaspaceUnpinRootPageDir_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceUnpinRootPageDir__
#define vaspaceUnpinRootPageDir(pVAS, pGpu) vaspaceUnpinRootPageDir_DISPATCH(pVAS, pGpu)
#define vaspaceInvalidateTlb_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceInvalidateTlb__
#define vaspaceInvalidateTlb(pVAS, pGpu, type) vaspaceInvalidateTlb_DISPATCH(pVAS, pGpu, type)
#define vaspaceGetPageTableInfo_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetPageTableInfo__
#define vaspaceGetPageTableInfo(pVAS, pParams) vaspaceGetPageTableInfo_DISPATCH(pVAS, pParams)
#define vaspaceGetPteInfo_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetPteInfo__
#define vaspaceGetPteInfo(pVAS, pGpu, pParams, pPhysAddr) vaspaceGetPteInfo_DISPATCH(pVAS, pGpu, pParams, pPhysAddr)
#define vaspaceSetPteInfo_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceSetPteInfo__
#define vaspaceSetPteInfo(pVAS, pGpu, pParams) vaspaceSetPteInfo_DISPATCH(pVAS, pGpu, pParams)
#define vaspaceFreeV2_FNPTR(pVAS) pVAS->__nvoc_metadata_ptr->vtable.__vaspaceFreeV2__
#define vaspaceFreeV2(pVAS, vAddr, pSize) vaspaceFreeV2_DISPATCH(pVAS, vAddr, pSize)

// Dispatch functions
static inline NV_STATUS vaspaceConstruct__DISPATCH(struct OBJVASPACE *pVAS, NvU32 classId, NvU32 vaspaceId, NvU64 vaStart, NvU64 vaLimit, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 flags) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceConstruct___(pVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags);
}

static inline NV_STATUS vaspaceAlloc_DISPATCH(struct OBJVASPACE *pVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSizeLockMask, VAS_ALLOC_FLAGS flags, NvU64 *pAddr) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceAlloc__(pVAS, size, align, rangeLo, rangeHi, pageSizeLockMask, flags, pAddr);
}

static inline NV_STATUS vaspaceFree_DISPATCH(struct OBJVASPACE *pVAS, NvU64 vAddr) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceFree__(pVAS, vAddr);
}

static inline NV_STATUS vaspaceApplyDefaultAlignment_DISPATCH(struct OBJVASPACE *pVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pAlign, NvU64 *pSize, NvU64 *pPageSizeLockMask) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceApplyDefaultAlignment__(pVAS, pAllocInfo, pAlign, pSize, pPageSizeLockMask);
}

static inline NV_STATUS vaspaceIncAllocRefCnt_DISPATCH(struct OBJVASPACE *pVAS, NvU64 vAddr) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceIncAllocRefCnt__(pVAS, vAddr);
}

static inline NvU64 vaspaceGetVaStart_DISPATCH(struct OBJVASPACE *pVAS) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetVaStart__(pVAS);
}

static inline NvU64 vaspaceGetVaLimit_DISPATCH(struct OBJVASPACE *pVAS) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetVaLimit__(pVAS);
}

static inline NV_STATUS vaspaceGetVasInfo_DISPATCH(struct OBJVASPACE *pVAS, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetVasInfo__(pVAS, pParams);
}

static inline NvU32 vaspaceGetFlags_DISPATCH(struct OBJVASPACE *pVAS) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetFlags__(pVAS);
}

static inline NV_STATUS vaspaceMap_DISPATCH(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const MMU_MAP_TARGET *pTarget, const VAS_MAP_FLAGS flags) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceMap__(pVAS, pGpu, vaLo, vaHi, pTarget, flags);
}

static inline void vaspaceUnmap_DISPATCH(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi) {
    pVAS->__nvoc_metadata_ptr->vtable.__vaspaceUnmap__(pVAS, pGpu, vaLo, vaHi);
}

static inline NV_STATUS vaspaceReserveMempool_DISPATCH(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, struct Device *pDevice, NvU64 size, NvU64 pageSizeLockMask, NvU32 flags) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceReserveMempool__(pVAS, pGpu, pDevice, size, pageSizeLockMask, flags);
}

static inline OBJEHEAP * vaspaceGetHeap_DISPATCH(struct OBJVASPACE *pVAS) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetHeap__(pVAS);
}

static inline NvU64 vaspaceGetMapPageSize_DISPATCH(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, EMEMBLOCK *pMemBlock) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetMapPageSize__(pVAS, pGpu, pMemBlock);
}

static inline NvU64 vaspaceGetBigPageSize_DISPATCH(struct OBJVASPACE *pVAS) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetBigPageSize__(pVAS);
}

static inline NvBool vaspaceIsMirrored_DISPATCH(struct OBJVASPACE *pVAS) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceIsMirrored__(pVAS);
}

static inline NvBool vaspaceIsFaultCapable_DISPATCH(struct OBJVASPACE *pVAS) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceIsFaultCapable__(pVAS);
}

static inline NvBool vaspaceIsExternallyOwned_DISPATCH(struct OBJVASPACE *pVAS) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceIsExternallyOwned__(pVAS);
}

static inline NvBool vaspaceIsAtsEnabled_DISPATCH(struct OBJVASPACE *pVAS) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceIsAtsEnabled__(pVAS);
}

static inline NV_STATUS vaspaceGetPasid_DISPATCH(struct OBJVASPACE *pVAS, NvU32 *pPasid) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetPasid__(pVAS, pPasid);
}

static inline PMEMORY_DESCRIPTOR vaspaceGetPageDirBase_DISPATCH(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetPageDirBase__(pVAS, pGpu);
}

static inline PMEMORY_DESCRIPTOR vaspaceGetKernelPageDirBase_DISPATCH(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetKernelPageDirBase__(pVAS, pGpu);
}

static inline NV_STATUS vaspacePinRootPageDir_DISPATCH(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspacePinRootPageDir__(pVAS, pGpu);
}

static inline void vaspaceUnpinRootPageDir_DISPATCH(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    pVAS->__nvoc_metadata_ptr->vtable.__vaspaceUnpinRootPageDir__(pVAS, pGpu);
}

static inline void vaspaceInvalidateTlb_DISPATCH(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, VAS_PTE_UPDATE_TYPE type) {
    pVAS->__nvoc_metadata_ptr->vtable.__vaspaceInvalidateTlb__(pVAS, pGpu, type);
}

static inline NV_STATUS vaspaceGetPageTableInfo_DISPATCH(struct OBJVASPACE *pVAS, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetPageTableInfo__(pVAS, pParams);
}

static inline NV_STATUS vaspaceGetPteInfo_DISPATCH(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams, RmPhysAddr *pPhysAddr) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceGetPteInfo__(pVAS, pGpu, pParams, pPhysAddr);
}

static inline NV_STATUS vaspaceSetPteInfo_DISPATCH(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceSetPteInfo__(pVAS, pGpu, pParams);
}

static inline NV_STATUS vaspaceFreeV2_DISPATCH(struct OBJVASPACE *pVAS, NvU64 vAddr, NvU64 *pSize) {
    return pVAS->__nvoc_metadata_ptr->vtable.__vaspaceFreeV2__(pVAS, vAddr, pSize);
}

static inline NV_STATUS vaspaceIncAllocRefCnt_14ee5e(struct OBJVASPACE *pVAS, NvU64 vAddr) {
    NV_ASSERT_PRECOMP(NV_FALSE);
    return NV_ERR_NOT_SUPPORTED;
}

NvU64 vaspaceGetVaStart_IMPL(struct OBJVASPACE *pVAS);

NvU64 vaspaceGetVaLimit_IMPL(struct OBJVASPACE *pVAS);

static inline NvU32 vaspaceGetFlags_edd98b(struct OBJVASPACE *pVAS) {
    return 0U;
}

static inline NV_STATUS vaspaceMap_14ee5e(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const MMU_MAP_TARGET *pTarget, const VAS_MAP_FLAGS flags) {
    NV_ASSERT_PRECOMP(NV_FALSE);
    return NV_ERR_NOT_SUPPORTED;
}

static inline void vaspaceUnmap_af5be7(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi) {
    NV_ASSERT_PRECOMP(NV_FALSE);
}

static inline NV_STATUS vaspaceReserveMempool_ac1694(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, struct Device *pDevice, NvU64 size, NvU64 pageSizeLockMask, NvU32 flags) {
    return NV_OK;
}

static inline OBJEHEAP *vaspaceGetHeap_9451a7(struct OBJVASPACE *pVAS) {
    NV_ASSERT_PRECOMP(NV_FALSE);
    return ((void *)0);
}

static inline NvU64 vaspaceGetMapPageSize_c26fae(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, EMEMBLOCK *pMemBlock) {
    NV_ASSERT_PRECOMP(NV_FALSE);
    return 0U;
}

static inline NvU64 vaspaceGetBigPageSize_c26fae(struct OBJVASPACE *pVAS) {
    NV_ASSERT_PRECOMP(NV_FALSE);
    return 0U;
}

static inline NvBool vaspaceIsMirrored_2fa1ff(struct OBJVASPACE *pVAS) {
    NV_ASSERT_PRECOMP(NV_FALSE);
    return NV_FALSE;
}

static inline NvBool vaspaceIsFaultCapable_2fa1ff(struct OBJVASPACE *pVAS) {
    NV_ASSERT_PRECOMP(NV_FALSE);
    return NV_FALSE;
}

static inline NvBool vaspaceIsExternallyOwned_2fa1ff(struct OBJVASPACE *pVAS) {
    NV_ASSERT_PRECOMP(NV_FALSE);
    return NV_FALSE;
}

static inline NvBool vaspaceIsAtsEnabled_2fa1ff(struct OBJVASPACE *pVAS) {
    NV_ASSERT_PRECOMP(NV_FALSE);
    return NV_FALSE;
}

static inline NV_STATUS vaspaceGetPasid_14ee5e(struct OBJVASPACE *pVAS, NvU32 *pPasid) {
    NV_ASSERT_PRECOMP(NV_FALSE);
    return NV_ERR_NOT_SUPPORTED;
}

static inline PMEMORY_DESCRIPTOR vaspaceGetPageDirBase_9451a7(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    NV_ASSERT_PRECOMP(NV_FALSE);
    return ((void *)0);
}

static inline PMEMORY_DESCRIPTOR vaspaceGetKernelPageDirBase_9451a7(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    NV_ASSERT_PRECOMP(NV_FALSE);
    return ((void *)0);
}

static inline NV_STATUS vaspacePinRootPageDir_14ee5e(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    NV_ASSERT_PRECOMP(NV_FALSE);
    return NV_ERR_NOT_SUPPORTED;
}

static inline void vaspaceUnpinRootPageDir_af5be7(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    NV_ASSERT_PRECOMP(NV_FALSE);
}

void vaspaceInvalidateTlb_IMPL(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, VAS_PTE_UPDATE_TYPE type);

static inline NV_STATUS vaspaceGetPageTableInfo_14ee5e(struct OBJVASPACE *pVAS, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(NV_FALSE);
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS vaspaceGetPteInfo_14ee5e(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams, RmPhysAddr *pPhysAddr) {
    NV_ASSERT_PRECOMP(NV_FALSE);
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS vaspaceSetPteInfo_14ee5e(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(NV_FALSE);
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS vaspaceFreeV2_14ee5e(struct OBJVASPACE *pVAS, NvU64 vAddr, NvU64 *pSize) {
    NV_ASSERT_PRECOMP(NV_FALSE);
    return NV_ERR_NOT_SUPPORTED;
}

void vaspaceIncRefCnt_IMPL(struct OBJVASPACE *pVAS);

#ifdef __nvoc_vaspace_h_disabled
static inline void vaspaceIncRefCnt(struct OBJVASPACE *pVAS) {
    NV_ASSERT_FAILED_PRECOMP("OBJVASPACE was disabled!");
}
#else //__nvoc_vaspace_h_disabled
#define vaspaceIncRefCnt(pVAS) vaspaceIncRefCnt_IMPL(pVAS)
#endif //__nvoc_vaspace_h_disabled

void vaspaceDecRefCnt_IMPL(struct OBJVASPACE *pVAS);

#ifdef __nvoc_vaspace_h_disabled
static inline void vaspaceDecRefCnt(struct OBJVASPACE *pVAS) {
    NV_ASSERT_FAILED_PRECOMP("OBJVASPACE was disabled!");
}
#else //__nvoc_vaspace_h_disabled
#define vaspaceDecRefCnt(pVAS) vaspaceDecRefCnt_IMPL(pVAS)
#endif //__nvoc_vaspace_h_disabled

NV_STATUS vaspaceGetByHandleOrDeviceDefault_IMPL(struct RsClient *pClient, NvHandle hDeviceOrSubDevice, NvHandle hVASpace, struct OBJVASPACE **ppVAS);

#define vaspaceGetByHandleOrDeviceDefault(pClient, hDeviceOrSubDevice, hVASpace, ppVAS) vaspaceGetByHandleOrDeviceDefault_IMPL(pClient, hDeviceOrSubDevice, hVASpace, ppVAS)
NV_STATUS vaspaceFillAllocParams_IMPL(struct OBJVASPACE *pVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pSize, NvU64 *pAlign, NvU64 *pRangeLo, NvU64 *pRangeHi, NvU64 *pPageSizeLockMask, VAS_ALLOC_FLAGS *pFlags);

#ifdef __nvoc_vaspace_h_disabled
static inline NV_STATUS vaspaceFillAllocParams(struct OBJVASPACE *pVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pSize, NvU64 *pAlign, NvU64 *pRangeLo, NvU64 *pRangeHi, NvU64 *pPageSizeLockMask, VAS_ALLOC_FLAGS *pFlags) {
    NV_ASSERT_FAILED_PRECOMP("OBJVASPACE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_vaspace_h_disabled
#define vaspaceFillAllocParams(pVAS, pAllocInfo, pSize, pAlign, pRangeLo, pRangeHi, pPageSizeLockMask, pFlags) vaspaceFillAllocParams_IMPL(pVAS, pAllocInfo, pSize, pAlign, pRangeLo, pRangeHi, pPageSizeLockMask, pFlags)
#endif //__nvoc_vaspace_h_disabled

#undef PRIVATE_FIELD


// Ideally all non-static base class method declaration should be in the _private.h file
NvU64 vaspaceGetVaStart_IMPL(struct OBJVASPACE *pVAS);

// For getting the address translation after the MMU (i.e.: after VA->PA translation)
#define VAS_ADDRESS_TRANSLATION(pVASpace) ((pVASpace)->addressTranslation)

#endif // _VASPACE_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_VASPACE_NVOC_H_
