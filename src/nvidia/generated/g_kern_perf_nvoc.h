
#ifndef _G_KERN_PERF_NVOC_H_
#define _G_KERN_PERF_NVOC_H_
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
#include "g_kern_perf_nvoc.h"

#ifndef KERNEL_PERF_H
#define KERNEL_PERF_H

/******************************************************************************
*
*       Kernel Perf module header
*       This file contains functions managing performance on CPU RM
*
******************************************************************************/
/* ------------------------ Includes --------------------------------------- */
#include "gpu/gpu.h"
#include "gpu/eng_state.h"
#include "gpu/gpu_halspec.h"
#include "gpu/perf/kern_perf_gpuboostsync.h"
#include "ctrl/ctrl2080/ctrl2080perf.h"

/* ----------------------------- Macros ------------------------------------- */
/*!
 * The rule of reentrancy is that routine can't run unless its flag and all lower
 * flags are clear. This is Kernel Perf reentrancy function ID for 1HZ Callback.
 */
#define KERNEL_PERF_REENTRANCY_TIMER_1HZ_CALLBACK    NVBIT(0)

/* -------------------------- Datatypes ------------------------------------- */
/*!
 * KernelPerf is a logical abstraction of the GPU Perf Engine. The
 * Public API of the Perf Engine is exposed through this object, and any
 * interfaces which do not manage the underlying Perf hardware can be
 * managed by this object.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERN_PERF_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct KernelPerf {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct KernelPerf *__nvoc_pbase_KernelPerf;    // kperf

    // Vtable with 15 per-object function pointers
    NV_STATUS (*__kperfConstructEngine__)(struct OBJGPU *, struct KernelPerf * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kperfStateInitLocked__)(struct OBJGPU *, struct KernelPerf * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kperfStateLoad__)(struct OBJGPU *, struct KernelPerf * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kperfStateUnload__)(struct OBJGPU *, struct KernelPerf * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    void (*__kperfStateDestroy__)(struct OBJGPU *, struct KernelPerf * /*this*/);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kperfGpuBoostSyncStateInit__)(struct OBJGPU *, struct KernelPerf * /*this*/);  // halified (2 hals) body
    void (*__kperfInitMissing__)(struct OBJGPU *, struct KernelPerf * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kperfStatePreInitLocked__)(struct OBJGPU *, struct KernelPerf * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kperfStatePreInitUnlocked__)(struct OBJGPU *, struct KernelPerf * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kperfStateInitUnlocked__)(struct OBJGPU *, struct KernelPerf * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kperfStatePreLoad__)(struct OBJGPU *, struct KernelPerf * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kperfStatePostLoad__)(struct OBJGPU *, struct KernelPerf * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kperfStatePreUnload__)(struct OBJGPU *, struct KernelPerf * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kperfStatePostUnload__)(struct OBJGPU *, struct KernelPerf * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NvBool (*__kperfIsPresent__)(struct OBJGPU *, struct KernelPerf * /*this*/);  // virtual inherited (engstate) base (engstate)

    // Data members
    struct KERNEL_PERF_GPU_BOOST_SYNC sliGpuBoostSync;
    NvU32 reentrancyMask;
};

#ifndef __NVOC_CLASS_KernelPerf_TYPEDEF__
#define __NVOC_CLASS_KernelPerf_TYPEDEF__
typedef struct KernelPerf KernelPerf;
#endif /* __NVOC_CLASS_KernelPerf_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelPerf
#define __nvoc_class_id_KernelPerf 0xc53a57
#endif /* __nvoc_class_id_KernelPerf */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelPerf;

#define __staticCast_KernelPerf(pThis) \
    ((pThis)->__nvoc_pbase_KernelPerf)

#ifdef __nvoc_kern_perf_h_disabled
#define __dynamicCast_KernelPerf(pThis) ((KernelPerf*)NULL)
#else //__nvoc_kern_perf_h_disabled
#define __dynamicCast_KernelPerf(pThis) \
    ((KernelPerf*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelPerf)))
#endif //__nvoc_kern_perf_h_disabled

// Property macros
#define PDB_PROP_KPERF_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KPERF_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelPerf(KernelPerf**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelPerf(KernelPerf**, Dynamic*, NvU32);
#define __objCreate_KernelPerf(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelPerf((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define kperfConstructEngine_FNPTR(pKernelPerf) pKernelPerf->__kperfConstructEngine__
#define kperfConstructEngine(pGpu, pKernelPerf, engDesc) kperfConstructEngine_DISPATCH(pGpu, pKernelPerf, engDesc)
#define kperfStateInitLocked_FNPTR(pKernelPerf) pKernelPerf->__kperfStateInitLocked__
#define kperfStateInitLocked(pGpu, pKernelPerf) kperfStateInitLocked_DISPATCH(pGpu, pKernelPerf)
#define kperfStateLoad_FNPTR(pKernelPerf) pKernelPerf->__kperfStateLoad__
#define kperfStateLoad(pGpu, pKernelPerf, flags) kperfStateLoad_DISPATCH(pGpu, pKernelPerf, flags)
#define kperfStateUnload_FNPTR(pKernelPerf) pKernelPerf->__kperfStateUnload__
#define kperfStateUnload(pGpu, pKernelPerf, flags) kperfStateUnload_DISPATCH(pGpu, pKernelPerf, flags)
#define kperfStateDestroy_FNPTR(pKernelPerf) pKernelPerf->__kperfStateDestroy__
#define kperfStateDestroy(pGpu, pKernelPerf) kperfStateDestroy_DISPATCH(pGpu, pKernelPerf)
#define kperfGpuBoostSyncStateInit_FNPTR(pKernelPerf) pKernelPerf->__kperfGpuBoostSyncStateInit__
#define kperfGpuBoostSyncStateInit(pGpu, pKernelPerf) kperfGpuBoostSyncStateInit_DISPATCH(pGpu, pKernelPerf)
#define kperfGpuBoostSyncStateInit_HAL(pGpu, pKernelPerf) kperfGpuBoostSyncStateInit_DISPATCH(pGpu, pKernelPerf)
#define kperfInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateInitMissing__
#define kperfInitMissing(pGpu, pEngstate) kperfInitMissing_DISPATCH(pGpu, pEngstate)
#define kperfStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreInitLocked__
#define kperfStatePreInitLocked(pGpu, pEngstate) kperfStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kperfStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreInitUnlocked__
#define kperfStatePreInitUnlocked(pGpu, pEngstate) kperfStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kperfStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateInitUnlocked__
#define kperfStateInitUnlocked(pGpu, pEngstate) kperfStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kperfStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreLoad__
#define kperfStatePreLoad(pGpu, pEngstate, arg3) kperfStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kperfStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__
#define kperfStatePostLoad(pGpu, pEngstate, arg3) kperfStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kperfStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreUnload__
#define kperfStatePreUnload(pGpu, pEngstate, arg3) kperfStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kperfStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePostUnload__
#define kperfStatePostUnload(pGpu, pEngstate, arg3) kperfStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kperfIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateIsPresent__
#define kperfIsPresent(pGpu, pEngstate) kperfIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS kperfConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf, ENGDESCRIPTOR engDesc) {
    return pKernelPerf->__kperfConstructEngine__(pGpu, pKernelPerf, engDesc);
}

static inline NV_STATUS kperfStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf) {
    return pKernelPerf->__kperfStateInitLocked__(pGpu, pKernelPerf);
}

static inline NV_STATUS kperfStateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf, NvU32 flags) {
    return pKernelPerf->__kperfStateLoad__(pGpu, pKernelPerf, flags);
}

static inline NV_STATUS kperfStateUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf, NvU32 flags) {
    return pKernelPerf->__kperfStateUnload__(pGpu, pKernelPerf, flags);
}

static inline void kperfStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf) {
    pKernelPerf->__kperfStateDestroy__(pGpu, pKernelPerf);
}

static inline NV_STATUS kperfGpuBoostSyncStateInit_DISPATCH(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf) {
    return pKernelPerf->__kperfGpuBoostSyncStateInit__(pGpu, pKernelPerf);
}

static inline void kperfInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelPerf *pEngstate) {
    pEngstate->__kperfInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kperfStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelPerf *pEngstate) {
    return pEngstate->__kperfStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kperfStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelPerf *pEngstate) {
    return pEngstate->__kperfStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kperfStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelPerf *pEngstate) {
    return pEngstate->__kperfStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kperfStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelPerf *pEngstate, NvU32 arg3) {
    return pEngstate->__kperfStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kperfStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelPerf *pEngstate, NvU32 arg3) {
    return pEngstate->__kperfStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kperfStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelPerf *pEngstate, NvU32 arg3) {
    return pEngstate->__kperfStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kperfStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelPerf *pEngstate, NvU32 arg3) {
    return pEngstate->__kperfStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline NvBool kperfIsPresent_DISPATCH(struct OBJGPU *pGpu, struct KernelPerf *pEngstate) {
    return pEngstate->__kperfIsPresent__(pGpu, pEngstate);
}

NV_STATUS kperfConstructEngine_IMPL(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf, ENGDESCRIPTOR engDesc);

NV_STATUS kperfStateInitLocked_IMPL(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf);

NV_STATUS kperfStateLoad_IMPL(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf, NvU32 flags);

NV_STATUS kperfStateUnload_IMPL(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf, NvU32 flags);

void kperfStateDestroy_IMPL(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf);

static inline NV_STATUS kperfGpuBoostSyncStateInit_56cd7a(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf) {
    return NV_OK;
}

NV_STATUS kperfGpuBoostSyncStateInit_IMPL(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf);

NV_STATUS kperfGpuBoostSyncActivate_IMPL(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf, NvBool bActivate);

#ifdef __nvoc_kern_perf_h_disabled
static inline NV_STATUS kperfGpuBoostSyncActivate(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf, NvBool bActivate) {
    NV_ASSERT_FAILED_PRECOMP("KernelPerf was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_perf_h_disabled
#define kperfGpuBoostSyncActivate(pGpu, pKernelPerf, bActivate) kperfGpuBoostSyncActivate_IMPL(pGpu, pKernelPerf, bActivate)
#endif //__nvoc_kern_perf_h_disabled

NV_STATUS kperfDoSyncGpuBoostLimits_IMPL(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS_PARAMS *pParams);

#ifdef __nvoc_kern_perf_h_disabled
static inline NV_STATUS kperfDoSyncGpuBoostLimits(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf, NV2080_CTRL_INTERNAL_PERF_GPU_BOOST_SYNC_SET_LIMITS_PARAMS *pParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelPerf was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_perf_h_disabled
#define kperfDoSyncGpuBoostLimits(pGpu, pKernelPerf, pParams) kperfDoSyncGpuBoostLimits_IMPL(pGpu, pKernelPerf, pParams)
#endif //__nvoc_kern_perf_h_disabled

NV_STATUS kperfBoostSet_IMPL(struct KernelPerf *pKernelPerf, struct Subdevice *pSubdevice, NV2080_CTRL_PERF_BOOST_PARAMS *pBoostParams);

#ifdef __nvoc_kern_perf_h_disabled
static inline NV_STATUS kperfBoostSet(struct KernelPerf *pKernelPerf, struct Subdevice *pSubdevice, NV2080_CTRL_PERF_BOOST_PARAMS *pBoostParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelPerf was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_perf_h_disabled
#define kperfBoostSet(pKernelPerf, pSubdevice, pBoostParams) kperfBoostSet_IMPL(pKernelPerf, pSubdevice, pBoostParams)
#endif //__nvoc_kern_perf_h_disabled

NV_STATUS kperfReentrancy_IMPL(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf, NvU32 function, NvBool bSet);

#ifdef __nvoc_kern_perf_h_disabled
static inline NV_STATUS kperfReentrancy(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf, NvU32 function, NvBool bSet) {
    NV_ASSERT_FAILED_PRECOMP("KernelPerf was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_perf_h_disabled
#define kperfReentrancy(pGpu, pKernelPerf, function, bSet) kperfReentrancy_IMPL(pGpu, pKernelPerf, function, bSet)
#endif //__nvoc_kern_perf_h_disabled

#undef PRIVATE_FIELD


/* ------------------------ External Definitions --------------------------- */
/* ------------------------ Function Prototypes ---------------------------- */
/* ------------------------ Include Derived Types -------------------------- */

#endif // KERNEL_PERF_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERN_PERF_NVOC_H_
