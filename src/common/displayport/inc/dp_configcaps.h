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

/******************************* DisplayPort *******************************\
*                                                                           *
* Module: dp_configcaps.h                                                   *
*    Abstraction for basic caps registers                                   *
*                                                                           *
\***************************************************************************/

#ifndef INCLUDED_DP_CONFIGCAPS_H
#define INCLUDED_DP_CONFIGCAPS_H

#include "dp_connector.h"
#include "dp_auxretry.h"
#include "dp_linkconfig.h"
#include "dp_regkeydatabase.h"

namespace DisplayPort
{
    enum PowerState
    {
        PowerStateD0 = 1,
        PowerStateD3 = 2,
        PowerStateD3AuxOn = 5
    };

    // Extended caps = offset 0x80
    enum DwnStreamPortType
    {
        DISPLAY_PORT = 0,
        ANALOG_VGA,
        DVI,
        HDMI,
        WITHOUT_EDID,
        DISPLAY_PORT_PLUSPLUS
    } ;

    enum DwnStreamPortAttribute
    {
        RESERVED = 0,
        IL_720_480_60HZ,
        IL_720_480_50HZ,
        IL_1920_1080_60HZ,
        IL_1920_1080_50HZ,
        PG_1280_720_60HZ,
        PG_1280_720_50_HZ,
    } ;

    // DPCD Offset 102 enums
    enum TrainingPatternSelectType
    {
        TRAINING_DISABLED,
        TRAINING_PAT_ONE,
        TRAINING_PAT_TWO,
        TRAINING_PAT_THREE,
    };

    enum SymbolErrorSelectType
    {
        DISPARITY_ILLEGAL_SYMBOL_ERROR,
        DISPARITY_ERROR,
        ILLEGAL_SYMBOL_ERROR,
    };

    // DPCD Offset 1A1 enums
    enum MultistreamHotplugMode
    {
        HPD_LONG_PULSE,
        IRQ_HPD,
    };

    // DPCD Offset 220
    enum TestPatternType
    {
        NO_PATTERN,
        COLOR_RAMPS,
        BLACK_WHITE,
        COLOR_SQUARE,
    } ;

    // DPCD Offset 232, 233
    enum ColorFormatType
    {
        RGB,
        YCbCr_422,
        YCbCr_444,
    } ;

    enum DynamicRangeType
    {
        VESA,
        CEA,
    } ;

    enum YCBCRCoeffType
    {
        ITU601,
        ITU709,
    } ;

    enum MainLinkChannelCoding
    {
        ChannelCoding8B10B = 0,
        ChannelCoding128B132B,
    };

    #define HDCP_BCAPS_SIZE                 (0x1)
    #define HDCP_VPRIME_SIZE                (0x14)
    #define HDCP_KSV_FIFO_SIZE              (0xF)
    #define HDCP_KSV_FIFO_WINDOWS_RETRY     (0x3)
    #define HDCP22_BCAPS_SIZE               (0x1)

    // Bstatus DPCD offset 0x68029
    #define HDCPREADY                       (0x1)
    #define R0PRIME_AVAILABLE               (0x2)
    #define LINK_INTEGRITY_FAILURE          (0x4)
    #define REAUTHENTICATION_REQUEST        (0x8)

    struct BInfo
    {
        bool maxCascadeExceeded;
        unsigned depth;
        bool maxDevsExceeded;
        unsigned deviceCount;
    };

    struct BCaps
    {
        bool repeater;
        bool HDCPCapable;
    };

    enum
    {
        PHYSICAL_PORT_START = 0x0,
        PHYSICAL_PORT_END   = 0x7,
        LOGICAL_PORT_START  = 0x8,
        LOGICAL_PORT_END    = 0xF
    };

    class LaneStatus
    {
    public:
        //
        //  Lane Status
        //      CAUTION: Only updated on IRQ/HPD right now
        //
        virtual bool     getLaneStatusClockRecoveryDone(int lane) = 0;                // DPCD offset 202, 203
        virtual bool     getLaneStatusSymbolLock(int lane)= 0;
        virtual bool     getLaneStatusChannelEqualizationDone(int lane)= 0;
        virtual bool     getInterlaneAlignDone() = 0;
        virtual bool     getDownStreamPortStatusChange() = 0;
    };

    class TestRequest
    {
    public:
        virtual bool getPendingTestRequestTraining() = 0;                              // DPCD offset 218
        virtual void getTestRequestTraining(LinkRate & rate, unsigned & lanes) = 0;    // DPCD offset 219, 220
        virtual bool getPendingAutomatedTestRequest() = 0;                             // DPCD offset 218
        virtual bool getPendingTestRequestEdidRead() = 0;                              // DPCD offset 218
        virtual bool getPendingTestRequestPhyCompliance() = 0;                         // DPCD offset 218
        virtual LinkQualityPatternType getPhyTestPattern() = 0;                        // DPCD offset 248
        virtual AuxRetry::status setTestResponse(bool ack, bool edidChecksumWrite = false) = 0;
        virtual AuxRetry::status setTestResponseChecksum(NvU8 checksum) = 0;
    };

    class LegacyPort
    {
    public:
        virtual DwnStreamPortType      getDownstreamPortType() = 0;
        virtual DwnStreamPortAttribute getDownstreamNonEDIDPortAttribute() = 0;

        // For port type = HDMI
        virtual NvU64                  getMaxTmdsClkRate() = 0;
    };

    class LinkState
    {
    public:
        //
        //  Link state
        //
        virtual bool             isPostLtAdjustRequestSupported() = 0;
        virtual void             setPostLtAdjustRequestGranted(bool bGrantPostLtRequest) = 0;
        virtual bool             getIsPostLtAdjRequestInProgress() = 0;                     // DPCD offset 204
        virtual TrainingPatternSelectType getTrainingPatternSelect() = 0;                   // DPCD offset 102

        virtual bool setTrainingMultiLaneSet(NvU8 numLanes,
                                             NvU8 *voltSwingSet,
                                             NvU8 *preEmphasisSet) = 0;

        virtual bool readTraining(NvU8* voltageSwingLane,
                                  NvU8* preemphasisLane = 0,
                                  NvU8* trainingScoreLane = 0,
                                  NvU8* postCursor = 0,
                                  NvU8 activeLaneCount = 0) = 0;

        virtual bool isLaneSettingsChanged(NvU8* oldVoltageSwingLane,
                                           NvU8* newVoltageSwingLane,
                                           NvU8* oldPreemphasisLane,
                                           NvU8* newPreemphasisLane,
                                           NvU8 activeLaneCount) = 0;

        virtual AuxRetry::status setIgnoreMSATimingParamters(bool msaTimingParamIgnoreEn) = 0;
        virtual AuxRetry::status setLinkQualLaneSet(unsigned lane, LinkQualityPatternType linkQualPattern) = 0;
        virtual AuxRetry::status setLinkQualPatternSet(LinkQualityPatternType linkQualPattern, unsigned laneCount = 0) = 0;
        virtual AuxRetry::status setLinkQualPatternSet(DP2xPatternInfo& patternInfo, unsigned laneCount = 0) = 0;
    };

    class LinkCapabilities
    {
    public:

        //
        //  Physical layer feature set
        //
        virtual NvU64    getMaxLinkRate() = 0;                                // Maximum byte-block in Hz
        virtual unsigned getMaxLaneCount() = 0;                               // DPCD offset 0x0002h
        //
        // Bit mask for sink side (sink and LTTPR) supported UHBR Link Rates.
        // Defines the same as NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS.UHBRSupportedByGpu and
        //                     NV0073_CTRL_DFP_GET_INFO_PARAMS.UHBRSupportedByDfp
        //
        virtual NvU32    getUHBRSupported() = 0;                              // DPCD offset 0x2215h
        virtual unsigned getMaxLaneCountSupportedAtLinkRate(LinkRate linkRate) = 0;
        virtual bool     getEnhancedFraming() = 0;
        virtual bool     getSupportsNoHandshakeTraining() = 0;
        virtual bool     getMsaTimingparIgnored() = 0;
        virtual bool     getDownstreamPort(NvU8 *portType) = 0;               // DPCD offset 0x0005h
        virtual bool     getSupportsMultistream() = 0;                        // DPCD offset 0x0021h
        virtual bool     getNoLinkTraining() = 0;                             // DPCD offset 0x0330h
        virtual unsigned getPhyRepeaterCount() = 0;                           // DPCD offset 0xF0002h
    };

    class OUI
    {
    public:
        virtual bool             getOuiSupported() = 0;
        virtual AuxRetry::status setOuiSource(unsigned ouiId, const char * model, size_t modelNameLength, NvU8 chipRevision) = 0;
        virtual bool             getOuiSource(unsigned &ouiId, char * modelName, size_t modelNameBufferSize, NvU8 & chipRevision) = 0;
        virtual bool             getOuiSink(unsigned &ouiId, unsigned char * modelName, size_t modelNameBufferSize, NvU8 & chipRevision) = 0;
    };

    class HDCP
    {
    public:
        virtual bool getBKSV(NvU8 *bKSV) = 0;                                   // DPCD offset 0x68000
        virtual bool getBCaps(BCaps &bCaps, NvU8 * rawByte = 0) = 0;            // DPCD offset 0x68028
        virtual bool getHdcp22BCaps(BCaps &bCaps, NvU8 * rawByte = 0) = 0;      // DPCD offset 0x6921D
        virtual bool getBinfo(BInfo &bInfo) = 0;                                // DPCD offset 0x6802A

        // Generic interfaces for HDCP 1.x / 2.2
        virtual bool getRxStatus(const HDCPState &hdcpState, NvU8 *data) = 0;
    };

    class DPCDHAL :
        virtual public Object,
        public TestRequest,
        public LaneStatus,
        public LinkState,
        public LinkCapabilities,
        public OUI,
        public HDCP
    {
    public:
        //
        //  Notifications of external events
        //      We sent IRQ/HPD events to the HAL so that it knows
        //      when to re-read the registers.  All the remaining
        //      calls are either accessors to cached state (caps),
        //      or DPCD get/setters
        //
        virtual void notifyIRQ() = 0;
        virtual void notifyHPD(bool status, bool bSkipDPCDRead = false) = 0;

        virtual void populateFakeDpcd() = 0;

        // DPCD override routines
        virtual void overrideMaxLinkRate(NvU32 overrideMaxLinkRate) = 0;
        virtual void overrideMaxLaneCount(NvU32 maxLaneCount) = 0;
        virtual void skipCableBWCheck(NvU32 maxLaneAtHighRate, NvU32 maxLaneAtLowRate) = 0;
        virtual void overrideOptimalLinkCfg(LinkRate optimalLinkRate, NvU32 optimalLaneCount) = 0;
        virtual void overrideOptimalLinkRate(LinkRate optimalLinkRate) = 0;

        virtual bool isDpcdOffline() = 0;
        virtual void setAuxBus(AuxBus * bus) = 0;
        virtual NvU32 getVideoFallbackSupported() = 0;
        //
        //  Cached CAPS
        //    These are only re-read when notifyHPD is called
        //
        virtual unsigned getRevisionMajor() = 0;
        virtual unsigned getRevisionMinor() = 0;

        virtual unsigned lttprGetRevisionMajor() = 0;
        virtual unsigned lttprGetRevisionMinor() = 0;

        virtual bool getSDPExtnForColorimetry() = 0;
        virtual bool getRootAsyncSDPSupported() = 0;

        bool isAtLeastVersion(unsigned major, unsigned minor)
        {
            if (getRevisionMajor() > major)
                return true;

            if (getRevisionMajor() < major)
                return false;

            return getRevisionMinor() >= minor;
        }

        bool isVersion(unsigned major, unsigned minor)
        {
            if ((getRevisionMajor() == major) &&
                (getRevisionMinor() == minor))
                return true;

            return false;
        }

        bool lttprIsAtLeastVersion(unsigned major, unsigned minor)
        {
            if (lttprGetRevisionMajor() > major)
                return true;

            if (lttprGetRevisionMajor() < major)
                return false;

            return lttprGetRevisionMinor() >= minor;
        }

        bool lttprIsVersion(unsigned major, unsigned minor)
        {
            if ((lttprGetRevisionMajor() == major) &&
                (lttprGetRevisionMinor() == minor))
                return true;

            return false;
        }

        // Convert Link Bandwidth read from DPCD 00001h/2201h 8b10b_MAX_LINK_RATE to 10M convention link rate
        NvU32 mapLinkBandiwdthToLinkrate(NvU32 linkBandwidth)
        {
            if (FLD_TEST_DRF(_DPCD, _MAX_LINK_BANDWIDTH, _VAL, _1_62_GBPS, linkBandwidth))
                return dp2LinkRate_1_62Gbps;
            else if (FLD_TEST_DRF(_DPCD, _MAX_LINK_BANDWIDTH, _VAL, _2_70_GBPS, linkBandwidth))
                return dp2LinkRate_2_70Gbps;
            else if (FLD_TEST_DRF(_DPCD, _MAX_LINK_BANDWIDTH, _VAL, _5_40_GBPS, linkBandwidth))
                return dp2LinkRate_5_40Gbps;
            else if (FLD_TEST_DRF(_DPCD14, _MAX_LINK_BANDWIDTH, _VAL, _8_10_GBPS, linkBandwidth))
                return dp2LinkRate_8_10Gbps;
            else
            {
                DP_ASSERT(0 && "Unknown link bandwidth. Assuming HBR");
                return dp2LinkRate_2_70Gbps;
            }
        }

        //
        //  Native aux transaction size (16 for AUX)
        //
        virtual size_t   getTransactionSize() = 0;

        //
        //  SST Branching device/dongle/repeater
        //     - Describes downstream port limitations
        //     - Not for use with MST
        //     - Primarily used for dongles (look at port 0 for pclk limits)
        //
        virtual LegacyPort     * getLegacyPort(unsigned index) = 0;
        virtual unsigned         getLegacyPortCount() = 0;

        virtual PCONCaps       * getPCONCaps() = 0;

        //
        //  Single stream specific caps
        //
        virtual unsigned getNumberOfAudioEndpoints() = 0;
        virtual int      getSinkCount() = 0;
        virtual void     setSinkCount(int sinkCount) = 0;

        //
        //  MISC
        //
        virtual bool isPC2Disabled() = 0;
        virtual void setPC2Disabled(bool disabled) = 0;

        virtual void setDPCDOffline(bool enable) = 0;
        virtual void updateDPCDOffline() = 0;
        virtual bool auxAccessAvailable() = 0;

        virtual void setSupportsESI(bool bIsESISupported) = 0;
        virtual void setLttprSupported(bool isLttprSupported) = 0;

        //
        // Intermediate Link Rate (eDP ILR)
        //
        virtual void setIndexedLinkrateEnabled(bool newVal) = 0;
        virtual bool isIndexedLinkrateEnabled() = 0;
        virtual bool isIndexedLinkrateCapable() = 0;
        virtual NvU16 *getLinkRateTable() = 0;
        virtual bool getRawLinkRateTable(NvU8 *buffer = NULL) = 0;

        //
        //  Link power state management
        //
        virtual bool setPowerState(PowerState newState) = 0;
        virtual PowerState getPowerState() = 0;
        //
        //  Multistream
        //
        virtual bool             getGUID(GUID & guid) = 0;                    // DPCD offset 30
        virtual AuxRetry::status setGUID(GUID & guid) = 0;
        virtual AuxRetry::status setMessagingEnable(bool uprequestEnable, bool upstreamIsSource) = 0;
        virtual AuxRetry::status setMultistreamLink(bool bMultistream) = 0;
        virtual void             payloadTableClearACT() = 0;
        virtual bool             payloadWaitForACTReceived() = 0;
        virtual bool             payloadAllocate(unsigned streamId, unsigned begin, unsigned count) = 0;
        virtual bool             clearPendingMsg() = 0;
        virtual bool             isMessagingEnabled() = 0;

        //
        //    If set to IRQ we'll receive CSN messages on hotplugs (which are actually easy to miss).
        //    If set to HPD mode we'll always receive an HPD whenever the topology changes.
        //    The library supports using both modes.
        //
        virtual AuxRetry::status setMultistreamHotplugMode(MultistreamHotplugMode notifyType) = 0;

        //
        //  Interrupts
        //
        virtual bool             interruptContentProtection() = 0;
        virtual void             clearInterruptContentProtection() = 0;

        virtual bool             intteruptMCCS() = 0;
        virtual void             clearInterruptMCCS() = 0;

        virtual bool             interruptDownReplyReady() = 0;
        virtual void             clearInterruptDownReplyReady() = 0;

        virtual bool             interruptUpRequestReady() = 0;
        virtual void             clearInterruptUpRequestReady() = 0;

        virtual bool             interruptCapabilitiesChanged() = 0;
        virtual void             clearInterruptCapabilitiesChanged() = 0;

        virtual bool             getLinkStatusChanged() = 0;
        virtual void             clearLinkStatusChanged() = 0;

        virtual bool             isPanelReplayErrorSet() = 0;
        virtual void             clearPanelReplayError() = 0;
        virtual void             readPanelReplayError() = 0;

        virtual bool             getHdmiLinkStatusChanged() = 0;
        virtual void             clearHdmiLinkStatusChanged() = 0;

        virtual bool             getStreamStatusChanged() = 0;
        virtual void             clearStreamStatusChanged() =0;

        virtual bool             getDpTunnelingIrq() = 0;
        virtual void             clearDpTunnelingIrq() = 0;

        virtual void             setDirtyLinkStatus(bool dirty) = 0;
        virtual void             refreshLinkStatus() = 0;
        virtual bool             isLinkStatusValid(unsigned lanes) = 0;

        // DPCD offset 250 - 259
        virtual void get80BitsCustomTestPattern(NvU8 *testPattern) = 0;
        // DPCD offset 2230 - 2250
        virtual void get264BitsCustomTestPattern(NvU8 *testPattern) = 0;
        // DPCD offset 249
        virtual void getSquarePatternNum(NvU8 *sqNum) = 0;
        //
        //  Message Boxes
        //
        virtual AuxRetry::status    writeDownRequestMessageBox(NvU8 * data, size_t length) = 0;
        virtual size_t              getDownRequestMessageBoxSize() = 0;

        virtual AuxRetry::status    writeUpReplyMessageBox(NvU8 * data, size_t length) = 0;
        virtual size_t              getUpReplyMessageBoxSize() = 0;

        virtual AuxRetry::status    readDownReplyMessageBox(NvU32 offset, NvU8 * data, size_t length) = 0;
        virtual size_t              getDownReplyMessageBoxSize() = 0;

        virtual AuxRetry::status    readUpRequestMessageBox(NvU32 offset, NvU8 * data, size_t length) = 0;
        virtual size_t              getUpRequestMessageBoxSize() = 0;

        // MST<->SST override
        virtual void overrideMultiStreamCap(bool mstCapable) = 0;
        virtual bool getMultiStreamCapOverride() = 0;

        virtual bool getDpcdMultiStreamCap(void) = 0;

        // Set GPU DP support capability
        virtual void setGpuDPSupportedVersions(NvU32 gpuDPSupportedVersions) = 0;

        // Set GPU FEC support capability
        virtual void setGpuFECSupported(bool bSupportFEC) = 0;

        virtual void applyRegkeyOverrides(const DP_REGKEY_DATABASE& dpRegkeyDatabase) = 0;

        // PCON configuration

        // Reset PCON (to default state)
        virtual void resetProtocolConverter() = 0;
        // Source control mode and FRL/HDMI mode selection.
        virtual bool setSourceControlMode(bool bEnableSourceControlMode, bool bEnableFRLMode) = 0;

        virtual bool checkPCONFrlReady(bool *bFrlReady) = 0;
        virtual bool setupPCONFrlLinkAssessment(NvU32   linkBw,
                                                bool    bEnableExtendLTMode     = false,
                                                bool    bEnableConcurrentMode   = false) = 0;

        virtual bool checkPCONFrlLinkStatus(NvU32 *frlRate) = 0;

        virtual bool queryHdmiLinkStatus(bool *bLinkActive, bool *bLinkReady) = 0;
        virtual NvU32 restorePCONFrlLink(NvU32   linkBwMask,
                                         bool    bEnableExtendLTMode     = false,
                                         bool    bEnableConcurrentMode   = false) = 0;

        virtual void readPsrCapabilities(vesaPsrSinkCaps *caps) = 0;
        virtual bool updatePsrConfiguration(vesaPsrConfig config) = 0;
        virtual bool readPsrConfiguration(vesaPsrConfig *config) = 0;
        virtual bool readPsrState(vesaPsrState *psrState) = 0;
        virtual bool readPsrDebugInfo(vesaPsrDebugStatus *psrDbgState) = 0;
        virtual bool writePsrErrorStatus(vesaPsrErrorStatus psrErr) = 0;
        virtual bool readPsrErrorStatus(vesaPsrErrorStatus *psrErr) = 0;
        virtual bool writePsrEvtIndicator(vesaPsrEventIndicator psrErr) = 0;
        virtual bool readPsrEvtIndicator(vesaPsrEventIndicator *psrErr) = 0;
        virtual bool readPrSinkDebugInfo(panelReplaySinkDebugInfo *prDbgInfo) = 0;

        virtual void     enableDpTunnelingBwAllocationSupport() = 0;
        virtual bool     isDpTunnelBwAllocationEnabled() = 0;
        virtual bool     getDpTunnelEstimatedBw(NvU8 &estimatedBw) = 0;
        virtual bool     getDpTunnelGranularityMultiplier(NvU8 &granularityMultiplier) = 0;
        virtual TriState getDpTunnelBwRequestStatus() = 0;
        virtual bool     setDpTunnelBwAllocation(bool bEnable) = 0;
        virtual bool     hasDpTunnelEstimatedBwChanged() = 0;
        virtual bool     hasDpTunnelBwAllocationCapabilityChanged() = 0;
        virtual bool     writeDpTunnelRequestedBw(NvU8 requestedBw) = 0;
        virtual bool     clearDpTunnelingBwRequestStatus() = 0;
        virtual bool     clearDpTunnelingEstimatedBwStatus() = 0;
        virtual bool     clearDpTunnelingBwAllocationCapStatus() = 0;

        virtual AuxRetry::status notifySDPErrDetectionCapability() = 0;
        virtual bool isDp2xChannelCodingCapable() = 0;
        virtual void setIgnoreCableIdCaps(bool bIgnore) = 0;
        virtual void overrideCableIdCap(LinkRate linkRate, bool bEnable) = 0;
        virtual void initialize() = 0;
        virtual AuxRetry::status setMainLinkChannelCoding(MainLinkChannelCoding channelCoding) = 0;
        virtual void setConnectorTypeC(bool bTypeC) = 0;
        virtual void setUSBCCableIDInfo(NV0073_CTRL_DP_USBC_CABLEID_INFO *cableIDInfo) = 0;
        virtual void setCableVconnSourceUnknown() = 0;
        virtual ~DPCDHAL() {}
    };

    //
    //  Implement interface
    //
    DPCDHAL * MakeDPCDHAL(AuxBus *  bus, Timer * timer, MainLink * main);

    struct DPCDHALImpl : DPCDHAL
    {
        AuxRetry  bus;
        Timer    * timer;
        bool      dpcdOffline;
        bool      bGrantsPostLtRequest;
        bool      pc2Disabled;
        bool      uprequestEnable;
        bool      upstreamIsSource;
        bool      bMultistream;
        bool      bGpuFECSupported;
        bool      bLttprSupported;
        bool      bBypassILREdpRevCheck;
        NvU32     overrideDpcdMaxLinkRate;
        NvU32     overrideDpcdRev;
        NvU32     overrideDpcdMaxLaneCount;

        NvU32     gpuDPSupportedVersions;

        struct _LegacyPort: public LegacyPort
        {
            DwnStreamPortType         type;
            DwnStreamPortAttribute    nonEDID;

            NvU64                     maxTmdsClkRate;

            DwnStreamPortType getDownstreamPortType()
            {
                return type;
            }

            DwnStreamPortAttribute getDownstreamNonEDIDPortAttribute()
            {
                return nonEDID;
            }

            NvU64 getMaxTmdsClkRate()
            {
                return maxTmdsClkRate;
            }

        } legacyPort[16];

        struct
        {
            unsigned  revisionMajor, revisionMinor;                 // DPCD offset 0
            bool      supportsESI;
            LinkRate  maxLinkRate;                                  // DPCD offset 1
            unsigned  maxLaneCount;                                 // DPCD offset 2
            unsigned  maxLanesAtHBR;
            unsigned  maxLanesAtRBR;
            bool      enhancedFraming;
            bool      bPostLtAdjustmentSupport;

            bool      supportsNoHandshakeTraining;
            bool      bSupportsTPS4;
            unsigned  NORP;                                         // DPCD offset 4

            bool      detailedCapInfo;                              // DPCD offset 5
            bool      downStreamPortPresent;
            NvU8      downStreamPortType;

            unsigned  downStreamPortCount;                          // DPCD offset 7
            bool      ouiSupported;
            bool      msaTimingParIgnored;

            NvU16     linkRateTable[NV_DPCD_SUPPORTED_LINK_RATES__SIZE]; // DPCD offset 10 ~ 1F

            bool      supportsMultistream;                          // DPCD offset 21
            unsigned  numberAudioEndpoints;                         // DPCD offset 22
            bool      overrideToSST;                                // force to SST even if MST capable
            bool      noLinkTraining;                               // DPCD offset 330h

            bool      extendedRxCapsPresent;                        // DPCD offset 000Eh [7] - Extended Receiver Capability present

            // DPCD Offset 2211h;
            unsigned  extendedSleepWakeTimeoutRequestMs;
            // DPCD Offset 0119h [0] - If we grant the extendedSleepWakeTimeoutRequest
            bool      bExtendedSleepWakeTimeoutGranted;

            bool      bFECSupported;

            // DPCD Offset F0002h - Number of Physical Repeaters present (after mapping) between Source and Sink
            unsigned  phyRepeaterCount;
            // DPCD offset 700 - EDP_DPCD_REV
            unsigned  eDpRevision;

            struct
            {
                unsigned  revisionMajor, revisionMinor;             // DPCD offset F0000h
                LinkRate  maxLinkRate;                              // DPCD offset F0001h
                unsigned  maxLaneCount;                             // DPCD offset F0004h
                unsigned  phyRepeaterExtendedWakeTimeoutMs;         // DPCD offset F0005h
                // The array to keep track of FEC capability of each LTTPR
                bool      bFECSupportedRepeater[NV_DPCD14_PHY_REPEATER_CNT_MAX];
                // If all the LTTPRs supports FEC
                bool      bFECSupported;
                bool      bAuxlessALPMSupported;                    // DPCD offset F0009
                // 0xE000D, DP Tunneling capabilities of DIA which acts as PHY Repeater
                bool      bDpTunnelingSupported;
                bool      bDpTunnelingBwAllocModeSupported;
            } repeaterCaps;

            struct
            {
                bool     bIsSupported;
                bool     bUsb4DriverBwAllocationSupport;
                bool     bIsPanelReplayOptimizationSupported;
                bool     bIsBwAllocationSupported;
                NvU8     maxLaneCount;
                LinkRate maxLinkRate;
            } dpInTunnelingCaps;

            PCONCaps pconCaps;
            vesaPsrSinkCaps psrCaps;
            NvU32    videoFallbackFormats;                          // DPCD offset 0200h

        } caps;

        // This is set by connectorImpl depending on the request from client/regkey
        bool bEnableDpTunnelBwAllocationSupport;
        bool bIsDpTunnelBwAllocationEnabled;                        // This is set to true after we succeed in enabling BW allocation

        struct
        {
            unsigned  sinkCount;                                    // DPCD offset 200
            bool      automatedTestRequest;
            bool      cpIRQ;
            bool      mccsIRQ;
            bool      downRepMsgRdy;
            bool      upReqMsgRdy;
            bool      prErrorStatus;                                // DPCD offset 2004h[3]
            bool      rxCapChanged;                                 // DPCD offset 2005
            bool      linkStatusChanged;                            // DPCD offset 2005
            bool      streamStatusChanged;                          // DPCD offset 2005
            bool      hdmiLinkStatusChanged;                        // DPCD offset 2005
            bool      dpTunnelingIrq;                               // DPCD offset 2005

            // DPCD offset 250 - 259
            NvU8      cstm80Bits[NV_DPCD_TEST_80BIT_CUSTOM_PATTERN__SIZE];
            struct
            {
                struct
                {
                    bool clockRecoveryDone;
                    bool channelEqualizationDone;
                    bool symbolLocked;
                } laneStatus[4];                                         // DPCD offset 202, 203

                bool interlaneAlignDone;                                 // DPCD offset 204
                bool downstmPortChng;
                bool linkStatusUpdated;

                //
                // (ESI specific) signifies that we have link trained and should
                // update the link status in the next query to isLinkLost. Keep in
                // mind that linkStatusChanged might still be zero.
                //
                bool linkStatusDirtied;
            } laneStatusIntr;

            struct
            {
                bool testRequestTraining;                          // DPCD offset 218
                LinkRate testRequestLinkRate;                      // DPCD offset 219
                unsigned testRequestLaneCount;                     // DPCD offset 220
            } testTraining;

            struct
            {
                bool testRequestEdidRead;                          // DPCD offset 218
            } testEdid;

            struct
            {
                bool                testRequestPattern;            // DPCD offset 218
                TestPatternType     testPatRequested;              // DPCD offset 221
                NvU16               testHorTotalPixels;            // DPCD offset 222, 223
                NvU16               testVerTotalLines;             // DPCD offset 224, 225
                NvU16               testHorStartPixels;            // DPCD offset 226, 227
                NvU16               testVerStartLines;             // DPCD offset 228, 229
                NvU16               testHsyncWidthPixels;          // DPCD offset 22A, 22B
                bool                testHsyncPolarity;
                NvU16               testVsyncWidthLines;           // DPCD offset 22C, 22D
                bool                testVsyncPolarity;
                NvU16               testActiveWidthPixels;         // DPCD offset 22E, 22F
                NvU16               testActiveHeightLines;         // DPCD offset 230, 231
            } testPattern;

            struct
            {
                bool testRequestPhyCompliance;                     // DPCD offset 218
                LinkQualityPatternType phyTestPattern;             // DPCD offset 248
            } testPhyCompliance;

        } interrupts;

        bool bIndexedLinkrateCapable, bIndexedLinkrateEnabled;

        public:
        DPCDHALImpl(AuxBus * bus, Timer * timer)
        : bus(bus), timer(timer), bGrantsPostLtRequest(false), uprequestEnable(false),
          upstreamIsSource(false), bMultistream(false), bGpuFECSupported(false),
          bBypassILREdpRevCheck(false), overrideDpcdMaxLinkRate(0),
          overrideDpcdRev(0), gpuDPSupportedVersions(0)
        {
            // start with default caps.
            dpcdOffline = true;

            //
            // fill out the bare minimum caps required ...
            // this should be extended in for more dpcd offsets in future.
            //
            caps.revisionMajor = 0x1;
            caps.revisionMinor = 0x1;
            caps.supportsESI = false;
            caps.maxLinkRate = dp2LinkRate_8_10Gbps;
            caps.maxLaneCount = 4;
            caps.enhancedFraming = true;
            caps.downStreamPortPresent = true;
            caps.downStreamPortCount = 1;

            // populate the sinkcount interrupt
            interrupts.sinkCount = 1;
        }

        ~DPCDHALImpl()
        {
        }

        virtual void setAuxBus(AuxBus * bus)
        {
            this->bus = bus;
        }

        bool isDpcdOffline()
        {
            return dpcdOffline;
        }

        void setDPCDOffline(bool bOffline)
        {
            dpcdOffline = bOffline;
        }

        void updateDPCDOffline();
        bool auxAccessAvailable();

        void setPC2Disabled(bool disabled)
        {
            pc2Disabled = disabled;
        }

        void setLttprSupported(bool isLttprSupported)
        {
            bLttprSupported = isLttprSupported;
        }

        bool isPC2Disabled()
        {
            return pc2Disabled;
        }

        virtual void parseAndReadCaps();
        virtual PCONCaps * getPCONCaps()
        {
            return &(caps.pconCaps);
        }

        // DPCD offset 0
        virtual unsigned getRevisionMajor()
        {
            return caps.revisionMajor;
        }

        virtual unsigned getRevisionMinor()
        {
            return caps.revisionMinor;
        }

        // DPCD offset F0000h
        virtual unsigned lttprGetRevisionMajor()
        {
            return caps.repeaterCaps.revisionMajor;
        }

        virtual unsigned lttprGetRevisionMinor()
        {
            return caps.repeaterCaps.revisionMinor;
        }

        virtual LinkRate getMaxLinkRate();

        virtual NvU32   getUHBRSupported()
        {
            return 0;
        }
        // DPCD offset 2
        virtual unsigned getMaxLaneCount();

        virtual bool getNoLinkTraining()
        {
            return caps.noLinkTraining;
        }

        virtual unsigned getPhyRepeaterCount()
        {
            return caps.phyRepeaterCount;
        }

        // Max lanes supported at the desired link rate.
        virtual unsigned getMaxLaneCountSupportedAtLinkRate(LinkRate linkRate);

        virtual bool getEnhancedFraming()
        {
            return caps.enhancedFraming;
        }

        // DPCD offset 5
        virtual bool getDownstreamPort(NvU8 *portType)
        {
            *portType = caps.downStreamPortType;
            return caps.downStreamPortPresent;
        }

        virtual bool getSupportsNoHandshakeTraining()
        {
            return caps.supportsNoHandshakeTraining;
        }

        // DPCD offset 7
        virtual unsigned getLegacyPortCount()
        {
            return caps.downStreamPortCount;
        }

        virtual LegacyPort * getLegacyPort(unsigned index)
        {
            return &legacyPort[index];
        }

        virtual bool getMsaTimingparIgnored()
        {
            return caps.msaTimingParIgnored;
        }

        virtual bool getOuiSupported()
        {
            return caps.ouiSupported;
        }

        virtual bool getSDPExtnForColorimetry();

        virtual bool getRootAsyncSDPSupported();

        virtual AuxRetry::status setOuiSource(unsigned ouiId, const char * model,
                                              size_t modelNameLength, NvU8 chipRevision);
        virtual bool getOuiSource(unsigned &ouiId, char * modelName,
                                  size_t modelNameBufferSize, NvU8 & chipRevision);
        virtual bool getOuiSink(unsigned &ouiId, unsigned char * modelName,
                                size_t modelNameBufferSize, NvU8 & chipRevision);

        // DPCD offset 21h
        virtual bool getSupportsMultistream()
        {
            return caps.supportsMultistream && (!caps.overrideToSST);
        }

        virtual void setSupportsESI(bool bIsESISupported)
        {
            caps.supportsESI = bIsESISupported;
        }

        //
        // Single stream specific caps
        // DPCD offset 22h
        //
        virtual unsigned getNumberOfAudioEndpoints();

        // DPCD offset 30h
        virtual bool getGUID(GUID & guid);
        virtual AuxRetry::status setGUID(GUID & guid);

        void parsePortDescriptors();

        //
        //  Notifications of external events
        //
        virtual void notifyIRQ()
        {
            parseAndReadInterrupts();
        }

        virtual void populateFakeDpcd();

        // DPCD override routine: Max link rate override.
        void overrideMaxLinkRate(NvU32 overrideMaxLinkRate);

        // DPCD override routine: Max lane count override.
        void overrideMaxLaneCount(NvU32 maxLaneCount)
        {
            caps.maxLaneCount = maxLaneCount;
            overrideDpcdMaxLaneCount = maxLaneCount;
        }

        // DPCD override routine: Max lane count override at a given link rate.
        void skipCableBWCheck(NvU32 maxLaneAtHighRate, NvU32 maxLaneAtLowRate)
        {
            caps.maxLanesAtHBR = maxLaneAtHighRate;
            caps.maxLanesAtRBR = maxLaneAtLowRate;
        }

        // DPCD override routine: Optimal link config (link rate and lane count) override.
        void overrideOptimalLinkCfg(LinkRate optimalLinkRate,
                                    NvU32 optimalLaneCount)
        {
            caps.maxLinkRate = optimalLinkRate;
            caps.maxLaneCount = optimalLaneCount;
        }

        // DPCD override routine: Optimal link rate
        void overrideOptimalLinkRate(LinkRate optimalLinkRate)
        {
            caps.maxLinkRate = optimalLinkRate;
        }

        virtual void notifyHPD(bool status, bool bSkipDPCDRead);
        virtual bool isPostLtAdjustRequestSupported()
        {
            //
            // If the upstream DPTX and downstream DPRX both support TPS4,
            // TPS4 shall be used instead of POST_LT_ADJ_REQ.
            //
            NvBool bTps4Supported = FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_DP_VERSIONS_SUPPORTED,
                                                 _DP1_4, _YES, gpuDPSupportedVersions) &&
                                    caps.bSupportsTPS4;
            return bGrantsPostLtRequest && !bTps4Supported;
        }

        virtual void setPostLtAdjustRequestGranted(bool bGrantPostLtRequest);
        virtual bool getIsPostLtAdjRequestInProgress();
        virtual TrainingPatternSelectType getTrainingPatternSelect();
        virtual bool setTrainingMultiLaneSet(NvU8 numLanes,
                                             NvU8 *voltSwingSet,
                                             NvU8 *preEmphasisSet);

        virtual AuxRetry::status setIgnoreMSATimingParamters(bool msaTimingParamIgnoreEn);

        virtual AuxRetry::status setLinkQualPatternSet(LinkQualityPatternType linkQualPattern, unsigned laneCount);
        virtual AuxRetry::status setLinkQualPatternSet(DP2xPatternInfo& patternInfo, unsigned laneCount);
        virtual AuxRetry::status setLinkQualLaneSet(unsigned lane, LinkQualityPatternType linkQualPattern);

        virtual AuxRetry::status setMessagingEnable(bool _uprequestEnable, bool _upstreamIsSource);
        virtual AuxRetry::status setMultistreamLink(bool enable);
        virtual AuxRetry::status setMultistreamHotplugMode(MultistreamHotplugMode notifyType);

        virtual bool parseTestRequestTraining(NvU8 * buffer /* 0x18-0x28 valid */);
        void parseAutomatedTestRequest(bool testRequestPending);

        virtual bool parseTestRequestPhy();

        virtual bool interruptCapabilitiesChanged()
        {
            return interrupts.rxCapChanged;
        }

        virtual void clearInterruptCapabilitiesChanged()
        {
            NvU8 irqVector = 0;
            irqVector = FLD_SET_DRF(_DPCD, _LINK_SERVICE_IRQ_VECTOR_ESI0, _RX_CAP_CHANGED, _YES, irqVector);
            bus.write(NV_DPCD_LINK_SERVICE_IRQ_VECTOR_ESI0, &irqVector, sizeof irqVector);
        }

        virtual bool isPanelReplayErrorSet()
        {
            return interrupts.prErrorStatus;
        }

        virtual void readPanelReplayError();
        virtual void clearPanelReplayError()
        {
            NvU8 irqVector = 0U;
            irqVector = FLD_SET_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR_ESI1,
                                    _PANEL_REPLAY_ERROR_STATUS, _YES, irqVector);
            bus.write(NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI1, &irqVector,
                      sizeof irqVector);
        }

        virtual bool getLinkStatusChanged()
        {
            return interrupts.linkStatusChanged;
        }

        virtual void clearLinkStatusChanged()
        {
            NvU8 irqVector = 0;
            irqVector = FLD_SET_DRF(_DPCD, _LINK_SERVICE_IRQ_VECTOR_ESI0, _LINK_STATUS_CHANGED, _YES, irqVector);
            bus.write(NV_DPCD_LINK_SERVICE_IRQ_VECTOR_ESI0, &irqVector, sizeof irqVector);
        }

        virtual bool getHdmiLinkStatusChanged()
        {
            return interrupts.hdmiLinkStatusChanged;
        }

        virtual void clearHdmiLinkStatusChanged()
        {
            NvU8 irqVector = 0;
            irqVector = FLD_SET_DRF(_DPCD, _LINK_SERVICE_IRQ_VECTOR_ESI0, _HDMI_LINK_STATUS_CHANGED, _YES, irqVector);
            bus.write(NV_DPCD_LINK_SERVICE_IRQ_VECTOR_ESI0, &irqVector, sizeof irqVector);
        }

        virtual bool getStreamStatusChanged()
        {
            return interrupts.streamStatusChanged;
        }

        virtual void clearStreamStatusChanged()
        {
            NvU8 irqVector = 0;
            irqVector = FLD_SET_DRF(_DPCD, _LINK_SERVICE_IRQ_VECTOR_ESI0, _STREAM_STATUS_CHANGED, _YES, irqVector);
            bus.write(NV_DPCD_LINK_SERVICE_IRQ_VECTOR_ESI0, &irqVector, sizeof irqVector);
        }

        virtual bool getDpTunnelingIrq()
        {
            return interrupts.dpTunnelingIrq;
        }

        virtual void clearDpTunnelingIrq()
        {
            NvU8 irqVector = 0;
            irqVector = FLD_SET_DRF(_DPCD20, _LINK_SERVICE_IRQ_VECTOR_ESI0, _DP_TUNNELING_IRQ, _YES, irqVector);
            bus.write(NV_DPCD20_LINK_SERVICE_IRQ_VECTOR_ESI0, &irqVector, sizeof irqVector);
        }

        virtual bool isLinkStatusValid(unsigned lanes);
        virtual void refreshLinkStatus();
        virtual void setDirtyLinkStatus(bool dirty)
        {
            interrupts.laneStatusIntr.linkStatusDirtied = dirty;
        }

        void parseAndReadInterruptsESI();

        void readLTTPRLinkStatus(NvS32 rxIndex, NvU8 *buffer);
        void resetIntrLaneStatus();

        void fetchLinkStatusESI();
        void fetchLinkStatusLegacy();

        virtual bool readTraining(NvU8* voltageSwingLane,  NvU8* preemphasisLane,
                                  NvU8* trainingScoreLane, NvU8* postCursor,
                                  NvU8  activeLaneCount);

        virtual bool isLaneSettingsChanged(NvU8* oldVoltageSwingLane,
                                           NvU8* newVoltageSwingLane,
                                           NvU8* oldPreemphasisLane,
                                           NvU8* newPreemphasisLane,
                                           NvU8 activeLaneCount);

        void parseAndReadInterruptsLegacy();

        void parseAndReadInterrupts()
        {
            if (caps.supportsESI)
                parseAndReadInterruptsESI();       // DP 1.2 should use the new ESI region
            else
                parseAndReadInterruptsLegacy();

        }

        virtual int getSinkCount() // DPCD offset 200
        {
            return interrupts.sinkCount;
        }

        //
        // This was introduced as part of WAR for HP SDC Panel since their
        // TCON sets DPCD 0x200 SINK_COUNT=0. It should never be called to
        // set the SinkCount in other cases since SinkCount comes from DPCD.
        //
        virtual void setSinkCount(int sinkCount)
        {
            interrupts.sinkCount = sinkCount;
        }

        virtual bool interruptContentProtection()
        {
            return interrupts.cpIRQ;
        }

        virtual void clearInterruptContentProtection();

        virtual bool intteruptMCCS()
        {
            return interrupts.mccsIRQ;
        }

        virtual void clearInterruptMCCS();

        virtual bool interruptDownReplyReady()
        {
            return interrupts.downRepMsgRdy;
        }

        virtual bool interruptUpRequestReady()
        {
            return interrupts.upReqMsgRdy;
        }

        virtual void clearInterruptDownReplyReady();
        virtual void clearInterruptUpRequestReady();

        virtual bool getLaneStatusSymbolLock(int lane)
        {
            return interrupts.laneStatusIntr.laneStatus[lane].symbolLocked;
        }

        virtual bool getLaneStatusClockRecoveryDone(int lane)
        {
            return interrupts.laneStatusIntr.laneStatus[lane].clockRecoveryDone;
        }

        virtual bool getLaneStatusChannelEqualizationDone(int lane)
        {
            return interrupts.laneStatusIntr.laneStatus[lane].channelEqualizationDone;
        }

        virtual bool getInterlaneAlignDone()                                             // DPCD offset 204
        {
            return interrupts.laneStatusIntr.interlaneAlignDone;
        }

        virtual bool getDownStreamPortStatusChange()
        {
            return interrupts.laneStatusIntr.downstmPortChng;
        }

        virtual bool getPendingTestRequestTraining()                                    // DPCD offset 218
        {
            return interrupts.testTraining.testRequestTraining;
        }

        virtual bool getPendingAutomatedTestRequest()
        {
            return interrupts.automatedTestRequest;
        }

        virtual bool getPendingTestRequestEdidRead()
        {
            return interrupts.testEdid.testRequestEdidRead;
        }

        virtual bool getPendingTestRequestPhyCompliance()
        {
            return interrupts.testPhyCompliance.testRequestPhyCompliance;
        }

        // DPCD offset 219, 220
        virtual void getTestRequestTraining(LinkRate & rate, unsigned & lanes)
        {
            rate = interrupts.testTraining.testRequestLinkRate;
            lanes = interrupts.testTraining.testRequestLaneCount;
        }

        // DPCD offset 248
        virtual LinkQualityPatternType getPhyTestPattern()
        {
            return interrupts.testPhyCompliance.phyTestPattern;
        }

        // DPCD offset 250 - 259
        virtual void get80BitsCustomTestPattern(NvU8 *testPattern)
        {
            int i;

            for (i = 0; i < 10; i++)
            {
                testPattern[i] = interrupts.cstm80Bits[i];
            }
        }

        // DPCD offset 2230 - 2250
        virtual void get264BitsCustomTestPattern(NvU8 *testPattern)
        {
            DP_ASSERT(0 && "DP1x should never get this request.");
            return;
        }

        // DPCD offset 249h
        virtual void getSquarePatternNum(NvU8 *sqNum)
        {
            DP_ASSERT(0 && "DP1x should never get this request.");
            return;
        }

        virtual bool getBKSV(NvU8 *bKSV);
        virtual bool getBCaps(BCaps &bCaps, NvU8 * rawByte);
        virtual bool getHdcp22BCaps(BCaps &bCaps, NvU8 *rawByte);
        virtual bool getBinfo(BInfo &bInfo);
        virtual bool getRxStatus(const HDCPState &hdcpState, NvU8 *data);

        virtual AuxRetry::status setTestResponseChecksum(NvU8 checksum)
        {
            if (caps.revisionMajor <= 0)
                DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

            return bus.write(NV_DPCD_TEST_EDID_CHKSUM, &checksum, sizeof checksum);
        }

        virtual AuxRetry::status setTestResponse(bool ack, bool edidChecksumWrite);

        //  Message box encoding
        virtual AuxRetry::status writeDownRequestMessageBox(NvU8 * data, size_t length)
        {
            //
            //  We can assume no message was sent if this fails.
            //     Reasoning:
            //        Sinks are not allowed to DEFER except on the first 16 byte write.
            //        If there isn't enough room for the 48 byte packet, that write
            //        will defer.
            //
            return bus.write(NV_DPCD_MBOX_DOWN_REQ, data, (unsigned)length);
        }

        virtual size_t getDownRequestMessageBoxSize()
        {
            return DP_MESSAGEBOX_SIZE;
        }

        virtual AuxRetry::status writeUpReplyMessageBox(NvU8 * data, size_t length)
        {
            if (caps.revisionMajor <= 0)
                DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

            //
            //  We can assume no message was sent if this fails.
            //     Reasoning:
            //        Sinks are not allowed to DEFER except on the first 16 byte write.
            //        If there isn't enough room for the 48 byte packet, that write
            //        will defer.
            //
            return bus.write(NV_DPCD_MBOX_UP_REP, data, (unsigned)length);
        }

        virtual size_t getUpReplyMessageBoxSize()
        {
            return DP_MESSAGEBOX_SIZE;
        }

        virtual AuxRetry::status readDownReplyMessageBox(NvU32 offset, NvU8 * data, size_t length)
        {
            //  if (caps.revisionMajor <= 0)
            //        DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

            DP_ASSERT(offset + length <= DP_MESSAGEBOX_SIZE);

            return bus.read(NV_DPCD_MBOX_DOWN_REP + offset, data, (unsigned)length);
        }

        virtual size_t getDownReplyMessageBoxSize()
        {
            return DP_MESSAGEBOX_SIZE;
        }

        virtual  AuxRetry::status readUpRequestMessageBox(NvU32 offset, NvU8 * data, size_t length)
        {
            if (caps.revisionMajor <= 0)
                DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

            DP_ASSERT(offset + length <= DP_MESSAGEBOX_SIZE);

            return bus.read(NV_DPCD_MBOX_UP_REQ + offset, data, (unsigned)length);
        }

        virtual size_t getUpRequestMessageBoxSize()
        {
            return DP_MESSAGEBOX_SIZE;
        }

        virtual size_t getTransactionSize()
        {
            return bus.getDirect()->transactionSize();
        }

        virtual PowerState getPowerState();
        virtual bool setPowerState(PowerState newState);
        virtual void payloadTableClearACT();
        virtual bool payloadWaitForACTReceived();
        virtual bool payloadAllocate(unsigned streamId, unsigned begin, unsigned count);

        void overrideMultiStreamCap(bool mstCapable)
        {
            caps.overrideToSST = !mstCapable;
        }

        bool getMultiStreamCapOverride()
        {
            return caps.overrideToSST;
        }

        bool getDpcdMultiStreamCap(void)
        {
            return caps.supportsMultistream;
        }

        virtual void setGpuDPSupportedVersions(NvU32 _gpuDPSupportedVersions);

        void setGpuFECSupported(bool bSupportFEC)
        {
            bGpuFECSupported = bSupportFEC;
        }

        void applyRegkeyOverrides(const DP_REGKEY_DATABASE& dpRegkeyDatabase);

        // To clear pending message {DOWN_REP/UP_REQ} and reply true if existed.
        virtual bool clearPendingMsg();

        virtual bool isMessagingEnabled();

        virtual void setIndexedLinkrateEnabled(bool val)
        {
            bIndexedLinkrateEnabled = val;
        }

        virtual bool isIndexedLinkrateEnabled()
        {
            return bIndexedLinkrateEnabled;
        }

        virtual bool isIndexedLinkrateCapable()
        {
            return bIndexedLinkrateCapable;
        }

        virtual NvU16 *getLinkRateTable();

        virtual NvU32 getVideoFallbackSupported()
        {
            return caps.videoFallbackFormats;
        }

        virtual bool getRawLinkRateTable(NvU8 *buffer);

        virtual void resetProtocolConverter()
        {
            NvU8    data = 0;
            bus.write(NV_DPCD14_PCON_FRL_LINK_CONFIG_1, &data, sizeof(data));
            bus.write(NV_DPCD14_PCON_FRL_LINK_CONFIG_2, &data, sizeof(data));
        }

        virtual bool setSourceControlMode(bool bEnableSourceControlMode, bool bEnableFRLMode);

        virtual bool checkPCONFrlReady(bool *bFrlReady);

        virtual bool setupPCONFrlLinkAssessment(NvU32   linkBwMask,
                                                bool    bEnableExtendLTMode = false,
                                                bool    bEnableConcurrentMode = false);

        virtual bool checkPCONFrlLinkStatus(NvU32 *frlRateMask);
        virtual bool queryHdmiLinkStatus(bool *bLinkActive, bool *bLinkReady);

        virtual NvU32 restorePCONFrlLink(NvU32   linkBwMask,
                                        bool    bEnableExtendLTMode     = false,
                                        bool    bEnableConcurrentMode   = false);

        virtual void readPsrCapabilities(vesaPsrSinkCaps *caps)
        {
            dpMemCopy(caps, &this->caps.psrCaps, sizeof(vesaPsrSinkCaps));
        }

        virtual bool updatePsrConfiguration(vesaPsrConfig psrcfg);
        virtual bool readPsrConfiguration(vesaPsrConfig *psrcfg);

        virtual bool readPsrState(vesaPsrState *psrState);
        virtual bool readPsrDebugInfo(vesaPsrDebugStatus *psrDbgState);

        virtual bool writePsrErrorStatus(vesaPsrErrorStatus psrErr);
        virtual bool readPsrErrorStatus(vesaPsrErrorStatus *psrErr);

        virtual bool writePsrEvtIndicator(vesaPsrEventIndicator psrEvt);
        virtual bool readPsrEvtIndicator(vesaPsrEventIndicator *psrEvt);

        virtual bool readPrSinkDebugInfo(panelReplaySinkDebugInfo *prDbgInfo);

        virtual void     configureDpTunnelBwAllocation();
        virtual bool     getDpTunnelGranularityMultiplier(NvU8 &granularityMultiplier);
        virtual TriState getDpTunnelBwRequestStatus();
        virtual bool     setDpTunnelBwAllocation(bool bEnable);

        virtual void     enableDpTunnelingBwAllocationSupport()
        {
            bEnableDpTunnelBwAllocationSupport = true;
        }

        virtual bool isDpTunnelBwAllocationEnabled()
        {
            return bIsDpTunnelBwAllocationEnabled;
        }

        bool getDpTunnelEstimatedBw(NvU8 &estimatedBw);
        bool hasDpTunnelEstimatedBwChanged();
        bool hasDpTunnelBwAllocationCapabilityChanged();
        bool writeDpTunnelRequestedBw(NvU8 requestedBw);
        bool clearDpTunnelingBwRequestStatus();
        bool clearDpTunnelingEstimatedBwStatus();
        bool clearDpTunnelingBwAllocationCapStatus();

        virtual AuxRetry::status notifySDPErrDetectionCapability() { return AuxRetry::ack; }
        virtual bool isDp2xChannelCodingCapable() { return false; }
        virtual void setIgnoreCableIdCaps(bool bIgnore) { return; }
        virtual void overrideCableIdCap(LinkRate linkRate, bool bEnable) { return; }
        virtual void setCableVconnSourceUnknown() { return; }

        // implement this function if DPCDHALImpl needs updated state between hotunplug/plug
        virtual void initialize(){}
        virtual AuxRetry::status setMainLinkChannelCoding(MainLinkChannelCoding channelCoding){ return AuxRetry::ack; }
        virtual MainLinkChannelCoding getMainLinkChannelCoding() { return ChannelCoding8B10B; }
        virtual void setConnectorTypeC(bool bTypeC) {};
        virtual void setUSBCCableIDInfo(NV0073_CTRL_DP_USBC_CABLEID_INFO *cableIDInfo) {}
    };

}

#endif //INCLUDED_DP_CONFIGCAPS_H
