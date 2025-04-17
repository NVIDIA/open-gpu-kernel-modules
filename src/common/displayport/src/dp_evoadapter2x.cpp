/*
 * SPDX-FileCopyrightText: Copyright (c) 2023-2025 NVIDIA CORPORATION & AFFILIATES.
 * All rights reserved.
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
* Module: dp_evoadapter2x.cpp                                               *
*    Interface for low level access to the aux bus for DP2x.                *
*    This is the synchronous version of the interface.                      *
*                                                                           *
\***************************************************************************/


#include "dp_evoadapter2x.h"
#include "dp_regkeydatabase.h"
#include "dp_printf.h"
#include "displayport2x.h"
#include "ctrl/ctrlc370/ctrlc370or.h"

using namespace DisplayPort;

//
// The entries are in the order of fallback sequence defined in DP2.1 spec, Table 3-31
// Note the table below also includes ILR rates.
// Sorting the entries based on the same logic of Table 3-31.
//
static const SIMPLIFIED_DP2X_LINKCONFIG defaultFallbackMandateTable[] =
{
    // {lane count, link rate, bSupported, bUseDP2xChannelCoding}
    {4, dp2LinkRate_20_0Gbps, NV_TRUE,  NV_TRUE},   {4, dp2LinkRate_13_5Gbps, NV_TRUE, NV_TRUE},
    {2, dp2LinkRate_20_0Gbps, NV_TRUE,  NV_TRUE},   {4, dp2LinkRate_10_0Gbps, NV_TRUE, NV_TRUE},
    {2, dp2LinkRate_13_5Gbps, NV_TRUE,  NV_TRUE},   {4, dp2LinkRate_8_10Gbps, NV_TRUE, NV_FALSE},
    {4, dp2LinkRate_6_75Gbps, NV_FALSE, NV_FALSE},  {1, dp2LinkRate_20_0Gbps, NV_TRUE, NV_TRUE},
    {2, dp2LinkRate_10_0Gbps, NV_TRUE,  NV_TRUE},   {4, dp2LinkRate_5_00Gbps, NV_TRUE, NV_TRUE},
    {4, dp2LinkRate_5_40Gbps, NV_TRUE,  NV_FALSE},  {4, dp2LinkRate_4_32Gbps, NV_FALSE, NV_FALSE},
    {1, dp2LinkRate_13_5Gbps, NV_TRUE,  NV_TRUE},   {2, dp2LinkRate_8_10Gbps, NV_TRUE, NV_FALSE},
    {2, dp2LinkRate_6_75Gbps, NV_FALSE, NV_FALSE},  {4, dp2LinkRate_2_70Gbps, NV_TRUE, NV_TRUE},
    {4, dp2LinkRate_3_24Gbps, NV_FALSE, NV_FALSE},  {1, dp2LinkRate_10_0Gbps, NV_TRUE, NV_TRUE},
    {2, dp2LinkRate_5_00Gbps, NV_TRUE,  NV_TRUE},   {4, dp2LinkRate_2_50Gbps, NV_TRUE, NV_TRUE},
    {2, dp2LinkRate_5_40Gbps, NV_TRUE,  NV_FALSE},  {4, dp2LinkRate_2_70Gbps, NV_TRUE, NV_FALSE},
    {4, dp2LinkRate_2_43Gbps, NV_FALSE, NV_FALSE},  {2, dp2LinkRate_4_32Gbps, NV_FALSE, NV_FALSE},
    {4, dp2LinkRate_2_16Gbps, NV_FALSE, NV_FALSE},  {1, dp2LinkRate_8_10Gbps, NV_TRUE, NV_FALSE},
    {1, dp2LinkRate_6_75Gbps, NV_FALSE, NV_FALSE},  {2, dp2LinkRate_2_70Gbps, NV_TRUE, NV_TRUE},
    {2, dp2LinkRate_3_24Gbps, NV_FALSE, NV_FALSE},  {4, dp2LinkRate_1_62Gbps, NV_TRUE, NV_FALSE},
    {1, dp2LinkRate_5_00Gbps, NV_TRUE,  NV_TRUE},   {2, dp2LinkRate_2_50Gbps, NV_TRUE, NV_TRUE},
    {1, dp2LinkRate_5_40Gbps, NV_TRUE,  NV_FALSE},  {2, dp2LinkRate_2_70Gbps, NV_TRUE, NV_FALSE},
    {2, dp2LinkRate_2_43Gbps, NV_FALSE, NV_FALSE},  {1, dp2LinkRate_4_32Gbps, NV_FALSE, NV_FALSE},
    {2, dp2LinkRate_2_16Gbps, NV_FALSE, NV_FALSE},  {1, dp2LinkRate_2_70Gbps, NV_TRUE, NV_TRUE},
    {1, dp2LinkRate_3_24Gbps, NV_FALSE, NV_FALSE},  {2, dp2LinkRate_1_62Gbps, NV_TRUE, NV_FALSE},
    {1, dp2LinkRate_2_50Gbps, NV_TRUE,  NV_TRUE},   {1, dp2LinkRate_2_70Gbps, NV_TRUE, NV_FALSE},
    {1, dp2LinkRate_2_43Gbps, NV_FALSE, NV_FALSE},  {1, dp2LinkRate_2_16Gbps, NV_FALSE, NV_FALSE},
    {1, dp2LinkRate_1_62Gbps, NV_TRUE,  NV_FALSE}
};

static NvU32 _getPollingIntervalMsForChannelEqDone(NvU32 pollingInfo)
{
    NvU32 pollIntervalMsVal     = 0;
    NvU32 pollIntervalUnit      = 0;
    NvU32 pollIntervalMs        = 0;

    pollIntervalMsVal   = DRF_VAL(0073_CTRL, _DP2X_POLLING_INFO, _TRAINING_AUX_RD_INTERVAL, pollingInfo);

    pollIntervalUnit    = FLD_TEST_DRF(0073_CTRL, _DP2X_POLLING_INFO, _TRAINING_AUX_RD_INTERVAL_UNIT, _2MS, pollingInfo) ?
                              NV_DP2X_LT_CHNL_EQ_TRAINING_AUX_RD_INTERVAL_UNIT_2MS :
                              NV_DP2X_LT_CHNL_EQ_TRAINING_AUX_RD_INTERVAL_UNIT_1MS;

    pollIntervalMs      = (pollIntervalMsVal + 1) * pollIntervalUnit;

    return pollIntervalMs;
}

EvoMainLink2x::EvoMainLink2x(EvoInterface *provider, Timer *timer)
    : EvoMainLink(provider, timer), gpuUhbrCaps(0), dfpUhbrCaps(0),
      bUseRgFlushSequence(false)
{
    NvU32   idx = 0U;
    applyDP2xRegkeyOverrides();
    // Init fallbackMandateTable to default values.
    for (idx = 0U; idx < NV_DP2X_VALID_LINK_CONFIGURATION_COUNT; idx++)
    {
        fallbackMandateTable[idx] = defaultFallbackMandateTable[idx];
    }
    channelEqualizationStartTimeUs = 0U;
    dpMemZero(&ltRmParams, sizeof(ltRmParams));
}

void EvoMainLink2x::applyDP2xRegkeyOverrides()
{
    this->bSupportUHBR2_50   = dpRegkeyDatabase.supportInternalUhbrOnFpga & NV_DP2X_REGKEY_FPGA_UHBR_SUPPORT_2_5G;
    this->bSupportUHBR2_70   = dpRegkeyDatabase.supportInternalUhbrOnFpga & NV_DP2X_REGKEY_FPGA_UHBR_SUPPORT_2_7G;
    this->bSupportUHBR5_00   = dpRegkeyDatabase.supportInternalUhbrOnFpga & NV_DP2X_REGKEY_FPGA_UHBR_SUPPORT_5_0G;
    this->bEnable5147205Fix  = dpRegkeyDatabase.bEnable5147205Fix;
	this->bCableVconnSourceUnknown = dpRegkeyDatabase.bCableVconnSourceUnknownWar;
}

NvU32 EvoMainLink2x::headToStream(NvU32 head, bool bSidebandMessageSupported,
                                  DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID streamIdentifier)
{
    NvU32 streamIndex = 0;

    NvU32 maxHeads = allHeadMask;
    NUMSETBITS_32(maxHeads);
    // When a sink doesn't support side band messages but uses MST protocol, i.e. 128b132b SST sink, force streamIndex to 1
    if (!bSidebandMessageSupported)
    {
        streamIndex = 1;
    }
    else
    {
        streamIndex = DP_MST_HEAD_TO_STREAMID(head, streamIdentifier, maxHeads);
    }

    return streamIndex;
}


/*!
 * @brief Check what UHBR link rates are supported on the GPU/PHY.
 *        Call into EvoMainLink::queryGPUCapability to probe other capabilities
 *
 *        Output is record in EvoMainLink2x::gpuUhbrCaps.
 *
 * @Possible return values:
 *      NV_TRUE
 *        EvoMainLink::queryGPUCapability() returns true and supported UHBR links
 *          are saved properly.
 *      NV_FALSE
 *        EvoMainLink::queryGPUCapability() returns false. DPLib assumes no UHBR
 *          is supported.
 */
bool EvoMainLink2x::queryGPUCapability()
{
    gpuUhbrCaps = 0;

    if (!EvoMainLink::queryGPUCapability())
        return false;

    gpuUhbrCaps         = dpParams.UHBRSupportedByGpu;
    bUseRgFlushSequence = dpParams.bUseRgFlushSequence;

    return true;
}

/*!
 * @brief Check what UHBR link rates are supported on the connector (Board SKU or DCB).
 *        Call into EvoMainLink::queryAndUpdateDfpParams to probe other capabilities
 *
 *        Output is record in EvoMainLink2x::dfpUhbrCaps.
 *
 * @Possible return values:
 *      NV_TRUE
 *        EvoMainLink::queryAndUpdateDfpParams() returns true and supported UHBR links
 *          are saved properly.
 *      NV_FALSE
 *        EvoMainLink::queryAndUpdateDfpParams() returns false. DPLib assumes no UHBR
 *          is supported.
 */
bool EvoMainLink2x::queryAndUpdateDfpParams()
{
    dfpUhbrCaps = 0;

    if (!EvoMainLink::queryAndUpdateDfpParams())
        return false;

    dfpUhbrCaps             = dfpParams.UHBRSupportedByDfp;
    bConnectorIsUSBTypeC    = FLD_TEST_DRF(0073, _CTRL_DFP_FLAGS, _TYPE_C_TO_DP_CONNECTOR, _TRUE, dfpParams.flags);

    return true;
}

/*!
 * @brief Update fallbackMandateTable based on the capabilities of GPU, Sink and CableId.
 *
 * @param[in]      maxLaneCount     Max Lanecount supported on the setup.
 * @param[in]      maxLinkRate      Max Link Rate supported on the setup.
 * @param[in]      uhbrCaps         Bitmask of what link rate is supported.
 *                                  Definition the same as NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS.UHBRSupportedByGpu
 *
 * Caller of this function has to complete the capabilities probing before calling
 * into this function.
 *
 * Output: EvoMainLink2x::fallbackMandateTable is updated for each entry to indicate
 *         if a specific link configuration is supported.
 *
 */
void EvoMainLink2x::updateFallbackMap
(
    NvU32    maxLaneCount,
    LinkRate maxLinkRate,
    NvU32    uhbrCaps
)
{
    bool    bUHBR10_0Supported = (FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_UHBR_SUPPORTED,
                                               _UHBR10_0, _YES, uhbrCaps));
    bool    bUHBR13_5Supported = (FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_UHBR_SUPPORTED,
                                              _UHBR13_5, _YES, uhbrCaps));
    bool    bUHBR20_0Supported = (FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_UHBR_SUPPORTED,
                                              _UHBR20_0, _YES, uhbrCaps));
    NvU8    idx = 0U;

    // Init fallbackMandateTable to default values.
    for (idx = 0U; idx < NV_DP2X_VALID_LINK_CONFIGURATION_COUNT; idx++)
    {
        fallbackMandateTable[idx] = defaultFallbackMandateTable[idx];
    }

    for (idx = 0U; idx < NV_DP2X_VALID_LINK_CONFIGURATION_COUNT; idx++)
    {
        // Invalidate all entries with lane count above maxLaneCount.
        if (fallbackMandateTable[idx].laneCount > maxLaneCount)
        {
            fallbackMandateTable[idx].bSupported = NV_FALSE;
            continue;
        }

        if (fallbackMandateTable[idx].bUseDP2xChannelCoding)
        {
            if (!IS_DP2X_UHBR_LINK_DATA_RATE(fallbackMandateTable[idx].linkRate))
            {
                fallbackMandateTable[idx].bSupported = NV_FALSE;
                continue;
            }

            // Sink side can support individual UHBRs.
            if (((fallbackMandateTable[idx].linkRate == dp2LinkRate_20_0Gbps) && (!bUHBR20_0Supported)) ||
                ((fallbackMandateTable[idx].linkRate == dp2LinkRate_13_5Gbps) && (!bUHBR13_5Supported)) ||
                ((fallbackMandateTable[idx].linkRate == dp2LinkRate_10_0Gbps) && (!bUHBR10_0Supported)) ||
                ((fallbackMandateTable[idx].linkRate == dp2LinkRate_5_00Gbps) && (!bSupportUHBR5_00))   ||
                ((fallbackMandateTable[idx].linkRate == dp2LinkRate_2_70Gbps) && (!bSupportUHBR2_70))   ||
                ((fallbackMandateTable[idx].linkRate == dp2LinkRate_2_50Gbps) && (!bSupportUHBR2_50)))
            {
                fallbackMandateTable[idx].bSupported = NV_FALSE;
            }
        }
        else if ((fallbackMandateTable[idx].linkRate == dp2LinkRate_2_70Gbps) && (bSupportUHBR2_70))
        {
            fallbackMandateTable[idx].bSupported = NV_FALSE;
        }
        else if (fallbackMandateTable[idx].linkRate > maxLinkRate)
        {
            fallbackMandateTable[idx].bSupported = NV_FALSE;
        }
    }
}

/*!
 * @brief Invalidate all entries with specific link rate in fallbackMandateTable based on request.
 *
 * @param[in]      linkRate     The link rate to be removed.
 *
 * Caller of this function has to complete the capabilities probing before calling
 * into this function.
 *
 * Output: EvoMainLink2x::fallbackMandateTable is updated for each entry to indicate
 *         if a specific link configuration is supported.
 */
void EvoMainLink2x::invalidateLinkRatesInFallbackTable(const LinkRate linkRate)
{
    NvU32 idx;

    for (idx = 0U; idx < NV_DP2X_VALID_LINK_CONFIGURATION_COUNT; idx++)
    {
        if (fallbackMandateTable[idx].linkRate == linkRate)
        {
            fallbackMandateTable[idx].bSupported = NV_FALSE;
            if (fallbackMandateTable[idx].laneCount == 1)
                return;
        }
    }
}

/*!
 * @brief Update fallbackMandateTable based on the capabilities of GPU, Sink and CableId.
 *
 * @param[in]      maxLaneCount     Max Lanecount supported on the setup.
 * @param[in]      maxLinkRate      Max Link Rate supported on the setup.
 * @param[in]      uhbrCaps         Bitmask of what link rate is supported.
 *                                  Definition the same as NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS.UHBRSupportedByGpu
 *
 * Caller of this function has to complete the capabilities probing before calling
 * into this function.
 *
 * Output: EvoMainLink2x::fallbackMandateTable is updated for each entry to indicate
 *         is a specific link configuration is supported.
 *
 */
void EvoMainLink2x::validateIlrInFallbackMap
(
    LinkRate    ilr,
    bool        bUseDP2xChannelCoding
)
{
    NvU8    idx = 0U;
    if (!IS_INTERMEDIATE_DP2_X_LINKBW(ilr))
        return;

    for (idx = 0U; idx < NV_DP2X_VALID_LINK_CONFIGURATION_COUNT; idx++)
    {
        if (fallbackMandateTable[idx].linkRate == ilr &&
            fallbackMandateTable[idx].bUseDP2xChannelCoding == bUseDP2xChannelCoding)
        {
            fallbackMandateTable[idx].bSupported = NV_TRUE;
        }
    }
}

//
// @brief This is to request RM to setup/reset link rate table, and save valid
//        link rates for use.
//
// @param[in]    pLinkRateTable    Pointer to link rate table to configure
// @param[Out]   pLinkRates        Pointer to LinkRates to keep valid link rates
//
// @return
//   true                   Link rate table configured with at least one valid link rate
//   false                  Otherwise
//
bool EvoMainLink2x::configureLinkRateTable
(
    const NvU16     *pLinkRateTable,
    LinkRates       *pLinkRates
)
{
    NV0073_CTRL_CMD_DP_CONFIG_INDEXED_LINK_RATES_PARAMS params;
    dpMemZero(&params, sizeof(params));

    // For now all ILR are using 8b/10b for now.
    bool        bUseDP2xChannelCoding = false;

    params.subDeviceInstance = subdeviceIndex;
    params.displayId = displayId;

    // Setup provided link rate table, otherwise it will be reset
    if (pLinkRateTable)
    {
        for (NvU32 i = 0U; i < NV0073_CTRL_DP_MAX_INDEXED_LINK_RATES; i++)
        {
            params.linkRateTbl[i] = pLinkRateTable[i];
        }
    }

    NvU32 code = provider->rmControl0073(NV0073_CTRL_CMD_DP_CONFIG_INDEXED_LINK_RATES,
                                         &params, sizeof(params));

    if ((pLinkRates != NULL ) && (code == NVOS_STATUS_SUCCESS) &&
        (params.linkBwCount <= NV0073_CTRL_DP_MAX_INDEXED_LINK_RATES))
    {
        pLinkRates->clear();
        for (NvU32 i = 0U; i < params.linkBwCount; i++)
        {
            if (IS_INTERMEDIATE_DP2_X_LINKBW(params.linkBwTbl[i]))
            {
                validateIlrInFallbackMap(params.linkBwTbl[i], bUseDP2xChannelCoding);
            }

            switch (params.linkBwTbl[i])
            {
                case dp2LinkRate_1_62Gbps:
                case dp2LinkRate_2_16Gbps:
                case dp2LinkRate_2_43Gbps:
                case dp2LinkRate_2_70Gbps:
                case dp2LinkRate_3_24Gbps:
                case dp2LinkRate_4_32Gbps:
                case dp2LinkRate_5_40Gbps:
                case dp2LinkRate_6_75Gbps:
                case dp2LinkRate_8_10Gbps:
                case dp2LinkRate_10_0Gbps:
                case dp2LinkRate_13_5Gbps:
                case dp2LinkRate_20_0Gbps:
                    pLinkRates->import((NvU16)params.linkBwTbl[i]);
                    break;
                default:
                    DP_PRINTF(DP_ERROR, "DP2xEVO> %s: Unsupported link rate received",
                           __FUNCTION__);
                    DP_ASSERT(0);
                    break;
            }
        }
        return true;
    }
    return false;
}

/*!
 * @brief Link train function in EvoMainLink layer which decides which channel coding
 *        is being to used for the link training.
 *
 * @param[in]      link                 The link configuration requested by client.
 * @param[in]      force                If the link training request would be faked.
 * @param[in]      linkTrainingType     Normal/Fast/No Link Training.
 * @param[out]     retLink              The real link configuration got trained in the end.
 * @param[in]      bSkipLt              If the LT request is only for updating SW states.
 *                                      The link should be already trained to
 *                                      the requested configuration
 * @param[in]      isPostLtAdjRequestGranted    If sink granted to request Post_LT_Adjust.
 * @param[in]      phyRepeaterCount     How many LTTPRs need to be trained.
 *
 * Output:
 *  True:   Link training succeed.
 *  False:  Link training failed.
 */
bool EvoMainLink2x::train(const LinkConfiguration & link, bool force,
                          LinkTrainingType linkTrainingType, LinkConfiguration *retLink,
                          bool bSkipLt, bool isPostLtAdjRequestGranted,
                          unsigned phyRepeaterCount)
{
    bool        ltStatus                    = false;
    bool        bSkipFallback               = false;
    bool        bFallback                   = false;
    bool        retryOnce                   = false;
    NvU32       ltCounter                   = retLink->getLTCounter();
    bool        bCur128b132bChannelCoding   = false;
    bool        bChannelCodingChanged       = false;
    bool        bIsGpuPowerDownLinkRequest  = false;

    if (provider->getSorIndex() == DP_INVALID_SOR_INDEX)
    {
        // bail out and Skip LT since SOR is not allocated for this displayID
        return false;
    }

    LinkConfiguration requestRmLC = link;

    //
    // Check if LinkConfiguration passed in is supported by the system
    // Skip the capability check if client forces the link training.
    //
    if (!force && !isSupportedDPLinkConfig(requestRmLC))
    {
        DP_PRINTF(DP_ERROR, "DP2xEVO> client requested link is not a supported link configuration!");
        return false;
    }

    DP2XResetParam resetParam;
    dpMemZero(&resetParam, sizeof(resetParam));
    resetParam.bForce = force;
    resetParam.bSkipLt = bSkipLt;

    // Get the original skipFallback setting.
    bSkipFallback = requestRmLC.policy.skipFallback();

    //
    // Check if this is a power down request
    //
    if (link.peakRate == dp2LinkRate_1_62Gbps && link.lanes == 0)
        bIsGpuPowerDownLinkRequest = true;

    do
    {
        // Keep record of requested channel coding.
        bCur128b132bChannelCoding = requestRmLC.bIs128b132bChannelCoding;

        // Reset DPRX link, and only move on if the link is clean.
        resetParam.reason = DP2X_ResetLinkForPreLT;
        if (!bIsGpuPowerDownLinkRequest && (!bFallback || bCur128b132bChannelCoding || bChannelCodingChanged))
        {
            if (!resetDPRXLink(resetParam))
            {
                DP_PRINTF(DP_ERROR, "DP2xEVO> Reset DP link before LT failed.");
                return false;
            }
        }

        // Check if the link is capable to train with 128b/132b
        if (requestRmLC.bIs128b132bChannelCoding)
        {
            ltStatus = trainDP2xChannelCoding(requestRmLC, force, linkTrainingType,
                                              retLink, bSkipLt,
                                              isPostLtAdjRequestGranted,
                                              phyRepeaterCount);
        }
        else
        {
            // Do not run the fallback sequence in EvoMainLink::train().
            requestRmLC.policy.setSkipFallBack(true);
            ltStatus = EvoMainLink::train(requestRmLC, force, linkTrainingType,
                                          retLink, bSkipLt, isPostLtAdjRequestGranted,
                                          phyRepeaterCount);
        }

        ltCounter++;

        // If LT passes or if client requests do not fallback, no fallback required.
        if (ltStatus || bSkipFallback)
        {
            bFallback = false;
        }
        else
        {
            bFallback = true;

            if (FLD_TEST_DRF(0073_CTRL, _DP2X_ERR, _LINK_STATUS, _DISCONNECTED, ltRmParams.err))
            {
                DP_PRINTF(DP_ERROR, "DP2xEVO> Link Disconnected - stop LT / Fallback.");
                // Do not fallback if link is disconnected.
                bFallback = false;
            }
            else if (retryOnce)
            {
                // If the link configuration fail the first time, try again.
                retryOnce = false;
            }
            else
            {
                if (this->isConnectorUSBTypeC() &&
                    requestRmLC.bIs128b132bChannelCoding &&
                    requestRmLC.peakRate > dp2LinkRate_10_0Gbps &&
                    bCableVconnSourceUnknown)
                {
                    //
                    // Invalidate the link rate from fallback table if the connector type is USB-C to DP
                    // and VCONN source is unknown.
                    // Source will not retry the same link rate if fallback LT fails again.
                    //
                    invalidateLinkRatesInFallbackTable(requestRmLC.peakRate);
                }
                //
                // Get next available link configuration based on DP2.1 spec, Table 3-31
                // Break here if next link configuration is not available.
                //
                if (!this->getFallbackForDP2xLinkTraining(&requestRmLC))
                {
                    DP_PRINTF(DP_ERROR, "DP2xEVO> No link configuration available for fallback");
                    bFallback = false;
                }

                //
                // Do not retry again later if it fails. Even it's a new link configuration.
                // We can only enable this once the option is formalized in spec
                // and implemented in compliance device.
                //
                retryOnce = false;
                bChannelCodingChanged = (requestRmLC.bIs128b132bChannelCoding != bCur128b132bChannelCoding);
                if (bChannelCodingChanged)
                {
                    DP_PRINTF(DP_NOTICE, "DP2xEVO> Fallback - Reset DP link before LT.");
                    // Reset link due to changing the channel coding during LT
                    resetParam.reason = DP2X_ResetLinkForFallback;
                    if (!resetDPRXLink(resetParam))
                    {
                        DP_PRINTF(DP_ERROR, "DP2xEVO> Reset DP link for fallback failed.");
                        return false;
                    }
                }
            }
        }
    } while (bFallback);

    retLink->setLaneRate(requestRmLC.peakRate, ltStatus ? requestRmLC.lanes : 0);
    retLink->setLTCounter(ltCounter);

    return ltStatus;
}

/*!
 * @brief The function in EvoMainLink layer which calls into RM for 128b/132b
 *        channel coding link training.
 *
 * @param[in]      link                 The link configuration requested by client.
 * @param[in]      force                If the link training request would be faked.
 * @param[in]      linkTrainingType     Normal/Fast/No Link Training.
 * @param[out]     retLink              The real link configuration got trained in the end.
 * @param[in]      bSkipLt              If the LT request is only for updating SW states.
 *                                      The link should be already trained to
 *                                      the requested configuration
 * @param[in]      isPostLtAdjRequestGranted    If sink granted to request Post_LT_Adjust.
 * @param[in]      phyRepeaterCount     How many LTTPRs need to be trained.
 *
 * Output:
 *  True:   Link training succeed.
 *  False:  Link training failed.
 */
bool EvoMainLink2x::trainDP2xChannelCoding
(
    LinkConfiguration  &link,
    bool                force,
    LinkTrainingType    linkTrainingType,
    LinkConfiguration  *retLink,
    bool                bSkipLt,
    bool                isPostLtAdjRequestGranted,
    unsigned            phyRepeaterCount
)
{
    NV_STATUS   status          = NV_OK;
    NvU32       dpCtrlCmd       = 0U;
    NvU32       dpCtrlData      = 0U;
    NvU32       pollIntervalMs  = 0U;
    bool        bPollStatus     = false;

    if (provider->getSorIndex() == DP_INVALID_SOR_INDEX)
    {
        // bail out and Skip LT since SOR is not allocated for this displayID
        return false;
    }
    NvU64 linkRate = link.peakRate;

    // The common settings across the whole training process.
    NvU32 dpCtrlCmdCommon   = DRF_DEF(0073_CTRL, _DP2X_CMD, _LINK_TRAINING_SETTING, _TRUE);

    if (bSkipLt)
    {
        dpCtrlCmdCommon |= DRF_DEF(0073_CTRL, _DP2X_CMD, _SKIP_HW_PROGRAMMING, _YES);
    }

    if (!this->isDownspreadSupported() || link.bDisableDownspread || _isDownspreadDisabledByRegkey)
    {
        dpCtrlCmdCommon |= DRF_DEF(0073_CTRL, _DP2X_CMD, _FORCED_DOWNSPREAD, _YES);
        dpCtrlCmdCommon |= DRF_DEF(0073_CTRL, _DP2X_CMD, _SET_DOWNSPREAD, _DISABLE);
    }

    if (force)
    {
        dpCtrlCmdCommon |= DRF_DEF(0073_CTRL, _DP2X_CMD, _FAKE_LINK_TRAINING,
                                   _DONOT_TOGGLE_TRANSMISSION);
    }

    //
    // Validation of link configuration for 128b/132b channel coding should be done in caller.
    // For example - EvoMainLink2x::train()
    //
    dpCtrlData = FLD_SET_DRF_NUM(0073_CTRL, _DP2X_DATA, _LANE_COUNT, link.lanes, dpCtrlData);

    switch (linkRate)
    {
        case dp2LinkRate_1_62Gbps:
            dpCtrlData = FLD_SET_DRF(0073_CTRL, _DP2X_DATA, _LINK_BW, _1_62GBPS, dpCtrlData);
            break;
        case dp2LinkRate_2_70Gbps:
            dpCtrlData = FLD_SET_DRF(0073_CTRL, _DP2X_DATA, _LINK_BW, _2_70GBPS, dpCtrlData);
            break;
        case dp2LinkRate_5_40Gbps:
            dpCtrlData = FLD_SET_DRF(0073_CTRL, _DP2X_DATA, _LINK_BW, _5_40GBPS, dpCtrlData);
            break;
        case dp2LinkRate_8_10Gbps:
            dpCtrlData = FLD_SET_DRF(0073_CTRL, _DP2X_DATA, _LINK_BW, _8_10GBPS, dpCtrlData);
            break;
        case dp2LinkRate_10_0Gbps:
            dpCtrlData = FLD_SET_DRF(0073_CTRL, _DP2X_DATA, _LINK_BW, _UHBR10_0, dpCtrlData);
            break;
        case dp2LinkRate_13_5Gbps:
            dpCtrlData = FLD_SET_DRF(0073_CTRL, _DP2X_DATA, _LINK_BW, _UHBR13_5, dpCtrlData);
            break;
        case dp2LinkRate_20_0Gbps:
            dpCtrlData = FLD_SET_DRF(0073_CTRL, _DP2X_DATA, _LINK_BW, _UHBR20_0, dpCtrlData);
            break;
        default:
            if (link.lanes != 0U)
            {
                DP_ASSERT(!"DP2xEVO> Unknown rate");
                return false;
            }
            break;
    }

    dpCtrlCmd = DRF_DEF(0073_CTRL, _DP2X_CMD, _LINK_TRAINING_SET, _PRE_LT);
    dpCtrlCmd |= dpCtrlCmdCommon;

    dpMemZero(&ltRmParams, sizeof(ltRmParams));
    ltRmParams.subDeviceInstance        = subdeviceIndex;
    ltRmParams.displayId                = displayId;
    ltRmParams.cmd                      = dpCtrlCmd;
    ltRmParams.data                     = dpCtrlData;

    status = provider->rmControl0073(NV0073_CTRL_CMD_DP2X_LINK_TRAINING_CTRL,
                                     &ltRmParams, sizeof(ltRmParams));
    if (status != NVOS_STATUS_SUCCESS)
    {
        DP_PRINTF(DP_ERROR, "DP2xEVO> Set PRE_LT failed.");
        return false;
    }

    dpCtrlCmd = DRF_DEF(0073_CTRL, _DP2X_CMD, _LINK_TRAINING_SET, _CHNL_EQ);
    dpCtrlCmd |= dpCtrlCmdCommon;

    dpMemZero(&ltRmParams, sizeof(ltRmParams));
    ltRmParams.subDeviceInstance        = subdeviceIndex;
    ltRmParams.displayId                = displayId;
    ltRmParams.cmd                      = dpCtrlCmd;
    ltRmParams.data                     = dpCtrlData;

    status = provider->rmControl0073(NV0073_CTRL_CMD_DP2X_LINK_TRAINING_CTRL,
                                     &ltRmParams, sizeof(ltRmParams));
    if (status != NVOS_STATUS_SUCCESS)
    {
        DP_PRINTF(DP_ERROR, "DP2xEVO> Set Channel Equalization failed.");
        return false;
    }

    // Calculate pollInterval per Spec
    pollIntervalMs = _getPollingIntervalMsForChannelEqDone(ltRmParams.pollingInfo);

    channelEqualizationStartTimeUs = timer->getTimeUs();

    bPollStatus = pollDP2XLinkTrainingStageDone(DP2X_LT_Poll_ChnlEq_Done,
                                                link.lanes,
                                                phyRepeaterCount,
                                                pollIntervalMs,
                                                force);
    if (!bPollStatus)
    {
        DP_PRINTF(DP_ERROR, "DP2xEVO> Poll ChannelEQ Done failed.");
        return false;
    }

    pollIntervalMs = NV_DP2X_LT_CHNL_EQ_INTERLANE_ALIGN_POLLING_INTERVAL;

    bPollStatus = pollDP2XLinkTrainingStageDone(DP2X_LT_Poll_ChnlEq_InterlaneAlign,
                                                link.lanes,
                                                phyRepeaterCount,
                                                pollIntervalMs,
                                                force);
    if (!bPollStatus)
    {
        DP_PRINTF(DP_ERROR, "DP2xEVO> Poll ChannelEQ Interlane Align failed.");
        return false;
    }

    dpCtrlCmd = DRF_DEF(0073_CTRL, _DP2X_CMD, _LINK_TRAINING_SET, _CDS);
    dpCtrlCmd |= dpCtrlCmdCommon;

    dpMemZero(&ltRmParams, sizeof(ltRmParams));
    ltRmParams.subDeviceInstance        = subdeviceIndex;
    ltRmParams.displayId                = displayId;
    ltRmParams.cmd                      = dpCtrlCmd;
    ltRmParams.data                     = dpCtrlData;

    status = provider->rmControl0073(NV0073_CTRL_CMD_DP2X_LINK_TRAINING_CTRL,
                                     &ltRmParams, sizeof(ltRmParams));

    if (status != NVOS_STATUS_SUCCESS)
    {
        DP_PRINTF(DP_ERROR, "DP2xEVO> Set CDS failed.");
        return false;
    }

    pollIntervalMs = NV_DP2X_LT_CDS_POLLING_INTERVAL;
    bPollStatus = pollDP2XLinkTrainingStageDone(DP2X_LT_Poll_CDS,
                                                link.lanes,
                                                phyRepeaterCount,
                                                pollIntervalMs,
                                                force);
    if (!bPollStatus)
    {
        DP_PRINTF(DP_ERROR, "DP2xEVO> Poll CDS failed.");
        return false;
    }

    dpCtrlCmd = DRF_DEF(0073_CTRL, _DP2X_CMD, _LINK_TRAINING_SET, _POST_LT);
    dpCtrlCmd |= dpCtrlCmdCommon;

    dpMemZero(&ltRmParams, sizeof(ltRmParams));
    ltRmParams.subDeviceInstance        = subdeviceIndex;
    ltRmParams.displayId                = displayId;
    ltRmParams.cmd                      = dpCtrlCmd;
    ltRmParams.data                     = dpCtrlData;

    status = provider->rmControl0073(NV0073_CTRL_CMD_DP2X_LINK_TRAINING_CTRL,
                                     &ltRmParams, sizeof(ltRmParams));
    if (status != NVOS_STATUS_SUCCESS)
    {
        DP_PRINTF(DP_ERROR, "DP2xEVO> Set POST_LT failed.");
        return false;
    }

    return (status == NV_OK);
}

/*!
 * @brief Link train function in EvoMainLink layer which polls for completion of
 *        each phase.
 *
 * @param[in]      stage                Specify which stage to poll.
 * @param[in]      laneCount            How many lanes are being trained.
 * @param[in]      phyRepeaterCount     How many LTTPRs are detected and being trained.
 * @param[in]      pollingInterval      How many ms between each call into RM.
 *
 * Output:
 *  True:   Polling for the stage completion succeed.
 *  False:  Polling for the stage completion failed.
 */
bool EvoMainLink2x::pollDP2XLinkTrainingStageDone
(
    NvU32 stage,
    NvU32 laneCount,
    NvU32 phyRepeaterCount,
    NvU32 pollingInterval,
    bool  force
)
{
    NvU32 loopCount     = 1U;
    NvU32 status        = NVOS_STATUS_SUCCESS;
    NvU32 dpCtrlData    = 0U;

    NvU32 dpCtrlCmd     = DRF_DEF(0073_CTRL, _DP2X_CMD, _LINK_TRAINING_POLLING, _TRUE);

    dpCtrlData          = FLD_SET_DRF_NUM(0073_CTRL, _DP2X_DATA, _LANE_COUNT,
                                          laneCount, dpCtrlData);

    NvU32 totalPollTime = 0U;
    NvU32 elapsedTimeMs = 0U;

    if (force)
        return true;

    switch (stage)
    {
        case DP2X_LT_Poll_ChnlEq_Done:
            totalPollTime = NV_DP2X_LT_MAX_TIME_POLL_CHNL_EQ_MS;
            dpCtrlCmd = FLD_SET_DRF(0073_CTRL, _DP2X_CMD, _LINK_TRAINING_POLL,
                                    _CHNL_EQ_DONE, dpCtrlCmd);
            break;
        case DP2X_LT_Poll_ChnlEq_InterlaneAlign:
            elapsedTimeMs = (NvU32)((timer->getTimeUs() - channelEqualizationStartTimeUs) / 1000);
            totalPollTime = (NV_DP2X_LT_MAX_TIME_POLL_CHNL_EQ_MS > elapsedTimeMs) ?
                            (NV_DP2X_LT_MAX_TIME_POLL_CHNL_EQ_MS - elapsedTimeMs) : 100;

            dpCtrlCmd = FLD_SET_DRF(0073_CTRL, _DP2X_CMD, _LINK_TRAINING_POLL,
                                    _CHNL_EQ_INTERLANE_ALIGN, dpCtrlCmd);
            break;
        case DP2X_LT_Poll_CDS:
            totalPollTime = NV_DP2X_LT_MAX_POLL_TIME_CDS_MS(phyRepeaterCount);
            dpCtrlCmd = FLD_SET_DRF(0073_CTRL, _DP2X_CMD, _LINK_TRAINING_POLL,
                                    _CDS, dpCtrlCmd);
            break;
        default:
            return false;
    }

    dpMemZero(&ltRmParams, sizeof(ltRmParams));
    ltRmParams.subDeviceInstance        = subdeviceIndex;
    ltRmParams.displayId                = displayId;
    ltRmParams.cmd                      = dpCtrlCmd;
    ltRmParams.data                     = dpCtrlData;

    Timeout timeout(this->timer, totalPollTime);
    do
    {
        timer->sleep(pollingInterval);
        status = provider->rmControl0073(NV0073_CTRL_CMD_DP2X_LINK_TRAINING_CTRL,
                                         &ltRmParams, sizeof(ltRmParams));

        if (FLD_TEST_DRF(0073_CTRL, _DP2X_ERR, _LT_FAILED, _YES, ltRmParams.err))
        {
            DP_PRINTF(DP_ERROR, "DP2xEVO> Link Training failed.");
            if (FLD_TEST_DRF(0073_CTRL, _DP2X_ERR, _CHANNEL_EQ, _FAILED, ltRmParams.err))
            {
                DP_PRINTF(DP_ERROR, "DP2xEVO> Polling in Channel Equalization phase failed.");
            }
            if (FLD_TEST_DRF(0073_CTRL, _DP2X_ERR, _CDS, _FAILED, ltRmParams.err))
            {
                DP_PRINTF(DP_ERROR, "DP2xEVO> Polling in CDS failed.");
            }
            return false;
        }

        if (status != NVOS_STATUS_SUCCESS)
            return false;

        if (FLD_TEST_DRF(0073_CTRL, _DP2X_POLLING_INFO, _RESULT, _DONE, ltRmParams.pollingInfo))
            return true;

        if (stage == DP2X_LT_Poll_ChnlEq_Done)
        {
            if (++loopCount > NV_DP2X_LT_MAX_LOOP_COUNT_POLL_CHNL_EQ_DONE)
            {
                break;
            }
            pollingInterval = _getPollingIntervalMsForChannelEqDone(ltRmParams.pollingInfo);
        }

    } while(timeout.valid());

    return (FLD_TEST_DRF(0073_CTRL, _DP2X_POLLING_INFO, _RESULT, _DONE, ltRmParams.pollingInfo));
}

/*!
 * @brief Check if LinkConfiguration is a supported LinkConfiguration
 *
 * @param[in]   link    LinkConfiguration object to check support
 *
 * Output:
 *  True:   link is supported DP LinkConfiguartion
 *  False:  link is unsupported DP LinkConfiguration
 */
bool EvoMainLink2x::isSupportedDPLinkConfig(LinkConfiguration &link)
{
    NvU8 linkIdx = 0;

    // Power Down Sequence
    if (link.lanes == 0 && link.peakRate == dp2LinkRate_1_62Gbps)
        return true;

    if (link.lanes == 0)
        return false;

    for (linkIdx = 0; linkIdx < NV_DP2X_VALID_LINK_CONFIGURATION_COUNT; linkIdx++)
    {
        if ((link.lanes    == fallbackMandateTable[linkIdx].laneCount) &&
            (link.peakRate == fallbackMandateTable[linkIdx].linkRate)  &&
            (link.bIs128b132bChannelCoding == fallbackMandateTable[linkIdx].bUseDP2xChannelCoding))
        {
            return fallbackMandateTable[linkIdx].bSupported;
        }
    }

    return false;
}

/*!
 * @brief Find next valid fallback configuration for specified link.
 *
 * @param[in/out]      link              In: The link configuration being checked.
 *                                      Out: The fallback link configuration.
 *
 * Output:
 *  True:   A valid fallback link configuration is found.
 *  False:  No valid fallback link configuration is found.
 */
bool EvoMainLink2x::getFallbackForDP2xLinkTraining(LinkConfiguration *link)
{
    NvU8    linkIdx = 0;
    NvU8    targetIdx = 0;
    bool    bTargetConfigFound = NV_FALSE;

    if (link->lanes == 1 && link->peakRate == dp2LinkRate_1_62Gbps &&
        link->bIs128b132bChannelCoding == false)
    {
        // if the last entry, nothing to fallback.
        return false;
    }

    for (linkIdx = 0; linkIdx < NV_DP2X_VALID_LINK_CONFIGURATION_COUNT; linkIdx++)
    {
        if ((link->lanes                     == fallbackMandateTable[linkIdx].laneCount) &&
            (link->peakRate                  == fallbackMandateTable[linkIdx].linkRate)  &&
            (link->bIs128b132bChannelCoding  == (bool)fallbackMandateTable[linkIdx].bUseDP2xChannelCoding))
        {
            targetIdx           = linkIdx;
            bTargetConfigFound  = NV_TRUE;
            break;
        }
    }

    if (!bTargetConfigFound)
    {
        DP_ASSERT(!"DP2xEVO> Unknown rate or no link configuration matched");
        return false;
    }

    for (linkIdx = targetIdx + 1; linkIdx < NV_DP2X_VALID_LINK_CONFIGURATION_COUNT; linkIdx++)
    {
        // since 8b/10b 1*RBR is ruled out earlier, the entry won't be the last entry.
        if (fallbackMandateTable[linkIdx].bSupported)
        {
            link->lanes                    = fallbackMandateTable[linkIdx].laneCount;
            link->peakRate                 = fallbackMandateTable[linkIdx].linkRate;
            link->bIs128b132bChannelCoding = fallbackMandateTable[linkIdx].bUseDP2xChannelCoding;
            return true;
        }
    }

    DP_ASSERT(!"DP2xEVO> Unknown rate or no link configuration matched");
    return false;
}

/*!
 * @brief Return the max link rate supported on the GPU side.
 *        Both GPU PHY capability and board capability (DCB) are required for
 *        any link rates.
 *
 * Output:
 *    The max link rate supported on the GPU side.
 */
NvU32 EvoMainLink2x::maxLinkRateSupported()
{
    NvU32 uhbrCaps = getUHBRSupported();

    if (FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_UHBR_SUPPORTED, _UHBR20_0, _YES, uhbrCaps))
    {
        return dp2LinkRate_20_0Gbps;
    }

    if (FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_UHBR_SUPPORTED, _UHBR13_5, _YES, uhbrCaps))
    {
        return dp2LinkRate_13_5Gbps;
    }

    if (FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_UHBR_SUPPORTED, _UHBR10_0, _YES, uhbrCaps))
    {
        return dp2LinkRate_10_0Gbps;
    }

    return EvoMainLink::maxLinkRateSupported();
}

/*!
 * @brief Engage flush mode in HW
 *
 * @param[in]   phase             The requested flush mode phase
 *
 * @return      Boolean to indicate success or failure
 */
bool EvoMainLink2x::setFlushMode(FlushModePhase phase)
{
    NVC370_CTRL_SET_SOR_FLUSH_MODE_PARAMS params = {0};

    params.subDeviceInstance = subdeviceIndex;
    params.sorNumber = provider->getSorIndex();
    params.bEnable = NV_TRUE;

    params.phase = NVC370_CTRL_SET_SOR_FLUSH_RUN_PHASE_INVALID;
    switch (phase)
    {
        case FlushModePhase1:
            params.phase = NVC370_CTRL_SET_SOR_FLUSH_RUN_PHASE1;
            break;
        case FlushModePhase2:
            params.phase = NVC370_CTRL_SET_SOR_FLUSH_RUN_PHASE2;
            break;
        default:
            DP_PRINTF(DP_ERROR, "DP2xEVO> Error: Unknown phase passed in.");
            return false;
    }

    NvU32 ret = provider->rmControl5070(NVC370_CTRL_CMD_SET_SOR_FLUSH_MODE,
                                        &params, sizeof params);
    if (ret != NVOS_STATUS_SUCCESS)
    {
        DP_PRINTF(DP_ERROR, "DP2xEVO> Enabling flush mode failed!");
    }

    return (ret == NVOS_STATUS_SUCCESS);
}

/*!
 * @brief Disengage flush mode in HW
 *
 * @param[in]   phase                    The requested flush phase
 * @param[in]   attachFailedHeadMask     Mask of all head indices where attach failed
 * @param[in]   headIndex                The head for which flush needs to be disabled. Valid only for phase 1
 *
 * @return      Boolean to indicate success or failure
 */
bool EvoMainLink2x::clearFlushMode(FlushModePhase phase,
                                   NvU32 attachFailedHeadMask,
                                   NvU32 headIndex)
{
    NVC370_CTRL_SET_SOR_FLUSH_MODE_PARAMS params = {0};

    params.subDeviceInstance = subdeviceIndex;
    params.sorNumber = provider->getSorIndex();
    params.bEnable = NV_FALSE;
    params.headIndex = headIndex;
    params.attachFailedHeadMask = attachFailedHeadMask;

    params.phase = NVC370_CTRL_SET_SOR_FLUSH_RUN_PHASE_INVALID;
    switch (phase)
    {
        case FlushModePhase1:
            params.phase = NVC370_CTRL_SET_SOR_FLUSH_RUN_PHASE1;
            break;
        case FlushModePhase2:
            params.phase = NVC370_CTRL_SET_SOR_FLUSH_RUN_PHASE2;
            break;
        default:
            DP_PRINTF(DP_ERROR, "DP2xEVO> Error: Unknown phase passed in.");
            return false;
    }

    NvU32 ret = provider->rmControl5070(NVC370_CTRL_CMD_SET_SOR_FLUSH_MODE,
                                        &params, sizeof params);
    if (ret != NVOS_STATUS_SUCCESS)
    {
        DP_PRINTF(DP_ERROR, "DP2xEVO> Disabling flush mode failed!");
    }

    return (ret == NVOS_STATUS_SUCCESS);
}

bool EvoMainLink2x::resetDPRXLink(DP2XResetParam resetParam)
{
    NvU32   ret         = 0U;
    bool    bFailed     = false;

    Timeout timeout(this->timer, NV_DP2X_LT_MAX_TIME_POLL_CHNL_EQ_MS);

    dpMemZero(&ltRmParams, sizeof(ltRmParams));

    ltRmParams.subDeviceInstance    = subdeviceIndex;
    ltRmParams.displayId            = displayId;
    ltRmParams.cmd                  = DRF_DEF(0073_CTRL, _DP2X_CMD, _LINK_TRAINING_SETTING, _TRUE) |
                                      DRF_DEF(0073_CTRL, _DP2X_CMD, _LINK_TRAINING_SET, _RESET_LINK);
    if (resetParam.bForce)
    {
        ltRmParams.cmd |= DRF_DEF(0073_CTRL, _DP2X_CMD, _FAKE_LINK_TRAINING, _DONOT_TOGGLE_TRANSMISSION);
    }

    if (resetParam.bSkipLt && bEnable5147205Fix)
    {
        ltRmParams.cmd |= DRF_DEF(0073_CTRL, _DP2X_CMD, _SKIP_HW_PROGRAMMING, _YES);
    }

    switch (resetParam.reason)
    {
        case DP2X_ResetLinkForPreLT:
            ltRmParams.data         = DRF_DEF(0073_CTRL, _DP2X_DATA, _RESET_REASON, _START_LT);
            break;
        case DP2X_ResetLinkForFallback:
            ltRmParams.data         = DRF_DEF(0073_CTRL, _DP2X_DATA, _RESET_REASON, _FALLBACK);
            break;
        case DP2X_ResetLinkForChannelCoding:
            ltRmParams.data         = DRF_DEF(0073_CTRL, _DP2X_DATA, _RESET_REASON, _CHANNEL_CODING);
            break;
    }

    do
    {
        ret = provider->rmControl0073(NV0073_CTRL_CMD_DP2X_LINK_TRAINING_CTRL,
                                      &ltRmParams, sizeof(ltRmParams));

        if (ret != NVOS_STATUS_SUCCESS)
        {
            // retry once if failed. If failed again bailed out.
            DP_ASSERT(!"DP2xEVO> EvoMainLink2x::resetDPRXLink(): failed to set reset link!\n");
            if (FLD_TEST_DRF(0073_CTRL, _DP2X_ERR, _RESET_LINK, _ERR, ltRmParams.err))
            {
                if (bFailed)
                    break;

                bFailed         = true;
                ltRmParams.err  = 0U;
                timer->sleep(NV_DP2X_LT_CHNL_EQ_INTERLANE_ALIGN_POLLING_INTERVAL);
                continue;
            }
            else
            {
                DP_ASSERT(!"DP2xEVO> EvoMainLink2x::resetDPRXLink(): Generic Error!\n");
                return false;
            }
        }
        else
        {
            // If it's done, then break.
            break;
        }
    } while (timeout.valid());

    if (ret != NVOS_STATUS_SUCCESS)
    {
        DP_PRINTF(DP_ERROR, "DP2xEVO> Failed to trigger DP link reset!");
        // Move on even if it fails, hopefully DPRX get ready later.
    }

    // For fallback scenario, TX doesn't need poll IntraHopAuxReply bit.
    if (resetParam.reason != DP2X_ResetLinkForPreLT)
    {
        return (ret == NVOS_STATUS_SUCCESS);
    }

    // Check if DPRX is ready to 128b/132b link training.
    ltRmParams.pollingInfo  = 0U;
    ltRmParams.cmd          = DRF_DEF(0073_CTRL, _DP2X_CMD, _LINK_TRAINING_POLLING, _TRUE) |
                              DRF_DEF(0073_CTRL, _DP2X_CMD, _LINK_TRAINING_POLL, _RESET_LINK);

    if (resetParam.bForce)
    {
        ltRmParams.cmd |= DRF_DEF(0073_CTRL, _DP2X_CMD, _FAKE_LINK_TRAINING, _DONOT_TOGGLE_TRANSMISSION);
    }

    do
    {
        ret = provider->rmControl0073(NV0073_CTRL_CMD_DP2X_LINK_TRAINING_CTRL,
                                      &ltRmParams, sizeof(ltRmParams));
        if (ret != NVOS_STATUS_SUCCESS)
        {
            DP_ASSERT(!"DP2xEVO> EvoMainLink2x::resetDPRXLink(): failed to poll reset link!\n");
            // retry once if aux failed. If failed again bailed out.
            if (FLD_TEST_DRF(0073_CTRL, _DP2X_ERR, _RESET_LINK, _ERR, ltRmParams.err))
            {
                if (bFailed)
                    break;

                bFailed             = true;
                ltRmParams.pollingInfo  = 0U;
                ltRmParams.err          = 0U;
                timer->sleep(NV_DP2X_LT_CHNL_EQ_INTERLANE_ALIGN_POLLING_INTERVAL);
                continue;
            }
        }
        // If it's done, leave.
        if (FLD_TEST_DRF(0073_CTRL, _DP2X_POLLING_INFO, _RESULT, _DONE, ltRmParams.pollingInfo))
            break;
    } while (timeout.valid());

    return ((FLD_TEST_DRF(0073_CTRL, _DP2X_ERR, _RESET_LINK, _NOERR, ltRmParams.pollingInfo)) &&
            (FLD_TEST_DRF(0073_CTRL, _DP2X_POLLING_INFO, _RESULT, _DONE, ltRmParams.pollingInfo)));
}

void EvoMainLink2x::getLinkConfig(unsigned &laneCount, NvU64 & linkRate)
{
    NV0073_CTRL_DP_GET_LINK_CONFIG_PARAMS params = {0};

    params.subDeviceInstance = subdeviceIndex;
    params.displayId = displayId;

    NvU32 code = provider->rmControl0073(NV0073_CTRL_CMD_DP_GET_LINK_CONFIG, &params, sizeof(params));

    if (code == NVOS_STATUS_SUCCESS)
    {
        laneCount = params.laneCount;
        if (params.linkBW != 0)
        {
            DP_ASSERT((params.dp2LinkBW == 0) && "DP2xEVO> dp2LinkBW should be zero if linkBw is not zero");
            linkRate = LINK_RATE_270MHZ_TO_10MHZ((NvU64)params.linkBW);
        }
        else if (params.dp2LinkBW != 0)
        {
            DP_ASSERT((params.linkBW == 0) && "DP2xEVO> linkBW should be zero if dp2LinkBW is not zero");
            linkRate = ((NvU64)params.dp2LinkBW);
        }
        else
        {
            // No link rate available.
            DP_ASSERT(!"DP2xEVO> Invalid linkBW returned");
            linkRate = 0;
        }
    }
    else
    {
        laneCount = 0;
        linkRate = 0;
    }
}

bool EvoMainLink2x::getDp2xLaneData(NvU32 *numLanes, NvU32 *data)
{
    NvU32   retVal = NVOS_STATUS_SUCCESS;

    NV0073_CTRL_DP2X_GET_LANE_DATA_PARAMS params = {0};

    params.subDeviceInstance = this->subdeviceIndex;
    params.displayId = this->displayId;

    retVal = provider->rmControl0073(NV0073_CTRL_CMD_DP2X_GET_LANE_DATA, &params, sizeof params);
    if (retVal == NVOS_STATUS_SUCCESS)
    {
        *numLanes = params.numLanes;
        dpMemCopy(data, params.data, NV0073_CTRL_DP2X_LANE_DATA_BUFFER_SIZE);
        return true;
    }
    else
    {
        return false;
    }
}

bool EvoMainLink2x::setDp2xLaneData(NvU32 numLanes, NvU32 *data)
{
    NvU32   retVal = NVOS_STATUS_SUCCESS;

    NV0073_CTRL_DP2X_SET_LANE_DATA_PARAMS params = {0};

    params.subDeviceInstance = this->subdeviceIndex;
    params.displayId = this->displayId;
    params.numLanes = numLanes;
    dpMemCopy(params.data, data, NV0073_CTRL_DP2X_LANE_DATA_BUFFER_SIZE);

    retVal = provider->rmControl0073(NV0073_CTRL_CMD_DP2X_SET_LANE_DATA, &params, sizeof params);
    if (retVal == NVOS_STATUS_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool EvoMainLink2x::physicalLayerSetDP2xTestPattern(DP2xPatternInfo *patternInfo)
{
    // Main parameter
    NV0073_CTRL_DP_SET_TESTPATTERN_PARAMS params;

    // To identify which test pattern to transmit.
    NV0073_CTRL_DP_TESTPATTERN ctrlPattern;

    dpMemZero(&params, sizeof(params));
    dpMemZero(&ctrlPattern, sizeof(ctrlPattern));

    switch (patternInfo->lqsPattern)
    {
        case LINK_QUAL_DISABLED:
            ctrlPattern.testPattern = FLD_SET_DRF(0073_CTRL, _DP_TESTPATTERN, _DATA, _NONE, ctrlPattern.testPattern);
            break;
        case LINK_QUAL_128B132B_TPS1:
            ctrlPattern.testPattern = FLD_SET_DRF(0073_CTRL, _DP_TESTPATTERN, _DATA, _128B132B_TPS1,
                                                  ctrlPattern.testPattern);
            break;
        case LINK_QUAL_128B132B_TPS2:
            ctrlPattern.testPattern = FLD_SET_DRF(0073_CTRL, _DP_TESTPATTERN, _DATA, _128B132B_TPS2,
                                                  ctrlPattern.testPattern);
            break;
        case LINK_QUAL_PRBS9:
            ctrlPattern.testPattern = FLD_SET_DRF(0073_CTRL, _DP_TESTPATTERN, _DATA, _PRBS_9, ctrlPattern.testPattern);
            break;
        case LINK_QUAL_PRBS11:
            ctrlPattern.testPattern = FLD_SET_DRF(0073_CTRL, _DP_TESTPATTERN, _DATA, _PRBS_11, ctrlPattern.testPattern);
            break;
        case LINK_QUAL_PRBS15:
            ctrlPattern.testPattern = FLD_SET_DRF(0073_CTRL, _DP_TESTPATTERN, _DATA, _PRBS_15, ctrlPattern.testPattern);
            break;
        case LINK_QUAL_PRBS23:
            ctrlPattern.testPattern = FLD_SET_DRF(0073_CTRL, _DP_TESTPATTERN, _DATA, _PRBS_23, ctrlPattern.testPattern);
            break;
        case LINK_QUAL_PRBS31:
            ctrlPattern.testPattern = FLD_SET_DRF(0073_CTRL, _DP_TESTPATTERN, _DATA, _PRBS_31, ctrlPattern.testPattern);
            break;
        case LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_ON_DE_EMPHASIS_ON:
            ctrlPattern.testPattern = FLD_SET_DRF(0073_CTRL, _DP_TESTPATTERN, _DATA, _SQNUM,
                                                  ctrlPattern.testPattern);
            ctrlPattern.testPattern = FLD_SET_DRF(0073_CTRL, _DP_TESTPATTERN, _SQ_PATTERN_PRESHOOT, _ENABLED,
                                                  ctrlPattern.testPattern);
            ctrlPattern.testPattern = FLD_SET_DRF(0073_CTRL, _DP_TESTPATTERN, _SQ_PATTERN_DE_EMPHASIS, _ENABLED,
                                                  ctrlPattern.testPattern);
            ctrlPattern.testPattern = FLD_SET_DRF_NUM(0073_CTRL, _DP_TESTPATTERN, _SQ_PATTERN_NUM, patternInfo->sqNum,
                                                      ctrlPattern.testPattern);
            break;
        case LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_OFF_DE_EMPHASIS_ON:
            ctrlPattern.testPattern = FLD_SET_DRF(0073_CTRL, _DP_TESTPATTERN, _DATA, _SQNUM,
                                                  ctrlPattern.testPattern);
            ctrlPattern.testPattern = FLD_SET_DRF(0073_CTRL, _DP_TESTPATTERN, _SQ_PATTERN_PRESHOOT, _DISABLED,
                                                  ctrlPattern.testPattern);
            ctrlPattern.testPattern = FLD_SET_DRF(0073_CTRL, _DP_TESTPATTERN, _SQ_PATTERN_DE_EMPHASIS, _ENABLED,
                                                  ctrlPattern.testPattern);
            ctrlPattern.testPattern = FLD_SET_DRF_NUM(0073_CTRL, _DP_TESTPATTERN, _SQ_PATTERN_NUM, patternInfo->sqNum,
                                                      ctrlPattern.testPattern);
            break;
        case LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_ON_DE_EMPHASIS_OFF:
            ctrlPattern.testPattern = FLD_SET_DRF(0073_CTRL, _DP_TESTPATTERN, _DATA, _SQNUM,
                                                  ctrlPattern.testPattern);
            ctrlPattern.testPattern = FLD_SET_DRF(0073_CTRL, _DP_TESTPATTERN, _SQ_PATTERN_PRESHOOT, _ENABLED,
                                                  ctrlPattern.testPattern);
            ctrlPattern.testPattern = FLD_SET_DRF(0073_CTRL, _DP_TESTPATTERN, _SQ_PATTERN_DE_EMPHASIS, _DISABLED,
                                                  ctrlPattern.testPattern);
            ctrlPattern.testPattern = FLD_SET_DRF_NUM(0073_CTRL, _DP_TESTPATTERN, _SQ_PATTERN_NUM, patternInfo->sqNum,
                                                      ctrlPattern.testPattern);
            break;
        case LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_OFF_DE_EMPHASIS_OFF:
            ctrlPattern.testPattern = FLD_SET_DRF(0073_CTRL, _DP_TESTPATTERN, _DATA, _SQNUM,
                                                  ctrlPattern.testPattern);
            ctrlPattern.testPattern = FLD_SET_DRF(0073_CTRL, _DP_TESTPATTERN, _SQ_PATTERN_PRESHOOT, _DISABLED,
                                                  ctrlPattern.testPattern);
            ctrlPattern.testPattern = FLD_SET_DRF(0073_CTRL, _DP_TESTPATTERN, _SQ_PATTERN_DE_EMPHASIS, _DISABLED,
                                                  ctrlPattern.testPattern);
            ctrlPattern.testPattern = FLD_SET_DRF_NUM(0073_CTRL, _DP_TESTPATTERN, _SQ_PATTERN_NUM, patternInfo->sqNum,
                                                      ctrlPattern.testPattern);
            break;
        case LINK_QUAL_264BIT_CUST:
        {
            ctrlPattern.testPattern = FLD_SET_DRF(0073_CTRL, _DP_TESTPATTERN, _DATA, _CSTM_264,
                                                  ctrlPattern.testPattern);
            params.cstm.field_31_0      = (NvU32)patternInfo->ctsmData[0];
            params.cstm.field_63_32     = (NvU32)patternInfo->ctsmData[4];
            params.cstm.field_95_64     = (NvU32)patternInfo->ctsmData[8];
            params.cstm.field_127_96    = (NvU32)patternInfo->ctsmData[12];
            params.cstm.field_159_128   = (NvU32)patternInfo->ctsmData[16];
            params.cstm.field_191_160   = (NvU32)patternInfo->ctsmData[20];
            params.cstm.field_223_192   = (NvU32)patternInfo->ctsmData[24];
            params.cstm.field_255_224   = (NvU32)patternInfo->ctsmData[28];
            params.cstm.field_263_256   = (NvU32)patternInfo->ctsmData[32];
            break;
        }
        default:
            DP_ASSERT(0 && "Unknown Phy Pattern");
            return false;
    }

    params.subDeviceInstance       = subdeviceIndex;
    params.displayId               = displayId;
    params.testPattern             = ctrlPattern;

    //
    // Set the appropriate laneMask based on the current lane count. The laneMask is used for GF119+ chips
    // only so it doesn't matter if we populate it for all chips. It is set to all lanes since
    // setting the test pattern on a lane that is off is effectively a nop.
    // The laneMask allows for setting the pattern on specific lanes to check for cross-talk, which is the
    // phenomenon of observing the signal crossing over to a different lane where it's not set.
    //
    params.laneMask                = 0xf;

    NvU32 code = provider->rmControl0073(NV0073_CTRL_CMD_DP_SET_TESTPATTERN, &params, sizeof(params));

    return (code == NVOS_STATUS_SUCCESS);
}

bool EvoMainLink2x::getUSBCCableIDInfo(NV0073_CTRL_DP_USBC_CABLEID_INFO *cableIDInfo)
{
    if (!cableIDInfo)
    {
        return false;
    }

    NV0073_CTRL_DP_USBC_CABLEID_INFO_PARAMS params = { 0 };

    // Setup input parameters for RM Control call to get details from PHY
    params.subDeviceInstance = this->subdeviceIndex;
    params.displayId = this->displayId;

    NvU32 code = provider->rmControl0073(NV0073_CTRL_CMD_DP_GET_CABLEID_INFO_FROM_MACRO, &params, sizeof(params));
    bool success = (code == NVOS_STATUS_SUCCESS);
    if (success)
    {
        cableIDInfo->uhbr10_0_capable   = params.cableIDInfo.uhbr10_0_capable;
        cableIDInfo->uhbr13_5_capable   = params.cableIDInfo.uhbr13_5_capable;
        cableIDInfo->uhbr20_0_capable   = params.cableIDInfo.uhbr20_0_capable;
        cableIDInfo->type               = params.cableIDInfo.type;
        cableIDInfo->vconn_source       = params.cableIDInfo.vconn_source;
    }

    return success;
}

