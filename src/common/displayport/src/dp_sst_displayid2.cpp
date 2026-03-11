/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
* Module: dp_sst_displayid2.c                                               *
*    Implementation Single Stream DisplayID2x reads                         *
*                                                                           *
\***************************************************************************/

#include "dp_internal.h"
#include "dp_displayid2.h"
#include "dp_printf.h"

using namespace DisplayPort;

/*!
 * @brief Read a 128-byte data block from display sink.
 *        Note this might not be aligned with DID2 section since DID2 section length is not fixed.
 *
 * @param[in]       aux         The auxiliary bus instance.
 * @param[in]       timer       The timer instance.
 * @param[in]       segment     The segment of the display sink.
 * @param[in]       offset      The offset of the display sink.
 * @param[in/out]   buffer      The buffer to store the data.
 *
 * @return          true if the data is read successfully, false otherwise.
 *
 */

static bool sstReadDid2Block
(
    AuxBus *auxBus,
    Timer  *timer,
    NvU8    segment,
    NvU8    offset,
    Buffer *buffer
)
{
    AuxBus::Type type = AuxBus::i2cMot;
    AuxBus::status auxStatus;

    unsigned retries = 0;
    unsigned sizeRequested;
    unsigned sizeCompleted;
    unsigned transactionSize =  auxBus->transactionSize();
    unsigned totalRead = 0;

    DP_ASSERT(auxBus);
    DP_ASSERT(transactionSize > 0);

    // ASSERT if displayId2x offset wasn't increased in block len sizes
    DP_ASSERT(offset == 0 || offset == NV_DISPLAYID2_BLOCK_SIZE);

    NvU32 totalSizeRequired = NV_DISPLAYID2_BLOCK_SIZE * segment * 2 + offset + NV_DISPLAYID2_BLOCK_SIZE;
    NvU32 startOffset = NV_DISPLAYID2_BLOCK_SIZE * segment * 2 + offset;
    if ((buffer->capacity < totalSizeRequired) && !(buffer->resize(totalSizeRequired)))
    {
        return false;
    }

    DP_ASSERT(sizeof(segment) == 1);
    DP_ASSERT(sizeof(offset) == 1);

    // start DID2 read by specifying appropriate DID2 segment id
    for (unsigned retry = 0; retry < NV_DISPLAYID2_MAX_RETRIES; retry++)
    {
        auxStatus = auxBus->transaction(AuxBus::write, AuxBus::i2cMot, NV_DISPLAYID2_DDC_SEG_SELECTOR_OFFSET >> 1,
                                        &segment, sizeof(segment), &sizeCompleted);
        if (auxStatus == AuxBus::success)
            break;

        // If deferred due to timeout
        if (auxStatus == AuxBus::defer)
        {
            // Wait for sometime between retries
            timer->sleep(NV_DISPLAYID2_AUX_WAIT_TIME);
            continue;
        }

        return false;
    }

    auxStatus = AuxBus::nack;

    for (retries = 0; totalRead < NV_DISPLAYID2_BLOCK_SIZE;)
    {
        //
        // For retry, rewrite the Offset for the internal read pointer
        // except when the previous Read auxstatus was an Aux::defer
        // since in that case, the offset was never incremented by sink
        //
        if ((auxStatus != AuxBus::success) && (auxStatus != AuxBus::defer))
        {
            // start from this offset, need to verify with display with multiple edid blocks
            for (unsigned retry = 0; retry < NV_DISPLAYID2_MAX_AUX_RETRIES; retry++)
            {
                auxStatus = auxBus->transaction(AuxBus::write, AuxBus::i2cMot, NV_DISPLAYID2_DDC_ADDRESS >> 1,
                    (NvU8*)(&offset), sizeof(offset), &sizeCompleted);
                if (auxStatus == AuxBus::success)
                    break;
                // If deferred due to timeout
                if (auxStatus == AuxBus::defer)
                {
                    // Wait for sometime between retries
                    timer->sleep(NV_DISPLAYID2_AUX_WAIT_TIME);
                    continue;
                }

                return false;
            }
            // if retries exceed NV_DISPLAYID2_MAX_AUX_RETRIES, give up
            if (auxStatus != AuxBus::success)
            {
                return false;
            }
        }
        // need to change to I2C (not MOT) to read just one last part of EDID block
        if (totalRead + transactionSize >= NV_DISPLAYID2_BLOCK_SIZE)
            type = AuxBus::i2c;

        sizeRequested = DP_MIN(transactionSize, NV_DISPLAYID2_BLOCK_SIZE - totalRead);
        auxStatus = auxBus->transaction(AuxBus::read, type, NV_DISPLAYID2_DDC_ADDRESS >> 1,
                                        &(buffer->data[startOffset + totalRead]), sizeRequested, &sizeCompleted);

        if (AuxBus::success != auxStatus || (sizeRequested && (sizeCompleted == 0)))
        {
            if (retries >= NV_DISPLAYID2_MAX_AUX_RETRIES)
                return false;

            DP_PRINTF(DP_WARNING, "DisplayPort: %s: Retrying at totalRead 0x%08x (replyType %x, size %x)",
                  __FUNCTION__, totalRead, auxStatus, sizeRequested);

            // Wait for sometime between retries
            timer->sleep(NV_DISPLAYID2_AUX_WAIT_TIME);
            retries++;

            continue;
        }

        // Assert when size mismatches and it is not last block
        if ((sizeRequested != sizeCompleted) &&
            (totalRead + transactionSize < NV_DISPLAYID2_BLOCK_SIZE))
        {
            DP_PRINTF(DP_ERROR, "DisplayPort: %s: dpAux returned edid block smaller than expected. Read from totalRead 0x%08x (replyType %x, size %x)",
                  __FUNCTION__, totalRead, auxStatus, sizeRequested);
            DP_ASSERT(0);
        }

        retries     = 0;                // reset the number of retries
        totalRead   += sizeCompleted;
        offset      += (NvU8)sizeCompleted;
    }

    return true;
}

static bool sstReadDid2(DisplayID2x & did2x, AuxBus * aux, Timer * timer, MainLink *main)
{
    NvU32           vesaDisplayIdSize   = 0;
    NvU32           sectionLength       = 0;
    NvU8            extensionCount      = 0;
    NvU8            loop                = 0;
    NvU8            segment             = 0;
    NvU8            offset              = 0;

    unsigned        retries             = 0;

    bool            status              = false;
    bool            bReadNextBlock      = false;
    NvU32           remainingBytes      = 0;

    NvU8           *pSection            = 0;
    Buffer         *buffer              = did2x.getBuffer();

    const NvU8 blockSize = NV_DISPLAYID2_BLOCK_SIZE;

    do
    {
        // Reset these variables before starting to read data.
        loop = 0;
        remainingBytes = 0;
        sectionLength = 0;
        extensionCount = 0;
        vesaDisplayIdSize = 0;

        // The main loop for reading all sections of a VESA DisplayID structure.
        do
        {
            //
            // The sub loop for doing i2c-over-dpAux sequential read transcation, and the
            // default size per transcation is a 128-byte block here. Although
            // the data size of each transcation is implementation-specific,
            // assuming a 128-byte block here like what we do for EDID increases
            // interoperability with most display sinks.
            //
            do
            {
                if ((remainingBytes <= NV_DISPLAYID2_BYTES_IN_SECTION_MIN) ||
                    (remainingBytes < sectionLength))
                {
                    segment = loop >> 1;
                    offset = (loop & 0x1) * blockSize;

                    status = sstReadDid2Block(aux, timer, segment, offset, buffer);

                    if (!status)
                        break;

                    // Increase the bytes read and loop count.
                    remainingBytes += blockSize;
                    loop++;
                }

                // Point to the current section of VESA DisplayID data structure.
                pSection = buffer->data + vesaDisplayIdSize;

                // Check the VESA DisplayID structure signature/version.
                if (!IS_VALID_DISPLAYID2_VERSION(pSection))
                {
                    status = false;
                    DP_PRINTF(DP_ERROR, "Invalid DisplayID2 version");
                    break;
                }

                // Read the bytes in section of this section.
                sectionLength = GET_DISPLAYID2_SECTION_LENGTH(pSection);

                // Only read the total extension count from Base section.
                if ((segment == 0) && (offset == 0))
                {
                    extensionCount = GET_DISPLAYID2_EXTENSION_COUNT(pSection);

                    //
                    // The initial buffer size sets to a 256-byte data section,
                    // so we're enlarging it to fit the worst-case here.
                    //
                    if (extensionCount > 0)
                    {
                        NvU32   maxSize = sectionLength +
                                          extensionCount * NV_DISPLAYID2_BLOCK_SIZE;

                        status = buffer->resize(maxSize);

                        if (!status)
                        {
                            DP_PRINTF(DP_ERROR, "Failed to resize buffer");
                            return status;
                        }
                        // Base block + extension blocks
                        did2x.setBlockCount(extensionCount + 1);
                    }
                }

                // Check if we need to read more bytes.
                if (sectionLength <= remainingBytes)
                {
                    if (getDID2BlockChecksum(pSection, sectionLength))
                    {
                        status = false;
                        DP_PRINTF(DP_ERROR, "Checksum validation failed");
                        break;
                    }

                    //
                    // The vesaDisplayIdSize is not 0 means that we're parsing
                    // the extension section, so we need to decrease the counter
                    // when the checksum validation is done.
                    //
                    if (vesaDisplayIdSize != 0)
                        extensionCount--;

                    //
                    // Update the total size of VESA DisplayID structure, and
                    // other variables for parsing next section if needed.
                    //
                    vesaDisplayIdSize += sectionLength;
                    remainingBytes -= sectionLength;
                    sectionLength = 0;

                    //
                    // Once this section is validated, set flag to exit current
                    // loop and move to the next section.
                    //
                    bReadNextBlock = false;
                }
                else
                {
                    // We don't have a whole section. Let's read another block.
                    bReadNextBlock = true;
                }
            } while (bReadNextBlock);

            // Something went wrong! Let's retry.
            if (!status)
                break;
        } while (extensionCount > 0);

        retries++;
    } while ((!status) &&
             (retries <= NV_DISPLAYID2_MAX_RETRIES));

    if (status)
    {
        status = buffer->resize(vesaDisplayIdSize);
        did2x.setDisplayId2xSize(vesaDisplayIdSize);
    }

    return status;
}

bool DisplayPort::DisplayId2ReadSST(DisplayID2x & did2x, AuxBus * aux, Timer * timer, MainLink *main)
{
    bool status = false;
    NvU8 retries = 0;

    Buffer *buffer = did2x.getBuffer();
    if (!buffer->resize(NV_DISPLAYID2_BLOCK_SIZE))
    {
        return false;
    }

    if ((aux == NULL) || (timer == NULL))
    {
        DP_PRINTF(DP_ERROR, "DP-DID2> Invalid parameters for SST read. aux: %p, timer: %p", aux, timer);
        return false;
    }

    do {
        status = sstReadDid2(did2x, aux, timer, main);
    } while ((!status) &&
             ((retries++) <= NV_DISPLAYID2_MAX_RETRIES));

    return status;
}

NvU8 DisplayPort::getDID2BlockChecksum(NvU8 * buffer, NvU8 blockSize)
{
    unsigned chksum = 0;
    for (unsigned i = 0; i < blockSize; i++)
    {
        chksum += buffer[i];
    }
    chksum = chksum & 0xFF;
    return (NvU8)chksum;
}
