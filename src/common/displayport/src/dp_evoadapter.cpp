/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
* Module: dp_evoadapter.cpp                                                 *
*    Interface for low level access to the aux bus.                         *
*    This is the synchronous version of the interface.                      *
*                                                                           *
\***************************************************************************/
#include "dp_internal.h"
#include "dp_evoadapter.h"
#include "dp_auxdefs.h"
#include "dp_tracing.h"
#include "dp_vrr.h"
#include "dp_printf.h"
#include <nvmisc.h>

#include <ctrl/ctrl0073/ctrl0073specific.h>
#include <ctrl/ctrl0073/ctrl0073system.h>
#include <ctrl/ctrl5070/ctrl5070or.h>

#include "displayport2x.h"
#include "dp_evoadapter2x.h"

using namespace DisplayPort;

//
//  Evo hardcodes the relationship between stream and head #
//    Head#x is always stream x+1
//
#define STREAM_TO_HEAD_ID(s) ((s) - 1)
#define HEAD_TO_STREAM_ID(s) ((s) + 1)

//
// Data Base used to store all the regkey values.
// The type is defined in dp_regkeydatabase.h.
// All entries set to 0 before initialized by the first EvoMainLink constructor.
// The first EvoMainLink constructor will populate that data base.
// Later EvoMainLink will use values from that data base.
//
struct DP_REGKEY_DATABASE dpRegkeyDatabase = {0};

enum DP_REG_VAL_TYPE
{
    DP_REG_VAL_BOOL = 0,
    DP_REG_VAL_U32  = 1,
    DP_REG_VAL_U16  = 2,
    DP_REG_VAL_U8   = 3
};

const struct
{
    const char* pName;
    void* pValue;
    DP_REG_VAL_TYPE valueType;
} DP_REGKEY_TABLE [] =
{
    {NV_DP_REGKEY_OVERRIDE_DPCD_REV,                &dpRegkeyDatabase.dpcdRevOveride,                  DP_REG_VAL_U32},
    {NV_DP_REGKEY_DISABLE_SSC,                      &dpRegkeyDatabase.bSscDisabled,                    DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_ENABLE_FAST_LINK_TRAINING,        &dpRegkeyDatabase.bFastLinkTrainingEnabled,        DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_DISABLE_MST,                      &dpRegkeyDatabase.bMstDisabled,                    DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_ENABLE_INBAND_STEREO_SIGNALING,   &dpRegkeyDatabase.bInbandStereoSignalingEnabled,   DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_SKIP_POWEROFF_EDP_IN_HEAD_DETACH, &dpRegkeyDatabase.bPoweroffEdpInHeadDetachSkipped, DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_ENABLE_OCA_LOGGING,               &dpRegkeyDatabase.bOcaLoggingEnabled,              DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_REPORT_DEVICE_LOST_BEFORE_NEW,    &dpRegkeyDatabase.bReportDeviceLostBeforeNew,      DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_APPLY_LINK_BW_OVERRIDE_WAR,       &dpRegkeyDatabase.bLinkBwOverrideWarApplied,       DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_APPLY_MAX_LINK_RATE_OVERRIDES,    &dpRegkeyDatabase.applyMaxLinkRateOverrides,       DP_REG_VAL_U32},
    {NV_DP_REGKEY_DISABLE_DSC,                      &dpRegkeyDatabase.bDscDisabled,                    DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_SKIP_ASSESSLINK_FOR_EDP,          &dpRegkeyDatabase.bAssesslinkForEdpSkipped,        DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_HDCP_AUTH_ONLY_ON_DEMAND,         &dpRegkeyDatabase.bHdcpAuthOnlyOnDemand,           DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_ENABLE_MSA_OVER_MST,              &dpRegkeyDatabase.bMsaOverMstEnabled,              DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_KEEP_OPT_LINK_ALIVE,              &dpRegkeyDatabase.bOptLinkKeptAlive,               DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_KEEP_OPT_LINK_ALIVE_MST,          &dpRegkeyDatabase.bOptLinkKeptAliveMst,            DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_KEEP_OPT_LINK_ALIVE_SST,          &dpRegkeyDatabase.bOptLinkKeptAliveSst,            DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_FORCE_EDP_ILR,                    &dpRegkeyDatabase.bBypassEDPRevCheck,              DP_REG_VAL_BOOL},
    {NV_DP_DSC_MST_CAP_BUG_3143315,                 &dpRegkeyDatabase.bDscMstCapBug3143315,            DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_POWER_DOWN_PHY,                   &dpRegkeyDatabase.bPowerDownPhyBeforeD3,           DP_REG_VAL_BOOL},
    {NV_DP2X_REGKEY_FPGA_UHBR_SUPPORT,              &dpRegkeyDatabase.supportInternalUhbrOnFpga,       DP_REG_VAL_U32},
    {NV_DP2X_IGNORE_CABLE_ID_CAPS,                  &dpRegkeyDatabase.bIgnoreCableIdCaps,              DP_REG_VAL_BOOL},
    {NV_DP2X_REGKEY_DISABLE_EFF_BPP_SST_8b10b,      &dpRegkeyDatabase.bDisableEffBppSST8b10b,          DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_MST_PCON_CAPS_READ_DISABLED,      &dpRegkeyDatabase.bMSTPCONCapsReadDisabled,        DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_DISABLE_TUNNEL_BW_ALLOCATION,     &dpRegkeyDatabase.bForceDisableTunnelBwAllocation, DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_DISABLE_DOWNSPREAD,               &dpRegkeyDatabase.bDownspreadDisabled,             DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_DISABLE_AVOID_HBR3_WAR,           &dpRegkeyDatabase.bDisableAvoidHBR3War,            DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_SKIP_ZERO_OUI_CACHE,              &dpRegkeyDatabase.bSkipZeroOuiCache,               DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_ENABLE_FIX_FOR_5147205,           &dpRegkeyDatabase.bEnable5147205Fix,               DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_FORCE_HEAD_SHUTDOWN,              &dpRegkeyDatabase.bForceHeadShutdown,              DP_REG_VAL_BOOL},
    {NV_DP_REGKEY_ENABLE_LOWER_BPP_CHECK_FOR_DSC,   &dpRegkeyDatabase.bEnableLowerBppCheckForDsc,      DP_REG_VAL_BOOL}
};

EvoMainLink::EvoMainLink(EvoInterface * provider, Timer * timer) :
                         provider(provider),
                         timer(timer),
                         displayId(provider->getDisplayId()),
                         subdeviceIndex(provider->getSubdeviceIndex())
{
    //
    //     Process GPU caps  (This needs to be replaced with a control call caps interface)
    //
    NvU32 code;

    // Initialize shared regkey data base, and apply the overrides
    this->initializeRegkeyDatabase();
    this->applyRegkeyOverrides();

    _isDynamicMuxCapable       = false;
    _isLTPhyRepeaterSupported  = true;
    _rmPhyRepeaterCount        = 0;
    dpMemZero(&_DSC, sizeof(_DSC));
    dpMemZero(&dfpParams, sizeof(dfpParams));
    dpMemZero(&dpParams, sizeof(dpParams));

    //
    //  Tell RM to hands off on the DisplayPort hardware
    //
    NV0073_CTRL_CMD_DP_SET_MANUAL_DISPLAYPORT_PARAMS setManualParams = {0};
    setManualParams.subDeviceInstance = subdeviceIndex;
    code = provider->rmControl0073(NV0073_CTRL_CMD_DP_SET_MANUAL_DISPLAYPORT, &setManualParams, sizeof setManualParams);
    DP_ASSERT (code == NVOS_STATUS_SUCCESS && "Unable to enable library mode");

    //
    //  Get the mask of valid heads
    //
    NV0073_CTRL_SPECIFIC_GET_ALL_HEAD_MASK_PARAMS allHeadMaskParams;
    dpMemZero(&allHeadMaskParams, sizeof allHeadMaskParams);
    allHeadMaskParams.subDeviceInstance = subdeviceIndex;
    code = provider->rmControl0073(NV0073_CTRL_CMD_SPECIFIC_GET_ALL_HEAD_MASK, &allHeadMaskParams, sizeof(allHeadMaskParams));

    if (code != NVOS_STATUS_SUCCESS)
    {
        DP_ASSERT(0 && "Unable to get head mask");
        allHeadMask = 3;
    }
    else
    {
        allHeadMask = allHeadMaskParams.headMask;
    }
}


bool EvoMainLink::vrrRunEnablementStage(unsigned stage, NvU32 *status)
{
    NV0073_CTRL_CMD_DP_ENABLE_VRR_PARAMS  params = {0};
    params.subDeviceInstance = subdeviceIndex;
    params.displayId = this->displayId;

    switch (stage)
    {
        case VRR_ENABLE_STAGE_MONITOR_ENABLE_BEGIN:
            params.cmd |= DRF_DEF(0073_CTRL_DP_CMD, _ENABLE_VRR_CMD, _STAGE, _MONITOR_ENABLE_BEGIN);
            break;
        case VRR_ENABLE_STAGE_MONITOR_ENABLE_CHALLENGE:
            params.cmd |= DRF_DEF(0073_CTRL_DP_CMD, _ENABLE_VRR_CMD, _STAGE, _MONITOR_ENABLE_CHALLENGE);
            break;
        case VRR_ENABLE_STAGE_MONITOR_ENABLE_CHECK:
            params.cmd |= DRF_DEF(0073_CTRL_DP_CMD, _ENABLE_VRR_CMD, _STAGE, _MONITOR_ENABLE_CHECK);
            break;
        case VRR_ENABLE_STAGE_DRIVER_ENABLE_BEGIN:
            params.cmd |= DRF_DEF(0073_CTRL_DP_CMD, _ENABLE_VRR_CMD, _STAGE, _DRIVER_ENABLE_BEGIN);
            break;
        case VRR_ENABLE_STAGE_DRIVER_ENABLE_CHALLENGE:
            params.cmd |= DRF_DEF(0073_CTRL_DP_CMD, _ENABLE_VRR_CMD, _STAGE, _DRIVER_ENABLE_CHALLENGE);
            break;
        case VRR_ENABLE_STAGE_DRIVER_ENABLE_CHECK:
            params.cmd |= DRF_DEF(0073_CTRL_DP_CMD, _ENABLE_VRR_CMD, _STAGE, _DRIVER_ENABLE_CHECK);
            break;
        case VRR_ENABLE_STAGE_RESET_MONITOR:
            params.cmd |= DRF_DEF(0073_CTRL_DP_CMD, _ENABLE_VRR_CMD, _STAGE, _RESET_MONITOR);
            break;
        case VRR_ENABLE_STAGE_INIT_PUBLIC_INFO:
            params.cmd |= DRF_DEF(0073_CTRL_DP_CMD, _ENABLE_VRR_CMD, _STAGE, _INIT_PUBLIC_INFO);
            break;
        case VRR_ENABLE_STAGE_GET_PUBLIC_INFO:
            params.cmd |= DRF_DEF(0073_CTRL_DP_CMD, _ENABLE_VRR_CMD, _STAGE, _GET_PUBLIC_INFO);
            break;
        case VRR_ENABLE_STAGE_STATUS_CHECK:
            params.cmd |= DRF_DEF(0073_CTRL_DP_CMD, _ENABLE_VRR_CMD, _STAGE, _STATUS_CHECK);
            break;
        default:
            DP_ASSERT(0 && "Undefined VRR Enablement Stage.");
            return false;
    }
    NvU32 retVal = provider->rmControl0073(NV0073_CTRL_CMD_DP_ENABLE_VRR, &params, sizeof(params));
    if (status)
    {
        *status = params.result;
    }
    if (retVal != NVOS_STATUS_SUCCESS)
    {
        return false;
    }
    return true;
}

bool EvoMainLink::getEdpPowerData(bool *panelPowerOn, bool *dpcdPowerStateD0)
{
    NV0073_CTRL_DP_GET_EDP_DATA_PARAMS  params;
    params.subDeviceInstance = subdeviceIndex;
    params.displayId = this->displayId;
    NvU32 code = provider->rmControl0073(NV0073_CTRL_CMD_DP_GET_EDP_DATA, &params, sizeof(params));

    if (code != NVOS_STATUS_SUCCESS)
    {
        DP_ASSERT(0 && "Unable to get eDP power data, assuming panel off.");
        if (panelPowerOn)
        {
            *panelPowerOn = false;
        }
        if (dpcdPowerStateD0)
        {
            *dpcdPowerStateD0 = false;
        }
        return false;
    }
    else
    {
        if (panelPowerOn)
        {
            *panelPowerOn = FLD_TEST_DRF(0073_CTRL_DP, _GET_EDP_DATA, _PANEL_POWER, _ON,
                                         params.data);
        }
        if (dpcdPowerStateD0)
        {
            *dpcdPowerStateD0 = FLD_TEST_DRF(0073_CTRL_DP, _GET_EDP_DATA, _DPCD_POWER_STATE, _D0,
                                           params.data);
        }
        return true;
    }
}

NvU32 EvoMainLink::headToStream(NvU32 head, bool bSidebandMessageSupported,
                                DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID streamIdentifier)
{
    NvU32 streamIndex = 0;

    NvU32 maxHeads = allHeadMask;
    NUMSETBITS_32(maxHeads);
    streamIndex = DP_MST_HEAD_TO_STREAMID(head, streamIdentifier, maxHeads);

    return streamIndex;
}

bool EvoMainLink::queryGPUCapability()
{
    dpMemZero(&dpParams, sizeof(dpParams));
    dpParams.subDeviceInstance = subdeviceIndex;
    dpParams.sorIndex = provider->getSorIndex();
    NvU32 code = provider->rmControl0073(NV0073_CTRL_CMD_DP_GET_CAPS, &dpParams, sizeof(dpParams));
    if (code != NVOS_STATUS_SUCCESS)
    {
        DP_ASSERT(0 && "Unable to process GPU caps");
        return false;
    }
    //
    // Check if MST feature needs to be disabled by regkey. This is requirement by few OEMs, they don't want to support
    // MST feature on particular sku, whenever requested through INF.
    //
    _hasMultistream                 = (dpParams.bIsMultistreamSupported == NV_TRUE) && !_isMstDisabledByRegkey;
    _isStreamCloningEnabled         = (dpParams.bIsSCEnabled == NV_TRUE) ? true : false;
    _hasIncreasedWatermarkLimits    = (dpParams.bHasIncreasedWatermarkLimits == NV_TRUE) ? true : false;
    _isFECSupported                 = (dpParams.bFECSupported == NV_TRUE) ? true : false;
    _useDfpMaxLinkRateCaps          = (dpParams.bOverrideLinkBw == NV_TRUE) ? true : false;
    _isLTPhyRepeaterSupported       = (dpParams.bIsTrainPhyRepeater == NV_TRUE) ? true : false;
    _isDownspreadSupported          = (dpParams.bSupportDPDownSpread == NV_TRUE) ? true : false;
    _bAvoidHBR3                     = (dpParams.bAvoidHBR3 == NV_TRUE) ? true : false;

    _gpuSupportedDpVersions         = dpParams.dpVersionsSupported;

    if (FLD_TEST_DRF(0073, _CTRL_CMD_DP_GET_CAPS, _MAX_LINK_RATE, _1_62, dpParams.maxLinkRate))
        _maxLinkRateSupportedGpu = dp2LinkRate_1_62Gbps; // in 10Mbps
    else if (FLD_TEST_DRF(0073, _CTRL_CMD_DP_GET_CAPS, _MAX_LINK_RATE, _2_70, dpParams.maxLinkRate))
        _maxLinkRateSupportedGpu = dp2LinkRate_2_70Gbps; // in 10Mbps
    else if (FLD_TEST_DRF(0073, _CTRL_CMD_DP_GET_CAPS, _MAX_LINK_RATE, _5_40, dpParams.maxLinkRate))
        _maxLinkRateSupportedGpu = dp2LinkRate_5_40Gbps; // in 10Mbps
    else if (FLD_TEST_DRF(0073, _CTRL_CMD_DP_GET_CAPS, _MAX_LINK_RATE, _8_10, dpParams.maxLinkRate))
        _maxLinkRateSupportedGpu = dp2LinkRate_8_10Gbps; // in 10Mbps
    else
    {
        DP_ASSERT(0 && "Unable to get max link rate");
        // Assume that we can at least support RBR.
        _maxLinkRateSupportedGpu = dp2LinkRate_1_62Gbps;
    }

    if (!_isDscDisabledByRegkey)
    {
        _DSC.isDscSupported = dpParams.DSC.bDscSupported ? true : false;
        _DSC.encoderColorFormatMask = dpParams.DSC.encoderColorFormatMask;
        _DSC.lineBufferSizeKB = dpParams.DSC.lineBufferSizeKB;
        _DSC.rateBufferSizeKB = dpParams.DSC.rateBufferSizeKB;
        _DSC.bitsPerPixelPrecision = dpParams.DSC.bitsPerPixelPrecision;
        _DSC.maxNumHztSlices = dpParams.DSC.maxNumHztSlices;
        _DSC.lineBufferBitDepth = dpParams.DSC.lineBufferBitDepth;
    }
    return true;
}

void EvoMainLink::triggerACT()
{
    NV0073_CTRL_CMD_DP_SEND_ACT_PARAMS params = {0};
    params.subDeviceInstance = this->subdeviceIndex;
    params.displayId = this->displayId;

    NvU32 ret = provider->rmControl0073(NV0073_CTRL_CMD_DP_SEND_ACT, &params, sizeof params);
    if (ret != NVOS_STATUS_SUCCESS)
    {
        DP_PRINTF(DP_ERROR, "triggerACT failed!");
    }
}

void EvoMainLink::configureHDCPRenegotiate(NvU64 cN, NvU64 cKSV, bool bForceReAuth, bool bRxIDMsgPending) {}
void EvoMainLink::configureHDCPGetHDCPState(HDCPState &hdcpState)
{
    // HDCP Not Supported
    hdcpState.HDCP_State_Repeater_Capable = false;
    hdcpState.HDCP_State_22_Capable = false;
    hdcpState.HDCP_State_Encryption = false;
    hdcpState.HDCP_State_Authenticated = false;
}

void EvoMainLink::configureSingleStream(NvU32 head,
                                        NvU32 hBlankSym,
                                        NvU32 vBlankSym,
                                        bool  bEnhancedFraming,
                                        NvU32 tuSize,
                                        NvU32 waterMark,
                                        DP_COLORFORMAT colorFormat,
                                        DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID streamId,
                                        DP_SINGLE_HEAD_MULTI_STREAM_MODE singleHeadMultiStreamMode,
                                        bool bAudioOverRightPanel,
                                        bool bEnable2Head1Or)
{
    NV0073_CTRL_CMD_DP_CONFIG_STREAM_PARAMS params = {0};
    params.subDeviceInstance = this->subdeviceIndex;
    params.head = head;
    params.sorIndex = provider->getSorIndex();
    params.bEnableTwoHeadOneOr = bEnable2Head1Or;

    if (singleHeadMultiStreamMode == DP_SINGLE_HEAD_MULTI_STREAM_MODE_SST)
    {
        // In 2-SST mode configure Head-SF on primary link, so primary link configuration
        // gets copied to secondary link.
        params.dpLink = streamId;
    }
    else
    {
        params.dpLink = provider->getLinkIndex();
    }

    params.bEnableOverride = NV_TRUE;
    params.bMST = NV_FALSE;
    params.hBlankSym = hBlankSym;
    params.vBlankSym = vBlankSym;
    params.colorFormat = colorFormat;

    params.SST.bEnhancedFraming = bEnhancedFraming;
    params.SST.tuSize = tuSize;
    params.SST.waterMark = waterMark;
    params.SST.bEnableAudioOverRightPanel = bAudioOverRightPanel;

    NvU32 ret = provider->rmControl0073(NV0073_CTRL_CMD_DP_CONFIG_STREAM, &params, sizeof params);
    if (ret != NVOS_STATUS_SUCCESS)
    {
        DP_PRINTF(DP_ERROR, "configureSingleStream failed!");
    }
}

void EvoMainLink::configureSingleHeadMultiStreamMode(NvU32 displayIDs[],
                                                     NvU32 numStreams,
                                                     NvU32 mode,
                                                     bool  bSetConfig,
                                                     NvU8  vbiosPrimaryDispIdIndex)
{
    NV0073_CTRL_CMD_DP_CONFIG_SINGLE_HEAD_MULTI_STREAM_PARAMS params = {0};
    params.subDeviceInstance = this->subdeviceIndex;

    for (NvU32 pipelineID = 0; pipelineID < numStreams; pipelineID++)
    {
        params.displayIDs[pipelineID] = displayIDs[pipelineID];
    }
    params.mode = mode;
    params.bSetConfig = bSetConfig;
    params.numStreams = numStreams;
    params.vbiosPrimaryDispIdIndex = vbiosPrimaryDispIdIndex;

    NvU32 ret = provider->rmControl0073(NV0073_CTRL_CMD_DP_CONFIG_SINGLE_HEAD_MULTI_STREAM,
                                        &params, sizeof params);
    if (ret != NVOS_STATUS_SUCCESS)
    {
        DP_PRINTF(DP_ERROR, "configureSingleHeadMultiStreamMode failed!");
    }
}

void EvoMainLink::configureMultiStream(NvU32 head,
                                       NvU32 hBlankSym,
                                       NvU32 vBlankSym,
                                       NvU32 slotStart,
                                       NvU32 slotEnd,
                                       NvU32 PBN,
                                       NvU32 Timeslice,
                                       DP_COLORFORMAT colorFormat,
                                       DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID streamIdentifier,
                                       DP_SINGLE_HEAD_MULTI_STREAM_MODE singleHeadMultistreamMode,
                                       bool bAudioOverRightPanel,
                                       bool bEnable2Head1Or)
{
    NV0073_CTRL_CMD_DP_CONFIG_STREAM_PARAMS params = {0};
    params.head = head;
    params.subDeviceInstance = this->subdeviceIndex;
    params.sorIndex = provider->getSorIndex();
    params.dpLink = provider->getLinkIndex();
    params.bEnableOverride = NV_TRUE;
    params.bMST = NV_TRUE;
    params.hBlankSym = hBlankSym;
    params.vBlankSym = vBlankSym;
    params.colorFormat = colorFormat;
    params.bEnableTwoHeadOneOr = bEnable2Head1Or;
    params.singleHeadMultistreamMode = singleHeadMultistreamMode;

    params.MST.slotStart = slotStart;
    params.MST.slotEnd = slotEnd;
    params.MST.PBN = PBN;
    params.MST.Timeslice = Timeslice;
    params.MST.singleHeadMSTPipeline = streamIdentifier;
    params.MST.bEnableAudioOverRightPanel = bAudioOverRightPanel;

    NvU32 ret = provider->rmControl0073(NV0073_CTRL_CMD_DP_CONFIG_STREAM, &params, sizeof params);
    if (ret != NVOS_STATUS_SUCCESS)
    {
        DP_PRINTF(DP_ERROR, "configureMultiStream failed!");
    }
}

void EvoMainLink::configureMsScratchRegisters(NvU32 address,
                                              NvU32 hopCount,
                                              NvU32 dpMsDevAddrState)
{
    NV0073_CTRL_CMD_DP_CONFIG_RAD_SCRATCH_REG_PARAMS params = {0};
    params.subDeviceInstance = this->subdeviceIndex;
    params.displayId = this->displayId;
    params.activeDevAddr = address;
    params.sorIndex = provider->getSorIndex();
    params.dpLink = provider->getLinkIndex();
    params.hopCount = hopCount;
    params.dpMsDevAddrState = dpMsDevAddrState;

    NvU32 ret = provider->rmControl0073(NV0073_CTRL_CMD_DP_CONFIG_RAD_SCRATCH_REG, &params, sizeof params);
    if (ret != NVOS_STATUS_SUCCESS)
    {
        DP_PRINTF(DP_ERROR, "configureMsScratchRegisters failed!");
    }
}

//
// EvoMainLink::setDpStereoMSAParameters does the DP library Stereo override for
// In-band signaling through the MSA MISC1 field and keeps the rest of the MSA
// params the same.
//
// On GK110 and later, when stereo is enabled, we send the stereo eye
// information to the sink device through the MSA MISC1 bits 2:1.  Certain
// DP 1.2 non-compliant DP->VGA dongles cannot handle this information, and
// lose all signal when these bits are non-zero.  This WAR uses a RM control
// to override those MSA bits to zero.  It should be called whenever a DP->VGA
// dongle is in use.
//
bool EvoMainLink::setDpStereoMSAParameters(bool bStereoEnable, const NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_PARAMS &msaparams)
{
    NV0073_CTRL_CMD_DP_SET_STEREO_MSA_PROPERTIES_PARAMS params = {0};
    params.subDeviceInstance = this->subdeviceIndex;
    params.displayId = msaparams.displayId;
    //clubbing the MSA params passed by DD with Dp Library Stereo Override
    params.bStereoPhaseInverse = msaparams.bStereoPhaseInverse;
    params.featureValues.misc[1] = msaparams.featureValues.misc[1];

    if (bStereoEnable) {
        params.bEnableMSA = NV_TRUE | msaparams.bEnableMSA;
        params.featureMask.miscMask[1] = DRF_SHIFTMASK(NV_DP_MSA_PROPERTIES_MISC1_STEREO) | msaparams.featureMask.miscMask[1];
    } else {
        params.bEnableMSA = NV_FALSE | msaparams.bEnableMSA;
        params.featureMask.miscMask[1] |= msaparams.featureMask.miscMask[1];
    }

    NvU32 ret = provider->rmControl0073(NV0073_CTRL_CMD_DP_SET_STEREO_MSA_PROPERTIES, &params, sizeof params);

    //
    // NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES is only implemented on GK110 and
    // later, but this WAR is unnecessary on other GPUs, so ignore
    // ERROR_NOT_SUPPORTED.
    //
    // XXX This may fail if a future GPU requires this WAR but does not
    // implement this rmcontrol.  To avoid that, this class would need to be
    // aware of which evo display HAL is in use.
    //
    if (ret != NVOS_STATUS_SUCCESS && ret != NVOS_STATUS_ERROR_NOT_SUPPORTED)
    {
        DP_ASSERT(!"Enabling MSA stereo override failed!");
        return false;
    }

    return true;
}

//
// EvoMainLink::setDpMSAParameters clubs MSA parameters passed by DD for format YCbCr4:2:0
// with DP library Stereo override for In-band signaling through the MSA MISC1 field.
//
// On GK110 and later, when stereo is enabled, we send the stereo eye
// information to the sink device through the MSA MISC1 bits 2:1.  Certain
// DP 1.2 non-compliant DP->VGA dongles cannot handle this information, and
// lose all signal when these bits are non-zero.  This WAR uses a RM control
// to override those MSA bits to zero.  It should be called whenever a DP->VGA
// dongle is in use.
//
bool EvoMainLink::setDpMSAParameters(bool bStereoEnable, const NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_PARAMS &msaparams)
{
    NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES_PARAMS params = {0};
    params.subDeviceInstance = this->subdeviceIndex;
    params.displayId = msaparams.displayId;
    //clubbing the MSA params passed by DD with Dp Library Stereo Override
    params.bStereoPhaseInverse = msaparams.bStereoPhaseInverse;
    params.bCacheMsaOverrideForNextModeset = true;
    params.featureValues.misc[0] = msaparams.featureValues.misc[0];
    params.featureValues.misc[1] = msaparams.featureValues.misc[1];
    params.featureMask.miscMask[0] = msaparams.featureMask.miscMask[0];

    params.featureValues.rasterTotalHorizontal = msaparams.featureValues.rasterTotalHorizontal;
    params.featureValues.rasterTotalVertical = msaparams.featureValues.rasterTotalVertical;
    params.featureValues.activeStartHorizontal = msaparams.featureValues.activeStartHorizontal;
    params.featureValues.activeStartVertical = msaparams.featureValues.activeStartVertical;
    params.featureValues.surfaceTotalHorizontal = msaparams.featureValues.surfaceTotalHorizontal;
    params.featureValues.surfaceTotalVertical = msaparams.featureValues.surfaceTotalVertical;
    params.featureValues.syncWidthHorizontal = msaparams.featureValues.syncWidthHorizontal;
    params.featureValues.syncPolarityHorizontal = msaparams.featureValues.syncPolarityHorizontal;
    params.featureValues.syncHeightVertical = msaparams.featureValues.syncHeightVertical;
    params.featureValues.syncPolarityVertical = msaparams.featureValues.syncPolarityVertical;

    params.featureMask.bRasterTotalHorizontal = msaparams.featureMask.bRasterTotalHorizontal;
    params.featureMask.bRasterTotalVertical = msaparams.featureMask.bRasterTotalVertical;
    params.featureMask.bActiveStartHorizontal = msaparams.featureMask.bActiveStartHorizontal;
    params.featureMask.bActiveStartVertical = msaparams.featureMask.bActiveStartVertical;
    params.featureMask.bSurfaceTotalHorizontal = msaparams.featureMask.bSurfaceTotalHorizontal;
    params.featureMask.bSurfaceTotalVertical = msaparams.featureMask.bSurfaceTotalVertical;
    params.featureMask.bSyncWidthHorizontal = msaparams.featureMask.bSyncWidthHorizontal;
    params.featureMask.bSyncPolarityHorizontal = msaparams.featureMask.bSyncPolarityHorizontal;
    params.featureMask.bSyncHeightVertical = msaparams.featureMask.bSyncHeightVertical;
    params.featureMask.bSyncPolarityVertical = msaparams.featureMask.bSyncPolarityVertical;

    params.featureValues.reserved[0] = msaparams.featureValues.reserved[0];
    params.featureValues.reserved[1] = msaparams.featureValues.reserved[1];
    params.featureValues.reserved[2] = msaparams.featureValues.reserved[2];

    params.pFeatureDebugValues = msaparams.pFeatureDebugValues;

    if (bStereoEnable) {
        params.bEnableMSA = NV_TRUE | msaparams.bEnableMSA;
        params.featureMask.miscMask[1] = DRF_SHIFTMASK(NV_DP_MSA_PROPERTIES_MISC1_STEREO) | msaparams.featureMask.miscMask[1];
    } else {
        params.bEnableMSA = NV_FALSE | msaparams.bEnableMSA;
        params.featureMask.miscMask[1] |= msaparams.featureMask.miscMask[1];
    }

    NvU32 ret = provider->rmControl0073(NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES, &params, sizeof params);

    //
    // NV0073_CTRL_CMD_DP_SET_MSA_PROPERTIES is only implemented on GK110 and
    // later, but this WAR is unnecessary on other GPUs, so ignore
    // ERROR_NOT_SUPPORTED.
    //
    // XXX This may fail if a future GPU requires this WAR but does not
    // implement this rmcontrol.  To avoid that, this class would need to be
    // aware of which evo display HAL is in use.
    //
    if (ret != NVOS_STATUS_SUCCESS && ret != NVOS_STATUS_ERROR_NOT_SUPPORTED) {
        DP_ASSERT(!"Enabling MSA stereo override failed!");
        return false;
    }

    return true;
}

bool EvoMainLink::setFlushMode()
{
    NV5070_CTRL_SET_SOR_FLUSH_MODE_PARAMS params;
    dpMemZero(&params, sizeof(params));

    params.bFireAndForget = NV_FALSE;

    params.base.subdeviceIndex = subdeviceIndex;
    params.sorNumber = provider->getSorIndex();
    params.bEnable = NV_TRUE;
    params.bForceRgDiv = NV_FALSE;
    params.bImmediate = NV_FALSE;
    params.headMask = 0;

    NvU32 ret = provider->rmControl5070(NV5070_CTRL_CMD_SET_SOR_FLUSH_MODE, &params, sizeof params);

    DP_ASSERT((ret == NVOS_STATUS_SUCCESS) && "Enabling flush mode failed!");

    return ret == NVOS_STATUS_SUCCESS;
}

void EvoMainLink::clearFlushMode(unsigned headMask, bool testMode)
{
    NV5070_CTRL_SET_SOR_FLUSH_MODE_PARAMS params;
    dpMemZero(&params, sizeof(params));

    params.bFireAndForget = NV_FALSE;
    params.base.subdeviceIndex = subdeviceIndex;
    params.sorNumber = provider->getSorIndex();
    params.bEnable = NV_FALSE;
    params.bImmediate = NV_FALSE;
    params.headMask = headMask;
    params.bForceRgDiv = testMode;

    NvU32 ret = provider->rmControl5070(NV5070_CTRL_CMD_SET_SOR_FLUSH_MODE, &params, sizeof params);
    if (ret != NVOS_STATUS_SUCCESS)
    {
        DP_PRINTF(DP_ERROR, "DP_EVO> Disabling flush mode failed!");
    }
}


bool EvoMainLink::physicalLayerSetTestPattern(PatternInfo * patternInfo)
{
    // Main parameter
    NV0073_CTRL_DP_SET_TESTPATTERN_PARAMS params;

    // To identify which test pattern to transmit.
    NV0073_CTRL_DP_TESTPATTERN ctrlPattern;

    dpMemZero(&params, sizeof(params));
    dpMemZero(&ctrlPattern, sizeof(ctrlPattern));

    switch (patternInfo->lqsPattern)
    {
        case LINK_QUAL_DISABLED:    ctrlPattern.testPattern = NV0073_CTRL_DP_TESTPATTERN_DATA_NONE; break;
        case LINK_QUAL_D10_2:       ctrlPattern.testPattern = NV0073_CTRL_DP_TESTPATTERN_DATA_D10_2; break;
        case LINK_QUAL_SYM_ERROR:   ctrlPattern.testPattern = NV0073_CTRL_DP_TESTPATTERN_DATA_SERMP; break;
        case LINK_QUAL_PRBS7:       ctrlPattern.testPattern = NV0073_CTRL_DP_TESTPATTERN_DATA_PRBS_7; break;
        case LINK_QUAL_CP2520PAT3:  ctrlPattern.testPattern = NV0073_CTRL_DP_TESTPATTERN_DATA_CP2520PAT3; break;
        case LINK_QUAL_80BIT_CUST:
        {
            ctrlPattern.testPattern = NV0073_CTRL_DP_TESTPATTERN_DATA_CSTM;

            params.cstm.field_31_0 = patternInfo->ctsmLower;
            params.cstm.field_63_32 = patternInfo->ctsmMiddle;
            params.cstm.field_95_64 = patternInfo->ctsmUpper;
            break;
        }
#ifdef NV0073_CTRL_DP_TESTPATTERN_DATA_HBR2COMPLIANCE
        case LINK_QUAL_HBR2_COMPLIANCE_EYE:
        {
            ctrlPattern.testPattern = NV0073_CTRL_DP_TESTPATTERN_DATA_HBR2COMPLIANCE;
            params.cstm.field_31_0 = 0;
            params.cstm.field_63_32 = 0;
            params.cstm.field_95_64 = 0;
            break;
        }
#endif
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

    return code == NVOS_STATUS_SUCCESS;
}

AuxBus::status EvoAuxBus::transaction(Action action, Type type, int address,
                                      NvU8 * buffer, unsigned sizeRequested,
                                      unsigned * sizeCompleted,
                                      unsigned *pNakReason,
                                      NvU8 offset, NvU8 nWriteTransactions)
{
    NV0073_CTRL_DP_AUXCH_CTRL_PARAMS params;

    DP_ASSERT(sizeRequested <= NV0073_CTRL_DP_AUXCH_MAX_DATA_SIZE);

    dpMemZero(&params, sizeof(params));
    params.subDeviceInstance       = subdeviceIndex;
    params.displayId               = displayId;

    params.cmd = 0;

    if (type == native)
        params.cmd |= DRF_DEF(0073_CTRL, _DP, _AUXCH_CMD_TYPE, _AUX);
    else
        params.cmd |= DRF_DEF(0073_CTRL, _DP, _AUXCH_CMD_TYPE, _I2C);

    if (type == i2cMot)
        params.cmd |= DRF_DEF(0073_CTRL, _DP, _AUXCH_CMD_I2C_MOT, _TRUE);
    else
        params.cmd |= DRF_DEF(0073_CTRL, _DP, _AUXCH_CMD_I2C_MOT, _FALSE);

    if (action == read)
        params.cmd |= DRF_DEF(0073_CTRL, _DP, _AUXCH_CMD_REQ_TYPE, _READ);
    else if (action == write)
    {
        params.cmd |= DRF_DEF(0073_CTRL, _DP, _AUXCH_CMD_REQ_TYPE, _WRITE);
        dpMemCopy(params.data, buffer, sizeRequested);
    }
    else if (action == writeStatusUpdateRequest)
        params.cmd |= DRF_DEF(0073_CTRL, _DP, _AUXCH_CMD_REQ_TYPE, _WRITE_STATUS);
    else
        DP_ASSERT(0 && "Unknown action");

    params.addr = address;

    //
    // By definition, an I2C-write-over-AUX request with
    // zero bytes of data is an "address-only" transaction.
    //
    if ((sizeRequested == 0) && (type & (i2cMot | i2c)) && (action == write))
    {
        DP_PRINTF(DP_NOTICE, "DP> Client requested address-only transaction");
        params.bAddrOnly = NV_TRUE;
    }
    else if ((sizeRequested == 0) && (type == native))
    {
        // Native aux transactions with size requested zero are not allowed.
        DP_ASSERT(0 && "Native Aux transactions shouldn't have zero size requested");
        return nack;
    }

    // Control call is taking size as 0-based.
    if (sizeRequested == 0)
    {
        //
        // I2c transactions with size requested zero. Decrementing by 1 will
        // lead to 0xffffff(RM_INVALID_DATA). So keep size as zero only.
        //
        params.size = 0;
    }
    else
    {
        params.size = sizeRequested - 1;
    }

    NvU32 code = 0;
    NvU8  retries = 0;
    do
    {
        retries++;
        params.retryTimeMs = 0;
        code = provider->rmControl0073(NV0073_CTRL_CMD_DP_AUXCH_CTRL, &params, sizeof(params));
        // eDP is not fully powered up yet. Should not access the panel too early.
        if (params.retryTimeMs > 0)
        {
            timer->sleep(params.retryTimeMs);
        }
    } while (NVOS_STATUS_SUCCESS != code && params.retryTimeMs && retries < 3);

    if (pNakReason != NULL)
    {
        *pNakReason = params.replyType;
    }

    if (action == writeStatusUpdateRequest && code == NVOS_STATUS_ERROR_NOT_SUPPORTED)
    {
        //
        // On some chips write status requests are generated implicitly by the
        // hardware. So while the RmControl() will fail with a "not supported"
        // error, the request still went out on the DPAUX channel as part of
        // the last IC-over-AUX write transaction. So the error should be ignored.
        //
        DP_PRINTF(DP_NOTICE, "DP> %s: Ignore ERROR_NOT_SUPPORTED for writeStatusUpdateRequest. Returning Success", __FUNCTION__);
        return AuxBus::success;
    }

    // In case of Timeout we need to retry again for minimum no. of times
    if (code != NVOS_STATUS_SUCCESS && code != NVOS_STATUS_ERROR_TIMEOUT)
    {
        if (devicePlugged)
        {
            DP_PRINTF(DP_WARNING, "DP> AuxChCtl Failing, if a device is connected you shouldn't be seeing this");
        }
        return nack;
    }
    else if (code == NVOS_STATUS_ERROR_TIMEOUT)
    {
        return AuxBus::defer;
    }

    *sizeCompleted = params.size;

    // Reset sizeCompleted if transaction failed.
    if (params.replyType == NV0073_CTRL_DP_AUXCH_REPLYTYPE_DEFER ||
        params.replyType == NV0073_CTRL_DP_AUXCH_REPLYTYPE_I2CDEFER)
        *sizeCompleted = 0;

    if (params.replyType == NV0073_CTRL_DP_AUXCH_REPLYTYPE_ACK)
    {
        // if it was read operation copy read data to buffer
        if (action == read)
        {
            // Check the size of data to be copied. Should not be
            // more than available buffer
            if (params.size > sizeRequested)
            {
                params.size = sizeRequested;
            }
            dpMemCopy(buffer, params.data, params.size);
        }

        return AuxBus::success;
    }

    if (params.replyType == NV0073_CTRL_DP_AUXCH_REPLYTYPE_NACK ||
        params.replyType == NV0073_CTRL_DP_AUXCH_REPLYTYPE_I2CNACK ||
        params.replyType == NV0073_CTRL_DP_AUXCH_REPLYTYPE_TIMEOUT)
        return AuxBus::nack;

    if (params.replyType == NV0073_CTRL_DP_AUXCH_REPLYTYPE_DEFER ||
        params.replyType == NV0073_CTRL_DP_AUXCH_REPLYTYPE_I2CDEFER)
        return AuxBus::defer;

    DP_ASSERT(0 && "Unknown reply type");
    return AuxBus::nack;
}

unsigned EvoAuxBus::transactionSize()
{
    return NV0073_CTRL_DP_AUXCH_MAX_DATA_SIZE;
}

void EvoAuxBus::setDevicePlugged(bool plugged)
{
    devicePlugged = plugged;
}

void EvoMainLink::preLinkTraining(NvU32 head)
{
    provider->preLinkTraining(head);
}

void EvoMainLink::postLinkTraining(NvU32 head)
{
    provider->postLinkTraining(head);
}

void EvoMainLink::initializeRegkeyDatabase()
{
    NvU32 i;

    if (dpRegkeyDatabase.bInitialized)
        return;

    for (i = 0; i < sizeof(DP_REGKEY_TABLE)/sizeof(DP_REGKEY_TABLE[0]); i++)
    {
        NvU32 tempValue = 0;
        tempValue = provider->getRegkeyValue(DP_REGKEY_TABLE[i].pName);
        switch (DP_REGKEY_TABLE[i].valueType)
        {
            case DP_REG_VAL_U32:
                *(NvU32*)(DP_REGKEY_TABLE[i].pValue) = tempValue;
                break;
            case DP_REG_VAL_U16:
                *(NvU16*)(DP_REGKEY_TABLE[i].pValue) = tempValue & 0xFFFF;
                break;
            case DP_REG_VAL_U8:
                *(NvU8*)(DP_REGKEY_TABLE[i].pValue)  = tempValue & 0xFF;
                break;
            case DP_REG_VAL_BOOL:
                *(bool*)(DP_REGKEY_TABLE[i].pValue)  = !!tempValue;
                break;
        }
    }

    dpRegkeyDatabase.bInitialized = true;
}

void EvoMainLink::applyRegkeyOverrides()
{
    if (!dpRegkeyDatabase.bInitialized)
    {
        DP_ASSERT(0 && "dpRegkeyDatabase is not initialized before calling applyRegkeyOverrides.");
        this->initializeRegkeyDatabase();
    }
    _isMstDisabledByRegkey               = dpRegkeyDatabase.bMstDisabled;
    _isDscDisabledByRegkey               = dpRegkeyDatabase.bDscDisabled;
    _skipPowerdownEDPPanelWhenHeadDetach = dpRegkeyDatabase.bPoweroffEdpInHeadDetachSkipped;
    _applyLinkBwOverrideWarRegVal        = dpRegkeyDatabase.bLinkBwOverrideWarApplied;
    _enableMSAOverrideOverMST            = dpRegkeyDatabase.bMsaOverMstEnabled;
    _isMSTPCONCapsReadDisabled           = dpRegkeyDatabase.bMSTPCONCapsReadDisabled;
    _isDownspreadDisabledByRegkey        = dpRegkeyDatabase.bDownspreadDisabled;
    _bAvoidHBR3DisabledByRegkey          = dpRegkeyDatabase.bDisableAvoidHBR3War;
}

NvU32 EvoMainLink::getRegkeyValue(const char *key)
{
    NvU32 i;
    if (!dpRegkeyDatabase.bInitialized)
    {
        DP_ASSERT(0 && "dpRegkeyDatabase is not initialized before calling getRegkeyValue.");
        initializeRegkeyDatabase();
    }
    if (key == NULL || key[0] == '\0')
        return 0;

    for (i = 0; i < sizeof(DP_REGKEY_TABLE)/sizeof(DP_REGKEY_TABLE[0]); i++)
    {
        NvU32 j = 0;
        bool strSame = true;
        while (key[j] != '\0' && DP_REGKEY_TABLE[i].pName[j] != '\0')
        {
            if (key[j] != DP_REGKEY_TABLE[i].pName[j])
            {
                strSame = false;
                break;
            }
            ++j;
        }
        if (strSame && key[j] == '\0' && DP_REGKEY_TABLE[i].pName[j] == '\0')
        {
            switch (DP_REGKEY_TABLE[i].valueType)
            {
                case DP_REG_VAL_U32:
                    return *(NvU32*)(DP_REGKEY_TABLE[i].pValue);
                case DP_REG_VAL_U16:
                    return (NvU32)*(NvU16*)(DP_REGKEY_TABLE[i].pValue);
                case DP_REG_VAL_U8:
                    return (NvU32)*(NvU8*)(DP_REGKEY_TABLE[i].pValue);
                case DP_REG_VAL_BOOL:
                    return (NvU32)*(bool*)(DP_REGKEY_TABLE[i].pValue);
            }
        }
    }
    DP_ASSERT(0 && "Requested regkey not found in dpRegkeyDatabase.");
    return 0;
}

const DP_REGKEY_DATABASE& EvoMainLink::getRegkeyDatabase()
{
    return dpRegkeyDatabase;
}

NvU32 EvoMainLink::getSorIndex()
{
    return provider->getSorIndex();
}

bool EvoMainLink::isInbandStereoSignalingSupported()
{
    return provider->isInbandStereoSignalingSupported();
}

bool EvoMainLink::train(const LinkConfiguration & link, bool force,
                        LinkTrainingType linkTrainingType,
                        LinkConfiguration *retLink, bool bSkipLt,
                        bool isPostLtAdjRequestGranted, unsigned phyRepeaterCount)
{
    NvU32       targetIndex;
    NvU32       ltCounter = retLink->getLTCounter();
    bool        bTrainPhyRepeater =
                    (!link.bDisableLTTPR) && (_isLTPhyRepeaterSupported);

    if (provider->getSorIndex() == DP_INVALID_SOR_INDEX)
    {
        // bail out and Skip LT since SOR is not allocated for this displayID
        return false;
    }
    NvU32 err = 0;

    NvU32 dpCtrlCmd = DRF_DEF(0073_CTRL, _DP_CMD, _SET_LANE_COUNT, _TRUE) |
        DRF_DEF(0073_CTRL, _DP_CMD, _SET_LINK_BW,    _TRUE);

    if (link.multistream)
        dpCtrlCmd |= DRF_DEF(0073_CTRL, _DP_CMD, _SET_FORMAT_MODE, _MULTI_STREAM);

    if(link.bEnableFEC)
        dpCtrlCmd |= DRF_DEF(0073_CTRL, _DP_CMD, _ENABLE_FEC, _TRUE);

    if (isPostLtAdjRequestGranted)
        dpCtrlCmd |= DRF_DEF(0073_CTRL, _DP_CMD, _POST_LT_ADJ_REQ_GRANTED, _YES);

    if (link.enhancedFraming)
        dpCtrlCmd |= DRF_DEF(0073_CTRL, _DP_CMD, _SET_ENHANCED_FRAMING, _TRUE);
    if (bSkipLt)
        dpCtrlCmd |= DRF_DEF(0073_CTRL, _DP_CMD, _SKIP_HW_PROGRAMMING, _YES);
    if (force)
        dpCtrlCmd |= DRF_DEF(0073_CTRL, _DP_CMD, _FAKE_LINK_TRAINING, _DONOT_TOGGLE_TRANSMISSION);

    if (linkTrainingType == NO_LINK_TRAINING)
        dpCtrlCmd |= DRF_DEF(0073_CTRL, _DP_CMD, _NO_LINK_TRAINING, _YES);
    else if (linkTrainingType == FAST_LINK_TRAINING)
        dpCtrlCmd |= DRF_DEF(0073_CTRL, _DP_CMD, _FAST_LINK_TRAINING, _YES);

    targetIndex = NV0073_CTRL_DP_DATA_TARGET_SINK;
    if (bTrainPhyRepeater && (_rmPhyRepeaterCount != phyRepeaterCount))
    {
        // If LTTPR count is out of sync between DPLib and RM, do not link train LTTPRs.
        bTrainPhyRepeater = false;
    }

    if (bTrainPhyRepeater)
    {
        dpCtrlCmd |= DRF_DEF(0073_CTRL, _DP_CMD, _TRAIN_PHY_REPEATER, _YES);
        //
        // Start from the one closest to GPU. Note this is 1-based index.
        //
        targetIndex = phyRepeaterCount;
    }

    if (!this->isDownspreadSupported() || link.bDisableDownspread || _isDownspreadDisabledByRegkey)
    {
        // If GPU does not support downspread, disabling downspread.
        dpCtrlCmd |= DRF_DEF(0073_CTRL, _DP_CMD, _USE_DOWNSPREAD_SETTING, _FORCE);
        dpCtrlCmd |= DRF_DEF(0073_CTRL, _DP_CMD, _DISABLE_DOWNSPREAD, _TRUE);
    }

    NV_DPTRACE_INFO(LINK_TRAINING_START, link.multistream, link.peakRate, link.lanes,
                        phyRepeaterCount, _rmPhyRepeaterCount, bTrainPhyRepeater, targetIndex);

    NvU32 status = 0;
    NvU8 retries = 0;
    bool fallback = false;

    //
    // Limited attempts to unblock infinite LT loop while CR failure restores
    // high rate and lanes for EQ failure
    //
    NvU32 crHighRateFallbackCount = 0;

    //
    // The rate and lane count we send to RM might be different than what client
    // sent to us since fallback might happen.
    //
    LinkConfiguration requestRmLC = link;
    do
    {
        NvU32 dpCtrlData = 0;
        NvU64 linkrate = requestRmLC.peakRate;
        NvU64 linkBw = 0;

        switch (linkrate)
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
                linkBw = LINK_RATE_10MHZ_TO_270MHZ(linkrate);
                dpCtrlData = FLD_SET_DRF_NUM(0073_CTRL, _DP_DATA, _SET_LINK_BW,
                                             linkBw, dpCtrlData);
                break;
            default:
                if (requestRmLC.lanes != 0)
                {
                    DP_ASSERT(0 && "Unknown rate");
                    return false;
                }
                break;
        }

        dpCtrlData = FLD_SET_DRF_NUM(0073_CTRL, _DP_DATA, _SET_LANE_COUNT,
                                                requestRmLC.lanes, dpCtrlData);

        if (requestRmLC.lanes == 0)
        {
            // Only need to target sink when powering down the link.
            targetIndex = NV0073_CTRL_DP_DATA_TARGET_SINK;
        }

        dpCtrlData = FLD_SET_DRF_NUM(0073_CTRL, _DP_DATA, _TARGET,
                                                        targetIndex, dpCtrlData);

        //  Properly wait eDP to power up before link training.
        status      = 0;
        retries     = 0;
        fallback    = false;
        dpCtrlCmd = FLD_SET_DRF(0073_CTRL, _DP_CMD, _FALLBACK_CONFIG, _FALSE, dpCtrlCmd);
        do
        {
            NV0073_CTRL_DP_CTRL_PARAMS params;

            dpMemZero(&params, sizeof(params));
            params.subDeviceInstance       = subdeviceIndex;
            params.displayId               = displayId;
            params.cmd                     = dpCtrlCmd;
            params.data                    = dpCtrlData;

            retries++;
            params.retryTimeMs = 0;
            status = provider->rmControl0073(NV0073_CTRL_CMD_DP_CTRL, &params, sizeof(params));
            ltCounter++;
            err = params.err;

            if (params.retryTimeMs > 0)
            {
                timer->sleep(params.retryTimeMs);
            }

            if (status == NVOS_STATUS_SUCCESS || bSkipLt)
            {
                // if LT failed when bSkipLt was marked, no point in attempting LT again.
                break;
            }

            if (!params.retryTimeMs || retries >= 3)
            {
                break;
            }

        } while (true);

        if (NVOS_STATUS_SUCCESS == status)
        {
            if (targetIndex != NV0073_CTRL_DP_DATA_TARGET_SINK)
            {
                targetIndex -= 1;
                continue;
            }
            else
            {
                // all done, leave the loop.
                break;
            }
        }

        if (requestRmLC.policy.skipFallback() || bSkipLt)
        {
            //
            // if LT failed when bSkipLT was marked, no point in falling back as the issue
            // is not with LinkConfig.
            //
            break;
        }

        if (FLD_TEST_DRF(0073_CTRL_DP, _ERR, _LINK_STATUS, _DISCONNECTED, err))
        {
            // Don't fallback if the device is already gone.
            break;
        }

        if (FLD_TEST_DRF(0073_CTRL_DP, _CMD, _TRAIN_PHY_REPEATER, _YES, dpCtrlCmd) &&
            FLD_TEST_DRF(0073_CTRL_DP, _ERR, _INVALID_PARAMETER, _ERR, err) &&
            FLD_TEST_DRF(0073_CTRL_DP, _ERR, _TRAIN_PHY_REPEATER, _ERR, err))
        {
            //
            // RM has less LTTPR than DPLib expected.
            //    - Force to do transparent mode.
            //
            targetIndex = NV0073_CTRL_DP_DATA_TARGET_SINK;
            dpCtrlCmd = FLD_SET_DRF(0073_CTRL, _DP_CMD, _TRAIN_PHY_REPEATER,
                                    _NO, dpCtrlCmd);
            continue;
        }

        dpCtrlCmd |= DRF_DEF(0073_CTRL, _DP_CMD, _FALLBACK_CONFIG, _TRUE);

        if (FLD_TEST_DRF(0073_CTRL_DP, _ERR, _CLOCK_RECOVERY, _ERR, err))
        {
            // If failed CR, check if we need to fallback.
            if (requestRmLC.peakRate != dp2LinkRate_1_62Gbps)
            {
                //
                // We need to fallback on link rate if the following conditions are met:
                // 1. CR or EQ phase failed.
                // 2. The request link bandwidth is NOT RBR
                //
                if (!requestRmLC.lowerConfig())
                {
                    // If no valid link config could be found, break here.
                    break;
                }
                fallback = true;
            }
            else
            {
                // Already RBR
                // Check how many lanes is done.
                requestRmLC.lanes = DRF_VAL(0073_CTRL_DP, _ERR, _CR_DONE_LANE, err);

                while (!IS_VALID_LANECOUNT(requestRmLC.lanes))
                {
                    requestRmLC.lanes--;
                }

                if (requestRmLC.lanes == 0)
                {
                    // This is to WAR some system that doesn't set CR_DONE or EQ_DONE at all.
                    // In this case, we just simply try half of lanes.
                    requestRmLC.lanes = DRF_VAL(0073_CTRL, _DP_DATA, _SET_LANE_COUNT, dpCtrlData) / 2;
                    if (requestRmLC.lanes == 0)
                    {
                        // Nothing to try. Bail out.
                        break;
                    }
                }
                // Set back to original desired rate.
                requestRmLC.peakRate = link.peakRate;
                fallback = true;
                crHighRateFallbackCount++;
            }
        }
        if (FLD_TEST_DRF(0073_CTRL_DP, _ERR, _CHANNEL_EQUALIZATION, _ERR, err))
        {
            //
            // If Channel equalization fails, we need to use the fallback policy
            // of reducing the lane count vs link rate, but in the special case
            // when all lanes have failed CR, we resort to lowering link rate instead
            // (this address the new Fallback SCR v2.0)
            //
            if (FLD_TEST_DRF(0073_CTRL_DP, _ERR, _CR_DONE_LANE, _0_LANE, err))
            {
                //Per spec, if link rate has already been reduced to RBR, exit fallback
                if(requestRmLC.peakRate == dp2LinkRate_1_62Gbps || !requestRmLC.lowerConfig())
                    break;
            }
            else
            {
                if(!requestRmLC.lowerConfig(true))  // bReduceLaneCnt = true
                    break;
            }
            fallback = true;
        }
        if (fallback == false)
        {
            // Nothing to fallback, give up.
            break;
        }
        if ((phyRepeaterCount > 0) && (bTrainPhyRepeater))
        {
            // If fallback, need to start from beginning.
            targetIndex = phyRepeaterCount;
        }
    } while (crHighRateFallbackCount < NV_DP_RBR_FALLBACK_MAX_TRIES);

    //
    // Result should be checked for only the control call status. 'err'
    // doesn't represent failure in LT - some compliance tests such as 700.1.1.2
    // intentionally test against unexpected sink caps
    //
    bool result = (status == NVOS_STATUS_SUCCESS);
    retLink->setLaneRate(requestRmLC.peakRate, result ? requestRmLC.lanes : 0);
    retLink->setLTCounter(ltCounter);

    if (requestRmLC.bEnableFEC && (FLD_TEST_DRF(0073_CTRL_DP, _ERR, _ENABLE_FEC, _ERR, err)))
    {
        retLink->bEnableFEC = false;
        DP_ASSERT(0);
    }

    NV_DPTRACE_INFO(LINK_TRAINING_DONE, status, requestRmLC.peakRate, requestRmLC.lanes);

    return result;
}

bool EvoMainLink::retrieveRingBuffer(NvU8 dpRingBuffertype, NvU32 numRecords)
{
    return false;
}

// Return the current mux state. Returns false if device is not mux capable
bool EvoMainLink::getDynamicMuxState(NvU32 *muxState)
{
    bool bIsMuxCapable = false;
    NvU32 ret = 0;
    NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS_PARAMS muxStatusParams;

    if (!muxState)
        return false;

    *muxState = 0;

    if (!isDynamicMuxCapable())
        return false;

    dpMemZero(&muxStatusParams, sizeof(muxStatusParams));
    muxStatusParams.subDeviceInstance = subdeviceIndex;
    muxStatusParams.displayId = displayId;
    muxStatusParams.muxStatus = 0;

    ret = provider->rmControl0073(NV0073_CTRL_CMD_DFP_GET_DISP_MUX_STATUS,
                                  &muxStatusParams, sizeof(muxStatusParams));
    if (ret == NV_OK &&
        DRF_VAL(0073, _CTRL_DFP_DISP_MUX, _STATE, muxStatusParams.muxStatus) != NV0073_CTRL_DFP_DISP_MUX_STATE_INVALID)
    {
        bIsMuxCapable = true;
        *muxState = muxStatusParams.muxStatus;
    }

    return bIsMuxCapable;
}

void EvoMainLink::configurePowerState(bool bPowerUp)
{
    NV0073_CTRL_DP_MAIN_LINK_CTRL_PARAMS params;

    dpMemZero(&params, sizeof(params));
    params.subDeviceInstance     = subdeviceIndex;
    params.displayId             = displayId;
    params.ctrl                  = bPowerUp ? NV0073_CTRL_DP_MAIN_LINK_CTRL_POWER_STATE_POWERUP :
                                              NV0073_CTRL_DP_MAIN_LINK_CTRL_POWER_STATE_POWERDOWN;

    NvU32 code = provider->rmControl0073(NV0073_CTRL_CMD_DP_MAIN_LINK_CTRL, &params, sizeof(params));

    DP_ASSERT(code == NVOS_STATUS_SUCCESS);
}

void EvoMainLink::getLinkConfig(unsigned &laneCount, NvU64 & linkRate)
{
    NV0073_CTRL_DP_GET_LINK_CONFIG_PARAMS params;
    dpMemZero(&params, sizeof(params));

    params.subDeviceInstance = subdeviceIndex;
    params.displayId = displayId;

    NvU32 code = provider->rmControl0073(NV0073_CTRL_CMD_DP_GET_LINK_CONFIG, &params, sizeof(params));

    if (code == NVOS_STATUS_SUCCESS)
    {
        laneCount = params.laneCount;
        if (params.linkBW != 0)
        {
            DP_ASSERT((params.dp2LinkBW == 0) && "dp2LinkBW should be zero if linkBw is not zero");
            linkRate = LINK_RATE_270MHZ_TO_10MHZ((NvU64)params.linkBW);
        }
        else
        {
            // No link rate available.
            linkRate = 0;
        }
    }
    else
    {
        laneCount = 0;
        linkRate = 0;
    }
}

bool EvoMainLink::getMaxLinkConfigFromUefi(NvU8 &linkRate, NvU8 &laneCount)
{
    if (provider->getMaxLinkConfigFromUefi(linkRate, laneCount))
    {
        if (IS_VALID_LANECOUNT(laneCount) && IS_VALID_LINKBW(linkRate))
        {
            return true;
        }
    }
    return false;
}

bool EvoMainLink::queryAndUpdateDfpParams()
{
    NvU32 dfpFlags;
    dpMemZero(&dfpParams, sizeof(dfpParams));
    dfpParams.subDeviceInstance = subdeviceIndex;
    dfpParams.displayId = displayId;

    NvU32 code = provider->rmControl0073(NV0073_CTRL_CMD_DFP_GET_INFO, &dfpParams, sizeof(dfpParams));

    if (code != NVOS_STATUS_SUCCESS)
    {
        DP_ASSERT(0 && "Unable to query DFP params.");
        return false;
    }

    dfpFlags = dfpParams.flags;
    _isEDP = DRF_VAL(0073, _CTRL_DFP_FLAGS, _EMBEDDED_DISPLAYPORT, dfpFlags) ==
                     NV0073_CTRL_DFP_FLAGS_EMBEDDED_DISPLAYPORT_TRUE;

    if (_isLTPhyRepeaterSupported)
    {
        _rmPhyRepeaterCount = DRF_VAL(0073_CTRL_DFP, _FLAGS,
                                      _DP_PHY_REPEATER_COUNT, dfpFlags);
    }

    _needForceRmEdid = DRF_VAL(0073, _CTRL_DFP_FLAGS, _DP_FORCE_RM_EDID ,dfpFlags) ==
                               NV0073_CTRL_DFP_FLAGS_DP_FORCE_RM_EDID_TRUE;

    _isPC2Disabled   = DRF_VAL(0073, _CTRL_DFP_FLAGS, _DP_POST_CURSOR2_DISABLED, dfpFlags) ==
                               NV0073_CTRL_DFP_FLAGS_DP_POST_CURSOR2_DISABLED_TRUE;


    switch(DRF_VAL(0073, _CTRL_DFP_FLAGS, _DP_LINK_BW, dfpFlags))
    {
        default:
            DP_ASSERT(0 && "maxLinkRate is set improperly in dfp object.");
            // intentionally fall-thru.
        case NV0073_CTRL_DFP_FLAGS_DP_LINK_BW_1_62GBPS:
            _maxLinkRateSupportedDfp = dp2LinkRate_1_62Gbps;
            break;
        case NV0073_CTRL_DFP_FLAGS_DP_LINK_BW_2_70GBPS:
            _maxLinkRateSupportedDfp = dp2LinkRate_2_70Gbps;
            break;
        case NV0073_CTRL_DFP_FLAGS_DP_LINK_BW_5_40GBPS:
            _maxLinkRateSupportedDfp = dp2LinkRate_5_40Gbps;
            break;
        case NV0073_CTRL_DFP_FLAGS_DP_LINK_BW_8_10GBPS:
            _maxLinkRateSupportedDfp = dp2LinkRate_8_10Gbps;
            break;
    }


    _isDynamicMuxCapable = FLD_TEST_DRF(0073, _CTRL_DFP_FLAGS, _DYNAMIC_MUX_CAPABLE, _TRUE, dfpFlags);

    return true;
}

bool EvoMainLink::fetchEdidByRmCtrl(NvU8* edidBuffer, NvU32 bufferSize)
{
    NV0073_CTRL_SPECIFIC_GET_EDID_V2_PARAMS    *pEdidParams;
    pEdidParams = (NV0073_CTRL_SPECIFIC_GET_EDID_V2_PARAMS*) dpMalloc(sizeof(*pEdidParams));

    if (pEdidParams == NULL) {
        return false;
    }

    dpMemZero(pEdidParams, sizeof(*pEdidParams));
    pEdidParams->subDeviceInstance = subdeviceIndex;
    pEdidParams->displayId = displayId;
    pEdidParams->flags = 0;       // use default settings.

    NvU32 code = provider->rmControl0073(NV0073_CTRL_CMD_SPECIFIC_GET_EDID_V2, pEdidParams, sizeof(*pEdidParams));

    if (code == NVOS_STATUS_SUCCESS)
    {
        // Silently dropping part of a too-large output buffer matches the
        // behavior of the "V1" of this control.
        // But it may make sense to revisit this behavior now that it's under
        // control of this client.
        NvU32 copySize = NV_MIN(pEdidParams->bufferSize, bufferSize);
        dpMemCopy(edidBuffer, pEdidParams->edidBuffer, copySize);
    } else {
        DP_ASSERT(0 && "Unable to read EDID.");
    }

    dpFree(pEdidParams);
    return code == NVOS_STATUS_SUCCESS;
}

bool EvoMainLink::applyEdidOverrideByRmCtrl(NvU8* edidBuffer, NvU32 bufferSize)
{
    NV0073_CTRL_SPECIFIC_APPLY_EDID_OVERRIDE_V2_PARAMS *pEdidOverrideParams =
        (NV0073_CTRL_SPECIFIC_APPLY_EDID_OVERRIDE_V2_PARAMS *)
            dpMalloc(sizeof(*pEdidOverrideParams));

    if (pEdidOverrideParams == NULL) {
        return false;
    }

    dpMemZero(pEdidOverrideParams, sizeof(*pEdidOverrideParams));
    pEdidOverrideParams->subDeviceInstance = subdeviceIndex;
    pEdidOverrideParams->displayId = displayId;
    if (bufferSize > sizeof(pEdidOverrideParams->edidBuffer)) {
        DP_ASSERT(0 && "EDID override too large for edidBuffer");
        dpFree(pEdidOverrideParams);
        return false;
    }
    pEdidOverrideParams->bufferSize = bufferSize;
    dpMemCopy(&pEdidOverrideParams->edidBuffer, edidBuffer, bufferSize);

    NvU32 code = provider->rmControl0073(NV0073_CTRL_CMD_SPECIFIC_APPLY_EDID_OVERRIDE_V2,
                                         pEdidOverrideParams,
                                         sizeof(*pEdidOverrideParams));
    if (code != NVOS_STATUS_SUCCESS)
    {
        DP_ASSERT(0 && "Unable to apply EDID override.");
        dpFree(pEdidOverrideParams);
        return false;
    }

    DP_ASSERT(pEdidOverrideParams->bufferSize == bufferSize);
    dpMemCopy(edidBuffer, &pEdidOverrideParams->edidBuffer, bufferSize);

    dpFree(pEdidOverrideParams);

    return true;

}

bool EvoMainLink::isEDP()
{
    return _isEDP;
}

bool EvoMainLink::supportMSAOverMST()
{
    return _enableMSAOverrideOverMST;
}

bool EvoMainLink::skipPowerdownEdpPanelWhenHeadDetach()
{
    return _skipPowerdownEDPPanelWhenHeadDetach;
}

bool EvoMainLink::isMSTPCONCapsReadDisabled()
{
    return _isMSTPCONCapsReadDisabled;
}

bool EvoMainLink::isActive()
{
    NV0073_CTRL_SYSTEM_GET_ACTIVE_PARAMS params;

    for (int i = 0; i < 32; i++)
    {
        //
        //  Skip floorswept heads
        //
        if (!(allHeadMask & (1 << i)))
        {
            continue;
        }

        dpMemZero(&params, sizeof params);
        params.subDeviceInstance = 0;
        params.head = i;

        NvU32 code = provider->rmControl0073(NV0073_CTRL_CMD_SYSTEM_GET_ACTIVE, &params, sizeof(params));

        if (code != NVOS_STATUS_SUCCESS)
        {
            DP_ASSERT(0 && "We can't get active displays, RM bug!");
        }
        else if (params.displayId & displayId)
        {
            return true;
        }
    }

    return false;
}

bool EvoMainLink::controlRateGoverning(NvU32 head, bool enable, bool updateNow)
{
    NV0073_CTRL_CMD_DP_SET_RATE_GOV_PARAMS params = {0};
    params.subDeviceInstance = this->subdeviceIndex;
    params.head = head;
    params.sorIndex = provider->getSorIndex();

    if (enable)
    {
        params.flags |= DRF_DEF(0073_CTRL, _CMD_DP_SET_RATE_GOV_FLAGS, _ENABLE_RG, _ON);
    }
    else
    {
        params.flags |= DRF_DEF(0073_CTRL, _CMD_DP_SET_RATE_GOV_FLAGS, _ENABLE_RG, _OFF);
    }
    if (updateNow)
    {
        params.flags |= DRF_DEF(0073_CTRL, _CMD_DP_SET_RATE_GOV_FLAGS, _TRIGGER_MODE, _IMMEDIATE);
    }
    else
    {
        params.flags |= DRF_DEF(0073_CTRL, _CMD_DP_SET_RATE_GOV_FLAGS, _TRIGGER_MODE, _LOADV);
    }

    NvU32 ret = provider->rmControl0073(NV0073_CTRL_CMD_DP_SET_RATE_GOV, &params, sizeof params);
    if (ret != NVOS_STATUS_SUCCESS)
    {
        DP_PRINTF(DP_ERROR, "controlRateGoverning(): Set RateGov failed!");
    }
    return true;
}

bool EvoMainLink::getDpTestPattern(NV0073_CTRL_DP_TESTPATTERN * testPattern)
{
    NV0073_CTRL_DP_GET_TESTPATTERN_PARAMS params = {0};

    params.subDeviceInstance = this->subdeviceIndex;
    params.displayId = this->displayId;

    if (!(provider->rmControl0073(NV0073_CTRL_CMD_DP_GET_TESTPATTERN, &params, sizeof params)))
    {
        testPattern->testPattern = params.testPattern.testPattern;
        return true;
    }
    else
        return false;
}

bool EvoMainLink::setDpTestPattern(NV0073_CTRL_DP_TESTPATTERN testPattern, NvU8 laneMask, NV0073_CTRL_DP_CSTM cstm, NvBool bIsHBR2, NvBool bSkipLaneDataOverride)
{
    NV0073_CTRL_DP_SET_TESTPATTERN_PARAMS params = {0};

    params.subDeviceInstance = this->subdeviceIndex;
    params.displayId = this->displayId;
    params.testPattern = testPattern;
    params.laneMask = laneMask;
    params.cstm = cstm;
    params.bIsHBR2 = bIsHBR2;
    params.bSkipLaneDataOverride = bSkipLaneDataOverride;

    if (!(provider->rmControl0073(NV0073_CTRL_CMD_DP_SET_TESTPATTERN, &params, sizeof params)))
        return true;
    else
        return false;
}

bool EvoMainLink::getDpLaneData(NvU32 *numLanes, NvU32 *data)
{
    NV0073_CTRL_DP_LANE_DATA_PARAMS params = {0};

    params.subDeviceInstance = this->subdeviceIndex;
    params.displayId = this->displayId;

    if (!(provider->rmControl0073(NV0073_CTRL_CMD_DP_GET_LANE_DATA, &params, sizeof params)))
    {
        *numLanes = params.numLanes;
        dpMemCopy(data, params.data, NV0073_CTRL_MAX_LANES*4);
        return true;
    }
    else
        return false;
}

void EvoMainLink::getLinkConfigWithFEC(unsigned &laneCount, NvU64 & linkRate, bool &bFECEnabled)
{
    NV0073_CTRL_DP_GET_LINK_CONFIG_PARAMS params;
    dpMemZero(&params, sizeof(params));

    params.subDeviceInstance = subdeviceIndex;
    params.displayId = displayId;

    NvU32 code = provider->rmControl0073(NV0073_CTRL_CMD_DP_GET_LINK_CONFIG, &params, sizeof(params));

    if (code == NVOS_STATUS_SUCCESS)
    {
        laneCount = params.laneCount;
        if (params.bFECEnabled)
        {
            bFECEnabled = true;
        }

        if (params.linkBW != 0)
        {
            DP_ASSERT((params.dp2LinkBW == 0) && "dp2LinkBW should be zero if linkBw is not zero");
            linkRate = LINK_RATE_270MHZ_TO_10MHZ((NvU64)params.linkBW);
        }
        else
        {
            // No link rate available.
            linkRate = 0;
        }
    }
    else
    {
        laneCount = 0;
        linkRate = 0;
    }
}

bool EvoMainLink::setDpLaneData(NvU32 numLanes, NvU32 *data)
{
    NV0073_CTRL_DP_LANE_DATA_PARAMS params = {0};

    params.subDeviceInstance = this->subdeviceIndex;
    params.displayId = this->displayId;
    params.numLanes = numLanes;
    dpMemCopy(params.data, data, NV0073_CTRL_MAX_LANES*4);

    if (!(provider->rmControl0073(NV0073_CTRL_CMD_DP_SET_LANE_DATA, &params, sizeof params)))
        return true;
    else
        return false;
}

NvU32 EvoMainLink::monitorDenylistInfo(NvU32 ManufacturerID, NvU32 ProductID, DpMonitorDenylistData *pDenylistData)
{
    return provider->monitorDenylistInfo(ManufacturerID, ProductID, pDenylistData);
}
bool EvoMainLink::rmUpdateDynamicDfpCache(NvU32 headIndex, RmDfpCache* dfpCache, NvBool bResetDfp)
{
    NV0073_CTRL_DFP_UPDATE_DYNAMIC_DFP_CACHE_PARAMS params = {0};
    params.headIndex = headIndex;
    params.bcaps = dfpCache->bcaps;
    for (unsigned i=0; i<5; i++)
        params.bksv[i] = dfpCache->bksv[i];

    params.bHdcpCapable = dfpCache->hdcpCapable;
    params.subDeviceInstance = subdeviceIndex;
    params.updateMask = dfpCache->updMask;
    if (bResetDfp)
        params.bResetDfp = NV_TRUE;

    if (!(provider->rmControl0073(NV0073_CTRL_CMD_DFP_UPDATE_DYNAMIC_DFP_CACHE, &params, sizeof params)))
        return true;
    else
        return false;
}

NvU32 EvoMainLink::allocDisplayId()
{
    NV0073_CTRL_CMD_DP_TOPOLOGY_ALLOCATE_DISPLAYID_PARAMS params = {0};

    params.subDeviceInstance = subdeviceIndex;
    params.displayId = displayId;

    NvU32 ret = provider->rmControl0073(NV0073_CTRL_CMD_DP_TOPOLOGY_ALLOCATE_DISPLAYID, &params, sizeof(params));
    if (ret == NVOS_STATUS_SUCCESS)
    {
        return params.displayIdAssigned;
    }

    return 0;
}

bool EvoMainLink::freeDisplayId(NvU32 displayId)
{
    NV0073_CTRL_CMD_DP_TOPOLOGY_FREE_DISPLAYID_PARAMS params = {0};

    params.subDeviceInstance = subdeviceIndex;
    params.displayId = displayId;

    NvU32 ret = provider->rmControl0073(NV0073_CTRL_CMD_DP_TOPOLOGY_FREE_DISPLAYID, &params, sizeof(params));
    return ret == NVOS_STATUS_SUCCESS;
}

void EvoMainLink::configureTriggerSelect(NvU32 head, DP_SINGLE_HEAD_MULTI_STREAM_PIPELINE_ID streamIdentifier)
{
    NV0073_CTRL_CMD_DP_SET_TRIGGER_SELECT_PARAMS params = {0};
    params.head = head;
    params.subDeviceInstance = subdeviceIndex;
    params.sorIndex = provider->getSorIndex();
    params.singleHeadMSTPipeline = streamIdentifier;
    NvU32 ret = provider->rmControl0073(NV0073_CTRL_CMD_DP_SET_TRIGGER_SELECT, &params, sizeof params);
    if (ret != NVOS_STATUS_SUCCESS)
    {
        DP_PRINTF(DP_ERROR, "configureTriggerSelect(): Set Trigger Select failed!");
    }
}

void EvoMainLink::configureTriggerAll(NvU32 head, bool enable)
{
    NV0073_CTRL_CMD_DP_SET_TRIGGER_ALL_PARAMS params = {0};
    params.head = head;
    params.subDeviceInstance = subdeviceIndex;
    params.enable = enable;
    NvU32 ret = provider->rmControl0073(NV0073_CTRL_CMD_DP_SET_TRIGGER_ALL, &params, sizeof params);
    if (ret != NVOS_STATUS_SUCCESS)
    {
        DP_PRINTF(DP_ERROR, "configureTriggerAll(): Set Trigger All failed!");
    }
}

MainLink * DisplayPort::MakeEvoMainLink(EvoInterface * provider, Timer * timer)
{
    MainLink *main;
    NvU32 nvosStatus;
    NV0073_CTRL_CMD_DP_GET_CAPS_PARAMS dpParams = {0};

    dpParams.subDeviceInstance = provider->getSubdeviceIndex();
    nvosStatus = provider->rmControl0073(NV0073_CTRL_CMD_DP_GET_CAPS, &dpParams, sizeof dpParams);

    if (nvosStatus != NVOS_STATUS_SUCCESS)
    {
        DP_ASSERT(0 && "Unable to get DP caps params");
        return NULL;
    }

    if (FLD_TEST_DRF(0073_CTRL_CMD_DP, _GET_CAPS_DP_VERSIONS_SUPPORTED,
                     _DP2_0, _YES, dpParams.dpVersionsSupported))
    {
        main = new EvoMainLink2x(provider, timer);
    }
    else
    {
        main = new EvoMainLink(provider, timer);
    }
    return main;
}

AuxBus   * DisplayPort::MakeEvoAuxBus(EvoInterface * provider, Timer * timer)
{
    return new EvoAuxBus(provider, timer);
}

bool EvoMainLink::dscCrcTransaction(NvBool bEnable, gpuDscCrc *data, NvU16 *headIndex)
{
    NV0073_CTRL_DFP_DSC_CRC_CONTROL_PARAMS params;
    NvU32 code;

    dpMemZero(&params, sizeof(params));
    params.bEnable                 = bEnable ? NV_TRUE : NV_FALSE;
    params.subDeviceInstance       = subdeviceIndex;
    params.headIndex               = *headIndex;

    // see if setup or querying needs to be specified
    if (data == NULL)
    {
        params.cmd = DRF_DEF(0073_CTRL, _DP_CRC_CONTROL, _CMD, _SETUP);
    }
    else
    {
        params.cmd = DRF_DEF(0073_CTRL, _DP_CRC_CONTROL, _CMD, _QUERY);
    }

    // GPU part of the call
    code = provider->rmControl0073(NV0073_CTRL_CMD_DFP_DSC_CRC_CONTROL, &params, sizeof(params));
    if (code != NVOS_STATUS_SUCCESS)
    {
        DP_PRINTF(DP_ERROR, "DP> Crc control failed.");
        return false;
    }

    // if the command is setup, return immediately
    if (data != NULL)
    {
        data->gpuCrc0 = params.gpuCrc0;
        data->gpuCrc1 = params.gpuCrc1;
        data->gpuCrc2 = params.gpuCrc2;
    }

    return true;
}

//
// @brief This is to request RM to setup/reset link rate table, and save valid
//        link rates for use.
//
// @param pLinkRateTable    Pointer to link rate table to configure
// @param pLinkRates        Pointer to LinkRates to keep valid link rates
// @return
//   true                   Link rate table configured with at least one valid link rate
//   false                  Otherwise
//
bool EvoMainLink::configureLinkRateTable
(
    const NvU16     *pLinkRateTable,
    LinkRates       *pLinkRates
)
{
    NV0073_CTRL_CMD_DP_CONFIG_INDEXED_LINK_RATES_PARAMS params;
    dpMemZero(&params, sizeof(params));

    params.subDeviceInstance = subdeviceIndex;
    params.displayId = displayId;

    // Setup provided link rate table, otherwise it will be reset
    if (pLinkRateTable)
    {
        for (NvU32 i = 0; i < NV0073_CTRL_DP_MAX_INDEXED_LINK_RATES; i++)
        {
            params.linkRateTbl[i] = pLinkRateTable[i];
        }
    }

    NvU32 code = provider->rmControl0073(NV0073_CTRL_CMD_DP_CONFIG_INDEXED_LINK_RATES, &params, sizeof(params));

    if ((pLinkRates != NULL ) && (code == NVOS_STATUS_SUCCESS) &&
        (params.linkBwCount <= NV0073_CTRL_DP_MAX_INDEXED_LINK_RATES))
    {
        pLinkRates->clear();
        for (int i = 0; i < params.linkBwCount; i++)
        {
            switch (params.linkBwTbl[i])
            {
                case dp2LinkRate_1_62Gbps:
                case dp2LinkRate_2_16Gbps:
                case dp2LinkRate_2_43Gbps:
                case dp2LinkRate_2_70Gbps:
                case dp2LinkRate_3_24Gbps:
                case dp2LinkRate_4_32Gbps:
                case dp2LinkRate_5_40Gbps:
                case dp2LinkRate_8_10Gbps:
                    pLinkRates->import((NvU16)params.linkBwTbl[i]);
                    break;
                default:
                    DP_PRINTF(DP_ERROR, "DP_EVO> %s: Unsupported link rate received",
                             __FUNCTION__);
                    DP_ASSERT(0);
                    break;
            }
        }
        return true;
    }
    return false;
}

//
// @brief This is to request RM to enable/disable Fec
//
// @param enableFec         Indicates if enable/disable is requested
// @return
//   true                   If FEC was configured successfully
//   false                  Otherwise
//
bool EvoMainLink::configureFec
(
    const bool bEnableFec
)
{
    NV0073_CTRL_CMD_DP_CONFIGURE_FEC_PARAMS params;
    dpMemZero(&params, sizeof(params));

    params.subDeviceInstance = subdeviceIndex;
    params.displayId = displayId;
    params.bEnableFec = bEnableFec;

    NvU32 code = provider->rmControl0073(NV0073_CTRL_CMD_DP_CONFIGURE_FEC, &params, sizeof(params));

    if (code == NVOS_STATUS_SUCCESS)
    {
        return true;
    }

    return false;
}
