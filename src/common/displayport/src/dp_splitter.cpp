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
* Module: dp_splitter.cpp                                                   *
*    Asynchronous Message Splitter                                          *
*                                                                           *
\***************************************************************************/

#include "dp_internal.h"
#include "dp_bitstream.h"
#include "dp_splitter.h"
#include "dp_auxdefs.h"
#include "dp_crc.h"
#include "dp_configcaps.h"
#include "dp_printf.h"

using namespace DisplayPort;

#define DP_MAX_HEADER_SIZE                   16
// timeout after 110ms with a retry recurring every 5ms for 10 times
#define DOWNSTREAM_RETRY_ON_DEFER_TIMEOUT    110
#define DOWNSTREAM_RETRY_ON_DEFER_PERIOD     5
#define DOWNSTREAM_RETRY_ON_DEFER_COUNT      10

bool MessageTransactionSplitter::get(Buffer & assemblyBuffer)
{
    unsigned i;
    unsigned payloadSize;
    bool isTransactionStart, isTransactionEnd;
    Address address;
    unsigned LCT;
    unsigned LCR;
    unsigned headerSizeBits;

    assemblyBuffer.reset();

    //
    //  Done?
    //
    if (this->messageOutstanding->buffer.length == this->assemblyTransmitted)
    {
        return false;
    }

    address = this->messageOutstanding->address;
    if (this->messageOutstanding->isBroadcast)
    {
        // no RAD
        address.clear();
        LCT = 1;
    }
    else
    {
        LCT = address.size();
    }

    //  Calculate header size
    headerSizeBits = 8 +                              // LCT/LCR
                     (((4 * (LCT -1)) + 4) &~ 7) +    // byte aligned RAD
                     16;

    //
    //  Pick how much data to send.  Header+payloadSize <= 48 bytes.
    //
    payloadSize = DP_MIN(DPCD_MESSAGEBOX_SIZE - (headerSizeBits+7)/8, /*crc*/1 + this->messageOutstanding->buffer.length - this->assemblyTransmitted);

    //
    //  Is the first or last transaction in the sequence?
    //
    isTransactionStart = assemblyTransmitted == 0;
    isTransactionEnd = (assemblyTransmitted + payloadSize - 1) == messageOutstanding->buffer.length;

    BitStreamWriter writer(&assemblyBuffer, 0);

    //
    //  Write the header
    //
    writer.write(LCT, 4);

    LCR = this->messageOutstanding->isBroadcast ? 6 : LCT > 1 ? LCT - 1 : 0;

    writer.write(LCR, 4);

    // port at i=0 is the outport of source/gpu which should not be included in the RAD in outgoing message header
    // if this is a broadcast message; LCT would be 1; hence no RAD.
    for (i = 1; i < LCT; i++)
        writer.write(address[i], 4);
    writer.align(8);

    writer.write(this->messageOutstanding->isBroadcast, 1);
    writer.write(this->messageOutstanding->isPathMessage, 1);
    writer.write(payloadSize, 6);

    writer.write(isTransactionStart, 1);
    writer.write(isTransactionEnd, 1);
    writer.write(0, 1);

    DP_ASSERT(messageOutstanding->messageNumber == 0 || messageOutstanding->messageNumber == 1);
    writer.write(messageOutstanding->messageNumber, 1);

    //
    //  Generate 4 bit CRC. (Nibble-wise CRC of previous values)
    //
    BitStreamReader reader(&assemblyBuffer, 0, writer.offset());
    writer.write(dpCalculateHeaderCRC(&reader), 4);

    DP_ASSERT(writer.offset() == headerSizeBits && "Header size mismatch");
    DP_ASSERT((writer.offset() & 7) == 0 && "Packet header must end byte aligned");

    //
    //    Generate body CRC
    //
    BitStreamReader bodyReader(&this->messageOutstanding->buffer, this->assemblyTransmitted * 8, (payloadSize - 1) * 8);
    NvU8 bodyCrc = (NvU8)dpCalculateBodyCRC(&bodyReader);

    // Copy in remaining buffer (leaving room for the CRC)
    for (i = 0; i < payloadSize - 1; ++i)
        writer.write(this->messageOutstanding->buffer.data[i + this->assemblyTransmitted], 8);
    writer.write(bodyCrc, 8);

    this->assemblyTransmitted += payloadSize - 1;

    return true;
}

void OutgoingTransactionManager::expired(const void * tag)
{
    writeToWindow(false);
}

void OutgoingTransactionManager::cancel(OutgoingTransactionManagerEventSink * sink)
{
    if (activeMessage && activeMessage->eventSink == sink)
        activeMessage->eventSink = 0;

    for (ListElement * el = queuedMessages.begin(); el && el!=queuedMessages.end(); el = el->next)
        if (((OutgoingMessage *)el)->eventSink == sink)
            ((OutgoingMessage *)el)->eventSink = 0;
}

bool OutgoingTransactionManager::send( EncodedMessage & payload, OutgoingTransactionManagerEventSink * sink)
{
    OutgoingMessage * om = new OutgoingMessage();

    if (!om)
    {
        return false;
    }

    om->eventSink = sink;
    om->message.swap(payload);

    if (!activeMessage)
    {
        activeMessage = om;
        transactionSplitter.set(&om->message);
        transactionSplitter.get(this->assemblyBuffer);
        writeToWindow(true);
    }
    else
    {
        queuedMessages.insertBack(om);
    }

    return true;
}

void OutgoingTransactionManager::writeToWindow( bool firstAttempt)
{
    AuxRetry::status result;

    if (!activeMessage || !activeMessage->eventSink)
        goto findNextMessage;

    result = this->writeMessageBox(assemblyBuffer.data, assemblyBuffer.length);

    if (result == AuxRetry::defer)
    {

        //
        //  if retries left; queue one.
        //
        if (firstAttempt || retriesLeft )
        {
            if (firstAttempt)
            {
                // initialize retriesLeft
                retriesLeft = DOWNSTREAM_RETRY_ON_DEFER_COUNT;
            }

            retriesLeft--;
            DP_PRINTF(DP_WARNING, "DP-MM> Messagebox write defer-ed. Q-ing retry.");
            this->timer->queueCallback(this, "SPDE", DOWNSTREAM_RETRY_ON_DEFER_PERIOD);

            return;
        }

        //
        // Notify message sender of failure. Keep in mind sender
        // might turn around immediately with a queue'd send.
        //
        if (activeMessage)
        {
            activeMessage->eventSink->splitterFailed(this);
        }

        goto findNextMessage;
    }
    else if (result == AuxRetry::ack)
    {
        //
        //  Split off another chunk and transmit
        //
        if (transactionSplitter.get(assemblyBuffer))
        {
            writeToWindow(true);
        }
        else
        {
            //
            // Notify message sender of success. Keep in mind sender
            // might turn around immediately with a queue'd send.
            //
            if (activeMessage)
            {
                activeMessage->eventSink->splitterTransmitted(this);
            }

            goto findNextMessage;
        }

        return;
    }

    //
    // Notify message sender of failure. Keep in mind sender
    // might turn around immediately with a queued send.
    //
    if (activeMessage)
    {
        activeMessage->eventSink->splitterFailed(this);
    }

findNextMessage:
    //
    // The old transaction is complete. Free the memory
    //
    delete activeMessage;
    activeMessage = 0;

    //
    //  Look for the next transaction
    //
    if (queuedMessages.isEmpty())
    {
        return;
    }
    else
    {
        activeMessage = (OutgoingMessage *)queuedMessages.begin();
        queuedMessages.remove(activeMessage);

        transactionSplitter.set(&activeMessage->message);
        transactionSplitter.get(this->assemblyBuffer);
        writeToWindow(true);
    }
}

OutgoingTransactionManager::OutgoingTransactionManager(Timer * timer)
    : timer(timer)
{
    this->activeMessage = 0;
}

AuxRetry::status DownRequestManager::writeMessageBox(NvU8 * data, size_t length)
{
    return hal->writeDownRequestMessageBox(data, length);
}

size_t           DownRequestManager::getMessageBoxSize()
{
    return hal->getDownRequestMessageBoxSize();
}

AuxRetry::status UpReplyManager::writeMessageBox(NvU8 * data, size_t length)
{
    return hal->writeUpReplyMessageBox(data, length);
}

size_t           UpReplyManager::getMessageBoxSize()
{
    return hal->getUpReplyMessageBoxSize();
}
