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
    } DP_TESTMESSAGE_REQUEST_TYPE;

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
                default:
                    return false;
            }
        }
        MessageManager *pMsgManager;
        DPTestMessageCompletion diagCompl;

        // Data Structure for Generic Message.
        NvU32 replyBytes;

    public:

        DP_TESTMESSAGE_REQUEST_STATUS       testMessageStatus;

        TestMessage() : testMessageStatus(DP_TESTMESSAGE_REQUEST_STATUS_DONE)
        {
            diagCompl.setParent(this);
            pConnector = 0;
            pMsgManager = 0;
            replyBytes = 0;
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

