/*
 * SPDX-FileCopyrightText: Copyright (c) 2011 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#ifndef __NVDP_CONNECTOR_EVENT_SINK_HPP__
#define __NVDP_CONNECTOR_EVENT_SINK_HPP__

#include <nvtypes.h>
#include <dp_connector.h>

#include "nvdp-evo-interface.hpp"


namespace nvkmsDisplayPort
{

class ConnectorEventSink : public DisplayPort::Object,
        public DisplayPort::Connector::EventSink
{
private:
    const NVConnectorEvoPtr pConnectorEvo;

public:
    ConnectorEventSink(NVConnectorEvoPtr pConnectorEvo);

    // From DisplayPort::Connector::EventSink
    virtual void newDevice(DisplayPort::Device *dev);
    virtual void lostDevice(DisplayPort::Device *dev);
    virtual void notifyMustDisconnect(DisplayPort::Group *grp);
    virtual void notifyDetectComplete();
    virtual void bandwidthChangeNotification(DisplayPort::Device *dev, bool isComplianceMode);
    virtual void notifyZombieStateChange(DisplayPort::Device *dev, bool zombied);
    virtual void notifyCableOkStateChange(DisplayPort::Device *dev, bool cableOk);
    virtual void notifyHDCPCapDone(DisplayPort::Device *dev, bool hdcpCap);
    virtual void notifyMCCSEvent(DisplayPort::Device *dev);
};

const char *nvDPGetDeviceGUIDStr(DisplayPort::Device *device);
bool nvDPGetDeviceGUID(DisplayPort::Device *device, NvU8 guid[DPCD_GUID_SIZE]);

}; // namespace nvkmsDisplayPort

struct _nv_dplibconnector {
    DisplayPort::Connector *connector;
    nvkmsDisplayPort::EvoInterface *evoInterface;
    nvkmsDisplayPort::ConnectorEventSink *evtSink;
    DisplayPort::MainLink *mainLink;
    DisplayPort::AuxBus *auxBus;

    NvBool isActive;

    // The VBIOS head is actively driving this connector.
    bool headInFirmware;
    NVConnectorEvoRec *pConnectorEvo;
    // Per-head DpLib group, allocated at the time of connector creation:
    //   In case of multi-streaming, multiple heads can be attached to single
    //   DP connector driving distinct DP streams.
    DisplayPort::Group *pGroup[NVKMS_MAX_HEADS_PER_DISP];
    NVDpyIdList dpyIdList[NVKMS_MAX_HEADS_PER_DISP];
    // Attached heads bitmask
    NvU32 headMask;

    // Connection status plugged/unplugged; gets initialized by
    // Connector::resume() and gets updated by
    // Connector::notifyLongPulse().
    NvBool plugged;

    // Indicates whether the HDMI/DVI half of the connector is active
    // If so link is being driven by HDMI/DVI and avoid LT etc on DP
    // link of the connector
    NvBool linkHandoffEnabled;
};

struct _nv_dplibdevice {
    DisplayPort::Device *device;
    NvBool isPlugged;
};

struct __nv_dplibmodesetstate {
    NVDpyIdList dpyIdList;
    DisplayPort::DpModesetParams modesetParams;
};

#endif // __NVDP_CONNECTOR_EVENT_SINK_HPP__
