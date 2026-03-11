/*
 * SPDX-FileCopyrightText: Copyright (c) 2025-2026 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
* Module: dp_mst_displayid2.c                                               *
*    Implementation Multi Stream DisplayID20 reads                          *
*                                                                           *
\***************************************************************************/

#include "dp_internal.h"
#include "dp_displayid2.h"
#include "dp_address.h"
#include "dp_messagecodings.h"
#include "dp_messages.h"
#include "dp_printf.h"

using namespace DisplayPort;

DID2ReadMultistream::~DID2ReadMultistream()
{
    timer->cancelCallbacks(this);
}

void DID2ReadMultistream::startReadingDid2()
{
    NvU8 offset             = 0;
    NvU8 nWriteTransactions = 0;

    I2cWriteTransaction i2cWriteTransactions[2];
    Address::StringBuffer buffer;
    DP_USED(buffer);
    DP_PRINTF(DP_NOTICE, "%s(): start for %s", __FUNCTION__, topologyAddress.toString(buffer));

    // Initialize the variables
    remainingBytes = 0;
    loop = 0;
    sectionLength = 0;
    extensionCount = 0;
    displayId2xSize = 0;

    // Reset the offset only.
    i2cWriteTransactions[0] = I2cWriteTransaction(NV_DISPLAYID2_DDC_ADDRESS >> 1,
                                                  sizeof(offset), &offset, true);

    nWriteTransactions = 1;

    remoteI2cRead.set(topologyAddress.parent(),         // topology Address
                      nWriteTransactions,               // number of write transactions
                      topologyAddress.tail(),           // port of Device
                      i2cWriteTransactions,             // list of write transactions
                      NV_DISPLAYID2_DDC_ADDRESS >> 1,   // right shifted DDC Address (request identifier in spec)
                      NV_DISPLAYID2_BLOCK_SIZE);        // requested size

    manager->post(&remoteI2cRead, this);
}

void DID2ReadMultistream::messageCompleted(MessageManager::Message * from)
{
    RemoteI2cReadMessage* I2CReadMessage = (RemoteI2cReadMessage*)from;
    unsigned char * data = 0;
    unsigned numBytesRead;
    Address::StringBuffer buffer;
    DP_USED(buffer);

    DP_PRINTF(DP_NOTICE, "- for %s", topologyAddress.toString(buffer));

    data = I2CReadMessage->replyGetI2CData(&numBytesRead);
    DP_ASSERT(data);

    remainingBytes += numBytesRead;

    stream.write(data, numBytesRead);

    bool bNotFinished = readNextRequest();

    if (bNotFinished)
    {
        loop++;
        readNextBlock();
    }
    else
    {
        // Did20 read is finished or failed.
        sink->mstDid2Completed(this);
    }
}

void DID2ReadMultistream::expired(const void * tag)
{
    Address::StringBuffer buffer;
    DP_USED(buffer);
    DP_PRINTF(DP_NOTICE, "%s on %s", __FUNCTION__, topologyAddress.toString(buffer));
    startReadingDid2();
}

void DID2ReadMultistream::messageFailed(MessageManager::Message * from, NakData * nakData)
{
    Address::StringBuffer buffer;
    DP_USED(buffer);
    DP_PRINTF(DP_NOTICE, "%s on %s", __FUNCTION__, topologyAddress.toString(buffer));

    if (nakData->reason == NakDefer || nakData->reason == NakTimeout)
    {
        if (retries < NV_DISPLAYID2_MAX_RETRIES)
        {
            ++retries;
            timer->queueCallback(this, "DID2", NV_DISPLAYID2_READ_RETRY_MS);
        }
        else
            sink->mstDid2ReadFailed(this);
    }
    else
    {
        sink->mstDid2ReadFailed(this);
    }
}

/*!
 * @brief Check if there is more blocks to read.
 *
 * @param[in/out]   seg         The segment of the display sink.
 * @param[in/out]   offset      The offset of the display sink.
 *
 * @return          true if there is more blocks to read, false otherwise.
 *
 */
bool DID2ReadMultistream::readNextRequest()
{
    Buffer * buffer = displayId2x.getBuffer();

    pSection = buffer->getData() + displayId2xSize;

    if (!IS_VALID_DISPLAYID2_VERSION(pSection))
    {
        DP_PRINTF(DP_ERROR, "Invalid DisplayID2 version");
        return false;
    }

    sectionLength = GET_DISPLAYID2_SECTION_LENGTH(pSection);
    if (loop == 0)
    {
        // Only read the total extension count from Base section.
        extensionCount = GET_DISPLAYID2_EXTENSION_COUNT(pSection);
        if (extensionCount > 0)
        {
            NvU32 maxSize = sectionLength + extensionCount * NV_DISPLAYID2_BLOCK_SIZE;

            if (!buffer->resize(maxSize))
            {
                DP_PRINTF(DP_ERROR, "Failed to resize buffer");
                return false;
            }
        }
    }

    if (sectionLength > remainingBytes)
    {
        // the current section is not complete, and we don't have enough bytes already.
        return true;
    }

    // Check if we need to read more bytes.
    if (sectionLength <= remainingBytes)
    {
        if (getDID2BlockChecksum(pSection, sectionLength))
        {
            // Something went wrong! Let's retry.
            if (retried)
            {
                DP_PRINTF(DP_ERROR, "Checksum validation still failed after retries");
                return false;
            }
            DP_PRINTF(DP_ERROR, "Checksum validation failed, retrying...");
            retried = true;
            return true;
        }

        //
        // The displayId2xSize is not 0 means that we're parsing
        // the extension section, so we need to decrease the counter
        // when the checksum validation is done.
        //
        if (displayId2xSize != 0)
            extensionCount--;

        //
        // Update the total size of VESA DisplayID structure, and
        // other variables for parsing next section if needed.
        //
        displayId2xSize += sectionLength;
        remainingBytes -= sectionLength;
        pSection = buffer->getData() + displayId2xSize;
        sectionLength = 0;
    }
    displayId2x.setDisplayId2xSize(displayId2xSize);
    return (extensionCount != 0);
}

void DID2ReadMultistream::readNextBlock()
{
    NvU8 seg    = loop >> 1;
    NvU8 offset = (loop & 0x1) * NV_DISPLAYID2_BLOCK_SIZE;

    I2cWriteTransaction i2cWriteTransactions[2];
    Address::StringBuffer buffer;
    DP_USED(buffer);

    // ensure that init function for i2cWriteTranscation for segment and offset won't break
    DP_ASSERT(sizeof(seg) == 1);
    DP_ASSERT(sizeof(offset) == 1);

    DP_PRINTF(DP_NOTICE, "DID2ReadMultistream::readNextBlock(): for %s (seg/offset) = %d/%d",
                          topologyAddress.toString(buffer),
                          seg, offset);

    unsigned nWriteTransactions = 2;
    // select segment
    i2cWriteTransactions[0] = I2cWriteTransaction(NV_DISPLAYID2_DDC_SEG_SELECTOR_OFFSET >> 1,
                                                  1, &seg, true);
    // set offset within segment
    i2cWriteTransactions[1] = I2cWriteTransaction(NV_DISPLAYID2_DDC_ADDRESS >> 1,
                                                  1, &offset, true);

    remoteI2cRead.set(topologyAddress.parent(),         // topology Address
                      nWriteTransactions,               // number of write transactions
                      topologyAddress.tail(),           // port of Device
                      i2cWriteTransactions,             // list of write transactions
                      NV_DISPLAYID2_DDC_ADDRESS >> 1,   // right shifted DDC Address (request identifier in spec)
                      NV_DISPLAYID2_BLOCK_SIZE);        // requested size

    manager->post(&remoteI2cRead, this, false);
}
