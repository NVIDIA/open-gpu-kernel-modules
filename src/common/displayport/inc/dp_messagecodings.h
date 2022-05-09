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
* Module: dp_messagecodings.h                                               *
*    Encoding routines for various messages.                                *
*                                                                           *
\***************************************************************************/
#ifndef INCLUDED_DP_MESSAGECODINGS_H
#define INCLUDED_DP_MESSAGECODINGS_H

#include "dp_messages.h"
#include "displayport.h"
#include "dp_auxdefs.h"

/* Fields for the HDCP stream status */
#define NV_DP_HDCP_STREAM_STATE                                                1:0
#define NV_DP_HDCP_STREAM_STATE_NO_EXIST                              (0x00000000)
#define NV_DP_HDCP_STREAM_STATE_NOT_ACTIVE                            (0x00000001)
#define NV_DP_HDCP_STREAM_STATE_ACTIVE                                (0x00000002)
#define NV_DP_HDCP_STREAM_STATE_ERROR                                 (0x00000003)
#define NV_DP_HDCP_STREAM_REPEATER                                             2:2
#define NV_DP_HDCP_STREAM_REPEATER_SIMPLE                             (0x00000000)
#define NV_DP_HDCP_STREAM_REPEATER_REPEATER                           (0x00000001)
#define NV_DP_HDCP_STREAM_ENCRYPTION                                           3:3
#define NV_DP_HDCP_STREAM_ENCRYPTION_OFF                              (0x00000000)
#define NV_DP_HDCP_STREAM_ENCRYPTION_ON                               (0x00000001)
#define NV_DP_HDCP_STREAM_AUTHENTICATION                                       4:4
#define NV_DP_HDCP_STREAM_AUTHENTICATION_OFF                          (0x00000000)
#define NV_DP_HDCP_STREAM_AUTHENTICATION_IP                           (0x00000000)
#define NV_DP_HDCP_STREAM_AUTHENTICATION_ON                           (0x00000001)
#define NV_DP_HDCP_STREAM_OUTPUT_SINK_LEGACY                                   8:8
#define NV_DP_HDCP_STREAM_OUTPUT_SINK_LEGACY_NO                       (0x00000000)
#define NV_DP_HDCP_STREAM_OUTPUT_SINK_LEGACY_YES                      (0x00000001)
#define NV_DP_HDCP_STREAM_OUTPUT_SINK_NON_DP1_2_CP                             9:9
#define NV_DP_HDCP_STREAM_OUTPUT_SINK_NON_DP1_2_CP_NO                 (0x00000000)
#define NV_DP_HDCP_STREAM_OUTPUT_SINK_NON_DP1_2_CP_YES                (0x00000001)
#define NV_DP_HDCP_STREAM_OUTPUT_SINK_MULTI                                  10:10
#define NV_DP_HDCP_STREAM_OUTPUT_SINK_MULTI_NO                        (0x00000000)
#define NV_DP_HDCP_STREAM_OUTPUT_SINK_MULTI_YES                       (0x00000001)
#define NV_DP_HDCP_STREAM_OUTPUT_CP_TYPE_HDCP1X                              11:11
#define NV_DP_HDCP_STREAM_OUTPUT_CP_TYPE_HDCP1X_NO                    (0x00000000)
#define NV_DP_HDCP_STREAM_OUTPUT_CP_TYPE_HDCP1X_YES                   (0x00000001)
#define NV_DP_HDCP_STREAM_OUTPUT_CP_TYPE_HDCP2X                              12:12
#define NV_DP_HDCP_STREAM_OUTPUT_CP_TYPE_HDCP2X_NO                    (0x00000000)
#define NV_DP_HDCP_STREAM_OUTPUT_CP_TYPE_HDCP2X_YES                   (0x00000001)

namespace DisplayPort
{
    typedef NakData Message_NakData;

    enum
    {
        REMOTE_READ_BUFFER_SIZE = 128,
    };

    typedef enum
    {
        None,
        UpstreamSourceOrSSTBranch,
        DownstreamBranch,
        DownstreamSink,
        Dongle
    }PeerDevice;

    struct I2cWriteTransaction
    {
        I2cWriteTransaction(unsigned WriteI2cDeviceId, unsigned NumBytes,
                            unsigned char * buffer, bool NoStopBit = false,
                            unsigned I2cTransactionDelay = 0);
        I2cWriteTransaction();
        unsigned        WriteI2cDeviceId;
        unsigned        NumBytes;
        unsigned char  *I2cData;
        bool            NoStopBit;
        unsigned        I2cTransactionDelay;
    };

    typedef enum
    {
        DoesNotExist = 0,
        NotActive = 1,
        Active = 2,
    }StreamState;

    typedef enum
    {
        CP_IRQ_ON = 0,
        No_EVENT = 1
    }StreamEvent;

    typedef enum
    {
        STREAM_BEHAVIOUR_MASK_OFF = 0,
        STREAM_BEHAVIOUR_MASK_ON = 1
    }StreamBehaviorMask;

    typedef enum
    {
        STREAM_EVENT_MASK_OFF = 0,
        STREAM_EVENT_MASK_ON  = 1
    }StreamEventMask;

    typedef enum
    {
        Force_Reauth = 0,
        BlockFlow = 1
    }StreamBehavior;


    typedef enum
    {
        StreamUnconnected = 0,
        NonAuthLegacyDevice = 1,  // TV or CRT
        DP_MST = 4
    }OutputSinkType;

    typedef enum
    {
        HDCP1x = 1,
        HDCP2x = 2
    }OutputCPType;

    typedef enum
    {
        SinkEvent0,
        SinkEvent255 = 0xFF
    }SinkEvent;

    //
    // LINK_ADDRESS    0x1
    //
    class LinkAddressMessage : public MessageManager::Message
    {
    public:
        struct Result
        {
            bool        isInputPort;
            PeerDevice  peerDeviceType;
            unsigned    portNumber;
            bool        hasMessaging;
            bool        dpPlugged;

            bool        legacyPlugged;
            unsigned    dpcdRevisionMajor;
            unsigned    dpcdRevisionMinor;
            GUID        peerGUID;
            unsigned    SDPStreams;
            unsigned    SDPStreamSinks;
        };

        virtual ParseResponseStatus parseResponseAck(EncodedMessage * message,
                                                     BitStreamReader * reader);

    private:
        struct
        {
            GUID guid;                  // originating branch device
            unsigned numberOfPorts;
            Result res[16];
        } reply;

    public:
        LinkAddressMessage() : Message(NV_DP_SBMSG_REQUEST_ID_LINK_ADDRESS,
                                       NV_DP_SBMSG_PRIORITY_LEVEL_2)
        {
            dpMemZero(&reply, sizeof(reply));
        }

        // Second stage init kept separate from constructor (reusable message)
        void set(const Address & target);

        void getGUID(GUID & guid){guid = reply.guid;}

        // Number of ports described
        unsigned resultCount(){return reply.numberOfPorts;}
        const Result * result(unsigned index)
        {
            return &reply.res[index];
        }
    };


    //
    // CONNECTION_STATUS_NOTIFY    0x2
    //
    class ConnStatusNotifyMessage : public MessageManager::MessageReceiver
    {
    public:
        typedef struct
        {
            GUID guid;
            unsigned port;
            bool legacyPlugged;
            bool devicePlugged;
            bool messagingCapability;
            bool isInputPort;
            PeerDevice peerDeviceType;
        }Request;

    protected:
        Request request;

    public:
        Request * getUpRequestData(){ return &request; }
        virtual bool processByType(EncodedMessage * message, BitStreamReader * reader);
        ConnStatusNotifyMessage(MessageReceiverEventSink * sink);
    };

    //
    // GENERIC_UP_REPLY  0xnn
    //
    class GenericUpReplyMessage : public MessageManager::Message
    {
        virtual ParseResponseStatus parseResponseAck(EncodedMessage * message,
                                                     BitStreamReader * reader);

        virtual void expired(const void * tag)
        { }

    public:
        GenericUpReplyMessage(const Address & target, unsigned requestId,
                              bool bReplyIsNack = false, bool bBroadcast = true,
                              bool bPath = false);
        GenericUpReplyMessage(unsigned requestId, bool bReplyIsNack,
                              bool bBroadcast, bool bPath);
        void set(const Address & target, bool bReplyIsNack = false,
                 bool bBroadcast = true, bool bPath = false);

    };

    //
    // CLEAR_PAYLOAD_ID_TABLE 0x14
    //
    class ClearPayloadIdTableMessage : public MessageManager::Message
    {
        virtual ParseResponseStatus parseResponseAck(EncodedMessage * message,
                                                     BitStreamReader * reader);
        virtual ParseResponseStatus parseResponse(EncodedMessage * message);
    public:
        ClearPayloadIdTableMessage();
    };

    //
    // ENUM_PATH_RESOURCES  0x10
    //
    class EnumPathResMessage : public MessageManager::Message
    {
        virtual ParseResponseStatus parseResponseAck(EncodedMessage * message,
                                                     BitStreamReader * reader);

    public:
        struct
        {
            unsigned portNumber;
            bool     bFECCapability;
            unsigned TotalPBN;
            unsigned FreePBN;
        } reply;
        EnumPathResMessage(const Address & target, unsigned port, bool point);
    };

    //
    // ALLOCATE_PAYLOAD     0x11
    //
    class AllocatePayloadMessage : public MessageManager::Message
    {
        virtual ParseResponseStatus parseResponseAck(EncodedMessage * message,
                                                     BitStreamReader * reader);

    private:
        struct
        {
            unsigned portNumber;
            unsigned PBN;
            unsigned virtualChannelPayloadId;
        }reply;

    public:

        AllocatePayloadMessage() : Message(NV_DP_SBMSG_REQUEST_ID_ALLOCATE_PAYLOAD,
                                           NV_DP_SBMSG_PRIORITY_LEVEL_4)
        {
            dpMemZero(&reply, sizeof(reply));
        }

        void set(const Address & target,
            unsigned port,
            unsigned nSDPStreams,
            unsigned vcPayloadId,
            unsigned PBN,
            unsigned* SDPStreamSink,
            bool entirePath);

        unsigned replyPortNumber(){return reply.portNumber;}
        unsigned replyPBN(){return reply.PBN;}
        unsigned replyVirtualChannelPayloadId(){return reply.virtualChannelPayloadId;}

    };

    //
    // QUERY_PAYLOAD        0x12
    //
    class QueryPayloadMessage : public MessageManager::Message
    {
        virtual ParseResponseStatus parseResponseAck(EncodedMessage * message,
                                                     BitStreamReader * reader);

    private:
        struct
        {
            unsigned portNumber;
            unsigned allocatedPBN;
        } reply;

    public:
        QueryPayloadMessage(const Address & target,
            unsigned port,
            unsigned vcPayloadId);

        unsigned replyPortNumber() {return reply.portNumber;}
        unsigned replyAllocatedPBN() {return reply.allocatedPBN;}
    };

    //
    // RESOURCE_STATUS_NOTIFY 0x13
    //
    class ResStatusNotifyMessage : public MessageManager::MessageReceiver
    {
        virtual bool processByType(EncodedMessage * message,
                                   BitStreamReader * reader);
    public:
        struct
        {
            unsigned port;
            GUID guid;
            unsigned PBN;
        } request;

    public:
        ResStatusNotifyMessage(MessageReceiverEventSink * sink);
    };

    //
    // REMOTE_DPCD_READ     0x20
    //
    class RemoteDpcdReadMessage : public MessageManager::Message
    {
        virtual ParseResponseStatus parseResponseAck(EncodedMessage * message,
                                                     BitStreamReader * reader);

    private:
        struct
        {
            unsigned portNumber;
            unsigned numBytesReadDPCD;
            unsigned char readData[REMOTE_READ_BUFFER_SIZE];    // Buffer
        } reply;

    public:
        void set(const Address & target,
            unsigned port,
            unsigned dpcdAddress,
            unsigned nBytesToRead);

        RemoteDpcdReadMessage() : Message(NV_DP_SBMSG_REQUEST_ID_REMOTE_DPCD_READ,
                                          NV_DP_SBMSG_PRIORITY_LEVEL_3)
        {
            dpMemZero(&reply, sizeof(reply));
        }


        unsigned replyPortNumber(){return reply.portNumber;}
        unsigned replyNumOfBytesReadDPCD(){return reply.numBytesReadDPCD;}

        const NvU8 * replyGetData()
        {
            return reply.readData;
        }
    };

    //
    // REMOTE_DPCD_WRITE     0x21
    //
    class RemoteDpcdWriteMessage : public MessageManager::Message
    {
        virtual ParseResponseStatus parseResponseAck(EncodedMessage * message,
                                                     BitStreamReader * reader);
    public:
        void set(const Address & target,
            unsigned port,
            unsigned dpcdAddress,
            unsigned nBytesToWrite,
            const NvU8 * writeData);

        RemoteDpcdWriteMessage() : Message(NV_DP_SBMSG_REQUEST_ID_REMOTE_DPCD_WRITE,
                                           NV_DP_SBMSG_PRIORITY_LEVEL_3)  {}
    };

    //
    // REMOTE_I2C_READ      0x22
    //
    class RemoteI2cReadMessage : public MessageManager::Message
    {
        virtual ParseResponseStatus parseResponseAck(EncodedMessage * message,
                                                     BitStreamReader * reader);
    private:
        struct
        {
            unsigned portNumber;
            unsigned numBytesReadI2C;
            unsigned char readData[REMOTE_READ_BUFFER_SIZE];
        } reply;

    public:

        RemoteI2cReadMessage() : Message(NV_DP_SBMSG_REQUEST_ID_REMOTE_I2C_READ,
                                         NV_DP_SBMSG_PRIORITY_LEVEL_3)
        {
            dpMemZero(&reply, sizeof(reply));
        }

        void set(const Address & target,
            unsigned nWriteTransactions,
            unsigned port,
            I2cWriteTransaction* transactions,
            unsigned readI2cDeviceId,
            unsigned nBytesToRead);

        unsigned replyPortNumber(){return reply.portNumber;}
        unsigned replyNumOfBytesReadI2C(){return reply.numBytesReadI2C;}
        unsigned char* replyGetI2CData(unsigned* numBytes)
        {
            *numBytes = this->replyNumOfBytesReadI2C();
            return reply.readData;
        }
    };

    //
    // REMOTE_I2C_WRITE      0x23
    //
    class RemoteI2cWriteMessage : public MessageManager::Message
    {
        virtual ParseResponseStatus parseResponseAck(EncodedMessage * message,
                                                     BitStreamReader * reader);
    private:
        struct
        {
            unsigned portNumber;
        } reply;

    public:

        RemoteI2cWriteMessage()  : Message(NV_DP_SBMSG_REQUEST_ID_REMOTE_I2C_WRITE,
                                           NV_DP_SBMSG_PRIORITY_LEVEL_3)
        {
            dpMemZero(&reply, sizeof(reply));
        }

        void set(const Address & target,
            unsigned port,
            unsigned writeI2cDeviceId,
            unsigned nBytesToWrite,
            unsigned char* writeData);

        unsigned replyPortNumber() {return reply.portNumber;}
    };

    //
    // POWER_UP_PHY         0x24
    //
    class PowerUpPhyMessage : public MessageManager::Message
    {
        virtual ParseResponseStatus parseResponseAck(EncodedMessage * message,
                                                     BitStreamReader * reader);

    private:
        struct
        {
            unsigned portNumber;
        } reply;

    public:
        PowerUpPhyMessage() : Message(NV_DP_SBMSG_REQUEST_ID_POWER_UP_PHY,
                                      NV_DP_SBMSG_PRIORITY_LEVEL_3)
        {
            dpMemZero(&reply, sizeof(reply));
        }

        void set(const Address & target,
            unsigned port,
            bool entirePath);

        unsigned replyPortNumber(){return reply.portNumber;}
    };

    //
    // POWER_DOWN_PHY       0x25
    //
    class PowerDownPhyMessage : public MessageManager::Message
    {
        virtual ParseResponseStatus parseResponseAck(EncodedMessage * message,
                                                     BitStreamReader * reader);

    private:
        struct
        {
            unsigned portNumber;
        } reply;

    public:
        PowerDownPhyMessage() : Message(NV_DP_SBMSG_REQUEST_ID_POWER_DOWN_PHY,
                                        NV_DP_SBMSG_PRIORITY_LEVEL_3)
        {
            dpMemZero(&reply, sizeof(reply));
        }

        void set(const Address & target,
            unsigned port,
            bool entirePath);

        unsigned replyPortNumber(){return reply.portNumber;}
    };

    //
    // SINK_EVENT_NOTIFY 0x30
    //
    class SinkEventNotifyMessage : public MessageManager::MessageReceiver
    {
        virtual bool processByType(EncodedMessage * message, BitStreamReader * reader);

    public:
        SinkEventNotifyMessage(MessageReceiverEventSink * sink, unsigned requestId);
    };

}

#endif //INCLUDED_DP_MESSAGECODINGS_H
