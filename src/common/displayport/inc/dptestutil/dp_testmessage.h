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

/******************************* DisplayPort*********************************\
*                                                                           *
* Module: dp_testmessage.h                                                  *
*                                                                           *
\***************************************************************************/
#ifndef INCLUDED_DP_TESTMESSAGE_H
#define INCLUDED_DP_TESTMESSAGE_H

#include "dp_auxdefs.h"

#define DP_TESTMESSAGE_QSES 0x38
#include "dp_qse.h"

#include "dp_connector.h"

#define DP_LPRIME_SIZE 20
namespace DisplayPort
{
    // test request status, for DP and nvapi
    typedef enum
    {
        DP_TESTMESSAGE_REQUEST_STATUS_PENDING = 0,          // the request is still be processing
        DP_TESTMESSAGE_REQUEST_STATUS_DONE = 1,             // request has been processed
        DP_TESTMESSAGE_REQUEST_STATUS_ERROR = 2,            // error, Dp lib busy with other request
        DP_TESTMESSAGE_REQUEST_STATUS_NEWREQUEST = 3,       // new request for user
    } DP_TESTMESSAGE_REQUEST_STATUS;

    // Request type enum.
    typedef enum
    {
        DP_TESTMESSAGE_REQUEST_TYPE_QSES,                   // TestMessage from DPTestUtil.
    } DP_TESTMESSAGE_REQUEST_TYPE;

    //
    // NVAPI QSES reply message struct.
    // Do NOT inherit any class, need keep consist with definition with Nvapi part,
    // which is C STRUCT
    //
    typedef struct
    {
        StreamState     streamState;
        bool            repeaterFuncPresent;
        bool            encryption;
        bool            authentication;
        OutputSinkType  sinkType;
        OutputCPType    cpType;
        bool            signedLPrime;
        NvU8            streamId;
        NvU8            LPrime[DP_LPRIME_SIZE];
    } DP_TESTMESSAGE_REQUEST_QSES_OUTPUT;

    //
    // Version of QSES_OUTPUT that consistent with struct in dp_messageencodings.h
    // ( without QSES Lprime).
    //
    // Considering nvapi backward compatibility, don't modify DP_TESTMESSAGE_REQUEST_QSES_OUTPUT
    // definition but has internal version to sync up with dplib implementation.
    //
    // DPLib message implementation is using this version for now. TestMessage
    // need this structure to safely copy info from QSES message structure.
    //
    typedef struct
    {
        StreamState     streamState;
        bool            repeaterFuncPresent;
        bool            encryption;
        bool            authentication;
        OutputSinkType  sinkType;
        OutputCPType    cpType;
        bool            signedLPrime;
        NvU8            streamId;
    } DP_TESTMESSAGE_REQUEST_QSES_OUTPUT_V2;

    typedef struct
    {
        // indicated what status to get, for DP, user need fill this
        DP_TESTMESSAGE_REQUEST_TYPE         requestType;
        // stream id for QSES to get, user need file this
        NvU32                               streamID;
        // replay buffer
        DP_TESTMESSAGE_REQUEST_QSES_OUTPUT  reply;
    } DP_TESTMESSAGE_REQUEST_QSES_INPUT;

    class TestMessage;
    struct ConnectorImpl;

    struct DPTestMessageCompletion : public MessageManager::Message::MessageEventSink
    {
        TestMessage *parent;

    public:
        void setParent(TestMessage *parent)
        {
            this->parent = parent;
        }
        // call back function if message fails, the status of the dp lib(testMessageStatus)
        // need to be set to DONE
        void messageFailed(MessageManager::Message * from, NakData * data);

        // call back function if message complete, the status of the dp lib(testMessageStatus)
        // need to be set to DONE.
        // If a message has a reply, it is necessary to record the reply in the dp lib to
        // send back to user later
        void messageCompleted(MessageManager::Message * from);

    };

    class TestMessage : virtual public Object
    {
    private:
        ConnectorImpl  *pConnector;
        // check if the user provided request struct is of valid size
        inline bool isValidStruct(DP_TESTMESSAGE_REQUEST_TYPE requestType, NvU32 structSize)
        {
            switch (requestType)
            {
                case DP_TESTMESSAGE_REQUEST_TYPE_QSES:
                {
                    return structSize == sizeof(DP_TESTMESSAGE_REQUEST_QSES_INPUT) ? true : false;
                }
                default:
                    return false;
            }
        }
        MessageManager *pMsgManager;
        DPTestMessageCompletion diagCompl;

        // Data Structure for Generic Message.
        NvU32 replyBytes;
        void  sendTestMsgQSES(void *pBuffer);

    public:
        DP_TESTMESSAGE_REQUEST_QSES_OUTPUT_V2  qsesReply;

        DP_TESTMESSAGE_REQUEST_STATUS       testMessageStatus;

        TestMessage() : testMessageStatus(DP_TESTMESSAGE_REQUEST_STATUS_DONE)
        {
            diagCompl.setParent(this);
            pConnector = 0;
            pMsgManager = 0;
            replyBytes = 0;
            qsesReply.streamState = DoesNotExist;
            qsesReply.repeaterFuncPresent = 0;
            qsesReply.encryption = 0;
            qsesReply.authentication = 0;
            qsesReply.sinkType = StreamUnconnected;
            qsesReply.cpType = HDCP1x;
            qsesReply.signedLPrime = 0;
            qsesReply.streamId = '\0';
        }
        DP_TESTMESSAGE_STATUS sendDPTestMessage(void    *pBuffer,
                                                NvU32    requestSize,
                                                NvU32   *pDpStatus);
        MessageManager * getMessageManager();
        void setupTestMessage(MessageManager *msgManager, ConnectorImpl *connector)
        {
            pMsgManager = msgManager;
            pConnector  = connector;
        }

    };
}


#endif

