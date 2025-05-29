/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
* Module: dp_connectorimpl2x.cpp                                            *
*    DP connector implementation for DP2.x                                  *
*                                                                           *
\***************************************************************************/



#include "dp_connectorimpl.h"
#include "dp_connectorimpl2x.h"
#include "dp_evoadapter2x.h"
#include "ctrl/ctrl0073/ctrl0073dp.h"
#include "dp_printf.h"

#define LOGICAL_LANES          4U
#define EFF_BPP_NON_DSC_SCALER 256U
#define EFF_BPP_DSC_SCALER     16U
//
// DP1: 8b/10b,    1 symbol is 8 bits.
// DP2: 128b/132b, 1 symbol is 32 bits.
//
#define DP1_SYMBOL_SIZE       8U
#define DP2_SYMBOL_SIZE      32U

#define GET_SYMBOL_SIZE(bIsDp2xChannelCoding) ((bIsDp2xChannelCoding) ? DP2_SYMBOL_SIZE : DP1_SYMBOL_SIZE)

using namespace DisplayPort;
//
// The regkey value is available in connector, so using a global variable
// to expose it in LinkConfiguration
//
NvU32 bSupportInternalUhbrOnFpga;

/*!
 * @brief Convert data rate to link rate
 *
 * @param[in]   dataRate      The data rate needs to be converted.
 *
 * @return      link rate corresponding to the dataRate.
 *
 */

void ConnectorImpl2x::applyDP2xRegkeyOverrides()
{
    this->bSupportUHBR2_50      = dpRegkeyDatabase.supportInternalUhbrOnFpga & NV_DP2X_REGKEY_FPGA_UHBR_SUPPORT_2_5G;
    this->bSupportUHBR2_70      = dpRegkeyDatabase.supportInternalUhbrOnFpga & NV_DP2X_REGKEY_FPGA_UHBR_SUPPORT_2_7G;
    this->bSupportUHBR5_00      = dpRegkeyDatabase.supportInternalUhbrOnFpga & NV_DP2X_REGKEY_FPGA_UHBR_SUPPORT_5_0G;
    this->maxLinkRateFromRegkey = dpRegkeyDatabase.applyMaxLinkRateOverrides;
    bSupportInternalUhbrOnFpga  = dpRegkeyDatabase.supportInternalUhbrOnFpga;
    if (dpRegkeyDatabase.bIgnoreCableIdCaps)
    {
        hal->setIgnoreCableIdCaps(true);
    }
    if (dpRegkeyDatabase.bCableVconnSourceUnknownWar)
    {
        hal->setCableVconnSourceUnknown();
    }
}

bool ConnectorImpl2x::getValidLowestLinkConfig
(
    LinkConfiguration      &lConfig,
    LinkConfiguration      &lowestSelected,
    ModesetInfo             modesetInfo,
    const DscParams        *pDscParams
)
{
    bool bIsModeSupported = ConnectorImpl::getValidLowestLinkConfig(lConfig, lowestSelected, modesetInfo, pDscParams);
    bool bAvoidHBR3       = main->isAvoidHBR3WAREnabled() && (lowestSelected.peakRate == dp2LinkRate_8_10Gbps);
    unsigned i = 0;
    if (!bIsModeSupported || !bAvoidHBR3)
    {
        return bIsModeSupported;
    }
    for (i = 0; i < numPossibleLnkCfg; i++)
    {
        if ((this->allPossibleLinkCfgs[i].lanes     != lowestSelected.lanes) ||
            (this->allPossibleLinkCfgs[i].peakRate  != lowestSelected.peakRate))
        {
            continue;
        }
        // check if it's the max possible link
        if ((this->allPossibleLinkCfgs[i].lanes     != highestAssessedLC.lanes) ||
            (this->allPossibleLinkCfgs[i].peakRate  != highestAssessedLC.peakRate))
        {
            // Get next entry.
            lowestSelected = this->allPossibleLinkCfgs[i+1];
            lowestSelected.enableFEC(lConfig.bEnableFEC);
        }
    }
    return bIsModeSupported;
}

bool ConnectorImpl2x::willLinkSupportModeSST
(
    const LinkConfiguration & linkConfig,
    const ModesetInfo & modesetInfo,
    const DscParams *pDscParams
)
{
    LinkConfiguration lc = linkConfig;
    if (!main->isSupportedDPLinkConfig(lc))
        return false;
    // no headIndex (default 0) for mode enumeration.
    return willLinkSupportMode(linkConfig, modesetInfo, 0, NULL, pDscParams);
}

bool ConnectorImpl2x::willLinkSupportMode
(
    const LinkConfiguration    &linkConfig,
    const ModesetInfo          &modesetInfo,
    NvU32                       headIndex,
    Watermark                  *watermark,
    const DscParams            *pDscParams
)
{
    //
    // mode is not known yet, we have to report is possible
    // Otherwise we're going to mark all devices as zombies on first HPD(c),
    // since modeset info is not available.
    //
    if (modesetInfo.pixelClockHz == 0)
        return true;

    if (linkConfig.lanes == 0 || linkConfig.peakRate == 0)
        return false;

    DP_ASSERT(this->isFECSupported());

    EvoInterface   *provider        = ((EvoMainLink *)main)->getProvider();
    NvU32           nvosStatus      = NVOS_STATUS_SUCCESS;

    NV0073_CTRL_CMD_CALCULATE_DP_IMP_PARAMS impParams;
    dpMemZero(&impParams, sizeof(NV0073_CTRL_CMD_CALCULATE_DP_IMP_PARAMS));

    impParams.subDeviceInstance                 = provider->getSubdeviceIndex();
    impParams.displayId                         = provider->getDisplayId();
    impParams.headIndex                         = headIndex;

    impParams.modesetInfo.twoChannelAudioHz     = modesetInfo.twoChannelAudioHz;
    impParams.modesetInfo.eightChannelAudioHz   = modesetInfo.eightChannelAudioHz;
    impParams.modesetInfo.pixelFrequencyKHz     = (NvU32)(modesetInfo.pixelClockHz / 1000);
    impParams.modesetInfo.rasterWidth           = modesetInfo.rasterWidth;
    impParams.modesetInfo.surfaceWidth          = modesetInfo.surfaceWidth;
    impParams.modesetInfo.surfaceHeight         = modesetInfo.surfaceHeight;
    impParams.modesetInfo.depth                 = modesetInfo.depth;
    impParams.modesetInfo.rasterBlankStartX     = modesetInfo.rasterBlankStartX;
    impParams.modesetInfo.rasterBlankEndX       = modesetInfo.rasterBlankEndX;
    impParams.modesetInfo.bitsPerComponent      = modesetInfo.bitsPerComponent;
    impParams.modesetInfo.colorFormat           = modesetInfo.colorFormat;
    impParams.modesetInfo.bDSCEnabled           = modesetInfo.bEnableDsc;

    impParams.linkConfig.linkRate10M            = (NvU32)linkConfig.peakRate;
    impParams.linkConfig.laneCount              = linkConfig.lanes;
    impParams.linkConfig.bEnhancedFraming       = linkConfig.enhancedFraming;
    impParams.linkConfig.bDp2xChannelCoding     = linkConfig.bIs128b132bChannelCoding;
    impParams.linkConfig.bFECEnabled            = linkConfig.bEnableFEC;
    impParams.linkConfig.bMultiStreamTopology   = linkConfig.multistream;
    impParams.linkConfig.bDisableEffBppSST8b10b = this->bDisableEffBppSST8b10b;

    if (pDscParams != NULL && pDscParams->forcedParams != NULL)
    {
        impParams.dscInfo.sliceCount            = pDscParams->forcedParams->sliceCount;
        impParams.dscInfo.sliceWidth            = pDscParams->forcedParams->sliceWidth;
        impParams.dscInfo.sliceHeight           = pDscParams->forcedParams->sliceHeight;
        impParams.dscInfo.dscVersionMajor       = pDscParams->forcedParams->dscRevision.versionMajor;
        impParams.dscInfo.dscVersionMinor       = pDscParams->forcedParams->dscRevision.versionMinor;
    }

    nvosStatus = provider->rmControl0073(NV0073_CTRL_CMD_CALCULATE_DP_IMP,
                                         &impParams, sizeof impParams);

    if ((nvosStatus == NVOS_STATUS_SUCCESS) && (watermark != NULL))
    {
        watermark->waterMark    = impParams.watermark.waterMark;
        watermark->tuSize       = impParams.watermark.tuSize;
        watermark->hBlankSym    = impParams.watermark.hBlankSym;
        watermark->vBlankSym    = impParams.watermark.vBlankSym;
        watermark->effectiveBpp = impParams.watermark.effectiveBpp;
    }

    return impParams.watermark.bIsModePossible;
}

/*!
 * @brief Check if the link configuration is valid on the system to be trained.
 *
 * @param[in]   lConfig         The link configuration to be validated.
 *
 * @return      If the input link configuration is valid
 */
bool ConnectorImpl2x::validateLinkConfiguration(const LinkConfiguration &lConfig)
{
   NvU64 linkRate10M = lConfig.peakRate;

    if (!IS_VALID_LANECOUNT(lConfig.lanes))
    {
        DP_PRINTF(DP_ERROR, "DP2xCONN> Invalid Lane Count=%d", lConfig.lanes);
        return false;
    }

    if (lConfig.lanes > hal->getMaxLaneCount())
    {
        DP_PRINTF(DP_ERROR, "DP2xCONN> Requested Lane Count=%d is larger than sinkMaxLaneCount=%d",
                  lConfig.lanes, hal->getMaxLaneCount());
        return false;
    }

    if (lConfig.lanes != 0)
    {
        if (!IS_VALID_DP2_X_LINKBW(linkRate10M))
        {
            DP_PRINTF(DP_ERROR, "DP2xCONN> Requested link rate=%d is not valid", linkRate10M);
            return false;
        }

        if (lConfig.peakRate > hal->getMaxLinkRate())
        {
            DP_PRINTF(DP_ERROR, "DP2xCONN> Requested link rate=%d is larger than sinkMaxLinkRate=%d",
                      linkRate10M, hal->getMaxLinkRate());
            return false;
        }

        if (IS_INTERMEDIATE_DP2_X_LINKBW(linkRate10M))
        {
            NvU16 *ilrTable;
            NvU32 i;
            if (!hal->isIndexedLinkrateEnabled())
            {
                DP_PRINTF(DP_ERROR, "DP2xCONN> Indexed Link Rate=%d is Not Enabled in Sink", linkRate10M);
                return false;
            }

            ilrTable = hal->getLinkRateTable();
            for (i = 0; i < NV0073_CTRL_DP_MAX_INDEXED_LINK_RATES; i++)
            {
                //
                // lConfig.peakRate is in 10M convention and ilrTable entries are the values read from DPCD which is 200k convention
                // Convert the ilrTable value to 10M from 200KHz before the comparison
                //
                if (LINK_RATE_200KHZ_TO_10MHZ(ilrTable[i]) == linkRate10M)
                    break;
                if (ilrTable[i] == 0)
                {
                    DP_PRINTF(DP_ERROR, "DP2xCONN> Indexed Link Rate=%d is Not Found", linkRate10M);
                    return false;
                }
            }
            if (i == NV0073_CTRL_DP_MAX_INDEXED_LINK_RATES)
            {
                DP_PRINTF(DP_ERROR, "DP2xCONN> Indexed Link Rate=%d is Not Found", linkRate10M);
                return false;
            }
        }
    }
    return true;
}

void ConnectorImpl2x::populateAllDpConfigs()
{
    LinkRate    maxLinkRate;

    // maxLinkRateSupported checks GPU and DFP caps. UHBR is included.
    LinkRate    gpuMaxLinkRate = main->maxLinkRateSupported();
    NvU32       gpuUhbrCaps    = main->getUHBRSupported();

    // sinkMaxLinkRate checks LTTPR and sink caps. UHBR is included.
    LinkRate    sinkMaxLinkRate = hal->getMaxLinkRate();
    NvU32       sinkUhbrCaps    = hal->getUHBRSupported();
    NvU32       uhbrCaps        = gpuUhbrCaps & sinkUhbrCaps;

    unsigned    maxLaneCount   = 4U;

    // For eDP, user can override maxLinkRate thru regkey, with valid value.
    if ((main->isEDP()) &&
        maxLinkRateFromRegkey && (IS_VALID_DP2_X_LINKBW(maxLinkRateFromRegkey)))
    {
        sinkMaxLinkRate = maxLinkRateFromRegkey;
    }

    if (FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_UHBR_SUPPORTED, _UHBR20_0, _YES, uhbrCaps))
    {
        maxLinkRate = dp2LinkRate_20_0Gbps;
    }
    else if (FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_UHBR_SUPPORTED, _UHBR13_5, _YES, uhbrCaps))
    {
        maxLinkRate = dp2LinkRate_13_5Gbps;
    }
    else if (FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_UHBR_SUPPORTED, _UHBR10_0, _YES, uhbrCaps))
    {
        maxLinkRate = dp2LinkRate_10_0Gbps;
    }
    else
    {
        maxLinkRate = sinkMaxLinkRate ? DP_MIN(sinkMaxLinkRate, gpuMaxLinkRate) : gpuMaxLinkRate;
    }

    if (hal->getMaxLaneCount())
    {
        maxLaneCount = DP_MIN(hal->getMaxLaneCountSupportedAtLinkRate(maxLinkRate),
                              hal->getMaxLaneCount());
    }

    // If sinkMaxLinkRate == 0 it means there is no real sink
    main->updateFallbackMap(maxLaneCount, maxLinkRate,
                            sinkMaxLinkRate ? uhbrCaps : gpuUhbrCaps);

    ConnectorImpl::populateAllDpConfigs();
}

/*!
 * @brief Find all the supported link rates.
 *
 * @return The list of supported link rates.
 */
LinkRates* ConnectorImpl2x::importDpLinkRates()
{
    LinkRate   linkRate;
    LinkRates  *pConnectorLinkRates = linkPolicy.getLinkRates();

    NvU32 gpuUhbrCaps      = main->getUHBRSupported();
    NvU32 sinkUhbrCaps     = hal->getUHBRSupported();
    NvU32 uhbrCaps         = (sinkUhbrCaps & gpuUhbrCaps);

    bool bUHBR10_0Supported = FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_UHBR_SUPPORTED,
                                                 _UHBR10_0, _YES, uhbrCaps);
    bool bUHBR13_5Supported = FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_UHBR_SUPPORTED,
                                                 _UHBR13_5, _YES, uhbrCaps);
    bool bUHBR20_0Supported = FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_UHBR_SUPPORTED,
                                                 _UHBR20_0, _YES, uhbrCaps);

    // Attempt to configure link rate table mode if supported
    if (hal->isIndexedLinkrateCapable() &&
        main->configureLinkRateTable(hal->getLinkRateTable(), pConnectorLinkRates))
    {
        // Maximal link rate is limited with link rate table
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

    if (this->bSupportUHBR2_50)
        pConnectorLinkRates->insert((NvU16)dp2LinkRate_2_50Gbps);

    if ((linkRate >= dp2LinkRate_2_70Gbps) || (this->bSupportUHBR2_70))
        pConnectorLinkRates->insert((NvU16)dp2LinkRate_2_70Gbps);

    if (linkRate >= dp2LinkRate_5_40Gbps)
        pConnectorLinkRates->insert((NvU16)dp2LinkRate_5_40Gbps);

    if (linkRate >= dp2LinkRate_8_10Gbps)
        pConnectorLinkRates->insert((NvU16)dp2LinkRate_8_10Gbps);

    if (this->bSupportUHBR5_00)
        pConnectorLinkRates->insert((NvU16)dp2LinkRate_5_00Gbps);

    if (bUHBR10_0Supported)
        pConnectorLinkRates->insert((NvU16)dp2LinkRate_10_0Gbps);

    if (bUHBR13_5Supported)
        pConnectorLinkRates->insert((NvU16)dp2LinkRate_13_5Gbps);

    if (bUHBR20_0Supported)
        pConnectorLinkRates->insert((NvU16)dp2LinkRate_20_0Gbps);

    return pConnectorLinkRates;
}

/*!
 * @brief   Find the max supported link configuration.
 *
 * @return  The max supported link configuration.
 */
LinkConfiguration ConnectorImpl2x::getMaxLinkConfig()
{
    DP_ASSERT(hal);

    LinkRate    maxLinkRate;
    unsigned    maxLaneCount   = 4U;

    // maxLinkRateSupported checks GPU and DFP caps. UHBR is included.
    LinkRate    gpuMaxLinkRate = main->maxLinkRateSupported();
    NvU32       gpuUhbrCaps    = main->getUHBRSupported();

    // sinkMaxLinkRate checks LTTPR and sink caps. UHBR is included.
    LinkRate    sinkMaxLinkRate = hal->getMaxLinkRate();
    NvU32       sinkUhbrCaps    = hal->getUHBRSupported();
    NvU32       uhbrCaps        = gpuUhbrCaps & sinkUhbrCaps;


    // For eDP, user can override maxLinkRate thru regkey, with valid value.
    if ((main->isEDP()) &&
        maxLinkRateFromRegkey && (IS_VALID_DP2_X_LINKBW(maxLinkRateFromRegkey)))
    {
        sinkMaxLinkRate = maxLinkRateFromRegkey;
    }

    maxLinkRate = sinkMaxLinkRate ? DP_MIN(sinkMaxLinkRate, gpuMaxLinkRate) : gpuMaxLinkRate;

    if (uhbrCaps)
    {
        if (FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_UHBR_SUPPORTED, _UHBR20_0, _YES, uhbrCaps))
        {
            maxLinkRate = dp2LinkRate_20_0Gbps;
        }
        else if (FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_UHBR_SUPPORTED, _UHBR13_5, _YES, uhbrCaps))
        {
            maxLinkRate = dp2LinkRate_13_5Gbps;
        }
        else if (FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_UHBR_SUPPORTED, _UHBR10_0, _YES, uhbrCaps))
        {
            maxLinkRate = dp2LinkRate_10_0Gbps;
        }
    }

    if (hal->getMaxLaneCount())
    {
        maxLaneCount = DP_MIN(hal->getMaxLaneCountSupportedAtLinkRate(maxLinkRate),
                              hal->getMaxLaneCount());
    }

    return LinkConfiguration (&this->linkPolicy,
                              maxLaneCount, maxLinkRate,
                              this->hal->getEnhancedFraming(),
                              linkUseMultistream(),
                              false,  /* disablePostLTRequest */
                              this->bFECEnable,
                              false,  /* bDisableLTTPR */
                              this->getDownspreadDisabled());
}

/*!
 * @brief   Check if the mode is possible for the current MST configuration.
 *          Note that 128b/132b is always MST with FEC enabled.
 *
 * @return  The mode info in the group is supported by current MST configuration
 */
bool ConnectorImpl2x::checkIsModePossibleMST(GroupImpl * targetGroup)
{
    if (!willLinkSupportMode(activeLinkConfig, targetGroup->lastModesetInfo, targetGroup->headIndex,
                             &targetGroup->timeslot.watermarks))
    {
        DP_ASSERT(!"DP2xCONN> DisplayDriver bug! This mode is not possible at "
                  "any link configuration. It should have been rejected at mode "
                  "filtering time!");
        return false;
    }

    return true;
}

bool ConnectorImpl2x::compoundQueryAttachMSTGeneric(Group * target,
                                                    const DpModesetParams &modesetParams,         // Modeset info
                                                    CompoundQueryAttachMSTInfo * localInfo,
                                                    DscParams *pDscParams,                        // DSC parameters
                                                    DP_IMP_ERROR *pErrorCode)
{
    if (!pDscParams || (pDscParams && !pDscParams->bEnableDsc))
    {
        NvU32 symbolSize = GET_SYMBOL_SIZE(activeLinkConfig.bIs128b132bChannelCoding);
        NvU32 hActive = localInfo->localModesetInfo.surfaceWidth;
        NvU32 bpp = localInfo->localModesetInfo.depth;

        NvU32 bitsPerLane                   = (NvU32)NV_CEIL(hActive, LOGICAL_LANES) * bpp;
        NvU32 totalSymbolsPerLane           = (NvU32)NV_CEIL(bitsPerLane, symbolSize);
        NvU32 totalSymbols                  = totalSymbolsPerLane * LOGICAL_LANES;
        localInfo->localModesetInfo.depth   = (NvU32)NV_CEIL((totalSymbols * symbolSize * EFF_BPP_NON_DSC_SCALER), hActive);
    }
    else
    {
        //
        // If DSC is enabled bpp will already be multiplied by 16, we need to mulitply by another 16 
        // to match scalar of 256 which is used in non-DSC case. 
        //
        localInfo->localModesetInfo.depth = localInfo->localModesetInfo.depth * EFF_BPP_DSC_SCALER;
    }

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
        return false;
    }

    //      Verify the min blanking, etc. No headIndex (default 0) for mode enumeration.
    if (!willLinkSupportMode(localInfo->lc, localInfo->localModesetInfo))
    {
        compoundQueryResult = false;
        SET_DP_IMP_ERROR(pErrorCode, DP_IMP_ERROR_WATERMARK_BLANKING)
        return false;
    }

    if (!hal->isDp2xChannelCodingCapable())
    {
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

                    if (tail->bandwidth.compound_query_state.timeslots_used_by_query >
                        tail->bandwidth.compound_query_state.totalTimeSlots)
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
    }
    else
    {
        for(Device * d = target->enumDevices(0); d; d = target->enumDevices(d))
        {
            DeviceImpl * tail = (DeviceImpl *)d;
            if (!(tail->bandwidth.compound_query_state.bandwidthAllocatedForIndex & (1 << compoundQueryCount)))
            {
                tail->bandwidth.compound_query_state.bandwidthAllocatedForIndex |= (1 << compoundQueryCount);
                tail->inferPathConstraints();

                if (slots_pbn > DP_MIN(tail->bandwidth.enum_path.total, tail->bandwidth.enum_path.dfpLinkAvailable))
                {
                    compoundQueryResult = false;
                    SET_DP_IMP_ERROR(pErrorCode, DP_IMP_ERROR_INSUFFICIENT_BANDWIDTH)
                }
            }
        }
    }

    return compoundQueryResult;
}

/*!
 * @brief   Notify library before/after modeset (update)
 * Here is what NAB essentially does:
 *   0. Makes sure TMDS is not attached
 *   1. Trains link to optimized link config ("optimized" depends on DP1.1, DP1.2)
 *   2. Performs quick watermark check for IMP. If IMP is not possible, forces link, zombies devices
 *   3. if anything of above fails, marks devices in given group as zombies
 *
 * Update to ConnectorImpl2x version:
 *   1. Drop all single head multi stream (SST/MST).
 *   2. Drop 2H1OR
 *
 * @return  true  - NAB passed
 *          false - NAB failed due to invalid params or link training failure
 *                  Link configs are forced in case of link training failure
 */
bool ConnectorImpl2x::notifyAttachBegin(Group *target, const DpModesetParams &modesetParams)
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
    bool       bEnableFEC                = true;
    bool       bEnableDsc                = modesetParams.modesetInfo.bEnableDsc;
    bool       bEnablePassThroughForPCON = modesetParams.modesetInfo.bEnablePassThroughForPCON;
    Device     *newDev                   = target->enumDevices(0);
    DeviceImpl *dev                      = (DeviceImpl *)newDev;

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

    DP_PRINTF(DP_NOTICE, "DP2xCONN> Notify Attach Begin (Head %d, pclk %d (KHz) raster %d x %d  %d bpp)",
              modesetParams.headIndex, (pixelClockHz/1000), rasterWidth, rasterHeight, depth);
    NV_DPTRACE_INFO(NOTIFY_ATTACH_BEGIN, modesetParams.headIndex, pixelClockHz, rasterWidth, rasterHeight,
                    depth, bEnableDsc, bEnableFEC);

    if (!depth || !pixelClockHz)
    {
        DP_ASSERT(!"DP2xCONN> Params with zero value passed to query!");
        return false;
    }

    for (Device * dev = target->enumDevices(0); dev; dev = target->enumDevices(dev))
    {
        Address::StringBuffer buffer;
        DP_USED(buffer);
        DP_PRINTF(DP_NOTICE, "DP2xCONN>   | %s (%s) |", dev->getTopologyAddress().toString(buffer),
                  dev->isVideoSink() ? "VIDEO" : "BRANCH");
    }

    if (firmwareGroup && ((GroupImpl *)firmwareGroup)->headInFirmware)
    {
        DP_ASSERT(bIsUefiSystem || (!"DP2xCONN> Firmware still active on head. De-activating"));
    }

    GroupImpl* targetImpl = (GroupImpl*)target;

    if (bEnableDsc)
    {
        DP_PRINTF(DP_NOTICE, "DP2xCONN> DSC Mode = %s", (modesetParams.modesetInfo.mode == DSC_SINGLE) ? "SINGLE" : "DUAL");
        targetImpl->dscModeRequest = modesetParams.modesetInfo.mode;
    }

    DP_ASSERT(!(targetImpl->isHeadAttached() && targetImpl->bIsHeadShutdownNeeded) &&
              "DP2xCONN> Head should have been shut down but it is still active!");

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

    if (this->linkGuessed)
    {
        DP_ASSERT(!"Link was not assessed previously. Probable reason: system was not in driver mode. Assessing now");
        this->assessLink();
    }

    DP_ASSERT(this->isLinkQuiesced == 0 && "DP2xCONN> According to bracketting calls TMDS/alternate DP still active!");

    // Transfer the group to active list
    inactiveGroups.remove(targetImpl);
    activeGroups.insertBack(targetImpl);

    if (modesetParams.colorFormat == dpColorFormat_YCbCr422 &&
        dev && dev->dscCaps.dscDecoderColorFormatCaps.bYCbCrNative422)
    {
        targetImpl->lastModesetInfo = ModesetInfo(twoChannelAudioHz, eightChannelAudioHz,
                                      pixelClockHz, rasterWidth, rasterHeight,
                                      (rasterBlankStartX - rasterBlankEndX), modesetParams.modesetInfo.surfaceHeight,
                                      depth, rasterBlankStartX, rasterBlankEndX, bEnableDsc, modesetParams.modesetInfo.mode,
                                      false, modesetParams.colorFormat);
    }
    else
    {
        targetImpl->lastModesetInfo = ModesetInfo(twoChannelAudioHz, eightChannelAudioHz,
                                      pixelClockHz, rasterWidth, rasterHeight,
                                      (rasterBlankStartX - rasterBlankEndX), modesetParams.modesetInfo.surfaceHeight,
                                      depth, rasterBlankStartX, rasterBlankEndX, bEnableDsc, modesetParams.modesetInfo.mode);
    }

    targetImpl->headIndex = modesetParams.headIndex;
    targetImpl->streamIndex = main->headToStream(modesetParams.headIndex, (messageManager != NULL), targetImpl->singleHeadMultiStreamID);
    targetImpl->colorFormat = modesetParams.colorFormat;

    DP_ASSERT(!this->isLinkQuiesced && "DP2xCONN> TMDS is attached, NABegin is impossible!");

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
            DP_ASSERT(!"DP2xCONN> eDP Source OUI is not cached!");
        }
    }

    // if failed, we're guaranteed that assessed link rate didn't meet the mode requirements
    // isZombie() will catch this
    bLinkTrainingStatus = trainLinkOptimized(getMaxLinkConfig());

    // if LT is successful, see if panel supports DSC and if so, set DSC enabled/disabled
    // according to the mode requested.
    if(bLinkTrainingStatus)
    {
        for (Device * dev = target->enumDevices(0); dev; dev = target->enumDevices(dev))
        {
            if (bPConConnected)
            {
                if (!(((DeviceImpl *)dev)->setDscEnableDPToHDMIPCON(bEnableDsc, bEnablePassThroughForPCON)))
                {
                    DP_ASSERT(!"DP2xCONN> Failed to configure DSC on DP to HDMI PCON!");
                }
            }
            else if(!setDeviceDscState(dev, bEnableDsc))
            {
                DP_ASSERT(!"DP2xCONN> Failed to configure DSC on Sink!");
            }
        }
    }

    if (linkUseMultistream())
    {
        unsigned symbolSize = GET_SYMBOL_SIZE(activeLinkConfig.bIs128b132bChannelCoding);

        if (bEnableDsc)
        {
            targetImpl->lastModesetInfo.depth *= EFF_BPP_DSC_SCALER;
        }
        else
        {
            NvU32 bitsPerLane                   = (NvU32)NV_CEIL(modesetParams.modesetInfo.surfaceWidth, LOGICAL_LANES) * depth;
            NvU32 totalSymbolsPerLane           = (NvU32)NV_CEIL(bitsPerLane, symbolSize);
            NvU32 totalSymbols                  = totalSymbolsPerLane * LOGICAL_LANES;
            targetImpl->lastModesetInfo.depth   = (NvU32)NV_CEIL((totalSymbols * symbolSize * EFF_BPP_NON_DSC_SCALER), 
                                                                  modesetParams.modesetInfo.surfaceWidth);
        }
    }

    beforeAddStream(targetImpl);

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

    // Move the group to intransistion since we are at the end of notifyAttachBegin
    intransitionGroups.insertFront(targetImpl);
    if (dev && dev->bApplyPclkWarBug4949066 == true)
    {
        EvoInterface   *provider = ((EvoMainLink *)main)->getProvider();
        NV0073_CTRL_CMD_DP_SET_PROP_FORCE_PCLK_FACTOR_PARAMS params = {0};
        params.subDeviceInstance = provider->getSubdeviceIndex();
        params.head = modesetParams.headIndex;
        params.bEnable = NV_TRUE;

        NvU32 ret = provider->rmControl0073(NV0073_CTRL_CMD_DP_SET_PROP_FORCE_PCLK_FACTOR, &params, sizeof(params));
        if (ret != NVOS_STATUS_SUCCESS)
        {
            DP_PRINTF(DP_ERROR, "Failed to enable the WAR for bug4949066!");
        }
    }
    bFromResumeToNAB = false;
    return bLinkTrainingStatus;
}

/*!
 * @brief   End of notify attach.
 *
 * modesetCancelled True, when DD respected NAB failure and cancelled modeset.
 *                  False, when NAB succeeded, or DD didn't honor NAB failure
 *
 * Here is what NAE supposed to do:
 * 1. modesetCancelled == TRUE, NAB failed:
 *         unzombie all devices and set linkForced to false; We have Status Quo for next modeset
 * 2. modesetCancelled == False, NAB failed:
 *        If NAB failed, linkForces is TRUE. NAE goes finds zombied devices and notifies DD about them.
 * 3. modesetCancelled == False, NAB succeeded:
 *        NAE is no-op. (but we have some special sanity code)
*/
void ConnectorImpl2x::notifyAttachEnd(bool modesetCancelled)
{
    if (!activeLinkConfig.bIs128b132bChannelCoding)
        return ConnectorImpl::notifyAttachEnd(modesetCancelled);

    GroupImpl* currentModesetDeviceGroup = NULL;
    DP_PRINTF(DP_NOTICE, "DP2xCONN> Notify Attach End for 128b/132b channel coding");
    NV_DPTRACE_INFO(NOTIFY_ATTACH_END);

    bFromResumeToNAB = false;

    if (intransitionGroups.isEmpty())
    {
        DP_ASSERT(!"DP2xCONN> INVALID STATE: Modeset Group is NULL");
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

/*!
 * @brief   Calculate and allocate time slot required for the mode.
 *          Calling ConnectorImpl version which handles bandwidth calculation.
 *          For 128b/132b channel coding and SSTopology, need to assign all 64 slots
 *          to the stream and make sure starting from slot 0.
 *
 * @return  If the allocation is passing.
 */
bool ConnectorImpl2x::allocateTimeslice(GroupImpl * targetGroup)
{
    bool result = true;
    if (activeLinkConfig.bIs128b132bChannelCoding)
    {
        firstFreeSlot = 0;
    }
    result = ConnectorImpl::allocateTimeslice(targetGroup);

    // If result is false means the mode is not supported.
    if (!result)
    {
        return false;
    }

    if (!activeLinkConfig.bIs128b132bChannelCoding || linkUseMultistream()) {
        goto done;
    }

    // 128b/132b SSTology takes all (64) slots.
    targetGroup->timeslot.count = maximumSlots;
    targetGroup->timeslot.begin = 0U;

    // no freeSlots.
    freeSlots = 0U;

done:
    targetGroup->setTimeslotAllocated(true);
    return result;
}

/*!
 * @brief Setup GPU before stream is added to connector.
 *
 * @param[in]   group           The stream needs to be attached.
 * @param[in]   test            If it's called to test HW.
 * @param[in]   forFlushMode    If it's called for flush mode.
 *
 * @return      If the stream is forced to add.
 */
bool ConnectorImpl2x::beforeAddStream(GroupImpl * group, bool test, bool forFlushMode)
{
    bool bForced = false;

    if (group->isTimeslotAllocated())
        return bForced;

    if (!activeLinkConfig.bIs128b132bChannelCoding)
    {
        // Handle NVD5 8b/10b modeset.
        NvBool      bIsModePossible = false;
        Watermark   water = {0};

        if (linkUseMultistream())
        {
            // MST - reuse DP1x code path.
            return ConnectorImpl::beforeAddStreamMST(group, test, forFlushMode);
        }
        // SST call into DP2x code path.
        bIsModePossible = willLinkSupportMode(activeLinkConfig, group->lastModesetInfo,
                                              group->headIndex, &water);
        if (bIsModePossible || test)
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
                                        false /*bEnable2Head1Or*/);
        }
        if (!bIsModePossible)
        {
            if (test)
            {
                bForced = true;
                DP_PRINTF(DP_ERROR, "DP2x-TS> Unable to allocate stream. Setting RG_DIV mode");
            }
            else
                DP_ASSERT(0);
        }
        return bForced;
    }

    // 128b/132b only has MST mode.
    if (allocateTimeslice(group))
    {
        flushTimeslotsToHardware();
        group->setTimeslotAllocated(true);

        if (forFlushMode || addStreamMSTIntransitionGroups.isEmpty())
        {
            hal->payloadTableClearACT();
            hal->payloadAllocate(group->streamIndex, group->timeslot.begin, group->timeslot.count);
            main->triggerACT();
        }
        else
        {
            group->bDeferredPayloadAlloc = true;
        }
        addStreamMSTIntransitionGroups.insertFront(group);
    }
    else
    {
        if (!test)
        {
            DP_PRINTF(DP_ERROR, "DP2x-TS> Unable to allocate stream.  Should call mainLink->configureStream to trigger RG_DIV mode");
            main->configureMultiStream(group->headIndex,
                group->timeslot.watermarks.hBlankSym, group->timeslot.watermarks.vBlankSym,
                1, 0, 0, 0, group->colorFormat, group->singleHeadMultiStreamID,
                group->singleHeadMultiStreamMode, bAudioOverRightPanel);
        }
        else
        {
            flushTimeslotsToHardware();

            if (forFlushMode)
            {
                hal->payloadTableClearACT();
                hal->payloadAllocate(group->streamIndex, group->timeslot.begin, group->timeslot.count);
                main->triggerACT();
            }

            DP_PRINTF(DP_ERROR, "DP2x-TS> Unable to allocate stream. Setting RG_DIV mode");
            bForced = true;
        }
    }

    return bForced;
}

/*!
 * @brief Setup sink and SW state after stream is added to connector.
 *
 * @param[in]   group       The stream needs to be attached.
 *
 */
void ConnectorImpl2x::afterAddStream(GroupImpl * group)
{
    if (!activeLinkConfig.bIs128b132bChannelCoding)
        return ConnectorImpl::afterAddStream(group);

    // For 128b/132b always use MST protocol
    DP_ASSERT(group->isTimeslotAllocated());

    // Skip this as there is no timeslot allocation
    if (!group->timeslot.count)
        return;

    if (group->bDeferredPayloadAlloc)
    {
        DP_ASSERT(addStreamMSTIntransitionGroups.contains(group));
        hal->payloadTableClearACT();
        hal->payloadAllocate(group->streamIndex, group->timeslot.begin, group->timeslot.count);
        main->triggerACT();
    }
    group->bDeferredPayloadAlloc = false;

    if (addStreamMSTIntransitionGroups.contains(group))
    {
        addStreamMSTIntransitionGroups.remove(group);
    }

    if (!hal->payloadWaitForACTReceived())
    {
        DP_ASSERT(!"DP2x-TS> ACT has not been received.Triggering ACT once more");

        //
        // Bug 1334070: During modeset for cloned displays on certain GPU family,
        // ACT triggered during SOR attach is not being received due to timing issues.
        // Also DP1.2 spec mentions that there is no harm in sending the ACT
        // again if there is no change in payload table. Hence triggering ACT once more here
        //
        main->triggerACT();
        if (!hal->payloadWaitForACTReceived())
        {
            DP_PRINTF(DP_ERROR, "DP2x-TS> Downstream device did not receive ACT during stream re-add.");
            return;
        }
    }

    for (Device * d = group->enumDevices(0); d; d = group->enumDevices(d))
    {
        group->update((DeviceImpl *)d, true);
    }
}

/*!
 * @brief Setup GPU before stream is deleted from the connector.
 *
 * @param[in]   group           The stream needs to be attached.
 * @param[in]   forFlushMode    If it's called for flush mode.
 *
 */
void ConnectorImpl2x::beforeDeleteStream(GroupImpl * group, bool forFlushMode)
{
    if (!group->isTimeslotAllocated() && !group->headInFirmware)
        return;

    if (!activeLinkConfig.bIs128b132bChannelCoding)
        return ConnectorImpl::beforeDeleteStream(group, forFlushMode);

    // check if this is a firmware group
    if (group && group->isHeadAttached() && group->headInFirmware)
    {
        // Firmware group can be assumed to be taking up all 63 slots.
        group->timeslot.begin = 1U;
        group->timeslot.count = 63U;
        this->freeSlots = 0U;

        // Clear the payload table
        hal->payloadTableClearACT();
        if (!hal->payloadAllocate(0, 0, 63))
        {
            DP_PRINTF(DP_ERROR, "DP2xCONN> Payload table could not be cleared");
        }
        // Clear gpu timeslots.
        freeTimeslice(group);
        flushTimeslotsToHardware();

        group->setTimeslotAllocated(false);

        main->triggerACT();
        group->bWaitForDeAllocACT = false;

        return;
    }

    if (group && group->isHeadAttached() && group->isTimeslotAllocated())
    {
        // Detach all the panels from payload
        for (Device * d = group->enumDevices(0); d; d = group->enumDevices(d))
        {
            group->update(d, false);
        }

        freeTimeslice(group);
        flushTimeslotsToHardware();
        group->setTimeslotAllocated(false);
        group->bWaitForDeAllocACT = true;

        // Delete the stream
        hal->payloadTableClearACT();
        hal->payloadAllocate(group->streamIndex, group->timeslot.begin, 0);
        main->triggerACT();
    }
}

/*!
 * @brief Setup GPU/Sink after stream is deleted from the connector.
 *
 * @param[in]   group           The stream needs to be attached.
 *
 */
void ConnectorImpl2x::afterDeleteStream(GroupImpl * group)
{
    if (!activeLinkConfig.bIs128b132bChannelCoding)
        return ConnectorImpl::afterDeleteStream(group);

    DP_ASSERT(!group->isTimeslotAllocated());

    if (group->isHeadAttached() && group->bWaitForDeAllocACT)
    {
        if (!hal->payloadWaitForACTReceived())
        {
            DP_ASSERT(!"DP2xCONN> Delete stream failed.  Device did not acknowledge stream deletion ACT!");
        }
    }
}

/*!
 * @brief Wrapper for link train function. Calls ConnectorImpl::train and update
 *        maximumSlots if 128b/132b channel coding is used.
 *
 * @param[in]   lConfig             Requested link configuration.
 * @param[in]   force               If the request is forced (fake LT).
 * @param[in]   trainType           NormalLinkTrain, NoLinkTrain or FastLinkTrain
 *
 * @return      If link training is done and result is the same as requested.
 */
bool ConnectorImpl2x::train(const LinkConfiguration &lConfig, bool force, LinkTrainingType trainType)
{
    bool trainResult = ConnectorImpl::train(lConfig, force, trainType);
    if (activeLinkConfig.lanes != 0 &&
        activeLinkConfig.bIs128b132bChannelCoding)
    {
        // For 128b/132b, all 64 timeslots are available to use.
        maximumSlots = 64;
        freeSlots = maximumSlots;
        firstFreeSlot = 0;
    }

    // Invalidate the UHBR if the connector is a USB-C to DP/USB-C
    // and VCONN source is unknown.
    if (!trainResult && main->isConnectorUSBTypeC() &&
        lConfig.bIs128b132bChannelCoding && lConfig.peakRate > dp2LinkRate_10_0Gbps &&
        main->isCableVconnSourceUnknown())
    {
        hal->overrideCableIdCap(lConfig.peakRate, false);
    }
    return trainResult;
}

/*!
 * @brief Wrapper for notifyDetachBegin.
 *        Setup HDCP before calls ConnectorImpl::notifyDetachBegin for 128b/132b SSTopology
 *
 * @param[in]   target             The stream to be attached.
 */
void ConnectorImpl2x::notifyDetachBegin(Group *target)
{
    if (!target)
        target = firmwareGroup;

    Device     *newDev  = target->enumDevices(0);
    DeviceImpl *dev     = (DeviceImpl *)newDev;
    GroupImpl  *group   = (GroupImpl*)target;

    if (dev != NULL && dev->bApplyPclkWarBug4949066 == true)
    {
        EvoInterface   *provider = ((EvoMainLink *)main)->getProvider();
        NV0073_CTRL_CMD_DP_SET_PROP_FORCE_PCLK_FACTOR_PARAMS params = {0};
        params.subDeviceInstance = provider->getSubdeviceIndex();
        params.head = group->headIndex;
        params.bEnable = NV_FALSE;

        NvU32 ret = provider->rmControl0073(NV0073_CTRL_CMD_DP_SET_PROP_FORCE_PCLK_FACTOR, &params, sizeof(params));
        if (ret != NVOS_STATUS_SUCCESS)
        {
            DP_PRINTF(DP_ERROR, "Failed to Disable the WAR for bug4949066!");
        }
    }
    return ConnectorImpl::notifyDetachBegin(target);
}

void ConnectorImpl2x::notifyDetachEnd(bool bKeepOdAlive)
{
    if (!activeLinkConfig.bIs128b132bChannelCoding)
        return ConnectorImpl::notifyDetachEnd(bKeepOdAlive);

    GroupImpl* currentModesetDeviceGroup = NULL;
    DP_PRINTF(DP_NOTICE, "DP2xCONN> Notify detach end");
    NV_DPTRACE_INFO(NOTIFY_DETACH_END);

    if (intransitionGroups.isEmpty())
    {
        DP_ASSERT(!"DP2xCONN> INVALID STATE: Modeset Group is NULL");
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

    // Reset value of bIsHeadShutdownNeeded to get rid of false asserts
    currentModesetDeviceGroup->bIsHeadShutdownNeeded = false;

    // If this is eDP and the LCD power is not ON, we don't need to Disable DSC here
    bool bPanelPwrSts = true;
    if ((!main->isEDP()) || (main->getEdpPowerData(&bPanelPwrSts, NULL) && bPanelPwrSts))
    {
        // Disable DSC decompression on the panel if panel supports DSC and reset bFECEnable Flag
        for (Device * dev = currentModesetDeviceGroup->enumDevices(0); dev;
                      dev = currentModesetDeviceGroup->enumDevices(dev))
        {
            if(!(setDeviceDscState(dev, false/*bEnableDsc*/)))
            {
                DP_ASSERT(!"DP2xCONN> Failed to configure DSC on Sink!");
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

/*!
 * @brief Engage flush mode in the HW
 *
 * @return      boolean to indicate success/failure
 *
 */

bool ConnectorImpl2x::enableFlush()
{
    // Fallback to legacy SF Flush sequence
    if (!main->isRgFlushSequenceUsed())
    {
        return ConnectorImpl::enableFlush();
    }

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
            DP_PRINTF(DP_ERROR, "DP2xCONN> SST-Flush mode should not be called when head is not attached. Returning early without enabling flush");
            bFlushSkipped = true;
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
            DP_PRINTF(DP_ERROR, "DP2xCONN> MST-Flush mode should not be called when head is not attached. Returning early without enabling flush");
            bFlushSkipped = true;
            return true;
        }
    }

    if (!main->setFlushMode(FlushModePhase1))
    {
        DP_PRINTF(DP_ERROR, "DP2xCONN> Set flush mode phase 1 failed\n");
        return false;
    }

    //
    // Enabling flush mode shuts down the link, so the next link training
    // call must not skip programming the hardware.  Otherwise, EVO will
    // hang if the head is still active when flush mode is disabled.
    //

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

        main->preLinkTraining(g->headIndex);

        beforeDeleteStream(g, true);

        if (this->linkUseMultistream() || activeLinkConfig.bIs128b132bChannelCoding)
        {
            main->triggerACT();
        }

        afterDeleteStream(g);
    }

    if (!main->setFlushMode(FlushModePhase2))
    {
        DP_PRINTF(DP_ERROR, "DP2xCONN> Set flush mode phase 2 failed\n");
        return false;
    }

    // Reset activeLinkConfig to indicate the link is now lost
    activeLinkConfig = LinkConfiguration();

    return true;
}

/*!
 * @brief Disengage flush mode in HW
 *
 * @param[in]   test             Indicates whether force disable even on failure
 *
 */

void ConnectorImpl2x::disableFlush(bool test)
{
    // Fallback to legacy SF Flush sequence
    if (!main->isRgFlushSequenceUsed())
    {
        return ConnectorImpl::disableFlush();
    }

    bool bHeadAttached         = false;
    NvU32 attachFailedHeadMask = 0;

    if (activeGroups.isEmpty())
        return;

    if (bFlushSkipped)
    {
        bFlushSkipped = false;
        return;
    }

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
            DP_PRINTF(DP_ERROR, "DP2xCONN> SST-Flush mode disable should not be called when head is not attached. Returning early without disabling flush\n");
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
            DP_PRINTF(DP_ERROR, "DP2xCONN> MST-Flush mode disable should not be called when head is not attached. Returning early without disabling flush\n");
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

        if (!g->isHeadAttached() && this->linkUseMultistream())
            continue;

        if (!main->clearFlushMode(FlushModePhase1, false, g->headIndex))
        {
            DP_PRINTF(DP_ERROR, "DP2xCONN> Clear flush mode phase 1 failed for headIndex: %d\n", g->headIndex);
        }

        // Allocate the timeslot configuration
        if (beforeAddStream(g, test, true))
        {
            attachFailedHeadMask |= 1 << g->headIndex;
        }

        main->postLinkTraining(g->headIndex);

        afterAddStream(g);       // ACT is triggered here
    }

    if (!main->clearFlushMode(FlushModePhase2, attachFailedHeadMask))
    {
        DP_PRINTF(DP_ERROR, "DP2xCONN> Clear flush mode phase 2 failed\n");
        return;
    }

    return;
}

static bool
_IsDP2xLinkQualityPattern
(
    LinkQualityPatternType lqsPattern
)
{
    switch (lqsPattern)
    {
        case LINK_QUAL_128B132B_TPS1:
        case LINK_QUAL_128B132B_TPS2:
        case LINK_QUAL_PRBS9:
        case LINK_QUAL_PRBS11:
        case LINK_QUAL_PRBS15:
        case LINK_QUAL_PRBS23:
        case LINK_QUAL_PRBS31:
        case LINK_QUAL_264BIT_CUST:
        case LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_ON_DE_EMPHASIS_ON:
        case LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_OFF_DE_EMPHASIS_ON:
        case LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_ON_DE_EMPHASIS_OFF:
        case LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_OFF_DE_EMPHASIS_OFF:
            return true;
        default:
            return false;
    }
    return false;
}

bool ConnectorImpl2x::handlePhyPatternRequest()
{
    bool                status      = true;
    DP2xPatternInfo     patternInfo = {};

    patternInfo.lqsPattern = hal->getPhyTestPattern();

    if (!_IsDP2xLinkQualityPattern(patternInfo.lqsPattern))
    {
        return ConnectorImpl::handlePhyPatternRequest();
    }

    // Get lane count from most current link training
    unsigned requestedLanes = this->activeLinkConfig.lanes;

    if (patternInfo.lqsPattern == LINK_QUAL_264BIT_CUST)
    {
        hal->get264BitsCustomTestPattern((NvU8 *)&patternInfo.ctsmData[0]);
    }
    else if ((patternInfo.lqsPattern == LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_ON_DE_EMPHASIS_ON) ||
             (patternInfo.lqsPattern == LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_OFF_DE_EMPHASIS_ON) ||
             (patternInfo.lqsPattern == LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_ON_DE_EMPHASIS_OFF) ||
             (patternInfo.lqsPattern == LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_OFF_DE_EMPHASIS_OFF))
    {
        hal->getSquarePatternNum(&(patternInfo.sqNum));
    }

    // send control call to rm for the pattern
    if (!main->physicalLayerSetDP2xTestPattern(&patternInfo))
    {
        DP_ASSERT(0 && "Could not set the PHY_TEST_PATTERN");
        status = false;
    }
    else
    {
        if (AuxRetry::ack != hal->setLinkQualPatternSet(patternInfo, requestedLanes))
        {
            DP_ASSERT(0 && "Could not set the LINK_QUAL_PATTERN");
            status = false;
        }
    }
    return status;
}

bool ConnectorImpl2x::handleTestLinkTrainRequest()
{
    if (!activeLinkConfig.multistream)
    {
        // route to 1.x funtion if SST
        return ConnectorImpl::handleTestLinkTrainRequest();
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

            bool bHeadAttached = false;
            GroupImpl *groupAttached = NULL;
            for (ListElement * e = activeGroups.begin(); e != activeGroups.end(); e = e->next)
            {
                groupAttached = (GroupImpl *)e;
                if (groupAttached->isHeadAttached())
                {
                    bHeadAttached  = true;
                    break;
                }
            }

            if (groupAttached == NULL)
            {
                DP_ASSERT(0 && "Compliance: no group attached");
            }

            DP_PRINTF(DP_NOTICE, "DP> Compliance: LT on IRQ request: 0x%x, %d.", requestedRate, requestedLanes);
            // now see whether the current resolution is supported on the requested link config
            LinkConfiguration lc(&linkPolicy, requestedLanes, requestedRate, hal->getEnhancedFraming(),
                                 false, // MST
                                 false, // disablePostLTRequest
                                 false, // bEnableFEC
                                 false, // bDisableLTTPR
                                 this->getDownspreadDisabled());

            if (bHeadAttached)
            {
                if (willLinkSupportMode(lc, groupAttached->lastModesetInfo, groupAttached->headIndex, NULL, NULL))
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
                else // linkconfig is not supporting bandwidth. Simply return NACK
                {
                    DP_PRINTF(DP_ERROR, "DP> Compliance: IMP failed with requested link configuration: 0x%x, %d.",
                              requestedRate, requestedLanes);
                    hal->setTestResponse(false);
                    return false;
                }
            }
            else
            {
                DP_PRINTF(DP_NOTICE, "DP> Compliance: Link Training when the head is not attached.");
                hal->setTestResponse(true);
                if (!train(lc, false))
                    train(lc, true);
                return true;
            }
        }
    }
}

ConnectorImpl2x::ConnectorImpl2x(MainLink * main, AuxBus * auxBus, Timer * timer, Connector::EventSink * sink)
    : ConnectorImpl(main, auxBus, timer, sink)
{
    bFlushSkipped = false;
    bDisableDownspread  = false;
    applyDP2xRegkeyOverrides();
}

bool ConnectorImpl2x::getDp2xLaneConfig(NvU32 *numLanes, NvU32 *data)
{
    return (main->getDp2xLaneData(numLanes, data));
}

bool ConnectorImpl2x::setDp2xLaneConfig(NvU32 numLanes, NvU32 *data)
{
    return (main->setDp2xLaneData(numLanes, data));
}

void ConnectorImpl2x::configInit()
{
    ConnectorImpl::configInit();
    bMstTimeslotBug4968411 = false;
    bApplyManualTimeslotBug4968411 = false;
    bDisableDownspread              = false;

    applyDP2xRegkeyOverrides();
}

void ConnectorImpl2x::applyTimeslotWAR(unsigned &slot_count)
{
    if (bApplyManualTimeslotBug4968411)
    {
        slot_count += 1;
    }
}

void ConnectorImpl2x::handleEdidWARs(Edid & edid, DiscoveryManager::Device & device)
{
    // handle DP1.x edid WARs
    ConnectorImpl::handleEdidWARs(edid, device);

    if (edid.WARFlags.bAllocateManualTimeslots)
    {
        if ((device.address.size() == 2) && bMstTimeslotBug4968411)
        {
            // Samsung G9 Monitor is behind internal branch, allocate one more timeslot
            bApplyManualTimeslotBug4968411 = true;
        }
    }

    if (edid.WARFlags.bForceHeadShutdown)
    {
        // SST mode
        if (device.address.size() <= 1)
        {
            bForceHeadShutdownPerMonitor = true;
        }
    }

    if (edid.WARFlags.bDP2XPreferNonDSCForLowPClk)
    {
        bDP2XPreferNonDSCForLowPClk = true;
    }

    if (edid.WARFlags.bDisableDscMaxBppLimit)
    {
        bDisableDscMaxBppLimit = true;
    }

    if (edid.WARFlags.bForceHeadShutdownOnModeTransition)
    {
        bForceHeadShutdownOnModeTransition = true;
    }
    if (edid.WARFlags.bDisableDownspread)
    {
        setDisableDownspread(true);
    }
}
