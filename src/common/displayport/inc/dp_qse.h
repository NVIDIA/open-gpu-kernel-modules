/*
 * SPDX-FileCopyrightText: Copyright (c) 2010-2020 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
* Module: dp_qse.h                                                          *
*    Class definition for HDCP Query Stream Encryption and relative reading.*
*                                                                           *
\***************************************************************************/
#ifndef INCLUDED_DP_QSE_H
#define INCLUDED_DP_QSE_H

#include "dp_messagecodings.h"
#include "dp_auxdefs.h"

namespace DisplayPort
{
#define CLIENT_ID_SIZE  7


    struct CLIENTID
    {
        NvU8 data[CLIENT_ID_SIZE];

        CLIENTID()
        {
            dpMemZero(&data, sizeof(data));
        }
    };

    enum QSE_REASON
    {
        qseReason_Generic,
        qseReason_Ssc
    };

    class QSENonceGenerator: public Object 
    {
        NvU32 previousRandomLSB;
        NvU32 previousRandomMSB;
        //
        // Linear congruential random number generator
        // Seed values chosen from numerical methods
        //
        NvU64 random();

     public:
        QSENonceGenerator():previousRandomLSB(0),previousRandomMSB(0)
        {}

        void clientIdBuilder(NvU64 aN);
        // For every clientId generation we need to call makeClientId
        void makeClientId(CLIENTID & clientId);
    };

    struct GroupImpl;
    struct ConnectorImpl;

    class StreamEncryptionStatusDetection : public Object, public MessageManager::Message::MessageEventSink, Timer::TimerCallback
    {
        GroupImpl                * parent;
        ConnectorImpl            * connector;
        QueryStreamEncryptionMessage qseMessage;
        unsigned                 retriesSendQSEMessage;
        QSE_REASON               reason;
        bool                     bIsHdcp22Qse;
        bool                     bIsRepeater;

    public:
        StreamEncryptionStatusDetection(GroupImpl * parent, ConnectorImpl * connector): 
            parent(parent), connector(connector), retriesSendQSEMessage(0), bIsHdcp22Qse(false), bIsRepeater(false)
        {}

        ~StreamEncryptionStatusDetection();

        void sendQSEMessage(GroupImpl * group, QSE_REASON reasonId = qseReason_Generic);
        void handleQSEDownReply();
        void messageFailed(MessageManager::Message * from, NakData * nakData);
        void messageCompleted(MessageManager::Message * from);
        void expired(const void * tag);
        bool handleQSEReplyValidation();
        void resetQseMessageState();
        void setHdcp22Qse(bool bHdcp22Qse);
    };

    struct DeviceImpl;
}

#endif // INCLUDED_DP_QSE_H
