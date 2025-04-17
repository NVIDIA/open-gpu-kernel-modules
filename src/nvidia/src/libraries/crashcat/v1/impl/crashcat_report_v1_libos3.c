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

#define NVOC_CRASHCAT_REPORT_H_PRIVATE_ACCESS_ALLOWED
#include "crashcat/crashcat_report.h"
#include "crashcat/crashcat_engine.h"
#include "libos_v3_crashcat.h"
#include "utils/nvprintf.h"
#include "nv-crashcat-decoder.h"

#include "g_all_dcl_pb.h"

static inline const char *crashcatReportModeToString_LIBOS3(NV_CRASHCAT_RISCV_MODE mode)
{
    switch (mode)
    {
        case NV_CRASHCAT_RISCV_MODE_M:
            return MAKE_NV_PRINTF_STR("monitor");
        case NV_CRASHCAT_RISCV_MODE_S:
            return MAKE_NV_PRINTF_STR("kernel");
        case NV_CRASHCAT_RISCV_MODE_U:
            return MAKE_NV_PRINTF_STR("task");
        default:
            return MAKE_NV_PRINTF_STR("unspecified");
    }
}

static inline const char *crashcatReportPanicReasonToString_LIBOS3(LibosPanicReason reason)
{
#define LIBOS_PANIC_REASON_CASE(reason, str)   \
    case LibosPanicReason ## reason: return MAKE_NV_PRINTF_STR(str)

    switch (reason)
    {
        LIBOS_PANIC_REASON_CASE(UnhandledState,         "unhandled state");
        LIBOS_PANIC_REASON_CASE(InvalidConfiguration,   "invalid configuration");
        LIBOS_PANIC_REASON_CASE(FatalHardwareError,     "fatal hardware error");
        LIBOS_PANIC_REASON_CASE(InsufficientResources,  "insufficient resources");
        LIBOS_PANIC_REASON_CASE(Timeout,                "timeout");
        LIBOS_PANIC_REASON_CASE(EnvCallFailed,          "environment call failed");
        LIBOS_PANIC_REASON_CASE(AsanMemoryError,        "asan memory error detected");
        LIBOS_PANIC_REASON_CASE(ProgrammingError,       "programming error");
        LIBOS_PANIC_REASON_CASE(AssertionFailed,        "condition failed");
        LIBOS_PANIC_REASON_CASE(TrapKernelPanic,        "unhandled trap");
        LIBOS_PANIC_REASON_CASE(TrapInstruction,        "instruction access fault");
        default: return MAKE_NV_PRINTF_STR("unknown error");
    }
}

void crashcatReportLogSource_V1_LIBOS3(CrashCatReport *pReport)
{
    NvCrashCatReport_V1 *pReportV1 = &pReport->v1.report;

    NvU8 taskId = crashcatReportV1SourceLibos3TaskId(pReportV1);
    NvCrashCatNvriscvPartition sourcePartition = crashcatReportV1SourcePartition(pReportV1);
    NvCrashCatNvriscvUcodeId sourceUcodeId = crashcatReportV1SourceUcodeId(pReportV1);    

#define CRASHCAT_LOG_LIBOS3_SOURCE(fmt, ...)                                                     \
    if (taskId == NV_CRASHCAT_REPORT_V1_SOURCE_ID_LIBOS3_TASK_ID_UNSPECIFIED)                    \
        crashcatEnginePrintf(pReport->pEngine, NV_TRUE, fmt ", partition:%u#%u",                 \
                             __VA_ARGS__, sourcePartition, sourceUcodeId);                       \
    else                                                                                         \
        crashcatEnginePrintf(pReport->pEngine, NV_TRUE, fmt ", partition:%u#%u, task:%u",        \
                             __VA_ARGS__, sourcePartition, sourceUcodeId, taskId)

    const char *pModeStr = crashcatReportModeToString_LIBOS3(crashcatReportV1SourceMode(pReportV1));
    switch (crashcatReportV1SourceCauseType(pReportV1))
    {
        case NV_CRASHCAT_CAUSE_TYPE_EXCEPTION:
        {
            // Kernel or task unhandled exception - the sourceData is the xcause value
            NvU64 xcause = pReportV1->sourceData;
            CRASHCAT_LOG_LIBOS3_SOURCE(
                "%s exception: %s (cause:0x%" NvU64_fmtx ") @ pc:0x%" NvU64_fmtx,
                pModeStr, crashcatReportRiscvCauseToString(xcause),
                xcause, pReportV1->sourcePc);
            break;
        }
        case NV_CRASHCAT_CAUSE_TYPE_TIMEOUT:
        {
            // Task timeout (no way for libos3 to self-report kernel timeouts)
            CRASHCAT_LOG_LIBOS3_SOURCE(
                "%s timeout @ pc:0x%" NvU64_fmtx,
                pModeStr, pReportV1->sourcePc);
            break;
        }
        case NV_CRASHCAT_CAUSE_TYPE_PANIC:
        {
            // Kernel or task panic
            LibosPanicReason reason = crashcatReportV1SourceCauseLibos3Reason(pReportV1);
            CRASHCAT_LOG_LIBOS3_SOURCE(
                "%s panic: %s (%u) @ pc:0x%" NvU64_fmtx ", aux:0x%" NvU64_fmtx,
                pModeStr, crashcatReportPanicReasonToString_LIBOS3(reason), reason,
                pReportV1->sourcePc, pReportV1->sourceData);
            break;
        }
    }
}

NV_CRASHCAT_CONTAINMENT crashcatReportSourceContainment_V1_LIBOS3(CrashCatReport *pReport)
{
    return crashcatReportV1SourceCauseContainment(&pReport->v1.report);
}

void crashcatReportLogReporter_V1_LIBOS3(CrashCatReport *pReport)
{
    NvCrashCatReport_V1 *pReportV1 = &pReport->v1.report;
    NvU8 taskId = crashcatReportV1ReporterLibos3TaskId(pReportV1);

    //
    // Though this is technically not a separate packet, we use the CRASHCAT_REPORT_LOG_PACKET_TYPE
    // macro to get the correct prefix/indentation for the reporter information.
    //
    if (taskId == NV_CRASHCAT_REPORT_V1_REPORTER_ID_LIBOS3_TASK_ID_UNSPECIFIED)
    {
        CRASHCAT_REPORT_LOG_PACKET_TYPE(pReport,
            "Reported by libos partition:%u#%u kernel v%u.%u [%u] @ ts:%u\n",
            crashcatReportV1ReporterPartition(pReportV1),
            crashcatReportV1ReporterUcodeId(pReportV1),
            crashcatReportV1ReporterVersionLibos3Major(pReportV1),
            crashcatReportV1ReporterVersionLibos3Minor(pReportV1),
            crashcatReportV1ReporterVersionLibos3Cl(pReportV1),
            crashcatReportV1ReporterTimestamp(pReportV1));
    }
    else
    {
        CRASHCAT_REPORT_LOG_PACKET_TYPE(pReport,
            "Reported by libos partition:%u#%u task:%u v%u.%u [%u] @ ts:%u\n",
            crashcatReportV1ReporterPartition(pReportV1),
            crashcatReportV1ReporterUcodeId(pReportV1),
            taskId, crashcatReportV1ReporterVersionLibos3Major(pReportV1),
            crashcatReportV1ReporterVersionLibos3Minor(pReportV1),
            crashcatReportV1ReporterVersionLibos3Cl(pReportV1),
            crashcatReportV1ReporterTimestamp(pReportV1));
    }
}

void crashcatReportLogVersionProtobuf_V1_LIBOS3(CrashCatReport *pReport, PRB_ENCODER *pCrashcatProtobufData)
{
    NvCrashCatReport_V1 *pReportV1 = &pReport->v1.report;

#define LIBOS_PANIC_REASON_PROTOBUF_CASE(reason, reasonUpper)   \
    case LibosPanicReason ## reason: prbEncAddEnum(pCrashcatProtobufData, CRASHCAT_REPORT_LIBOSPANICREASON, CRASHCAT_REPORT_LIBOSPANICREASON ## reasonUpper)

    if (crashcatReportV1SourceCauseType(pReportV1) == NV_CRASHCAT_CAUSE_TYPE_PANIC)
    {
        switch (crashcatReportV1SourceCauseLibos3Reason(pReportV1))
        {
            LIBOS_PANIC_REASON_PROTOBUF_CASE(UnhandledState, UNHANDLEDSTATE);
            LIBOS_PANIC_REASON_PROTOBUF_CASE(InvalidConfiguration, INVALIDCONFIGURATION);
            LIBOS_PANIC_REASON_PROTOBUF_CASE(FatalHardwareError, FATALHARDWAREERROR);
            LIBOS_PANIC_REASON_PROTOBUF_CASE(InsufficientResources, INSUFFICIENTRESOURCES);
            LIBOS_PANIC_REASON_PROTOBUF_CASE(Timeout, TIMEOUT);
            LIBOS_PANIC_REASON_PROTOBUF_CASE(EnvCallFailed, ENVCALLFAILED);
            LIBOS_PANIC_REASON_PROTOBUF_CASE(AsanMemoryError, ASANMEMORYERROR);
            LIBOS_PANIC_REASON_PROTOBUF_CASE(ProgrammingError, PROGRAMMINGERROR);
            LIBOS_PANIC_REASON_PROTOBUF_CASE(AssertionFailed, ASSERTIONFAILED);
            LIBOS_PANIC_REASON_PROTOBUF_CASE(TrapKernelPanic, TRAPKERNELPANIC);
            LIBOS_PANIC_REASON_PROTOBUF_CASE(TrapInstruction, TRAPINSTRUCTION);
            default: prbEncAddEnum(pCrashcatProtobufData, CRASHCAT_REPORT_LIBOSPANICREASON, CRASHCAT_REPORT_LIBOSPANICREASONUNSPECIFIED);
        }
    }

    prbEncAddUInt32(pCrashcatProtobufData, CRASHCAT_REPORT_LIBOSVERSIONMAJOR, crashcatReportV1ReporterVersionLibos3Major(pReportV1));
    prbEncAddUInt32(pCrashcatProtobufData, CRASHCAT_REPORT_LIBOSVERSIONMINOR, crashcatReportV1ReporterVersionLibos3Minor(pReportV1));
    prbEncAddUInt32(pCrashcatProtobufData, CRASHCAT_REPORT_LIBOSVERSIONCL, crashcatReportV1ReporterVersionLibos3Cl(pReportV1));
}