//*****************************************************************************
//
//  SPDX-FileCopyrightText: Copyright (c) 2022 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
//  SPDX-License-Identifier: MIT
//
//  Permission is hereby granted, free of charge, to any person obtaining a
//  copy of this software and associated documentation files (the "Software"),
//  to deal in the Software without restriction, including without limitation
//  the rights to use, copy, modify, merge, publish, distribute, sublicense,
//  and/or sell copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
//  THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//  DEALINGS IN THE SOFTWARE.
//
//  File:       displayid20.h
//
//  Purpose:    the template for DisplayID 2.0 parsing (future replacement for EDID)
//
//*****************************************************************************


#ifndef __DISPLAYID20_H_
#define __DISPLAYID20_H_

#include "nvtiming.h"

// The structures below must be tightly packed, in order to correctly
// overlay on the DisplayID 2.0  block bytes.  Both MSVC and
// gcc support the pack() pragma for this.

#if defined(__GNUC__) || defined(_MSC_VER)
#  define __SUPPORTS_PACK_PRAGMA 1
#else
#  error "unrecognized compiler: displayid structures must be tightly packed"
#endif

#ifdef __SUPPORTS_PACK_PRAGMA
#pragma pack(1)
#endif

#define DISPLAYID_2_0_SECTION_SIZE_TOTAL(_pSectionHeader_)  ((_pSectionHeader_).section_bytes +      \
                                                             sizeof(DISPLAYID_2_0_SECTION_HEADER) +  \
                                                             sizeof(NvU8))
#define DISPLAYID_2_0_DATA_BLOCK_SIZE_TOTAL(_pBlockHeader_) ((_pBlockHeader_)->data_bytes +          \
                                                             sizeof(DISPLAYID_2_0_DATA_BLOCK_HEADER))
#define DISPLAYID_2_0_SECTION_SIZE_MAX                      256
#define DISPLAYID_2_0_SECTION_DATA_SIZE_MAX                 (DISPLAYID_2_0_SECTION_SIZE_MAX -        \
                                                            sizeof(DISPLAYID_2_0_SECTION_HEADER)

typedef struct _tagDISPLAYID_2_0_SECTION_HEADER
{
    NvU8 revision:4;       // displayID revision
    NvU8 version:4;        // displayID version
    NvU8 section_bytes;    // length of this displayID section excluding mandatory bytes [0, 251]

    NvU8 product_type:4;   // Display Product Primary Use Case
    NvU8 reserved:4;       // RESERVED
    NvU8 extension_count;  // Total extension count.
} DISPLAYID_2_0_SECTION_HEADER; 

typedef struct _tagDISPLAYID_2_0_SECTION
{
    DISPLAYID_2_0_SECTION_HEADER header;

    NvU8 data[DISPLAYID_2_0_SECTION_SIZE_MAX]; // data blocks. Note, DisplayID has variable length
} DISPLAYID_2_0_SECTION;

#define DISPLAYID_2_0_VERSION                           2
#define DISPLAYID_2_0_REVISION                          0

#define DISPLAYID_2_0_PROD_EXTENSION                    0 // Extension (same primary use case as base section)
#define DISPLAYID_2_0_PROD_TEST                         1 // Test Structure/Test Equipment
#define DISPLAYID_2_0_PROD_GENERIC_DISPLAY              2 // None of the listed primary use cases; generic display 
#define DISPLAYID_2_0_PROD_TELEVISION                   3 // Television (TV) display
#define DISPLAYID_2_0_PROD_DESKTOP_PRODUCTIVITY_DISPLAY 4 // Desktop productivity display
#define DISPLAYID_2_0_PROD_DESKTOP_GAMING_DISPLAY       5 // Desktop gaming display
#define DISPLAYID_2_0_PROD_PRESENTATION_DISPLAY         6 // Presentation display
#define DISPLAYID_2_0_PROD_HMD_VR                       7 // Head mounted Virtual Reality display
#define DISPLAYID_2_0_PROD_HMD_AR                       8 // Head mounted Augmented Reality display

typedef struct _tagDISPLAYID_2_0_DATA_BLOCK_HEADER
{
    NvU8  type;        // Data block tag
    NvU8  revision:3;  // block revision
    NvU8  reserved:5;
    NvU8  data_bytes;  // number of payload bytes in Block [ 0, 248]
} DISPLAYID_2_0_DATA_BLOCK_HEADER;

#define DISPLAYID_2_0_BLOCK_TYPE_PRODUCT_IDENTITY     0x20
#define DISPLAYID_2_0_BLOCK_TYPE_DISPLAY_PARAM        0x21
#define DISPLAYID_2_0_BLOCK_TYPE_TIMING_7             0x22 
#define DISPLAYID_2_0_BLOCK_TYPE_TIMING_8             0x23
#define DISPLAYID_2_0_BLOCK_TYPE_TIMING_9             0x24
#define DISPLAYID_2_0_BLOCK_TYPE_RANGE_LIMITS         0x25
#define DISPLAYID_2_0_BLOCK_TYPE_INTERFACE_FEATURES   0x26
#define DISPLAYID_2_0_BLOCK_TYPE_STEREO               0x27
#define DISPLAYID_2_0_BLOCK_TYPE_TILED_DISPLAY        0x28
#define DISPLAYID_2_0_BLOCK_TYPE_CONTAINER_ID         0x29
#define DISPLAYID_2_0_BLOCK_TYPE_TIMING_10            0x2A
#define DISPLAYID_2_0_BLOCK_TYPE_ADAPTIVE_SYNC        0x2B
#define DISPLAYID_2_0_BLOCK_TYPE_ARVR_HMD             0x2C
#define DISPLAYID_2_0_BLOCK_TYPE_ARVR_LAYER           0x2D
// 0x7D - 0x2E RESERVED for Additional VESA-defined Data Blocks
#define DISPLAYID_2_0_BLOCK_TYPE_VENDOR_SPEC          0x7E
// 0x80 - 0x7F RESERVED
#define DISPLAYID_2_0_BLOCK_TYPE_CTA_DATA             0x81
// 0xFF - 0x82 RESERVED for additional data blocks related to external standards organization(s). 

#define DISPLAYID_2_0_PRODUCT_NAME_STRING_MAX_LEN     ((0xFB - 0xF) + 1)

typedef struct _tagDISPLAYID_2_0_PROD_IDENTIFICATION_BLOCK
{
    // Product Identification Data Block (0x20)
    // Number of payload bytes 12(0xC) - 248(0xF8)
    DISPLAYID_2_0_DATA_BLOCK_HEADER header;

    NvU8  vendor[3];
    NvU8  product_code[2];
    NvU8  serial_number[4];
    NvU8  model_tag;
    NvU8  model_year;
    NvU8  product_name_string_size;
    NvU8  product_name_string[DISPLAYID_2_0_PRODUCT_NAME_STRING_MAX_LEN];
} DISPLAYID_2_0_PROD_IDENTIFICATION_BLOCK;

typedef struct _tagDISPLAY_2_0_DISPLAY_PARAM_BLOCK_HEADER
{
    NvU8  type;        // Display Parameters Data Block (0x21)
    NvU8  revision:3;
    NvU8  reserved:4;
    NvU8  image_size_multiplier:1;
    NvU8  data_bytes;  // number of payload bytes 29(0x1D)
} DISPLAY_2_0_DISPLAY_PARAM_BLOCK_HEADER;

typedef struct _tagDISPLAYID_2_0_COLOR_CHROMATICITY
{
    NvU8 color_x_bits_low;
    struct {
        NvU8 color_x_bits_high:4;
        NvU8 color_y_bits_low:4;
    } color_bits_mid;
    NvU8 color_y_bits_high;
} DISPLAYID_2_0_COLOR_CHROMATICITY;

typedef enum _tagDISPLAYID_2_0_NATIVE_COLOR_DEPTH
{
    NATIVE_COLOR_NOT_DEFINED = 0,
    NATIVE_COLOR_BPC_6       = 1,
    NATIVE_COLOR_BPC_8       = 2,
    NATIVE_COLOR_BPC_10      = 3,
    NATIVE_COLOR_BPC_12      = 4,
    NATIVE_COLOR_BPC_16      = 5,
} DISPLAYID_2_0_NATIVE_COLOR_DEPTH;

#define DISPLAYID_2_0_DISPLAY_PARAM_BLOCK_PAYLOAD_LENGTH   29
typedef struct _tagDISPLAYID_2_0_DISPLAY_PARAM_BLOCK
{
    DISPLAY_2_0_DISPLAY_PARAM_BLOCK_HEADER header;

    NvU8  horizontal_image_size[2];
    NvU8  vertical_image_size[2];
    NvU8  horizontal_pixel_count[2];
    NvU8  vertical_pixel_count[2];

    struct {
        NvU8  scan_orientation          :3;
        NvU8  luminance_information     :2;
        NvU8  reserved                  :1;
        NvU8  color_information         :1;
        NvU8  audio_speaker_information :1;
    } feature;

    DISPLAYID_2_0_COLOR_CHROMATICITY primary_color_1_chromaticity;
    DISPLAYID_2_0_COLOR_CHROMATICITY primary_color_2_chromaticity;
    DISPLAYID_2_0_COLOR_CHROMATICITY primary_color_3_chromaticity;
    DISPLAYID_2_0_COLOR_CHROMATICITY white_point_chromaticity;
    NvU8 max_luminance_full_coverage[2];
    NvU8 max_luminance_1_percent_rectangular_coverage[2];
    NvU8 min_luminance[2];

    struct {
        NvU8  color_depth             :3;
        NvU8  reserved0               :1;
        NvU8  device_technology       :3;
        NvU8  device_theme_preference :1;
    } color_depth_and_device_technology;

    NvU8  gamma_EOTF;
} DISPLAYID_2_0_DISPLAY_PARAM_BLOCK;

#define DISPLAYID_2_0_SCAN_ORIENTATION_LRTB         0  // Left to right, top to bottom
#define DISPLAYID_2_0_SCAN_ORIENTATION_RLTB         1  // Right to left, top to bottom
#define DISPLAYID_2_0_SCAN_ORIENTATION_TBRL         2  // Top to bottom, right to left
#define DISPLAYID_2_0_SCAN_ORIENTATION_BTRL         3  // Bottom to top, right to left
#define DISPLAYID_2_0_SCAN_ORIENTATION_RLBT         4  // Right to left, bottom to top
#define DISPLAYID_2_0_SCAN_ORIENTATION_LRBT         5  // Left to right, bottom to top
#define DISPLAYID_2_0_SCAN_ORIENTATION_BTLR         6  // Bottom to top, left to right
#define DISPLAYID_2_0_SCAN_ORIENTATION_TBLR         7  // Top to bottom, left to right

#define DISPLAYID_2_0_COLOR_INFORMATION_1931_CIE    0
#define DISPLAYID_2_0_color_INFORMATION_1976_CIE    1

#define DISPLAYID_2_0_AUDIO_SPEAKER_INTEGRATED      0
#define DISPLAYID_2_0_AUDIO_SPEAKER_NOT_INTEGRATED  1

#define DISPLAYID_2_0_DEVICE_TECHNOLOGY_UNSPECIFIED 0
#define DISPLAYID_2_0_DEVICE_TECHNOLOGY_LCD         1
#define DISPLAYID_2_0_DEVICE_TECHNOLOGY_OLED        2

#define DISPLAYID_2_0_TYPE7_DSC_PASSTHRU_REVISION   1
#define DISPLAYID_2_0_TYPE7_YCC420_SUPPORT_REVISION 2

// DisplayID_v2.0 E5 - DSC Pass-Through timing
// DisplayID_v2.0 E7 - YCC420 and > 20 bytes per descriptor supported
typedef struct _tagDISPLAYID_2_0_TIMING_7_BLOCK_HEADER
{
    NvU8  type;                // Type VII Timing (0x22)
    NvU8  revision          :3;
    NvU8  dsc_passthrough   :1;
    NvU8  payload_bytes_len :3;
    NvU8  reserved          :1;
    NvU8  data_bytes;          // Values range from 1(0x01) to 248(0xF8)
} DISPLAYID_2_0_TIMING_7_BLOCK_HEADER;

typedef struct _tag_DISPLAYID_2_0_TIMING_7_DESCRIPTOR
{
    // Range is defined as 0.001 through 16,777.216 MP/s
    NvU8 pixel_clock[3];

    struct
    {
        NvU8 aspect_ratio                   : 4;
        NvU8 interface_frame_scanning_type  : 1;
        NvU8 stereo_support                 : 2;
        NvU8 is_preferred_or_ycc420         : 1;
    } options;

    struct
    {
        NvU8 active_image_pixels[2];
        NvU8 blank_pixels[2];
        NvU8 front_porch_pixels_low;
        NvU8 front_porch_pixels_high        : 7;
        NvU8 sync_polarity                  : 1;
        NvU8 sync_width_pixels[2];
    } horizontal;

    struct
    {
        NvU8 active_image_lines[2];
        NvU8 blank_lines[2];
        NvU8 front_porch_lines_low;
        NvU8 front_porch_lines_high         : 7;
        NvU8 sync_polarity                  : 1;
        NvU8 sync_width_lines[2];
    } vertical;
} DISPLAYID_2_0_TIMING_7_DESCRIPTOR;

#define DISPLAYID_2_0_TIMING_7_MAX_DESCRIPTORS 12

typedef struct _tagDISPLAYID_2_0_TIMING_7_BLOCK
{
    DISPLAYID_2_0_TIMING_7_BLOCK_HEADER   header;
    DISPLAYID_2_0_TIMING_7_DESCRIPTOR     descriptors[DISPLAYID_2_0_TIMING_7_MAX_DESCRIPTORS];
} DISPLAYID_2_0_TIMING_7_BLOCK;

#define DISPLAYID_2_0_TIMING_DSC_PASSTHRU_TIMING    1

// the following fields apply to Timing Descriptors 7 (Not all of them are
// used per descriptor, but the format is the same
#define DISPLAYID_2_0_TIMING_ASPECT_RATIO_1_1       0
#define DISPLAYID_2_0_TIMING_ASPECT_RATIO_5_4       1
#define DISPLAYID_2_0_TIMING_ASPECT_RATIO_4_3       2
#define DISPLAYID_2_0_TIMING_ASPECT_RATIO_15_9      3
#define DISPLAYID_2_0_TIMING_ASPECT_RATIO_16_9      4
#define DISPLAYID_2_0_TIMING_ASPECT_RATIO_16_10     5
#define DISPLAYID_2_0_TIMING_ASPECT_RATIO_64_27     6
#define DISPLAYID_2_0_TIMING_ASPECT_RATIO_256_135   7
#define DISPLAYID_2_0_TIMING_ASPECT_RATIO_CALCULATE 8  // calculate using Horizontal and Vertical Active Image Pixels

#define DISPLAYID_2_0_TIMING_PROGRESSIVE_SCAN       0
#define DISPLAYID_2_0_TIMING_INTERLACED_SCAN        1

#define DISPLAYID_2_0_TIMING_3D_STEREO_MONO         0
#define DISPLAYID_2_0_TIMING_3D_STEREO_STEREO       1
#define DISPLAYID_2_0_TIMING_3D_STEREO_EITHER       2

#define DISPLAYID_2_0_TIMING_SYNC_POLARITY_NEGATIVE 0
#define DISPLAYID_2_0_TIMING_SYNC_POLARITY_POSITIVE 1

typedef struct _tagDISPLAYID_2_0_TIMING_8_BLOCK_HEADER
{
    NvU8  type;                // Type VIII Timing (0x23)
    NvU8  revision          :3;
    NvU8  timing_code_size  :1;
    NvU8  reserved          :1;
    NvU8  is_support_yuv420 :1;
    NvU8  timing_code_type  :2;
    NvU8  data_bytes;          // Values range from 1(0x01) to 248(0xF8)
} DISPLAYID_2_0_TIMING_8_BLOCK_HEADER;

typedef struct _tagDISPLAYID_2_0_TIMING_8_ONE_BYTE_CODE
{
    NvU8 timing_code;
} DISPLAYID_2_0_TIMING_8_ONE_BYTE_CODE;

typedef struct _tagDISPLAYID_2_0_TIMING_8_TWO_BYTE_CODE
{
    NvU8 timing_code[2];
} DISPLAYID_2_0_TIMING_8_TWO_BYTE_CODE;

#define DISPLAYID_2_0_TIMING_8_MAX_CODES    248

typedef struct _tagDISPLAYID_2_0_TIMING_8_BLOCK
{
    DISPLAYID_2_0_TIMING_8_BLOCK_HEADER header;

    union
    {
        DISPLAYID_2_0_TIMING_8_ONE_BYTE_CODE  timing_code_1[DISPLAYID_2_0_TIMING_8_MAX_CODES];
        DISPLAYID_2_0_TIMING_8_TWO_BYTE_CODE  timing_code_2[DISPLAYID_2_0_TIMING_8_MAX_CODES / 2];
    };
} DISPLAYID_2_0_TIMING_8_BLOCK;

#define DISPLAYID_2_0_TIMING_CODE_DMT       0
#define DISPLAYID_2_0_TIMING_CODE_CTA_VIC   1
#define DISPLAYID_2_0_TIMING_CODE_HDMI_VIC  2
#define DISPLAYID_2_0_TIMING_CODE_RSERVED   3
#define DISPLAYID_2_0_TIMING_CODE_SIZE_1_BYTE 0
#define DISPLAYID_2_0_TIMING_CODE_SIZE_2_BYTE 1

typedef struct _TAG_DISPLAYID_2_0_TIMING_9_DESCRIPTOR
{
    struct {
        NvU8 timing_formula          :3;
        NvU8 reserved0               :1;
        NvU8 rr_1000div1001_support  :1;
        NvU8 stereo_support          :2;
        NvU8 reserved1               :1;
    } options;

    NvU8  horizontal_active_pixels[2];
    NvU8  vertical_active_lines[2];
    NvU8  refresh_rate;              // 1 Hz to 256 Hz
} DISPLAYID_2_0_TIMING_9_DESCRIPTOR;

#define DISPLAYID_2_0_TIMING_FORMULA_CVT_1_2_STANDARD            0
#define DISPLAYID_2_0_TIMING_FORMULA_CVT_1_2_REDUCED_BLANKING_1  1
#define DISPLAYID_2_0_TIMING_FORMULA_CVT_2_0_REDUCED_BLANKING_2  2
#define DISPLAYID_2_0_TIMING_FORMULA_CVT_2_0_REDUCED_BLANKING_3  3

#define DISPLAYID_2_0_TIMING_9_MAX_DESCRIPTORS 18

typedef struct _tagDISPLAYID_2_0_TIMING_9_BLOCK
{
    // Type IX Timing (0x24)
    DISPLAYID_2_0_DATA_BLOCK_HEADER   header;
    DISPLAYID_2_0_TIMING_9_DESCRIPTOR descriptors[DISPLAYID_2_0_TIMING_9_MAX_DESCRIPTORS];
} DISPLAYID_2_0_TIMING_9_BLOCK;

#define DISPLAYID_2_0_TIMING_10_PAYLOAD_BYTES_6    0
#define DISPLAYID_2_0_TIMING_10_PAYLOAD_BYTES_7    1

typedef struct _tagDISPLAYID_2_0_TIMING_10_BLOCK_HEADER
{
    NvU8  type;                    // Type X Timing (0x2A)
    NvU8  revision          :3;
    NvU8  reserved0         :1;
    NvU8  payload_bytes_len :3;
    NvU8  reserved1         :1;
    NvU8  payload_bytes;
} DISPLAYID_2_0_TIMING_10_BLOCK_HEADER;

typedef struct _DISPLAYID_2_0_TIMING_10_6BYTES_DESCRIPTOR
{
    struct {
        NvU8 timing_formula :3;
        NvU8 early_vsync    :1;        
        NvU8 rr1000div1001_or_hblank :1;              
        NvU8 stereo_support :2;
        NvU8 ycc420_support :1;
    } options;

    NvU8  horizontal_active_pixels[2];
    NvU8  vertical_active_lines[2];
    NvU8  refresh_rate;                 // 1 Hz to 256 Hz
} DISPLAYID_2_0_TIMING_10_6BYTES_DESCRIPTOR;

typedef struct _DISPLAYID_2_0_TIMING_10_7BYTES_DESCRIPTOR
{
    DISPLAYID_2_0_TIMING_10_6BYTES_DESCRIPTOR descriptor_6_bytes;
    NvU8                                      refresh_rate_high        :2;
    NvU8                                      delta_hblank             :3;
    NvU8                                      additional_vblank_timing :3;
} DISPLAYID_2_0_TIMING_10_7BYTES_DESCRIPTOR;

#define DISPLAYID_2_0_TIMING_10_MAX_6BYTES_DESCRIPTORS 18
#define DISPLAYID_2_0_TIMING_10_MAX_7BYTES_DESCRIPTORS 16

typedef struct _DISPLAYID_2_0_TIMING_10_BLOCK
{
    DISPLAYID_2_0_TIMING_10_BLOCK_HEADER    header;
    NvU8                                    descriptors[120];
} DISPLAYID_2_0_TIMING_10_BLOCK;

#define DISPLAYID_2_0_RANGE_LIMITS_BLOCK_PAYLOAD_LENGTH   9
typedef struct _tagDISPLAYID_2_0_RANGE_LIMITS_BLOCK
{
    DISPLAYID_2_0_DATA_BLOCK_HEADER   header;

    NvU8  pixel_clock_min[3];
    NvU8  pixel_clock_max[3];
    NvU8  vertical_frequency_min;
    NvU8  vertical_frequency_max_7_0;

    struct {
       NvU8  vertical_frequency_max_9_8           :2;
       NvU8  reserved                             :5;
       NvU8  seamless_dynamic_video_timing_change :1;
    } dynamic_video_timing_range_support;
} DISPLAYID_2_0_RANGE_LIMITS_BLOCK;

#define DISPLAYID_2_0_SEAMLESS_DYNAMIC_VIDEO_TIMING_CHANGE_NOT_SUPPORTED 0
#define DISPLAYID_2_0_SEAMLESS_DYNAMIC_VIDEO_TIMING_CHANGE_SUPPORTED     1

#define DISPLAYID_2_0_INTERFACE_FEATURES_BLOCK_PAYLOAD_LENGTH_MIN 9
#define DISPLAYID_2_0_MAX_COLOR_SPACE_AND_EOTF     7
typedef struct _tagDISPLAYID_2_0_INTERFACE_FEATURES_BLOCK
{
    // Display Interface Features Data Block (0x26)
    DISPLAYID_2_0_DATA_BLOCK_HEADER   header;

    struct {
        NvU8  bit_per_primary_6:1;
        NvU8  bit_per_primary_8:1;
        NvU8  bit_per_primary_10:1;
        NvU8  bit_per_primary_12:1;
        NvU8  bit_per_primary_14:1;
        NvU8  bit_per_primary_16:1;
        NvU8  reserved:2;
    } interface_color_depth_rgb;

    struct {
        NvU8  bit_per_primary_6:1;
        NvU8  bit_per_primary_8:1;
        NvU8  bit_per_primary_10:1;
        NvU8  bit_per_primary_12:1;
        NvU8  bit_per_primary_14:1;
        NvU8  bit_per_primary_16:1;
        NvU8  reserved:2;
    } interface_color_depth_ycbcr444;

    struct {
        NvU8  bit_per_primary_8:1;
        NvU8  bit_per_primary_10:1;
        NvU8  bit_per_primary_12:1;
        NvU8  bit_per_primary_14:1;
        NvU8  bit_per_primary_16:1;
        NvU8  reserved:3;
    } interface_color_depth_ycbcr422;

    struct {
        NvU8  bit_per_primary_8:1;
        NvU8  bit_per_primary_10:1;
        NvU8  bit_per_primary_12:1;
        NvU8  bit_per_primary_14:1;
        NvU8  bit_per_primary_16:1;
        NvU8  reserved:3;
    } interface_color_depth_ycbcr420;

    NvU8  min_pixel_rate_ycbcr420; // x 74.25MP/s

    struct {
        NvU8  reserved:5;
        NvU8  sample_rate_48_khz:1;
        NvU8  sample_rate_44_1_khz:1;
        NvU8  sample_rate_32_khz:1;
    } audio_capability;

    struct {
        NvU8  color_space_srgb_eotf_srgb:1;
        NvU8  color_space_bt601_eotf_bt601:1;
        NvU8  color_space_bt709_eotf_bt1886:1;
        NvU8  color_space_adobe_rgb_eotf_adobe_rgb:1;
        NvU8  color_space_dci_p3_eotf_dci_p3:1;
        NvU8  color_space_bt2020_eotf_bt2020:1;
        NvU8  color_space_bt2020_eotf_smpte_st2084:1;
        NvU8  reserved:1;
    } color_space_and_eotf_1;

    struct {
        NvU8  reserved;
    } color_space_and_eotf_2;

    struct {
        NvU8  count:3;
        NvU8  reserved:5;
    } additional_color_space_and_eotf_count;

    struct {
        NvU8  eotf:4;
        NvU8  color_space:4;
    } additional_color_space_and_eotf[DISPLAYID_2_0_MAX_COLOR_SPACE_AND_EOTF];
} DISPLAYID_2_0_INTERFACE_FEATURES_BLOCK;

#define DISPLAYID_2_0_INTERFACE_FEATURES_SUPPORTED_COLORSPACE_NOT_DEFINED  0
#define DISPLAYID_2_0_INTERFACE_FEATURES_SUPPORTED_COLORSPACE_SRGB         1
#define DISPLAYID_2_0_INTERFACE_FEATURES_SUPPORTED_COLORSPACE_BT601        2
#define DISPLAYID_2_0_INTERFACE_FEATURES_SUPPORTED_COLORSPACE_BT709        3
#define DISPLAYID_2_0_INTERFACE_FEATURES_SUPPORTED_COLORSPACE_ADOBE_RGB    4
#define DISPLAYID_2_0_INTERFACE_FEATURES_SUPPORTED_COLORSPACE_DCI_P3       5
#define DISPLAYID_2_0_INTERFACE_FEATURES_SUPPORTED_COLORSPACE_BT2020       6
#define DISPLAYID_2_0_INTERFACE_FEATURES_SUPPORTED_COLORSPACE_CUSTOM       7

#define DISPLAYID_2_0_INTERFACE_FEATURES_SUPPORTED_EOTF_NOT_DEFINED  0
#define DISPLAYID_2_0_INTERFACE_FEATURES_SUPPORTED_EOTF_SRGB         1
#define DISPLAYID_2_0_INTERFACE_FEATURES_SUPPORTED_EOTF_BT601        2
#define DISPLAYID_2_0_INTERFACE_FEATURES_SUPPORTED_EOTF_BT709        3
#define DISPLAYID_2_0_INTERFACE_FEATURES_SUPPORTED_EOTF_ADOBE_RGB    4
#define DISPLAYID_2_0_INTERFACE_FEATURES_SUPPORTED_EOTF_DCI_P3       5
#define DISPLAYID_2_0_INTERFACE_FEATURES_SUPPORTED_EOTF_BT2020       6
#define DISPLAYID_2_0_INTERFACE_FEATURES_SUPPORTED_EOTF_GAMMA        7
#define DISPLAYID_2_0_INTERFACE_FEATURES_SUPPORTED_EOTF_SMPTE_ST2084 8
#define DISPLAYID_2_0_INTERFACE_FEATURES_SUPPORTED_EOTF_HYBRID_LOG   9
#define DISPLAYID_2_0_INTERFACE_FEATURES_SUPPORTED_EOTF_CUSTOM       10

typedef struct _tagDISPLAYID_2_0_STEREO_INTERFACE_BLOCK_HEADER
{
    NvU8  type;
    NvU8  revision:3;
    NvU8  reserved:3;
    NvU8  stereo_timing_support:2; 
} DISPLAYID_2_0_STEREO_INTERFACE_BLOCK_HEADER;

typedef struct _tagDISPLAYID_2_0_STEREO_TIMING_DESCRIPTOR
{
    NvU8  supported_timing_code_count:5;
    NvU8  reserved:1;
    NvU8  timing_code_type:2;
    NvU8  timing_code[0x1F];
} DISPLAYID_2_0_STEREO_TIMING_DESCRIPTOR;

typedef struct _tagDISPLAYID_2_0_STEREO_FIELD_SEQUENTIAL_INTERFACE_DESCRIPTOR
{
    NvU8  polarity_descriptor:1;
    NvU8  reserved:7;
    DISPLAYID_2_0_STEREO_TIMING_DESCRIPTOR timing_descriptor;
} DISPLAYID_2_0_STEREO_FIELD_SEQUENTIAL_INTERFACE_DESCRIPTOR;

typedef struct _tagDISPLAYID_2_0_STEREO_SIDE_BY_SIDE_INTERFACE_DESCRIPTOR
{
    NvU8  view_identity_descriptor:1;
    NvU8  reserved:7;
    DISPLAYID_2_0_STEREO_TIMING_DESCRIPTOR timing_descriptor;
} DISPLAYID_2_0_STEREO_SIDE_BY_SIDE_INTERFACE_DESCRIPTOR;

typedef struct _tagDISPLAYID_2_0_STEREO_PIXEL_INTERLEAVED_DESCRIPTOR
{
    NvU8  interleaved_pattern_descriptor[8];
    DISPLAYID_2_0_STEREO_TIMING_DESCRIPTOR timing_descriptor;
} DISPLAYID_2_0_STEREO_PIXEL_INTERLEAVED_DESCRIPTOR;

typedef struct _tagDISPLAYID_2_0_STEREO_DUAL_INTERFACE_LEFT_AND_RIGHT_SEPARATE_DESCRIPTOR
{
    NvU8  left_and_right_polarity_descriptor:1;
    NvU8  mirroring_descriptor:2;
    NvU8  reserved:5;
    DISPLAYID_2_0_STEREO_TIMING_DESCRIPTOR timing_descriptor;
} DISPLAYID_2_0_STEREO_DUAL_INTERFACE_LEFT_AND_RIGHT_SEPARATE_DESCRIPTOR;

typedef struct _tagDISPLAYID_2_0_STEREO_MULTI_VIEW_DESCRIPTOR
{
    NvU8  views_descriptors_count;
    NvU8  view_interleaving_method_code_descriptor;
    DISPLAYID_2_0_STEREO_TIMING_DESCRIPTOR timing_descriptor;
} DISPLAYID_2_0_STEREO_MULTI_VIEW_DESCRIPTOR;

typedef struct _tagDISPLAYID_2_0_STEREO_STACKED_FRAME_DESCRIPTOR
{
    NvU8  view_identity_descriptor:1;
    NvU8  reserved:7;
    DISPLAYID_2_0_STEREO_TIMING_DESCRIPTOR timing_descriptor;
} DISPLAYID_2_0_STEREO_STACKED_FRAME_DESCRIPTOR;

typedef struct _tagDISPLAYID_2_0_STEREO_PROPRIETARY_DESCRIPTOR
{
    DISPLAYID_2_0_STEREO_TIMING_DESCRIPTOR timing_descriptor;
} DISPLAYID_2_0_STEREO_PROPRIETARY_DESCRIPTOR;

typedef struct _tagDISPLAYID_2_0_STEREO_INTERFACE_METHOD_BLOCK
{
    DISPLAYID_2_0_STEREO_INTERFACE_BLOCK_HEADER header;

    NvU8 stereo_bytes;
    NvU8 stereo_code;
    union {
        DISPLAYID_2_0_STEREO_FIELD_SEQUENTIAL_INTERFACE_DESCRIPTOR             field_sequential;
        DISPLAYID_2_0_STEREO_SIDE_BY_SIDE_INTERFACE_DESCRIPTOR                 side_by_side;
        DISPLAYID_2_0_STEREO_PIXEL_INTERLEAVED_DESCRIPTOR                      pixel_interleaved;
        DISPLAYID_2_0_STEREO_DUAL_INTERFACE_LEFT_AND_RIGHT_SEPARATE_DESCRIPTOR dual_interface;
        DISPLAYID_2_0_STEREO_MULTI_VIEW_DESCRIPTOR                             multi_view;
        DISPLAYID_2_0_STEREO_STACKED_FRAME_DESCRIPTOR                          stacked_frame;
        DISPLAYID_2_0_STEREO_PROPRIETARY_DESCRIPTOR                            proprietary;
    };
} DISPLAYID_2_0_STEREO_INTERFACE_METHOD_BLOCK;

#define DISPLAYID_2_0_STEREO_CODE_FIELD_SEQUENTIAL  0x0
#define DISPLAYID_2_0_STEREO_CODE_SIDE_BY_SIDE      0x1
#define DISPLAYID_2_0_STEREO_CODE_PIXEL_INTERLEAVED 0x2
#define DISPLAYID_2_0_STEREO_CODE_DUAL_INTERFACE    0x3
#define DISPLAYID_2_0_STEREO_CODE_MULTIVIEW         0x4
#define DISPLAYID_2_0_STEREO_CODE_STACKED_FRAME     0x5
#define DISPLAYID_2_0_STEREO_CODE_PROPRIETARY       0xFF

#define DISPLAYID_STEREO_MIRRORING 2:1
#define DISPLAYID_STEREO_POLARITY  0:0

#define DISPLAYID_2_0_TILED_DISPLAY_BLOCK_PAYLOAD_LENGTH   22
typedef struct _tagDISPLAYID_2_0_TILED_DISPLAY_BLOCK
{
    DISPLAYID_2_0_DATA_BLOCK_HEADER   header;
    struct
    {
        NvU8 single_tile_behavior:3;       // 0x03
        NvU8 multi_tile_behavior:2;        // 0x03
        NvU8 rsvd             :1;          // 0x03
        NvU8 has_bezel_info   :1;          // 0x03
        NvU8 single_enclosure :1;          // 0x03
    } capability;
    struct
    {
        NvU8 row              :4;          // 0x04
        NvU8 col              :4;          // 0x04
    } topo_low;
    struct
    {
        NvU8 y                :4;          // 0x05
        NvU8 x                :4;          // 0x05
    } loc_low;
    struct
    {
        NvU8 y                :2;          // 0x06
        NvU8 x                :2;          // 0x06
        NvU8 row              :2;          // 0x06
        NvU8 col              :2;          // 0x06
    } topo_loc_high;
    struct
    {
        NvU8 width_low;                   // 0x07
        NvU8 width_high;                  // 0x08
        NvU8 height_low;                  // 0x09
        NvU8 height_high;                 // 0X0A
    } native_resolution;
    struct
    {
        NvU8 pixel_density;                // 0x0B
        NvU8 top;                          // 0x0C
        NvU8 bottom;                       // 0x0D
        NvU8 right;                        // 0x0E
        NvU8 left;                         // 0x0F
    } bezel_info;
    struct
    {
        NvU8 vendor_id[3];                 // 0x10 ~ 0x12
        NvU8 product_id[2];                // 0x13 ~ 0x14
        NvU8 serial_number[4];             // 0x15 ~ 0x18
    } topo_id;
} DISPLAYID_2_0_TILED_DISPLAY_BLOCK;

#define DISPLAYID_2_0_CONTAINERID_BLOCK_PAYLOAD_LENGTH   16
typedef struct _tagDISPLAYID_2_0_CONTAINERID_BLOCK
{
    DISPLAYID_2_0_DATA_BLOCK_HEADER header;
    NvU8  container_id[DISPLAYID_2_0_CONTAINERID_BLOCK_PAYLOAD_LENGTH];
} DISPLAYID_2_0_CONTAINERID_BLOCK;

#define DISPLAYID_2_0_ADAPTIVE_SYNC_DETAILED_TIMING_COUNT 4
typedef struct _tagDISPLAYID_2_0_ADAPTIVE_SYNC_BLOCK_HEADER
{
    NvU8  type;                                // Adaptive-Sync (0x2B)
    NvU8  revision                        :3;
    NvU8  reserved0                       :1;
    NvU8  payload_bytes_adaptive_sync_len :3;
    NvU8  reserved1                       :1;
    NvU8  payload_bytes;
} DISPLAYID_2_0_ADAPTIVE_SYNC_BLOCK_HEADER;

typedef struct _tagDISPLAYID_2_0_ADAPTIVE_SYNC_DESCRIPTOR
{
    struct
    {
        NvU8 range                          : 1;
        NvU8 successive_frame_inc_tolerance : 1;
        NvU8 modes                          : 2;
        NvU8 seamless_transition_not_support: 1;
        NvU8 successive_frame_dec_tolerance : 1;
        NvU8 reserved                       : 2;
    } operation_range_info;
    
    // 6.2 format (six integer bits and two fractional bits)
    // six integer bits    == 0 - 63ms
    // two fractional bits == 0.00(00), 0.25(01b),0.50(10), 0.75(11b)
    NvU8 max_single_frame_inc;
    NvU8 min_refresh_rate;
    struct
    {
        NvU8 max_rr_7_0;
        NvU8 max_rr_9_8 : 2;
        NvU8 reserved   : 6;
    } max_refresh_rate;
    
    // same as max_single_frame_inc expression
    NvU8 max_single_frame_dec; 
} DISPLAYID_2_0_ADAPTIVE_SYNC_DESCRIPTOR;

typedef struct _tagDISPLAYID_2_0_ADAPTIVE_SYNC_BLOCK
{
    DISPLAYID_2_0_ADAPTIVE_SYNC_BLOCK_HEADER header;
    DISPLAYID_2_0_ADAPTIVE_SYNC_DESCRIPTOR   descriptors[DISPLAYID_2_0_ADAPTIVE_SYNC_DETAILED_TIMING_COUNT];
} DISPLAYID_2_0_ADAPTIVE_SYNC_BLOCK;

typedef struct _tagDISPLAYID_2_0_VENDOR_SPECIFIC_BLOCK
{
    DISPLAYID_2_0_DATA_BLOCK_HEADER header;
    NvU8  vendor_id[3];
    NvU8  vendor_specific_data[245];
} DISPLAYID_2_0_VENDOR_SPECIFIC_BLOCK;

typedef struct _tagDISPLAYID_2_0_CTA_BLOCK
{
    DISPLAYID_2_0_DATA_BLOCK_HEADER header;
    NvU8  cta_data[248];
} DISPLAYID_2_0_CTA_BLOCK;

#ifdef __SUPPORTS_PACK_PRAGMA
#pragma pack()
#endif

// Entry point functions both used in DID20 and DID20ext
NVT_STATUS parseDisplayId20DataBlock(const DISPLAYID_2_0_DATA_BLOCK_HEADER *pDataBlock, NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo);
NvU8       computeDisplayId20SectionCheckSum(const NvU8 *pSectionBytes, NvU32 length);

#endif // __DISPLAYID20_H_
