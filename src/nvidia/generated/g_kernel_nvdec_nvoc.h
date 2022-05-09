#ifndef _G_KERNEL_NVDEC_NVOC_H_
#define _G_KERNEL_NVDEC_NVOC_H_
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

#include "g_kernel_nvdec_nvoc.h"

#ifndef KERNEL_NVDEC_H
#define KERNEL_NVDEC_H

#include "core/core.h"
#include "gpu/eng_state.h"
#include "gpu/falcon/kernel_falcon.h"
#include "gpu/gpu.h"

#ifdef NVOC_KERNEL_NVDEC_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelNvdec {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct KernelFalcon __nvoc_base_KernelFalcon;
    struct IntrService __nvoc_base_IntrService;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct KernelFalcon *__nvoc_pbase_KernelFalcon;
    struct IntrService *__nvoc_pbase_IntrService;
    struct KernelNvdec *__nvoc_pbase_KernelNvdec;
    NV_STATUS (*__knvdecConstructEngine__)(struct OBJGPU *, struct KernelNvdec *, ENGDESCRIPTOR);
    void (*__knvdecConfigureFalcon__)(struct OBJGPU *, struct KernelNvdec *);
    NV_STATUS (*__knvdecResetHw__)(struct OBJGPU *, struct KernelNvdec *);
    NvBool (*__knvdecIsEngineInReset__)(struct OBJGPU *, struct KernelNvdec *);
    NvBool (*__knvdecIsEngineDisabled__)(struct OBJGPU *, struct KernelNvdec *);
    NvU32 (*__knvdecReadUcodeFuseVersion__)(struct OBJGPU *, struct KernelNvdec *, NvU32);
    void (*__knvdecRegisterIntrService__)(struct OBJGPU *, struct KernelNvdec *, IntrServiceRecord *);
    NV_STATUS (*__knvdecServiceNotificationInterrupt__)(struct OBJGPU *, struct KernelNvdec *, IntrServiceServiceNotificationInterruptArguments *);
    void (*__knvdecStateDestroy__)(POBJGPU, struct KernelNvdec *);
    void (*__knvdecFreeTunableState__)(POBJGPU, struct KernelNvdec *, void *);
    NV_STATUS (*__knvdecCompareTunableState__)(POBJGPU, struct KernelNvdec *, void *, void *);
    NvBool (*__knvdecClearInterrupt__)(struct OBJGPU *, struct KernelNvdec *, IntrServiceClearInterruptArguments *);
    NvBool (*__knvdecIsPresent__)(POBJGPU, struct KernelNvdec *);
    NvU32 (*__knvdecServiceInterrupt__)(struct OBJGPU *, struct KernelNvdec *, IntrServiceServiceInterruptArguments *);
    NV_STATUS (*__knvdecReconcileTunableState__)(POBJGPU, struct KernelNvdec *, void *);
    NV_STATUS (*__knvdecStateLoad__)(POBJGPU, struct KernelNvdec *, NvU32);
    NV_STATUS (*__knvdecStateUnload__)(POBJGPU, struct KernelNvdec *, NvU32);
    NV_STATUS (*__knvdecStateInitLocked__)(POBJGPU, struct KernelNvdec *);
    NV_STATUS (*__knvdecStatePreLoad__)(POBJGPU, struct KernelNvdec *, NvU32);
    NV_STATUS (*__knvdecStatePostUnload__)(POBJGPU, struct KernelNvdec *, NvU32);
    NV_STATUS (*__knvdecStatePreUnload__)(POBJGPU, struct KernelNvdec *, NvU32);
    NV_STATUS (*__knvdecStateInitUnlocked__)(POBJGPU, struct KernelNvdec *);
    void (*__knvdecInitMissing__)(POBJGPU, struct KernelNvdec *);
    NV_STATUS (*__knvdecStatePreInitLocked__)(POBJGPU, struct KernelNvdec *);
    NV_STATUS (*__knvdecStatePreInitUnlocked__)(POBJGPU, struct KernelNvdec *);
    NV_STATUS (*__knvdecGetTunableState__)(POBJGPU, struct KernelNvdec *, void *);
    NV_STATUS (*__knvdecStatePostLoad__)(POBJGPU, struct KernelNvdec *, NvU32);
    NV_STATUS (*__knvdecAllocTunableState__)(POBJGPU, struct KernelNvdec *, void **);
    NV_STATUS (*__knvdecSetTunableState__)(POBJGPU, struct KernelNvdec *, void *);
};

#ifndef __NVOC_CLASS_KernelNvdec_TYPEDEF__
#define __NVOC_CLASS_KernelNvdec_TYPEDEF__
typedef struct KernelNvdec KernelNvdec;
#endif /* __NVOC_CLASS_KernelNvdec_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelNvdec
#define __nvoc_class_id_KernelNvdec 0xaba9df
#endif /* __nvoc_class_id_KernelNvdec */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelNvdec;

#define __staticCast_KernelNvdec(pThis) \
    ((pThis)->__nvoc_pbase_KernelNvdec)

#ifdef __nvoc_kernel_nvdec_h_disabled
#define __dynamicCast_KernelNvdec(pThis) ((KernelNvdec*)NULL)
#else //__nvoc_kernel_nvdec_h_disabled
#define __dynamicCast_KernelNvdec(pThis) \
    ((KernelNvdec*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelNvdec)))
#endif //__nvoc_kernel_nvdec_h_disabled

#define PDB_PROP_KNVDEC_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KNVDEC_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelNvdec(KernelNvdec**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelNvdec(KernelNvdec**, Dynamic*, NvU32);
#define __objCreate_KernelNvdec(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelNvdec((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define knvdecConstructEngine(pGpu, pKernelNvdec, arg0) knvdecConstructEngine_DISPATCH(pGpu, pKernelNvdec, arg0)
#define knvdecConstructEngine_HAL(pGpu, pKernelNvdec, arg0) knvdecConstructEngine_DISPATCH(pGpu, pKernelNvdec, arg0)
#define knvdecConfigureFalcon(pGpu, pKernelNvdec) knvdecConfigureFalcon_DISPATCH(pGpu, pKernelNvdec)
#define knvdecConfigureFalcon_HAL(pGpu, pKernelNvdec) knvdecConfigureFalcon_DISPATCH(pGpu, pKernelNvdec)
#define knvdecResetHw(pGpu, pKernelNvdec) knvdecResetHw_DISPATCH(pGpu, pKernelNvdec)
#define knvdecResetHw_HAL(pGpu, pKernelNvdec) knvdecResetHw_DISPATCH(pGpu, pKernelNvdec)
#define knvdecIsEngineInReset(pGpu, pKernelNvdec) knvdecIsEngineInReset_DISPATCH(pGpu, pKernelNvdec)
#define knvdecIsEngineDisabled(pGpu, pKernelNvdec) knvdecIsEngineDisabled_DISPATCH(pGpu, pKernelNvdec)
#define knvdecIsEngineDisabled_HAL(pGpu, pKernelNvdec) knvdecIsEngineDisabled_DISPATCH(pGpu, pKernelNvdec)
#define knvdecReadUcodeFuseVersion(pGpu, pKernelNvdec, ucodeId) knvdecReadUcodeFuseVersion_DISPATCH(pGpu, pKernelNvdec, ucodeId)
#define knvdecReadUcodeFuseVersion_HAL(pGpu, pKernelNvdec, ucodeId) knvdecReadUcodeFuseVersion_DISPATCH(pGpu, pKernelNvdec, ucodeId)
#define knvdecRegisterIntrService(arg0, arg1, arg2) knvdecRegisterIntrService_DISPATCH(arg0, arg1, arg2)
#define knvdecServiceNotificationInterrupt(arg0, arg1, arg2) knvdecServiceNotificationInterrupt_DISPATCH(arg0, arg1, arg2)
#define knvdecStateDestroy(pGpu, pEngstate) knvdecStateDestroy_DISPATCH(pGpu, pEngstate)
#define knvdecFreeTunableState(pGpu, pEngstate, pTunableState) knvdecFreeTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define knvdecCompareTunableState(pGpu, pEngstate, pTunables1, pTunables2) knvdecCompareTunableState_DISPATCH(pGpu, pEngstate, pTunables1, pTunables2)
#define knvdecClearInterrupt(pGpu, pIntrService, pParams) knvdecClearInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define knvdecIsPresent(pGpu, pEngstate) knvdecIsPresent_DISPATCH(pGpu, pEngstate)
#define knvdecServiceInterrupt(pGpu, pIntrService, pParams) knvdecServiceInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define knvdecReconcileTunableState(pGpu, pEngstate, pTunableState) knvdecReconcileTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define knvdecStateLoad(pGpu, pEngstate, arg0) knvdecStateLoad_DISPATCH(pGpu, pEngstate, arg0)
#define knvdecStateUnload(pGpu, pEngstate, arg0) knvdecStateUnload_DISPATCH(pGpu, pEngstate, arg0)
#define knvdecStateInitLocked(pGpu, pEngstate) knvdecStateInitLocked_DISPATCH(pGpu, pEngstate)
#define knvdecStatePreLoad(pGpu, pEngstate, arg0) knvdecStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define knvdecStatePostUnload(pGpu, pEngstate, arg0) knvdecStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define knvdecStatePreUnload(pGpu, pEngstate, arg0) knvdecStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define knvdecStateInitUnlocked(pGpu, pEngstate) knvdecStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define knvdecInitMissing(pGpu, pEngstate) knvdecInitMissing_DISPATCH(pGpu, pEngstate)
#define knvdecStatePreInitLocked(pGpu, pEngstate) knvdecStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define knvdecStatePreInitUnlocked(pGpu, pEngstate) knvdecStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define knvdecGetTunableState(pGpu, pEngstate, pTunableState) knvdecGetTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
#define knvdecStatePostLoad(pGpu, pEngstate, arg0) knvdecStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define knvdecAllocTunableState(pGpu, pEngstate, ppTunableState) knvdecAllocTunableState_DISPATCH(pGpu, pEngstate, ppTunableState)
#define knvdecSetTunableState(pGpu, pEngstate, pTunableState) knvdecSetTunableState_DISPATCH(pGpu, pEngstate, pTunableState)
void knvdecNonstallIntrCheckAndClear_TU102(struct OBJGPU *arg0, struct KernelNvdec *arg1, struct THREAD_STATE_NODE *arg2);

#ifdef __nvoc_kernel_nvdec_h_disabled
static inline void knvdecNonstallIntrCheckAndClear(struct OBJGPU *arg0, struct KernelNvdec *arg1, struct THREAD_STATE_NODE *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelNvdec was disabled!");
}
#else //__nvoc_kernel_nvdec_h_disabled
#define knvdecNonstallIntrCheckAndClear(arg0, arg1, arg2) knvdecNonstallIntrCheckAndClear_TU102(arg0, arg1, arg2)
#endif //__nvoc_kernel_nvdec_h_disabled

#define knvdecNonstallIntrCheckAndClear_HAL(arg0, arg1, arg2) knvdecNonstallIntrCheckAndClear(arg0, arg1, arg2)

NV_STATUS knvdecConstructEngine_IMPL(struct OBJGPU *pGpu, struct KernelNvdec *pKernelNvdec, ENGDESCRIPTOR arg0);

static inline NV_STATUS knvdecConstructEngine_395e98(struct OBJGPU *pGpu, struct KernelNvdec *pKernelNvdec, ENGDESCRIPTOR arg0) {
    return NV_ERR_NOT_SUPPORTED;
}

static inline NV_STATUS knvdecConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelNvdec *pKernelNvdec, ENGDESCRIPTOR arg0) {
    return pKernelNvdec->__knvdecConstructEngine__(pGpu, pKernelNvdec, arg0);
}

void knvdecConfigureFalcon_TU102(struct OBJGPU *pGpu, struct KernelNvdec *pKernelNvdec);

void knvdecConfigureFalcon_GA100(struct OBJGPU *pGpu, struct KernelNvdec *pKernelNvdec);

void knvdecConfigureFalcon_GA102(struct OBJGPU *pGpu, struct KernelNvdec *pKernelNvdec);

static inline void knvdecConfigureFalcon_f2d351(struct OBJGPU *pGpu, struct KernelNvdec *pKernelNvdec) {
    NV_ASSERT_PRECOMP(0);
}

static inline void knvdecConfigureFalcon_DISPATCH(struct OBJGPU *pGpu, struct KernelNvdec *pKernelNvdec) {
    pKernelNvdec->__knvdecConfigureFalcon__(pGpu, pKernelNvdec);
}

static inline NV_STATUS knvdecResetHw_ac1694(struct OBJGPU *pGpu, struct KernelNvdec *pKernelNvdec) {
    return NV_OK;
}

static inline NV_STATUS knvdecResetHw_5baef9(struct OBJGPU *pGpu, struct KernelNvdec *pKernelNvdec) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

static inline NV_STATUS knvdecResetHw_DISPATCH(struct OBJGPU *pGpu, struct KernelNvdec *pKernelNvdec) {
    return pKernelNvdec->__knvdecResetHw__(pGpu, pKernelNvdec);
}

static inline NvBool knvdecIsEngineInReset_167f46(struct OBJGPU *pGpu, struct KernelNvdec *pKernelNvdec) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((NvBool)(0 != 0)));
    return ((NvBool)(0 != 0));
}

static inline NvBool knvdecIsEngineInReset_DISPATCH(struct OBJGPU *pGpu, struct KernelNvdec *pKernelNvdec) {
    return pKernelNvdec->__knvdecIsEngineInReset__(pGpu, pKernelNvdec);
}

NvBool knvdecIsEngineDisabled_TU102(struct OBJGPU *pGpu, struct KernelNvdec *pKernelNvdec);

NvBool knvdecIsEngineDisabled_GA100(struct OBJGPU *pGpu, struct KernelNvdec *pKernelNvdec);

NvBool knvdecIsEngineDisabled_GA102(struct OBJGPU *pGpu, struct KernelNvdec *pKernelNvdec);

static inline NvBool knvdecIsEngineDisabled_108313(struct OBJGPU *pGpu, struct KernelNvdec *pKernelNvdec) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, ((NvBool)(0 != 0)));
}

static inline NvBool knvdecIsEngineDisabled_DISPATCH(struct OBJGPU *pGpu, struct KernelNvdec *pKernelNvdec) {
    return pKernelNvdec->__knvdecIsEngineDisabled__(pGpu, pKernelNvdec);
}

static inline NvU32 knvdecReadUcodeFuseVersion_b2b553(struct OBJGPU *pGpu, struct KernelNvdec *pKernelNvdec, NvU32 ucodeId) {
    return 0;
}

NvU32 knvdecReadUcodeFuseVersion_GA100(struct OBJGPU *pGpu, struct KernelNvdec *pKernelNvdec, NvU32 ucodeId);

static inline NvU32 knvdecReadUcodeFuseVersion_474d46(struct OBJGPU *pGpu, struct KernelNvdec *pKernelNvdec, NvU32 ucodeId) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, 0);
}

static inline NvU32 knvdecReadUcodeFuseVersion_DISPATCH(struct OBJGPU *pGpu, struct KernelNvdec *pKernelNvdec, NvU32 ucodeId) {
    return pKernelNvdec->__knvdecReadUcodeFuseVersion__(pGpu, pKernelNvdec, ucodeId);
}

void knvdecRegisterIntrService_IMPL(struct OBJGPU *arg0, struct KernelNvdec *arg1, IntrServiceRecord arg2[155]);

static inline void knvdecRegisterIntrService_DISPATCH(struct OBJGPU *arg0, struct KernelNvdec *arg1, IntrServiceRecord arg2[155]) {
    arg1->__knvdecRegisterIntrService__(arg0, arg1, arg2);
}

NV_STATUS knvdecServiceNotificationInterrupt_IMPL(struct OBJGPU *arg0, struct KernelNvdec *arg1, IntrServiceServiceNotificationInterruptArguments *arg2);

static inline NV_STATUS knvdecServiceNotificationInterrupt_DISPATCH(struct OBJGPU *arg0, struct KernelNvdec *arg1, IntrServiceServiceNotificationInterruptArguments *arg2) {
    return arg1->__knvdecServiceNotificationInterrupt__(arg0, arg1, arg2);
}

static inline void knvdecStateDestroy_DISPATCH(POBJGPU pGpu, struct KernelNvdec *pEngstate) {
    pEngstate->__knvdecStateDestroy__(pGpu, pEngstate);
}

static inline void knvdecFreeTunableState_DISPATCH(POBJGPU pGpu, struct KernelNvdec *pEngstate, void *pTunableState) {
    pEngstate->__knvdecFreeTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS knvdecCompareTunableState_DISPATCH(POBJGPU pGpu, struct KernelNvdec *pEngstate, void *pTunables1, void *pTunables2) {
    return pEngstate->__knvdecCompareTunableState__(pGpu, pEngstate, pTunables1, pTunables2);
}

static inline NvBool knvdecClearInterrupt_DISPATCH(struct OBJGPU *pGpu, struct KernelNvdec *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return pIntrService->__knvdecClearInterrupt__(pGpu, pIntrService, pParams);
}

static inline NvBool knvdecIsPresent_DISPATCH(POBJGPU pGpu, struct KernelNvdec *pEngstate) {
    return pEngstate->__knvdecIsPresent__(pGpu, pEngstate);
}

static inline NvU32 knvdecServiceInterrupt_DISPATCH(struct OBJGPU *pGpu, struct KernelNvdec *pIntrService, IntrServiceServiceInterruptArguments *pParams) {
    return pIntrService->__knvdecServiceInterrupt__(pGpu, pIntrService, pParams);
}

static inline NV_STATUS knvdecReconcileTunableState_DISPATCH(POBJGPU pGpu, struct KernelNvdec *pEngstate, void *pTunableState) {
    return pEngstate->__knvdecReconcileTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS knvdecStateLoad_DISPATCH(POBJGPU pGpu, struct KernelNvdec *pEngstate, NvU32 arg0) {
    return pEngstate->__knvdecStateLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS knvdecStateUnload_DISPATCH(POBJGPU pGpu, struct KernelNvdec *pEngstate, NvU32 arg0) {
    return pEngstate->__knvdecStateUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS knvdecStateInitLocked_DISPATCH(POBJGPU pGpu, struct KernelNvdec *pEngstate) {
    return pEngstate->__knvdecStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS knvdecStatePreLoad_DISPATCH(POBJGPU pGpu, struct KernelNvdec *pEngstate, NvU32 arg0) {
    return pEngstate->__knvdecStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS knvdecStatePostUnload_DISPATCH(POBJGPU pGpu, struct KernelNvdec *pEngstate, NvU32 arg0) {
    return pEngstate->__knvdecStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS knvdecStatePreUnload_DISPATCH(POBJGPU pGpu, struct KernelNvdec *pEngstate, NvU32 arg0) {
    return pEngstate->__knvdecStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS knvdecStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelNvdec *pEngstate) {
    return pEngstate->__knvdecStateInitUnlocked__(pGpu, pEngstate);
}

static inline void knvdecInitMissing_DISPATCH(POBJGPU pGpu, struct KernelNvdec *pEngstate) {
    pEngstate->__knvdecInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS knvdecStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct KernelNvdec *pEngstate) {
    return pEngstate->__knvdecStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS knvdecStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelNvdec *pEngstate) {
    return pEngstate->__knvdecStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS knvdecGetTunableState_DISPATCH(POBJGPU pGpu, struct KernelNvdec *pEngstate, void *pTunableState) {
    return pEngstate->__knvdecGetTunableState__(pGpu, pEngstate, pTunableState);
}

static inline NV_STATUS knvdecStatePostLoad_DISPATCH(POBJGPU pGpu, struct KernelNvdec *pEngstate, NvU32 arg0) {
    return pEngstate->__knvdecStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS knvdecAllocTunableState_DISPATCH(POBJGPU pGpu, struct KernelNvdec *pEngstate, void **ppTunableState) {
    return pEngstate->__knvdecAllocTunableState__(pGpu, pEngstate, ppTunableState);
}

static inline NV_STATUS knvdecSetTunableState_DISPATCH(POBJGPU pGpu, struct KernelNvdec *pEngstate, void *pTunableState) {
    return pEngstate->__knvdecSetTunableState__(pGpu, pEngstate, pTunableState);
}

#undef PRIVATE_FIELD


#endif  // KERNEL_NVDEC_H

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_KERNEL_NVDEC_NVOC_H_
