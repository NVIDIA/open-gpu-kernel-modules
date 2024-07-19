/*
 * SPDX-FileCopyrightText: Copyright (c) 2020-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
* Module: dp_regkeydatabase.h                                               *
*    Definition of the DP_REGKEY_DATABASE                                   *
*                                                                           *
\***************************************************************************/

#ifndef INCLUDED_DP_REGKEYDATABASE_H
#define INCLUDED_DP_REGKEYDATABASE_H

#include "dp_auxdefs.h"

// Regkey Names
#define NV_DP_REGKEY_OVERRIDE_DPCD_REV                "OVERRIDE_DPCD_REV"
#define NV_DP_REGKEY_DISABLE_SSC                      "DISABLE_SSC"
#define NV_DP_REGKEY_ENABLE_FAST_LINK_TRAINING        "ENABLE_FAST_LINK_TRAINING"
#define NV_DP_REGKEY_DISABLE_MST                      "DISABLE_MST"
#define NV_DP_REGKEY_ENABLE_INBAND_STEREO_SIGNALING   "ENABLE_INBAND_STEREO_SIGNALING"
#define NV_DP_REGKEY_SKIP_POWEROFF_EDP_IN_HEAD_DETACH "SKIP_POWEROFF_EDP_IN_HEAD_DETACH"
#define NV_DP_REGKEY_ENABLE_OCA_LOGGING               "ENABLE_OCA_LOGGING"
#define NV_DP_REGKEY_REPORT_DEVICE_LOST_BEFORE_NEW    "HP_WAR_1707690"
#define NV_DP_REGKEY_APPLY_LINK_BW_OVERRIDE_WAR       "APPLY_LINK_BW_OVERRIDE_WAR"
// For DP2x, the regkey value needs to be in 10M convention
#define NV_DP_REGKEY_APPLY_MAX_LINK_RATE_OVERRIDES    "APPLY_OVERRIDES_FOR_BUG_2489143"
#define NV_DP_REGKEY_DISABLE_DSC                      "DISABLE_DSC"
#define NV_DP_REGKEY_SKIP_ASSESSLINK_FOR_EDP          "HP_WAR_2189772"
#define NV_DP_REGKEY_HDCP_AUTH_ONLY_ON_DEMAND         "DP_HDCP_AUTH_ONLY_ON_DEMAND"
#define NV_DP_REGKEY_ENABLE_MSA_OVER_MST              "ENABLE_MSA_OVER_MST"

// Keep link alive for SST and MST
#define NV_DP_REGKEY_KEEP_OPT_LINK_ALIVE              "DP_KEEP_OPT_LINK_ALIVE"
// Keep link alive when connector is in MST
#define NV_DP_REGKEY_KEEP_OPT_LINK_ALIVE_MST          "DP_KEEP_OPT_LINK_ALIVE_MST"
// Keep link alive when connector is in SST
#define NV_DP_REGKEY_KEEP_OPT_LINK_ALIVE_SST          "DP_KEEP_OPT_LINK_ALIVE_SST"

#define NV_DP_REGKEY_FORCE_EDP_ILR                    "DP_BYPASS_EDP_ILR_REV_CHECK"

// Message to power down video stream before power down link (set D3)
#define NV_DP_REGKEY_POWER_DOWN_PHY                   "DP_POWER_DOWN_PHY"

//
// Regkey to re-assess max link if the first assessed link config
// is lower than the panel max
//
#define NV_DP_REGKEY_REASSESS_MAX_LINK                "DP_REASSESS_MAX_LINK"

//
// DSC capability of downstream device should be decided based on device's own
// and its parent's DSC capability.
//
#define NV_DP_DSC_MST_CAP_BUG_3143315                  "DP_DSC_MST_CAP_BUG_3143315"


// Bug 4388987 : This regkey will disable reading PCON caps for MST.
#define NV_DP_REGKEY_MST_PCON_CAPS_READ_DISABLED       "DP_BUG_4388987_WAR"

// Bug 4426624: Flush timeslot change to HW when dirty bit is set.
#define NV_DP_REGKEY_FLUSH_TIMESLOT_INFO_WHEN_DIRTY    "DP_BUG_4426624_WAR"

#define NV_DP_REGKEY_DISABLE_TUNNEL_BW_ALLOCATION      "DP_DISABLE_TUNNEL_BW_ALLOCATION"

//
// Data Base used to store all the regkey values.
// The actual data base is declared statically in dp_evoadapter.cpp.
// All entries set to 0 before initialized by the first EvoMainLink constructor.
// The first EvoMainLink constructor will populate that data base.
// Later EvoMainLink will use values from that data base.
//
struct DP_REGKEY_DATABASE
{
    bool  bInitialized; // set to true after the first EvoMainLink instance is constructed
    // Below are regkey values
    NvU32 dpcdRevOveride;
    bool  bSscDisabled;
    bool  bFastLinkTrainingEnabled;
    bool  bMstDisabled;
    bool  bInbandStereoSignalingEnabled;
    bool  bPoweroffEdpInHeadDetachSkipped;
    bool  bOcaLoggingEnabled;
    bool  bReportDeviceLostBeforeNew;
    bool  bLinkBwOverrideWarApplied;
    NvU32 applyMaxLinkRateOverrides;
    bool  bDscDisabled;
    bool  bAssesslinkForEdpSkipped;
    bool  bHdcpAuthOnlyOnDemand;
    bool  bMsaOverMstEnabled;
    bool  bOptLinkKeptAlive;
    bool  bOptLinkKeptAliveMst;
    bool  bOptLinkKeptAliveSst;
    bool  bBypassEDPRevCheck;
    bool  bDscMstCapBug3143315;
    bool  bPowerDownPhyBeforeD3;
    bool  bReassessMaxLink;
    bool  bMSTPCONCapsReadDisabled;
    bool  bFlushTimeslotWhenDirty;
    bool  bForceDisableTunnelBwAllocation;
};

extern struct DP_REGKEY_DATABASE dpRegkeyDatabase;

#endif //INCLUDED_DP_REGKEYDATABASE_H

