/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************* DisplayPort********************************\
*                                                                           *
* Module: dp_merger.cpp                                                     *
*    Asynchronous Message merger                                            *
*                                                                           *
\***************************************************************************/

#include "dp_internal.h"
#include "dp_bitstream.h"
#include "dp_merger.h"
#include "dp_auxdefs.h"
#include "dp_crc.h"
#include "dp_messageheader.h"
#include "dp_printf.h"

using namespace DisplayPort;


EncodedMessage * MessageTransactionMerger::pushTransaction(MessageHeader * header, Buffer * data)
{
    if (freeOnNextCall)
    {
        delete freeOnNextCall;
        freeOnNextCall = 0;
    }

    IncompleteMessage * imsg = getTransactionRecord(header->address, header->messageNumber);

    if (!imsg)
    {
        DP_PRINTF(DP_WARNING, "DP-MM> Ignore message due to OOM");
        return 0;
    }

    if (header->isTransactionStart)
    {
        imsg->message.isPathMessage = header->isPathMessage;
        imsg->message.isBroadcast = header->isBroadcast;
    }
    else
    {
        if (imsg->message.buffer.length == 0)
        {
            DP_PRINTF(DP_NOTICE, "DP-MM> Expected transaction-start, ignoring message transaction");
            return 0;
        }

        if (imsg->message.isPathMessage != header->isPathMessage || 
            imsg->message.isBroadcast != header->isBroadcast)
        {
            DP_ASSERT(0 && "Message type changed during transmission");
        }
    }

    //
    //  Check for redundant start
    //
    if (header->isTransactionStart && imsg->message.buffer.length)
    {
        DP_PRINTF(DP_WARNING, "DP-MM> Unexpected repeated transaction-start, resetting message state.");

        // We must have seen a previous incomplete transaction from this device
        // they've begun a new packet.  Forget about the old thing
        imsg->message.buffer.reset();
    }

    //
    //  Kill the buffer if we've got less payload than we should
    //
    if (header->payloadBytes > data->length)
    {
        freeOnNextCall = imsg;
        imsg->message.buffer.reset();
        DP_PRINTF(DP_ERROR, "DP-MM> Received truncated or corrupted message transaction");
        return 0;
    }

    //
    //  Verify transaction CRC
    //
    BitStreamReader bsr(data, header->headerSizeBits, (header->payloadBytes-1)*8);
    NvU8 dataCrc = (NvU8)dpCalculateBodyCRC(&bsr);

    DP_ASSERT(header->headerSizeBits  % 8 == 0 && "Header must be byte aligned");

    if (dataCrc != data->data[header->headerSizeBits/8 + header->payloadBytes - 1] ||
        header->payloadBytes == 0)
    {
        DP_PRINTF(DP_ERROR, "DP-MM> Received corruption message transactions");
        freeOnNextCall = imsg;
        imsg->message.buffer.reset();
        return 0;
    }

    // Discount the processed CRC from the payload count
    header->payloadBytes--; 

    //
    //  Append active buffer
    //
    unsigned i = imsg->message.buffer.length;
    imsg->message.buffer.resize(i + header->payloadBytes);
    dpMemCopy(&imsg->message.buffer.data[i], &data->data[header->headerSizeBits/8], header->payloadBytes);

    //
    //  Check for end of message transaction
    //
    if (header->isTransactionEnd)
    {
        freeOnNextCall = imsg;

        return &imsg->message;
    }

    return 0;
}

MessageTransactionMerger::IncompleteMessage * MessageTransactionMerger::getTransactionRecord(const Address & address, unsigned messageNumber)
{
    IncompleteMessage * msg;
    NvU64 currentTime = this->timer->getTimeUs();

    //
    //  Search for existing record
    //
    for (ListElement * i = incompleteMessages.begin();i != incompleteMessages.end();)
    {
        msg = (IncompleteMessage *)i;
        i = i->next;
        if (msg->message.address == address && msg->message.messageNumber == messageNumber)
        {
            goto found;
        }

        //
        //  Found a stale message in the list
        //
        if (msg->lastUpdated + incompleteMessageTimeoutMs < currentTime)
            delete msg;
    }

    //
    //  None exists? Add a new one
    //
    msg = new IncompleteMessage();
    msg->message.address = address;
    msg->message.messageNumber = messageNumber;
    this->incompleteMessages.insertFront(msg);

found:
    //
    //  Update the timestamp
    //
    msg->lastUpdated = currentTime;

    return msg;
}

void IncomingTransactionManager::mailboxInterrupt()
{
    MessageHeader msg;
    unsigned totalSize;
    AuxRetry::status result;
    unsigned txSize = (unsigned)getTransactionSize();
    
    //
    //  Size the static aux window
    //
    this->localWindow.resize(DP_MAX((unsigned)getTransactionSize(), (unsigned)getMessageBoxSize()));
    if (this->localWindow.isError())
        return;

    //
    //  Read one aux-transaction worth of data
    //
    result = readMessageBox(0, &this->localWindow.data[0], txSize);

    DP_ASSERT( result != AuxRetry::defer && "Unexpected?!" );

    if (result != AuxRetry::ack)
        return;

    BitStreamReader reader(&this->localWindow, 0, 8*txSize);


    //
    //  Before decoding the header, start with the downstream
    //    ports address prefix
    //
    if (!decodeHeader(&reader, &msg, addressPrefix))
    {
        //
        // It's possible we should be NACKing here.  Ignoring for now
        // to allow the message originator to time out (can take seconds).
        //
        DP_ASSERT(0 && "Not yet implemented");

        return;
    }

    //
    //  Let's get the entire sideband message in the localWindow
    //

    totalSize = (msg.headerSizeBits / 8) + msg.payloadBytes;

    if (totalSize > txSize)
    {
        if (totalSize > DPCD_MESSAGEBOX_SIZE)
        {
            //
            //  Corrupt packet - total packet can't be larger than the window
            //
            return;
        }
        if (AuxRetry::ack!=readMessageBox(txSize, &this->localWindow.data[txSize], totalSize - txSize))
        {
            //
            //  Failed to read second half of message
            //
            return;
        }
    }

    clearMessageBoxInterrupt();

    EncodedMessage * em = incompleteMessages.pushTransaction(&msg, &this->localWindow);
  
    if (em)
    {
        this->sink->messagedReceived(this, em);
    }
}

IncomingTransactionManager::~IncomingTransactionManager()
{
}


IncomingTransactionManager::IncomingTransactionManager(Timer * timer, const Address & addressPrefix, IncomingTransactionManagerEventSink * sink)
    : incompleteMessages(timer, DP_INCOMPLETE_MESSAGE_TIMEOUT_USEC), addressPrefix(addressPrefix)
{
    this->sink = sink;
    this->timer = timer;
}



AuxRetry::status DownReplyManager::readMessageBox(NvU32 offset, NvU8 * data, size_t length) 
{
    return hal->readDownReplyMessageBox(offset, data, length);
}

size_t           DownReplyManager::getMessageBoxSize() 
{
    return hal->getDownReplyMessageBoxSize();
}

size_t           DownReplyManager::getTransactionSize()
{
    return hal->getTransactionSize();
}

void     DownReplyManager::clearMessageBoxInterrupt()
{
    hal->clearInterruptDownReplyReady();
}

AuxRetry::status UpRequestManager::readMessageBox(NvU32 offset, NvU8 * data, size_t length) 
{
    return hal->readUpRequestMessageBox(offset, data, length);
}

size_t           UpRequestManager::getMessageBoxSize() 
{
    return hal->getUpRequestMessageBoxSize();
}

size_t           UpRequestManager::getTransactionSize()
{
    return hal->getTransactionSize();
}

void     UpRequestManager::clearMessageBoxInterrupt()
{
    hal->clearInterruptUpRequestReady();
}

