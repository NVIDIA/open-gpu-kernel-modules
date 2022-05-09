/*
 * SPDX-FileCopyrightText: Copyright (c) 2010-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************* DisplayPort *******************************\
*                                                                           *
* Module: dp_edid.c                                                         *
*    Implementation of SST/MST EDID reader                                  *
*                                                                           *
\***************************************************************************/

#include "dp_buffer.h"
#include "dp_internal.h"
#include "dp_edid.h"

using namespace DisplayPort;

EdidAssembler::EdidAssembler(Edid * const edid, bool bPatchCrc):
    edid(edid), stream(edid->getBuffer()), oldBlockChecksum(0x00),
    blocksRead(0), totalBlockCnt(0), retriesCount(0),
    bPatchCrc(bPatchCrc) {}


bool EdidAssembler::readIsComplete()
{
    return (blocksRead > 0 && blocksRead == totalBlockCnt);
}

void EdidAssembler::reset()
{
    oldBlockChecksum = 0x00;
    blocksRead = 0;
    totalBlockCnt = 0;
    retriesCount = 0;
    stream.seek(0);
}

void EdidAssembler::postReply(const Buffer & buffer, unsigned sizeCompleted, bool success)
{
    if (!success || buffer.isError())
    {
        retriesCount++;
        return;
    }

    //
    // For SST:
    // Check the Checksum Error Per Block reading, mark the EDID as "patched" if
    // CRC is wrong. DPLib will return fallback EDID.
    //
    blocksRead++;
    stream.write(buffer.data, sizeCompleted);
    if (getEDIDBlockChecksum(buffer))
    {
        if (bPatchCrc)
            edid->patchCrc();
        edid->setPatchedChecksum(true);
    }
    return;
}

void EdidAssembler::postReply(unsigned char * data, unsigned sizeCompleted, bool success)
{
    //
    // For MST: When read of edid block failed, library will attempt to read
    // same block again, but not more than EDID_POLICY_BLOCK_READ_MAX_RETRY_COUNT times
    //
    if (!success)
    {
        retriesCount++;
        return;
    }

    //
    // Check the Checksum Error Per Block reading,
    // library will attempt to read same block again,
    // but not more than EDID_POLICY_BLOCK_READ_MAX_RETRY_COUNT times.
    //
    Buffer buffer(data, EDID_BLOCK_SIZE);
    if (buffer.isError())
    {
        retriesCount++;
        return;
    }

    NvU8 newBlockChecksum = getEDIDBlockChecksum(buffer);
    if (newBlockChecksum)
    {
        if (this->oldBlockChecksum != newBlockChecksum) //First failure?
        {
            this->oldBlockChecksum = newBlockChecksum;
            retriesCount++;
            return;
        }
    }

    this->oldBlockChecksum = 0;
    retriesCount = 0;
    blocksRead++;
    stream.write(data, sizeCompleted);
}

bool EdidAssembler::readNextRequest(NvU8 & seg, NvU8 & offset)
{
    //
    // cache totalBlockCnt,
    // In EDID 1.3 HF-EEODB, it might changes after 1 extension block read.
    //
    if ((blocksRead == 1) || (blocksRead == 2))
        totalBlockCnt = edid->getBlockCount();

    //
    // will return false in two scenarios
    //  1. EDID read is complete, all extension blocks were read
    //  2. First EDID block was corrupted, then totalBlockCnt = 0
    //
    if (blocksRead >= totalBlockCnt)
        return false;

    // Retry count exceeded for particular block?
    if (retriesCount > EDID_POLICY_BLOCK_READ_MAX_RETRY_COUNT)
        return false;

    seg     = NvU8(blocksRead >> 1);
    offset  = NvU8((blocksRead & 0x1) * EDID_BLOCK_SIZE);
    return true;
}


enum
{
    EDID_V1_IDX_EXTENSION       = 0x7E,
    EDID_V1_IDX_HEADER0         = 0x00,
    EDID_V1_HEADER0             = 0x00,

    EDID_V1_IDX_HEADER1         = 0x01,
    EDID_V1_HEADER1             = 0xFF,

    EDID_V1_IDX_VERSION         = 0x12,
    EDID_V1_VERSION_1           = 0x01,
    EDID_V2_IDX_VERREV          = 0x00,

    //
    // from od_edid.h RM to identify VER 2, use 7:4 bits.
    // #define EDID_V2_VERREV_VERSION        7:4   /* RW--F */
    // #define EDID_V2_VERREV_VERSION_2     0x02   /* RWI-V */
    //
    // Avoiding FLD_* macros, thus shift VER2 value 4 bits to left
    //
    EDID_V2_VERREV_VERSION_2        = 0x02 << 4,
    EDID_FLAGS_CHKSUM_ATTEMPTS_DP   = 0x5,
};

enum
{
    // EDID CTA-EXT (CTA 861 Extension) block defines
    EDID_CTA_EXT_HEADER_OFFSET                 = 0x00,
    EDID_CTA_EXT_HEADER                        = 0x02,
    EDID_CTA_EXT_VERSION_OFFSET                = 0x01,
    EDID_CTA_EXT_VERSION_3                     = 0x03,
    EDID_CTA_EXT_DATA_BLOCK_HEADER_OFFSET      = 0x04,
    EDID_CTA_EXT_DATA_BLOCK_HEADER_HF_EEODB    = 0xE2,
    EDID_CTA_EXT_DATA_BLOCK_TAG_OFFSET         = 0x05,
    EDID_CTA_EXT_DATA_BLOCK_TAG_HF_EEODB       = 0x78,
    EDID_CTA_EXT_DATA_BLOCK_EXT_COUNT_OFFSET   = 0x06,
};

Edid::Edid(): buffer()
{
    // fill EDID buffer with zeroes
    this->buffer.memZero();
    checkSumValid = false;
    forcedCheckSum = false;
    fallbackEdid = false;
    patchedChecksum = false;

    // clear the WARFlags
    _WARFlags temp = {0};
    WARFlags = temp;
}

Edid::~Edid()
{
}

bool Edid::verifyCRC()
{
    if (getEdidSize() > 0)
    {
        this->validateCheckSum();
        return this->checkSumValid;
    }
    else
        return false;
}

// this routine patches the edid crc after it has been overridden for WARs.
void Edid::patchCrc()
{
    // we always override some bytes within the first 128
    // recalculate and fix the checksum for the first page only.
    unsigned chksum = 0;
    for (unsigned i = 0; i < 128; i++)
    {
        chksum += buffer.data[i];
    }
    chksum = chksum & 0xFF;

    if (chksum)
        buffer.data[127] = 0xFF & (buffer.data[127] + (0x100 - chksum));
}

bool Edid::isChecksumValid() const
{
    // return checksum valid if it is.
    // else return checksum is valid if checksum wasn't valid but we will assume it to be.
    return (checkSumValid || forcedCheckSum);
}

bool Edid::isFallbackEdid() const
{
    return fallbackEdid;
}

NvU8 Edid::getFirstPageChecksum()
{
    DP_ASSERT(buffer.getLength() >= 128);
    if (buffer.getLength() < 128)
        return 0;
    else
        return buffer.data[127];
}

NvU8 Edid::getLastPageChecksum()
{
    NvU32   bufferSize = buffer.getLength();
    NvU32   checksumLocation = this->getBlockCount() * 128 - 1;

    if (bufferSize == 0 || bufferSize < (this->getBlockCount() * 128))
    {
        DP_LOG(("DP-EDID> Edid length is 0 or less than required"));
        return 0;
    }

    if (bufferSize % 128 != 0)
    {
        DP_LOG(("DP-EDID> Edid length is not a multiple of 128"));
        return 0;
    }

    return buffer.data[checksumLocation];

}

void Edid::validateCheckSum()
{
    // Each page has its own checksum
    checkSumValid = false;
    for (unsigned chunk = 0; chunk < this->buffer.length; chunk += 128)
    {
        unsigned chksum = 0;
        for (unsigned i = 0; i < 128; i++)
        {
            chksum += buffer.data[i+chunk];
        }

        if ((chksum & 0xFF) != 0)
            return;
    }
    checkSumValid = true;
}

unsigned Edid::getEdidVersion()
{
    if (buffer.isError() || buffer.length < EDID_BLOCK_SIZE)
    {
        return 0;
    }

    // 0 version is "unknown"
    unsigned version = 0;

    // Check for Version 1 EDID
    if (this->buffer.data[EDID_V1_IDX_VERSION] == EDID_V1_VERSION_1)
    {
        version = 1;
    }
    // Check for version 2 EDID
    else if (this->buffer.data[EDID_V2_IDX_VERREV] & EDID_V2_VERREV_VERSION_2)
    {
        //
        // Version 2 has 256 bytes by default.
        // There is a note about an extra 256 byte block if byte 0x7E
        // bit 7 is set but there's no definition for it listed in
        // the EDID Version 3 (971113). So, let's just skip it for now.
        //
        version = 2;
    }
    else
    {
        DP_ASSERT(version && "Unknown EDID version");
    }

    return version;
}

const char * Edid::getName() const
{
    static char decodedName[16] = {0};
    int tail = 0;
    if (buffer.length < 128)
        return "?";

    for (int i = 0; i < 4; i++)
        if (buffer.data[0x39 + i * 18 + 0] == 0xFC)
        {
            for (int j = 0; j < 13; j++)
                decodedName[tail++] = buffer.data[0x39 + i*18 + 2 + j];
            break;
        }
    decodedName[tail++] = 0;
    return decodedName;
}

unsigned Edid::getBlockCount()
{
    if (buffer.isError() || buffer.length < EDID_BLOCK_SIZE)
    {
        return 0;
    }

    unsigned version = getEdidVersion();

    if (version == 1)
    {
        NvU32 blockCount = (unsigned) this->buffer.data[EDID_V1_IDX_EXTENSION]+1;

        if (blockCount > EDID_MAX_BLOCK_COUNT)
        {
            DP_LOG(("DPEDID> %s: DDC read returned questionable results: "
                   "Total block Count too high: %d",
                   __FUNCTION__, blockCount));
            return 1;
        }
        //
        // Check for the HF-EEODB defined in HDMI 2.1 specification.
        // 1. It is EDID version 1.3 and the extension block count is 1 (total block count = 2)
        // 2. The 1st EDID extension block is already read. (buffer.length > block size)
        // 3. The 1st EDID extension block is CTA extension block.
        // 4. It has HF-EEODB (1st extension block: byte4 == 0xE2 and byte5 == 0x78)
        //
        if ((blockCount == 2) && (buffer.length >= EDID_BLOCK_SIZE * 2))
        {
            NvU8 *pExt = &(this->buffer.data[EDID_BLOCK_SIZE]);

            //
            // If it's a CTA-EXT block version 3 and has HF-EEODB
            // defined, update the total block count.
            //
            if ((pExt[EDID_CTA_EXT_HEADER_OFFSET] == EDID_CTA_EXT_HEADER) &&
                (pExt[EDID_CTA_EXT_VERSION_OFFSET] == EDID_CTA_EXT_VERSION_3) &&
                (pExt[EDID_CTA_EXT_DATA_BLOCK_HEADER_OFFSET] == EDID_CTA_EXT_DATA_BLOCK_HEADER_HF_EEODB) &&
                (pExt[EDID_CTA_EXT_DATA_BLOCK_TAG_OFFSET] == EDID_CTA_EXT_DATA_BLOCK_TAG_HF_EEODB))
            {
                blockCount = pExt[EDID_CTA_EXT_DATA_BLOCK_EXT_COUNT_OFFSET] + 1;
            }

        }
        return blockCount;
    }
    else if (version == 2)
    {
        //
        // Version 2 has 256 bytes by default.
        // There is a note about an extra 256 byte block
        // if byte 0x7E bit 7 is set, but there's no
        // definition for it listed in the
        // EDID Version 3 (971113) So, let's just skip
        // it for now.
        //
        return 2;
    }
    else
    {
        // Unknown EDID version. Skip it.
        DP_LOG(("DPEDID> %s: Unknown EDID Version!",__FUNCTION__));
        DP_ASSERT(0 && "Unknown EDID version!");
        return 1;
    }
}

unsigned Edid::getEdidSize() const
{
    return this->buffer.length;
}

void DisplayPort::Edid::swap(Edid & right)
{
    swapBuffers(buffer, right.buffer);
    validateCheckSum();
}

const NvU8 fallbackEdidModes[5][EDID_BLOCK_SIZE] = {
    // ID Manufacturer Name: NVD
    // VIDEO INPUT DEFINITION:
    //    Digital Signal
    //    VESA DFP 1.x Compatible

    //
    // The first 4 entries are for NV_DPCD_SINK_VIDEO_FALLBACK_FORMATS (DPCD 0x20)
    // 1024x768x60Hz:  defined in bit 0.
    // 1280x720x60Hz:  defined in bit 1.
    // 1920x1080x60Hz: defined in bit 2. [Mandatory]
    //
    {
        // Bit 2: 1920x1080x60 only
        0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
        0x3A, 0xC4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x01, 0x04, 0xA5, 0x00, 0x00, 0x64,
        0xEE, 0x91, 0xA3, 0x54, 0x4C, 0x99, 0x26, 0x0F,
        0x50, 0x54, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A,
        0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
        0x43, 0x00, 0xC0, 0x1C, 0x32, 0x00, 0x00, 0x1C,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDB
    },
    {
        // bit 2 + bit 0: 1920x1080x60 + 1024x768x60
        0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
        0x3A, 0xC4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x01, 0x04, 0xA5, 0x00, 0x00, 0x64,
        0xEE, 0x91, 0xA3, 0x54, 0x4C, 0x99, 0x26, 0x0F,
        0x50, 0x54, 0x00, 0x00, 0x08, 0x00, 0x01, 0x01,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A,
        0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
        0x43, 0x00, 0xC0, 0x1C, 0x32, 0x00, 0x00, 0x1C,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD3
    },
    {
        // bit 2 + bit 1: 1920x1080x60 + 1280x720x60
        0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
        0x3A, 0xC4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x01, 0x04, 0xA5, 0x00, 0x00, 0x64,
        0xEE, 0x91, 0xA3, 0x54, 0x4C, 0x99, 0x26, 0x0F,
        0x50, 0x54, 0x00, 0x00, 0x00, 0x00, 0x81, 0xC0,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A,
        0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
        0x43, 0x00, 0xC0, 0x1C, 0x32, 0x00, 0x00, 0x1C,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9C
    },
    {
        // bit2 + bit 1 + bit 0: All 3 modes.
        0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
        0x3A, 0xC4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x01, 0x04, 0xA5, 0x00, 0x00, 0x64,
        0xEE, 0x91, 0xA3, 0x54, 0x4C, 0x99, 0x26, 0x0F,
        0x50, 0x54, 0x00, 0x00, 0x08, 0x00, 0x81, 0xC0,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A,
        0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
        0x43, 0x00, 0xC0, 0x1C, 0x32, 0x00, 0x00, 0x1C,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x94
    },
    {
        // ESTABLISHED TIMING I:
        //    640 X 480 @ 60Hz (IBM,VGA)
        0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00,
        0x3A, 0xC4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x01, 0x04, 0x95, 0x00, 0x00, 0x78,
        0xEE, 0x91, 0xA3, 0x54, 0x4C, 0x99, 0x26, 0x0F,
        0x50, 0x54, 0x00, 0x20, 0x00, 0x00, 0x01, 0x01,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x92
    }
};

//
// Definition of DPCD 0x20:
// 1024x768x60Hz:  defined in bit 0.
// 1280x720x60Hz:  defined in bit 1.
// 1920x1080x60Hz: defined in bit 2. [Mandatory]
// MIN value is 4 (only 1920x1080 supported)
// MAX value is 7 (supports all 3 modes)
//
#define SINK_VIDEO_FALLBACK_FORMATS_MIN_VALUE            (0x00000004)
#define SINK_VIDEO_FALLBACK_FORMATS_MAX_VALUE            (0x00000007)

void DisplayPort::makeEdidFallback(Edid & edid, NvU32 fallbackFormatSupported)
{
    const NvU8 *data;

    // fallbackFormatSupported valid values = 4~7
    if (fallbackFormatSupported > SINK_VIDEO_FALLBACK_FORMATS_MAX_VALUE ||
        fallbackFormatSupported < SINK_VIDEO_FALLBACK_FORMATS_MIN_VALUE)
    {
        // 4 is default fallback mode. (only 640x480)
        data = fallbackEdidModes[4];
    }
    else
    {
        data = fallbackEdidModes[fallbackFormatSupported-4];
    }
    if (!edid.getBuffer()->resize(EDID_BLOCK_SIZE))
        return;

    dpMemCopy(edid.getBuffer()->getData(), (const NvU8*)data, EDID_BLOCK_SIZE);
    DP_ASSERT(edid.verifyCRC());
    edid.setFallbackFlag(true);
}

/*
Fake EDID for DP2VGA dongle when the EDID of the real monitor is not available

Established Timings [20 CE 00]
    640 x 480 @ 60Hz
    800 x 600 @ 72Hz
    800 x 600 @ 75Hz
    1024 x 768 @ 60Hz
    1024 x 768 @ 70Hz
    1024 x 768 @ 75Hz

Standard Timings
    Timing [3159]                       :  640 x  480 @  85Hz (4:3)
    Timing [4559]                       :  800 x  600 @  85Hz (4:3)
    Timing [6159]                       : 1024 x  768 @  85Hz (4:3)
    Timing [714F]                       : 1152 x  864 @  75Hz (4:3)

Detailed Timing [DTD] 1280 x 1024 @ 60.02Hz
    Pixel Clock                         : 108.00Mhz
    HBlank, HBorder                     : 408, 0
    HSyncStart, HSyncWidth              : 48, 112
    VBlank, VBorder                     : 42, 0
    VSyncStart, VSyncWidth              : 1, 3
    Image size                          : 376mm x 301mm
    DigitalSeparate +/+
*/

void DisplayPort::makeEdidFallbackVGA(Edid & edid)
{
     const NvU8 data[] = {
          0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x3A, 0xC4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
          0x01, 0x13, 0x01, 0x03, 0x80, 0x26, 0x1E, 0x78, 0xEE, 0xCB, 0x05, 0xA3, 0x58, 0x4C, 0x9B, 0x25,
          0x13, 0x50, 0x54, 0x20, 0xCE, 0x00, 0x31, 0x59, 0x45, 0x59, 0x61, 0x59, 0x71, 0x4F, 0x81, 0x40,
          0x81, 0x80, 0x01, 0x01, 0x01, 0x01, 0x30, 0x2A, 0x00, 0x98, 0x51, 0x00, 0x2A, 0x40, 0x30, 0x70,
          0x13, 0x00, 0x78, 0x2D, 0x11, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x30, 0x55, 0x1F,
          0x52, 0x0E, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x4C,
          0x43, 0x44, 0x5F, 0x56, 0x47, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD8
     };

     if (!edid.getBuffer()->resize(sizeof(data)))
          return;

     dpMemCopy(edid.getBuffer()->getData(), (const NvU8*)data, sizeof data);
     DP_ASSERT(edid.verifyCRC());
     edid.setFallbackFlag(true);
}

NvU8 DisplayPort::getEDIDBlockChecksum(const Buffer & buffer)
{
    DP_ASSERT(buffer.getLength() == 128);

    unsigned chksum = 0;
    for (unsigned i = 0; i < buffer.getLength(); i++)
    {
        chksum += buffer.data[i];
    }
    chksum = chksum & 0xFF;
    return (NvU8)chksum;
}
