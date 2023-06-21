#ifndef _G_VASPACE_NVOC_H_
#define _G_VASPACE_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2013-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "containers/eheap_old.h"
#include "gpu/mem_mgr/heap_base.h"
#include "gpu/mem_mgr/mem_desc.h"


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
// DEFAULT_SIZE               Ignore varange parameters and use default
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
#define VASPACE_FLAGS_DEFAULT_SIZE                                NVBIT(12)
#define VASPACE_FLAGS_DEFAULT_PARAMS                              NVBIT(13)
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
#ifdef NVOC_VASPACE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct OBJVASPACE {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Object __nvoc_base_Object;
    struct Object *__nvoc_pbase_Object;
    struct OBJVASPACE *__nvoc_pbase_OBJVASPACE;
    NV_STATUS (*__vaspaceConstruct___)(struct OBJVASPACE *, NvU32, NvU32, NvU64, NvU64, NvU64, NvU64, NvU32);
    NV_STATUS (*__vaspaceAlloc__)(struct OBJVASPACE *, NvU64, NvU64, NvU64, NvU64, NvU64, VAS_ALLOC_FLAGS, NvU64 *);
    NV_STATUS (*__vaspaceFree__)(struct OBJVASPACE *, NvU64);
    NV_STATUS (*__vaspaceApplyDefaultAlignment__)(struct OBJVASPACE *, const FB_ALLOC_INFO *, NvU64 *, NvU64 *, NvU64 *);
    NV_STATUS (*__vaspaceIncAllocRefCnt__)(struct OBJVASPACE *, NvU64);
    NvU64 (*__vaspaceGetVaStart__)(struct OBJVASPACE *);
    NvU64 (*__vaspaceGetVaLimit__)(struct OBJVASPACE *);
    NV_STATUS (*__vaspaceGetVasInfo__)(struct OBJVASPACE *, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *);
    NvU32 (*__vaspaceGetFlags__)(struct OBJVASPACE *);
    NvBool (*__vaspaceIsInternalVaRestricted__)(struct OBJVASPACE *);
    NV_STATUS (*__vaspaceMap__)(struct OBJVASPACE *, struct OBJGPU *, const NvU64, const NvU64, const MMU_MAP_TARGET *, const VAS_MAP_FLAGS);
    void (*__vaspaceUnmap__)(struct OBJVASPACE *, struct OBJGPU *, const NvU64, const NvU64);
    NV_STATUS (*__vaspaceReserveMempool__)(struct OBJVASPACE *, struct OBJGPU *, NvHandle, NvU64, NvU64, NvU32);
    struct OBJEHEAP *(*__vaspaceGetHeap__)(struct OBJVASPACE *);
    NvU64 (*__vaspaceGetMapPageSize__)(struct OBJVASPACE *, struct OBJGPU *, EMEMBLOCK *);
    NvU64 (*__vaspaceGetBigPageSize__)(struct OBJVASPACE *);
    NvBool (*__vaspaceIsMirrored__)(struct OBJVASPACE *);
    NvBool (*__vaspaceIsFaultCapable__)(struct OBJVASPACE *);
    NvBool (*__vaspaceIsExternallyOwned__)(struct OBJVASPACE *);
    NvBool (*__vaspaceIsAtsEnabled__)(struct OBJVASPACE *);
    NV_STATUS (*__vaspaceGetPasid__)(struct OBJVASPACE *, NvU32 *);
    PMEMORY_DESCRIPTOR (*__vaspaceGetPageDirBase__)(struct OBJVASPACE *, struct OBJGPU *);
    PMEMORY_DESCRIPTOR (*__vaspaceGetKernelPageDirBase__)(struct OBJVASPACE *, struct OBJGPU *);
    NV_STATUS (*__vaspacePinRootPageDir__)(struct OBJVASPACE *, struct OBJGPU *);
    void (*__vaspaceUnpinRootPageDir__)(struct OBJVASPACE *, struct OBJGPU *);
    void (*__vaspaceInvalidateTlb__)(struct OBJVASPACE *, struct OBJGPU *, VAS_PTE_UPDATE_TYPE);
    NV_STATUS (*__vaspaceGetPageTableInfo__)(struct OBJVASPACE *, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *);
    NV_STATUS (*__vaspaceGetPteInfo__)(struct OBJVASPACE *, struct OBJGPU *, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *, RmPhysAddr *);
    NV_STATUS (*__vaspaceSetPteInfo__)(struct OBJVASPACE *, struct OBJGPU *, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *);
    NV_STATUS (*__vaspaceFreeV2__)(struct OBJVASPACE *, NvU64, NvU64 *);
    NvU32 gpuMask;
    ADDRESS_TRANSLATION addressTranslation;
    NvU32 refCnt;
    NvU32 vaspaceId;
    NvU64 vasStart;
    NvU64 vasLimit;
};

#ifndef __NVOC_CLASS_OBJVASPACE_TYPEDEF__
#define __NVOC_CLASS_OBJVASPACE_TYPEDEF__
typedef struct OBJVASPACE OBJVASPACE;
#endif /* __NVOC_CLASS_OBJVASPACE_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJVASPACE
#define __nvoc_class_id_OBJVASPACE 0x6c347f
#endif /* __nvoc_class_id_OBJVASPACE */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJVASPACE;

#define __staticCast_OBJVASPACE(pThis) \
    ((pThis)->__nvoc_pbase_OBJVASPACE)

#ifdef __nvoc_vaspace_h_disabled
#define __dynamicCast_OBJVASPACE(pThis) ((OBJVASPACE*)NULL)
#else //__nvoc_vaspace_h_disabled
#define __dynamicCast_OBJVASPACE(pThis) \
    ((OBJVASPACE*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJVASPACE)))
#endif //__nvoc_vaspace_h_disabled


NV_STATUS __nvoc_objCreateDynamic_OBJVASPACE(OBJVASPACE**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJVASPACE(OBJVASPACE**, Dynamic*, NvU32);
#define __objCreate_OBJVASPACE(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJVASPACE((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define vaspaceConstruct_(pVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags) vaspaceConstruct__DISPATCH(pVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags)
#define vaspaceAlloc(pVAS, size, align, rangeLo, rangeHi, pageSizeLockMask, flags, pAddr) vaspaceAlloc_DISPATCH(pVAS, size, align, rangeLo, rangeHi, pageSizeLockMask, flags, pAddr)
#define vaspaceFree(pVAS, vAddr) vaspaceFree_DISPATCH(pVAS, vAddr)
#define vaspaceApplyDefaultAlignment(pVAS, pAllocInfo, pAlign, pSize, pPageSizeLockMask) vaspaceApplyDefaultAlignment_DISPATCH(pVAS, pAllocInfo, pAlign, pSize, pPageSizeLockMask)
#define vaspaceIncAllocRefCnt(pVAS, vAddr) vaspaceIncAllocRefCnt_DISPATCH(pVAS, vAddr)
#define vaspaceGetVaStart(pVAS) vaspaceGetVaStart_DISPATCH(pVAS)
#define vaspaceGetVaLimit(pVAS) vaspaceGetVaLimit_DISPATCH(pVAS)
#define vaspaceGetVasInfo(pVAS, pParams) vaspaceGetVasInfo_DISPATCH(pVAS, pParams)
#define vaspaceGetFlags(pVAS) vaspaceGetFlags_DISPATCH(pVAS)
#define vaspaceIsInternalVaRestricted(pVAS) vaspaceIsInternalVaRestricted_DISPATCH(pVAS)
#define vaspaceMap(pVAS, pGpu, vaLo, vaHi, pTarget, flags) vaspaceMap_DISPATCH(pVAS, pGpu, vaLo, vaHi, pTarget, flags)
#define vaspaceUnmap(pVAS, pGpu, vaLo, vaHi) vaspaceUnmap_DISPATCH(pVAS, pGpu, vaLo, vaHi)
#define vaspaceReserveMempool(pVAS, pGpu, hClient, size, pageSizeLockMask, flags) vaspaceReserveMempool_DISPATCH(pVAS, pGpu, hClient, size, pageSizeLockMask, flags)
#define vaspaceGetHeap(pVAS) vaspaceGetHeap_DISPATCH(pVAS)
#define vaspaceGetMapPageSize(pVAS, pGpu, pMemBlock) vaspaceGetMapPageSize_DISPATCH(pVAS, pGpu, pMemBlock)
#define vaspaceGetBigPageSize(pVAS) vaspaceGetBigPageSize_DISPATCH(pVAS)
#define vaspaceIsMirrored(pVAS) vaspaceIsMirrored_DISPATCH(pVAS)
#define vaspaceIsFaultCapable(pVAS) vaspaceIsFaultCapable_DISPATCH(pVAS)
#define vaspaceIsExternallyOwned(pVAS) vaspaceIsExternallyOwned_DISPATCH(pVAS)
#define vaspaceIsAtsEnabled(pVAS) vaspaceIsAtsEnabled_DISPATCH(pVAS)
#define vaspaceGetPasid(pVAS, pPasid) vaspaceGetPasid_DISPATCH(pVAS, pPasid)
#define vaspaceGetPageDirBase(pVAS, pGpu) vaspaceGetPageDirBase_DISPATCH(pVAS, pGpu)
#define vaspaceGetKernelPageDirBase(pVAS, pGpu) vaspaceGetKernelPageDirBase_DISPATCH(pVAS, pGpu)
#define vaspacePinRootPageDir(pVAS, pGpu) vaspacePinRootPageDir_DISPATCH(pVAS, pGpu)
#define vaspaceUnpinRootPageDir(pVAS, pGpu) vaspaceUnpinRootPageDir_DISPATCH(pVAS, pGpu)
#define vaspaceInvalidateTlb(pVAS, pGpu, type) vaspaceInvalidateTlb_DISPATCH(pVAS, pGpu, type)
#define vaspaceGetPageTableInfo(pVAS, pParams) vaspaceGetPageTableInfo_DISPATCH(pVAS, pParams)
#define vaspaceGetPteInfo(pVAS, pGpu, pParams, pPhysAddr) vaspaceGetPteInfo_DISPATCH(pVAS, pGpu, pParams, pPhysAddr)
#define vaspaceSetPteInfo(pVAS, pGpu, pParams) vaspaceSetPteInfo_DISPATCH(pVAS, pGpu, pParams)
#define vaspaceFreeV2(pVAS, vAddr, pSize) vaspaceFreeV2_DISPATCH(pVAS, vAddr, pSize)
static inline NV_STATUS vaspaceConstruct__DISPATCH(struct OBJVASPACE *pVAS, NvU32 classId, NvU32 vaspaceId, NvU64 vaStart, NvU64 vaLimit, NvU64 vaStartInternal, NvU64 vaLimitInternal, NvU32 flags) {
    return pVAS->__vaspaceConstruct___(pVAS, classId, vaspaceId, vaStart, vaLimit, vaStartInternal, vaLimitInternal, flags);
}

static inline NV_STATUS vaspaceAlloc_DISPATCH(struct OBJVASPACE *pVAS, NvU64 size, NvU64 align, NvU64 rangeLo, NvU64 rangeHi, NvU64 pageSizeLockMask, VAS_ALLOC_FLAGS flags, NvU64 *pAddr) {
    return pVAS->__vaspaceAlloc__(pVAS, size, align, rangeLo, rangeHi, pageSizeLockMask, flags, pAddr);
}

static inline NV_STATUS vaspaceFree_DISPATCH(struct OBJVASPACE *pVAS, NvU64 vAddr) {
    return pVAS->__vaspaceFree__(pVAS, vAddr);
}

static inline NV_STATUS vaspaceApplyDefaultAlignment_DISPATCH(struct OBJVASPACE *pVAS, const FB_ALLOC_INFO *pAllocInfo, NvU64 *pAlign, NvU64 *pSize, NvU64 *pPageSizeLockMask) {
    return pVAS->__vaspaceApplyDefaultAlignment__(pVAS, pAllocInfo, pAlign, pSize, pPageSizeLockMask);
}

static inline NV_STATUS vaspaceIncAllocRefCnt_b7902c(struct OBJVASPACE *pVAS, NvU64 vAddr) {
    NV_ASSERT_PRECOMP(((NvBool)(0 != 0)));
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS vaspaceIncAllocRefCnt_DISPATCH(struct OBJVASPACE *pVAS, NvU64 vAddr) {
    return pVAS->__vaspaceIncAllocRefCnt__(pVAS, vAddr);
}

NvU64 vaspaceGetVaStart_IMPL(struct OBJVASPACE *pVAS);

static inline NvU64 vaspaceGetVaStart_DISPATCH(struct OBJVASPACE *pVAS) {
    return pVAS->__vaspaceGetVaStart__(pVAS);
}

NvU64 vaspaceGetVaLimit_IMPL(struct OBJVASPACE *pVAS);

static inline NvU64 vaspaceGetVaLimit_DISPATCH(struct OBJVASPACE *pVAS) {
    return pVAS->__vaspaceGetVaLimit__(pVAS);
}

static inline NV_STATUS vaspaceGetVasInfo_DISPATCH(struct OBJVASPACE *pVAS, NV0080_CTRL_DMA_ADV_SCHED_GET_VA_CAPS_PARAMS *pParams) {
    return pVAS->__vaspaceGetVasInfo__(pVAS, pParams);
}

static inline NvU32 vaspaceGetFlags_edd98b(struct OBJVASPACE *pVAS) {
    return 0U;
}

static inline NvU32 vaspaceGetFlags_DISPATCH(struct OBJVASPACE *pVAS) {
    return pVAS->__vaspaceGetFlags__(pVAS);
}

NvBool vaspaceIsInternalVaRestricted_IMPL(struct OBJVASPACE *pVAS);

static inline NvBool vaspaceIsInternalVaRestricted_DISPATCH(struct OBJVASPACE *pVAS) {
    return pVAS->__vaspaceIsInternalVaRestricted__(pVAS);
}

static inline NV_STATUS vaspaceMap_b7902c(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const MMU_MAP_TARGET *pTarget, const VAS_MAP_FLAGS flags) {
    NV_ASSERT_PRECOMP(((NvBool)(0 != 0)));
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS vaspaceMap_DISPATCH(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi, const MMU_MAP_TARGET *pTarget, const VAS_MAP_FLAGS flags) {
    return pVAS->__vaspaceMap__(pVAS, pGpu, vaLo, vaHi, pTarget, flags);
}

static inline void vaspaceUnmap_8b86a5(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi) {
    NV_ASSERT_PRECOMP(((NvBool)(0 != 0)));
}

static inline void vaspaceUnmap_DISPATCH(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, const NvU64 vaLo, const NvU64 vaHi) {
    pVAS->__vaspaceUnmap__(pVAS, pGpu, vaLo, vaHi);
}

static inline NV_STATUS vaspaceReserveMempool_ac1694(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, NvHandle hClient, NvU64 size, NvU64 pageSizeLockMask, NvU32 flags) {
    return NV_OK;
}

static inline NV_STATUS vaspaceReserveMempool_DISPATCH(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, NvHandle hClient, NvU64 size, NvU64 pageSizeLockMask, NvU32 flags) {
    return pVAS->__vaspaceReserveMempool__(pVAS, pGpu, hClient, size, pageSizeLockMask, flags);
}

static inline struct OBJEHEAP *vaspaceGetHeap_128d6d(struct OBJVASPACE *pVAS) {
    NV_ASSERT_PRECOMP(((NvBool)(0 != 0)));
    return ((void *)0);
}

static inline struct OBJEHEAP *vaspaceGetHeap_DISPATCH(struct OBJVASPACE *pVAS) {
    return pVAS->__vaspaceGetHeap__(pVAS);
}

static inline NvU64 vaspaceGetMapPageSize_07238a(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, EMEMBLOCK *pMemBlock) {
    NV_ASSERT_PRECOMP(((NvBool)(0 != 0)));
    return 0U;
}

static inline NvU64 vaspaceGetMapPageSize_DISPATCH(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, EMEMBLOCK *pMemBlock) {
    return pVAS->__vaspaceGetMapPageSize__(pVAS, pGpu, pMemBlock);
}

static inline NvU64 vaspaceGetBigPageSize_07238a(struct OBJVASPACE *pVAS) {
    NV_ASSERT_PRECOMP(((NvBool)(0 != 0)));
    return 0U;
}

static inline NvU64 vaspaceGetBigPageSize_DISPATCH(struct OBJVASPACE *pVAS) {
    return pVAS->__vaspaceGetBigPageSize__(pVAS);
}

static inline NvBool vaspaceIsMirrored_814c13(struct OBJVASPACE *pVAS) {
    NV_ASSERT_PRECOMP(((NvBool)(0 != 0)));
    return ((NvBool)(0 != 0));
}

static inline NvBool vaspaceIsMirrored_DISPATCH(struct OBJVASPACE *pVAS) {
    return pVAS->__vaspaceIsMirrored__(pVAS);
}

static inline NvBool vaspaceIsFaultCapable_814c13(struct OBJVASPACE *pVAS) {
    NV_ASSERT_PRECOMP(((NvBool)(0 != 0)));
    return ((NvBool)(0 != 0));
}

static inline NvBool vaspaceIsFaultCapable_DISPATCH(struct OBJVASPACE *pVAS) {
    return pVAS->__vaspaceIsFaultCapable__(pVAS);
}

static inline NvBool vaspaceIsExternallyOwned_814c13(struct OBJVASPACE *pVAS) {
    NV_ASSERT_PRECOMP(((NvBool)(0 != 0)));
    return ((NvBool)(0 != 0));
}

static inline NvBool vaspaceIsExternallyOwned_DISPATCH(struct OBJVASPACE *pVAS) {
    return pVAS->__vaspaceIsExternallyOwned__(pVAS);
}

static inline NvBool vaspaceIsAtsEnabled_814c13(struct OBJVASPACE *pVAS) {
    NV_ASSERT_PRECOMP(((NvBool)(0 != 0)));
    return ((NvBool)(0 != 0));
}

static inline NvBool vaspaceIsAtsEnabled_DISPATCH(struct OBJVASPACE *pVAS) {
    return pVAS->__vaspaceIsAtsEnabled__(pVAS);
}

static inline NV_STATUS vaspaceGetPasid_b7902c(struct OBJVASPACE *pVAS, NvU32 *pPasid) {
    NV_ASSERT_PRECOMP(((NvBool)(0 != 0)));
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS vaspaceGetPasid_DISPATCH(struct OBJVASPACE *pVAS, NvU32 *pPasid) {
    return pVAS->__vaspaceGetPasid__(pVAS, pPasid);
}

static inline PMEMORY_DESCRIPTOR vaspaceGetPageDirBase_128d6d(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    NV_ASSERT_PRECOMP(((NvBool)(0 != 0)));
    return ((void *)0);
}

static inline PMEMORY_DESCRIPTOR vaspaceGetPageDirBase_DISPATCH(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    return pVAS->__vaspaceGetPageDirBase__(pVAS, pGpu);
}

static inline PMEMORY_DESCRIPTOR vaspaceGetKernelPageDirBase_128d6d(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    NV_ASSERT_PRECOMP(((NvBool)(0 != 0)));
    return ((void *)0);
}

static inline PMEMORY_DESCRIPTOR vaspaceGetKernelPageDirBase_DISPATCH(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    return pVAS->__vaspaceGetKernelPageDirBase__(pVAS, pGpu);
}

static inline NV_STATUS vaspacePinRootPageDir_b7902c(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    NV_ASSERT_PRECOMP(((NvBool)(0 != 0)));
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS vaspacePinRootPageDir_DISPATCH(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    return pVAS->__vaspacePinRootPageDir__(pVAS, pGpu);
}

static inline void vaspaceUnpinRootPageDir_8b86a5(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    NV_ASSERT_PRECOMP(((NvBool)(0 != 0)));
}

static inline void vaspaceUnpinRootPageDir_DISPATCH(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu) {
    pVAS->__vaspaceUnpinRootPageDir__(pVAS, pGpu);
}

void vaspaceInvalidateTlb_IMPL(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, VAS_PTE_UPDATE_TYPE type);

static inline void vaspaceInvalidateTlb_DISPATCH(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, VAS_PTE_UPDATE_TYPE type) {
    pVAS->__vaspaceInvalidateTlb__(pVAS, pGpu, type);
}

static inline NV_STATUS vaspaceGetPageTableInfo_b7902c(struct OBJVASPACE *pVAS, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(((NvBool)(0 != 0)));
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS vaspaceGetPageTableInfo_DISPATCH(struct OBJVASPACE *pVAS, NV0080_CTRL_DMA_GET_PDE_INFO_PARAMS *pParams) {
    return pVAS->__vaspaceGetPageTableInfo__(pVAS, pParams);
}

static inline NV_STATUS vaspaceGetPteInfo_b7902c(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams, RmPhysAddr *pPhysAddr) {
    NV_ASSERT_PRECOMP(((NvBool)(0 != 0)));
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS vaspaceGetPteInfo_DISPATCH(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_GET_PTE_INFO_PARAMS *pParams, RmPhysAddr *pPhysAddr) {
    return pVAS->__vaspaceGetPteInfo__(pVAS, pGpu, pParams, pPhysAddr);
}

static inline NV_STATUS vaspaceSetPteInfo_b7902c(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams) {
    NV_ASSERT_PRECOMP(((NvBool)(0 != 0)));
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS vaspaceSetPteInfo_DISPATCH(struct OBJVASPACE *pVAS, struct OBJGPU *pGpu, NV0080_CTRL_DMA_SET_PTE_INFO_PARAMS *pParams) {
    return pVAS->__vaspaceSetPteInfo__(pVAS, pGpu, pParams);
}

static inline NV_STATUS vaspaceFreeV2_b7902c(struct OBJVASPACE *pVAS, NvU64 vAddr, NvU64 *pSize) {
    NV_ASSERT_PRECOMP(((NvBool)(0 != 0)));
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS vaspaceFreeV2_DISPATCH(struct OBJVASPACE *pVAS, NvU64 vAddr, NvU64 *pSize) {
    return pVAS->__vaspaceFreeV2__(pVAS, vAddr, pSize);
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
