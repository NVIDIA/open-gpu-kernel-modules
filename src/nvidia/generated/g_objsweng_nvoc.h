
#ifndef _G_OBJSWENG_NVOC_H_
#define _G_OBJSWENG_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/***************************** Modular includes *****************************\
*                                                                           *
*   OBJSWENG implements the 'Software Engine' from the GPU host             *
*   perspective. That is the software component that emulates a GPU host    *
*   engine by implementing SW methods in the driver.                        *
*                                                                           *
****************************************************************************/

#pragma once
#include "g_objsweng_nvoc.h"

#ifndef _OBJSWENG_H_
#define _OBJSWENG_H_

#include "core/core.h"
#include "gpu/eng_state.h"


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_OBJSWENG_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__OBJSWENG;
struct NVOC_METADATA__OBJENGSTATE;
struct NVOC_VTABLE__OBJSWENG;


struct OBJSWENG {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__OBJSWENG *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct OBJSWENG *__nvoc_pbase_OBJSWENG;    // sweng
};


// Vtable with 14 per-class function pointers
struct NVOC_VTABLE__OBJSWENG {
    NV_STATUS (*__swengConstructEngine__)(struct OBJGPU *, struct OBJSWENG * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    void (*__swengInitMissing__)(struct OBJGPU *, struct OBJSWENG * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__swengStatePreInitLocked__)(struct OBJGPU *, struct OBJSWENG * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__swengStatePreInitUnlocked__)(struct OBJGPU *, struct OBJSWENG * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__swengStateInitLocked__)(struct OBJGPU *, struct OBJSWENG * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__swengStateInitUnlocked__)(struct OBJGPU *, struct OBJSWENG * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__swengStatePreLoad__)(struct OBJGPU *, struct OBJSWENG * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__swengStateLoad__)(struct OBJGPU *, struct OBJSWENG * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__swengStatePostLoad__)(struct OBJGPU *, struct OBJSWENG * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__swengStatePreUnload__)(struct OBJGPU *, struct OBJSWENG * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__swengStateUnload__)(struct OBJGPU *, struct OBJSWENG * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__swengStatePostUnload__)(struct OBJGPU *, struct OBJSWENG * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    void (*__swengStateDestroy__)(struct OBJGPU *, struct OBJSWENG * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__swengIsPresent__)(struct OBJGPU *, struct OBJSWENG * /*this*/);  // virtual inherited (engstate) base (engstate)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__OBJSWENG {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
    const struct NVOC_VTABLE__OBJSWENG vtable;
};

#ifndef __NVOC_CLASS_OBJSWENG_TYPEDEF__
#define __NVOC_CLASS_OBJSWENG_TYPEDEF__
typedef struct OBJSWENG OBJSWENG;
#endif /* __NVOC_CLASS_OBJSWENG_TYPEDEF__ */

#ifndef __nvoc_class_id_OBJSWENG
#define __nvoc_class_id_OBJSWENG 0x95a6f5
#endif /* __nvoc_class_id_OBJSWENG */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_OBJSWENG;

#define __staticCast_OBJSWENG(pThis) \
    ((pThis)->__nvoc_pbase_OBJSWENG)

#ifdef __nvoc_objsweng_h_disabled
#define __dynamicCast_OBJSWENG(pThis) ((OBJSWENG*) NULL)
#else //__nvoc_objsweng_h_disabled
#define __dynamicCast_OBJSWENG(pThis) \
    ((OBJSWENG*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(OBJSWENG)))
#endif //__nvoc_objsweng_h_disabled

// Property macros
#define PDB_PROP_SWENG_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_SWENG_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_OBJSWENG(OBJSWENG**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_OBJSWENG(OBJSWENG**, Dynamic*, NvU32);
#define __objCreate_OBJSWENG(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_OBJSWENG((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define swengConstructEngine_FNPTR(pSweng) pSweng->__nvoc_metadata_ptr->vtable.__swengConstructEngine__
#define swengConstructEngine(pGpu, pSweng, arg3) swengConstructEngine_DISPATCH(pGpu, pSweng, arg3)
#define swengInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define swengInitMissing(pGpu, pEngstate) swengInitMissing_DISPATCH(pGpu, pEngstate)
#define swengStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__
#define swengStatePreInitLocked(pGpu, pEngstate) swengStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define swengStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define swengStatePreInitUnlocked(pGpu, pEngstate) swengStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define swengStateInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__
#define swengStateInitLocked(pGpu, pEngstate) swengStateInitLocked_DISPATCH(pGpu, pEngstate)
#define swengStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define swengStateInitUnlocked(pGpu, pEngstate) swengStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define swengStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define swengStatePreLoad(pGpu, pEngstate, arg3) swengStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define swengStateLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateLoad__
#define swengStateLoad(pGpu, pEngstate, arg3) swengStateLoad_DISPATCH(pGpu, pEngstate, arg3)
#define swengStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__
#define swengStatePostLoad(pGpu, pEngstate, arg3) swengStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define swengStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__
#define swengStatePreUnload(pGpu, pEngstate, arg3) swengStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define swengStateUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateUnload__
#define swengStateUnload(pGpu, pEngstate, arg3) swengStateUnload_DISPATCH(pGpu, pEngstate, arg3)
#define swengStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define swengStatePostUnload(pGpu, pEngstate, arg3) swengStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define swengStateDestroy_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateDestroy__
#define swengStateDestroy(pGpu, pEngstate) swengStateDestroy_DISPATCH(pGpu, pEngstate)
#define swengIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define swengIsPresent(pGpu, pEngstate) swengIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS swengConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct OBJSWENG *pSweng, ENGDESCRIPTOR arg3) {
    return pSweng->__nvoc_metadata_ptr->vtable.__swengConstructEngine__(pGpu, pSweng, arg3);
}

static inline void swengInitMissing_DISPATCH(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__swengInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS swengStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swengStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS swengStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swengStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS swengStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swengStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS swengStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swengStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS swengStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swengStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS swengStateLoad_DISPATCH(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swengStateLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS swengStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swengStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS swengStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swengStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS swengStateUnload_DISPATCH(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swengStateUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS swengStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swengStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline void swengStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__swengStateDestroy__(pGpu, pEngstate);
}

static inline NvBool swengIsPresent_DISPATCH(struct OBJGPU *pGpu, struct OBJSWENG *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__swengIsPresent__(pGpu, pEngstate);
}

NV_STATUS swengConstructEngine_IMPL(struct OBJGPU *pGpu, struct OBJSWENG *pSweng, ENGDESCRIPTOR arg3);

#undef PRIVATE_FIELD


#endif // _OBJSWENG_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_OBJSWENG_NVOC_H_
