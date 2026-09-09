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

#define ECC_TPC_REPAIR_PENDING_SAME_GPC_XID_MESSAGE_FMT \
    "Retiring TPC %d from GPC %d with a spare from the same GPC."
#define ECC_TPC_REPAIR_PENDING_DIFFERENT_GPC_XID_MESSAGE_FMT \
    "Retiring TPC %d from GPC %d with a TPC from a different GPC."
#define ECC_TPC_REPAIR_FAILURE_NO_SPARE_XID_MESSAGE_FMT \
    "Unable to retire TPC %d from GPC %d as there are no spare TPCs available."
#define ECC_TPC_REPAIR_FAILURE_NO_SPARE_MIG_XID_MESSAGE_FMT \
    "Unable to retire TPC %d from GPC %d in MIG mode as there are no spare TPCs in the same GPC."

#define ECC_DRAM_ECC_INTR_STORM_XID_MESSAGE_FMT \
    "Disabling ECC single-bit error interrupts in framebuffer at logical partition " \
    "%d, due to high error rate"
#define ECC_SM_ECC_INTR_STORM_XID_MESSAGE_FMT \
    "SM SBE interrupt storm detected"
#define ECC_RESIDUAL_UNCORRECTABLE_ERROR_XID_MESSAGE_FMT \
    "An uncorrectable ECC error detected (possible firmware handling failure) " \
    "DRAM:%d, LTC:%d, MMU:%d, PCIE:%d"

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
    OBJGPU *pGpu
)
{
    RowRemappingCtx *pContext = (RowRemappingCtx *)pContextHeader;

    if (pContext->source == GPU_OPERATIONAL_EVENT_REPORTING_SOURCE_INTERRUPT)
    {
        NV_ERROR_LOG(pGpu, INFOROM_DRAM_RETIREMENT_EVENT, ECC_ROW_REMAP_PENDING_INTR_XID_MESSAGE_FMT,
                     pContext->physicalAddress);
    }
    else
    {
        NV_ERROR_LOG(pGpu, INFOROM_DRAM_RETIREMENT_EVENT,
                     ECC_ROW_REMAP_PENDING_CTRL_CALL_XID_MESSAGE_FMT);
    }
}

static void _eccRowRemappingFailureTableFullEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu
)
{
    RowRemappingFailureCtx *pContext = (RowRemappingFailureCtx *)pContextHeader;

    NV_ERROR_LOG(pGpu, INFOROM_DRAM_RETIREMENT_FAILURE,
                 ECC_ROW_REMAPPING_FAILURE_TABLE_FULL_XID_MESSAGE_FMT,
                 pContext->physicalAddress);
}

static void _eccRowRemappingFailureBankFullEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu
)
{
    RowRemappingFailureCtx *pContext = (RowRemappingFailureCtx *)pContextHeader;

    NV_ERROR_LOG(pGpu, INFOROM_DRAM_RETIREMENT_FAILURE,
                 ECC_ROW_REMAPPING_FAILURE_BANK_FULL_XID_MESSAGE_FMT,
                 pContext->physicalAddress);
}

static void _eccRowRemappingFailureInternalErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu
)
{
    RowRemappingFailureCtx *pContext = (RowRemappingFailureCtx *)pContextHeader;

    NV_ERROR_LOG(pGpu, INFOROM_DRAM_RETIREMENT_FAILURE,
                 ECC_ROW_REMAPPING_FAILURE_INTERNAL_ERROR_XID_MESSAGE_FMT,
                 pContext->physicalAddress,
                 pContext->errorString);
}

static void _eccDramRetirementFailureInternalErrorEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu
)
{
    DramRetirementFailureInternalErrorCtx *pContext =
        (DramRetirementFailureInternalErrorCtx *)pContextHeader;

    switch (pContext->errorReason)
    {
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_INTERNAL_ERROR:
            NV_ERROR_LOG(pGpu, INFOROM_DRAM_RETIREMENT_FAILURE,
                        ECC_DRAM_RETIREMENT_FAILURE_INTERNAL_ERROR_XID_MESSAGE_FMT,
                         pContext->physicalAddress);
            break;
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_DISABLED_RETIREMENT:
            NV_ERROR_LOG(pGpu, INFOROM_DRAM_RETIREMENT_FAILURE,
                        ECC_DRAM_RETIREMENT_FAILURE_DISABLED_RETIREMENT_XID_MESSAGE_FMT,
                         pContext->physicalAddress);
            break;
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_PENDING_ROW_REMAP:
            NV_ERROR_LOG(pGpu, INFOROM_DRAM_RETIREMENT_FAILURE,
                        ECC_DRAM_RETIREMENT_FAILURE_PENDING_ROW_REMAP_XID_MESSAGE_FMT,
                         pContext->physicalAddress);
            break;
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_PENDING_BANK_REMAP:
            NV_ERROR_LOG(pGpu, INFOROM_DRAM_RETIREMENT_FAILURE,
                        ECC_DRAM_RETIREMENT_FAILURE_PENDING_BANK_REMAP_XID_MESSAGE_FMT,
                         pContext->physicalAddress);
            break;
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_PENDING_LTS_SWAP:
            NV_ERROR_LOG(pGpu, INFOROM_DRAM_RETIREMENT_FAILURE,
                        ECC_DRAM_RETIREMENT_FAILURE_PENDING_LTS_SWAP_XID_MESSAGE_FMT,
                         pContext->physicalAddress);
            break;
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_ADDRESS_RETRIEVAL_ERROR:
            NV_ERROR_LOG(pGpu, INFOROM_DRAM_RETIREMENT_FAILURE,
                        ECC_DRAM_RETIREMENT_FAILURE_ADDRESS_RETRIEVAL_ERROR_XID_MESSAGE_FMT,
                         pContext->physicalAddress);
            break;
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_ECC_ADDRESS_TRACKING_ERROR:
            NV_ERROR_LOG(pGpu, INFOROM_DRAM_RETIREMENT_FAILURE,
                        ECC_DRAM_RETIREMENT_FAILURE_ECC_ADDRESS_TRACKING_ERROR_XID_MESSAGE_FMT,
                         pContext->physicalAddress);
            break;
        case ECC_DRAM_RETIREMENT_INTERNAL_ERROR_REASON_UNKNOWN_ERROR:
        default:
            NV_ERROR_LOG(pGpu, INFOROM_DRAM_RETIREMENT_FAILURE,
                        ECC_DRAM_RETIREMENT_FAILURE_UNKNOWN_ERROR_XID_MESSAGE_FMT,
                         pContext->physicalAddress);
            break;
    }
}

static void _eccRowRemappingFailureReservedRowEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu
)
{
    RowRemappingFailureCtx *pContext = (RowRemappingFailureCtx *)pContextHeader;

    NV_ERROR_LOG(pGpu, INFOROM_DRAM_RETIREMENT_FAILURE,
                 ECC_ROW_REMAPPING_FAILURE_RESERVED_ROW_ERROR_XID_MESSAGE_FMT,
                 pContext->physicalAddress);
}

static void _eccRowRemappingPageOfflineFailureEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu
)
{
    RowRemappingFailureCtx *pContext = (RowRemappingFailureCtx *)pContextHeader;

    NV_ERROR_LOG(pGpu, INFOROM_DRAM_RETIREMENT_FAILURE,
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
    OBJGPU *pGpu
)
{
    RowRemappingFailureCtx *pContext = (RowRemappingFailureCtx *)pContextHeader;

    NV_ERROR_LOG(pGpu, INFOROM_DRAM_RETIREMENT_FAILURE,
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
    OBJGPU *pGpu
)
{
    RowRemappingFailureCtx *pContext = (RowRemappingFailureCtx *)pContextHeader;

    NV_ERROR_LOG(pGpu, INFOROM_DRAM_RETIREMENT_FAILURE,
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
    OBJGPU *pGpu
)
{
    RowRemappingFailureCtx *pContext = (RowRemappingFailureCtx *)pContextHeader;

    NV_ERROR_LOG(pGpu, INFOROM_DRAM_RETIREMENT_FAILURE,
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
    OBJGPU *pGpu
)
{
    (void)pContextHeader;

    NV_ERROR_LOG(pGpu, BANK_REMAPPING_EVENT,
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
    OBJGPU *pGpu
)
{
    LtsRepairCtx *pContext = (LtsRepairCtx *)pContextHeader;

    NV_ERROR_LOG(pGpu, CHANNEL_RETIREMENT_EVENT,
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
    OBJGPU *pGpu
)
{
    LtsRepairCtx *pContext = (LtsRepairCtx *)pContextHeader;

    NV_ERROR_LOG(pGpu, CHANNEL_RETIREMENT_EVENT,
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
    OBJGPU *pGpu
)
{
    NV_ERROR_LOG(pGpu, CHANNEL_RETIREMENT_FAILURE,
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
    OBJGPU *pGpu
)
{
    NV_ERROR_LOG(pGpu, CHANNEL_RETIREMENT_FAILURE,
                 ECC_CHANNEL_REPAIR_FAILURE_XID_MESSAGE_FMT);
}

static const EventContextOutputAdapters g_eccChannelRepairFailureOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccChannelRepairFailureEmitOsLogXid,
};

static void _eccTpcRepairPendingSameGpcEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu
)
{
    TpcRepairCtx *pContext = (TpcRepairCtx *)pContextHeader;

    NV_ERROR_LOG(pGpu, RESOURCE_RETIREMENT_EVENT,
                 ECC_TPC_REPAIR_PENDING_SAME_GPC_XID_MESSAGE_FMT,
                 pContext->sublocation,
                 pContext->location);
}

static const EventContextOutputAdapters g_eccTpcRepairPendingSameGpcOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccTpcRepairPendingSameGpcEmitOsLogXid,
};

static void _eccTpcRepairPendingDifferentGpcEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu
)
{
    TpcRepairCtx *pContext = (TpcRepairCtx *)pContextHeader;

    NV_ERROR_LOG(pGpu, RESOURCE_RETIREMENT_EVENT,
                 ECC_TPC_REPAIR_PENDING_DIFFERENT_GPC_XID_MESSAGE_FMT,
                 pContext->sublocation,
                 pContext->location);
}

static const EventContextOutputAdapters g_eccTpcRepairPendingDifferentGpcOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccTpcRepairPendingDifferentGpcEmitOsLogXid,
};

static void _eccTpcRepairFailureNoSpareEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu
)
{
    TpcRepairCtx *pContext = (TpcRepairCtx *)pContextHeader;

    NV_ERROR_LOG(pGpu, RESOURCE_RETIREMENT_FAILURE,
                 ECC_TPC_REPAIR_FAILURE_NO_SPARE_XID_MESSAGE_FMT,
                 pContext->sublocation,
                 pContext->location);
}

static const EventContextOutputAdapters g_eccTpcRepairFailureNoSpareOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccTpcRepairFailureNoSpareEmitOsLogXid,
};

static void _eccTpcRepairFailureNoSpareMigEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu
)
{
    TpcRepairCtx *pContext = (TpcRepairCtx *)pContextHeader;

    NV_ERROR_LOG(pGpu, RESOURCE_RETIREMENT_FAILURE,
                 ECC_TPC_REPAIR_FAILURE_NO_SPARE_MIG_XID_MESSAGE_FMT,
                 pContext->sublocation,
                 pContext->location);
}

static const EventContextOutputAdapters g_eccTpcRepairFailureNoSpareMigOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuEventCtxSerializeToEventBufferGoeOpaqueCtx,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _eccTpcRepairFailureNoSpareMigEmitOsLogXid,
};

static void _eccDramEccIntrStormEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU *pGpu
)
{
    DramEccIntrStormCtx *pContext = (DramEccIntrStormCtx *)pContextHeader;

    NV_ERROR_LOG(pGpu, EXCESSIVE_SBE_INTERRUPTS,
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
    OBJGPU *pGpu
)
{
    (void)pContextHeader;

    NV_ERROR_LOG(pGpu, EXCESSIVE_SBE_INTERRUPTS,
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
    OBJGPU *pGpu
)
{
    EccResidualUncorrectableErrorCtx *pContext =
        (EccResidualUncorrectableErrorCtx *)pContextHeader;

    NV_ERROR_LOG(pGpu, UNRECOVERABLE_ECC_ERROR_ESCAPE,
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

NV_STATUS tpcrepairpendingsamegpcConstruct_IMPL
(
    TpcRepairPendingSameGpc *pEvent,
    EventBus *pEventBus,
    NvU32 tpcId,
    NvU32 gpcId
)
{
    pEvent->context.header.pOutputAdapters = &g_eccTpcRepairPendingSameGpcOutputAdapters;
    pEvent->context.location = gpcId;
    pEvent->context.sublocation = tpcId;
    pEvent->context.header.dataSize = sizeof(pEvent->context.location) +
                                      sizeof(pEvent->context.sublocation);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid156.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid156.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid156.data.message,
                      sizeof(pEvent->xid156.data.message),
                      ECC_TPC_REPAIR_PENDING_SAME_GPC_XID_MESSAGE_FMT,
                      pEvent->context.sublocation,
                      pEvent->context.location) + 1);
    pEvent->xid156.data.xidCode = RESOURCE_RETIREMENT_EVENT;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid156.header);

    return NV_OK;
}

NV_STATUS tpcrepairpendingdifferentgpcConstruct_IMPL
(
    TpcRepairPendingDifferentGpc *pEvent,
    EventBus *pEventBus,
    NvU32 tpcId,
    NvU32 gpcId
)
{
    pEvent->context.header.pOutputAdapters = &g_eccTpcRepairPendingDifferentGpcOutputAdapters;
    pEvent->context.location = gpcId;
    pEvent->context.sublocation = tpcId;
    pEvent->context.header.dataSize = sizeof(pEvent->context.location) +
                                      sizeof(pEvent->context.sublocation);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid156.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid156.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid156.data.message,
                      sizeof(pEvent->xid156.data.message),
                      ECC_TPC_REPAIR_PENDING_DIFFERENT_GPC_XID_MESSAGE_FMT,
                      pEvent->context.sublocation,
                      pEvent->context.location) + 1);
    pEvent->xid156.data.xidCode = RESOURCE_RETIREMENT_EVENT;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid156.header);

    return NV_OK;
}

NV_STATUS tpcrepairfailurenospareConstruct_IMPL
(
    TpcRepairFailureNoSpare *pEvent,
    EventBus *pEventBus,
    NvU32 tpcId,
    NvU32 gpcId
)
{
    pEvent->context.header.pOutputAdapters = &g_eccTpcRepairFailureNoSpareOutputAdapters;
    pEvent->context.location = gpcId;
    pEvent->context.sublocation = tpcId;
    pEvent->context.header.dataSize = sizeof(pEvent->context.location) +
                                      sizeof(pEvent->context.sublocation);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid157.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid157.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid157.data.message,
                      sizeof(pEvent->xid157.data.message),
                      ECC_TPC_REPAIR_FAILURE_NO_SPARE_XID_MESSAGE_FMT,
                      pEvent->context.sublocation,
                      pEvent->context.location) + 1);
    pEvent->xid157.data.xidCode = RESOURCE_RETIREMENT_FAILURE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid157.header);

    return NV_OK;
}

NV_STATUS tpcrepairfailurenosparemigConstruct_IMPL
(
    TpcRepairFailureNoSpareMig *pEvent,
    EventBus *pEventBus,
    NvU32 tpcId,
    NvU32 gpcId
)
{
    pEvent->context.header.pOutputAdapters = &g_eccTpcRepairFailureNoSpareMigOutputAdapters;
    pEvent->context.location = gpcId;
    pEvent->context.sublocation = tpcId;
    pEvent->context.header.dataSize = sizeof(pEvent->context.location) +
                                      sizeof(pEvent->context.sublocation);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid157.header.pOutputAdapters = &g_eccXidOutputAdapters;
    pEvent->xid157.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid157.data.message,
                      sizeof(pEvent->xid157.data.message),
                      ECC_TPC_REPAIR_FAILURE_NO_SPARE_MIG_XID_MESSAGE_FMT,
                      pEvent->context.sublocation,
                      pEvent->context.location) + 1);
    pEvent->xid157.data.xidCode = RESOURCE_RETIREMENT_FAILURE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid157.header);

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

