
#ifndef _G_INTR_SERVICE_NVOC_H_
#define _G_INTR_SERVICE_NVOC_H_
#include "nvoc/runtime.h"

// Version of generated metadata structures
#ifdef NVOC_METADATA_VERSION
#undef NVOC_METADATA_VERSION
#endif
#define NVOC_METADATA_VERSION 0

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

#pragma once
#include "g_intr_service_nvoc.h"

#ifndef INTR_SERVICE_H
#define INTR_SERVICE_H

/*!
 * @brief  Provides definitions for IntrService class,
 * which is an interface for classes which can handle interrupts.
 */

#include "core/core.h"
#include "core/thread_state.h"
#include "gpu/gpu.h"
#include "kernel/gpu/intr/engine_idx.h"


struct IntrService;

#ifndef __NVOC_CLASS_IntrService_TYPEDEF__
#define __NVOC_CLASS_IntrService_TYPEDEF__
typedef struct IntrService IntrService;
#endif /* __NVOC_CLASS_IntrService_TYPEDEF__ */

#ifndef __nvoc_class_id_IntrService
#define __nvoc_class_id_IntrService 0x2271cc
#endif /* __nvoc_class_id_IntrService */



typedef struct {
    struct IntrService *pInterruptService;
    struct IntrService *pNotificationService;
    NvBool bFifoWaiveNotify;
} IntrServiceRecord;

typedef struct {
    NvU16 engineIdx;
} IntrServiceClearInterruptArguments;

typedef struct {
    NvU16 engineIdx;
} IntrServiceServiceInterruptArguments;

typedef struct {
    THREAD_STATE_NODE *pThreadState;
    NvU16              engineIdx;
} IntrServiceServiceNotificationInterruptArguments;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_INTR_SERVICE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct IntrService {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Ancestor object pointers for `staticCast` feature
    struct IntrService *__nvoc_pbase_IntrService;    // intrserv

    // Vtable with 4 per-object function pointers
    void (*__intrservRegisterIntrService__)(struct OBJGPU *, struct IntrService * /*this*/, IntrServiceRecord *);  // virtual
    NvBool (*__intrservClearInterrupt__)(struct OBJGPU *, struct IntrService * /*this*/, IntrServiceClearInterruptArguments *);  // virtual
    NvU32 (*__intrservServiceInterrupt__)(struct OBJGPU *, struct IntrService * /*this*/, IntrServiceServiceInterruptArguments *);  // virtual
    NV_STATUS (*__intrservServiceNotificationInterrupt__)(struct OBJGPU *, struct IntrService * /*this*/, IntrServiceServiceNotificationInterruptArguments *);  // virtual
};

#ifndef __NVOC_CLASS_IntrService_TYPEDEF__
#define __NVOC_CLASS_IntrService_TYPEDEF__
typedef struct IntrService IntrService;
#endif /* __NVOC_CLASS_IntrService_TYPEDEF__ */

#ifndef __nvoc_class_id_IntrService
#define __nvoc_class_id_IntrService 0x2271cc
#endif /* __nvoc_class_id_IntrService */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_IntrService;

#define __staticCast_IntrService(pThis) \
    ((pThis)->__nvoc_pbase_IntrService)

#ifdef __nvoc_intr_service_h_disabled
#define __dynamicCast_IntrService(pThis) ((IntrService*)NULL)
#else //__nvoc_intr_service_h_disabled
#define __dynamicCast_IntrService(pThis) \
    ((IntrService*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(IntrService)))
#endif //__nvoc_intr_service_h_disabled

NV_STATUS __nvoc_objCreateDynamic_IntrService(IntrService**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_IntrService(IntrService**, Dynamic*, NvU32);
#define __objCreate_IntrService(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_IntrService((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define intrservRegisterIntrService_FNPTR(pIntrService) pIntrService->__intrservRegisterIntrService__
#define intrservRegisterIntrService(pGpu, pIntrService, pRecords) intrservRegisterIntrService_DISPATCH(pGpu, pIntrService, pRecords)
#define intrservClearInterrupt_FNPTR(pIntrService) pIntrService->__intrservClearInterrupt__
#define intrservClearInterrupt(pGpu, pIntrService, pParams) intrservClearInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define intrservServiceInterrupt_FNPTR(pIntrService) pIntrService->__intrservServiceInterrupt__
#define intrservServiceInterrupt(pGpu, pIntrService, pParams) intrservServiceInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define intrservServiceNotificationInterrupt_FNPTR(pIntrService) pIntrService->__intrservServiceNotificationInterrupt__
#define intrservServiceNotificationInterrupt(pGpu, pIntrService, pParams) intrservServiceNotificationInterrupt_DISPATCH(pGpu, pIntrService, pParams)

// Dispatch functions
static inline void intrservRegisterIntrService_DISPATCH(struct OBJGPU *pGpu, struct IntrService *pIntrService, IntrServiceRecord pRecords[175]) {
    pIntrService->__intrservRegisterIntrService__(pGpu, pIntrService, pRecords);
}

static inline NvBool intrservClearInterrupt_DISPATCH(struct OBJGPU *pGpu, struct IntrService *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return pIntrService->__intrservClearInterrupt__(pGpu, pIntrService, pParams);
}

static inline NvU32 intrservServiceInterrupt_DISPATCH(struct OBJGPU *pGpu, struct IntrService *pIntrService, IntrServiceServiceInterruptArguments *pParams) {
    return pIntrService->__intrservServiceInterrupt__(pGpu, pIntrService, pParams);
}

static inline NV_STATUS intrservServiceNotificationInterrupt_DISPATCH(struct OBJGPU *pGpu, struct IntrService *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return pIntrService->__intrservServiceNotificationInterrupt__(pGpu, pIntrService, pParams);
}

void intrservRegisterIntrService_IMPL(struct OBJGPU *pGpu, struct IntrService *pIntrService, IntrServiceRecord pRecords[175]);

NvBool intrservClearInterrupt_IMPL(struct OBJGPU *pGpu, struct IntrService *pIntrService, IntrServiceClearInterruptArguments *pParams);

NvU32 intrservServiceInterrupt_IMPL(struct OBJGPU *pGpu, struct IntrService *pIntrService, IntrServiceServiceInterruptArguments *pParams);

NV_STATUS intrservServiceNotificationInterrupt_IMPL(struct OBJGPU *pGpu, struct IntrService *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams);

#undef PRIVATE_FIELD


#endif // INTR_SERVICE_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_INTR_SERVICE_NVOC_H_
