
#ifndef _G_KERN_PMU_NVOC_H_
#define _G_KERN_PMU_NVOC_H_

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
#include "g_kern_pmu_nvoc.h"

#ifndef KERNEL_PMU_H
#define KERNEL_PMU_H

/******************************************************************************
*
*   Kernel Pmu module header
*   This file contains functions managing PMU core on CPU RM
*
******************************************************************************/

#include "rmconfig.h"
#include "nvrm_registry.h"
#include "gpu/gpu.h"
#include "gpu/eng_state.h"
#include "liblogdecode.h"

#define PMU_LOG_BUFFER_MAX_SIZE 0x1000

/*!
 * Alignment to use for reserved memory region
 */
#define KPMU_RESERVED_MEMORY_ALIGNMENT \
    (128U * (1U << 10U))

/*!
 * KernelPmu is a logical abstraction of the GPU Pmu Engine. The
 * Public API of the Pmu Engine is exposed through this object, and any
 * interfaces which do not manage the underlying Pmu hardware can be
 * managed by this object.
 */

// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERN_PMU_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelPmu;
struct NVOC_METADATA__OBJENGSTATE;


struct KernelPmu {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelPmu *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct KernelPmu *__nvoc_pbase_KernelPmu;    // kpmu

    // Vtable with 2 per-object function pointers
    NvU32 (*__kpmuReservedMemoryBackingStoreSizeGet__)(struct KernelPmu * /*this*/);  // halified (3 hals) body
    NvU64 (*__kpmuComputeReservedMemoryOffset__)(struct OBJGPU *, struct KernelPmu * /*this*/, NvU64);  // halified (2 hals) body

    // 1 PDB property
//  NvBool PDB_PROP_KPMU_IS_MISSING inherited from OBJENGSTATE

    // Data members
    LIBOS_LOG_DECODE logDecode;
    NvU32 printBufSize;
    NvU8 *pPrintBuf;
    void *pLogElf;
    NvU32 logElfSize;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__KernelPmu {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
};

#ifndef __nvoc_class_id_KernelPmu
#define __nvoc_class_id_KernelPmu 0xab9d7du
typedef struct KernelPmu KernelPmu;
#endif /* __nvoc_class_id_KernelPmu */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelPmu;

#define __staticCast_KernelPmu(pThis) \
    ((pThis)->__nvoc_pbase_KernelPmu)

#ifdef __nvoc_kern_pmu_h_disabled
#define __dynamicCast_KernelPmu(pThis) ((KernelPmu*) NULL)
#else //__nvoc_kern_pmu_h_disabled
#define __dynamicCast_KernelPmu(pThis) \
    ((KernelPmu*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelPmu)))
#endif //__nvoc_kern_pmu_h_disabled

// Property macros
#define PDB_PROP_KPMU_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KPMU_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING


NV_STATUS __nvoc_objCreateDynamic_KernelPmu(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelPmu(KernelPmu**, Dynamic*, NvU32);
#define __objCreate_KernelPmu(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags) \
    __nvoc_objCreate_KernelPmu((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags))


// Wrapper macros for implementation functions
void kpmuDestruct_IMPL(struct KernelPmu *pKernelPmu);
#define __nvoc_kpmuDestruct(pKernelPmu) kpmuDestruct_IMPL(pKernelPmu)

NvU32 kpmuReservedMemorySizeGet_IMPL(struct KernelPmu *pKernelPmu);
#ifdef __nvoc_kern_pmu_h_disabled
static inline NvU32 kpmuReservedMemorySizeGet(struct KernelPmu *pKernelPmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelPmu was disabled!");
    return 0;
}
#else // __nvoc_kern_pmu_h_disabled
#define kpmuReservedMemorySizeGet(pKernelPmu) kpmuReservedMemorySizeGet_IMPL(pKernelPmu)
#endif // __nvoc_kern_pmu_h_disabled

NV_STATUS kpmuInitLibosLoggingStructures_IMPL(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu);
#ifdef __nvoc_kern_pmu_h_disabled
static inline NV_STATUS kpmuInitLibosLoggingStructures(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelPmu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_kern_pmu_h_disabled
#define kpmuInitLibosLoggingStructures(pGpu, pKernelPmu) kpmuInitLibosLoggingStructures_IMPL(pGpu, pKernelPmu)
#endif // __nvoc_kern_pmu_h_disabled

void kpmuFreeLibosLoggingStructures_IMPL(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu);
#ifdef __nvoc_kern_pmu_h_disabled
static inline void kpmuFreeLibosLoggingStructures(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu) {
    NV_ASSERT_FAILED_PRECOMP("KernelPmu was disabled!");
}
#else // __nvoc_kern_pmu_h_disabled
#define kpmuFreeLibosLoggingStructures(pGpu, pKernelPmu) kpmuFreeLibosLoggingStructures_IMPL(pGpu, pKernelPmu)
#endif // __nvoc_kern_pmu_h_disabled

void kpmuLogBuf_IMPL(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu, NvU8 *pBuf, NvU32 bufSize);
#ifdef __nvoc_kern_pmu_h_disabled
static inline void kpmuLogBuf(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu, NvU8 *pBuf, NvU32 bufSize) {
    NV_ASSERT_FAILED_PRECOMP("KernelPmu was disabled!");
}
#else // __nvoc_kern_pmu_h_disabled
#define kpmuLogBuf(pGpu, pKernelPmu, pBuf, bufSize) kpmuLogBuf_IMPL(pGpu, pKernelPmu, pBuf, bufSize)
#endif // __nvoc_kern_pmu_h_disabled


// Wrapper macros for halified functions
#define kpmuConstructEngine_FNPTR(pKernelPmu) pKernelPmu->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructEngine__
#define kpmuConstructEngine(pGpu, pKernelPmu, engDesc) kpmuConstructEngine_DISPATCH(pGpu, pKernelPmu, engDesc)
#define kpmuStateDestroy_FNPTR(pKernelPmu) pKernelPmu->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateDestroy__
#define kpmuStateDestroy(pGpu, pKernelPmu) kpmuStateDestroy_DISPATCH(pGpu, pKernelPmu)
#define kpmuStateInitLocked_FNPTR(pKernelPmu) pKernelPmu->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__
#define kpmuStateInitLocked(pGpu, pKernelPmu) kpmuStateInitLocked_DISPATCH(pGpu, pKernelPmu)
#define kpmuReservedMemoryBackingStoreSizeGet_FNPTR(pKernelPmu) pKernelPmu->__kpmuReservedMemoryBackingStoreSizeGet__
#define kpmuReservedMemoryBackingStoreSizeGet(pKernelPmu) kpmuReservedMemoryBackingStoreSizeGet_DISPATCH(pKernelPmu)
#define kpmuReservedMemoryBackingStoreSizeGet_HAL(pKernelPmu) kpmuReservedMemoryBackingStoreSizeGet_DISPATCH(pKernelPmu)
#define kpmuComputeReservedMemoryOffset_FNPTR(pKernelPmu) pKernelPmu->__kpmuComputeReservedMemoryOffset__
#define kpmuComputeReservedMemoryOffset(pGpu, pKernelPmu, fbSize) kpmuComputeReservedMemoryOffset_DISPATCH(pGpu, pKernelPmu, fbSize)
#define kpmuComputeReservedMemoryOffset_HAL(pGpu, pKernelPmu, fbSize) kpmuComputeReservedMemoryOffset_DISPATCH(pGpu, pKernelPmu, fbSize)
#define kpmuConstructApertures_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructApertures__
#define kpmuConstructApertures(pGpu, pEngstate) kpmuConstructApertures_DISPATCH(pGpu, pEngstate)
#define kpmuDestructApertures_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateDestructApertures__
#define kpmuDestructApertures(pGpu, pEngstate) kpmuDestructApertures_DISPATCH(pGpu, pEngstate)
#define kpmuInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define kpmuInitMissing(pGpu, pEngstate) kpmuInitMissing_DISPATCH(pGpu, pEngstate)
#define kpmuStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__
#define kpmuStatePreInitLocked(pGpu, pEngstate) kpmuStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kpmuStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define kpmuStatePreInitUnlocked(pGpu, pEngstate) kpmuStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kpmuStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define kpmuStateInitUnlocked(pGpu, pEngstate) kpmuStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kpmuStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define kpmuStatePreLoad(pGpu, pEngstate, arg3) kpmuStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kpmuStateLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateLoad__
#define kpmuStateLoad(pGpu, pEngstate, arg3) kpmuStateLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kpmuStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__
#define kpmuStatePostLoad(pGpu, pEngstate, arg3) kpmuStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kpmuStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__
#define kpmuStatePreUnload(pGpu, pEngstate, arg3) kpmuStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kpmuStateUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateUnload__
#define kpmuStateUnload(pGpu, pEngstate, arg3) kpmuStateUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kpmuStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define kpmuStatePostUnload(pGpu, pEngstate, arg3) kpmuStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kpmuIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define kpmuIsPresent(pGpu, pEngstate) kpmuIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS kpmuConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu, ENGDESCRIPTOR engDesc) {
    return pKernelPmu->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructEngine__(pGpu, staticCast(pKernelPmu, OBJENGSTATE), engDesc);
}

static inline void kpmuStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu) {
    pKernelPmu->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateDestroy__(pGpu, staticCast(pKernelPmu, OBJENGSTATE));
}

static inline NV_STATUS kpmuStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu) {
    return pKernelPmu->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__(pGpu, staticCast(pKernelPmu, OBJENGSTATE));
}

static inline NvU32 kpmuReservedMemoryBackingStoreSizeGet_DISPATCH(struct KernelPmu *pKernelPmu) {
    return pKernelPmu->__kpmuReservedMemoryBackingStoreSizeGet__(pKernelPmu);
}

static inline NvU64 kpmuComputeReservedMemoryOffset_DISPATCH(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu, NvU64 fbSize) {
    return pKernelPmu->__kpmuComputeReservedMemoryOffset__(pGpu, pKernelPmu, fbSize);
}

static inline NV_STATUS kpmuConstructApertures_DISPATCH(struct OBJGPU *pGpu, struct KernelPmu *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateConstructApertures__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline void kpmuDestructApertures_DISPATCH(struct OBJGPU *pGpu, struct KernelPmu *pEngstate) {
    pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateDestructApertures__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline void kpmuInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelPmu *pEngstate) {
    pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS kpmuStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelPmu *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS kpmuStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelPmu *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS kpmuStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelPmu *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

static inline NV_STATUS kpmuStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS kpmuStateLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateLoad__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS kpmuStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS kpmuStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS kpmuStateUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateUnload__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NV_STATUS kpmuStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelPmu *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__(pGpu, staticCast(pEngstate, OBJENGSTATE), arg3);
}

static inline NvBool kpmuIsPresent_DISPATCH(struct OBJGPU *pGpu, struct KernelPmu *pEngstate) {
    return pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__(pGpu, staticCast(pEngstate, OBJENGSTATE));
}

// Virtual method declarations and/or inline definitions
NV_STATUS kpmuConstructEngine_IMPL(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu, ENGDESCRIPTOR engDesc);

void kpmuStateDestroy_IMPL(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu);

NV_STATUS kpmuStateInitLocked_IMPL(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu);

// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
NvU64 kpmuComputeReservedMemoryOffset_GB100(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu, NvU64 fbSize);

// Inline HAL method definitions
static inline NvU32 kpmuReservedMemoryBackingStoreSizeGet_3c0c39(struct KernelPmu *pKernelPmu){
    return 10485760;
}

static inline NvU32 kpmuReservedMemoryBackingStoreSizeGet_bdab1d(struct KernelPmu *pKernelPmu){
    return 9437184;
}

static inline NvU32 kpmuReservedMemoryBackingStoreSizeGet_b2b553(struct KernelPmu *pKernelPmu){
    return 0;
}

static inline NvU64 kpmuComputeReservedMemoryOffset_b2b553(struct OBJGPU *pGpu, struct KernelPmu *pKernelPmu, NvU64 fbSize){
    return 0;
}

// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // KERNEL_PMU_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERN_PMU_NVOC_H_
