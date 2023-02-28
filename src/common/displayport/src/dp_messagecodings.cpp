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
* Module: dp_messagecodings.cpp                                             *
*    Encoding routines for various messages                                 *
*                                                                           *
\***************************************************************************/
#include "dp_internal.h"
#include "dp_messagecodings.h"
#include "dp_auxdefs.h"

using namespace DisplayPort;

//
// LINK_ADDRESS    0x1
//
void LinkAddressMessage::set(const Address & target)
{
    clear();
    BitStreamWriter writer(&encodedMessage.buffer, 0);

    //
    //    Write request identifier
    //
    writer.write(0 /*zero*/, 1);
    writer.write(requestIdentifier, 7);

    encodedMessage.isPathMessage = false;
    encodedMessage.isBroadcast  = false;
    encodedMessage.address = target;
}

ParseResponseStatus LinkAddressMessage::parseResponseAck(EncodedMessage * message, BitStreamReader * reader)
{
    DisplayPort::extractGUID(reader, &reply.guid);
    reader->readOrDefault(4 /*zeroes*/, 0);
    reply.numberOfPorts = reader->readOrDefault(4 /*Number_Of_Ports*/, 0xF);

    for (unsigned i = 0; i < reply.numberOfPorts; i++)
    {
        reply.res[i].isInputPort    = !!reader->readOrDefault(1 /*Input_Port*/, 1);
        reply.res[i].peerDeviceType = (PeerDevice) reader->readOrDefault(3 /*Peer_Device_Type*/, 0x0);
        reply.res[i].portNumber     = reader->readOrDefault(4 /*Port_Number*/, 0xF);
        reply.res[i].hasMessaging   = !!reader->readOrDefault(1 /*Messaging_Capability_Status*/, 0x1);
        reply.res[i].dpPlugged      = !!reader->readOrDefault(1 /*DisplayPort_Device_Plug_Status*/, 0x1);

        if (reply.res[i].isInputPort == false)
        {
            reply.res[i].legacyPlugged  = !!reader->readOrDefault(1 /*Legacy_Device_Plug_Status*/, 0x1);

            reader->readOrDefault(5 /*zeroes*/, 0x0);

            unsigned ver                = reader->readOrDefault(8/*DPCD_Revision*/, 0);
            reply.res[i].dpcdRevisionMajor = ver >> 4;
            reply.res[i].dpcdRevisionMinor = ver & 0xF;
            DisplayPort::extractGUID(reader, &reply.res[i].peerGUID);
            reply.res[i].SDPStreams     = reader->readOrDefault(4 /*Number_SDP_Streams*/, 0xF);
            reply.res[i].SDPStreamSinks = reader->readOrDefault(4 /*Number_SDP_Stream_Sinks*/, 0xF);
        }
        else
        {
            reader->readOrDefault(6 /*zeroes*/, 0x0);
        }
    }

    return ParseResponseSuccess;
}

//
// CONNECTION_STATUS_NOTIFY    0x2
//
ConnStatusNotifyMessage::ConnStatusNotifyMessage(MessageReceiverEventSink * sink)
: MessageReceiver(sink, NV_DP_SBMSG_REQUEST_ID_CONNECTION_STATUS_NOTIFY /*request id*/)
{
}

bool ConnStatusNotifyMessage::processByType(EncodedMessage * message, BitStreamReader * reader)
{
    // read the request body
    request.port = reader->readOrDefault(4/*Port_Number*/, 0xF);
    reader->readOrDefault(4/*zeroes*/, 0);
    bool status = DisplayPort::extractGUID(reader/*GUID of the originating branch device*/, &request.guid);
    reader->readOrDefault(1/*zero*/, 0);
    request.legacyPlugged = !!reader->readOrDefault(1/*Legacy_Device_Plug_Status*/, 0);
    request.devicePlugged = !!reader->readOrDefault(1/*DisplayPort_Device_Plug_Status*/, 0);
    request.messagingCapability = !!reader->readOrDefault(1/*Messaging_Capability_Status*/, 0);
    request.isInputPort = !!reader->readOrDefault(1/*Input_Port*/, 0);
    request.peerDeviceType = (PeerDevice) reader->readOrDefault(3/*Peer_Device_Type*/, 0);

    // action will be implemented by evensink
    this->sink->messageProcessed(this);
    return status;
}

//
// GENERIC_UP_REPLY   0xnn
//
void GenericUpReplyMessage::set(const Address & target,
                                bool bReplyIsNack,
                                bool bBroadcast,
                                bool bPath)
{
    clear();
    BitStreamWriter writer(&encodedMessage.buffer, 0);

    writer.write(bReplyIsNack?1:0, 1);
    writer.write(requestIdentifier, 7);

    encodedMessage.isPathMessage = bPath;
    encodedMessage.isBroadcast  = bBroadcast;
    encodedMessage.address = target;
}

GenericUpReplyMessage::GenericUpReplyMessage(unsigned requestId, bool bReplyIsNack, bool bBroadcast, bool bPath)
:  Message(requestId, NV_DP_SBMSG_PRIORITY_LEVEL_DEFAULT)
{
    BitStreamWriter writer(&encodedMessage.buffer, 0);

    //
    //    Write request identifier
    //
    writer.write(bReplyIsNack?1:0, 1);
    writer.write(requestId, 7);

    encodedMessage.isPathMessage = bPath;
    encodedMessage.isBroadcast  = bBroadcast;
}

GenericUpReplyMessage::GenericUpReplyMessage(const Address & target, unsigned requestId, bool bReplyIsNack, bool bBroadcast, bool bPath)
:  Message(requestId, NV_DP_SBMSG_PRIORITY_LEVEL_DEFAULT)
{
    BitStreamWriter writer(&encodedMessage.buffer, 0);

    //
    //    Write request identifier
    //
    writer.write(bReplyIsNack?1:0, 1);
    writer.write(requestId, 7);

    encodedMessage.isPathMessage = bPath;
    encodedMessage.isBroadcast  = bBroadcast;
    encodedMessage.address = target;
}

ParseResponseStatus GenericUpReplyMessage::parseResponseAck(EncodedMessage * message, BitStreamReader * reader)
{
    //
    // we are not expecting any replies here
    // Since the corresponding post for this kind of message is of reply type;
    // message manager won't queue an awaiting down reply for the same.
    //
    DP_ASSERT(0 && "We shouldn't be here!!");
    return ParseResponseSuccess;
}

//
// CLEAR_PAYLOAD_ID_TABLE 0x14
//
ClearPayloadIdTableMessage::ClearPayloadIdTableMessage()
: Message(NV_DP_SBMSG_REQUEST_ID_CLEAR_PAYLOAD_ID_TABLE /* request id */, NV_DP_SBMSG_PRIORITY_LEVEL_1)
{
    BitStreamWriter writer(&encodedMessage.buffer, 0);

    //    Write request identifier
    writer.write(0/*zero*/, 1);
    writer.write(requestIdentifier, 7);

    encodedMessage.isPathMessage = true;
    encodedMessage.isBroadcast  = true;
    encodedMessage.address = Address();
}

ParseResponseStatus ClearPayloadIdTableMessage::parseResponseAck(EncodedMessage * message, BitStreamReader * reader)
{
    return ParseResponseSuccess;
}

ParseResponseStatus ClearPayloadIdTableMessage::parseResponse(EncodedMessage * message)
{
    sink->messageCompleted(this);
    return ParseResponseSuccess;
}

//
// ENUM_PATH_RESOURCES  0x10
//
EnumPathResMessage::EnumPathResMessage(const Address & target, unsigned port, bool point)
: Message(NV_DP_SBMSG_REQUEST_ID_ENUM_PATH_RESOURCES /* request identifier */,
          NV_DP_SBMSG_PRIORITY_LEVEL_4)
{
    BitStreamWriter writer(&encodedMessage.buffer, 0);

    //    Write request identifier
    writer.write(0/*zereo*/, 1);
    writer.write(requestIdentifier, 7);
    writer.write(port, 4);
    writer.write(0/*zeroes*/, 4);

    encodedMessage.isPathMessage = !point;
    encodedMessage.isBroadcast  = false;
    encodedMessage.address = target;
    sinkPort = port;
    dpMemZero(&reply, sizeof(reply));
}

ParseResponseStatus EnumPathResMessage::parseResponseAck(EncodedMessage * message, BitStreamReader * reader)
{
    reply.portNumber        = reader->readOrDefault(4 /*Port_Number*/, 0xF);
    reply.availableStreams  = reader->readOrDefault(3 /*Available_Streams*/, 0);
    reply.bFECCapability    = (reader->readOrDefault(1 /*FEC*/, 0x0) == 1) ? true : false;
    reply.TotalPBN          = reader->readOrDefault(16 /*PBN*/, 0xFFFF);
    reply.FreePBN           = reader->readOrDefault(16 /*PBN*/, 0xFFFF);

    if (this->getSinkPort() != reply.portNumber)
        return ParseResponseWrong;

    return ParseResponseSuccess;
}

//
// ALLOCATE_PAYLOAD     0x11
//
void AllocatePayloadMessage::set
(
    const Address & target,
    unsigned port,
    unsigned nSDPStreams,
    unsigned vcPayloadId,
    unsigned PBN,
    unsigned* SDPStreamSink,
    bool entirePath
)
{
    clear();
    BitStreamWriter writer(&encodedMessage.buffer, 0);

    // Write request identifier
    writer.write(0/*zero*/, 1);
    writer.write(requestIdentifier, 7);

    DP_ASSERT(SDPStreamSink || (!nSDPStreams));

    // Write message request body
    writer.write(port, 4);
    writer.write(nSDPStreams, 4);
    writer.write(0/*zero*/, 1);
    writer.write(vcPayloadId, 7);
    writer.write(PBN, 16);
    for (unsigned i=0; i<nSDPStreams; i++)
    {
        writer.write(SDPStreamSink[i], 4);
    }

    // emit 0s until byte aligned.
    writer.align(8);

    encodedMessage.isPathMessage = entirePath;
    encodedMessage.isBroadcast  = false;
    encodedMessage.address = target;
    sinkPort = port;
}

ParseResponseStatus AllocatePayloadMessage::parseResponseAck(EncodedMessage * message, BitStreamReader * reader)
{
    reply.portNumber = reader->readOrDefault(4 /*Port_Number*/, 0xF);
    reader->readOrDefault(5 /*zeroes*/, 0);
    reply.virtualChannelPayloadId = reader->readOrDefault(7 /*Virtual_Channel_Payload_Identifier*/, 0x0);
    reply.PBN = reader->readOrDefault(16 /*PBN*/, 0xFFFF);

    if (this->getSinkPort() != reply.portNumber)
        return ParseResponseWrong;

    return ParseResponseSuccess;
}
//
// QUERY_PAYLOAD        0x12
//
QueryPayloadMessage::QueryPayloadMessage
(
    const Address & target,
    unsigned port,
    unsigned vcPayloadId
)
 : Message(NV_DP_SBMSG_REQUEST_ID_QUERY_PAYLOAD /* request identifier*/,
           NV_DP_SBMSG_PRIORITY_LEVEL_DEFAULT)
{
    BitStreamWriter writer(&encodedMessage.buffer, 0);


    // Write request identifier
    writer.write(0 /*zero*/, 1);
    writer.write(requestIdentifier, 7);

    // Write message request
    writer.write(port, 4);
    writer.write(0 /*zeroes*/, 5);
    writer.write(vcPayloadId, 7);

    encodedMessage.isPathMessage = false;
    encodedMessage.isBroadcast  = false;
    encodedMessage.address = target;
    sinkPort = port;
    dpMemZero(&reply, sizeof(reply));
}

ParseResponseStatus QueryPayloadMessage::parseResponseAck(EncodedMessage * message, BitStreamReader * reader)
{
    reply.portNumber = reader->readOrDefault(4 /*Port_Number*/, 0xF);
    reader->readOrDefault(4 /*zeroes*/, 0);
    reply.allocatedPBN = reader->readOrDefault(16 /*Allocated_PBN*/, 0xFFFF);

    if (this->getSinkPort() != reply.portNumber)
        return ParseResponseWrong;

    return ParseResponseSuccess;
}


//
// RESOURCE_STATUS_NOTIFY 0x13
//

ResStatusNotifyMessage::ResStatusNotifyMessage(MessageReceiverEventSink * sink)
: MessageReceiver(sink, NV_DP_SBMSG_REQUEST_ID_RESOURCE_STATUS_NOTIFY /*request id*/)
{
    dpMemZero(&request, sizeof(request));
}

bool ResStatusNotifyMessage::processByType(EncodedMessage * message, BitStreamReader * reader)
{
    bool status;

    // read the request body
    request.port                = reader->readOrDefault(4 /*Port_Number*/, 0xF);
    request.availableStreams    = reader->readOrDefault(3 /*Available_Streams*/, 0);
    request.bFECCapability      = reader->readOrDefault(1 /*FEC Capability*/, 0);
    status                      = DisplayPort::extractGUID(reader, &request.guid);
    request.PBN                 = reader->readOrDefault(16/*Available_PBN*/, 0);

    // action will be implemented by evensink
    this->sink->messageProcessed(this);
    return status;
}

//
// REMOTE_DPCD_READ     0x20
//
void RemoteDpcdReadMessage::set
(
    const Address & target,
    unsigned port,
    unsigned dpcdAddress,
    unsigned nBytesToRead
)
{
    clear();

    BitStreamWriter writer(&encodedMessage.buffer, 0);

    //    Write request identifier
    writer.write(0/*zero*/, 1);
    writer.write(requestIdentifier, 7);

    // write request data
    writer.write(port, 4);
    writer.write(dpcdAddress, 20);
    writer.write(nBytesToRead, 8);

    encodedMessage.isPathMessage = false;
    encodedMessage.isBroadcast  = false;
    encodedMessage.address = target;
    sinkPort = port;
}

ParseResponseStatus RemoteDpcdReadMessage::parseResponseAck(EncodedMessage * message, BitStreamReader * reader)
{
    reader->readOrDefault(4 /*zeroes*/, 0);
    reply.portNumber = reader->readOrDefault(4 /*Port_Number*/, 0xF);
    reply.numBytesReadDPCD = reader->readOrDefault(8 /*Num_Of_Bytes_Read*/, 0x0);
    for (unsigned i=0; i<reply.numBytesReadDPCD; i++)
    {
        reply.readData[i] = (NvU8)reader->readOrDefault(8 /*data*/, 0x0);
    }

    if (this->getSinkPort() != reply.portNumber)
        return ParseResponseWrong;

    return ParseResponseSuccess;
}

//
// REMOTE_DPCD_WRITE     0x21
//
void RemoteDpcdWriteMessage::set
(
    const Address & target,
    unsigned port,
    unsigned dpcdAddress,
    unsigned nBytesToWrite,
    const NvU8 * writeData
)
{
    clear();
    BitStreamWriter writer(&encodedMessage.buffer, 0);

    DP_ASSERT(writeData || (!nBytesToWrite));

    //    Write request identifier
    writer.write(0/*zero*/, 1);
    writer.write(requestIdentifier, 7);

    // write request data
    writer.write(port, 4);
    writer.write(dpcdAddress, 20);
    writer.write(nBytesToWrite, 8);

    for (unsigned i=0; i<nBytesToWrite; i++)
    {
        writer.write(writeData[i], 8);
    }

    encodedMessage.isPathMessage = false;
    encodedMessage.isBroadcast  = false;
    encodedMessage.address = target;
    sinkPort = port;
}

ParseResponseStatus RemoteDpcdWriteMessage::parseResponseAck(EncodedMessage * message, BitStreamReader * reader)
{
    reader->readOrDefault(4 /*zeroes*/, 0);
    unsigned portNumber = reader->readOrDefault(4 /*Port_Number*/, 0xF);

    DP_ASSERT(portNumber == this->sinkPort);
    DP_USED(portNumber);

    if (this->getSinkPort() != portNumber)
        return ParseResponseWrong;

    return ParseResponseSuccess;
}

//
// REMOTE_I2C_READ      0x22
//
void RemoteI2cReadMessage::set
(
    const Address & target,
    unsigned nWriteTransactions,
    unsigned port,
    I2cWriteTransaction* transactions,
    unsigned readI2cDeviceId,
    unsigned nBytesToRead
)
{
    clear();

    BitStreamWriter writer(&encodedMessage.buffer, 0);

    DP_ASSERT(transactions || (!nWriteTransactions));

    //    Write request identifier
    writer.write(0 /*zero*/, 1);
    writer.write(requestIdentifier, 7);

    // write request specific data
    writer.write(port, 4);
    writer.write(0/*zeroes*/, 2);
    writer.write(nWriteTransactions, 2);

    for (unsigned i=0; i<nWriteTransactions; i++)
    {
        writer.write(0/*zero*/, 1);
        writer.write(transactions[i].WriteI2cDeviceId, 7);
        writer.write(transactions[i].NumBytes, 8);
        for(unsigned j=0; j<transactions[i].NumBytes; j++)
        {
            writer.write(transactions[i].I2cData[j], 8);
        }
        writer.write(0/*zeroes*/, 3);
        writer.write(transactions[i].NoStopBit ? 1 : 0, 1);
        writer.write(transactions[i].I2cTransactionDelay, 4);
    }
    writer.write(0/*zero*/, 1);
    writer.write(readI2cDeviceId, 7);
    writer.write(nBytesToRead, 8);

    encodedMessage.isPathMessage = false;
    encodedMessage.isBroadcast  = false;
    encodedMessage.address = target;
    sinkPort = port;
}

ParseResponseStatus RemoteI2cReadMessage::parseResponseAck(EncodedMessage * message, BitStreamReader * reader)
{
    reader->readOrDefault(4 /*zeroes*/, 0);
    reply.portNumber = reader->readOrDefault(4 /*Port_Number*/, 0xF);
    reply.numBytesReadI2C = reader->readOrDefault(8 /*Num_Of_Bytes_Read*/, 0x0);
    for (unsigned i=0; i<reply.numBytesReadI2C; i++)
    {
        reply.readData[i] = (NvU8)reader->readOrDefault(8 /*data*/, 0x0);
    }

    if (this->getSinkPort() != reply.portNumber)
        return ParseResponseWrong;

    return ParseResponseSuccess;
}

//
// REMOTE_I2C_WRITE     0x23
//
void RemoteI2cWriteMessage::set
(
    const Address & target,
    unsigned port,
    unsigned writeI2cDeviceId,
    unsigned nBytesToWrite,
    unsigned char* writeData
)
{
    clear();

    BitStreamWriter writer(&encodedMessage.buffer, 0);

    DP_ASSERT(writeData || (!nBytesToWrite));

    //    Write request identifier
    writer.write(0 /*zero*/, 1);
    writer.write(requestIdentifier, 7);

    // write request data
    writer.write(port, 4);
    writer.write(0/*zero*/, 5);
    writer.write(writeI2cDeviceId, 7);
    writer.write(nBytesToWrite, 8);

    for (unsigned i=0; i<nBytesToWrite; i++)
    {
        writer.write(writeData[i], 8);
    }

    encodedMessage.isPathMessage = false;
    encodedMessage.isBroadcast  = false;
    encodedMessage.address = target;
    sinkPort = port;
}

ParseResponseStatus RemoteI2cWriteMessage::parseResponseAck(EncodedMessage * message, BitStreamReader * reader)
{
    reader->readOrDefault(4 /*zeroes*/, 0);
    reply.portNumber = reader->readOrDefault(4 /*Port_Number*/, 0xF);

    if (this->getSinkPort() != reply.portNumber)
        return ParseResponseWrong;

    return ParseResponseSuccess;
}
//
// POWER_UP_PHY         0x24
//
void PowerUpPhyMessage::set
(
    const Address & target,
    unsigned port,
    bool entirePath
)
{
    clear();

    BitStreamWriter writer(&encodedMessage.buffer, 0);

    // Write request identifier
    writer.write(0 /*zero*/, 1);
    writer.write(requestIdentifier, 7);

    // write request specific data
    writer.write(port, 4);
    writer.write(0 /*zero*/, 4);

    encodedMessage.isPathMessage = entirePath;
    encodedMessage.isBroadcast  = false;
    encodedMessage.address = target;
    sinkPort = port;
}

//
// POWER_DOWN_PHY       0x25
//
ParseResponseStatus PowerUpPhyMessage::parseResponseAck(EncodedMessage * message, BitStreamReader * reader)
{
    reply.portNumber = reader->readOrDefault(4 /*Port_Number*/, 0xF);
    reader->readOrDefault(4 /*zeroes*/, 0);

    if (this->getSinkPort() != reply.portNumber)
        return ParseResponseWrong;

    return ParseResponseSuccess;
}

void PowerDownPhyMessage::set
(
    const Address & target,
    unsigned port,
    bool entirePath
)
{
    BitStreamWriter writer(&encodedMessage.buffer, 0);

    // Write request identifier
    writer.write(0 /*zero*/, 1);
    writer.write(requestIdentifier, 7);

    // write request specific data
    writer.write(port, 4);
    writer.write(0/*zeros*/, 4);

    encodedMessage.isPathMessage = entirePath;
    encodedMessage.isBroadcast  = false;
    encodedMessage.address = target;
    sinkPort = port;
}

ParseResponseStatus PowerDownPhyMessage::parseResponseAck(EncodedMessage * message, BitStreamReader * reader)
{
    reply.portNumber = reader->readOrDefault(4 /*Port_Number*/, 0xF);
    reader->readOrDefault(4 /*zeroes*/, 0);

    if (this->getSinkPort() != reply.portNumber)
        return ParseResponseWrong;

    return ParseResponseSuccess;
}

//
// SINK_EVENT_NOTIFY 0x30
//

SinkEventNotifyMessage::SinkEventNotifyMessage(MessageReceiverEventSink * sink, unsigned requestId)
: MessageReceiver(sink, 0x30 /*request id*/)
{
}

bool SinkEventNotifyMessage::processByType(EncodedMessage * message, BitStreamReader * reader)
{
    return true;
}


I2cWriteTransaction::I2cWriteTransaction
(
    unsigned WriteI2cDeviceId,
    unsigned NumBytes,
    unsigned char * buffer,
    bool NoStopBit,
    unsigned I2cTransactionDelay
)
{
    this->WriteI2cDeviceId = WriteI2cDeviceId;
    this->NumBytes = NumBytes;
    this->NoStopBit = NoStopBit;
    this->I2cTransactionDelay = I2cTransactionDelay;
    this->I2cData = buffer;
}

I2cWriteTransaction::I2cWriteTransaction():
WriteI2cDeviceId(0), NumBytes(0), I2cData(0), NoStopBit(0), I2cTransactionDelay(0)
{
}

