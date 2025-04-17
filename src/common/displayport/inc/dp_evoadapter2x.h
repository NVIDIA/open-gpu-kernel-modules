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

/******************************* List **************************************\
*                                                                           *
* Module: dp_evoadapter2x.h                                                 *
*    Interface for low level access to the aux bus for dp2x.                *
*    This is the synchronous version of the interface.                      *
*                                                                           *
\***************************************************************************/
#ifndef INCLUDED_DP_EVOADAPTER2X_H
#define INCLUDED_DP_EVOADAPTER2X_H

#include "dp_evoadapter.h"
#include "dp_timeout.h"
#include "dp_linkconfig.h"

// For channel equalization, total poll time is 450ms. Set it to 500ms for timer granularity
#define NV_DP2X_LT_MAX_TIME_POLL_CHNL_EQ_MS                      (500U)
// For channel equalization, max loop count is 20 when waiting CHANNEL_EQ_DONE set.
#define NV_DP2X_LT_MAX_LOOP_COUNT_POLL_CHNL_EQ_DONE               (20U)
// For channel equalization, polling interval is 3 when waiting INTERLANE_ALIGN_DONE.
#define NV_DP2X_LT_CHNL_EQ_INTERLANE_ALIGN_POLLING_INTERVAL        (3U)
// Unit definitions for Aux Read Intervals
#define NV_DP2X_LT_CHNL_EQ_TRAINING_AUX_RD_INTERVAL_UNIT_1MS       (1U)
#define NV_DP2X_LT_CHNL_EQ_TRAINING_AUX_RD_INTERVAL_UNIT_2MS       (2U)


// For phase CDS, total time is (LTTPR + 1) * 20
#define NV_DP2X_LT_MAX_POLL_TIME_CDS_MS(i)                 ((i+1) * 20)
#define NV_DP2X_LT_CDS_POLLING_INTERVAL                             3U

//
// Total possible link configurations for DP2.x fallback table.
// Total number of link rates supported is 15:
//  3 UHBR:      UHBR20, UHBR13.5, UHBR10
//  3 UHBR for internal test:
//               UHBR2.5, UHBR2.7, UHBR5.0
//  4 Legacy:    HBR3, HBR2, HBR, RBR
//  5 ILR:       2.16G, 2.43G, 3.24G, 4.32G, 6.75G
// Total number of lane count supported is 3 (1, 2, 4)
// Total possible link configurations => 15 * 3 = 45.
//
#define NV_DP2X_VALID_LINK_CONFIGURATION_COUNT                     45U

namespace DisplayPort
{
    // Simplified LinkConfiguration for fallback map
    typedef struct
    {
        NvU32   laneCount;

        // 10M unit
        LinkRate linkRate;

        //
        // The flag indicates if the link configuration is avaiable on the setup.
        // Both link count and link rate have to be supported.
        //
        bool  bSupported;

        //
        // The flag indicates if the link rate can be trained with 128b/132b channel
        // coding. Default false for non-UHBR link rates.
        //
        bool  bUseDP2xChannelCoding;
    } SIMPLIFIED_DP2X_LINKCONFIG;

    typedef struct
    {
        DP2X_RESET_LINK_REASON  reason;
        bool                    bForce;
        bool                    bSkipLt;
    } DP2XResetParam;

    class EvoMainLink2x : public EvoMainLink
    {
        //
        // Bit mask for GPU/DFP supported UHBR Link Rates.
        // Defines the same as NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS.UHBRSupportedByGpu and
        //                     NV0073_CTRL_DFP_GET_INFO_PARAMS.UHBRSupportedByDfp
        //
        NvU32   gpuUhbrCaps;
        NvU32   dfpUhbrCaps;
        bool    bUseRgFlushSequence;

        bool    bSupportUHBR2_50;               // Support UHBR2.5 for internal testing.
        bool    bSupportUHBR2_70;               // Support UHBR2.7 for internal testing.
        bool    bSupportUHBR5_00;               // Support UHBR5.0 for internal testing.
        bool    bEnable5147205Fix;              // Enable 5147205 fix.

        bool    bConnectorIsUSBTypeC;
        bool    bCableVconnSourceUnknown;

        // Start time of DP2.x LT Channel Eqaulization phase.
        NvU64   channelEqualizationStartTimeUs;

        bool    pollDP2XLinkTrainingStageDone(NvU32 stage, NvU32 laneCount,
                                              NvU32 phyRepeaterCount, NvU32 pollingInterval, bool force);

        //
        // Find next available link configuration in fallback mandate.
        // Return false if not available
        //
        bool  getFallbackForDP2xLinkTraining(LinkConfiguration *link);

        //
        // Check if the link configuration is supported on the system,
        // and with correct channel encoding.
        // Return false if not.
        //
        virtual bool  isSupportedDPLinkConfig(LinkConfiguration &link);

        // Before link training start, reset DPRX link and make sure it's ready.
        bool  resetDPRXLink(DP2XResetParam param);

        SIMPLIFIED_DP2X_LINKCONFIG fallbackMandateTable[NV_DP2X_VALID_LINK_CONFIGURATION_COUNT];
        NV0073_CTRL_CMD_DP2X_LINK_TRAINING_CTRL_PARAMS ltRmParams;

    public:
        virtual bool queryGPUCapability();
        virtual bool queryAndUpdateDfpParams();
        virtual void updateFallbackMap(NvU32 maxLaneCount, LinkRate maxLinkRate, NvU32 uhbrCaps = 0);
        virtual bool configureLinkRateTable(const NvU16 *pLinkRateTable, LinkRates *pLinkRates);
        void validateIlrInFallbackMap(LinkRate ilr, bool bUseDP2xChannelCoding);

        virtual NvU32 maxLinkRateSupported();
        virtual bool setFlushMode(FlushModePhase phase);
        virtual bool clearFlushMode(FlushModePhase phase, NvU32 attachFailedHeadMask = 0, NvU32 headIndex = 0);
        virtual bool isRgFlushSequenceUsed() {return bUseRgFlushSequence;}
        void applyDP2xRegkeyOverrides();
        virtual NvU32 headToStream(NvU32 head, bool bSidebandMessageSupported,
                                   DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID streamIdentifier = DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID_PRIMARY);

        // Link Rate will return the value with 10M convention!
        virtual void getLinkConfig(unsigned &laneCount, NvU64 & linkRate);

        virtual NvU32 getUHBRSupported(void)
        {
            return (gpuUhbrCaps & dfpUhbrCaps);
        }

        virtual bool train(const LinkConfiguration & link, bool force,
                           LinkTrainingType linkTrainingType,
                           LinkConfiguration *retLink, bool bSkipLt = false,
                           bool isPostLtAdjRequestGranted = false,
                           unsigned phyRepeaterCount = 0);

        bool trainDP2xChannelCoding(LinkConfiguration & link, bool force,
                                    LinkTrainingType linkTrainingType,
                                    LinkConfiguration *retLink,
                                    bool bSkipLt, bool isPostLtAdjRequestGranted,
                                    unsigned phyRepeaterCount);

        EvoMainLink2x(EvoInterface *provider, Timer * timer);
        virtual bool getDp2xLaneData(NvU32 *numLanes, NvU32 *data);
        virtual bool setDp2xLaneData(NvU32 numLanes, NvU32 *data);
        virtual bool physicalLayerSetDP2xTestPattern(DP2xPatternInfo *patternInfo);

        virtual bool isConnectorUSBTypeC()
        {
            return bConnectorIsUSBTypeC;
        }
        virtual bool isCableVconnSourceUnknown()
        {
            return bCableVconnSourceUnknown;
        }
        virtual void invalidateLinkRatesInFallbackTable(const LinkRate linkRate);
        virtual bool getUSBCCableIDInfo(NV0073_CTRL_DP_USBC_CABLEID_INFO *cableIDInfo);
    };

}
#endif // INCLUDED_DP_EVOADAPTER_H
