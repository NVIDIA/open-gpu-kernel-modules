
#ifndef _G_CODE_COVERAGE_MGR_NVOC_H_
#define _G_CODE_COVERAGE_MGR_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_code_coverage_mgr_nvoc.h"

#ifndef __CODE_COVERAGE_MGR_H__
#define __CODE_COVERAGE_MGR_H__

#include "gpu/gpu.h"
#include "core/core.h"

#define GFID_TASK_RM 0
#define BULLSEYE_TASK_VGPU_COVERAGE_SIZE    (32 << 10)
#define BULLSEYE_TASK_RM_COVERAGE_SIZE      (3 << 20)
#define BULLSEYE_GSP_RM_COVERAGE_SIZE       (BULLSEYE_TASK_RM_COVERAGE_SIZE) + (MAX_PARTITIONS_WITH_GFID * BULLSEYE_TASK_VGPU_COVERAGE_SIZE)

typedef struct
{
    NvLength length;
    NvU8     *dataBuffer;
} GSP_BULLSEYE_OUTPUT_BUFFER;

typedef struct
{
    NvU32       gfid; // 0 for task_rm, 1-32 (inclusive) for task_vgpu objects
    NvU32       gpuInstance;
    NvU8        *pCoverageData;
    NvLength    bufferLength;
} GSP_BULLSEYE_COVERAGE_DATA;
 
// list storing 33 (or more) coverage buffers for task_rm and task_vgpu
MAKE_LIST(GSP_BULLSEYE_COVERAGE_DATA_LIST, GSP_BULLSEYE_COVERAGE_DATA);


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_CODE_COVERAGE_MGR_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__CodeCoverageManager;
struct NVOC_METADATA__Object;


struct CodeCoverageManager {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__CodeCoverageManager *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct CodeCoverageManager *__nvoc_pbase_CodeCoverageManager;    // codecovmgr

    // Data members
    GSP_BULLSEYE_COVERAGE_DATA_LIST covDataList;
    GSP_BULLSEYE_OUTPUT_BUFFER bullseyeOutputBuffer;
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__CodeCoverageManager {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
};

#ifndef __NVOC_CLASS_CodeCoverageManager_TYPEDEF__
#define __NVOC_CLASS_CodeCoverageManager_TYPEDEF__
typedef struct CodeCoverageManager CodeCoverageManager;
#endif /* __NVOC_CLASS_CodeCoverageManager_TYPEDEF__ */

#ifndef __nvoc_class_id_CodeCoverageManager
#define __nvoc_class_id_CodeCoverageManager 0x62cbfb
#endif /* __nvoc_class_id_CodeCoverageManager */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_CodeCoverageManager;

#define __staticCast_CodeCoverageManager(pThis) \
    ((pThis)->__nvoc_pbase_CodeCoverageManager)

#ifdef __nvoc_code_coverage_mgr_h_disabled
#define __dynamicCast_CodeCoverageManager(pThis) ((CodeCoverageManager*) NULL)
#else //__nvoc_code_coverage_mgr_h_disabled
#define __dynamicCast_CodeCoverageManager(pThis) \
    ((CodeCoverageManager*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(CodeCoverageManager)))
#endif //__nvoc_code_coverage_mgr_h_disabled

NV_STATUS __nvoc_objCreateDynamic_CodeCoverageManager(CodeCoverageManager**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_CodeCoverageManager(CodeCoverageManager**, Dynamic*, NvU32);
#define __objCreate_CodeCoverageManager(ppNewObj, pParent, createFlags) \
    __nvoc_objCreate_CodeCoverageManager((ppNewObj), staticCast((pParent), Dynamic), (createFlags))


// Wrapper macros

// Dispatch functions
NV_STATUS codecovmgrConstruct_IMPL(struct CodeCoverageManager *arg_pCodeCovMgr);

#define __nvoc_codecovmgrConstruct(arg_pCodeCovMgr) codecovmgrConstruct_IMPL(arg_pCodeCovMgr)
void codecovmgrDestruct_IMPL(struct CodeCoverageManager *pCodeCovMgr);

#define __nvoc_codecovmgrDestruct(pCodeCovMgr) codecovmgrDestruct_IMPL(pCodeCovMgr)
GSP_BULLSEYE_COVERAGE_DATA *codecovmgrGetCoverageNode_IMPL(struct CodeCoverageManager *pCodeCovMgr, NvU32 gfid, NvU32 gpuInstance);

#ifdef __nvoc_code_coverage_mgr_h_disabled
static inline GSP_BULLSEYE_COVERAGE_DATA *codecovmgrGetCoverageNode(struct CodeCoverageManager *pCodeCovMgr, NvU32 gfid, NvU32 gpuInstance) {
    NV_ASSERT_FAILED_PRECOMP("CodeCoverageManager was disabled!");
    return NULL;
}
#else //__nvoc_code_coverage_mgr_h_disabled
#define codecovmgrGetCoverageNode(pCodeCovMgr, gfid, gpuInstance) codecovmgrGetCoverageNode_IMPL(pCodeCovMgr, gfid, gpuInstance)
#endif //__nvoc_code_coverage_mgr_h_disabled

NvU8 *codecovmgrGetCoverageBuffer_IMPL(struct CodeCoverageManager *pCodeCovMgr, NvU32 gfid, NvU32 gpuInstance);

#ifdef __nvoc_code_coverage_mgr_h_disabled
static inline NvU8 *codecovmgrGetCoverageBuffer(struct CodeCoverageManager *pCodeCovMgr, NvU32 gfid, NvU32 gpuInstance) {
    NV_ASSERT_FAILED_PRECOMP("CodeCoverageManager was disabled!");
    return NULL;
}
#else //__nvoc_code_coverage_mgr_h_disabled
#define codecovmgrGetCoverageBuffer(pCodeCovMgr, gfid, gpuInstance) codecovmgrGetCoverageBuffer_IMPL(pCodeCovMgr, gfid, gpuInstance)
#endif //__nvoc_code_coverage_mgr_h_disabled

void codecovmgrMergeCoverage_IMPL(struct CodeCoverageManager *pCodeCovMgr, NvU32 gfid, NvU32 gpuInstance, NvU8 *pSysmemBuffer);

#ifdef __nvoc_code_coverage_mgr_h_disabled
static inline void codecovmgrMergeCoverage(struct CodeCoverageManager *pCodeCovMgr, NvU32 gfid, NvU32 gpuInstance, NvU8 *pSysmemBuffer) {
    NV_ASSERT_FAILED_PRECOMP("CodeCoverageManager was disabled!");
}
#else //__nvoc_code_coverage_mgr_h_disabled
#define codecovmgrMergeCoverage(pCodeCovMgr, gfid, gpuInstance, pSysmemBuffer) codecovmgrMergeCoverage_IMPL(pCodeCovMgr, gfid, gpuInstance, pSysmemBuffer)
#endif //__nvoc_code_coverage_mgr_h_disabled

void codecovmgrResetCoverage_IMPL(struct CodeCoverageManager *pCodeCovMgr, NvU32 gfid, NvU32 gpuInstance);

#ifdef __nvoc_code_coverage_mgr_h_disabled
static inline void codecovmgrResetCoverage(struct CodeCoverageManager *pCodeCovMgr, NvU32 gfid, NvU32 gpuInstance) {
    NV_ASSERT_FAILED_PRECOMP("CodeCoverageManager was disabled!");
}
#else //__nvoc_code_coverage_mgr_h_disabled
#define codecovmgrResetCoverage(pCodeCovMgr, gfid, gpuInstance) codecovmgrResetCoverage_IMPL(pCodeCovMgr, gfid, gpuInstance)
#endif //__nvoc_code_coverage_mgr_h_disabled

void codecovmgrRegisterCoverageBuffer_IMPL(struct CodeCoverageManager *pCodeCovMgr, NvU32 gfid, NvU32 gpuInstance, NvU64 bufferSize);

#ifdef __nvoc_code_coverage_mgr_h_disabled
static inline void codecovmgrRegisterCoverageBuffer(struct CodeCoverageManager *pCodeCovMgr, NvU32 gfid, NvU32 gpuInstance, NvU64 bufferSize) {
    NV_ASSERT_FAILED_PRECOMP("CodeCoverageManager was disabled!");
}
#else //__nvoc_code_coverage_mgr_h_disabled
#define codecovmgrRegisterCoverageBuffer(pCodeCovMgr, gfid, gpuInstance, bufferSize) codecovmgrRegisterCoverageBuffer_IMPL(pCodeCovMgr, gfid, gpuInstance, bufferSize)
#endif //__nvoc_code_coverage_mgr_h_disabled

void codecovmgrDeregisterCoverageBuffer_IMPL(struct CodeCoverageManager *pCodeCovMgr, NvU32 gfid, NvU32 gpuInstance);

#ifdef __nvoc_code_coverage_mgr_h_disabled
static inline void codecovmgrDeregisterCoverageBuffer(struct CodeCoverageManager *pCodeCovMgr, NvU32 gfid, NvU32 gpuInstance) {
    NV_ASSERT_FAILED_PRECOMP("CodeCoverageManager was disabled!");
}
#else //__nvoc_code_coverage_mgr_h_disabled
#define codecovmgrDeregisterCoverageBuffer(pCodeCovMgr, gfid, gpuInstance) codecovmgrDeregisterCoverageBuffer_IMPL(pCodeCovMgr, gfid, gpuInstance)
#endif //__nvoc_code_coverage_mgr_h_disabled

#undef PRIVATE_FIELD


#endif //__CODE_COVERAGE_MGR_H__

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_CODE_COVERAGE_MGR_NVOC_H_
