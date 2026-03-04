/*
 * SPDX-FileCopyrightText: Copyright (c) 1993-2016 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

/*
===============================================================================

    dp_sdp.cpp

    Provide definition needed for display port secondary data packet.

================================================================================
*/

#ifndef __DPSDP_H__
#define __DPSDP_H__

#include "nvtypes.h"

#define DP_SDP_HEADER_SIZE 4
#define DP_SDP_DATA_SIZE 28

// TODO: needs to wait for RM to provide the enum. Therefore, hardcoded to 7, which is the packet type for VSC SDP
typedef enum tagSDP_PACKET_TYPE
{
    SDP_PACKET_TYPE_VSC = 7,
} SDP_PACKET_TYPE;

typedef enum tagSDP_VSC_REVNUM
{
    SDP_VSC_REVNUM_DISABLED          = 0,
    SDP_VSC_REVNUM_STEREO            = 1,
    SDP_VSC_REVNUM_STEREO_PSR,
    SDP_VSC_REVNUM_STEREO_PSR2,
    SDP_VSC_REVNUM_PSR2_EXTN,
    SDP_VSC_REVNUM_STEREO_PSR2_COLOR,
    SDP_VSC_REVNUM_STEREO_PR,
    SDP_VSC_REVNUM_STEREO_PR_COLOR,
} SDP_VSC_REVNUM;

typedef enum tagSDP_VSC_VALID_DATA_BYTES
{
    SDP_VSC_VALID_DATA_BYTES_DISABLED          = 0,
    SDP_VSC_VALID_DATA_BYTES_STEREO            = 1,
    SDP_VSC_VALID_DATA_BYTES_STEREO_PSR        = 8,
    SDP_VSC_VALID_DATA_BYTES_PSR2              = 12,
    SDP_VSC_VALID_DATA_BYTES_PSR2_COLOR        = 19,
    SDP_VSC_VALID_DATA_BYTES_PR                = 16,
    SDP_VSC_VALID_DATA_BYTES_PR_COLOR          = 19,
} SDP_VSC_VALID_DATA_BYTES;

typedef enum tagSDP_VSC_DYNAMIC_RANGE
{
    SDP_VSC_DYNAMIC_RANGE_VESA,
    SDP_VSC_DYNAMIC_RANGE_CEA,
} SDP_VSC_DYNAMIC_RANGE;

typedef enum tagSDP_VSC_PIX_ENC
{
    SDP_VSC_PIX_ENC_RGB,
    SDP_VSC_PIX_ENC_YCBCR444,
    SDP_VSC_PIX_ENC_YCBCR422,
    SDP_VSC_PIX_ENC_YCBCR420,
    SDP_VSC_PIX_ENC_Y,
    SDP_VSC_PIX_ENC_RAW,
} SDP_VSC_PIX_ENC;

typedef enum tagSDP_VSC_BIT_DEPTH_RGB
{
    SDP_VSC_BIT_DEPTH_RGB_6BPC = 0,
    SDP_VSC_BIT_DEPTH_RGB_8BPC,
    SDP_VSC_BIT_DEPTH_RGB_10BPC,
    SDP_VSC_BIT_DEPTH_RGB_12BPC,
    SDP_VSC_BIT_DEPTH_RGB_16BPC,

} SDP_VSC_BIT_DEPTH_RGB;

typedef enum tagSDP_VSC_BIT_DEPTH_YCBCR
{
    SDP_VSC_BIT_DEPTH_YCBCR_8BPC = 1,
    SDP_VSC_BIT_DEPTH_YCBCR_10BPC,
    SDP_VSC_BIT_DEPTH_YCBCR_12BPC,
    SDP_VSC_BIT_DEPTH_YCBCR_16BPC,

} SDP_VSC_BIT_DEPTH_YCBCR;

typedef enum tagSDP_VSC_BIT_DEPTH_RAW
{
    SDP_VSC_BIT_DEPTH_RAW_6BPC = 1,
    SDP_VSC_BIT_DEPTH_RAW_7BPC,
    SDP_VSC_BIT_DEPTH_RAW_8BPC,
    SDP_VSC_BIT_DEPTH_RAW_10BPC,
    SDP_VSC_BIT_DEPTH_RAW_12BPC,
    SDP_VSC_BIT_DEPTH_RAW_14PC,
    SDP_VSC_BIT_DEPTH_RAW_16PC,

} SDP_VSC_BIT_DEPTH_RAW;

typedef enum tagSDP_VSC_CONTENT_TYPE
{
    SDP_VSC_CONTENT_TYPE_UNDEFINED = 0,
    SDP_VSC_CONTENT_TYPE_GRAPHICS,
    SDP_VSC_CONTENT_TYPE_PHOTO,
    SDP_VSC_CONTENT_TYPE_VIDEO,
    SDP_VSC_CONTENT_TYPE_GAMES,

} SDP_VSC_CONTENT_TYPE;

typedef enum tagSDP_VSC_COLOR_FMT_RGB_COLORIMETRY
{
    SDP_VSC_COLOR_FMT_RGB_COLORIMETRY_SRGB = 0,
    SDP_VSC_COLOR_FMT_RGB_COLORIMETRY_RGB_WIDE_GAMUT_FIXED,
    SDP_VSC_COLOR_FMT_RGB_COLORIMETRY_RGB_SCRGB,
    SDP_VSC_COLOR_FMT_RGB_COLORIMETRY_ADOBERGB,
    SDP_VSC_COLOR_FMT_RGB_COLORIMETRY_DCI_P3,
    SDP_VSC_COLOR_FMT_RGB_COLORIMETRY_CUSTOM,
    SDP_VSC_COLOR_FMT_RGB_COLORIMETRY_ITU_R_BT2020_RGB,
} SDP_VSC_COLOR_FMT_RGB_COLORIMETRY;

typedef enum tagSDP_VSC_COLOR_FMT_YCBCR_COLORIMETRY
{
    SDP_VSC_COLOR_FMT_YCBCR_COLORIMETRY_ITU_R_BT601 = 0,
    SDP_VSC_COLOR_FMT_YCBCR_COLORIMETRY_ITU_R_BT709,
    SDP_VSC_COLOR_FMT_YCBCR_COLORIMETRY_XVYCC601,
    SDP_VSC_COLOR_FMT_YCBCR_COLORIMETRY_XVYCC709,
    SDP_VSC_COLOR_FMT_YCBCR_COLORIMETRY_SYCC601,
    SDP_VSC_COLOR_FMT_YCBCR_COLORIMETRY_ADOBEYCC601,
    SDP_VSC_COLOR_FMT_YCBCR_COLORIMETRY_ITU_R_BT2020_YCCBCCRC,
    SDP_VSC_COLOR_FMT_YCBCR_COLORIMETRY_ITU_R_BT2020_YCBCR,
} SDP_VSC_COLOR_FMT_YCBCR_COLORIMETRY;

typedef enum tagSDP_VSC_COLOR_FMT_RAW_COLORIMETRY
{
    SDP_VSC_COLOR_FMT_RAW_COLORIMETRY_CUSTOM_COLOR_PROFILE = 0,
} SDP_VSC_COLOR_FMT_RAW;

typedef enum tagSDP_VSC_COLOR_FMT_Y_COLORIMETRY
{
    SDP_VSC_COLOR_FMT_Y_COLORIMETRY_DICOM = 0,
} SDP_VSC_COLOR_FMT_Y;

// The struct element field hb and db fields are arranged to match the HW registers
// NV_PDISP_SF_DP_GENERIC_INFOFRAME_HEADER* and NV_PDISP_SF_DP_GENERIC_INFOFRAME_SUBPACK0_DB*
typedef struct tagDPSDP_DP_VSC_SDP_DESCRIPTOR
{
    NvU8 dataSize; // the db data size

    // header
    struct
    {
        NvU8 hb0;                        // DP1.3 spec, the value = 0
        NvU8 hb1;                        // DP1.3 spec, value = 7
        NvU8 revisionNumber         : 5;
        NvU8 hb2Reserved            : 3;
        NvU8 numValidDataBytes      : 5; // number of valid data bytes
        NvU8 hb3Reserved            : 3;
    } hb;

    // data content
    struct
    {
        // Stereo field. Note: Needs to be expanded when needed. Refer to DP1.3 spec.
        NvU8 stereoInterface; // DB0
        // PSR Field. Note: Needs to be expanded when needed. Refer to DP1.3 spec.
        NvU8 psrState : 1;            //DB1
        NvU8 psrUpdateRfb : 1;
        NvU8 psrCrcValid : 1;
        NvU8 psrSuValid : 1;
        NvU8 psrSuFirstScanLine : 1;
        NvU8 psrSuLastScanLine : 1;
        NvU8 psrYCoordinateValid : 1;
        NvU8 psrReserved : 1;
        NvU8 db2;
        NvU8 db3;
        NvU8 db4;
        NvU8 db5;
        NvU8 db6;
        NvU8 db7;
        // DB8 - DB15 are undefined in DP 1.3 spec.
        NvU8 db8;
        NvU8 db9;
        NvU8 db10;
        NvU8 db11;
        NvU8 db12;
        NvU8 db13;
        NvU8 db14;
        NvU8 db15;

        // Colorimetry Infoframe Secondary Data Package following DP1.3 spec
        NvU8 colorimetryFormat      : 4; // DB16 infoframe per DP1.3 spec
        NvU8 pixEncoding            : 4; // DB16 infoframe per DP1.3 spec

        NvU8 bitDepth               : 7; // DB17 infoframe per DP1.3 spec
        NvU8 dynamicRange           : 1; // DB17 infoframe per DP1.3 spec

        NvU8 contentType            : 3; // DB18 infoframe per DP1.3 spec
        NvU8 db18Reserved           : 5;

        NvU8 db19;
        NvU8 db20;
        NvU8 db21;
        NvU8 db22;
        NvU8 db23;
        NvU8 db24;
        NvU8 db25;
        NvU8 db26;
        NvU8 db27;
    } db;

} DPSDP_DP_VSC_SDP_DESCRIPTOR;

typedef struct tagDPSDP_DP_PR_VSC_SDP_DESCRIPTOR
{
    NvU8 dataSize; // the db data size

    // header
    struct
    {
        NvU8 hb0;                        // DP1.3 spec, the value = 0
        NvU8 hb1;                        // DP1.3 spec, value = 7
        NvU8 revisionNumber         : 5;
        NvU8 hb2Reserved            : 3;
        NvU8 numValidDataBytes      : 5; // number of valid data bytes
        NvU8 hb3Reserved            : 3;
    } hb;

    // data content
    struct
    {
        // Stereo field. Note: Needs to be expanded when needed. Refer to DP1.3 spec.
        NvU8 stereoInterface;                       // DB0
        // PSR Field. Note: Needs to be expanded when needed. Refer to DP1.3 spec.
        NvU8 prState                : 1;            // DB1
        NvU8 prReserved             : 1;            // Always ZERO
        NvU8 prCrcValid             : 1;
        NvU8 prSuValid              : 1;
        NvU8 prReservedEx           : 4;

        NvU8 db2;
        NvU8 db3;
        NvU8 db4;
        NvU8 db5;
        NvU8 db6;
        NvU8 db7;
        // DB8 - DB15 are undefined in DP 1.3 spec.
        NvU8 db8;
        NvU8 db9;
        NvU8 db10;
        NvU8 db11;
        NvU8 db12;
        NvU8 db13;
        NvU8 db14;
        NvU8 db15;

        // Colorimetry Infoframe Secondary Data Package following DP1.3 spec
        NvU8 colorimetryFormat      : 4; // DB16 infoframe per DP1.3 spec
        NvU8 pixEncoding            : 4; // DB16 infoframe per DP1.3 spec

        NvU8 bitDepth               : 7; // DB17 infoframe per DP1.3 spec
        NvU8 dynamicRange           : 1; // DB17 infoframe per DP1.3 spec

        NvU8 contentType            : 3; // DB18 infoframe per DP1.3 spec
        NvU8 db18Reserved           : 5;

        NvU8 db19;
        NvU8 db20;
        NvU8 db21;
        NvU8 db22;
        NvU8 db23;
        NvU8 db24;
        NvU8 db25;
        NvU8 db26;
        NvU8 db27;
    } db;

} DPSDP_DP_PR_VSC_SDP_DESCRIPTOR;

typedef struct tagDPSDP_DESCRIPTOR
{
    NvU8 dataSize;

    // header byte
    struct
    {
        NvU8 hb0;
        NvU8 hb1;
        NvU8 hb2;
        NvU8 hb3;
    } hb;

    // content byte
    struct
    {
        NvU8 db0;
        NvU8 db1;
        NvU8 db2;
        NvU8 db3;
        NvU8 db4;
        NvU8 db5;
        NvU8 db6;
        NvU8 db7;
        NvU8 db8;
        NvU8 db9;
        NvU8 db10;
        NvU8 db11;
        NvU8 db12;
        NvU8 db13;
        NvU8 db14;
        NvU8 db15;
        NvU8 db16;
        NvU8 db17;
        NvU8 db18;
        NvU8 db19;
        NvU8 db20;
        NvU8 db21;
        NvU8 db22;
        NvU8 db23;
        NvU8 db24;
        NvU8 db25;
        NvU8 db26;
        NvU8 db27;
        NvU8 db28;
        NvU8 db29;
        NvU8 db30;
        NvU8 db31;
    } db;

} DPSDP_DESCRIPTOR;

// The following #defines are for RGB only
#define DP_VSC_SDP_BIT_DEPTH_RGB_6BPC                                0
#define DP_VSC_SDP_BIT_DEPTH_RGB_8BPC                                1
#define DP_VSC_SDP_BIT_DEPTH_RGB_10BPC                               2
#define DP_VSC_SDP_BIT_DEPTH_RGB_12BPC                               3
#define DP_VSC_SDP_BIT_DEPTH_RGB_16BPC                               4

// The following #defines are for YUV only
#define DP_VSC_SDP_BIT_DEPTH_YUV_8BPC                                1
#define DP_VSC_SDP_BIT_DEPTH_YUV_10BPC                               2
#define DP_VSC_SDP_BIT_DEPTH_YUV_12BPC                               3
#define DP_VSC_SDP_BIT_DEPTH_YUV_16BPC                               4

// The following #defines are for RAW only
#define DP_VSC_SDP_BIT_DEPTH_RAW_6BPC                                1
#define DP_VSC_SDP_BIT_DEPTH_RAW_7BPC                                2
#define DP_VSC_SDP_BIT_DEPTH_RAW_8BPC                                3
#define DP_VSC_SDP_BIT_DEPTH_RAW_10BPC                               4
#define DP_VSC_SDP_BIT_DEPTH_RAW_12BPC                               5
#define DP_VSC_SDP_BIT_DEPTH_RAW_14BPC                               6
#define DP_VSC_SDP_BIT_DEPTH_RAW_16BPC                               7

#define DP_INFOFRAME_SDP_V1_3_VERSION                                0x13
#define DP_INFOFRAME_SDP_V1_3_HB3_VERSION_MASK                       0xFC
#define DP_INFOFRAME_SDP_V1_3_HB3_VERSION_SHIFT                      2
#define DP_INFOFRAME_SDP_V1_3_HB3_MSB_MASK                           0x3
#define DP_INFOFRAME_SDP_V1_3_NON_AUDIO_SIZE                         30
#endif // __DPSDP_H_
