/*
 * SPDX-FileCopyrightText: Copyright (c) 2021-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*!
 * KernelGsp functions and helpers for parsing FWSEC ucode from a
 * VBIOS image.
 *
 * TODO: JIRA CORERM-4685: Consider moving stuff in here to, e.g. KernelVbios
 */

#include "gpu/gsp/kernel_gsp.h"

#include "gpu/mem_mgr/mem_mgr.h"
#include "gpu/gpu.h"
#include "gpu/vbios/bios_types.h"
#include "gpu/mem_mgr/mem_desc.h"

// ---------------------------------------------------------------------------
// BIOS Information Table (BIT) structures and defines
// (header, tokens, falcon ucodes)
// ---------------------------------------------------------------------------

#define BIT_HEADER_ID                     0xB8FF
#define BIT_HEADER_SIGNATURE              0x00544942  // "BIT\0"
#define BIT_HEADER_SIZE_OFFSET            8

struct BIT_HEADER_V1_00
{
    bios_U016 Id;
    bios_U032 Signature;
    bios_U016 BCD_Version;
    bios_U008 HeaderSize;
    bios_U008 TokenSize;
    bios_U008 TokenEntries;
    bios_U008 HeaderChksum;
};
#define BIT_HEADER_V1_00_FMT "1w1d1w4b"
typedef struct BIT_HEADER_V1_00 BIT_HEADER_V1_00;

struct BIT_TOKEN_V1_00
{
    bios_U008 TokenId;
    bios_U008 DataVersion;
    bios_U016 DataSize;
    bios_U032 DataPtr;
};

#define BIT_TOKEN_V1_00_SIZE_6     6U
#define BIT_TOKEN_V1_00_SIZE_8     8U

#define BIT_TOKEN_V1_00_FMT_SIZE_6 "2b2w"
#define BIT_TOKEN_V1_00_FMT_SIZE_8 "2b1w1d"
typedef struct BIT_TOKEN_V1_00 BIT_TOKEN_V1_00;

#define BIT_TOKEN_BIOSDATA          0x42

// structure for only version info from BIT_DATA_BIOSDATA_V1 and BIT_DATA_BIOSDATA_V2
typedef struct
{
    bios_U032 Version;     // BIOS Binary Version Ex. 5.40.00.01.12 = 0x05400001
    bios_U008 OemVersion;  // OEM Version Number  Ex. 5.40.00.01.12 = 0x12
} BIT_DATA_BIOSDATA_BINVER;

#define BIT_DATA_BIOSDATA_VERSION_1         0x1
#define BIT_DATA_BIOSDATA_VERSION_2         0x2

#define BIT_DATA_BIOSDATA_BINVER_FMT "1d1b"
#define BIT_DATA_BIOSDATA_BINVER_SIZE_5    5

#define BIT_TOKEN_FALCON_DATA       0x70

typedef struct
{
    bios_U032 FalconUcodeTablePtr;
} BIT_DATA_FALCON_DATA_V2;

#define BIT_DATA_FALCON_DATA_V2_4_FMT       "1d"
#define BIT_DATA_FALCON_DATA_V2_SIZE_4      4

typedef struct
{
    bios_U008 Version;
    bios_U008 HeaderSize;
    bios_U008 EntrySize;
    bios_U008 EntryCount;
    bios_U008 DescVersion;
    bios_U008 DescSize;
} FALCON_UCODE_TABLE_HDR_V1;

#define FALCON_UCODE_TABLE_HDR_V1_VERSION   1
#define FALCON_UCODE_TABLE_HDR_V1_SIZE_6    6
#define FALCON_UCODE_TABLE_HDR_V1_6_FMT     "6b"

typedef struct
{
    bios_U008 ApplicationID;
    bios_U008 TargetID;
    bios_U032 DescPtr;
} FALCON_UCODE_TABLE_ENTRY_V1;

#define FALCON_UCODE_TABLE_ENTRY_V1_VERSION             1
#define FALCON_UCODE_TABLE_ENTRY_V1_SIZE_6              6
#define FALCON_UCODE_TABLE_ENTRY_V1_6_FMT               "2b1d"

#define FALCON_UCODE_ENTRY_APPID_FIRMWARE_SEC_LIC       0x05
#define FALCON_UCODE_ENTRY_APPID_FWSEC_DBG              0x45
#define FALCON_UCODE_ENTRY_APPID_FWSEC_PROD             0x85

#define NV_BIT_FALCON_UCODE_DESC_HEADER_VDESC_FLAGS_VERSION                0:0
#define NV_BIT_FALCON_UCODE_DESC_HEADER_VDESC_FLAGS_VERSION_UNAVAILABLE    0x00
#define NV_BIT_FALCON_UCODE_DESC_HEADER_VDESC_FLAGS_VERSION_AVAILABLE      0x01
#define NV_BIT_FALCON_UCODE_DESC_HEADER_VDESC_FLAGS_RESERVED               1:1
#define NV_BIT_FALCON_UCODE_DESC_HEADER_VDESC_FLAGS_ENCRYPTED              2:2
#define NV_BIT_FALCON_UCODE_DESC_HEADER_VDESC_RESERVED                     7:3
#define NV_BIT_FALCON_UCODE_DESC_HEADER_VDESC_VERSION                      15:8
#define NV_BIT_FALCON_UCODE_DESC_HEADER_VDESC_VERSION_V1                   0x01
#define NV_BIT_FALCON_UCODE_DESC_HEADER_VDESC_VERSION_V2                   0x02
#define NV_BIT_FALCON_UCODE_DESC_HEADER_VDESC_VERSION_V3                   0x03
#define NV_BIT_FALCON_UCODE_DESC_HEADER_VDESC_VERSION_V4                   0x04
#define NV_BIT_FALCON_UCODE_DESC_HEADER_VDESC_SIZE                         31:16

typedef struct
{
    bios_U032 vDesc;
} FALCON_UCODE_DESC_HEADER;
#define FALCON_UCODE_DESC_HEADER_FORMAT   "1d"

typedef struct
{
    FALCON_UCODE_DESC_HEADER Hdr;
    bios_U032 StoredSize;
    bios_U032 UncompressedSize;
    bios_U032 VirtualEntry;
    bios_U032 InterfaceOffset;
    bios_U032 IMEMPhysBase;
    bios_U032 IMEMLoadSize;
    bios_U032 IMEMVirtBase;
    bios_U032 IMEMSecBase;
    bios_U032 IMEMSecSize;
    bios_U032 DMEMOffset;
    bios_U032 DMEMPhysBase;
    bios_U032 DMEMLoadSize;
    bios_U032 altIMEMLoadSize;
    bios_U032 altDMEMLoadSize;
} FALCON_UCODE_DESC_V2;

#define FALCON_UCODE_DESC_V2_SIZE_60    60
#define FALCON_UCODE_DESC_V2_60_FMT     "15d"

typedef struct {
    FALCON_UCODE_DESC_HEADER Hdr;
    bios_U032 StoredSize;
    bios_U032 PKCDataOffset;
    bios_U032 InterfaceOffset;
    bios_U032 IMEMPhysBase;
    bios_U032 IMEMLoadSize;
    bios_U032 IMEMVirtBase;
    bios_U032 DMEMPhysBase;
    bios_U032 DMEMLoadSize;
    bios_U016 EngineIdMask;
    bios_U008 UcodeId;
    bios_U008 SignatureCount;
    bios_U016 SignatureVersions;
    bios_U016 Reserved;
} FALCON_UCODE_DESC_V3;

#define FALCON_UCODE_DESC_V3_SIZE_44    44
#define FALCON_UCODE_DESC_V3_44_FMT     "9d1w2b2w"
#define BCRT30_RSA3K_SIG_SIZE 384

typedef union
{
    // v1 is unused on platforms supported by GSP-RM
    FALCON_UCODE_DESC_V2  v2;
    FALCON_UCODE_DESC_V3  v3;
} FALCON_UCODE_DESC_UNION;

typedef struct FlcnUcodeDescFromBit
{
    NvU32 descVersion;
    NvU32 descOffset;
    NvU32 descSize;
    FALCON_UCODE_DESC_UNION descUnion;
} FlcnUcodeDescFromBit;


// ---------------------------------------------------------------------------
// Functions for parsing FWSEC falcon ucode from VBIOS image
// ---------------------------------------------------------------------------

/*!
 * Calculate packed data size based on given data format
 *
 * @param[in]   format          Data format
 */
static NvU32
s_biosStructCalculatePackedSize
(
    const char *format
)
{

    NvU32 packedSize = 0;
    NvU32 count;
    char fmt;

    while ((fmt = *format++))
    {
        count = 0;
        while ((fmt >= '0') && (fmt <= '9'))
        {
            count *= 10;
            count += fmt - '0';
            fmt = *format++;
        }
        if (count == 0)
            count = 1;

        switch (fmt)
        {
            case 'b':
                packedSize += count * 1;
                break;

            case 's':    // signed byte
                packedSize += count * 1;
                break;

            case 'w':
                packedSize += count * 2;
                break;

            case 'd':
                packedSize += count * 4;
                break;
        }
    }

    return packedSize;
}

/*!
 * Parse packed little endian data and unpack into padded structure.
 *
 * @param[in]   packedData      Packed little endien data
 * @param[out]  unpackedData    Unpacked padded structure
 * @param[in]   format          Data format
 */
static NV_STATUS
s_biosUnpackStructure
(
    const NvU8 *packedData,
    NvU32 *unpackedData,  // out
    const char *format
)
{

    NvU32 count;
    NvU32 data;
    char fmt;

    while ((fmt = *format++))
    {
        count = 0;
        while ((fmt >= '0') && (fmt <= '9'))
        {
            count *= 10;
            count += fmt - '0';
            fmt = *format++;
        }
        if (count == 0)
            count = 1;

        while (count--)
        {
            switch (fmt)
            {
                case 'b':
                    data = *packedData++;
                    break;

                case 's':    // signed byte
                    data = *packedData++;
                    if (data & 0x80)
                        data |= ~0xff;
                    break;

                case 'w':
                    data  = *packedData++;
                    data |= *packedData++ << 8;
                    break;

                case 'd':
                    data  = *packedData++;
                    data |= *packedData++ << 8;
                    data |= *packedData++ << 16;
                    data |= *packedData++ << 24;
                    break;

                default:
                    return NV_ERR_GENERIC;
            }
            *unpackedData++ = data;
        }
    }

    return NV_OK;
}

/*!
 * Read packed little endian data and unpack it to padded structure.
 *
 * @param[in]   pVbiosImg   VBIOS image containing packed little endien data
 * @param[out]  pStructure  Unpacked padded structure
 * @param[in]   offset      Offset within packed data
 * @param[in]   format      Data format
 */
static NV_STATUS
s_vbiosReadStructure
(
    const KernelGspVbiosImg * const pVbiosImg,
    void *pStructure,  // out
    const NvU32 offset,
    const char *format
)
{

    NvU32 packedSize;
    NvU32 maxOffset;
    NvBool bSafe;

    // check for overflow in offset + packedSize
    packedSize = s_biosStructCalculatePackedSize(format);

    bSafe = portSafeAddU32(offset, packedSize, &maxOffset);
    if (NV_UNLIKELY(!bSafe || maxOffset > pVbiosImg->biosSize))
    {
        return NV_ERR_INVALID_OFFSET;
    }

    return s_biosUnpackStructure(pVbiosImg->pImage + offset, pStructure, format);
}

static NvU8 s_vbiosRead8(const KernelGspVbiosImg *pVbiosImg, NvU32 offset, NV_STATUS *pStatus)
{
    bios_U008 data;  // ReadStructure expects 'bios' types
    if (NV_UNLIKELY(*pStatus != NV_OK))
    {
        return 0;
    }
    *pStatus = s_vbiosReadStructure(pVbiosImg, &data, offset, "b");
    return (NvU8) data;
}

static NvU16 s_vbiosRead16(const KernelGspVbiosImg *pVbiosImg, NvU32 offset, NV_STATUS *pStatus)
{
    bios_U016 data;  // ReadStructure expects 'bios' types
    if (NV_UNLIKELY(*pStatus != NV_OK))
    {
        return 0;
    }
    *pStatus = s_vbiosReadStructure(pVbiosImg, &data, offset, "w");
    return (NvU16) data;
}

static NvU32 s_vbiosRead32(const KernelGspVbiosImg *pVbiosImg, NvU32 offset, NV_STATUS *pStatus)
{
    bios_U032 data;  // ReadStructure expects 'bios' types
    if (NV_UNLIKELY(*pStatus != NV_OK))
    {
        return 0;
    }
    *pStatus = s_vbiosReadStructure(pVbiosImg, &data, offset, "d");
    return (NvU32) data;
}

/*!
 * Find offset of BIT header (BIOS Information Table header) within VBIOS image.
 *
 * @param[in]   pVbiosImg   VBIOS image
 * @param[out]  pBitAddr    Offset of BIT header (if found)
 */
static NV_STATUS
s_vbiosFindBitHeader
(
    const KernelGspVbiosImg * const pVbiosImg,
    NvU32 *pBitAddr  // out
)
{

    NV_STATUS status = NV_OK;
    NvU32 addr;

    NV_ASSERT_OR_RETURN(pVbiosImg != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pVbiosImg->pImage != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pVbiosImg->biosSize > 0, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pBitAddr != NULL, NV_ERR_INVALID_ARGUMENT);

    for (addr = 0; addr < pVbiosImg->biosSize - 3; addr++)
    {
        if ((s_vbiosRead16(pVbiosImg, addr, &status) == BIT_HEADER_ID) &&
            (s_vbiosRead32(pVbiosImg, addr + 2, &status) == BIT_HEADER_SIGNATURE))
        {
            // found candidate BIT header
            NvU32 candidateBitAddr = addr;

            // verify BIT header checksum
            NvU32 headerSize = s_vbiosRead8(pVbiosImg,
                                            candidateBitAddr + BIT_HEADER_SIZE_OFFSET, &status);

            NvU32 checksum = 0;
            NvU32 j;

            for (j = 0; j < headerSize; j++)
            {
                checksum += (NvU32) s_vbiosRead8(pVbiosImg, candidateBitAddr + j, &status);
            }

            NV_ASSERT_OK_OR_RETURN(status);

            if ((checksum & 0xFF) == 0x0)
            {
                // found!
                // candidate BIT header passes checksum, lets use it
                *pBitAddr = candidateBitAddr;
                return status;
            }
        }

        NV_ASSERT_OK_OR_RETURN(status);
    }

    // not found
    return NV_ERR_GENERIC;
}

/*!
 * Find and parse a ucode desc (from BIT) for FWSEC from VBIOS image.
 *
 * @param[in]   pVbiosImg               VBIOS image
 * @param[in]   bitAddr                 Offset of BIT header within VBIOS image
 * @param[in]   bUseDebugFwsec          Whether to look for debug or prod FWSEC
 * @param[out]  pFwsecUcodeDescFromBit  Resulting ucode desc
 * @param[out]  pVbiosVersionCombined   (optional) output VBIOS version
 */
static NV_STATUS
s_vbiosParseFwsecUcodeDescFromBit
(
    const KernelGspVbiosImg * const pVbiosImg,
    const NvU32 bitAddr,
    const NvBool bUseDebugFwsec,
    FlcnUcodeDescFromBit *pFwsecUcodeDescFromBit,  // out
    NvU64 *pVbiosVersionCombined  // out
)
{

    NV_STATUS status;
    BIT_HEADER_V1_00 bitHeader;
    NvU32 tokIdx;
    const char *bitTokenSzFmt;

    NV_ASSERT_OR_RETURN(pVbiosImg != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pVbiosImg->pImage != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pVbiosImg->biosSize > 0, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pFwsecUcodeDescFromBit != NULL, NV_ERR_INVALID_ARGUMENT);

    // read BIT header
    status = s_vbiosReadStructure(pVbiosImg, &bitHeader, bitAddr, BIT_HEADER_V1_00_FMT);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to read BIT table structure: 0x%x\n", status);
        return status;
    }

    if (bitHeader.TokenSize >= BIT_TOKEN_V1_00_SIZE_8)
    {
        bitTokenSzFmt = BIT_TOKEN_V1_00_FMT_SIZE_8;
    }
    else if (bitHeader.TokenSize >= BIT_TOKEN_V1_00_SIZE_6)
    {
        bitTokenSzFmt = BIT_TOKEN_V1_00_FMT_SIZE_6;
    }
    else
    {
        NV_PRINTF(LEVEL_ERROR,
            "Invalid BIT token size: %u\n", bitHeader.TokenSize);
        DBG_BREAKPOINT();
        return NV_ERR_INVALID_STATE;
    }

    // loop through all BIT tokens
    for (tokIdx = 0; tokIdx < bitHeader.TokenEntries; tokIdx++)
    {
        BIT_TOKEN_V1_00 bitToken = {0};

        BIT_DATA_FALCON_DATA_V2 falconData;
        FALCON_UCODE_TABLE_HDR_V1 ucodeHeader;
        NvU32 entryIdx;

        // read BIT token
        status = s_vbiosReadStructure(pVbiosImg, &bitToken,
                                      bitAddr + bitHeader.HeaderSize +
                                        tokIdx * bitHeader.TokenSize,
                                      bitTokenSzFmt);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "failed to read BIT token %u, skipping: 0x%x\n",
                      tokIdx, status);
            continue;
        }

        // catch BIOSDATA token (for capturing VBIOS version)
        if (pVbiosVersionCombined != NULL &&
            bitToken.TokenId == BIT_TOKEN_BIOSDATA &&
            ((bitToken.DataVersion == BIT_DATA_BIOSDATA_VERSION_1) ||
             (bitToken.DataVersion == BIT_DATA_BIOSDATA_VERSION_2)) &&
            bitToken.DataSize > BIT_DATA_BIOSDATA_BINVER_SIZE_5)
        {
            BIT_DATA_BIOSDATA_BINVER binver;
            status = s_vbiosReadStructure(pVbiosImg, &binver,
                                          bitToken.DataPtr, BIT_DATA_BIOSDATA_BINVER_FMT);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "failed to read BIOSDATA (BIT token %u), skipping: 0x%x\n",
                          tokIdx, status);
                continue;
            }

            *pVbiosVersionCombined = (((NvU64) binver.Version) << 8) | ((NvU32) binver.OemVersion);
        }

        // skip tokens that are not for falcon ucode data v2
        if (bitToken.TokenId != BIT_TOKEN_FALCON_DATA ||
            bitToken.DataVersion != 2 ||
            bitToken.DataSize < BIT_DATA_FALCON_DATA_V2_SIZE_4)
        {
            continue;
        }

        // read falcon ucode data
        status = s_vbiosReadStructure(pVbiosImg, &falconData,
                                      bitToken.DataPtr, BIT_DATA_FALCON_DATA_V2_4_FMT);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "failed to read Falcon ucode data (BIT token %u), skipping: 0x%x\n",
                      tokIdx, status);
            continue;
        }

        // read falcon ucode header
        status = s_vbiosReadStructure(pVbiosImg, &ucodeHeader,
                                      pVbiosImg->expansionRomOffset +
                                        falconData.FalconUcodeTablePtr,
                                      FALCON_UCODE_TABLE_HDR_V1_6_FMT);
        if (status != NV_OK)
        {
            NV_PRINTF(LEVEL_ERROR,
                      "failed to read Falcon ucode header (BIT token %u), skipping: 0x%x\n",
                      tokIdx, status);
            continue;
        }

        // skip headers with undesired version
        if ((ucodeHeader.Version != FALCON_UCODE_TABLE_HDR_V1_VERSION) ||
            (ucodeHeader.HeaderSize < FALCON_UCODE_TABLE_HDR_V1_SIZE_6) ||
            (ucodeHeader.EntrySize < FALCON_UCODE_TABLE_ENTRY_V1_SIZE_6))
        {
            continue;
        }

        // loop through falcon ucode entries
        for (entryIdx = 0; entryIdx < ucodeHeader.EntryCount; entryIdx++)
        {
            FALCON_UCODE_TABLE_ENTRY_V1 ucodeEntry;
            FALCON_UCODE_DESC_HEADER ucodeDescHdr;

            NvU8 ucodeDescVersion;
            NvU32 ucodeDescSize;
            NvU32 ucodeDescOffset;
            const char *ucodeDescFmt;

            status = s_vbiosReadStructure(pVbiosImg, &ucodeEntry,
                                          pVbiosImg->expansionRomOffset +
                                            falconData.FalconUcodeTablePtr +
                                            ucodeHeader.HeaderSize +
                                            entryIdx * ucodeHeader.EntrySize,
                                          FALCON_UCODE_TABLE_ENTRY_V1_6_FMT);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "failed to read Falcon ucode entry %u (BIT token %u), skipping: 0x%x\n",
                          entryIdx, tokIdx, status);
                continue;
            }

            // skip entries that are not FWSEC
            if (ucodeEntry.ApplicationID != FALCON_UCODE_ENTRY_APPID_FIRMWARE_SEC_LIC &&
                ((bUseDebugFwsec && (ucodeEntry.ApplicationID != FALCON_UCODE_ENTRY_APPID_FWSEC_DBG)) ||
                 (!bUseDebugFwsec && (ucodeEntry.ApplicationID != FALCON_UCODE_ENTRY_APPID_FWSEC_PROD))))
            {
                continue;
            }

            // determine desc version, format, and size
            status = s_vbiosReadStructure(pVbiosImg, &ucodeDescHdr,
                                          pVbiosImg->expansionRomOffset + ucodeEntry.DescPtr,
                                          FALCON_UCODE_DESC_HEADER_FORMAT);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "failed to read Falcon ucode desc header for entry %u (BIT token %u), skipping: 0x%x\n",
                          entryIdx, tokIdx, status);
                continue;
            }

            // skip entries with desc version not V2 and V3 (FWSEC should be either V2 or V3)
            if (FLD_TEST_DRF(_BIT, _FALCON_UCODE_DESC_HEADER_VDESC_FLAGS, _VERSION, _UNAVAILABLE, ucodeDescHdr.vDesc))
            {
                NV_PRINTF(LEVEL_ERROR,
                          "unexpected ucode desc version missing for entry %u (BIT token %u), skipping\n",
                          entryIdx, tokIdx);
                continue;
            }
            else
            {
                ucodeDescVersion = (NvU8) DRF_VAL(_BIT, _FALCON_UCODE_DESC_HEADER_VDESC, _VERSION, ucodeDescHdr.vDesc);
                ucodeDescSize = DRF_VAL(_BIT, _FALCON_UCODE_DESC_HEADER_VDESC, _SIZE, ucodeDescHdr.vDesc);
            }

            if (ucodeDescVersion == NV_BIT_FALCON_UCODE_DESC_HEADER_VDESC_VERSION_V2 &&
                ucodeDescSize >= FALCON_UCODE_DESC_V2_SIZE_60)
            {
                ucodeDescFmt = FALCON_UCODE_DESC_V2_60_FMT;
            }
            else if (ucodeDescVersion == NV_BIT_FALCON_UCODE_DESC_HEADER_VDESC_VERSION_V3 &&
                     ucodeDescSize >= FALCON_UCODE_DESC_V3_SIZE_44)
            {
                ucodeDescFmt = FALCON_UCODE_DESC_V3_44_FMT;
            }
            else
            {
                NV_PRINTF(LEVEL_ERROR,
                          "unexpected ucode desc version 0x%x or size 0x%x for entry %u (BIT token %u), skipping\n",
                          ucodeDescVersion, ucodeDescSize, entryIdx, tokIdx);
                continue;
            }

            ucodeDescOffset = ucodeEntry.DescPtr + pVbiosImg->expansionRomOffset;

            status = s_vbiosReadStructure(pVbiosImg, &pFwsecUcodeDescFromBit->descUnion,
                                          ucodeDescOffset, ucodeDescFmt);
            if (status != NV_OK)
            {
                NV_PRINTF(LEVEL_ERROR,
                          "failed to read Falcon ucode desc (desc version 0x%x) for entry %u (BIT token %u), skipping: 0x%x\n",
                          ucodeDescVersion, entryIdx, tokIdx, status);
                continue;
            }

            pFwsecUcodeDescFromBit->descVersion = ucodeDescVersion;
            pFwsecUcodeDescFromBit->descOffset = ucodeDescOffset;
            pFwsecUcodeDescFromBit->descSize = ucodeDescSize;

            return NV_OK;
        }
    }

    // not found
    return NV_ERR_INVALID_DATA;
}

/*!
 * Fill a KernelGspFlcnUcode structure from a V2 ucode desc (from BIT).
 *
 * @param[in]   pGpu         OBJGPU pointer
 * @param[in]   pVbiosImg    VBIOS image
 * @param[in]   pDescV2      V2 ucode desc (from BIT)
 * @param[in]   descOffset   Offset of ucode desc (from BIT) in VBIOS image
 * @param[in]   descSize     Size of ucode desc (from BIT)
 * @param[out]  pFlcnUcode   KernelGspFlcnUcode structure to fill
 */
static NV_STATUS
s_vbiosFillFlcnUcodeFromDescV2
(
    OBJGPU *pGpu,  // for memdesc
    const KernelGspVbiosImg * const pVbiosImg,
    const FALCON_UCODE_DESC_V2 * const pDescV2,
    const NvU32 descOffset,
    const NvU32 descSize,
    KernelGspFlcnUcode *pFlcnUcode  // out
)
{
    NV_STATUS status;
    KernelGspFlcnUcodeBootWithLoader *pUcode = NULL;

    NvU8 *pMappedCodeMem = NULL;
    NvU8 *pMappedDataMem = NULL;

    NvBool bSafe;
    NvU32 codeSizeAligned;
    NvU32 dataSizeAligned;

    // offsets within pVbiosImg->pImage
    NvU32 imageCodeOffset;
    NvU32 imageCodeMaxOffset;
    NvU32 imageDataOffset;
    NvU32 imageDataMaxOffset;

    // offsets within mapped mem
    NvU32 mappedCodeMaxOffset;
    NvU32 mappedDataMaxOffset;

    NV_ASSERT(pVbiosImg != NULL);
    NV_ASSERT(pVbiosImg->pImage != NULL);
    NV_ASSERT(pDescV2 != NULL);
    NV_ASSERT(pFlcnUcode != NULL);

    pFlcnUcode->bootType = KGSP_FLCN_UCODE_BOOT_WITH_LOADER;
    pUcode = &pFlcnUcode->ucodeBootWithLoader;

    pUcode->pCodeMemDesc = NULL;
    pUcode->pDataMemDesc = NULL;

    pUcode->codeOffset = 0;
    pUcode->imemSize = pDescV2->IMEMLoadSize;
    pUcode->imemNsSize = pDescV2->IMEMLoadSize - pDescV2->IMEMSecSize;
    pUcode->imemNsPa = pDescV2->IMEMPhysBase;
    pUcode->imemSecSize = NV_ALIGN_UP(pDescV2->IMEMSecSize, 256);
    pUcode->imemSecPa = pDescV2->IMEMSecBase - pDescV2->IMEMVirtBase + pDescV2->IMEMPhysBase;
    pUcode->codeEntry = pDescV2->VirtualEntry;  // 0?

    pUcode->dataOffset = pDescV2->DMEMOffset;
    pUcode->dmemSize = pDescV2->DMEMLoadSize;
    pUcode->dmemPa = pDescV2->DMEMPhysBase;

    pUcode->interfaceOffset = pDescV2->InterfaceOffset;

    codeSizeAligned = NV_ALIGN_UP(pUcode->imemSize, 256);
    dataSizeAligned = NV_ALIGN_UP(pUcode->dmemSize, 256);

    // verify offsets within pVbiosImg->pImage
    bSafe = portSafeAddU32(descOffset, descSize, &imageCodeOffset);
    if (!bSafe || imageCodeOffset >= pVbiosImg->biosSize)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    bSafe = portSafeAddU32(imageCodeOffset, pUcode->imemSize, &imageCodeMaxOffset);
    if (!bSafe || imageCodeMaxOffset > pVbiosImg->biosSize)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    bSafe = portSafeAddU32(imageCodeOffset, pUcode->dataOffset, &imageDataOffset);
    if (!bSafe || imageDataOffset >= pVbiosImg->biosSize)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    bSafe = portSafeAddU32(imageDataOffset, pUcode->dmemSize, &imageDataMaxOffset);
    if (!bSafe || imageDataMaxOffset > pVbiosImg->biosSize)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    // verify offsets within mapped mem
    if (pUcode->imemNsPa >= codeSizeAligned)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    bSafe = portSafeAddU32(pUcode->imemNsPa, pUcode->imemSize, &mappedCodeMaxOffset);
    if (!bSafe || mappedCodeMaxOffset > codeSizeAligned)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    if (pUcode->dmemPa >= dataSizeAligned)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    bSafe = portSafeAddU32(pUcode->dmemPa, pUcode->dmemSize, &mappedDataMaxOffset);
    if (!bSafe || mappedDataMaxOffset > dataSizeAligned)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    NV_ASSERT_OK_OR_RETURN(
        memdescCreate(&pUcode->pCodeMemDesc, pGpu, codeSizeAligned,
                      256, NV_TRUE, ADDR_SYSMEM, NV_MEMORY_UNCACHED, MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS));

    NV_ASSERT_OK_OR_RETURN(
        memdescCreate(&pUcode->pDataMemDesc, pGpu, dataSizeAligned,
                      256, NV_TRUE, ADDR_SYSMEM, NV_MEMORY_UNCACHED, MEMDESC_FLAGS_PHYSICALLY_CONTIGUOUS));

    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_9,
                    pUcode->pCodeMemDesc);
    if (status != NV_OK)
    {
        return status;
    }

    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_10,
                    pUcode->pDataMemDesc);
    if (status != NV_OK)
    {
        return status;
    }

    pMappedCodeMem = memdescMapInternal(pGpu, pUcode->pCodeMemDesc, TRANSFER_FLAGS_NONE);
    if (pMappedCodeMem == NULL)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    pMappedDataMem = memdescMapInternal(pGpu, pUcode->pDataMemDesc, TRANSFER_FLAGS_NONE);
    if (pMappedDataMem == NULL)
    {
        memdescUnmapInternal(pGpu, pUcode->pCodeMemDesc,
                             TRANSFER_FLAGS_DESTROY_MAPPING);
        pMappedCodeMem = NULL;
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    portMemSet(pMappedCodeMem, 0, codeSizeAligned);
    portMemCopy(pMappedCodeMem + pUcode->imemNsPa, pUcode->imemSize,
                pVbiosImg->pImage + imageCodeOffset, pUcode->imemSize);

    portMemSet(pMappedDataMem, 0, dataSizeAligned);
    portMemCopy(pMappedDataMem + pUcode->dmemPa, pUcode->dmemSize,
                pVbiosImg->pImage + imageDataOffset, pUcode->dmemSize);

    memdescUnmapInternal(pGpu, pUcode->pCodeMemDesc,
                         TRANSFER_FLAGS_DESTROY_MAPPING);
    pMappedCodeMem = NULL;
    memdescUnmapInternal(pGpu, pUcode->pDataMemDesc,
                         TRANSFER_FLAGS_DESTROY_MAPPING);
    pMappedDataMem = NULL;

    return status;
}

/*!
 * Fill a KernelGspFlcnUcode structure from a V3 ucode desc (from BIT).
 *
 * @param[in]   pGpu         OBJGPU pointer
 * @param[in]   pVbiosImg    VBIOS image
 * @param[in]   pDescV3      V3 ucode desc (from BIT)
 * @param[in]   descOffset   Offset of ucode desc (from BIT) in VBIOS image
 * @param[in]   descSize     Size of ucode desc (from BIT)
 * @param[out]  pFlcnUcode   KernelGspFlcnUcode structure to fill
 */
static NV_STATUS
s_vbiosFillFlcnUcodeFromDescV3
(
    OBJGPU *pGpu,  // for memdesc
    const KernelGspVbiosImg * const pVbiosImg,
    const FALCON_UCODE_DESC_V3 * const pDescV3,
    const NvU32 descOffset,
    const NvU32 descSize,
    KernelGspFlcnUcode *pFlcnUcode  // out
)
{
    NV_STATUS status;
    KernelGspFlcnUcodeBootFromHs *pUcode = NULL;

    NvU8 *pMappedUcodeMem = NULL;
    NvBool bSafe;

    // offsets within pVbiosImg->pImage
    NvU32 imageOffset;
    NvU32 imageMaxOffset;

    // offsets with ucode image
    NvU32 dataMaxOffset;
    NvU32 sigDataOffset;
    NvU32 sigDataMaxOffset;

    NV_ASSERT(pVbiosImg != NULL);
    NV_ASSERT(pVbiosImg->pImage != NULL);
    NV_ASSERT(pDescV3 != NULL);
    NV_ASSERT(pFlcnUcode != NULL);

    pFlcnUcode->bootType = KGSP_FLCN_UCODE_BOOT_FROM_HS;
    pUcode = &pFlcnUcode->ucodeBootFromHs;

    pUcode->pUcodeMemDesc = NULL;
    pUcode->size = RM_ALIGN_UP(pDescV3->StoredSize, 256);

    pUcode->codeOffset = 0;
    pUcode->imemSize = pDescV3->IMEMLoadSize;
    pUcode->imemPa = pDescV3->IMEMPhysBase;
    pUcode->imemVa = pDescV3->IMEMVirtBase;

    pUcode->dataOffset = pUcode->imemSize;
    pUcode->dmemSize = pDescV3->DMEMLoadSize;
    pUcode->dmemPa = pDescV3->DMEMPhysBase;
    pUcode->dmemVa = FLCN_DMEM_VA_INVALID;

    pUcode->hsSigDmemAddr = pDescV3->PKCDataOffset;
    pUcode->ucodeId = pDescV3->UcodeId;
    pUcode->engineIdMask = pDescV3->EngineIdMask;

    pUcode->pSignatures = NULL;
    pUcode->signaturesTotalSize = 0;
    pUcode->sigSize = BCRT30_RSA3K_SIG_SIZE;
    pUcode->sigCount = pDescV3->SignatureCount;

    pUcode->vbiosSigVersions = pDescV3->SignatureVersions;
    pUcode->interfaceOffset = pDescV3->InterfaceOffset;

    // compute imageOffset and sanity check size
    bSafe = portSafeAddU32(descOffset, descSize, &imageOffset);
    if (!bSafe || imageOffset >= pVbiosImg->biosSize)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    bSafe = portSafeAddU32(imageOffset, pUcode->size, &imageMaxOffset);
    if (!bSafe || imageMaxOffset > pVbiosImg->biosSize)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    // sanity check imemSize
    if (pUcode->imemSize > pUcode->size)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    // sanity check dataOffset and dataSize
    if (pUcode->dataOffset >= pUcode->size)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    bSafe = portSafeAddU32(pUcode->dataOffset, pUcode->dmemSize, &dataMaxOffset);
    if (!bSafe || dataMaxOffset > pUcode->size)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    // sanity check hsSigDmemAddr
    bSafe = portSafeAddU32(pUcode->dataOffset, pUcode->hsSigDmemAddr, &sigDataOffset);
    if (!bSafe || sigDataOffset >= pUcode->size)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    bSafe = portSafeAddU32(sigDataOffset, pUcode->sigSize, &sigDataMaxOffset);
    if (!bSafe || sigDataMaxOffset > pUcode->size)
    {
        return NV_ERR_INVALID_OFFSET;
    }

    // compute signaturesTotalSize and populate pSignatures
    if (descSize < FALCON_UCODE_DESC_V3_SIZE_44)
    {
        return NV_ERR_INVALID_STATE;
    }
    pUcode->signaturesTotalSize = descSize - FALCON_UCODE_DESC_V3_SIZE_44;

    pUcode->pSignatures = portMemAllocNonPaged(pUcode->signaturesTotalSize);
    if (pUcode->pSignatures == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }

    portMemCopy(pUcode->pSignatures, pUcode->signaturesTotalSize,
                pVbiosImg->pImage + descOffset + FALCON_UCODE_DESC_V3_SIZE_44, pUcode->signaturesTotalSize);

    NV_ASSERT_OK_OR_RETURN(
        memdescCreate(&pUcode->pUcodeMemDesc, pGpu, pUcode->size,
                      256, NV_TRUE, ADDR_SYSMEM, NV_MEMORY_UNCACHED, MEMDESC_FLAGS_NONE));

    memdescTagAlloc(status, NV_FB_ALLOC_RM_INTERNAL_OWNER_UNNAMED_TAG_11,
                    pUcode->pUcodeMemDesc);
    if (status != NV_OK)
    {
        return status;
    }

    pMappedUcodeMem = memdescMapInternal(pGpu, pUcode->pUcodeMemDesc, TRANSFER_FLAGS_NONE);
    if (pMappedUcodeMem == NULL)
    {
        return NV_ERR_INSUFFICIENT_RESOURCES;
    }

    portMemCopy(pMappedUcodeMem, pUcode->size,
                pVbiosImg->pImage + imageOffset, pUcode->size);

    memdescUnmapInternal(pGpu, pUcode->pUcodeMemDesc,
                         TRANSFER_FLAGS_DESTROY_MAPPING);
    pMappedUcodeMem = NULL;

    return status;
}

/*!
 * Create a new KernelGspFlcnUcode structure from a ucode desc (from BIT).
 *
 * The resulting KernelGspFlcnUcode should be freed with kgspFreeFlcnUcode
 * after use.
 *
 * @param[in]   pGpu                     OBJGPU pointer
 * @param[in]   pVbiosImg                VBIOS image
 * @param[in]   pFlcnUcodeDescFromBit    V2 ucode desc (from BIT)
 * @param[out]  ppFlcnUcode              Pointer to resulting KernelGspFlcnUcode
 */
static NV_STATUS
s_vbiosNewFlcnUcodeFromDesc
(
    OBJGPU *pGpu,  // for memdesc
    const KernelGspVbiosImg * const pVbiosImg,
    const FlcnUcodeDescFromBit * const pFlcnUcodeDescFromBit,
    KernelGspFlcnUcode **ppFlcnUcode  // out
)
{
    NV_STATUS status;
    KernelGspFlcnUcode *pFlcnUcode = NULL;

    NV_ASSERT(pGpu != NULL);
    NV_ASSERT(pVbiosImg != NULL);
    NV_ASSERT(pFlcnUcodeDescFromBit != NULL);
    NV_ASSERT(ppFlcnUcode != NULL);

    pFlcnUcode = portMemAllocNonPaged(sizeof(*pFlcnUcode));
    if (pFlcnUcode == NULL)
    {
        return NV_ERR_NO_MEMORY;
    }
    portMemSet(pFlcnUcode, 0, sizeof(*pFlcnUcode));

    if (pFlcnUcodeDescFromBit->descVersion == NV_BIT_FALCON_UCODE_DESC_HEADER_VDESC_VERSION_V2)
    {
        status = s_vbiosFillFlcnUcodeFromDescV2(pGpu, pVbiosImg,
                                                &pFlcnUcodeDescFromBit->descUnion.v2,
                                                pFlcnUcodeDescFromBit->descOffset,
                                                pFlcnUcodeDescFromBit->descSize,
                                                pFlcnUcode);
    }
    else if (pFlcnUcodeDescFromBit->descVersion == NV_BIT_FALCON_UCODE_DESC_HEADER_VDESC_VERSION_V3)
    {
        status = s_vbiosFillFlcnUcodeFromDescV3(pGpu, pVbiosImg,
                                                &pFlcnUcodeDescFromBit->descUnion.v3,
                                                pFlcnUcodeDescFromBit->descOffset,
                                                pFlcnUcodeDescFromBit->descSize,
                                                pFlcnUcode);
    }
    else
    {
        NV_ASSERT(0);
        return NV_ERR_INVALID_STATE;
    }

    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "failed to parse/prepare Falcon ucode (desc: version 0x%x, offset 0x%x, size 0x%x): 0x%x\n",
                  pFlcnUcodeDescFromBit->descVersion,
                  pFlcnUcodeDescFromBit->descOffset,
                  pFlcnUcodeDescFromBit->descSize,
                  status);

        kgspFreeFlcnUcode(pFlcnUcode);
        pFlcnUcode = NULL;
    }

    *ppFlcnUcode = pFlcnUcode;
    return NV_OK;
}

/*!
 * Parse FWSEC ucode from VBIOS image.
 *
 * The resulting KernelGspFlcnUcode should be freed with kgspFlcnUcodeFree
 * after use.
 *
 * @param[in]   pGpu                    OBJGPU pointer
 * @param[in]   pKernelGsp              KernelGsp pointer
 * @param[in]   pVbiosImg               VBIOS image
 * @param[out]  ppFwsecUcode            Pointer to resulting KernelGspFlcnUcode
 * @param[out]  pVbiosVersionCombined   (optional) pointer to output VBIOS version
 */
NV_STATUS
kgspParseFwsecUcodeFromVbiosImg_IMPL
(
    OBJGPU *pGpu,
    KernelGsp *pKernelGsp,
    const KernelGspVbiosImg * const pVbiosImg,
    KernelGspFlcnUcode **ppFwsecUcode,  // out
    NvU64 *pVbiosVersionCombined  // out
)
{
    NV_STATUS status;

    FlcnUcodeDescFromBit fwsecUcodeDescFromBit;
    NvU32 bitAddr;
    NvBool bUseDebugFwsec = NV_FALSE;

    NV_ASSERT_OR_RETURN(!IS_VIRTUAL(pGpu), NV_ERR_NOT_SUPPORTED);
    NV_ASSERT_OR_RETURN(IS_GSP_CLIENT(pGpu), NV_ERR_NOT_SUPPORTED);

    NV_ASSERT_OR_RETURN(pVbiosImg != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(pVbiosImg->pImage != NULL, NV_ERR_INVALID_ARGUMENT);
    NV_ASSERT_OR_RETURN(ppFwsecUcode, NV_ERR_INVALID_ARGUMENT);

    status = s_vbiosFindBitHeader(pVbiosImg, &bitAddr);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to find BIT header in VBIOS image: 0x%x\n", status);
        return status;
    }

    bUseDebugFwsec = kgspIsDebugModeEnabled_HAL(pGpu, pKernelGsp);
    status = s_vbiosParseFwsecUcodeDescFromBit(pVbiosImg, bitAddr, bUseDebugFwsec,
                                               &fwsecUcodeDescFromBit, pVbiosVersionCombined);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR, "failed to parse FWSEC ucode desc from VBIOS image: 0x%x\n", status);
        return status;
    }

    status = s_vbiosNewFlcnUcodeFromDesc(pGpu, pVbiosImg, &fwsecUcodeDescFromBit, ppFwsecUcode);
    if (status != NV_OK)
    {
        NV_PRINTF(LEVEL_ERROR,
                  "failed to prepare new flcn ucode for FWSEC: 0x%x\n",
                  status);
        return status;
    }

    return status;
}
