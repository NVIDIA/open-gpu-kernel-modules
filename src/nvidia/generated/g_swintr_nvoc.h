
#ifndef _G_SWINTR_NVOC_H_
#define _G_SWINTR_NVOC_H_

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

#pragma once
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


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_SWINTR_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__SwIntr;
struct NVOC_METADATA__OBJENGSTATE;
struct NVOC_METADATA__IntrService;
struct NVOC_VTABLE__SwIntr;


struct SwIntr {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__SwIntr *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct IntrService __nvoc_base_IntrService;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct IntrService *__nvoc_pbase_IntrService;    // intrserv super
    struct SwIntr *__nvoc_pbase_SwIntr;    // swintr
};


// Vtable with 18 per-class function pointers
struct NVOC_VTABLE__SwIntr {
    void (*__swintrRegisterIntrService__)(OBJGPU *, struct SwIntr * /*this*/, IntrServiceRecord *);  // virtual override (intrserv) base (intrserv)
    NvU32 (*__swintrServiceInterrupt__)(OBJGPU *, struct SwIntr * /*this*/, IntrServiceServiceInterruptArguments *);  // virtual override (intrserv) base (intrserv)
    NV_STATUS (*__swintrConstructEngine__)(struct OBJGPU *, struct SwIntr * /*this*/, ENGDESCRIPTOR);  // virtual inherited (engstate) base (engstate)
    void (*__swintrInitMissing__)(struct OBJGPU *, struct SwIntr * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__swintrStatePreInitLocked__)(struct OBJGPU *, struct SwIntr * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__swintrStatePreInitUnlocked__)(struct OBJGPU *, struct SwIntr * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__swintrStateInitLocked__)(struct OBJGPU *, struct SwIntr * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__swintrStateInitUnlocked__)(struct OBJGPU *, struct SwIntr * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__swintrStatePreLoad__)(struct OBJGPU *, struct SwIntr * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__swintrStateLoad__)(struct OBJGPU *, struct SwIntr * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__swintrStatePostLoad__)(struct OBJGPU *, struct SwIntr * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__swintrStatePreUnload__)(struct OBJGPU *, struct SwIntr * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__swintrStateUnload__)(struct OBJGPU *, struct SwIntr * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__swintrStatePostUnload__)(struct OBJGPU *, struct SwIntr * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    void (*__swintrStateDestroy__)(struct OBJGPU *, struct SwIntr * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__swintrIsPresent__)(struct OBJGPU *, struct SwIntr * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__swintrClearInterrupt__)(OBJGPU *, struct SwIntr * /*this*/, IntrServiceClearInterruptArguments *);  // virtual inherited (intrserv) base (intrserv)
    NV_STATUS (*__swintrServiceNotificationInterrupt__)(OBJGPU *, struct SwIntr * /*this*/, IntrServiceServiceNotificationInterruptArguments *);  // virtual inherited (intrserv) base (intrserv)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__SwIntr {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
    const struct NVOC_METADATA__IntrService metadata__IntrService;
    const struct NVOC_VTABLE__SwIntr vtable;
};

#ifndef __NVOC_CLASS_SwIntr_TYPEDEF__
#define __NVOC_CLASS_SwIntr_TYPEDEF__
typedef struct SwIntr SwIntr;
#endif /* __NVOC_CLASS_SwIntr_TYPEDEF__ */

#ifndef __nvoc_class_id_SwIntr
#define __nvoc_class_id_SwIntr 0x5ca633
#endif /* __nvoc_class_id_SwIntr */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_SwIntr;

#define __staticCast_SwIntr(pThis) \
    ((pThis)->__nvoc_pbase_SwIntr)

#ifdef __nvoc_swintr_h_disabled
#define __dynamicCast_SwIntr(pThis) ((SwIntr*) NULL)
#else //__nvoc_swintr_h_disabled
#define __dynamicCast_SwIntr(pThis) \
    ((SwIntr*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(SwIntr)))
#endif //__nvoc_swintr_h_disabled

// Property macros
#define PDB_PROP_SWINTR_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_SWINTR_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_SwIntr(SwIntr**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_SwIntr(SwIntr**, Dynamic*, NvU32);
#define __objCreate_SwIntr(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_SwIntr((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define swintrRegisterIntrService_FNPTR(pSwIntr) pSwIntr->__nvoc_metadata_ptr->vtable.__swintrRegisterIntrService__
#define swintrRegisterIntrService(pGpu, pSwIntr, pRecords) swintrRegisterIntrService_DISPATCH(pGpu, pSwIntr, pRecords)
#define swintrServiceInterrupt_FNPTR(pSwIntr) pSwIntr->__nvoc_metadata_ptr->vtable.__swintrServiceInterrupt__
#define swintrServiceInterrupt(pGpu, pSwIntr, pParams) swintrServiceInterrupt_DISPATCH(pGpu, pSwIntr, pParams)
#define swintrConstructEngine_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructEngine__
#define swintrConstructEngine(pGpu, pEngstate, arg3) swintrConstructEngine_DISPATCH(pGpu, pEngstate, arg3)
#define swintrInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define swintrInitMissing(pGpu, pEngstate) swintrInitMissing_DISPATCH(pGpu, pEngstate)
#define swintrStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__
#define swintrStatePreInitLocked(pGpu, pEngstate) swintrStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define swintrStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define swintrStatePreInitUnlocked(pGpu, pEngstate) swintrStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define swintrStateInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__
#define swintrStateInitLocked(pGpu, pEngstate) swintrStateInitLocked_DISPATCH(pGpu, pEngstate)
#define swintrStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define swintrStateInitUnlocked(pGpu, pEngstate) swintrStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define swintrStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define swintrStatePreLoad(pGpu, pEngstate, arg3) swintrStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define swintrStateLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateLoad__
#define swintrStateLoad(pGpu, pEngstate, arg3) swintrStateLoad_DISPATCH(pGpu, pEngstate, arg3)
#define swintrStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__
#define swintrStatePostLoad(pGpu, pEngstate, arg3) swintrStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define swintrStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__
#define swintrStatePreUnload(pGpu, pEngstate, arg3) swintrStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define swintrStateUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateUnload__
#define swintrStateUnload(pGpu, pEngstate, arg3) swintrStateUnload_DISPATCH(pGpu, pEngstate, arg3)
#define swintrStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define swintrStatePostUnload(pGpu, pEngstate, arg3) swintrStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define swintrStateDestroy_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateDestroy__
#define swintrStateDestroy(pGpu, pEngstate) swintrStateDestroy_DISPATCH(pGpu, pEngstate)
#define swintrIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define swintrIsPresent(pGpu, pEngstate) swintrIsPresent_DISPATCH(pGpu, pEngstate)
#define swintrClearInterrupt_FNPTR(pIntrService) pIntrService->__nvoc_base_IntrService.__nvoc_metadata_ptr->vtable.__intrservClearInterrupt__
#define swintrClearInterrupt(pGpu, pIntrService, pParams) swintrClearInterrupt_DISPATCH(pGpu, pIntrService, pParams)
#define swintrServiceNotificationInterrupt_FNPTR(pIntrService) pIntrService->__nvoc_base_IntrService.__nvoc_metadata_ptr->vtable.__intrservServiceNotificationInterrupt__
#define swintrServiceNotificationInterrupt(pGpu, pIntrService, pParams) swintrServiceNotificationInterrupt_DISPATCH(pGpu, pIntrService, pParams)

// Dispatch functions
static inline void swintrRegisterIntrService_DISPATCH(OBJGPU *pGpu, struct SwIntr *pSwIntr, IntrServiceRecord pRecords[179]) {
    pSwIntr->__nvoc_metadata_ptr->vtable.__swintrRegisterIntrService__(pGpu, pSwIntr, pRecords);
}

static inline NvU32 swintrServiceInterrupt_DISPATCH(OBJGPU *pGpu, struct SwIntr *pSwIntr, IntrServiceServiceInterruptArguments *pParams) {
    return pSwIntr->__nvoc_metadata_ptr->vtable.__swintrServiceInterrupt__(pGpu, pSwIntr, pParams);
}

static inline NV_STATUS swintrConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct SwIntr *pEngstate, ENGDESCRIPTOR arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swintrConstructEngine__(pGpu, pEngstate, arg3);
}

static inline void swintrInitMissing_DISPATCH(struct OBJGPU *pGpu, struct SwIntr *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__swintrInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS swintrStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct SwIntr *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swintrStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS swintrStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct SwIntr *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swintrStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS swintrStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct SwIntr *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swintrStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS swintrStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct SwIntr *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swintrStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS swintrStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct SwIntr *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swintrStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS swintrStateLoad_DISPATCH(struct OBJGPU *pGpu, struct SwIntr *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swintrStateLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS swintrStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct SwIntr *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swintrStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS swintrStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct SwIntr *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swintrStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS swintrStateUnload_DISPATCH(struct OBJGPU *pGpu, struct SwIntr *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swintrStateUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS swintrStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct SwIntr *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swintrStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline void swintrStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct SwIntr *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__swintrStateDestroy__(pGpu, pEngstate);
}

static inline NvBool swintrIsPresent_DISPATCH(struct OBJGPU *pGpu, struct SwIntr *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swintrIsPresent__(pGpu, pEngstate);
}

static inline NvBool swintrClearInterrupt_DISPATCH(OBJGPU *pGpu, struct SwIntr *pIntrService, IntrServiceClearInterruptArguments *pParams) {
    return pIntrService->__nvoc_metadata_ptr->vtable.__swintrClearInterrupt__(pGpu, pIntrService, pParams);
}

static inline NV_STATUS swintrServiceNotificationInterrupt_DISPATCH(OBJGPU *pGpu, struct SwIntr *pIntrService, IntrServiceServiceNotificationInterruptArguments *pParams) {
    return pIntrService->__nvoc_metadata_ptr->vtable.__swintrServiceNotificationInterrupt__(pGpu, pIntrService, pParams);
}

void swintrRegisterIntrService_IMPL(OBJGPU *pGpu, struct SwIntr *pSwIntr, IntrServiceRecord pRecords[179]);

NvU32 swintrServiceInterrupt_IMPL(OBJGPU *pGpu, struct SwIntr *pSwIntr, IntrServiceServiceInterruptArguments *pParams);

#undef PRIVATE_FIELD


#endif // SWINTR_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_SWINTR_NVOC_H_
