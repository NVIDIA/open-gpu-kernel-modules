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
* Module: dp_linkconfig.cpp                                                 *
*    DP LinkConfiguration implementation                                    *
*                                                                           *
\***************************************************************************/


#include "dp_linkconfig.h"
#include "dp_regkeydatabase.h"

using namespace DisplayPort;
bool LinkConfiguration::lowerConfig(bool bReduceLaneCnt)
{
    //
    // TODO: bReduceLaneCnt is set to fallback to 4 lanes with lower
    //       valid link rate. But we should reset to max lane count
    //       sink supports instead.
    //

    LinkRate lowerRate = policy.getLinkRates()->getLowerRate(peakRate);

    if(bReduceLaneCnt)
    {
        // Reduce laneCount before reducing linkRate
        if(lanes == laneCount_1)
        {
            if (lowerRate)
            {
                lanes = laneCount_4;
                peakRate = lowerRate;
            }
            else
            {
                lanes = laneCount_0;
            }
        }
        else
        {
            lanes /= 2;
        }
    }
    else
    {
        // Reduce the link rate instead of lane count
        if (lowerRate)
        {
            peakRate = lowerRate;
        }
        else
        {
            lanes /= 2;
        }
    }

    minRate = linkOverhead(peakRate);
    setChannelCoding();
    return lanes != laneCount_0;
}

LinkConfiguration::LinkConfiguration(LinkPolicy * p, unsigned lanes, LinkRate peakRate,
                                     bool enhancedFraming, bool MST, bool disablePostLTRequest, 
                                     bool bEnableFEC, bool bDisableLTTPR, bool bDisableDownspread)
    : lanes(lanes), peakRatePossible(peakRate), peakRate(peakRate),
      enhancedFraming(enhancedFraming), multistream(MST),
      disablePostLTRequest(disablePostLTRequest), bEnableFEC(bEnableFEC),
      bDisableLTTPR(bDisableLTTPR), bDisableDownspread(bDisableDownspread),
      linkTrainCounter(0)
{
    // downrate for spread and FEC
    minRate = linkOverhead(peakRate);
    if (p)
    {
        policy = *p;
    }
    setChannelCoding();
}

void LinkConfiguration::setChannelCoding()
{
    bool b128b132bHBREnabled = ((bSupportInternalUhbrOnFpga & NV_DP2X_REGKEY_FPGA_UHBR_SUPPORT_2_7G) != 0);
    if (IS_DP2_X_UHBR_LINKBW(peakRate) ||
       (peakRate == dp2LinkRate_2_70Gbps && b128b132bHBREnabled))
    {
        bIs128b132bChannelCoding = true;
        // FEC is always enabled for 128b/132b.
        bEnableFEC = true;
    }
    else
    {
        bIs128b132bChannelCoding = false;
    }
}

NvU64 LinkConfiguration::getBytesPerTimeslot()
{
    NvU64 bytes_per_timeslot;

    // spread is already considered during pbn calculation for required mode. No need to consider here
    if(bIs128b132bChannelCoding)
    {
        // 128b/132b case
        NvU64 linkRateBytes = DP_LINK_RATE_BITSPS_TO_BYTESPS(DP_LINK_RATE_10M_TO_BPS(peakRate) * lanes);
        NvU64 total_data_bw = DATA_BW_EFF_128B_132B(linkRateBytes); // Unit: Bps (bytes)
        bytes_per_timeslot = divide_floor(total_data_bw, 64);
    }
    else
    {
        // 8b/10b case
        bytes_per_timeslot = getTotalDataRate() / 64;
    }

    return bytes_per_timeslot;
}

NvU32 LinkConfiguration::PBNForSlots(NvU32 slots)
{
    NvU64 bytes_per_pbn      = 54 * 1000000 / 64;
    NvU64 bytes_per_timeslot = getBytesPerTimeslot();

    return (NvU32)(bytes_per_timeslot * slots/ bytes_per_pbn); // Rounded down
}

NvU32 LinkConfiguration::slotsForPBN(NvU32 allocatedPBN, bool usable)
{
    NvU64 bytes_per_pbn      = 54 * 1000000 / 64;
    NvU64 bytes_per_timeslot = getBytesPerTimeslot();
    NvU32 slots;

    if (bytes_per_timeslot == 0)
        return (NvU32)-1;

    if (usable)
    {
        // round down to find the usable integral slots for a given value of PBN.
        slots = (NvU32)divide_floor(allocatedPBN * bytes_per_pbn, bytes_per_timeslot);
        DP_ASSERT(slots <= 64);
    }
    else
    {
        slots = (NvU32)divide_ceil(allocatedPBN * bytes_per_pbn, bytes_per_timeslot);
    }

    return slots;
}

void LinkConfiguration::pbnRequired(const ModesetInfo & modesetInfo, unsigned & base_pbn, unsigned & slots, unsigned & slots_pbn)
{

    if (bIs128b132bChannelCoding)
    {
        base_pbn = pbnForMode(modesetInfo, false);
    }
    else
    {
        base_pbn = pbnForMode(modesetInfo, true);
    }

    if (!bIs128b132bChannelCoding)
    {
    if (bEnableFEC)
    {
        // IF FEC is enabled, we need to consider 2.4% overhead as per DP1.4 spec.
        base_pbn = (NvU32)(divide_ceil(base_pbn * 1000, 976));
    }
    }

    slots = slotsForPBN(base_pbn);
    slots_pbn = PBNForSlots(slots);
}
