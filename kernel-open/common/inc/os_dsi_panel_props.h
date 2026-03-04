/*
 * SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
#ifndef _OS_DSI_PANEL_PARAMS_H_
#define _OS_DSI_PANEL_PARAMS_H_

#define DSI_GENERIC_LONG_WRITE                  0x29
#define DSI_DCS_LONG_WRITE                      0x39
#define DSI_GENERIC_SHORT_WRITE_1_PARAMS        0x13
#define DSI_GENERIC_SHORT_WRITE_2_PARAMS        0x23
#define DSI_DCS_WRITE_0_PARAM                   0x05
#define DSI_DCS_WRITE_1_PARAM                   0x15
#define DSI_DCS_READ_PARAM                      0x06
#define DSI_DCS_COMPRESSION_MODE		        0x07
#define DSI_DCS_PPS_LONG_WRITE			        0x0A

#define DSI_DCS_SET_ADDR_MODE                   0x36
#define DSI_DCS_EXIT_SLEEP_MODE                 0x11
#define DSI_DCS_ENTER_SLEEP_MODE                0x10
#define DSI_DCS_SET_DISPLAY_ON                  0x29
#define DSI_DCS_SET_DISPLAY_OFF                 0x28
#define DSI_DCS_SET_TEARING_EFFECT_OFF          0x34
#define DSI_DCS_SET_TEARING_EFFECT_ON           0x35
#define DSI_DCS_NO_OP                           0x0
#define DSI_NULL_PKT_NO_DATA                    0x9
#define DSI_BLANKING_PKT_NO_DATA                0x19
#define DSI_DCS_SET_COMPRESSION_METHOD          0xC0

/* DCS commands for command mode */
#define DSI_ENTER_PARTIAL_MODE                  0x12
#define DSI_SET_PIXEL_FORMAT                    0x3A
#define DSI_AREA_COLOR_MODE                     0x4C
#define DSI_SET_PARTIAL_AREA                    0x30
#define DSI_SET_PAGE_ADDRESS                    0x2B
#define DSI_SET_ADDRESS_MODE                    0x36
#define DSI_SET_COLUMN_ADDRESS                  0x2A
#define DSI_WRITE_MEMORY_START                  0x2C
#define DSI_WRITE_MEMORY_CONTINUE               0x3C

#define PKT_ID0(id)     ((((id) & 0x3f) << 3) | \
                        (((DSI_ENABLE) & 0x1) << 9))
#define PKT_LEN0(len)   (((len) & 0x7) << 0)
#define PKT_ID1(id)     ((((id) & 0x3f) << 13) | \
                        (((DSI_ENABLE) & 0x1) << 19))
#define PKT_LEN1(len)   (((len) & 0x7) << 10)
#define PKT_ID2(id)     ((((id) & 0x3f) << 23) | \
                        (((DSI_ENABLE) & 0x1) << 29))
#define PKT_LEN2(len)   (((len) & 0x7) << 20)
#define PKT_ID3(id)     ((((id) & 0x3f) << 3) | \
                        (((DSI_ENABLE) & 0x1) << 9))
#define PKT_LEN3(len)   (((len) & 0x7) << 0)
#define PKT_ID4(id)     ((((id) & 0x3f) << 13) | \
                        (((DSI_ENABLE) & 0x1) << 19))
#define PKT_LEN4(len)   (((len) & 0x7) << 10)
#define PKT_ID5(id)     ((((id) & 0x3f) << 23) | \
                        (((DSI_ENABLE) & 0x1) << 29))
#define PKT_LEN5(len)   (((len) & 0x7) << 20)
#define PKT_LP          (((DSI_ENABLE) & 0x1) << 30)
#define NUMOF_PKT_SEQ   12

/* DSI pixel data format, enum values should match with dt-bindings in tegra-panel.h */
typedef enum
{
    DSI_PIXEL_FORMAT_16BIT_P,
    DSI_PIXEL_FORMAT_18BIT_P,
    DSI_PIXEL_FORMAT_18BIT_NP,
    DSI_PIXEL_FORMAT_24BIT_P,
    DSI_PIXEL_FORMAT_8BIT_DSC,
    DSI_PIXEL_FORMAT_12BIT_DSC,
    DSI_PIXEL_FORMAT_16BIT_DSC,
    DSI_PIXEL_FORMAT_10BIT_DSC,
    DSI_PIXEL_FORMAT_30BIT_P,
    DSI_PIXEL_FORMAT_36BIT_P,
} DSIPIXELFORMAT;

/* DSI virtual channel number */
typedef enum
{
    DSI_VIRTUAL_CHANNEL_0,
    DSI_VIRTUAL_CHANNEL_1,
    DSI_VIRTUAL_CHANNEL_2,
    DSI_VIRTUAL_CHANNEL_3,
} DSIVIRTUALCHANNEL;

/* DSI transmit method for video data */
typedef enum
{
    DSI_VIDEO_TYPE_VIDEO_MODE,
    DSI_VIDEO_TYPE_COMMAND_MODE,
} DSIVIDEODATAMODE;

/* DSI HS clock mode */
typedef enum
{
    DSI_VIDEO_CLOCK_CONTINUOUS,
    DSI_VIDEO_CLOCK_TX_ONLY,
} DSICLOCKMODE;

/* DSI burst mode setting in video mode. Each mode is assigned with a
 * fixed value. The rationale behind this is to avoid change of these
 * values, since the calculation of dsi clock depends on them. */
typedef enum
{
    DSI_VIDEO_NON_BURST_MODE = 0,
    DSI_VIDEO_NON_BURST_MODE_WITH_SYNC_END = 1,
    DSI_VIDEO_BURST_MODE_LOWEST_SPEED = 2,
    DSI_VIDEO_BURST_MODE_LOW_SPEED = 3,
    DSI_VIDEO_BURST_MODE_MEDIUM_SPEED = 4,
    DSI_VIDEO_BURST_MODE_FAST_SPEED = 5,
    DSI_VIDEO_BURST_MODE_FASTEST_SPEED = 6,
} DSIVIDEOBURSTMODE;

/* DSI Ganged Mode */
typedef enum
{
    DSI_GANGED_SYMMETRIC_LEFT_RIGHT = 1,
    DSI_GANGED_SYMMETRIC_EVEN_ODD = 2,
    DSI_GANGED_SYMMETRIC_LEFT_RIGHT_OVERLAP = 3,
} DSIGANGEDTYPE;

typedef enum
{
    DSI_LINK0,
    DSI_LINK1,
} DSILINKNUM;

/* DSI Command Packet type */
typedef enum
{
    DSI_PACKET_CMD,
    DSI_DELAY_MS,
    DSI_GPIO_SET,
    DSI_SEND_FRAME,
    DSI_PACKET_VIDEO_VBLANK_CMD,
    DSI_DELAY_US,
} DSICMDPKTTYPE;

/* DSI Phy type */
typedef enum
{
    DSI_DPHY,
    DSI_CPHY,
} DSIPHYTYPE;

enum {
    DSI_GPIO_LCD_RESET,
    DSI_GPIO_PANEL_EN,
    DSI_GPIO_PANEL_EN_1,
    DSI_GPIO_BL_ENABLE,
    DSI_GPIO_BL_PWM,
    DSI_GPIO_AVDD_AVEE_EN,
    DSI_GPIO_VDD_1V8_LCD_EN,
    DSI_GPIO_TE,
    DSI_GPIO_BRIDGE_EN_0,
    DSI_GPIO_BRIDGE_EN_1,
    DSI_GPIO_BRIDGE_REFCLK_EN,
    DSI_N_GPIO_PANEL, /* add new gpio above this entry */
};

enum
{
    DSI_DISABLE,
    DSI_ENABLE,
};


typedef struct
{
    NvU8      cmd_type;
    NvU8      data_id;
    union
    {
        NvU16 data_len;
        NvU16 delay_ms;
        NvU16 delay_us;
        NvU32 gpio;
        NvU16 frame_cnt;
        struct
        {
            NvU8 data0;
            NvU8 data1;
        } sp;
    } sp_len_dly;
    NvU32      *pdata;
    NvU8      link_id;
    NvBool    club_cmd;
} DSI_CMD, *PDSICMD;

typedef struct
{
    NvU16             t_hsdexit_ns;
    NvU16             t_hstrail_ns;
    NvU16             t_datzero_ns;
    NvU16             t_hsprepare_ns;
    NvU16             t_hsprebegin_ns;
    NvU16             t_hspost_ns;

    NvU16             t_clktrail_ns;
    NvU16             t_clkpost_ns;
    NvU16             t_clkzero_ns;
    NvU16             t_tlpx_ns;

    NvU16             t_clkprepare_ns;
    NvU16             t_clkpre_ns;
    NvU16             t_wakeup_ns;

    NvU16             t_taget_ns;
    NvU16             t_tasure_ns;
    NvU16             t_tago_ns;
} DSI_PHY_TIMING_IN_NS;

typedef struct
{
    NvU32 hActive;
    NvU32 vActive;
    NvU32 hFrontPorch;
    NvU32 vFrontPorch;
    NvU32 hBackPorch;
    NvU32 vBackPorch;
    NvU32 hSyncWidth;
    NvU32 vSyncWidth;
    NvU32 hPulsePolarity;
    NvU32 vPulsePolarity;
    NvU32 pixelClkRate;
} DSITIMINGS, *PDSITIMINGS;

typedef struct
{
    NvU8              n_data_lanes;                   /* required */
    NvU8              pixel_format;                   /* required */
    NvU8              refresh_rate;                   /* required */
    NvU8              rated_refresh_rate;
    NvU8              panel_reset;                    /* required */
    NvU8              virtual_channel;                /* required */
    NvU8              dsi_instance;
    NvU16             dsi_panel_rst_gpio;
    NvU16             dsi_panel_bl_en_gpio;
    NvU16             dsi_panel_bl_pwm_gpio;
    NvU16             even_odd_split_width;
    NvU8              controller_vs;

    NvBool            panel_has_frame_buffer; /* required*/

    /* Deprecated. Use DSI_SEND_FRAME panel command instead. */
    NvBool            panel_send_dc_frames;

    DSI_CMD           *dsi_init_cmd;          /* required */
    NvU16             n_init_cmd;             /* required */
    NvU32             *dsi_init_cmd_array;
    NvU32             init_cmd_array_size;
    NvBool            sendInitCmdsEarly;

    DSI_CMD           *dsi_early_suspend_cmd;
    NvU16             n_early_suspend_cmd;
    NvU32             *dsi_early_suspend_cmd_array;
    NvU32             early_suspend_cmd_array_size;

    DSI_CMD           *dsi_late_resume_cmd;
    NvU16             n_late_resume_cmd;
    NvU32             *dsi_late_resume_cmd_array;
    NvU32             late_resume_cmd_array_size;

    DSI_CMD           *dsi_postvideo_cmd;
    NvU16             n_postvideo_cmd;
    NvU32             *dsi_postvideo_cmd_array;
    NvU32             postvideo_cmd_array_size;

    DSI_CMD           *dsi_suspend_cmd;               /* required */
    NvU16             n_suspend_cmd;                  /* required */
    NvU32             *dsi_suspend_cmd_array;
    NvU32             suspend_cmd_array_size;

    NvU8              video_data_type;                /* required */
    NvU8              video_clock_mode;
    NvU8              video_burst_mode;
    NvU8              ganged_type;
    NvU16             ganged_overlap;
    NvBool            ganged_swap_links;
    NvBool            ganged_write_to_all_links;
    NvU8              split_link_type;

    NvU8              suspend_aggr;

    NvU16             panel_buffer_size_byte;
    NvU16             panel_reset_timeout_msec;

    NvBool            hs_cmd_mode_supported;
    NvBool            hs_cmd_mode_on_blank_supported;
    NvBool            enable_hs_clock_on_lp_cmd_mode;
    NvBool            no_pkt_seq_eot; /* 1st generation panel may not
                                       * support eot. Don't set it for
                                       * most panels.*/
    const NvU32       *pktSeq;
    NvU32             *pktSeq_array;
    NvU32             pktSeq_array_size;
    NvBool            skip_dsi_pkt_header;
    NvBool            power_saving_suspend;
    NvBool            suspend_stop_stream_late;
    NvBool            dsi2lvds_bridge_enable;
    NvBool            dsi2edp_bridge_enable;

    NvU32             max_panel_freq_khz;
    NvU32             lp_cmd_mode_freq_khz;
    NvU32             lp_read_cmd_mode_freq_khz;
    NvU32             hs_clk_in_lp_cmd_mode_freq_khz;
    NvU32             burst_mode_freq_khz;
    NvU32             fpga_freq_khz;

    NvU32             te_gpio;
    NvBool            te_polarity_low;
    NvBool            dsiEnVRR;
    NvBool            dsiVrrPanelSupportsTe;
    NvBool            dsiForceSetTePin;

    int               panel_gpio[DSI_N_GPIO_PANEL];
    NvBool            panel_gpio_populated;

    NvU32             dpd_dsi_pads;

    DSI_PHY_TIMING_IN_NS phyTimingNs;

    NvU8              *bl_name;

    NvBool            lp00_pre_panel_wakeup;
    NvBool            ulpm_not_supported;
    NvBool            use_video_host_fifo_for_cmd;
    NvBool            dsi_csi_loopback;
    NvBool            set_max_timeout;
    NvBool            use_legacy_dphy_core;
    // Swap P/N pins polarity of all data lanes
    NvBool            swap_data_lane_polarity;
    // Swap P/N pins polarity of clock lane
    NvBool            swap_clock_lane_polarity;
    // Reverse clock polarity for partition A/B. 1st SOT bit goes on negedge of Clock lane
    NvBool            reverse_clock_polarity;
    // DSI Lane Crossbar. Allocating xbar array for max number of lanes
    NvBool            lane_xbar_exists;
    NvU32             lane_xbar_ctrl[8];
    NvU32             refresh_rate_adj;

    NvU8              dsiPhyType;
    NvBool            en_data_scrambling;

    NvU32             dsipll_vco_rate_hz;
    NvU32             dsipll_clkoutpn_rate_hz;
    NvU32             dsipll_clkouta_rate_hz;
    NvU32             vpll0_rate_hz;

    DSITIMINGS        dsiTimings;

    // DSC Parameters
    NvBool            dsiDscEnable;
    NvU32             dsiDscBpp;
    NvU32             dsiDscNumSlices;
    NvU32             dsiDscSliceWidth;
    NvU32             dsiDscSliceHeight;
    NvBool            dsiDscEnBlockPrediction;
    NvBool            dsiDscEnDualDsc;
    NvU32             dsiDscDecoderMajorVersion;
    NvU32             dsiDscDecoderMinorVersion;
    NvBool            dsiDscUseCustomPPS;
    NvU32             dsiDscCustomPPSData[32];

    // Driver allocates memory for PPS cmd to be sent to Panel
    NvBool            ppsCmdMemAllocated;
} DSI_PANEL_INFO;

#endif
