#ifndef _G_CRASHCAT_REPORT_NVOC_H_
#define _G_CRASHCAT_REPORT_NVOC_H_
#include "nvoc/runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * SPDX-FileCopyrightText: Copyright (c) 2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "g_crashcat_report_nvoc.h"

#ifndef CRASHCAT_REPORT_H
#define CRASHCAT_REPORT_H

#include "nv-crashcat.h"
#include "nvoc/object.h"

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

#ifdef NVOC_CRASHCAT_REPORT_H_PRIVATE_ACCESS_ALLOWED
#define PRIVATE_FIELD(x) x
#else
#define PRIVATE_FIELD(x) NVOC_PRIVATE_FIELD(x)
#endif
struct __nvoc_inner_struc_CrashCatReport_1__ {
    NvCrashCatReport_V1 report;
    NvCrashCatRiscv64CsrState_V1 riscv64CsrState;
    NvCrashCatRiscv64GprState_V1 riscv64GprState;
    NvCrashCatRiscv64Trace_V1 *pRiscv64Trace;
    NvCrashCatIo32State_V1 *pIo32State;
};


struct CrashCatReport {
    const struct NVOC_RTTI *__nvoc_rtti;
    struct Object __nvoc_base_Object;
    struct Object *__nvoc_pbase_Object;
    struct CrashCatReport *__nvoc_pbase_CrashCatReport;
    void (*__crashcatReportLogReporter__)(struct CrashCatReport *);
    void (*__crashcatReportLogSource__)(struct CrashCatReport *);
    struct CrashCatReportHal reportHal;
    struct CrashCatEngine *PRIVATE_FIELD(pEngine);
    NvU32 PRIVATE_FIELD(validTags);
    struct __nvoc_inner_struc_CrashCatReport_1__ PRIVATE_FIELD(v1);
};

#ifndef __NVOC_CLASS_CrashCatReport_TYPEDEF__
#define __NVOC_CLASS_CrashCatReport_TYPEDEF__
typedef struct CrashCatReport CrashCatReport;
#endif /* __NVOC_CLASS_CrashCatReport_TYPEDEF__ */

#ifndef __nvoc_class_id_CrashCatReport
#define __nvoc_class_id_CrashCatReport 0xde4777
#endif /* __nvoc_class_id_CrashCatReport */

extern const struct NVOC_CLASS_DEF __nvoc_class_def_CrashCatReport;

#define __staticCast_CrashCatReport(pThis) \
    ((pThis)->__nvoc_pbase_CrashCatReport)

#ifdef __nvoc_crashcat_report_h_disabled
#define __dynamicCast_CrashCatReport(pThis) ((CrashCatReport*)NULL)
#else //__nvoc_crashcat_report_h_disabled
#define __dynamicCast_CrashCatReport(pThis) \
    ((CrashCatReport*)__nvoc_dynamicCast(staticCast((pThis), Dynamic), classInfo(CrashCatReport)))
#endif //__nvoc_crashcat_report_h_disabled


NV_STATUS __nvoc_objCreateDynamic_CrashCatReport(CrashCatReport**, Dynamic*, NvU32, va_list);

NV_STATUS __nvoc_objCreate_CrashCatReport(CrashCatReport**, Dynamic*, NvU32,
        NV_CRASHCAT_PACKET_FORMAT_VERSION CrashCatReportHal_version, CrashCatImplementer CrashCatReportHal_implementer, void ** arg_ppReportBytes, NvLength arg_bytesRemaining);
#define __objCreate_CrashCatReport(ppNewObj, pParent, createFlags, CrashCatReportHal_version, CrashCatReportHal_implementer, arg_ppReportBytes, arg_bytesRemaining) \
    __nvoc_objCreate_CrashCatReport((ppNewObj), staticCast((pParent), Dynamic), (createFlags), CrashCatReportHal_version, CrashCatReportHal_implementer, arg_ppReportBytes, arg_bytesRemaining)

#define crashcatReportLogReporter(arg0) crashcatReportLogReporter_DISPATCH(arg0)
#define crashcatReportLogReporter_HAL(arg0) crashcatReportLogReporter_DISPATCH(arg0)
#define crashcatReportLogSource(arg0) crashcatReportLogSource_DISPATCH(arg0)
#define crashcatReportLogSource_HAL(arg0) crashcatReportLogSource_DISPATCH(arg0)
void crashcatReportDestruct_V1(struct CrashCatReport *arg0);


#define __nvoc_crashcatReportDestruct(arg0) crashcatReportDestruct_V1(arg0)
void *crashcatReportExtract_V1(struct CrashCatReport *arg0, void *pReportBytes, NvLength bytesRemaining);


#ifdef __nvoc_crashcat_report_h_disabled
static inline void *crashcatReportExtract(struct CrashCatReport *arg0, void *pReportBytes, NvLength bytesRemaining) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
    return NULL;
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportExtract(arg0, pReportBytes, bytesRemaining) crashcatReportExtract_V1(arg0, pReportBytes, bytesRemaining)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportExtract_HAL(arg0, pReportBytes, bytesRemaining) crashcatReportExtract(arg0, pReportBytes, bytesRemaining)

void *crashcatReportExtractReport_V1(struct CrashCatReport *arg0, void *pReportBytes, NvLength bytesRemaining);


#ifdef __nvoc_crashcat_report_h_disabled
static inline void *crashcatReportExtractReport(struct CrashCatReport *arg0, void *pReportBytes, NvLength bytesRemaining) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
    return NULL;
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportExtractReport(arg0, pReportBytes, bytesRemaining) crashcatReportExtractReport_V1(arg0, pReportBytes, bytesRemaining)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportExtractReport_HAL(arg0, pReportBytes, bytesRemaining) crashcatReportExtractReport(arg0, pReportBytes, bytesRemaining)

void *crashcatReportExtractRiscv64CsrState_V1(struct CrashCatReport *arg0, void *pReportBytes, NvLength bytesRemaining);


#ifdef __nvoc_crashcat_report_h_disabled
static inline void *crashcatReportExtractRiscv64CsrState(struct CrashCatReport *arg0, void *pReportBytes, NvLength bytesRemaining) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
    return NULL;
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportExtractRiscv64CsrState(arg0, pReportBytes, bytesRemaining) crashcatReportExtractRiscv64CsrState_V1(arg0, pReportBytes, bytesRemaining)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportExtractRiscv64CsrState_HAL(arg0, pReportBytes, bytesRemaining) crashcatReportExtractRiscv64CsrState(arg0, pReportBytes, bytesRemaining)

void *crashcatReportExtractRiscv64GprState_V1(struct CrashCatReport *arg0, void *pReportBytes, NvLength bytesRemaining);


#ifdef __nvoc_crashcat_report_h_disabled
static inline void *crashcatReportExtractRiscv64GprState(struct CrashCatReport *arg0, void *pReportBytes, NvLength bytesRemaining) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
    return NULL;
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportExtractRiscv64GprState(arg0, pReportBytes, bytesRemaining) crashcatReportExtractRiscv64GprState_V1(arg0, pReportBytes, bytesRemaining)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportExtractRiscv64GprState_HAL(arg0, pReportBytes, bytesRemaining) crashcatReportExtractRiscv64GprState(arg0, pReportBytes, bytesRemaining)

void *crashcatReportExtractRiscv64Trace_V1(struct CrashCatReport *arg0, void *pReportBytes, NvLength bytesRemaining);


#ifdef __nvoc_crashcat_report_h_disabled
static inline void *crashcatReportExtractRiscv64Trace(struct CrashCatReport *arg0, void *pReportBytes, NvLength bytesRemaining) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
    return NULL;
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportExtractRiscv64Trace(arg0, pReportBytes, bytesRemaining) crashcatReportExtractRiscv64Trace_V1(arg0, pReportBytes, bytesRemaining)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportExtractRiscv64Trace_HAL(arg0, pReportBytes, bytesRemaining) crashcatReportExtractRiscv64Trace(arg0, pReportBytes, bytesRemaining)

void *crashcatReportExtractIo32State_V1(struct CrashCatReport *arg0, void *pReportBytes, NvLength bytesRemaining);


#ifdef __nvoc_crashcat_report_h_disabled
static inline void *crashcatReportExtractIo32State(struct CrashCatReport *arg0, void *pReportBytes, NvLength bytesRemaining) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
    return NULL;
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportExtractIo32State(arg0, pReportBytes, bytesRemaining) crashcatReportExtractIo32State_V1(arg0, pReportBytes, bytesRemaining)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportExtractIo32State_HAL(arg0, pReportBytes, bytesRemaining) crashcatReportExtractIo32State(arg0, pReportBytes, bytesRemaining)

void crashcatReportLogRiscv64CsrState_V1(struct CrashCatReport *arg0);


#ifdef __nvoc_crashcat_report_h_disabled
static inline void crashcatReportLogRiscv64CsrState(struct CrashCatReport *arg0) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportLogRiscv64CsrState(arg0) crashcatReportLogRiscv64CsrState_V1(arg0)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportLogRiscv64CsrState_HAL(arg0) crashcatReportLogRiscv64CsrState(arg0)

void crashcatReportLogRiscv64GprState_V1(struct CrashCatReport *arg0);


#ifdef __nvoc_crashcat_report_h_disabled
static inline void crashcatReportLogRiscv64GprState(struct CrashCatReport *arg0) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportLogRiscv64GprState(arg0) crashcatReportLogRiscv64GprState_V1(arg0)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportLogRiscv64GprState_HAL(arg0) crashcatReportLogRiscv64GprState(arg0)

void crashcatReportLogRiscv64Trace_V1(struct CrashCatReport *arg0);


#ifdef __nvoc_crashcat_report_h_disabled
static inline void crashcatReportLogRiscv64Trace(struct CrashCatReport *arg0) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportLogRiscv64Trace(arg0) crashcatReportLogRiscv64Trace_V1(arg0)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportLogRiscv64Trace_HAL(arg0) crashcatReportLogRiscv64Trace(arg0)

void crashcatReportLogIo32State_V1(struct CrashCatReport *arg0);


#ifdef __nvoc_crashcat_report_h_disabled
static inline void crashcatReportLogIo32State(struct CrashCatReport *arg0) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportLogIo32State(arg0) crashcatReportLogIo32State_V1(arg0)
#endif //__nvoc_crashcat_report_h_disabled

#define crashcatReportLogIo32State_HAL(arg0) crashcatReportLogIo32State(arg0)

void crashcatReportLogReporter_V1_GENERIC(struct CrashCatReport *arg0);

void crashcatReportLogReporter_V1_LIBOS2(struct CrashCatReport *arg0);

static inline void crashcatReportLogReporter_DISPATCH(struct CrashCatReport *arg0) {
    arg0->__crashcatReportLogReporter__(arg0);
}

void crashcatReportLogSource_V1_GENERIC(struct CrashCatReport *arg0);

void crashcatReportLogSource_V1_LIBOS2(struct CrashCatReport *arg0);

static inline void crashcatReportLogSource_DISPATCH(struct CrashCatReport *arg0) {
    arg0->__crashcatReportLogSource__(arg0);
}

NV_STATUS crashcatReportConstruct_IMPL(struct CrashCatReport *arg_, void **arg_ppReportBytes, NvLength arg_bytesRemaining);

#define __nvoc_crashcatReportConstruct(arg_, arg_ppReportBytes, arg_bytesRemaining) crashcatReportConstruct_IMPL(arg_, arg_ppReportBytes, arg_bytesRemaining)
void crashcatReportLog_IMPL(struct CrashCatReport *arg0);

#ifdef __nvoc_crashcat_report_h_disabled
static inline void crashcatReportLog(struct CrashCatReport *arg0) {
    NV_ASSERT_FAILED_PRECOMP("CrashCatReport was disabled!");
}
#else //__nvoc_crashcat_report_h_disabled
#define crashcatReportLog(arg0) crashcatReportLog_IMPL(arg0)
#endif //__nvoc_crashcat_report_h_disabled

#undef PRIVATE_FIELD

#ifndef NVOC_CRASHCAT_REPORT_H_PRIVATE_ACCESS_ALLOWED
#undef crashcatReportLogReporter
void NVOC_PRIVATE_FUNCTION(crashcatReportLogReporter)(struct CrashCatReport *arg0);

#undef crashcatReportLogReporter_HAL
void NVOC_PRIVATE_FUNCTION(crashcatReportLogReporter_HAL)(struct CrashCatReport *arg0);

#undef crashcatReportLogSource
void NVOC_PRIVATE_FUNCTION(crashcatReportLogSource)(struct CrashCatReport *arg0);

#undef crashcatReportLogSource_HAL
void NVOC_PRIVATE_FUNCTION(crashcatReportLogSource_HAL)(struct CrashCatReport *arg0);

#ifndef __nvoc_crashcat_report_h_disabled
#undef crashcatReportExtract
void *NVOC_PRIVATE_FUNCTION(crashcatReportExtract)(struct CrashCatReport *arg0, void *pReportBytes, NvLength bytesRemaining);
#endif //__nvoc_crashcat_report_h_disabled

#ifndef __nvoc_crashcat_report_h_disabled
#undef crashcatReportExtractReport
void *NVOC_PRIVATE_FUNCTION(crashcatReportExtractReport)(struct CrashCatReport *arg0, void *pReportBytes, NvLength bytesRemaining);
#endif //__nvoc_crashcat_report_h_disabled

#ifndef __nvoc_crashcat_report_h_disabled
#undef crashcatReportExtractRiscv64CsrState
void *NVOC_PRIVATE_FUNCTION(crashcatReportExtractRiscv64CsrState)(struct CrashCatReport *arg0, void *pReportBytes, NvLength bytesRemaining);
#endif //__nvoc_crashcat_report_h_disabled

#ifndef __nvoc_crashcat_report_h_disabled
#undef crashcatReportExtractRiscv64GprState
void *NVOC_PRIVATE_FUNCTION(crashcatReportExtractRiscv64GprState)(struct CrashCatReport *arg0, void *pReportBytes, NvLength bytesRemaining);
#endif //__nvoc_crashcat_report_h_disabled

#ifndef __nvoc_crashcat_report_h_disabled
#undef crashcatReportExtractRiscv64Trace
void *NVOC_PRIVATE_FUNCTION(crashcatReportExtractRiscv64Trace)(struct CrashCatReport *arg0, void *pReportBytes, NvLength bytesRemaining);
#endif //__nvoc_crashcat_report_h_disabled

#ifndef __nvoc_crashcat_report_h_disabled
#undef crashcatReportExtractIo32State
void *NVOC_PRIVATE_FUNCTION(crashcatReportExtractIo32State)(struct CrashCatReport *arg0, void *pReportBytes, NvLength bytesRemaining);
#endif //__nvoc_crashcat_report_h_disabled

#ifndef __nvoc_crashcat_report_h_disabled
#undef crashcatReportLogRiscv64CsrState
void NVOC_PRIVATE_FUNCTION(crashcatReportLogRiscv64CsrState)(struct CrashCatReport *arg0);
#endif //__nvoc_crashcat_report_h_disabled

#ifndef __nvoc_crashcat_report_h_disabled
#undef crashcatReportLogRiscv64GprState
void NVOC_PRIVATE_FUNCTION(crashcatReportLogRiscv64GprState)(struct CrashCatReport *arg0);
#endif //__nvoc_crashcat_report_h_disabled

#ifndef __nvoc_crashcat_report_h_disabled
#undef crashcatReportLogRiscv64Trace
void NVOC_PRIVATE_FUNCTION(crashcatReportLogRiscv64Trace)(struct CrashCatReport *arg0);
#endif //__nvoc_crashcat_report_h_disabled

#ifndef __nvoc_crashcat_report_h_disabled
#undef crashcatReportLogIo32State
void NVOC_PRIVATE_FUNCTION(crashcatReportLogIo32State)(struct CrashCatReport *arg0);
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
    crashcatEnginePrintf(pReport->pEngine, NV_FALSE,                        \
        CRASHCAT_LOG_PREFIX CRASHCAT_LOG_INDENT fmt, ##__VA_ARGS__)
#define CRASHCAT_REPORT_LOG_DATA(pReport, fmt, ...)                         \
    crashcatEnginePrintf(pReport->pEngine, NV_FALSE,                        \
        CRASHCAT_LOG_PREFIX CRASHCAT_LOG_INDENT CRASHCAT_LOG_INDENT fmt,    \
        ##__VA_ARGS__)

#endif // CRASHCAT_REPORT_H

#ifdef __cplusplus
} // extern "C"
#endif

#endif // _G_CRASHCAT_REPORT_NVOC_H_
