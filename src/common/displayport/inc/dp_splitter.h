/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
* Module: dp_splitter.h                                                     *
*    Asynchronous Message splitter                                          *
*                                                                           *
\***************************************************************************/
#ifndef INCLUDED_DP_SPLITTER_H
#define INCLUDED_DP_SPLITTER_H

#include "dp_list.h"
#include "dp_auxretry.h"
#include "dp_timer.h"
#include "dp_auxdefs.h"
#include "dp_messageheader.h"

namespace DisplayPort
{

    struct EncodedMessage;
    class DPCDHAL;

    class MessageTransactionSplitter
    {
        //
        // If set we've pulled an item out of the downQueue queue.
        // One or more transactions have been sent as a result
        // messageOutstanding->messageOffset show how far into
        // the message we are.
        //
        EncodedMessage * messageOutstanding;
        unsigned assemblyTransmitted;
    public:
        void set(EncodedMessage * messageOutstanding)
        {
            this->messageOutstanding = messageOutstanding;
            assemblyTransmitted = 0;
        }

        //
        // Encode the next transaction.
        //      returns false if there are no more transactions
        //
        bool get(Buffer & assemblyBuffer);

        MessageTransactionSplitter() : messageOutstanding(0), assemblyTransmitted(0)
        {}
    };

    class OutgoingTransactionManager:
        virtual public Object,
        private Timer::TimerCallback
    {
    public:
        class OutgoingTransactionManagerEventSink
        {
        public:
            virtual void splitterFailed(OutgoingTransactionManager * from) = 0;            // Sink DEFER the writes
            virtual void splitterTransmitted(OutgoingTransactionManager * from) = 0;       // message was sent (may NACK later)
        };

        // Send the encoded message.  This call is destructive to the EncodedMessage
        // passed in
        bool send( EncodedMessage & payload, OutgoingTransactionManagerEventSink * sink);

        OutgoingTransactionManager(Timer * timer);
        virtual ~OutgoingTransactionManager() { timer->cancelCallbacks(this); }

        // Do not make any calls to the event sink
        void cancel(OutgoingTransactionManagerEventSink * sink);

    protected:
        virtual AuxRetry::status writeMessageBox(NvU8 * data, size_t length) = 0;
        virtual size_t           getMessageBoxSize() = 0;
    private:
        void writeToWindow( bool firstAttempt);
        void split();
        void expired(const void * tag); // timer callback

        unsigned retriesLeft;

        Buffer                      assemblyBuffer;
        MessageTransactionSplitter  transactionSplitter;

        //
        //  List of outgoing messages
        //
        struct OutgoingMessage : ListElement
        {
            OutgoingTransactionManagerEventSink*  eventSink;
            EncodedMessage                        message;
        };

        List                  queuedMessages;

        //
        //  Message currently assembled in transactionSplitter
        //      (if any)
        //
        OutgoingMessage     * activeMessage;
        Timer   *  timer;
    };


    class DownRequestManager : public OutgoingTransactionManager
    {
    public:
        DownRequestManager(DPCDHAL * hal, Timer * timer)
            : OutgoingTransactionManager(timer), hal(hal)
        {
        }

        virtual ~DownRequestManager() {}
    protected:
        DPCDHAL * hal;

        virtual AuxRetry::status writeMessageBox(NvU8 * data, size_t length);
        virtual size_t           getMessageBoxSize();
    };

    class UpReplyManager : public OutgoingTransactionManager
    {
    public:
        UpReplyManager(DPCDHAL * hal, Timer * timer)
            :  OutgoingTransactionManager(timer), hal(hal)
        {
        }
        virtual ~UpReplyManager() {}
    protected:
        DPCDHAL * hal;

        virtual AuxRetry::status writeMessageBox(NvU8 * data, size_t length);
        virtual size_t           getMessageBoxSize();
    };
}

#endif //INCLUDED_DP_SPLITTER_H
