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
#include "nvmisc.h"
#include "nverror.h"
#include "nvport/nvport.h"

#define NVLINK_ALI_TRAINING_FAILURE_XID_MESSAGE_FMT \
    "NVLink: Link training failed for link %u(0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x)"
#define NVLINK_ALI_TRAINING_FAILURE_BW_XID_MESSAGE_FMT \
    "NVLink: Link training failed for links 0x%llx(0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x)"
#define NVLINK_MSE_LEGACY_XID_MESSAGE_FMT \
    "%s %s XC%01u i%01u Link %02d (0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x)\n"
#define NVLINK_SW_LINK_DOWN_LEGACY_XID_MESSAGE_FMT \
    "%s %s XC%01u i%01u Link %02u : Link Down While Active.\n"

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
    NvU32 *pBytesWritten
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(
        pCtxHeader,
        pOutput,
        outputSize,
        pBytesWritten,
        NVLINK_EVENT_CTX_TYPE_ALI_TRAINING_FAILURE_DATA,
        NVLINK_ALI_TRAINING_FAILURE_DATA_VERSION);
}

static void
_nvlinkAliTrainingFailureEmitOsLogXid
(
    EventContextHeader *pCtxHeader,
    OBJGPU *pGpu
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
        NV_ERROR_LOG(pGpu, ALI_TRAINING_FAIL, NVLINK_ALI_TRAINING_FAILURE_BW_XID_MESSAGE_FMT,
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

        NV_ERROR_LOG(pGpu, ALI_TRAINING_FAIL, NVLINK_ALI_TRAINING_FAILURE_XID_MESSAGE_FMT,
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
    NvU32 *pBytesWritten
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(
        pCtxHeader,
        pOutput,
        outputSize,
        pBytesWritten,
        NVLINK_EVENT_CTX_TYPE_MSE_ERROR_DATA,
        NVLINK_MSE_ERROR_DATA_VERSION);
}

static void
_nvlinkMseErrorEmitOsLogXid
(
    EventContextHeader *pCtxHeader,
    OBJGPU *pGpu
)
{
    NvlinkMseErrorLegacyCtx *pContext = (NvlinkMseErrorLegacyCtx *)pCtxHeader;

    NV_ERROR_LOG(pGpu, pContext->data.legacyXidCode, NVLINK_MSE_LEGACY_XID_MESSAGE_FMT,
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
    NvU32 *pBytesWritten
)
{
    return gpuEventCtxSerializeToEventBufferGoeCtxType(
        pCtxHeader,
        pOutput,
        outputSize,
        pBytesWritten,
        NVLINK_EVENT_CTX_TYPE_SW_LINK_DOWN_ERROR_DATA,
        NVLINK_SW_LINK_DOWN_ERROR_DATA_VERSION);
}

static void
_nvlinkSwLinkDownEmitOsLogXid
(
    EventContextHeader *pCtxHeader,
    OBJGPU *pGpu
)
{
    NvlinkSwLinkDownLegacyCtx *pContext = (NvlinkSwLinkDownLegacyCtx *)pCtxHeader;

    NV_ERROR_LOG(pGpu, pContext->data.legacyXidCode, NVLINK_SW_LINK_DOWN_LEGACY_XID_MESSAGE_FMT,
                 "NVLINK_SW_DEFINED_ERROR",
                 pContext->data.bLegacyFatal ? " Fatal  " : "Nonfatal",
                 pContext->data.bLegacyXContain,
                 pContext->data.bLegacyInjected,
                 pContext->data.linkId);
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

