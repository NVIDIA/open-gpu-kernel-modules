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
* Module: dp_discovery.h                                                    *
*    Class definition for discovery manager.                                *
*                                                                           *
\***************************************************************************/
#ifndef INCLUDED_DP_DISCOVERY_H
#define INCLUDED_DP_DISCOVERY_H

#include "dp_address.h"
#include "dp_list.h"
#include "dp_messages.h"
#include "dp_messagecodings.h"

namespace DisplayPort
{
    class DiscoveryManager : virtual public Object
    {
    public:
        struct Device
        {
            Address     address;            // direct topology address

            bool        legacy;             // legacy (NON DP) device emulated on this port
            bool        branch;             // DP 1.2 style branching device
            PeerDevice  peerDevice;         // connector type of the device on this port
            unsigned    dpcdRevisionMajor;
            unsigned    dpcdRevisionMinor;
            GUID        peerGuid;           // device guid
            unsigned    SDPStreams;         // maximum number of audio streams supported
            unsigned    SDPStreamSinks;     // number of outputs to select from
            bool        dirty;              // got updates for the same device
            PortMap     portMap;
            bool        videoSink;          // Should be true when a video sink is supported
            NvU64       maxTmdsClkRate;

            Device():peerDevice(None),SDPStreams(0),SDPStreamSinks(0),dirty(false),videoSink(false),maxTmdsClkRate(0)
            {
                portMap.validMap = portMap.inputMap = portMap.internalMap = 0;
            }

            ~Device(){}

        };

        struct ReceiverSink :
            virtual public Object,
            public MessageManager::MessageReceiver::MessageReceiverEventSink
        {
            DiscoveryManager * parent;

            // will handle CSN (up_req) and generate a up_reply for it.
            virtual void messageProcessed(MessageManager::MessageReceiver * from);
            void handleCSN(MessageManager::MessageReceiver * from);

            ReceiverSink(DiscoveryManager * parent)
                :parent(parent)
            {}

            virtual ~ReceiverSink()
            {}
        };

        // This will account for upreplies and their failures/retries.
        struct CsnUpReplyContainer : ListElement, Timer::TimerCallback, MessageManager::Message::MessageEventSink
        {
            struct CsnUpReply: public GenericUpReplyMessage
            {
                CsnUpReplyContainer * container;

                CsnUpReply(CsnUpReplyContainer * container, const Address & target)
                    : GenericUpReplyMessage(target, 0x2), container(container)
                {}

                ~CsnUpReply()
                {}

            };

            DiscoveryManager * parent;
            CsnUpReply upReplyMessage;
            unsigned delayInUsec;
            unsigned retries;
            Address target;

            virtual void messageFailed(MessageManager::Message * from, NakData * nakData)
            {
                // if reason of failure is not timeout or defer; just forget trying again.
                if (!(nakData->reason == NakDefer || nakData->reason == NakTimeout))
                {
                    messageCompleted(from);
                    return;
                }

                // queue a callback to reset and send again
                queueUpReply();
                return;
            }

            virtual void messageCompleted(MessageManager::Message * from)
            {
                // don't delete now. Queue callback to delete later
                retries = 0;
                parent->timer->queueCallback(this, "CSNF", 5000);
            }

            void queueUpReply()
            {
                parent->timer->queueCallback(this, "CSNF", delayInUsec/1000);
            }

            void postUpReply()
            {
                upReplyMessage.set(target);
                parent->messageManager->postReply(&this->upReplyMessage, this);
            }

            virtual void expired(const void * tag)
            {
                if (retries)
                    retries--;

                if (retries)
                    postUpReply();
                else
                {
                    // enough retries. wrap up.
                    delete this;
                }
            }

            CsnUpReplyContainer(DiscoveryManager * parent)
                :parent(parent), upReplyMessage(this, target), delayInUsec(200000), retries(4), target(Address(0))
            {}

            virtual ~CsnUpReplyContainer()
            {
                // remove self from queue and delete
                // cancel all pending callbacks
                parent->timer->cancelCallbacks(this);
                parent->pendingCsnUpReplies.remove(this);
            }

        };

        ReceiverSink            receiverSink;

        ConnStatusNotifyMessage connectionStatusNotifyProcessor;

        GUIDBuilder             guidBuilder;

        List                    pendingCsnUpReplies;

    public:

        struct  DiscoveryManagerEventSink
        {
            virtual void discoveryDetectComplete() = 0;                     // reply to processDetect
            virtual void discoveryNewDevice(const DiscoveryManager::Device & device) = 0;         // these can go out anytime
            virtual void discoveryLostDevice(const Address & address) = 0;
        };

        enum {
            maximumTopologyNodes =  128
        };

        Device  currentDevices[maximumTopologyNodes];
        unsigned currentDevicesCount;

        Device * findDevice(const Address & address);
        Device * findDevice(GUID & guid);
        void addDevice(const Device & device);
        void removeDevice(Device * device);
        void removeDeviceTree(const Address & prefix);
        Device * findChildDeviceForBranchWithGuid(GUID guid, unsigned port, Address & childAddr);

        //
        //  This is responsible for a "complete" detection of a sink. Specifically using remote dpcd reads and writes
        //
        struct SinkDetection : MessageManager::Message::MessageEventSink, ListElement, Timer::TimerCallback
        {
            Device                   device;
            Address                  address;
            RemoteDpcdWriteMessage   remoteDpcdWriteMessage;
            RemoteDpcdReadMessage    remoteDpcdReadMessage;
            PowerUpPhyMessage        powerUpPhyMessage;
            LinkAddressMessage       linkAddressMessage;
            DiscoveryManager       * parent;
            bool                     completed;
            unsigned                 retriesRemoteDpcdWriteMessage;
            bool                     retryRemoteDpcdWriteMessage;
            unsigned                 retriesRemoteDpcdReadMessage;
            bool                     retryRemoteDpcdReadMessage;
            unsigned                 retriesLinkAddressMessage;
            bool                     retryLinkAddressMessage;

            bool                     bFromCSN;

            SinkDetection(DiscoveryManager * parent, const Device & device, bool bFromCSN)
                : device(device), address(device.address), parent(parent), completed(false),
                  retriesRemoteDpcdWriteMessage(0), retryRemoteDpcdWriteMessage(false),
                  retriesRemoteDpcdReadMessage(0), retryRemoteDpcdReadMessage(false),
                  retriesLinkAddressMessage(0), retryLinkAddressMessage(false),
                  bFromCSN(bFromCSN)
            {}

            ~SinkDetection();
            void expired(const void * tag);
            void start();

            void detectCompleted(bool passed);
            void messageFailed(MessageManager::Message * from, NakData * nakData);
            void handleRemoteDpcdReadDownReply();
            void handleRemoteDpcdWriteDownReply();
            void handleLinkAddressDownReply();

            void messageCompleted(MessageManager::Message * from);

        };

        //
        //  This object represents an address in some stage of detection
        //
        struct BranchDetection : MessageManager::Message::MessageEventSink, ListElement, Timer::TimerCallback
        {
            Device                     parentDevice;
            Address                    address;
            LinkAddressMessage::Result child[16];
            unsigned                   childCount;

            LinkAddressMessage      linkAddressMessage;
            RemoteDpcdWriteMessage  remoteDpcdWriteMessage;

            DiscoveryManager       * parent;
            bool                     completed;
            bool                     retryLinkAddressMessage;
            unsigned                 retriesLinkAddressMessage;
            unsigned                 retriesRemoteDpcdWriteMessage;
            bool                     retryRemoteDpcdWriteMessage;

            BranchDetection(DiscoveryManager * parent, const Device & device)
                : parentDevice(device), address(parentDevice.address),
                  parent(parent), completed(false),
                  retryLinkAddressMessage(false), retriesLinkAddressMessage(0),
                  retriesRemoteDpcdWriteMessage(0), retryRemoteDpcdWriteMessage(false)
            {}

            void expired(const void * tag);
            void start();
            ~BranchDetection();

            void detectCompleted(bool present);
            void messageFailed(MessageManager::Message * from, NakData * nakData) ;
            void handleLinkAddressDownReply();
            void handleRemoteDpcdReadDownReply();
            void messageCompleted(MessageManager::Message * from);
        };

        void detect(const Address & address);
        void detectBranch(Device device);
        void detectSink(Device newDevice, bool bFromCSN);

public:

        List                        outstandingBranchDetections;
        List                        outstandingSinkDetections;
        DiscoveryManagerEventSink * sink;               // To call NotifyDetectComplete()
        MessageManager *            messageManager;     // For transmit and receive
        Timer *                     timer;
        DPCDHAL *                   hal;

        DiscoveryManager(MessageManager * messageManager, DiscoveryManagerEventSink * sink, Timer * timer, DPCDHAL * hal)
            : receiverSink(this),
              connectionStatusNotifyProcessor(&receiverSink),
              guidBuilder(timer, 0x10DE9070),
              currentDevicesCount(0),
              sink(sink),
              messageManager(messageManager),
              timer(timer),
              hal(hal)
        {

            //
            //  Register to filter all the upmessages.  We want to know when
            //   connection status notify events are on their way.
            //
            messageManager->registerReceiver(&connectionStatusNotifyProcessor);
        }

       ~DiscoveryManager()
       {
           while (!this->outstandingBranchDetections.isEmpty())
             delete this->outstandingBranchDetections.front();

           while (!this->outstandingSinkDetections.isEmpty())
               delete this->outstandingSinkDetections.front();

           while (!this->pendingCsnUpReplies.isEmpty())
               delete this->pendingCsnUpReplies.front();
       }

       void notifyLongPulse(bool status);

    };
}
#endif //INCLUDED_DP_DISCOVERY_H
