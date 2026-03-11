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
* Module: dp_qse.cpp                                                        *
*    The DP HDCP Query Stream Encryption.                                   *
*                                                                           *
\***************************************************************************/

#include "dp_auxdefs.h"

#include "dp_qse.h"
#include "dp_internal.h"
#include "dp_deviceimpl.h"
#include "dp_connectorimpl.h"
#include "dp_printf.h"
#include <nvmisc.h>

using namespace DisplayPort;

NvU64 
QSENonceGenerator::random()
{
    NvU64 randomNumber;

    previousRandomLSB = static_cast<NvU32>(((NvU64)1664525 * previousRandomLSB + 1013904223));
    previousRandomMSB = static_cast<NvU32>(((NvU64)1664525 * previousRandomMSB + 1013904223));

    randomNumber = ((NvU64)previousRandomMSB << 32) | previousRandomLSB ;

    return randomNumber;
}

void 
QSENonceGenerator::clientIdBuilder
(
    NvU64 aN
)
{
    previousRandomMSB =  (NvU32)(aN >> 32) ;
    previousRandomLSB = (NvU32)(aN & 0xFFFFFFFF);
}

void 
QSENonceGenerator::makeClientId
(
    CLIENTID &clientId
)
{
    // Generate 56 bit nonce
    NvU64 rnd = random();

    clientId.data[0] = static_cast<NvU8>( rnd & 0xFF);
    clientId.data[1] = static_cast<NvU8>((rnd >> 8)  & 0xFF);
    clientId.data[2] = static_cast<NvU8>((rnd >> 16) & 0xFF);
    clientId.data[3] = static_cast<NvU8>((rnd >> 24) & 0xFF);
    clientId.data[4] = static_cast<NvU8>((rnd >> 32) & 0xFF);
    clientId.data[5] = static_cast<NvU8>((rnd >> 40) & 0xFF);
    clientId.data[6] = static_cast<NvU8>((rnd >> 48) & 0xFF);
}

StreamEncryptionStatusDetection::~StreamEncryptionStatusDetection()
{
    connector->timer->cancelCallbacks(this);
}

void 
StreamEncryptionStatusDetection::messageFailed
(
    MessageManager::Message *from,
    NakData *nakData
)
{
    if (from == &qseMessage)
    {
        connector->messageManager->clearAwaitingQSEReplyDownRequest();

        if ((retriesSendQSEMessage < DPCD_QUERY_STREAM_MESSAGE_RETRIES) &&
            (nakData->reason == NakDefer || nakData->reason == NakTimeout))
        {
            connector->timer->cancelCallback(parent, &(parent->tagStreamValidation));
            retriesSendQSEMessage++;
            sendQSEMessage(parent);
            connector->timer->queueCallback(parent, &(parent->tagStreamValidation), HDCP_STREAM_VALIDATION_REQUEST_COOLDOWN);
            return;
        }
        //
        // If message failed is called after all retries have expired then
        // we should disable the HDCP.
        //
        else
        {
             DP_PRINTF(DP_ERROR, "DP-QSE> Downstream failed to handle %s QSES message",
                       reason == qseReason_Ssc ? "SSC" : "generic");
             //
             // Non-QSE supported branch would get HDCP off if we honor QSES's result even w/o SSC from it.
             // So to improve compatibility, we honor QSES's result to have HDCP off only if it's fired for SSC.
             //
             if (reason == qseReason_Ssc)
             {
                 for (ListElement * i = connector->activeGroups.begin(); i != connector->activeGroups.end(); i = i->next)
                 {
                     GroupImpl * group = (GroupImpl *)i;
                     if (group->hdcpEnabled)
                     {
                         //
                         // In case of MST, time slots will be deleted and add back for clearing ECF
                         // This will lead to blank screen momentarily
                         // Similarly for all other QSES errors
                         // 
                         group->hdcpSetEncrypted(false, NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_TYPE_0, NV_TRUE, NV_TRUE);
                     }
                 }
                 connector->main->configureHDCPAbortAuthentication(KSV_SIG);
                 connector->main->configureHDCPDisableAuthentication();
                 // Clear HDCP cap for groups and connector and devices.
                 connector->isHDCPAuthOn = false;
             }
             else
             {
                 connector->bIsEncryptionQseValid = false;
             }

             retriesSendQSEMessage = 0;
             parent->streamValidationDone = true;

             //Reset the MessageManager pointer state
             resetQseMessageState();
        }
    }
}

void 
StreamEncryptionStatusDetection::expired(
    const void * tag
)
{
   // Not required as of now.
}

void 
StreamEncryptionStatusDetection::handleQSEDownReply()
{
    if ((connector->bValidQSERequest) && (handleQSEReplyValidation()))
    {
        parent->streamValidationDone = true;
    }
    else
    {
        connector->bValidQSERequest = true;
        parent->streamValidationDone = true;
    }
}

void 
StreamEncryptionStatusDetection::messageCompleted
(
    MessageManager::Message *from
)
{
    if (from == &qseMessage)
    {
        handleQSEDownReply();

        //Reset the MessageManager pointer state
        resetQseMessageState();
    }
}

void 
StreamEncryptionStatusDetection::sendQSEMessage
(
    GroupImpl *group,
    QSE_REASON reasonId
)
{
    Address address(0);
    CLIENTID clientId;
    HDCPState hdcpState = {0};

    // Get hdcp version to see if hdcp22 QSE or not.
    connector->main->configureHDCPGetHDCPState(hdcpState);
    setHdcp22Qse(hdcpState.HDCP_State_22_Capable);

    // Check whether repeater or not. 
    bIsRepeater = hdcpState.HDCP_State_Repeater_Capable;

    //Generate the Pseudo Random number
    connector->qseNonceGenerator->makeClientId(clientId);
    for (unsigned i = 0 ; i < CLIENT_ID_SIZE; i++)
    {
       group->clientId[i] = clientId.data[i];
    }
    this->reason = reasonId;
    group->streamValidationDone = false;
    qseMessage.set( address,
                    group->streamIndex,
                    clientId.data,
                    CP_IRQ_ON,
                    STREAM_EVENT_MASK_ON,
                    Force_Reauth,
                    STREAM_BEHAVIOUR_MASK_ON);
    connector->messageManager->post(&qseMessage, this);
}

bool 
StreamEncryptionStatusDetection::handleQSEReplyValidation()
{
    if (parent->streamIndex != qseMessage.getStreamId())
    {
        DP_PRINTF(DP_ERROR, "DP-QSE> Query the active Stream ID %d, but reply Stream ID %d mismatch.", parent->streamIndex, qseMessage.getStreamId());
        parent->hdcpSetEncrypted(false, NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_TYPE_0, NV_TRUE, NV_TRUE);
        return false;
    }

    NvU16 streamStatus = 0;
    streamStatus = qseMessage.getStreamStatus();
    DP_PRINTF(DP_NOTICE, "DP-QSE> Query the active Stream ID %d. The reply streamStatus: %d", parent->streamIndex, streamStatus);

    NvU16 streamState = DRF_VAL(_DP, _HDCP, _STREAM_STATE, streamStatus);
    if ((streamState == NV_DP_HDCP_STREAM_STATE_NO_EXIST) ||
        (streamState == NV_DP_HDCP_STREAM_STATE_ERROR))
    {
        DP_PRINTF(DP_ERROR, "DP-QSE> Query the active Stream ID %d, but reply as Stream does not exist or Error/Reserved", parent->streamIndex);
        parent->hdcpSetEncrypted(false, NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_TYPE_0, NV_TRUE, NV_TRUE);
        return false;
    }
    else if (streamState == NV_DP_HDCP_STREAM_STATE_NOT_ACTIVE)
    {
        DP_PRINTF(DP_ERROR, "DP-QSE> Query the active Stream ID %d, but reply as Stream not active", parent->streamIndex);
        parent->hdcpSetEncrypted(false, NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_TYPE_0, NV_TRUE, NV_TRUE);
        return false;
    }

    NvU16 streamAuth = DRF_VAL(_DP, _HDCP, _STREAM_AUTHENTICATION, streamStatus);
    if (streamAuth == NV_DP_HDCP_STREAM_AUTHENTICATION_OFF)
    {
        DP_PRINTF(DP_ERROR, "DP-QSE> Query the Stream ID %d, reply as failed authentication all the way down", parent->streamIndex);

        parent->hdcpSetEncrypted(false, NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_TYPE_0, NV_TRUE, NV_TRUE);
        return false;
    }

    // Watch here for not over reacting encryption policy here.
    NvU16 streamEncrypt = DRF_VAL(_DP, _HDCP, _STREAM_ENCRYPTION, streamStatus);
    if (streamEncrypt == NV_DP_HDCP_STREAM_ENCRYPTION_OFF)
    {
        if (parent->hdcpEnabled)
        {
            DP_PRINTF(DP_ERROR, "DP-QSE> Query the Stream ID %d, reply as not encryption all the way down", parent->streamIndex);
            parent->qseEncryptionStatusMismatch = parent->hdcpEnabled;
            parent->hdcpSetEncrypted(false, NV0073_CTRL_SPECIFIC_HDCP_CTRL_HDCP22_TYPE_0, NV_TRUE, NV_TRUE);
        }
        else
            return false;
    }

    return true;
}

void 
StreamEncryptionStatusDetection::resetQseMessageState()
{
    qseMessage.clear();
}

void
StreamEncryptionStatusDetection::setHdcp22Qse(bool bHdcp22Qse)
{
    bIsHdcp22Qse = bHdcp22Qse;
    qseMessage.setHdcp22Qse(bHdcp22Qse);
}
