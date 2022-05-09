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

    {
        {
            DP_ASSERT(0 && "unknown msg type when msg failed");
        }
    }
}

void DPTestMessageCompletion::messageCompleted(MessageManager::Message * from)
{
    parent->testMessageStatus = DP_TESTMESSAGE_REQUEST_STATUS_DONE;

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

    *pDpStatus = DP_TESTMESSAGE_REQUEST_STATUS_ERROR;
    return DP_TESTMESSAGE_STATUS_ERROR;
}

