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
#include "utils/nvassert.h"
#include "nv-crashcat-decoder.h"
#include "utils/nvprintf.h"

#include "g_all_dcl_pb.h"


static NV_INLINE NvUPtr ptrDiff(void *pStart, void *pEnd)
{
    return (NvUPtr)pEnd - (NvUPtr)pStart;
}

static NV_INLINE void *advancePtr(void *pStart, void *pEnd, NvUPtr *pBytesRemaining)
{
    *pBytesRemaining -= ptrDiff(pStart, pEnd);
    return pEnd;
}

static NV_INLINE NvCrashCatPacketHeader_V1 crashcatReadPacketHeader(void *pHdr)
{
    return *(NvCrashCatPacketHeader_V1 *)pHdr;
}

void crashcatReportDestruct_V1(CrashCatReport *pReport)
{
    portMemFree(pReport->v1.pRiscv64StackTrace);
    portMemFree(pReport->v1.pRiscv64PcTrace);
    portMemFree(pReport->v1.pIo32State);
}

void *crashcatReportExtract_V1
(
    CrashCatReport *pReport,
    void *pReportBytes,
    NvLength bytesRemaining
)
{
    // Check for the base report first
    void *pStart = pReportBytes;
    void *pEnd = crashcatReportExtractReport_HAL(pReport, pStart, bytesRemaining);

    // If nothing was extracted, there's nothing more to do
    if (pEnd == pStart)
        return pStart;

    NV_ASSERT_OR_RETURN(pStart < pEnd, pStart);

    //
    // Pull out additional report data until we've exhausted the buffer or the start of the next
    // report is found.
    //
    for (pStart = advancePtr(pStart, pEnd, &bytesRemaining);
         bytesRemaining > 0;
         pStart = advancePtr(pStart, pEnd, &bytesRemaining))
    {
        NvCrashCatPacketHeader_V1 hdr = crashcatReadPacketHeader(pStart);

        // Not a valid header, nothing else to do here
        if (!crashcatPacketHeaderValid(hdr))
        {
            NV_PRINTF(LEVEL_WARNING,
                "Invalid packet header (0x%" NvU64_fmtx ") found, skipping %" NvUPtr_fmtu
                " bytes remaining\n", hdr, bytesRemaining);
            pEnd = (void *)((NvUPtr)pStart + bytesRemaining);
            continue;
        }

        NV_CRASHCAT_PACKET_TYPE packetType = crashcatPacketHeaderV1Type(hdr);

        // This is the next report (which may have a different format version)
        if (packetType == NV_CRASHCAT_PACKET_TYPE_REPORT)
            break;

        switch (packetType)
        {
            case NV_CRASHCAT_PACKET_TYPE_RISCV64_CSR_STATE:
                pEnd = crashcatReportExtractRiscv64CsrState_HAL(pReport, pStart, bytesRemaining);
                break;
            case NV_CRASHCAT_PACKET_TYPE_RISCV64_GPR_STATE:
                pEnd = crashcatReportExtractRiscv64GprState_HAL(pReport, pStart, bytesRemaining);
                break;
            case NV_CRASHCAT_PACKET_TYPE_RISCV64_TRACE:
                pEnd = crashcatReportExtractRiscv64Trace_HAL(pReport, pStart, bytesRemaining);
                break;
            case NV_CRASHCAT_PACKET_TYPE_IO32_STATE:
                pEnd = crashcatReportExtractIo32State_HAL(pReport, pStart, bytesRemaining);
                break;
            default:
                NV_PRINTF(LEVEL_WARNING, "Unsupported V1 packet type 0x%02x\n", packetType);
                break;
        }

        // Did packet extraction fail?
        if (pEnd == pStart)
        {
            NvLength dataSize = crashcatPacketHeaderPayloadSize(hdr) +
                                    sizeof(NvCrashCatPacketHeader_V1);
            NV_PRINTF(LEVEL_WARNING,
                "Unable to extract packet type 0x%02x (%" NvUPtr_fmtu " bytes), skipping\n",
                packetType, dataSize);
            pEnd = (void *)((NvUPtr)pStart + dataSize);
            continue;
        }
    }

    return pStart;
}

void *crashcatReportExtractReport_V1
(
    CrashCatReport *pReport,
    void *pReportBytes,
    NvLength bytesRemaining
)
{
    if (bytesRemaining < sizeof(NvCrashCatReport_V1))
    {
        NV_PRINTF(LEVEL_WARNING,
            "Not enough data (%" NvUPtr_fmtu " bytes) to decode NvCrashCatReport_V1\n",
            bytesRemaining);
        return pReportBytes;
    }

    NvCrashCatPacketHeader_V1 hdr = crashcatReadPacketHeader(pReportBytes);
    const NvLength expPayloadSize = sizeof(pReport->v1.report)
                                  - sizeof(pReport->v1.report.header);
    if (!crashcatPacketHeaderValid(hdr) ||
        (crashcatPacketHeaderFormatVersion(hdr) != NV_CRASHCAT_PACKET_FORMAT_VERSION_1) ||
        (crashcatPacketHeaderPayloadSize(hdr) != expPayloadSize))
    {
        NV_PRINTF(LEVEL_WARNING, "Invalid V1 report header 0x%" NvU64_fmtx "\n", hdr);
        return pReportBytes;
    }

    pReport->v1.report = *(NvCrashCatReport_V1 *)pReportBytes;
    pReport->validTags |= NVBIT(NV_CRASHCAT_PACKET_TYPE_REPORT);

    return (void *)((NvUPtr)pReportBytes + sizeof(NvCrashCatReport_V1));
}

void *crashcatReportExtractRiscv64CsrState_V1
(
    CrashCatReport *pReport,
    void *pReportBytes,
    NvLength bytesRemaining
)
{
    if (bytesRemaining < sizeof(NvCrashCatRiscv64CsrState_V1))
    {
        NV_PRINTF(LEVEL_WARNING,
            "Not enough data (%" NvUPtr_fmtu " bytes) to decode NvCrashCatRiscv64CsrState_V1\n",
            bytesRemaining);
        return pReportBytes;
    }

    NvCrashCatPacketHeader_V1 hdr = crashcatReadPacketHeader(pReportBytes);
    const NvLength expPayloadSize = sizeof(pReport->v1.riscv64CsrState)
                                  - sizeof(pReport->v1.riscv64CsrState.header);
    if (!crashcatPacketHeaderValid(hdr) ||
        (crashcatPacketHeaderFormatVersion(hdr) != NV_CRASHCAT_PACKET_FORMAT_VERSION_1) ||
        (crashcatPacketHeaderPayloadSize(hdr) != expPayloadSize))
    {
        NV_PRINTF(LEVEL_WARNING, "Invalid V1 RISCV CSR state header 0x%" NvU64_fmtx "\n", hdr);
        return pReportBytes;
    }

    pReport->v1.riscv64CsrState = *(NvCrashCatRiscv64CsrState_V1 *)pReportBytes;
    pReport->validTags |= NVBIT(NV_CRASHCAT_PACKET_TYPE_RISCV64_CSR_STATE);

    return (void *)((NvUPtr)pReportBytes + sizeof(NvCrashCatRiscv64CsrState_V1));
}

void *crashcatReportExtractRiscv64GprState_V1
(
    CrashCatReport *pReport,
    void *pReportBytes,
    NvLength bytesRemaining
)
{
    if (bytesRemaining < sizeof(NvCrashCatRiscv64GprState_V1))
    {
        NV_PRINTF(LEVEL_WARNING,
            "Not enough data (%" NvUPtr_fmtu " bytes) to decode NvCrashCatRiscv64GprState_V1\n",
            bytesRemaining);
        return pReportBytes;
    }

    NvCrashCatPacketHeader_V1 hdr = crashcatReadPacketHeader(pReportBytes);
    const NvLength expPayloadSize = sizeof(pReport->v1.riscv64GprState)
                                  - sizeof(pReport->v1.riscv64GprState.header);
    if (!crashcatPacketHeaderValid(hdr) ||
        (crashcatPacketHeaderFormatVersion(hdr) != NV_CRASHCAT_PACKET_FORMAT_VERSION_1) ||
        (crashcatPacketHeaderPayloadSize(hdr) != expPayloadSize))
    {
        NV_PRINTF(LEVEL_WARNING, "Invalid V1 RISCV GPR state header 0x%" NvU64_fmtx "\n", hdr);
        return pReportBytes;
    }

    pReport->v1.riscv64GprState = *(NvCrashCatRiscv64GprState_V1 *)pReportBytes;
    pReport->validTags |= NVBIT(NV_CRASHCAT_PACKET_TYPE_RISCV64_GPR_STATE);

    return (void *)((NvUPtr)pReportBytes + sizeof(NvCrashCatRiscv64GprState_V1));
}

void *crashcatReportExtractRiscv64Trace_V1
(
    CrashCatReport *pReport,
    void *pReportBytes,
    NvLength bytesRemaining
)
{
    if (bytesRemaining < sizeof(NvCrashCatPacketHeader_V1))
    {
        NV_PRINTF(LEVEL_WARNING,
            "Not enough data (%" NvUPtr_fmtu " bytes) to decode NvCrashCatRiscv64Trace_V1\n",
            bytesRemaining);
        return pReportBytes;
    }

    NvCrashCatPacketHeader_V1 hdr = crashcatReadPacketHeader(pReportBytes);
    if (!crashcatPacketHeaderValid(hdr) ||
        (crashcatPacketHeaderFormatVersion(hdr) != NV_CRASHCAT_PACKET_FORMAT_VERSION_1))
    {
        NV_PRINTF(LEVEL_WARNING, "Invalid V1 trace packet header 0x%" NvU64_fmtx "\n", hdr);
        return pReportBytes;
    }

    // Allocate the buffer for the trace
    const NvU32 payloadSize = crashcatPacketHeaderPayloadSize(hdr);
    const NvLength sizeBytes = sizeof(NvCrashCatRiscv64Trace_V1) + payloadSize;
    NV_CHECK_OR_RETURN(LEVEL_ERROR, payloadSize > 0, pReportBytes);

    NvCrashCatRiscv64Trace_V1 *pRiscv64Trace = portMemAllocNonPaged(sizeBytes);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pRiscv64Trace != NULL, pReportBytes);

    portMemCopy(pRiscv64Trace, sizeBytes, pReportBytes, sizeBytes);

    //
    // We support both stack and PC traces in one report. They share a tag bit (will be set if
    // either are present).
    //
    switch (crashcatRiscv64TraceV1Type(pRiscv64Trace))
    {
        case NV_CRASHCAT_TRACE_TYPE_STACK:
            if (pReport->v1.pRiscv64StackTrace != NULL)
            {
                NV_PRINTF(LEVEL_WARNING,
                    "Only one stack trace packet is supported, discarding the old one\n");
                portMemFree(pReport->v1.pRiscv64StackTrace);
            }

            pReport->v1.pRiscv64StackTrace = pRiscv64Trace;
            pReport->validTags |= NVBIT(NV_CRASHCAT_PACKET_TYPE_RISCV64_TRACE);
            break;
        case NV_CRASHCAT_TRACE_TYPE_NVRVTB:
            if (pReport->v1.pRiscv64PcTrace != NULL)
            {
                NV_PRINTF(LEVEL_WARNING,
                    "Only one PC trace packet is supported, discarding the old one\n");
                portMemFree(pReport->v1.pRiscv64PcTrace);
            }

            pReport->v1.pRiscv64PcTrace = pRiscv64Trace;
            pReport->validTags |= NVBIT(NV_CRASHCAT_PACKET_TYPE_RISCV64_TRACE);
            break;
        default:
            NV_PRINTF(LEVEL_WARNING, "Unknown CrashCat trace type (0x%x), discarding\n",
                      crashcatRiscv64TraceV1Type(pRiscv64Trace));
            portMemFree(pRiscv64Trace);
            break;
    }

    return (void *)((NvUPtr)pReportBytes + sizeBytes);
}

void *crashcatReportExtractIo32State_V1
(
    CrashCatReport *pReport,
    void *pReportBytes,
    NvLength bytesRemaining
)
{
    if (bytesRemaining < sizeof(NvCrashCatPacketHeader_V1))
    {
        NV_PRINTF(LEVEL_WARNING,
            "Not enough data (%" NvUPtr_fmtu " bytes) to decode NvCrashCatIo32State_V1\n",
            bytesRemaining);
        return pReportBytes;
    }

    //
    // TODO: support multiple IO32_STATE packets. This implementation assumes there will be only
    //       one, and always keeps the first one.
    //
    if ((pReport->validTags & NVBIT(NV_CRASHCAT_PACKET_TYPE_IO32_STATE)) != 0)
    {
        NV_PRINTF(LEVEL_WARNING, "Report already contains valid reg32 data, skipping\n");
        return pReportBytes;
    }

    NvCrashCatPacketHeader_V1 hdr = crashcatReadPacketHeader(pReportBytes);
    if (!crashcatPacketHeaderValid(hdr) ||
        (crashcatPacketHeaderFormatVersion(hdr) != NV_CRASHCAT_PACKET_FORMAT_VERSION_1))
    {
        NV_PRINTF(LEVEL_WARNING, "Invalid V1 reg32 state header 0x%" NvU64_fmtx "\n", hdr);
        return pReportBytes;
    }

    // Allocate the buffer for the register state.
    const NvU32 payloadSize = crashcatPacketHeaderPayloadSize(hdr);
    const NvLength sizeBytes = sizeof(NvCrashCatIo32State_V1) + payloadSize;
    NV_CHECK_OR_RETURN(LEVEL_ERROR, payloadSize > 0, pReportBytes);

    pReport->v1.pIo32State = portMemAllocNonPaged(sizeBytes);
    NV_CHECK_OR_RETURN(LEVEL_ERROR, pReport->v1.pIo32State != NULL, pReportBytes);

    portMemCopy(pReport->v1.pIo32State, sizeBytes, pReportBytes, sizeBytes);
    pReport->validTags |= NVBIT(NV_CRASHCAT_PACKET_TYPE_IO32_STATE);

    return (void *)((NvUPtr)pReportBytes + sizeBytes);
}


void crashcatReportLogRiscv64CsrState_V1(CrashCatReport *pReport)
{
    NvCrashCatRiscv64CsrState_V1 *pCsrStateV1 = &pReport->v1.riscv64CsrState;
    NV_CRASHCAT_RISCV_MODE mode = crashcatRiscv64CsrStateV1Mode(pCsrStateV1);

    //
    // TODO: this equates M-mode with NVRISCV 1.x (e.g., mbadaddr vs stval). This happens to be
    //       correct as of this writing because there isn't an M-mode implementer on NVRISCV 2.x+,
    //       but we should have a way of distinguishing in the protocol.
    //
    CRASHCAT_REPORT_LOG_PACKET_TYPE(pReport, "RISC-V CSR State:\n");
    switch (mode)
    {
        case NV_CRASHCAT_RISCV_MODE_M:
            CRASHCAT_REPORT_LOG_DATA(pReport,
                "mstatus:0x%016" NvU64_fmtx "  mscratch:0x%016" NvU64_fmtx "     mie:0x%016" NvU64_fmtx "  mip:0x%016" NvU64_fmtx "\n",
                pCsrStateV1->xstatus, pCsrStateV1->xscratch, pCsrStateV1->xie, pCsrStateV1->xip);
            CRASHCAT_REPORT_LOG_DATA(pReport,
                "mepc:0x%016" NvU64_fmtx "  mbadaddr:0x%016" NvU64_fmtx "  mcause:0x%016" NvU64_fmtx "\n",
                pCsrStateV1->xepc, pCsrStateV1->xtval, pCsrStateV1->xcause);
            break;
        case NV_CRASHCAT_RISCV_MODE_S:
            CRASHCAT_REPORT_LOG_DATA(pReport,
                "sstatus:0x%016" NvU64_fmtx "  sscratch:0x%016" NvU64_fmtx "     sie:0x%016" NvU64_fmtx "  sip:0x%016" NvU64_fmtx "\n",
                pCsrStateV1->xstatus, pCsrStateV1->xscratch, pCsrStateV1->xie, pCsrStateV1->xip);
            CRASHCAT_REPORT_LOG_DATA(pReport,
                "sepc:0x%016" NvU64_fmtx "     stval:0x%016" NvU64_fmtx "  scause:0x%016" NvU64_fmtx "\n",
                pCsrStateV1->xepc, pCsrStateV1->xtval, pCsrStateV1->xcause);
            break;
        default:
            // We don't expect CSR dumps from other modes
            NV_PRINTF(LEVEL_WARNING, "Unexpected RISC-V mode (%u) CSR dump\n", mode);
            CRASHCAT_REPORT_LOG_DATA(pReport,
                "xstatus:0x%016" NvU64_fmtx "  xscratch:0x%016" NvU64_fmtx "     xie:0x%016" NvU64_fmtx "  xip:0x%016" NvU64_fmtx "\n",
                pCsrStateV1->xstatus, pCsrStateV1->xscratch, pCsrStateV1->xie, pCsrStateV1->xip);
            CRASHCAT_REPORT_LOG_DATA(pReport,
                "xepc:0x%016" NvU64_fmtx "     xtval:0x%016" NvU64_fmtx "  xcause:0x%016" NvU64_fmtx "\n",
                pCsrStateV1->xepc, pCsrStateV1->xtval, pCsrStateV1->xcause);
            break;
    }
}

void crashcatReportLogRiscv64GprState_V1(CrashCatReport *pReport)
{
    NvCrashCatRiscv64GprState_V1 *pGprStateV1 = &pReport->v1.riscv64GprState;

    // TODO: log the mode of the GPR state
    CRASHCAT_REPORT_LOG_PACKET_TYPE(pReport, "RISC-V GPR State:\n");
    CRASHCAT_REPORT_LOG_DATA(pReport,
        "ra:0x%016" NvU64_fmtx "   sp:0x%016" NvU64_fmtx "   gp:0x%016" NvU64_fmtx "   tp:0x%016" NvU64_fmtx "\n",
        pGprStateV1->ra, pGprStateV1->sp, pGprStateV1->gp, pGprStateV1->tp);
    CRASHCAT_REPORT_LOG_DATA(pReport,
        "a0:0x%016" NvU64_fmtx "   a1:0x%016" NvU64_fmtx "   a2:0x%016" NvU64_fmtx "   a3:0x%016" NvU64_fmtx "\n",
        pGprStateV1->a0, pGprStateV1->a1, pGprStateV1->a2, pGprStateV1->a3);
    CRASHCAT_REPORT_LOG_DATA(pReport,
        "a4:0x%016" NvU64_fmtx "   a5:0x%016" NvU64_fmtx "   a6:0x%016" NvU64_fmtx "   a7:0x%016" NvU64_fmtx "\n",
        pGprStateV1->a4, pGprStateV1->a5, pGprStateV1->a6, pGprStateV1->a7);
    CRASHCAT_REPORT_LOG_DATA(pReport,
        "s0:0x%016" NvU64_fmtx "   s1:0x%016" NvU64_fmtx "   s2:0x%016" NvU64_fmtx "   s3:0x%016" NvU64_fmtx "\n",
        pGprStateV1->s0, pGprStateV1->s1, pGprStateV1->s2, pGprStateV1->s3);
    CRASHCAT_REPORT_LOG_DATA(pReport,
        "s4:0x%016" NvU64_fmtx "   s5:0x%016" NvU64_fmtx "   s6:0x%016" NvU64_fmtx "   s7:0x%016" NvU64_fmtx "\n",
        pGprStateV1->s4, pGprStateV1->s5, pGprStateV1->s6, pGprStateV1->s7);
    CRASHCAT_REPORT_LOG_DATA(pReport,
        "s8:0x%016" NvU64_fmtx "   s9:0x%016" NvU64_fmtx "  s10:0x%016" NvU64_fmtx "  s11:0x%016" NvU64_fmtx "\n",
        pGprStateV1->s8, pGprStateV1->s9, pGprStateV1->s10, pGprStateV1->s11);
    CRASHCAT_REPORT_LOG_DATA(pReport,
        "t0:0x%016" NvU64_fmtx "   t1:0x%016" NvU64_fmtx "   t2:0x%016" NvU64_fmtx "   t3:0x%016" NvU64_fmtx "\n",
        pGprStateV1->t0, pGprStateV1->t1, pGprStateV1->t2, pGprStateV1->t3);
    CRASHCAT_REPORT_LOG_DATA(pReport,
        "t4:0x%016" NvU64_fmtx "   t5:0x%016" NvU64_fmtx "   t6:0x%016" NvU64_fmtx "\n",
        pGprStateV1->t4, pGprStateV1->t5, pGprStateV1->t6);
}

static void crashcatReportLogRiscv64Trace_V1
(
    CrashCatReport *pReport,
    NvCrashCatRiscv64Trace_V1 *pTraceV1
)
{
    NvU16 entries = crashcatPacketHeaderPayloadSize(pTraceV1->header) >> 3;
    NV_CRASHCAT_TRACE_TYPE traceType = crashcatRiscv64TraceV1Type(pTraceV1);

    // TODO: log the mode of the trace using implementer terminology (e.g., kernel/task)
    switch (traceType)
    {
        case NV_CRASHCAT_TRACE_TYPE_STACK:
            CRASHCAT_REPORT_LOG_PACKET_TYPE(pReport, "Stack Trace:\n");
            break;
        case NV_CRASHCAT_TRACE_TYPE_NVRVTB:
            CRASHCAT_REPORT_LOG_PACKET_TYPE(pReport, "PC Trace:\n");
            break;
        default:
            CRASHCAT_REPORT_LOG_PACKET_TYPE(pReport, "Address Trace:\n");
    }

    if (traceType == NV_CRASHCAT_TRACE_TYPE_NVRVTB)
    {
        // PC traces are too long with each entry on its own line
        const NvU8 ENTRIES_PER_LINE = 5;
        NvU16 idx;
        for (idx = 0; idx < (entries / ENTRIES_PER_LINE) * ENTRIES_PER_LINE;
             idx += ENTRIES_PER_LINE)
        {
            CRASHCAT_REPORT_LOG_DATA(pReport,
                "0x%016" NvU64_fmtx "  0x%016" NvU64_fmtx "  0x%016" NvU64_fmtx "  "
                "0x%016" NvU64_fmtx "  0x%016" NvU64_fmtx "\n",
                pTraceV1->addr[idx + 0], pTraceV1->addr[idx + 1], pTraceV1->addr[idx + 2],
                pTraceV1->addr[idx + 3], pTraceV1->addr[idx + 4]);
        }

        switch (entries - idx)
        {
            case 4:
                CRASHCAT_REPORT_LOG_DATA(pReport,
                    "0x%016" NvU64_fmtx "  0x%016" NvU64_fmtx "  0x%016" NvU64_fmtx "  "
                    "0x%016" NvU64_fmtx "\n",
                    pTraceV1->addr[idx + 0], pTraceV1->addr[idx + 1], pTraceV1->addr[idx + 2],
                    pTraceV1->addr[idx + 3]);
                break;
            case 3:
                CRASHCAT_REPORT_LOG_DATA(pReport,
                    "0x%016" NvU64_fmtx "  0x%016" NvU64_fmtx "  0x%016" NvU64_fmtx "\n",
                    pTraceV1->addr[idx + 0], pTraceV1->addr[idx + 1], pTraceV1->addr[idx + 2]);
                break;
            case 2:
                CRASHCAT_REPORT_LOG_DATA(pReport, "0x%016" NvU64_fmtx "  0x%016" NvU64_fmtx "\n",
                    pTraceV1->addr[idx + 0], pTraceV1->addr[idx + 1]);
                break;
            case 1:
                CRASHCAT_REPORT_LOG_DATA(pReport, "0x%016" NvU64_fmtx "\n", pTraceV1->addr[idx + 0]);
                break;
            default:
                break;
        }
    }
    else
    {
        for (NvU16 idx = 0; idx < entries; idx++)
        {
            CRASHCAT_REPORT_LOG_DATA(pReport, "0x%016" NvU64_fmtx "\n", pTraceV1->addr[idx]);
        }
    }
}

void crashcatReportLogRiscv64Traces_V1(CrashCatReport *pReport)
{
    if (pReport->v1.pRiscv64StackTrace != NULL)
        crashcatReportLogRiscv64Trace_V1(pReport, pReport->v1.pRiscv64StackTrace);
    if (pReport->v1.pRiscv64PcTrace != NULL)
        crashcatReportLogRiscv64Trace_V1(pReport, pReport->v1.pRiscv64PcTrace);
}

void crashcatReportLogIo32State_V1(CrashCatReport *pReport)
{
    NvCrashCatIo32State_V1 *pIo32StateV1 = pReport->v1.pIo32State;
    NvU16 entries = crashcatPacketHeaderPayloadSize(pIo32StateV1->header) >> 3;
    NV_CRASHCAT_IO_APERTURE aperture = crashcatIo32StateV1Aperture(pIo32StateV1);

    switch (aperture)
    {
        case NV_CRASHCAT_IO_APERTURE_INTIO:
            CRASHCAT_REPORT_LOG_PACKET_TYPE(pReport, "Local I/O Register State:\n");
            break;
        case NV_CRASHCAT_IO_APERTURE_EXTIO:
            CRASHCAT_REPORT_LOG_PACKET_TYPE(pReport, "External I/O Register State:\n");
            break;
        default:
            CRASHCAT_REPORT_LOG_PACKET_TYPE(pReport, "Additional Register State:\n");
    }

    // Print 4 registers on a line to save space
    const NvU8 REGS_PER_LINE = 4;
    NvU16 idx;
    for (idx = 0; idx < (entries / REGS_PER_LINE) * REGS_PER_LINE; idx += REGS_PER_LINE)
        CRASHCAT_REPORT_LOG_DATA(pReport,
            "0x%08x:0x%08x   0x%08x:0x%08x   0x%08x:0x%08x   0x%08x:0x%08x\n",
            pIo32StateV1->regs[idx + 0].offset, pIo32StateV1->regs[idx + 0].value,
            pIo32StateV1->regs[idx + 1].offset, pIo32StateV1->regs[idx + 1].value,
            pIo32StateV1->regs[idx + 2].offset, pIo32StateV1->regs[idx + 2].value,
            pIo32StateV1->regs[idx + 3].offset, pIo32StateV1->regs[idx + 3].value);

    switch (entries - idx)
    {
        case 3:
            CRASHCAT_REPORT_LOG_DATA(pReport, "0x%08x:0x%08x   0x%08x:0x%08x   0x%08x:0x%08x\n",
                pIo32StateV1->regs[idx + 0].offset, pIo32StateV1->regs[idx + 0].value,
                pIo32StateV1->regs[idx + 1].offset, pIo32StateV1->regs[idx + 1].value,
                pIo32StateV1->regs[idx + 2].offset, pIo32StateV1->regs[idx + 2].value);
            break;
        case 2:
            CRASHCAT_REPORT_LOG_DATA(pReport, "0x%08x:0x%08x   0x%08x:0x%08x\n",
                pIo32StateV1->regs[idx + 0].offset, pIo32StateV1->regs[idx + 0].value,
                pIo32StateV1->regs[idx + 1].offset, pIo32StateV1->regs[idx + 1].value);
            break;
        case 1:
            CRASHCAT_REPORT_LOG_DATA(pReport, "0x%08x:0x%08x\n",
                pIo32StateV1->regs[idx + 0].offset, pIo32StateV1->regs[idx + 0].value);
            break;
        default:
            break;
    }
}

void crashcatReportLogToProtobuf_V1(CrashCatReport *pReport, PRB_ENCODER *pCrashcatProtobufData)
{
    NV_STATUS status = NV_OK;
    NvCrashCatReport_V1 *pReportV1 = &pReport->v1.report;
    NvCrashCatRiscv64CsrState_V1 *pCsrStateV1 = &pReport->v1.riscv64CsrState;
    NvCrashCatRiscv64GprState_V1 *pGprStateV1 = &pReport->v1.riscv64GprState;

    status = prbEncNestedStart(pCrashcatProtobufData, GSP_XIDREPORT_CRASHCATREPORT);
    if (status != NV_OK)
        return;

    prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_REPORT_PC, pReportV1->sourcePc);
    prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_REPORT_TIMESTAMP, crashcatReportV1ReporterTimestamp(pReportV1));

    switch (crashcatReportV1SourceMode(pReportV1))
    {
        case NV_CRASHCAT_RISCV_MODE_M:
            status = prbEncAddEnum(pCrashcatProtobufData, CRASHCAT_REPORT_MODE, CRASHCAT_REPORT_MODE_M);
            break;
        case NV_CRASHCAT_RISCV_MODE_S:
            status = prbEncAddEnum(pCrashcatProtobufData, CRASHCAT_REPORT_MODE, CRASHCAT_REPORT_MODE_S);
            break;
        case NV_CRASHCAT_RISCV_MODE_U:
            status = prbEncAddEnum(pCrashcatProtobufData, CRASHCAT_REPORT_MODE, CRASHCAT_REPORT_MODE_U);
            break;
        default:
            status = prbEncAddEnum(pCrashcatProtobufData, CRASHCAT_REPORT_MODE, CRASHCAT_REPORT_MODE_X);
    }

    crashcatReportLogVersionProtobuf_HAL(pReport, pCrashcatProtobufData);

    switch (crashcatReportV1SourceCauseType(pReportV1))
    {
        case NV_CRASHCAT_CAUSE_TYPE_EXCEPTION:
        {
            if (status == NV_OK)
                status = prbEncAddEnum(pCrashcatProtobufData, CRASHCAT_REPORT_CAUSE, CRASHCAT_REPORT_EXCEPTION);
            break;
        }
        case NV_CRASHCAT_CAUSE_TYPE_TIMEOUT:
        {
            if (status == NV_OK)
                status = prbEncAddEnum(pCrashcatProtobufData, CRASHCAT_REPORT_CAUSE, CRASHCAT_REPORT_TIMEOUT);
            break;
        }
        case NV_CRASHCAT_CAUSE_TYPE_PANIC:
        {
            if (status == NV_OK)
                status = prbEncAddEnum(pCrashcatProtobufData, CRASHCAT_REPORT_CAUSE, CRASHCAT_REPORT_PANIC);
            break;
        }
        default:
            if (status == NV_OK)
                status = prbEncAddEnum(pCrashcatProtobufData, CRASHCAT_REPORT_CAUSE, CRASHCAT_REPORT_UNKNOWN);
    }

    prbEncAddUInt32(pCrashcatProtobufData, CRASHCAT_REPORT_SOURCEPARTITION, crashcatReportV1SourcePartition(pReportV1));
    prbEncAddUInt32(pCrashcatProtobufData, CRASHCAT_REPORT_SOURCEUCODEID, crashcatReportV1SourceUcodeId(pReportV1));
    prbEncAddUInt32(pCrashcatProtobufData, CRASHCAT_REPORT_REPORTERPARTITION, crashcatReportV1ReporterPartition(pReportV1));
    prbEncAddUInt32(pCrashcatProtobufData, CRASHCAT_REPORT_REPORTERUCODEID, crashcatReportV1ReporterUcodeId(pReportV1));

    status = prbEncNestedStart(pCrashcatProtobufData, CRASHCAT_REPORT_CSRSTATE);
    if (status == NV_OK)
    {
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64CSRSTATE_XSTATUS, pCsrStateV1->xstatus);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64CSRSTATE_XIE, pCsrStateV1->xie);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64CSRSTATE_XIP, pCsrStateV1->xip);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64CSRSTATE_XEPC, pCsrStateV1->xepc);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64CSRSTATE_XTVAL, pCsrStateV1->xtval);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64CSRSTATE_XCAUSE, pCsrStateV1->xcause);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64CSRSTATE_XSCRATCH, pCsrStateV1->xscratch);
        prbEncNestedEnd(pCrashcatProtobufData);
    }

    status = prbEncNestedStart(pCrashcatProtobufData, CRASHCAT_REPORT_GPRSTATE);
    if (status == NV_OK)
    {
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_RA, pGprStateV1->ra);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_SP, pGprStateV1->sp);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_GP, pGprStateV1->gp);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_TP, pGprStateV1->tp);

        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_A0, pGprStateV1->a0);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_A1, pGprStateV1->a1);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_A2, pGprStateV1->a2);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_A3, pGprStateV1->a3);

        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_A4, pGprStateV1->a4);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_A5, pGprStateV1->a5);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_A6, pGprStateV1->a6);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_A7, pGprStateV1->a7);

        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_S0, pGprStateV1->s0);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_S1, pGprStateV1->s1);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_S2, pGprStateV1->s2);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_S3, pGprStateV1->s3);

        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_S4, pGprStateV1->s4);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_S5, pGprStateV1->s5);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_S6, pGprStateV1->s6);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_S7, pGprStateV1->s7);

        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_S8, pGprStateV1->s8);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_S9, pGprStateV1->s9);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_S10, pGprStateV1->s10);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_S11, pGprStateV1->s11);

        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_T0, pGprStateV1->t0);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_T1, pGprStateV1->t1);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_T2, pGprStateV1->t2);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_T3, pGprStateV1->t3);

        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_T4, pGprStateV1->t4);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_T5, pGprStateV1->t5);
        prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_RISCV64GPRSTATE_T6, pGprStateV1->t6);

        prbEncNestedEnd(pCrashcatProtobufData);
    }

    if (pReport->v1.pIo32State != NULL)
    {
        NvCrashCatIo32State_V1 *pIo32StateV1 = pReport->v1.pIo32State;
        NvU16 entries = crashcatPacketHeaderPayloadSize(pIo32StateV1->header) >> 3;

        for (NvU16 idx = 0; idx < entries; idx++)
        {
            status = prbEncNestedStart(pCrashcatProtobufData, CRASHCAT_REPORT_LOCALIOREGISTER);
            if (status == NV_OK)
            {
                status = prbEncAddUInt32(pCrashcatProtobufData, CRASHCAT_LOCALIOREGISTER_ADDRESS, pIo32StateV1->regs[idx].offset);
                status = prbEncAddUInt32(pCrashcatProtobufData, CRASHCAT_LOCALIOREGISTER_VALUE, pIo32StateV1->regs[idx].value);
                status = prbEncNestedEnd(pCrashcatProtobufData);
            }
        }
    }

    if (pReport->v1.pRiscv64StackTrace != NULL)
    {
        NvCrashCatRiscv64Trace_V1 *pRiscv64StackTrace = pReport->v1.pRiscv64StackTrace;
        NvU16 entries = crashcatPacketHeaderPayloadSize(pRiscv64StackTrace->header) >> 3;

        for (NvU16 idx = 0; idx < entries; idx++)
        {
            if (status == NV_OK)
            {
                status = prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_REPORT_STACKTRACE, pRiscv64StackTrace->addr[idx]);
            }
        }
    }

    if (pReport->v1.pRiscv64PcTrace != NULL)
    {
        NvCrashCatRiscv64Trace_V1 *pRiscv64PcTrace = pReport->v1.pRiscv64PcTrace;
        NvU16 entries = crashcatPacketHeaderPayloadSize(pRiscv64PcTrace->header) >> 3;

        for (NvU16 idx = 0; idx < entries; idx++)
        {
            if (status == NV_OK)
            {
                status = prbEncAddUInt64(pCrashcatProtobufData, CRASHCAT_REPORT_PCTRACE, pRiscv64PcTrace->addr[idx]);
            }
        }
    }

    status = prbEncNestedEnd(pCrashcatProtobufData);
}

NvU64 crashcatReportRa_V1(CrashCatReport *pReport)
{
    return pReport->v1.riscv64GprState.ra;
}

NvU64 crashcatReportXcause_V1(CrashCatReport *pReport)
{
    return pReport->v1.riscv64CsrState.xcause;
}

NvU64 crashcatReportXtval_V1(CrashCatReport *pReport)
{
    return pReport->v1.riscv64CsrState.xtval;
}