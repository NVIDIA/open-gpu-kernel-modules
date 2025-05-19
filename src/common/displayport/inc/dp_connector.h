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

/******************************* DisplayPort *******************************\
*                                                                           *
* Module: dp_connector.h                                                    *
*    This is the primary client interface.                                  *
*                                                                           *
\***************************************************************************/
#ifndef INCLUDED_DP_CONNECTOR_H
#define INCLUDED_DP_CONNECTOR_H

#include "dp_auxdefs.h"
#include "dp_object.h"
#include "dp_mainlink.h"
#include "dp_auxbus.h"
#include "dp_address.h"
#include "dp_guid.h"
#include "dp_evoadapter.h"
#include "dp_auxbus.h"
#include "dp_auxretry.h"
#include "displayport.h"
#include "dp_vrr.h"
#include "../../modeset/timing/nvt_dsc_pps.h"
#include "ctrl/ctrl0073/ctrl0073dp.h"
#include "nvcfg_sdk.h"

namespace DisplayPort
{
    class EvoInterface;

#define SET_DP_IMP_ERROR(pErrorCode, errorCode) \
        if (pErrorCode && *pErrorCode == DP_IMP_ERROR_NONE) *pErrorCode = errorCode;

    typedef enum
    {
        DP_IMP_ERROR_NONE,
        DP_IMP_ERROR_ZERO_VALUE_PARAMS,
        DP_IMP_ERROR_AUDIO_BEYOND_48K,
        DP_IMP_ERROR_DSC_SYNAPTICS_COLOR_FORMAT,
        DP_IMP_ERROR_PPS_DSC_DUAL_FORCE,
        DP_IMP_ERROR_DSC_PCON_FRL_BANDWIDTH,
        DP_IMP_ERROR_DSC_PCON_HDMI2_BANDWIDTH,
        DP_IMP_ERROR_DSC_LAST_HOP_BANDWIDTH,
        DP_IMP_ERROR_INSUFFICIENT_BANDWIDTH,
        DP_IMP_ERROR_INSUFFICIENT_BANDWIDTH_DSC,
        DP_IMP_ERROR_INSUFFICIENT_BANDWIDTH_NO_DSC,
        DP_IMP_ERROR_INSUFFICIENT_DP_TUNNELING_BANDWIDTH,
        DP_IMP_ERROR_WATERMARK_BLANKING,
        DP_IMP_ERROR_PPS_COLOR_FORMAT_NOT_SUPPORTED,
        DP_IMP_ERROR_PPS_INVALID_HBLANK,
        DP_IMP_ERROR_PPS_INVALID_BPC,
        DP_IMP_ERROR_PPS_MAX_LINE_BUFFER_ERROR,
        DP_IMP_ERROR_PPS_OVERALL_THROUGHPUT_ERROR,
        DP_IMP_ERROR_PPS_DSC_SLICE_ERROR,
        DP_IMP_ERROR_PPS_PPS_SLICE_COUNT_ERROR,
        DP_IMP_ERROR_PPS_PPS_SLICE_HEIGHT_ERROR,
        DP_IMP_ERROR_PPS_PPS_SLICE_WIDTH_ERROR,
        DP_IMP_ERROR_PPS_INVALID_PEAK_THROUGHPUT,
        DP_IMP_ERROR_PPS_MIN_SLICE_COUNT_ERROR,
        DP_IMP_ERROR_PPS_GENERIC_ERROR,
    } DP_IMP_ERROR;

    typedef enum
    {
        DP_TESTMESSAGE_STATUS_SUCCESS                           = 0,
        DP_TESTMESSAGE_STATUS_ERROR                             = 0xDEADBEEF,
        DP_TESTMESSAGE_STATUS_ERROR_INSUFFICIENT_INPUT_BUFFER   = 0xDEADBEED,
        DP_TESTMESSAGE_STATUS_ERROR_INVALID_PARAM               = 0xDEADBEEC
        // new error code should be here
    } DP_TESTMESSAGE_STATUS;

    typedef enum
    {
        False = 0,
        True = 1,
        Indeterminate = 2
    } TriState;

    enum ConnectorType
    {
        connectorDisplayPort,
        connectorHDMI,
        connectorDVI,
        connectorVGA
    };

    typedef struct portMap
    {
        NvU16   validMap;       // port i is valid = bit i is high
        NvU16   inputMap;       // port i is input port = bit i is high && validMap bit i is high
        NvU16   internalMap;    // port i is internal = bit i is high && validMap bit i is high
    } PortMap;

    enum ForceDsc
    {
        DSC_DEFAULT,
        DSC_FORCE_ENABLE,
        DSC_FORCE_DISABLE
    };

    struct DpModesetParams
    {
        unsigned           headIndex;
        ModesetInfo        modesetInfo;
        DP_COLORFORMAT     colorFormat;
        NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_PARAMS msaparams;

        DpModesetParams() : headIndex(0), modesetInfo(), colorFormat(dpColorFormat_Unknown), msaparams() {}

        DpModesetParams(unsigned        newHeadIndex,
            ModesetInfo     newModesetInfo,
            DP_COLORFORMAT  newColorFormat = dpColorFormat_Unknown) :
            headIndex(newHeadIndex),
            modesetInfo(newModesetInfo),
            colorFormat(newColorFormat),
            msaparams() {}

        DpModesetParams(unsigned        newHeadIndex,
            ModesetInfo     *newModesetInfo,
            DP_COLORFORMAT  newColorFormat = dpColorFormat_Unknown) :
            headIndex(newHeadIndex),
            modesetInfo(*newModesetInfo),
            colorFormat(newColorFormat),
            msaparams() {}

    };

    struct DscOutParams
    {
        unsigned PPS[DSC_MAX_PPS_SIZE_DWORD];   // Out - PPS SDP data
    };

    struct DscParams
    {
        bool bCheckWithDsc;                        // [IN]     - Client telling DP Library to check with DSC.
        ForceDsc forceDsc;                         // [IN]     - Client telling DP Library to force enable/disable DSC
        DSC_INFO::FORCED_DSC_PARAMS* forcedParams; // [IN]     - Client telling DP Library to force certain DSC params.
        bool bEnableDsc;                           // [OUT]    - DP Library telling client that DSC is needed for this mode.
        NvU32 sliceCountMask;                      // [OUT]    - DP Library telling client what all slice counts can be used for the mode.
        unsigned bitsPerPixelX16;                  // [IN/OUT] - Bits per pixel value multiplied by 16
        DscOutParams *pDscOutParams;               // [OUT]    - DSC parameters

        DscParams() : bCheckWithDsc(false), forceDsc(DSC_DEFAULT), forcedParams(NULL), bEnableDsc(false), sliceCountMask(0), bitsPerPixelX16(0), pDscOutParams(NULL) {}
    };

    class Group;

    struct DpLinkIsModePossibleParams
    {
        struct
        {
            Group * pTarget;
            DpModesetParams *pModesetParams;
            DP_IMP_ERROR *pErrorStatus;
            DscParams *pDscParams;
        } head[NV_MAX_HEADS];
    };

    struct DpPreModesetParams
    {
        struct
        {
            Group *pTarget;
            const DpModesetParams *pModesetParams;
        } head[NV_MAX_HEADS];
        NvU32 headMask;
    };

    bool SetConfigSingleHeadMultiStreamMode(Group **targets,                         // Array of group pointers given for getting configured in single head multistream mode.
                                            NvU32 displayIDs[],                      // Array of displayIDs  given for getting  configured in single head multistream mode.
                                            NvU32 numStreams,                        // Number of streams driven out from single head.
                                            DP_SINGLE_HEAD_MULTI_STREAM_MODE mode,   // Configuration mode : SST or MST
                                            bool bSetConfig,                         // Set or clear the configuration.
                                            NvU8 vbiosPrimaryDispIdIndex = DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY, // VBIOS primary display ID index in displayIDs[] array
                                            bool bEnableAudioOverRightPanel = false); // Audio MUX config : right or left panel

    //
    //  Device object
    //      This object represents a displayport device.  Devices are not reported
    //      to clients until the EDID is already on file.
    //
    class Device : virtual public Object
    {
    public:

        virtual bool            isPlugged() = 0;
        virtual bool            isVideoSink() = 0;          // Invariant: won't change once reported
        virtual bool            isAudioSink() = 0;          // Invariant

        virtual bool            isLoop() = 0;               // the address starts and ends at th same device
        virtual bool            isRedundant() = 0;
        virtual bool            isMustDisconnect() = 0;     // Is this monitor's head being attach preventing
                                                            // us from enumerating other panels?
        virtual bool            isZombie() = 0;             // Head is attached but we're not connected
        virtual bool            isCableOk() = 0;            // cable can be not ok, whenwe saw hpd, device connected, but can't talk over aux

         virtual bool           isLogical() = 0;            // Is device connected to logical port

        virtual Address         getTopologyAddress() const = 0; // Invariant
        virtual bool            isMultistream() = 0;

        virtual ConnectorType   getConnectorType() = 0;         // Invariant

        virtual unsigned        getEDIDSize() const= 0;        // Invariant
        // Copies EDID into client buffer. Fails if the buffer is too small
        virtual bool            getEDID(char * buffer, unsigned size) const = 0;

        virtual unsigned        getRawEDIDSize() const= 0;
        // Copies RawEDID into client buffer. Fails if the buffer is too small
        virtual bool            getRawEDID(char * buffer, unsigned size) const = 0;

        virtual bool            getPCONCaps(PCONCaps *pPCONCaps) = 0;

        virtual bool            isFallbackEdid() = 0;           // is the device edid a fallback one?
        virtual GUID            getGUID() const = 0;            // Returns the GUID for the device
        virtual bool            isPowerSuspended() = 0;
        virtual bool            isActive() = 0;                 // Whether the device has a head attached to it
        virtual TriState        hdcpAvailableHop() = 0;         // Whether the device support HDCP,
                                                                // regardless of whether the path leading to it supports HDCP.
        virtual TriState        hdcpAvailable() = 0;            // Whether HDCP can be enabled.
                                                                // Note this checks that the entire path to the node support HDCP.

        virtual                 PortMap getPortMap() const = 0;

        virtual void            setPanelPowerParams(bool bSinkPowerStateD0, bool bPanelPowerStateOn) = 0;
        virtual Group         * getOwningGroup() = 0;           // Return the group this device is currently a member of

        virtual AuxBus        * getRawAuxChannel() = 0;         // No automatic retry on DEFER.  See limitations in dp_auxbus.h
        virtual AuxRetry      * getAuxChannel() = 0;            // User friendly AUX interface

        virtual Device        * getParent() = 0;
        virtual Device        * getChild(unsigned portNumber) = 0;

        virtual void dpcdOverrides() = 0;            // Apply DPCD overrides if required

        virtual bool getDpcdRevision(unsigned * major, unsigned * minor) = 0;  // get the dpcd revision (maybe cached)

        virtual bool getSDPExtnForColorimetrySupported() = 0;
        virtual bool getAsyncSDPSupported() = 0;

        virtual bool getPanelFwRevision(NvU16 *revision) = 0;

        virtual bool getIgnoreMSACap() = 0;

        virtual AuxRetry::status setIgnoreMSAEnable(bool msaTimingParamIgnoreEn) = 0;

        virtual NvBool isDSCPossible() = 0;

        virtual NvBool isDSCSupported() = 0;

        virtual NvBool isDSCDecompressionSupported() = 0;

        virtual NvBool isDSCPassThroughSupported() = 0;

        virtual DscCaps getDscCaps() = 0;

        virtual NvBool isDynamicPPSSupported() = 0;

        virtual NvBool isDynamicDscToggleSupported() = 0;

        //
        // This function returns the device itself or its parent device that is doing
        // DSC decompression for it.
        //
        virtual Device* getDevDoingDscDecompression() = 0;
        virtual void    markDeviceForDeletion() = 0;

        virtual bool getRawDscCaps(NvU8 *buffer, NvU32 bufferSize) = 0;
        virtual bool setRawDscCaps(const NvU8 *buffer, NvU32 bufferSize) = 0;

        // This interface is still nascent. Please don't use it. Read size limit is 16 bytes.
        virtual AuxBus::status getDpcdData(unsigned offset, NvU8 * buffer,
                                           unsigned sizeRequested,
                                           unsigned * sizeCompleted,
                                           unsigned * pNakReason = NULL) = 0;

        virtual AuxBus::status setDpcdData(unsigned offset, NvU8 * buffer,
                                           unsigned sizeRequested,
                                           unsigned * sizeCompleted,
                                           unsigned * pNakReason = NULL) = 0;

        virtual AuxBus::status dscCrcControl(NvBool bEnable, gpuDscCrc *gpuData, sinkDscCrc *sinkData) = 0;
        virtual AuxBus::status queryFecData(NvU8 *fecStatus, NvU16 **fecErrorCount, NvU32 flags) = 0;

        //
        // The address send here will be right shifted by the library. DD should
        // send the DDC address without the shift.
        // Parameter bForceMot in both getI2cData and setI2cData is used to forfully set
        // the MOT bit. It is needed for some special cases where the MOT bit shouldn't
        // be set but some customers need it to please their monitors.
        //
        virtual bool getI2cData(unsigned offset, NvU8 * buffer, unsigned sizeRequested, unsigned * sizeCompleted, bool bForceMot = false) = 0;
        virtual bool setI2cData(unsigned offset, NvU8 * buffer, unsigned sizeRequested, unsigned * sizeCompleted, bool bForceMot = false) = 0;

        //
        // Calls VRR enablement implementation in dp_vrr.cpp.
        // The enablement steps include interaction over DPAux in the vendor specific
        // DPCD space.
        //
        virtual bool startVrrEnablement() = 0; // VF: calls actual enablement code.
        virtual void resetVrrEnablement() = 0; // VF: resets enablement state.
        virtual bool isVrrMonitorEnabled() = 0; // VF: gets monitor enablement state.
        virtual bool isVrrDriverEnabled() = 0;  // VF: gets driver enablement state.

        // If the sink support MSA override in MST environment.
        virtual bool isMSAOverMSTCapable() = 0;
        virtual bool isFakedMuxDevice() = 0;
        virtual bool setPanelReplayConfig(panelReplayConfig prcfg) = 0;
        virtual bool getPanelReplayConfig(panelReplayConfig *pPrcfg) = 0;
        virtual bool isPanelReplaySupported() = 0;
        virtual bool getPanelReplayStatus(PanelReplayStatus *pPrStatus) = 0;
        virtual bool getDeviceSpecificData(NvU8 *oui, NvU8 *deviceIdString,
                                           NvU8 *hwRevision, NvU8 *swMajorRevision,
                                           NvU8 *swMinorRevision) = 0;
        virtual bool getParentSpecificData(NvU8 *oui, NvU8 *deviceIdString,
                                           NvU8 *hwRevision, NvU8 *swMajorRevision,
                                           NvU8 *swMinorRevision) = 0;

        virtual bool setModeList(DisplayPort::DpModesetParams *pModeList, unsigned numModes) = 0;

        virtual NvBool isSelectiveUpdateSupported(void) = 0;
        virtual NvBool isEarlyRegionTpSupported(void) = 0;
        virtual NvBool enableAdaptiveSyncSdp(NvBool enable) = 0;
        virtual SelectiveUpdateCaps getSelectiveUpdateCaps(void) = 0;
        virtual NvBool isAdaptiveSyncSdpNotSupportedInPr(void) = 0;
        virtual NvBool isdscDecodeNotSupportedInPr(void) = 0;
        virtual NvBool isLinkOffSupportedAfterAsSdpInPr(void) = 0;
        virtual void getAlpmCaps(void) = 0;
        virtual NvBool setAlpmConfig(AlpmConfig alpmcfg) = 0;
        virtual NvBool getAlpmStatus(AlpmStatus *pAlpmStatus) = 0;
        virtual NvBool isAuxLessAlpmSupported(void) = 0;

    protected:
            virtual ~Device() {}

    };

    class Group : virtual public Object
    {
    public:

        //
        //  Routines for changing which panels are in a group.  To move a stream to a new
        //  monitor without a modeset:
        //       remove(old_panel)
        //       insert(new_panel)
        //  The library will automatically switch over to the new configuration
        //
        virtual void insert(Device * dev) = 0;
        virtual void remove(Device * dev) = 0;

        //
        //      group->enumDevices(0)   - Get first element
        //      group->enumDevices(i)   - Get next element
        //
        //      for (Device * i = group->enumDevices(0); i; i = group->enumDevices(i))
        //
        virtual Device * enumDevices(Device * previousDevice) = 0;

        virtual void destroy() = 0;                 // Destroy the group object

        // Toggles the encryption status for the stream.
        // Returns whether encryption is currently enabled.
        virtual bool hdcpGetEncrypted() = 0;

        protected:
            virtual ~Group() {}

    };

    class Connector : virtual public Object
    {
    public:
        //
        //  Normally the Connector::EventSink callsback can occur in response to the following
        //      1. Timer callbacks
        //      2. notifyLongPulse/notifyShortPulse
        //
        class EventSink
        {
        public:
            virtual void newDevice(Device * dev) = 0;                       // New device appears in topology
            virtual void lostDevice(Device * dev) = 0;                      // Lost device from topology
                                                                            //   Device object ceases to exist after this call

            virtual void notifyMustDisconnect(Group * grp) = 0;             // Notification that an attached head is preventing
                                                                            // us from completing detection of a newly connected device

            virtual void notifyDetectComplete() = 0;                        // Rolling call.  Happens every time we've done another full
                                                                            //  detect on the topology

            virtual void bandwidthChangeNotification(Device * dev, bool isComplianceMode) = 0;     // Available bandwidth to panel has changed, or panel has
                                                                                                   //   become a zombie

            virtual void notifyZombieStateChange(Device * dev, bool zombied)  = 0;  // Notification that zombie device was attached or dettached
            virtual void notifyCableOkStateChange(Device * dev, bool cableOk) = 0;  // Notification that device got cable state chagne (true - cable is good, false - cables is bad)
            virtual void notifyHDCPCapDone(Device * dev, bool hdcpCap) = 0;         // Notification that device's HDCP cap detection is done and get state change.
            virtual void notifyMCCSEvent(Device * dev) = 0;                         // Notification that an MCCS event is coming
        };

        // Query current Device topology
        virtual Device * enumDevices(Device * previousDevice) = 0;

        //  Called before system enters an S3 state
        virtual void pause() = 0;

        // Get maximum link configuration
        virtual LinkConfiguration getMaxLinkConfig() = 0;

        // Get currently active link configuration
        virtual LinkConfiguration getActiveLinkConfig() = 0;

        // Get Current link configuration
        virtual void getCurrentLinkConfig(unsigned &laneCount, NvU64 &linkRate) = 0;

        // Get the clock calculation supported by the panel
        virtual unsigned getPanelDataClockMultiplier() = 0;

        // Get the clock calculation supported by the GPU
        virtual unsigned getGpuDataClockMultiplier() = 0;

        // Resume from standby/initial boot notification
        //   The library is considered to start up in the suspended state.  You must make this
        //   API call to enable the library.  None of the library APIs are functional before
        //   this call.
        //
        //   Returns the group representing the firmware panel if any is active.
        //
        //  plugged                 Does RM report the root-port DisplayId in
        //                          its plugged connector mask
        //
        //  firmwareLinkHandsOff    RM does NOT report the rootport displayId as active,
        //                          but one of the active panels shares the same SOR.
        //
        //  firmwareDPActive        RM reports the rootport displayId in the active device list
        //                          but display-driver hasn't yet performed its first modeset.
        //
        //  isUefiSystem            DD tells the library whether this system is a UEFI based
        //                          one so that the library can get the current and max link config
        //                          from RM/UEFI instead of trying to determine them on its own.
        //
        //  firmwareHead            Head being used to drive the firmware
        //                          display, if firmwareDPActive is true.
        //
        //  bFirmwareLinkUseMultistream
        //                          Specifies whether the firmware connector is being driven in SST
        //                          (false) or MST (true) mode.
        //
        //  bDisableVbiosScratchRegisterUpdate
        //                          Disables update of
        //                          NV_PDISP_SOR_DP_SCRATCH_RAD/MISC scratch
        //                          pad registers with last lit up display
        //                          address. This address is used by VBIOS in
        //                          case of driver unload or BSOD.
        //
        //  bAllowMST    Allow/Disallow Multi-streaming
        //
        virtual Group * resume(bool firmwareLinkHandsOff,
                               bool firmwareDPActive,
                               bool plugged,
                               bool isUefiSystem = false,
                               unsigned firmwareHead = 0,
                               bool bFirmwareLinkUseMultistream = false,
                               bool bDisableVbiosScratchRegisterUpdate = false,
                               bool bAllowMST = true) = 0;

        // The display-driver should enable hands off mode when attempting
        //   to use a shared resource (such as the SOR) in a non-DP configuration.
        virtual void enableLinkHandsOff() = 0;
        virtual void releaseLinkHandsOff() = 0;

        // Usage scenario:
        //    beginCompoundQuery()
        //      compoundQueryAttach(1280x1024)
        //      compoundQueryAttach(1920x1080)
        //    .endCompoundQuery()
        //  Will tell you if you have sufficient bandwidth to operate
        //  two panels at 1920x1080 and 1280x1024 assuming all currently
        //  attached panels are detached.
        virtual void beginCompoundQuery(const bool bForceEnableFEC = false) = 0;

        //
        // twoChannelAudioHz
        //    If you need 192khz stereo specify 192000 here.
        //
        // eightChannelAudioHz
        //   Same setting for multi channel audio.
        //   DisplayPort encodes 3-8 channel streams as 8 channel
        //
        // pixelClockHz
        //   Requested pixel clock for the mode
        //
        // depth
        //   Requested color depth
        //
        virtual bool compoundQueryAttach(Group * target,
                                 unsigned twoChannelAudioHz,
                                 unsigned eightChannelAudioHz,
                                 NvU64 pixelClockHz,
                                 unsigned rasterWidth,
                                 unsigned rasterHeight,
                                 unsigned rasterBlankStartX,
                                 unsigned rasterBlankEndX,
                                 unsigned depth,
                                 DP_IMP_ERROR *errorStatus = NULL) = 0;

        virtual bool compoundQueryAttach(Group * target,
                                         const DpModesetParams &modesetParams,      // Modeset info
                                         DscParams *pDscParams,                     // DSC parameters
                                         DP_IMP_ERROR *errorStatus = NULL) = 0;     // Error Status code

        virtual bool endCompoundQuery() = 0;

        virtual bool dpLinkIsModePossible(const DpLinkIsModePossibleParams &params) = 0;

        // Interface to indicate if clients need to perform a head shutdown before a modeset
        virtual bool isHeadShutDownNeeded(Group * target,            // Group of panels we're attaching to this head
                                 unsigned headIndex,
                                 unsigned twoChannelAudioHz,         // if you need 192khz stereo specify 192000 here
                                 unsigned eightChannelAudioHz,       // Same setting for multi channel audio.
                                                                     //  DisplayPort encodes 3-8 channel streams as 8 channel
                                 NvU64 pixelClockHz,                 // Requested pixel clock for the mode
                                 unsigned rasterWidth,
                                 unsigned rasterHeight,
                                 unsigned rasterBlankStartX,
                                 unsigned rasterBlankEndX,
                                 unsigned depth) = 0;

        // Interface to indicate if clients need to perform a head shutdown before a modeset
        virtual bool isHeadShutDownNeeded(Group * target,               // Group of panels we're attaching to this head
                                         unsigned headIndex,
                                         ModesetInfo modesetInfo) = 0;  // Modeset info relevant DSC data

        //
        // Interface for clients to query library if the link is going to be trained during notifyAttachBegin(modeset).
        // Note: This API is not intended to know if a link training will be performed during assessment of the link.
        // This API is added to see if library can avoid link training during modeset so that client can take necessary decision
        // to avoid a destructive modeset from UEFI mode at post to a GPU driver detected mode
        // (thus prevent a visible glitch - i.e. Smooth Transition)
        //
        // How isLinkTrainingNeededForModeset API is different from isHeadShutDownNeeded API -
        //    In case of MST : we always shutdown head and link train if link is inactive, so both APIs return TRUE
        //    In case of SST :
        //       - If requested link config < active link config, we shutdown head to prevent overflow
        //         as head will still be driving at higher mode during link training to lower mode
        //         So both APIs return TRUE
        //       - If requested link config >= active link config, we don't need a head shutdown since
        //         SOR clocks can be changed by entering flush mode but will need to link train for mode change
        //         So isHeadShutDownNeeded returns FALSE and isLinkTrainingNeededForModeset returns TRUE
        //
        virtual bool isLinkTrainingNeededForModeset (ModesetInfo modesetInfo) = 0;

        // Notify library before/after modeset (update)
        virtual bool notifyAttachBegin(Group * target,               // Group of panels we're attaching to this head
                                 unsigned headIndex,
                                 unsigned twoChannelAudioHz,         // if you need 192khz stereo specify 192000 here
                                 unsigned eightChannelAudioHz,       // Same setting for multi channel audio.
                                                                     //  DisplayPort encodes 3-8 channel streams as 8 channel
                                 NvU64 pixelClockHz,                 // Requested pixel clock for the mode
                                 unsigned rasterWidth,
                                 unsigned rasterHeight,
                                 unsigned rasterBlankStartX,
                                 unsigned rasterBlankEndX,
                                 unsigned depth) = 0;

        // Group of panels we're attaching to this head
        virtual bool notifyAttachBegin(Group * target, const DpModesetParams &modesetParams) = 0;

        virtual void dpPreModeset(const DpPreModesetParams &modesetParams) = 0;
        virtual void dpPostModeset(void) = 0;

        virtual void readRemoteHdcpCaps() = 0;

        // modeset might be cancelled when NAB failed
        virtual void notifyAttachEnd(bool modesetCancelled) = 0;

        //
        // Client needs to be notified about the SST<->MST transition,
        // based on which null modeset will be sent.
        //
        virtual bool isLinkAwaitingTransition() = 0;

        virtual void resetLinkTrainingCounter() = 0;

        // Notify library before/after shutdown (update)
        virtual void notifyDetachBegin(Group * target) = 0;
        virtual void notifyDetachEnd(bool bKeepOdAlive = false) = 0;

        // Notify library to assess PCON link capability
        virtual bool assessPCONLinkCapability(PCONLinkControl *params) = 0;

        // Notify library of hotplug/IRQ
        virtual void notifyLongPulse(bool statusConnected) = 0;
        virtual void notifyShortPulse() = 0;

        // Notify Library when ACPI initialization is done
        virtual void notifyAcpiInitDone() = 0;

        // Notify Library when GPU capability changes. Usually because power event.
        virtual void notifyGPUCapabilityChange() = 0;
        virtual void notifyHBR2WAREngage() = 0;

        virtual bool dpUpdateDscStream(Group *target, NvU32 dscBpp) = 0;

        // Create a new Group.  Note that if you wish to do a modeset but send the
        // stream nowhere, you may do a modeset with an EMPTY group.  This is expected
        // to be the mechanism by which monitor faking is implemented.
        virtual Group * newGroup() = 0;

        // Shutdown and the destroy the connector manager
        virtual void destroy() = 0;

        virtual void createFakeMuxDevice(const NvU8 *buffer, NvU32 bufferSize) = 0;
        virtual void deleteFakeMuxDevice() = 0;
        virtual bool getRawDscCaps(NvU8 *buffer, NvU32 bufferSize) = 0;

        //
        //  OS Modeset Order mitigation causes the library to delay the reporting
        //   of new devices until they can be safely turned on.
        //  When enabled the library client will not see connection events until
        //    MustDisconnect messages are processed.
        //
        //   Policy state should be set before the library is brought out of
        //   the suspended state.
        //
        //   Important Note: This option changes the definition of QueryMode.
        //      Without OS order mitigation query mode assumes that you will
        //      deatach all of the heads from any zombied monitors *before*
        //      activating the new panel.  If your driver cannot guarantee
        //      this invariant, then it must enable order mitigation.
        //
        virtual void setPolicyModesetOrderMitigation(bool enabled) = 0;

        //
        //  force LT at NAB for compliance test (Power Management) in Win10 RS2+ (WDDM 2.2)
        //
        //  RS2 no longer sends an explicit call for setPanelPowerParams during the Resume.
        //  It does that by specifying an additional flag during the call to SetTimings. Due to
        //  this DP lib doesn't get chance to perform this transition from setPanelPowerParams
        //  and since it was already skipping LT in NAB/modeswitch, so LT get missed out on the
        //  compliance device during resume from S3/S4.
        //
        virtual void setPolicyForceLTAtNAB(bool enabled) = 0;

        //
        //  There are cases where OS does not detach heads from connector immediately after hot-unplug,
        //  on next hot-plug there is no guarantee that newly connected sink is capable to drive existing
        //  raster timings. Flush mode has following restriction
        //      When exiting flush mode S/W should ensure that the final
        //      link clock & lane count should be able to support existing raster.
        //  If we run into this situation and use flush mode then that will cause display engine to hang.
        //  This variable ensures to assess link safely in this situation and instead of using flush mode ask
        //  DD to detach/reattach heads for link training.
        //
        virtual void setPolicyAssessLinkSafely(bool enabled) = 0;

        //
        // These interfaces are meant to be used *ONLY* for tool purposes.
        // Clients should *NOT* use them for their own implementation.
        //
        // Sets the preferred link config which the tool has requested to train to.
        // Each set call should be paired with a reset call. Also, preferred link configs won't persist across HPDs.
        // It is advisable to do compound queries before setting a mode on a preferred config.
        // Compound queries and notify attaches(link train) would use the preferred link config unless it is reset again.
        // (not advisable to leave a preferred link config always ON).
        //
        virtual bool setPreferredLinkConfig(LinkConfiguration &lc, bool commit,
                                            bool force = false,
                                            LinkTrainingType forceTrainType = NORMAL_LINK_TRAINING,
                                            bool forcePreferredLinkConfig = false) = 0;

        //
        // Resets the preferred link config and lets the library go back to default LT policy.
        // Should follow a previous set call.
        //
        virtual bool resetPreferredLinkConfig(bool force=false) = 0;

        //
        // These interfaces are used by client to allow/disallow
        // Multi-streaming.
        //
        // If connected sink is MST capable then:
        // Client should detach all active MST video/audio streams before
        // disallowing MST, vise-versa client should detach active SST
        // stream before allowing MST.
        //
        virtual void setAllowMultiStreaming(bool bAllowMST) = 0;
        virtual bool getAllowMultiStreaming(void) = 0;

        // This function reads sink MST capability from DPCD register(s).
        virtual bool getSinkMultiStreamCap(void) = 0;

        // These interfaces are Deprecated, use setAllowMultiStreaming()
        virtual void setDp11ProtocolForced() = 0;
        virtual void resetDp11ProtocolForced() = 0;
        virtual bool isDp11ProtocolForced() = 0;

        virtual bool getHDCPAbortCodesDP12(NvU32 &hdcpAbortCodesDP12) = 0;

        virtual bool getOuiSink(unsigned &ouiId, unsigned char * modelName,
                                size_t modelNameBufferSize, NvU8 &chipRevision) = 0;

        virtual bool getIgnoreSourceOuiHandshake() = 0;
        virtual void setIgnoreSourceOuiHandshake(bool bIgnore) = 0;

        //
        // The following function is to be used to get the capability bit that tells the client whether the connector
        // can do multistream.
        //
        virtual bool isMultiStreamCapable() = 0;
        virtual bool isFlushSupported() = 0;
        virtual bool isStreamCloningEnabled() = 0;
        virtual NvU32 maxLinkRateSupported() = 0;
        virtual bool isFECSupported() = 0;
        virtual bool isFECCapable() = 0;

        // Following APIs are for link test/config for DP Test Utility
        virtual bool getTestPattern(NV0073_CTRL_DP_TESTPATTERN *pTestPattern) = 0;
        virtual bool setTestPattern(NV0073_CTRL_DP_TESTPATTERN testPattern,
                                    NvU8 laneMask, NV0073_CTRL_DP_CSTM cstm,
                                    NvBool bIsHBR2, NvBool bSkipLaneDataOverride) = 0;

        // "data" is an array of NV0073_CTRL_MAX_LANES unsigned ints
        virtual bool getLaneConfig(NvU32 *numLanes, NvU32 *data) = 0;
        // "data" is an array of NV0073_CTRL_MAX_LANES unsigned ints
        virtual bool setLaneConfig(NvU32 numLanes, NvU32 *data) = 0;

        // "data" is an array of NV0073_CTRL_MAX_LANES unsigned ints
        virtual bool getDp2xLaneConfig(NvU32 *numLanes, NvU32 *data) = 0;
        // "data" is an array of NV0073_CTRL_MAX_LANES unsigned ints
        virtual bool setDp2xLaneConfig(NvU32 numLanes, NvU32 *data) = 0;
        virtual DP_TESTMESSAGE_STATUS sendDPTestMessage(void *pBuffer,
                                                        NvU32 requestSize,
                                                        NvU32 *pDpStatus) = 0;

        virtual DP_TESTMESSAGE_STATUS getStreamIDs(NvU32 *pStreamIDs, NvU32 *pCount) = 0;
        // Function to configure power up/down for DP Main Link
        virtual void configurePowerState(bool bPowerUp) = 0;

    virtual void readPsrCapabilities(vesaPsrSinkCaps *caps) = 0;
    virtual bool updatePsrConfiguration(vesaPsrConfig config) = 0;
    virtual bool readPsrConfiguration(vesaPsrConfig *config) = 0;
    virtual bool readPsrState(vesaPsrState *psrState) = 0;
    virtual bool readPsrDebugInfo(vesaPsrDebugStatus *psrDbgState) = 0;
    virtual bool writePsrErrorStatus(vesaPsrErrorStatus psrErr) = 0;
    virtual bool readPsrErrorStatus(vesaPsrErrorStatus *psrErr) = 0;
    virtual bool writePsrEvtIndicator(vesaPsrEventIndicator psrErr) = 0;
    virtual bool readPsrEvtIndicator(vesaPsrEventIndicator *psrErr) = 0;
    virtual bool updatePsrLinkState(bool bTurnOnLink) = 0;

    virtual bool readPrSinkDebugInfo(panelReplaySinkDebugInfo *prDbgInfo) = 0;
    virtual void enableDpTunnelingBwAllocationSupport() = 0;
    virtual bool willLinkSupportModeSST(const LinkConfiguration &linkConfig,
                                        const ModesetInfo &modesetInfo,
                                        const DscParams *pDscParams = NULL) = 0;

    protected:
           virtual ~Connector() {}
    };

    //
    //  Library routine to create primary port interface
    //      (Not intended to be used by display driver)
    Connector * createConnector(MainLink * mainInterface,           // DisplayDriver implemented MainLink object
                                AuxBus * auxInterface,              // DisplayDriver implemented AuxRetry wrapper
                                Timer * timerInterface,             // DisplayDriver provided Timer services
                                Connector::EventSink * sink);       // Interface to notify DisplayDriver of events
}
#endif //INCLUDED_DP_CONNECTOR_H
