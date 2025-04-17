
#ifndef _G_KERNEL_GRAPHICS_MANAGER_NVOC_H_
#define _G_KERNEL_GRAPHICS_MANAGER_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2021-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kernel_graphics_manager_nvoc.h"

#ifndef KERNEL_GRAPHICS_MANAGER_H
#define KERNEL_GRAPHICS_MANAGER_H

#include "core/core.h"
#include "gpu/eng_state.h"
#include "gpu/gpu.h"
#include "kernel/mem_mgr/ctx_buf_pool.h"
#include "kernel/gpu/gr/kernel_graphics_context.h"

#define KGRMGR_MAX_GR                  8
#define KGRMGR_MAX_GPC                 12
#define GR_INDEX_INVALID               0xFFFFFFFF
#define KGRMGR_MAX_GPCGRP              4

typedef struct KERNEL_MIG_GPU_INSTANCE KERNEL_MIG_GPU_INSTANCE;
typedef struct GRMGR_LEGACY_KGRAPHICS_STATIC_INFO
{
    NV2080_CTRL_INTERNAL_STATIC_GR_FLOORSWEEPING_MASKS floorsweepingMasks;
    NV2080_CTRL_INTERNAL_STATIC_GR_PPC_MASKS *pPpcMasks;
    NV2080_CTRL_INTERNAL_STATIC_GR_INFO *pGrInfo;
    NvBool bInitialized;
} GRMGR_LEGACY_KGRAPHICS_STATIC_INFO;

typedef struct KGRMGR_FECS_GLOBAL_TRACE_INFO KGRMGR_FECS_GLOBAL_TRACE_INFO;
typedef struct KGRMGR_FECS_VGPU_GLOBAL_TRACE_INFO KGRMGR_FECS_VGPU_GLOBAL_TRACE_INFO;

/*!
 * KernelGraphicsManager provides means to access KernelGraphics engine with specified index.
 * It also houses information at a higher level or that is common between KernelGraphics engines.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_GRAPHICS_MANAGER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelGraphicsManager;
struct NVOC_METADATA__OBJENGSTATE;
struct NVOC_VTABLE__KernelGraphicsManager;


struct KernelGraphicsManager {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelGraphicsManager *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct KernelGraphicsManager *__nvoc_pbase_KernelGraphicsManager;    // kgrmgr

    // Vtable with 1 per-object function pointer
    NV_STATUS (*__kgrmgrGetVeidsFromGpcCount__)(OBJGPU *, struct KernelGraphicsManager * /*this*/, NvU32, NvU32 *);  // halified (4 hals)

    // Data members
    GRMGR_LEGACY_KGRAPHICS_STATIC_INFO PRIVATE_FIELD(legacyKgraphicsStaticInfo);
    NvU64 PRIVATE_FIELD(veidInUseMask);
    NvU64 PRIVATE_FIELD(grIdxVeidMask)[8];
    KGRMGR_FECS_GLOBAL_TRACE_INFO *PRIVATE_FIELD(pFecsGlobalTraceInfo);
    KGRMGR_FECS_VGPU_GLOBAL_TRACE_INFO *PRIVATE_FIELD(pFecsVgpuGlobalTraceInfo);
    CTX_BUF_INFO PRIVATE_FIELD(globalCtxBufInfo)[10];
};


struct KernelGraphicsManager_PRIVATE {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelGraphicsManager *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct KernelGraphicsManager *__nvoc_pbase_KernelGraphicsManager;    // kgrmgr

    // Vtable with 1 per-object function pointer
    NV_STATUS (*__kgrmgrGetVeidsFromGpcCount__)(OBJGPU *, struct KernelGraphicsManager * /*this*/, NvU32, NvU32 *);  // halified (4 hals)

    // Data members
    GRMGR_LEGACY_KGRAPHICS_STATIC_INFO legacyKgraphicsStaticInfo;
    NvU64 veidInUseMask;
    NvU64 grIdxVeidMask[8];
    KGRMGR_FECS_GLOBAL_TRACE_INFO *pFecsGlobalTraceInfo;
    KGRMGR_FECS_VGPU_GLOBAL_TRACE_INFO *pFecsVgpuGlobalTraceInfo;
    CTX_BUF_INFO globalCtxBufInfo[10];
};


// Vtable with 14 per-class function pointers
struct NVOC_VTABLE__KernelGraphicsManager {
    NV_STATUS (*__kgrmgrConstructEngine__)(OBJGPU *, struct KernelGraphicsManager * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    void (*__kgrmgrStateDestroy__)(OBJGPU *, struct KernelGraphicsManager * /*this*/);  // virtual override (engstate) base (engstate)
    void (*__kgrmgrInitMissing__)(struct OBJGPU *, struct KernelGraphicsManager * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgrmgrStatePreInitLocked__)(struct OBJGPU *, struct KernelGraphicsManager * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgrmgrStatePreInitUnlocked__)(struct OBJGPU *, struct KernelGraphicsManager * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgrmgrStateInitLocked__)(struct OBJGPU *, struct KernelGraphicsManager * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgrmgrStateInitUnlocked__)(struct OBJGPU *, struct KernelGraphicsManager * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgrmgrStatePreLoad__)(struct OBJGPU *, struct KernelGraphicsManager * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgrmgrStateLoad__)(struct OBJGPU *, struct KernelGraphicsManager * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgrmgrStatePostLoad__)(struct OBJGPU *, struct KernelGraphicsManager * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgrmgrStatePreUnload__)(struct OBJGPU *, struct KernelGraphicsManager * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgrmgrStateUnload__)(struct OBJGPU *, struct KernelGraphicsManager * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgrmgrStatePostUnload__)(struct OBJGPU *, struct KernelGraphicsManager * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NvBool (*__kgrmgrIsPresent__)(struct OBJGPU *, struct KernelGraphicsManager * /*this*/);  // virtual inherited (engstate) base (engstate)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelGraphicsManager {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
    const struct NVOC_VTABLE__KernelGraphicsManager vtable;
};

#ifndef __NVOC_CLASS_KernelGraphicsManager_TYPEDEF__
#define __NVOC_CLASS_KernelGraphicsManager_TYPEDEF__
typedef struct KernelGraphicsManager KernelGraphicsManager;
#endif /* __NVOC_CLASS_KernelGraphicsManager_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGraphicsManager
#define __nvoc_class_id_KernelGraphicsManager 0xd22179
#endif /* __nvoc_class_id_KernelGraphicsManager */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphicsManager;

#define __staticCast_KernelGraphicsManager(pThis) \
    ((pThis)->__nvoc_pbase_KernelGraphicsManager)

#ifdef __nvoc_kernel_graphics_manager_h_disabled
#define __dynamicCast_KernelGraphicsManager(pThis) ((KernelGraphicsManager*) NULL)
#else //__nvoc_kernel_graphics_manager_h_disabled
#define __dynamicCast_KernelGraphicsManager(pThis) \
    ((KernelGraphicsManager*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelGraphicsManager)))
#endif //__nvoc_kernel_graphics_manager_h_disabled

// Property macros
#define PDB_PROP_KGRMGR_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KGRMGR_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelGraphicsManager(KernelGraphicsManager**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelGraphicsManager(KernelGraphicsManager**, Dynamic*, NvU32);
#define __objCreate_KernelGraphicsManager(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelGraphicsManager((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define kgrmgrConstructEngine_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kgrmgrConstructEngine__
#define kgrmgrConstructEngine(arg1, arg_this, arg3) kgrmgrConstructEngine_DISPATCH(arg1, arg_this, arg3)
#define kgrmgrStateDestroy_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kgrmgrStateDestroy__
#define kgrmgrStateDestroy(arg1, arg_this) kgrmgrStateDestroy_DISPATCH(arg1, arg_this)
#define kgrmgrGetVeidsFromGpcCount_FNPTR(arg_this) arg_this->__kgrmgrGetVeidsFromGpcCount__
#define kgrmgrGetVeidsFromGpcCount(arg1, arg_this, gpcCount, pVeidCount) kgrmgrGetVeidsFromGpcCount_DISPATCH(arg1, arg_this, gpcCount, pVeidCount)
#define kgrmgrGetVeidsFromGpcCount_HAL(arg1, arg_this, gpcCount, pVeidCount) kgrmgrGetVeidsFromGpcCount_DISPATCH(arg1, arg_this, gpcCount, pVeidCount)
#define kgrmgrInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define kgrmgrInitMissing(pGpu, pEngstate) kgrmgrInitMissing_DISPATCH(pGpu, pEngstate)
#define kgrmgrStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__
#define kgrmgrStatePreInitLocked(pGpu, pEngstate) kgrmgrStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kgrmgrStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define kgrmgrStatePreInitUnlocked(pGpu, pEngstate) kgrmgrStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kgrmgrStateInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__
#define kgrmgrStateInitLocked(pGpu, pEngstate) kgrmgrStateInitLocked_DISPATCH(pGpu, pEngstate)
#define kgrmgrStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define kgrmgrStateInitUnlocked(pGpu, pEngstate) kgrmgrStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kgrmgrStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define kgrmgrStatePreLoad(pGpu, pEngstate, arg3) kgrmgrStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kgrmgrStateLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateLoad__
#define kgrmgrStateLoad(pGpu, pEngstate, arg3) kgrmgrStateLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kgrmgrStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__
#define kgrmgrStatePostLoad(pGpu, pEngstate, arg3) kgrmgrStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kgrmgrStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__
#define kgrmgrStatePreUnload(pGpu, pEngstate, arg3) kgrmgrStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kgrmgrStateUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateUnload__
#define kgrmgrStateUnload(pGpu, pEngstate, arg3) kgrmgrStateUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kgrmgrStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define kgrmgrStatePostUnload(pGpu, pEngstate, arg3) kgrmgrStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kgrmgrIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define kgrmgrIsPresent(pGpu, pEngstate) kgrmgrIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS kgrmgrConstructEngine_DISPATCH(OBJGPU *arg1, struct KernelGraphicsManager *arg_this, ENGDESCRIPTOR arg3) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kgrmgrConstructEngine__(arg1, arg_this, arg3);
}

static inline void kgrmgrStateDestroy_DISPATCH(OBJGPU *arg1, struct KernelGraphicsManager *arg_this) {
    arg_this->__nvoc_metadata_ptr->vtable.__kgrmgrStateDestroy__(arg1, arg_this);
}

static inline NV_STATUS kgrmgrGetVeidsFromGpcCount_DISPATCH(OBJGPU *arg1, struct KernelGraphicsManager *arg_this, NvU32 gpcCount, NvU32 *pVeidCount) {
    return arg_this->__kgrmgrGetVeidsFromGpcCount__(arg1, arg_this, gpcCount, pVeidCount);
}

static inline void kgrmgrInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__kgrmgrInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kgrmgrStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgrmgrStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgrmgrStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgrmgrStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgrmgrStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgrmgrStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgrmgrStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgrmgrStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgrmgrStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgrmgrStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kgrmgrStateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgrmgrStateLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kgrmgrStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgrmgrStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kgrmgrStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgrmgrStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kgrmgrStateUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgrmgrStateUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kgrmgrStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgrmgrStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline NvBool kgrmgrIsPresent_DISPATCH(struct OBJGPU *pGpu, struct KernelGraphicsManager *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgrmgrIsPresent__(pGpu, pEngstate);
}

NV_STATUS kgrmgrConstructEngine_IMPL(OBJGPU *arg1, struct KernelGraphicsManager *arg2, ENGDESCRIPTOR arg3);

void kgrmgrStateDestroy_IMPL(OBJGPU *arg1, struct KernelGraphicsManager *arg2);

NV_STATUS kgrmgrGetVeidsFromGpcCount_GA100(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 gpcCount, NvU32 *pVeidCount);

NV_STATUS kgrmgrGetVeidsFromGpcCount_GB100(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 gpcCount, NvU32 *pVeidCount);

NV_STATUS kgrmgrGetVeidsFromGpcCount_GB10B(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 gpcCount, NvU32 *pVeidCount);

static inline NV_STATUS kgrmgrGetVeidsFromGpcCount_46f6a7(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 gpcCount, NvU32 *pVeidCount) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NvU64 kgrmgrGetGrIdxVeidMask(OBJGPU *pGpu, struct KernelGraphicsManager *pKernelGraphicsManager, NvU32 grIdx) {
    struct KernelGraphicsManager_PRIVATE *pKernelGraphicsManager_PRIVATE = (struct KernelGraphicsManager_PRIVATE *)pKernelGraphicsManager;
    return pKernelGraphicsManager_PRIVATE->grIdxVeidMask[grIdx];
}

static inline const GRMGR_LEGACY_KGRAPHICS_STATIC_INFO *kgrmgrGetLegacyKGraphicsStaticInfo(OBJGPU *pGpu, struct KernelGraphicsManager *pKernelGraphicsManager) {
    struct KernelGraphicsManager_PRIVATE *pKernelGraphicsManager_PRIVATE = (struct KernelGraphicsManager_PRIVATE *)pKernelGraphicsManager;
    return &pKernelGraphicsManager_PRIVATE->legacyKgraphicsStaticInfo;
}

static inline void kgrmgrSetGrIdxVeidMask(OBJGPU *pGpu, struct KernelGraphicsManager *pKernelGraphicsManager, NvU32 grIdx, NvU64 mask) {
    struct KernelGraphicsManager_PRIVATE *pKernelGraphicsManager_PRIVATE = (struct KernelGraphicsManager_PRIVATE *)pKernelGraphicsManager;
    pKernelGraphicsManager_PRIVATE->grIdxVeidMask[grIdx] = mask;
}

static inline NvU64 kgrmgrGetVeidInUseMask(OBJGPU *pGpu, struct KernelGraphicsManager *pKernelGraphicsManager) {
    struct KernelGraphicsManager_PRIVATE *pKernelGraphicsManager_PRIVATE = (struct KernelGraphicsManager_PRIVATE *)pKernelGraphicsManager;
    return pKernelGraphicsManager_PRIVATE->veidInUseMask;
}

static inline void kgrmgrSetVeidInUseMask(OBJGPU *pGpu, struct KernelGraphicsManager *pKernelGraphicsManager, NvU64 mask) {
    struct KernelGraphicsManager_PRIVATE *pKernelGraphicsManager_PRIVATE = (struct KernelGraphicsManager_PRIVATE *)pKernelGraphicsManager;
    pKernelGraphicsManager_PRIVATE->veidInUseMask = mask;
}

static inline KGRMGR_FECS_GLOBAL_TRACE_INFO *kgrmgrGetFecsGlobalTraceInfo(OBJGPU *pGpu, struct KernelGraphicsManager *pKernelGraphicsManager) {
    struct KernelGraphicsManager_PRIVATE *pKernelGraphicsManager_PRIVATE = (struct KernelGraphicsManager_PRIVATE *)pKernelGraphicsManager;
    return pKernelGraphicsManager_PRIVATE->pFecsGlobalTraceInfo;
}

static inline KGRMGR_FECS_VGPU_GLOBAL_TRACE_INFO *kgrmgrGetFecsVgpuGlobalTraceInfo(OBJGPU *pGpu, struct KernelGraphicsManager *pKernelGraphicsManager) {
    struct KernelGraphicsManager_PRIVATE *pKernelGraphicsManager_PRIVATE = (struct KernelGraphicsManager_PRIVATE *)pKernelGraphicsManager;
    return pKernelGraphicsManager_PRIVATE->pFecsVgpuGlobalTraceInfo;
}

void kgrmgrGetGrObjectType_IMPL(NvU32 classNum, NvU32 *pObjectType);

#define kgrmgrGetGrObjectType(classNum, pObjectType) kgrmgrGetGrObjectType_IMPL(classNum, pObjectType)
NvBool kgrmgrIsCtxBufSupported_IMPL(GR_CTX_BUFFER arg1, NvBool bClassSupported2D);

#define kgrmgrIsCtxBufSupported(arg1, bClassSupported2D) kgrmgrIsCtxBufSupported_IMPL(arg1, bClassSupported2D)
NvBool kgrmgrIsGlobalCtxBufSupported_IMPL(GR_GLOBALCTX_BUFFER arg1, NvBool bClassSupported2D);

#define kgrmgrIsGlobalCtxBufSupported(arg1, bClassSupported2D) kgrmgrIsGlobalCtxBufSupported_IMPL(arg1, bClassSupported2D)
void kgrmgrCtrlSetEngineID_IMPL(NvU32 engID, NV2080_CTRL_GR_ROUTE_INFO *arg2);

#define kgrmgrCtrlSetEngineID(engID, arg2) kgrmgrCtrlSetEngineID_IMPL(engID, arg2)
void kgrmgrCtrlSetChannelHandle_IMPL(NvHandle hChannel, NV2080_CTRL_GR_ROUTE_INFO *arg2);

#define kgrmgrCtrlSetChannelHandle(hChannel, arg2) kgrmgrCtrlSetChannelHandle_IMPL(hChannel, arg2)
void kgrmgrDestruct_IMPL(struct KernelGraphicsManager *arg1);

#define __nvoc_kgrmgrDestruct(arg1) kgrmgrDestruct_IMPL(arg1)
void kgrmgrSetLegacyKgraphicsStaticInfo_IMPL(OBJGPU *arg1, struct KernelGraphicsManager *arg2, struct KernelGraphics *arg3);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline void kgrmgrSetLegacyKgraphicsStaticInfo(OBJGPU *arg1, struct KernelGraphicsManager *arg2, struct KernelGraphics *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrSetLegacyKgraphicsStaticInfo(arg1, arg2, arg3) kgrmgrSetLegacyKgraphicsStaticInfo_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrCtrlRouteKGRWithDevice_IMPL(OBJGPU *arg1, struct KernelGraphicsManager *arg2, Device *pDevice, const NV2080_CTRL_GR_ROUTE_INFO *pGrRouteInfo, struct KernelGraphics **ppKernelGraphics);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrCtrlRouteKGRWithDevice(OBJGPU *arg1, struct KernelGraphicsManager *arg2, Device *pDevice, const NV2080_CTRL_GR_ROUTE_INFO *pGrRouteInfo, struct KernelGraphics **ppKernelGraphics) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrCtrlRouteKGRWithDevice(arg1, arg2, pDevice, pGrRouteInfo, ppKernelGraphics) kgrmgrCtrlRouteKGRWithDevice_IMPL(arg1, arg2, pDevice, pGrRouteInfo, ppKernelGraphics)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NvU32 kgrmgrGetLegacyGpcMask_IMPL(OBJGPU *arg1, struct KernelGraphicsManager *arg2);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NvU32 kgrmgrGetLegacyGpcMask(OBJGPU *arg1, struct KernelGraphicsManager *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return 0;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetLegacyGpcMask(arg1, arg2) kgrmgrGetLegacyGpcMask_IMPL(arg1, arg2)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NvU32 kgrmgrGetLegacyPhysGfxGpcMask_IMPL(OBJGPU *arg1, struct KernelGraphicsManager *arg2);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NvU32 kgrmgrGetLegacyPhysGfxGpcMask(OBJGPU *arg1, struct KernelGraphicsManager *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return 0;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetLegacyPhysGfxGpcMask(arg1, arg2) kgrmgrGetLegacyPhysGfxGpcMask_IMPL(arg1, arg2)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NvU32 kgrmgrGetLegacyTpcMask_IMPL(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 gpcId);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NvU32 kgrmgrGetLegacyTpcMask(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 gpcId) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return 0;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetLegacyTpcMask(arg1, arg2, gpcId) kgrmgrGetLegacyTpcMask_IMPL(arg1, arg2, gpcId)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrGetLegacyPpcMask_IMPL(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 physGpcId, NvU32 *pPpcMask);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrGetLegacyPpcMask(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 physGpcId, NvU32 *pPpcMask) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetLegacyPpcMask(arg1, arg2, physGpcId, pPpcMask) kgrmgrGetLegacyPpcMask_IMPL(arg1, arg2, physGpcId, pPpcMask)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NvU32 kgrmgrGetLegacyZcullMask_IMPL(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 physGpcId);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NvU32 kgrmgrGetLegacyZcullMask(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 physGpcId) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return 0;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetLegacyZcullMask(arg1, arg2, physGpcId) kgrmgrGetLegacyZcullMask_IMPL(arg1, arg2, physGpcId)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrAllocVeidsForGrIdx_IMPL(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 grIdx, NvU32 veidSpanOffset, NvU32 veidCount, KERNEL_MIG_GPU_INSTANCE *arg6);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrAllocVeidsForGrIdx(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 grIdx, NvU32 veidSpanOffset, NvU32 veidCount, KERNEL_MIG_GPU_INSTANCE *arg6) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrAllocVeidsForGrIdx(arg1, arg2, grIdx, veidSpanOffset, veidCount, arg6) kgrmgrAllocVeidsForGrIdx_IMPL(arg1, arg2, grIdx, veidSpanOffset, veidCount, arg6)
#endif //__nvoc_kernel_graphics_manager_h_disabled

void kgrmgrClearVeidsForGrIdx_IMPL(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 grIdx);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline void kgrmgrClearVeidsForGrIdx(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 grIdx) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrClearVeidsForGrIdx(arg1, arg2, grIdx) kgrmgrClearVeidsForGrIdx_IMPL(arg1, arg2, grIdx)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrGetVeidStepSize_IMPL(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 *pVeidStepSize);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrGetVeidStepSize(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 *pVeidStepSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetVeidStepSize(arg1, arg2, pVeidStepSize) kgrmgrGetVeidStepSize_IMPL(arg1, arg2, pVeidStepSize)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrGetVeidSizePerSpan_IMPL(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 *pVeidSizePerSpan);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrGetVeidSizePerSpan(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 *pVeidSizePerSpan) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetVeidSizePerSpan(arg1, arg2, pVeidSizePerSpan) kgrmgrGetVeidSizePerSpan_IMPL(arg1, arg2, pVeidSizePerSpan)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrGetVeidBaseForGrIdx_IMPL(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 grIdx, NvU32 *pVeidStart);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrGetVeidBaseForGrIdx(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 grIdx, NvU32 *pVeidStart) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetVeidBaseForGrIdx(arg1, arg2, grIdx, pVeidStart) kgrmgrGetVeidBaseForGrIdx_IMPL(arg1, arg2, grIdx, pVeidStart)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrGetGrIdxForVeid_IMPL(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 veid, NvU32 *pGrIdx);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrGetGrIdxForVeid(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 veid, NvU32 *pGrIdx) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetGrIdxForVeid(arg1, arg2, veid, pGrIdx) kgrmgrGetGrIdxForVeid_IMPL(arg1, arg2, veid, pGrIdx)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrDiscoverMaxLocalCtxBufInfo_IMPL(OBJGPU *arg1, struct KernelGraphicsManager *arg2, struct KernelGraphics *arg3, NvU32 swizzId);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrDiscoverMaxLocalCtxBufInfo(OBJGPU *arg1, struct KernelGraphicsManager *arg2, struct KernelGraphics *arg3, NvU32 swizzId) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrDiscoverMaxLocalCtxBufInfo(arg1, arg2, arg3, swizzId) kgrmgrDiscoverMaxLocalCtxBufInfo_IMPL(arg1, arg2, arg3, swizzId)
#endif //__nvoc_kernel_graphics_manager_h_disabled

const CTX_BUF_INFO *kgrmgrGetGlobalCtxBufInfo_IMPL(OBJGPU *arg1, struct KernelGraphicsManager *arg2, GR_GLOBALCTX_BUFFER arg3);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline const CTX_BUF_INFO *kgrmgrGetGlobalCtxBufInfo(OBJGPU *arg1, struct KernelGraphicsManager *arg2, GR_GLOBALCTX_BUFFER arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NULL;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetGlobalCtxBufInfo(arg1, arg2, arg3) kgrmgrGetGlobalCtxBufInfo_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_graphics_manager_h_disabled

void kgrmgrSetGlobalCtxBufInfo_IMPL(OBJGPU *arg1, struct KernelGraphicsManager *arg2, GR_GLOBALCTX_BUFFER arg3, NvU64 size, NvU64 align, RM_ATTR_PAGE_SIZE attr, NvBool bContiguous);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline void kgrmgrSetGlobalCtxBufInfo(OBJGPU *arg1, struct KernelGraphicsManager *arg2, GR_GLOBALCTX_BUFFER arg3, NvU64 size, NvU64 align, RM_ATTR_PAGE_SIZE attr, NvBool bContiguous) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrSetGlobalCtxBufInfo(arg1, arg2, arg3, size, align, attr, bContiguous) kgrmgrSetGlobalCtxBufInfo_IMPL(arg1, arg2, arg3, size, align, attr, bContiguous)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrDiscoverMaxGlobalCtxBufSizes_IMPL(OBJGPU *arg1, struct KernelGraphicsManager *arg2, struct KernelGraphics *arg3, NvBool bMemoryPartitioningNeeded);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrDiscoverMaxGlobalCtxBufSizes(OBJGPU *arg1, struct KernelGraphicsManager *arg2, struct KernelGraphics *arg3, NvBool bMemoryPartitioningNeeded) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrDiscoverMaxGlobalCtxBufSizes(arg1, arg2, arg3, bMemoryPartitioningNeeded) kgrmgrDiscoverMaxGlobalCtxBufSizes_IMPL(arg1, arg2, arg3, bMemoryPartitioningNeeded)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NvU32 kgrmgrGetLegacyGpcTpcCount_IMPL(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 gpcId);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NvU32 kgrmgrGetLegacyGpcTpcCount(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU32 gpcId) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return 0;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetLegacyGpcTpcCount(arg1, arg2, gpcId) kgrmgrGetLegacyGpcTpcCount_IMPL(arg1, arg2, gpcId)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrCheckVeidsRequest_IMPL(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU64 *pInUseMask, NvU32 veidCount, NvU32 *pSpanStart, KERNEL_MIG_GPU_INSTANCE *arg6);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrCheckVeidsRequest(OBJGPU *arg1, struct KernelGraphicsManager *arg2, NvU64 *pInUseMask, NvU32 veidCount, NvU32 *pSpanStart, KERNEL_MIG_GPU_INSTANCE *arg6) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrCheckVeidsRequest(arg1, arg2, pInUseMask, veidCount, pSpanStart, arg6) kgrmgrCheckVeidsRequest_IMPL(arg1, arg2, pInUseMask, veidCount, pSpanStart, arg6)
#endif //__nvoc_kernel_graphics_manager_h_disabled

#undef PRIVATE_FIELD


#endif // KERNEL_GRAPHICS_MANAGER_H


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_GRAPHICS_MANAGER_NVOC_H_
