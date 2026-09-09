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
 * @brief UEFI CPER (Common Platform Error Record) composition library implementation
 */

#include <nv_stddef.h>

#include "cper/cper.h"
#include "cper/gpu_cper.h"

#include "nvport/nvport.h"

/*
 * ============================================================================
 * Dump/Decode Helpers (Logging)
 * ============================================================================
 */

static NvBool _isValidCperHeader(const void *pBuffer, NvU32 bufferSize);

void cperDumpRecord(PORT_DEVICE        *pDev,
                    PORT_LOG_LEVEL      level,
                    const void         *pBuffer,
                    NvU32               bufferSize,
                    const char         *pLogPrefix)
{
    const NV_CPER_RECORD_HEADER *pHdr;
    NvU32 i;
    NvU32 eventIdx = 0;
    NvU32 seq;

    if (pLogPrefix == NULL)
        pLogPrefix = CPER_LOG_LEVEL_FW_INFO;

    if (pBuffer == NULL || bufferSize < NV_CPER_RECORD_HEADER_SIZE)
    {
        portDbgDevicePrintf(pDev, PORT_LOG_LEVEL_ERROR, "invalid CPER buffer\n");
        return;
    }

    if (!_isValidCperHeader(pBuffer, bufferSize))
    {
        portDbgDevicePrintf(pDev, PORT_LOG_LEVEL_ERROR, "invalid CPER signature\n");
        return;
    }

    pHdr = (const NV_CPER_RECORD_HEADER *)pBuffer;
    seq = cperRecordIdToSequence(pHdr->recordId);

    // For now this is NVIDIA-specific text formatting
    cperNvidiaEventDumpRecordHeader(pDev, level, pHdr, seq, pLogPrefix);

    for (i = 0; i < pHdr->sectionCount; i++)
    {
        const NV_CPER_SECTION_DESCRIPTOR *pDesc;
        const NvU8 *pSection;
        NvU32 sectionOffset;
        NvU32 sectionLength;
        static const NV_CPER_GUID nvEventSectionType = NV_CPER_SECTION_NVIDIA_EVENT_GUID;

        pDesc = (const NV_CPER_SECTION_DESCRIPTOR *)((const NvU8 *)pBuffer +
                NV_CPER_RECORD_HEADER_SIZE + (i * NV_CPER_SECTION_DESCRIPTOR_SIZE));

        if (cperGuidIsNull(&pDesc->sectionType))
            continue;

        sectionOffset = pDesc->sectionOffset;
        sectionLength = pDesc->sectionLength;

        if (sectionOffset + sectionLength > pHdr->recordLength ||
            sectionOffset + sectionLength > bufferSize ||
            sectionOffset < NV_CPER_RECORD_HEADER_SIZE)
        {
            CPER_PRINT(pDev, level, seq, pLogPrefix, " Event %u, type: %s", eventIdx,
                       cperSeverityToString((NV_CPER_SEVERITY)pDesc->sectionSeverity));
            CPER_PRINT(pDev, level, seq, pLogPrefix, " decoding_error: section out of bounds");
            eventIdx++;
            continue;
        }

        pSection = (const NvU8 *)pBuffer + sectionOffset;

        if (cperGuidEqual(&pDesc->sectionType, &nvEventSectionType))
        {
            cperNvidiaEventDumpSection(pDev, level, eventIdx, pHdr, pDesc, pSection,
                                       sectionLength, seq, pLogPrefix);
        }
        else
        {
            CPER_PRINT(pDev, level, seq, pLogPrefix, " Event %u, type: %s", eventIdx,
                       cperSeverityToString((NV_CPER_SEVERITY)pDesc->sectionSeverity));
            CPER_PRINT(pDev, level, seq, pLogPrefix, "  section_type: " NV_CPER_GUID_FMT,
                       NV_CPER_GUID_FMT_ARGS(&pDesc->sectionType));
        }

        eventIdx++;
    }

    return;
}

NvBool cperGuidFromUuidBytes
(
    const NvU8     uuidBytes[16],
    NV_CPER_GUID  *pGuid
)
{
    if (pGuid == NULL)
        return NV_FALSE;

    portMemSet(pGuid, 0, sizeof(*pGuid));

    if (uuidBytes == NULL)
        return NV_FALSE;

    //
    // Convert canonical UUID bytes into EFI GUID fields for CPER.
    // UUID byte order is as printed: 8-4-4-4-12 (big-endian within the first 3 groups).
    //
    pGuid->data1 = ((NvU32)uuidBytes[0] << 24) |
                   ((NvU32)uuidBytes[1] << 16) |
                   ((NvU32)uuidBytes[2] <<  8) |
                   ((NvU32)uuidBytes[3] <<  0);
    pGuid->data2 = (NvU16)(((NvU16)uuidBytes[4] << 8) |
                           ((NvU16)uuidBytes[5] << 0));
    pGuid->data3 = (NvU16)(((NvU16)uuidBytes[6] << 8) |
                           ((NvU16)uuidBytes[7] << 0));
    portMemCopy(pGuid->data4, sizeof(pGuid->data4), &uuidBytes[8], sizeof(pGuid->data4));

    return NV_TRUE;
}

/*
 * Convert a value to BCD (Binary-Coded Decimal)
 */
static NvU8 _toBcd(NvU8 value)
{
    return (NvU8)(((value / 10) << 4) | (value % 10));
}

/*
 * Convert CPER severity to a rank for comparison purposes.
 * Higher rank = more severe.
 * Order: INFORMATIONAL < CORRECTED < RECOVERABLE < FATAL
 */
static inline NvU32 _severityRank(NV_CPER_SEVERITY severity)
{
    static const NvU8 sSeverityRank[] = {
        [NV_CPER_SEVERITY_RECOVERABLE] = 2,
        [NV_CPER_SEVERITY_FATAL] = 3,
        [NV_CPER_SEVERITY_CORRECTED] = 1,
        [NV_CPER_SEVERITY_INFORMATIONAL] = 0,
    };

    return sSeverityRank[severity];
}

/*
 * Check if newSeverity is higher (more severe) than currentSeverity
 */
static NvBool _severityIsHigher(NV_CPER_SEVERITY newSeverity, NV_CPER_SEVERITY currentSeverity)
{
    return _severityRank(newSeverity) > _severityRank(currentSeverity);
}

/*
 * Get the Unix epoch time in microseconds for the start of a decade.
 * Decade is determined by the year: 2020-2029 -> 2020, 2030-2039 -> 2030, etc.
 */
static NvU64 _getDecadeEpochMicroseconds(void)
{
    PORT_WALLTIME decadeStart = {0};
    PORT_WALLTIME wallTime = portTimeGetLocalWallTime();

    // If walltime isn't supported, don't bother with the remaining logic
    if (wallTime.year == 0)
        return 0;

    decadeStart.year = (wallTime.year / 10) * 10;
    decadeStart.month = 1;
    decadeStart.day = 1;

    return portTimeConvertToUnixMs(decadeStart) * 1000ULL;
}

//
// Generate a unique record ID.
// High 49 bits: microseconds since the start of the current decade.
// Low 15 bits: atomic counter.
//
// The decade is derived from the current year (2020-2029 -> 2020, etc.).
// 49 bits of microseconds covers ~17.8 years, fully covering any decade.
// Combined with the BCD timestamp in the record header (which has the full
// year), this provides unique identification without Y2K38 issues.
//
// Public so that producers like the OpEventLog can construct group cursors in
// the same encoding and avoid divergence when the cursor is later written into
// the CPER record header.
//
NvU64 cperGenerateRecordId(void)
{
    static PORT_ATOMIC NvU32 sCounter = 0;
    NvU64 currentUs;
    NvU64 epochUs;
    NvU64 usFromEpoch;
    NvU32 counterValue;

    currentUs = portTimeGetMicroseconds();
    epochUs = _getDecadeEpochMicroseconds();

    usFromEpoch = currentUs - epochUs;
    counterValue = portAtomicIncrementU32(&sCounter) & 0x7FFF;  // 15 bits

    return (usFromEpoch << 15) | counterValue;
}

/*
 * Populate timestamp from a supplied microsecond wall clock, or current time.
 */
static NvBool _populateTimestamp
(
    NV_CPER_TIMESTAMP *pTimestamp,
    NvU64              timestampUs,
    NvBool             bPrecise
)
{
    PORT_WALLTIME wallTime = {0};

    if (timestampUs != 0)
        wallTime = portTimeConvertToWallTime(timestampUs / 1000ULL);
    else
        wallTime = portTimeGetLocalWallTime();

    if (wallTime.year == 0)
        return NV_FALSE;

    pTimestamp->seconds = _toBcd((NvU8)wallTime.second);
    pTimestamp->minutes = _toBcd((NvU8)wallTime.minute);
    pTimestamp->hours   = _toBcd((NvU8)wallTime.hour);
    pTimestamp->day     = _toBcd((NvU8)wallTime.day);
    pTimestamp->month   = _toBcd((NvU8)wallTime.month);
    pTimestamp->year    = _toBcd((NvU8)(wallTime.year % 100));
    pTimestamp->century = _toBcd((NvU8)(wallTime.year / 100));
    pTimestamp->flags   = bPrecise ? 0x01 : 0x00;
    return NV_TRUE;
}

/*
 * Validate that a buffer contains a valid CPER header
 */
static NvBool _isValidCperHeader(const void *pBuffer, NvU32 bufferSize)
{
    const NV_CPER_RECORD_HEADER *pHeader;

    if (pBuffer == NULL || bufferSize < NV_CPER_RECORD_HEADER_SIZE)
    {
        return NV_FALSE;
    }

    pHeader = (const NV_CPER_RECORD_HEADER *)pBuffer;

    return (pHeader->signatureStart == NV_CPER_SIGNATURE &&
            pHeader->signatureEnd == 0xFFFFFFFF);
}

/*
 * Find an unused pre-allocated section descriptor.
 * An unused descriptor has a null section type GUID (zero-initialized).
 * Returns the index of the unused descriptor, or sectionCount if none found.
 */
static NvU32 _findUnusedDescriptor(const void *pBuffer, NvU16 sectionCount)
{
    const NV_CPER_SECTION_DESCRIPTOR *pDesc;
    NvU32 i;

    for (i = 0; i < sectionCount; i++)
    {
        pDesc = (const NV_CPER_SECTION_DESCRIPTOR *)((const NvU8 *)pBuffer +
                NV_CPER_RECORD_HEADER_SIZE + (i * NV_CPER_SECTION_DESCRIPTOR_SIZE));

        if (cperGuidIsNull(&pDesc->sectionType))
            return i;
    }

    return sectionCount;
}

/*
 * Shift existing section data forward to make room for a new descriptor.
 * Also updates existing descriptor offsets.
 */
static void _shiftSectionData
(
    void  *pBuffer,
    NvU32  bufferSize,
    NvU16  sectionCount,
    NvU32  currentSize
)
{
    NV_CPER_SECTION_DESCRIPTOR *pDesc;
    NvU32 descriptorOffset;
    NvU32 i;

    descriptorOffset = NV_CPER_RECORD_HEADER_SIZE +
                       (sectionCount * NV_CPER_SECTION_DESCRIPTOR_SIZE);

    // Only shift if there's existing section data after the descriptors
    if (currentSize > descriptorOffset)
    {
        NvU32 dataStart = descriptorOffset;
        NvU32 dataSize = currentSize - descriptorOffset;

        // Move data forward by one descriptor size
        portMemMove((NvU8 *)pBuffer + dataStart + NV_CPER_SECTION_DESCRIPTOR_SIZE,
                    bufferSize - dataStart - NV_CPER_SECTION_DESCRIPTOR_SIZE,
                    (NvU8 *)pBuffer + dataStart,
                    dataSize);

        // Update existing section descriptors' offsets
        for (i = 0; i < sectionCount; i++)
        {
            pDesc = (NV_CPER_SECTION_DESCRIPTOR *)((NvU8 *)pBuffer +
                    NV_CPER_RECORD_HEADER_SIZE + (i * NV_CPER_SECTION_DESCRIPTOR_SIZE));
            if (!cperGuidIsNull(&pDesc->sectionType))
            {
                pDesc->sectionOffset += NV_CPER_SECTION_DESCRIPTOR_SIZE;
            }
        }
    }
}

/*
 * Populate a section descriptor with the given parameters.
 */
static void _populateDescriptor
(
    NV_CPER_SECTION_DESCRIPTOR       *pDesc,
    NvU32                             sectionOffset,
    const NV_CPER_SECTION_PARAMS     *pParams
)
{
    portMemSet(pDesc, 0, sizeof(*pDesc));

    pDesc->sectionOffset = sectionOffset;
    pDesc->sectionLength = pParams->dataSize;
    pDesc->revision = NV_CPER_SECTION_REVISION;
    pDesc->validationBits = 0;
    pDesc->flags = pParams->flags;
    pDesc->sectionSeverity = (NvU32)pParams->severity;

    cperGuidCopy(&pDesc->sectionType, pParams->pSectionType);

    // Set optional FRU ID
    if (pParams->pFruId != NULL)
    {
        cperGuidCopy(&pDesc->fruId, pParams->pFruId);
        pDesc->validationBits |= NV_CPER_SECTION_VALID_FRU_ID;
    }

    // Set optional FRU text
    if (pParams->pFruText != NULL)
    {
        portStringCopy((char *)pDesc->fruText, NV_CPER_MAX_FRU_TEXT_LEN,
                       pParams->pFruText, NV_CPER_MAX_FRU_TEXT_LEN);
        pDesc->validationBits |= NV_CPER_SECTION_VALID_FRU_TEXT;
    }
}

/*
 * ============================================================================
 * Public API Implementation
 * ============================================================================
 */

NV_STATUS cperInit
(
    void                      *pBuffer,
    NvU32                      bufferSize,
    const NV_CPER_INIT_PARAMS *pParams
)
{
    NV_CPER_RECORD_HEADER *pHeader;
    NvU32 requiredSize;

    if (pBuffer == NULL || pParams == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Notification type and creator ID are required
    if (pParams->pNotifyType == NULL || pParams->pCreatorId == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Calculate required size including pre-allocated section descriptors
    requiredSize = NV_CPER_RECORD_HEADER_SIZE +
                   (pParams->sectionCount * NV_CPER_SECTION_DESCRIPTOR_SIZE);

    if (bufferSize < requiredSize)
    {
        return NV_ERR_BUFFER_TOO_SMALL;
    }

    portMemSet(pBuffer, 0, bufferSize);

    pHeader = (NV_CPER_RECORD_HEADER *)pBuffer;

    // Initialize fixed fields
    pHeader->signatureStart = NV_CPER_SIGNATURE;
    pHeader->revision = NV_CPER_REVISION;
    pHeader->signatureEnd = 0xFFFFFFFF;
    pHeader->sectionCount = pParams->sectionCount;
    pHeader->errorSeverity = (NvU32)NV_CPER_SEVERITY_INFORMATIONAL;
    pHeader->recordLength = requiredSize;
    pHeader->recordId = (pParams->recordId != 0) ? pParams->recordId : cperGenerateRecordId();
    pHeader->flags = 0;
    pHeader->persistenceInfo = 0;

    // Populate timestamp from caller-supplied or current wall clock.
    if (_populateTimestamp(&pHeader->timestamp, pParams->timestampUs, pParams->bTimestampPrecise))
        pHeader->validationBits = NV_CPER_VALID_TIMESTAMP;
    else
        pHeader->validationBits = 0;

    // Copy notification type and creator ID
    cperGuidCopy(&pHeader->notificationType, pParams->pNotifyType);
    cperGuidCopy(&pHeader->creatorId, pParams->pCreatorId);

    // Set optional platform ID
    if (pParams->pPlatformId != NULL)
    {
        cperGuidCopy(&pHeader->platformId, pParams->pPlatformId);
        pHeader->validationBits |= NV_CPER_VALID_PLATFORM_ID;
    }

    // Set optional partition ID
    if (pParams->pPartitionId != NULL)
    {
        cperGuidCopy(&pHeader->partitionId, pParams->pPartitionId);
        pHeader->validationBits |= NV_CPER_VALID_PARTITION_ID;
    }

    return NV_OK;
}

NV_STATUS cperAddSection
(
    void                         *pBuffer,
    NvU32                         bufferSize,
    const NV_CPER_SECTION_PARAMS *pParams,
    NV_CPER_SECTION_STATE        *pState
)
{
    NV_CPER_RECORD_HEADER *pHeader;
    NV_CPER_SECTION_DESCRIPTOR *pDesc;
    NvU32 currentSize;
    NvU32 requiredSize;
    NvU32 sectionOffset;
    NvU32 descriptorOffset;
    NvU32 descriptorIdx;

    if (pBuffer == NULL || pParams == NULL || pParams->pSectionType == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    // NOTE: pData == NULL with dataSize > 0 is valid - caller reserves space and writes directly

    // cperInit() must have been called already
    if (!_isValidCperHeader(pBuffer, bufferSize))
        return NV_ERR_INVALID_STATE;

    pHeader = (NV_CPER_RECORD_HEADER *)pBuffer;
    currentSize = pHeader->recordLength;

    requiredSize = currentSize + pParams->dataSize;
    descriptorOffset = NV_CPER_RECORD_HEADER_SIZE;
    sectionOffset = currentSize;

    if (requiredSize > bufferSize)
        return NV_ERR_BUFFER_TOO_SMALL;

    //
    // Layout: [Header][Desc0][Desc1]...[DescN][Data0][Data1]...[DataN]
    //
    // First, scan for an unused pre-allocated section descriptor.
    // If found, use it directly without shifting existing data.
    // Otherwise, add a new descriptor and shift existing section data.
    //
    descriptorIdx = _findUnusedDescriptor(pBuffer, pHeader->sectionCount);
    descriptorOffset += descriptorIdx * NV_CPER_SECTION_DESCRIPTOR_SIZE;
    if (descriptorIdx == pHeader->sectionCount)
    {
        // No unused descriptor - need to add a new one and shift existing data
        requiredSize += NV_CPER_SECTION_DESCRIPTOR_SIZE;
        sectionOffset += NV_CPER_SECTION_DESCRIPTOR_SIZE;

        if (requiredSize > bufferSize)
            return NV_ERR_BUFFER_TOO_SMALL;

        _shiftSectionData(pBuffer, bufferSize, pHeader->sectionCount, currentSize);

        pHeader->sectionCount++;
    }

    // Initialize the section descriptor
    pDesc = (NV_CPER_SECTION_DESCRIPTOR *)((NvU8 *)pBuffer + descriptorOffset);
    _populateDescriptor(pDesc, sectionOffset, pParams);

    // Copy section data if provided
    if (pParams->dataSize > 0 && pParams->pData != NULL)
    {
        NvU8 *pDst = (NvU8 *)pBuffer + sectionOffset;
        portMemCopy(pDst, bufferSize - sectionOffset, pParams->pData, pParams->dataSize);
    }

    // Update record length
    pHeader->recordLength = requiredSize;

    // Raise record severity if section severity is higher
    if (_severityIsHigher(pParams->severity, (NV_CPER_SEVERITY)pHeader->errorSeverity))
        pHeader->errorSeverity = (NvU32)pParams->severity;

    // Populate state handle if requested
    if (pState != NULL)
    {
        pState->pBuffer = pBuffer;
        pState->bufferSize = bufferSize;
        pState->sectionDescOffset = descriptorOffset;
        pState->sectionDataOffset = sectionOffset;
    }

    return NV_OK;
}

NV_STATUS cperGetRecordSize
(
    const void *pBuffer,
    NvU32       bufferSize,
    NvU32      *pRecordSize
)
{
    const NV_CPER_RECORD_HEADER *pHeader;

    if (pBuffer == NULL || pRecordSize == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    if (!_isValidCperHeader(pBuffer, bufferSize))
    {
        return NV_ERR_INVALID_STATE;
    }

    pHeader = (const NV_CPER_RECORD_HEADER *)pBuffer;
    *pRecordSize = pHeader->recordLength;

    return NV_OK;
}

/*
 * ============================================================================
 * Section Handle Functions
 * ============================================================================
 */

void *cperSectionGetData(const NV_CPER_SECTION_STATE *pState)
{
    if (pState == NULL || pState->pBuffer == NULL)
        return NULL;

    return (NvU8 *)pState->pBuffer + pState->sectionDataOffset;
}

static NV_CPER_SECTION_DESCRIPTOR *_getSectionDescriptor(const NV_CPER_SECTION_STATE *pState)
{
    if (pState == NULL || pState->pBuffer == NULL)
        return NULL;

    return (NV_CPER_SECTION_DESCRIPTOR *)((NvU8 *)pState->pBuffer +
                                           pState->sectionDescOffset);
}

/*
 * Check if the section in the given state is the last section in the record.
 * Only the last section can be safely resized.
 */
static NvBool _isSectionLast(const NV_CPER_SECTION_STATE *pState)
{
    NV_CPER_SECTION_DESCRIPTOR *pDesc;
    NV_CPER_RECORD_HEADER *pHeader;
    NvU32 sectionEnd;

    if (pState == NULL || pState->pBuffer == NULL)
        return NV_FALSE;

    pDesc = _getSectionDescriptor(pState);
    if (pDesc == NULL)
        return NV_FALSE;

    pHeader = (NV_CPER_RECORD_HEADER *)pState->pBuffer;

    // Section is last if its data ends at the record length
    sectionEnd = pState->sectionDataOffset + pDesc->sectionLength;
    return (sectionEnd == pHeader->recordLength);
}

NvU32 cperSectionGetDataSize(const NV_CPER_SECTION_STATE *pState)
{
    NV_CPER_SECTION_DESCRIPTOR *pDesc = _getSectionDescriptor(pState);
    if (pDesc == NULL)
        return 0;

    return pDesc->sectionLength;
}

NV_STATUS cperSectionSetDataSize
(
    const NV_CPER_SECTION_STATE *pState,
    NvU32                        newDataSize
)
{
    NV_CPER_SECTION_DESCRIPTOR *pDesc;
    NV_CPER_RECORD_HEADER *pHeader;
    NvU32 oldDataSize;
    NvS32 sizeDelta;
    NvU32 newRecordLength;

    if (pState == NULL || pState->pBuffer == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    // Only the last section in the record can be resized
    if (!_isSectionLast(pState))
        return NV_ERR_INVALID_STATE;

    pDesc = _getSectionDescriptor(pState);
    if (pDesc == NULL)
        return NV_ERR_INVALID_ARGUMENT;

    pHeader = (NV_CPER_RECORD_HEADER *)pState->pBuffer;
    oldDataSize = pDesc->sectionLength;
    sizeDelta = (NvS32)newDataSize - (NvS32)oldDataSize;
    newRecordLength = (NvU32)((NvS32)pHeader->recordLength + sizeDelta);

    // Check buffer bounds when extending
    if (newRecordLength > pState->bufferSize)
        return NV_ERR_BUFFER_TOO_SMALL;

    pDesc->sectionLength = newDataSize;
    pHeader->recordLength = newRecordLength;

    return NV_OK;
}

/*
 * ============================================================================
 * GUID Utility Functions
 * ============================================================================
 */

NvBool cperGuidEqual(
    const NV_CPER_GUID *pGuid1,
    const NV_CPER_GUID *pGuid2
)
{
    if (pGuid1 == NULL || pGuid2 == NULL)
    {
        return NV_FALSE;
    }

    return (pGuid1->data1 == pGuid2->data1 &&
            pGuid1->data2 == pGuid2->data2 &&
            pGuid1->data3 == pGuid2->data3 &&
            portMemCmp(pGuid1->data4, pGuid2->data4, 8) == 0);
}

NvBool cperGuidIsNull(const NV_CPER_GUID *pGuid)
{
    static const NV_CPER_GUID nullGuid = {0};

    if (pGuid == NULL)
    {
        return NV_TRUE;
    }

    return cperGuidEqual(pGuid, &nullGuid);
}

void cperGuidCopy(
    NV_CPER_GUID       *pDst,
    const NV_CPER_GUID *pSrc
)
{
    if (pDst == NULL || pSrc == NULL)
    {
        return;
    }

    pDst->data1 = pSrc->data1;
    pDst->data2 = pSrc->data2;
    pDst->data3 = pSrc->data3;
    portMemCopy(pDst->data4, sizeof(pDst->data4), pSrc->data4, sizeof(pSrc->data4));
}

NV_STATUS cperGetFirstSectionFruId
(
    const void   *pBuffer,
    NvU32         bufferSize,
    NV_CPER_GUID *pOutFruId
)
{
    const NV_CPER_RECORD_HEADER *pHeader;
    const NV_CPER_SECTION_DESCRIPTOR *pDesc;

    if (pBuffer == NULL || pOutFruId == NULL)
    {
        return NV_ERR_INVALID_ARGUMENT;
    }

    // Validate the buffer contains a properly formed CPER header
    if (!_isValidCperHeader(pBuffer, bufferSize))
    {
        return NV_ERR_INVALID_STATE;
    }

    pHeader = (const NV_CPER_RECORD_HEADER *)pBuffer;

    if (pHeader->sectionCount == 0)
    {
        // No sections exist in this record
        return NV_ERR_INVALID_INDEX;
    }

    // The first section descriptor always immediately follows the record header
    pDesc = (const NV_CPER_SECTION_DESCRIPTOR *)((const NvU8 *)pBuffer + NV_CPER_RECORD_HEADER_SIZE);

    // Verify that the FRU ID field is populated/valid for this section
    if ((pDesc->validationBits & NV_CPER_SECTION_VALID_FRU_ID) == 0)
    {
        return NV_ERR_GPU_UUID_NOT_FOUND;
    }

    // Safely copy the FRU ID to the output parameter
    cperGuidCopy(pOutFruId, &pDesc->fruId);

    return NV_OK;
}
