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

#include "events/gpu/nvlink/nvlink_events.h"

#include "cper/gpu_cper.h"
#include "diagnostics/op_event.h"
#include "gpu/gpu.h"
#include "gpu/nvlink/kernel_nvlink.h"
#include "nvmisc.h"
#include "nverror.h"
#include "nvport/nvport.h"

#define NVLINK_ALI_TRAINING_FAILURE_XID_MESSAGE_FMT \
    "NVLink: Link training failed for link %u(0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x)\n"
#define NVLINK_ALI_TRAINING_FAILURE_BW_XID_MESSAGE_FMT \
    "NVLink: Link training failed for links 0x%llx(0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x)\n"
#define NVLINK_MSE_LEGACY_XID_MESSAGE_FMT \
    "%s %s XC%01u i%01u Link %02d (0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x)\n"
#define NVLINK_SW_LINK_DOWN_LEGACY_XID_MESSAGE_FMT \
    "%s %s XC%01u i%01u Link %02u : Link Down While Active.\n"
#define NVLINK_FATAL_ERROR_LEGACY_XID_MESSAGE_FMT \
    "NVLink: fatal error detected on link %u" \
            "(0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x)"
#define NVLINK_PRIV_ERROR_WARNING_XID_MESSAGE_FMT \
    "RLW_RXPIPE interrupt hit on link %d on GPU%d: PRIV Error\n"
#define NVLINK_BLACKWELL_LEGACY_ERROR_XID_MESSAGE_FMT \
    "%s %s XC%01u i%01u Link %02d (0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x)\n"

static const char *
_nvlinkMseLegacyXidErrorNameToString
(
    NVLINK_MSE_LEGACY_XID_ERROR_NAME legacyXidErrorName
)
{
    switch (legacyXidErrorName)
    {
        case NVLINK_MSE_LEGACY_XID_ERROR_NAME_WATCHDOG:
            return "MSE_WATCHDOG";
        case NVLINK_MSE_LEGACY_XID_ERROR_NAME_DEGRADED:
            return "MSE degraded";
        default:
            NV_ASSERT_FAILED("Unexpected NVLink MSE legacy XID error name");
            return "MSE";
    }
}

static NV_STATUS
_nvlinkAliTrainingFailureSerializeToEventBuffer
(
    EventContextHeader *pCtxHeader,
    NV_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32 outputSize,
    EventBufferOpEventCtxInfo *pInfo
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(
        pCtxHeader,
        pOutput,
        outputSize,
        pInfo,
        NVLINK_EVENT_CTX_TYPE_ALI_TRAINING_FAILURE_DATA,
        NVLINK_ALI_TRAINING_FAILURE_DATA_VERSION);
}

static void
_nvlinkAliTrainingFailureEmitOsLogXid
(
    EventContextHeader *pCtxHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    NvlinkAliTrainingFailureLegacyCtx *pContext =
        (NvlinkAliTrainingFailureLegacyCtx *)pCtxHeader;
    NvU32 entryCount = pContext->data.entryCount;
    NvU32 debugDataCount = pContext->data.debugDataCount;
    NvU32 entry;

    //
    // If there is only one debug data word per link, print the legacy XID message format.
    // where it provides a link mask and the first 7 links of debug data.
    //
    if (debugDataCount == 1)
    {
        OP_EVENT_XID_LOG(pGpu, bSimulated, ALI_TRAINING_FAIL, NVLINK_ALI_TRAINING_FAILURE_BW_XID_MESSAGE_FMT,
                     pContext->data.legacyLinkMask,
                     pContext->data.linkErrorInfo[0].debugData[0],
                     pContext->data.linkErrorInfo[1].debugData[0],
                     pContext->data.linkErrorInfo[2].debugData[0],
                     pContext->data.linkErrorInfo[3].debugData[0],
                     pContext->data.linkErrorInfo[4].debugData[0],
                     pContext->data.linkErrorInfo[5].debugData[0],
                     pContext->data.linkErrorInfo[6].debugData[0]);

            return;
    }

    for (entry = 0; entry < entryCount; entry++)
    {
        NVLINK_ALI_TRAINING_FAILURE_ENTRY *pLinkErrorInfo =
            &pContext->data.linkErrorInfo[entry];
        NvU32 debugData[NVLINK_ALI_TRAINING_FAILURE_MAX_DEBUG_WORDS] = { 0 };
        NvU32 word;

        for (word = 0; word < debugDataCount; word++)
        {
            debugData[word] = pLinkErrorInfo->debugData[word];
        }

        OP_EVENT_XID_LOG(pGpu, bSimulated, ALI_TRAINING_FAIL, NVLINK_ALI_TRAINING_FAILURE_XID_MESSAGE_FMT,
                     pLinkErrorInfo->linkId,
                     debugData[0],
                     debugData[1],
                     debugData[2],
                     debugData[3],
                     debugData[4],
                     debugData[5],
                     debugData[6]);
    }
}

static NV_STATUS
_nvlinkMseErrorSerializeToEventBuffer
(
    EventContextHeader *pCtxHeader,
    NV_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32 outputSize,
    EventBufferOpEventCtxInfo *pInfo
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(
        pCtxHeader,
        pOutput,
        outputSize,
        pInfo,
        NVLINK_EVENT_CTX_TYPE_MSE_ERROR_DATA,
        NVLINK_MSE_ERROR_DATA_VERSION);
}

static void
_nvlinkMseErrorEmitOsLogXid
(
    EventContextHeader *pCtxHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    NvlinkMseErrorLegacyCtx *pContext = (NvlinkMseErrorLegacyCtx *)pCtxHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, pContext->data.legacyXidCode, NVLINK_MSE_LEGACY_XID_MESSAGE_FMT,
                 _nvlinkMseLegacyXidErrorNameToString(pContext->data.legacyXidErrorName),
                 pContext->data.bLegacyFatal ? " Fatal  " : "Nonfatal",
                 pContext->data.bLegacyXContain,
                 pContext->data.bLegacyInjected,
                 pContext->data.legacyLinkId,
                 pContext->data.legacyIntrInfo,
                 pContext->data.legacyErrorStatus,
                 pContext->data.debugData[0],
                 pContext->data.debugData[1],
                 pContext->data.debugData[2],
                 pContext->data.debugData[3]);
}

static NV_STATUS
_nvlinkSwLinkDownSerializeToEventBuffer
(
    EventContextHeader *pCtxHeader,
    NV_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32 outputSize,
    EventBufferOpEventCtxInfo *pInfo
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(
        pCtxHeader,
        pOutput,
        outputSize,
        pInfo,
        NVLINK_EVENT_CTX_TYPE_SW_LINK_DOWN_ERROR_DATA,
        NVLINK_SW_LINK_DOWN_ERROR_DATA_VERSION);
}

static NV_STATUS
_nvlinkBlackwellErrorSerializeToEventBuffer
(
    EventContextHeader *pCtxHeader,
    NV_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32 outputSize,
    EventBufferOpEventCtxInfo *pInfo
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(
        pCtxHeader,
        pOutput,
        outputSize,
        pInfo,
        NVLINK_EVENT_CTX_TYPE_BLACKWELL_ERROR_DATA,
        NVLINK_BLACKWELL_ERROR_DATA_VERSION);
}

static NV_STATUS
_nvlinkSawReservedDataSerializeToEventBuffer
(
    EventContextHeader *pCtxHeader,
    NV_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32 outputSize,
    EventBufferOpEventCtxInfo *pInfo
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(
        pCtxHeader,
        pOutput,
        outputSize,
        pInfo,
        NVLINK_EVENT_CTX_TYPE_SAW_ERROR_DATA,
        NVLINK_SAW_ERROR_DATA_VERSION);
}

static NV_STATUS
_nvlinkRlwReservedDataSerializeToEventBuffer
(
    EventContextHeader *pCtxHeader,
    NV_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32 outputSize,
    EventBufferOpEventCtxInfo *pInfo
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(
        pCtxHeader,
        pOutput,
        outputSize,
        pInfo,
        NVLINK_EVENT_CTX_TYPE_RLW_ERROR_DATA,
        NVLINK_RLW_ERROR_DATA_VERSION);
}

static NV_STATUS
_nvlinkTlwReservedDataSerializeToEventBuffer
(
    EventContextHeader *pCtxHeader,
    NV_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32 outputSize,
    EventBufferOpEventCtxInfo *pInfo
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(
        pCtxHeader,
        pOutput,
        outputSize,
        pInfo,
        NVLINK_EVENT_CTX_TYPE_TLW_ERROR_DATA,
        NVLINK_TLW_ERROR_DATA_VERSION);
}
static NV_STATUS
_nvlinkNvlpwCtrlReservedDataSerializeToEventBuffer
(
    EventContextHeader *pCtxHeader,
    NV_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32 outputSize,
    EventBufferOpEventCtxInfo *pInfo
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(
        pCtxHeader,
        pOutput,
        outputSize,
        pInfo,
        NVLINK_EVENT_CTX_TYPE_NVLPW_CTRL_ERROR_DATA,
        NVLINK_NVLPW_CTRL_ERROR_DATA_VERSION);
}

static NV_STATUS
_nvlinkTrexReservedDataSerializeToEventBuffer
(
    EventContextHeader *pCtxHeader,
    NV_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32 outputSize,
    EventBufferOpEventCtxInfo *pInfo
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(
        pCtxHeader,
        pOutput,
        outputSize,
        pInfo,
        NVLINK_EVENT_CTX_TYPE_TREX_ERROR_DATA,
        NVLINK_TREX_ERROR_DATA_VERSION);
}

static NV_STATUS
_nvlinkNetirReservedDataSerializeToEventBuffer
(
    EventContextHeader *pCtxHeader,
    NV_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32 outputSize,
    EventBufferOpEventCtxInfo *pInfo
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(
        pCtxHeader,
        pOutput,
        outputSize,
        pInfo,
        NVLINK_EVENT_CTX_TYPE_NETIR_ERROR_DATA,
        NVLINK_NETIR_ERROR_DATA_VERSION);
}

static void
_nvlinkSwLinkDownEmitOsLogXid
(
    EventContextHeader *pCtxHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    NvlinkSwLinkDownLegacyCtx *pContext = (NvlinkSwLinkDownLegacyCtx *)pCtxHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, pContext->data.legacyXidCode, NVLINK_SW_LINK_DOWN_LEGACY_XID_MESSAGE_FMT,
                 "NVLINK_SW_DEFINED_ERROR",
                 pContext->data.bLegacyFatal ? " Fatal  " : "Nonfatal",
                 pContext->data.bLegacyXContain,
                 pContext->data.bLegacyInjected,
                 pContext->data.linkId);
}

static NV_STATUS
_nvlinkFatalErrorNvl3SerializeToEventBuffer
(
    EventContextHeader *pCtxHeader,
    NV_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32 outputSize,
    EventBufferOpEventCtxInfo *pInfo
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(
        pCtxHeader,
        pOutput,
        outputSize,
        pInfo,
        NVLINK_EVENT_CTX_TYPE_FATAL_ERROR_NVL3_DATA,
        NVLINK_FATAL_ERROR_DATA_NVL3_VERSION);
}

static void
_nvlinkFatalErrorNvl3EmitOsLogXid
(
    EventContextHeader *pCtxHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    NvlinkFatalErrorNvl3LegacyCtx *pContext = (NvlinkFatalErrorNvl3LegacyCtx *)pCtxHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, NVLINK_ERROR, NVLINK_FATAL_ERROR_LEGACY_XID_MESSAGE_FMT,
                 pContext->data.linkId,
                 pContext->data.dword0,
                 pContext->data.dword1,
                 pContext->data.dword2,
                 pContext->data.dword3,
                 pContext->data.dword4,
                 pContext->data.dword5,
                 pContext->data.dword6);
}

NV_STATUS
_nvlinkPrivErrorSerializeToEventBuffer
(
    EventContextHeader *pCtxHeader,
    NV_OPERATIONAL_EVENT_CONTEXT *pOutput,
    NvU32 outputSize,
    EventBufferOpEventCtxInfo *pInfo
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(
        pCtxHeader,
        pOutput,
        outputSize,
        pInfo,
        NVLINK_EVENT_CTX_TYPE_PRIV_ERROR_DATA,
        NVLINK_PRIV_ERROR_DATA_VERSION);
}

static void 
_nvlinkPrivErrorEmitOsLogXid
(
    EventContextHeader *pCtxHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    NvlinkPrivErrorCtx *pContext = (NvlinkPrivErrorCtx *)pCtxHeader;

    OP_EVENT_XID_LOG(pGpu, bSimulated, NVLINK_PRIV_ERR, NVLINK_PRIV_ERROR_WARNING_XID_MESSAGE_FMT,
                     pContext->data.linkId,
                     pGpu->gpuInstance);
}

static const EventContextOutputAdapters g_nvlinkAliTrainingFailureCtxOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = _nvlinkAliTrainingFailureSerializeToEventBuffer,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = NULL,
};

static const EventContextOutputAdapters g_nvlinkAliTrainingFailureLegacyCtxOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = NULL,
    .serializeToCperEventCtx = NULL,
    .emitToOsLogXid = _nvlinkAliTrainingFailureEmitOsLogXid,
};

static const EventContextOutputAdapters g_nvlinkMseErrorCtxOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = _nvlinkMseErrorSerializeToEventBuffer,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = NULL,
};

static const EventContextOutputAdapters g_nvlinkMseErrorLegacyCtxOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = NULL,
    .serializeToCperEventCtx = NULL,
    .emitToOsLogXid = _nvlinkMseErrorEmitOsLogXid,
};

static const EventContextOutputAdapters g_nvlinkSwLinkDownCtxOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = _nvlinkSwLinkDownSerializeToEventBuffer,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = NULL,
};

static const EventContextOutputAdapters g_nvlinkSwLinkDownLegacyCtxOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = NULL,
    .serializeToCperEventCtx = NULL,
    .emitToOsLogXid = _nvlinkSwLinkDownEmitOsLogXid,
};

static const EventContextOutputAdapters g_nvlinkFatalErrorNvl3CtxOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = _nvlinkFatalErrorNvl3SerializeToEventBuffer,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = NULL,
};

static const EventContextOutputAdapters g_nvlinkFatalErrorNvl3LegacyCtxOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = NULL,
    .serializeToCperEventCtx = NULL,
    .emitToOsLogXid = _nvlinkFatalErrorNvl3EmitOsLogXid,
};

static const EventContextOutputAdapters g_nvlinkPrivErrorCtxOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = _nvlinkPrivErrorSerializeToEventBuffer,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = _nvlinkPrivErrorEmitOsLogXid,
};

static const EventContextOutputAdapters g_nvlinkPrivErrorXidOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = gpuXidEventCtxSerializeToEventBufferGoeCtx,
    .serializeToCperEventCtx = gpuXidEventCtxSerializeToCperCtx,
    .emitToOsLogXid = NULL,
};

NV_STATUS
nvlinkalitrainfailConstruct_IMPL
(
    NvlinkAliTrainingFailure *pEvent,
    EventBus *pEventBus,
    NvU32 severity,
    NvU32 entryCount,
    NvU32 debugDataCount,
    const NvU8 *pLinkIds,
    const NvU32 *pDebugData
)
{
    NvU32 entry;
    NvU32 word;
    NvU32 clampedEntryCount =
        (entryCount < NVLINK_ALI_TRAINING_FAILURE_MAX_ENTRIES) ?
            entryCount : NVLINK_ALI_TRAINING_FAILURE_MAX_ENTRIES;
    NvU32 clampedDebugDataCount =
        (debugDataCount < NVLINK_ALI_TRAINING_FAILURE_MAX_DEBUG_WORDS) ?
            debugDataCount : NVLINK_ALI_TRAINING_FAILURE_MAX_DEBUG_WORDS;

    pEvent->context.header.pOutputAdapters = &g_nvlinkAliTrainingFailureCtxOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.data);
    portMemSet(&pEvent->context.data, 0, sizeof(pEvent->context.data));
    pEvent->context.data.entryCount = (NvU8)clampedEntryCount;
    pEvent->context.data.debugDataCount = (NvU8)clampedDebugDataCount;

    for (entry = 0; entry < clampedEntryCount; entry++)
    {
        pEvent->context.data.linkErrorInfo[entry].linkId =
            (pLinkIds != NULL) ? pLinkIds[entry] : 0;

        for (word = 0; word < clampedDebugDataCount; word++)
        {
            pEvent->context.data.linkErrorInfo[entry].debugData[word] =
                (pDebugData != NULL) ?
                    pDebugData[(entry * clampedDebugDataCount) + word] :
                    0;
        }
    }
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    return NV_OK;
}

NV_STATUS
nvlinkalitrainfaillegacyConstruct_IMPL
(
    NvlinkAliTrainingFailureLegacy *pEvent,
    EventBus *pEventBus,
    NvU32 severity,
    NvU32 entryCount,
    NvU32 debugDataCount,
    const NvU8 *pLinkIds,
    const NvU32 *pDebugData,
    NvU32 xidCode
)
{
    NvU32 entry;
    NvU32 word;

    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        debugDataCount <= NVLINK_ALI_TRAINING_FAILURE_MAX_DEBUG_WORDS,
        NV_ERR_INVALID_ARGUMENT);

    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        entryCount <= NVLINK_ALI_TRAINING_FAILURE_MAX_ENTRIES,
        NV_ERR_INVALID_ARGUMENT);

    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (pDebugData != NULL && pLinkIds != NULL),
        NV_ERR_INVALID_ARGUMENT);

    pEvent->context.header.pOutputAdapters =
        &g_nvlinkAliTrainingFailureLegacyCtxOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.data);
    portMemSet(&pEvent->context.data, 0, sizeof(pEvent->context.data));
    pEvent->context.data.entryCount = (NvU8)entryCount;
    pEvent->context.data.debugDataCount = (NvU8)debugDataCount;
    pEvent->context.data.legacyXidCode = xidCode;
    pEvent->context.data.legacyLinkMask = 0;

    for (entry = 0; entry < entryCount; entry++)
    {
        pEvent->context.data.linkErrorInfo[entry].linkId = pLinkIds[entry];
        pEvent->context.data.legacyLinkMask |= NVBIT64(pEvent->context.data.linkErrorInfo[entry].linkId);

        for (word = 0; word < debugDataCount; word++)
        {
            pEvent->context.data.linkErrorInfo[entry].debugData[word] =
                pDebugData[(entry * debugDataCount) + word];
        }
    }
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    return NV_OK;
}

NV_STATUS
nvlinkmseerrorConstruct_IMPL
(
    NvlinkMseError *pEvent,
    EventBus *pEventBus,
    NvU32 eventCode,
    NvU32 severity,
    NvU32 errorStatus,
    const NvU32 *pDebugData
)
{
    NvU32 word;

    pEvent->context.header.pOutputAdapters = &g_nvlinkMseErrorCtxOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.data);
    portMemSet(&pEvent->context.data, 0, sizeof(pEvent->context.data));
    pEvent->context.data.errorStatus = errorStatus;

    for (word = 0; word < NVLINK_MSE_ERROR_DEBUG_WORDS; word++)
    {
        pEvent->context.data.debugData[word] = (pDebugData != NULL) ? pDebugData[word] : 0;
    }

    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    return NV_OK;
}

NV_STATUS
nvlinkmseerrorlegacyConstruct_IMPL
(
    NvlinkMseErrorLegacy *pEvent,
    EventBus *pEventBus,
    NvU32 eventCode,
    NvU32 severity,
    const NvU32 *pDebugData,
    NvU32 xidCode,
    NVLINK_MSE_LEGACY_XID_ERROR_NAME legacyXidErrorName,
    NvBool bFatal,
    NvBool bXContain,
    NvBool bInjected,
    NvS32 linkId,
    NvU32 intrInfo,
    NvU32 legacyErrorStatus
)
{
    NvU32 word;

    pEvent->context.header.pOutputAdapters = &g_nvlinkMseErrorLegacyCtxOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.data);
    portMemSet(&pEvent->context.data, 0, sizeof(pEvent->context.data));
    pEvent->context.data.legacyXidErrorName = legacyXidErrorName;
    pEvent->context.data.bLegacyFatal = bFatal;
    pEvent->context.data.bLegacyXContain = bXContain;
    pEvent->context.data.bLegacyInjected = bInjected;
    pEvent->context.data.legacyLinkId = linkId;
    pEvent->context.data.legacyIntrInfo = intrInfo;
    pEvent->context.data.legacyErrorStatus = legacyErrorStatus;
    pEvent->context.data.legacyXidCode = xidCode;

    for (word = 0; word < NVLINK_MSE_ERROR_DEBUG_WORDS; word++)
    {
        pEvent->context.data.debugData[word] = (pDebugData != NULL) ? pDebugData[word] : 0;
    }

    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    return NV_OK;
}

NV_STATUS
nvlinkswlinkdownConstruct_IMPL
(
    NvlinkSwLinkDown *pEvent,
    EventBus *pEventBus,
    NvU32 linkId,
    const NvU32 *pDebugData
)
{
    NvU32 word;

    pEvent->context.header.pOutputAdapters = &g_nvlinkSwLinkDownCtxOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.data);
    portMemSet(&pEvent->context.data, 0, sizeof(pEvent->context.data));
    pEvent->context.data.linkId = (NvU8)linkId;

    for (word = 0; word < NVLINK_SW_LINK_DOWN_ERROR_DEBUG_WORDS; word++)
        pEvent->context.data.debugData[word] = (pDebugData != NULL) ? pDebugData[word] : 0;

    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    return NV_OK;
}

NV_STATUS
nvlinkswlinkdownlegacyConstruct_IMPL
(
    NvlinkSwLinkDownLegacy *pEvent,
    EventBus *pEventBus,
    NvU32 linkId,
    NvU32 xidCode,
    NvBool bFatal,
    NvBool bXContain,
    NvBool bInjected
)
{
    pEvent->context.header.pOutputAdapters = &g_nvlinkSwLinkDownLegacyCtxOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.data);
    portMemSet(&pEvent->context.data, 0, sizeof(pEvent->context.data));
    pEvent->context.data.bLegacyFatal = bFatal;
    pEvent->context.data.bLegacyXContain = bXContain;
    pEvent->context.data.bLegacyInjected = bInjected;
    pEvent->context.data.legacyXidCode = xidCode;
    pEvent->context.data.linkId = (NvU8)linkId;

    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    return NV_OK;
}

NV_STATUS
nvlinkfatalerrornvl3Construct_IMPL
(
    NvlinkFatalErrorNvl3 *pEvent,
    EventBus *pEventBus,
    NvU32 linkId,
    NvU32 debugDataCount,
    const NvU32 *pDebugData
)
{
    NvU32 word;

    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (pDebugData != NULL) && (debugDataCount <= NVLINK_FATAL_ERROR_DATA_NVL3_DEBUG_WORDS),
        NV_ERR_INVALID_ARGUMENT);

    pEvent->context.header.pOutputAdapters = &g_nvlinkFatalErrorNvl3CtxOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.data);
    portMemSet(&pEvent->context.data, 0, sizeof(pEvent->context.data));
    pEvent->context.data.linkId = (NvU8)linkId;

    for (word = 0; word < debugDataCount; word++)
    {
        pEvent->context.data.debugData[word] = pDebugData[word];
    }

    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    return NV_OK;
}

NV_STATUS
nvlinkfatalerrornvl3legacyConstruct_IMPL
(
    NvlinkFatalErrorNvl3Legacy *pEvent,
    EventBus *pEventBus,
    NvU32 linkId,
    NvU32 dword0,
    NvU32 dword1,
    NvU32 dword2,
    NvU32 dword3,
    NvU32 dword4,
    NvU32 dword5,
    NvU32 dword6
)
{
    pEvent->context.header.pOutputAdapters = &g_nvlinkFatalErrorNvl3LegacyCtxOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.data);
    portMemSet(&pEvent->context.data, 0, sizeof(pEvent->context.data));
    pEvent->context.data.dword0 = dword0;
    pEvent->context.data.dword1 = dword1;
    pEvent->context.data.dword2 = dword2;
    pEvent->context.data.dword3 = dword3;
    pEvent->context.data.dword4 = dword4;
    pEvent->context.data.dword5 = dword5;
    pEvent->context.data.dword6 = dword6;
    pEvent->context.data.linkId = (NvU8)linkId;

    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    return NV_OK;
}

NV_STATUS
nvlinkpriverrorConstruct_IMPL
(
    NvlinkPrivError *pEvent,
    EventBus *pEventBus,
    NvU32 linkId,
    NvU32 gpuId
)
{
    pEvent->context.header.pOutputAdapters = &g_nvlinkPrivErrorCtxOutputAdapters;
    pEvent->context.header.dataSize = sizeof(pEvent->context.data);
    portMemSet(&pEvent->context.data, 0, sizeof(pEvent->context.data));
    pEvent->context.data.linkId = (NvU8)linkId;

    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    pEvent->xid137.header.pOutputAdapters = &g_nvlinkPrivErrorXidOutputAdapters;
    pEvent->xid137.header.dataSize = (NvU32)GPU_OPERATIONAL_EVENT_CTX_GPU_LEGACY_XID_SIZE(
        nvDbgSnprintf(pEvent->xid137.data.message,
                      sizeof(pEvent->xid137.data.message),
                      NVLINK_PRIV_ERROR_WARNING_XID_MESSAGE_FMT,
                      linkId,
                      gpuId) + 1);
    pEvent->xid137.data.xidCode = NVLINK_PRIV_ERR;
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->xid137.header);

    return NV_OK;
}

static void
_nvlinkBlackwellLegacyErrorEmitOsLogXid
(
    EventContextHeader *pCtxHeader,
    OBJGPU *pGpu,
    NvBool bSimulated
)
{
    NvlinkBlackwellLegacyErrorCtx *pContext =
        (NvlinkBlackwellLegacyErrorCtx *)pCtxHeader;

   OP_EVENT_XID_LOG(pGpu, bSimulated, pContext->data.legacyXidCode, NVLINK_BLACKWELL_LEGACY_ERROR_XID_MESSAGE_FMT,
                pContext->data.pLegacyXidErrorName,
                pContext->data.pSeverityString,
                pContext->data.bLegacyXContain,
                pContext->data.bLegacyInjected,
                pContext->data.legacyLinkId,
                pContext->data.legacyIntrInfo,
                pContext->data.legacyErrorStatus,
                pContext->data.debugData[0],
                pContext->data.debugData[1],
                pContext->data.debugData[2],
                pContext->data.debugData[3]);

}

static const EventContextOutputAdapters g_nvlinkBlackwellLegacyErrorCtxOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = NULL,
    .serializeToCperEventCtx = NULL,
    .emitToOsLogXid = _nvlinkBlackwellLegacyErrorEmitOsLogXid,
};

static const EventContextOutputAdapters g_nvlinkBlackwellErrorCtxOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = _nvlinkBlackwellErrorSerializeToEventBuffer,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = NULL,
};

static const EventContextOutputAdapters g_nvlinkSawReservedDataCtxOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = _nvlinkSawReservedDataSerializeToEventBuffer,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = NULL,
};

static const EventContextOutputAdapters g_nvlinkRlwReservedDataCtxOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = _nvlinkRlwReservedDataSerializeToEventBuffer,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = NULL,
};

static const EventContextOutputAdapters g_nvlinkTlwReservedDataCtxOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = _nvlinkTlwReservedDataSerializeToEventBuffer,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = NULL,
};

static const EventContextOutputAdapters g_nvlinkNvlpwCtrlReservedDataCtxOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = _nvlinkNvlpwCtrlReservedDataSerializeToEventBuffer,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = NULL,
};

static const EventContextOutputAdapters g_nvlinkNetirReservedDataCtxOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = _nvlinkNetirReservedDataSerializeToEventBuffer,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = NULL,
};

static const EventContextOutputAdapters g_nvlinkTrexReservedDataCtxOutputAdapters =
{
    .serializeToEventBufferOpEventCtx = _nvlinkTrexReservedDataSerializeToEventBuffer,
    .serializeToCperEventCtx = gpuEventCtxSerializeToCperOpaqueCtx,
    .emitToOsLogXid = NULL,
};

static void
_nvlinkBlackwellErrorDataInit
(
    NvlinkBlackwellErrorCtx *pCtx,
    NvS32 linkId,
    NvU32 errorStatus,
    NvU32 intrInfo,
    const NvU32 *pDebugData,
    NvU32 debugDataCount
)
{
    NvU32 word;

    pCtx->header.pOutputAdapters = &g_nvlinkBlackwellErrorCtxOutputAdapters;
    pCtx->header.dataSize = sizeof(pCtx->data);
    portMemSet(&pCtx->data, 0, sizeof(pCtx->data));
    pCtx->data.linkId = linkId;
    pCtx->data.errorStatus = errorStatus;
    pCtx->data.intrInfo = intrInfo;

    for (word = 0; word < debugDataCount; word++)
    {
        pCtx->data.debugData[word] = (pDebugData != NULL) ? pDebugData[word] : 0;
    }
}

static void
_nvlinkBlackwellLegacyErrorDataInit
(
    NvlinkBlackwellLegacyErrorCtx *pCtx,
    NvS32 linkId,
    NvU32 errorStatus,
    NvU32 intrInfo,
    const NvU32 *pDebugData,
    NvU32 debugDataCount,
    NvU32 legacyXidCode,
    NvBool bFatal,
    NvBool bXContain,
    NvBool bInjected,
    const char *pSeverityString,
    const char *pLegacyXidErrorName
)
{
    NvU32 i;

    pCtx->header.pOutputAdapters = &g_nvlinkBlackwellLegacyErrorCtxOutputAdapters;
    pCtx->header.dataSize = sizeof(pCtx->data);
    portMemSet(&pCtx->data, 0, sizeof(pCtx->data));
    pCtx->data.pLegacyXidErrorName = pLegacyXidErrorName;
    pCtx->data.pSeverityString = pSeverityString;
    pCtx->data.bLegacyFatal = bFatal;
    pCtx->data.bLegacyXContain = bXContain;
    pCtx->data.bLegacyInjected = bInjected;
    pCtx->data.legacyLinkId = linkId;
    pCtx->data.legacyIntrInfo = intrInfo;
    pCtx->data.legacyErrorStatus = errorStatus;
    pCtx->data.legacyXidCode = legacyXidCode;

    for (i = 0; i < debugDataCount; i++)
    {
        pCtx->data.debugData[i] = (pDebugData != NULL) ? pDebugData[i] : 0;
    }
}

#define NVLINK_RESERVED_DATA_INIT(pReservedContext, unit)                                            \
    do                                                                                         \
    {                                                                                          \
        (pReservedContext)->header.pOutputAdapters = &g_nvlink##unit##ReservedDataCtxOutputAdapters;    \
        (pReservedContext)->header.dataSize = sizeof((pReservedContext)->data);                 \
        portMemSet(&(pReservedContext)->data, 0, sizeof((pReservedContext)->data));             \
    } while (0)

NV_STATUS
nvlinksawerrorgpufatalConstruct_IMPL
(
    NvlinkSawErrorGpuFatal *pEvent,
    EventBus *pEventBus,
    NvS32 linkId,
    NvU32 errorStatus,
    NvU32 intrInfo,
    const NvU32 *pDebugData,
    NvU32 debugDataCount,
    NvU32 legacyXidCode,
    NvBool bFatal,
    NvBool bXContain,
    NvBool bInjected,
    const char *pSeverityString,
    const char *pLegacyXidErrorName
)
{
    // This should never happen, but just in case.
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (pEvent != NULL && pEventBus != NULL && pDebugData != NULL &&
         pSeverityString != NULL && pLegacyXidErrorName != NULL),
        NV_ERR_INVALID_ARGUMENT);

    // Input sanity check
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (debugDataCount <= NVLINK_ERROR_DATA_BLACKWELL_DEBUG_DWORDS),
        NV_ERR_INVALID_ARGUMENT);

    // Initialize the blackwell data context
    _nvlinkBlackwellErrorDataInit(&pEvent->blackwellContext, linkId, errorStatus, intrInfo, pDebugData, debugDataCount);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellContext.header);

    // As of today, all error specific context is reserved. Initialize that reserved data context.
    NVLINK_RESERVED_DATA_INIT(&pEvent->context, Saw);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    // Initialize the blackwell legacy XID data context
    _nvlinkBlackwellLegacyErrorDataInit(&pEvent->blackwellLegacyContext, linkId, errorStatus, intrInfo,
        pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected,
        pSeverityString, pLegacyXidErrorName);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellLegacyContext.header);

    return NV_OK;
}

NV_STATUS
nvlinksawerrornonfatalwarningConstruct_IMPL
(
    NvlinkSawErrorNonFatalWarning *pEvent,
    EventBus *pEventBus,
    NvS32 linkId,
    NvU32 errorStatus,
    NvU32 intrInfo,
    const NvU32 *pDebugData,
    NvU32 debugDataCount,
    NvU32 legacyXidCode,
    NvBool bFatal,
    NvBool bXContain,
    NvBool bInjected,
    const char *pSeverityString,
    const char *pLegacyXidErrorName
)
{
    // This should never happen, but just in case.
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (pEvent != NULL && pEventBus != NULL && pDebugData != NULL &&
         pSeverityString != NULL && pLegacyXidErrorName != NULL),
        NV_ERR_INVALID_ARGUMENT);

    // Input sanity check
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (debugDataCount <= NVLINK_ERROR_DATA_BLACKWELL_DEBUG_DWORDS),
        NV_ERR_INVALID_ARGUMENT);

    // Initialize the blackwell data context
    _nvlinkBlackwellErrorDataInit(&pEvent->blackwellContext, linkId, errorStatus, intrInfo, pDebugData, debugDataCount);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellContext.header);

    // As of today, all error specific context is reserved. Initialize that reserved data context.
    NVLINK_RESERVED_DATA_INIT(&pEvent->context, Saw);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    // Initialize the blackwell legacy XID data context
    _nvlinkBlackwellLegacyErrorDataInit(&pEvent->blackwellLegacyContext, linkId, errorStatus, intrInfo,
        pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected,
        pSeverityString, pLegacyXidErrorName);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellLegacyContext.header);

    return NV_OK;
}

NV_STATUS
nvlinksawerrornonfatalcontainedConstruct_IMPL
(
    NvlinkSawErrorNonFatalContained *pEvent,
    EventBus *pEventBus,
    NvS32 linkId,
    NvU32 errorStatus,
    NvU32 intrInfo,
    const NvU32 *pDebugData,
    NvU32 debugDataCount,
    NvU32 legacyXidCode,
    NvBool bFatal,
    NvBool bXContain,
    NvBool bInjected,
    const char *pSeverityString,
    const char *pLegacyXidErrorName
)
{
    // This should never happen, but just in case.
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (pEvent != NULL && pEventBus != NULL && pDebugData != NULL &&
         pSeverityString != NULL && pLegacyXidErrorName != NULL),
        NV_ERR_INVALID_ARGUMENT);

    // Input sanity check
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (debugDataCount <= NVLINK_ERROR_DATA_BLACKWELL_DEBUG_DWORDS),
        NV_ERR_INVALID_ARGUMENT);

    // Initialize the blackwell data context
    _nvlinkBlackwellErrorDataInit(&pEvent->blackwellContext, linkId, errorStatus, intrInfo, pDebugData, debugDataCount);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellContext.header);

    // As of today, all error specific context is reserved. Initialize that reserved data context.
    NVLINK_RESERVED_DATA_INIT(&pEvent->context, Saw);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    // Initialize the blackwell legacy XID data context
    _nvlinkBlackwellLegacyErrorDataInit(&pEvent->blackwellLegacyContext, linkId, errorStatus, intrInfo,
        pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected,
        pSeverityString, pLegacyXidErrorName);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellLegacyContext.header);

    return NV_OK;
}

NV_STATUS
nvlinkrlwerrorapplicationfatalConstruct_IMPL
(
    NvlinkRlwErrorApplicationFatal *pEvent,
    EventBus *pEventBus,
    NvS32 linkId,
    NvU32 errorStatus,
    NvU32 intrInfo,
    const NvU32 *pDebugData,
    NvU32 debugDataCount,
    NvU32 legacyXidCode,
    NvBool bFatal,
    NvBool bXContain,
    NvBool bInjected,
    const char *pSeverityString,
    const char *pLegacyXidErrorName
)
{
    // This should never happen, but just in case.
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (pEvent != NULL && pEventBus != NULL && pDebugData != NULL &&
         pSeverityString != NULL && pLegacyXidErrorName != NULL),
        NV_ERR_INVALID_ARGUMENT);

    // Input sanity check
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (debugDataCount <= NVLINK_ERROR_DATA_BLACKWELL_DEBUG_DWORDS),
        NV_ERR_INVALID_ARGUMENT);

    // Initialize the blackwell data context
    _nvlinkBlackwellErrorDataInit(&pEvent->blackwellContext, linkId, errorStatus, intrInfo, pDebugData, debugDataCount);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellContext.header);

    // As of today, all error specific context is reserved. Initialize that reserved data context.
    NVLINK_RESERVED_DATA_INIT(&pEvent->context, Rlw);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    // Initialize the blackwell legacy XID data context
    _nvlinkBlackwellLegacyErrorDataInit(&pEvent->blackwellLegacyContext, linkId, errorStatus, intrInfo,
        pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected,
        pSeverityString, pLegacyXidErrorName);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellLegacyContext.header);

    return NV_OK;
}

NV_STATUS
nvlinkrlwerrorgpufatalConstruct_IMPL
(
    NvlinkRlwErrorGpuFatal *pEvent,
    EventBus *pEventBus,
    NvS32 linkId,
    NvU32 errorStatus,
    NvU32 intrInfo,
    const NvU32 *pDebugData,
    NvU32 debugDataCount,
    NvU32 legacyXidCode,
    NvBool bFatal,
    NvBool bXContain,
    NvBool bInjected,
    const char *pSeverityString,
    const char *pLegacyXidErrorName
)
{
    // This should never happen, but just in case.
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (pEvent != NULL && pEventBus != NULL && pDebugData != NULL &&
         pSeverityString != NULL && pLegacyXidErrorName != NULL),
        NV_ERR_INVALID_ARGUMENT);

    // Input sanity check
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (debugDataCount <= NVLINK_ERROR_DATA_BLACKWELL_DEBUG_DWORDS),
        NV_ERR_INVALID_ARGUMENT);

    // Initialize the blackwell data context
    _nvlinkBlackwellErrorDataInit(&pEvent->blackwellContext, linkId, errorStatus, intrInfo, pDebugData, debugDataCount);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellContext.header);

    // As of today, all error specific context is reserved. Initialize that reserved data context.
    NVLINK_RESERVED_DATA_INIT(&pEvent->context, Rlw);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    // Initialize the blackwell legacy XID data context
    _nvlinkBlackwellLegacyErrorDataInit(&pEvent->blackwellLegacyContext, linkId, errorStatus, intrInfo,
        pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected,
        pSeverityString, pLegacyXidErrorName);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellLegacyContext.header);

    return NV_OK;
}

NV_STATUS
nvlinkrlwerrornonfatalwarningConstruct_IMPL
(
    NvlinkRlwErrorNonFatalWarning *pEvent,
    EventBus *pEventBus,
    NvS32 linkId,
    NvU32 errorStatus,
    NvU32 intrInfo,
    const NvU32 *pDebugData,
    NvU32 debugDataCount,
    NvU32 legacyXidCode,
    NvBool bFatal,
    NvBool bXContain,
    NvBool bInjected,
    const char *pSeverityString,
    const char *pLegacyXidErrorName
)
{
    // This should never happen, but just in case.
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (pEvent != NULL && pEventBus != NULL && pDebugData != NULL &&
         pSeverityString != NULL && pLegacyXidErrorName != NULL),
        NV_ERR_INVALID_ARGUMENT);

    // Input sanity check
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (debugDataCount <= NVLINK_ERROR_DATA_BLACKWELL_DEBUG_DWORDS),
        NV_ERR_INVALID_ARGUMENT);

    // Initialize the blackwell data context
    _nvlinkBlackwellErrorDataInit(&pEvent->blackwellContext, linkId, errorStatus, intrInfo, pDebugData, debugDataCount);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellContext.header);

    // As of today, all error specific context is reserved. Initialize that reserved data context.
    NVLINK_RESERVED_DATA_INIT(&pEvent->context, Rlw);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    // Initialize the blackwell legacy XID data context
    _nvlinkBlackwellLegacyErrorDataInit(&pEvent->blackwellLegacyContext, linkId, errorStatus, intrInfo,
        pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected,
        pSeverityString, pLegacyXidErrorName);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellLegacyContext.header);

    return NV_OK;
}

NV_STATUS
nvlinktlwerrornonfatalcontainedConstruct_IMPL
(
    NvlinkTlwErrorNonFatalContained *pEvent,
    EventBus *pEventBus,
    NvS32 linkId,
    NvU32 errorStatus,
    NvU32 intrInfo,
    const NvU32 *pDebugData,
    NvU32 debugDataCount,
    NvU32 legacyXidCode,
    NvBool bFatal,
    NvBool bXContain,
    NvBool bInjected,
    const char *pSeverityString,
    const char *pLegacyXidErrorName
)
{
    // This should never happen, but just in case.
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (pEvent != NULL && pEventBus != NULL && pDebugData != NULL &&
         pSeverityString != NULL && pLegacyXidErrorName != NULL),
        NV_ERR_INVALID_ARGUMENT);

    // Input sanity check
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (debugDataCount <= NVLINK_ERROR_DATA_BLACKWELL_DEBUG_DWORDS),
        NV_ERR_INVALID_ARGUMENT);

    // Initialize the blackwell data context
    _nvlinkBlackwellErrorDataInit(&pEvent->blackwellContext, linkId, errorStatus, intrInfo, pDebugData, debugDataCount);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellContext.header);

    // As of today, all error specific context is reserved. Initialize that reserved data context.
    NVLINK_RESERVED_DATA_INIT(&pEvent->context, Tlw);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    // Initialize the blackwell legacy XID data context
    _nvlinkBlackwellLegacyErrorDataInit(&pEvent->blackwellLegacyContext, linkId, errorStatus, intrInfo,
        pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected,
        pSeverityString, pLegacyXidErrorName);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellLegacyContext.header);

    return NV_OK;
}

NV_STATUS
nvlinktlwerrorgpufatalConstruct_IMPL
(
    NvlinkTlwErrorGpuFatal *pEvent,
    EventBus *pEventBus,
    NvS32 linkId,
    NvU32 errorStatus,
    NvU32 intrInfo,
    const NvU32 *pDebugData,
    NvU32 debugDataCount,
    NvU32 legacyXidCode,
    NvBool bFatal,
    NvBool bXContain,
    NvBool bInjected,
    const char *pSeverityString,
    const char *pLegacyXidErrorName
)
{
    // This should never happen, but just in case.
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (pEvent != NULL && pEventBus != NULL && pDebugData != NULL &&
         pSeverityString != NULL && pLegacyXidErrorName != NULL),
        NV_ERR_INVALID_ARGUMENT);

    // Input sanity check
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (debugDataCount <= NVLINK_ERROR_DATA_BLACKWELL_DEBUG_DWORDS),
        NV_ERR_INVALID_ARGUMENT);

    // Initialize the blackwell data context
    _nvlinkBlackwellErrorDataInit(&pEvent->blackwellContext, linkId, errorStatus, intrInfo, pDebugData, debugDataCount);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellContext.header);

    // As of today, all error specific context is reserved. Initialize that reserved data context.
    NVLINK_RESERVED_DATA_INIT(&pEvent->context, Tlw);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    // Initialize the blackwell legacy XID data context
    _nvlinkBlackwellLegacyErrorDataInit(&pEvent->blackwellLegacyContext, linkId, errorStatus, intrInfo,
        pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected,
        pSeverityString, pLegacyXidErrorName);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellLegacyContext.header);

    return NV_OK;
}

NV_STATUS
nvlinktlwerrornonfatalwarningConstruct_IMPL
(
    NvlinkTlwErrorNonFatalWarning *pEvent,
    EventBus *pEventBus,
    NvS32 linkId,
    NvU32 errorStatus,
    NvU32 intrInfo,
    const NvU32 *pDebugData,
    NvU32 debugDataCount,
    NvU32 legacyXidCode,
    NvBool bFatal,
    NvBool bXContain,
    NvBool bInjected,
    const char *pSeverityString,
    const char *pLegacyXidErrorName
)
{
    // This should never happen, but just in case.
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (pEvent != NULL && pEventBus != NULL && pDebugData != NULL &&
         pSeverityString != NULL && pLegacyXidErrorName != NULL),
        NV_ERR_INVALID_ARGUMENT);

    // Input sanity check
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (debugDataCount <= NVLINK_ERROR_DATA_BLACKWELL_DEBUG_DWORDS),
        NV_ERR_INVALID_ARGUMENT);

    // Initialize the blackwell data context
    _nvlinkBlackwellErrorDataInit(&pEvent->blackwellContext, linkId, errorStatus, intrInfo, pDebugData, debugDataCount);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellContext.header);

    // As of today, all error specific context is reserved. Initialize that reserved data context.
    NVLINK_RESERVED_DATA_INIT(&pEvent->context, Tlw);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    // Initialize the blackwell legacy XID data context
    _nvlinkBlackwellLegacyErrorDataInit(&pEvent->blackwellLegacyContext, linkId, errorStatus, intrInfo,
        pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected,
        pSeverityString, pLegacyXidErrorName);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellLegacyContext.header);

    return NV_OK;
}

NV_STATUS
nvlinktrexerrornonfatalwarningConstruct_IMPL
(
    NvlinkTrexErrorNonFatalWarning *pEvent,
    EventBus *pEventBus,
    NvS32 linkId,
    NvU32 errorStatus,
    NvU32 intrInfo,
    const NvU32 *pDebugData,
    NvU32 debugDataCount,
    NvU32 legacyXidCode,
    NvBool bFatal,
    NvBool bXContain,
    NvBool bInjected,
    const char *pSeverityString,
    const char *pLegacyXidErrorName
)
{
    // This should never happen, but just in case.
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (pEvent != NULL && pEventBus != NULL && pDebugData != NULL &&
         pSeverityString != NULL && pLegacyXidErrorName != NULL),
        NV_ERR_INVALID_ARGUMENT);

    // Input sanity check
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (debugDataCount <= NVLINK_ERROR_DATA_BLACKWELL_DEBUG_DWORDS),
        NV_ERR_INVALID_ARGUMENT);

    // Initialize the blackwell data context
    _nvlinkBlackwellErrorDataInit(&pEvent->blackwellContext, linkId, errorStatus, intrInfo, pDebugData, debugDataCount);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellContext.header);

    // As of today, all error specific context is reserved. Initialize that reserved data context.
    NVLINK_RESERVED_DATA_INIT(&pEvent->context, Trex);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    // Initialize the blackwell legacy XID data context
    _nvlinkBlackwellLegacyErrorDataInit(&pEvent->blackwellLegacyContext, linkId, errorStatus, intrInfo,
        pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected,
        pSeverityString, pLegacyXidErrorName);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellLegacyContext.header);

    return NV_OK;
}

NV_STATUS
nvlinknvlpwerrornonfatalwarningConstruct_IMPL
(
    NvlinkNvlpwErrorNonFatalWarning *pEvent,
    EventBus *pEventBus,
    NvS32 linkId,
    NvU32 errorStatus,
    NvU32 intrInfo,
    const NvU32 *pDebugData,
    NvU32 debugDataCount,
    NvU32 legacyXidCode,
    NvBool bFatal,
    NvBool bXContain,
    NvBool bInjected,
    const char *pSeverityString,
    const char *pLegacyXidErrorName
)
{
    // This should never happen, but just in case.
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (pEvent != NULL && pEventBus != NULL && pDebugData != NULL &&
         pSeverityString != NULL && pLegacyXidErrorName != NULL),
        NV_ERR_INVALID_ARGUMENT);

    // Input sanity check
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (debugDataCount <= NVLINK_ERROR_DATA_BLACKWELL_DEBUG_DWORDS),
        NV_ERR_INVALID_ARGUMENT);

    // Initialize the blackwell data context
    _nvlinkBlackwellErrorDataInit(&pEvent->blackwellContext, linkId, errorStatus, intrInfo, pDebugData, debugDataCount);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellContext.header);

    // As of today, all error specific context is reserved. Initialize that reserved data context.
    NVLINK_RESERVED_DATA_INIT(&pEvent->context, NvlpwCtrl);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    // Initialize the blackwell legacy XID data context
    _nvlinkBlackwellLegacyErrorDataInit(&pEvent->blackwellLegacyContext, linkId, errorStatus, intrInfo,
        pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected,
        pSeverityString, pLegacyXidErrorName);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellLegacyContext.header);

    return NV_OK;
}

NV_STATUS
nvlinknetirerrornonfatalwarningConstruct_IMPL
(
    NvlinkNetirErrorNonFatalWarning *pEvent,
    EventBus *pEventBus,
    NvS32 linkId,
    NvU32 errorStatus,
    NvU32 intrInfo,
    const NvU32 *pDebugData,
    NvU32 debugDataCount,
    NvU32 legacyXidCode,
    NvBool bFatal,
    NvBool bXContain,
    NvBool bInjected,
    const char *pSeverityString,
    const char *pLegacyXidErrorName
)
{
    // This should never happen, but just in case.
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (pEvent != NULL && pEventBus != NULL && pDebugData != NULL &&
         pSeverityString != NULL && pLegacyXidErrorName != NULL),
        NV_ERR_INVALID_ARGUMENT);

    // Input sanity check
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (debugDataCount <= NVLINK_ERROR_DATA_BLACKWELL_DEBUG_DWORDS),
        NV_ERR_INVALID_ARGUMENT);

    // Initialize the blackwell data context
    _nvlinkBlackwellErrorDataInit(&pEvent->blackwellContext, linkId, errorStatus, intrInfo, pDebugData, debugDataCount);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellContext.header);

    // As of today, all error specific context is reserved. Initialize that reserved data context.
    NVLINK_RESERVED_DATA_INIT(&pEvent->context, Netir);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    // Initialize the blackwell legacy XID data context
    _nvlinkBlackwellLegacyErrorDataInit(&pEvent->blackwellLegacyContext, linkId, errorStatus, intrInfo,
        pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected,
        pSeverityString, pLegacyXidErrorName);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellLegacyContext.header);

    return NV_OK;
}

NV_STATUS
nvlinknetirerrorgpufatalConstruct_IMPL
(
    NvlinkNetirErrorGpuFatal *pEvent,
    EventBus *pEventBus,
    NvS32 linkId,
    NvU32 errorStatus,
    NvU32 intrInfo,
    const NvU32 *pDebugData,
    NvU32 debugDataCount,
    NvU32 legacyXidCode,
    NvBool bFatal,
    NvBool bXContain,
    NvBool bInjected,
    const char *pSeverityString,
    const char *pLegacyXidErrorName
)
{
    // This should never happen, but just in case.
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (pEvent != NULL && pEventBus != NULL && pDebugData != NULL &&
         pSeverityString != NULL && pLegacyXidErrorName != NULL),
        NV_ERR_INVALID_ARGUMENT);

    // Input sanity check
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (debugDataCount <= NVLINK_ERROR_DATA_BLACKWELL_DEBUG_DWORDS),
        NV_ERR_INVALID_ARGUMENT);

    // Initialize the blackwell data context
    _nvlinkBlackwellErrorDataInit(&pEvent->blackwellContext, linkId, errorStatus, intrInfo, pDebugData, debugDataCount);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellContext.header);

    // As of today, all error specific context is reserved. Initialize that reserved data context.
    NVLINK_RESERVED_DATA_INIT(&pEvent->context, Netir);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    // Initialize the blackwell legacy XID data context
    _nvlinkBlackwellLegacyErrorDataInit(&pEvent->blackwellLegacyContext, linkId, errorStatus, intrInfo,
        pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected,
        pSeverityString, pLegacyXidErrorName);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellLegacyContext.header);

    return NV_OK;
}

NV_STATUS
nvlinknetirerrorlinkfatalConstruct_IMPL
(
    NvlinkNetirErrorLinkFatal *pEvent,
    EventBus *pEventBus,
    NvS32 linkId,
    NvU32 errorStatus,
    NvU32 intrInfo,
    const NvU32 *pDebugData,
    NvU32 debugDataCount,
    NvU32 legacyXidCode,
    NvBool bFatal,
    NvBool bXContain,
    NvBool bInjected,
    const char *pSeverityString,
    const char *pLegacyXidErrorName
)
{
    // This should never happen, but just in case.
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (pEvent != NULL && pEventBus != NULL && pDebugData != NULL &&
         pSeverityString != NULL && pLegacyXidErrorName != NULL),
        NV_ERR_INVALID_ARGUMENT);

    // Input sanity check
    NV_CHECK_OR_RETURN(LEVEL_ERROR,
        (debugDataCount <= NVLINK_ERROR_DATA_BLACKWELL_DEBUG_DWORDS),
        NV_ERR_INVALID_ARGUMENT);

    // Initialize the blackwell data context
    _nvlinkBlackwellErrorDataInit(&pEvent->blackwellContext, linkId, errorStatus, intrInfo, pDebugData, debugDataCount);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellContext.header);

    // As of today, all error specific context is reserved. Initialize that reserved data context.
    NVLINK_RESERVED_DATA_INIT(&pEvent->context, Netir);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->context.header);

    // Initialize the blackwell legacy XID data context
    _nvlinkBlackwellLegacyErrorDataInit(&pEvent->blackwellLegacyContext, linkId, errorStatus, intrInfo,
        pDebugData, debugDataCount, legacyXidCode, bFatal, bXContain, bInjected,
        pSeverityString, pLegacyXidErrorName);
    opevtAddContext(staticCast(pEvent, OperationalEvent), &pEvent->blackwellLegacyContext.header);

    return NV_OK;
}
