
#ifndef _G_KERN_HWPM_NVOC_H_
#define _G_KERN_HWPM_NVOC_H_
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
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_kern_hwpm_nvoc.h"

#ifndef KERNEL_HWPM_H
#define KERNEL_HWPM_H

/******************************************************************************
 *
 *   Kernel Hwpm module header
 *   This file contains functions managing HWPM on CPU RM
 *
 ******************************************************************************/

#include "gpu/gpu.h"
#include "gpu/eng_state.h"
#include "gpu/hwpm/kern_hwpm_power.h"
#include "gpu/hwpm/kern_hwpm_common_defs.h"

#define INVALID_PMA_CHANNEL_IDX NV_U32_MAX

// default values for the perf vaspace base and size
#define PERF_VASPACE_BASE (4*1024*1024*1024ULL)
#define PERF_VASPACE_SIZE (1024*1024*1024*1024ULL)


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERN_HWPM_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


struct KernelHwpm {

    // Metadata
    const struct NVOC_RTTI *__nvoc_rtti;

    // Parent (i.e. superclass or base class) object pointers
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct KernelHwpm *__nvoc_pbase_KernelHwpm;    // khwpm

    // Vtable with 15 per-object function pointers
    NV_STATUS (*__khwpmStateInitUnlocked__)(OBJGPU *, struct KernelHwpm * /*this*/);  // virtual override (engstate) base (engstate)
    void (*__khwpmStateDestroy__)(OBJGPU *, struct KernelHwpm * /*this*/);  // virtual override (engstate) base (engstate)
    void (*__khwpmGetCblockInfo__)(OBJGPU *, struct KernelHwpm * /*this*/, NvU32 *, NvU32 *);  // halified (3 hals) body
    NV_STATUS (*__khwpmConstructEngine__)(struct OBJGPU *, struct KernelHwpm * /*this*/, ENGDESCRIPTOR);  // virtual inherited (engstate) base (engstate)
    void (*__khwpmInitMissing__)(struct OBJGPU *, struct KernelHwpm * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__khwpmStatePreInitLocked__)(struct OBJGPU *, struct KernelHwpm * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__khwpmStatePreInitUnlocked__)(struct OBJGPU *, struct KernelHwpm * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__khwpmStateInitLocked__)(struct OBJGPU *, struct KernelHwpm * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__khwpmStatePreLoad__)(struct OBJGPU *, struct KernelHwpm * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__khwpmStateLoad__)(struct OBJGPU *, struct KernelHwpm * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__khwpmStatePostLoad__)(struct OBJGPU *, struct KernelHwpm * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__khwpmStatePreUnload__)(struct OBJGPU *, struct KernelHwpm * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__khwpmStateUnload__)(struct OBJGPU *, struct KernelHwpm * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__khwpmStatePostUnload__)(struct OBJGPU *, struct KernelHwpm * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NvBool (*__khwpmIsPresent__)(struct OBJGPU *, struct KernelHwpm * /*this*/);  // virtual inherited (engstate) base (engstate)

    // 1 PDB property
    NvBool PDB_PROP_KHWPM_MULTIPLE_PMA_SUPPORTED;

    // Data members
    NvU32 numPma;
    NvU32 maxCblocks;
    NvU32 maxCblocksPerPma;
    NvU32 maxPmaChannels;
    NvU32 maxChannelPerCblock;
    HWPM_STREAMOUT_STATE *streamoutState;
    NvU64 vaSpaceBase;
    NvU64 vaSpaceSize;
};

#ifndef __NVOC_CLASS_KernelHwpm_TYPEDEF__
#define __NVOC_CLASS_KernelHwpm_TYPEDEF__
typedef struct KernelHwpm KernelHwpm;
#endif /* __NVOC_CLASS_KernelHwpm_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelHwpm
#define __nvoc_class_id_KernelHwpm 0xc8c00f
#endif /* __nvoc_class_id_KernelHwpm */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHwpm;

#define __staticCast_KernelHwpm(pThis) \
    ((pThis)->__nvoc_pbase_KernelHwpm)

#ifdef __nvoc_kern_hwpm_h_disabled
#define __dynamicCast_KernelHwpm(pThis) ((KernelHwpm*)NULL)
#else //__nvoc_kern_hwpm_h_disabled
#define __dynamicCast_KernelHwpm(pThis) \
    ((KernelHwpm*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelHwpm)))
#endif //__nvoc_kern_hwpm_h_disabled

// Property macros
#define PDB_PROP_KHWPM_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KHWPM_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING
#define PDB_PROP_KHWPM_MULTIPLE_PMA_SUPPORTED_BASE_CAST
#define PDB_PROP_KHWPM_MULTIPLE_PMA_SUPPORTED_BASE_NAME PDB_PROP_KHWPM_MULTIPLE_PMA_SUPPORTED

NV_STATUS __nvoc_objCreateDynamic_KernelHwpm(KernelHwpm**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelHwpm(KernelHwpm**, Dynamic*, NvU32);
#define __objCreate_KernelHwpm(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelHwpm((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define khwpmStateInitUnlocked_FNPTR(pKernelHwpm) pKernelHwpm->__khwpmStateInitUnlocked__
#define khwpmStateInitUnlocked(pGpu, pKernelHwpm) khwpmStateInitUnlocked_DISPATCH(pGpu, pKernelHwpm)
#define khwpmStateDestroy_FNPTR(pKernelHwpm) pKernelHwpm->__khwpmStateDestroy__
#define khwpmStateDestroy(pGpu, pKernelHwpm) khwpmStateDestroy_DISPATCH(pGpu, pKernelHwpm)
#define khwpmGetCblockInfo_FNPTR(pKernelHwpm) pKernelHwpm->__khwpmGetCblockInfo__
#define khwpmGetCblockInfo(pGpu, pKernelHwpm, arg3, arg4) khwpmGetCblockInfo_DISPATCH(pGpu, pKernelHwpm, arg3, arg4)
#define khwpmGetCblockInfo_HAL(pGpu, pKernelHwpm, arg3, arg4) khwpmGetCblockInfo_DISPATCH(pGpu, pKernelHwpm, arg3, arg4)
#define khwpmConstructEngine_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateConstructEngine__
#define khwpmConstructEngine(pGpu, pEngstate, arg3) khwpmConstructEngine_DISPATCH(pGpu, pEngstate, arg3)
#define khwpmInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateInitMissing__
#define khwpmInitMissing(pGpu, pEngstate) khwpmInitMissing_DISPATCH(pGpu, pEngstate)
#define khwpmStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreInitLocked__
#define khwpmStatePreInitLocked(pGpu, pEngstate) khwpmStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define khwpmStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreInitUnlocked__
#define khwpmStatePreInitUnlocked(pGpu, pEngstate) khwpmStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define khwpmStateInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateInitLocked__
#define khwpmStateInitLocked(pGpu, pEngstate) khwpmStateInitLocked_DISPATCH(pGpu, pEngstate)
#define khwpmStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreLoad__
#define khwpmStatePreLoad(pGpu, pEngstate, arg3) khwpmStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define khwpmStateLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateLoad__
#define khwpmStateLoad(pGpu, pEngstate, arg3) khwpmStateLoad_DISPATCH(pGpu, pEngstate, arg3)
#define khwpmStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePostLoad__
#define khwpmStatePostLoad(pGpu, pEngstate, arg3) khwpmStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define khwpmStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePreUnload__
#define khwpmStatePreUnload(pGpu, pEngstate, arg3) khwpmStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define khwpmStateUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStateUnload__
#define khwpmStateUnload(pGpu, pEngstate, arg3) khwpmStateUnload_DISPATCH(pGpu, pEngstate, arg3)
#define khwpmStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateStatePostUnload__
#define khwpmStatePostUnload(pGpu, pEngstate, arg3) khwpmStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define khwpmIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__engstateIsPresent__
#define khwpmIsPresent(pGpu, pEngstate) khwpmIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS khwpmStateInitUnlocked_DISPATCH(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm) {
    return pKernelHwpm->__khwpmStateInitUnlocked__(pGpu, pKernelHwpm);
}

static inline void khwpmStateDestroy_DISPATCH(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm) {
    pKernelHwpm->__khwpmStateDestroy__(pGpu, pKernelHwpm);
}

static inline void khwpmGetCblockInfo_DISPATCH(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 *arg3, NvU32 *arg4) {
    pKernelHwpm->__khwpmGetCblockInfo__(pGpu, pKernelHwpm, arg3, arg4);
}

static inline NV_STATUS khwpmConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, ENGDESCRIPTOR arg3) {
    return pEngstate->__khwpmConstructEngine__(pGpu, pEngstate, arg3);
}

static inline void khwpmInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate) {
    pEngstate->__khwpmInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS khwpmStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate) {
    return pEngstate->__khwpmStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS khwpmStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate) {
    return pEngstate->__khwpmStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS khwpmStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate) {
    return pEngstate->__khwpmStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS khwpmStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3) {
    return pEngstate->__khwpmStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS khwpmStateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3) {
    return pEngstate->__khwpmStateLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS khwpmStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3) {
    return pEngstate->__khwpmStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS khwpmStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3) {
    return pEngstate->__khwpmStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS khwpmStateUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3) {
    return pEngstate->__khwpmStateUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS khwpmStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate, NvU32 arg3) {
    return pEngstate->__khwpmStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline NvBool khwpmIsPresent_DISPATCH(struct OBJGPU *pGpu, struct KernelHwpm *pEngstate) {
    return pEngstate->__khwpmIsPresent__(pGpu, pEngstate);
}

static inline NV_STATUS khwpmPmaStreamSriovSetGfid_56cd7a(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 arg3, NvU32 arg4) {
    return NV_OK;
}

NV_STATUS khwpmPmaStreamSriovSetGfid_GA100(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 arg3, NvU32 arg4);

NV_STATUS khwpmPmaStreamSriovSetGfid_GH100(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 arg3, NvU32 arg4);

NV_STATUS khwpmPmaStreamSriovSetGfid_GB100(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 arg3, NvU32 arg4);

static inline NV_STATUS khwpmPmaStreamSriovSetGfid_92bfc3(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 arg3, NvU32 arg4) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_kern_hwpm_h_disabled
static inline NV_STATUS khwpmPmaStreamSriovSetGfid(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 arg3, NvU32 arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelHwpm was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_hwpm_h_disabled
#define khwpmPmaStreamSriovSetGfid(pGpu, pKernelHwpm, arg3, arg4) khwpmPmaStreamSriovSetGfid_56cd7a(pGpu, pKernelHwpm, arg3, arg4)
#endif //__nvoc_kern_hwpm_h_disabled

#define khwpmPmaStreamSriovSetGfid_HAL(pGpu, pKernelHwpm, arg3, arg4) khwpmPmaStreamSriovSetGfid(pGpu, pKernelHwpm, arg3, arg4)

NV_STATUS khwpmStateInitUnlocked_IMPL(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm);

void khwpmStateDestroy_IMPL(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm);

void khwpmGetCblockInfo_GM107(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 *arg3, NvU32 *arg4);

void khwpmGetCblockInfo_GH100(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 *arg3, NvU32 *arg4);

void khwpmGetCblockInfo_GB100(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 *arg3, NvU32 *arg4);

NV_STATUS khwpmStreamoutAllocPmaStream_IMPL(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU64 arg3, MEMORY_DESCRIPTOR *arg4, MEMORY_DESCRIPTOR *arg5, NvU32 arg6, HWPM_PMA_STREAM *arg7);

#ifdef __nvoc_kern_hwpm_h_disabled
static inline NV_STATUS khwpmStreamoutAllocPmaStream(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU64 arg3, MEMORY_DESCRIPTOR *arg4, MEMORY_DESCRIPTOR *arg5, NvU32 arg6, HWPM_PMA_STREAM *arg7) {
    NV_ASSERT_FAILED_PRECOMP("KernelHwpm was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_hwpm_h_disabled
#define khwpmStreamoutAllocPmaStream(pGpu, pKernelHwpm, arg3, arg4, arg5, arg6, arg7) khwpmStreamoutAllocPmaStream_IMPL(pGpu, pKernelHwpm, arg3, arg4, arg5, arg6, arg7)
#endif //__nvoc_kern_hwpm_h_disabled

NV_STATUS khwpmStreamoutFreePmaStream_IMPL(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU64 arg3, HWPM_PMA_STREAM *arg4, NvU32 arg5);

#ifdef __nvoc_kern_hwpm_h_disabled
static inline NV_STATUS khwpmStreamoutFreePmaStream(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU64 arg3, HWPM_PMA_STREAM *arg4, NvU32 arg5) {
    NV_ASSERT_FAILED_PRECOMP("KernelHwpm was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_hwpm_h_disabled
#define khwpmStreamoutFreePmaStream(pGpu, pKernelHwpm, arg3, arg4, arg5) khwpmStreamoutFreePmaStream_IMPL(pGpu, pKernelHwpm, arg3, arg4, arg5)
#endif //__nvoc_kern_hwpm_h_disabled

NV_STATUS khwpmStreamoutCreatePmaVaSpace_IMPL(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 arg3);

#ifdef __nvoc_kern_hwpm_h_disabled
static inline NV_STATUS khwpmStreamoutCreatePmaVaSpace(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelHwpm was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_hwpm_h_disabled
#define khwpmStreamoutCreatePmaVaSpace(pGpu, pKernelHwpm, arg3) khwpmStreamoutCreatePmaVaSpace_IMPL(pGpu, pKernelHwpm, arg3)
#endif //__nvoc_kern_hwpm_h_disabled

NV_STATUS khwpmStreamoutFreePmaVaSpace_IMPL(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 arg3);

#ifdef __nvoc_kern_hwpm_h_disabled
static inline NV_STATUS khwpmStreamoutFreePmaVaSpace(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelHwpm was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_hwpm_h_disabled
#define khwpmStreamoutFreePmaVaSpace(pGpu, pKernelHwpm, arg3) khwpmStreamoutFreePmaVaSpace_IMPL(pGpu, pKernelHwpm, arg3)
#endif //__nvoc_kern_hwpm_h_disabled

#undef PRIVATE_FIELD


NV_STATUS khwpmGetRequestCgStatusMask(NvU32 *pCgStatusMask, HWPM_POWER_REQUEST_FEATURES_PARAMS *pParams);

#endif // KERNEL_HWPM_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERN_HWPM_NVOC_H_
