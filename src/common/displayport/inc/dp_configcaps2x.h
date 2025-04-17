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

/******************************* DisplayPort *******************************\
*                                                                           *
* Module: dp_configcaps2x.h                                                 *
*    Abstraction for DP2x caps registers                                    *
*                                                                           *
\***************************************************************************/

#ifndef INCLUDED_DP_CONFIGCAPS_2X_H
#define INCLUDED_DP_CONFIGCAPS_2X_H

#include "dp_configcaps.h"

namespace DisplayPort
{
    enum CableType
    {
        CableTypeUnknown = 0,
        CableTypePassive = 1,
        CableTypeLRD = 2,
        CableTypeActiveReTimer = 3,
        CableTypeOptical = 4,
    };

    struct DPCDHALImpl2x : DPCDHALImpl
    {
        struct
        {
            // 0x2206, if the sink supports 128b/132b
            bool      bDP2xChannelCodingSupported;
            // 0x2215
            bool      bUHBR_10GSupported;
            bool      bUHBR_13_5GSupported;
            bool      bUHBR_20GSupported;

            struct
            {
                // 0xF0006, if the PHY Repeater supports 128b/132b
                bool      bDP2xChannelCodingSupported;
                // 0xF0007
                bool      bUHBR_10GSupported;
                bool      bUHBR_13_5GSupported;
                bool      bUHBR_20GSupported;
            } repeaterCaps;

            struct
            {
                bool     bDP2xChannelCodingSupported;
                bool     bUHBR_10GSupported;
                bool     bUHBR_13_5GSupported;
                bool     bUHBR_20GSupported;
            } dpInTunnelingCaps;

            /* 
             * Cable capabilities determined on the sink end
             * read by source via DPCD
             */
            struct
            {
                bool      bUHBR_10GSupported;
                bool      bUHBR_13_5GSupported;
                bool      bUHBR_20GSupported;
                CableType cableType;
            } rxCableCaps;

            /* 
             * Cable capabilities determined on the source end
             */
            struct
            {
                bool      bIsSupported;
                bool      bUHBR_10GSupported;
                bool      bUHBR_13_5GSupported;
                bool      bUHBR_20GSupported;
                CableType cableType;
                bool      bVconnSource;
            } txCableCaps;
        } caps2x;

        struct
        {
            // DPCD offset 2230 - 2250
            NvU8      cstm264Bits[NV_DPCD20_TEST_264BIT_CUSTOM_PATTERN__SIZE];
            struct
            {
                bool                    testRequestPhyCompliance;       // DPCD offset 218
                LinkQualityPatternType  phyTestPattern;                 // DPCD offset 248
                //
                // Only valid when phyTestPattern is
                // LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_ON_DE_EMPHASIS_ON
                // LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_OFF_DE_EMPHASIS_ON
                // LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_ON_DE_EMPHASIS_OFF
                // LINK_QUAL_SQUARE_SEQ_WITH_PRESHOOT_OFF_DE_EMPHASIS_OFF
                //
                NvU32 sqNum;                                        // DPCD offset 249
            } testPhyCompliance2x;
        } interrupts2x;

        virtual AuxRetry::status            notifySDPErrDetectionCapability();
        virtual AuxRetry::status            setMainLinkChannelCoding(MainLinkChannelCoding channelCoding);
        virtual MainLinkChannelCoding       getMainLinkChannelCoding();

        void                        performCableIdHandshake();
        void                        performCableIdHandshakeForTypeC();
        void                        parseAndSetCableId(NvU8 cableId);

        virtual void                setGpuDPSupportedVersions(NvU32 _gpuDPSupportedVersions);
        virtual bool                isDp2xChannelCodingCapable();
        virtual void                parseAndReadCaps();
        virtual LinkRate            getMaxLinkRate();
        virtual NvU32               getUHBRSupported();
        virtual void                setIgnoreCableIdCaps(bool bIgnore){ bIgnoreCableIdCaps = bIgnore; }
        virtual void                overrideCableIdCap(LinkRate linkRate, bool bEnable);

        void                        setCableVconnSourceUnknown()
        {
            bCableVconnSourceUnknown = true;
        }
        void                        resetTxCableCaps()
        {
            caps2x.txCableCaps.bUHBR_10GSupported = true;
            caps2x.txCableCaps.bUHBR_13_5GSupported = true;
            caps2x.txCableCaps.bUHBR_20GSupported = true;
            caps2x.txCableCaps.bVconnSource = true;
            caps2x.txCableCaps.bIsSupported = false;
        }
        virtual void                setUSBCCableIDInfo(NV0073_CTRL_DP_USBC_CABLEID_INFO *cableIDInfo);

        virtual bool                parseTestRequestPhy();
        virtual bool                parseTestRequestTraining(NvU8 * buffer);
        // DPCD offset 2230 - 2250
        virtual void                get264BitsCustomTestPattern(NvU8 *testPattern);
        // DPCD offset 249h
        virtual void                getSquarePatternNum(NvU8 *sqNum);
        virtual AuxRetry::status    setLinkQualLaneSet(unsigned lane, LinkQualityPatternType linkQualPattern);
        virtual AuxRetry::status    setLinkQualPatternSet(DP2xPatternInfo& patternInfo, unsigned laneCount);

        // class fields that need re-initialization
        bool bIgnoreCableIdCaps;
        bool bConnectorIsTypeC;
        bool bCableVconnSourceUnknown;
        
        virtual void initialize()
        {
            setIgnoreCableIdCaps(false);
        }

        virtual void setConnectorTypeC(bool bTypeC);

        DPCDHALImpl2x(AuxBus * bus, Timer * timer) : DPCDHALImpl(bus, timer), bIgnoreCableIdCaps(false), bConnectorIsTypeC(false)
        {
            dpMemZero(&caps2x, sizeof(caps2x));
            dpMemZero(&interrupts2x, sizeof(interrupts2x));

            caps2x.bDP2xChannelCodingSupported = true;
            caps2x.bUHBR_10GSupported = true;
            caps2x.bUHBR_13_5GSupported = true;
            caps2x.bUHBR_20GSupported = true;

            caps2x.rxCableCaps.bUHBR_10GSupported = true;
            caps2x.rxCableCaps.bUHBR_13_5GSupported = true;
            caps2x.rxCableCaps.bUHBR_20GSupported = true;

            // txCableCaps are set only if data from connector
            // is available to be read
            resetTxCableCaps();
        };
    };
}
#endif //INCLUDED_DP_CONFIGCAPS_2X_H
