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

/**
 * @file
 * @brief NVIDIA GPU-specific CPER section implementation
 */

#include <nv-stddef.h>

#include "cper/gpu_cper.h"
#include "nvport/nvport.h"

/*
 * Alignment constant for NVIDIA event contexts
 */
#define EVENT_CONTEXT_DATA_ALIGNMENT 16

/*
 * Calculate the aligned size of an event context
 */
static inline NvU32 _calcContextSize(NvU32 dataSize)
{
    return NV_ALIGN_UP(sizeof(NV_CPER_NV_EVENT_CONTEXT_HEADER) + dataSize,
                       EVENT_CONTEXT_DATA_ALIGNMENT);
}

/*
 * Get the event header from the section state.
 * The event header is at the start of the section data.
 */
static inline NV_CPER_NV_EVENT_HEADER *_getEventHeader
(
    NV_CPER_NV_EVENT_SECTION_STATE *pState
)
{
    return (NV_CPER_NV_EVENT_HEADER *)cperSectionGetData(&pState->section);
}

NV_STATUS cperAddNvidiaEventSection
(
    void                           *pBuffer,
    NvU32                           bufferSize,
    const NV_CPER_NV_EVENT_PARAMS  *pParams,
    NV_CPER_NV_EVENT_SECTION_STATE *pState
)
{
    NV_STATUS status;
    NV_CPER_SECTION_PARAMS sectionParams = {0};
    NV_CPER_NV_EVENT_HEADER *pEventHeader;
    NV_CPER_NV_GPU_EVENT_INFO *pEventInfo;
    NvU32 sectionDataSize;

    static const NV_CPER_GUID nvEventSectionType = NV_CPER_SECTION_NVIDIA_EVENT_GUID;

    if (pBuffer == NULL || pParams == NULL || pState == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    // Initial section data size: event header + GPU event info (no contexts yet)
    sectionDataSize = sizeof(NV_CPER_NV_EVENT_HEADER) + sizeof(NV_CPER_NV_GPU_EVENT_INFO);

    // Prepare section params - we pass NULL for pData and will write directly after
    portMemSet(&sectionParams, 0, sizeof(sectionParams));
    sectionParams.pSectionType = &nvEventSectionType;
    sectionParams.severity = pParams->severity;
    sectionParams.flags = pParams->sectionFlags;
    sectionParams.pData = NULL;
    sectionParams.dataSize = sectionDataSize;
    sectionParams.pFruId = pParams->pFruId;
    sectionParams.pFruText = pParams->pFruText;

    // Add the section (this reserves space and returns a handle)
    status = cperAddSection(pBuffer, bufferSize, &sectionParams, &pState->section);
    if (status != NV_OK)
        return status;

    // Write the event header directly into the section data area
    pEventHeader = (NV_CPER_NV_EVENT_HEADER *)cperSectionGetData(&pState->section);
    portMemSet(pEventHeader, 0, sizeof(*pEventHeader));
    pEventHeader->version = NV_CPER_NV_EVENT_HEADER_VERSION;
    pEventHeader->eventContextCount = 0;  // No contexts yet
    pEventHeader->sourceDeviceType = NV_CPER_NV_SOURCE_DEVICE_TYPE_GPU;
    pEventHeader->eventType = pParams->eventType;
    pEventHeader->eventSubType = pParams->eventSubType;
    pEventHeader->eventLinkId = pParams->eventLinkId;

    // Copy module signature
    if (pParams->pModuleSignature != NULL)
    {
        portStringCopy((char *)pEventHeader->sourceModuleSignature,
                       NV_CPER_NV_MODULE_SIGNATURE_LEN,
                       pParams->pModuleSignature,
                       NV_CPER_NV_MODULE_SIGNATURE_LEN);
    }

    // Write GPU event info
    pEventInfo = (NV_CPER_NV_GPU_EVENT_INFO *)((NvU8 *)pEventHeader + sizeof(*pEventHeader));
    portMemSet(pEventInfo, 0, sizeof(*pEventInfo));
    pEventInfo->version = NV_CPER_NV_GPU_EVENT_INFO_VERSION;
    pEventInfo->size = NV_CPER_NV_GPU_EVENT_INFO_SIZE;
    pEventInfo->eventOriginator = (NvU8)pParams->originator;
    pEventInfo->sourcePartition = pParams->sourcePartition;
    pEventInfo->sourceSubPartition = pParams->sourceSubPartition;
    pEventInfo->pdi = pParams->pdi;

    // Initialize the next context offset (after event header + GPU event info)
    pState->nextContextOffset = pState->section.sectionDataOffset + sectionDataSize;

    return NV_OK;
}

NV_STATUS cperNvidiaEventSectionAddContext
(
    NV_CPER_NV_EVENT_SECTION_STATE *pState,
    NvU16                           dataFormatType,
    NvU16                           dataFormatVersion,
    NvU32                           dataSize,
    void                          **ppData
)
{
    NV_STATUS status;
    NV_CPER_NV_EVENT_HEADER *pEventHeader;
    NV_CPER_NV_EVENT_CONTEXT_HEADER *pContextHeader;
    NvU32 alignedContextSize;
    NvU32 currentSectionLength;
    NvU32 newSectionLength;
    NvU8 *pContextStart;

    if (pState == NULL || ppData == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    alignedContextSize = _calcContextSize(dataSize);

    // Check if there's room in the buffer
    if (pState->nextContextOffset + alignedContextSize > pState->section.bufferSize)
        return NV_ERR_BUFFER_TOO_SMALL;

    pEventHeader = _getEventHeader(pState);

    // Update section and record lengths via the core API
    currentSectionLength = cperSectionGetDataSize(&pState->section);
    newSectionLength = currentSectionLength + alignedContextSize;
    status = cperSectionSetDataSize(&pState->section, newSectionLength);
    if (status != NV_OK)
        return status;

    // Write context header
    pContextStart = (NvU8 *)pState->section.pBuffer + pState->nextContextOffset;
    pContextHeader = (NV_CPER_NV_EVENT_CONTEXT_HEADER *)pContextStart;

    portMemSet(pContextHeader, 0, sizeof(*pContextHeader));
    pContextHeader->contextSize = alignedContextSize;
    pContextHeader->version = NV_CPER_NV_EVENT_CONTEXT_VERSION;
    pContextHeader->dataFormatType = dataFormatType;
    pContextHeader->dataFormatVersion = dataFormatVersion;
    pContextHeader->dataSize = dataSize;

    // Zero out the data area (including padding)
    portMemSet(pContextStart + sizeof(*pContextHeader), 0,
               alignedContextSize - sizeof(*pContextHeader));

    // Return pointer to data area
    *ppData = pContextStart + sizeof(NV_CPER_NV_EVENT_CONTEXT_HEADER);

    // Increment context count in event header
    pEventHeader->eventContextCount++;

    // Advance next context offset
    pState->nextContextOffset += alignedContextSize;

    return NV_OK;
}

NV_STATUS cperNvidiaEventSectionSetContextDataSize
(
    NV_CPER_NV_EVENT_SECTION_STATE *pState,
    void                           *pData,
    NvU32                           newDataSize
)
{
    NV_CPER_NV_EVENT_CONTEXT_HEADER *pContextHeader;
    NvU32 oldAlignedSize;
    NvU32 newAlignedSize;
    NvS32 sizeDelta;
    NvU32 currentSectionLength;
    NvU32 newSectionLength;

    if (pState == NULL || pData == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    // The context header is immediately before the data
    pContextHeader = (NV_CPER_NV_EVENT_CONTEXT_HEADER *)
        ((NvU8 *)pData - sizeof(NV_CPER_NV_EVENT_CONTEXT_HEADER));

    // Validate the new size doesn't exceed original
    if (newDataSize > pContextHeader->dataSize)
        return NV_ERR_INVALID_ARGUMENT;

    oldAlignedSize = pContextHeader->contextSize;
    newAlignedSize = _calcContextSize(newDataSize);
    sizeDelta = (NvS32)newAlignedSize - (NvS32)oldAlignedSize;

    // Update context header
    pContextHeader->dataSize = newDataSize;
    pContextHeader->contextSize = newAlignedSize;

    // If the aligned size changed, update section and record lengths
    if (sizeDelta != 0)
    {
        currentSectionLength = cperSectionGetDataSize(&pState->section);
        newSectionLength = (NvU32)((NvS32)currentSectionLength + sizeDelta);
        cperSectionSetDataSize(&pState->section, newSectionLength);

        pState->nextContextOffset = (NvU32)((NvS32)pState->nextContextOffset + sizeDelta);
    }

    return NV_OK;
}

/*
 * ============================================================================
 * Typed Context Addition Functions
 * ============================================================================
 */

NV_STATUS cperNvidiaEventAddGpuInitMetadataContext
(
    NV_CPER_NV_EVENT_SECTION_STATE     *pState,
    const NV_CPER_NV_GPU_INIT_METADATA *pMetadata
)
{
    NV_STATUS status;
    void *pData;

    if (pState == NULL || pMetadata == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    status = cperNvidiaEventSectionAddContext(pState,
                                              NV_CPER_NV_DATA_FORMAT_GPU_INIT_METADATA,
                                              NV_CPER_NV_GPU_INIT_META_VERSION,
                                              sizeof(NV_CPER_NV_GPU_INIT_METADATA),
                                              &pData);
    if (status != NV_OK)
        return status;

    portMemCopy(pData, sizeof(NV_CPER_NV_GPU_INIT_METADATA),
                pMetadata, sizeof(NV_CPER_NV_GPU_INIT_METADATA));

    return NV_OK;
}

NV_STATUS cperNvidiaEventAddGpuLegacyXidContext
(
    NV_CPER_NV_EVENT_SECTION_STATE *pState,
    NvU32                           xidCode,
    const char                     *pMessage
)
{
    NV_STATUS status;
    void *pData;
    NV_CPER_NV_GPU_LEGACY_XID *pXid;
    NvU32 maxDataSize;
    NvU32 actualDataSize;
    NvLength msgLen;

    if (pState == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    // Allocate max size for Xid struct + message
    maxDataSize = sizeof(NV_CPER_NV_GPU_LEGACY_XID) + NV_CPER_NV_GPU_LEGACY_XID_MAX_MSG_LEN;

    status = cperNvidiaEventSectionAddContext(pState,
                                              NV_CPER_NV_DATA_FORMAT_GPU_LEGACY_XID,
                                              NV_CPER_NV_GPU_LEGACY_XID_VERSION,
                                              maxDataSize,
                                              &pData);
    if (status != NV_OK)
        return status;

    pXid = (NV_CPER_NV_GPU_LEGACY_XID *)pData;
    pXid->xidCode = xidCode;

    // Copy message and get actual length (including NUL terminator)
    if (pMessage != NULL)
        msgLen = portStringCopy((char *)(pXid + 1),
                                NV_CPER_NV_GPU_LEGACY_XID_MAX_MSG_LEN,
                                pMessage,
                                NV_CPER_NV_GPU_LEGACY_XID_MAX_MSG_LEN);
    else
        msgLen = 0;

    // Shrink context to actual size
    actualDataSize = (NvU32)(sizeof(NV_CPER_NV_GPU_LEGACY_XID) + msgLen);
    cperNvidiaEventSectionSetContextDataSize(pState, pData, actualDataSize);

    return NV_OK;
}

NV_STATUS cperNvidiaEventAddGpuRecActionsContext
(
    NV_CPER_NV_EVENT_SECTION_STATE           *pState,
    const NV_CPER_NV_GPU_RECOMMENDED_ACTIONS *pActions
)
{
    NV_STATUS status;
    void *pData;

    if (pState == NULL || pActions == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    status = cperNvidiaEventSectionAddContext(pState,
                                              NV_CPER_NV_DATA_FORMAT_GPU_REC_ACTIONS,
                                              NV_CPER_NV_GPU_REC_ACTIONS_VERSION,
                                              sizeof(NV_CPER_NV_GPU_RECOMMENDED_ACTIONS),
                                              &pData);
    if (status != NV_OK)
        return status;

    portMemCopy(pData, sizeof(NV_CPER_NV_GPU_RECOMMENDED_ACTIONS),
                pActions, sizeof(NV_CPER_NV_GPU_RECOMMENDED_ACTIONS));

    return NV_OK;
}

NV_STATUS cperNvidiaEventAddOpaqueContext
(
    NV_CPER_NV_EVENT_SECTION_STATE *pState,
    const void                     *pSrcData,
    NvU32                           dataSize
)
{
    NV_STATUS status;
    void *pData;

    if (pState == NULL || (pSrcData == NULL && dataSize > 0))
        return NV_ERR_INVALID_ARGUMENT;

    status = cperNvidiaEventSectionAddContext(pState,
                                              NV_CPER_NV_DATA_FORMAT_OPAQUE,
                                              0, // No version for opaque
                                              dataSize,
                                              &pData);
    if (status != NV_OK)
        return status;

    if (dataSize > 0)
        portMemCopy(pData, dataSize, pSrcData, dataSize);

    return NV_OK;
}

NV_STATUS cperNvidiaEventAdd8BKVContext
(
    NV_CPER_NV_EVENT_SECTION_STATE *pState,
    const NvU64                    *pKeyValues,
    NvU32                           count
)
{
    NV_STATUS status;
    void *pData;
    NvU32 dataSize;

    if (pState == NULL || (pKeyValues == NULL && count > 0))
        return NV_ERR_INVALID_ARGUMENT;

    // Each key-value pair is 2 x NvU64 = 16 bytes
    dataSize = count * 2 * sizeof(NvU64);

    status = cperNvidiaEventSectionAddContext(pState,
                                              NV_CPER_NV_DATA_FORMAT_KEY_VALUE_64,
                                              0,
                                              dataSize,
                                              &pData);
    if (status != NV_OK)
        return status;

    if (dataSize > 0)
        portMemCopy(pData, dataSize, pKeyValues, dataSize);

    return NV_OK;
}

NV_STATUS cperNvidiaEventAdd4BKVContext
(
    NV_CPER_NV_EVENT_SECTION_STATE *pState,
    const NvU32                    *pKeyValues,
    NvU32                           count
)
{
    NV_STATUS status;
    void *pData;
    NvU32 dataSize;

    if (pState == NULL || (pKeyValues == NULL && count > 0))
        return NV_ERR_INVALID_ARGUMENT;

    // Each key-value pair is 2 x NvU32 = 8 bytes
    dataSize = count * 2 * sizeof(NvU32);

    status = cperNvidiaEventSectionAddContext(pState,
                                              NV_CPER_NV_DATA_FORMAT_KEY_VALUE_32,
                                              0,
                                              dataSize,
                                              &pData);
    if (status != NV_OK)
        return status;

    if (dataSize > 0)
        portMemCopy(pData, dataSize, pKeyValues, dataSize);

    return NV_OK;
}

NV_STATUS cperNvidiaEventAdd8BVContext
(
    NV_CPER_NV_EVENT_SECTION_STATE *pState,
    const NvU64                    *pValues,
    NvU32                           count
)
{
    NV_STATUS status;
    void *pData;
    NvU32 dataSize;

    if (pState == NULL || (pValues == NULL && count > 0))
        return NV_ERR_INVALID_ARGUMENT;

    dataSize = count * sizeof(NvU64);

    status = cperNvidiaEventSectionAddContext(pState,
                                              NV_CPER_NV_DATA_FORMAT_VALUES_64,
                                              0,
                                              dataSize,
                                              &pData);
    if (status != NV_OK)
        return status;

    if (dataSize > 0)
        portMemCopy(pData, dataSize, pValues, dataSize);

    return NV_OK;
}

NV_STATUS cperNvidiaEventAdd4BVContext
(
    NV_CPER_NV_EVENT_SECTION_STATE *pState,
    const NvU32                    *pValues,
    NvU32                           count
)
{
    NV_STATUS status;
    void *pData;
    NvU32 dataSize;

    if (pState == NULL || (pValues == NULL && count > 0))
        return NV_ERR_INVALID_ARGUMENT;

    dataSize = count * sizeof(NvU32);

    status = cperNvidiaEventSectionAddContext(pState,
                                              NV_CPER_NV_DATA_FORMAT_VALUES_32,
                                              0,
                                              dataSize,
                                              &pData);
    if (status != NV_OK)
        return status;

    if (dataSize > 0)
        portMemCopy(pData, dataSize, pValues, dataSize);

    return NV_OK;
}

void cperNvidiaEventDumpRecordHeader
(
    const NV_CPER_RECORD_HEADER *pHdr,
    NvU32                        seq,
    const char                  *pLogPrefix
)
{
    if (pHdr == NULL)
        return;

    if (pLogPrefix == NULL)
        pLogPrefix = CPER_LOG_LEVEL_FW_WARN;

    CPER_PRINT(seq, pLogPrefix, "%s reported by the %s",
               cperNvidiaNotifyTypeToString(&pHdr->notificationType),
               cperNvidiaCreatorIdToString(&pHdr->creatorId));
    CPER_PRINT(seq, pLogPrefix, "event severity: %s",
               cperSeverityToString((NV_CPER_SEVERITY)pHdr->errorSeverity));
}

void cperNvidiaEventDumpSection
(
    NvU32                             eventIdx,
    const NV_CPER_RECORD_HEADER      *pHdr,
    const NV_CPER_SECTION_DESCRIPTOR *pDesc,
    const NvU8                       *pSection,
    NvU32                             sectionLength,
    NvU32                             seq,
    const char                       *pLogPrefix
)
{
    const NV_CPER_NV_EVENT_HEADER *pEv;
    const NV_CPER_NV_GPU_EVENT_INFO *pInfo;
    NvU32 offset;
    NvU32 ctxIdx;

    if (pHdr == NULL || pDesc == NULL || pSection == NULL)
        return;

    if (pLogPrefix == NULL)
        pLogPrefix = CPER_LOG_LEVEL_FW_WARN;

    CPER_PRINT(seq, pLogPrefix, " Event %u, type: %s",
               eventIdx,
               cperSeverityToString((NV_CPER_SEVERITY)pDesc->sectionSeverity));

    if (pDesc->validationBits & NV_CPER_SECTION_VALID_FRU_ID)
        CPER_PRINT(seq, pLogPrefix, " fru_id: " NV_CPER_GUID_FMT,
                   NV_CPER_GUID_FMT_ARGS(&pDesc->fruId));

    if (pDesc->validationBits & NV_CPER_SECTION_VALID_FRU_TEXT)
        CPER_PRINT(seq, pLogPrefix, " fru_text: %s", (const char *)pDesc->fruText);

    CPER_PRINT(seq, pLogPrefix, "  section_type: NVIDIA Event v1");

    if (sectionLength < (sizeof(*pEv) + sizeof(*pInfo)))
    {
        CPER_PRINT(seq, pLogPrefix, "  decoding_error: section too small");
        return;
    }

    pEv = (const NV_CPER_NV_EVENT_HEADER *)pSection;
    pInfo = (const NV_CPER_NV_GPU_EVENT_INFO *)(pSection + sizeof(*pEv));

    {
        const char *pTypeStr = cperNvidiaSourceDeviceTypeToString(pEv->sourceDeviceType);
        const char *pOrigStr = cperNvidiaGpuEventOriginatorToString(
            (NV_CPER_NV_GPU_EVENT_ORIGINATOR)pInfo->eventOriginator);
        const char *pSig = (const char *)pEv->sourceModuleSignature;

        CPER_PRINT(seq, pLogPrefix, "  source_device_type: %u, %s",
                   pEv->sourceDeviceType, pTypeStr);
        CPER_PRINT(seq, pLogPrefix, "  event_originator: %u, %s", pInfo->eventOriginator, pOrigStr);
        CPER_PRINT(seq, pLogPrefix, "  module_signature: %s", pSig);
        CPER_PRINT(seq, pLogPrefix, "  event_type: 0x%x", pEv->eventType);
        CPER_PRINT(seq, pLogPrefix, "  event_subtype: 0x%x", pEv->eventSubType);
        CPER_PRINT(seq, pLogPrefix, "  event_link_id: %llu", pEv->eventLinkId);
    }

    // Walk all contexts and print any recognized ones.
    offset = (NvU32)(sizeof(*pEv) + sizeof(*pInfo));
    for (ctxIdx = 0; ctxIdx < pEv->eventContextCount; ctxIdx++)
    {
        const NV_CPER_NV_EVENT_CONTEXT_HEADER *pCtxHdr;
        NvU32 ctxTotalSize;
        NvU32 ctxMaxDataSize;
        const void *pCtxData;

        if (offset + sizeof(*pCtxHdr) > sectionLength)
            break;

        pCtxHdr = (const NV_CPER_NV_EVENT_CONTEXT_HEADER *)(pSection + offset);
        ctxTotalSize = pCtxHdr->contextSize;

        if (ctxTotalSize < sizeof(*pCtxHdr) || offset + ctxTotalSize > sectionLength)
            break;

        ctxMaxDataSize = ctxTotalSize - (NvU32)sizeof(*pCtxHdr);
        if (pCtxHdr->dataSize > ctxMaxDataSize)
        {
            CPER_PRINT(seq, pLogPrefix,
                       "   Event Context %u, type: 0x%x decoding_error: data_size %u > max %u",
                       ctxIdx, pCtxHdr->dataFormatType, pCtxHdr->dataSize, ctxMaxDataSize);
            offset += ctxTotalSize;
            continue;
        }

        pCtxData = (const void *)(pCtxHdr + 1);

        if (pCtxHdr->dataFormatType == NV_CPER_NV_DATA_FORMAT_GPU_INIT_METADATA &&
            pCtxHdr->dataSize >= sizeof(NV_CPER_NV_GPU_INIT_METADATA))
        {
            const NV_CPER_NV_GPU_INIT_METADATA *pMeta =
                (const NV_CPER_NV_GPU_INIT_METADATA *)pCtxData;

            CPER_PRINT(seq, pLogPrefix, "   Event Context %u, type: GPU Init Info", ctxIdx);
            CPER_PRINT(seq, pLogPrefix, "    device_name: %s",
                       (const char *)pMeta->deviceName);
            CPER_PRINT(seq, pLogPrefix, "    vbios_version: %s",
                       (const char *)pMeta->firmwareVersion);
            CPER_PRINT(seq, pLogPrefix, "    gsp_fw_version: %s",
                       (const char *)pMeta->pfDriverMicrocodeVersion);
            CPER_PRINT(seq, pLogPrefix, "    pf_driver_version: %s",
                       (const char *)pMeta->pfDriverVersion);

            if (pMeta->vfDriverVersion[0] != 0)
                CPER_PRINT(seq, pLogPrefix, "    vf_driver_version: %s",
                           (const char *)pMeta->vfDriverVersion);

            if (pMeta->pdi != 0)
                CPER_PRINT(seq, pLogPrefix, "    pdi: 0x%llx", pMeta->pdi);

            if ((pMeta->pciVendorId != 0) && (pMeta->pciDeviceId != 0))
            {
                CPER_PRINT(seq, pLogPrefix, "    pci: %04x:%04x subsys %04x:%04x rev %02x",
                           pMeta->pciVendorId, pMeta->pciDeviceId,
                           pMeta->pciSubsystemVendorId, pMeta->pciSubsystemId,
                           pMeta->pciRev);
            }

            if (pMeta->architectureId != 0)
                CPER_PRINT(seq, pLogPrefix, "    architecture_id: 0x%x", pMeta->architectureId);

            if (pMeta->bar0Start != 0 && pMeta->bar0Size != 0)
                CPER_PRINT(seq, pLogPrefix, "    bar0: start 0x%llx size 0x%llx",
                           pMeta->bar0Start, pMeta->bar0Size);
            if (pMeta->bar1Start != 0 && pMeta->bar1Size != 0)
                CPER_PRINT(seq, pLogPrefix, "    bar1: start 0x%llx size 0x%llx",
                           pMeta->bar1Start, pMeta->bar1Size);
            if (pMeta->bar2Start != 0 && pMeta->bar2Size != 0)
                CPER_PRINT(seq, pLogPrefix, "    bar2: start 0x%llx size 0x%llx",
                           pMeta->bar2Start, pMeta->bar2Size);
        }
        else if (pCtxHdr->dataFormatType == NV_CPER_NV_DATA_FORMAT_GPU_LEGACY_XID &&
                 pCtxHdr->dataSize >= sizeof(NV_CPER_NV_GPU_LEGACY_XID))
        {
            const NV_CPER_NV_GPU_LEGACY_XID *pLegacyXid =
                (const NV_CPER_NV_GPU_LEGACY_XID *)pCtxData;
            NvU32 msgMax = pCtxHdr->dataSize - (NvU32)sizeof(*pLegacyXid);
            const char *pMsg = (const char *)(pLegacyXid + 1);

            CPER_PRINT(seq, pLogPrefix, "   Event Context %u, type: GPU Legacy Xid", ctxIdx);
            CPER_PRINT(seq, pLogPrefix, "    xid: %u", pLegacyXid->xidCode);
            if (msgMax > 0)
                CPER_PRINT(seq, pLogPrefix, "    message: \"%s\"", pMsg);
        }

        offset += ctxTotalSize;
    }
}
