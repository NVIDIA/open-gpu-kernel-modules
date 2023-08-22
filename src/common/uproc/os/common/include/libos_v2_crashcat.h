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
#ifndef LIBOS_V2_CRASHCAT_H
#define LIBOS_V2_CRASHCAT_H

#include "nv-crashcat.h"
#include "nv-crashcat-decoder.h"

// libosv2 implements the CrashCat V1 protocol with the following implementation-defined bits

typedef enum
{
    LibosPanicReasonUnspecified            = 0x00,
    LibosPanicReasonUnrecoverableTaskCrash = 0x01,
    LibosPanicReasonUnhandledState         = 0x02,
    LibosPanicReasonInvalidConfiguration   = 0x03,
    LibosPanicReasonFatalHardwareError     = 0x04,
    LibosPanicReasonInsufficientResources  = 0x05,
    LibosPanicReasonTimeout                = 0x06,
    LibosPanicReasonEnvCallFailed          = 0x07,
    LibosPanicReasonSspStackCheckFailed    = 0x08,
    LibosPanicReasonAsanMemoryError        = 0x09,
    LibosPanicReasonTest                   = 0x0a,
    LibosPanicReasonProgrammingError       = 0x0b,
    LibosPanicReasonDebugAssertionFailed   = 0x0c,
    LibosPanicReasonCount
} LibosPanicReason;

// NV_CRASHCAT_REPORT_IMPLEMENTER_SIGNATURE (bits 63:0) - "LIBOS2.0"
#define NV_CRASHCAT_REPORT_IMPLEMENTER_SIGNATURE_LIBOS2 (0x4C49424F53322E30ull)

// NV_CRASHCAT_REPORT_V1_REPORTER_ID_IMPL_DEF (bits 63:24)
#define NV_CRASHCAT_REPORT_V1_REPORTER_ID_LIBOS2_TASK_ID                31:24
#define NV_CRASHCAT_REPORT_V1_REPORTER_ID_LIBOS2_TASK_ID_UNSPECIFIED    0xFF
#define NV_CRASHCAT_REPORT_V1_REPORTER_ID_LIBOS2_RESERVED               63:32

static NV_INLINE
void crashcatReportV1SetReporterLibos2TaskId(NvCrashCatReport_V1 *pReport, NvU8 task_id)
{
    pReport->reporterId = FLD_SET_DRF_NUM64(_CRASHCAT, _REPORT_V1_REPORTER_ID, _LIBOS2_TASK_ID,
                                            task_id, pReport->reporterId);
}

static NV_INLINE
NvU8 crashcatReportV1ReporterLibos2TaskId(NvCrashCatReport_V1 *pReport)
{
    return (NvU8)DRF_VAL64(_CRASHCAT, _REPORT_V1_REPORTER_ID, _LIBOS2_TASK_ID, pReport->reporterId);
}

// NV_CRASHCAT_REPORT_V1_REPORTER_DATA_VERSION (bits 31:0)
#define NV_CRASHCAT_REPORT_V1_REPORTER_DATA_VERSION_LIBOS2_CL           23:0
#define NV_CRASHCAT_REPORT_V1_REPORTER_DATA_VERSION_LIBOS2_MINOR        27:24
#define NV_CRASHCAT_REPORT_V1_REPORTER_DATA_VERSION_LIBOS2_MAJOR        31:28

static NV_INLINE
void crashcatReportV1SetReporterVersionLibos2(NvCrashCatReport_V1 *pReport, NvU32 cl)
{
    pReport->reporterData = FLD_SET_DRF_NUM64(_CRASHCAT, _REPORT_V1_REPORTER_DATA,
                                              _VERSION_LIBOS2_MAJOR, 2, pReport->reporterData);
    pReport->reporterData = FLD_SET_DRF_NUM64(_CRASHCAT, _REPORT_V1_REPORTER_DATA,
                                              _VERSION_LIBOS2_MINOR, 0, pReport->reporterData);
    pReport->reporterData = FLD_SET_DRF_NUM64(_CRASHCAT, _REPORT_V1_REPORTER_DATA,
                                              _VERSION_LIBOS2_CL, cl, pReport->reporterData);
}

static NV_INLINE
NvU32 crashcatReportV1ReporterVersionLibos2Cl(NvCrashCatReport_V1 *pReport)
{
    return DRF_VAL(_CRASHCAT, _REPORT_V1_REPORTER_DATA_VERSION, _LIBOS2_CL,
                   crashcatReportV1ReporterVersion(pReport));
}

static NV_INLINE
NvU8 crashcatReportV1ReporterVersionLibos2Minor(NvCrashCatReport_V1 *pReport)
{
    return (NvU8)DRF_VAL(_CRASHCAT, _REPORT_V1_REPORTER_DATA_VERSION, _LIBOS2_MINOR,
                         crashcatReportV1ReporterVersion(pReport));
}

static NV_INLINE
NvU8 crashcatReportV1ReporterVersionLibos2Major(NvCrashCatReport_V1 *pReport)
{
    return (NvU8)DRF_VAL(_CRASHCAT, _REPORT_V1_REPORTER_DATA_VERSION, _LIBOS2_MAJOR,
                         crashcatReportV1ReporterVersion(pReport));
}

// NV_CRASHCAT_REPORT_V1_SOURCE_ID_IMPL_DEF (63:24)
#define NV_CRASHCAT_REPORT_V1_SOURCE_ID_LIBOS2_TASK_ID                          31:24
#define NV_CRASHCAT_REPORT_V1_SOURCE_ID_LIBOS2_TASK_ID_UNSPECIFIED              0xFF
#define NV_CRASHCAT_REPORT_V1_SOURCE_ID_LIBOS2_RESERVED                         63:32

static NV_INLINE
void crashcatReportV1SetSourceLibos2TaskId(NvCrashCatReport_V1 *pReport, NvU8 task_id)
{
    pReport->sourceId = FLD_SET_DRF_NUM64(_CRASHCAT, _REPORT_V1_SOURCE_ID, _LIBOS2_TASK_ID, task_id,
                                          pReport->sourceId);
}

static NV_INLINE
NvU8 crashcatReportV1SourceLibos2TaskId(NvCrashCatReport_V1 *pReport)
{
    return (NvU8)DRF_VAL64(_CRASHCAT, _REPORT_V1_SOURCE_ID, _LIBOS2_TASK_ID, pReport->sourceId);
}

// NV_CRASHCAT_REPORT_V1_SOURCE_CAUSE_IMPL_DEF (63:32)
#define NV_CRASHCAT_REPORT_V1_SOURCE_CAUSE_LIBOS2_REASON                        39:32
#define NV_CRASHCAT_REPORT_V1_SOURCE_CAUSE_LIBOS2_RESERVED                      63:40

ct_assert(LibosPanicReasonCount <=
            NVBIT(DRF_SIZE(NV_CRASHCAT_REPORT_V1_SOURCE_CAUSE_LIBOS2_REASON)));

static NV_INLINE
void crashcatReportV1SetSourceCauseLibos2Reason(NvCrashCatReport_V1 *pReport,
                                                LibosPanicReason reason)
{
    pReport->sourceCause = FLD_SET_DRF_NUM64(_CRASHCAT, _REPORT_V1_SOURCE_CAUSE, _LIBOS2_REASON,
                                             reason, pReport->sourceCause);
}

static NV_INLINE
LibosPanicReason crashcatReportV1SourceCauseLibos2Reason(NvCrashCatReport_V1 *pReport)
{
    return (LibosPanicReason)DRF_VAL64(_CRASHCAT, _REPORT_V1_SOURCE_CAUSE, _LIBOS2_REASON,
                                       pReport->sourceCause);
}

#endif // LIBOS_V2_CRASHCAT_H
