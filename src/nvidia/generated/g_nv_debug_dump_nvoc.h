
#ifndef _G_NV_DEBUG_DUMP_NVOC_H_
#define _G_NV_DEBUG_DUMP_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 1993-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_nv_debug_dump_nvoc.h"

#ifndef _NV_DEBUG_DUMP_H_
#define _NV_DEBUG_DUMP_H_

#include "gpu/eng_state.h"
#include "gpu/mem_mgr/mem_desc.h"

#include "nvdump.h"

#include "lib/protobuf/prb.h"
#include "rmapi/control.h"
#include "gpu/gpu.h"

// Os Independent Error Types
typedef enum
{
    NVD_SKIP_ZERO,
    NVD_GPU_HUNG,
    NVD_FAILURE_TO_RECOVER,
    NVD_MACHINE_CHECK,
    NVD_POWERUP_FAILURE,
    NVD_CPU_EXCEPTION,
    NVD_EXTERNALLY_GENERATED,
    NVD_GPU_GENERATED,
} NVD_ERROR_TYPE;

#define NV_NVD_ERROR_CODE_MAJOR            31:16
#define NV_NVD_ERROR_CODE_MINOR            15:0

#define NVD_ERROR_CODE(Major, Minor)             \
    (DRF_NUM(_NVD, _ERROR_CODE, _MAJOR, Major) | \
     DRF_NUM(_NVD, _ERROR_CODE, _MINOR, Minor))


#define NVD_ENGINE_FLAGS_PRIORITY                       1:0
#define NVD_ENGINE_FLAGS_PRIORITY_LOW                   0x00000000
#define NVD_ENGINE_FLAGS_PRIORITY_MED                   0x00000001
#define NVD_ENGINE_FLAGS_PRIORITY_HIGH                  0x00000002
#define NVD_ENGINE_FLAGS_PRIORITY_CRITICAL              0x00000003

/*
 * NVD_ENGINE_FLAGS_SOURCE
 *
 * CPU -  Always run on CPU, even if running as GSP-RM client.
 * GSP -  Run on GSP for GSP-RM client, otherwise run on CPU.
 * BOTH - Engine dump is split between GSP-RM and CPU.  Run both.
 */
#define NVD_ENGINE_FLAGS_SOURCE                         3:2
#define NVD_ENGINE_FLAGS_SOURCE_CPU                     0x00000001
#define NVD_ENGINE_FLAGS_SOURCE_GSP                     0x00000002
#define NVD_ENGINE_FLAGS_SOURCE_BOTH                    0x00000003


#define NV_NVD_ENGINE_STEP_MAJOR            31:16
#define NV_NVD_ENGINE_STEP_MINOR            15:0

#define NVD_ENGINE_STEP(Major, Minor)             \
    (DRF_NUM(_NVD, _ENGINE_STEP, _MAJOR, Major) | \
     DRF_NUM(_NVD, _ENGINE_STEP, _MINOR, Minor))

typedef enum
{
    NVD_FIRST_ENGINE = 0,
    NVD_LAST_ENGINE = 0xFF,
} NVD_WHICH_ENGINE;

typedef struct _def_nvd_debug_buffer {
    NvU32 tag;
    MEMORY_DESCRIPTOR *pMemDesc;
    struct _def_nvd_debug_buffer *pNext;
} NVD_DEBUG_BUFFER;

// Enumeration of Dump Types (Journal Entry, OCA dump, or API requested dump)
typedef enum
{
    NVD_DUMP_TYPE_JOURNAL,                   // Very small records only.  Total for
                                             //   whole Journal is 4K (including overhead),
                                             //   actual amount of raw data stored is less.
    NVD_DUMP_TYPE_OCA,                       // Assume 8K - 512 K total
    NVD_DUMP_TYPE_API,                       // Mini Dump >512K
} NVD_DUMP_TYPE;

// Enumeration of Sizes returned by nvDumpGetDumpBufferSizeEnum
typedef enum
{
    NVD_DUMP_SIZE_JOURNAL_WRITE,             // Very small records only.
    NVD_DUMP_SIZE_SMALL,                     // Assume 8K - 512 K total
    NVD_DUMP_SIZE_MEDIUM,                    // Mini Dump >512K
    NVD_DUMP_SIZE_LARGE                      // Megs of space
} NVD_DUMP_SIZE;

//
// NV Dump State
//
// State passed into all dump routines.
//
typedef struct _def_nvd_state NVD_STATE;

struct _def_nvd_state
{
    NvBool         bDumpInProcess;           // Currently creating dump.
    NvBool         bRMLock;                  // Acquired the RM lock.
    NvBool         bGpuAccessible;           // OK to read priv registers on GPU.
    NvU32          bugCheckCode;             // Raw OS bugcheck code.
    NvU32          internalCode;             // OS Independent error code.
    NvU32          initialbufferSize;        // Size of buffer passed in.
    NVD_DUMP_TYPE  nvDumpType;               // Type of DUMP.
};


NVD_DUMP_SIZE nvDumpGetDumpBufferSizeEnum( NVD_STATE  *pNvDumpState );

typedef NV_STATUS NvdDumpEngineFunc(struct OBJGPU *pGpu, PRB_ENCODER *pPrbEnc, NVD_STATE *pNvDumpState, void *pvData);

typedef struct _def_nvd_engine_callback {
    NvdDumpEngineFunc   *pDumpEngineFunc;      // Callback function.
    NvU32                engDesc;            // Indicates which engine this is.
    NvU32                flags;                // See NVD_ENGINE_FLAGS above.
    void                *pvData;               // Opaque pointer to data passed to callback function.
    struct _def_nvd_engine_callback *pNext;    // Next Engine
} NVD_ENGINE_CALLBACK;


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_NV_DEBUG_DUMP_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__NvDebugDump;
struct NVOC_METADATA__OBJENGSTATE;
struct NVOC_VTABLE__NvDebugDump;


struct NvDebugDump {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__NvDebugDump *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super^2
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;    // engstate super
    struct NvDebugDump *__nvoc_pbase_NvDebugDump;    // nvd

    // 1 PDB property

    // Data members
    NVD_DEBUG_BUFFER *pHeadDebugBuffer;
    NVD_ENGINE_CALLBACK *pCallbacks;
};


// Vtable with 14 per-class function pointers
struct NVOC_VTABLE__NvDebugDump {
    NV_STATUS (*__nvdConstructEngine__)(struct OBJGPU *, struct NvDebugDump * /*this*/, ENGDESCRIPTOR);  // virtual override (engstate) base (engstate)
    NV_STATUS (*__nvdStateInitLocked__)(struct OBJGPU *, struct NvDebugDump * /*this*/);  // virtual override (engstate) base (engstate)
    void (*__nvdInitMissing__)(struct OBJGPU *, struct NvDebugDump * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__nvdStatePreInitLocked__)(struct OBJGPU *, struct NvDebugDump * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__nvdStatePreInitUnlocked__)(struct OBJGPU *, struct NvDebugDump * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__nvdStateInitUnlocked__)(struct OBJGPU *, struct NvDebugDump * /*this*/);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__nvdStatePreLoad__)(struct OBJGPU *, struct NvDebugDump * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__nvdStateLoad__)(struct OBJGPU *, struct NvDebugDump * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__nvdStatePostLoad__)(struct OBJGPU *, struct NvDebugDump * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__nvdStatePreUnload__)(struct OBJGPU *, struct NvDebugDump * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__nvdStateUnload__)(struct OBJGPU *, struct NvDebugDump * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    NV_STATUS (*__nvdStatePostUnload__)(struct OBJGPU *, struct NvDebugDump * /*this*/, NvU32);  // virtual inherited (engstate) base (engstate)
    void (*__nvdStateDestroy__)(struct OBJGPU *, struct NvDebugDump * /*this*/);  // virtual inherited (engstate) base (engstate)
    NvBool (*__nvdIsPresent__)(struct OBJGPU *, struct NvDebugDump * /*this*/);  // virtual inherited (engstate) base (engstate)
};

// Metadata with per-class RTTI and vtable with ancestor(s)
struct NVOC_METADATA__NvDebugDump {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__OBJENGSTATE metadata__OBJENGSTATE;
    const struct NVOC_VTABLE__NvDebugDump vtable;
};

#ifndef __NVOC_CLASS_NvDebugDump_TYPEDEF__
#define __NVOC_CLASS_NvDebugDump_TYPEDEF__
typedef struct NvDebugDump NvDebugDump;
#endif /* __NVOC_CLASS_NvDebugDump_TYPEDEF__ */

#ifndef __nvoc_class_id_NvDebugDump
#define __nvoc_class_id_NvDebugDump 0x7e80a2
#endif /* __nvoc_class_id_NvDebugDump */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvDebugDump;

#define __staticCast_NvDebugDump(pThis) \
    ((pThis)->__nvoc_pbase_NvDebugDump)

#ifdef __nvoc_nv_debug_dump_h_disabled
#define __dynamicCast_NvDebugDump(pThis) ((NvDebugDump*) NULL)
#else //__nvoc_nv_debug_dump_h_disabled
#define __dynamicCast_NvDebugDump(pThis) \
    ((NvDebugDump*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvDebugDump)))
#endif //__nvoc_nv_debug_dump_h_disabled

// Property macros
#define PDB_PROP_NVD_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_NVD_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_NvDebugDump(NvDebugDump**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvDebugDump(NvDebugDump**, Dynamic*, NvU32);
#define __objCreate_NvDebugDump(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_NvDebugDump((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros
#define nvdConstructEngine_FNPTR(pNvd) pNvd->__nvoc_metadata_ptr->vtable.__nvdConstructEngine__
#define nvdConstructEngine(pGpu, pNvd, arg3) nvdConstructEngine_DISPATCH(pGpu, pNvd, arg3)
#define nvdStateInitLocked_FNPTR(pNvd) pNvd->__nvoc_metadata_ptr->vtable.__nvdStateInitLocked__
#define nvdStateInitLocked(pGpu, pNvd) nvdStateInitLocked_DISPATCH(pGpu, pNvd)
#define nvdInitMissing_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateInitMissing__
#define nvdInitMissing(pGpu, pEngstate) nvdInitMissing_DISPATCH(pGpu, pEngstate)
#define nvdStatePreInitLocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitLocked__
#define nvdStatePreInitLocked(pGpu, pEngstate) nvdStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define nvdStatePreInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreInitUnlocked__
#define nvdStatePreInitUnlocked(pGpu, pEngstate) nvdStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define nvdStateInitUnlocked_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateInitUnlocked__
#define nvdStateInitUnlocked(pGpu, pEngstate) nvdStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define nvdStatePreLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreLoad__
#define nvdStatePreLoad(pGpu, pEngstate, arg3) nvdStatePreLoad_DISPATCH(pGpu, pEngstate, arg3)
#define nvdStateLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateLoad__
#define nvdStateLoad(pGpu, pEngstate, arg3) nvdStateLoad_DISPATCH(pGpu, pEngstate, arg3)
#define nvdStatePostLoad_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostLoad__
#define nvdStatePostLoad(pGpu, pEngstate, arg3) nvdStatePostLoad_DISPATCH(pGpu, pEngstate, arg3)
#define nvdStatePreUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePreUnload__
#define nvdStatePreUnload(pGpu, pEngstate, arg3) nvdStatePreUnload_DISPATCH(pGpu, pEngstate, arg3)
#define nvdStateUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateUnload__
#define nvdStateUnload(pGpu, pEngstate, arg3) nvdStateUnload_DISPATCH(pGpu, pEngstate, arg3)
#define nvdStatePostUnload_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStatePostUnload__
#define nvdStatePostUnload(pGpu, pEngstate, arg3) nvdStatePostUnload_DISPATCH(pGpu, pEngstate, arg3)
#define nvdStateDestroy_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateStateDestroy__
#define nvdStateDestroy(pGpu, pEngstate) nvdStateDestroy_DISPATCH(pGpu, pEngstate)
#define nvdIsPresent_FNPTR(pEngstate) pEngstate->__nvoc_base_OBJENGSTATE.__nvoc_metadata_ptr->vtable.__engstateIsPresent__
#define nvdIsPresent(pGpu, pEngstate) nvdIsPresent_DISPATCH(pGpu, pEngstate)

// Dispatch functions
static inline NV_STATUS nvdConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, ENGDESCRIPTOR arg3) {
    return pNvd->__nvoc_metadata_ptr->vtable.__nvdConstructEngine__(pGpu, pNvd, arg3);
}

static inline NV_STATUS nvdStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct NvDebugDump *pNvd) {
    return pNvd->__nvoc_metadata_ptr->vtable.__nvdStateInitLocked__(pGpu, pNvd);
}

static inline void nvdInitMissing_DISPATCH(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__nvdInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS nvdStatePreInitLocked_DISPATCH(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__nvdStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS nvdStatePreInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__nvdStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS nvdStateInitUnlocked_DISPATCH(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__nvdStateInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS nvdStatePreLoad_DISPATCH(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__nvdStatePreLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS nvdStateLoad_DISPATCH(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__nvdStateLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS nvdStatePostLoad_DISPATCH(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__nvdStatePostLoad__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS nvdStatePreUnload_DISPATCH(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__nvdStatePreUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS nvdStateUnload_DISPATCH(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__nvdStateUnload__(pGpu, pEngstate, arg3);
}

static inline NV_STATUS nvdStatePostUnload_DISPATCH(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate, NvU32 arg3) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__nvdStatePostUnload__(pGpu, pEngstate, arg3);
}

static inline void nvdStateDestroy_DISPATCH(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate) {
    pEngstate->__nvoc_metadata_ptr->vtable.__nvdStateDestroy__(pGpu, pEngstate);
}

static inline NvBool nvdIsPresent_DISPATCH(struct OBJGPU *pGpu, struct NvDebugDump *pEngstate) {
    return pEngstate->__nvoc_metadata_ptr->vtable.__nvdIsPresent__(pGpu, pEngstate);
}

NV_STATUS nvdConstructEngine_IMPL(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, ENGDESCRIPTOR arg3);

NV_STATUS nvdStateInitLocked_IMPL(struct OBJGPU *pGpu, struct NvDebugDump *pNvd);

void nvdDestruct_IMPL(struct NvDebugDump *pNvd);

#define __nvoc_nvdDestruct(pNvd) nvdDestruct_IMPL(pNvd)
NV_STATUS nvdDumpComponent_IMPL(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, NvU32 component, NVDUMP_BUFFER *pBuffer, NVDUMP_BUFFER_POLICY policy, PrbBufferCallback *pBufferCallback);

#ifdef __nvoc_nv_debug_dump_h_disabled
static inline NV_STATUS nvdDumpComponent(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, NvU32 component, NVDUMP_BUFFER *pBuffer, NVDUMP_BUFFER_POLICY policy, PrbBufferCallback *pBufferCallback) {
    NV_ASSERT_FAILED_PRECOMP("NvDebugDump was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_nv_debug_dump_h_disabled
#define nvdDumpComponent(pGpu, pNvd, component, pBuffer, policy, pBufferCallback) nvdDumpComponent_IMPL(pGpu, pNvd, component, pBuffer, policy, pBufferCallback)
#endif //__nvoc_nv_debug_dump_h_disabled

NV_STATUS nvdAllocDebugBuffer_IMPL(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, NvU32 arg3, NvU32 *arg4, MEMORY_DESCRIPTOR **arg5);

#ifdef __nvoc_nv_debug_dump_h_disabled
static inline NV_STATUS nvdAllocDebugBuffer(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, NvU32 arg3, NvU32 *arg4, MEMORY_DESCRIPTOR **arg5) {
    NV_ASSERT_FAILED_PRECOMP("NvDebugDump was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_nv_debug_dump_h_disabled
#define nvdAllocDebugBuffer(pGpu, pNvd, arg3, arg4, arg5) nvdAllocDebugBuffer_IMPL(pGpu, pNvd, arg3, arg4, arg5)
#endif //__nvoc_nv_debug_dump_h_disabled

NV_STATUS nvdFreeDebugBuffer_IMPL(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, MEMORY_DESCRIPTOR *arg3);

#ifdef __nvoc_nv_debug_dump_h_disabled
static inline NV_STATUS nvdFreeDebugBuffer(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, MEMORY_DESCRIPTOR *arg3) {
    NV_ASSERT_FAILED_PRECOMP("NvDebugDump was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_nv_debug_dump_h_disabled
#define nvdFreeDebugBuffer(pGpu, pNvd, arg3) nvdFreeDebugBuffer_IMPL(pGpu, pNvd, arg3)
#endif //__nvoc_nv_debug_dump_h_disabled

NV_STATUS nvdDumpDebugBuffers_IMPL(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, PRB_ENCODER *arg3);

#ifdef __nvoc_nv_debug_dump_h_disabled
static inline NV_STATUS nvdDumpDebugBuffers(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, PRB_ENCODER *arg3) {
    NV_ASSERT_FAILED_PRECOMP("NvDebugDump was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_nv_debug_dump_h_disabled
#define nvdDumpDebugBuffers(pGpu, pNvd, arg3) nvdDumpDebugBuffers_IMPL(pGpu, pNvd, arg3)
#endif //__nvoc_nv_debug_dump_h_disabled

NV_STATUS nvdEngineSignUp_IMPL(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, NvdDumpEngineFunc *arg3, NvU32 engDesc, NvU32 flags, void *arg6);

#ifdef __nvoc_nv_debug_dump_h_disabled
static inline NV_STATUS nvdEngineSignUp(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, NvdDumpEngineFunc *arg3, NvU32 engDesc, NvU32 flags, void *arg6) {
    NV_ASSERT_FAILED_PRECOMP("NvDebugDump was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_nv_debug_dump_h_disabled
#define nvdEngineSignUp(pGpu, pNvd, arg3, engDesc, flags, arg6) nvdEngineSignUp_IMPL(pGpu, pNvd, arg3, engDesc, flags, arg6)
#endif //__nvoc_nv_debug_dump_h_disabled

NV_STATUS nvdEngineRelease_IMPL(struct OBJGPU *pGpu, struct NvDebugDump *pNvd);

#ifdef __nvoc_nv_debug_dump_h_disabled
static inline NV_STATUS nvdEngineRelease(struct OBJGPU *pGpu, struct NvDebugDump *pNvd) {
    NV_ASSERT_FAILED_PRECOMP("NvDebugDump was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_nv_debug_dump_h_disabled
#define nvdEngineRelease(pGpu, pNvd) nvdEngineRelease_IMPL(pGpu, pNvd)
#endif //__nvoc_nv_debug_dump_h_disabled

NV_STATUS nvdDoEngineDump_IMPL(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, PRB_ENCODER *pPrbEnc, NVD_STATE *pNvDumpState, NvU32 arg5);

#ifdef __nvoc_nv_debug_dump_h_disabled
static inline NV_STATUS nvdDoEngineDump(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, PRB_ENCODER *pPrbEnc, NVD_STATE *pNvDumpState, NvU32 arg5) {
    NV_ASSERT_FAILED_PRECOMP("NvDebugDump was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_nv_debug_dump_h_disabled
#define nvdDoEngineDump(pGpu, pNvd, pPrbEnc, pNvDumpState, arg5) nvdDoEngineDump_IMPL(pGpu, pNvd, pPrbEnc, pNvDumpState, arg5)
#endif //__nvoc_nv_debug_dump_h_disabled

NV_STATUS nvdDumpAllEngines_IMPL(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, PRB_ENCODER *pPrbEnc, NVD_STATE *pNvDumpState);

#ifdef __nvoc_nv_debug_dump_h_disabled
static inline NV_STATUS nvdDumpAllEngines(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, PRB_ENCODER *pPrbEnc, NVD_STATE *pNvDumpState) {
    NV_ASSERT_FAILED_PRECOMP("NvDebugDump was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_nv_debug_dump_h_disabled
#define nvdDumpAllEngines(pGpu, pNvd, pPrbEnc, pNvDumpState) nvdDumpAllEngines_IMPL(pGpu, pNvd, pPrbEnc, pNvDumpState)
#endif //__nvoc_nv_debug_dump_h_disabled

NV_STATUS nvdFindEngine_IMPL(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, NvU32 engDesc, NVD_ENGINE_CALLBACK **ppEngineCallback);

#ifdef __nvoc_nv_debug_dump_h_disabled
static inline NV_STATUS nvdFindEngine(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, NvU32 engDesc, NVD_ENGINE_CALLBACK **ppEngineCallback) {
    NV_ASSERT_FAILED_PRECOMP("NvDebugDump was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_nv_debug_dump_h_disabled
#define nvdFindEngine(pGpu, pNvd, engDesc, ppEngineCallback) nvdFindEngine_IMPL(pGpu, pNvd, engDesc, ppEngineCallback)
#endif //__nvoc_nv_debug_dump_h_disabled

#undef PRIVATE_FIELD


#endif // _NV_DEBUG_DUMP_H_

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_NV_DEBUG_DUMP_NVOC_H_
