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
* Module: dp_messageheader.h                                                *
*    DP message header parser                                               *
*                                                                           *
\***************************************************************************/

#ifndef INCLUDED_DP_MESSAGEHEADER_H
#define INCLUDED_DP_MESSAGEHEADER_H

#include "dp_internal.h"
#include "dp_list.h"
#include "dp_auxretry.h"
#include "dp_timer.h"
#include "dp_bitstream.h"
#include "dp_address.h"

namespace DisplayPort
{
    //
    //  User filled message structure
    //
    #define MAX_MESSAGE_SIZE 64
    struct EncodedMessage : public Object
    {
        unsigned messageNumber;            // 0 or 1
        Address  address;                  // target device for message (source for reply)
        Buffer   buffer;
        bool     isBroadcast;
        bool     isPathMessage;

        EncodedMessage()
            : messageNumber(0), isBroadcast(false), isPathMessage(false)
        {}

        void swap(EncodedMessage & other)
        {
            swap_args(messageNumber, other.messageNumber);
            swap_args(address, other.address);
            swap_args(isBroadcast, other.isBroadcast);
            swap_args(isPathMessage, other.isPathMessage);
            buffer.swap(other.buffer);
        }
    };

    //
    //  Decoded message header
    //
    struct MessageHeader
    {
        Address address;
        unsigned messageNumber; 
        unsigned payloadBytes; 
        bool isBroadcast;
        bool isPathMessage;
        bool isTransactionStart;
        bool isTransactionEnd;
        unsigned headerSizeBits;
    };

    bool decodeHeader(BitStreamReader * reader, MessageHeader * header, const Address & address);

    //
    //  Routines for maintaining a list of partially complete messages
    //

    // after 4 secs delete dead transactions
    #define DP_INCOMPLETE_MESSAGE_TIMEOUT_USEC 4000000

}
#endif //INCLUDED_DP_MESSAGEHEADER_H
