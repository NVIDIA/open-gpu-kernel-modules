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

/******************************* DisplayPort********************************\
*                                                                           *
* Module: dp_configcaps.cpp                                                 *
*    Abstraction for basic caps registers                                   *
*                                                                           *
\***************************************************************************/
#include "dp_internal.h"
#include "dp_auxbus.h"
#include "dp_configcaps.h"
#include "dp_auxdefs.h"
#include "displayport.h"

using namespace DisplayPort;

struct DPCDHALImpl : DPCDHAL
{
    AuxRetry  bus;
    Timer    * timer;
    bool      dpcdOffline;
    bool      gpuDP1_2Supported;
    bool      gpuDP1_4Supported;
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

        // 0x2206, if the sink supports 128b/132b
        bool      bDP20ChannelCodingSupported;
        // 0x2215
        bool      bUHBR_10GSupported;
        bool      bUHBR_13_5GSupported;
        bool      bUHBR_20GSupported;


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

            // 0xF0006, if the PHY Repeater supports 128b/132b
            bool      bDP20ChannelCodingSupported;
            // 0xF0007
            bool      UHBR_10GSupported;
            bool      UHBR_13_5GSupported;
            bool      UHBR_20GSupported;
        } repeaterCaps;

        PCONCaps pconCaps;
        vesaPsrSinkCaps psrCaps;
        NvU32    videoFallbackFormats;                          // DPCD offset 0200h

    } caps;

    struct
    {
        unsigned  sinkCount;                                    // DPCD offset 200
        bool      automatedTestRequest;
        bool      cpIRQ;
        bool      mccsIRQ;
        bool      downRepMsgRdy;
        bool      upReqMsgRdy;
        bool      rxCapChanged;                                 // DPCD offset 2005
        bool      linkStatusChanged;                            // DPCD offset 2005
        bool      streamStatusChanged;                          // DPCD offset 2005
        bool      hdmiLinkStatusChanged;                        // DPCD offset 2005
        NvU8      eightyBitCustomPat[10];                       // DPCD offset 250 - 259

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
    : bus(bus),
    timer(timer),
    gpuDP1_2Supported(false),
    gpuDP1_4Supported(false),
    bGrantsPostLtRequest(false),
    uprequestEnable(false),
    upstreamIsSource(false),
    bMultistream(false),
    bGpuFECSupported(false),
    bBypassILREdpRevCheck(false),
    overrideDpcdMaxLinkRate(0),
    overrideDpcdRev(0)
    {
        // start with default caps.
        populateFakeDpcd();
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

    void updateDPCDOffline()
    {
        NvU8 buffer[16];
        unsigned retries = 16;
        // Burst read from 0x00 to 0x0F.
        if (AuxRetry::ack != bus.read(NV_DPCD_REV, &buffer[0], sizeof buffer, retries))
        {
            dpcdOffline = true;
        }
        else
        {
            dpcdOffline = false;
        }
    }

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
    void parseAndReadCaps()
    {
        NvU8 buffer[16];
        NvU8 byte = 0;
        AuxRetry::status status;
        unsigned retries = 16;
        // Burst read from 0x00 to 0x0F.

        //
        // The Extended Receiver Capability field at DPCD Addresses 02200h through 022FFh is valid
        // with DPCD Rev. 1.4 (and higher).
        //
        // A DPRX that supports the Extended Receiver Capability field must set the
        // EXTENDED_RECEIVER_CAPABILITY_FIELD_PRESENT bit in the TRAINING_AUX_RD_INTERVAL
        // register (DPCD Address 0000Eh, bit 7) to 1
        //
        caps.extendedRxCapsPresent = false;
        if (AuxRetry::ack == bus.read(NV_DPCD_TRAINING_AUX_RD_INTERVAL, &byte, sizeof byte))
        {
            caps.extendedRxCapsPresent = DRF_VAL(_DPCD14, _TRAINING_AUX_RD_INTERVAL, _EXTENDED_RX_CAP, byte);
        }

        if (caps.extendedRxCapsPresent)
        {
            status = bus.read(NV_DPCD14_EXTENDED_REV, &buffer[0], sizeof buffer, retries);
        }
        else
        {
            status = bus.read(NV_DPCD_REV, &buffer[0], sizeof buffer, retries);
        }

        if (AuxRetry::ack != status)
        {
            // Failed to read caps.
            // Set an invalid state here and make sure we REMEMBER we couldn't get the caps
            caps.revisionMajor = 0;
            dpcdOffline = true;
            return;
        }

        // reset the faked dpcd flag since real LT should be possible now.
        dpcdOffline = false;

        // reset edp revision to 0
        caps.eDpRevision = 0;

        if (overrideDpcdRev)
        {
            // Override the revision no. as DPCD override regkey is set
            caps.revisionMajor = DRF_VAL(_DPCD, _REV, _MAJOR, overrideDpcdRev);
            caps.revisionMinor = DRF_VAL(_DPCD, _REV, _MINOR, overrideDpcdRev);
        }
        else
        {
            caps.revisionMajor = DRF_VAL(_DPCD, _REV, _MAJOR, buffer[0]);
            caps.revisionMinor = DRF_VAL(_DPCD, _REV, _MINOR, buffer[0]);
            if (isAtLeastVersion(1, 2))
            {
                //
                // WAR required for panels with MSTAR chip as they report themselves as
                // DP1.2 but they don't support DP1.2. Check OUI & ESI sinkCount. if OUI
                // is not supported & sinkCount is "0", downgrade the revision to 1.1.
                //
                if (FLD_TEST_DRF(_DPCD, _DOWN_STREAM_PORT, _OUI_SUPPORT, _NO, buffer[7]))
                {
                    // Read the ESI sinkCount & overwrite revision no. if ESI not supported
                    NvU8 esiBuffer[1] = {0};
                    NvU32 sinkCount;
                    AuxRetry::status status;
                    //
                    // Don't just check the transaction status as not-supporting ESI means it may
                    // NACK a transaction to ESI space or may return "0" as sinkCount. We need
                    // to override the revision Minor in both cases.
                    //
                    status = bus.read(NV_DPCD_SINK_COUNT_ESI, &esiBuffer[0], sizeof esiBuffer);
                    sinkCount = DRF_VAL(_DPCD, _SINK_COUNT_ESI, _SINK_COUNT, esiBuffer[0]);

                    if ((sinkCount == 0) || (status != AuxRetry::ack))
                    {
                        // If ESI not supported then overwrite the revision
                        caps.revisionMajor = 1;
                        caps.revisionMinor = 1;
                    }
                }

                // Check if DPCD_DISPLAY_CONTROL_CAPABLE = 1
                if (FLD_TEST_DRF(_DPCD, _EDP_CONFIG_CAP, _DISPLAY_CONTROL_CAPABLE, _YES, buffer[0x0D]))
                {
                    NvU8 edpBuffer[1] = {0};
                    status = bus.read(NV_DPCD_EDP_REV, &edpBuffer[0], sizeof edpBuffer);
                    caps.eDpRevision = DRF_VAL(_DPCD, _EDP, _REV_VAL, edpBuffer[0]);
                }
            }
        }

        bIndexedLinkrateCapable = false;

        if (isAtLeastVersion(1,4) && caps.extendedRxCapsPresent == false)
        {
            DP_ASSERT(0 && "A DPRX with DPCD Rev. 1.4 (or higher) must have Extended Receiver Capability field.");
        }

        caps.supportsESI = (isAtLeastVersion(1,2) && gpuDP1_2Supported);     // Support ESI register space only when GPU support DP1.2MST

        if (caps.eDpRevision >= NV_DPCD_EDP_REV_VAL_1_4 || this->bBypassILREdpRevCheck)
        {
            NvU16 linkRate = 0;
            if (getRawLinkRateTable((NvU8*)&caps.linkRateTable[0]))
            {
                // First entry must be non-zero for validation
                if (caps.linkRateTable[0] != 0)
                {
                    bIndexedLinkrateCapable = true;
                    for (int i = 0; caps.linkRateTable[i] && (i < NV_DPCD_SUPPORTED_LINK_RATES__SIZE); i++)
                    {
                        if (linkRate < caps.linkRateTable[i])
                            linkRate = caps.linkRateTable[i];
                    }
                    if (linkRate)
                        caps.maxLinkRate = LINK_RATE_KHZ_TO_MBPS((NvU64)linkRate * DP_LINK_RATE_TABLE_MULTIPLIER_KHZ);
                }
            }
        }
        if (!bIndexedLinkrateCapable)
        {
            if (FLD_TEST_DRF(_DPCD, _MAX_LINK_BANDWIDTH, _VAL, _1_62_GBPS, buffer[1]))
                caps.maxLinkRate = RBR;
            else if (FLD_TEST_DRF(_DPCD, _MAX_LINK_BANDWIDTH, _VAL, _2_70_GBPS, buffer[1]))
               caps.maxLinkRate = HBR;
            else if (FLD_TEST_DRF(_DPCD, _MAX_LINK_BANDWIDTH, _VAL, _5_40_GBPS, buffer[1]))
                caps.maxLinkRate = HBR2;
            else if (FLD_TEST_DRF(_DPCD14, _MAX_LINK_BANDWIDTH, _VAL, _8_10_GBPS, buffer[1]))
                caps.maxLinkRate = HBR3;
            else
            {
                DP_ASSERT(0 && "Unknown max link rate. Assuming DP 1.1 defaults");
                caps.maxLinkRate = HBR;
            }
        }

        //
        // To prevent WAR being overridden.
        //
        if (overrideDpcdMaxLaneCount)
        {
            caps.maxLaneCount = overrideDpcdMaxLaneCount;
        }
        else
        {
            caps.maxLaneCount = DRF_VAL(_DPCD, _MAX_LANE_COUNT, _LANE, buffer[0x2]);
        }

        if (!IS_VALID_LANECOUNT(caps.maxLaneCount))
        {
            DP_ASSERT(0 && "Invalid lane count.  Assuming 1");
            caps.maxLaneCount = 1;
        }

        caps.bPostLtAdjustmentSupport = FLD_TEST_DRF(_DPCD, _MAX_LANE_COUNT, _POST_LT_ADJ_REQ_SUPPORT, _YES, buffer[0x2]);
        caps.enhancedFraming          = FLD_TEST_DRF(_DPCD, _MAX_LANE_COUNT, _ENHANCED_FRAMING, _YES, buffer[0x2]);
        if (isAtLeastVersion(1,1) && (!caps.enhancedFraming))
        {
            DP_ASSERT(0 && "A DPRX with DPCD Rev. 1.1 (or higher) must have enhanced framing capability.");
        }

        if (isAtLeastVersion(1,2) && gpuDP1_2Supported && caps.bPostLtAdjustmentSupport)
        {
            // Source grants post Link training adjustment support
            bGrantsPostLtRequest = true;
        }
        else
        {
            // Disable post Link training adjustment support whenever sink does not report capability
            // This covers the case of MST to SST transition during which initially this flag is set, we need to explicitly reset this
            // in order to avoid PostLTAdjustment during LT.
            bGrantsPostLtRequest = false;
        }

        caps.supportsNoHandshakeTraining = FLD_TEST_DRF(_DPCD, _MAX_DOWNSPREAD, _NO_AUX_HANDSHAKE_LT, _TRUE, buffer[0x3]);
        caps.bSupportsTPS4               = FLD_TEST_DRF(_DPCD14, _MAX_DOWNSPREAD, _TPS4_SUPPORTED, _YES, buffer[0x3]);

        caps.NORP = DRF_VAL(_DPCD, _NORP, _VAL, buffer[0x4]) + 1;

        caps.downStreamPortPresent = FLD_TEST_DRF(_DPCD, _DOWNSTREAMPORT, _PRESENT, _YES, buffer[0x5]);
        caps.detailedCapInfo       = FLD_TEST_DRF(_DPCD, _DOWNSTREAMPORT, _DETAILED_CAP_INFO_AVAILABLE, _YES, buffer[0x5]);
        caps.downStreamPortType    = DRF_VAL(_DPCD, _DOWNSTREAMPORT, _TYPE, buffer[0x5]);

        switch (DRF_VAL(_DPCD, _DOWNSTREAMPORT, _TYPE, buffer[0x5]))
        {
            case 0: legacyPort[0].type = DISPLAY_PORT;    break;
            case 1: legacyPort[0].type = ANALOG_VGA;      break;
            case 2: legacyPort[0].type = DVI;             break;
            case 3: legacyPort[0].type = WITHOUT_EDID;    break;
            default: DP_ASSERT(0 && "Unknown port type");    break;
        }

        caps.downStreamPortCount = DRF_VAL(_DPCD, _DOWN_STREAM_PORT, _COUNT, buffer[0x7]);
        caps.msaTimingParIgnored = FLD_TEST_DRF(_DPCD, _DOWN_STREAM_PORT, _MSA_TIMING_PAR_IGNORED, _YES, buffer[0x7]);
        caps.ouiSupported        = FLD_TEST_DRF(_DPCD, _DOWN_STREAM_PORT, _OUI_SUPPORT, _YES, buffer[0x7]);

        if (caps.downStreamPortPresent && !caps.downStreamPortCount)
        {
            DP_LOG(("DPHAL> Non-compliant device, reporting downstream port present, but no downstream ports. Overriding port count to 1."));
            caps.downStreamPortCount = 1;
        }

        // Burst read from 0x20 to 0x22.
        bus.read(NV_DPCD_SINK_VIDEO_FALLBACK_FORMATS, &buffer[0], 0x22 - 0x20 + 1);

        caps.videoFallbackFormats = buffer[0];

        caps.supportsMultistream = FLD_TEST_DRF(_DPCD, _MSTM, _CAP, _YES, buffer[0x1]);

        caps.numberAudioEndpoints = (unsigned)(DRF_VAL(_DPCD, _NUMBER_OF_AUDIO_ENDPOINTS, _VALUE, buffer[0x2]));

        // 02206h
        if (AuxRetry::ack == bus.read(NV_DPCD14_EXTENDED_MAIN_LINK_CHANNEL_CODING, &buffer[0], 1))
        {
            caps.bDP20ChannelCodingSupported =
                                   FLD_TEST_DRF(_DPCD14,
                                                _EXTENDED_MAIN_LINK_CHANNEL_CODING,
                                                _ANSI_128B_132B,
                                                _YES,
                                                buffer[0]);
            if (caps.bDP20ChannelCodingSupported == true)
            {
                // 0x2215
                if (AuxRetry::ack == bus.read(NV_DPCD20_128B_132B_SUPPORTED_LINK_RATES, &buffer[0], 1))
                {
                    caps.bUHBR_10GSupported =
                         FLD_TEST_DRF(_DPCD20,
                                      _128B_132B_SUPPORTED_LINK_RATES,
                                      _UHBR10,
                                      _YES,
                                      buffer[0]);

                    caps.bUHBR_13_5GSupported =
                         FLD_TEST_DRF(_DPCD20,
                                      _128B_132B_SUPPORTED_LINK_RATES,
                                      _UHBR13_5,
                                      _YES,
                                      buffer[0]);

                    caps.bUHBR_20GSupported =
                         FLD_TEST_DRF(_DPCD20,
                                      _128B_132B_SUPPORTED_LINK_RATES,
                                      _UHBR20,
                                      _YES,
                                      buffer[0]);
                }
                DP_ASSERT(caps.bUHBR_10GSupported && "Unknown max link rate or HBR2 without at least DP 1.2. Assuming DP 1.1 defaults");
            }
        }

        if (bLttprSupported)
        {
            // Burst read from 0xF0000 to 0xF0007
            if (AuxRetry::ack == bus.read(NV_DPCD14_LT_TUNABLE_PHY_REPEATER_REV, &buffer[0], 0x8, retries))
            {
                caps.repeaterCaps.revisionMinor = DRF_VAL(_DPCD14, _LT_TUNABLE_PHY_REPEATER_REV, _MINOR, buffer[0x0]);
                caps.repeaterCaps.revisionMajor = DRF_VAL(_DPCD14, _LT_TUNABLE_PHY_REPEATER_REV, _MAJOR, buffer[0x0]);

                if (lttprIsAtLeastVersion(1, 4))
                {
                    caps.phyRepeaterCount = mapPhyRepeaterVal(DRF_VAL(_DPCD14, _PHY_REPEATER_CNT, _VAL, buffer[0x2]));

                    if (caps.phyRepeaterCount != 0)
                    {
                        if (FLD_TEST_DRF(_DPCD14, _MAX_LINK_RATE_PHY_REPEATER, _VAL, _1_62_GBPS, buffer[1]))
                            caps.repeaterCaps.maxLinkRate = RBR;
                        else if (FLD_TEST_DRF(_DPCD14, _MAX_LINK_RATE_PHY_REPEATER, _VAL, _2_70_GBPS, buffer[1]))
                            caps.repeaterCaps.maxLinkRate = HBR;
                        else if (FLD_TEST_DRF(_DPCD14, _MAX_LINK_RATE_PHY_REPEATER, _VAL, _5_40_GBPS, buffer[1]))
                            caps.repeaterCaps.maxLinkRate = HBR2;
                        else if (FLD_TEST_DRF(_DPCD14, _MAX_LINK_RATE_PHY_REPEATER, _VAL, _8_10_GBPS, buffer[1]))
                            caps.repeaterCaps.maxLinkRate = HBR3;
                        else
                        {
                            DP_ASSERT(0 && "Unknown max link rate or HBR2 without at least DP 1.2. Assuming DP 1.1 defaults");
                            caps.repeaterCaps.maxLinkRate = HBR;
                        }

                        caps.repeaterCaps.maxLaneCount =
                             DRF_VAL(_DPCD14, _MAX_LANE_COUNT_PHY_REPEATER,
                                     _VAL, buffer[0x4]);

                        // The cumulative number of 10ms.
                        caps.repeaterCaps.phyRepeaterExtendedWakeTimeoutMs =
                             DRF_VAL(_DPCD14,
                                     _PHY_REPEATER_EXTENDED_WAKE_TIMEOUT,
                                     _REQ, buffer[0x5]) * 10;

                        // An LTTPR that supports 128b/132b channel coding shall program this register to 20h.
                        if (lttprIsAtLeastVersion(2, 0))
                        {
                            caps.repeaterCaps.bDP20ChannelCodingSupported =
                                 FLD_TEST_DRF(_DPCD14,
                                              _PHY_REPEATER_MAIN_LINK_CHANNEL_CODING,
                                              _128B_132B_SUPPORTED,
                                              _YES,
                                              buffer[6]);

                            caps.repeaterCaps.UHBR_10GSupported =
                                 FLD_TEST_DRF(_DPCD14,
                                              _PHY_REPEATER_128B_132B_RATES,
                                              _10G_SUPPORTED,
                                              _YES,
                                              buffer[7]);

                            caps.repeaterCaps.UHBR_13_5GSupported =
                                 FLD_TEST_DRF(_DPCD14,
                                              _PHY_REPEATER_128B_132B_RATES,
                                              _13_5G_SUPPORTED,
                                              _YES,
                                              buffer[7]);

                            caps.repeaterCaps.UHBR_20GSupported =
                                 FLD_TEST_DRF(_DPCD14,
                                              _PHY_REPEATER_128B_132B_RATES,
                                              _20G_SUPPORTED,
                                              _YES,
                                              buffer[7]);

                            if (buffer[7] && !caps.repeaterCaps.bDP20ChannelCodingSupported)
                            {
                                DP_ASSERT(0 && "UHBR is supported without 128b/132b Channel Encoding Supported!");
                            }
                        }
                    }
                    else
                    {
                        caps.repeaterCaps.maxLinkRate = 0;
                    }
                }
                else
                {
                    // not supported DP revision, we should not be doing LTTPR training
                    caps.phyRepeaterCount = 0;
                    caps.repeaterCaps.maxLinkRate = 0;
                }
            }
        }

        // Check if the device requests extended sleep wake timeout
        if (AuxRetry::ack == bus.read(NV_DPCD14_EXTENDED_DPRX_SLEEP_WAKE_TIMEOUT_REQUEST, &buffer[0], 1))
        {
            if (buffer[0] == NV_DPCD14_EXTENDED_DPRX_SLEEP_WAKE_TIMEOUT_REQUEST_PERIOD_1MS)
            {
                caps.extendedSleepWakeTimeoutRequestMs = DP_EXTENDED_DPRX_SLEEP_WAKE_TIMEOUT_DEFAULT_MS;
            }
            else
            {
                caps.extendedSleepWakeTimeoutRequestMs = buffer[0] * 20;
            }
        }
        else
        {
            caps.extendedSleepWakeTimeoutRequestMs = 0;
        }

        byte = 0U;
        dpMemZero(&caps.psrCaps, sizeof(vesaPsrSinkCaps));

        status = bus.read(NV_DPCD_EDP_PSR_VERSION, &byte, sizeof byte);
        if (status == AuxRetry::ack && byte > 0U)
        {
            caps.psrCaps.psrVersion = byte;
        }

        if (caps.psrCaps.psrVersion)
        {
            unsigned psrSetupTimeMap[8] = { 330U, 275U, 220U, 165U, 110U, 55U, 0U };
            byte = 0U;
            if (AuxRetry::ack == bus.read(NV_DPCD_EDP_PSR_CAP, &byte, sizeof byte))
            {
                caps.psrCaps.linkTrainingRequired =
                    FLD_TEST_DRF(_DPCD_EDP, _PSR_CAP, _LT_NEEDED, _YES, byte);
                caps.psrCaps.psrSetupTime =
                    psrSetupTimeMap[DRF_VAL(_DPCD_EDP, _PSR_CAP,_SETUP_TIME, byte)];
                caps.psrCaps.yCoordinateRequired =
                    FLD_TEST_DRF(_DPCD_EDP, _PSR_CAP,_Y_COORD_NEEDED, _YES, byte);
                caps.psrCaps.psr2UpdateGranularityRequired =
                    FLD_TEST_DRF(_DPCD_EDP, _PSR_CAP,_GRAN_REQUIRED, _YES, byte);
            }

            // Version 2 supports PSR2 and SU
            if (caps.psrCaps.psrVersion == 2U)
            {
                NvU16 xGranular = 0U;
                if (AuxRetry::ack == bus.read(NV_DPCD_EDP_PSR2_X_GRANULARITY_H, &byte, sizeof byte))
                {
                    xGranular = byte;
                }

                byte = 0U;
                if (AuxRetry::ack == bus.read(NV_DPCD_EDP_PSR2_X_GRANULARITY_L, &byte, sizeof byte))
                {
                    xGranular = (xGranular << 8U) | byte;
                }

                caps.psrCaps.suXGranularity = xGranular;
            }

            // version 3 supports Y coordinate
            if (caps.psrCaps.psrVersion > 2U)
            {
                if (AuxRetry::ack == bus.read(NV_DPCD_EDP_PSR2_Y_GRANULARITY, &byte, sizeof byte))
                {
                    caps.psrCaps.suYGranularity = byte;
                }
            }
        }

        parsePortDescriptors();
    }

    virtual PCONCaps * getPCONCaps()
    {
        return &(caps.pconCaps);
    }

    virtual unsigned getRevisionMajor()                                        // DPCD offset 0
    {
        return caps.revisionMajor;
    }

    virtual unsigned getRevisionMinor()
    {
        return caps.revisionMinor;
    }

    virtual unsigned lttprGetRevisionMajor()                                        // DPCD offset F0000h
    {
        return caps.repeaterCaps.revisionMajor;
    }

    virtual unsigned lttprGetRevisionMinor()
    {
        return caps.repeaterCaps.revisionMinor;
    }

    virtual LinkRate getMaxLinkRate()                                          // DPCD offset 1 * 27000000
    {
        if (caps.phyRepeaterCount == 0)
            return caps.maxLinkRate;
        else
            return DP_MIN(caps.maxLinkRate, caps.repeaterCaps.maxLinkRate);
    }

    virtual unsigned getMaxLaneCount()                                         // DPCD offset 2
    {
        if (caps.phyRepeaterCount == 0)
            return caps.maxLaneCount;
        else
            return DP_MIN(caps.maxLaneCount, caps.repeaterCaps.maxLaneCount);
    }

    virtual bool getNoLinkTraining()
    {
        return caps.noLinkTraining;
    }

    virtual unsigned getPhyRepeaterCount()
    {
        return caps.phyRepeaterCount;
    }

    // Max lanes supported at the desired link rate.
    virtual unsigned getMaxLaneCountSupportedAtLinkRate(LinkRate linkRate)
    {
        if (linkRate == HBR)
        {
            if (caps.maxLanesAtHBR)
            {
                return DP_MIN(caps.maxLanesAtHBR, getMaxLaneCount());
            }
        }
        else if (linkRate == RBR)
        {
            if (caps.maxLanesAtRBR)
            {
                return DP_MIN(caps.maxLanesAtRBR, getMaxLaneCount());
            }
        }
        // None of the above cases got hit, simply return the max lane count
        return getMaxLaneCount();
    }

    virtual bool getEnhancedFraming()
    {
        return caps.enhancedFraming;
    }

    virtual bool getDownstreamPort(NvU8 *portType)                          // DPCD offset 5
    {
        *portType = caps.downStreamPortType;
        return caps.downStreamPortPresent;
    }

    virtual bool getSupportsNoHandshakeTraining()
    {
        return caps.supportsNoHandshakeTraining;
    }

    virtual unsigned getLegacyPortCount()                                // DPCD offset 7
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

    virtual bool getSDPExtnForColorimetry()
    {
        bool bSDPExtnForColorimetry = false;
        NvU8 byte = 0;
        if (caps.extendedRxCapsPresent)
        {
            if (AuxRetry::ack == bus.read(NV_DPCD14_EXTENDED_DPRX_FEATURE_ENUM_LIST, &byte,  sizeof byte))
            {
                bSDPExtnForColorimetry = FLD_TEST_DRF(_DPCD14, _EXTENDED_DPRX_FEATURE_ENUM_LIST,
                                                      _VSC_SDP_EXT_FOR_COLORIMETRY, _YES, byte);
            }
        }
        return bSDPExtnForColorimetry;
    }

    virtual AuxRetry::status setOuiSource(unsigned ouiId, const char * model, size_t modelNameLength, NvU8 chipRevision)
    {
        NvU8 ouiBuffer[16];

        //  The first 3 bytes are IEEE_OUI. 2 hex digits per register.
        ouiBuffer[0] = (ouiId >> 16) & 0xFF;
        ouiBuffer[1] = (ouiId >> 8) & 0xFF;
        ouiBuffer[2] = ouiId & 0xFF;

        if (caps.revisionMajor <= 0)
            DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

        if (modelNameLength > NV_DPCD_SOURCE_DEV_ID_STRING__SIZE)
        {
            DP_LOG(("DPHAL> setOuiSource(): modelNameLength should not be greater than 6"));
            modelNameLength = NV_DPCD_SOURCE_DEV_ID_STRING__SIZE;
        }

        // Next 6 bytes are Device Identification String.
        for (unsigned int i = 0; i < modelNameLength; i++)
        {
            ouiBuffer[3+i] = *model;
            if (*model)
                model++;
        }
        ouiBuffer[9] = chipRevision;

        for (int i = 0xA; i<=0xF; ++i)
            ouiBuffer[i] = 0;

        return bus.write(NV_DPCD_SOURCE_IEEE_OUI, &ouiBuffer[0], sizeof ouiBuffer);
    }

    virtual bool getOuiSink(unsigned &ouiId, char * modelName, size_t modelNameBufferSize, NvU8 & chipRevision)
    {
        NvU8 ouiBuffer[16];
        int address = NV_DPCD_SINK_IEEE_OUI;

        if (caps.revisionMajor <= 0)
            DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

        // If buffer size is larger than dev_id size, the extras are not used.
        // If buffer size is smaller, than we can only get certain bytes.
        if (modelNameBufferSize > NV_DPCD_SOURCE_DEV_ID_STRING__SIZE)
        {
            modelNameBufferSize = NV_DPCD_SOURCE_DEV_ID_STRING__SIZE;
        }

        //
        // Check if there is a downstream facing port (DFP)
        // If DFP is present, device is a branch device - use branch offset
        // Else device is a sink device - use sink offset
        //
        if(caps.downStreamPortPresent)
        {
            address = NV_DPCD_BRANCH_IEEE_OUI;
        }

        if (AuxRetry::ack != bus.read(address, &ouiBuffer[0], sizeof ouiBuffer))
        {
            *modelName = 0;
            ouiId = 0;
            chipRevision = 0;
            return false;
        }
        //  The first 3 bytes are IEEE_OUI. 2 hex digits per register.
        ouiId = ouiBuffer[0] | (ouiBuffer[1] << 8) | (ouiBuffer[2] << 16);

        // Next 6 bytes are Device Identification String, copy as much as we can (limited buffer case).
        unsigned int i;
        for (i = 0; i < modelNameBufferSize; i++)
            modelName[i] = ouiBuffer[3+i];

        chipRevision = ouiBuffer[9];

        return true;
    }

    virtual bool getSupportsMultistream()                                     // DPCD offset 21h
    {
        return caps.supportsMultistream && (!caps.overrideToSST);
    }

    virtual void setSupportsESI(bool bIsESISupported)
    {
        caps.supportsESI = bIsESISupported;
    }

    //
    //  Single stream specific caps
    //
    virtual unsigned getNumberOfAudioEndpoints()                            // DPCD offset 22h
    {
        if (caps.numberAudioEndpoints)
            return caps.numberAudioEndpoints;
        else
            return caps.NORP > 1;
    }

    virtual bool getGUID(GUID & guid)                                          // DPCD offset 30h
    {
        NvU8 buffer[DPCD_GUID_SIZE];

        if (caps.revisionMajor <= 0)
            DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

        bus.read(NV_DPCD_GUID, &buffer[0], sizeof(buffer));

        for (unsigned i = 0; i < DPCD_GUID_SIZE; i++)
        {
            guid.data[i] = buffer[i];
        }
        return true;
    }

    virtual AuxRetry::status setGUID(GUID & guid)
    {
        NvU8 buffer[DPCD_GUID_SIZE];

        if (caps.revisionMajor <= 0)
            DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

        for (unsigned i = 0; i < DPCD_GUID_SIZE; i++)
        {
            buffer[i] = guid.data[i];
        }

        return bus.write(NV_DPCD_GUID, buffer, sizeof(buffer));
    }

    void parsePortDescriptors()
    {
        NvU8 basicCaps[128];
        unsigned bytesPerPort = caps.detailedCapInfo ? 4 : 1;
        // When Detailed_cap_info_available bit is set to 1, the max number
        // of downstream port is limited to 32. Otherwise it supports up to 127
        unsigned maxPorts = caps.detailedCapInfo ? 32 : 127;
        unsigned infoByte0;
        if (caps.downStreamPortCount > maxPorts)
            caps.downStreamPortCount = 1;
        unsigned size = (bytesPerPort * caps.downStreamPortCount);

        if (AuxRetry::ack != bus.read(NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT(0), &basicCaps[0], size))
        {
            DP_LOG(("DPHAL> Unable to read detailed caps!"));
            caps.downStreamPortCount = 0;
            return;
        }

        if (!((isVersion(1,0)) ||
              (isVersion(1,1) && basicCaps[0] == 0 &&
               legacyPort[0].type == ANALOG_VGA)))
        {
            for (unsigned port = 0; port < caps.downStreamPortCount; port++)
            {
                //  The index to access detailed info byte 0
                infoByte0 = port * bytesPerPort;
                switch (DRF_VAL(_DPCD, _DETAILED_CAP_INFO_DWNSTRM_PORT, _TX_TYPE, basicCaps[infoByte0]))
                {
                case NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_TX_TYPE_DISPLAYPORT:
                    {
                        legacyPort[port].type = DISPLAY_PORT;
                        break;
                    }
                case NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_TX_TYPE_ANALOG:
                    {
                        legacyPort[port].type = ANALOG_VGA;
                        break;
                    }
                case NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_TX_TYPE_DVI:
                    {
                        legacyPort[port].type = DVI;
                        break;
                    }
                case NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_TX_TYPE_HDMI:
                    {
                        NvU8    pConCaps   = basicCaps[infoByte0+2];

                        legacyPort[port].type = HDMI;

                        caps.pconCaps.maxTmdsClkRate = basicCaps[infoByte0+1];

                        caps.pconCaps.bSourceControlModeSupported =
                             FLD_TEST_DRF(_DPCD, _DETAILED_CAP_INFO, _SRC_CONTROL_MODE_SUPPORT, _YES, pConCaps);
                        caps.pconCaps.bConcurrentLTSupported =
                             FLD_TEST_DRF(_DPCD, _DETAILED_CAP_INFO, _CONCURRENT_LT_SUPPORT, _YES, pConCaps);
                        caps.pconCaps.maxHdmiLinkBandwidthGbps =
                             DRF_VAL(_DPCD, _DETAILED_CAP_INFO, _MAX_FRL_LINK_BW_SUPPORT, pConCaps);

                        switch (DRF_VAL(_DPCD, _DETAILED_CAP_INFO, _MAX_BITS_PER_COMPONENT_DEF, pConCaps))
                        {
                            case NV_DPCD_DETAILED_CAP_INFO_MAX_BITS_PER_COMPONENT_DEF_10BPC:
                                caps.pconCaps.maxBpc = 10;
                                break;
                            case NV_DPCD_DETAILED_CAP_INFO_MAX_BITS_PER_COMPONENT_DEF_12BPC:
                                caps.pconCaps.maxBpc = 12;
                                break;
                            case NV_DPCD_DETAILED_CAP_INFO_MAX_BITS_PER_COMPONENT_DEF_16BPC:
                                caps.pconCaps.maxBpc = 16;
                                break;
                            case NV_DPCD_DETAILED_CAP_INFO_MAX_BITS_PER_COMPONENT_DEF_8BPC:
                            default:
                                caps.pconCaps.maxBpc = 8;
                                break;
                        }
                        break;
                    }
                case NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_TX_TYPE_OTHERS_NO_EDID:
                    {
                        legacyPort[port].type = WITHOUT_EDID;
                        switch (DRF_VAL(_DPCD, _DETAILED_CAP_INFO_DWNSTRM_PORT, _NON_EDID_ATTR, basicCaps[infoByte0]))
                        {
                        default:
                            {
                                DP_ASSERT(0 && "Unknown non-edid type, assume Reserved");
                                legacyPort[port].nonEDID  = RESERVED;
                                break;
                            }
                        case NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_NON_EDID_480I_60HZ:
                            {
                                legacyPort[port].nonEDID  = IL_720_480_60HZ;
                                break;
                            }
                        case NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_NON_EDID_480I_50HZ:
                            {
                                legacyPort[port].nonEDID  = IL_720_480_50HZ;
                                break;
                            }
                        case NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_NON_EDID_1080I_60HZ:
                            {
                                legacyPort[port].nonEDID  = IL_1920_1080_60HZ;
                                break;
                            }
                        case NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_NON_EDID_1080I_50HZ:
                            {
                                legacyPort[port].nonEDID  = IL_1920_1080_50HZ;
                                break;
                            }
                        case NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_NON_EDID_720P_60HZ:
                            {
                                legacyPort[port].nonEDID  = PG_1280_720_60HZ;
                                break;
                            }
                        case NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_NON_EDID_720P_50HZ:
                            {
                                legacyPort[port].nonEDID  = PG_1280_720_50_HZ;
                                break;
                            }
                        }
                        break;
                    }
                case NV_DPCD_DETAILED_CAP_INFO_DWNSTRM_PORT_TX_TYPE_DP_PLUSPLUS:
                    {
                        legacyPort[port].type = DISPLAY_PORT_PLUSPLUS;
                        break;
                    }
                default:
                    {
                        DP_ASSERT(0 && "Unknown port type");
                        break;
                    }
                }

                // Set the Init value to Zero
                legacyPort[port].maxTmdsClkRate = 0;

                if (legacyPort[port].type == DVI ||
                      legacyPort[port].type == HDMI ||
                      legacyPort[port].type == DISPLAY_PORT_PLUSPLUS)
                {
                    legacyPort[port].maxTmdsClkRate = ((NvU64)basicCaps[infoByte0 + 1]) * 2500000;
                    if (legacyPort[port].maxTmdsClkRate == 0)
                    {
                        DP_ASSERT(legacyPort[port].maxTmdsClkRate && "No Max TMDS clock rate limits.");
                    }

                    /*
                        Bug : 3202060
                        Parse Byte 2 as well to check the Dongle supports HDMI FRL Output
                        If HDMI FRL is supported, the maxTmdsClkRate limit should be removed.
                    */

                    if (DRF_VAL(_DPCD, _DETAILED_CAP_INFO, _MAX_FRL_LINK_BW_SUPPORT, basicCaps[infoByte0 + 2]))
                    {
                        // Disable the TMDS CLK Limit
                        legacyPort[port].maxTmdsClkRate = 0;
                    }
                }
            }
        }
    }

    //
    //  Notifications of external events
    //
    virtual void notifyIRQ()
    {
        parseAndReadInterrupts();
    }

    virtual void populateFakeDpcd()
    {
        dpcdOffline = true;
        // fill out the bare minimum caps required ... this should be extended in for more dpcd offsets in future.
        caps.revisionMajor = 0x1;
        caps.revisionMinor = 0x1;
        caps.supportsESI = false;
        caps.maxLinkRate = HBR3;
        caps.maxLaneCount = 4;
        caps.enhancedFraming = true;
        caps.downStreamPortPresent = true;
        caps.downStreamPortCount = 1;

        // populate the sinkcount interrupt
        interrupts.sinkCount = 1;
    }

    // DPCD override routine: Max link rate override.
    void overrideMaxLinkRate(NvU32 overrideMaxLinkRate)
    {
        if (overrideMaxLinkRate)
        {
            caps.maxLinkRate = mapLinkBandiwdthToLinkrate(overrideMaxLinkRate);
        }
    }

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

    virtual void notifyHPD(bool status, bool bSkipDPCDRead)
    {
        if (!status)
        {
            // check if dpcd is alive
            NvU8 buffer;
            unsigned retries = 16;
            if (AuxRetry::ack == bus.read(NV_DPCD_REV, &buffer, sizeof buffer, retries))
                return;

            // Support for EDID locking:
            // Refill the cache with "default" dpcd data on an unplug event as later on
            // the client may send a hot-plug event for edid locked fake device (no real dpcd).
            // Also raise flag "dpcdOffline" so that dpcd accesses may be optimized.
            populateFakeDpcd();
            return;
        }

        // Skip DPCD read if requested.
        if (!bSkipDPCDRead)
        {
            parseAndReadCaps();
        }

        //
        // For Allienware eDp Panel more time is required to assert the HPD &
        // power on the AUX link. Retry 1 more time if it has failed. This is
        // a BAD way to do it but no EDID is available to differentiate here
        // this is the first access which happens before EDID read to read caps.
        // We also found that some LG panels on HP NBs goes in a bad state after
        // factory reset. Retyring 3 times works for them. So making faultyRetries as 3.
        //
        NvU32 faultyRetries = 3;
        while ((dpcdOffline) && (faultyRetries > 0))
        {
            // Read the caps again
            parseAndReadCaps();
            --faultyRetries;
        }

        parseAndReadInterrupts();
    }

    virtual bool isPostLtAdjustRequestSupported()
    {
        //
        // If the upstream DPTX and downstream DPRX both support TPS4,
        // TPS4 shall be used instead of POST_LT_ADJ_REQ.
        //
        NvBool bTps4Supported = gpuDP1_4Supported && caps.bSupportsTPS4;
        return bGrantsPostLtRequest && !bTps4Supported;
    }

    virtual void setPostLtAdjustRequestGranted(bool bGrantPostLtRequest)
    {
        NvU8 data = 0;

        bus.read(NV_DPCD_LANE_COUNT_SET, &data, sizeof data);

        if (bGrantPostLtRequest)
        {
            data = FLD_SET_DRF(_DPCD, _LANE_COUNT_SET, _POST_LT_ADJ_REQ_GRANTED, _YES, data);
        }

        else
        {
            data = FLD_SET_DRF(_DPCD, _LANE_COUNT_SET, _POST_LT_ADJ_REQ_GRANTED, _NO, data);
        }

        if (AuxRetry::ack != bus.write(NV_DPCD_LANE_COUNT_SET, &data, sizeof data))
        {
            DP_LOG(("DPCONN> Failed to set POST_LT_ADJ_REQ_GRANTED bit."));
        }
    }

    virtual bool getIsPostLtAdjRequestInProgress()               // DPCD offset 204
    {
        NvU8 buffer;

        if (AuxRetry::ack != bus.read(NV_DPCD_LANE_ALIGN_STATUS_UPDATED, &buffer, 1))
        {
            DP_LOG(("DPCONN> Post Link Training : Failed to read POST_LT_ADJ_REQ_IN_PROGRESS"));
            return false;
        }

        return FLD_TEST_DRF(_DPCD, _LANE_ALIGN_STATUS_UPDATED,
                            _POST_LT_ADJ_REQ_IN_PROGRESS, _YES, buffer);
    }

    virtual TrainingPatternSelectType getTrainingPatternSelect()
    {
        NvU8 trainingPat = 0;
        TrainingPatternSelectType pattern = TRAINING_DISABLED;

        if (caps.revisionMajor <= 0)
            DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

        bus.read(NV_DPCD_TRAINING_PATTERN_SET, &trainingPat, sizeof trainingPat);

        trainingPat = DRF_VAL(_DPCD, _TRAINING_PATTERN_SET, _TPS, trainingPat);

        if (trainingPat == NV_DPCD_TRAINING_PATTERN_SET_TPS_NONE)
            pattern = TRAINING_DISABLED;
        if (trainingPat == NV_DPCD_TRAINING_PATTERN_SET_TPS_TP1)
            pattern = TRAINING_PAT_ONE;
        if (trainingPat == NV_DPCD_TRAINING_PATTERN_SET_TPS_TP2)
            pattern = TRAINING_PAT_TWO;
        if (trainingPat == NV_DPCD_TRAINING_PATTERN_SET_TPS_TP3)
            pattern = TRAINING_PAT_THREE;

        return pattern;
    }

    virtual bool setTrainingMultiLaneSet(NvU8 numLanes,
                                         NvU8 *voltSwingSet,
                                         NvU8 *preEmphasisSet)
    {
        NvU8 trainingCtrl[DP_MAX_LANES] = {0};
        unsigned writeAddress = NV_DPCD_TRAINING_LANE_SET(0);
        NvU8 laneIndex;

        if (caps.revisionMajor <= 0)
            DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

        for (laneIndex = 0; laneIndex < numLanes; laneIndex++)
        {
            if (voltSwingSet[laneIndex] <= NV_DPCD_MAX_VOLTAGE_SWING)
            {
                trainingCtrl[laneIndex] = FLD_SET_DRF_NUM(_DPCD, _TRAINING_LANE_SET,
                                                          _VOLTAGE_SWING, voltSwingSet[laneIndex],
                                                          trainingCtrl[laneIndex]);
            }
            else
            {
                DP_ASSERT(0 && "Out of bounds voltage swing.  Assuming 0");
            }

            if (voltSwingSet[laneIndex] == NV_DPCD_MAX_VOLTAGE_SWING)
            {
                trainingCtrl[laneIndex] = FLD_SET_DRF(_DPCD, _TRAINING_LANE_SET,
                                                      _VOLTAGE_SWING_MAX_REACHED,
                                                      _TRUE, trainingCtrl[laneIndex]);
            }

            if (preEmphasisSet[laneIndex] <= NV_DPCD_MAX_VOLTAGE_PREEMPHASIS)
            {
                trainingCtrl[laneIndex] = FLD_SET_DRF_NUM(_DPCD, _TRAINING_LANE_SET,
                                                          _PREEMPHASIS, preEmphasisSet[laneIndex],
                                                          trainingCtrl[laneIndex]);
            }
            else
            {
                DP_ASSERT(0 && "Out of bounds preemphasis.  Assuming 0");
            }

            if (preEmphasisSet[laneIndex] == NV_DPCD_MAX_VOLTAGE_PREEMPHASIS)
            {
                trainingCtrl[laneIndex] = FLD_SET_DRF(_DPCD, _TRAINING_LANE_SET,
                                                      _PREEMPHASIS_MAX_REACHED, _TRUE,
                                                      trainingCtrl[laneIndex]);
            }
        }

        return(AuxRetry::ack == bus.write(writeAddress, trainingCtrl, (unsigned)numLanes));
    }

    virtual AuxRetry::status setIgnoreMSATimingParamters(bool msaTimingParamIgnoreEn)
    {

        NvU8 downspreadCtrl = 0;

        if (caps.revisionMajor <= 0)
            DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

        bus.read(NV_DPCD_DOWNSPREAD_CTRL, &downspreadCtrl, sizeof downspreadCtrl);

        if (msaTimingParamIgnoreEn)
            downspreadCtrl = FLD_SET_DRF(_DPCD, _DOWNSPREAD_CTRL, _MSA_TIMING_PAR_IGNORED, _TRUE, downspreadCtrl);
        else
            downspreadCtrl = FLD_SET_DRF(_DPCD, _DOWNSPREAD_CTRL, _MSA_TIMING_PAR_IGNORED, _FALSE, downspreadCtrl);

        return bus.write(NV_DPCD_DOWNSPREAD_CTRL, &downspreadCtrl, sizeof downspreadCtrl);
    }

    virtual AuxRetry::status setLinkQualPatternSet(LinkQualityPatternType linkQualPattern, unsigned laneCount)
    {
        if (caps.revisionMajor <= 0)
        {
            DP_ASSERT(0 && "Something is wrong, revision major should be > 0");
            return AuxRetry::nack;
        }

        if (this->isVersion(1, 1) == true)
        {
            NvU8 buffer = 0;
            if (AuxRetry::ack != bus.read(NV_DPCD_TRAINING_PATTERN_SET, &buffer, 1))
            {
                DP_ASSERT(0 && "Can't read from NV_DPCD_TRAINING_PATTERN_SET.");
                return AuxRetry::nack;
            }

            // write on bits 3:2
            NvU8 value = ((linkQualPattern << 2) & 0xc) | (buffer & (~0xc));
            return bus.write(NV_DPCD_TRAINING_PATTERN_SET, &value, sizeof value);
        }
        else if (isAtLeastVersion(1,2) == true)
        {
            AuxRetry::status requestStatus = AuxRetry::nack ;

            // Set test patterns for all requested lanes
            for (unsigned i = 0; i < laneCount; i++)
            {
                requestStatus = setLinkQualLaneSet(i, linkQualPattern);
                if (requestStatus != AuxRetry::ack)
                    break;
            }

            return requestStatus;
        }
        else
        {
            DP_ASSERT(0 && "Regs only supported for DP1.2");
            return AuxRetry::unsupportedRegister;
        }
    }

    virtual AuxRetry::status setLinkQualLaneSet(unsigned lane, LinkQualityPatternType linkQualPattern)
    {
        NvU8 linkQuality = 0;
        unsigned writeAddress = NV_DPCD_LINK_QUAL_LANE_SET(lane);

        if (caps.revisionMajor <= 0)
            DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

        if (isAtLeastVersion(1,2) == false)
        {
            DP_ASSERT(0 && "Regs only supported for DP1.2");
            return AuxRetry::unsupportedRegister;
        }

        // check if parameter is valid
        if (lane >= displayPort_LaneSupported)
        {
            DP_ASSERT(0 && "Unknown lane selected.  Assuming Lane 0");
            writeAddress = NV_DPCD_LINK_QUAL_LANE_SET(0);
        }

        if (linkQualPattern == LINK_QUAL_DISABLED)
            linkQuality = FLD_SET_DRF(_DPCD, _LINK_QUAL_LANE_SET, _LQS, _NO, linkQuality);
        if (linkQualPattern == LINK_QUAL_D10_2)
            linkQuality = FLD_SET_DRF(_DPCD, _LINK_QUAL_LANE_SET, _LQS, _D10_2, linkQuality);
        if (linkQualPattern == LINK_QUAL_SYM_ERROR)
            linkQuality = FLD_SET_DRF(_DPCD, _LINK_QUAL_LANE_SET, _LQS, _SYM_ERR_MEASUREMENT_CNT, linkQuality);
        if (linkQualPattern == LINK_QUAL_PRBS7)
            linkQuality = FLD_SET_DRF(_DPCD, _LINK_QUAL_LANE_SET, _LQS, _PRBS7, linkQuality);
        if (linkQualPattern == LINK_QUAL_80BIT_CUST)
            linkQuality = FLD_SET_DRF(_DPCD, _LINK_QUAL_LANE_SET, _LQS, _80_BIT_CUSTOM, linkQuality);
        if (linkQualPattern == LINK_QUAL_HBR2_COMPLIANCE_EYE)
            linkQuality = FLD_SET_DRF(_DPCD, _LINK_QUAL_LANE_SET, _LQS, _HBR2, linkQuality);
        if (linkQualPattern == LINK_QUAL_CP2520PAT3)
            linkQuality = FLD_SET_DRF(_DPCD14, _LINK_QUAL_LANE_SET, _LQS, _CP2520PAT3, linkQuality);

        return bus.write(writeAddress, &linkQuality, sizeof linkQuality);
    }

    virtual AuxRetry::status setMessagingEnable(bool _uprequestEnable, bool _upstreamIsSource)
    {
        NvU8 mstmCtrl = 0;

        if (!this->isAtLeastVersion(1, 2))
        {
            DP_ASSERT(!_uprequestEnable && "Can't enable multistream on DP 1.1");
            return AuxRetry::nack;
        }

        uprequestEnable = _uprequestEnable;
        upstreamIsSource = _upstreamIsSource;

        //
        // Lets not touch the MST enable bit here.
        // Branch might be getting driven in MST mode and we do not want to
        // change that unless we are sure there are no more streams being driven.
        //
        if (AuxRetry::ack != bus.read(NV_DPCD_MSTM_CTRL, &mstmCtrl, 1))
        {
            DP_LOG(("DPHAL> ERROR! Unable to read 00111h MSTM_CTRL."));
        }

        if (_uprequestEnable)
        {
            bMultistream = FLD_TEST_DRF(_DPCD, _MSTM_CTRL, _EN, _YES, mstmCtrl);
        }
        else
        {
            bMultistream = false;
        }
        mstmCtrl = 0;
        if (bMultistream)
            mstmCtrl = FLD_SET_DRF(_DPCD, _MSTM_CTRL, _EN, _YES, mstmCtrl);
        if (uprequestEnable)
            mstmCtrl = FLD_SET_DRF(_DPCD, _MSTM_CTRL, _UP_REQ_EN, _YES, mstmCtrl);
        if (upstreamIsSource)
            mstmCtrl = FLD_SET_DRF(_DPCD, _MSTM_CTRL, _UPSTREAM_IS_SRC, _YES, mstmCtrl);

        return bus.write(NV_DPCD_MSTM_CTRL, &mstmCtrl, sizeof mstmCtrl);
    }

    virtual AuxRetry::status setMultistreamLink(bool enable)
    {
        NvU8 mstmCtrl = 0;

        if (!this->isAtLeastVersion(1, 2))
        {
            DP_ASSERT(!enable && "Can't enable multistream on DP 1.1");
            return AuxRetry::nack;
        }

        bMultistream = enable;

        if (bMultistream)
            mstmCtrl = FLD_SET_DRF(_DPCD, _MSTM_CTRL, _EN, _YES, mstmCtrl);
        if (uprequestEnable)
            mstmCtrl = FLD_SET_DRF(_DPCD, _MSTM_CTRL, _UP_REQ_EN, _YES, mstmCtrl);
        if (upstreamIsSource)
            mstmCtrl = FLD_SET_DRF(_DPCD, _MSTM_CTRL, _UPSTREAM_IS_SRC, _YES, mstmCtrl);

        return bus.write(NV_DPCD_MSTM_CTRL, &mstmCtrl, sizeof mstmCtrl);
    }

    virtual AuxRetry::status setMultistreamHotplugMode(MultistreamHotplugMode notifyType)
    {
        NvU8 deviceCtrl = 0;

        if (caps.revisionMajor <= 0)
            DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

        //  notifytype == HPD_LONG_PULSE, adapter 0
        if (notifyType == IRQ_HPD)
            deviceCtrl = FLD_SET_DRF(_DPCD, _BRANCH_DEV_CTRL, _HOTPLUG_EVENT_TYPE, _IRQ_HPD, deviceCtrl);

        return bus.write(NV_DPCD_BRANCH_DEV_CTRL, &deviceCtrl, sizeof deviceCtrl);
    }


    bool parseTestRequestTraining(NvU8 * buffer /* 0x18-0x28 valid */)
    {
        if (buffer[1] == 0x6)
            interrupts.testTraining.testRequestLinkRate = RBR;
        else if (buffer[1] == 0xa)
            interrupts.testTraining.testRequestLinkRate = HBR;
        else if (buffer[1] == 0x14)
            interrupts.testTraining.testRequestLinkRate = HBR2;
        else if (buffer[1] == 0x1E)
            interrupts.testTraining.testRequestLinkRate = HBR3;
        else
        {
            DP_ASSERT(0 && "Unknown max link rate.  Assuming RBR");
            interrupts.testTraining.testRequestLinkRate = RBR;
        }

        interrupts.testTraining.testRequestLaneCount  = buffer[(0x220 - 0x218)] & 0xf;

        return true;
    }

    void parseAutomatedTestRequest(bool testRequestPending)
    {
        NvU8 buffer[16];

        interrupts.automatedTestRequest = false;
        interrupts.testEdid.testRequestEdidRead = false;
        interrupts.testTraining.testRequestTraining = false;
        interrupts.testPhyCompliance.testRequestPhyCompliance = false;

        if (!testRequestPending)
        {
            return;
        }
        interrupts.automatedTestRequest = true;

        if (AuxRetry::ack != bus.read(NV_DPCD_TEST_REQUEST, &buffer[0], 16))
        {
            DP_LOG(("DPHAL> ERROR! Automated test request found. Unable to read 0x218 register."));
            return;
        }

        if (FLD_TEST_DRF(_DPCD, _TEST_REQUEST, _TEST_LINK_TRAINING, _YES, buffer[0]))
        {
            interrupts.testTraining.testRequestTraining = parseTestRequestTraining(&buffer[0]);
        }

        if (FLD_TEST_DRF(_DPCD, _TEST_REQUEST, _TEST_EDID_READ, _YES, buffer[0]))
        {
            interrupts.testEdid.testRequestEdidRead = true;
        }

        if (FLD_TEST_DRF(_DPCD, _TEST_REQUEST, _TEST_PHY_TEST_PATTERN, _YES, buffer[0]))
        {
            interrupts.testPhyCompliance.testRequestPhyCompliance = parseTestRequestPhy();
        }
    }

    virtual bool parseTestRequestPhy()
    {
        NvU8 buffer = 0;
        NvU8 bits = 0;
        if (AuxRetry::ack != bus.read(NV_DPCD_PHY_TEST_PATTERN, &buffer, 1))
        {
            DP_LOG(("DPHAL> ERROR! Test pattern request found but unable to read NV_DPCD_PHY_TEST_PATTERN register."));
            return false;
        }

        if (isVersion(1,0))
            bits = 0;
        else
            bits = DRF_VAL(_DPCD, _PHY_TEST_PATTERN_SEL, _DP12, buffer);

        if (bits == NV_DPCD_PHY_TEST_PATTERN_SEL_NO)
            interrupts.testPhyCompliance.phyTestPattern = LINK_QUAL_DISABLED;
        else if (bits == NV_DPCD_PHY_TEST_PATTERN_SEL_D10_2)
            interrupts.testPhyCompliance.phyTestPattern = LINK_QUAL_D10_2;
        else if (bits == NV_DPCD_PHY_TEST_PATTERN_SEL_SYM_ERR_MEASUREMENT_CNT)
            interrupts.testPhyCompliance.phyTestPattern = LINK_QUAL_SYM_ERROR;
        else if (bits == NV_DPCD_LINK_QUAL_LANE_SET_LQS_PRBS7)
            interrupts.testPhyCompliance.phyTestPattern = LINK_QUAL_PRBS7;
        else if (bits == NV_DPCD_LINK_QUAL_LANE_SET_LQS_80_BIT_CUSTOM)
            interrupts.testPhyCompliance.phyTestPattern = LINK_QUAL_80BIT_CUST;
        else if (bits == NV_DPCD_LINK_QUAL_LANE_SET_LQS_HBR2)
            interrupts.testPhyCompliance.phyTestPattern = LINK_QUAL_HBR2_COMPLIANCE_EYE;
        else if (bits == NV_DPCD14_PHY_TEST_PATTERN_SEL_CP2520PAT3)
            interrupts.testPhyCompliance.phyTestPattern = LINK_QUAL_CP2520PAT3;
        else
        {
            DP_ASSERT(0 && "Unknown pattern type, assuming none");
            interrupts.testPhyCompliance.phyTestPattern = LINK_QUAL_DISABLED;
            return false;
        }

        if (interrupts.testPhyCompliance.phyTestPattern == LINK_QUAL_80BIT_CUST)
        {
            NvU8 buffer[NV_DPCD_TEST_80BIT_CUSTOM_PATTERN__SIZE] = {0};
            if (AuxRetry::ack != bus.read(NV_DPCD_TEST_80BIT_CUSTOM_PATTERN(0), &buffer[0],
                                          NV_DPCD_TEST_80BIT_CUSTOM_PATTERN__SIZE))
            {
                DP_LOG(("DPHAL> ERROR! Request for 80 bit custom pattern. Can't read from 250h."));
                return false;
            }

            for (unsigned i = 0; i < NV_DPCD_TEST_80BIT_CUSTOM_PATTERN__SIZE; i++)
            {
                interrupts.eightyBitCustomPat[i] = buffer[i];
            }
        }

        return true;
    }

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

    virtual bool isLinkStatusValid(unsigned lanes)
    {
        bool linkStatus = true;

        this->setDirtyLinkStatus(true);
        this->refreshLinkStatus();

        for (unsigned lane = 0; lane < lanes ; lane++)
        {
            linkStatus = linkStatus && interrupts.laneStatusIntr.laneStatus[lane].clockRecoveryDone &&
                         interrupts.laneStatusIntr.laneStatus[lane].channelEqualizationDone &&
                         interrupts.laneStatusIntr.laneStatus[lane].symbolLocked;
        }

        linkStatus = linkStatus && interrupts.laneStatusIntr.interlaneAlignDone;

        return linkStatus;
    }

    virtual void refreshLinkStatus()
    {
        if (interrupts.laneStatusIntr.linkStatusDirtied)
        {
            if (caps.supportsESI &&
                (caps.eDpRevision != NV_DPCD_EDP_REV_VAL_1_4) &&
                (caps.eDpRevision != NV_DPCD_EDP_REV_VAL_1_4A))
            {
                this->fetchLinkStatusESI();
            }
            else
            {
                this->fetchLinkStatusLegacy();
            }
        }
    }

    virtual void setDirtyLinkStatus(bool dirty)
    {
        interrupts.laneStatusIntr.linkStatusDirtied = dirty;
    }

    void parseAndReadInterruptsESI()
    {
        NvU8 buffer[16] = {0};
        bool automatedTestRequest;

        if (AuxRetry::ack != bus.read(NV_DPCD_SINK_COUNT_ESI, &buffer[2], 0x2005 - 0x2002 + 1))
            return;

        interrupts.sinkCount = DRF_VAL(_DPCD, _SINK_COUNT_ESI, _SINK_COUNT, buffer[2]);

        // check if edp revision is v1.4 or v1.4a
        if ((caps.eDpRevision != NV_DPCD_EDP_REV_VAL_1_4) && (caps.eDpRevision != NV_DPCD_EDP_REV_VAL_1_4A))
        {
            automatedTestRequest               = FLD_TEST_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR_ESI0, _AUTO_TEST, _YES, buffer[3]);
        }
        else
        {
            // if edp rev is v1.4 or v1.4a, then use legacy address for auto test.
            NvU8 legacy = 0;
            if (AuxRetry::ack != bus.read(NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR, &legacy, 1))
                return;
            automatedTestRequest               = FLD_TEST_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR, _AUTO_TEST, _YES, legacy);
        }

        interrupts.cpIRQ                       = FLD_TEST_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR_ESI0, _CP, _YES, buffer[3]);
        interrupts.mccsIRQ                     = FLD_TEST_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR_ESI0, _MCCS_IRQ, _YES, buffer[3]);
        interrupts.downRepMsgRdy               = FLD_TEST_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR_ESI0, _DOWN_REP_MSG_RDY, _YES, buffer[3]);
        interrupts.upReqMsgRdy                 = FLD_TEST_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR_ESI0, _UP_REQ_MSG_RDY, _YES, buffer[3]);

        interrupts.rxCapChanged                = FLD_TEST_DRF(_DPCD, _LINK_SERVICE_IRQ_VECTOR_ESI0, _RX_CAP_CHANGED, _YES, buffer[5]);
        interrupts.linkStatusChanged           = FLD_TEST_DRF(_DPCD, _LINK_SERVICE_IRQ_VECTOR_ESI0, _LINK_STATUS_CHANGED, _YES, buffer[5]);
        interrupts.streamStatusChanged         = FLD_TEST_DRF(_DPCD, _LINK_SERVICE_IRQ_VECTOR_ESI0, _STREAM_STATUS_CHANGED, _YES, buffer[5]);
        interrupts.hdmiLinkStatusChanged       = FLD_TEST_DRF(_DPCD, _LINK_SERVICE_IRQ_VECTOR_ESI0, _HDMI_LINK_STATUS_CHANGED, _YES, buffer[5]);
        //
        // Link status changed bit is not necessarily set at all times when the sink
        // loses the lane status. Refresh the lane status in any case on an IRQ
        //
        if ((caps.eDpRevision != NV_DPCD_EDP_REV_VAL_1_4) &&
            (caps.eDpRevision != NV_DPCD_EDP_REV_VAL_1_4A))
        {
            fetchLinkStatusESI();
        }
        else
        {
            fetchLinkStatusLegacy();
        }

        if (interrupts.linkStatusChanged)
        {
            this->clearLinkStatusChanged();
        }

        if (interrupts.rxCapChanged)
        {

            DP_LOG(("DPHAL> RX Capabilities have changed!"));
            parseAndReadCaps();
            this->clearInterruptCapabilitiesChanged();
        }

        if (interrupts.hdmiLinkStatusChanged)
        {
            this->clearHdmiLinkStatusChanged();
        }

        parseAutomatedTestRequest(automatedTestRequest);
    }

    void readLTTPRLinkStatus(NvS32 rxIndex, NvU8 *buffer)
    {
        int addrLane01Status;
        // LINK_STATUS for LTTPR is 3 bytes. (NV_DPCD14_PHY_REPEATER_START(i) + 0x20 ~ 0x22)
        int bytesToRead = 3;

        DP_ASSERT((rxIndex > 0 && rxIndex <= 8) && "Invalid rxIndex");
        //
        // NV_DPCD14_PHY_REPEATER_START is 0-based.
        // rxIndex is 1-based.
        //
        addrLane01Status = NV_DPCD14_PHY_REPEATER_START(rxIndex - 1) +
                                NV_DPCD14_LANE0_1_STATUS_PHY_REPEATER;
        bus.read(addrLane01Status, buffer, bytesToRead);
    }

    void resetIntrLaneStatus()
    {
        //
        // Reset all laneStatus to true.
        // These bits can only set to true when all DPRX (including sink and LTTPRs) set
        // the corresponding bit to true. Set to true as init value, and later will do &=
        // through all the lanes.
        //
        for (int lane = 0; lane < 4; lane++)
        {
            interrupts.laneStatusIntr.laneStatus[lane].clockRecoveryDone        = true;
            interrupts.laneStatusIntr.laneStatus[lane].channelEqualizationDone  = true;
            interrupts.laneStatusIntr.laneStatus[lane].symbolLocked             = true;
        }
        interrupts.laneStatusIntr.interlaneAlignDone    = true;
        interrupts.laneStatusIntr.downstmPortChng       = true;
        interrupts.laneStatusIntr.linkStatusUpdated     = true;
    }

    void fetchLinkStatusESI()
    {
        NvU8 buffer[16] = {0};
        NvS32 rxIndex;

        // LINK_STATUS_ESI from 0x200C to 0x200E
        int bytesToRead = 3;

        // Reset all laneStatus to true.
        resetIntrLaneStatus();

        for (rxIndex = caps.phyRepeaterCount; rxIndex >= (NvS32) NV0073_CTRL_DP_DATA_TARGET_SINK; rxIndex--)
        {
            if (rxIndex != NV0073_CTRL_DP_DATA_TARGET_SINK)
            {
                readLTTPRLinkStatus(rxIndex, &buffer[0xC]);
            }
            else
            {
                bus.read(NV_DPCD_LANE0_1_STATUS_ESI, &buffer[0xC], bytesToRead);
            }

            for (int lane = 0; lane < 4; lane++)
            {
                unsigned laneBits = buffer[0xC+lane/2] >> (4*(lane & 1));
                interrupts.laneStatusIntr.laneStatus[lane].clockRecoveryDone        &= !!(laneBits & 1);
                interrupts.laneStatusIntr.laneStatus[lane].channelEqualizationDone  &= !!(laneBits & 2);
                interrupts.laneStatusIntr.laneStatus[lane].symbolLocked             &= !!(laneBits & 4);
            }

            interrupts.laneStatusIntr.interlaneAlignDone    &=
                FLD_TEST_DRF(_DPCD, _LANE_ALIGN_STATUS_UPDATED_ESI, _INTERLANE_ALIGN_DONE, _YES, buffer[0xE]);
            interrupts.laneStatusIntr.downstmPortChng       &=
                FLD_TEST_DRF(_DPCD, _LANE_ALIGN_STATUS_UPDATED_ESI, _DOWNSTRM_PORT_STATUS_DONE, _YES, buffer[0xE]);
            interrupts.laneStatusIntr.linkStatusUpdated     &=
                FLD_TEST_DRF(_DPCD, _LANE_ALIGN_STATUS_UPDATED_ESI, _LINK_STATUS_UPDATED, _YES, buffer[0xE]);
        }
        this->setDirtyLinkStatus(false);
    }

    void fetchLinkStatusLegacy()
    {
        NvU8 buffer[16] = {0};
        NvS32 rxIndex;
        // LINK_STATUS from 0x202 to 0x204
        int bytesToRead = 3;

        // Reset all laneStatus to true.
        resetIntrLaneStatus();

        for (rxIndex = caps.phyRepeaterCount; rxIndex >= (NvS32) NV0073_CTRL_DP_DATA_TARGET_SINK; rxIndex--)
        {
            if (rxIndex != NV0073_CTRL_DP_DATA_TARGET_SINK)
            {
                readLTTPRLinkStatus(rxIndex, &buffer[2]);
            }
            else
            {
                bus.read(NV_DPCD_LANE0_1_STATUS, &buffer[2], bytesToRead);
            }

            for (int lane = 0; lane < 4; lane++)
            {
                unsigned laneBits = buffer[2+lane/2] >> (4*(lane & 1));
                interrupts.laneStatusIntr.laneStatus[lane].clockRecoveryDone        &= !!(laneBits & 1);
                interrupts.laneStatusIntr.laneStatus[lane].channelEqualizationDone  &= !!(laneBits & 2);
                interrupts.laneStatusIntr.laneStatus[lane].symbolLocked             &= !!(laneBits & 4);
            }

            interrupts.laneStatusIntr.interlaneAlignDone    &=
                FLD_TEST_DRF(_DPCD, _LANE_ALIGN_STATUS_UPDATED, _INTERLANE_ALIGN_DONE, _YES, buffer[4]);
            interrupts.laneStatusIntr.downstmPortChng       &=
                FLD_TEST_DRF(_DPCD, _LANE_ALIGN_STATUS_UPDATED, _D0WNSTRM_PORT_STATUS_DONE, _YES, buffer[4]);
            interrupts.laneStatusIntr.linkStatusUpdated     &=
                FLD_TEST_DRF(_DPCD, _LANE_ALIGN_STATUS_UPDATED, _LINK_STATUS_UPDATED, _YES, buffer[4]);
        }
        this->setDirtyLinkStatus(false);
    }

    virtual bool readTraining(NvU8* voltageSwingLane,  NvU8* preemphasisLane,
                              NvU8* trainingScoreLane, NvU8* postCursor,
                              NvU8  activeLaneCount)
    {
        NvU8 buffer[0xd] = {0};
        if (voltageSwingLane && preemphasisLane)
        {
            if (AuxRetry::ack != bus.read(NV_DPCD_LANE0_1_ADJUST_REQ, &buffer[0x6], 2))
            {
                DP_ASSERT(0 && "Can't read NV_DPCD_LANE0_1_ADJUST_REQ.");
                return false;
            }
            voltageSwingLane[0] = DRF_VAL(_DPCD, _LANEX_XPLUS1_ADJUST_REQ, _LANEX_DRIVE_CURRENT, buffer[6]);
            voltageSwingLane[1] = DRF_VAL(_DPCD, _LANEX_XPLUS1_ADJUST_REQ, _LANEXPLUS1_DRIVE_CURRENT, buffer[6]);
            voltageSwingLane[2] = DRF_VAL(_DPCD, _LANEX_XPLUS1_ADJUST_REQ, _LANEX_DRIVE_CURRENT, buffer[7]);
            voltageSwingLane[3] = DRF_VAL(_DPCD, _LANEX_XPLUS1_ADJUST_REQ, _LANEXPLUS1_DRIVE_CURRENT, buffer[7]);

            preemphasisLane[0] = DRF_VAL(_DPCD, _LANEX_XPLUS1_ADJUST_REQ, _LANEX_PREEMPHASIS, buffer[6]);
            preemphasisLane[1] = DRF_VAL(_DPCD, _LANEX_XPLUS1_ADJUST_REQ, _LANEXPLUS1_PREEMPHASIS, buffer[6]);
            preemphasisLane[2] = DRF_VAL(_DPCD, _LANEX_XPLUS1_ADJUST_REQ, _LANEX_PREEMPHASIS, buffer[7]);
            preemphasisLane[3] = DRF_VAL(_DPCD, _LANEX_XPLUS1_ADJUST_REQ, _LANEXPLUS1_PREEMPHASIS, buffer[7]);

        }
        if (trainingScoreLane)
        {
            if (AuxRetry::ack != bus.read(NV_DPCD_TRAINING_SCORE_LANE(0), &buffer[0x8], 4))
            {
                DP_ASSERT(0 && "Can't read NV_DPCD_TRAINING_SCORE_LANE(0).");
                return false;
            }
            trainingScoreLane[0] = buffer[0x8];
            trainingScoreLane[1] = buffer[0x9];
            trainingScoreLane[2] = buffer[0xa];
            trainingScoreLane[3] = buffer[0xb];
        }
        if (postCursor)
        {
            if (AuxRetry::ack != bus.read(NV_DPCD_ADJUST_REQ_POST_CURSOR2, &buffer[0xc], 1))
            {
                DP_ASSERT(0 && "Can't read NV_DPCD_ADJUST_REQ_POST_CURSOR2.");
                return false;
            }
            postCursor[0] = DRF_IDX_VAL(_DPCD, _ADJUST_REQ_POST_CURSOR2, _LANE, 0, buffer[0xc]);
            postCursor[1] = DRF_IDX_VAL(_DPCD, _ADJUST_REQ_POST_CURSOR2, _LANE, 1, buffer[0xc]);
            postCursor[2] = DRF_IDX_VAL(_DPCD, _ADJUST_REQ_POST_CURSOR2, _LANE, 2, buffer[0xc]);
            postCursor[3] = DRF_IDX_VAL(_DPCD, _ADJUST_REQ_POST_CURSOR2, _LANE, 3, buffer[0xc]);
        }
        return true;
    }

    virtual bool isLaneSettingsChanged(NvU8* oldVoltageSwingLane,
                                       NvU8* newVoltageSwingLane,
                                       NvU8* oldPreemphasisLane,
                                       NvU8* newPreemphasisLane,
                                       NvU8 activeLaneCount)
    {
        for (unsigned i = 0; i < activeLaneCount; i++)
        {
            if (oldVoltageSwingLane[i] != newVoltageSwingLane[i] ||
                oldPreemphasisLane[i] != newPreemphasisLane[i] )
            {
                return true;
            }
        }
        return false;
    }

    void parseAndReadInterruptsLegacy()
    {
        bool automatedTestRequest = false;
        NvU8 buffer[16] = {0};

        if (AuxRetry::ack != bus.read(NV_DPCD_SINK_COUNT, &buffer[0], 2))
            return;

        interrupts.sinkCount = NV_DPCD_SINK_COUNT_VAL(buffer[0]);

        automatedTestRequest                   = FLD_TEST_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR, _AUTO_TEST, _YES, buffer[1]);
        interrupts.cpIRQ                       = FLD_TEST_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR, _CP, _YES, buffer[1]);
        interrupts.mccsIRQ                     = FLD_TEST_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR, _MCCS_IRQ, _YES, buffer[1]);
        interrupts.downRepMsgRdy               = FLD_TEST_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR, _DOWN_REP_MSG_RDY, _YES, buffer[1]);
        interrupts.upReqMsgRdy                 = FLD_TEST_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR, _UP_REQ_MSG_RDY, _YES, buffer[1]);

        fetchLinkStatusLegacy();
        this->setDirtyLinkStatus(false);

        parseAutomatedTestRequest(automatedTestRequest);
    }

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

    virtual void clearInterruptContentProtection()
    {
        if (caps.supportsESI)
        {
            NvU8 irqVector = 0;

            irqVector = FLD_SET_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR_ESI0, _CP, _YES, irqVector);

            bus.write(NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0, &irqVector, sizeof irqVector);
        }
        else
        {
            NvU8 irqVector = 0;

            irqVector = FLD_SET_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR, _CP, _YES, irqVector);

            bus.write(NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR, &irqVector, sizeof irqVector);
        }
    }

    virtual bool intteruptMCCS()
    {
        return interrupts.mccsIRQ;
    }

    virtual void clearInterruptMCCS()
    {
        if (caps.supportsESI)
        {
            NvU8 irqVector = 0;
            irqVector = FLD_SET_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR_ESI0, _MCCS_IRQ, _YES, irqVector);
            bus.write(NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0, &irqVector, sizeof irqVector);
        }
        else
        {
            NvU8 irqVector = 0;
            irqVector = FLD_SET_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR, _MCCS_IRQ, _YES, irqVector);
            bus.write(NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR, &irqVector, sizeof irqVector);
        }
    }

    virtual bool interruptDownReplyReady()
    {
        return interrupts.downRepMsgRdy;
    }

    virtual bool interruptUpRequestReady()
    {
        return interrupts.upReqMsgRdy;
    }

    virtual void clearInterruptDownReplyReady()
    {
        if (caps.supportsESI)
        {
            NvU8 irqVector = 0;
            irqVector = FLD_SET_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR_ESI0, _DOWN_REP_MSG_RDY, _YES, irqVector);
            bus.write(NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0, &irqVector, sizeof irqVector);
        }
        else
        {
            NvU8 irqVector = 0;
            irqVector = FLD_SET_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR, _DOWN_REP_MSG_RDY, _YES, irqVector);
            bus.write(NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR, &irqVector, sizeof irqVector);
        }
    }

    virtual void clearInterruptUpRequestReady()
    {
        if (caps.supportsESI)
        {
            NvU8 irqVector = 0;
            irqVector = FLD_SET_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR_ESI0, _UP_REQ_MSG_RDY, _YES, irqVector);
            bus.write(NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0, &irqVector, sizeof irqVector);
        }
        else
        {
            NvU8 irqVector = 0;
            irqVector = FLD_SET_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR, _UP_REQ_MSG_RDY, _YES, irqVector);
            bus.write(NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR, &irqVector, sizeof irqVector);
        }
    }

    virtual bool getLaneStatusSymbolLock(int lane)
    {
        return interrupts.laneStatusIntr.laneStatus[lane].symbolLocked;
    }

    virtual bool getLaneStatusClockRecoveryDone(int lane)
    {
        return interrupts.laneStatusIntr.laneStatus[lane].clockRecoveryDone;
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

    virtual void getTestRequestTraining(LinkRate & rate, unsigned & lanes) // DPCD offset 219, 220
    {
        rate = interrupts.testTraining.testRequestLinkRate;
        lanes = interrupts.testTraining.testRequestLaneCount;
    }

    virtual LinkQualityPatternType getPhyTestPattern()                            // DPCD offset 248
    {
        return interrupts.testPhyCompliance.phyTestPattern;
    }

    virtual void getCustomTestPattern(NvU8 *testPattern)                         // DPCD offset 250 - 259
    {
        int i;

        for (i = 0; i < 10; i++)
        {
            testPattern[i] = interrupts.eightyBitCustomPat[i];
        }
    }

    virtual bool getBKSV(NvU8 *bKSV)           //DPCD offset 0x68000
    {
        if (caps.revisionMajor <= 0)
            DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

        if (AuxRetry::ack == bus.read(NV_DPCD_HDCP_BKSV_OFFSET, &bKSV[0], HDCP_KSV_SIZE))
        {
            DP_LOG(("Found HDCP Bksv= %02x %02x %02x %02x %02x",
                    bKSV[4], bKSV[3], bKSV[2], bKSV[1], bKSV[0]));
            return true;
        }
        return false;
    }

    virtual bool getBCaps(BCaps &bCaps, NvU8 * rawByte)             //DPCD offset 0x68028
    {
        NvU8 buffer;
        if (caps.revisionMajor <= 0)
            DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

        if (AuxRetry::ack == bus.read(NV_DPCD_HDCP_BCAPS_OFFSET, &buffer, sizeof buffer))
        {
            bCaps.HDCPCapable  = FLD_TEST_DRF(_DPCD, _HDCP_BCAPS_OFFSET, _HDCP_CAPABLE, _YES, buffer);
            bCaps.repeater     = FLD_TEST_DRF(_DPCD, _HDCP_BCAPS_OFFSET, _HDCP_REPEATER, _YES, buffer);
            if (rawByte)
                *rawByte = buffer;
            return true;
        }

        DP_ASSERT(!"Unable to get BCaps");
        return false;
    }

    virtual bool getHdcp22BCaps(BCaps &bCaps, NvU8 *rawByte)        //DPCD offset 0x6921D
    {
        NvU8 buffer;
        if (caps.revisionMajor <= 0)
            DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

        if (AuxRetry::ack == bus.read(NV_DPCD_HDCP22_BCAPS_OFFSET, &buffer, sizeof buffer))
        {
            bCaps.HDCPCapable  = FLD_TEST_DRF(_DPCD, _HDCP22_BCAPS_OFFSET, _HDCP_CAPABLE, _YES, buffer);
            bCaps.repeater     = FLD_TEST_DRF(_DPCD, _HDCP22_BCAPS_OFFSET, _HDCP_REPEATER, _YES, buffer);
            if (rawByte)
                *rawByte = buffer;
            return true;
        }

        DP_ASSERT(!"Unable to get 22BCaps");
        return false;
    }

    virtual bool getBinfo(BInfo &bInfo)            //DPCD offset 0x6802A
    {
        NvU16 buffer;
        if (caps.revisionMajor <= 0)
            DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

        if (AuxRetry::ack == bus.read(NV_DPCD_HDCP_BINFO_OFFSET, (NvU8*)&buffer, sizeof buffer))
        {
            bInfo.maxCascadeExceeded   = FLD_TEST_DRF(_DPCD_HDCP, _BINFO_OFFSET, _MAX_CASCADE_EXCEEDED, _TRUE, buffer);
            bInfo.depth                = DRF_VAL(_DPCD_HDCP, _BINFO_OFFSET, _DEPTH, buffer);
            bInfo.maxDevsExceeded      = FLD_TEST_DRF(_DPCD_HDCP, _BINFO_OFFSET, _MAX_DEVS_EXCEEDED, _TRUE, buffer);
            bInfo.deviceCount          = DRF_VAL(_DPCD_HDCP, _BINFO_OFFSET, _DEVICE_COUNT, buffer);
            return true;
        }

        DP_ASSERT(!"Unable to get Binfo");
        return false;
    }

    // Get RxStatus per provided HDCP cap
    virtual bool getRxStatus(const HDCPState &hdcpState, NvU8 *data)
    {
        if (caps.revisionMajor <= 0)
            DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

        NvU32 addr = hdcpState.HDCP_State_22_Capable ?
                     NV_DPCD_HDCP22_RX_STATUS : NV_DPCD_HDCP_BSTATUS_OFFSET;

        if (AuxRetry::ack == bus.read(addr, data, sizeof(NvU8)))
        {
            return true;
        }

        DP_ASSERT(!"Unable to get RxStatus//Bstatus");
        return false;
    }

    virtual AuxRetry::status setTestResponseChecksum(NvU8 checksum)
    {
        if (caps.revisionMajor <= 0)
            DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

        return bus.write(NV_DPCD_TEST_EDID_CHKSUM, &checksum, sizeof checksum);
    }

    virtual AuxRetry::status setTestResponse(bool ack, bool edidChecksumWrite)
    {
        NvU8 testResponse = 0;

        if (caps.revisionMajor <= 0)
            DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

        if (ack)
            testResponse = FLD_SET_DRF(_DPCD, _TEST_RESPONSE, _TEST_ACK, _YES, testResponse);
        else
            testResponse = FLD_SET_DRF(_DPCD, _TEST_RESPONSE, _TEST_NACK, _YES, testResponse);

        if (edidChecksumWrite)
            testResponse = FLD_SET_DRF(_DPCD, _TEST_RESPONSE, _TEST_EDID_CHKSUM_WRITE, _YES, testResponse);

        return bus.write(NV_DPCD_TEST_RESPONSE, &testResponse, sizeof testResponse);
    }

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
        return 48;
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

    virtual PowerState getPowerState()
    {
        NvU8 data;
        if (AuxRetry::ack != bus.read(NV_DPCD_SET_POWER, &data, sizeof data, 0))
        {
            // Assume powerdown state
            return PowerStateD3;
        }

        switch (DRF_VAL(_DPCD, _SET_POWER, _VAL, data))
        {
        case NV_DPCD_SET_POWER_VAL_D3_PWRDWN:
            return PowerStateD3;

        case NV_DPCD_SET_POWER_VAL_D0_NORMAL:
            return PowerStateD0;

        case NV_DPCD_SET_POWER_VAL_D3_AUX_ON:
            {
                DP_ASSERT(isAtLeastVersion(1, 2) && "DP 1.2 specific power state to be set on a non-DP1.2 system!?");
                return PowerStateD3AuxOn;
            }
        default:
            DP_ASSERT(0 && "Unknown power state! Assuming device is asleep");
            return PowerStateD3;
        }
    }

    virtual bool setPowerState(PowerState newState)
    {
        NvU8    timeoutMs = 0;

        if (newState == PowerStateD0)
            timeoutMs = caps.extendedSleepWakeTimeoutRequestMs;

        // Default behavior is 2ms for better tolerance.
        if (timeoutMs < 2)
            timeoutMs = 2;

        //
        // A Branch Device must forward this value to its downstream devices.
        // When set to D3 state, a Sink Device may put its AUX CH circuit in a "power
        // saving" state. In this mode the AUX CH circuit may only detect the presence of a
        // differential signal input without replying to an AUX CH request transaction. Upon
        // detecting the presence of a differential signal input, the Sink Device must exit the
        // "power saving" state within 1ms.
        //
        if (isAtLeastVersion(1, 1))
        {
            NvU8 data = 0;
            if (newState == PowerStateD0)
                data |= NV_DPCD_SET_POWER_VAL_D0_NORMAL;
            else if (newState == PowerStateD3)
            {
                if (caps.extendedSleepWakeTimeoutRequestMs > 1)
                {
                    NvU8    grant = 0;
                    // Grant extended sleep wake timeout before go D3.
                    grant = FLD_SET_DRF(_DPCD, _EXTENDED_DPRX_WAKE_TIMEOUT, _PERIOD_GRANTED, _YES, grant);
                    if (AuxRetry::ack == bus.write(NV_DPCD_EXTENDED_DPRX_WAKE_TIMEOUT, &grant, sizeof(grant)))
                    {
                        DP_LOG(("DisplayPort: Failed to grant extended sleep wake timeout before D3\n"));
                    }
                }
                data = NV_DPCD_SET_POWER_VAL_D3_PWRDWN;
            }
            else
            {
                DP_ASSERT(0 && "Unknown power state");
            }

            //
            //  If we're powering on, we need to allow up to 1ms for the power
            //  to come online.  Ideally we'd handle this with a callback,
            //  but for now we're going to do a wait here.
            //
            Timeout timeout(timer, timeoutMs);
            unsigned retries = 0;

            do
            {
                if (AuxRetry::ack == bus.write(NV_DPCD_SET_POWER, &data, sizeof(data)))
                {
                    return true;
                }
                retries++;
            }
            while (timeout.valid() || (retries < 40) /* some panels need up to 40 retries */);

            DP_LOG(("DisplayPort: Failed to bring panel back to wake state"));
        }
        else
        {
            //  DP 1.0 devices cannot be put to sleep
            if (newState == PowerStateD0)
                return true;
        }

        return false;
    }

    virtual void payloadTableClearACT()
    {
        NvU8 byte = NV_DPCD_PAYLOAD_TABLE_UPDATE_STATUS_UPDATED_YES;
        bus.write(NV_DPCD_PAYLOAD_TABLE_UPDATE_STATUS, &byte, sizeof byte);
    }

    virtual bool payloadWaitForACTReceived()
    {
        NvU8 byte = 0;
        int retries = 0;

        while (true)
        {
            if (++retries > 40)
            {
                DP_LOG(("DPHAL> ACT Not received by sink device!"));
                return false;
            }

            if (AuxRetry::ack == bus.read(NV_DPCD_PAYLOAD_TABLE_UPDATE_STATUS, &byte, sizeof byte))
            {
                if (FLD_TEST_DRF(_DPCD, _PAYLOAD_TABLE_UPDATE_STATUS, _ACT_HANDLED, _YES, byte))
                {
                    DP_LOG(("DPHAL> ACT Received"));
                    return true;
                }
            }
        }
    }

    virtual bool payloadAllocate(unsigned streamId, unsigned begin, unsigned count)
    {
        bool bResult = false;
        NvU8 payloadAllocate[3];
        DP_ASSERT(streamId < 64 && "Invalid stream location");
        payloadAllocate[0] = (NvU8)streamId;
        payloadAllocate[1] = (NvU8)begin;
        payloadAllocate[2] = (NvU8)count;

        AuxRetry::status status = bus.write(NV_DPCD_PAYLOAD_ALLOC_SET, (NvU8*)&payloadAllocate, sizeof payloadAllocate);

        if (status == AuxRetry::ack)
        {
            //
            // Bit 0 = VC Payload Table Updated(Change/Read only)
            //       1 = Update, cleared to zero when u Packet Source writes 1
            //       0 = Not updated since the last time this bit was cleared
            //
            NvU8 payloadStatus;
            int retries = 0;

            //
            // Bug 1385165 that Synaptics branch revision 1.0 found to spend more than 200ms before table updated.
            // Retries without delay is too soon for device to complete table update process.
            // That will hit bug 1334070 and trigger monitor unplug/hotplug at early return.
            //
            do
            {
                if ((bus.read(NV_DPCD_PAYLOAD_TABLE_UPDATE_STATUS, &payloadStatus, sizeof(payloadStatus)) == AuxRetry::ack))
                {
                    if (FLD_TEST_DRF(_DPCD, _PAYLOAD_TABLE_UPDATE_STATUS, _UPDATED, _YES, payloadStatus))
                    {
                        bResult = true;
                        break;
                    }
                }
                else
                {
                    DP_LOG(("DPHAL> Read NV_DPCD_PAYLOAD_TABLE_UPDATE_STATUS failed."));
                }

                timer->sleep(1);
            } while (++retries < PAYLOADIDTABLE_UPDATED_CHECK_RETRIES);
        }
        else
        {
            DP_LOG(("DPHAL> Send NV_DPCD_PAYLOAD_ALLOC_SET failed."));
        }

        DP_LOG(("DPHAL> Requesting allocation Stream:%d | First Slot:%d | Count:%d  (%s)", streamId, begin, count, bResult ? "OK" : "FAILED"));
        return bResult;
    }

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

    void setGpuDPSupportedVersions(bool supportDp1_2, bool supportDp1_4)
    {
        if (supportDp1_4)
            DP_ASSERT(supportDp1_2 && "GPU supports DP1.4 should also support DP1.2!");

        gpuDP1_2Supported = supportDp1_2;
        gpuDP1_4Supported = supportDp1_4;
    }

    void setGpuFECSupported(bool bSupportFEC)
    {
        bGpuFECSupported = bSupportFEC;
    }

    void applyRegkeyOverrides(const DP_REGKEY_DATABASE& dpRegkeyDatabase)
    {
        DP_ASSERT(dpRegkeyDatabase.bInitialized &&
                  "All regkeys are invalid because dpRegkeyDatabase is not initialized!");
        overrideDpcdRev          = dpRegkeyDatabase.dpcdRevOveride;
        bBypassILREdpRevCheck    = dpRegkeyDatabase.bBypassEDPRevCheck;
    }

    // To clear pending message {DOWN_REP/UP_REQ} and reply true if existed.
    virtual bool clearPendingMsg()
    {
        NvU8 irqVector, data = 0;
        if (AuxRetry::ack == bus.read(NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0,
                                      &irqVector, sizeof(irqVector)))
        {
            if (FLD_TEST_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR_ESI0, _DOWN_REP_MSG_RDY, _YES, irqVector))
            {
                // Clear pending DOWN_REP.
                data = FLD_SET_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR_ESI0, _DOWN_REP_MSG_RDY, _YES, 0);
            }
            if (FLD_TEST_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR_ESI0, _UP_REQ_MSG_RDY, _YES, irqVector))
            {
                // Clear pending UP_REQ
                data = FLD_SET_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR_ESI0, _UP_REQ_MSG_RDY, _YES, data);
            }
            if (!data ||
                (AuxRetry::ack != bus.write(NV_DPCD_DEVICE_SERVICE_IRQ_VECTOR_ESI0,
                                            &data, sizeof(data))))
            {
                DP_LOG(("DPCONN> %s(): No Pending Message or "
                        "Failed to clear pending message: irqVector/data = 0x%08x/0x%08x",
                        __FUNCTION__, irqVector, data));
                return false;
            }

            return true;
        }
        else
        {
            DP_LOG(("DPCONN> Clear Pending MSG: Failed to read ESI0"));
        }

        return false;
    }

    virtual bool isMessagingEnabled()
    {
        NvU8 mstmCtrl;

        if ((AuxRetry::ack == bus.read(NV_DPCD_MSTM_CTRL, &mstmCtrl, 1)) &&
            (FLD_TEST_DRF(_DPCD, _MSTM_CTRL, _EN, _YES, mstmCtrl)))
        {
            return true;
        }
        return false;
    }

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

    virtual NvU16 *getLinkRateTable()
    {
        if (!bIndexedLinkrateCapable)
        {
            DP_LOG(("DPCONN> link rate table should be invalid"));
        }
        return &caps.linkRateTable[0];
    }

    virtual NvU32 getVideoFallbackSupported()
    {
        return caps.videoFallbackFormats;
    }

    virtual bool getRawLinkRateTable(NvU8 *buffer)
    {
        NvU16 temp[NV_DPCD_SUPPORTED_LINK_RATES__SIZE];
        NvU8 *data = (buffer == NULL) ? (NvU8*)&temp[0] : buffer;

        if (AuxRetry::ack != bus.read(NV_DPCD_SUPPORTED_LINK_RATES(0), data,
                                      NV_DPCD_SUPPORTED_LINK_RATES__SIZE * sizeof(NvU16)))
        {
            return false;
        }
        return true;
    }

    virtual void resetProtocolConverter()
    {
        NvU8    data = 0;
        bus.write(NV_DPCD20_PCON_FRL_LINK_CONFIG_1, &data, sizeof(data));
        bus.write(NV_DPCD20_PCON_FRL_LINK_CONFIG_2, &data, sizeof(data));

    }

    virtual bool setSourceControlMode(bool bEnableSourceControlMode, bool bEnableFRLMode)
    {
        NvU8    data = 0;

        if (bEnableSourceControlMode)
        {
            data = FLD_SET_DRF(_DPCD20, _PCON_FRL_LINK_CONFIG_1, _SRC_CONTROL_MODE, _ENABLE, data);
            if (bEnableFRLMode)
            {
                data = FLD_SET_DRF(_DPCD20, _PCON_FRL_LINK_CONFIG_1, _LINK_FRL_MODE, _ENABLE, data);
                data = FLD_SET_DRF(_DPCD20, _PCON_FRL_LINK_CONFIG_1, _IRQ_LINK_FRL_MODE, _ENABLE, data);
            }
            else
            {
                data = FLD_SET_DRF(_DPCD20, _PCON_FRL_LINK_CONFIG_1, _LINK_FRL_MODE, _DISABLE, data);
                data = FLD_SET_DRF(_DPCD20, _PCON_FRL_LINK_CONFIG_1, _IRQ_LINK_FRL_MODE, _DISABLE, data);
            }
        }
        else
        {
            data = FLD_SET_DRF(_DPCD20, _PCON_FRL_LINK_CONFIG_1, _SRC_CONTROL_MODE, _DISABLE, data);
            data = FLD_SET_DRF(_DPCD20, _PCON_FRL_LINK_CONFIG_1, _LINK_FRL_MODE, _DISABLE, data);
            data = FLD_SET_DRF(_DPCD20, _PCON_FRL_LINK_CONFIG_1, _IRQ_LINK_FRL_MODE, _DISABLE, data);
        }

        if (AuxRetry::ack != bus.write(NV_DPCD20_PCON_FRL_LINK_CONFIG_1, &data, sizeof(data)))
        {
            return false;
        }
        return true;
    }

    virtual bool checkPCONFrlReady(bool *bFrlReady)
    {
        NvU8        data = 0;

        if (bFrlReady == NULL)
        {
            DP_ASSERT(0);
            return true;
        }

        *bFrlReady = false;

        if (AuxRetry::ack != bus.read(NV_DPCD_LINK_SERVICE_IRQ_VECTOR_ESI0, &data, sizeof(data)))
        {
            return false;
        }

        if (data == 0)
        {
            return false;
        }

        if (FLD_TEST_DRF(_DPCD, _LINK_SERVICE_IRQ_VECTOR_ESI0, _HDMI_LINK_STATUS_CHANGED, _NO, data))
        {
            parseAndReadInterruptsESI();
            return false;
        }

        // Clear only this interrupt bit.
        this->clearHdmiLinkStatusChanged();

        if (AuxRetry::ack != bus.read(NV_DPCD20_PCON_HDMI_TX_LINK_STATUS, &data, sizeof(data)))
        {
            return false;
        }

        if (FLD_TEST_DRF(_DPCD20, _PCON_HDMI_TX_LINK_STATUS, _LINK_READY, _YES, data))
        {
            *bFrlReady = true;
        }
        return true;
    }

    virtual bool setupPCONFrlLinkAssessment(NvU32   linkBwMask,
                                            bool    bEnableExtendLTMode = false,
                                            bool    bEnableConcurrentMode = false)
    {
        NvU8        data = 0;

        NvU32       requestedMaxBw  = (NvU32)(getMaxFrlBwFromMask(linkBwMask)) + 1; // +1 to convert PCONHdmiLinkBw enum to DPCD FRL BW cap definition
        NvU32       targetBw        = NV_MIN(caps.pconCaps.maxHdmiLinkBandwidthGbps,
                                             requestedMaxBw);

        // Step 1: Configure FRL Link (FRL BW, BW mask / Concurrent)
        if (bEnableExtendLTMode)
        {
            //
            // Set FRL_LT_CONTROL to Extended mode:
            // PCON FW trains for all Link BW selected in Link BW Mask (Bit 0~5)
            //
            data = linkBwMask;
            data = FLD_SET_DRF(_DPCD20, _PCON_FRL_LINK_CONFIG_2, _FRL_LT_CONTROL,
                               _EXTENDED, data);
        }
        else
        {
            // Set FRL_LT_CONTROL to Normal mode, so PCON stops when first FRL LT succeed.
            data = FLD_SET_DRF(_DPCD20, _PCON_FRL_LINK_CONFIG_2, _FRL_LT_CONTROL,
                               _NORMAL, data);
        }

        if (AuxRetry::ack != bus.write(NV_DPCD20_PCON_FRL_LINK_CONFIG_2, &data, sizeof(data)))
        {
            return false;
        }

        if (AuxRetry::ack != bus.read(NV_DPCD20_PCON_FRL_LINK_CONFIG_1, &data, sizeof(data)))
        {
            return false;
        }

        if (bEnableConcurrentMode && caps.pconCaps.bConcurrentLTSupported)
        {
            // Client selects concurrent.
            data = FLD_SET_DRF(_DPCD20, _PCON_FRL_LINK_CONFIG_1, _CONCURRENT_LT_MODE,
                               _ENABLE, data);
        }
        else
        {
            //
            // Don't do concurrent LT for now.
            //
            data = FLD_SET_DRF(_DPCD20, _PCON_FRL_LINK_CONFIG_1, _CONCURRENT_LT_MODE,
                               _DISABLE, data);
        }
        data = FLD_SET_DRF(_DPCD20, _PCON_FRL_LINK_CONFIG_1, _HDMI_LINK,
                           _ENABLE, data);
        data = FLD_SET_DRF_NUM(_DPCD20, _PCON_FRL_LINK_CONFIG_1, _MAX_LINK_BW,
                               targetBw, data);

        if (AuxRetry::ack != bus.write(NV_DPCD20_PCON_FRL_LINK_CONFIG_1, &data, sizeof(data)))
        {
            return false;
        }

        return true;
    }

    virtual bool checkPCONFrlLinkStatus(NvU32 *frlRateMask)
    {
        NvU8        data = 0;

        if (frlRateMask == NULL)
        {
            DP_ASSERT(0);
            return true;
        }

        *frlRateMask = 0;
        // Check if IRQ happens.
        if (AuxRetry::ack != bus.read(NV_DPCD_LINK_SERVICE_IRQ_VECTOR_ESI0, &data, sizeof(data)))
        {
            return false;
        }

        if (FLD_TEST_DRF(_DPCD, _LINK_SERVICE_IRQ_VECTOR_ESI0, _HDMI_LINK_STATUS_CHANGED, _NO, data))
        {
            return false;
        }
        // Check HDMI Link Active status (0x303B Bit 0) and Link Config (0x3036)
        if (AuxRetry::ack != bus.read(NV_DPCD20_PCON_HDMI_TX_LINK_STATUS, &data, sizeof(data)))
        {
            return false;
        }

        if (FLD_TEST_DRF(_DPCD20, _PCON_HDMI_TX_LINK_STATUS, _LINK_ACTIVE, _YES, data))
        {
            if (AuxRetry::ack == bus.read(NV_DPCD20_PCON_HDMI_LINK_CONFIG_STATUS, &data, sizeof(data)))
            {
                *frlRateMask = DRF_VAL(_DPCD20, _PCON_HDMI_LINK_CONFIG_STATUS, _LT_RESULT, data);
            }

        }

        return true;
    }

    virtual bool queryHdmiLinkStatus(bool *bLinkActive, bool *bLinkReady)
    {
        NvU8        data = 0;

        if (bLinkReady == NULL && bLinkReady == NULL)
            return false;

        if (AuxRetry::ack != bus.read(NV_DPCD20_PCON_HDMI_TX_LINK_STATUS, &data, sizeof(data)))
        {
            return false;
        }
        if (bLinkReady != NULL)
        {
            *bLinkReady = (FLD_TEST_DRF(_DPCD20, _PCON_HDMI_TX_LINK_STATUS,
                                        _LINK_READY, _YES, data));
        }
        if (bLinkActive != NULL)
        {
            *bLinkActive = (FLD_TEST_DRF(_DPCD20, _PCON_HDMI_TX_LINK_STATUS,
                                         _LINK_ACTIVE, _YES, data));
        }
        return true;
    }

    virtual NvU32 restorePCONFrlLink(NvU32   linkBwMask,
                                     bool    bEnableExtendLTMode     = false,
                                     bool    bEnableConcurrentMode   = false)
    {
        // Restore HDMI Link.
        // 1. Clear HDMI link enable bit (305A bit 7)
        NvU8    data = 0;
        NvU32   loopCount;
        NvU32   frlRate;
        if (AuxRetry::ack != bus.read(NV_DPCD20_PCON_FRL_LINK_CONFIG_1, &data, sizeof(data)))
        {
            return false;
        }
        data = FLD_SET_DRF(_DPCD20, _PCON_FRL_LINK_CONFIG_1, _HDMI_LINK, _DISABLE, data);
        if (AuxRetry::ack != bus.write(NV_DPCD20_PCON_FRL_LINK_CONFIG_1, &data, sizeof(data)))
        {
            return false;
        }
        // 2. Set FRL or TMDS (Optional if not changed) (305A bit 5)
        // 3. Read FRL Ready Bit (303B bit 1)

        Timeout timeout(timer, 500 /* 500ms */);
        data = 0;
        do
        {
            if (AuxRetry::ack != bus.read(NV_DPCD20_PCON_HDMI_TX_LINK_STATUS,
                                          &data, sizeof(data)))
                continue;
            if (FLD_TEST_DRF(_DPCD20, _PCON_HDMI_TX_LINK_STATUS, _LINK_READY, _YES, data))
                break;
        } while (timeout.valid());

        if (FLD_TEST_DRF(_DPCD20, _PCON_HDMI_TX_LINK_STATUS, _LINK_READY, _NO, data))
        {
            return false;
        }

        // 4. Configure FRL Link (Optional if not changed)
        // 5. Set HDMI Enable Bit.
        data = 0;

        if (AuxRetry::ack != bus.read(NV_DPCD20_PCON_FRL_LINK_CONFIG_1, &data, sizeof(data)))
        {
            return false;
        }
        data = FLD_SET_DRF(_DPCD20, _PCON_FRL_LINK_CONFIG_1, _HDMI_LINK, _ENABLE, data);
        if (AuxRetry::ack != bus.write(NV_DPCD20_PCON_FRL_LINK_CONFIG_1, &data, sizeof(data)))
        {
            return false;
        }

        // 6. Read HDMI Link Status link active bit (2005 bit 3)
        // 7. Read HDMI link active status bit and link config status (303b bit0 / 3036)
        loopCount = NV_PCON_FRL_LT_TIMEOUT_THRESHOLD;
        do
        {
            if (checkPCONFrlLinkStatus(&frlRate) == true)
            {
                break;
            }
            Timeout timeout(this->timer, NV_PCON_FRL_LT_TIMEOUT_INTERVAL_MS);
            while(timeout.valid());
            continue;
        } while (--loopCount);

        return frlRate;
    }

    virtual void readPsrCapabilities(vesaPsrSinkCaps *caps)
    {
        dpMemCopy(caps, &this->caps.psrCaps, sizeof(vesaPsrSinkCaps));
    }

    virtual bool updatePsrConfiguration(vesaPsrConfig psrcfg)
    {
        NvU8 config = 0U;

        if (psrcfg.psrCfgEnable)
        {
            config = FLD_SET_DRF(_DPCD_EDP, _PSR_CONFIG,
                _SINK_ENABLE, _YES, config);
        }
        if (psrcfg.srcTxEnabledInPsrActive)
        {
            config = FLD_SET_DRF(_DPCD_EDP, _PSR_CONFIG,
                _SOURCE_LINK_ACTIVE, _YES, config);
        }
        if (psrcfg.crcVerifEnabledInPsrActive)
        {
            config = FLD_SET_DRF(_DPCD_EDP, _PSR_CONFIG,
                _CRC_VERIFICATION_ACTIVE, _YES, config);
        }
        if (psrcfg.frameCaptureSecondActiveFrame)
        {
            config = FLD_SET_DRF(_DPCD_EDP, _PSR_CONFIG,
                _FRAME_CAPTURE_INDICATION, _SECOND, config);
        }
        if (psrcfg.selectiveUpdateOnSecondActiveline)
        {
            config = FLD_SET_DRF(_DPCD_EDP, _PSR_CONFIG,
                _SU_LINE_CAPTURE_INDICATION, _SECOND, config);
        }
        if (psrcfg.enableHpdIrqOnCrcMismatch)
        {
            config = FLD_SET_DRF(_DPCD_EDP, _PSR_CONFIG,
                _HPD_IRQ_ON_CRC_ERROR, _YES, config);
        }
        if (psrcfg.enablePsr2)
        {
            config = FLD_SET_DRF(_DPCD_EDP, _PSR_CONFIG,
                _ENABLE_PSR2, _YES, config);
        }

        return AuxRetry::ack ==
            bus.write(NV_DPCD_EDP_PSR_CONFIG, &config, 1);
    }

    virtual bool readPsrConfiguration(vesaPsrConfig *psrcfg)
    {
        NvU8 config = 0U;
        bool retVal = AuxRetry::ack ==
            bus.read(NV_DPCD_EDP_PSR_CONFIG, &config, 1);

        psrcfg->psrCfgEnable =
            FLD_TEST_DRF(_DPCD_EDP, _PSR_CONFIG, _SINK_ENABLE, _YES, config);
        psrcfg->srcTxEnabledInPsrActive =
            FLD_TEST_DRF(_DPCD_EDP, _PSR_CONFIG, _SOURCE_LINK_ACTIVE, _YES, config);
        psrcfg->crcVerifEnabledInPsrActive =
            FLD_TEST_DRF(_DPCD_EDP, _PSR_CONFIG, _CRC_VERIFICATION_ACTIVE,
                _YES, config);
        psrcfg->frameCaptureSecondActiveFrame =
            FLD_TEST_DRF(_DPCD_EDP, _PSR_CONFIG, _FRAME_CAPTURE_INDICATION,
                _SECOND, config);
        psrcfg->selectiveUpdateOnSecondActiveline =
            FLD_TEST_DRF(_DPCD_EDP, _PSR_CONFIG,
                _SU_LINE_CAPTURE_INDICATION, _SECOND, config);
        psrcfg->enableHpdIrqOnCrcMismatch =
            FLD_TEST_DRF(_DPCD_EDP, _PSR_CONFIG, _HPD_IRQ_ON_CRC_ERROR, _YES, config);
        psrcfg->enablePsr2 =
            FLD_TEST_DRF(_DPCD_EDP, _PSR_CONFIG, _ENABLE_PSR2, _YES, config);

        return retVal;
    }

    virtual bool readPsrState(vesaPsrState *psrState)
    {
        NvU8 config = 0U;
        bool retVal = AuxRetry::ack ==
            bus.read(NV_DPCD_PANEL_SELF_REFRESH_STATUS, &config, 1);

        if (retVal)
        {
            *psrState =
                (vesaPsrState)DRF_VAL(_DPCD, _PANEL_SELF_REFRESH_STATUS,
                    _VAL, config);
        }
        return retVal;
    }

    virtual bool readPsrDebugInfo(vesaPsrDebugStatus *psrDbgState)
    {
        NvU8 config[2] = { 0U , 0U };
        bool retVal = AuxRetry::ack ==
            bus.read(NV_DPCD_PANEL_SELF_REFRESH_DEBUG0,
                &config[0], sizeof(config));

        if (retVal)
        {
            psrDbgState->maxResyncFrames    =
                DRF_VAL(_DPCD_PANEL_SELF_REFRESH,
                        _DEBUG0, _MAX_RESYNC_FRAME_CNT, config[0]);
            psrDbgState->actualResyncFrames =
                DRF_VAL(_DPCD_PANEL_SELF_REFRESH,
                        _DEBUG0, _LAST_RESYNC_FRAME_CNT, config[0]);

            psrDbgState->lastSdpPsrState =
                !!DRF_VAL(_DPCD_PANEL_SELF_REFRESH, _LAST_SDP,
                    _PSR_STATE_BIT, config[1]);
            psrDbgState->lastSdpUpdateRfb =
                !!DRF_VAL(_DPCD_PANEL_SELF_REFRESH, _LAST_SDP,
                    _RFB_BIT, config[1]);
            psrDbgState->lastSdpCrcValid =
                !!DRF_VAL(_DPCD_PANEL_SELF_REFRESH, _LAST_SDP,
                    _CRC_VALID_BIT, config[1]);
            psrDbgState->lastSdpSuValid  =
                !!DRF_VAL(_DPCD_PANEL_SELF_REFRESH, _LAST_SDP,
                    _SU_VALID_BIT, config[1]);
            psrDbgState->lastSdpFirstSURcvd =
                !!DRF_VAL(_DPCD_PANEL_SELF_REFRESH, _LAST_SDP,
                    _SU_FIRST_LINE_RCVD, config[1]);
            psrDbgState->lastSdpLastSURcvd =
                !!DRF_VAL(_DPCD_PANEL_SELF_REFRESH, _LAST_SDP,
                    _SU_LAST_LINE_RCVD, config[1]);
            psrDbgState->lastSdpYCoordValid =
                !!DRF_VAL(_DPCD_PANEL_SELF_REFRESH, _LAST_SDP,
                    _Y_CORD_VALID, config[1]);
        }
        return retVal;
    }

    virtual bool writePsrErrorStatus(vesaPsrErrorStatus psrErr)
    {
        NvU8 config = 0U;
        config = FLD_SET_DRF_NUM(_DPCD_PANEL_SELF_REFRESH,
            _ERR_STATUS,
            _LINK_CRC_ERR,
            psrErr.linkCrcError,
            config);
        config = FLD_SET_DRF_NUM(_DPCD_PANEL_SELF_REFRESH,
            _ERR_STATUS,
            _RFB_ERR,
            psrErr.rfbStoreError,
            config);
        config = FLD_SET_DRF_NUM(_DPCD_PANEL_SELF_REFRESH,
            _ERR_STATUS,
            _VSC_SDP_ERR,
            psrErr.vscSdpError,
            config);

        return AuxRetry::ack == bus.write(
            NV_DPCD_PANEL_SELF_REFRESH_ERR_STATUS, &config, 1);
    }

    virtual bool readPsrErrorStatus(vesaPsrErrorStatus *psrErr)
    {
        NvU8 config = 0U;
        bool retVal;
        retVal = AuxRetry::ack == bus.read(
                                     NV_DPCD_PANEL_SELF_REFRESH_ERR_STATUS,
                                     &config, sizeof(config));

        if (retVal)
        {
            psrErr->vscSdpError = FLD_TEST_DRF(_DPCD,
                                               _PANEL_SELF_REFRESH_ERR_STATUS,
                                               _LINK_CRC_ERR, _YES, config);
            psrErr->rfbStoreError = FLD_TEST_DRF(_DPCD,
                                               _PANEL_SELF_REFRESH_ERR_STATUS,
                                               _RFB_ERR, _YES, config);
            psrErr->linkCrcError = FLD_TEST_DRF(_DPCD,
                                               _PANEL_SELF_REFRESH_ERR_STATUS,
                                               _VSC_SDP_ERR,
                                               _YES, config);
        }
        return retVal;
    }

    virtual bool writePsrEvtIndicator(vesaPsrEventIndicator psrEvt)
    {
        NvU8 config = 0U;

        if (psrEvt.sinkCapChange)
        {
            config = FLD_SET_DRF(_DPCD,
                _PANEL_SELF_REFRESH_EVENT_STATUS,
                _CAP_CHANGE,
                _YES, config);
        }
        return AuxRetry::ack == bus.write(
            NV_DPCD_PANEL_SELF_REFRESH_EVENT_STATUS, &config, 1);
    }

    virtual bool readPsrEvtIndicator(vesaPsrEventIndicator *psrEvt)
    {
        NvU8 config = 0U;
        bool retVal;
        retVal = AuxRetry::ack == bus.read(
            NV_DPCD_PANEL_SELF_REFRESH_EVENT_STATUS,
            &config, sizeof(config));

        if (retVal)
        {
            psrEvt->sinkCapChange = DRF_VAL(_DPCD,
                _PANEL_SELF_REFRESH_EVENT_STATUS,
                _CAP_CHANGE,
                config);
        }
        return retVal;
    }
};

DPCDHAL * DisplayPort::MakeDPCDHAL(AuxBus *  bus, Timer * timer)
{
    return new DPCDHALImpl(bus, timer);
}
