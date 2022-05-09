/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
        virtual AuxRetry::status       setLinkQualLaneSet(unsigned lane, LinkQualityPatternType linkQualPattern) = 0;
        virtual AuxRetry::status setLinkQualPatternSet(LinkQualityPatternType linkQualPattern, unsigned laneCount = 0) = 0;
    };

    class LinkCapabilities
    {
    public:

        //
        //  Physical layer feature set
        //
        virtual NvU64    getMaxLinkRate() = 0;                                // Maximum byte-block in Hz
        virtual unsigned getMaxLaneCount() = 0;                               // DPCD offset 2
        virtual unsigned getMaxLaneCountSupportedAtLinkRate(LinkRate linkRate) = 0;
        virtual bool     getEnhancedFraming() = 0;
        virtual bool     getSupportsNoHandshakeTraining() = 0;
        virtual bool     getMsaTimingparIgnored() = 0;
        virtual bool     getDownstreamPort(NvU8 *portType) = 0;               // DPCD offset 5
        virtual bool     getSupportsMultistream() = 0;                        // DPCD offset 21h
        virtual bool     getNoLinkTraining() = 0;                             // DPCD offset 330h
        virtual unsigned getPhyRepeaterCount() = 0;                           // DPCD offset F0002h
    };

    class OUI
    {
    public:
        virtual bool             getOuiSupported() = 0;
        virtual AuxRetry::status setOuiSource(unsigned ouiId, const char * model, size_t modelNameLength, NvU8 chipRevision) = 0;
        virtual bool             getOuiSink(unsigned &ouiId, char * modelName, size_t modelNameBufferSize, NvU8 & chipRevision) = 0;
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

        bool  isAtLeastVersion(unsigned major, unsigned minor)
        {
            if (getRevisionMajor() > major)
                return true;

            if (getRevisionMajor() < major)
                return false;

            return getRevisionMinor() >= minor;
        }

        bool  isVersion(unsigned major, unsigned minor)
        {
            if ((getRevisionMajor() == major) &&
                (getRevisionMinor() == minor))
                return true;

            return false;
        }

        bool  lttprIsAtLeastVersion(unsigned major, unsigned minor)
        {
            if (lttprGetRevisionMajor() > major)
                return true;

            if (lttprGetRevisionMinor() < major)
                return false;

            return lttprGetRevisionMinor() >= minor;
        }

        bool  lttprIsVersion(unsigned major, unsigned minor)
        {
            if ((lttprGetRevisionMajor() == major) &&
                (lttprGetRevisionMinor() == minor))
                return true;

            return false;
        }

        // Convert Link Bandwidth read from DPCD register to Linkrate
        NvU64 mapLinkBandiwdthToLinkrate(NvU32 linkBandwidth)
        {
            if (FLD_TEST_DRF(_DPCD, _MAX_LINK_BANDWIDTH, _VAL, _1_62_GBPS, linkBandwidth))
                return RBR;
            else if (FLD_TEST_DRF(_DPCD, _MAX_LINK_BANDWIDTH, _VAL, _2_70_GBPS, linkBandwidth))
                return HBR;
            else if (FLD_TEST_DRF(_DPCD, _MAX_LINK_BANDWIDTH, _VAL, _5_40_GBPS, linkBandwidth))
                return HBR2;
            else if (FLD_TEST_DRF(_DPCD14, _MAX_LINK_BANDWIDTH, _VAL, _8_10_GBPS, linkBandwidth))
                return HBR3;
            else
            {
                DP_ASSERT(0 && "Unknown link bandwidth. Assuming HBR");
                return HBR;
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
        virtual AuxRetry::status     setMultistreamHotplugMode(MultistreamHotplugMode notifyType) = 0;

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

        virtual bool             getHdmiLinkStatusChanged() = 0;
        virtual void             clearHdmiLinkStatusChanged() = 0;

        virtual bool             getStreamStatusChanged() = 0;
        virtual void             clearStreamStatusChanged() =0;

        virtual void             setDirtyLinkStatus(bool dirty) = 0;
        virtual void             refreshLinkStatus() = 0;
        virtual bool             isLinkStatusValid(unsigned lanes) = 0;

        virtual void getCustomTestPattern(NvU8 *testPattern) = 0;                         // DPCD offset 250 - 259

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
        virtual void setGpuDPSupportedVersions(bool supportDp1_2, bool supportDp1_4) = 0;

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

        virtual ~DPCDHAL() {}

    };

    //
    //  Implement interface
    //
    DPCDHAL * MakeDPCDHAL(AuxBus *  bus, Timer * timer);
}

#endif //INCLUDED_DP_CONFIGCAPS_H
