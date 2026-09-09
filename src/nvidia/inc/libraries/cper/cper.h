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
 * @brief UEFI CPER (Common Platform Error Record) composition library
 *
 * This library provides functions to compose UEFI CPERs as defined in the
 * UEFI Specification Appendix N.
 */

#ifndef CPER_H
#define CPER_H

#include "nvtypes.h"
#include "nvmisc.h"
#include "nvstatus.h"

#include "nvport/nvport.h"

/*
 * ============================================================================
 * CPER Dump Log Prefix Levels
 * ============================================================================
 *
 * These are string tags used by cperDumpRecord() when emitting decoded CPER
 * contents via portDbgDevicePrintf(). The log level is distinct from the
 * severity. These strings are embedded in the printed text alongside the
 * typed PORT_LOG_LEVEL value used for kernel syslog severity routing.
 * 
 * These are defined to match the prefixes used in the Linux kernel.
 */
#define CPER_LOG_LEVEL_HW_ERR  "[Hardware Error]"
#define CPER_LOG_LEVEL_FW_BUG  "[Firmware Bug]"
#define CPER_LOG_LEVEL_FW_WARN "[Firmware Warn]"
#define CPER_LOG_LEVEL_FW_INFO "[Firmware Info]"

/*
 * ============================================================================
 * CPER Dump Print Helpers
 * ============================================================================
 *
 * Convenience macros for emitting CPER dump lines via portDbgDevicePrintf().
 * The device handle is used by the NvPort backend for OS-level device-aware
 * logging (e.g. dev_printk() on Linux); the typed PORT_LOG_LEVEL value is
 * used for kernel syslog severity routing. The prefix string is preserved
 * inside the format text for human readability and consistency with the
 * Linux EDAC layer.
 */
#define CPER_PREFIX_FMT "NVRM: {%u}%s: "
#define CPER_PRINT(pDev, level, seq, prefix, fmt, ...) \
    portDbgDevicePrintf((pDev), (level), CPER_PREFIX_FMT fmt "\n", \
                        (seq), (prefix), ##__VA_ARGS__)

/*
 * ============================================================================
 * Core CPER Types and Structures
 * ============================================================================
 */

/*
 * CPER GUID structure (matches EFI_GUID format)
 */
#pragma pack(1)
typedef struct NV_CPER_GUID
{
    NvU32 data1;
    NvU16 data2;
    NvU16 data3;
    NvU8  data4[8];
} NV_CPER_GUID;
#pragma pack()

/*
 * GUID initializer helper
 *
 * Use this instead of C99 compound literals so GUID macros are usable in both
 * C and C++ initializers under MSVC.
 */
#define NV_CPER_GUID_INIT(_data1, _data2, _data3, _d0, _d1, _d2, _d3, _d4, _d5, _d6, _d7) \
    { (_data1), (_data2), (_data3), { (_d0), (_d1), (_d2), (_d3), (_d4), (_d5), (_d6), (_d7) } }

#define NV_CPER_GUID_FMT "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x"
#define NV_CPER_GUID_FMT_ARGS(pGuid) \
    (pGuid)->data1, \
    (pGuid)->data2, \
    (pGuid)->data3, \
    (pGuid)->data4[0], (pGuid)->data4[1], \
    (pGuid)->data4[2], (pGuid)->data4[3], (pGuid)->data4[4], (pGuid)->data4[5], (pGuid)->data4[6], (pGuid)->data4[7]

/*
 * CPER Signature - "CPER" in little-endian
 */
#define NV_CPER_SIGNATURE 0x52455043

/*
 * CPER Revision - UEFI 2.10 defines revision 0x0104
 */
#define NV_CPER_REVISION_MAJOR 0x01
#define NV_CPER_REVISION_MINOR 0x04
#define NV_CPER_REVISION       ((NV_CPER_REVISION_MAJOR << 8) | NV_CPER_REVISION_MINOR)

/*
 * CPER Section Revision
 */
#define NV_CPER_SECTION_REVISION_MAJOR 0x01
#define NV_CPER_SECTION_REVISION_MINOR 0x00
#define NV_CPER_SECTION_REVISION       ((NV_CPER_SECTION_REVISION_MAJOR << 8) | NV_CPER_SECTION_REVISION_MINOR)

/*
 * CPER Severity levels (matches UEFI specification)
 */
typedef enum NV_CPER_SEVERITY
{
    NV_CPER_SEVERITY_RECOVERABLE   = 0,
    NV_CPER_SEVERITY_FATAL         = 1,
    NV_CPER_SEVERITY_CORRECTED     = 2,
    NV_CPER_SEVERITY_INFORMATIONAL = 3
} NV_CPER_SEVERITY;

static NV_INLINE const char *
cperSeverityToString(NV_CPER_SEVERITY severity)
{
    switch (severity)
    {
        case NV_CPER_SEVERITY_INFORMATIONAL: return "informational";
        case NV_CPER_SEVERITY_CORRECTED:     return "corrected";
        case NV_CPER_SEVERITY_RECOVERABLE:   return "recoverable";
        case NV_CPER_SEVERITY_FATAL:         return "fatal";
        default:                             return "unknown";
    }
}

/*
 * CPER Validation Bits for record header
 */
#define NV_CPER_VALID_PLATFORM_ID               NVBIT(0)
#define NV_CPER_VALID_TIMESTAMP                 NVBIT(1)
#define NV_CPER_VALID_PARTITION_ID              NVBIT(2)

/*
 * CPER Section Descriptor Flags
 */
#define NV_CPER_SECTION_FLAG_PRIMARY            NVBIT(0)
#define NV_CPER_SECTION_FLAG_CONTAINMENT_WARN   NVBIT(1)
#define NV_CPER_SECTION_FLAG_RESET              NVBIT(2)
#define NV_CPER_SECTION_FLAG_THRESHOLD_EXCEED   NVBIT(3)
#define NV_CPER_SECTION_FLAG_RESOURCE_NA        NVBIT(4)
#define NV_CPER_SECTION_FLAG_LATENT_ERROR       NVBIT(5)
#define NV_CPER_SECTION_FLAG_PROPAGATED         NVBIT(6)
#define NV_CPER_SECTION_FLAG_OVERFLOW           NVBIT(7)

/*
 * CPER Section Descriptor Validation Bits
 */
#define NV_CPER_SECTION_VALID_FRU_ID            NVBIT(0)
#define NV_CPER_SECTION_VALID_FRU_TEXT          NVBIT(1)

/*
 * ============================================================================
 * UEFI Standard Notification Type GUIDs
 * ============================================================================
 */

/*!
 * PCI Express Error
 *
 * UUID: cf93c01f-1a16-4dfc-b8bc-9c4daf67c104
 * Defined by the UEFI standard.
 */
#define NV_CPER_NOTIFY_PCIE_GUID \
    NV_CPER_GUID_INIT(0xCF93C01F, 0x1A16, 0x4DFC, 0xB8, 0xBC, 0x9C, 0x4D, 0xAF, 0x67, 0xC1, 0x04)

/*
 * ============================================================================
 * Core CPER Structures (Binary Format)
 * ============================================================================
 */

/*
 * CPER Timestamp structure (BCD encoded)
 */
#pragma pack(1)
typedef struct NV_CPER_TIMESTAMP
{
    NvU8  seconds;
    NvU8  minutes;
    NvU8  hours;
    NvU8  flags;       // Bit 0: Precise, Bits 1-7: Reserved
    NvU8  day;
    NvU8  month;
    NvU8  year;
    NvU8  century;
} NV_CPER_TIMESTAMP;
#pragma pack()

/*
 * CPER Record Header (matches EFI_COMMON_ERROR_RECORD_HEADER)
 * Total size: 128 bytes
 */
#pragma pack(1)
typedef struct NV_CPER_RECORD_HEADER
{
    NvU32             signatureStart;     // "CPER" signature
    NvU16             revision;           // CPER revision
    NvU32             signatureEnd;       // 0xFFFFFFFF
    NvU16             sectionCount;       // Number of sections
    NvU32             errorSeverity;      // Overall error severity
    NvU32             validationBits;     // Which fields are valid
    NvU32             recordLength;       // Total record length including header
    NV_CPER_TIMESTAMP timestamp;          // Timestamp (BCD)
    NV_CPER_GUID      platformId;         // Platform ID
    NV_CPER_GUID      partitionId;        // Partition ID
    NV_CPER_GUID      creatorId;          // Creator ID (identifies creator)
    NV_CPER_GUID      notificationType;   // Notification type GUID
    NvU64             recordId;           // Unique record ID
    NvU32             flags;              // Record flags
    NvU64             persistenceInfo;    // Persistence information
    NvU8              reserved[12];       // Reserved, must be zero
} NV_CPER_RECORD_HEADER;
#pragma pack()

/*
 * CPER Record ID helpers
 *
 * The CPER library encodes a 15-bit sequence counter in the low bits of recordId.
 */
#define NV_CPER_RECORD_ID_SEQUENCE_MASK 0x7FFFull

static NV_INLINE NvU32
cperRecordIdToSequence(NvU64 recordId)
{
    return (NvU32)(recordId & NV_CPER_RECORD_ID_SEQUENCE_MASK);
}

//
// Generate a fresh CPER recordId in the standard encoding (high 49 bits
// = microseconds since the current decade epoch; low 15 bits = atomic
// sequence counter). Producers that need a recordId-format value before
// they have a CPER record on hand — e.g. the OpEventLog group cursor,
// which is stamped into the CPER header on render — should use this so
// the cursor and the recordId share one construction.
//
NvU64 cperGenerateRecordId(void);

/*
 * CPER Section Descriptor (matches EFI_ERROR_SECTION_DESCRIPTOR)
 * Total size: 72 bytes
 */
#pragma pack(1)
typedef struct NV_CPER_SECTION_DESCRIPTOR
{
    NvU32        sectionOffset;       // Offset from start of record to section
    NvU32        sectionLength;       // Length of section data
    NvU16        revision;            // Section revision
    NvU8         validationBits;      // Which fields are valid
    NvU8         reserved;            // Reserved
    NvU32        flags;               // Section flags
    NV_CPER_GUID sectionType;         // Section type GUID
    NV_CPER_GUID fruId;               // FRU ID (optional)
    NvU32        sectionSeverity;     // Section-specific severity
    NvU8         fruText[20];         // FRU text description (optional)
} NV_CPER_SECTION_DESCRIPTOR;
#pragma pack()

/*
 * Size constants
 */
#define NV_CPER_RECORD_HEADER_SIZE      sizeof(NV_CPER_RECORD_HEADER)
#define NV_CPER_SECTION_DESCRIPTOR_SIZE sizeof(NV_CPER_SECTION_DESCRIPTOR)
#define NV_CPER_MAX_FRU_TEXT_LEN        20

/*
 * ============================================================================
 * CPER API Parameter Structures
 * ============================================================================
 */

/**
 * @brief Parameters for initializing a CPER record header
 *
 * The record severity is automatically initialized to INFORMATIONAL and
 * promoted to a higher severity as sections are added (based on the worst
 * section severity).
 */
typedef struct NV_CPER_INIT_PARAMS
{
    const NV_CPER_GUID    *pNotifyType;    ///< Notification type GUID (how error was detected)
    const NV_CPER_GUID    *pCreatorId;     ///< Creator ID GUID (who created this record)
    const NV_CPER_GUID    *pPlatformId;    ///< Platform ID (SMBIOS UUID), NULL if not available
    const NV_CPER_GUID    *pPartitionId;   ///< Partition ID (VM GUID), NULL if not applicable
    NvU64                  timestampUs;    ///< Microsecond wall clock timestamp, or 0 for current time
    NvU64                  recordId;       ///< CPER record ID, or 0 to generate one
    NvBool                 bTimestampPrecise; ///< Whether timestamp is precise (vs. imprecise estimate)
    NvU16                  sectionCount;   ///< Number of section descriptors to pre-allocate
} NV_CPER_INIT_PARAMS;

/**
 * @brief Parameters for adding a generic section to a CPER
 */
typedef struct NV_CPER_SECTION_PARAMS
{
    const NV_CPER_GUID    *pSectionType;   ///< Section type GUID
    NV_CPER_SEVERITY       severity;       ///< Section-specific severity
    NvU32                  flags;          ///< Section flags (NV_CPER_SECTION_FLAG_*)
    const void            *pData;          ///< Section data
    NvU32                  dataSize;       ///< Size of section data in bytes
    const NV_CPER_GUID    *pFruId;         ///< FRU ID (e.g., device UUID), NULL if not available
    const char            *pFruText;       ///< FRU text description, NULL if not available
} NV_CPER_SECTION_PARAMS;

/*
 * ============================================================================
 * CPER API Functions
 * ============================================================================
 */

/**
 * @brief Initialize a CPER record header in a buffer
 *
 * This function initializes a CPER record header at the start of the provided
 * buffer. The timestamp is populated from @ref NV_CPER_INIT_PARAMS::timestampUs
 * when provided, or from the current system wall clock otherwise.
 *
 * @param[out] pBuffer      Buffer to write the CPER header into
 * @param[in]  bufferSize   Size of the buffer in bytes
 * @param[in]  pParams      Initialization parameters
 *
 * @return NV_OK on success
 * @return NV_ERR_INVALID_ARGUMENT if pBuffer or pParams is NULL
 * @return NV_ERR_BUFFER_TOO_SMALL if buffer is too small for the header
 */
NV_STATUS cperInit(void *pBuffer, NvU32 bufferSize, const NV_CPER_INIT_PARAMS *pParams);

/**
 * @brief State for a CPER section being built
 *
 * This state is returned by cperAddSection() and can be used to modify
 * the section after it has been added. The section must be the last
 * section in the record to be modifiable.
 *
 * @note Only the last section in the record can be modified. Attempting
 *       to modify a section that is not at the end of the record will fail.
 */
typedef struct NV_CPER_SECTION_STATE
{
    void  *pBuffer;              ///< The CPER buffer
    NvU32  bufferSize;           ///< Total buffer size
    NvU32  sectionDescOffset;    ///< Offset to section descriptor
    NvU32  sectionDataOffset;    ///< Offset to section data
} NV_CPER_SECTION_STATE;

/**
 * @brief Add a generic section to an existing CPER record
 *
 * This function adds a section descriptor and reserves space for section data
 * in an existing CPER record. The record header's section count and record
 * length are automatically updated.
 *
 * If pParams->pData is non-NULL, the data is copied to the section.
 * If pParams->pData is NULL, space is reserved but not initialized.
 *
 * @param[in,out] pBuffer      Buffer containing the CPER record
 * @param[in]     bufferSize   Total size of the buffer in bytes
 * @param[in]     pParams      Section parameters
 * @param[out]    pState       State for the created section (optional, may be NULL)
 *
 * @return NV_OK on success
 * @return NV_ERR_INVALID_ARGUMENT if any required parameter is NULL
 * @return NV_ERR_BUFFER_TOO_SMALL if buffer is too small
 * @return NV_ERR_INVALID_STATE if buffer doesn't contain a valid CPER header
 */
NV_STATUS cperAddSection(
    void                         *pBuffer,
    NvU32                         bufferSize,
    const NV_CPER_SECTION_PARAMS *pParams,
    NV_CPER_SECTION_STATE        *pState
);

/**
 * @brief Get the current size of a CPER record in a buffer
 *
 * @param[in]  pBuffer     Buffer containing the CPER record
 * @param[in]  bufferSize  Size of the buffer
 * @param[out] pRecordSize Current record size (from header's recordLength)
 *
 * @return NV_OK on success
 * @return NV_ERR_INVALID_ARGUMENT if pBuffer or pRecordSize is NULL
 * @return NV_ERR_INVALID_STATE if buffer doesn't contain a valid CPER header
 */
NV_STATUS cperGetRecordSize(const void *pBuffer, NvU32 bufferSize, NvU32 *pRecordSize);

/**
 * @brief Get pointer to section data from a section state
 *
 * @param[in] pState Section state from cperAddSection()
 *
 * @return Pointer to section data, or NULL if state is invalid
 */
void *cperSectionGetData(const NV_CPER_SECTION_STATE *pState);

/**
 * @brief Get the current section data size
 *
 * @param[in] pState Section state from cperAddSection()
 *
 * @return Current section data size, or 0 if state is invalid
 */
NvU32 cperSectionGetDataSize(const NV_CPER_SECTION_STATE *pState);

/**
 * @brief Adjust the section data size and update record length
 *
 * Use this to shrink or extend a section's data size. The record length
 * is also updated accordingly. When extending, the caller is responsible
 * for ensuring the new data area is properly initialized.
 *
 * @note Only the last section in the record can be resized. Attempting to
 *       resize a section that is not at the end of the record will fail.
 *       This ensures that resizing does not corrupt subsequent sections.
 *
 * @param[in] pState       Section state from cperAddSection()
 * @param[in] newDataSize  New data size
 *
 * @return NV_OK on success
 * @return NV_ERR_INVALID_ARGUMENT if pState is NULL
 * @return NV_ERR_INVALID_STATE if section is not the last section in the record
 * @return NV_ERR_BUFFER_TOO_SMALL if extending would exceed buffer size
 */
NV_STATUS cperSectionSetDataSize(const NV_CPER_SECTION_STATE *pState, NvU32 newDataSize);

/*
 * ============================================================================
 * Utility Functions
 * ============================================================================
 */

/**
 * @brief Compare two GUIDs for equality
 *
 * @param[in] pGuid1 First GUID
 * @param[in] pGuid2 Second GUID
 *
 * @return NV_TRUE if GUIDs are equal, NV_FALSE otherwise
 */
NvBool cperGuidEqual(const NV_CPER_GUID *pGuid1, const NV_CPER_GUID *pGuid2);

/**
 * @brief Check if a GUID is all zeros (null GUID)
 *
 * @param[in] pGuid GUID to check
 *
 * @return NV_TRUE if GUID is null, NV_FALSE otherwise
 */
NvBool cperGuidIsNull(const NV_CPER_GUID *pGuid);

/**
 * @brief Copy a GUID
 *
 * @param[out] pDst Destination GUID
 * @param[in]  pSrc Source GUID
 */
void cperGuidCopy(NV_CPER_GUID *pDst, const NV_CPER_GUID *pSrc);

/**
 * @brief Convert UUID bytes to a CPER GUID (EFI_GUID layout)
 *
 * The byte array is the canonical 16-byte UUID representation used in RM
 * (big-endian within the first 3 groups as printed). CPER GUID fields follow
 * EFI layout.
 *
 * @param[in]  pUuidBytes Source UUID bytes (must be initialized/valid)
 * @param[out] pGuid Destination GUID
 *
 * @return NV_TRUE on success, NV_FALSE on invalid/unused UUID
 */
NvBool cperGuidFromUuidBytes(const NvU8 uuidBytes[16], NV_CPER_GUID *pGuid);

/*!
 * @brief Dump a CPER record to the system log.
 *
 * This helper is intended for in-kernel use (it prints via NV_PRINTF).
 * In non-kernel builds it is a no-op.
 */
void cperDumpRecord(PORT_DEVICE        *pDev,
                    PORT_LOG_LEVEL      level,
                    const void         *pBuffer,
                    NvU32               bufferSize,
                    const char         *pLogPrefix);

/*!
 * @brief Extract the FruId from the first CPER section
 *
 * This helper will be used for ID filtering
 */
NV_STATUS cperGetFirstSectionFruId(const void *pBuffer, NvU32 bufferSize, NV_CPER_GUID *pOutFruId);

#endif /* CPER_H */
