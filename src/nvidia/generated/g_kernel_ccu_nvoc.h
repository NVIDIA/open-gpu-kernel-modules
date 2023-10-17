#ifndef _G_KERNEL_CCU_NVOC_H_
#define _G_KERNEL_CCU_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "rmapi/rmapi.h"
#include "objtmr.h"
#include "ctrl/ctrlcbca.h"
#include "ctrl/ctrl2080/ctrl2080internal.h"

/*!
 * KernelCcu is a logical abstraction of the GPU Ccu Engine. The
 * Public API of the Ccu Engine is exposed through this object, and any
 * interfaces which do not manage the underlying Ccu can be managed by
 * this object.
 */

#define CCU_PER_GPU_COUNTER_Q_SIZE     (16384)
#define CCU_TIMESTAMP_SIZE             (sizeof(NvU64))
#define CCU_TIMEOUT_100MSECS           (100000000)
#define CCU_MIG_INST_COUNTER_Q_SIZE    (2048)

#define CCU_DEV_SHRBUF_ID       (0)
#define CCU_MIG_SHRBUF_ID_START (1)

#define CCU_DEV_SHRBUF_COUNT    (1)
#define CCU_SHRBUF_COUNT_MAX    (GPUMGR_MAX_GPU_INSTANCES + CCU_DEV_SHRBUF_COUNT)
#define CCU_MIG_SWIZZID_SIZE    (sizeof(NvU8))
#define CCU_MIG_INVALID_SWIZZID (0xFF)
#define CCU_MIG_COMPUTEID_SIZE  (sizeof(NvU8))
#define CCU_MIG_INVALID_COMPUTEID (0xFF)

#define CCU_DEV_COUNTER_ENTRY_MAX (2048)
#define CCU_MIG_COUNTER_ENTRY_MAX (256)

#define CCU_STREAM_STATE_DISABLE (0)
#define CCU_STREAM_STATE_ENABLE  (1)

#define CCU_GPU_SHARED_BUFFER_SIZE_MAX (CCU_PER_GPU_COUNTER_Q_SIZE + (2 * CCU_TIMESTAMP_SIZE) + CCU_MIG_SWIZZID_SIZE) // 16K counter block + head & tail timestamp size + mig-SwizzId size
#define CCU_MIG_INST_SHARED_BUFFER_SIZE_MAX (CCU_MIG_INST_COUNTER_Q_SIZE + (2 * CCU_TIMESTAMP_SIZE) + CCU_MIG_SWIZZID_SIZE + CCU_MIG_COMPUTEID_SIZE) // 2K counter block + head & tail timestamp size + mig-SwizzId size + compute-instId size

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

#ifdef NVOC_KERNEL_CCU_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct KernelCcu {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct KernelCcu *__nvoc_pbase_KernelCcu;
    NV_STATUS (*__kccuConstructEngine__)(OBJGPU *, struct KernelCcu *, ENGDESCRIPTOR);
    NV_STATUS (*__kccuStateLoad__)(OBJGPU *, struct KernelCcu *, NvU32);
    NV_STATUS (*__kccuStateUnload__)(OBJGPU *, struct KernelCcu *, NvU32);
    NV_STATUS (*__kccuMigShrBufHandler__)(OBJGPU *, struct KernelCcu *, NvBool);
    NV_STATUS (*__kccuStateInitLocked__)(POBJGPU, struct KernelCcu *);
    NV_STATUS (*__kccuStatePreLoad__)(POBJGPU, struct KernelCcu *, NvU32);
    NV_STATUS (*__kccuStatePostUnload__)(POBJGPU, struct KernelCcu *, NvU32);
    void (*__kccuStateDestroy__)(POBJGPU, struct KernelCcu *);
    NV_STATUS (*__kccuStatePreUnload__)(POBJGPU, struct KernelCcu *, NvU32);
    NV_STATUS (*__kccuStateInitUnlocked__)(POBJGPU, struct KernelCcu *);
    void (*__kccuInitMissing__)(POBJGPU, struct KernelCcu *);
    NV_STATUS (*__kccuStatePreInitLocked__)(POBJGPU, struct KernelCcu *);
    NV_STATUS (*__kccuStatePreInitUnlocked__)(POBJGPU, struct KernelCcu *);
    NV_STATUS (*__kccuStatePostLoad__)(POBJGPU, struct KernelCcu *, NvU32);
    NvBool (*__kccuIsPresent__)(POBJGPU, struct KernelCcu *);
    MEMORY_DESCRIPTOR *pMemDesc[9];
    NvBool bStreamState;
    CCU_SHRBUF shrBuf[9];
    NvBool bMigShrBufAllocated;
};

#ifndef __NVOC_CLASS_KernelCcu_TYPEDEF__
#define __NVOC_CLASS_KernelCcu_TYPEDEF__
typedef struct KernelCcu KernelCcu;
#endif /* __NVOC_CLASS_KernelCcu_TYPEDEF__ */

#ifndef __nvoc_class_id_KernelCcu
#define __nvoc_class_id_KernelCcu 0x5d5b68
#endif /* __nvoc_class_id_KernelCcu */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_KernelCcu;

#define __staticCast_KernelCcu(pThis) \
    ((pThis)->__nvoc_pbase_KernelCcu)

#ifdef __nvoc_kernel_ccu_h_disabled
#define __dynamicCast_KernelCcu(pThis) ((KernelCcu*)NULL)
#else //__nvoc_kernel_ccu_h_disabled
#define __dynamicCast_KernelCcu(pThis) \
    ((KernelCcu*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(KernelCcu)))
#endif //__nvoc_kernel_ccu_h_disabled

#define PDB_PROP_KCCU_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_KCCU_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_KernelCcu(KernelCcu**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_KernelCcu(KernelCcu**, Dynamic*, NvU32);
#define __objCreate_KernelCcu(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_KernelCcu((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define kccuConstructEngine(pGpu, pKernelCcu, engDesc) kccuConstructEngine_DISPATCH(pGpu, pKernelCcu, engDesc)
#define kccuConstructEngine_HAL(pGpu, pKernelCcu, engDesc) kccuConstructEngine_DISPATCH(pGpu, pKernelCcu, engDesc)
#define kccuStateLoad(arg0, arg1, flags) kccuStateLoad_DISPATCH(arg0, arg1, flags)
#define kccuStateUnload(arg0, arg1, flags) kccuStateUnload_DISPATCH(arg0, arg1, flags)
#define kccuMigShrBufHandler(arg0, arg1, bMigEnabled) kccuMigShrBufHandler_DISPATCH(arg0, arg1, bMigEnabled)
#define kccuMigShrBufHandler_HAL(arg0, arg1, bMigEnabled) kccuMigShrBufHandler_DISPATCH(arg0, arg1, bMigEnabled)
#define kccuStateInitLocked(pGpu, pEngstate) kccuStateInitLocked_DISPATCH(pGpu, pEngstate)
#define kccuStatePreLoad(pGpu, pEngstate, arg0) kccuStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kccuStatePostUnload(pGpu, pEngstate, arg0) kccuStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kccuStateDestroy(pGpu, pEngstate) kccuStateDestroy_DISPATCH(pGpu, pEngstate)
#define kccuStatePreUnload(pGpu, pEngstate, arg0) kccuStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define kccuStateInitUnlocked(pGpu, pEngstate) kccuStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kccuInitMissing(pGpu, pEngstate) kccuInitMissing_DISPATCH(pGpu, pEngstate)
#define kccuStatePreInitLocked(pGpu, pEngstate) kccuStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define kccuStatePreInitUnlocked(pGpu, pEngstate) kccuStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define kccuStatePostLoad(pGpu, pEngstate, arg0) kccuStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define kccuIsPresent(pGpu, pEngstate) kccuIsPresent_DISPATCH(pGpu, pEngstate)
void kccuDestruct_IMPL(struct KernelCcu *arg0);


#define __nvoc_kccuDestruct(arg0) kccuDestruct_IMPL(arg0)
NV_STATUS kccuConstructEngine_IMPL(OBJGPU *pGpu, struct KernelCcu *pKernelCcu, ENGDESCRIPTOR engDesc);

static inline NV_STATUS kccuConstructEngine_DISPATCH(OBJGPU *pGpu, struct KernelCcu *pKernelCcu, ENGDESCRIPTOR engDesc) {
    return pKernelCcu->__kccuConstructEngine__(pGpu, pKernelCcu, engDesc);
}

NV_STATUS kccuStateLoad_IMPL(OBJGPU *arg0, struct KernelCcu *arg1, NvU32 flags);

static inline NV_STATUS kccuStateLoad_DISPATCH(OBJGPU *arg0, struct KernelCcu *arg1, NvU32 flags) {
    return arg1->__kccuStateLoad__(arg0, arg1, flags);
}

NV_STATUS kccuStateUnload_IMPL(OBJGPU *arg0, struct KernelCcu *arg1, NvU32 flags);

static inline NV_STATUS kccuStateUnload_DISPATCH(OBJGPU *arg0, struct KernelCcu *arg1, NvU32 flags) {
    return arg1->__kccuStateUnload__(arg0, arg1, flags);
}

static inline NV_STATUS kccuMigShrBufHandler_46f6a7(OBJGPU *arg0, struct KernelCcu *arg1, NvBool bMigEnabled) {
    return NV_ERR_NOT_SUPPORTED;
}

NV_STATUS kccuMigShrBufHandler_GH100(OBJGPU *arg0, struct KernelCcu *arg1, NvBool bMigEnabled);

static inline NV_STATUS kccuMigShrBufHandler_DISPATCH(OBJGPU *arg0, struct KernelCcu *arg1, NvBool bMigEnabled) {
    return arg1->__kccuMigShrBufHandler__(arg0, arg1, bMigEnabled);
}

static inline NV_STATUS kccuStateInitLocked_DISPATCH(POBJGPU pGpu, struct KernelCcu *pEngstate) {
    return pEngstate->__kccuStateInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kccuStatePreLoad_DISPATCH(POBJGPU pGpu, struct KernelCcu *pEngstate, NvU32 arg0) {
    return pEngstate->__kccuStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kccuStatePostUnload_DISPATCH(POBJGPU pGpu, struct KernelCcu *pEngstate, NvU32 arg0) {
    return pEngstate->__kccuStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline void kccuStateDestroy_DISPATCH(POBJGPU pGpu, struct KernelCcu *pEngstate) {
    pEngstate->__kccuStateDestroy__(pGpu, pEngstate);
}

static inline NV_STATUS kccuStatePreUnload_DISPATCH(POBJGPU pGpu, struct KernelCcu *pEngstate, NvU32 arg0) {
    return pEngstate->__kccuStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS kccuStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelCcu *pEngstate) {
    return pEngstate->__kccuStateInitUnlocked__(pGpu, pEngstate);
}

static inline void kccuInitMissing_DISPATCH(POBJGPU pGpu, struct KernelCcu *pEngstate) {
    pEngstate->__kccuInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS kccuStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct KernelCcu *pEngstate) {
    return pEngstate->__kccuStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS kccuStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct KernelCcu *pEngstate) {
    return pEngstate->__kccuStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS kccuStatePostLoad_DISPATCH(POBJGPU pGpu, struct KernelCcu *pEngstate, NvU32 arg0) {
    return pEngstate->__kccuStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NvBool kccuIsPresent_DISPATCH(POBJGPU pGpu, struct KernelCcu *pEngstate) {
    return pEngstate->__kccuIsPresent__(pGpu, pEngstate);
}

NV_STATUS kccuMemDescGetForSwizzId_IMPL(OBJGPU *arg0, struct KernelCcu *arg1, NvU8 swizzId, MEMORY_DESCRIPTOR **arg2);

#ifdef __nvoc_kernel_ccu_h_disabled
static inline NV_STATUS kccuMemDescGetForSwizzId(OBJGPU *arg0, struct KernelCcu *arg1, NvU8 swizzId, MEMORY_DESCRIPTOR **arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ccu_h_disabled
#define kccuMemDescGetForSwizzId(arg0, arg1, swizzId, arg2) kccuMemDescGetForSwizzId_IMPL(arg0, arg1, swizzId, arg2)
#endif //__nvoc_kernel_ccu_h_disabled

NV_STATUS kccuMemDescGetForShrBufId_IMPL(OBJGPU *arg0, struct KernelCcu *arg1, NvU32 shrbufId, MEMORY_DESCRIPTOR **arg2);

#ifdef __nvoc_kernel_ccu_h_disabled
static inline NV_STATUS kccuMemDescGetForShrBufId(OBJGPU *arg0, struct KernelCcu *arg1, NvU32 shrbufId, MEMORY_DESCRIPTOR **arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ccu_h_disabled
#define kccuMemDescGetForShrBufId(arg0, arg1, shrbufId, arg2) kccuMemDescGetForShrBufId_IMPL(arg0, arg1, shrbufId, arg2)
#endif //__nvoc_kernel_ccu_h_disabled

NvU32 kccuCounterBlockSizeGet_IMPL(OBJGPU *arg0, struct KernelCcu *arg1, NvBool bDevCounter);

#ifdef __nvoc_kernel_ccu_h_disabled
static inline NvU32 kccuCounterBlockSizeGet(OBJGPU *arg0, struct KernelCcu *arg1, NvBool bDevCounter) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcu was disabled!");
    return 0;
}
#else //__nvoc_kernel_ccu_h_disabled
#define kccuCounterBlockSizeGet(arg0, arg1, bDevCounter) kccuCounterBlockSizeGet_IMPL(arg0, arg1, bDevCounter)
#endif //__nvoc_kernel_ccu_h_disabled

NV_STATUS kccuStreamStateSet_IMPL(OBJGPU *arg0, struct KernelCcu *arg1, NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *arg2);

#ifdef __nvoc_kernel_ccu_h_disabled
static inline NV_STATUS kccuStreamStateSet(OBJGPU *arg0, struct KernelCcu *arg1, NV_COUNTER_COLLECTION_UNIT_STREAM_STATE_PARAMS *arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ccu_h_disabled
#define kccuStreamStateSet(arg0, arg1, arg2) kccuStreamStateSet_IMPL(arg0, arg1, arg2)
#endif //__nvoc_kernel_ccu_h_disabled

NvBool kccuStreamStateGet_IMPL(OBJGPU *arg0, struct KernelCcu *arg1);

#ifdef __nvoc_kernel_ccu_h_disabled
static inline NvBool kccuStreamStateGet(OBJGPU *arg0, struct KernelCcu *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcu was disabled!");
    return NV_FALSE;
}
#else //__nvoc_kernel_ccu_h_disabled
#define kccuStreamStateGet(arg0, arg1) kccuStreamStateGet_IMPL(arg0, arg1)
#endif //__nvoc_kernel_ccu_h_disabled

NV_STATUS kccuInitMigSharedBuffer_IMPL(OBJGPU *arg0, struct KernelCcu *arg1);

#ifdef __nvoc_kernel_ccu_h_disabled
static inline NV_STATUS kccuInitMigSharedBuffer(OBJGPU *arg0, struct KernelCcu *arg1) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ccu_h_disabled
#define kccuInitMigSharedBuffer(arg0, arg1) kccuInitMigSharedBuffer_IMPL(arg0, arg1)
#endif //__nvoc_kernel_ccu_h_disabled

NV_STATUS kccuShrBufInfoToCcu_IMPL(OBJGPU *arg0, struct KernelCcu *arg1, NvU32 shrBufStartIdx);

#ifdef __nvoc_kernel_ccu_h_disabled
static inline NV_STATUS kccuShrBufInfoToCcu(OBJGPU *arg0, struct KernelCcu *arg1, NvU32 shrBufStartIdx) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ccu_h_disabled
#define kccuShrBufInfoToCcu(arg0, arg1, shrBufStartIdx) kccuShrBufInfoToCcu_IMPL(arg0, arg1, shrBufStartIdx)
#endif //__nvoc_kernel_ccu_h_disabled

void kccuShrBufIdxCleanup_IMPL(OBJGPU *arg0, struct KernelCcu *arg1, NvU32 shrBufIdx);

#ifdef __nvoc_kernel_ccu_h_disabled
static inline void kccuShrBufIdxCleanup(OBJGPU *arg0, struct KernelCcu *arg1, NvU32 shrBufIdx) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcu was disabled!");
}
#else //__nvoc_kernel_ccu_h_disabled
#define kccuShrBufIdxCleanup(arg0, arg1, shrBufIdx) kccuShrBufIdxCleanup_IMPL(arg0, arg1, shrBufIdx)
#endif //__nvoc_kernel_ccu_h_disabled

NV_STATUS kccuMemDescGetForComputeInst_IMPL(OBJGPU *arg0, struct KernelCcu *arg1, NvU8 swizzId, NvU8 computeId, MEMORY_DESCRIPTOR **arg2);

#ifdef __nvoc_kernel_ccu_h_disabled
static inline NV_STATUS kccuMemDescGetForComputeInst(OBJGPU *arg0, struct KernelCcu *arg1, NvU8 swizzId, NvU8 computeId, MEMORY_DESCRIPTOR **arg2) {
    NV_ASSERT_FAILED_PRECOMP("KernelCcu was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_kernel_ccu_h_disabled
#define kccuMemDescGetForComputeInst(arg0, arg1, swizzId, computeId, arg2) kccuMemDescGetForComputeInst_IMPL(arg0, arg1, swizzId, computeId, arg2)
#endif //__nvoc_kernel_ccu_h_disabled

#undef PRIVATE_FIELD


#endif // KERNEL_CCU_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_KERNEL_CCU_NVOC_H_
