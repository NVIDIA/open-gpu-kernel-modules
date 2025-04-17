
#ifndef _G_VIRT_MEM_ALLOCATOR_NVOC_H_
#define _G_VIRT_MEM_ALLOCATOR_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_virt_mem_allocator_nvoc.h"

#ifndef VIRT_MEM_ALLOCATOR_H
#define VIRT_MEM_ALLOCATOR_H

/**************** Resource Manager Defines and Structures ******************\
*                                                                          *
*       Defines and structures used for the VirtMemAllocator Object.       *
*                                                                          *
\***************************************************************************/

#include "kernel/core/core.h"
#include "kernel/core/info_block.h"
#include "kernel/gpu/eng_state.h"
#include "kernel/gpu/gpu.h"
#include "kernel/gpu/mem_mgr/virt_mem_allocator_common.h"
#include "kernel/mem_mgr/virtual_mem.h"

#include "ctrl/ctrl0080/ctrl0080dma.h" // NV0080_CTRL_DMA_*_PARAMS

typedef struct DMA_PAGE_ARRAY DMA_PAGE_ARRAY;


struct OBJVASPACE;

#ifndef __NVOC_CLASS_OBJVASPACE_TYPEDEF__
#define __NVOC_CLASS_OBJVASPACE_TYPEDEF__
typedef struct OBJVASPACE OBJVASPACE;
#endif /* __NVOC_CLASS_OBJVASPACE_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJVASPACE
#define __nvoc_class_id_OBJVASPACE 0x6c347f
#endif /* __nvoc_class_id_OBJVASPACE */



struct KernelChannel;

#ifndef __NVOC_CLASS_KernelChannel_TYPEDEF__
#define __NVOC_CLASS_KernelChannel_TYPEDEF__
typedef struct KernelChannel KernelChannel;
#endif /* __NVOC_CLASS_KernelChannel_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelChannel
#define __nvoc_class_id_KernelChannel 0x5d8d70
#endif /* __nvoc_class_id_KernelChannel */


typedef struct _def_client_dma_mapping_info CLI_DMA_MAPPING_INFO;
typedef struct _def_client_dma_alloc_map_info CLI_DMA_ALLOC_MAP_INFO;

//
// DMA mapping calls can invalidate synchronously which always leaves the TLB in a
// consistent state with the PTEs.  For performance reasons we sometimes defer
// the TLB invalidation when we have multiple mappings to perform before the
// mappings will be used.  Please use deferred invalidates with care.
//
enum
{
    DMA_TLB_INVALIDATE = 0,
    DMA_DEFER_TLB_INVALIDATE = 1
};

//
// aperture capabilities
//
#define DMA_GPU_GART_CAPS_SNOOP                 0x00000001
#define DMA_GPU_GART_CAPS_NOSNOOP               0x00000002

// The parameters for dmaAllocBar1P2PMapping
typedef struct _def_dma_bar1p2p_mapping_params
{
    struct OBJVASPACE *pVas;                       // Virtual address space for request
    struct OBJGPU *pPeerGpu;                       // The target GPU which owns the PeerMemDesc
    MEMORY_DESCRIPTOR *pPeerMemDesc;        // The memdesc of the target GPU vidmem
    MEMORY_DESCRIPTOR *pMemDescOut;         // The new memdesc of the mapped BAR1 region on the target GPU
    NvU32 flags;                            // The flags used for the peer mapping
    NvU32 flagsOut;                         // The new flags for for the new pPeerMemDesc
    NvU64 offset;                           // The offset requested by the client.
    NvU64 offsetOut;                        // The Adjusted offset by the new BAR1 surface mapping
    NvU64 length;                           // The requested length by the client
    CLI_DMA_MAPPING_INFO *pDmaMappingInfo;  // The Dma mapping info structure.
} DMA_BAR1P2P_MAPPING_PRARAMS;



// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_VIRT_MEM_ALLOCATOR_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__VirtMemAllocator;
struct NVOC_METADATA__OBJENGSTATE;
struct NVOC_VTABLE__VirtMemAllocator;


struct VirtMemAllocator {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__VirtMemAllocator *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct VirtMemAllocator *__nvoc_pbase_VirtMemAllocator;    // dma

    // Vtable with 2 per-object function pointers
    NV_STATUS (*__dmaAllocBar1P2PMapping__)(struct OBJGPU *, struct VirtMemAllocator * /*this*/, DMA_BAR1P2P_MAPPING_PRARAMS *);  // halified (2 hals) body
    void (*__dmaFreeBar1P2PMapping__)(struct VirtMemAllocator * /*this*/, CLI_DMA_MAPPING_INFO *);  // halified (2 hals) body

    // Data members
    NvU32 gpuGartCaps;
    NvU32 increaseRsvdPages;
    NvBool bDmaMemoryMapOverride;
    NvBool bDmaRestrictVaRange;
    NvBool bDmaShaderAccessSupported;
    NvBool bDmaIsSupportedSparseVirtual;
    NvBool bDmaMmuInvalidateDisable;
    NvBool bDmaEnforce32BitPointer;
    NvBool bDmaEnableFullCompTagLine;
    NvBool bDmaMultipleVaspaceSupported;
    struct ENG_INFO_LINK_NODE *infoList;
};


// Vtable with 14 per-class function pointers
struct NVOC_VTABLE__VirtMemAllocator {
    NV_STATUS (*__dmaConstructEngine__)(struct OBJGPU *, struct VirtMemAllocator * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__dmaStateInitLocked__)(struct OBJGPU *, struct VirtMemAllocator * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__dmaStatePostLoad__)(struct OBJGPU *, struct VirtMemAllocator * /*this*/, NvU32);  // virtual halified (singleton optimized) override (engstate) base (engstate) body
    void (*__dmaInitMissing__)(struct OBJGPU *, struct VirtMemAllocator * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__dmaStatePreInitLocked__)(struct OBJGPU *, struct VirtMemAllocator * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__dmaStatePreInitUnlocked__)(struct OBJGPU *, struct VirtMemAllocator * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__dmaStateInitUnlocked__)(struct OBJGPU *, struct VirtMemAllocator * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__dmaStatePreLoad__)(struct OBJGPU *, struct VirtMemAllocator * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__dmaStateLoad__)(struct OBJGPU *, struct VirtMemAllocator * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__dmaStatePreUnload__)(struct OBJGPU *, struct VirtMemAllocator * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__dmaStateUnload__)(struct OBJGPU *, struct VirtMemAllocator * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__dmaStatePostUnload__)(struct OBJGPU *, struct VirtMemAllocator * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    void (*__dmaStateDestroy__)(struct OBJGPU *, struct VirtMemAllocator * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__dmaIsPresent__)(struct OBJGPU *, struct VirtMemAllocator * /*this*/);  // virtual inherited (engstate) base (engstate)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__VirtMemAllocator {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
    const struct NVOC_VTABLE__VirtMemAllocator vtable;
};

#ifndef __NVOC_CLASS_VirtMemAllocator_TYPEDEF__
#define __NVOC_CLASS_VirtMemAllocator_TYPEDEF__
typedef struct VirtMemAllocator VirtMemAllocator;
#endif /* __NVOC_CLASS_VirtMemAllocator_TYPEDEF__ */

#ifndef __nvoc_class_id_VirtMemAllocator
#define __nvoc_class_id_VirtMemAllocator 0x899e48
#endif /* __nvoc_class_id_VirtMemAllocator */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_VirtMemAllocator;

#define __staticCast_VirtMemAllocator(pThis) \
    ((pThis)->__nvoc_pbase_VirtMemAllocator)

#ifdef __nvoc_virt_mem_allocator_h_disabled
#define __dynamicCast_VirtMemAllocator(pThis) ((VirtMemAllocator*) NULL)
#else //__nvoc_virt_mem_allocator_h_disabled
#define __dynamicCast_VirtMemAllocator(pThis) \
    ((VirtMemAllocator*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(VirtMemAllocator)))
#endif //__nvoc_virt_mem_allocator_h_disabled

// Property macros
#define PDB_PROP_DMA_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_DMA_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_VirtMemAllocator(VirtMemAllocator**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_VirtMemAllocator(VirtMemAllocator**, Dynamic*, NvU32);
#define __objCreate_VirtMemAllocator(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_VirtMemAllocator((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define dmaConstructEngine_FNPTR(pDma) pDma->__nvoc_metadata_ptr->vtable.__dmaConstructEngine__
#define dmaConstructEngine(pGpu, pDma, arg3) dmaConstructEngine_DISPATCH(pGpu, pDma, arg3)
#define dmaStateInitLocked_FNPTR(pDma) pDma->__nvoc_metadata_ptr->vtable.__dmaStateInitLocked__
#define dmaStateInitLocked(pGpu, pDma) dmaStateInitLocked_DISPATCH(pGpu, pDma)
#define dmaAllocBar1P2PMapping_FNPTR(pDma) pDma->__dmaAllocBar1P2PMapping__
#define dmaAllocBar1P2PMapping(pGpu, pDma, params) dmaAllocBar1P2PMapping_DISPATCH(pGpu, pDma, params)
#define dmaAllocBar1P2PMapping_HAL(pGpu, pDma, params) dmaAllocBar1P2PMapping_DISPATCH(pGpu, pDma, params)
#define dmaFreeBar1P2PMapping_FNPTR(pDma) pDma->__dmaFreeBar1P2PMapping__
#define dmaFreeBar1P2PMapping(pDma, arg2) dmaFreeBar1P2PMapping_DISPATCH(pDma, arg2)
#define dmaFreeBar1P2PMapping_HAL(pDma, arg2) dmaFreeBar1P2PMapping_DISPATCH(pDma, arg2)
#define dmaStatePostLoad_FNPTR(pDma) pDma->__nvoc_metadata_ptr->vtable.__dmaStatePostLoad__
#define dmaStatePostLoad(pGpu, pDma, arg3) dmaStatePostLoad_DISPATCH(pGpu, pDma, arg3)
#define dmaStatePostLoad_HAL(pGpu, pDma, arg3) dmaStatePostLoad_DISPATCH(pGpu, pDma, arg3)
#define dmaInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define dmaInitMissing(pGpu, pEngstate) dmaInitMissing_DISPATCH(pGpu, pEngstate)
#define dmaStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__
#define dmaStatePreInitLocked(pGpu, pEngstate) dmaStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define dmaStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define dmaStatePreInitUnlocked(pGpu, pEngstate) dmaStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define dmaStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define dmaStateInitUnlocked(pGpu, pEngstate) dmaStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define dmaStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define dmaStatePreLoad(pGpu, pEngstate, arg3) dmaStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define dmaStateLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateLoad__
#define dmaStateLoad(pGpu, pEngstate, arg3) dmaStateLoad_DISPATCH(pGpu, pEngstate, arg3)
#define dmaStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__
#define dmaStatePreUnload(pGpu, pEngstate, arg3) dmaStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define dmaStateUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateUnload__
#define dmaStateUnload(pGpu, pEngstate, arg3) dmaStateUnload_DISPATCH(pGpu, pEngstate, arg3)
#define dmaStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define dmaStatePostUnload(pGpu, pEngstate, arg3) dmaStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define dmaStateDestroy_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateDestroy__
#define dmaStateDestroy(pGpu, pEngstate) dmaStateDestroy_DISPATCH(pGpu, pEngstate)
#define dmaIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define dmaIsPresent(pGpu, pEngstate) dmaIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS dmaConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, ENGDESCRIPTOR arg3) {
    return pDma->__nvoc_metadata_ptr->vtable.__dmaConstructEngine__(pGpu, pDma, arg3);
}

static inline NV_STATUS dmaStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma) {
    return pDma->__nvoc_metadata_ptr->vtable.__dmaStateInitLocked__(pGpu, pDma);
}

static inline NV_STATUS dmaAllocBar1P2PMapping_DISPATCH(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, DMA_BAR1P2P_MAPPING_PRARAMS *params) {
    return pDma->__dmaAllocBar1P2PMapping__(pGpu, pDma, params);
}

static inline void dmaFreeBar1P2PMapping_DISPATCH(struct VirtMemAllocator *pDma, CLI_DMA_MAPPING_INFO *arg2) {
    pDma->__dmaFreeBar1P2PMapping__(pDma, arg2);
}

static inline NV_STATUS dmaStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, NvU32 arg3) {
    return pDma->__nvoc_metadata_ptr->vtable.__dmaStatePostLoad__(pGpu, pDma, arg3);
}

static inline void dmaInitMissing_DISPATCH(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__dmaInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS dmaStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__dmaStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS dmaStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__dmaStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS dmaStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__dmaStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS dmaStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__dmaStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS dmaStateLoad_DISPATCH(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__dmaStateLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS dmaStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__dmaStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS dmaStateUnload_DISPATCH(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__dmaStateUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS dmaStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__dmaStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline void dmaStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__dmaStateDestroy__(pGpu, pEngstate);
}

static inline NvBool dmaIsPresent_DISPATCH(struct OBJGPU *pGpu, struct VirtMemAllocator *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__dmaIsPresent__(pGpu, pEngstate);
}

NV_STATUS dmaInit_GM107(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma);


#ifdef __nvoc_virt_mem_allocator_h_disabled
static inline NV_STATUS dmaInit(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma) {
    NV_ASSERT_FAILED_PRECOMP("VirtMemAllocator was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_virt_mem_allocator_h_disabled
#define dmaInit(pGpu, pDma) dmaInit_GM107(pGpu, pDma)
#endif //__nvoc_virt_mem_allocator_h_disabled

#define dmaInit_HAL(pGpu, pDma) dmaInit(pGpu, pDma)

NV_STATUS dmaConstructHal_VF(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma);

static inline NV_STATUS dmaConstructHal_56cd7a(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma) {
    return NV_OK;
}


#ifdef __nvoc_virt_mem_allocator_h_disabled
static inline NV_STATUS dmaConstructHal(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma) {
    NV_ASSERT_FAILED_PRECOMP("VirtMemAllocator was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_virt_mem_allocator_h_disabled
#define dmaConstructHal(pGpu, pDma) dmaConstructHal_VF(pGpu, pDma)
#endif //__nvoc_virt_mem_allocator_h_disabled

#define dmaConstructHal_HAL(pGpu, pDma) dmaConstructHal(pGpu, pDma)

void dmaDestruct_GM107(struct VirtMemAllocator *pDma);


#define __nvoc_dmaDestruct(pDma) dmaDestruct_GM107(pDma)
NV_STATUS dmaInitGart_GM107(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma);


#ifdef __nvoc_virt_mem_allocator_h_disabled
static inline NV_STATUS dmaInitGart(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma) {
    NV_ASSERT_FAILED_PRECOMP("VirtMemAllocator was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_virt_mem_allocator_h_disabled
#define dmaInitGart(pGpu, pDma) dmaInitGart_GM107(pGpu, pDma)
#endif //__nvoc_virt_mem_allocator_h_disabled

#define dmaInitGart_HAL(pGpu, pDma) dmaInitGart(pGpu, pDma)

NV_STATUS dmaAllocMapping_GM107(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, struct OBJVASPACE *arg3, MEMORY_DESCRIPTOR *arg4, NvU64 *arg5, NvU32 arg6, CLI_DMA_ALLOC_MAP_INFO *arg7, NvU32 arg8);


#ifdef __nvoc_virt_mem_allocator_h_disabled
static inline NV_STATUS dmaAllocMapping(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, struct OBJVASPACE *arg3, MEMORY_DESCRIPTOR *arg4, NvU64 *arg5, NvU32 arg6, CLI_DMA_ALLOC_MAP_INFO *arg7, NvU32 arg8) {
    NV_ASSERT_FAILED_PRECOMP("VirtMemAllocator was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_virt_mem_allocator_h_disabled
#define dmaAllocMapping(pGpu, pDma, arg3, arg4, arg5, arg6, arg7, arg8) dmaAllocMapping_GM107(pGpu, pDma, arg3, arg4, arg5, arg6, arg7, arg8)
#endif //__nvoc_virt_mem_allocator_h_disabled

#define dmaAllocMapping_HAL(pGpu, pDma, arg3, arg4, arg5, arg6, arg7, arg8) dmaAllocMapping(pGpu, pDma, arg3, arg4, arg5, arg6, arg7, arg8)

NV_STATUS dmaFreeMapping_GM107(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, struct OBJVASPACE *arg3, NvU64 arg4, MEMORY_DESCRIPTOR *arg5, NvU32 arg6, CLI_DMA_ALLOC_MAP_INFO *arg7);


#ifdef __nvoc_virt_mem_allocator_h_disabled
static inline NV_STATUS dmaFreeMapping(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, struct OBJVASPACE *arg3, NvU64 arg4, MEMORY_DESCRIPTOR *arg5, NvU32 arg6, CLI_DMA_ALLOC_MAP_INFO *arg7) {
    NV_ASSERT_FAILED_PRECOMP("VirtMemAllocator was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_virt_mem_allocator_h_disabled
#define dmaFreeMapping(pGpu, pDma, arg3, arg4, arg5, arg6, arg7) dmaFreeMapping_GM107(pGpu, pDma, arg3, arg4, arg5, arg6, arg7)
#endif //__nvoc_virt_mem_allocator_h_disabled

#define dmaFreeMapping_HAL(pGpu, pDma, arg3, arg4, arg5, arg6, arg7) dmaFreeMapping(pGpu, pDma, arg3, arg4, arg5, arg6, arg7)

NV_STATUS dmaUpdateVASpace_GF100(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, struct OBJVASPACE *pVAS, MEMORY_DESCRIPTOR *pMemDesc, NvU8 *tgtPteMem, NvU64 vAddr, NvU64 vAddrLimit, NvU32 flags, DMA_PAGE_ARRAY *pPageArray, NvU32 overmapPteMod, COMPR_INFO *pComprInfo, NvU64 surfaceOffset, NvU32 valid, NvU32 aperture, NvU32 peer, NvU64 fabricAddr, NvU32 deferInvalidate, NvBool bSparse, NvU64 pageSize);


#ifdef __nvoc_virt_mem_allocator_h_disabled
static inline NV_STATUS dmaUpdateVASpace(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, struct OBJVASPACE *pVAS, MEMORY_DESCRIPTOR *pMemDesc, NvU8 *tgtPteMem, NvU64 vAddr, NvU64 vAddrLimit, NvU32 flags, DMA_PAGE_ARRAY *pPageArray, NvU32 overmapPteMod, COMPR_INFO *pComprInfo, NvU64 surfaceOffset, NvU32 valid, NvU32 aperture, NvU32 peer, NvU64 fabricAddr, NvU32 deferInvalidate, NvBool bSparse, NvU64 pageSize) {
    NV_ASSERT_FAILED_PRECOMP("VirtMemAllocator was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_virt_mem_allocator_h_disabled
#define dmaUpdateVASpace(pGpu, pDma, pVAS, pMemDesc, tgtPteMem, vAddr, vAddrLimit, flags, pPageArray, overmapPteMod, pComprInfo, surfaceOffset, valid, aperture, peer, fabricAddr, deferInvalidate, bSparse, pageSize) dmaUpdateVASpace_GF100(pGpu, pDma, pVAS, pMemDesc, tgtPteMem, vAddr, vAddrLimit, flags, pPageArray, overmapPteMod, pComprInfo, surfaceOffset, valid, aperture, peer, fabricAddr, deferInvalidate, bSparse, pageSize)
#endif //__nvoc_virt_mem_allocator_h_disabled

#define dmaUpdateVASpace_HAL(pGpu, pDma, pVAS, pMemDesc, tgtPteMem, vAddr, vAddrLimit, flags, pPageArray, overmapPteMod, pComprInfo, surfaceOffset, valid, aperture, peer, fabricAddr, deferInvalidate, bSparse, pageSize) dmaUpdateVASpace(pGpu, pDma, pVAS, pMemDesc, tgtPteMem, vAddr, vAddrLimit, flags, pPageArray, overmapPteMod, pComprInfo, surfaceOffset, valid, aperture, peer, fabricAddr, deferInvalidate, bSparse, pageSize)

NV_STATUS dmaXlateVAtoPAforChannel_GM107(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, struct KernelChannel *pKernelChannel, NvU64 vAddr, NvU64 *pAddr, NvU32 *memType);


#ifdef __nvoc_virt_mem_allocator_h_disabled
static inline NV_STATUS dmaXlateVAtoPAforChannel(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, struct KernelChannel *pKernelChannel, NvU64 vAddr, NvU64 *pAddr, NvU32 *memType) {
    NV_ASSERT_FAILED_PRECOMP("VirtMemAllocator was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_virt_mem_allocator_h_disabled
#define dmaXlateVAtoPAforChannel(pGpu, pDma, pKernelChannel, vAddr, pAddr, memType) dmaXlateVAtoPAforChannel_GM107(pGpu, pDma, pKernelChannel, vAddr, pAddr, memType)
#endif //__nvoc_virt_mem_allocator_h_disabled

#define dmaXlateVAtoPAforChannel_HAL(pGpu, pDma, pKernelChannel, vAddr, pAddr, memType) dmaXlateVAtoPAforChannel(pGpu, pDma, pKernelChannel, vAddr, pAddr, memType)

NvU32 dmaGetPTESize_GM107(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma);


#ifdef __nvoc_virt_mem_allocator_h_disabled
static inline NvU32 dmaGetPTESize(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma) {
    NV_ASSERT_FAILED_PRECOMP("VirtMemAllocator was disabled!");
    return 0;
}
#else //__nvoc_virt_mem_allocator_h_disabled
#define dmaGetPTESize(pGpu, pDma) dmaGetPTESize_GM107(pGpu, pDma)
#endif //__nvoc_virt_mem_allocator_h_disabled

#define dmaGetPTESize_HAL(pGpu, pDma) dmaGetPTESize(pGpu, pDma)

NV_STATUS dmaMapBuffer_GM107(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, struct OBJVASPACE *pVAS, PMEMORY_DESCRIPTOR pMemDesc, NvU64 *pVaddr, NvU32 allocFlags, NvU32 mapFlags);


#ifdef __nvoc_virt_mem_allocator_h_disabled
static inline NV_STATUS dmaMapBuffer(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, struct OBJVASPACE *pVAS, PMEMORY_DESCRIPTOR pMemDesc, NvU64 *pVaddr, NvU32 allocFlags, NvU32 mapFlags) {
    NV_ASSERT_FAILED_PRECOMP("VirtMemAllocator was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_virt_mem_allocator_h_disabled
#define dmaMapBuffer(pGpu, pDma, pVAS, pMemDesc, pVaddr, allocFlags, mapFlags) dmaMapBuffer_GM107(pGpu, pDma, pVAS, pMemDesc, pVaddr, allocFlags, mapFlags)
#endif //__nvoc_virt_mem_allocator_h_disabled

#define dmaMapBuffer_HAL(pGpu, pDma, pVAS, pMemDesc, pVaddr, allocFlags, mapFlags) dmaMapBuffer(pGpu, pDma, pVAS, pMemDesc, pVaddr, allocFlags, mapFlags)

void dmaUnmapBuffer_GM107(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, struct OBJVASPACE *pVAS, NvU64 vaddr);


#ifdef __nvoc_virt_mem_allocator_h_disabled
static inline void dmaUnmapBuffer(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, struct OBJVASPACE *pVAS, NvU64 vaddr) {
    NV_ASSERT_FAILED_PRECOMP("VirtMemAllocator was disabled!");
}
#else //__nvoc_virt_mem_allocator_h_disabled
#define dmaUnmapBuffer(pGpu, pDma, pVAS, vaddr) dmaUnmapBuffer_GM107(pGpu, pDma, pVAS, vaddr)
#endif //__nvoc_virt_mem_allocator_h_disabled

#define dmaUnmapBuffer_HAL(pGpu, pDma, pVAS, vaddr) dmaUnmapBuffer(pGpu, pDma, pVAS, vaddr)

static inline struct OBJVASPACE *dmaGetPrivateVAS_fa6e19(struct VirtMemAllocator *pDma) {
    return ((void *)0);
}


#ifdef __nvoc_virt_mem_allocator_h_disabled
static inline struct OBJVASPACE *dmaGetPrivateVAS(struct VirtMemAllocator *pDma) {
    NV_ASSERT_FAILED_PRECOMP("VirtMemAllocator was disabled!");
    return NULL;
}
#else //__nvoc_virt_mem_allocator_h_disabled
#define dmaGetPrivateVAS(pDma) dmaGetPrivateVAS_fa6e19(pDma)
#endif //__nvoc_virt_mem_allocator_h_disabled

#define dmaGetPrivateVAS_HAL(pDma) dmaGetPrivateVAS(pDma)

NV_STATUS dmaConstructEngine_IMPL(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, ENGDESCRIPTOR arg3);

NV_STATUS dmaStateInitLocked_IMPL(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma);

NV_STATUS dmaAllocBar1P2PMapping_GH100(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, DMA_BAR1P2P_MAPPING_PRARAMS *params);

static inline NV_STATUS dmaAllocBar1P2PMapping_46f6a7(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, DMA_BAR1P2P_MAPPING_PRARAMS *params) {
    return NV_ERR_NOT_SUPPORTED;
}

void dmaFreeBar1P2PMapping_GH100(struct VirtMemAllocator *pDma, CLI_DMA_MAPPING_INFO *arg2);

static inline void dmaFreeBar1P2PMapping_b3696a(struct VirtMemAllocator *pDma, CLI_DMA_MAPPING_INFO *arg2) {
    return;
}

NV_STATUS dmaStatePostLoad_GM107(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, NvU32 arg3);

NV_STATUS dmaAllocMap_IMPL(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, struct OBJVASPACE *arg3, struct VirtualMemory *arg4, struct Memory *arg5, CLI_DMA_MAPPING_INFO *arg6);

#ifdef __nvoc_virt_mem_allocator_h_disabled
static inline NV_STATUS dmaAllocMap(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, struct OBJVASPACE *arg3, struct VirtualMemory *arg4, struct Memory *arg5, CLI_DMA_MAPPING_INFO *arg6) {
    NV_ASSERT_FAILED_PRECOMP("VirtMemAllocator was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_virt_mem_allocator_h_disabled
#define dmaAllocMap(pGpu, pDma, arg3, arg4, arg5, arg6) dmaAllocMap_IMPL(pGpu, pDma, arg3, arg4, arg5, arg6)
#endif //__nvoc_virt_mem_allocator_h_disabled

NV_STATUS dmaFreeMap_IMPL(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, struct OBJVASPACE *arg3, struct VirtualMemory *arg4, CLI_DMA_MAPPING_INFO *arg5, NvU32 flags);

#ifdef __nvoc_virt_mem_allocator_h_disabled
static inline NV_STATUS dmaFreeMap(struct OBJGPU *pGpu, struct VirtMemAllocator *pDma, struct OBJVASPACE *arg3, struct VirtualMemory *arg4, CLI_DMA_MAPPING_INFO *arg5, NvU32 flags) {
    NV_ASSERT_FAILED_PRECOMP("VirtMemAllocator was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_virt_mem_allocator_h_disabled
#define dmaFreeMap(pGpu, pDma, arg3, arg4, arg5, flags) dmaFreeMap_IMPL(pGpu, pDma, arg3, arg4, arg5, flags)
#endif //__nvoc_virt_mem_allocator_h_disabled

#undef PRIVATE_FIELD


//
// Virtual Memory Manager
//

//
// The VA space is the root of an address space.
//
// Currently we support 2 page sizes
//
#define VAS_PAGESIZE_IDX_4K    0
#define VAS_PAGESIZE_IDX_BIG   1
#define VAS_PAGESIZE_IDX_HUGE  2
#define VAS_PAGESIZE_IDX_512M  3
#define VAS_PAGESIZE_IDX_256G  4

/*!
 * Abstracts an array of physical page addresses.
 */
struct DMA_PAGE_ARRAY
{
    void        *pData;       //!< Array of PTE addresses or opaque OS-specific data.
    NvU32        startIndex;  //!< Base index into the pData array.
    NvU32        count;       //!< Number of pages represented by this array.
    NvBool       bOsFormat;   //!< Indicates if pData is an opaque OS-specific data.
    NvBool       bDuplicate;  //!< Indicates to duplicate the address of the first page.
    NvBool       bLocalized;  //!< Indicates if its localized memory or not.
    OS_GPU_INFO *pOsGpuInfo;  //!< OS-specific GPU info needed for IOMMU on Windows.
};

// page array operations
void dmaPageArrayInit(DMA_PAGE_ARRAY *pPageArray, void *pPageData, NvU32 pageCount);

#define DMA_PAGE_ARRAY_FLAGS_NONE       0ULL

#define DMA_PAGE_ARRARY_FLAGS_LOCALIZED  NVBIT64(0)

#define DMA_PAGE_ARRAY_LOCALIZED_OFFSET  NVBIT64(39)

void dmaPageArrayInitWithFlags(DMA_PAGE_ARRAY *pPageArray, void *pPageData, NvU32 pageCount,
                               NvU64 flags);

void dmaPageArrayInitFromMemDesc(DMA_PAGE_ARRAY *pPageArray,
                                 MEMORY_DESCRIPTOR *pMemDesc,
                                 ADDRESS_TRANSLATION addressTranslation);
RmPhysAddr dmaPageArrayGetPhysAddr(DMA_PAGE_ARRAY *pPageArray, NvU32 pageIndex);

//
// hal.dmaUpdateVASpace() flags
//
#define DMA_UPDATE_VASPACE_FLAGS_NONE               0
#define DMA_UPDATE_VASPACE_FLAGS_UPDATE_PADDR       NVBIT(0)
#define DMA_UPDATE_VASPACE_FLAGS_UPDATE_COMPR       NVBIT(1)
#define DMA_UPDATE_VASPACE_FLAGS_UPDATE_ACCESS      NVBIT(2)
#define DMA_UPDATE_VASPACE_FLAGS_UPDATE_VALID       NVBIT(3)
#define DMA_UPDATE_VASPACE_FLAGS_UPDATE_PRIV        NVBIT(4)
#define DMA_UPDATE_VASPACE_FLAGS_UPDATE_KIND        NVBIT(5)
#define DMA_UPDATE_VASPACE_FLAGS_UPDATE_APERTURE    NVBIT(6)
#define DMA_UPDATE_VASPACE_FLAGS_UPDATE_PEER        NVBIT(7)
#define DMA_UPDATE_VASPACE_FLAGS_UPDATE_ENCRYPTED   NVBIT(8)
#define DMA_UPDATE_VASPACE_FLAGS_UPDATE_TLB_LOCK    NVBIT(9)
#define DMA_UPDATE_VASPACE_FLAGS_UPDATE_CACHE       NVBIT(10)         // VOLATILE of fermi
#define DMA_UPDATE_VASPACE_FLAGS_UPDATE_SHADER_ACCESS NVBIT(11)       // Kepler shader access
#define DMA_UPDATE_VASPACE_FLAGS_UPDATE_ALL         MASK_BITS(11)

#define DMA_UPDATE_VASPACE_FLAGS_SKIP_4K_PTE_CHECK  NVBIT(12)
#define DMA_UPDATE_VASPACE_FLAGS_VOLATILE           NVBIT(13)
#define DMA_UPDATE_VASPACE_FLAGS_NONVOLATILE        NVBIT(14)
#define DMA_UPDATE_VASPACE_FLAGS_INDIRECT_PEER      NVBIT(22)
#define DMA_UPDATE_VASPACE_FLAGS_ALLOW_REMAP        NVBIT(23)
#define DMA_UPDATE_VASPACE_FLAGS_UNALIGNED_COMP     NVBIT(24)
#define DMA_UPDATE_VASPACE_FLAGS_FILL_PTE_MEM       NVBIT(25)
#define DMA_UPDATE_VASPACE_FLAGS_DISABLE_ENCRYPTION NVBIT(26)
#define DMA_UPDATE_VASPACE_FLAGS_READ_ONLY          NVBIT(27)
#define DMA_UPDATE_VASPACE_FLAGS_PRIV               NVBIT(28)
#define DMA_UPDATE_VASPACE_FLAGS_TLB_LOCK           NVBIT(29)
#define DMA_UPDATE_VASPACE_FLAGS_SHADER_WRITE_ONLY  NVBIT(30)         // Kepler shader access
#define DMA_UPDATE_VASPACE_FLAGS_SHADER_READ_ONLY   NVBIT(31)         // Kepler shader access

//
// hal.dmaAllocVASpace() flags
//
#define DMA_ALLOC_VASPACE_NONE                      0
#define DMA_VA_LIMIT_49B                            NVBIT(0)
#define DMA_VA_LIMIT_57B                            NVBIT(1)
#define DMA_ALLOC_VASPACE_SIZE_ALIGNED              NVBIT(9)
//
// Bug 3610538 For unlinked SLI, clients want to restrict internal buffers to
// Internal VA range, so that SLI vaspaces can mirror each other.
//
#define DMA_ALLOC_VASPACE_USE_RM_INTERNAL_VALIMITS  NVBIT(10)

//
// Internal device allocation flags
//
#define NV_DEVICE_INTERNAL_ALLOCATION_FLAGS_NONE                         0
#define NV_DEVICE_INTERNAL_ALLOCATION_FLAGS_ENABLE_PRIVILEGED_VASPACE    NVBIT(0)

//
// UVM privileged region
//
#define UVM_KERNEL_PRIVILEGED_REGION_START      (0xFFF8000000ULL)
#define UVM_KERNEL_PRIVILEGED_REGION_LENGTH     (0x0008000000ULL)

#endif // VIRT_MEM_ALLOCATOR_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_VIRT_MEM_ALLOCATOR_NVOC_H_
