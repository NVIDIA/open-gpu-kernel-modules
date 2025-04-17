
#ifndef _G_CRASHCAT_REPORT_NVOC_H_
#define _G_CRASHCAT_REPORT_NVOC_H_

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
 * SPDX-FileCopyrightText: Copyright (c) 2023-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#include "g_crashcat_report_nvoc.h"

#ifndef CRASHCAT_REPORT_H
#define CRASHCAT_REPORT_H

#include "nv-crashcat.h"
#include "nvoc/object.h"
#include "utils/nvprintf.h"
#include "nvlog/nvlog_printf.h"
#include "lib/protobuf/prb.h"


struct CrashCatEngine;

#ifndef __NVOC_CLASS_CrashCatEngine_TYPEDEF__
#define __NVOC_CLASS_CrashCatEngine_TYPEDEF__
typedef struct CrashCatEngine CrashCatEngine;
#endif /* __NVOC_CLASS_CrashCatEngine_TYPEDEF__ */

#ifndef __nvoc_class_id_CrashCatEngine
#define __nvoc_class_id_CrashCatEngine 0x654166
#endif /* __nvoc_class_id_CrashCatEngine */



typedef NvU64 CrashCatImplementer;

#define CRASHCAT_IMPLEMENTER_UNSPECIFIED    0ull
#define CRASHCAT_IMPLEMENTER_LIBOS2         0x4C49424F53322E30ull   // "LIBOS2.0"
#define CRASHCAT_IMPLEMENTER_LIBOS3         0x4C49424F53332E31ull   // "LIBOS3.1"

struct CrashCatReportHal {
    unsigned short __nvoc_HalVarIdx;
};
typedef struct CrashCatReportHal CrashCatReportHal;
void __nvoc_init_halspec_CrashCatReportHal(CrashCatReportHal*, NV_CRASHCAT_PACKET_FORMAT_VERSION, CrashCatImplementer);


// Private field names are wrapped in PRIVATE_FIELD, which does nothing for
// the matching C source file, but causes diagnostics to be issued if another
// source file references the field.
#ifdef NVOC_CRASHCAT_REPORT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__CrashCatReport;
struct NVOC_METADATA__Object;

struct __nvoc_inner_struc_CrashCatReport_1__ {
    NvCrashCatReport_V1 report;
    NvCrashCatRiscv64CsrState_V1 riscv64CsrState;
    NvCrashCatRiscv64GprState_V1 riscv64GprState;
    NvCrashCatRiscv64Trace_V1 *pRiscv64StackTrace;
    NvCrashCatRiscv64Trace_V1 *pRiscv64PcTrace;
    NvCrashCatIo32State_V1 *pIo32State;
};



struct CrashCatReport {

    // Metadata starts with RTTI structure.
    union {
         const struct NVOC_METADATA__CrashCatReport *__nvoc_metadata_ptr;
         const struct NVOC_RTTI *__nvoc_rtti;
    };

    // Parent (i.e. superclass or base class) objects
    struct Object __nvoc_base_Object;

    // Ancestor object pointers for `staticCast` feature
    struct Object *__nvoc_pbase_Object;    // obj super
    struct CrashCatReport *__nvoc_pbase_CrashCatReport;    // crashcatReport

    // Vtable with 4 per-object function pointers
    NV_CRASHCAT_CONTAINMENT (*__crashcatReportSourceContainment__)(struct CrashCatReport * /*this*/);  // halified (2 hals)
    void (*__crashcatReportLogReporter__)(struct CrashCatReport * /*this*/);  // halified (3 hals)
    void (*__crashcatReportLogSource__)(struct CrashCatReport * /*this*/);  // halified (3 hals)
    void (*__crashcatReportLogVersionProtobuf__)(struct CrashCatReport * /*this*/, PRB_ENCODER *);  // halified (3 hals)

    // Data members
    struct CrashCatReportHal reportHal;
    struct CrashCatEngine *PRIVATE_FIELD(pEngine);
    NvU32 PRIVATE_FIELD(validTags);
    struct __nvoc_inner_struc_CrashCatReport_1__ PRIVATE_FIELD(v1);
};


// Metadata with per-class RTTI with ancestor(s)
struct NVOC_METADATA__CrashCatReport {
    const struct NVOC_RTTI rtti;
    const struct NVOC_METADATA__Object metadata__Object;
};

#ifndef __NVOC_CLASS_CrashCatReport_TYPEDEF__
#define __NVOC_CLASS_CrashCatReport_TYPEDEF__
typedef struct CrashCatReport CrashCatReport;
#endif /* __NVOC_CLASS_CrashCatReport_TYPEDEF__ */

#ifndef __nvoc_class_id_CrashCatReport
#define __nvoc_class_id_CrashCatReport 0xde4777
#endif /* __nvoc_class_id_CrashCatReport */

// Casting support
extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatReport;

#define __staticCast_CrashCatReport(pThis) \
    ((pThis)->__nvoc_pbase_CrashCatReport)

#ifdef __nvoc_crashcat_report_h_disabled
#define __dynamicCast_CrashCatReport(pThis) ((CrashCatReport*) NULL)
#else //__nvoc_crashcat_report_h_disabled
#define __dynamicCast_CrashCatReport(pThis) \
    ((CrashCatReport*) __nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(CrashCatReport)))
#endif //__nvoc_crashcat_report_h_disabled

NV_STATUS __nvoc_objCreateDynamic_CrashCatReport(CrashCatReport**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_CrashCatReport(CrashCatReport**, Dynamic*, NvU32,
        NV_CRASHCAT_PACKET_FORMAT_VERSION CrashCatReportHal_version, CrashCatImplementer CrashCatReportHal_implementer, void **arg_ppReportBytes, NvLength arg_bytesRemaining);
#define __objCreate_CrashCatReport(ppNewObj, pParent, createFlags, CrashCatReportHal_version, CrashCatReportHal_implementer, arg_ppReportBytes, arg_bytesRemaining) \
    __nvoc_objCreate_CrashCatReport((ppNewObj), staticCast((pParent), Dynamic), (createFlags), CrashCatReportHal_version, CrashCatReportHal_implementer, arg_ppReportBytes, arg_bytesRemaining)


// Wrapper macros
#define crashcatReportSourceContainment_FNPTR(arg_this) arg_this->__crashcatReportSourceContainment__
#define crashcatReportSourceContainment(arg_this) crashcatReportSourceContainment_DISPATCH(arg_this)
#define crashcatReportSourceContainment_HAL(arg_this) crashcatReportSourceContainment_DISPATCH(arg_this)
#define crashcatReportLogReporter_FNPTR(arg_this) arg_this->__crashcatReportLogReporter__
#define crashcatReportLogReporter(arg_this) crashcatReportLogReporter_DISPATCH(arg_this)
#define crashcatReportLogReporter_HAL(arg_this) crashcatReportLogReporter_DISPATCH(arg_this)
#define crashcatReportLogSource_FNPTR(arg_this) arg_this->__crashcatReportLogSource__
#define crashcatReportLogSource(arg_this) crashcatReportLogSource_DISPATCH(arg_this)
#define crashcatReportLogSource_HAL(arg_this) crashcatReportLogSource_DISPATCH(arg_this)
#define crashcatReportLogVersionProtobuf_FNPTR(arg_this) arg_this->__crashcatReportLogVersionProtobuf__
#define crashcatReportLogVersionProtobuf(arg_this, pCrashcatProtobufData) crashcatReportLogVersionProtobuf_DISPATCH(arg_this, pCrashcatProtobufData)
#define crashcatReportLogVersionProtobuf_HAL(arg_this, pCrashcatProtobufData) crashcatReportLogVersionProtobuf_DISPATCH(arg_this, pCrashcatProtobufData)

// Dispatch functions
static inline NV_CRASHCAT_CONTAINMENT crashcatReportSourceContainment_DISPATCH(struct CrashCatReport *arg_this) {
    return arg_this->__crashcatReportSourceContainment__(arg_this);
}

static inline void crashcatReportLogReporter_DISPATCH(struct CrashCatReport *arg_this) {
    arg_this->__crashcatReportLogReporter__(arg_this);
}

static inline void crashcatReportLogSource_DISPATCH(struct CrashCatReport *arg_this) {
    arg_this->__crashcatReportLogSource__(arg_this);
}

static inline void crashcatReportLogVersionProtobuf_DISPATCH(struct CrashCatReport *arg_this, PRB_ENCODER *pCrashcatProtobufData) {
    arg_this->__crashcatReportLogVersionProtobuf__(arg_this, pCrashcatProtobufData);
}

void crashcatReportDestruct_V1(struct CrashCatReport *arg1);


#define __nvoc_crashcatReportDestruct(arg1) crashcatReportDestruct_V1(arg1)
void crashcatReportLogToProtobuf_V1(struct CrashCatReport *arg1, PRB_ENCODER *pCrashcatProtobufData);


#ifdef __nvoc_crashcat_report_h_disabled
static inline void crashcatReportLogToProtobuf(struct CrashCatReport *arg1, PRB_ENCODER *pCrashcatProtobufData) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportLogToProtobuf(arg1, pCrashcatProtobufData) crashcatReportLogToProtobuf_V1(arg1, pCrashcatProtobufData)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportLogToProtobuf_HAL(arg1, pCrashcatProtobufData) crashcatReportLogToProtobuf(arg1, pCrashcatProtobufData)

NvU64 crashcatReportRa_V1(struct CrashCatReport *arg1);


#ifdef __nvoc_crashcat_report_h_disabled
static inline NvU64 crashcatReportRa(struct CrashCatReport *arg1) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
    return 0;
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportRa(arg1) crashcatReportRa_V1(arg1)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportRa_HAL(arg1) crashcatReportRa(arg1)

NvU64 crashcatReportXcause_V1(struct CrashCatReport *arg1);


#ifdef __nvoc_crashcat_report_h_disabled
static inline NvU64 crashcatReportXcause(struct CrashCatReport *arg1) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
    return 0;
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportXcause(arg1) crashcatReportXcause_V1(arg1)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportXcause_HAL(arg1) crashcatReportXcause(arg1)

NvU64 crashcatReportXtval_V1(struct CrashCatReport *arg1);


#ifdef __nvoc_crashcat_report_h_disabled
static inline NvU64 crashcatReportXtval(struct CrashCatReport *arg1) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
    return 0;
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportXtval(arg1) crashcatReportXtval_V1(arg1)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportXtval_HAL(arg1) crashcatReportXtval(arg1)

void *crashcatReportExtract_V1(struct CrashCatReport *arg1, void *pReportBytes, NvLength bytesRemaining);


#ifdef __nvoc_crashcat_report_h_disabled
static inline void *crashcatReportExtract(struct CrashCatReport *arg1, void *pReportBytes, NvLength bytesRemaining) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
    return NULL;
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportExtract(arg1, pReportBytes, bytesRemaining) crashcatReportExtract_V1(arg1, pReportBytes, bytesRemaining)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportExtract_HAL(arg1, pReportBytes, bytesRemaining) crashcatReportExtract(arg1, pReportBytes, bytesRemaining)

void *crashcatReportExtractReport_V1(struct CrashCatReport *arg1, void *pReportBytes, NvLength bytesRemaining);


#ifdef __nvoc_crashcat_report_h_disabled
static inline void *crashcatReportExtractReport(struct CrashCatReport *arg1, void *pReportBytes, NvLength bytesRemaining) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
    return NULL;
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportExtractReport(arg1, pReportBytes, bytesRemaining) crashcatReportExtractReport_V1(arg1, pReportBytes, bytesRemaining)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportExtractReport_HAL(arg1, pReportBytes, bytesRemaining) crashcatReportExtractReport(arg1, pReportBytes, bytesRemaining)

void *crashcatReportExtractRiscv64CsrState_V1(struct CrashCatReport *arg1, void *pReportBytes, NvLength bytesRemaining);


#ifdef __nvoc_crashcat_report_h_disabled
static inline void *crashcatReportExtractRiscv64CsrState(struct CrashCatReport *arg1, void *pReportBytes, NvLength bytesRemaining) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
    return NULL;
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportExtractRiscv64CsrState(arg1, pReportBytes, bytesRemaining) crashcatReportExtractRiscv64CsrState_V1(arg1, pReportBytes, bytesRemaining)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportExtractRiscv64CsrState_HAL(arg1, pReportBytes, bytesRemaining) crashcatReportExtractRiscv64CsrState(arg1, pReportBytes, bytesRemaining)

void *crashcatReportExtractRiscv64GprState_V1(struct CrashCatReport *arg1, void *pReportBytes, NvLength bytesRemaining);


#ifdef __nvoc_crashcat_report_h_disabled
static inline void *crashcatReportExtractRiscv64GprState(struct CrashCatReport *arg1, void *pReportBytes, NvLength bytesRemaining) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
    return NULL;
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportExtractRiscv64GprState(arg1, pReportBytes, bytesRemaining) crashcatReportExtractRiscv64GprState_V1(arg1, pReportBytes, bytesRemaining)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportExtractRiscv64GprState_HAL(arg1, pReportBytes, bytesRemaining) crashcatReportExtractRiscv64GprState(arg1, pReportBytes, bytesRemaining)

void *crashcatReportExtractRiscv64Trace_V1(struct CrashCatReport *arg1, void *pReportBytes, NvLength bytesRemaining);


#ifdef __nvoc_crashcat_report_h_disabled
static inline void *crashcatReportExtractRiscv64Trace(struct CrashCatReport *arg1, void *pReportBytes, NvLength bytesRemaining) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
    return NULL;
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportExtractRiscv64Trace(arg1, pReportBytes, bytesRemaining) crashcatReportExtractRiscv64Trace_V1(arg1, pReportBytes, bytesRemaining)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportExtractRiscv64Trace_HAL(arg1, pReportBytes, bytesRemaining) crashcatReportExtractRiscv64Trace(arg1, pReportBytes, bytesRemaining)

void *crashcatReportExtractIo32State_V1(struct CrashCatReport *arg1, void *pReportBytes, NvLength bytesRemaining);


#ifdef __nvoc_crashcat_report_h_disabled
static inline void *crashcatReportExtractIo32State(struct CrashCatReport *arg1, void *pReportBytes, NvLength bytesRemaining) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
    return NULL;
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportExtractIo32State(arg1, pReportBytes, bytesRemaining) crashcatReportExtractIo32State_V1(arg1, pReportBytes, bytesRemaining)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportExtractIo32State_HAL(arg1, pReportBytes, bytesRemaining) crashcatReportExtractIo32State(arg1, pReportBytes, bytesRemaining)

void crashcatReportLogRiscv64CsrState_V1(struct CrashCatReport *arg1);


#ifdef __nvoc_crashcat_report_h_disabled
static inline void crashcatReportLogRiscv64CsrState(struct CrashCatReport *arg1) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportLogRiscv64CsrState(arg1) crashcatReportLogRiscv64CsrState_V1(arg1)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportLogRiscv64CsrState_HAL(arg1) crashcatReportLogRiscv64CsrState(arg1)

void crashcatReportLogRiscv64GprState_V1(struct CrashCatReport *arg1);


#ifdef __nvoc_crashcat_report_h_disabled
static inline void crashcatReportLogRiscv64GprState(struct CrashCatReport *arg1) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportLogRiscv64GprState(arg1) crashcatReportLogRiscv64GprState_V1(arg1)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportLogRiscv64GprState_HAL(arg1) crashcatReportLogRiscv64GprState(arg1)

void crashcatReportLogRiscv64Traces_V1(struct CrashCatReport *arg1);


#ifdef __nvoc_crashcat_report_h_disabled
static inline void crashcatReportLogRiscv64Traces(struct CrashCatReport *arg1) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportLogRiscv64Traces(arg1) crashcatReportLogRiscv64Traces_V1(arg1)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportLogRiscv64Traces_HAL(arg1) crashcatReportLogRiscv64Traces(arg1)

void crashcatReportLogIo32State_V1(struct CrashCatReport *arg1);


#ifdef __nvoc_crashcat_report_h_disabled
static inline void crashcatReportLogIo32State(struct CrashCatReport *arg1) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportLogIo32State(arg1) crashcatReportLogIo32State_V1(arg1)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportLogIo32State_HAL(arg1) crashcatReportLogIo32State(arg1)

static inline NV_CRASHCAT_CONTAINMENT crashcatReportSourceContainment_3e9f29(struct CrashCatReport *arg1) {
    return NV_CRASHCAT_CONTAINMENT_UNSPECIFIED;
}

NV_CRASHCAT_CONTAINMENT crashcatReportSourceContainment_V1_LIBOS3(struct CrashCatReport *arg1);

void crashcatReportLogReporter_V1_GENERIC(struct CrashCatReport *arg1);

void crashcatReportLogReporter_V1_LIBOS2(struct CrashCatReport *arg1);

void crashcatReportLogReporter_V1_LIBOS3(struct CrashCatReport *arg1);

void crashcatReportLogSource_V1_GENERIC(struct CrashCatReport *arg1);

void crashcatReportLogSource_V1_LIBOS2(struct CrashCatReport *arg1);

void crashcatReportLogSource_V1_LIBOS3(struct CrashCatReport *arg1);

void crashcatReportLogVersionProtobuf_V1_GENERIC(struct CrashCatReport *arg1, PRB_ENCODER *pCrashcatProtobufData);

void crashcatReportLogVersionProtobuf_V1_LIBOS2(struct CrashCatReport *arg1, PRB_ENCODER *pCrashcatProtobufData);

void crashcatReportLogVersionProtobuf_V1_LIBOS3(struct CrashCatReport *arg1, PRB_ENCODER *pCrashcatProtobufData);

NV_STATUS crashcatReportConstruct_IMPL(struct CrashCatReport *arg_, void **arg_ppReportBytes, NvLength arg_bytesRemaining);

#define __nvoc_crashcatReportConstruct(arg_, arg_ppReportBytes, arg_bytesRemaining) crashcatReportConstruct_IMPL(arg_, arg_ppReportBytes, arg_bytesRemaining)
void crashcatReportLog_IMPL(struct CrashCatReport *arg1);

#ifdef __nvoc_crashcat_report_h_disabled
static inline void crashcatReportLog(struct CrashCatReport *arg1) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportLog(arg1) crashcatReportLog_IMPL(arg1)
#endif //__nvoc_crashcat_report_h_disabled

#undef PRIVATE_FIELD

#ifndef NVOC_CRASHCAT_REPORT_H_PRIVATE_ACCESS_ALLOWED
#undef crashcatReportLogReporter
void NVOC_PRIVATE_FUNCTION(crashcatReportLogReporter)(struct CrashCatReport *arg1);

#undef crashcatReportLogReporter_HAL
void NVOC_PRIVATE_FUNCTION(crashcatReportLogReporter_HAL)(struct CrashCatReport *arg1);

#undef crashcatReportLogSource
void NVOC_PRIVATE_FUNCTION(crashcatReportLogSource)(struct CrashCatReport *arg1);

#undef crashcatReportLogSource_HAL
void NVOC_PRIVATE_FUNCTION(crashcatReportLogSource_HAL)(struct CrashCatReport *arg1);

#undef crashcatReportLogVersionProtobuf
void NVOC_PRIVATE_FUNCTION(crashcatReportLogVersionProtobuf)(struct CrashCatReport *arg1, PRB_ENCODER *pCrashcatProtobufData);

#undef crashcatReportLogVersionProtobuf_HAL
void NVOC_PRIVATE_FUNCTION(crashcatReportLogVersionProtobuf_HAL)(struct CrashCatReport *arg1, PRB_ENCODER *pCrashcatProtobufData);

#ifndef __nvoc_crashcat_report_h_disabled
#undef crashcatReportExtract
void *NVOC_PRIVATE_FUNCTION(crashcatReportExtract)(struct CrashCatReport *arg1, void *pReportBytes, NvLength bytesRemaining);
#endif //__nvoc_crashcat_report_h_disabled

#ifndef __nvoc_crashcat_report_h_disabled
#undef crashcatReportExtractReport
void *NVOC_PRIVATE_FUNCTION(crashcatReportExtractReport)(struct CrashCatReport *arg1, void *pReportBytes, NvLength bytesRemaining);
#endif //__nvoc_crashcat_report_h_disabled

#ifndef __nvoc_crashcat_report_h_disabled
#undef crashcatReportExtractRiscv64CsrState
void *NVOC_PRIVATE_FUNCTION(crashcatReportExtractRiscv64CsrState)(struct CrashCatReport *arg1, void *pReportBytes, NvLength bytesRemaining);
#endif //__nvoc_crashcat_report_h_disabled

#ifndef __nvoc_crashcat_report_h_disabled
#undef crashcatReportExtractRiscv64GprState
void *NVOC_PRIVATE_FUNCTION(crashcatReportExtractRiscv64GprState)(struct CrashCatReport *arg1, void *pReportBytes, NvLength bytesRemaining);
#endif //__nvoc_crashcat_report_h_disabled

#ifndef __nvoc_crashcat_report_h_disabled
#undef crashcatReportExtractRiscv64Trace
void *NVOC_PRIVATE_FUNCTION(crashcatReportExtractRiscv64Trace)(struct CrashCatReport *arg1, void *pReportBytes, NvLength bytesRemaining);
#endif //__nvoc_crashcat_report_h_disabled

#ifndef __nvoc_crashcat_report_h_disabled
#undef crashcatReportExtractIo32State
void *NVOC_PRIVATE_FUNCTION(crashcatReportExtractIo32State)(struct CrashCatReport *arg1, void *pReportBytes, NvLength bytesRemaining);
#endif //__nvoc_crashcat_report_h_disabled

#ifndef __nvoc_crashcat_report_h_disabled
#undef crashcatReportLogRiscv64CsrState
void NVOC_PRIVATE_FUNCTION(crashcatReportLogRiscv64CsrState)(struct CrashCatReport *arg1);
#endif //__nvoc_crashcat_report_h_disabled

#ifndef __nvoc_crashcat_report_h_disabled
#undef crashcatReportLogRiscv64GprState
void NVOC_PRIVATE_FUNCTION(crashcatReportLogRiscv64GprState)(struct CrashCatReport *arg1);
#endif //__nvoc_crashcat_report_h_disabled

#ifndef __nvoc_crashcat_report_h_disabled
#undef crashcatReportLogRiscv64Traces
void NVOC_PRIVATE_FUNCTION(crashcatReportLogRiscv64Traces)(struct CrashCatReport *arg1);
#endif //__nvoc_crashcat_report_h_disabled

#ifndef __nvoc_crashcat_report_h_disabled
#undef crashcatReportLogIo32State
void NVOC_PRIVATE_FUNCTION(crashcatReportLogIo32State)(struct CrashCatReport *arg1);
#endif //__nvoc_crashcat_report_h_disabled

#endif // NVOC_CRASHCAT_REPORT_H_PRIVATE_ACCESS_ALLOWED


// Utility to convert a cause code to a user-friendly string
const char *crashcatReportRiscvCauseToString(NvU64 xcause);

// Log indentation used for multi-line reports
#define CRASHCAT_LOG_INDENT "    "

// Prefix used for multi-line reports
#if defined(NVRM)
#define CRASHCAT_LOG_PREFIX "NVRM: "
#else
#define CRASHCAT_LOG_PREFIX
#endif

#define CRASHCAT_REPORT_LOG_PACKET_TYPE(pReport, fmt, ...)                  \
    portDbgPrintf(CRASHCAT_LOG_PREFIX CRASHCAT_LOG_INDENT fmt, ##__VA_ARGS__);            \
    NVLOG_PRINTF(NV_PRINTF_MODULE, NVLOG_ROUTE_RM, LEVEL_ERROR,         \
                CRASHCAT_LOG_PREFIX CRASHCAT_LOG_INDENT fmt, ##__VA_ARGS__)

#define CRASHCAT_REPORT_LOG_DATA(pReport, fmt, ...)                         \
    portDbgPrintf(CRASHCAT_LOG_PREFIX CRASHCAT_LOG_INDENT CRASHCAT_LOG_INDENT fmt, ##__VA_ARGS__);            \
    NVLOG_PRINTF(NV_PRINTF_MODULE, NVLOG_ROUTE_RM, LEVEL_ERROR,         \
                CRASHCAT_LOG_PREFIX CRASHCAT_LOG_INDENT CRASHCAT_LOG_INDENT fmt, ##__VA_ARGS__)

#endif // CRASHCAT_REPORT_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_CRASHCAT_REPORT_NVOC_H_
