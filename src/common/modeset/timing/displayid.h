//*****************************************************************************
//
//  SPDX-FileCopyrightText: Copyright (c) 2021 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
//  File:       displayid.h
//
//  Purpose:    the template for DisplayID parsing (future replacement for EDID)
//
//*****************************************************************************


#ifndef __DISPLAYID_H_
#define __DISPLAYID_H_

#include "nvtiming.h"

// The structures below must be tightly packed, in order to correctly
// overlay on the EDID DisplayID extension block bytes.  Both MSVC and
// gcc support the pack() pragma for this.

#if defined(__GNUC__) || defined(_MSC_VER)
#  define __SUPPORTS_PACK_PRAGMA 1
#else
#  error "unrecognized compiler: displayid structures must be tightly packed"
#endif

#ifdef __SUPPORTS_PACK_PRAGMA
#pragma pack(1)
#endif

typedef struct _tagDISPLAYID_SECTION
{
    NvU8 version; // displayid version
    NvU8 section_bytes; // length of this displayID section excluding mandatory bytes [0, 251]

    NvU8 product_type;    // NVT_DISPLAYID_PROD_X
    NvU8 extension_count;

    NvU8 data[NVT_DISPLAYID_SECTION_MAX_SIZE]; // data blocks. Note, the length of this structure may
                                                    // exceed valid memory, as DisplayID has variable length

} DISPLAYID_SECTION;

#define NVT_DISPLAYID_VER_1_1 0x101

#define NVT_DISPLAYID_PROD_EXTENSION          0 // Extension (product type not declared)
#define NVT_DISPLAYID_PROD_TEST               1 // Test Structure/Test Equipment
#define NVT_DISPLAYID_PROD_DISPLAY_PANEL      2 // Display Panel, LCD, or PDP module, etc.
#define NVT_DISPLAYID_PROD_STANDALONE_MONITOR 3 // Standalone display device, desktop monitor, TV monitor
#define NVT_DISPLAYID_PROD_RECEIVER           4 // Television receiver or display product capable of RF signals
#define NVT_DISPLAYID_PROD_REPEATER           5 // Repeater/translator that is not intended as display device
#define NVT_DISPLAYID_PROD_DIRECT_DRIVE       6 // Direct Drive monitor
#define NVT_DISPLAYID_PROD_MAX_NUMBER         6 // max product number


typedef struct _tagDISPLAYID_DATA_BLOCK_HEADER
{
    NvU8 type; // identification
    NvU8 revision;
    NvU8 data_bytes; // number of payload bytes [0, 248]

} DISPLAYID_DATA_BLOCK_HEADER;

#define NVT_DISPLAYID_BLOCK_TYPE_PRODUCT_IDENTITY  0 // Product Identification block
#define NVT_DISPLAYID_BLOCK_TYPE_DISPLAY_PARAM     1 // Display Parameters block
#define NVT_DISPLAYID_BLOCK_TYPE_COLOR_CHAR        2 // Color Characteristics block
#define NVT_DISPLAYID_BLOCK_TYPE_TIMING_1          3 // Type 1 Detailed Timing block
#define NVT_DISPLAYID_BLOCK_TYPE_TIMING_2          4 // Type 2 Detailed Timing block
#define NVT_DISPLAYID_BLOCK_TYPE_TIMING_3          5 // Type 3 Short Timing block
#define NVT_DISPLAYID_BLOCK_TYPE_TIMING_4          6 // Type 4 DMT ID Timing block
#define NVT_DISPLAYID_BLOCK_TYPE_TIMING_VESA       7 // VESA Standard Timing block
#define NVT_DISPLAYID_BLOCK_TYPE_TIMING_CEA        8 // CEA Standard Timing block
#define NVT_DISPLAYID_BLOCK_TYPE_RANGE_LIMITS      9 // Video Timing Range Limits block
#define NVT_DISPLAYID_BLOCK_TYPE_SERIAL_NUMBER     10 // Product Serial Number block
#define NVT_DISPLAYID_BLOCK_TYPE_ASCII_STRING      11 // General Purpose ASCII String block
#define NVT_DISPLAYID_BLOCK_TYPE_DEVICE_DATA       12 // Display Device Data block
#define NVT_DISPLAYID_BLOCK_TYPE_INTERFACE_POWER   13 // Interface Power Sequencing block
#define NVT_DISPLAYID_BLOCK_TYPE_TRANSFER_CHAR     14 // Transfer Characteristics block
#define NVT_DISPLAYID_BLOCK_TYPE_DISPLAY_INTERFACE 15 // Display Interface Data Block
#define NVT_DISPLAYID_BLOCK_TYPE_STEREO            16 // Stereo Data Block
#define NVT_DISPLAYID_BLOCK_TYPE_TIMING_5          17 // Type V Timing Short Descriptor
#define NVT_DISPLAYID_BLOCK_TYPE_TILEDDISPLAY      18 // Tiled Display Data Block
#define NVT_DISPLAYID_BLOCK_TYPE_DISPLAY_INTERFACE_FEATURES 0X26 // DisplayID2.0 Display Interface Features Data Block                                                      //
#define NVT_DISPLAYID_BLOCK_TYPE_CTA_DATA        0x81 // DIsplay ID data block
#define NVT_DISPLAYID_BLOCK_TYPE_VENDOR_SPEC     0x7F // Vendor Specific Data Block

#define NVT_DISPLAYID_PRODUCT_IDENTITY_MIN_LEN 12
#define NVT_DISPLAYID_PRODUCT_IDENTITY_MAX_STRING_LEN 0xE9

typedef struct _tagDISPLAYID_PROD_IDENTIFICATION_BLOCK
{
    DISPLAYID_DATA_BLOCK_HEADER header;

    NvU8 vendor[3];
    NvU16 product_code;
    NvU32 serial_number;
    NvU8 model_tag;
    NvU8 model_year;
    NvU8 productid_string_size;
    
    NvU8 productid_string[NVT_DISPLAYID_PRODUCT_IDENTITY_MAX_STRING_LEN];
} DISPLAYID_PROD_IDENTIFICATION_BLOCK;

typedef struct _tagDISPLAYID_DISPLAY_PARAM_BLOCK
{
    DISPLAYID_DATA_BLOCK_HEADER header;
    NvU16 horizontal_image_size;
    NvU16 vertical_image_size;
    NvU16 horizontal_pixel_count;
    NvU16 vertical_pixel_count;

    NvU8 feature;

    NvU8 transfer_char_gamma;
    NvU8 aspect_ratio;
    NvU8 color_bit_depth;
} DISPLAYID_DISPLAY_PARAM_BLOCK;

#define NVT_DISPLAYID_DISPLAY_PARAM_BLOCK_LEN 0x0C

#define NVT_DISPLAYID_DISPLAY_PARAM_SUPPORT_AUDIO        7:7
#define NVT_DISPLAYID_DISPLAY_PARAM_SEPARATE_AUDIO       6:6
#define NVT_DISPLAYID_DISPLAY_PARAM_AUDIO_INPUT_OVERRIDE 5:5
#define NVT_DISPLAYID_DISPLAY_PARAM_POWER_MANAGEMENT     4:4
#define NVT_DISPLAYID_DISPLAY_PARAM_FIXED_TIMING         3:3
#define NVT_DISPLAYID_DISPLAY_PARAM_FIXED_PIXEL_FORMAT   2:2
#define NVT_DISPLAYID_DISPLAY_PARAM_DEINTERLACING        0:0

#define NVT_DISPLAYID_DISPLAY_PARAM_DEPTH_OVERALL 7:4
#define NVT_DISPLAYID_DISPLAY_PARAM_DEPTH_NATIVE  3:0

typedef struct _tagDISPLAYID_COLOR_POINT
{
    NvU8 color_x_bits_low;
    NvU8 color_bits_mid;
    NvU8 color_y_bits_high;
} DISPLAYID_COLOR_POINT;

#define NVT_DISPLAYID_COLOR_POINT_Y 7:4
#define NVT_DISPLAYID_COLOR_POINT_X 3:0

#define NVT_DISPLAYID_COLOR_MAX_POINTS 22

typedef struct _tagDISPLAYID_COLOR_CHAR_BLOCK
{
    DISPLAYID_DATA_BLOCK_HEADER header;

    // Color Characteristics Information
    NvU8 point_info;

    DISPLAYID_COLOR_POINT points[NVT_DISPLAYID_COLOR_MAX_POINTS];
} DISPLAYID_COLOR_CHAR_BLOCK;

#define NVT_DISPLAYID_COLOR_PRIMARIES    6:4 
#define NVT_DISPLAYID_COLOR_WHITE_POINTS 3:0
#define NVT_DISPLAYID_COLOR_TEMPORAL 7:7

// the following fields apply to Timing Descriptors 1-3 (Not all of them are
// used per descriptor, but the format is the same
#define NVT_DISPLAYID_TIMING_PREFERRED 7:7
#define NVT_DISPLAYID_TIMING_3D_STEREO 6:5
#define NVT_DISPLAYID_TIMING_3D_STEREO_MONO   0
#define NVT_DISPLAYID_TIMING_3D_STEREO_STEREO 1
#define NVT_DISPLAYID_TIMING_3D_STEREO_EITHER 2
#define NVT_DISPLAYID_TIMING_INTERLACE  4:4
#define NVT_DISPLAYID_TIMING_ASPECT_RATIO 2:0
#define NVT_DISPLAYID_TIMING_ASPECT_RATIO_1_1   0
#define NVT_DISPLAYID_TIMING_ASPECT_RATIO_5_4   1
#define NVT_DISPLAYID_TIMING_ASPECT_RATIO_4_3   2
#define NVT_DISPLAYID_TIMING_ASPECT_RATIO_15_9  3
#define NVT_DISPLAYID_TIMING_ASPECT_RATIO_16_9  4
#define NVT_DISPLAYID_TIMING_ASPECT_RATIO_16_10 5

typedef struct _tag_DISPLAYID_TIMING_1_DESCRIPTOR
{
    NvU8 pixel_clock_low_minus_0_01MHz;
    NvU8 pixel_clock_mid;
    NvU8 pixel_clock_high;

    struct
    {
        NvU8 aspect_ratio                   : 3;
        NvU8 rsvd                           : 1;
        NvU8 interface_frame_scanning_type  : 1;
        NvU8 stereo_support                 : 2;
        NvU8 is_preferred_detailed_timing   : 1;
    }options;

    struct
    {
        NvU8 active_image_pixels_low_minus_1;
        NvU8 active_image_pixels_high;
        NvU8 blank_pixels_low_minus_1;
        NvU8 blank_pixels_high;
        NvU8 front_porch_low_minus_1;
        NvU8 front_porch_high               : 7;
        NvU8 sync_polarity                  : 1;
        NvU8 sync_width_low_minus_1;
        NvU8 sync_width_high;
    }horizontal;

    struct
    {
        NvU8 active_image_lines_low_minus_1;
        NvU8 active_image_lines_high;
        NvU8 blank_lines_low_minus_1;
        NvU8 blank_lines_high;
        NvU8 front_porch_lines_low_minus_1;
        NvU8 front_porch_lines_high         : 7;
        NvU8 sync_polarity                  : 1;
        NvU8 sync_width_lines_low_minus_1;
        NvU8 sync_width_lines_high;
    }vertical;

} DISPLAYID_TIMING_1_DESCRIPTOR;

#define NVT_DISPLAYID_TIMING_1_MAX_DESCRIPTORS 12

typedef struct _tagDISPLAYID_TIMING_1_BLOCK
{
    DISPLAYID_DATA_BLOCK_HEADER header;
    DISPLAYID_TIMING_1_DESCRIPTOR descriptors[NVT_DISPLAYID_TIMING_1_MAX_DESCRIPTORS];
} DISPLAYID_TIMING_1_BLOCK;

#define NVT_DISPLAYID_TIMING_1_POLARITY_SHIFT 15
#define NVT_DISPLAYID_CHAR_WIDTH_IN_PIXELS    8

typedef struct _tag_DISPLAYID_TIMING_2_DESCRIPTOR
{
    NvU8 pixel_clock_low_minus_0_01MHz;
    NvU8 pixel_clock_mid;
    NvU8 pixel_clock_high;

    struct
    {
        NvU8 rsvd                           : 2;
        NvU8 vsync_polarity                 : 1;
        NvU8 hsync_polarity                 : 1;
        NvU8 interface_frame_scanning_type  : 1;
        NvU8 stereo_support                 : 2;
        NvU8 is_preferred_detailed_timing   : 1;
    }options;

    struct
    {
        NvU8 active_image_in_char_minus_1;
        NvU8 active_image_in_char_high      : 1;
        NvU8 blank_in_char_minus_1          : 7;
        NvU8 sync_width_in_char_minus_1     : 4;
        NvU8 front_porch_in_char_minus_1    : 4;
    }horizontal;

    struct
    {
        NvU8 active_image_lines_low_minus_1;
        NvU8 active_image_lines_high        : 4;
        NvU8 reserved                       : 4;
        NvU8 blank_lines_minus_1;
        NvU8 sync_width_lines_minus_1       : 4;
        NvU8 front_porch_lines_minus_1      : 4;
    }vertical;

} DISPLAYID_TIMING_2_DESCRIPTOR;
 
#define NVT_DISPLAYID_TIMING_2_HORIZ_BLANK_PIXEL       7:1
#define NVT_DISPLAYID_TIMING_2_HORIZ_ACTIVE_PIXEL_HIGH 0:0
#define NVT_DISPLAYID_TIMING_2_HORIZ_OFFSET            7:4
#define NVT_DISPLAYID_TIMING_2_HORIZ_SYNC              3:0
#define NVT_DISPLAYID_TIMING_2_VERT_ACTIVE_PIXEL_HIGH  3:0
#define NVT_DISPLAYID_TIMING_2_VERT_OFFSET             7:4
#define NVT_DISPLAYID_TIMING_2_VERT_SYNC               3:0

#define NVT_DISPLAYID_TIMING_2_MAX_DESCRIPTORS 22

typedef struct _tagDISPLAYID_TIMING_2_BLOCK
{
    DISPLAYID_DATA_BLOCK_HEADER header;
    DISPLAYID_TIMING_2_DESCRIPTOR descriptors[NVT_DISPLAYID_TIMING_2_MAX_DESCRIPTORS];
} DISPLAYID_TIMING_2_BLOCK;

typedef struct _TAG_DISPLAYID_TIMING_3_DESCRIPTOR
{
    NvU8 optns;
    NvU8 horizontal_active_pixels;
    NvU8 transfer;
} DISPLAYID_TIMING_3_DESCRIPTOR;

#define NVT_DISPLAYID_TIMING_3_FORMULA                  6:4
#define NVT_DISPLAYID_TIMING_3_FORMULA_STANDARD         0
#define NVT_DISPLAYID_TIMING_3_FORMULA_REDUCED_BLANKING 1
#define NVT_DISPLAYID_TIMING_3_ASPECT_RATIO             3:0
#define NVT_DISPLAYID_TIMING_3_ASPECT_RATIO_1_1         0
#define NVT_DISPLAYID_TIMING_3_ASPECT_RATIO_5_4         1
#define NVT_DISPLAYID_TIMING_3_ASPECT_RATIO_4_3         2
#define NVT_DISPLAYID_TIMING_3_ASPECT_RATIO_15_9        3
#define NVT_DISPLAYID_TIMING_3_ASPECT_RATIO_16_9        4
#define NVT_DISPLAYID_TIMING_3_ASPECT_RATIO_16_10       5
#define NVT_DISPLAYID_TIMING_3_INTERLACE                7:7
#define NVT_DISPLAYID_TIMING_3_REFRESH_RATE             6:0

#define NVT_DISPLAYID_TIMING_3_MAX_DESCRIPTORS 82

typedef struct _tagDISPLAYID_TIMING_3_BLOCK
{
    DISPLAYID_DATA_BLOCK_HEADER header;
    DISPLAYID_TIMING_3_DESCRIPTOR descriptors[NVT_DISPLAYID_TIMING_3_MAX_DESCRIPTORS];
} DISPLAYID_TIMING_3_BLOCK;

#define NVT_DISPLAYID_TIMING_4_MAX_CODES NVT_DISPLAYID_DATABLOCK_MAX_PAYLOAD_LEN

typedef struct _tagDISPLAYID_TIMING_4_BLOCK
{
    DISPLAYID_DATA_BLOCK_HEADER header;
    NvU8 timing_codes[NVT_DISPLAYID_TIMING_4_MAX_CODES];
} DISPLAYID_TIMING_4_BLOCK;

#define NVT_DISPLAYID_TIMING_5_STEREO_SUPPORT_MASK 0x60
#define NVT_DISPLAYID_TIMING_5_FRACTIONAL_RR_SUPPORT_MASK 0x10
#define NVT_DISPLAYID_TIMING_5_FORMULA_SUPPORT_MASK 3

typedef struct _TAG_DISPLAYID_TIMING_5_DESCRIPTOR
{
    NvU8 optns;
    NvU8 rsvd;
    NvU8 horizontal_active_pixels_low;
    NvU8 horizontal_active_pixels_high;
    NvU8 vertical_active_pixels_low;
    NvU8 vertical_active_pixels_high;
    NvU8 refresh_rate;
} DISPLAYID_TIMING_5_DESCRIPTOR;

#define NVT_DISPLAYID_TIMING_5_MAX_DESCRIPTORS 53

typedef struct _tagDISPLAYID_TIMING_5_BLOCK
{
    DISPLAYID_DATA_BLOCK_HEADER header;
    DISPLAYID_TIMING_5_DESCRIPTOR descriptors[NVT_DISPLAYID_TIMING_5_MAX_DESCRIPTORS];
} DISPLAYID_TIMING_5_BLOCK;

#define DISPLAYID_TIMING_VESA_BLOCK_SIZE 0x0A
#define DISPLAYID_TIMING_CEA_BLOCK_SIZE 0x08

typedef struct _tagDISPLAYID_TIMING_MODE_BLOCK
{
    DISPLAYID_DATA_BLOCK_HEADER header;
    NvU8 timing_modes[DISPLAYID_TIMING_VESA_BLOCK_SIZE];
} DISPLAYID_TIMING_MODE_BLOCK;


typedef struct _tagDISPLAYID_RANGE_LIMITS_BLOCK
{
    DISPLAYID_DATA_BLOCK_HEADER header;
    NvU8 pixel_clock_min[3];
    NvU8 pixel_clock_max[3];
    NvU8 horizontal_frequency_min;
    NvU8 horizontal_frequency_max;
    NvU16 horizontal_blanking_min;
    NvU8 vertical_refresh_rate_min;
    NvU8 vertical_refresh_rate_max;
    NvU16 vertical_blanking_min;

    NvU8 optns;
} DISPLAYID_RANGE_LIMITS_BLOCK;

#define DISPLAYID_RANGE_LIMITS_BLOCK_LEN 0xF

#define NVT_DISPLAYID_RANGE_LIMITS_INTERLACE    7:7
#define NVT_DISPLAYID_RANGE_LIMITS_CVT_STANDARD 6:6
#define NVT_DISPLAYID_RANGE_LIMITS_CVT_REDUCED  5:5
#define NVT_DISPLAYID_RANGE_LIMITS_DFD          4:4

typedef struct _tagDISPLAYID_ASCII_STRING_BLOCK
{
    DISPLAYID_DATA_BLOCK_HEADER header;
    NvU8 data[NVT_DISPLAYID_DATABLOCK_MAX_PAYLOAD_LEN];
} DISPLAYID_ASCII_STRING_BLOCK;

typedef struct _tagDISPLAYID_DEVICE_DATA_BLOCK
{
    DISPLAYID_DATA_BLOCK_HEADER header;

    NvU8 technology;
    NvU8 operating_mode;
    NvU16 horizontal_pixel_count;
    NvU16 vertical_pixel_count;
    NvU8 aspect_ratio;
    NvU8 orientation;

    NvU8 subpixel_info;
    NvU8 horizontal_pitch;
    NvU8 vertical_pitch;

    NvU8 color_bit_depth;
    NvU8 response_time;

} DISPLAYID_DEVICE_DATA_BLOCK;

#define DISPLAYID_DEVICE_DATA_BLOCK_LEN 0xD

#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_CRT_MONOCHROME              0x00
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_CRT_STANDARD                0x01
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_CRT_OTHER                   0x02
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_LCD_PASSIVE_MATRIX_TN       0x10
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_LCD_PASSIVE_MATRIX_CHOL_LC  0x11
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_LCD_PASSIVE_MATRIX_FERRO_LC 0x12
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_LCD_PASSIVE_MATRIX_OTHER    0x13
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_LCD_ACTIVE_MATRIX_TN        0x14
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_LCD_ACTIVE_MATRIX_IPS       0x15
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_LCD_ACTIVE_MATRIX_VA        0x16
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_LCD_ACTIVE_MATRIX_OCB       0x17
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_LCD_ACTIVE_MATRIX_FERRO     0x18
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_LCD_OTHER                   0x1F
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_PLASMA_DC                   0x20
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_PLASMA_AC                   0x21
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_ELECTROLUM                  0x30
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_INORGANIC_LED               0x40
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_ORGANIC_LED                 0x50
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_FED                         0x60
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_ELECTROPHORETIC             0x70
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_ELECTROCHROMIC              0x80
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_ELECTROMECHANICAL           0x90
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_ELECTROWETTING              0xA0
#define NVT_DISPLAYID_DEVICE_TECHNOLOGY_OTHER                       0xF0

// Display Device operating mode info
#define NVT_DISPLAYID_DEVICE_OPERATING_MODE                        7:4
#define NVT_DISPLAYID_DEVICE_OPERATING_MODE_REFLECTIVE_NO_ILLUM    0x0
#define NVT_DISPLAYID_DEVICE_OPERATING_MODE_REFLECTIVE_ILLUM       0x1
#define NVT_DISPLAYID_DEVICE_OPERATING_MODE_REFLECTIVE_ILLUM_DEF   0x2
#define NVT_DISPLAYID_DEVICE_OPERATING_MODE_TRANSMISSIVE_NO_ILLUM  0x3
#define NVT_DISPLAYID_DEVICE_OPERATING_MODE_TRANSMISSIVE_ILLUM     0x4
#define NVT_DISPLAYID_DEVICE_OPERATING_MODE_TRANSMISSIVE_ILLUM_DEF 0x5
#define NVT_DISPLAYID_DEVICE_OPERATING_MODE_EMISSIVE               0x6
#define NVT_DISPLAYID_DEVICE_OPERATING_MODE_TRANSFLECTIVE_REF      0x7
#define NVT_DISPLAYID_DEVICE_OPERATING_MODE_TRANSFLECTIVE_TRANS    0x8
#define NVT_DISPLAYID_DEVICE_OPERATING_MODE_TRANSPARENT_AMB        0x9
#define NVT_DISPLAYID_DEVICE_OPERATING_MODE_TRANSPARENT_EMIS       0xA
#define NVT_DISPLAYID_DEVICE_OPERATING_MODE_PROJECTION_REF         0xB
#define NVT_DISPLAYID_DEVICE_OPERATING_MODE_PROJECTION_TRANS       0xC
#define NVT_DISPLAYID_DEVICE_OPERATING_MODE_PROJECTION_EMIS        0xD
#define NVT_DISPLAYID_DEVICE_BACKLIGHT                             3:3
#define NVT_DISPLAYID_DEVICE_INTENSITY                             2:2

// Display Device aspect ratio/orientation info
#define NVT_DISPLAYID_DEVICE_ORIENTATION               7:6
#define NVT_DISPLAYID_DEVICE_ORIENTATION_LANDSCAPE     0
#define NVT_DISPLAYID_DEVICE_ORIENTATION_PORTRAIT      1
#define NVT_DISPLAYID_DEVICE_ORIENTATION_NOT_FIXED     2
#define NVT_DISPLAYID_DEVICE_ORIENTATION_UNDEFINED     3
#define NVT_DISPLAYID_DEVICE_ROTATION                  5:4
#define NVT_DISPLAYID_DEVICE_ROTATION_NONE             0
#define NVT_DISPLAYID_DEVICE_ROTATION_CLOCKWISE        1
#define NVT_DISPLAYID_DEVICE_ROTATION_COUNTERCLOCKWISE 2
#define NVT_DISPLAYID_DEVICE_ROTATION_BOTH             3
#define NVT_DISPLAYID_DEVICE_ZERO_PIXEL                3:2
#define NVT_DISPLAYID_DEVICE_ZERO_PIXEL_UPPER_LEFT     0
#define NVT_DISPLAYID_DEVICE_ZERO_PIXEL_UPPER_RIGHT    1
#define NVT_DISPLAYID_DEVICE_ZERO_PIXEL_LOWER_LEFT     2
#define NVT_DISPLAYID_DEVICE_ZERO_PIXEL_LOWER RIGHT    3
#define NVT_DISPLAYID_DEVICE_SCAN                      1:0
#define NVT_DISPLAYID_DEVICE_SCAN_UNDEFINED            0
#define NVT_DISPLAYID_DEVICE_SCAN_FAST_LONG            1
#define NVT_DISPLAYID_DEVICE_SCAN_FAST_SHORT           2

// Display Device Color Depth information
#define NVT_DISPLAYID_DEVICE_COLOR_DEPTH 3:0

// Display Device Response Time information
#define NVT_DISPLAYID_DEVICE_WHITE_BLACK       7:7
#define NVT_DISPLAYID_DEVICE_RESPONSE_TIME     6:0

#define NVT_DISPLAYID_SUBPIXEL_UNDEFINED            0
#define NVT_DISPLAYID_SUBPIXEL_RGB_VERTICAL         1
#define NVT_DISPLAYID_SUBPIXEL_RGB_HORIZONTAL       2
#define NVT_DISPLAYID_SUBPIXEL_VERTICAL_STR         3
#define NVT_DISPLAYID_SUBPIXEL_HORIZONTAL_STR       4
#define NVT_DISPLAYID_SUBPIXEL_QUAD_RED_TOP_LEFT    5
#define NVT_DISPLAYID_SUBPIXEL_QUAD_RED_BOTTOM_LEFT 6
#define NVT_DISPLAYID_SUBPIXEL_DELTA_RGB            7
#define NVT_DISPLAYID_SUBPIXEL_MOSAIC               8
#define NVT_DISPLAYID_SUBPIXEL_QUAD_INC_WHITE       9
#define NVT_DISPLAYID_SUBPIXEL_FIVE                 10
#define NVT_DISPLAYID_SUBPIXEL_SIX                  11
#define NVT_DISPLAYID_SUBPIXEL_PENTILE              12

typedef struct _tagDISPLAYID_INTERFACE_POWER_BLOCK
{
    DISPLAYID_DATA_BLOCK_HEADER header;
    NvU8 power_sequence_T1;
    NvU8 power_sequence_T2;
    NvU8 power_sequence_T3;
    NvU8 power_sequence_T4_min;
    NvU8 power_sequence_T5_min;
    NvU8 power_sequence_T6_min;
} DISPLAYID_INTERFACE_POWER_BLOCK;

#define DISPLAYID_INTERFACE_POWER_BLOCK_LEN 0x6

#define NVT_DISPLAYID_POWER_T1_MIN 7:4
#define NVT_DISPLAYID_POWER_T1_MAX 3:0
#define NVT_DISPLAYID_POWER_T2 5:0
#define NVT_DISPLAYID_POWER_T3 5:0
#define NVT_DISPLAYID_POWER_T4_MIN 6:0
#define NVT_DISPLAYID_POWER_T5_MIN 5:0
#define NVT_DISPLAYID_POWER_T6_MIN 5:0

typedef struct _tagDISPLAYID_TRANSFER_CHAR_BLOCK
{
    DISPLAYID_DATA_BLOCK_HEADER header;
    NvU8 info;
    NvU8 samples;
    NvU8 curve_data[NVT_DISPLAYID_DATABLOCK_MAX_PAYLOAD_LEN - 2];
} DISPLAYID_TRANSFER_CHAR_BLOCK;

typedef struct _tagDISPLAYID_INTERFACE_DATA_BLOCK
{
    DISPLAYID_DATA_BLOCK_HEADER header;
    NvU8 info;

    NvU8 version;
    NvU8 color_depth_rgb;
    NvU8 color_depth_ycbcr444;
    NvU8 color_depth_ycbcr422;
    NvU8 content_protection;
    NvU8 content_protection_version;

    NvU8 spread;

    NvU8 interface_attribute_1;
    NvU8 interface_attribute_2;
} DISPLAYID_INTERFACE_DATA_BLOCK;

#define DISPLAYID_INTERFACE_DATA_BLOCK_LEN 0xA

#define NVT_DISPLAYID_INTERFACE_TYPE        7:4

// Interface Codes (note exception for Analog Interface)
#define NVT_DISPLAYID_INTERFACE_TYPE_ANALOG        0
#define NVT_DISPLAYID_INTERFACE_TYPE_LVDS          1
#define NVT_DISPLAYID_INTERFACE_TYPE_TMDS          2
#define NVT_DISPLAYID_INTERFACE_TYPE_RSDS          3
#define NVT_DISPLAYID_INTERFACE_TYPE_DVI_D         4
#define NVT_DISPLAYID_INTERFACE_TYPE_DVI_I_ANALOG  5
#define NVT_DISPLAYID_INTERFACE_TYPE_DVI_I_DIGITAL 6
#define NVT_DISPLAYID_INTERFACE_TYPE_HDMI_A        7
#define NVT_DISPLAYID_INTERFACE_TYPE_HDMI_B        8
#define NVT_DISPLAYID_INTERFACE_TYPE_MDDI          9
#define NVT_DISPLAYID_INTERFACE_TYPE_DISPLAYPORT   10
#define NVT_DISPLAYID_INTERFACE_TYPE_PROPRIETARY   11

// Analog Interface Subtype codes
#define NVT_DISPLAYID_INTERFACE_TYPE_ANALOG_VGA            0
#define NVT_DISPLAYID_INTERFACE_TYPE_ANALOG_VESA_NAVI_V    1
#define NVT_DISPLAYID_INTERFACE_TYPE_ANALOG_VESA_NAVI_D    2

#define NVT_DISPLAYID_INTERFACE_NUMLINKS           3:0
#define NVT_DISPLAYID_INTERFACE_CONTENT            2:0
#define NVT_DISPLAYID_INTERFACE_CONTENT_NONE       0
#define NVT_DISPLAYID_INTERFACE_CONTENT_HDCP       1
#define NVT_DISPLAYID_INTERFACE_CONTENT_DTCP       2
#define NVT_DISPLAYID_INTERFACE_CONTENT_DPCP       3
#define NVT_DISPLAYID_INTERFACE_SPREAD_TYPE        7:6
#define NVT_DISPLAYID_INTERFACE_SPREAD_TYPE_NONE   0
#define NVT_DISPLAYID_INTERFACE_SPREAD_TYPE_DOWN   1
#define NVT_DISPLAYID_INTERFACE_SPREAD_TYPE_CENTER 2
#define NVT_DISPLAYID_INTERFACE_SPREAD_PER         3:0

#define NVT_DISPLAYID_INTERFACE_RGB16 5:5
#define NVT_DISPLAYID_INTERFACE_RGB14 4:4
#define NVT_DISPLAYID_INTERFACE_RGB12 3:3
#define NVT_DISPLAYID_INTERFACE_RGB10 2:2
#define NVT_DISPLAYID_INTERFACE_RGB8  1:1
#define NVT_DISPLAYID_INTERFACE_RGB6  0:0

#define NVT_DISPLAYID_INTERFACE_YCBCR444_16 5:5
#define NVT_DISPLAYID_INTERFACE_YCBCR444_14 4:4
#define NVT_DISPLAYID_INTERFACE_YCBCR444_12 3:3
#define NVT_DISPLAYID_INTERFACE_YCBCR444_10 2:2
#define NVT_DISPLAYID_INTERFACE_YCBCR444_8  1:1
#define NVT_DISPLAYID_INTERFACE_YCBCR444_6  0:0

#define NVT_DISPLAYID_INTERFACE_YCBCR422_16 4:4
#define NVT_DISPLAYID_INTERFACE_YCBCR422_14 3:3
#define NVT_DISPLAYID_INTERFACE_YCBCR422_12 2:2
#define NVT_DISPLAYID_INTERFACE_YCBCR422_10 1:1
#define NVT_DISPLAYID_INTERFACE_YCBCR422_8  0:0

// LVDS specific settings
#define NVT_DISPLAYID_LVDS_COLOR 4:4
#define NVT_DISPLAYID_LVDS_2_8   3:3
#define NVT_DISPLAYID_LVDS_12    2:2
#define NVT_DISPLAYID_LVDS_5     1:1
#define NVT_DISPLAYID_LVDS_3_3   0:0

#define NVT_DISPLAYID_INTERFACE_DE       2:2
#define NVT_DISPLAYID_INTERFACE_POLARITY 1:1
#define NVT_DISPLAYID_INTERFACE_STROBE   0:0

typedef struct _tagDISPLAYID_STEREO_INTERFACE_METHOD_BLOCK
{
    DISPLAYID_DATA_BLOCK_HEADER header;
    NvU8 stereo_bytes;
    NvU8 stereo_code;
    NvU8 timing_sub_block[NVT_DISPLAYID_DATABLOCK_MAX_PAYLOAD_LEN];
} DISPLAYID_STEREO_INTERFACE_METHOD_BLOCK;

#define NVT_DISPLAYID_STEREO_FIELD_SEQUENTIAL  0x0
#define NVT_DISPLAYID_STEREO_SIDE_BY_SIDE      0x1
#define NVT_DISPLAYID_STEREO_PIXEL_INTERLEAVED 0x2
#define NVT_DISPLAYID_STEREO_DUAL_INTERFACE    0x3
#define NVT_DISPLAYID_STEREO_MULTIVIEW         0x4
#define NVT_DISPLAYID_STEREO_PROPRIETARY       0xFF

#define NVT_DISPLAYID_STEREO_MIRRORING 2:1
#define NVT_DISPLAYID_STEREO_POLARITY  0:0

typedef struct _tagDISPLAYID_TILED_DISPLAY_BLOCK
{
    DISPLAYID_DATA_BLOCK_HEADER header;
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
    } topology_low;
    struct
    {
        NvU8 y                :4;          // 0x05
        NvU8 x                :4;          // 0x05
    } location_low;
    struct
    {
        NvU8 y                :1;          // 0x06
        NvU8 reserved1        :1;          // 0x06
        NvU8 x                :1;          // 0x06
        NvU8 reserved2        :1;          // 0x06
        NvU8 row              :1;          // 0x06
        NvU8 reserved3        :1;          // 0x06
        NvU8 col              :1;          // 0x06
        NvU8 reserved4        :1;          // 0x06
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
    } topology_id;
} DISPLAYID_TILED_DISPLAY_BLOCK;

typedef struct _tagDISPLAYID_INTERFACE_FEATURES_DATA_BLOCK
{
    DISPLAYID_DATA_BLOCK_HEADER header;
    NvU8 supported_color_depth_rgb;
    NvU8 supported_color_depth_ycbcr444;
    NvU8 supported_color_depth_ycbcr422;
    NvU8 supported_color_depth_ycbcr420;
    NvU8 minimum_pixel_rate_ycbcr420;
    NvU8 supported_audio_capability;
    NvU8 supported_colorspace_eotf_combination_1;
    NvU8 supported_colorspace_eotf_combination_2;
    NvU8 additional_supported_colorspace_eotf_total;
    NvU8 additional_supported_colorspace_eotf[NVT_DISPLAYID_DISPLAY_INTERFACE_FEATURES_MAX_ADDITIONAL_SUPPORTED_COLORSPACE_EOTF];
} DISPLAYID_INTERFACE_FEATURES_DATA_BLOCK;

#define DISPLAYID_INTERFACE_FEATURES_DATA_BLOCK_MAX_LEN sizeof(DISPLAYID_INTERFACE_FEATURES_DATA_BLOCK)

#define NVT_DISPLAYID_INTERFACE_FEATURES_RGB16 5:5
#define NVT_DISPLAYID_INTERFACE_FEATURES_RGB14 4:4
#define NVT_DISPLAYID_INTERFACE_FEATURES_RGB12 3:3
#define NVT_DISPLAYID_INTERFACE_FEATURES_RGB10 2:2
#define NVT_DISPLAYID_INTERFACE_FEATURES_RGB8  1:1
#define NVT_DISPLAYID_INTERFACE_FEATURES_RGB6  0:0

#define NVT_DISPLAYID_INTERFACE_FEATURES_YCBCR444_16 5:5
#define NVT_DISPLAYID_INTERFACE_FEATURES_YCBCR444_14 4:4
#define NVT_DISPLAYID_INTERFACE_FEATURES_YCBCR444_12 3:3
#define NVT_DISPLAYID_INTERFACE_FEATURES_YCBCR444_10 2:2
#define NVT_DISPLAYID_INTERFACE_FEATURES_YCBCR444_8  1:1
#define NVT_DISPLAYID_INTERFACE_FEATURES_YCBCR444_6  0:0

#define NVT_DISPLAYID_INTERFACE_FEATURES_YCBCR422_16 4:4
#define NVT_DISPLAYID_INTERFACE_FEATURES_YCBCR422_14 3:3
#define NVT_DISPLAYID_INTERFACE_FEATURES_YCBCR422_12 2:2
#define NVT_DISPLAYID_INTERFACE_FEATURES_YCBCR422_10 1:1
#define NVT_DISPLAYID_INTERFACE_FEATURES_YCBCR422_8  0:0

#define NVT_DISPLAYID_INTERFACE_FEATURES_YCBCR420_16 4:4
#define NVT_DISPLAYID_INTERFACE_FEATURES_YCBCR420_14 3:3
#define NVT_DISPLAYID_INTERFACE_FEATURES_YCBCR420_12 2:2
#define NVT_DISPLAYID_INTERFACE_FEATURES_YCBCR420_10 1:1
#define NVT_DISPLAYID_INTERFACE_FEATURES_YCBCR420_8  0:0

#define NVT_DISPLAYID_INTERFACE_FEATURES_AUDIO_SUPPORTED_32KHZ      7:7
#define NVT_DISPLAYID_INTERFACE_FEATURES_AUDIO_SUPPORTED_44_1KHZ    6:6
#define NVT_DISPLAYID_INTERFACE_FEATURES_AUDIO_SUPPORTED_48KHZ      5:5

#define NVT_DISPLAYID_INTERFACE_FEATURES_COLORSPACE_BT2020_EOTF_SMPTE_ST2084    6:6
#define NVT_DISPLAYID_INTERFACE_FEATURES_COLORSPACE_BT2020_EOTF_BT2020          5:5
#define NVT_DISPLAYID_INTERFACE_FEATURES_COLORSPACE_DCI_P3_EOTF_DCI_P3          4:4
#define NVT_DISPLAYID_INTERFACE_FEATURES_COLORSPACE_ADOBE_RGB_EOTF_ADOBE_RGB    3:3
#define NVT_DISPLAYID_INTERFACE_FEATURES_COLORSPACE_BT709_EOTF_BT1886           2:2
#define NVT_DISPLAYID_INTERFACE_FEATURES_COLORSPACE_BT601_EOTF_BT601            1:1
#define NVT_DISPLAYID_INTERFACE_FEATURES_COLORSPACE_SRGB_EOTF_SRGB              0:0

#define NVT_DISPLAYID_INTERFACE_FEATURES_ADDITIONAL_SUPPORTED_COLORSPACE_EOTF_TOTAL   2:0

#define NVT_DISPLAYID_INTERFACE_FEATURES_ADDITIONAL_SUPPORTED_COLORSPACE              7:4
#define NVT_DISPLAYID_INTERFACE_FEATURES_ADDITIONAL_SUPPORTED_COLORSPACE_NOT_DEFINED  0
#define NVT_DISPLAYID_INTERFACE_FEATURES_ADDITIONAL_SUPPORTED_COLORSPACE_SRGB         1
#define NVT_DISPLAYID_INTERFACE_FEATURES_ADDITIONAL_SUPPORTED_COLORSPACE_BT601        2
#define NVT_DISPLAYID_INTERFACE_FEATURES_ADDITIONAL_SUPPORTED_COLORSPACE_BT709        3
#define NVT_DISPLAYID_INTERFACE_FEATURES_ADDITIONAL_SUPPORTED_COLORSPACE_ADOBE_RGB    4
#define NVT_DISPLAYID_INTERFACE_FEATURES_ADDITIONAL_SUPPORTED_COLORSPACE_DCI_P3       5
#define NVT_DISPLAYID_INTERFACE_FEATURES_ADDITIONAL_SUPPORTED_COLORSPACE_BT2020       6
#define NVT_DISPLAYID_INTERFACE_FEATURES_ADDITIONAL_SUPPORTED_COLORSPACE_CUSTOM       7

#define NVT_DISPLAYID_INTERFACE_FEATURES_ADDITIONAL_SUPPORTED_EOTF              3:0
#define NVT_DISPLAYID_INTERFACE_FEATURES_ADDITIONAL_SUPPORTED_EOTF_NOT_DEFINED  0
#define NVT_DISPLAYID_INTERFACE_FEATURES_ADDITIONAL_SUPPORTED_EOTF_SRGB         1
#define NVT_DISPLAYID_INTERFACE_FEATURES_ADDITIONAL_SUPPORTED_EOTF_BT601        2
#define NVT_DISPLAYID_INTERFACE_FEATURES_ADDITIONAL_SUPPORTED_EOTF_BT709        3
#define NVT_DISPLAYID_INTERFACE_FEATURES_ADDITIONAL_SUPPORTED_EOTF_ADOBE_RGB    4
#define NVT_DISPLAYID_INTERFACE_FEATURES_ADDITIONAL_SUPPORTED_EOTF_DCI_P3       5
#define NVT_DISPLAYID_INTERFACE_FEATURES_ADDITIONAL_SUPPORTED_EOTF_BT2020       6
#define NVT_DISPLAYID_INTERFACE_FEATURES_ADDITIONAL_SUPPORTED_EOTF_GAMMA        7
#define NVT_DISPLAYID_INTERFACE_FEATURES_ADDITIONAL_SUPPORTED_EOTF_SMPTE_ST2084 8
#define NVT_DISPLAYID_INTERFACE_FEATURES_ADDITIONAL_SUPPORTED_EOTF_HYBRID_LOG   9
#define NVT_DISPLAYID_INTERFACE_FEATURES_ADDITIONAL_SUPPORTED_EOTF_CUSTOM       10


#ifdef __SUPPORTS_PACK_PRAGMA
#pragma pack()
#endif

#endif // __DISPLAYID_H_
