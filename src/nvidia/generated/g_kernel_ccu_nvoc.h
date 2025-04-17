
#ifndef _G_KERNEL_CCU_NVOC_H_
#define _G_KERNEL_CCU_NVOC_H_

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
#include "g_kernel_ccu_nvoc.h"

#ifndef KERNEL_CCU_H
#define KERNEL_CCU_H

/******************************************************************************
*
*   Kernel CCU module header
*   This file contains functions managing CCU core on CPU RM
*
******************************************************************************/

#include "gpu/gpu.h"
#include "gpu/eng_state.h"
#include "gpu_mgr/gpu_mgr.h" // GPUMGR_MAX_GPU_INSTANCES
#include "rmapi/rmapi.h"
#include "gpu/timer/objtmr.h"
#include "ctrl/ctrlcbca.h"
#include "ctrl/ctrl2080/ctrl2080internal.h"

/*!
 * KernelCcu is a logical abstraction of the GPU Ccu Engine. The
 * Public API of the Ccu Engine is exposed through this object, and any
 * interfaces which do not manage the underlying Ccu can be managed by
 * this object.
 */
#define CCU_DEV_BUF_SIZE_16KB          (16 * 1024)
#define CCU_MIG_BUF_SIZE_2KB           (2 * 1024)

#define CCU_TIMESTAMP_SIZE             (sizeof(NvU64))
#define CCU_TIMEOUT_100MSECS           (100000000)

#define CCU_DEV_SHRBUF_ID       (0)
#define CCU_MIG_SHRBUF_ID_START (1)

#define CCU_DEV_SHRBUF_COUNT    (1)
#define CCU_SHRBUF_COUNT_MAX    (GPUMGR_MAX_GPU_INSTANCES + CCU_DEV_SHRBUF_COUNT)
#define CCU_MIG_SWIZZID_SIZE    (sizeof(NvU8))
#define CCU_MIG_INVALID_SWIZZID (0xFF)
#define CCU_MIG_COMPUTEID_SIZE  (sizeof(NvU8))
#define CCU_MIG_INVALID_COMPUTEID (0xFF)

#define CCU_STREAM_STATE_DISABLE (0)
#define CCU_STREAM_STATE_ENABLE  (1)

#define CCU_DEV_SHARED_BUF_SIZE (CCU_DEV_BUF_SIZE_16KB + (2 * CCU_TIMESTAMP_SIZE) + CCU_MIG_SWIZZID_SIZE) // 16K counter block + head & tail timestamp size + mig-SwizzId size
#define CCU_MIG_SHARED_BUF_SIZE  (CCU_MIG_BUF_SIZE_2KB + (2 * CCU_TIMESTAMP_SIZE) + CCU_MIG_SWIZZID_SIZE + CCU_MIG_COMPUTEID_SIZE) // 2K counter block + head & tail timestamp size + mig-SwizzId size + compute-instId size

typedef struct
{
    NvP64 addr;                // Kernel address
    NvP64 priv;                // Private page count
} SHARED_BUFFER_MAP_INFO;

typedef struct
{
    /* shared buffer info */
    NvU64 *pHeadTimeStamp;   // Timestamp before the start of counter block
    NvU64 *pTailTimeStamp;   // Timestamp at the end of counter block
    NvU8  *pSwizzId;         // Mig inst swizz id
    NvU8  *pComputeId;       // Compute inst id
    NvP64 pCounterBlock;     // Counter block start address
    NvU32 counterBlockSize;  // Counter block size
} CCU_SHRBUF_INFO;

typedef struct
{
    CCU_SHRBUF_INFO *pCounterDstInfo;           // Counter shared buffer info
    SHARED_BUFFER_MAP_INFO *pKernelMapInfo;     // Shared buffer kernel map info
} CCU_SHRBUF;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_KERNEL_CCU_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelCcu;
struct NVOC_METADATA__OBJENGSTATE;
struct NVOC_VTABLE__KernelCcu;


struct KernelCcu {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__KernelCcu *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct KernelCcu *__nvoc_pbase_KernelCcu;    // kccu

    // Vtable with 2 per-object function pointers
    NV_STATUS (*__kccuMigShrBufHandler__)(OBJGPU *, struct KernelCcu * /*this*/, NvBool);  // halified (2 hals)
    NV_STATUS (*__kccuGetBufSize__)(OBJGPU *, struct KernelCcu * /*this*/);  // halified (3 hals)

    // 1 PDB property

    // Data members
    MEMORY_DESCRIPTOR *pMemDesc[9];
    NvBool bStreamState;
    CCU_SHRBUF shrBuf[9];
    NvBool bMigShrBufAllocated;
    NvU32 devBufSize;
    NvU32 devSharedBufSize;
    NvU32 migBufSize;
    NvU32 migSharedBufSize;
};


// Vtable with 14 per-class function pointers
struct NVOC_VTABLE__KernelCcu {
    NV_STATUS (*__kccuConstructEngine__)(OBJGPU *, struct KernelCcu * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kccuStateLoad__)(OBJGPU *, struct KernelCcu * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__kccuStateUnload__)(OBJGPU *, struct KernelCcu * /*this*/, NvU32);  // virtual override (engstate) base (engstate)
    void (*__kccuInitMissing__)(struct OBJGPU *, struct KernelCcu * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kccuStatePreInitLocked__)(struct OBJGPU *, struct KernelCcu * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kccuStatePreInitUnlocked__)(struct OBJGPU *, struct KernelCcu * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kccuStateInitLocked__)(struct OBJGPU *, struct KernelCcu * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kccuStateInitUnlocked__)(struct OBJGPU *, struct KernelCcu * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kccuStatePreLoad__)(struct OBJGPU *, struct KernelCcu * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kccuStatePostLoad__)(struct OBJGPU *, struct KernelCcu * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kccuStatePreUnload__)(struct OBJGPU *, struct KernelCcu * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__kccuStatePostUnload__)(struct OBJGPU *, struct KernelCcu * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    void (*__kccuStateDestroy__)(struct OBJGPU *, struct KernelCcu * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__kccuIsPresent__)(struct OBJGPU *, struct KernelCcu * /*this*/);  // virtual inherited (engstate) base (engstate)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__KernelCcu {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
    const struct NVOC_VTABLE__KernelCcu vtable;
};

#ifndef __NVOC_CLASS_KernelCcu_TYPEDEF__
#define __NVOC_CLASS_KernelCcu_TYPEDEF__
typedef struct KernelCcu KernelCcu;
#endif /* __NVOC_CLASS_KernelCcu_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelCcu
#define __nvoc_class_id_KernelCcu 0x5d5b68
#endif /* __nvoc_class_id_KernelCcu */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCcu;

#define __staticCast_KernelCcu(pThis) \
    ((pThis)->__nvoc_pbase_KernelCcu)

#ifdef __nvoc_kernel_ccu_h_disabled
#define __dynamicCast_KernelCcu(pThis) ((KernelCcu*) NULL)
#else //__nvoc_kernel_ccu_h_disabled
#define __dynamicCast_KernelCcu(pThis) \
    ((KernelCcu*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelCcu)))
#endif //__nvoc_kernel_ccu_h_disabled

// Property macros
#define PDB_PROP_KCCU_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KCCU_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelCcu(KernelCcu**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelCcu(KernelCcu**, Dynamic*, NvU32);
#define __objCreate_KernelCcu(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelCcu((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define kccuConstructEngine_FNPTR(pKernelCcu) pKernelCcu->__nvoc_metadata_ptr->vtable.__kccuConstructEngine__
#define kccuConstructEngine(pGpu, pKernelCcu, engDesc) kccuConstructEngine_DISPATCH(pGpu, pKernelCcu, engDesc)
#define kccuStateLoad_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kccuStateLoad__
#define kccuStateLoad(arg1, arg_this, flags) kccuStateLoad_DISPATCH(arg1, arg_this, flags)
#define kccuStateUnload_FNPTR(arg_this) arg_this->__nvoc_metadata_ptr->vtable.__kccuStateUnload__
#define kccuStateUnload(arg1, arg_this, flags) kccuStateUnload_DISPATCH(arg1, arg_this, flags)
#define kccuMigShrBufHandler_FNPTR(arg_this) arg_this->__kccuMigShrBufHandler__
#define kccuMigShrBufHandler(arg1, arg_this, bMigEnabled) kccuMigShrBufHandler_DISPATCH(arg1, arg_this, bMigEnabled)
#define kccuMigShrBufHandler_HAL(arg1, arg_this, bMigEnabled) kccuMigShrBufHandler_DISPATCH(arg1, arg_this, bMigEnabled)
#define kccuGetBufSize_FNPTR(arg_this) arg_this->__kccuGetBufSize__
#define kccuGetBufSize(arg1, arg_this) kccuGetBufSize_DISPATCH(arg1, arg_this)
#define kccuGetBufSize_HAL(arg1, arg_this) kccuGetBufSize_DISPATCH(arg1, arg_this)
#define kccuInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define kccuInitMissing(pGpu, pEngstate) kccuInitMissing_DISPATCH(pGpu, pEngstate)
#define kccuStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__
#define kccuStatePreInitLocked(pGpu, pEngstate) kccuStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kccuStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define kccuStatePreInitUnlocked(pGpu, pEngstate) kccuStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kccuStateInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitLocked__
#define kccuStateInitLocked(pGpu, pEngstate) kccuStateInitLocked_DISPATCH(pGpu, pEngstate)
#define kccuStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define kccuStateInitUnlocked(pGpu, pEngstate) kccuStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kccuStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define kccuStatePreLoad(pGpu, pEngstate, arg3) kccuStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kccuStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__
#define kccuStatePostLoad(pGpu, pEngstate, arg3) kccuStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define kccuStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__
#define kccuStatePreUnload(pGpu, pEngstate, arg3) kccuStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kccuStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define kccuStatePostUnload(pGpu, pEngstate, arg3) kccuStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define kccuStateDestroy_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateDestroy__
#define kccuStateDestroy(pGpu, pEngstate) kccuStateDestroy_DISPATCH(pGpu, pEngstate)
#define kccuIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define kccuIsPresent(pGpu, pEngstate) kccuIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS kccuConstructEngine_DISPATCH(OBJGPU *pGpu, struct KernelCcu *pKernelCcu, ENGDESCRIPTOR engDesc) {
    return pKernelCcu->__nvoc_metadata_ptr->vtable.__kccuConstructEngine__(pGpu, pKernelCcu, engDesc);
}

static inline NV_STATUS kccuStateLoad_DISPATCH(OBJGPU *arg1, struct KernelCcu *arg_this, NvU32 flags) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kccuStateLoad__(arg1, arg_this, flags);
}

static inline NV_STATUS kccuStateUnload_DISPATCH(OBJGPU *arg1, struct KernelCcu *arg_this, NvU32 flags) {
    return arg_this->__nvoc_metadata_ptr->vtable.__kccuStateUnload__(arg1, arg_this, flags);
}

static inline NV_STATUS kccuMigShrBufHandler_DISPATCH(OBJGPU *arg1, struct KernelCcu *arg_this, NvBool bMigEnabled) {
    return arg_this->__kccuMigShrBufHandler__(arg1, arg_this, bMigEnabled);
}

static inline NV_STATUS kccuGetBufSize_DISPATCH(OBJGPU *arg1, struct KernelCcu *arg_this) {
    return arg_this->__kccuGetBufSize__(arg1, arg_this);
}

static inline void kccuInitMissing_DISPATCH(struct OBJGPU *pGpu, struct KernelCcu *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__kccuInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kccuStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelCcu *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kccuStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kccuStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelCcu *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kccuStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kccuStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct KernelCcu *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kccuStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kccuStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct KernelCcu *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kccuStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kccuStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelCcu *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kccuStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kccuStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct KernelCcu *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kccuStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kccuStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelCcu *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kccuStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS kccuStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct KernelCcu *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kccuStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline void kccuStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct KernelCcu *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__kccuStateDestroy__(pGpu, pEngstate);
}

static inline NvBool kccuIsPresent_DISPATCH(struct OBJGPU *pGpu, struct KernelCcu *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__kccuIsPresent__(pGpu, pEngstate);
}

NV_STATUS kccuConstructEngine_IMPL(OBJGPU *pGpu, struct KernelCcu *pKernelCcu, ENGDESCRIPTOR engDesc);

NV_STATUS kccuStateLoad_IMPL(OBJGPU *arg1, struct KernelCcu *arg2, NvU32 flags);

NV_STATUS kccuStateUnload_IMPL(OBJGPU *arg1, struct KernelCcu *arg2, NvU32 flags);

static inline NV_STATUS kccuMigShrBufHandler_46f6a7(OBJGPU *arg1, struct KernelCcu *arg2, NvBool bMigEnabled) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kccuMigShrBufHandler_GH100(OBJGPU *arg1, struct KernelCcu *arg2, NvBool bMigEnabled);

NV_STATUS kccuGetBufSize_GB100(OBJGPU *arg1, struct KernelCcu *arg2);

NV_STATUS kccuGetBufSize_GH100(OBJGPU *arg1, struct KernelCcu *arg2);

static inline NV_STATUS kccuGetBufSize_46f6a7(OBJGPU *arg1, struct KernelCcu *arg2) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kccuMemDescGetForShrBufId_IMPL(OBJGPU *arg1, struct KernelCcu *arg2, NvU32 shrbufId, MEMORY_DESCRIPTOR **arg4);

#ifdef __nvoc_kernel_ccu_h_disabled
static inline NV_STATUS kccuMemDescGetForShrBufId(OBJGPU *arg1, struct KernelCcu *arg2, NvU32 shrbufId, MEMORY_DESCRIPTOR **arg4) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ccu_h_disabled
#define kccuMemDescGetForShrBufId(arg1, arg2, shrbufId, arg4) kccuMemDescGetForShrBufId_IMPL(arg1, arg2, shrbufId, arg4)
#endif //__nvoc_kernel_ccu_h_disabled

NvU32 kccuCounterBlockSizeGet_IMPL(OBJGPU *arg1, struct KernelCcu *arg2, NvBool bDevCounter);

#ifdef __nvoc_kernel_ccu_h_disabled
static inline NvU32 kccuCounterBlockSizeGet(OBJGPU *arg1, struct KernelCcu *arg2, NvBool bDevCounter) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcu was disabled!");
    return 0;
}
#else //__nvoc_kernel_ccu_h_disabled
#define kccuCounterBlockSizeGet(arg1, arg2, bDevCounter) kccuCounterBlockSizeGet_IMPL(arg1, arg2, bDevCounter)
#endif //__nvoc_kernel_ccu_h_disabled

NV_STATUS kccuStreamStateSet_IMPL(OBJGPU *arg1, struct KernelCcu *arg2, NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *arg3);

#ifdef __nvoc_kernel_ccu_h_disabled
static inline NV_STATUS kccuStreamStateSet(OBJGPU *arg1, struct KernelCcu *arg2, NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *arg3) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ccu_h_disabled
#define kccuStreamStateSet(arg1, arg2, arg3) kccuStreamStateSet_IMPL(arg1, arg2, arg3)
#endif //__nvoc_kernel_ccu_h_disabled

NvBool kccuStreamStateGet_IMPL(OBJGPU *arg1, struct KernelCcu *arg2);

#ifdef __nvoc_kernel_ccu_h_disabled
static inline NvBool kccuStreamStateGet(OBJGPU *arg1, struct KernelCcu *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcu was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_ccu_h_disabled
#define kccuStreamStateGet(arg1, arg2) kccuStreamStateGet_IMPL(arg1, arg2)
#endif //__nvoc_kernel_ccu_h_disabled

NV_STATUS kccuInitMigSharedBuffer_IMPL(OBJGPU *arg1, struct KernelCcu *arg2);

#ifdef __nvoc_kernel_ccu_h_disabled
static inline NV_STATUS kccuInitMigSharedBuffer(OBJGPU *arg1, struct KernelCcu *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ccu_h_disabled
#define kccuInitMigSharedBuffer(arg1, arg2) kccuInitMigSharedBuffer_IMPL(arg1, arg2)
#endif //__nvoc_kernel_ccu_h_disabled

NV_STATUS kccuShrBufInfoToCcu_IMPL(OBJGPU *arg1, struct KernelCcu *arg2, NvU32 shrBufStartIdx);

#ifdef __nvoc_kernel_ccu_h_disabled
static inline NV_STATUS kccuShrBufInfoToCcu(OBJGPU *arg1, struct KernelCcu *arg2, NvU32 shrBufStartIdx) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ccu_h_disabled
#define kccuShrBufInfoToCcu(arg1, arg2, shrBufStartIdx) kccuShrBufInfoToCcu_IMPL(arg1, arg2, shrBufStartIdx)
#endif //__nvoc_kernel_ccu_h_disabled

void kccuShrBufIdxCleanup_IMPL(OBJGPU *arg1, struct KernelCcu *arg2, NvU32 shrBufIdx);

#ifdef __nvoc_kernel_ccu_h_disabled
static inline void kccuShrBufIdxCleanup(OBJGPU *arg1, struct KernelCcu *arg2, NvU32 shrBufIdx) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcu was disabled!");
}
#else //__nvoc_kernel_ccu_h_disabled
#define kccuShrBufIdxCleanup(arg1, arg2, shrBufIdx) kccuShrBufIdxCleanup_IMPL(arg1, arg2, shrBufIdx)
#endif //__nvoc_kernel_ccu_h_disabled

NV_STATUS kccuMemDescGetForComputeInst_IMPL(OBJGPU *arg1, struct KernelCcu *arg2, NvU8 swizzId, NvU8 computeId, MEMORY_DESCRIPTOR **arg5);

#ifdef __nvoc_kernel_ccu_h_disabled
static inline NV_STATUS kccuMemDescGetForComputeInst(OBJGPU *arg1, struct KernelCcu *arg2, NvU8 swizzId, NvU8 computeId, MEMORY_DESCRIPTOR **arg5) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ccu_h_disabled
#define kccuMemDescGetForComputeInst(arg1, arg2, swizzId, computeId, arg5) kccuMemDescGetForComputeInst_IMPL(arg1, arg2, swizzId, computeId, arg5)
#endif //__nvoc_kernel_ccu_h_disabled

NV_STATUS kccuInitVgpuMigSharedBuffer_IMPL(OBJGPU *arg1, struct KernelCcu *arg2, NvU32 swizzId, NvU32 computeId);

#ifdef __nvoc_kernel_ccu_h_disabled
static inline NV_STATUS kccuInitVgpuMigSharedBuffer(OBJGPU *arg1, struct KernelCcu *arg2, NvU32 swizzId, NvU32 computeId) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ccu_h_disabled
#define kccuInitVgpuMigSharedBuffer(arg1, arg2, swizzId, computeId) kccuInitVgpuMigSharedBuffer_IMPL(arg1, arg2, swizzId, computeId)
#endif //__nvoc_kernel_ccu_h_disabled

NV_STATUS kccuDeInitVgpuMigSharedBuffer_IMPL(OBJGPU *arg1, struct KernelCcu *arg2, NvU32 swizzId, NvU32 computeId);

#ifdef __nvoc_kernel_ccu_h_disabled
static inline NV_STATUS kccuDeInitVgpuMigSharedBuffer(OBJGPU *arg1, struct KernelCcu *arg2, NvU32 swizzId, NvU32 computeId) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ccu_h_disabled
#define kccuDeInitVgpuMigSharedBuffer(arg1, arg2, swizzId, computeId) kccuDeInitVgpuMigSharedBuffer_IMPL(arg1, arg2, swizzId, computeId)
#endif //__nvoc_kernel_ccu_h_disabled

#undef PRIVATE_FIELD


#endif // KERNEL_CCU_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_CCU_NVOC_H_
