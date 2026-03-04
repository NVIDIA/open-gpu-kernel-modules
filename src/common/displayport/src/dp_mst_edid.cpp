/*
 * SPDX-FileCopyrightText: Copyright (c) 2010-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
* Module: dp_mst_edid.c                                                     *
*    Implementation Multi Stream EDID reads                                 *
*                                                                           *
\***************************************************************************/

#include "dp_internal.h"
#include "dp_edid.h"
#include "dp_address.h"
#include "dp_messagecodings.h"
#include "dp_messages.h"
#include "dp_printf.h"

using namespace DisplayPort;

EdidReadMultistream::~EdidReadMultistream()
{
    timer->cancelCallbacks(this);
}

void EdidReadMultistream::startReadingEdid()
{
    NvU8 offset = 0;
    I2cWriteTransaction i2cWriteTransactions[1];
    Address::StringBuffer buffer;
    DP_USED(buffer);
    DP_PRINTF(DP_NOTICE, "%s(): start for %s", __FUNCTION__,
                                      topologyAddress.toString(buffer));

    edidReaderManager.reset();
    edid.resetData();

    DDCAddress = ddcAddrList[ddcIndex];

    // set offset within segment 0, no need to set segment, because we're starting reading EDID
    i2cWriteTransactions[0] = I2cWriteTransaction(DDCAddress >> 1,
                                                  sizeof(offset),
                                                  &offset,
                                                  true);
    NvU8 nWriteTransactions = 1;

    remoteI2cRead.set(topologyAddress.parent(), // topology Address
        nWriteTransactions,             // number of write transactions
        topologyAddress.tail(),         // port of Device
        i2cWriteTransactions,           // list of write transactions
        DDCAddress >> 1,                // right shifted DDC Address (request identifier in spec)
        EDID_BLOCK_SIZE);               // requested size

    manager->post(&remoteI2cRead, this);
}

void EdidReadMultistream::messageCompleted(MessageManager::Message * from)
{
    RemoteI2cReadMessage* I2CReadMessage = (RemoteI2cReadMessage*)from;
    unsigned char * data = 0;
    unsigned numBytesRead;
    Address::StringBuffer buffer;
    DP_USED(buffer);

    NvU8 seg;
    NvU8 offset;
    DP_PRINTF(DP_NOTICE, "%s for %s", __FUNCTION__, topologyAddress.toString(buffer));

    DP_ASSERT(DDCAddress && "DDCAddress is 0, it is wrong");

    data = I2CReadMessage->replyGetI2CData(&numBytesRead);
    DP_ASSERT(data);

    // this is not required, but I'd like to keep things simple at first submission
    DP_ASSERT(numBytesRead == EDID_BLOCK_SIZE);
    edidReaderManager.postReply(data, numBytesRead, true);

    if (edidReaderManager.readNextRequest(seg, offset))
    {
        readNextBlock(seg, offset);
    }
    else    // EDID read is finished or failed.
    {
        edidAttemptDone(edidReaderManager.readIsComplete() && edid.verifyCRC());
    }
}

void EdidReadMultistream::edidAttemptDone(bool succeeded)
{
    if (succeeded)
        sink->mstEdidCompleted(this);
    else if (ddcIndex + 1 < ddcAddrListSize)
    {
        ddcIndex++;
        startReadingEdid();
    }
    else
        sink->mstEdidReadFailed(this);
}

void EdidReadMultistream::readNextBlock(NvU8 seg, NvU8 offset)
{
    I2cWriteTransaction i2cWriteTransactions[2];
    Address::StringBuffer buffer;
    DP_USED(buffer);

    // ensure that init function for i2cWriteTranscation for segment and offset won't break
    DP_ASSERT(sizeof(seg) == 1);
    DP_ASSERT(sizeof(offset) == 1);

    DP_PRINTF(DP_NOTICE, "%s(): for %s (seg/offset) = %d/%d", __FUNCTION__,
                                      topologyAddress.toString(buffer),
                                      seg, offset);

    unsigned nWriteTransactions = 2;
    if (seg)
    {
        // select segment
        i2cWriteTransactions[0] = I2cWriteTransaction(EDID_SEG_SELECTOR_OFFSET >> 1,
                                                      1, &seg, true);
        // set offset within segment
        i2cWriteTransactions[1] = I2cWriteTransaction(DDCAddress >> 1,
                                                      1, &offset, true);
    }
    else
    {
        // set offset within segment 0
        i2cWriteTransactions[0] = I2cWriteTransaction(DDCAddress >> 1, 1, &offset, true);
        nWriteTransactions = 1;
    }

    remoteI2cRead.set(topologyAddress.parent(), // topology Address
        nWriteTransactions,             // number of write transactions
        topologyAddress.tail(),         // port of Device
        i2cWriteTransactions,           // list of write transactions
        DDCAddress >> 1,                // right shifted DDC Address (request identifier in spec)
        EDID_BLOCK_SIZE);               // requested size

    manager->post(&remoteI2cRead, this, false);
}

void EdidReadMultistream::expired(const void * tag)
{
    Address::StringBuffer buffer;
    DP_USED(buffer);
    DP_PRINTF(DP_NOTICE, "%s on %s", __FUNCTION__, topologyAddress.toString(buffer));
    startReadingEdid();
}

void EdidReadMultistream::messageFailed(MessageManager::Message * from, NakData * nakData)
{
    Address::StringBuffer buffer;
    DP_USED(buffer);
    DP_PRINTF(DP_NOTICE, "%s on %s", __FUNCTION__, topologyAddress.toString(buffer));

    if (nakData->reason == NakDefer || nakData->reason == NakTimeout)
    {
        if (retries < MST_EDID_RETRIES)
        {
            ++retries;
            timer->queueCallback(this, "EDID", MST_EDID_COOLDOWN);
        }
        else
            edidAttemptDone(false /* failed */);
    }
    else
    {
        edidAttemptDone(false /* failed */);
    }
}
