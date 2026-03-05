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
*    Module: dp_testmessage.cpp                                               *
*    Used for DP Test Utility                                               *
*                                                                           *
\***************************************************************************/
#include "dp_internal.h"
#include "dp_auxdefs.h"
#include "dp_messages.h"
#include "dp_testmessage.h"
#include "dp_connectorimpl.h"
using namespace DisplayPort;

// the dp lib status must be set to DONE indicating there is no pending message
void DPTestMessageCompletion::messageFailed(MessageManager::Message * from, NakData * data)
{
    parent->testMessageStatus = DP_TESTMESSAGE_REQUEST_STATUS_DONE;

    if (from->getMsgType() == NV_DP_SBMSG_REQUEST_ID_QUERY_STREAM_ENCRYPTION_STATUS)
    {
        delete (QueryStreamEncryptionMessage *)from;
    }
    else 
    {
        {
            DP_ASSERT(0 && "unknown msg type when msg failed");
        }
    }
}

void DPTestMessageCompletion::messageCompleted(MessageManager::Message * from)
{
    parent->testMessageStatus = DP_TESTMESSAGE_REQUEST_STATUS_DONE;

    if (from->getMsgType() == NV_DP_SBMSG_REQUEST_ID_QUERY_STREAM_ENCRYPTION_STATUS)
    {
        ((QueryStreamEncryptionMessage *)from)->getReply(&parent->qsesReply);
        delete (QueryStreamEncryptionMessage *)from;
    }
    else
    {
        {
            DP_ASSERT(0 && "unknown msg type when msg complete");
        }
    }
}

MessageManager * TestMessage::getMessageManager()
{
    return pMsgManager;
}

//pBuffer should point to a DP_TESTMESSAGE_REQUEST_QSES_INPUT structure
void TestMessage::sendTestMsgQSES(void *pBuffer)
{
    //Generate the Pseudo Random number
    QSENonceGenerator qseNonceGenerator;

    //for qses, send to the root branch
    Address address(0);
    CLIENTID clientId;
    QueryStreamEncryptionMessage *pQseMessage = new QueryStreamEncryptionMessage();

    DP_TESTMESSAGE_REQUEST_QSES_INPUT *pQSES =
                                (DP_TESTMESSAGE_REQUEST_QSES_INPUT *)pBuffer;

    pQseMessage->set(address,
                    pQSES->streamID,
                    clientId.data,
                    CP_IRQ_ON,
                    STREAM_EVENT_MASK_ON,
                    Force_Reauth,
                    STREAM_BEHAVIOUR_MASK_ON);

    pMsgManager->post(pQseMessage, &diagCompl);
}

//
//    The function request that the request struct size should be check first to ensure the right structure is used and
//    no BSOD will happen.
//
//    For each request type, the DP lib status for that type should be check in case of request conflict. At one time,
//    for each request type, only ONE instance could be processed
//
DP_TESTMESSAGE_STATUS TestMessage::sendDPTestMessage
(
    void    *pBuffer,
    NvU32    requestSize,
    NvU32   *pDpStatus
)
{
    DP_ASSERT(pBuffer);
    DP_TESTMESSAGE_REQUEST_TYPE type;

    // the buffer must contain a requestType field at least
    if (requestSize < sizeof(DP_TESTMESSAGE_REQUEST_TYPE))
        return DP_TESTMESSAGE_STATUS_ERROR_INVALID_PARAM;

    type = *(DP_TESTMESSAGE_REQUEST_TYPE *)pBuffer;

    if (!isValidStruct(type, requestSize))
        return DP_TESTMESSAGE_STATUS_ERROR_INVALID_PARAM;

    switch (type)
    {
        case DP_TESTMESSAGE_REQUEST_TYPE_QSES:
            // new request, try send message
            if (*pDpStatus == DP_TESTMESSAGE_REQUEST_STATUS_NEWREQUEST)
            {
                //there is still processing request, new one not allow now
                if (testMessageStatus == DP_TESTMESSAGE_REQUEST_STATUS_PENDING)
                {
                    *pDpStatus = DP_TESTMESSAGE_REQUEST_STATUS_ERROR;
                    return DP_TESTMESSAGE_STATUS_ERROR;
                }
                else
                {
                    sendTestMsgQSES(pBuffer);
                    //need change the DP lib status accordingly
                    *pDpStatus = DP_TESTMESSAGE_REQUEST_STATUS_PENDING;
                    testMessageStatus = DP_TESTMESSAGE_REQUEST_STATUS_PENDING;
                }
            }
            //old request, check if request finished
            else if(*pDpStatus == DP_TESTMESSAGE_REQUEST_STATUS_PENDING)
            {
                //already finished, fill in the data
                if (testMessageStatus == DP_TESTMESSAGE_REQUEST_STATUS_DONE)
                {
                    DP_TESTMESSAGE_REQUEST_QSES_INPUT *p =
                                  (DP_TESTMESSAGE_REQUEST_QSES_INPUT *)pBuffer;
                    p->reply = *(DP_TESTMESSAGE_REQUEST_QSES_OUTPUT *)&qsesReply;
                    *pDpStatus = DP_TESTMESSAGE_REQUEST_STATUS_DONE;
                }
                //otherwise, just return and ask the user try again
            }
            break;
        default:
            *pDpStatus = DP_TESTMESSAGE_REQUEST_STATUS_ERROR;
            return DP_TESTMESSAGE_STATUS_ERROR;
    }

    return DP_TESTMESSAGE_STATUS_SUCCESS;
}

