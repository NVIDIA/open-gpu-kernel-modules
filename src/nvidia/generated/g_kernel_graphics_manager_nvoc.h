#ifndef _G_KERNEL_GRAPHICS_MANAGER_NVOC_H_
#define _G_KERNEL_GRAPHICS_MANAGER_NVOC_H_
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


/*!
 * KernelGraphicsManager provides means to access KernelGraphics engine with specified index.
 * It also houses information at a higher level or that is common between KernelGraphics engines.
 */
#ifdef NVOC_KERNEL_GRAPHICS_MANAGER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelGraphicsManager {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct KernelGraphicsManager *__nvoc_pbase_KernelGraphicsManager;
    NV_STATUS (*__kgrmgrConstructEngine__)(OBJGPU *, struct KernelGraphicsManager *, ENGDESCRIPTOR);
    NV_STATUS (*__kgrmgrGetVeidsFromGpcCount__)(OBJGPU *, struct KernelGraphicsManager *, NvU32, NvU32 *);
    NV_STATUS (*__kgrmgrStateLoad__)(POBJGPU, struct KernelGraphicsManager *, NvU32);
    NV_STATUS (*__kgrmgrStateUnload__)(POBJGPU, struct KernelGraphicsManager *, NvU32);
    NV_STATUS (*__kgrmgrStateInitLocked__)(POBJGPU, struct KernelGraphicsManager *);
    NV_STATUS (*__kgrmgrStatePreLoad__)(POBJGPU, struct KernelGraphicsManager *, NvU32);
    NV_STATUS (*__kgrmgrStatePostUnload__)(POBJGPU, struct KernelGraphicsManager *, NvU32);
    void (*__kgrmgrStateDestroy__)(POBJGPU, struct KernelGraphicsManager *);
    NV_STATUS (*__kgrmgrStatePreUnload__)(POBJGPU, struct KernelGraphicsManager *, NvU32);
    NV_STATUS (*__kgrmgrStateInitUnlocked__)(POBJGPU, struct KernelGraphicsManager *);
    void (*__kgrmgrInitMissing__)(POBJGPU, struct KernelGraphicsManager *);
    NV_STATUS (*__kgrmgrStatePreInitLocked__)(POBJGPU, struct KernelGraphicsManager *);
    NV_STATUS (*__kgrmgrStatePreInitUnlocked__)(POBJGPU, struct KernelGraphicsManager *);
    NV_STATUS (*__kgrmgrStatePostLoad__)(POBJGPU, struct KernelGraphicsManager *, NvU32);
    NvBool (*__kgrmgrIsPresent__)(POBJGPU, struct KernelGraphicsManager *);
    GRMGR_LEGACY_KGRAPHICS_STATIC_INFO PRIVATE_FIELD(legacyKgraphicsStaticInfo);
    NvU32 PRIVATE_FIELD(fecsCallbackScheduled);
    NvU64 PRIVATE_FIELD(veidInUseMask);
    NvU64 PRIVATE_FIELD(grIdxVeidMask)[8];
    CTX_BUF_INFO PRIVATE_FIELD(globalCtxBufInfo)[10];
};

struct KernelGraphicsManager_PRIVATE {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct KernelGraphicsManager *__nvoc_pbase_KernelGraphicsManager;
    NV_STATUS (*__kgrmgrConstructEngine__)(OBJGPU *, struct KernelGraphicsManager *, ENGDESCRIPTOR);
    NV_STATUS (*__kgrmgrGetVeidsFromGpcCount__)(OBJGPU *, struct KernelGraphicsManager *, NvU32, NvU32 *);
    NV_STATUS (*__kgrmgrStateLoad__)(POBJGPU, struct KernelGraphicsManager *, NvU32);
    NV_STATUS (*__kgrmgrStateUnload__)(POBJGPU, struct KernelGraphicsManager *, NvU32);
    NV_STATUS (*__kgrmgrStateInitLocked__)(POBJGPU, struct KernelGraphicsManager *);
    NV_STATUS (*__kgrmgrStatePreLoad__)(POBJGPU, struct KernelGraphicsManager *, NvU32);
    NV_STATUS (*__kgrmgrStatePostUnload__)(POBJGPU, struct KernelGraphicsManager *, NvU32);
    void (*__kgrmgrStateDestroy__)(POBJGPU, struct KernelGraphicsManager *);
    NV_STATUS (*__kgrmgrStatePreUnload__)(POBJGPU, struct KernelGraphicsManager *, NvU32);
    NV_STATUS (*__kgrmgrStateInitUnlocked__)(POBJGPU, struct KernelGraphicsManager *);
    void (*__kgrmgrInitMissing__)(POBJGPU, struct KernelGraphicsManager *);
    NV_STATUS (*__kgrmgrStatePreInitLocked__)(POBJGPU, struct KernelGraphicsManager *);
    NV_STATUS (*__kgrmgrStatePreInitUnlocked__)(POBJGPU, struct KernelGraphicsManager *);
    NV_STATUS (*__kgrmgrStatePostLoad__)(POBJGPU, struct KernelGraphicsManager *, NvU32);
    NvBool (*__kgrmgrIsPresent__)(POBJGPU, struct KernelGraphicsManager *);
    GRMGR_LEGACY_KGRAPHICS_STATIC_INFO legacyKgraphicsStaticInfo;
    NvU32 fecsCallbackScheduled;
    NvU64 veidInUseMask;
    NvU64 grIdxVeidMask[8];
    CTX_BUF_INFO globalCtxBufInfo[10];
};

#ifndef __NVOC_CLASS_KernelGraphicsManager_TYPEDEF__
#define __NVOC_CLASS_KernelGraphicsManager_TYPEDEF__
typedef struct KernelGraphicsManager KernelGraphicsManager;
#endif /* __NVOC_CLASS_KernelGraphicsManager_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGraphicsManager
#define __nvoc_class_id_KernelGraphicsManager 0xd22179
#endif /* __nvoc_class_id_KernelGraphicsManager */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGraphicsManager;

#define __staticCast_KernelGraphicsManager(pThis) \
    ((pThis)->__nvoc_pbase_KernelGraphicsManager)

#ifdef __nvoc_kernel_graphics_manager_h_disabled
#define __dynamicCast_KernelGraphicsManager(pThis) ((KernelGraphicsManager*)NULL)
#else //__nvoc_kernel_graphics_manager_h_disabled
#define __dynamicCast_KernelGraphicsManager(pThis) \
    ((KernelGraphicsManager*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelGraphicsManager)))
#endif //__nvoc_kernel_graphics_manager_h_disabled

#define PDB_PROP_KGRMGR_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KGRMGR_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelGraphicsManager(KernelGraphicsManager**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelGraphicsManager(KernelGraphicsManager**, Dynamic*, NvU32);
#define __objCreate_KernelGraphicsManager(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelGraphicsManager((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define kgrmgrConstructEngine(arg0, arg1, arg2) kgrmgrConstructEngine_DISPATCH(arg0, arg1, arg2)
#define kgrmgrGetVeidsFromGpcCount(arg0, arg1, gpcCount, pVeidCount) kgrmgrGetVeidsFromGpcCount_DISPATCH(arg0, arg1, gpcCount, pVeidCount)
#define kgrmgrGetVeidsFromGpcCount_HAL(arg0, arg1, gpcCount, pVeidCount) kgrmgrGetVeidsFromGpcCount_DISPATCH(arg0, arg1, gpcCount, pVeidCount)
#define kgrmgrStateLoad(pGpu, pEngstate, arg0) kgrmgrStateLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kgrmgrStateUnload(pGpu, pEngstate, arg0) kgrmgrStateUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kgrmgrStateInitLocked(pGpu, pEngstate) kgrmgrStateInitLocked_DISPATCH(pGpu, pEngstate)
#define kgrmgrStatePreLoad(pGpu, pEngstate, arg0) kgrmgrStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kgrmgrStatePostUnload(pGpu, pEngstate, arg0) kgrmgrStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kgrmgrStateDestroy(pGpu, pEngstate) kgrmgrStateDestroy_DISPATCH(pGpu, pEngstate)
#define kgrmgrStatePreUnload(pGpu, pEngstate, arg0) kgrmgrStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kgrmgrStateInitUnlocked(pGpu, pEngstate) kgrmgrStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kgrmgrInitMissing(pGpu, pEngstate) kgrmgrInitMissing_DISPATCH(pGpu, pEngstate)
#define kgrmgrStatePreInitLocked(pGpu, pEngstate) kgrmgrStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kgrmgrStatePreInitUnlocked(pGpu, pEngstate) kgrmgrStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kgrmgrStatePostLoad(pGpu, pEngstate, arg0) kgrmgrStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kgrmgrIsPresent(pGpu, pEngstate) kgrmgrIsPresent_DISPATCH(pGpu, pEngstate)
NV_STATUS kgrmgrConstructEngine_IMPL(OBJGPU *arg0, struct KernelGraphicsManager *arg1, ENGDESCRIPTOR arg2);

static inline NV_STATUS kgrmgrConstructEngine_DISPATCH(OBJGPU *arg0, struct KernelGraphicsManager *arg1, ENGDESCRIPTOR arg2) {
    return arg1->__kgrmgrConstructEngine__(arg0, arg1, arg2);
}

NV_STATUS kgrmgrGetVeidsFromGpcCount_GA100(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 gpcCount, NvU32 *pVeidCount);

static inline NV_STATUS kgrmgrGetVeidsFromGpcCount_46f6a7(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 gpcCount, NvU32 *pVeidCount) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS kgrmgrGetVeidsFromGpcCount_DISPATCH(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 gpcCount, NvU32 *pVeidCount) {
    return arg1->__kgrmgrGetVeidsFromGpcCount__(arg0, arg1, gpcCount, pVeidCount);
}

static inline NV_STATUS kgrmgrStateLoad_DISPATCH(POBJGPU pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg0) {
    return pEngstate->__kgrmgrStateLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kgrmgrStateUnload_DISPATCH(POBJGPU pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg0) {
    return pEngstate->__kgrmgrStateUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kgrmgrStateInitLocked_DISPATCH(POBJGPU pGpu, struct KernelGraphicsManager *pEngstate) {
    return pEngstate->__kgrmgrStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgrmgrStatePreLoad_DISPATCH(POBJGPU pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg0) {
    return pEngstate->__kgrmgrStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kgrmgrStatePostUnload_DISPATCH(POBJGPU pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg0) {
    return pEngstate->__kgrmgrStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline void kgrmgrStateDestroy_DISPATCH(POBJGPU pGpu, struct KernelGraphicsManager *pEngstate) {
    pEngstate->__kgrmgrStateDestroy__(pGpu, pEngstate);
}

static inline NV_STATUS kgrmgrStatePreUnload_DISPATCH(POBJGPU pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg0) {
    return pEngstate->__kgrmgrStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kgrmgrStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelGraphicsManager *pEngstate) {
    return pEngstate->__kgrmgrStateInitUnlocked__(pGpu, pEngstate);
}

static inline void kgrmgrInitMissing_DISPATCH(POBJGPU pGpu, struct KernelGraphicsManager *pEngstate) {
    pEngstate->__kgrmgrInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kgrmgrStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct KernelGraphicsManager *pEngstate) {
    return pEngstate->__kgrmgrStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgrmgrStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelGraphicsManager *pEngstate) {
    return pEngstate->__kgrmgrStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgrmgrStatePostLoad_DISPATCH(POBJGPU pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg0) {
    return pEngstate->__kgrmgrStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NvBool kgrmgrIsPresent_DISPATCH(POBJGPU pGpu, struct KernelGraphicsManager *pEngstate) {
    return pEngstate->__kgrmgrIsPresent__(pGpu, pEngstate);
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

void kgrmgrGetGrObjectType_IMPL(NvU32 classNum, NvU32 *pObjectType);

#define kgrmgrGetGrObjectType(classNum, pObjectType) kgrmgrGetGrObjectType_IMPL(classNum, pObjectType)
NvBool kgrmgrIsCtxBufSupported_IMPL(GR_CTX_BUFFER arg0, NvBool bClassSupported2D);

#define kgrmgrIsCtxBufSupported(arg0, bClassSupported2D) kgrmgrIsCtxBufSupported_IMPL(arg0, bClassSupported2D)
NvBool kgrmgrIsGlobalCtxBufSupported_IMPL(GR_GLOBALCTX_BUFFER arg0, NvBool bClassSupported2D);

#define kgrmgrIsGlobalCtxBufSupported(arg0, bClassSupported2D) kgrmgrIsGlobalCtxBufSupported_IMPL(arg0, bClassSupported2D)
void kgrmgrCtrlSetEngineID_IMPL(NvU32 engID, NV2080_CTRL_GR_ROUTE_INFO *arg0);

#define kgrmgrCtrlSetEngineID(engID, arg0) kgrmgrCtrlSetEngineID_IMPL(engID, arg0)
void kgrmgrCtrlSetChannelHandle_IMPL(NvHandle hChannel, NV2080_CTRL_GR_ROUTE_INFO *arg0);

#define kgrmgrCtrlSetChannelHandle(hChannel, arg0) kgrmgrCtrlSetChannelHandle_IMPL(hChannel, arg0)
void kgrmgrDestruct_IMPL(struct KernelGraphicsManager *arg0);

#define __nvoc_kgrmgrDestruct(arg0) kgrmgrDestruct_IMPL(arg0)
void kgrmgrSetLegacyKgraphicsStaticInfo_IMPL(OBJGPU *arg0, struct KernelGraphicsManager *arg1, struct KernelGraphics *arg2);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline void kgrmgrSetLegacyKgraphicsStaticInfo(OBJGPU *arg0, struct KernelGraphicsManager *arg1, struct KernelGraphics *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrSetLegacyKgraphicsStaticInfo(arg0, arg1, arg2) kgrmgrSetLegacyKgraphicsStaticInfo_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrCtrlRouteKGRWithDevice_IMPL(OBJGPU *arg0, struct KernelGraphicsManager *arg1, Device *pDevice, const NV2080_CTRL_GR_ROUTE_INFO *pGrRouteInfo, struct KernelGraphics **ppKernelGraphics);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrCtrlRouteKGRWithDevice(OBJGPU *arg0, struct KernelGraphicsManager *arg1, Device *pDevice, const NV2080_CTRL_GR_ROUTE_INFO *pGrRouteInfo, struct KernelGraphics **ppKernelGraphics) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrCtrlRouteKGRWithDevice(arg0, arg1, pDevice, pGrRouteInfo, ppKernelGraphics) kgrmgrCtrlRouteKGRWithDevice_IMPL(arg0, arg1, pDevice, pGrRouteInfo, ppKernelGraphics)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NvU32 kgrmgrGetLegacyGpcMask_IMPL(OBJGPU *arg0, struct KernelGraphicsManager *arg1);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NvU32 kgrmgrGetLegacyGpcMask(OBJGPU *arg0, struct KernelGraphicsManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return 0;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetLegacyGpcMask(arg0, arg1) kgrmgrGetLegacyGpcMask_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NvU32 kgrmgrGetLegacyPhysGfxGpcMask_IMPL(OBJGPU *arg0, struct KernelGraphicsManager *arg1);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NvU32 kgrmgrGetLegacyPhysGfxGpcMask(OBJGPU *arg0, struct KernelGraphicsManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return 0;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetLegacyPhysGfxGpcMask(arg0, arg1) kgrmgrGetLegacyPhysGfxGpcMask_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NvU32 kgrmgrGetLegacyTpcMask_IMPL(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 gpcId);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NvU32 kgrmgrGetLegacyTpcMask(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 gpcId) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return 0;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetLegacyTpcMask(arg0, arg1, gpcId) kgrmgrGetLegacyTpcMask_IMPL(arg0, arg1, gpcId)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrGetLegacyPpcMask_IMPL(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 physGpcId, NvU32 *pPpcMask);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrGetLegacyPpcMask(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 physGpcId, NvU32 *pPpcMask) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetLegacyPpcMask(arg0, arg1, physGpcId, pPpcMask) kgrmgrGetLegacyPpcMask_IMPL(arg0, arg1, physGpcId, pPpcMask)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NvU32 kgrmgrGetLegacyZcullMask_IMPL(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 physGpcId);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NvU32 kgrmgrGetLegacyZcullMask(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 physGpcId) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return 0;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetLegacyZcullMask(arg0, arg1, physGpcId) kgrmgrGetLegacyZcullMask_IMPL(arg0, arg1, physGpcId)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrAllocVeidsForGrIdx_IMPL(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 grIdx, NvU32 veidSpanOffset, NvU32 veidCount, KERNEL_MIG_GPU_INSTANCE *arg2);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrAllocVeidsForGrIdx(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 grIdx, NvU32 veidSpanOffset, NvU32 veidCount, KERNEL_MIG_GPU_INSTANCE *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrAllocVeidsForGrIdx(arg0, arg1, grIdx, veidSpanOffset, veidCount, arg2) kgrmgrAllocVeidsForGrIdx_IMPL(arg0, arg1, grIdx, veidSpanOffset, veidCount, arg2)
#endif //__nvoc_kernel_graphics_manager_h_disabled

void kgrmgrClearVeidsForGrIdx_IMPL(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 grIdx);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline void kgrmgrClearVeidsForGrIdx(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 grIdx) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrClearVeidsForGrIdx(arg0, arg1, grIdx) kgrmgrClearVeidsForGrIdx_IMPL(arg0, arg1, grIdx)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrGetVeidStepSize_IMPL(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 *pVeidStepSize);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrGetVeidStepSize(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 *pVeidStepSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetVeidStepSize(arg0, arg1, pVeidStepSize) kgrmgrGetVeidStepSize_IMPL(arg0, arg1, pVeidStepSize)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrGetVeidBaseForGrIdx_IMPL(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 grIdx, NvU32 *pVeidStart);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrGetVeidBaseForGrIdx(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 grIdx, NvU32 *pVeidStart) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetVeidBaseForGrIdx(arg0, arg1, grIdx, pVeidStart) kgrmgrGetVeidBaseForGrIdx_IMPL(arg0, arg1, grIdx, pVeidStart)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrGetGrIdxForVeid_IMPL(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 veid, NvU32 *pGrIdx);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrGetGrIdxForVeid(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 veid, NvU32 *pGrIdx) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetGrIdxForVeid(arg0, arg1, veid, pGrIdx) kgrmgrGetGrIdxForVeid_IMPL(arg0, arg1, veid, pGrIdx)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrDiscoverMaxLocalCtxBufInfo_IMPL(OBJGPU *arg0, struct KernelGraphicsManager *arg1, struct KernelGraphics *arg2, NvU32 swizzId);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrDiscoverMaxLocalCtxBufInfo(OBJGPU *arg0, struct KernelGraphicsManager *arg1, struct KernelGraphics *arg2, NvU32 swizzId) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrDiscoverMaxLocalCtxBufInfo(arg0, arg1, arg2, swizzId) kgrmgrDiscoverMaxLocalCtxBufInfo_IMPL(arg0, arg1, arg2, swizzId)
#endif //__nvoc_kernel_graphics_manager_h_disabled

const CTX_BUF_INFO *kgrmgrGetGlobalCtxBufInfo_IMPL(OBJGPU *arg0, struct KernelGraphicsManager *arg1, GR_GLOBALCTX_BUFFER arg2);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline const CTX_BUF_INFO *kgrmgrGetGlobalCtxBufInfo(OBJGPU *arg0, struct KernelGraphicsManager *arg1, GR_GLOBALCTX_BUFFER arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NULL;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetGlobalCtxBufInfo(arg0, arg1, arg2) kgrmgrGetGlobalCtxBufInfo_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_graphics_manager_h_disabled

void kgrmgrSetGlobalCtxBufInfo_IMPL(OBJGPU *arg0, struct KernelGraphicsManager *arg1, GR_GLOBALCTX_BUFFER arg2, NvU64 size, NvU64 align, RM_ATTR_PAGE_SIZE attr, NvBool bContiguous);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline void kgrmgrSetGlobalCtxBufInfo(OBJGPU *arg0, struct KernelGraphicsManager *arg1, GR_GLOBALCTX_BUFFER arg2, NvU64 size, NvU64 align, RM_ATTR_PAGE_SIZE attr, NvBool bContiguous) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrSetGlobalCtxBufInfo(arg0, arg1, arg2, size, align, attr, bContiguous) kgrmgrSetGlobalCtxBufInfo_IMPL(arg0, arg1, arg2, size, align, attr, bContiguous)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrDiscoverMaxGlobalCtxBufSizes_IMPL(OBJGPU *arg0, struct KernelGraphicsManager *arg1, struct KernelGraphics *arg2, NvBool bMemoryPartitioningNeeded);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrDiscoverMaxGlobalCtxBufSizes(OBJGPU *arg0, struct KernelGraphicsManager *arg1, struct KernelGraphics *arg2, NvBool bMemoryPartitioningNeeded) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrDiscoverMaxGlobalCtxBufSizes(arg0, arg1, arg2, bMemoryPartitioningNeeded) kgrmgrDiscoverMaxGlobalCtxBufSizes_IMPL(arg0, arg1, arg2, bMemoryPartitioningNeeded)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NvU32 kgrmgrGetLegacyGpcTpcCount_IMPL(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 gpcId);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NvU32 kgrmgrGetLegacyGpcTpcCount(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 gpcId) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return 0;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetLegacyGpcTpcCount(arg0, arg1, gpcId) kgrmgrGetLegacyGpcTpcCount_IMPL(arg0, arg1, gpcId)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrCheckVeidsRequest_IMPL(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU64 *pInUseMask, NvU32 veidCount, NvU32 *pSpanStart, KERNEL_MIG_GPU_INSTANCE *arg2);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrCheckVeidsRequest(OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU64 *pInUseMask, NvU32 veidCount, NvU32 *pSpanStart, KERNEL_MIG_GPU_INSTANCE *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrCheckVeidsRequest(arg0, arg1, pInUseMask, veidCount, pSpanStart, arg2) kgrmgrCheckVeidsRequest_IMPL(arg0, arg1, pInUseMask, veidCount, pSpanStart, arg2)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NvBool kgrmgrSignalFecsCallbackScheduled_IMPL(OBJGPU *pGpu, struct KernelGraphicsManager *pKernelGraphicsManager);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NvBool kgrmgrSignalFecsCallbackScheduled(OBJGPU *pGpu, struct KernelGraphicsManager *pKernelGraphicsManager) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrSignalFecsCallbackScheduled(pGpu, pKernelGraphicsManager) kgrmgrSignalFecsCallbackScheduled_IMPL(pGpu, pKernelGraphicsManager)
#endif //__nvoc_kernel_graphics_manager_h_disabled

void kgrmgrClearFecsCallbackScheduled_IMPL(OBJGPU *pGpu, struct KernelGraphicsManager *pKernelGraphicsManager);

#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline void kgrmgrClearFecsCallbackScheduled(OBJGPU *pGpu, struct KernelGraphicsManager *pKernelGraphicsManager) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrClearFecsCallbackScheduled(pGpu, pKernelGraphicsManager) kgrmgrClearFecsCallbackScheduled_IMPL(pGpu, pKernelGraphicsManager)
#endif //__nvoc_kernel_graphics_manager_h_disabled

#undef PRIVATE_FIELD


#endif // KERNEL_GRAPHICS_MANAGER_H


#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_GRAPHICS_MANAGER_NVOC_H_
