//****************************************************************************
//
//  SPDX-FileCopyrightText: Copyright (c) 2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
//  File:       nvtiming.h
//
//  Purpose:    This file is the common header all nv timing library clients.
//
//*****************************************************************************

#ifndef __NVTIMING_H__
#define __NVTIMING_H__

#include "nvtypes.h"


#define abs_delta(a,b) ((a)>(b)?((a)-(b)):((b)-(a)))

//***********************
// The Timing Structure
//***********************
//
// Nvidia specific timing extras
typedef struct tagNVT_HDMIEXT
{
    // in the case of stereo, the NVT_TIMING structure will hold the 2D
    // instance of the timing parameters, and the stereo extension will
    // contain the variants required to produce the stereo frame.
    NvU8                    StereoStructureType;
    NvU8                    SideBySideHalfDetail;
    NvU16                   VActiveSpace[2];
} NVT_HDMIEXT;
#define NVT_HDMI_3D_SUPPORTED_STRUCT_MASK(x)        (1 << (x))
#define NVT_HDMI_3D_SUPPORTED_FRAMEPACK_MASK        NVT_HDMI_3D_SUPPORTED_STRUCT_MASK(NVT_HDMI_VS_BYTE5_HDMI_3DS_FRAMEPACK)
#define NVT_HDMI_3D_SUPPORTED_FIELD_ALT_MASK        NVT_HDMI_3D_SUPPORTED_STRUCT_MASK(NVT_HDMI_VS_BYTE5_HDMI_3DS_FIELD_ALT)
#define NVT_HDMI_3D_SUPPORTED_LINE_ALT_MASK         NVT_HDMI_3D_SUPPORTED_STRUCT_MASK(NVT_HDMI_VS_BYTE5_HDMI_3DS_LINE_ALT)
#define NVT_HDMI_3D_SUPPORTED_SIDEBYSIDEFULL_MASK   NVT_HDMI_3D_SUPPORTED_STRUCT_MASK(NVT_HDMI_VS_BYTE5_HDMI_3DS_SIDEBYSIDEFULL)
#define NVT_HDMI_3D_SUPPORTED_LDEPTH_MASK           NVT_HDMI_3D_SUPPORTED_STRUCT_MASK(NVT_HDMI_VS_BYTE5_HDMI_3DS_LDEPTH)
#define NVT_HDMI_3D_SUPPORTED_LDEPTHGFX_MASK        NVT_HDMI_3D_SUPPORTED_STRUCT_MASK(NVT_HDMI_VS_BYTE5_HDMI_3DS_LDEPTHGFX)
#define NVT_HDMI_3D_SUPPORTED_TOPBOTTOM_MASK        NVT_HDMI_3D_SUPPORTED_STRUCT_MASK(NVT_HDMI_VS_BYTE5_HDMI_3DS_TOPBOTTOM)
#define NVT_HDMI_3D_SUPPORTED_SIDEBYSIDEHALF_MASK   NVT_HDMI_3D_SUPPORTED_STRUCT_MASK(NVT_HDMI_VS_BYTE5_HDMI_3DS_SIDEBYSIDEHALF)
#define NVT_ALL_HDMI_3D_STRUCT_SUPPORTED_MASK      (NVT_HDMI_3D_SUPPORTED_FRAMEPACK_MASK | NVT_HDMI_3D_SUPPORTED_TOPBOTTOM_MASK | NVT_HDMI_3D_SUPPORTED_SIDEBYSIDEHALF_MASK)

typedef union tagNVT_COLORDEPTH
{
    NvU8 bpcs;
    struct
    {
        NvU8    bpc6   : 1;
        NvU8    bpc8   : 1;
        NvU8    bpc10  : 1;
        NvU8    bpc12  : 1;
        NvU8    bpc14  : 1;
        NvU8    bpc16  : 1;
        NvU8    rsrvd1 : 1;    // must be 0
        NvU8    rsrvd2 : 1;    // must be 0
    } bpc;
} NVT_COLORDEPTH;

#define IS_BPC_SUPPORTED_COLORFORMAT(colorDepth) (!!((NvU8)(colorDepth)))
#define UPDATE_BPC_FOR_COLORFORMAT(colorFormat, b6bpc, b8bpc, b10bpc, b12bpc, b14bpc, b16bpc) \
                            if ((b6bpc))  ((colorFormat).bpc.bpc6 = 1); \
                            if ((b8bpc))  ((colorFormat).bpc.bpc8 = 1); \
                            if ((b10bpc)) ((colorFormat).bpc.bpc10 = 1); \
                            if ((b12bpc)) ((colorFormat).bpc.bpc12 = 1); \
                            if ((b14bpc)) ((colorFormat).bpc.bpc14 = 1); \
                            if ((b16bpc)) ((colorFormat).bpc.bpc16 = 1);

#define SET_BPC_FOR_COLORFORMAT(_colorFormat, _bpc) \
                            if ((_bpc) == NVT_EDID_VIDEOSIGNAL_BPC_6) \
                            ((_colorFormat).bpc.bpc6 = 1); \
                            else if ((_bpc) == NVT_EDID_VIDEOSIGNAL_BPC_8) \
                            ((_colorFormat).bpc.bpc8 = 1); \
                            else if ((_bpc) == NVT_EDID_VIDEOSIGNAL_BPC_10) \
                            ((_colorFormat).bpc.bpc10 = 1); \
                            else if ((_bpc) == NVT_EDID_VIDEOSIGNAL_BPC_12) \
                            ((_colorFormat).bpc.bpc12 = 1); \
                            else if ((_bpc) == NVT_EDID_VIDEOSIGNAL_BPC_16) ((_colorFormat).bpc.bpc16 = 1);

#define CLEAR_BPC_FOR_COLORFORMAT(_colorFormat, _bpc) \
                            if ((_bpc) == NVT_EDID_VIDEOSIGNAL_BPC_6) \
                            ((_colorFormat).bpc.bpc6 = 0); \
                            else if ((_bpc) == NVT_EDID_VIDEOSIGNAL_BPC_8) \
                            ((_colorFormat).bpc.bpc8 = 0); \
                            else if ((_bpc) == NVT_EDID_VIDEOSIGNAL_BPC_10) \
                            ((_colorFormat).bpc.bpc10 = 0); \
                            else if ((_bpc) == NVT_EDID_VIDEOSIGNAL_BPC_12) \
                            ((_colorFormat).bpc.bpc12 = 0); \
                            else if ((_bpc) == NVT_EDID_VIDEOSIGNAL_BPC_16) ((_colorFormat).bpc.bpc16 = 0);

#define NVT_COLORDEPTH_HIGHEST_BPC(_colorFormat)                   \
        (_colorFormat).bpc.bpc16 ? NVT_EDID_VIDEOSIGNAL_BPC_16 :   \
        (_colorFormat).bpc.bpc12 ? NVT_EDID_VIDEOSIGNAL_BPC_12 :   \
        (_colorFormat).bpc.bpc10 ? NVT_EDID_VIDEOSIGNAL_BPC_10 :   \
        (_colorFormat).bpc.bpc8 ? NVT_EDID_VIDEOSIGNAL_BPC_8 :     \
        (_colorFormat).bpc.bpc6 ? NVT_EDID_VIDEOSIGNAL_BPC_6 : NVT_EDID_VIDEOSIGNAL_BPC_NOT_DEFINED

#define NVT_COLORDEPTH_LOWEREST_BPC(_colorFormat)                   \
        (_colorFormat).bpc.bpc6 ? NVT_EDID_VIDEOSIGNAL_BPC_6 :      \
        (_colorFormat).bpc.bpc8 ? NVT_EDID_VIDEOSIGNAL_BPC_8 :      \
        (_colorFormat).bpc.bpc10 ? NVT_EDID_VIDEOSIGNAL_BPC_10 :    \
        (_colorFormat).bpc.bpc12 ? NVT_EDID_VIDEOSIGNAL_BPC_12 :    \
        (_colorFormat).bpc.bpc16 ? NVT_EDID_VIDEOSIGNAL_BPC_16 : NVT_EDID_VIDEOSIGNAL_BPC_NOT_DEFINED

typedef struct tagNVT_TIMINGEXT
{
    NvU32            flag;          // reserve for NV h/w based enhancement like double-scan.
    NvU16            rr;            // the logical refresh rate to present
    NvU32            rrx1k;         // the physical vertical refresh rate in 0.001Hz
    NvU32            aspect;        // the display aspect ratio Hi(aspect):horizontal-aspect, Low(aspect):vertical-aspect
    //
    // Bitmask of one-hot encoded possible pixel repetitions:
    //   0x1: no pixel repetition (i.e., display each pixel once)
    //   0x2: each pixel is displayed twice horizontally;
    //   0x3: use either no pixel repetition or display each pixel twice
    //   ...
    //
    NvU16            rep;
    NVT_COLORDEPTH   rgb444;        // each bit within is set if rgb444 supported on that bpc
    NVT_COLORDEPTH   yuv444;        // each bit within is set if yuv444 supported on that bpc
    NVT_COLORDEPTH   yuv422;        // each bit within is set if yuv422 supported on that bpc
    NVT_COLORDEPTH   yuv420;        // each bit within is set if yuv420 supported on that bpc
    NvU32   status;        // the timing standard being used
    NvU8    name[51];      // the name of the timing
}NVT_TIMINGEXT;
//
//
//The very basic timing structure based on the VESA standard:
//
//           |<----------------------------htotal--------------------------->|
//            ---------"active" video-------->|<-------blanking------>|<-----
//           |<-------hvisible-------->|<-hb->|<-hfp->|<-hsw->|<-hbp->|<-hb->|
// ----------+-------------------------+      |       |       |       |      |
//   A     A |                         |      |       |       |       |      |
//   :     : |                         |      |       |       |       |      |
//   :     : |                         |      |       |       |       |      |
//   :verical|    addressable video    |      |       |       |       |      |
//   :visible|                         |      |       |       |       |      |
//   :     : |                         |      |       |       |       |      |
//   :     : |                         |      |       |       |       |      |
// verical V |                         |      |       |       |       |      |
//  total  --+-------------------------+      |       |       |       |      |
//   :     vb         border                  |       |       |       |      |
//   :     -----------------------------------+       |       |       |      |
//   :     vfp        front porch                     |       |       |      |
//   :     -------------------------------------------+       |       |      |
//   :     vsw        sync width                              |       |      |
//   :     ---------------------------------------------------+       |      |
//   :     vbp        back porch                                      |      |
//   :     -----------------------------------------------------------+      |
//   V     vb         border                                                 |
// --------------------------------------------------------------------------+
//
typedef struct tagNVT_TIMING
{
    // VESA scan out timing parameters:
    NvU16 HVisible;         //horizontal visible
    NvU16 HBorder;          //horizontal border
    NvU16 HFrontPorch;      //horizontal front porch
    NvU16 HSyncWidth;       //horizontal sync width
    NvU16 HTotal;           //horizontal total
    NvU8  HSyncPol;         //horizontal sync polarity: 1-negative, 0-positive

    NvU16 VVisible;         //vertical visible
    NvU16 VBorder;          //vertical border
    NvU16 VFrontPorch;      //vertical front porch
    NvU16 VSyncWidth;       //vertical sync width
    NvU16 VTotal;           //vertical total
    NvU8  VSyncPol;         //vertical sync polarity: 1-negative, 0-positive

    NvU16 interlaced;       //1-interlaced, 0-progressive
    NvU32 pclk;             //pixel clock in 10KHz
    NvU32 pclk1khz;         //pixel clock in 1kHz for Type7, CVT RB2, CVT RB3

    //other timing related extras
    NVT_TIMINGEXT etc;
}NVT_TIMING;

#define NVT_MAX_TOTAL_TIMING                  128

//
// The below VSync/HSync Polarity definition have been inverted to match
// HW Display Class definition.
// timing related constants:
#define NVT_H_SYNC_POSITIVE                   0
#define NVT_H_SYNC_NEGATIVE                   1
#define NVT_H_SYNC_DEFAULT                    NVT_H_SYNC_NEGATIVE
//
#define NVT_V_SYNC_POSITIVE                   0
#define NVT_V_SYNC_NEGATIVE                   1
#define NVT_V_SYNC_DEFAULT                    NVT_V_SYNC_POSITIVE
//
#define NVT_PROGRESSIVE                                         0
#define NVT_INTERLACED                                          1
#define NVT_INTERLACED_EXTRA_VBLANK_ON_FIELD2                   1
#define NVT_INTERLACED_NO_EXTRA_VBLANK_ON_FIELD2                2

// timing related macros:
#define NVT_FRAME_HEIGHT(_vvisible_, _interlaced_) ((_vvisible_) * ((_interlaced_ != 0) ? 2 : 1))

//*************************************
// The Timing Status encoded in
// NVT_TIMING::NVT_TIMINGEXT::status
//*************************************
//
// TIMING_STATUS has the following kinds of info:
//
// NVT_TIMING::NVT_TIMINGEXT::status
//
//     +----+----+---------+----+----+------------------------------+---+---------------+---+----------------+
//  bit31 bit30 bit29    bit22 bit21 bit20                        bit16 bit15          bit8 bit7             bit0
// |native|cust|<-cta format->|Dual|<--------mismatch status-------->|<---timing type--->|<---timing seq#--->|
//
// 1. the monitor preferred timing flag and cust EDID entry flag
//
#define NVT_STATUS_TIMING_NATIVE_FLAG_MASK    0x80000000
#define NVT_STATUS_TIMING_NATIVE_FLAG_SHIFT   31
#define NVT_IS_NATIVE_TIMING(n)               (((n)&NVT_STATUS_TIMING_NATIVE_FLAG_MASK)>>NVT_STATUS_TIMING_NATIVE_FLAG_SHIFT)
#define NVT_SET_NATIVE_TIMING_FLAG(n)         ((n)|=1U<< NVT_STATUS_TIMING_NATIVE_FLAG_SHIFT)
#define NVT_PREFERRED_TIMING_MODE_MASK        0x2
//
#define NVT_STATUS_TIMING_CUST_ENTRY_MASK     0x40000000
#define NVT_STATUS_TIMING_CUST_ENTRY_SHIFT    30
#define NVT_IS_CUST_ENTRY(n)                  (((n)&NVT_STATUS_TIMING_CUST_ENTRY_MASK)>>NVT_STATUS_TIMING_CUST_ENTRY_SHIFT)
#define NVT_SET_CUST_ENTRY_FLAG(n)            ((n)|=1<<NVT_STATUS_TIMING_CUST_ENTRY_SHIFT)
//
#define NVT_STATUS_TIMING_CEA_FORMAT_MASK     0x3FC00000
#define NVT_STATUS_TIMING_CEA_FORMAT_SHIFT    22
#define NVT_GET_CEA_FORMAT(n)                 (((n)&NVT_STATUS_TIMING_CEA_FORMAT_MASK)>>NVT_STATUS_TIMING_CEA_FORMAT_SHIFT)
#define NVT_SET_CEA_FORMAT(n,index)           {(n)&=~NVT_STATUS_TIMING_CEA_FORMAT_MASK;(n)|=(index)<<NVT_STATUS_TIMING_CEA_FORMAT_SHIFT;}
//
//
// 2. CEA/DMT dual standard flag
//
#define NVT_STATUS_TIMING_CEA_DMT_MASK        0x00200000
#define NVT_STATUS_TIMING_CEA_DMT_SHIFT       21
#define NVT_IS_CEA_DMT_DUAL_STANDARD(n)       (((n)&NVT_STATUS_TIMING_CEA_DMT_MASK)>>NVT_STATUS_TIMING_CEA_DMT_SHIFT)
#define NVT_SET_CEA_DMT_DUAL_STANDARD_FLAG(n) ((n)|=NVT_STATUS_TIMING_CEA_DMT_MASK)
//
//
// 3. the mismatch status
#define NVT_STATUS_TIMING_MISMATCH_MASK       0x001F0000
#define NVT_STATUS_TIMING_MISMATCH_SHIFT      16
#define NVT_STATUS_TIMING_MISMATCH_SIZE       0x1     //visible width and height don't match with the asked width/height
#define NVT_STATUS_TIMING_MISMATCH_RR         0x2     //the refresh rate doesn't match with the requested
#define NVT_STATUS_TIMING_MISMATCH_FORMAT     0x4     //other timing info doesn't match (i.e. progressive/interlaced, double, reduced-blanking etc...)
#define NVT_STATUS_TIMING_MISMATCH_ALIGNMENT  0x8     //the asking alignment doesn't match the spec
//
// macroes to set/get the timing mismatch status
#define NVT_SET_TIMING_STATUS_MISMATCH(m,n)   ((m)|=(((n)<<NVT_STATUS_TIMING_MISMATCH_SHIFT)&NVT_STATUS_TIMING_MISMATCH_MASK))
#define NVT_GET_TIMING_STATUS_MATCH(n)        (((n)&NVT_STATUS_TIMING_MISMATCH_MASK)>>NVT_STATUS_TIMING_MISMATCH_SHIFT)
//
//
// 4. the timing type
//
#define NVT_STATUS_TIMING_TYPE_MASK           0x0000FF00
#define NVT_STATUS_TIMING_TYPE_SHIFT          8
//
typedef enum NVT_TIMING_TYPE
{
    NVT_TYPE_DMT = 1,                                 // DMT
    NVT_TYPE_GTF,                                     // GTF
    NVT_TYPE_ASPR,                                    // wide aspect ratio timing, for legacy support only
    NVT_TYPE_NTSC_TV,                                 // NTSC TV timing. for legacy support only
    NVT_TYPE_PAL_TV,                                  // PAL TV timing, legacy support only
    NVT_TYPE_CVT,                                     // CVT timing
    NVT_TYPE_CVT_RB,                                  // CVT timing with reduced blanking
    NVT_TYPE_CUST,                                    // Customized timing
    NVT_TYPE_EDID_DTD,                                // EDID detailed timing
    NVT_TYPE_EDID_STD,                                // = 10 EDID standard timing
    NVT_TYPE_EDID_EST,                                // EDID established timing
    NVT_TYPE_EDID_CVT,                                // EDID defined CVT timing (EDID 1.4)
    NVT_TYPE_EDID_861ST,                              // EDID defined CEA/EIA 861 timing (in the CTA861 extension)
    NVT_TYPE_NV_PREDEFINED,                           // NV pre-defined timings (PsF timings)
    NVT_TYPE_DMT_RB,                                  // DMT timing with reduced blanking
    NVT_TYPE_EDID_EXT_DTD,                            // EDID detailed timing in the extension
    NVT_TYPE_SDTV,                                    // SDTV timing (including NTSC, PAL etc)
    NVT_TYPE_HDTV,                                    // HDTV timing (480p,480i,720p, 1080i etc)
    NVT_TYPE_SMPTE,                                   // deprecated ? still used by drivers\unix\nvkms\src\nvkms-dpy.c
    NVT_TYPE_EDID_VTB_EXT,                            // = 20 EDID defined VTB extension timing
    NVT_TYPE_EDID_VTB_EXT_STD,                        // EDID defined VTB extension standard timing
    NVT_TYPE_EDID_VTB_EXT_DTD,                        // EDID defined VTB extension detailed timing
    NVT_TYPE_EDID_VTB_EXT_CVT,                        // EDID defined VTB extension cvt timing
    NVT_TYPE_HDMI_STEREO,                             // EDID defined HDMI stereo timing
    NVT_TYPE_DISPLAYID_1,                             // DisplayID Type 1 timing
    NVT_TYPE_DISPLAYID_2,                             // DisplayID Type 2 timing
    NVT_TYPE_HDMI_EXT,                                // EDID defined HDMI extended resolution timing (UHDTV - 4k, 8k etc.)
    NVT_TYPE_CUST_AUTO,                               // Customized timing generated automatically by NVCPL
    NVT_TYPE_CUST_MANUAL,                             // Customized timing entered manually by user
    NVT_TYPE_CVT_RB_2,                                // = 30 CVT timing with reduced blanking V2
    NVT_TYPE_DMT_RB_2,                                // DMT timing with reduced blanking V2
    NVT_TYPE_DISPLAYID_7,                             // DisplayID 2.0 detailed timing - Type VII
    NVT_TYPE_DISPLAYID_8,                             // DisplayID 2.0 enumerated timing - Type VIII
    NVT_TYPE_DISPLAYID_9,                             // DisplayID 2.0 formula-based timing - Type IX
    NVT_TYPE_DISPLAYID_10,                            // DisplayID 2.0 formula-based timing - Type X
    NVT_TYPE_CVT_RB_3,                                // CVT timing with reduced blanking V3
    NVT_TYPE_CTA861_DID_T7,                           // EDID defined CTA861 DisplayID Type VII  timing (in the CTA861 extension)
    NVT_TYPE_CTA861_DID_T8,                           // EDID defined CTA861 DisplayID Type VIII timing (in the CTA861 extension)
    NVT_TYPE_CTA861_DID_T10                           // EDID defined CTA861 DisplayID Type X    timing (in the CTA861 extension)
}NVT_TIMING_TYPE;
//
// 5. the timing sequence number like the TV format and EIA861B predefined timing format
//    **the numbers are chosen to match with the NV h/w format**
//
#define NVT_STATUS_TIMING_SEQ_MASK            0x000000FF
//
typedef enum NVT_TV_FORMAT
{
    NVT_NTSC       = 0,
    NVT_NTSC_M     = 0,
    NVT_NTSC_J     = 1,
    NVT_PAL        = 2,
    NVT_PAL_M      = 2,
    NVT_PAL_A      = 3,
    NVT_PAL_N      = 4,
    NVT_PAL_NC     = 5,
    NVT_HD576I     = 8,
    NVT_HD480I,
    NVT_HD480P,
    NVT_HD576P,
    NVT_HD720P,
    NVT_HD1080I,
    NVT_HD1080P,
    NVT_HD720P50,
    NVT_HD1080P24,
    NVT_HD1080I50,
    NVT_HD1080P50,
    NVT_MAX_TV_FORMAT,
    NVT_AUTO_SDTV_FORMAT = (NvU32)(-2), // Not supported in NvTiming_GetTvTiming
    NVT_AUTO_HDTV_FORMAT = (NvU32)(-1),
}NVT_TV_FORMAT;

#define NVT_DEFAULT_HDTV_FMT NVT_HD1080I
//
// macros to set/get the timing type and seq number
//
#define NVT_DEF_TIMING_STATUS(type, seq)     ((((type)<<NVT_STATUS_TIMING_TYPE_SHIFT)&NVT_STATUS_TIMING_TYPE_MASK) | ((seq)&NVT_STATUS_TIMING_SEQ_MASK))
#define NVT_SET_TIMING_STATUS_TYPE(n, type)  (((n)&(~NVT_STATUS_TIMING_TYPE_MASK)) | ((type)<<NVT_STATUS_TIMING_TYPE_SHIFT))
#define NVT_GET_TIMING_STATUS_TYPE(n)        ((n)&NVT_STATUS_TIMING_TYPE_MASK)>>NVT_STATUS_TIMING_TYPE_SHIFT
#define NVT_GET_TIMING_STATUS_SEQ(n)         ((n)&NVT_STATUS_TIMING_SEQ_MASK)
//
//
//
// the timing type definitions
#define NVT_STATUS_DMT                       NVT_DEF_TIMING_STATUS(NVT_TYPE_DMT, 0)        // DMT
#define NVT_STATUS_GTF                       NVT_DEF_TIMING_STATUS(NVT_TYPE_GTF, 0)        // GTF
#define NVT_STATUS_ASPR                      NVT_DEF_TIMING_STATUS(NVT_TYPE_ASPR, 0)       // ASPR
#define NVT_STATUS_NTSC_TV                   NVT_DEF_TIMING_STATUS(NVT_TYPE_NTSC_TV, 0)    // TVN
#define NVT_STATUS_PAL_TV                    NVT_DEF_TIMING_STATUS(NVT_TYPE_PAL_TV,  0)    // TVP
#define NVT_STATUS_CVT                       NVT_DEF_TIMING_STATUS(NVT_TYPE_CVT, 0)        // CVT timing with regular blanking
#define NVT_STATUS_CVT_RB                    NVT_DEF_TIMING_STATUS(NVT_TYPE_CVT_RB,  0)    // CVT_RB timing V1
#define NVT_STATUS_CVT_RB_2                  NVT_DEF_TIMING_STATUS(NVT_TYPE_CVT_RB_2,  0)  // CVT_RB timing V2
#define NVT_STATUS_CVT_RB_3                  NVT_DEF_TIMING_STATUS(NVT_TYPE_CVT_RB_3,  0)  // CVT_RB timing V3
#define NVT_STATUS_CUST                      NVT_DEF_TIMING_STATUS(NVT_TYPE_CUST,    0)    // Customized timing
#define NVT_STATUS_EDID_DTD                  NVT_DEF_TIMING_STATUS(NVT_TYPE_EDID_DTD, 0)
#define NVT_STATUS_EDID_STD                  NVT_DEF_TIMING_STATUS(NVT_TYPE_EDID_STD, 0)
#define NVT_STATUS_EDID_EST                  NVT_DEF_TIMING_STATUS(NVT_TYPE_EDID_EST, 0)
#define NVT_STATUS_EDID_CVT                  NVT_DEF_TIMING_STATUS(NVT_TYPE_EDID_CVT, 0)
#define NVT_STATUS_EDID_861ST                NVT_DEF_TIMING_STATUS(NVT_TYPE_EDID_861ST, 0)
#define NVT_STATUS_DMT_RB                    NVT_DEF_TIMING_STATUS(NVT_TYPE_DMT_RB, 0)
#define NVT_STATUS_EDID_EXT_DTD              NVT_DEF_TIMING_STATUS(NVT_TYPE_EDID_EXT_DTD, 0)
#define NVT_STATUS_SDTV_NTSC                 NVT_DEF_TIMING_STATUS(NVT_TYPE_SDTV, NVT_NTSC)
#define NVT_STATUS_SDTV_NTSC_M               NVT_DEF_TIMING_STATUS(NVT_TYPE_SDTV, NVT_NTSC)
#define NVT_STATUS_SDTV_NTSC_J               NVT_DEF_TIMING_STATUS(NVT_TYPE_SDTV, NVT_NTSC_J)
#define NVT_STATUS_SDTV_PAL                  NVT_DEF_TIMING_STATUS(NVT_TYPE_SDTV, NVT_PAL)
#define NVT_STATUS_SDTV_PAL_M                NVT_DEF_TIMING_STATUS(NVT_TYPE_SDTV, NVT_PAL)
#define NVT_STATUS_SDTV_PAL_A                NVT_DEF_TIMING_STATUS(NVT_TYPE_SDTV, NVT_PAL_A)
#define NVT_STATUS_SDTV_PAL_N                NVT_DEF_TIMING_STATUS(NVT_TYPE_SDTV, NVT_PAL_N)
#define NVT_STATUS_SDTV_PAL_NC               NVT_DEF_TIMING_STATUS(NVT_TYPE_SDTV, NVT_PAL_NC)
#define NVT_STATUS_HDTV_480I                 NVT_DEF_TIMING_STATUS(NVT_TYPE_HDTV, NVT_HD480I)
#define NVT_STATUS_HDTV_480P                 NVT_DEF_TIMING_STATUS(NVT_TYPE_HDTV, NVT_HD480P)
#define NVT_STATUS_HDTV_576I                 NVT_DEF_TIMING_STATUS(NVT_TYPE_HDTV, NVT_HD576I)
#define NVT_STATUS_HDTV_576P                 NVT_DEF_TIMING_STATUS(NVT_TYPE_HDTV, NVT_HD576P)
#define NVT_STATUS_HDTV_720P                 NVT_DEF_TIMING_STATUS(NVT_TYPE_HDTV, NVT_HD720P)
#define NVT_STATUS_HDTV_1080I                NVT_DEF_TIMING_STATUS(NVT_TYPE_HDTV, NVT_HD1080I)
#define NVT_STATUS_HDTV_1080P                NVT_DEF_TIMING_STATUS(NVT_TYPE_HDTV, NVT_HD1080P)
#define NVT_STATUS_HDTV_720P50               NVT_DEF_TIMING_STATUS(NVT_TYPE_HDTV, NVT_HD720P50)
#define NVT_STATUS_HDTV_1080P24              NVT_DEF_TIMING_STATUS(NVT_TYPE_HDTV, NVT_HD1080P24)
#define NVT_STATUS_HDTV_1080I50              NVT_DEF_TIMING_STATUS(NVT_TYPE_HDTV, NVT_HD1080I50)
#define NVT_STATUS_HDTV_1080P50              NVT_DEF_TIMING_STATUS(NVT_TYPE_HDTV, NVT_HD1080P50)
#define NVT_STATUS_EDID_VTB_EXT              NVT_DEF_TIMING_STATUS(NVT_TYPE_VTB_EXT, 0)
#define NVT_STATUS_EDID_VTB_EXT_DTD          NVT_DEF_TIMING_STATUS(NVT_TYPE_EDID_VTB_EXT_DTD, 0)
#define NVT_STATUS_EDID_VTB_EXT_CVT          NVT_DEF_TIMING_STATUS(NVT_TYPE_EDID_VTB_EXT_CVT, 0)
#define NVT_STATUS_EDID_VTB_EXT_STD          NVT_DEF_TIMING_STATUS(NVT_TYPE_EDID_VTB_EXT_STD, 0)
#define NVT_STATUS_HDMI_STEREO               NVT_DEF_TIMING_STATUS(NVT_TYPE_HDMI_STEREO, 0)
#define NVT_STATUS_DISPLAYID_1               NVT_DEF_TIMING_STATUS(NVT_TYPE_DISPLAYID_1, 0)
#define NVT_STATUS_DISPLAYID_2               NVT_DEF_TIMING_STATUS(NVT_TYPE_DISPLAYID_2, 0)
#define NVT_STATUS_DISPLAYID_7               NVT_DEF_TIMING_STATUS(NVT_TYPE_DISPLAYID_7, 0)
#define NVT_STATUS_DISPLAYID_8               NVT_DEF_TIMING_STATUS(NVT_TYPE_DISPLAYID_8, 0)
#define NVT_STATUS_DISPLAYID_9               NVT_DEF_TIMING_STATUS(NVT_TYPE_DISPLAYID_9, 0)
#define NVT_STATUS_DISPLAYID_10              NVT_DEF_TIMING_STATUS(NVT_TYPE_DISPLAYID_10, 0)
#define NVT_STATUS_HDMI_EXT                  NVT_DEF_TIMING_STATUS(NVT_TYPE_HDMI_EXT, 0)
#define NVT_STATUS_CUST_AUTO                 NVT_DEF_TIMING_STATUS(NVT_TYPE_CUST_AUTO, 0)
#define NVT_STATUS_CUST_MANUAL               NVT_DEF_TIMING_STATUS(NVT_TYPE_CUST_MANUAL, 0)

//
// adding the timing sequence (from the EDID) to the modeset status
#define NVT_STATUS_DTD1                      NVT_DEF_TIMING_STATUS(NVT_TYPE_EDID_DTD, 1)
#define NVT_STATUS_EDID_DTDn(n)              NVT_DEF_TIMING_STATUS(NVT_TYPE_EDID_DTD, n)
#define NVT_STATUS_EDID_STDn(n)              NVT_DEF_TIMING_STATUS(NVT_TYPE_EDID_STD, n)
#define NVT_STATUS_EDID_ESTn(n)              NVT_DEF_TIMING_STATUS(NVT_TYPE_EDID_EST, n)
#define NVT_STATUS_EDID_CVTn(n)              NVT_DEF_TIMING_STATUS(NVT_TYPE_EDID_CVT, n)
#define NVT_STATUS_EDID_861STn(n)            NVT_DEF_TIMING_STATUS(NVT_TYPE_EDID_861ST, n)
#define NVT_STATUS_EDID_EXT_DTDn(n)          NVT_DEF_TIMING_STATUS(NVT_TYPE_EDID_EXT_DTD, n)
#define NVT_STATUS_CUSTn(n)                  NVT_DEF_TIMING_STATUS(NVT_TYPE_CUST, n)
#define NVT_TYPE_NV_PREDEFINEDn(n)           NVT_DEF_TIMING_STATUS(NVT_TYPE_NV_PREDEFINED, n)
#define NVT_STATUS_EDID_VTB_EXTn(n)          NVT_DEF_TIMING_STATUS(NVT_TYPE_EDID_VTB_EXT, n)
#define NVT_STATUS_EDID_VTB_EXT_DTDn(n)      NVT_DEF_TIMING_STATUS(NVT_TYPE_EDID_VTB_EXT_DTD, n)
#define NVT_STATUS_EDID_VTB_EXT_STDn(n)      NVT_DEF_TIMING_STATUS(NVT_TYPE_EDID_VTB_EXT_STD, n)
#define NVT_STATUS_EDID_VTB_EXT_CVTn(n)      NVT_DEF_TIMING_STATUS(NVT_TYPE_EDID_VTB_EXT_CVT, n)
#define NVT_STATUS_HDMI_STEREO_REQn(n)       NVT_DEF_TIMING_STATUS(NVT_TYPE_HDMI_STEREO_REQ, n)
#define NVT_STATUS_DISPLAYID_1N(n)           NVT_DEF_TIMING_STATUS(NVT_TYPE_DISPLAYID_1, n)
#define NVT_STATUS_DISPLAYID_2N(n)           NVT_DEF_TIMING_STATUS(NVT_TYPE_DISPLAYID_2, n)
#define NVT_STATUS_DISPLAYID_7N(n)           NVT_DEF_TIMING_STATUS(NVT_TYPE_DISPLAYID_7, n)
#define NVT_STATUS_DISPLAYID_8N(n)           NVT_DEF_TIMING_STATUS(NVT_TYPE_DISPLAYID_8, n)
#define NVT_STATUS_DISPLAYID_9N(n)           NVT_DEF_TIMING_STATUS(NVT_TYPE_DISPLAYID_9, n)
#define NVT_STATUS_DISPLAYID_10N(n)          NVT_DEF_TIMING_STATUS(NVT_TYPE_DISPLAYID_10, n)
#define NVT_STATUS_HDMI_EXTn(n)              NVT_DEF_TIMING_STATUS(NVT_TYPE_HDMI_EXT, n)
#define NVT_STATUS_CTA861_DID_T7N(n)         NVT_DEF_TIMING_STATUS(NVT_TYPE_CTA861_DID_T7, n)
#define NVT_STATUS_CTA861_DID_T8N(n)         NVT_DEF_TIMING_STATUS(NVT_TYPE_CTA861_DID_T8, n)
#define NVT_STATUS_CTA861_DID_T10N(n)        NVT_DEF_TIMING_STATUS(NVT_TYPE_CTA861_DID_T10, n)
#define NVT_STATUS_CTA861_OVT_Tn(n)          NVT_DEF_TIMING_STATUS(NVT_TYPE_CTA861_OVT, n)

//********************************
// CEA/EIA 861 related EDID info
//********************************
#define NVT_CEA861_REV_NONE                  0
#define NVT_CEA861_REV_ORIGINAL              1
#define NVT_CEA861_REV_A                     2
#define NVT_CEA861_REV_B                     3
#define NVT_CEA861_REV_C                     3
#define NVT_CEA861_REV_D                     3
#define NVT_CEA861_REV_E                     3
#define NVT_CEA861_REV_F                     3
#define NVT_CTA861_REV_G                     3
#define NVT_CTA861_REV_H                     3
//
// max data after misc/basic_caps in EIA861EXTENSION
#define NVT_CEA861_MAX_PAYLOAD               123
//
// the basic info encoded in byte[3]
#define NVT_CEA861_CAP_UNDERSCAN             0x80 // DTV monitor supports underscan
#define NVT_CEA861_CAP_BASIC_AUDIO           0x40 // DTV monitor supports basic audio
#define NVT_CEA861_CAP_YCbCr_444             0x20 // DTV monitor supports YCbCr4:4:4
#define NVT_CEA861_CAP_YCbCr_422             0x10 // DTV monitor supports YCbCr4:2:2
//
#define NVT_CEA861_TOTAL_LT_MASK             0x0F //the max number of 18-byte detailed timing descriptor
//
//
#define NVT_CEA861_SHORT_DESCRIPTOR_SIZE_MASK 0x1F
#define NVT_CEA861_SHORT_DESCRIPTOR_TAG_MASK  0xE0
#define NVT_CEA861_SHORT_DESCRIPTOR_TAG_SHIFT 5
//
// the CTA Tag Codes
#define NVT_CEA861_TAG_RSVD                  0    // reserved block
#define NVT_CEA861_TAG_NONE                  0    // reserved block
#define NVT_CEA861_TAG_AUDIO                 1    // Audio Data Block
#define NVT_CEA861_TAG_VIDEO                 2    // Video Data Block
#define NVT_CEA861_TAG_VENDOR                3    // Vendor Specific Data Block
#define NVT_CEA861_TAG_SPEAKER_ALLOC         4    // Speaker Allocation Data Block
#define NVT_CEA861_TAG_VESA_DTC              5    // VESA DTC data block
#define NVT_CTA861_TAG_VIDEO_FORMAT          6    // Video Format Data Block in CTA861.6
#define NVT_CEA861_TAG_EXTENDED_FLAG         7    // use Extended Tag
//
// the extended tag codes when NVT_CEA861_TAG_EXTENDED_FLAG
#define NVT_CEA861_EXT_TAG_VIDEO_CAP                    0   // Video Capability Data Block
#define NVT_CEA861_EXT_TAG_VENDOR_SPECIFIC_VIDEO        1   // Vendor-Specific Video Data Block
#define NVT_CEA861_EXT_TAG_VESA_VIDEO_DISPLAY_DEVICE    2   // VESA Video Display Device Information Data Block
#define NVT_CEA861_EXT_TAG_VESA_VIDEO                   3   // Reserved for VESA Video Data BLock
#define NVT_CEA861_EXT_TAG_HDMI_VIDEO                   4   // Reserved for HDMI Video Data Block
#define NVT_CEA861_EXT_TAG_COLORIMETRY                  5   // Colorimetry Data Block
#define NVT_CEA861_EXT_TAG_HDR_STATIC_METADATA          6   // HDR Static Metadata Data Block CEA861.3 HDR extension for HDMI 2.0a
#define NVT_CTA861_EXT_TAG_HDR_DYNAMIC_METADATA         7   // CTA861-H HDR Dynamic Metadata Data Block
#define NVT_CTA861_EXT_TAG_NATIVE_VIDEO_RESOLUTION      8   // CTA861.6 Native Video Resolution Data Block
#define NVT_CTA861_EXT_TAG_VIDEO_RSVD_MIN               9   // 9...12  :  Reserved for video-related blocks
#define NVT_CTA861_EXT_TAG_VIDEO_RSVD_MAX              12
#define NVT_CEA861_EXT_TAG_VIDEO_FORMAT_PREFERENCE     13   // CEA861F  Video Format Preference Data Block
#define NVT_CEA861_EXT_TAG_YCBCR420_VIDEO              14   // CEA861F  YCBCR 4:2:0 Video Data Block
#define NVT_CEA861_EXT_TAG_YCBCR420_CAP                15   // CEA861F  YCBCR 4:2:0 Capability Map Data Block
#define NVT_CEA861_EXT_TAG_MISC_AUDIO                  16   // CEA Miscellaneous Audio Fields
#define NVT_CEA861_EXT_TAG_VENDOR_SPECIFIC_AUDIO       17   // Vendor-Specific Audio Data Block
#define NVT_CTA861_EXT_TAG_HDMI_AUDIO                  18   // Reserved for HDMI Audio Data Block
#define NVT_CTA861_EXT_TAG_ROOM_CONFIGURATION          19   // CTA861-H Room Configuration Data Block
#define NVT_CTA861_EXT_TAG_SPEACKER_LOCATION           20   // CTA861-H Speaker Location Data Block
#define NVT_CTA861_EXT_TAG_AUDIO_RSVD_MIN              21   // 21...31  :  Reserved for audio-related blocks
#define NVT_CTA861_EXT_TAG_AUDIO_RSVD_MAX              31
#define NVT_CEA861_EXT_TAG_INFOFRAME                   32   // Infoframe Data Block
#define NVT_CTA861_EXT_TAG_RSVD                        33   // Reserved
#define NVT_CTA861_EXT_TAG_DID_TYPE_VII                34   // DisplayID Type VII Video Timing Data Block
#define NVT_CTA861_EXT_TAG_DID_TYPE_VIII               35   // DisplayID Type VIII Video Timing Data Block
#define NVT_CTA861_EXT_TAG_RSVD_MIN_1                  36   // 36...41 :  Reserved for general
#define NVT_CTA861_EXT_TAG_RSVD_MAX_1                  41
#define NVT_CTA861_EXT_TAG_DID_TYPE_X                  42   // DisplayID Type X Video Timing Data Block
#define NVT_CTA861_EXT_TAG_RSVD_MIN_2                  43   // 43...119 :  Reserved for general
#define NVT_CTA861_EXT_TAG_RSVD_MAX_2                 119
#define NVT_CTA861_EXT_TAG_HF_EEODB                   120   // HDMI Forum Edid Extension Override Data Block
#define NVT_CTA861_EXT_TAG_SCDB                       121   // 0x79 == Tag for Sink Capability Data Block
#define NVT_CTA861_EXT_TAG_HDMI_RSVD_MIN              122   // 122...127 :  Reserved for HDMI
#define NVT_CTA861_EXT_TAG_HDMI_RSVD_MAX              127
#define NVT_CTA861_EXT_TAG_RSVD_MIN_3                 128   // 128...255 :  Reserved for general
#define NVT_CTA861_EXT_TAG_RSVD_MAX_3                 255
//
//the extended tag payload size; the size includes the extended tag code
#define NVT_CEA861_EXT_VIDEO_CAP_SD_SIZE                2
#define NVT_CEA861_EXT_COLORIMETRY_SD_SIZE              3
#define NVT_CTA861_EXT_HDR_STATIC_METADATA_SIZE         6
#define NVT_CTA861_EXT_SCDB_PAYLOAD_MAX_LENGTH          NVT_CEA861_VSDB_PAYLOAD_MAX_LENGTH
//
//
#define NVT_CEA861_GET_SHORT_DESCRIPTOR_TAG(a)  (((a)&NVT_CEA861_SHORT_DESCRIPTOR_TAG_MASK)>>NVT_CEA861_SHORT_DESCRIPTOR_TAG_SHIFT)
#define NVT_CEA861_GET_SHORT_DESCRIPTOR_SIZE(a) ((NvU32)((a)&NVT_CEA861_SHORT_DESCRIPTOR_SIZE_MASK))


//********************************
// VTB Extension related info
//********************************

#define NVT_VTB_REV_NONE 0
#define NVT_VTB_REV_A    1

#define NVT_VTB_MAX_PAYLOAD 122

//*************************
// short descriptor
//*************************
#define NVT_CEA861_SD_HEADER_SIZE            1
#define NVT_CEA861_SD_PAYLOAD_SIZE           31
#define NVT_CEA861_SD_TOTAL_SIZE             (NVT_CEA861_SD_HEADER_SIZE + NVT_CEA861_SD_PAYLOAD_SIZE)

//*************************
// short video descriptor
//*************************
#define NVT_CEA861_VIDEO_SD_SIZE             1
// the max total short video descriptors possible; See CEA-861-E, section 7.5,
// "It is also possible to have more than one of a specific type of data block if necessary
// to include all of the descriptors needed to describe the sinks capabilities."
#define NVT_CEA861_VIDEO_MAX_DESCRIPTOR      ((NVT_CEA861_MAX_PAYLOAD / NVT_CEA861_SD_TOTAL_SIZE) * (NVT_CEA861_SD_PAYLOAD_SIZE / NVT_CEA861_VIDEO_SD_SIZE) + \
                                              (NVT_CEA861_MAX_PAYLOAD % NVT_CEA861_SD_TOTAL_SIZE - NVT_CEA861_SD_HEADER_SIZE) / NVT_CEA861_VIDEO_SD_SIZE)
#define NVT_CTA861_VIDEO_VIC_MASK            0xFF  //the VIC mask of the short video descriptor
#define NVT_CTA861_7BIT_VIDEO_VIC_MASK       0x7F  //the 7 bits VIC mask of the short video descriptor
#define NVT_CTA861_VIDEO_NATIVE_MASK         0x80  //the Native mask of the short video descriptor
#define NVT_HDMI_YUV_420_PCLK_SUPPORTED_MIN  59000 //the vale shall equal or larger than 590MHz to support YCbCr in HDMI2.1

// CTA-861G supports more SVDs which is over 0x7F index
// All value below 192 will be treated as 7 bit VIC. Value 128~192 shall be forbidden.
#define NVT_GET_CTA_8BIT_VIC(vic)            (((vic) <= NVT_CTA861_7BIT_8BIT_SEPARATE_VALUE) ? ((vic) & NVT_CTA861_7BIT_VIDEO_VIC_MASK) : ((vic) & NVT_CTA861_VIDEO_VIC_MASK))
//

// According to CEA-861-E Spec.
// Note 3. A video timing with a vertical frequency that is an integer multiple
// of 6.00 Hz (i.e. 24.00, 30.00, 60.00, 120.00 or 240.00 Hz) is considered to
// be the same as a video timing with the equivalent detailed timing
// information but where the vertical frequency is adjusted by a factor of
// 1000/1001 (i.e., 24/1.001, 30/1.001, 60/1.001, 120/1.001 or 240/1.001).
// Excluding ceaIndex 1 640x480 which is a PC Mode.
#define NVT_CTA861_TIMING_FRR(_VID_, _RR_)       ((_VID_) > 1 && ((_RR_) % 6) == 0)
#define NVT_CEA861_640X480P_59940HZ_4X3      1     // Video Identification Code: format 1
#define NVT_CEA861_720X480P_59940HZ_4X3      2     // Video Identification Code: format 2
#define NVT_CEA861_720X480P_59940HZ_16X9     3     // Video Identification Code: format 3
#define NVT_CEA861_1280X720P_59940HZ_16X9    4     // ...
#define NVT_CEA861_1920X1080I_59940HZ_16X9   5     // ...
#define NVT_CEA861_1440X480I_59940HZ_4X3     6     // ...
#define NVT_CEA861_1440X480I_59940HZ_16X9    7     // ...
#define NVT_CEA861_1440X240P_59940HZ_4X3     8     // ...
#define NVT_CEA861_1440X240P_59940HZ_16X9    9     // ...
#define NVT_CEA861_2880X480I_59940HZ_4X3     10    // ...
#define NVT_CEA861_2880X480I_59940HZ_16X9    11    // ...
#define NVT_CEA861_2880X240P_59940HZ_4X3     12    // ...
#define NVT_CEA861_2880X240P_59940HZ_16X9    13    // ...
#define NVT_CEA861_1440X480P_59940HZ_4X3     14    // ...
#define NVT_CEA861_1440X480P_59940HZ_16X9    15    // ...
#define NVT_CEA861_1920X1080P_59940HZ_16X9   16    // ...
#define NVT_CEA861_720X576P_50000HZ_4X3      17    // ...
#define NVT_CEA861_720X576P_50000HZ_16X9     18    // ...
#define NVT_CEA861_1280X720P_50000HZ_16X9    19    // ...
#define NVT_CEA861_1920X1080I_50000HZ_16X9   20    // ...
#define NVT_CEA861_1440X576I_50000HZ_4X3     21    // ...
#define NVT_CEA861_1440X576I_50000HZ_16X9    22    // ...
#define NVT_CEA861_1440X288P_50000HZ_4X3     23    // ...
#define NVT_CEA861_1440X288P_50000HZ_16X9    24    // ...
#define NVT_CEA861_2880X576I_50000HZ_4X3     25    // ...
#define NVT_CEA861_2880X576I_50000HZ_16X9    26    // ...
#define NVT_CEA861_2880X288P_50000HZ_4X3     27    // ...
#define NVT_CEA861_2880X288P_50000HZ_16X9    28    // ...
#define NVT_CEA861_1440X576P_50000HZ_4X3     29    // ...
#define NVT_CEA861_1440X576P_50000HZ_16X9    30    // ...
#define NVT_CEA861_1920X1080P_50000HZ_16X9   31    // ...
#define NVT_CEA861_1920X1080P_23976HZ_16X9   32    // ...
#define NVT_CEA861_1920X1080P_25000HZ_16X9   33    // ...
#define NVT_CEA861_1920X1080P_29970HZ_16X9   34    // ...
#define NVT_CEA861_2880X480P_59940HZ_4X3     35    // ...
#define NVT_CEA861_2880X480P_59940HZ_16X9    36    // ...
#define NVT_CEA861_2880X576P_50000HZ_4X3     37    // ...
#define NVT_CEA861_2880X576P_50000HZ_16X9    38    // ...
#define NVT_CEA861_1920X1250I_50000HZ_16X9   39    // ...
#define NVT_CEA861_1920X1080I_100000HZ_16X9  40    // ...
#define NVT_CEA861_1280X720P_100000HZ_16X9   41    // ...
#define NVT_CEA861_720X576P_100000HZ_4X3     42    // ...
#define NVT_CEA861_720X576P_100000HZ_16X9    43    // ...
#define NVT_CEA861_1440X576I_100000HZ_4X3    44    // ...
#define NVT_CEA861_1440X576I_100000HZ_16X9   45    // ...
#define NVT_CEA861_1920X1080I_119880HZ_16X9  46    // ...
#define NVT_CEA861_1280X720P_119880HZ_16X9   47    // ...
#define NVT_CEA861_720X480P_119880HZ_4X3     48    // ...
#define NVT_CEA861_720X480P_119880HZ_16X9    49    // ...
#define NVT_CEA861_1440X480I_119880HZ_4X3    50    // ...
#define NVT_CEA861_1440X480I_119880HZ_16X9   51    // ...
#define NVT_CEA861_720X576P_200000HZ_4X3     52    // ...
#define NVT_CEA861_720X576P_200000HZ_16X9    53    // ...
#define NVT_CEA861_1440X576I_200000HZ_4X3    54    // ...
#define NVT_CEA861_1440X576I_200000HZ_16X9   55    // ...
#define NVT_CEA861_720X480P_239760HZ_4X3     56    // ...
#define NVT_CEA861_720X480P_239760HZ_16X9    57    // ...
#define NVT_CEA861_1440X480I_239760HZ_4X3    58    // Video Identification Code: format 58
#define NVT_CEA861_1440X480I_239760HZ_16X9   59    // Video Identification Code: format 59
#define NVT_CEA861_1280X720P_23976HZ_16X9    60    // ...
#define NVT_CEA861_1280X720P_25000HZ_16X9    61    // ...
#define NVT_CEA861_1280X720P_29970HZ_16X9    62    // ...
#define NVT_CEA861_1920X1080P_119880HZ_16X9  63    // ...
#define NVT_CEA861_1920X1080P_100000HZ_16X9  64    // ...

// Following modes are from CEA-861F
#define NVT_CEA861_1280X720P_23980HZ_64X27    65   // Video Identification Code: format 65
#define NVT_CEA861_1280X720P_25000HZ_64X27    66   // Video Identification Code: format 66
#define NVT_CEA861_1280X720P_29970HZ_64X27    67   // Video Identification Code: format 67
#define NVT_CEA861_1280X720P_50000HZ_64X27    68
#define NVT_CEA861_1280X720P_59940HZ_64X27    69
#define NVT_CEA861_1280X720P_100000HZ_64X27   70
#define NVT_CEA861_1280X720P_119880HZ_64X27   71
#define NVT_CEA861_1920X1080P_23980HZ_64X27   72
#define NVT_CEA861_1920X1080P_25000HZ_64X27   73
#define NVT_CEA861_1920X1080P_29970HZ_64X27   74
#define NVT_CEA861_1920X1080P_50000HZ_64X27   75
#define NVT_CEA861_1920X1080P_59940HZ_64X27   76
#define NVT_CEA861_1920X1080P_100000HZ_64X27  77
#define NVT_CEA861_1920X1080P_119880HZ_64X27  78
#define NVT_CEA861_1680X720P_23980HZ_64X27    79
#define NVT_CEA861_1680X720P_25000HZ_64X27    80
#define NVT_CEA861_1680X720P_29970HZ_64X27    81
#define NVT_CEA861_1680X720P_50000HZ_64X27    82
#define NVT_CEA861_1680X720P_59940HZ_64X27    83
#define NVT_CEA861_1680X720P_100000HZ_64X27   84
#define NVT_CEA861_1680X720P_119880HZ_64X27   85
#define NVT_CEA861_2560X1080P_23980HZ_64X27   86
#define NVT_CEA861_2560X1080P_25000HZ_64X27   87
#define NVT_CEA861_2560X1080P_29970HZ_64X27   88
#define NVT_CEA861_2560X1080P_50000HZ_64X27   89
#define NVT_CEA861_2560X1080P_59940HZ_64X27   90
#define NVT_CEA861_2560X1080P_100000HZ_64X27  91
#define NVT_CEA861_2560X1080P_119880HZ_64X27  92
#define NVT_CEA861_3840X2160P_23980HZ_16X9    93
#define NVT_CEA861_3840X2160P_25000HZ_16X9    94
#define NVT_CEA861_3840X2160P_29970HZ_16X9    95
#define NVT_CEA861_3840X2160P_50000HZ_16X9    96
#define NVT_CEA861_3840X2160P_59940HZ_16X9    97
#define NVT_CEA861_4096X2160P_23980HZ_256X135 98
#define NVT_CEA861_4096X2160P_25000HZ_256X135 99
#define NVT_CEA861_4096X2160P_29970HZ_256X135 100
#define NVT_CEA861_4096X2160P_50000HZ_256X135 101
#define NVT_CEA861_4096X2160P_59940HZ_256X135 102
#define NVT_CEA861_4096X2160P_23980HZ_64X27   103
#define NVT_CEA861_4096X2160P_25000HZ_64X27   104
#define NVT_CEA861_4096X2160P_29970HZ_64X27   105
#define NVT_CEA861_4096X2160P_50000HZ_64X27   106
#define NVT_CEA861_4096X2160P_59940HZ_64X27   107

// Following modes are from CTA-861G
#define NVT_CTA861_1280X720P_47950HZ_16X9     108
#define NVT_CTA861_1280X720P_47950HZ_64x27    109
#define NVT_CTA861_1680X720P_47950HZ_64x27    110
#define NVT_CTA861_1920X1080P_47950HZ_16X9    111
#define NVT_CTA861_1920X1080P_47950HZ_64x27   112
#define NVT_CTA861_2560X1080P_47950HZ_64x27   113
#define NVT_CTA861_3840X2160P_47950HZ_16X9    114
#define NVT_CTA861_4096x2160p_47950HZ_256X135 115
#define NVT_CTA861_3840x2160p_47950HZ_64x276  116
#define NVT_CTA861_3840x2160p_100000HZ_16X9   117
#define NVT_CTA861_3840x2160p_119880HZ_16X9   118
#define NVT_CTA861_3840x2160p_100000HZ_64X276 119
#define NVT_CTA861_3840x2160p_119880HZ_64X276 120
#define NVT_CTA861_5120x2160p_23980HZ_64X276  121
#define NVT_CTA861_5120x2160p_25000HZ_64X276  122
#define NVT_CTA861_5120x2160p_29970HZ_64X276  123
#define NVT_CTA861_5120x2160p_47950Hz_64X276  124
#define NVT_CTA861_5120x2160p_50000HZ_64X276  125
#define NVT_CTA861_5120x2160p_59940HZ_64X276  126
#define NVT_CTA861_5120x2160p_100000HZ_64X276 127

#define NVT_CTA861_7BIT_8BIT_SEPARATE_VALUE   192

#define NVT_CTA861_5120x2160p_119880HZ_64X276 193
#define NVT_CTA861_7680x4320p_23980HZ_16X9    194
#define NVT_CTA861_7680x4320p_25000HZ_16X9    195
#define NVT_CTA861_7680x4320p_29970HZ_16X9    196
#define NVT_CTA861_7680x4320p_47950HZ_16X9    197
#define NVT_CTA861_7680x4320p_50000HZ_16X9    198
#define NVT_CTA861_7680x4320p_59940HZ_16X9    199
#define NVT_CTA861_7680x4320p_100000HZ_16X9   200
#define NVT_CTA861_7680x4320p_119880HZ_16X9   201
#define NVT_CTA861_7680x4320p_23980HZ_64X276  202
#define NVT_CTA861_7680x4320p_25000HZ_64X276  203
#define NVT_CTA861_7680x4320p_29970HZ_64X276  204
#define NVT_CTA861_7680x4320p_47950HZ_64X276  205
#define NVT_CTA861_7680x4320p_50000HZ_64X276  206
#define NVT_CTA861_7680x4320p_59940HZ_64X276  207
#define NVT_CTA861_7680x4320p_100000HZ_64X276 208
#define NVT_CTA861_7680x4320p_119880HZ_64X276 209
#define NVT_CTA861_10240x4320p_23980HZ_64X276 210
#define NVT_CTA861_10240x4320p_25000HZ_64X276 211
#define NVT_CTA861_10240x4320p_29970HZ_64X276 212
#define NVT_CTA861_10240x4320p_47950HZ_64X276 213
#define NVT_CTA861_10240x4320p_50000HZ_64X276 214
#define NVT_CTA861_10240x4320p_59940HZ_64X276 215
#define NVT_CTA861_10240x4320p_100000HZ_64X276 216
#define NVT_CTA861_10240x4320p_119880HZ_64X276 217
#define NVT_CTA861_4096x2160p_100000HZ_256X135 218
#define NVT_CTA861_4096x2160p_119880HZ_256X135 219

// When defining new CEA861 format:
// Search code base to update array of certain category of CEA formats, such as 720p, 1080i, etc...
// Ideally, it's better to define these groups in one module. However, they should not reside
// in this .h file, thus updating these groups in other file is still needed.
// example of the group: 720p: NVT_CEA861_1280X720P_59940HZ_16X9,
//                             NVT_CEA861_1280X720P_100000HZ_16X9,
//                             NVT_CEA861_1280X720P_119880HZ_16X9

// According to CEA-861-I Spec.
// Table 11 - Resoution Identification (RID)
#define NVT_CTA861_OVT_TIMING_FRR(_FLAG_, _RR_)  (((_FLAG_) & (NVT_FLAG_CTA_OVT_TIMING)) != 0 && ((_RR_) % 6) == 0 && (_RR_) != 300)
#define NVT_CTA861_RID_NONE                NVT_INFOFRAME_CTRL_DONTCARE
#define NVT_CTA861_RID_1280x720p_16x9      1
#define NVT_CTA861_RID_1280x720p_64x27     2
#define NVT_CTA861_RID_1680x720p_64x27     3
#define NVT_CTA861_RID_1920x1080p_16x9     4
#define NVT_CTA861_RID_1920x1080p_64x27    5
#define NVT_CTA861_RID_2560x1080p_64x27    6
#define NVT_CTA861_RID_3840x1080p_32x9     7
#define NVT_CTA861_RID_2560x1440p_16x9     8
#define NVT_CTA861_RID_3440x1440p_64x27    9
#define NVT_CTA861_RID_5120x1440p_32x9     10
#define NVT_CTA861_RID_3840x2160p_16x9     11
#define NVT_CTA861_RID_3840x2160p_64x27    12
#define NVT_CTA861_RID_5120x2160p_64x27    13
#define NVT_CTA861_RID_7680x2160p_32x9     14
#define NVT_CTA861_RID_5120x2880p_16x9     15
#define NVT_CTA861_RID_5120x2880p_64x27    16
#define NVT_CTA861_RID_6880x2880p_64x27    17
#define NVT_CTA861_RID_10240x2880p_32x9    18
#define NVT_CTA861_RID_7680x4320p_16x9     19
#define NVT_CTA861_RID_7680x4320p_64x27    20
#define NVT_CTA861_RID_10240x4320p_64x27   21
#define NVT_CTA861_RID_15360x4320p_32x9    22
#define NVT_CTA861_RID_11520x6480p_16x9    23
#define NVT_CTA861_RID_11520x6480p_64x27   24
#define NVT_CTA861_RID_15360x6480p_64x27   25
#define NVT_CTA861_RID_15360x8640p_16x9    26
#define NVT_CTA861_RID_15360x8640p_64x27   27
#define NVT_CTA861_RID_20480x8640p_64x27   28
#define NVT_CTA861_RID_EXCEED_RESOLUTION   NVT_CTA861_RID_NONE

// Table 12 - AVI InfoFrame Video Format Frame Rate
#define NVT_CTA861_FR_NO_DATA              NVT_INFOFRAME_CTRL_DONTCARE
#define NVT_CTA861_FR_2398                 1
#define NVT_CTA861_FR_2400                 2
#define NVT_CTA861_FR_2500                 3
#define NVT_CTA861_FR_2997                 4
#define NVT_CTA861_FR_3000                 5
#define NVT_CTA861_FR_4795                 6
#define NVT_CTA861_FR_4800                 7
#define NVT_CTA861_FR_5000                 8
#define NVT_CTA861_FR_5994                 9
#define NVT_CTA861_FR_6000                 10
#define NVT_CTA861_FR_10000                11
#define NVT_CTA861_FR_11988                12
#define NVT_CTA861_FR_12000                13
#define NVT_CTA861_FR_14386                14
#define NVT_CTA861_FR_14400                15
#define NVT_CTA861_FR_20000                16
#define NVT_CTA861_FR_23976                17
#define NVT_CTA861_FR_24000                18
#define NVT_CTA861_FR_30000                19
#define NVT_CTA861_FR_35964                20
#define NVT_CTA861_FR_36000                21
#define NVT_CTA861_FR_40000                22
#define NVT_CTA861_FR_47952                23
#define NVT_CTA861_FR_48000                24

//*************************
// short audio descriptor
//*************************
#define NVT_CEA861_AUDIO_SD_SIZE             sizeof(NVT_3BYTES)
// the max total short audio descriptors possible; See CEA-861-E, section 7.5 on repeated types
#define NVT_CEA861_AUDIO_MAX_DESCRIPTOR      ((NVT_CEA861_MAX_PAYLOAD / NVT_CEA861_SD_TOTAL_SIZE) * (NVT_CEA861_SD_PAYLOAD_SIZE / NVT_CEA861_AUDIO_SD_SIZE) + \
                                              (NVT_CEA861_MAX_PAYLOAD % NVT_CEA861_SD_TOTAL_SIZE - NVT_CEA861_SD_HEADER_SIZE) / NVT_CEA861_AUDIO_SD_SIZE)
//
// short audio descriptor - byte 1
#define NVT_CEA861_AUDIO_FORMAT_MASK         0x78  //the audio format mask of the CEA short
#define NVT_CEA861_AUDIO_FORMAT_SHIFT        3     //the audio format data shift
//
#define NVT_CEA861_AUDIO_FORMAT_RSVD         0     // short audio descriptor format - reserved
#define NVT_CEA861_AUDIO_FORMAT_LINEAR_PCM   1     // short audio descriptor format - Linear PCM (uncompressed)
#define NVT_CEA861_AUDIO_FORMAT_AC3          2     // short audio descriptor format - AC3
#define NVT_CEA861_AUDIO_FORMAT_MPEG1        3     // short audio descriptor format - MPEG1(layer 1&2)
#define NVT_CEA861_AUDIO_FORMAT_MP3          4     // short audio descriptor format - MP3(MPEG1 layer 3)
#define NVT_CEA861_AUDIO_FORMAT_MPEG2        5     // short audio descriptor format - MPEG2 (multichannel)
#define NVT_CEA861_AUDIO_FORMAT_AAC          6     // short audio descriptor format - AAC
#define NVT_CEA861_AUDIO_FORMAT_DTS          7     // short audio descriptor format - DTS
#define NVT_CEA861_AUDIO_FORMAT_ATRAC        8     // short audio descriptor format - ATRAC
#define NVT_CEA861_AUDIO_FORMAT_ONE_BIT      9     // short audio descriptor format - one bit audio
#define NVT_CEA861_AUDIO_FORMAT_DDP          10    // short audio descriptor format - dolby digital +
#define NVT_CEA861_AUDIO_FORMAT_DTS_HD       11    // short audio descriptor format - DTS_HD
#define NVT_CEA861_AUDIO_FORMAT_MAT          12    // short audio descriptor format - MAT(MLP)
#define NVT_CEA861_AUDIO_FORMAT_DST          13    // short audio descriptor format - DST
#define NVT_CEA861_AUDIO_FORMAT_WMA_PRO      14    // short audio descriptor format - WMA Pro
#define NVT_CEA861_AUDIO_FORMAT_RSVD15       15    // short audio descriptor format - reserved
//
#define NVT_CEA861_AUDIO_MAX_CHANNEL_MASK    7     // short audio descriptor format - Max Number of channels - 1
#define NVT_CEA861_AUDIO_MAX_CHANNEL_SHIFT   0     // short audio descriptor format shift
//
// short audio descriptor - byte 2
#define NVT_CEA861_AUDIO_SAMPLE_RATE_MASK    0x7F  //the sample rate mask
#define NVT_CEA861_AUDIO_SAMPLE_RATE_SHIFT   0     //the sample rate shift
//
#define NVT_CEA861_AUDIO_SAMPLE_RATE_32KHZ   0x01  // short audio descriptor - sample rate : 32KHz
#define NVT_CEA861_AUDIO_SAMPLE_RATE_44KHZ   0x02  // short audio descriptor - sample rate : 44KHz
#define NVT_CEA861_AUDIO_SAMPLE_RATE_48KHZ   0x04  // short audio descriptor - sample rate : 48KHz
#define NVT_CEA861_AUDIO_SAMPLE_RATE_88KHZ   0x08  // short audio descriptor - sample rate : 88KHz
#define NVT_CEA861_AUDIO_SAMPLE_RATE_96KHZ   0x10  // short audio descriptor - sample rate : 96KHz
#define NVT_CEA861_AUDIO_SAMPLE_RATE_176KHZ  0x20  // short audio descriptor - sample rate : 176KHz
#define NVT_CEA861_AUDIO_SAMPLE_RATE_192KHZ  0x40  // short audio descriptor - sample rate : 192KHz
#define NVT_CEA861_AUDIO_SAMPLE_RATE_RSVD    0x80  // short audio descriptor - sample rate : reserved
//
// short audio descriptor - byte 3
#define NVT_CEA861_AUDIO_SAMPLE_DEPTH_MASK   0x07  // the uncompressed audio resolution mask
#define NVT_CEA861_AUDIO_SAMPLE_DEPTH_SHIFT  0     // the uncompressed audio resolution shift
//
#define NVT_CEA861_AUDIO_SAMPLE_SIZE_16BIT   0x01  // uncompressed (Linear PCM) audio A/D resolution - 16bit
#define NVT_CEA861_AUDIO_SAMPLE_SIZE_20BIT   0x02  // uncompressed (Linear PCM) audio A/D resolution - 20bit
#define NVT_CEA861_AUDIO_SAMPLE_SIZE_24BIT   0x04  // uncompressed (Linear PCM) audio A/D resolution - 24bit

//**************************
// speaker allocation data
//**************************
#define NVT_CEA861_SPEAKER_SD_SIZE           sizeof(NVT_3BYTES)
// the max total short speaker descriptors possible; See CEA-861-E, section 7.5 on repeated types
#define NVT_CEA861_SPEAKER_MAX_DESCRIPTOR    ((NVT_CEA861_MAX_PAYLOAD / NVT_CEA861_SD_TOTAL_SIZE) * (NVT_CEA861_SD_PAYLOAD_SIZE / NVT_CEA861_SPEAKER_SD_SIZE) + \
                                              (NVT_CEA861_MAX_PAYLOAD % NVT_CEA861_SD_TOTAL_SIZE - NVT_CEA861_SD_HEADER_SIZE) / NVT_CEA861_SPEAKER_SD_SIZE)
#define NVT_CEA861_SPEAKER_ALLOC_MASK        0x7F  // the speaker allocation mask
#define NVT_CEA861_SPEAKER_ALLOC_SHIFT       0     // the speaker allocation mask shift
//
#define NVT_CEA861_SPEAKER_ALLOC_FL_FR       0x01  // speaker allocation : Front Left + Front Right
#define NVT_CEA861_SPEAKER_ALLOC_LFE         0x02  // speaker allocation : Low Frequency Effect
#define NVT_CEA861_SPEAKER_ALLOC_FC          0x04  // speaker allocation : Front Center
#define NVT_CEA861_SPEAKER_ALLOC_RL_RR       0x08  // speaker allocation : Rear Left + Rear Right
#define NVT_CEA861_SPEAKER_ALLOC_RC          0x10  // speaker allocation : Rear Center
#define NVT_CEA861_SPEAKER_ALLOC_FLC_FRC     0x20  // speaker allocation : Front Left Center + Front Right Center
#define NVT_CEA861_SPEAKER_ALLOC_RLC_RRC     0x40  // speaker allocation : Rear Left Center + Rear Right Center

//***********************
// vendor specific data block (VSDB)
//***********************
#define NVT_CEA861_VSDB_HEADER_SIZE          4
#define NVT_CEA861_VSDB_PAYLOAD_MAX_LENGTH   28    // max allowed vendor specific data block payload (in byte)
#define NVT_CEA861_HDMI_IEEE_ID              0x0C03
#define NVT_CEA861_HDMI_LLC_IEEE_ID          NVT_CEA861_HDMI_IEEE_ID
#define NVT_CEA861_NVDA_IEEE_ID              0x44B
#define NVT_CEA861_HDMI_FORUM_IEEE_ID        0xC45DD8
#define NVT_CEA861_MSFT_IEEE_ID              0xCA125C

#define NVT_CEA861_VSDB_MAX_BLOCKS           4    // NOTE: The maximum number of VSDB blocks should be:
                                                  // (NVT_CEA861_MAX_PAYLOAD / (NVT_CEA861_VSDB_HEADER_SIZE + 1)) (assume at least 1 byte of payload)
                                                  // As of Sept 2013, there are 3 different VSDBs defined in the spec. Hence allocating space for all 24
                                                  // is overkill. As a tradeoff, we define this limit as 4 for now. If required, this should be increased later.

typedef struct VSDB_DATA
{
    NvU32      ieee_id;
    NvU32      vendor_data_size;                                   // size of data copied to vendor_data (excludes ieee_id from frame)
    NvU8       vendor_data[NVT_CEA861_VSDB_PAYLOAD_MAX_LENGTH];
} VSDB_DATA;

//*******************************
// vendor specific video data block (VSVDB)
//*******************************
#define NVT_CEA861_VSVDB_MAX_BLOCKS                 2           // Dolby Vision, HDR10+ VSVDBs
#define NVT_CEA861_DV_IEEE_ID                       0x00D046
#define NVT_CEA861_HDR10PLUS_IEEE_ID                0x90848B
#define NVT_CEA861_VSVDB_PAYLOAD_MAX_LENGTH         25          // max allowed vendor specific video data block payload (in byte)
#define NVT_CEA861_VSVDB_VERSION_MASK               0xE0        // vsdb version mask
#define NVT_CEA861_VSVDB_VERSION_MASK_SHIFT         5           // vsdb version shift mask

typedef struct VSVDB_DATA
{
    NvU32      ieee_id;
    NvU32      vendor_data_size;                                   // size of data copied to vendor_data (excludes ieee_id from frame)
    NvU8       vendor_data[NVT_CEA861_VSVDB_PAYLOAD_MAX_LENGTH];
} VSVDB_DATA;

//*******************************
// Video Format Data Block (VFDB)
//*******************************

#define NVT_CTA861_VF_MAX_BLOCKS         4
#define NVT_CTA861_VF_MAX_DESCRIPTORS    30

#define NVT_CTA861_VF_RID_MASK           0x3F

typedef struct tagNVT_RID_CODES
{
    NvU16 HVisible;   // horizontal visible
    NvU8  HSyncPol;   // horizontal sync polarity: 1-negative, 0-positive
    NvU16 VVisible;   // vertical visible
    NvU8  VSyncPol;   // vertical sync polarity: 1-negative, 0-positive
    NvU16 interlaced; // 1-interlaced, 0-progressive
    NvU32 aspect;     // the display aspect ratio Hi(aspect):horizontal-aspect, Low(aspect):vertical-aspect
    NvU8  rid;        // Resolution Identification (RID)
} NVT_RID_CODES;

#pragma pack(1)
typedef struct tagVFD_ONE_BYTE
{
    NvU8 rid          : 6;
    NvU8 fr24         : 1;
    NvU8 bfr50        : 1;
} VFD_ONE_BYTE;

typedef struct tagVFD_TWO_BYTE
{
    VFD_ONE_BYTE in_onebyte;
    NvU8 frRate       : 6;
    NvU8 fr144        : 1;
    NvU8 bfr60        : 1;
} VFD_TWO_BYTE;

typedef struct tagVFD_THREE_BYTE
{
    VFD_TWO_BYTE in_twobyte;
    NvU8 fr48         : 1;
    NvU8 f31_37       : 7;
} VFD_THREE_BYTE;

typedef struct tagVFD_FOUR_BYTE
{
    VFD_THREE_BYTE in_threebyte;
    NvU8           f40_47;
} VFD_FOUR_BYTE;

typedef struct tagVFDB_DATA
{
    struct {
        NvU8 vfd_len  : 2;
        NvU8 f22_25   : 4;
        NvU8 ntsc     : 1;
        NvU8 y420     : 1;
    } info;

    NvU8 total_vfd;
    NvU8 video_format_desc[NVT_CTA861_VF_MAX_DESCRIPTORS];
} VFDB_DATA;

typedef struct tagNVT_DV_STATIC_METADATA_TYPE0
{
    // first byte
    NvU8 supports_YUV422_12bit      : 1;
    NvU8 supports_2160p60hz         : 1;
    NvU8 supports_global_dimming    : 1;
    NvU8 reserved_1                 : 2;
    NvU8 VSVDB_version              : 3;

    // second- fourth byte
    NvU8 cc_red_y_3_0               : 4;
    NvU8 cc_red_x_3_0               : 4;
    NvU8 cc_red_x_11_4              : 8;
    NvU8 cc_red_y_11_4              : 8;

    NvU8 cc_green_y_3_0             : 4;
    NvU8 cc_green_x_3_0             : 4;
    NvU8 cc_green_x_11_4            : 8;
    NvU8 cc_green_y_11_4            : 8;

    NvU8 cc_blue_y_3_0              : 4;
    NvU8 cc_blue_x_3_0              : 4;
    NvU8 cc_blue_x_11_4             : 8;
    NvU8 cc_blue_y_11_4             : 8;

    NvU8 cc_white_y_3_0             : 4;
    NvU8 cc_white_x_3_0             : 4;
    NvU8 cc_white_x_11_4            : 8;
    NvU8 cc_white_y_11_4            : 8;

    NvU8 target_max_pq_3_0          : 4;
    NvU8 target_min_pq_3_0          : 4;
    NvU8 target_min_pq_11_4         : 8;
    NvU8 target_max_pq_11_4         : 8;

    NvU8 dm_version_minor           : 4;
    NvU8 dm_version_major           : 4;

    NvU8 reserved_2                 : 8;
    NvU8 reserved_3                 : 8;
    NvU8 reserved_4                 : 8;
    NvU8 reserved_5                 : 8;
} NVT_DV_STATIC_METADATA_TYPE0;

typedef struct tagNVT_DV_STATIC_METADATA_TYPE1
{
    // first byte
    NvU8 supports_YUV422_12bit      : 1;
    NvU8 supports_2160p60hz         : 1;
    NvU8 dm_version                 : 3;
    NvU8 VSVDB_version              : 3;

    // second byte
    NvU8 supports_global_dimming    : 1;
    NvU8 target_max_luminance       : 7;

    // third byte
    NvU8 colorimetry                : 1;
    NvU8 target_min_luminance       : 7;

    //fourth byte
    NvU8 reserved                   : 8;
    //fith to tenth byte
    NvU8 cc_red_x                   : 8;
    NvU8 cc_red_y                   : 8;
    NvU8 cc_green_x                 : 8;
    NvU8 cc_green_y                 : 8;
    NvU8 cc_blue_x                  : 8;
    NvU8 cc_blue_y                  : 8;
} NVT_DV_STATIC_METADATA_TYPE1;

typedef struct tagNVT_DV_STATIC_METADATA_TYPE1_1
{
    // first byte
    NvU8 supports_YUV422_12bit       : 1;
    NvU8 supports_2160p60hz          : 1;
    NvU8 dm_version                  : 3;
    NvU8 VSVDB_version               : 3;

    // second byte
    NvU8 supports_global_dimming     : 1;
    NvU8 target_max_luminance        : 7;

    // third byte
    NvU8 colorimetry                 : 1;
    NvU8 target_min_luminance        : 7;

    //fourth byte
    NvU8 interface_supported_by_sink : 2;
    NvU8 unique_By                   : 3;
    NvU8 unique_Bx                   : 3;

    //fifth byte
    NvU8 unique_Ry_bit_0             : 1;
    NvU8 unique_Gx                   : 7;

    //sixth byte
    NvU8 unique_Ry_bit_1             : 1;
    NvU8 unique_Gy                   : 7;

    //seventh byte
    NvU8 unique_Rx                   : 5;
    NvU8 unique_Ry_bit_2_to_4        : 3;

} NVT_DV_STATIC_METADATA_TYPE1_1;

typedef struct tagNVT_DV_STATIC_METADATA_TYPE2
{
    // first byte
    NvU8 supports_YUV422_12bit       : 1;
    NvU8 supports_backlight_control  : 1;
    NvU8 dm_version                  : 3;
    NvU8 VSVDB_version               : 3;

    // second byte
    NvU8 reserved                    : 2;
    NvU8 supports_global_dimming     : 1;
    NvU8 target_min_luminance        : 5;

    // third byte
    NvU8 interface_supported_by_sink : 2;
    NvU8 parity                      : 1;
    NvU8 target_max_luminance        : 5;

    //fourth byte
    NvU8 supports_10b_12b_444_bit1   : 1;
    NvU8 unique_Gx                   : 7;

    //fifth byte
    NvU8 supports_10b_12b_444_bit0   : 1;
    NvU8 unique_Gy                   : 7;

    //sixth byte
    NvU8 unique_Bx                   : 3;
    NvU8 unique_Rx                   : 5;

    //seventh byte
    NvU8 unique_By                   : 3;
    NvU8 unique_Ry                   : 5;

} NVT_DV_STATIC_METADATA_TYPE2;

typedef struct tagNVT_HDR10PLUS_INFO
{
    // first byte
    NvU8 application_version                : 2;
    NvU8 full_frame_peak_luminance_index    : 2;
    NvU8 peak_luminance_index               : 4;
} NVT_HDR10PLUS_INFO;
#pragma pack()

//***************************
// colorimetry data block
//***************************
//
// Colorimetry capabilities - byte 3
#define NVT_CEA861_COLORIMETRY_MASK          0xFF  // the colorimetry cap mask
#define NVT_CEA861_COLORIMETRY_SHIFT            0  // the colorimetry cap shift

#define NVT_CEA861_COLORIMETRY_NO_DATA       0x00
#define NVT_CEA861_COLORIMETRY_xvYCC_601     0x01  // xvYCC601 capable
#define NVT_CEA861_COLORIMETRY_xvYCC_709     0x02  // xvYCC709 capable
#define NVT_CEA861_COLORIMETRY_sYCC_601      0x04  // sYCC601 capable
#define NVT_CEA861_COLORIMETRY_AdobeYCC_601  0x08  // AdobeYCC601 capable
#define NVT_CEA861_COLORIMETRY_AdobeRGB      0x10  // AdobeRGB capable
#define NVT_CEA861_COLORIMETRY_BT2020cYCC    0x20  // BT2020 cYCbCr (constant luminance) capable
#define NVT_CEA861_COLORIMETRY_BT2020YCC     0x40  // BT2020 Y'CbCr capable
#define NVT_CEA861_COLORIMETRY_BT2020RGB     0x80  // BT2020 RGB capable
// Colorimetry capabilities - byte 4
#define NVT_CEA861_COLORIMETRY_defaultRGB    0x10 // based on the default chromaticity in Basic Display Parameters and Feature Block
#define NVT_CEA861_COLORIMETRY_sRGB          0x20 // IEC 61966-2-1
#define NVT_CEA861_COLORIMETRY_ICtCp         0x40 // ITU-R BT.2100 ICtCp
#define NVT_CEA861_COLORIMETRY_ST2113RGB     0x80 // SMPTE ST 2113 R'G'B'
//
// gamut-related metadata capabilities - byte 4
#define NVT_CEA861_GAMUT_METADATA_MASK       0x8F  // the colorimetry or gamut-related metadata block mask
#define NVT_CEA861_GAMUT_METADATA_SHIFT      0     // the metadata block shift
//
#define NVT_CEA861_GAMUT_METADATA_MD0        0x01  // MD0
#define NVT_CEA861_GAMUT_METADATA_MD1        0x02  // MD1
#define NVT_CEA861_GAMUT_METADATA_MD2        0x04  // MD2
#define NVT_CEA861_GAMUT_METADATA_MD3        0x08  // MD2

//***************************
// HDR static metadata data block
//***************************
//
typedef struct tagNVT_5BYTES
{
    NvU8   byte1;
    NvU8   byte2;
    NvU8   byte3;
    NvU8   byte4;
    NvU8   byte5;
} NVT_5BYTES;

// Supported Electro-Optical Transfer Function - byte 3
#define NVT_CEA861_EOTF_MASK                        0x3F  // the EOTF cap mask
#define NVT_CEA861_EOTF_SHIFT                       0     // the EOTF cap shift
//
#define NVT_CEA861_EOTF_GAMMA_SDR                   0x01  // ET_0 Traditional gamma - SDR Luminance Range
#define NVT_CEA861_EOTF_GAMMA_HDR                   0x02  // ET_1 Traditional gamma - HDR Luminance Range
#define NVT_CEA861_EOTF_SMPTE_ST2084                0x04  // ET_2 SMPTE ST2084 EOTF (a.k.a PQ - Perceptual Quantizer EOTF)
#define NVT_CEA861_EOTF_FUTURE                      0x08  // ET_3 Future EOTF

//
// Supported Static Metadata Descriptor - byte 4
#define NVT_CEA861_STATIC_METADATA_DESCRIPTOR_MASK  0x01  // the supported static metadata descriptor block mask
#define NVT_CEA861_STATIC_METADATA_SHIFT            0     // the metadata block shift
//
#define NVT_CEA861_STATIC_METADATA_SM0              0x00  // Static Metadata Type 1

//
// Desired Content Max Luminance data  - byte 5
#define NVT_CEA861_MAX_CLL_MASK                     0xFF  // the desired content max luminance level (MaxCLL) data block mask
#define NVT_CEA861_MAX_CLL_SHIFT                    0     // the metadata block shift

// Desired Content Max Frame-Average Luminance data  - byte 6
#define NVT_CEA861_MAX_FALL_MASK                    0xFF  // the desired content max frame-average luminance (MaxFALL) data block mask
#define NVT_CEA861_MAX_FALL_SHIFT                   0     // the metadata block shift

// Desired Content Min Luminance data  - byte 7
#define NVT_CEA861_MIN_CLL_MASK                     0xFF  // the desired content min luminance level (MinCLL) data block mask
#define NVT_CEA861_MIN_CLL_SHIFT                    0     // the metadata block shift

//***************************
// video capability data block
//***************************
//
#define NVT_CEA861_VIDEO_CAPABILITY_MASK     0x7F  // the video capability data block mask
#define NVT_CEA861_VIDEO_CAPABILITY_SHIFT    0     // the video capability data block shift
//
#define NVT_CEA861_VIDEO_CAPABILITY_S_CE0    0x01  // S_CE0
#define NVT_CEA861_VIDEO_CAPABILITY_S_CE1    0x02  // S_CE1
#define NVT_CEA861_VIDEO_CAPABILITY_S_IT0    0x04  // S_IT0
#define NVT_CEA861_VIDEO_CAPABILITY_S_IT1    0x08  // S_IT1
#define NVT_CEA861_VIDEO_CAPABILITY_S_PT0    0x10  // S_PT0
#define NVT_CEA861_VIDEO_CAPABILITY_S_PT1    0x20  // S_PT1
#define NVT_CEA861_VIDEO_CAPABILITY_S_QS     0x40  // S_QS

//**************************
// EDID 861 Extension Info
//**************************
typedef struct tagNVT_3BYTES
{
    NvU8   byte1;
    NvU8   byte2;
    NvU8   byte3;
} NVT_3BYTES;

//***********************
// VCDB specific data
//***********************
#define NVT_CEA861_VCDB_QS_MASK                 0x40  // quantization range selectable mask
#define NVT_CEA861_VCDB_QS_SHIFT                6     // quantization range selectable shift

#define NVT_CEA861_VCDB_S_PT_MASK               0x30  // PT over/underscan behavior mask
#define NVT_CEA861_VCDB_S_PT_SHIFT              4     // PT over/underscan behavior shift
#define NVT_CEA861_VCDB_S_PT_NO_DATA            0x00
#define NVT_CEA861_VCDB_S_PT_ALWAYS_OVERSCAN    0x01
#define NVT_CEA861_VCDB_S_PT_ALWAYS_UNDERSCAN   0x02
#define NVT_CEA861_VCDB_S_PT_OVER_OR_UNDERSCAN  0x03

#define NVT_CEA861_VCDB_S_IT_MASK               0x0C  // IT over/underscan behavior mask
#define NVT_CEA861_VCDB_S_IT_SHIFT              2     // IT over/underscan behavior shift
#define NVT_CEA861_VCDB_S_IT_NOT_SUPPORTED      0x00
#define NVT_CEA861_VCDB_S_IT_ALWAYS_OVERSCAN    0x01
#define NVT_CEA861_VCDB_S_IT_ALWAYS_UNDERSCAN   0x02
#define NVT_CEA861_VCDB_S_IT_OVER_OR_UNDERSCAN  0x03

#define NVT_CEA861_VCDB_S_CE_MASK               0x03  // CE over/underscan behavior mask
#define NVT_CEA861_VCDB_S_CE_SHIFT              0     // CE over/underscan behavior shift
#define NVT_CEA861_VCDB_S_CE_NOT_SUPPORTED      0x00
#define NVT_CEA861_VCDB_S_CE_ALWAYS_OVERSCAN    0x01
#define NVT_CEA861_VCDB_S_CE_ALWAYS_UNDERSCAN   0x02
#define NVT_CEA861_VCDB_S_CE_OVER_OR_UNDERSCAN  0x03

//
typedef struct tagNVT_2BYTES
{
    NvU8   byte1;
    NvU8   byte2;
} NVT_2BYTES;

#pragma pack(1)
#define NVT_CTA861_DID_MAX_DATA_BLOCK           4
//***********************
// DisplayID VII Video Timing Data Block (T7VDB)
//***********************
#define NVT_CTA861_DID_TYPE7_DESCRIPTORS_MIN    1
#define NVT_CTA861_DID_TYPE7_DESCRIPTORS_MAX    1
#define NVT_CTA861_DID_TYPE7_DESCRIPTORS_LENGTH 20

typedef struct tagDID_TYPE7_DATA
{
    struct {
        NvU8 revision : 3;
        NvU8 dsc_pt   : 1;
        NvU8 t7_m     : 3;
        NvU8 F37      : 1;
    } version;

    NvU8 total_descriptors;
    NvU8 payload[29];  // t7_m=0 so only 20byte used
} DID_TYPE7_DATA;

//***********************
// DisplayID VIII Video Timing Data Block (T8VDB)
//***********************
#define NVT_CTA861_DID_TYPE8_ONE_BYTE_DESCRIPTOR       1
#define NVT_CTA861_DID_TYPE8_TWO_BYTE_DESCRIPTOR       2
#define NVT_CTA861_DID_TYPE8_DESCRIPTORS_MIN           1
#define NVT_CTA861_DID_TYPE8_ONE_BYTE_DESCRIPTORS_MAX  28
#define NVT_CTA861_DID_TYPE8_TWO_BYTE_DESCRIPTORS_MAX  14

typedef struct tagDID_TYPE8_DATA
{
    struct {
        NvU8 revision  : 3;
        NvU8 tcs       : 1;
        NvU8 F34       : 1;
        NvU8 t8y420    : 1;
        NvU8 code_type : 2;
    } version;

    NvU8 total_descriptors;
    NvU8 payload[NVT_CTA861_DID_TYPE8_ONE_BYTE_DESCRIPTORS_MAX];  // used one_byte descriptor length
} DID_TYPE8_DATA;

//***********************
// DisplayID X Video Timing Data Block (T10VDB)
//***********************
#define NVT_CTA861_DID_TYPE10_DESCRIPTORS_MIN    1
#define NVT_CTA861_DID_TYPE10_DESCRIPTORS_MAX    4

typedef struct tagDID_TYPE10_DATA
{
    struct {
        NvU8 revision : 3;
        NvU8 F33      : 1;
        NvU8 t10_m    : 3;
        NvU8 F37      : 1;
    } version;

    NvU8 total_descriptors;
    NvU8 payload[28];  // given the 7bytes * 4 space
} DID_TYPE10_DATA;

//***********************
// Native Video Resolution Data Block (NVRDB)
//***********************
typedef struct tagNATIVE_VIDEO_RESOLUTION_DATA
{
    NvU8  native_svr;

    struct {
        NvU8 img_size : 1;
        NvU8 f41      : 1;
        NvU8 f42      : 1;
        NvU8 f43      : 1;
        NvU8 f44      : 1;
        NvU8 f45      : 1;
        NvU8 f46      : 1;
        NvU8 sz_prec  : 1;
    } option;

    NvU8 image_size[4];
} NATIVE_VIDEO_RESOLUTION_DATA;

#pragma pack()

// See CEA-861E, Table 42, 43 Extended Tags; indicates that the corresponding CEA extended data block value is valid,
// e.g. if colorimetry is set, then NVT_EDID_CEA861_INFO::colorimetry is valid
typedef struct tagNVT_VALID_EXTENDED_BLOCKS
{
    NvU32   VCDB                :  1;
    NvU32   VSVD                :  1;
    NvU32   colorimetry         :  1;
    NvU32   H14B_VSDB           :  1;
    NvU32   H20_HF_VSDB         :  1;
    NvU32   y420cmdb            :  1;
    NvU32   hdr_static_metadata :  1;
    NvU32   dv_static_metadata  :  1;
    NvU32   hdr10Plus           :  1;
    NvU32   SCDB                :  1;
    NvU32   HF_EEODB            :  1;
    NvU32   nvda_vsdb           :  1;
    NvU32   msft_vsdb           :  1;
    NvU32   NVRDB               :  1;
} NVT_VALID_EXTENDED_BLOCKS;

//*************************
// extended data blocks
//*************************
#define NVT_CEA861_SD_EXT_HEADER_SIZE           1

#define NVT_CEA861_Y420VDB_SD_SIZE              1

// Max number of YUV420 VDBs for each VDB block is 30 per CTA-861-G spec sec. 7.5.10
// Accomodate 2 blocks
#define NVT_CEA861_Y420VDB_MAX_DESCRIPTOR       60

#define NVT_CEA861_Y420CMDB_SD_SIZE             1

// Max number of YUV420 SVDs for each VDB block is 30 per CTA-861-G spec sec. 7.5.11
// Accomodate 2 blocks
#define NVT_CEA861_Y420CMDB_MAX_DESCRIPTOR      60
#define NVT_CEA861_VFPDB_SD_SIZE                1
#define NVT_CEA861_VFPDB_MAX_DESCRIPTOR         16  // NOTE: Limiting to 16 to not allocate too much space.  The maximum descriptor should be:
                                                    // ((NVT_CEA861_MAX_PAYLOAD / NVT_CEA861_SD_TOTAL_SIZE) * (NVT_CEA861_SD_PAYLOAD_SIZE / NVT_CEA861_VFPDB_SD_SIZE) +
                                                    // (NVT_CEA861_MAX_PAYLOAD % NVT_CEA861_SD_TOTAL_SIZE - NVT_CEA861_SD_HEADER_SIZE - NVT_CEA861_SD_EXT_HEADER_SIZE) / NVT_CEA861_VFPDB_SD_SIZE)

typedef enum tagNVT_CTA861_ORIGIN
{
    FROM_CTA861_EXTENSION,
    FROM_DISPLAYID_13_DATA_BLOCK,
    FROM_DISPLAYID_20_DATA_BLOCK,
} NVT_CTA861_ORIGIN;

//
typedef struct tagEDID_CEA861_INFO
{
    NvU8       revision;
    NvU8       basic_caps;

    // short video descriptor
    NvU8       total_svd;
    NvU8       video[NVT_CEA861_VIDEO_MAX_DESCRIPTOR];

    // short audio descriptor
    NvU8       total_sad;
    NVT_3BYTES audio[NVT_CEA861_AUDIO_MAX_DESCRIPTOR];

    // speaker allocation data
    NvU8       total_ssd;
    NVT_3BYTES speaker[NVT_CEA861_SPEAKER_MAX_DESCRIPTOR];

    // vendor specific data
    NvU8       total_vsdb;
    VSDB_DATA  vsdb[NVT_CEA861_VSDB_MAX_BLOCKS];

    // vendor specific video data
    NvU8       total_vsvdb;
    VSVDB_DATA vsvdb[NVT_CEA861_VSVDB_MAX_BLOCKS];

    // video format data
    NvU8       total_vfdb;
    VFDB_DATA  vfdb[NVT_CTA861_VF_MAX_BLOCKS];

    // indicates which of the extended data blocks below contain valid data excluding extended blocks with total count
    NVT_VALID_EXTENDED_BLOCKS   valid;
    // extended data blocks
    NVT_2BYTES                  colorimetry;        // Colorimetry Data Block
    NvU8                        video_capability;   // Video Capability Block

    // HDR Static Metadata Data Block. See CEA-861.3 HDR Static Metadata Extensions, Section 4.2
    NVT_5BYTES                  hdr_static_metadata;

    // VFPDB extended block. See CEA861-H, Section 7.5.12 Video Format Preference Data Block
    NvU8       total_svr;
    NvU8       svr_vfpdb[NVT_CEA861_VFPDB_MAX_DESCRIPTOR];          // svr of preferred video formats

    // NVRDB extended block. see CTA861.6, Section 7.5.18 Native Video Resolution Data Block
    NATIVE_VIDEO_RESOLUTION_DATA native_video_resolution_db;

    // Y420VDB extended block. See CEA861-F, Section 7.5.10 YCBCR 4:2:0 Video Data Block
    NvU8       total_y420vdb;
    NvU8       svd_y420vdb[NVT_CEA861_Y420VDB_MAX_DESCRIPTOR];      // svd of video formats that only support YCbCr 4:2:0

    // Y420CMDB extended block. See CEA861-F, Section 7.5.11 YCBCR 4:2:0 Capability Map Data Block
    NvU8       total_y420cmdb;
    NvU8       map_y420cmdb[NVT_CEA861_Y420CMDB_MAX_DESCRIPTOR];    // bit map to svd in video[] that also supports YCbCr 4:2:0

    // NVT_EDID_CEA861_INFO::vsvdb.SCDB = 1 in case hfscdb is exposed by sink.
    NvU32       hfscdbSize;
    NvU8        hfscdb[NVT_CTA861_EXT_SCDB_PAYLOAD_MAX_LENGTH];

    // DID Type VII Video extended block, see 7.5.17.1 in CTA861-H
    NvU8            total_did_type7db;
    DID_TYPE7_DATA  did_type7_data_block[NVT_CTA861_DID_MAX_DATA_BLOCK];

    // DID Type VIII Video extended block, see 7.5.17.2 in CTA861-H
    NvU8            total_did_type8db;
    DID_TYPE8_DATA  did_type8_data_block[NVT_CTA861_DID_MAX_DATA_BLOCK];

    // DID Type X Video extended block, see 7.5.17.3 in CTA861-H
    NvU8            total_did_type10db;
    DID_TYPE10_DATA did_type10_data_block[NVT_CTA861_DID_MAX_DATA_BLOCK];

    NvU8        hfeeodb;                            // HDMI Forum Edid Extension Override Data Block.
} NVT_EDID_CEA861_INFO;


//*******************
// Parsed DisplayID Information
//*******************
#define NVT_DISPLAYID_SECTION_MAX_SIZE                251
#define NVT_DISPLAYID_SECTION_HEADER_LEN              5
#define NVT_DISPLAYID_DATABLOCK_MAX_PAYLOAD_LEN       248
#define NVT_DISPLAYID_DATABLOCK_HEADER_LEN            3

#define NVT_DISPLAYID_PRODUCT_STRING_MAX_LEN          233
#define NVT_DISPLAYID_COLOR_MAX_WHITEPOINTS           5
#define NVT_DISPLAYID_COLOR_MAX_PRIMARIES             3
#define NVT_DISPLAYID_RANGE_LIMITS_MAX_COUNT          2
#define NVT_DISPLAYID_DISPLAY_INTERFACE_FEATURES_MAX_ADDITIONAL_SUPPORTED_COLORSPACE_EOTF 7

typedef enum tagNVT_SINGLE_TILE_BEHAVIOR
{
    NVT_SINGLE_TILE_BEHAVIOR_OTHER = 0,
    NVT_SINGLE_TILE_BEHAVIOR_SOURCE_DRIVEN,
    NVT_SINGLE_TILE_BEHAVIOR_SCALE,
    NVT_SINGLE_TILE_BEHAVIOR_CLONE
} NVT_SINGLE_TILE_BEHAVIOR;

typedef enum tagNVT_MULTI_TILE_BEHAVIOR
{
    NVT_MULTI_TILE_BEHAVIOR_OTHER = 0,
    NVT_MULTI_TILE_BEHAVIOR_SOURCE_DRIVEN
} NVT_MULTI_TILE_BEHAVIOR;

typedef struct _tagNVT_TILEDDISPLAY_TOPOLOGY_ID
{
        NvU32 vendor_id;
        NvU16 product_id;
        NvU32 serial_number;
} NVT_TILEDDISPLAY_TOPOLOGY_ID;

typedef struct _tagNVT_COLOR_POINT
{
    NvU16 x;
    NvU16 y;
} NVT_COLOR_POINT;

typedef struct _tagNVT_DISPLAYID_RANGE_LIMITS
{
    NvU32   revision;
    NvU32   pclk_min;
    NvU32   pclk_max;
    NvU8    hfreq_min;
    NvU8    hfreq_max;
    NvU16   hblank_min;
    NvU8    vfreq_min;
    NvU16   vfreq_max;
    NvU16   vblank_min;
    NvU8    interlaced                           : 1;
    NvU8    cvt                                  : 1;
    NvU8    cvt_reduced                          : 1;
    NvU8    dfd                                  : 1;
    NvU8    seamless_dynamic_video_timing_change : 1;
} NVT_DISPLAYID_RANGE_LIMITS;

#define NVT_DID_MAX_EXT_PAYLOAD    122

typedef struct _tagNVT_DISPLAYID_INFO
{
    // Top Level Header Information
    NvU8    version;
    NvU8    product_type;

    // Product Identification (0 or 1 Blocks Allowed)
    NvU32   vendor_id;
    NvU16   product_id;
    NvU32   serial_number;
    NvU8    week;
    NvU8    year;
    NvU8    product_string[NVT_DISPLAYID_PRODUCT_STRING_MAX_LEN + 1];

    // Display Parameters
    NvU16   horiz_size;
    NvU16   vert_size;
    NvU16   horiz_pixels;
    NvU16   vert_pixels;
    NvU8    support_audio      : 1;
    NvU8    separate_audio     : 1;
    NvU8    audio_override     : 1;
    NvU8    power_management   : 1;
    NvU8    fixed_timing       : 1;
    NvU8    fixed_pixel_format : 1;
    NvU8    rsvd4              : 1;
    NvU8    deinterlace        : 1;
    NvU16   gamma;
    NvU8    aspect_ratio;
    NvU8    depth_overall      : 4;
    NvU8    depth_native       : 4;

    // Color Characteristics
    NvU8    total_white_points;
    NvU8    total_primaries    : 3;
    NvU8    temporal           : 1;
    NVT_COLOR_POINT white_points[NVT_DISPLAYID_COLOR_MAX_WHITEPOINTS];
    NVT_COLOR_POINT primaries[NVT_DISPLAYID_COLOR_MAX_PRIMARIES];

    // Range Limits
    NvU8    rl_num;
    NVT_DISPLAYID_RANGE_LIMITS range_limits[NVT_DISPLAYID_RANGE_LIMITS_MAX_COUNT];

    // Display Data
    NvU8    tech_type;
    NvU8    device_op_mode     : 4;
    NvU8    support_backlight  : 1;
    NvU8    support_intensity  : 1;
    NvU8    rsvd1              : 2;
    NvU16   horiz_pixel_count;
    NvU16   vert_pixel_count;
    NvU8    orientation        : 2;
    NvU8    rotation           : 2;
    NvU8    zero_pixel         : 2;
    NvU8    scan_direction     : 2;
    NvU8    subpixel_info;
    NvU8    horiz_pitch;
    NvU8    vert_pitch;
    NvU8    rsvd2              : 4;
    NvU8    color_bit_depth    : 4;
    NvU8    white_to_black     : 1;
    NvU8    response_time      : 7;

    // Power Settings
    NvU8    t1_min             : 4;
    NvU8    t1_max             : 4;
    NvU8    t2_max;
    NvU8    t3_max;
    NvU8    t4_min;
    NvU8    t5_min;
    NvU8    t6_min;

    union
    {
        struct
        {
            NvU8 rsvd          : 3;
            NvU8 color_map     : 1;
            NvU8 support_2_8v  : 1;
            NvU8 support_12v   : 1;
            NvU8 support_5v    : 1;
            NvU8 support_3_3v  : 1;
            NvU8 rsvd2         : 5;
            NvU8 DE_mode       : 1;
            NvU8 polarity      : 1;
            NvU8 data_strobe   : 1;
        } lvds;

        struct
        {
            NvU8 rsvd          : 5;
            NvU8 DE_mode       : 1;
            NvU8 polarity      : 1;
            NvU8 data_strobe   : 1;
        } proprietary;
    } u2;

    // Stereo Interface
    NvU8     stereo_code;
    union
    {
        struct
        {
            NvU8 stereo_polarity;
        } field_sequential;

        struct
        {
            NvU8 view_identity;
        } side_by_side;

        struct
        {
            NvU8 interleave_pattern[8];
        } pixel_interleaved;

        struct
        {
            NvU8 rsvd          : 5;
            NvU8 mirroring     : 2;
            NvU8 polarity      : 1;
        } left_right_separate;

        struct
        {
            NvU8 num_views;
            NvU8 code;
        } multiview;
    } u3;

    NvU32 tiled_display_revision;
    struct
    {
        NvBool bSingleEnclosure;
        NvBool bHasBezelInfo;
        NVT_SINGLE_TILE_BEHAVIOR single_tile_behavior;
        NVT_MULTI_TILE_BEHAVIOR multi_tile_behavior;
    } tile_capability;

    struct
    {
        NvU32 row;
        NvU32 col;
    } tile_topology;

    struct
    {
        NvU32 x;
        NvU32 y;
    } tile_location;

    struct
    {
        NvU32 width;
        NvU32 height;
    } native_resolution;

    struct
    {
        NvU32 pixel_density;
        NvU32 top;
        NvU32 bottom;
        NvU32 right;
        NvU32 left;
    } bezel_info;

    NVT_TILEDDISPLAY_TOPOLOGY_ID tile_topology_id;
    NvU8 cea_data_block_present;

    NvU8 supported_displayId2_0;
    union
    {
        // Display Interface
        struct
        {
            NvU8    interface_type     : 4;
            union
            {
                NvU8 analog_subtype    : 4;
                NvU8 digital_num_links : 4;
            } u1;

            NvU8    interface_version;

            struct
            {
                NvU8 rsvd              : 2;
                NvU8 support_16b       : 1;
                NvU8 support_14b       : 1;
                NvU8 support_12b       : 1;
                NvU8 support_10b       : 1;
                NvU8 support_8b        : 1;
                NvU8 support_6b        : 1;
            } rgb_depth;

            struct
            {
                NvU8 rsvd              : 2;
                NvU8 support_16b       : 1;
                NvU8 support_14b       : 1;
                NvU8 support_12b       : 1;
                NvU8 support_10b       : 1;
                NvU8 support_8b        : 1;
                NvU8 support_6b        : 1;
            } ycbcr444_depth;

            struct
            {
                NvU8 rsvd              : 3;
                NvU8 support_16b       : 1;
                NvU8 support_14b       : 1;
                NvU8 support_12b       : 1;
                NvU8 support_10b       : 1;
                NvU8 support_8b        : 1;
            } ycbcr422_depth;

            NvU8     content_protection;
            NvU8     content_protection_version;
            NvU8     spread_spectrum   : 2;
            NvU8     rsvd3             : 2;
            NvU8     spread_percent    : 4;

        } display_interface;

        //display interface features for DID2.0
        struct
        {
            struct
            {
                NvU8 rsvd              : 2;
                NvU8 support_16b       : 1;
                NvU8 support_14b       : 1;
                NvU8 support_12b       : 1;
                NvU8 support_10b       : 1;
                NvU8 support_8b        : 1;
                NvU8 support_6b        : 1;
            } rgb_depth;

            struct
            {
                NvU8 rsvd              : 2;
                NvU8 support_16b       : 1;
                NvU8 support_14b       : 1;
                NvU8 support_12b       : 1;
                NvU8 support_10b       : 1;
                NvU8 support_8b        : 1;
                NvU8 support_6b        : 1;
            } ycbcr444_depth;

            struct
            {
                NvU8 rsvd              : 3;
                NvU8 support_16b       : 1;
                NvU8 support_14b       : 1;
                NvU8 support_12b       : 1;
                NvU8 support_10b       : 1;
                NvU8 support_8b        : 1;
            } ycbcr422_depth;

            struct
            {
                NvU8 rsvd              : 3;
                NvU8 support_16b       : 1;
                NvU8 support_14b       : 1;
                NvU8 support_12b       : 1;
                NvU8 support_10b       : 1;
                NvU8 support_8b        : 1;
            } ycbcr420_depth;

            // based on the DID2.0 spec. minimum pixel rate at which the Sink device shall support YCbCr 4:2:0 encoding
            NvU8     minimum_pixel_rate_ycbcr420;

            struct
            {
                NvU8 support_32khz     : 1;
                NvU8 support_44_1khz   : 1;
                NvU8 support_48khz     : 1;
                NvU8 rsvd              : 5;
            } audio_capability;

            struct
            {
                NvU8 rsvd              : 1;
                NvU8 support_colorspace_bt2020_eotf_smpte_st2084: 1;
                NvU8 support_colorspace_bt2020_eotf_bt2020      : 1;
                NvU8 support_colorspace_dci_p3_eotf_dci_p3      : 1;
                NvU8 support_colorspace_adobe_rgb_eotf_adobe_rgb: 1;
                NvU8 support_colorspace_bt709_eotf_bt1886       : 1;
                NvU8 support_colorspace_bt601_eotf_bt601        : 1;
                NvU8 support_colorspace_srgb_eotf_srgb          : 1;
            } colorspace_eotf_combination_1;

            struct
            {
                NvU8 rsvd              : 8;
            } colorspace_eotf_combination_2;

            struct
            {
                NvU8 rsvd               : 5;
                NvU8 total              : 3;
            } total_additional_colorspace_eotf;

            struct
            {
                NvU8 support_colorspace     : 4;
                NvU8 support_eotf           : 4;
            } additional_colorspace_eotf[NVT_DISPLAYID_DISPLAY_INTERFACE_FEATURES_MAX_ADDITIONAL_SUPPORTED_COLORSPACE_EOTF];
        } display_interface_features;
    } u4;

} NVT_DISPLAYID_INFO;

//***********************************
// EDID 18-byte display descriptors
//***********************************
//
//
//*** (Tag = 0xFF) ***/
// Display Product Serial Number
#define NVT_EDID_LDD_PAYLOAD_SIZE            13
typedef struct tagNVT_EDID_DD_SERIAL_NUMBER
{
    NvU8 str[NVT_EDID_LDD_PAYLOAD_SIZE];
    NvU8 padding[16 - NVT_EDID_LDD_PAYLOAD_SIZE];
} NVT_EDID_DD_SERIAL_NUMBER;
//
//
//
//*** (Tag = 0xFE) ***/
// Alphanumeric Data String (ASCII)
typedef struct tagNVT_EDID_DD_DATA_STRING
{
    NvU8 str[NVT_EDID_LDD_PAYLOAD_SIZE];
    NvU8 padding[16 - NVT_EDID_LDD_PAYLOAD_SIZE];
} NVT_EDID_DD_DATA_STRING;
//
//
//
//*** (Tag = 0xFD) ***/
// Display Range Limit
//
typedef struct tagNVT_EDID_DD_RANGE_GTF2
{
    NvU8  C;
    NvU8  K;
    NvU8  J;
    NvU16 M;
} NVT_EDID_DD_RANGE_GTF2;

typedef struct tagNVT_EDID_DD_RANGE_CVT
{
    NvU16 max_active_pixels_per_line;

    NvU8  pixel_clock_adjustment        : 2; // this is in 0.25Hz, subtract from max_pixel_clock
                                             // the whole number part (if existing) gets subtracted
                                             // from max_pclk_MHz right away
    NvU8  aspect_supported              : 5;

    NvU8  aspect_preferred              : 3;
    NvU8  blanking_support              : 2;
    NvU8  reserved1                     : 3;

    NvU8  scaling_support               : 4;
    NvU8  reserved2                     : 4;

    NvU8  preferred_refresh_rate;
} NVT_EDID_DD_RANGE_CVT;

typedef struct tagNVT_EDID_DD_RANGE_LIMIT
{
    NvU16 min_v_rate;
    NvU16 max_v_rate;
    NvU16 min_h_rate;
    NvU16 max_h_rate;
    NvU16 max_pclk_MHz;
    NvU8  timing_support;    // indicates 2nd GTF / CVT support
    union
    {
        // if timing_support = 0x02
        NVT_EDID_DD_RANGE_GTF2 gtf2;

        // if timing_support = 0x04
        NVT_EDID_DD_RANGE_CVT  cvt;
    }u;
} NVT_EDID_DD_RANGE_LIMIT;

typedef struct tagNVT_EDID_RANGE_LIMIT
{
    NvU32 min_v_rate_hzx1k;
    NvU32 max_v_rate_hzx1k;
    NvU32 min_h_rate_hz;
    NvU32 max_h_rate_hz;
    NvU32 max_pclk_10khz;
} NVT_EDID_RANGE_LIMIT;

// timing support
#define NVT_EDID_RANGE_SUPPORT_GTF2     0x02
#define NVT_EDID_RANGE_SUPPORT_CVT      0x04

// supported aspect ratios
#define NVT_EDID_CVT_ASPECT_SUPPORT_MAX     5

#define NVT_EDID_CVT_ASPECT_SUPPORT_4X3     0x10
#define NVT_EDID_CVT_ASPECT_SUPPORT_16X9    0x08
#define NVT_EDID_CVT_ASPECT_SUPPORT_16X10   0x04
#define NVT_EDID_CVT_ASPECT_SUPPORT_5X4     0x02
#define NVT_EDID_CVT_ASPECT_SUPPORT_15X9    0x01

// preferred aspect ratios
#define NVT_EDID_CVT_ASPECT_PREFER_4X3      0x00
#define NVT_EDID_CVT_ASPECT_PREFER_16X9     0x01
#define NVT_EDID_CVT_ASPECT_PREFER_16X10    0x02
#define NVT_EDID_CVT_ASPECT_PREFER_5X4      0x03
#define NVT_EDID_CVT_ASPECT_PREFER_15X9     0x04

// cvt blanking support
#define NVT_EDID_CVT_BLANKING_STANDARD      0x01
#define NVT_EDID_CVT_BLANKING_REDUCED       0x02

// scaling support
#define NVT_EDID_CVT_SCALING_HOR_SHRINK     0x08
#define NVT_EDID_CVT_SCALING_HOR_STRETCH    0x04
#define NVT_EDID_CVT_SCALING_VER_SHRINK     0x02
#define NVT_EDID_CVT_SCALING_VER_STRETCH    0x01

//
//
//
//*** (Tag = 0xFC) ***/
// Display Product Name
typedef struct tagNVT_EDID_DD_PRODUCT_NAME
{
    NvU8 str[NVT_EDID_LDD_PAYLOAD_SIZE];
    NvU8 padding[16 - NVT_EDID_LDD_PAYLOAD_SIZE];
} NVT_EDID_DD_PRODUCT_NAME;
//
//
//
//*** (Tag = 0xFB) ***/
// the 18-byte display descriptors
// Display Color Point Data
typedef struct tagNVT_EDID_DD_COLOR_POINT
{
    NvU8  wp1_index;
    NvU16 wp1_x;
    NvU16 wp1_y;
    NvU16 wp1_gamma;
    NvU8  wp2_index;
    NvU16 wp2_x;
    NvU16 wp2_y;
    NvU16 wp2_gamma;
} NVT_EDID_DD_COLOR_POINT;
//
//
//
//*** (Tag = 0xFA) ***/
// Standard Timing Identifications
#define NVT_EDID_DD_STI_NUM    6

typedef struct tagNVT_EDID_DD_STD_TIMING
{
    NvU16 descriptor[NVT_EDID_DD_STI_NUM];
} NVT_EDID_DD_STD_TIMING;
//
//
//
//*** (Tag = 0xF9) ***/
// Display Color Management Data (DCM)
typedef struct tagNVT_EDID_DD_COLOR_MANAGEMENT_DATA
{
    NvU16 red_a3;
    NvU16 red_a2;
    NvU16 green_a3;
    NvU16 green_a2;
    NvU16 blue_a3;
    NvU16 blue_a2;
} NVT_EDID_DD_COLOR_MANAGEMENT_DATA;
//
//
//
//*** (Tag = 0xF8) ***/
// CVT 3 Byte Timing Code
#define NVT_EDID_DD_MAX_CVT3_PER_DESCRITPOR     4

typedef struct tagEDID_DD_CVT_3BYTE_BLOCK
{
    NvU16 addressable_lines     : 14;
    NvU8  aspect_ratio          : 2;
    NvU8  reserved0             : 1;
    NvU8  preferred_vert_rates  : 2;
    NvU8  supported_vert_rates  : 5;

} NVT_EDID_DD_CVT_3BYTE_BLOCK;

typedef struct tagNVT_EDID_DD_CVT_3BYTE
{
    NVT_EDID_DD_CVT_3BYTE_BLOCK block[NVT_EDID_DD_MAX_CVT3_PER_DESCRITPOR];
} NVT_EDID_DD_CVT_3BYTE;

#define NVT_EDID_CVT3_ASPECT_4X3            0x00
#define NVT_EDID_CVT3_ASPECT_16X9           0x01
#define NVT_EDID_CVT3_ASPECT_16X10          0x02
#define NVT_EDID_CVT3_ASPECT_15X9           0x03

#define NVT_EDID_CVT3_PREFFERED_RATE_50HZ   0x00
#define NVT_EDID_CVT3_PREFFERED_RATE_60HZ   0x01
#define NVT_EDID_CVT3_PREFFERED_RATE_75HZ   0x02
#define NVT_EDID_CVT3_PREFFERED_RATE_85HZ   0x03

#define NVT_EDID_CVT3_SUPPORTED_RATE_50HZ   0x10
#define NVT_EDID_CVT3_SUPPORTED_RATE_60HZ   0x08
#define NVT_EDID_CVT3_SUPPORTED_RATE_75HZ   0x04
#define NVT_EDID_CVT3_SUPPORTED_RATE_85HZ   0x02
#define NVT_EDID_CVT3_SUPPORTED_RATE_60HZ_REDUCED_BLANKING  0x01
//
//
//
//*** (Tag = 0xF7) ***/
// Established Timings III
//
#define NVT_EDID_DD_EST_TIMING3_NUM     6

typedef struct tagNVT_EDID_DD_EST_TIMING3
{
    NvU8 revision;
    NvU8 data[NVT_EDID_DD_EST_TIMING3_NUM];
} NVT_EDID_DD_EST_TIMING3;
//
//
//
//*** (Tag = 0x10) ***/
// Dummy Descriptor Definition
typedef struct tagNVT_EDID_DD_DUMMY_DESCRIPTOR
{
    NvU8 data[13];
} NVT_EDID_DD_DUMMY_DESCRIPTOR;
//
//
//
//*** (Tag = 0x00 to 0x0F) ***/
// Manufacturer Special Data
typedef struct tagNVT_EDID_DD_MANUF_DATA
{
    NvU8 data[13];
} NVT_EDID_DD_MANUF_DATA;
//
//
//
// the translated generic 18-byte long descriptor
typedef struct tagNVT_EDID_18BYTE_DESCRIPTOR
{
    NvU8 tag;
    union
    {
        NVT_EDID_DD_SERIAL_NUMBER          serial_number;
        NVT_EDID_DD_DATA_STRING            data_str;
        NVT_EDID_DD_RANGE_LIMIT            range_limit;
        NVT_EDID_DD_PRODUCT_NAME           product_name;
        NVT_EDID_DD_COLOR_POINT            color_point;
        NVT_EDID_DD_STD_TIMING             std_timing;
        NVT_EDID_DD_COLOR_MANAGEMENT_DATA  color_man;
        NVT_EDID_DD_CVT_3BYTE              cvt;
        NVT_EDID_DD_EST_TIMING3            est3;
        NVT_EDID_DD_DUMMY_DESCRIPTOR       dummy;
        NVT_EDID_DD_MANUF_DATA             manuf_data;
    } u;
} NVT_EDID_18BYTE_DESCRIPTOR;
//
//
// Display Descriptor Tags
#define NVT_EDID_DISPLAY_DESCRIPTOR_DPSN     0xFF   // display product serial number
#define NVT_EDID_DISPLAY_DESCRIPTOR_ADS      0xFE   // alphanumeric data string (ASCII)
#define NVT_EDID_DISPLAY_DESCRIPTOR_DRL      0xFD   // display range limit
#define NVT_EDID_DISPLAY_DESCRITPOR_DPN      0xFC   // display product name
#define NVT_EDID_DISPLAY_DESCRIPTOR_CPD      0xFB   // color point data
#define NVT_EDID_DISPLAY_DESCRIPTOR_STI      0xFA   // standard timing identification
#define NVT_EDID_DISPLAY_DESCRIPTOR_DCM      0xF9   // display color management
#define NVT_EDID_DISPLAY_DESCRIPTOR_CVT      0xF8   // CVT 3-byte timing code
#define NVT_EDID_DISPLAY_DESCRIPTOR_ESTIII   0xF7   // establishied timing III
#define NVT_EDID_DISPLAY_DESCRIPTOR_DUMMY    0x10   // dummy descriptor

//*******************
// Raw EDID offsets and info
//*******************
//
// Byte 14, video input definition
//
#define NVT_EDID_DIGITAL_VIDEO_INTERFACE_STANDARD_MASK  0x0F    // dvi/hdmi/dp
#define NVT_EDID_VIDEO_COLOR_BIT_DEPTH_MASK             0x70    // bpc support
#define NVT_EDID_VIDEO_INPUT_DEFINITION_DIGITAL_MASK    0x80    // digital/analog
//
#define NVT_EDID_DIGITAL_VIDEO_INTERFACE_STANDARD_SHIFT 0
#define NVT_EDID_VIDEO_COLOR_BIT_DEPTH_SHIFT            4
#define NVT_EDID_VIDEO_INPUT_DEFINITION_DIGITAL_SHIFT   7
//
#define NVT_EDID_DIGITAL_VIDEO_INTERFACE_STANDARD_UNDEFINED             0
#define NVT_EDID_DIGITAL_VIDEO_INTERFACE_STANDARD_DVI_SUPPORTED         1
#define NVT_EDID_DIGITAL_VIDEO_INTERFACE_STANDARD_HDMI_A_SUPPORTED      2
#define NVT_EDID_DIGITAL_VIDEO_INTERFACE_STANDARD_HDMI_B_SUPPORTED      3
#define NVT_EDID_DIGITAL_VIDEO_INTERFACE_STANDARD_MDDI_SUPPORTED        4
#define NVT_EDID_DIGITAL_VIDEO_INTERFACE_STANDARD_DISPLAYPORT_SUPPORTED 5
//#define NVT_EDID_DIGITAL_VIDEO_INTERFACE_STANDARD_RESERVED              6 - 15
#define NVT_EDID_VIDEO_COLOR_BIT_DEPTH_UNDEFINED        0
#define NVT_EDID_VIDEO_COLOR_BIT_DEPTH_6BPC             1
#define NVT_EDID_VIDEO_COLOR_BIT_DEPTH_8BPC             2
#define NVT_EDID_VIDEO_COLOR_BIT_DEPTH_10BPC            3
#define NVT_EDID_VIDEO_COLOR_BIT_DEPTH_12BPC            4
#define NVT_EDID_VIDEO_COLOR_BIT_DEPTH_14BPC            5
#define NVT_EDID_VIDEO_COLOR_BIT_DEPTH_16BPC            6
//#define NVT_EDID_VIDEO_COLOR_BIT_DEPTH_RESERVED         7
#define NVT_EDID_VIDEO_INPUT_DEFINITION_DIGITAL         0x01
//
// Byte 18, feature support
//
#define NVT_EDID_OTHER_FEATURES_MASK                    0x07    // sRGB space, preferred timing, continuous freq.
#define NVT_EDID_DISPLAY_COLOR_TYPE_MASK                0x18    // for analog, see byte 14, bit 7
#define NVT_EDID_DISPLAY_COLOR_ENCODING_MASK            0x18    // for digital
#define NVT_EDID_DISPLAY_POWER_MANAGEMENT_MASK          0xE0    // standby/suspend/active off
//
#define NVT_EDID_OTHER_FEATURES_SHIFT                   0
#define NVT_EDID_DISPLAY_COLOR_TYPE_SHIFT               3
#define NVT_EDID_DISPLAY_COLOR_ENCODING_SHIFT           3
#define NVT_EDID_DISPLAY_POWER_MANAGEMENT_SHIFT         5
//
#define NVT_EDID_OTHER_FEATURES_USES_CONTINUOUS_FREQ                (1 << 0)
#define NVT_EDID_OTHER_FEATURES_PTM_INCLUDE_NATIVE                  (1 << 1)
#define NVT_EDID_OTHER_FEATURES_SRGB_DEFAULT_COLORSPACE             (1 << 2)
//
#define NVT_EDID_DISPLAY_COLOR_TYPE_MONOCHROME                      0
#define NVT_EDID_DISPLAY_COLOR_TYPE_RGB                             1
#define NVT_EDID_DISPLAY_COLOR_TYPE_NON_RGB                         2
#define NVT_EDID_DISPLAY_COLOR_TYPE_UNDEFINED                       3
//
#define NVT_EDID_DISPLAY_COLOR_ENCODING_YCBCR_444                   (1 << 0)    // RGB is always supported
#define NVT_EDID_DISPLAY_COLOR_ENCODING_YCBCR_422                   (1 << 1)     // RGB is always supported
//
#define NVT_EDID_DISPLAY_POWER_MANAGEMENT_SUPPORTS_ACTIVE_OFF       (1 << 0)
#define NVT_EDID_DISPLAY_POWER_MANAGEMENT_SUPPORTS_SUSPENDED_MODE   (1 << 1)
#define NVT_EDID_DISPLAY_POWER_MANAGEMENT_SUPPORTS_STANDBY_MODE     (1 << 2)
//
// edid offsets
//
#define NVT_EDID_VIDEO_INPUT_DEFINITION                 0x14
#define NVT_EDID_FEATURE_SUPPORT                        0x18


//*******************
// Parsed EDID info
//*******************
//
#define NVT_EDID_MAX_LONG_DISPLAY_DESCRIPTOR 4
#define NVT_EDID_MAX_STANDARD_TIMINGS        8
#define NVT_EDID_MAX_TOTAL_TIMING            NVT_MAX_TOTAL_TIMING
#define NVT_EDID_VER_1_1                     0x101
#define NVT_EDID_VER_1_2                     0x102
#define NVT_EDID_VER_1_3                     0x103
#define NVT_EDID_VER_1_4                     0x104
//
// byte 0x14, Digital
//    bits 0-3
#define NVT_EDID_VIDEOSIGNAL_INTERFACE_NOT_DEFINED  0x0
#define NVT_EDID_VIDEOSIGNAL_INTERFACE_DVI          0x1
#define NVT_EDID_VIDEOSIGNAL_INTERFACE_HDMI_A       0x2
#define NVT_EDID_VIDEOSIGNAL_INTERFACE_HDMI_B       0x3
#define NVT_EDID_VIDEOSIGNAL_INTERFACE_MDDI         0x4
#define NVT_EDID_VIDEOSIGNAL_INTERFACE_DP           0x5
//    bits 4-6; these are translated values. See NvTiming_ParseEDIDInfo()
#define NVT_EDID_VIDEOSIGNAL_BPC_NOT_DEFINED        0
#define NVT_EDID_VIDEOSIGNAL_BPC_6                  6
#define NVT_EDID_VIDEOSIGNAL_BPC_8                  8
#define NVT_EDID_VIDEOSIGNAL_BPC_10                 10
#define NVT_EDID_VIDEOSIGNAL_BPC_12                 12
#define NVT_EDID_VIDEOSIGNAL_BPC_14                 14
#define NVT_EDID_VIDEOSIGNAL_BPC_16                 16
//
// byte 0x18, edid 1.3
//    bits 3-4
#define NVT_EDID_FEATURESUPPORT_COLOR_MONOCHROME          0x0 /* Monochrome/grayscale display */
#define NVT_EDID_FEATURESUPPORT_COLOR_RGB                 0x1 /* R/G/B color display */
#define NVT_EDID_FEATURESUPPORT_COLOR_MULTICOLOR          0x2 /* non R/G/B multicolor displays e.g. R/G/Y */
#define NVT_EDID_FEATURESUPPORT_COLOR_UNDEFINED           0x3 /* Undefined  */
//
// byte 0x18, edid 1.4
//    bits 3-4
#define NVT_EDID_FEATURESUPPORT_COLOR_ENCODING_RBG          0x0 /* RGB always supported */
#define NVT_EDID_FEATURESUPPORT_COLOR_ENCODING_YCRCB444     0x1 /* RGB + 444 */
#define NVT_EDID_FEATURESUPPORT_COLOR_ENCODING_YCRCB422     0x2 /* RGB + 422 */
#define NVT_EDID_FEATURESUPPORT_COLOR_ENCODING_YCRCB        0x3 /* RGB + 444 + 422 supported  */
//
//
// structure used internally to map support for HDMI 3D modes.
#define MAX_EDID_ADDRESSABLE_3D_VICS 16
#define MAX_3D_VICS_RESERVED_FOR_MANDATORY 8
#define MAX_3D_VICS_SUPPORTED (MAX_EDID_ADDRESSABLE_3D_VICS + MAX_3D_VICS_RESERVED_FOR_MANDATORY)

//Constants given by Dolby to be appended for chromaticity information
#define NVT_DOLBY_CHROMATICITY_MSB_BX 0x20
#define NVT_DOLBY_CHROMATICITY_MSB_BY 0x08
#define NVT_DOLBY_CHROMATICITY_MSB_GX 0x00
#define NVT_DOLBY_CHROMATICITY_MSB_GY 0x80
#define NVT_DOLBY_CHROMATICITY_MSB_RX 0xA0
#define NVT_DOLBY_CHROMATICITY_MSB_RY 0x40

typedef struct _HDMI3DDetails
{
    NvU8    Vic;
    NvU16   StereoStructureMask;
    NvU8    SideBySideHalfDetail;
} HDMI3DDETAILS;

typedef struct _SupportMap
{
    HDMI3DDETAILS map[MAX_3D_VICS_SUPPORTED];
    NvU32 total;
} HDMI3DSUPPORTMAP;

typedef struct tagNVT_EXT_TIMING
{
        NVT_TIMING timing;
        NVT_HDMIEXT HDMI3D;
} NVT_EXT_TIMING;

typedef struct _NVDA_VSDB_PARSED_INFO
{
    NvBool valid;
    NvU8 vsdbVersion;

    // these fields are specified in version 1 of the NVDA VSDB
    union
    {
        struct
        {
            NvBool supportsVrr;
            NvU8   minRefreshRate;
        } v1;
    } vrrData;

} NVDA_VSDB_PARSED_INFO;

typedef enum _MSFT_VSDB_DESKTOP_USAGE
{
    MSFT_VSDB_NOT_USABLE_BY_DESKTOP = 0,
    MSFT_VSDB_USABLE_BY_DESKTOP = 1
} MSFT_VSDB_DESKTOP_USAGE;

typedef enum _MSFT_VSDB_THIRD_PARTY_USAGE
{
    MSFT_VSDB_NOT_USABLE_BY_THIRD_PARTY = 0,
    MSFT_VSDB_USABLE_BY_THIRD_PARTY = 1
} MSFT_VSDB_THIRD_PARTY_USAGE;

typedef enum _MSFT_VSDB_PRIMARY_USE_CASE
{
    MSFT_VSDB_FOR_UNDEFINED = 0,
    MSFT_VSDB_FOR_TEST_EQUIPMENT = 0x1,
    MSFT_VSDB_FOR_GENERIC_DISPLAY = 0x2,
    MSFT_VSDB_FOR_TELEVISION_DISPLAY = 0x3,
    MSFT_VSDB_FOR_DESKTOP_PRODUCTIVITY_DISPLAY = 0x4,
    MSFT_VSDB_FOR_DESKTOP_GAMING_DISPLAY = 0x5,
    MSFT_VSDB_FOR_PRESENTATION_DISPLAY = 0x6,
    MSFT_VSDB_FOR_VIRTUAL_REALITY_HEADSETS = 0x7,
    MSFT_VSDB_FOR_AUGMENTED_REALITY = 0x8,
    MSFT_VSDB_FOR_VIDEO_WALL_DISPLAY = 0x10,
    MSFT_VSDB_FOR_MEDICAL_IMAGING_DISPLAY = 0x11,
    MSFT_VSDB_FOR_DEDICATED_GAMING_DISPLAY = 0x12,
    MSFT_VSDB_FOR_DEDICATED_VIDEO_MONITOR_DISPLAY = 0x13,
    MSFT_VSDB_FOR_ACCESSORY_DISPLAY = 0X14
} MSFT_VSDB_PRIMARY_USE_CASE;

#define MSFT_VSDB_CONTAINER_ID_SIZE     (16)
#define MSFT_VSDB_MAX_VERSION_SUPPORT   (3)

typedef struct _MSFT_VSDB_PARSED_INFO
{
    NvBool valid;
    NvU8   version;

    MSFT_VSDB_DESKTOP_USAGE         desktopUsage;
    MSFT_VSDB_THIRD_PARTY_USAGE     thirdPartyUsage;
    MSFT_VSDB_PRIMARY_USE_CASE      primaryUseCase;
    NvU8                            containerId[MSFT_VSDB_CONTAINER_ID_SIZE];

} MSFT_VSDB_PARSED_INFO;

typedef struct tagNVT_HDMI_LLC_INFO
{
    // A.B.C.D address
    NvU8 addrA;
    NvU8 addrB;
    NvU8 addrC;
    NvU8 addrD;

    NvU8 supports_AI             : 1;
    NvU8 dc_48_bit               : 1;
    NvU8 dc_36_bit               : 1;
    NvU8 dc_30_bit               : 1;
    NvU8 dc_y444                 : 1;
    NvU8 dual_dvi                : 1;
    NvU8 max_tmds_clock;
    NvU8 effective_tmds_clock;
    NvU8 latency_field_present   : 1;
    NvU8 i_latency_field_present : 1;
    NvU8 hdmi_video_present      : 1;
    NvU8 cnc3                    : 1;
    NvU8 cnc2                    : 1;
    NvU8 cnc1                    : 1;
    NvU8 cnc0                    : 1;
    NvU8 video_latency;
    NvU8 audio_latency;
    NvU8 interlaced_video_latency;
    NvU8 interlaced_audio_latency;
    NvU8 threeD_present          : 1;
    NvU8 threeD_multi_present    : 2;
    NvU8 image_size              : 2;
    NvU8 hdmi_vic_len            : 3;
    NvU8 hdmi_3d_len             : 5;
    // for now ignoring the other extensions
    // ....
} NVT_HDMI_LLC_INFO;

typedef struct tagNVT_HDMI_FORUM_INFO
{
    NvU8 max_TMDS_char_rate;

    NvU8 threeD_Osd_Disparity       :  1;
    NvU8 dual_view                  :  1;
    NvU8 independent_View           :  1;
    NvU8 lte_340Mcsc_scramble       :  1;
    NvU8 ccbpci                     :  1;
    NvU8 cable_status               :  1;
    NvU8 rr_capable                 :  1;
    NvU8 scdc_present               :  1;

    NvU8 dc_30bit_420               :  1;
    NvU8 dc_36bit_420               :  1;
    NvU8 dc_48bit_420               :  1;
    NvU8 uhd_vic                    :  1;
    NvU8 max_FRL_Rate               :  4;

    NvU8 fapa_start_location        :  1;
    NvU8 allm                       :  1;
    NvU8 fva                        :  1;
    NvU8 cnmvrr                     :  1;
    NvU8 cinemaVrr                  :  1;
    NvU8 m_delta                    :  1;
    NvU8 qms                        :  1;
    NvU8 fapa_end_extended          :  1;

    NvU16 vrr_min                   :  6;
    NvU16 vrr_max                   : 10;

    NvU8  qms_tfr_min               :  1;
    NvU8  qms_tfr_max               :  1;
    NvU16 dsc_MaxSlices             :  6;
    NvU16 dsc_MaxPclkPerSliceMHz    : 10;

    NvU8 dsc_10bpc                  :  1;
    NvU8 dsc_12bpc                  :  1;
    NvU8 dsc_16bpc                  :  1;
    NvU8 dsc_All_bpp                :  1;
    NvU8 dsc_Max_FRL_Rate           :  4;

    NvU8 dsc_Native_420             :  1;
    NvU8 dsc_1p2                    :  1;
    NvU8 rsvd_2                     :  6;

    NvU8 dsc_totalChunkKBytes       :  7; // = 1 + EDID reported DSC_TotalChunkKBytes
    NvU8 rsvd_3                     :  1;

} NVT_HDMI_FORUM_INFO;

typedef struct tagNVT_HDR_STATIC_METADATA
{
    struct
    {
        NvU8 trad_gamma_sdr_eotf : 1;
        NvU8 trad_gamma_hdr_eotf : 1;
        NvU8 smpte_st_2084_eotf : 1;
        NvU8 future_eotf : 1;
    } supported_eotf;

    NvU8 static_metadata_type;      // set to 1 if the sink support for static meta data type 1
    NvU8 max_cll;                   // maximum luminance level value
    NvU8 max_fall;                  // maximum fram-average luminance
    NvU8 min_cll;                   // minimum luminance level value

}NVT_HDR_STATIC_METADATA;

typedef struct tagNVT_DV_STATIC_METADATA
{
    NvU32 ieee_id                     : 24;
    NvU32 VSVDB_version               : 3;
    NvU32 dm_version                  : 8;
    NvU32 supports_2160p60hz          : 1;
    NvU32 supports_YUV422_12bit       : 1;
    NvU32 supports_global_dimming     : 1;
    NvU32 colorimetry                 : 1;
    NvU32 target_min_luminance        : 12;
    NvU32 target_max_luminance        : 12;
    NvU32 cc_red_x                    : 12;
    NvU32 cc_red_y                    : 12;
    NvU32 cc_green_x                  : 12;
    NvU32 cc_green_y                  : 12;
    NvU32 cc_blue_x                   : 12;
    NvU32 cc_blue_y                   : 12;
    NvU32 cc_white_x                  : 12;
    NvU32 cc_white_y                  : 12;
    NvU32 supports_backlight_control  : 2;
    NvU32 backlt_min_luma             : 2;
    NvU32 interface_supported_by_sink : 2;
    NvU32 supports_10b_12b_444        : 2;
    NvU32 parity                      : 1;
}NVT_DV_STATIC_METADATA;

//***********************************
// parsed DisplayID 2.0 definitions
//***********************************
#define NVT_DISPLAYID_2_0_PRODUCT_STRING_MAX_LEN      236

// the basic info encoded in byte[3]
#define NVT_DISPLAY_2_0_CAP_BASIC_AUDIO           0x40 // DTV monitor supports basic audio
#define NVT_DISPLAY_2_0_CAP_YCbCr_444             0x20 // DTV monitor supports YCbCr4:4:4
#define NVT_DISPLAY_2_0_CAP_YCbCr_422             0x10 // DTV monitor supports YCbCr4:2:2

// vendor specific
#define NVT_VESA_VENDOR_SPECIFIC_IEEE_ID                 0x3A0292
#define NVT_VESA_VENDOR_SPECIFIC_LENGTH                  7

#define NVT_VESA_ORG_VSDB_DATA_TYPE_MASK                 0x07
#define NVT_VESA_ORG_VSDB_COLOR_SPACE_AND_EOTF_MASK      0x80
#define NVT_VESA_ORG_VSDB_COLOR_SPACE_AND_EOTF_SHIFT     7
#define NVT_VESA_ORG_VSDB_PIXELS_OVERLAPPING_MASK        0x0F
#define NVT_VESA_ORG_VSDB_MULTI_SST_MODE_MASK            0x60
#define NVT_VESA_ORG_VSDB_MULTI_SST_MODE_SHIFT           5
#define NVT_VESA_ORG_VSDB_PASS_THROUGH_INTEGER_MASK      0x3F
#define NVT_VESA_ORG_VSDB_PASS_THROUGH_FRACTIOINAL_MASK  0x0F

// adaptive-sync
#define NVT_ADAPTIVE_SYNC_DESCRIPTOR_MAX_COUNT           0x04

typedef enum _tagNVT_DISPLAYID_PRODUCT_PRIMARY_USE_CASE
{
    PRODUCT_PRIMARY_USE_TEST_EQUIPMENT               = 1,
    PRODUCT_PRIMARY_USE_GENERIC_DISPLAY              = 2,
    PRODUCT_PRIMARY_USE_TELEVISION                   = 3,
    PRODUCT_PRIMARY_USE_DESKTOP_PRODUCTIVITY         = 4,
    PRODUCT_PRIMARY_USE_DESKTOP_GAMING               = 5,
    PRODUCT_PRIMARY_USE_PRESENTATION                 = 6,
    PRODUCT_PRIMARY_USE_HEAD_MOUNT_VIRTUAL_REALITY   = 7,
    PRODUCT_PRIMARY_USE_HEAD_MOUNT_AUGMENTED_REALITY = 8,
} NVT_DISPLAYID_PRODUCT_PRIMARY_USE_CASE;

typedef enum _tagNVT_DISPLAYID_SCAN_ORIENTATION
{
    SCAN_ORIENTATION_LRTB          = 0,
    SCAN_ORIENTATION_RLTB          = 1,
    SCAN_ORIENTATION_TBRL          = 2,
    SCAN_ORIENTATION_BTRL          = 3,
    SCAN_ORIENTATION_RLBT          = 4,
    SCAN_ORIENTATION_LRBT          = 5,
    SCAN_ORIENTATION_BTLR          = 6,
    SCAN_ORIENTATION_TBLR          = 7,
} NVT_DISPLAYID_SCAN_ORIENTATION;

typedef enum _tagNVT_DISPLAYID_INTERFACE_EOTF
{
    INTERFACE_EOTF_NOT_DEFINED     = 0x0,
    INTERFACE_EOTF_SRGB            = 0x1,
    INTERFACE_EOTF_BT601           = 0x2,
    INTERFACE_EOTF_BT1886          = 0x3,
    INTERFACE_EOTF_ADOBE_RGB       = 0x4,
    INTERFACE_EOTF_DCI_P3          = 0x5,
    INTERFACE_EOTF_BT2020          = 0x6,
    INTERFACE_EOTF_NATIVE_GAMMA    = 0x7,
    INTERFACE_EOTF_SMPTE_ST2084    = 0x8,
    INTERFACE_EOTF_HYBRID_LOG      = 0x9,
    INTERFACE_EOTF_CUSTOM          = 0x10,
} NVT_DISPLAYID_INTERFACE_EOTF;

typedef enum _tagNVT_DISPLAYID_INTERFACE_COLOR_SPACE
{
    INTERFACE_COLOR_SPACE_NOT_DEFINED = 0x0,
    INTERFACE_COLOR_SPACE_SRGB        = 0x1,
    INTERFACE_COLOR_SPACE_BT601       = 0x2,
    INTERFACE_COLOR_SPACE_BT709       = 0x3,
    INTERFACE_COLOR_SPACE_ADOBE_RGB   = 0x4,
    INTERFACE_COLOR_SPACE_DCI_P3      = 0x5,
    INTERFACE_COLOR_SPACE_BT2020      = 0x6,
    INTERFACE_COLOR_SPACE_CUSTOM      = 0x7,
} NVT_DISPLAYID_INTERFACE_COLOR_SPACE;

typedef enum _tagNVT_DISPLAYID_DEVICE_TECHNOLOGY
{
    DEVICE_TECHNOLOGY_NOT_SPECIFIED,
    DEVICE_TECHNOLOGY_LCD,
    DEVICE_TECHNOLOGY_OLED,
} NVT_DISPLAYID_DEVICE_TECHNOLOGY;

typedef struct _tagNVT_DISPLAYID_TILED_DISPLAY_TOPOLOGY
{
    NvU32 revision;

    struct
    {
         NvBool bSingleEnclosure;
         NvBool bHasBezelInfo;
         NVT_SINGLE_TILE_BEHAVIOR single_tile_behavior;
         NVT_MULTI_TILE_BEHAVIOR multi_tile_behavior;
    } capability;

    struct
    {
        NvU32 row;
        NvU32 col;
    } topology;

    struct
    {
        NvU32 x;
        NvU32 y;
    } location;

    struct
    {
        NvU32 width;
        NvU32 height;
    } native_resolution;

    struct
    {
        NvU32 top;              // Top bezel in pixels
        NvU32 bottom;           // Bottom bezel in pixels
        NvU32 right;            // Right bezel in pixels
        NvU32 left;             // Left bezel in pixels
    } bezel_info;

    NVT_TILEDDISPLAY_TOPOLOGY_ID tile_topology_id;
} NVT_DISPLAYID_TILED_DISPLAY_TOPOLOGY;

typedef struct _tagNVT_DISPLAYID_CONTAINERID
{
    NvU32 revision;
    NvU32 data1;
    NvU16 data2;
    NvU16 data3;
    NvU16 data4;
    NvU8  data5[6];
} NVT_DISPLAYID_CONTAINERID;

typedef struct _tagNVT_DISPLAYID_INTERFACE_FEATURES
{
    NvU32 revision;

    NVT_COLORDEPTH   rgb444;        // each bit within is set if rgb444 supported on that bpc
    NVT_COLORDEPTH   yuv444;        // each bit within is set if yuv444 supported on that bpc
    NVT_COLORDEPTH   yuv422;        // each bit within is set if yuv422 supported on that bpc
    NVT_COLORDEPTH   yuv420;        // each bit within is set if yuv420 supported on that bpc

    NvU32            yuv420_min_pclk;

    struct
    {
        NvU8 support_32khz     : 1;
        NvU8 support_44_1khz   : 1;
        NvU8 support_48khz     : 1;
        NvU8 rsvd              : 5;
    } audio_capability;

    NvU32     combination_count;
    struct
    {
        NVT_DISPLAYID_INTERFACE_EOTF         eotf;
        NVT_DISPLAYID_INTERFACE_COLOR_SPACE  color_space;
    } colorspace_eotf_combination[NVT_DISPLAYID_DISPLAY_INTERFACE_FEATURES_MAX_ADDITIONAL_SUPPORTED_COLORSPACE_EOTF + 1];

} NVT_DISPLAYID_INTERFACE_FEATURES;

typedef struct _tagNVT_DISPLAYID_PRODUCT_IDENTITY
{
    NvU32   revision;
    NvU32   vendor_id;
    NvU16   product_id;
    NvU32   serial_number;
    NvU16   week;
    NvU16   year;
    NvU8    product_string[NVT_DISPLAYID_2_0_PRODUCT_STRING_MAX_LEN + 1];
} NVT_DISPLAYID_PRODUCT_IDENTITY;

typedef enum _tagNVT_COLOR_MAP_STANDARD
{
    COLOR_MAP_CIE_1931,
    COLOR_MAP_CIE_1976,
} NVT_COLOR_MAP_STANDARD;

typedef enum _tagNVT_AUDIO_SPEAKER_INTEGRATED
{
    AUDIO_SPEAKER_INTEGRATED_SUPPORTED     = 0,
    AUDIO_SPEAKER_INTEGRATED_NOT_SUPPORTED = 1,
} NVT_AUDIO_SPEAKER_INTEGRATED;

typedef enum _tagNVT_NATIVE_LUMINANCE_INFO
{
    NATIVE_LUMINANCE_INFO_MIN_GURANTEE_VALUE = 0,
    NATIVE_LUMINANCE_INFO_SOURCE_DEVICE_GUIDANCE = 1,
} NVT_NATIVE_LUMINANCE_INFO;

typedef struct _tagNVT_DISPLAYID_DISPLAY_PARAMETERS
{
    NvU32                           revision;
    NvU32                           h_image_size_micro_meter;
    NvU32                           v_image_size_micro_meter;
    NvU16                           h_pixels;
    NvU16                           v_pixels;
    NVT_DISPLAYID_SCAN_ORIENTATION  scan_orientation;
    NVT_COLOR_MAP_STANDARD          color_map_standard;
    NVT_COLOR_POINT                 primaries[3];
    NVT_COLOR_POINT                 white;
    NVT_NATIVE_LUMINANCE_INFO       native_luminance_info;
    NvU16                           native_max_luminance_full_coverage;
    NvU16                           native_max_luminance_10_percent_rect_coverage;
    NvU16                           native_min_luminance;
    NVT_COLORDEPTH                  native_color_depth;
    NvU16                           gamma_x100;
    NVT_DISPLAYID_DEVICE_TECHNOLOGY device_technology;
    NvBool                          device_theme_Preference;
    NvBool                          audio_speakers_integrated;
} NVT_DISPLAYID_DISPLAY_PARAMETERS;

typedef struct _tagNVT_DISPLAYID_ADAPTIVE_SYNC
{
    union
    {
        NvU8 operation_range_info;
        struct
        {
            NvU8  adaptive_sync_range       : 1;
            NvU8  duration_inc_flicker_perf : 1;
            NvU8  modes                     : 2;
            NvU8  seamless_not_support      : 1;
            NvU8  duration_dec_flicker_perf : 1;
            NvU8  reserved                  : 2;
        } information;
    } u;

    NvU8  max_duration_inc;
    NvU8  min_rr;
    NvU16 max_rr;
    NvU8  max_duration_dec;
} NVT_DISPLAYID_ADAPTIVE_SYNC;

typedef struct _tagVESA_VSDB_PARSED_INFO
{
    struct
    {
        NvU8 type                      : 3;
        NvU8 reserved                  : 4;
        NvU8 color_space_and_eotf      : 1;
    } data_struct_type;

    struct
    {
        NvU8 pixels_overlapping_count  : 4;
        NvU8 reserved_0                : 1;
        NvU8 multi_sst                 : 2;
        NvU8 reserved_1                : 1;
    } overlapping;

    struct
    {
        NvU8 pass_through_integer_dsc  : 6;
        NvU8 reserved                  : 2;
    } pass_through_integer;

    struct
    {
        NvU8 pass_through_fraction_dsc : 4;
        NvU8 reserved                  : 4;
    } pass_through_fractional;
} VESA_VSDB_PARSED_INFO;

typedef struct _tagNVT_DISPLAYID_VENDOR_SPECIFIC
{
    NVT_HDMI_LLC_INFO              hdmiLlc;
    NVT_HDMI_FORUM_INFO            hfvs;
    NVDA_VSDB_PARSED_INFO          nvVsdb;
    MSFT_VSDB_PARSED_INFO          msftVsdb;
    VESA_VSDB_PARSED_INFO          vesaVsdb;
} NVT_DISPLAYID_VENDOR_SPECIFIC;

typedef struct _tagNVT_DISPLAYID_CTA
{
    NVT_EDID_CEA861_INFO           cta861_info;
    NVT_HDR_STATIC_METADATA        hdrInfo;
    NVT_DV_STATIC_METADATA         dvInfo;
    NVT_HDR10PLUS_INFO             hdr10PlusInfo;
} NVT_DISPLAYID_CTA;

typedef struct _tagNVT_DISPLAYID_BRIGHTNESS_LUMINANCE_RANGE
{
    NvU32   revision;
    NvU16   min_sdr_luminance;
    NvU16   max_sdr_luminance;
    NvU16   max_boost_sdr_luminance;
} NVT_DISPLAYID_BRIGHTNESS_LUMINANCE_RANGE;

typedef struct _tagNVT_VALID_DATA_BLOCKS
{
    NvBool product_id_present;
    NvBool parameters_present;
    NvBool type7Timing_present;
    NvBool type8Timing_present;
    NvBool type9Timing_present;
    NvBool dynamic_range_limit_present;
    NvBool interface_feature_present;
    NvBool stereo_interface_present;
    NvBool tiled_display_present;
    NvBool container_id_present;
    NvBool type10Timing_present;
    NvBool adaptive_sync_present;
    NvBool arvr_hmd_present;
    NvBool arvr_layer_present;
    NvBool brightness_luminance_range_present;
    NvBool vendor_specific_present;
    NvBool cta_data_present;
} NVT_VALID_DATA_BLOCKS;

#define NVT_DISPLAYID_MAX_TOTAL_TIMING            NVT_MAX_TOTAL_TIMING
typedef struct _tagNVT_DISPLAYID_2_0_INFO
{
    NvU8                                      revision;
    NvU8                                      version;

    // support audio/yuv444/yuv422 color for CTA861 compatible
    NvU8                                      basic_caps;

    // the all extensions that may appear following the base section
    NvU32                                     extension_count;

    // this displayID20 is EDID extension or not
    NvBool                                    as_edid_extension;

    // data blocks present or not
    NVT_VALID_DATA_BLOCKS                     valid_data_blocks;

    NVT_DISPLAYID_PRODUCT_PRIMARY_USE_CASE    primary_use_case;

    // Product Identification Data Block (Mandatory)
    NVT_DISPLAYID_PRODUCT_IDENTITY            product_identity;

    // Display Parameter Data Block (Mandatory for Display Use)
    NVT_DISPLAYID_DISPLAY_PARAMETERS          display_param;

    // Detailed Timing Data Block (Mandatory for Display Use)
    NvU32                                     total_timings;
    NVT_TIMING                                timing[NVT_DISPLAYID_MAX_TOTAL_TIMING];

    // Enumerated Timing Code Data Block (Not Mandatory)

    // Formula-based Timing Data Block (Not Mandatory)

    // Dynamic Video Timing Range Limits Data Block (Not Mandatory)
    NVT_DISPLAYID_RANGE_LIMITS                range_limits;

    // Display Interface Features Data Block (Mandatory)
    NVT_DISPLAYID_INTERFACE_FEATURES          interface_features;

    // Stereo Display Interface Data Block (Not Mandatory)

    // Tiled Display Topology Data Block (Not Mandatory)
    NVT_DISPLAYID_TILED_DISPLAY_TOPOLOGY      tile_topo;

    // ContainerID Data Block (Mandatory for Multi-function Device)
    NVT_DISPLAYID_CONTAINERID                 container_id;

    // Adaptive-Sync Data Block (Mandatory for display device supports Adaptive-Sync)
    NvU32                                     total_adaptive_sync_descriptor;
    NVT_DISPLAYID_ADAPTIVE_SYNC               adaptive_sync_descriptor[NVT_ADAPTIVE_SYNC_DESCRIPTOR_MAX_COUNT];

    // Brightness Luminance Range Data Block (Mandatory for display device supports Nits based brightness control)
    NVT_DISPLAYID_BRIGHTNESS_LUMINANCE_RANGE  luminance_ranges;

    // Vendor-specific Data Block (Not Mandatory)
    NVT_DISPLAYID_VENDOR_SPECIFIC             vendor_specific;

    // CTA DisplayID Data Block (Not Mandatory)
    NVT_DISPLAYID_CTA                         cta;
} NVT_DISPLAYID_2_0_INFO;

#define NVT_EDID_PRIMARY_COLOR_FP2INT_FACTOR 1024  // Per EDID 1.4, 10bit color primary is encoded in floating point as (bit9/2 + bit8/4 + bi7/8 + ... + bit0)
typedef struct tagNVT_EDID_INFO
{
    // generic edid info
    NvU32  version;
    NvU16  manuf_id;
    NvU16  manuf_id_hi;
    NvU8   manuf_name[4];
    NvU16  product_id;
    NvU32  serial_number;
    NvU8   week;
    NvU16  year;

    // the interface info
    struct
    {
       union
       {
           struct
           {
               NvU8 serrations          : 1;
               NvU8 sync_type           : 3;
               NvU8 video_setup         : 1;
               NvU8 vp_p                : 2;
           } analog;
           struct
           {
              NvU8 video_interface      : 4;
              NvU8 bpc                  : 5;
           } digital;
           NvU8 analog_data             : 7;
       } u;
       NvU8 isDigital                   : 1;
    } input;

    // the screen size info
    NvU8   screen_size_x;           // horizontal screen size in cm
    NvU8   screen_size_y;           // verical screen size in cm
    NvU16  screen_aspect_x;         // aspect ratio
    NvU16  screen_aspect_y;         // aspect ratio

    // display transfer characteristics
    NvU16  gamma;

    // features support
    union
    {
        NvU8   feature;
        struct
        {
            NvU8  support_gtf                   : 1;
            NvU8  preferred_timing_is_native    : 1; // should be "Preferred_timing_is_dtd1". To be exact, "Native" is referenced as the native HDTV timing by CEA861 extension block
            NvU8  default_colorspace_srgb       : 1;
            NvU8  color_type                    : 2;
            NvU8  support_active_off            : 1;
            NvU8  support_suspend               : 1;
            NvU8  support_standby               : 1;

        } feature_ver_1_3;
        struct
        {
            NvU8  continuous_frequency          : 1;
            NvU8  preferred_timing_is_native    : 1; // should be "Preferred_timing_is_dtd1".  To be exact, "Native" is referenced as the native HDTV timing by CEA861 extension block
            NvU8  default_colorspace_srgb       : 1;
            NvU8  color_type                    : 2;
            NvU8  support_active_off            : 1;
            NvU8  support_suspend               : 1;
            NvU8  support_standby               : 1;
        } feature_ver_1_4_analog;
        struct
        {
            NvU8  continuous_frequency          : 1;
            NvU8  preferred_timing_is_native    : 1; // should be "Preferred_timing_is_dtd1".  To be exact, "Native" is referenced as the native HDTV timing by CEA861 extension block
            NvU8  default_colorspace_srgb       : 1;
            NvU8  support_ycrcb_444             : 1;
            NvU8  support_ycrcb_422             : 1;
            NvU8  support_active_off            : 1;
            NvU8  support_suspend               : 1;
            NvU8  support_standby               : 1;
        } feature_ver_1_4_digital;
    }u;

    // chromaticity coordinates
    NvU16  cc_red_x;
    NvU16  cc_red_y;
    NvU16  cc_green_x;
    NvU16  cc_green_y;
    NvU16  cc_blue_x;
    NvU16  cc_blue_y;
    NvU16  cc_white_x;
    NvU16  cc_white_y;

    // established timings 1 and 2
    NvU16  established_timings_1_2;

    // Manufacturer reserved timings
    NvU16  manufReservedTimings;

    // standard timings
    NvU16  standard_timings[NVT_EDID_MAX_STANDARD_TIMINGS];

    // 18-bytes display descriptor info
    NVT_EDID_18BYTE_DESCRIPTOR    ldd[NVT_EDID_MAX_LONG_DISPLAY_DESCRIPTOR];

    // the parse timing
    NVT_TIMING timing[NVT_EDID_MAX_TOTAL_TIMING];

    // Note: This contains the timing after validation.
    NvU32  total_timings;

    // This contains the count timing that were invalidated because they don't meet
    // some policies (PClk, etc).
    NvU32  total_invalidTimings;

    // indicates support for HDMI 1.4+ 3D stereo modes are present
    NvU32  HDMI3DSupported;

    HDMI3DSUPPORTMAP        Hdmi3Dsupport;

    // Data parsed from NVDA VSDB - Variable Refresh Rate Monitor capabilities
    NVDA_VSDB_PARSED_INFO   nvdaVsdbInfo;

    // Data parsed from MSFT VSDB - HMD and Specialized (Direct display) Monitor capabilities
    MSFT_VSDB_PARSED_INFO   msftVsdbInfo;

    // HDR capability information from the HDR Metadata Data Block
    NVT_HDR_STATIC_METADATA hdr_static_metadata_info;

    // DV capability information from the DV Metadata Data Block
    NVT_DV_STATIC_METADATA dv_static_metadata_info;

    // HDR10+ capability information from the HDR10+ LLC VSVDB
    NVT_HDR10PLUS_INFO hdr10PlusInfo;

    // HDMI LLC info
    NVT_HDMI_LLC_INFO       hdmiLlcInfo;

    // HDMI 2.0 information
    NVT_HDMI_FORUM_INFO     hdmiForumInfo;
    // deprecating the following, please use hdmiForumInfo;
    struct
    {
        NvU8 max_TMDS_char_rate;
        NvU8 lte_340Mcsc_scramble   :1;
        NvU8 rr_capable             :1;
        NvU8 SCDC_present           :1;
    } hdmi_2_0_info;

    // the total edid extension(s) attached to the basic block
    NvU32  total_extensions;
    // the total displayid2 extension(s) attached to the basic block.
    NvU32  total_did2_extensions;

    NvU8   checksum;
    NvU8   checksum_ok;

    // extension info
    NVT_EDID_CEA861_INFO ext861;

    // for the 2nd CEA/EIA861 extension
    // note: "ext861" should really be an array but since it requires massive name change and it's hard
    //        to find more than one 861 extension in the real world, I made a trade off like this for now.
    NVT_EDID_CEA861_INFO ext861_2;

    NVT_DISPLAYID_INFO     ext_displayid;
    NVT_DISPLAYID_2_0_INFO ext_displayid20;
} NVT_EDID_INFO;

typedef enum
{
    NVT_PROTOCOL_UNKNOWN = 0,
    NVT_PROTOCOL_DP      = 1,
    NVT_PROTOCOL_HDMI    = 2,
    NVT_PROTOCOL_DVI     = 3,
} NVT_PROTOCOL;

// the display interface/connector claimed by the EDID
#define NVT_EDID_INPUT_DIGITAL_UNDEFINED    0x00  // undefined digital interface
#define NVT_EDID_INPUT_DVI                  0x01
#define NVT_EDID_INPUT_HDMI_TYPE_A          0x02
#define NVT_EDID_INPUT_HDMI_TYPE_B          0x03
#define NVT_EDID_INPUT_MDDI                 0x04
#define NVT_EDID_INPUT_DISPLAY_PORT         0x05


// the EDID extension TAG
#define NVT_EDID_EXTENSION_CTA              0x02  // CTA 861 series extensions
#define NVT_EDID_EXTENSION_VTB              0x10  // video timing block extension
#define NVT_EDID_EXTENSION_DI               0x40  // display information extension
#define NVT_EDID_EXTENSION_LS               0x50  // localized string extension
#define NVT_EDID_EXTENSION_DPVL             0x60  // digital packet video link extension
#define NVT_EDID_EXTENSION_DISPLAYID        0x70  // display id
#define NVT_EDID_EXTENSION_BM               0xF0  // extension block map
#define NVT_EDID_EXTENSION_OEM              0xFF  // extension defined by the display manufacturer

//************************************
// Audio and Video Infoframe Control
//************************************
//
// the control info for generating infoframe data
#define NVT_INFOFRAME_CTRL_DONTCARE  0xFF
//
typedef struct tagNVT_VIDEO_INFOFRAME_CTRL
{
    NvU8  color_space;
    NvU8  active_format_info_present;
    NvU8  bar_info;
    NvU8  scan_info;
    NvU8  colorimetry;
    NvU8  pic_aspect_ratio;
    NvU8  active_format_aspect_ratio;
    NvU8  it_content;
    NvU8  it_content_type;
    NvU8  extended_colorimetry;
    NvU8  rgb_quantization_range;
    NvU8  nonuniform_scaling;
    NvU8  video_format_id;
    NvU8  pixel_repeat;
    NvU16 top_bar;
    NvU16 bottom_bar;
    NvU16 left_bar;
    NvU16 right_bar;
    NvU8  addition_colorimetry_ext;
    NvU8  frame_rate;
    NvU8  rid;
}NVT_VIDEO_INFOFRAME_CTRL;

//
typedef struct tagNVT_AUDIO_INFOFRAME_CTRL
{
    NvU8 coding_type;
    NvU8 channel_count;
    NvU8 sample_rate;
    NvU8 sample_depth;
    NvU8 speaker_placement;
    NvU8 level_shift;
    NvU8 down_mix_inhibit;
}NVT_AUDIO_INFOFRAME_CTRL;

typedef struct tagNVT_VENDOR_SPECIFIC_INFOFRAME_CTRL
{
    NvU8            Enable;
    NvU8            VSIFVersion;
    NvU8            HDMIFormat;
    NvU8            HDMI_VIC;
    NvBool          ALLMEnable;
    NvU8            ThreeDStruc;
    NvU8            ThreeDDetail;
    NvU8            MetadataPresent;
    NvU8            MetadataType;
    NvU8            Metadata[8];       // type determines length

} NVT_VENDOR_SPECIFIC_INFOFRAME_CTRL;
#define NVT_3D_METADTATA_TYPE_PARALAX   0x00
#define NVT_3D_METADTATA_PARALAX_LEN    0x08
#define NVT_VSIF_VERSION_NONE              0
#define NVT_VSIF_VERSION_H14B_VSIF        14
#define NVT_VSIF_VERSION_HF_VSIF          20


#define NVT_EXTENDED_METADATA_PACKET_INFOFRAME_VER_HDMI21      0x0
#define NVT_EXTENDED_METADATA_PACKET_INFOFRAME_VER_HDMI21A     0x1
typedef struct tagNVT_EXTENDED_METADATA_PACKET_INFOFRAME_CTRL
{
    NvU32 version;          // See #define NVT_EXTENDED_METADATA_PACKET_INFOFRAME_VER
    NvU32 EnableVRR;
    NvU32 ITTiming;
    NvU32 BaseVFP;
    NvU32 ReducedBlanking;
    NvU32 BaseRefreshRate;
    NvU32 EnableQMS;
    NvU32 NextTFR;
    NvU32 Sync;
    NvU32 MConst;
} NVT_EXTENDED_METADATA_PACKET_INFOFRAME_CTRL;

typedef struct tagNVT_ADAPTIVE_SYNC_SDP_CTRL
{
    NvU32   minVTotal;
    NvU32   targetRefreshRate;
    NvU32   srCoastingVTotal;
    NvBool  bFixedVTotal;
    NvBool  bRefreshRateDivider;
}NVT_ADAPTIVE_SYNC_SDP_CTRL;

//***********************************
// the actual Auido/Video Infoframe
//***********************************
//
// info frame type code
#define NVT_INFOFRAME_TYPE_VENDOR_SPECIFIC               1
#define NVT_INFOFRAME_TYPE_VIDEO                         2
#define NVT_INFOFRAME_TYPE_SOURCE_PRODUCT_DESCRIPTION    3
#define NVT_INFOFRAME_TYPE_AUDIO                         4
#define NVT_INFOFRAME_TYPE_MPEG_SOURCE                   5
#define NVT_INFOFRAME_TYPE_SELF_REFRESH                  6
#define NVT_INFOFRAME_TYPE_DYNAMIC_RANGE_MASTERING       7
#define NVT_INFOFRAME_TYPE_EXTENDED_METADATA_PACKET      8
//
//
typedef struct tagNVT_INFOFRAME_HEADER
{
    NvU8 type;
    NvU8 version;
    NvU8 length;
}NVT_INFOFRAME_HEADER;

typedef struct tagNVT_EXTENDED_METADATA_PACKET_INFOFRAME_HEADER
{
    NvU8 type;
    NvU8 firstLast;
    NvU8 sequenceIndex;
} NVT_EXTENDED_METADATA_PACKET_INFOFRAME_HEADER;

#define NVT_EMP_HEADER_FIRST                               0x80
#define NVT_EMP_HEADER_LAST                                0x40
#define NVT_EMP_HEADER_FIRST_LAST                          0xC0

// SPD Infoframe
typedef struct tagNVT_SPD_INFOFRAME_PAYLOAD
{
    NvU8 vendorBytes[8];
    NvU8 productBytes[16];

    NvU8 sourceInformation;

} NVT_SPD_INFOFRAME_PAYLOAD;

typedef struct tagNVT_SPD_INFOFRAME
{
    NVT_INFOFRAME_HEADER       Header;
    NVT_SPD_INFOFRAME_PAYLOAD  Data;
} NVT_SPD_INFOFRAME;

// the video infoframe version 1-3 structure
typedef struct tagNVT_VIDEO_INFOFRAME
{
    NvU8 type;
    NvU8 version;
    NvU8 length;

    // byte 1~5
    NvU8 byte1;
    NvU8 byte2;
    NvU8 byte3;
    NvU8 byte4;
    NvU8 byte5;

    // byte 6~13
    NvU8 top_bar_low;
    NvU8 top_bar_high;
    NvU8 bottom_bar_low;
    NvU8 bottom_bar_high;
    NvU8 left_bar_low;
    NvU8 left_bar_high;
    NvU8 right_bar_low;
    NvU8 right_bar_high;

    // byte 14~15
    NvU8 byte14;
    NvU8 byte15;
}NVT_VIDEO_INFOFRAME;
//
#define NVT_VIDEO_INFOFRAME_VERSION_1                     1
#define NVT_VIDEO_INFOFRAME_VERSION_2                     2
#define NVT_VIDEO_INFOFRAME_VERSION_3                     3
#define NVT_VIDEO_INFOFRAME_VERSION_4                     4
//
#define NVT_VIDEO_INFOFRAME_BYTE1_S1S0_MASK               0x03
#define NVT_VIDEO_INFOFRAME_BYTE1_S1S0_SHIFT              0
#define NVT_VIDEO_INFOFRAME_BYTE1_S1S0_NO_DATA            0
#define NVT_VIDEO_INFOFRAME_BYTE1_S1S0_OVERSCANNED        1
#define NVT_VIDEO_INFOFRAME_BYTE1_S1S0_UNDERSCANNED       2
#define NVT_VIDEO_INFOFRAME_BYTE1_S1S0_FUTURE             3
//
#define NVT_VIDEO_INFOFRAME_BYTE1_B1B0_MASK               0x0C
#define NVT_VIDEO_INFOFRAME_BYTE1_B1B0_SHIFT              2
#define NVT_VIDEO_INFOFRAME_BYTE1_B1B0_NOT_VALID          0
#define NVT_VIDEO_INFOFRAME_BYTE1_B1B0_VERT_VALID         1
#define NVT_VIDEO_INFOFRAME_BYTE1_B1B0_HORIZ_VALID        2
#define NVT_VIDEO_INFOFRAME_BYTE1_B1B0_H_V_VALID          3
//
#define NVT_VIDEO_INFOFRAME_BYTE1_A0_MASK                 0x10    // active format info present
#define NVT_VIDEO_INFOFRAME_BYTE1_A0_SHIFT                4       // active format info present
#define NVT_VIDEO_INFOFRAME_BYTE1_A0_NO_DATA              0
#define NVT_VIDEO_INFOFRAME_BYTE1_A0_VALID                1
//
// CTA-861G new requirement - DD changed this policy
#define NVT_VIDEO_INFOFRAME_BYTE1_Y2_MASK                 8
#define NVT_VIDEO_INFOFRAME_BYTE1_Y2Y1Y0_MASK             0xE0
#define NVT_VIDEO_INFOFRAME_BYTE1_Y2Y1Y0_SHIFT            0x5
#define NVT_VIDEO_INFOFRAME_BYTE1_Y2Y1Y0_RGB              0
#define NVT_VIDEO_INFOFRAME_BYTE1_Y2Y1Y0_YCbCr422         1
#define NVT_VIDEO_INFOFRAME_BYTE1_Y2Y1Y0_YCbCr444         2
#define NVT_VIDEO_INFOFRAME_BYTE1_Y2Y1Y0_YCbCr420         3
#define NVT_VIDEO_INFOFRAME_BYTE1_Y2Y1Y0_FUTURE           3 // nvlEscape still uses this line 4266
// CTA-861I new requirement
#define NVT_VIDEO_INFOFRAME_BYTE1_Y2Y1Y0_IDODEFINED       7
// CEA-861-F - Unix still used this one
#define NVT_VIDEO_INFOFRAME_BYTE1_Y1Y0_MASK               0x60
#define NVT_VIDEO_INFOFRAME_BYTE1_Y1Y0_SHIFT              0x5
#define NVT_VIDEO_INFOFRAME_BYTE1_Y1Y0_RGB                0
#define NVT_VIDEO_INFOFRAME_BYTE1_Y1Y0_YCbCr422           1
#define NVT_VIDEO_INFOFRAME_BYTE1_Y1Y0_YCbCr444           2
#define NVT_VIDEO_INFOFRAME_BYTE1_Y1Y0_YCbCr420           3
#define NVT_VIDEO_INFOFRAME_BYTE1_Y1Y0_FUTURE             3 // nvlEscape still uses this lline 4266
//
#define NVT_VIDEO_INFOFRAME_BYTE1_RESERVED_MASK           0x80  // for Inforframe V1 / V2
#define NVT_VIDEO_INFOFRAME_BYTE1_RESERVED_SHIFT          7
//
#define NVT_VIDEO_INFOFRAME_BYTE2_R3R2R1R0_MASK           0x0F    // active format aspect ratio
#define NVT_VIDEO_INFOFRAME_BYTE2_R3R2R1R0_SHIFT          0
#define NVT_VIDEO_INFOFRAME_BYTE2_R3R2R1R0_SAME_AS_M1M0   8
#define NVT_VIDEO_INFOFRAME_BYTE2_R3R2R1R0_4X3_CENTER     9
#define NVT_VIDEO_INFOFRAME_BYTE2_R3R2R1R0_16X9_CENTER    10
#define NVT_VIDEO_INFOFRAME_BYTE2_R3R2R1R0_14x9_CENTER    11
//
#define NVT_VIDEO_INFOFRAME_BYTE2_M1M0_MASK               0x30    // picture aspect ratio
#define NVT_VIDEO_INFOFRAME_BYTE2_M1M0_SHIFT              4       // picture aspect ratio
#define NVT_VIDEO_INFOFRAME_BYTE2_M1M0_NO_DATA            0
#define NVT_VIDEO_INFOFRAME_BYTE2_M1M0_4X3                1
#define NVT_VIDEO_INFOFRAME_BYTE2_M1M0_16X9               2
#define NVT_VIDEO_INFOFRAME_BYTE2_M1M0_FUTURE             3
//
#define NVT_VIDEO_INFOFRAME_BYTE2_C1C0_MASK               0xC0    // colorimetry
#define NVT_VIDEO_INFOFRAME_BYTE2_C1C0_SHIFT              6
#define NVT_VIDEO_INFOFRAME_BYTE2_C1C0_NO_DATA            0
#define NVT_VIDEO_INFOFRAME_BYTE2_C1C0_SMPTE170M_ITU601   1
#define NVT_VIDEO_INFOFRAME_BYTE2_C1C0_ITU709             2
#define NVT_VIDEO_INFOFRAME_BYTE2_C1C0_EXT_COLORIMETRY    3
//
#define NVT_VIDEO_INFOFRAME_BYTE3_SC_MASK                 0x03    // non-uniform scaling
#define NVT_VIDEO_INFOFRAME_BYTE3_SC_SHIFT                0
#define NVT_VIDEO_INFOFRAME_BYTE3_SC_NONE                 0
#define NVT_VIDEO_INFOFRAME_BYTE3_SC_HORIZ_SCALED         1
#define NVT_VIDEO_INFOFRAME_BYTE3_SC_VERT_SCALED          2
#define NVT_VIDEO_INFOFRAME_BYTE3_SC_H_V_SCALED           3
//
#define NVT_VIDEO_INFOFRAME_BYTE3_Q1Q0_MASK               0x0C    // quantization
#define NVT_VIDEO_INFOFRAME_BYTE3_Q1Q0_SHIFT              2
#define NVT_VIDEO_INFOFRAME_BYTE3_Q1Q0_DEFAULT            0
#define NVT_VIDEO_INFOFRAME_BYTE3_Q1Q0_LIMITED_RANGE      1
#define NVT_VIDEO_INFOFRAME_BYTE3_Q1Q0_FULL_RANGE         2
#define NVT_VIDEO_INFOFRAME_BYTE3_Q1Q0_RESERVED           3
//
#define NVT_VIDEO_INFOFRAME_BYTE3_EC_MASK                 0x70    // extended colorimetry
#define NVT_VIDEO_INFOFRAME_BYTE3_EC_SHIFT                4
#define NVT_VIDEO_INFOFRAME_BYTE3_EC_xvYCC_601            0
#define NVT_VIDEO_INFOFRAME_BYTE3_EC_xvYCC_709            1
#define NVT_VIDEO_INFOFRAME_BYTE3_EC_sYCC_601             2
#define NVT_VIDEO_INFOFRAME_BYTE3_EC_AdobeYCC_601         3
#define NVT_VIDEO_INFOFRAME_BYTE3_EC_AdobeRGB             4
#define NVT_VIDEO_INFOFRAME_BYTE3_EC_BT2020cYCC           5       // CEA-861-F define it as "ITU-R BT.2020 YcCbcCrc" at Table 12
#define NVT_VIDEO_INFOFRAME_BYTE3_EC_BT2020RGBYCC         6       // CEA-861-F define it as "ITU-R BT.2020 YcCbCr" at Table 12
#define NVT_VIDEO_INFOFRAME_BYTE3_EC_RESERVED7            7       // CEA-861-F define it as "Reserved" at Table 12
#define NVT_VIDEO_INFOFRAME_BYTE3_EC_AdditionalColorExt   7       // CTA-861-G define it as "Additional Colorimtry Ext Info Valid" at Table_13
//
#define NVT_VIDEO_INFOFRAME_BYTE3_ITC_MASK                0x80    // IT content
#define NVT_VIDEO_INFOFRAME_BYTE3_ITC_SHIFT               7
#define NVT_VIDEO_INFOFRAME_BYTE3_ITC_NO_DATA             0
#define NVT_VIDEO_INFOFRAME_BYTE3_ITC_IT_CONTENT          1
//
#define NVT_VIDEO_INFOFRAME_BYTE3_RESERVED_V1_MASK        0x60    // reserved
#define NVT_VIDEO_INFOFRAME_BYTE3_RESERVED_V1_SHIFT       5
//
#define NVT_VIDEO_INFOFRAME_BYTE4_VIC_MASK                0xFF    // video identification code
#define NVT_VIDEO_INFOFRAME_BYTE4_VIC_SHIFT               0
#define NVT_VIDEO_INFOFRAME_BYTE4_VIC7                    0x80
//
#define NVT_VIDEO_INFOFRAME_BYTE4_RESERVED_V3_MASK        0x00
#define NVT_VIDEO_INFOFRAME_BYTE4_RESERVED_V3_SHIFT       0
#define NVT_VIDEO_INFOFRAME_BYTE4_RESERVED_V2_MASK        0x80
#define NVT_VIDEO_INFOFRAME_BYTE4_RESERVED_V2_SHIFT       7
#define NVT_VIDEO_INFOFRAME_BYTE4_RESERVED_V1_MASK        0xFF
#define NVT_VIDEO_INFOFRAME_BYTE4_RESERVED_V1_SHIFT       0
//
#define NVT_VIDEO_INFOFRAME_BYTE5_PR_MASK                 0x0F    // pixel repetitions
#define NVT_VIDEO_INFOFRAME_BYTE5_PR_SHIFT                0
#define NVT_VIDEO_INFOFRAME_BYTE5_PR_NO_PEP               0
#define NVT_VIDEO_INFOFRAME_BYTE5_PR_2X                   1
#define NVT_VIDEO_INFOFRAME_BYTE5_PR_3X                   2
#define NVT_VIDEO_INFOFRAME_BYTE5_PR_4X                   3
#define NVT_VIDEO_INFOFRAME_BYTE5_PR_5X                   4
#define NVT_VIDEO_INFOFRAME_BYTE5_PR_6X                   5
#define NVT_VIDEO_INFOFRAME_BYTE5_PR_7X                   6
#define NVT_VIDEO_INFOFRAME_BYTE5_PR_8X                   7
#define NVT_VIDEO_INFOFRAME_BYTE5_PR_9X                   8
#define NVT_VIDEO_INFOFRAME_BYTE5_PR_10X                  9
//
#define NVT_VIDEO_INFOFRAME_BYTE5_CN1CN0_MASK             0x30     // Content Information
#define NVT_VIDEO_INFOFRAME_BYTE5_CN1CN0_SHIFT            4
#define NVT_VIDEO_INFOFRAME_BYTE5_CN1CN0_NODATA           0     // ITC = 0
#define NVT_VIDEO_INFOFRAME_BYTE5_CN1CN0_GRAPHICS         0     // ITC = 1
#define NVT_VIDEO_INFOFRAME_BYTE5_CN1CN0_PHOTO            1     // ITC = don't care
#define NVT_VIDEO_INFOFRAME_BYTE5_CN1CN0_CINEMA           2     // ITC = don't care
#define NVT_VIDEO_INFOFRAME_BYTE5_CN1CN0_GAME             3     // ITC = don't care

#define NVT_VIDEO_INFOFRAME_BYTE5_YQ1YQ0_MASK             0xC0     // YCC quantization
#define NVT_VIDEO_INFOFRAME_BYTE5_YQ1YQ0_SHIFT            6
#define NVT_VIDEO_INFOFRAME_BYTE5_YQ1YQ0_LIMITED_RANGE    1
#define NVT_VIDEO_INFOFRAME_BYTE5_YQ1YQ0_FULL_RANGE       2
#define NVT_VIDEO_INFOFRAME_BYTE5_YQ1YQ0_RESERVED3        3
#define NVT_VIDEO_INFOFRAME_BYTE5_YQ1YQ0_RESERVED4        4
//
#define NVT_VIDEO_INFOFRAME_BYTE5_YQ_MASK                 0xc0  // content type
#define NVT_VIDEO_INFOFRAME_BYTE5_YQ_SHIFT                6
#define NVT_VIDEO_INFOFRAME_BYTE5_YQ_LIMITED              0
#define NVT_VIDEO_INFOFRAME_BYTE5_YQ_FULL                 1
#define NVT_VIDEO_INFOFRAME_BYTE5_YQ_RSVD1                2
#define NVT_VIDEO_INFOFRAME_BYTE5_YQ_RSVD2                3
//
#define NVT_VIDEO_INFOFRAME_BYTE5_RESERVED_V2_MASK        0x00
#define NVT_VIDEO_INFOFRAME_BYTE5_RESERVED_V2_SHIFT       0
#define NVT_VIDEO_INFOFRAME_BYTE5_RESERVED_V1_MASK        0xFF
#define NVT_VIDEO_INFOFRAME_BYTE5_RESERVED_V1_SHIFT       0
//
#define NVT_VIDEO_INFOFRAME_BYTE14_FR0_FR3_MASK           0x0F
#define NVT_VIDEO_INFOFRAME_BYTE14_FR0_FR3_SHIFT          0
#define NVT_VIDEO_INFOFRAME_BYTE14_FR0_FR3_NODATA         0
#define NVT_VIDEO_INFOFRAME_BYTE14_FR4_ONE_BIT_MASK       0x10
#define NVT_VIDEO_INFOFRAME_BYTE15_FR4_MASK               0x40
#define NVT_VIDEO_INFOFRAME_BYTE15_FR4_NODATA             0
#define NVT_VIDEO_INFOFRAME_BYTE15_FR4_SHIFT              2
//
#define NVT_VIDEO_INFOFRAME_BYTE15_RID_MASK               0x3F
#define NVT_VIDEO_INFOFRAME_BYTE15_RID_SHIFT              0
#define NVT_VIDEO_INFOFRAME_BYTE15_RID_NODATA             0
//
#define NVT_VIDEO_INFOFRAME_BYTE14_ACE0_3_MASK            0xF0
#define NVT_VIDEO_INFOFRAME_BYTE14_ACE0_3_SHIFT           4
#define NVT_VIDEO_INFOFRAME_BYTE14_ACE0_3_P3D65RGB        0
#define NVT_VIDEO_INFOFRAME_BYTE14_ACE0_3_P3DCIRGB        1
#define NVT_VIDEO_INFOFRAME_BYTE14_ACE0_3_BT2100_ICtCp    2
#define NVT_VIDEO_INFOFRAME_BYTE14_ACE0_3_sRGB            3
#define NVT_VIDEO_INFOFRAME_BYTE14_ACE0_3_defaultRGB      4
//
#define NVT_VIDEO_INFOFRAME_CONTENT_VIDEO                 0
#define NVT_VIDEO_INFOFRAME_CONTENT_GRAPHICS              1
#define NVT_VIDEO_INFOFRAME_CONTENT_PHOTO                 2
#define NVT_VIDEO_INFOFRAME_CONTENT_CINEMA                3
#define NVT_VIDEO_INFOFRAME_CONTENT_GAME                  4
#define NVT_VIDEO_INFOFRAME_CONTENT_LAST                  4

#pragma pack(1)
typedef struct
{
    // byte 1
    struct
    {
        NvU8 scanInfo                : 2;
        NvU8 barInfo                 : 2;
        NvU8 activeFormatInfoPresent : 1;
        NvU8 colorSpace              : 2;
        NvU8 rsvd_bits_byte1         : 1;
    } byte1;

    // byte 2
    struct
    {
        NvU8 activeFormatAspectRatio : 4;
        NvU8 picAspectRatio          : 2;
        NvU8 colorimetry             : 2;
    } byte2;

    // byte 3
    struct
    {
        NvU8 nonuniformScaling       : 2;
        NvU8 rgbQuantizationRange    : 2;
        NvU8 extendedColorimetry     : 3;
        NvU8 itContent               : 1;
    } byte3;

    // byte 4
    struct
    {
        NvU8 vic                     : 7;
        NvU8 rsvd_bits_byte4         : 1;
    } byte4;

    // byte 5
    struct
    {
        NvU8 pixelRepeat             : 4;
        NvU8 contentTypes            : 2;
        NvU8 yccQuantizationRange    : 2;
    } byte5;

    NvU16 topBar;
    NvU16 bottomBar;
    NvU16 leftBar;
    NvU16 rightBar;

    // byte 14~15
    struct
    {
        NvU8 fr_low                  : 4;
        NvU8 ace                     : 4;
    } byte14;

    struct
    {
        NvU8 rid                     : 6;
        NvU8 fr_hi                   : 1;
        NvU8 rsvd_bits_byte15        : 1;
    }byte15;
} NVT_VIDEO_INFOFRAME_OVERRIDE;
#pragma pack()

typedef struct
{
    NvU32 vic                     : 8;
    NvU32 pixelRepeat             : 5;
    NvU32 colorSpace              : 3;
    NvU32 colorimetry             : 3;
    NvU32 extendedColorimetry     : 4;
    NvU32 rgbQuantizationRange    : 3;
    NvU32 yccQuantizationRange    : 3;
    NvU32 itContent               : 2;
    NvU32 contentTypes            : 3;
    NvU32 scanInfo                : 3;
    NvU32 activeFormatInfoPresent : 2;
    NvU32 activeFormatAspectRatio : 5;
    NvU32 picAspectRatio          : 3;
    NvU32 nonuniformScaling       : 3;
    NvU32 barInfo                 : 3;
    NvU32 top_bar                 : 17;
    NvU32 bottom_bar              : 17;
    NvU32 left_bar                : 17;
    NvU32 right_bar               : 17;
    NvU32 Future17                : 2;
    NvU32 Future47                : 2;
} NVT_INFOFRAME_VIDEO;


#define NVT_VIDEO_INFOFRAME_OVERRIDE_BYTE1_S1S0_MASK               0x3
#define NVT_VIDEO_INFOFRAME_OVERRIDE_BYTE1_B1B0_MASK               0x3
#define NVT_VIDEO_INFOFRAME_OVERRIDE_BYTE1_A0_MASK                 0x1    // active format info present
#define NVT_VIDEO_INFOFRAME_OVERRIDE_BYTE1_Y1Y0_MASK               0x3
#define NVT_VIDEO_INFOFRAME_OVERRIDE_BYTE1_Y2Y1Y0_MASK             0x7
//
#define NVT_VIDEO_INFOFRAME_OVERRIDE_BYTE2_R3R2R1R0_MASK           0xF    // active format aspect ratio
#define NVT_VIDEO_INFOFRAME_OVERRIDE_BYTE2_M1M0_MASK               0x3    // picture aspect ratio
#define NVT_VIDEO_INFOFRAME_OVERRIDE_BYTE2_C1C0_MASK               0x3    // colorimetry
//
#define NVT_VIDEO_INFOFRAME_OVERRIDE_BYTE3_SC_MASK                 0x3    // non-uniform scaling
#define NVT_VIDEO_INFOFRAME_OVERRIDE_BYTE3_Q1Q0_MASK               0x3    // quantization
#define NVT_VIDEO_INFOFRAME_OVERRIDE_BYTE3_EC_MASK                 0x7    // extended colorimetry
#define NVT_VIDEO_INFOFRAME_OVERRIDE_BYTE3_ITC_MASK                0x1    // IT content
//
#define NVT_VIDEO_INFOFRAME_OVERRIDE_BYTE4_VIC_MASK                0x7F   // video identification code
//
#define NVT_VIDEO_INFOFRAME_OVERRIDE_BYTE5_PR_MASK                 0xF    // pixel repetitions
#define NVT_VIDEO_INFOFRAME_OVERRIDE_BYTE5_CN1CN0_MASK             0x3
#define NVT_VIDEO_INFOFRAME_OVERRIDE_BYTE5_YQ1YQ0_MASK             0x3    // YCC quantization
//
#define NVT_VIDEO_INFOFRAME_OVERRIDE_BYTE14_FR0FR3_MASK            0xF    // Frame rate 0-3 bits in Byte14
#define NVT_VIDEO_INFOFRAME_OVERRIDE_BYTE14_ACE0ACE3_MASK          0xF    // Additional Colorimetry Extension
#define NVT_VIDEO_INFOFRAME_OVERRIDE_BYTE15_RID0RID5_MASK          0x3F   // Resolution Identification
#define NVT_VIDEO_INFOFRAME_OVERRIDE_BYTE15_FR4_MASK               0x1    // Frame rate 4th bit in Byte 15

// audio infoframe structure
typedef struct tagNVT_AUDIO_INFOFRAME
{
    NvU8 type;
    NvU8 version;
    NvU8 length;

    // byte 1~5
    NvU8 byte1;
    NvU8 byte2;
    NvU8 byte3;
    NvU8 byte4;
    NvU8 byte5;

    // byte 6~10
    NvU8 rsvd_byte6;
    NvU8 rsvd_byte7;
    NvU8 rsvd_byte8;
    NvU8 rsvd_byte9;
    NvU8 rsvd_byte10;

}NVT_AUDIO_INFOFRAME;

// self refresh infoframe structure. See SR spec.
typedef struct tagNVT_SR_INFOFRAME
{
    NvU8 type;
    NvU8 version;
    NvU8 length;

    NvU8 data;

}NVT_SR_INFOFRAME;

//
#define NVT_AUDIO_INFOFRAME_VERSION_1                            1
//
#define NVT_AUDIO_INFOFRAME_BYTE1_CC_MASK                        0x07
#define NVT_AUDIO_INFOFRAME_BYTE1_CC_SHIFT                       0
#define NVT_AUDIO_INFOFRAME_BYTE1_CC_REF_HEADER                  0
#define NVT_AUDIO_INFOFRAME_BYTE1_CC_2CH                         1
#define NVT_AUDIO_INFOFRAME_BYTE1_CC_DO_NOT_USE                  2
#define NVT_AUDIO_INFOFRAME_BYTE1_CC_4CH                         3
#define NVT_AUDIO_INFOFRAME_BYTE1_CC_5CH                         4
#define NVT_AUDIO_INFOFRAME_BYTE1_CC_6CH                         5
#define NVT_AUDIO_INFOFRAME_BYTE1_CC_7CH                         6
#define NVT_AUDIO_INFOFRAME_BYTE1_CC_8CH                         7
//
#define NVT_AUDIO_INFOFRAME_BYTE1_RESERVED_MASK                  0x08
#define NVT_AUDIO_INFOFRAME_BYTE1_RESERVED_SHIFT                 3
//
#define NVT_AUDIO_INFOFRAME_BYTE1_CT_MASK                        0xF0
#define NVT_AUDIO_INFOFRAME_BYTE1_CT_SHIFT                       4
#define NVT_AUDIO_INFOFRAME_BYTE1_CT_REF_HEADER                  0
#define NVT_AUDIO_INFOFRAME_BYTE1_CT_PCM                         1
#define NVT_AUDIO_INFOFRAME_BYTE1_CT_DO_NOT_USE                  2
#define NVT_AUDIO_INFOFRAME_BYTE1_CT_MPEG1                       3
#define NVT_AUDIO_INFOFRAME_BYTE1_CT_MP3                         4
#define NVT_AUDIO_INFOFRAME_BYTE1_CT_MPEG2                       5
#define NVT_AUDIO_INFOFRAME_BYTE1_CT_AAC                         6
#define NVT_AUDIO_INFOFRAME_BYTE1_CT_DTS                         7
#define NVT_AUDIO_INFOFRAME_BYTE1_CT_USE_CODING_EXTENSION_TYPE   15
//
#define NVT_AUDIO_INFOFRAME_BYTE2_SS_MASK                        0x3
#define NVT_AUDIO_INFOFRAME_BYTE2_SS_SHIFT                       0
#define NVT_AUDIO_INFOFRAME_BYTE2_SS_REF_HEADER                  0
#define NVT_AUDIO_INFOFRAME_BYTE2_SS_16BIT                       1
#define NVT_AUDIO_INFOFRAME_BYTE2_SS_20BIT                       2
#define NVT_AUDIO_INFOFRAME_BYTE2_SS_24BIT                       3
//
#define NVT_AUDIO_INFOFRAME_BYTE2_SF_MASK                        0x1C
#define NVT_AUDIO_INFOFRAME_BYTE2_SF_SHIFT                       2
#define NVT_AUDIO_INFOFRAME_BYTE2_SF_HEADER                      0
#define NVT_AUDIO_INFOFRAME_BYTE2_SF_32KHz                       1
#define NVT_AUDIO_INFOFRAME_BYTE2_SF_44KHz                       2
#define NVT_AUDIO_INFOFRAME_BYTE2_SF_48KHz                       3
#define NVT_AUDIO_INFOFRAME_BYTE2_SF_88KHz                       4
#define NVT_AUDIO_INFOFRAME_BYTE2_SF_96KHz                       5
#define NVT_AUDIO_INFOFRAME_BYTE2_SF_176KHz                      6
#define NVT_AUDIO_INFOFRAME_BYTE2_SF_192KHz                      7
//
#define NVT_AUDIO_INFOFRAME_BYTE2_RESERVED_MASK                  0xE0
#define NVT_AUDIO_INFOFRAME_BYTE2_RESERVED_SHIFT                 5
//
#define NVT_AUDIO_INFOFRAME_BYTE3_CXT_MASK                       0x1F
#define NVT_AUDIO_INFOFRAME_BYTE3_CXT_SHIFT                      0
#define NVT_AUDIO_INFOFRAME_BYTE3_CXT_RESERVE31                  31
//
#define NVT_AUDIO_INFOFRAME_BYTE3_RESERVED_MASK                  0xE0
#define NVT_AUDIO_INFOFRAME_BYTE3_RESERVED_SHIFT                 5
//
#define NVT_AUDIO_INFOFRAME_BYTE4_CA_MASK                        0xFF
#define NVT_AUDIO_INFOFRAME_BYTE4_CA_SHIFT                       0
#define NVT_AUDIO_INFOFRAME_BYTE4_CA_FRW_FLW_RR_RL_FC_LFE_FR_FL  49
//
#define NVT_AUDIO_INFOFRAME_BYTE5_LFEPBL_MASK                    0x03
#define NVT_AUDIO_INFOFRAME_BYTE5_LFEPBL_SHIFT                   0
#define NVT_AUDIO_INFOFRAME_BYTE5_LFEPBL_NO_DATA                 0
#define NVT_AUDIO_INFOFRAME_BYTE5_LFEPBL_0DB                     1
#define NVT_AUDIO_INFOFRAME_BYTE5_LFEPBL_PLUS10DB                2
#define NVT_AUDIO_INFOFRAME_BYTE5_LFEPBL_RESERVED03              3
//
#define NVT_AUDIO_INFOFRAME_BYTE5_RESERVED_MASK                  0x4
#define NVT_AUDIO_INFOFRAME_BYTE5_RESERVED_SHIFT                 2
//
#define NVT_AUDIO_INFOFRAME_BYTE5_LSV_MASK                       0x78
#define NVT_AUDIO_INFOFRAME_BYTE5_LSV_SHIFT                      3
#define NVT_AUDIO_INFOFRAME_BYTE5_LSV_0dB                        0
#define NVT_AUDIO_INFOFRAME_BYTE5_LSV_1dB                        1
#define NVT_AUDIO_INFOFRAME_BYTE5_LSV_2dB                        2
#define NVT_AUDIO_INFOFRAME_BYTE5_LSV_3dB                        3
#define NVT_AUDIO_INFOFRAME_BYTE5_LSV_4dB                        4
#define NVT_AUDIO_INFOFRAME_BYTE5_LSV_5dB                        5
#define NVT_AUDIO_INFOFRAME_BYTE5_LSV_6dB                        6
#define NVT_AUDIO_INFOFRAME_BYTE5_LSV_7dB                        7
#define NVT_AUDIO_INFOFRAME_BYTE5_LSV_8dB                        8
#define NVT_AUDIO_INFOFRAME_BYTE5_LSV_9dB                        9
#define NVT_AUDIO_INFOFRAME_BYTE5_LSV_10dB                       10
#define NVT_AUDIO_INFOFRAME_BYTE5_LSV_11dB                       11
#define NVT_AUDIO_INFOFRAME_BYTE5_LSV_12dB                       12
#define NVT_AUDIO_INFOFRAME_BYTE5_LSV_13dB                       13
#define NVT_AUDIO_INFOFRAME_BYTE5_LSV_14dB                       14
#define NVT_AUDIO_INFOFRAME_BYTE5_LSV_15dB                       15
//
#define NVT_AUDIO_INFOFRAME_BYTE5_DM_INH_MASK                    0x80
#define NVT_AUDIO_INFOFRAME_BYTE5_DM_INH_SHIFT                   7
#define NVT_AUDIO_INFOFRAME_BYTE5_DM_INH_PERMITTED               0
#define NVT_AUDIO_INFOFRAME_BYTE5_DM_INH_PROHIBITED              1
//
#define NVT_AUDIO_INFOFRAME_BYTE6_RESERVED_MASK                  0xFF
#define NVT_AUDIO_INFOFRAME_BYTE6_RESERVED_SHIFT                 0
//
//
#define NVT_AUDIO_INFOFRAME_BYTE7_RESERVED_MASK                  0xFF
#define NVT_AUDIO_INFOFRAME_BYTE7_RESERVED_SHIFT                 0
//
///
#define NVT_AUDIO_INFOFRAME_BYTE8_RESERVED_MASK                  0xFF
#define NVT_AUDIO_INFOFRAME_BYTE8_RESERVED_SHIFT                 0
//
//
#define NVT_AUDIO_INFOFRAME_BYTE9_RESERVED_MASK                  0xFF
#define NVT_AUDIO_INFOFRAME_BYTE9_RESERVED_SHIFT                 0
//
//
#define NVT_AUDIO_INFOFRAME_BYTE10_RESERVED_MASK                 0xFF
#define NVT_AUDIO_INFOFRAME_BYTE10_RESERVED_SHIFT                0
//

typedef struct
{
    // byte 1
    struct
    {
        NvU8 channelCount        : 3;
        NvU8 rsvd_bits_byte1     : 1;
        NvU8 codingType          : 4;
    } byte1;

    // byte 2
    struct
    {
        NvU8 sampleSize          : 2;
        NvU8 sampleRate          : 3;
        NvU8 rsvd_bits_byte2     : 3;
    } byte2;


    // byte 3
    struct
    {
        NvU8 codingExtensionType : 5;
        NvU8 rsvd_bits_byte3     : 3;
    } byte3;

    // byte 4
    NvU8  speakerPlacement;

    // byte 5
    struct
    {
        NvU8 lfePlaybackLevel    : 2;
        NvU8 rsvd_bits_byte5     : 1;
        NvU8 levelShift          : 4;
        NvU8 downmixInhibit      : 1;
    } byte5;

    // byte 6~10
    NvU8 rsvd_byte6;
    NvU8 rsvd_byte7;
    NvU8 rsvd_byte8;
    NvU8 rsvd_byte9;
    NvU8 rsvd_byte10;
} NVT_AUDIO_INFOFRAME_OVERRIDE;

typedef struct
{
    NvU32 codingType          : 5;
    NvU32 codingExtensionType : 6;
    NvU32 sampleSize          : 3;
    NvU32 sampleRate          : 4;
    NvU32 channelCount        : 4;
    NvU32 speakerPlacement    : 9;
    NvU32 downmixInhibit      : 2;
    NvU32 lfePlaybackLevel    : 3;
    NvU32 levelShift          : 5;
    NvU32 Future12            : 2;
    NvU32 Future2x            : 4;
    NvU32 Future3x            : 4;
    NvU32 Future52            : 2;
    NvU32 Future6             : 9;
    NvU32 Future7             : 9;
    NvU32 Future8             : 9;
    NvU32 Future9             : 9;
    NvU32 Future10            : 9;
} NVT_INFOFRAME_AUDIO;

#define NVT_AUDIO_INFOFRAME_OVERRIDE_BYTE1_CC_MASK            0x07
#define NVT_AUDIO_INFOFRAME_OVERRIDE_BYTE1_CT_MASK            0x0F
#define NVT_AUDIO_INFOFRAME_OVERRIDE_BYTE2_SS_MASK            0x03
#define NVT_AUDIO_INFOFRAME_OVERRIDE_BYTE2_SF_MASK            0x03
#define NVT_AUDIO_INFOFRAME_OVERRIDE_BYTE3_CXT_MASK           0x1F
#define NVT_AUDIO_INFOFRAME_OVERRIDE_BYTE4_CA_MASK            0xFF
#define NVT_AUDIO_INFOFRAME_OVERRIDE_BYTE5_LFEPBL_MASK        0x03
#define NVT_AUDIO_INFOFRAME_OVERRIDE_BYTE5_LSV_MASK           0x0F
#define NVT_AUDIO_INFOFRAME_OVERRIDE_BYTE5_DM_INH_MASK        0x01

//
// HDMI 1.3a GCP, ColorDepth
//
#define NVT_HDMI_COLOR_DEPTH_DEFAULT                 0x0
#define NVT_HDMI_COLOR_DEPTH_RSVD1                   0x1
#define NVT_HDMI_COLOR_DEPTH_RSVD2                   0x2
#define NVT_HDMI_COLOR_DEPTH_RSVD3                   0x3
#define NVT_HDMI_COLOR_DEPTH_24                      0x4
#define NVT_HDMI_COLOR_DEPTH_30                      0x5
#define NVT_HDMI_COLOR_DEPTH_36                      0x6
#define NVT_HDMI_COLOR_DEPTH_48                      0x7
#define NVT_HDMI_COLOR_DEPTH_RSVD8                   0x8
#define NVT_HDMI_COLOR_DEPTH_RSVD9                   0x9
#define NVT_HDMI_COLOR_DEPTH_RSVD10                  0xA
#define NVT_HDMI_COLOR_DEPTH_RSVD11                  0xB
#define NVT_HDMI_COLOR_DEPTH_RSVD12                  0xC
#define NVT_HDMI_COLOR_DEPTH_RSVD13                  0xD
#define NVT_HDMI_COLOR_DEPTH_RSVD14                  0xE
#define NVT_HDMI_COLOR_DEPTH_RSVD15                  0xF

// HDMI 1.3a GCP, PixelPacking Phase
#define NVT_HDMI_PIXELPACKING_PHASE4                 0x0
#define NVT_HDMI_PIXELPACKING_PHASE1                 0x1
#define NVT_HDMI_PIXELPACKING_PHASE2                 0x2
#define NVT_HDMI_PIXELPACKING_PHASE3                 0x3
#define NVT_HDMI_PIXELPACKING_RSVD4                  0x4
#define NVT_HDMI_PIXELPACKING_RSVD5                  0x5
#define NVT_HDMI_PIXELPACKING_RSVD6                  0x6
#define NVT_HDMI_PIXELPACKING_RSVD7                  0x7
#define NVT_HDMI_PIXELPACKING_RSVD8                  0x8
#define NVT_HDMI_PIXELPACKING_RSVD9                  0x9
#define NVT_HDMI_PIXELPACKING_RSVD10                 0xA
#define NVT_HDMI_PIXELPACKING_RSVD11                 0xB
#define NVT_HDMI_PIXELPACKING_RSVD12                 0xC
#define NVT_HDMI_PIXELPACKING_RSVD13                 0xD
#define NVT_HDMI_PIXELPACKING_RSVD14                 0xE
#define NVT_HDMI_PIXELPACKING_RSVD15                 0xF

#define NVT_HDMI_RESET_DEFAULT_PIXELPACKING_PHASE    0x0
#define NVT_HDMI_SET_DEFAULT_PIXELPACKING_PHASE      0x1

#define NVT_HDMI_GCP_SB1_CD_SHIFT                    0
#define NVT_HDMI_GCP_SB1_PP_SHIFT                    4


// Vendor specific info frame (HDMI 1.4 specific)
typedef struct tagNVT_VENDOR_SPECIFIC_INFOFRAME_PAYLOAD
{
    // byte 1~5
    NvU8 byte1;
    NvU8 byte2;
    NvU8 byte3;
    NvU8 byte4;
    NvU8 byte5;
    NvU8 optionalBytes[22];
}NVT_VENDOR_SPECIFIC_INFOFRAME_PAYLOAD;
typedef struct tagNVT_VENDOR_SPECIFIC_INFOFRAME
{
    NVT_INFOFRAME_HEADER                    Header;
    NVT_VENDOR_SPECIFIC_INFOFRAME_PAYLOAD   Data;
} NVT_VENDOR_SPECIFIC_INFOFRAME;
//
#define NVT_HDMI_VS_INFOFRAME_VERSION_1                1

#define NVT_HDMI_VS_HB0_MASK                        0xFF
#define NVT_HDMI_VS_HB0_SHIFT                       0x00
#define NVT_HDMI_VS_HB0_VALUE                       0x01

#define NVT_HDMI_VS_HB1_MASK                        0xFF
#define NVT_HDMI_VS_HB1_SHIFT                       0x00
#define NVT_HDMI_VS_HB1_VALUE                       0x01

#define NVT_HDMI_VS_HB2_MASK                        0xFF
#define NVT_HDMI_VS_HB2_SHIFT                       0x00
#define NVT_HDMI_VS_HB2_VALUE                       0x06

#define NVT_HDMI_VS_BYTE1_OUI_MASK                  0xff
#define NVT_HDMI_VS_BYTE1_OUI_SHIFT                 0x00
#define NVT_HDMI_VS_BYTE1_OUI_VER_1_4               0x03
#define NVT_HDMI_VS_BYTE1_OUI_VER_2_0               0xD8

#define NVT_HDMI_VS_BYTE2_OUI_MASK                  0xff
#define NVT_HDMI_VS_BYTE2_OUI_SHIFT                 0x00
#define NVT_HDMI_VS_BYTE2_OUI_VER_1_4               0x0C
#define NVT_HDMI_VS_BYTE2_OUI_VER_2_0               0x5D

#define NVT_HDMI_VS_BYTE3_OUI_MASK                  0xff
#define NVT_HDMI_VS_BYTE3_OUI_SHIFT                 0x00
#define NVT_HDMI_VS_BYTE3_OUI_VER_1_4               0x00
#define NVT_HDMI_VS_BYTE3_OUI_VER_2_0               0xC4

//
#define NVT_HDMI_VS_BYTE4_RSVD_MASK                 0x1f
#define NVT_HDMI_VS_BYTE4_RSVD_SHIFT                0x00
#define NVT_HDMI_VS_BYTE4_HDMI_VID_FMT_MASK         0xe0
#define NVT_HDMI_VS_BYTE4_HDMI_VID_FMT_SHIFT        0x05
#define NVT_HDMI_VS_BYTE4_HDMI_VID_FMT_NONE         0x00
#define NVT_HDMI_VS_BYTE4_HDMI_VID_FMT_EXT          0x01
#define NVT_HDMI_VS_BYTE4_HDMI_VID_FMT_3D           0x02
// 0x03-0x07 reserved
//
#define NVT_HDMI_VS_BYTE5_HDMI_VIC_MASK             0xff    // HDMI_VID_FMT = HDMI_VID_FMT_EXT
#define NVT_HDMI_VS_BYTE5_HDMI_VIC_SHIFT            0x00
#define NVT_HDMI_VS_BYTE5_HDMI_VIC_NA               0xfe
#define NVT_HDMI_VS_BYTE5_HDMI_VIC_RSVD             0x00
#define NVT_HDMI_VS_BYTE5_HDMI_VIC_4Kx2Kx30Hz       0x01
#define NVT_HDMI_VS_BYTE5_HDMI_VIC_4Kx2Kx25Hz       0x02
#define NVT_HDMI_VS_BYTE5_HDMI_VIC_4Kx2Kx24Hz       0x03
#define NVT_HDMI_VS_BYTE5_HDMI_VIC_4Kx2Kx24Hz_SMPTE 0x04
// 0x05-0xff reserved
//
#define NVT_HDMI_VS_BYTE5_HDMI_RSVD_MASK            0x07    // HDMI_VID_FMT = HDMI_VID_FMT_3D
#define NVT_HDMI_VS_BYTE5_HDMI_RSVD_SHIFT           0x00
#define NVT_HDMI_VS_BYTE5_3D_META_PRESENT_MASK      0x01
#define NVT_HDMI_VS_BYTE5_3D_META_PRESENT_SHIFT     0x03
#define NVT_HDMI_VS_BYTE5_HDMI_META_PRESENT_NOTPRES 0x00    // HDMI Metadata is not present
#define NVT_HDMI_VS_BYTE5_HDMI_META_PRESENT_PRES    0x01    // HDMI Metadata is present
#define NVT_HDMI_VS_BYTE5_ALLM_MODE_MASK            0x02    // ALLM is field of length 1 bit at Bit Number 1
#define NVT_HDMI_VS_BYTE5_ALLM_MODE_DIS             0x00
#define NVT_HDMI_VS_BYTE5_ALLM_MODE_EN              0x01
#define NVT_HDMI_VS_BYTE5_ALLM_MODE_SHIFT           0x01    // ALLM is byte5 bit position 1, so shift 1 bit
#define NVT_HDMI_VS_BYTE5_HDMI_3DS_MASK             0xf0
#define NVT_HDMI_VS_BYTE5_HDMI_3DS_SHIFT            0x04
#define NVT_HDMI_VS_BYTE5_HDMI_3DS_NA               0xfe
#define NVT_HDMI_VS_BYTE5_HDMI_3DS_FRAMEPACK        0x00
#define NVT_HDMI_VS_BYTE5_HDMI_3DS_FIELD_ALT        0x01
#define NVT_HDMI_VS_BYTE5_HDMI_3DS_LINE_ALT         0x02
#define NVT_HDMI_VS_BYTE5_HDMI_3DS_SIDEBYSIDEFULL   0x03
#define NVT_HDMI_VS_BYTE5_HDMI_3DS_LDEPTH           0x04
#define NVT_HDMI_VS_BYTE5_HDMI_3DS_LDEPTHGFX        0x05
#define NVT_HDMI_VS_BYTE5_HDMI_3DS_TOPBOTTOM        0x06
//0x06-0x07 reserved
#define NVT_HDMI_VS_BYTE5_HDMI_3DS_SIDEBYSIDEHALF   0x08
//0x09-0x0f reserved
//
// bytes 6-21 are optional depending on the 3D mode & the presence/abcense of metadata
#define NVT_HDMI_VS_BYTE_OPT1_HDMI_RSVD_MASK            0x0f    // HDMI_VID_FMT = HDMI_VID_FMT_3D
#define NVT_HDMI_VS_BYTE_OPT1_HDMI_RSVD_SHIFT           0x00
#define NVT_HDMI_VS_BYTE_OPT1_HDMI_3DEX_MASK            0xf0
#define NVT_HDMI_VS_BYTE_OPT1_HDMI_3DEX_SHIFT           0x04
#define NVT_HDMI_VS_BYTE_OPT1_HDMI_3DEX_NA              0xfe    // Extended data is not applicable
#define NVT_HDMI_VS_BYTE_OPT1_HDMI_3DEX_SSH             0x01    // Horizontal subsampling 1.4a defines a single subsampling vs 1.4s 4.
#define NVT_HDMI_VS_BYTE_OPT1_HDMI_3DEX_SSH_H_OL_OR     0x00    // Horizontal subsampling    Odd  Left    Odd  Right
#define NVT_HDMI_VS_BYTE_OPT1_HDMI_3DEX_SSH_H_OL_ER     0x01    // Horizontal subsampling    Odd  Left    Even Right
#define NVT_HDMI_VS_BYTE_OPT1_HDMI_3DEX_SSH_H_EL_OR     0x02    // Horizontal subsampling    Even Left    Odd  Right
#define NVT_HDMI_VS_BYTE_OPT1_HDMI_3DEX_SSH_H_EL_ER     0x03    // Horizontal subsampling    Even Left    Even Right
#define NVT_HDMI_VS_BYTE_OPT1_HDMI_3DEX_SSH_Q_OL_OR     0x04    // Quincunx matrix           Odd  Left    Odd  Right
#define NVT_HDMI_VS_BYTE_OPT1_HDMI_3DEX_SSH_Q_OL_ER     0x05    // Quincunx matrix           Odd  Left    Even Right
#define NVT_HDMI_VS_BYTE_OPT1_HDMI_3DEX_SSH_Q_EL_OR     0x06    // Quincunx matrix           Even Left    Odd  Right
#define NVT_HDMI_VS_BYTE_OPT1_HDMI_3DEX_SSH_Q_EL_ER     0x07    // Quincunx matrix           Even Left    Even Right
//0x08-0x0f reserved
#define NVT_HDMI_VS_BYTE_OPT2_HDMI_METADATA_LEN_MASK    0xf0    // HDMI_VID_FMT = HDMI_VID_FMT_3D; HDMI_3D_META_PRESENT = 1
#define NVT_HDMI_VS_BYTE_OPT2_HDMI_METADATA_LEN_SHIFT   0x04    //
#define NVT_HDMI_VS_BYTE_OPT2_HDMI_METADATA_LEN_NONE    0x00    // length of no metadata
#define NVT_HDMI_VS_BYTE_OPT2_HDMI_METADATA_LEN_PARALLAX 0x08   // length of paralax data

#define NVT_HDMI_VS_BYTE_OPT2_HDMI_METADATA_TYPE_MASK   0x0f    // HDMI_VID_FMT = HDMI_VID_FMT_3D
#define NVT_HDMI_VS_BYTE_OPT2_HDMI_METADATA_TYPE_SHIFT  0x00
#define NVT_HDMI_VS_BYTE_OPT2_HDMI_METADATA_TYPE_PARALLAX 0x00  // parallax metadata in the frame
#define NVT_HDMI_VS_BYTE_OPT2_HDMI_METADATA_TYPE_NA     0xfe    // no metadata in the frame

#define NVT_HDMI_VS_BYTENv_RSVD_MASK                 0xff    // if last byte of infoframe, will move depending on HDMI_VID_FMT, 3D metadata present, 3D_Metadata type.
#define NVT_HDMI_VS_BYTENv_RSVD_SHIFT                0x00
#define NVT_HDMI_VS_BYTENv_RSVD                      0x00


// Extended Metadata Packet (HDMI 2.1 specific)
typedef struct tagNVT_EXTENDED_METADATA_PACKET_INFOFRAME_PAYLOAD
{
    // byte 1~7
    NvU8 byte1;
    NvU8 byte2;
    NvU8 byte3;
    NvU8 byte4;
    NvU8 byte5;
    NvU8 byte6;
    NvU8 byte7;

    NvU8 metadataBytes[21];
} NVT_EXTENDED_METADATA_PACKET_INFOFRAME_PAYLOAD;

typedef struct tagNVT_EXTENDED_METADATA_PACKET_INFOFRAME
{
    NVT_EXTENDED_METADATA_PACKET_INFOFRAME_HEADER       Header;
    NVT_EXTENDED_METADATA_PACKET_INFOFRAME_PAYLOAD      Data;
} NVT_EXTENDED_METADATA_PACKET_INFOFRAME;

#define NVT_HDMI_EMP_BYTE1_RSVD_MASK                                                0x01
#define NVT_HDMI_EMP_BYTE1_RSVD_SHIFT                                               0

#define NVT_HDMI_EMP_BYTE1_SYNC_MASK                                                0x02
#define NVT_HDMI_EMP_BYTE1_SYNC_SHIFT                                               1
#define NVT_HDMI_EMP_BYTE1_SYNC_DISABLE                                             0
#define NVT_HDMI_EMP_BYTE1_SYNC_ENABLE                                              1

#define NVT_HDMI_EMP_BYTE1_VFR_MASK                                                 0x04
#define NVT_HDMI_EMP_BYTE1_VFR_SHIFT                                                2
#define NVT_HDMI_EMP_BYTE1_VFR_DISABLE                                              0
#define NVT_HDMI_EMP_BYTE1_VFR_ENABLE                                               1

#define NVT_HDMI_EMP_BYTE1_AFR_MASK                                                 0x08
#define NVT_HDMI_EMP_BYTE1_AFR_SHIFT                                                3
#define NVT_HDMI_EMP_BYTE1_AFR_DISABLE                                              0
#define NVT_HDMI_EMP_BYTE1_AFR_ENABLE                                               1

#define NVT_HDMI_EMP_BYTE1_DS_TYPE_MASK                                             0x30
#define NVT_HDMI_EMP_BYTE1_DS_TYPE_SHIFT                                            4
#define NVT_HDMI_EMP_BYTE1_DS_TYPE_PERIODIC_PSEUDO_STATIC                           0
#define NVT_HDMI_EMP_BYTE1_DS_TYPE_PERIODIC_DYNAMIC                                 1
#define NVT_HDMI_EMP_BYTE1_DS_TYPE_UNIQUE                                           2
#define NVT_HDMI_EMP_BYTE1_DS_TYPE_RSVD                                             3

#define NVT_HDMI_EMP_BYTE1_END_MASK                                                 0x40
#define NVT_HDMI_EMP_BYTE1_END_SHIFT                                                6
#define NVT_HDMI_EMP_BYTE1_END_DISABLE                                              0
#define NVT_HDMI_EMP_BYTE1_END_ENABLE                                               1

#define NVT_HDMI_EMP_BYTE1_NEW_MASK                                                 0x80
#define NVT_HDMI_EMP_BYTE1_NEW_SHIFT                                                7
#define NVT_HDMI_EMP_BYTE1_NEW_DISABLE                                              0
#define NVT_HDMI_EMP_BYTE1_NEW_ENABLE                                               1

#define NVT_HDMI_EMP_BYTE2_RSVD_MASK                                                0xff
#define NVT_HDMI_EMP_BYTE2_RSVD_SHIFT                                               0

#define NVT_HDMI_EMP_BYTE3_ORGANIZATION_ID_MASK                                     0xff
#define NVT_HDMI_EMP_BYTE3_ORGANIZATION_ID_SHIFT                                    0
#define NVT_HDMI_EMP_BYTE3_ORGANIZATION_ID_VENDOR_SPECIFIC                          0
#define NVT_HDMI_EMP_BYTE3_ORGANIZATION_ID_SPEC_DEFINED                             1
#define NVT_HDMI_EMP_BYTE3_ORGANIZATION_ID_CTA_DEFINED                              2
#define NVT_HDMI_EMP_BYTE3_ORGANIZATION_ID_VESA_DEFINED                             3

#define NVT_HDMI_EMP_BYTE4_DATA_SET_TAG_MSB_MASK                                    0xff
#define NVT_HDMI_EMP_BYTE4_DATA_SET_TAG_MSB_SHIFT                                   0

#define NVT_HDMI_EMP_BYTE5_DATA_SET_TAG_LSB_MASK                                    0xff
#define NVT_HDMI_EMP_BYTE5_DATA_SET_TAG_LSB_SHIFT                                   0

#define NVT_HDMI_EMP_BYTE6_DATA_SET_LENGTH_MSB_MASK                                 0xff
#define NVT_HDMI_EMP_BYTE6_DATA_SET_LENGTH_MSB_SHIFT                                0

#define NVT_HDMI_EMP_BYTE7_DATA_SET_LENGTH_LSB_MASK                                 0xff
#define NVT_HDMI_EMP_BYTE7_DATA_SET_LENGTH_LSB_SHIFT                                0

#define NVT_HDMI_EMP_BYTE8_MD0_VRR_EN_MASK                                          0x01
#define NVT_HDMI_EMP_BYTE8_MD0_VRR_EN_SHIFT                                         0
#define NVT_HDMI_EMP_BYTE8_MD0_VRR_EN_DISABLE                                       0
#define NVT_HDMI_EMP_BYTE8_MD0_VRR_EN_ENABLE                                        1
#define NVT_HDMI_EMP_BYTE8_MD0_M_CONST_MASK                                         0x02
#define NVT_HDMI_EMP_BYTE8_MD0_M_CONST_SHIFT                                        1
#define NVT_HDMI_EMP_BYTE8_MD0_QMS_EN_MASK                                          0x04
#define NVT_HDMI_EMP_BYTE8_MD0_QMS_EN_SHIFT                                         2
#define NVT_HDMI_EMP_BYTE8_MD0_QMS_EN_DISABLE                                       0
#define NVT_HDMI_EMP_BYTE8_MD0_QMS_EN_ENABLE                                        1

#define NVT_HDMI_EMP_BYTE8_MD1_BASE_VFRONT_MASK                                     0xff
#define NVT_HDMI_EMP_BYTE8_MD1_BASE_VFRONT_SHIFT                                    0

#define NVT_HDMI_EMP_BYTE8_MD2_RB_MASK                                              0x04
#define NVT_HDMI_EMP_BYTE8_MD2_RB_SHIFT                                             2
#define NVT_HDMI_EMP_BYTE8_MD2_RB_DISABLE                                           0
#define NVT_HDMI_EMP_BYTE8_MD2_RB_ENABLE                                            1

#define NVT_HDMI_EMP_BYTE8_MD2_NEXT_TFR_MASK                                        0xf8
#define NVT_HDMI_EMP_BYTE8_MD2_NEXT_TFR_SHIFT                                       3

#define NVT_HDMI_EMP_BYTE8_MD2_BASE_RR_MSB_MASK                                     0x03
#define NVT_HDMI_EMP_BYTE8_MD2_BASE_RR_MSB_SHIFT                                    0

#define NVT_HDMI_EMP_BYTE8_MD3_BASE_RR_LSB_MASK                                     0xff
#define NVT_HDMI_EMP_BYTE8_MD3_BASE_RR_LSB_SHIFT                                    0

#define NVT_DP_ADAPTIVE_SYNC_SDP_PACKET_TYPE                                        0x22
#define NVT_DP_ADAPTIVE_SYNC_SDP_VERSION                                            0x2
#define NVT_DP_ADAPTIVE_SYNC_SDP_LENGTH                                             0x9
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB0_VARIABLE_FRAME_RATE_MASK                       0x3
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB0_VARIABLE_FRAME_RATE_SHIFT                      0
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB0_VARIABLE_FRAME_RATE_AVT_VARIABLE               0
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB0_VARIABLE_FRAME_RATE_AVT_FIXED                  1
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB0_VARIABLE_FRAME_RATE_FAVT_TARGET_NOT_REACHED    2
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB0_VARIABLE_FRAME_RATE_FAVT_TARGET_REACHED        3

#define NVT_DP_ADAPTIVE_SYNC_SDP_DB0_DISABLE_PR_ACTIVE_MASK                         0x4
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB0_DISABLE_PR_ACTIVE_SHIFT                        2
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB0_DISABLE_PR_ACTIVE_SOURCE_SINK_SYNC_ENABLED     0
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB0_DISABLE_PR_ACTIVE_SOURCE_SINK_SYNC_DISABLED    1

#define NVT_DP_ADAPTIVE_SYNC_SDP_DB0_DISABLE_PR_ACTIVE_RFB_UPDATE_MASK              0x8
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB0_DISABLE_PR_ACTIVE_RFB_UPDATE_SHIFT             3
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB0_DISABLE_PR_ACTIVE_RFB_UPDATE_NO_UPDATE         0
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB0_DISABLE_PR_ACTIVE_RFB_UPDATE_UPDATE            1

#define NVT_DP_ADAPTIVE_SYNC_SDP_DB0_RSVD_MASK                                      0xf0
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB0_RSVD_SHIFT                                     4

#define NVT_DP_ADAPTIVE_SYNC_SDP_DB1_MIN_VTOTAL_LSB_MASK                            0xff
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB1_MIN_VTOTAL_LSB_SHIFT                           0

#define NVT_DP_ADAPTIVE_SYNC_SDP_DB2_MIN_VTOTAL_MSB_MASK                            0xff
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB2_MIN_VTOTAL_MSB_SHIFT                           0

#define NVT_DP_ADAPTIVE_SYNC_SDP_DB3_TARGET_RR_LSB_MASK                             0xff
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB3_TARGET_RR_LSB_SHIFT                            0

#define NVT_DP_ADAPTIVE_SYNC_SDP_DB4_TARGET_RR_MSB_MASK                             0x03
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB4_TARGET_RR_MSB_SHIFT                            0

#define NVT_DP_ADAPTIVE_SYNC_SDP_DB4_RSVD_MASK                                      0x1c
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB4_RSVD_SHIFT                                     2

#define NVT_DP_ADAPTIVE_SYNC_SDP_DB4_TARGET_RR_DIVIDER_MASK                         0x20
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB4_TARGET_RR_DIVIDER_SHIFT                        5
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB4_TARGET_RR_DIVIDER_DISABLE                      0
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB4_TARGET_RR_DIVIDER_ENABLE                       1

#define NVT_DP_ADAPTIVE_SYNC_SDP_DB4_SUCCESSIVE_FRAME_INC_MASK                      0x40
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB4_SUCCESSIVE_FRAME_INC_SHIFT                     6
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB4_SUCCESSIVE_FRAME_INC_DISABLE                   0
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB4_SUCCESSIVE_FRAME_INC_ENABLE                    1

#define NVT_DP_ADAPTIVE_SYNC_SDP_DB4_SUCCESSIVE_FRAME_DEC_MASK                      0x80
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB4_SUCCESSIVE_FRAME_DEC_SHIFT                     7
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB4_SUCCESSIVE_FRAME_DEC_DISABLE                   0
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB4_SUCCESSIVE_FRAME_DEC_ENABLE                    1

#define NVT_DP_ADAPTIVE_SYNC_SDP_DB5_DURATION_INCREASE_CONSTRAINT_LSB_MASK          0xff
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB5_DURATION_INCREASE_CONSTRAINT_LSB_SHIFT         0

#define NVT_DP_ADAPTIVE_SYNC_SDP_DB6_DURATION_INCREASE_CONSTRAINT_MSB_MASK          0xff
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB6_DURATION_INCREASE_CONSTRAINT_MSB_SHIFT         0

#define NVT_DP_ADAPTIVE_SYNC_SDP_DB7_PR_COASTING_VTOTAL_LSB_MASK                    0xff
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB7_PR_COASTING_VTOTAL_LSB_SHIFT                   0

#define NVT_DP_ADAPTIVE_SYNC_SDP_DB8_PR_COASTING_VTOTAL_MSB_MASK                    0xff
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB8_PR_COASTING_VTOTAL_MSB_SHIFT                   0

#define NVT_DP_ADAPTIVE_SYNC_SDP_DB1_MIN_VTOTAL_BYTE2_MASK                          0xff
#define NVT_DP_ADAPTIVE_SYNC_SDP_DB1_MIN_VTOTAL_BYTE2_SHIFT                         0

typedef struct tagNVT_ADAPTIVE_SYNC_SDP_HEADER
{
    NvU8 hb0;
    NvU8 type;
    NvU8 version;
    NvU8 length;
}NVT_ADAPTIVE_SYNC_SDP_HEADER;

typedef struct tagNVT_ADAPTIVE_SYNC_SDP_PAYLOAD
{
    NvU8 db0; // operatingMode
    NvU8 db1; // minVTotalLSB
    NvU8 db2; // minVTotalMSB
    NvU8 db3; // targetRefreshRateLSB
    NvU8 db4; // targetRefreshRateMSB, rsvd, targetRRDivider, frameInc/Dec Config
    NvU8 db5; // frameDurationIncMs
    NvU8 db6; // frameDurationDecreaseMs
    NvU8 db7; // coastingVTotalPrLSB
    NvU8 db8; // coastingVTotalPrMSB

    NvU8 rsvd[23];
}NVT_ADAPTIVE_SYNC_SDP_PAYLOAD;

typedef struct tagADAPTIVE_SYNC_SDP
{
    NVT_ADAPTIVE_SYNC_SDP_HEADER header;
    NVT_ADAPTIVE_SYNC_SDP_PAYLOAD payload;
}NVT_ADAPTIVE_SYNC_SDP;

// the Vendor-Specific-Data-Block header
typedef struct tagNVT_CEA861_VSDB_HEADER
{
    // byte 0
    NvU32    length                     : 5;
    NvU32    vendorSpecificTag          : 3;
    // byte 1-3
    NvU32    ieee_id                    : 24;

} NVT_CEA861_VSDB_HEADER;

// HDMI LLC Vendor-Specific data block
// from HDMI 1.4 spec (superset of VSDB from HDMI 1.3a spec)
typedef struct tagNVT_CEA861_LATENCY
{
    NvU8       Video_Latency;
    NvU8       Audio_Latency;

} NVT_CEA861_LATENCY;

typedef struct tagNVT_HDMI_VIDEO
{
    NvU8       Rsvd_1                   : 3;
    NvU8       ImageSize                : 2;
    NvU8       ThreeD_Multi_Present     : 2;
    NvU8       ThreeD_Present           : 1;
    NvU8       HDMI_3D_Len              : 5;
    NvU8       HDMI_VIC_Len             : 3;
} NVT_HDMI_VIDEO;

typedef struct tagNVT_HDMI_VIC_LIST
{
    NvU8       HDMI_VIC[1];             // note: list length is actually specified in HDMI_VIC_Len
} NVT_HDMI_VIC_LIST;

typedef struct tagNVT_3D_STRUCT_ALL
{
    NvU8    ThreeDStructALL0_FramePacking : 1;
    NvU8    ThreeDStructALL1_FieldAlt     : 1;
    NvU8    ThreeDStructALL2_LineAlt      : 1;
    NvU8    ThreeDStructALL3_SSFull       : 1;
    NvU8    ThreeDStructALL4_LDepth       : 1;
    NvU8    ThreeDStructALL5_LDepthGFX    : 1;
    NvU8    ThreeDStructALL6_TopBottom    : 1;
    NvU8    ThreeDStructALL7              : 1;
    NvU8    ThreeDStructALL8_SSHalf       : 1;
    NvU8    Rsvd_1                        : 7;
} NVT_3D_STRUCT_ALL;

typedef struct tagNVT_3D_MULTI_LIST
{
    NvU8       ThreeD_Structure           : 4;
    NvU8       TwoD_VIC_order             : 4;
    NvU8       Rsvd_2                     : 4;
    NvU8       ThreeD_Detail              : 4;
} NVT_3D_MULTI_LIST;

#define NVT_3D_DETAILS_ALL                                 0x00
#define NVT_3D_DETAILS_ALL_HORIZONTAL                      0x01
#define NVT_3D_DETAILS_HORIZONTAL_ODD_LEFT_ODD_RIGHT       0x02
#define NVT_3D_DETAILS_HORIZONTAL_ODD_LEFT_EVEN_RIGHT      0x03
#define NVT_3D_DETAILS_HORIZONTAL_EVEN_LEFT_ODD_RIGHT      0x04
#define NVT_3D_DETAILS_HORIZONTAL_EVEN_LEFT_EVEN_RIGHT     0x05
#define NVT_3D_DETAILS_ALL_QUINCUNX                        0x06
#define NVT_3D_DETAILS_QUINCUNX_ODD_LEFT_ODD_RIGHT         0x07
#define NVT_3D_DETAILS_QUINCUNX_ODD_LEFT_EVEN_RIGHT        0x08
#define NVT_3D_DETAILS_QUINCUNX_EVEN_LEFT_ODD_RIGHT        0x09
#define NVT_3D_DETAILS_QUINCUNX_EVEN_LEFT_EVEN_RIGHT       0x0a

typedef struct tagNVT_HDMI_LLC_VSDB_PAYLOAD
{
    // 1st byte
    NvU8       B                        : 4;
    NvU8       A                        : 4;
    // 2nd byte
    NvU8       D                        : 4;
    NvU8       C                        : 4;
    // 3rd byte
    NvU8       DVI_Dual                 : 1;
    NvU8       Rsvd_3                   : 2;
    NvU8       DC_Y444                  : 1;
    NvU8       DC_30bit                 : 1;
    NvU8       DC_36bit                 : 1;
    NvU8       DC_48bit                 : 1;
    NvU8       Supports_AI              : 1;
    // 4th byte
    NvU8       Max_TMDS_Clock;
    // 5th byte
    NvU8       CNC0                     : 1;
    NvU8       CNC1                     : 1;
    NvU8       CNC2                     : 1;
    NvU8       CNC3                     : 1;
    NvU8       Rsvd_5                   : 1;
    NvU8       HDMI_Video_present       : 1;
    NvU8       I_Latency_Fields_Present : 1;
    NvU8       Latency_Fields_Present   : 1;

    // the rest of the frame may contain optional data as defined
    // in the NVT_CEA861_LATENCY, HDMI_VIDEO, HDMI_VIC, NVT_3D_STRUCT_ALL & 3D_MULTI_LIST structures
    // and as specified by the corresponding control bits
    NvU8       Data[NVT_CEA861_VSDB_PAYLOAD_MAX_LENGTH - 5];

} NVT_HDMI_LLC_VSDB_PAYLOAD;

// HDMI LLC Vendor Specific Data Block
typedef struct tagNVT_HDMI_LLC_DATA
{
    NVT_CEA861_VSDB_HEADER              header;
    NVT_HDMI_LLC_VSDB_PAYLOAD           payload;
} NVT_HDMI_LLC_DATA;

typedef struct tagNVT_NVDA_VSDB_PAYLOAD
{
    NvU8    opcode;                     // Nvidia specific opcode - please refer to VRR monitor spec v17
    NvU8    vrrMinRefreshRate;          // Minimum refresh rate supported by this monitor
} NVT_NVDA_VSDB_PAYLOAD;

// NVIDIA Vendor Specific Data Block
typedef struct tagNVT_NVDA_VSDB_DATA
{
    NVT_CEA861_VSDB_HEADER              header;
    NVT_NVDA_VSDB_PAYLOAD               payload;
} NVT_NVDA_VSDB_DATA;

typedef struct _NVT_MSFT_VSDB_PAYLOAD
{
    NvU8    version;
    NvU8    primaryUseCase      : 5;
    NvU8    thirdPartyUsage     : 1;
    NvU8    desktopUsage        : 1;
    NvU8    reserved            : 1;
    NvU8    containerId[MSFT_VSDB_CONTAINER_ID_SIZE];
} NVT_MSFT_VSDB_PAYLOAD;

typedef struct _NVT_MSFT_VSDB_DATA
{
    NVT_CEA861_VSDB_HEADER              header;
    NVT_MSFT_VSDB_PAYLOAD               payload;
} NVT_MSFT_VSDB_DATA;

#define NVT_MSFT_VSDB_BLOCK_SIZE    (sizeof(NVT_MSFT_VSDB_DATA))

typedef struct tagNVT_HDMI_FORUM_VSDB_PAYLOAD
{
    // first byte
    NvU8    Version;
    // second byte
    NvU8    Max_TMDS_Character_Rate;
    // third byte
    NvU8    ThreeD_Osd_Disparity    : 1;
    NvU8    Dual_View               : 1;
    NvU8    Independent_View        : 1;
    NvU8    Lte_340mcsc_Scramble    : 1;
    NvU8    CCBPCI                  : 1;
    NvU8    CABLE_STATUS            : 1;
    NvU8    RR_Capable              : 1;
    NvU8    SCDC_Present            : 1;
    // fourth byte
    NvU8    DC_30bit_420            : 1;
    NvU8    DC_36bit_420            : 1;
    NvU8    DC_48bit_420            : 1;
    NvU8    UHD_VIC                 : 1;
    NvU8    Max_FRL_Rate            : 4;
    // fifth byte
    NvU8    FAPA_start_location     : 1;
    NvU8    ALLM                    : 1;
    NvU8    FVA                     : 1;
    NvU8    CNMVRR                  : 1;
    NvU8    CinemaVRR               : 1;
    NvU8    M_delta                 : 1;
    NvU8    QMS                     : 1;
    NvU8    FAPA_End_Extended       : 1;

    // sixth byte
    NvU8    VRR_min                 : 6;
    NvU8    VRR_max_high            : 2;
    // seventh byte
    NvU8    VRR_max_low             : 8;
    // eighth byte
    NvU8    DSC_10bpc               : 1;
    NvU8    DSC_12bpc               : 1;
    NvU8    DSC_16bpc               : 1;
    NvU8    DSC_All_bpp             : 1;
    NvU8    QMS_TFR_min             : 1;
    NvU8    QMS_TFR_max             : 1;
    NvU8    DSC_Native_420          : 1;
    NvU8    DSC_1p2                 : 1;
    // ninth byte
    NvU8    DSC_MaxSlices           : 4;
    NvU8    DSC_Max_FRL_Rate        : 4;
    // tenth byte
    NvU8    DSC_totalChunkKBytes    : 6;
    NvU8    Rsvd_4                  : 2;
} NVT_HDMI_FORUM_VSDB_PAYLOAD;

// HDMI Forum Vendor Specific Data Block
typedef struct tagNVT_HDMI_FORUM_DATA
{
    NVT_CEA861_VSDB_HEADER              header;
    NVT_HDMI_FORUM_VSDB_PAYLOAD         payload;
} NVT_HDMI_FORUM_DATA;

//
//
// Video Capability Data Block (VCDB)
typedef struct _NV_ESC_MONITOR_CAPS_VCDB
{
    NvU8    quantizationRangeYcc         : 1;
    NvU8    quantizationRangeRgb         : 1;
    NvU8    scanInfoPreferredVideoFormat : 2;
    NvU8    scanInfoITVideoFormats       : 2;
    NvU8    scanInfoCEVideoFormats       : 2;
} NVT_HDMI_VCDB_DATA;

//
//
//***********************************************************
// Dynamic Range and Mastering Infoframe (HDR)
//***********************************************************
//
typedef struct tagNVT_HDR_INFOFRAME_MASTERING_DATA
{
    NvU16    displayPrimary_x0;                //!< x coordinate of color primary 0 (e.g. Red) of mastering display ([0x0000-0xC350] = [0.0 - 1.0])
    NvU16    displayPrimary_y0;                //!< y coordinate of color primary 0 (e.g. Red) of mastering display ([0x0000-0xC350] = [0.0 - 1.0])

    NvU16    displayPrimary_x1;                //!< x coordinate of color primary 1 (e.g. Green) of mastering display ([0x0000-0xC350] = [0.0 - 1.0])
    NvU16    displayPrimary_y1;                //!< y coordinate of color primary 1 (e.g. Green) of mastering display ([0x0000-0xC350] = [0.0 - 1.0])

    NvU16    displayPrimary_x2;                //!< x coordinate of color primary 2 (e.g. Blue) of mastering display ([0x0000-0xC350] = [0.0 - 1.0])
    NvU16    displayPrimary_y2;                //!< y coordinate of color primary 2 (e.g. Blue) of mastering display ([0x0000-0xC350] = [0.0 - 1.0])

    NvU16    displayWhitePoint_x;              //!< x coordinate of white point of mastering display ([0x0000-0xC350] = [0.0 - 1.0])
    NvU16    displayWhitePoint_y;              //!< y coordinate of white point of mastering display ([0x0000-0xC350] = [0.0 - 1.0])

    NvU16    max_display_mastering_luminance;  //!< Maximum display mastering luminance ([0x0001-0xFFFF] = [1.0 - 65535.0] cd/m^2)
    NvU16    min_display_mastering_luminance;  //!< Minimum display mastering luminance ([0x0001-0xFFFF] = [1.0 - 6.55350] cd/m^2)

    NvU16    max_content_light_level;          //!< Maximum Content Light level (MaxCLL) ([0x0001-0xFFFF] = [1.0 - 65535.0] cd/m^2)
    NvU16    max_frame_average_light_level;    //!< Maximum Frame-Average Light Level (MaxFALL) ([0x0001-0xFFFF] = [1.0 - 65535.0] cd/m^2)
} NVT_HDR_INFOFRAME_MASTERING_DATA;

#define NVT_CEA861_HDR_INFOFRAME_EOTF_SDR_GAMMA  0      //SDR Luminance Range
#define NVT_CEA861_HDR_INFOFRAME_EOTF_HDR_GAMMA  1      //HDR Luminance Range
#define NVT_CEA861_HDR_INFOFRAME_EOTF_ST2084     2
#define NVT_CEA861_HDR_INFOFRAME_EOTF_Future     3
#define NVT_CEA861_STATIC_METADATA_TYPE1_PRIMARY_COLOR_NORMALIZE_FACTOR 0xC350  // Per CEA-861.3 spec

typedef struct tagNVT_HDR_INFOFRAME_PAYLOAD
{
    //byte 1
    NvU8    eotf                    : 3;
    NvU8    f13_17                  : 5;     // These bits are reserved for future use
    //byte 2
    NvU8    static_metadata_desc_id : 3;
    NvU8    f23_27                  : 5;     // These bits are reserved for future use

    NVT_HDR_INFOFRAME_MASTERING_DATA type1;
} NVT_HDR_INFOFRAME_PAYLOAD;

#pragma pack(1)
typedef struct tagNVT_HDR_INFOFRAME
{
    NVT_INFOFRAME_HEADER            header;
    NVT_HDR_INFOFRAME_PAYLOAD       payload;
} NVT_HDR_INFOFRAME;
#pragma pack()

//
//
//***********************************************************
// Gamut Metadata Range and Vertices structures
//***********************************************************
//
// GBD structure formats
//
#define NVT_GAMUT_FORMAT_VERTICES   0
#define NVT_GAMUT_FORMAT_RANGE      1

typedef struct tagNVT_GAMUT_HEADER
{
    NvU8    type:8;

    // byte 1
    NvU8    AGSNum:4;
    NvU8    GBD_profile:3;
    NvU8    Next_Field:1;

    // byte 2
    NvU8    CGSNum:4;
    NvU8    Packet_Seq:2;
    NvU8    Rsvd:1;
    NvU8    No_Cmt_GBD:1;

} NVT_GAMUT_HEADER;

typedef struct tagNVT_GAMUT_METADATA_RANGE_8BIT{

    // Header
    NvU8    GBD_Color_Space:3;
    NvU8    GBD_Color_Precision:2;
    NvU8    Rsvd:2;
    NvU8    Format_Flag:1;

    // Packaged data
    NvU8    Min_Red_Data:8;
    NvU8    Max_Red_Data:8;
    NvU8    Min_Green_Data:8;
    NvU8    Max_Green_Data:8;
    NvU8    Min_Blue_Data:8;
    NvU8    Max_Blue_Data:8;
} NVT_GAMUT_METADATA_RANGE_8BIT;

typedef struct tagNVT_GAMUT_METADATA_RANGE_10BIT{

    // Header
    NvU8    GBD_Color_Space:3;
    NvU8    GBD_Color_Precision:2;
    NvU8    Rsvd:2;
    NvU8    Format_Flag:1;

    // Packaged data
    NvU8    Min_Red_Data_HI:8;

    NvU8    Max_Red_Data_HI:6;
    NvU8    Min_Red_Data_LO:2;

    NvU8    Min_Green_Data_HI:4;
    NvU8    Max_Red_Data_LO:4;

    NvU8    Max_Green_Data_HI:2;
    NvU8    Min_Green_Data_LO:6;

    NvU8    Max_Green_Data_LO:8;

    NvU8    Min_Blue_Data_HI:8;

    NvU8    Max_Blue_Data_HI:6;
    NvU8    Min_Blue_Data_LO:2;

    NvU8    Data_Rsvd:4;
    NvU8    Max_Blue_Data_LO:4;

} NVT_GAMUT_METADATA_RANGE_10BIT;

typedef struct tagNVT_GAMUT_METADATA_RANGE_12BIT{

    // Header
    NvU8    GBD_Color_Space:3;
    NvU8    GBD_Color_Precision:2;
    NvU8    Rsvd:2;
    NvU8    Format_Flag:1;

    // Packaged data
    NvU8    Min_Red_Data_HI:8;

    NvU8    Max_Red_Data_HI:4;
    NvU8    Min_Red_Data_LO:4;

    NvU8    Max_Red_Data_LO:8;

    NvU8    Min_Green_Data_HI:8;

    NvU8    Max_Green_Data_HI:4;
    NvU8    Min_Green_Data_LO:4;

    NvU8    Max_Green_Data_LO:8;

    NvU8    Min_Blue_Data_HI:8;

    NvU8    Max_Blue_Data_HI:4;
    NvU8    Min_Blue_Data_LO:4;

    NvU8    Max_Blue_Data_LO:8;

} NVT_GAMUT_METADATA_RANGE_12BIT;

typedef struct tagNVT_GAMUT_METADATA_VERTICES_8BIT
{
    // Header
    NvU8    GBD_Color_Space:3;
    NvU8    GBD_Color_Precision:2;
    NvU8    Rsvd:1;
    NvU8    Facet_Mode:1;           // Must be set to 0
    NvU8    Format_Flag:1;          // Must be set to 0
    NvU8    Number_Vertices_H:8;    // Must be set to 0
    NvU8    Number_Vertices_L:8;    // Must be set to 4

    // Packaged data
    NvU8    Black_Y_R;
    NvU8    Black_Cb_G;
    NvU8    Black_Cr_B;
    NvU8    Red_Y_R;
    NvU8    Red_Cb_G;
    NvU8    Red_Cr_B;
    NvU8    Green_Y_R;
    NvU8    Green_Cb_G;
    NvU8    Green_Cr_B;
    NvU8    Blue_Y_R;
    NvU8    Blue_Cb_G;
    NvU8    Blue_Cr_B;
} NVT_GAMUT_METADATA_VERTICES_8BIT;

typedef struct tagNVT_GAMUT_METADATA_VERTICES_10BIT
{
    // Header
    NvU8    GBD_Color_Space:3;
    NvU8    GBD_Color_Precision:2;
    NvU8    Rsvd:1;
    NvU8    Facet_Mode:1;           // Must be set to 0
    NvU8    Format_Flag:1;          // Must be set to 0
    NvU8    Number_Vertices_H:8;    // Must be set to 0
    NvU8    Number_Vertices_L:8;    // Must be set to 4

    // Packaged data
    NvU8    Black_Y_R_HI;

    NvU8    Black_Cb_G_HI:6;
    NvU8    Black_Y_R_LO:2;

    NvU8    Black_Cr_B_HI:4;
    NvU8    Black_Cb_G_LO:4;

    NvU8    Red_Y_R_HI:2;
    NvU8    Black_Cr_B_LO:6;

    NvU8    Red_Y_R_LO;

    NvU8    Red_Cb_G_HI;

    NvU8    Red_Cr_B_HI:6;
    NvU8    Red_Cb_G_LO:2;

    NvU8    Green_Y_R_HI:4;
    NvU8    Red_Cr_B_LO:4;

    NvU8    Green_Cb_G_HI:2;
    NvU8    Green_Y_R_LO:6;

    NvU8    Green_Cb_G_LO;

    NvU8    Green_Cr_B_HI;

    NvU8    Blue_Y_R_HI:6;
    NvU8    Green_Cr_B_LO:2;

    NvU8    Blue_Cb_G_HI:4;
    NvU8    Blue_Y_R_LO:4;

    NvU8    Blue_Cr_B_HI:2;
    NvU8    Blue_Cb_G_LO:6;

    NvU8    Blue_Cr_B_LO;
} NVT_GAMUT_METADATA_VERTICES_10BIT;

typedef struct tagNVT_GAMUT_METADATA_VERTICES_12BIT
{
    // Header
    NvU8    GBD_Color_Space:3;
    NvU8    GBD_Color_Precision:2;
    NvU8    Rsvd:1;
    NvU8    Facet_Mode:1;           // Must be set to 0
    NvU8    Format_Flag:1;          // Must be set to 0
    NvU8    Number_Vertices_H:8;    // Must be set to 0
    NvU8    Number_Vertices_L:8;    // Must be set to 4

    // Packaged data
    NvU8    Black_Y_R_HI;

    NvU8    Black_Cb_G_HI:4;
    NvU8    Black_Y_R_LO:4;

    NvU8    Black_Cb_G_LO;

    NvU8    Black_Cr_B_HI;

    NvU8    Red_Y_R_HI:4;
    NvU8    Black_Cr_B_LO:4;

    NvU8    Red_Y_R_LO;

    NvU8    Red_Cb_G_HI;

    NvU8    Red_Cr_B_HI:4;
    NvU8    Red_Cb_G_LO:4;

    NvU8    Red_Cr_B_LO;

    NvU8    Green_Y_R_HI;

    NvU8    Green_Cb_G_HI:4;
    NvU8    Green_Y_R_LO:4;

    NvU8    Green_Cb_G_LO;

    NvU8    Green_Cr_B_HI;

    NvU8    Blue_Y_R_HI:4;
    NvU8    Green_Cr_B_LO:4;

    NvU8    Blue_Y_R_LO;

    NvU8    Blue_Cb_G_HI;

    NvU8    Blue_Cr_B_HI:4;
    NvU8    Blue_Cb_G_LO:4;

    NvU8    Blue_Cr_B_LO;
} NVT_GAMUT_METADATA_VERTICES_12BIT;

typedef struct tagNVT_GAMUT_METADATA
{
    NVT_GAMUT_HEADER     header;

    union
    {
        NVT_GAMUT_METADATA_RANGE_8BIT      range8Bit;
        NVT_GAMUT_METADATA_RANGE_10BIT     range10Bit;
        NVT_GAMUT_METADATA_RANGE_12BIT     range12Bit;
        NVT_GAMUT_METADATA_VERTICES_8BIT   vertices8bit;
        NVT_GAMUT_METADATA_VERTICES_10BIT  vertices10bit;
        NVT_GAMUT_METADATA_VERTICES_12BIT  vertices12bit;
    }payload;

}NVT_GAMUT_METADATA;
//
//***********************************
// Display Port Configuration Data
//***********************************
//
// DPCD field offset
#define NVT_DPCD_ADDRESS_RECEIVER_CAPABILITY_FIELD          0x00000
#define NVT_DPCD_ADDRESS_LINK_CONFIG_FIELD                  0x00100
#define NVT_DPCD_ADDRESS_MSTM_CTRL_FIELD                    0x00111 //DPMST Control MST <-> ST
#define NVT_DPCD_ADDRESS_MSTM_BRANCH_DEVICE                 0x001A1
#define NVT_DPCD_ADDRESS_LINK_SINK_STATUS_FIELD             0x00200
#define NVT_DPCD_ADDRESS_VENDOR_SPECIFIC_SOURCE_DEVICE      0x00300
#define NVT_DPCD_ADDRESS_VENDOR_SPECIFIC_SINK_DEVICE        0x00400
#define NVT_DPCD_ADDRESS_VENDOR_SPECIFIC_BRANCH_DEVICE      0x00500
#define NVT_DPCD_ADDRESS_SINK_CTRL_FIELD                    0x00600
#define NVT_DPCD_ADDRESS_DOWN_REQ_BUFFER_FIELD              0x01000
#define NVT_DPCD_ADDRESS_UP_REP_BUFFER_FIELD                0x01200
#define NVT_DPCD_ADDRESS_DOWN_REP_BUFFER_FIELD              0x01400
#define NVT_DPCD_ADDRESS_UP_REQ_BUFFER_FIELD                0x01600
#define NVT_DPCD_ADDRESS_DEVICE_SERVICE_IRQ_VECTOR_ESI0     0x02003
#define NVT_DPCD_ADDRESS_DP_TUNNELING_DEVICE_IEEE_OUI       0xE0000
#define NVT_DPCD_ADDRESS_DP_TUNNELING_DEVICE_ID_STRING      0xE0003
#define NVT_DPCD_ADDRESS_DP_TUNNELING_CAPS_SUPPORT_FIELD    0xE000D
#define NVT_DPCD_ADDRESS_DP_IN_ADAPTER_INFO_FIELD           0xE000E
#define NVT_DPCD_ADDRESS_USB4_DRIVER_ID_FIELD               0xE000F
#define NVT_DPCD_ADDRESS_USB4_ROUTER_TOPOLOGY_ID_FIELD      0xE001B

//
// Raw DPCD data format - Receiver Capability Field               // 00000h - 000FFh
typedef struct tagNVT_DPCD_RECEIVER_CAP
{
    NvU8 rev;                                                     // 00000h
    NvU8 max_link_rate;                                           // 00001h
    NvU8 max_lane_count;                                          // 00002h
    NvU8 max_downspread;                                          // 00003h
    NvU8 norp;                                                    // 00004h
    NvU8 downstream_port_present;                                 // 00005h
    NvU8 main_link_ch_coding;                                     // 00006h
    NvU8 down_stream_port_count;                                  // 00007h
    NvU8 receive_port0_cap_0;                                     // 00008h
    NvU8 receive_port0_cap_1;                                     // 00009h
    NvU8 receive_port1_cap_0;                                     // 0000Ah
    NvU8 receive_port1_cap_1;                                     // 0000Bh
    NvU8 reserved_0[0x7F - 0xC + 1];                              // 0000Ch - 0007Fh
    NvU8 down_strm_port0_cap[0x8F - 0x80 + 1];                    // 00080h - 0008Fh
    //NvU8 reserved_1[0xFF - 0x90 + 1];                             // 00090h - 000FFh
}NVT_DPCD_RECEIVER_CAP;

//
// Raw DPCD data format - Link Configuration Field                // 00100h - 001FFh
typedef struct tagNVT_DPCD_LINK_CFG
{
    NvU8 link_bw_set;                                             // 00100h
    NvU8 lane_count_set;                                          // 00101h
    NvU8 training_pattern_set;                                    // 00102h
    NvU8 training_lane0_set;                                      // 00103h
    NvU8 training_lane1_set;                                      // 00104h
    NvU8 training_lane2_set;                                      // 00105h
    NvU8 training_lane3_set;                                      // 00106h
    NvU8 downspread_ctrl;                                         // 00107h
    NvU8 main_link_ch_coding_set;                                 // 00108h
    NvU8 reserved_0[0x110 - 0x109 + 1];                           // 00110h - 00109h
    NvU8 mstm_ctrl;                                               // 00111h
    // NvU8 reserved_0[0x1FF - 0x111 + 1];
}NVT_DPCD_LINK_CFG;
//
// Raw DPCD data format - Link/Sink Status Field                  // 00200h - 002FFh
typedef struct tagNVT_DPCD_LINK_SINK_STATUS
{
    NvU8 sink_count;                                              // 00200h
    NvU8 device_service_irq_vector;                               // 00201h
    NvU8 lane0_1_status;                                          // 00202h
    NvU8 lane2_3_status;                                          // 00203h
    NvU8 lane_align_status_update;                                // 00204h
    NvU8 sink_status;                                             // 00205h
    NvU8 adjust_req_lane0_1;                                      // 00206h
    NvU8 adjust_req_lane2_3;                                      // 00207h
    NvU8 training_score_lane0;                                    // 00208h
    NvU8 training_score_lane1;                                    // 00209h
    NvU8 training_score_lane2;                                    // 0020Ah
    NvU8 training_score_lane3;                                    // 0020Bh
    NvU8 reserved_0[0x20F - 0x20C + 1];                           // 0020Fh - 0020Ch
    NvU16 sym_err_count_lane0;                                    // 00210h - 00211h
    NvU16 sym_err_count_lane1;                                    // 00212h - 00213h
    NvU16 sym_err_count_lane2;                                    // 00214h - 00215h
    NvU16 sym_err_count_lane3;                                    // 00217h - 00216h
    NvU8 test_req;                                                // 00218h
    NvU8 test_link_rate;                                          // 00219h
    NvU8 reserved_1[0x21F - 0x21A + 1];                           // 0021Fh - 0021Ah
    NvU8 test_lane_count;                                         // 00220h
    NvU8 test_pattern;                                            // 00221h
    NvU16 test_h_total;                                           // 00222h - 00223h
    NvU16 test_v_total;                                           // 00224h - 00225h
    NvU16 test_h_start;                                           // 00226h - 00227h
    NvU16 test_v_start;                                           // 00228h - 00229h
    NvU16 test_hsync;                                             // 0022Ah - 0022Bh
    NvU16 test_vsync;                                             // 0022Ch - 0022Dh
    NvU16 test_h_width;                                           // 0022Eh - 0022Fh
    NvU16 test_v_height;                                          // 00230h - 00231h
    NvU16 test_misc;                                              // 00232h - 00233h
    NvU8 test_refresh_rate_numerator;                             // 00234h
    NvU8 reserved_2[0x23F - 0x235 + 1];                           // 00235h - 0023Fh
    NvU16 test_crc_R_Cr;                                          // 00240h - 00241h
    NvU16 test_crc_G_Y;                                           // 00242h - 00243h
    NvU16 test_crc_B_Cb;                                          // 00244h - 00245h
    NvU8 test_sink_misc;                                          // 00246h
    NvU8 reserved_3[0x25F - 0x247 + 1];                           // 00247h - 0025fh
    NvU8 test_response;                                           // 00260h
    NvU8 test_edid_checksum;                                      // 00261h
    NvU8 reserved_4[0x26F - 0x262 + 1];                           // 00262h - 0026Fh
    NvU8 test_sink;                                               // 00270h
    //NvU8 reserved_5[0x27F - 0x271 + 1];                           // 00271h - 0027Fh
    //NvU8 reserved_6[0x2FF - 0x280 + 1];                           // 00280h - 002FFh
}NVT_DPCD_LINK_SINK_STATUS;

#define NV_DPCD_DONGLE_NVIDIA_OUI                  0x00044B

//
// Raw DPCD data format - Vendor-Specific Field for Source Device // 00300h - 003FFh
// Raw DPCD data format - Vendor-Specific Field for Sink Device   // 00400h - 004FFh
// Raw DPCD data format - Vendor-Specific Field for Branch Device // 00500h - 005FFh
typedef struct tagNVT_DPCD_VENDOR_SPECIFIC_FIELD
{
    NvU8 ieee_oui7_0;                                             // 00300h
    NvU8 ieee_oui15_8;                                            // 00301h
    NvU8 ieee_oui23_16;                                           // 00302h
    //NvU8 reserved[0x3FF - 0x303 + 1];                             // 003FFh - 00303h
}NVT_DPCD_VENDOR_SPECIFIC_FIELD;
//
// Raw DPCD data format - Dongle Specific Field
typedef struct tagNVT_DPCD_DONGLE_SPECIFIC_FIELD
{
    NvU8 vendor_b0;                                               // 00300h
    NvU8 vendor_b1;                                               // 00301h
    NvU8 vendor_b2;                                               // 00302h
    NvU8 model[6];                                                // 00303h - 00308h
    NvU8 chipIDVersion;                                           // 00309h
    //NvU8 reserved[0x3FF - 0x30A + 1];                           // 0030Ah - 005FFh
}NVT_DPCD_DONGLE_SPECIFIC_FIELD;
//
// Raw DPCD data format - DualDP Specific Field
typedef struct tagNVT_DPCD_DUALDP_SPECIFIC_FIELD
{
    NvU8 vendor_b0;                                               // 00300h
    NvU8 vendor_b1;                                               // 00301h
    NvU8 vendor_b2;                                               // 00302h
    NvU8 model[6];                                                // 00303h - 00308h
    NvU8 chipd_id_version;                                        // 00309h
    NvU8 reserved_1[0x3AF - 0x30A + 1];                           // 0030Ah - 003AFh
    NvU8 dual_dp_cap;                                             // 003B0h
    NvU8 dual_dp_base_addr[3];                                    // 003B1h - 003B3h
    //NvU8 reserved_2[0x3FF - 0x3B4 + 1];                           // 003B4h - 003FFh
}NVT_DPCD_DUALDP_SPECIFIC_FIELD;

//
// Raw DPCD data format - Sink Control Field                       // 00600h - 006FFh
typedef struct tagNVT_DPCD_SINK_CTRL_FIELD
{
    NvU8 set_power;                                               // 00600h
    //NvU8 reserved[0x6FF - 0x601 + 1];                             // 00601h - 006FFh
}NVT_DPCD_SINK_CTRL_FIELD;
//
// The entire DPCD data block
typedef struct tagNVT_DPCD
{
    NVT_DPCD_RECEIVER_CAP                                        receiver_cap;
    NVT_DPCD_LINK_CFG                                            link_cfg;
    NVT_DPCD_LINK_SINK_STATUS                                    link_status;
    NVT_DPCD_VENDOR_SPECIFIC_FIELD                               vsp_source_device;
    NVT_DPCD_VENDOR_SPECIFIC_FIELD                               vsp_sink_device;
    NVT_DPCD_VENDOR_SPECIFIC_FIELD                               vsp_branch_device;
    NVT_DPCD_SINK_CTRL_FIELD                                     sink_ctrl;
}NVT_DPCD;
//
//
// Parsed DPCD info
//
//
#define NVT_DPCD_REV_10                                 NVT_DPCD_DPCD_REV_10       // DPCD revision 1.0
#define NVT_DPCD_REV_11                                 NVT_DPCD_DPCD_REV_11       // DPCD revision 1.1
#define NVT_DPCD_REV_12                                 NVT_DPCD_DPCD_REV_12       // DPCD revision 1.2
#define NVT_DPCD_RECEIVER_MAX_DOWNSTREAM_PORT           16         // the max downstream port possible per device
#define NVT_DPCD_RECEIVER_DOWNSTREAM_PORT_TYPE_DP       NVT_DPCD_DWN_STRM_PORT0_CAP_DWN_STRM_PORT0_TYPE_DISPLAYPORT // Display Port
#define NVT_DPCD_RECEIVER_DOWNSTREAM_PORT_TYPE_VGA      NVT_DPCD_DWN_STRM_PORT0_CAP_DWN_STRM_PORT0_TYPE_VGA         // analog VGA or analog video over DVI-I
#define NVT_DPCD_RECEIVER_DOWNSTREAM_PORT_TYPE_DVI      NVT_DPCD_DWN_STRM_PORT0_CAP_DWN_STRM_PORT0_TYPE_DVI         // DVI
#define NVT_DPCD_RECEIVER_DOWNSTREAM_PORT_TYPE_HDMI     NVT_DPCD_DWN_STRM_PORT0_CAP_DWN_STRM_PORT0_TYPE_HDMI        // HDMI
#define NVT_DPCD_RECEIVER_DOWNSTREAM_PORT_TYPE_OTHERS   NVT_DPCD_DWN_STRM_PORT0_CAP_DWN_STRM_PORT0_TYPE_NO_EDID     // the downstream port type will have no EDID in sink device such as Composite/SVideo.
#define NVT_DPCD_RECEIVER_DOWNSTREAM_PORT_TYPE_DP_PP    NVT_DPCD_DWN_STRM_PORT0_CAP_DWN_STRM_PORT0_TYPE_DISPLAYPORT_PP  // Display Port++
#define NVT_DPCD_LINK_RATE_1_62_GBPS                    NVT_DPCD_LINK_BW_SET_LINK_BW_SET_1_62GPBS_PER_LANE          // 1.62Gbps per lane
#define NVT_DPCD_LINK_RATE_2_70_GBPS                    NVT_DPCD_LINK_BW_SET_LINK_BW_SET_2_70GPBS_PER_LANE          // 2.70Gbps per lane
#define NVT_DPCD_LINK_RATE_5_40_GBPS                    NVT_DPCD_LINK_BW_SET_LINK_BW_SET_5_40GPBS_PER_LANE          // 5.40Gbps per lane
#define NVT_DPCD_LINK_RATE_8_10_GBPS                    NVT_DPCD_LINK_BW_SET_LINK_BW_SET_8_10GPBS_PER_LANE          // 5.40Gbps per lane
#define NVT_DPCD_LINK_RATE_FACTOR_IN_10KHZ_MBPS         2700       // e.g. NVT_DPCD_LINK_RATE_1_62_GBPS * 0.27Gbps per lane (in 10KHz)
#define NVT_DPCD_LANE_COUNT_1                           NVT_DPCD_LANE_COUNT_SET_LANE_COUNT_SET_1_LANE
#define NVT_DPCD_LANE_COUNT_2                           NVT_DPCD_LANE_COUNT_SET_LANE_COUNT_SET_2_LANES
#define NVT_DPCD_LANE_COUNT_4                           NVT_DPCD_LANE_COUNT_SET_LANE_COUNT_SET_4_LANES
#define NVT_DPCD_LANE_COUNT_8                           8

// note: the values of NVT_COLOR_FORMAT_* are fixed in order to match the equivalent NV classes
typedef enum _NVT_COLOR_FORMAT
{
    NVT_COLOR_FORMAT_RGB      = 0,
    NVT_COLOR_FORMAT_YCbCr422 = 1,
    NVT_COLOR_FORMAT_YCbCr444 = 2,
    NVT_COLOR_FORMAT_YCbCr420 = 3,
    NVT_COLOR_FORMAT_Y        = 4,
    NVT_COLOR_FORMAT_RAW      = 5,
    NVT_COLOR_FORMAT_INVALID  = 0xFF
} NVT_COLOR_FORMAT;

typedef enum
{
    NVT_COLOR_RANGE_FULL    = 0,
    NVT_COLOR_RANGE_LIMITED = 1
} NVT_COLOR_RANGE;

// note: the values of NVT_COLORIMETRY_* are fixed in order to match the equivalent NV classes
typedef enum
{
    NVT_COLORIMETRY_RGB        = 0,
    NVT_COLORIMETRY_YUV_601    = 1,
    NVT_COLORIMETRY_YUV_709    = 2,
    NVT_COLORIMETRY_EXTENDED   = 3,
    NVT_COLORIMETRY_XVYCC_601  = 4,
    NVT_COLORIMETRY_XVYCC_709  = 5,
    NVT_COLORIMETRY_ADOBERGB   = 6,
    NVT_COLORIMETRY_BT2020cYCC = 7,
    NVT_COLORIMETRY_BT2020YCC  = 8,
    NVT_COLORIMETRY_BT2020RGB  = 9,
    NVT_COLORIMETRY_INVALID    = 0xFF
} NVT_COLORIMETRY;

#define NVT_DPCD_BPC_DEFAULT                                       0x00
#define NVT_DPCD_BPC_6                                             0x01
#define NVT_DPCD_BPC_8                                             0x02
#define NVT_DPCD_BPC_10                                            0x03
#define NVT_DPCD_BPC_12                                            0x04
#define NVT_DPCD_BPC_16                                            0x05

#define NVT_DPCD_AUTOMATED_TEST                                    0x02
#define NVT_DPCD_CP_IRQ                                            0x04

#define NVT_DPCD_LANES_2_3_TRAINED                                 0x77
#define NVT_DPCD_LANE_1_TRAINED                                    0x07
#define NVT_DPCD_LANE_0_TRAINED                                    0x07
#define NVT_DPCD_INTERLANE_ALIGN_DONE                              0x1

#define NVT_DPCD_LANE_1_STATUS                                     7:4
#define NVT_DPCD_LANE_0_STATUS                                     3:0
#define NVT_DPCD_ADDRESS_LANE_STATUS                               0x00202

#define NVT_DPCD_TEST_REQ_LINK_TRAINING                            0x01
#define NVT_DPCD_TEST_REQ_TEST_PATTERN                             0x02
#define NVT_DPCD_TEST_REQ_EDID_READ                                0x04
#define NVT_DPCD_TEST_REQ_PHY_TEST_PATTERN                         0x08

#define NVT_DPCD_TEST_ACK                                          0x01
#define NVT_DPCD_TEST_NAK                                          0x02
#define NVT_DPCD_TEST_EDID_CHECKSUM_WRITE                          0x04

#define NVT_DPCD_TEST_MISC_COLOR_FORMAT                            2:1
#define NVT_DPCD_TEST_MISC_DYNAMIC_RANGE                           3:3
#define NVT_DPCD_TEST_MISC_YCbCr_COEFFICIENT                       4:4
#define NVT_DPCD_TEST_MISC_BIT_DEPTH                               7:5

#define NVT_DPCD_TEST_EDID_CHECKSUM_ADDRESS                        0x261
#define NVT_DPCD_TEST_RESPONSE_ADDRESS                             0x260
#define NVT_EDID_CHECKSUM_BYTE                                     127

#define NVT_DPCD_POWER_STATE_NORMAL                                0x01
#define NVT_DPCD_POWER_STATE_POWER_DOWN                            0x02

// *******************
// ** DPCD 1.1 Spec **
// *******************

// 0x000h DPCD_REV
#define NVT_DPCD_DPCD_REV                                                0x000
#define NVT_DPCD_DPCD_REV_MINOR_VER                                        3:0
#define NVT_DPCD_DPCD_REV_MAJOR_VER                                        7:4
#define NVT_DPCD_DPCD_REV_10                                              0x10
#define NVT_DPCD_DPCD_REV_11                                              0x11
#define NVT_DPCD_DPCD_REV_12                                              0x12

// 0x001h MAX_LINK_RATE
#define NVT_DPCD_MAX_LINK_RATE                                           0x001
#define NVT_DPCD_MAX_LINK_RATE_MAX_LINK_RATE                               7:0
#define NVT_DPCD_MAX_LINK_RATE_MAX_LINK_RATE_1_62GPS_PER_LANE             0x06
#define NVT_DPCD_MAX_LINK_RATE_MAX_LINK_RATE_2_70GPS_PER_LANE             0x0A
#define NVT_DPCD_MAX_LINK_RATE_MAX_LINK_RATE_5_40GPS_PER_LANE             0x14
#define NVT_DPCD_MAX_LINK_RATE_MAX_LINK_RATE_8_10GPS_PER_LANE             0x1E

// 0x002h - MAX_LANE_COUNT
#define NVT_DPCD_MAX_LANE_COUNT                                          0x002
#define NVT_DPCD_MAX_LANE_COUNT_MAX_LANE_COUNT                             4:0
#define NVT_DPCD_MAX_LANE_COUNT_RSVD                                       6:5
#define NVT_DPCD_MAX_LANE_COUNT_ENHANCED_FRAME_CAP                         7:7

// 0x003h - MAX_DOWNSPREAD
#define NVT_DPCD_MAX_DOWNSPREAD                                          0x003
#define NVT_DPCD_MAX_DOWNSPREAD_MAX_DOWNSPREAD                             0:0
#define NVT_DPCD_MAX_DOWNSPREAD_MAX_DOWNSPREAD_NO                            0
#define NVT_DPCD_MAX_DOWNSPREAD_MAX_DOWNSPREAD_YES                           1
#define NVT_DPCD_MAX_DOWNSPREAD_RSVD                                       5:1
#define NVT_DPCD_MAX_DOWNSPREAD_NO_AUX_HANDSHAKE_LINK_TRAINING             6:6
#define NVT_DPCD_MAX_DOWNSPREAD_RSVD_2                                     7:7

// 0x004h - NORP
#define NVT_DPCD_NORP                                                    0x004
#define NVT_DPCD_NORP_NUMBER_OF_RECEIVER_PORT_SUBTRACT_ONE                 0:0
#define NVT_DPCD_NORP_RSVD                                                 7:1

// 0x005 - DOWNSTREAMPORT_PRESENT
#define NVT_DPCD_DOWNSTREAMPORT_PRESENT                                  0x005
#define NVT_DPCD_DOWNSTREAMPORT_PRESENT_DWN_STRM_PORT_PRESENT              0:0
#define NVT_DPCD_DOWNSTREAMPORT_PRESENT_DWN_STRM_PORT_TYPE                 2:1
#define NVT_DPCD_DOWNSTREAMPORT_PRESENT_DWN_STRM_PORT_TYPE_DISPLAYPORT       0
#define NVT_DPCD_DOWNSTREAMPORT_PRESENT_DWN_STRM_PORT_TYPE_VGA               1
#define NVT_DPCD_DOWNSTREAMPORT_PRESENT_DWN_STRM_PORT_TYPE_DVI_HDMI          2
#define NVT_DPCD_DOWNSTREAMPORT_PRESENT_DWN_STRM_PORT_TYPE_OTHERS            3
#define NVT_DPCD_DOWNSTREAMPORT_PRESENT_FORMAT_CONVERSION                  3:3
#define NVT_DPCD_DOWNSTREAMPORT_PRESENT_RSVD                               7:4

// 0x006 - MAIN_LINK_CHANNEL_CODING
#define NVT_DPCD_MAIN_LINK_CHANNEL_CODING                                0x006
#define NVT_DPCD_MAIN_LINK_CHANNEL_CODING_ANSI8B_10B                       0:0
#define NVT_DPCD_MAIN_LINK_CHANNEL_CODING_RSVD                             7:1

// 0x007 - DOWN_STREAM_PORT_COUNT
#define NVT_DPCD_DOWN_STREAM_PORT_COUNT                                  0x007
#define NVT_DPCD_DOWN_STREAM_PORT_COUNT_DWN_STRM_PORT_COUNT                3:0
#define NVT_DPCD_DOWN_STREAM_PORT_COUNT_RSVD                               6:4
#define NVT_DPCD_DOWN_STREAM_PORT_COUNT_OUI_SUPPORT                        7:7
#define NVT_DPCD_DOWN_STREAM_PORT_COUNT_OUI_SUPPORT_YES                      1
#define NVT_DPCD_DOWN_STREAM_PORT_COUNT_OUI_SUPPORT_NO                       0

// 0x008h - RECEIVE_PORT0_CAP_0
#define NVT_DPCD_RECEIVE_PORT0_CAP_0                                     0x008
#define NVT_DPCD_RECEIVE_PORT0_CAP_0_RSVD                                  0:0
#define NVT_DPCD_RECEIVE_PORT0_CAP_0_LOCAL_EDID_PRESENT                    1:1
#define NVT_DPCD_RECEIVE_PORT0_CAP_0_LOCAL_EDID_PRESENT_YES                  1
#define NVT_DPCD_RECEIVE_PORT0_CAP_0_LOCAL_EDID_PRESENT_NO                   0
#define NVT_DPCD_RECEIVE_PORT0_CAP_0_ASSOCIATED_TO_PRECEDING_PORT          2:2
#define NVT_DPCD_RECEIVE_PORT0_CAP_0_ASSOCIATED_TO_PRECEDING_PORT_YES        1
#define NVT_DPCD_RECEIVE_PORT0_CAP_0_ASSOCIATED_TO_PRECEDING_PORT_NO         0
#define NVT_DPCD_RECEIVE_PORT0_CAP_0_RSVD_2                                7:3

// 0x009h - RECEIVE_PORT0_CAP_1
#define NVT_DPCD_RECEIVE_PORT0_CAP_1                                     0x009
#define NVT_DPCD_RECEIVE_PORT0_CAP_1_BUFFER_SIZE                           7:0

// 0x00Ah - RECEIVE_PORT1_CAP_0
#define NVT_DPCD_RECEIVE_PORT1_CAP_0                                     0x00A
#define NVT_DPCD_RECEIVE_PORT1_CAP_0_RSVD                                  0:0
#define NVT_DPCD_RECEIVE_PORT1_CAP_0_LOCAL_EDID_PRESENT                    1:1
#define NVT_DPCD_RECEIVE_PORT1_CAP_0_LOCAL_EDID_PRESENT_YES                  1
#define NVT_DPCD_RECEIVE_PORT1_CAP_0_LOCAL_EDID_PRESENT_NO                   0
#define NVT_DPCD_RECEIVE_PORT1_CAP_0_ASSOCIATED_TO_PRECEDING_PORT          2:2
#define NVT_DPCD_RECEIVE_PORT1_CAP_0_ASSOCIATED_TO_PRECEDING_PORT_YES        1
#define NVT_DPCD_RECEIVE_PORT1_CAP_0_ASSOCIATED_TO_PRECEDING_PORT_NO         0
#define NVT_DPCD_RECEIVE_PORT1_CAP_0_RSVD_2                                7:3

// 0x00Bh - RECEIVE_PORT1_CAP_1
#define NVT_DPCD_RECEIVE_PORT1_CAP_1                                     0x00B
#define NVT_DPCD_RECEIVE_PORT1_CAP_1_BUFFER_SIZE                           7:0

// 0x021h - MST_CAP
#define NVT_DPCD_MSTM_CAP                                                0x021
#define NVT_DPCD_MSTM_CAP_MST_CAP                                          0:0
#define NVT_DPCD_MSTM_CAP_MST_CAP_NO                                         0
#define NVT_DPCD_MSTM_CAP_MST_CAP_YES                                        1

// 0x080h ~ 0x08Fh - DWN_STRM_PORT0_CAP
#define NVT_DPCD_DWN_STRM_PORT0_CAP                                      0x080
#define NVT_DPCD_DWN_STRM_PORT0_CAP_DWN_STRM_PORT0_TYPE                    2:0
#define NVT_DPCD_DWN_STRM_PORT0_CAP_DWN_STRM_PORT0_TYPE_DISPLAYPORT          0
#define NVT_DPCD_DWN_STRM_PORT0_CAP_DWN_STRM_PORT0_TYPE_VGA                  1
#define NVT_DPCD_DWN_STRM_PORT0_CAP_DWN_STRM_PORT0_TYPE_DVI                  2
#define NVT_DPCD_DWN_STRM_PORT0_CAP_DWN_STRM_PORT0_TYPE_HDMI                 3
#define NVT_DPCD_DWN_STRM_PORT0_CAP_DWN_STRM_PORT0_TYPE_NO_EDID              4
#define NVT_DPCD_DWN_STRM_PORT0_CAP_DWN_STRM_PORT0_TYPE_DISPLAYPORT_PP       5  //Defined in Post DP 1.2 draft
#define NVT_DPCD_DWN_STRM_PORT0_CAP_DWN_STRM_PORT0_HPD                     3:3
#define NVT_DPCD_DWN_STRM_PORT0_CAP_DWN_STRM_PORT0_HPD_AWARE_YES             1
#define NVT_DPCD_DWN_STRM_PORT0_CAP_DWN_STRM_PORT0_HPD_AWARE_NO              0
#define NVT_DPCD_DWN_STRM_PORT0_CAP_RSVD                                   7:4

// 0x100h - LINK_BW_SET
#define NVT_DPCD_LINK_BW_SET                                             0x100
#define NVT_DPCD_LINK_BW_SET_LINK_BW_SET                                   7:0
#define NVT_DPCD_LINK_BW_SET_LINK_BW_SET_1_62GPBS_PER_LANE                0x06
#define NVT_DPCD_LINK_BW_SET_LINK_BW_SET_2_70GPBS_PER_LANE                0x0A
#define NVT_DPCD_LINK_BW_SET_LINK_BW_SET_5_40GPBS_PER_LANE                0x14
#define NVT_DPCD_LINK_BW_SET_LINK_BW_SET_8_10GPBS_PER_LANE                0x1E

// 0x101h - LANE_COUNT_SET
#define NVT_DPCD_LANE_COUNT_SET                                          0x101
#define NVT_DPCD_LANE_COUNT_SET_LANE_COUNT_SET                             4:0
#define NVT_DPCD_LANE_COUNT_SET_LANE_COUNT_SET_1_LANE                        1
#define NVT_DPCD_LANE_COUNT_SET_LANE_COUNT_SET_2_LANES                       2
#define NVT_DPCD_LANE_COUNT_SET_LANE_COUNT_SET_4_LANES                       4
#define NVT_DPCD_LANE_COUNT_SET_RSVD                                       6:5
#define NVT_DPCD_LANE_COUNT_SET_ENHANCED_FRAME_EN                          7:7
#define NVT_DPCD_LANE_COUNT_SET_ENHANCED_FRAME_EN_YES                        1
#define NVT_DPCD_LANE_COUNT_SET_ENHANCED_FRAME_EN_NO                         0

// 0x102h - TRAINING_PATTERN_SET
#define NVT_DPCD_TRAINING_PATTERN_SET                                    0x102
#define NVT_DPCD_TRAINING_PATTERN_SET_TRAINING_PATTERN_SET                 1:0
#define NVT_DPCD_TRAINING_PATTERN_SET_TRAINING_PATTERN_SET_NOT_IN_PROGRESS   0
#define NVT_DPCD_TRAINING_PATTERN_SET_TRAINING_PATTERN_SET_PATTERN_1         1
#define NVT_DPCD_TRAINING_PATTERN_SET_TRAINING_PATTERN_SET_PATTERN_2         2
#define NVT_DPCD_TRAINING_PATTERN_SET_TRAINING_PATTERN_SET_RSVD              3
#define NVT_DPCD_TRAINING_PATTERN_SET_LINK_QUAL_PATTERN_SET                3:2
#define NVT_DPCD_TRAINING_PATTERN_SET_LINK_QUAL_PATTERN_SET_NOT_TRANSMITTED  0
#define NVT_DPCD_TRAINING_PATTERN_SET_LINK_QUAL_PATTERN_SET_D10_2            1
#define NVT_DPCD_TRAINING_PATTERN_SET_LINK_QUAL_PATTERN_SET_SERMPT           2
#define NVT_DPCD_TRAINING_PATTERN_SET_LINK_QUAL_PATTERN_SET_PRBS7            3
#define NVT_DPCD_TRAINING_PATTERN_SET_RECOVERED_CLOCK_OUT_EN               4:4
#define NVT_DPCD_TRAINING_PATTERN_SET_RECOVERED_CLOCK_OUT_EN_NO              0
#define NVT_DPCD_TRAINING_PATTERN_SET_RECOVERED_CLOCK_OUT_EN_YES             1
#define NVT_DPCD_TRAINING_PATTERN_SET_SCRAMBLING_DISABLE                   5:5
#define NVT_DPCD_TRAINING_PATTERN_SET_SCRAMBLING_DISABLE_NO                  0
#define NVT_DPCD_TRAINING_PATTERN_SET_SCRAMBLING_DISABLE_YES                 1
#define NVT_DPCD_TRAINING_PATTERN_SET_SYMBOL_ERROR_COUNT_SEL               7:6
#define NVT_DPCD_TRAINING_PATTERN_SET_SYMBOL_ERROR_COUNT_SEL_DIS_ERROR       0
#define NVT_DPCD_TRAINING_PATTERN_SET_SYMBOL_ERROR_COUNT_SEL_D_ERROR         1
#define NVT_DPCD_TRAINING_PATTERN_SET_SYMBOL_ERROR_COUNT_SEL_IS_ERROR        2
#define NVT_DPCD_TRAINING_PATTERN_SET_SYMBOL_ERROR_COUNT_SEL_RSVD            3

// 0x103h ~ 0x106h - TRAINING_LANE?_SET
#define NVT_DPCD_TRAINING_LANE0_SET                                      0x103
#define NVT_DPCD_TRAINING_LANE1_SET                                      0x104
#define NVT_DPCD_TRAINING_LANE2_SET                                      0x105
#define NVT_DPCD_TRAINING_LANE3_SET                                      0x106
#define NVT_DPCD_TRAINING_LANE0_SET_VOLTAGE_SWING_SET                      1:0
#define NVT_DPCD_TRAINING_LANE0_SET_VOLTAGE_SWING_SET_TP1_VS_L0              0
#define NVT_DPCD_TRAINING_LANE0_SET_VOLTAGE_SWING_SET_TP1_VS_L1              1
#define NVT_DPCD_TRAINING_LANE0_SET_VOLTAGE_SWING_SET_TP1_VS_L2              2
#define NVT_DPCD_TRAINING_LANE0_SET_VOLTAGE_SWING_SET_TP1_VS_L3              3
#define NVT_DPCD_TRAINING_LANE0_SET_MAX_SWING_REACHED                      2:2
#define NVT_DPCD_TRAINING_LANE0_SET_MAX_SWING_REACHED_NO                     0
#define NVT_DPCD_TRAINING_LANE0_SET_MAX_SWING_REACHED_YES                    1
#define NVT_DPCD_TRAINING_LANE0_SET_PRE_EMPHASIS_SET                       4:3
#define NVT_DPCD_TRAINING_LANE0_SET_PRE_EMPHASIS_SET_TP2_PE_NONE             0
#define NVT_DPCD_TRAINING_LANE0_SET_PRE_EMPHASIS_SET_TP2_PE_L1               1
#define NVT_DPCD_TRAINING_LANE0_SET_PRE_EMPHASIS_SET_TP2_PE_L2               2
#define NVT_DPCD_TRAINING_LANE0_SET_PRE_EMPHASIS_SET_TP2_PE_L3               3
#define NVT_DPCD_TRAINING_LANE0_SET_MAX_PRE_EMPHASIS_REACHED               5:5
#define NVT_DPCD_TRAINING_LANE0_SET_MAX_PRE_EMPHASIS_REACHED_NO              0
#define NVT_DPCD_TRAINING_LANE0_SET_MAX_PRE_EMPHASIS_REACHED_YES             1
#define NVT_DPCD_TRAINING_LANE0_SET_RSVD                                   7:6

// 0x107h - DOWNSPREAD_CTRL
#define NVT_DPCD_DOWNSPREAD_CTRL                                         0x107
#define NVT_DPCD_DOWNSPREAD_CTRL_RSVD                                      3:0
#define NVT_DPCD_DOWNSPREAD_CTRL_SPREAD_AMP                                4:4
#define NVT_DPCD_DOWNSPREAD_CTRL_SPREAD_AMP_NO                               0
#define NVT_DPCD_DOWNSPREAD_CTRL_SPREAD_AMP_YES                              1
#define NVT_DPCD_DOWNSPREAD_CTRL_RSVD_2                                    7:5

// 0x108h - MAIN_LINK_CHANNEL_CODING_SET
#define NVT_DPCD_MAIN_LINK_CHANNEL_CODING_SET                            0x108
#define NVT_DPCD_MAIN_LINK_CHANNEL_CODING_SET_SET_ANSI8B10B                0:0
#define NVT_DPCD_MAIN_LINK_CHANNEL_CODING_SET_RSVD                         7:1

// 0x111h - MSTM_CTRL
#define NVT_DPCD_MSTM_CTRL                                               0x111
#define NVT_DPCD_MSTM_CTRL_MST_EN                                          0:0
#define NVT_DPCD_MSTM_CTRL_MST_EN_NO                                         0
#define NVT_DPCD_MSTM_CTRL_MST_EN_YES                                        1
#define NVT_DPCD_MSTM_CTRL_UP_REQ_EN                                       1:1
#define NVT_DPCD_MSTM_CTRL_UP_REQ_EN_NO                                      0
#define NVT_DPCD_MSTM_CTRL_UP_REQ_EN_YES                                     1
#define NVT_DPCD_MSTM_CTRL_UPSTREAM_IS_SRC                                 2:2
#define NVT_DPCD_MSTM_CTRL_UPSTREAM_IS_SRC_NO                                0
#define NVT_DPCD_MSTM_CTRL_UPSTREAM_IS_SRC_YES                               1
#define NVT_DPCD_MSTM_CTRL_MST_RSVD                                        7:3

// 0x1A1h - BRANCH_DEVICE_CTRL
#define NVT_DPCD_BRANCH_DEVICE_CTRL                                      0x1A1
#define NVT_DPCD_BRANCH_DEVICE_CTRL_HPD_NOTIF_TYPE                         0:0
#define NVT_DPCD_BRANCH_DEVICE_CTRL_HPD_NOTIF_TYPE_LONG_HPD_PULSE            0
#define NVT_DPCD_BRANCH_DEVICE_CTRL_HPD_NOTIF_TYPE_SHORT_IRQ_PULSE           1
#define NVT_DPCD_BRANCH_DEVICE_CTRL_RSVD                                   7:1

#define NVT_DPCD_PAYLOAD_ALLOCATE_SET                                    0x1C0
#define NVT_DPCD_PAYLOAD_ALLOCATE_SET_VC_ID                                6:0

#define NVT_DPCD_PAYLOAD_ALLOCATE_START_TIME_SLOT                        0x1C1
#define NVT_DPCD_PAYLOAD_ALLOCATE_START_TIME_SLOT_FIELD                    5:0

#define NVT_DPCD_PAYLOAD_ALLOCATE_TIME_SLOT_COUNT                        0x1C2
#define NVT_DPCD_PAYLOAD_ALLOCATE_TIME_SLOT_COUNT_FIELD                    5:0

// 0x200h - SINK_COUNT
#define NVT_DPCD_SINK_COUNT                                              0x200
#define NVT_DPCD_SINK_COUNT_SINK_COUNT                                     5:0
#define NVT_DPCD_SINK_COUNT_CP_READY                                       6:6
#define NVT_DPCD_SINK_COUNT_RSVD                                           7:7

// 0x201h - DEVICE_SERVICE_IRQ_VECTOR
#define NVT_DPCD_DEVICE_SERVICE_IRQ_VECTOR                               0x201
#define NVT_DPCD_DEVICE_SERVICE_IRQ_VECTOR_RSVD_REMOTE_CTRL_CMD_PENDING    0:0
#define NVT_DPCD_DEVICE_SERVICE_IRQ_VECTOR_AUTOMATED_TEST_REQUEST          1:1
#define NVT_DPCD_DEVICE_SERVICE_IRQ_VECTOR_CP_IRQ                          2:2
#define NVT_DPCD_DEVICE_SERVICE_IRQ_VECTOR_MCCS_IRQ                        3:3
#define NVT_DPCD_DEVICE_SERVICE_IRQ_VECTOR_DOWN_REP_MSG_READY              4:4
#define NVT_DPCD_DEVICE_SERVICE_IRQ_VECTOR_UP_REQ_MSG_READY                5:5
#define NVT_DPCD_DEVICE_SERVICE_IRQ_VECTOR_SINK_SPECIFIC_IRQ               6:6
#define NVT_DPCD_DEVICE_SERVICE_IRQ_VECTOR_RSVD_2                          7:7

// 0x202h ~ 0x203h - LANE0_1_STATUS
#define NVT_DPCD_LANE0_1_STATUS                                          0x202
#define NVT_DPCD_LANE2_3_STATUS                                          0x203
#define NVT_DPCD_LANE0_1_STATUS_LANE0_CR_DONE                              0:0
#define NVT_DPCD_LANE0_1_STATUS_LANE0_CHANNEL_EQ_DONE                      1:1
#define NVT_DPCD_LANE0_1_STATUS_LANE0_SYMBOL_LOCKED                        2:2
#define NVT_DPCD_LANE0_1_STATUS_RSVD                                       3:3
#define NVT_DPCD_LANE0_1_STATUS_LANE1_CR_DONE                              4:4
#define NVT_DPCD_LANE0_1_STATUS_LANE1_CHANNEL_EQ_DONE                      5:5
#define NVT_DPCD_LANE0_1_STATUS_LANE1_SYMBOL_LOCKED                        6:6
#define NVT_DPCD_LANE0_1_STATUS_RSVD_2                                     7:7

// 0x204h - LANE_ALIGN_STATUS_UPDATED
// Temporary until Linux/Apple change their code.
#define NVT_DPCD_LANE_ALIGN_STAUTS_UPDATED                               NVT_DPCD_LANE_ALIGN_STATUS_UPDATED
#define NVT_DPCD_LANE_ALIGN_STATUS_UPDATED                               0x204
#define NVT_DPCD_LANE_ALIGN_STATUS_UPDATED_INTERLANE_ALIGN_DONE            0:0
#define NVT_DPCD_LANE_ALIGN_STATUS_UPDATED_RSVD                            5:1
#define NVT_DPCD_LANE_ALIGN_STATUS_UPDATED_DOWNSTREAM_PORT_STATUS_CHANGED  6:6
#define NVT_DPCD_LANE_ALIGN_STATUS_UPDATED_LINK_STATUS_UPDATED             7:7

// 0x205 - SINK_STATUS
#define NVT_DPCD_SINK_STATUS                                             0x205
#define NVT_DPCD_SINK_STATUS_RECEIVE_PORT_0_STATUS                         0:0
#define NVT_DPCD_SINK_STATUS_RECEIVE_PORT_0_STATUS_OUT_OF_SYNC               0
#define NVT_DPCD_SINK_STATUS_RECEIVE_PORT_0_STATUS_IN_SYNC                   1
#define NVT_DPCD_SINK_STATUS_RECEIVE_PORT_1_STATUS                         1:1
#define NVT_DPCD_SINK_STATUS_RECEIVE_PORT_1_STATUS_OUT_OF_SYNC               0
#define NVT_DPCD_SINK_STATUS_RECEIVE_PORT_1_STATUS_IN_SYNC                   1
#define NVT_DPCD_SINK_STATUS_RSVD                                          7:2

// 0x206h ~ 0x207h - ADJUST_REQUEST_LANE0_1
#define NVT_DPCD_ADJUST_REQUEST_LANE0_1                                  0x206
#define NVT_DPCD_ADJUST_REQUEST_LANE2_3                                  0x207
#define NVT_DPCD_ADJUST_REQUEST_LANE0_1_VOLTAGE_SWING_LANE0                1:0
#define NVT_DPCD_ADJUST_REQUEST_LANE0_1_VOLTAGE_SWING_LANE0_LEVEL_0          0
#define NVT_DPCD_ADJUST_REQUEST_LANE0_1_VOLTAGE_SWING_LANE0_LEVEL_1          1
#define NVT_DPCD_ADJUST_REQUEST_LANE0_1_VOLTAGE_SWING_LANE0_LEVEL_2          2
#define NVT_DPCD_ADJUST_REQUEST_LANE0_1_VOLTAGE_SWING_LANE0_LEVEL_3          3
#define NVT_DPCD_ADJUST_REQUEST_LANE0_1_PRE_EMPHASIS_LANE0                 3:2
#define NVT_DPCD_ADJUST_REQUEST_LANE0_1_PRE_EMPHASIS_LANE0_LEVEL_0           0
#define NVT_DPCD_ADJUST_REQUEST_LANE0_1_PRE_EMPHASIS_LANE0_LEVEL_1           1
#define NVT_DPCD_ADJUST_REQUEST_LANE0_1_PRE_EMPHASIS_LANE0_LEVEL_2           2
#define NVT_DPCD_ADJUST_REQUEST_LANE0_1_PRE_EMPHASIS_LANE0_LEVEL_3           3
#define NVT_DPCD_ADJUST_REQUEST_LANE0_1_VOLTAGE_SWING_LANE1                5:4
#define NVT_DPCD_ADJUST_REQUEST_LANE0_1_VOLTAGE_SWING_LANE1_LEVEL_0          0
#define NVT_DPCD_ADJUST_REQUEST_LANE0_1_VOLTAGE_SWING_LANE1_LEVEL_1          1
#define NVT_DPCD_ADJUST_REQUEST_LANE0_1_VOLTAGE_SWING_LANE1_LEVEL_2          2
#define NVT_DPCD_ADJUST_REQUEST_LANE0_1_VOLTAGE_SWING_LANE1_LEVEL_3          3
#define NVT_DPCD_ADJUST_REQUEST_LANE0_1_PRE_EMPHASIS_LANE1                 7:6
#define NVT_DPCD_ADJUST_REQUEST_LANE0_1_PRE_EMPHASIS_LANE1_LEVEL_0           0
#define NVT_DPCD_ADJUST_REQUEST_LANE0_1_PRE_EMPHASIS_LANE1_LEVEL_1           1
#define NVT_DPCD_ADJUST_REQUEST_LANE0_1_PRE_EMPHASIS_LANE1_LEVEL_2           2
#define NVT_DPCD_ADJUST_REQUEST_LANE0_1_PRE_EMPHASIS_LANE1_LEVEL_3           3

// 0x208h ~ 0x20Bh TRAINING_SCORE_LANE0~3
#define NVT_DPCD_TRAINING_SCORE_LANE0                                    0x208
#define NVT_DPCD_TRAINING_SCORE_LANE1                                    0x209
#define NVT_DPCD_TRAINING_SCORE_LANE2                                    0x20A
#define NVT_DPCD_TRAINING_SCORE_LANE3                                    0x20B

// 0x210h ~ 0x217h SYMBOL_ERROR_COUNT_LANE0 (16bit)
#define NVT_DPCD_SYMBOL_ERROR_COUNT_LANE0_LO                             0x210
#define NVT_DPCD_SYMBOL_ERROR_COUNT_LANE0_HI                             0x211
#define NVT_DPCD_SYMBOL_ERROR_COUNT_LANE1_LO                             0x212
#define NVT_DPCD_SYMBOL_ERROR_COUNT_LANE1_HI                             0x213
#define NVT_DPCD_SYMBOL_ERROR_COUNT_LANE2_LO                             0x214
#define NVT_DPCD_SYMBOL_ERROR_COUNT_LANE2_HI                             0x215
#define NVT_DPCD_SYMBOL_ERROR_COUNT_LANE3_LO                             0x216
#define NVT_DPCD_SYMBOL_ERROR_COUNT_LANE3_HI                             0x217
#define NVT_DPCD_SYMBOL_ERROR_COUNT_LANE0_ERROR_COUNT_LO                   7:0
#define NVT_DPCD_SYMBOL_ERROR_COUNT_LANE0_ERROR_COUNT_HI                   6:0
#define NVT_DPCD_SYMBOL_ERROR_COUNT_LANE0_ERROR_COUNT_VALID                7:7

// 0x218h TEST_REQUEST
#define NVT_DPCD_TEST_REQUEST                                            0x218
#define NVT_DPCD_TEST_REQUEST_TEST_LINK_TRAINING                           0:0
#define NVT_DPCD_TEST_REQUEST_TEST_PATTERN                                 1:1
#define NVT_DPCD_TEST_REQUEST_TEST_EDID_READ                               2:2
#define NVT_DPCD_TEST_REQUEST_PHY_TEST_PATTERN                             3:3
#define NVT_DPCD_TEST_REQUEST_RSVD                                         7:4

// 0x219h TEST_LINK_RATE
#define NVT_DPCD_TEST_LINK_RATE                                          0x219

// 0x220h TEST_LANE_COUNT
#define NVT_DPCD_TEST_LANE_COUNT                                         0x220
#define NVT_DPCD_TEST_LANE_COUNT_TEST_LANE_COUNT                           4:0
#define NVT_DPCD_TEST_LANE_COUNT_TEST_LANE_COUNT_ONE_LANE                    1
#define NVT_DPCD_TEST_LANE_COUNT_TEST_LANE_COUNT_TWO_LANES                   2
#define NVT_DPCD_TEST_LANE_COUNT_TEST_LANE_COUNT_FOUR_LANES                  4
#define NVT_DPCD_TEST_LANE_COUNT_RSVD                                      7:5

// 0x221h TEST_PATTERN
#define NVT_DPCD_TEST_PATTERN                                            0x221
#define NVT_DPCD_TEST_PATTERN_NO_TEST_PATTERN_TRANSMITTED                    0
#define NVT_DPCD_TEST_PATTERN_COLOR_RAMPS                                    1
#define NVT_DPCD_TEST_PATTERN_BLACK_AND_WHITE_VERTICAL_LINES                 2
#define NVT_DPCD_TEST_PATTERN_COLOR_SQUARE                                   3

// 0x222h ~ 0x223h TEST_H_TOTAL
#define NVT_DPCD_TEST_H_TOTAL_HI                                         0x222
#define NVT_DPCD_TEST_H_TOTAL_LO                                         0x223

// 0x224h ~ 0x225h TEST_V_TOTAL
#define NVT_DPCD_TEST_V_TOTAL_HI                                         0x224
#define NVT_DPCD_TEST_V_TOTAL_LO                                         0x225

// 0x226h ~ 0x227h TEST_H_START
#define NVT_DPCD_TEST_H_START_HI                                         0x226
#define NVT_DPCD_TEST_H_START_LO                                         0x227

// 0x228h ~ 0x229h TEST_V_START
#define NVT_DPCD_TEST_V_START_HI                                         0x228
#define NVT_DPCD_TEST_V_START_LO                                         0x229

// 0x22Ah ~ 0x22Bh TEST_HSYNC
#define NVT_DPCD_TEST_HSYNC_HI                                           0x22A
#define NVT_DPCD_TEST_HSYNC_LO                                           0x22B
#define NVT_DPCD_TEST_HSYNC_HI_TEST_HSYNC_WIDTH_14_8                       6:0
#define NVT_DPCD_TEST_HSYNC_HI_TEST_HSYNC_POLARITY                         7:7

// 0x22Ch ~ 0x22Dh TEST_VSYNC
#define NVT_DPCD_TEST_VSYNC_HI                                           0x22C
#define NVT_DPCD_TEST_VSYNC_LO                                           0x22D
#define NVT_DPCD_TEST_VSYNC_HI_TEST_VSYNC_WIDTH_14_8                       6:0
#define NVT_DPCD_TEST_VSYNC_HI_TEST_VSYNC_POLARITY                         7:7

// 0x22Eh ~ 0x22Fh TEST_H_WIDTH
#define NVT_DPCD_TEST_H_WIDTH_HI                                         0x22E
#define NVT_DPCD_TEST_H_WIDTH_LO                                         0x22F

// 0x230h ~ 0x231h TEST_V_WIDTH
#define NVT_DPCD_TEST_V_HEIGHT_HI                                        0x230
#define NVT_DPCD_TEST_V_HEIGHT_LO                                        0x231

// 0x232h ~ 0x233h TEST_MISC
#define NVT_DPCD_TEST_MISC_LO                                            0x232
#define NVT_DPCD_TEST_MISC_LO_TEST_SYNCHRONOUS_CLOCK                       0:0
#define NVT_DPCD_TEST_MISC_LO_TEST_SYNCHRONOUS_CLOCK_ASYNC                   0
#define NVT_DPCD_TEST_MISC_LO_TEST_SYNCHRONOUS_CLOCK_SYNC                    1
#define NVT_DPCD_TEST_MISC_LO_TEST_COLOR_FORMAT                            2:1
#define NVT_DPCD_TEST_MISC_LO_TEST_COLOR_FORMAT_RGB                          0
#define NVT_DPCD_TEST_MISC_LO_TEST_COLOR_FORMAT_YCbCr422                     1
#define NVT_DPCD_TEST_MISC_LO_TEST_COLOR_FORMAT_YCbCr444                     2
#define NVT_DPCD_TEST_MISC_LO_TEST_COLOR_FORMAT_RSVD                         3
#define NVT_DPCD_TEST_MISC_LO_TEST_DYNAMIC_RANGE                           3:3
#define NVT_DPCD_TEST_MISC_LO_TEST_DYNAMIC_RANGE_VESA                        0
#define NVT_DPCD_TEST_MISC_LO_TEST_DYNAMIC_RANGE_CEA                         1
#define NVT_DPCD_TEST_MISC_LO_TEST_YCBCR_COEFFICIENTS                      4:4
#define NVT_DPCD_TEST_MISC_LO_TEST_YCBCR_COEFFICIENTS_ITU601                 0
#define NVT_DPCD_TEST_MISC_LO_TEST_YCBCR_COEFFICIENTS_ITU709                 1
#define NVT_DPCD_TEST_MISC_LO_TEST_BIT_DEPTH                               7:5
#define NVT_DPCD_TEST_MISC_LO_TEST_BIT_DEPTH_6BPC                            0
#define NVT_DPCD_TEST_MISC_LO_TEST_BIT_DEPTH_8BPC                            1
#define NVT_DPCD_TEST_MISC_LO_TEST_BIT_DEPTH_10BPC                           2
#define NVT_DPCD_TEST_MISC_LO_TEST_BIT_DEPTH_12BPC                           3
#define NVT_DPCD_TEST_MISC_LO_TEST_BIT_DEPTH_16BPC                           4
#define NVT_DPCD_TEST_MISC_HI                                            0x233
#define NVT_DPCD_TEST_MISC_HI_TEST_REFRESH_DENOMINATOR                     0:0
#define NVT_DPCD_TEST_MISC_HI_TEST_REFRESH_DENOMINATOR_1                     0
#define NVT_DPCD_TEST_MISC_HI_TEST_REFRESH_DENOMINATOR_1001                  1
#define NVT_DPCD_TEST_MISC_HI_TEST_INTERLACED                              1:1
#define NVT_DPCD_TEST_MISC_HI_TEST_INTERLACED_NO                             0
#define NVT_DPCD_TEST_MISC_HI_TEST_INTERLACED_YES                            1
#define NVT_DPCD_TEST_MISC_HI_TEST_INTERLACED_RSVD                         7:2

// 0x234h TEST_REFRESH_RATE_NUMERATOR
#define NVT_DPCD_TEST_REFRESH_RATE_NUMERATOR                             0x234

// 0x240h ~ 0x241h TEST_CRC_R_Cr
#define NVT_DPCD_TEST_CRC_R_Cr_LO                                        0x240
#define NVT_DPCD_TEST_CRC_R_Cr_HI                                        0x241

// 0x242h ~ 0x243h TEST_CRC_G_Y
#define NVT_DPCD_TEST_CRC_G_Y_LO                                         0x242
#define NVT_DPCD_TEST_CRC_G_Y_HI                                         0x243

// 0x244h ~ 0x245h TEST_CRC_B_Cb
#define NVT_DPCD_TEST_CRC_B_Cb_LO                                        0x244
#define NVT_DPCD_TEST_CRC_B_Cb_HI                                        0x245

// 0x246h TEST_SINC_MISC
#define NVT_DPCD_TEST_SINK_MISC                                          0x246
#define NVT_DPCD_TEST_SINK_MISC_TEST_CRC_COUNT                             3:0
#define NVT_DPCD_TEST_SINK_MISC_TEST_CRC_SUPPORTED                         5:5
#define NVT_DPCD_TEST_SINK_MISC_TEST_CRC_SUPPORTED_NO                        0
#define NVT_DPCD_TEST_SINK_MISC_TEST_CRC_SUPPORTED_YES                       1
#define NVT_DPCD_TEST_SINK_MISC_RSVD                                       7:6

// 0x248h PHY_TEST_PATTERN
#define NVT_DPCD_PHY_TEST_PATTERN                                        0x248
#define NVT_DPCD_PHY_TEST_PATTERN_PHY_TEST_PATTERN_SEL                     1:0
#define NVT_DPCD_PHY_TEST_PATTERN_PHY_TEST_PATTERN_SEL_NO_TEST_PATTERN       0
#define NVT_DPCD_PHY_TEST_PATTERN_PHY_TEST_PATTERN_SEL_D10_2                 1
#define NVT_DPCD_PHY_TEST_PATTERN_PHY_TEST_PATTERN_SEL_SEMC                  2
#define NVT_DPCD_PHY_TEST_PATTERN_PHY_TEST_PATTERN_SEL_PRBS7                 3
#define NVT_DPCD_PHY_TEST_PATTERN_RSVD                                     7:2

// 0x260h TEST_RESPONSE
#define NVT_DPCD_TEST_RESPONSE                                           0x260
#define NVT_DPCD_TEST_RESPONSE_TEST_ACK                                    0:0
#define NVT_DPCD_TEST_RESPONSE_TEST_ACK_KEEP_TEST_REQ                        0
#define NVT_DPCD_TEST_RESPONSE_TEST_ACK_CLEAR_TEST_REQ                       1
#define NVT_DPCD_TEST_RESPONSE_TEST_NAK                                    1:1
#define NVT_DPCD_TEST_RESPONSE_TEST_NACK_KEEP_TEST_REQ                       0
#define NVT_DPCD_TEST_RESPONSE_TEST_NACK_CLEAR_TEST_REQ                      1
#define NVT_DPCD_TEST_RESPONSE_TEST_EDID_CHECKSUM_WRITE                    2:2
#define NVT_DPCD_TEST_RESPONSE_TEST_EDID_CHECKSUM_WRITE_NO                   0
#define NVT_DPCD_TEST_RESPONSE_TEST_EDID_CHECKSUM_WRITE_YES                  1
#define NVT_DPCD_TEST_RESPONSE_RSVD                                        7:3

// 0x261h TEST_EDID_CHECKSUM
#define NVT_DPCD_TEST_EDID_CHECKSUM                                      0x261

// 0x270 TEST_SINK
#define NVT_DPCD_TEST_SINK                                               0x270
#define NVT_DPCD_TEST_SINK_TEST_SINK_START                                 0:0
#define NVT_DPCD_TEST_SINK_TEST_SINK_START_STOP_CALC_CRC                     0
#define NVT_DPCD_TEST_SINK_TEST_SINK_START_START_CALC_CRC                    1
#define NVT_DPCD_TEST_SINK_RSVD                                            7:1

#define NVT_DPCD_PAYLOAD_TABLE_UPDATE_STATUS                             0x2C0
#define NVT_DPCD_PAYLOAD_TABLE_UPDATE_STATUS_TABLE_UPDATED                 0:0
#define NVT_DPCD_PAYLOAD_TABLE_UPDATE_STATUS_TABLE_UPDATED_NO                0
#define NVT_DPCD_PAYLOAD_TABLE_UPDATE_STATUS_TABLE_UPDATED_YES               1
#define NVT_DPCD_PAYLOAD_TABLE_UPDATE_STATUS_ACT_HANDLED                   1:1
#define NVT_DPCD_PAYLOAD_TABLE_UPDATE_STATUS_ACT_HANDLED_NO                  0
#define NVT_DPCD_PAYLOAD_TABLE_UPDATE_STATUS_ACT_HANDLED_YES                 1

// 0x300h ~ 0x302h SOURCE_IEEE_OUT
#define NVT_DPCD_SOURCE_IEEE_OUT_7_0                                     0x300
#define NVT_DPCD_SOURCE_IEEE_OUT_15_8                                    0x301
#define NVT_DPCD_SOURCE_IEEE_OUT_23_16                                   0x302

// 0x400h ~ 0x402h SINK_IEEE_OUT
#define NVT_DPCD_SINK_IEEE_OUT_7_0                                       0x400
#define NVT_DPCD_SINK_IEEE_OUT_15_8                                      0x401
#define NVT_DPCD_SINK_IEEE_OUT_23_16                                     0x402

// 0x500h ~ 0x502h BRANCH_IEEE_OUT
#define NVT_DPCD_BRANCH_IEEE_OUT_7_0                                     0x500
#define NVT_DPCD_BRANCH_IEEE_OUT_15_8                                    0x501
#define NVT_DPCD_BRANCH_IEEE_OUT_23_16                                   0x502

// 0x600 SET_POWER
#define NVT_DPCD_SET_POWER                                               0x600
#define NVT_DPCD_SET_POWER_SET_POWER_STATE                                 1:0
#define NVT_DPCD_SET_POWER_SET_POWER_STATE_RSVD                              0
#define NVT_DPCD_SET_POWER_SET_POWER_STATE_D0                                1
#define NVT_DPCD_SET_POWER_SET_POWER_STATE_D3                                2
#define NVT_DPCD_SET_POWER_SET_POWER_STATE_RSVD_2                            3
#define NVT_DPCD_SET_POWER_RSVD                                            7:2

//*************************************
// DP 1.2 Main Stream Attribute Fiedls
//*************************************

#define NVT_DP_INFOFRAME_MSA_MISC0_SYNC_CLOCK_MASK                      0x01    // MISC0 bit 0 Synchronous Clock
#define NVT_DP_INFOFRAME_MSA_MISC0_SYNC_CLOCK_SHIFT                     0x0
#define NVT_DP_INFOFRAME_MSA_MISC0_SYNC_CLOCK_ASYNC                     0x0
#define NVT_DP_INFOFRAME_MSA_MISC0_SYNC_CLOCK_INSYNC                    0x1

#define NVT_DP_INFOFRAME_MSA_MISC0_BITS_PER_COLOR_MASK                  0xe0    // MISC0 bits 7:5 number of bits per color
#define NVT_DP_INFOFRAME_MSA_MISC0_BITS_PER_COLOR_SHIFT                 0x5
#define NVT_DP_INFOFRAME_MSA_MISC0_BITS_PER_COLOR_6                     0x0
#define NVT_DP_INFOFRAME_MSA_MISC0_BITS_PER_COLOR_8                     0x1
#define NVT_DP_INFOFRAME_MSA_MISC0_BITS_PER_COLOR_10                    0x2
#define NVT_DP_INFOFRAME_MSA_MISC0_BITS_PER_COLOR_12                    0x3
#define NVT_DP_INFOFRAME_MSA_MISC0_BITS_PER_COLOR_16                    0x4

#define NVT_DP_INFOFRAME_MSA_MISC0_COLOR_FORMAT_MASK                    0x1e    // MISC0 bits 4:1 Color Encoding Format and Content Color Gamut
#define NVT_DP_INFOFRAME_MSA_MISC0_COLOR_FORMAT_SHIFT                   0x1
#define NVT_DP_INFOFRAME_MSA_MISC0_COLOR_FORMAT_LEGACY                  0x0       // RGB unspecified color space (legacy RGB mode)
#define NVT_DP_INFOFRAME_MSA_MISC0_COLOR_FORMAT_CEA_RGB                 0x4       // CEA RGB (sRGB primaries)
#define NVT_DP_INFOFRAME_MSA_MISC0_COLOR_FORMAT_WIDE_GAMUT_FIXED_POINT  0x3       // RGB wide gamut fixed point (XR8,XR10, XR12)
#define NVT_DP_INFOFRAME_MSA_MISC0_COLOR_FORMAT_WIDE_GAMUT_FLOAT_POINT  0xb      // RGB wide gamut floating point(scRGB)
#define NVT_DP_INFOFRAME_MSA_MISC0_COLOR_FORMAT_YCBCR_422_ITU601        0x5
#define NVT_DP_INFOFRAME_MSA_MISC0_COLOR_FORMAT_YCBCR_422_ITU709        0xd
#define NVT_DP_INFOFRAME_MSA_MISC0_COLOR_FORMAT_YCBCR_444_ITU601        0x6
#define NVT_DP_INFOFRAME_MSA_MISC0_COLOR_FORMAT_YCBCR_444_ITU709        0xe
#define NVT_DP_INFOFRAME_MSA_MISC0_COLOR_FORMAT_XVYCC_422_ITU601        0x1
#define NVT_DP_INFOFRAME_MSA_MISC0_COLOR_FORMAT_XVYCC_422_ITU709        0x9
#define NVT_DP_INFOFRAME_MSA_MISC0_COLOR_FORMAT_XVYCC_444_ITU601        0x2
#define NVT_DP_INFOFRAME_MSA_MISC0_COLOR_FORMAT_XVYCC_444_ITU709        0xa
#define NVT_DP_INFOFRAME_MSA_MISC0_COLOR_FORMAT_ADOBE_RGB               0xc

#define NVT_DP_INFOFRAME_MSA_MISC1_INTERLACED_V_TOTAL_MASK              0x01    // MISC1 bit 0 Interlaced Vertical Total
#define NVT_DP_INFOFRAME_MSA_MISC1_INTERLACED_V_TOTAL_SHIFT             0x0
#define NVT_DP_INFOFRAME_MSA_MISC1_INTERLACED_V_TOTAL_ODD               0x0
#define NVT_DP_INFOFRAME_MSA_MISC1_INTERLACED_V_TOTAL_EVEN              0x1

#define NVT_DP_INFOFRAME_MSA_MISC1_STEREO_MASK                          0x06    // MISC1 bits 2:1 stereo video attribute
#define NVT_DP_INFOFRAME_MSA_MISC1_STEREO_SHIFT                         0x1
#define NVT_DP_INFOFRAME_MSA_MISC1_STEREO_NONE                          0x0
#define NVT_DP_INFOFRAME_MSA_MISC1_STEREO_RIGHT_LEFT                    0x1
#define NVT_DP_INFOFRAME_MSA_MISC1_STEREO_LEFT_RIGHT                    0x3
#define NVT_DP_INFOFRAME_MSA_MISC1_STEREO_RESERVED                      0x2

#define NVT_DP_INFOFRAME_MSA_MISC1_RESERVED_MASK                        0x38    // MISC1 bits 5:3 reserved (DP1.3). Note: DP1.2 MISC 6:3 is reserved and undefined.
#define NVT_DP_INFOFRAME_MSA_MISC1_RESERVED_SHIFT                       0x3
#define NVT_DP_INFOFRAME_MSA_MISC1_RESERVED_DEFAULT                     0x0

#define NVT_DP_INFOFRAME_MSA_MISC1_VSC_SDP_MASK                         0x40    // MISC1 bit Using VSC SDP, and sink to ignore MISC1 bit 7 and MISC0 7:1.
#define NVT_DP_INFOFRAME_MSA_MISC1_VSC_SDP_SHIFT                        0x6
#define NVT_DP_INFOFRAME_MSA_MISC1_VSC_SDP_DISABLE                      0x0
#define NVT_DP_INFOFRAME_MSA_MISC1_VSC_SDP_ENABLE                       0x1

#define NVT_DP_INFOFRAME_MSA_MISC1_BITS_PER_COLOR_OR_LUMINANCE_MASK     0x80    // MISC1 bit 7 Y-Only Video
#define NVT_DP_INFOFRAME_MSA_MISC1_BITS_PER_COLOR_OR_LUMINANCE_SHIFT    0x7
#define NVT_DP_INFOFRAME_MSA_MISC1_BITS_PER_COLOR                       0x0
#define NVT_DP_INFOFRAME_MSA_MISC1_BITS_PER_LUMINANCE                   0x1

// ************************
// ** HDCP DPCD 1.0 Spec **
// ************************

// 0x68029 BSTATUS
#define NVT_DPCD_HDCP_BSTATUS                                          0x68029
#define NVT_DPCD_HDCP_BSTATUS_LINK_INTEGRITY_FAILURE                      0x04
#define NVT_DPCD_HDCP_BSTATUS_REAUTHENTICATION_REQUEST                    0x08

#define NVT_DPCD_HDCP_BCAPS_OFFSET                                   0x00068028
#define NVT_DPCD_HDCP_BCAPS_OFFSET_HDCP_CAPABLE                             0:0
#define NVT_DPCD_HDCP_BCAPS_OFFSET_HDCP_CAPABLE_NO                   0x00000000
#define NVT_DPCD_HDCP_BCAPS_OFFSET_HDCP_CAPABLE_YES                  0x00000001
#define NVT_DPCD_HDCP_BCAPS_OFFSET_HDCP_REPEATER                            1:1
#define NVT_DPCD_HDCP_BCAPS_OFFSET_HDCP_REPEATER_NO                  0x00000000
#define NVT_DPCD_HDCP_BCAPS_OFFSET_HDCP_REPEATER_YES                 0x00000001

#define NVT_DPCD_HDCP_BKSV_OFFSET                                    0x00068000
#define HDCP_KSV_SIZE                                                         5


// *********************************************
// ** Vendor DPCD for Apple's mDP->VGA dongle **
// *********************************************

// 0x30F DP2VGA_I2C_SPEED_CONTROL
#define NVT_DPCD_DP2VGA_I2C_SPEED_CONTROL                                0x30F

// 0x50C DP2VGA_GENERAL_STATUS
#define NVT_DPCD_DP2VGA_GENERAL_STATUS                                   0x50C

// 0x50D DP2VGA_I2C_SPEED_CAP
#define NVT_DPCD_DP2VGA_I2C_SPEED_CAP                                    0x50D
#define NVT_DPCD_DP2VGA_I2C_SPEED_CAP_SLOWEST                             0xFF
#define NVT_DPCD_DP2VGA_I2C_SPEED_CAP_1KBPS                               0x01
#define NVT_DPCD_DP2VGA_I2C_SPEED_CAP_3KBPS                               0x02
#define NVT_DPCD_DP2VGA_I2C_SPEED_CAP_10KBPS                              0x04
#define NVT_DPCD_DP2VGA_I2C_SPEED_CAP_100KBPS                             0x08
#define NVT_DPCD_DP2VGA_I2C_SPEED_CAP_400KBPS                             0x10
#define NVT_DPCD_DP2VGA_I2C_SPEED_CAP_1MBPS                               0x20


//
// HDMI/DP common definitions

#define NVT_DYNAMIC_RANGE_VESA                                     0x00
#define NVT_DYNAMIC_RANGE_CEA                                      0x01
#define NVT_DYNAMIC_RANGE_AUTO                                     0xFF


typedef struct tagNVT_PARSED_DPCD_INFO_DOWNSTREAM_PORT
{
    NvU8 type          :  3;                         // the downstream port type
    NvU8 isHpdAware    :  1;                         // if it's HPD aware
    NvU8 reserved      :  4;
}NVT_PARSED_DPCD_INFO_DOWNSTREAM_PORT;
//
typedef struct tagNVT_DPCD_PARSED_RECEIVER_INFO
{
    // receiver info
    NvU32 rev;                                        // DPCD version number
    NvU32 maxLinkRate;                                // the max link rate of main link lanes in 10KHz
    NvU32 maxLaneCount;                               // the max number of lanes
    NvU32 numOfPorts;                                 // the number of receiver ports
    NvU32 p0BufferSizePerLane;                        // the buffer size per lane (in BYTE)
    NvU32 p1BufferSizePerLane;                        // the buffer size per lane (in BYTE)

    // downstream port info
    NvU32 downstreamPortCount;                        // the total number of down stream ports
    NvU32 downstreamPort0Type;                        // type of downstream port 0
    NVT_PARSED_DPCD_INFO_DOWNSTREAM_PORT              downstreamPort[NVT_DPCD_RECEIVER_MAX_DOWNSTREAM_PORT];

    // other misc info
    NvU32 cap_support0_005DownSpread          : 1;
    NvU32 cap_supportEnhancedFrame            : 1;
    NvU32 cap_noAuxHandshakeLinkTraining      : 1;
    NvU32 cap_downstreamPortHasFormatConvBlk  : 1;
    NvU32 cap_mainLinkChSupportANSI8B10B      : 1;
    NvU32 cap_downstreamPortSupportOUI        : 1;
    NvU32 cap_p0HasEDID                       : 1;
    NvU32 cap_p0AssociatedToPrecedingPort     : 1;
    NvU32 cap_p1HasEDID                       : 1;
    NvU32 cap_p1AssociatedToPrecedingPort     : 1;

    // DP 1.2 fields
    NvU32 cap_mstm                            : 1;
    NvU32 cap_reserved                        : 21;
}NVT_DPCD_PARSED_RECEIVER_INFO;

#define NVT_DPCD_NUM_TRAINING_LANES 4

typedef struct tagNVT_TRAINING_LANE_SETTING
{
    NvU8 voltageSwing;
    NvU8 maxSwingReached;
    NvU8 preEmphasis;
    NvU8 maxPreEmphasisReached;
}NVT_TRAINING_LANE_SETTING;

// 00100h LINK CONFIGURATION FIELD
typedef struct tagNVT_DPCD_PARSED_LINK_CONFIG
{
    NvU8 linkRate;
    NvU8 laneCount;

    NVT_TRAINING_LANE_SETTING trainLaneSetting[NVT_DPCD_NUM_TRAINING_LANES];

    NvU32 enhancedFrameEnabled              : 1;
    NvU32 trainingPatternSetting            : 2;
    NvU32 linkQualityPatternSetting         : 2;
    NvU32 recoveredClockOutputEnabled       : 1;
    NvU32 scramblingDisable                 : 1;
    NvU32 symbolErrorCount                  : 2;
    NvU32 spreadAmp                         : 1;
    NvU32 mainLinkCoding8b10b               : 1;
    NvU32 multiStreamEnabled                : 1;
    NvU32 reserved                          : 19;
}NVT_DPCD_PARSED_LINK_CONFIG;

typedef struct tagNVT_DPCD_INFO
{
    NVT_DPCD_PARSED_RECEIVER_INFO        receiver;
    NVT_DPCD_PARSED_LINK_CONFIG          linkConfig;
    NvU32                                sourceOUI;
    NvU32                                sinkOUI;
    NvU32                                branchOUI;
}NVT_DPCD_INFO;

typedef struct tagNVT_DPCD_CONFIG
{
    NvU32 dpInfoFlags;
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_ENABLED                            0:0
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_ENABLED_FALSE             (0x00000000)
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_ENABLED_TRUE              (0x00000001)
#define  NV_DISPLAYPORT_INFO_FLAGS_DONGLE_TYPE                           7:4
#define  NV_DISPLAYPORT_INFO_FLAGS_DONGLE_TYPE_NONE             (0x00000000)
#define  NV_DISPLAYPORT_INFO_FLAGS_DONGLE_TYPE_DP2DVI           (0x00000001)  // B2: dp2dvi-singlelink
#define  NV_DISPLAYPORT_INFO_FLAGS_DONGLE_TYPE_DP2HDMI          (0x00000002)  // dp2hdmi
#define  NV_DISPLAYPORT_INFO_FLAGS_DONGLE_TYPE_DP2DVI2          (0x00000003)  // B3: dp2dvi-duallink
#define  NV_DISPLAYPORT_INFO_FLAGS_DONGLE_TYPE_DP2VGA           (0x00000004)  // B4: dp2vga
#define  NV_DISPLAYPORT_INFO_FLAGS_DONGLE_TYPE_DP2TV            (0x00000005)  // Composite/SVideo
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_MAX_CAP_LANECOUNT                 10:8   // Maximum supported laneCount
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_MAX_CAP_LANECOUNT_1_LANE  (0x00000000)
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_MAX_CAP_LANECOUNT_2_LANE  (0x00000001)
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_MAX_CAP_LANECOUNT_4_LANE  (0x00000002)
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_MAX_CAP_LINKRATE                 13:11   // Maximum supported linkRate
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_MAX_CAP_LINKRATE_1_62GBPS (0x00000000)
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_MAX_CAP_LINKRATE_2_70GBPS (0x00000001)
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_MAX_CAP_LINKRATE_5_40GBPS (0x00000002)
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_MAX_CAP_LINKRATE_8_10GBPS (0x00000003)
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_MULTISTREAM                      16:16  // Bit to check MST/SST
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_MULTISTREAM_DISABLED      (0x00000000)
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_MULTISTREAM_ENABLED       (0x00000001)
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_ENHANCED_FRAMING                 17:17  // Bit to check enhanced framing support
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_ENHANCED_FRAMING_DISABLED (0x00000000)
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_ENHANCED_FRAMING_ENABLED  (0x00000001)
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_DOWNSPREAD                       18:18  // Bit to check downspread support
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_DOWNSPREAD_DISABLED       (0x00000000)
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_DOWNSPREAD_ENABLED        (0x00000001)
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_SCRAMBLING                       19:19  // Bit to check scrambling
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_SCRAMBLING_DISABLED       (0x00000000)
#define  NV_DISPLAYPORT_INFO_FLAGS_DP_SCRAMBLING_ENABLED        (0x00000001)
    NvU32 linkRate;
    NvU32 laneCount;
    NvU32 colorFormat;
    NvU32 dynamicRange;
    NvU32 colorimetry;
    NvU32 bpc;
    NvU32 bpp;

    // pre-emphasis and drive current level (EFI might need this information)
    NvU8  laneData[4];
    // DP max pixelClock supported based on DP max laneCount/linkRate
    NvU32 dpMaxPixelClk;
    NvU8  maxCapLinkRate;
    NvU8  maxCapLaneCount;

    // B4 (DP2VGA) Vendor Specific I2C Speed Control
    NvU8  dp2vga_i2cCap;
    NvU8  dp2vga_i2cCtrl;

    NvU8  bDpOffline;
}NVT_DPCD_CONFIG;

typedef struct tagNVT_DPCD_DP_TUNNELING_CAPS
{
    NvU8 dpTunneling                      : 1; // DP Tunneling through USB4 Support
    NvU8 reserved                         : 5; // Reserved.
    NvU8 dpPanelReplayTunnelingOptSupport : 1; // Panel Replay Tunneling Optimization Support
    NvU8 dpInBwAllocationModeSupport      : 1; // DP IN Bandwidth Allocation Mode Support
}NVT_DPCD_DP_TUNNELING_CAPS;

typedef struct tagNVT_DPCD_DP_IN_ADAPTER_INFO
{
    NvU8 dpInAdapterNumber : 6; // DP IN Adapter Number
    NvU8 reserved          : 2;
}NVT_DPCD_DP_IN_ADAPTER_INFO;

typedef struct tagNVT_DPCD_USB4_DRIVER_ID
{
    NvU8 usb4DriverId : 4; // USB4 Driver ID
    NvU8 reserved     : 4;
}NVT_DPCD_USB4_DRIVER_ID;

//******************************
// Intel EDID Like Data (ELD)
//******************************
#define NVT_ELD_VER_1                                 0x1      // ELD version 1, which is an obsolete ELD structure. Treated as reserved
#define NVT_ELD_VER_2                                 0x2      // ELD version 2, which supports CEA version 861-D or below. Max baseline ELD size of 80 bytes (15 short audio descriptors)
#define NVT_ELD_VER_VIDEO_DRIVER_UNLOAD               0x1F     // Indicates an ELD that has been partially populated through implementation specific mean of default programming before an external
                                                               // graphics driver is load, Only the fields that is called out as "canned" fields will be populated, and audio driver should
                                                               // ignore the non "canned" fields.
#define NVT_ELD_CONN_TYPE_HDMI                        0x0      // indicates an HDMI connection type
#define NVT_ELD_CONN_TYPE_DP                          0x1      // indicates a DP connection type


//******************************
// Audio
//******************************
#define NVT_AUDIO_768KHZ                              768000   // HBR Audio
#define NVT_AUDIO_384KHZ                              384000   // HBR Audio
#define NVT_AUDIO_192KHZ                              192000
#define NVT_AUDIO_176KHZ                              176000
#define NVT_AUDIO_96KHZ                               96000
#define NVT_AUDIO_88KHZ                               88000
#define NVT_AUDIO_48KHZ                               48000
#define NVT_AUDIO_44KHZ                               44000
#define NVT_AUDIO_32KHZ                               32000

//Default format for HDTV is NVT_DEFAULT_HDTV_FMT i.e 1080i
#define NVT_DEFAULT_HDTV_PREFERRED_TIMING(x, y, z, p) \
    if(((x) == 1920) && ((y) == 1080) && ((z) != D3DDDI_VSSLO_PROGRESSIVE )) p = 1;

//Default format for non-DDC displays is 10x7
#define NVT_DEFAULT_NONDCC_PREFERRED_TIMING(x, y, z, p) \
    if(((x) == 1024) && ((y) == 768) && ((z) == 60 )) p = 1;


// Length of user-friendly monitor name, derived from the EDID's
// Display Product Name descriptor block, plus the EDID manufacturer PNP
// ID.  The Display Product can be distributed across four 13-byte
// descriptor blocks, and the PNP ID currently decodes to at most 40
// characters: 4*13 + 40 = 92
#define NVT_EDID_MONITOR_NAME_STRING_LENGTH                  96

// Compute the actual size of an EDID with a pointer to an NVT_EDID_INFO.
static NV_INLINE NvU32 NVT_EDID_ACTUAL_SIZE(const NVT_EDID_INFO *pInfo)
{
    return (pInfo->total_extensions + 1) * 128;
}

//******************************
//******************************
//**   the export functions   **
//******************************
//******************************

// the common timing function return values
typedef enum
{
    NVT_STATUS_SUCCESS = 0,          // Success (no status)
    NVT_STATUS_ERR     = 0x80000000, // generic get timing error
    NVT_STATUS_INVALID_PARAMETER,    // passed an invalid parameter
    NVT_STATUS_NO_MEMORY,            // memory allocation failed
    NVT_STATUS_COLOR_FORMAT_NOT_SUPPORTED,
    NVT_STATUS_INVALID_HBLANK,
    NVT_STATUS_INVALID_BPC,
    NVT_STATUS_INVALID_BPP,
    NVT_STATUS_MAX_LINE_BUFFER_ERROR,
    NVT_STATUS_OVERALL_THROUGHPUT_ERROR,
    NVT_STATUS_DSC_SLICE_ERROR,
    NVT_STATUS_PPS_SLICE_COUNT_ERROR,
    NVT_STATUS_PPS_SLICE_HEIGHT_ERROR,
    NVT_STATUS_PPS_SLICE_WIDTH_ERROR,
    NVT_STATUS_INVALID_PEAK_THROUGHPUT,
    NVT_STATUS_MIN_SLICE_COUNT_ERROR,
} NVT_STATUS;

//*************************************
// The EDID validation Mask
//*************************************
#define NVT_EDID_VALIDATION_MASK              0xFFFFFFFF
#define NVT_IS_EDID_VALIDATION_FLAGS(x, n)    ((((x)&NVT_EDID_VALIDATION_MASK)) & NVBIT32(n))
#define NVT_CLEAR_EDID_VALIDATION_FLAGS(x, n) ((x)&=(~NVBIT32(n)))

typedef enum
{
    // errors returned as a bitmask by NvTiming_EDIDValidationMask()
    NVT_EDID_VALIDATION_ERR_EXT = 0,
    NVT_EDID_VALIDATION_ERR_VERSION,
    NVT_EDID_VALIDATION_ERR_SIZE,
    NVT_EDID_VALIDATION_ERR_CHECKSUM,
    NVT_EDID_VALIDATION_ERR_RANGE_LIMIT,
    NVT_EDID_VALIDATION_ERR_DTD,
    NVT_EDID_VALIDATION_ERR_HEADER,
    NVT_EDID_VALIDATION_ERR_EXT_DTD,
    NVT_EDID_VALIDATION_ERR_EXTENSION_TAG,
    NVT_EDID_VALIDATION_ERR_EXTENSION_COUNT,
    NVT_EDID_VALIDATION_ERR_DESCRIPTOR,
    NVT_EDID_VALIDATION_ERR_EXT_CTA_BASIC,
    NVT_EDID_VALIDATION_ERR_EXT_CTA_DTD,
    NVT_EDID_VALIDATION_ERR_EXT_CTA_TAG,
    NVT_EDID_VALIDATION_ERR_EXT_CTA_SVD,
    NVT_EDID_VALIDATION_ERR_EXT_CTA_INVALID_DATA_BLOCK,
    NVT_EDID_VALIDATION_ERR_EXT_CTA_CHECKSUM,
    NVT_EDID_VALIDATION_ERR_EXT_DID_VERSION,
    NVT_EDID_VALIDATION_ERR_EXT_DID_EXTCOUNT,
    NVT_EDID_VALIDATION_ERR_EXT_DID_CHECKSUM,
    NVT_EDID_VALIDATION_ERR_EXT_DID_SEC_SIZE,
    NVT_EDID_VALIDATION_ERR_EXT_DID13_TAG,
    NVT_EDID_VALIDATION_ERR_EXT_DID13_TYPE1,
    NVT_EDID_VALIDATION_ERR_EXT_DID2_TAG,
    NVT_EDID_VALIDATION_ERR_EXT_DID2_USE_CASE,
    NVT_EDID_VALIDATION_ERR_EXT_DID2_MANDATORY_BLOCKS,
    NVT_EDID_VALIDATION_ERR_EXT_DID2_TYPE7,
    NVT_EDID_VALIDATION_ERR_EXT_DID2_TYPE10,
    NVT_EDID_VALIDATION_ERR_EXT_RANGE_LIMIT,
    NVT_EDID_VALIDATION_ERR_EXT_DID2_ADAPTIVE_SYNC,
} NVT_EDID_VALIDATION_ERR_STATUS;
#define NVT_EDID_VALIDATION_ERR_MASK(x) NVBIT32(x)

//*************************************
// The DisplayID2 validation Mask
//*************************************
typedef enum
{
    // errors returned as a bitmask by NvTiming_DisplayID2ValidationMask()
    NVT_DID2_VALIDATION_ERR_VERSION = 0,
    NVT_DID2_VALIDATION_ERR_SIZE,
    NVT_DID2_VALIDATION_ERR_CHECKSUM,
    NVT_DID2_VALIDATION_ERR_NO_DATA_BLOCK,
    NVT_EDID_VALIDATION_ERR_TAG,
    NVT_DID2_VALIDATION_ERR_RANGE_LIMIT,
    NVT_DID2_VALIDATION_ERR_NATIVE_DTD,
    NVT_DID2_VALIDATION_ERR_MANDATORY_BLOCKS,
    NVT_DID2_VALIDATION_ERR_PRODUCT_IDENTIFY,
    NVT_DID2_VALIDATION_ERR_PARAMETER,
    NVT_DID2_VALIDATION_ERR_INTERFACE,
    NVT_DID2_VALIDATION_ERR_TYPE7,
    NVT_DID2_VALIDATION_ERR_TYPE10,
    NVT_DID2_VALIDATION_ERR_ADAPTIVE_SYNC,
} NVT_DID2_VALIDATION_ERR_STATUS;
#define NVT_DID2_VALIDATION_ERR_MASK(x) NVBIT32(x)

// timing calculation flags:
#define NVT_FLAG_PROGRESSIVE_TIMING              0x00000000
#define NVT_FLAG_INTERLACED_TIMING               NVT_INTERLACED
#define NVT_FLAG_INTERLACED_TIMING2              NVT_INTERLACED_NO_EXTRA_VBLANK_ON_FIELD2  //without extra vblank on field 2
#define NVT_FLAG_DOUBLE_SCAN_TIMING              0x00000010
#define NVT_FLAG_REDUCED_BLANKING_TIMING         0x00000020
#define NVT_FLAG_MAX_EDID_TIMING                 0x00000040
#define NVT_FLAG_NV_DOUBLE_SCAN_TIMING           0x00000080
#define NVT_FLAG_NATIVE_TIMING                   0x00000100
#define NVT_FLAG_EDID_TIMING                     0x00000200
#define NVT_FLAG_CEA_4X3_TIMING                  0x00000400
#define NVT_FLAG_CEA_16X9_TIMING                 0x00000800
#define NVT_FLAG_OS_ADDED_TIMING                 0x00001000
#define NVT_FLAG_SPECTRUM_SPREAD                 0x00002000
#define NVT_FLAG_EDID_TIMING_RR_MATCH            0x00004000
#define NVT_FLAG_EDID_861_ST                     0x00008000
#define NVT_FLAG_EDID_DTD_EIZO_SPLIT             0x00010000
#define NVT_FLAG_DTD1_TIMING                     0x00020000
#define NVT_FLAG_NV_PREFERRED_TIMING             0x00040000
#define NVT_FLAG_DTD1_PREFERRED_TIMING           0x00080000
#define NVT_FLAG_DISPLAYID_DTD_PREFERRED_TIMING  0x00100000
#define NVT_FLAG_CTA_PREFERRED_TIMING            0x00200000
#define NVT_FLAG_DISPLAYID_T7_DSC_PASSTHRU       0x00400000
#define NVT_FLAG_DISPLAYID_2_0_TIMING            0x00800000  // this one for the CTA861 embedded in DID20
#define NVT_FLAG_DISPLAYID_T7_T8_EXPLICT_YUV420  0x01000000  // DID2 E7 spec. supported yuv420 indicated
#define NVT_FLAG_CTA_NATIVE_TIMING               0x02000000  // NVRDB defined
#define NVT_FLAG_CTA_OVT_TIMING                  0x04000000  // CTA861 CTA OVT Timing
#define NVT_FLAG_CTA_OVT_FRR_TIMING              0x08000000  // CTA861 CTA OVT Timing supported ntsc

#define NVT_FLAG_INTERLACED_MASK                 (NVT_FLAG_INTERLACED_TIMING | NVT_FLAG_INTERLACED_TIMING2)

#ifdef __cplusplus
extern "C" {
#endif

// Generic timing parameter calculation
NvU16 NvTiming_CalcRR(NvU32 pclk, NvU16 interlaced, NvU16 HTotal, NvU16 VTotal);
NvU32 NvTiming_CalcRRx1k(NvU32 pclk, NvU16 interlaced, NvU16 HTotal, NvU16 VTotal);

NvU32 NvTiming_IsRoundedRREqual(NvU16 rr1, NvU32 rr1x1k, NvU16 rr2);
NvU32 NvTiming_IsTimingExactEqual(const NVT_TIMING *pT1, const NVT_TIMING *pT2);
NvU32 NvTiming_IsTimingExactEqualEx(const NVT_TIMING *pT1, const NVT_TIMING *pT2);
NvU32 NvTiming_IsTimingRelaxedEqual(const NVT_TIMING *pT1, const NVT_TIMING *pT2);
NvU16 NvTiming_MaxFrameWidth(NvU16 HVisible, NvU16 rep);

NvU32 NvTiming_GetVrrFmin(const NVT_EDID_INFO *pEdidInfo, const NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo,
                          NvU32 nominalRefreshRateHz, NVT_PROTOCOL sinkProtocol);

// Establish timing enumeration
NVT_STATUS NvTiming_EnumEST(NvU32 index, NVT_TIMING *pT);
NVT_STATUS NvTiming_EnumESTIII(NvU32 index, NVT_TIMING *pT);

// GTF timing calculation
NVT_STATUS NvTiming_CalcGTF(NvU32 width, NvU32 height, NvU32 rr, NvU32 flag, NVT_TIMING *pT);

// DMT timing calculation
NVT_STATUS NvTiming_EnumDMT(NvU32 dmtId, NVT_TIMING *pT);
NVT_STATUS NvTiming_EnumStdTwoBytesCode(NvU16 std2ByteCodes, NVT_TIMING *pT);
NVT_STATUS NvTiming_CalcDMT(NvU32 width, NvU32 height, NvU32 rr, NvU32 flag, NVT_TIMING *pT);
NVT_STATUS NvTiming_CalcDMT_RB(NvU32 width, NvU32 height, NvU32 rr, NvU32 flag, NVT_TIMING *pT);
NVT_STATUS NvTiming_CalcDMT_RB2(NvU32 width, NvU32 height, NvU32 rr, NvU32 flag, NVT_TIMING *pT);

// CVT timing calculation
NVT_STATUS NvTiming_CalcCVT(NvU32 width, NvU32 height, NvU32 rr, NvU32 flag, NVT_TIMING *pT);
NVT_STATUS NvTiming_CalcCVT_RB(NvU32 width, NvU32 height, NvU32 rr, NvU32 flag, NVT_TIMING *pT);
NVT_STATUS NvTiming_CalcCVT_RB2(NvU32 width, NvU32 height, NvU32 rr, NvBool is1000div1001, NvBool isAltMiniVblankTiming, NVT_TIMING *pT);
NVT_STATUS NvTiming_CalcCVT_RB3(NvU32 width, NvU32 height, NvU32 rr, NvU32 deltaHBlank, NvU32 vBlankMicroSec, NvBool isAltMiniVblankTiming, NvBool isEarlyVSync, NVT_TIMING *pT);
NvBool NvTiming_IsTimingCVTRB(const NVT_TIMING *pTiming);

// OVT timing calculation
NVT_STATUS NvTiming_CalcOVT(NvU32 width, NvU32 height, NvU32 rr, NVT_TIMING *pT);
NvBool NvTiming_IsTimingOVT(const NVT_TIMING *pTiming);

// CEA/EIA/Psf timing
NVT_STATUS NvTiming_CalcCEA861bTiming(NvU32 width, NvU32 height, NvU32 rr, NvU32 flag, NvU32 pixelRepeatCount, NVT_TIMING *pT);
NVT_STATUS NvTiming_EnumCEA861bTiming(NvU32 ceaFormat, NVT_TIMING *pT);
NVT_STATUS NvTiming_EnumNvPsfTiming(NvU32 nvPsfFormat, NVT_TIMING *pT);
NvU32      NvTiming_GetCEA861TimingIndex(NVT_TIMING *pT);

//expose the HDMI extended video timing defined by the HDMI LLC VSDB
NVT_STATUS NvTiming_EnumHdmiVsdbExtendedTiming(NvU32 hdmi_vic, NVT_TIMING *pT);

// TV(analog) based timing
NVT_STATUS NvTiming_GetTvTiming(NvU32 width, NvU32 height, NvU32 rr, NvU32 flag, NvU32 tvFormat, NVT_TIMING *pT);

// Get EDID timing
NVT_STATUS NvTiming_GetEdidTimingExWithPclk(NvU32 width, NvU32 height, NvU32 rr, NvU32 flag, NVT_EDID_INFO *pEdidInfo, NVT_TIMING *pT, NvU32 rrx1k, NvU32 pclk);
NVT_STATUS NvTiming_GetEdidTimingEx(NvU32 width, NvU32 height, NvU32 rr, NvU32 flag, NVT_EDID_INFO *pEdidInfo, NVT_TIMING *pT, NvU32 rrx1k);
NVT_STATUS NvTiming_GetEdidTiming(NvU32 width, NvU32 height, NvU32 rr, NvU32 flag, NVT_EDID_INFO *pEdidInfo, NVT_TIMING *pT);

// Get EDID based HDMI Stereo timing
NVT_STATUS NvTiming_GetHDMIStereoExtTimingFromEDID(NvU32 width, NvU32 height, NvU32 rr, NvU8 structure, NvU8 detail, NvU32 flag, NVT_EDID_INFO *pEdidInfo, NVT_EXT_TIMING *pT);
void       NvTiming_GetHDMIStereoTimingFrom2DTiming(const NVT_TIMING *pTiming, NvU8 StereoStructureType, NvU8 SideBySideHalfDetail, NVT_EXT_TIMING *pExtTiming);
NVT_STATUS NvTiming_GetHDMIStereoMandatoryFormatDetail(const NvU8 vic, NvU16 *pStereoStructureMask, NvU8 *pSideBySideHalfDetail);

// EDID based AspectRatio Timing
NVT_STATUS NvTiming_GetEDIDBasedASPRTiming(NvU16 width, NvU16 height, NvU16 rr, NVT_EDID_INFO *pEI, NVT_TIMING *ft);

// EDID or DISPLAYID2 version
NvU32 NvTiming_GetVESADisplayDescriptorVersion(NvU8 *rawData, NvU32 *pVer);

// EDID entry parse
NVT_STATUS NV_STDCALL NvTiming_ParseEDIDInfo(NvU8 *pEdid, NvU32 length, NVT_EDID_INFO *pEdidInfo);
NvU32 NvTiming_EDIDValidationMask(NvU8 *pEdid, NvU32 length, NvBool bIsStrongValidation);
NvU32 NvTiming_EDIDStrongValidationMask(NvU8 *pEdid, NvU32 length);
NVT_STATUS NvTiming_EDIDValidation(NvU8 *pEdid, NvU32 length, NvBool bIsStrongValidation);

// DisplayID20 standalone entry parse
NVT_STATUS NV_STDCALL NvTiming_parseDisplayId20Info(const NvU8 *pDisplayId, NvU32 length, NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo);
NvU32 NvTiming_DisplayID2ValidationMask(NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo, NvBool bIsStrongValidation);
NVT_STATUS NvTiming_DisplayID2ValidationDataBlocks(NVT_DISPLAYID_2_0_INFO *pDisplayIdInfo, NvBool bIsStrongValidation);

NVT_STATUS NvTiming_Get18ByteLongDescriptorIndex(NVT_EDID_INFO *pEdidInfo, NvU8 tag, NvU32 *dtdIndex);
NVT_STATUS NvTiming_GetProductName(const NVT_EDID_INFO *pEdidInfo, NvU8 *pProductName, const NvU32 productNameLength);
NvU32 NvTiming_CalculateEDIDCRC32(NvU8* pEDIDBuffer,  NvU32 edidsize);
NvU32 NvTiming_CalculateCommonEDIDCRC32(NvU8* pEDIDBuffer, NvU32 edidVersion);
NVT_STATUS NvTiming_CalculateEDIDLimits(NVT_EDID_INFO *pEdidInfo, NVT_EDID_RANGE_LIMIT *pLimit);
void NvTiming_GetMonitorName(NVT_EDID_INFO *pEdidInfo, NvU8 monitor_name[NVT_EDID_MONITOR_NAME_STRING_LENGTH]);

// utility routines
NvU64 axb_div_c_64(NvU64 a, NvU64 b, NvU64 c);
NvU32 axb_div_c(NvU32 a, NvU32 b, NvU32 c);
NvU32 a_div_b(NvU32 a, NvU32 b);
NvU32 calculateCRC32(NvU8* pBuf, NvU32 bufsize);
void  patchChecksum(NvU8* pBuf);
NvBool isChecksumValid(NvU8* pBuf);
NvU32 RRx1kToPclk (NVT_TIMING *pT);

NVT_STATUS NvTiming_ComposeCustTimingString(NVT_TIMING *pT);

// Infoframe/SDP composer
NVT_STATUS  NvTiming_ConstructVideoInfoframeCtrl(const NVT_TIMING *pTiming, NVT_VIDEO_INFOFRAME_CTRL *pCtrl);
NVT_STATUS  NvTiming_ConstructVideoInfoframe(NVT_EDID_INFO *pEdidInfo, NVT_VIDEO_INFOFRAME_CTRL *pCtrl, NVT_VIDEO_INFOFRAME *pContext, NVT_VIDEO_INFOFRAME *p);
NVT_STATUS  NvTiming_ConstructAudioInfoframe(NVT_AUDIO_INFOFRAME_CTRL *pCtrl, NVT_AUDIO_INFOFRAME *pContext, NVT_AUDIO_INFOFRAME *p);
NVT_STATUS  NvTiming_ConstructVendorSpecificInfoframe(NVT_EDID_INFO *pEdidInfo, NVT_VENDOR_SPECIFIC_INFOFRAME_CTRL *pCtrl, NVT_VENDOR_SPECIFIC_INFOFRAME *p);
NVT_STATUS  NvTiming_ConstructExtendedMetadataPacketInfoframe(NVT_EXTENDED_METADATA_PACKET_INFOFRAME_CTRL *pCtrl, NVT_EXTENDED_METADATA_PACKET_INFOFRAME *p);
void        NvTiming_ConstructAdaptiveSyncSDP(const NVT_ADAPTIVE_SYNC_SDP_CTRL *pCtrl, NVT_ADAPTIVE_SYNC_SDP *p);


// Get specific timing from parsed EDID
NVT_STATUS NvTiming_GetDTD1Timing (NVT_EDID_INFO * pEdidInfo, NVT_TIMING * pT);

#define NVT_IS_DTD(d)                               (NVT_GET_TIMING_STATUS_TYPE((d)) == NVT_TYPE_EDID_DTD)
#define NVT_IS_EXT_DTD(d)                           (NVT_GET_TIMING_STATUS_TYPE((d)) == NVT_TYPE_EDID_EXT_DTD)
#define NVT_IS_CTA861(d)                            (NVT_GET_TIMING_STATUS_TYPE((d)) == NVT_TYPE_EDID_861ST)
#define NVT_IS_CTA861_DID_T7(d)                     (NVT_GET_TIMING_STATUS_TYPE((d)) == NVT_TYPE_CTA861_DID_T7)
#define NVT_IS_CTA861_DID_T8(d)                     (NVT_GET_TIMING_STATUS_TYPE((d)) == NVT_TYPE_CTA861_DID_T8)
#define NVT_IS_CTA861_DID_T10(d)                    (NVT_GET_TIMING_STATUS_TYPE((d)) == NVT_TYPE_CTA861_DID_T10)

#define NVT_IS_DTD1(d)                              ((NVT_IS_DTD((d)))            && (NVT_GET_TIMING_STATUS_SEQ((d)) == 1))
#define NVT_IS_DTDn(d, n)                           ((NVT_IS_DTD((d)))            && (NVT_GET_TIMING_STATUS_SEQ((d)) == n))
#define NVT_IS_EXT_DTDn(d, n)                       ((NVT_IS_EXT_DTD((d)))        && (NVT_GET_TIMING_STATUS_SEQ((d)) == n))
#define NVT_IS_CTA861_DID_T7n(d, n)                 ((NVT_IS_CTA861_DID_T7((d)))  && (NVT_GET_TIMING_STATUS_SEQ((d)) == n))
#define NVT_IS_CTA861_DID_T8_1(d)                   ((NVT_IS_CTA861_DID_T8((d)))  && (NVT_GET_TIMING_STATUS_SEQ((d)) == 1))
#define NVT_IS_CTA861_DID_T10n(d, n)                ((NVT_IS_CTA861_DID_T10((d))) && (NVT_GET_TIMING_STATUS_SEQ((d)) == n))

#define NVT_IS_CTA861_OVT_Tn(flag, status, n)       ((0 != (NVT_FLAG_CTA_OVT_TIMING & (flag))) && (NVT_GET_TIMING_STATUS_SEQ((status)) == n))

#define NVT_DID20_TIMING_IS_CTA861(flag, status)    ((NVT_IS_CTA861((status))) && (0 != (NVT_FLAG_DISPLAYID_2_0_TIMING & (flag))))
#define NVT_PREFERRED_TIMING_IS_DTD1(flag, status)  ((NVT_IS_DTD1((status)))   && (0 != (NVT_FLAG_DTD1_PREFERRED_TIMING & (flag))))
#define NVT_PREFERRED_TIMING_IS_DISPLAYID(flag)     (0 != (NVT_FLAG_DISPLAYID_DTD_PREFERRED_TIMING & flag))
#define NVT_PREFERRED_TIMING_IS_CTA(flag)           (0 != (NVT_FLAG_CTA_PREFERRED_TIMING & flag))
#define NVT_NATIVE_TIMING_IS_CTA(flag)              (0 != (NVT_FLAG_CTA_NATIVE_TIMING & flag))
#define NVT_TIMING_IS_OVT(flag)                     (0 != (NVT_FLAG_CTA_OVT_TIMING & flag))
#define NVT_FRR_TIMING_IS_OVT(flag)                 (0 != (NVT_FLAG_CTA_OVT_FRR_TIMING & flag))

#ifdef __cplusplus
}
#endif

#endif //__NVTIMING_H__
