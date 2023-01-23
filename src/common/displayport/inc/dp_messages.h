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
* Module: dp_messages.h                                                     *
*    Encoding routines for aux common messages.                             *
*                                                                           *
\***************************************************************************/
#ifndef INCLUDED_DP_MESSAGES_H
#define INCLUDED_DP_MESSAGES_H

#include "dp_address.h"
#include "dp_bitstream.h"
#include "dp_splitter.h"
#include "dp_merger.h"
#include "dp_crc.h"
#include "dp_list.h"
#include "dp_connector.h"
#include "dp_messageheader.h"
#include "dp_auxdefs.h"

namespace DisplayPort
{
    bool extractGUID(BitStreamReader * reader, GUID * guid);

    typedef enum
    {
        NakUndefined,
        NakWriteFailure,
        NakInvalidRAD,
        NakCrcFailure,
        NakBadParam,
        NakDefer,
        NakLinkFailure,
        NakNoResources,
        NakDpcdFail,
        NakI2cNak,
        NakAllocateFail,

        // Extensions
        NakTimeout = 0x100             // Message was unable to be transmitted

    } NakReason;

    typedef struct
    {
        GUID        guid;
        NakReason   reason;
        unsigned    nak_data;
    } NakData;

    typedef enum
    {
        ParseResponseSuccess,
        ParseResponseFailed,
        ParseResponseWrong
    } ParseResponseStatus;

    //
    // Priority levels are defined to prioritize SBMs for DP1.4 (Highest Priority - LEVEL1, Lowest Priority - DEFAULT)
    // Current implementation has the following priority levels
    //   CLEAR_PAYLOAD_ID_TABLE                =  NV_DP_SBMSG_PRIORITY_LEVEL_1
    //   LINK_ADDRESS                          =  NV_DP_SBMSG_PRIORITY_LEVEL_2
    //   REMOTE_DPCD_READ, REMOTE_DPCD_WRITE   =  NV_DP_SBMSG_PRIORITY_LEVEL_3
    //   REMOTE_I2C_READ, REMOTE_I2C_WRITE     =  NV_DP_SBMSG_PRIORITY_LEVEL_3
    //   POWER_UP_PHY, POWER_DOWN_PHY          =  NV_DP_SBMSG_PRIORITY_LEVEL_3
    //   ENUM_PATH_RESOURCES, ALLOCATE_PAYLOAD =  NV_DP_SBMSG_PRIORITY_LEVEL_4
    //   All other messages                    =  NV_DP_SBMSG_PRIORITY_LEVEL_DEFAULT
    //
    // However, Message::setMessagePriority can be used to override this priority levels, if required.
    //
    typedef enum
    {
        NV_DP_SBMSG_PRIORITY_LEVEL_DEFAULT,
        NV_DP_SBMSG_PRIORITY_LEVEL_4,
        NV_DP_SBMSG_PRIORITY_LEVEL_3,
        NV_DP_SBMSG_PRIORITY_LEVEL_2,
        NV_DP_SBMSG_PRIORITY_LEVEL_1,
    } DPSideBandMessagePriority;

    //
    //  CLASS: MessageManager
    //
    class MessageManager :
        virtual public Object,
        IncomingTransactionManager::IncomingTransactionManagerEventSink
    {

        Timer            *  timer;
        DPCDHAL          *  hal;
        DownRequestManager  splitterDownRequest;
        UpReplyManager      splitterUpReply;
        UpRequestManager    mergerUpRequest;
        DownReplyManager    mergerDownReply;
        bool                isBeingDestroyed;
        bool                isPaused;

        List                messageReceivers;
        List                notYetSentDownRequest;    // Down Messages yet to be processed
        List                notYetSentUpReply;        // Up Reply Messages yet to be processed
        List                awaitingReplyDownRequest; // Transmitted, Split, but not yet replied to

        void onUpRequestReceived(bool status, EncodedMessage * message);
        void onDownReplyReceived(bool status, EncodedMessage * message);
        void transmitAwaitingDownRequests();
        void transmitAwaitingUpReplies();

        // IncomingTransactionManager
        void messagedReceived(IncomingTransactionManager * from, EncodedMessage * message);

    public:
        class Message;
        void cancelAllByType(unsigned type);
        void cancelAll(Message * message);

        void pause()
        {
            isPaused = true;
        }

        void clearPendingMsg()
        {
            hal->clearPendingMsg();
        }
        void IRQUpReqest()
        {
            mergerUpRequest.mailboxInterrupt();
        }

        void IRQDownReply()
        {
            mergerDownReply.mailboxInterrupt();
        }

        MessageManager(DPCDHAL * hal, Timer * timer)
          : timer(timer), hal(hal),
            splitterDownRequest(hal, timer),
            splitterUpReply(hal, timer),
            mergerUpRequest(hal, timer, Address(0), this),
            mergerDownReply(hal, timer, Address(0), this),
            isBeingDestroyed(false)
        {
        }

        //
        //  CLASS: MessageReceiver
        //
        class MessageReceiver : public ListElement, OutgoingTransactionManager::OutgoingTransactionManagerEventSink
        {
        public:
            class MessageReceiverEventSink
            {
            public:
                virtual void messageProcessed(MessageReceiver * from) = 0;
            };

            // Returns false if the message should be passed to the next receiver
            virtual bool process(EncodedMessage * message);

            // per message type should implement this
            virtual bool processByType(EncodedMessage * message, BitStreamReader * reader) = 0;

            unsigned getRequestId() {return requestId;}
            Address & getAddress() {return address;}

            MessageReceiver(MessageReceiverEventSink*  sink, unsigned requestId)
                : sink(sink),
                requestId(requestId),
                bProcessed(true),
                address(0)  // 0 to start with
            {}

            virtual void splitterFailed(OutgoingTransactionManager * from)
            {
                DP_ASSERT(0 && "why did we send a reply");
            }

            virtual void splitterTransmitted(OutgoingTransactionManager * from)
            {
                DP_ASSERT(0 && "why did we send a reply");
            }

        protected:
            MessageReceiverEventSink *  sink;
            unsigned                    requestId;
            bool                        bProcessed;
            Address                     address;
            MessageManager *            parent;

        };

        //
        //  CLASS: Message
        //
        class Message : public ListElement,
                        OutgoingTransactionManager::OutgoingTransactionManagerEventSink,
                        Timer::TimerCallback /* countdown timer for reply */
        {
        public:
            class MessageEventSink
            {
            public:
                virtual void messageFailed(Message * from, NakData * nakData) = 0;
                virtual void messageCompleted(Message * from) = 0;
            };
            unsigned getMsgType() {return requestIdentifier;}
            unsigned getSinkPort() {return sinkPort;}
        protected:
            // Encoded message body (set in dp_messagecodings)
            //   this data structure is invalidated on post
            //   as the data gets swapped into the transmit buffer.
            EncodedMessage encodedMessage;
            MessageEventSink * sink;

            MessageManager * parent;
            bool             transmitReply;
            bool             bTransmitted;
            bool             bBusyWaiting;
            unsigned         requestIdentifier;
            unsigned         messagePriority;
            unsigned         sinkPort;

            // State updated by post operation
            struct {
                unsigned         messageNumber;
                Address          target;
            } state;

            virtual ParseResponseStatus parseResponseAck(
                        EncodedMessage * message, BitStreamReader * reader) = 0;
            virtual ParseResponseStatus parseResponse(EncodedMessage * message);
            virtual void splitterFailed(OutgoingTransactionManager * from);
            virtual void expired(const void * tag);
            virtual void splitterTransmitted(OutgoingTransactionManager * from);

        public:
            friend class MessageManager;

            Message(int requestIdentifier, int messagePriority)
                : sink(0),
                  parent(0),
                  transmitReply(false),
                  bTransmitted(false),
                  bBusyWaiting(false),
                  requestIdentifier(requestIdentifier),
                  messagePriority(messagePriority),
                  sinkPort(0xFF)
            {
            }

            void clear()
            {
                if (parent) {
                    parent->timer->cancelCallbacks(this);
                    parent->splitterDownRequest.cancel(this);
                }

                parent = 0;
                List::remove(this);
                encodedMessage.buffer.reset();
            }

            // This function can be used to override the already set priority of the message from it's constructor.
            void setMessagePriority(DPSideBandMessagePriority priorityLevel)
            {
                this->messagePriority = priorityLevel;
                return;
            }

        protected:
            ~Message()
            {
                clear();
            }
        };

        //
        // Register new receiver for unpair messages
        // (eg. broadcast messages or sink->source messages)
        //
        void registerReceiver(MessageReceiver * receiver);

        // Post a message to be asynchronously transmitted
        void post(Message * message, Message::MessageEventSink * sink, bool isReply = false);
        void postReply(Message * message, Message::MessageEventSink * sink);
        void cancel(Message * message);

        bool send(Message * message, NakData & nakData);
        friend class Message;
        ~MessageManager();
    };
    struct GenericMessageCompletion : public MessageManager::Message::MessageEventSink
    {
        bool    failed;
        bool    completed;
        NakData nakData;
        GenericMessageCompletion();
        void messageFailed(MessageManager::Message * from, NakData * data);
        void messageCompleted(MessageManager::Message * from);
    };
}

#endif //INCLUDED_DP_MESSAGES_H
