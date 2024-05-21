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
* Module: dp_discovery.cpp                                                  *
*    The DP MST discovery manager.                                          *
*                                                                           *
\***************************************************************************/

#include "dp_discovery.h"
#include "dp_messages.h"
#include "dp_tracing.h"
#include "dp_printf.h"

using namespace DisplayPort;

void DiscoveryManager::notifyLongPulse(bool status)
{
    if (status)
    {
        Device device;
        device.address = Address(0);
        device.branch = hal->getSupportsMultistream();
        device.legacy = false;

        detectBranch(device);
    }
    else if (!status)
    {
        removeDeviceTree(Address());
    }
}

void DiscoveryManager::detectBranch(Device device)
{
    Address::StringBuffer sb;
    DP_USED(sb);

    //
    // 1. Create a LINK_ADDRESS_MESSAGE to send to this target so that we can find who he is
    // 2. Create a REMOTE_DPCD_WRITE to set the GUID for this target
    //     *alternatively* we may have to use the local DPCD HAL to write this
    // 3. Enumerate any children that we may wish to queue detect on.
    //
    DP_PRINTF(DP_NOTICE, "%s(): target = %s", __FUNCTION__, device.address.toString(sb));

    BranchDetection * branchDetection = new BranchDetection(this, device);
    outstandingBranchDetections.insertBack(branchDetection);
    branchDetection->start();
}

void DiscoveryManager::detectSink(DiscoveryManager::Device device, bool bFromCSN)
{
    Address::StringBuffer sb;
    DP_USED(sb);

    DP_PRINTF(DP_NOTICE, "%s(): target = %s", __FUNCTION__, device.address.toString(sb));
    SinkDetection * sinkDetection = new SinkDetection(this, device, bFromCSN);
    sinkDetection->start();
}

DiscoveryManager::Device * DiscoveryManager::findDevice(const Address & address)
{
    for (unsigned i = 0; i < currentDevicesCount; i++)
        if (currentDevices[i].address == address)
        {
            if (currentDevices[i].peerGuid.isGuidZero() && currentDevices[i].peerDevice != Dongle &&
                (currentDevices[i].dpcdRevisionMajor >= 1 && currentDevices[i].dpcdRevisionMinor >= 2))
            {
                DP_ASSERT(0 && "Zero guid for device even though its not a dongle type.");
            }
            return &currentDevices[i];
        }

    return 0;
}

DiscoveryManager::Device * DiscoveryManager::findDevice(GUID & guid)
{
    if (guid.isGuidZero())
    {
        DP_ASSERT(0 && "zero guid search");
        return 0;
    }

    for (unsigned i = 0; i < currentDevicesCount; i++)
    {
        if (currentDevices[i].dpcdRevisionMajor <= 1 && currentDevices[i].dpcdRevisionMinor < 2)
            continue;

        if (currentDevices[i].peerGuid == guid)
            return &currentDevices[i];
    }

    return 0;
}

void DiscoveryManager::addDevice(const DiscoveryManager::Device & device)
{
    Address::StringBuffer sb;
    DP_USED(sb);

    GUID guid = device.peerGuid;
    if (guid.isGuidZero() &&
        (device.peerDevice != Dongle) &&
        (device.dpcdRevisionMajor >= 1 && device.dpcdRevisionMinor >= 2))
    {
        DP_ASSERT(0 && "GUID missing for the device");
    }
    DP_ASSERT(!findDevice(device.address) && "Redundant add");
    sink->discoveryNewDevice(device);

    DP_PRINTF(DP_NOTICE, "DP-DM> New device '%s' %s %s %s", device.address.toString(sb),
              device.branch ? "Branch" : "", device.legacy ? "Legacy" : "",
              device.peerDevice == Dongle ? "Dongle" :
              device.peerDevice == DownstreamSink ? "DownstreamSink" : "");

    Address::NvU32Buffer addrBuffer;
    dpMemZero(addrBuffer, sizeof(addrBuffer));
    device.address.toNvU32Buffer(addrBuffer);
    NV_DPTRACE_INFO(NEW_MST_DEVICE, device.address.size(), addrBuffer[0], addrBuffer[1],
                    addrBuffer[2], addrBuffer[3], device.branch, device.legacy, device.peerDevice);

    if (currentDevicesCount < maximumTopologyNodes)
    {
        currentDevices[currentDevicesCount++] = device;
    }
}

void DiscoveryManager::removeDevice(Device * device)
{
    Address::StringBuffer sb;
    DP_USED(sb);

    DP_PRINTF(DP_NOTICE, "DP-DM> Lost device '%s' %s %s %s", device->address.toString(sb),
              device->branch ? "Branch" : "", device->legacy ? "Legacy" : "",
              device->peerDevice == Dongle ? "Dongle" :
              device->peerDevice == DownstreamSink ? "DownstreamSink" : "");

    sink->discoveryLostDevice(device->address);

    for (unsigned i = (unsigned)(device-&currentDevices[0]); i < currentDevicesCount - 1; i++)
        currentDevices[i] = currentDevices[i+1];
    currentDevicesCount--;
}

void DiscoveryManager::removeDeviceTree(const Address & prefix)
{
    for (unsigned i = 0; i < currentDevicesCount;)
        if (currentDevices[i].address.under(prefix))
            removeDevice(&currentDevices[i]);
        else
            i++;
}

DiscoveryManager::Device * DiscoveryManager::findChildDeviceForBranchWithGuid
(
    GUID        guid,
    unsigned    port,
    Address   & childAddr
)
{
    // Find it in relevant parent's device list
    DiscoveryManager::Device * parentDevice = findDevice(guid);
    if (!parentDevice)
    {
        DP_PRINTF(DP_ERROR, "DM> No Parent present for the device in DB.");
        return 0;
    }

    childAddr = parentDevice->address;
    childAddr.append(port);
    return (findDevice(childAddr));
}

void DiscoveryManager::SinkDetection::detectCompleted(bool passed)
{
    // we could not read or write the guid
    if (!passed)
    {
        //
        // DP1.2 monitors that do not support GUID get filtered and dropped as 'not present'.
        // Instead we demote such monitors to DP1.1 and continue sink detection so that end
        // user at least gets active display scanout on such monitors (albeit reduced to DP1.1).
        //
        if (device.dpcdRevisionMajor > 1 || device.dpcdRevisionMinor >= 2)
        {
            Address::StringBuffer sb;
            DP_USED(sb);
            DP_PRINTF(DP_ERROR, "DP-DM> sink at '%s' failed GUID identification, demote to 1.1 sink.",
                      address.toString(sb));
            device.dpcdRevisionMajor = 1;
            device.dpcdRevisionMinor = 1;
        }
        else
        {
            // Had it previously been reported as present?
            if (Device * device = parent->findDevice(address))
                parent->removeDevice(device);

            delete this;
            return;
        }
    }

    // at this point we are sure that we have a device GUID.
    // We need to check whether the device is new to the DB.
    //     Had we previously reported the device?

    Device * oldDevice = parent->findDevice(device.address);

    if (!oldDevice)
    {
        // completely new device
        parent->addDevice(device);
    }
    // If it was a branch and now isn't.. delete the tree of devices under it
    else if (oldDevice && oldDevice->branch && !device.branch)
    {
        parent->removeDeviceTree(device.address);
    }
    // It changed, delete the previously reported
    else if (oldDevice && (oldDevice->legacy != device.legacy ||
                           oldDevice->dpcdRevisionMajor!= device.dpcdRevisionMajor ||
                           oldDevice->dpcdRevisionMinor!= device.dpcdRevisionMinor ||
                           oldDevice->peerDevice != device.peerDevice||
                           oldDevice->peerGuid != device.peerGuid ||
                           oldDevice->SDPStreams != device.SDPStreams||
                           oldDevice->SDPStreamSinks != device.SDPStreamSinks ||
                           oldDevice->videoSink != device.videoSink))
    {
        parent->removeDevice(oldDevice);
    }

    // otherwise.. it already existed, and still does

    // We're done
    completed = true;
    delete this;
}

void DiscoveryManager::BranchDetection::detectCompleted(bool present)
{
    //
    //     Handle device not present
    //
    if (!present)
    {
        // Had it previously been reported as present?
        if (Device * device = parent->findDevice(address))
            parent->removeDevice(device);

        delete this;
        return;
    }

    //
    // We've got a linkAddressMessage and we were able to program the GUID!
    // Report the branch and queue any children that were enumerated for detection
    //
    parent->addDevice(parentDevice);

    unsigned portsToDelete = (1 << (Address::maxPortCount+1)) - 1;    // 16 ports
    for (unsigned i = 0; i < childCount; i++)
    {
        Device newDevice;
        newDevice.address = address;
        newDevice.address.append(child[i].portNumber);

        //
        //     Input port? Nothing plugged in? Delete the tree of all devices under this one
        //     DP 1.2 Spec : 2.11.9.5.x
        //
        if (child[i].isInputPort || !child[i].dpPlugged) {
            continue;
        }

        portsToDelete &= ~(1 << child[i].portNumber);

        newDevice.peerDevice = child[i].peerDeviceType;
        newDevice.legacy = child[i].legacyPlugged && (newDevice.peerDevice == Dongle);
        newDevice.dpcdRevisionMajor = child[i].dpcdRevisionMajor;
        newDevice.dpcdRevisionMinor = child[i].dpcdRevisionMinor;
        // if internal device; use parent's GUID which we ourselves generated or got from the LAM.
        if (child[i].portNumber > PHYSICAL_PORT_END)
            newDevice.peerGuid = parentDevice.peerGuid;
        else
            newDevice.peerGuid = child[i].peerGUID;

        newDevice.SDPStreams = child[i].SDPStreams;
        newDevice.SDPStreamSinks = child[i].SDPStreamSinks;

        if (child[i].peerDeviceType == DownstreamBranch &&
            child[i].hasMessaging)
        {
            newDevice.branch = true;
            newDevice.videoSink = false;
        }
        else
        {
            newDevice.branch = false;
            newDevice.videoSink = ((child[i].peerDeviceType == Dongle) ?
                                    child[i].legacyPlugged : true);
        }

        //
        //     Had we previously reported the device?
        //
        Device * oldDevice = parent->findDevice(newDevice.address);

        // If it was a branch and now isn't.. delete the tree of devices under it
        if (oldDevice && oldDevice->branch && !newDevice.branch)
        {
            parent->removeDeviceTree(newDevice.address);
        }
        // It changed, delete
        else if (oldDevice && (oldDevice->legacy != newDevice.legacy ||
                               oldDevice->dpcdRevisionMajor!= newDevice.dpcdRevisionMajor ||
                               oldDevice->dpcdRevisionMinor!= newDevice.dpcdRevisionMinor ||
                               oldDevice->peerDevice != newDevice.peerDevice||
                               oldDevice->peerGuid != newDevice.peerGuid ||
                               oldDevice->SDPStreams != newDevice.SDPStreams||
                               oldDevice->SDPStreamSinks != newDevice.SDPStreamSinks ||
                               oldDevice->videoSink != newDevice.videoSink))
        {
            parent->removeDevice(oldDevice);
        }

        // otherwise.. it already existed, and still does
        if (newDevice.branch)
        {
            parent->detectBranch(newDevice);
        }
        else
        {
            // the new device is a sink. It may or may not have a guid.
            // write the guid if needed.
            parent->detectSink(newDevice, false);
        }
    }

    for (unsigned i = 0; i <= Address::maxPortCount; i++)
        if ((portsToDelete >> i) & 1)
        {
            Address a = address;
            a.append(i);
            parent->removeDeviceTree(a);
        }

    // We're done
    completed = true;
    delete this;
}

void DiscoveryManager::BranchDetection::expired(const void * tag)
{
    if (retryLinkAddressMessage)
    {
        Address::StringBuffer sb;
        DP_USED(sb);
        DP_PRINTF(DP_NOTICE, "DP-DM> Requeing LINK_ADDRESS_MESSAGE to %s", address.toString(sb));

        retryLinkAddressMessage = false;
        linkAddressMessage.set(address);
        parent->messageManager->post(&linkAddressMessage, this);
    }
    else if (retryRemoteDpcdWriteMessage)
    {
        Address parentAddress = address;
        parentAddress.pop();

        Address::StringBuffer sb;
        DP_USED(sb);
        DP_PRINTF(DP_NOTICE, "DP-DM> Requeing REMOTE_DPCD_WRITE_MESSAGE to %s", parentAddress.toString(sb));

        retryRemoteDpcdWriteMessage = false;
        remoteDpcdWriteMessage.set(parentAddress, parentAddress.tail(), NV_DPCD_GUID, sizeof(GUID), (NvU8 *)&parentDevice.peerGuid);
        DP_PRINTF(DP_NOTICE, "DP-DM> Setting GUID (remotely) for '%s' sent REMOTE_DPCD_WRITE {%p}", address.toString(sb), &remoteDpcdWriteMessage);

        parent->messageManager->post(&remoteDpcdWriteMessage, this);
    }
}

void DiscoveryManager::SinkDetection::expired(const void * tag)
{
    if (retryLinkAddressMessage)
    {
        Address parentAddress = address;
        parentAddress.pop();

        Address::StringBuffer sb;
        DP_USED(sb);
        DP_PRINTF(DP_NOTICE, "DP-DM> Requeueing LAM message to %s", parentAddress.toString(sb));

        retryLinkAddressMessage = false;
        linkAddressMessage.set(parentAddress);

        parent->messageManager->post(&linkAddressMessage, this);
    }
    else if (retryRemoteDpcdReadMessage)
    {
        Address parentAddress = address;
        parentAddress.pop();

        Address::StringBuffer sb;
        DP_USED(sb);
        DP_PRINTF(DP_NOTICE, "DP-DM> Requeueing REMOTE_DPCD_READ_MESSAGE to %s", parentAddress.toString(sb));

        retryRemoteDpcdReadMessage = false;
        remoteDpcdReadMessage.set(parentAddress, parentAddress.tail(), NV_DPCD_GUID, sizeof(GUID));
        DP_PRINTF(DP_NOTICE, "DP-DM> Setting GUID (remotely) for '%s' sent REMOTE_DPCD_READ {%p}", address.toString(sb), &remoteDpcdReadMessage);

        parent->messageManager->post(&remoteDpcdReadMessage, this);
    }
    else if (retryRemoteDpcdWriteMessage)
    {
        Address parentAddress = address;
        parentAddress.pop();

        Address::StringBuffer sb;
        DP_USED(sb);
        DP_PRINTF(DP_NOTICE, "DP-DM> Requeueing REMOTE_DPCD_WRITE_MESSAGE to %s", parentAddress.toString(sb));

        retryRemoteDpcdWriteMessage = false;
        remoteDpcdWriteMessage.set(parentAddress,
                                    parentAddress.tail(),
                                    NV_DPCD_GUID, sizeof(GUID),
                                    (NvU8 *)&device.peerGuid);
        DP_PRINTF(DP_NOTICE, "DP-DM> Setting GUID (remotely) for '%s' sent REMOTE_DPCD_WRITE {%p}", address.toString(sb), &remoteDpcdWriteMessage);

        parent->messageManager->post(&remoteDpcdWriteMessage, this);
    }
}

void DiscoveryManager::BranchDetection::messageFailed(MessageManager::Message * from, NakData * nakData)
{
    //
    // If any of our messages fail, we've completed detection on this buzzard.
    // The only exception is if we get a DEFER - then we retry indefinitely
    //
    if (from == &linkAddressMessage)
    {
        if (retriesLinkAddressMessage < DPCD_LINK_ADDRESS_MESSAGE_RETRIES &&
            (nakData->reason == NakDefer || nakData->reason == NakTimeout))
        {
            retriesLinkAddressMessage++;
            retryLinkAddressMessage = true;
            parent->timer->queueCallback(this, "DISC", DPCD_LINK_ADDRESS_MESSAGE_COOLDOWN);
            return;
        }
    }

    if (from == &remoteDpcdWriteMessage)
    {
        if ((retriesRemoteDpcdWriteMessage < DPCD_REMOTE_DPCD_WRITE_MESSAGE_RETRIES) &&
            (nakData->reason == NakDefer || nakData->reason == NakTimeout))
        {
            retriesRemoteDpcdWriteMessage++;
            retryRemoteDpcdWriteMessage = true;
            parent->timer->queueCallback(this, "DISC", DPCD_REMOTE_DPCD_WRITE_MESSAGE_COOLDOWN);
            return;
        }
    }

    Address::StringBuffer sb;
    DP_USED(sb);
    DP_PRINTF(DP_ERROR, "DP-DM> Message %s {%p} at '%s' failed. Device marked not present.",
              from == &linkAddressMessage ? "LINK_ADDRESS_MESSAGE" :
              from == &remoteDpcdWriteMessage ? "REMOTE_DPCD_WRITE(GUID)" : "???",
              from, address.toString(sb));


    //
    //  Detection is done and branch doesn't exist.
    //  (Note this automatically removes self from any list we're in)
    //
    detectCompleted(false);
}

void DiscoveryManager::SinkDetection::messageFailed(MessageManager::Message * from, NakData * nakData)
{
    if (from == &remoteDpcdReadMessage)
    {
        if ((retriesRemoteDpcdReadMessage < DPCD_REMOTE_DPCD_READ_MESSAGE_RETRIES) &&
            (nakData->reason == NakDefer || nakData->reason == NakTimeout))
        {
            retriesRemoteDpcdReadMessage++;
            retryRemoteDpcdReadMessage = true;
            parent->timer->queueCallback(this, "DISC", DPCD_REMOTE_DPCD_READ_MESSAGE_COOLDOWN);
            return;
        }
    }

    if (from == &remoteDpcdWriteMessage)
    {
        if ((retriesRemoteDpcdWriteMessage < DPCD_REMOTE_DPCD_WRITE_MESSAGE_RETRIES) &&
            (nakData->reason == NakDefer || nakData->reason == NakTimeout))
        {
            retriesRemoteDpcdWriteMessage++;
            retryRemoteDpcdWriteMessage = true;
            parent->timer->queueCallback(this, "DISC", DPCD_REMOTE_DPCD_WRITE_MESSAGE_COOLDOWN);
            return;
        }
    }

    if (from == &linkAddressMessage)
    {
        if ((retriesLinkAddressMessage < DPCD_LINK_ADDRESS_MESSAGE_RETRIES) &&
            (nakData->reason == NakDefer || nakData->reason == NakTimeout))
        {
            retriesLinkAddressMessage++;
            retryLinkAddressMessage = true;
            parent->timer->queueCallback(this, "DISC", DPCD_LINK_ADDRESS_MESSAGE_COOLDOWN);
            return;
        }
    }

    Address::StringBuffer sb;
    DP_USED(sb);
    DP_PRINTF(DP_ERROR, "DP-DM> Message %s {%p} at '%s' failed.",
              from == &remoteDpcdWriteMessage ? "REMOTE_DPCD_WRITE(GUID)" :
              from == &remoteDpcdReadMessage ? "REMOTE_DPCD_READ(GUID)" :
              from == &linkAddressMessage ? "LINK_ADDRESS_MESSAGE" : "???",
              from, address.toString(sb));

    detectCompleted(false);
}

void DiscoveryManager::SinkDetection::handleLinkAddressDownReply()
{
    Address::StringBuffer sb;
    DP_USED(sb);
    LinkAddressMessage::Result child;
    child = *linkAddressMessage.result(address.tail());

    device.peerDevice = child.peerDeviceType;
    device.dpcdRevisionMajor = child.dpcdRevisionMajor;
    device.dpcdRevisionMinor = child.dpcdRevisionMinor;

    if (device.dpcdRevisionMajor == 0)
    {
        device.dpcdRevisionMajor = 1;
        device.dpcdRevisionMinor = 1;
    }
    device.portMap.inputMap |= (1 << child.portNumber);

    DP_PRINTF(DP_NOTICE, "DP-DM> handleLinkAddressDownReply for sink device on '%s': DPCD Rev = %d.%d",
              address.toString(sb), device.dpcdRevisionMajor, device.dpcdRevisionMinor);

    // Check if the device already has a GUID
    // or it is a dongle or on a logical port ; in which case no GUID is required.
    if ((!device.peerGuid.isGuidZero()) ||
        (device.peerDevice == Dongle)   ||
        (device.dpcdRevisionMajor <= 1 && device.dpcdRevisionMinor < 2) ||
        (device.address.tail() > PHYSICAL_PORT_END))
    {
        parent->addDevice(device);
        delete this;
        return;
    }

    Address parentAddress = address.parent();
    remoteDpcdReadMessage.set(parentAddress, address.tail(), NV_DPCD_GUID, sizeof(GUID));

    parent->messageManager->post(&remoteDpcdReadMessage, this);

}

void DiscoveryManager::SinkDetection::handleRemoteDpcdReadDownReply()
{
    Address::StringBuffer sb;
    DP_USED(sb);
    DP_PRINTF(DP_NOTICE, "DP-DM> REMOTE_DPCD_READ {%p} at '%s' completed",
              (MessageManager::Message *)&remoteDpcdReadMessage,
              address.toString(sb));
    if (remoteDpcdReadMessage.replyNumOfBytesReadDPCD() != sizeof(GUID))
    {
        DP_ASSERT(0 && "Incomplete GUID in remote DPCD read message");
        detectCompleted(false);
        return;
    }

    DP_ASSERT(remoteDpcdReadMessage.replyPortNumber() == address.tail());
    device.peerGuid.copyFrom(remoteDpcdReadMessage.replyGetData());

    if (!device.peerGuid.isGuidZero())
    {
        // we got the GUID ... handle device add/remove
        detectCompleted(true);
    }
    else
    {
        //
        //     We need to give ourselves a non-zero GUID!
        //
        parent->guidBuilder.makeGuid(device.peerGuid);

        Address parentAddress = address.parent();
        remoteDpcdWriteMessage.set(parentAddress,
                                    address.tail(),
                                    NV_DPCD_GUID, sizeof(GUID),
                                    (NvU8 *)&device.peerGuid);

        DP_PRINTF(DP_NOTICE, "DP-DM> Setting GUID (remotely) for '%s' sent REMOTE_DPCD_WRITE {%p}",
                  address.toString(sb), &remoteDpcdWriteMessage);

        parent->messageManager->post(&remoteDpcdWriteMessage, this);
    }
}

void DiscoveryManager::BranchDetection::handleLinkAddressDownReply()
{
    Address::StringBuffer sb;
    DP_USED(sb);

    //
    //  Copy link address results out of the structure
    //    - We cannot process the contents until after
    //      we've programmed the GUID.  The reasoning is
    //      that we need to make sure we do not enumerate
    //      devices not yet in a usable state.
    //
    childCount = linkAddressMessage.resultCount();
    for (unsigned i = 0; i < childCount; i++)
    {
        child[i] = *linkAddressMessage.result(i);

        // also update the portmap
        parentDevice.portMap.internalMap = 0xFF00;  // ports 0x8 to 0xF are internal
        parentDevice.portMap.validMap |= (1 << child[i].portNumber);
        if (child[i].isInputPort)
        {
            parentDevice.peerDevice = child[i].peerDeviceType;
            parentDevice.portMap.inputMap |= (1 << child[i].portNumber);
            if (address == Address(0))
            {
                //
                // For immediate branch device, we will have already read DPCD version
                // in notifyHPD. So we can just use that to populate here. 
                // For the remaining devices, LAM to parent branch will report the child 
                // DPCD version in reply and we are populating it in 
                // BranchDetection::detectCompleted.
                //
                parentDevice.dpcdRevisionMajor = parent->hal->getRevisionMajor();
                parentDevice.dpcdRevisionMinor = parent->hal->getRevisionMinor();
            }
        }
    }

    linkAddressMessage.getGUID(parentDevice.peerGuid);
    if (parentDevice.peerGuid.isGuidZero())
    {
        //
        //     We need to give ourselves a non-zero GUID!
        //
        parent->guidBuilder.makeGuid(parentDevice.peerGuid);

        if (address == Address(0))
        {
            DP_PRINTF(DP_NOTICE, "DP-DM> Setting GUID (locally) for '%s'", address.toString(sb));
            //
            // We're locally connected, use the DPCD HAL to write the new GUID
            //
            if (AuxRetry::ack != parent->hal->setGUID(parentDevice.peerGuid))
            {
                detectCompleted(false);
                return;
            }

            detectCompleted(true);
        }
        else
        {
            //
            //     Let's build a remote DPCD request.  Remember the target is the *parent*
            //   of the device we want to talk to
            //
            Address parentAddress = address;
            parentAddress.pop();
            remoteDpcdWriteMessage.set(parentAddress, address.tail(),
                                       NV_DPCD_GUID, sizeof(GUID),
                                       (NvU8 *)&parentDevice.peerGuid);

            DP_PRINTF(DP_NOTICE, "DP-DM> Setting GUID (remotely) for '%s' sent REMOTE_DPCD_WRITE {%p}",
                     address.toString(sb), &remoteDpcdWriteMessage);

            parent->messageManager->post(&remoteDpcdWriteMessage, this);
        }
    }
    else
    {
        //
        //     Already had a GUID
        //
        detectCompleted(true);
    }

}

void DiscoveryManager::BranchDetection::messageCompleted(MessageManager::Message * from)
{
    if (from == &linkAddressMessage)
        handleLinkAddressDownReply();
    else if (from == &remoteDpcdWriteMessage)
        detectCompleted(true);
}

void DiscoveryManager::SinkDetection::messageCompleted(MessageManager::Message * from)
{
    if (from == &remoteDpcdReadMessage)
        handleRemoteDpcdReadDownReply();
    else if (from == &linkAddressMessage)
        handleLinkAddressDownReply();
    else if (from == &remoteDpcdWriteMessage)
        detectCompleted(true);
}

void DiscoveryManager::BranchDetection::start()
{
    //
    // 1. Create a LINK_ADDRESS_MESSAGE to send to this target so that we can find who he is
    // 2. Create a REMOTE_DPCD_WRITE to set the GUID for this target
    //     *alternatively* we may have to use the local DPCD HAL to write this
    // 3. Enumerate any children that we may wish to queue detect on.
    //
    linkAddressMessage.set(address);

    Address::StringBuffer sb;
    DP_USED(sb);
    DP_PRINTF(DP_NOTICE, "DP-DM> Detecting '%s' (sending LINK_ADDRESS_MESSAGE {%p})",
              address.toString(sb),
              (MessageManager::Message *)&linkAddressMessage);

    parent->messageManager->post(&linkAddressMessage, this);
}

void DiscoveryManager::SinkDetection::start()
{
    //
    // Per DP1.4 requirement:
    // Send PowerUpPhy message first, to make sure device is ready to work
    //
    NakData nakData;
    powerUpPhyMessage.set(address.parent(), address.tail(), NV_TRUE);
    parent->messageManager->send(&powerUpPhyMessage, nakData);

    Address::StringBuffer sb;
    DP_USED(sb);

    // The sink is found in CSN, missing dpcd revision
    if (bFromCSN)
    {
        parent->outstandingSinkDetections.insertBack(this);
        // Create a LINK_ADDRESS_MESSAGE to send to parent of this target
        linkAddressMessage.set(address.parent());

        DP_PRINTF(DP_NOTICE, "DP-DM> Detecting '%s' (sending LINK_ADDRESS_MESSAGE {%p})",
                  address.toString(sb),
                  (MessageManager::Message *)&linkAddressMessage);
        parent->messageManager->post(&linkAddressMessage, this);
    }
    else // The sink is found in LAM sent for branch, and with DPCD rev.
    {
        // Check if the device already has a GUID
        // or it is a dongle or on a logical port ; in which case no GUID is required.
        if ((!device.peerGuid.isGuidZero()) ||
            (device.peerDevice == Dongle)   ||
            (device.dpcdRevisionMajor <= 1 && device.dpcdRevisionMinor < 2) ||
            (device.address.tail() > PHYSICAL_PORT_END))
        {
            parent->addDevice(device);
            delete this;
            return;
        }

        parent->outstandingSinkDetections.insertBack(this);
        Address parentAddress = address.parent();
        remoteDpcdReadMessage.set(parentAddress, address.tail(), NV_DPCD_GUID, sizeof(GUID));

        parent->messageManager->post(&remoteDpcdReadMessage, this);
    }

}

DiscoveryManager::BranchDetection::~BranchDetection()
{
    List::remove(this);

    if (parent->outstandingSinkDetections.isEmpty() &&
        parent->outstandingBranchDetections.isEmpty())
        parent->sink->discoveryDetectComplete();

    parent->timer->cancelCallbacks(this);
}

DiscoveryManager::SinkDetection::~SinkDetection()
{
    List::remove(this);

    if (parent->outstandingSinkDetections.isEmpty() &&
        parent->outstandingBranchDetections.isEmpty())
        parent->sink->discoveryDetectComplete();

    parent->timer->cancelCallbacks(this);
}

void DiscoveryManager::ReceiverSink::messageProcessed(MessageManager::MessageReceiver * from)
{
    DP_ASSERT((from->getRequestId() == 0x2) && "This receiver is only meant for CSNs");

    // CSNs are broadcast messages. So replies will always go to immediate downstream branch
    CsnUpReplyContainer * csnReplyContainer = new CsnUpReplyContainer(parent);
    parent->pendingCsnUpReplies.insertBack(csnReplyContainer);

    //Send acknowledgement to the CSN sender.
    csnReplyContainer->postUpReply();

    ConnStatusNotifyMessage* csnMessage = static_cast<ConnStatusNotifyMessage*>(from);

    if (csnMessage->getUpRequestData()->isInputPort)
    {
        DP_PRINTF(DP_ERROR, "Concentrator?? Got CSN for an upstream port!");
        return;
    }

    Address childAddr;
    DiscoveryManager::Device * oldDevice = parent->findChildDeviceForBranchWithGuid(csnMessage->getUpRequestData()->guid,
                                            csnMessage->getUpRequestData()->port, childAddr);
    if (!csnMessage->getUpRequestData()->devicePlugged)    // some device was unplugged or powered off
    {
        if (oldDevice)
            parent->removeDeviceTree(childAddr);
        return;
    }

    handleCSN(from);
}

void DiscoveryManager::ReceiverSink::handleCSN(MessageManager::MessageReceiver * from)
{
    ConnStatusNotifyMessage* csnMessage = static_cast<ConnStatusNotifyMessage*>(from);

    // There is no point in serving an upRequest when no device is present.
    if (parent->currentDevicesCount == 0)
    {
       DP_ASSERT(0 && "DM> No Device in the Topology");
       return;
    }

    //
    // Check for non-zero GUID in CSN message. It is mandatory to find respective parent
    // Branch should not send CSN with Zero GUID as a unique GUID is set before CSN
    //
    if ((csnMessage->getUpRequestData()->guid).isGuidZero())
    {
       DP_ASSERT(0 && "Ignoring CSN. Invalid parent device due to zero-GUID.");
       return;
    }

    Address childAddr;
    unsigned port = csnMessage->getUpRequestData()->port;
    DiscoveryManager::Device * oldDevice =
        parent->findChildDeviceForBranchWithGuid(csnMessage->getUpRequestData()->guid,
                                                 port,
                                                 childAddr);

    // Check if we already have a device
    if (oldDevice)
    {
        oldDevice->dirty = true;

        // Set the videoSink status of oldDevice again as old device might be a legacy dongle
        // and a video sink is now added with it
        oldDevice->videoSink = ((csnMessage->getUpRequestData()->peerDeviceType == Dongle) ?
                                csnMessage->getUpRequestData()->legacyPlugged : true);

        parent->sink->discoveryNewDevice(*oldDevice);
        return;
    }

    // Exit if no valid address matched for further detection.
    if ((childAddr.size() == 0) ||
        (childAddr.size() > Address::maxHops))
    {
        DP_ASSERT(0 && "Ignoring CSN. Invalid parent device due to GUID not found in discovered topology");
        return;
    }

    DiscoveryManager::Device newDevice;
    newDevice.address = childAddr;
    newDevice.branch = (csnMessage->getUpRequestData()->messagingCapability == true) &&
                       (csnMessage->getUpRequestData()->peerDeviceType == DownstreamBranch);

    newDevice.peerDevice = csnMessage->getUpRequestData()->peerDeviceType;
    newDevice.legacy = csnMessage->getUpRequestData()->legacyPlugged == true;
    newDevice.SDPStreams = newDevice.SDPStreamSinks = 0;

    if (csnMessage->getUpRequestData()->devicePlugged)  // Check for a new device only if it's plugged
    {
        if (newDevice.branch)
        {
            newDevice.videoSink = false;
            // send a LAM and the whole nine yards
            DP_ASSERT(newDevice.legacy == false);
            parent->detectBranch(newDevice);
            return;
        }
        else
        {
            newDevice.SDPStreams = newDevice.SDPStreamSinks = 1;
            newDevice.videoSink = ((csnMessage->getUpRequestData()->peerDeviceType == Dongle) ?
                                    csnMessage->getUpRequestData()->legacyPlugged : true);

            parent->detectSink(newDevice, true);
            return;
        }
    }
}
