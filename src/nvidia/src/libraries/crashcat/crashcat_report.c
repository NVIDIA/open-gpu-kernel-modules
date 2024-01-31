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
#include "utils/nvassert.h"

NV_STATUS crashcatReportConstruct_IMPL
(
    CrashCatReport *pReport,
    void **ppReportBytes,
    NvLength numBytes
)
{
    // Cache the CrashCatEngine pointer for quick access during logging
    pReport->pEngine = CRASHCAT_GET_ENGINE(pReport);

    // Extract the report bytes into our structured report record
    void *pBufferStart = *ppReportBytes;
    *ppReportBytes = crashcatReportExtract_HAL(pReport, pBufferStart, numBytes);
    if (pBufferStart == *ppReportBytes)
    {
        NV_PRINTF(LEVEL_ERROR, "no report data extracted from %" NvUPtr_fmtu " bytes\n", numBytes);
        return NV_ERR_INVALID_DATA;
    }

    return NV_OK;
}

void crashcatReportLog_IMPL(CrashCatReport *pReport)
{
    NV_CRASHCAT_PACKET_TYPE packetType;

    NV_ASSERT_OR_RETURN_VOID((pReport->validTags & NVBIT(NV_CRASHCAT_PACKET_TYPE_REPORT)) != 0);

    // TODO: acquire mutex to prevent multi-line reports interleaving

    crashcatReportLogSource_HAL(pReport);
    crashcatReportLogReporter_HAL(pReport);

    // Log additional data associated with the report
    FOR_EACH_INDEX_IN_MASK(32, packetType,
                           pReport->validTags & ~NVBIT(NV_CRASHCAT_PACKET_TYPE_REPORT))
    {
        switch (packetType)
        {
            case NV_CRASHCAT_PACKET_TYPE_RISCV64_CSR_STATE:
                crashcatReportLogRiscv64CsrState_HAL(pReport);
                break;
            case NV_CRASHCAT_PACKET_TYPE_RISCV64_GPR_STATE:
                crashcatReportLogRiscv64GprState_HAL(pReport);
                break;
            case NV_CRASHCAT_PACKET_TYPE_RISCV64_TRACE:
                crashcatReportLogRiscv64Traces_HAL(pReport);
                break;
            case NV_CRASHCAT_PACKET_TYPE_IO32_STATE:
                crashcatReportLogIo32State_HAL(pReport);
                break;
            default:
                NV_PRINTF(LEVEL_WARNING, "unrecognized packet type 0x%02x\n", packetType);
                break;
        }
    }
    FOR_EACH_INDEX_IN_MASK_END;

    crashcatEnginePrintf(pReport->pEngine, NV_FALSE,
                         CRASHCAT_LOG_PREFIX "------------[ end crash report ]------------");
}

// xcause CSR format and codes are a backward-compatible part of the RISC-V standard
#define RISCV_CSR_XCAUSE_EXCODE             4:0
#define RISCV_CSR_XCAUSE_EXCODE_IAMA        0x00000000
#define RISCV_CSR_XCAUSE_EXCODE_IACC_FAULT  0x00000001
#define RISCV_CSR_XCAUSE_EXCODE_ILL         0x00000002
#define RISCV_CSR_XCAUSE_EXCODE_BKPT        0x00000003
#define RISCV_CSR_XCAUSE_EXCODE_LAMA        0x00000004
#define RISCV_CSR_XCAUSE_EXCODE_LACC_FAULT  0x00000005
#define RISCV_CSR_XCAUSE_EXCODE_SAMA        0x00000006
#define RISCV_CSR_XCAUSE_EXCODE_SACC_FAULT  0x00000007
#define RISCV_CSR_XCAUSE_EXCODE_UCALL       0x00000008
#define RISCV_CSR_XCAUSE_EXCODE_SCALL       0x00000009
#define RISCV_CSR_XCAUSE_EXCODE_MCALL       0x0000000b
#define RISCV_CSR_XCAUSE_EXCODE_IPAGE_FAULT 0x0000000c
#define RISCV_CSR_XCAUSE_EXCODE_LPAGE_FAULT 0x0000000d
#define RISCV_CSR_XCAUSE_EXCODE_SPAGE_FAULT 0x0000000f
#define RISCV_CSR_XCAUSE_EXCODE_U_SWINT     0x00000000
#define RISCV_CSR_XCAUSE_EXCODE_S_SWINT     0x00000001
#define RISCV_CSR_XCAUSE_EXCODE_M_SWINT     0x00000003
#define RISCV_CSR_XCAUSE_EXCODE_U_TINT      0x00000004
#define RISCV_CSR_XCAUSE_EXCODE_S_TINT      0x00000005
#define RISCV_CSR_XCAUSE_EXCODE_M_TINT      0x00000007
#define RISCV_CSR_XCAUSE_EXCODE_U_EINT      0x00000008
#define RISCV_CSR_XCAUSE_EXCODE_S_EINT      0x00000009
#define RISCV_CSR_XCAUSE_EXCODE_M_EINT      0x0000000b
#define RISCV_CSR_XCAUSE_INT                63:63

#define RISCV_CSR_XCAUSE_EXCODE_CASE(code, str)  \
    case RISCV_CSR_XCAUSE_EXCODE_ ## code: return MAKE_NV_PRINTF_STR(str)

const char *crashcatReportRiscvCauseToString(NvU64 xcause)
{
    NvBool bIntr = (NvBool)REF_VAL64(RISCV_CSR_XCAUSE_INT, xcause);
    NvU8 excode = (NvU8)REF_VAL64(RISCV_CSR_XCAUSE_EXCODE, xcause);

    if (bIntr)
    {
        switch (excode)
        {
            RISCV_CSR_XCAUSE_EXCODE_CASE(U_SWINT, "user software interrupt");
            RISCV_CSR_XCAUSE_EXCODE_CASE(S_SWINT, "supervisor software interrupt");
            RISCV_CSR_XCAUSE_EXCODE_CASE(M_SWINT, "machine software interrupt");
            RISCV_CSR_XCAUSE_EXCODE_CASE(U_TINT, "user timer interrupt");
            RISCV_CSR_XCAUSE_EXCODE_CASE(S_TINT, "supervisor timer interrupt");
            RISCV_CSR_XCAUSE_EXCODE_CASE(M_TINT, "machine timer interrupt");
            RISCV_CSR_XCAUSE_EXCODE_CASE(U_EINT, "user external interrupt");
            RISCV_CSR_XCAUSE_EXCODE_CASE(S_EINT, "supervisor external interrupt");
            RISCV_CSR_XCAUSE_EXCODE_CASE(M_EINT, "machine external interrupt");
            default: return MAKE_NV_PRINTF_STR("unknown interrupt");
        }
    }
    else
    {
        switch (excode)
        {
            RISCV_CSR_XCAUSE_EXCODE_CASE(IAMA, "instruction address misaligned");
            RISCV_CSR_XCAUSE_EXCODE_CASE(IACC_FAULT, "instruction access fault");
            RISCV_CSR_XCAUSE_EXCODE_CASE(ILL, "illegal instruction");
            RISCV_CSR_XCAUSE_EXCODE_CASE(BKPT, "breakpoint");
            RISCV_CSR_XCAUSE_EXCODE_CASE(LAMA, "load address misaligned");
            RISCV_CSR_XCAUSE_EXCODE_CASE(LACC_FAULT, "load access fault");
            RISCV_CSR_XCAUSE_EXCODE_CASE(SAMA, "store address misaligned");
            RISCV_CSR_XCAUSE_EXCODE_CASE(SACC_FAULT, "store access fault");
            RISCV_CSR_XCAUSE_EXCODE_CASE(UCALL, "environment call from U-mode");
            RISCV_CSR_XCAUSE_EXCODE_CASE(SCALL, "environment call from S-mode");
            RISCV_CSR_XCAUSE_EXCODE_CASE(MCALL, "environment call from M-mode");
            RISCV_CSR_XCAUSE_EXCODE_CASE(IPAGE_FAULT, "instruction access page fault");
            RISCV_CSR_XCAUSE_EXCODE_CASE(LPAGE_FAULT, "load access page fault");
            RISCV_CSR_XCAUSE_EXCODE_CASE(SPAGE_FAULT, "store access page fault");
            default: return MAKE_NV_PRINTF_STR("unknown exception");
        }
    }
}
#undef RISCV_CSR_MCAUSE_EXCODE_CASE
