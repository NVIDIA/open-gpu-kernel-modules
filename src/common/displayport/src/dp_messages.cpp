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
* Module: dp_messages.cpp                                                   *
*    Encoding for aux common messages.                                      *
*                                                                           *
\***************************************************************************/
#include "dp_internal.h"
#include "dp_bitstream.h"
#include "dp_splitter.h"
#include "dp_messages.h"
#include "dp_merger.h"
#include "dp_list.h"
#include "dp_tracing.h"
#include "dp_printf.h"

using namespace DisplayPort;
namespace DisplayPort
{
    GenericMessageCompletion::GenericMessageCompletion() :
        failed(false), completed(false)
    {
        // Initialize nakData seperately.
        nakData.reason      = NakUndefined;
        nakData.nak_data    = 0;
        // nakData.guid is initalized in its own constructor.
    }
    void GenericMessageCompletion::messageFailed(MessageManager::Message * from, NakData * data)
    {
        nakData = *data;
        failed = true;
        completed = true;
    }
    void GenericMessageCompletion::messageCompleted(MessageManager::Message * from)
    {
        failed = false;
        completed = true;
    }
};

//
//     Transmit a message and wait for the response in place.
//
bool MessageManager::send(MessageManager::Message * message, NakData & nakData)
{
    GenericMessageCompletion completion;
    Address::StringBuffer sb;
    DP_USED(sb);

    NvU64 startTime, elapsedTime;
    message->bBusyWaiting = true;
    message->setMessagePriority(NV_DP_SBMSG_PRIORITY_LEVEL_1);
    post(message, &completion);
    startTime = timer->getTimeUs();
    do
    {
        hal->updateDPCDOffline();
        if (hal->isDpcdOffline())
        {
            DP_PRINTF(DP_WARNING, "DP-MM> Device went offline while waiting for reply and so ignoring message %p (ID = %02X, target = %s)",
                      message, message->requestIdentifier, ((message->state).target).toString(sb));
            completion.nakData.reason = NakDpcdFail;
            nakData = completion.nakData;
            completion.failed = true;
            break;
        }

        hal->notifyIRQ();
        if (hal->interruptDownReplyReady())
            IRQDownReply();

        if (completion.completed)
        {
            nakData = completion.nakData;
            break;
        }

        elapsedTime = timer->getTimeUs() - startTime;

        if (elapsedTime > (DPCD_MESSAGE_REPLY_TIMEOUT * 1000))
        {
            message->expired(NULL);
            nakData.reason = NakTimeout;
            break;
        }

        // Sleep while processing timer callbacks
        timer->sleep(1);
    } while(true);

    return !completion.failed;
}

bool DisplayPort::extractGUID(BitStreamReader * reader, GUID * guid)
{
    for (unsigned i=0; i < 128; i += 8)
    {
        unsigned data;
        if (!reader->read(&data, 8))
        {
            return false;
        }

        guid->data[i/8] = (NvU8)data;
    }

    return true;
}

void  MessageManager::messagedReceived(IncomingTransactionManager * from, EncodedMessage * message)
{
    if (from == &mergerUpRequest)
    {
        onUpRequestReceived(true, message);
    }
    else
    {
        onDownReplyReceived(true, message);
    }
}

void MessageManager::Message::splitterFailed(OutgoingTransactionManager * from)
{
    //
    //  Message failed
    //
    NakData nakData;
    nakData.reason = NakTimeout;
    MessageManager * parent = this->parent;

    if (sink)
        sink->messageFailed(this, &nakData);

    if (from == &parent->splitterDownRequest)
    {
        //
        // Tell the message manager he may begin sending the next message
        //
        parent->transmitAwaitingDownRequests();
    }
    else
    {
        parent->transmitAwaitingUpReplies();
    }
}

void MessageManager::Message::splitterTransmitted(OutgoingTransactionManager * from)
{
    bTransmitted = true;
    MessageManager * parent = this->parent;

    if (from == &parent->splitterDownRequest)
    {
        // Client will busy-waiting for the message to complete, we don't need the countdown timer.
        if (!bBusyWaiting)
        {
            // Start the countdown timer for the reply
            parent->timer->queueCallback(this, "SPLI", DPCD_MESSAGE_REPLY_TIMEOUT);
        }
        // Tell the message manager he may begin sending the next message
        parent->transmitAwaitingDownRequests();
    }
    else    // UpReply
    {
        if (sink)
            sink->messageCompleted(this);           // This is the end for an up reply

        parent->transmitAwaitingUpReplies();
    }

}

// Since transmit DPCD_MESSAGE_REPLY_TIMEOUT time has elapsed.
//   - Let's assume the message was not replied to
void  MessageManager::Message::expired(const void * tag)
{
    Address::StringBuffer sb;
    DP_USED(sb);

    DP_PRINTF(DP_WARNING, "DP-MM> Message transmit time expired on message %p (ID = %02X, target = %s)",
          (Message*)this, ((Message*)this)->requestIdentifier, (((Message*)this)->state.target).toString(sb));

    Address::NvU32Buffer addrBuffer;
    dpMemZero(addrBuffer, sizeof(addrBuffer));
    (((Message*)this)->state.target).toNvU32Buffer(addrBuffer);
    NV_DPTRACE_WARNING(MESSAGE_EXPIRED, ((Message*)this)->requestIdentifier, (((Message*)this)->state.target).size(),
                       addrBuffer[0], addrBuffer[1], addrBuffer[2], addrBuffer[3]);

    NakData nakData;
    nakData.reason = NakTimeout;

    MessageManager * parent = this->parent;

    DP_ASSERT(parent);
    if (parent && !parent->isBeingDestroyed)
    {
        parent->awaitingReplyDownRequest.remove(this);
        parent->clearPendingMsg();
        parent->transmitAwaitingDownRequests();
        parent->transmitAwaitingUpReplies();
    }

    if (sink)
        sink->messageFailed(this, &nakData);
}

//
//  Enqueue the next message to the splitterDownRequest
//
void MessageManager::transmitAwaitingDownRequests()
{
    for (ListElement * i = notYetSentDownRequest.begin(); i!=notYetSentDownRequest.end(); )
    {
        Message * m = (Message *)i;
        i = i->next;                    // Do this first since we may unlink the current node

        if (awaitingReplyDownRequest.isEmpty())
        {
            //
            //    Set the message number, and unlink from the outgoing queue
            //
            m->encodedMessage.messageNumber = 0;
            m->state.messageNumber = 0;

            notYetSentDownRequest.remove(m);
            awaitingReplyDownRequest.insertBack(m);

            //
            //  This call can cause transmitAwaitingDownRequests to be called again
            //
            bool sent = splitterDownRequest.send(m->encodedMessage, m);
            DP_ASSERT(sent);

            return;
        }
    }
}

//
//  Enqueue the next message to the splitterUpReply
//
void MessageManager::transmitAwaitingUpReplies()
{
    for (ListElement * i = notYetSentUpReply.begin(); i!=notYetSentUpReply.end(); )
    {
        Message * m = (Message *)i;
        i = i->next;                    // Do this first since we may unlink the current node

        notYetSentUpReply.remove(m);

        //
        //  This call can cause transmitAwaitingUpReplies to be called again
        //
        bool sent = splitterUpReply.send(m->encodedMessage, m);
        DP_ASSERT(sent);
    }
}

void MessageManager::postReply(Message * message, Message::MessageEventSink * sink)
{
    post(message, sink, true);
}

void MessageManager::cancelAllByType(unsigned type)
{
    for (ListElement * i = notYetSentDownRequest.begin(); i!=notYetSentDownRequest.end(); )
    {
        Message * m = (Message *)i;
        i = i->next;

        if (m->requestIdentifier == type)
            notYetSentDownRequest.remove(m);
    }

    for (ListElement * i = awaitingReplyDownRequest.begin(); i!=awaitingReplyDownRequest.end(); )
    {
        Message * m = (Message *)i;
        i = i->next;

        if (m->requestIdentifier == type)
            awaitingReplyDownRequest.remove(m);
    }
}

void MessageManager::cancelAll(Message * message)
{
    for (ListElement * i = notYetSentDownRequest.begin(); i!=notYetSentDownRequest.end(); )
    {
        Message * m = (Message *)i;
        i = i->next;

        if (m == message && m->requestIdentifier == message->requestIdentifier)
            notYetSentDownRequest.remove(m);
    }

    for (ListElement * i = awaitingReplyDownRequest.begin(); i!=awaitingReplyDownRequest.end(); )
    {
        Message * m = (Message *)i;
        i = i->next;

        if (m == message && m->requestIdentifier == message->requestIdentifier)
            awaitingReplyDownRequest.remove(m);
    }
}

void MessageManager::post(Message * message, Message::MessageEventSink * sink, bool transmitReply)
{
    DP_ASSERT(!isBeingDestroyed && "You may not post messages in response to a shutdown");

    if (isPaused)
        return;

    //
    //  Initialize the fields
    //
    message->sink = sink;
    message->bTransmitted = false;

    //
    //    Queue the message for the outgoing queue.
    //         Later on we'll walk to the queue and make sure
    //         we have at most two outstanding messages PER
    //         target address.  This is how the message
    //         number is decided.
    //

    message->parent = this;
    message->transmitReply = transmitReply;
    if (message->encodedMessage.isBroadcast)
    {
        // if its a broadcast message; the target would be the immediate branch.
        Address addr;
        addr.clear();
        addr.append(0);
        message->state.target = addr;
    }
    else
        message->state.target = message->encodedMessage.address;

    if ( transmitReply )
    {
        notYetSentUpReply.insertBack(message);
        transmitAwaitingUpReplies();
    }
    else
    {
        //
        //   If the list is empty or the incoming message has the least priority possible (DEFAULT priority),
        //   then just add the incoming  message to the back of the list.
        //   Otherwise, find the right location by traversing the list.
        //
        if(message->messagePriority == NV_DP_SBMSG_PRIORITY_LEVEL_DEFAULT || notYetSentDownRequest.isEmpty())
        {
            notYetSentDownRequest.insertBack(message);
        }
        else
        {
            ListElement *tmp = notYetSentDownRequest.last();
            Message *msg = (Message*) notYetSentDownRequest.last();
            while((msg->prev != tmp) && (msg->messagePriority < message->messagePriority))
            {
                msg = (Message*)msg->prev;
            }
            notYetSentDownRequest.insertBefore(msg->next, message);
        }
        transmitAwaitingDownRequests();
    }
}

void MessageManager::onUpRequestReceived(bool status, EncodedMessage * message)
{
    if (!status)
    {
        return;
    }

    //
    //  Broadcast the up-request message to all
    //  the receivers on messageReceivers
    //
    for (ListElement * i = messageReceivers.begin(); i!=messageReceivers.end(); i=i->next)
    {
        MessageReceiver * rcr = (MessageReceiver *)i;
        if (rcr->process((EncodedMessage *)message))
        {
            return;
        }
    }

    DP_ASSERT(0 && "Warning: Unknown upstream UP_REQ message");
}


void MessageManager::onDownReplyReceived(bool status, EncodedMessage * message)
{
    if (!status)
    {
        return;
    }

    //
    //  Broadcast the down-request message to all
    //  the receivers on awaitingReplyDownRequest
    //
    for (ListElement * i = awaitingReplyDownRequest.begin(); i!=awaitingReplyDownRequest.end(); i=i->next)
    {
        Message * messageAwaitingReply = (Message *)i;

        if( messageAwaitingReply->state.target == message->address &&
            messageAwaitingReply->state.messageNumber == message->messageNumber)
        {
            awaitingReplyDownRequest.remove(messageAwaitingReply);
            if (messageAwaitingReply->parseResponse(message) == ParseResponseWrong)
            {
                //
                // parseResponse() returns ParseResposeWrong when 'Request_Identifier' of down request
                // message and down reply message are mis-matched. So insert message in waiting queue
                // and wait for correct down reply message.
                //
                awaitingReplyDownRequest.insertBack(messageAwaitingReply);
            }

            goto nextMessage;
        }
    }

    DP_PRINTF(DP_WARNING, "DPMM> Warning: Unmatched reply message");
nextMessage:
    transmitAwaitingUpReplies();
    transmitAwaitingDownRequests();
}

MessageManager::~MessageManager()
{
    // This causes any posts they may attempt to do to fail
    isBeingDestroyed = true;

    //
    // The message manager should not be shut down until
    // all outgoing messages are in the cancelled state
    //
    NakData nakUndef;
    nakUndef.reason = NakUndefined;

    for (ListElement * i = notYetSentDownRequest.begin(); i!=notYetSentDownRequest.end(); )
    {
        ListElement * next = i->next;
        if (((Message *)i)->sink)
            ((Message *)i)->sink->messageFailed(((Message *)i), &nakUndef);
        i = next;
    }
    if (!notYetSentDownRequest.isEmpty())
    {

        for (ListElement * i = notYetSentDownRequest.begin(); i!=notYetSentDownRequest.end(); )
        {
            ListElement * next = i->next;
            DP_PRINTF(DP_WARNING, "Down request message type 0x%x client is not cleaning up.", ((Message *)i)->requestIdentifier);
            i = next;
        }
    }

    for (ListElement * i = notYetSentUpReply.begin(); i!=notYetSentUpReply.end();)
    {
        ListElement * next = i->next;
        if (((Message *)i)->sink)
            ((Message *)i)->sink->messageFailed(((Message *)i), &nakUndef);
        i = next;
    }
    if (!notYetSentUpReply.isEmpty())
    {

        for (ListElement * i = notYetSentUpReply.begin(); i!=notYetSentUpReply.end(); )
        {
            ListElement * next = i->next;
            DP_PRINTF(DP_WARNING, "Up reply message type 0x%x client is not cleaning up.", ((Message *)i)->requestIdentifier);
            i = next;
        }
    }

    for (ListElement * i = awaitingReplyDownRequest.begin(); i!=awaitingReplyDownRequest.end(); )
    {
        ListElement * next = i->next;
        if (((Message *)i)->sink)
            ((Message *)i)->sink->messageFailed(((Message *)i), &nakUndef);
        i = next;
    }
    if (!awaitingReplyDownRequest.isEmpty())
    {

        for (ListElement * i = awaitingReplyDownRequest.begin(); i!=awaitingReplyDownRequest.end(); )
        {
            ListElement * next = i->next;
            DP_PRINTF(DP_WARNING, "Down request message type 0x%x client is not cleaning up.", ((Message *)i)->requestIdentifier);
            i = next;
        }
    }

    // Do not reclaim the memory of our registered receivers
    while (!messageReceivers.isEmpty())
        messageReceivers.remove(messageReceivers.front());
}

ParseResponseStatus MessageManager::Message::parseResponse(EncodedMessage * message)
{
    BitStreamReader reader(&message->buffer, 0, message->buffer.length*8);

    // Read ReplyType
    bool replyNacked =  !!reader.readOrDefault(1, true);

    // Read RequestIdentifier
    unsigned requestId = reader.readOrDefault(7, 0);
    if (requestId != requestIdentifier)
    {
        DP_PRINTF(DP_NOTICE, "DP-MM> Requested = %x Received = %x", requestId, requestIdentifier);
        DP_ASSERT(0 && "Reply type doesn't match");
        return ParseResponseWrong;
    }

    if (replyNacked)
    {
        NakData nakData;

        // failure handler will parse the NAK response and do the required action
        if (DisplayPort::extractGUID(&reader, &nakData.guid) == false)
        {
            DP_ASSERT(0 && "Invalid GUID in NAK");
        }

        nakData.reason = (NakReason)reader.readOrDefault(8, 0);
        nakData.nak_data = reader.readOrDefault(8, 0);

        // call specific handler after parsing.
        parent->timer->cancelCallbacks(this);

        MessageManager * parent = this->parent;

        if (sink)
            sink->messageFailed(this, &nakData);

        parent->transmitAwaitingDownRequests();

        return ParseResponseSuccess;
    }

    ParseResponseStatus parseResult = parseResponseAck(message, &reader);

    if (parseResult == ParseResponseSuccess)
    {
        parent->timer->cancelCallbacks(this);

        if (this->sink)
        {
            MessageEventSink * msgSink = this->sink;
            msgSink->messageCompleted(this);
        }
    }

    return parseResult;
}

void MessageManager::Message::MessageEventSink::messageFailed(Message * from, NakData * nakData)
{

}

void MessageManager::registerReceiver(MessageReceiver * receiver)
{
    messageReceivers.insertBack(receiver);
}


bool MessageManager::MessageReceiver::process(EncodedMessage * message)
{
    BitStreamReader reader(&message->buffer, 0, message->buffer.length*8);

    // Read RequestIdentifier
    reader.readOrDefault(1, 0);
    unsigned reqId = reader.readOrDefault(7, 0);

    if (reqId != this->getRequestId())
    {
        //
        // This receiver is not meant for this message;
        // let the next in the queue handle it.
        //
        return false;
    }

    this->address = message->address;

    // processByType should parse the request, create a response and queue it if needed
    bool status = processByType(message, &reader);
    if (!status)
    {
        //
        // if we are here; we could get a receiver to handle the request
        // but something else went wrong.
        //
        DP_ASSERT(0);
    }

    return  true;
}
