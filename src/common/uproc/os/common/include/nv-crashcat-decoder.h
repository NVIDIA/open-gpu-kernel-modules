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

#ifndef NV_CRASHCAT_DECODER_H
#define NV_CRASHCAT_DECODER_H

#include "nv-crashcat.h"

// This header defines decoder/consumer-side implementation helpers for the CrashCat protocol

//
// Wayfinder L0 Bitfield Accessors
//
static NV_INLINE
NvBool crashcatWayfinderL0Valid(NvCrashCatWayfinderL0_V1 wfl0)
{
    return FLD_TEST_DRF(_CRASHCAT, _WAYFINDER_L0, _SIGNATURE, _VALID, wfl0);
}

static NV_INLINE
NV_CRASHCAT_WAYFINDER_VERSION crashcatWayfinderL0Version(NvCrashCatWayfinderL0_V1 wfl0)
{
    return (NV_CRASHCAT_WAYFINDER_VERSION)DRF_VAL(_CRASHCAT, _WAYFINDER_L0, _VERSION, wfl0);
}

static NV_INLINE
NV_CRASHCAT_SCRATCH_GROUP_ID crashcatWayfinderL0V1Wfl1Location(NvCrashCatWayfinderL0_V1 wfl0)
{
    return (NV_CRASHCAT_SCRATCH_GROUP_ID)DRF_VAL(_CRASHCAT, _WAYFINDER_L0_V1, _WFL1_LOCATION, wfl0);
}

//
// Wayfinder L1 Bitfield Accessors
//
static NV_INLINE
NV_CRASHCAT_MEM_APERTURE crashcatWayfinderL1V1QueueAperture(NvCrashCatWayfinderL1_V1 wfl1)
{
    return (NV_CRASHCAT_MEM_APERTURE)DRF_VAL64(_CRASHCAT, _WAYFINDER_L1_V1, _QUEUE_APERTURE, wfl1);
}

static NV_INLINE
NvLength crashcatWayfinderL1V1QueueSize(NvCrashCatWayfinderL1_V1 wfl1)
{
    NvU8 unitShift;
    NV_CRASHCAT_MEM_UNIT_SIZE unitSize =
        (NV_CRASHCAT_MEM_UNIT_SIZE)DRF_VAL64(_CRASHCAT, _WAYFINDER_L1_V1, _QUEUE_UNIT_SIZE, wfl1);
    switch (unitSize)
    {
        case NV_CRASHCAT_MEM_UNIT_SIZE_1KB:  unitShift = 10; break;
        case NV_CRASHCAT_MEM_UNIT_SIZE_4KB:  unitShift = 12; break;
        case NV_CRASHCAT_MEM_UNIT_SIZE_64KB: unitShift = 16; break;
        default: return 0;
    }

    // Increment size, since the size in the header is size - 1 (queue of 0 size is not encodable)
    return (NvLength)((DRF_VAL64(_CRASHCAT, _WAYFINDER_L1_V1, _QUEUE_SIZE, wfl1) + 1) << unitShift);
}

static NV_INLINE
NvU64 crashcatWayfinderL1V1QueueOffset(NvCrashCatWayfinderL1_V1 wfl1)
{
    return DRF_VAL64(_CRASHCAT, _WAYFINDER_L1_V1, _QUEUE_OFFSET_1KB, wfl1) << 10;
}

//
// CrashCat Packet Header (Unversioned) Bitfield Accessors
//
static NV_INLINE
NV_CRASHCAT_PACKET_FORMAT_VERSION crashcatPacketHeaderFormatVersion(NvCrashCatPacketHeader hdr)
{
    return (NV_CRASHCAT_PACKET_FORMAT_VERSION)DRF_VAL64(_CRASHCAT, _PACKET_HEADER, _FORMAT_VERSION,
                                                        hdr);
}

static NV_INLINE
NvLength crashcatPacketHeaderPayloadSize(NvCrashCatPacketHeader hdr)
{
    NvU8 unitShift = 0;
    NV_CRASHCAT_MEM_UNIT_SIZE unitSize =
        (NV_CRASHCAT_MEM_UNIT_SIZE)DRF_VAL64(_CRASHCAT, _PACKET_HEADER, _PAYLOAD_UNIT_SIZE, hdr);
    switch (unitSize)
    {
        case NV_CRASHCAT_MEM_UNIT_SIZE_8B:   unitShift = 3; break;
        case NV_CRASHCAT_MEM_UNIT_SIZE_1KB:  unitShift = 10; break;
        case NV_CRASHCAT_MEM_UNIT_SIZE_4KB:  unitShift = 12; break;
        case NV_CRASHCAT_MEM_UNIT_SIZE_64KB: unitShift = 16; break;
    }

    // Increment size, since the size in the header is size - 1 (payload of 0 size is not encodable)
    return (NvLength)((DRF_VAL64(_CRASHCAT, _PACKET_HEADER, _PAYLOAD_SIZE, hdr) + 1) << unitShift);
}

static NV_INLINE
NvBool crashcatPacketHeaderValid(NvCrashCatPacketHeader hdr)
{
    return (FLD_TEST_DRF64(_CRASHCAT, _PACKET_HEADER, _SIGNATURE, _VALID, hdr) &&
            (crashcatPacketHeaderFormatVersion(hdr) > 0) &&
            (crashcatPacketHeaderFormatVersion(hdr) <= NV_CRASHCAT_PACKET_FORMAT_VERSION_LAST) &&
            (crashcatPacketHeaderPayloadSize(hdr) > 0));
}

//
// CrashCat Packet Header (V1) Bitfield Accessors
//
static NV_INLINE
NV_CRASHCAT_PACKET_TYPE crashcatPacketHeaderV1Type(NvCrashCatPacketHeader_V1 hdr)
{
    return (NV_CRASHCAT_PACKET_TYPE)DRF_VAL64(_CRASHCAT, _PACKET_HEADER, _V1_TYPE, hdr);
}

//
// CrashCat Report V1 Bitfield Accessors
//
static NV_INLINE
NvCrashCatNvriscvPartition crashcatReportV1ReporterPartition(NvCrashCatReport_V1 *pReport)
{
    return (NvCrashCatNvriscvPartition)DRF_VAL64(_CRASHCAT, _REPORT_V1_REPORTER_ID,
                                                 _NVRISCV_PARTITION, pReport->reporterId);
}

static NV_INLINE
NvCrashCatNvriscvUcodeId crashcatReportV1ReporterUcodeId(NvCrashCatReport_V1 *pReport)
{
    return (NvCrashCatNvriscvUcodeId)DRF_VAL64(_CRASHCAT, _REPORT_V1_REPORTER_ID, _NVRISCV_UCODE_ID,
                                               pReport->reporterId);
}

static NV_INLINE
NV_CRASHCAT_RISCV_MODE crashcatReportV1ReporterMode(NvCrashCatReport_V1 *pReport)
{
    return (NV_CRASHCAT_RISCV_MODE)DRF_VAL64(_CRASHCAT, _REPORT_V1_REPORTER_ID, _RISCV_MODE,
                                             pReport->reporterId);
}

static NV_INLINE
NvU32 crashcatReportV1ReporterVersion(NvCrashCatReport_V1 *pReport)
{
    return (NvU32)DRF_VAL64(_CRASHCAT, _REPORT_V1_REPORTER_DATA, _VERSION, pReport->reporterData);
}

static NV_INLINE
NvU32 crashcatReportV1ReporterTimestamp(NvCrashCatReport_V1 *pReport)
{
    return (NvU32)DRF_VAL64(_CRASHCAT, _REPORT_V1_REPORTER_DATA, _TIMESTAMP, pReport->reporterData);
}


static NV_INLINE
NvCrashCatNvriscvPartition crashcatReportV1SourcePartition(NvCrashCatReport_V1 *pReport)
{
    return (NvCrashCatNvriscvPartition)DRF_VAL64(_CRASHCAT, _REPORT_V1_SOURCE_ID,
                                                 _NVRISCV_PARTITION, pReport->sourceId);
}

static NV_INLINE
NvCrashCatNvriscvUcodeId crashcatReportV1SourceUcodeId(NvCrashCatReport_V1 *pReport)
{
    return (NvCrashCatNvriscvUcodeId)DRF_VAL64(_CRASHCAT, _REPORT_V1_SOURCE_ID, _NVRISCV_UCODE_ID,
                                               pReport->sourceId);
}

static NV_INLINE
NV_CRASHCAT_RISCV_MODE crashcatReportV1SourceMode(NvCrashCatReport_V1 *pReport)
{
    return (NV_CRASHCAT_RISCV_MODE)DRF_VAL64(_CRASHCAT, _REPORT_V1_SOURCE_ID, _RISCV_MODE,
                                             pReport->sourceId);
}

static NV_INLINE
NV_CRASHCAT_CAUSE_TYPE crashcatReportV1SourceCauseType(NvCrashCatReport_V1 *pReport)
{
    return (NV_CRASHCAT_CAUSE_TYPE)DRF_VAL64(_CRASHCAT, _REPORT_V1_SOURCE_CAUSE, _TYPE,
                                             pReport->sourceCause);
}

static NV_INLINE
NV_CRASHCAT_CONTAINMENT crashcatReportV1SourceCauseContainment(NvCrashCatReport_V1 *pReport)
{
    return (NV_CRASHCAT_CONTAINMENT)DRF_VAL64(_CRASHCAT, _REPORT_V1_SOURCE_CAUSE, _CONTAINMENT,
                                              pReport->sourceCause);
}

//
// CrashCat RISC-V 64-bit CSR State V1 Bitfield Accessors
//
static NV_INLINE
NV_CRASHCAT_RISCV_MODE crashcatRiscv64CsrStateV1Mode(NvCrashCatRiscv64CsrState_V1 *pRiscv64CsrState)
{
    return (NV_CRASHCAT_RISCV_MODE)DRF_VAL64(_CRASHCAT, _RISCV64_CSR_STATE_V1_HEADER, _RISCV_MODE,
                                             pRiscv64CsrState->header);
}

//
// CrashCat RISC-V 64-bit GPR State V1 Bitfield Accessors
//
static NV_INLINE
NV_CRASHCAT_RISCV_MODE crashcatRiscv64GprStateV1Mode(NvCrashCatRiscv64GprState_V1 *pRiscv64GprState)
{
    return (NV_CRASHCAT_RISCV_MODE)DRF_VAL64(_CRASHCAT, _RISCV64_GPR_STATE_V1_HEADER, _RISCV_MODE,
                                             pRiscv64GprState->header);
}

//
// CrashCat RISC-V 64-bit Trace V1 Bitfield Accessors
//
static NV_INLINE
NV_CRASHCAT_RISCV_MODE crashcatRiscv64TraceV1Mode(NvCrashCatRiscv64Trace_V1 *pRiscv64Trace)
{
    return (NV_CRASHCAT_RISCV_MODE)DRF_VAL64(_CRASHCAT, _RISCV64_TRACE_V1_HEADER, _RISCV_MODE,
                                             pRiscv64Trace->header);
}

static NV_INLINE
NV_CRASHCAT_TRACE_TYPE crashcatRiscv64TraceV1Type(NvCrashCatRiscv64Trace_V1 *pRiscv64Trace)
{
    return (NV_CRASHCAT_TRACE_TYPE)DRF_VAL64(_CRASHCAT, _RISCV64_TRACE_V1_HEADER, _TRACE_TYPE,
                                             pRiscv64Trace->header);
}

//
// CrashCat 32-bit I/O State V1 Bitfield Accessors
//
static NV_INLINE
NV_CRASHCAT_IO_APERTURE crashcatIo32StateV1Aperture(NvCrashCatIo32State_V1 *pIo32State)
{
    return (NV_CRASHCAT_IO_APERTURE)DRF_VAL64(_CRASHCAT, _IO32_STATE_V1_HEADER, _APERTURE,
                                              pIo32State->header);
}

#endif  // NV_CRASHCAT_DECODER_H
