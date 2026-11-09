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

#include "events/gpu/pcie/pcie_events.h"

#include "cper/gpu_cper.h"
#include "diagnostics/op_event.h"
#include "gpu/gpu.h"
#include "gpu/bif/kernel_bif.h"
#include "nvmisc.h"
#include "nvport/nvport.h"
#include "nverror.h"

/* -------------------------------------------------------------------------
 * XID message format strings.
 * These must remain identical to the pre-existing dmesg strings for XID 167,
 * 176, and 182 so that existing log parsers and tooling are not broken.
 * -------------------------------------------------------------------------
 */
#define PCIE_FATAL_TIMEOUT_XID_MESSAGE_FMT \
    "%s Timeout: Reg 0x%x"
#define PCIE_CORRECTABLE_ERROR_FAULT_XID_MESSAGE_FMT \
    "PCIe_Correctable_Error Threshold %d Exceeded for type 0x%x and version %d"
#define PCIE_RESET_FAILURE_XID_MESSAGE_FMT \
    "PCIE_RESET_FAILURE: Classification: %d, Recommendation: Perform %s"

/* -------------------------------------------------------------------------
 * Error type name strings for XID 167's %s format argument.
 * Indexed by PCIE_FATAL_TIMEOUT_ERROR_TYPE.
 * -------------------------------------------------------------------------
 */
static const char *const g_pcieFatalTimeoutErrorTypeNames[] =
{
    [PCIE_FATAL_ERR_TYPE_NONE]            = "Unknown",
    [PCIE_FATAL_ERR_TYPE_PRI_REQ_TIMEOUT] = "PRI REQ",
    [PCIE_FATAL_ERR_TYPE_PRI_RSP_TIMEOUT] = "PRI RSP",
    [PCIE_FATAL_ERR_TYPE_LMEM_TIMEOUT]    = "LMEM",
    [PCIE_FATAL_ERR_TYPE_TRS_TIMEOUT]     = "TRS",
};

/* -------------------------------------------------------------------------
 * Reset recovery action name strings for XID 182's %s format argument.
 * Indexed by resetRecAction values.
 * -------------------------------------------------------------------------
 */
static const char *const g_pcieResetRecActionNames[] =
{
    [0] = "Unknown",
    [1] = "SBR",
};

/* =========================================================================
 * XID 167 — Fatal PCIe XAL Timeout
 * Adapters: EventBuffer + CPER + OS log XID
 * =========================================================================
 */

static NV_STATUS _pcieFatalTimeoutSerializeToEventBufferOpEventCtx
(
    EventContextHeader           *pContextHeader,
    NV_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32                         outputSize,
    EventBufferOpEventCtxInfo    *pInfo
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(
            pContextHeader,
            pOutput,
            outputSize,
            pInfo,
            PCIE_EVENT_CTX_TYPE_FATAL_TIMEOUT_DATA,
            PCIE_FATAL_TIMEOUT_DATA_VERSION);
}

static void _pcieFatalTimeoutEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU             *pGpu,
    NvBool              bSimulated
)
{
    PcieFatalTimeoutCtx *pContext = (PcieFatalTimeoutCtx *)pContextHeader;
    const char          *pErrorTypeName;

    //
    // Clamp errorType to the name table. Unknown values fall back to a numeric
    // string to ensure the XID line is always well-formed.
    //
    if (pContext->data.errorType < NV_ARRAY_ELEMENTS(g_pcieFatalTimeoutErrorTypeNames))
    {
        pErrorTypeName = g_pcieFatalTimeoutErrorTypeNames[pContext->data.errorType];
    }
    else
    {
        pErrorTypeName = "Unknown Timeout";
    }

    OP_EVENT_XID_LOG(pGpu, bSimulated, PCIE_FATAL_TIMEOUT, PCIE_FATAL_TIMEOUT_XID_MESSAGE_FMT,
                     pErrorTypeName,
                     pContext->data.regVal);
}

//
// XID 167 is fatal: all three output channels are populated.
// emitToOsLogXid uses the structured PcieFatalTimeoutCtx directly so it can
// resolve errorType to a human-readable string. The xid167 GpuXidEventCtx
// carries the pre-rendered string for EventBuffer and CPER legacy XID contexts.
//
static const EventContextOutputAdapters g_pcieFatalTimeoutOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = _pcieFatalTimeoutSerializeToEventBufferOpEventCtx,
    .serializeToCperEventCtx          = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid                   = _pcieFatalTimeoutEmitOsLogXid,
};

static const EventContextOutputAdapters g_xid167OutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuXidEventCtxSerializeToEventBufferGoeCtx,
    .serializeToCperEventCtx          = gpuXidEventCtxSerializeToCperCtx,
    .emitToOsLogXid                   = NULL,
};

NV_STATUS pciefataltimeoutConstruct_IMPL
(
    PcieFatalTimeout *pEvent,
    EventBus        *pEventBus,
    NvU32            errorType,
    NvU32            regVal
)
{
    const char *pErrorTypeName;

    // Populate and register the PCIE-specific fatal timeout context.
    pEvent->context.header.pOutputAdapters = &g_pcieFatalTimeoutOutputAdapters;
    pEvent->context.header.dataSize        = sizeof(pEvent->context.data);
    pEvent->context.data.errorType         = errorType;
    pEvent->context.data.regVal            = regVal;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    //
    // Pre-render the XID 167 message string into the GpuXidEventCtx so that
    // the EventBuffer and CPER serializers can emit a legacy XID context
    // without re-accessing the raw register data.
    //
    if (errorType < NV_ARRAY_ELEMENTS(g_pcieFatalTimeoutErrorTypeNames))
    {
        pErrorTypeName = g_pcieFatalTimeoutErrorTypeNames[errorType];
    }
    else
    {
        pErrorTypeName = "Unknown Timeout";
    }

    pEvent->xid167.header.pOutputAdapters = &g_xid167OutputAdapters;
    pEvent->xid167.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid167.data.message,
                      sizeof(pEvent->xid167.data.message),
                      PCIE_FATAL_TIMEOUT_XID_MESSAGE_FMT,
                      pErrorTypeName,
                      pEvent->context.data.regVal) + 1);
    pEvent->xid167.data.xidCode = PCIE_FATAL_TIMEOUT;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid167.header);

    return NV_OK;
}

/* =========================================================================
 * XID 176 — Correctable PCIe Error Fault
 * Adapters: EventBuffer + OS log XID (no CPER — error is self-corrected)
 * =========================================================================
 */

static NV_STATUS _pcieCorrectableErrorFaultSerializeToEventBufferOpEventCtx
(
    EventContextHeader           *pContextHeader,
    NV_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32                         outputSize,
    EventBufferOpEventCtxInfo    *pInfo
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(
            pContextHeader,
            pOutput,
            outputSize,
            pInfo,
            PCIE_EVENT_CTX_TYPE_CORRECTABLE_ERROR_FAULT_DATA,
            PCIE_CORRECTABLE_ERROR_FAULT_DATA_VERSION);
}

static void _pcieCorrectableErrorFaultEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU             *pGpu,
    NvBool              bSimulated
)
{
    PcieCorrectableErrorFaultCtx *pContext = (PcieCorrectableErrorFaultCtx *)pContextHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, PCIE_CORRECTABLE_ERROR_FAULT,
                     PCIE_CORRECTABLE_ERROR_FAULT_XID_MESSAGE_FMT,
                     pContext->data.ceRateTupleNum,
                     pContext->data.ceRateNerdNumber,
                     pContext->data.ceRateTupleVerNum);
}

//
// XID 176 is correctable: CPER is omitted since the error is self-healing
// and does not require firmware-level persistence across a crash or reboot.
// EventBuffer is retained for trend analysis of correctable error rates.
//
static const EventContextOutputAdapters g_pcieCorrectableErrorFaultOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = _pcieCorrectableErrorFaultSerializeToEventBufferOpEventCtx,
    .serializeToCperEventCtx          = NULL,
    .emitToOsLogXid                   = _pcieCorrectableErrorFaultEmitOsLogXid,
};

static const EventContextOutputAdapters g_xid176OutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuXidEventCtxSerializeToEventBufferGoeCtx,
    .serializeToCperEventCtx          = NULL,
    .emitToOsLogXid                   = NULL,
};

NV_STATUS pciecorrecterrConstruct_IMPL
(
    PcieCorrectableErrorFault *pEvent,
    EventBus                 *pEventBus,
    NvU32                     ceRateTupleNum,
    NvU32                     ceRateNerdNumber,
    NvU32                     ceRateTupleVerNum
)
{
    // Populate and register the PCIE-specific correctable error context.
    pEvent->context.header.pOutputAdapters       = &g_pcieCorrectableErrorFaultOutputAdapters;
    pEvent->context.header.dataSize              = sizeof(pEvent->context.data);
    pEvent->context.data.ceRateTupleNum          = ceRateTupleNum;
    pEvent->context.data.ceRateNerdNumber        = ceRateNerdNumber;
    pEvent->context.data.ceRateTupleVerNum       = ceRateTupleVerNum;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    //
    // Pre-render the XID 176 message string for the EventBuffer legacy XID
    // context. No CPER XID context is needed since serializeToCperEventCtx
    // is NULL for this event.
    //
    pEvent->xid176.header.pOutputAdapters = &g_xid176OutputAdapters;
    pEvent->xid176.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid176.data.message,
                      sizeof(pEvent->xid176.data.message),
                      PCIE_CORRECTABLE_ERROR_FAULT_XID_MESSAGE_FMT,
                      pEvent->context.data.ceRateTupleNum,
                      pEvent->context.data.ceRateNerdNumber,
                      pEvent->context.data.ceRateTupleVerNum) + 1);
    pEvent->xid176.data.xidCode = PCIE_CORRECTABLE_ERROR_FAULT;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid176.header);

    return NV_OK;
}

/* =========================================================================
 * XID 182 — PCIe Reset Failure
 * Adapters: EventBuffer + CPER + OS log XID
 * =========================================================================
 */

static NV_STATUS _pcieResetFailureSerializeToEventBufferOpEventCtx
(
    EventContextHeader           *pContextHeader,
    NV_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32                         outputSize,
    EventBufferOpEventCtxInfo    *pInfo
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(
            pContextHeader,
            pOutput,
            outputSize,
            pInfo,
            PCIE_EVENT_CTX_TYPE_RESET_FAILURE_DATA,
            PCIE_RESET_FAILURE_DATA_VERSION);
}

static void _pcieResetFailureEmitOsLogXid
(
    EventContextHeader *pContextHeader,
    OBJGPU             *pGpu,
    NvBool              bSimulated
)
{
    PcieResetFailureCtx *pContext = (PcieResetFailureCtx *)pContextHeader;
    const char         *pRecActionName;

    if (pContext->data.resetRecAction < NV_ARRAY_ELEMENTS(g_pcieResetRecActionNames))
    {
        pRecActionName = g_pcieResetRecActionNames[pContext->data.resetRecAction];
    }
    else
    {
        pRecActionName = "Unknown";
    }

    OP_EVENT_XID_LOG(pGpu, bSimulated, PCIE_RESET_FAILURE, PCIE_RESET_FAILURE_XID_MESSAGE_FMT,
                     pContext->data.resetStatus,
                     pRecActionName);
}

//
// XID 182 is fatal: all three output channels are populated.
// CPER is critical here — a reset failure may leave the system unstable, and
// the CPER record ensures the failure is preserved across a subsequent crash.
//
static const EventContextOutputAdapters g_pcieResetFailureOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = _pcieResetFailureSerializeToEventBufferOpEventCtx,
    .serializeToCperEventCtx          = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid                   = _pcieResetFailureEmitOsLogXid,
};

static const EventContextOutputAdapters g_xid182OutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuXidEventCtxSerializeToEventBufferGoeCtx,
    .serializeToCperEventCtx          = gpuXidEventCtxSerializeToCperCtx,
    .emitToOsLogXid                   = NULL,
};

NV_STATUS pcieresetfailureConstruct_IMPL
(
    PcieResetFailure *pEvent,
    EventBus        *pEventBus,
    NvU32            resetStatus,
    NvU32            resetRecAction
)
{
    const char *pRecActionName;

    // Populate and register the PCIE-specific reset failure context.
    pEvent->context.header.pOutputAdapters  = &g_pcieResetFailureOutputAdapters;
    pEvent->context.header.dataSize         = sizeof(pEvent->context.data);
    pEvent->context.data.resetStatus        = resetStatus;
    pEvent->context.data.resetRecAction     = resetRecAction;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    //
    // Pre-render the XID 182 message string into the GpuXidEventCtx for
    // EventBuffer and CPER legacy XID context output.
    //
    if (resetRecAction < NV_ARRAY_ELEMENTS(g_pcieResetRecActionNames))
    {
        pRecActionName = g_pcieResetRecActionNames[resetRecAction];
    }
    else
    {
        pRecActionName = "Unknown";
    }

    pEvent->xid182.header.pOutputAdapters = &g_xid182OutputAdapters;
    pEvent->xid182.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid182.data.message,
                      sizeof(pEvent->xid182.data.message),
                      PCIE_RESET_FAILURE_XID_MESSAGE_FMT,
                      pEvent->context.data.resetStatus,
                      pRecActionName) + 1);
    pEvent->xid182.data.xidCode = PCIE_RESET_FAILURE;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid182.header);

    return NV_OK;
}

