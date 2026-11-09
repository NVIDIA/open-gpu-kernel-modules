/*
 * SPDX-FileCopyrightText: Copyright (c) 2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#include "events/gpu/ras/ras_events.h"

#include "diagnostics/op_event.h"
#include "gpu/gpu.h"
#include "nverror.h"
#include "nvport/nvport.h"

#define ECC_ROW_REMAP_PENDING_INTR_XID_MESSAGE_FMT \
    "Row Remapper: New row (0x%016llx) marked for remapping, reset gpu to activate."
#define ECC_ROW_REMAP_PENDING_CTRL_CALL_XID_MESSAGE_FMT \
    "Row Remapper: New row marked for remapping, reset gpu to activate."
#define ECC_ROW_REMAPPING_FAILURE_TABLE_FULL_XID_MESSAGE_FMT \
    "Row Remapper Error: (0x%016llx) - Row Remapping table is full"
#define ECC_ROW_REMAPPING_FAILURE_BANK_FULL_XID_MESSAGE_FMT \
    "Row Remapper Error: (0x%016llx) - All reserved rows for bank are remapped"
#define ECC_ROW_REMAPPING_FAILURE_INTERNAL_ERROR_XID_MESSAGE_FMT \
    "Row Remapper Error: (0x%016llx) - Internal %s error"
#define ECC_ROW_REMAPPING_FAILURE_RESERVED_ROW_ERROR_XID_MESSAGE_FMT \
    "Row Remapper Error: (0x%016llx) - Attempting to remap a reserved row"
#define ECC_ROW_REMAPPING_FAILURE_PAGE_OFFLINE_XID_MESSAGE_FMT \
    "Row Remapper: (0x%016llx) - Attempting to remap a row that is already pending remapping. " \
    "Remapping will occur when the GPU is reset"

#define ECC_DRAM_RETIREMENT_FAILURE_INTERNAL_ERROR_XID_MESSAGE_FMT \
    "DRAM Retirement failed due to internal error at 0x%016llx"
#define ECC_DRAM_RETIREMENT_FAILURE_DISABLED_RETIREMENT_XID_MESSAGE_FMT \
    "DRAM Retirement failed due to disabled retirement at 0x%016llx"
#define ECC_DRAM_RETIREMENT_FAILURE_PENDING_ROW_REMAP_XID_MESSAGE_FMT \
    "DRAM Retirement failed due to pending row remap at 0x%016llx"
#define ECC_DRAM_RETIREMENT_FAILURE_PENDING_BANK_REMAP_XID_MESSAGE_FMT \
    "DRAM Retirement failed due to pending bank remap at 0x%016llx"
#define ECC_DRAM_RETIREMENT_FAILURE_PENDING_LTS_SWAP_XID_MESSAGE_FMT \
    "DRAM Retirement failed due to pending LTS swap at 0x%016llx"
#define ECC_DRAM_RETIREMENT_FAILURE_ADDRESS_RETRIEVAL_ERROR_XID_MESSAGE_FMT \
    "DRAM Retirement failed due to internal address retrieval error at 0x%016llx"
#define ECC_DRAM_RETIREMENT_FAILURE_ECC_ADDRESS_TRACKING_ERROR_XID_MESSAGE_FMT \
    "DRAM Retirement failed due to internal ECC address tracking error at 0x%016llx"
#define ECC_DRAM_RETIREMENT_FAILURE_UNKNOWN_ERROR_XID_MESSAGE_FMT \
    "DRAM Retirement failed due to unknown error at 0x%016llx"
#define ECC_DRAM_RETIREMENT_FAILURE_INFOROM_FULL_XID_MESSAGE_FMT \
    "DRAM Retirement failed due to full inforom object at 0x%016llx"
#define ECC_DRAM_RETIREMENT_FAILURE_HW_LIMIT_XID_MESSAGE_FMT \
    "DRAM Retirement failed due to HW limit reached at 0x%016llx"
#define ECC_DRAM_RETIREMENT_FAILURE_NO_SPARE_XID_MESSAGE_FMT \
    "DRAM Retirement failed due to no spare for retirement at 0x%016llx"

#define ECC_BANK_REMAP_PENDING_XID_MESSAGE_FMT \
    "Bank Remapper: New bank marked for remapping, reset gpu to activate."

#define ECC_LTS_REPAIR_PENDING_XID_MESSAGE_FMT \
    "Marking LTS %d in FPB %d along with its pair for repair. Perform %s to activate repair."
#define ECC_CHANNEL_REPAIR_PENDING_XID_MESSAGE_FMT \
    "Marking Channel %d in FBPA %d along with its pair for repair. Perform %s to activate repair."
#define ECC_LTS_REPAIR_FAILURE_XID_MESSAGE_FMT \
    "Repairing LTS failed as there are no more spare L2 slices."
#define ECC_CHANNEL_REPAIR_FAILURE_XID_MESSAGE_FMT \
    "Repairing Channel failed as there are no more spare channels."

#define ECC_SPARING_REPAIR_PENDING_XID_MESSAGE_FMT \
    "Sparing Repair Pending"
#define ECC_SPARING_REPAIR_NOT_POSSIBLE_XID_MESSAGE_FMT \
    "Sparing Repair Not Possible"
#define ECC_SPARING_EVENT_XID_MESSAGE_FMT \
    "Sparing Event detected"

#define ECC_DRAM_ECC_INTR_STORM_XID_MESSAGE_FMT \
    "Disabling ECC single-bit error interrupts in framebuffer at logical partition " \
    "%d, due to high error rate"
#define ECC_SM_ECC_INTR_STORM_XID_MESSAGE_FMT \
    "SM SBE interrupt storm detected"
#define ECC_RESIDUAL_UNCORRECTABLE_ERROR_XID_MESSAGE_FMT \
    "An uncorrectable ECC error detected (possible firmware handling failure) " \
    "DRAM:%d, LTC:%d, MMU:%d, PCIE:%d"
#define ECC_UNIT_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT \
    "An uncorrectable ECC error has been detected on GPU in the %s unit"
#define ECC_UNIT_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT \
    "%s, Uncorrectable SRAM Error"
#define ECC_COPY_ENGINE_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT \
    "An uncorrectable ECC error has been detected on GPU in CE dieletType %d, " \
    "dieletId %d, PCE %d"
#define ECC_COPY_ENGINE_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT \
    "CE, Uncorrectable SRAM Error in dieletType %d dieletId %d PCE %d"
#define ECC_FECS_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT \
    "An uncorrectable ECC error has been detected on GPU in FECS"
#define ECC_FECS_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT \
    "FECS, Uncorrectable SRAM error"
#define ECC_SM_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT \
    "An uncorrectable ECC error has been detected on the GPU in the %s at gpc %d, tpc %d."
#define ECC_SM_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT \
    "%s, Uncorrectable SRAM error in GPC %d TPC %d"
#define ECC_GCC_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT \
    "An uncorrectable ECC error has been detected on GPU in GPC%d GCC"
#define ECC_GCC_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT \
    "GCC, Uncorrectable SRAM error in GPC %d"
#define ECC_CONTEXT_SWITCH_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT \
    "An uncorrectable ECC error has been detected on GPU in GPC%d GPCCS"
#define ECC_CONTEXT_SWITCH_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT \
    "GPCCS, Uncorrectable SRAM error in GPC %d"
#define ECC_GPCMMU_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT \
    "An uncorrectable ECC error has been detected on GPU in the GPCMMU at gpc %d."
#define ECC_GPCMMU_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT \
    "GPCMMU, Uncorrectable SRAM error in GPC %d"
#define ECC_GSP_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT \
    "An uncorrectable ECC error has been detected on GPU in GSP-RISCV"
#define ECC_GSP_UNCORRECTABLE_ERROR_LOCATION_XID48_MESSAGE_FMT \
    "An uncorrectable ECC error has been detected on GPU in GSP-RISCV location %d"
#define ECC_GSP_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT \
    "GSP-RISCV, Uncorrectable SRAM error"
#define ECC_GSP_UNCORRECTABLE_ERROR_LOCATION_XID172_MESSAGE_FMT \
    "GSP-RISCV, Uncorrectable SRAM Error in location %d"
#define ECC_LPSEC_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT \
    "An uncorrectable ECC error has been detected on GPU in LPSEC sequencer %d"
#define ECC_LPSEC_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT \
    "LPSEC, Uncorrectable SRAM Error in sequencer %d"
#define ECC_NVLINK_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT \
    "%s uncorrectable ECC error"
#define ECC_NVLINK_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT \
    "%s, Uncorrectable SRAM Error"
#define ECC_PMU_FALCON_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT \
    "An uncorrectable ECC error has been detected on GPU in the PMU Falcon"
#define ECC_PMU_FALCON_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT \
    "PMU, Uncorrectable SRAM Error"
#define ECC_PMU_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT \
    "An uncorrectable ECC error has been detected on GPU in PMU-RISCV"
#define ECC_PMU_UNCORRECTABLE_ERROR_LOCATION_XID48_MESSAGE_FMT \
    "An uncorrectable ECC error has been detected on GPU in PMU-RISCV location %d"
#define ECC_PMU_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT \
    "PMU-RISCV, Uncorrectable SRAM Error"
#define ECC_PMU_UNCORRECTABLE_ERROR_LOCATION_XID172_MESSAGE_FMT \
    "PMU-RISCV, Uncorrectable SRAM Error in location %d"
#define ECC_SEC2_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT \
    "An uncorrectable ECC error has been detected on GPU in SEC2-RISCV"
#define ECC_SEC2_UNCORRECTABLE_ERROR_LOCATION_XID48_MESSAGE_FMT \
    "An uncorrectable ECC error has been detected on GPU in SEC2-RISCV location %d"
#define ECC_SEC2_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT \
    "SEC2-RISCV, Uncorrectable SRAM Error"
#define ECC_SEC2_UNCORRECTABLE_ERROR_LOCATION_XID172_MESSAGE_FMT \
    "SEC2-RISCV, Uncorrectable SRAM Error in location %d"
#define ECC_DRAM_DBE_XID_MESSAGE_FMT \
    "An uncorrectable double bit error (DBE) has been detected on GPU " \
    "in the framebuffer at physAddr 0x%08llx partition %d, subpartition %d."
#define ECC_DRAM_ERROR_XID_MESSAGE_FMT \
    "%s, Uncorrectable DRAM error in FBPA %d subpartition %d physAddr 0x%08llx"
#define ECC_DRAM_ERROR_WITH_BANK_REMAP_XID_MESSAGE_FMT \
    "%s, Uncorrectable DRAM error in FBPA %d subpartition %d physAddr 0x%08llx (0x%08x)"
#define ECC_DRAM_CBC_FATAL_ERROR_XID_MESSAGE_FMT \
    "Uncontained: DED CBC (0x%x, 0x%x). RST: Yes, D-RST: No"
#define ECC_DRAM_CBC_NON_FATAL_ERROR_XID_MESSAGE_FMT \
    "Contained: DED CBC (0x%x, 0x%x). RST: No, D-RST: Yes"
#define ECC_LTC_UNSUPPORTED_CLIENT_POISON_UNCONTAINED_ERROR_XID_MESSAGE_FMT \
    "Uncontained: LTC GPC (0x%x, 0x%x). RST: No, D-RST: No"
#define ECC_LTC_UNSUPPORTED_CLIENT_POISON_CONTAINED_ERROR_XID_MESSAGE_FMT \
    "Contained: LTC GPC (0x%x, 0x%x). RST: No, D-RST: No"
#define ECC_LTC_DBE_XID_MESSAGE_FMT \
    "An uncorrectable double bit error (DBE) has been detected on GPU " \
    "in the L2 cache at cache %d, slice %d."
#define ECC_LTC_SRAM_ERROR_XID_MESSAGE_FMT \
    "LTC, Uncorrectable SRAM error in LTC %d slice %d"
#define ECC_LTC_FATAL_ERROR_XID_MESSAGE_FMT \
    "Uncontained: %s (0x%x, 0x%x). RST: Yes, D-RST: No"
#define ECC_LRC_DBE_XID_MESSAGE_FMT \
    "An uncorrectable double bit error (DBE) has been detected on GPU " \
    "in LRCC %d, LRC %d."
#define ECC_LRC_SRAM_ERROR_XID_MESSAGE_FMT \
    "LRC, Uncorrectable SRAM error in LRCC %d LRC %d"
#define ECC_LRC_UNCONTAINED_XID_MESSAGE_FMT \
    "Uncontained: LRC (0x%x, 0x%x). RST: Yes, D-RST: No"
#define ECC_PCIE_INGRESS_POISON_CONTAINED_ERROR_XID_MESSAGE_FMT \
    "Contained: PCIE Ingress (0x%x). RST: No, D-RST: No"
#define ECC_PCIE_INGRESS_POISON_UNCONTAINED_ERROR_XID_MESSAGE_FMT \
    "Uncontained: PCIE Ingress. RST: Yes, D-RST: No"
#define ECC_PCIE_EGRESS_POISON_ERROR_XID_MESSAGE_FMT \
    "Uncontained: PCIE Egress. RST: Yes, D-RST: No"
#define ECC_FBHUB_FATAL_POISON_ERROR_XID_MESSAGE_FMT \
    "Uncontained: FBHUB. RST: Yes, D-RST: No"
#define ECC_FBHUB_NON_FATAL_POISON_ERROR_XID_MESSAGE_FMT \
    "Uncontained: FBHUB. RST: No, D-RST: No"
#define ECC_MMU_POISON_ERROR_XID_MESSAGE_FMT \
    "Contained: MMU. RST: No, D-RST: No"
#define ECC_SM_POISON_ERROR_XID_MESSAGE_FMT \
    "Contained: SM (0x%x). RST: No, D-RST: No"
#define ECC_GCC_POISON_ERROR_XID_MESSAGE_FMT \
    "Contained: GCC (0x%x). RST: No, D-RST: No"
#define ECC_CONTEXT_SWITCH_POISON_ERROR_XID_MESSAGE_FMT \
    "Contained: CTXSW (0x%x). RST: No, D-RST: No"
#define ECC_GSP_POISON_ERROR_XID_MESSAGE_FMT \
    "Contained: GSP. RST: Yes, D-RST: No"
#define ECC_PMU_POISON_ERROR_XID_MESSAGE_FMT \
    "Contained: PMU. RST: Yes, D-RST: No"
#define ECC_SEC2_POISON_ERROR_XID_MESSAGE_FMT \
    "Contained: SEC2. RST: Yes, D-RST: No"
#define ECC_FBFALCON_POISON_ERROR_XID_MESSAGE_FMT \
    "Contained: FB Falcon. RST: Yes, D-RST: No"
#define ECC_VIDEO_ENGINE_POISON_ERROR_XID_MESSAGE_FMT \
    "Contained: %s (0x%x). RST: No, D-RST: No"
#define ECC_COPY_ENGINE_POISON_CONTAINED_ERROR_XID_MESSAGE_FMT \
    "Contained: CE User Channel (0x%x). RST: No, D-RST: No"
#define ECC_COPY_ENGINE_POISON_UNCONTAINED_ERROR_XID_MESSAGE_FMT \
    "Uncontained: CE Kernel Channel (0x%x). RST: Yes, D-RST: No"
#define ECC_SYSLTC_DBE_XID_MESSAGE_FMT \
    "An uncorrectable double bit error (DBE) has been detected on GPU " \
    "in SYSLTC dieletType %d, dieletId %d, instance %d, slice %d."
#define ECC_SYSLTC_SRAM_ERROR_XID_MESSAGE_FMT \
    "SYSLTC, Uncorrectable SRAM error in SYSLTC dieletType %d, " \
    "dieletId %d, instance %d slice %d"
#define ECC_SYSLTC_FATAL_ERROR_XID_MESSAGE_FMT \
    "Uncontained: %s (0x%x, 0x%x, 0x%x, 0x%x). RST: Yes, D-RST: No"

static const char *
_eccLtcFatalErrorXidErrorString
(
    NvBool bvidL2,
    L2_ERROR_TYPE errorType
)
{
    switch (errorType)
    {
        case L2_ERROR_TYPE_DSTG:
            return bvidL2 ? "LTC Data Fatal" : "SYSLTC Data Fatal";
        case L2_ERROR_TYPE_TSTG:
            return bvidL2 ? "LTC TAG" : "SYSLTC Tag";
        case L2_ERROR_TYPE_RSTG:
            return bvidL2 ? "LTC CBC" : "SYSLTC CBC";
        default:
            return bvidL2 ? "LTC Unknown" : "SYSLTC Unknown";
    }
}

static const char *
_eccDramRetirementFailureInternalErrorXidMessageFmt
(
    ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON errorReason
)
{
    switch (errorReason)
    {
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_INTERNAL_ERROR:
            return ECC_DRAM_RETIREMENT_FAILURE_INTERNAL_ERROR_XID_MESSAGE_FMT;
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_DISABLED_RETIREMENT:
            return ECC_DRAM_RETIREMENT_FAILURE_DISABLED_RETIREMENT_XID_MESSAGE_FMT;
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_PENDING_ROW_REMAP:
            return ECC_DRAM_RETIREMENT_FAILURE_PENDING_ROW_REMAP_XID_MESSAGE_FMT;
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_PENDING_BANK_REMAP:
            return ECC_DRAM_RETIREMENT_FAILURE_PENDING_BANK_REMAP_XID_MESSAGE_FMT;
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_PENDING_LTS_SWAP:
            return ECC_DRAM_RETIREMENT_FAILURE_PENDING_LTS_SWAP_XID_MESSAGE_FMT;
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_ADDRESS_RETRIEVAL_ERROR:
            return ECC_DRAM_RETIREMENT_FAILURE_ADDRESS_RETRIEVAL_ERROR_XID_MESSAGE_FMT;
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_ECC_ADDRESS_TRACKING_ERROR:
            return ECC_DRAM_RETIREMENT_FAILURE_ECC_ADDRESS_TRACKING_ERROR_XID_MESSAGE_FMT;
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_UNKNOWN_ERROR:
        default:
            return ECC_DRAM_RETIREMENT_FAILURE_UNKNOWN_ERROR_XID_MESSAGE_FMT;
    }
}

static void _eccRowRemapPendingEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    RowRemappingCtx *pContext = (RowRemappingCtx *)pContextHeader;

    if (pContext->source == GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT)
    {
        OP_EVENT_XID_LOG(pGpu, bSimulated, INFOROM_DRAM_RETIREMENT_EVENT, ECC_ROW_REMAP_PENDING_INTR_XID_MESSAGE_FMT,
                     pContext->physicalAddress);
    }
    else
    {
        OP_EVENT_XID_LOG(pGpu, bSimulated, INFOROM_DRAM_RETIREMENT_EVENT,
                     ECC_ROW_REMAP_PENDING_CTRL_CALL_XID_MESSAGE_FMT);
    }
}

static void _eccRowRemappingFailureTableFullEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    RowRemappingFailureCtx *pContext = (RowRemappingFailureCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, INFOROM_DRAM_RETIREMENT_FAILURE,
                 ECC_ROW_REMAPPING_FAILURE_TABLE_FULL_XID_MESSAGE_FMT,
                 pContext->physicalAddress);
}

static void _eccRowRemappingFailureBankFullEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    RowRemappingFailureCtx *pContext = (RowRemappingFailureCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, INFOROM_DRAM_RETIREMENT_FAILURE,
                 ECC_ROW_REMAPPING_FAILURE_BANK_FULL_XID_MESSAGE_FMT,
                 pContext->physicalAddress);
}

static void _eccRowRemappingFailureInternalErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    RowRemappingFailureCtx *pContext = (RowRemappingFailureCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, INFOROM_DRAM_RETIREMENT_FAILURE,
                 ECC_ROW_REMAPPING_FAILURE_INTERNAL_ERROR_XID_MESSAGE_FMT,
                 pContext->physicalAddress,
                 pContext->errorString);
}

static void _eccDramRetirementFailureInternalErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    DramRetirementFailureInternalErrorCtx *pContext =
        (DramRetirementFailureInternalErrorCtx *)pContextHeader;

    switch (pContext->errorReason)
    {
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_INTERNAL_ERROR:
            OP_EVENT_XID_LOG(pGpu, bSimulated, INFOROM_DRAM_RETIREMENT_FAILURE,
                        ECC_DRAM_RETIREMENT_FAILURE_INTERNAL_ERROR_XID_MESSAGE_FMT,
                         pContext->physicalAddress);
            break;
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_DISABLED_RETIREMENT:
            OP_EVENT_XID_LOG(pGpu, bSimulated, INFOROM_DRAM_RETIREMENT_FAILURE,
                        ECC_DRAM_RETIREMENT_FAILURE_DISABLED_RETIREMENT_XID_MESSAGE_FMT,
                         pContext->physicalAddress);
            break;
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_PENDING_ROW_REMAP:
            OP_EVENT_XID_LOG(pGpu, bSimulated, INFOROM_DRAM_RETIREMENT_FAILURE,
                        ECC_DRAM_RETIREMENT_FAILURE_PENDING_ROW_REMAP_XID_MESSAGE_FMT,
                         pContext->physicalAddress);
            break;
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_PENDING_BANK_REMAP:
            OP_EVENT_XID_LOG(pGpu, bSimulated, INFOROM_DRAM_RETIREMENT_FAILURE,
                        ECC_DRAM_RETIREMENT_FAILURE_PENDING_BANK_REMAP_XID_MESSAGE_FMT,
                         pContext->physicalAddress);
            break;
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_PENDING_LTS_SWAP:
            OP_EVENT_XID_LOG(pGpu, bSimulated, INFOROM_DRAM_RETIREMENT_FAILURE,
                        ECC_DRAM_RETIREMENT_FAILURE_PENDING_LTS_SWAP_XID_MESSAGE_FMT,
                         pContext->physicalAddress);
            break;
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_ADDRESS_RETRIEVAL_ERROR:
            OP_EVENT_XID_LOG(pGpu, bSimulated, INFOROM_DRAM_RETIREMENT_FAILURE,
                        ECC_DRAM_RETIREMENT_FAILURE_ADDRESS_RETRIEVAL_ERROR_XID_MESSAGE_FMT,
                         pContext->physicalAddress);
            break;
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_ECC_ADDRESS_TRACKING_ERROR:
            OP_EVENT_XID_LOG(pGpu, bSimulated, INFOROM_DRAM_RETIREMENT_FAILURE,
                        ECC_DRAM_RETIREMENT_FAILURE_ECC_ADDRESS_TRACKING_ERROR_XID_MESSAGE_FMT,
                         pContext->physicalAddress);
            break;
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_UNKNOWN_ERROR:
        default:
            OP_EVENT_XID_LOG(pGpu, bSimulated, INFOROM_DRAM_RETIREMENT_FAILURE,
                        ECC_DRAM_RETIREMENT_FAILURE_UNKNOWN_ERROR_XID_MESSAGE_FMT,
                         pContext->physicalAddress);
            break;
    }
}

static void _eccRowRemappingFailureReservedRowEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    RowRemappingFailureCtx *pContext = (RowRemappingFailureCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, INFOROM_DRAM_RETIREMENT_FAILURE,
                 ECC_ROW_REMAPPING_FAILURE_RESERVED_ROW_ERROR_XID_MESSAGE_FMT,
                 pContext->physicalAddress);
}

static void _eccRowRemappingPageOfflineFailureEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    RowRemappingFailureCtx *pContext = (RowRemappingFailureCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, INFOROM_DRAM_RETIREMENT_FAILURE,
                 ECC_ROW_REMAPPING_FAILURE_PAGE_OFFLINE_XID_MESSAGE_FMT,
                 pContext->physicalAddress);
}

static const EventContextOutputAdapters g_eccRowRemapPendingOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccRowRemapPendingEmitOsLogXid,
};

static const EventContextOutputAdapters g_eccRowRemappingFailureReservedRowOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccRowRemappingFailureReservedRowEmitOsLogXid,
};

static const EventContextOutputAdapters g_eccRowRemappingFailureTableFullOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccRowRemappingFailureTableFullEmitOsLogXid,
};

static const EventContextOutputAdapters g_eccRowRemappingFailureBankFullOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccRowRemappingFailureBankFullEmitOsLogXid,
};

static const EventContextOutputAdapters g_eccRowRemappingFailureInternalErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccRowRemappingFailureInternalErrorEmitOsLogXid,
};

static const EventContextOutputAdapters g_eccDramRetirementFailureInternalErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccDramRetirementFailureInternalErrorEmitOsLogXid,
};

static void _eccDramRetirementInforomFullEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    RowRemappingFailureCtx *pContext = (RowRemappingFailureCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, INFOROM_DRAM_RETIREMENT_FAILURE,
                 ECC_DRAM_RETIREMENT_FAILURE_INFOROM_FULL_XID_MESSAGE_FMT,
                 pContext->physicalAddress);
}

static const EventContextOutputAdapters g_eccDramRetirementInforomFullOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccDramRetirementInforomFullEmitOsLogXid,
};

static void _eccDramRetirementHwLimitEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    RowRemappingFailureCtx *pContext = (RowRemappingFailureCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, INFOROM_DRAM_RETIREMENT_FAILURE,
                 ECC_DRAM_RETIREMENT_FAILURE_HW_LIMIT_XID_MESSAGE_FMT,
                 pContext->physicalAddress);
}

static const EventContextOutputAdapters g_eccDramRetirementHwLimitOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccDramRetirementHwLimitEmitOsLogXid,
};

static void _eccDramRetirementNoSpareEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    RowRemappingFailureCtx *pContext = (RowRemappingFailureCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, INFOROM_DRAM_RETIREMENT_FAILURE,
                 ECC_DRAM_RETIREMENT_FAILURE_NO_SPARE_XID_MESSAGE_FMT,
                 pContext->physicalAddress);
}

static const EventContextOutputAdapters g_eccDramRetirementNoSpareOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccDramRetirementNoSpareEmitOsLogXid,
};

static void _eccBankRemappingPendingEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    (void)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, BANK_REMAPPING_EVENT,
                 ECC_BANK_REMAP_PENDING_XID_MESSAGE_FMT);
}

static const EventContextOutputAdapters g_eccBankRemappingPendingOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccBankRemappingPendingEmitOsLogXid,
};

static void _eccLtsRepairPendingEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    LtsRepairCtx *pContext = (LtsRepairCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, CHANNEL_RETIREMENT_EVENT,
                 ECC_LTS_REPAIR_PENDING_XID_MESSAGE_FMT,
                 pContext->sublocation,
                 pContext->location,
                 pContext->errorString);
}

static const EventContextOutputAdapters g_eccLtsRepairPendingOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccLtsRepairPendingEmitOsLogXid,
};

static void _eccChannelRepairPendingEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    LtsRepairCtx *pContext = (LtsRepairCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, CHANNEL_RETIREMENT_EVENT,
                 ECC_CHANNEL_REPAIR_PENDING_XID_MESSAGE_FMT,
                 pContext->sublocation,
                 pContext->location,
                 pContext->errorString);
}

static const EventContextOutputAdapters g_eccChannelRepairPendingOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccChannelRepairPendingEmitOsLogXid,
};

static void _eccLtsRepairFailureEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    OP_EVENT_XID_LOG(pGpu, bSimulated, CHANNEL_RETIREMENT_FAILURE,
                 ECC_LTS_REPAIR_FAILURE_XID_MESSAGE_FMT);
}

static const EventContextOutputAdapters g_eccLtsRepairFailureOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccLtsRepairFailureEmitOsLogXid,
};

static void _eccChannelRepairFailureEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    OP_EVENT_XID_LOG(pGpu, bSimulated, CHANNEL_RETIREMENT_FAILURE,
                 ECC_CHANNEL_REPAIR_FAILURE_XID_MESSAGE_FMT);
}

static const EventContextOutputAdapters g_eccChannelRepairFailureOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccChannelRepairFailureEmitOsLogXid,
};

static void _eccSparingEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    GpuXidEventCtx *pContext = (GpuXidEventCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, pContext->data.xidCode,
                     "%s", pContext->data.message);
}

static const EventContextOutputAdapters g_eccSparingXidOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuXidEventCtxSerializeToEventBufferGoeCtx,
    .serializeToProtobuf = gpuXidEventCtxSerializeToProtobuf,
    .serializeToCperEventCtx = gpuXidEventCtxSerializeToCperCtx,
    .emitToOsLogXid = _eccSparingEmitOsLogXid,
};

static void _eccLtcErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    LtcErrorCtx *pContext = (LtcErrorCtx *)pContextHeader;

    if (!pContext->bvidL2)
    {
        OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_GPU_ECC_DBE,
                         ECC_SYSLTC_DBE_XID_MESSAGE_FMT,
                         pContext->dieletType,
                         pContext->dieletId,
                         pContext->ltc,
                         pContext->slice);
        OP_EVENT_XID_LOG(pGpu, bSimulated, UNCORRECTABLE_SRAM_ERROR,
                         ECC_SYSLTC_SRAM_ERROR_XID_MESSAGE_FMT,
                         pContext->dieletType,
                         pContext->dieletId,
                         pContext->ltc,
                         pContext->slice);
    }
    else
    {
        OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_GPU_ECC_DBE,
                         ECC_LTC_DBE_XID_MESSAGE_FMT,
                         pContext->ltc,
                         pContext->slice);
        OP_EVENT_XID_LOG(pGpu, bSimulated, UNCORRECTABLE_SRAM_ERROR,
                         ECC_LTC_SRAM_ERROR_XID_MESSAGE_FMT,
                         pContext->ltc,
                         pContext->slice);
    }
}

static const EventContextOutputAdapters g_eccLtcErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccLtcErrorEmitOsLogXid,
};

static void _eccLtcFatalErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    LtcErrorCtx *pContext = (LtcErrorCtx *)pContextHeader;

    if (!pContext->bvidL2)
    {
        OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_UNCONTAINED_ERROR,
                         ECC_SYSLTC_FATAL_ERROR_XID_MESSAGE_FMT,
                         _eccLtcFatalErrorXidErrorString(pContext->bvidL2,
                                                         pContext->errorType),
                         pContext->dieletType,
                         pContext->dieletId,
                         pContext->ltc,
                         pContext->slice);
    }
    else
    {
        OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_UNCONTAINED_ERROR,
                         ECC_LTC_FATAL_ERROR_XID_MESSAGE_FMT,
                         _eccLtcFatalErrorXidErrorString(pContext->bvidL2,
                                                         pContext->errorType),
                         pContext->ltc,
                         pContext->slice);
    }
}

static const EventContextOutputAdapters g_eccLtcFatalErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccLtcFatalErrorEmitOsLogXid,
};

static void _eccLtcUnsupportedClientPoisonUncontainedErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    LtcUnsupportedClientPoisonErrorCtx *pContext =
        (LtcUnsupportedClientPoisonErrorCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_UNCONTAINED_ERROR,
                     ECC_LTC_UNSUPPORTED_CLIENT_POISON_UNCONTAINED_ERROR_XID_MESSAGE_FMT,
                     pContext->partition,
                     pContext->slice);
}

static const EventContextOutputAdapters g_eccLtcUnsupportedClientPoisonUncontainedErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccLtcUnsupportedClientPoisonUncontainedErrorEmitOsLogXid,
};

static void _eccLtcUnsupportedClientPoisonContainedErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    LtcUnsupportedClientPoisonErrorCtx *pContext =
        (LtcUnsupportedClientPoisonErrorCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_CONTAINED_ERROR,
                     ECC_LTC_UNSUPPORTED_CLIENT_POISON_CONTAINED_ERROR_XID_MESSAGE_FMT,
                     pContext->partition,
                     pContext->slice);
}

static const EventContextOutputAdapters g_eccLtcUnsupportedClientPoisonContainedErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccLtcUnsupportedClientPoisonContainedErrorEmitOsLogXid,
};

static void _eccLrcFatalErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    LrcFatalErrorCtx *pContext = (LrcFatalErrorCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_GPU_ECC_DBE,
                     ECC_LRC_DBE_XID_MESSAGE_FMT,
                     pContext->slice,
                     pContext->partition);
    OP_EVENT_XID_LOG(pGpu, bSimulated, UNCORRECTABLE_SRAM_ERROR,
                     ECC_LRC_SRAM_ERROR_XID_MESSAGE_FMT,
                     pContext->slice,
                     pContext->partition);
    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_UNCONTAINED_ERROR,
                     ECC_LRC_UNCONTAINED_XID_MESSAGE_FMT,
                     pContext->partition,
                     pContext->slice);
}

static const EventContextOutputAdapters g_eccLrcFatalErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccLrcFatalErrorEmitOsLogXid,
};

static void _eccPcieIngressPoisonContainedErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    PcieIngressPoisonContainedErrorCtx *pContext =
        (PcieIngressPoisonContainedErrorCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_CONTAINED_ERROR,
                     ECC_PCIE_INGRESS_POISON_CONTAINED_ERROR_XID_MESSAGE_FMT,
                     pContext->gfid);
}

static const EventContextOutputAdapters g_eccPcieIngressPoisonContainedErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccPcieIngressPoisonContainedErrorEmitOsLogXid,
};

static void _eccPcieIngressPoisonUncontainedErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_UNCONTAINED_ERROR,
                     ECC_PCIE_INGRESS_POISON_UNCONTAINED_ERROR_XID_MESSAGE_FMT);
}

static const EventContextOutputAdapters g_eccPcieIngressPoisonUncontainedErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccPcieIngressPoisonUncontainedErrorEmitOsLogXid,
};

static void _eccPcieEgressPoisonErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_UNCONTAINED_ERROR,
                     ECC_PCIE_EGRESS_POISON_ERROR_XID_MESSAGE_FMT);
}

static const EventContextOutputAdapters g_eccPcieEgressPoisonErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccPcieEgressPoisonErrorEmitOsLogXid,
};

static void _eccFbHubFatalPoisonErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_UNCONTAINED_ERROR,
                     ECC_FBHUB_FATAL_POISON_ERROR_XID_MESSAGE_FMT);
}

static const EventContextOutputAdapters g_eccFbHubFatalPoisonErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccFbHubFatalPoisonErrorEmitOsLogXid,
};

static void _eccFbHubNonFatalPoisonErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_UNCONTAINED_ERROR,
                     ECC_FBHUB_NON_FATAL_POISON_ERROR_XID_MESSAGE_FMT);
}

static const EventContextOutputAdapters g_eccFbHubNonFatalPoisonErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccFbHubNonFatalPoisonErrorEmitOsLogXid,
};

static void _eccMmuPoisonErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_CONTAINED_ERROR,
                     ECC_MMU_POISON_ERROR_XID_MESSAGE_FMT);
}

static const EventContextOutputAdapters g_eccMmuPoisonErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccMmuPoisonErrorEmitOsLogXid,
};

static void _eccSmPoisonErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    SmPoisonErrorCtx *pContext = (SmPoisonErrorCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_CONTAINED_ERROR,
                     ECC_SM_POISON_ERROR_XID_MESSAGE_FMT,
                     pContext->nv2080EngineType);
}

static const EventContextOutputAdapters g_eccSmPoisonErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccSmPoisonErrorEmitOsLogXid,
};

static void _eccGccPoisonErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    GccPoisonErrorCtx *pContext = (GccPoisonErrorCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_CONTAINED_ERROR,
                     ECC_GCC_POISON_ERROR_XID_MESSAGE_FMT,
                     pContext->nv2080EngineType);
}

static const EventContextOutputAdapters g_eccGccPoisonErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccGccPoisonErrorEmitOsLogXid,
};

static void _eccContextSwitchPoisonErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    ContextSwitchPoisonErrorCtx *pContext =
        (ContextSwitchPoisonErrorCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_CONTAINED_ERROR,
                     ECC_CONTEXT_SWITCH_POISON_ERROR_XID_MESSAGE_FMT,
                     pContext->nv2080EngineType);
}

static const EventContextOutputAdapters g_eccContextSwitchPoisonErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccContextSwitchPoisonErrorEmitOsLogXid,
};

static void _eccGspPoisonErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_CONTAINED_ERROR,
                     ECC_GSP_POISON_ERROR_XID_MESSAGE_FMT);
}

static const EventContextOutputAdapters g_eccGspPoisonErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccGspPoisonErrorEmitOsLogXid,
};

static void _eccPmuPoisonErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_CONTAINED_ERROR,
                     ECC_PMU_POISON_ERROR_XID_MESSAGE_FMT);
}

static const EventContextOutputAdapters g_eccPmuPoisonErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccPmuPoisonErrorEmitOsLogXid,
};

static void _eccSec2PoisonErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_CONTAINED_ERROR,
                     ECC_SEC2_POISON_ERROR_XID_MESSAGE_FMT);
}

static const EventContextOutputAdapters g_eccSec2PoisonErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccSec2PoisonErrorEmitOsLogXid,
};

static void _eccFbFalconPoisonErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_CONTAINED_ERROR,
                     ECC_FBFALCON_POISON_ERROR_XID_MESSAGE_FMT);
}

static const EventContextOutputAdapters g_eccFbFalconPoisonErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccFbFalconPoisonErrorEmitOsLogXid,
};

static const char *_eccVideoEnginePoisonErrorGetEngineName
(
    NvU32 nv2080EngineType
)
{
    if (NV2080_ENGINE_TYPE_IS_NVDEC(nv2080EngineType))
    {
        return "NVDEC";
    }

    if (NV2080_ENGINE_TYPE_IS_NVJPEG(nv2080EngineType))
    {
        return "NVJPG";
    }

    if (NV2080_ENGINE_TYPE_IS_OFA(nv2080EngineType))
    {
        return "OFA";
    }

    return NULL;
}

static void _eccVideoEnginePoisonErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    VideoEnginePoisonErrorCtx *pContext =
        (VideoEnginePoisonErrorCtx *)pContextHeader;
    const char *pEngineName =
        _eccVideoEnginePoisonErrorGetEngineName(pContext->nv2080EngineType);

    NV_ASSERT(pEngineName != NULL);
    if (pEngineName == NULL)
    {
        return;
    }

    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_CONTAINED_ERROR,
                     ECC_VIDEO_ENGINE_POISON_ERROR_XID_MESSAGE_FMT,
                     pEngineName,
                     pContext->nv2080EngineType);
}

static const EventContextOutputAdapters g_eccVideoEnginePoisonErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccVideoEnginePoisonErrorEmitOsLogXid,
};

static void _eccCopyEnginePoisonContainedErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    CopyEnginePoisonErrorCtx *pContext =
        (CopyEnginePoisonErrorCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_CONTAINED_ERROR,
                     ECC_COPY_ENGINE_POISON_CONTAINED_ERROR_XID_MESSAGE_FMT,
                     pContext->nv2080EngineType);
}

static const EventContextOutputAdapters g_eccCopyEnginePoisonContainedErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccCopyEnginePoisonContainedErrorEmitOsLogXid,
};

static void _eccCopyEnginePoisonUncontainedErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    CopyEnginePoisonErrorCtx *pContext =
        (CopyEnginePoisonErrorCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_UNCONTAINED_ERROR,
                     ECC_COPY_ENGINE_POISON_UNCONTAINED_ERROR_XID_MESSAGE_FMT,
                     pContext->nv2080EngineType);
}

static const EventContextOutputAdapters g_eccCopyEnginePoisonUncontainedErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccCopyEnginePoisonUncontainedErrorEmitOsLogXid,
};

static void _eccDramErrorEmitOsLogXidCommon
(
    OBJGPU *pGpu,
    NvBool bSimulated,
    const char *pDramType,
    NvU32 fbpa,
    NvU32 sublocation,
    NvU64 physicalAddress,
    NvU32 encodedBankRemapRegVal
)
{
    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_GPU_ECC_DBE,
                     ECC_DRAM_DBE_XID_MESSAGE_FMT,
                     physicalAddress,
                     fbpa,
                     sublocation);
    if (encodedBankRemapRegVal != ECC_DRAM_BANK_REMAP_XID_VALUE_INVALID)
    {
        OP_EVENT_XID_LOG(pGpu, bSimulated, UNCORRECTABLE_DRAM_ERROR,
                         ECC_DRAM_ERROR_WITH_BANK_REMAP_XID_MESSAGE_FMT,
                         pDramType,
                         fbpa,
                         sublocation,
                         physicalAddress,
                         encodedBankRemapRegVal);
    }
    else
    {
        OP_EVENT_XID_LOG(pGpu, bSimulated, UNCORRECTABLE_DRAM_ERROR,
                         ECC_DRAM_ERROR_XID_MESSAGE_FMT,
                         pDramType,
                         fbpa,
                         sublocation,
                         physicalAddress);
    }
}

static int _eccDramErrorXidMessageSnprintf
(
    char *pMessage,
    NvU32 messageSize,
    const char *pDramType,
    NvU32 fbpa,
    NvU32 sublocation,
    NvU64 physicalAddress,
    NvU32 encodedBankRemapRegVal
)
{
    if (encodedBankRemapRegVal != ECC_DRAM_BANK_REMAP_XID_VALUE_INVALID)
    {
        return nvDbgSnprintf(pMessage, messageSize,
                             ECC_DRAM_ERROR_WITH_BANK_REMAP_XID_MESSAGE_FMT,
                             pDramType, fbpa, sublocation, physicalAddress, encodedBankRemapRegVal);
    }

    return nvDbgSnprintf(pMessage, messageSize,
                         ECC_DRAM_ERROR_XID_MESSAGE_FMT,
                         pDramType, fbpa, sublocation, physicalAddress);
}

static void _eccDramErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    DramErrorCtx *pContext = (DramErrorCtx *)pContextHeader;

    _eccDramErrorEmitOsLogXidCommon(pGpu,
                                    bSimulated,
                                    pContext->bHbm ? "HBM" : "GDDR",
                                    pContext->fbpa,
                                    pContext->sublocation,
                                    pContext->physicalAddress,
                                    pContext->encodedBankRemapRegVal);
}

static void _eccDramSidebandErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    DramErrorCtx *pContext = (DramErrorCtx *)pContextHeader;

    _eccDramErrorEmitOsLogXidCommon(pGpu,
                                    bSimulated,
                                    "Sideband",
                                    pContext->fbpa,
                                    pContext->sublocation,
                                    pContext->physicalAddress,
                                    pContext->encodedBankRemapRegVal);
}

static const EventContextOutputAdapters g_eccDramErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccDramErrorEmitOsLogXid,
};

static const EventContextOutputAdapters g_eccDramSidebandErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccDramSidebandErrorEmitOsLogXid,
};

static void _eccDramCbcFatalErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    DramCbcErrorCtx *pContext = (DramCbcErrorCtx *)pContextHeader;

    _eccDramErrorEmitOsLogXidCommon(pGpu,
                                    bSimulated,
                                    pContext->bHbm ? "HBM" : "GDDR",
                                    pContext->fbpaPart,
                                    pContext->fbpaSubloc,
                                    pContext->physicalAddress,
                                    ECC_DRAM_BANK_REMAP_XID_VALUE_INVALID);
    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_UNCONTAINED_ERROR,
                     ECC_DRAM_CBC_FATAL_ERROR_XID_MESSAGE_FMT,
                     pContext->ltcPart,
                     pContext->slice);
}

static const EventContextOutputAdapters g_eccDramCbcFatalErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccDramCbcFatalErrorEmitOsLogXid,
};

static void _eccDramCbcNonFatalErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    DramCbcErrorCtx *pContext = (DramCbcErrorCtx *)pContextHeader;

    _eccDramErrorEmitOsLogXidCommon(pGpu,
                                    bSimulated,
                                    pContext->bHbm ? "HBM" : "GDDR",
                                    pContext->fbpaPart,
                                    pContext->fbpaSubloc,
                                    pContext->physicalAddress,
                                    ECC_DRAM_BANK_REMAP_XID_VALUE_INVALID);
    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_CONTAINED_ERROR,
                     ECC_DRAM_CBC_NON_FATAL_ERROR_XID_MESSAGE_FMT,
                     pContext->ltcPart,
                     pContext->slice);
}

static const EventContextOutputAdapters g_eccDramCbcNonFatalErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccDramCbcNonFatalErrorEmitOsLogXid,
};

static void _eccDramEccIntrStormEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    DramEccIntrStormCtx *pContext = (DramEccIntrStormCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, EXCESSIVE_SBE_INTERRUPTS,
                 ECC_DRAM_ECC_INTR_STORM_XID_MESSAGE_FMT,
                 pContext->fbpaIndexVirt);
}

static const EventContextOutputAdapters g_eccDramEccIntrStormOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccDramEccIntrStormEmitOsLogXid,
};

static void _eccSmEccIntrStormEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    (void)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, EXCESSIVE_SBE_INTERRUPTS,
                 ECC_SM_ECC_INTR_STORM_XID_MESSAGE_FMT);
}

static const EventContextOutputAdapters g_eccSmEccIntrStormOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccSmEccIntrStormEmitOsLogXid,
};

static void _eccResidualUncorrectableErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    EccResidualUncorrectableErrorCtx *pContext =
        (EccResidualUncorrectableErrorCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, UNRECOVERABLE_ECC_ERROR_ESCAPE,
                 ECC_RESIDUAL_UNCORRECTABLE_ERROR_XID_MESSAGE_FMT,
                 pContext->dramCount,
                 pContext->ltcCount,
                 pContext->mmuCount,
                 pContext->pcieCount);
}

static const EventContextOutputAdapters g_eccResidualUncorrectableErrorOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx          = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid                   = _eccResidualUncorrectableErrorEmitOsLogXid,
};

static const EventContextOutputAdapters g_eccRowRemappingPageOfflineFailureOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccRowRemappingPageOfflineFailureEmitOsLogXid,
};

static const EventContextOutputAdapters g_eccXidOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuXidEventCtxSerializeToEventBufferGoeCtx,
    .serializeToProtobuf = gpuXidEventCtxSerializeToProtobuf,
    .serializeToCperEventCtx = gpuXidEventCtxSerializeToCperCtx,
    .emitToOsLogXid = NULL,
};

NV_STATUS rrpendingConstruct_IMPL
(
    RowRemapPending *pEvent,
    EventBus *pEventBus,
    NvU32 source,
    NvU64 physicalAddress
)
{
    pEvent->context.header.pOutputAdapters = &g_eccRowRemapPendingOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.physicalAddress) +
                                      sizeof(pEvent->context.source);
    pEvent->context.physicalAddress = physicalAddress;
    pEvent->context.source = source;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid63.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid63.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid63.data.message,
                      sizeof(pEvent->xid63.data.message),
                      pEvent->context.source == GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT ?
                          ECC_ROW_REMAP_PENDING_INTR_XID_MESSAGE_FMT :
                          ECC_ROW_REMAP_PENDING_CTRL_CALL_XID_MESSAGE_FMT,
                      pEvent->context.physicalAddress) + 1);
    pEvent->xid63.data.xidCode = INFOROM_DRAM_RETIREMENT_EVENT;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid63.header);

    return NV_OK;
}

NV_STATUS rrtablefullConstruct_IMPL
(
    RowRemappingFailureTableFull *pEvent,
    EventBus *pEventBus,
    NvU64 physicalAddress
)
{
    pEvent->context.header.pOutputAdapters = &g_eccRowRemappingFailureTableFullOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.physicalAddress);
    pEvent->context.physicalAddress = physicalAddress;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid64.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid64.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid64.data.message,
                      sizeof(pEvent->xid64.data.message),
                      ECC_ROW_REMAPPING_FAILURE_TABLE_FULL_XID_MESSAGE_FMT,
                      pEvent->context.physicalAddress) + 1);
    pEvent->xid64.data.xidCode = INFOROM_DRAM_RETIREMENT_FAILURE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid64.header);

    return NV_OK;
}

NV_STATUS rrbankfullConstruct_IMPL
(
    RowRemappingFailureBankFull *pEvent,
    EventBus *pEventBus,
    NvU64 physicalAddress
)
{
    pEvent->context.header.pOutputAdapters = &g_eccRowRemappingFailureBankFullOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.physicalAddress);
    pEvent->context.physicalAddress = physicalAddress;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid64.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid64.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid64.data.message,
                      sizeof(pEvent->xid64.data.message),
                      ECC_ROW_REMAPPING_FAILURE_BANK_FULL_XID_MESSAGE_FMT,
                      pEvent->context.physicalAddress) + 1);
    pEvent->xid64.data.xidCode = INFOROM_DRAM_RETIREMENT_FAILURE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid64.header);

    return NV_OK;
}

NV_STATUS rrinternalerrConstruct_IMPL
(
    RowRemappingFailureInternalError *pEvent,
    EventBus *pEventBus,
    NvU64 physicalAddress,
    const char *pErrorString
)
{
    NvLength errorStringLength;

    pEvent->context.header.pOutputAdapters = &g_eccRowRemappingFailureInternalErrorOutputAdapters;
    pEvent->context.physicalAddress = physicalAddress;
    errorStringLength = portStringCopy(pEvent->context.errorString,
                                       sizeof(pEvent->context.errorString),
                                       (pErrorString != NULL) ? pErrorString : "unknown",
                                       sizeof(pEvent->context.errorString));
    pEvent->context.header.dataSize = (NvU32)(sizeof(pEvent->context.physicalAddress) +
                                              errorStringLength);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid64.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid64.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid64.data.message,
                      sizeof(pEvent->xid64.data.message),
                      ECC_ROW_REMAPPING_FAILURE_INTERNAL_ERROR_XID_MESSAGE_FMT,
                      pEvent->context.physicalAddress,
                      pEvent->context.errorString) + 1);
    pEvent->xid64.data.xidCode = INFOROM_DRAM_RETIREMENT_FAILURE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid64.header);

    return NV_OK;
}

NV_STATUS drinternalerrConstruct_IMPL
(
    DramRetirementFailureInternalError *pEvent,
    EventBus *pEventBus,
    NvU64 physicalAddress,
    ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON errorReason
)
{
    pEvent->context.header.pOutputAdapters = &g_eccDramRetirementFailureInternalErrorOutputAdapters;
    pEvent->context.physicalAddress = physicalAddress;
    pEvent->context.errorReason = errorReason;
    pEvent->context.header.dataSize = sizeof(pEvent->context.physicalAddress) +
                                      sizeof(pEvent->context.errorReason);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid64.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid64.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid64.data.message,
                      sizeof(pEvent->xid64.data.message),
                      _eccDramRetirementFailureInternalErrorXidMessageFmt(errorReason),
                      pEvent->context.physicalAddress) + 1);
    pEvent->xid64.data.xidCode = INFOROM_DRAM_RETIREMENT_FAILURE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid64.header);

    return NV_OK;
}

NV_STATUS drinforomfullConstruct_IMPL
(
    DramRetirementFailureInforomFull *pEvent,
    EventBus *pEventBus,
    NvU64 physicalAddress
)
{
    pEvent->context.header.pOutputAdapters = &g_eccDramRetirementInforomFullOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.physicalAddress);
    pEvent->context.physicalAddress = physicalAddress;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid64.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid64.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid64.data.message,
                      sizeof(pEvent->xid64.data.message),
                      ECC_DRAM_RETIREMENT_FAILURE_INFOROM_FULL_XID_MESSAGE_FMT,
                      pEvent->context.physicalAddress) + 1);
    pEvent->xid64.data.xidCode = INFOROM_DRAM_RETIREMENT_FAILURE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid64.header);

    return NV_OK;
}

NV_STATUS drhwlimitConstruct_IMPL
(
    DramRetirementFailureHwLimit *pEvent,
    EventBus *pEventBus,
    NvU64 physicalAddress
)
{
    pEvent->context.header.pOutputAdapters = &g_eccDramRetirementHwLimitOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.physicalAddress);
    pEvent->context.physicalAddress = physicalAddress;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid64.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid64.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid64.data.message,
                      sizeof(pEvent->xid64.data.message),
                      ECC_DRAM_RETIREMENT_FAILURE_HW_LIMIT_XID_MESSAGE_FMT,
                      pEvent->context.physicalAddress) + 1);
    pEvent->xid64.data.xidCode = INFOROM_DRAM_RETIREMENT_FAILURE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid64.header);

    return NV_OK;
}

NV_STATUS drnospareConstruct_IMPL
(
    DramRetirementFailureNoSpare *pEvent,
    EventBus *pEventBus,
    NvU64 physicalAddress
)
{
    pEvent->context.header.pOutputAdapters = &g_eccDramRetirementNoSpareOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.physicalAddress);
    pEvent->context.physicalAddress = physicalAddress;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid64.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid64.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid64.data.message,
                      sizeof(pEvent->xid64.data.message),
                      ECC_DRAM_RETIREMENT_FAILURE_NO_SPARE_XID_MESSAGE_FMT,
                      pEvent->context.physicalAddress) + 1);
    pEvent->xid64.data.xidCode = INFOROM_DRAM_RETIREMENT_FAILURE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid64.header);

    return NV_OK;
}

NV_STATUS rrreservedrowConstruct_IMPL
(
    RowRemappingFailureReservedRow *pEvent,
    EventBus *pEventBus,
    NvU64 physicalAddress
)
{
    pEvent->context.header.pOutputAdapters = &g_eccRowRemappingFailureReservedRowOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.physicalAddress);
    pEvent->context.physicalAddress = physicalAddress;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid64.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid64.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid64.data.message,
                      sizeof(pEvent->xid64.data.message),
                      ECC_ROW_REMAPPING_FAILURE_RESERVED_ROW_ERROR_XID_MESSAGE_FMT,
                      pEvent->context.physicalAddress) + 1);
    pEvent->xid64.data.xidCode = INFOROM_DRAM_RETIREMENT_FAILURE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid64.header);

    return NV_OK;
}

NV_STATUS rrpageofflinefailureConstruct_IMPL
(
    RowRemappingPageOfflineFailure *pEvent,
    EventBus *pEventBus,
    NvU64 physicalAddress
)
{
    pEvent->context.header.pOutputAdapters = &g_eccRowRemappingPageOfflineFailureOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.physicalAddress);
    pEvent->context.physicalAddress = physicalAddress;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid64.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid64.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid64.data.message,
                      sizeof(pEvent->xid64.data.message),
                      ECC_ROW_REMAPPING_FAILURE_PAGE_OFFLINE_XID_MESSAGE_FMT,
                      pEvent->context.physicalAddress) + 1);
    pEvent->xid64.data.xidCode = INFOROM_DRAM_RETIREMENT_FAILURE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid64.header);

    return NV_OK;
}

NV_STATUS brpendingConstruct_IMPL
(
    BankRemappingPending *pEvent,
    EventBus *pEventBus,
    NvU32 source
)
{
    pEvent->context.header.pOutputAdapters = &g_eccBankRemappingPendingOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.source);
    pEvent->context.source = source;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid177.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid177.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid177.data.message,
                      sizeof(pEvent->xid177.data.message),
                      ECC_BANK_REMAP_PENDING_XID_MESSAGE_FMT) + 1);
    pEvent->xid177.data.xidCode = BANK_REMAPPING_EVENT;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid177.header);

    return NV_OK;
}

NV_STATUS ltsrepairpendingConstruct_IMPL
(
    LtsRepairPending *pEvent,
    EventBus *pEventBus,
    NvU32 sublocation,
    NvU32 location,
    NV2080_CTRL_GPU_RECOVERY_ACTION recoveryAction,
    const char *pErrorString
)
{
    NvLength errorStringLength;

    pEvent->context.header.pOutputAdapters = &g_eccLtsRepairPendingOutputAdapters;
    pEvent->context.location = location;
    pEvent->context.sublocation = sublocation;
    pEvent->context.recoveryAction = recoveryAction;
    errorStringLength = portStringCopy(pEvent->context.errorString,
                                       sizeof(pEvent->context.errorString),
                                       (pErrorString != NULL) ? pErrorString : "unknown",
                                       sizeof(pEvent->context.errorString));
    pEvent->context.header.dataSize = (NvU32)(sizeof(pEvent->context.location) +
                                              sizeof(pEvent->context.sublocation) +
                                              sizeof(pEvent->context.recoveryAction) +
                                              errorStringLength);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid160.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid160.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid160.data.message,
                      sizeof(pEvent->xid160.data.message),
                      ECC_LTS_REPAIR_PENDING_XID_MESSAGE_FMT,
                      pEvent->context.sublocation,
                      pEvent->context.location,
                      pEvent->context.errorString) + 1);
    pEvent->xid160.data.xidCode = CHANNEL_RETIREMENT_EVENT;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid160.header);

    return NV_OK;
}

NV_STATUS memorychannelrepairpendingConstruct_IMPL
(
    MemoryChannelRepairPending *pEvent,
    EventBus *pEventBus,
    NvU32 sublocation,
    NvU32 location,
    NV2080_CTRL_GPU_RECOVERY_ACTION recoveryAction,
    const char *pErrorString
)
{
    NvLength errorStringLength;

    pEvent->context.header.pOutputAdapters = &g_eccChannelRepairPendingOutputAdapters;
    pEvent->context.location = location;
    pEvent->context.sublocation = sublocation;
    pEvent->context.recoveryAction = recoveryAction;
    errorStringLength = portStringCopy(pEvent->context.errorString,
                                       sizeof(pEvent->context.errorString),
                                       (pErrorString != NULL) ? pErrorString : "unknown",
                                       sizeof(pEvent->context.errorString));
    pEvent->context.header.dataSize = (NvU32)(sizeof(pEvent->context.location) +
                                              sizeof(pEvent->context.sublocation) +
                                              sizeof(pEvent->context.recoveryAction) +
                                              errorStringLength);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid160.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid160.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid160.data.message,
                      sizeof(pEvent->xid160.data.message),
                      ECC_CHANNEL_REPAIR_PENDING_XID_MESSAGE_FMT,
                      pEvent->context.sublocation,
                      pEvent->context.location,
                      pEvent->context.errorString) + 1);
    pEvent->xid160.data.xidCode = CHANNEL_RETIREMENT_EVENT;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid160.header);

    return NV_OK;
}

NV_STATUS ltsrepairfailureConstruct_IMPL
(
    LtsRepairFailure *pEvent,
    EventBus *pEventBus
)
{
    pEvent->context.header.pOutputAdapters = &g_eccLtsRepairFailureOutputAdapters;
    pEvent->context.header.dataSize = 0;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid161.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid161.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid161.data.message,
                      sizeof(pEvent->xid161.data.message),
                      ECC_LTS_REPAIR_FAILURE_XID_MESSAGE_FMT) + 1);
    pEvent->xid161.data.xidCode = CHANNEL_RETIREMENT_FAILURE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid161.header);

    return NV_OK;
}

NV_STATUS memorychannelrepairfailureConstruct_IMPL
(
    MemoryChannelRepairFailure *pEvent,
    EventBus *pEventBus
)
{
    pEvent->context.header.pOutputAdapters = &g_eccChannelRepairFailureOutputAdapters;
    pEvent->context.header.dataSize = 0;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid161.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid161.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid161.data.message,
                      sizeof(pEvent->xid161.data.message),
                      ECC_CHANNEL_REPAIR_FAILURE_XID_MESSAGE_FMT) + 1);
    pEvent->xid161.data.xidCode = CHANNEL_RETIREMENT_FAILURE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid161.header);

    return NV_OK;
}

NV_STATUS sparingrepairpendingConstruct_IMPL
(
    SparingRepairPending *pEvent,
    EventBus *pEventBus
)
{
    pEvent->xid156.header.pOutputAdapters = &g_eccSparingXidOutputAdapters;
    pEvent->xid156.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid156.data.message,
                      sizeof(pEvent->xid156.data.message),
                      ECC_SPARING_REPAIR_PENDING_XID_MESSAGE_FMT) + 1);
    pEvent->xid156.data.xidCode = RESOURCE_RETIREMENT_EVENT;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid156.header);

    return NV_OK;
}

NV_STATUS sparingrepairnotpossibleConstruct_IMPL
(
    SparingRepairNotPossible *pEvent,
    EventBus *pEventBus
)
{
    pEvent->xid157.header.pOutputAdapters = &g_eccSparingXidOutputAdapters;
    pEvent->xid157.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid157.data.message,
                      sizeof(pEvent->xid157.data.message),
                      ECC_SPARING_REPAIR_NOT_POSSIBLE_XID_MESSAGE_FMT) + 1);
    pEvent->xid157.data.xidCode = RESOURCE_RETIREMENT_FAILURE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid157.header);

    return NV_OK;
}

NV_STATUS sparingeventConstruct_IMPL
(
    SparingEvent *pEvent,
    EventBus *pEventBus
)
{
    pEvent->xid184.header.pOutputAdapters = &g_eccSparingXidOutputAdapters;
    pEvent->xid184.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid184.data.message,
                      sizeof(pEvent->xid184.data.message),
                      ECC_SPARING_EVENT_XID_MESSAGE_FMT) + 1);
    pEvent->xid184.data.xidCode = SPARING_EVENT;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid184.header);

    return NV_OK;
}

NV_STATUS ltcfatalerrConstruct_IMPL
(
    LtcFatalError *pEvent,
    EventBus *pEventBus,
    NV_ERROR_CONT_LOCATION loc,
    L2_ERROR_TYPE errorType
)
{
    NV_ASSERT_OR_RETURN((loc.locType == NV_ERROR_CONT_LOCATION_TYPE_LTC) ||
                        (loc.locType == NV_ERROR_CONT_LOCATION_TYPE_SYSLTC),
                        NV_ERR_INVALID_ARGUMENT);

    pEvent->context.header.pOutputAdapters = &g_eccLtcFatalErrorOutputAdapters;
    pEvent->context.header.dataSize =
        sizeof(pEvent->context) - sizeof(pEvent->context.header);
    pEvent->context.errorType = errorType;

    if (loc.locType == NV_ERROR_CONT_LOCATION_TYPE_SYSLTC)
    {
        pEvent->context.dieletType = loc.locInfo.sysLtcLoc.dieletType;
        pEvent->context.dieletId = loc.locInfo.sysLtcLoc.dieletId;
        pEvent->context.ltc = loc.locInfo.sysLtcLoc.instanceId;
        pEvent->context.slice = loc.locInfo.sysLtcLoc.slice;
        pEvent->context.bvidL2 = NV_FALSE;
    }
    else
    {
        pEvent->context.dieletType = 0;
        pEvent->context.dieletId = 0;
        pEvent->context.ltc = loc.locInfo.ltcLoc.partition;
        pEvent->context.slice = loc.locInfo.ltcLoc.slice;
        pEvent->context.bvidL2 = NV_TRUE;
    }

    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid95.header.pOutputAdapters = &g_eccXidOutputAdapters;
    if (!pEvent->context.bvidL2)
    {
        pEvent->xid95.header.dataSize =
            (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
                nvDbgSnprintf(pEvent->xid95.data.message,
                              sizeof(pEvent->xid95.data.message),
                              ECC_SYSLTC_FATAL_ERROR_XID_MESSAGE_FMT,
                              _eccLtcFatalErrorXidErrorString(pEvent->context.bvidL2,
                                                              pEvent->context.errorType),
                              pEvent->context.dieletType,
                              pEvent->context.dieletId,
                              pEvent->context.ltc,
                              pEvent->context.slice) + 1);
    }
    else
    {
        pEvent->xid95.header.dataSize =
            (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
                nvDbgSnprintf(pEvent->xid95.data.message,
                              sizeof(pEvent->xid95.data.message),
                              ECC_LTC_FATAL_ERROR_XID_MESSAGE_FMT,
                              _eccLtcFatalErrorXidErrorString(pEvent->context.bvidL2,
                                                              pEvent->context.errorType),
                              pEvent->context.ltc,
                              pEvent->context.slice) + 1);
    }
    pEvent->xid95.data.xidCode = ROBUST_CHANNEL_UNCONTAINED_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid95.header);

    return NV_OK;
}

NV_STATUS ltcerrConstruct_IMPL
(
    LtcError *pEvent,
    EventBus *pEventBus,
    NV_ERROR_CONT_LOCATION loc
)
{
    NV_ASSERT_OR_RETURN((loc.locType == NV_ERROR_CONT_LOCATION_TYPE_LTC) ||
                        (loc.locType == NV_ERROR_CONT_LOCATION_TYPE_SYSLTC),
                        NV_ERR_INVALID_ARGUMENT);

    pEvent->context.header.pOutputAdapters = &g_eccLtcErrorOutputAdapters;
    pEvent->context.header.dataSize =
        sizeof(pEvent->context) - sizeof(pEvent->context.header);
    pEvent->context.errorType = L2_ERROR_TYPE_UNKNOWN;

    if (loc.locType == NV_ERROR_CONT_LOCATION_TYPE_SYSLTC)
    {
        pEvent->context.dieletType = loc.locInfo.sysLtcLoc.dieletType;
        pEvent->context.dieletId = loc.locInfo.sysLtcLoc.dieletId;
        pEvent->context.ltc = loc.locInfo.sysLtcLoc.instanceId;
        pEvent->context.slice = loc.locInfo.sysLtcLoc.slice;
        pEvent->context.bvidL2 = NV_FALSE;
    }
    else
    {
        pEvent->context.dieletType = 0;
        pEvent->context.dieletId = 0;
        pEvent->context.ltc = loc.locInfo.ltcLoc.partition;
        pEvent->context.slice = loc.locInfo.ltcLoc.slice;
        pEvent->context.bvidL2 = NV_TRUE;
    }

    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid48.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid172.header.pOutputAdapters = &g_eccXidOutputAdapters;

    if (!pEvent->context.bvidL2)
    {
        pEvent->xid48.header.dataSize =
            (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
                nvDbgSnprintf(pEvent->xid48.data.message,
                              sizeof(pEvent->xid48.data.message),
                              ECC_SYSLTC_DBE_XID_MESSAGE_FMT,
                              pEvent->context.dieletType,
                              pEvent->context.dieletId,
                              pEvent->context.ltc,
                              pEvent->context.slice) + 1);
        pEvent->xid172.header.dataSize =
            (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
                nvDbgSnprintf(pEvent->xid172.data.message,
                              sizeof(pEvent->xid172.data.message),
                              ECC_SYSLTC_SRAM_ERROR_XID_MESSAGE_FMT,
                              pEvent->context.dieletType,
                              pEvent->context.dieletId,
                              pEvent->context.ltc,
                              pEvent->context.slice) + 1);
    }
    else
    {
        pEvent->xid48.header.dataSize =
            (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
                nvDbgSnprintf(pEvent->xid48.data.message,
                              sizeof(pEvent->xid48.data.message),
                              ECC_LTC_DBE_XID_MESSAGE_FMT,
                              pEvent->context.ltc,
                              pEvent->context.slice) + 1);
        pEvent->xid172.header.dataSize =
            (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
                nvDbgSnprintf(pEvent->xid172.data.message,
                              sizeof(pEvent->xid172.data.message),
                              ECC_LTC_SRAM_ERROR_XID_MESSAGE_FMT,
                              pEvent->context.ltc,
                              pEvent->context.slice) + 1);
    }

    pEvent->xid48.data.xidCode = ROBUST_CHANNEL_GPU_ECC_DBE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid48.header);

    pEvent->xid172.data.xidCode = UNCORRECTABLE_SRAM_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid172.header);

    return NV_OK;
}

NV_STATUS dramerrConstruct_IMPL
(
    DramError *pEvent,
    EventBus *pEventBus,
    NvU32 fbpa,
    NvU32 sublocation,
    NvU64 physicalAddress,
    NvBool bHbm,
    NvU32 encodedBankRemapRegVal
)
{
    pEvent->context.header.pOutputAdapters = &g_eccDramErrorOutputAdapters;
    pEvent->context.header.dataSize =
        sizeof(pEvent->context) - sizeof(pEvent->context.header);
    pEvent->context.fbpa = fbpa;
    pEvent->context.sublocation = sublocation;
    pEvent->context.bHbm = bHbm;
    pEvent->context.encodedBankRemapRegVal = encodedBankRemapRegVal;
    pEvent->context.physicalAddress = physicalAddress;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid48.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid48.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid48.data.message,
                          sizeof(pEvent->xid48.data.message),
                          ECC_DRAM_DBE_XID_MESSAGE_FMT,
                          pEvent->context.physicalAddress,
                          pEvent->context.fbpa,
                          pEvent->context.sublocation) + 1);
    pEvent->xid48.data.xidCode = ROBUST_CHANNEL_GPU_ECC_DBE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid48.header);

    pEvent->xid171.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid171.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            _eccDramErrorXidMessageSnprintf(pEvent->xid171.data.message,
                                            sizeof(pEvent->xid171.data.message),
                                            bHbm ? "HBM" : "GDDR",
                                            pEvent->context.fbpa,
                                            pEvent->context.sublocation,
                                            pEvent->context.physicalAddress,
                                            pEvent->context.encodedBankRemapRegVal) + 1);
    pEvent->xid171.data.xidCode = UNCORRECTABLE_DRAM_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid171.header);

    return NV_OK;
}

NV_STATUS dramsidebanderrConstruct_IMPL
(
    DramSidebandError *pEvent,
    EventBus *pEventBus,
    NvU32 fbpa,
    NvU32 sublocation,
    NvU64 physicalAddress,
    NvU32 encodedBankRemapRegVal
)
{
    pEvent->context.header.pOutputAdapters = &g_eccDramSidebandErrorOutputAdapters;
    pEvent->context.header.dataSize =
        sizeof(pEvent->context) - sizeof(pEvent->context.header);
    pEvent->context.fbpa = fbpa;
    pEvent->context.sublocation = sublocation;
    pEvent->context.bHbm = NV_FALSE;
    pEvent->context.encodedBankRemapRegVal = encodedBankRemapRegVal;
    pEvent->context.physicalAddress = physicalAddress;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid48.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid48.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid48.data.message,
                          sizeof(pEvent->xid48.data.message),
                          ECC_DRAM_DBE_XID_MESSAGE_FMT,
                          pEvent->context.physicalAddress,
                          pEvent->context.fbpa,
                          pEvent->context.sublocation) + 1);
    pEvent->xid48.data.xidCode = ROBUST_CHANNEL_GPU_ECC_DBE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid48.header);

    pEvent->xid171.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid171.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            _eccDramErrorXidMessageSnprintf(pEvent->xid171.data.message,
                                            sizeof(pEvent->xid171.data.message),
                                            "Sideband",
                                            pEvent->context.fbpa,
                                            pEvent->context.sublocation,
                                            pEvent->context.physicalAddress,
                                            pEvent->context.encodedBankRemapRegVal) + 1);
    pEvent->xid171.data.xidCode = UNCORRECTABLE_DRAM_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid171.header);

    return NV_OK;
}

NV_STATUS dramcbcfatalerrConstruct_IMPL
(
    DramCbcFatalError *pEvent,
    EventBus *pEventBus,
    NV_ERROR_CONT_LOCATION loc,
    NvBool bHbm
)
{
    NV_ASSERT_OR_RETURN(loc.locType == NV_ERROR_CONT_LOCATION_TYPE_CBC,
                        NV_ERR_INVALID_ARGUMENT);

    pEvent->context.header.pOutputAdapters = &g_eccDramCbcFatalErrorOutputAdapters;
    pEvent->context.header.dataSize =
        sizeof(pEvent->context) - sizeof(pEvent->context.header);
    pEvent->context.ltcPart = loc.locInfo.cbcLoc.ltcPart;
    pEvent->context.slice = loc.locInfo.cbcLoc.slice;
    pEvent->context.fbpaPart = loc.locInfo.cbcLoc.fbpaPart;
    pEvent->context.fbpaSubloc = loc.locInfo.cbcLoc.fbpaSubloc;
    pEvent->context.bHbm = bHbm;
    pEvent->context.physicalAddress = loc.locInfo.cbcLoc.physicalAddress;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid48.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid48.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid48.data.message,
                          sizeof(pEvent->xid48.data.message),
                          ECC_DRAM_DBE_XID_MESSAGE_FMT,
                          pEvent->context.physicalAddress,
                          pEvent->context.fbpaPart,
                          pEvent->context.fbpaSubloc) + 1);
    pEvent->xid48.data.xidCode = ROBUST_CHANNEL_GPU_ECC_DBE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid48.header);

    pEvent->xid171.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid171.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            _eccDramErrorXidMessageSnprintf(pEvent->xid171.data.message,
                                            sizeof(pEvent->xid171.data.message),
                                            pEvent->context.bHbm ? "HBM" : "GDDR",
                                            pEvent->context.fbpaPart,
                                            pEvent->context.fbpaSubloc,
                                            pEvent->context.physicalAddress,
                                            ECC_DRAM_BANK_REMAP_XID_VALUE_INVALID) + 1);
    pEvent->xid171.data.xidCode = UNCORRECTABLE_DRAM_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid171.header);

    pEvent->xid95.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid95.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid95.data.message,
                          sizeof(pEvent->xid95.data.message),
                          ECC_DRAM_CBC_FATAL_ERROR_XID_MESSAGE_FMT,
                          pEvent->context.ltcPart,
                          pEvent->context.slice) + 1);
    pEvent->xid95.data.xidCode = ROBUST_CHANNEL_UNCONTAINED_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid95.header);

    return NV_OK;
}

NV_STATUS dramcbcnonfatalerrConstruct_IMPL
(
    DramCbcNonFatalError *pEvent,
    EventBus *pEventBus,
    NV_ERROR_CONT_LOCATION loc,
    NvBool bHbm
)
{
    NV_ASSERT_OR_RETURN(loc.locType == NV_ERROR_CONT_LOCATION_TYPE_CBC,
                        NV_ERR_INVALID_ARGUMENT);

    pEvent->context.header.pOutputAdapters = &g_eccDramCbcNonFatalErrorOutputAdapters;
    pEvent->context.header.dataSize =
        sizeof(pEvent->context) - sizeof(pEvent->context.header);
    pEvent->context.ltcPart = loc.locInfo.cbcLoc.ltcPart;
    pEvent->context.slice = loc.locInfo.cbcLoc.slice;
    pEvent->context.fbpaPart = loc.locInfo.cbcLoc.fbpaPart;
    pEvent->context.fbpaSubloc = loc.locInfo.cbcLoc.fbpaSubloc;
    pEvent->context.bHbm = bHbm;
    pEvent->context.physicalAddress = loc.locInfo.cbcLoc.physicalAddress;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid48.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid48.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid48.data.message,
                          sizeof(pEvent->xid48.data.message),
                          ECC_DRAM_DBE_XID_MESSAGE_FMT,
                          pEvent->context.physicalAddress,
                          pEvent->context.fbpaPart,
                          pEvent->context.fbpaSubloc) + 1);
    pEvent->xid48.data.xidCode = ROBUST_CHANNEL_GPU_ECC_DBE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid48.header);

    pEvent->xid171.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid171.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            _eccDramErrorXidMessageSnprintf(pEvent->xid171.data.message,
                                            sizeof(pEvent->xid171.data.message),
                                            pEvent->context.bHbm ? "HBM" : "GDDR",
                                            pEvent->context.fbpaPart,
                                            pEvent->context.fbpaSubloc,
                                            pEvent->context.physicalAddress,
                                            ECC_DRAM_BANK_REMAP_XID_VALUE_INVALID) + 1);
    pEvent->xid171.data.xidCode = UNCORRECTABLE_DRAM_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid171.header);

    pEvent->xid94.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid94.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid94.data.message,
                          sizeof(pEvent->xid94.data.message),
                          ECC_DRAM_CBC_NON_FATAL_ERROR_XID_MESSAGE_FMT,
                          pEvent->context.ltcPart,
                          pEvent->context.slice) + 1);
    pEvent->xid94.data.xidCode = ROBUST_CHANNEL_CONTAINED_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid94.header);

    return NV_OK;
}

NV_STATUS ltcclientpoisonuncontainederrConstruct_IMPL
(
    LtcUnsupportedClientPoisonUncontainedError *pEvent,
    EventBus *pEventBus,
    NV_ERROR_CONT_LOCATION loc
)
{
    NV_ASSERT_OR_RETURN(loc.locType == NV_ERROR_CONT_LOCATION_TYPE_LTC,
                        NV_ERR_INVALID_ARGUMENT);

    pEvent->context.header.pOutputAdapters =
        &g_eccLtcUnsupportedClientPoisonUncontainedErrorOutputAdapters;
    pEvent->context.header.dataSize =
        sizeof(pEvent->context) - sizeof(pEvent->context.header);
    pEvent->context.partition = loc.locInfo.ltcLoc.partition;
    pEvent->context.slice = loc.locInfo.ltcLoc.slice;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid95.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid95.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid95.data.message,
                          sizeof(pEvent->xid95.data.message),
                          ECC_LTC_UNSUPPORTED_CLIENT_POISON_UNCONTAINED_ERROR_XID_MESSAGE_FMT,
                          pEvent->context.partition,
                          pEvent->context.slice) + 1);
    pEvent->xid95.data.xidCode = ROBUST_CHANNEL_UNCONTAINED_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid95.header);

    return NV_OK;
}

NV_STATUS ltcclientpoisoncontainederrConstruct_IMPL
(
    LtcUnsupportedClientPoisonContainedError *pEvent,
    EventBus *pEventBus,
    NV_ERROR_CONT_LOCATION loc
)
{
    NV_ASSERT_OR_RETURN(loc.locType == NV_ERROR_CONT_LOCATION_TYPE_LTC,
                        NV_ERR_INVALID_ARGUMENT);

    pEvent->context.header.pOutputAdapters =
        &g_eccLtcUnsupportedClientPoisonContainedErrorOutputAdapters;
    pEvent->context.header.dataSize =
        sizeof(pEvent->context) - sizeof(pEvent->context.header);
    pEvent->context.partition = loc.locInfo.ltcLoc.partition;
    pEvent->context.slice = loc.locInfo.ltcLoc.slice;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid94.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid94.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid94.data.message,
                          sizeof(pEvent->xid94.data.message),
                          ECC_LTC_UNSUPPORTED_CLIENT_POISON_CONTAINED_ERROR_XID_MESSAGE_FMT,
                          pEvent->context.partition,
                          pEvent->context.slice) + 1);
    pEvent->xid94.data.xidCode = ROBUST_CHANNEL_CONTAINED_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid94.header);

    return NV_OK;
}

NV_STATUS lrcfatalerrConstruct_IMPL
(
    LrcFatalError *pEvent,
    EventBus *pEventBus,
    NvU32 partition,
    NvU32 slice
)
{
    pEvent->context.header.pOutputAdapters = &g_eccLrcFatalErrorOutputAdapters;
    pEvent->context.header.dataSize =
        sizeof(pEvent->context) - sizeof(pEvent->context.header);
    pEvent->context.partition = partition;
    pEvent->context.slice = slice;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid48.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid48.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid48.data.message,
                          sizeof(pEvent->xid48.data.message),
                          ECC_LRC_DBE_XID_MESSAGE_FMT,
                          pEvent->context.slice,
                          pEvent->context.partition) + 1);
    pEvent->xid48.data.xidCode = ROBUST_CHANNEL_GPU_ECC_DBE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid48.header);

    pEvent->xid172.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid172.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid172.data.message,
                          sizeof(pEvent->xid172.data.message),
                          ECC_LRC_SRAM_ERROR_XID_MESSAGE_FMT,
                          pEvent->context.slice,
                          pEvent->context.partition) + 1);
    pEvent->xid172.data.xidCode = UNCORRECTABLE_SRAM_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid172.header);

    pEvent->xid95.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid95.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid95.data.message,
                          sizeof(pEvent->xid95.data.message),
                          ECC_LRC_UNCONTAINED_XID_MESSAGE_FMT,
                          pEvent->context.partition,
                          pEvent->context.slice) + 1);
    pEvent->xid95.data.xidCode = ROBUST_CHANNEL_UNCONTAINED_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid95.header);

    return NV_OK;
}

NV_STATUS pcieingresspoisoncontainederrConstruct_IMPL
(
    PcieIngressPoisonContainedError *pEvent,
    EventBus *pEventBus,
    NvU32 gfid
)
{
    pEvent->context.header.pOutputAdapters =
        &g_eccPcieIngressPoisonContainedErrorOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.gfid);
    pEvent->context.gfid = gfid;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid94.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid94.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid94.data.message,
                          sizeof(pEvent->xid94.data.message),
                          ECC_PCIE_INGRESS_POISON_CONTAINED_ERROR_XID_MESSAGE_FMT,
                          pEvent->context.gfid) + 1);
    pEvent->xid94.data.xidCode = ROBUST_CHANNEL_CONTAINED_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid94.header);

    return NV_OK;
}

NV_STATUS pcieingresspoisonuncontainederrConstruct_IMPL
(
    PcieIngressPoisonUncontainedError *pEvent,
    EventBus *pEventBus
)
{
    pEvent->context.header.pOutputAdapters =
        &g_eccPcieIngressPoisonUncontainedErrorOutputAdapters;
    pEvent->context.header.dataSize = 0;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid95.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid95.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid95.data.message,
                          sizeof(pEvent->xid95.data.message),
                          ECC_PCIE_INGRESS_POISON_UNCONTAINED_ERROR_XID_MESSAGE_FMT) + 1);
    pEvent->xid95.data.xidCode = ROBUST_CHANNEL_UNCONTAINED_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid95.header);

    return NV_OK;
}

NV_STATUS pcieegresspoisonerrConstruct_IMPL
(
    PcieEgressPoisonError *pEvent,
    EventBus *pEventBus
)
{
    pEvent->context.header.pOutputAdapters = &g_eccPcieEgressPoisonErrorOutputAdapters;
    pEvent->context.header.dataSize = 0;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid95.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid95.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid95.data.message,
                          sizeof(pEvent->xid95.data.message),
                          ECC_PCIE_EGRESS_POISON_ERROR_XID_MESSAGE_FMT) + 1);
    pEvent->xid95.data.xidCode = ROBUST_CHANNEL_UNCONTAINED_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid95.header);

    return NV_OK;
}

NV_STATUS fbhubfatalpoisonerrConstruct_IMPL
(
    FbHubFatalPoisonError *pEvent,
    EventBus *pEventBus
)
{
    pEvent->context.header.pOutputAdapters =
        &g_eccFbHubFatalPoisonErrorOutputAdapters;
    pEvent->context.header.dataSize = 0;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid95.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid95.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid95.data.message,
                          sizeof(pEvent->xid95.data.message),
                          ECC_FBHUB_FATAL_POISON_ERROR_XID_MESSAGE_FMT) + 1);
    pEvent->xid95.data.xidCode = ROBUST_CHANNEL_UNCONTAINED_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid95.header);

    return NV_OK;
}

NV_STATUS fbhubnonfatalpoisonerrConstruct_IMPL
(
    FbHubNonFatalPoisonError *pEvent,
    EventBus *pEventBus
)
{
    pEvent->context.header.pOutputAdapters =
        &g_eccFbHubNonFatalPoisonErrorOutputAdapters;
    pEvent->context.header.dataSize = 0;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid95.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid95.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid95.data.message,
                          sizeof(pEvent->xid95.data.message),
                          ECC_FBHUB_NON_FATAL_POISON_ERROR_XID_MESSAGE_FMT) + 1);
    pEvent->xid95.data.xidCode = ROBUST_CHANNEL_UNCONTAINED_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid95.header);

    return NV_OK;
}

NV_STATUS mmupoisonerrConstruct_IMPL
(
    MmuPoisonError *pEvent,
    EventBus *pEventBus
)
{
    pEvent->context.header.pOutputAdapters = &g_eccMmuPoisonErrorOutputAdapters;
    pEvent->context.header.dataSize = 0;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid94.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid94.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid94.data.message,
                          sizeof(pEvent->xid94.data.message),
                          ECC_MMU_POISON_ERROR_XID_MESSAGE_FMT) + 1);
    pEvent->xid94.data.xidCode = ROBUST_CHANNEL_CONTAINED_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid94.header);

    return NV_OK;
}

NV_STATUS smpoisonerrConstruct_IMPL
(
    SmPoisonError *pEvent,
    EventBus *pEventBus,
    NvU32 nv2080EngineType
)
{
    pEvent->context.header.pOutputAdapters = &g_eccSmPoisonErrorOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.nv2080EngineType);
    pEvent->context.nv2080EngineType = nv2080EngineType;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid94.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid94.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid94.data.message,
                          sizeof(pEvent->xid94.data.message),
                          ECC_SM_POISON_ERROR_XID_MESSAGE_FMT,
                          pEvent->context.nv2080EngineType) + 1);
    pEvent->xid94.data.xidCode = ROBUST_CHANNEL_CONTAINED_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid94.header);

    return NV_OK;
}

NV_STATUS ctxswpoisonerrConstruct_IMPL
(
    ContextSwitchPoisonError *pEvent,
    EventBus *pEventBus,
    NvU32 nv2080EngineType
)
{
    pEvent->context.header.pOutputAdapters =
        &g_eccContextSwitchPoisonErrorOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.nv2080EngineType);
    pEvent->context.nv2080EngineType = nv2080EngineType;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid94.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid94.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid94.data.message,
                          sizeof(pEvent->xid94.data.message),
                          ECC_CONTEXT_SWITCH_POISON_ERROR_XID_MESSAGE_FMT,
                          pEvent->context.nv2080EngineType) + 1);
    pEvent->xid94.data.xidCode = ROBUST_CHANNEL_CONTAINED_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid94.header);

    return NV_OK;
}

NV_STATUS gccpoisonerrConstruct_IMPL
(
    GccPoisonError *pEvent,
    EventBus *pEventBus,
    NvU32 nv2080EngineType
)
{
    pEvent->context.header.pOutputAdapters = &g_eccGccPoisonErrorOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.nv2080EngineType);
    pEvent->context.nv2080EngineType = nv2080EngineType;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid94.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid94.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid94.data.message,
                          sizeof(pEvent->xid94.data.message),
                          ECC_GCC_POISON_ERROR_XID_MESSAGE_FMT,
                          pEvent->context.nv2080EngineType) + 1);
    pEvent->xid94.data.xidCode = ROBUST_CHANNEL_CONTAINED_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid94.header);

    return NV_OK;
}

NV_STATUS gsppoisonerrConstruct_IMPL
(
    GspPoisonError *pEvent,
    EventBus *pEventBus
)
{
    pEvent->context.header.pOutputAdapters = &g_eccGspPoisonErrorOutputAdapters;
    pEvent->context.header.dataSize = 0;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid94.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid94.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid94.data.message,
                          sizeof(pEvent->xid94.data.message),
                          ECC_GSP_POISON_ERROR_XID_MESSAGE_FMT) + 1);
    pEvent->xid94.data.xidCode = ROBUST_CHANNEL_CONTAINED_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid94.header);

    return NV_OK;
}

NV_STATUS pmupoisonerrConstruct_IMPL
(
    PmuPoisonError *pEvent,
    EventBus *pEventBus
)
{
    pEvent->context.header.pOutputAdapters = &g_eccPmuPoisonErrorOutputAdapters;
    pEvent->context.header.dataSize = 0;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid94.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid94.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid94.data.message,
                          sizeof(pEvent->xid94.data.message),
                          ECC_PMU_POISON_ERROR_XID_MESSAGE_FMT) + 1);
    pEvent->xid94.data.xidCode = ROBUST_CHANNEL_CONTAINED_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid94.header);

    return NV_OK;
}

NV_STATUS sec2poisonerrConstruct_IMPL
(
    Sec2PoisonError *pEvent,
    EventBus *pEventBus
)
{
    pEvent->context.header.pOutputAdapters = &g_eccSec2PoisonErrorOutputAdapters;
    pEvent->context.header.dataSize = 0;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid94.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid94.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid94.data.message,
                          sizeof(pEvent->xid94.data.message),
                          ECC_SEC2_POISON_ERROR_XID_MESSAGE_FMT) + 1);
    pEvent->xid94.data.xidCode = ROBUST_CHANNEL_CONTAINED_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid94.header);

    return NV_OK;
}

NV_STATUS fbfalconpoisonerrConstruct_IMPL
(
    FbFalconPoisonError *pEvent,
    EventBus *pEventBus
)
{
    pEvent->context.header.pOutputAdapters =
        &g_eccFbFalconPoisonErrorOutputAdapters;
    pEvent->context.header.dataSize = 0;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid94.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid94.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid94.data.message,
                          sizeof(pEvent->xid94.data.message),
                          ECC_FBFALCON_POISON_ERROR_XID_MESSAGE_FMT) + 1);
    pEvent->xid94.data.xidCode = ROBUST_CHANNEL_CONTAINED_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid94.header);

    return NV_OK;
}

NV_STATUS videoenginepoisonerrConstruct_IMPL
(
    VideoEnginePoisonError *pEvent,
    EventBus *pEventBus,
    NvU32 nv2080EngineType
)
{
    const char *pEngineName =
        _eccVideoEnginePoisonErrorGetEngineName(nv2080EngineType);

    NV_ASSERT_OR_RETURN(pEngineName != NULL, NV_ERR_INVALID_ARGUMENT);

    pEvent->context.header.pOutputAdapters =
        &g_eccVideoEnginePoisonErrorOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.nv2080EngineType);
    pEvent->context.nv2080EngineType = nv2080EngineType;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid94.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid94.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid94.data.message,
                          sizeof(pEvent->xid94.data.message),
                          ECC_VIDEO_ENGINE_POISON_ERROR_XID_MESSAGE_FMT,
                          pEngineName,
                          pEvent->context.nv2080EngineType) + 1);
    pEvent->xid94.data.xidCode = ROBUST_CHANNEL_CONTAINED_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid94.header);

    return NV_OK;
}

NV_STATUS copyenginepoisoncontainederrConstruct_IMPL
(
    CopyEnginePoisonContainedError *pEvent,
    EventBus *pEventBus,
    NvU32 nv2080EngineType
)
{
    pEvent->context.header.pOutputAdapters =
        &g_eccCopyEnginePoisonContainedErrorOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.nv2080EngineType);
    pEvent->context.nv2080EngineType = nv2080EngineType;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid94.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid94.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid94.data.message,
                          sizeof(pEvent->xid94.data.message),
                          ECC_COPY_ENGINE_POISON_CONTAINED_ERROR_XID_MESSAGE_FMT,
                          pEvent->context.nv2080EngineType) + 1);
    pEvent->xid94.data.xidCode = ROBUST_CHANNEL_CONTAINED_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid94.header);

    return NV_OK;
}

NV_STATUS copyenginepoisonuncontainederrConstruct_IMPL
(
    CopyEnginePoisonUncontainedError *pEvent,
    EventBus *pEventBus,
    NvU32 nv2080EngineType
)
{
    pEvent->context.header.pOutputAdapters =
        &g_eccCopyEnginePoisonUncontainedErrorOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.nv2080EngineType);
    pEvent->context.nv2080EngineType = nv2080EngineType;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid95.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid95.header.dataSize =
        (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
            nvDbgSnprintf(pEvent->xid95.data.message,
                          sizeof(pEvent->xid95.data.message),
                          ECC_COPY_ENGINE_POISON_UNCONTAINED_ERROR_XID_MESSAGE_FMT,
                          pEvent->context.nv2080EngineType) + 1);
    pEvent->xid95.data.xidCode = ROBUST_CHANNEL_UNCONTAINED_ERROR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid95.header);

    return NV_OK;
}

NV_STATUS cedramstormConstruct_IMPL
(
    DramEccIntrStorm *pEvent,
    EventBus *pEventBus,
    NvU32 fbpaIndexVirt
)
{
    pEvent->context.header.pOutputAdapters = &g_eccDramEccIntrStormOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.fbpaIndexVirt);
    pEvent->context.fbpaIndexVirt = fbpaIndexVirt;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid92.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid92.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid92.data.message,
                      sizeof(pEvent->xid92.data.message),
                      ECC_DRAM_ECC_INTR_STORM_XID_MESSAGE_FMT,
                      pEvent->context.fbpaIndexVirt) + 1);
    pEvent->xid92.data.xidCode = EXCESSIVE_SBE_INTERRUPTS;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid92.header);

    return NV_OK;
}

NV_STATUS cesmstormConstruct_IMPL
(
    SmEccIntrStorm *pEvent,
    EventBus *pEventBus
)
{
    pEvent->context.header.pOutputAdapters = &g_eccSmEccIntrStormOutputAdapters;
    pEvent->context.header.dataSize = 0;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid92.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid92.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid92.data.message,
                      sizeof(pEvent->xid92.data.message),
                      ECC_SM_ECC_INTR_STORM_XID_MESSAGE_FMT) + 1);
    pEvent->xid92.data.xidCode = EXCESSIVE_SBE_INTERRUPTS;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid92.header);

    return NV_OK;
}

NV_STATUS eccresidualConstruct_IMPL
(
    EccResidualUncorrectableError *pEvent,
    EventBus *pEventBus,
    NvU32 severity,
    NvU32 dramCount,
    NvU32 ltcCount,
    NvU32 mmuCount,
    NvU32 pcieCount
)
{
    pEvent->context.header.pOutputAdapters = &g_eccResidualUncorrectableErrorOutputAdapters;
    pEvent->context.dramCount = dramCount;
    pEvent->context.ltcCount = ltcCount;
    pEvent->context.mmuCount = mmuCount;
    pEvent->context.pcieCount = pcieCount;
    pEvent->context.header.dataSize = sizeof(pEvent->context.dramCount) +
                                      sizeof(pEvent->context.ltcCount) +
                                      sizeof(pEvent->context.mmuCount) +
                                      sizeof(pEvent->context.pcieCount);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid140.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid140.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid140.data.message,
                      sizeof(pEvent->xid140.data.message),
                      ECC_RESIDUAL_UNCORRECTABLE_ERROR_XID_MESSAGE_FMT,
                      pEvent->context.dramCount,
                      pEvent->context.ltcCount,
                      pEvent->context.mmuCount,
                      pEvent->context.pcieCount) + 1);
    pEvent->xid140.data.xidCode = UNRECOVERABLE_ECC_ERROR_ESCAPE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid140.header);

    return NV_OK;
}

#if !defined(SRT_BUILD) && RMCFG_FEATURE_GSP_CLIENT_RM
NV_STATUS rrpendingDeserialize
(
    OBJGPU *pGpu,
    const NV_GPU_OPERATIONAL_EVENT_RECORD *pRecord,
    const NvU8 *pContextData,
    NvU32 contextDataSize
)
{
    static const GpuOpEventExpectedMetadata expected =
    {
        .pModuleSignature = "GPU-RAS",
        .moduleEventCode = RAS_OP_EVENT_CODE_ROW_REMAPPING_PENDING,
        .eventCategory = GPU_OPERATIONAL_EVENT_CATEGORY_RESOURCE_RETIREMENT,
        .allowedSeverityMask = GPU_OP_EVENT_SEVERITY_MASK_RECOVERABLE,
        .attributes = (NvU8)REF_DEF(OPERATIONAL_EVENT_ATTR_PRIMARY, _TRUE),
        .scope = GPU_OPERATIONAL_EVENT_SCOPE_DEVICE,
        .reportingSource = GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_UNKNOWN,
        .logLevel = GPU_OPERATIONAL_EVENT_LOG_LEVEL_NOTICE,
        .bReportingSourceVariable = NV_TRUE,
    };
    const NvU8 *pPayload;
    NvU32 payloadSize;
    NvU64 physicalAddress;
    NvU32 source;
    NV_STATUS status;

    if ((pGpu == NULL) || (pRecord == NULL))
        return NV_ERR_INVALID_ARGUMENT;

    status = gpuOpEventValidateMetadata(pRecord, &expected);
    if (status != NV_OK)
        return status;

    status = gpuOpEventFindContext(pRecord, pContextData, contextDataSize,
                                   OPERATIONAL_EVENT_CTX_TYPE_OPAQUE, 0,
                                   &pPayload, &payloadSize);
    if (status != NV_OK)
        return status;
    if (payloadSize != (sizeof(physicalAddress) + sizeof(source)))
        return NV_ERR_INVALID_DATA;

    portMemCopy(&physicalAddress, sizeof(physicalAddress),
                pPayload, sizeof(physicalAddress));
    portMemCopy(&source, sizeof(source),
                pPayload + sizeof(physicalAddress), sizeof(source));
    if ((source != GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_DRIVER_CLIENT) &&
        (source != GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT))
    {
        return NV_ERR_INVALID_DATA;
    }
    if (source != pRecord->reportingSource)
        return NV_ERR_INVALID_DATA;

    eventTryEmitSimulated(RowRemapPending, pGpu, status,
                          source, physicalAddress);
    return status;
}

NV_STATUS rrtablefullDeserialize
(
    OBJGPU *pGpu,
    const NV_GPU_OPERATIONAL_EVENT_RECORD *pRecord,
    const NvU8 *pContextData,
    NvU32 contextDataSize
)
{
    static const GpuOpEventExpectedMetadata expected =
    {
        .pModuleSignature = "GPU-RAS",
        .moduleEventCode = RAS_OP_EVENT_CODE_ROW_REMAPPING_FAILURE_TABLE_FULL,
        .eventCategory = GPU_OPERATIONAL_EVENT_CATEGORY_RESOURCE_EXHAUSTED,
        .allowedSeverityMask = GPU_OP_EVENT_SEVERITY_MASK_FATAL,
        .attributes = (NvU8)REF_DEF(OPERATIONAL_EVENT_ATTR_PRIMARY, _TRUE),
        .scope = GPU_OPERATIONAL_EVENT_SCOPE_DEVICE,
        .reportingSource = GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_SW_CHECK,
        .logLevel = GPU_OPERATIONAL_EVENT_LOG_LEVEL_ERROR,
        .bReportingSourceVariable = NV_FALSE,
    };
    const NvU8 *pPayload;
    NvU32 payloadSize;
    NvU64 physicalAddress;
    NV_STATUS status;

    if ((pGpu == NULL) || (pRecord == NULL))
        return NV_ERR_INVALID_ARGUMENT;

    status = gpuOpEventValidateMetadata(pRecord, &expected);
    if (status != NV_OK)
        return status;

    status = gpuOpEventFindContext(pRecord, pContextData, contextDataSize,
                                   OPERATIONAL_EVENT_CTX_TYPE_OPAQUE, 0,
                                   &pPayload, &payloadSize);
    if (status != NV_OK)
        return status;
    if (payloadSize != sizeof(physicalAddress))
        return NV_ERR_INVALID_DATA;

    portMemCopy(&physicalAddress, sizeof(physicalAddress),
                pPayload, sizeof(physicalAddress));
    eventTryEmitSimulated(RowRemappingFailureTableFull, pGpu, status,
                          physicalAddress);
    return status;
}
#endif

static const char *
_eccUnitUncorrectableErrorName
(
    NvU32 unit
)
{
    switch (unit)
    {
        case NV2080_CTRL_GPU_ECC_UNIT_HUBMMU_L2TLB:
            return "HUBMMU L2TLB";
        case NV2080_CTRL_GPU_ECC_UNIT_HUBMMU_HUBTLB:
            return "HUBMMU HUBTLB";
        case NV2080_CTRL_GPU_ECC_UNIT_HUBMMU_FILLUNIT:
            return "HUBMMU FILLUNIT";
        case NV2080_CTRL_GPU_ECC_UNIT_HSHUBMMU:
            return "HSHUBMMU";
        case NV2080_CTRL_GPU_ECC_UNIT_PCIE_REORDER:
            return "PCIE REORDER";
        case NV2080_CTRL_GPU_ECC_UNIT_PCIE_P2PREQ:
            return "PCIE P2PREQ";
        case NV2080_CTRL_GPU_ECC_UNIT_PCIE_TRS_P:
            return "PCIE TRS_P";
        case NV2080_CTRL_GPU_ECC_UNIT_PCIE_RBUF:
            return "PCIE RBUF";
        case NV2080_CTRL_GPU_ECC_UNIT_PCIE_SEQ_LUT:
            return "PCIE SEQ_LUT";
        case NV2080_CTRL_GPU_ECC_UNIT_PCIE_XTL:
            return "PCIE XTL";
        case NV2080_CTRL_GPU_ECC_UNIT_PCIE_XPL:
            return "PCIE XPL";
        case NV2080_CTRL_GPU_ECC_UNIT_PCIE_CXL:
            return "PCIE CXL";
        default:
            NV_PRINTF(LEVEL_ERROR, "Unknown unit name\n");
            NV_ASSERT(0);
            return NULL;
    }
}

static const char *
_eccSmUncorrectableErrorXid48UnitName
(
    NvU32 nv2080GpuEccUnit
)
{
    switch (nv2080GpuEccUnit)
    {
        case NV2080_CTRL_GPU_ECC_UNIT_SM:
            return "SM LRF";
        case NV2080_CTRL_GPU_ECC_UNIT_SM_L1_DATA:
            return "SM L1 Data";
        case NV2080_CTRL_GPU_ECC_UNIT_SM_L1_TAG:
            return "SM L1 Tag";
        case NV2080_CTRL_GPU_ECC_UNIT_SM_CBU:
            return "SM CBU";
        case NV2080_CTRL_GPU_ECC_UNIT_SM_ICACHE:
            return "SM ICACHE";
        case NV2080_CTRL_GPU_ECC_UNIT_SM_RAMS:
            return "SM RAMS";
        default:
            NV_PRINTF(LEVEL_ERROR, "Unknown SM ECC unit: 0x%x\n", nv2080GpuEccUnit);
            return NULL;
    }
}

static const char *
_eccSmUncorrectableErrorXid172UnitName
(
    NvU32 nv2080GpuEccUnit
)
{
    switch (nv2080GpuEccUnit)
    {
        case NV2080_CTRL_GPU_ECC_UNIT_SM:
            return "LRF";
        case NV2080_CTRL_GPU_ECC_UNIT_SM_L1_DATA:
            return "L1 Data";
        case NV2080_CTRL_GPU_ECC_UNIT_SM_L1_TAG:
            return "L1 Tag";
        case NV2080_CTRL_GPU_ECC_UNIT_SM_CBU:
            return "CBU";
        case NV2080_CTRL_GPU_ECC_UNIT_SM_ICACHE:
            return "SM ICACHE";
        case NV2080_CTRL_GPU_ECC_UNIT_SM_RAMS:
            return "SM RAMS";
        default:
            NV_PRINTF(LEVEL_ERROR, "Unknown SM ECC unit: 0x%x\n", nv2080GpuEccUnit);
            return NULL;
    }
}

static const char *
_eccNvlinkUncorrectableErrorUnitName
(
    NvU32 nv2080GpuEccUnit
)
{
    switch (nv2080GpuEccUnit)
    {
        case NV2080_CTRL_GPU_ECC_UNIT_MSE:
            return "MSE-RISCV";
        case NV2080_CTRL_GPU_ECC_UNIT_NVLINK_SAW:
            return "NVLINK SAW";
        case NV2080_CTRL_GPU_ECC_UNIT_NVLINK_RLW:
            return "NVLINK RLW";
        case NV2080_CTRL_GPU_ECC_UNIT_NVLINK_TLW:
            return "NVLINK TLW";
        case NV2080_CTRL_GPU_ECC_UNIT_NVLINK_MVB:
            return "NVLINK MVB";
        default:
            NV_PRINTF(LEVEL_ERROR, "Unknown unit name\n");
            NV_ASSERT(0);
            return NULL;
    }
}

static void
_eccAddLegacyXidContext
(
    OperationalEvent *pOperationalEvent,
    GpuXidEventCtx *pXid,
    NvU32 xidCode
)
{
    pXid->header.pOutputAdapters = &g_eccXidOutputAdapters;
    pXid->header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        portStringLength(pXid->data.message) + 1U);
    pXid->data.xidCode = xidCode;
    opevtAddContext(pOperationalEvent, &pXid->header);
}

static void
_eccUnitUncorrectableErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    EccUnitUncorrectableErrorCtx *pContext =
        (EccUnitUncorrectableErrorCtx *)pContextHeader;
    const char *pUnitName = _eccUnitUncorrectableErrorName(pContext->unit);

    NV_ASSERT_OR_RETURN_VOID(pUnitName != NULL);

    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_GPU_ECC_DBE,
                     ECC_UNIT_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT,
                     pUnitName);
    OP_EVENT_XID_LOG(pGpu, bSimulated, UNCORRECTABLE_SRAM_ERROR,
                     ECC_UNIT_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT,
                     pUnitName);
}

static void
_eccCopyEngineUncorrectableErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    CopyEngineUncorrectableErrorCtx *pContext =
        (CopyEngineUncorrectableErrorCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_GPU_ECC_DBE,
                     ECC_COPY_ENGINE_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT,
                     pContext->dieletType, pContext->dieletId, pContext->pce);
    OP_EVENT_XID_LOG(pGpu, bSimulated, UNCORRECTABLE_SRAM_ERROR,
                     ECC_COPY_ENGINE_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT,
                     pContext->dieletType, pContext->dieletId, pContext->pce);
}

static void
_eccFecsUncorrectableErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    (void)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_GPU_ECC_DBE,
                     ECC_FECS_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT);
    OP_EVENT_XID_LOG(pGpu, bSimulated, UNCORRECTABLE_SRAM_ERROR,
                     ECC_FECS_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT);
}

static void
_eccSmUncorrectableErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    SmUncorrectableErrorCtx *pContext = (SmUncorrectableErrorCtx *)pContextHeader;
    const char *pXid48UnitName = _eccSmUncorrectableErrorXid48UnitName(pContext->unit);
    const char *pXid172UnitName = _eccSmUncorrectableErrorXid172UnitName(pContext->unit);

    NV_ASSERT_OR_RETURN_VOID((pXid48UnitName != NULL) && (pXid172UnitName != NULL));

    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_GPU_ECC_DBE,
                     ECC_SM_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT,
                     pXid48UnitName, pContext->gpc, pContext->tpc);
    OP_EVENT_XID_LOG(pGpu, bSimulated, UNCORRECTABLE_SRAM_ERROR,
                     ECC_SM_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT,
                     pXid172UnitName, pContext->gpc, pContext->tpc);
}

static void
_eccGccUncorrectableErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    GrUncorrectableErrorCtx *pContext = (GrUncorrectableErrorCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_GPU_ECC_DBE,
                     ECC_GCC_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT,
                     pContext->gpc);
    OP_EVENT_XID_LOG(pGpu, bSimulated, UNCORRECTABLE_SRAM_ERROR,
                     ECC_GCC_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT,
                     pContext->gpc);
}

static void
_eccContextSwitchUncorrectableErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    GrUncorrectableErrorCtx *pContext = (GrUncorrectableErrorCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_GPU_ECC_DBE,
                     ECC_CONTEXT_SWITCH_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT,
                     pContext->gpc);
    OP_EVENT_XID_LOG(pGpu, bSimulated, UNCORRECTABLE_SRAM_ERROR,
                     ECC_CONTEXT_SWITCH_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT,
                     pContext->gpc);
}

static void
_eccGpcMmuUncorrectableErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    GrUncorrectableErrorCtx *pContext = (GrUncorrectableErrorCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_GPU_ECC_DBE,
                     ECC_GPCMMU_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT,
                     pContext->gpc);
    OP_EVENT_XID_LOG(pGpu, bSimulated, UNCORRECTABLE_SRAM_ERROR,
                     ECC_GPCMMU_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT,
                     pContext->gpc);
}

static void
_eccGspUncorrectableErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    RiscvUncorrectableErrorCtx *pContext =
        (RiscvUncorrectableErrorCtx *)pContextHeader;

    if (pContext->bHasLocation)
    {
        OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_GPU_ECC_DBE,
                         ECC_GSP_UNCORRECTABLE_ERROR_LOCATION_XID48_MESSAGE_FMT,
                         pContext->location);
        OP_EVENT_XID_LOG(pGpu, bSimulated, UNCORRECTABLE_SRAM_ERROR,
                         ECC_GSP_UNCORRECTABLE_ERROR_LOCATION_XID172_MESSAGE_FMT,
                         pContext->location);
    }
    else
    {
        OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_GPU_ECC_DBE,
                         ECC_GSP_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT);
        OP_EVENT_XID_LOG(pGpu, bSimulated, UNCORRECTABLE_SRAM_ERROR,
                         ECC_GSP_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT);
    }
}

static void
_eccLpsecUncorrectableErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    LpsecUncorrectableErrorCtx *pContext =
        (LpsecUncorrectableErrorCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_GPU_ECC_DBE,
                     ECC_LPSEC_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT,
                     pContext->sequencer);
    OP_EVENT_XID_LOG(pGpu, bSimulated, UNCORRECTABLE_SRAM_ERROR,
                     ECC_LPSEC_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT,
                     pContext->sequencer);
}

static void
_eccNvlinkUncorrectableErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    EccUnitUncorrectableErrorCtx *pContext =
        (EccUnitUncorrectableErrorCtx *)pContextHeader;
    const char *pUnitName = _eccNvlinkUncorrectableErrorUnitName(pContext->unit);

    NV_ASSERT_OR_RETURN_VOID(pUnitName != NULL);

    OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_GPU_ECC_DBE,
                     ECC_NVLINK_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT,
                     pUnitName);
    OP_EVENT_XID_LOG(pGpu, bSimulated, UNCORRECTABLE_SRAM_ERROR,
                     ECC_NVLINK_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT,
                     pUnitName);
}

static void
_eccPmuUncorrectableErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    PmuUncorrectableErrorCtx *pContext =
        (PmuUncorrectableErrorCtx *)pContextHeader;

    if (pContext->bIsFalcon)
    {
        OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_GPU_ECC_DBE,
                         ECC_PMU_FALCON_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT);
        OP_EVENT_XID_LOG(pGpu, bSimulated, UNCORRECTABLE_SRAM_ERROR,
                         ECC_PMU_FALCON_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT);
    }
    else if (pContext->bHasLocation)
    {
        OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_GPU_ECC_DBE,
                         ECC_PMU_UNCORRECTABLE_ERROR_LOCATION_XID48_MESSAGE_FMT,
                         pContext->location);
        OP_EVENT_XID_LOG(pGpu, bSimulated, UNCORRECTABLE_SRAM_ERROR,
                         ECC_PMU_UNCORRECTABLE_ERROR_LOCATION_XID172_MESSAGE_FMT,
                         pContext->location);
    }
    else
    {
        OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_GPU_ECC_DBE,
                         ECC_PMU_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT);
        OP_EVENT_XID_LOG(pGpu, bSimulated, UNCORRECTABLE_SRAM_ERROR,
                         ECC_PMU_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT);
    }
}

static void
_eccSec2UncorrectableErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    RiscvUncorrectableErrorCtx *pContext =
        (RiscvUncorrectableErrorCtx *)pContextHeader;

    if (pContext->bHasLocation)
    {
        OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_GPU_ECC_DBE,
                         ECC_SEC2_UNCORRECTABLE_ERROR_LOCATION_XID48_MESSAGE_FMT,
                         pContext->location);
        OP_EVENT_XID_LOG(pGpu, bSimulated, UNCORRECTABLE_SRAM_ERROR,
                         ECC_SEC2_UNCORRECTABLE_ERROR_LOCATION_XID172_MESSAGE_FMT,
                         pContext->location);
    }
    else
    {
        OP_EVENT_XID_LOG(pGpu, bSimulated, ROBUST_CHANNEL_GPU_ECC_DBE,
                         ECC_SEC2_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT);
        OP_EVENT_XID_LOG(pGpu, bSimulated, UNCORRECTABLE_SRAM_ERROR,
                         ECC_SEC2_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT);
    }
}

#define ECC_UNCORRECTABLE_ERROR_OUTPUT_ADAPTERS(_name, _emit)              \
    static const EventContextOutputAdapters _name =                        \
    {                                                                       \
        .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx, \
        .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,      \
        .emitToOsLogXid = _emit,                                            \
    }

ECC_UNCORRECTABLE_ERROR_OUTPUT_ADAPTERS(
    g_eccUnitUncorrectableErrorOutputAdapters,
    _eccUnitUncorrectableErrorEmitOsLogXid);
ECC_UNCORRECTABLE_ERROR_OUTPUT_ADAPTERS(
    g_eccCopyEngineUncorrectableErrorOutputAdapters,
    _eccCopyEngineUncorrectableErrorEmitOsLogXid);
ECC_UNCORRECTABLE_ERROR_OUTPUT_ADAPTERS(
    g_eccFecsUncorrectableErrorOutputAdapters,
    _eccFecsUncorrectableErrorEmitOsLogXid);
ECC_UNCORRECTABLE_ERROR_OUTPUT_ADAPTERS(
    g_eccSmUncorrectableErrorOutputAdapters,
    _eccSmUncorrectableErrorEmitOsLogXid);
ECC_UNCORRECTABLE_ERROR_OUTPUT_ADAPTERS(
    g_eccGccUncorrectableErrorOutputAdapters,
    _eccGccUncorrectableErrorEmitOsLogXid);
ECC_UNCORRECTABLE_ERROR_OUTPUT_ADAPTERS(
    g_eccContextSwitchUncorrectableErrorOutputAdapters,
    _eccContextSwitchUncorrectableErrorEmitOsLogXid);
ECC_UNCORRECTABLE_ERROR_OUTPUT_ADAPTERS(
    g_eccGpcMmuUncorrectableErrorOutputAdapters,
    _eccGpcMmuUncorrectableErrorEmitOsLogXid);
ECC_UNCORRECTABLE_ERROR_OUTPUT_ADAPTERS(
    g_eccGspUncorrectableErrorOutputAdapters,
    _eccGspUncorrectableErrorEmitOsLogXid);
ECC_UNCORRECTABLE_ERROR_OUTPUT_ADAPTERS(
    g_eccLpsecUncorrectableErrorOutputAdapters,
    _eccLpsecUncorrectableErrorEmitOsLogXid);
ECC_UNCORRECTABLE_ERROR_OUTPUT_ADAPTERS(
    g_eccNvlinkUncorrectableErrorOutputAdapters,
    _eccNvlinkUncorrectableErrorEmitOsLogXid);
ECC_UNCORRECTABLE_ERROR_OUTPUT_ADAPTERS(
    g_eccPmuUncorrectableErrorOutputAdapters,
    _eccPmuUncorrectableErrorEmitOsLogXid);
ECC_UNCORRECTABLE_ERROR_OUTPUT_ADAPTERS(
    g_eccSec2UncorrectableErrorOutputAdapters,
    _eccSec2UncorrectableErrorEmitOsLogXid);

#undef ECC_UNCORRECTABLE_ERROR_OUTPUT_ADAPTERS

NV_STATUS pcieuceerrConstruct_IMPL
(
    PcieUncorrectableError *pEvent,
    EventBus *pEventBus,
    NvU32 unit
)
{
    const char *pUnitName = _eccUnitUncorrectableErrorName(unit);

    NV_ASSERT_OR_RETURN(pUnitName != NULL, NV_ERR_INVALID_ARGUMENT);

    pEvent->context.header.pOutputAdapters = &g_eccUnitUncorrectableErrorOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.unit);
    pEvent->context.unit = unit;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    nvDbgSnprintf(pEvent->xid48.data.message,
                  sizeof(pEvent->xid48.data.message),
                  ECC_UNIT_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT,
                  pUnitName);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid48,
                            ROBUST_CHANNEL_GPU_ECC_DBE);

    nvDbgSnprintf(pEvent->xid172.data.message,
                  sizeof(pEvent->xid172.data.message),
                  ECC_UNIT_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT,
                  pUnitName);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid172,
                            UNCORRECTABLE_SRAM_ERROR);

    return NV_OK;
}

NV_STATUS copyengineuceConstruct_IMPL
(
    CopyEngineUncorrectableError *pEvent,
    EventBus *pEventBus,
    NvU32 dieletType,
    NvU32 dieletId,
    NvU32 pce
)
{
    pEvent->context.header.pOutputAdapters =
        &g_eccCopyEngineUncorrectableErrorOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.dieletType) +
                                      sizeof(pEvent->context.dieletId) +
                                      sizeof(pEvent->context.pce);
    pEvent->context.dieletType = dieletType;
    pEvent->context.dieletId = dieletId;
    pEvent->context.pce = pce;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    nvDbgSnprintf(pEvent->xid48.data.message,
                  sizeof(pEvent->xid48.data.message),
                  ECC_COPY_ENGINE_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT,
                  dieletType, dieletId, pce);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid48,
                            ROBUST_CHANNEL_GPU_ECC_DBE);

    nvDbgSnprintf(pEvent->xid172.data.message,
                  sizeof(pEvent->xid172.data.message),
                  ECC_COPY_ENGINE_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT,
                  dieletType, dieletId, pce);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid172,
                            UNCORRECTABLE_SRAM_ERROR);

    return NV_OK;
}

NV_STATUS mmuuceerrConstruct_IMPL
(
    MmuUncorrectableError *pEvent,
    EventBus *pEventBus,
    NvU32 unit
)
{
    const char *pUnitName = _eccUnitUncorrectableErrorName(unit);

    NV_ASSERT_OR_RETURN(pUnitName != NULL, NV_ERR_INVALID_ARGUMENT);

    pEvent->context.header.pOutputAdapters = &g_eccUnitUncorrectableErrorOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.unit);
    pEvent->context.unit = unit;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    nvDbgSnprintf(pEvent->xid48.data.message,
                  sizeof(pEvent->xid48.data.message),
                  ECC_UNIT_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT,
                  pUnitName);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid48,
                            ROBUST_CHANNEL_GPU_ECC_DBE);

    nvDbgSnprintf(pEvent->xid172.data.message,
                  sizeof(pEvent->xid172.data.message),
                  ECC_UNIT_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT,
                  pUnitName);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid172,
                            UNCORRECTABLE_SRAM_ERROR);

    return NV_OK;
}

NV_STATUS fecsuceerrConstruct_IMPL
(
    FecsUncorrectableError *pEvent,
    EventBus *pEventBus
)
{
    pEvent->context.pOutputAdapters = &g_eccFecsUncorrectableErrorOutputAdapters;
    pEvent->context.dataSize = 0;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context);

    nvDbgSnprintf(pEvent->xid48.data.message,
                  sizeof(pEvent->xid48.data.message),
                  ECC_FECS_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid48,
                            ROBUST_CHANNEL_GPU_ECC_DBE);

    nvDbgSnprintf(pEvent->xid172.data.message,
                  sizeof(pEvent->xid172.data.message),
                  ECC_FECS_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid172,
                            UNCORRECTABLE_SRAM_ERROR);

    return NV_OK;
}

NV_STATUS smuceerrConstruct_IMPL
(
    SmUncorrectableError *pEvent,
    EventBus *pEventBus,
    NvU32 unit,
    NvU32 gpc,
    NvU32 tpc
)
{
    const char *pXid48UnitName = _eccSmUncorrectableErrorXid48UnitName(unit);
    const char *pXid172UnitName = _eccSmUncorrectableErrorXid172UnitName(unit);

    NV_ASSERT_OR_RETURN((pXid48UnitName != NULL) && (pXid172UnitName != NULL),
                        NV_ERR_INVALID_ARGUMENT);

    pEvent->context.header.pOutputAdapters = &g_eccSmUncorrectableErrorOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.unit) +
                                      sizeof(pEvent->context.gpc) +
                                      sizeof(pEvent->context.tpc);
    pEvent->context.unit = unit;
    pEvent->context.gpc = gpc;
    pEvent->context.tpc = tpc;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    nvDbgSnprintf(pEvent->xid48.data.message,
                  sizeof(pEvent->xid48.data.message),
                  ECC_SM_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT,
                  pXid48UnitName, gpc, tpc);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid48,
                            ROBUST_CHANNEL_GPU_ECC_DBE);

    nvDbgSnprintf(pEvent->xid172.data.message,
                  sizeof(pEvent->xid172.data.message),
                  ECC_SM_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT,
                  pXid172UnitName, gpc, tpc);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid172,
                            UNCORRECTABLE_SRAM_ERROR);

    return NV_OK;
}

NV_STATUS gccuceerrConstruct_IMPL
(
    GccUncorrectableError *pEvent,
    EventBus *pEventBus,
    NvU32 gpc
)
{
    pEvent->context.header.pOutputAdapters = &g_eccGccUncorrectableErrorOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.gpc);
    pEvent->context.gpc = gpc;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    nvDbgSnprintf(pEvent->xid48.data.message,
                  sizeof(pEvent->xid48.data.message),
                  ECC_GCC_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT,
                  gpc);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid48,
                            ROBUST_CHANNEL_GPU_ECC_DBE);

    nvDbgSnprintf(pEvent->xid172.data.message,
                  sizeof(pEvent->xid172.data.message),
                  ECC_GCC_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT,
                  gpc);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid172,
                            UNCORRECTABLE_SRAM_ERROR);

    return NV_OK;
}

NV_STATUS ctxswuceerrConstruct_IMPL
(
    ContextSwitchUncorrectableError *pEvent,
    EventBus *pEventBus,
    NvU32 gpc
)
{
    pEvent->context.header.pOutputAdapters =
        &g_eccContextSwitchUncorrectableErrorOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.gpc);
    pEvent->context.gpc = gpc;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    nvDbgSnprintf(pEvent->xid48.data.message,
                  sizeof(pEvent->xid48.data.message),
                  ECC_CONTEXT_SWITCH_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT,
                  gpc);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid48,
                            ROBUST_CHANNEL_GPU_ECC_DBE);

    nvDbgSnprintf(pEvent->xid172.data.message,
                  sizeof(pEvent->xid172.data.message),
                  ECC_CONTEXT_SWITCH_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT,
                  gpc);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid172,
                            UNCORRECTABLE_SRAM_ERROR);

    return NV_OK;
}

NV_STATUS gpcmmuuceerrConstruct_IMPL
(
    GpcMmuUncorrectableError *pEvent,
    EventBus *pEventBus,
    NvU32 gpc
)
{
    pEvent->context.header.pOutputAdapters =
        &g_eccGpcMmuUncorrectableErrorOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.gpc);
    pEvent->context.gpc = gpc;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    nvDbgSnprintf(pEvent->xid48.data.message,
                  sizeof(pEvent->xid48.data.message),
                  ECC_GPCMMU_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT,
                  gpc);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid48,
                            ROBUST_CHANNEL_GPU_ECC_DBE);

    nvDbgSnprintf(pEvent->xid172.data.message,
                  sizeof(pEvent->xid172.data.message),
                  ECC_GPCMMU_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT,
                  gpc);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid172,
                            UNCORRECTABLE_SRAM_ERROR);

    return NV_OK;
}

NV_STATUS gspuceerrConstruct_IMPL
(
    GspUncorrectableError *pEvent,
    EventBus *pEventBus,
    NvBool bHasLocation,
    NvU32 location
)
{
    pEvent->context.header.pOutputAdapters = &g_eccGspUncorrectableErrorOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.location) +
                                      sizeof(pEvent->context.bHasLocation);
    pEvent->context.location = location;
    pEvent->context.bHasLocation = bHasLocation;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    if (bHasLocation)
    {
        nvDbgSnprintf(pEvent->xid48.data.message,
                      sizeof(pEvent->xid48.data.message),
                      ECC_GSP_UNCORRECTABLE_ERROR_LOCATION_XID48_MESSAGE_FMT,
                      location);
        nvDbgSnprintf(pEvent->xid172.data.message,
                      sizeof(pEvent->xid172.data.message),
                      ECC_GSP_UNCORRECTABLE_ERROR_LOCATION_XID172_MESSAGE_FMT,
                      location);
    }
    else
    {
        nvDbgSnprintf(pEvent->xid48.data.message,
                      sizeof(pEvent->xid48.data.message),
                      ECC_GSP_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT);
        nvDbgSnprintf(pEvent->xid172.data.message,
                      sizeof(pEvent->xid172.data.message),
                      ECC_GSP_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT);
    }

    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid48,
                            ROBUST_CHANNEL_GPU_ECC_DBE);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid172,
                            UNCORRECTABLE_SRAM_ERROR);

    return NV_OK;
}

NV_STATUS lpsecuceerrConstruct_IMPL
(
    LpsecUncorrectableError *pEvent,
    EventBus *pEventBus,
    NvU32 sequencer
)
{
    pEvent->context.header.pOutputAdapters =
        &g_eccLpsecUncorrectableErrorOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.sequencer);
    pEvent->context.sequencer = sequencer;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    nvDbgSnprintf(pEvent->xid48.data.message,
                  sizeof(pEvent->xid48.data.message),
                  ECC_LPSEC_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT,
                  sequencer);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid48,
                            ROBUST_CHANNEL_GPU_ECC_DBE);

    nvDbgSnprintf(pEvent->xid172.data.message,
                  sizeof(pEvent->xid172.data.message),
                  ECC_LPSEC_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT,
                  sequencer);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid172,
                            UNCORRECTABLE_SRAM_ERROR);

    return NV_OK;
}

NV_STATUS nvlinkuceerrConstruct_IMPL
(
    NvlinkUncorrectableError *pEvent,
    EventBus *pEventBus,
    NvU32 nv2080GpuEccUnit
)
{
    const char *pUnitName =
        _eccNvlinkUncorrectableErrorUnitName(nv2080GpuEccUnit);

    NV_ASSERT_OR_RETURN(pUnitName != NULL, NV_ERR_INVALID_ARGUMENT);

    pEvent->context.header.pOutputAdapters = &g_eccNvlinkUncorrectableErrorOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.unit);
    pEvent->context.unit = nv2080GpuEccUnit;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    nvDbgSnprintf(pEvent->xid48.data.message,
                  sizeof(pEvent->xid48.data.message),
                  ECC_NVLINK_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT,
                  pUnitName);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid48,
                            ROBUST_CHANNEL_GPU_ECC_DBE);

    nvDbgSnprintf(pEvent->xid172.data.message,
                  sizeof(pEvent->xid172.data.message),
                  ECC_NVLINK_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT,
                  pUnitName);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid172,
                            UNCORRECTABLE_SRAM_ERROR);

    return NV_OK;
}

NV_STATUS pmuuceerrConstruct_IMPL
(
    PmuUncorrectableError *pEvent,
    EventBus *pEventBus,
    NvBool bIsFalcon,
    NvBool bHasLocation,
    NvU32 location
)
{
    pEvent->context.header.pOutputAdapters = &g_eccPmuUncorrectableErrorOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.location) +
                                      sizeof(pEvent->context.bHasLocation) +
                                      sizeof(pEvent->context.bIsFalcon);
    pEvent->context.location = location;
    pEvent->context.bHasLocation = bHasLocation;
    pEvent->context.bIsFalcon = bIsFalcon;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    if (bIsFalcon)
    {
        nvDbgSnprintf(pEvent->xid48.data.message,
                      sizeof(pEvent->xid48.data.message),
                      ECC_PMU_FALCON_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT);
        nvDbgSnprintf(pEvent->xid172.data.message,
                      sizeof(pEvent->xid172.data.message),
                      ECC_PMU_FALCON_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT);
    }
    else if (bHasLocation)
    {
        nvDbgSnprintf(pEvent->xid48.data.message,
                      sizeof(pEvent->xid48.data.message),
                      ECC_PMU_UNCORRECTABLE_ERROR_LOCATION_XID48_MESSAGE_FMT,
                      location);
        nvDbgSnprintf(pEvent->xid172.data.message,
                      sizeof(pEvent->xid172.data.message),
                      ECC_PMU_UNCORRECTABLE_ERROR_LOCATION_XID172_MESSAGE_FMT,
                      location);
    }
    else
    {
        nvDbgSnprintf(pEvent->xid48.data.message,
                      sizeof(pEvent->xid48.data.message),
                      ECC_PMU_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT);
        nvDbgSnprintf(pEvent->xid172.data.message,
                      sizeof(pEvent->xid172.data.message),
                      ECC_PMU_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT);
    }

    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid48,
                            ROBUST_CHANNEL_GPU_ECC_DBE);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid172,
                            UNCORRECTABLE_SRAM_ERROR);

    return NV_OK;
}

NV_STATUS sec2uceerrConstruct_IMPL
(
    Sec2UncorrectableError *pEvent,
    EventBus *pEventBus,
    NvBool bHasLocation,
    NvU32 location
)
{
    pEvent->context.header.pOutputAdapters = &g_eccSec2UncorrectableErrorOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.location) +
                                      sizeof(pEvent->context.bHasLocation);
    pEvent->context.location = location;
    pEvent->context.bHasLocation = bHasLocation;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    if (bHasLocation)
    {
        nvDbgSnprintf(pEvent->xid48.data.message,
                      sizeof(pEvent->xid48.data.message),
                      ECC_SEC2_UNCORRECTABLE_ERROR_LOCATION_XID48_MESSAGE_FMT,
                      location);
        nvDbgSnprintf(pEvent->xid172.data.message,
                      sizeof(pEvent->xid172.data.message),
                      ECC_SEC2_UNCORRECTABLE_ERROR_LOCATION_XID172_MESSAGE_FMT,
                      location);
    }
    else
    {
        nvDbgSnprintf(pEvent->xid48.data.message,
                      sizeof(pEvent->xid48.data.message),
                      ECC_SEC2_UNCORRECTABLE_ERROR_XID48_MESSAGE_FMT);
        nvDbgSnprintf(pEvent->xid172.data.message,
                      sizeof(pEvent->xid172.data.message),
                      ECC_SEC2_UNCORRECTABLE_ERROR_XID172_MESSAGE_FMT);
    }

    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid48,
                            ROBUST_CHANNEL_GPU_ECC_DBE);
    _eccAddLegacyXidContext(staticCast(pEvent, OperationalEvent),
                            &pEvent->xid172,
                            UNCORRECTABLE_SRAM_ERROR);

    return NV_OK;
}
