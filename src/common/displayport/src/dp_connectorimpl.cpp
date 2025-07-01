/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
* Module: dp_connectorimpl.cpp                                              *
*    DP connector implementation                                            *
*                                                                           *
\***************************************************************************/

#include "dp_internal.h"
#include "dp_guid.h"
#include "dp_configcaps.h"
#include "dp_list.h"
#include "dp_buffer.h"
#include "dp_auxdefs.h"
#include "dp_watermark.h"
#include "dp_edid.h"
#include "dp_discovery.h"
#include "dp_groupimpl.h"
#include "dp_deviceimpl.h"
#include "dp_connectorimpl.h"
#include "dp_printf.h"

#include "dp_auxbus.h"
#include "dpringbuffertypes.h"

#include "dp_connectorimpl2x.h"

#include "ctrl/ctrl0073/ctrl0073dfp.h"
#include "ctrl/ctrl0073/ctrl0073dp.h"
#include "dp_tracing.h"

using namespace DisplayPort;

ConnectorImpl::ConnectorImpl(MainLink * main, AuxBus * auxBus, Timer * timer, Connector::EventSink * sink)
    : main(main),
      auxBus(auxBus),
      timer(timer),
      sink(sink),
      cachedSourceOUI(0),
      bOuiCached(false),
      bIgnoreSrcOuiHandshake(false),
      linkPolicy(),
      linkGuessed(false),
      isLinkQuiesced(false),
      bNoLtDoneAfterHeadDetach(false),
      isDP12AuthCap(false),
      isHDCPAuthOn(false),
      isHDCPReAuthPending(false),
      isHDCPAuthTriggered(false),
      isHopLimitExceeded(false),
      isDiscoveryDetectComplete(false),
      bDeferNotifyLostDevice(false),
      hdcpValidateData(),
      authRetries(0),
      retryLT(0),
      hdcpCapsRetries(0),
      hdcpCpIrqRxStatusRetries(0),
      bFromResumeToNAB(false),
      bAttachOnResume(false),
      bHdcpAuthOnlyOnDemand(false),
      constructorFailed(false),
      policyModesetOrderMitigation(false),
      policyForceLTAtNAB(false),
      policyAssessLinkSafely(false),
      bDisableVbiosScratchRegisterUpdate(false),
      modesetOrderMitigation(false),
      compoundQueryActive(false),
      compoundQueryResult(false),
      compoundQueryCount(0),
      messageManager(0),
      discoveryManager(0),
      numPossibleLnkCfg(0),
      linkAwaitingTransition(false),
      linkState(DP_TRANSPORT_MODE_INIT),
      bAudioOverRightPanel(false),
      connectorActive(false),
      firmwareGroup(0),
      bAcpiInitDone(false),
      bIsUefiSystem(false),
      bSkipLt(false),
      bMitigateZombie(false),
      bDelayAfterD3(false),
      bKeepOptLinkAlive(false),
      bNoFallbackInPostLQA(false),
      LT2FecLatencyMs(0),
      bFECEnable(false),
      bDscCapBasedOnParent(false),
      allocatedDpTunnelBw(0),
      inTransitionHeadMask(0x0),
      ResStatus(this)
{
    clearTimeslices();
    firmwareGroup = createFirmwareGroup();

    if (firmwareGroup == NULL)
    {
        constructorFailed = true;
        return;
    }

    main->queryGPUCapability();
    main->queryAndUpdateDfpParams();
    hal = MakeDPCDHAL(auxBus, timer, main);
    if (hal == NULL)
    {
        constructorFailed = true;
        return;
    }

    hal->setPC2Disabled(main->isPC2Disabled());

    //
    // If a GPU is DP1.2 or DP1.4 supported then set these capalibilities.
    // This is used for accessing DP1.2/DP1.4 specific register space & features
    //
    hal->setGpuDPSupportedVersions(main->getGpuDpSupportedVersions());

    // Set if GPU supports FEC. Check panel FEC caps only if GPU supports it.
    hal->setGpuFECSupported(main->isFECSupported());

    // Set if LTTPR training is supported per regKey
    hal->setLttprSupported(main->isLttprSupported());

    const DP_REGKEY_DATABASE& dpRegkeyDatabase = main->getRegkeyDatabase();
    this->applyRegkeyOverrides(dpRegkeyDatabase);
    hal->applyRegkeyOverrides(dpRegkeyDatabase);

    hal->setConnectorTypeC(main->isConnectorUSBTypeC());
    highestAssessedLC = initMaxLinkConfig();
}

void ConnectorImpl::applyRegkeyOverrides(const DP_REGKEY_DATABASE& dpRegkeyDatabase)
{
    DP_ASSERT(dpRegkeyDatabase.bInitialized &&
              "All regkeys are invalid because dpRegkeyDatabase is not initialized!");

    this->bSkipAssessLinkForEDP = dpRegkeyDatabase.bAssesslinkForEdpSkipped;

    // If Hdcp authenticatoin on demand regkey is set, override to the provided value.
    this->bHdcpAuthOnlyOnDemand = dpRegkeyDatabase.bHdcpAuthOnlyOnDemand;

    if (dpRegkeyDatabase.bOptLinkKeptAlive)
    {
        this->bKeepLinkAliveMST = true;
        this->bKeepLinkAliveSST = true;
    }
    else
    {
        this->bKeepLinkAliveMST = dpRegkeyDatabase.bOptLinkKeptAliveMst;
        this->bKeepLinkAliveSST = dpRegkeyDatabase.bOptLinkKeptAliveSst;
    }
    this->bReportDeviceLostBeforeNew       = dpRegkeyDatabase.bReportDeviceLostBeforeNew;
    this->bDisableSSC                      = dpRegkeyDatabase.bSscDisabled;
    this->bEnableFastLT                    = dpRegkeyDatabase.bFastLinkTrainingEnabled;
    this->bDscMstCapBug3143315             = dpRegkeyDatabase.bDscMstCapBug3143315;
    this->bPowerDownPhyBeforeD3            = dpRegkeyDatabase.bPowerDownPhyBeforeD3;
    if (dpRegkeyDatabase.applyMaxLinkRateOverrides)
    {
        this->maxLinkRateFromRegkey        = hal->mapLinkBandiwdthToLinkrate(dpRegkeyDatabase.applyMaxLinkRateOverrides); // BW to linkrate
    }
    this->bForceDisableTunnelBwAllocation  = dpRegkeyDatabase.bForceDisableTunnelBwAllocation;
    this->bSkipZeroOuiCache                = dpRegkeyDatabase.bSkipZeroOuiCache;
    this->bForceHeadShutdownFromRegkey     = dpRegkeyDatabase.bForceHeadShutdown;
    this->bEnableLowerBppCheckForDsc       = dpRegkeyDatabase.bEnableLowerBppCheckForDsc;
    this->bDisableEffBppSST8b10b            = dpRegkeyDatabase.bDisableEffBppSST8b10b;
}

void ConnectorImpl::setPolicyModesetOrderMitigation(bool enabled)
{
    policyModesetOrderMitigation = enabled;
}

void ConnectorImpl::setPolicyForceLTAtNAB(bool enabled)
{
    policyForceLTAtNAB = enabled;
}

void ConnectorImpl::setPolicyAssessLinkSafely(bool enabled)
{
    policyAssessLinkSafely = enabled;
}

//
// This function is to re-read remote HDCP BKSV and BCAPS.
//
// Function is added for DP1.2 devices which don't have valid BKSV at HPD and
// make BKSV available after Payload Ack.
//
void ConnectorImpl::readRemoteHdcpCaps()
{
    if (hdcpCapsRetries)
    {
        fireEvents();
        return;
    }

}

void ConnectorImpl::discoveryDetectComplete()
{
    fireEvents();
    // no outstanding EDID reads and branch/sink detections for MST
    if (pendingEdidReads.isEmpty() &&
        (!discoveryManager ||
         (discoveryManager->outstandingBranchDetections.isEmpty() &&
          discoveryManager->outstandingSinkDetections.isEmpty())))
    {
        bDeferNotifyLostDevice = false;
        isDiscoveryDetectComplete = true;
        bIsDiscoveryDetectActive = false;

        // Complete detection and see if can enter power saving state.
        isNoActiveStreamAndPowerdown();

        fireEvents();
    }
}

void ConnectorImpl::applyEdidWARs(Edid & edid, DiscoveryManager::Device & device)
{
    DpMonitorDenylistData *pDenylistData = new DpMonitorDenylistData();
    NvU32 warFlag = 0;
    warFlag = main->monitorDenylistInfo(edid.getManufId(), edid.getProductId(), pDenylistData);

    // Apply any edid overrides if required
    edid.applyEdidWorkArounds(warFlag, pDenylistData);
    this->handleEdidWARs(edid, device);

    delete pDenylistData;
}

void DisplayPort::DevicePendingEDIDRead::mstEdidCompleted(EdidReadMultistream * from)
{
    Address::StringBuffer sb;
    DP_USED(sb);
    DP_PRINTF(DP_NOTICE, "DP-CONN> Edid read complete: %s %s",
              from->topologyAddress.toString(sb),
              from->edid.getName());
    ConnectorImpl * connector = parent;
    parent->applyEdidWARs(from->edid, device);
    parent->processNewDevice(device, from->edid, true, DISPLAY_PORT, RESERVED);
    delete this;
    connector->discoveryDetectComplete();
}

void DisplayPort::DevicePendingEDIDRead::mstEdidReadFailed(EdidReadMultistream * from)
{
    Address::StringBuffer sb;
    DP_USED(sb);
    DP_PRINTF(DP_ERROR, "DP-CONN> Edid read failed: %s (using fallback)",
               from->topologyAddress.toString(sb));
    ConnectorImpl * connector = parent;
    parent->processNewDevice(device, Edid(), true, DISPLAY_PORT, RESERVED);
    delete this;
    connector->discoveryDetectComplete();
}

void ConnectorImpl::messageProcessed(MessageManager::MessageReceiver * from)
{
    if (from == &ResStatus)
    {
        for (Device * i = enumDevices(0); i; i = enumDevices(i))
            if (i->getGUID() == ResStatus.request.guid)
            {
                DeviceImpl * child = ((DeviceImpl *)i)->children[ResStatus.request.port];
                if (child)
                {
                    child->resetCacheInferredLink();
                    sink->bandwidthChangeNotification((DisplayPort::Device*)child, false);
                    return;
                }

                break;
            }

        // Child wasn't found... Invalidate all bandwidths on topology
        for (Device * i = enumDevices(0); i; i = enumDevices(i)) {
            ((DeviceImpl *)i)->resetCacheInferredLink();
        }
    }
    else
        DP_ASSERT(0 && "Received unexpected upstream message that we AREN'T registered for");
}

void ConnectorImpl::discoveryNewDevice(const DiscoveryManager::Device & device)
{
    //
    //  We're guaranteed that there isn't already a device on the list with the same
    //  address.  If we receive the same device announce again - it is considered
    //  a notification that the device underlying may have seen an HPD.
    //
    //  We're going to queue an EDID read, and remember which device we did it on.
    //  If the EDID comes back different we'll have mark the old device object
    //  as disconnected - and create a new one.  This is required because
    //  EDID is one of the fields considered to be immutable.
    //

    if (!device.branch)
    {
        if (!device.videoSink)
        {
            // Don't read EDID on a device having no videoSink
            processNewDevice(device, Edid(), false, DISPLAY_PORT, RESERVED);
            return;
        }
        pendingEdidReads.insertBack(new DevicePendingEDIDRead(this, messageManager, device));
    }
    else
    {
        // Don't try to read the EDID on a branch device
        processNewDevice(device, Edid(), true, DISPLAY_PORT, RESERVED);
    }
}

void ConnectorImpl::processNewDevice(const DiscoveryManager::Device & device,
                                     const Edid & edid,
                                     bool isMultistream,
                                     DwnStreamPortType portType,
                                     DwnStreamPortAttribute portAttribute,
                                     bool isCompliance)
{
    //
    // Ideally we should read EDID here. But instead just report the device
    //   try to find device in list of devices
    //
    DeviceImpl * existingDev = findDeviceInList(device.address);
    if (existingDev)
        existingDev->resetCacheInferredLink();

    //
    //  Process fallback EDID
    //
    Edid processedEdid = edid;

    if (!edid.getEdidSize()   || !edid.isChecksumValid() || !edid.isValidHeader() ||
        edid.isPatchedChecksum())
    {
        if (portType == WITHOUT_EDID)
        {
            switch(portAttribute)
            {
            case RESERVED:
            case IL_720_480_60HZ:
            case IL_720_480_50HZ:
            case IL_1920_1080_60HZ:
            case IL_1920_1080_50HZ:
            case PG_1280_720_60HZ:
            case PG_1280_720_50_HZ:
                DP_ASSERT(0 && "Default EDID feature not supported!");
                break;
            }

        }
        if (portType == ANALOG_VGA)
            makeEdidFallbackVGA(processedEdid);
        else
        {
            makeEdidFallback(processedEdid, hal->getVideoFallbackSupported());
        }
    }

    //
    //  Process caps
    //
    bool   hasAudio = device.SDPStreams && device.SDPStreamSinks;
    bool   hasVideo = device.videoSink;
    NvU64  maxTmdsClkRate = 0U;
    ConnectorType connector = connectorDisplayPort;

    if (portType == DISPLAY_PORT_PLUSPLUS || portType == DVI || portType == HDMI)
    {
        maxTmdsClkRate = device.maxTmdsClkRate;
    }

    switch(portType)
    {
        case DISPLAY_PORT:
        case DISPLAY_PORT_PLUSPLUS: // DP port that supports DP and TMDS
            connector = connectorDisplayPort;
            break;

        case ANALOG_VGA:
            connector = connectorVGA;
            break;

        case DVI:
            connector = connectorDVI;
            break;

        case HDMI:
            connector = connectorHDMI;
            break;

        case WITHOUT_EDID:
            connector = connectorDisplayPort;
            break;
    }

    // Dongle in SST mode.
    if ((device.peerDevice == Dongle) && (device.address.size() == 0))
        hasAudio = hasVideo = false;

    if (device.branch)
        hasAudio = hasVideo = false;

    if (!existingDev)
        goto create;

    if (isCompliance && (existingDev->processedEdid == processedEdid))
    {
        // unzombie the old device
    }
    else if (existingDev->audioSink != hasAudio ||
             existingDev->videoSink != hasVideo ||
             existingDev->rawEDID != edid ||
             existingDev->processedEdid != processedEdid ||
             existingDev->connectorType != connector ||
             existingDev->multistream != isMultistream ||
             existingDev->complianceDeviceEdidReadTest != isCompliance ||
             existingDev->maxTmdsClkRate != maxTmdsClkRate ||
             (existingDev->address.size() > 1 && !existingDev->getParent()) ||
             // If it is an Uninitialized Mux device, goto create so that we can properly
             // initialize the device and all its caps
             existingDev->isFakedMuxDevice())
        goto create;

    // Complete match, make sure its marked as plugged
    existingDev->plugged = true;
     if (existingDev->isActive())
        existingDev->activeGroup->update(existingDev, true);


    fireEvents();
    return;
create:
    // If there is an existing device, mark it as no longer available.
    if (existingDev)
        existingDev->plugged = false;

    // Find parent
    DeviceImpl * parent = 0;
    if (device.address.size() != 0)
    {
        for (Device * i = enumDevices(0); i; i = enumDevices(i))
        {
            if ((i->getTopologyAddress() == device.address.parent()) &&
                (((DeviceImpl *)i)->plugged))
            {
                parent = (DeviceImpl*)i;
                break;
            }
        }
    }

    DP_ASSERT((parent || device.address.size() <= 1) && "Device was registered before parent");

    DeviceImpl * newDev;
    //
    // If it is a faked Mux device, we have already notified DD of few of its caps.
    // Reuse the same device to make sure that DD updates the same device's parameters
    // otherwise create a new device
    //
    if (existingDev && existingDev->isFakedMuxDevice())
    {
        newDev = existingDev;
        existingDev = NULL;
    }
    else
    {
        newDev = new DeviceImpl(hal, this, parent);
    }

    if (parent)
        parent->children[device.address.tail()] = newDev;

    if (!newDev)
    {
        DP_ASSERT(0 && "new failed");
        return;
    }

    // Fill out the new device
    newDev->address = device.address;
    newDev->multistream = isMultistream;
    newDev->videoSink = hasVideo;
    newDev->audioSink = hasAudio;
    newDev->plugged = true;
    newDev->rawEDID = edid;
    newDev->processedEdid = processedEdid;
    newDev->connectorType = connector;
    newDev->guid = device.peerGuid;
    newDev->peerDevice = device.peerDevice;
    newDev->portMap = device.portMap;
    newDev->dpcdRevisionMajor = device.dpcdRevisionMajor;
    newDev->dpcdRevisionMinor = device.dpcdRevisionMinor;
    newDev->complianceDeviceEdidReadTest = isCompliance;
    newDev->maxTmdsClkRate = maxTmdsClkRate;
    newDev->bApplyPclkWarBug4949066 = false;

    Address::NvU32Buffer addrBuffer;
    dpMemZero(addrBuffer, sizeof(addrBuffer));
    newDev->address.toNvU32Buffer(addrBuffer);
    NV_DPTRACE_INFO(NEW_SINK_DETECTED, newDev->address.size(), addrBuffer[0], addrBuffer[1], addrBuffer[2], addrBuffer[3],
                        newDev->multistream, newDev->rawEDID.getManufId(), newDev->rawEDID.getProductId());

    if(newDev->rawEDID.getManufId() ==  0x6D1E)
    {
        newDev->bApplyPclkWarBug4949066 = true;
    }

    // Apply any DPCD overrides if required
    newDev->dpcdOverrides();

    this->setDisableDownspread(processedEdid.WARFlags.bDisableDownspread);
    //
    // Some 4K eDP panel needs HBR2 to support higher modes, Highest assessed LC
    // remains in a stale state after applying DPCD overrides here. So we need to
    // assess the link again.
    //
    if (newDev->isOptimalLinkConfigOverridden())
    {
        this->assessLink();
    }

    // Postpone the remote HDCPCap read for Dongles
    DP_ASSERT(!isLinkInD3() && "Hdcp probe at D3");
    if (device.peerDevice != Dongle)
    {
        DP_ASSERT(newDev->isDeviceHDCPDetectionAlive == false);
        if ((newDev->deviceHDCPDetection = new DeviceHDCPDetection(newDev, messageManager, timer)))
        {
            //
            // We cannot move the hdcpDetection after stream added because DD
            // needs hdcp Cap before stream added.
            //
            newDev->isDeviceHDCPDetectionAlive = true;
            newDev->deviceHDCPDetection->start();
        }
        else
        {
            // For the risk control, make the device as not HDCPCap.
            DP_ASSERT(0 && "new failed");
            newDev->isDeviceHDCPDetectionAlive = false;
            newDev->isHDCPCap = False;

            if (!newDev->isMultistream())
                newDev->shadow.hdcpCapDone = true;
        }
    }

    newDev->vrrEnablement = new VrrEnablement(newDev);
    if (!newDev->vrrEnablement)
    {
        DP_ASSERT(0 && "new VrrEnablement failed");
    }

    BInfo bInfo;
    if ((!isHopLimitExceeded) && (hal->getBinfo(bInfo)))
    {
        if (bInfo.maxCascadeExceeded || bInfo.maxDevsExceeded)
        {
            if (isHDCPAuthOn)
            {
                isHDCPAuthOn = false;
            }
            isHopLimitExceeded = true;
        }
        else
            isHopLimitExceeded = false;
    }

    //
    // If the device is a faked Mux device, then we just initizlied it.
    // Reset its faked status and skip adding it to the deviceList
    //
    if (newDev->isFakedMuxDevice())
    {
        newDev->bIsFakedMuxDevice = false;
        newDev->bIsPreviouslyFakedMuxDevice = true;
    }
    else
    {
        deviceList.insertBack(newDev);
    }

    // if a new device has replaced a previous compliance device; let this event be exposed to DD now.
    // ie : the old device will be zombied/lost now ... lazily(instead of at an unplug which happened a while back.)
    if (existingDev && existingDev->complianceDeviceEdidReadTest)
        existingDev->lazyExitNow = true;

    if(newDev->isBranchDevice() && newDev->isAtLeastVersion(1,4))
    {
        //
        // GUID_2 will be non-zero for a virtual peer device and 0 for others.
        // This will help identify if a device is virtual peer device or not.
        //
        newDev->queryGUID2();
    }

    if (!linkAwaitingTransition)
    {
        //
        // When link is awaiting SST<->MST transition, DSC caps read from downstream
        // DSC branch device might be wrong. DSC Caps exposed by DSC MST branch depends
        // on the current link state. If it is in SST mode ie MST_EN (0x111[bit 0]) is 0 and
        // panel connected behind it supports DSC, then branch will expose the DSC caps
        // of the panel connected down stream rather than it's own. This is because source
        // will have no other way to read the caps of the downstream panel. In fact when
        // MST_EN = 0 and UP_REQ_EN (0x111 [bit 1]) = 1 source can read the caps of the
        // downstream panel using REMOTE_DPCD_READ but branch device's behavior depends
        // only on MST_EN bit. Similarly in SST, if the panel connected downstream to branch
        // does not support DSC, DSC MST branch will expose it's own DSC caps.
        // During boot since VBIOS drives display in SST mode and when driver takes over,
        // linkAwaitingTransition will be true. DpLib does link assessment and topology
        // discovery by setting UP_REQ_EN to true while still keeping MST_EN to false.
        // This is to ensure we detach the head and active modeset groups that are in SST mode
        // before switching the link to MST mode. When processNewDevice is called at this
        // point to create new devices we should not read DSC caps due to above mentioned reason.
        // As long as linkIsAwaitingTransition is true, Dplib will not report new Devices to
        // to client since isPendingNewDevice() will be false even though DPlib discovered
        // new devices. After Dplib completes topology discovery, DD initiates notifyDetachBegin/End
        // to remove active groups from the link and notifyDetachEnd calls assessLink
        // where we toggle the link state. Only after this we should read DSC caps in this case.
        // Following this assesslink calls fireEvents() which will report
        // the new devies to clients and client will have the correct DSC caps.
        //
        bool bGpuDscSupported;

        // Check GPU DSC Support
        main->getDscCaps(&bGpuDscSupported);
        if (bGpuDscSupported)
        {
            if (newDev->getDSCSupport())
            {
                // Read and parse DSC caps only if panel supports DSC
                newDev->readAndParseDSCCaps();

                // Read and Parse Branch Specific DSC Caps
                if (!newDev->isVideoSink() && !newDev->isAudioSink())
                {
                    newDev->readAndParseBranchSpecificDSCCaps();
                }
            }

            if (!processedEdid.WARFlags.bIgnoreDscCap)
            {
                // Check if DSC is possible for the device and if so, set DSC Decompression device.
                newDev->setDscDecompressionDevice(this->bDscCapBasedOnParent);
            }
        }
    }

    if (newDev->peerDevice == Dongle)
    {
        // For Dongle, we need to read detailed port caps if DPCD access is available on DP 1.4+.
        if (newDev->isAtLeastVersion(1,4))
        {
            newDev->getPCONCaps(&(newDev->pconCaps));
        }

        //
        // If dongle does not have DPCD access but it is native PCON with Virtual peer support,
        // we can get dongle port capabilities from parent VP DPCD detailed port descriptors.
        //
        else if (newDev->parent && (newDev->parent)->isVirtualPeerDevice())
        {
            if (!main->isMSTPCONCapsReadDisabled())
            {
                newDev->parent->getPCONCaps(&(newDev->pconCaps));
                newDev->connectorType = newDev->parent->getConnectorType();
            }
        }
    }

    // Read panel replay capabilities
    newDev->getPanelReplayCaps();
    // Read ALPM caps on panel
    newDev->getAlpmCaps();

    // Get Panel FEC support only if GPU supports FEC
    if (this->isFECSupported())
    {
        newDev->getFECSupport();
    }

    if (main->supportMSAOverMST())
    {
        newDev->bMSAOverMSTCapable = newDev->getSDPExtnForColorimetrySupported();
    }
    else
    {
        newDev->bMSAOverMSTCapable = false;
    }

    newDev->applyOUIOverrides();

    if (main->isEDP() && !bOuiCached)
    {
        //
        // if the regkey is set, and the system is in discrete mode, skip OUI caching.
        // i.e. cache OUI if regkey is not set OR system is in dynamic mode.
        //
        if ((!this->bSkipZeroOuiCache) || main->isInternalPanelDynamicMuxCapable())
        {
            // Save Source OUI information for eDP.
            hal->getOuiSource(cachedSourceOUI, &cachedSourceModelName[0],
                              sizeof(cachedSourceModelName), cachedSourceChipRevision);
            this->bOuiCached = true;
        }
    }

    fireEvents();
}

LinkRates* ConnectorImpl::importDpLinkRates()
{
    LinkRate   linkRate;
    LinkRates  *pConnectorLinkRates = linkPolicy.getLinkRates();

    // Attempt to configure link rate table mode if supported
    if (hal->isIndexedLinkrateCapable() &&
        main->configureLinkRateTable(hal->getLinkRateTable(), pConnectorLinkRates))
    {
        hal->setIndexedLinkrateEnabled(true);
    }
    else
    {
        // Reset configured link rate table if ever enabled to get RM act right
        if (hal->isIndexedLinkrateEnabled())
        {
            main->configureLinkRateTable(NULL, NULL);
            hal->setIndexedLinkrateEnabled(false);
        }
        pConnectorLinkRates->clear();
    }

    // Get maximal link rate supported by GPU
    linkRate = main->maxLinkRateSupported();

    // Insert in order any additional entries regardless of ILR Capability

    if (linkRate >= dp2LinkRate_1_62Gbps)
        pConnectorLinkRates->insert((NvU16)dp2LinkRate_1_62Gbps);

    if (linkRate >= dp2LinkRate_2_70Gbps)
        pConnectorLinkRates->insert((NvU16)dp2LinkRate_2_70Gbps);

    if (linkRate >= dp2LinkRate_5_40Gbps)
        pConnectorLinkRates->insert((NvU16)dp2LinkRate_5_40Gbps);

    if (linkRate >= dp2LinkRate_8_10Gbps)
        pConnectorLinkRates->insert((NvU16)dp2LinkRate_8_10Gbps);

    return pConnectorLinkRates;
}

void ConnectorImpl::populateAllDpConfigs()
{
    LinkRate   linkRate;
    LinkRates  *pConnectorLinkRates = linkPolicy.getLinkRates();

    unsigned   laneCounts[] = {laneCount_1, laneCount_2, laneCount_4};
    unsigned   laneSets = sizeof(laneCounts) / sizeof(laneCounts[0]);

    //
    //    Following sequence is to be followed for saving power by default;
    //    It may vary with sinks which support link rate table.
    //
    //    Link Config     MBPS
    //    1*RBR           162
    //    1*HBR           270
    //    2*RBR           324
    //    1*HBR2          540
    //    2*HBR           540
    //    4*RBR           648
    //    1*HBR3          810
    //    ...
    //
    if (numPossibleLnkCfg)
    {
        DP_PRINTF(DP_NOTICE, "DPCONN> Rebuild possible link rate configurations");
        delete[] allPossibleLinkCfgs;
        numPossibleLnkCfg = 0;
    }

    importDpLinkRates();

    numPossibleLnkCfg = laneSets * pConnectorLinkRates->getNumLinkRates();
    if (numPossibleLnkCfg == 0)
    {
        DP_PRINTF(DP_ERROR, "DPCONN> %s: lane count %d or link rates %d!",
                  __FUNCTION__, pConnectorLinkRates->getNumLinkRates(), laneSets);
        DP_ASSERT(0 && "Invalid lane count or link rates!");
        return;
    }

    allPossibleLinkCfgs = new LinkConfiguration[numPossibleLnkCfg]();

    if (allPossibleLinkCfgs == NULL)
    {
        DP_PRINTF(DP_ERROR, "DPCONN> %s: Failed to allocate allPossibleLinkCfgs array",
                  __FUNCTION__);
        numPossibleLnkCfg = 0;
        return;
    }

    // Populate all possible link configuration
    linkRate = pConnectorLinkRates->getMaxRate();
    for (unsigned i = 0; i < pConnectorLinkRates->getNumLinkRates(); i++)
    {
        for (unsigned j = 0; j < laneSets; j++)
        {
            allPossibleLinkCfgs[i * laneSets + j].setLaneRate(linkRate, laneCounts[j]);
            allPossibleLinkCfgs[i * laneSets + j].bDisableDownspread = this->getDownspreadDisabled();
        }
        linkRate = pConnectorLinkRates->getLowerRate(linkRate);
    }

    // Sort link configurations per bandwidth from low to high
    for (unsigned i = 0; i < numPossibleLnkCfg - 1; i++)
    {
        LinkConfiguration *pLowCfg = &allPossibleLinkCfgs[i];
        for (unsigned j = i + 1; j < numPossibleLnkCfg; j++)
        {
            if (allPossibleLinkCfgs[j] < *pLowCfg)
                pLowCfg = &allPossibleLinkCfgs[j];
        }
        // Swap
        if (pLowCfg != &allPossibleLinkCfgs[i])
        {
            LinkRate swapRate  = pLowCfg->peakRate;
            unsigned swapLanes = pLowCfg->lanes;
            pLowCfg->setLaneRate(allPossibleLinkCfgs[i].peakRate,
                                 allPossibleLinkCfgs[i].lanes);
            allPossibleLinkCfgs[i].setLaneRate(swapRate, swapLanes);
        }
    }
}

void ConnectorImpl::discoveryLostDevice(const Address & address)
{
    DeviceImpl * existingDev = findDeviceInList(address);

    if (!existingDev)
    {
        DP_ASSERT(0 && "Device lost on device not in database?!");
        return;
    }

    existingDev->plugged = false;
    existingDev->devDoingDscDecompression = NULL;
    fireEvents();
}

ConnectorImpl::~ConnectorImpl()
{
    if (numPossibleLnkCfg)
        delete[] allPossibleLinkCfgs;

    timer->cancelCallbacks(this);
    delete discoveryManager;
    pendingEdidReads.clear();
    delete messageManager;
    delete hal;
}

//
//   Clear all the state associated with the head attachment
//
void ConnectorImpl::hardwareWasReset()
{
    activeLinkConfig.lanes = 0;

    while (!activeGroups.isEmpty())
    {
        GroupImpl * g = (GroupImpl *)activeGroups.front();
        activeGroups.remove(g);
        inactiveGroups.insertBack(g);

        g->setHeadAttached(false);
    }

    while (!dscEnabledDevices.isEmpty())
        (void) dscEnabledDevices.pop();
}

Group * ConnectorImpl::resume(bool firmwareLinkHandsOff,
                              bool firmwareDPActive,
                              bool plugged,
                              bool isUefiSystem,
                              unsigned firmwareHead,
                              bool bFirmwareLinkUseMultistream,
                              bool bDisableVbiosScratchRegisterUpdate,
                              bool bAllowMST)
{
    Group  * result = 0;
    hardwareWasReset();
    previousPlugged = false;
    connectorActive = true;
    bIsUefiSystem = isUefiSystem;

    this->bDisableVbiosScratchRegisterUpdate = bDisableVbiosScratchRegisterUpdate;

    bFromResumeToNAB = true;

    if (firmwareLinkHandsOff)
    {
        isLinkQuiesced = true;
    }
    else if (firmwareDPActive)
    {
        DP_PRINTF(DP_NOTICE, "CONN> Detected firmware panel is active on head %d.", firmwareHead);
        ((GroupImpl *)firmwareGroup)->setHeadAttached(true);
        ((GroupImpl *)firmwareGroup)->headIndex = firmwareHead;
        ((GroupImpl *)firmwareGroup)->streamIndex = 1;
        ((GroupImpl *)firmwareGroup)->headInFirmware = true;

        this->linkState = bFirmwareLinkUseMultistream ? DP_TRANSPORT_MODE_MULTI_STREAM : DP_TRANSPORT_MODE_SINGLE_STREAM;

        inactiveGroups.remove((GroupImpl *)firmwareGroup);
        activeGroups.remove((GroupImpl *)firmwareGroup);
        activeGroups.insertBack((GroupImpl *)firmwareGroup);

        result = firmwareGroup;
    }

    hal->overrideMultiStreamCap(bAllowMST);

    //
    // In resume code path, all devices on this connector gets lost and deleted on first fireEvents()
    // and that could generate unnecessary new/lost device events. Therefore defer to lost devices
    // until discovery detect gets completed, this allows processNewDevice() function to look
    // at matching existing devices and optimize creation of new devices. We only have to set the flag
    // to true when plugged = true, since if disconnected, we are not going to defer anything.
    //
    bDeferNotifyLostDevice = plugged;
    bAttachOnResume = true;

    //
    // If we are resuming, record the allocatedDpTunnelBw before calling NLP
    // NLP will reset and try to allocate BW = LinkConfiguration
    // The previous CQA would have been performed with this allocated BW
    // If this is different from the new allocation, we will queue a DP allocation changed event
    //
    NvU64 previousAllocatedDpTunnelBw = allocatedDpTunnelBw;
    notifyLongPulse(plugged);

    // Reallocate DP BW for all connected and known clients
    updateDpTunnelBwAllocation();
    if (previousAllocatedDpTunnelBw != allocatedDpTunnelBw)
    {
        timer->queueCallback(this, &tagDpBwAllocationChanged, 0, false /* not allowed in sleep */);
    }

    bAttachOnResume = false;

    return result;
}

void ConnectorImpl::pause()
{
    connectorActive = false;
    if (messageManager)
    {
        messageManager->pause();
    }
}

// Query current Device topology
Device * ConnectorImpl::enumDevices(Device * previousDevice)
{
    if (previousDevice)
        previousDevice = (DeviceImpl *)((DeviceImpl*)previousDevice)->next;
    else
        previousDevice = (DeviceImpl *)deviceList.begin();

    if ((DeviceImpl*)previousDevice == deviceList.end())
        return 0;
    else
        return (DeviceImpl *)previousDevice;
}

LinkConfiguration ConnectorImpl::getMaxLinkConfig()
{
    NvU64 maxLinkRate;
    NvU64 gpuMaxLinkRate;

    DP_ASSERT(hal);

    if (main->isEDP())
    {
        // Regkey is supported on eDP panels only
        maxLinkRate = maxLinkRateFromRegkey;
        // Check if valid value is present in regkey
        if (!(maxLinkRate && (IS_VALID_LINKBW_10M(maxLinkRate))))
        {
            maxLinkRate = hal->getMaxLinkRate();
        }
    }
    else
    {
        maxLinkRate = hal->getMaxLinkRate();
    }

    gpuMaxLinkRate = main->maxLinkRateSupported();

    LinkRate linkRate = maxLinkRate ?
                        DP_MIN(maxLinkRate, gpuMaxLinkRate) :
                        gpuMaxLinkRate;

    unsigned laneCount = hal->getMaxLaneCount() ?
                         DP_MIN(hal->getMaxLaneCountSupportedAtLinkRate(linkRate), hal->getMaxLaneCount()) :
                         4U;

    return LinkConfiguration (&this->linkPolicy,
                              laneCount, linkRate,
                              this->hal->getEnhancedFraming(),
                              linkUseMultistream(),
                              false,  /* disablePostLTRequest */
                              this->bFECEnable,
                              false,  /* disableLTTPR */
                              this->getDownspreadDisabled());
}

LinkConfiguration ConnectorImpl::getActiveLinkConfig()
{
    DP_ASSERT(hal);

    return activeLinkConfig;
}

LinkConfiguration ConnectorImpl::initMaxLinkConfig()
{
    LinkRate linkRate = dp2LinkRate_8_10Gbps;
    unsigned laneCount = 4;

    return LinkConfiguration (&this->linkPolicy,
                              laneCount, linkRate,
                              this->hal->getEnhancedFraming(),
                              linkUseMultistream(),
                              false,  /* disablePostLTRequest */
                              this->bFECEnable);
}

void ConnectorImpl::beginCompoundQuery(const bool bForceEnableFEC)
{
    if (linkGuessed && (main->getSorIndex() != DP_INVALID_SOR_INDEX))
    {
        assessLink();
    }

    DP_ASSERT( !compoundQueryActive && "Previous compoundQuery was not ended.");
    compoundQueryActive = true;
    compoundQueryCount = 0;
    compoundQueryResult = true;
    compoundQueryLocalLinkPBN = 0;
    compoundQueryUsedTunnelingBw = 0;
    compoundQueryForceEnableFEC = bForceEnableFEC;

    for (Device * i = enumDevices(0); i; i=enumDevices(i))
    {
        DeviceImpl * dev = (DeviceImpl *)i;

        if (i->getTopologyAddress().size() <= 1)
        {
            dev->bandwidth.lastHopLinkConfig = highestAssessedLC;
            dev->bandwidth.compound_query_state.totalTimeSlots = 63;
            dev->bandwidth.compound_query_state.timeslots_used_by_query = 0;
            continue;
        }

        if (!this->linkUseMultistream())
            continue;

        // Initialize starting conditions
        //
        // Note: this compound query code assumes that the total bandwidth is
        // available for the configuration being queried.  This ignores the
        // concentrator case where some bandwidth may be in use by streams not
        // controlled by this driver instance.  Concentrators are currently not
        // supported.
        dev->bandwidth.compound_query_state.timeslots_used_by_query = 0;
        dev->inferLeafLink(&dev->bandwidth.compound_query_state.totalTimeSlots);

        //
        // Some VBIOS leave the branch in stale state and allocatePayload request queued
        // at branch end gets processed much later causing the FreePBN returned to be stale.
        // Clear the PBN in case EPR reports 0 free PBN when we have not explicitly requested
        // for it, to clear up any previous stale allocations
        //
        if (dev->bandwidth.compound_query_state.totalTimeSlots == 0 &&
           !dev->payloadAllocated && dev->plugged)
        {
            GroupImpl *group = dev->activeGroup;
            if (group != NULL)
            {
                NakData nakData;
                Address devAddress = dev->getTopologyAddress();

                AllocatePayloadMessage allocate;
                unsigned sink = 0;    // hardcode the audio sink to 0th in the device.
                allocate.set(devAddress.parent(), devAddress.tail(),
                    dev->isAudioSink() ? 1 : 0, group->streamIndex, 0, &sink, true);

                ((DeviceImpl *)dev)->bandwidth.enum_path.dataValid = false;

                if (group->parent->messageManager->send(&allocate, nakData))
                    dev->inferLeafLink(&dev->bandwidth.compound_query_state.totalTimeSlots);
            }
        }

        // Clear assement state
        dev->bandwidth.compound_query_state.bandwidthAllocatedForIndex = 0;
    }
}

static DP_IMP_ERROR translatePpsErrorToDpImpError(NVT_STATUS ppsErrorCode)
{
    switch (ppsErrorCode)
    {
        case NVT_STATUS_COLOR_FORMAT_NOT_SUPPORTED:
            return DP_IMP_ERROR_PPS_COLOR_FORMAT_NOT_SUPPORTED;
        case NVT_STATUS_INVALID_HBLANK:
            return DP_IMP_ERROR_PPS_INVALID_HBLANK;
        case NVT_STATUS_INVALID_BPC:
            return DP_IMP_ERROR_PPS_INVALID_BPC;
        case NVT_STATUS_MAX_LINE_BUFFER_ERROR:
            return DP_IMP_ERROR_PPS_MAX_LINE_BUFFER_ERROR;
        case NVT_STATUS_OVERALL_THROUGHPUT_ERROR:
            return DP_IMP_ERROR_PPS_OVERALL_THROUGHPUT_ERROR;
        case NVT_STATUS_DSC_SLICE_ERROR:
            return DP_IMP_ERROR_PPS_DSC_SLICE_ERROR;
        case NVT_STATUS_PPS_SLICE_COUNT_ERROR:
            return DP_IMP_ERROR_PPS_PPS_SLICE_COUNT_ERROR;
        case NVT_STATUS_PPS_SLICE_HEIGHT_ERROR:
            return DP_IMP_ERROR_PPS_PPS_SLICE_HEIGHT_ERROR;
        case NVT_STATUS_PPS_SLICE_WIDTH_ERROR:
            return DP_IMP_ERROR_PPS_PPS_SLICE_WIDTH_ERROR;
        case NVT_STATUS_INVALID_PEAK_THROUGHPUT:
            return DP_IMP_ERROR_PPS_INVALID_PEAK_THROUGHPUT;
        case NVT_STATUS_MIN_SLICE_COUNT_ERROR:
            return DP_IMP_ERROR_PPS_MIN_SLICE_COUNT_ERROR;
        default:
            return DP_IMP_ERROR_PPS_GENERIC_ERROR;
    }
}

bool ConnectorImpl::compoundQueryAttachTunneling(const DpModesetParams &modesetParams,
                                                 DscParams *pDscParams,
                                                 DP_IMP_ERROR *pErrorCode)
{
    if (!hal->isDpTunnelBwAllocationEnabled())
    {
        return true;
    }

    NvU64 bpp = modesetParams.modesetInfo.depth;
    if (pDscParams->bEnableDsc)
    {
        bpp = divide_ceil(pDscParams->bitsPerPixelX16, 16);
    }

    NvU64 modeBwRequired = modesetParams.modesetInfo.pixelClockHz * bpp;
    NvU64 freeTunnelingBw = allocatedDpTunnelBw - compoundQueryUsedTunnelingBw;

    if (modeBwRequired > freeTunnelingBw)
    {
        SET_DP_IMP_ERROR(pErrorCode, DP_IMP_ERROR_INSUFFICIENT_DP_TUNNELING_BANDWIDTH);
        return false;
    }

    compoundQueryUsedTunnelingBw += modeBwRequired;

    return true;
}

//
// This call will be deprecated as soon as all clients move to the new API
//
bool ConnectorImpl::compoundQueryAttach(Group * target,
                                        unsigned twoChannelAudioHz,         // if you need 192khz stereo specify 192000 here
                                        unsigned eightChannelAudioHz,       // Same setting for multi channel audio.
                                                                            //  DisplayPort encodes 3-8 channel streams as 8 channel
                                        NvU64 pixelClockHz,                 // Requested pixel clock for the mode
                                        unsigned rasterWidth,
                                        unsigned rasterHeight,
                                        unsigned rasterBlankStartX,
                                        unsigned rasterBlankEndX,
                                        unsigned depth,
                                        DP_IMP_ERROR *pErrorCode)
{
    ModesetInfo modesetInfo(twoChannelAudioHz, eightChannelAudioHz, pixelClockHz,
                            rasterWidth, rasterHeight, (rasterBlankStartX - rasterBlankEndX),
                            0/*surfaceHeight*/, depth, rasterBlankStartX, rasterBlankEndX);

    DpModesetParams modesetParams(0, modesetInfo);
    return compoundQueryAttach(target, modesetParams, NULL, pErrorCode);
}

bool ConnectorImpl::compoundQueryAttach(Group * target,
                                        const DpModesetParams &modesetParams,   // Modeset info
                                        DscParams *pDscParams,                  // DSC parameters
                                        DP_IMP_ERROR *pErrorCode)
{
    DP_ASSERT(compoundQueryActive);
    if (pErrorCode)
        *pErrorCode = DP_IMP_ERROR_NONE;

    compoundQueryCount++;
    DpModesetParams _dpModesetParams = modesetParams;
    _dpModesetParams.modesetInfo.colorFormat = modesetParams.colorFormat;

    if (!modesetParams.modesetInfo.depth || !modesetParams.modesetInfo.pixelClockHz)
    {
        DP_ASSERT(!"DP-CONN> Params with zero value passed to query!");
        compoundQueryResult = false;
        SET_DP_IMP_ERROR(pErrorCode, DP_IMP_ERROR_ZERO_VALUE_PARAMS)
        return false;
    }

    if (linkUseMultistream())
    {
        compoundQueryResult = compoundQueryAttachMST(target, _dpModesetParams,
                                                     pDscParams, pErrorCode);
    }
    else    // SingleStream case
    {
        compoundQueryResult = compoundQueryAttachSST(target, _dpModesetParams,
                                                     pDscParams, pErrorCode);
    }

    if (compoundQueryResult)
    {
        compoundQueryResult = compoundQueryAttachTunneling(_dpModesetParams, pDscParams, pErrorCode);
    }

    return compoundQueryResult;
}

bool ConnectorImpl::dpLinkIsModePossible(const DpLinkIsModePossibleParams &params)
{
    bool bResult;
    NvU32 numNonDscStreams;
    bool bEnableFEC = false;

reRunCompoundQuery:
    bResult = true;
    numNonDscStreams = 0;

    for (NvU32 i = 0; i < NV_MAX_HEADS; i++)
    {
        if (params.head[i].pDscParams != NULL)
            params.head[i].pDscParams->bEnableDsc = false;

        if (params.head[i].pErrorStatus != NULL)
            *params.head[i].pErrorStatus = DP_IMP_ERROR_NONE;
    }

    this->beginCompoundQuery(bEnableFEC /* bForceEnableFEC */);

    for (NvU32 i = 0; i < NV_MAX_HEADS; i++)
    {
        if (params.head[i].pTarget == NULL)
            continue;

        DP_ASSERT(params.head[i].pModesetParams->headIndex == i);

        bResult = this->compoundQueryAttach(params.head[i].pTarget,
                                            *params.head[i].pModesetParams,
                                            params.head[i].pDscParams,
                                            params.head[i].pErrorStatus);
        if (!bResult)
            break;

        if ((params.head[i].pDscParams == NULL) ||
            !params.head[i].pDscParams->bEnableDsc)
        {
            numNonDscStreams++;
            continue;
        }

        //
        // When DSC is enabled, FEC also need to be enabled. The previously
        // attached non-dsc streams needs to consider 3% FEC overhead,
        // therefore terminate existing compound query, force enable FEC and
        // re-run the compound query.
        //
        if ((numNonDscStreams > 0) && !bEnableFEC)
        {
            this->endCompoundQuery();
            bEnableFEC = true;
            goto reRunCompoundQuery;
        }

        bEnableFEC = true;
    }

    if (!this->endCompoundQuery())
        bResult = false;

    return bResult;
}

bool ConnectorImpl::compoundQueryAttachMST(Group * target,
                                           const DpModesetParams &modesetParams,         // Modeset info
                                           DscParams *pDscParams,                        // DSC parameters
                                           DP_IMP_ERROR *pErrorCode)
{
    CompoundQueryAttachMSTInfo localInfo;
    NvBool result = true;

    localInfo.localModesetInfo = modesetParams.modesetInfo;
    if (this->preferredLinkConfig.isValid())
        localInfo.lc = preferredLinkConfig;
    else
        localInfo.lc = highestAssessedLC;

    if (compoundQueryForceEnableFEC)
    {
        localInfo.lc.enableFEC(isFECCapable());
    }

    if (compoundQueryAttachMSTIsDscPossible(target, modesetParams, pDscParams))
    {
        unsigned int forceDscBitsPerPixelX16 = pDscParams->bitsPerPixelX16;
        result = compoundQueryAttachMSTDsc(target, modesetParams, &localInfo,
                                           pDscParams, pErrorCode);
        if (!result)
        {
            return false;
        }

        compoundQueryResult = compoundQueryAttachMSTGeneric(target, modesetParams, &localInfo,
                                                            pDscParams, pErrorCode);
        //
        // compoundQueryAttachMST Generic might fail due to the insufficient bandwidth ,
        // We only check whether bpp can be fit in the available bandwidth based on the tranied link config in compoundQueryAttachMSTDsc function.
        // There might be cases where the default 10 bpp might fit in the available bandwidth based on the trained link config,
        // however, the bandwidth might be insufficient at the actual bottleneck link between source and sink to drive the mode, causing CompoundQueryAttachMSTGeneric to fail.
        // Incase of CompoundQueryAttachMSTGeneric failure, instead of returning false, check whether the mode can be supported with the max dsc compression bpp
        // and return true if it can be supported.

        if (!compoundQueryResult && forceDscBitsPerPixelX16 == 0U && this->bEnableLowerBppCheckForDsc)
        {
            pDscParams->bitsPerPixelX16 = MAX_DSC_COMPRESSION_BPPX16;
            result = compoundQueryAttachMSTDsc(target, modesetParams, &localInfo,
                                               pDscParams, pErrorCode);
            if (!result)
            {
                return false;
            }

            return compoundQueryAttachMSTGeneric(target, modesetParams, &localInfo,
                                                 pDscParams, pErrorCode);
        }
        return compoundQueryResult;
    }

    return compoundQueryAttachMSTGeneric(target, modesetParams, &localInfo,
                                         pDscParams, pErrorCode);
}

bool ConnectorImpl::compoundQueryAttachMSTIsDscPossible
(
    Group * target,
    const DpModesetParams &modesetParams,       // Modeset info
    DscParams *pDscParams                       // DSC parameters
)
{
    Device     * newDev = target->enumDevices(0);
    DeviceImpl * dev    = (DeviceImpl *)newDev;
    bool bFecCapable = false;
    bool bGpuDscSupported;
    main->getDscCaps(&bGpuDscSupported);

    if (pDscParams && (pDscParams->forceDsc != DSC_FORCE_DISABLE))
    {
        if (dev && dev->isDSCPossible())
        {
            if ((dev->devDoingDscDecompression != dev) ||
                ((dev->devDoingDscDecompression == dev) &&
                (dev->isLogical() && dev->parent)))
            {
                //
                // If DSC decoding is going to happen at sink's parent or
                // decoding will be done by sink but sink is a logical port,
                // where intermediate link between Branch DFP and Rx Panel can be
                // anything other than DP (i.e. DSI, LVDS or something else),
                // then we have to only make sure the path from source to sink's
                // parent is fec is capable.
                // Refer DP 1.4 Spec 5.4.5
                //
                bFecCapable = dev->parent->isFECSupported();
            }
            else
            {
                bFecCapable = dev->isFECSupported();
            }
        }
    }
    else
    {
        return false;
    }
    // Make sure panel/it's parent & GPU supports DSC and the whole path supports FEC
    if (bGpuDscSupported &&                                 // If GPU supports DSC
        this->isFECSupported() &&                           // If GPU supports FEC
        pDscParams &&                                       // If client sent DSC info
        pDscParams->bCheckWithDsc &&                        // If client wants to check with DSC
        (dev && dev->devDoingDscDecompression) &&           // Either device or it's parent supports DSC
        bFecCapable &&                                      // If path up to dsc decoding device supports FEC
        (modesetParams.modesetInfo.bitsPerComponent != 6))  // DSC doesn't support bpc = 6
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool ConnectorImpl::compoundQueryAttachMSTDsc(Group * target,
                                              const DpModesetParams &modesetParams,         // Modeset info
                                              CompoundQueryAttachMSTInfo * localInfo,
                                              DscParams *pDscParams,                        // DSC parameters
                                              DP_IMP_ERROR *pErrorCode)
{
    NVT_STATUS result;

    Device     * newDev = target->enumDevices(0);
    DeviceImpl * dev    = (DeviceImpl *)newDev;

    bool bGpuDscSupported;
    main->getDscCaps(&bGpuDscSupported);

    DSC_INFO dscInfo;
    MODESET_INFO modesetInfoDSC;
    WAR_DATA warData;
    NvU64 availableBandwidthBitsPerSecond = 0;
    unsigned PPS[DSC_MAX_PPS_SIZE_DWORD];
    unsigned bitsPerPixelX16 = 0;
    bool bDscBppForced = false;

    if (!pDscParams->bitsPerPixelX16)
    {
        //
        // For now, we will keep a pre defined value for bitsPerPixel for MST = 10
        // bitsPerPixelX16 = 160
        //
        pDscParams->bitsPerPixelX16 = PREDEFINED_DSC_MST_BPPX16;
    }
    else
    {
        bDscBppForced = true;
    }

    bitsPerPixelX16 = pDscParams->bitsPerPixelX16;

    if (!this->preferredLinkConfig.isValid())
    {
        localInfo->lc.enableFEC(true);
    }

    dpMemZero(PPS, sizeof(unsigned) * DSC_MAX_PPS_SIZE_DWORD);
    dpMemZero(&dscInfo, sizeof(DSC_INFO));
    dpMemZero(&warData, sizeof(WAR_DATA));

    // Populate DSC related info for PPS calculations
    populateDscCaps(&dscInfo, dev->devDoingDscDecompression, pDscParams->forcedParams);

    // populate modeset related info for PPS calculations
    populateDscModesetInfo(&modesetInfoDSC, &modesetParams);

    // checking for DSC v1.1 and YUV combination
    if ((dscInfo.sinkCaps.algorithmRevision.versionMajor == 1) &&
        (dscInfo.sinkCaps.algorithmRevision.versionMinor == 1) &&
        (modesetParams.colorFormat == dpColorFormat_YCbCr444 ))
    {
        DP_PRINTF(DP_WARNING, "WARNING: DSC v1.2 or higher is recommended for using YUV444");
        DP_PRINTF(DP_WARNING, "Current version is 1.1");
    }

    if ((dev->devDoingDscDecompression == dev) && dev->parent)
    {
        if (dev->parent->bDscPassThroughColorFormatWar)
        {
            //
            // Bug 3692417
            // Color format should only depend on device doing DSC decompression when DSC is enabled according to DP Spec.
            // But when Synaptics VMM5320 is the parent of the device doing DSC decompression, if a certain color
            // format is not supported by Synaptics Virtual Peer Device decoder(parent), even though it is pass through mode
            // and panel supports the color format, panel cannot light up. Once Synaptics fixes this issue, we will modify
            // the WAR to be applied only before the firmware version that fixes it.
            //
            if ((modesetParams.colorFormat == dpColorFormat_RGB      && !dev->parent->dscCaps.dscDecoderColorFormatCaps.bRgb)      ||
                (modesetParams.colorFormat == dpColorFormat_YCbCr444 && !dev->parent->dscCaps.dscDecoderColorFormatCaps.bYCbCr444) ||
                (modesetParams.colorFormat == dpColorFormat_YCbCr422 && !dev->parent->dscCaps.dscDecoderColorFormatCaps.bYCbCrSimple422))
            {
                if ((pDscParams->forceDsc == DSC_FORCE_ENABLE) ||
                    (modesetParams.modesetInfo.mode == DSC_DUAL))
                {
                    //
                    // If DSC is force enabled or DSC_DUAL mode is requested,
                    // then return failure here
                    //
                    compoundQueryResult = false;
                    SET_DP_IMP_ERROR(pErrorCode, DP_IMP_ERROR_DSC_SYNAPTICS_COLOR_FORMAT)
                    pDscParams->bEnableDsc = false;
                    return false;
                }
                else
                {
                    // We should check if mode is possible without DSC.
                    pDscParams->bEnableDsc = false;
                    if (!compoundQueryForceEnableFEC)
                    {
                        localInfo->lc.enableFEC(false);
                    }
                    return true;
                }
            }
        }
    }

    availableBandwidthBitsPerSecond = localInfo->lc.convertMinRateToDataRate() * 8 * localInfo->lc.lanes;

    warData.dpData.linkRateHz = localInfo->lc.peakRate;
    warData.dpData.bIs128b132bChannelCoding = localInfo->lc.bIs128b132bChannelCoding;
    warData.dpData.bDisableEffBppSST8b10b = this->bDisableEffBppSST8b10b;

    warData.dpData.laneCount = localInfo->lc.lanes;
    warData.dpData.dpMode = DSC_DP_MST;
    warData.dpData.hBlank = modesetParams.modesetInfo.rasterWidth - modesetParams.modesetInfo.surfaceWidth;
    warData.connectorType = DSC_DP;
    warData.dpData.bDisableDscMaxBppLimit = bDisableDscMaxBppLimit;

    //
    // Dplib needs to pass sliceCountMask to clients
    // with all slice counts that can support the mode since clients
    // might need to use a slice count other than the minimum slice count
    // that supports the mode. Currently we keep the same policy of
    // trying 10 bpp first and if that does not pass, try 8pp. But later
    // with dynamic PPS update, this will be moved a better algorithm,
    // that optimizes bpp for requested mode on each display.
    //

    result = DSC_GeneratePPSWithSliceCountMask(&dscInfo, &modesetInfoDSC,
                                               &warData, availableBandwidthBitsPerSecond,
                                               (NvU32*)(PPS),
                                               (NvU32*)(&bitsPerPixelX16),
                                               &(pDscParams->sliceCountMask));

    // Try max dsc compression bpp = 8 once to check if that can support that mode.
    if (result != NVT_STATUS_SUCCESS && !bDscBppForced)
    {
        pDscParams->bitsPerPixelX16 = MAX_DSC_COMPRESSION_BPPX16;
        bitsPerPixelX16 = pDscParams->bitsPerPixelX16;
        result = DSC_GeneratePPSWithSliceCountMask(&dscInfo, &modesetInfoDSC,
                                                   &warData, availableBandwidthBitsPerSecond,
                                                   (NvU32*)(PPS),
                                                   (NvU32*)(&bitsPerPixelX16),
                                                   &(pDscParams->sliceCountMask));
    }

    if (result != NVT_STATUS_SUCCESS)
    {
        //
        // If generating PPS failed
        //          AND
        //    (DSC is force enabled
        //          OR
        //    the requested DSC mode = DUAL)
        //then
        //    return failure here
        // Else
        //    we will check if non DSC path is possible.
        //
        // If dsc mode = DUAL failed to generate PPS and if we pursue
        // non DSC path, DD will still follow 2Head1OR modeset path with
        // DSC disabled, eventually leading to HW hang. Bug 3632901
        //
        if ((pDscParams->forceDsc == DSC_FORCE_ENABLE) ||
            (modesetParams.modesetInfo.mode == DSC_DUAL))
        {
            compoundQueryResult = false;
            SET_DP_IMP_ERROR(pErrorCode, translatePpsErrorToDpImpError(result))
            pDscParams->bEnableDsc = false;
            return false;
        }
        else
        {
            // If PPS calculation failed then try without DSC
            pDscParams->bEnableDsc = false;
            if (!compoundQueryForceEnableFEC)
            {
                localInfo->lc.enableFEC(false);
            }
            return true;
        }
    }
    else
    {
        pDscParams->bEnableDsc = true;
        compoundQueryResult = true;
        localInfo->localModesetInfo.bEnableDsc = true;
        localInfo->localModesetInfo.depth = bitsPerPixelX16;
        if (modesetParams.colorFormat == dpColorFormat_YCbCr422 &&
            dev->dscCaps.dscDecoderColorFormatCaps.bYCbCrNative422 &&
            (dscInfo.gpuCaps.encoderColorFormatMask & DSC_ENCODER_COLOR_FORMAT_Y_CB_CR_NATIVE_422) &&
            (dscInfo.sinkCaps.decoderColorFormatMask & DSC_DECODER_COLOR_FORMAT_Y_CB_CR_NATIVE_422))
        {
            localInfo->localModesetInfo.colorFormat = dpColorFormat_YCbCr422_Native;
        }

        if (dev->peerDevice == Dongle && dev->connectorType == connectorHDMI)
        {
            //
            // For DP2HDMI PCON, if FRL BW is available in detailed caps,
            // we need to check if we have enough BW for the stream on FRL link.
            //
            if (dev->pconCaps.maxHdmiLinkBandwidthGbps != 0)
            {
                NvU64 requiredBw = (NvU64)(modesetParams.modesetInfo.pixelClockHz * modesetParams.modesetInfo.depth);
                NvU64 availableBw = (NvU64)(dev->pconCaps.maxHdmiLinkBandwidthGbps * (NvU64)1000000000U);
                if (requiredBw > availableBw)
                {
                    compoundQueryResult = false;
                    SET_DP_IMP_ERROR(pErrorCode, DP_IMP_ERROR_DSC_PCON_FRL_BANDWIDTH)
                    pDscParams->bEnableDsc = false;
                    return false;
                }
            }
            //
            // If DP2HDMI PCON does not support FRL, but advertises TMDS
            // Character clock rate on detailed caps, we need to honor that.
            //
            else if (dev->pconCaps.maxTmdsClkRate != 0)
            {
                NvU64 maxTmdsClkRateU64 = (NvU64)(dev->pconCaps.maxTmdsClkRate);
                NvU64 requiredBw        = (NvU64)(modesetParams.modesetInfo.pixelClockHz * modesetParams.modesetInfo.depth);
                if (modesetParams.colorFormat == dpColorFormat_YCbCr420)
                {
                    if (maxTmdsClkRateU64 < ((requiredBw/24)/2))
                    {
                        compoundQueryResult = false;
                        SET_DP_IMP_ERROR(pErrorCode, DP_IMP_ERROR_DSC_PCON_HDMI2_BANDWIDTH)
                        return false;
                    }
                }
                else
                {
                    if (maxTmdsClkRateU64 < (requiredBw/24))
                    {
                        compoundQueryResult = false;
                        SET_DP_IMP_ERROR(pErrorCode, DP_IMP_ERROR_DSC_PCON_HDMI2_BANDWIDTH)
                        return false;
                    }
                }
            }
        }
        else if (dev->devDoingDscDecompression != dev)
        {
            //
            // Device's parent is doing DSC decompression so we need to check
            // if device's parent can send uncompressed stream to Sink.
            //
            unsigned mode_pbn;

            mode_pbn = pbnForMode(modesetParams.modesetInfo);

            //
            // As Device's Parent is doing DSC decompression, this is leaf device and
            // complete available bandwidth at this node is available for requested mode.
            //
            if (mode_pbn > dev->bandwidth.enum_path.total)
            {
                compoundQueryResult = false;
                SET_DP_IMP_ERROR(pErrorCode, DP_IMP_ERROR_DSC_LAST_HOP_BANDWIDTH)
                pDscParams->bEnableDsc = false;
                return false;
            }
        }

        if (pDscParams->pDscOutParams != NULL)
        {
            //
            // If requested then DP Library is supposed to return if mode is
            // possible with DSC and calculated PPS and bits per pixel.
            //
            dpMemCopy(pDscParams->pDscOutParams->PPS, PPS, sizeof(unsigned) * DSC_MAX_PPS_SIZE_DWORD);
            pDscParams->bitsPerPixelX16 = bitsPerPixelX16;
        }
        else
        {
            //
            // Client only wants to know if mode is possible or not but doesn't
            // need all calculated PPS parameters in case DSC is required. Do nothing.
            //
        }
    }
    return true;
}

bool ConnectorImpl::compoundQueryAttachMSTGeneric(Group * target,
                                                  const DpModesetParams &modesetParams,         // Modeset info
                                                  CompoundQueryAttachMSTInfo * localInfo,
                                                  DscParams *pDscParams,                        // DSC parameters
                                                  DP_IMP_ERROR *pErrorCode)
{
    // I. Evaluate use of local link bandwidth

    //      Calculate the PBN required
    unsigned base_pbn, slots, slots_pbn;
    localInfo->lc.pbnRequired(localInfo->localModesetInfo, base_pbn, slots, slots_pbn);

    //      Accumulate the amount of PBN rounded up to nearest timeslot
    compoundQueryLocalLinkPBN += slots_pbn;
    if (compoundQueryLocalLinkPBN > localInfo->lc.pbnTotal())
    {
        compoundQueryResult = false;
        SET_DP_IMP_ERROR(pErrorCode, DP_IMP_ERROR_INSUFFICIENT_BANDWIDTH)
    }

    //      Verify the min blanking, etc
    Watermark dpinfo;

    if (this->isFECSupported())
    {
        if (!isModePossibleMSTWithFEC(localInfo->lc, localInfo->localModesetInfo, &dpinfo))
        {
            compoundQueryResult = false;
            SET_DP_IMP_ERROR(pErrorCode, DP_IMP_ERROR_WATERMARK_BLANKING)
        }
    }
    else
    {
        if (!isModePossibleMST(localInfo->lc, localInfo->localModesetInfo, &dpinfo))
        {
            compoundQueryResult = false;
            SET_DP_IMP_ERROR(pErrorCode, DP_IMP_ERROR_WATERMARK_BLANKING)
        }
    }

    for(Device * d = target->enumDevices(0); d; d = target->enumDevices(d))
    {
        DeviceImpl * i = (DeviceImpl *)d;

        // Allocate bandwidth for the entire path to the root
        //   NOTE: Above we're already handle the local link
        DeviceImpl * tail = i;
        while (tail && tail->getParent())
        {
            // Have we already accounted for this stream?
            if (!(tail->bandwidth.compound_query_state.bandwidthAllocatedForIndex & (1 << compoundQueryCount)))
            {
                tail->bandwidth.compound_query_state.bandwidthAllocatedForIndex |= (1 << compoundQueryCount);

                LinkConfiguration * linkConfig = tail->inferLeafLink(NULL);
                tail->bandwidth.compound_query_state.timeslots_used_by_query += linkConfig->slotsForPBN(base_pbn);

                if ( tail->bandwidth.compound_query_state.timeslots_used_by_query > tail->bandwidth.compound_query_state.totalTimeSlots)
                {
                    compoundQueryResult = false;
                    if(this->bEnableLowerBppCheckForDsc)
                    {
                        tail->bandwidth.compound_query_state.timeslots_used_by_query -= linkConfig->slotsForPBN(base_pbn);
                        tail->bandwidth.compound_query_state.bandwidthAllocatedForIndex &= ~(1 << compoundQueryCount);
                    }
                    SET_DP_IMP_ERROR(pErrorCode, DP_IMP_ERROR_INSUFFICIENT_BANDWIDTH)
                }
            }
            tail = (DeviceImpl*)tail->getParent();
        }
    }

    // If the compoundQueryResult is false, we need to reset the compoundQueryLocalLinkPBN
    if (!compoundQueryResult && this->bEnableLowerBppCheckForDsc)
    {
        compoundQueryLocalLinkPBN -= slots_pbn;
    }

    return compoundQueryResult;
}

bool ConnectorImpl::compoundQueryAttachSSTIsDscPossible
(
    const DpModesetParams &modesetParams,
    DscParams *pDscParams
)
{
    bool bGpuDscSupported = false;
    main->getDscCaps(&bGpuDscSupported);
    DeviceImpl * nativeDev = this->findDeviceInList(Address());

    if (bGpuDscSupported &&                                 // if GPU supports DSC
        this->isFECSupported() &&                           // If GPU supports FEC
        pDscParams &&                                       // if client sent DSC info
        pDscParams->bCheckWithDsc &&                        // if client wants to check with DSC
        nativeDev->isDSCPossible() &&                       // if device supports DSC decompression
        (nativeDev->isFECSupported() || main->isEDP()) &&   // if device supports FEC decoding or is an DSC capable eDP panel which doesn't support FEC
        (modesetParams.modesetInfo.bitsPerComponent != 6))  // DSC doesn't support bpc = 6
    {
        return true;
    }

    return  false;
}

bool ConnectorImpl::compoundQueryAttachSSTDsc
(
    const DpModesetParams &modesetParams,
    LinkConfiguration lc,
    DscParams *pDscParams,
    DP_IMP_ERROR *pErrorCode
)
{
    DSC_INFO dscInfo;
    MODESET_INFO modesetInfoDSC;
    WAR_DATA warData;
    NvU64 availableBandwidthBitsPerSecond = 0;
    unsigned PPS[DSC_MAX_PPS_SIZE_DWORD];
    unsigned bitsPerPixelX16 = pDscParams->bitsPerPixelX16;
    bool result;
    NVT_STATUS ppsStatus;
    ModesetInfo localModesetInfo = modesetParams.modesetInfo;

    DeviceImpl * nativeDev = this->findDeviceInList(Address());

    if (!this->preferredLinkConfig.isValid() && nativeDev->isFECSupported())
    {
        lc.enableFEC(true);
    }

    dpMemZero(PPS, sizeof(unsigned) * DSC_MAX_PPS_SIZE_DWORD);
    dpMemZero(&dscInfo, sizeof(DSC_INFO));
    dpMemZero(&warData, sizeof(WAR_DATA));

    // Populate DSC related info for PPS calculations
    this->populateDscCaps(&dscInfo, nativeDev->devDoingDscDecompression, pDscParams->forcedParams);

    // Populate modeset related info for PPS calculations
    this->populateDscModesetInfo(&modesetInfoDSC, &modesetParams);

    // checking for DSC v1.1 and YUV combination
    if ( (dscInfo.sinkCaps.algorithmRevision.versionMajor == 1) &&
         (dscInfo.sinkCaps.algorithmRevision.versionMinor == 1) &&
         (modesetParams.colorFormat == dpColorFormat_YCbCr444 ))
    {
        DP_PRINTF(DP_WARNING, "WARNING: DSC v1.2 or higher is recommended for using YUV444");
        DP_PRINTF(DP_WARNING, "Current version is 1.1");
    }

    availableBandwidthBitsPerSecond = lc.convertMinRateToDataRate() * 8 * lc.lanes;

    warData.dpData.linkRateHz = lc.peakRate;
    warData.dpData.bIs128b132bChannelCoding = lc.bIs128b132bChannelCoding;
    warData.dpData.bDisableEffBppSST8b10b = this->bDisableEffBppSST8b10b;
    warData.dpData.laneCount = lc.lanes;
    warData.dpData.hBlank = modesetParams.modesetInfo.rasterWidth - modesetParams.modesetInfo.surfaceWidth;
    warData.dpData.dpMode = DSC_DP_SST;
    warData.connectorType = DSC_DP;
    warData.dpData.bDisableDscMaxBppLimit = bDisableDscMaxBppLimit;

    if (main->isEDP())
    {
        warData.dpData.bIsEdp = true;
    }

    ppsStatus = DSC_GeneratePPSWithSliceCountMask(&dscInfo,
                                                  &modesetInfoDSC,
                                                  &warData,
                                                  availableBandwidthBitsPerSecond,
                                                  (NvU32*)(PPS),
                                                  (NvU32*)(&bitsPerPixelX16),
                                                  &(pDscParams->sliceCountMask));

    if (ppsStatus != NVT_STATUS_SUCCESS)
    {
        result = false;
        SET_DP_IMP_ERROR(pErrorCode, translatePpsErrorToDpImpError(ppsStatus))
        pDscParams->bEnableDsc = false;
    }
    else
    {
        localModesetInfo.bEnableDsc = true;
        localModesetInfo.depth = bitsPerPixelX16;
        LinkConfiguration lowestSelected;
        bool bIsModeSupported = false;

        if (this->preferredLinkConfig.isValid())
        {
            // Check if mode is possible with preferred link config
            bIsModeSupported = willLinkSupportModeSST(lc, localModesetInfo, pDscParams);
        }
        else
        {
            //
            // Check if mode is possible with calculated bits_per_pixel.
            // Check with all possible link configs and not just highest
            // assessed because with DSC, mode can fail with higher
            // link config and pass for lower one. This is because
            // if raster parameters are really small and DP bandwidth is
            // very high then we may end up with some TU with 0 active
            // symbols in SST. This may cause HW hang and so DP IMP rejects
            // this mode. Refer Bug 200379426.
            //
            bIsModeSupported = getValidLowestLinkConfig(lc, lowestSelected, localModesetInfo, pDscParams);
        }

        if (!bIsModeSupported)
        {
            pDscParams->bEnableDsc = false;
            SET_DP_IMP_ERROR(pErrorCode, DP_IMP_ERROR_INSUFFICIENT_BANDWIDTH_DSC)
            result = false;
        }
        else
        {
            pDscParams->bEnableDsc = true;
            result = true;

            if (pDscParams->pDscOutParams != NULL)
            {
                //
                // If requested then DP Library is supposed to return if mode is
                // possible with DSC and calculated PPS and bits per pixel.
                //
                dpMemCopy(pDscParams->pDscOutParams->PPS, PPS, sizeof(unsigned) * DSC_MAX_PPS_SIZE_DWORD);
                pDscParams->bitsPerPixelX16 = bitsPerPixelX16;
            }
            else
            {
                //
                // Client only wants to know if mode is possible or not but doesn't
                // need all calculated PPS parameters in case DSC is required. Do nothing.
                //
            }
        }
    }

    return result;
}

bool ConnectorImpl::compoundQueryAttachSST(Group * target,
                                           const DpModesetParams &modesetParams,         // Modeset info
                                           DscParams *pDscParams,                        // DSC parameters
                                           DP_IMP_ERROR *pErrorCode)
{
    DeviceImpl * nativeDev = findDeviceInList(Address());

    if (compoundQueryCount != 1)
    {
        compoundQueryResult = false;
        return false;
    }

    if (nativeDev && (nativeDev->connectorType == connectorHDMI))
    {
        if (modesetParams.colorFormat == dpColorFormat_YCbCr420)
        {
            if ((nativeDev->maxTmdsClkRate) &&
                (nativeDev->maxTmdsClkRate <
                ((modesetParams.modesetInfo.pixelClockHz * modesetParams.modesetInfo.depth /24)/2)))
            {
                compoundQueryResult = false;
                SET_DP_IMP_ERROR(pErrorCode, DP_IMP_ERROR_DSC_PCON_HDMI2_BANDWIDTH)
                return false;
            }
        }
        else
        {
            if ((nativeDev->maxTmdsClkRate) &&
                (nativeDev->maxTmdsClkRate <
                (modesetParams.modesetInfo.pixelClockHz * modesetParams.modesetInfo.depth /24)))
            {
                compoundQueryResult = false;
                SET_DP_IMP_ERROR(pErrorCode, DP_IMP_ERROR_DSC_PCON_HDMI2_BANDWIDTH)
                return false;
            }
        }
    }

    LinkConfiguration lc = highestAssessedLC;

    // check if there is a special request from the client
    if (this->preferredLinkConfig.isValid())
    {
        lc = preferredLinkConfig;
    }
    else
    {
        //
        // Always check for DP IMP without FEC overhead first before
        // trying with DSC/FEC
        //
        lc.enableFEC(false);
    }

    // If a valid native DP device was not found, force legacy DP IMP
    if (!nativeDev)
    {
        compoundQueryResult = this->willLinkSupportModeSST(lc, modesetParams.modesetInfo, pDscParams);
        if (!compoundQueryResult)
        {
            SET_DP_IMP_ERROR(pErrorCode, DP_IMP_ERROR_WATERMARK_BLANKING)
        }
    }
    else
    {
        if ((lc.peakRate == dp2LinkRate_8_10Gbps) &&
            (main->isAvoidHBR3WAREnabled()) &&
            (compoundQueryAttachSSTIsDscPossible(modesetParams, pDscParams)))
        {
            LinkConfiguration lowerLc = lc;
            lowerLc.lowerConfig(false);

            if ((pDscParams && (pDscParams->forceDsc == DSC_FORCE_ENABLE)) ||
                (modesetParams.modesetInfo.mode == DSC_DUAL) ||
                (!this->willLinkSupportModeSST(lowerLc, modesetParams.modesetInfo, pDscParams)))
            {
                if (pDscParams && pDscParams->forceDsc != DSC_FORCE_DISABLE)
                {
                    bool result = compoundQueryAttachSSTDsc(modesetParams, lowerLc, pDscParams, pErrorCode);
                    if (result == true)
                        return result;
                }
            }
            else
            {
                // Mode was successful
                return true;
            }
        }

        if ((pDscParams && (pDscParams->forceDsc == DSC_FORCE_ENABLE)) ||                   // DD has forced DSC Enable
            (modesetParams.modesetInfo.mode == DSC_DUAL) ||                                 // DD decided to use 2 Head 1 OR mode
            (!this->willLinkSupportModeSST(lc, modesetParams.modesetInfo, pDscParams)))     // Mode is not possible without DSC
        {
            // If DP IMP fails without DSC or client requested to force DSC
            if (pDscParams && pDscParams->forceDsc != DSC_FORCE_DISABLE)
            {
                // Check if panel and GPU both supports DSC or not. Also check if panel supports FEC
                if (compoundQueryAttachSSTIsDscPossible(modesetParams, pDscParams))
                {
                    compoundQueryResult = compoundQueryAttachSSTDsc(modesetParams,
                                                                    lc,
                                                                    pDscParams,
                                                                    pErrorCode);
                }
                else
                {
                    // Either GPU or Sink doesn't support DSC
                    compoundQueryResult = false;
                }
            }
            else
            {
                // Client hasn't sent DSC params info or has asked to force disable DSC.
                compoundQueryResult = false;
                SET_DP_IMP_ERROR(pErrorCode, DP_IMP_ERROR_INSUFFICIENT_BANDWIDTH_NO_DSC)
            }
        }
        else
        {
            // Mode was successful
            compoundQueryResult = true;
        }
    }

    return compoundQueryResult;
}

void ConnectorImpl::populateDscModesetInfo(MODESET_INFO* pModesetInfo, const DpModesetParams* pModesetParams)
{
    pModesetInfo->pixelClockHz = pModesetParams->modesetInfo.pixelClockHz;
    pModesetInfo->activeWidth = pModesetParams->modesetInfo.surfaceWidth;
    pModesetInfo->activeHeight = pModesetParams->modesetInfo.surfaceHeight;
    pModesetInfo->bitsPerComponent = pModesetParams->modesetInfo.bitsPerComponent;

    if (pModesetParams->colorFormat == dpColorFormat_RGB)
    {
        pModesetInfo->colorFormat = NVT_COLOR_FORMAT_RGB;
    }
    else if (pModesetParams->colorFormat == dpColorFormat_YCbCr444)
    {
        pModesetInfo->colorFormat = NVT_COLOR_FORMAT_YCbCr444;
    }
    else if (pModesetParams->colorFormat == dpColorFormat_YCbCr422)
    {
        pModesetInfo->colorFormat = NVT_COLOR_FORMAT_YCbCr422;
    }
    else if (pModesetParams->colorFormat == dpColorFormat_YCbCr420)
    {
        pModesetInfo->colorFormat = NVT_COLOR_FORMAT_YCbCr420;
    }
    else
    {
        pModesetInfo->colorFormat = NVT_COLOR_FORMAT_RGB;
    }

    if (pModesetParams->modesetInfo.mode == DSC_DUAL)
    {
        pModesetInfo->bDualMode = true;
    }
    else
    {
        pModesetInfo->bDualMode = false;
    }

    if (pModesetParams->modesetInfo.mode == DSC_DROP)
    {
        pModesetInfo->bDropMode = true;
    }
    else
    {
        pModesetInfo->bDropMode = false;
    }
}

void ConnectorImpl::populateDscGpuCaps(DSC_INFO* dscInfo)
{
    unsigned encoderColorFormatMask;
    unsigned lineBufferSizeKB;
    unsigned rateBufferSizeKB;
    unsigned bitsPerPixelPrecision;
    unsigned maxNumHztSlices;
    unsigned lineBufferBitDepth;

    // Get GPU DSC capabilities
    main->getDscCaps(NULL,
        &encoderColorFormatMask,
        &lineBufferSizeKB,
        &rateBufferSizeKB,
        &bitsPerPixelPrecision,
        &maxNumHztSlices,
        &lineBufferBitDepth);

    if (encoderColorFormatMask & NV0073_CTRL_CMD_DP_GET_CAPS_DSC_ENCODER_COLOR_FORMAT_RGB)
    {
        dscInfo->gpuCaps.encoderColorFormatMask |= DSC_ENCODER_COLOR_FORMAT_RGB;
    }

    if (encoderColorFormatMask & NV0073_CTRL_CMD_DP_GET_CAPS_DSC_ENCODER_COLOR_FORMAT_Y_CB_CR_444)
    {
        dscInfo->gpuCaps.encoderColorFormatMask |= DSC_ENCODER_COLOR_FORMAT_Y_CB_CR_444;
    }

    if (encoderColorFormatMask & NV0073_CTRL_CMD_DP_GET_CAPS_DSC_ENCODER_COLOR_FORMAT_Y_CB_CR_NATIVE_422)
    {
        dscInfo->gpuCaps.encoderColorFormatMask |= DSC_ENCODER_COLOR_FORMAT_Y_CB_CR_NATIVE_422;
    }

    if (encoderColorFormatMask & NV0073_CTRL_CMD_DP_GET_CAPS_DSC_ENCODER_COLOR_FORMAT_Y_CB_CR_NATIVE_420)
    {
        dscInfo->gpuCaps.encoderColorFormatMask |= DSC_ENCODER_COLOR_FORMAT_Y_CB_CR_NATIVE_420;
    }

    dscInfo->gpuCaps.lineBufferSize = lineBufferSizeKB;

    if (bitsPerPixelPrecision == NV0073_CTRL_CMD_DP_GET_CAPS_DSC_BITS_PER_PIXEL_PRECISION_1_16)
    {
        dscInfo->gpuCaps.bitsPerPixelPrecision = DSC_BITS_PER_PIXEL_PRECISION_1_16;
    }

    if (bitsPerPixelPrecision == NV0073_CTRL_CMD_DP_GET_CAPS_DSC_BITS_PER_PIXEL_PRECISION_1_8)
    {
        dscInfo->gpuCaps.bitsPerPixelPrecision = DSC_BITS_PER_PIXEL_PRECISION_1_8;
    }

    if (bitsPerPixelPrecision == NV0073_CTRL_CMD_DP_GET_CAPS_DSC_BITS_PER_PIXEL_PRECISION_1_4)
    {
        dscInfo->gpuCaps.bitsPerPixelPrecision = DSC_BITS_PER_PIXEL_PRECISION_1_4;
    }

    if (bitsPerPixelPrecision == NV0073_CTRL_CMD_DP_GET_CAPS_DSC_BITS_PER_PIXEL_PRECISION_1_2)
    {
        dscInfo->gpuCaps.bitsPerPixelPrecision = DSC_BITS_PER_PIXEL_PRECISION_1_2;
    }

    if (bitsPerPixelPrecision == NV0073_CTRL_CMD_DP_GET_CAPS_DSC_BITS_PER_PIXEL_PRECISION_1)
    {
        dscInfo->gpuCaps.bitsPerPixelPrecision = DSC_BITS_PER_PIXEL_PRECISION_1;
    }

    dscInfo->gpuCaps.maxNumHztSlices = maxNumHztSlices;

    dscInfo->gpuCaps.lineBufferBitDepth = lineBufferBitDepth;
}

void ConnectorImpl::populateDscBranchCaps(DSC_INFO* dscInfo, DeviceImpl * dev)
{
    dscInfo->branchCaps.overallThroughputMode0 = dev->dscCaps.branchDSCOverallThroughputMode0;
    dscInfo->branchCaps.overallThroughputMode1 = dev->dscCaps.branchDSCOverallThroughputMode1;
    dscInfo->branchCaps.maxLineBufferWidth = dev->dscCaps.branchDSCMaximumLineBufferWidth;

    return;
}

void ConnectorImpl::populateDscSinkCaps(DSC_INFO* dscInfo, DeviceImpl * dev)
{
    // Early return if dscInfo or dev is NULL
    if ((dscInfo == NULL) || (dev == NULL))
    {
        return;
    }

    if (dev->dscCaps.dscDecoderColorFormatCaps.bRgb)
    {
        dscInfo->sinkCaps.decoderColorFormatMask |= DSC_DECODER_COLOR_FORMAT_RGB;
    }

    if (dev->dscCaps.dscDecoderColorFormatCaps.bYCbCr444)
    {
        dscInfo->sinkCaps.decoderColorFormatMask |= DSC_DECODER_COLOR_FORMAT_Y_CB_CR_444;
    }
    if (dev->dscCaps.dscDecoderColorFormatCaps.bYCbCrSimple422)
    {
        dscInfo->sinkCaps.decoderColorFormatMask |= DSC_DECODER_COLOR_FORMAT_Y_CB_CR_SIMPLE_422;
    }
    if (dev->dscCaps.dscDecoderColorFormatCaps.bYCbCrNative422)
    {
        dscInfo->sinkCaps.decoderColorFormatMask |= DSC_DECODER_COLOR_FORMAT_Y_CB_CR_NATIVE_422;
    }
    if (dev->dscCaps.dscDecoderColorFormatCaps.bYCbCrNative420)
    {
        dscInfo->sinkCaps.decoderColorFormatMask |= DSC_DECODER_COLOR_FORMAT_Y_CB_CR_NATIVE_420;
    }

    switch (dev->dscCaps.dscBitsPerPixelIncrement)
    {
        case BITS_PER_PIXEL_PRECISION_1_16:
            dscInfo->sinkCaps.bitsPerPixelPrecision = DSC_BITS_PER_PIXEL_PRECISION_1_16;
            break;
        case BITS_PER_PIXEL_PRECISION_1_8:
            dscInfo->sinkCaps.bitsPerPixelPrecision = DSC_BITS_PER_PIXEL_PRECISION_1_8;
            break;
        case BITS_PER_PIXEL_PRECISION_1_4:
            dscInfo->sinkCaps.bitsPerPixelPrecision = DSC_BITS_PER_PIXEL_PRECISION_1_4;
            break;
        case BITS_PER_PIXEL_PRECISION_1_2:
            dscInfo->sinkCaps.bitsPerPixelPrecision = DSC_BITS_PER_PIXEL_PRECISION_1_2;
            break;
        case BITS_PER_PIXEL_PRECISION_1:
            dscInfo->sinkCaps.bitsPerPixelPrecision = DSC_BITS_PER_PIXEL_PRECISION_1;
            break;
    }

    // Decoder color depth mask
    if (dev->dscCaps.dscDecoderColorDepthMask & DSC_BITS_PER_COLOR_MASK_12)
    {
        dscInfo->sinkCaps.decoderColorDepthMask |= DSC_DECODER_COLOR_DEPTH_CAPS_12_BITS;
    }

    if (dev->dscCaps.dscDecoderColorDepthMask & DSC_BITS_PER_COLOR_MASK_10)
    {
        dscInfo->sinkCaps.decoderColorDepthMask |= DSC_DECODER_COLOR_DEPTH_CAPS_10_BITS;
    }

    if (dev->dscCaps.dscDecoderColorDepthMask & DSC_BITS_PER_COLOR_MASK_8)
    {
        dscInfo->sinkCaps.decoderColorDepthMask |= DSC_DECODER_COLOR_DEPTH_CAPS_8_BITS;
    }

    dscInfo->sinkCaps.maxSliceWidth = dev->dscCaps.dscMaxSliceWidth;
    dscInfo->sinkCaps.sliceCountSupportedMask = dev->dscCaps.sliceCountSupportedMask;
    dscInfo->sinkCaps.maxNumHztSlices = dev->dscCaps.maxSlicesPerSink;
    dscInfo->sinkCaps.lineBufferBitDepth = dev->dscCaps.lineBufferBitDepth;
    dscInfo->sinkCaps.bBlockPrediction = dev->dscCaps.bDscBlockPredictionSupport;
    dscInfo->sinkCaps.algorithmRevision.versionMajor = dev->dscCaps.versionMajor;
    dscInfo->sinkCaps.algorithmRevision.versionMinor = dev->dscCaps.versionMinor;
    dscInfo->sinkCaps.peakThroughputMode0 = dev->dscCaps.dscPeakThroughputMode0;
    dscInfo->sinkCaps.peakThroughputMode1 = dev->dscCaps.dscPeakThroughputMode1;
    dscInfo->sinkCaps.maxBitsPerPixelX16 = dev->dscCaps.maxBitsPerPixelX16;

    // If panel does not populate peak DSC throughput, use _MODE0_340.
    if (!dscInfo->sinkCaps.peakThroughputMode0)
    {
        dscInfo->sinkCaps.peakThroughputMode0 =
            NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE0_340;
    }

    // If panel does not populate max slice width, use 2560.
    if (!dscInfo->sinkCaps.maxSliceWidth)
    {
        dscInfo->sinkCaps.maxSliceWidth = 2560;
    }

    //
    // If panel support Native 422 mode  but does not populate peak DSC
    // throughput, use _MODE0_340.
    //
    if (dev->dscCaps.dscDecoderColorFormatCaps.bYCbCrNative422 &&
            !dscInfo->sinkCaps.peakThroughputMode1)
    {
        dscInfo->sinkCaps.peakThroughputMode1 =
            NV_DPCD14_DSC_PEAK_THROUGHPUT_MODE1_340;
    }
}

void ConnectorImpl::populateForcedDscParams(DSC_INFO* dscInfo, DSC_INFO::FORCED_DSC_PARAMS * forcedParams)
{
    if(forcedParams)
    {
        dscInfo->forcedDscParams.sliceWidth = forcedParams->sliceWidth;
        dscInfo->forcedDscParams.sliceHeight = forcedParams->sliceHeight;
        dscInfo->forcedDscParams.sliceCount = forcedParams->sliceCount;
        dscInfo->forcedDscParams.dscRevision = forcedParams->dscRevision;
    }

}

void ConnectorImpl::populateDscCaps(DSC_INFO* dscInfo, DeviceImpl * dev, DSC_INFO::FORCED_DSC_PARAMS * forcedParams)
{
    // Sink DSC capabilities
    populateDscSinkCaps(dscInfo, dev);

    // Branch Specific DSC Capabilities
    if (!dev->isVideoSink() && !dev->isAudioSink())
    {
        populateDscBranchCaps(dscInfo, dev);
    }

    // GPU DSC capabilities
    populateDscGpuCaps(dscInfo);

    // Forced DSC params
    populateForcedDscParams(dscInfo, forcedParams);
}

bool ConnectorImpl::endCompoundQuery()
{
    DP_ASSERT(compoundQueryActive && "Spurious compoundQuery end.");
    compoundQueryActive = false;
    return compoundQueryResult;
}

//
//     Set link to HDMI mode
//
void ConnectorImpl::enableLinkHandsOff()
{
    if (isLinkQuiesced)
    {
        DP_ASSERT(0 && "Link is already quiesced.");
        return;
    }

    isLinkQuiesced = true;

    // Set the Lane Count to 0 to shut down the link.
    powerdownLink();
}

//
//     Restore from HDMI mode
//
void ConnectorImpl::releaseLinkHandsOff()
{
    if (!isLinkQuiesced)
    {
        DP_PRINTF(DP_ERROR, "DPCONN> Link is already in use.");
        return;
    }

    isLinkQuiesced = false;
    assessLink();
}

//
//     Timer callback for event management
//          Uses: fireEvents()
void ConnectorImpl::expired(const void * tag)
{
    if (tag == &tagFireEvents)
        fireEventsInternal();
    else if (tag == &tagDpBwAllocationChanged)
    {
        for (Device * i = enumDevices(0); i; i = enumDevices(i))
        {
            DeviceImpl * dev = (DeviceImpl *)i;
            sink->bandwidthChangeNotification(dev, false);
        }
    }
    else
    {
        DP_ASSERT(0);
    }
}

// Generate Events.
//          useTimer specifies whether we fire the events on the timer
//      context, or this context.
void ConnectorImpl::fireEvents()
{
    bool eventsPending = false;

    // Don't fire any events if we're not done with the modeset
    if (!intransitionGroups.isEmpty())
    {
        return;
    }

    // Walk through the devices looking for state changes
    for (ListElement * e = deviceList.begin(); e != deviceList.end(); e = e->next)
    {
        DeviceImpl * dev  = (DeviceImpl *)e;

        if (dev->isPendingNewDevice()  ||
            dev->isPendingLostDevice() ||
            dev->isPendingCableOk() ||
            dev->isPendingZombie() ||
            dev->isPendingHDCPCapDone())
            eventsPending = true;
    }

    // If there were any queue an immediate callback to handle them
    if (eventsPending || isDiscoveryDetectComplete)
    {
        {
            // Queue the fireEventsInternal.
            //   It's critical we don't allow this to be processed in a sleep
            //   since DD may do a modeset in response
            timer->queueCallback(this, &tagFireEvents, 0, false /* not allowed in sleep */);
        }
    }
}

void ConnectorImpl::fireEventsInternal()
{
    ListElement * next;
    Address::StringBuffer sb, sb1;
    DP_USED(sb);
    DP_USED(sb1);
    for (ListElement * e = deviceList.begin(); e != deviceList.end(); e = next)
    {
        next = e->next;
        DeviceImpl * dev  = (DeviceImpl *)e;

        if (dev->isPendingLostDevice())
        {
            //
            // For bug 2335599, where the connected monitor is switched to MST
            // from SST after S3 resume, we need to disconnect SST monitor
            // early before adding MST monitors. This will avoid client from
            // mistaking the disconnection of SST monitor later as parent of
            // MST monitors, which will wrongly disconnect MST monitors too.
            //
            if (!(!dev->multistream && linkUseMultistream()) &&
                bDeferNotifyLostDevice)
            {
                continue;
            }
            dev->shadow.plugged = false;
            DP_PRINTF(DP_ERROR, "DPCONN> Lost device %s", dev->address.toString(sb));
            Address::NvU32Buffer addrBuffer;
            dpMemZero(addrBuffer, sizeof(addrBuffer));
            dev->address.toNvU32Buffer(addrBuffer);
            NV_DPTRACE_WARNING(LOST_DEVICE, dev->address.size(), addrBuffer[0], addrBuffer[1],
                                   addrBuffer[2], addrBuffer[3]);
            sink->lostDevice(dev);
#if defined(DEBUG)
            // Assert that this device is not contained in any groups.
            List* groupLists[] = {
                &activeGroups,
                &inactiveGroups
            };

            for (unsigned i = 0; i < sizeof(groupLists) / sizeof(groupLists[0]); i++)
            {
                List *groupList = groupLists[i];
                for (ListElement *e = groupList->begin(); e != groupList->end(); e = e->next)
                {
                    GroupImpl *g = (GroupImpl *)e;
                    DP_ASSERT(!g->contains(dev));
                }
            }
#endif
            delete dev;

            // Now that the device is deleted, update the DP Tunnel BW allocation
            NvU64 previousAllocatedDpTunnelBw = allocatedDpTunnelBw;
            updateDpTunnelBwAllocation();
            if (previousAllocatedDpTunnelBw != allocatedDpTunnelBw)
            {
                timer->queueCallback(this, &tagDpBwAllocationChanged, 0, false /* not allowed in sleep */);
            }

            continue;
        }

        if (dev->isPendingCableOk())
        {
            dev->shadow.cableOk = dev->isCableOk();
            sink->notifyCableOkStateChange(dev, dev->shadow.cableOk);
        }

        if (dev->isPendingZombie())
        {
            dev->shadow.zombie =  dev->isZombie();
            if (dev->complianceDeviceEdidReadTest)
            {
                // the zombie event will be hidden for DD/OS
                DP_PRINTF(DP_WARNING, "DPCONN> Compliance: Device Internal Zombie? :  %d 0x%x", dev->shadow.zombie ? 1 : 0, dev);
                return;
            }
            bMitigateZombie = false;
            DP_PRINTF(DP_WARNING, "DPCONN> Zombie? :  %d 0x%x", dev->shadow.zombie ? 1 : 0, dev);
            sink->notifyZombieStateChange(dev, dev->shadow.zombie);
        }

        if (dev->isPendingHDCPCapDone())
        {
            DP_ASSERT(dev->isHDCPCap != Indeterminate && "HDCPCap reading is not done!!");
            if (dev->isHDCPCap != Indeterminate)
            {
                // Notify RM about the new Bcaps..
                if (dev->isActive())
                {
                    RmDfpCache dfpCache = {0};
                    dfpCache.updMask = 0;
                    dfpCache.bcaps = *dev->BCAPS;
                    for (unsigned i=0; i<HDCP_KSV_SIZE; i++)
                        dfpCache.bksv[i] = dev->BKSV[i];

                    dfpCache.updMask |= (1 << NV0073_CTRL_DFP_UPDATE_DYNAMIC_DFP_CACHE_MASK_BCAPS);
                    dfpCache.updMask |= (1 << NV0073_CTRL_DFP_UPDATE_DYNAMIC_DFP_CACHE_MASK_BKSV);
                    dev->connector->main->rmUpdateDynamicDfpCache(dev->activeGroup->headIndex, &dfpCache, False);
                }

                sink->notifyHDCPCapDone(dev, !!dev->isHDCPCap);
                DP_PRINTF(DP_NOTICE, "DPCONN> Notify HDCP cap Done : %x", !!dev->isHDCPCap);
            }
            else
            {
                sink->notifyHDCPCapDone(dev, false);
            }

            dev->shadow.hdcpCapDone = true;
        }

        bool mustDisconnect = dev->isMustDisconnect();
        if (dev->shadow.mustDisconnect != mustDisconnect && mustDisconnect)
        {
            dev->shadow.mustDisconnect = mustDisconnect;
            sink->notifyMustDisconnect(dev->activeGroup);
        }
    }

    for (ListElement * e = deviceList.begin(); e != deviceList.end(); e = next)
    {
        next = e->next;
        DeviceImpl * dev  = (DeviceImpl *)e;

        if (dev->isPendingNewDevice())
        {
            if (bReportDeviceLostBeforeNew && bDeferNotifyLostDevice)
            {
                // Let's try to find if there's a device pending lost on the same address
                DeviceImpl* _device = NULL;
                for (ListElement * le = deviceList.begin(); le != deviceList.end(); le = le->next)
                {
                    _device = (DeviceImpl*)le;
                    if ((_device->address == dev->address) && (_device->plugged != dev->plugged))
                        break;
                }
                if (_device &&
                    (_device->address == dev->address) &&
                    (_device->plugged != dev->plugged))
                {
                    // If yes, then we need to report this lost device first.
                    _device->shadow.plugged = false;
                    DP_PRINTF(DP_WARNING, "DPCONN> Lost device 0x%x", _device);
                    sink->lostDevice(_device);
                    DP_ASSERT(!_device->activeGroup && "DD didn't remove panel from group");
                    delete _device;
                }
            }
            dev->shadow.plugged = true;
            if (dev->isDSCPossible())
            {
                if (dev->isDSCSupported())
                {
                    DP_PRINTF(DP_NOTICE, "DPCONN> New device %s | Native DSC Capability - Capable | "
                              "DSC Decompression Device - %s", dev->address.toString(sb),
                              (dev->devDoingDscDecompression) ? dev->devDoingDscDecompression->address.toString(sb1):"NA");
                }
                else
                {
                    DP_PRINTF(DP_NOTICE, "DPCONN> New device %s | Native DSC Capability - Not Capable | "
                              "DSC Decompression Device - %s", dev->address.toString(sb),
                              (dev->devDoingDscDecompression) ? dev->devDoingDscDecompression->address.toString(sb1):"NA");
                }
            }
            else
            {
                DP_PRINTF(DP_NOTICE, "DPCONN> New device %s | DSC Not Possible", dev->address.toString(sb));
            }

            Address::NvU32Buffer addrBuffer;
            dpMemZero(addrBuffer, sizeof(addrBuffer));
            dev->address.toNvU32Buffer(addrBuffer);
            NV_DPTRACE_INFO(NEW_SINK_REPORTED, dev->address.size(), addrBuffer[0], addrBuffer[1],
                                addrBuffer[2], addrBuffer[3]);

            //
            // During newDevice, clients would run CQA for modelist validation and to identify the max BW
            // For these calls, we expect to run CQA as if the monitor is driven independently
            // Update the SW cache of the allocatedDpTunnelBw to Link Configuration
            // The newDevice sequence would call dev->setModeList() which would allocate the BW required for all devices
            // and set the right value for allocatedDpTunnelBw.
            // Additionally, the client is expected to lock DPLib during all the operations so that no
            // other thread/IRQ can update allocatedDpTunnelBw
            //
            allocatedDpTunnelBwShadow = allocatedDpTunnelBw;
            allocatedDpTunnelBw = getMaxTunnelBw();
            sink->newDevice(dev);
        }
    }

    if (isDiscoveryDetectComplete)
    {
        //
        // Bug 200236666 :
        // isDiscoveryDetectComplete can be set when we process a new device after
        // completing last edid read. In such scenario we will send notifyDetectComplete
        // before newDevice for that sink has been sent to DD
        //    a/ sink->newDevice(dev) above can trigger the pending edid read
        //    b/ after last edid read completes (::mstEdidCompleted), ::processNewDevice
        //       will set the plugged flag for new device
        //    c/ this will queue pendingNewDevice event callback for the last device pending discovery
        //    d/ isDiscoveryDetectComplete flag set during b/ will trigger a
        //       premature notifyDetectComplete to DD before pendingNewDevice callback
        // To fix above scenario : check if there is any newly pending new/lost device
        //                         if yes, then defer sending notifyDetectComplete till next callback
        //
        bool bDeferNotifyDetectComplete = false;
        for (ListElement * e = deviceList.begin(); e != deviceList.end(); e = next)
        {
            next = e->next;
            DeviceImpl * dev  = (DeviceImpl *)e;

            if (dev->isPendingNewDevice() || dev->isPendingLostDevice())
            {
                bDeferNotifyDetectComplete = true;
                DP_ASSERT(0 && "DP-CONN> Defer notifyDetectComplete as a new/lost device is pending!");
                break;
            }
        }

        if (!bDeferNotifyDetectComplete)
        {
            isDiscoveryDetectComplete = false;
            DP_PRINTF(DP_NOTICE, "DP-CONN> NotifyDetectComplete");
            sink->notifyDetectComplete();
        }
    }

}

//
// This call will be deprecated as soon as all clients move to the new API
//
bool ConnectorImpl::isHeadShutDownNeeded(Group * target,               // Group of panels we're attaching to this head
                                         unsigned headIndex,
                                         unsigned twoChannelAudioHz,   // if you need 192khz stereo specify 192000 here
                                         unsigned eightChannelAudioHz, // Same setting for multi channel audio.
                                         //  DisplayPort encodes 3-8 channel streams as 8 channel
                                         NvU64 pixelClockHz,           // Requested pixel clock for the mode
                                         unsigned rasterWidth,
                                         unsigned rasterHeight,
                                         unsigned rasterBlankStartX,
                                         unsigned rasterBlankEndX,
                                         unsigned depth)
{
    ModesetInfo modesetInfo = ModesetInfo(twoChannelAudioHz, eightChannelAudioHz, pixelClockHz,
                                          rasterWidth, rasterHeight, (rasterBlankStartX - rasterBlankEndX), 0 /*surfaceHeight*/,
                                          depth, rasterBlankStartX, rasterBlankEndX);
    return isHeadShutDownNeeded(target, headIndex, modesetInfo);
}

//
// Head shutdown will be needed if any of the following conditions are true:
// a. Link rate is going lower than current
// b. Head is activated as MST
//
bool ConnectorImpl::isHeadShutDownNeeded(Group * target,               // Group of panels we're attaching to this head
                                         unsigned headIndex,
                                         ModesetInfo modesetInfo)
{
    if (bForceHeadShutdownFromRegkey || bForceHeadShutdownPerMonitor)
    {
        return true;
    }

    if (linkUseMultistream())
    {
         return true;
    }

    if (activeGroups.isEmpty())
    {
        return false;
    }

    bool bHeadShutdownNeeded = true;
    LinkConfiguration lowestSelected = getMaxLinkConfig();

    // Force highestLink config in SST
    bool bSkipLowestConfigCheck = false;
    bool bIsModeSupported = false;

    GroupImpl* targetImpl = (GroupImpl*)target;

    // Certain panels only work when link train to highest linkConfig in SST mode.
    for (Device * i = enumDevices(0); i; i=enumDevices(i))
    {
        DeviceImpl * dev = (DeviceImpl *)i;
        if (dev->forceMaxLinkConfig())
        {
            bSkipLowestConfigCheck = true;
        }
    }

    //
    // Check if there is a special request from the client,
    // If so, skip lowering down the link config.
    //
    if (this->preferredLinkConfig.isValid())
    {
        lowestSelected = preferredLinkConfig;
        bSkipLowestConfigCheck = true;
    }

    // If the flag is set, simply neglect downgrading to lowest possible linkConfig
    if (!bSkipLowestConfigCheck)
    {
        LinkConfiguration lConfig = lowestSelected;

        bIsModeSupported = getValidLowestLinkConfig(lConfig, lowestSelected, modesetInfo, NULL);
    }
    else
    {
        if (this->willLinkSupportModeSST(lowestSelected, modesetInfo, NULL))
        {
            bIsModeSupported = true;
        }
    }

    if (bIsModeSupported)
    {
        //
        // This is to handle a case where we query current link config
        // to UEFI during boot time and it fails to return. Currently
        // we do not handle this scenario and head is not shut down
        // though it's actually required. This is to allow head shutdown
        // in such cases.
        //
        if (!isLinkActive())
        {
            return true;
        }

        //
        // In case of DSC, if bpc is changing, we need to shut down the head
        // since PPS can change
        // In case of mode transition (DSC <-> non-DSC), if the link config is same as previous mode, we need to shut down the head
        // since VBID[6] needs to be updated accordingly
        //
        if ((bForceHeadShutdownOnModeTransition &&
            ((modesetInfo.bEnableDsc && targetImpl->lastModesetInfo.bEnableDsc) &&
             (modesetInfo.bitsPerComponent != targetImpl->lastModesetInfo.bitsPerComponent))) ||
            ((lowestSelected.getTotalDataRate() == activeLinkConfig.getTotalDataRate()) &&
             (modesetInfo.bEnableDsc != targetImpl->lastModesetInfo.bEnableDsc)))
        {
            return true;
        }

        // For dual DP while changing link config, we need to shut
        // down the head
        if (lowestSelected.lanes == 8)
        {
            // If link config is changing, head shutdown will be needed.
            if ((activeLinkConfig.lanes == lowestSelected.lanes) &&
                (activeLinkConfig.peakRate == lowestSelected.peakRate))
            {
                bHeadShutdownNeeded = false;
            }
        }
        //
        // If link config is going lower then we need to shut down the
        // head. If we link train to a lower config before reducing the
        // mode, we will hang the HW since head would still be driving
        // the higher mode at the time of link train.
        //
        else if ((lowestSelected.getTotalDataRate()) >= (activeLinkConfig.getTotalDataRate()))
        {
            bHeadShutdownNeeded = false;
        }
    }
    else
    {
        DP_ASSERT( 0 && "DP-CONN> This mode is not possible at any link configuration!");
    }

    if (targetImpl)
    {
        targetImpl->bIsHeadShutdownNeeded = bHeadShutdownNeeded;
    }

    return bHeadShutdownNeeded;
}

bool ConnectorImpl::isLinkTrainingNeededForModeset(ModesetInfo modesetInfo)
{
    // Force highestLink config in SST
    bool bSkipLowestConfigCheck      = false;
    bool bIsModeSupported            = false;
    LinkConfiguration lowestSelected = getMaxLinkConfig();

    if (linkUseMultistream())
    {
        if (!isLinkActive())
        {
            // If MST, we always need to link train if link is not active
            return true;
        }
        else if (lowestSelected != activeLinkConfig)
        {
            //
            // If the link is active, we have to retrain, if active Link Config is
            // not the highest possible Link Config.
            //
            return true;
        }
        else
        {
            //
            // We don't have to retrain if link is active and at highest possible config
            // since for MST we should always link train to highest possible Link Config.
            //
            return false;
        }
    }

    //
    // Link training is needed if link is not alive OR alive but inactive
    // ie., lane status reports symbol lock/interlane align/CR failures
    //
    if (isLinkLost() || !isLinkActive())
    {
        return true;
    }

    //
    // Link training is needed if link config was previously guessed (not assessed by the driver).
    // The link config is marked as guessed in below cases -
    //    a. Invalid link rate returned by UEFI
    //    b. When max link config is HBR3 and currently assessed by UEFI != HBR3
    //    c. If a SOR is not assigned to display during link assessment
    //
    if (this->linkGuessed)
    {
        return true;
    }

    // Certain panels only work when link train to highest linkConfig in SST mode.
    for (Device * i = enumDevices(0); i; i=enumDevices(i))
    {
        DeviceImpl * dev = (DeviceImpl *)i;
        if (dev->forceMaxLinkConfig())
        {
            bSkipLowestConfigCheck = true;
        }
    }

    //
    // Check if there is a special request from the client,
    // If so, skip lowering down the link config.
    //
    if (this->preferredLinkConfig.isValid())
    {
        lowestSelected = preferredLinkConfig;
        bSkipLowestConfigCheck = true;
    }

    // If the flag is set, simply neglect downgrading to lowest possible linkConfig
    if (!bSkipLowestConfigCheck)
    {
        LinkConfiguration lConfig = lowestSelected;

        bIsModeSupported = getValidLowestLinkConfig(lConfig, lowestSelected, modesetInfo, NULL);
    }
    else
    {
        if (this->willLinkSupportModeSST(lowestSelected, modesetInfo, NULL))
        {
            bIsModeSupported = true;
        }
    }

    //
    // Link training is needed if requested mode/link config is
    // different from the active mode/link config
    //
    if (bIsModeSupported)
    {
        if ((activeLinkConfig.lanes != lowestSelected.lanes) ||
                (activeLinkConfig.peakRate != lowestSelected.peakRate))
        {
            return true;
        }
    }
    else
    {
        DP_ASSERT( 0 && "DP-CONN> This mode is not possible at any link configuration!");
    }

    return false;
}

bool DisplayPort::SetConfigSingleHeadMultiStreamMode(Group **targets,
                                                NvU32 displayIDs[],
                                                NvU32 numStreams,
                                                DP_SINGLE_HEAD_MULTI_STREAM_MODE mode,
                                                bool bSetConfig,
                                                NvU8 vbiosPrimaryDispIdIndex,
                                                bool bEnableAudioOverRightPanel)
{
    GroupImpl     *pTargetImpl = NULL;
    ConnectorImpl *pConnectorImpl = NULL;
    ConnectorImpl *pPrevConnectorImpl = NULL;

    if (numStreams > NV0073_CTRL_CMD_DP_SINGLE_HEAD_MAX_STREAMS || numStreams <= 0)
    {
        DP_PRINTF(DP_ERROR, "DP-CONN> ERROR: in configuring single head multistream mode "
                          "invalid number of streams");
        return false;
    }

    for (NvU32 iter = 0; iter < numStreams; iter++)
    {
        pTargetImpl = (GroupImpl*)targets[iter];

        if(pTargetImpl == NULL)
        {
            DP_PRINTF(DP_ERROR, "DP-CONN> ERROR: in configuring single head multistream mode:"
                      "invalid target passed by client");
            return false;
        }

        pConnectorImpl = (ConnectorImpl*) (pTargetImpl->parent);

        if (bSetConfig)
        {
            if (DP_SINGLE_HEAD_MULTI_STREAM_MODE_SST == mode)
            {
                //
                // Detach any active firmware groups before configuring singleHead dual SST
                //
                if (pTargetImpl->isHeadAttached() && pTargetImpl->headInFirmware)
                {
                    pConnectorImpl->notifyDetachBegin(NULL);
                    pConnectorImpl->notifyDetachEnd();
                }

                if (displayIDs[iter] != pConnectorImpl->main->getRootDisplayId())
                {
                    DP_ASSERT( 0 && "DP-CONN> invalid single head multistream SST configuration !");
                    return false;
                }

                // 0th index is primary connector index,
                // 1st is secondary connector index so on
                if (iter > DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY)
                {
                    pPrevConnectorImpl->pCoupledConnector = pConnectorImpl;
                    if (iter == (numStreams - 1))
                    {
                        pConnectorImpl->pCoupledConnector =
                            (ConnectorImpl*)((GroupImpl*)targets[DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY])->parent;
                    }
                    // Clear secondary connector's link guessed state
                    pConnectorImpl->linkGuessed = false;
                }

                pPrevConnectorImpl = pConnectorImpl;
            }

            pTargetImpl->singleHeadMultiStreamMode = mode;
            pTargetImpl->singleHeadMultiStreamID = (DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID)iter;

            // Save the 'Audio over Right Pannel' configuration in Connector Impl
            // Use this configuration when SF gets programed.
            if (bEnableAudioOverRightPanel)
            {
                pConnectorImpl->bAudioOverRightPanel = true;
            }
        }
        else
        {
            pTargetImpl->singleHeadMultiStreamMode = DP_SINGLE_HEAD_MULTI_STREAM_MODE_NONE;
            pTargetImpl->singleHeadMultiStreamID = DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY;
            pConnectorImpl->pCoupledConnector = NULL;
            pConnectorImpl->bAudioOverRightPanel = false;
        }
    }

    pConnectorImpl->main->configureSingleHeadMultiStreamMode(displayIDs,
                    numStreams,
                    (NvU32)mode,
                    bSetConfig,
                    vbiosPrimaryDispIdIndex);

    return true;
}

//
// This call will be deprecated as soon as all clients move to the new API
//
bool ConnectorImpl::notifyAttachBegin(Group * target,               // Group of panels we're attaching to this head
                                      unsigned headIndex,
                                      unsigned twoChannelAudioHz,         // if you need 192khz stereo specify 192000 here
                                      unsigned eightChannelAudioHz,       // Same setting for multi channel audio.
                                      //  DisplayPort encodes 3-8 channel streams as 8 channel
                                      NvU64 pixelClockHz,                 // Requested pixel clock for the mode
                                      unsigned rasterWidth,
                                      unsigned rasterHeight,
                                      unsigned rasterBlankStartX,
                                      unsigned rasterBlankEndX,
                                      unsigned depth)
{
    ModesetInfo modesetInfo(twoChannelAudioHz, eightChannelAudioHz, pixelClockHz, rasterWidth,
                            rasterHeight, (rasterBlankStartX - rasterBlankEndX), 0 /*surfaceHeight*/,
                            depth, rasterBlankStartX, rasterBlankEndX);

    DpModesetParams modesetParams(headIndex, modesetInfo);

    return notifyAttachBegin (target, modesetParams);
}

bool ConnectorImpl::setDeviceDscState(Device * dev, bool bEnableDsc)
{
    if (!((DeviceImpl *)dev)->isDSCPossible())
    {
        return true;
    }

    if (bEnableDsc)
    {
        if(!(((DeviceImpl *)dev)->setDscEnable(true /*bEnableDsc*/)))
        {
            DP_ASSERT(!"DP-CONN> Failed to configure DSC on Sink!");
            return false;
        }

        if (!dscEnabledDevices.contains(dev))
            dscEnabledDevices.insertFront(dev);
    }
    else
    {
        bool bCurrDscEnable = false;
        // Get Current DSC Enable State
        if (!((DeviceImpl *)dev)->getDscEnable(&bCurrDscEnable))
        {
            DP_PRINTF(DP_WARNING, "DP> Not able to get DSC Enable State!");
        }

        if (bCurrDscEnable)
        {

            bool bDisableDsc = true;
            // Before disabling DSC check if other device with same parent has DSC enabled or not
            for (Device * i = dscEnabledDevices.next(NULL); i != NULL; i = dscEnabledDevices.next(i))
            {
                if ((i != dev) && (((DeviceImpl *)i)->parent == ((DeviceImpl *)dev)->parent))
                {
                    DP_PRINTF(DP_WARNING, "Parent is shared among devices and other device has DSC enabled so we can't disable DSC");
                    bDisableDsc = false;
                    break;
                }
            }

            if(bDisableDsc && !((DeviceImpl *)dev)->setDscEnable(false /*bEnableDsc*/))
            {
                DP_ASSERT(!"DP-CONN> Failed to configure DSC on Sink!");
                return false;
            }
        }

        if (dscEnabledDevices.contains(dev))
            dscEnabledDevices.remove(dev);
    }
    return true;
}

bool ConnectorImpl::needToEnableFEC(const DpPreModesetParams &params)
{
    for (NvU32 i = 0; i < NV_MAX_HEADS; i++)
    {
        if ((params.headMask & NVBIT(i)) == 0x0)
            continue;

        if ((params.head[i].pTarget == NULL) ||
            !params.head[i].pModesetParams->modesetInfo.bEnableDsc)
            continue;

        // eDP can support DSC with and without FEC
        DeviceImpl * nativeDev = this->findDeviceInList(Address());
        if (this->main->isEDP() && nativeDev)
            return nativeDev->getFECSupport();
        else
            return true;
    }

    return false;
}

void ConnectorImpl::dpPreModeset(const DpPreModesetParams &params)
{
    this->bFECEnable |= this->needToEnableFEC(params);

    DP_ASSERT(this->inTransitionHeadMask == 0x0);
    this->inTransitionHeadMask = 0x0;

    for (NvU32 i = 0; i < NV_MAX_HEADS; i++)
    {
        if ((params.headMask & NVBIT(i)) == 0x0)
            continue;

        this->inTransitionHeadMask |= NVBIT(i);

        if (params.head[i].pTarget != NULL)
        {
            DP_ASSERT(params.head[i].pModesetParams->headIndex == i);
            this->notifyAttachBegin(params.head[i].pTarget,
                                    *params.head[i].pModesetParams);
        }
        else
        {
            this->notifyDetachBegin(this->perHeadAttachedGroup[i]);
        }
        this->perHeadAttachedGroup[i] = params.head[i].pTarget;
    }
}

void ConnectorImpl::dpPostModeset(void)
{
    for (NvU32 i = 0; i < NV_MAX_HEADS; i++)
    {
        if ((this->inTransitionHeadMask & NVBIT(i)) == 0x0)
            continue;

        if (this->perHeadAttachedGroup[i] != NULL)
            this->notifyAttachEnd(false);
        else
            this->notifyDetachEnd();

        this->inTransitionHeadMask &= ~NVBIT(i);
    }
}

//
// Notify library before/after modeset (update)
// Here is what NAB essentially does:
//   0. Makes sure TMDS is not attached
//   1. Trains link to optimized link config ("optimized" depends on DP1.1, DP1.2)
//   2. Performs quick watermark check for IMP. If IMP is not possible, forces link, zombies devices
//   3. if anything of above fails, marks devices in given group as zombies
//
// Return : true  - NAB passed
//          false - NAB failed due to invalid params or link training failure
//                  Link configs are forced in case of link training failure
//
bool ConnectorImpl::notifyAttachBegin(Group *                target,       // Group of panels we're attaching to this head
                                      const DpModesetParams       &modesetParams)
{
    unsigned   twoChannelAudioHz         = modesetParams.modesetInfo.twoChannelAudioHz;
    unsigned   eightChannelAudioHz       = modesetParams.modesetInfo.eightChannelAudioHz;
    NvU64      pixelClockHz              = modesetParams.modesetInfo.pixelClockHz;
    unsigned   rasterWidth               = modesetParams.modesetInfo.rasterWidth;
    unsigned   rasterHeight              = modesetParams.modesetInfo.rasterHeight;
    unsigned   rasterBlankStartX         = modesetParams.modesetInfo.rasterBlankStartX;
    unsigned   rasterBlankEndX           = modesetParams.modesetInfo.rasterBlankEndX;
    unsigned   depth                     = modesetParams.modesetInfo.depth;
    bool       bLinkTrainingStatus       = true;
    bool       bEnableDsc                = modesetParams.modesetInfo.bEnableDsc;
    bool       bEnableFEC;
    bool       bEnablePassThroughForPCON = modesetParams.modesetInfo.bEnablePassThroughForPCON;
    Device     *newDev                   = target->enumDevices(0);
    DeviceImpl *dev                      = (DeviceImpl *)newDev;

    if (hal->isDpTunnelBwAllocationEnabled() &&
        ((allocatedDpTunnelBwShadow != 0) ||
         (allocatedDpTunnelBw == 0)))
    {
        //
        // We should never be here.
        // One possible reason this could happen is if client missed calling setModeList after a newDevice.
        // At this point we are definitely in need for mode BW than allocated.
        // We could either try to greedily allocate BW = LC or assert and fail.
        // However given that DpLib's SW state is possibly incorrect, safer to could assert and fail.
        //
        DP_ASSERT(!"Shadow BW non zero or no BW allocated. Failing notifyAttachBegin");
        return false;
    }

    if(preferredLinkConfig.isValid())
    {
        bEnableFEC = preferredLinkConfig.bEnableFEC;
    }
    else
    {
        DeviceImpl * nativeDev = findDeviceInList(Address());
        if (main->isEDP() && nativeDev)
        {
            // eDP can support DSC with and without FEC
            bEnableFEC = bEnableDsc && nativeDev->getFECSupport();
        }
        else
        {
            bEnableFEC = bEnableDsc;
        }
    }

    DP_PRINTF(DP_NOTICE, "DPCONN> Notify Attach Begin (Head %d, pclk %d raster %d x %d  %d bpp)",
              modesetParams.headIndex, pixelClockHz, rasterWidth, rasterHeight, depth);
    NV_DPTRACE_INFO(NOTIFY_ATTACH_BEGIN, modesetParams.headIndex, pixelClockHz, rasterWidth, rasterHeight,
                       depth, bEnableDsc, bEnableFEC);

    if (!depth || !pixelClockHz)
    {
        DP_ASSERT(!"DP-CONN> Params with zero value passed to query!");
        return false;
    }

    if ((modesetParams.modesetInfo.mode == DSC_DUAL) ||
        (modesetParams.modesetInfo.mode == DSC_DROP))
    {
        if ((modesetParams.headIndex == NV_SECONDARY_HEAD_INDEX_1) ||
            (modesetParams.headIndex == NV_SECONDARY_HEAD_INDEX_3))
        {
            DP_ASSERT(!"DP-CONN> For Two Head One OR, client should send Primary Head index!");
            return false;
        }
    }

    for (Device * dev = target->enumDevices(0); dev; dev = target->enumDevices(dev))
    {
        Address::StringBuffer buffer;
        DP_USED(buffer);
        if (dev->isVideoSink())
        {
            DP_PRINTF(DP_NOTICE, "DPCONN> | %s (VIDEO) |", dev->getTopologyAddress().toString(buffer));
        }
        else
        {
            DP_PRINTF(DP_NOTICE, "DPCONN> | %s (BRANCH) |", dev->getTopologyAddress().toString(buffer));
        }
    }

    if (firmwareGroup && ((GroupImpl *)firmwareGroup)->headInFirmware)
    {
        DP_ASSERT(bIsUefiSystem || (0 && "DPCONN> Firmware still active on head. De-activating"));
    }

    GroupImpl* targetImpl = (GroupImpl*)target;

    if (bEnableDsc)
    {
        switch (modesetParams.modesetInfo.mode)
        {
            case DSC_SINGLE:
                DP_PRINTF(DP_NOTICE, "DPCONN> DSC Mode = SINGLE");
                break;
            case DSC_DUAL:
                DP_PRINTF(DP_NOTICE, "DPCONN> DSC Mode = DUAL");
                break;
            case DSC_DROP:
                DP_PRINTF(DP_NOTICE, "DPCONN> DSC Mode = DROP");
                break;
            case DSC_MODE_NONE:
                DP_PRINTF(DP_NOTICE, "DPCONN> DSC Mode = NONE");
                break;
        }

        targetImpl->dscModeRequest = modesetParams.modesetInfo.mode;
    }

    DP_ASSERT(!(targetImpl->isHeadAttached() && targetImpl->bIsHeadShutdownNeeded) && "Head should have been shut down but it is still active!");

    targetImpl->headInFirmware = false;
    if (firmwareGroup)
    {
        ((GroupImpl *)firmwareGroup)->headInFirmware = false;
    }

    if (firmwareGroup && activeGroups.contains((GroupImpl*)firmwareGroup))
    {
        if (((GroupImpl *)firmwareGroup)->isHeadAttached())
        {
            targetImpl->setHeadAttached(true);
        }
        activeGroups.remove((GroupImpl*)firmwareGroup);
        inactiveGroups.insertBack((GroupImpl*)firmwareGroup);
    }

    if (this->linkGuessed && (targetImpl->singleHeadMultiStreamMode != DP_SINGLE_HEAD_MULTI_STREAM_MODE_SST))
    {
        DP_ASSERT(!(this->linkGuessed) && "Link was not assessed previously. Probable reason: system was not in driver mode. Assessing now.");
        this->assessLink();
    }

    DP_ASSERT(this->isLinkQuiesced == 0 && "According to bracketting calls TMDS/alternate DP still active!");

    // Transfer the group to active list
    inactiveGroups.remove(targetImpl);
    activeGroups.insertBack(targetImpl);
    intransitionGroups.insertFront(targetImpl);

    if (modesetParams.colorFormat == dpColorFormat_YCbCr422 &&
        dev && dev->dscCaps.dscDecoderColorFormatCaps.bYCbCrNative422)
    {
        targetImpl->lastModesetInfo = ModesetInfo(twoChannelAudioHz, eightChannelAudioHz,
                                      pixelClockHz, rasterWidth, rasterHeight,
                                      (rasterBlankStartX - rasterBlankEndX), modesetParams.modesetInfo.surfaceHeight,
                                      depth, rasterBlankStartX, rasterBlankEndX, bEnableDsc, modesetParams.modesetInfo.mode,
                                      false, dpColorFormat_YCbCr422_Native);
    }
    else
    {
        targetImpl->lastModesetInfo = ModesetInfo(twoChannelAudioHz, eightChannelAudioHz,
                                      pixelClockHz, rasterWidth, rasterHeight,
                                      (rasterBlankStartX - rasterBlankEndX), modesetParams.modesetInfo.surfaceHeight,
                                      depth, rasterBlankStartX, rasterBlankEndX, bEnableDsc, modesetParams.modesetInfo.mode,
                                      false, modesetParams.colorFormat);
    }

    targetImpl->headIndex = modesetParams.headIndex;
    targetImpl->streamIndex = main->headToStream(modesetParams.headIndex, (messageManager != NULL), targetImpl->singleHeadMultiStreamID);
    targetImpl->colorFormat = modesetParams.colorFormat;

    DP_ASSERT(!this->isLinkQuiesced && "TMDS is attached, NABegin is impossible!");

    //
    // Update the FEC enabled flag according to the mode requested.
    //
    // In MST config, if one panel needs DSC/FEC and the other one does not,
    // we still need to keep FEC enabled on the connector since at least one
    // stream needs it.
    //
    this->bFECEnable |= bEnableFEC;

    highestAssessedLC.enableFEC(this->bFECEnable);

    if (main->isEDP())
    {
        main->configurePowerState(true);
        if (bOuiCached)
        {
            hal->setOuiSource(cachedSourceOUI, &cachedSourceModelName[0],
                              6 /* string length of ieeeOuiDevId */,
                              cachedSourceChipRevision);
        }
        else
        {
            if (!this->bSkipZeroOuiCache)
            {
                DP_ASSERT("eDP Source OUI is not cached!");
            }
            else
            {
                this->performIeeeOuiHandshake();
            }
        }
    }

    LinkConfiguration maxLinkConfig = getMaxLinkConfig();

    // if failed, we're guaranteed that assessed link rate didn't meet the mode requirements
    // isZombie() will catch this
    bLinkTrainingStatus = trainLinkOptimized(maxLinkConfig);

    // If panel supports DSC, set DSC enabled/disabled
    // according to the mode requested.

    for (Device * dev = target->enumDevices(0); dev; dev = target->enumDevices(dev))
    {
        if (bPConConnected)
        {
            if (!(((DeviceImpl *)dev)->setDscEnableDPToHDMIPCON(bEnableDsc, bEnablePassThroughForPCON)))
            {
                DP_ASSERT(!"DP-CONN> Failed to configure DSC on DP to HDMI PCON!");
            }
        }
        else if(!setDeviceDscState(dev, bEnableDsc))
        {
            DP_ASSERT(!"DP-CONN> Failed to configure DSC on Sink!");
        }
    }

// TODO: Need to check if we can completely remove DP_OPTION_HDCP_12_ENABLED and remove it

    beforeAddStream(targetImpl);

    if (linkUseMultistream())
    {
        // Which pipeline to take the affect out of trigger ACT
        if ((targetImpl->singleHeadMultiStreamMode != DP_SINGLE_HEAD_MULTI_STREAM_MODE_MST) ||
            (targetImpl->singleHeadMultiStreamID   == DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY))
        {
            main->configureTriggerSelect(targetImpl->headIndex, targetImpl->singleHeadMultiStreamID);
        }
    }

    if (!linkUseMultistream() || main->supportMSAOverMST())
    {
        bool enableInbandStereoSignaling = false;

        DP_ASSERT(activeGroups.isEmpty() == false);

        if (main->isInbandStereoSignalingSupported())
        {
            enableInbandStereoSignaling = true;
        }

        //
        // Bug 200362535
        // setDpStereoMSAParameters does not cache the msa params. It will immediately
        // apply just the stereo specific parameters. This is required because we
        // can toggle the msa params using nvidia control panel and in that scenario
        // we do not get supervisor interrupts. Since SV interrupts do not occur the
        // msa parameters do not get applied. So to avoid having to reboot to apply the
        // stereo msa params setDpStereoMSAParameters is called.
        //
        // setDpMSAParameters will contain all msa params, including stereo cached.
        // These will be applied during supervisor interrupt. So if we will get
        // SV interrupts later the same stereo settings will be applied twice.
        // first by setDpStereoMSAParameters and later by setDpMSAParameters.
        //
        main->setDpStereoMSAParameters(!enableInbandStereoSignaling, modesetParams.msaparams);
        main->setDpMSAParameters(!enableInbandStereoSignaling, modesetParams.msaparams);
    }

    NV_DPTRACE_INFO(NOTIFY_ATTACH_BEGIN_STATUS, bLinkTrainingStatus);

    bFromResumeToNAB = false;
    return bLinkTrainingStatus;
}


//
// modesetCancelled True, when DD respected NAB failure and cancelled modeset.
//                  False, when NAB succeeded, or DD didn't honor NAB failure
//
// Here is what NAE supposed to do:
// 1. modesetCancelled == TRUE, NAB failed:
//         unzombie all devices and set linkForced to false; We have Status Quo for next modeset
// 2. modesetCancelled == False, NAB failed:
//        If NAB failed, linkForces is TRUE. NAE goes finds zombied devices and notifies DD about them.
// 3. modesetCancelled == False, NAB succeeded:
//        NAE is no-op. (but we have some special sanity code)
//
void ConnectorImpl::notifyAttachEnd(bool modesetCancelled)
{
    GroupImpl* currentModesetDeviceGroup = NULL;
    DP_PRINTF(DP_NOTICE, "DPCONN> Notify Attach End");
    NV_DPTRACE_INFO(NOTIFY_ATTACH_END);

    bFromResumeToNAB = false;

    if (intransitionGroups.isEmpty())
    {
        DP_ASSERT( 0 && "INVALID STATE: Modeset Group is NULL");
        return;
    }

    currentModesetDeviceGroup = intransitionGroups.pop();

    if (modesetCancelled)
    {
        currentModesetDeviceGroup->setHeadAttached(false);
    }

    // set dscModeActive to what was requested in NAB and clear dscModeRequest
    currentModesetDeviceGroup->dscModeActive = currentModesetDeviceGroup->dscModeRequest;
    currentModesetDeviceGroup->dscModeRequest = DSC_MODE_NONE;

    currentModesetDeviceGroup->setHeadAttached(true);
    RmDfpCache dfpCache = {0};
    dfpCache.updMask = 0;
    if (currentModesetDeviceGroup->isHeadAttached())
    {
        for (DeviceImpl * dev = (DeviceImpl *)currentModesetDeviceGroup->enumDevices(0);
            dev; dev = (DeviceImpl *)currentModesetDeviceGroup->enumDevices(dev))
        {
            dfpCache.bcaps = *dev->BCAPS;
            for (unsigned i=0; i<HDCP_KSV_SIZE; i++)
                dfpCache.bksv[i] = dev->BKSV[i];

            dfpCache.updMask |= (1 << NV0073_CTRL_DFP_UPDATE_DYNAMIC_DFP_CACHE_MASK_BCAPS);
            dfpCache.updMask |= (1 << NV0073_CTRL_DFP_UPDATE_DYNAMIC_DFP_CACHE_MASK_BKSV);
            main->rmUpdateDynamicDfpCache(dev->activeGroup->headIndex, &dfpCache, True);

            // Remove this while enabling HDCP for MSC
            break;
        }
    }

    //
    // Add rest of the streams (other than primary) in notifyAE, since this can't be done
    // unless a SOR is attached to a Head (part of modeset), and trigger ACT immediate
    //
    if ((currentModesetDeviceGroup->singleHeadMultiStreamMode == DP_SINGLE_HEAD_MULTI_STREAM_MODE_MST) &&
        (currentModesetDeviceGroup->singleHeadMultiStreamID >    DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY))
    {
        DP_ASSERT(linkUseMultistream() && "it should be multistream link to configure single head MST");
        hal->payloadTableClearACT();
        hal->payloadAllocate(currentModesetDeviceGroup->streamIndex,
            currentModesetDeviceGroup->timeslot.begin, currentModesetDeviceGroup->timeslot.count);
        main->configureTriggerSelect(currentModesetDeviceGroup->headIndex, currentModesetDeviceGroup->singleHeadMultiStreamID);
        main->triggerACT();
    }

    afterAddStream(currentModesetDeviceGroup);

    //
    // Turn on the Authentication/Encryption back if previous is on.
    // For DP1.1, let the upstream to turn it back.
    // For DP1.2, we should turn the modeset back if it was on.
    // The authentication will be called off during the modeset.
    //
    HDCPState hdcpState = {0};
    main->configureHDCPGetHDCPState(hdcpState);
    if ((!hdcpState.HDCP_State_Authenticated) && (isHDCPAuthOn == true)
        && (currentModesetDeviceGroup->hdcpEnabled))
    {
        if (!this->linkUseMultistream())
        {
            currentModesetDeviceGroup->hdcpEnabled = isHDCPAuthOn = false;
        }
    }

    fireEvents();
}

// Notify library before/after shutdown (update)
void ConnectorImpl::notifyDetachBegin(Group * target)
{
    if (!target)
        target = firmwareGroup;

    NV_DPTRACE_INFO(NOTIFY_DETACH_BEGIN);

    GroupImpl * group = (GroupImpl*)target;

    DP_PRINTF(DP_NOTICE, "DPCONN> Notify detach begin");
    DP_ASSERT((group->headInFirmware || group->isHeadAttached()) && "Disconnecting an inactive device");

    // check to see if a pattern request was on. if yes clear the pattern
    PatternInfo pattern_info;
    pattern_info.lqsPattern = hal->getPhyTestPattern();
    // send control call to rm for the pattern
    if (pattern_info.lqsPattern != LINK_QUAL_DISABLED)
    {
        pattern_info.lqsPattern = LINK_QUAL_DISABLED;
        if (!main->physicalLayerSetTestPattern(&pattern_info))
            DP_ASSERT(0 && "Could not set the PHY_TEST_PATTERN");
    }

    beforeDeleteStream(group);

    //
    // Set the trigger select so as to which frontend corresponding to the stream
    // to take the affect
    //
    if (linkUseMultistream())
    {
        main->configureTriggerSelect(group->headIndex, group->singleHeadMultiStreamID);

        // Clear payload of other than primary streams and trigger ACT immediate
        if ((group->singleHeadMultiStreamMode == DP_SINGLE_HEAD_MULTI_STREAM_MODE_MST) &&
            (group->singleHeadMultiStreamID   != DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY))
        {
            main->triggerACT();
            if (!hal->payloadWaitForACTReceived())
            {
                DP_PRINTF(DP_ERROR, "DP-TS> Downstream device did not receive ACT during stream clear");
                DP_ASSERT(0);
            }
        }
    }

    intransitionGroups.insertFront(group);
}

//
// Here is what NDE does:
//  1. delete unplugged devices (they were zombies, if they're on this list)
//  2. unmark zombies (they were plugged zombies, they might want to get link trained next time)
//  3. mark head as detached (so that we can delete any HPD unplugged devices)
//
void ConnectorImpl::notifyDetachEnd(bool bKeepOdAlive)
{
    GroupImpl* currentModesetDeviceGroup = NULL;
    DP_PRINTF(DP_NOTICE, "DPCONN> Notify detach end");
    NV_DPTRACE_INFO(NOTIFY_DETACH_END);

    if (intransitionGroups.isEmpty())
    {
        DP_ASSERT( 0 && "INVALID STATE: Modeset Group is NULL");
        return;
    }

    currentModesetDeviceGroup = intransitionGroups.pop();

    afterDeleteStream(currentModesetDeviceGroup);

    if (!linkUseMultistream())
    {
        Device * d = 0;
        for (d = currentModesetDeviceGroup->enumDevices(0);
             currentModesetDeviceGroup->enumDevices(d) != 0;
             d = currentModesetDeviceGroup->enumDevices(d))
        {
            // only one device in the group
            DP_ASSERT(d && (((DeviceImpl*)d)->activeGroup == currentModesetDeviceGroup));
        }
    }

    // nullify last modeset info
    dpMemZero(&currentModesetDeviceGroup->lastModesetInfo, sizeof(ModesetInfo));
    currentModesetDeviceGroup->setHeadAttached(false);
    currentModesetDeviceGroup->headInFirmware = false;
    currentModesetDeviceGroup->dscModeActive = DSC_MODE_NONE;

    // Mark head as disconnected
    bNoLtDoneAfterHeadDetach = true;

    //
    // Update the last modeset HDCP status here. Hdcp got disabled after modeset
    // thus hdcpPreviousStatus would be false to SST after device inserted.
    //
    HDCPState hdcpState = {0};
    main->configureHDCPGetHDCPState(hdcpState);
    if (!(isHDCPAuthOn = hdcpState.HDCP_State_Authenticated))
    {
        currentModesetDeviceGroup->hdcpEnabled = false;
    }

    // Update Vbios scratch register
    for (Device * d = currentModesetDeviceGroup->enumDevices(0); d;
         d = currentModesetDeviceGroup->enumDevices(d))
    {
        currentModesetDeviceGroup->updateVbiosScratchRegister(d);
    }

    // Reset value of bIsHeadShutdownNeeded to get rid of false asserts
    currentModesetDeviceGroup->bIsHeadShutdownNeeded = false;

    // If this is eDP and the LCD power is not ON, we don't need to Disable DSC here
    bool bPanelPwrSts = true;
    if ((!main->isEDP()) || (main->getEdpPowerData(&bPanelPwrSts, NULL) && bPanelPwrSts))
    {
        // Disable DSC decompression on the panel if panel supports DSC and reset bFECEnable Flag
        for (Device * dev = currentModesetDeviceGroup->enumDevices(0); dev; dev = currentModesetDeviceGroup->enumDevices(dev))
        {
            if(!(setDeviceDscState(dev, false/*bEnableDsc*/)))
            {
                DP_ASSERT(!"DP-CONN> Failed to configure DSC on Sink!");
            }
        }
    }

    // Transfer to inactive group and cancel pending callbacks for that group.
    currentModesetDeviceGroup->cancelHdcpCallbacks();
    activeGroups.remove(currentModesetDeviceGroup);
    inactiveGroups.insertBack(currentModesetDeviceGroup);

    if (activeGroups.isEmpty())
    {
        cancelHdcpCallbacks();

        // We disconnected a panel, try to clear the transition
        if (linkAwaitingTransition)
        {
            assessLink();
        }
        //
        // Power down the links as we have switched away from the monitor.
        // Only power down if we are in single stream
        //
        else
        {
            //
            // Power down the links as we have switched away from the monitor.
            // For shared SOR case, we need this to keep SW stats in DP instances in sync.
            // Only power down the link when it's not a compliance test device.
            //
            // Some eDP panels are known having problems when power down.
            // See bug 1425706, 1376753, 1347872, 1355592
            //
            // Hotplug may trigger detach before processNewDevice if previous state has
            // lost device not yet detached. Avoid to powerdown for the case for following
            // device discovery hdcp probe.
            //
            if (!bIsDiscoveryDetectActive)
                powerdownLink(!main->skipPowerdownEdpPanelWhenHeadDetach() && !bKeepOdAlive);
        }
        if (this->policyModesetOrderMitigation && this->modesetOrderMitigation)
            this->modesetOrderMitigation = false;
    }
    fireEvents();
}

bool ConnectorImpl::trainPCONFrlLink(PCONLinkControl *pconControl)
{
    NvU32   loopCount   = NV_PCON_SOURCE_CONTROL_MODE_TIMEOUT_THRESHOLD;
    NvU32   frlRateMask = 0;
    bool    bFrlReady   = false;
    bool    result      = false;

    // Initial return values.
    pconControl->result.trainedFrlBwMask    = 0;
    pconControl->result.maxFrlBwTrained     = PCON_HDMI_LINK_BW_FRL_INVALID;

    // Step 1: Setup PCON for later operation

    // Step 1.1: Set D0 power
    hal->setPowerState(PowerStateD0);

    hal->resetProtocolConverter();

    // Step 1.2: Enable Source Control Mode and FRL mode, enable FRL-Ready IRQ
    hal->setSourceControlMode(true, true);

    do
    {
        //
        // Step 1.3: Poll for HDMI-Link-Status Change (0x2005 Bit 3)
        //           Get FRL Ready Bit (0x303B Bit 1)
        //
        hal->checkPCONFrlReady(&bFrlReady);
        if (bFrlReady == true)
        {
            break;
        }
        Timeout timeout(this->timer, NV_PCON_SOURCE_CONTROL_MODE_TIMEOUT_INTERVAL_MS);
        while(timeout.valid());
        continue;
    } while (--loopCount);

    if (bFrlReady == false)
    {
        pconControl->result.status = NV_DP_PCON_CONTROL_STATUS_ERROR_TIMEOUT;
        return false;
    }

    // Step 2: Assess FRL Link capability.

    //
    // Step 2.1: Configure FRL Link (FRL BW, BW mask / Concurrent)
    // Start with mask for all bandwidth. Please refer to definition of DPCD 0x305B.
    //
    result = hal->setupPCONFrlLinkAssessment(pconControl->frlHdmiBwMask,
                                             pconControl->flags.bExtendedLTMode,
                                             pconControl->flags.bConcurrentMode);
    if (result == false)
    {
        pconControl->result.status = NV_DP_PCON_CONTROL_STATUS_ERROR_GENERIC;
        return false;
    }

    // Step 2.2: Poll for HDMI-Link-Status Change (0x2005 Bit 3)
    loopCount = NV_PCON_FRL_LT_TIMEOUT_THRESHOLD;
    do
    {
        result = hal->checkPCONFrlLinkStatus(&frlRateMask);
        if (result == true)
        {
            break;
        }
        Timeout timeout(this->timer, NV_PCON_FRL_LT_TIMEOUT_INTERVAL_MS);
        while(timeout.valid());
        continue;
    } while (--loopCount);

    if (result == true)
    {
        //
        // frlRateMask is result from checkPCONFrlLinkStatus (0x3036) Bit 1~6.
        //
        pconControl->result.status              = NV_DP_PCON_CONTROL_STATUS_SUCCESS;
        pconControl->result.trainedFrlBwMask    = frlRateMask;
        pconControl->result.maxFrlBwTrained     = getMaxFrlBwFromMask(frlRateMask);
    }
    else
    {
        pconControl->result.status              = NV_DP_PCON_CONTROL_STATUS_ERROR_FRL_LT_FAILURE;
    }
    return result;
}

bool ConnectorImpl::assessPCONLinkCapability(PCONLinkControl *pConControl)
{
    if (pConControl == NULL || !this->previousPlugged)
        return false;

    bool bIsFlushModeEnabled = enableFlush();

    if (!bIsFlushModeEnabled)
    {
        return false;
    }

    if (pConControl->flags.bSourceControlMode)
    {
        if (trainPCONFrlLink(pConControl) == false)
        {
            // restore Autonomous mode and treat this as an active DP dongle.
            hal->resetProtocolConverter();
            // Exit flush mode
            disableFlush();
            if (!pConControl->flags.bSkipFallback)
            {
                bSkipAssessLinkForPCon = false;
                assessLink();
            }
            return false;
        }
        activePConLinkControl.flags = pConControl->flags;
        activePConLinkControl.frlHdmiBwMask = pConControl->frlHdmiBwMask;
        activePConLinkControl.result = pConControl->result;
    }
    else
    {
        // restore Autonomous mode and treat this as an active DP dongle.
        hal->resetProtocolConverter();
    }

    // Step 3: Assess DP Link capability.
    LinkConfiguration lConfig = getMaxLinkConfig();
    highestAssessedLC = getMaxLinkConfig();

    hal->updateDPCDOffline();
    if (hal->isDpcdOffline())
    {
        disableFlush();
        return false;
    }
    if (!train(lConfig, false /* do not force LT */))
    {
        //
        // Note that now train() handles fallback, activeLinkConfig
        // has the max link config that was assessed.
        //
        lConfig = activeLinkConfig;
    }

    highestAssessedLC = lConfig;
    linkGuessed = false;
    disableFlush();

    this->bKeepLinkAliveForPCON = pConControl->flags.bKeepPCONLinkAlive;
    return true;
}

bool ConnectorImpl::getOuiSink(unsigned &ouiId, unsigned char * modelName, size_t modelNameBufferSize, NvU8 & chipRevision)
{
    if (!previousPlugged || !hal->getOuiSupported())
        return false;

    return hal->getOuiSink(ouiId, modelName, modelNameBufferSize, chipRevision);
}

void ConnectorImpl::setIgnoreSourceOuiHandshake(bool bIgnoreOuiHandShake)
{
    bIgnoreSrcOuiHandshake = bIgnoreOuiHandShake;
}

bool ConnectorImpl::getIgnoreSourceOuiHandshake()
{
    return bIgnoreSrcOuiHandshake;
}

bool ConnectorImpl::performIeeeOuiHandshake()
{
    const char *ieeeOuiDevId = "NVIDIA";

    if (!hal->getOuiSupported() || getIgnoreSourceOuiHandshake())
        return false;

    if (hal->setOuiSource(DPCD_OUI_NVIDIA, ieeeOuiDevId, 6 /* string length of ieeeOuiDevId */, 0) == AuxRetry::ack)
    {
        NvU8 chipRevision = 0;

        // parse client OUI.
        if (hal->getOuiSink(ouiId, &modelName[0], sizeof(modelName), chipRevision))
        {
            DP_PRINTF(DP_NOTICE, "DP> SINK-OUI id(0x%08x) %s: rev:%d.%d", ouiId,
                          (NvU8*)modelName,
                          (unsigned)DRF_VAL(_DPCD, _SINK_HARDWARE_REV, _MAJOR, chipRevision),
                          (unsigned)DRF_VAL(_DPCD, _SINK_HARDWARE_REV, _MINOR, chipRevision));
            return true;
        }
    }
    return false;
}

bool ConnectorImpl::willLinkSupportModeSST
(
    const LinkConfiguration & linkConfig,
    const ModesetInfo & modesetInfo,
    const DscParams *pDscParams
)
{
    DP_ASSERT(!linkUseMultistream() && "IMP for SST only");

    //
    // mode is not known yet, we have to report is possible
    // Otherwise we're going to mark all devices as zombies on first HPD(c),
    // since modeset info is not available.
    //
    if (modesetInfo.pixelClockHz == 0)
        return true;

    if (linkConfig.lanes == 0 || linkConfig.peakRate == 0)
        return false;

    Watermark water;

    if (this->isFECSupported())
    {
        if (!isModePossibleSSTWithFEC(linkConfig, modesetInfo, &water, main->hasIncreasedWatermarkLimits()))
        {
            // Verify audio
            return false;
        }
    }
    else
    {
        if (!isModePossibleSST(linkConfig, modesetInfo, &water, main->hasIncreasedWatermarkLimits()))
        {
            // Verify audio
            return false;
        }
    }
    return true;
}

// gets max values for DPCD HAL and forces link trainig with that config
void ConnectorImpl::forceLinkTraining()
{
    LinkConfiguration forcedMaxConfig(getMaxLinkConfig());
    train(forcedMaxConfig, true);
}

void ConnectorImpl::powerdownLink(bool bPowerdownPanel)
{
    bool bPanelPwrSts = true;

    LinkConfiguration powerOff = getMaxLinkConfig();
    powerOff.lanes             = 0;
    powerOff.peakRate          = dp2LinkRate_1_62Gbps; // Set to lowest peakRate
    powerOff.setChannelCoding();

    if (linkUseMultistream() && bPowerDownPhyBeforeD3)
    {
        // Inform Sink about Main Link Power Down.
        PowerDownPhyMessage powerDownPhyMsg;
        NakData nack;

        for (Device * i = enumDevices(0); i; i=enumDevices(i))
        {
            if (i->isPlugged() && i->isVideoSink())
            {
                Address devAddress = ((DeviceImpl*)i)->address;
                powerDownPhyMsg.set(devAddress.parent(), devAddress.tail(), NV_TRUE);
                this->messageManager->send(&powerDownPhyMsg, nack);
            }
        }
    }

    //
    // 1> If it is eDP and the power is not on, we don't need to put it into D3 here
    // 2> If FEC is enabled then we have to put panel in D3 after powering down mainlink
    //    as FEC disable has to be detected by panel which will happen as part of link
    //    power down, we need to keep panel in D0 for this.
    //
    if (!this->bFECEnable &&
        ((!main->isEDP()) || (main->getEdpPowerData(&bPanelPwrSts, NULL) && bPanelPwrSts)))
    {
        hal->setPowerState(PowerStateD3);
    }

    train(powerOff, !bPowerdownPanel);  // Train to 0 laneCount, RBR linkRate (powerDown sequence)

    //
    // If FEC is enabled, put panel to D3 here for non-eDP.
    // For eDP with FEC support, FEC state would be cleared as part of panel
    // power down
    //
    if (this->bFECEnable && (!main->isEDP()))
    {
        hal->setPowerState(PowerStateD3);
    }

    // Set FEC state as false in link power down
    this->bFECEnable = false;
    highestAssessedLC.enableFEC(false);
}

GroupImpl * ConnectorImpl::getActiveGroupForSST()
{
    if (this->linkUseMultistream())
        return 0;
    GroupImpl * groupAttached = 0;
    for (ListElement * e = activeGroups.begin(); e != activeGroups.end(); e = e->next)
    {
        // there should only be one group for the connector.
        if (groupAttached)
        {
            DP_ASSERT(0 && "Multiple attached heads");
            return 0;
        }
        groupAttached = (GroupImpl * )e;
    }
    return groupAttached;
}

bool ConnectorImpl::trainSingleHeadMultipleSSTLinkNotAlive(GroupImpl *pGroupAttached)
{
    GroupImpl *pPriGrpAttached =  NULL;
    GroupImpl *pSecGrpAttached =  NULL;
    ConnectorImpl *pPriConnImpl = NULL;
    ConnectorImpl *pSecConnImpl = NULL;

    if ((pGroupAttached == NULL) ||
        (pCoupledConnector == NULL) ||
        (pGroupAttached->singleHeadMultiStreamMode != DP_SINGLE_HEAD_MULTI_STREAM_MODE_SST))
    {
        return false;
    }
    if (pGroupAttached->singleHeadMultiStreamID == DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY)
    {
        pSecGrpAttached = pCoupledConnector->getActiveGroupForSST();
        pPriGrpAttached = pGroupAttached;
        pSecConnImpl = pCoupledConnector;
        pPriConnImpl = this;
    }
    else if (pGroupAttached->singleHeadMultiStreamID == DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_SECONDARY)
    {
        pPriGrpAttached = pCoupledConnector->getActiveGroupForSST();
        pSecGrpAttached = pGroupAttached;
        pPriConnImpl = pCoupledConnector;
        pSecConnImpl = this;
    }
    else
    {
        DP_ASSERT(0 && "Invalid 2-SST configuration ");
        return false;
    }

    if (!pPriGrpAttached || !pSecGrpAttached || !pPriConnImpl || !pSecConnImpl)
    {
        DP_ASSERT(0 && "Invalid 2-SST configuration ");
        return false;
    }

    if (!pPriConnImpl->trainLinkOptimizedSingleHeadMultipleSST(pPriGrpAttached))
    {
        DP_ASSERT(0 && "not able to configure 2-SST mode on primary link");
        return false;
    }

    if (!pSecConnImpl->trainLinkOptimizedSingleHeadMultipleSST(pSecGrpAttached))
    {
        DP_ASSERT(0 && "not able to configure 2-SST mode for secondary link");
        return false;
    }

    return true;
}

TriState ConnectorImpl::requestDpTunnelBw(NvU8 requestBw)
{
    TriState status = Indeterminate;

    if (!hal->writeDpTunnelRequestedBw(requestBw))
    {
        return status;
    }

    Timeout timeout(this->timer, DP_TUNNEL_REQUEST_BW_MAX_TIME_MS);
    do
    {
        timer->sleep(DP_TUNNEL_REQUEST_BW_POLLING_INTERVAL_MS);
        status = hal->getDpTunnelBwRequestStatus();
        if (status != Indeterminate)
        {
            break;
        }
    } while(timeout.valid());

    return status;
}

/*!
 * @brief Interface to allow client to enable BW allocation support
 */
void ConnectorImpl::enableDpTunnelingBwAllocationSupport()
{
    // If regkey is set to disable, return early
    if (bForceDisableTunnelBwAllocation)
    {
        return;
    }

    hal->enableDpTunnelingBwAllocationSupport();
}

/*!
 * @brief Allocate the requested Tunnel BW
 *
 * @return      maximum tunnel bw required for this connector
 *
 */
NvU64 ConnectorImpl::getMaxTunnelBw()
{
    return highestAssessedLC.getTotalDataRate() * 8;
}

/*!
 * @brief Allocate the requested Tunnel BW
 *
 * @param[in]   bandwidth    Requested BW in bps
 * @return      boolean to indicate success/failure
 *
 */
bool ConnectorImpl::allocateDpTunnelBw(NvU64 bandwidth)
{
    NvU8      estimatedBw           = 0;
    NvU8      granularityMultiplier = 0;
    NvU8      requestBw             = 0;
    TriState  requestStatus         = Indeterminate;

    if (!hal->isDpTunnelBwAllocationEnabled())
    {
        DP_PRINTF(DP_NOTICE, "Bw allocation not enabled");
        return false;
    }

    // Threshold the request bw to the max link configuration
    if (bandwidth > getMaxTunnelBw())
    {
        bandwidth = getMaxTunnelBw();
    }

    if (!hal->getDpTunnelEstimatedBw(estimatedBw))
    {
        return false;
    }

    if (!hal->getDpTunnelGranularityMultiplier(granularityMultiplier))
    {
        return false;
    }

    DP_PRINTF(DP_INFO, "Estimated BW: %d Mbps, Requested BW: %d Mbps",
              ((NvU64) estimatedBw * 1000) / (NvU64) granularityMultiplier,
              bandwidth / (1000 * 1000));

    //
    // Granularity is in Gbps. Eg: 0.25 Gpbs, 0.5 Gpbs, 1 Gbps
    // bandwidth is in bps
    // granularityMultiplier is 1/Granularity
    // bandwidth  = DPCD Value * Granularity
    //            = DPCD Value / granularityMultiplier
    // DPCD Value = bandwidth * granularityMultiplier
    //
    requestBw = (NvU8) divide_ceil(bandwidth * granularityMultiplier, 1000 * 1000 * 1000);

    if (requestBw > estimatedBw)
    {
        requestBw = estimatedBw;
    }

    requestStatus = requestDpTunnelBw(requestBw);
    // This shouldn't be Indeterminate. The request can succeed or fail. Indeterminate means something else went wrong
    if (requestStatus == Indeterminate)
    {
        DP_PRINTF(DP_ERROR, "Tunneling chip didn't reply for the BW request\n");
        return false;
    }

    if (requestStatus == False)
    {
        // As per DP spec, if the allocation fails, the Estimated BW now contains the actual BW. Request estimatedBW now
        if (!hal->getDpTunnelEstimatedBw(estimatedBw))
        {
            return false;
        }

        if (!hal->getDpTunnelGranularityMultiplier(granularityMultiplier))
        {
            return false;
        }

        DP_PRINTF(DP_INFO, "Failed to get requested BW, requesting updated Estimated BW: %d\n",
                  ((NvU64) estimatedBw * 1000) / (NvU64) granularityMultiplier);

        requestBw = estimatedBw;
        requestStatus = requestDpTunnelBw(requestBw);
        //
        // This shouldn't be Indeterminate. The request can succeed or fail.
        // Intrdeterminate means something else went wrong
        //
        if (requestStatus == Indeterminate)
        {
            return false;
        }
    }

    if (requestStatus == True)
    {
        // Convert this back to bps and record the allocated BW
        this->allocatedDpTunnelBw       = ((NvU64) requestBw * 1000 * 1000 * 1000) / (NvU64) granularityMultiplier;
        this->allocatedDpTunnelBwShadow = 0;
        DP_PRINTF(DP_INFO, "Allocated BW: %d Mbps", this->allocatedDpTunnelBw / (1000 * 1000));
    }

    return requestStatus;
}

bool ConnectorImpl::allocateMaxDpTunnelBw()
{
    if (!hal->isDpTunnelBwAllocationEnabled())
    {
        return true;
    }

    NvU64 bandwidth = getMaxTunnelBw();
    if (!allocateDpTunnelBw(bandwidth))
    {
        DP_PRINTF(DP_ERROR, "Failed to allocate DP Tunnel BW. Requested BW: %d Mbps",
                  bandwidth / (1000 * 1000));
        return false;
    }

    return true;
}

void ConnectorImpl::assessLink(LinkTrainingType trainType)
{
    this->bSkipLt = false;  // Assesslink should never skip LT, so let's reset it in case it was set.
    bool  bLinkStateToggle = false;
    NvU32 retryCount = 0;

    LinkConfiguration _maxLinkConfig = getMaxLinkConfig();

    // Cap system max link configuration to preferredLinkConfig
    if (preferredLinkConfig.isValid() && this->forcePreferredLinkConfig)
    {
        _maxLinkConfig = preferredLinkConfig;
    }

    if (bSkipAssessLinkForPCon)
    {
        // Skip assessLink() for PCON. client should call assessPCONLinkCapability later.
        return;
    }

    if (trainType == NO_LINK_TRAINING)
    {
        train(preferredLinkConfig, false, trainType);
        return;
    }

    if (isLinkQuiesced ||
        (firmwareGroup && ((GroupImpl *)firmwareGroup)->headInFirmware))
    {
        highestAssessedLC = _maxLinkConfig;
        if (bIsUefiSystem && !hal->getSupportsMultistream())
        {
            //
            // Since this is a UEFI based system which can provide max link config
            // supported on this panel. So try to get the max supported link config
            // and update the highestAssessedLC. Once done set linkGuessed as false.
            //
            unsigned laneCount = 0;
            NvU64    linkRate = 0;
            bool     bFECEnabled = false;
            NvU8     linkRateFromUefi, laneCountFromUefi;

            // Query the max link config if provided by UEFI.
            if ((!linkGuessed) && (main->getMaxLinkConfigFromUefi(linkRateFromUefi, laneCountFromUefi)))
            {
                laneCount = laneCountFromUefi;

                if (linkRateFromUefi == 0x6)
                {
                    linkRate = dp2LinkRate_1_62Gbps;
                }
                else if (linkRateFromUefi == 0xA)
                {
                    linkRate = dp2LinkRate_2_70Gbps;
                }
                else if (linkRateFromUefi == 0x14)
                {
                    linkRate = dp2LinkRate_5_40Gbps;
                }
                else if (linkRateFromUefi == 0x1E)
                {
                    linkRate = dp2LinkRate_8_10Gbps;
                }
                else
                {
                    DP_ASSERT(0 && "DP> Invalid link rate returned from UEFI!");
                    linkGuessed = true;
                }

                if ((highestAssessedLC.peakRate == dp2LinkRate_8_10Gbps) &&
                    (linkRate != dp2LinkRate_8_10Gbps))
                {
                    //
                    // UEFI does not support HBR3 yet (The support will be added in Volta).
                    // Mark the link as guessed when max supported link config is HBR3 and
                    // the currently assessed link config, by UEFI is not the highest, to
                    // force the link assessment by driver.
                    //
                    linkGuessed = true;
                }
                else
                {
                    //
                    // SW policy change: If the BIOS max link config isn't same as max of panel, mark DPlib for re-link
                    // assessment by marking linkGuessed as true.
                    // Re-link training is prefereable over glitchless and booting at low resolutions
                    //
                    if (laneCount != highestAssessedLC.lanes || linkRate != highestAssessedLC.peakRate)
                    {
                        linkGuessed = true;
                    }
                    else
                    {
                        linkGuessed = false;
                        // Update software state with latest link status info
                        hal->setDirtyLinkStatus(true);
                        hal->refreshLinkStatus();
                    }
                }
            }
            else if (!linkGuessed)
            {
                // We failed to query max link config from UEFI. Mark link as guessed.
                DP_PRINTF(DP_WARNING, "DP CONN> Failed to query max link config from UEFI.");
                linkGuessed = true;
            }

            if (!linkGuessed)
            {
                // Update SW state with UEFI provided max link config
                highestAssessedLC = LinkConfiguration (&this->linkPolicy,
                                                       laneCount, linkRate,
                                                       this->hal->getEnhancedFraming(),
                                                       linkUseMultistream(),
                                                       false, /* disablePostLTRequest */
                                                       false, /* bEnableFEC */
                                                       false, /* bDisableLTTPR */
                                                       this->getDownspreadDisabled());

                // Get the currently applied linkconfig and update SW state
                getCurrentLinkConfigWithFEC(laneCount, linkRate, bFECEnabled);

                activeLinkConfig = LinkConfiguration (&this->linkPolicy,
                                                      laneCount, linkRate,
                                                      this->hal->getEnhancedFraming(),
                                                      linkUseMultistream(),
                                                      false, /* disablePostLTRequest */
                                                      false, /* bEnableFEC */
                                                      false, /* bDisableLTTPR */
                                                      this->getDownspreadDisabled());
            }
        }
        else
        {
            linkGuessed = true;
        }

        allocateMaxDpTunnelBw();
        return;
    }

    if (linkAwaitingTransition)
    {
        if (activeGroups.isEmpty())
        {
            linkState = hal->getSupportsMultistream() ?
                DP_TRANSPORT_MODE_MULTI_STREAM : DP_TRANSPORT_MODE_SINGLE_STREAM;
            linkAwaitingTransition = false;
            bLinkStateToggle = true;
        }
        else
        {
            //
            // If modesetOrderMitigation isn't on, we need to reassess
            // immediately. This is because we will report the connects at the
            // same time as the disconnects.  IMP Query can be done immediately
            // on connects. On the other hand if modeset order mitigation is
            // off - all attached devices are going to be reported as
            // disconnected and might as well use the old configuration.
            //
            if (this->policyModesetOrderMitigation && this->modesetOrderMitigation)
                return;
        }
    }
    else
    {
        if (hal->isDpcdOffline())
            linkState = DP_TRANSPORT_MODE_INIT;
    }

    // linkState might be different from beginning, update _maxLinkConfig to keep it in sync.
    _maxLinkConfig.multistream = this->linkUseMultistream();

    //
    // Bug 1545352: This is done to avoid shutting down a display for freeing up a SOR for LT,
    // when no SOR is assigned properly to the connector. It can happen when more
    // than max supported number of display(s) is connected.
    // It came as a requirement from some clients to avoid glitches when shutting
    // down a display to make SOR availability for those monitors.
    //
    if (main->getSorIndex() == DP_INVALID_SOR_INDEX)
    {
        highestAssessedLC = _maxLinkConfig;
        linkGuessed = true;
        return;
    }

    LinkConfiguration lConfig = _maxLinkConfig;

    LinkConfiguration preFlushModeActiveLinkConfig = activeLinkConfig;

    if (main->isInternalPanelDynamicMuxCapable())
    {
        // Skip Link assessment for Dynamic MUX capable Internal Panel
        if ((activeLinkConfig.lanes == lConfig.lanes) &&
            (activeLinkConfig.peakRate == lConfig.peakRate) &&
            (!isLinkInD3()) && (!isLinkLost()))
        {
            linkGuessed = false;
            return;
        }
    }

    // Find the active group(s)
    GroupImpl * groupAttached = 0;
    for (ListElement * e = activeGroups.begin(); e != activeGroups.end(); e = e->next)
    {
        DP_ASSERT(bIsUefiSystem || linkUseMultistream() || (!groupAttached && "Multiple attached heads"));
        groupAttached = (GroupImpl * )e;
    }

    //  Disconnect heads
    bool bIsFlushModeEnabled = enableFlush();

    if (bIsFlushModeEnabled)
    {
        do
        {
            //
            // if dpcd is offline; avoid assessing. Just consider max.
            // keep lowering lane/rate config till train succeeds
            //
            hal->updateDPCDOffline();

            // At first trial / when retraining, always start with _maxLinkConfig
            lConfig = _maxLinkConfig;
            if (hal->isDpcdOffline())
            {
                break;
            }
            if (!train(lConfig, false /* do not force LT */))
            {
                //
                // Note that now train() handles fallback, activeLinkConfig
                // has the max link config that was assessed.
                //
                lConfig = activeLinkConfig;
            }
            // Check if training completed at _maxLinkConfig, no need to retry
            if (lConfig == _maxLinkConfig || lConfig.lanes == 0)
            {
                break;
            }
            timer->sleep(40);
        } while (retryCount++ < WAR_MAX_REASSESS_ATTEMPT);

        if (!activeLinkConfig.isValid())
        {
            if (groupAttached && groupAttached->lastModesetInfo.pixelClockHz != 0)
            {
                // If there is no active link, force LT to max before disable flush
                lConfig = _maxLinkConfig;
                train(lConfig, true);
            }
        }
        disableFlush();
    }

    if (lConfig != _maxLinkConfig)
    {
        if (lConfig.lanes == 0)
        {
            DP_PRINTF(DP_NOTICE, "DP> assessLink(): Device unplugged or offline.");
        }
        else
        {
            DP_PRINTF(DP_WARNING,
                      "DP> assessLink(): Failed to reach max link configuration (%d x %d).",
                      lConfig.lanes, lConfig.peakRate);
        }
    }

    if (!hal->isDpcdOffline() && !this->linkUseMultistream() && this->policyAssessLinkSafely)
    {
        GroupImpl * groupAttached = this->getActiveGroupForSST();
        if (groupAttached && groupAttached->isHeadAttached() &&
            !willLinkSupportModeSST(lConfig, groupAttached->lastModesetInfo))
        {
            DP_ASSERT(0 && "DP> Maximum assessed link configuration is not capable driving existing raster!");

            bIsFlushModeEnabled = enableFlush();
            if (bIsFlushModeEnabled)
            {
                train(preFlushModeActiveLinkConfig, true);
                disableFlush();
            }
            linkGuessed = true;
            goto done;
        }
    }

    highestAssessedLC = lConfig;

    // It is critical that this restore the original (desired) configuration
    trainLinkOptimized(lConfig);

    linkGuessed = false;

done:

    NV_DPTRACE_INFO(LINK_ASSESSMENT, highestAssessedLC.peakRate, highestAssessedLC.lanes);

    if (bLinkStateToggle)
    {
        DP_PRINTF(DP_NOTICE, "DP> Link state toggled, reading DSC caps now");
        // Read panel DSC support only if GPU supports DSC
        bool bGpuDscSupported;
        main->getDscCaps(&bGpuDscSupported);
        if (bGpuDscSupported)
        {
            for (Device * i = enumDevices(0); i; i=enumDevices(i))
            {
                DeviceImpl * dev = (DeviceImpl *)i;
                if(dev->getDSCSupport())
                {
                    // Read and parse DSC caps only if panel and GPU supports DSC
                    dev->readAndParseDSCCaps();
                }
                if (!(dev->processedEdid.WARFlags.bIgnoreDscCap))
                {
                    dev->setDscDecompressionDevice(this->bDscCapBasedOnParent);
                }
            }
        }
    }


    //
    // Now that we know the max link rate and lane count possible, try to acquire the full BW.
    // Ideally clients are expected to call into DpLib to update the BW requirements
    // This however is a failsafe to ensure that at least some BW is allocated on the DP Tunnel IN
    //
    allocateMaxDpTunnelBw();
}

bool ConnectorImpl::handleCPIRQ()
{
    NvU8        bStatus;
    HDCPState   hdcpState = {0};

    if (!isLinkActive())
    {
        DP_PRINTF(DP_WARNING, "DP> CP_IRQ: Ignored with link down");
        return true;
    }

    main->configureHDCPGetHDCPState(hdcpState);
    if (hal->getRxStatus(hdcpState, &bStatus))
    {
        NvBool bReAuthReq = NV_FALSE;
        NvBool bRxIDMsgPending = NV_FALSE;
        DP_PRINTF(DP_NOTICE, "DP> CP_IRQ HDCP ver:%s RxStatus:0x%2x HDCP Authenticated:%s Encryption:%s",
                  hdcpState.HDCP_State_22_Capable ? "2.2" : "1.x",
                  bStatus,
                  hdcpState.HDCP_State_Authenticated ? "YES" : "NO",
                  hdcpState.HDCP_State_Encryption ? "ON" : "OFF");

        // Check device if HDCP2.2 capable instead actual encryption status,
        if (hdcpState.HDCP_State_22_Capable)
        {
            if (FLD_TEST_DRF(_DPCD, _HDCP22_RX_STATUS, _REAUTH_REQUEST, _YES, bStatus) ||
                FLD_TEST_DRF(_DPCD, _HDCP22_RX_STATUS, _LINK_INTEGRITY_FAILURE, _YES, bStatus))
            {
                if (this->linkUseMultistream())
                {
                    //
                    // Bug 2860192: Some MST hub throw integrity failure before source trigger
                    // authentication. This may be stale data since Branch is
                    // doing protocol translation(DP to HDMI), and cannot treat
                    // as sink's fault.
                    // For MST, we would not lose anything here by ignoring either
                    // CP_Irq event since Auth never started after HPD high or
                    // LinkTraining start.
                    //
                    if (isHDCPAuthTriggered)
                    {
                        bReAuthReq = NV_TRUE;
                    }
                    else
                    {
                        DP_PRINTF(DP_NOTICE, "DP>Ignore integrity failure or ReAuth in transition or before AKE_INIT.");
                    }
                }
                else
                {
                    bReAuthReq = NV_TRUE;
                }
            }

            if (FLD_TEST_DRF(_DPCD, _HDCP22_RX_STATUS, _READY, _YES, bStatus))
            {
                bRxIDMsgPending = NV_TRUE;
            }
        }
        else
        {
            if (FLD_TEST_DRF(_DPCD, _HDCP_BSTATUS, _REAUTHENTICATION_REQUESET, _TRUE, bStatus) ||
                FLD_TEST_DRF(_DPCD, _HDCP_BSTATUS, _LINK_INTEGRITY_FAILURE, _TRUE, bStatus))
            {
                bReAuthReq = NV_TRUE;
            }
        }

        if (bReAuthReq || bRxIDMsgPending)
        {
            DP_PRINTF(DP_NOTICE, "DP> CP_IRQ: REAUTHENTICATION/RXIDPENDING REQUEST");

            if (bReAuthReq)
            {
                authRetries = 0;
            }

            if (!this->linkUseMultistream())
            {
                // Get primary connector when multi-stream SST deployed.
                GroupImpl *pGroupAttached = getActiveGroupForSST();
                ConnectorImpl *sstPrim = this;

                if (pGroupAttached &&
                    (pGroupAttached->singleHeadMultiStreamMode == DP_SINGLE_HEAD_MULTI_STREAM_MODE_SST) &&
                    (pGroupAttached->singleHeadMultiStreamID   == DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_SECONDARY))
                {
                    DP_ASSERT(this->pCoupledConnector);
                    sstPrim = this->pCoupledConnector;
                }

                sstPrim->main->configureHDCPRenegotiate(HDCP_DUMMY_CN,
                                                        HDCP_DUMMY_CKSV,
                                                        !!bReAuthReq,
                                                        !!bRxIDMsgPending);
                sstPrim->main->configureHDCPGetHDCPState(hdcpState);
                isHDCPAuthOn = hdcpState.HDCP_State_Authenticated;
            }
        }

        return true;
    }
    else
    {
        DP_PRINTF(DP_ERROR, "DP> CP_IRQ: RxStatus Read failed.");
        return false;
    }
}

void ConnectorImpl::handleSSC()
{
}

void ConnectorImpl::handleHdmiLinkStatusChanged()
{
    bool    bLinkActive;
    NvU32   newFrlRate;
    // Check Link status
    if (!hal->queryHdmiLinkStatus(&bLinkActive, NULL))
    {
        return;
    }
    if (!bLinkActive)
    {
        newFrlRate = hal->restorePCONFrlLink(activePConLinkControl.frlHdmiBwMask,
                                             activePConLinkControl.flags.bExtendedLTMode,
                                             activePConLinkControl.flags.bConcurrentMode);

        if (newFrlRate != activePConLinkControl.result.trainedFrlBwMask)
        {
            activePConLinkControl.result.trainedFrlBwMask = newFrlRate;
            activePConLinkControl.result.maxFrlBwTrained  = getMaxFrlBwFromMask(newFrlRate);
            for (Device *i = enumDevices(0); i; i = enumDevices(i))
            {
                DeviceImpl *dev = (DeviceImpl *)i;
                if ((dev->activeGroup != NULL) && (dev->plugged))
                {
                    sink->bandwidthChangeNotification(dev, false);
                }
            }
        }
    }
}

void ConnectorImpl::handleMCCSIRQ()
{
    for (Device *i = enumDevices(0); i; i = enumDevices(i))
    {
        DeviceImpl *dev = (DeviceImpl *)i;
        if ((dev->activeGroup != NULL) && (dev->plugged))
        {
            sink->notifyMCCSEvent(dev);
        }
    }
}

void ConnectorImpl::handlePanelReplayError()
{
    hal->readPanelReplayError();
}

//
// Checks if the link is still trained.
// Note that these hal registers are ONLY re-read in response to an IRQ.
// Calling this function returns the information from the last interrupt.
//
bool ConnectorImpl::isLinkLost()
{
    if (isLinkActive())
    {
        // Bug 200320196: Add DPCD offline check to avoid link-train in unplugged state.
        if (!hal->isDpcdOffline())
        {
            unsigned laneCount;
            NvU64 linkRate;
            getCurrentLinkConfig(laneCount, linkRate);
            //
            // Check SW lane count in RM in case it's disabled beyond DPLib.
            // Bug 1933751/2897747
            //
            if (laneCount == laneCount_0)
                return true;
        }

        // update the sw cache if required
        hal->refreshLinkStatus();
        if (!hal->getInterlaneAlignDone())
            return true;

        for (unsigned i = 0; i < activeLinkConfig.lanes; i++)
        {
            if (!hal->getLaneStatusSymbolLock(i))
                return true;
            if (!hal->getLaneStatusClockRecoveryDone(i))
                return true;
            if (!hal->getLaneStatusChannelEqualizationDone(i))
                return true;
        }

        if (!hal->getInterlaneAlignDone())
            return true;
    }
    return false;
}

bool ConnectorImpl::isLinkActive()
{
    return (activeLinkConfig.isValid());
}

bool ConnectorImpl::isLinkInD3()
{
    return (hal->getPowerState() == PowerStateD3);
}

bool ConnectorImpl::trainLinkOptimizedSingleHeadMultipleSST(GroupImpl *pGroupAttached)
{
    if (!pGroupAttached)
    {
        DP_PRINTF(DP_ERROR, "DP-CONN> 2-sst group not valid");
        return false;
    }

    if (preferredLinkConfig.isValid())
    {
        ConnectorImpl *pSecConImpl = this->pCoupledConnector;
        if (pSecConImpl->preferredLinkConfig.isValid() &&
            (preferredLinkConfig.lanes == laneCount_4) && (pSecConImpl->preferredLinkConfig.lanes == laneCount_4) &&
            (preferredLinkConfig.peakRate == pSecConImpl->preferredLinkConfig.peakRate))
        {
            if (willLinkSupportModeSST(preferredLinkConfig, pGroupAttached->lastModesetInfo))
            {
                if (pGroupAttached->singleHeadMultiStreamID == DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY)
                {
                    if (!this->enableFlush())
                        return false;
                }
                preferredLinkConfig.policy.setSkipFallBack(true);
                if (!train(preferredLinkConfig, false))
                {
                    DP_PRINTF(DP_ERROR, "DP-CONN> Unable to set preferred linkconfig on 2-SST display");
                    return false;
                }
                if (pGroupAttached->singleHeadMultiStreamID == DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_SECONDARY)
                {
                    this->disableFlush();
                }
                return true;
            }
            else
            {
                DP_PRINTF(DP_ERROR, "DP-CONN> Invalid 2-SST Preferred link configuration");
                return false;
            }
        }
    }

    if (pGroupAttached->singleHeadMultiStreamMode == DP_SINGLE_HEAD_MULTI_STREAM_MODE_SST)
    {
        if (pGroupAttached->singleHeadMultiStreamID == DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_SECONDARY)
        {
            if (this->pCoupledConnector->oneHeadSSTSecPrefLnkCfg.isValid())
            {
                bool trainDone = false;
                this->pCoupledConnector->oneHeadSSTSecPrefLnkCfg.policy.setSkipFallBack(true);
                if (!train(this->pCoupledConnector->oneHeadSSTSecPrefLnkCfg, false))
                {
                    DP_PRINTF(DP_ERROR, "DP-CONN> Unable set the primary configuration on secondary display");
                    trainDone = false;
                }
                else
                {
                    trainDone =  true;
                }
                this->disableFlush();
                return trainDone;
            }
        }

    }

    // Order for 2-SST link training and must be with 4 lanes
    unsigned linkRateList[] = {dp2LinkRate_1_62Gbps, dp2LinkRate_2_70Gbps, dp2LinkRate_5_40Gbps, dp2LinkRate_8_10Gbps};
    NvU8     linkRateCount = sizeof(linkRateList) / sizeof(unsigned);

    for (NvU8 i = 0; i < linkRateCount; i++)
    {
        LinkConfiguration linkCfg = LinkConfiguration(&this->linkPolicy,
                                                      laneCount_4, linkRateList[i],
                                                      hal->getEnhancedFraming(),
                                                      false, // MST
                                                      false, /* disablePostLTRequest */
                                                      false, /* bEnableFEC */
                                                      false, /* bDisableLTTPR */
                                                      this->getDownspreadDisabled());
        linkCfg.policy.setSkipFallBack(true);
        if (willLinkSupportModeSST(linkCfg, pGroupAttached->lastModesetInfo))
        {
            if (!this->enableFlush())
                return false;
            if (!train(linkCfg, false))
            {
                if (i == linkRateCount - 1)
                {
                    // Re-train max link config
                    linkCfg = getMaxLinkConfig();
                    linkCfg.policy.setSkipFallBack(true);
                    if (!train(linkCfg, false))
                    {
                        DP_ASSERT(0 && "DPCONN> 2-SST setting max link configuration failed ");
                        break;
                    }
                }
            }
            else
            {
                oneHeadSSTSecPrefLnkCfg = linkCfg;
                break;
            }
        }
    }

    return true;
}

bool ConnectorImpl::isNoActiveStreamAndPowerdown()
{
    if (activeGroups.isEmpty())
    {
        bool bKeepMSTLinkAlive = (this->bKeepLinkAliveMST && activeLinkConfig.multistream);
        bool bKeepSSTLinkAlive = (this->bKeepLinkAliveSST && !activeLinkConfig.multistream);
        //
        // Power saving unless:
        // - Setting fake flag as true to prevent panel power down here.
        // - Regkey sets to keep link alive for MST and it's in MST.
        // - Regkey sets to keep link alive for SST and it's in SST.
        // - bKeepOptLinkAlive is set to true - to avoid link retraining.
        // - Device discovery processing that processNewDevice has HDCP probe.
        // - Pending remote HDCP detection messages - prevent power down to access HDCP DCPD regs.
        // - Keep link active with compliance device as we always do
        //
        if ((!bKeepMSTLinkAlive) &&
            (!bKeepSSTLinkAlive) &&
            (!bKeepOptLinkAlive) &&
            (!bKeepLinkAliveForPCON) &&
            (!bIsDiscoveryDetectActive) &&
            (pendingRemoteHdcpDetections == 0) &&
            (!main->isInternalPanelDynamicMuxCapable())
           )
        {
            powerdownLink();

            // Sharp panel for HP Valor QHD+ needs 50 ms after D3
            if (bDelayAfterD3)
            {
                timer->sleep(50);
            }
        }

        return true;
    }

    return false;
}

bool ConnectorImpl::trainLinkOptimized(LinkConfiguration lConfig)
{
    LinkConfiguration lowestSelected;         // initializes to 0
    bool bSkipLowestConfigCheck  = false;     // Force highestLink config in SST
    bool bSkipRedundantLt        = false;     // Skip redundant LT
    bool bEnteredFlushMode       = false;
    bool bLinkTrainingSuccessful = true;      // status indicating if link training actually succeeded
                                              // forced link training is considered a failure
    bool bTwoHeadOneOrLinkRetrain = false;    // force link re-train if any attached
                                              // groups are in 2Head1OR mode.

    if (isNoActiveStreamAndPowerdown())
    {
        DP_PRINTF(DP_INFO, "Power off the link because no stream are active");
        return true;
    }

    //
    //   Split policy.
    //    If we're multistream we *always pick the highest link configuration available
    //           - we don't want to interrupt existing panels to light up new ones
    //    If we're singlestream we always pick the lowest power configurations
    //           - there can't be multiple streams, so the previous limitation doesn't apply
    //

    //
    // Find the active group(s)
    //
    GroupImpl * groupAttached = 0;
    for (ListElement * e = activeGroups.begin(); e != activeGroups.end(); e = e->next)
    {
        DP_ASSERT(bIsUefiSystem || linkUseMultistream() || (!groupAttached && "Multiple attached heads"));
        groupAttached = (GroupImpl * )e;

        if ((groupAttached->dscModeRequest == DSC_DUAL) && (groupAttached->dscModeActive != DSC_DUAL))
        {
            //
            // If current modeset group requires 2Head1OR and
            //  - group is not active yet (first modeset on the group)
            //  - group is active but not in 2Head1OR mode (last modeset on the group did not require 2Head1OR)
            // then re-train the link
            // This is because for 2Head1OR mode, we need to set some LT parametes for slave SOR after
            // successful LT on primary SOR without which 2Head1OR modeset will lead to HW hang.
            //
            bTwoHeadOneOrLinkRetrain = true;
            break;
        }
    }

    lowestSelected = getMaxLinkConfig();

    if (!activeLinkConfig.multistream)
    {
        if (groupAttached &&
            groupAttached->singleHeadMultiStreamMode == DP_SINGLE_HEAD_MULTI_STREAM_MODE_SST)
        {
            return trainLinkOptimizedSingleHeadMultipleSST(groupAttached);
        }

        if (preferredLinkConfig.isValid())
        {
            if (activeLinkConfig != preferredLinkConfig)
            {
                // if a tool has requested a preferred link config; check if its possible; and train to it.
                // else choose the normal path
                if (groupAttached &&
                    willLinkSupportModeSST(preferredLinkConfig, groupAttached->lastModesetInfo))
                {
                    if (!this->enableFlush())
                        return false;
                    if (!train(preferredLinkConfig, false))
                    {
                        DP_PRINTF(DP_ERROR, "DP-CONN> Preferred linkconfig could not be applied. Forcing on gpu side.");
                        train(preferredLinkConfig, true);
                    }
                    this->disableFlush();
                    return true;
                }
                else
                {
                    DP_PRINTF(DP_ERROR, "DP-CONN> Preferred linkconfig does not support the mode");
                    return false;
                }
            }
            else
            {
                // We are already at preferred. Nothing to do here. Return.
                return true;
            }
        }

        //
        // This is required for making certain panels to work by training them in
        // highest linkConfig in SST mode.
        //
        for (Device * i = enumDevices(0); i; i=enumDevices(i))
        {
            DeviceImpl * dev = (DeviceImpl *)i;
            if (dev->forceMaxLinkConfig())
            {
                bSkipLowestConfigCheck = true;
            }
            if (dev->skipRedundantLt())
            {
                bSkipRedundantLt = true;
            }
        }

        if (bPConConnected)
        {
            // When PCON is connected, always LT to max to avoid LT.
            bSkipLowestConfigCheck = true;
        }

        // If the flag is set, simply neglect downgrading to lowest possible linkConfig
        if (!bSkipLowestConfigCheck)
        {
            lConfig = lowestSelected;

            if (groupAttached)
            {
                lConfig.enableFEC(this->bFECEnable);
                // Find lowest link configuration supporting the mode
                getValidLowestLinkConfig(lConfig, lowestSelected, groupAttached->lastModesetInfo);
            }
        }

        if (lowestSelected.isValid())
        {
            //
            // Check if we are already trained to the desired link config?
            // Make sure requested FEC state matches with the current FEC state of link.
            // If 2Head1OR mode is requested, retrain if group is not active or
            // last modeset on active group was not in 2Head1OR mode.
            // bTwoHeadOneOrLinkRetrain tracks this requirement.
            //

            //
            // Set linkStatus to be dirty so that when isLinkLost() calls
            // refreshLinkStatus() it will get real time status. This is to
            // fix an issue that when UEFI-to-Driver transition, LTTPR is not
            // link trainined but will be link trainined by RM.
            //
            hal->setDirtyLinkStatus(true);
            if ((activeLinkConfig == lowestSelected) &&
                (!isLinkInD3()) &&
                (!isLinkLost()) &&
                (this->bFECEnable == activeLinkConfig.bEnableFEC) &&
                !bTwoHeadOneOrLinkRetrain)
            {
                if (bSkipRedundantLt || main->isInternalPanelDynamicMuxCapable())
                {
                    // Skip LT if the links are already trained to desired config.
                    DP_PRINTF(DP_NOTICE, "DP-CONN> Skipping redundant LT.");
                    return true;
                }
                else
                {
                    // Make sure link status is still good.
                    if (activeLinkConfig.lanes && hal->isLinkStatusValid(activeLinkConfig.lanes))
                    {
                        // Pass on a flag to RM ctrl call to skip LT at RM level.
                        DP_PRINTF(DP_NOTICE, "DP-CONN> Skipping redundant LT from RM.");
                        bSkipLt = true;
                    }
                }
            }
            else
            {
                bSkipLt = false;
            }

            // Enter flush mode/detach head before LT
            if (!bSkipLt)
            {
                if (!(bEnteredFlushMode = this->enableFlush()))
                    return false;
            }

            bLinkTrainingSuccessful = train(lowestSelected, false);
            //
            // If LT failed, check if skipLT was marked. If so, clear the flag and
            // enable flush mode if required (headattached) and try real LT once.
            //
            if (!bLinkTrainingSuccessful && bSkipLt)
            {
                bSkipLt = false;
                if (!(bEnteredFlushMode = this->enableFlush()))
                    return false;
                bLinkTrainingSuccessful = train(lowestSelected, false);
            }
            if (!bLinkTrainingSuccessful)
            {
                LinkConfiguration maxLinkConfig = getMaxLinkConfig();
                //
                // If optimized link config fails, try max link config with fallback.
                // Note: It's possible some link rates are dynamically invalidated
                //       during failed link training. That means we can't assume
                //       maxLinkConfig is always greater than the lowestSelected
                //       link configuration.
                //
                train(maxLinkConfig, false);

                //
                // Note here that fallback might happen while attempting LT to max link config.
                // activeLinkConfig will be set to that passing config.
                //
                if (!willLinkSupportModeSST(activeLinkConfig, groupAttached->lastModesetInfo))
                {
                    //
                    // If none of the link configs pass LT or a fall back link config passed LT
                    // but cannot support the mode, then we will force the optimized link config
                    // on the link and mark LT as fail.
                    //

                    // Force LT really should not fail!
                    DP_ASSERT(train(lowestSelected, true));
                    bLinkTrainingSuccessful = false;
                }
                else
                {
                    //
                    // If a fallback link config pass LT and can support
                    // the mode, mark LT as pass.
                    //
                    bLinkTrainingSuccessful = true;
                }
            }
        }
        else
        {
            if (groupAttached && groupAttached->isHeadAttached())
            {
                if (!(bEnteredFlushMode = this->enableFlush()))
                    return false;
            }
            LinkConfiguration maxLinkConfig = getMaxLinkConfig();
            // Mode wasn't possible at any assessed configuration.
            train(maxLinkConfig, true);

            // Mark link training as failed since we forced it
            bLinkTrainingSuccessful = false;
        }

        lConfig = activeLinkConfig;

        if (bEnteredFlushMode)
        {
            this->disableFlush();
        }

        // In case this was set, we should reset it to prevent skipping LT next time.
        bSkipLt = false;
    }
    else
    {
        bool bRetrainToEnsureLinkStatus;

        //
        //     Multistream:
        //          If we can't restore all streams after a link train - we need to make sure that
        //          we set RG_DIV to "slow down" the effective pclk for that head.  RG_DIV does give
        //          us enough room to account for both the HBR2->RBR drop and the 4->1 drop.
        //          This should allow us to keep the link up and operating at a sane frequency.
        //          .. thus we'll allow training at any frequency ..
        //

        // for MST; the setPreferred calls assessLink directly.
        if (preferredLinkConfig.isValid() && (activeLinkConfig != preferredLinkConfig))
        {
            if (!train(preferredLinkConfig, false))
            {
                DP_PRINTF(DP_ERROR, "DP-CONN> Preferred linkconfig could not be applied. Forcing on gpu side.");
                train(preferredLinkConfig, true);
            }
            return true;
        }

        //
        // Make sure link is physically active and healthy, otherwise re-train.
        // Make sure requested FEC state matches with the current FEC state of link.
        // If 2Head1OR mode is requested, retrain if group is not active or last modeset on active group
        // was not in 2Head1OR mode. bTwoHeadOneOrLinkRetrain tracks this requirement.
        //
        bRetrainToEnsureLinkStatus = (isLinkActive() && isLinkInD3()) ||
                                     isLinkLost() ||
                                     (activeLinkConfig.bEnableFEC != this->bFECEnable) ||
                                     bTwoHeadOneOrLinkRetrain;

        if (bRetrainToEnsureLinkStatus || (!isLinkActive()))
        {
            //
            // Train to the highestAssesed link config for MST cases to avoid redundant
            // fallback. There is no point of trying to link train at highest link config
            // when it failed during the assessment.
            // train() handles fallback now. So we don't need to step down when LT fails.
            //
            LinkConfiguration desired = highestAssessedLC;

            NvU8 retries = DP_LT_MAX_FOR_MST_MAX_RETRIES;

            desired.enableFEC(this->bFECEnable);

            if (bRetrainToEnsureLinkStatus)
            {
                bEnteredFlushMode = enableFlush();
            }

            //
            // In some cases, the FEC isn't enabled and link is not lost (e.g. DP_KEEP_OPT_LINK_ALIVE = 1),
            // but we're going to enable DSC. We need to update bSkipLt for retraining the link with FEC.
            // As the bSkipLt was set to true prviously while link is not lost.
            //
            if (activeLinkConfig.bEnableFEC != this->bFECEnable)
            {
                bSkipLt = false;
            }

            train(desired, false);
            if (!activeLinkConfig.isValid())
            {
                LinkConfiguration maxLinkConfig = getMaxLinkConfig();
                DP_PRINTF(DP_ERROR, "DPCONN> Unable to train link (at all).  Forcing training (picture won't show up)");
                train(maxLinkConfig, true);

                // Mark link training as failed since we forced it
                bLinkTrainingSuccessful = false;
            }

            //
            // Bug 2354318: On some MST branches, we might see a problem that LT failed during
            // assessLink(), but somehow works later. In this case, we should not
            // retry since highestAssessedLC is not a valid comparison now.
            //
            if (highestAssessedLC.isValid())
            {
                while ((highestAssessedLC != activeLinkConfig) && retries > 0)
                {
                    // Give it a few more chances.
                    train(desired, false);
                    retries--;
                };
            }

            lConfig = activeLinkConfig;

            if (bEnteredFlushMode)
            {
                disableFlush();
            }
        }
    }

    return (bLinkTrainingSuccessful && lConfig.isValid());
}

bool ConnectorImpl::getValidLowestLinkConfig
(
    LinkConfiguration      &lConfig,
    LinkConfiguration      &lowestSelected,
    ModesetInfo             modesetInfo,
    const DscParams        *pDscParams
)
{
    bool bIsModeSupported = false;
    unsigned i;
    LinkConfiguration selectedConfig;

    for (i = 0; i < numPossibleLnkCfg; i++)
    {
        if ((this->allPossibleLinkCfgs[i].lanes > lConfig.lanes) ||
            (this->allPossibleLinkCfgs[i].peakRate > lConfig.peakRate))
        {
            continue;
        }

        // Update enhancedFraming for target config
        this->allPossibleLinkCfgs[i].enhancedFraming = lConfig.enhancedFraming;

        selectedConfig = this->allPossibleLinkCfgs[i];

        selectedConfig.enableFEC(lConfig.bEnableFEC);
        if (lConfig.bIs128b132bChannelCoding &&
            !(selectedConfig.bIs128b132bChannelCoding) &&
            !modesetInfo.bEnableDsc)
        {
            //
            // If highest link rate is UHBR 128b132b and current selected config is 8b10b,
            // FEC should not be enabled if DSC is not enabled since
            // 1. This function will be called only for SST and that too when preferred
            //  link config is not set.
            // 2. for SST and in 8b10b mode, FEC is enabled only when DSC is enabled
            //
            selectedConfig.enableFEC(false);
        }
        if (willLinkSupportModeSST(selectedConfig, modesetInfo))
        {
            bIsModeSupported = true;
            break;
        }
    }

    if (bIsModeSupported)
    {
        lowestSelected = selectedConfig;
        lowestSelected.bDisableDownspread = lConfig.bDisableDownspread;
    }
    else
    {
        // Invalidate link config if mode is not possible at all
        lowestSelected.lanes = 0;
    }

    return bIsModeSupported;
}

bool ConnectorImpl::postLTAdjustment(const LinkConfiguration & lConfig, bool force)
{
    NvU8 lastVoltageSwingLane[DP_MAX_LANES]     = {0};
    NvU8 lastPreemphasisLane[DP_MAX_LANES]      = {0};
    NvU8 lastTrainingScoreLane[DP_MAX_LANES]    = {0};
    NvU8 lastPostCursor[DP_MAX_LANES]           = {0};
    NvU8 currVoltageSwingLane[DP_MAX_LANES]     = {0};
    NvU8 currPreemphasisLane[DP_MAX_LANES]      = {0};
    NvU8 currTrainingScoreLane[DP_MAX_LANES]    = {0};
    NvU8 currPostCursor[DP_MAX_LANES]           = {0};
    NvU32 updatedLaneSettings[DP_MAX_LANES]     = {0};
    NvU8 adjReqCount = 0;
    NvU64 startTime;
    LinkConfiguration linkConfig = lConfig;

    // Cache Voltage Swing and Preemphasis value just after Link training
    if (!hal->readTraining(lastVoltageSwingLane,
                           lastPreemphasisLane,
                           lastTrainingScoreLane,
                           lastPostCursor,
                           (NvU8)activeLinkConfig.lanes))
    {
        DP_PRINTF(DP_ERROR, "DPCONN> Post Link Training : Unable to read current training values");
    }

    if (hal->getTrainingPatternSelect() != TRAINING_DISABLED)
    {
        DP_PRINTF(DP_WARNING, "DPCONN> Post Link Training : Training pattern is not disabled.");
    }

    //
    // We have cleared DPCD 102h
    // Now hardware will automatically send the idle pattern
    //
    startTime = timer->getTimeUs();

    do
    {
        if (!hal->getIsPostLtAdjRequestInProgress())
        {
            // Clear POST_LT_ADJ_REQ_GRANTED bit and start normal AV transmission
            hal->setPostLtAdjustRequestGranted(false);
            return true;
        }

        // Wait for 2ms
        Timeout timeout(timer, 2);

        // check if DPCD 00206h~00207h change has reached to ADJ_REQ_LIMIT
        if (adjReqCount > DP_POST_LT_ADJ_REQ_LIMIT)
        {
            // Clear POST_LT_ADJ_REQ_GRANTED bit and start normal AV transmission
            hal->setPostLtAdjustRequestGranted(false);
            return true;
        }

        if (!hal->readTraining(currVoltageSwingLane,
                               currPreemphasisLane,
                               currTrainingScoreLane,
                               currPostCursor,
                               (NvU8)activeLinkConfig.lanes))
        {
            DP_PRINTF(DP_ERROR, "DPCONN> Post Link Training : Unable to read current training values");
        }
        else
        {
            if (!hal->isLaneSettingsChanged(lastVoltageSwingLane,
                                            currVoltageSwingLane,
                                            lastPreemphasisLane,
                                            currPreemphasisLane,
                                            (NvU8)activeLinkConfig.lanes))
            {
                // Check if we have exceeded DP_POST_LT_ADJ_REQ_TIMER (200 ms)
                if ((timer->getTimeUs() - startTime) > DP_POST_LT_ADJ_REQ_TIMER)
                {
                    DP_PRINTF(DP_ERROR, "DPCONN> Post Link Training : DP_POST_LT_ADJ_REQ_TIMER is timed out.");
                    // Clear POST_LT_ADJ_REQ_GRANTED bit and start normal AV transmission
                    hal->setPostLtAdjustRequestGranted(false);
                    return true;
                }
            }
            else
            {
                adjReqCount++;

                // Clear ADJ_REQ_TIMER
                startTime = timer->getTimeUs();

                // Change RX drive settings according to DPCD 00206h & 00207h
                if (!hal->setTrainingMultiLaneSet((NvU8)activeLinkConfig.lanes,
                                                  currVoltageSwingLane,
                                                  currPreemphasisLane))
                {
                    DP_PRINTF(DP_ERROR, "DPCONN> Post Link Training : Failed to set RX drive setting according to DPCD 00206h & 00207h.");
                }

                // Populate updated lane settings for currently active lanes
                populateUpdatedLaneSettings(currVoltageSwingLane, currPreemphasisLane, updatedLaneSettings);

                // Change TX drive settings according to DPCD 00206h & 00207h
                if (!setLaneConfig(activeLinkConfig.lanes, updatedLaneSettings))
                {
                    DP_PRINTF(DP_ERROR, "DPCONN> Post Link Training : Failed to set TX drive setting according to DPCD 00206h & 00207h.");
                }

                // Update last Voltage Swing and Preemphasis values
                if (!hal->readTraining(lastVoltageSwingLane,
                                       lastPreemphasisLane,
                                       lastTrainingScoreLane,
                                       lastPostCursor,
                                       (NvU8)activeLinkConfig.lanes))
                {
                    DP_PRINTF(DP_ERROR, "DPCONN> Post Link Training : Unable to read current training values");
                }
            }
        }

        // Mark the linkStatus as dirty since we need to retrain in case Rx has lost sync
         hal->setDirtyLinkStatus(true);
    }while (!isLinkLost());

    // Clear POST_LT_ADJ_REQ_GRANTED bit
    hal->setPostLtAdjustRequestGranted(false);

    if (isLinkLost())
    {
        if (bNoFallbackInPostLQA && (retryLT < WAR_MAX_RETRAIN_ATTEMPT))
        {
            //
            // A monitor may lose link sometimes during assess link or link training.
            // So retry for 3 times before fallback to lower config
            //
            retryLT++;
            train(lConfig, force);
            return true;
        }
        //
        // If the link is not alive, then we need to retrain at a lower config
        // There is no reason to try at the same link configuration. Follow the
        // fallback policy that is followed for CR phase of LT
        //
        if (!linkConfig.lowerConfig())
        {
            DP_PRINTF(DP_ERROR, "DPCONN> Post Link Training : Already at the lowest link rate. Cannot reduce further");
            return false;
        }
        train(linkConfig, force);
    }
    else if (bNoFallbackInPostLQA && (retryLT != 0))
    {
        retryLT = 0;
    }

    return true;
}

void ConnectorImpl::populateUpdatedLaneSettings(NvU8* voltageSwingLane, NvU8* preemphasisLane, NvU32 *data)
{
    NvU32 laneIndex;

    for (laneIndex = 0; laneIndex < activeLinkConfig.lanes; laneIndex++)
    {
        switch (voltageSwingLane[laneIndex])
        {
            case driveCurrent_Level0:
                data[laneIndex] = FLD_SET_DRF(0073_CTRL, _DP_LANE_DATA, _DRIVECURRENT, _LEVEL0, data[laneIndex]);
                break;

            case driveCurrent_Level1:
                data[laneIndex] = FLD_SET_DRF(0073_CTRL, _DP_LANE_DATA, _DRIVECURRENT, _LEVEL1, data[laneIndex]);
                break;

            case driveCurrent_Level2:
                data[laneIndex] = FLD_SET_DRF(0073_CTRL, _DP_LANE_DATA, _DRIVECURRENT, _LEVEL2, data[laneIndex]);
                break;

            case driveCurrent_Level3:
                data[laneIndex] = FLD_SET_DRF(0073_CTRL, _DP_LANE_DATA, _DRIVECURRENT, _LEVEL3, data[laneIndex]);
                break;
        }

        switch (preemphasisLane[laneIndex])
        {
            case preEmphasis_Level1:
                data[laneIndex] = FLD_SET_DRF(0073_CTRL, _DP_LANE_DATA, _PREEMPHASIS, _LEVEL1, data[laneIndex]);
                break;

            case preEmphasis_Level2:
                data[laneIndex] = FLD_SET_DRF(0073_CTRL, _DP_LANE_DATA, _PREEMPHASIS, _LEVEL2, data[laneIndex]);
                break;

            case preEmphasis_Level3:
                data[laneIndex] = FLD_SET_DRF(0073_CTRL, _DP_LANE_DATA, _PREEMPHASIS, _LEVEL3, data[laneIndex]);
                break;
        }
    }
}

bool ConnectorImpl::validateLinkConfiguration(const LinkConfiguration & lConfig)
{
    NvU64 linkRate10M = lConfig.peakRate;

    if (!IS_VALID_LANECOUNT(lConfig.lanes))
    {
        DP_PRINTF(DP_ERROR, "DPCONN> Invalid Lane Count=%d", lConfig.lanes);
        return false;
    }

    if (lConfig.lanes > hal->getMaxLaneCount())
    {
        DP_PRINTF(DP_ERROR, "DPCONN> Requested Lane Count=%d is larger than sinkMaxLaneCount=%d",
                  lConfig.lanes, hal->getMaxLaneCount());
        return false;
    }

    if (lConfig.lanes != 0)
    {
        if (!IS_VALID_LINKBW_10M(linkRate10M))
        {
            DP_PRINTF(DP_ERROR, "DPCONN> Requested link rate=%d is not valid", linkRate10M);
            return false;
        }

        if (linkRate10M > hal->getMaxLinkRate())
        {
            DP_PRINTF(DP_ERROR, "DPCONN> Requested link rate=%d is larger than sinkMaxLinkRate=%d",
                      linkRate10M, hal->getMaxLinkRate());
            return false;
        }

        if (IS_INTERMEDIATE_LINKBW_10M(linkRate10M))
        {
            NvU16 *ilrTable;
            NvU32 i;
            if (!hal->isIndexedLinkrateEnabled())
            {
                DP_PRINTF(DP_ERROR, "DPCONN> Indexed Link Rate=%d is Not Enabled in Sink", linkRate10M);
                return false;
            }

            ilrTable = hal->getLinkRateTable();
            for (i = 0; i < NV0073_CTRL_DP_MAX_INDEXED_LINK_RATES; i++)
            {
                //
                // linkRate10M is in 10M convention and ilrTable entries are the values read from DPCD in 200Kunits
                // Convert the ilrTable value to 10M convention before the comparison
                //
                if (LINK_RATE_200KHZ_TO_10MHZ(ilrTable[i]) == linkRate10M)
                    break;
                if (ilrTable[i] == 0)
                {
                    DP_PRINTF(DP_ERROR, "DPCONN> Indexed Link Rate=%d is Not Found", linkRate10M);
                    return false;
                }
            }

            if (i == NV0073_CTRL_DP_MAX_INDEXED_LINK_RATES)
            {
                DP_PRINTF(DP_ERROR, "DPCONN> Indexed Link Rate=%d is Not Found", linkRate10M);
                return false;
            }
        }
    }
    return true;
}

bool ConnectorImpl::train(const LinkConfiguration & lConfig, bool force,
                          LinkTrainingType trainType)
{
    LinkTrainingType preferredTrainingType  = trainType;
    bool             result                 = true;
    NvBool           bSkipSettingStreamMode = false;

    //  Validate link config against caps
    if (!force && !validateLinkConfiguration(lConfig))
    {
        return false;
    }

    if (!lConfig.multistream)
    {
        for (Device * i = enumDevices(0); i; i=enumDevices(i))
        {
            DeviceImpl * dev = (DeviceImpl *)i;
            if (dev->powerOnMonitorBeforeLt() && lConfig.lanes != 0)
            {
                //
                // Some panels expose that they are in D0 even when they are not.
                // Explicit write to DPCD 0x600 is required to wake up such panel before LT.
                //
                hal->setPowerState(PowerStateD0);
            }
        }
        //
        // Enable special LT only when regkey 'ENABLE_FAST_LINK_TRAINING' set
        // to 1 in DD's path.
        //
        if (bEnableFastLT)
        {
            // If the panel can support NLT or FLT, then let's try it first
            if (hal->getNoLinkTraining())
                preferredTrainingType = NO_LINK_TRAINING;
            else if (hal->getSupportsNoHandshakeTraining())
                preferredTrainingType = FAST_LINK_TRAINING;
        }
    }

    //
    //    Don't set the stream if we're:
    //    - forcing the config
    //    - Skipping LT and the flag to skip stream mode setting is true
    //    - shutting off the link
    //
    bSkipSettingStreamMode = force ||
                             (bSkipLt && this->bSkipResetMSTMBeforeLt) ||
                             (lConfig.lanes == 0);

    if (!bSkipSettingStreamMode)
    {
        if (isLinkActive())
        {
            if (activeLinkConfig.multistream != lConfig.multistream)
            {
                activeLinkConfig.lanes = 0;
                rawTrain(activeLinkConfig, true, NORMAL_LINK_TRAINING);
            }
        }

        if (AuxRetry::ack != hal->setMultistreamLink(lConfig.multistream))
        {
            DP_PRINTF(DP_WARNING, "DP> Failed to enable multistream mode on current link");
        }
    }

    //
    // Read link rate table before link-train to assure on-board re-driver
    // knows link rate going to be set in link rate table.
    // If eDP's power has been shutdown here, don't query Link rate table,
    // else it will cause panel wake up.
    //
    if (hal->isIndexedLinkrateEnabled() && (lConfig.lanes != 0))
    {
        hal->getRawLinkRateTable();
    }

    activeLinkConfig = lConfig;
    result = rawTrain(lConfig, force, preferredTrainingType);

    // If NLT or FLT failed, then fallback to normal LT again
    if (!result && (preferredTrainingType != NORMAL_LINK_TRAINING))
        result = rawTrain(lConfig, force, NORMAL_LINK_TRAINING);

    if (!result)
        activeLinkConfig.lanes = 0;
    else
    {
        if (activeLinkConfig.multistream)
        {
            // Total slot is 64, reserve slot 0 for header
            maximumSlots = 63;
            freeSlots = maximumSlots;
            firstFreeSlot = 1;
        }
        bNoLtDoneAfterHeadDetach = false;
    }

    if (!force && result)
        this->hal->setDirtyLinkStatus(true);

    // We don't need post LQA while powering down the lanes.
    if ((lConfig.lanes != 0) && hal->isPostLtAdjustRequestSupported() && result)
    {
        result = postLTAdjustment(activeLinkConfig, force);
    }

    if((lConfig.lanes != 0) && result && activeLinkConfig.bEnableFEC
        && !activeLinkConfig.bIs128b132bChannelCoding
    )
    {
        //
        // Extended latency from link-train end to FEC enable pattern
        // to avoid link lost or blank screen with Synaptics branch.
        // (Bug 2561206)
        //
        if (LT2FecLatencyMs)
        {
            timer->sleep(LT2FecLatencyMs);
        }

        result = main->configureFec(true /*bEnableFec*/);
        DP_ASSERT(result);
    }

    //
    // Do not compare bEnableFEC here. In DDS case FEC might be requested but
    // not performed in RM.
    //
    if ((lConfig.lanes != activeLinkConfig.lanes) ||
        (lConfig.peakRate != activeLinkConfig.peakRate) ||
        (lConfig.enhancedFraming != activeLinkConfig.enhancedFraming) ||
        (lConfig.multistream != activeLinkConfig.multistream))
    {
        // fallback happens, returns fail to make sure clients notice it.
        result = false;
    }

    if (result)
    {
        // update PSR link cache on successful LT
        this->psrLinkConfig = activeLinkConfig;
    }

    return result;
}

void ConnectorImpl::sortActiveGroups(bool ascending)
{
    List activeSortedGroups;

    while(!activeGroups.isEmpty())
    {
        ListElement * e = activeGroups.begin();
        GroupImpl * g = (GroupImpl *)e;

        GroupImpl * groupToInsertBefore = NULL;

        // Remove from active group for sorting
        activeGroups.remove(g);

        for (ListElement *e1 = activeSortedGroups.begin(); e1 != activeSortedGroups.end(); e1 = e1->next)
        {
            GroupImpl * g1 = (GroupImpl *)e1;
            if ((g->headIndex < g1->headIndex) ||
                ((g->headIndex == g1->headIndex) &&
                 ((ascending && (g->singleHeadMultiStreamID < g1->singleHeadMultiStreamID)) ||
                 (!ascending && (g->singleHeadMultiStreamID > g1->singleHeadMultiStreamID)))
                ))
            {
                groupToInsertBefore = g1;
                break;
            }
        }

        if (NULL == groupToInsertBefore)
        {
            activeSortedGroups.insertBack(g);
        }
        else
        {
            activeSortedGroups.insertBefore(groupToInsertBefore, g);
        }
    }

    // Repopulate active group list
    while (!activeSortedGroups.isEmpty())
    {
        ListElement * e = activeSortedGroups.begin();

        // Remove from sorted list
        activeSortedGroups.remove(e);
        // Insert back to active group list
        activeGroups.insertBack(e);
    }
}

bool ConnectorImpl::enableFlush()
{
    bool bHeadAttached = false;

    if (activeGroups.isEmpty())
        return true;

    //
    // If SST check that head should be attached with single group else if MST at least
    // 1 group should have headAttached before calling flush on SOR
    //
    if (!this->linkUseMultistream())
    {
        GroupImpl * activeGroup = this->getActiveGroupForSST();

        if (activeGroup && !activeGroup->isHeadAttached() && intransitionGroups.isEmpty())
        {
            DP_PRINTF(DP_ERROR, "DPCONN> SST-Flush mode should not be called when head is not attached. Returning early without enabling flush");
            return true;
        }
    }
    else
    {
        for (ListElement * e = activeGroups.begin(); e != activeGroups.end(); e = e->next)
        {
            GroupImpl * group = (GroupImpl *)e;
            if (group->isHeadAttached())
            {
                bHeadAttached  = true;
                break;
            }
        }

        if (!bHeadAttached)
        {
            DP_PRINTF(DP_ERROR, "DPCONN> MST-Flush mode should not be called when head is not attached. Returning early without enabling flush");
            return true;
        }
    }

    if (!main->setFlushMode())
        return false;

    //
    // Enabling flush mode shuts down the link:
    // 1. reset activeLinkConfig to indicate the link is now lost.
    // 2. The next link training call must not skip programming the hardware.
    //    Otherwise, EVO will hang if the head is still active when flush mode is disabled.
    //
    activeLinkConfig = LinkConfiguration();
    bSkipLt = false;

    sortActiveGroups(false);

    for (ListElement * e = activeGroups.begin(); e != activeGroups.end(); e = e->next)
    {
        GroupImpl * g = (GroupImpl *)e;

        if (!this->linkUseMultistream())
        {
            GroupImpl * activeGroup = this->getActiveGroupForSST();
            DP_ASSERT(g == activeGroup);
        }

        bool skipPreLinkTraining = (((g->singleHeadMultiStreamMode == DP_SINGLE_HEAD_MULTI_STREAM_MODE_MST) ||
                                     (g->singleHeadMultiStreamMode == DP_SINGLE_HEAD_MULTI_STREAM_MODE_SST)) &&
                                    (g->singleHeadMultiStreamID == DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_SECONDARY));
         if (!skipPreLinkTraining)
            main->preLinkTraining(g->headIndex);

        beforeDeleteStream(g, true);
        if (this->linkUseMultistream())
        {
            main->configureTriggerSelect(g->headIndex, g->singleHeadMultiStreamID);
            main->triggerACT();
        }
        afterDeleteStream(g);
    }

    return true;
}

//
// This is a wrapper for call to mainlink::train().
bool ConnectorImpl::rawTrain(const LinkConfiguration & lConfig, bool force, LinkTrainingType linkTrainingType)
{
    {
        //
        // this is the common path
        // activeLinkConfig will be updated in main->train() in case fallback happens.
        // if the link config sent has disable Post LT request set, we send false for corresponding flag
        //
        if (lConfig.disablePostLTRequest)
        {
            return (main->train(lConfig, force, linkTrainingType, &activeLinkConfig, bSkipLt, false,
                    hal->getPhyRepeaterCount()));
        }
        return (main->train(lConfig, force, linkTrainingType, &activeLinkConfig, bSkipLt, hal->isPostLtAdjustRequestSupported(),
            hal->getPhyRepeaterCount()));
    }
}

//
//  Timeslot management
//

bool ConnectorImpl::deleteAllVirtualChannels()
{
    // Clear the payload table
    hal->payloadTableClearACT();
    if (!hal->payloadAllocate(0, 0, 63))
    {
        DP_PRINTF(DP_WARNING, "DPCONN> Payload table could not be cleared");
    }

    // send clear_payload_id_table
    DP_PRINTF(DP_NOTICE, "DPCONN> Sending CLEAR_PAYLOAD_ID_TABLE broadcast");

    for (unsigned retries = 0 ; retries < 7; retries++)
    {
        ClearPayloadIdTableMessage clearPayload;
        NakData nack;

        if (this->messageManager->send(&clearPayload, nack))
            return true;
    }

    // we should not have reached here.
    DP_ASSERT(0 && "DPCONN> CLEAR_PAYLOAD_ID failed!");
    return false;
}

void ConnectorImpl::clearTimeslices()
{
    for (ListElement * i = activeGroups.begin(); i != activeGroups.end(); i = i->next)
    {
        GroupImpl * group = (GroupImpl *)((Group *)i);
        group->timeslot.PBN = 0;
        group->timeslot.count = 0;
        group->timeslot.begin = 1;
        group->timeslot.hardwareDirty = false;
    }

    maximumSlots = 63;
    freeSlots = maximumSlots;
}


void ConnectorImpl::freeTimeslice(GroupImpl * targetGroup)
{
    // compact timeslot allocation
    for (ListElement * e = activeGroups.begin(); e != activeGroups.end(); e = e->next)
    {
        GroupImpl * group = (GroupImpl *)e;

        if (group->timeslot.begin > targetGroup->timeslot.begin) {
            group->timeslot.begin -= targetGroup->timeslot.count;
            group->timeslot.hardwareDirty = true;

            //
            // enable TRIGGER_ALL on SFs corresponding to the the single head MST driving heads
            // as both both pipelines need to take the affect of the shift happening due to deactivating
            // an MST display being driven through same SOR
            //
            if ((DP_SINGLE_HEAD_MULTI_STREAM_MODE_MST == group->singleHeadMultiStreamMode) &&
                (DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY == group->singleHeadMultiStreamID))
            {
                main->configureTriggerAll(group->headIndex, true);
            }
        }
    }

    // mark stream as free
    freeSlots += targetGroup->timeslot.count;
    targetGroup->timeslot.PBN = 0;
    targetGroup->timeslot.count = 0;
    targetGroup->timeslot.hardwareDirty = true;
}

bool ConnectorImpl::checkIsModePossibleMST(GroupImpl *targetGroup)
{
    if (this->isFECSupported())
    {
        if (!isModePossibleMSTWithFEC(activeLinkConfig,
                                      targetGroup->lastModesetInfo,
                                      &targetGroup->timeslot.watermarks))
        {
            DP_ASSERT(0 && "DisplayDriver bug! This mode is not possible at any "
                           "link configuration. It should have been rejected at mode filtering time!");
            return false;
        }
    }
    else
    {
        if (!isModePossibleMST(activeLinkConfig,
                               targetGroup->lastModesetInfo,
                               &targetGroup->timeslot.watermarks))
        {
            DP_ASSERT(0 && "DisplayDriver bug! This mode is not possible at any "
                           "link configuration. It should have been rejected at mode filtering time!");
            return false;
        }
    }
    return true;
}

bool ConnectorImpl::allocateTimeslice(GroupImpl * targetGroup)
{
    unsigned base_pbn, slot_count, slots_pbn;
    int firstSlot = firstFreeSlot;

    DP_ASSERT(isLinkActive());

    if (!checkIsModePossibleMST(targetGroup))
        return false;

    activeLinkConfig.pbnRequired(targetGroup->lastModesetInfo, base_pbn, slot_count, slots_pbn);

    applyTimeslotWAR(slot_count);

    // Check for available timeslots
    if (slot_count > freeSlots)
    {
        DP_PRINTF(DP_ERROR, "DP-TS> Failed to allocate timeslot!! Not enough free slots. slot_count: %d, freeSlots: %d",
                  slot_count, freeSlots);
        return false;
    }

    for (ListElement * i = activeGroups.begin(); i != activeGroups.end(); i = i->next)
    {
        GroupImpl * group = (GroupImpl *)i;

        if (group->timeslot.count != 0 &&
            (group->timeslot.begin + group->timeslot.count) >= firstSlot)
        {
            firstSlot = group->timeslot.begin + group->timeslot.count;
        }
    }

    // Already allocated?
    DP_ASSERT(!targetGroup->timeslot.count && "Reallocation of stream that is already present");

    targetGroup->timeslot.count = slot_count;
    targetGroup->timeslot.begin = firstSlot;
    targetGroup->timeslot.PBN = base_pbn;
    targetGroup->timeslot.hardwareDirty = true;
    freeSlots -= slot_count;

    return true;
}

void ConnectorImpl::flushTimeslotsToHardware()
{
    for (ListElement * i = activeGroups.begin(); i != activeGroups.end(); i = i->next)
    {
        GroupImpl * group = (GroupImpl *)i;

        if (group->timeslot.hardwareDirty)
        {
            group->timeslot.hardwareDirty = false;
            bool bEnable2Head1Or = false;

            if ((group->lastModesetInfo.mode == DSC_DUAL) ||
                (group->lastModesetInfo.mode == DSC_DROP))
            {
                bEnable2Head1Or = true;
            }

            main->configureMultiStream(group->headIndex,
                                       group->timeslot.watermarks.hBlankSym,
                                       group->timeslot.watermarks.vBlankSym,
                                       group->timeslot.begin,
                                       group->timeslot.begin+group->timeslot.count - 1,
                                       group->timeslot.PBN,
                                       activeLinkConfig.PBNForSlots(group->timeslot.count),
                                       group->colorFormat,
                                       group->singleHeadMultiStreamID,
                                       group->singleHeadMultiStreamMode,
                                       bAudioOverRightPanel,
                                       bEnable2Head1Or);
        }
    }
}

void ConnectorImpl::beforeDeleteStream(GroupImpl * group, bool forFlushMode)
{
    //
    // During flush entry, if the link is not trained, retrain
    // the link so that ACT can be ack'd by the sink.
    // (ACK is only for multistream case)
    //
    // Note: A re-training might be required even in cases where link is not
    // alive in non-flush mode case (Eg: beforeDeleteStream called from NAB).
    // However we cannot simply re-train is such cases, without ensuring that
    // head is not actively driving pixels and this needs to be handled
    // differently .
    //
    if (forFlushMode && linkUseMultistream())
    {
        if(isLinkLost())
        {
            train(highestAssessedLC, false);
        }
    }

    // check if this is a firmware group
    if (group && group->isHeadAttached() && group->headInFirmware)
    {
        // check if MST is enabled and we have inited messagemanager
        if (hal->getSupportsMultistream() && messageManager)
        {
            // Firmware group can be assumed to be taking up all 63 slots.
            group->timeslot.begin = 1;
            group->timeslot.count = 63;
            this->freeSlots = 0;

            // 1. clear the timeslots using CLEAR_PAYLAOD_TABLE
            // 2. clear gpu timeslots.
            if (!deleteAllVirtualChannels())
                DP_ASSERT(0 && "Failed to delete VCs. Vbios state in branch could not be cleaned.");

            freeTimeslice(group);
            flushTimeslotsToHardware();
            group->bWaitForDeAllocACT = false;
            group->setTimeslotAllocated(false);

            return;
        }
    }

    if (linkUseMultistream() &&
        group && group->isHeadAttached() &&
        (group->isTimeslotAllocated() ||
         group->timeslot.count ||
         group->timeslot.hardwareDirty))
    {
        // Detach all the panels from payload
        for (Device * d = group->enumDevices(0); d; d = group->enumDevices(d))
        {
            group->update(d, false);
        }

        freeTimeslice(group);
        flushTimeslotsToHardware();
        group->bWaitForDeAllocACT = true;
        group->setTimeslotAllocated(false);

        // Delete the stream
        hal->payloadTableClearACT();
        hal->payloadAllocate(group->streamIndex, group->timeslot.begin, 0);

        //
        // If entering flush mode, enable RG (with Immediate effect) otherwise for detaching a display,
        // if not single heas MST, not required to enable RG. For single head MST streams deletion, enable
        // RG at loadv
        //
        if (forFlushMode ||
            ((group->singleHeadMultiStreamMode == DP_SINGLE_HEAD_MULTI_STREAM_MODE_MST) &&
             (group->singleHeadMultiStreamID   != DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY)))
        {
            main->controlRateGoverning(group->headIndex, true/*enable*/, forFlushMode /*Immediate/loadv*/);
        }
    }
}

void ConnectorImpl::afterDeleteStream(GroupImpl * group)
{
    if (linkUseMultistream() && group->isHeadAttached() && group->bWaitForDeAllocACT)
    {
        if (!hal->payloadWaitForACTReceived())
        {
            DP_PRINTF(DP_ERROR, "DP> Delete stream failed.  Device did not acknowledge stream deletion ACT!");
            DP_ASSERT(0);
        }
    }
}

void ConnectorImpl::afterAddStream(GroupImpl * group)
{
    // Skip this as there is no timeslot allocation
    if (!linkUseMultistream() || !group->timeslot.count)
        return;

    if (group->bDeferredPayloadAlloc)
    {
        DP_ASSERT(addStreamMSTIntransitionGroups.contains(group));
        hal->payloadTableClearACT();
        hal->payloadAllocate(group->streamIndex, group->timeslot.begin, group->timeslot.count);
        main->triggerACT();
    }
    group->bDeferredPayloadAlloc = false;

    if (addStreamMSTIntransitionGroups.contains(group)) {
        addStreamMSTIntransitionGroups.remove(group);
    }

    if (!hal->payloadWaitForACTReceived())
    {
        DP_PRINTF(DP_ERROR, "ACT has not been received.Triggering ACT once more");
        DP_ASSERT(0);

        //
        // Bug 1334070: During modeset for cloned displays on certain GPU family,
        // ACT triggered during SOR attach is not being received due to timing issues.
        // Also DP1.2 spec mentions that there is no harm in sending the ACT
        // again if there is no change in payload table. Hence triggering ACT once more here
        //
        main->triggerACT();
        if (!hal->payloadWaitForACTReceived())
        {
            DP_PRINTF(DP_ERROR, "DP-TS> Downstream device did not receive ACT during stream re-add.");
            return;
        }
    }

    for (Device * d = group->enumDevices(0); d; d = group->enumDevices(d))
    {
        group->update((DeviceImpl *)d, true);

        lastDeviceSetForVbios = d;
    }

    // Disable rate gov at the end of adding all streams
    if ((DP_SINGLE_HEAD_MULTI_STREAM_MODE_MST != group->singleHeadMultiStreamMode) ||
        (DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_MAX == group->singleHeadMultiStreamID))
    {
        main->controlRateGoverning(group->headIndex, false/*disable*/, false/*loadv*/);
    }

    group->updateVbiosScratchRegister(lastDeviceSetForVbios);
}

bool ConnectorImpl::beforeAddStream(GroupImpl * group, bool test, bool forFlushMode)
{
    bool res = false;
    if (linkUseMultistream())
    {
        res = beforeAddStreamMST(group, test, forFlushMode);
    }
    else
    {
        // SST
        Watermark water;
        bool bEnable2Head1Or = false;
        bool bIsModePossible = false;

        if ((group->lastModesetInfo.mode == DSC_DUAL) ||
            (group->lastModesetInfo.mode == DSC_DROP))
        {
            bEnable2Head1Or = true;
        }

        if (this->isFECSupported())
        {
            bIsModePossible = isModePossibleSSTWithFEC(activeLinkConfig,
                                                       group->lastModesetInfo,
                                                       &water,
                                                       main->hasIncreasedWatermarkLimits());
        }
        else
        {
            bIsModePossible = isModePossibleSST(activeLinkConfig,
                                                group->lastModesetInfo,
                                                &water,
                                                main->hasIncreasedWatermarkLimits());
        }

        if (bIsModePossible)
        {
            if (group->singleHeadMultiStreamMode == DP_SINGLE_HEAD_MULTI_STREAM_MODE_SST)
            {
                if (group->singleHeadMultiStreamID == DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_SECONDARY)
                {
                    //
                    // configure sf parameters after secondary linktraining on primary link.
                    //
                    main->configureSingleStream(group->headIndex,
                                                water.hBlankSym,
                                                water.vBlankSym,
                                                activeLinkConfig.enhancedFraming,
                                                water.tuSize,
                                                water.waterMark,
                                                group->colorFormat,
                                                DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY,
                                                group->singleHeadMultiStreamMode,
                                                bAudioOverRightPanel);
                }
            }
            else
            {
                main->configureSingleStream(group->headIndex,
                                            water.hBlankSym,
                                            water.vBlankSym,
                                            activeLinkConfig.enhancedFraming,
                                            water.tuSize,
                                            water.waterMark,
                                            group->colorFormat,
                                            DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY,
                                            DP_SINGLE_HEAD_MULTI_STREAM_MODE_NONE,
                                            false /*bEnableAudioOverRightPanel*/,
                                            bEnable2Head1Or);
            }
        }
        else
        {
            if (test)
            {
                main->configureSingleStream(group->headIndex,
                                            water.hBlankSym,
                                            water.vBlankSym,
                                            activeLinkConfig.enhancedFraming,
                                            water.tuSize,
                                            water.waterMark,
                                            group->colorFormat,
                                            DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY,
                                            DP_SINGLE_HEAD_MULTI_STREAM_MODE_NONE,
                                            false /*bEnableAudioOverRightPanel*/,
                                            bEnable2Head1Or);
                DP_PRINTF(DP_ERROR, "DP-TS> Unable to allocate stream. Setting RG_DIV mode");
                res = true;
            }
            else
                DP_ASSERT(0);
        }
    }
    return res;
}

bool ConnectorImpl::beforeAddStreamMST(GroupImpl * group, bool test, bool forFlushMode)
{
    bool res = false;
    bool isPrimaryStream = (DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY == group->singleHeadMultiStreamID);
    if (allocateTimeslice(group))
    {
        flushTimeslotsToHardware();
        group->setTimeslotAllocated(true);
        if (!forFlushMode && isPrimaryStream)
        {
            main->controlRateGoverning(group->headIndex, true /*enable*/);
        }

        // If not single Head MST mode or if primary stream then program here
        // other streams programmed in NAE
        if (forFlushMode ||
            (isPrimaryStream &&
             addStreamMSTIntransitionGroups.isEmpty()))
        {
            hal->payloadTableClearACT();
            hal->payloadAllocate(group->streamIndex, group->timeslot.begin, group->timeslot.count);
        }
        else if (isPrimaryStream &&
                 !addStreamMSTIntransitionGroups.isEmpty())
        {

            group->bDeferredPayloadAlloc = true;
        }

        addStreamMSTIntransitionGroups.insertFront(group);
    }
    else
    {
        if (!test)
        {
            DP_PRINTF(DP_ERROR, "DP-TS> Unable to allocate stream.  Should call mainLink->configureStream to trigger RG_DIV mode");
            main->configureMultiStream(group->headIndex,
                group->timeslot.watermarks.hBlankSym, group->timeslot.watermarks.vBlankSym,
                1, 0, 0, 0, group->colorFormat, group->singleHeadMultiStreamID, group->singleHeadMultiStreamMode, bAudioOverRightPanel);
        }
        else
        {
            flushTimeslotsToHardware();

            if (forFlushMode ||
                (DP_SINGLE_HEAD_MULTI_STREAM_MODE_MST != group->singleHeadMultiStreamMode) || isPrimaryStream)
            {
                main->configureTriggerSelect(group->headIndex, group->singleHeadMultiStreamID);
                hal->payloadTableClearACT();
                hal->payloadAllocate(group->streamIndex, group->timeslot.begin, group->timeslot.count);
            }

            DP_PRINTF(DP_ERROR, "DP-TS> Unable to allocate stream. Setting RG_DIV mode");
            res = true;
        }
    }

    return res;
}

void ConnectorImpl::disableFlush( bool test)
{
    bool bHeadAttached = false;

    if (activeGroups.isEmpty())
        return;

    sortActiveGroups(true);

    //
    // If SST check that head should be attached with single group else if MST at least
    // 1 group should have headAttached before calling disable flush on SOR
    //
    if (!this->linkUseMultistream())
    {
        GroupImpl * activeGroup = this->getActiveGroupForSST();

        if (activeGroup && !activeGroup->isHeadAttached() && intransitionGroups.isEmpty())
        {
            DP_PRINTF(DP_ERROR, "DPCONN> SST-Flush mode disable should not be called when head is not attached. Returning early without disabling flush");
            return;
        }
    }
    else
    {
        for (ListElement * e = activeGroups.begin(); e != activeGroups.end(); e = e->next)
        {
            GroupImpl * group = (GroupImpl *)e;
            if (group->isHeadAttached())
            {
                bHeadAttached  = true;
                break;
            }
        }

        if (!bHeadAttached)
        {
            DP_PRINTF(DP_ERROR, "DPCONN> MST-Flush mode disable should not be called when head is not attached. Returning early without disabling flush");
            return;
        }
    }

    //
    // We need to rebuild the tiemslot configuration when exiting flush mode
    // Bug 1550750: Change the order to proceed from last to front as they were added.
    // Some tiled monitors are happy with this.
    //
    for (ListElement * e = activeGroups.last(); e != activeGroups.end(); e = e->prev)
    {
        GroupImpl * g = (GroupImpl *)e;
        bool force = false;
        NvU32 headMask = 0;

        if (!g->isHeadAttached() && this->linkUseMultistream())
            continue;

        bool skipPostLinkTraining = (((g->singleHeadMultiStreamMode == DP_SINGLE_HEAD_MULTI_STREAM_MODE_MST) ||
                                      (g->singleHeadMultiStreamMode == DP_SINGLE_HEAD_MULTI_STREAM_MODE_SST)) &&
                                     (g->singleHeadMultiStreamID == DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_SECONDARY));

        //
        // Allocate the timeslot configuration
        //
        force = beforeAddStream(g, test, true);
        if (this->linkUseMultistream())
        {
            main->configureTriggerSelect(g->headIndex, g->singleHeadMultiStreamID);
        }

        if (g->lastModesetInfo.mode == DSC_DUAL)
        {
            // For 2 Head 1 OR - Legal combinations are Head0 and Head1, Head2 and Head3
            headMask = (1 << g->headIndex) | (1 << (g->headIndex + 1));
        }
        else
        {
            headMask = (1 << g->headIndex);
        }

        main->clearFlushMode(headMask, force);       // ACT is triggered here
        if (!skipPostLinkTraining)
            main->postLinkTraining(g->headIndex);
        afterAddStream(g);
    }
}

DeviceImpl* ConnectorImpl::findDeviceInList(const Address & address)
{
    for (ListElement * e = deviceList.begin(); e != deviceList.end(); e = e->next)
    {
        DeviceImpl* device = (DeviceImpl*)e;

        //
        // There may be multiple hits with the same address.  This can
        // happen when the head is still attached to the old device.branch
        // We never need to resurrect old unplugged devices - and their
        // object will be destroyed as soon as the DD handles the
        // notifyZombie message.
        //
        if ((device->address == address) && device->plugged)
            return device;
    }

    //
    // If no plugged devices are found, we should search back through zombied devices.
    // This is purely as an optimizations to allow the automatic restoration of a
    // panel if it 'reappears' while its still being driven
    //
    for (ListElement * e = deviceList.begin(); e != deviceList.end(); e = e->next)
    {
        DeviceImpl* device = (DeviceImpl*)e;

        if (device->address == address)
            return device;
    }

    return 0;
}

void ConnectorImpl::disconnectDeviceList()
{
    for (Device * d = enumDevices(0); d; d = enumDevices(d))
    {
        ((DeviceImpl*)d)->plugged = false;
        // Clear the active bit (payload_allocate)
        ((DeviceImpl*)d)->payloadAllocated = false;

        // Deallocate object which may go stale after long pulse handling.
        if (((DeviceImpl*)d)->isDeviceHDCPDetectionAlive)
        {
            delete ((DeviceImpl*)d)->deviceHDCPDetection;
            ((DeviceImpl*)d)->deviceHDCPDetection = NULL;
            ((DeviceImpl*)d)->isHDCPCap = False;
        }
    }
}

// status == true: attach, == false: detach
void ConnectorImpl::notifyLongPulse(bool statusConnected)
{
    NvU32 muxState = 0;
    NV_DPTRACE_INFO(HOTPLUG, statusConnected, connectorActive, previousPlugged);

    if (!connectorActive)
    {
        DP_PRINTF(DP_ERROR, "DP> Got a long pulse before any connector is active!!");
        return;
    }

    if (main->getDynamicMuxState(&muxState))
    {
        DeviceImpl * existingDev = findDeviceInList(Address());
        bool bIsMuxOnDgpu = DRF_VAL(0073, _CTRL_DFP_DISP_MUX, _STATE, muxState) == NV0073_CTRL_DFP_DISP_MUX_STATE_DISCRETE_GPU;

        if (existingDev && existingDev->isFakedMuxDevice() && !bIsMuxOnDgpu)
        {
            DP_PRINTF(DP_ERROR, "NotifyLongPulse ignored as mux is not pointing to dGPU and there is a faked device. Marking detect complete");
            sink->notifyDetectComplete();
            return;
        }

        if (existingDev && existingDev->isPreviouslyFakedMuxDevice() && !existingDev->isMarkedForDeletion())
        {
            DP_PRINTF(DP_NOTICE, "NotifyLongPulse ignored as there is a previously faked device but it is not marked for deletion");
            if (!statusConnected)
            {
                DP_PRINTF(DP_NOTICE, "Calling notifyDetectComplete");
                sink->notifyDetectComplete();
            }
            return;
        }
    }

    if (previousPlugged && statusConnected)
    {
        if (main->isInternalPanelDynamicMuxCapable()
           )
        {
            return;
        }

        DP_PRINTF(DP_NOTICE, "DP> Redundant plug");
        // When tunneling is enabled send out BW changed event for client to call set modelist again
        timer->queueCallback(this, &tagDpBwAllocationChanged, 0, false /* not allowed in sleep */);

        for (Device * i = enumDevices(0); i; i=enumDevices(i))
        {
            DeviceImpl * dev = (DeviceImpl *)i;
            if (dev->ignoreRedundantHotplug())
            {
                DP_PRINTF(DP_NOTICE, "DP> Skipping link assessment");
                return;
            }
        }

        //
        // Exit early to avoid coonector re-initialization from breaking MST
        // branch state when streams are allocated.
        // Additional exceptions:
        // - UEFI post(firmwareGroup->headInFirmware)for fresh init.
        // - MST to SST transition for that unplug event may be filtered by RM.
        //   Messaging will be disabled in this case.
        //
        if (linkUseMultistream() && (!activeGroups.isEmpty()) &&
            (!(firmwareGroup && ((GroupImpl *)firmwareGroup)->headInFirmware)) &&
            (hal->isMessagingEnabled()))
        {
            DP_PRINTF(DP_ERROR, "DP> Bail out early on redundant hotplug with active MST stream");
            return;
        }
    }

    this->notifyLongPulseInternal(statusConnected);
}

/*!
 * @brief Compute the max BW required across all devices and try to allocate that BW
 *
 * @return      Boolean to indicate success or failure
 */
bool ConnectorImpl::updateDpTunnelBwAllocation()
{
    NvU64 connectorTunnelBw = 0;
    if (!hal->isDpTunnelBwAllocationEnabled())
    {
        return true;
    }

    for (Device * i = enumDevices(0); i; i = enumDevices(i))
    {
        DeviceImpl * dev = (DeviceImpl *)i;
        NvU64 devMaxModeBwRequired = dev->getMaxModeBwRequired();
        connectorTunnelBw += devMaxModeBwRequired;
    }

    DP_PRINTF(DP_INFO, "Required Connector Tunnel BW: %d Mbps", connectorTunnelBw / (1000 * 1000));

    NvU64 maxTunnelBw = getMaxTunnelBw();
    if (connectorTunnelBw > maxTunnelBw)
    {
        DP_PRINTF(DP_INFO, "Requested connector tunnel BW is larger than max Tunnel BW of %d Mbps. Overriding Max Tunnel BW\n",
                  maxTunnelBw / (1000 * 1000));
        connectorTunnelBw = maxTunnelBw;
    }

    if (!allocateDpTunnelBw(connectorTunnelBw))
    {
        DP_PRINTF(DP_ERROR, "Failed to allocate Dp Tunnel BW: %d Mbps", connectorTunnelBw / (1000 * 1000));
        return false;
    }
    return true;
}

//
// notifyLongPulse() filters redundant hotplug notifications and calls into
// notifyLongPulseInternal().
//
// setAllowMultiStreaming() calls into notifyLongPulseInternal() in order to
// re-detect already connected sink after enabling/disabling
// MST support.
//
void ConnectorImpl::notifyLongPulseInternal(bool statusConnected)
{
    // start from scratch when forcePreferredLinkConfig is not set
    if (!(preferredLinkConfig.isValid() && this->forcePreferredLinkConfig))
    {
        preferredLinkConfig = LinkConfiguration();
    }

    bPConConnected = false;
    bSkipAssessLinkForPCon = false;

    hal->initialize();

    // DP2.1 Capability info about USB-C cable
    NV0073_CTRL_DP_USBC_CABLEID_INFO cableIDInfo = { 0 };
    if (main->getUSBCCableIDInfo(&cableIDInfo))
    {
        hal->setUSBCCableIDInfo(&cableIDInfo);
    }
    else
    {
        hal->setUSBCCableIDInfo(NULL);
    }

    //
    // Check if the panel is eDP and DPCD data for that is already parsed.
    // Passing this as a parameter inside notifyHPD to skip reading of DPCD
    // data in case of eDP after sleep/hibernate resume.
    //
    hal->notifyHPD(statusConnected, (!hal->isDpcdOffline() && main->isEDP()));
    if (main->isLttprSupported())
    {
        //
        // Update LTTPR counts since it's only correct after HPD.
        // If there are some other DFP parameters might change during HPD cycle
        // then we can remove the isLttprSupported() check.
        //
        main->queryAndUpdateDfpParams();
    }

    // For bug 2489143, max link rate needs to be forced on eDP through regkey
    if (main->isEDP())
    {
        hal->overrideMaxLinkRate(maxLinkRateFromRegkey);
    }

    // Some panels whose TCON erroneously sets DPCD 0x200 SINK_COUNT=0.
    if (main->isEDP() && hal->getSinkCount() == 0)
        hal->setSinkCount(1);

    // disconnect all devices
    for (ListElement * i = activeGroups.begin(); i != activeGroups.end(); i = i->next) {
        GroupImpl * g = (GroupImpl *)i;

        // Clear the timeslot table
        freeTimeslice(g);
    }

    disconnectDeviceList();

    auxBus->setDevicePlugged(statusConnected);

    if (statusConnected)
    {
        // Reset all settings for previous downstream device
        configInit();

        if (!hal->isAtLeastVersion(1, 0))
            goto completed;

        DP_PRINTF(DP_NOTICE, "DP> HPD v%d.%d", hal->getRevisionMajor(), hal->getRevisionMinor());

        //
        // Handle to clear pending CP_IRQ that throw short pulse before L-HPD. There's no
        // more short pulse corresponding to CP_IRQ after HPD, but IRQ vector needs to be
        // clear or block following CP_IRQ.
        //
        if (hal->interruptContentProtection())
        {
            DP_PRINTF(DP_NOTICE, "DP>clear pending CP interrupt at hpd");
            hal->clearInterruptContentProtection();
        }

        populateAllDpConfigs();

        //
        // Perform OUI authentication
        //
        if (!performIeeeOuiHandshake() && hal->isAtLeastVersion(1, 2))
        {
            DP_PRINTF(DP_WARNING, "DP> OUI Noncompliance! Sink is DP 1.2 and is required to implement");
        }

        // Apply Oui WARs here
        this->applyOuiWARs();

        hal->notifySDPErrDetectionCapability();

        // Tear down old message manager
        DP_ASSERT( !hal->getSupportsMultistream() || (hal->isAtLeastVersion(1, 2) && " Device supports multistream but not DP 1.2 !?!? "));

        // Check if we should be attempting a transition between MST<->SST
        if (main->hasMultistream())
        {
            if (linkState == DP_TRANSPORT_MODE_INIT)
            {
                linkState = hal->getSupportsMultistream() ?
                                    DP_TRANSPORT_MODE_MULTI_STREAM :
                                    DP_TRANSPORT_MODE_SINGLE_STREAM;
                linkAwaitingTransition = false;
            }
            else
            {
                if (linkUseMultistream() != hal->getSupportsMultistream())
                {
                    linkAwaitingTransition = true;
                    DP_PRINTF(DP_NOTICE, "CONN> Link Awaiting Transition.");
                }
                else
                {
                    linkAwaitingTransition = false;
                }
            }
        }

        //
        //    Only transition between multiStream and single stream when there
        //    are no active panels.  Note: that if we're unable to transition
        //    we will mark all of the displays as MUST_DISCONNECT.
        //

        //
        //     Shutdown the old message manager if there was one
        //     If there is a previous stale messageManager or discoveryManager
        //     present then there is a chance on certain docks where MSTM bits
        //     needs to be cleared as previous transactions might still be in
        //     flight. Just checking IRQ VECTOR field might not be enough to
        //     check for stale messages.
        //     Please see bug 3928070/4066192
        //
        if (discoveryManager || messageManager)
        {
            bForceClearPendingMsg = true;
        }
        delete discoveryManager;
        isDiscoveryDetectComplete = false;
        bIsDiscoveryDetectActive = true;

        pendingEdidReads.clear();   // destroy any half completed requests
        delete messageManager;
        messageManager = 0;
        discoveryManager = 0;

        cancelHdcpCallbacks();

        if (hal->getSupportsMultistream() && main->hasMultistream())
        {
            bool bDeleteFirmwareVC = false;

            DP_PRINTF(DP_NOTICE, "DP> Multistream panel detected, building message manager");

            // Update preferredLinkConfig multistream status to MST
            if (preferredLinkConfig.isValid() && this->forcePreferredLinkConfig)
            {
                preferredLinkConfig.multistream = true;
            }

            //
            // Rebuild the message manager to reset and half received messages
            //   that may be in the pipe.
            //
            messageManager = new MessageManager(hal, timer);
            messageManager->registerReceiver(&ResStatus);

            //
            // Create a discovery manager to initiate detection
            //
            if (AuxRetry::ack != hal->setMessagingEnable(true, true))
            {
                DP_PRINTF(DP_WARNING, "DP> Failed to enable messaging for multistream panel");
            }

            if (AuxRetry::ack != hal->setMultistreamHotplugMode(IRQ_HPD))
            {
                DP_PRINTF(DP_WARNING, "DP> Failed to enable hotplug mode for multistream panel");
            }

            discoveryManager = new DiscoveryManager(messageManager, this, timer, hal);

            // Check and clear if any pending message here
            if (hal->clearPendingMsg() ||  bForceClearPendingMsg)
            {
                DP_PRINTF(DP_NOTICE, "DP> Stale MSG found: set branch to D3 and back to D0...");
                if (hal->isAtLeastVersion(1, 4))
                {
                    hal->setMessagingEnable(false, true);
                }
                hal->setPowerState(PowerStateD3);
                hal->setPowerState(PowerStateD0);
                if (hal->isAtLeastVersion(1, 4))
                {
                    hal->setMessagingEnable(true, true);
                }
            }
            pendingRemoteHdcpDetections = 0;

            //
            // We need to clear payload table and payload id table during a hotplug in cases
            // where DD does not send a null modeset for a device that was plugged. Otherwise
            // this will lead to issues where branch does not clear the PBN and sends stale
            // available PBN values. One of the scenarios is BSOD in SLI mode, where the secondary
            // GPUs are not used for primary boot by VBIOS
            //
            bDeleteFirmwareVC = ((GroupImpl *)firmwareGroup &&
                                   !((GroupImpl *)firmwareGroup)->isHeadAttached() &&
                                   !bIsUefiSystem);

            if (bDeleteFirmwareVC || !bAttachOnResume)
            {
                deleteAllVirtualChannels();
            }

            assessLink();                                   // Link assessment may re-add a stream
                                                            // and must be done AFTER the messaging system
                                                            // is restored.
            discoveryManager->notifyLongPulse(true);
        }
        else  // SST case
        {
            DiscoveryManager::Device dev;
            Edid tmpEdid;
            bool isComplianceForEdidTest = false;
            dev.address = Address();

            // Update preferredLinkConfig multistream status to SST
            if (preferredLinkConfig.isValid() && this->forcePreferredLinkConfig)
            {
                preferredLinkConfig.multistream = false;
            }

            if (AuxRetry::ack != hal->setMessagingEnable(false, true))
            {
                DP_PRINTF(DP_WARNING, "DP> Failed to clear messaging for singlestream panel");
            }

            //  We will report a dongle as new device with videoSink flag as false.
            if (hal->getSinkCount() == 0)
            {
                dev.peerDevice = Dongle;
            }
            else
            {
                dev.peerDevice = DownstreamSink;

                //  Handle fallback EDID
                if(!EdidReadSST(tmpEdid, auxBus, timer,
                                hal->getPendingTestRequestEdidRead(),
                                main->isForceRmEdidRequired(),
                                main->isForceRmEdidRequired() ? main : 0))
                {
                    bool status = false;
                    //
                    // For some DP2VGA dongle which is unable to get the right after several retries.
                    // Before library, we do give 26 times retries for DP2VGA dongle EDID retries.
                    // Give most 24 times here for another re-start in library.
                    // Bug 996248.
                    //
                    if (hal->getLegacyPortCount())
                    {
                        LegacyPort * port = hal->getLegacyPort(0);
                        if (port->getDownstreamPortType() == ANALOG_VGA)
                        {
                            NvU8 retries = DP_READ_EDID_MAX_RETRIES;
                            for (NvU8 i = 0; i < retries; i++)
                            {
                                status = EdidReadSST(tmpEdid, auxBus, timer,
                                                     hal->getPendingTestRequestEdidRead(),
                                                     main->isForceRmEdidRequired(),
                                                     main->isForceRmEdidRequired() ? main : 0);
                                if (status)
                                    break;
                            }
                        }
                    }
                    if (!status)
                    {
                        // corrupt edid
                        DP_PRINTF(DP_ERROR, "DP-CONN> Corrupt Edid!");

                        // Reading the EDID can fail if AUX is dead.
                        // So update DPCD state after max number of retries.
                        hal->updateDPCDOffline();
                    }
                }

                DP_PRINTF(DP_NOTICE, "DP-CONN> Edid read complete: Manuf Id: 0x%x, Name: %s", tmpEdid.getManufId(), tmpEdid.getName());
                dev.branch = false;
                dev.dpcdRevisionMajor = hal->getRevisionMajor();
                dev.dpcdRevisionMinor = hal->getRevisionMinor();
                dev.legacy = false;
                dev.SDPStreams = hal->getNumberOfAudioEndpoints() ? 1 : 0;
                dev.SDPStreamSinks = hal->getNumberOfAudioEndpoints();
                dev.videoSink = true;
                dev.maxTmdsClkRate = 0U;

                // Apply EDID based WARs and update the WAR flags if needed
                applyEdidWARs(tmpEdid, dev);

                //
                // HP Valor QHD+ needs 50ms delay after D3
                // to prevent black screen
                //
                if (tmpEdid.WARFlags.delayAfterD3)
                {
                    bDelayAfterD3 = true;
                }
                // Do not reset MST_EN before LT for Sony SDM27Q10S in SST mode
                this->bSkipResetMSTMBeforeLt = tmpEdid.WARFlags.bSkipResetMSTMBeforeLt;

                // Panels use Legacy address range for interrupt reporting
                if (tmpEdid.WARFlags.useLegacyAddress)
                {
                    hal->setSupportsESI(false);
                }

                //
                // For some devices short pulse comes in after we disconnect the
                // link, so DPLib ignores the request and link trains after modeset
                // happens. When modeset happens the link configuration picked may
                // be different than what we assessed before. So we skip the link
                // power down in assessLink() in such cases
                //
                if (tmpEdid.WARFlags.keepLinkAlive)
                {
                    DP_PRINTF(DP_NOTICE, "tmpEdid.WARFlags.keepLinkAlive = true, set bKeepOptLinkAlive to true. (keep link alive after assessLink())");
                    bKeepOptLinkAlive = true;
                }
                // Ack the test response, no matter it is a ref sink or not
                if (hal->getPendingTestRequestEdidRead())
                {
                    isComplianceForEdidTest = true;
                    hal->setTestResponseChecksum(tmpEdid.getLastPageChecksum());
                    hal->setTestResponse(true, true);
                }
            }

            //
            // If this is a zombie VRR device that was previously enabled,
            // re-enable it now.  This must happen before link training if
            // VRR was enabled before the device became a zombie or else the
            // monitor will report that it's in normal mode even if the GPU is
            // driving it in VRR mode.
            //
            {
                DeviceImpl * existingDev = findDeviceInList(dev.address);
                if (existingDev && existingDev->isVrrMonitorEnabled() &&
                    !existingDev->isVrrDriverEnabled())
                {
                    DP_PRINTF(DP_NOTICE, "DP> Re-enabling previously enabled zombie VRR monitor");
                    existingDev->resetVrrEnablement();
                    existingDev->startVrrEnablement();
                }
            }

            if ((hal->getPCONCaps())->bSourceControlModeSupported)
            {
                bPConConnected = true;
            }

            LinkConfiguration maxLinkConfig = getMaxLinkConfig();
            if (bPConConnected ||
                (main->isEDP() && this->bSkipAssessLinkForEDP) ||
                (main->isInternalPanelDynamicMuxCapable()))
            {
                this->highestAssessedLC = maxLinkConfig;
                this->linkGuessed = bPConConnected;
                this->bSkipAssessLinkForPCon = bPConConnected;
            }
            else
            {
                if (tmpEdid.WARFlags.powerOnBeforeLt)
                {
                    //
                    // Some panels expose that they are in D0 even when they are not.
                    // Explicit write to DPCD 0x600 is required to wake up such panel before LT.
                    //
                    hal->setPowerState(PowerStateD0);
                }
                this->assessLink();
            }

            if (hal->getLegacyPortCount() != 0)
            {
                LegacyPort * port = hal->getLegacyPort(0);
                DwnStreamPortType portType = port->getDownstreamPortType();
                dev.maxTmdsClkRate = port->getMaxTmdsClkRate();
                processNewDevice(dev, tmpEdid, false, portType, port->getDownstreamNonEDIDPortAttribute());
            }
            else
            {
                processNewDevice(dev, tmpEdid, false, DISPLAY_PORT, RESERVED, isComplianceForEdidTest);
            }

            // After processNewDevice, we should not defer any lost device.
            bDeferNotifyLostDevice = false;
        }
    }
    else    // HPD unplug
    {
        //
        //     Shutdown the old message manager if there was one
        //
        delete discoveryManager;
        isDiscoveryDetectComplete = false;
        pendingEdidReads.clear();   // destroy any half completed requests
        bDeferNotifyLostDevice = false;

        delete messageManager;
        messageManager = 0;
        discoveryManager = 0;
        bAcpiInitDone = false;
        bKeepOptLinkAlive = false;
        bNoFallbackInPostLQA = false;
        bDscCapBasedOnParent = false;
        linkState = DP_TRANSPORT_MODE_INIT;
        linkAwaitingTransition = false;

    }
completed:
    previousPlugged = statusConnected;

    fireEvents();

    if (!statusConnected)
    {
        sink->notifyDetectComplete();
        return;
    }
    if (!(hal->getSupportsMultistream() && main->hasMultistream()))
    {
        // Ensure NewDev will be processed before notifyDetectComplete on SST
        discoveryDetectComplete();
    }
}

void ConnectorImpl::handleDpTunnelingIrq()
{
    bool notifyClient = false;
    // Unconditionally reset the BW request status
    hal->clearDpTunnelingBwRequestStatus();

    if (hal->hasDpTunnelEstimatedBwChanged())
    {
        NvU64 previousAllocatedDpTunnelBw = allocatedDpTunnelBw;
        updateDpTunnelBwAllocation();
        if (previousAllocatedDpTunnelBw < allocatedDpTunnelBw)
        {
            notifyClient = true;
        }

        hal->clearDpTunnelingEstimatedBwStatus();
    }

    if (hal->hasDpTunnelBwAllocationCapabilityChanged())
    {
        notifyClient = true;
        // Try to allocate max tunnel BW if we enabled BW allocation above
        if (hal->isDpTunnelBwAllocationEnabled())
        {
            allocateMaxDpTunnelBw();
        }

        hal->clearDpTunnelingBwAllocationCapStatus();
    }

    if (notifyClient)
    {
        timer->queueCallback(this, &tagDpBwAllocationChanged, 0, false /* not allowed in sleep */);
    }
}

void ConnectorImpl::notifyShortPulse()
{
    //
    // Do nothing if device is not plugged or
    // resume has not been called after hibernate
    // to activate the connector
    //
    if (!connectorActive || !previousPlugged)
    {
        DP_PRINTF(DP_ERROR, "DP> Got a short pulse after an unplug or before any connector is active!!");
        return;
    }
    DP_PRINTF(DP_INFO, "DP> IRQ");
    hal->notifyIRQ();

    // Handle CP_IRQ
    if (hal->interruptContentProtection())
    {
        // Cancel previous queued delay handling and reset retry counter.
        hdcpCpIrqRxStatusRetries = 0;
        timer->cancelCallback(this, &tagDelayedHDCPCPIrqHandling);

        if (handleCPIRQ())
        {
            hal->clearInterruptContentProtection();
        }
        else
        {
            timer->queueCallback(this, &tagDelayedHDCPCPIrqHandling, HDCP_CPIRQ_RXSTATUS_COOLDOWN);
        }
    }

    if (hal->getStreamStatusChanged())
    {
        if (!messageManager)
        {
            DP_PRINTF(DP_NOTICE, "DP> Received Stream status changed Interrupt, but not in multistream mode. Ignoring.");
        }
        else
        {
            handleSSC();
            hal->clearStreamStatusChanged();

            //
            // Handling of SSC takes longer time during which time we miss IRQs.
            // Populate interrupts again.
            //
            hal->notifyIRQ();
        }
    }

    if (hal->interruptCapabilitiesChanged())
    {
        DP_PRINTF(DP_NOTICE, "DP> Sink capabilities changed, re-reading caps and reinitializing the link.");
        // We need to set dpcdOffline to re-read the caps
        hal->setDPCDOffline(true);
        hal->clearInterruptCapabilitiesChanged();
        notifyLongPulse(true);
        return;
    }

    if (detectSinkCountChange())
    {
        DP_PRINTF(DP_NOTICE, "DP> Change in downstream sink count. Re-analysing link.");
        // We need to set dpcdOffline to re-read the caps
        hal->setDPCDOffline(true);
        notifyLongPulse(true);
        return;
    }

    if (hal->interruptDownReplyReady())
    {
        if (!messageManager)
        {
            DP_PRINTF(DP_NOTICE, "DP> Received DownReply Interrupt, but not in multistream mode. Ignoring.");
        }
        else
        {
            messageManager->IRQDownReply();
        }
    }

    if (hal->interruptUpRequestReady())
    {
        if (!messageManager)
        {
            DP_PRINTF(DP_NOTICE, "DP> Received UpRequest Interrupt, but not in multistream mode. Ignoring.");
        }
        else
        {
            messageManager->IRQUpReqest();
        }
    }

    if (hal->getDownStreamPortStatusChange() && hal->getSinkCount())
    {
        Edid target;
        if (!EdidReadSST(target, auxBus, timer, hal->getPendingTestRequestEdidRead()))
        {
            DP_PRINTF(DP_WARNING, "DP> Failed to read EDID.");
        }

        return;
    }

    if (hal->getPendingAutomatedTestRequest())
    {
        if (hal->getPendingTestRequestEdidRead())
        {
            Edid target;
            if (EdidReadSST(target, auxBus, timer, true))
            {
                hal->setTestResponseChecksum(target.getLastPageChecksum());
                hal->setTestResponse(true, true);
            }
            else
                hal->setTestResponse(false);
        }
        else if (hal->getPendingTestRequestTraining())
        {
            // handleTestLinkTrainRequest will call hal->setTestResponse() once verified the request.
            handleTestLinkTrainRequest();
        }
        else if (hal->getPendingTestRequestPhyCompliance())
        {
            hal->setTestResponse(handlePhyPatternRequest());
        }
    }

    // Handle MCCS_IRQ
    if (hal->intteruptMCCS())
    {
        DP_PRINTF(DP_NOTICE, "DP> MCCS_IRQ");
        handleMCCSIRQ();
        hal->clearInterruptMCCS();
    }

    if (hal->getHdmiLinkStatusChanged())
    {
        DP_PRINTF(DP_NOTICE, "DP> HDMI Link Status Changed");
        handleHdmiLinkStatusChanged();
    }

    if (hal->isPanelReplayErrorSet())
    {
        DP_PRINTF(DP_ERROR, "DP> Sink set Panel replay error");
        handlePanelReplayError();
        hal->clearPanelReplayError();
    }

    if (hal->getDpTunnelingIrq())
    {
        handleDpTunnelingIrq();
        hal->clearDpTunnelingIrq();
    }

    //
    //  Check to make sure sink is not in D3 low power mode
    //  and interlane alignment is good, etc
    //  if not - trigger training
    //
    if (!isLinkInD3() && isLinkLost())
    {
        // If the link status of a VRR monitor has changed, we need to check the enablement again.
        if (hal->getLinkStatusChanged())
        {
            for (Device *i = enumDevices(0); i; i = enumDevices(i))
            {
                DeviceImpl *dev = (DeviceImpl *)i;

                if ((dev->plugged) && (dev->activeGroup != NULL) && (dev->isVrrMonitorEnabled()))
                {
                    // Trigger the full enablement, if the monitor is in locked state.
                    NvU8 retries = VRR_MAX_RETRIES;
                    if (!dev->isVrrDriverEnabled())
                    {
                        DP_PRINTF(DP_NOTICE, "DP> VRR enablement state is not synced. Re-enable it.");
                        do
                        {
                            if (!dev->startVrrEnablement())
                            {
                                continue;
                            }
                            else
                                break;
                        }while(--retries);

                        if (!retries)
                        {
                            DP_PRINTF(DP_WARNING, "DP> VRR enablement failed on multiple retries.");
                        }
                    }
                }
            }
        }

        // If DPCD access is not available, skip trying to restore link configuration.
        hal->updateDPCDOffline();
        if (hal->isDpcdOffline())
        {
            return;
        }

        DP_PRINTF(DP_WARNING, "DP> Link not alive, Try to restore link configuration");

        if (trainSingleHeadMultipleSSTLinkNotAlive(getActiveGroupForSST()))
        {
            return;
        }

        // Save the previous highest assessed LC
        LinkConfiguration previousAssessedLC = highestAssessedLC;
        // Save original active link configuration.
        LinkConfiguration originalActiveLinkConfig = activeLinkConfig;

        if (main->isConnectorUSBTypeC() &&
            activeLinkConfig.bIs128b132bChannelCoding &&
            activeLinkConfig.peakRate > dp2LinkRate_10_0Gbps &&
            main->isCableVconnSourceUnknown())
        {
            if (activeLinkConfig.isValid() && enableFlush())
            {
                train(originalActiveLinkConfig, true);
                disableFlush();
            }

            main->invalidateLinkRatesInFallbackTable(originalActiveLinkConfig.peakRate);
            hal->overrideCableIdCap(originalActiveLinkConfig.peakRate, false);

            highestAssessedLC = getMaxLinkConfig();

            DeviceImpl * dev = findDeviceInList(Address());
            if (dev)
            {
                sink->bandwidthChangeNotification(dev, false);
            }
            return;
        }

        if (activeLinkConfig.isValid() && enableFlush())
        {
            if (!train(originalActiveLinkConfig, false))
            {
                //
                // If original link config could not be restored force
                // original config, else SF will overflow when we come
                // out of flush mode.
                //
                DP_PRINTF(DP_ERROR, "DP> After IRQ, original link config could not be restored. Forcing config");
                train(originalActiveLinkConfig, true);
            }
            disableFlush();
        }

        // if link train fails, call assessLink() again.
        if (!activeLinkConfig.isValid())
        {
            assessLink();
        }

        //If the highest assessed LC has changed, send notification
        if(highestAssessedLC != previousAssessedLC)
        {
            DeviceImpl * dev = findDeviceInList(Address());
            if (dev)
            {
                sink->bandwidthChangeNotification(dev, false);
            }
        }
    }
}

bool ConnectorImpl::detectSinkCountChange()
{
    if (this->linkUseMultistream())
        return false;

    DeviceImpl * existingDev = findDeviceInList(Address());
    if (!existingDev)
        return false;

    // detect a zero to non-zero sink count change or vice versa
    bool hasSink = !!(hal->getSinkCount());
    return ((existingDev->videoSink || existingDev->audioSink) != hasSink);
}

/*!
 * @brief Sets the preferred link config which the tool has requested to train to.
 *
 * @param[in]   lc                       client requested link configuration
 * @param[in]   commit                   initiate assessLink with lc
 * @param[in]   force                    link train to lc. Flush Mode is used.
 * @param[in]   trainType                parameter for assessLink for NORMAL, NO, FAST LT
 * @param[in]   forcePreferredLinkConfig cap system LT configuration during NLP, IMP, NAB
 *
 * @return      Boolean to indicate success or failure
 */
bool ConnectorImpl::setPreferredLinkConfig(LinkConfiguration & lc, bool commit,
                                           bool force, LinkTrainingType trainType,
                                           bool forcePreferredLinkConfig)
{
    bool bEnteredFlushMode;
    Device *dev;

    dev = enumDevices(0);
    DeviceImpl * nativeDev = (DeviceImpl *)dev;
    if (preferredLinkConfig.lanes || preferredLinkConfig.peakRate || preferredLinkConfig.minRate)
        DP_ASSERT(0 && "Missing reset call for a preveious set preferred call");

    if (lc.bEnableFEC &&
        ((nativeDev && !nativeDev->isFECSupported()) || (!this->isFECSupported())))
    {
        DP_ASSERT(0 && "Client requested to enable FEC but either panel or GPU doesn't support FEC");
        return false;
    }

    if (!validateLinkConfiguration(lc))
    {
        DP_PRINTF(DP_ERROR, "Client requested bad LinkConfiguration. peakRate=%d, lanes=%d, bIs128b132ChannelCoding=%d",
            lc.peakRate, lc.lanes, lc.bIs128b132bChannelCoding);
        return false;
    }

    preferredLinkConfig = lc;
    preferredLinkConfig.enhancedFraming = hal->getEnhancedFraming();
    preferredLinkConfig.multistream = this->linkUseMultistream();
    preferredLinkConfig.policy = this->linkPolicy;

    // need to force assessLink and during NotifyAttachBegin
    this->forcePreferredLinkConfig = forcePreferredLinkConfig;

    if (force)
    {
        // Do flushmode
        if (!(bEnteredFlushMode = this->enableFlush()))
            DP_ASSERT(0 && "Flush fails");
        if (this->train(preferredLinkConfig, false))
            activeLinkConfig = preferredLinkConfig;
        if (bEnteredFlushMode)
            this->disableFlush(true);
    }
    else
    {
        if (commit)
        {
            assessLink(trainType);
        }
    }

    return true;
}

bool ConnectorImpl::resetPreferredLinkConfig(bool force)
{
    preferredLinkConfig = LinkConfiguration();
    this->forcePreferredLinkConfig = false;

    if (force)
        assessLink();
    return true;
}

bool ConnectorImpl::isAcpiInitDone()
{
    return (hal->getSupportsMultistream() ? false : bAcpiInitDone);
}

void ConnectorImpl::notifyAcpiInitDone()
{
    Edid ddcReadEdid;

    // Initiate the EDID Read mechanism only if it is in SST mode & plugged
    if (!hal->getSupportsMultistream() && previousPlugged)
    {
        // Read EDID using RM Control call - NV0073_CTRL_CMD_SPECIFIC_GET_EDID_V2
        if (EdidReadSST(ddcReadEdid, auxBus, timer, false, true, main))
        {
            // Fill the data in device's ddcEdid & mark ACPI Init done
            for (Device * i = enumDevices(0); i; i=enumDevices(i))
            {
                DP_PRINTF(DP_NOTICE, "DPCONN> ACPI Init Done. DDC EDID Read completed!!");

                DeviceImpl * dev = (DeviceImpl*)i;
                dev->ddcEdid = ddcReadEdid;

                this->bAcpiInitDone = true;
                break;
            }
        }
    }

    return;
}

bool ConnectorImpl::getHDCPAbortCodesDP12(NvU32 &hdcpAbortCodesDP12)
{
    hdcpAbortCodesDP12 = 0;

    return false;
}

bool ConnectorImpl::hdcpValidateKsv(const NvU8 *ksv, NvU32 Size)
{

    if (HDCP_KSV_SIZE <= Size)
    {
        NvU32 i, j;
        NvU32 count_ones = 0;
        for (i=0; i < HDCP_KSV_SIZE; i++)
        {
            for (j = 0; j < 8; j++)
            {
                if (ksv[i] & (1 <<(j)))
                {
                    count_ones++;
                }
            }
        }

        if (count_ones == 20)
        {
            return true;
        }
    }
    return false;
}

void ConnectorImpl::cancelHdcpCallbacks()
{
    this->isHDCPReAuthPending = false;
    this->isHDCPAuthTriggered = false;
    this->authRetries = 0;

    timer->cancelCallback(this, &tagHDCPReauthentication);      // Cancel any queue the auth callback.
    timer->cancelCallback(this, &tagDelayedHdcpCapRead);        // Cancel any HDCP cap callbacks.


    for (ListElement * i = activeGroups.begin(); i != activeGroups.end(); i = i->next)
    {
        GroupImpl * group = (GroupImpl *)i;
        group->cancelHdcpCallbacks();
    }
}

// Create a new Group
Group * ConnectorImpl::newGroup()
{
    Group * g = new GroupImpl(this);
    if (g)
    {
        inactiveGroups.insertBack((GroupImpl*)g);
    }
    return g;
}

// Create a new Group
Group * ConnectorImpl::createFirmwareGroup()
{
    Group * g = new GroupImpl(this, true);
    if (g)
    {
        inactiveGroups.insertBack((GroupImpl*)g);
    }
    return g;
}

// Shutdown and the destroy the connector manager
void ConnectorImpl::destroy()
{
    delete this;
}

void ConnectorImpl::createFakeMuxDevice(const NvU8 *buffer, NvU32 bufferSize)
{
    if (!buffer)
        return;

    // Return immediately if DSC is not supported
    if(FLD_TEST_DRF(_DPCD14, _DSC_SUPPORT, _DECOMPRESSION, _YES, buffer[0]) != 1)
        return;

    DeviceImpl * existingDev = findDeviceInList(Address());

    // Return immediately if we already have a device
    if (existingDev)
    {
        return;
    }

    DeviceImpl *newDev = new DeviceImpl(hal, this, NULL);
    if (!newDev)
    {
        return;
    }

    newDev->connectorType               = connectorDisplayPort;
    newDev->plugged                     = true;
    newDev->videoSink                   = true;
    newDev->bIsFakedMuxDevice           = true;
    newDev->bIsPreviouslyFakedMuxDevice = false;

    // Initialize DSC state
    newDev->dscCaps.bDSCSupported = true;
    newDev->dscCaps.bDSCDecompressionSupported = true;
    if (!(newDev->setRawDscCaps(buffer, DP_MIN(bufferSize, DSC_CAPS_SIZE))))
    {
        DP_ASSERT(0 && "Faking DSC caps failed!");
    }
    newDev->bDSCPossible = true;
    newDev->devDoingDscDecompression = newDev;

    populateAllDpConfigs();
    deviceList.insertBack(newDev);
    sink->newDevice(newDev);
    sink->notifyDetectComplete();
}

void ConnectorImpl::deleteFakeMuxDevice()
{
    DeviceImpl * existingDev = findDeviceInList(Address());
    if (!existingDev)
        return;

    // If this is not a fake device then don't delete it
    if (!existingDev->isPreviouslyFakedMuxDevice())
        return;

    existingDev->markDeviceForDeletion();
    notifyLongPulse(false);

    return;
}

bool ConnectorImpl::getRawDscCaps(NvU8 *buffer, NvU32 bufferSize)
{
    DeviceImpl * existingDev = findDeviceInList(Address());
    if (!existingDev)
        return false;

    return existingDev->getRawDscCaps(buffer, bufferSize);
}

bool ConnectorImpl::isMultiStreamCapable()
{
    return main->hasMultistream();
}

bool ConnectorImpl::isFlushSupported()
{
    return true;
}

bool ConnectorImpl::isStreamCloningEnabled()
{
    return main->isStreamCloningEnabled();
}

bool ConnectorImpl::isFECSupported()
{
    return main->isFECSupported();
}

bool ConnectorImpl::isFECCapable()
{
    DeviceImpl *dev;

    for (Device * i = enumDevices(0); i; i = enumDevices(i))
    {
        dev = (DeviceImpl *)i;
        // If it's SST, or if it's the first connected branch.
        if (!this->linkUseMultistream() || dev->address.size() == 1)
        {
            return (dev->getFECSupport() && this->isFECSupported());
        }
    }
    return false;
}

NvU32 ConnectorImpl::maxLinkRateSupported()
{
    return main->maxLinkRateSupported();
}

Connector * DisplayPort::createConnector
(
    MainLink * main,
    AuxBus * aux,
    Timer * timer,
    Connector::EventSink * sink
)
{
    ConnectorImpl *connector;
    EvoInterface *provider = ((EvoMainLink *)main)->getProvider();
    NvU32 nvosStatus;
    NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS dpParams = {0};

    dpParams.subDeviceInstance = provider->getSubdeviceIndex();
    nvosStatus = provider->rmControl0073(NV0073_CTRL_CMD_DP_GET_CAPS, &dpParams, sizeof dpParams);

    if (nvosStatus != NVOS_STATUS_SUCCESS)
    {
        DP_ASSERT(0 && "Unable to get DP caps params");
        return NULL;
    }

    if (FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_DP_VERSIONS_SUPPORTED,
                     _DP2_0, _YES, dpParams.dpVersionsSupported))
    {
        connector = new ConnectorImpl2x(main, aux, timer, sink);
    }
    else
    {
        connector = new ConnectorImpl(main, aux, timer, sink);
    }

    if (connector == NULL || connector->constructorFailed) {
        delete connector;
        return NULL;
    }

    if (main->getRegkeyValue(NV_DP_REGKEY_ENABLE_OCA_LOGGING))
    {
        main->retrieveRingBuffer(LOG_CALL, MAX_RECORD_COUNT);
        main->retrieveRingBuffer(ASSERT_HIT, MAX_RECORD_COUNT);
    }
    return connector;
}

void ConnectorImpl::setAllowMultiStreaming(bool bAllowMST)
{
    //
    // hal->getMultiStreamCapOverride() returns true, if MST cap has been
    // overridden to SST.
    //
    if (!hal->getMultiStreamCapOverride() == bAllowMST)
        return;

    if (previousPlugged &&
        getSinkMultiStreamCap() &&
        !activeGroups.isEmpty() && linkUseMultistream() != bAllowMST)
    {
        DP_ASSERT(!"If connected sink is MST capable then:"
                   "Client should detach all active MST video/audio streams "
                   "before disallowing MST, vise-versa client should detach "
                   "active SST stream before allowing MST.");
    }

    //
    // Disable MST messaging, if client has disallowed MST;
    // notifyLongPulseInternal() enable back MST messaging when client
    // allow MST.
    //
    if (previousPlugged && linkUseMultistream() && !bAllowMST)
        hal->setMessagingEnable(
            false /* _uprequestEnable */, true /* _upstreamIsSource */);

    hal->overrideMultiStreamCap(bAllowMST /* mstCapable */ );

    // Re-detect already connected sink, and to keep software state in sync
    if (previousPlugged && getSinkMultiStreamCap())
    {
        isHDCPAuthOn = isDP12AuthCap = false;
        notifyLongPulseInternal(true);
    }
}

bool ConnectorImpl::getAllowMultiStreaming(void)
{
    //
    // hal->getMultiStreamCapOverride() returns true, if MST cap has been
    // overridden to SST.
    //
    return !hal->getMultiStreamCapOverride();
}

bool ConnectorImpl::getSinkMultiStreamCap(void)
{
    return hal->getDpcdMultiStreamCap();
}

void ConnectorImpl::setDp11ProtocolForced()
{
    if (!this->linkUseMultistream())
    {
        return;
    }

    this->notifyLongPulse(false);
    hal->setMessagingEnable(false, true);
    hal->setMultistreamLink(false);
    hal->overrideMultiStreamCap(false /*no mst*/);
    this->notifyLongPulse(true);
}

void ConnectorImpl::resetDp11ProtocolForced()
{
    if (this->linkUseMultistream())
    {
        return;
    }

    this->notifyLongPulse(false);
    hal->overrideMultiStreamCap(true /*mst capable*/);
    this->notifyLongPulse(true);
}

bool ConnectorImpl::isDp11ProtocolForced()
{
    return hal->getMultiStreamCapOverride();
}

bool ConnectorImpl::getTestPattern(NV0073_CTRL_DP_TESTPATTERN * testPattern)
{
    return (main->getDpTestPattern(testPattern));
}

bool ConnectorImpl::setTestPattern(NV0073_CTRL_DP_TESTPATTERN testPattern, NvU8 laneMask, NV0073_CTRL_DP_CSTM cstm, NvBool bIsHBR2, NvBool bSkipLaneDataOverride)
{
    return (main->setDpTestPattern(testPattern, laneMask, cstm, bIsHBR2, bSkipLaneDataOverride));
}

bool ConnectorImpl::getLaneConfig(NvU32 *numLanes, NvU32 *data)
{
    return (main->getDpLaneData(numLanes, data));
}

bool ConnectorImpl::setLaneConfig(NvU32 numLanes, NvU32 *data)
{
    return (main->setDpLaneData(numLanes, data));
}

void ConnectorImpl::getCurrentLinkConfig(unsigned & laneCount, NvU64 & linkRate)
{
    main->getLinkConfig(laneCount, linkRate);
}

void ConnectorImpl::getCurrentLinkConfigWithFEC(unsigned & laneCount, NvU64 & linkRate, bool &bFECEnabled)
{
    main->getLinkConfigWithFEC(laneCount, linkRate, bFECEnabled);
}
unsigned ConnectorImpl::getPanelDataClockMultiplier()
{
    LinkConfiguration linkConfig = getMaxLinkConfig();
    return getDataClockMultiplier(linkConfig.convertLinkRateToDataRate(linkConfig.peakRatePossible), linkConfig.lanes);
}

unsigned ConnectorImpl::getGpuDataClockMultiplier()
{
    unsigned laneCount;
    NvU64 linkRate;
    // Need to get the GPU caps, not monitor caps.
    linkRate = maxLinkRateSupported();
    linkRate = LINK_RATE_TO_DATA_RATE_8B_10B(linkRate);
    laneCount = laneCount_4;

    return getDataClockMultiplier(linkRate, laneCount);
}

void ConnectorImpl::configurePowerState(bool bPowerUp)
{
    main->configurePowerState(bPowerUp);
}

bool ConnectorImpl::readPsrState(vesaPsrState *psrState)
{
    return hal->readPsrState(psrState);
}

void ConnectorImpl::readPsrCapabilities(vesaPsrSinkCaps *caps)
{
    hal->readPsrCapabilities(caps);
}

bool ConnectorImpl::readPsrConfiguration(vesaPsrConfig *psrConfig)
{
    return hal->readPsrConfiguration(psrConfig);
}

bool ConnectorImpl::updatePsrConfiguration(vesaPsrConfig config)
{
    return hal->updatePsrConfiguration(config);
}

bool ConnectorImpl::readPsrDebugInfo(vesaPsrDebugStatus *psrDbgState)
{
    return hal->readPsrDebugInfo(psrDbgState);
}

bool ConnectorImpl::writePsrErrorStatus(vesaPsrErrorStatus psrErr)
{
    return hal->writePsrErrorStatus(psrErr);
}

bool ConnectorImpl::readPsrErrorStatus(vesaPsrErrorStatus *psrErr)
{
    return hal->readPsrErrorStatus(psrErr);
}

bool ConnectorImpl::writePsrEvtIndicator(vesaPsrEventIndicator psrEvt)
{
    return hal->writePsrEvtIndicator(psrEvt);
}

bool ConnectorImpl::readPsrEvtIndicator(vesaPsrEventIndicator *psrEvt)
{
    return hal->readPsrEvtIndicator(psrEvt);
}

bool ConnectorImpl::updatePsrLinkState(bool bTurnOnLink)
{
    bool bRet = true;
    bool bEnteredFlushMode = false;

    if (bTurnOnLink)
    {
        hal->setPowerState(PowerStateD0);

        if (isLinkLost())
        {
            if (!this->psrLinkConfig.isValid())
            {
                DP_ASSERT(0 && "Invalid PSR link config");
                return false;
            }

            // NOTE: always verify changes to below line with 2H1OR case
            if (!(bEnteredFlushMode = this->enableFlush()))
            {
                DP_ASSERT(0 && "Flush fails");
            }

            bRet = this->train(this->psrLinkConfig, false);

            if (bEnteredFlushMode)
            {
                this->disableFlush(true);
            }
        }
        else
        {
            // return early if link is already up
            return true;
        }
    }
    else
    {
        // Save the current link config
        this->psrLinkConfig = getActiveLinkConfig();
    }
    return bRet;
}

bool ConnectorImpl::readPrSinkDebugInfo(panelReplaySinkDebugInfo *prDbgInfo)
{
    return hal->readPrSinkDebugInfo(prDbgInfo);
}

bool ConnectorImpl::handlePhyPatternRequest()
{

    bool status = true;
    PatternInfo pattern_info;

    pattern_info.lqsPattern = hal->getPhyTestPattern();

    // Get lane count from most current link training
    unsigned requestedLanes = this->activeLinkConfig.lanes;

    if (pattern_info.lqsPattern == LINK_QUAL_80BIT_CUST)
    {
        hal->get80BitsCustomTestPattern((NvU8 *)&pattern_info.ctsmLower);
    }

    // send control call to rm for the pattern
    if (!main->physicalLayerSetTestPattern(&pattern_info))
    {
        DP_ASSERT(0 && "Could not set the PHY_TEST_PATTERN");
        status = false;
    }
    else
    {
        if (AuxRetry::ack != hal->setLinkQualPatternSet(pattern_info.lqsPattern, requestedLanes))
        {
            DP_ASSERT(0 && "Could not set the LINK_QUAL_PATTERN");
            status = false;
        }
    }
    return status;
}

// return ACK/NACK result of the request
bool ConnectorImpl::handleTestLinkTrainRequest()
{
    if (activeLinkConfig.multistream)
    {
        hal->setTestResponse(false);
        return false;
    }
    else
    {
        LinkRate    requestedRate;
        unsigned    requestedLanes;

        hal->getTestRequestTraining(requestedRate, requestedLanes);
        // if one of them is illegal; don't ack. let the box try again.
        if (requestedRate == 0 || requestedLanes == 0)
        {
            DP_ASSERT(0 && "illegal requestedRate/Lane, retry..");
            hal->setTestResponse(false);
            return false;
        }
        else
        {
            // Compliance shouldn't ask us to train above its caps
            if (requestedRate == 0 || requestedRate > hal->getMaxLinkRate())
            {
                DP_ASSERT(0 && "illegal requestedRate");
                requestedRate = hal->getMaxLinkRate();
            }

            if (requestedLanes == 0 || requestedLanes > hal->getMaxLaneCount())
            {
                DP_ASSERT(0 && "illegal requestedLanes");
                requestedLanes = hal->getMaxLaneCount();
            }

            DeviceImpl * dev = findDeviceInList(Address());
            if (!dev || !dev->plugged || dev->multistream)
            {
                hal->setTestResponse(false);
                return false;
            }
            else
            {
                GroupImpl * groupAttached = this->getActiveGroupForSST();
                DP_ASSERT(groupAttached && groupAttached->isHeadAttached());

                if (!dev->activeGroup || (dev->activeGroup != groupAttached))
                {
                    DP_ASSERT(0 && "Compliance: no group attached");
                }

                DP_PRINTF(DP_NOTICE, "DP> Compliance: LT on IRQ request: 0x%x, %d.", requestedRate, requestedLanes);
                // now see whether the current resolution is supported on the requested link config
                LinkConfiguration lc(&linkPolicy, requestedLanes, requestedRate,
                                     hal->getEnhancedFraming(),
                                     false, /* MST */
                                     false, /* disablePostLTRequest */
                                     false, /* bEnableFEC */
                                     false, /* bDisableLTTPR */
                                     this->getDownspreadDisabled());    // DisableDownspread

                if (groupAttached && groupAttached->isHeadAttached())
                {
                    if (willLinkSupportModeSST(lc, groupAttached->lastModesetInfo))
                    {
                        DP_PRINTF(DP_NOTICE, "DP> Compliance: Executing LT on IRQ: 0x%x, %d.", requestedRate, requestedLanes);
                        // we need to force the requirement irrespective of whether is supported or not.
                        if (!enableFlush())
                        {
                            hal->setTestResponse(false);
                            return false;
                        }
                        else
                        {
                            //
                            // Check if linkTraining fails, perform fake linktraining. This is required because
                            // if we simply fail linkTraining we will not configure the head which results in
                            // TDRs if any modset happens after this.
                            //
                            hal->setTestResponse(true);
                            if (!train(lc, false))
                                train(lc, true);
                            disableFlush();
                            // Don't force/commit. Only keep the request.
                            setPreferredLinkConfig(lc, false, false);
                            return true;
                        }
                    }
                    else // linkconfig is not supporting bandwidth. Fallback to default edid and notify DD.
                    {
                        // override the device with fallback edid and notify a bw change to DD.
                        DP_PRINTF(DP_NOTICE, "DP> Compliance: Switching to compliance fallback EDID after IMP failure.");
                        dev->switchToComplianceFallback();

                        DP_PRINTF(DP_NOTICE, "DP> Compliance: Notifying bandwidth change to DD after IMP failure.");
                        // notify a bandwidth change to DD
                        sink->bandwidthChangeNotification(dev, true);

                        return false;
                    }
                }
                else
                {
                    DP_PRINTF(DP_NOTICE, "DP> Compliance: Link Training when the head is not attached.");
                    hal->setTestResponse(true);
                    if (!train(lc, false))
                        train(lc, true);

                    setPreferredLinkConfig(lc, false, false);
                    return true;
                }
            }
        }
    }
}

//
// This function is used to send dp test message.
// requestSize indicates the buffer size pointed by pBuffer
//
DP_TESTMESSAGE_STATUS ConnectorImpl::sendDPTestMessage
(
    void    *pBuffer,
    NvU32    requestSize,
    NvU32   *pDpStatus
)
{
    if (messageManager)
    {
        testMessage.setupTestMessage(messageManager, this);
        return testMessage.sendDPTestMessage(pBuffer, requestSize, pDpStatus);
    }
    else
    {
        return DP_TESTMESSAGE_STATUS_ERROR;
    }
}

//
// This function is designed for user to call twcie. The first time with NULL of
// pStreamIDs to get the number of streams.
// The second time, user would call the function with allocated buffer.
//
DP_TESTMESSAGE_STATUS ConnectorImpl::getStreamIDs(NvU32 *pStreamIDs, NvU32 *pCount)
{
    DP_TESTMESSAGE_STATUS ret;

    NvU32 streamCnt = activeGroups.size();
    if (NULL == pStreamIDs)
    {
        ret = DP_TESTMESSAGE_STATUS_SUCCESS;
    }
    else if (*pCount >= streamCnt)
    {
        NvU32 n = 0;
        for (ListElement * i = activeGroups.begin(); i != activeGroups.end(); i = i->next)
        {
            GroupImpl * group = (GroupImpl *)i;
            pStreamIDs[n++] = group->streamIndex;
        }
        ret = DP_TESTMESSAGE_STATUS_SUCCESS;
    }
    else
    {
        //buffer size not enough, the return value will be mapped and returned to nvapi
        ret = DP_TESTMESSAGE_STATUS_ERROR_INSUFFICIENT_INPUT_BUFFER;
    }

    *pCount = streamCnt;

    return ret;
}

void ConnectorImpl::notifyGPUCapabilityChange()
{
    // Query current GPU capabilities.
    main->queryGPUCapability();
}

void ConnectorImpl::notifyHBR2WAREngage()
{
    bool peakBwChanged = false;
    LinkConfiguration preLc = getMaxLinkConfig();
    // Update GPU capabilities
    this->notifyGPUCapabilityChange();
    LinkConfiguration postLc = getMaxLinkConfig();

    peakBwChanged = (preLc.peakRatePossible != postLc.peakRatePossible);

    if (this->previousPlugged && peakBwChanged)
    {
        // Set caps change status to make sure device becomes zombie
        this->bMitigateZombie = true;

        if (this->policyModesetOrderMitigation)
        {
            this->modesetOrderMitigation = true;
        }
        // NEED TO CHECK. MAY GO AFTER LONGPULSE TRUE ????
        // If multistream, delete the MST slots allocation in Branch device
        if (this->linkUseMultistream())
            this->deleteAllVirtualChannels();

        // Disconnect the device
        this->notifyLongPulse(false);

        // Connect the device again
        this->notifyLongPulse(true);
    }

}

bool ConnectorImpl::isLinkAwaitingTransition()
{
    return this->linkAwaitingTransition;
}

void ConnectorImpl::configInit()
{
    // Reset branch specific flags
    bKeepOptLinkAlive = 0;
    bNoFallbackInPostLQA = 0;
    LT2FecLatencyMs = 0;
    bDscCapBasedOnParent = false;
    bForceClearPendingMsg = false;
    allocatedDpTunnelBw = 0;
    allocatedDpTunnelBwShadow = 0;
    bForceHeadShutdownPerMonitor = false;
    bDP2XPreferNonDSCForLowPClk = false;
    bDisableDscMaxBppLimit = false;
    bForceHeadShutdownOnModeTransition = false;
    bSkipResetMSTMBeforeLt = false;
}

bool ConnectorImpl::dpUpdateDscStream(Group *target, NvU32 dscBpp)
{
    // TODO : Implement logic
    return true;
}

