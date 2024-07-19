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
* Module: dp_connectorimpl.cpp                                              *
*    DP connector implementation                                            *
*                                                                           *
\***************************************************************************/
#ifndef INCLUDED_DP_CONNECTORIMPL_H
#define INCLUDED_DP_CONNECTORIMPL_H

#include "dp_internal.h"
#include "dp_guid.h"
#include "dp_connector.h"
#include "dp_configcaps.h"
#include "dp_list.h"
#include "dp_buffer.h"
#include "dp_auxdefs.h"
#include "dp_watermark.h"
#include "dp_edid.h"
#include "dp_discovery.h"
#include "dp_groupimpl.h"
#include "dp_deviceimpl.h"
#include "./dptestutil/dp_testmessage.h"

// HDCP abort codes
#define    HDCP_FLAGS_ABORT_DEVICE_REVOKED     0x00000800 // Abort due to a revoked device in DP1.2 topology
#define    HDCP_FLAGS_ABORT_DEVICE_INVALID     0x00080000 // Abort due to an invalid device in DP1.2 topology
#define    HDCP_FLAGS_ABORT_HOP_LIMIT_EXCEEDED 0x80000000 // Abort, number of devices in DP1.2 topology exceeds supported limit

#define    DP_TUNNEL_REQUEST_BW_MAX_TIME_MS          (1000U)
#define    DP_TUNNEL_REQUEST_BW_POLLING_INTERVAL_MS    (10U)

static inline unsigned getDataClockMultiplier(NvU64 linkRate, NvU64 laneCount)
{
    //
    // To get the clock multiplier:
    // - Convert the linkRate from Hz to 10kHz by dividing it by 10000.
    // - Multiply the 10kHz linkRate by the laneCount.
    // - Multiply by 10.0/8, to account for the 8b/10b encoding overhead in the DP protocol layer.
    //
    // Avoid floating point in the arithmetic in the calculation
    // through the following conversions:
    //   linkRate/10000.0 * laneCount * 10.0/8
    //   (linkRate * laneCount * 10) / (10000 * 8)
    //   (linkRate * laneCount) / (1000 * 8)
    //
    return (unsigned) DisplayPort::axb_div_c_64(linkRate, laneCount, 8000);
}

namespace DisplayPort
{

    typedef enum
    {
        DP_TRANSPORT_MODE_INIT          = 0,
        DP_TRANSPORT_MODE_SINGLE_STREAM = 1,
        DP_TRANSPORT_MODE_MULTI_STREAM  = 2,
    } DP_TRANSPORT_MODE;

    // Information required during compound query attach for MST
    typedef struct _CompoundQueryAttachMSTInfo
    {
        ModesetInfo         localModesetInfo;
        LinkConfiguration   lc;
    } CompoundQueryAttachMSTInfo;

    struct ConnectorImpl : public Connector, DiscoveryManager::DiscoveryManagerEventSink, Timer::TimerCallback, MessageManager::MessageReceiver::MessageReceiverEventSink
    {
        // DPCD HAL Layer - We should use this in place of direct register accesses
        DPCDHAL * hal;

        MainLink * main;                        // Main link controls
        AuxBus * auxBus;

        TestMessage testMessage;                // TestMessage instance

        Timer * timer;                          // OS provided timer services
        Connector::EventSink * sink;            // Event Sink

        // Cached Source OUI for restoring eDP OUI when powering up
        unsigned cachedSourceOUI;
        char     cachedSourceModelName[NV_DPCD_SOURCE_DEV_ID_STRING__SIZE + 1];
        NvU8     cachedSourceChipRevision;
        bool     bOuiCached;

        unsigned ouiId;                                                   // Sink ouiId
        unsigned char modelName[NV_DPCD_SOURCE_DEV_ID_STRING__SIZE + 1];  // Device Model-name
        bool    bIgnoreSrcOuiHandshake;                                   // Skip writing source OUI

        LinkPolicy    linkPolicy;

        bool    linkGuessed;                    // True when link was "guessed" during HPD in TMDS mode
        bool    isLinkQuiesced;                 // True when link was set to quiet mode by TMDS modeset

        bool    bNoLtDoneAfterHeadDetach;       // True when head is disconnected in NDE

        bool    isDP12AuthCap;                  // To tell whether this DP1.2 connector/ upmost device has the authentication Cap.
        bool    isHDCPAuthOn;                   // To tell whether this connector has the authentication on.
        bool    isHDCPReAuthPending;            // To tell whether HDCP Auth is pending (at every stream addition and cleared at handler).
        bool    isHDCPAuthTriggered;            // To tell whether HDCP Auth is triggered and only cleared at unplug/device detach for MST.
        bool    isHopLimitExceeded;             // To tell the current topology is over limitation.
        bool    bIsDiscoveryDetectActive;       // To tell device discovery is active ( isDiscoveryDetectComplete is also used as DD notify and not want to impacts that. )
        bool    isDiscoveryDetectComplete;      // To tell device discovery is finished.
        bool    bDeferNotifyLostDevice;         // To tell if we should defer notify lost device event to client.

        HDCPValidateData hdcpValidateData;      // Cache the HDCP ValidateData.
        unsigned authRetries;                   // Retry counter for the authentication.
        unsigned retryLT;                       // Retry counter for link training in case of link lost in PostLQA
        unsigned hdcpCapsRetries;               // Retry counter for Hdcp Caps read.
        unsigned hdcpCpIrqRxStatusRetries;      // Retry counter for CPIRQ RxStatus read.
        bool    bLTPhyRepeater;                 // Link Train PHY Repeaters between Source and Sink
        bool    bFromResumeToNAB;               // True if from resume to NAB, WAR flag for unblocking GA1.5
        bool    bAttachOnResume;                // True if notifyLongPulse is called for resume (reboot/S3/S4)
        bool    bSkipAssessLinkForEDP;          // Skip assessLink() for eDP. Assuming max is reachable.
        bool    bPConConnected;                 // HDMI2.1-Protocol Converter (Support SRC control mode) connected.
        bool    bSkipAssessLinkForPCon;         // Skip assessLink() for PCON. DD will call assessFRLLink later.
        bool    bHdcpAuthOnlyOnDemand;          // True if only initiate Hdcp authentication on demand and MST won't auto-trigger authenticate at device attach.
        bool    bReassessMaxLink;               // Retry assessLink() if the first assessed link config is lower than the panel max config.

        bool    constructorFailed;

        //
        //  OS Modeset Order mitigation causes the library to delay the reporting
        //   of new devices until they can be safely turned on.
        //  When enabled the library client will not see connection events until
        //    MustDisconnect messages are processed.
        //
        //   Policy state should be set before the library is brought out of
        //   the suspended state.
        //
        bool policyModesetOrderMitigation;

        //
        //  force LT at NAB for compliance test (Power Management) in Win10 RS2+ (WDDM 2.2)
        //
        //  RS2 no longer sends an explicit call for setPanelPowerParams during the Resume.
        //  It does that by specifying an additional flag during the call to SetTimings. Due to
        //  this DP lib doesn't get chance to perform this transition from setPanelPowerParams
        //  and since it was already skipping LT in NAB/modeswitch, so LT get missed out on the
        //  compliance device during resume from S3/S4.
        //
        bool policyForceLTAtNAB;

        //
        //  There are cases where OS does not detach heads from connector immediately after hot-unplug,
        //  on next hot-plug there is no guarantee that newly connected sink is capable to drive existing
        //  raster timings. Flush mode has following restriction
        //      When exiting flush mode S/W should ensure that the final
        //      link clock & lane count should be able to support existing raster.
        //  If we run into this situation and use flush mode then that will cause display engine to hang.
        //  This variable ensures to assess link safely in this situation: if newly connected sink is
        //  not capable to drive existing raster then just restore link configuration which was there
        //  before enabling flush mode, through fake link training.
        //
        bool policyAssessLinkSafely;

        bool bDisableVbiosScratchRegisterUpdate;

        // Only works when policyModesetOrderMitigation is true.
        // To record if we should report newDevice.
        bool modesetOrderMitigation;

        List deviceList;
        List activeGroups;
        LinkedList<GroupImpl> intransitionGroups;
        LinkedList<GroupImpl> addStreamMSTIntransitionGroups;
        List inactiveGroups;

        LinkedList<Device> dscEnabledDevices;

        // Compound query
        bool compoundQueryActive;
        bool compoundQueryResult;
        unsigned compoundQueryCount;
        unsigned compoundQueryLocalLinkPBN;
        NvU64 compoundQueryUsedTunnelingBw;
        bool compoundQueryForceEnableFEC;

        unsigned freeSlots;
        unsigned maximumSlots;
        int firstFreeSlot;

        // Multistream messaging
        MessageManager *    messageManager;
        DiscoveryManager *  discoveryManager;

        // Multistream timeslot management (on local link)
        LinkConfiguration highestAssessedLC;    // As of last assess, the highest possible link configuration

        LinkConfiguration activeLinkConfig;     // Current link config.

        // this is the link config requested by a client.
        // can be set and reset by the client for a given operation.
        LinkConfiguration preferredLinkConfig;
        bool forcePreferredLinkConfig;

        //
        // Desired link configuration of single head multiple sst secondary connector.
        //
        LinkConfiguration oneHeadSSTSecPrefLnkCfg;

        // All possible link configs
        LinkConfiguration * allPossibleLinkCfgs;
        unsigned numPossibleLnkCfg;

        PCONLinkControl activePConLinkControl;

        //
        // We're waiting for an MST<->SST transition
        // The transition cannot be made without the DD
        // disconnecting all heads.  All devices are reported
        // as must_disconnect.  Once the last device blocking
        // the transition is deattached from a head - we transition.
        //
        bool              linkAwaitingTransition;

        // Unless we're awaiting transition this is identical to hal->getSupportsMultistream()
        DP_TRANSPORT_MODE linkState;

        bool              bAudioOverRightPanel;

        bool previousPlugged;
        bool connectorActive;                    // Keep track of if connector is active to serve any IRQ

        Group           * firmwareGroup;         // The group used for book-keeping when we're in firmware mode

        List pendingEdidReads;                   // List of DevicePendingEDIDRead structures.
                                                 // This list tracks the currently in progress MST Edid Reads

        Device          * lastDeviceSetForVbios;

        // Flag which gets set when ACPI init is done. DD calls notifyAcpiInitDone to tell client that ACPI init is completed
        // & client can now initiate DDC EDID read for a device which supports EDID through SBIOS
        bool        bAcpiInitDone;

        // Flag to check if the system is UEFI.
        bool        bIsUefiSystem;

        // Flag to check if LT should be skipped.
        bool        bSkipLt;

        // Flag to make sure that zombie gets triggred when a powerChange event happens
        bool bMitigateZombie;

        //
        // HP Valor QHD+ N15P-Q3 EDP needs 50ms delay after D3
        // during trainLinkOptimized to come up on S4
        //
        bool        bDelayAfterD3;

        //
        // ASUS and Samsung monitors have inconsistent behavior when
        // DPCD 0x600 updated to D3. Skip D3 only in case these monitors
        // are driven in SST config
        //
        bool        bKeepLinkAlive;

        //
        // HP Trump dock link training is unstable during S4 resume, which causes
        // system to hang. Keep the link alive to increase stability.
        // See Bug 2109823.
        //
        bool        bKeepLinkAliveMST;

        // Keep the link alive when connector is in SST
        bool        bKeepLinkAliveSST;

        //
        // HTC Vive Link box is not happy when we power down the link
        // during link training when there is no stream present. It requests
        // for a link retraining pulse which is not required.
        // WAR to address this - NV Bug# 1793084
        //
        bool        bKeepOptLinkAlive;

        // Keep both DP and FRL link alive to save time.
        bool        bKeepLinkAliveForPCON;

        //
        // Remote HDCP DCPD access should be D0 but won't introduce extra Dx
        // state toggle. Use the counter to avoid powerdownlink when HDCP probe.
        //
        unsigned   pendingRemoteHdcpDetections;

        //
        // ASUS PQ 321 tiled monitor sometimes loses link while assessing link
        // or link training .So if we lower config from HBR2 to HBR and when
        // we retrain the link , we see black screen.
        // So WAR is to retry link training with same config for 3 times before
        // lowering link config. NV Bug #1846925
        //
        bool        bNoFallbackInPostLQA;

        bool        bReportDeviceLostBeforeNew;
        bool        bDisableSSC;
        bool        bEnableFastLT;
        NvU32       maxLinkRateFromRegkey;
        bool        bFlushTimeslotWhenDirty;

        //
        // Latency(ms) to apply between link-train and FEC enable for bug
        // 2561206.
        //
        NvU32       LT2FecLatencyMs;

        //
        // Dual SST Partner connector object pointer
        ConnectorImpl *pCoupledConnector;

        // Set to true when a DSC mode is requested.
        bool bFECEnable;

        // Save link config before entering PSR.
        LinkConfiguration psrLinkConfig;

        //
        // Apply MST DSC caps WAR based on OUI ID of sink
        //
        bool        bDscMstCapBug3143315;

        //
        // Synaptics branch device doesn't support Virtual Peer Devices so DSC
        // capability of downstream device should be decided based on device's own
        // and its parent's DSC capability
        //
        bool        bDscCapBasedOnParent;

        //
        // MST device connnected to dock may issue IRQ for link lost.
        // Send PowerDown path msg to suppress that.
        //
        bool        bPowerDownPhyBeforeD3;

        //
        // Reset the MSTM_CTRL registers on branch device irrespective of
        // IRQ VECTOR register having stale message. Certain branch devices
        // need to reset the topology before issuing new discovery commands
        // as there can be case where previous is still in process and a
        // possibility that clearPendingMessage() might not be able to catch
        // the stale messages from previous discovery.
        //
        bool        bForceClearPendingMsg;
        NvU64       allocatedDpTunnelBw;
        NvU64       allocatedDpTunnelBwShadow;
        bool        bForceDisableTunnelBwAllocation;

        Group *perHeadAttachedGroup[NV_MAX_HEADS];
        NvU32 inTransitionHeadMask;

        void sharedInit();
        ConnectorImpl(MainLink * main, AuxBus * auxBus, Timer * timer, Connector::EventSink * sink);
        void setPolicyModesetOrderMitigation(bool enabled);
        void setPolicyForceLTAtNAB(bool enabled);
        void setPolicyAssessLinkSafely(bool enabled);

        void discoveryDetectComplete();
        void discoveryNewDevice(const DiscoveryManager::Device &device);
        void discoveryLostDevice(const Address &address);
        void processNewDevice(const DiscoveryManager::Device &device,
            const Edid &edid,
            bool isMultistream,
            DwnStreamPortType portType,
            DwnStreamPortAttribute portAttribute,
            bool isCompliance = false);

        void applyEdidWARs(Edid &edid, DiscoveryManager::Device device);
        void applyRegkeyOverrides(const DP_REGKEY_DATABASE& dpRegkeyDatabase);

        ResStatusNotifyMessage ResStatus;

        void messageProcessed(MessageManager::MessageReceiver * from);

        ~ConnectorImpl();

        //
        //  Utility functions
        //
        virtual void hardwareWasReset();
        virtual LinkConfiguration getMaxLinkConfig();
        virtual LinkConfiguration getActiveLinkConfig();
        virtual void powerdownLink(bool bPowerdownPanel = false);

        GroupImpl * getActiveGroupForSST();
        bool detectSinkCountChange();
        bool handlePhyPatternRequest();
        void applyOuiWARs();
        bool linkUseMultistream()
        {
            return (linkState == DP_TRANSPORT_MODE_MULTI_STREAM);
        }

        void populateAllDpConfigs();
        virtual LinkRates* importDpLinkRates();

        //
        //  Suspend resume API
        //
        virtual Group * resume(bool firmwareLinkHandsOff,
                               bool firmwareDPActive,
                               bool plugged,
                               bool isUefiSystem = false,
                               unsigned firmwareHead = 0,
                               bool bFirmwareLinkUseMultistream = false,
                               bool bDisableVbiosScratchRegisterUpdate = false,
                               bool bAllowMST = true);
        virtual void    pause();

        virtual Device * enumDevices(Device * previousDevice) ;


        virtual void beginCompoundQuery(const bool bForceEnableFEC = false) ;
        virtual bool compoundQueryAttach(Group * target,
            unsigned twoChannelAudioHz,         // if you need 192khz stereo specify 192000 here
            unsigned eightChannelAudioHz,       // Same setting for multi channel audio.
                                                //  DisplayPort encodes 3-8 channel streams as 8 channel
            NvU64 pixelClockHz,                 // Requested pixel clock for the mode
            unsigned rasterWidth,
            unsigned rasterHeight,
            unsigned rasterBlankStartX,
            unsigned rasterBlankEndX,
            unsigned depth,
            DP_IMP_ERROR *errorStatus = NULL);

        virtual bool compoundQueryAttach(Group * target,
                                         const DpModesetParams &modesetParams,      // Modeset info
                                         DscParams *pDscParams = NULL,              // DSC parameters
                                         DP_IMP_ERROR *pErrorCode = NULL);          // Error Status code
        virtual bool compoundQueryAttachTunneling(const DpModesetParams &modesetParams,
                                                  DscParams *pDscParams = NULL,
                                                  DP_IMP_ERROR *pErrorCode = NULL);

        virtual bool endCompoundQuery();

        virtual bool dpLinkIsModePossible(const DpLinkIsModePossibleParams &params);

        virtual bool compoundQueryAttachMST(Group * target,
                                            const DpModesetParams &modesetParams,      // Modeset info
                                            DscParams *pDscParams = NULL,              // DSC parameters
                                            DP_IMP_ERROR *pErrorCode = NULL);          // Error Status code

        virtual bool compoundQueryAttachMSTIsDscPossible
        (
            Group * target,
            const DpModesetParams &modesetParams,      // Modeset info
            DscParams *pDscParams = NULL               // DSC parameters
        );

        // Calculate and Configure SW state based on DSC
        virtual bool compoundQueryAttachMSTDsc
        (
            Group * target,
            const DpModesetParams &modesetParams,      // Modeset info
            CompoundQueryAttachMSTInfo * info,         // local info to update for later use
            DscParams *pDscParams = NULL,              // DSC parameters
            DP_IMP_ERROR *pErrorCode = NULL            // Error Status code
        );

        // General part of CQA MST for DSC/non-DSC
        virtual bool compoundQueryAttachMSTGeneric
        (
            Group * target,
            const DpModesetParams &modesetParams,       // Modeset info
            CompoundQueryAttachMSTInfo * info,          // local info with updates for DSC
            DscParams *pDscParams = NULL,               // DSC parameters
            DP_IMP_ERROR *pErrorCode = NULL             // Error Status code
        );

        virtual bool compoundQueryAttachSST(Group * target,
                                            const DpModesetParams &modesetParams,      // Modeset info
                                            DscParams *pDscParams = NULL,              // DSC parameters
                                            DP_IMP_ERROR *pErrorCode = NULL);          // Error Status code


        //
        //  Timer callback tags.
        //   (we pass the address of these variables as context to ::expired)
        char tagFireEvents;
        char tagDelayedLinkTrain;
        char tagHDCPReauthentication;
        char tagDelayedHdcpCapRead;
        char tagDelayedHDCPCPIrqHandling;
        char tagDpBwAllocationChanged;

        //
        //  Enable disable TMDS mode
        //
        virtual void enableLinkHandsOff();
        virtual void releaseLinkHandsOff();

        //
        //  Timer callback for event management
        //      Uses: fireEvents()
        virtual void expired(const void * tag);

        // Generate Events.
        //      useTimer specifies whether we fire the events on the timer
        //      context, or this context.
        void fireEvents();

        // returns the number of pending notifications.
        void fireEventsInternal();

        virtual bool isHeadShutDownNeeded(Group * target,   // Group of panels we're attaching to this head
            unsigned headIndex,
            ModesetInfo modesetInfo);

        virtual bool isLinkTrainingNeededForModeset(ModesetInfo modesetInfo);

        virtual bool notifyAttachBegin(Group * target,      // Group of panels we're attaching to this head
            const DpModesetParams &modesetParams);

        bool needToEnableFEC(const DpPreModesetParams &params);

        virtual void dpPreModeset(const DpPreModesetParams &modesetParams);
        virtual void dpPostModeset(void);

        virtual bool isHeadShutDownNeeded(Group * target,   // Group of panels we're attaching to this head
            unsigned headIndex,
            unsigned twoChannelAudioHz,         // if you need 192khz stereo specify 192000 here
            unsigned eightChannelAudioHz,       // Same setting for multi channel audio. DisplayPort encodes 3-8 channel streams as 8 channel
            NvU64 pixelClockHz,                 // Requested pixel clock for the mode
            unsigned rasterWidth,
            unsigned rasterHeight,
            unsigned rasterBlankStartX,
            unsigned rasterBlankEndX,
            unsigned depth) ;

        virtual bool notifyAttachBegin(Group * target,      // Group of panels we're attaching to this head
            unsigned headIndex,
            unsigned twoChannelAudioHz,                     // if you need 192khz stereo specify 192000 here
            unsigned eightChannelAudioHz,                   // Same setting for multi channel audio.
                                                            // DisplayPort encodes 3-8 channel streams as 8 channel
            NvU64 pixelClockHz,                             // Requested pixel clock for the mode
            unsigned rasterWidth,
            unsigned rasterHeight,
            unsigned rasterBlankStartX,
            unsigned rasterBlankEndX,
            unsigned depth) ;

        virtual void readRemoteHdcpCaps();
        virtual void notifyAttachEnd(bool modesetCancelled);
        virtual void notifyDetachBegin(Group * target);
        virtual void notifyDetachEnd(bool bKeepOdAlive = false);
        virtual bool willLinkSupportModeSST(const LinkConfiguration &linkConfig, const ModesetInfo &modesetInfo);

        bool performIeeeOuiHandshake();
        void setIgnoreSourceOuiHandshake(bool bIgnore);
        bool getIgnoreSourceOuiHandshake();
        void forceLinkTraining();

        bool     updateDpTunnelBwAllocation();
        TriState requestDpTunnelBw(NvU8 requestedBw);
        bool     allocateDpTunnelBw(NvU64 bandwidth);
        bool     allocateMaxDpTunnelBw();
        NvU64    getMaxTunnelBw();
        void     enableDpTunnelingBwAllocationSupport();

        void assessLink(LinkTrainingType trainType = NORMAL_LINK_TRAINING);

        bool isLinkInD3();
        bool isLinkActive();
        bool isLinkLost();
        bool trainSingleHeadMultipleSSTLinkNotAlive(GroupImpl *pGroupAttached);
        bool isLinkAwaitingTransition();
        bool isNoActiveStreamAndPowerdown();
        void incPendingRemoteHdcpDetection()
        {
            pendingRemoteHdcpDetections++;
        }
        void decPendingRemoteHdcpDetection()
        {
            if (pendingRemoteHdcpDetections > 0)
            {
                pendingRemoteHdcpDetections--;
            }
        }
        bool trainLinkOptimized(LinkConfiguration lConfig);
        bool trainLinkOptimizedSingleHeadMultipleSST(GroupImpl * group);
        bool getValidLowestLinkConfig(LinkConfiguration &lConfig, LinkConfiguration &lowestSelected, ModesetInfo queryModesetInfo);
        bool postLTAdjustment(const LinkConfiguration &, bool force);
        void populateUpdatedLaneSettings(NvU8* voltageSwingLane, NvU8* preemphasisLane, NvU32 *data);
        void populateDscCaps(DSC_INFO* dscInfo, DeviceImpl * dev, DSC_INFO::FORCED_DSC_PARAMS* forcedParams);
        void populateDscGpuCaps(DSC_INFO* dscInfo);
        void populateForcedDscParams(DSC_INFO* dscInfo, DSC_INFO::FORCED_DSC_PARAMS* forcedParams);
        void populateDscSinkCaps(DSC_INFO* dscInfo, DeviceImpl * dev);
        void populateDscBranchCaps(DSC_INFO* dscInfo, DeviceImpl * dev);
        void populateDscModesetInfo(MODESET_INFO * pModesetInfo, const DpModesetParams * pModesetParams);

        virtual bool train(const LinkConfiguration &lConfig, bool force, LinkTrainingType trainType = NORMAL_LINK_TRAINING);
        virtual bool validateLinkConfiguration(const LinkConfiguration &lConfig);

        virtual bool assessPCONLinkCapability(PCONLinkControl *params);
        bool trainPCONFrlLink(PCONLinkControl *pConControl);

        // Set Device DSC state based on current DSC state of all active devices on this connector
        bool setDeviceDscState(Device * dev, bool bEnableDsc);

        // the lowest level function(nearest to the hal) for the connector.
        bool rawTrain(const LinkConfiguration &lConfig, bool force, LinkTrainingType linkTrainingType);

        virtual bool enableFlush();
        virtual bool beforeAddStream(GroupImpl * group, bool force=false, bool forFlushMode = false);
        virtual void afterAddStream(GroupImpl * group);
        virtual void beforeDeleteStream(GroupImpl * group, bool forFlushMode = false);
        virtual void afterDeleteStream(GroupImpl * group);
        virtual void disableFlush(bool test=false);

        bool beforeAddStreamMST(GroupImpl * group, bool force = false, bool forFlushMode = false);

        virtual bool checkIsModePossibleMST(GroupImpl * group);

        bool deleteAllVirtualChannels();
        void clearTimeslices();
        virtual bool allocateTimeslice(GroupImpl * targetGroup);
        void freeTimeslice(GroupImpl * targetGroup);
        void flushTimeslotsToHardware();
        bool getHDCPAbortCodesDP12(NvU32 &hdcpAbortCodesDP12);
        bool getOuiSink(unsigned &ouiId, unsigned char * modelName, size_t modelNameBufferSize, NvU8 &chipRevision);
        bool hdcpValidateKsv(const NvU8 *ksv, NvU32 Size);
        void cancelHdcpCallbacks();
        bool handleCPIRQ();
        void handleSSC();
        void handleMCCSIRQ();
        void handleDpTunnelingIrq();
        void handleHdmiLinkStatusChanged();
        void sortActiveGroups(bool ascending);
        void configInit();
        void handlePanelReplayError();

        virtual DeviceImpl* findDeviceInList(const Address &address);
        virtual void disconnectDeviceList();
        void notifyLongPulseInternal(bool statusConnected);
        virtual void notifyLongPulse(bool status);
        virtual void notifyShortPulse();
        virtual Group * newGroup();
        virtual void destroy();
        virtual void createFakeMuxDevice(const NvU8 *buffer, NvU32 bufferSize);
        virtual void deleteFakeMuxDevice();
        virtual bool getRawDscCaps(NvU8 *buffer, NvU32 bufferSize);
        virtual bool isMultiStreamCapable();
        virtual bool isFlushSupported();
        virtual bool isStreamCloningEnabled();
        virtual bool isFECSupported();
        virtual bool isFECCapable();
        virtual NvU32 maxLinkRateSupported();
        bool setPreferredLinkConfig(LinkConfiguration &lc, bool commit, 
                                    bool force = false,
                                    LinkTrainingType trainType = NORMAL_LINK_TRAINING,
                                    bool forcePreferredLinkConfig = false);
        virtual bool resetPreferredLinkConfig(bool force = false);
        virtual void setAllowMultiStreaming(bool bAllowMST);
        virtual bool getAllowMultiStreaming(void);
        virtual bool getSinkMultiStreamCap(void);
        virtual void setDp11ProtocolForced();
        virtual void resetDp11ProtocolForced();
        virtual bool isDp11ProtocolForced();

        bool isAcpiInitDone();
        virtual void notifyAcpiInitDone();
        Group * createFirmwareGroup();
        virtual void notifyGPUCapabilityChange();
        virtual void notifyHBR2WAREngage();
        bool dpUpdateDscStream(Group *target, NvU32 dscBpp);

        bool getTestPattern(NV0073_CTRL_DP_TESTPATTERN *testPattern);
        bool setTestPattern(NV0073_CTRL_DP_TESTPATTERN testPattern, NvU8 laneMask, NV0073_CTRL_DP_CSTM cstm, NvBool bIsHBR2, NvBool bSkipLaneDataOverride = false);
        bool getLaneConfig(NvU32 *numLanes, NvU32 *data);    // "data" is an array of NV0073_CTRL_MAX_LANES unsigned ints
        bool setLaneConfig(NvU32 numLanes, NvU32 *data);    // "data" is an array of NV0073_CTRL_MAX_LANES unsigned ints
        void getCurrentLinkConfig(unsigned &laneCount, NvU64 &linkRate);  // CurrentLink Configuration
        unsigned getPanelDataClockMultiplier();
        unsigned getGpuDataClockMultiplier();
        void configurePowerState(bool bPowerUp);
        virtual void readPsrCapabilities(vesaPsrSinkCaps *caps);
        virtual bool updatePsrConfiguration(vesaPsrConfig config);
        virtual bool readPsrConfiguration(vesaPsrConfig *config);
        virtual bool readPsrDebugInfo(vesaPsrDebugStatus *psrDbgState);
        virtual bool writePsrErrorStatus(vesaPsrErrorStatus psrErr);
        virtual bool readPsrErrorStatus(vesaPsrErrorStatus *psrErr);
        virtual bool writePsrEvtIndicator(vesaPsrEventIndicator psrErr);
        virtual bool readPsrEvtIndicator(vesaPsrEventIndicator *psrErr);
        virtual bool readPsrState(vesaPsrState *psrState);
        virtual bool updatePsrLinkState(bool bTurnOnLink);

        virtual bool readPrSinkDebugInfo(panelReplaySinkDebugInfo *prDbgInfo);

        // for dp test utility. pBuffer is the request buffer of type DP_STATUS_REQUEST_xxxx
        DP_TESTMESSAGE_STATUS sendDPTestMessage(void *pBuffer,
                                                NvU32 requestSize,
                                                NvU32 *pDpStatus);

        DP_TESTMESSAGE_STATUS getStreamIDs(NvU32 *pStreamIDs, NvU32 *pCount);  // for dp test utility, called by DD

        // Reset link training counter for the active link configuration.
        virtual void resetLinkTrainingCounter()
        {
            activeLinkConfig.setLTCounter(0);
        }
    };

    //
    //  New devices do not get a DeviceImpl created until after
    //   the EDID read has completed.  This object is used
    //   to track the necessary state.
    //
    struct DevicePendingEDIDRead : protected EdidReadMultistream::EdidReadMultistreamEventSink, public ListElement
    {
        ConnectorImpl *          parent;
        DiscoveryManager::Device device;
        EdidReadMultistream      reader;

        void mstEdidCompleted(EdidReadMultistream * from);
        void mstEdidReadFailed(EdidReadMultistream * from);

    public:
        DevicePendingEDIDRead(ConnectorImpl *  _parent, MessageManager * manager, DiscoveryManager::Device dev)
            : parent(_parent), device(dev), reader(_parent->timer, manager, this, dev.address)
        {
        }
    };
}

#endif //INCLUDED_DP_CONNECTORIMPL_H
