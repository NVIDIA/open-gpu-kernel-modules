#ifndef _G_NV_DEBUG_DUMP_NVOC_H_
#define _G_NV_DEBUG_DUMP_NVOC_H_
#include "nvoc/runtime.h"

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
#include "g_nv_debug_dump_nvoc.h"

#ifndef _NV_DEBUG_DUMP_H_
#define _NV_DEBUG_DUMP_H_

#include "gpu/eng_state.h"
#include "gpu/mem_mgr/mem_desc.h"
#include "core/info_block.h"

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

#ifdef NVOC_NV_DEBUG_DUMP_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct NvDebugDump {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct OBJENGSTATE __nvoc_base_OBJENGSTATE;
    struct Object *__nvoc_pbase_Object;
    struct OBJENGSTATE *__nvoc_pbase_OBJENGSTATE;
    struct NvDebugDump *__nvoc_pbase_NvDebugDump;
    NV_STATUS (*__nvdConstructEngine__)(struct OBJGPU *, struct NvDebugDump *, ENGDESCRIPTOR);
    NV_STATUS (*__nvdStateInitLocked__)(struct OBJGPU *, struct NvDebugDump *);
    NV_STATUS (*__nvdStateLoad__)(POBJGPU, struct NvDebugDump *, NvU32);
    NV_STATUS (*__nvdStateUnload__)(POBJGPU, struct NvDebugDump *, NvU32);
    NV_STATUS (*__nvdStatePreLoad__)(POBJGPU, struct NvDebugDump *, NvU32);
    NV_STATUS (*__nvdStatePostUnload__)(POBJGPU, struct NvDebugDump *, NvU32);
    void (*__nvdStateDestroy__)(POBJGPU, struct NvDebugDump *);
    NV_STATUS (*__nvdStatePreUnload__)(POBJGPU, struct NvDebugDump *, NvU32);
    NV_STATUS (*__nvdStateInitUnlocked__)(POBJGPU, struct NvDebugDump *);
    void (*__nvdInitMissing__)(POBJGPU, struct NvDebugDump *);
    NV_STATUS (*__nvdStatePreInitLocked__)(POBJGPU, struct NvDebugDump *);
    NV_STATUS (*__nvdStatePreInitUnlocked__)(POBJGPU, struct NvDebugDump *);
    NV_STATUS (*__nvdStatePostLoad__)(POBJGPU, struct NvDebugDump *, NvU32);
    NvBool (*__nvdIsPresent__)(POBJGPU, struct NvDebugDump *);
    NVD_DEBUG_BUFFER *pHeadDebugBuffer;
    NVD_ENGINE_CALLBACK *pCallbacks;
};

#ifndef __NVOC_CLASS_NvDebugDump_TYPEDEF__
#define __NVOC_CLASS_NvDebugDump_TYPEDEF__
typedef struct NvDebugDump NvDebugDump;
#endif /* __NVOC_CLASS_NvDebugDump_TYPEDEF__ */

#ifndef __nvoc_class_id_NvDebugDump
#define __nvoc_class_id_NvDebugDump 0x7e80a2
#endif /* __nvoc_class_id_NvDebugDump */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_NvDebugDump;

#define __staticCast_NvDebugDump(pThis) \
    ((pThis)->__nvoc_pbase_NvDebugDump)

#ifdef __nvoc_nv_debug_dump_h_disabled
#define __dynamicCast_NvDebugDump(pThis) ((NvDebugDump*)NULL)
#else //__nvoc_nv_debug_dump_h_disabled
#define __dynamicCast_NvDebugDump(pThis) \
    ((NvDebugDump*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(NvDebugDump)))
#endif //__nvoc_nv_debug_dump_h_disabled

#define PDB_PROP_NVD_IS_MISSING_BASE_CAST __nvoc_base_OBJENGSTATE.
#define PDB_PROP_NVD_IS_MISSING_BASE_NAME PDB_PROP_ENGSTATE_IS_MISSING

NV_STATUS __nvoc_objCreateDynamic_NvDebugDump(NvDebugDump**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_NvDebugDump(NvDebugDump**, Dynamic*, NvU32);
#define __objCreate_NvDebugDump(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_NvDebugDump((ppNewObj), staticCast((pParent), Dynamic), (createFlags))

#define nvdConstructEngine(pGpu, pNvd, arg0) nvdConstructEngine_DISPATCH(pGpu, pNvd, arg0)
#define nvdStateInitLocked(pGpu, pNvd) nvdStateInitLocked_DISPATCH(pGpu, pNvd)
#define nvdStateLoad(pGpu, pEngstate, arg0) nvdStateLoad_DISPATCH(pGpu, pEngstate, arg0)
#define nvdStateUnload(pGpu, pEngstate, arg0) nvdStateUnload_DISPATCH(pGpu, pEngstate, arg0)
#define nvdStatePreLoad(pGpu, pEngstate, arg0) nvdStatePreLoad_DISPATCH(pGpu, pEngstate, arg0)
#define nvdStatePostUnload(pGpu, pEngstate, arg0) nvdStatePostUnload_DISPATCH(pGpu, pEngstate, arg0)
#define nvdStateDestroy(pGpu, pEngstate) nvdStateDestroy_DISPATCH(pGpu, pEngstate)
#define nvdStatePreUnload(pGpu, pEngstate, arg0) nvdStatePreUnload_DISPATCH(pGpu, pEngstate, arg0)
#define nvdStateInitUnlocked(pGpu, pEngstate) nvdStateInitUnlocked_DISPATCH(pGpu, pEngstate)
#define nvdInitMissing(pGpu, pEngstate) nvdInitMissing_DISPATCH(pGpu, pEngstate)
#define nvdStatePreInitLocked(pGpu, pEngstate) nvdStatePreInitLocked_DISPATCH(pGpu, pEngstate)
#define nvdStatePreInitUnlocked(pGpu, pEngstate) nvdStatePreInitUnlocked_DISPATCH(pGpu, pEngstate)
#define nvdStatePostLoad(pGpu, pEngstate, arg0) nvdStatePostLoad_DISPATCH(pGpu, pEngstate, arg0)
#define nvdIsPresent(pGpu, pEngstate) nvdIsPresent_DISPATCH(pGpu, pEngstate)
NV_STATUS nvdConstructEngine_IMPL(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, ENGDESCRIPTOR arg0);

static inline NV_STATUS nvdConstructEngine_DISPATCH(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, ENGDESCRIPTOR arg0) {
    return pNvd->__nvdConstructEngine__(pGpu, pNvd, arg0);
}

NV_STATUS nvdStateInitLocked_IMPL(struct OBJGPU *pGpu, struct NvDebugDump *pNvd);

static inline NV_STATUS nvdStateInitLocked_DISPATCH(struct OBJGPU *pGpu, struct NvDebugDump *pNvd) {
    return pNvd->__nvdStateInitLocked__(pGpu, pNvd);
}

static inline NV_STATUS nvdStateLoad_DISPATCH(POBJGPU pGpu, struct NvDebugDump *pEngstate, NvU32 arg0) {
    return pEngstate->__nvdStateLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS nvdStateUnload_DISPATCH(POBJGPU pGpu, struct NvDebugDump *pEngstate, NvU32 arg0) {
    return pEngstate->__nvdStateUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS nvdStatePreLoad_DISPATCH(POBJGPU pGpu, struct NvDebugDump *pEngstate, NvU32 arg0) {
    return pEngstate->__nvdStatePreLoad__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS nvdStatePostUnload_DISPATCH(POBJGPU pGpu, struct NvDebugDump *pEngstate, NvU32 arg0) {
    return pEngstate->__nvdStatePostUnload__(pGpu, pEngstate, arg0);
}

static inline void nvdStateDestroy_DISPATCH(POBJGPU pGpu, struct NvDebugDump *pEngstate) {
    pEngstate->__nvdStateDestroy__(pGpu, pEngstate);
}

static inline NV_STATUS nvdStatePreUnload_DISPATCH(POBJGPU pGpu, struct NvDebugDump *pEngstate, NvU32 arg0) {
    return pEngstate->__nvdStatePreUnload__(pGpu, pEngstate, arg0);
}

static inline NV_STATUS nvdStateInitUnlocked_DISPATCH(POBJGPU pGpu, struct NvDebugDump *pEngstate) {
    return pEngstate->__nvdStateInitUnlocked__(pGpu, pEngstate);
}

static inline void nvdInitMissing_DISPATCH(POBJGPU pGpu, struct NvDebugDump *pEngstate) {
    pEngstate->__nvdInitMissing__(pGpu, pEngstate);
}

static inline NV_STATUS nvdStatePreInitLocked_DISPATCH(POBJGPU pGpu, struct NvDebugDump *pEngstate) {
    return pEngstate->__nvdStatePreInitLocked__(pGpu, pEngstate);
}

static inline NV_STATUS nvdStatePreInitUnlocked_DISPATCH(POBJGPU pGpu, struct NvDebugDump *pEngstate) {
    return pEngstate->__nvdStatePreInitUnlocked__(pGpu, pEngstate);
}

static inline NV_STATUS nvdStatePostLoad_DISPATCH(POBJGPU pGpu, struct NvDebugDump *pEngstate, NvU32 arg0) {
    return pEngstate->__nvdStatePostLoad__(pGpu, pEngstate, arg0);
}

static inline NvBool nvdIsPresent_DISPATCH(POBJGPU pGpu, struct NvDebugDump *pEngstate) {
    return pEngstate->__nvdIsPresent__(pGpu, pEngstate);
}

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

NV_STATUS nvdAllocDebugBuffer_IMPL(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, NvU32 arg0, NvU32 *arg1, MEMORY_DESCRIPTOR **arg2);

#ifdef __nvoc_nv_debug_dump_h_disabled
static inline NV_STATUS nvdAllocDebugBuffer(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, NvU32 arg0, NvU32 *arg1, MEMORY_DESCRIPTOR **arg2) {
    NV_ASSERT_FAILED_PRECOMP("NvDebugDump was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_nv_debug_dump_h_disabled
#define nvdAllocDebugBuffer(pGpu, pNvd, arg0, arg1, arg2) nvdAllocDebugBuffer_IMPL(pGpu, pNvd, arg0, arg1, arg2)
#endif //__nvoc_nv_debug_dump_h_disabled

NV_STATUS nvdFreeDebugBuffer_IMPL(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, MEMORY_DESCRIPTOR *arg0);

#ifdef __nvoc_nv_debug_dump_h_disabled
static inline NV_STATUS nvdFreeDebugBuffer(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, MEMORY_DESCRIPTOR *arg0) {
    NV_ASSERT_FAILED_PRECOMP("NvDebugDump was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_nv_debug_dump_h_disabled
#define nvdFreeDebugBuffer(pGpu, pNvd, arg0) nvdFreeDebugBuffer_IMPL(pGpu, pNvd, arg0)
#endif //__nvoc_nv_debug_dump_h_disabled

NV_STATUS nvdDumpDebugBuffers_IMPL(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, PRB_ENCODER *arg0);

#ifdef __nvoc_nv_debug_dump_h_disabled
static inline NV_STATUS nvdDumpDebugBuffers(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, PRB_ENCODER *arg0) {
    NV_ASSERT_FAILED_PRECOMP("NvDebugDump was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_nv_debug_dump_h_disabled
#define nvdDumpDebugBuffers(pGpu, pNvd, arg0) nvdDumpDebugBuffers_IMPL(pGpu, pNvd, arg0)
#endif //__nvoc_nv_debug_dump_h_disabled

NV_STATUS nvdEngineSignUp_IMPL(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, NvdDumpEngineFunc *arg0, NvU32 engDesc, NvU32 flags, void *arg1);

#ifdef __nvoc_nv_debug_dump_h_disabled
static inline NV_STATUS nvdEngineSignUp(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, NvdDumpEngineFunc *arg0, NvU32 engDesc, NvU32 flags, void *arg1) {
    NV_ASSERT_FAILED_PRECOMP("NvDebugDump was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_nv_debug_dump_h_disabled
#define nvdEngineSignUp(pGpu, pNvd, arg0, engDesc, flags, arg1) nvdEngineSignUp_IMPL(pGpu, pNvd, arg0, engDesc, flags, arg1)
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

NV_STATUS nvdDoEngineDump_IMPL(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, PRB_ENCODER *pPrbEnc, NVD_STATE *pNvDumpState, NvU32 arg0);

#ifdef __nvoc_nv_debug_dump_h_disabled
static inline NV_STATUS nvdDoEngineDump(struct OBJGPU *pGpu, struct NvDebugDump *pNvd, PRB_ENCODER *pPrbEnc, NVD_STATE *pNvDumpState, NvU32 arg0) {
    NV_ASSERT_FAILED_PRECOMP("NvDebugDump was disabled!");
    return NV_ERR_NOT_SUPPORTED;
}
#else //__nvoc_nv_debug_dump_h_disabled
#define nvdDoEngineDump(pGpu, pNvd, pPrbEnc, pNvDumpState, arg0) nvdDoEngineDump_IMPL(pGpu, pNvd, pPrbEnc, pNvDumpState, arg0)
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
