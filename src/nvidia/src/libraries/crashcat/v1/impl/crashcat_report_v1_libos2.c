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

#define NVOC_CRASHCAT_REPORT_H_PRIVATE_ACCESS_ALLOWED
#include "crashcat/crashcat_report.h"
#include "crashcat/crashcat_engine.h"
#include "libos_v2_crashcat.h"
#include "utils/nvprintf.h"
#include "nv-crashcat-decoder.h"

static inline const char *crashcatReportModeToString_LIBOS2(NV_CRASHCAT_RISCV_MODE mode)
{
    switch (mode)
    {
        case NV_CRASHCAT_RISCV_MODE_M:
        case NV_CRASHCAT_RISCV_MODE_S:
            return MAKE_NV_PRINTF_STR("kernel");
        case NV_CRASHCAT_RISCV_MODE_U:
            return MAKE_NV_PRINTF_STR("task");
        default:
            return MAKE_NV_PRINTF_STR("unspecified");
    }
}

static inline const char *crashcatReportPanicReasonToString_LIBOS2(LibosPanicReason reason)
{
#define LIBOS_PANIC_REASON_CASE(reason, str)   \
    case LibosPanicReason ## reason: return MAKE_NV_PRINTF_STR(str)
    switch (reason)
    {
        LIBOS_PANIC_REASON_CASE(UnrecoverableTaskCrash, "unrecoverable task crash");
        LIBOS_PANIC_REASON_CASE(UnhandledState,         "unhandled state");
        LIBOS_PANIC_REASON_CASE(InvalidConfiguration,   "invalid configuration");
        LIBOS_PANIC_REASON_CASE(FatalHardwareError,     "fatal hardware error");
        LIBOS_PANIC_REASON_CASE(InsufficientResources,  "insufficient resources");
        LIBOS_PANIC_REASON_CASE(Timeout,                "timeout");
        LIBOS_PANIC_REASON_CASE(EnvCallFailed,          "environment call failed");
        LIBOS_PANIC_REASON_CASE(SspStackCheckFailed,    "stack smashing detected");
        LIBOS_PANIC_REASON_CASE(AsanMemoryError,        "asan memory error detected");
        LIBOS_PANIC_REASON_CASE(Test,                   "test");
        LIBOS_PANIC_REASON_CASE(ProgrammingError,       "programming error");
        LIBOS_PANIC_REASON_CASE(DebugAssertionFailed,   "debug assertion failed");
        default: return MAKE_NV_PRINTF_STR("unknown error");
    }
}

void crashcatReportLogSource_V1_LIBOS2(CrashCatReport *pReport)
{
    NvCrashCatReport_V1 *pReportV1 = &pReport->v1.report;

    NvU8 taskId = crashcatReportV1SourceLibos2TaskId(pReportV1);

#define CRASHCAT_LOG_LIBOS2_SOURCE(fmt, ...)                                                    \
    if (taskId == NV_CRASHCAT_REPORT_V1_SOURCE_ID_LIBOS2_TASK_ID_UNSPECIFIED)                   \
        crashcatEnginePrintf(pReport->pEngine, NV_TRUE, fmt, __VA_ARGS__ );                     \
    else                                                                                        \
        crashcatEnginePrintf(pReport->pEngine, NV_TRUE, fmt ", task:%u", __VA_ARGS__, taskId)

    const char *pModeStr = crashcatReportModeToString_LIBOS2(crashcatReportV1SourceMode(pReportV1));
    switch (crashcatReportV1SourceCauseType(pReportV1))
    {
        case NV_CRASHCAT_CAUSE_TYPE_EXCEPTION:
        {
            // Kernel or task unhandled exception - the sourceData is the xcause value
            NvU64 xcause = pReportV1->sourceData;
            CRASHCAT_LOG_LIBOS2_SOURCE(
                "%s exception: %s (cause:0x%" NvU64_fmtx ") @ pc:0x%" NvU64_fmtx,
                pModeStr, crashcatReportRiscvCauseToString(xcause), xcause,
                pReportV1->sourcePc);
            break;
        }
        case NV_CRASHCAT_CAUSE_TYPE_TIMEOUT:
        {
            // Task timeout (no way for libos2 to self-report kernel timeouts)
            CRASHCAT_LOG_LIBOS2_SOURCE(
                "%s timeout @ pc:0x%" NvU64_fmtx, pModeStr, pReportV1->sourcePc);
            break;
        }
        case NV_CRASHCAT_CAUSE_TYPE_PANIC:
        {
            // Kernel or task panic
            LibosPanicReason reason = crashcatReportV1SourceCauseLibos2Reason(pReportV1);
            CRASHCAT_LOG_LIBOS2_SOURCE(
                "%s panic: %s (%u) @ pc:0x%" NvU64_fmtx ", aux:0x%" NvU64_fmtx,
                pModeStr, crashcatReportPanicReasonToString_LIBOS2(reason),
                reason, pReportV1->sourcePc, pReportV1->sourceData);
            break;
        }
    }
}

void crashcatReportLogReporter_V1_LIBOS2(CrashCatReport *pReport)
{
    NvCrashCatReport_V1 *pReportV1 = &pReport->v1.report;
    NvU8 taskId = crashcatReportV1ReporterLibos2TaskId(pReportV1);

    //
    // Though this is technically not a separate packet, we use the CRASHCAT_REPORT_LOG_PACKET_TYPE
    // macro to get the correct prefix/indentation for the reporter information.
    //
    if (taskId == NV_CRASHCAT_REPORT_V1_REPORTER_ID_LIBOS2_TASK_ID_UNSPECIFIED)
    {
        CRASHCAT_REPORT_LOG_PACKET_TYPE(pReport, "Reported by libos kernel v%u.%u [%u] @ %u",
            crashcatReportV1ReporterVersionLibos2Major(pReportV1),
            crashcatReportV1ReporterVersionLibos2Minor(pReportV1),
            crashcatReportV1ReporterVersionLibos2Cl(pReportV1),
            crashcatReportV1ReporterTimestamp(pReportV1));
    }
    else
    {
        CRASHCAT_REPORT_LOG_PACKET_TYPE(pReport, "Reported by libos task:%u v%u.%u [%u] @ ts:%u",
            taskId, crashcatReportV1ReporterVersionLibos2Major(pReportV1),
            crashcatReportV1ReporterVersionLibos2Minor(pReportV1),
            crashcatReportV1ReporterVersionLibos2Cl(pReportV1),
            crashcatReportV1ReporterTimestamp(pReportV1));
    }
}
