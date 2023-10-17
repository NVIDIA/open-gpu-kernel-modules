#ifndef _G_KERN_PMU_NVOC_H_
#define _G_KERN_PMU_NVOC_H_
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

#include "g_kern_pmu_nvoc.h"

#ifndef KERNEL_PMU_H
#define KERNEL_PMU_H

/******************************************************************************
*
*   Kernel Pmu module header
*   This file contains functions managing PMU core on CPU RM
*
******************************************************************************/

#include "gpu/gpu.h"
#include "gpu/eng_state.h"
#include "liblogdecode.h"

#define PMU_LOG_BUFFER_MAX_SIZE 0x1000

/*!
 * KernelPmu is a logical abstraction of the GPU Pmu Engine. The
 * Public API of the Pmu Engine is exposed through this object, and any
 * interfaces which do not manage the underlying Pmu hardware can be
 * managed by this object.
 */
#ifdef NVOC_KERN_PMU_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelPmu {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct KernelPmu *__nvoc_pbase_KernelPmu;
    NV_STATUS (*__kpmuConstructEngine__)(struct OBJGPU *, struct KernelPmu *, ENGDESCRIPTOR);
    void (*__kpmuStateDestroy__)(struct OBJGPU *, struct KernelPmu *);
    NV_STATUS (*__kpmuStateInitLocked__)(struct OBJGPU *, struct KernelPmu *);
    NV_STATUS (*__kpmuStateLoad__)(POBJGPU, struct KernelPmu *, NvU32);
    NV_STATUS (*__kpmuStateUnload__)(POBJGPU, struct KernelPmu *, NvU32);
    NV_STATUS (*__kpmuStatePreLoad__)(POBJGPU, struct KernelPmu *, NvU32);
    NV_STATUS (*__kpmuStatePostUnload__)(POBJGPU, struct KernelPmu *, NvU32);
    NV_STATUS (*__kpmuStatePreUnload__)(POBJGPU, struct KernelPmu *, NvU32);
    NV_STATUS (*__kpmuStateInitUnlocked__)(POBJGPU, struct KernelPmu *);
    void (*__kpmuInitMissing__)(POBJGPU, struct KernelPmu *);
    NV_STATUS (*__kpmuStatePreInitLocked__)(POBJGPU, struct KernelPmu *);
    NV_STATUS (*__kpmuStatePreInitUnlocked__)(POBJGPU, struct KernelPmu *);
    NV_STATUS (*__kpmuStatePostLoad__)(POBJGPU, struct KernelPmu *, NvU32);
    NvBool (*__kpmuIsPresent__)(POBJGPU, struct KernelPmu *);
    MEMORY_DESCRIPTOR *pPmuRsvdMemdesc;
    LIBOS_LOG_DECODE logDecode;
    NvU32 printBufSize;
    NvU8 *pPrintBuf;
    void *pLogElf;
    NvU32 logElfSize;
};

#ifndef __NVOC_CLASS_KernelPmu_TYPEDEF__
#define __NVOC_CLASS_KernelPmu_TYPEDEF__
typedef struct KernelPmu KernelPmu;
#endif /* __NVOC_CLASS_KernelPmu_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelPmu
#define __nvoc_class_id_KernelPmu 0xab9d7d
#endif /* __nvoc_class_id_KernelPmu */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelPmu;

#define __staticCast_KernelPmu(pThis) \
    ((pThis)->__nvoc_pbase_KernelPmu)

#ifdef __nvoc_kern_pmu_h_disabled
#define __dynamicCast_KernelPmu(pThis) ((KernelPmu*)NULL)
#else //__nvoc_kern_pmu_h_disabled
#define __dynamicCast_KernelPmu(pThis) \
    ((KernelPmu*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelPmu)))
#endif //__nvoc_kern_pmu_h_disabled

#define PDB_PROP_KPMU_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KPMU_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelPmu(KernelPmu**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelPmu(KernelPmu**, Dynamic*, NvU32);
#define __objCreate_KernelPmu(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelPmu((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define kpmuConstructEngine(pGpu, pKernelPmu, engDesc) kpmuConstructEngine_DISPATCH(pGpu, pKernelPmu, engDesc)
#define kpmuStateDestroy(pGpu, pKernelPmu) kpmuStateDestroy_DISPATCH(pGpu, pKernelPmu)
#define kpmuStateInitLocked(pGpu, pKernelPmu) kpmuStateInitLocked_DISPATCH(pGpu, pKernelPmu)
#define kpmuStateLoad(pGpu, pEngstate, arg0) kpmuStateLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kpmuStateUnload(pGpu, pEngstate, arg0) kpmuStateUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kpmuStatePreLoad(pGpu, pEngstate, arg0) kpmuStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kpmuStatePostUnload(pGpu, pEngstate, arg0) kpmuStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kpmuStatePreUnload(pGpu, pEngstate, arg0) kpmuStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kpmuStateInitUnlocked(pGpu, pEngstate) kpmuStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kpmuInitMissing(pGpu, pEngstate) kpmuInitMissing_DISPATCH(pGpu, pEngstate)
#define kpmuStatePreInitLocked(pGpu, pEngstate) kpmuStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kpmuStatePreInitUnlocked(pGpu, pEngstate) kpmuStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kpmuStatePostLoad(pGpu, pEngstate, arg0) kpmuStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kpmuIsPresent(pGpu, pEngstate) kpmuIsPresent_DISPATCH(pGpu, pEngstate)
NV_STATUS kpmuConstructEngine_IMPL(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu, ENGDESCRIPTOR engDesc);

static inline NV_STATUS kpmuConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu, ENGDESCRIPTOR engDesc) {
    return pKernelPmu->__kpmuConstructEngine__(pGpu, pKernelPmu, engDesc);
}

void kpmuStateDestroy_IMPL(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu);

static inline void kpmuStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu) {
    pKernelPmu->__kpmuStateDestroy__(pGpu, pKernelPmu);
}

NV_STATUS kpmuStateInitLocked_IMPL(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu);

static inline NV_STATUS kpmuStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu) {
    return pKernelPmu->__kpmuStateInitLocked__(pGpu, pKernelPmu);
}

static inline NV_STATUS kpmuStateLoad_DISPATCH(POBJGPU pGpu, struct KernelPmu *pEngstate, NvU32 arg0) {
    return pEngstate->__kpmuStateLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kpmuStateUnload_DISPATCH(POBJGPU pGpu, struct KernelPmu *pEngstate, NvU32 arg0) {
    return pEngstate->__kpmuStateUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kpmuStatePreLoad_DISPATCH(POBJGPU pGpu, struct KernelPmu *pEngstate, NvU32 arg0) {
    return pEngstate->__kpmuStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kpmuStatePostUnload_DISPATCH(POBJGPU pGpu, struct KernelPmu *pEngstate, NvU32 arg0) {
    return pEngstate->__kpmuStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kpmuStatePreUnload_DISPATCH(POBJGPU pGpu, struct KernelPmu *pEngstate, NvU32 arg0) {
    return pEngstate->__kpmuStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kpmuStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelPmu *pEngstate) {
    return pEngstate->__kpmuStateInitUnlocked__(pGpu, pEngstate);
}

static inline void kpmuInitMissing_DISPATCH(POBJGPU pGpu, struct KernelPmu *pEngstate) {
    pEngstate->__kpmuInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kpmuStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct KernelPmu *pEngstate) {
    return pEngstate->__kpmuStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kpmuStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelPmu *pEngstate) {
    return pEngstate->__kpmuStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kpmuStatePostLoad_DISPATCH(POBJGPU pGpu, struct KernelPmu *pEngstate, NvU32 arg0) {
    return pEngstate->__kpmuStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NvBool kpmuIsPresent_DISPATCH(POBJGPU pGpu, struct KernelPmu *pEngstate) {
    return pEngstate->__kpmuIsPresent__(pGpu, pEngstate);
}

void kpmuDestruct_IMPL(struct KernelPmu *pKernelPmu);

#define __nvoc_kpmuDestruct(pKernelPmu) kpmuDestruct_IMPL(pKernelPmu)
NvU32 kpmuReservedMemorySizeGet_IMPL(struct KernelPmu *pKernelPmu);

#ifdef __nvoc_kern_pmu_h_disabled
static inline NvU32 kpmuReservedMemorySizeGet(struct KernelPmu *pKernelPmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelPmu was disabled!");
    return 0;
}
#else //__nvoc_kern_pmu_h_disabled
#define kpmuReservedMemorySizeGet(pKernelPmu) kpmuReservedMemorySizeGet_IMPL(pKernelPmu)
#endif //__nvoc_kern_pmu_h_disabled

NvU64 kpmuReservedMemoryOffsetGet_IMPL(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu);

#ifdef __nvoc_kern_pmu_h_disabled
static inline NvU64 kpmuReservedMemoryOffsetGet(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelPmu was disabled!");
    return 0;
}
#else //__nvoc_kern_pmu_h_disabled
#define kpmuReservedMemoryOffsetGet(pGpu, pKernelPmu) kpmuReservedMemoryOffsetGet_IMPL(pGpu, pKernelPmu)
#endif //__nvoc_kern_pmu_h_disabled

NV_STATUS kpmuInitLibosLoggingStructures_IMPL(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu);

#ifdef __nvoc_kern_pmu_h_disabled
static inline NV_STATUS kpmuInitLibosLoggingStructures(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelPmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kern_pmu_h_disabled
#define kpmuInitLibosLoggingStructures(pGpu, pKernelPmu) kpmuInitLibosLoggingStructures_IMPL(pGpu, pKernelPmu)
#endif //__nvoc_kern_pmu_h_disabled

void kpmuFreeLibosLoggingStructures_IMPL(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu);

#ifdef __nvoc_kern_pmu_h_disabled
static inline void kpmuFreeLibosLoggingStructures(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelPmu was disabled!");
}
#else //__nvoc_kern_pmu_h_disabled
#define kpmuFreeLibosLoggingStructures(pGpu, pKernelPmu) kpmuFreeLibosLoggingStructures_IMPL(pGpu, pKernelPmu)
#endif //__nvoc_kern_pmu_h_disabled

void kpmuLogBuf_IMPL(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu, NvU8 *pBuf, NvU32 bufSize);

#ifdef __nvoc_kern_pmu_h_disabled
static inline void kpmuLogBuf(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu, NvU8 *pBuf, NvU32 bufSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelPmu was disabled!");
}
#else //__nvoc_kern_pmu_h_disabled
#define kpmuLogBuf(pGpu, pKernelPmu, pBuf, bufSize) kpmuLogBuf_IMPL(pGpu, pKernelPmu, pBuf, bufSize)
#endif //__nvoc_kern_pmu_h_disabled

#undef PRIVATE_FIELD


#endif // KERNEL_PMU_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERN_PMU_NVOC_H_
