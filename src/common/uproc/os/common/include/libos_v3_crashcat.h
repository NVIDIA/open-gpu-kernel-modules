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
#ifndef LIBOS_V3_CRASH_CAT_H
#define LIBOS_V3_CRASH_CAT_H

#include "nv-crashcat.h"
#include "nv-crashcat-decoder.h"

// libosv3 implements the CrashCat V1 protocol with the following implementation-defined bits

typedef enum
{
    LibosPanicReasonUnspecified           = 0x00,
    LibosPanicReasonUnhandledState        = 0x01,
    LibosPanicReasonInvalidConfiguration  = 0x02,
    LibosPanicReasonFatalHardwareError    = 0x03,
    LibosPanicReasonInsufficientResources = 0x04,
    LibosPanicReasonTimeout               = 0x05,
    LibosPanicReasonEnvCallFailed         = 0x06,
    LibosPanicReasonAsanMemoryError       = 0x08,
    LibosPanicReasonProgrammingError      = 0x0a,
    LibosPanicReasonAssertionFailed       = 0x0b,
    LibosPanicReasonTrapKernelPanic       = 0x0c,
    LibosPanicReasonTrapInstruction       = 0x0d,
    LibosPanicReasonCount
} LibosPanicReason;

typedef enum{
    LibosKernelModuleInit          = 0x00,
    LibosKernelModuleIpi           = 0x01,
    LibosKernelModuleLoader        = 0x02,
    LibosKernelModulePartition     = 0x03,
    LibosKernelModuleSbi           = 0x04,
    LibosKernelModulePagetable     = 0x05,
    LibosKernelModuleAddressSpace  = 0x06,
    LibosKernelModuleIdentity      = 0x07,
    LibosKernelModuleObjectPool    = 0x08,
    LibosKernelModulePageState     = 0x09,
    LibosKernelModuleMemoryPool    = 0x10,
    LibosKernelModuleBuddyState    = 0x11,
    LibosKernelModuleServer        = 0x12,
    LibosKernelModuleDmaDriver     = 0x13,
    LibosKernelModuleExtIntrDriver = 0x14,
    LibosKernelModuleSoftMmuCore   = 0x15,
    LibosKernelModulePagestream    = 0x16,
    LibosKernelModuleTask          = 0x17,
    LibosKernelModuleGdmaDriver    = 0x18,
    LibosKernelModuleCacheDriver   = 0x19,
    LibosKernelModuleMinTree       = 0x20,
    LibosKernelModuleList          = 0x21,
    LibosKernelModuleSpinlock      = 0x22,
    LibosKernelModuleScheduler     = 0x23,
    LibosKernelModulePort          = 0x24,
    LibosKernelModuleGlobalPort    = 0x25,
    LibosKernelModuleProxyPort     = 0x26,
    LibosKernelModuleTimer         = 0x27,
} LibosKernelModule;

// NV_CRASHCAT_REPORT_IMPLEMENTER_SIGNATURE (bits 63:0) - "LIBOS3.1"
#define NV_CRASHCAT_REPORT_IMPLEMENTER_SIGNATURE_LIBOS3 (0x4C49424F53332E31ull)

// NV_CRASHCAT_REPORT_V1_REPORTER_ID_IMPL_DEF (bits 63:18)
#define NV_CRASHCAT_REPORT_V1_REPORTER_ID_LIBOS3_TASK_ID              31:24
#define NV_CRASHCAT_REPORT_V1_REPORTER_ID_LIBOS3_RESERVED             63:32
#define NV_CRASHCAT_REPORT_V1_REPORTER_ID_LIBOS3_TASK_ID_UNSPECIFIED  (0xFF)

static NV_INLINE
void crashcatReportV1SetReporterLibos3TaskId(NvCrashCatReport_V1 *pReport, NvU8 task_id)
{
    pReport->reporterId = FLD_SET_DRF_NUM64(_CRASHCAT, _REPORT_V1_REPORTER_ID, _LIBOS3_TASK_ID,
                                            task_id, pReport->reporterId);
}

static NV_INLINE
NvU8 crashcatReportV1ReporterLibos3TaskId(NvCrashCatReport_V1 *pReport)
{
    return (NvU8)DRF_VAL64(_CRASHCAT, _REPORT_V1_REPORTER_ID, _LIBOS3_TASK_ID, pReport->reporterId);
}

// NV_CRASHCAT_REPORT_V1_REPORTER_DATA_VERSION (bits 31:0)
#define NV_CRASHCAT_REPORT_V1_REPORTER_DATA_VERSION_LIBOS3_CL         23:0
#define NV_CRASHCAT_REPORT_V1_REPORTER_DATA_VERSION_LIBOS3_MINOR      27:24
#define NV_CRASHCAT_REPORT_V1_REPORTER_DATA_VERSION_LIBOS3_MAJOR      31:28

static NV_INLINE
void crashcatReportV1SetReporterVersionLibos3(NvCrashCatReport_V1 *pReport, NvU32 cl)
{
    pReport->reporterData = FLD_SET_DRF_NUM64(_CRASHCAT, _REPORT_V1_REPORTER_DATA,
                                              _VERSION_LIBOS3_MAJOR, 3, pReport->reporterData);
    pReport->reporterData = FLD_SET_DRF_NUM64(_CRASHCAT, _REPORT_V1_REPORTER_DATA,
                                              _VERSION_LIBOS3_MINOR, 1, pReport->reporterData);
    pReport->reporterData = FLD_SET_DRF_NUM64(_CRASHCAT, _REPORT_V1_REPORTER_DATA,
                                              _VERSION_LIBOS3_CL, cl, pReport->reporterData);
}

static NV_INLINE
NvU32 crashcatReportV1ReporterVersionLibos3Cl(NvCrashCatReport_V1 *pReport)
{
    return DRF_VAL(_CRASHCAT, _REPORT_V1_REPORTER_DATA_VERSION, _LIBOS3_CL,
                   crashcatReportV1ReporterVersion(pReport));
}

static NV_INLINE
NvU8 crashcatReportV1ReporterVersionLibos3Minor(NvCrashCatReport_V1 *pReport)
{
    return (NvU8)DRF_VAL(_CRASHCAT, _REPORT_V1_REPORTER_DATA_VERSION, _LIBOS3_MINOR,
                         crashcatReportV1ReporterVersion(pReport));
}

static NV_INLINE
NvU8 crashcatReportV1ReporterVersionLibos3Major(NvCrashCatReport_V1 *pReport)
{
    return (NvU8)DRF_VAL(_CRASHCAT, _REPORT_V1_REPORTER_DATA_VERSION, _LIBOS3_MAJOR,
                         crashcatReportV1ReporterVersion(pReport));
}

// NV_CRASHCAT_REPORT_V1_SOURCE_ID_IMPL_DEF (63:18)
#define NV_CRASHCAT_REPORT_V1_SOURCE_ID_LIBOS3_TASK_ID                31:24
#define NV_CRASHCAT_REPORT_V1_SOURCE_ID_LIBOS3_RESERVED               63:32
#define NV_CRASHCAT_REPORT_V1_SOURCE_ID_LIBOS3_TASK_ID_UNSPECIFIED    (0xFF)

static NV_INLINE
void crashcatReportV1SetSourceLibos3TaskId(NvCrashCatReport_V1 *pReport, NvU8 task_id)
{
    pReport->sourceId = FLD_SET_DRF_NUM64(_CRASHCAT, _REPORT_V1_SOURCE_ID, _LIBOS3_TASK_ID, task_id,
                                          pReport->sourceId);
}

static NV_INLINE
NvU8 crashcatReportV1SourceLibos3TaskId(NvCrashCatReport_V1 *pReport)
{
    return (NvU8)DRF_VAL64(_CRASHCAT, _REPORT_V1_SOURCE_ID, _LIBOS3_TASK_ID, pReport->sourceId);
}

// NV_CRASHCAT_REPORT_V1_SOURCE_CAUSE_IMPL_DEF (63:32)
#define NV_CRASHCAT_REPORT_V1_SOURCE_CAUSE_LIBOS3_REASON              39:32
#define NV_CRASHCAT_REPORT_V1_SOURCE_CAUSE_LIBOS3_RESERVED            63:40

static NV_INLINE
void crashcatReportV1SetSourceCauseLibos3Reason(NvCrashCatReport_V1 *pReport,
                                                LibosPanicReason reason)
{
    pReport->sourceCause = FLD_SET_DRF_NUM64(_CRASHCAT, _REPORT_V1_SOURCE_CAUSE, _LIBOS3_REASON,
                                             reason, pReport->sourceCause);
}

static NV_INLINE
LibosPanicReason crashcatReportV1SourceCauseLibos3Reason(NvCrashCatReport_V1 *pReport)
{
    return (LibosPanicReason)DRF_VAL64(_CRASHCAT, _REPORT_V1_SOURCE_CAUSE, _LIBOS3_REASON,
                                       pReport->sourceCause);
}

#endif // LIBOS_V3_CRASH_CAT_H
