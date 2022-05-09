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
* Module: dp_wardatabase.h                                                  *
*         EDID and OUI based workarounds for panel/TCON issues              *
*                                                                           *
\***************************************************************************/

#ifndef INCLUDED_DP_WARDATABASE_H
#define INCLUDED_DP_WARDATABASE_H

#include "dp_object.h"

namespace DisplayPort
{
    #define WAR_MAX_REASSESS_ATTEMPT 3
    #define WAR_MAX_RETRAIN_ATTEMPT 3

    typedef enum
    {
        DP_MONITOR_CAPABILITY_DP_SKIP_REDUNDANT_LT                          =   (1 << 0), // Do not train if the link B/W and lane count are already set to the desired quantities
        DP_MONITOR_CAPABILITY_DP_SKIP_CABLE_BW_CHECK                        =   (1 << 1), // Skip the link training attempts to test cable bandwidth in CheckDpLink
        DP_MONITOR_CAPABILITY_DP_MULTI_WRITE_DPCD_0x600                     =   (1 << 2), // Repeatedly write 0x1 to 0x600 with extra delays until the read verifies the write
        DP_MONITOR_CAPABILITY_DP_WRITE_0x600_BEFORE_LT                      =   (1 << 3), // Power on a monitor before every link training
        DP_MONITOR_CAPABILITY_DP_OVERRIDE_OPTIMAL_LINK_CONFIG               =   (1 << 4), // Override optimal link config
        DP_MONITOR_CAPABILITY_DP_OVERRIDE_MAX_LANE_COUNT                    =   (1 << 5), // WAR for some DP monitors which claims more lane count than it really supports. It may generate interrupt storm if unsupported lane count is applied
        DP_MONITOR_CAPABILITY_DP_AVOID_UPDATE_POWER_STATE                   =   (1 << 6), // Don't update panel power state when head detach or lid closed
    } DP_MONITOR_CAPABILITY; 

    struct DpMonitorDenylistData: virtual public Object
    {
        // Max lane count supported override value
        unsigned int dpMaxLaneCountOverride;

        // Link rate and Lane count value overrides
        // when we need to skip BW check
        struct
        {
            unsigned int   maxLaneAtHighRate;
            unsigned int   maxLaneAtLowRate;
        } dpSkipCheckLink;

        // Link rate and Lane count value overrides
        // when we need to force optimal link config
        struct
        {
            unsigned int   linkRate;
            unsigned int   laneCount;
        } dpOverrideOptimalLinkConfig;
    };
}

#endif // INCLUDED_DP_WARDATABASE_H
