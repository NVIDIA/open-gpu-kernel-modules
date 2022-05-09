#ifndef _G_KERNEL_GRAPHICS_MANAGER_NVOC_H_
#define _G_KERNEL_GRAPHICS_MANAGER_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * KernelGraphicsManager provides means to access KernelGraphics engine with specified index.
 * It also houses information at a higher level or that is common between KernelGraphics engines.
 */
#ifdef NVOC_KERNEL_GRAPHICS_MANAGER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct GRMGR_LEGACY_KGRAPHICS_STATIC_INFO {
    NV2080_CTRL_INTERNAL_STATIC_GR_FLOORSWEEPING_MASKS floorsweepingMasks;
    NV2080_CTRL_INTERNAL_STATIC_GR_PPC_MASKS *pPpcMasks;
    NV2080_CTRL_INTERNAL_STATIC_GR_INFO *pGrInfo;
    NvBool bInitialized;
};


struct KernelGraphicsManager {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct KernelGraphicsManager *__nvoc_pbase_KernelGraphicsManager;
    NV_STATUS (*__kgrmgrConstructEngine__)(struct OBJGPU *, struct KernelGraphicsManager *, ENGDESCRIPTOR);
    NV_STATUS (*__kgrmgrReconcileTunableState__)(POBJGPU, struct KernelGraphicsManager *, void *);
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
    NV_STATUS (*__kgrmgrGetTunableState__)(POBJGPU, struct KernelGraphicsManager *, void *);
    NV_STATUS (*__kgrmgrCompareTunableState__)(POBJGPU, struct KernelGraphicsManager *, void *, void *);
    void (*__kgrmgrFreeTunableState__)(POBJGPU, struct KernelGraphicsManager *, void *);
    NV_STATUS (*__kgrmgrStatePostLoad__)(POBJGPU, struct KernelGraphicsManager *, NvU32);
    NV_STATUS (*__kgrmgrAllocTunableState__)(POBJGPU, struct KernelGraphicsManager *, void **);
    NV_STATUS (*__kgrmgrSetTunableState__)(POBJGPU, struct KernelGraphicsManager *, void *);
    NvBool (*__kgrmgrIsPresent__)(POBJGPU, struct KernelGraphicsManager *);
    struct GRMGR_LEGACY_KGRAPHICS_STATIC_INFO legacyKgraphicsStaticInfo;
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
#define kgrmgrReconcileTunableState(pGpu, pEngstate, pTunableState) kgrmgrReconcileTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
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
#define kgrmgrGetTunableState(pGpu, pEngstate, pTunableState) kgrmgrGetTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define kgrmgrCompareTunableState(pGpu, pEngstate, pTunables1, pTunables2) kgrmgrCompareTunableState_DISPATCH(pGpu, pEngstate, pTunables1, pTunables2)
#define kgrmgrFreeTunableState(pGpu, pEngstate, pTunableState) kgrmgrFreeTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define kgrmgrStatePostLoad(pGpu, pEngstate, arg0) kgrmgrStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kgrmgrAllocTunableState(pGpu, pEngstate, ppTunableState) kgrmgrAllocTunableState_DISPATCH(pGpu, pEngstate, ppTunableState)
#define kgrmgrSetTunableState(pGpu, pEngstate, pTunableState) kgrmgrSetTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define kgrmgrIsPresent(pGpu, pEngstate) kgrmgrIsPresent_DISPATCH(pGpu, pEngstate)
NV_STATUS kgrmgrConstructEngine_IMPL(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, ENGDESCRIPTOR arg2);

static inline NV_STATUS kgrmgrConstructEngine_DISPATCH(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, ENGDESCRIPTOR arg2) {
    return arg1->__kgrmgrConstructEngine__(arg0, arg1, arg2);
}

static inline NV_STATUS kgrmgrReconcileTunableState_DISPATCH(POBJGPU pGpu, struct KernelGraphicsManager *pEngstate, void *pTunableState) {
    return pEngstate->__kgrmgrReconcileTunableState__(pGpu, pEngstate, pTunableState);
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

static inline NV_STATUS kgrmgrGetTunableState_DISPATCH(POBJGPU pGpu, struct KernelGraphicsManager *pEngstate, void *pTunableState) {
    return pEngstate->__kgrmgrGetTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS kgrmgrCompareTunableState_DISPATCH(POBJGPU pGpu, struct KernelGraphicsManager *pEngstate, void *pTunables1, void *pTunables2) {
    return pEngstate->__kgrmgrCompareTunableState__(pGpu, pEngstate, pTunables1, pTunables2);
}

static inline void kgrmgrFreeTunableState_DISPATCH(POBJGPU pGpu, struct KernelGraphicsManager *pEngstate, void *pTunableState) {
    pEngstate->__kgrmgrFreeTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS kgrmgrStatePostLoad_DISPATCH(POBJGPU pGpu, struct KernelGraphicsManager *pEngstate, NvU32 arg0) {
    return pEngstate->__kgrmgrStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kgrmgrAllocTunableState_DISPATCH(POBJGPU pGpu, struct KernelGraphicsManager *pEngstate, void **ppTunableState) {
    return pEngstate->__kgrmgrAllocTunableState__(pGpu, pEngstate, ppTunableState);
}

static inline NV_STATUS kgrmgrSetTunableState_DISPATCH(POBJGPU pGpu, struct KernelGraphicsManager *pEngstate, void *pTunableState) {
    return pEngstate->__kgrmgrSetTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NvBool kgrmgrIsPresent_DISPATCH(POBJGPU pGpu, struct KernelGraphicsManager *pEngstate) {
    return pEngstate->__kgrmgrIsPresent__(pGpu, pEngstate);
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
void kgrmgrSetLegacyKgraphicsStaticInfo_IMPL(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, struct KernelGraphics *arg2);
#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline void kgrmgrSetLegacyKgraphicsStaticInfo(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, struct KernelGraphics *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrSetLegacyKgraphicsStaticInfo(arg0, arg1, arg2) kgrmgrSetLegacyKgraphicsStaticInfo_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrCtrlRouteKGR_IMPL(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvHandle hClient, const NV2080_CTRL_GR_ROUTE_INFO *pGrRouteInfo, struct KernelGraphics **ppKernelGraphics);
#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrCtrlRouteKGR(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvHandle hClient, const NV2080_CTRL_GR_ROUTE_INFO *pGrRouteInfo, struct KernelGraphics **ppKernelGraphics) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrCtrlRouteKGR(arg0, arg1, hClient, pGrRouteInfo, ppKernelGraphics) kgrmgrCtrlRouteKGR_IMPL(arg0, arg1, hClient, pGrRouteInfo, ppKernelGraphics)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NvU32 kgrmgrGetLegacyGpcMask_IMPL(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1);
#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NvU32 kgrmgrGetLegacyGpcMask(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return 0;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetLegacyGpcMask(arg0, arg1) kgrmgrGetLegacyGpcMask_IMPL(arg0, arg1)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NvU32 kgrmgrGetLegacyTpcMask_IMPL(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 gpcId);
#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NvU32 kgrmgrGetLegacyTpcMask(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 gpcId) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return 0;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetLegacyTpcMask(arg0, arg1, gpcId) kgrmgrGetLegacyTpcMask_IMPL(arg0, arg1, gpcId)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrGetLegacyPpcMask_IMPL(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 physGpcId, NvU32 *pPpcMask);
#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrGetLegacyPpcMask(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 physGpcId, NvU32 *pPpcMask) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetLegacyPpcMask(arg0, arg1, physGpcId, pPpcMask) kgrmgrGetLegacyPpcMask_IMPL(arg0, arg1, physGpcId, pPpcMask)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NvU32 kgrmgrGetLegacyZcullMask_IMPL(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 physGpcId);
#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NvU32 kgrmgrGetLegacyZcullMask(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 physGpcId) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return 0;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetLegacyZcullMask(arg0, arg1, physGpcId) kgrmgrGetLegacyZcullMask_IMPL(arg0, arg1, physGpcId)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrAllocVeidsForGrIdx_IMPL(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 grIdx, NvU32 gpcCount, KERNEL_MIG_GPU_INSTANCE *arg2);
#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrAllocVeidsForGrIdx(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 grIdx, NvU32 gpcCount, KERNEL_MIG_GPU_INSTANCE *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrAllocVeidsForGrIdx(arg0, arg1, grIdx, gpcCount, arg2) kgrmgrAllocVeidsForGrIdx_IMPL(arg0, arg1, grIdx, gpcCount, arg2)
#endif //__nvoc_kernel_graphics_manager_h_disabled

void kgrmgrClearVeidsForGrIdx_IMPL(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 grIdx);
#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline void kgrmgrClearVeidsForGrIdx(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 grIdx) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrClearVeidsForGrIdx(arg0, arg1, grIdx) kgrmgrClearVeidsForGrIdx_IMPL(arg0, arg1, grIdx)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrGetMaxVeidsPerGpc_IMPL(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 *pMaxVeidsPerGpc);
#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrGetMaxVeidsPerGpc(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 *pMaxVeidsPerGpc) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetMaxVeidsPerGpc(arg0, arg1, pMaxVeidsPerGpc) kgrmgrGetMaxVeidsPerGpc_IMPL(arg0, arg1, pMaxVeidsPerGpc)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrGetVeidBaseForGrIdx_IMPL(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 grIdx, NvU32 *pVeidStart);
#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrGetVeidBaseForGrIdx(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 grIdx, NvU32 *pVeidStart) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetVeidBaseForGrIdx(arg0, arg1, grIdx, pVeidStart) kgrmgrGetVeidBaseForGrIdx_IMPL(arg0, arg1, grIdx, pVeidStart)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrGetGrIdxForVeid_IMPL(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 veid, NvU32 *pGrIdx);
#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrGetGrIdxForVeid(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 veid, NvU32 *pGrIdx) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetGrIdxForVeid(arg0, arg1, veid, pGrIdx) kgrmgrGetGrIdxForVeid_IMPL(arg0, arg1, veid, pGrIdx)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrDiscoverMaxLocalCtxBufInfo_IMPL(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, struct KernelGraphics *arg2, NvU32 swizzId);
#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrDiscoverMaxLocalCtxBufInfo(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, struct KernelGraphics *arg2, NvU32 swizzId) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrDiscoverMaxLocalCtxBufInfo(arg0, arg1, arg2, swizzId) kgrmgrDiscoverMaxLocalCtxBufInfo_IMPL(arg0, arg1, arg2, swizzId)
#endif //__nvoc_kernel_graphics_manager_h_disabled

const CTX_BUF_INFO *kgrmgrGetGlobalCtxBufInfo_IMPL(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, GR_GLOBALCTX_BUFFER arg2);
#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline const CTX_BUF_INFO *kgrmgrGetGlobalCtxBufInfo(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, GR_GLOBALCTX_BUFFER arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NULL;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetGlobalCtxBufInfo(arg0, arg1, arg2) kgrmgrGetGlobalCtxBufInfo_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_graphics_manager_h_disabled

void kgrmgrSetGlobalCtxBufInfo_IMPL(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, GR_GLOBALCTX_BUFFER arg2, NvU64 size, NvU64 align, RM_ATTR_PAGE_SIZE attr, NvBool bContiguous);
#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline void kgrmgrSetGlobalCtxBufInfo(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, GR_GLOBALCTX_BUFFER arg2, NvU64 size, NvU64 align, RM_ATTR_PAGE_SIZE attr, NvBool bContiguous) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrSetGlobalCtxBufInfo(arg0, arg1, arg2, size, align, attr, bContiguous) kgrmgrSetGlobalCtxBufInfo_IMPL(arg0, arg1, arg2, size, align, attr, bContiguous)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NV_STATUS kgrmgrDiscoverMaxGlobalCtxBufSizes_IMPL(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, struct KernelGraphics *arg2, NvBool bMemoryPartitioningNeeded);
#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NV_STATUS kgrmgrDiscoverMaxGlobalCtxBufSizes(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, struct KernelGraphics *arg2, NvBool bMemoryPartitioningNeeded) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrDiscoverMaxGlobalCtxBufSizes(arg0, arg1, arg2, bMemoryPartitioningNeeded) kgrmgrDiscoverMaxGlobalCtxBufSizes_IMPL(arg0, arg1, arg2, bMemoryPartitioningNeeded)
#endif //__nvoc_kernel_graphics_manager_h_disabled

NvU32 kgrmgrGetLegacyGpcTpcCount_IMPL(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 gpcId);
#ifdef __nvoc_kernel_graphics_manager_h_disabled
static inline NvU32 kgrmgrGetLegacyGpcTpcCount(struct OBJGPU *arg0, struct KernelGraphicsManager *arg1, NvU32 gpcId) {
    NV_ASSERT_FAILED_PRECOMP("KernelGraphicsManager was disabled!");
    return 0;
}
#else //__nvoc_kernel_graphics_manager_h_disabled
#define kgrmgrGetLegacyGpcTpcCount(arg0, arg1, gpcId) kgrmgrGetLegacyGpcTpcCount_IMPL(arg0, arg1, gpcId)
#endif //__nvoc_kernel_graphics_manager_h_disabled

#undef PRIVATE_FIELD


#endif // KERNEL_GRAPHICS_MANAGER_H


#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_KERNEL_GRAPHICS_MANAGER_NVOC_H_
