
#ifndef _G_INSTRUMENTATION_MANAGER_NVOC_H_
#define _G_INSTRUMENTATION_MANAGER_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2024-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_instrumentation_manager_nvoc.h"

#ifndef __INSTRUMENTATION_MANAGER_H__
#define __INSTRUMENTATION_MANAGER_H__

#include "gpu/gpu.h"
#include "core/core.h"

#define GFID_TASK_RM                            0
#define BULLSEYE_TASK_VGPU_COVERAGE_SIZE        (32 << 10)
#define BULLSEYE_TASK_RM_COVERAGE_SIZE          (3 << 20)
#define MAX_PARTITIONS_WITH_CODE_COVERAGE       (32)
#define BULLSEYE_GSP_RM_COVERAGE_SIZE           \
            (BULLSEYE_TASK_RM_COVERAGE_SIZE) + (MAX_PARTITIONS_WITH_CODE_COVERAGE * BULLSEYE_TASK_VGPU_COVERAGE_SIZE)

typedef struct
{
    NvLength length;
    NvU8     *dataBuffer;
} GSP_INSTRUMENTATION_BUFFER;

typedef struct
{
    NvU32       gfid; // 0 for task_rm, 1-32 (inclusive) for task_vgpu objects
    NvU32       gpuInstance;
    NvU8        *pData;
    NvLength    bufferLength;
} GSP_INSTRUMENTATION_DATA;
 
// list storing 33 (or more) coverage buffers for task_rm and task_vgpu
MAKE_LIST(GSP_INSTRUMENTATION_DATA_LIST, GSP_INSTRUMENTATION_DATA);


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_INSTRUMENTATION_MANAGER_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__InstrumentationManager;
struct NVOC_METADATA__Object;


struct InstrumentationManager {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__InstrumentationManager *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct InstrumentationManager *__nvoc_pbase_InstrumentationManager;    // instrumentationmanager

    // Data members
    GSP_INSTRUMENTATION_DATA_LIST bufferList;
    GSP_INSTRUMENTATION_BUFFER outputBuffer;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__InstrumentationManager {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
};

#ifndef __NVOC_CLASS_InstrumentationManager_TYPEDEF__
#define __NVOC_CLASS_InstrumentationManager_TYPEDEF__
typedef struct InstrumentationManager InstrumentationManager;
#endif /* __NVOC_CLASS_InstrumentationManager_TYPEDEF__ */

#ifndef __nvoc_class_id_InstrumentationManager
#define __nvoc_class_id_InstrumentationManager 0x30d0c5
#endif /* __nvoc_class_id_InstrumentationManager */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_InstrumentationManager;

#define __staticCast_InstrumentationManager(pThis) \
    ((pThis)->__nvoc_pbase_InstrumentationManager)

#ifdef __nvoc_instrumentation_manager_h_disabled
#define __dynamicCast_InstrumentationManager(pThis) ((InstrumentationManager*) NULL)
#else //__nvoc_instrumentation_manager_h_disabled
#define __dynamicCast_InstrumentationManager(pThis) \
    ((InstrumentationManager*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(InstrumentationManager)))
#endif //__nvoc_instrumentation_manager_h_disabled

NV_STATUS __nvoc_objCreateDynamic_InstrumentationManager(InstrumentationManager**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_InstrumentationManager(InstrumentationManager**, Dynamic*, NvU32);
#define __objCreate_InstrumentationManager(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_InstrumentationManager((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros for implementation functions
NV_STATUS instrumentationmanagerConstruct_IMPL(struct InstrumentationManager *arg_pInstrumentationManager);
#define __nvoc_instrumentationmanagerConstruct(arg_pInstrumentationManager) instrumentationmanagerConstruct_IMPL(arg_pInstrumentationManager)

void instrumentationmanagerDestruct_IMPL(struct InstrumentationManager *pInstrumentationManager);
#define __nvoc_instrumentationmanagerDestruct(pInstrumentationManager) instrumentationmanagerDestruct_IMPL(pInstrumentationManager)

GSP_INSTRUMENTATION_DATA * instrumentationmanagerGetNode_IMPL(struct InstrumentationManager *pInstrumentationManager, NvU32 gfid, NvU32 gpuInstance);
#ifdef __nvoc_instrumentation_manager_h_disabled
static inline GSP_INSTRUMENTATION_DATA * instrumentationmanagerGetNode(struct InstrumentationManager *pInstrumentationManager, NvU32 gfid, NvU32 gpuInstance) {
    NV_ASSERT_FAILED_PRECOMP("InstrumentationManager was disabled!");
    return NULL;
}
#else // __nvoc_instrumentation_manager_h_disabled
#define instrumentationmanagerGetNode(pInstrumentationManager, gfid, gpuInstance) instrumentationmanagerGetNode_IMPL(pInstrumentationManager, gfid, gpuInstance)
#endif // __nvoc_instrumentation_manager_h_disabled

NvU8 * instrumentationmanagerGetBuffer_IMPL(struct InstrumentationManager *pInstrumentationManager, NvU32 gfid, NvU32 gpuInstance);
#ifdef __nvoc_instrumentation_manager_h_disabled
static inline NvU8 * instrumentationmanagerGetBuffer(struct InstrumentationManager *pInstrumentationManager, NvU32 gfid, NvU32 gpuInstance) {
    NV_ASSERT_FAILED_PRECOMP("InstrumentationManager was disabled!");
    return NULL;
}
#else // __nvoc_instrumentation_manager_h_disabled
#define instrumentationmanagerGetBuffer(pInstrumentationManager, gfid, gpuInstance) instrumentationmanagerGetBuffer_IMPL(pInstrumentationManager, gfid, gpuInstance)
#endif // __nvoc_instrumentation_manager_h_disabled

void instrumentationmanagerMerge_IMPL(struct InstrumentationManager *pInstrumentationMgr, NvU32 gfid, NvU32 gpuInstance, NvU8 *pSysmemBuffer);
#ifdef __nvoc_instrumentation_manager_h_disabled
static inline void instrumentationmanagerMerge(struct InstrumentationManager *pInstrumentationMgr, NvU32 gfid, NvU32 gpuInstance, NvU8 *pSysmemBuffer) {
    NV_ASSERT_FAILED_PRECOMP("InstrumentationManager was disabled!");
}
#else // __nvoc_instrumentation_manager_h_disabled
#define instrumentationmanagerMerge(pInstrumentationMgr, gfid, gpuInstance, pSysmemBuffer) instrumentationmanagerMerge_IMPL(pInstrumentationMgr, gfid, gpuInstance, pSysmemBuffer)
#endif // __nvoc_instrumentation_manager_h_disabled

void instrumentationmanagerReset_IMPL(struct InstrumentationManager *pInstrumentationMgr, NvU32 gfid, NvU32 gpuInstance);
#ifdef __nvoc_instrumentation_manager_h_disabled
static inline void instrumentationmanagerReset(struct InstrumentationManager *pInstrumentationMgr, NvU32 gfid, NvU32 gpuInstance) {
    NV_ASSERT_FAILED_PRECOMP("InstrumentationManager was disabled!");
}
#else // __nvoc_instrumentation_manager_h_disabled
#define instrumentationmanagerReset(pInstrumentationMgr, gfid, gpuInstance) instrumentationmanagerReset_IMPL(pInstrumentationMgr, gfid, gpuInstance)
#endif // __nvoc_instrumentation_manager_h_disabled

void instrumentationmanagerRegisterBuffer_IMPL(struct InstrumentationManager *pInstrumentationMgr, NvU32 gfid, NvU32 gpuInstance, NvU64 bufferSize);
#ifdef __nvoc_instrumentation_manager_h_disabled
static inline void instrumentationmanagerRegisterBuffer(struct InstrumentationManager *pInstrumentationMgr, NvU32 gfid, NvU32 gpuInstance, NvU64 bufferSize) {
    NV_ASSERT_FAILED_PRECOMP("InstrumentationManager was disabled!");
}
#else // __nvoc_instrumentation_manager_h_disabled
#define instrumentationmanagerRegisterBuffer(pInstrumentationMgr, gfid, gpuInstance, bufferSize) instrumentationmanagerRegisterBuffer_IMPL(pInstrumentationMgr, gfid, gpuInstance, bufferSize)
#endif // __nvoc_instrumentation_manager_h_disabled

void instrumentationmanagerDeregisterBuffer_IMPL(struct InstrumentationManager *pInstrumentationMgr, NvU32 gfid, NvU32 gpuInstance);
#ifdef __nvoc_instrumentation_manager_h_disabled
static inline void instrumentationmanagerDeregisterBuffer(struct InstrumentationManager *pInstrumentationMgr, NvU32 gfid, NvU32 gpuInstance) {
    NV_ASSERT_FAILED_PRECOMP("InstrumentationManager was disabled!");
}
#else // __nvoc_instrumentation_manager_h_disabled
#define instrumentationmanagerDeregisterBuffer(pInstrumentationMgr, gfid, gpuInstance) instrumentationmanagerDeregisterBuffer_IMPL(pInstrumentationMgr, gfid, gpuInstance)
#endif // __nvoc_instrumentation_manager_h_disabled


// Wrapper macros for halified functions

// Dispatch functions
#undef PRIVATE_FIELD


#endif //__INSTRUMENTATION_MANAGER_H__

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_INSTRUMENTATION_MANAGER_NVOC_H_
