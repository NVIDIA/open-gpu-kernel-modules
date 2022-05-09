#ifndef _G_INTRABLE_NVOC_H_
#define _G_INTRABLE_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2019-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_intrable_nvoc.h"

#ifndef INTRABLE_H
#define INTRABLE_H

/*!
 * @file   intrable.h
 * @brief  Provides definitions for all OBJINTRABLE data structures and interfaces.
 */

#include "core/core.h"
#include "gpu/gpu_halspec.h"

typedef struct OBJINTRABLE *POBJINTRABLE;

/*!
 * Interface class for all Intrable modules.
 */
#ifdef NVOC_INTRABLE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct OBJINTRABLE {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJINTRABLE *__nvoc_pbase_OBJINTRABLE;
    NV_STATUS (*__intrableGetNotificationIntrVector__)(OBJGPU *, struct OBJINTRABLE *, NvU32 *);
    NV_STATUS (*__intrableGetPhysicalIntrVectors__)(OBJGPU *, struct OBJINTRABLE *, NvU32, NvU32 *, NvU32 *, NvU32 *);
    NV_STATUS (*__intrableGetKernelIntrVectors__)(OBJGPU *, struct OBJINTRABLE *, NvU32, NvU32 *, NvU32 *, NvU32 *);
    NV_STATUS (*__intrableSetNotificationIntrVector__)(OBJGPU *, struct OBJINTRABLE *, NvU32);
    NvU32 partitionAssignedNotificationVector;
    NvU32 originalNotificationIntrVector;
};

#ifndef __NVOC_CLASS_OBJINTRABLE_TYPEDEF__
#define __NVOC_CLASS_OBJINTRABLE_TYPEDEF__
typedef struct OBJINTRABLE OBJINTRABLE;
#endif /* __NVOC_CLASS_OBJINTRABLE_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJINTRABLE
#define __nvoc_class_id_OBJINTRABLE 0x31ccb7
#endif /* __nvoc_class_id_OBJINTRABLE */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJINTRABLE;

#define __staticCast_OBJINTRABLE(pThis) \
    ((pThis)->__nvoc_pbase_OBJINTRABLE)

#ifdef __nvoc_intrable_h_disabled
#define __dynamicCast_OBJINTRABLE(pThis) ((OBJINTRABLE*)NULL)
#else //__nvoc_intrable_h_disabled
#define __dynamicCast_OBJINTRABLE(pThis) \
    ((OBJINTRABLE*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJINTRABLE)))
#endif //__nvoc_intrable_h_disabled


NV_STATUS __nvoc_objCreateDynamic_OBJINTRABLE(OBJINTRABLE**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJINTRABLE(OBJINTRABLE**, Dynamic*, NvU32);
#define __objCreate_OBJINTRABLE(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJINTRABLE((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define intrableGetNotificationIntrVector(pGpu, pIntrable, pIntrVector) intrableGetNotificationIntrVector_DISPATCH(pGpu, pIntrable, pIntrVector)
#define intrableGetPhysicalIntrVectors(pGpu, pIntrable, maxIntrs, pIntrs, pMcEngineIdxs, pCount) intrableGetPhysicalIntrVectors_DISPATCH(pGpu, pIntrable, maxIntrs, pIntrs, pMcEngineIdxs, pCount)
#define intrableGetKernelIntrVectors(pGpu, pIntrable, maxIntrs, pIntrs, pMcEngineIdxs, pCount) intrableGetKernelIntrVectors_DISPATCH(pGpu, pIntrable, maxIntrs, pIntrs, pMcEngineIdxs, pCount)
#define intrableSetNotificationIntrVector(pGpu, pIntrable, intrVector) intrableSetNotificationIntrVector_DISPATCH(pGpu, pIntrable, intrVector)
static inline NvU32 intrableUpdateIntrCtrlValue_4a4dee(OBJGPU *pGpu, struct OBJINTRABLE *pIntrable, NvU32 intrCtrl, NvU32 routing) {
    return 0;
}

#ifdef __nvoc_intrable_h_disabled
static inline NvU32 intrableUpdateIntrCtrlValue(OBJGPU *pGpu, struct OBJINTRABLE *pIntrable, NvU32 intrCtrl, NvU32 routing) {
    NV_ASSERT_FAILED_PRECOMP("OBJINTRABLE was disabled!");
    return 0;
}
#else //__nvoc_intrable_h_disabled
#define intrableUpdateIntrCtrlValue(pGpu, pIntrable, intrCtrl, routing) intrableUpdateIntrCtrlValue_4a4dee(pGpu, pIntrable, intrCtrl, routing)
#endif //__nvoc_intrable_h_disabled

#define intrableUpdateIntrCtrlValue_HAL(pGpu, pIntrable, intrCtrl, routing) intrableUpdateIntrCtrlValue(pGpu, pIntrable, intrCtrl, routing)

static inline void intrableSetRouting_b3696a(OBJGPU *pGpu, struct OBJINTRABLE *pIntrable, NvU32 intrCtrl, NvU32 intrCtrlReg, NvU32 routing) {
    return;
}

#ifdef __nvoc_intrable_h_disabled
static inline void intrableSetRouting(OBJGPU *pGpu, struct OBJINTRABLE *pIntrable, NvU32 intrCtrl, NvU32 intrCtrlReg, NvU32 routing) {
    NV_ASSERT_FAILED_PRECOMP("OBJINTRABLE was disabled!");
}
#else //__nvoc_intrable_h_disabled
#define intrableSetRouting(pGpu, pIntrable, intrCtrl, intrCtrlReg, routing) intrableSetRouting_b3696a(pGpu, pIntrable, intrCtrl, intrCtrlReg, routing)
#endif //__nvoc_intrable_h_disabled

#define intrableSetRouting_HAL(pGpu, pIntrable, intrCtrl, intrCtrlReg, routing) intrableSetRouting(pGpu, pIntrable, intrCtrl, intrCtrlReg, routing)

NV_STATUS intrableGetNotificationIntrVector_IMPL(OBJGPU *pGpu, struct OBJINTRABLE *pIntrable, NvU32 *pIntrVector);

static inline NV_STATUS intrableGetNotificationIntrVector_DISPATCH(OBJGPU *pGpu, struct OBJINTRABLE *pIntrable, NvU32 *pIntrVector) {
    return pIntrable->__intrableGetNotificationIntrVector__(pGpu, pIntrable, pIntrVector);
}

NV_STATUS intrableGetPhysicalIntrVectors_IMPL(OBJGPU *pGpu, struct OBJINTRABLE *pIntrable, NvU32 maxIntrs, NvU32 *pIntrs, NvU32 *pMcEngineIdxs, NvU32 *pCount);

static inline NV_STATUS intrableGetPhysicalIntrVectors_DISPATCH(OBJGPU *pGpu, struct OBJINTRABLE *pIntrable, NvU32 maxIntrs, NvU32 *pIntrs, NvU32 *pMcEngineIdxs, NvU32 *pCount) {
    return pIntrable->__intrableGetPhysicalIntrVectors__(pGpu, pIntrable, maxIntrs, pIntrs, pMcEngineIdxs, pCount);
}

NV_STATUS intrableGetKernelIntrVectors_IMPL(OBJGPU *pGpu, struct OBJINTRABLE *pIntrable, NvU32 maxIntrs, NvU32 *pIntrs, NvU32 *pMcEngineIdxs, NvU32 *pCount);

static inline NV_STATUS intrableGetKernelIntrVectors_DISPATCH(OBJGPU *pGpu, struct OBJINTRABLE *pIntrable, NvU32 maxIntrs, NvU32 *pIntrs, NvU32 *pMcEngineIdxs, NvU32 *pCount) {
    return pIntrable->__intrableGetKernelIntrVectors__(pGpu, pIntrable, maxIntrs, pIntrs, pMcEngineIdxs, pCount);
}

NV_STATUS intrableSetNotificationIntrVector_IMPL(OBJGPU *pGpu, struct OBJINTRABLE *pIntrable, NvU32 intrVector);

static inline NV_STATUS intrableSetNotificationIntrVector_DISPATCH(OBJGPU *pGpu, struct OBJINTRABLE *pIntrable, NvU32 intrVector) {
    return pIntrable->__intrableSetNotificationIntrVector__(pGpu, pIntrable, intrVector);
}

NV_STATUS intrableConstruct_IMPL(struct OBJINTRABLE *arg_pIntrable);
#define __nvoc_intrableConstruct(arg_pIntrable) intrableConstruct_IMPL(arg_pIntrable)
NV_STATUS intrableCacheAndSetPartitionNotificationIntrVector_IMPL(OBJGPU *pGpu, struct OBJINTRABLE *pIntrable, NvU32 intrVector);
#ifdef __nvoc_intrable_h_disabled
static inline NV_STATUS intrableCacheAndSetPartitionNotificationIntrVector(OBJGPU *pGpu, struct OBJINTRABLE *pIntrable, NvU32 intrVector) {
    NV_ASSERT_FAILED_PRECOMP("OBJINTRABLE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intrable_h_disabled
#define intrableCacheAndSetPartitionNotificationIntrVector(pGpu, pIntrable, intrVector) intrableCacheAndSetPartitionNotificationIntrVector_IMPL(pGpu, pIntrable, intrVector)
#endif //__nvoc_intrable_h_disabled

NV_STATUS intrableSetPartitionNotificationIntrVector_IMPL(OBJGPU *pGpu, struct OBJINTRABLE *pIntrable);
#ifdef __nvoc_intrable_h_disabled
static inline NV_STATUS intrableSetPartitionNotificationIntrVector(OBJGPU *pGpu, struct OBJINTRABLE *pIntrable) {
    NV_ASSERT_FAILED_PRECOMP("OBJINTRABLE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intrable_h_disabled
#define intrableSetPartitionNotificationIntrVector(pGpu, pIntrable) intrableSetPartitionNotificationIntrVector_IMPL(pGpu, pIntrable)
#endif //__nvoc_intrable_h_disabled

NV_STATUS intrableGetPartitionNotificationIntrVector_IMPL(OBJGPU *pGpu, struct OBJINTRABLE *pIntrable);
#ifdef __nvoc_intrable_h_disabled
static inline NV_STATUS intrableGetPartitionNotificationIntrVector(OBJGPU *pGpu, struct OBJINTRABLE *pIntrable) {
    NV_ASSERT_FAILED_PRECOMP("OBJINTRABLE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intrable_h_disabled
#define intrableGetPartitionNotificationIntrVector(pGpu, pIntrable) intrableGetPartitionNotificationIntrVector_IMPL(pGpu, pIntrable)
#endif //__nvoc_intrable_h_disabled

NV_STATUS intrableRevertNotificationIntrVector_IMPL(OBJGPU *pGpu, struct OBJINTRABLE *pIntrable);
#ifdef __nvoc_intrable_h_disabled
static inline NV_STATUS intrableRevertNotificationIntrVector(OBJGPU *pGpu, struct OBJINTRABLE *pIntrable) {
    NV_ASSERT_FAILED_PRECOMP("OBJINTRABLE was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_intrable_h_disabled
#define intrableRevertNotificationIntrVector(pGpu, pIntrable) intrableRevertNotificationIntrVector_IMPL(pGpu, pIntrable)
#endif //__nvoc_intrable_h_disabled

#undef PRIVATE_FIELD


#define INTRABLE_MAX_INTR_PER_ENGINE            (1)
#define INTRABLE_MAX_INTR_PER_HOST_ENGINE       (1)

#define INTR_ROUTE_DISABLE          0
#define INTR_ROUTE_PHYSICAL         1
#define INTR_ROUTE_KERNEL           2

#endif // INTRABLE_H

#ifdef __cplusplus
} // extern "C"
#endif
#endif // _G_INTRABLE_NVOC_H_
