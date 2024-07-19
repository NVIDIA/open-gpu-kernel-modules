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
* Module: dp_configcaps.cpp                                                 *
*    Abstraction for basic caps registers                                   *
*                                                                           *
\***************************************************************************/
#include "dp_internal.h"
#include "dp_auxbus.h"
#include "dp_configcaps.h"
#include "dp_auxdefs.h"
#include "dp_printf.h"
#include "displayport.h"

using namespace DisplayPort;

void DPCDHALImpl::updateDPCDOffline()
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

bool DPCDHALImpl::auxAccessAvailable()
{
    NvU8 buffer[16];
    unsigned retries = 16;
    // Burst read from 0x00 to 0x0F.
    if (AuxRetry::ack != bus.read(NV_DPCD_REV, &buffer[0], sizeof buffer, retries))
    {
        return false;
    }
    else
    {
        return true;
    }
}

/*!
 * @brief Enable/Disable DP Tunnel BW allocation depending on support and client request
 *
 * @return      Boolean to indicate success or failure
 */
void DPCDHALImpl::configureDpTunnelBwAllocation()
{
    NvU8 byte = 0;

    // Client has not requested or it is not enabled due to regkey override
    if (!this->bEnableDpTunnelBwAllocationSupport)
    {
        bIsDpTunnelBwAllocationEnabled = false;
        return;
    }

    bIsDpTunnelBwAllocationEnabled = false;

    if (AuxRetry::ack ==
        bus.read(NV_DPCD20_DP_TUNNEL_CAPABILITIES, &byte, sizeof byte))
    {
        caps.dpInTunnelingCaps.bIsSupported =
            FLD_TEST_DRF(_DPCD20, _DP_TUNNEL_CAPABILITIES,
                            _DPTUNNELING_SUPPORT, _YES, byte);

        caps.dpInTunnelingCaps.bIsPanelReplayOptimizationSupported =
            FLD_TEST_DRF(_DPCD20, _DP_TUNNEL_CAPABILITIES,
                            _PANEL_REPLAY_TUNNELING_OPTIMIZATION_SUPPORT,
                            _YES, byte);

        caps.dpInTunnelingCaps.bIsBwAllocationSupported =
            FLD_TEST_DRF(_DPCD20, _DP_TUNNEL_CAPABILITIES,
                            _DPIN_BW_ALLOCATION_MODE_SUPPORT,
                            _YES, byte);
    }


    if (caps.dpInTunnelingCaps.bIsSupported)
    {
        if (AuxRetry::ack ==
            bus.read(NV_DPCD20_USB4_DRIVER_BW_CAPABILITY, &byte, sizeof byte))
        {
            caps.dpInTunnelingCaps.bUsb4DriverBwAllocationSupport =
                FLD_TEST_DRF(_DPCD20, _USB4_DRIVER, _BW_ALLOCATION, _YES, byte);
        }
    }

    bool bIsDpTunnelBwAllocationSupported = false;

    bIsDpTunnelBwAllocationSupported = caps.dpInTunnelingCaps.bIsSupported &&
                                       caps.dpInTunnelingCaps.bUsb4DriverBwAllocationSupport &&
                                       caps.dpInTunnelingCaps.bIsBwAllocationSupported;

    if (bIsDpTunnelBwAllocationEnabled == bIsDpTunnelBwAllocationSupported)
    {
        DP_PRINTF(DP_NOTICE, "Bw Allocation already in requested state: %d", bIsDpTunnelBwAllocationSupported);
        return;
    }

    if (!setDpTunnelBwAllocation(bIsDpTunnelBwAllocationSupported))
    {
        DP_PRINTF(DP_ERROR, "Failed to set DP Tunnel BW allocation");
    }

    if (bIsDpTunnelBwAllocationEnabled != bIsDpTunnelBwAllocationSupported)
    {
        DP_PRINTF(DP_WARNING, "Unable to set BW allocation to requested state: %d", bIsDpTunnelBwAllocationSupported);
    }
}

void DPCDHALImpl::parseAndReadCaps()
{
    NvU8 buffer[16];
    NvU8 byte = 0;
    AuxRetry::status status;
    unsigned retries = 16;

    NvU8 lttprIdx = 0;

    caps.phyRepeaterCount = 0;

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

    configureDpTunnelBwAllocation();

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
    // Support ESI register space only when GPU support DP1.2MST
    caps.supportsESI = (isAtLeastVersion(1,2) &&
                        FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_DP_VERSIONS_SUPPORTED, _DP1_2, _YES, gpuDPSupportedVersions));


    if (caps.eDpRevision >= NV_DPCD_EDP_REV_VAL_1_4 || this->bBypassILREdpRevCheck)
    {
        NvU16 linkRate = 0;
        if (getRawLinkRateTable((NvU8*)&caps.linkRateTable[0]))
        {
            // First entry must be non-zero for validation
            if (caps.linkRateTable[0] != 0)
            {
                bIndexedLinkrateCapable = true;
                for (int i = 0; (i < NV_DPCD_SUPPORTED_LINK_RATES__SIZE) && caps.linkRateTable[i]; i++)
                {
                    if (linkRate < caps.linkRateTable[i])
                        linkRate = caps.linkRateTable[i];
                }
                if (linkRate)
                    caps.maxLinkRate = LINK_RATE_200KHZ_TO_10MHZ((NvU64)linkRate);
            }
        }
    }
    if (!bIndexedLinkrateCapable)
    {
        if (FLD_TEST_DRF(_DPCD, _MAX_LINK_BANDWIDTH, _VAL, _1_62_GBPS, buffer[1]))
            caps.maxLinkRate = dp2LinkRate_1_62Gbps;
        else if (FLD_TEST_DRF(_DPCD, _MAX_LINK_BANDWIDTH, _VAL, _2_70_GBPS, buffer[1]))
            caps.maxLinkRate = dp2LinkRate_2_70Gbps;
        else if (FLD_TEST_DRF(_DPCD, _MAX_LINK_BANDWIDTH, _VAL, _5_40_GBPS, buffer[1]))
            caps.maxLinkRate = dp2LinkRate_5_40Gbps;
        else if (FLD_TEST_DRF(_DPCD14, _MAX_LINK_BANDWIDTH, _VAL, _8_10_GBPS, buffer[1]))
            caps.maxLinkRate = dp2LinkRate_8_10Gbps;
        else
        {
            DP_ASSERT(0 && "Unknown max link rate. Assuming DP 1.1 defaults");
            caps.maxLinkRate = dp2LinkRate_2_70Gbps;
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

    if (isAtLeastVersion(1,2) &&
        FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_DP_VERSIONS_SUPPORTED, _DP1_2, _YES, gpuDPSupportedVersions) &&
        caps.bPostLtAdjustmentSupport)
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
        DP_PRINTF(DP_WARNING, "DPHAL> Non-compliant device, reporting downstream port present, but no downstream ports. Overriding port count to 1.");
        caps.downStreamPortCount = 1;
    }

    // Burst read from 0x20 to 0x22.
    bus.read(NV_DPCD_SINK_VIDEO_FALLBACK_FORMATS, &buffer[0], 0x22 - 0x20 + 1);

    caps.videoFallbackFormats = buffer[0];

    caps.supportsMultistream = FLD_TEST_DRF(_DPCD, _MSTM, _CAP, _YES, buffer[0x1]);

    caps.numberAudioEndpoints = (unsigned)(DRF_VAL(_DPCD, _NUMBER_OF_AUDIO_ENDPOINTS, _VALUE, buffer[0x2]));

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
                        caps.repeaterCaps.maxLinkRate = dp2LinkRate_1_62Gbps;
                    else if (FLD_TEST_DRF(_DPCD14, _MAX_LINK_RATE_PHY_REPEATER, _VAL, _2_70_GBPS, buffer[1]))
                        caps.repeaterCaps.maxLinkRate = dp2LinkRate_2_70Gbps;
                    else if (FLD_TEST_DRF(_DPCD14, _MAX_LINK_RATE_PHY_REPEATER, _VAL, _5_40_GBPS, buffer[1]))
                        caps.repeaterCaps.maxLinkRate = dp2LinkRate_5_40Gbps;
                    else if (FLD_TEST_DRF(_DPCD14, _MAX_LINK_RATE_PHY_REPEATER, _VAL, _8_10_GBPS, buffer[1]))
                        caps.repeaterCaps.maxLinkRate = dp2LinkRate_8_10Gbps;
                    else
                    {
                        DP_ASSERT(0 && "Unknown max link rate or HBR2 without at least DP 1.2. Assuming DP 1.1 defaults");
                        caps.repeaterCaps.maxLinkRate = dp2LinkRate_2_70Gbps;
                    }

                    caps.repeaterCaps.maxLaneCount =
                        DRF_VAL(_DPCD14, _MAX_LANE_COUNT_PHY_REPEATER,
                                _VAL, buffer[0x4]);

                    // The cumulative number of 10ms.
                    caps.repeaterCaps.phyRepeaterExtendedWakeTimeoutMs =
                        DRF_VAL(_DPCD14,
                                _PHY_REPEATER_EXTENDED_WAKE_TIMEOUT,
                                _REQ, buffer[0x5]) * 10;

                    // Set FEC to Capable for repeater by default.
                    caps.repeaterCaps.bFECSupported = true;
                    for (lttprIdx = 0; lttprIdx < caps.phyRepeaterCount; lttprIdx++)
                    {
                        if (AuxRetry::ack ==
                                bus.read(NV_DPCD14_PHY_REPEATER_FEC_CAP_0(lttprIdx), &byte, 1))
                        {
                            caps.repeaterCaps.bFECSupportedRepeater[lttprIdx] =
                                            FLD_TEST_DRF(_DPCD14,
                                                         _PHY_REPEATER_FEC_CAP_0,
                                                         _FEC_CAPABLE,
                                                         _YES,
                                                         byte);

                            // bFECSupported is only true if all LTTPR supports FEC.
                            caps.repeaterCaps.bFECSupported &=
                                            caps.repeaterCaps.bFECSupportedRepeater[lttprIdx];
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

    if (bIsDpTunnelBwAllocationEnabled)
    {
        AuxRetry::status busReadStatus = bus.read(NV_DPCD20_DP_TUNNELING_MAX_LANE_COUNT, &byte, sizeof byte);

        if (AuxRetry::ack == busReadStatus)
        {
            caps.dpInTunnelingCaps.maxLaneCount = DRF_VAL(_DPCD20, _DP_TUNNELING_MAX_LANE_COUNT, _LANE, byte);
        }
        else
        {
            DP_PRINTF(DP_ERROR, "Failed to read NV_DPCD20_DP_TUNNELING_MAX_LANE_COUNT. Return: %d", busReadStatus);
        }

        busReadStatus = bus.read(NV_DPCD20_DP_TUNNELING_8B10B_MAX_LINK_RATE, &byte, sizeof byte);
        if (AuxRetry::ack == busReadStatus)
        {
            if (FLD_TEST_DRF(_DPCD20, _DP_TUNNELING_8B10B, _MAX_LINK_RATE_VAL, _1_62_GBPS, byte))
            {
                caps.dpInTunnelingCaps.maxLinkRate = dp2LinkRate_1_62Gbps;
            }
            else if (FLD_TEST_DRF(_DPCD20, _DP_TUNNELING_8B10B, _MAX_LINK_RATE_VAL, _2_70_GBPS, byte))
            {
                caps.dpInTunnelingCaps.maxLinkRate = dp2LinkRate_2_70Gbps;
            }
            else if (FLD_TEST_DRF(_DPCD20, _DP_TUNNELING_8B10B, _MAX_LINK_RATE_VAL, _5_40_GBPS, byte))
            {
                caps.dpInTunnelingCaps.maxLinkRate = dp2LinkRate_5_40Gbps;
            }
            else if (FLD_TEST_DRF(_DPCD20, _DP_TUNNELING_8B10B, _MAX_LINK_RATE_VAL, _8_10_GBPS, byte))
            {
                caps.dpInTunnelingCaps.maxLinkRate = dp2LinkRate_8_10Gbps;
            }
            else
            {
                DP_ASSERT(0 && "Unknown max link rate. Assuming DP 1.1 defaults");
                caps.dpInTunnelingCaps.maxLinkRate = dp2LinkRate_2_70Gbps;
            }
        }
        else
        {
            DP_PRINTF(DP_ERROR, "Failed to read NV_DPCD20_DP_TUNNELING_8B10B_MAX_LINK_RATE. Return: %d", busReadStatus);
        }

    }
    parsePortDescriptors();
}

//
// Legacy link rates: DPCD offset 1 * 27000000
// ILRs: DPCD offset: 0x10~0x1F
//
LinkRate DPCDHALImpl::getMaxLinkRate()
{
    LinkRate maxLinkRate = caps.maxLinkRate;

    if (caps.phyRepeaterCount > 0)
    {
        maxLinkRate = DP_MIN(maxLinkRate, caps.repeaterCaps.maxLinkRate);
    }

    if (caps.dpInTunnelingCaps.bIsSupported && bIsDpTunnelBwAllocationEnabled)
    {
        maxLinkRate = DP_MIN(maxLinkRate, caps.dpInTunnelingCaps.maxLinkRate);
    }

    return maxLinkRate;
}

unsigned DPCDHALImpl::getMaxLaneCount()
{
    unsigned maxLaneCount = caps.maxLaneCount;

    if (caps.phyRepeaterCount > 0)
    {
        maxLaneCount = DP_MIN(maxLaneCount, caps.repeaterCaps.maxLaneCount);
    }

    if (caps.dpInTunnelingCaps.bIsSupported && bIsDpTunnelBwAllocationEnabled)
    {
        maxLaneCount = DP_MIN(maxLaneCount, caps.dpInTunnelingCaps.maxLaneCount);
    }

    return maxLaneCount;
}

// Max lanes supported at the desired link rate.
unsigned DPCDHALImpl::getMaxLaneCountSupportedAtLinkRate(LinkRate linkRate)
{
    if (linkRate == dp2LinkRate_2_70Gbps)
    {
        if (caps.maxLanesAtHBR)
        {
            return DP_MIN(caps.maxLanesAtHBR, getMaxLaneCount());
        }
    }
    else if (linkRate == dp2LinkRate_1_62Gbps)
    {
        if (caps.maxLanesAtRBR)
        {
            return DP_MIN(caps.maxLanesAtRBR, getMaxLaneCount());
        }
    }
    // None of the above cases got hit, simply return the max lane count
    return getMaxLaneCount();
}

//
// Single stream specific caps
// DPCD offset 22h
//
unsigned DPCDHALImpl::getNumberOfAudioEndpoints()
{
    if (caps.numberAudioEndpoints)
        return caps.numberAudioEndpoints;
    else
        return caps.NORP > 1;
}

bool DPCDHALImpl::getSDPExtnForColorimetry()
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

bool DPCDHALImpl::getRootAsyncSDPSupported()
{
    NvU8 byte = 0;
    if (!caps.extendedRxCapsPresent)
        return false;
    if (AuxRetry::ack != bus.read(NV_DPCD14_DPRX_FEATURE_ENUM_LIST, &byte,  sizeof byte) ||
        FLD_TEST_DRF(_DPCD14, _DPRX_FEATURE_ENUM_LIST, _ADAPTIVE_SYNC_SDP_SUPPORTED, _NO, byte))
    {
        return false;
    }
    if (AuxRetry::ack != bus.read(NV_DPCD_DOWN_STREAM_PORT, &byte,  sizeof byte) ||
        FLD_TEST_DRF(_DPCD, _DOWN_STREAM_PORT, _MSA_TIMING_PAR_IGNORED, _NO, byte))
    {
        return false;
    }
    return true;
}

AuxRetry::status DPCDHALImpl::setOuiSource
(
    unsigned ouiId,
    const char * model,
    size_t modelNameLength,
    NvU8 chipRevision
)
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
        DP_PRINTF(DP_WARNING, "DPHAL> setOuiSource(): modelNameLength should not be greater than 6");
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

bool DPCDHALImpl::getOuiSource(unsigned &ouiId, char * modelName,
                               size_t modelNameBufferSize, NvU8 & chipRevision)
{
    NvU8 ouiBuffer[16];
    int address = NV_DPCD_SOURCE_IEEE_OUI;

    if (caps.revisionMajor <= 0)
        DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

    // If buffer size is larger than dev_id size, the extras are not used.
    // If buffer size is smaller, than we can only get certain bytes.
    if (modelNameBufferSize > NV_DPCD_SOURCE_DEV_ID_STRING__SIZE)
    {
        modelNameBufferSize = NV_DPCD_SOURCE_DEV_ID_STRING__SIZE;
    }

    if (AuxRetry::ack != bus.read(address, &ouiBuffer[0], sizeof ouiBuffer))
    {
        *modelName = 0;
        ouiId = 0;
        chipRevision = 0;
        return false;
    }
    //  The first 3 bytes are IEEE_OUI. 2 hex digits per register.
    ouiId = ouiBuffer[2] | (ouiBuffer[1] << 8) | (ouiBuffer[0] << 16);

    // Next 6 bytes are Device Identification String, copy as much as we can (limited buffer case).
    unsigned int i;
    for (i = 0; i < modelNameBufferSize; i++)
        modelName[i] = ouiBuffer[3+i];

    chipRevision = ouiBuffer[9];

    return true;
}

bool DPCDHALImpl::getOuiSink
(
    unsigned &ouiId,
    unsigned char * modelName,
    size_t modelNameBufferSize,
    NvU8 & chipRevision
)
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
        modelName[i] = (unsigned char)ouiBuffer[3+i];

    chipRevision = ouiBuffer[9];

    return true;
}

// DPCD offset 30h
bool DPCDHALImpl::getGUID(DisplayPort::GUID & guid)
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

AuxRetry::status DPCDHALImpl::setGUID(DisplayPort::GUID & guid)
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

void DPCDHALImpl::parsePortDescriptors()
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
        DP_PRINTF(DP_ERROR, "DPHAL> Unable to read detailed caps!");
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

                    NvU8    pConColorConvCaps   = basicCaps[infoByte0+3];
                    caps.pconCaps.bConv444To420Supported = FLD_TEST_DRF(_DPCD, _DETAILED_CAP, _CONV_YCBCR444_TO_YCBCR420_SUPPORTED, _YES, pConColorConvCaps);
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

void DPCDHALImpl::populateFakeDpcd()
{
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

// DPCD override routine: Max link rate override.
void DPCDHALImpl::overrideMaxLinkRate(NvU32 overrideMaxLinkRate)
{
    if (overrideMaxLinkRate)
    {
        caps.maxLinkRate = overrideMaxLinkRate;
    }
}

void DPCDHALImpl::notifyHPD(bool status, bool bSkipDPCDRead)
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

void DPCDHALImpl::setPostLtAdjustRequestGranted(bool bGrantPostLtRequest)
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
        DP_PRINTF(DP_ERROR, "DPCONN> Failed to set POST_LT_ADJ_REQ_GRANTED bit.");
    }
}

// DPCD offset 204
bool DPCDHALImpl::getIsPostLtAdjRequestInProgress()
{
    NvU8 buffer;

    if (AuxRetry::ack != bus.read(NV_DPCD_LANE_ALIGN_STATUS_UPDATED, &buffer, 1))
    {
        DP_PRINTF(DP_ERROR, "DPCONN> Post Link Training : Failed to read POST_LT_ADJ_REQ_IN_PROGRESS");
        return false;
    }

    return FLD_TEST_DRF(_DPCD, _LANE_ALIGN_STATUS_UPDATED,
                        _POST_LT_ADJ_REQ_IN_PROGRESS, _YES, buffer);
}

TrainingPatternSelectType DPCDHALImpl::getTrainingPatternSelect()
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

bool
DPCDHALImpl::setTrainingMultiLaneSet
(
    NvU8 numLanes,
    NvU8 *voltSwingSet,
    NvU8 *preEmphasisSet
)
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

AuxRetry::status DPCDHALImpl::setIgnoreMSATimingParamters(bool msaTimingParamIgnoreEn)
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

AuxRetry::status DPCDHALImpl::setLinkQualPatternSet
(
    LinkQualityPatternType linkQualPattern,
    unsigned laneCount
)
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

AuxRetry::status DPCDHALImpl::setLinkQualLaneSet(unsigned lane, LinkQualityPatternType linkQualPattern)
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

AuxRetry::status DPCDHALImpl::setMessagingEnable(bool _uprequestEnable, bool _upstreamIsSource)
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
        DP_PRINTF(DP_ERROR, "DPHAL> ERROR! Unable to read 00111h MSTM_CTRL.");
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

AuxRetry::status DPCDHALImpl::setMultistreamLink(bool enable)
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

AuxRetry::status DPCDHALImpl::setMultistreamHotplugMode(MultistreamHotplugMode notifyType)
{
    NvU8 deviceCtrl = 0;

    if (caps.revisionMajor <= 0)
        DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

    //  notifytype == HPD_LONG_PULSE, adapter 0
    if (notifyType == IRQ_HPD)
        deviceCtrl = FLD_SET_DRF(_DPCD, _BRANCH_DEV_CTRL, _HOTPLUG_EVENT_TYPE, _IRQ_HPD, deviceCtrl);

    return bus.write(NV_DPCD_BRANCH_DEV_CTRL, &deviceCtrl, sizeof deviceCtrl);
}

bool DPCDHALImpl::parseTestRequestTraining(NvU8 * buffer /* 0x18-0x28 valid */)
{
    if (buffer[1] == 0x6)
        interrupts.testTraining.testRequestLinkRate = dp2LinkRate_1_62Gbps;
    else if (buffer[1] == 0xa)
        interrupts.testTraining.testRequestLinkRate = dp2LinkRate_2_70Gbps;
    else if (buffer[1] == 0x14)
        interrupts.testTraining.testRequestLinkRate = dp2LinkRate_5_40Gbps;
    else if (buffer[1] == 0x1E)
        interrupts.testTraining.testRequestLinkRate = dp2LinkRate_8_10Gbps;
    else
    {
        DP_ASSERT(0 && "Unknown max link rate.  Assuming RBR");
        interrupts.testTraining.testRequestLinkRate = dp2LinkRate_1_62Gbps;
    }

    interrupts.testTraining.testRequestLaneCount  = buffer[(0x220 - 0x218)] & 0xf;

    return true;
}

void DPCDHALImpl::parseAutomatedTestRequest(bool testRequestPending)
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
        DP_PRINTF(DP_ERROR, "DPHAL> ERROR! Automated test request found. Unable to read 0x218 register.");
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

bool DPCDHALImpl::parseTestRequestPhy()
{
    NvU8 buffer = 0;
    NvU8 bits = 0;
    if (AuxRetry::ack != bus.read(NV_DPCD_PHY_TEST_PATTERN, &buffer, 1))
    {
        DP_PRINTF(DP_ERROR, "DPHAL> ERROR! Test pattern request found but unable to read NV_DPCD_PHY_TEST_PATTERN register.");
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
            DP_PRINTF(DP_ERROR, "DPHAL> ERROR! Request for 80 bit custom pattern. Can't read from 250h.");
            return false;
        }

        for (unsigned i = 0; i < NV_DPCD_TEST_80BIT_CUSTOM_PATTERN__SIZE; i++)
        {
            interrupts.eightyBitCustomPat[i] = buffer[i];
        }
    }

    return true;
}

void DPCDHALImpl::readPanelReplayError()
{
    NvU8 config = 0U;
    bool bRetVal = (AuxRetry::ack == bus.read(NV_DPCD20_PANEL_REPLAY_ERROR_STATUS,
                                              &config, sizeof(config)));

    if (bRetVal)
    {
        if (FLD_TEST_DRF(_DPCD20_PANEL_REPLAY, _ERROR_STATUS,
                _ACTIVE_FRAME_CRC_ERROR, _YES, config))
        {
            DP_PRINTF(DP_ERROR, "DPHAL> ERROR! Active Frame CRC Error set in PanelReplay status register");
        }
        if (FLD_TEST_DRF(_DPCD20_PANEL_REPLAY, _ERROR_STATUS,
                _RFB_STORAGE_ERROR, _YES, config))
        {
            DP_PRINTF(DP_ERROR, "DPHAL> ERROR! RFB Storage Error set in PanelReplay status register");
        }
        if (FLD_TEST_DRF(_DPCD20_PANEL_REPLAY, _ERROR_STATUS,
                _VSC_SDP_UNCORRECTABLE_ERROR, _YES, config))
        {
            DP_PRINTF(DP_ERROR, "DPHAL> ERROR! VSC SDP Uncorrectable Error set in PanelReplay status register");
        }
    }
    else
    {
        DP_PRINTF(DP_ERROR, "DPHAL> readPanelReplayError: Failed to read PanelReplay error status");
    }
}

bool DPCDHALImpl::isLinkStatusValid(unsigned lanes)
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

void DPCDHALImpl::refreshLinkStatus()
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

void DPCDHALImpl::parseAndReadInterruptsESI()
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

    interrupts.prErrorStatus               = FLD_TEST_DRF(_DPCD, _DEVICE_SERVICE_IRQ_VECTOR_ESI1,
                                                          _PANEL_REPLAY_ERROR_STATUS, _YES, buffer[4]);

    interrupts.rxCapChanged                = FLD_TEST_DRF(_DPCD, _LINK_SERVICE_IRQ_VECTOR_ESI0,
                                                          _RX_CAP_CHANGED, _YES, buffer[5]);
    interrupts.linkStatusChanged           = FLD_TEST_DRF(_DPCD, _LINK_SERVICE_IRQ_VECTOR_ESI0,
                                                          _LINK_STATUS_CHANGED, _YES, buffer[5]);
    interrupts.streamStatusChanged         = FLD_TEST_DRF(_DPCD, _LINK_SERVICE_IRQ_VECTOR_ESI0,
                                                          _STREAM_STATUS_CHANGED, _YES, buffer[5]);
    interrupts.hdmiLinkStatusChanged       = FLD_TEST_DRF(_DPCD, _LINK_SERVICE_IRQ_VECTOR_ESI0,
                                                          _HDMI_LINK_STATUS_CHANGED, _YES, buffer[5]);
    interrupts.dpTunnelingIrq              = FLD_TEST_DRF(_DPCD20, _LINK_SERVICE_IRQ_VECTOR_ESI0,
                                                          _DP_TUNNELING_IRQ, _YES, buffer[5]);

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

        DP_PRINTF(DP_WARNING, "DPHAL> RX Capabilities have changed!");
        parseAndReadCaps();
        this->clearInterruptCapabilitiesChanged();
    }

    if (interrupts.dpTunnelingIrq && hasDpTunnelBwAllocationCapabilityChanged())
    {
        // Re read caps and turn on BW allocation if needed
        parseAndReadCaps();
    }

    if (interrupts.hdmiLinkStatusChanged)
    {
        this->clearHdmiLinkStatusChanged();
    }

    if (interrupts.prErrorStatus)
    {
        this->clearPanelReplayError();
    }

    parseAutomatedTestRequest(automatedTestRequest);
}

void DPCDHALImpl::readLTTPRLinkStatus(NvS32 rxIndex, NvU8 *buffer)
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

void DPCDHALImpl::resetIntrLaneStatus()
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

void DPCDHALImpl::fetchLinkStatusESI()
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

void DPCDHALImpl::fetchLinkStatusLegacy()
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

bool DPCDHALImpl::readTraining(NvU8* voltageSwingLane,  NvU8* preemphasisLane,
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

bool DPCDHALImpl::isLaneSettingsChanged(NvU8* oldVoltageSwingLane,
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

bool DPCDHALImpl::setPowerState(PowerState newState)
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
                    DP_PRINTF(DP_ERROR, "DisplayPort: Failed to grant extended sleep wake timeout before D3");
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

        DP_PRINTF(DP_ERROR, "DisplayPort: Failed to bring panel back to wake state");
    }
    else
    {
        //  DP 1.0 devices cannot be put to sleep
        if (newState == PowerStateD0)
            return true;
    }

    return false;
}

void DPCDHALImpl::parseAndReadInterruptsLegacy()
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

void DPCDHALImpl::clearInterruptContentProtection()
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

void DPCDHALImpl::clearInterruptMCCS()
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

void DPCDHALImpl::clearInterruptDownReplyReady()
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

void DPCDHALImpl::clearInterruptUpRequestReady()
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

// DPCD offset 0x68000
bool DPCDHALImpl::getBKSV(NvU8 *bKSV)
{
    if (caps.revisionMajor <= 0)
        DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

    if (AuxRetry::ack == bus.read(NV_DPCD_HDCP_BKSV_OFFSET, &bKSV[0], HDCP_KSV_SIZE))
    {
        DP_PRINTF(DP_NOTICE, "Found HDCP Bksv= %02x %02x %02x %02x %02x",
                  bKSV[4], bKSV[3], bKSV[2], bKSV[1], bKSV[0]);
        return true;
    }
    return false;
}

// DPCD offset 0x68028
bool DPCDHALImpl::getBCaps(BCaps &bCaps, NvU8 * rawByte)
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

// DPCD offset 0x6921D
bool DPCDHALImpl::getHdcp22BCaps(BCaps &bCaps, NvU8 *rawByte)
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

// DPCD offset 0x6802A
bool DPCDHALImpl::getBinfo(BInfo &bInfo)
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
bool DPCDHALImpl::getRxStatus(const HDCPState &hdcpState, NvU8 *data)
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

AuxRetry::status DPCDHALImpl::setTestResponse(bool ack, bool edidChecksumWrite)
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

PowerState DPCDHALImpl::getPowerState()
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

void DPCDHALImpl::payloadTableClearACT()
{
    NvU8 byte = NV_DPCD_PAYLOAD_TABLE_UPDATE_STATUS_UPDATED_YES;
    bus.write(NV_DPCD_PAYLOAD_TABLE_UPDATE_STATUS, &byte, sizeof byte);
}

bool DPCDHALImpl::payloadWaitForACTReceived()
{
    NvU8 byte = 0;
    int retries = 0;

    while (true)
    {
        if (++retries > 40)
        {
            DP_PRINTF(DP_ERROR, "DPHAL> ACT Not received by sink device!");
            return false;
        }

        if (AuxRetry::ack == bus.read(NV_DPCD_PAYLOAD_TABLE_UPDATE_STATUS, &byte, sizeof byte))
        {
            if (FLD_TEST_DRF(_DPCD, _PAYLOAD_TABLE_UPDATE_STATUS, _ACT_HANDLED, _YES, byte))
            {
                DP_PRINTF(DP_NOTICE, "DPHAL> ACT Received");
                return true;
            }
        }
    }
}

bool DPCDHALImpl::payloadAllocate(unsigned streamId, unsigned begin, unsigned count)
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
                DP_PRINTF(DP_ERROR, "DPHAL> Read NV_DPCD_PAYLOAD_TABLE_UPDATE_STATUS failed.");
            }

            timer->sleep(1);
        } while (++retries < PAYLOADIDTABLE_UPDATED_CHECK_RETRIES);
    }
    else
    {
        DP_PRINTF(DP_ERROR, "DPHAL> Send NV_DPCD_PAYLOAD_ALLOC_SET failed.");
    }

    DP_PRINTF(DP_NOTICE, "DPHAL> Requesting allocation Stream:%d | First Slot:%d | Count:%d  (%s)",
              streamId, begin, count, bResult ? "OK" : "FAILED");
    return bResult;
}

void DPCDHALImpl::setGpuDPSupportedVersions(NvU32 _gpuDPSupportedVersions)
{
    bool bSupportDp1_2 = FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_DP_VERSIONS_SUPPORTED, _DP1_2,
                                      _YES, _gpuDPSupportedVersions);
    bool bSupportDp1_4 = FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_DP_VERSIONS_SUPPORTED, _DP1_4,
                                      _YES, _gpuDPSupportedVersions);

    if (bSupportDp1_4)
    {
        DP_ASSERT(bSupportDp1_2 && "GPU supports DP1.4 should also support DP1.2!");
    }

    gpuDPSupportedVersions = _gpuDPSupportedVersions;
}

void DPCDHALImpl::applyRegkeyOverrides(const DP_REGKEY_DATABASE& dpRegkeyDatabase)
{
    DP_ASSERT(dpRegkeyDatabase.bInitialized &&
            "All regkeys are invalid because dpRegkeyDatabase is not initialized!");
    overrideDpcdRev          = dpRegkeyDatabase.dpcdRevOveride;
    bBypassILREdpRevCheck    = dpRegkeyDatabase.bBypassEDPRevCheck;
}

bool DPCDHALImpl::clearPendingMsg()
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
            DP_PRINTF(DP_ERROR, "DPCONN> %s(): No Pending Message or "
                      "Failed to clear pending message: irqVector/data = 0x%08x/0x%08x",
                      __FUNCTION__, irqVector, data);
            return false;
        }

        return true;
    }
    else
    {
        DP_PRINTF(DP_ERROR, "DPCONN> Clear Pending MSG: Failed to read ESI0");
    }

    return false;
}

bool DPCDHALImpl::isMessagingEnabled()
{
    NvU8 mstmCtrl;

    if ((AuxRetry::ack == bus.read(NV_DPCD_MSTM_CTRL, &mstmCtrl, 1)) &&
        (FLD_TEST_DRF(_DPCD, _MSTM_CTRL, _EN, _YES, mstmCtrl)))
    {
        return true;
    }
    return false;
}

NvU16 * DPCDHALImpl::getLinkRateTable()
{
    if (!bIndexedLinkrateCapable)
    {
        DP_PRINTF(DP_ERROR, "DPCONN> link rate table is invalid");
    }
    return &caps.linkRateTable[0];
}

bool DPCDHALImpl::getRawLinkRateTable(NvU8 *buffer)
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

bool DPCDHALImpl::setSourceControlMode(bool bEnableSourceControlMode, bool bEnableFRLMode)
{
    NvU8    data = 0;

    if (bEnableSourceControlMode)
    {
        data = FLD_SET_DRF(_DPCD14, _PCON_FRL_LINK_CONFIG_1, _SRC_CONTROL_MODE, _ENABLE, data);
        if (bEnableFRLMode)
        {
            data = FLD_SET_DRF(_DPCD14, _PCON_FRL_LINK_CONFIG_1, _LINK_FRL_MODE, _ENABLE, data);
            data = FLD_SET_DRF(_DPCD14, _PCON_FRL_LINK_CONFIG_1, _IRQ_LINK_FRL_MODE, _ENABLE, data);
        }
        else
        {
            data = FLD_SET_DRF(_DPCD14, _PCON_FRL_LINK_CONFIG_1, _LINK_FRL_MODE, _DISABLE, data);
            data = FLD_SET_DRF(_DPCD14, _PCON_FRL_LINK_CONFIG_1, _IRQ_LINK_FRL_MODE, _DISABLE, data);
        }
    }
    else
    {
        data = FLD_SET_DRF(_DPCD14, _PCON_FRL_LINK_CONFIG_1, _SRC_CONTROL_MODE, _DISABLE, data);
        data = FLD_SET_DRF(_DPCD14, _PCON_FRL_LINK_CONFIG_1, _LINK_FRL_MODE, _DISABLE, data);
        data = FLD_SET_DRF(_DPCD14, _PCON_FRL_LINK_CONFIG_1, _IRQ_LINK_FRL_MODE, _DISABLE, data);
    }

    if (AuxRetry::ack != bus.write(NV_DPCD14_PCON_FRL_LINK_CONFIG_1, &data, sizeof(data)))
    {
        return false;
    }
    return true;
}

bool DPCDHALImpl::checkPCONFrlReady(bool *bFrlReady)
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

    if (AuxRetry::ack != bus.read(NV_DPCD14_PCON_HDMI_TX_LINK_STATUS, &data, sizeof(data)))
    {
        return false;
    }

    if (FLD_TEST_DRF(_DPCD14, _PCON_HDMI_TX_LINK_STATUS, _LINK_READY, _YES, data))
    {
        *bFrlReady = true;
    }
    return true;
}

bool DPCDHALImpl::setupPCONFrlLinkAssessment(NvU32   linkBwMask,
                                             bool    bEnableExtendLTMode,
                                             bool    bEnableConcurrentMode)
{
    NvU8        data = 0;

    // +1 to convert PCONHdmiLinkBw enum to DPCD FRL BW cap definition
    NvU32       requestedMaxBw  = (NvU32)(getMaxFrlBwFromMask(linkBwMask)) + 1;
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
        data = FLD_SET_DRF(_DPCD14, _PCON_FRL_LINK_CONFIG_2, _FRL_LT_CONTROL,
                        _EXTENDED, data);
    }
    else
    {
        // Set FRL_LT_CONTROL to Normal mode, so PCON stops when first FRL LT succeed.
        data = FLD_SET_DRF(_DPCD14, _PCON_FRL_LINK_CONFIG_2, _FRL_LT_CONTROL,
                        _NORMAL, data);
    }

    if (AuxRetry::ack != bus.write(NV_DPCD14_PCON_FRL_LINK_CONFIG_2, &data, sizeof(data)))
    {
        return false;
    }

    if (AuxRetry::ack != bus.read(NV_DPCD14_PCON_FRL_LINK_CONFIG_1, &data, sizeof(data)))
    {
        return false;
    }

    if (bEnableConcurrentMode && caps.pconCaps.bConcurrentLTSupported)
    {
        // Client selects concurrent.
        data = FLD_SET_DRF(_DPCD14, _PCON_FRL_LINK_CONFIG_1, _CONCURRENT_LT_MODE,
                        _ENABLE, data);
    }
    else
    {
        //
        // Don't do concurrent LT for now.
        //
        data = FLD_SET_DRF(_DPCD14, _PCON_FRL_LINK_CONFIG_1, _CONCURRENT_LT_MODE,
                        _DISABLE, data);
    }
    data = FLD_SET_DRF(_DPCD14, _PCON_FRL_LINK_CONFIG_1, _HDMI_LINK,
                    _ENABLE, data);
    data = FLD_SET_DRF_NUM(_DPCD14, _PCON_FRL_LINK_CONFIG_1, _MAX_LINK_BW,
                        targetBw, data);

    if (AuxRetry::ack != bus.write(NV_DPCD14_PCON_FRL_LINK_CONFIG_1, &data, sizeof(data)))
    {
        return false;
    }

    return true;
}

bool DPCDHALImpl::checkPCONFrlLinkStatus(NvU32 *frlRateMask)
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
    if (AuxRetry::ack != bus.read(NV_DPCD14_PCON_HDMI_TX_LINK_STATUS, &data, sizeof(data)))
    {
        return false;
    }

    if (FLD_TEST_DRF(_DPCD14, _PCON_HDMI_TX_LINK_STATUS, _LINK_ACTIVE, _YES, data))
    {
        if (AuxRetry::ack == bus.read(NV_DPCD14_PCON_HDMI_LINK_CONFIG_STATUS, &data, sizeof(data)))
        {
            *frlRateMask = DRF_VAL(_DPCD14, _PCON_HDMI_LINK_CONFIG_STATUS, _LT_RESULT, data);
        }

    }

    return true;
}

bool DPCDHALImpl::queryHdmiLinkStatus(bool *bLinkActive, bool *bLinkReady)
{
    NvU8        data = 0;

    if (bLinkActive == NULL && bLinkReady == NULL)
        return false;

    if (AuxRetry::ack != bus.read(NV_DPCD14_PCON_HDMI_TX_LINK_STATUS, &data, sizeof(data)))
    {
        return false;
    }
    if (bLinkReady != NULL)
    {
        *bLinkReady = (FLD_TEST_DRF(_DPCD14, _PCON_HDMI_TX_LINK_STATUS,
                                    _LINK_READY, _YES, data));
    }
    if (bLinkActive != NULL)
    {
        *bLinkActive = (FLD_TEST_DRF(_DPCD14, _PCON_HDMI_TX_LINK_STATUS,
                                    _LINK_ACTIVE, _YES, data));
    }
    return true;
}

NvU32 DPCDHALImpl::restorePCONFrlLink(NvU32   linkBwMask,
                                      bool    bEnableExtendLTMode,
                                      bool    bEnableConcurrentMode)
{
    // Restore HDMI Link.
    // 1. Clear HDMI link enable bit (305A bit 7)
    NvU8    data = 0;
    NvU32   loopCount;
    NvU32   frlRate;
    if (AuxRetry::ack != bus.read(NV_DPCD14_PCON_FRL_LINK_CONFIG_1, &data, sizeof(data)))
    {
        return false;
    }
    data = FLD_SET_DRF(_DPCD14, _PCON_FRL_LINK_CONFIG_1, _HDMI_LINK, _DISABLE, data);
    if (AuxRetry::ack != bus.write(NV_DPCD14_PCON_FRL_LINK_CONFIG_1, &data, sizeof(data)))
    {
        return false;
    }
    // 2. Set FRL or TMDS (Optional if not changed) (305A bit 5)
    // 3. Read FRL Ready Bit (303B bit 1)

    Timeout timeout(timer, 500 /* 500ms */);
    data = 0;
    do
    {
        if (AuxRetry::ack != bus.read(NV_DPCD14_PCON_HDMI_TX_LINK_STATUS,
                                    &data, sizeof(data)))
            continue;
        if (FLD_TEST_DRF(_DPCD14, _PCON_HDMI_TX_LINK_STATUS, _LINK_READY, _YES, data))
            break;
    } while (timeout.valid());

    if (FLD_TEST_DRF(_DPCD14, _PCON_HDMI_TX_LINK_STATUS, _LINK_READY, _NO, data))
    {
        return false;
    }

    // 4. Configure FRL Link (Optional if not changed)
    // 5. Set HDMI Enable Bit.
    data = 0;

    if (AuxRetry::ack != bus.read(NV_DPCD14_PCON_FRL_LINK_CONFIG_1, &data, sizeof(data)))
    {
        return false;
    }
    data = FLD_SET_DRF(_DPCD14, _PCON_FRL_LINK_CONFIG_1, _HDMI_LINK, _ENABLE, data);
    if (AuxRetry::ack != bus.write(NV_DPCD14_PCON_FRL_LINK_CONFIG_1, &data, sizeof(data)))
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

bool DPCDHALImpl::updatePsrConfiguration(vesaPsrConfig psrcfg)
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

bool DPCDHALImpl::readPsrConfiguration(vesaPsrConfig *psrcfg)
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

bool DPCDHALImpl::readPsrState(vesaPsrState *psrState)
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

bool DPCDHALImpl::readPsrDebugInfo(vesaPsrDebugStatus *psrDbgState)
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

bool DPCDHALImpl::writePsrErrorStatus(vesaPsrErrorStatus psrErr)
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

bool DPCDHALImpl::readPsrErrorStatus(vesaPsrErrorStatus *psrErr)
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

bool DPCDHALImpl::writePsrEvtIndicator(vesaPsrEventIndicator psrEvt)
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

bool DPCDHALImpl::readPsrEvtIndicator(vesaPsrEventIndicator *psrEvt)
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

bool DPCDHALImpl::readPrSinkDebugInfo(panelReplaySinkDebugInfo *prDbgInfo)
{
    NvU8 config = 0U;
    bool bRetVal = (AuxRetry::ack ==
            bus.read(NV_DPCD20_PANEL_REPLAY_ERROR_STATUS,
                &config, sizeof(config)));

    if (bRetVal)
    {
        prDbgInfo->activeFrameCrcError =
            FLD_TEST_DRF(_DPCD20_PANEL_REPLAY, _ERROR_STATUS,
                    _ACTIVE_FRAME_CRC_ERROR, _YES, config);
        prDbgInfo->rfbStorageError =
            FLD_TEST_DRF(_DPCD20_PANEL_REPLAY, _ERROR_STATUS,
                    _RFB_STORAGE_ERROR, _YES, config);
        prDbgInfo->vscSdpUncorrectableError =
            FLD_TEST_DRF(_DPCD20_PANEL_REPLAY, _ERROR_STATUS,
                _VSC_SDP_UNCORRECTABLE_ERROR, _YES, config);
        prDbgInfo->adaptiveSyncSdpMissing =
            FLD_TEST_DRF(_DPCD20_PANEL_REPLAY, _ERROR_STATUS,
                _ADAPTIVE_SYNC_SDP_MISSING, _YES, config);
    }
    else
    {
        DP_PRINTF(DP_ERROR, "DPHAL> readPrSinkDebugInfo: Failed to read PanelReplay error status");
        return bRetVal;
    }

    config = 0U;
    bRetVal = (AuxRetry::ack ==
        bus.read(NV_DPCD20_PANEL_REPLAY_AND_FRAME_LOCK_STATUS,
            &config, sizeof(config)));
    if (bRetVal)
    {
        prDbgInfo->sinkPrStatus = DRF_VAL(_DPCD20,
            _PANEL_REPLAY_AND_FRAME_LOCK_STATUS, _PR_STATUS, config);

        prDbgInfo->sinkFramelocked = DRF_VAL(_DPCD20,
            _PANEL_REPLAY_AND_FRAME_LOCK_STATUS, _SINK_FRAME_LOCKED, config);

        prDbgInfo->sinkFrameLockedValid =
            FLD_TEST_DRF(_DPCD20_PANEL_REPLAY_AND_FRAME_LOCK_STATUS,
                _SINK_FRAME_LOCKED, _VALID, _YES, config);
    }
    else
    {
        DP_PRINTF(DP_ERROR, "DPHAL> readPanelReplayError: Failed to read PanelReplay frame lock status");
        return bRetVal;
    }

    config = 0U;
    bRetVal = AuxRetry::ack ==
        bus.read(NV_DPCD20_PANEL_REPLAY_DEBUG_LAST_VSC_SDP_CARRYING_PR_INFO,
            &config, sizeof(config));
    if (bRetVal)
    {
        prDbgInfo->currentPrState =
            FLD_TEST_DRF(_DPCD20_PANEL_REPLAY_DEBUG_LAST,
                _VSC_SDP_CARRYING_PR_INFO, _STATE, _ACTIVE, config);
        prDbgInfo->crcValid =
            FLD_TEST_DRF(_DPCD20_PANEL_REPLAY_DEBUG_LAST,
                _VSC_SDP_CARRYING_PR_INFO, _CRC_VALID, _YES, config);
        prDbgInfo->suCoordinatesValid  =
            FLD_TEST_DRF(_DPCD20_PANEL_REPLAY_DEBUG_LAST,
                _VSC_SDP_CARRYING_PR_INFO, _SU_COORDINATE_VALID,
                    _YES, config);
    }
    else
    {
        DP_PRINTF(DP_ERROR, "DPHAL> readPanelReplayError: Failed to read PanelReplay VSC SDP status");
        return bRetVal;
    }
    return bRetVal;
}

bool DPCDHALImpl::getDpTunnelGranularityMultiplier(NvU8 &granularityMultiplier)
{
    NvU8 granularity = 0;
    if (AuxRetry::ack !=
        bus.read(NV_DPCD20_DP_TUNNEL_BW_GRANULARITY, &granularity, sizeof(granularity)))
    {
        DP_PRINTF(DP_ERROR, "Failed to read DP Tunnel granularity");
        return false;
    }

    switch (DRF_VAL(_DPCD20, _DP_TUNNEL_BW_GRANULARITY, _VAL, granularity))
    {
    case NV_DPCD20_DP_TUNNEL_BW_GRANULARITY_VAL_0_25_GBPS:
        granularityMultiplier = 4;
        break;
    case NV_DPCD20_DP_TUNNEL_BW_GRANULARITY_VAL_0_50_GBPS:
        granularityMultiplier = 2;
        break;
    case NV_DPCD20_DP_TUNNEL_BW_GRANULARITY_VAL_1_00_GBPS:
        granularityMultiplier = 1;
        break;
    default:
        DP_PRINTF(DP_WARNING, "Unknown DP Tunnel granularity read from sink. Assuming 1 Gbps");
        granularityMultiplier = 1;
        break;
    }

    return true;
}

TriState DPCDHALImpl::getDpTunnelBwRequestStatus()
{
    NvU8 bwRequestStatus = 0;
    TriState status = Indeterminate;
    if (AuxRetry::ack !=
        bus.read(NV_DPCD20_DP_TUNNELING_STATUS, &bwRequestStatus, sizeof(bwRequestStatus)))
    {
        DP_PRINTF(DP_ERROR, "Failed to read DP Tunneling status");
        return status;
    }

    if (FLD_TEST_DRF(_DPCD20, _DP_TUNNELING_BW_REQUEST, _FAILED, _YES, bwRequestStatus))
    {
        status = False;
    }
    else if (FLD_TEST_DRF(_DPCD20, _DP_TUNNELING_BW_REQUEST, _SUCCEEDED, _YES, bwRequestStatus))
    {
        status = True;
    }

    return status;
}

bool DPCDHALImpl::setDpTunnelBwAllocation(bool bEnable)
{
    DP_PRINTF(DP_INFO, "requested: %d", bEnable);
    NvU8 bwAllocationControl = 0;
    if(AuxRetry::ack != bus.read(NV_DPCD20_DPTX_BW_ALLOCATION_MODE_CONTROL,
                                 &bwAllocationControl, sizeof(bwAllocationControl)))
    {
        return false;
    }

    if (bEnable)
    {
        bwAllocationControl = FLD_SET_DRF(_DPCD20, _DPTX, _UNMASK_BW_ALLOCATION_IRQ, _YES, bwAllocationControl);
        bwAllocationControl = FLD_SET_DRF(_DPCD20, _DPTX, _DISPLAY_DRIVER_BW_ALLOCATION_MODE_ENABLE,
                                          _YES, bwAllocationControl);
    }
    else
    {
        bwAllocationControl = FLD_SET_DRF(_DPCD20, _DPTX, _UNMASK_BW_ALLOCATION_IRQ, _NO, bwAllocationControl);
        bwAllocationControl = FLD_SET_DRF(_DPCD20, _DPTX, _DISPLAY_DRIVER_BW_ALLOCATION_MODE_ENABLE,
                                          _NO, bwAllocationControl);
    }

    if (AuxRetry::ack !=
        bus.write(NV_DPCD20_DPTX_BW_ALLOCATION_MODE_CONTROL, &bwAllocationControl, sizeof(bwAllocationControl)))
    {
        DP_PRINTF(DP_ERROR, "Failed to write bw allocation control: %d", bwAllocationControl);
        return false;
    }

    bIsDpTunnelBwAllocationEnabled = bEnable;

    return true;
}

bool DPCDHALImpl::getDpTunnelEstimatedBw(NvU8 &estimatedBw)
{
    if (AuxRetry::ack !=
        bus.read(NV_DPCD20_DP_TUNNEL_ESTIMATED_BW, &estimatedBw, sizeof(estimatedBw)))
    {
        DP_PRINTF(DP_ERROR, "Failed to read DP Tunnel estimated BW");
        return false;
    }

    return true;
}

bool DPCDHALImpl::hasDpTunnelEstimatedBwChanged()
{
    NvU8 byte = 0;
    if (AuxRetry::ack !=
        bus.read(NV_DPCD20_DP_TUNNELING_STATUS, &byte, sizeof(byte)))
    {
        DP_PRINTF(DP_ERROR, "Failed to read DP Tunneling status");
        return false;
    }

    return FLD_TEST_DRF(_DPCD20, _DP_TUNNELING, _ESTIMATED_BW_CHANGED, _YES, byte);
}

bool DPCDHALImpl::hasDpTunnelBwAllocationCapabilityChanged()
{
    NvU8 byte = 0;
    if (AuxRetry::ack !=
        bus.read(NV_DPCD20_DP_TUNNELING_STATUS, &byte, sizeof(byte)))
    {
        DP_PRINTF(DP_ERROR, "Failed to read DP Tunneling status");
        return false;
    }

    return FLD_TEST_DRF(_DPCD20, _DP_TUNNELING,
                        _BW_ALLOCATION_CAPABILITY_CHANGED, _YES, byte);
}

bool DPCDHALImpl::writeDpTunnelRequestedBw(NvU8 requestedBw)
{
    if (AuxRetry::ack !=
        bus.write(NV_DPCD20_DP_TUNNEL_REQUESTED_BW, &requestedBw, sizeof(requestedBw)))
    {
        DP_PRINTF(DP_ERROR, "Failed to write requested BW");
        return false;
    }

    return true;
}

bool DPCDHALImpl::clearDpTunnelingBwRequestStatus()
{
    NvU8 readByte = 0;
    NvU8 writeByte = 0;
    if (AuxRetry::ack !=
        bus.read(NV_DPCD20_DP_TUNNELING_STATUS, &readByte, sizeof(readByte)))
    {
        DP_PRINTF(DP_ERROR, "Failed to read DP Tunneling status");
        return false;
    }

    if (FLD_TEST_DRF(_DPCD20, _DP_TUNNELING, _BW_REQUEST_FAILED, _YES, readByte))
    {
        writeByte = FLD_SET_DRF(_DPCD20, _DP_TUNNELING, _BW_REQUEST_FAILED, _YES, writeByte);
    }

    if (FLD_TEST_DRF(_DPCD20, _DP_TUNNELING, _BW_REQUEST_SUCCEEDED, _YES, readByte))
    {
        writeByte = FLD_SET_DRF(_DPCD20, _DP_TUNNELING, _BW_REQUEST_SUCCEEDED, _YES, writeByte);
    }

    if (writeByte > 0)
    {
        if (AuxRetry::ack !=
            bus.write(NV_DPCD20_DP_TUNNELING_STATUS, &writeByte, sizeof(writeByte)))
        {
            DP_PRINTF(DP_ERROR, "Failed to write DP_TUNNELING_STATUS");
            return false;
        }

    }
    return true;
}

bool DPCDHALImpl::clearDpTunnelingEstimatedBwStatus()
{
    NvU8 byte = 0;
    byte = FLD_SET_DRF(_DPCD20, _DP_TUNNELING, _ESTIMATED_BW_CHANGED, _YES, byte);
    if (AuxRetry::ack !=
        bus.write(NV_DPCD20_DP_TUNNELING_STATUS, &byte, sizeof(byte)))
    {
        DP_PRINTF(DP_ERROR, "Failed to write clear estimated BW status");
        return false;
    }

    return true;
}

bool DPCDHALImpl::clearDpTunnelingBwAllocationCapStatus()
{
    NvU8 byte = 0;
    byte = FLD_SET_DRF(_DPCD20, _DP_TUNNELING, _BW_ALLOCATION_CAPABILITY_CHANGED, _YES, byte);

    if (AuxRetry::ack !=
        bus.write(NV_DPCD20_DP_TUNNELING_STATUS, &byte, sizeof(byte)))
    {
        DP_PRINTF(DP_ERROR, "Failed to write clear bw allocation capability changed status");
        return false;
    }

    return true;
}

DPCDHAL * DisplayPort::MakeDPCDHAL(AuxBus *  bus, Timer * timer, MainLink * main)
{
    return new DPCDHALImpl(bus, timer);
}

