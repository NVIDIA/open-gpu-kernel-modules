/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/******************************* DisplayPort********************************\
*                                                                           *
* Module: dp_groupimpl.cpp                                                  *
*    DP device group implementation                                         *
*                                                                           *
\***************************************************************************/

#include "dp_internal.h"
#include "dp_connector.h"
#include "dp_list.h"
#include "dp_auxdefs.h"
#include "dp_deviceimpl.h"
#include "dp_groupimpl.h"
#include "dp_connectorimpl.h"
#include "dp_printf.h"

using namespace DisplayPort;

void GroupImpl::update(Device * dev, bool allocationState)
{
    Address::StringBuffer sb;
    Address devAddress = dev->getTopologyAddress();
    DP_USED(sb);

    // Do not map a stream that is not yet turned on in the gpu. An update shall be sent later during NAE.
    if (allocationState && !this->isHeadAttached())
        return;

    //
    // Do not enable the stream on an unplugged device but take care of
    // detached devices. We need to clear PBNs allocated by such devices
    //
    if (allocationState && !((DeviceImpl *)dev)->plugged)
        return;

    //
    // Check if Parent's messageManager exist or not. This is required for cases
    // where parent branch itself has been unplugged. No message can be sent in this case.
    //
    if (!parent->messageManager)
        return;

    if (timeslot.count == 0 ||
        ((DeviceImpl *)dev)->payloadAllocated == allocationState)
        return;

    if (!dev->getParent() || !((dev->getParent())->isPlugged()))
    {
        DeviceImpl * parentDev = NULL;

        //
        // Send ALLOCATE_PAYLOAD with pbn 0 to parent port of previous branch
        // Find first plugged parent branch & send message to it
        //
        while(devAddress.size() > 2)
        {
            devAddress.pop();
            parentDev = parent->findDeviceInList(devAddress.parent());

            if (parentDev && parentDev->isPlugged())
                break;
        }

        // If no parent found simply return as we don't have a valid address to send message
        if (!parentDev)
            return;
    }

    NakData nakData;
    for (int retries = 0 ; retries < 7; retries++)
    {
        AllocatePayloadMessage allocate;
        unsigned sink = 0;    // hardcode the audio sink to 0th in the device.
        allocate.set(devAddress.parent(), devAddress.tail(),
                     dev->isAudioSink() ? 1 : 0, streamIndex, allocationState ? timeslot.PBN : 0,
                     &sink, true);

        // Trigger a refetch of epr
        ((DeviceImpl *)dev)->bandwidth.enum_path.dataValid = false;
        DeviceImpl * tail = (DeviceImpl *) dev;
        while (tail && tail->getParent())
        {
            tail->bandwidth.enum_path.dataValid = false;
            tail = (DeviceImpl *)tail->getParent();
        }

        if (parent->messageManager->send(&allocate, nakData))
        {
            if (allocationState)
            {
                DP_PRINTF(DP_NOTICE, "DP-TM> Attached stream:%d to %s", streamIndex, dev->getTopologyAddress().toString(sb));
            }
            else
            {
                DP_PRINTF(DP_NOTICE, "DP-TM> Detached stream:%d from %s", streamIndex, dev->getTopologyAddress().toString(sb));
            }

            ((DeviceImpl *)dev)->payloadAllocated = allocationState;

            return;
        }
    }

    // we should not have ideally reached here unless allocate payload failed.
    if (allocationState)
    {
        DP_PRINTF(DP_ERROR, "DP-TM> Allocate_payload: Failed to ATTACH stream:%d to %s", streamIndex, dev->getTopologyAddress().toString(sb));
        DP_ASSERT(0);
    }
    else
    {
        DP_PRINTF(DP_ERROR, "DP-TM> Allocate_payload: Failed to DETACH stream:%d from %s", streamIndex, dev->getTopologyAddress().toString(sb));
        DP_ASSERT(0);
    }

}

void GroupImpl::insert(Device * dev)
{
    DP_ASSERT(!headInFirmware && "Cannot add or remove from a firmware group.  You must perform a modeset away from the device");
    DeviceImpl * di = (DeviceImpl *)dev;

    if (isHeadAttached())
    {
        if (di->activeGroup && di->activeGroup != this)
        {
            DP_ASSERT(0 && "Device already in active group, cannot add to another active group!");
            return;
        }
        di->activeGroup = this;
    }

    members.insertFront(di);

    update(dev, true);

}

void GroupImpl::remove(Device * dev)
{
     DP_ASSERT(!headInFirmware && "Cannot add or remove from a firmware group.  You must perform a modeset away from the device");

     DeviceImpl * di = (DeviceImpl *)dev;

    if (isHeadAttached())
    {
        /*
         * The device may become lost and free after removal from the active
         * group. Therefore, also remove the device from the
         * 'dscEnabledDevices' list and ensure that its dangling pointer is not
         * left behind.
         */
        if (parent->dscEnabledDevices.contains(dev)) {
            parent->dscEnabledDevices.remove(dev);
        }

        di->activeGroup = 0;
    }
    members.remove(di);

    update(dev, false);

    updateVbiosScratchRegister(dev);
}

void GroupImpl::destroy()
{
    ConnectorImpl* parent = NULL;
    for (Device * i = enumDevices(0); i; i = enumDevices(i))
        remove(i);

    // Cancel any queue the auth callback.
    cancelHdcpCallbacks();

    parent = this->parent;

    if (parent)
    {
        if (!parent->activeGroups.isEmpty())
        {
            for (ListElement * i = parent->activeGroups.begin(); i != parent->activeGroups.end(); i = i->next)
            {
                GroupImpl * group = (GroupImpl *)i;
                if (group == this)
                {
                    parent->activeGroups.remove(this);
                    break;
                }
            }
        }


        if (!parent->inactiveGroups.isEmpty())
        {
            for (ListElement * i = parent->inactiveGroups.begin(); i != parent->inactiveGroups.end(); i = i->next)
            {
                GroupImpl * group = (GroupImpl *)i;
                if (group == this)
                {
                    parent->inactiveGroups.remove(this);
                    break;
                }
            }
        }

        if (parent->intransitionGroups.contains(this))
        {
            parent->intransitionGroups.remove(this);
        }

        if (parent->addStreamMSTIntransitionGroups.contains(this))
        {
            parent->addStreamMSTIntransitionGroups.remove(this);
        }
    }

    delete this;
}

void GroupImpl::cancelHdcpCallbacks()
{
    authRetries = 0;

    parent->timer->cancelCallback(this, &tagHDCPReauthentication);
    parent->timer->cancelCallback(this, &tagStreamValidation);

}

Device * GroupImpl::enumDevices(Device * previousDevice)
{
    return members.next(previousDevice);
}

void GroupImpl::expired(const void * tag)
{
    if (tag == &tagHDCPReauthentication)
    {
        HDCPState hdcpState = {0};
        parent->main->configureHDCPGetHDCPState(hdcpState);

        if (authRetries < HDCP_AUTHENTICATION_RETRIES)
        {
            this->hdcpEnabled = hdcpState.HDCP_State_Encryption;
            if (hdcpState.HDCP_State_Authenticated)
            {
                parent->isHDCPAuthOn = true;
                authRetries = 0;
            }
            else
            {
                unsigned authDelay = (hdcpState.HDCP_State_22_Capable ?
                    HDCP22_AUTHENTICATION_COOLDOWN : HDCP_AUTHENTICATION_COOLDOWN);

                authRetries++;
                parent->main->configureHDCPRenegotiate();
                parent->isHDCPAuthOn = false;
                parent->timer->queueCallback(this, &tagHDCPReauthentication,
                                             authDelay);
            }
        }
        else
        {
            parent->isHDCPAuthOn = this->hdcpEnabled = false;
        }
    }
    else if ( tag == &tagStreamValidation)
    {
       if (!(this->streamValidationDone))
       {
          // If we are here we need to debug what has caused the problem for not getting notification from DD.
          DP_ASSERT(0 && "DP> Didn't get final notification." );
       }
    }
}

bool GroupImpl::hdcpGetEncrypted()
{
    //
    // Returns whether encryption is currently enabled
    //
    if (parent->isHDCPAuthOn)
    {
        return this->hdcpEnabled;
    }
    else
    {
        return false;
    }
}

void GroupImpl::updateVbiosScratchRegister(Device * lastDev)
{
    if (!parent->bDisableVbiosScratchRegisterUpdate &&
         parent->lastDeviceSetForVbios == lastDev)
    {
        // Take a device which is part of a group
        for (ListElement * e = parent->deviceList.begin();
             e != parent->deviceList.end(); e = e->next)
        {
            DeviceImpl * dev  = (DeviceImpl *)e;

            if (dev->activeGroup && dev->activeGroup->isHeadAttached())
            {
                NvU32 address  = 0;
                NvU32 addrSize = dev->getTopologyAddress().size();

                // Set the MS_SCRATCH_REGISTER for lighted up display
                for (NvU32 i = addrSize; i; --i)
                {
                    address |= ((dev->address[i-1] & 0xF) << ((addrSize - i)*4));
                }

                parent->main->configureMsScratchRegisters(address, addrSize, 3);

                parent->lastDeviceSetForVbios = (Device *)dev;

                return;
            }
        }
    }
}

//
// Helper function for attaching and detaching heads.
//
// For attach, we will assert if group already has head attached but for
// some device in the group, active group did not point to current group.
// For detach, we will assert if the group does not have head attached but
// some device in group has an active group OR head is marked attached but
// not all devies in the group have the current group as active group.
// This also sets or clears dev->activeGroup for each contained
// device.
//
void GroupImpl::setHeadAttached(bool attached)
{
    for (Device * i = enumDevices(0); i; i = enumDevices(i))
    {
        DeviceImpl *di = (DeviceImpl *)i;

        if (attached)
        {
            if (headAttached)
            {
                DP_ASSERT(di->activeGroup == this);
            }
            di->activeGroup = this;
        }
        else
        {
            if (!headAttached)
            {
                DP_ASSERT(di->activeGroup == NULL);
            }
            else
            {
                DP_ASSERT(di->activeGroup == this);
            }
            di->activeGroup = NULL;
        }
    }
    headAttached = attached;
}
