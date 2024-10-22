/*
 * SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
    return lanes != laneCount_0;
}

LinkConfiguration::LinkConfiguration(LinkPolicy * p, unsigned lanes, LinkRate peakRate,
                                     bool enhancedFraming, bool MST, bool disablePostLTRequest,
                                     bool bEnableFEC, bool bDisableLTTPR)
    : lanes(lanes), peakRatePossible(peakRate), peakRate(peakRate), enhancedFraming(enhancedFraming),
      multistream(MST), disablePostLTRequest(disablePostLTRequest), bEnableFEC(bEnableFEC),
      bDisableLTTPR(bDisableLTTPR), linkTrainCounter(0)
{
    // downrate for spread and FEC
    minRate = linkOverhead(peakRate);
    if (p)
    {
        policy = *p;
    }
}

