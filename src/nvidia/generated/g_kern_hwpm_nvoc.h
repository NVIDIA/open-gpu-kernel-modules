#ifndef _G_KERN_HWPM_NVOC_H_
#define _G_KERN_HWPM_NVOC_H_
#include "nvoc/runtime.h"

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
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct KernelHwpm *__nvoc_pbase_KernelHwpm;
    NV_STATUS (*__khwpmStateInitUnlocked__)(OBJGPU *, struct KernelHwpm *);
    void (*__khwpmStateDestroy__)(OBJGPU *, struct KernelHwpm *);
    void (*__khwpmGetCblockInfo__)(OBJGPU *, struct KernelHwpm *, NvU32 *, NvU32 *);
    NV_STATUS (*__khwpmStateLoad__)(POBJGPU, struct KernelHwpm *, NvU32);
    NV_STATUS (*__khwpmStateUnload__)(POBJGPU, struct KernelHwpm *, NvU32);
    NV_STATUS (*__khwpmStateInitLocked__)(POBJGPU, struct KernelHwpm *);
    NV_STATUS (*__khwpmStatePreLoad__)(POBJGPU, struct KernelHwpm *, NvU32);
    NV_STATUS (*__khwpmStatePostUnload__)(POBJGPU, struct KernelHwpm *, NvU32);
    NV_STATUS (*__khwpmStatePreUnload__)(POBJGPU, struct KernelHwpm *, NvU32);
    void (*__khwpmInitMissing__)(POBJGPU, struct KernelHwpm *);
    NV_STATUS (*__khwpmStatePreInitLocked__)(POBJGPU, struct KernelHwpm *);
    NV_STATUS (*__khwpmStatePreInitUnlocked__)(POBJGPU, struct KernelHwpm *);
    NV_STATUS (*__khwpmStatePostLoad__)(POBJGPU, struct KernelHwpm *, NvU32);
    NV_STATUS (*__khwpmConstructEngine__)(POBJGPU, struct KernelHwpm *, ENGDESCRIPTOR);
    NvBool (*__khwpmIsPresent__)(POBJGPU, struct KernelHwpm *);
    NvBool PDB_PROP_KHWPM_MULTIPLE_PMA_SUPPORTED;
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

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelHwpm;

#define __staticCast_KernelHwpm(pThis) \
    ((pThis)->__nvoc_pbase_KernelHwpm)

#ifdef __nvoc_kern_hwpm_h_disabled
#define __dynamicCast_KernelHwpm(pThis) ((KernelHwpm*)NULL)
#else //__nvoc_kern_hwpm_h_disabled
#define __dynamicCast_KernelHwpm(pThis) \
    ((KernelHwpm*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelHwpm)))
#endif //__nvoc_kern_hwpm_h_disabled

#define PDB_PROP_KHWPM_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KHWPM_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING
#define PDB_PROP_KHWPM_MULTIPLE_PMA_SUPPORTED_BASE_CAST
#define PDB_PROP_KHWPM_MULTIPLE_PMA_SUPPORTED_BASE_NAME PDB_PROP_KHWPM_MULTIPLE_PMA_SUPPORTED

NV_STATUS __nvoc_objCreateDynamic_KernelHwpm(KernelHwpm**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelHwpm(KernelHwpm**, Dynamic*, NvU32);
#define __objCreate_KernelHwpm(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelHwpm((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define khwpmStateInitUnlocked(pGpu, pKernelHwpm) khwpmStateInitUnlocked_DISPATCH(pGpu, pKernelHwpm)
#define khwpmStateDestroy(pGpu, pKernelHwpm) khwpmStateDestroy_DISPATCH(pGpu, pKernelHwpm)
#define khwpmGetCblockInfo(pGpu, pKernelHwpm, arg0, arg1) khwpmGetCblockInfo_DISPATCH(pGpu, pKernelHwpm, arg0, arg1)
#define khwpmGetCblockInfo_HAL(pGpu, pKernelHwpm, arg0, arg1) khwpmGetCblockInfo_DISPATCH(pGpu, pKernelHwpm, arg0, arg1)
#define khwpmStateLoad(pGpu, pEngstate, arg0) khwpmStateLoad_DISPATCH(pGpu, pEngstate, arg0)
#define khwpmStateUnload(pGpu, pEngstate, arg0) khwpmStateUnload_DISPATCH(pGpu, pEngstate, arg0)
#define khwpmStateInitLocked(pGpu, pEngstate) khwpmStateInitLocked_DISPATCH(pGpu, pEngstate)
#define khwpmStatePreLoad(pGpu, pEngstate, arg0) khwpmStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define khwpmStatePostUnload(pGpu, pEngstate, arg0) khwpmStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define khwpmStatePreUnload(pGpu, pEngstate, arg0) khwpmStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define khwpmInitMissing(pGpu, pEngstate) khwpmInitMissing_DISPATCH(pGpu, pEngstate)
#define khwpmStatePreInitLocked(pGpu, pEngstate) khwpmStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define khwpmStatePreInitUnlocked(pGpu, pEngstate) khwpmStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define khwpmStatePostLoad(pGpu, pEngstate, arg0) khwpmStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define khwpmConstructEngine(pGpu, pEngstate, arg0) khwpmConstructEngine_DISPATCH(pGpu, pEngstate, arg0)
#define khwpmIsPresent(pGpu, pEngstate) khwpmIsPresent_DISPATCH(pGpu, pEngstate)
static inline NV_STATUS khwpmPmaStreamSriovSetGfid_56cd7a(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 arg0, NvU32 arg1) {
    return NV_OK;
}

NV_STATUS khwpmPmaStreamSriovSetGfid_GA100(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 arg0, NvU32 arg1);

NV_STATUS khwpmPmaStreamSriovSetGfid_GH100(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 arg0, NvU32 arg1);

static inline NV_STATUS khwpmPmaStreamSriovSetGfid_92bfc3(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 arg0, NvU32 arg1) {
    NV_ASSERT_PRECOMP(0);
    return NV_ERR_NOT_SUPPORTED;
}


#ifdef __nvoc_kern_hwpm_h_disabled
static inline NV_STATUS khwpmPmaStreamSriovSetGfid(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 arg0, NvU32 arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelHwpm was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_hwpm_h_disabled
#define khwpmPmaStreamSriovSetGfid(pGpu, pKernelHwpm, arg0, arg1) khwpmPmaStreamSriovSetGfid_56cd7a(pGpu, pKernelHwpm, arg0, arg1)
#endif //__nvoc_kern_hwpm_h_disabled

#define khwpmPmaStreamSriovSetGfid_HAL(pGpu, pKernelHwpm, arg0, arg1) khwpmPmaStreamSriovSetGfid(pGpu, pKernelHwpm, arg0, arg1)

NV_STATUS khwpmStateInitUnlocked_IMPL(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm);

static inline NV_STATUS khwpmStateInitUnlocked_DISPATCH(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm) {
    return pKernelHwpm->__khwpmStateInitUnlocked__(pGpu, pKernelHwpm);
}

void khwpmStateDestroy_IMPL(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm);

static inline void khwpmStateDestroy_DISPATCH(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm) {
    pKernelHwpm->__khwpmStateDestroy__(pGpu, pKernelHwpm);
}

void khwpmGetCblockInfo_GM107(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 *arg0, NvU32 *arg1);

void khwpmGetCblockInfo_GH100(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 *arg0, NvU32 *arg1);

static inline void khwpmGetCblockInfo_DISPATCH(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 *arg0, NvU32 *arg1) {
    pKernelHwpm->__khwpmGetCblockInfo__(pGpu, pKernelHwpm, arg0, arg1);
}

static inline NV_STATUS khwpmStateLoad_DISPATCH(POBJGPU pGpu, struct KernelHwpm *pEngstate, NvU32 arg0) {
    return pEngstate->__khwpmStateLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS khwpmStateUnload_DISPATCH(POBJGPU pGpu, struct KernelHwpm *pEngstate, NvU32 arg0) {
    return pEngstate->__khwpmStateUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS khwpmStateInitLocked_DISPATCH(POBJGPU pGpu, struct KernelHwpm *pEngstate) {
    return pEngstate->__khwpmStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS khwpmStatePreLoad_DISPATCH(POBJGPU pGpu, struct KernelHwpm *pEngstate, NvU32 arg0) {
    return pEngstate->__khwpmStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS khwpmStatePostUnload_DISPATCH(POBJGPU pGpu, struct KernelHwpm *pEngstate, NvU32 arg0) {
    return pEngstate->__khwpmStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS khwpmStatePreUnload_DISPATCH(POBJGPU pGpu, struct KernelHwpm *pEngstate, NvU32 arg0) {
    return pEngstate->__khwpmStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline void khwpmInitMissing_DISPATCH(POBJGPU pGpu, struct KernelHwpm *pEngstate) {
    pEngstate->__khwpmInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS khwpmStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct KernelHwpm *pEngstate) {
    return pEngstate->__khwpmStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS khwpmStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelHwpm *pEngstate) {
    return pEngstate->__khwpmStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS khwpmStatePostLoad_DISPATCH(POBJGPU pGpu, struct KernelHwpm *pEngstate, NvU32 arg0) {
    return pEngstate->__khwpmStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS khwpmConstructEngine_DISPATCH(POBJGPU pGpu, struct KernelHwpm *pEngstate, ENGDESCRIPTOR arg0) {
    return pEngstate->__khwpmConstructEngine__(pGpu, pEngstate, arg0);
}

static inline NvBool khwpmIsPresent_DISPATCH(POBJGPU pGpu, struct KernelHwpm *pEngstate) {
    return pEngstate->__khwpmIsPresent__(pGpu, pEngstate);
}

NV_STATUS khwpmStreamoutAllocPmaStream_IMPL(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU64 arg0, MEMORY_DESCRIPTOR *arg1, MEMORY_DESCRIPTOR *arg2, NvU32 arg3, HWPM_PMA_STREAM *arg4);

#ifdef __nvoc_kern_hwpm_h_disabled
static inline NV_STATUS khwpmStreamoutAllocPmaStream(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU64 arg0, MEMORY_DESCRIPTOR *arg1, MEMORY_DESCRIPTOR *arg2, NvU32 arg3, HWPM_PMA_STREAM *arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelHwpm was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_hwpm_h_disabled
#define khwpmStreamoutAllocPmaStream(pGpu, pKernelHwpm, arg0, arg1, arg2, arg3, arg4) khwpmStreamoutAllocPmaStream_IMPL(pGpu, pKernelHwpm, arg0, arg1, arg2, arg3, arg4)
#endif //__nvoc_kern_hwpm_h_disabled

NV_STATUS khwpmStreamoutFreePmaStream_IMPL(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU64 arg0, HWPM_PMA_STREAM *arg1, NvU32 arg2);

#ifdef __nvoc_kern_hwpm_h_disabled
static inline NV_STATUS khwpmStreamoutFreePmaStream(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU64 arg0, HWPM_PMA_STREAM *arg1, NvU32 arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelHwpm was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_hwpm_h_disabled
#define khwpmStreamoutFreePmaStream(pGpu, pKernelHwpm, arg0, arg1, arg2) khwpmStreamoutFreePmaStream_IMPL(pGpu, pKernelHwpm, arg0, arg1, arg2)
#endif //__nvoc_kern_hwpm_h_disabled

NV_STATUS khwpmStreamoutCreatePmaVaSpace_IMPL(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 arg0);

#ifdef __nvoc_kern_hwpm_h_disabled
static inline NV_STATUS khwpmStreamoutCreatePmaVaSpace(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelHwpm was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_hwpm_h_disabled
#define khwpmStreamoutCreatePmaVaSpace(pGpu, pKernelHwpm, arg0) khwpmStreamoutCreatePmaVaSpace_IMPL(pGpu, pKernelHwpm, arg0)
#endif //__nvoc_kern_hwpm_h_disabled

NV_STATUS khwpmStreamoutFreePmaVaSpace_IMPL(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 arg0);

#ifdef __nvoc_kern_hwpm_h_disabled
static inline NV_STATUS khwpmStreamoutFreePmaVaSpace(OBJGPU *pGpu, struct KernelHwpm *pKernelHwpm, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("KernelHwpm was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_hwpm_h_disabled
#define khwpmStreamoutFreePmaVaSpace(pGpu, pKernelHwpm, arg0) khwpmStreamoutFreePmaVaSpace_IMPL(pGpu, pKernelHwpm, arg0)
#endif //__nvoc_kern_hwpm_h_disabled

#undef PRIVATE_FIELD


NV_STATUS khwpmGetRequestCgStatusMask(NvU32 *pCgStatusMask, HWPM_POWER_REQUEST_FEATURES_PARAMS *pParams);

#endif // KERNEL_HWPM_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERN_HWPM_NVOC_H_
