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
//  File:       edid.h
//
//  Purpose:    the template for EDID parse
//
//*****************************************************************************

#ifndef __EDID_H_
#define __EDID_H_

#include "nvtiming_pvt.h"
#include "displayid.h"
#include "displayid20.h"

// EDID 1.x detailed timing template

#define NVT_PVT_EDID_LDD_PAYLOAD_SIZE       13

typedef struct  _tagEDID_LONG_DISPLAY_DESCRIPTOR
{
    // the header
    NvU8   prefix[2];                  // 0x00 ~ 0x01
    NvU8   rsvd;                       // 0x02
    NvU8   tag;                        // 0x03
    NvU8   rsvd2;                      // 0x04

    // the payload
    NvU8   data[NVT_PVT_EDID_LDD_PAYLOAD_SIZE];  // 0x05~0x11
}EDID_LONG_DISPLAY_DESCRIPTOR;
typedef struct _tagEDID_MONITOR_RANGE_GTF2
{
    NvU8    reserved;               // byte 0x0B: reserved as 00
    NvU8    startFreq;              // byte 0x0C: start frequency for secondary curve, hot. freq./2[kHz]
    NvU8    C;                      // byte 0x0D: C*2  0 <= 127
    NvU8    M_LSB;                  // byte 0x0E-0x0F: M (LSB) 0 <= M <= 65535
    NvU8    M_MSB;
    NvU8    K;                      // byte 0x10: K   0 <= K <= 255
    NvU8    J;                      // byte 0x11: J*2 0 <= J <= 127
}EDID_MONITOR_RANGE_GTF2;

typedef struct _tagEDID_MONITOR_RANGE_CVT
{
    NvU8    version;                    // byte 0x0B: cvt version
    NvU8    pixel_clock;                // byte 0x0C: [bits 7:2]pixel clock precision
                                        //            [bits 1:0]max active MSB
    NvU8    max_active;                 // byte 0x0D: with byte 12 [bits 1:0], max active pixels per line
    NvU8    aspect_supported;           // byte 0x0E: supported aspect ratios
    NvU8    aspect_preferred_blanking;  // byte 0x0F: preferred aspect ratio / blanking style support
    NvU8    scaling_support;            // byte 0x10: display scaling support
    NvU8    preferred_refresh_rate;     // byte 0x11: preferred vertical refresh rate
}EDID_MONITOR_RANGE_CVT;

// cvt support in display range limit block
#define NVT_PVT_EDID_CVT_PIXEL_CLOCK_MASK       0xFC
#define NVT_PVT_EDID_CVT_PIXEL_CLOCK_SHIFT      2
#define NVT_PVT_EDID_CVT_ACTIVE_MSB_MASK        0x03
#define NVT_PVT_EDID_CVT_ACTIVE_MSB_SHIFT       8

#define NVT_PVT_EDID_CVT_ASPECT_SUPPORTED_MASK  0xF8
#define NVT_PVT_EDID_CVT_ASPECT_SUPPORTED_SHIFT 3
#define NVT_PVT_EDID_CVT_RESERVED0_MASK         0x07
#define NVT_PVT_EDID_CVT_RESERVED0_SHIFT        0

#define NVT_PVT_EDID_CVT_ASPECT_PREFERRED_MASK  0xE0
#define NVT_PVT_EDID_CVT_ASPECT_PREFERRED_SHIFT 5
#define NVT_PVT_EDID_CVT_BLANKING_MASK          0x18
#define NVT_PVT_EDID_CVT_BLANKING_SHIFT         3
#define NVT_PVT_EDID_CVT_RESERVED1_MASK         0x07
#define NVT_PVT_EDID_CVT_RESERVED1_SHIFT        0

#define NVT_PVT_EDID_CVT_SCALING_MASK           0xF0
#define NVT_PVT_EDID_CVT_SCALING_SHIFT          4
#define NVT_PVT_EDID_CVT_RESERVED2_MASK         0x0F
#define NVT_PVT_EDID_CVT_RESERVED2_SHIFT        0

typedef struct _tagEDID_MONITOR_RANGE_LIMIT
{
    // the header in monitor descriptor data
    NvU8    minVRate;               // byte 0x05: min vertical rate
    NvU8    maxVRate;               // byte 0x06: max vertical rate
    NvU8    minHRate;               // byte 0x07: min horizontal rate
    NvU8    maxHRate;               // byte 0x08: max horizontal rate
    NvU8    maxPClock10M;           // byte 0x09: max pixel clock in 10M
    NvU8    timing_support;         // byte 0x0A: 2nd GTF / CVT timing formula support
    union
    {
        EDID_MONITOR_RANGE_GTF2 gtf2; // bytes 0x0B-0x11
        EDID_MONITOR_RANGE_CVT  cvt;  // ...
    }u;
} EDID_MONITOR_RANGE_LIMIT;

// timing_support
#define NVT_PVT_EDID_RANGE_OFFSET_VER_MIN       0x01
#define NVT_PVT_EDID_RANGE_OFFSET_VER_MAX       0x02
#define NVT_PVT_EDID_RANGE_OFFSET_HOR_MIN       0x04
#define NVT_PVT_EDID_RANGE_OFFSET_HOR_MAX       0x08
#define NVT_PVT_EDID_RANGE_OFFSET_AMOUNT        255

typedef struct _tagEDID_CVT_3BYTE_BLOCK
{
    NvU8 addressable_lines;     // byte 0: 8 lsb of addressable lines 
    NvU8 lines_ratio;           // byte 1 : [bits7:4] 4 msb of addressable lines [bits3:2] aspect ratio 
    NvU8 refresh_rates;         // byte 2 : supported/preferred refresh rates
}EDID_CVT_3BYTE_BLOCK;

typedef struct _tagEDID_CVT_3BYTE
{
    // the header in monitor descriptor data.
    NvU8  version;                                                   // byte 0x05 : version code (0x01)
    EDID_CVT_3BYTE_BLOCK block[NVT_EDID_DD_MAX_CVT3_PER_DESCRITPOR]; // bytes 0x06-0x11
}EDID_CVT_3BYTE;

// CVT 3byte
#define NVT_PVT_EDID_CVT3_LINES_MSB_MASK    0xF0
#define NVT_PVT_EDID_CVT3_LINES_MSB_SHIFT   4
#define NVT_PVT_EDID_CVT3_ASPECT_MASK       0x0C
#define NVT_PVT_EDID_CVT3_ASPECT_SHIFT      2

#define NVT_PVT_EDID_CVT3_PREFERRED_RATE_MASK   0x60
#define NVT_PVT_EDID_CVT3_PREFERRED_RATE_SHIFT  5
#define NVT_PVT_EDID_CVT3_SUPPORTED_RATE_MASK   0x1F
#define NVT_PVT_EDID_CVT3_SUPPORTED_RATE_SHIFT  0

typedef struct _tagEDID_COLOR_POINT_DATA
{
    NvU8  wp1_index;        // 0x05: white point index number
    NvU8  wp1_x_y;          // 0x06: [bits3:2] lsb of wp1_x [bits1:0] lsb of wp1_y
    NvU8  wp1_x;            // 0x07: msb of wp1_x
    NvU8  wp1_y;            // 0x08: msb of wp1_y
    NvU8  wp1_gamma;        // 0x09: (gamma x 100) - 100
    NvU8  wp2_index;        // 0x0A: ...
    NvU8  wp2_x_y;          // 0x0B: ...
    NvU8  wp2_x;            // 0x0C: ...
    NvU8  wp2_y;            // 0x0D: ...
    NvU8  wp2_gamma;        // 0x0E: ...
    NvU8  line_feed;        // 0x0F: reserved for line feed (0x0A)
    NvU16 reserved0;        // 0x10-0x11: reserved for space (0x2020)
}EDID_COLOR_POINT_DATA;

#define NVT_PVT_EDID_CPD_WP_X_MASK      0x0C
#define NVT_PVT_EDID_CPD_WP_X_SHIFT     2
#define NVT_PVT_EDID_CPD_WP_Y_MASK      0x03
#define NVT_PVT_EDID_CPD_WP_Y_SHIFT     0

typedef struct _tagEDID_STANDARD_TIMING_ID
{
    NvU16 std_timing[NVT_EDID_DD_STI_NUM];    //0x05-0x10: 6 standard timings
    NvU8  line_feed;                          //0x11: reserved for line feed (0x0A)
}EDID_STANDARD_TIMING_ID;

typedef struct _tagEDID_COLOR_MANAGEMENT_DATA
{
    NvU8  version;          //0x05: version (0x03)
    NvU8  red_a3_lsb;       //0x06: Red a3 LSB
    NvU8  red_a3_msb;       //0x07: Red a3 MSB
    NvU8  red_a2_lsb;       //0x08
    NvU8  red_a2_msb;       //0x09
    NvU8  green_a3_lsb;     //0x0A
    NvU8  green_a3_msb;     //0x0B
    NvU8  green_a2_lsb;     //0x0C
    NvU8  green_a2_msb;     //0x0D
    NvU8  blue_a3_lsb;      //0x0E
    NvU8  blue_a3_msb;      //0x0F
    NvU8  blue_a2_lsb;      //0x10
    NvU8  blue_a2_msb;      //0x11
}EDID_COLOR_MANAGEMENT_DATA;

typedef struct _tagEDID_EST_TIMINGS_III
{
    NvU8 revision;          //0x05: revision (0x0A)
    NvU8 timing_byte[12];   //0x05-0x11: established timings III
}EDID_EST_TIMINGS_III;

typedef struct  _tagDETAILEDTIMINGDESCRIPTOR
{
    NvU16  wDTPixelClock;              // 0x00
    NvU8   bDTHorizontalActive;        // 0x02
    NvU8   bDTHorizontalBlanking;      // 0x03
    NvU8   bDTHorizActiveBlank;        // 0x04
    NvU8   bDTVerticalActive;          // 0x05
    NvU8   bDTVerticalBlanking;        // 0x06
    NvU8   bDTVertActiveBlank;         // 0x07
    NvU8   bDTHorizontalSync;          // 0x08
    NvU8   bDTHorizontalSyncWidth;     // 0x09
    NvU8   bDTVerticalSync;            // 0x0A
    NvU8   bDTHorizVertSyncOverFlow;   // 0x0B
    NvU8   bDTHorizontalImage;         // 0x0C
    NvU8   bDTVerticalImage;           // 0x0D
    NvU8   bDTHorizVertImage;          // 0x0E
    NvU8   bDTHorizontalBorder;        // 0x0F
    NvU8   bDTVerticalBorder;          // 0x10
    NvU8   bDTFlags;                   // 0x11
}DETAILEDTIMINGDESCRIPTOR;

// EDID 1.x basic block template
typedef struct _tagEDIDV1STRUC
{
    NvU8   bHeader[8];                 // 0x00-0x07
    NvU16  wIDManufName;               // 0x08
    NvU16  wIDProductCode;             // 0x0A
    NvU32  dwIDSerialNumber;           // 0x0C
    NvU8   bWeekManuf;                 // 0x10
    NvU8   bYearManuf;                 // 0x11
    NvU8   bVersionNumber;             // 0x12
    NvU8   bRevisionNumber;            // 0x13
    NvU8   bVideoInputDef;             // 0x14
    NvU8   bMaxHorizImageSize;         // 0x15
    NvU8   bMaxVertImageSize;          // 0x16
    NvU8   bDisplayXferChar;           // 0x17
    NvU8   bFeatureSupport;            // 0x18
    NvU8   Chromaticity[10];           // 0x19-0x22
    NvU8   bEstablishedTimings1;       // 0x23
    NvU8   bEstablishedTimings2;       // 0x24
    NvU8   bManufReservedTimings;      // 0x25
    NvU16  wStandardTimingID[8];       // 0x26
    DETAILEDTIMINGDESCRIPTOR           DetailedTimingDesc[4];  // 0x36
    NvU8   bExtensionFlag;             // 0x7E
    NvU8   bChecksum;                  // 0x7F
}EDIDV1STRUC;

// EDID 2.x basic block template
typedef struct _tagEDIDV2STRUC
{
    NvU8   bHeader;                        // 0x00
    NvU16  wIDManufName;                   // 0x01
    NvU16  wIDProductCode;                 // 0x03
    NvU8   bWeekManuf;                     // 0x05
    NvU16  wYearManuf;                     // 0x06
    NvU8   bProductIDString[32];           // 0x08
    NvU8   bSerialNumber[16];              // 0x28
    NvU8   bReserved1[8];                  // 0x38
    NvU8   bPhysicalInterfaceType;         // 0x40
    NvU8   bVideoInterfaceType;            // 0x41
    NvU8   bInterfaceDataFormat[8];        // 0x42
    NvU8   bInterfaceColor[5];             // 0x4A
    NvU8   bDisplayTechType;               // 0x4F
    NvU8   bMajorDisplayChar;              // 0x50
    NvU8   bFeaturesSupported[3];          // 0x51
    NvU16  wDisplayResponseTime;           // 0x54
    NvU32  dwDisplayXferChar;              // 0x56
    NvU32  dwMaxLuminance;                 // 0x5A
    NvU8   bColorimetry[20];               // 0x5E
    NvU16  wMaxHorizImageSize;             // 0x72
    NvU16  wMaxVertImageSize;              // 0x74
    NvU16  wMaxHorizAddressibility;        // 0x76
    NvU16  wMaxVertAddressibility;         // 0x78
    NvU8   bHorizPixelPitch;               // 0x7A
    NvU8   bVertPixelPitch;                // 0x7B
    NvU8   bReserved2;                     // 0x7C
    NvU8   bGTFSupportInfo;                // 0x7D
    NvU16  wTimingInfoMap;                 // 0x7E
    NvU8   bTableDescriptors[127];         // 0x80
    NvU8   bChecksum;                      // 0xFF
}EDIDV2STRUC;

// EDID CEA/EIA-861 extension block template
typedef struct _tagEIA861EXTENSION
{
    NvU8   tag;                            // 0x00
    NvU8   revision;                       // 0x01
    NvU8   offset;                         // 0x02
    NvU8   misc;                           // 0x03
    NvU8   data[NVT_CEA861_MAX_PAYLOAD];   // 0x04 - 0x7E
    NvU8   checksum;                       // 0x7F
}EIA861EXTENSION;

typedef struct _tagVTBEXTENSION
{
    NvU8 tag;                              // 0x00
    NvU8 revision;                         // 0x01
    NvU8 num_detailed;                     // 0x02
    NvU8 num_cvt;                          // 0x03
    NvU8 num_standard;                     // 0x04
    NvU8 data[NVT_VTB_MAX_PAYLOAD];        // 0x05 - 0x7E
    NvU8 checksum;
}VTBEXTENSION;

// EDID DisplayID extension block template
typedef struct _tagDIDEXTENSION
{
    NvU8   tag;                            // 0x00
    NvU8   struct_version;                 // 0x01
    NvU8   length;                         // 0x02
    NvU8   use_case;                       // 0x03
    NvU8   ext_count;                      // 0x04
    NvU8   data[NVT_DID_MAX_EXT_PAYLOAD];  // 0x05 - 0x7E
    NvU8   checksum;                       // 0x7F
}DIDEXTENSION;

// video signal interface mask
#define NVT_PVT_EDID_INPUT_ISDIGITAL_MASK       0x80 // 0==analog
#define NVT_PVT_EDID_INPUT_ISDIGITAL_SHIFT      7
#define NVT_PVT_EDID_INPUT_ANALOG_ETC_MASK      0x7F
#define NVT_PVT_EDID_INPUT_ANALOG_ETC_SHIFT     0

#define NVT_PVT_EDID_INPUT_INTERFACE_MASK       0x0F
#define NVT_PVT_EDID_INPUT_INTERFACE_SHIFT      0

#define NVT_PVT_EDID_INPUT_BPC_MASK             0x70
#define NVT_PVT_EDID_INPUT_BPC_SHIFT            4
#define NVT_PVT_EDID_INPUT_BPC_UNDEF            0x00
#define NVT_PVT_EDID_INPUT_BPC_6                0x01
#define NVT_PVT_EDID_INPUT_BPC_8                0x02
#define NVT_PVT_EDID_INPUT_BPC_10               0x03
#define NVT_PVT_EDID_INPUT_BPC_12               0x04
#define NVT_PVT_EDID_INPUT_BPC_14               0x05
#define NVT_PVT_EDID_INPUT_BPC_16               0x06

// color characteristic
#define NVT_PVT_EDID_CC_RED_X1_X0_MASK            0xC0
#define NVT_PVT_EDID_CC_RED_X1_X0_SHIFT           6
#define NVT_PVT_EDID_CC_RED_Y1_Y0_MASK            0x30
#define NVT_PVT_EDID_CC_RED_Y1_Y0_SHIFT           4

#define NVT_PVT_EDID_CC_GREEN_X1_X0_MASK          0x0C
#define NVT_PVT_EDID_CC_GREEN_X1_X0_SHIFT         2
#define NVT_PVT_EDID_CC_GREEN_Y1_Y0_MASK          0x03
#define NVT_PVT_EDID_CC_GREEN_Y1_Y0_SHIFT         0

#define NVT_PVT_EDID_CC_BLUE_X1_X0_MASK           0xC0
#define NVT_PVT_EDID_CC_BLUE_X1_X0_SHIFT          6
#define NVT_PVT_EDID_CC_BLUE_Y1_Y0_MASK           0x30
#define NVT_PVT_EDID_CC_BLUE_Y1_Y0_SHIFT          4

#define NVT_PVT_EDID_CC_WHITE_X1_X0_MASK          0x0C
#define NVT_PVT_EDID_CC_WHITE_X1_X0_SHIFT         2
#define NVT_PVT_EDID_CC_WHITE_Y1_Y0_MASK          0x03
#define NVT_PVT_EDID_CC_WHITE_Y1_Y0_SHIFT         0

#endif // __EDID_H_
