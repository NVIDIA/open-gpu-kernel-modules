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

// This file implements the event sink class, which the DisplayPort library
// uses to notify the driver of display devices being connected or
// disconnected.

#include "dp/nvdp-connector-event-sink.h"

#include "nvdp-connector-event-sink.hpp"

#include "nvkms-types.h"
#include "nvkms-dpy.h"
#include "nvkms-utils.h"
#include "nvkms-vrr.h"

#include "nvkms-attributes.h"
#include "nvkms-private.h"

namespace nvkmsDisplayPort {

static void EnableVRR(NVDpyEvoPtr pDpyEvo);

ConnectorEventSink::ConnectorEventSink(NVConnectorEvoPtr pConnectorEvo)
    : pConnectorEvo(pConnectorEvo)
{
}

static NVDpyEvoPtr FindDpyByDevice(NVConnectorEvoPtr pConnectorEvo,
                                   DisplayPort::Device *device)
{
    NVDispEvoPtr pDispEvo = pConnectorEvo->pDispEvo;
    NVDpyEvoPtr pDpyEvo;

    if (nvConnectorUsesDPLib(pConnectorEvo)) {
        FOR_ALL_EVO_DPYS(pDpyEvo, pDispEvo->validDisplays, pDispEvo) {
            if (pDpyEvo->dp.pDpLibDevice &&
                pDpyEvo->dp.pDpLibDevice->device == device) {
                return pDpyEvo;
            }
        }
    }
    return NULL;
}

// Looks for a display that matches the given DP device from
// the list of disconnected dpys.
static NVDpyEvoPtr FindMatchingDisconnectedDpy(NVDispEvoPtr pDispEvo,
                                               NVConnectorEvoPtr pConnectorEvo,
                                               NVDPLibDevicePtr pDpLibDevice)
{
    NVDpyEvoPtr pDpyEvo;

    // A match is simply that the display appears on the same connector.
    // DP MST devices are matched by topology address in nvGetDPMSTDpy.
    const NVDpyIdList dpyIdList =
        nvAddDpyIdToEmptyDpyIdList(pConnectorEvo->displayId);

    FOR_ALL_EVO_DPYS(pDpyEvo, dpyIdList, pDispEvo) {
        if (!pDpyEvo->dp.pDpLibDevice || !pDpyEvo->dp.pDpLibDevice->isPlugged) {
            return pDpyEvo;
        }
    }
    return NULL;
}

const char *nvDPGetDeviceGUIDStr(DisplayPort::Device *device)
{
    DisplayPort::GUID guid;

    if (!device) {
        return NULL;
    }

    guid = device->getGUID();
    if (!guid.isGuidZero()) {
        static DisplayPort::GUID::StringBuffer sb;
        guid.toString(sb);
        return sb;
    }

    return NULL;
}

bool nvDPGetDeviceGUID(DisplayPort::Device *device,
                       NvU8 guidData[DPCD_GUID_SIZE])
{
    DisplayPort::GUID guid;

    if (!device) {
        return false;
    }

    guid = device->getGUID();
    if (guid.isGuidZero()) {
        return false;
    }

    nvkms_memcpy((void*)guidData, (void*)guid.data, sizeof(guid.data));

    return true;
}


static const char *DPGetDevicePortStr(DisplayPort::Device *device,
                                      bool skipLeadingZero)
{
    DisplayPort::Address addr;

    if (!device) {
        return NULL;
    }

    addr = device->getTopologyAddress();
    if (addr.size() > 0) {
        static DisplayPort::Address::StringBuffer sb;
        addr.toString(sb, skipLeadingZero);
        return sb;
    }

    return NULL;
}


static void nvDPPrintDeviceInfo(NVConnectorEvoPtr pConnectorEvo,
                                DisplayPort::Device *device)
{
#if defined(DEBUG)
    NVDispEvoPtr pDispEvo = pConnectorEvo->pDispEvo;
    const char *connectorType;
    unsigned major, minor;
    const char *tmp;

    device->getDpcdRevision(&major, &minor);

    switch (device->getConnectorType()) {
    case DisplayPort::connectorDisplayPort:
        connectorType = "DisplayPort";
        break;

    case DisplayPort::connectorHDMI:
        connectorType = "HDMI";
        break;

    case DisplayPort::connectorDVI:
        connectorType = "DVI";
        break;

    case DisplayPort::connectorVGA:
        connectorType = "VGA";
        break;

    default:
        connectorType = "unknown";
        break;
    }

    nvEvoLogDisp(pDispEvo, EVO_LOG_INFO,
                 "%s-%d: new DisplayPort %d.%d device detected",
                 NvKmsConnectorTypeString(pConnectorEvo->type),
                 pConnectorEvo->typeIndex, major, minor);
    tmp = DPGetDevicePortStr(device, false /* skipLeadingZero */);
    if (tmp) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_INFO,
                     "  Address:     %s", tmp);
    }
    tmp = nvDPGetDeviceGUIDStr(device);
    if (tmp) {
        nvEvoLogDisp(pDispEvo, EVO_LOG_INFO,
                     "  GUID:        {%s}", tmp);
    }
    nvEvoLogDisp(pDispEvo, EVO_LOG_INFO,
                 "  Connector:   %s", connectorType);
    nvEvoLogDisp(pDispEvo, EVO_LOG_INFO,
                 "  Video:       %s", device->isVideoSink() ? "yes" : "no");
    nvEvoLogDisp(pDispEvo, EVO_LOG_INFO,
                 "  Audio:       %s", device->isAudioSink() ? "yes" : "no");
#endif
}

static void nvDPAddDeviceToActiveGroup(NVDpyEvoPtr pDpyEvo)
{
    const NVDPLibConnectorRec *pDpLibConnector =
                               pDpyEvo->pConnectorEvo->pDpLibConnector;
    const NVDevEvoRec *pDevEvo = pDpyEvo->pDispEvo->pDevEvo;
    NvU32 head;

    // If the device is being driven by the firmware group, then we're just
    // tracking it so that it can be shut down by the modeset path, and we
    // don't have any timing information for it.

    if (pDpLibConnector->headInFirmware) {
        return;
    }

    for (head = 0; head < pDevEvo->numHeads; head++) {
        if (nvDpyIdIsInDpyIdList(pDpyEvo->id,
                                 pDpLibConnector->dpyIdList[head])) {
            pDpLibConnector->pGroup[head]->insert(
                pDpyEvo->dp.pDpLibDevice->device);
            break;
        }
    }
}

static bool DpyHasVRREDID(NVDpyEvoPtr pDpyEvo)
{
    return pDpyEvo->parsedEdid.valid &&
           pDpyEvo->parsedEdid.info.nvdaVsdbInfo.valid &&
           // As of this writing, only version 1 is defined.
           pDpyEvo->parsedEdid.info.nvdaVsdbInfo.vsdbVersion == 1 &&
           pDpyEvo->parsedEdid.info.nvdaVsdbInfo.vrrData.v1.supportsVrr;
}

static void EnableVRR(NVDpyEvoPtr pDpyEvo)
{
    NVDispEvoPtr pDispEvo = pDpyEvo->pDispEvo;
    DisplayPort::Device *device = pDpyEvo->dp.pDpLibDevice->device;
    const NvBool dispSupportsVrr = nvDispSupportsVrr(pDispEvo);

    // If the dpy is a laptop internal panel and an SBIOS cookie indicates that
    // it supports VRR, override its enable flag and timeout.  Note that in the
    // internal panel scenario, the EDID may not claim VRR support, so honor
    // hasPlatformCookie even if DpyHasVRREDID() reports FALSE.
    if (pDpyEvo->internal && pDispEvo->vrr.hasPlatformCookie) {

        if (pDispEvo->pDevEvo->hal->caps.supportsDisplayRate) {
            pDpyEvo->vrr.needsSwFramePacing = TRUE;
        }

        pDpyEvo->vrr.type = NVKMS_DPY_VRR_TYPE_GSYNC;
        return;
    }

    // If the DP library already has the monitor VRR-enabled, then we don't need to
    // do it again, but we should still update the minimum refresh rate from the
    // EDID if one is available.
    const bool alreadyEnabled = device->isVrrMonitorEnabled() &&
                                device->isVrrDriverEnabled();

    if (DpyHasVRREDID(pDpyEvo) && !alreadyEnabled) {
        // Perform VRR enablement whenever the monitor supports VRR, but only
        // record it as actually enabled if the rest of the system supports VRR.
        // Other state such as the availability of NV_CTRL_GSYNC_ALLOWED is
        // keyed off of the presence of a dpy with vrr.type !=
        // NVKMS_DPY_VRR_TYPE_NONE.
        if (device->startVrrEnablement() && dispSupportsVrr) {
            pDpyEvo->vrr.type = NVKMS_DPY_VRR_TYPE_GSYNC;
        } else {
            pDpyEvo->vrr.type = NVKMS_DPY_VRR_TYPE_NONE;
        }

        if ((pDpyEvo->vrr.type == NVKMS_DPY_VRR_TYPE_NONE) && dispSupportsVrr) {
            nvEvoLogDisp(pDispEvo, EVO_LOG_WARN,
                         "%s: Failed to initialize G-SYNC",
                         pDpyEvo->name);
        }
    } else if (pDispEvo->pDevEvo->caps.supportsDP13 &&
               device->getIgnoreMSACap()) {
        // DP monitors indicate Adaptive-Sync support through the
        // MSA_TIMING_PAR_IGNORED bit in the DOWN_STREAM_PORT_COUNT register
        // (DP spec 1.4a section 2.2.4.1.1)
        if (dispSupportsVrr) {
            if (nvDpyIsAdaptiveSyncDefaultlisted(pDpyEvo)) {
                pDpyEvo->vrr.type =
                    NVKMS_DPY_VRR_TYPE_ADAPTIVE_SYNC_DEFAULTLISTED;
            } else {
                pDpyEvo->vrr.type =
                    NVKMS_DPY_VRR_TYPE_ADAPTIVE_SYNC_NON_DEFAULTLISTED;
            }
        } else {
            pDpyEvo->vrr.type = NVKMS_DPY_VRR_TYPE_NONE;
        }

        if (pDispEvo->pDevEvo->hal->caps.supportsDisplayRate) {
            pDpyEvo->vrr.needsSwFramePacing = dispSupportsVrr;
        }
    } else {
        // Assign pDpyEvo->vrr.type independent of DpyHasVRREDID(), so that if
        // the monitor is successfully reenabled by the DP library before it
        // calls notifyZombieStateChange(), it'll pick up the correct state.  If
        // reenablement succeeds, the monitor supports VRR even if we haven't
        // read an EDID that says it does yet.
        if (alreadyEnabled && dispSupportsVrr) {
            pDpyEvo->vrr.type = NVKMS_DPY_VRR_TYPE_GSYNC;
        } else {
            pDpyEvo->vrr.type = NVKMS_DPY_VRR_TYPE_NONE;
        }
    }
}

// when we get this event, the DP lib has done link training and the
// EDID has been read (by the DP lib)
void ConnectorEventSink::newDevice(DisplayPort::Device *device)
{
    NVDPLibDevicePtr pDpLibDevice = NULL;
    NVDispEvoPtr pDispEvo = pConnectorEvo->pDispEvo;
    NVDpyEvoPtr pDpyEvo = NULL;
    NvBool dynamicDpyCreated = FALSE;

    // XXX [VM DP MST] Current POR requires we also check/handle:
    //    - More than 64 DP dpys on a connector = print error.
    //    - More than 127 dpys on a system = print error.

    nvDPPrintDeviceInfo(pConnectorEvo, device);

    // Only add video sink devices.
    if (!device->isVideoSink()) {
        return;
    }

    // Protect against redundant newDevices()
    pDpyEvo = FindDpyByDevice(pConnectorEvo, device);
    if (pDpyEvo) {
        nvAssert(!"Got (redundant) DP Lib newDevice() on known display, "
                 "ignoring.");
        return;
    }

    pDpLibDevice = (NVDPLibDevicePtr)nvCalloc(1, sizeof(*pDpLibDevice));
    if (!pDpLibDevice) {
        goto fail;
    }

    nvAssert(!device->getOwningGroup());

    // XXX For DP MST, we'll want to handle dynamic display IDs.  For now,
    // use the connector's display ID.
    pDpLibDevice->device = device;

    if (device->isMultistream()) {
        // Get a dynamic pDpy for this device based on its bus topology path.
        // This will create one if it doesn't exist.
        pDpyEvo = nvGetDPMSTDpyEvo(
            pConnectorEvo,
            DPGetDevicePortStr(device, true /* skipLeadingZero */),
            &dynamicDpyCreated);

    } else {
        // Look for a (previously) disconnected pDpy that matches this device.
        pDpyEvo = FindMatchingDisconnectedDpy(pDispEvo, pConnectorEvo,
                                              pDpLibDevice);
    }

    if (!pDpyEvo) {
        goto fail;
    }

    nvAssert(pDpyEvo->pConnectorEvo == pConnectorEvo);

    // At this point, the pDpy should no longer be tracking a DP lib device.
    if (pDpyEvo->dp.pDpLibDevice) {
        nvAssert(!"DP Lib should have already called lostDevice() for this DP "
                 "device");

        // Call lost device ourselves, if the DP lib calls this again later,
        // we'll ignore it then.
        lostDevice(pDpyEvo->dp.pDpLibDevice->device);
    }

    nvAssert(device->isPlugged());

    pDpLibDevice->isPlugged = TRUE;
    pDpyEvo->dp.pDpLibDevice = pDpLibDevice;

    // If there's an active group that this pDpy is supposed to be a member of,
    // insert it now.
    nvDPAddDeviceToActiveGroup(pDpyEvo);

    if (dynamicDpyCreated) {
        nvSendDpyEventEvo(pDpyEvo, NVKMS_EVENT_TYPE_DYNAMIC_DPY_CONNECTED);
    }

    nvSendDpyEventEvo(pDpyEvo, NVKMS_EVENT_TYPE_DPY_CHANGED);

    return;

 fail:
    nvAssert(pDpyEvo == NULL);
    nvFree(pDpLibDevice);
}

void ConnectorEventSink::lostDevice(DisplayPort::Device *device)
{
    NVDpyEvoPtr pDpyEvo;

    // Ignore non-video sink devices.
    if (!device->isVideoSink()) {
        return;
    }

    pDpyEvo = FindDpyByDevice(pConnectorEvo, device);
    if (!pDpyEvo) {
        nvAssert(!"Got DP Lib lostDevice() on unknown display.");
        return;
    }

    NVDPLibDevicePtr pDpLibDevice = pDpyEvo->dp.pDpLibDevice;
    nvAssert(pDpLibDevice != NULL);

    if (pDpyEvo->vrr.type != NVKMS_DPY_VRR_TYPE_NONE) {
        device->resetVrrEnablement();
        pDpyEvo->vrr.type = NVKMS_DPY_VRR_TYPE_NONE;
    }

    if (device->getOwningGroup()) {
        device->getOwningGroup()->remove(device);
    }

    if (pDpLibDevice->isPlugged) {
        pDpLibDevice->isPlugged = FALSE;
        nvSendDpyEventEvo(pDpyEvo, NVKMS_EVENT_TYPE_DPY_CHANGED);
    }

    if (device->isMultistream()) {
        nvSendDpyEventEvo(pDpyEvo, NVKMS_EVENT_TYPE_DYNAMIC_DPY_DISCONNECTED);
    }

    pDpyEvo->dp.pDpLibDevice = NULL;
    nvFree(pDpLibDevice);
}

void ConnectorEventSink::notifyMustDisconnect(DisplayPort::Group *grp)
{
}

// notifyDetectComplete() is called when DP Library has done a full detect on
// the topology.  There is no one-to-one relationship between a long pulse to
// a detectCompleted.
void ConnectorEventSink::notifyDetectComplete()
{
    pConnectorEvo->detectComplete = TRUE;

    // XXX[DP MST] potentially use this call to notify NV-CONTROL of topology
    // change;

    // issue: not as current as new/lostDevice and may pose sync issues, but
    // less chatty.
}

void ConnectorEventSink::bandwidthChangeNotification(DisplayPort::Device *dev,
                                                     bool isComplianceMode)
{
    nvDPLibUpdateDpyLinkConfiguration(FindDpyByDevice(pConnectorEvo, dev));
}

void ConnectorEventSink::notifyZombieStateChange(DisplayPort::Device *dev,
                                                 bool zombied)
{
    NVDpyEvoPtr pDpyEvo = FindDpyByDevice(pConnectorEvo, dev);
    NvBool sendEvent = FALSE;

    if (pDpyEvo == NULL) {
        return;
    }

    NVDPLibDevicePtr pDpLibDevice = pDpyEvo->dp.pDpLibDevice;
    if (zombied) {
        dev->getOwningGroup()->remove(dev);

        if (pDpLibDevice->isPlugged && !dev->isPlugged()) {
            pDpLibDevice->isPlugged = FALSE;
            sendEvent = TRUE;
        }

        // Don't reset VRR enablement here.  Though normally NVKMS initiates VRR
        // enablement, the DP library needs to initiate VRR re-enablement of a
        // zombie device itself before performing link training or else the
        // monitor might remain blank if a VRR stream is active when it's
        // plugged back in.
    } else {
        if (!pDpLibDevice->isPlugged && dev->isPlugged()) {
            pDpLibDevice->isPlugged = TRUE;
            sendEvent = TRUE;
        }

        // Determine whether the DP library reenabled VRR on this display.
        EnableVRR(pDpyEvo);

        nvDPAddDeviceToActiveGroup(pDpyEvo);
    }

    if (sendEvent) {
        nvSendDpyEventEvo(pDpyEvo, NVKMS_EVENT_TYPE_DPY_CHANGED);
    }
}

void ConnectorEventSink::notifyCableOkStateChange(DisplayPort::Device *dev,
                                                  bool cableOk)
{
}

void ConnectorEventSink::notifyHDCPCapDone(DisplayPort::Device *dev,
                                                      bool hdcpCap)
{
}

void ConnectorEventSink::notifyMCCSEvent(DisplayPort::Device *dev)
{
}

}; // namespace nvkmsDisplayPort

// The functions below are exported to the rest of nvkms.  Declare them outside
// of the 'nvkmsDisplayPort' namespace.  Their prototypes in
// nvdp-connector-event-sink.h are declared as extern "C".

NvBool nvDPLibDpyIsConnected(NVDpyEvoPtr pDpyEvo)
{
    nvAssert(nvConnectorUsesDPLib(pDpyEvo->pConnectorEvo));

    return ((pDpyEvo->dp.pDpLibDevice != NULL) &&
            pDpyEvo->dp.pDpLibDevice->isPlugged);
}

// Adaptive-Sync is enabled/disabled by setting the MSA_TIMING_PAR_IGNORE_EN
// bit in the DOWNSPREAD_CTRL register (DP spec 1.4a appendix K)
void nvDPLibSetAdaptiveSync(const NVDispEvoRec *pDispEvo, NvU32 head,
                            NvBool enable)
{
    const NVConnectorEvoRec *pConnectorEvo =
        pDispEvo->headState[head].pConnectorEvo;
    NVDPLibConnectorPtr pDpLibConnector = pConnectorEvo->pDpLibConnector;
    DisplayPort::Group *pGroup = pDpLibConnector->pGroup[head];
    DisplayPort::Device *dev;

    for (dev = pGroup->enumDevices(0); dev != NULL;
            dev = pGroup->enumDevices(dev)) {
        dev->setIgnoreMSAEnable(enable);
    }
}

// Read the link configuration from the connector and stores it in the pDpy so
// it can be sent to clients via NV-CONTROL.  Also generate events if the values
// change.
void nvDPLibUpdateDpyLinkConfiguration(NVDpyEvoPtr pDpyEvo)
{
    if (!pDpyEvo) {
        return;
    }

    NVDPLibDevicePtr pDpLibDevice = pDpyEvo->dp.pDpLibDevice;
    DisplayPort::Device *dev = pDpLibDevice ? pDpLibDevice->device : NULL;
    DisplayPort::Connector *connector =
        pDpyEvo->pConnectorEvo->pDpLibConnector->connector;
    unsigned laneCount;
    NvU64 linkRate;
    NvU64 linkRate10MHz;
    enum NvKmsDpyAttributeDisplayportConnectorTypeValue connectorType;
    NvBool sinkIsAudioCapable;

    if (!dev || !pDpLibDevice->isPlugged) {
        linkRate = 0;
        linkRate10MHz = 0;
        laneCount = NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_LANE_COUNT_1;
        connectorType = NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_CONNECTOR_TYPE_UNKNOWN;
        sinkIsAudioCapable = FALSE;
    } else {
        // XXX[AGP]: Can the path down to a single device have a different link
        // configuration from the connector itself?
        connector->getCurrentLinkConfig(laneCount, linkRate);

        // The DisplayPort library multiplies the link rate enum value by
        // 27000000.  Convert back to NV-CONTROL's defines.
        linkRate /= 27000000;
        linkRate10MHz = linkRate * 27;

        nvkmsDisplayPort::EnableVRR(pDpyEvo);

        switch (pDpLibDevice->device->getConnectorType()) {
        case DisplayPort::connectorDisplayPort:
            connectorType = NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_CONNECTOR_TYPE_DISPLAYPORT;
            break;
        case DisplayPort::connectorHDMI:
            connectorType = NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_CONNECTOR_TYPE_HDMI;
            break;
        case DisplayPort::connectorDVI:
            connectorType = NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_CONNECTOR_TYPE_DVI;
            break;
        case DisplayPort::connectorVGA:
            connectorType = NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_CONNECTOR_TYPE_VGA;
            break;
        default:
            connectorType = NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_CONNECTOR_TYPE_UNKNOWN;
            break;
        }

        sinkIsAudioCapable = pDpLibDevice->device->isAudioSink();
    }

    // The DisplayPort library reports a disabled link as 0 lanes.  NV-CONTROL,
    // for historical reasons, uses a setting of "1 lane @ disabled" for a
    // disabled link, so translate to that.
    if (laneCount == 0) {
        linkRate = 0;
        laneCount = NV0073_CTRL_CMD_DP_GET_LINK_CONFIG_LANE_COUNT_1;
    }

    // Update pDpy and send events if anything changed.
    if (laneCount != pDpyEvo->dp.laneCount) {
        pDpyEvo->dp.laneCount = laneCount;
        nvSendDpyAttributeChangedEventEvo(pDpyEvo,
                          NV_KMS_DPY_ATTRIBUTE_DIGITAL_LINK_TYPE,
                          nvRMLaneCountToNvKms(laneCount));
    }

    if (linkRate != pDpyEvo->dp.linkRate) {
        pDpyEvo->dp.linkRate = linkRate;
        nvSendDpyAttributeChangedEventEvo(pDpyEvo,
                          NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_LINK_RATE,
                          linkRate);
    }

    if (linkRate10MHz != pDpyEvo->dp.linkRate10MHz) {
        pDpyEvo->dp.linkRate10MHz = linkRate10MHz;
        nvSendDpyAttributeChangedEventEvo(pDpyEvo,
                          NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_LINK_RATE_10MHZ,
                          linkRate10MHz);
    }

    if (connectorType != pDpyEvo->dp.connectorType) {
        pDpyEvo->dp.connectorType = connectorType;
        nvSendDpyAttributeChangedEventEvo(pDpyEvo,
                          NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_CONNECTOR_TYPE,
                          connectorType);
    }

    if (sinkIsAudioCapable != pDpyEvo->dp.sinkIsAudioCapable) {
        pDpyEvo->dp.sinkIsAudioCapable = sinkIsAudioCapable;
        nvSendDpyAttributeChangedEventEvo(pDpyEvo,
                          NV_KMS_DPY_ATTRIBUTE_DISPLAYPORT_SINK_IS_AUDIO_CAPABLE,
                          sinkIsAudioCapable);
    }
}
