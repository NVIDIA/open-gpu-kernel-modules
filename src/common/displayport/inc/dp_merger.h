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
* Module: dp_merger.h                                                       *
*    Asynchronous Message merger                                            *
*                                                                           *
\***************************************************************************/

#ifndef INCLUDED_DP_MERGER_H
#define INCLUDED_DP_MERGER_H

#include "dp_list.h"
#include "dp_auxretry.h"
#include "dp_timer.h"
#include "dp_bitstream.h"
#include "dp_address.h"
#include "dp_messageheader.h"
#include "dp_configcaps.h"

namespace DisplayPort
{
    // after 4 secs delete dead transactions
    #define DP_INCOMPLETE_MESSAGE_TIMEOUT_USEC 4000000
    struct EncodedMessage;

    class  MessageTransactionMerger : virtual public Object
    {
        class  IncompleteMessage : public ListElement
        {
        public:
            EncodedMessage  message;
            NvU64           lastUpdated;

        };

        List    incompleteMessages;
        Timer * timer;
        NvU64   incompleteMessageTimeoutMs;
        IncompleteMessage * freeOnNextCall; // we don't need to delete it on destruct
                                            // since this is ALSO a member of the list we own

        IncompleteMessage * getTransactionRecord(const Address & address, unsigned messageNumber);
    public:
        MessageTransactionMerger(Timer * timer, unsigned incompleteMessageTimeoutMs)
            : timer(timer), incompleteMessageTimeoutMs(incompleteMessageTimeoutMs), freeOnNextCall(0)
        {
        }

        //
        //  Pushes data into the queue and returns an encoded
        //  message if an entire message is assembled.
        //
        EncodedMessage * pushTransaction(MessageHeader * header, Buffer * data);
    };

    class IncomingTransactionManager : virtual public Object
    {
    public:
        class IncomingTransactionManagerEventSink
        {
        public:
            virtual void messagedReceived(IncomingTransactionManager * from, EncodedMessage * message) = 0;
        };

        void mailboxInterrupt();
        
        //
        //  Create a message merger object
        //      - sink is called whenever a new message is received
        //         Callback::fired is passed an IncompleteMessage as the data arg.
        //
        IncomingTransactionManager(Timer * timerInterface, const Address & addressPrefix, IncomingTransactionManagerEventSink * sink);
        virtual ~IncomingTransactionManager();

    protected:
        virtual AuxRetry::status readMessageBox(NvU32 offset, NvU8 * data, size_t length) = 0;
        virtual size_t           getMessageBoxSize() = 0;
        virtual size_t           getTransactionSize() = 0;
        virtual void             clearMessageBoxInterrupt() = 0;
    private:
        MessageTransactionMerger     incompleteMessages;  // List<IncompleteMessage>
     
        Buffer                                 localWindow;
        Timer  *                               timer;
        IncomingTransactionManagerEventSink *  sink;   
        Address                                addressPrefix;       // This is the aux address of the downstream port
                                                        // This field will be prepended to the address decoded.                                        
    };

    class DownReplyManager : public IncomingTransactionManager
    {
    public:
        DownReplyManager(DPCDHAL * hal, Timer * timer, const Address & addressPrefix, IncomingTransactionManagerEventSink * sink)
            : IncomingTransactionManager(timer, addressPrefix, sink), hal(hal)
        {
        }
        virtual ~DownReplyManager() {}

    protected:
        DPCDHAL * hal;

        virtual AuxRetry::status readMessageBox(NvU32 offset, NvU8 * data, size_t length);
        virtual size_t           getMessageBoxSize();
        virtual size_t           getTransactionSize();
        virtual void             clearMessageBoxInterrupt();
    };

    class UpRequestManager : public IncomingTransactionManager
    {
    public:
        UpRequestManager(DPCDHAL * hal, Timer * timer, const Address & addressPrefix, IncomingTransactionManagerEventSink * sink)
            : IncomingTransactionManager(timer, addressPrefix, sink), hal(hal)
        {
        }
        virtual ~UpRequestManager() {}
    protected:
        DPCDHAL * hal;

        virtual AuxRetry::status readMessageBox(NvU32 offset, NvU8 * data, size_t length);
        virtual size_t           getMessageBoxSize();
        virtual size_t           getTransactionSize();
        virtual void             clearMessageBoxInterrupt();
    };
}

#endif //INCLUDED_DP_MERGER_H
