
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
 * SPDX-FileCopyrightText: Copyright (c) 2021-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelMc;
struct NVOC_METADATA__OBJENGSTATE;


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

    // Vtable with 2 per-object function pointers
    NV_STATUS (*__kmcWritePmcEnableReg__)(struct OBJGPU *, struct KernelMc * /*this*/, NvU32, NvBool, NvBool);  // halified (3 hals) body
    NV_STATUS (*__kmcPrepareForXVEReset__)(struct OBJGPU *, struct KernelMc * /*this*/);  // halified (2 hals) body

    // 1 PDB property
//  NvBool PDB_PROP_KMC_IS_MISSING inherited from OBJENGSTATE

    // Data members
    LATENCY_TIMER_CONTROL LatencyTimerControl;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelMc {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
};

#ifndef __nvoc_class_id_KernelMc
#define __nvoc_class_id_KernelMc 0x3827ffu
typedef struct KernelMc KernelMc;
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


NV_STATUS __nvoc_objCreateDynamic_KernelMc(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelMc(KernelMc**, Dynamic*, NvU32);
#define __objCreate_KernelMc(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags) \
    __nvoc_objCreate_KernelMc((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags))


// Wrapper macros for implementation functions

// Wrapper macros for halified functions
#define kmcStateInitLocked_FNPTR(pKernelMc) pKernelMc->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__
#define kmcStateInitLocked(pGpu, pKernelMc) kmcStateInitLocked_DISPATCH(pGpu, pKernelMc)
#define kmcStateLoad_FNPTR(pKernelMc) pKernelMc->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateLoad__
#define kmcStateLoad(pGpu, pKernelMc, arg3) kmcStateLoad_DISPATCH(pGpu, pKernelMc, arg3)
#define kmcWritePmcEnableReg_FNPTR(pKernelMc) pKernelMc->__kmcWritePmcEnableReg__
#define kmcWritePmcEnableReg(pGpu, pKernelMc, arg3, arg4, arg5) kmcWritePmcEnableReg_DISPATCH(pGpu, pKernelMc, arg3, arg4, arg5)
#define kmcWritePmcEnableReg_HAL(pGpu, pKernelMc, arg3, arg4, arg5) kmcWritePmcEnableReg_DISPATCH(pGpu, pKernelMc, arg3, arg4, arg5)
#define kmcPrepareForXVEReset_FNPTR(pKernelMc) pKernelMc->__kmcPrepareForXVEReset__
#define kmcPrepareForXVEReset(pGpu, pKernelMc) kmcPrepareForXVEReset_DISPATCH(pGpu, pKernelMc)
#define kmcPrepareForXVEReset_HAL(pGpu, pKernelMc) kmcPrepareForXVEReset_DISPATCH(pGpu, pKernelMc)
#define kmcConstructEngine_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructEngine__
#define kmcConstructEngine(pGpu, pEngstate, arg3) kmcConstructEngine_DISPATCH(pGpu, pEngstate, arg3)
#define kmcConstructApertures_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructApertures__
#define kmcConstructApertures(pGpu, pEngstate) kmcConstructApertures_DISPATCH(pGpu, pEngstate)
#define kmcDestructApertures_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateDestructApertures__
#define kmcDestructApertures(pGpu, pEngstate) kmcDestructApertures_DISPATCH(pGpu, pEngstate)
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
    return pKernelMc->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__(pGpu, staticCast(pKernelMc, OBJENGSTATE));
}

static inline NV_STATUS kmcStateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvU32 arg3) {
    return pKernelMc->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateLoad__(pGpu, staticCast(pKernelMc, OBJENGSTATE), arg3);
}

static inline NV_STATUS kmcWritePmcEnableReg_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvU32 arg3, NvBool arg4, NvBool arg5) {
    return pKernelMc->__kmcWritePmcEnableReg__(pGpu, pKernelMc, arg3, arg4, arg5);
}

static inline NV_STATUS kmcPrepareForXVEReset_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pKernelMc) {
    return pKernelMc->__kmcPrepareForXVEReset__(pGpu, pKernelMc);
}

static inline NV_STATUS kmcConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate, ENGDESCRIPTOR arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructEngine__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS kmcConstructApertures_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructApertures__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline void kmcDestructApertures_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate) {
    pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateDestructApertures__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline void kmcInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate) {
    pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS kmcStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS kmcStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS kmcStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS kmcStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS kmcStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS kmcStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS kmcStateUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateUnload__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS kmcStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline void kmcStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate) {
    pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateDestroy__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NvBool kmcIsPresent_DISPATCH(struct OBJGPU *pGpu, struct KernelMc *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

// Virtual method declarations and/or inline definitions
NV_STATUS kmcStateInitLocked_IMPL(struct OBJGPU *pGpu, struct KernelMc *pKernelMc);

NV_STATUS kmcStateLoad_IMPL(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvU32 arg3);

// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
NV_STATUS kmcWritePmcEnableReg_GM107(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvU32 arg3, NvBool arg4, NvBool arg5);

NV_STATUS kmcWritePmcEnableReg_GA100(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvU32 arg3, NvBool arg4, NvBool arg5);

NV_STATUS kmcPrepareForXVEReset_GM107(struct OBJGPU *pGpu, struct KernelMc *pKernelMc);

// Inline HAL method definitions
static inline NV_STATUS kmcWritePmcEnableReg_5baef9(struct OBJGPU *pGpu, struct KernelMc *pKernelMc, NvU32 arg3, NvBool arg4, NvBool arg5){
    NV_ASSERT_OR_RETURN_PRECOMP(0, NV_ERR_NOT_SUPPORTED);
}

static inline NV_STATUS kmcPrepareForXVEReset_ac1694(struct OBJGPU *pGpu, struct KernelMc *pKernelMc){
    return NV_OK;
}

// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // KERNEL_MC_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_MC_NVOC_H_
