#ifndef _G_KERN_PERF_NVOC_H_
#define _G_KERN_PERF_NVOC_H_
#include "nvoc/runtime.h"

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
#ifdef NVOC_KERN_PERF_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelPerf {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct KernelPerf *__nvoc_pbase_KernelPerf;
    NV_STATUS (*__kperfConstructEngine__)(struct OBJGPU *, struct KernelPerf *, ENGDESCRIPTOR);
    NV_STATUS (*__kperfStateInitLocked__)(struct OBJGPU *, struct KernelPerf *);
    NV_STATUS (*__kperfStateLoad__)(struct OBJGPU *, struct KernelPerf *, NvU32);
    NV_STATUS (*__kperfStateUnload__)(struct OBJGPU *, struct KernelPerf *, NvU32);
    void (*__kperfStateDestroy__)(struct OBJGPU *, struct KernelPerf *);
    NV_STATUS (*__kperfStatePreLoad__)(POBJGPU, struct KernelPerf *, NvU32);
    NV_STATUS (*__kperfStatePostUnload__)(POBJGPU, struct KernelPerf *, NvU32);
    NV_STATUS (*__kperfStatePreUnload__)(POBJGPU, struct KernelPerf *, NvU32);
    NV_STATUS (*__kperfStateInitUnlocked__)(POBJGPU, struct KernelPerf *);
    void (*__kperfInitMissing__)(POBJGPU, struct KernelPerf *);
    NV_STATUS (*__kperfStatePreInitLocked__)(POBJGPU, struct KernelPerf *);
    NV_STATUS (*__kperfStatePreInitUnlocked__)(POBJGPU, struct KernelPerf *);
    NV_STATUS (*__kperfStatePostLoad__)(POBJGPU, struct KernelPerf *, NvU32);
    NvBool (*__kperfIsPresent__)(POBJGPU, struct KernelPerf *);
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

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelPerf;

#define __staticCast_KernelPerf(pThis) \
    ((pThis)->__nvoc_pbase_KernelPerf)

#ifdef __nvoc_kern_perf_h_disabled
#define __dynamicCast_KernelPerf(pThis) ((KernelPerf*)NULL)
#else //__nvoc_kern_perf_h_disabled
#define __dynamicCast_KernelPerf(pThis) \
    ((KernelPerf*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelPerf)))
#endif //__nvoc_kern_perf_h_disabled

#define PDB_PROP_KPERF_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KPERF_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelPerf(KernelPerf**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelPerf(KernelPerf**, Dynamic*, NvU32);
#define __objCreate_KernelPerf(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelPerf((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define kperfConstructEngine(pGpu, pKernelPerf, engDesc) kperfConstructEngine_DISPATCH(pGpu, pKernelPerf, engDesc)
#define kperfStateInitLocked(pGpu, pKernelPerf) kperfStateInitLocked_DISPATCH(pGpu, pKernelPerf)
#define kperfStateLoad(pGpu, pKernelPerf, flags) kperfStateLoad_DISPATCH(pGpu, pKernelPerf, flags)
#define kperfStateUnload(pGpu, pKernelPerf, flags) kperfStateUnload_DISPATCH(pGpu, pKernelPerf, flags)
#define kperfStateDestroy(pGpu, pKernelPerf) kperfStateDestroy_DISPATCH(pGpu, pKernelPerf)
#define kperfStatePreLoad(pGpu, pEngstate, arg0) kperfStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kperfStatePostUnload(pGpu, pEngstate, arg0) kperfStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kperfStatePreUnload(pGpu, pEngstate, arg0) kperfStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kperfStateInitUnlocked(pGpu, pEngstate) kperfStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kperfInitMissing(pGpu, pEngstate) kperfInitMissing_DISPATCH(pGpu, pEngstate)
#define kperfStatePreInitLocked(pGpu, pEngstate) kperfStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kperfStatePreInitUnlocked(pGpu, pEngstate) kperfStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kperfStatePostLoad(pGpu, pEngstate, arg0) kperfStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kperfIsPresent(pGpu, pEngstate) kperfIsPresent_DISPATCH(pGpu, pEngstate)
NV_STATUS kperfGpuBoostSyncStateInit_IMPL(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf);


#ifdef __nvoc_kern_perf_h_disabled
static inline NV_STATUS kperfGpuBoostSyncStateInit(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf) {
    NV_ASSERT_FAILED_PRECOMP("KernelPerf was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_perf_h_disabled
#define kperfGpuBoostSyncStateInit(pGpu, pKernelPerf) kperfGpuBoostSyncStateInit_IMPL(pGpu, pKernelPerf)
#endif //__nvoc_kern_perf_h_disabled

#define kperfGpuBoostSyncStateInit_HAL(pGpu, pKernelPerf) kperfGpuBoostSyncStateInit(pGpu, pKernelPerf)

NV_STATUS kperfBoostSet_3x(struct KernelPerf *pKernelPerf, struct Subdevice *pSubdevice, NV2080_CTRL_PERF_BOOST_PARAMS *pBoostParams);


#ifdef __nvoc_kern_perf_h_disabled
static inline NV_STATUS kperfBoostSet(struct KernelPerf *pKernelPerf, struct Subdevice *pSubdevice, NV2080_CTRL_PERF_BOOST_PARAMS *pBoostParams) {
    NV_ASSERT_FAILED_PRECOMP("KernelPerf was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_perf_h_disabled
#define kperfBoostSet(pKernelPerf, pSubdevice, pBoostParams) kperfBoostSet_3x(pKernelPerf, pSubdevice, pBoostParams)
#endif //__nvoc_kern_perf_h_disabled

#define kperfBoostSet_HAL(pKernelPerf, pSubdevice, pBoostParams) kperfBoostSet(pKernelPerf, pSubdevice, pBoostParams)

NV_STATUS kperfConstructEngine_IMPL(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf, ENGDESCRIPTOR engDesc);

static inline NV_STATUS kperfConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf, ENGDESCRIPTOR engDesc) {
    return pKernelPerf->__kperfConstructEngine__(pGpu, pKernelPerf, engDesc);
}

NV_STATUS kperfStateInitLocked_IMPL(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf);

static inline NV_STATUS kperfStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf) {
    return pKernelPerf->__kperfStateInitLocked__(pGpu, pKernelPerf);
}

NV_STATUS kperfStateLoad_IMPL(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf, NvU32 flags);

static inline NV_STATUS kperfStateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf, NvU32 flags) {
    return pKernelPerf->__kperfStateLoad__(pGpu, pKernelPerf, flags);
}

NV_STATUS kperfStateUnload_IMPL(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf, NvU32 flags);

static inline NV_STATUS kperfStateUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf, NvU32 flags) {
    return pKernelPerf->__kperfStateUnload__(pGpu, pKernelPerf, flags);
}

void kperfStateDestroy_IMPL(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf);

static inline void kperfStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct KernelPerf *pKernelPerf) {
    pKernelPerf->__kperfStateDestroy__(pGpu, pKernelPerf);
}

static inline NV_STATUS kperfStatePreLoad_DISPATCH(POBJGPU pGpu, struct KernelPerf *pEngstate, NvU32 arg0) {
    return pEngstate->__kperfStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kperfStatePostUnload_DISPATCH(POBJGPU pGpu, struct KernelPerf *pEngstate, NvU32 arg0) {
    return pEngstate->__kperfStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kperfStatePreUnload_DISPATCH(POBJGPU pGpu, struct KernelPerf *pEngstate, NvU32 arg0) {
    return pEngstate->__kperfStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kperfStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelPerf *pEngstate) {
    return pEngstate->__kperfStateInitUnlocked__(pGpu, pEngstate);
}

static inline void kperfInitMissing_DISPATCH(POBJGPU pGpu, struct KernelPerf *pEngstate) {
    pEngstate->__kperfInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kperfStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct KernelPerf *pEngstate) {
    return pEngstate->__kperfStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kperfStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelPerf *pEngstate) {
    return pEngstate->__kperfStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kperfStatePostLoad_DISPATCH(POBJGPU pGpu, struct KernelPerf *pEngstate, NvU32 arg0) {
    return pEngstate->__kperfStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NvBool kperfIsPresent_DISPATCH(POBJGPU pGpu, struct KernelPerf *pEngstate) {
    return pEngstate->__kperfIsPresent__(pGpu, pEngstate);
}

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
