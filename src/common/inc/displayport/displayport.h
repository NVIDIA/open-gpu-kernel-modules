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

#ifndef _DISPLAYPORT_H_
#define _DISPLAYPORT_H_

#include "nvmisc.h"
#include "dpcd.h"
#include "dpcd14.h"
#include "dpcd20.h"

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
* Module: DISPLAYPORT.H                                                     *
*       Defines DISPLAYPORT V1.2                                            *
*                                                                           *
\***************************************************************************/

// Displayport interoperability with HDMI dongle i2c addr
#define DP2HDMI_DONGLE_I2C_ADDR                         0x80
#define DP2HDMI_DONGLE_DDC_BUFFER_ID_LEN                16
#define DP2HDMI_DONGLE_CAP_BUFFER_LEN                   32

// Offset to read the dongle identifier
#define NV_DP2HDMI_DONGLE_IDENTIFIER                    (0x00000010)
#define NV_DP2HDMI_DONGLE_IDENTIFIER_ADAPTER_REV                2:0
#define NV_DP2HDMI_DONGLE_IDENTIFIER_ADAPTER_REV_TYPE2  (0x00000000)
#define NV_DP2HDMI_DONGLE_IDENTIFIER_ADAPTER_ID                 7:4
#define NV_DP2HDMI_DONGLE_IDENTIFIER_ADAPTER_ID_TYPE2   (0x0000000A)

// Offset to read the dongle TMDS clock rate
#define NV_DP2HDMI_DONGLE_TMDS_CLOCK_RATE               (0x0000001D)

// HDMI dongle types
#define DP2HDMI_DONGLE_TYPE_1                           0x1
#define DP2HDMI_DONGLE_TYPE_2                           0x2

// HDMI dongle frequency limits
#define DP2HDMI_DONGLE_TYPE_1_PCLK_LIMIT                165*1000*1000
#define DP2HDMI_DONGLE_TYPE_2_PCLK_LIMIT                300*1000*1000

#define DPCD_VERSION_12                                 0x12
#define DPCD_VERSION_13                                 0x13
#define DPCD_VERSION_14                                 0x14

#define DP_LINKINDEX_0                                  0x0
#define DP_LINKINDEX_1                                  0x1

// Two Head One OR
#define NV_PRIMARY_HEAD_INDEX_0                         0
#define NV_SECONDARY_HEAD_INDEX_1                       1
#define NV_PRIMARY_HEAD_INDEX_2                         2
#define NV_SECONDARY_HEAD_INDEX_3                       3

typedef enum
{
    displayPort_Lane0              = 0,
    displayPort_Lane1              = 1,
    displayPort_Lane2              = 2,
    displayPort_Lane3              = 3,
    displayPort_Lane4              = 4,
    displayPort_Lane5              = 5,
    displayPort_Lane6              = 6,
    displayPort_Lane7              = 7,
    displayPort_LaneSupported
} DP_LANE;

typedef enum
{
    laneCount_0                    = 0x0,
    laneCount_1                    = 0x1,
    laneCount_2                    = 0x2,
    laneCount_4                    = 0x4,
    laneCount_8                    = 0x8,
    laneCount_Supported
} DP_LANE_COUNT;

typedef enum
{
    // enum value unit = 270M
    linkBW_1_62Gbps                = 0x06,
    linkBW_2_16Gbps                = 0x08,
    linkBW_2_43Gbps                = 0x09,
    linkBW_2_70Gbps                = 0x0A,
    linkBW_3_24Gbps                = 0x0C,
    linkBW_4_32Gbps                = 0x10,
    linkBW_5_40Gbps                = 0x14,
    linkBW_8_10Gbps                = 0x1E,
    linkBW_Supported
} DP_LINK_BANDWIDTH;

typedef enum
{
    linkSpeedId_1_62Gbps                = 0x00,
    linkSpeedId_2_70Gbps                = 0x01,
    linkSpeedId_5_40Gbps                = 0x02,
    linkSpeedId_8_10Gbps                = 0x03,
    linkSpeedId_2_16Gbps                = 0x04,
    linkSpeedId_2_43Gbps                = 0x05,
    linkSpeedId_3_24Gbps                = 0x06,
    linkSpeedId_4_32Gbps                = 0x07,
    linkSpeedId_Supported
} DP_LINK_SPEED_INDEX;

typedef enum
{
    postCursor2_Level0             = 0,
    postCursor2_Level1             = 1,
    postCursor2_Level2             = 2,
    postCursor2_Level3             = 3,
    postCursor2_Supported
} DP_POSTCURSOR2;

typedef enum
{
    preEmphasis_Disabled            = 0,
    preEmphasis_Level1              = 1,
    preEmphasis_Level2              = 2,
    preEmphasis_Level3              = 3,
    preEmphasis_Supported
} DP_PREEMPHASIS;

typedef enum
{
    driveCurrent_Level0             = 0,
    driveCurrent_Level1             = 1,
    driveCurrent_Level2             = 2,
    driveCurrent_Level3             = 3,
    driveCurrent_Supported
} DP_DRIVECURRENT;

typedef enum
{
    trainingPattern_Disabled        = 0x0,
    trainingPattern_1               = 0x1,
    trainingPattern_2               = 0x2,
    trainingPattern_3               = 0x3,
    trainingPattern_4               = 0xB
} DP_TRAININGPATTERN;

typedef enum
{
    dpOverclock_Percentage_0        =  0,
    dpOverclock_Percentage_10       = 10,
    dpOverclock_Percentage_20       = 20
}DP_OVERCLOCKPERCENTAGE;

typedef enum
{
    dpColorFormat_RGB              = 0,
    dpColorFormat_YCbCr444         = 0x1,
    dpColorFormat_YCbCr422         = 0x2,
    dpColorFormat_YCbCr420         = 0x3,
    dpColorFormat_Unknown          = 0xF
} DP_COLORFORMAT;

typedef enum
{
    dp_pktType_VideoStreamconfig     = 0x7,
    dp_pktType_CeaHdrMetaData        = 0x21,
    dp_pktType_SRInfoFrame           = 0x7f, // Self refresh infoframe for eDP enter/exit self refresh, SRS 1698
    dp_pktType_Cea861BInfoFrame      = 0x80,
    dp_pktType_VendorSpecInfoFrame   = 0x81,
    dp_pktType_AviInfoFrame          = 0x82,
    dp_pktType_AudioInfoFrame        = 0x84,
    dp_pktType_SrcProdDescInfoFrame  = 0x83,
    dp_pktType_MpegSrcInfoFrame      = 0x85,
    dp_pktType_DynamicRangeMasteringInfoFrame = 0x87
} DP_PACKET_TYPE;

typedef enum
{
    DSC_SLICES_PER_SINK_1 = 1,
    DSC_SLICES_PER_SINK_2 = 2,
    DSC_SLICES_PER_SINK_4 = 4,
    DSC_SLICES_PER_SINK_6 = 6,
    DSC_SLICES_PER_SINK_8 = 8,
    DSC_SLICES_PER_SINK_10 = 10,
    DSC_SLICES_PER_SINK_12 = 12,
    DSC_SLICES_PER_SINK_16 = 16,
    DSC_SLICES_PER_SINK_20 = 20,
    DSC_SLICES_PER_SINK_24 = 24
} DscSliceCount;

typedef enum
{
    DSC_BITS_PER_COLOR_MASK_8    = 1,
    DSC_BITS_PER_COLOR_MASK_10   = 2,
    DSC_BITS_PER_COLOR_MASK_12   = 4
}DscBitsPerColorMask;

enum DSC_MODE
{
    DSC_SINGLE,
    DSC_DUAL,
    DSC_DROP,
    DSC_MODE_NONE
};

typedef enum
{
    BITS_PER_PIXEL_PRECISION_1_16 = 0,
    BITS_PER_PIXEL_PRECISION_1_8 = 1,
    BITS_PER_PIXEL_PRECISION_1_4 = 2,
    BITS_PER_PIXEL_PRECISION_1_2 = 3,
    BITS_PER_PIXEL_PRECISION_1   = 4
}BITS_PER_PIXEL_INCREMENT;

typedef enum
{
    NV_DP_FEC_UNCORRECTED   = 0,
    NV_DP_FEC_CORRECTED     = 1,
    NV_DP_FEC_BIT           = 2,
    NV_DP_FEC_PARITY_BLOCK  = 3,
    NV_DP_FEC_PARITY_BIT    = 4
}FEC_ERROR_COUNTER;

typedef struct DscCaps
{
    NvBool bDSCSupported;
    NvBool bDSCDecompressionSupported;
    NvBool bDSCPassThroughSupported;
    unsigned versionMajor, versionMinor;
    unsigned rcBufferBlockSize;
    unsigned rcBuffersize;
    unsigned maxSlicesPerSink;
    unsigned lineBufferBitDepth;
    NvBool   bDscBlockPredictionSupport;
    unsigned maxBitsPerPixelX16;
    unsigned sliceCountSupportedMask;

    struct
    {
        NvBool bRgb;
        NvBool bYCbCr444;
        NvBool bYCbCrSimple422;
        NvBool bYCbCrNative422;
        NvBool bYCbCrNative420;
    }dscDecoderColorFormatCaps;

    unsigned dscDecoderColorDepthMask;
    unsigned dscPeakThroughputMode0;
    unsigned dscPeakThroughputMode1;
    unsigned dscMaxSliceWidth;

    unsigned branchDSCOverallThroughputMode0;
    unsigned branchDSCOverallThroughputMode1;
    unsigned branchDSCMaximumLineBufferWidth;

    BITS_PER_PIXEL_INCREMENT dscBitsPerPixelIncrement;
} DscCaps;

typedef struct GpuDscCrc
{
    NvU16 gpuCrc0;
    NvU16 gpuCrc1;
    NvU16 gpuCrc2;
} gpuDscCrc;

typedef struct SinkDscCrc
{
    NvU16 sinkCrc0;
    NvU16 sinkCrc1;
    NvU16 sinkCrc2;
} sinkDscCrc;

typedef struct
{
    NvBool  bSourceControlModeSupported;
    NvBool  bConcurrentLTSupported;
    NvBool  bConv444To420Supported;
    NvU32   maxTmdsClkRate;
    NvU8    maxBpc;
    NvU8    maxHdmiLinkBandwidthGbps;
} PCONCaps;

typedef enum
{
    PCON_HDMI_LINK_BW_FRL_9GBPS = 0,
    PCON_HDMI_LINK_BW_FRL_18GBPS,
    PCON_HDMI_LINK_BW_FRL_24GBPS,
    PCON_HDMI_LINK_BW_FRL_32GBPS,
    PCON_HDMI_LINK_BW_FRL_40GBPS,
    PCON_HDMI_LINK_BW_FRL_48GBPS,
    PCON_HDMI_LINK_BW_FRL_INVALID
} PCONHdmiLinkBw;

typedef enum
{
    NV_DP_PCON_CONTROL_STATUS_SUCCESS                          = 0,
    NV_DP_PCON_CONTROL_STATUS_ERROR_TIMEOUT                    = 0x80000001,
    NV_DP_PCON_CONTROL_STATUS_ERROR_FRL_LT_FAILURE             = 0x80000002,
    NV_DP_PCON_CONTROL_STATUS_ERROR_FRL_NOT_SUPPORTED          = 0x80000003,
    NV_DP_PCON_CONTROL_STATUS_ERROR_GENERIC                    = 0x8000000F
} NV_DP_PCON_CONTROL_STATUS;
//
// Poll HDMI-Link Status change and FRL Ready.
// Spec says it should be done in 500ms, we give it 20% extra time:
// 60 times with interval 10ms.
//
#define NV_PCON_SOURCE_CONTROL_MODE_TIMEOUT_THRESHOLD            (60)
#define NV_PCON_SOURCE_CONTROL_MODE_TIMEOUT_INTERVAL_MS          (10)
//
// Poll HDMI-Link Status change IRQ and Link Status.
// Spec says it should be done in 250ms, we give it 20% extra time:
// 30 times with interval 10ms.
//
#define NV_PCON_FRL_LT_TIMEOUT_THRESHOLD            (30)
#define NV_PCON_FRL_LT_TIMEOUT_INTERVAL_MS          (10)

typedef struct _PCONLinkControl
{
    struct
    {
        // This struct is being passed in for assessPCONLink I/F
        NvU32 bAssessLink        : 1;

        // Specify if client wants to use src control - set it false DPLib can just do DP LT alone.
        // By default it should be true.
        NvU32 bSourceControlMode : 1;

        // Default is sequential mode, set this to choose concurrent mode
        NvU32 bConcurrentMode    : 1;

        // Default is normal link training mode (stop once FRL-LT succeed).
        // Set this to link train all requested FRL Bw in allowedFrlBwMask.
        NvU32 bExtendedLTMode    : 1;

        // Keep PCON links (DP and FRL link) alive
        NvU32 bKeepPCONLinkAlive  : 1;

        // Default DPLib will fallback to autonomous mode and perform DP assessLink.
        NvU32 bSkipFallback       : 1;
    } flags;

    // Input: Clients use this to specify the FRL BW PCON should try.
    NvU32          frlHdmiBwMask;

    struct
    {
        NV_DP_PCON_CONTROL_STATUS   status;
        PCONHdmiLinkBw              maxFrlBwTrained;
        NvU32                       trainedFrlBwMask;
    } result;
} PCONLinkControl;

static NV_INLINE PCONHdmiLinkBw getMaxFrlBwFromMask(NvU32 frlRateMask)
{
    if (frlRateMask == 0)
    {
        // Nothing is set. Assume TMDS
        return PCON_HDMI_LINK_BW_FRL_INVALID;
    }

    // find highest set bit (destructive operation)
    HIGHESTBITIDX_32(frlRateMask);

    return (PCONHdmiLinkBw)frlRateMask;
}

/*
    EDP VESA PSR defines
*/

// PSR state transitions
typedef enum
{
    vesaPsrStatus_Inactive            = 0,
    vesaPsrStatus_Transition2Active   = 1,
    vesaPsrStatus_DisplayFromRfb      = 2,
    vesaPsrStatus_CaptureAndDisplay   = 3,
    vesaPsrStatus_Transition2Inactive = 4,
    vesaPsrStatus_Undefined5          = 5,
    vesaPsrStatus_Undefined6          = 6,
    vesaPsrStatus_SinkError           = 7
} vesaPsrState;

typedef struct VesaPsrConfig
{
    NvU8 psrCfgEnable : 1;
    NvU8 srcTxEnabledInPsrActive : 1;
    NvU8 crcVerifEnabledInPsrActive : 1;
    NvU8 frameCaptureSecondActiveFrame : 1;
    NvU8 selectiveUpdateOnSecondActiveline : 1;
    NvU8 enableHpdIrqOnCrcMismatch : 1;
    NvU8 enablePsr2 : 1;
    NvU8 reserved : 1;
} vesaPsrConfig;

typedef struct VesaPsrDebugStatus
{
    NvBool lastSdpPsrState;
    NvBool lastSdpUpdateRfb;
    NvBool lastSdpCrcValid;
    NvBool lastSdpSuValid;
    NvBool lastSdpFirstSURcvd;
    NvBool lastSdpLastSURcvd;
    NvBool lastSdpYCoordValid;
    NvU8   maxResyncFrames;
    NvU8   actualResyncFrames;
} vesaPsrDebugStatus;

typedef struct VesaPsrErrorStatus
{
    NvU8 linkCrcError  : 1;
    NvU8 rfbStoreError : 1;
    NvU8 vscSdpError : 1;
    NvU8 rsvd : 5;
} vesaPsrErrorStatus;

typedef struct VesaPsrEventIndicator
{
    NvU8 sinkCapChange : 1;
    NvU8 rsvd : 7;
} vesaPsrEventIndicator;

#pragma pack(1)
typedef struct VesaPsrSinkCaps
{
    NvU8  psrVersion;
    NvU8  linkTrainingRequired : 1;
    NvU8  psrSetupTime : 3;
    NvU8  yCoordinateRequired : 1;
    NvU8  psr2UpdateGranularityRequired : 1;
    NvU8  reserved : 2;
    NvU16 suXGranularity;
    NvU8  suYGranularity;
} vesaPsrSinkCaps;
#pragma pack()

typedef struct PanelReplayCaps
{
    NvBool panelReplaySupported;
} panelReplayCaps;

typedef struct PanelReplayConfig
{
    NvBool   enablePanelReplay;
} panelReplayConfig;

// PR state
typedef enum
{
    PanelReplay_Inactive            = 0,
    PanelReplay_CaptureAndDisplay   = 1,
    PanelReplay_DisplayFromRfb      = 2,
    PanelReplay_Undefined           = 7
} PanelReplayState;

typedef struct
{
    PanelReplayState prState;
} PanelReplayStatus;

// Multiplier constant to get link frequency in KHZ
// Maximum link rate of Main Link lanes = Value x 270M.
// To get it to KHz unit, we need to multiply 270K.
#define DP_LINK_BW_FREQUENCY_MULTIPLIER_KHZ             (270*1000)

// Multiplier constant to get link rate table's in KHZ
#define DP_LINK_RATE_TABLE_MULTIPLIER_KHZ 200

//
// Multiplier constant to get link frequency (multiplier of 270MHz) in MBps
// a * 270 * 1000 * 1000(270Mhz) * (8 / 10)(8b/10b) / 8(Byte)
// = a * 27000000
//
#define DP_LINK_BW_FREQ_MULTI_MBPS 27000000

//
// Get link rate in multiplier of 270MHz from KHz:
// a * 1000(KHz) / 270 * 1000 * 1000(270Mhz)
//
#define LINK_RATE_KHZ_TO_MULTP(a) ((a) / 270000)

//
// Get link rate in MBps from KHz:
// a * 1000 * (8 / 10)(8b/10b) / 8(Byte)
// = a * 100
//
#define LINK_RATE_KHZ_TO_MBPS(a) ((a) * 100)

#define DP_MAX_LANES                                8   // This defines the maximum number of lanes supported on a chip.
#define DP_MAX_LANES_PER_LINK                       4   // This defines the maximum number of lanes per link in a chip.
#define DP_AUX_CHANNEL_MAX_BYTES                   16
#define DP_CLOCK_RECOVERY_TOT_TRIES                10
#define DP_CLOCK_RECOVERY_MAX_TRIES                 5
#define DP_CH_EQ_MAX_RETRIES                        5
#define DP_LT_MAX_FOR_MST_MAX_RETRIES               3
#define DP_READ_EDID_MAX_RETRIES                    7
#define DP_AUX_CHANNEL_DEFAULT_DEFER_MAX_TRIES      7
#define DP_AUX_CHANNEL_TIMEOUT_MAX_TRIES            2
#define DP_SET_POWER_D0_NORMAL_MAX_TRIES            3
#define DP_SW_AUTO_READ_REQ_SIZE                    6
#define NV_DP_RBR_FALLBACK_MAX_TRIES                3

#define DP_EXTENDED_DPRX_SLEEP_WAKE_TIMEOUT_DEFAULT_MS      1

#define DP_AUX_CHANNEL_TIMEOUT_WAITIDLE             400  // source is required to wait at least 400us before it considers the AUX transaction to have timed out.
#define DP_AUX_CHANNEL_TIMEOUT_VALUE_DEFAULT        400
#define DP_AUX_CHANNEL_TIMEOUT_VALUE_MAX            3200

#define DP_PHY_REPEATER_INDEX_FOR_SINK              0xFFFFFFFF

#define DP_MESSAGEBOX_SIZE                          48
#define DP_POST_LT_ADJ_REQ_LIMIT                    6
#define DP_POST_LT_ADJ_REQ_TIMER                    200000

#define DP_AUX_HYBRID_TIMEOUT                         600
#define DP_AUX_SEMA_ACQUIRE_TIMEOUT                 20000

#define DP_CONFIG_WATERMARK_ADJUST                   2
#define DP_CONFIG_WATERMARK_LIMIT                   20
#define DP_CONFIG_INCREASED_WATERMARK_ADJUST         8
#define DP_CONFIG_INCREASED_WATERMARK_LIMIT         22

#define NV_DP_MSA_PROPERTIES_MISC1_STEREO 2:1

#define DP_LANE_STATUS_ARRAY_SIZE ((displayPort_LaneSupported + 1) / 2)
#define DP_LANE_STATUS_ARRAY_INDEX(lane) ((lane) < displayPort_LaneSupported ? ((lane) / 2) : 0)

#define IS_VALID_LANECOUNT(val) (((NvU32)(val)==0) || ((NvU32)(val)==1) || \
                                 ((NvU32)(val)==2) || ((NvU32)(val)==4) || \
                                 ((NvU32)(val)==8))

#define IS_STANDARD_LINKBW(val) (((NvU32)(val)==linkBW_1_62Gbps) || \
                                 ((NvU32)(val)==linkBW_2_70Gbps) || \
                                 ((NvU32)(val)==linkBW_5_40Gbps) || \
                                 ((NvU32)(val)==linkBW_8_10Gbps))

#define IS_INTERMEDIATE_LINKBW(val) (((NvU32)(val)==linkBW_2_16Gbps) || \
                                     ((NvU32)(val)==linkBW_2_43Gbps) || \
                                     ((NvU32)(val)==linkBW_3_24Gbps) || \
                                     ((NvU32)(val)==linkBW_4_32Gbps))

#define IS_VALID_LINKBW(val) (IS_STANDARD_LINKBW(val)     || \
                              IS_INTERMEDIATE_LINKBW(val))

//
// Phy Repeater count read from DPCD offset F0002h is an
// 8 bit value where each bit represents the total count
// 80h = 1 repeater, 40h = 2 , 20h = 3 ... 01h = 8
// This function maps it to decimal system
//
static NV_INLINE NvU32 mapPhyRepeaterVal(NvU32 value)
{
    switch (value)
    {
        case NV_DPCD14_PHY_REPEATER_CNT_VAL_0:
            return 0;
        case NV_DPCD14_PHY_REPEATER_CNT_VAL_1:
            return 1;
        case NV_DPCD14_PHY_REPEATER_CNT_VAL_2:
            return 2;
        case NV_DPCD14_PHY_REPEATER_CNT_VAL_3:
            return 3;
        case NV_DPCD14_PHY_REPEATER_CNT_VAL_4:
            return 4;
        case NV_DPCD14_PHY_REPEATER_CNT_VAL_5:
            return 5;
        case NV_DPCD14_PHY_REPEATER_CNT_VAL_6:
            return 6;
        case NV_DPCD14_PHY_REPEATER_CNT_VAL_7:
            return 7;
        case NV_DPCD14_PHY_REPEATER_CNT_VAL_8:
            return 8;
        default:
            return 0;
    }
}

// HDCP specific definitions

#define HDCP22_RTX_SIMPLE_PATTERN           0x12345678
#define HDCP22_TX_CAPS_PATTERN_BIG_ENDIAN   {0x02, 0x00, 0x00}

#define DP_MST_HEAD_TO_STREAMID(head, pipeId, numHeads)     ((head) + 1 + (pipeId) * (numHeads))
#define DP_MST_STREAMID_TO_HEAD(streamid, pipeId, numHeads) ((streamid) - 1 - ((pipeId) * (numHeads)))
#define DP_MST_STREAMID_TO_PIPE(streamid, head, numHeads)   (((streamid) - (head) - 1) / (numHeads))

typedef enum
{
    NV_DP_SBMSG_REQUEST_ID_GET_MESSAGE_TRANSACTION_VERSION  = 0x00,
    NV_DP_SBMSG_REQUEST_ID_LINK_ADDRESS                     = 0x01,
    NV_DP_SBMSG_REQUEST_ID_CONNECTION_STATUS_NOTIFY         = 0x02,

    NV_DP_SBMSG_REQUEST_ID_ENUM_PATH_RESOURCES              = 0x10,
    NV_DP_SBMSG_REQUEST_ID_ALLOCATE_PAYLOAD                 = 0x11,
    NV_DP_SBMSG_REQUEST_ID_QUERY_PAYLOAD                    = 0x12,
    NV_DP_SBMSG_REQUEST_ID_RESOURCE_STATUS_NOTIFY           = 0x13,
    NV_DP_SBMSG_REQUEST_ID_CLEAR_PAYLOAD_ID_TABLE           = 0x14,

    NV_DP_SBMSG_REQUEST_ID_REMOTE_DPCD_READ                 = 0x20,
    NV_DP_SBMSG_REQUEST_ID_REMOTE_DPCD_WRITE                = 0x21,
    NV_DP_SBMSG_REQUEST_ID_REMOTE_I2C_READ                  = 0x22,
    NV_DP_SBMSG_REQUEST_ID_REMOTE_I2C_WRITE                 = 0x23,
    NV_DP_SBMSG_REQUEST_ID_POWER_UP_PHY                     = 0x24,
    NV_DP_SBMSG_REQUEST_ID_POWER_DOWN_PHY                   = 0x25,

    NV_DP_SBMSG_REQUEST_ID_SINK_EVENT_NOTIFY                = 0x30,
    NV_DP_SBMSG_REQUEST_ID_QUERY_STREAM_ENCRYPTION_STATUS   = 0x38,

    NV_DP_SBMSG_REQUEST_ID_UNDEFINED                        = 0xFF,
} NV_DP_SBMSG_REQUEST_ID;

// FEC

#define NV_DP_FEC_FLAGS_SELECT_ALL       0x7
#define NV_DP_ERROR_COUNTERS_PER_LANE    5
#define NV_DP_MAX_NUM_OF_LANES           4
#define NV_DP_FEC_ERROR_COUNT_INVALID    0xbadf
#define NV_DP_UNCORRECTED_ERROR          NV_DP_FEC_UNCORRECTED  : NV_DP_FEC_UNCORRECTED
#define NV_DP_CORRECTED_ERROR            NV_DP_FEC_CORRECTED    : NV_DP_FEC_CORRECTED
#define NV_DP_BIT_ERROR                  NV_DP_FEC_BIT          : NV_DP_FEC_BIT
#define NV_DP_PARITY_BLOCK_ERROR         NV_DP_FEC_PARITY_BLOCK : NV_DP_FEC_PARITY_BLOCK
#define NV_DP_PARITY_BIT_ERROR           NV_DP_FEC_PARITY_BIT   : NV_DP_FEC_PARITY_BIT
#define NV_DP_UNCORRECTED_ERROR_NO       0
#define NV_DP_UNCORRECTED_ERROR_YES      1
#define NV_DP_CORRECTED_ERROR_NO         0
#define NV_DP_CORRECTED_ERROR_YES        1
#define NV_DP_BIT_ERROR_NO               0
#define NV_DP_BIT_ERROR_YES              1
#define NV_DP_PARITY_BLOCK_ERROR_NO      0
#define NV_DP_PARITY_BLOCK_ERROR_YES     1
#define NV_DP_PARITY_BIT_ERROR_NO        0
#define NV_DP_PARITY_BIT_ERROR_YES       1


#endif // #ifndef _DISPLAYPORT_H_
