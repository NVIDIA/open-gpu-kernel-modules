
#ifndef _G_KERNEL_MC_NVOC_H_
#define _G_KERNEL_MC_NVOC_H_

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
#include "g_kernel_mc_nvoc.h"

#ifndef KERNEL_MC_H
#define KERNEL_MC_H

/******************************************************************************
*
*       Kernel Master Control module header
*       This file contains functions required for MC in Kernel RM
*
******************************************************************************/

#include "gpu/eng_state.h"
#include "gpu/gpu_halspec.h"

// Latency Timer Control determines how we set or dont set the PCI latency timer.
typedef struct LATENCY_TIMER_CONTROL
{
    NvBool DontModifyTimerValue;      // Dont touch the timer value at all.
    NvU32 LatencyTimerValue;        // Requested value for PCI latency timer.
} LATENCY_TIMER_CONTROL;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_MC_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelMc;
struct NVOC_METADATA__OBJENGSTATE;
struct NVOC_VTABLE__KernelMc;


struct KernelMc {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelMc *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct KernelMc *__nvoc_pbase_KernelMc;    // kmc

    // Vtable with 1 per-object function pointer
    NV_STATUS (*__kmcWritePmcEnableReg__)(struct OBJGPU *, struct KernelMc * /*this*/, NvU32, NvBool, NvBool);  // halified (3 hals) body

    // Data members
    LATENCY_TIMER_CONTROL LatencyTimerControl;
};


// Vtable with 14 per-class function pointers
struct NVOC_VTABLE__KernelMc {
    NV_STATUS (*__kmcStateInitLocked__)(struct OBJGPU *, struct KernelMc * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kmcStateLoad__)(struct OBJGPU *, struct KernelMc * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kmcConstructEngine__)(struct OBJGPU *, struct KernelMc * /*this*/, ENGDESCRIPTOR);  // virtual inherited (engstate) base (engstate)
    void (*__kmcInitMissing__)(struct OBJGPU *, struct KernelMc * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmcStatePreInitLocked__)(struct OBJGPU *, struct KernelMc * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmcStatePreInitUnlocked__)(struct OBJGPU *, struct KernelMc * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmcStateInitUnlocked__)(struct OBJGPU *, struct KernelMc * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmcStatePreLoad__)(struct OBJGPU *, struct KernelMc * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmcStatePostLoad__)(struct OBJGPU *, struct KernelMc * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmcStatePreUnload__)(struct OBJGPU *, struct KernelMc * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmcStateUnload__)(struct OBJGPU *, struct KernelMc * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kmcStatePostUnload__)(struct OBJGPU *, struct KernelMc * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    void (*__kmcStateDestroy__)(struct OBJGPU *, struct KernelMc * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__kmcIsPresent__)(struct OBJGPU *, struct KernelMc * /*this*/);  // virtual inherited (engstate) base (engstate)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelMc {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
    const struct NVOC_VTABLE__KernelMc vtable;
};

#ifndef __NVOC_CLASS_KernelMc_TYPEDEF__
#define __NVOC_CLASS_KernelMc_TYPEDEF__
typedef struct KernelMc KernelMc;
#endif /* __NVOC_CLASS_KernelMc_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelMc
#define __nvoc_class_id_KernelMc 0x3827ff
#endif /* __nvoc_class_id_KernelMc */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelMc;

#define __staticCast_KernelMc(pThis) \
    ((pThis)->__nvoc_pbase_KernelMc)

#ifdef __nvoc_kernel_mc_h_disabled
#define __dynamicCast_KernelMc(pThis) ((KernelMc*) NULL)
#else //__nvoc_kernel_mc_h_disabled
#define __dynamicCast_KernelMc(pThis) \
    ((KernelMc*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelMc)))
#endif //__nvoc_kernel_mc_h_disabled

// Property macros
#define PDB_PROP_KMC_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KMC_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelMc(KernelMc**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelMc(KernelMc**, Dynamic*, NvU32);
#define __objCreate_KernelMc(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelMc((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define kmcStateInitLocked_FNPTR(pKernelMc) pKernelMc->__nvoc_metadata_ptr->vtable.__kmcStateInitLocked__
#define kmcStateInitLocked(pGpu, pKernelMc) kmcStateInitLocked_DISPATCH(pGpu, pKernelMc)
#define kmcStateLoad_FNPTR(pKernelMc) pKernelMc->__nvoc_metadata_ptr->vtable.__kmcStateLoad__
#define kmcStateLoad(pGpu, pKernelMc, arg3) kmcStateLoad_DISPATCH(pGpu, pKernelMc, arg3)
#define kmcWritePmcEnableReg_FNPTR(pKernelMc) pKernelMc->__kmcWritePmcEnableReg__
#define kmcWritePmcEnableReg(pGpu, pKernelMc, arg3, arg4, arg5) kmcWritePmcEnableReg_DISPATCH(pGpu, pKernelMc, arg3, arg4, arg5)
#define kmcWritePmcEnableReg_HAL(pGpu, pKernelMc, arg3, arg4, arg5) kmcWritePmcEnableReg_DISPATCH(pGpu, pKernelMc, arg3, arg4, arg5)
#define kmcConstructEngine_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructEngine__
#define kmcConstructEngine(pGpu, pEngstate, arg3) kmcConstructEngine_DISPATCH(pGpu, pEngstate, arg3)
#define kmcInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define kmcInitMissing(pGpu, pEngstate) kmcInitMissing_DISPATCH(pGpu, pEngstate)
#define kmcStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__
#define kmcStatePreInitLocked(pGpu, pEngstate) kmcStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kmcStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define kmcStatePreInitUnlocked(pGpu, pEngstate) kmcStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kmcStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define kmcStateInitUnlocked(pGpu, pEngstate) kmcStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kmcStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define kmcStatePreLoad(pGpu, pEngstate, arg3) kmcStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kmcStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__
#define kmcStatePostLoad(pGpu, pEngstate, arg3) kmcStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kmcStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__
#define kmcStatePreUnload(pGpu, pEngstate, arg3) kmcStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kmcStateUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateUnload__
#define kmcStateUnload(pGpu, pEngstate, arg3) kmcStateUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kmcStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define kmcStatePostUnload(pGpu, pEngstate, arg3) kmcStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kmcStateDestroy_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateDestroy__
#define kmcStateDestroy(pGpu, pEngstate) kmcStateDestroy_DISPATCH(pGpu, pEngstate)
#define kmcIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define kmcIsPresent(pGpu, pEngstate) kmcIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS kmcStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pKernelMc) {
    return pKernelMc->__nvoc_metadata_ptr->vtable.__kmcStateInitLocked__(pGpu, pKernelMc);
}

static inline NV_STATUS kmcStateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvU32 arg3) {
    return pKernelMc->__nvoc_metadata_ptr->vtable.__kmcStateLoad__(pGpu, pKernelMc, arg3);
}

static inline NV_STATUS kmcWritePmcEnableReg_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvU32 arg3, NvBool arg4, NvBool arg5) {
    return pKernelMc->__kmcWritePmcEnableReg__(pGpu, pKernelMc, arg3, arg4, arg5);
}

static inline NV_STATUS kmcConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate, ENGDESCRIPTOR arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kmcConstructEngine__(pGpu, pEngstate, arg3);
}

static inline void kmcInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__kmcInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kmcStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kmcStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kmcStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kmcStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kmcStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kmcStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kmcStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kmcStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kmcStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kmcStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kmcStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kmcStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kmcStateUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kmcStateUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kmcStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kmcStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline void kmcStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__kmcStateDestroy__(pGpu, pEngstate);
}

static inline NvBool kmcIsPresent_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kmcIsPresent__(pGpu, pEngstate);
}

NV_STATUS kmcPrepareForXVEReset_GM107(struct OBJGPU *pGpu, struct KernelMc *pKernelMc);


#ifdef __nvoc_kernel_mc_h_disabled
static inline NV_STATUS kmcPrepareForXVEReset(struct OBJGPU *pGpu, struct KernelMc *pKernelMc) {
    NV_ASSERT_FAILED_PRECOMP("KernelMc was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mc_h_disabled
#define kmcPrepareForXVEReset(pGpu, pKernelMc) kmcPrepareForXVEReset_GM107(pGpu, pKernelMc)
#endif //__nvoc_kernel_mc_h_disabled

#define kmcPrepareForXVEReset_HAL(pGpu, pKernelMc) kmcPrepareForXVEReset(pGpu, pKernelMc)

NV_STATUS kmcGetMcBar0MapInfo_GM107(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvU64 *arg3, NvU32 *arg4);


#ifdef __nvoc_kernel_mc_h_disabled
static inline NV_STATUS kmcGetMcBar0MapInfo(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvU64 *arg3, NvU32 *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelMc was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_mc_h_disabled
#define kmcGetMcBar0MapInfo(pGpu, pKernelMc, arg3, arg4) kmcGetMcBar0MapInfo_GM107(pGpu, pKernelMc, arg3, arg4)
#endif //__nvoc_kernel_mc_h_disabled

#define kmcGetMcBar0MapInfo_HAL(pGpu, pKernelMc, arg3, arg4) kmcGetMcBar0MapInfo(pGpu, pKernelMc, arg3, arg4)

NV_STATUS kmcStateInitLocked_IMPL(struct OBJGPU *pGpu, struct KernelMc *pKernelMc);

NV_STATUS kmcStateLoad_IMPL(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvU32 arg3);

NV_STATUS kmcWritePmcEnableReg_GM107(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvU32 arg3, NvBool arg4, NvBool arg5);

NV_STATUS kmcWritePmcEnableReg_GA100(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvU32 arg3, NvBool arg4, NvBool arg5);

static inline NV_STATUS kmcWritePmcEnableReg_5baef9(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvU32 arg3, NvBool arg4, NvBool arg5) {
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

#undef PRIVATE_FIELD


#endif // KERNEL_MC_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_MC_NVOC_H_
