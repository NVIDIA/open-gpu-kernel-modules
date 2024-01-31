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
#include "nv-crashcat-decoder.h"

static inline const char crashcatReportModeToChar_GENERIC(NV_CRASHCAT_RISCV_MODE mode)
{
    switch (mode)
    {
        case NV_CRASHCAT_RISCV_MODE_M: return 'M';
        case NV_CRASHCAT_RISCV_MODE_S: return 'S';
        case NV_CRASHCAT_RISCV_MODE_U: return 'U';
        default: return '?';
    }
}

static inline const char *crashcatReportCauseTypeToString_GENERIC(NV_CRASHCAT_CAUSE_TYPE causeType)
{
    switch (causeType)
    {
        case NV_CRASHCAT_CAUSE_TYPE_EXCEPTION: return MAKE_NV_PRINTF_STR("exception");
        case NV_CRASHCAT_CAUSE_TYPE_PANIC: return MAKE_NV_PRINTF_STR("panic");
        case NV_CRASHCAT_CAUSE_TYPE_TIMEOUT: return MAKE_NV_PRINTF_STR("timeout");
        default: return MAKE_NV_PRINTF_STR("unknown failure");
    }
}

void crashcatReportLogSource_V1_GENERIC(CrashCatReport *pReport)
{
    NvCrashCatReport_V1 *pReportV1 = &pReport->v1.report;
    NvCrashCatNvriscvPartition partition = crashcatReportV1SourcePartition(pReportV1);
    NvCrashCatNvriscvUcodeId ucodeId = crashcatReportV1SourceUcodeId(pReportV1);
    NV_CRASHCAT_RISCV_MODE riscvMode = crashcatReportV1SourceMode(pReportV1);
    NV_CRASHCAT_CAUSE_TYPE causeType = crashcatReportV1SourceCauseType(pReportV1);

    crashcatEnginePrintf(pReport->pEngine, NV_TRUE,
        "%s in partition:%u ucode:%u [%c-mode] @ pc:0x%" NvU64_fmtx ", data:0x%" NvU64_fmtx,
        crashcatReportCauseTypeToString_GENERIC(causeType), partition, ucodeId,
        crashcatReportModeToChar_GENERIC(riscvMode), pReportV1->sourcePc, pReportV1->sourceData);
}

void crashcatReportLogReporter_V1_GENERIC(CrashCatReport *pReport)
{
    NvCrashCatReport_V1 *pReportV1 = &pReport->v1.report;
    NvCrashCatNvriscvPartition partition = crashcatReportV1ReporterPartition(pReportV1);
    NvCrashCatNvriscvUcodeId ucodeId = crashcatReportV1ReporterUcodeId(pReportV1);
    NV_CRASHCAT_RISCV_MODE riscvMode = crashcatReportV1ReporterMode(pReportV1);

    //
    // Though this is technically not a separate packet, we use the CRASHCAT_REPORT_LOG_PACKET_TYPE
    // macro to get the correct prefix/indentation for the reporter information.
    //
    CRASHCAT_REPORT_LOG_PACKET_TYPE(pReport,
        "Reported by partition:%u ucode:%u [%c-mode] version:%u @ %u",
        partition, ucodeId, crashcatReportModeToChar_GENERIC(riscvMode),
        crashcatReportV1ReporterVersion(pReportV1),
        crashcatReportV1ReporterTimestamp(pReportV1));
}
