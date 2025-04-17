/*
 * SPDX-FileCopyrightText: Copyright (c) 2024-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
* Module: dp_configcaps2x.cpp                                               *
*    Abstraction for DP2x caps registers                                    *
*                                                                           *
\***************************************************************************/
#include "dp_internal.h"
#include "dp_auxbus.h"
#include "dp_auxdefs.h"
#include "displayport.h"
#include "dp_configcaps2x.h"

using namespace DisplayPort;

void DPCDHALImpl2x::parseAndSetCableId(NvU8 cableId)
{
    caps2x.rxCableCaps.bUHBR_10GSupported = true;

    caps2x.rxCableCaps.bUHBR_20GSupported =
        FLD_TEST_DRF(_DPCD20, _CABLE_ATTRIBUTES_UPDATED_BY_DPRX, _UHBR20_10_CAPABILITY, _10_AND_20_GBPS_SUPPORTED, cableId);

    caps2x.rxCableCaps.bUHBR_13_5GSupported =
        FLD_TEST_DRF(_DPCD20, _CABLE_ATTRIBUTES_UPDATED_BY_DPRX, _13_5_GBPS_SUPPORTED, _YES, cableId);

    switch (DRF_VAL(_DPCD20, _CABLE_ATTRIBUTES_UPDATED_BY_DPRX, _CABLE_TYPE, cableId))
    {
        case NV_DPCD20_CABLE_ATTRIBUTES_UPDATED_BY_DPRX_CABLE_TYPE_CABLE_TYPE_UNKNOWN:
            caps2x.rxCableCaps.cableType = CableTypeUnknown;
            break;
        case NV_DPCD20_CABLE_ATTRIBUTES_UPDATED_BY_DPRX_CABLE_TYPE_PASSIVE:
            caps2x.rxCableCaps.cableType = CableTypePassive;
            break;
        case NV_DPCD20_CABLE_ATTRIBUTES_UPDATED_BY_DPRX_CABLE_TYPE_LRD:
            caps2x.rxCableCaps.cableType = CableTypeLRD;
            break;
        case NV_DPCD20_CABLE_ATTRIBUTES_UPDATED_BY_DPRX_CABLE_TYPE_ACTIVE_RETIMER:
            caps2x.rxCableCaps.cableType = CableTypeActiveReTimer;
            break;
        default:
            DP_PRINTF(DP_ERROR, "Unknown cable type\n");
            break;
    }
}

void DPCDHALImpl2x::performCableIdHandshakeForTypeC()
{
    NvU8 txCableCaps = 0;
    NvU8 rxCableCaps = 0;

    if (AuxRetry::ack !=
        bus.read(NV_DPCD20_CABLE_ATTRIBUTES_UPDATED_BY_DPRX, &rxCableCaps, sizeof rxCableCaps))
    {
        DP_PRINTF(DP_WARNING, "Failed to read NV_DPCD20_CABLE_ATTRIBUTES_UPDATED_BY_DPRX for updated results");
    }
    else
    {
        parseAndSetCableId(rxCableCaps);
    }

    if (caps2x.txCableCaps.bIsSupported)
    {
        if (caps2x.txCableCaps.bUHBR_20GSupported)
        {
            txCableCaps = FLD_SET_DRF(_DPCD20, _CABLE_ATTRIBUTES_UPDATED_BY_DPTX, _UHBR20_10_CAPABILITY,
                                      _10_AND_20_GBPS_SUPPORTED, txCableCaps);
        }
        else
        {
            txCableCaps = FLD_SET_DRF(_DPCD20, _CABLE_ATTRIBUTES_UPDATED_BY_DPTX, _UHBR20_10_CAPABILITY,
                                      _10_0_GBPS_SUPPORTED, txCableCaps);
        }
        if (caps2x.txCableCaps.bUHBR_13_5GSupported)
        {
            txCableCaps = FLD_SET_DRF(_DPCD20, _CABLE_ATTRIBUTES_UPDATED_BY_DPTX, _13_5_GBPS_SUPPORTED,
                                      _YES, txCableCaps);
        }
        switch (caps2x.txCableCaps.cableType)
        {
            case CableTypePassive:
                txCableCaps = FLD_SET_DRF(_DPCD20, _CABLE_ATTRIBUTES_UPDATED_BY_DPTX, _CABLE_TYPE, _PASSIVE, txCableCaps);
                break;
            case CableTypeActiveReTimer:
                txCableCaps = FLD_SET_DRF(_DPCD20, _CABLE_ATTRIBUTES_UPDATED_BY_DPTX, _CABLE_TYPE, _ACTIVE_RETIMER, txCableCaps);
                break;
            case CableTypeLRD:
                txCableCaps = FLD_SET_DRF(_DPCD20, _CABLE_ATTRIBUTES_UPDATED_BY_DPTX, _CABLE_TYPE, _LRD, txCableCaps);
                break;
            case CableTypeOptical:
            case CableTypeUnknown:
            default:
                txCableCaps = FLD_SET_DRF(_DPCD20, _CABLE_ATTRIBUTES_UPDATED_BY_DPTX, _CABLE_TYPE, _CABLE_TYPE_UNKNOWN, txCableCaps);
                break;
        }
    }
    else if (bCableVconnSourceUnknown)
    {
        if (caps2x.bUHBR_20GSupported)
        {
            txCableCaps = FLD_SET_DRF(_DPCD20, _CABLE_ATTRIBUTES_UPDATED_BY_DPTX, _UHBR20_10_CAPABILITY,
                                      _10_AND_20_GBPS_SUPPORTED, txCableCaps);
        }
        else
        {
            txCableCaps = FLD_SET_DRF(_DPCD20, _CABLE_ATTRIBUTES_UPDATED_BY_DPTX, _UHBR20_10_CAPABILITY,
                                      _10_0_GBPS_SUPPORTED, txCableCaps);
        }
        if (caps2x.bUHBR_13_5GSupported)
        {
            txCableCaps = FLD_SET_DRF(_DPCD20, _CABLE_ATTRIBUTES_UPDATED_BY_DPTX, _13_5_GBPS_SUPPORTED,
                                      _YES, txCableCaps);
        }

        // Set cable type based on if any LTTPR is detected.
        if (!bLttprSupported || (this->caps.phyRepeaterCount == 0))
        {
            txCableCaps = FLD_SET_DRF(_DPCD20, _CABLE_ATTRIBUTES_UPDATED_BY_DPTX, _CABLE_TYPE,
                                      _PASSIVE, txCableCaps);
        }
        else
        {
            txCableCaps = FLD_SET_DRF(_DPCD20, _CABLE_ATTRIBUTES_UPDATED_BY_DPTX, _CABLE_TYPE,
                                      _ACTIVE_RETIMER, txCableCaps);
        }
    }

    if (caps2x.txCableCaps.bIsSupported)
    {
        NvU8 cableCaps = txCableCaps;

        if (caps2x.txCableCaps.bVconnSource)
        {
            // write the least common denominator of 0x2217 read value (rxCableCaps) and the eMarker content (txCableCaps)
            if (caps2x.txCableCaps.bUHBR_20GSupported && caps2x.rxCableCaps.bUHBR_20GSupported)
            {
                cableCaps = FLD_SET_DRF(_DPCD20, _CABLE_ATTRIBUTES_UPDATED_BY_DPTX, _UHBR20_10_CAPABILITY,
                                        _10_AND_20_GBPS_SUPPORTED, cableCaps);
            }
            else if (caps2x.txCableCaps.bUHBR_10GSupported && caps2x.rxCableCaps.bUHBR_10GSupported)
            {
                cableCaps = FLD_SET_DRF(_DPCD20, _CABLE_ATTRIBUTES_UPDATED_BY_DPTX, _UHBR20_10_CAPABILITY,
                                        _10_0_GBPS_SUPPORTED, cableCaps);
            }
            else
            {
                cableCaps = FLD_SET_DRF(_DPCD20, _CABLE_ATTRIBUTES_UPDATED_BY_DPTX, _UHBR20_10_CAPABILITY,
                                        _UHBR_NOT_CAPABLE, cableCaps);
            }

            if (caps2x.txCableCaps.bUHBR_13_5GSupported && caps2x.rxCableCaps.bUHBR_13_5GSupported)
            {
                cableCaps = FLD_SET_DRF(_DPCD20, _CABLE_ATTRIBUTES_UPDATED_BY_DPTX, _13_5_GBPS_SUPPORTED,
                                        _YES, cableCaps);
            }
            else
            {
                cableCaps = FLD_SET_DRF(_DPCD20, _CABLE_ATTRIBUTES_UPDATED_BY_DPTX, _13_5_GBPS_SUPPORTED,
                                        _NO, cableCaps);
            }
        }
        else
        {
            cableCaps = rxCableCaps;
        }

        if (AuxRetry::ack != bus.write(NV_DPCD20_CABLE_ATTRIBUTES_UPDATED_BY_DPTX, &cableCaps,
                                    sizeof cableCaps))
        {
            DP_PRINTF(DP_WARNING, "Failed to write NV_DPCD20_CABLE_ATTRIBUTES_UPDATED_BY_DPTX");
        }
    }
    else if (bCableVconnSourceUnknown)
    {
        // Write sink caps to NV_DPCD20_CABLE_ATTRIBUTES_UPDATED_BY_DPTX first.
        if (AuxRetry::ack != bus.write(NV_DPCD20_CABLE_ATTRIBUTES_UPDATED_BY_DPTX, &txCableCaps,
                                    sizeof txCableCaps))
        {
            DP_PRINTF(DP_WARNING, "Failed to write NV_DPCD20_CABLE_ATTRIBUTES_UPDATED_BY_DPTX");
        }

        // Check Cable ID from DPRX
        if (AuxRetry::ack !=
            bus.read(NV_DPCD20_CABLE_ATTRIBUTES_UPDATED_BY_DPRX, &rxCableCaps, sizeof rxCableCaps))
        {
            DP_PRINTF(DP_WARNING, "Failed to read NV_DPCD20_CABLE_ATTRIBUTES_UPDATED_BY_DPRX for updated results");
        }
        else
        {
            parseAndSetCableId(rxCableCaps);
        }

        // If no matches, reflect that to the DPRX
        if (txCableCaps != rxCableCaps)
        {
            if (AuxRetry::ack !=
                bus.write(NV_DPCD20_CABLE_ATTRIBUTES_UPDATED_BY_DPTX, &rxCableCaps, sizeof rxCableCaps))
            {
                DP_PRINTF(DP_WARNING, "Failed to update NV_DPCD20_CABLE_ATTRIBUTES_UPDATED_BY_DPTX -> %02x", rxCableCaps);
            }
        }
    }
}

void DPCDHALImpl2x::performCableIdHandshake()
{
    NvU8 rxCableCaps    = 0;

    if (this->bConnectorIsTypeC)
    {
        return performCableIdHandshakeForTypeC();
    }

    // Read Cable ID from DPRX
    if (AuxRetry::ack ==
        bus.read(NV_DPCD20_CABLE_ATTRIBUTES_UPDATED_BY_DPRX, &rxCableCaps, sizeof rxCableCaps))
    {
        parseAndSetCableId(rxCableCaps);
    }
}

void DPCDHALImpl2x::setUSBCCableIDInfo(NV0073_CTRL_DP_USBC_CABLEID_INFO *cableIDInfo)
{
    if (!cableIDInfo)
    {
        // Reset state as there is no limitation to be imposed from Tx Cable Info
        resetTxCableCaps();
        return;
    }

    caps2x.txCableCaps.bIsSupported           = true;
    caps2x.txCableCaps.bUHBR_10GSupported     = cableIDInfo->uhbr10_0_capable;
    caps2x.txCableCaps.bUHBR_13_5GSupported   = cableIDInfo->uhbr13_5_capable;
    caps2x.txCableCaps.bUHBR_20GSupported     = cableIDInfo->uhbr20_0_capable;
    caps2x.txCableCaps.bVconnSource           = cableIDInfo->vconn_source;
    switch (cableIDInfo->type)
    {
        case NV0073_CTRL_DP_USBC_CABLEID_CABLETYPE_PASSIVE:
            caps2x.txCableCaps.cableType = CableTypePassive;
            break;
        case NV0073_CTRL_DP_USBC_CABLEID_CABLETYPE_ACTIVE_RETIMER:
            caps2x.txCableCaps.cableType = CableTypeActiveReTimer;
            break;
        case NV0073_CTRL_DP_USBC_CABLEID_CABLETYPE_ACTIVE_LIN_REDRIVER:
            caps2x.txCableCaps.cableType = CableTypeLRD;
            break;
        case NV0073_CTRL_DP_USBC_CABLEID_CABLETYPE_OPTICAL:
            caps2x.txCableCaps.cableType = CableTypeOptical;
            break;
        case NV0073_CTRL_DP_USBC_CABLEID_CABLETYPE_UNKNOWN:
        default:
            caps2x.txCableCaps.cableType = CableTypeUnknown;
            break;
    }
    return;
}

void DPCDHALImpl2x::parseAndReadCaps()
{
    NvU8 buffer[16];
    NvU8 byte = 0;

    unsigned retries = 16;

    DPCDHALImpl::parseAndReadCaps();

    // 02206h
    if (AuxRetry::ack == bus.read(NV_DPCD14_EXTENDED_MAIN_LINK_CHANNEL_CODING, &buffer[0], 1))
    {
        caps2x.bDP2xChannelCodingSupported = FLD_TEST_DRF(_DPCD20, _EXTENDED_MAIN_LINK_CHANNEL_CODING,
                                                          _ANSI_128B_132B, _YES,
                                                          buffer[0]);

        // Read this unconditionally when the connection is tunneled
        if (caps2x.bDP2xChannelCodingSupported == true || caps.dpInTunnelingCaps.bIsSupported)
        {
            // 0x2215
            if (AuxRetry::ack == bus.read(NV_DPCD20_128B_132B_SUPPORTED_LINK_RATES, &buffer[0], 1))
            {
                caps2x.bUHBR_10GSupported =
                    FLD_TEST_DRF(_DPCD20,
                                 _128B_132B_SUPPORTED_LINK_RATES,
                                 _UHBR10,
                                 _YES,
                                 buffer[0]);

                caps2x.bUHBR_13_5GSupported =
                    FLD_TEST_DRF(_DPCD20,
                                 _128B_132B_SUPPORTED_LINK_RATES,
                                 _UHBR13_5,
                                 _YES,
                                 buffer[0]);

                caps2x.bUHBR_20GSupported =
                    FLD_TEST_DRF(_DPCD20,
                                 _128B_132B_SUPPORTED_LINK_RATES,
                                 _UHBR20,
                                 _YES,
                                 buffer[0]);
            }
            DP_ASSERT(caps2x.bUHBR_10GSupported && "Failed to read UHBR link rates or 128B/132B supported without UHBR10");
            if (AuxRetry::ack == bus.read(NV_DPCD14_FEC_CAPABILITY, &buffer[0], 1))
            {
                caps.bFECSupported = FLD_TEST_DRF(_DPCD14, _FEC_CAPABILITY,
                                                  _FEC_CAPABLE,
                                                  _YES,
                                                  buffer[0]);
            }
            DP_ASSERT(caps.bFECSupported && "Failed to read FEC Capability or 128B/132B supported without FEC");
        }
        else
        {
            caps2x.bUHBR_10GSupported   = NV_FALSE;
            caps2x.bUHBR_13_5GSupported = NV_FALSE;
            caps2x.bUHBR_20GSupported   = NV_FALSE;
        }
    }
    if (bLttprSupported)
    {
        if (AuxRetry::ack == bus.read(NV_DPCD14_LT_TUNABLE_PHY_REPEATER_REV, &buffer[0], 0x8, retries))
        {
            caps2x.repeaterCaps.bDP2xChannelCodingSupported =
                FLD_TEST_DRF(_DPCD20,
                             _PHY_REPEATER_MAIN_LINK_CHANNEL_CODING,
                             _128B_132B_SUPPORTED,
                             _YES,
                             buffer[6]);

            caps2x.repeaterCaps.bUHBR_10GSupported =
                FLD_TEST_DRF(_DPCD20,
                             _PHY_REPEATER_128B_132B_RATES,
                             _10G_SUPPORTED,
                             _YES,
                             buffer[7]);

            caps2x.repeaterCaps.bUHBR_13_5GSupported =
                FLD_TEST_DRF(_DPCD20,
                             _PHY_REPEATER_128B_132B_RATES,
                             _13_5G_SUPPORTED,
                             _YES,
                             buffer[7]);

            caps2x.repeaterCaps.bUHBR_20GSupported =
                FLD_TEST_DRF(_DPCD20,
                             _PHY_REPEATER_128B_132B_RATES,
                             _20G_SUPPORTED,
                             _YES,
                             buffer[7]);

            if (buffer[7] && !caps2x.repeaterCaps.bDP2xChannelCodingSupported)
            {
                DP_ASSERT(0 && "LTTPR: UHBR is supported without 128b/132b Channel Encoding Supported!");
            }

            if (caps2x.repeaterCaps.bDP2xChannelCodingSupported)
            {
                DP_ASSERT(caps.repeaterCaps.bFECSupported &&
                          "LTTPR: UHBR is supported without FEC Support!");
            }
        }
    }

    if (caps2x.bDP2xChannelCodingSupported && !bIgnoreCableIdCaps)
    {
        performCableIdHandshake();
    }

    if (caps.dpInTunnelingCaps.bIsSupported)
    {
        if (AuxRetry::ack ==
            bus.read(NV_DPCD20_DP_TUNNELING_MAIN_LINK_CHANNEL_CODING, &byte, sizeof byte))
        {
            caps2x.dpInTunnelingCaps.bDP2xChannelCodingSupported =
                FLD_TEST_DRF(_DPCD20, _DP_TUNNELING_128B132B,
                            _DP_SUPPORTED, _YES, byte);
        }
    }
    if (caps2x.dpInTunnelingCaps.bDP2xChannelCodingSupported)
    {
        if (AuxRetry::ack ==
            bus.read(NV_DPCD20_DP_TUNNELING_128B132B_LINK_RATES, &byte, sizeof byte))
        {
            caps2x.dpInTunnelingCaps.bUHBR_10GSupported =
                FLD_TEST_DRF(_DPCD20, _DP_TUNNELING_128B132B_LINK_RATES, _10_0_GPBS_SUPPORTED, _YES, byte);

            caps2x.dpInTunnelingCaps.bUHBR_13_5GSupported =
                FLD_TEST_DRF(_DPCD20, _DP_TUNNELING_128B132B_LINK_RATES, _13_5_GPBS_SUPPORTED, _YES, byte);

            caps2x.dpInTunnelingCaps.bUHBR_20GSupported =
                FLD_TEST_DRF(_DPCD20, _DP_TUNNELING_128B132B_LINK_RATES, _20_0_GPBS_SUPPORTED, _YES, byte);
        }
    }
}

void DPCDHALImpl2x::setGpuDPSupportedVersions(NvU32 _gpuDPSupportedVersions)
{
    bool bSupportDp1_2 = FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_DP_VERSIONS_SUPPORTED, _DP1_2,
                                      _YES, _gpuDPSupportedVersions);
    bool bSupportDp1_4 = FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_DP_VERSIONS_SUPPORTED, _DP1_4,
                                      _YES, _gpuDPSupportedVersions);
    bool bSupportDp2_0 = FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_DP_VERSIONS_SUPPORTED, _DP2_0,
                                      _YES, _gpuDPSupportedVersions);
    if (bSupportDp2_0)
    {
        DP_ASSERT(bSupportDp1_4 && bSupportDp1_2 && "GPU supports DP1.4 should also support DP1.4 and DP1.2!");
    }

    return DPCDHALImpl::setGpuDPSupportedVersions(_gpuDPSupportedVersions);
}

void DPCDHALImpl2x::setConnectorTypeC(bool bTypeC)
{
    this->bConnectorIsTypeC = bTypeC;
}

AuxRetry::status DPCDHALImpl2x::notifySDPErrDetectionCapability()
{
    NvU8 config = 0;
    AuxRetry::status status = AuxRetry::ack;

    if (!FLD_TEST_DRF(0073_CTRL_CMD_DP,
                      _GET_CAPS_DP_VERSIONS_SUPPORTED,
                      _DP2_0, _YES, gpuDPSupportedVersions))
    {
        // If the GPU does not support DP2.0 features, don't do anything.
        return status;
    }

    if (!isAtLeastVersion(1, 4) || !(caps2x.bDP2xChannelCodingSupported))
    {
        //
        // If the root device is less then DP1.4 or does not support 128b/132b channel encoding,
        // don't do anything.
        //
        return status;
    }

    if ((caps.phyRepeaterCount != 0) &&
        (!caps2x.repeaterCaps.bDP2xChannelCodingSupported))
    {
        // If there are LTTPRs that does not support 128b/132b channel encoding, don't do anything.
        return status;
    }

    config = FLD_SET_DRF(_DPCD20, _SDP_ERR_DETECTION_CONF, _CRC16_128B_132B_SUPPORTED,
                         _YES, config);

    status = bus.write(NV_DPCD20_SDP_ERR_DETECTION_CONF, &config, sizeof(config));
    if (AuxRetry::ack != status)
    {
        DP_PRINTF(DP_WARNING, "Failed to write NV_DPCD20_SDP_ERR_DETECTION_CONF");
    }
    return status;
}

bool DPCDHALImpl2x::isDp2xChannelCodingCapable()
{
    // return false if the device does not support 128b/132b.
    // However when dpTunneling is enabled, read the tunneling cap instead
    if (caps.dpInTunnelingCaps.bIsSupported)
    {
        // return false if DP-IN Tunneling is supported but not support 128b/132b.
        if (!caps2x.dpInTunnelingCaps.bDP2xChannelCodingSupported)
            return false;
    }
    else
    {
        if (!caps2x.bDP2xChannelCodingSupported)
            return false;
    }

    // return true if there is no LTTPR.
    if (!bLttprSupported || (caps.phyRepeaterCount == 0))
        return true;

    // If there is any LTTPR, return false if any of the LTTPR does not support 128b/132b
    if (!caps2x.repeaterCaps.bDP2xChannelCodingSupported || !lttprIsAtLeastVersion(2, 0))
    {
        // If there are LTTPRs that does not support 128b/132b channel encoding, it's not DP2x capable.
        return false;
    }

    return true;
}

//
// Legacy link rates: DPCD offset 1 * 27000000
// ILRs: DPCD offset: 0x10~0x1F
// UHBR sink: DPCD offset: 0x2215
//     LTTPR: DPCD offset: 0xF0007
//
LinkRate DPCDHALImpl2x::getMaxLinkRate()
{
    LinkRate    maxLinkRate                 = DPCDHALImpl::getMaxLinkRate();
    NvU32       uhbrCaps                    = getUHBRSupported();
    bool        bDP2xChannelCodingSupported = isDp2xChannelCodingCapable();

    if (bDP2xChannelCodingSupported)
    {
        if(FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_UHBR_SUPPORTED, _UHBR10_0, _YES, uhbrCaps))
        {
            maxLinkRate = dp2LinkRate_10_0Gbps;
        }

        if(FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_UHBR_SUPPORTED, _UHBR13_5, _YES, uhbrCaps))
        {
            maxLinkRate = dp2LinkRate_13_5Gbps;
        }

        if(FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_UHBR_SUPPORTED, _UHBR20_0, _YES, uhbrCaps))
        {
            maxLinkRate = dp2LinkRate_20_0Gbps;
        }
    }
    return maxLinkRate;
}

//
// The function checks for sink side UHBR capabilities as a combination of
// a. Sink Caps.
// b. Cable ID Caps
// c. LTTPR Caps.
// d. DP Tunneling Caps.
//
NvU32 DPCDHALImpl2x::getUHBRSupported()
{
    NvU32   uhbrCaps             = 0;

    bool    bUHBR_10GSupported   = caps2x.bUHBR_10GSupported;
    bool    bUHBR_13_5GSupported = caps2x.bUHBR_13_5GSupported;
    bool    bUHBR_20GSupported   = caps2x.bUHBR_20GSupported;

    // When tunneling is supported and bw allocation is enabled, override the caps from tunneling caps
    if (caps.dpInTunnelingCaps.bIsSupported && bIsDpTunnelBwAllocationEnabled)
    {
        bUHBR_10GSupported   = caps2x.dpInTunnelingCaps.bUHBR_10GSupported;
        bUHBR_13_5GSupported = caps2x.dpInTunnelingCaps.bUHBR_13_5GSupported;
        bUHBR_20GSupported   = caps2x.dpInTunnelingCaps.bUHBR_20GSupported;
    }

    if (!bIgnoreCableIdCaps)
    {
        bUHBR_10GSupported   = bUHBR_10GSupported   &&  caps2x.rxCableCaps.bUHBR_10GSupported
                                                            && caps2x.txCableCaps.bUHBR_10GSupported;
        bUHBR_13_5GSupported = bUHBR_13_5GSupported &&  caps2x.rxCableCaps.bUHBR_13_5GSupported
                                                            && caps2x.txCableCaps.bUHBR_13_5GSupported;
        bUHBR_20GSupported   = bUHBR_20GSupported   &&  caps2x.rxCableCaps.bUHBR_20GSupported
                                                            && caps2x.txCableCaps.bUHBR_20GSupported;
    }

    if (caps.phyRepeaterCount > 0)
    {
        bUHBR_10GSupported   = bUHBR_10GSupported   && caps2x.repeaterCaps.bUHBR_10GSupported;
        bUHBR_13_5GSupported = bUHBR_13_5GSupported && caps2x.repeaterCaps.bUHBR_13_5GSupported;
        bUHBR_20GSupported   = bUHBR_20GSupported   && caps2x.repeaterCaps.bUHBR_20GSupported;
    }

    if (bUHBR_10GSupported)
    {
        uhbrCaps = FLD_SET_DRF(0073_CTRL_CMD_DP, _GET_CAPS_UHBR_SUPPORTED, _UHBR10_0, _YES, uhbrCaps);
    }
    if (bUHBR_13_5GSupported)
    {
        uhbrCaps = FLD_SET_DRF(0073_CTRL_CMD_DP, _GET_CAPS_UHBR_SUPPORTED, _UHBR13_5, _YES, uhbrCaps);
    }
    if (bUHBR_20GSupported)
    {
        uhbrCaps = FLD_SET_DRF(0073_CTRL_CMD_DP, _GET_CAPS_UHBR_SUPPORTED, _UHBR20_0, _YES, uhbrCaps);
    }

    return uhbrCaps;
}

void DPCDHALImpl2x::overrideCableIdCap(LinkRate linkRate, bool bEnable)
{
    switch (linkRate)
    {
        case dp2LinkRate_20_0Gbps:
            caps2x.rxCableCaps.bUHBR_20GSupported = bEnable;
            break;
        case dp2LinkRate_13_5Gbps:
            caps2x.rxCableCaps.bUHBR_13_5GSupported = bEnable;
            break;
        case dp2LinkRate_10_0Gbps:
            caps2x.rxCableCaps .bUHBR_10GSupported = bEnable;
            break;
        default:
            DP_PRINTF(DP_ERROR, "DPHAL> Invalid link rate (%d) to override.", linkRate);
    }
}

bool DPCDHALImpl2x::parseTestRequestPhy()
{
    NvU8 buffer[2] = {0};

    if (AuxRetry::ack != bus.read(NV_DPCD20_PHY_TEST_PATTERN, &buffer[0], 2))
    {
        DP_PRINTF(DP_ERROR, "DPHAL> ERROR! Test pattern request found but unable to read NV_DPCD20_PHY_TEST_PATTERN register.");
        return false;
    }

    switch (DRF_VAL(_DPCD20, _PHY_TEST_PATTERN_SEL, _DP20, buffer[0]))
    {
        case NV_DPCD20_PHY_TEST_PATTERN_SEL_128B132B_TPS1:
            interrupts2x.testPhyCompliance2x.phyTestPattern = LINK_QUAL_128B132B_TPS1;
            break;
        case NV_DPCD20_PHY_TEST_PATTERN_SEL_128B132B_TPS2:
            interrupts2x.testPhyCompliance2x.phyTestPattern = LINK_QUAL_128B132B_TPS2;
            break;
        case NV_DPCD20_PHY_TEST_PATTERN_SEL_PRBS9:
            interrupts2x.testPhyCompliance2x.phyTestPattern = LINK_QUAL_PRBS9;
            break;
        case NV_DPCD20_PHY_TEST_PATTERN_SEL_PRBS11:
            interrupts2x.testPhyCompliance2x.phyTestPattern = LINK_QUAL_PRBS11;
            break;
        case NV_DPCD20_PHY_TEST_PATTERN_SEL_PRBS15:
            interrupts2x.testPhyCompliance2x.phyTestPattern = LINK_QUAL_PRBS15;
            break;
        case NV_DPCD20_PHY_TEST_PATTERN_SEL_PRBS23:
            interrupts2x.testPhyCompliance2x.phyTestPattern = LINK_QUAL_PRBS23;
            break;
        case NV_DPCD20_PHY_TEST_PATTERN_SEL_PRBS31:
            interrupts2x.testPhyCompliance2x.phyTestPattern = LINK_QUAL_PRBS31;
            break;
        case NV_DPCD20_PHY_TEST_PATTERN_SEL_264_BIT_CUSTOM:
            interrupts2x.testPhyCompliance2x.phyTestPattern = LINK_QUAL_264BIT_CUST;
            break;
        case NV_DPCD20_PHY_TEST_PATTERN_SEL_SQUARE_SEQ_WITH_PRESHOOT_ON_DE_EMPHASIS_ON:
            interrupts2x.testPhyCompliance2x.phyTestPattern = LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_ON_DE_EMPHASIS_ON;
            interrupts2x.testPhyCompliance2x.sqNum          = buffer[1];
            break;
        case NV_DPCD20_PHY_TEST_PATTERN_SEL_SQUARE_SEQ_WITH_PRESHOOT_OFF_DE_EMPHASIS_ON:
            interrupts2x.testPhyCompliance2x.phyTestPattern = LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_OFF_DE_EMPHASIS_ON;
            interrupts2x.testPhyCompliance2x.sqNum          = buffer[1];
            break;
        case NV_DPCD20_PHY_TEST_PATTERN_SEL_SQUARE_SEQ_WITH_PRESHOOT_ON_DE_EMPHASIS_OFF:
            interrupts2x.testPhyCompliance2x.phyTestPattern = LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_ON_DE_EMPHASIS_OFF;
            interrupts2x.testPhyCompliance2x.sqNum          = buffer[1];
            break;
        case NV_DPCD20_PHY_TEST_PATTERN_SEL_SQUARE_SEQ_WITH_PRESHOOT_OFF_DE_EMPHASIS_OFF:
            interrupts2x.testPhyCompliance2x.phyTestPattern = LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_OFF_DE_EMPHASIS_OFF;
            interrupts2x.testPhyCompliance2x.sqNum          = buffer[1];
            break;
        default:
            // now look for DP1.X test patterns
            interrupts2x.testPhyCompliance2x.phyTestPattern = LINK_QUAL_DISABLED;
            return DPCDHALImpl::parseTestRequestPhy();
    }

    if (interrupts2x.testPhyCompliance2x.phyTestPattern == LINK_QUAL_264BIT_CUST)
    {
        if (AuxRetry::ack != bus.read(NV_DPCD20_TEST_264BIT_CUSTOM_PATTERN(0),
                                      &(interrupts2x.cstm264Bits[0]),
                                      NV_DPCD20_TEST_264BIT_CUSTOM_PATTERN__SIZE))
        {
            DP_PRINTF(DP_ERROR, "DPHAL> ERROR! Request for 264bit custom pattern. Can't read from 0x%08xh.",
                      NV_DPCD20_TEST_264BIT_CUSTOM_PATTERN(0));
            return false;
        }
    }

    // copy the parsed interrupt info to parent struct
    interrupts.testPhyCompliance.phyTestPattern = interrupts2x.testPhyCompliance2x.phyTestPattern;
    interrupts2x.testPhyCompliance2x.testRequestPhyCompliance = true;
    return true;
}

bool DPCDHALImpl2x::parseTestRequestTraining(NvU8 * buffer)
{
    if (buffer == NULL)
    {
        DP_ASSERT(0 && "Buffer invalid, bail out");
        return false;
    }

    // 0x218, bit 4
    bool    bDP2xChannelCodingRequested = FLD_TEST_DRF(_DPCD20, _TEST_REQUEST,
                                                       _PHY_TEST_CHANNEL_CODING, _128B132B, buffer[0]);

    if (!bDP2xChannelCodingRequested)
        return DPCDHALImpl::parseTestRequestTraining(buffer);

    // 0x219
    switch (buffer[1])
    {
        case NV_DPCD20_TEST_LINK_RATE_TYPE_20_0G:
            interrupts.testTraining.testRequestLinkRate = dp2LinkRate_20_0Gbps;
            break;
        case NV_DPCD20_TEST_LINK_RATE_TYPE_13_5G:
        case NV_DPCD20_TEST_LINK_RATE_TYPE_13_5G_2:
            interrupts.testTraining.testRequestLinkRate = dp2LinkRate_13_5Gbps;
            break;
        case NV_DPCD20_TEST_LINK_RATE_TYPE_10_0G:
            interrupts.testTraining.testRequestLinkRate = dp2LinkRate_10_0Gbps;
            break;
        default:
            interrupts.testTraining.testRequestLinkRate = dp2LinkRate_10_0Gbps;
            DP_ASSERT(0 && "Unknown requested link rate.  Assuming UHBR10");
            break;
    }

    // 0x220.
    interrupts.testTraining.testRequestLaneCount  = buffer[(0x220 - 0x218)] & 0xf;

    return true;
}

void DPCDHALImpl2x::get264BitsCustomTestPattern(NvU8 *testPattern)
{
    int i;

    for (i = 0; i < NV_DPCD20_TEST_264BIT_CUSTOM_PATTERN__SIZE; i++)
    {
        testPattern[i] = interrupts2x.cstm264Bits[i];
    }
}

void DPCDHALImpl2x::getSquarePatternNum(NvU8 *sqNum)
{
    if (sqNum != NULL)
    {
        *sqNum = interrupts2x.testPhyCompliance2x.sqNum;
    }
    return;
}

AuxRetry::status DPCDHALImpl2x::setLinkQualPatternSet
(
    DP2xPatternInfo& patternInfo,
    unsigned laneCount
)
{
    unsigned writeAddr = NV_DPCD20_LINK_SQUARE_PATTERN_NUM_PLUS_1;

    if ((patternInfo.lqsPattern == LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_ON_DE_EMPHASIS_ON) ||
        (patternInfo.lqsPattern == LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_OFF_DE_EMPHASIS_ON) ||
        (patternInfo.lqsPattern == LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_ON_DE_EMPHASIS_OFF) ||
        (patternInfo.lqsPattern == LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_OFF_DE_EMPHASIS_OFF))
    {
        if (AuxRetry::ack != bus.write(writeAddr, &(patternInfo.sqNum), sizeof (patternInfo.sqNum)))
        {
            DP_ASSERT(0 && "Could not set LINK_SQUARE_PATTERN_NUM_PLUS_1");
        }
    }

    return DPCDHALImpl::setLinkQualPatternSet(patternInfo.lqsPattern, laneCount);
}

AuxRetry::status DPCDHALImpl2x::setLinkQualLaneSet(unsigned lane, LinkQualityPatternType linkQualPattern)
{
    NvU8 linkQuality = 0;
    unsigned writeAddress = NV_DPCD20_LINK_QUAL_LANE_SET(lane);

    if (caps.revisionMajor <= 0)
        DP_ASSERT(0 && "Something is wrong, revision major should be > 0");

    if (isAtLeastVersion(1, 2) == false)
    {
        DP_ASSERT(0 && "Regs only supported for DP1.2");
        return AuxRetry::unsupportedRegister;
    }

    // check if parameter is valid
    if (lane >= displayPort_LaneSupported)
    {
        DP_ASSERT(0 && "Unknown lane selected.  Assuming Lane 0");
        writeAddress = NV_DPCD20_LINK_QUAL_LANE_SET(0);
    }

    switch (linkQualPattern)
    {
        case LINK_QUAL_DISABLED:
            linkQuality = FLD_SET_DRF(_DPCD, _LINK_QUAL_LANE_SET, _LQS, _NO, linkQuality);
            break;
        case LINK_QUAL_D10_2:
            linkQuality = FLD_SET_DRF(_DPCD, _LINK_QUAL_LANE_SET, _LQS, _D10_2, linkQuality);
            break;
        case LINK_QUAL_SYM_ERROR:
            linkQuality = FLD_SET_DRF(_DPCD, _LINK_QUAL_LANE_SET, _LQS, _SYM_ERR_MEASUREMENT_CNT, linkQuality);
            break;
        case LINK_QUAL_PRBS7:
            linkQuality = FLD_SET_DRF(_DPCD, _LINK_QUAL_LANE_SET, _LQS, _PRBS7, linkQuality);
            break;
        case LINK_QUAL_80BIT_CUST:
            linkQuality = FLD_SET_DRF(_DPCD, _LINK_QUAL_LANE_SET, _LQS, _80_BIT_CUSTOM, linkQuality);
            break;
        case LINK_QUAL_HBR2_COMPLIANCE_EYE:
            linkQuality = FLD_SET_DRF(_DPCD, _LINK_QUAL_LANE_SET, _LQS, _HBR2, linkQuality);
            break;
        case LINK_QUAL_CP2520PAT3:
            linkQuality = FLD_SET_DRF(_DPCD14, _LINK_QUAL_LANE_SET, _LQS, _CP2520PAT3, linkQuality);
            break;
        case LINK_QUAL_128B132B_TPS1:
            linkQuality = FLD_SET_DRF(_DPCD20, _LINK_QUAL_LANE_SET, _LQS, _128B132B_TPS1, linkQuality);
            break;
        case LINK_QUAL_128B132B_TPS2:
            linkQuality = FLD_SET_DRF(_DPCD20, _LINK_QUAL_LANE_SET, _LQS, _128B132B_TPS2, linkQuality);
            break;
        case LINK_QUAL_PRBS9:
            linkQuality = FLD_SET_DRF(_DPCD20, _LINK_QUAL_LANE_SET, _LQS, _PRBS9, linkQuality);
            break;
        case LINK_QUAL_PRBS11:
            linkQuality = FLD_SET_DRF(_DPCD20, _LINK_QUAL_LANE_SET, _LQS, _PRBS11, linkQuality);
            break;
        case LINK_QUAL_PRBS15:
            linkQuality = FLD_SET_DRF(_DPCD20, _LINK_QUAL_LANE_SET, _LQS, _PRBS15, linkQuality);
            break;
        case LINK_QUAL_PRBS23:
            linkQuality = FLD_SET_DRF(_DPCD20, _LINK_QUAL_LANE_SET, _LQS, _PRBS23, linkQuality);
            break;
        case LINK_QUAL_PRBS31:
            linkQuality = FLD_SET_DRF(_DPCD20, _LINK_QUAL_LANE_SET, _LQS, _PRBS31, linkQuality);
            break;
        case LINK_QUAL_264BIT_CUST:
            linkQuality = FLD_SET_DRF(_DPCD20, _LINK_QUAL_LANE_SET, _LQS, _264_BIT_CUSTOM, linkQuality);
            break;
        case LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_ON_DE_EMPHASIS_ON:
            linkQuality = FLD_SET_DRF(_DPCD20, _LINK_QUAL_LANE_SET, _LQS, _SQUARE_SEQ_WITH_PRESHOOT_ON_DE_EMPHASIS_ON, linkQuality);
            break;
        case LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_OFF_DE_EMPHASIS_ON:
            linkQuality = FLD_SET_DRF(_DPCD20, _LINK_QUAL_LANE_SET, _LQS, _SQUARE_SEQ_WITH_PRESHOOT_OFF_DE_EMPHASIS_ON, linkQuality);
            break;
        case LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_ON_DE_EMPHASIS_OFF:
            linkQuality = FLD_SET_DRF(_DPCD20, _LINK_QUAL_LANE_SET, _LQS, _SQUARE_SEQ_WITH_PRESHOOT_ON_DE_EMPHASIS_OFF, linkQuality);
            break;
        case LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_OFF_DE_EMPHASIS_OFF:
            linkQuality = FLD_SET_DRF(_DPCD20, _LINK_QUAL_LANE_SET, _LQS, _SQUARE_SEQ_WITH_PRESHOOT_OFF_DE_EMPHASIS_OFF, linkQuality);
            break;
    }
    return bus.write(writeAddress, &linkQuality, sizeof linkQuality);
}

AuxRetry::status DPCDHALImpl2x::setMainLinkChannelCoding(MainLinkChannelCoding channelCoding)
{
    NvU8 channelCodingSet = 0;
    switch (channelCoding)
    {
        case ChannelCoding8B10B:
            channelCodingSet = FLD_SET_DRF(_DPCD, _MAIN_LINK_CHANNEL_CODING_SET, _ANSI_8B_10B, _TRUE, channelCodingSet);
            break;
        case ChannelCoding128B132B:
            channelCodingSet = FLD_SET_DRF(_DPCD, _MAIN_LINK_CHANNEL_CODING_SET, _ANSI_128B_132B, _TRUE, channelCodingSet);
            break;
    }

    return bus.write(NV_DPCD_MAIN_LINK_CHANNEL_CODING_SET, &channelCodingSet , sizeof channelCodingSet);
}

MainLinkChannelCoding DPCDHALImpl2x::getMainLinkChannelCoding()
{
    NvU8 channelCodingSet = 0;

    if (!isDp2xChannelCodingCapable())
        return ChannelCoding8B10B;

    if (AuxRetry::ack != bus.read(NV_DPCD_MAIN_LINK_CHANNEL_CODING_SET, &channelCodingSet , sizeof channelCodingSet))
        return ChannelCoding8B10B;

    if (FLD_TEST_DRF(_DPCD, _MAIN_LINK_CHANNEL_CODING_SET, _ANSI_128B_132B, _TRUE, channelCodingSet))
        return ChannelCoding128B132B;

    return ChannelCoding8B10B;
}

