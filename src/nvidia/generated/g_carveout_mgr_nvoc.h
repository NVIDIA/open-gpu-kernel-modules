
#ifndef _G_CARVEOUT_MGR_NVOC_H_
#define _G_CARVEOUT_MGR_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_carveout_mgr_nvoc.h"

#ifndef CARVEOUT_MGR_H
#define CARVEOUT_MGR_H

#include "core/core.h"

typedef struct OBJEHEAP OBJEHEAP;
typedef struct MEMORY_DESCRIPTOR MEMORY_DESCRIPTOR;
struct OBJGPU;

#ifndef __nvoc_class_id_OBJGPU
#define __nvoc_class_id_OBJGPU 0x7ef3cbu
typedef struct OBJGPU OBJGPU;
#endif /* __nvoc_class_id_OBJGPU */


struct CarveoutMgr;

#ifndef __nvoc_class_id_CarveoutMgr
#define __nvoc_class_id_CarveoutMgr 0xe81fb1u
typedef struct CarveoutMgr CarveoutMgr;
#endif /* __nvoc_class_id_CarveoutMgr */




// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_CARVEOUT_MGR_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__CarveoutMgr;
struct NVOC_METADATA__Object;


struct CarveoutMgr {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__CarveoutMgr *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct CarveoutMgr *__nvoc_pbase_CarveoutMgr;    // carveoutmgr

    // Data members
    OBJEHEAP *pHeap;
    PORT_MUTEX *pStateLock;
    NvU64 base;
    NvU64 size;
    NvU32 attachedGpuMask;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__CarveoutMgr {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
};

#ifndef __nvoc_class_id_CarveoutMgr
#define __nvoc_class_id_CarveoutMgr 0xe81fb1u
typedef struct CarveoutMgr CarveoutMgr;
#endif /* __nvoc_class_id_CarveoutMgr */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_CarveoutMgr;

#define __staticCast_CarveoutMgr(pThis) \
    ((pThis)->__nvoc_pbase_CarveoutMgr)

#ifdef __nvoc_carveout_mgr_h_disabled
#define __dynamicCast_CarveoutMgr(pThis) ((CarveoutMgr*) NULL)
#else //__nvoc_carveout_mgr_h_disabled
#define __dynamicCast_CarveoutMgr(pThis) \
    ((CarveoutMgr*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(CarveoutMgr)))
#endif //__nvoc_carveout_mgr_h_disabled

NV_STATUS __nvoc_objCreateDynamic_CarveoutMgr(Dynamic**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_CarveoutMgr(CarveoutMgr**, Dynamic*, NvU32);
#define __objCreate_CarveoutMgr(__nvoc_ppNewObj, __nvoc_pParent, __nvoc_createFlags) \
    __nvoc_objCreate_CarveoutMgr((__nvoc_ppNewObj), staticCast((__nvoc_pParent), Dynamic), (__nvoc_createFlags))


// Wrapper macros for implementation functions
NV_STATUS carveoutmgrConstruct_IMPL(struct CarveoutMgr *arg_this);
#define __nvoc_carveoutmgrConstruct(arg_this) carveoutmgrConstruct_IMPL(arg_this)

void carveoutmgrDestruct_IMPL(struct CarveoutMgr *arg_this);
#define __nvoc_carveoutmgrDestruct(arg_this) carveoutmgrDestruct_IMPL(arg_this)

NV_STATUS carveoutmgrAcquireHeap_IMPL(struct CarveoutMgr *arg_this, struct OBJGPU *arg2, NvU64 arg3, NvU64 arg4);
#ifdef __nvoc_carveout_mgr_h_disabled
static inline NV_STATUS carveoutmgrAcquireHeap(struct CarveoutMgr *arg_this, struct OBJGPU *arg2, NvU64 arg3, NvU64 arg4) {
    NV_ASSERT_FAILED_PRECOMP("CarveoutMgr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_carveout_mgr_h_disabled
#define carveoutmgrAcquireHeap(arg_this, arg2, arg3, arg4) carveoutmgrAcquireHeap_IMPL(arg_this, arg2, arg3, arg4)
#endif // __nvoc_carveout_mgr_h_disabled

void carveoutmgrReleaseHeap_IMPL(struct CarveoutMgr *arg_this, struct OBJGPU *arg2);
#ifdef __nvoc_carveout_mgr_h_disabled
static inline void carveoutmgrReleaseHeap(struct CarveoutMgr *arg_this, struct OBJGPU *arg2) {
    NV_ASSERT_FAILED_PRECOMP("CarveoutMgr was disabled!");
}
#else // __nvoc_carveout_mgr_h_disabled
#define carveoutmgrReleaseHeap(arg_this, arg2) carveoutmgrReleaseHeap_IMPL(arg_this, arg2)
#endif // __nvoc_carveout_mgr_h_disabled

NvBool carveoutmgrIsGpuAttached_IMPL(struct CarveoutMgr *arg_this, struct OBJGPU *arg2);
#ifdef __nvoc_carveout_mgr_h_disabled
static inline NvBool carveoutmgrIsGpuAttached(struct CarveoutMgr *arg_this, struct OBJGPU *arg2) {
    NV_ASSERT_FAILED_PRECOMP("CarveoutMgr was disabled!");
    return NV_FALSE;
}
#else // __nvoc_carveout_mgr_h_disabled
#define carveoutmgrIsGpuAttached(arg_this, arg2) carveoutmgrIsGpuAttached_IMPL(arg_this, arg2)
#endif // __nvoc_carveout_mgr_h_disabled

NV_STATUS carveoutmgrGetHeapInfo_IMPL(struct CarveoutMgr *arg_this, NvU64 *arg2, NvU64 *arg3);
#ifdef __nvoc_carveout_mgr_h_disabled
static inline NV_STATUS carveoutmgrGetHeapInfo(struct CarveoutMgr *arg_this, NvU64 *arg2, NvU64 *arg3) {
    NV_ASSERT_FAILED_PRECOMP("CarveoutMgr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_carveout_mgr_h_disabled
#define carveoutmgrGetHeapInfo(arg_this, arg2, arg3) carveoutmgrGetHeapInfo_IMPL(arg_this, arg2, arg3)
#endif // __nvoc_carveout_mgr_h_disabled

NV_STATUS carveoutmgrAllocHeapBlock_IMPL(struct CarveoutMgr *arg_this, struct OBJGPU *arg2, NvU32 arg3, NvU32 *arg4, NvU64 *arg5, NvU64 *arg6, NvU64 arg7, NvU64 arg8, NvU64 arg9);
#ifdef __nvoc_carveout_mgr_h_disabled
static inline NV_STATUS carveoutmgrAllocHeapBlock(struct CarveoutMgr *arg_this, struct OBJGPU *arg2, NvU32 arg3, NvU32 *arg4, NvU64 *arg5, NvU64 *arg6, NvU64 arg7, NvU64 arg8, NvU64 arg9) {
    NV_ASSERT_FAILED_PRECOMP("CarveoutMgr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_carveout_mgr_h_disabled
#define carveoutmgrAllocHeapBlock(arg_this, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) carveoutmgrAllocHeapBlock_IMPL(arg_this, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9)
#endif // __nvoc_carveout_mgr_h_disabled

NV_STATUS carveoutmgrDuplicateHeapBlock_IMPL(struct CarveoutMgr *arg_this, struct OBJGPU *arg2, NvU64 arg3);
#ifdef __nvoc_carveout_mgr_h_disabled
static inline NV_STATUS carveoutmgrDuplicateHeapBlock(struct CarveoutMgr *arg_this, struct OBJGPU *arg2, NvU64 arg3) {
    NV_ASSERT_FAILED_PRECOMP("CarveoutMgr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_carveout_mgr_h_disabled
#define carveoutmgrDuplicateHeapBlock(arg_this, arg2, arg3) carveoutmgrDuplicateHeapBlock_IMPL(arg_this, arg2, arg3)
#endif // __nvoc_carveout_mgr_h_disabled

NV_STATUS carveoutmgrFreeHeapBlock_IMPL(struct CarveoutMgr *arg_this, NvU64 arg2);
#ifdef __nvoc_carveout_mgr_h_disabled
static inline NV_STATUS carveoutmgrFreeHeapBlock(struct CarveoutMgr *arg_this, NvU64 arg2) {
    NV_ASSERT_FAILED_PRECOMP("CarveoutMgr was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else // __nvoc_carveout_mgr_h_disabled
#define carveoutmgrFreeHeapBlock(arg_this, arg2) carveoutmgrFreeHeapBlock_IMPL(arg_this, arg2)
#endif // __nvoc_carveout_mgr_h_disabled

void carveoutmgrReleaseMemDesc_IMPL(struct CarveoutMgr *arg_this, MEMORY_DESCRIPTOR *arg2);
#ifdef __nvoc_carveout_mgr_h_disabled
static inline void carveoutmgrReleaseMemDesc(struct CarveoutMgr *arg_this, MEMORY_DESCRIPTOR *arg2) {
    NV_ASSERT_FAILED_PRECOMP("CarveoutMgr was disabled!");
}
#else // __nvoc_carveout_mgr_h_disabled
#define carveoutmgrReleaseMemDesc(arg_this, arg2) carveoutmgrReleaseMemDesc_IMPL(arg_this, arg2)
#endif // __nvoc_carveout_mgr_h_disabled


// Wrapper macros for halified functions

// Dispatch functions
// Virtual method declarations and/or inline definitions
// Exported method declarations and/or inline definitions
// HAL method declarations without bodies
// Inline HAL method definitions
// Static dispatch method declarations
// Static inline method definitions
#undef PRIVATE_FIELD


#endif // CARVEOUT_MGR_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_CARVEOUT_MGR_NVOC_H_
