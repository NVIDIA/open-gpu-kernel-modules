#ifndef _G_SWINTR_NVOC_H_
#define _G_SWINTR_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_swintr_nvoc.h"

#ifndef SWINTR_H
#define SWINTR_H

/*!
 * @file   swintr.h
 * @brief  This class exists to give an OBJENGSTATE responsible for the doorbell interrupts.
 */

#include "core/core.h"
#include "gpu/gpu.h"
#include "gpu/eng_state.h"
#include "kernel/gpu/intr/intr_service.h"
#include "kernel/gpu/intrable/intrable.h"

#ifdef NVOC_SWINTR_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct SwIntr {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct OBJINTRABLE __nvoc_base_OBJINTRABLE;
    struct IntrService __nvoc_base_IntrService;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct OBJINTRABLE *__nvoc_pbase_OBJINTRABLE;
    struct IntrService *__nvoc_pbase_IntrService;
    struct SwIntr *__nvoc_pbase_SwIntr;
    void (*__swintrRegisterIntrService__)(OBJGPU *, struct SwIntr *, IntrServiceRecord *);
    NvU32 (*__swintrServiceInterrupt__)(OBJGPU *, struct SwIntr *, IntrServiceServiceInterruptArguments *);
    NV_STATUS (*__swintrGetKernelIntrVectors__)(OBJGPU *, struct SwIntr *, NvU32, NvU32 *, NvU32 *, NvU32 *);
    void (*__swintrStateDestroy__)(POBJGPU, struct SwIntr *);
    void (*__swintrFreeTunableState__)(POBJGPU, struct SwIntr *, void *);
    NV_STATUS (*__swintrCompareTunableState__)(POBJGPU, struct SwIntr *, void *, void *);
    NvBool (*__swintrClearInterrupt__)(OBJGPU *, struct SwIntr *, IntrServiceClearInterruptArguments *);
    NV_STATUS (*__swintrGetNotificationIntrVector__)(OBJGPU *, struct SwIntr *, NvU32 *);
    NvBool (*__swintrIsPresent__)(POBJGPU, struct SwIntr *);
    NV_STATUS (*__swintrReconcileTunableState__)(POBJGPU, struct SwIntr *, void *);
    NV_STATUS (*__swintrStateLoad__)(POBJGPU, struct SwIntr *, NvU32);
    NV_STATUS (*__swintrGetPhysicalIntrVectors__)(OBJGPU *, struct SwIntr *, NvU32, NvU32 *, NvU32 *, NvU32 *);
    NV_STATUS (*__swintrStateUnload__)(POBJGPU, struct SwIntr *, NvU32);
    NV_STATUS (*__swintrSetNotificationIntrVector__)(OBJGPU *, struct SwIntr *, NvU32);
    NV_STATUS (*__swintrStateInitLocked__)(POBJGPU, struct SwIntr *);
    NV_STATUS (*__swintrStatePreLoad__)(POBJGPU, struct SwIntr *, NvU32);
    NV_STATUS (*__swintrStatePostUnload__)(POBJGPU, struct SwIntr *, NvU32);
    NV_STATUS (*__swintrStatePreUnload__)(POBJGPU, struct SwIntr *, NvU32);
    NV_STATUS (*__swintrGetTunableState__)(POBJGPU, struct SwIntr *, void *);
    NV_STATUS (*__swintrStateInitUnlocked__)(POBJGPU, struct SwIntr *);
    void (*__swintrInitMissing__)(POBJGPU, struct SwIntr *);
    NV_STATUS (*__swintrStatePreInitLocked__)(POBJGPU, struct SwIntr *);
    NV_STATUS (*__swintrStatePreInitUnlocked__)(POBJGPU, struct SwIntr *);
    NV_STATUS (*__swintrServiceNotificationInterrupt__)(OBJGPU *, struct SwIntr *, IntrServiceServiceNotificationInterruptArguments *);
    NV_STATUS (*__swintrStatePostLoad__)(POBJGPU, struct SwIntr *, NvU32);
    NV_STATUS (*__swintrAllocTunableState__)(POBJGPU, struct SwIntr *, void **);
    NV_STATUS (*__swintrSetTunableState__)(POBJGPU, struct SwIntr *, void *);
    NV_STATUS (*__swintrConstructEngine__)(POBJGPU, struct SwIntr *, ENGDESCRIPTOR);
};

#ifndef __NVOC_CLASS_SwIntr_TYPEDEF__
#define __NVOC_CLASS_SwIntr_TYPEDEF__
typedef struct SwIntr SwIntr;
#endif /* __NVOC_CLASS_SwIntr_TYPEDEF__ */

#ifndef __nvoc_class_id_SwIntr
#define __nvoc_class_id_SwIntr 0x5ca633
#endif /* __nvoc_class_id_SwIntr */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_SwIntr;

#define __staticCast_SwIntr(pThis) \
    ((pThis)->__nvoc_pbase_SwIntr)

#ifdef __nvoc_swintr_h_disabled
#define __dynamicCast_SwIntr(pThis) ((SwIntr*)NULL)
#else //__nvoc_swintr_h_disabled
#define __dynamicCast_SwIntr(pThis) \
    ((SwIntr*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(SwIntr)))
#endif //__nvoc_swintr_h_disabled

#define PDB_PROP_SWINTR_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_SWINTR_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_SwIntr(SwIntr**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_SwIntr(SwIntr**, Dynamic*, NvU32);
#define __objCreate_SwIntr(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_SwIntr((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define swintrRegisterIntrService(pGpu, pSwIntr, pRecords) swintrRegisterIntrService_DISPATCH(pGpu, pSwIntr, pRecords)
#define swintrServiceInterrupt(pGpu, pSwIntr, pParams) swintrServiceInterrupt_DISPATCH(pGpu, pSwIntr, pParams)
#define swintrGetKernelIntrVectors(pGpu, pSwIntr, maxIntrs, pIntrs, pMcEngineIdxs, pCount) swintrGetKernelIntrVectors_DISPATCH(pGpu, pSwIntr, maxIntrs, pIntrs, pMcEngineIdxs, pCount)
#define swintrGetKernelIntrVectors_HAL(pGpu, pSwIntr, maxIntrs, pIntrs, pMcEngineIdxs, pCount) swintrGetKernelIntrVectors_DISPATCH(pGpu, pSwIntr, maxIntrs, pIntrs, pMcEngineIdxs, pCount)
#define swintrStateDestroy(pGpu, pEngstate) swintrStateDestroy_DISPATCH(pGpu, pEngstate)
#define swintrFreeTunableState(pGpu, pEngstate, pTunableState) swintrFreeTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define swintrCompareTunableState(pGpu, pEngstate, pTunables1, pTunables2) swintrCompareTunableState_DISPATCH(pGpu, pEngstate, pTunables1, pTunables2)
#define swintrClearInterrupt(pGpu, pIntrService, pParams) swintrClearInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define swintrGetNotificationIntrVector(pGpu, pIntrable, pIntrVector) swintrGetNotificationIntrVector_DISPATCH(pGpu, pIntrable, pIntrVector)
#define swintrIsPresent(pGpu, pEngstate) swintrIsPresent_DISPATCH(pGpu, pEngstate)
#define swintrReconcileTunableState(pGpu, pEngstate, pTunableState) swintrReconcileTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define swintrStateLoad(pGpu, pEngstate, arg0) swintrStateLoad_DISPATCH(pGpu, pEngstate, arg0)
#define swintrGetPhysicalIntrVectors(pGpu, pIntrable, maxIntrs, pIntrs, pMcEngineIdxs, pCount) swintrGetPhysicalIntrVectors_DISPATCH(pGpu, pIntrable, maxIntrs, pIntrs, pMcEngineIdxs, pCount)
#define swintrStateUnload(pGpu, pEngstate, arg0) swintrStateUnload_DISPATCH(pGpu, pEngstate, arg0)
#define swintrSetNotificationIntrVector(pGpu, pIntrable, intrVector) swintrSetNotificationIntrVector_DISPATCH(pGpu, pIntrable, intrVector)
#define swintrStateInitLocked(pGpu, pEngstate) swintrStateInitLocked_DISPATCH(pGpu, pEngstate)
#define swintrStatePreLoad(pGpu, pEngstate, arg0) swintrStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define swintrStatePostUnload(pGpu, pEngstate, arg0) swintrStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define swintrStatePreUnload(pGpu, pEngstate, arg0) swintrStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define swintrGetTunableState(pGpu, pEngstate, pTunableState) swintrGetTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define swintrStateInitUnlocked(pGpu, pEngstate) swintrStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define swintrInitMissing(pGpu, pEngstate) swintrInitMissing_DISPATCH(pGpu, pEngstate)
#define swintrStatePreInitLocked(pGpu, pEngstate) swintrStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define swintrStatePreInitUnlocked(pGpu, pEngstate) swintrStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define swintrServiceNotificationInterrupt(pGpu, pIntrService, pParams) swintrServiceNotificationInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define swintrStatePostLoad(pGpu, pEngstate, arg0) swintrStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define swintrAllocTunableState(pGpu, pEngstate, ppTunableState) swintrAllocTunableState_DISPATCH(pGpu, pEngstate, ppTunableState)
#define swintrSetTunableState(pGpu, pEngstate, pTunableState) swintrSetTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define swintrConstructEngine(pGpu, pEngstate, arg0) swintrConstructEngine_DISPATCH(pGpu, pEngstate, arg0)
void swintrRegisterIntrService_IMPL(OBJGPU *pGpu, struct SwIntr *pSwIntr, IntrServiceRecord pRecords[155]);

static inline void swintrRegisterIntrService_DISPATCH(OBJGPU *pGpu, struct SwIntr *pSwIntr, IntrServiceRecord pRecords[155]) {
    pSwIntr->__swintrRegisterIntrService__(pGpu, pSwIntr, pRecords);
}

NvU32 swintrServiceInterrupt_IMPL(OBJGPU *pGpu, struct SwIntr *pSwIntr, IntrServiceServiceInterruptArguments *pParams);

static inline NvU32 swintrServiceInterrupt_DISPATCH(OBJGPU *pGpu, struct SwIntr *pSwIntr, IntrServiceServiceInterruptArguments *pParams) {
    return pSwIntr->__swintrServiceInterrupt__(pGpu, pSwIntr, pParams);
}

static inline NV_STATUS swintrGetKernelIntrVectors_46f6a7(OBJGPU *pGpu, struct SwIntr *pSwIntr, NvU32 maxIntrs, NvU32 *pIntrs, NvU32 *pMcEngineIdxs, NvU32 *pCount) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS swintrGetKernelIntrVectors_DISPATCH(OBJGPU *pGpu, struct SwIntr *pSwIntr, NvU32 maxIntrs, NvU32 *pIntrs, NvU32 *pMcEngineIdxs, NvU32 *pCount) {
    return pSwIntr->__swintrGetKernelIntrVectors__(pGpu, pSwIntr, maxIntrs, pIntrs, pMcEngineIdxs, pCount);
}

static inline void swintrStateDestroy_DISPATCH(POBJGPU pGpu, struct SwIntr *pEngstate) {
    pEngstate->__swintrStateDestroy__(pGpu, pEngstate);
}

static inline void swintrFreeTunableState_DISPATCH(POBJGPU pGpu, struct SwIntr *pEngstate, void *pTunableState) {
    pEngstate->__swintrFreeTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS swintrCompareTunableState_DISPATCH(POBJGPU pGpu, struct SwIntr *pEngstate, void *pTunables1, void *pTunables2) {
    return pEngstate->__swintrCompareTunableState__(pGpu, pEngstate, pTunables1, pTunables2);
}

static inline NvBool swintrClearInterrupt_DISPATCH(OBJGPU *pGpu, struct SwIntr *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return pIntrService->__swintrClearInterrupt__(pGpu, pIntrService, pParams);
}

static inline NV_STATUS swintrGetNotificationIntrVector_DISPATCH(OBJGPU *pGpu, struct SwIntr *pIntrable, NvU32 *pIntrVector) {
    return pIntrable->__swintrGetNotificationIntrVector__(pGpu, pIntrable, pIntrVector);
}

static inline NvBool swintrIsPresent_DISPATCH(POBJGPU pGpu, struct SwIntr *pEngstate) {
    return pEngstate->__swintrIsPresent__(pGpu, pEngstate);
}

static inline NV_STATUS swintrReconcileTunableState_DISPATCH(POBJGPU pGpu, struct SwIntr *pEngstate, void *pTunableState) {
    return pEngstate->__swintrReconcileTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS swintrStateLoad_DISPATCH(POBJGPU pGpu, struct SwIntr *pEngstate, NvU32 arg0) {
    return pEngstate->__swintrStateLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS swintrGetPhysicalIntrVectors_DISPATCH(OBJGPU *pGpu, struct SwIntr *pIntrable, NvU32 maxIntrs, NvU32 *pIntrs, NvU32 *pMcEngineIdxs, NvU32 *pCount) {
    return pIntrable->__swintrGetPhysicalIntrVectors__(pGpu, pIntrable, maxIntrs, pIntrs, pMcEngineIdxs, pCount);
}

static inline NV_STATUS swintrStateUnload_DISPATCH(POBJGPU pGpu, struct SwIntr *pEngstate, NvU32 arg0) {
    return pEngstate->__swintrStateUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS swintrSetNotificationIntrVector_DISPATCH(OBJGPU *pGpu, struct SwIntr *pIntrable, NvU32 intrVector) {
    return pIntrable->__swintrSetNotificationIntrVector__(pGpu, pIntrable, intrVector);
}

static inline NV_STATUS swintrStateInitLocked_DISPATCH(POBJGPU pGpu, struct SwIntr *pEngstate) {
    return pEngstate->__swintrStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS swintrStatePreLoad_DISPATCH(POBJGPU pGpu, struct SwIntr *pEngstate, NvU32 arg0) {
    return pEngstate->__swintrStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS swintrStatePostUnload_DISPATCH(POBJGPU pGpu, struct SwIntr *pEngstate, NvU32 arg0) {
    return pEngstate->__swintrStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS swintrStatePreUnload_DISPATCH(POBJGPU pGpu, struct SwIntr *pEngstate, NvU32 arg0) {
    return pEngstate->__swintrStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS swintrGetTunableState_DISPATCH(POBJGPU pGpu, struct SwIntr *pEngstate, void *pTunableState) {
    return pEngstate->__swintrGetTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS swintrStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct SwIntr *pEngstate) {
    return pEngstate->__swintrStateInitUnlocked__(pGpu, pEngstate);
}

static inline void swintrInitMissing_DISPATCH(POBJGPU pGpu, struct SwIntr *pEngstate) {
    pEngstate->__swintrInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS swintrStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct SwIntr *pEngstate) {
    return pEngstate->__swintrStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS swintrStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct SwIntr *pEngstate) {
    return pEngstate->__swintrStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS swintrServiceNotificationInterrupt_DISPATCH(OBJGPU *pGpu, struct SwIntr *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return pIntrService->__swintrServiceNotificationInterrupt__(pGpu, pIntrService, pParams);
}

static inline NV_STATUS swintrStatePostLoad_DISPATCH(POBJGPU pGpu, struct SwIntr *pEngstate, NvU32 arg0) {
    return pEngstate->__swintrStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS swintrAllocTunableState_DISPATCH(POBJGPU pGpu, struct SwIntr *pEngstate, void **ppTunableState) {
    return pEngstate->__swintrAllocTunableState__(pGpu, pEngstate, ppTunableState);
}

static inline NV_STATUS swintrSetTunableState_DISPATCH(POBJGPU pGpu, struct SwIntr *pEngstate, void *pTunableState) {
    return pEngstate->__swintrSetTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS swintrConstructEngine_DISPATCH(POBJGPU pGpu, struct SwIntr *pEngstate, ENGDESCRIPTOR arg0) {
    return pEngstate->__swintrConstructEngine__(pGpu, pEngstate, arg0);
}

#undef PRIVATE_FIELD


#endif // SWINTR_H

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_SWINTR_NVOC_H_
