
#ifndef _G_KERNEL_GSPLITE_NVOC_H_
#define _G_KERNEL_GSPLITE_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_kernel_gsplite_nvoc.h"
#ifndef KERNEL_GSPLITE_H
#define KERNEL_GSPLITE_H

/******************************************************************************
*
*   Kernel GSPLITE module header
*
******************************************************************************/

#include "kernel/gpu/gpu.h"
#include "kernel/gpu/eng_state.h"
#include "kernel/gpu/mem_mgr/mem_desc.h"

#include "core/core.h"
#include "core/bin_data.h"
#include "liblogdecode.h"


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_GSPLITE_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelGsplite;
struct NVOC_METADATA__OBJENGSTATE;
struct NVOC_VTABLE__KernelGsplite;


struct KernelGsplite {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelGsplite *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct KernelGsplite *__nvoc_pbase_KernelGsplite;    // kgsplite

    // 1 PDB property

    // Data members
    NvU32 PublicId;
};


// Vtable with 14 per-class function pointers
struct NVOC_VTABLE__KernelGsplite {
    NV_STATUS (*__kgspliteConstructEngine__)(struct OBJGPU *, struct KernelGsplite * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kgspliteStateInitUnlocked__)(struct OBJGPU *, struct KernelGsplite * /*this*/);  // virtual override (engstate) base (engstate)
    void (*__kgspliteInitMissing__)(struct OBJGPU *, struct KernelGsplite * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgspliteStatePreInitLocked__)(struct OBJGPU *, struct KernelGsplite * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgspliteStatePreInitUnlocked__)(struct OBJGPU *, struct KernelGsplite * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgspliteStateInitLocked__)(struct OBJGPU *, struct KernelGsplite * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgspliteStatePreLoad__)(struct OBJGPU *, struct KernelGsplite * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgspliteStateLoad__)(struct OBJGPU *, struct KernelGsplite * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgspliteStatePostLoad__)(struct OBJGPU *, struct KernelGsplite * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgspliteStatePreUnload__)(struct OBJGPU *, struct KernelGsplite * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgspliteStateUnload__)(struct OBJGPU *, struct KernelGsplite * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kgspliteStatePostUnload__)(struct OBJGPU *, struct KernelGsplite * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    void (*__kgspliteStateDestroy__)(struct OBJGPU *, struct KernelGsplite * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__kgspliteIsPresent__)(struct OBJGPU *, struct KernelGsplite * /*this*/);  // virtual inherited (engstate) base (engstate)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelGsplite {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
    const struct NVOC_VTABLE__KernelGsplite vtable;
};

#ifndef __NVOC_CLASS_KernelGsplite_TYPEDEF__
#define __NVOC_CLASS_KernelGsplite_TYPEDEF__
typedef struct KernelGsplite KernelGsplite;
#endif /* __NVOC_CLASS_KernelGsplite_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelGsplite
#define __nvoc_class_id_KernelGsplite 0x927969
#endif /* __nvoc_class_id_KernelGsplite */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelGsplite;

#define __staticCast_KernelGsplite(pThis) \
    ((pThis)->__nvoc_pbase_KernelGsplite)

#ifdef __nvoc_kernel_gsplite_h_disabled
#define __dynamicCast_KernelGsplite(pThis) ((KernelGsplite*) NULL)
#else //__nvoc_kernel_gsplite_h_disabled
#define __dynamicCast_KernelGsplite(pThis) \
    ((KernelGsplite*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelGsplite)))
#endif //__nvoc_kernel_gsplite_h_disabled

// Property macros
#define PDB_PROP_KGSPLITE_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KGSPLITE_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelGsplite(KernelGsplite**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelGsplite(KernelGsplite**, Dynamic*, NvU32);
#define __objCreate_KernelGsplite(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelGsplite((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define kgspliteConstructEngine_FNPTR(pKernelGsplite) pKernelGsplite->__nvoc_metadata_ptr->vtable.__kgspliteConstructEngine__
#define kgspliteConstructEngine(pGpu, pKernelGsplite, engDesc) kgspliteConstructEngine_DISPATCH(pGpu, pKernelGsplite, engDesc)
#define kgspliteStateInitUnlocked_FNPTR(pKernelGsplite) pKernelGsplite->__nvoc_metadata_ptr->vtable.__kgspliteStateInitUnlocked__
#define kgspliteStateInitUnlocked(pGpu, pKernelGsplite) kgspliteStateInitUnlocked_DISPATCH(pGpu, pKernelGsplite)
#define kgspliteInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define kgspliteInitMissing(pGpu, pEngstate) kgspliteInitMissing_DISPATCH(pGpu, pEngstate)
#define kgspliteStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__
#define kgspliteStatePreInitLocked(pGpu, pEngstate) kgspliteStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kgspliteStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define kgspliteStatePreInitUnlocked(pGpu, pEngstate) kgspliteStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kgspliteStateInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__
#define kgspliteStateInitLocked(pGpu, pEngstate) kgspliteStateInitLocked_DISPATCH(pGpu, pEngstate)
#define kgspliteStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define kgspliteStatePreLoad(pGpu, pEngstate, arg3) kgspliteStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kgspliteStateLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateLoad__
#define kgspliteStateLoad(pGpu, pEngstate, arg3) kgspliteStateLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kgspliteStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__
#define kgspliteStatePostLoad(pGpu, pEngstate, arg3) kgspliteStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kgspliteStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__
#define kgspliteStatePreUnload(pGpu, pEngstate, arg3) kgspliteStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kgspliteStateUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateUnload__
#define kgspliteStateUnload(pGpu, pEngstate, arg3) kgspliteStateUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kgspliteStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define kgspliteStatePostUnload(pGpu, pEngstate, arg3) kgspliteStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kgspliteStateDestroy_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateDestroy__
#define kgspliteStateDestroy(pGpu, pEngstate) kgspliteStateDestroy_DISPATCH(pGpu, pEngstate)
#define kgspliteIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define kgspliteIsPresent(pGpu, pEngstate) kgspliteIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS kgspliteConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelGsplite *pKernelGsplite, ENGDESCRIPTOR engDesc) {
    return pKernelGsplite->__nvoc_metadata_ptr->vtable.__kgspliteConstructEngine__(pGpu, pKernelGsplite, engDesc);
}

static inline NV_STATUS kgspliteStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelGsplite *pKernelGsplite) {
    return pKernelGsplite->__nvoc_metadata_ptr->vtable.__kgspliteStateInitUnlocked__(pGpu, pKernelGsplite);
}

static inline void kgspliteInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__kgspliteInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kgspliteStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgspliteStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgspliteStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgspliteStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgspliteStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgspliteStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kgspliteStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgspliteStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kgspliteStateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgspliteStateLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kgspliteStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgspliteStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kgspliteStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgspliteStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kgspliteStateUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgspliteStateUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kgspliteStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgspliteStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline void kgspliteStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__kgspliteStateDestroy__(pGpu, pEngstate);
}

static inline NvBool kgspliteIsPresent_DISPATCH(struct OBJGPU *pGpu, struct KernelGsplite *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kgspliteIsPresent__(pGpu, pEngstate);
}

NV_STATUS kgspliteConstructEngine_IMPL(struct OBJGPU *pGpu, struct KernelGsplite *pKernelGsplite, ENGDESCRIPTOR engDesc);

NV_STATUS kgspliteStateInitUnlocked_IMPL(struct OBJGPU *pGpu, struct KernelGsplite *pKernelGsplite);

void kgspliteDestruct_IMPL(struct KernelGsplite *pKernelGsplite);

#define __nvoc_kgspliteDestruct(pKernelGsplite) kgspliteDestruct_IMPL(pKernelGsplite)
#undef PRIVATE_FIELD


#endif // KERNEL_GSPLITE_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_GSPLITE_NVOC_H_
