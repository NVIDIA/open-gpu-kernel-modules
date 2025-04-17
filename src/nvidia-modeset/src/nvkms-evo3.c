/*
 * SPDX-FileCopyrightText: Copyright (c) 2010-2023 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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
 * This file contains implementations of the EVO HAL methods for display class
 * 3.x (also known as "nvdisplay").
 */

#include "nvkms-dma.h"
#include "nvkms-types.h"
#include "nvkms-rmapi.h"
#include "nvkms-surface.h"
#include "nvkms-softfloat.h"
#include "nvkms-evo.h"
#include "nvkms-evo1.h"
#include "nvkms-evo3.h"
#include "nvkms-prealloc.h"
#include "nv-float.h"
#include "nvkms-dpy.h"
#include "nvkms-vrr.h"
#include "nvkms-ctxdma.h"

#include <nvmisc.h>

#include <class/clc372sw.h> // NVC372_DISPLAY_SW
#include <class/clc373.h> // NVC373_DISP_CAPABILITIES
#include <class/clc37b.h> // NVC37B_WINDOW_IMM_CHANNEL_DMA
#include <class/clc37d.h> // NVC37D_CORE_CHANNEL_DMA
#include <class/clc37dcrcnotif.h> // NVC37D_NOTIFIER_CRC
#include <class/clc37dswspare.h> // NVC37D_HEAD_SET_SW_SPARE_*
#include <class/clc37e.h> // NVC37E_WINDOW_CHANNEL_DMA
#include <class/clc573.h> // NVC573_DISP_CAPABILITIES
#include <class/clc57d.h> // NVC57D_CORE_CHANNEL_DMA
#include <class/clc57e.h> // NVC57E_WINDOW_CHANNEL_DMA
#include <class/clc57esw.h>
#include <class/clc673.h> // NVC673_DISP_CAPABILITIES
#include <class/clc67d.h> // NVC67D_CORE_CHANNEL_DMA
#include <class/clc67e.h> // NVC67E_WINDOW_CHANNEL_DMA

#include <ctrl/ctrlc370/ctrlc370chnc.h>
#include <ctrl/ctrlc370/ctrlc370rg.h>
#include <ctrl/ctrlc372/ctrlc372chnc.h>

#define NV_EVO3_X_EMULATED_SURFACE_MEMORY_FORMATS_C3        \
    (NVBIT64(NvKmsSurfaceMemoryFormatRF16GF16BF16XF16))

#define NV_EVO3_X_EMULATED_SURFACE_MEMORY_FORMATS_C5        \
    (NVBIT64(NvKmsSurfaceMemoryFormatRF16GF16BF16XF16))

/** Number of CRCs supported by hardware on NVC37D hardware (SF/SOR, Comp, RG) */
#define NV_EVO3_NUM_CRC_FIELDS 3

/** Number of CRCs supported by hardware on NVC37D hardware SF/SOR, Comp, RG Ovf and Count */
#define NV_EVO3_NUM_CRC_FLAGS 4

enum FMTCoeffType
{
    FMT_COEFF_TYPE_IDENTITY = 0,

    FMT_COEFF_TYPE_REC601_YUV_8BPC_LTD_TO_RGB_16BPC_FULL,
    FMT_COEFF_TYPE_REC601_YUV_8BPC_FULL_TO_RGB_16BPC_FULL,
    FMT_COEFF_TYPE_REC601_YUV_10BPC_LTD_TO_RGB_16BPC_FULL,
    FMT_COEFF_TYPE_REC601_YUV_10BPC_FULL_TO_RGB_16BPC_FULL,
    FMT_COEFF_TYPE_REC601_YUV_12BPC_LTD_TO_RGB_16BPC_FULL,
    FMT_COEFF_TYPE_REC601_YUV_12BPC_FULL_TO_RGB_16BPC_FULL,

    FMT_COEFF_TYPE_REC709_YUV_8BPC_LTD_TO_RGB_16BPC_FULL,
    FMT_COEFF_TYPE_REC709_YUV_8BPC_FULL_TO_RGB_16BPC_FULL,
    FMT_COEFF_TYPE_REC709_YUV_10BPC_LTD_TO_RGB_16BPC_FULL,
    FMT_COEFF_TYPE_REC709_YUV_10BPC_FULL_TO_RGB_16BPC_FULL,
    FMT_COEFF_TYPE_REC709_YUV_12BPC_LTD_TO_RGB_16BPC_FULL,
    FMT_COEFF_TYPE_REC709_YUV_12BPC_FULL_TO_RGB_16BPC_FULL,

    FMT_COEFF_TYPE_REC2020_YUV_8BPC_LTD_TO_RGB_16BPC_FULL,
    FMT_COEFF_TYPE_REC2020_YUV_8BPC_FULL_TO_RGB_16BPC_FULL,
    FMT_COEFF_TYPE_REC2020_YUV_10BPC_LTD_TO_RGB_16BPC_FULL,
    FMT_COEFF_TYPE_REC2020_YUV_10BPC_FULL_TO_RGB_16BPC_FULL,
    FMT_COEFF_TYPE_REC2020_YUV_12BPC_LTD_TO_RGB_16BPC_FULL,
    FMT_COEFF_TYPE_REC2020_YUV_12BPC_FULL_TO_RGB_16BPC_FULL,

    // FMT is always identity for RGB to avoid possible calculation error.

    // must be the last entry
    FMT_COEFF_TYPE_MAX
};

static const NvU32 FMTMatrix[FMT_COEFF_TYPE_MAX][12] =
{
    // FMT_COEFF_TYPE_IDENTITY
    {  0x10000,        0,        0,        0,        0,  0x10000,        0,        0,        0,        0,  0x10000,        0 },

    // FMT_COEFF_TYPE_REC601_YUV_8BPC_LTD_TO_RGB_16BPC_FULL
    {  0x19A29,  0x12B3C,        0, 0x1F2038, 0x1F2F14,  0x12B3C, 0x1F9B52,   0x8819,        0,  0x12B3C,  0x20668, 0x1EEA18 },
    // FMT_COEFF_TYPE_REC601_YUV_8BPC_FULL_TO_RGB_16BPC_FULL
    {  0x1684C,  0x100FD,        0, 0x1F4D42, 0x1F487A,  0x100FD, 0x1FA790,   0x86EB,        0,  0x100FD,  0x1C762, 0x1F1E16 },
    // FMT_COEFF_TYPE_REC601_YUV_10BPC_LTD_TO_RGB_16BPC_FULL
    {  0x19A29,  0x12B3C,        0, 0x1F2038, 0x1F2F14,  0x12B3C, 0x1F9B52,   0x8819,        0,  0x12B3C,  0x20668, 0x1EEA18 },
    // FMT_COEFF_TYPE_REC601_YUV_10BPC_FULL_TO_RGB_16BPC_FULL
    {  0x1673E,  0x1003C,        0, 0x1F4CBB, 0x1F4903,  0x1003C, 0x1FA7D2,   0x8751,        0,  0x1003C,  0x1C60C, 0x1F1D6B },
    // FMT_COEFF_TYPE_REC601_YUV_12BPC_LTD_TO_RGB_16BPC_FULL
    {  0x19A29,  0x12B3C,        0, 0x1F2038, 0x1F2F14,  0x12B3C, 0x1F9B52,   0x8819,        0,  0x12B3C,  0x20668, 0x1EEA18 },
    // FMT_COEFF_TYPE_REC601_YUV_12BPC_FULL_TO_RGB_16BPC_FULL
    {  0x166FA,  0x1000C,        0, 0x1F4C99, 0x1F4926,  0x1000C, 0x1FA7E3,   0x876B,        0,  0x1000C,  0x1C5B7, 0x1F1D41 },

    // FMT_COEFF_TYPE_REC709_YUV_8BPC_LTD_TO_RGB_16BPC_FULL
    {  0x1CCB7,  0x12B3C,        0, 0x1F06F1, 0x1F770C,  0x12B3C, 0x1FC933,   0x4D2D,        0,  0x12B3C,  0x21EDD, 0x1EDDDE },
    // FMT_COEFF_TYPE_REC709_YUV_8BPC_FULL_TO_RGB_16BPC_FULL
    {  0x194B4,  0x100FD,        0, 0x1F373A, 0x1F87B3,  0x100FD, 0x1FCFDC,   0x5390,        0,  0x100FD,  0x1DCDE, 0x1F136E },
    // FMT_COEFF_TYPE_REC709_YUV_10BPC_LTD_TO_RGB_16BPC_FULL
    {  0x1CCB7,  0x12B3C,        0, 0x1F06F1, 0x1F770C,  0x12B3C, 0x1FC933,   0x4D2D,        0,  0x12B3C,  0x21EDD, 0x1EDDDE },
    // FMT_COEFF_TYPE_REC709_YUV_10BPC_FULL_TO_RGB_16BPC_FULL
    {  0x19385,  0x1003C,        0, 0x1F36A3, 0x1F880D,  0x1003C, 0x1FD000,   0x53CF,        0,  0x1003C,  0x1DB78, 0x1F12BB },
    // FMT_COEFF_TYPE_REC709_YUV_12BPC_LTD_TO_RGB_16BPC_FULL
    {  0x1CCB7,  0x12B3C,        0, 0x1F06F1, 0x1F770C,  0x12B3C, 0x1FC933,   0x4D2D,        0,  0x12B3C,  0x21EDD, 0x1EDDDE },
    // FMT_COEFF_TYPE_REC709_YUV_12BPC_FULL_TO_RGB_16BPC_FULL
    {  0x19339,  0x1000C,        0, 0x1F367D, 0x1F8823,  0x1000C, 0x1FD009,   0x53DF,        0,  0x1000C,  0x1DB1F, 0x1F128E },

    // FMT_COEFF_TYPE_REC2020_YUV_8BPC_LTD_TO_RGB_16BPC_FULL
    {  0x1AF66,  0x12B3C,        0, 0x1F1599, 0x1F58D9,  0x12B3C, 0x1FCFDC,   0x58F2,        0,  0x12B3C,  0x22669, 0x1EDA18 },
    // FMT_COEFF_TYPE_REC2020_YUV_8BPC_FULL_TO_RGB_16BPC_FULL
    {  0x17AF4,  0x100FD,        0, 0x1F4401, 0x1F6D2B,  0x100FD, 0x1FD5B6,   0x5DD2,        0,  0x100FD,  0x1E37F, 0x1F1024 },
    // FMT_COEFF_TYPE_REC2020_YUV_10BPC_LTD_TO_RGB_16BPC_FULL
    {  0x1AF66,  0x12B3C,        0, 0x1F1599, 0x1F58D9,  0x12B3C, 0x1FCFDC,   0x58F2,        0,  0x12B3C,  0x22669, 0x1EDA18 },
    // FMT_COEFF_TYPE_REC2020_YUV_10BPC_FULL_TO_RGB_16BPC_FULL
    {  0x179D8,  0x1003C,        0, 0x1F4372, 0x1F6D99,  0x1003C, 0x1FD5D6,   0x5E19,        0,  0x1003C,  0x1E214, 0x1F0F6E },
    // FMT_COEFF_TYPE_REC2020_YUV_12BPC_LTD_TO_RGB_16BPC_FULL
    {  0x1AF66,  0x12B3C,        0, 0x1F1599, 0x1F58D9,  0x12B3C, 0x1FCFDC,   0x58F2,        0,  0x12B3C,  0x22669, 0x1EDA18 },
    // FMT_COEFF_TYPE_REC2020_YUV_12BPC_FULL_TO_RGB_16BPC_FULL
    {  0x17991,  0x1000C,        0, 0x1F434F, 0x1F6DB5,  0x1000C, 0x1FD5DE,   0x5E2B,        0,  0x1000C,  0x1E1BA, 0x1F0F41 },
};

static void SetCsc00MatrixC5(NVEvoChannelPtr pChannel,
                             const struct NvKmsCscMatrix *matrix);
static void SetCsc01MatrixC5(NVEvoChannelPtr pChannel,
                             const struct NvKmsCscMatrix *matrix);
static void SetCsc10MatrixC5(NVEvoChannelPtr pChannel,
                             const struct NvKmsCscMatrix *matrix);
static void SetCsc11MatrixC5(NVEvoChannelPtr pChannel,
                             const struct NvKmsCscMatrix *matrix);
static void
UpdateCompositionC3(NVDevEvoPtr pDevEvo,
                    NVEvoChannelPtr pChannel,
                    const struct NvKmsCompositionParams *pCompParams,
                    NVEvoUpdateState *updateState,
                    enum NvKmsSurfaceMemoryFormat format);
static void
UpdateCompositionC5(NVDevEvoPtr pDevEvo,
                    NVEvoChannelPtr pChannel,
                    const struct NvKmsCompositionParams *pCompParams,
                    NVEvoUpdateState *updateState,
                    NvBool bypassComposition,
                    enum NvKmsSurfaceMemoryFormat format);

static void
EvoSetupIdentityOutputLutC5(NVEvoLutDataRec *pData,
                            enum NvKmsLUTState *lutState,
                            NvU32 *lutSize,
                            NvBool *isLutModeVss);

static void
EvoSetupIdentityBaseLutC5(NVEvoLutDataRec *pData,
                          enum NvKmsLUTState *lutState,
                          NvU32 *lutSize,
                          NvBool *isLutModeVss);

ct_assert(NV_EVO_LOCK_PIN_0 >
          NVC37D_HEAD_SET_CONTROL_MASTER_LOCK_PIN_INTERNAL_SCAN_LOCK__SIZE_1);

/* nvdisplay has a maximum of 2 eyes and 3 planes per surface */
ct_assert((NVKMS_MAX_EYES * NVKMS_MAX_PLANES_PER_SURFACE) == 6);

/* Windows support all composition modes. */
#define NV_EVO3_SUPPORTED_WINDOW_COMP_BLEND_MODES              \
    ((1 << NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE)                    | \
     (1 << NVKMS_COMPOSITION_BLENDING_MODE_TRANSPARENT)               | \
     (1 << NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_ALPHA)         | \
     (1 << NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_ALPHA)             | \
     (1 << NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_SURFACE_ALPHA) | \
     (1 << NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_SURFACE_ALPHA))

#define NV_EVO3_DEFAULT_WINDOW_USAGE_BOUNDS_C3                                     \
    (DRF_DEF(C37D, _WINDOW_SET_WINDOW_USAGE_BOUNDS, _INPUT_LUT, _USAGE_1025)     | \
     DRF_DEF(C37D, _WINDOW_SET_WINDOW_USAGE_BOUNDS, _INPUT_SCALER_TAPS, _TAPS_2) | \
     DRF_DEF(C37D, _WINDOW_SET_WINDOW_USAGE_BOUNDS, _UPSCALING_ALLOWED, _FALSE))

static inline NvU8 EyeAndPlaneToCtxDmaIdx(const NvU8 eye, const NvU8 plane)
{
    /*
     * See the definition of the SetContextDmaIso and SetOffset methods in the
     * relevant nvdClass_01.mfs file to see how these method array indices are
     * mapped.
     */
    nvAssert((eye < NVKMS_MAX_EYES) && (plane < NVKMS_MAX_PLANES_PER_SURFACE));

    return eye + (plane << 1);
}

static void InitChannelCapsC3(NVDevEvoPtr pDevEvo,
                              NVEvoChannelPtr pChannel)
{
    if ((pChannel->channelMask & NV_EVO_CHANNEL_MASK_WINDOW_ALL) != 0) {
        static const NVEvoChannelCaps WindowCaps = {
            /*
             * Window classes always support timestamp flips, and allow full
             * use of the 64-bit timestamp value.
             */
            .validTimeStampBits = 64,
            /* Window classes always support tearing flips. */
            .tearingFlips = TRUE,
            .vrrTearingFlips = TRUE,
            /* Window classes support per-eye stereo flips. */
            .perEyeStereoFlips = TRUE,
        };

        pChannel->caps = WindowCaps;
    }
}

// The coefficient values are obtained from bug 1953108 comment 10
// Per MFS: However since all 5 coefficients have to add up to 1.0, only 4 need to be specified, and
//          HW can derive the missing one. The center coefficient is the one that is left out, so
//          if the 5 taps need weights (c0, c1, c2, c3, c4) then only (c0, c1, c3, c4) are stored,
//          and c2 is calculated by HW.
//          Phase 0 is the center phase and the corresponding filter kernel is symmetrical:
//          c0=c4, c1=c3  --> only c0 and c1 need to be stored.
//          Phase 16 (and -16) is the edge phase and the corresponding filter kernels are:
//          (0, c0, c1, c1, c0) for phase +16
//          (c0, c1, c1, c0, 0) for phase -16
//          The difference between +16 and -16 is automatically handled by HW. The table only needs
//          to store c0 and c1 for either case.
// Therefore, based on MFS above, the matrix below contains the values loaded to HW.
// Real Phase 0 is commented for easy reference.
// Also, phase 16 values (last row) are commented, but its C0,C1 values are loaded in row 0/phase 0.
const NvU32 scalerTaps5Coeff[NUM_SCALER_RATIOS][NUM_TAPS5_COEFF_PHASES][NUM_TAPS5_COEFF_VALUES] =
{
 // ratio = 1
 {{  0  ,   0 ,             -16 ,  144}, // real phase 0:{ 0, 0, /*256,*/ 0, 0 },
  {  0  ,  -5 ,  /*255,*/     5 ,    0},
  {  0  ,  -9 ,  /*254,*/    11 ,    0},
  { -1  , -12 ,  /*251,*/    18 ,   -1},
  { -1  , -15 ,  /*248,*/    25 ,   -1},
  { -1  , -18 ,  /*243,*/    33 ,   -2},
  { -2  , -20 ,  /*238,*/    42 ,   -3},
  { -2  , -21 ,  /*232,*/    51 ,   -3},
  { -3  , -22 ,  /*225,*/    60 ,   -5},
  { -3  , -22 ,  /*217,*/    70 ,   -6},
  { -4  , -22 ,  /*208,*/    81 ,   -7},
  { -4  , -22 ,  /*199,*/    91 ,   -9},
  { -5  , -21 ,  /*190,*/   102 ,  -10},
  { -5  , -20 ,  /*180,*/   113 ,  -12},
  { -5  , -19 ,  /*169,*/   125 ,  -13},
  { -6  , -18 ,  /*158,*/   136 ,  -15}
  // real phase 16: {  0  , -16 ,  144,   144 ,  -16        }
 },
 // ratio = 2
 {{ 3,    60 ,               20 , 108 }, // real phase 0:  {3 ,   60 ,  130 ,   60  ,   3 },
  { 3 ,   57 ,   /*130,*/    63 ,   4 },
  { 2 ,   54 ,   /*130,*/    66 ,   4 },
  { 2 ,   51 ,   /*129,*/    69 ,   5 },
  { 2 ,   48 ,   /*128,*/    72 ,   6 },
  { 1 ,   45 ,   /*128,*/    75 ,   7 },
  { 1 ,   43 ,   /*127,*/    78 ,   7 },
  { 1 ,   40 ,   /*125,*/    81 ,   8 },
  { 1 ,   37 ,   /*124,*/    84 ,   9 },
  { 0 ,   35 ,   /*122,*/    88 ,  10 },
  { 0 ,   33 ,   /*121,*/    91 ,  12 },
  { 0 ,   30 ,   /*119,*/    94 ,  13 },
  { 0 ,   28 ,   /*117,*/    97 ,  14 },
  { 0 ,   26 ,   /*115,*/    99 ,  16 },
  { 0 ,   24 ,   /*112,*/   102 ,  17 },
  { 0 ,   22 ,   /*110,*/   105 ,  19 },
  // real phase 16:{0 ,   20 ,  108 ,  108  ,  20 },
 },
 // ratio = 4
 {{ 4 ,  62 ,               23  , 105 }, // real phase 0: {4  ,  62 ,  124 ,   62  ,   4 ,
  { 4 ,  59 ,    /*124,*/   64  ,   5 },
  { 3 ,  56 ,    /*124,*/   67  ,   6 },
  { 3 ,  53 ,    /*123,*/   70  ,   7 },
  { 2 ,  51 ,    /*123,*/   73  ,   8 },
  { 2 ,  48 ,    /*122,*/   76  ,   8 },
  { 2 ,  45 ,    /*121,*/   79  ,   9 },
  { 1 ,  43 ,    /*120,*/   81  ,  10 },
  { 1 ,  40 ,    /*119,*/   84  ,  12 },
  { 1 ,  38 ,    /*117,*/   87  ,  13 },
  { 1 ,  36 ,    /*116,*/   90  ,  14 },
  { 0 ,  34 ,    /*114,*/   92  ,  15 },
  { 0 ,  31 ,    /*113,*/   95  ,  17 },
  { 0 ,  29 ,    /*111,*/   97  ,  18 },
  { 0 ,  27 ,    /*109,*/  100  ,  20 },
  { 0 ,  25 ,    /*107,*/  102  ,  22 },
  // real phase 16: {0  ,  23 ,  105 ,  105  ,  23 },
 }
};

void nvInitScalerCoefficientsPrecomp5(NVEvoChannelPtr pChannel,
                                           NvU32 coeff, NvU32 index)
{
    nvDmaSetStartEvoMethod(pChannel, NVC57E_SET_INPUT_SCALER_COEFF_VALUE, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C57E, _SET_INPUT_SCALER_COEFF_VALUE, _DATA, coeff) |
        DRF_NUM(C57E, _SET_INPUT_SCALER_COEFF_VALUE, _INDEX, index));
}

static void InitScalerCoefficientsPostcomp5(NVDevEvoPtr pDevEvo,
                                            NVEvoChannelPtr pChannel,
                                            NvU32 coeff, NvU32 index)
{
    NvU32 h;

    for (h = 0; h < pDevEvo->numHeads; h++) {
        nvDmaSetStartEvoMethod(pChannel,
            NVC57D_HEAD_SET_OUTPUT_SCALER_COEFF_VALUE(h), 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(C57D, _HEAD_SET_OUTPUT_SCALER_COEFF_VALUE, _DATA, coeff) |
            DRF_NUM(C57D, _HEAD_SET_OUTPUT_SCALER_COEFF_VALUE, _INDEX, index));
    }
}

static void InitTaps5ScalerCoefficientsC5(NVDevEvoPtr pDevEvo,
                                          NVEvoChannelPtr pChannel,
                                          NvBool isPrecomp)
{
    NvU8 ratio;

    if (isPrecomp) {
        const NVEvoWindowCaps *pWinCaps =
            &pDevEvo->gpus[0].capabilities.window[pChannel->instance];
        const NVEvoScalerCaps *pScalerCaps = &pWinCaps->scalerCaps;

        if (!pScalerCaps->present) {
            return;
        }
    }

    for (ratio = 0; ratio < NUM_SCALER_RATIOS; ratio++) {
        NvU8 phase;
        for (phase = 0; phase < NUM_TAPS5_COEFF_PHASES; phase++) {
            NvU8 coeffIdx;
            for (coeffIdx = 0; coeffIdx < NUM_TAPS5_COEFF_VALUES; coeffIdx++) {
                NvU32 coeff = scalerTaps5Coeff[ratio][phase][coeffIdx];
                NvU32 index = ratio << 6 | phase << 2 | coeffIdx;

                if (isPrecomp) {
                    nvInitScalerCoefficientsPrecomp5(pChannel, coeff, index);
                } else {
                    InitScalerCoefficientsPostcomp5(pDevEvo,
                                                    pChannel, coeff, index);
                }
            }
        }
    }
}

/*
 * This is a 3x4 matrix with S5.14 coefficients (truncated from S5.16
 * SW-specified values).
 */
static const struct NvKmsCscMatrix Rec2020RGBToLMS = {{
    { 0x697c, 0x8620, 0x1064, 0 },
    { 0x2aa8, 0xb86c, 0x1ce8, 0 },
    {  0x62c, 0x1354, 0xe684, 0 },
}};

/*
 * This is a 3x4 matrix with S5.14 coefficients (truncated from S5.16
 * SW-specified values).
 */
static const struct NvKmsCscMatrix Rec709RGBToLMS = {{
    { 0x4bb8, 0x9f84, 0x14c8, 0 },
    { 0x27fc, 0xba2c, 0x1dd4, 0 },
    { 0x8fc,  0x2818, 0xcef0, 0 },
}};

/*
 * This is a 3x4 matrix with S5.14 coefficients (truncated from S5.16
 * SW-specified values).
 */
static const struct NvKmsCscMatrix LMSToRec709RGB = {{
    { 0x62c48,  0x1aadf4, 0x25a8,   0 },
    { 0x1ead18, 0x28f64,  0x1fc390, 0 },
    { 0x1ffd00, 0x1fbc34, 0x146c4,  0 },
}};

/*
 * This is a 3x4 matrix with S5.14 coefficients (truncated from S5.16
 * SW-specified values).
 */
static const struct NvKmsCscMatrix LMSToRec2020RGB = {{
    { 0x36fc0,  0x1d7e54, 0x11e0,   0 },
    { 0x1f3584, 0x1fbc8,  0x1fcebc, 0 },
    { 0x1ff964, 0x1fe6a4, 0x11ff4,  0 },
}};

/*
 * This is a 3x4 matrix with S5.14 coefficients (truncated from S5.16
 * SW-specified values).
 */
static const struct NvKmsCscMatrix LMSToICtCp = {{
    { 0x460d0, 0x1bc120, 0x1fde10, 0x8000 },
    { 0x8000,  0x8000,   0,        0      },
    { 0x19d20, 0x1cad30, 0x1b5b0,  0x8000 },
}};

/*
 * This is a 3x4 matrix with S5.14 coefficients (truncated from S5.16
 * SW-specified values).
 */
static const struct NvKmsCscMatrix ICtCpToLMS = {{
    { 0x1c6c,   0x10000, 0x234,    0x1ff0b0 },
    { 0x1fe394, 0x10000, 0x1ffdcc, 0xf50    },
    { 0x1fadec, 0x10000, 0x8f5e,   0x1fe15b },
}};

/*
 * The two arrays below specify the PQ OETF transfer function that's used to
 * convert from linear LMS FP16 to PQ encoded L'M'S' fixed-point.
 */
static const NvU32 OetfPQ512SegSizesLog2[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 2, 2, 3, 3,
    3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5,
    5,
};

static const NvU16 OetfPQ512Entries[] = {
    0x0000, 0x000C, 0x0014, 0x001C, 0x0028, 0x003C, 0x005C, 0x008C, 0x00D0, 0x0134, 0x0184, 0x01C8, 0x0238, 0x029C, 0x033C, 0x03C4,
    0x043C, 0x04A4, 0x0504, 0x0560, 0x0600, 0x0690, 0x0714, 0x078C, 0x07FC, 0x0864, 0x08C8, 0x0924, 0x0980, 0x09D4, 0x0A24, 0x0A70,
    0x0B04, 0x0B90, 0x0C10, 0x0C88, 0x0CFC, 0x0D68, 0x0DD4, 0x0E38, 0x0EF4, 0x0FA4, 0x1048, 0x10E4, 0x1174, 0x1200, 0x1284, 0x1304,
    0x13F4, 0x14D0, 0x159C, 0x165C, 0x1714, 0x17C0, 0x1864, 0x1900, 0x1A28, 0x1B34, 0x1C30, 0x1D1C, 0x1DFC, 0x1ECC, 0x1F94, 0x2050,
    0x2104, 0x21B0, 0x2258, 0x22F8, 0x2390, 0x2424, 0x24B4, 0x2540, 0x25C4, 0x2648, 0x26C4, 0x2740, 0x27B8, 0x282C, 0x289C, 0x290C,
    0x29E0, 0x2AAC, 0x2B70, 0x2C2C, 0x2CE0, 0x2D90, 0x2E38, 0x2ED8, 0x2F74, 0x300C, 0x30A0, 0x3130, 0x31BC, 0x3244, 0x32C8, 0x3348,
    0x3440, 0x352C, 0x360C, 0x36E4, 0x37B4, 0x387C, 0x393C, 0x39F8, 0x3AA8, 0x3B58, 0x3C00, 0x3CA4, 0x3D44, 0x3DDC, 0x3E74, 0x3F04,
    0x401C, 0x4128, 0x4228, 0x431C, 0x4408, 0x44E8, 0x45C4, 0x4694, 0x475C, 0x4820, 0x48DC, 0x4994, 0x4A48, 0x4AF4, 0x4B9C, 0x4C3C,
    0x4D78, 0x4EA0, 0x4FBC, 0x50CC, 0x51D0, 0x52CC, 0x53BC, 0x54A0, 0x5580, 0x5658, 0x5728, 0x57F0, 0x58B4, 0x5974, 0x5A2C, 0x5ADC,
    0x5C34, 0x5D7C, 0x5EB4, 0x5FDC, 0x60F4, 0x6204, 0x630C, 0x6404, 0x64F8, 0x65E0, 0x66C4, 0x679C, 0x6870, 0x693C, 0x6A04, 0x6AC4,
    0x6C38, 0x6D94, 0x6EE4, 0x7020, 0x7150, 0x7274, 0x738C, 0x7498, 0x7598, 0x7694, 0x7784, 0x786C, 0x794C, 0x7A24, 0x7AF8, 0x7BC4,
    0x7D50, 0x7EC4, 0x8024, 0x8174, 0x82B4, 0x83E8, 0x850C, 0x8628, 0x8738, 0x883C, 0x8938, 0x8A2C, 0x8B18, 0x8BFC, 0x8CD8, 0x8DB0,
    0x8F4C, 0x90D0, 0x9240, 0x939C, 0x94EC, 0x962C, 0x975C, 0x9880, 0x999C, 0x9AAC, 0x9BB0, 0x9CAC, 0x9DA0, 0x9E8C, 0x9F70, 0xA04C,
    0xA1F4, 0xA384, 0xA500, 0xA664, 0xA7BC, 0xA904, 0xAA3C, 0xAB6C, 0xAC8C, 0xADA0, 0xAEAC, 0xAFAC, 0xB0A4, 0xB194, 0xB27C, 0xB360,
    0xB510, 0xB6A4, 0xB824, 0xB994, 0xBAF0, 0xBC3C, 0xBD78, 0xBEA8, 0xBFCC, 0xC0E4, 0xC1F0, 0xC2F4, 0xC3F0, 0xC4E4, 0xC5CC, 0xC6B0,
    0xC78C, 0xC860, 0xC930, 0xC9F8, 0xCABC, 0xCB7C, 0xCC38, 0xCCEC, 0xCD9C, 0xCE48, 0xCEF0, 0xCF94, 0xD034, 0xD0D4, 0xD16C, 0xD200,
    0xD294, 0xD324, 0xD3B4, 0xD43C, 0xD4C4, 0xD54C, 0xD5CC, 0xD650, 0xD6CC, 0xD748, 0xD7C4, 0xD83C, 0xD8B0, 0xD924, 0xD994, 0xDA08,
    0xDAE0, 0xDBB4, 0xDC84, 0xDD4C, 0xDE10, 0xDECC, 0xDF84, 0xE038, 0xE0E8, 0xE194, 0xE238, 0xE2DC, 0xE37C, 0xE418, 0xE4B0, 0xE544,
    0xE5D4, 0xE664, 0xE6F0, 0xE778, 0xE800, 0xE884, 0xE904, 0xE984, 0xEA00, 0xEA7C, 0xEAF4, 0xEB68, 0xEBDC, 0xEC50, 0xECC0, 0xED30,
    0xEE08, 0xEED8, 0xEFA4, 0xF068, 0xF128, 0xF1E4, 0xF298, 0xF348, 0xF3F4, 0xF49C, 0xF540, 0xF5E0, 0xF67C, 0xF714, 0xF7A8, 0xF83C,
    0xF8CC, 0xF958, 0xF9E0, 0xFA68, 0xFAEC, 0xFB6C, 0xFBE8, 0xFC64, 0xFCE0, 0xFD58, 0xFDCC, 0xFE40, 0xFEB4, 0xFF24, 0xFF90, 0xFFFC,
};

/*
 * The two arrays below specify the PQ EOTF transfer function that's used to
 * convert from PQ encoded L'M'S' fixed-point to linear LMS FP16. This transfer
 * function is the inverse of the OETF curve.
 */
static const NvU32 EotfPQ512SegSizesLog2[] = {
    6, 6, 4, 4, 4, 3, 4, 3, 3, 3, 2, 2, 2, 3, 3, 2,
    2, 2, 2, 2, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    6, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 2, 2, 1, 2,
    2, 1, 1, 2, 2, 2, 2, 1, 2, 1, 1, 2, 1, 4, 2, 2,
};

static const NvU16 EotfPQ512Entries[] = {
    0x0000, 0x0001, 0x0003, 0x0005, 0x0008, 0x000C, 0x0011, 0x0016, 0x001B, 0x0022, 0x0028, 0x002F, 0x0037, 0x003F, 0x0048, 0x0051,
    0x005A, 0x0064, 0x006F, 0x007A, 0x0085, 0x0091, 0x009E, 0x00AB, 0x00B8, 0x00C6, 0x00D4, 0x00E3, 0x00F3, 0x0102, 0x0113, 0x0123,
    0x0135, 0x0146, 0x0158, 0x016B, 0x017E, 0x0192, 0x01A6, 0x01BB, 0x01D0, 0x01E5, 0x01FC, 0x0212, 0x0229, 0x0241, 0x0259, 0x0272,
    0x028B, 0x02A4, 0x02BE, 0x02D9, 0x02F4, 0x0310, 0x032C, 0x0349, 0x0366, 0x0384, 0x03A2, 0x03C1, 0x03E0, 0x0400, 0x0421, 0x0442,
    0x0463, 0x0485, 0x04A8, 0x04CB, 0x04EF, 0x0513, 0x0538, 0x055D, 0x0583, 0x05AA, 0x05D1, 0x05F9, 0x0621, 0x064A, 0x0673, 0x069D,
    0x06C7, 0x06F3, 0x071E, 0x074B, 0x0777, 0x07A5, 0x07D3, 0x0801, 0x0819, 0x0830, 0x0849, 0x0861, 0x087A, 0x0893, 0x08AD, 0x08C7,
    0x08E1, 0x08FB, 0x0916, 0x0931, 0x094C, 0x0968, 0x0984, 0x09A0, 0x09BD, 0x09DA, 0x09F7, 0x0A15, 0x0A33, 0x0A51, 0x0A70, 0x0A8F,
    0x0AAE, 0x0ACE, 0x0AEE, 0x0B0E, 0x0B2F, 0x0B50, 0x0B71, 0x0B93, 0x0BB5, 0x0BD7, 0x0BFA, 0x0C0F, 0x0C20, 0x0C32, 0x0C44, 0x0C56,
    0x0C69, 0x0CB5, 0x0D03, 0x0D55, 0x0DA9, 0x0E01, 0x0E5B, 0x0EB9, 0x0F1B, 0x0F7F, 0x0FE7, 0x1029, 0x1061, 0x109A, 0x10D5, 0x1111,
    0x1150, 0x1190, 0x11D3, 0x1217, 0x125E, 0x12A6, 0x12F0, 0x133D, 0x138B, 0x13DC, 0x1417, 0x1442, 0x146D, 0x149A, 0x14C8, 0x14F7,
    0x1527, 0x1558, 0x158B, 0x15BF, 0x15F4, 0x162A, 0x1662, 0x169B, 0x16D5, 0x1711, 0x174E, 0x178C, 0x17CC, 0x1806, 0x1828, 0x184A,
    0x186D, 0x18B4, 0x18FF, 0x194D, 0x199E, 0x19F3, 0x1A4B, 0x1AA7, 0x1B06, 0x1B37, 0x1B69, 0x1B9B, 0x1BCF, 0x1C02, 0x1C1D, 0x1C38,
    0x1C54, 0x1C70, 0x1C8D, 0x1CAB, 0x1CC9, 0x1CE7, 0x1D06, 0x1D26, 0x1D46, 0x1D88, 0x1DCC, 0x1E13, 0x1E5C, 0x1EA8, 0x1EF6, 0x1F47,
    0x1F9A, 0x1FF1, 0x2025, 0x2053, 0x2082, 0x20B3, 0x20E6, 0x211A, 0x214F, 0x2187, 0x21C0, 0x21FA, 0x2237, 0x2275, 0x22B5, 0x22F7,
    0x233B, 0x23C9, 0x2430, 0x247F, 0x24D3, 0x252B, 0x2589, 0x25EB, 0x2653, 0x26C1, 0x2734, 0x27AD, 0x2817, 0x2838, 0x285A, 0x287C,
    0x28A0, 0x28C5, 0x28EA, 0x2911, 0x2938, 0x2960, 0x298A, 0x29B4, 0x29DF, 0x2A0C, 0x2A39, 0x2A68, 0x2A98, 0x2AFA, 0x2B62, 0x2BCE,
    0x2C20, 0x2C5B, 0x2C99, 0x2CDA, 0x2D1E, 0x2D65, 0x2DB0, 0x2DFD, 0x2E4E, 0x2EA3, 0x2EFC, 0x2F58, 0x2FB8, 0x300E, 0x3043, 0x307A,
    0x30B3, 0x30D0, 0x30EE, 0x310D, 0x312C, 0x314C, 0x316D, 0x318E, 0x31B0, 0x31D3, 0x31F6, 0x321A, 0x323F, 0x3265, 0x328B, 0x32B2,
    0x32DA, 0x332D, 0x3383, 0x33DC, 0x341D, 0x344D, 0x347F, 0x34B4, 0x34EA, 0x3523, 0x355E, 0x359B, 0x35DB, 0x361D, 0x3662, 0x36A9,
    0x36F3, 0x3740, 0x3791, 0x37E4, 0x381D, 0x384A, 0x3879, 0x38A9, 0x38DB, 0x3910, 0x3946, 0x397E, 0x39B8, 0x39F5, 0x3A34, 0x3A75,
    0x3AB9, 0x3AFF, 0x3B48, 0x3B94, 0x3BE2, 0x3C1A, 0x3C44, 0x3C70, 0x3C9D, 0x3CA0, 0x3CA3, 0x3CA6, 0x3CA9, 0x3CAC, 0x3CAF, 0x3CB1,
    0x3CB4, 0x3CB7, 0x3CBA, 0x3CBD, 0x3CC0, 0x3CC3, 0x3CC6, 0x3CC9, 0x3CCC, 0x3CCF, 0x3CD2, 0x3CD5, 0x3CD8, 0x3CDB, 0x3CDE, 0x3CE1,
    0x3CE4, 0x3CE7, 0x3CEA, 0x3CEE, 0x3CF1, 0x3CF4, 0x3CF7, 0x3CFA, 0x3CFD, 0x3D00, 0x3D03, 0x3D06, 0x3D09, 0x3D0D, 0x3D10, 0x3D13,
    0x3D16, 0x3D19, 0x3D1C, 0x3D20, 0x3D23, 0x3D26, 0x3D29, 0x3D2C, 0x3D30, 0x3D33, 0x3D36, 0x3D39, 0x3D3D, 0x3D40, 0x3D43, 0x3D46,
    0x3D4A, 0x3D4D, 0x3D50, 0x3D54, 0x3D57, 0x3D5A, 0x3D5D, 0x3D61, 0x3D64, 0x3D9B, 0x3DD3, 0x3E0D, 0x3E4A, 0x3E89, 0x3ECA, 0x3F0E,
    0x3F54, 0x3F9C, 0x3FE8, 0x401B, 0x4043, 0x406D, 0x4099, 0x40C6, 0x40F4, 0x4124, 0x4156, 0x418A, 0x41C0, 0x41F8, 0x4232, 0x426D,
    0x42AB, 0x42EB, 0x432E, 0x4373, 0x43BA, 0x4428, 0x4479, 0x44D0, 0x452D, 0x4591, 0x45FC, 0x466F, 0x46EB, 0x472C, 0x476F, 0x47B5,
    0x47FE, 0x4824, 0x484B, 0x4874, 0x489D, 0x48F5, 0x4954, 0x4986, 0x49B9, 0x49EF, 0x4A26, 0x4A5F, 0x4A9B, 0x4AD9, 0x4B19, 0x4B9F,
    0x4C18, 0x4C66, 0x4CBA, 0x4CE6, 0x4D13, 0x4D43, 0x4D74, 0x4DA7, 0x4DDC, 0x4E12, 0x4E4B, 0x4E86, 0x4EC3, 0x4F02, 0x4F44, 0x4F88,
    0x4FCE, 0x500C, 0x5032, 0x5082, 0x50D8, 0x5106, 0x5135, 0x5166, 0x5199, 0x5205, 0x5278, 0x52F5, 0x537C, 0x53C3, 0x5406, 0x542D,
    0x5454, 0x54A9, 0x5503, 0x550F, 0x551B, 0x5527, 0x5533, 0x5540, 0x554C, 0x5559, 0x5565, 0x5572, 0x557F, 0x558C, 0x5599, 0x55A7,
    0x55B4, 0x55C1, 0x55CF, 0x5607, 0x5641, 0x567E, 0x56BC, 0x56FE, 0x5741, 0x5788, 0x57D1,
};

#define TMO_LUT_NUM_SEGMENTS 64
#define TMO_LUT_SEG_SIZE_LOG2 4
#define TMO_LUT_NUM_ENTRIES 1024

struct TmoLutSettings
{
    NvU32 satMode;
    NvU32 lowIntensityZoneEnd;
    NvU32 lowIntensityValueLinWeight;
    NvU32 lowIntensityValueNonLinWeight;
    NvU32 lowIntensityValueThreshold;

    NvU32 medIntensityZoneStart;
    NvU32 medIntensityZoneEnd;
    NvU32 medIntensityValueLinWeight;
    NvU32 medIntensityValueNonLinWeight;
    NvU32 medIntensityValueThreshold;

    NvU32 highIntensityZoneStart;
    NvU32 highIntensityValueLinWeight;
    NvU32 highIntensityValueNonLinWeight;
    NvU32 highIntensityValueThreshold;
};

// No color correction.
static const struct TmoLutSettings TMO_LUT_SETTINGS_NO_CORRECTION = { 2, 1280, 256, 256, 255, 4960,  4961, 256, 256, 255, 10640, 256, 256, 255 };

static void InitCsc0LUT(NVEvoChannelPtr pChannel,
                        const NvU32 *pSegmentSizes, NvU32 numSegmentSizes,
                        const NvU16 *pLUTEntries, NvU32 numEntries)
{
    NvU32 i;

    for (i = 0; i < numSegmentSizes; i++) {
        nvDmaSetStartEvoMethod(pChannel, NVC57E_SET_CSC0LUT_SEGMENT_SIZE, 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(C57E, _SET_CSC0LUT_SEGMENT_SIZE, _IDX, i) |
            DRF_NUM(C57E, _SET_CSC0LUT_SEGMENT_SIZE, _VALUE, pSegmentSizes[i]));
    }

    for (i = 0; i < numEntries; i++) {
        nvDmaSetStartEvoMethod(pChannel, NVC57E_SET_CSC0LUT_ENTRY, 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(C57E, _SET_CSC0LUT_ENTRY, _IDX, i) |
            DRF_NUM(C57E, _SET_CSC0LUT_ENTRY, _VALUE, pLUTEntries[i]));
    }
}

static void InitCsc1LUT(NVEvoChannelPtr pChannel,
                        const NvU32 *pSegmentSizes, NvU32 numSegmentSizes,
                        const NvU16 *pLUTEntries, NvU32 numEntries)
{
    NvU32 i;

    for (i = 0; i < numSegmentSizes; i++) {
        nvDmaSetStartEvoMethod(pChannel, NVC57E_SET_CSC1LUT_SEGMENT_SIZE, 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(C57E, _SET_CSC1LUT_SEGMENT_SIZE, _IDX, i) |
            DRF_NUM(C57E, _SET_CSC1LUT_SEGMENT_SIZE, _VALUE, pSegmentSizes[i]));
    }

    for (i = 0; i < numEntries; i++) {
        nvDmaSetStartEvoMethod(pChannel, NVC57E_SET_CSC1LUT_ENTRY, 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(C57E, _SET_CSC1LUT_ENTRY, _IDX, i) |
            DRF_NUM(C57E, _SET_CSC1LUT_ENTRY, _VALUE, pLUTEntries[i]));
    }
}

static void ConfigureCsc0C5(NVDevEvoPtr pDevEvo,
                            NVEvoChannelPtr pChannel,
                            const NVFlipChannelEvoHwState *pHwState,
                            NvBool enable)
{
    NVEvoWindowCaps *pWinCaps =
        &pDevEvo->gpus[0].capabilities.window[pChannel->instance];
    struct NvKmsCscMatrix csc00Matrix = { };
    struct NvKmsCscMatrix csc01Matrix = { };
    NvU32 lutData = 0;

    if (!pWinCaps->csc0MatricesPresent) {
        return;
    }

    nvAssert(pWinCaps->cscLUTsPresent);

    if (enable) {
        /* Linear RGB FP16 -> Linear LMS FP16 */
        if (pHwState->colorSpace == NVKMS_INPUT_COLOR_SPACE_BT2100) {
            csc00Matrix = Rec2020RGBToLMS;
        } else {
            csc00Matrix = Rec709RGBToLMS;
        }

        /* Linear LMS FP16 -> PQ encoded L'M'S' fixed-point */
        lutData = DRF_DEF(C57E, _SET_CSC0LUT_CONTROL, _INTERPOLATE, _ENABLE) |
                  DRF_DEF(C57E, _SET_CSC0LUT_CONTROL, _MIRROR, _DISABLE) |
                  DRF_DEF(C57E, _SET_CSC0LUT_CONTROL, _ENABLE, _ENABLE);

        /* PQ encoded L'M'S' fixed-point -> ICtCp */
        csc01Matrix = LMSToICtCp;
    } else {
        csc00Matrix = NVKMS_IDENTITY_CSC_MATRIX;

        lutData = DRF_DEF(C57E, _SET_CSC0LUT_CONTROL, _INTERPOLATE, _DISABLE) |
                  DRF_DEF(C57E, _SET_CSC0LUT_CONTROL, _MIRROR, _DISABLE) |
                  DRF_DEF(C57E, _SET_CSC0LUT_CONTROL, _ENABLE, _DISABLE);

        csc01Matrix = NVKMS_IDENTITY_CSC_MATRIX;
    }

    if (pHwState->csc00Override.enabled) {
        csc00Matrix = pHwState->csc00Override.matrix;
    }

    if (pHwState->csc01Override.enabled) {
        csc01Matrix = pHwState->csc01Override.matrix;
    }

    /* CSC0LUT must be enabled if CSC01 or CSC10 is in use. */
    if ((pHwState->csc01Override.enabled &&
         !nvIsCscMatrixIdentity(&pHwState->csc01Override.matrix)) ||
        (pHwState->csc10Override.enabled &&
         !nvIsCscMatrixIdentity(&pHwState->csc10Override.matrix))) {

        lutData = DRF_DEF(C57E, _SET_CSC0LUT_CONTROL, _INTERPOLATE, _ENABLE) |
                  DRF_DEF(C57E, _SET_CSC0LUT_CONTROL, _MIRROR, _DISABLE) |
                  DRF_DEF(C57E, _SET_CSC0LUT_CONTROL, _ENABLE, _ENABLE);
    }

    SetCsc00MatrixC5(pChannel, &csc00Matrix);

    nvDmaSetStartEvoMethod(pChannel, NVC57E_SET_CSC0LUT_CONTROL, 1);
    nvDmaSetEvoMethodData(pChannel, lutData);

    SetCsc01MatrixC5(pChannel, &csc01Matrix);
}

static inline float64_t maxF(float64_t a, float64_t b)
{
    return f64_lt(a, b) ? b : a;
}

static inline float64_t clampF(float64_t value, float64_t min, float64_t max)
{
    value = maxF(value, min);
    value = f64_lt(max, value) ? max : value;
    return value;
}

static float64_t PQEotf(float64_t colorValue, NvBool inverse)
{
    const float64_t zero  = {0x0000000000000000}; // 0.0
    const float64_t one   = {0x3FF0000000000000}; // 1.0
    const float64_t m1    = {0x3FC463FFFFFFB9A2}; // 0.1593017578125
    const float64_t m2    = {0x4053B60000000000}; // 78.84375
    const float64_t c1    = {0x3FEAC00000000000}; // 0.8359375
    const float64_t c2    = {0x4032DA0000000000}; // 18.8515625
    const float64_t c3    = {0x4032B00000000000}; // 18.6875

    const float64_t invm1 = {0x40191C0D56E72ABA}; // 1/m1 = 6.27739463602
    const float64_t invm2 = {0x3F89F9B585D7C997}; // 1/m2 = 0.01268331351

    if (inverse) {
        // Convert from linear to PQ-encoded values.
        float64_t L = clampF(colorValue, zero, one);
        float64_t powLm1 = nvKmsPow(L, m1);
        float64_t N = nvKmsPow(f64_div(f64_add(c1,  f64_mul(c2, powLm1)),
                                       f64_add(one, f64_mul(c3, powLm1))), m2);

        return clampF(N, zero, one);
    } else {
        // Convert from PQ-encoded values to linear values.
        float64_t N = clampF(colorValue, zero, one);
        float64_t powNinvM2 = nvKmsPow(N, invm2);
        float64_t L = nvKmsPow(f64_div(maxF(f64_sub(powNinvM2, c1), zero),
                                       f64_sub(c2, f64_mul(c3, powNinvM2))),
                               invm1);

        return clampF(L, zero, one);
    }
}

// Hermite spline
static float64_t P(float64_t B, float64_t KS, float64_t maxLum)
{
    const float64_t one    = {0x3FF0000000000000}; // 1.0
    const float64_t two    = {0x4000000000000000}; // 2.0
    const float64_t negtwo = {0xC000000000000000}; // -2.0
    const float64_t three  = {0x4008000000000000}; // 3.0

    float64_t t  = f64_div(f64_sub(B, KS), f64_sub(one, KS));
    float64_t t2 = f64_mul(t, t);
    float64_t t3 = f64_mul(t2, t);

    return
        f64_add(f64_add(
            f64_mul(f64_add(f64_sub(f64_mul(two, t3), f64_mul(three, t2)), one), KS),
            f64_mul(f64_add(f64_sub(t3, f64_mul(two, t2)), t), f64_sub(one, KS))),
            f64_mul(f64_add(f64_mul(negtwo, t3), f64_mul(three, t2)), maxLum));
}

/*
 * PQ tone mapping operator with no remapping of blacks or "toe" section of
 * curve. Messing with nonlinearity and remapping in the SDR portion of the
 * curve results in bad looking PC desktop and game content.
 *
 * Lmax        = InvPQEotf(targetMaxLum/10000.0)
 * Lw          = InvPQEotf(srcMaxLum/10000.0)
 * maxLumRatio = Lmax/Lw
 * KS          = 1.5*maxLumRatio - 0.5
 * KSEqualsOne = (KS == 1.0)
 *
 * XXX HDR TODO: Remap blacks and implement toe section for video content?
 */
static NvU16 TmoLutEntry(NvU32 i,
                         const float64_t Lmax,
                         const float64_t Lw,
                         const float64_t maxLumRatio,
                         const float64_t KS,
                         const NvBool KSEqualsOne)
{
    const float64_t zero         = {0x0000000000000000}; // 0.0
    const float64_t maxIntensity = {0x40CFFF8000000000}; // 16383.0

    float64_t outputF;
    float64_t inputF =
        f64_div(ui32_to_f64(i), ui32_to_f64(TMO_LUT_NUM_ENTRIES - 1));

    float64_t E1;
    float64_t E2;

    E1 = f64_div(inputF, Lw);

    if (KSEqualsOne || f64_lt(E1, KS)) {
        E2 = E1;
    } else {
        E2 = P(E1, KS, maxLumRatio);
    }

    outputF = clampF(f64_mul(E2, Lw), zero, Lmax);

    return (NvU16) f64_to_ui32(clampF(f64_mul(outputF, maxIntensity),
                                      zero, maxIntensity),
                               softfloat_round_near_even, FALSE) << 2;
}

static void InitializeTmoLut(const NVEvoChannelPtr pChannel,
                             NVSurfaceEvoPtr pLutSurfaceEvo,
                             NvU32 sd)
{
    NVEvoLutDataRec *pData = pLutSurfaceEvo->cpuAddress[sd];
    NvU64 vssHead = 0;
    NvU32 lutEntryCounter = 0, i;

    // Precalculate constants for TmoLutEntry().
    const float64_t tenThousand = {0x40C3880000000000}; // 10000.0
    const float64_t one         = {0x3FF0000000000000}; // 1.0
    const float64_t half        = {0x3FE0000000000000}; // 0.5
    const float64_t oneHalf     = {0x3FF8000000000000}; // 1.5
    // Lmax = InvPQEotf(targetMaxLum/10,000)
    const float64_t Lmax =
        PQEotf(f64_div(ui32_to_f64(pChannel->tmoParams.targetMaxLums[sd]),
                       tenThousand), TRUE);
    // Lw = InvPQEotf(srcMaxLum/10,000)
    const float64_t Lw =
        PQEotf(f64_div(ui32_to_f64(pChannel->tmoParams.srcMaxLum),
                       tenThousand), TRUE);
    // maxLumRatio = Lmax/Lw
    const float64_t maxLumRatio = f64_div(Lmax, Lw);
    // KS = 1.5*maxLumRatio - 0.5
    const float64_t KS = f64_sub(f64_mul(oneHalf, maxLumRatio), half);
    // KSEqualsOne = (KS == 1.0)
    const NvBool KSEqualsOne = f64_eq(KS, one);

    nvAssert(pChannel->tmoParams.srcMaxLum >=
             pChannel->tmoParams.targetMaxLums[sd]);

    // VSS Header
    for (lutEntryCounter = 0; lutEntryCounter < NV_LUT_VSS_HEADER_SIZE; lutEntryCounter++) {
        vssHead = 0;
        for (i = 0; ((i < 16) && (((lutEntryCounter * 16) + i) < TMO_LUT_NUM_SEGMENTS)); i++) {
            NvU64 temp = TMO_LUT_SEG_SIZE_LOG2;
            vssHead |= temp << (i * 3);
        }
        nvkms_memcpy(&(pData->base[lutEntryCounter]), &vssHead, sizeof(NVEvoLutEntryRec));
    }

    for (i = 0; i < TMO_LUT_NUM_ENTRIES; i++) {
        pData->base[i + NV_LUT_VSS_HEADER_SIZE].Red =
        pData->base[i + NV_LUT_VSS_HEADER_SIZE].Green =
        pData->base[i + NV_LUT_VSS_HEADER_SIZE].Blue =
            TmoLutEntry(i, Lmax, Lw, maxLumRatio, KS, KSEqualsOne);
    }

    // Copy the last entry for interpolation
    pData->base[TMO_LUT_NUM_ENTRIES + NV_LUT_VSS_HEADER_SIZE].Red =
        pData->base[TMO_LUT_NUM_ENTRIES + NV_LUT_VSS_HEADER_SIZE - 1].Red;
    pData->base[TMO_LUT_NUM_ENTRIES + NV_LUT_VSS_HEADER_SIZE].Blue =
        pData->base[TMO_LUT_NUM_ENTRIES + NV_LUT_VSS_HEADER_SIZE - 1].Blue;
    pData->base[TMO_LUT_NUM_ENTRIES + NV_LUT_VSS_HEADER_SIZE].Green =
        pData->base[TMO_LUT_NUM_ENTRIES + NV_LUT_VSS_HEADER_SIZE - 1].Green;
}

static NvBool UpdateTmoParams(NVEvoChannelPtr pChannel,
                              NvBool enabled,
                              NvU32 srcMaxLum,
                              const NvU32 targetMaxLums[NVKMS_MAX_SUBDEVICES])
{
    NvU16 sd;
    NvBool dirty = FALSE;

    if (pChannel->tmoParams.clientSpecified) {
        pChannel->tmoParams.clientSpecified = FALSE;
        dirty = TRUE;
    }

    if (pChannel->tmoParams.enabled != enabled) {
        pChannel->tmoParams.enabled = enabled;
        dirty = TRUE;
    }

    if (pChannel->tmoParams.srcMaxLum != srcMaxLum) {
        pChannel->tmoParams.srcMaxLum = srcMaxLum;
        dirty = TRUE;
    }

    for (sd = 0; sd < NVKMS_MAX_SUBDEVICES; sd++) {
        if (pChannel->tmoParams.targetMaxLums[sd] != targetMaxLums[sd]) {
            pChannel->tmoParams.targetMaxLums[sd] = targetMaxLums[sd];
            dirty = TRUE;
        }
    }

    return dirty;
}

static void EvoSetTmoLutSurfaceAddressC5(
    const NVDevEvoRec *pDevEvo,
    NVEvoChannelPtr pChannel,
    const NVSurfaceDescriptor *pSurfaceDesc,
    NvU32 offset)
{
    NvU32 ctxDmaHandle = pSurfaceDesc ? pSurfaceDesc->ctxDmaHandle : 0;

    nvDmaSetStartEvoMethod(pChannel, NVC57E_SET_CONTEXT_DMA_TMO_LUT, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C57E, _SET_CONTEXT_DMA_TMO_LUT, _HANDLE, ctxDmaHandle));

    nvDmaSetStartEvoMethod(pChannel, NVC57E_SET_OFFSET_TMO_LUT, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C57E, _SET_OFFSET_TMO_LUT, _ORIGIN, offset >> 8));
}

static void ConfigureTmoLut(NVDevEvoPtr pDevEvo,
                            const NVFlipChannelEvoHwState *pHwState,
                            NVEvoChannelPtr pChannel)
{
    NvU16 sd;
    const NvU32 win = NV_EVO_CHANNEL_MASK_WINDOW_NUMBER(pChannel->channelMask);
    const NvU32 head = pDevEvo->headForWindow[win];
    NvU32 offset = offsetof(NVEvoLutDataRec, base);
    NvU32 lutSize = NV_LUT_VSS_HEADER_SIZE + TMO_LUT_NUM_ENTRIES + 1;
    const struct TmoLutSettings *tmoLutSettings = &TMO_LUT_SETTINGS_NO_CORRECTION;
    NvBool enableLut = FALSE;

    if (pHwState->tmoLut.fromOverride) {
        enableLut = (pHwState->tmoLut.pLutSurfaceEvo != NULL);
        pChannel->tmoParams.clientSpecified = TRUE;
        offset = pHwState->tmoLut.offset;
        lutSize = NV_LUT_VSS_HEADER_SIZE + pHwState->tmoLut.lutEntries;
    } else {
        NvBool needsTmoLut = FALSE;
        const NvU32 srcMaxLum = nvGetHDRSrcMaxLum(pHwState);
        NvU32 targetMaxLums[NVKMS_MAX_SUBDEVICES] = {0};
        for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
            const NVDispHeadStateEvoRec *pHeadState =
                &pDevEvo->pDispEvo[sd]->headState[head];

            targetMaxLums[sd] = pHeadState->hdrInfoFrame.staticMetadata.maxCLL;

            // If any head needs tone mapping, enable TMO for channel
            if (nvNeedsTmoLut(pDevEvo, pChannel, pHwState,
                              srcMaxLum, targetMaxLums[sd])) {
                needsTmoLut = TRUE;
            }
        }

        if (!UpdateTmoParams(pChannel, needsTmoLut, srcMaxLum, targetMaxLums)) {
            // No change in parameters, no need to reconfigure.
            return;
        }
        enableLut = pChannel->tmoParams.enabled;

        if (enableLut) {
            // Initialize TMO LUT on all subdevices
            for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
                nvAssert(pHwState->tmoLut.pLutSurfaceEvo != NULL);
                InitializeTmoLut(pChannel, pHwState->tmoLut.pLutSurfaceEvo, sd);
            }
        }
    }

    if (!enableLut) {
        pDevEvo->hal->SetTmoLutSurfaceAddress(pDevEvo, pChannel,
            NULL /* pSurfaceDesc */, 0 /* offset */);
        return;
    }

    nvDmaSetStartEvoMethod(pChannel, NVC57E_SET_TMO_CONTROL, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C57E, _SET_TMO_CONTROL, _SIZE, lutSize) |
        DRF_DEF(C57E, _SET_TMO_CONTROL, _INTERPOLATE, _ENABLE)    |
        DRF_NUM(C57E, _SET_TMO_CONTROL, _SAT_MODE, tmoLutSettings->satMode));

    pDevEvo->hal->SetTmoLutSurfaceAddress(pDevEvo, pChannel,
        &pHwState->tmoLut.pLutSurfaceEvo->planes[0].surfaceDesc, offset);

    // Low Intensity

    nvDmaSetStartEvoMethod(pChannel, NVC57E_SET_TMO_LOW_INTENSITY_ZONE, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C57E, _SET_TMO_LOW_INTENSITY_ZONE, _END,
                tmoLutSettings->lowIntensityZoneEnd));

    nvDmaSetStartEvoMethod(pChannel, NVC57E_SET_TMO_LOW_INTENSITY_VALUE, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C57E, _SET_TMO_LOW_INTENSITY_VALUE, _LIN_WEIGHT,
                tmoLutSettings->lowIntensityValueLinWeight)           |
        DRF_NUM(C57E, _SET_TMO_LOW_INTENSITY_VALUE, _NON_LIN_WEIGHT,
                tmoLutSettings->lowIntensityValueNonLinWeight)        |
        DRF_NUM(C57E, _SET_TMO_LOW_INTENSITY_VALUE, _THRESHOLD,
                tmoLutSettings->lowIntensityValueThreshold));

    // Medium Intensity

    nvDmaSetStartEvoMethod(pChannel, NVC57E_SET_TMO_MEDIUM_INTENSITY_ZONE, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C57E, _SET_TMO_MEDIUM_INTENSITY_ZONE, _START,
                tmoLutSettings->medIntensityZoneStart)         |
        DRF_NUM(C57E, _SET_TMO_MEDIUM_INTENSITY_ZONE, _END,
                tmoLutSettings->medIntensityZoneEnd));

    nvDmaSetStartEvoMethod(pChannel, NVC57E_SET_TMO_MEDIUM_INTENSITY_VALUE, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C57E, _SET_TMO_MEDIUM_INTENSITY_VALUE, _LIN_WEIGHT,
                tmoLutSettings->medIntensityValueLinWeight)              |
        DRF_NUM(C57E, _SET_TMO_MEDIUM_INTENSITY_VALUE, _NON_LIN_WEIGHT,
                tmoLutSettings->medIntensityValueNonLinWeight)           |
        DRF_NUM(C57E, _SET_TMO_MEDIUM_INTENSITY_VALUE, _THRESHOLD,
                tmoLutSettings->medIntensityValueThreshold));

    // High Intensity

    nvDmaSetStartEvoMethod(pChannel, NVC57E_SET_TMO_HIGH_INTENSITY_ZONE, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C57E, _SET_TMO_HIGH_INTENSITY_ZONE, _START,
                tmoLutSettings->highIntensityZoneStart));

    nvDmaSetStartEvoMethod(pChannel, NVC57E_SET_TMO_HIGH_INTENSITY_VALUE, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C57E, _SET_TMO_HIGH_INTENSITY_VALUE, _LIN_WEIGHT,
                tmoLutSettings->highIntensityValueLinWeight)           |
        DRF_NUM(C57E, _SET_TMO_HIGH_INTENSITY_VALUE, _NON_LIN_WEIGHT,
                tmoLutSettings->highIntensityValueNonLinWeight)        |
        DRF_NUM(C57E, _SET_TMO_HIGH_INTENSITY_VALUE, _THRESHOLD,
                tmoLutSettings->highIntensityValueThreshold));
}

static void ConfigureCsc1C5(NVDevEvoPtr pDevEvo,
                            NVEvoChannelPtr pChannel,
                            const NVFlipChannelEvoHwState *pHwState,
                            NvBool enable)
{
    NVEvoWindowCaps *pWinCaps =
        &pDevEvo->gpus[0].capabilities.window[pChannel->instance];
    struct NvKmsCscMatrix csc10Matrix = { };
    struct NvKmsCscMatrix csc11Matrix = { };
    NvU32 lutData = 0;
    const NvU32 win = NV_EVO_CHANNEL_MASK_WINDOW_NUMBER(pChannel->channelMask);
    const NvU32 head = pDevEvo->headForWindow[win];

    if (head == NV_INVALID_HEAD) {
        return;
    }

    if (enable) {
        const NvU32 sdMask = nvPeekEvoSubDevMask(pDevEvo);
        const NvU32 sd = (sdMask == 0) ? 0 : nv_ffs(sdMask) - 1;
        const NVDispHeadStateEvoRec *pHeadState;

        /*
         * All callers of this path should push a single sd on the stack,
         * so that ffs(sdMask) is safe.
         */
        nvAssert(nvPopCount32(sdMask) == 1);

        pHeadState = &pDevEvo->pDispEvo[sd]->headState[head];

        /* ICtCp -> PQ encoded L'M'S' fixed-point */
        csc10Matrix = ICtCpToLMS;

        /* PQ encoded L'M'S' fixed-point -> Linear LMS FP16 */
        lutData = DRF_DEF(C57E, _SET_CSC1LUT_CONTROL, _INTERPOLATE, _ENABLE) |
                  DRF_DEF(C57E, _SET_CSC1LUT_CONTROL, _MIRROR, _DISABLE) |
                  DRF_DEF(C57E, _SET_CSC1LUT_CONTROL, _ENABLE, _ENABLE);

        /* Linear LMS FP16 -> Linear RGB FP16 */
        // If postcomp is PQ, composite in Rec2020
        if (pHeadState->tf == NVKMS_OUTPUT_TF_PQ) {
            csc11Matrix = LMSToRec2020RGB;
        } else {
            csc11Matrix = LMSToRec709RGB;
        }
    } else {
        csc10Matrix = NVKMS_IDENTITY_CSC_MATRIX;

        lutData = DRF_DEF(C57E, _SET_CSC1LUT_CONTROL, _INTERPOLATE, _DISABLE) |
                  DRF_DEF(C57E, _SET_CSC1LUT_CONTROL, _MIRROR, _DISABLE) |
                  DRF_DEF(C57E, _SET_CSC1LUT_CONTROL, _ENABLE, _DISABLE);

        csc11Matrix = pHwState->cscMatrix;
    }

    if (pHwState->csc10Override.enabled) {
        csc10Matrix = pHwState->csc10Override.matrix;
    }

    if (pHwState->csc11Override.enabled) {
        csc11Matrix = pHwState->csc11Override.matrix;
    }

    nvAssert(pWinCaps->csc10MatrixPresent ||
             !pWinCaps->csc0MatricesPresent);

    if (pWinCaps->csc10MatrixPresent) {
        nvAssert(pWinCaps->cscLUTsPresent);

        /* CSC1LUT must be enabled if CSC01 or CSC10 is in use. */
        if ((pHwState->csc01Override.enabled &&
             !nvIsCscMatrixIdentity(&pHwState->csc01Override.matrix)) ||
            (pHwState->csc10Override.enabled &&
             !nvIsCscMatrixIdentity(&pHwState->csc10Override.matrix))) {
            lutData = DRF_DEF(C57E, _SET_CSC1LUT_CONTROL, _INTERPOLATE, _ENABLE) |
                      DRF_DEF(C57E, _SET_CSC1LUT_CONTROL, _MIRROR, _DISABLE) |
                      DRF_DEF(C57E, _SET_CSC1LUT_CONTROL, _ENABLE, _ENABLE);
        }

        SetCsc10MatrixC5(pChannel, &csc10Matrix);

        nvDmaSetStartEvoMethod(pChannel, NVC57E_SET_CSC1LUT_CONTROL, 1);
        nvDmaSetEvoMethodData(pChannel, lutData);
    }

    nvAssert(pWinCaps->csc11MatrixPresent);
    SetCsc11MatrixC5(pChannel, &csc11Matrix);
}

static void InitDesktopColorC3(NVDevEvoPtr pDevEvo, NVEvoChannelPtr pChannel)
{
    NvU32 head;

    for (head = 0; head < pDevEvo->numHeads; head++) {
        nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_DESKTOP_COLOR(head), 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(C37D, _HEAD_SET_DESKTOP_COLOR, _RED,   0) |
            DRF_NUM(C37D, _HEAD_SET_DESKTOP_COLOR, _GREEN, 0) |
            DRF_NUM(C37D, _HEAD_SET_DESKTOP_COLOR, _BLUE,  0) |
            DRF_NUM(C37D, _HEAD_SET_DESKTOP_COLOR, _ALPHA, 255));
    }
}

static void InitDesktopColorC5(NVDevEvoPtr pDevEvo, NVEvoChannelPtr pChannel)
{
    NvU32 head;

    for (head = 0; head < pDevEvo->numHeads; head++) {
        nvDmaSetStartEvoMethod(pChannel, NVC57D_HEAD_SET_DESKTOP_COLOR_ALPHA_RED(head), 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(C57D, _HEAD_SET_DESKTOP_COLOR_ALPHA_RED, _ALPHA, 255) |
            DRF_NUM(C57D, _HEAD_SET_DESKTOP_COLOR_ALPHA_RED, _RED, 0));

        nvDmaSetStartEvoMethod(pChannel, NVC57D_HEAD_SET_DESKTOP_COLOR_GREEN_BLUE(head), 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(C57D, _HEAD_SET_DESKTOP_COLOR_GREEN_BLUE, _GREEN, 0) |
            DRF_NUM(C57D, _HEAD_SET_DESKTOP_COLOR_GREEN_BLUE, _BLUE, 0));
    }
}

void nvEvoInitChannel3(NVDevEvoPtr pDevEvo, NVEvoChannelPtr pChannel)
{
    InitChannelCapsC3(pDevEvo, pChannel);
}

static void EvoInitChannelC3(NVDevEvoPtr pDevEvo, NVEvoChannelPtr pChannel)
{
    const NvBool isCore =
            FLD_TEST_DRF64(_EVO, _CHANNEL_MASK, _CORE, _ENABLE,
                           pChannel->channelMask);

    nvEvoInitChannel3(pDevEvo, pChannel);

    if (isCore) {
        InitDesktopColorC3(pDevEvo, pChannel);
    }
}

static void EvoInitChannelC5(NVDevEvoPtr pDevEvo, NVEvoChannelPtr pChannel)
{
    const NvBool isCore =
            FLD_TEST_DRF64(_EVO, _CHANNEL_MASK, _CORE, _ENABLE,
                           pChannel->channelMask);
    const NvBool isWindow =
        ((pChannel->channelMask & NV_EVO_CHANNEL_MASK_WINDOW_ALL) != 0);

    nvEvoInitChannel3(pDevEvo, pChannel);

    if (isCore) {
        InitTaps5ScalerCoefficientsC5(pDevEvo, pChannel, FALSE);
        InitDesktopColorC5(pDevEvo, pChannel);
    } else if (isWindow) {
        NVEvoWindowCaps *pWinCaps =
            &pDevEvo->gpus[0].capabilities.window[pChannel->instance];
        NvU32 csc0SizesLen = ARRAY_LEN(OetfPQ512SegSizesLog2);
        NvU32 csc0EntriesLen = ARRAY_LEN(OetfPQ512Entries);
        NvU32 csc1SizesLen = ARRAY_LEN(EotfPQ512SegSizesLog2);
        NvU32 csc1EntriesLen = ARRAY_LEN(EotfPQ512Entries);

        InitTaps5ScalerCoefficientsC5(pDevEvo, pChannel, TRUE);

        if (pWinCaps->cscLUTsPresent) {
            InitCsc0LUT(pChannel,
                        OetfPQ512SegSizesLog2, csc0SizesLen,
                        OetfPQ512Entries, csc0EntriesLen);
            InitCsc1LUT(pChannel,
                        EotfPQ512SegSizesLog2, csc1SizesLen,
                        EotfPQ512Entries, csc1EntriesLen);
        }
    }
}

static enum FMTCoeffType EvoGetFMTCoeffType(
    NvBool isYUV,
    enum NvKmsInputColorSpace colorSpace,
    NvU8 depthPerComponent,
    enum NvKmsInputColorRange colorRange)
{
#define FMT(nvkms_space, coeff_space, depth, nvkms_range, coeff_range)                              \
    if ((colorSpace == NVKMS_INPUT_COLOR_SPACE_##nvkms_space) &&                                    \
        (depthPerComponent == depth) &&                                                             \
        (colorRange == NVKMS_INPUT_COLOR_RANGE_##nvkms_range)) {                                    \
        return FMT_COEFF_TYPE_##coeff_space##_YUV_##depth##BPC_##coeff_range##_TO_RGB_16BPC_FULL;   \
    }

    // RGB colorspaces use identity FMT
    if (!isYUV) {
        return FMT_COEFF_TYPE_IDENTITY;
    }

    if (colorRange == NVKMS_INPUT_COLOR_RANGE_DEFAULT) {
        // For YUV, default to limited color range
        colorRange = NVKMS_INPUT_COLOR_RANGE_LIMITED;
    }

    FMT(BT601,  REC601,   8, LIMITED, LTD);
    FMT(BT601,  REC601,   8, FULL,    FULL);
    FMT(BT601,  REC601,  10, LIMITED, LTD);
    FMT(BT601,  REC601,  10, FULL,    FULL);
    FMT(BT601,  REC601,  12, LIMITED, LTD);
    FMT(BT601,  REC601,  12, FULL,    FULL);

    FMT(BT709,  REC709,   8, LIMITED, LTD);
    FMT(BT709,  REC709,   8, FULL,    FULL);
    FMT(BT709,  REC709,  10, LIMITED, LTD);
    FMT(BT709,  REC709,  10, FULL,    FULL);
    FMT(BT709,  REC709,  12, LIMITED, LTD);
    FMT(BT709,  REC709,  12, FULL,    FULL);

    FMT(BT2100, REC2020,  8, LIMITED, LTD);
    FMT(BT2100, REC2020,  8, FULL,    FULL);
    FMT(BT2100, REC2020, 10, LIMITED, LTD);
    FMT(BT2100, REC2020, 10, FULL,    FULL);
    FMT(BT2100, REC2020, 12, LIMITED, LTD);
    FMT(BT2100, REC2020, 12, FULL,    FULL);

    // Unsupported formats also use identity FMT
    return FMT_COEFF_TYPE_IDENTITY;
#undef FMT
}

static const NvU32* EvoGetFMTMatrixC5(
    const enum NvKmsSurfaceMemoryFormat format,
    const NVFlipChannelEvoHwState *pHwState)
{
    const NvKmsSurfaceMemoryFormatInfo *pFormatInfo =
        nvKmsGetSurfaceMemoryFormatInfo(format);

    // Choose FMT matrix based on input colorspace, bpc, and colorrange.
    return FMTMatrix[EvoGetFMTCoeffType(pFormatInfo->isYUV,
                                        pHwState->colorSpace,
                                        pFormatInfo->isYUV ? pFormatInfo->yuv.depthPerComponent
                                                           : pFormatInfo->rgb.bitsPerPixel / 3,
                                        pHwState->colorRange)];
}

static void EvoSetFMTMatrixC5(
    NVEvoChannelPtr pChannel, const enum NvKmsSurfaceMemoryFormat format,
    const NVFlipChannelEvoHwState *pHwState)
{
    const NvU32 *matrix = EvoGetFMTMatrixC5(format, pHwState);
    NvU32 method = NVC57E_SET_FMT_COEFFICIENT_C00;
    int i;

    for (i = 0; i < 12; i++) {
        nvDmaSetStartEvoMethod(pChannel, method, 1);
        nvDmaSetEvoMethodData(pChannel, matrix[i]);

        method += 4;
    }
}

void nvEvoInitDefaultLutC5(NVDevEvoPtr pDevEvo)
{
    NVSurfaceEvoPtr pLut = pDevEvo->lut.defaultLut;
    NvU16 sd;

    nvAssert(pLut);

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        NvU32 lutSize;
        NvBool isLutModeVss;
        NVEvoLutDataRec *pData = pLut->cpuAddress[sd];

        EvoSetupIdentityBaseLutC5(pData,
                                  &pDevEvo->lut.defaultBaseLUTState[sd],
                                  &lutSize, &isLutModeVss);

        EvoSetupIdentityOutputLutC5(pData,
                                    &pDevEvo->lut.defaultOutputLUTState[sd],
                                    &lutSize, &isLutModeVss);
    }
}

static NvU32 GetWindowOwnerHead(const NVDevEvoRec *pDevEvo, const NvU32 win)
{
    if (pDevEvo->headForWindow[win] == NV_INVALID_HEAD) {
        return NVC37D_WINDOW_SET_CONTROL_OWNER_NONE;
    }

    nvAssert(pDevEvo->headForWindow[win] < pDevEvo->numHeads);
    return pDevEvo->headForWindow[win];
}

static void EvoInitWindowMapping3(NVDevEvoPtr pDevEvo,
                                  NVEvoModesetUpdateState *pModesetUpdateState)
{
    NVEvoUpdateState *updateState = &pModesetUpdateState->updateState;
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvU32 win, sd;

    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    /* Bind each window to a head.  On GV100, there is a fixed mapping. */
    for (win = 0; win < pDevEvo->numWindows; win++) {
        NvU32 head = GetWindowOwnerHead(pDevEvo, win);
        nvDmaSetStartEvoMethod(pChannel, NVC37D_WINDOW_SET_CONTROL(win), 1);
        nvDmaSetEvoMethodData(pChannel, DRF_NUM(C37D, _WINDOW_SET_CONTROL, _OWNER, head));
    }

    pModesetUpdateState->windowMappingChanged = FALSE;

    for (sd = 0;  sd < pDevEvo->numSubDevices; sd++) {
        void *pCoreDma = pDevEvo->pSubDevices[sd]->pCoreDma;
        /*
         * Short timeout (100ms) because we don't expect display to be very
         * busy at this point (it should at most be processing methods from
         * InitChannel()).
         */
        const NvU32 timeout = 100000;
        NvU64 startTime = 0;

        if (!((nvPeekEvoSubDevMask(pDevEvo) & (1 << sd)))) {
            continue;
        }

        /* This core channel must be idle before reading state cache */
        do {
            NvBool isIdle = NV_FALSE;
            if (!nvEvoIsChannelIdleC3(pDevEvo, pChannel, sd, &isIdle)) {
                nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR, "nvEvoIsChannelIdleC3() failed!");
            }
            if (isIdle) {
                break;
            }
            if (nvExceedsTimeoutUSec(pDevEvo, &startTime, timeout)) {
                nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                            "Timed out waiting for core channel idle.");
                break;
            }
        } while (TRUE);

        for (win = 0; win < pDevEvo->numWindows; win++) {
            NvU32 data = nvDmaLoadPioMethod(pCoreDma, NVC37D_WINDOW_SET_CONTROL(win));
            NvU32 head = GetWindowOwnerHead(pDevEvo, win);

            if (DRF_VAL(C37D, _WINDOW_SET_CONTROL, _OWNER, data) != head) {
                pModesetUpdateState->windowMappingChanged = TRUE;

                nvPushEvoSubDevMask(pDevEvo, NVBIT(sd));
                nvDisableCoreInterlockUpdateState(pDevEvo,
                                                  updateState,
                                                  pDevEvo->window[win]);
                nvPopEvoSubDevMask(pDevEvo);
            }
        }
    }
}

static void EvoInitWindowMappingC3(const NVDispEvoRec *pDispEvo,
                                   NVEvoModesetUpdateState *pModesetUpdateState)
{
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NVEvoUpdateState *updateState = &pModesetUpdateState->updateState;
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvU32 win;

    nvPushEvoSubDevMaskDisp(pDispEvo);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    EvoInitWindowMapping3(pDevEvo,
                          pModesetUpdateState);

    // Set window usage bounds
    for (win = 0; win < pDevEvo->numWindows; win++) {
        nvDmaSetStartEvoMethod(pChannel, NVC37D_WINDOW_SET_WINDOW_USAGE_BOUNDS(win), 1);
        /* XXXnvdisplay: window scaling */
        nvDmaSetEvoMethodData(pChannel, NV_EVO3_DEFAULT_WINDOW_USAGE_BOUNDS_C3);
    }
    nvPopEvoSubDevMask(pDevEvo);
}

void nvEvoInitWindowMappingC5(const NVDispEvoRec *pDispEvo,
                                   NVEvoModesetUpdateState *pModesetUpdateState)
{
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NVEvoUpdateState *updateState = &pModesetUpdateState->updateState;
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvU32 win;

    nvPushEvoSubDevMaskDisp(pDispEvo);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    EvoInitWindowMapping3(pDevEvo,
                          pModesetUpdateState);

    // Set window usage bounds
    for (win = 0; win < pDevEvo->numWindows; win++) {
        NvU32 bounds = NV_EVO3_DEFAULT_WINDOW_USAGE_BOUNDS_C5;

        bounds |=
            DRF_DEF(C57D, _WINDOW_SET_WINDOW_USAGE_BOUNDS, _INPUT_SCALER_TAPS, _TAPS_2) |
            DRF_DEF(C57D, _WINDOW_SET_WINDOW_USAGE_BOUNDS, _UPSCALING_ALLOWED, _FALSE);

        nvDmaSetStartEvoMethod(pChannel, NVC57D_WINDOW_SET_WINDOW_USAGE_BOUNDS(win), 1);
        nvDmaSetEvoMethodData(pChannel, bounds);
    }
    nvPopEvoSubDevMask(pDevEvo);
}

NvBool nvComputeMinFrameIdle(
    const NVHwModeTimingsEvo *pTimings,
    NvU16 *pLeadingRasterLines,
    NvU16 *pTrailingRasterLines)
{
    const NVHwModeViewPortEvo *pViewPort = &pTimings->viewPort;

    /*
     * leadingRasterLines defines the number of lines between the start of the
     * frame (vsync) and the start of the active region.  This includes Vsync,
     * Vertical Back Porch, and the top part of the overscan border.  The
     * minimum value is 2 because vsync and VBP must be at least 1 line each.
     *
     * trailingRasterLines defines the number of lines between the end of the
     * active region and the end of the frame.  This includes the bottom part
     * of the overscan border and the Vertical Front Porch.
     */
    const NvU32 activeHeight = (pTimings->rasterBlankStart.y -
                                pTimings->rasterBlankEnd.y);
    /* This is how it's done in dispClassNVD20CoreUpdateErrorChecks_hls.c */
    const NvU32 overscan = (activeHeight / 2) - (pViewPort->out.height / 2);

    /*
     * The +1 is justified by this comment in the error check:
     *
     * If the value is 1, that means there are 2 lines of vblank (lines 0 and
     * 1) before active.  That is why the uLeadingBorder equation needs +1;
     */
    const NvU32 leadingRasterLines =
        pTimings->rasterBlankEnd.y + overscan + pViewPort->out.yAdjust + 1;
    const NvU32 trailingRasterLines =
        pTimings->rasterSize.y - (leadingRasterLines + pViewPort->out.height);

    /* nvdClass_01.mfs says: "The minimum value is 2 because vsync and VBP must
     * be at least 1 line each." */
    if (leadingRasterLines < 2) {
        return FALSE;
    }

    *pLeadingRasterLines = leadingRasterLines;
    *pTrailingRasterLines = trailingRasterLines;

    return TRUE;
}

static void EvoSetRasterParams3(NVDevEvoPtr pDevEvo, int head,
                                const NVHwModeTimingsEvo *pTimings,
                                const NVEvoColorRec *pOverscanColor,
                                NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;
    /* XXXnvdisplay: Convert these for YCbCr, as necessary */
    NvU32 overscanColor =
        DRF_NUM(C37D, _HEAD_SET_OVERSCAN_COLOR, _RED_CR, pOverscanColor->red) |
        DRF_NUM(C37D, _HEAD_SET_OVERSCAN_COLOR, _GREEN_Y, pOverscanColor->green) |
        DRF_NUM(C37D, _HEAD_SET_OVERSCAN_COLOR, _BLUE_CB, pOverscanColor->blue);
    NvU32 hdmiStereoCtrl;
    NvU16 minFrameIdleLeadingRasterLines, minFrameIdleTrailingRasterLines;
    NvBool ret;

    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    // XXX[AGP]: These methods are sequential and could use an incrementing
    // method, but it's not clear if there's a bug in EVO that causes corruption
    // sometimes.  Play it safe and send methods with count=1.

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_OVERSCAN_COLOR(head), 1);
    nvDmaSetEvoMethodData(pChannel, overscanColor);

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_RASTER_SIZE(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37D, _HEAD_SET_RASTER_SIZE, _WIDTH, pTimings->rasterSize.x) |
        DRF_NUM(C37D, _HEAD_SET_RASTER_SIZE, _HEIGHT, pTimings->rasterSize.y));

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_RASTER_SYNC_END(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37D, _HEAD_SET_RASTER_SYNC_END, _X, pTimings->rasterSyncEnd.x) |
        DRF_NUM(C37D, _HEAD_SET_RASTER_SYNC_END, _Y, pTimings->rasterSyncEnd.y));

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_RASTER_BLANK_END(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37D, _HEAD_SET_RASTER_BLANK_END, _X, pTimings->rasterBlankEnd.x) |
        DRF_NUM(C37D, _HEAD_SET_RASTER_BLANK_END, _Y, pTimings->rasterBlankEnd.y));

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_RASTER_BLANK_START(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37D, _HEAD_SET_RASTER_BLANK_START, _X, pTimings->rasterBlankStart.x) |
        DRF_NUM(C37D, _HEAD_SET_RASTER_BLANK_START, _Y, pTimings->rasterBlankStart.y));

    ret = nvComputeMinFrameIdle(pTimings,
                              &minFrameIdleLeadingRasterLines,
                              &minFrameIdleTrailingRasterLines);
    if (!ret) {
        /* This should have been ensured by IMP in AssignPerHeadImpParams. */
        nvAssert(ret);
        /* In case a mode validation override was used to skip IMP, program the
         * default values.  This may still cause a hardware exception. */
        minFrameIdleLeadingRasterLines = 2;
        minFrameIdleTrailingRasterLines = 1;
    }

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_MIN_FRAME_IDLE(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37D, _HEAD_SET_MIN_FRAME_IDLE, _LEADING_RASTER_LINES,
                minFrameIdleLeadingRasterLines) |
        DRF_NUM(C37D, _HEAD_SET_MIN_FRAME_IDLE, _TRAILING_RASTER_LINES,
                minFrameIdleTrailingRasterLines));

    nvAssert((KHzToHz(pTimings->pixelClock) &
        ~DRF_MASK(NVC37D_HEAD_SET_PIXEL_CLOCK_FREQUENCY_HERTZ)) == 0x0);

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_PIXEL_CLOCK_FREQUENCY(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37D, _HEAD_SET_PIXEL_CLOCK_FREQUENCY, _HERTZ,
                KHzToHz(pTimings->pixelClock)) |
        DRF_DEF(C37D, _HEAD_SET_PIXEL_CLOCK_FREQUENCY, _ADJ1000DIV1001,_FALSE));

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_PIXEL_CLOCK_CONFIGURATION(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_DEF(C37D, _HEAD_SET_PIXEL_CLOCK_CONFIGURATION, _NOT_DRIVER, _FALSE) |
        DRF_DEF(C37D, _HEAD_SET_PIXEL_CLOCK_CONFIGURATION, _HOPPING, _DISABLE) |
        DRF_DEF(C37D, _HEAD_SET_PIXEL_CLOCK_CONFIGURATION, _HOPPING_MODE, _VBLANK));

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_PIXEL_CLOCK_FREQUENCY_MAX(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37D, _HEAD_SET_PIXEL_CLOCK_FREQUENCY_MAX, _HERTZ,
                KHzToHz(pTimings->pixelClock)) |
        DRF_DEF(C37D, _HEAD_SET_PIXEL_CLOCK_FREQUENCY_MAX, _ADJ1000DIV1001,_FALSE));

    nvDmaSetStartEvoMethod(pChannel,
        NVC37D_HEAD_SET_FRAME_PACKED_VACTIVE_COLOR(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37D, _HEAD_SET_FRAME_PACKED_VACTIVE_COLOR, _RED_CR, 0) |
#if defined(DEBUG)
        DRF_NUM(C37D, _HEAD_SET_FRAME_PACKED_VACTIVE_COLOR, _GREEN_Y,  512) |
#else
        DRF_NUM(C37D, _HEAD_SET_FRAME_PACKED_VACTIVE_COLOR, _GREEN_Y,  0) |
#endif
        DRF_NUM(C37D, _HEAD_SET_FRAME_PACKED_VACTIVE_COLOR, _BLUE_CB, 0));

    hdmiStereoCtrl = DRF_NUM(C37D, _HEAD_SET_HDMI_CTRL, _HDMI_VIC, 0);
    if (pTimings->hdmi3D) {
        hdmiStereoCtrl =
            FLD_SET_DRF(C37D, _HEAD_SET_HDMI_CTRL, _VIDEO_FORMAT, _STEREO3D, hdmiStereoCtrl);
    } else {
        hdmiStereoCtrl =
            FLD_SET_DRF(C37D, _HEAD_SET_HDMI_CTRL, _VIDEO_FORMAT, _NORMAL, hdmiStereoCtrl);
    }
    nvDmaSetStartEvoMethod(pChannel,
        NVC37D_HEAD_SET_HDMI_CTRL(head), 1);
    nvDmaSetEvoMethodData(pChannel, hdmiStereoCtrl);
}

static void EvoSetRasterParamsC3(NVDevEvoPtr pDevEvo, int head,
                                 const NVHwModeTimingsEvo *pTimings,
                                 const NvU8 tilePosition,
                                 const NVDscInfoEvoRec *pDscInfo,
                                 const NVEvoColorRec *pOverscanColor,
                                 NVEvoUpdateState *updateState)
{
    nvAssert(tilePosition == 0);
    EvoSetRasterParams3(pDevEvo, head, pTimings, pOverscanColor, updateState);
}

static void EvoSetRasterParams5(NVDevEvoPtr pDevEvo, int head,
                                const NVHwModeTimingsEvo *pTimings,
                                const NvU8 tilePosition,
                                const NVEvoColorRec *pOverscanColor,
                                NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;

    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    EvoSetRasterParams3(pDevEvo, head, pTimings, pOverscanColor, updateState);

    nvDmaSetStartEvoMethod(pChannel, NVC57D_HEAD_SET_TILE_POSITION(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C57D, _HEAD_SET_TILE_POSITION, _X, tilePosition) |
        DRF_NUM(C57D, _HEAD_SET_TILE_POSITION, _Y, 0));
}

static void EvoSetRasterParamsC5(NVDevEvoPtr pDevEvo, int head,
                                 const NVHwModeTimingsEvo *pTimings,
                                 const NvU8 tilePosition,
                                 const NVDscInfoEvoRec *pDscInfo,
                                 const NVEvoColorRec *pOverscanColor,
                                 NVEvoUpdateState *updateState)
{
    nvAssert(pDscInfo->type != NV_DSC_INFO_EVO_TYPE_HDMI);
    EvoSetRasterParams5(pDevEvo, head, pTimings, tilePosition, pOverscanColor,
                        updateState);
}

static NvU32 GetHdmiDscHBlankPixelTarget(const NVHwModeTimingsEvo *pTimings,
                                         const NVDscInfoEvoRec *pDscInfo)
{
    nvAssert((pDscInfo->dp.dscMode == NV_DSC_EVO_MODE_DUAL) ||
                 (pDscInfo->dp.dscMode == NV_DSC_EVO_MODE_SINGLE));

    const NvU32 hblankMin =
        (pDscInfo->dp.dscMode == NV_DSC_EVO_MODE_DUAL) ?
            ((pDscInfo->hdmi.hblankMin + 1) / 2) :
            pDscInfo->hdmi.hblankMin;

    NvU32 hBlankPixelTarget =
        NV_UNSIGNED_DIV_CEIL((pTimings->rasterSize.x *
                              pDscInfo->hdmi.dscTBlankToTTotalRatioX1k),
                              1000);

    hBlankPixelTarget = NV_MAX(hblankMin, hBlankPixelTarget);

    if (pDscInfo->dp.dscMode == NV_DSC_EVO_MODE_DUAL) {
        hBlankPixelTarget += (hBlankPixelTarget % 2);
    }

    return hBlankPixelTarget;
}

static void EvoSetRasterParamsC6(NVDevEvoPtr pDevEvo, int head,
                                 const NVHwModeTimingsEvo *pTimings,
                                 const NvU8 tilePosition,
                                 const NVDscInfoEvoRec *pDscInfo,
                                 const NVEvoColorRec *pOverscanColor,
                                 NVEvoUpdateState *updateState)
{
    NvU32 rasterHBlankDelay;
    NVEvoChannelPtr pChannel = pDevEvo->core;

    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    EvoSetRasterParams5(pDevEvo, head, pTimings, tilePosition, pOverscanColor,
                        updateState);

    if (pDscInfo->type == NV_DSC_INFO_EVO_TYPE_HDMI) {
        const NvU32 hBlank = pTimings->rasterSize.x -
            pTimings->rasterBlankStart.x + pTimings->rasterBlankEnd.x;
        const NvU32 hBlankPixelTarget =
            GetHdmiDscHBlankPixelTarget(pTimings, pDscInfo);

        const NvU32 headSetRasterHBlankDelayStart =
            pTimings->rasterSize.x - pTimings->rasterBlankStart.x - 2;
        const NvU32 headSetRasterHBlankDelayEnd =
            hBlankPixelTarget - hBlank + headSetRasterHBlankDelayStart;

        rasterHBlankDelay =
            DRF_NUM(C67D, _HEAD_SET_RASTER_HBLANK_DELAY, _BLANK_START,
                    headSetRasterHBlankDelayStart);
        rasterHBlankDelay |=
            DRF_NUM(C67D, _HEAD_SET_RASTER_HBLANK_DELAY, _BLANK_END,
                    headSetRasterHBlankDelayEnd);
    } else {
        rasterHBlankDelay = 0;
    }

    nvDmaSetStartEvoMethod(pChannel, NVC67D_HEAD_SET_RASTER_HBLANK_DELAY(head), 1);
    nvDmaSetEvoMethodData(pChannel, rasterHBlankDelay);
}

static void EvoSetProcAmpC3(NVDispEvoPtr pDispEvo, const NvU32 head,
                            NVEvoUpdateState *updateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoChannelPtr pChannel = pDevEvo->core;
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    NvU8 colorSpace;
    NvU32 dynRange;

    /* These methods should only apply to a single pDpyEvo */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    // These NVT defines match the HEAD_SET_PROCAMP ones.
    ct_assert(NVT_COLORIMETRY_RGB == NVC37D_HEAD_SET_PROCAMP_COLOR_SPACE_RGB);
    ct_assert(NVT_COLORIMETRY_YUV_601 == NVC37D_HEAD_SET_PROCAMP_COLOR_SPACE_YUV_601);
    ct_assert(NVT_COLORIMETRY_YUV_709 == NVC37D_HEAD_SET_PROCAMP_COLOR_SPACE_YUV_709);
    ct_assert(NVT_COLOR_RANGE_FULL == NVC37D_HEAD_SET_PROCAMP_RANGE_COMPRESSION_DISABLE);
    ct_assert(NVT_COLOR_RANGE_LIMITED == NVC37D_HEAD_SET_PROCAMP_RANGE_COMPRESSION_ENABLE);

    if (pHeadState->procAmp.colorimetry == NVT_COLORIMETRY_BT2020RGB) {
        colorSpace = NVC37D_HEAD_SET_PROCAMP_COLOR_SPACE_RGB;
    } else if (pHeadState->procAmp.colorimetry == NVT_COLORIMETRY_BT2020YCC) {
        colorSpace = NVC37D_HEAD_SET_PROCAMP_COLOR_SPACE_YUV_2020;
    } else {
        colorSpace = pHeadState->procAmp.colorimetry;
    }

    if (pHeadState->procAmp.colorRange == NVT_COLOR_RANGE_FULL) {
        dynRange = DRF_DEF(C37D, _HEAD_SET_PROCAMP, _DYNAMIC_RANGE, _VESA);
    } else {
        nvAssert(pHeadState->procAmp.colorRange == NVT_COLOR_RANGE_LIMITED);
        dynRange = DRF_DEF(C37D, _HEAD_SET_PROCAMP, _DYNAMIC_RANGE, _CEA);
    }

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_PROCAMP(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37D, _HEAD_SET_PROCAMP, _COLOR_SPACE, colorSpace) |
        DRF_DEF(C37D, _HEAD_SET_PROCAMP, _CHROMA_LPF, _DISABLE) |
        DRF_NUM(C37D, _HEAD_SET_PROCAMP, _SAT_COS,
                pHeadState->procAmp.satCos) |
        DRF_NUM(C37D, _HEAD_SET_PROCAMP, _SAT_SINE, 0) |
        dynRange |
        DRF_NUM(C37D, _HEAD_SET_PROCAMP, _RANGE_COMPRESSION,
                pHeadState->procAmp.colorRange) |
        DRF_DEF(C37D, _HEAD_SET_PROCAMP, _BLACK_LEVEL, _GRAPHICS));
}

static const struct NvKmsCscMatrix RGBToLimitedRangeYCbCrRec2020Matrix = {{
    {   0x7000, 0x1f9900, 0x1ff700, 0x8000 },
    {   0x3988,   0x947c,    0xcfc, 0x1000 },
    { 0x1fe0b8, 0x1faf44,   0x7000, 0x8000 },
}};
static const struct NvKmsCscMatrix RGBToLimitedRangeYCbCrRec709Matrix = {{
    {   0x7000, 0x1f9a44, 0x1ff5bc, 0x8000 },
    {   0x2e90,   0x9ca4,    0xfd0, 0x1000 },
    { 0x1fe654, 0x1fa9a8,   0x7000, 0x8000 },
}};
static const struct NvKmsCscMatrix RGBToLimitedRangeYCbCrRec601Matrix = {{
    {   0x7000, 0x1fa234, 0x1fedc8, 0x8000 },
    {   0x417c,   0x8090,   0x18f8, 0x1000 },
    { 0x1fda34, 0x1fb5cc,   0x7000, 0x8000 },
}};
static const struct NvKmsCscMatrix RGBToLimitedRangeRGB = {{
    { 0xdb04,      0,       0, 0x1000 },
    {      0, 0xdb04,       0, 0x1000 },
    {      0,      0,  0xdb04, 0x1000 },
}};


/*!
 * Return the appropriate OCSC1 matrix for the requested color range and
 * colorimetry, or NULL if the OCSC1 should be disabled.
 */
const struct NvKmsCscMatrix* nvEvoGetOCsc1MatrixC5(const NVDispHeadStateEvoRec *pHeadState)
{
    if (pHeadState->procAmp.colorRange == NVT_COLOR_RANGE_FULL) {
        switch (pHeadState->procAmp.colorimetry) {
            case NVT_COLORIMETRY_BT2020RGB:
                // fall through
            case NVT_COLORIMETRY_RGB:
                // No OCSC1 needed.
                return NULL;
            default:
                nvAssert(!"Unexpected colorimetry");
                return NULL;
        }
    } else {
        switch (pHeadState->procAmp.colorimetry) {
            case NVT_COLORIMETRY_BT2020RGB:
                // fall through
            case NVT_COLORIMETRY_RGB:
                return &RGBToLimitedRangeRGB;
            case NVT_COLORIMETRY_YUV_601:
                return &RGBToLimitedRangeYCbCrRec601Matrix;
            case NVT_COLORIMETRY_YUV_709:
                return &RGBToLimitedRangeYCbCrRec709Matrix;
            case NVT_COLORIMETRY_BT2020YCC:
                return &RGBToLimitedRangeYCbCrRec2020Matrix;
            default:
                nvAssert(!"Unexpected colorimetry");
                return NULL;
        }
    }
}

/*!
 * Return the output clamping ranges for the requested color range and
 * colorimetry.
 */
struct EvoClampRangeC5
nvEvoGetOCsc1ClampRange(const NVDispHeadStateEvoRec *pHeadState)
{
    if (pHeadState->procAmp.colorRange == NVT_COLOR_RANGE_FULL) {
        return (struct EvoClampRangeC5) {
            .green    = DRF_NUM(C57D, _HEAD_SET_CLAMP_RANGE_GREEN, _LOW,  0x0) |
                        DRF_NUM(C57D, _HEAD_SET_CLAMP_RANGE_GREEN, _HIGH, 0xFFF),
            .red_blue = DRF_NUM(C57D, _HEAD_SET_CLAMP_RANGE_RED_BLUE, _LOW,  0x0) |
                        DRF_NUM(C57D, _HEAD_SET_CLAMP_RANGE_RED_BLUE, _HIGH, 0xFFF),
        };
    } else {
        switch (pHeadState->procAmp.colorimetry) {
            default:
                nvAssert(!"Unexpected colorimetry");
                // fall through
            case NVT_COLORIMETRY_BT2020RGB:
                // fall through
            case NVT_COLORIMETRY_RGB:
                return (struct EvoClampRangeC5) {
                    .green    = DRF_NUM(C57D, _HEAD_SET_CLAMP_RANGE_GREEN, _LOW,  0x100) |
                                DRF_NUM(C57D, _HEAD_SET_CLAMP_RANGE_GREEN, _HIGH, 0xEB0),
                    .red_blue = DRF_NUM(C57D, _HEAD_SET_CLAMP_RANGE_RED_BLUE, _LOW,  0x100) |
                                DRF_NUM(C57D, _HEAD_SET_CLAMP_RANGE_RED_BLUE, _HIGH, 0xEB0),
                };
            case NVT_COLORIMETRY_YUV_601:
            case NVT_COLORIMETRY_YUV_709:
            case NVT_COLORIMETRY_BT2020YCC:
                return (struct EvoClampRangeC5) {
                    .green    = DRF_NUM(C57D, _HEAD_SET_CLAMP_RANGE_GREEN, _LOW,  0x100) |
                                DRF_NUM(C57D, _HEAD_SET_CLAMP_RANGE_GREEN, _HIGH, 0xEB0),
                    .red_blue = DRF_NUM(C57D, _HEAD_SET_CLAMP_RANGE_RED_BLUE, _LOW,  0x100) |
                                DRF_NUM(C57D, _HEAD_SET_CLAMP_RANGE_RED_BLUE, _HIGH, 0xF00),
                };
        }
    }
}

/*
 *     1.402       1.0       0.0
 * -0.714136       1.0 -0.344136
 *       0.0       1.0     1.772
 */
static const struct NvKmsMatrix CrYCb601toRGBMatrix = { {
    { 0x3fb374bc, 0x3f800000, 0x00000000 },
    { 0xbf36d19e, 0x3f800000, 0xbeb03298 },
    { 0x00000000, 0x3f800000, 0x3fe2d0e5 }
} };

/*
 *    1.5748       1.0       0.0
 * -0.468124       1.0 -0.187324
 *       0.0       1.0    1.8556
 */
static const struct NvKmsMatrix CrYCb709toRGBMatrix = { {
    { 0x3fc9930c, 0x3f800000, 0x00000000 },
    { 0xbeefadf3, 0x3f800000, 0xbe3fd1dd },
    { 0x00000000, 0x3f800000, 0x3fed844d }
} };

/*
 *       0.5 -0.418688 -0.081312
 *     0.299     0.587     0.114
 * -0.168736 -0.331264       0.5
 */
static const struct NvKmsMatrix RGBtoCrYCb601Matrix = { {
    { 0x3f000000, 0xbed65e46, 0xbda686e8 },
    { 0x3e991687, 0x3f1645a2, 0x3de978d5 },
    { 0xbe2cc921, 0xbea99b6f, 0x3f000000 }
} };

/*
 *       0.5  -0.45415  -0.04585
 *   0.21260   0.71520   0.07220
 *  -0.11457  -0.38543       0.5
 */
static const struct NvKmsMatrix RGBtoCrYCb709Matrix = { {
    { 0x3f000000, 0xbee88659, 0xbd3bcd36 },
    { 0x3e59b3d0, 0x3f371759, 0x3d93dd98 },
    { 0xbdeaa3ad, 0xbec55715, 0x3f000000 }
} };

static void EvoSetOCsc1C5(NVDispEvoPtr pDispEvo, const NvU32 head)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoChannelPtr pChannel = pDevEvo->core;
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    const struct NvKmsCscMatrix *matrix = nvEvoGetOCsc1MatrixC5(pHeadState);
    struct EvoClampRangeC5 clamp = nvEvoGetOCsc1ClampRange(pHeadState);

    nvDmaSetStartEvoMethod(pChannel, NVC57D_HEAD_SET_CLAMP_RANGE_GREEN(head), 1);
    nvDmaSetEvoMethodData(pChannel, clamp.green);
    nvDmaSetStartEvoMethod(pChannel, NVC57D_HEAD_SET_CLAMP_RANGE_RED_BLUE(head), 1);
    nvDmaSetEvoMethodData(pChannel, clamp.red_blue);

    if (matrix) {
        int x, y;
        NvU32 method = NVC57D_HEAD_SET_OCSC1COEFFICIENT_C00(head);

        nvDmaSetStartEvoMethod(pChannel, NVC57D_HEAD_SET_OCSC1CONTROL(head), 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_DEF(C57D, _HEAD_SET_OCSC1CONTROL, _ENABLE, _ENABLE));

        for (y = 0; y < 3; y++) {
            for (x = 0; x < 4; x++) {
                nvDmaSetStartEvoMethod(pChannel, method, 1);
                nvDmaSetEvoMethodData(pChannel, matrix->m[y][x]);

                method += 4;
            }
        }
    } else {
        nvDmaSetStartEvoMethod(pChannel, NVC57D_HEAD_SET_OCSC1CONTROL(head), 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_DEF(C57D, _HEAD_SET_OCSC1CONTROL, _ENABLE, _DISABLE));
    }
}

/*
 * Sets up the OCSC0 matrix coefficients, used to perform saturation
 * adjustment.
 *
 * The pipeline operates in FP16 RGB, however this adjustment must be
 * performed in CrYCb. Therefore, we multiply the saturation
 * adjustment matrix by the appropriate color space conversion
 * matrix. The specific color space used depends on the colorimetry of
 * the final output. Then we multiply by its inverse to convert back
 * to RGB. Finally, we convert the coefficients to S5.14 fixed point
 * format.
 *
 * NOTE: Hue and saturation adjustment would not typically be used with HDR
 * (BT2020) output, but is allowed here for the sake of compatibility.
 * TODO: Do hue/saturation adjustment in BT2020 with BT2020 output colorimetry.
 */
void nvEvo3PickOCsc0(const NVDispEvoRec *pDispEvo, const NvU32 head,
                     struct NvKms3x4MatrixF32 *ocsc0MatrixOutput, NvBool *pOutputRoundingFix)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    const NvU32 dispIdx = pDispEvo->displayOwner;
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    const NVEvoSubDevHeadStateRec *pSdHeadState = &pDevEvo->gpus[dispIdx].headState[head];

    const float32_t zeroF32 = NvU32viewAsF32(NV_FLOAT_ZERO);
    const float32_t oneF32 = NvU32viewAsF32(NV_FLOAT_ONE);
    const float32_t inv2048F32 = f32_div(NvU32viewAsF32(NV_FLOAT_HALF),
                                         NvU32viewAsF32(NV_FLOAT_1024));
    /* divide satCos by the default setting of 1024 */
    const float32_t satCos = f32_div(i32_to_f32(pHeadState->procAmp.satCos),
                                     NvU32viewAsF32(NV_FLOAT_1024));
    const struct NvKmsMatrixF32 satHueMatrix = { {
        {  satCos, zeroF32, zeroF32 },
        { zeroF32,  oneF32, zeroF32 },
        { zeroF32, zeroF32,  satCos }
    } };
    struct NvKms3x4MatrixF32 ocsc0Matrix = { {
        {  oneF32, zeroF32, zeroF32, zeroF32 },
        { zeroF32,  oneF32, zeroF32, zeroF32 },
        { zeroF32, zeroF32,  oneF32, zeroF32 }
    } };

    struct NvKmsMatrixF32 CrYCbtoRGBMatrix;
    struct NvKmsMatrixF32 RGBtoCrYCbMatrix;

    NvBool outputRoundingFix = FALSE;


    switch (pHeadState->procAmp.colorimetry) {
    default:
        nvAssert(!"Unexpected colorimetry");
        /* fallthrough */
    case NVT_COLORIMETRY_RGB:
    case NVT_COLORIMETRY_BT2020RGB:
        /* fallthrough; for RGB output, perform saturation adjustment in YUV709 */
    case NVT_COLORIMETRY_BT2020YCC:
        /* fallthrough; for BT2020 YUV output, perform saturation adjustment in YUV709 */
    case NVT_COLORIMETRY_YUV_709:
        CrYCbtoRGBMatrix = NvKmsMatrixToNvKmsMatrixF32(CrYCb709toRGBMatrix);
        RGBtoCrYCbMatrix = NvKmsMatrixToNvKmsMatrixF32(RGBtoCrYCb709Matrix);
        break;
    case NVT_COLORIMETRY_YUV_601:
        CrYCbtoRGBMatrix = NvKmsMatrixToNvKmsMatrixF32(CrYCb601toRGBMatrix);
        RGBtoCrYCbMatrix = NvKmsMatrixToNvKmsMatrixF32(RGBtoCrYCb601Matrix);
        break;
    }

    ocsc0Matrix = nvMultiply3x4Matrix(&RGBtoCrYCbMatrix, &ocsc0Matrix);
    ocsc0Matrix = nvMultiply3x4Matrix(&satHueMatrix, &ocsc0Matrix);
    ocsc0Matrix = nvMultiply3x4Matrix(&CrYCbtoRGBMatrix, &ocsc0Matrix);

    if (nvkms_output_rounding_fix()) {
        /*
         * Only apply WAR for bug 2267663 for linear output TFs. Non-linear
         * TFs could amplify the 1/2048 factor to the point of being
         * perceptible, so don't apply WAR if the OLUT has been specified.
         *
         * Custom OLUTs may be non-linear, so unconditionally disable the WAR if
         * one is specified.
         */

        if ((pHeadState->tf == NVKMS_OUTPUT_TF_NONE) &&
            (pSdHeadState->outputLut.pLutSurfaceEvo == NULL)) {
            ocsc0Matrix.m[0][3] = f32_add(ocsc0Matrix.m[0][3], inv2048F32);
            ocsc0Matrix.m[1][3] = f32_add(ocsc0Matrix.m[1][3], inv2048F32);
            ocsc0Matrix.m[2][3] = f32_add(ocsc0Matrix.m[2][3], inv2048F32);

            outputRoundingFix = TRUE;
        }
    }

    *ocsc0MatrixOutput = ocsc0Matrix;
    *pOutputRoundingFix = outputRoundingFix;
}

/*
 * Programs the OCSC0 matrix coefficients, used to perform saturation
 * adjustment.
 *
 * The OCSC0 matrix will be enabled later in EvoSetOutputLutC5 if
 * and only if we also enable the OLUT as required by the
 * specification.
 */
static void EvoSetOCsc0C5(const NVDispEvoRec *pDispEvo, const NvU32 head,
                          NvBool *pOutputRoundingFix)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NVEvoChannelPtr pChannel = pDevEvo->core;
    struct NvKms3x4MatrixF32 ocsc0Matrix;

    nvEvo3PickOCsc0(pDispEvo, head, &ocsc0Matrix, pOutputRoundingFix);

    nvDmaSetStartEvoMethod(pChannel, NVC57D_HEAD_SET_OCSC0COEFFICIENT_C00(head), 12);
    nvDmaSetEvoMethodData(pChannel, DRF_NUM(C57D, _HEAD_SET_OCSC0COEFFICIENT_C00, _VALUE, nvCscCoefConvertS514(ocsc0Matrix.m[0][0])));
    nvDmaSetEvoMethodData(pChannel, DRF_NUM(C57D, _HEAD_SET_OCSC0COEFFICIENT_C01, _VALUE, nvCscCoefConvertS514(ocsc0Matrix.m[0][1])));
    nvDmaSetEvoMethodData(pChannel, DRF_NUM(C57D, _HEAD_SET_OCSC0COEFFICIENT_C02, _VALUE, nvCscCoefConvertS514(ocsc0Matrix.m[0][2])));
    nvDmaSetEvoMethodData(pChannel, DRF_NUM(C57D, _HEAD_SET_OCSC0COEFFICIENT_C03, _VALUE, nvCscCoefConvertS514(ocsc0Matrix.m[0][3])));

    nvDmaSetEvoMethodData(pChannel, DRF_NUM(C57D, _HEAD_SET_OCSC0COEFFICIENT_C10, _VALUE, nvCscCoefConvertS514(ocsc0Matrix.m[1][0])));
    nvDmaSetEvoMethodData(pChannel, DRF_NUM(C57D, _HEAD_SET_OCSC0COEFFICIENT_C11, _VALUE, nvCscCoefConvertS514(ocsc0Matrix.m[1][1])));
    nvDmaSetEvoMethodData(pChannel, DRF_NUM(C57D, _HEAD_SET_OCSC0COEFFICIENT_C12, _VALUE, nvCscCoefConvertS514(ocsc0Matrix.m[1][2])));
    nvDmaSetEvoMethodData(pChannel, DRF_NUM(C57D, _HEAD_SET_OCSC0COEFFICIENT_C13, _VALUE, nvCscCoefConvertS514(ocsc0Matrix.m[1][3])));

    nvDmaSetEvoMethodData(pChannel, DRF_NUM(C57D, _HEAD_SET_OCSC0COEFFICIENT_C20, _VALUE, nvCscCoefConvertS514(ocsc0Matrix.m[2][0])));
    nvDmaSetEvoMethodData(pChannel, DRF_NUM(C57D, _HEAD_SET_OCSC0COEFFICIENT_C21, _VALUE, nvCscCoefConvertS514(ocsc0Matrix.m[2][1])));
    nvDmaSetEvoMethodData(pChannel, DRF_NUM(C57D, _HEAD_SET_OCSC0COEFFICIENT_C22, _VALUE, nvCscCoefConvertS514(ocsc0Matrix.m[2][2])));
    nvDmaSetEvoMethodData(pChannel, DRF_NUM(C57D, _HEAD_SET_OCSC0COEFFICIENT_C23, _VALUE, nvCscCoefConvertS514(ocsc0Matrix.m[2][3])));
}

static void EvoSetProcAmpC5(NVDispEvoPtr pDispEvo, const NvU32 head,
                            NVEvoUpdateState *updateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoChannelPtr pChannel = pDevEvo->core;
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    NvU32 dynRange, chromaLpf, chromaDownV;
    NvU32 colorimetry;
    NvBool outputRoundingFix;

    NVT_COLORIMETRY nvtColorimetry = pHeadState->procAmp.colorimetry;
    NVT_COLOR_RANGE nvtColorRange = pHeadState->procAmp.colorRange;

    /* These methods should only apply to a single pDpyEvo */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    switch (nvtColorimetry) {
        default:
            nvAssert(!"Unrecognized colorimetry");
            // fall through
        case NVT_COLORIMETRY_BT2020RGB:
            // fall through
        case NVT_COLORIMETRY_RGB:
            colorimetry = DRF_DEF(C57D, _HEAD_SET_PROCAMP, _COLOR_SPACE, _RGB);
            break;
        case NVT_COLORIMETRY_YUV_601:
            colorimetry = DRF_DEF(C57D, _HEAD_SET_PROCAMP, _COLOR_SPACE, _YUV_601);
            break;
        case NVT_COLORIMETRY_YUV_709:
            colorimetry = DRF_DEF(C57D, _HEAD_SET_PROCAMP, _COLOR_SPACE, _YUV_709);
            break;
        case NVT_COLORIMETRY_BT2020YCC:
            colorimetry = DRF_DEF(C57D, _HEAD_SET_PROCAMP, _COLOR_SPACE, _YUV_2020);
            break;
    }

    if (nvtColorRange == NVT_COLOR_RANGE_FULL) {
        dynRange = DRF_DEF(C57D, _HEAD_SET_PROCAMP, _DYNAMIC_RANGE, _VESA);
    } else {
        nvAssert(nvtColorRange == NVT_COLOR_RANGE_LIMITED);
        dynRange = DRF_DEF(C57D, _HEAD_SET_PROCAMP, _DYNAMIC_RANGE, _CEA);
    }

    /*
     * NVC67D_HEAD_SET_PROCAMP_CHROMA_DOWN_V is only defined in NVC67D, but
     * it is an unused bit in NVC57D_HEAD_SET_PROCAMP, and YUV420 should only
     * be set on >=nvdisplay 4.0, so it's okay to set it here.
     */
    if (pHeadState->procAmp.colorFormat == NVT_COLOR_FORMAT_YCbCr420) {
        chromaLpf = DRF_DEF(C57D, _HEAD_SET_PROCAMP, _CHROMA_LPF, _ENABLE);
        chromaDownV = DRF_DEF(C67D, _HEAD_SET_PROCAMP, _CHROMA_DOWN_V, _ENABLE);
    } else {
        chromaLpf = DRF_DEF(C57D, _HEAD_SET_PROCAMP, _CHROMA_LPF, _DISABLE);
        chromaDownV = DRF_DEF(C67D, _HEAD_SET_PROCAMP, _CHROMA_DOWN_V, _DISABLE);
    }

    nvDmaSetStartEvoMethod(pChannel, NVC57D_HEAD_SET_PROCAMP(head), 1);
    nvDmaSetEvoMethodData(pChannel,
                          colorimetry | dynRange | chromaLpf | chromaDownV);

    EvoSetOCsc0C5(pDispEvo, head, &outputRoundingFix);
    EvoSetOCsc1C5(pDispEvo, head);
}

/*
 * With nvdisplay, external fliplock pins are controlled via a headless
 * SetControl method, unlike previous EVO display implementations which
 * specified this information in the per-head HeadSetControl method.  This
 * function loops over all of the core nvkms HeadControl data structures to
 * determine which pins should be enabled in the SetControl method.  It should
 * be called any time the HeadControl data structures are updated.
 */
void nvEvoSetControlC3(NVDevEvoPtr pDevEvo, int sd)
{
    NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[sd];
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvU32 data = 0;
    NvU32 head;

    for (head = 0; head < pDevEvo->numHeads; head++) {
        NVEvoHeadControlPtr pHC = &pEvoSubDev->headControl[head];
        if (pHC->flipLock && !NV_EVO_LOCK_PIN_IS_INTERNAL(pHC->flipLockPin)) {
            NvU32 pin = pHC->flipLockPin - NV_EVO_LOCK_PIN_0;
            data = FLD_IDX_SET_DRF(C37D, _SET_CONTROL, _FLIP_LOCK_PIN,
                                   pin, _ENABLE, data);
        }
    }

    /*
     * GV100 HW bug 2062029 WAR
     *
     * GV100 always holds the external fliplock line low as if
     * NVC37D_SET_CONTROL_FLIP_LOCK_PIN was enabled.  To work around this,
     * the GV100 VBIOS initializes the fliplock GPIOs to be software
     * controlled (forced off).  The following rmctrl needs to be called to
     * switch HW control of the fliplock GPIOs back on whenever external
     * fliplock is enabled.
     */
    {
        NVC370_CTRL_SET_SWAPRDY_GPIO_WAR_PARAMS params = { };

        params.base.subdeviceIndex = pEvoSubDev->subDeviceInstance;
        params.bEnable = (data != 0);

        if (nvRmApiControl(
                nvEvoGlobal.clientHandle,
                pDevEvo->displayHandle,
                NVC370_CTRL_CMD_SET_SWAPRDY_GPIO_WAR,
                &params, sizeof(params)) != NVOS_STATUS_SUCCESS) {
            nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR, "Failed to override fliplock GPIO");
        }
    }

    nvDmaSetStartEvoMethod(pChannel, NVC37D_SET_CONTROL, 1);
    nvDmaSetEvoMethodData(pChannel, data);
}

static void EvoSetHeadControlC3(NVDevEvoPtr pDevEvo, int sd, int head,
                                NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[sd];
    /*
     * NOTE: This function should only push state to the hardware based on data
     * in the pHC.  If not, then we may miss updates due to the memcmp of the
     * HeadControl structure in UpdateEvoLockState().
     */
    NVEvoHeadControlPtr pHC = &pEvoSubDev->headControl[head];
    NvU32 data = 0, pin;
    NvU32 serverLockMode, clientLockMode;

    /* These methods should only apply to a single subdevice */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    switch (pHC->serverLock) {
    case NV_EVO_NO_LOCK:
        serverLockMode = NVC37D_HEAD_SET_CONTROL_MASTER_LOCK_MODE_NO_LOCK;
        break;
    case NV_EVO_FRAME_LOCK:
        serverLockMode = NVC37D_HEAD_SET_CONTROL_MASTER_LOCK_MODE_FRAME_LOCK;
        break;
    case NV_EVO_RASTER_LOCK:
        serverLockMode = NVC37D_HEAD_SET_CONTROL_MASTER_LOCK_MODE_RASTER_LOCK;
        break;
    default:
        nvAssert(!"Invalid server lock mode");
        return;
    }

    switch (pHC->clientLock) {
    case NV_EVO_NO_LOCK:
        clientLockMode = NVC37D_HEAD_SET_CONTROL_SLAVE_LOCK_MODE_NO_LOCK;
        break;
    case NV_EVO_FRAME_LOCK:
        clientLockMode = NVC37D_HEAD_SET_CONTROL_SLAVE_LOCK_MODE_FRAME_LOCK;
        break;
    case NV_EVO_RASTER_LOCK:
        clientLockMode = NVC37D_HEAD_SET_CONTROL_SLAVE_LOCK_MODE_RASTER_LOCK;
        break;
    default:
        nvAssert(!"Invalid client lock mode");
        return;
    }

    // Convert head control state to EVO method values.
    nvAssert(!pHC->interlaced);
    data |= DRF_DEF(C37D, _HEAD_SET_CONTROL, _STRUCTURE, _PROGRESSIVE);

    nvAssert(pHC->serverLockPin != NV_EVO_LOCK_PIN_ERROR);
    nvAssert(pHC->clientLockPin != NV_EVO_LOCK_PIN_ERROR);

    if (serverLockMode == NVC37D_HEAD_SET_CONTROL_MASTER_LOCK_MODE_NO_LOCK) {
        data |= DRF_DEF(C37D, _HEAD_SET_CONTROL, _MASTER_LOCK_PIN, _LOCK_PIN_NONE);
    } else if (NV_EVO_LOCK_PIN_IS_INTERNAL(pHC->serverLockPin)) {
        pin = pHC->serverLockPin - NV_EVO_LOCK_PIN_INTERNAL_0;
        /*
         * nvdClass_01.mfs says:
         * "master lock pin, if internal, must be set to the corresponding
         * internal pin for that head" (error check #12)
         */
        nvAssert(pin == head);
        data |= DRF_NUM(C37D, _HEAD_SET_CONTROL, _MASTER_LOCK_PIN,
                        NVC37D_HEAD_SET_CONTROL_MASTER_LOCK_PIN_INTERNAL_SCAN_LOCK(pin));
    } else {
        pin = pHC->serverLockPin - NV_EVO_LOCK_PIN_0;
        data |= DRF_NUM(C37D, _HEAD_SET_CONTROL, _MASTER_LOCK_PIN,
                        NVC37D_HEAD_SET_CONTROL_MASTER_LOCK_PIN_LOCK_PIN(pin));
    }
    data |= DRF_NUM(C37D, _HEAD_SET_CONTROL, _MASTER_LOCK_MODE, serverLockMode);

    if (clientLockMode == NVC37D_HEAD_SET_CONTROL_SLAVE_LOCK_MODE_NO_LOCK) {
        data |= DRF_DEF(C37D, _HEAD_SET_CONTROL, _SLAVE_LOCK_PIN, _LOCK_PIN_NONE);
    } else if (NV_EVO_LOCK_PIN_IS_INTERNAL(pHC->clientLockPin)) {
        pin = pHC->clientLockPin - NV_EVO_LOCK_PIN_INTERNAL_0;
        data |= DRF_NUM(C37D, _HEAD_SET_CONTROL, _SLAVE_LOCK_PIN,
                        NVC37D_HEAD_SET_CONTROL_SLAVE_LOCK_PIN_INTERNAL_SCAN_LOCK(pin));
    } else {
        pin = pHC->clientLockPin - NV_EVO_LOCK_PIN_0;
        data |= DRF_NUM(C37D, _HEAD_SET_CONTROL, _SLAVE_LOCK_PIN,
                        NVC37D_HEAD_SET_CONTROL_SLAVE_LOCK_PIN_LOCK_PIN(pin));
    }
    data |= DRF_NUM(C37D, _HEAD_SET_CONTROL, _SLAVE_LOCK_MODE, clientLockMode);
    data |= DRF_NUM(C37D, _HEAD_SET_CONTROL, _SLAVE_LOCKOUT_WINDOW,
                    pHC->clientLockoutWindow);

    /*
     * We always enable stereo lock when it's available and either framelock
     * or rasterlock is in use.
     */
    if (pHC->stereoLocked) {
        if (pHC->serverLock != NV_EVO_NO_LOCK) {
            data |= DRF_NUM(C37D, _HEAD_SET_CONTROL, _MASTER_STEREO_LOCK_MODE,
                            NVC37D_HEAD_SET_CONTROL_MASTER_STEREO_LOCK_MODE_ENABLE);
        }
        if (pHC->clientLock != NV_EVO_NO_LOCK) {
            data |= DRF_NUM(C37D, _HEAD_SET_CONTROL, _SLAVE_STEREO_LOCK_MODE,
                            NVC37D_HEAD_SET_CONTROL_SLAVE_STEREO_LOCK_MODE_ENABLE);
        }
    }

    nvAssert(pHC->stereoPin != NV_EVO_LOCK_PIN_ERROR);
    if (NV_EVO_LOCK_PIN_IS_INTERNAL(pHC->stereoPin)) {
        data |= DRF_DEF(C37D, _HEAD_SET_CONTROL, _STEREO_PIN, _LOCK_PIN_NONE);
    } else {
        pin = pHC->stereoPin - NV_EVO_LOCK_PIN_0;
        data |= DRF_NUM(C37D, _HEAD_SET_CONTROL, _STEREO_PIN,
                        NVC37D_HEAD_SET_CONTROL_STEREO_PIN_LOCK_PIN(pin));
    }

    if (pHC->hdmi3D) {
        data |= DRF_DEF(C37D, _HEAD_SET_CONTROL, _STEREO3D_STRUCTURE, _FRAME_PACKED);
    } else {
        data |= DRF_DEF(C37D, _HEAD_SET_CONTROL, _STEREO3D_STRUCTURE, _NORMAL);
    }

    /*
     * NVC67D_HEAD_SET_CONTROL_YUV420PACKER is only defined in NVC67D, but
     * it is an unused bit in NVC37D_HEAD_SET_CONTROL, and YUV420 should only
     * be set on >=nvdisplay 4.0, so it's okay to set it here.
     */
    if (pHC->hwYuv420) {
        data |= DRF_DEF(C67D, _HEAD_SET_CONTROL, _YUV420PACKER, _ENABLE);
    } else {
        data |= DRF_DEF(C67D, _HEAD_SET_CONTROL, _YUV420PACKER, _DISABLE);
    }

    // Send the HeadSetControl method.
    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_CONTROL(head), 1);
    nvDmaSetEvoMethodData(pChannel, data);

    nvEvoSetControlC3(pDevEvo, sd);

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_LOCK_CHAIN(head), 1);
    nvDmaSetEvoMethodData(pChannel, DRF_NUM(C37D, _HEAD_SET_LOCK_CHAIN, _POSITION,
                                     pHC->lockChainPosition));

/* XXX temporary WAR; see bug 4028718 */
#if !defined(NVC37D_HEAD_SET_LOCK_OFFSET)
#define NVC37D_HEAD_SET_LOCK_OFFSET(a)                                          (0x00002040 + (a)*0x00000400)
#define NVC37D_HEAD_SET_LOCK_OFFSET_X                                           14:0
#define NVC37D_HEAD_SET_LOCK_OFFSET_Y                                           30:16
#endif

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_LOCK_OFFSET(head), 1);
    nvDmaSetEvoMethodData(pChannel, pHC->setLockOffsetX ?
                          DRF_NUM(C37D, _HEAD_SET_LOCK_OFFSET, _X,
                                     27) : 0);
}

static void EvoSetHeadRefClkC3(NVDevEvoPtr pDevEvo, int head, NvBool external,
                               NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvU32 sd;

    /* These methods should only apply to a single subdevice */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        if (nvPeekEvoSubDevMask(pDevEvo) & (1 << sd)) {
            if (external) {
                pDevEvo->gpus[sd].setSwSpareA[head] =
                    FLD_SET_DRF(C37D,
                                _HEAD_SET_SW_SPARE_A_CODE,
                                _VPLL_REF,
                                _QSYNC,
                                pDevEvo->gpus[sd].setSwSpareA[head]);
            } else {
                pDevEvo->gpus[sd].setSwSpareA[head] =
                    FLD_SET_DRF(C37D,
                                _HEAD_SET_SW_SPARE_A_CODE,
                                _VPLL_REF,
                                _NO_PREF,
                                pDevEvo->gpus[sd].setSwSpareA[head]);
            }

            nvPushEvoSubDevMask(pDevEvo, NVBIT(sd));
            nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_SW_SPARE_A(head), 1);
            nvDmaSetEvoMethodData(pChannel, pDevEvo->gpus[sd].setSwSpareA[head]);
            nvPopEvoSubDevMask(pDevEvo);
        }
    }
}

static void EvoSORSetControlC3(const NVConnectorEvoRec *pConnectorEvo,
                               const enum nvKmsTimingsProtocol protocol,
                               const NvU32 orIndex,
                               const NvU32 headMask)
{
    NVDevEvoPtr pDevEvo = pConnectorEvo->pDispEvo->pDevEvo;
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvU32 hwProtocol = 0;

    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);
    nvAssert(orIndex != NV_INVALID_OR);

    if (headMask != 0) {
        switch (protocol) {
        default:
            nvAssert(!"Unknown SOR protocol");
            /* Fall through */
        case NVKMS_PROTOCOL_SOR_SINGLE_TMDS_A:
            hwProtocol = NVC37D_SOR_SET_CONTROL_PROTOCOL_SINGLE_TMDS_A;
            break;
        case NVKMS_PROTOCOL_SOR_SINGLE_TMDS_B:
            hwProtocol = NVC37D_SOR_SET_CONTROL_PROTOCOL_SINGLE_TMDS_B;
            break;
        case NVKMS_PROTOCOL_SOR_DUAL_TMDS:
            hwProtocol = NVC37D_SOR_SET_CONTROL_PROTOCOL_DUAL_TMDS;
            break;
        case NVKMS_PROTOCOL_SOR_DP_A:
            hwProtocol = NVC37D_SOR_SET_CONTROL_PROTOCOL_DP_A;
            break;
        case NVKMS_PROTOCOL_SOR_DP_B:
            hwProtocol = NVC37D_SOR_SET_CONTROL_PROTOCOL_DP_B;
            break;
        case NVKMS_PROTOCOL_SOR_LVDS_CUSTOM:
            hwProtocol = NVC37D_SOR_SET_CONTROL_PROTOCOL_LVDS_CUSTOM;
            break;
        case NVKMS_PROTOCOL_SOR_HDMI_FRL:
            hwProtocol = NVC67D_SOR_SET_CONTROL_PROTOCOL_HDMI_FRL;
            break;
        }
    }

    nvDmaSetStartEvoMethod(pChannel, NVC37D_SOR_SET_CONTROL(orIndex), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37D, _SOR_SET_CONTROL, _OWNER_MASK, headMask) |
        DRF_NUM(C37D, _SOR_SET_CONTROL, _PROTOCOL, hwProtocol) |
        DRF_DEF(C37D, _SOR_SET_CONTROL, _DE_SYNC_POLARITY, _POSITIVE_TRUE) |
        DRF_DEF(C37D, _SOR_SET_CONTROL, _PIXEL_REPLICATE_MODE, _OFF));
}

NvU32 nvEvoGetPixelDepthC3(const enum nvKmsPixelDepth pixelDepth)
{
    switch (pixelDepth) {
    case NVKMS_PIXEL_DEPTH_18_444:
        return NVC37D_HEAD_SET_CONTROL_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_18_444;
    case NVKMS_PIXEL_DEPTH_24_444:
        return NVC37D_HEAD_SET_CONTROL_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_24_444;
    case NVKMS_PIXEL_DEPTH_30_444:
        return NVC37D_HEAD_SET_CONTROL_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_30_444;
    case NVKMS_PIXEL_DEPTH_16_422:
        return NVC37D_HEAD_SET_CONTROL_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_16_422;
    case NVKMS_PIXEL_DEPTH_20_422:
        return NVC37D_HEAD_SET_CONTROL_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_20_422;

    }
    nvAssert(!"Unexpected pixel depth");
    return NVC37D_HEAD_SET_CONTROL_OUTPUT_RESOURCE_PIXEL_DEPTH_BPP_24_444;
}

static void EvoPIORSetControlC3(const NVConnectorEvoRec *pConnectorEvo,
                                const enum nvKmsTimingsProtocol protocol,
                                const NvU32 orIndex,
                                const NvU32 headMask)
{
    NVDevEvoPtr pDevEvo = pConnectorEvo->pDispEvo->pDevEvo;
    NVEvoChannelPtr pChannel = pDevEvo->core;

    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    if (headMask != 0) {
        nvAssert(protocol == NVKMS_PROTOCOL_PIOR_EXT_TMDS_ENC);
    }

    nvDmaSetStartEvoMethod(pChannel, NVC37D_PIOR_SET_CONTROL(orIndex), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37D, _PIOR_SET_CONTROL, _OWNER_MASK, headMask) |
        DRF_DEF(C37D, _PIOR_SET_CONTROL, _PROTOCOL, _EXT_TMDS_ENC) |
        DRF_DEF(C37D, _PIOR_SET_CONTROL, _DE_SYNC_POLARITY, _POSITIVE_TRUE));
}

static void EvoDSISetControlC6(const NVConnectorEvoRec *pConnectorEvo,
                               const enum nvKmsTimingsProtocol protocol,
                               const NvU32 orIndex,
                               const NvU32 headMask)
{
    NVDevEvoPtr pDevEvo = pConnectorEvo->pDispEvo->pDevEvo;
    NVEvoChannelPtr pChannel = pDevEvo->core;

    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);
    /* Only Head 0 can be used to drive DSI output on Orin */
    nvAssert((headMask == 0x0) || (headMask == 0x1));
    /* Only one DSI engine exists on Orin */
    nvAssert(orIndex == 0);

    if (headMask != 0) {
        nvAssert(protocol == NVKMS_PROTOCOL_DSI);
    }

    nvDmaSetStartEvoMethod(pChannel, NVC67D_DSI_SET_CONTROL(orIndex), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C67D, _DSI_SET_CONTROL, _OWNER_MASK, headMask));
}

static void EvoORSetControlC3Helper(const NVConnectorEvoRec *pConnectorEvo,
                                    const enum nvKmsTimingsProtocol protocol,
                                    const NvU32 orIndex,
                                    const NvU32 headMask)
{
    switch (pConnectorEvo->or.type) {
    case NV0073_CTRL_SPECIFIC_OR_TYPE_SOR:
        EvoSORSetControlC3(pConnectorEvo, protocol, orIndex, headMask);
        break;
    case NV0073_CTRL_SPECIFIC_OR_TYPE_PIOR:
        EvoPIORSetControlC3(pConnectorEvo, protocol, orIndex, headMask);
        break;
    case NV0073_CTRL_SPECIFIC_OR_TYPE_DAC:
        /* No DAC support on nvdisplay.  Fall through. */
    default:
        nvAssert(!"Invalid pConnectorEvo->or.type");
        break;
    }
}

void nvEvoORSetControlC3(NVDevEvoPtr pDevEvo,
                              const NVConnectorEvoRec *pConnectorEvo,
                              const enum nvKmsTimingsProtocol protocol,
                              const NvU32 orIndex,
                              const NvU32 headMask,
                              NVEvoUpdateState *updateState)
{
    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pDevEvo->core);

    EvoORSetControlC3Helper(pConnectorEvo, protocol, orIndex, headMask);
}

static void EvoORSetControlC6(NVDevEvoPtr pDevEvo,
                              const NVConnectorEvoRec *pConnectorEvo,
                              const enum nvKmsTimingsProtocol protocol,
                              const NvU32 orIndex,
                              const NvU32 headMask,
                              NVEvoUpdateState *updateState)
{
    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pDevEvo->core);

    switch (pConnectorEvo->or.type) {
    case NV0073_CTRL_SPECIFIC_OR_TYPE_DSI:
        EvoDSISetControlC6(pConnectorEvo, protocol, orIndex, headMask);
        break;
    default:
        EvoORSetControlC3Helper(pConnectorEvo, protocol, orIndex, headMask);
        break;
    }
}

static void EvoHeadSetControlORC3(NVDevEvoPtr pDevEvo,
                                  const int head,
                                  const NVHwModeTimingsEvo *pTimings,
                                  const enum nvKmsPixelDepth pixelDepth,
                                  const NvBool colorSpaceOverride,
                                  NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;
    const NvU32 hwPixelDepth = nvEvoGetPixelDepthC3(pixelDepth);
    const NvU16 colorSpaceFlag = nvEvo1GetColorSpaceFlag(pDevEvo,
                                                         colorSpaceOverride);

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_CONTROL_OUTPUT_RESOURCE(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_DEF(C37D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _CRC_MODE, _COMPLETE_RASTER) |
        (pTimings->hSyncPol ?
            DRF_DEF(C37D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _HSYNC_POLARITY, _NEGATIVE_TRUE) :
            DRF_DEF(C37D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _HSYNC_POLARITY, _POSITIVE_TRUE)) |
        (pTimings->vSyncPol ?
            DRF_DEF(C37D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _VSYNC_POLARITY, _NEGATIVE_TRUE) :
            DRF_DEF(C37D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _VSYNC_POLARITY, _POSITIVE_TRUE)) |
         DRF_NUM(C37D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _PIXEL_DEPTH, hwPixelDepth) |
        (colorSpaceOverride ?
            (DRF_DEF(C37D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _COLOR_SPACE_OVERRIDE, _ENABLE) |
             DRF_NUM(C37D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _COLOR_SPACE_FLAG, colorSpaceFlag)) :
            DRF_DEF(C37D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _COLOR_SPACE_OVERRIDE, _DISABLE)));
}

static void EvoHeadSetControlORC5(NVDevEvoPtr pDevEvo,
                                  const int head,
                                  const NVHwModeTimingsEvo *pTimings,
                                  const enum nvKmsPixelDepth pixelDepth,
                                  const NvBool colorSpaceOverride,
                                  NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;
    const NvU32 hwPixelDepth = nvEvoGetPixelDepthC3(pixelDepth);
    const NvU16 colorSpaceFlag = nvEvo1GetColorSpaceFlag(pDevEvo,
                                                         colorSpaceOverride);

    nvDmaSetStartEvoMethod(pChannel, NVC57D_HEAD_SET_CONTROL_OUTPUT_RESOURCE(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_DEF(C57D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _CRC_MODE, _COMPLETE_RASTER) |
        (pTimings->hSyncPol ?
            DRF_DEF(C57D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _HSYNC_POLARITY, _NEGATIVE_TRUE) :
            DRF_DEF(C57D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _HSYNC_POLARITY, _POSITIVE_TRUE)) |
        (pTimings->vSyncPol ?
            DRF_DEF(C57D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _VSYNC_POLARITY, _NEGATIVE_TRUE) :
            DRF_DEF(C57D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _VSYNC_POLARITY, _POSITIVE_TRUE)) |
         DRF_NUM(C57D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _PIXEL_DEPTH, hwPixelDepth) |
        (colorSpaceOverride ?
            (DRF_DEF(C57D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _COLOR_SPACE_OVERRIDE, _ENABLE) |
             DRF_NUM(C57D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _COLOR_SPACE_FLAG, colorSpaceFlag)) :
            DRF_DEF(C57D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _COLOR_SPACE_OVERRIDE, _DISABLE)) |
        DRF_DEF(C57D, _HEAD_SET_CONTROL_OUTPUT_RESOURCE, _EXT_PACKET_WIN, _NONE));
}

static void EvoHeadSetDisplayIdC3(NVDevEvoPtr pDevEvo,
                                  const NvU32 head, const NvU32 displayId,
                                  NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;

    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_DISPLAY_ID(head, 0), 1);
    nvDmaSetEvoMethodData(pChannel, displayId);
}

static void SetFormatUsageBoundsOneWindow3(NVDevEvoPtr pDevEvo, NvU32 window,
                                           const NvU64 supportedFormats,
                                           NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvU32 value = 0;

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED1BPP) {
        value = FLD_SET_DRF(C37D, _WINDOW_SET_WINDOW_FORMAT_USAGE_BOUNDS,
                            _RGB_PACKED1BPP, _TRUE, value);
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED2BPP) {
        value = FLD_SET_DRF(C37D, _WINDOW_SET_WINDOW_FORMAT_USAGE_BOUNDS,
                            _RGB_PACKED2BPP, _TRUE, value);
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED4BPP) {
        value = FLD_SET_DRF(C37D, _WINDOW_SET_WINDOW_FORMAT_USAGE_BOUNDS,
                            _RGB_PACKED4BPP, _TRUE, value);
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED8BPP) {
        value = FLD_SET_DRF(C37D, _WINDOW_SET_WINDOW_FORMAT_USAGE_BOUNDS,
                            _RGB_PACKED8BPP, _TRUE, value);
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_YUV_PACKED422) {
        value = FLD_SET_DRF(C37D, _WINDOW_SET_WINDOW_FORMAT_USAGE_BOUNDS,
                            _YUV_PACKED422, _TRUE, value);
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_YUV_SP420) {
        value = FLD_SET_DRF(C37D, _WINDOW_SET_WINDOW_FORMAT_USAGE_BOUNDS,
                            _YUV_SEMI_PLANAR420, _TRUE, value);
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_YUV_SP422) {
        value = FLD_SET_DRF(C37D, _WINDOW_SET_WINDOW_FORMAT_USAGE_BOUNDS,
                            _YUV_SEMI_PLANAR422, _TRUE, value);
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_YUV_SP444) {
        value = FLD_SET_DRF(C37D, _WINDOW_SET_WINDOW_FORMAT_USAGE_BOUNDS,
                            _YUV_SEMI_PLANAR444, _TRUE, value);
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_EXT_YUV_SP420) {
        value = FLD_SET_DRF(C37D, _WINDOW_SET_WINDOW_FORMAT_USAGE_BOUNDS,
                            _EXT_YUV_SEMI_PLANAR420, _TRUE, value);
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_EXT_YUV_SP422) {
        value = FLD_SET_DRF(C37D, _WINDOW_SET_WINDOW_FORMAT_USAGE_BOUNDS,
                            _EXT_YUV_SEMI_PLANAR422, _TRUE, value);
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_EXT_YUV_SP444) {
        value = FLD_SET_DRF(C37D, _WINDOW_SET_WINDOW_FORMAT_USAGE_BOUNDS,
                            _EXT_YUV_SEMI_PLANAR444, _TRUE, value);
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_YUV_PLANAR444) {
        value = FLD_SET_DRF(C37D, _WINDOW_SET_WINDOW_FORMAT_USAGE_BOUNDS,
                            _YUV_PLANAR444, _TRUE, value);
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_YUV_PLANAR420) {
        value = FLD_SET_DRF(C37D, _WINDOW_SET_WINDOW_FORMAT_USAGE_BOUNDS,
                            _YUV_PLANAR420, _TRUE, value);
    }

    if (supportedFormats != 0 && value == 0) {
        nvAssert(!"Unknown depth in SetFormatUsageBoundsOneWindow");
    }

    nvDmaSetStartEvoMethod(pChannel, NVC37D_WINDOW_SET_WINDOW_FORMAT_USAGE_BOUNDS(window), 1);
    nvDmaSetEvoMethodData(pChannel, value);
}

static void SetScalingUsageBoundsOneWindow5(
                                NVDevEvoPtr pDevEvo, NvU32 window,
                                const struct NvKmsScalingUsageBounds *pScaling,
                                NvBool layerUsable,
                                const NVHwModeViewPortEvo *pViewPort,
                                NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvU32 setWindowUsageBounds = NV_EVO3_DEFAULT_WINDOW_USAGE_BOUNDS_C5;
    NvU32 maxPixelsFetchedPerLine;

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    nvDmaSetStartEvoMethod(pChannel,
        NVC57D_WINDOW_SET_MAX_INPUT_SCALE_FACTOR(window), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C57D, _WINDOW_SET_MAX_INPUT_SCALE_FACTOR, _HORIZONTAL,
                pScaling->maxHDownscaleFactor) |
        DRF_NUM(C57D, _WINDOW_SET_MAX_INPUT_SCALE_FACTOR, _VERTICAL,
                pScaling->maxVDownscaleFactor));

    if (layerUsable) {
        maxPixelsFetchedPerLine = nvGetMaxPixelsFetchedPerLine(pViewPort->in.width,
                                                   pScaling->maxHDownscaleFactor);
    } else {
        maxPixelsFetchedPerLine = 0;
    }

    setWindowUsageBounds |=
        (DRF_NUM(C57D, _WINDOW_SET_WINDOW_USAGE_BOUNDS, _MAX_PIXELS_FETCHED_PER_LINE,maxPixelsFetchedPerLine)) |
        (pScaling->vTaps >= NV_EVO_SCALER_5TAPS ?
            DRF_DEF(C57D, _WINDOW_SET_WINDOW_USAGE_BOUNDS, _INPUT_SCALER_TAPS, _TAPS_5) :
            DRF_DEF(C57D, _WINDOW_SET_WINDOW_USAGE_BOUNDS, _INPUT_SCALER_TAPS, _TAPS_2)) |
        (pScaling->vUpscalingAllowed ?
            DRF_DEF(C57D, _WINDOW_SET_WINDOW_USAGE_BOUNDS, _UPSCALING_ALLOWED, _TRUE) :
            DRF_DEF(C57D, _WINDOW_SET_WINDOW_USAGE_BOUNDS, _UPSCALING_ALLOWED, _FALSE));
    nvDmaSetStartEvoMethod(pChannel,
        NVC57D_WINDOW_SET_WINDOW_USAGE_BOUNDS(window), 1);
    nvDmaSetEvoMethodData(pChannel, setWindowUsageBounds);
}

static NvBool EvoSetUsageBounds3(NVDevEvoPtr pDevEvo, NvU32 sd, NvU32 head,
                                 const struct NvKmsUsageBounds *pUsage,
                                 NVEvoUpdateState *updateState)
{
    const struct NvKmsUsageBounds *pCurrentUsage =
        &pDevEvo->gpus[sd].headState[head].usage;
    /* Return FALSE if a core channel UPDATE isn't actually needed. */
    NvBool needCoreUpdate = FALSE;
    NvU32 layer;

    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        NvU64 currentFormats = 0;
        NvU64 targetFormats = 0;

        if (pCurrentUsage->layer[layer].usable) {
            currentFormats =
                pCurrentUsage->layer[layer].supportedSurfaceMemoryFormats;
        }

        if (pUsage->layer[layer].usable) {
            targetFormats = pUsage->layer[layer].supportedSurfaceMemoryFormats;
        }

        if (targetFormats == currentFormats) {
            continue;
        }

        SetFormatUsageBoundsOneWindow3(pDevEvo,
                                       NV_EVO_CHANNEL_MASK_WINDOW_NUMBER(
                                        pDevEvo->head[head].layer[layer]->channelMask),
                                       targetFormats,
                                       updateState);
        needCoreUpdate = TRUE;
    }

    return needCoreUpdate;
}

static NvBool EvoSetUsageBoundsC3(NVDevEvoPtr pDevEvo, NvU32 sd, NvU32 head,
                                  const struct NvKmsUsageBounds *pUsage,
                                  NVEvoUpdateState *updateState)
{
    return EvoSetUsageBounds3(pDevEvo, sd, head, pUsage, updateState);
}

NvBool nvEvoSetUsageBoundsC5(NVDevEvoPtr pDevEvo, NvU32 sd, NvU32 head,
                                  const struct NvKmsUsageBounds *pUsage,
                                  NVEvoUpdateState *updateState)
{
    const struct NvKmsUsageBounds *pCurrentUsage =
        &pDevEvo->gpus[sd].headState[head].usage;
    NvBool needCoreUpdate;
    NvU32 layer;

    needCoreUpdate = EvoSetUsageBounds3(pDevEvo, sd, head, pUsage, updateState);

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        if ((pCurrentUsage->layer[layer].usable != pUsage->layer[layer].usable) ||
            (!nvEvoScalingUsageBoundsEqual(&pCurrentUsage->layer[layer].scaling,
                                           &pUsage->layer[layer].scaling))) {
            const NVHwModeViewPortEvo *pViewPort =
                &pDevEvo->gpus[sd].pDispEvo->headState[head].timings.viewPort;

            SetScalingUsageBoundsOneWindow5(
                pDevEvo,
                NV_EVO_CHANNEL_MASK_WINDOW_NUMBER(
                    pDevEvo->head[head].layer[layer]->channelMask),
                &pUsage->layer[layer].scaling,
                pUsage->layer[layer].usable,
                pViewPort,
                updateState);
            needCoreUpdate = TRUE;
        }
    }

    return needCoreUpdate;
}

static void EvoSetCoreNotifierSurfaceAddressAndControlC3(
    const NVDevEvoRec *pDevEvo,
    NVEvoChannelPtr pChannel,
    const NVSurfaceDescriptor *pSurfaceDesc,
    NvU32 notifierOffset,
    NvU32 ctrlVal)
{
    NvU32 ctxDmaHandle = pSurfaceDesc ? pSurfaceDesc->ctxDmaHandle : 0;

    nvDmaSetStartEvoMethod(pChannel, NVC37D_SET_CONTEXT_DMA_NOTIFIER, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37D, _SET_CONTEXT_DMA_NOTIFIER, _HANDLE, ctxDmaHandle));

    nvDmaSetStartEvoMethod(pChannel, NVC37D_SET_NOTIFIER_CONTROL, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37D, _SET_NOTIFIER_CONTROL, _OFFSET, notifierOffset) | ctrlVal);
}

void nvEvoSetNotifierC3(NVDevEvoRec *pDevEvo,
                             const NvBool notify,
                             const NvBool awaken,
                             const NvU32 notifier,
                             NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvU32 ctrlVal = 0;

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    /*
     * XXXnvdisplay: Note that nvdClass_01.mfs says:
     * "The units of the offset are 16 bytes.", while dispClass_02.mfs says:
     * "The units of the offset are 32 bit words."
     * The "legacy" 32-bit notifier format is no longer supported.  This will
     * have to be exposed to upper layers.
     */
    ASSERT_DRF_NUM(C37D, _SET_NOTIFIER_CONTROL, _OFFSET, notifier);

    ctrlVal = (awaken ?
               DRF_DEF(C37D, _SET_NOTIFIER_CONTROL, _MODE, _WRITE_AWAKEN) :
               DRF_DEF(C37D, _SET_NOTIFIER_CONTROL, _MODE, _WRITE));
    ctrlVal |= (notify ?
                DRF_DEF(C37D, _SET_NOTIFIER_CONTROL, _NOTIFY, _ENABLE) :
                DRF_DEF(C37D, _SET_NOTIFIER_CONTROL, _NOTIFY, _DISABLE));

    // To work around HW BUG 1945716, set the core channel completion notifier
    // context DMA to 0 when notification is not requested.
    if (notify) {
        NvU32 sd;
        for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
            if (nvPeekEvoSubDevMask(pDevEvo) & (1 << sd)) {
                nvPushEvoSubDevMask(pDevEvo, NVBIT(sd));
                pDevEvo->hal->SetCoreNotifierSurfaceAddressAndControl(pDevEvo,
                    pChannel, &pDevEvo->core->notifiersDma[sd].surfaceDesc,
                    notifier, ctrlVal);
                nvPopEvoSubDevMask(pDevEvo);
            }
        }
    } else {
        pDevEvo->hal->SetCoreNotifierSurfaceAddressAndControl(pDevEvo, pChannel,
            NULL /* pSurfaceDesc */, 0 /* offset */ , 0 /* ctrlVal */);
    }
}

static void UpdateCoreC3(NVEvoChannelPtr pChannel,
                         NVEvoChannelMask interlockChannelMask,
                         NvU32 flipLockPin,
                         NvBool releaseElv)
{
    NvU32 head, interlockFlags = 0;
    NvU32 window, windowInterlockFlags = 0;
    NvU32 update = DRF_NUM(C37D, _UPDATE, _FLIP_LOCK_PIN, flipLockPin);

    update |= releaseElv ? DRF_DEF(C37D, _UPDATE, _RELEASE_ELV, _TRUE) : 0;

    for (head = 0; head < NV_EVO_CHANNEL_MASK_CURSOR__SIZE; head++) {
        if (FLD_IDX_TEST_DRF64(_EVO, _CHANNEL_MASK, _CURSOR, head, _ENABLE,
                               interlockChannelMask)) {
            interlockFlags |=
                DRF_IDX_DEF(C37D, _SET_INTERLOCK_FLAGS,
                            _INTERLOCK_WITH_CURSOR, head, _ENABLE);
        }
    }

    for (window = 0; window < NV_EVO_CHANNEL_MASK_WINDOW__SIZE; window++) {
        if (FLD_IDX_TEST_DRF64(_EVO, _CHANNEL_MASK, _WINDOW, window, _ENABLE,
                               interlockChannelMask)) {
            windowInterlockFlags |=
                DRF_IDX_DEF(C37D, _SET_WINDOW_INTERLOCK_FLAGS,
                            _INTERLOCK_WITH_WINDOW, window, _ENABLE);
        }
    }

    nvDmaSetStartEvoMethod(pChannel, NVC37D_SET_INTERLOCK_FLAGS, 1);
    nvDmaSetEvoMethodData(pChannel, interlockFlags);

    nvDmaSetStartEvoMethod(pChannel, NVC37D_SET_WINDOW_INTERLOCK_FLAGS, 1);
    nvDmaSetEvoMethodData(pChannel, windowInterlockFlags);

    nvDmaSetStartEvoMethod(pChannel, NVC37D_UPDATE, 1);
    nvDmaSetEvoMethodData(pChannel, update);

    nvDmaKickoffEvo(pChannel);
}

static void UpdateWindowIMM(NVEvoChannelPtr pChannel,
                            NVEvoChannelMask winImmChannelMask,
                            NVEvoChannelMask winImmInterlockMask,
                            NvBool releaseElv)
{
    nvAssert((winImmChannelMask & ~NV_EVO_CHANNEL_MASK_WINDOW_ALL) == 0);
    nvAssert((winImmInterlockMask & ~NV_EVO_CHANNEL_MASK_WINDOW_ALL) == 0);

    if ((winImmChannelMask & pChannel->channelMask) != 0) {
        NvU32 updateImm = 0;

        if ((winImmInterlockMask & pChannel->channelMask) != 0) {
            updateImm |= DRF_DEF(C37B, _UPDATE, _INTERLOCK_WITH_WINDOW, _ENABLE);
        } else {
            updateImm |= DRF_DEF(C37B, _UPDATE, _INTERLOCK_WITH_WINDOW, _DISABLE);
        }
        updateImm |= releaseElv ? DRF_DEF(C37B, _UPDATE, _RELEASE_ELV, _TRUE) : 0;

        nvDmaSetStartEvoMethod(pChannel->imm.u.dma, NVC37B_UPDATE, 1);
        nvDmaSetEvoMethodData(pChannel->imm.u.dma, updateImm);
        nvDmaKickoffEvo(pChannel->imm.u.dma);
    }
}

static void UpdateWindowC3(NVEvoChannelPtr pChannel,
                           NVEvoChannelMask interlockChannelMask,
                           NVEvoChannelMask winImmChannelMask,
                           NVEvoChannelMask winImmInterlockMask,
                           NvBool transitionWAR,
                           NvU32 flipLockPin,
                           NvBool releaseElv)
{
    NvU32 head, interlockFlags = 0;
    NvU32 window, windowInterlockFlags = 0;
    NvU32 update = DRF_NUM(C37E, _UPDATE, _FLIP_LOCK_PIN, flipLockPin);

    update |= releaseElv ? DRF_DEF(C37E, _UPDATE, _RELEASE_ELV, _TRUE) : 0;

    if ((winImmInterlockMask & pChannel->channelMask) != 0) {
        /*
         * We expect winImmChannelMask to always be a superset of
         * winImmInterlockMask. We should never interlock with a window
         * immediate channel if we're not also going to kick off that
         * window immediate channel.
         */
        nvAssert((winImmChannelMask & pChannel->channelMask) != 0);

        update |= DRF_DEF(C37E, _UPDATE, _INTERLOCK_WITH_WIN_IMM, _ENABLE);
    } else {
        update |= DRF_DEF(C37E, _UPDATE, _INTERLOCK_WITH_WIN_IMM, _DISABLE);
    }

    // Nothing currently requires updating a window channel without releasing
    // ELV.
    nvAssert(releaseElv);

    if (FLD_TEST_DRF64(_EVO, _CHANNEL_MASK, _CORE, _ENABLE,
                       interlockChannelMask)) {
        interlockFlags |=
            DRF_DEF(C37E, _SET_INTERLOCK_FLAGS, _INTERLOCK_WITH_CORE, _ENABLE);
    }

    for (head = 0; head < NV_EVO_CHANNEL_MASK_CURSOR__SIZE; head++) {
        if (FLD_IDX_TEST_DRF64(_EVO, _CHANNEL_MASK, _CURSOR, head, _ENABLE,
                               interlockChannelMask)) {
            interlockFlags |=
                DRF_IDX_DEF(C37E, _SET_INTERLOCK_FLAGS,
                            _INTERLOCK_WITH_CURSOR, head, _ENABLE);
        }
    }

    for (window = 0; window < NV_EVO_CHANNEL_MASK_WINDOW__SIZE; window++) {
        if (FLD_IDX_TEST_DRF64(_EVO, _CHANNEL_MASK, _WINDOW, window, _ENABLE,
                               interlockChannelMask)) {
            windowInterlockFlags |=
                DRF_IDX_DEF(C37E, _SET_WINDOW_INTERLOCK_FLAGS,
                            _INTERLOCK_WITH_WINDOW, window, _ENABLE);
        }
    }

    nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_INTERLOCK_FLAGS, 1);
    nvDmaSetEvoMethodData(pChannel, interlockFlags);

    nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_WINDOW_INTERLOCK_FLAGS, 1);
    nvDmaSetEvoMethodData(pChannel, windowInterlockFlags);

    /*
     * If we determined that this update will transition from NULL to non-NULL
     * ctxdma or vice-versa, bookend this update method with software methods
     * to notify RM to apply a workaround for hardware bug 2193096.
     */
    if (transitionWAR) {
        nvDmaSetStartEvoMethod(pChannel, NVC57E_SW_SET_MCLK_SWITCH, 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_DEF(C57E, _SW_SET_MCLK_SWITCH, _ENABLE, _FALSE));
    }

    nvDmaSetStartEvoMethod(pChannel, NVC37E_UPDATE, 1);
    nvDmaSetEvoMethodData(pChannel, update);

    if (transitionWAR) {
        nvDmaSetStartEvoMethod(pChannel, NVC57E_SW_SET_MCLK_SWITCH, 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_DEF(C57E, _SW_SET_MCLK_SWITCH, _ENABLE, _TRUE));
    }

    UpdateWindowIMM(pChannel, winImmChannelMask,
                    winImmInterlockMask, releaseElv);

    nvDmaKickoffEvo(pChannel);
}

/*!
 * This function finds any fliplocked channels in the current update and pushes
 * flips for them setting the appropriate fliplock pin and interlock masks.
 *
 * All of this complexity is here to support the case where multiple heads on a
 * single GPU are fliplocked together, but flip requests come in for only a
 * subset of those heads at a time (e.g., separate X screens on a single GPU).
 * Unlike previous hardware, we're required to interlock all channels which are
 * part of a fliplock update, instead of just using fliplock across heads.
 */
/*
 * There are two scenarios:
 * a) All fliplocked channels on this GPU are already part of this update.  In
 *    that case we just need to set the appropriate fliplock pin for each, and
 *    we're done -- they're already interlocked.
 * b) Some fliplocked channels are not part of this update.  We still need to
 *    set them in the interlock mask, but it's dangerous to interlock with any
 *    channels *not* in the fliplock group; as an example:
 *    With two separate X screens on a single GPU, each driving one monitor,
 *    fliplocked together, if we get a flip request for screen 0/head 0 that
 *    interlocks core and base, then a second flip request for screen 1/head1
 *    that interlocks core and base, we would end up programming one flip on
 *    the window on head 0, one flip on the window on head 1, and two flips in
 *    the core channel.  The second core channel flip would never complete
 *    since it would be waiting for an interlock with the other window
 *    channels.
 *
 *    To handle this case we pull the fliplocked channels out of this update
 *    and update them interlocked with all fliplocked channels (including those
 *    that aren't in this update), then proceed with a normal interlocked
 *    update excluding the fliplocked channels.
 *
 * \return Channel mask of channels which were handled by this function.
 *              Channels in this mask should be considered done and have no
 *              further updates pushed.  No other channels should be
 *              interlocked with them.
 */
static NVEvoChannelMask ProcessFlipLockUpdates(
    NVDevEvoPtr pDevEvo,
    NvU32 sd,
    NvU32 *pFlipLockPin,
    const NVEvoUpdateState *updateState)
{
    NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[sd];
    NvU32 head, window;
    /* Channels that are part of this update which need to be fliplocked. */
    NVEvoChannelMask flipLockUpdateMask = 0;
    /* All channels on this subdevice which are fliplocked. */
    NVEvoChannelMask flipLockAllMask = 0;
    /* Channels which this function has handled and do not need further
     * processing. */
    NVEvoChannelMask handledMask = 0;
    NVEvoLockPin pin = NV_EVO_LOCK_PIN_ERROR;
    NvU32 hwPin = NVC37E_UPDATE_FLIP_LOCK_PIN_LOCK_PIN_NONE;

    /* First check if any of the fliplock-qualifying channels are actually
     * fliplocked, and determine which pin they're using. */
    for (head = 0; head < pDevEvo->numHeads; head++) {
        NVEvoHeadControlPtr pHC = &pEvoSubDev->headControl[head];

        if (pHC->flipLock) {
            /* Convert the head index to a window index (two windows per head,
             * one "base" and one "overlay"; we only fliplock "base") */
            NVEvoChannelMask windowMask =
                DRF_IDX_DEF64(_EVO, _CHANNEL_MASK, _WINDOW, head * 2, _ENABLE);
            if (updateState->subdev[sd].flipLockQualifyingMask & windowMask) {
                if (flipLockUpdateMask == 0) {
                    pin = pHC->flipLockPin;
                } else {
                    /* For now, we only support kicking off a single fliplock
                     * group as part of a single update call. */
                    nvAssert(pin == pHC->flipLockPin);
                }
                flipLockUpdateMask |= windowMask;
            }
        }
    }

    /* If we don't have any fliplocked updates, then we're done. */
    if (flipLockUpdateMask == 0) {
        goto done;
    }

    /*
     * Gather all of the channels on this GPU which are part of this fliplock
     * group (some of which may not be part of this update).
     */
    for (head = 0; head < pDevEvo->numHeads; head++) {
        NVEvoHeadControlPtr pHC = &pEvoSubDev->headControl[head];

        if (pHC->flipLock && (pHC->flipLockPin == pin)) {
            NVEvoChannelMask windowMask =
                DRF_IDX_DEF64(_EVO, _CHANNEL_MASK, _WINDOW, head * 2, _ENABLE);
            flipLockAllMask |= windowMask;
        }
    }

    /* Convert the pin to a hardware enum. */
    if (NV_EVO_LOCK_PIN_IS_INTERNAL(pin)) {
        hwPin = NVC37E_UPDATE_FLIP_LOCK_PIN_INTERNAL_FLIP_LOCK_0 +
                (pin - NV_EVO_LOCK_PIN_INTERNAL_0);
    } else {
        hwPin = NVC37E_UPDATE_FLIP_LOCK_PIN_LOCK_PIN(pin - NV_EVO_LOCK_PIN_0);
    }

    /* If we're updating all of the fliplocked channels in this update, we can
     * interlock with other channels as normal. */
    if (flipLockUpdateMask == flipLockAllMask) {
        goto done;
    }

    /*
     * Kick off each of our update channels, using the full fliplock mask and
     * hwPin calculated above.
     */
    nvAssert((flipLockUpdateMask & ~NV_EVO_CHANNEL_MASK_WINDOW_ALL) == 0);
    for (window = 0; window < pDevEvo->numWindows; window++) {
        const NVEvoChannelMask windowMask =
            DRF_IDX_DEF64(_EVO, _CHANNEL_MASK, _WINDOW, window, _ENABLE);
        NVEvoChannelMask winImmChannelMask =
            updateState->subdev[sd].winImmChannelMask;
        NVEvoChannelMask winImmInterlockMask =
            updateState->subdev[sd].winImmInterlockMask;
        if (flipLockUpdateMask & windowMask) {
            const NvBool transitionWAR =
                (updateState->subdev[sd].flipTransitionWAR & windowMask) != 0;
            UpdateWindowC3(pDevEvo->window[window],
                           flipLockAllMask,
                           winImmChannelMask,
                           winImmInterlockMask,
                           transitionWAR,
                           hwPin, TRUE /* releaseElv */);
        } else {
            UpdateWindowIMM(pDevEvo->window[window], winImmChannelMask,
                            winImmInterlockMask, TRUE /* releaseElv */);
        }
    }
    handledMask = flipLockUpdateMask;
    hwPin = NVC37E_UPDATE_FLIP_LOCK_PIN_LOCK_PIN_NONE;

done:
    *pFlipLockPin = hwPin;
    return handledMask;
}

void nvEvoUpdateC3(NVDevEvoPtr pDevEvo,
                        const NVEvoUpdateState *updateState,
                        NvBool releaseElv)
{
    NvU32 sd, window;

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        NVEvoChannelMask updateChannelMask =
            updateState->subdev[sd].channelMask;
        const NVEvoChannelMask noCoreInterlockMask =
            updateState->subdev[sd].noCoreInterlockMask;
        NVEvoChannelMask coreInterlockMask =
            updateChannelMask & ~noCoreInterlockMask;
        const NvU32 subDeviceMask = (1 << sd);
        NvU32 flipLockPin = NVC37E_UPDATE_FLIP_LOCK_PIN_LOCK_PIN_NONE;

        nvPushEvoSubDevMask(pDevEvo, subDeviceMask);

        if (updateState->subdev[sd].flipLockQualifyingMask) {
            NVEvoChannelMask handledChannels = 0;

            nvAssert((updateState->subdev[sd].flipLockQualifyingMask &
                      ~updateChannelMask) == 0);
            nvAssert((updateState->subdev[sd].flipLockQualifyingMask &
                      updateState->subdev[sd].noCoreInterlockMask) == 0);

            handledChannels =
                ProcessFlipLockUpdates(pDevEvo, sd, &flipLockPin, updateState);

            updateChannelMask &= ~handledChannels;
            coreInterlockMask &= ~handledChannels;
        }

        if (FLD_TEST_DRF64(_EVO, _CHANNEL_MASK, _CORE, _ENABLE,
                           updateChannelMask)) {
            const NVEvoChannelMask thisInterlockMask =
                FLD_TEST_DRF64(_EVO, _CHANNEL_MASK, _CORE, _ENABLE,
                               coreInterlockMask) ? coreInterlockMask : 0;
            UpdateCoreC3(pDevEvo->core, thisInterlockMask, flipLockPin,
                         releaseElv);
        }

        for (window = 0; window < pDevEvo->numWindows; window++) {
            const NVEvoChannelMask windowMask =
                DRF_IDX_DEF64(_EVO, _CHANNEL_MASK, _WINDOW, window, _ENABLE);
            NVEvoChannelMask winImmChannelMask =
                updateState->subdev[sd].winImmChannelMask;
            NVEvoChannelMask winImmInterlockMask =
                updateState->subdev[sd].winImmInterlockMask;
            if (updateChannelMask & windowMask) {
                const NvBool transitionWAR =
                    (updateState->subdev[sd].flipTransitionWAR & windowMask) != 0;
                NVEvoChannelMask thisInterlockMask =
                    FLD_IDX_TEST_DRF64(_EVO, _CHANNEL_MASK, _WINDOW, window, _ENABLE,
                                       coreInterlockMask) ? coreInterlockMask : 0;
                UpdateWindowC3(pDevEvo->window[window],
                               thisInterlockMask,
                               winImmChannelMask,
                               winImmInterlockMask,
                               transitionWAR,
                               flipLockPin,
                               releaseElv);
            } else {
                UpdateWindowIMM(pDevEvo->window[window], winImmChannelMask,
                                winImmInterlockMask, releaseElv);
            }
        }

        nvPopEvoSubDevMask(pDevEvo);
    }
}

/*!
 * Initialize head-specific IMP param fields.
 *
 * Initialize the NVC372_CTRL_IMP_HEAD for the specific head.
 *
 * \param[out]  pImpHead   The param structure to initialize.
 * \param[in]   pTimings   The rastering timings and viewport configuration.
 * \param[in]   head       The number of the head that will be driven.
 *
 * \return      FALSE iff the parameters aren't even legal for HW.
 */
static NvBool AssignPerHeadImpParams(NVC372_CTRL_IMP_HEAD *pImpHead,
                                     const NVHwModeTimingsEvo *pTimings,
                                     const NvBool enableDsc,
                                     const NvBool b2Heads1Or,
                                     const int head,
                                     const NVEvoScalerCaps *pScalerCaps)
{
    const NVHwModeViewPortEvo *pViewPort = &pTimings->viewPort;
    struct NvKmsScalingUsageBounds scalingUsageBounds = { };

    pImpHead->headIndex = head;

    /* raster timings */

    pImpHead->maxPixelClkKHz = pTimings->pixelClock;

    pImpHead->rasterSize.width           = pTimings->rasterSize.x;
    pImpHead->rasterSize.height          = pTimings->rasterSize.y;
    pImpHead->rasterBlankStart.X         = pTimings->rasterBlankStart.x;
    pImpHead->rasterBlankStart.Y         = pTimings->rasterBlankStart.y;
    pImpHead->rasterBlankEnd.X           = pTimings->rasterBlankEnd.x;
    pImpHead->rasterBlankEnd.Y           = pTimings->rasterBlankEnd.y;
    pImpHead->rasterVertBlank2.yStart    = pTimings->rasterVertBlank2Start;
    pImpHead->rasterVertBlank2.yEnd      = pTimings->rasterVertBlank2End;

    /* XXX TODO: Fill in correct scanlock information (only needed for
     * MIN_VPSTATE). */
    pImpHead->control.masterLockMode = NV_DISP_LOCK_MODE_NO_LOCK;
    pImpHead->control.masterLockPin = NV_DISP_LOCK_PIN_UNSPECIFIED;
    pImpHead->control.slaveLockMode = NV_DISP_LOCK_MODE_NO_LOCK;
    pImpHead->control.slaveLockPin = NV_DISP_LOCK_PIN_UNSPECIFIED;

    if (!nvComputeScalingUsageBounds(pScalerCaps,
                                    pViewPort->in.width, pViewPort->in.height,
                                    pViewPort->out.width, pViewPort->out.height,
                                    pViewPort->hTaps, pViewPort->vTaps,
                                    &scalingUsageBounds)) {
        return FALSE;
    }
    pImpHead->bUpscalingAllowedV = scalingUsageBounds.vUpscalingAllowed;
    pImpHead->maxDownscaleFactorV = scalingUsageBounds.maxVDownscaleFactor;
    pImpHead->maxDownscaleFactorH = scalingUsageBounds.maxHDownscaleFactor;
    pImpHead->outputScalerVerticalTaps =
        NVEvoScalerTapsToNum(scalingUsageBounds.vTaps);

    if (!nvComputeMinFrameIdle(pTimings,
                             &pImpHead->minFrameIdle.leadingRasterLines,
                             &pImpHead->minFrameIdle.trailingRasterLines)) {
        return FALSE;
    }

    /* Assume we'll need the full 1025-entry output LUT. */
    pImpHead->lut = NVC372_CTRL_IMP_LUT_USAGE_1025;

    /* Cursor width, in units of 32 pixels.  Assume we use the maximum size. */
    pImpHead->cursorSize32p = 256 / 32;

    pImpHead->bEnableDsc = enableDsc;

    pImpHead->bIs2Head1Or = b2Heads1Or;

    pImpHead->bYUV420Format =
        (pTimings->yuv420Mode == NV_YUV420_MODE_HW);

    return TRUE;
}

/*!
 * Initialize window-specific IMP param fields.
 *
 * Initialize the NVC372_CTRL_IMP_WINDOW for the specific window.
 *
 * \param[out]  pImpWindow          The param structure to initialize.
 * \param[in]   pViewPort           The viewport configuration for the head that
 *                                  the window is bound to.
 * \param[in]   supportedFormats    The surface memory formats that can be
 *                                  supported on this window.
 * \param[in]   window              The number of the window.
 * \param[in]   head                The number of the head that the window is
 *                                  bound to.
 */
static void AssignPerWindowImpParams(NVC372_CTRL_IMP_WINDOW *pImpWindow,
                                const NVHwModeViewPortEvo *pViewPort,
                                const NvU64 supportedFormats,
                                const struct NvKmsScalingUsageBounds *pScaling,
                                const int window,
                                const int head)
{
    pImpWindow->windowIndex = window;
    pImpWindow->owningHead = head;

    pImpWindow->formatUsageBound = 0;
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED1BPP) {
        pImpWindow->formatUsageBound |= NVC372_CTRL_FORMAT_RGB_PACKED_1_BPP;
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED2BPP) {
        pImpWindow->formatUsageBound |= NVC372_CTRL_FORMAT_RGB_PACKED_2_BPP;
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED4BPP) {
        pImpWindow->formatUsageBound |= NVC372_CTRL_FORMAT_RGB_PACKED_4_BPP;
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_RGB_PACKED8BPP) {
        pImpWindow->formatUsageBound |= NVC372_CTRL_FORMAT_RGB_PACKED_8_BPP;
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_YUV_PACKED422) {
        pImpWindow->formatUsageBound |= NVC372_CTRL_FORMAT_YUV_PACKED_422;
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_YUV_SP420) {
        pImpWindow->formatUsageBound |= NVC372_CTRL_FORMAT_YUV_SEMI_PLANAR_420;
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_YUV_SP422) {
        pImpWindow->formatUsageBound |= NVC372_CTRL_FORMAT_YUV_SEMI_PLANAR_422;
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_YUV_SP444) {
        pImpWindow->formatUsageBound |= NVC372_CTRL_FORMAT_YUV_SEMI_PLANAR_444;
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_EXT_YUV_SP420) {
        pImpWindow->formatUsageBound |=
            NVC372_CTRL_FORMAT_EXT_YUV_SEMI_PLANAR_420;
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_EXT_YUV_SP422) {
        pImpWindow->formatUsageBound |=
            NVC372_CTRL_FORMAT_EXT_YUV_SEMI_PLANAR_422;
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_EXT_YUV_SP444) {
        pImpWindow->formatUsageBound |=
            NVC372_CTRL_FORMAT_EXT_YUV_SEMI_PLANAR_444;
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_YUV_PLANAR444) {
        pImpWindow->formatUsageBound |=
            NVC372_CTRL_FORMAT_YUV_PLANAR_444;
    }
    if (supportedFormats & NVKMS_SURFACE_MEMORY_FORMATS_YUV_PLANAR420) {
        pImpWindow->formatUsageBound |=
            NVC372_CTRL_FORMAT_YUV_PLANAR_420;
    }

    if (pImpWindow->formatUsageBound == 0) {
        nvAssert(!"Unknown format in AssignPerWindowImpParams");
    }

    pImpWindow->maxPixelsFetchedPerLine =
        nvGetMaxPixelsFetchedPerLine(pViewPort->in.width,
                                   pScaling->maxHDownscaleFactor);

    pImpWindow->maxDownscaleFactorH = pScaling->maxHDownscaleFactor;
    pImpWindow->maxDownscaleFactorV = pScaling->maxVDownscaleFactor;
    pImpWindow->bUpscalingAllowedV = pScaling->vUpscalingAllowed;
    pImpWindow->inputScalerVerticalTaps =
        NVEvoScalerTapsToNum(pScaling->vTaps);

    /* Assume we need a full 1025-entry window (input) and tone-mapping
     * output (TMO) LUT. */
    pImpWindow->lut = NVC372_CTRL_IMP_LUT_USAGE_1025;
    pImpWindow->tmoLut = NVC372_CTRL_IMP_LUT_USAGE_1025;
}

NvBool
nvEvoSetCtrlIsModePossibleParams3(NVDispEvoPtr pDispEvo,
                                  const NVEvoIsModePossibleDispInput *pInput,
                                  NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS *pImp)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    const NVEvoCapabilitiesPtr pEvoCaps = &pDevEvo->gpus[0].capabilities;
    NvU32 head;

    nvkms_memset(pImp, 0, sizeof(*pImp));

    for (head = 0; head < NVKMS_MAX_HEADS_PER_DISP; head++) {
        const NVHwModeTimingsEvo *pTimings = pInput->head[head].pTimings;
        const NvU32 enableDsc = pInput->head[head].enableDsc;
        const NvBool b2Heads1Or = pInput->head[head].b2Heads1Or;
        const struct NvKmsUsageBounds *pUsage = pInput->head[head].pUsage;
        const NVHwModeViewPortEvo *pViewPort;
        NvU8 impHeadIndex;
        NvU32 layer;

        if (pTimings == NULL) {
            continue;
        }

        pViewPort = &pTimings->viewPort;

        impHeadIndex = pImp->numHeads;
        pImp->numHeads++;
        nvAssert(impHeadIndex < NVC372_CTRL_MAX_POSSIBLE_HEADS);

        if (!AssignPerHeadImpParams(&pImp->head[impHeadIndex],
                                    pTimings,
                                    enableDsc,
                                    b2Heads1Or,
                                    head,
                                    &pEvoCaps->head[head].scalerCaps)) {
            return FALSE;
        }

        /* XXXnvdisplay: This assumes a fixed window<->head mapping */
        for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
            if (!pUsage->layer[layer].usable) {
                continue;
            }

            nvAssert(pImp->numWindows < NVC372_CTRL_MAX_POSSIBLE_WINDOWS);

            AssignPerWindowImpParams(
                &pImp->window[pImp->numWindows],
                pViewPort,
                pUsage->layer[layer].supportedSurfaceMemoryFormats,
                &pUsage->layer[layer].scaling,
                NV_EVO_CHANNEL_MASK_WINDOW_NUMBER(
                    pDevEvo->head[head].layer[layer]->channelMask),
                head);

            pImp->numWindows++;
        }
    }

    pImp->base.subdeviceIndex = pDispEvo->displayOwner;

    /* XXXnvdisplay: Set bUseCachedPerfState? */

    /*
     * Set NEED_MIN_VPSTATE if reallocBandwidth != NONE. RM-IMP will only
     * output the min required display bandwidth values if NEED_MIN_VPSTATE
     * is set.
     */
    if (pInput->requireBootClocks ||
        (pInput->reallocBandwidth != NV_EVO_REALLOCATE_BANDWIDTH_MODE_NONE)) {
        // XXX TODO: IMP requires lock pin information if pstate information is
        // requested. For now, just assume no locking.
        pImp->options = NVC372_CTRL_IS_MODE_POSSIBLE_OPTIONS_NEED_MIN_VPSTATE;
    }

    return TRUE;
}

void
nvEvoSetIsModePossibleDispOutput3(const NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS *pImp,
                                  const NvBool result,
                                  NVEvoIsModePossibleDispOutput *pOutput)
{
    pOutput->possible = result;
    if (pOutput->possible) {
        pOutput->minRequiredBandwidthKBPS = pImp->minRequiredBandwidthKBPS;
        pOutput->floorBandwidthKBPS = pImp->floorBandwidthKBPS;
    }
}

void
nvEvoIsModePossibleC3(NVDispEvoPtr pDispEvo,
                    const NVEvoIsModePossibleDispInput *pInput,
                    NVEvoIsModePossibleDispOutput *pOutput)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS *pImp =
        nvPreallocGet(pDevEvo, PREALLOC_TYPE_IMP_PARAMS, sizeof(*pImp));
    NvBool result = FALSE;
    NvU32 ret;

    if (!nvEvoSetCtrlIsModePossibleParams3(pDispEvo, pInput, pImp)) {
        goto done;
    }

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->rmCtrlHandle,
                         NVC372_CTRL_CMD_IS_MODE_POSSIBLE,
                         pImp, sizeof(*pImp));

    // XXXnvdisplay TODO: check pImp->minImpVPState if
    // pInput->requireBootClocks is true?
    if (ret != NV_OK || !pImp->bIsPossible) {
        goto done;
    }

    result = TRUE;

done:
    for (NvU32 head = 0; head < pDevEvo->numHeads; head++) {
        pOutput->head[head].dscSliceCount = pInput->head[head].dscSliceCount;
    }

    nvEvoSetIsModePossibleDispOutput3(pImp, result, pOutput);

    nvPreallocRelease(pDevEvo, PREALLOC_TYPE_IMP_PARAMS);
}

void nvEvoPrePostIMPC3(NVDispEvoPtr pDispEvo, NvBool isPre)
{
    /* Nothing to do on nvdisplay -- pre/post IMP calls are not required. */
}

static void
EvoFlipC3(NVDevEvoPtr pDevEvo,
          NVEvoChannelPtr pChannel,
          const NVFlipChannelEvoHwState *pHwState,
          NVEvoUpdateState *updateState,
          NvBool bypassComposition);

/*
 * Returns TRUE iff the CSC should be enabled (i.e., the matrix is not the
 * identity matrix).
 */
static NvBool SetCscMatrixC3(NVEvoChannelPtr pChannel,
                             const struct NvKmsCscMatrix *matrix)
{
    NvU32 method = NVC37E_SET_CSC_RED2RED;
    int y;

    if (nvIsCscMatrixIdentity(matrix)) {
        return FALSE;
    }

    for (y = 0; y < 3; y++) {
        int x;

        for (x = 0; x < 4; x++) {
            // Use DRF_NUM to truncate client-supplied values that are out of
            // range.
            NvU32 val = DRF_NUM(C37E, _SET_CSC_RED2RED, _COEFF,
                                matrix->m[y][x]);

            nvDmaSetStartEvoMethod(pChannel, method, 1);
            nvDmaSetEvoMethodData(pChannel, val);

            method += 4;
        }
    }

    return TRUE;
}

static void SetCscMatrixC5Wrapper(NVEvoChannelPtr pChannel,
                                  const struct NvKmsCscMatrix *matrix,
                                  NvU32 coeffMethod, NvU32 controlMethod,
                                  NvU32 enableMethodData,
                                  NvU32 disableMethodData)
{
    int y;

    if (nvIsCscMatrixIdentity(matrix)) {
        nvDmaSetStartEvoMethod(pChannel, controlMethod, 1);
        nvDmaSetEvoMethodData(pChannel, disableMethodData);
        return;
    }

    nvDmaSetStartEvoMethod(pChannel, controlMethod, 1);
    nvDmaSetEvoMethodData(pChannel, enableMethodData);

    for (y = 0; y < 3; y++) {
        int x;

        for (x = 0; x < 4; x++) {
            // Use DRF_NUM to truncate client-supplied values that are out of
            // range.
            //
            // Note that it doesn't matter whether we use the CSC00 or CSC11
            // methods to truncate since they're identical.
            NvU32 val = DRF_NUM(C57E, _SET_CSC00COEFFICIENT_C00, _VALUE,
                                matrix->m[y][x]);

            nvDmaSetStartEvoMethod(pChannel, coeffMethod, 1);
            nvDmaSetEvoMethodData(pChannel, val);

            coeffMethod += 4;
        }
    }
}

static void SetCsc00MatrixC5(NVEvoChannelPtr pChannel,
                             const struct NvKmsCscMatrix *matrix)
{
    SetCscMatrixC5Wrapper(pChannel,
                    matrix,
                    NVC57E_SET_CSC00COEFFICIENT_C00, NVC57E_SET_CSC00CONTROL,
                    DRF_DEF(C57E, _SET_CSC00CONTROL, _ENABLE, _ENABLE),
                    DRF_DEF(C57E, _SET_CSC00CONTROL, _ENABLE, _DISABLE));
}

static void SetCsc01MatrixC5(NVEvoChannelPtr pChannel,
                             const struct NvKmsCscMatrix *matrix)
{
    SetCscMatrixC5Wrapper(pChannel,
                    matrix,
                    NVC57E_SET_CSC01COEFFICIENT_C00, NVC57E_SET_CSC01CONTROL,
                    DRF_DEF(C57E, _SET_CSC01CONTROL, _ENABLE, _ENABLE),
                    DRF_DEF(C57E, _SET_CSC01CONTROL, _ENABLE, _DISABLE));
}

static void SetCsc10MatrixC5(NVEvoChannelPtr pChannel,
                             const struct NvKmsCscMatrix *matrix)
{
    SetCscMatrixC5Wrapper(pChannel,
                    matrix,
                    NVC57E_SET_CSC10COEFFICIENT_C00, NVC57E_SET_CSC10CONTROL,
                    DRF_DEF(C57E, _SET_CSC10CONTROL, _ENABLE, _ENABLE),
                    DRF_DEF(C57E, _SET_CSC10CONTROL, _ENABLE, _DISABLE));
}

static void SetCsc11MatrixC5(NVEvoChannelPtr pChannel,
                             const struct NvKmsCscMatrix *matrix)
{
    SetCscMatrixC5Wrapper(pChannel,
                    matrix,
                    NVC57E_SET_CSC11COEFFICIENT_C00, NVC57E_SET_CSC11CONTROL,
                    DRF_DEF(C57E, _SET_CSC11CONTROL, _ENABLE, _ENABLE),
                    DRF_DEF(C57E, _SET_CSC11CONTROL, _ENABLE, _DISABLE));
}

/*
 * WAR for GV100 HW bug 1978592:
 *
 * Timestamped flips allow SW to specify the earliest time that the next UPDATE
 * will complete.  Due to a HW bug, GV100 waits for the timestamp in the ARMED
 * state (i.e. the timestamps that were pushed in the previous UPDATE) instead
 * of the timestamp in the ASSEMBLY state (the time we want to postpone this
 * flip until).
 *
 * This WAR inserts an additional UPDATE to push the timestamp from ASSEMBLY to
 * ARMED while changing no other state, so the following normal UPDATE can
 * wait for the correct timestamp.
 *
 * This update needs to have the following characteristics:
 *
 * - MIN_PRESENT_INTERVAL 0
 * - TIMESTAMP_MODE       _ENABLE
 * - All other SET_PRESENT_CONTROL fields unmodified from previous UPDATE
 * - SET_UPDATE_TIMESTAMP (target timestamp)
 * - RELEASE_ELV _FALSE
 * - Non-interlocked
 * - Non-fliplocked
 */
static void
InsertAdditionalTimestampFlip(NVDevEvoPtr pDevEvo,
                              NVEvoChannelPtr pChannel,
                              const NVFlipChannelEvoHwState *pHwState,
                              NVEvoUpdateState *updateState)
{
    NvU32 presentControl = pChannel->oldPresentControl;

    /* This hardware bug is only present on GV100 which uses window
     * class C37E. */
    nvAssert(pChannel->hwclass == NVC37E_WINDOW_CHANNEL_DMA);

    nvAssert(pHwState->timeStamp != 0);

    /*
     * Update the necessary fields in SET_PRESENT_CONTROL without modifying
     * the existing values by using the cached SET_PRESENT_CONTROL values
     * from the previous update.
     *
     * Note that BEGIN_MODE must not be changed here; even though BEGIN_MODE
     * may currently be NON_TEARING, a NON_TEARING + MIN_PRESENT_INTERVAL 0
     * flip will be correctly collapsed with the surrounding
     * MIN_PRESENT_INTERVAL 1 flips.  If we were to change BEGIN_MODE to
     * IMMEDIATE, this would cause an additional delay due to the transition
     * from NON_TEARING to IMMEDIATE.
     */
    presentControl = FLD_SET_DRF_NUM(C37E, _SET_PRESENT_CONTROL,
                                     _MIN_PRESENT_INTERVAL,
                                     0, presentControl);
    presentControl = FLD_SET_DRF(C37E, _SET_PRESENT_CONTROL,
                                 _TIMESTAMP_MODE,
                                 _ENABLE, presentControl);

    nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_PRESENT_CONTROL, 1);
    nvDmaSetEvoMethodData(pChannel, presentControl);

    nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_UPDATE_TIMESTAMP_LO, 2);
    nvDmaSetEvoMethodData(pChannel, NvU64_LO32(pHwState->timeStamp));
    nvDmaSetEvoMethodData(pChannel, NvU64_HI32(pHwState->timeStamp));

    // Issue non-interlocked, non-fliplocked, non-ReleaseElv UPDATE
    nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_INTERLOCK_FLAGS, 1);
    nvDmaSetEvoMethodData(pChannel, 0);

    nvDmaSetStartEvoMethod(pChannel,
                           NVC37E_SET_WINDOW_INTERLOCK_FLAGS,
                           1);
    nvDmaSetEvoMethodData(pChannel, 0);

    nvDmaSetStartEvoMethod(pChannel, NVC37E_UPDATE, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_DEF(C37E, _UPDATE, _RELEASE_ELV, _FALSE) |
        DRF_NUM(C37E, _UPDATE, _FLIP_LOCK_PIN,
            NVC37E_UPDATE_FLIP_LOCK_PIN_LOCK_PIN_NONE) |
        DRF_DEF(C37E, _UPDATE, _INTERLOCK_WITH_WIN_IMM,
            _DISABLE));
}

static void
EvoProgramSemaphore3(NVDevEvoPtr pDevEvo,
                     NVEvoChannelPtr pChannel,
                     const NVFlipChannelEvoHwState *pHwState)
{
    nvAssertSameSemaphoreSurface(pHwState);

    if (pHwState->syncObject.u.semaphores.acquireSurface.pSurfaceEvo == NULL) {
        nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_CONTEXT_DMA_SEMAPHORE, 1);
        nvDmaSetEvoMethodData(pChannel, 0);
        nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_SEMAPHORE_CONTROL, 1);
        nvDmaSetEvoMethodData(pChannel, 0);
        nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_SEMAPHORE_ACQUIRE, 1);
        nvDmaSetEvoMethodData(pChannel, 0);
        nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_SEMAPHORE_RELEASE, 1);
        nvDmaSetEvoMethodData(pChannel, 0);
    } else {
        const NVFlipNIsoSurfaceEvoHwState *pNIso =
            &pHwState->syncObject.u.semaphores.acquireSurface;

        nvAssert(pNIso->format == NVKMS_NISO_FORMAT_FOUR_WORD_NVDISPLAY);
        /* XXX nvdisplay: enforce this at a higher level */
        nvAssert((pNIso->offsetInWords % 4) == 0);

        nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_CONTEXT_DMA_SEMAPHORE, 1);
        nvDmaSetEvoMethodData(pChannel, pNIso->pSurfaceEvo->planes[0].surfaceDesc.ctxDmaHandle);

        nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_SEMAPHORE_ACQUIRE, 1);
        nvDmaSetEvoMethodData(pChannel,
            pHwState->syncObject.u.semaphores.acquireValue);

        nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_SEMAPHORE_RELEASE, 1);
        nvDmaSetEvoMethodData(pChannel,
            pHwState->syncObject.u.semaphores.releaseValue);

        nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_SEMAPHORE_CONTROL, 1);
        nvDmaSetEvoMethodData(pChannel, DRF_NUM(C37E, _SET_SEMAPHORE_CONTROL, _OFFSET,
                                         pNIso->offsetInWords / 4));
    }
}

static void EvoSetSemaphoreSurfaceAddressAndControlC6(
    const NVDevEvoRec *pDevEvo,
    NVEvoChannelPtr pChannel,
    const NVSurfaceDescriptor *pSurfaceDesc,
    NvU32 semaphoreOffset,
    NvU32 ctrlVal)
{
    NvU32 ctxDmaHandle = pSurfaceDesc ? pSurfaceDesc->ctxDmaHandle : 0;

    /*! set ctx dma handle */
    nvDmaSetStartEvoMethod(pChannel, NVC67E_SET_CONTEXT_DMA_SEMAPHORE, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C67E, _SET_CONTEXT_DMA_SEMAPHORE, _HANDLE, ctxDmaHandle));

    /*! set semaphore control and acq-rel mode */
    nvDmaSetStartEvoMethod(pChannel, NVC67E_SET_SEMAPHORE_CONTROL, 1);
    nvDmaSetEvoMethodData(pChannel, semaphoreOffset | ctrlVal);
}

static void EvoSetAcqSemaphoreSurfaceAddressAndControlC6(
    const NVDevEvoRec *pDevEvo,
    NVEvoChannelPtr pChannel,
    const NVSurfaceDescriptor *pSurfaceDesc,
    NvU32 semaphoreOffset,
    NvU32 ctrlVal)
{
    NvU32 ctxDmaHandle = pSurfaceDesc ? pSurfaceDesc->ctxDmaHandle : 0;

    /*! set ctx dma handle */
    nvDmaSetStartEvoMethod(pChannel, NVC67E_SET_CONTEXT_DMA_ACQ_SEMAPHORE, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C67E, _SET_CONTEXT_DMA_ACQ, _SEMAPHORE_HANDLE, ctxDmaHandle));

    /*! set semaphore control and acq mode */
    nvDmaSetStartEvoMethod(pChannel, NVC67E_SET_ACQ_SEMAPHORE_CONTROL, 1);
    nvDmaSetEvoMethodData(pChannel, semaphoreOffset | ctrlVal);
}

/*!
 * On Tegra, syncpts are used for synchronization between SW and HW,
 * and also across HW engines. Since NvDisplay 4.0 only natively
 * understands semaphores, there's a SHIM layer in the memory subsystem
 * that will convert semaphore acquires/releases into corresponding
 * syncpoint reads/writes. As such, each syncpoint is mapped to an
 * underlying 'dummy' semaphore surface, and the methods for these surfaces
 * need to be programmed as if they were real memory-backed semaphores.
 */

static void
EvoProgramSemaphore6(NVDevEvoPtr pDevEvo,
                     NVEvoChannelPtr pChannel,
                     const NVFlipChannelEvoHwState *pHwState)
{
    NvU32 offset, acqMode, relMode, value;
    const NVSurfaceDescriptor *pSurfaceDesc = NULL;
    const NVFlipNIsoSurfaceEvoHwState *pNIso;

    /*! Program Acq-only semaphore */
    pSurfaceDesc = NULL;
    offset = acqMode = relMode = value = 0;
    if (pHwState->syncObject.usingSyncpt &&
        pHwState->syncObject.u.syncpts.isPreSyncptSpecified) {
        NvU32 id = pHwState->syncObject.u.syncpts.preSyncpt;
        pSurfaceDesc = &pDevEvo->preSyncptTable[id].surfaceDesc;
        acqMode = DRF_DEF(C67E, _SET_ACQ_SEMAPHORE_CONTROL, _ACQ_MODE, _CGEQ);
        value = pHwState->syncObject.u.syncpts.preValue;
    } else {
        if (pHwState->syncObject.u.semaphores.acquireSurface.pSurfaceEvo != NULL) {
            pNIso = &pHwState->syncObject.u.semaphores.acquireSurface;
            pSurfaceDesc = &pNIso->pSurfaceEvo->planes[0].surfaceDesc;
            offset = pNIso->offsetInWords / 4;
            acqMode = DRF_DEF(C67E, _SET_ACQ_SEMAPHORE_CONTROL, _ACQ_MODE, _EQ);
            value = pHwState->syncObject.u.semaphores.acquireValue;
        }
    }

    pDevEvo->hal->SetAcqSemaphoreSurfaceAddressAndControl(pDevEvo, pChannel,
        pSurfaceDesc, offset, acqMode);

    /*! set semaphore value */
    nvDmaSetStartEvoMethod(pChannel, NVC67E_SET_ACQ_SEMAPHORE_VALUE, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C67E, _SET_ACQ_SEMAPHORE_VALUE, _VALUE, value));

    /*! Program Rel-only semaphore */
    pSurfaceDesc = NULL;
    offset = acqMode = relMode = value = 0;
    if (pHwState->syncObject.usingSyncpt &&
        pHwState->syncObject.u.syncpts.isPostSyncptSpecified) {
        pSurfaceDesc = &pHwState->syncObject.u.syncpts.surfaceDesc;
        acqMode = DRF_DEF(C67E, _SET_SEMAPHORE_CONTROL, _SKIP_ACQ, _TRUE);
        relMode = DRF_DEF(C67E, _SET_SEMAPHORE_CONTROL, _REL_MODE, _WRITE);
        value = pHwState->syncObject.u.syncpts.postValue;
        /*! increase local max val as well */
        pChannel->postSyncpt.syncptMaxVal++;
    } else {
        if (pHwState->syncObject.u.semaphores.releaseSurface.pSurfaceEvo != NULL) {
            pNIso = &pHwState->syncObject.u.semaphores.releaseSurface;
            pSurfaceDesc = &pNIso->pSurfaceEvo->planes[0].surfaceDesc;
            offset = pNIso->offsetInWords / 4;
            acqMode = DRF_DEF(C67E, _SET_SEMAPHORE_CONTROL, _SKIP_ACQ, _TRUE);
            relMode = DRF_DEF(C67E, _SET_SEMAPHORE_CONTROL, _REL_MODE, _WRITE);
            value = pHwState->syncObject.u.semaphores.releaseValue;
        }
    }

    pDevEvo->hal->SetSemaphoreSurfaceAddressAndControl(pDevEvo, pChannel,
        pSurfaceDesc, offset, (acqMode | relMode));

    /*! set semaphore value */
    nvDmaSetStartEvoMethod(pChannel, NVC67E_SET_SEMAPHORE_RELEASE, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C67E, _SET_SEMAPHORE_RELEASE, _VALUE, value));
}

static void EvoSetWinNotifierSurfaceAddressAndControlC3(
    const NVDevEvoRec *pDevEvo,
    NVEvoChannelPtr pChannel,
    const NVSurfaceDescriptor *pSurfaceDesc,
    NvU32 notifierOffset,
    NvU32 ctrlVal)
{
    NvU32 ctxDmaHandle = pSurfaceDesc ? pSurfaceDesc->ctxDmaHandle : 0;

    nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_CONTEXT_DMA_NOTIFIER, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37E, _SET_CONTEXT_DMA_NOTIFIER, _HANDLE, ctxDmaHandle));

    nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_NOTIFIER_CONTROL, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37E, _SET_NOTIFIER_CONTROL, _OFFSET, notifierOffset) | ctrlVal);
}

static void EvoSetISOSurfaceAddressC3(
    const NVDevEvoRec *pDevEvo,
    NVEvoChannelPtr pChannel,
    const NVSurfaceDescriptor *pSurfaceDesc,
    NvU32 offset,
    NvU32 ctxDmaIdx,
    NvBool isBlocklinear)
{
    NvU32 ctxDmaHandle = pSurfaceDesc ? pSurfaceDesc->ctxDmaHandle : 0;

    nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_CONTEXT_DMA_ISO(ctxDmaIdx), 1);
    nvDmaSetEvoMethodData(pChannel, ctxDmaHandle);

    nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_OFFSET(ctxDmaIdx), 1);
    nvDmaSetEvoMethodData(pChannel, nvCtxDmaOffsetFromBytes(offset));
}

static NvBool
EvoFlipC3Common(NVDevEvoPtr pDevEvo,
         NVEvoChannelPtr pChannel,
         const NVFlipChannelEvoHwState *pHwState,
         NVEvoUpdateState *updateState)
{
    const NvKmsSurfaceMemoryFormatInfo *pFormatInfo;
    NvU32 presentControl, eye;
    NvU32 storage;
    NvU8 planeIndex;
    NVSurfaceDescriptor *pSurfaceDesc = NULL;
    NvU32 offset, ctrlVal;

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    /* program notifier */

    if (pHwState->completionNotifier.surface.pSurfaceEvo == NULL) {
        offset = ctrlVal = 0;
        pDevEvo->hal->SetWinNotifierSurfaceAddressAndControl(pDevEvo,
            pChannel, NULL, offset, ctrlVal);
    } else {
        const NVFlipNIsoSurfaceEvoHwState *pNIso =
            &pHwState->completionNotifier.surface;

        nvAssert(pNIso->format == NVKMS_NISO_FORMAT_FOUR_WORD_NVDISPLAY);
        /* XXX nvdisplay: enforce this at a higher level */
        nvAssert((pNIso->offsetInWords % 4) == 0);

        pSurfaceDesc = &pNIso->pSurfaceEvo->planes[0].surfaceDesc;
        offset = pNIso->offsetInWords / 4;
        ctrlVal = 0;
        if (pHwState->completionNotifier.awaken) {
            ctrlVal = FLD_SET_DRF(C37E, _SET_NOTIFIER_CONTROL, _MODE,
                                  _WRITE_AWAKEN, ctrlVal);
        } else {
            ctrlVal = FLD_SET_DRF(C37E, _SET_NOTIFIER_CONTROL, _MODE,
                                  _WRITE, ctrlVal);
        }

        pDevEvo->hal->SetWinNotifierSurfaceAddressAndControl(pDevEvo,
            pChannel, pSurfaceDesc, offset, ctrlVal);
    }

    if (!pHwState->pSurfaceEvo[NVKMS_LEFT]) {
        // Disable this window, and set all its ctxdma entries to NULL.
        for (eye = 0; eye < NVKMS_MAX_EYES; eye++) {
            for (planeIndex = 0;
                 planeIndex < NVKMS_MAX_PLANES_PER_SURFACE;
                 planeIndex++) {
                const NvU8 ctxDmaIdx = EyeAndPlaneToCtxDmaIdx(eye, planeIndex);
                pDevEvo->hal->SetISOSurfaceAddress(pDevEvo, pChannel,
                    NULL /* pSurfaceDec */, 0 /* offset */, ctxDmaIdx,
                    NV_FALSE /* isBlocklinear */);
            }
        }

        return FALSE;
    }

    presentControl = DRF_NUM(C37E, _SET_PRESENT_CONTROL, _MIN_PRESENT_INTERVAL,
                             pHwState->minPresentInterval);

    if (pHwState->timeStamp != 0) {
        presentControl = FLD_SET_DRF(C37E, _SET_PRESENT_CONTROL, _TIMESTAMP_MODE,
                                     _ENABLE, presentControl);
    } else {
        presentControl = FLD_SET_DRF(C37E, _SET_PRESENT_CONTROL, _TIMESTAMP_MODE,
                                     _DISABLE, presentControl);
    }

    if (pHwState->tearing) {
        presentControl = FLD_SET_DRF(C37E, _SET_PRESENT_CONTROL, _BEGIN_MODE,
                                     _IMMEDIATE, presentControl);
    } else {
        presentControl = FLD_SET_DRF(C37E, _SET_PRESENT_CONTROL, _BEGIN_MODE,
                                     _NON_TEARING, presentControl);
    }

    if (pHwState->pSurfaceEvo[NVKMS_RIGHT]) {
        if (pHwState->perEyeStereoFlip) {
            presentControl = FLD_SET_DRF(C37E, _SET_PRESENT_CONTROL, _STEREO_MODE,
                                         _AT_ANY_FRAME, presentControl);
        } else {
            presentControl = FLD_SET_DRF(C37E, _SET_PRESENT_CONTROL, _STEREO_MODE,
                                         _PAIR_FLIP, presentControl);
        }
    } else {
        presentControl = FLD_SET_DRF(C37E, _SET_PRESENT_CONTROL, _STEREO_MODE,
                                     _MONO, presentControl);
    }
    nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_PRESENT_CONTROL, 1);
    nvDmaSetEvoMethodData(pChannel, presentControl);

    /*
     * GV100 timestamped flips need a duplicate update which only changes
     * TIMESTAMP_MODE and MIN_PRESENT_INTERVAL fields in SET_PRESENT_CONTROL;
     * to allow updating these fields without changing anything else in
     * SET_PRESENT_CONTROL, cache the values we sent in previous flips here.
     * (bug 1990958)
     */
    pChannel->oldPresentControl = presentControl;

    /* Set the surface parameters. */
    FOR_ALL_EYES(eye) {
        const NVSurfaceEvoRec *pSurfaceEvoPerEye = pHwState->pSurfaceEvo[eye];
        NvU8 numSurfacePlanes = 0;
        NvBool isBlockLinear = NV_FALSE;

        if (pSurfaceEvoPerEye != NULL) {
            pFormatInfo =
                nvKmsGetSurfaceMemoryFormatInfo(pSurfaceEvoPerEye->format);
            numSurfacePlanes = pFormatInfo->numPlanes;
            isBlockLinear =
                (pSurfaceEvoPerEye->layout == NvKmsSurfaceMemoryLayoutBlockLinear);
        }

        for (planeIndex = 0;
             planeIndex < NVKMS_MAX_PLANES_PER_SURFACE;
             planeIndex++) {
            const NVSurfaceDescriptor *pSurfaceDesc = NULL;
            NvU64 offset = 0;
            const NvU8 ctxDmaIdx = EyeAndPlaneToCtxDmaIdx(eye, planeIndex);

            if (planeIndex < numSurfacePlanes) {
                pSurfaceDesc = &pSurfaceEvoPerEye->planes[planeIndex].surfaceDesc;
                offset = pSurfaceEvoPerEye->planes[planeIndex].offset;
            }

            pDevEvo->hal->SetISOSurfaceAddress(pDevEvo, pChannel,
                pSurfaceDesc, offset, ctxDmaIdx, isBlockLinear);
        }
    }

    nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_SIZE, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37E, _SET_SIZE, _WIDTH, pHwState->pSurfaceEvo[NVKMS_LEFT]->widthInPixels) |
        DRF_NUM(C37E, _SET_SIZE, _HEIGHT, pHwState->pSurfaceEvo[NVKMS_LEFT]->heightInPixels));

    nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_SIZE_IN, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37E, _SET_SIZE_IN, _WIDTH, pHwState->sizeIn.width) |
        DRF_NUM(C37E, _SET_SIZE_IN, _HEIGHT, pHwState->sizeIn.height));

    nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_SIZE_OUT, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37E, _SET_SIZE_OUT, _WIDTH, pHwState->sizeOut.width) |
        DRF_NUM(C37E, _SET_SIZE_OUT, _HEIGHT, pHwState->sizeOut.height));

    /* XXX nvdisplay: enforce pitch/BL layout are consistent between eyes at a
     * higher level */

    storage = 0;
    if (pHwState->pSurfaceEvo[NVKMS_LEFT]->layout ==
        NvKmsSurfaceMemoryLayoutBlockLinear) {
        const NvU32 blockHeight = pHwState->pSurfaceEvo[NVKMS_LEFT]->log2GobsPerBlockY;
        storage |= DRF_NUM(C37E, _SET_STORAGE, _BLOCK_HEIGHT, blockHeight);
        if (pDevEvo->hal->caps.supportsSetStorageMemoryLayout) {
            storage |= DRF_DEF(C37E, _SET_STORAGE, _MEMORY_LAYOUT, _BLOCKLINEAR);
        }
    } else if (pDevEvo->hal->caps.supportsSetStorageMemoryLayout) {
        storage |= DRF_DEF(C37E, _SET_STORAGE, _MEMORY_LAYOUT, _PITCH);
    }
    nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_STORAGE, 1);
    nvDmaSetEvoMethodData(pChannel, storage);

    pFormatInfo = nvKmsGetSurfaceMemoryFormatInfo(
                    pHwState->pSurfaceEvo[NVKMS_LEFT]->format);

    for (planeIndex = 0;
         planeIndex < NVKMS_MAX_PLANES_PER_SURFACE;
         planeIndex++) {
        NvU32 pitch;

        nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_PLANAR_STORAGE(planeIndex),
                               1);

        if (planeIndex >= pFormatInfo->numPlanes) {
            nvDmaSetEvoMethodData(pChannel,
                DRF_NUM(C37E, _SET_PLANAR_STORAGE, _PITCH, 0));
            continue;
        }

        /*
         * Per nvdClass_01.mfs, the HEAD_SET_STORAGE_PITCH "units are blocks
         * if the layout is BLOCKLINEAR, the units are multiples of 64 bytes
         * if the layout is PITCH."
         */
        pitch = pHwState->pSurfaceEvo[NVKMS_LEFT]->planes[planeIndex].pitch;
        if (pHwState->pSurfaceEvo[NVKMS_LEFT]->layout ==
            NvKmsSurfaceMemoryLayoutBlockLinear) {
            /* pitch is already in units of blocks; no conversion needed. */
            nvDmaSetEvoMethodData(pChannel,
                DRF_NUM(C37E, _SET_PLANAR_STORAGE, _PITCH, pitch));
        } else {
            /* XXX nvdisplay: enforce this at a higher level */
            nvAssert((pitch & 63) == 0);
            nvDmaSetEvoMethodData(pChannel,
                DRF_NUM(C37E, _SET_PLANAR_STORAGE, _PITCH, pitch >> 6));
        }
    }

    ASSERT_EYES_MATCH(pHwState->pSurfaceEvo, format);

    nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_UPDATE_TIMESTAMP_LO, 2);
    nvDmaSetEvoMethodData(pChannel, NvU64_LO32(pHwState->timeStamp));
    nvDmaSetEvoMethodData(pChannel, NvU64_HI32(pHwState->timeStamp));

    return TRUE;
}

/*
 * This function returns TRUE if precomp needs to swap the U and V components to
 * support the given input surface format. For all such formats,
 * SetParams.SwapUV needs to be enabled.
 *
 * Due to the "feature" described in bug 1640117, there's a mismatch in the
 * ihub<->precomp interface:
 * - For all Yx___UxVx_N444 and Yx___UxVx_N422 formats, ihub will fetch and send
 *   the V sample as the first chroma byte, and the U sample as the second byte.
 *   However, precomp expects the U sample as the first byte, and the V sample
 *   as the second byte.
 * - For all Yx___VxUx_N420 formats, ihub will fetch and send the U sample as
 *   the first chroma byte, and the V sample as the second byte.
 *   However, precomp expects the V sample as the first byte, and the U sample
 *   as the second byte.
 *
 * In the above explanation, note that ihub simply fetches and sends the chroma
 * bytes in the same order that they're packed in memory.
 */
static NvBool IsSurfaceFormatUVSwapped(
    const enum NvKmsSurfaceMemoryFormat format)
{
    switch (format) {
    case NvKmsSurfaceMemoryFormatY8___U8V8_N444:
    case NvKmsSurfaceMemoryFormatY8___U8V8_N422:
    case NvKmsSurfaceMemoryFormatY8___V8U8_N420:
    case NvKmsSurfaceMemoryFormatY10___U10V10_N444:
    case NvKmsSurfaceMemoryFormatY10___U10V10_N422:
    case NvKmsSurfaceMemoryFormatY10___V10U10_N420:
    case NvKmsSurfaceMemoryFormatY12___U12V12_N444:
    case NvKmsSurfaceMemoryFormatY12___U12V12_N422:
    case NvKmsSurfaceMemoryFormatY12___V12U12_N420:
        return TRUE;
    case NvKmsSurfaceMemoryFormatY8_U8__Y8_V8_N422:
    case NvKmsSurfaceMemoryFormatU8_Y8__V8_Y8_N422:
    case NvKmsSurfaceMemoryFormatY8___V8U8_N444:
    case NvKmsSurfaceMemoryFormatY8___V8U8_N422:
    case NvKmsSurfaceMemoryFormatY8___U8V8_N420:
    case NvKmsSurfaceMemoryFormatY10___V10U10_N444:
    case NvKmsSurfaceMemoryFormatY10___V10U10_N422:
    case NvKmsSurfaceMemoryFormatY10___U10V10_N420:
    case NvKmsSurfaceMemoryFormatY12___V12U12_N444:
    case NvKmsSurfaceMemoryFormatY12___V12U12_N422:
    case NvKmsSurfaceMemoryFormatY12___U12V12_N420:
    case NvKmsSurfaceMemoryFormatY8___U8___V8_N444:
    case NvKmsSurfaceMemoryFormatY8___U8___V8_N420:
        return FALSE;
    case NvKmsSurfaceMemoryFormatI8:
    case NvKmsSurfaceMemoryFormatA1R5G5B5:
    case NvKmsSurfaceMemoryFormatX1R5G5B5:
    case NvKmsSurfaceMemoryFormatR5G6B5:
    case NvKmsSurfaceMemoryFormatA8R8G8B8:
    case NvKmsSurfaceMemoryFormatX8R8G8B8:
    case NvKmsSurfaceMemoryFormatA8B8G8R8:
    case NvKmsSurfaceMemoryFormatX8B8G8R8:
    case NvKmsSurfaceMemoryFormatA2B10G10R10:
    case NvKmsSurfaceMemoryFormatX2B10G10R10:
    case NvKmsSurfaceMemoryFormatRF16GF16BF16AF16:
    case NvKmsSurfaceMemoryFormatRF16GF16BF16XF16:
    case NvKmsSurfaceMemoryFormatR16G16B16A16:
    case NvKmsSurfaceMemoryFormatRF32GF32BF32AF32:
        return FALSE;
    }

    return FALSE;
}

/*
 * Map the given NvKmsSurfaceMemoryFormat to its corresponding HW format for the
 * C370 (Volta) NVDISPLAY class.
 *
 * Volta supports YUV422 packed, but this function excludes the corresponding
 * mappings because the required programming support hasn't been added to NVKMS
 * yet.
 *
 * Return 0 in the case of an unrecognized NvKmsSurfaceMemoryFormat.
 */
static NvU32 nvHwFormatFromKmsFormatC3(
    const enum NvKmsSurfaceMemoryFormat format)
{
    switch (format) {
    case NvKmsSurfaceMemoryFormatI8:
        return NVC37E_SET_PARAMS_FORMAT_I8;
    case NvKmsSurfaceMemoryFormatA1R5G5B5:
    case NvKmsSurfaceMemoryFormatX1R5G5B5:
        return NVC37E_SET_PARAMS_FORMAT_A1R5G5B5;
    case NvKmsSurfaceMemoryFormatR5G6B5:
        return NVC37E_SET_PARAMS_FORMAT_R5G6B5;
    case NvKmsSurfaceMemoryFormatA8R8G8B8:
        return NVC37E_SET_PARAMS_FORMAT_A8R8G8B8;
    case NvKmsSurfaceMemoryFormatX8R8G8B8:
        return NVC37E_SET_PARAMS_FORMAT_X8R8G8B8;
    case NvKmsSurfaceMemoryFormatA8B8G8R8:
        return NVC37E_SET_PARAMS_FORMAT_A8B8G8R8;
    case NvKmsSurfaceMemoryFormatX8B8G8R8:
        return NVC37E_SET_PARAMS_FORMAT_X8B8G8R8;
    case NvKmsSurfaceMemoryFormatA2B10G10R10:
        return NVC37E_SET_PARAMS_FORMAT_A2B10G10R10;
    case NvKmsSurfaceMemoryFormatX2B10G10R10:
        return NVC37E_SET_PARAMS_FORMAT_X2BL10GL10RL10_XRBIAS;
    case NvKmsSurfaceMemoryFormatRF16GF16BF16AF16:
    case NvKmsSurfaceMemoryFormatRF16GF16BF16XF16:
        return NVC37E_SET_PARAMS_FORMAT_RF16_GF16_BF16_AF16;
    case NvKmsSurfaceMemoryFormatR16G16B16A16:
        return NVC37E_SET_PARAMS_FORMAT_R16_G16_B16_A16;
    case NvKmsSurfaceMemoryFormatRF32GF32BF32AF32:
    case NvKmsSurfaceMemoryFormatY8_U8__Y8_V8_N422:
    case NvKmsSurfaceMemoryFormatU8_Y8__V8_Y8_N422:
    case NvKmsSurfaceMemoryFormatY8___U8V8_N444:
    case NvKmsSurfaceMemoryFormatY8___V8U8_N444:
    case NvKmsSurfaceMemoryFormatY8___U8V8_N422:
    case NvKmsSurfaceMemoryFormatY8___V8U8_N422:
    case NvKmsSurfaceMemoryFormatY8___U8V8_N420:
    case NvKmsSurfaceMemoryFormatY8___V8U8_N420:
    case NvKmsSurfaceMemoryFormatY10___U10V10_N444:
    case NvKmsSurfaceMemoryFormatY10___V10U10_N444:
    case NvKmsSurfaceMemoryFormatY10___U10V10_N422:
    case NvKmsSurfaceMemoryFormatY10___V10U10_N422:
    case NvKmsSurfaceMemoryFormatY10___U10V10_N420:
    case NvKmsSurfaceMemoryFormatY10___V10U10_N420:
    case NvKmsSurfaceMemoryFormatY12___U12V12_N444:
    case NvKmsSurfaceMemoryFormatY12___V12U12_N444:
    case NvKmsSurfaceMemoryFormatY12___U12V12_N422:
    case NvKmsSurfaceMemoryFormatY12___V12U12_N422:
    case NvKmsSurfaceMemoryFormatY12___U12V12_N420:
    case NvKmsSurfaceMemoryFormatY12___V12U12_N420:
    case NvKmsSurfaceMemoryFormatY8___U8___V8_N444:
    case NvKmsSurfaceMemoryFormatY8___U8___V8_N420:
        return 0;
    }

    return 0;
}

/*
 * Map the given NvKmsSurfaceMemoryFormat to its corresponding HW format for the
 * C570 (Turing) NVDISPLAY class.
 *
 * Return 0 in the case of an unrecognized NvKmsSurfaceMemoryFormat.
 */
static NvU32 nvHwFormatFromKmsFormatC5(
    const enum NvKmsSurfaceMemoryFormat format)
{
    switch (format) {
    case NvKmsSurfaceMemoryFormatY8_U8__Y8_V8_N422:
        return NVC57E_SET_PARAMS_FORMAT_Y8_U8__Y8_V8_N422;
    case NvKmsSurfaceMemoryFormatU8_Y8__V8_Y8_N422:
        return NVC57E_SET_PARAMS_FORMAT_U8_Y8__V8_Y8_N422;
    case NvKmsSurfaceMemoryFormatY8___U8V8_N444:
    case NvKmsSurfaceMemoryFormatY8___V8U8_N444:
        return NVC57E_SET_PARAMS_FORMAT_Y8___U8V8_N444;
    case NvKmsSurfaceMemoryFormatY8___U8V8_N422:
    case NvKmsSurfaceMemoryFormatY8___V8U8_N422:
        return NVC57E_SET_PARAMS_FORMAT_Y8___U8V8_N422;
    case NvKmsSurfaceMemoryFormatY8___U8V8_N420:
    case NvKmsSurfaceMemoryFormatY8___V8U8_N420:
        return NVC57E_SET_PARAMS_FORMAT_Y8___V8U8_N420;
    case NvKmsSurfaceMemoryFormatY10___U10V10_N444:
    case NvKmsSurfaceMemoryFormatY10___V10U10_N444:
        return NVC57E_SET_PARAMS_FORMAT_Y10___U10V10_N444;
    case NvKmsSurfaceMemoryFormatY10___U10V10_N422:
    case NvKmsSurfaceMemoryFormatY10___V10U10_N422:
        return NVC57E_SET_PARAMS_FORMAT_Y10___U10V10_N422;
    case NvKmsSurfaceMemoryFormatY10___U10V10_N420:
    case NvKmsSurfaceMemoryFormatY10___V10U10_N420:
        return NVC57E_SET_PARAMS_FORMAT_Y10___V10U10_N420;
    case NvKmsSurfaceMemoryFormatY12___U12V12_N444:
    case NvKmsSurfaceMemoryFormatY12___V12U12_N444:
        return NVC57E_SET_PARAMS_FORMAT_Y12___U12V12_N444;
    case NvKmsSurfaceMemoryFormatY12___U12V12_N422:
    case NvKmsSurfaceMemoryFormatY12___V12U12_N422:
        return NVC57E_SET_PARAMS_FORMAT_Y12___U12V12_N422;
    case NvKmsSurfaceMemoryFormatY12___U12V12_N420:
    case NvKmsSurfaceMemoryFormatY12___V12U12_N420:
        return NVC57E_SET_PARAMS_FORMAT_Y12___V12U12_N420;
    case NvKmsSurfaceMemoryFormatY8___U8___V8_N444:
    case NvKmsSurfaceMemoryFormatY8___U8___V8_N420:
    case NvKmsSurfaceMemoryFormatI8:
    case NvKmsSurfaceMemoryFormatA1R5G5B5:
    case NvKmsSurfaceMemoryFormatX1R5G5B5:
    case NvKmsSurfaceMemoryFormatR5G6B5:
    case NvKmsSurfaceMemoryFormatA8R8G8B8:
    case NvKmsSurfaceMemoryFormatX8R8G8B8:
    case NvKmsSurfaceMemoryFormatA8B8G8R8:
    case NvKmsSurfaceMemoryFormatX8B8G8R8:
    case NvKmsSurfaceMemoryFormatA2B10G10R10:
    case NvKmsSurfaceMemoryFormatX2B10G10R10:
    case NvKmsSurfaceMemoryFormatRF16GF16BF16AF16:
    case NvKmsSurfaceMemoryFormatRF16GF16BF16XF16:
    case NvKmsSurfaceMemoryFormatR16G16B16A16:
    case NvKmsSurfaceMemoryFormatRF32GF32BF32AF32:
        return nvHwFormatFromKmsFormatC3(format);
    }

    return 0;
}

/*
 * Map the given NvKmsSurfaceMemoryFormat to its corresponding HW format for the
 * C670 (Orin and Ampere) NVDISPLAY class.
 *
 * Return 0 in the case of an unrecognized NvKmsSurfaceMemoryFormat.
 */
NvU32 nvHwFormatFromKmsFormatC6(const enum NvKmsSurfaceMemoryFormat format)
{
    switch (format) {
    case NvKmsSurfaceMemoryFormatY8___U8___V8_N444:
        return NVC67E_SET_PARAMS_FORMAT_Y8___U8___V8_N444;
    case NvKmsSurfaceMemoryFormatY8___U8___V8_N420:
        return NVC67E_SET_PARAMS_FORMAT_Y8___U8___V8_N420;
    case NvKmsSurfaceMemoryFormatX2B10G10R10:
        return NVC67E_SET_PARAMS_FORMAT_A2B10G10R10;
    case NvKmsSurfaceMemoryFormatY8_U8__Y8_V8_N422:
    case NvKmsSurfaceMemoryFormatU8_Y8__V8_Y8_N422:
    case NvKmsSurfaceMemoryFormatY8___U8V8_N444:
    case NvKmsSurfaceMemoryFormatY8___V8U8_N444:
    case NvKmsSurfaceMemoryFormatY8___U8V8_N422:
    case NvKmsSurfaceMemoryFormatY8___V8U8_N422:
    case NvKmsSurfaceMemoryFormatY8___U8V8_N420:
    case NvKmsSurfaceMemoryFormatY8___V8U8_N420:
    case NvKmsSurfaceMemoryFormatY10___U10V10_N444:
    case NvKmsSurfaceMemoryFormatY10___V10U10_N444:
    case NvKmsSurfaceMemoryFormatY10___U10V10_N422:
    case NvKmsSurfaceMemoryFormatY10___V10U10_N422:
    case NvKmsSurfaceMemoryFormatY10___U10V10_N420:
    case NvKmsSurfaceMemoryFormatY10___V10U10_N420:
    case NvKmsSurfaceMemoryFormatY12___U12V12_N444:
    case NvKmsSurfaceMemoryFormatY12___V12U12_N444:
    case NvKmsSurfaceMemoryFormatY12___U12V12_N422:
    case NvKmsSurfaceMemoryFormatY12___V12U12_N422:
    case NvKmsSurfaceMemoryFormatY12___U12V12_N420:
    case NvKmsSurfaceMemoryFormatY12___V12U12_N420:
    case NvKmsSurfaceMemoryFormatI8:
    case NvKmsSurfaceMemoryFormatA1R5G5B5:
    case NvKmsSurfaceMemoryFormatX1R5G5B5:
    case NvKmsSurfaceMemoryFormatR5G6B5:
    case NvKmsSurfaceMemoryFormatA8R8G8B8:
    case NvKmsSurfaceMemoryFormatX8R8G8B8:
    case NvKmsSurfaceMemoryFormatA8B8G8R8:
    case NvKmsSurfaceMemoryFormatX8B8G8R8:
    case NvKmsSurfaceMemoryFormatA2B10G10R10:
    case NvKmsSurfaceMemoryFormatRF16GF16BF16AF16:
    case NvKmsSurfaceMemoryFormatRF16GF16BF16XF16:
    case NvKmsSurfaceMemoryFormatR16G16B16A16:
    case NvKmsSurfaceMemoryFormatRF32GF32BF32AF32:
        return nvHwFormatFromKmsFormatC5(format);
    }

    return 0;
}

static
NVSurfaceEvoPtr EvoGetLutSurface3(NVDevEvoPtr pDevEvo,
                                  NVEvoChannelPtr pChannel,
                                  const NVFlipChannelEvoHwState *pHwState,
                                  NvU32 *lutSize,
                                  NvU64 *offset,
                                  NvBool *isLutModeVss)
{
    NvU32 win = NV_EVO_CHANNEL_MASK_WINDOW_NUMBER(pChannel->channelMask);
    NvU32 head = pDevEvo->headForWindow[win];
    NvBool found = FALSE;
    const NVDispEvoRec *pDispEvo = NULL;
    NvU32 sd;

    if ((pHwState->pSurfaceEvo[NVKMS_LEFT] == NULL) ||
        (head == NV_INVALID_HEAD)) {
        return NULL;
    }

    /* Input Lut is explicitly enabled by client */
    if (pHwState->inputLut.pLutSurfaceEvo != NULL) {
        *lutSize = pHwState->inputLut.lutEntries + NV_LUT_VSS_HEADER_SIZE;
        *offset = pHwState->inputLut.offset;
        *isLutModeVss = (pHwState->inputLut.vssSegments > 0);

        return pHwState->inputLut.pLutSurfaceEvo;
    }

    /*
     * For everything but I8 surfaces, we can just use the specified
     * LUT, even if it's NULL.
     * For I8 surfaces, we can only use the specified surface if it's
     * non-NULL (an input LUT is required).
     */
    if (pHwState->pSurfaceEvo[NVKMS_LEFT]->format !=
        NvKmsSurfaceMemoryFormatI8) {
        return NULL;
    }

    /*
     * The rest of the function is to handle the I8 case where no input
     * LUT was specified: look up the LUT to use from the device.
     */

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        if (nvPeekEvoSubDevMask(pDevEvo) & (1 << sd)) {
            if (found) {
                nvAssert(pDispEvo == pDevEvo->gpus[sd].pDispEvo);
            } else {
                pDispEvo = pDevEvo->gpus[sd].pDispEvo;
                found = TRUE;
            }
        }
    }

    nvAssert(found);

    /*
     * It is not allowed to change the input LUT on immediate flips. The
     * higher-level code should makes sure to disable tearing if there is change
     * in the surface format and curLUTIndex does not change until next
     * EvoSetLUTContextDma3() call which also makes sure to disable tearing.
     */
    return pDispEvo->headState[head].lut.pCurrSurface;
}

static void
EvoFlipC3(NVDevEvoPtr pDevEvo,
          NVEvoChannelPtr pChannel,
          const NVFlipChannelEvoHwState *pHwState,
          NVEvoUpdateState *updateState,
          NvBool bypassComposition)
{
    NvBool enableCSC, swapUV, flip3Return;
    enum NvKmsSurfaceMemoryFormat format;
    /*
     * lutSize and isLutModeVss are unused, since we only support 1025 and
     * non-VSS on Volta, but we declare them to pass to EvoGetLutSurface3.
     *
     * TODO: Maybe validate the resulting values?
     */
    NvU32 lutSize = NV_NUM_EVO_LUT_ENTRIES;
    NvU64 offset = offsetof(NVEvoLutDataRec, base);
    NvBool isLutModeVss = FALSE;
    NVSurfaceEvoPtr pLutSurfaceEvo = EvoGetLutSurface3(pDevEvo, pChannel, pHwState,
                                                       &lutSize, &offset, &isLutModeVss);

    if (pHwState->timeStamp != 0) {
        InsertAdditionalTimestampFlip(pDevEvo, pChannel, pHwState,
                                      updateState);
    }

    flip3Return = EvoFlipC3Common(pDevEvo, pChannel, pHwState, updateState);

    /* program semaphore */
    EvoProgramSemaphore3(pDevEvo, pChannel, pHwState);

    if (!flip3Return) {
        return;
    }

    format = pHwState->pSurfaceEvo[NVKMS_LEFT]->format;

    enableCSC = SetCscMatrixC3(pChannel, &pHwState->cscMatrix);
    swapUV = IsSurfaceFormatUVSwapped(format);
    nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_PARAMS, 1);
    nvDmaSetEvoMethodData(pChannel,
        (enableCSC ? DRF_DEF(C37E, _SET_PARAMS, _CSC, _ENABLE) :
                     DRF_DEF(C37E, _SET_PARAMS, _CSC, _DISABLE)) |
        DRF_NUM(C37E, _SET_PARAMS, _FORMAT, nvHwFormatFromKmsFormatC3(format)) |
        (swapUV ? DRF_DEF(C37E, _SET_PARAMS, _SWAP_UV, _ENABLE) :
                  DRF_DEF(C37E, _SET_PARAMS, _SWAP_UV, _DISABLE)) |
        DRF_DEF(C37E, _SET_PARAMS, _UNDERREPLICATE, _DISABLE));

    if (pLutSurfaceEvo) {
        const NvU32 ctxDma = pLutSurfaceEvo->planes[0].surfaceDesc.ctxDmaHandle;

        nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_CONTROL_INPUT_LUT, 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_DEF(C37E, _SET_CONTROL_INPUT_LUT, _SIZE, _SIZE_1025) |
            DRF_DEF(C37E, _SET_CONTROL_INPUT_LUT, _RANGE, _UNITY) |
            DRF_DEF(C37E, _SET_CONTROL_INPUT_LUT, _OUTPUT_MODE, _INDEX));

        nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_OFFSET_INPUT_LUT, 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(C37E, _SET_OFFSET_INPUT_LUT, _ORIGIN, offset));

        nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_CONTEXT_DMA_INPUT_LUT, 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(C37E, _SET_CONTEXT_DMA_INPUT_LUT, _HANDLE, ctxDma));
    } else {
        nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_CONTEXT_DMA_INPUT_LUT, 1);
        nvDmaSetEvoMethodData(pChannel, 0);
    }

    UpdateCompositionC3(pDevEvo, pChannel,
                        &pHwState->composition, updateState,
                        format);
}

static void EvoSetupPQEotfBaseLutC5(NVEvoLutDataRec *pData,
                                    enum NvKmsLUTState *lutState,
                                    NvU32 *lutSize,
                                    NvBool *isLutModeVss)
{
    NvU32 lutDataStartingIndex = NV_LUT_VSS_HEADER_SIZE;
    NvU32 numEotfPQ512Entries = ARRAY_LEN(EotfPQ512Entries);
    NvU32 eotfTableIdx;
    NvU64 vssHead = 0;
    NvU32 lutEntryCounter = 0, i;

    // Skip LUT data init if already done
    if (*lutState == NvKmsLUTStatePQ) {
        goto skipInit;
    }

    // VSS Header
    for (lutEntryCounter = 0; lutEntryCounter < NV_LUT_VSS_HEADER_SIZE; lutEntryCounter++) {
        vssHead = 0;
        for (i = 0; ((i < 16) && (((lutEntryCounter * 16) + i) < ARRAY_LEN(EotfPQ512SegSizesLog2))); i++) {
            NvU64 temp = EotfPQ512SegSizesLog2[(lutEntryCounter * 16) + i];
            temp = temp << (i * 3);
            vssHead |= temp;
        }
        nvkms_memcpy(&(pData->base[lutEntryCounter]), &vssHead, sizeof(NVEvoLutEntryRec));
    }

    for (eotfTableIdx = 0; eotfTableIdx < numEotfPQ512Entries; eotfTableIdx++) {
        /*
         * Values are in range [0.0, 125.0], will be scaled back by OLUT.
         * XXX HDR TODO: Divide by 125.0 if output mode is not HDR?
         */
        pData->base[eotfTableIdx + lutDataStartingIndex].Red =
        pData->base[eotfTableIdx + lutDataStartingIndex].Green =
        pData->base[eotfTableIdx + lutDataStartingIndex].Blue =
            EotfPQ512Entries[eotfTableIdx];
    }

    // Copy the last entry for interpolation
    pData->base[numEotfPQ512Entries + lutDataStartingIndex].Red =
        pData->base[numEotfPQ512Entries + lutDataStartingIndex - 1].Red;
    pData->base[numEotfPQ512Entries + lutDataStartingIndex].Green =
        pData->base[numEotfPQ512Entries + lutDataStartingIndex - 1].Green;
    pData->base[numEotfPQ512Entries + lutDataStartingIndex].Blue =
        pData->base[numEotfPQ512Entries + lutDataStartingIndex - 1].Blue;

skipInit:
    *lutState = NvKmsLUTStatePQ;
    *lutSize = NV_LUT_VSS_HEADER_SIZE + numEotfPQ512Entries + 1;
    *isLutModeVss = TRUE;
}

static void
EvoSetupIdentityBaseLutC5(NVEvoLutDataRec *pData,
                          enum NvKmsLUTState *lutState,
                          NvU32 *lutSize,
                          NvBool *isLutModeVss)
{
    int i;

    // Skip LUT data init if already done
    if (*lutState == NvKmsLUTStateIdentity) {
        goto skipInit;
    }

    ct_assert(NV_NUM_EVO_LUT_ENTRIES == 1025);

    // nvdisplay 3 uses FP16 entries in the ILUT.
    for (i = 0; i < 1024; i++) {
        pData->base[NV_LUT_VSS_HEADER_SIZE + i].Red =
        pData->base[NV_LUT_VSS_HEADER_SIZE + i].Green =
        pData->base[NV_LUT_VSS_HEADER_SIZE + i].Blue = nvUnorm10ToFp16(i).v;
    }
    pData->base[NV_LUT_VSS_HEADER_SIZE + 1024] =
        pData->base[NV_LUT_VSS_HEADER_SIZE + 1023];

skipInit:
    *lutState = NvKmsLUTStateIdentity;
    *lutSize = NV_LUT_VSS_HEADER_SIZE + NV_NUM_EVO_LUT_ENTRIES;
    *isLutModeVss = FALSE;
}

static void EvoSetILUTSurfaceAddressC5(
    const NVDevEvoRec *pDevEvo,
    NVEvoChannelPtr pChannel,
    const NVSurfaceDescriptor *pSurfaceDesc,
    NvU32 offset)
{
    NvU32 ctxDmaHandle = pSurfaceDesc ? pSurfaceDesc->ctxDmaHandle : 0;

    nvDmaSetStartEvoMethod(pChannel, NVC57E_SET_CONTEXT_DMA_ILUT, 1);
    nvDmaSetEvoMethodData(pChannel, DRF_NUM(C57E, _SET_CONTEXT_DMA_ILUT, _HANDLE, ctxDmaHandle));

    nvDmaSetStartEvoMethod(pChannel, NVC57E_SET_OFFSET_ILUT, 1);
    nvDmaSetEvoMethodData(pChannel, DRF_NUM(C57E, _SET_OFFSET_ILUT, _ORIGIN, offset));
}

static void
EvoFlipC5Common(NVDevEvoPtr pDevEvo,
         NVEvoChannelPtr pChannel,
         const NVFlipChannelEvoHwState *pHwState,
         NVEvoUpdateState *updateState,
         NvBool bypassComposition)
{
    enum NvKmsSurfaceMemoryFormat format;
    NvBool swapUV;
    NvU32 hTaps, vTaps;
    NvBool scaling = FALSE;
    NVSurfaceEvoPtr pLutSurfaceEvo = NULL;
    NvU64 lutOffset = offsetof(NVEvoLutDataRec, base);
    NvU32 lutSize = NV_NUM_EVO_LUT_ENTRIES;
    NvBool isLutModeVss = FALSE;

    NvU32 win = NV_EVO_CHANNEL_MASK_WINDOW_NUMBER(pChannel->channelMask);
    NvU32 head = pDevEvo->headForWindow[win];

    const NvU32 sdMask = nvPeekEvoSubDevMask(pDevEvo);
    const NvU32 sd = (sdMask == 0) ? 0 : nv_ffs(sdMask) - 1;
    const NVDispHeadStateEvoRec *pHeadState = &pDevEvo->pDispEvo[sd]->headState[head];

    // XXX HDR TODO: Handle other transfer funcions
    // XXX HDR TODO: Enable custom input LUTs with HDR
    if (pHwState->inputLut.fromOverride ||
        (pHwState->tf != NVKMS_INPUT_TF_PQ)) {
        pLutSurfaceEvo = EvoGetLutSurface3(pDevEvo, pChannel, pHwState,
                                           &lutSize, &lutOffset, &isLutModeVss);
    }

    if (!EvoFlipC3Common(pDevEvo, pChannel, pHwState, updateState)) {
        ConfigureTmoLut(pDevEvo, pHwState, pChannel);
        return;
    }

    format = pHwState->pSurfaceEvo[NVKMS_LEFT]->format;

    swapUV = IsSurfaceFormatUVSwapped(format);
    nvDmaSetStartEvoMethod(pChannel, NVC57E_SET_PARAMS, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C57E, _SET_PARAMS, _FORMAT, nvHwFormatFromKmsFormatC6(format)) |
        (swapUV ? DRF_DEF(C57E, _SET_PARAMS, _SWAP_UV, _ENABLE) :
                  DRF_DEF(C57E, _SET_PARAMS, _SWAP_UV, _DISABLE)));

    /*
     * In nvdisplay 2, there was a fixed-function block in the precomp FMT
     * module that was responsible for YUV->RGB conversion.
     *
     * In nvdisplay 3, that fixed-function block no longer exists.
     * In its place, there's a generic 3x4 S5.16 coefficient matrix that SW must
     * explicitly configure to convert the input surface format to the internal
     * RGB pipe native format.
     */
    EvoSetFMTMatrixC5(pChannel, format, pHwState);

    vTaps = (pHwState->vTaps >= NV_EVO_SCALER_5TAPS) ?
            NVC57E_SET_CONTROL_INPUT_SCALER_VERTICAL_TAPS_TAPS_5 :
            NVC57E_SET_CONTROL_INPUT_SCALER_VERTICAL_TAPS_TAPS_2;
    hTaps = (pHwState->hTaps >= NV_EVO_SCALER_5TAPS) ?
            NVC57E_SET_CONTROL_INPUT_SCALER_HORIZONTAL_TAPS_TAPS_5 :
            NVC57E_SET_CONTROL_INPUT_SCALER_HORIZONTAL_TAPS_TAPS_2;

    nvDmaSetStartEvoMethod(pChannel, NVC57E_SET_CONTROL_INPUT_SCALER, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C57E, _SET_CONTROL_INPUT_SCALER, _VERTICAL_TAPS, vTaps) |
        DRF_NUM(C57E, _SET_CONTROL_INPUT_SCALER, _HORIZONTAL_TAPS, hTaps));

    scaling = (pHwState->sizeIn.width != pHwState->sizeOut.width) ||
              (pHwState->sizeIn.height != pHwState->sizeOut.height);
    nvAssert(!(scaling && bypassComposition));

    /*
     * If scaling or tonemapping, we must enable the CSC0 and CSC1 pipelines.
     *
     * If no scaling or tonemapping, just use CSC11 to convert from the input
     * gamut to the output (panel) gamut, and disable everything else.
     */
    if (scaling ||
        nvNeedsTmoLut(pDevEvo, pChannel, pHwState,
                      nvGetHDRSrcMaxLum(pHwState),
                      pHeadState->hdrInfoFrame.staticMetadata.maxCLL)) {
        ConfigureCsc0C5(pDevEvo, pChannel, pHwState, TRUE);
        ConfigureCsc1C5(pDevEvo, pChannel, pHwState, TRUE);
    } else {
        ConfigureCsc0C5(pDevEvo, pChannel, pHwState, FALSE);
        ConfigureCsc1C5(pDevEvo, pChannel, pHwState, FALSE);
    }

    // In nvdisplay 3, an ILUT is required to convert the input surface to FP16,
    // unless the surface being displayed is already FP16 to begin with.
    if ((format == NvKmsSurfaceMemoryFormatRF16GF16BF16AF16) ||
        (format == NvKmsSurfaceMemoryFormatRF16GF16BF16XF16) || bypassComposition) {
        nvAssert(pHwState->tf == NVKMS_INPUT_TF_LINEAR);
        pLutSurfaceEvo = NULL;
    } else if (!pLutSurfaceEvo) {
        NVEvoLutDataRec *pData = NULL;

        pLutSurfaceEvo = pDevEvo->lut.defaultLut;
        pData = pLutSurfaceEvo->cpuAddress[sd];

        nvAssert(pData);

        switch (pHwState->tf) {
            case NVKMS_INPUT_TF_PQ:
                EvoSetupPQEotfBaseLutC5(pData,
                                        &pDevEvo->lut.defaultBaseLUTState[sd],
                                        &lutSize, &isLutModeVss);
                break;
            case NVKMS_INPUT_TF_LINEAR:
                EvoSetupIdentityBaseLutC5(pData,
                                          &pDevEvo->lut.defaultBaseLUTState[sd],
                                          &lutSize, &isLutModeVss);
                break;
            default: // XXX HDR TODO: Handle other colorspaces
                nvAssert(FALSE);
                EvoSetupIdentityBaseLutC5(pData,
                                          &pDevEvo->lut.defaultBaseLUTState[sd],
                                          &lutSize, &isLutModeVss);
                break;
        }
    }

    if (pLutSurfaceEvo) {
        nvDmaSetStartEvoMethod(pChannel, NVC57E_SET_ILUT_CONTROL, 1);
        nvDmaSetEvoMethodData(pChannel,
            (isLutModeVss ? DRF_DEF(C57E, _SET_ILUT_CONTROL, _INTERPOLATE, _ENABLE) :
                            DRF_DEF(C57E, _SET_ILUT_CONTROL, _INTERPOLATE, _DISABLE)) |
            DRF_DEF(C57E, _SET_ILUT_CONTROL, _MIRROR, _DISABLE) |
            (isLutModeVss ? DRF_DEF(C57E, _SET_ILUT_CONTROL, _MODE, _SEGMENTED) :
                            DRF_DEF(C57E, _SET_ILUT_CONTROL, _MODE, _DIRECT10)) |
            DRF_NUM(C57E, _SET_ILUT_CONTROL, _SIZE, lutSize));

        pDevEvo->hal->SetILUTSurfaceAddress(pDevEvo, pChannel,
            &pLutSurfaceEvo->planes[0].surfaceDesc, lutOffset);
    } else {
        pDevEvo->hal->SetILUTSurfaceAddress(pDevEvo, pChannel,
            NULL /* pSurfaceDesc */, 0 /* offset */);
    }

    ConfigureTmoLut(pDevEvo, pHwState, pChannel);

    UpdateCompositionC5(pDevEvo, pChannel,
                        &pHwState->composition, updateState,
                        bypassComposition,
                        format);
}

static void
EvoFlipC5(NVDevEvoPtr pDevEvo,
          NVEvoChannelPtr pChannel,
          const NVFlipChannelEvoHwState *pHwState,
          NVEvoUpdateState *updateState,
          NvBool bypassComposition)
{
    EvoFlipC5Common(pDevEvo, pChannel, pHwState, updateState, bypassComposition);

    /* Work around bug 2117571: whenever the tearing mode is changing, send a
     * software method to notify RM. */
    if (pHwState->tearing != pChannel->oldTearingMode) {
        NvU32 win = NV_EVO_CHANNEL_MASK_WINDOW_NUMBER(pChannel->channelMask);
        NvU32 head = pDevEvo->headForWindow[win];

        if (head != NV_INVALID_HEAD) {
            nvDmaSetStartEvoMethod(pChannel, NVC57E_WINDOWS_NOTIFY_RM, 1);
            nvDmaSetEvoMethodData(pChannel,
                DRF_DEF(C57E, _WINDOWS_NOTIFY_RM, _VSYNC_STATE_CHANGE, _TRUE) |
                DRF_NUM(C57E, _WINDOWS_NOTIFY_RM, _ASSOCIATED_HEAD, head) |
                (pHwState->tearing ?
                    DRF_DEF(C57E, _WINDOWS_NOTIFY_RM, _VSYNC_STATE, _OFF) :
                    DRF_DEF(C57E, _WINDOWS_NOTIFY_RM, _VSYNC_STATE, _ON)));
        }

        pChannel->oldTearingMode = pHwState->tearing;
    }

    /* program semaphore */
    EvoProgramSemaphore3(pDevEvo, pChannel, pHwState);
}

void
nvEvoFlipC6(NVDevEvoPtr pDevEvo,
          NVEvoChannelPtr pChannel,
          const NVFlipChannelEvoHwState *pHwState,
          NVEvoUpdateState *updateState,
          NvBool bypassComposition)
{
    NvBool fromTop = TRUE;
    NvBool fromLeft = TRUE;

    NvU32 vDirVal = 0;
    NvU32 hDirVal = 0;

    switch (pHwState->rrParams.rotation) {
        case NVKMS_ROTATION_90:
        case NVKMS_ROTATION_270:
            nvAssert(!"Invalid rotation requested.");
            /* Fall-through */
        case NVKMS_ROTATION_0:
            break;
        case NVKMS_ROTATION_180:
            fromTop = FALSE;
            fromLeft = FALSE;
            break;
    }

    if (pHwState->rrParams.reflectionX) {
        fromLeft = !fromLeft;
    }
    if (pHwState->rrParams.reflectionY) {
        fromTop = !fromTop;
    }

    vDirVal = (fromTop ?
               DRF_DEF(C67E, _SET_SCAN_DIRECTION, _VERTICAL_DIRECTION, _FROM_TOP) :
               DRF_DEF(C67E, _SET_SCAN_DIRECTION, _VERTICAL_DIRECTION, _FROM_BOTTOM));
    hDirVal = (fromLeft ?
               DRF_DEF(C67E, _SET_SCAN_DIRECTION, _HORIZONTAL_DIRECTION, _FROM_LEFT) :
               DRF_DEF(C67E, _SET_SCAN_DIRECTION, _HORIZONTAL_DIRECTION, _FROM_RIGHT));

    nvDmaSetStartEvoMethod(pChannel, NVC67E_SET_SCAN_DIRECTION, 1);
    nvDmaSetEvoMethodData(pChannel, vDirVal | hDirVal);

    EvoFlipC5Common(pDevEvo, pChannel, pHwState, updateState, bypassComposition);

    /* program semaphore */
    EvoProgramSemaphore6(pDevEvo, pChannel, pHwState);
}

static void UpdateComposition(NVDevEvoPtr pDevEvo,
                              NVEvoChannelPtr pChannel,
                              /* smaller => closer to front */
                              NvU32 depth,
                              NvU32 colorKeySelect,
                              NvU32 constantAlpha,
                              NvU32 compositionFactorSelect,
                              const NVColorKey key,
                              NVEvoUpdateState *updateState)
{
    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_COMPOSITION_CONTROL, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37E, _SET_COMPOSITION_CONTROL, _COLOR_KEY_SELECT, colorKeySelect) |
        DRF_NUM(C37E, _SET_COMPOSITION_CONTROL, _DEPTH, depth));

    nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_COMPOSITION_CONSTANT_ALPHA, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37E, _SET_COMPOSITION_CONSTANT_ALPHA, _K1, constantAlpha));

    nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_COMPOSITION_FACTOR_SELECT, 1);
    nvDmaSetEvoMethodData(pChannel, compositionFactorSelect);

#define UPDATE_COMPONENT(_COMP, _C, _c) \
    nvDmaSetStartEvoMethod(pChannel, NVC37E_SET_KEY_##_COMP, 1); \
    if (key.match##_C) { \
        nvDmaSetEvoMethodData(pChannel, \
            DRF_NUM(C37E, _SET_KEY_##_COMP, _MIN, key._c) | \
            DRF_NUM(C37E, _SET_KEY_##_COMP, _MAX, key._c)); \
    } else { \
        nvDmaSetEvoMethodData(pChannel, \
            DRF_NUM(C37E, _SET_KEY_##_COMP, _MIN, 0) | \
            DRF_SHIFTMASK(NVC37E_SET_KEY_##_COMP##_MAX)); \
    }

    if (colorKeySelect !=
        NVC37E_SET_COMPOSITION_CONTROL_COLOR_KEY_SELECT_DISABLE) {
        UPDATE_COMPONENT(ALPHA, A, a);
        UPDATE_COMPONENT(RED_CR, R, r);
        UPDATE_COMPONENT(GREEN_Y, G, g);
        UPDATE_COMPONENT(BLUE_CB, B, b);
    }

#undef UPDATE_COMPONENT
}

static void EvoFlipTransitionWARC3(NVDevEvoPtr pDevEvo, NvU32 sd, NvU32 head,
                                   const NVEvoSubDevHeadStateRec *pSdHeadState,
                                   const NVFlipEvoHwState *pFlipState,
                                   NVEvoUpdateState *updateState)
{
    /* Nothing to do for Volta */
}

/*
 * Hardware bug 2193096 requires that we send special software methods around
 * a window channel update that transitions from NULL ctxdma to non-NULL or
 * vice versa.  Below we compare the current hardware state in pSdHeadState
 * against the state to be pushed in this update in pFlipState, and add any
 * window(s) that qualify to the 'flipTransitionWAR' mask in the updateState.
 */
static void EvoFlipTransitionWARC5(NVDevEvoPtr pDevEvo, NvU32 sd, NvU32 head,
                                   const NVEvoSubDevHeadStateRec *pSdHeadState,
                                   const NVFlipEvoHwState *pFlipState,
                                   NVEvoUpdateState *updateState)
{
    NvU32 layer;

    for (layer = 0; layer < pDevEvo->head[head].numLayers; layer++) {
        const NvBool enabledPrev =
            pSdHeadState->layer[layer].pSurfaceEvo[NVKMS_LEFT] != NULL;
        const NvBool enabledNext =
            pFlipState->layer[layer].pSurfaceEvo[NVKMS_LEFT] != NULL;

        if (enabledPrev != enabledNext) {
            /* XXX TODO: dynamic window assignment */
            const NvU32 win = NV_EVO_CHANNEL_MASK_WINDOW_NUMBER(
                pDevEvo->head[head].layer[layer]->channelMask);
            updateState->subdev[sd].flipTransitionWAR |=
                DRF_IDX_DEF64(_EVO, _CHANNEL_MASK, _WINDOW, win, _ENABLE);

            nvAssert(pFlipState->dirty.layer[layer]);
        }
    }
}

void nvEvoFlipTransitionWARC6(NVDevEvoPtr pDevEvo, NvU32 sd, NvU32 head,
                                   const NVEvoSubDevHeadStateRec *pSdHeadState,
                                   const NVFlipEvoHwState *pFlipState,
                                   NVEvoUpdateState *updateState)
{
    /* Nothing to do for Orin/Ampere for now */
}

static void
UpdateCompositionC3(NVDevEvoPtr pDevEvo,
                    NVEvoChannelPtr pChannel,
                    const struct NvKmsCompositionParams *pCompParams,
                    NVEvoUpdateState *updateState,
                    enum NvKmsSurfaceMemoryFormat format)
{
    NvU32 colorKeySelect;
    NvU32 compositionFactorSelect = 0;
    NvU32 constantAlpha = 0;
    NvU32 match;

    switch (pCompParams->colorKeySelect) {
        case NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE:
            colorKeySelect =
                NVC37E_SET_COMPOSITION_CONTROL_COLOR_KEY_SELECT_DISABLE;
            break;
        case NVKMS_COMPOSITION_COLOR_KEY_SELECT_SRC:
            colorKeySelect =
                NVC37E_SET_COMPOSITION_CONTROL_COLOR_KEY_SELECT_SRC;

            break;
        case NVKMS_COMPOSITION_COLOR_KEY_SELECT_DST:
            colorKeySelect =
                NVC37E_SET_COMPOSITION_CONTROL_COLOR_KEY_SELECT_DST;

           break;
        default:
            nvAssert(!"Invalid color key select");
            return;
    }

    /* Match and nomatch pixels should not use alpha blending mode at once. */
    nvAssert((colorKeySelect == NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE) ||
             (!NvKmsIsCompositionModeUseAlpha(pCompParams->blendingMode[0])) ||
             (!NvKmsIsCompositionModeUseAlpha(pCompParams->blendingMode[1])));

    /*
     * Match and nomatch pixels should not use blending mode PREMULT_ALPHA,
     * NON_PREMULT_ALPHA, PREMULT_SURFACE_ALPHA, and NON_PREMULT_SURFACE_ALPHA
     * at once.
     */
    nvAssert(pCompParams->blendingMode[0] == NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE ||
             pCompParams->blendingMode[0] == NVKMS_COMPOSITION_BLENDING_MODE_TRANSPARENT ||
             pCompParams->blendingMode[1] == NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE ||
             pCompParams->blendingMode[1] == NVKMS_COMPOSITION_BLENDING_MODE_TRANSPARENT);

    for (match = 0; match <= 1; match++) {
        switch (pCompParams->blendingMode[match]) {
            case NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE:
                if (match == 1) {
                    compositionFactorSelect |=
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _SRC_COLOR_FACTOR_MATCH_SELECT, _ONE) |
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _DST_COLOR_FACTOR_MATCH_SELECT, _ZERO);
                } else {
                    compositionFactorSelect |=
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _SRC_COLOR_FACTOR_NO_MATCH_SELECT, _ONE) |
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _DST_COLOR_FACTOR_NO_MATCH_SELECT, _ZERO);
                }
                break;
            case NVKMS_COMPOSITION_BLENDING_MODE_TRANSPARENT:
                if (match == 1) {
                    compositionFactorSelect |=
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _SRC_COLOR_FACTOR_MATCH_SELECT, _ZERO) |
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _DST_COLOR_FACTOR_MATCH_SELECT, _ONE);
                } else {
                    compositionFactorSelect |=
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _SRC_COLOR_FACTOR_NO_MATCH_SELECT, _ZERO) |
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _DST_COLOR_FACTOR_NO_MATCH_SELECT, _ONE);
                }
                break;
            case NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_ALPHA:
                constantAlpha = 255;
                if (match == 1) {
                    compositionFactorSelect |=
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _SRC_COLOR_FACTOR_MATCH_SELECT, _K1_TIMES_SRC) |
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _DST_COLOR_FACTOR_MATCH_SELECT, _NEG_K1_TIMES_SRC);
                } else {
                    compositionFactorSelect |=
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _SRC_COLOR_FACTOR_NO_MATCH_SELECT, _K1_TIMES_SRC) |
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _DST_COLOR_FACTOR_NO_MATCH_SELECT, _NEG_K1_TIMES_SRC);
                }
                break;
            case NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_ALPHA:
                constantAlpha = 255;
                if (match == 1) {
                    compositionFactorSelect |=
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _SRC_COLOR_FACTOR_MATCH_SELECT, _K1) |
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _DST_COLOR_FACTOR_MATCH_SELECT, _NEG_K1_TIMES_SRC);
                } else {
                    compositionFactorSelect |=
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _SRC_COLOR_FACTOR_NO_MATCH_SELECT, _K1) |
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _DST_COLOR_FACTOR_NO_MATCH_SELECT, _NEG_K1_TIMES_SRC);
                }
                break;
            case NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_SURFACE_ALPHA:
                constantAlpha = pCompParams->surfaceAlpha;
                if (match == 1) {
                    compositionFactorSelect |=
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _SRC_COLOR_FACTOR_MATCH_SELECT, _K1_TIMES_SRC) |
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _DST_COLOR_FACTOR_MATCH_SELECT, _NEG_K1_TIMES_SRC);
                } else {
                    compositionFactorSelect |=
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _SRC_COLOR_FACTOR_NO_MATCH_SELECT, _K1_TIMES_SRC) |
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _DST_COLOR_FACTOR_NO_MATCH_SELECT, _NEG_K1_TIMES_SRC);
                }
                break;
            case NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_SURFACE_ALPHA:
                constantAlpha = pCompParams->surfaceAlpha;
                if (match == 1) {
                    compositionFactorSelect |=
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _SRC_COLOR_FACTOR_MATCH_SELECT, _K1) |
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _DST_COLOR_FACTOR_MATCH_SELECT, _NEG_K1_TIMES_SRC);
                } else {
                    compositionFactorSelect |=
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _SRC_COLOR_FACTOR_NO_MATCH_SELECT, _K1) |
                        DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _DST_COLOR_FACTOR_NO_MATCH_SELECT, _NEG_K1_TIMES_SRC);
                }
                break;
             default:
                nvAssert(!"Invalid blend mode");
                return;
        }

        /* Override the  composition factors for X channel emulated surface format. */
        if (NvKmsIsCompositionModeUseAlpha(pCompParams->blendingMode[match]) &&
            ((pDevEvo->hal->caps.xEmulatedSurfaceMemoryFormats & NVBIT64(format)) != 0U)) {
            if (match == 1) {
                /* Clear the previously selected composition factors for match pixels. */
                compositionFactorSelect &= ~(DRF_MASK(NVC37E_SET_COMPOSITION_FACTOR_SELECT_SRC_COLOR_FACTOR_MATCH_SELECT) <<
                                             DRF_SHIFT(NVC37E_SET_COMPOSITION_FACTOR_SELECT_SRC_COLOR_FACTOR_MATCH_SELECT));
                compositionFactorSelect &= ~(DRF_MASK(NVC37E_SET_COMPOSITION_FACTOR_SELECT_DST_COLOR_FACTOR_MATCH_SELECT) <<
                                             DRF_SHIFT(NVC37E_SET_COMPOSITION_FACTOR_SELECT_DST_COLOR_FACTOR_MATCH_SELECT));

                compositionFactorSelect |=
                    DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _SRC_COLOR_FACTOR_MATCH_SELECT, _K1) |
                    DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _DST_COLOR_FACTOR_MATCH_SELECT, _NEG_K1);
            } else {
                /* Clear the previously selected composition factors for no-match pixels. */
                compositionFactorSelect &= ~(DRF_MASK(NVC37E_SET_COMPOSITION_FACTOR_SELECT_SRC_COLOR_FACTOR_NO_MATCH_SELECT) <<
                                             DRF_SHIFT(NVC37E_SET_COMPOSITION_FACTOR_SELECT_SRC_COLOR_FACTOR_NO_MATCH_SELECT));
                compositionFactorSelect &= ~(DRF_MASK(NVC37E_SET_COMPOSITION_FACTOR_SELECT_DST_COLOR_FACTOR_NO_MATCH_SELECT) <<
                                             DRF_SHIFT(NVC37E_SET_COMPOSITION_FACTOR_SELECT_DST_COLOR_FACTOR_NO_MATCH_SELECT));

                compositionFactorSelect |=
                    DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _SRC_COLOR_FACTOR_NO_MATCH_SELECT, _K1) |
                    DRF_DEF(C37E, _SET_COMPOSITION_FACTOR_SELECT, _DST_COLOR_FACTOR_NO_MATCH_SELECT, _NEG_K1);
            }
        }
    }

    UpdateComposition(pDevEvo,
                      pChannel,
                      pCompParams->depth,
                      colorKeySelect,
                      constantAlpha,
                      compositionFactorSelect,
                      pCompParams->colorKey,
                      updateState);
}

static void EvoBypassCompositionC5(NVDevEvoPtr pDevEvo,
                                   NVEvoChannelPtr pChannel,
                                   NVEvoUpdateState *updateState)
{
    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    nvDmaSetStartEvoMethod(pChannel, NVC57E_SET_COMPOSITION_CONTROL, 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_DEF(C57E, _SET_COMPOSITION_CONTROL, _BYPASS, _ENABLE));
}

static void
UpdateCompositionC5(NVDevEvoPtr pDevEvo,
                    NVEvoChannelPtr pChannel,
                    const struct NvKmsCompositionParams *pCompParams,
                    NVEvoUpdateState *updateState,
                    NvBool bypassComposition,
                    enum NvKmsSurfaceMemoryFormat format)
{
    if (bypassComposition) {
        EvoBypassCompositionC5(pDevEvo, pChannel, updateState);
    } else {
        UpdateCompositionC3(pDevEvo, pChannel, pCompParams,
                            updateState, format);
    }
}

/*
 * The LUT entries in INDEX_1025_UNITY_RANGE have 16 bits, with the
 * black value at 24576, and the white at 49151. Since the effective
 * range is 16384, we treat this as a 14-bit LUT.  However, we need to
 * clear the low 3 bits to WAR hardware bug 813188.  This gives us
 * 14-bit LUT values, but only 11 bits of precision.
 * XXXnvdisplay: Bug 813188 is supposed to be fixed on NVDisplay; can we expose
 * more precision?
 */
static inline NvU16 ColorToLUTEntry(NvU16 val)
{
    const NvU16 val14bit = val >> 2;
    return (val14bit & ~7) + 24576;
}

/*
 * Unlike earlier EVO implementations, the INDEX mode of the input LUT on
 * NVDisplay is straightforward: the value of the input component is expanded
 * to the LUT size by simply shifting left by the difference between the LUT
 * index width and the component width.  We do the same, here, to select the
 * right LUT entry to fill.
 */
static inline NvU32 GetLUTIndex(int i, int componentSize)
{
    return i << (10 - componentSize);
}

static void
EvoFillLUTSurfaceC3(NVEvoLutEntryRec *pLUTBuffer,
                    const NvU16 *red,
                    const NvU16 *green,
                    const NvU16 *blue,
                    int nColorMapEntries, int depth)
{
    int i;
    NvU32 rSize, gSize, bSize;

    switch (depth) {
    case 15:
        rSize = gSize = bSize = 5;
        break;
    case 16:
        rSize = bSize = 5;
        gSize = 6;
        break;
    case 8:
    case 24:
        rSize = gSize = bSize = 8;
        break;
    case 30:
        rSize = gSize = bSize = 10;
        break;
    default:
        nvAssert(!"invalid depth");
        return;
    }

    for (i = 0; i < nColorMapEntries; i++) {
        if (i < (1 << rSize)) {
            pLUTBuffer[GetLUTIndex(i, rSize)].Red = ColorToLUTEntry(red[i]);
        }
        if (i < (1 << gSize)) {
            pLUTBuffer[GetLUTIndex(i, gSize)].Green = ColorToLUTEntry(green[i]);
        }
        if (i < (1 << bSize)) {
            pLUTBuffer[GetLUTIndex(i, bSize)].Blue = ColorToLUTEntry(blue[i]);
        }
    }
}

static inline float16_t ColorToFp16(NvU16 val, float32_t maxf)
{
    return nvUnormToFp16(val, maxf);
}

void
nvEvoFillLUTSurfaceC5(NVEvoLutEntryRec *pLUTBuffer,
                    const NvU16 *red,
                    const NvU16 *green,
                    const NvU16 *blue,
                    int nColorMapEntries, int depth)
{
    int i;
    NvU32 rSize, gSize, bSize;
    const float32_t maxf = ui32_to_f32(0xffff);

    switch (depth) {
    case 15:
        rSize = gSize = bSize = 5;
        break;
    case 16:
        rSize = bSize = 5;
        gSize = 6;
        break;
    case 8:
    case 24:
        rSize = gSize = bSize = 8;
        break;
    case 30:
        rSize = gSize = bSize = 10;
        break;
    default:
        nvAssert(!"invalid depth");
        return;
    }

    // Skip the VSS header
    pLUTBuffer += NV_LUT_VSS_HEADER_SIZE;

    for (i = 0; i < nColorMapEntries; i++) {
        if (i < (1 << rSize)) {
            pLUTBuffer[GetLUTIndex(i, rSize)].Red =
                ColorToFp16(red[i], maxf).v;
        }
        if (i < (1 << gSize)) {
            pLUTBuffer[GetLUTIndex(i, gSize)].Green =
                ColorToFp16(green[i], maxf).v;
        }
        if (i < (1 << bSize)) {
            pLUTBuffer[GetLUTIndex(i, bSize)].Blue =
                ColorToFp16(blue[i], maxf).v;
        }
    }
}

static void EvoSetOutputLutC3(NVDevEvoPtr pDevEvo,
                              NvU32 sd,
                              NvU32 head,
                              const NVFlipLutHwState *pOutputLut,
                              NvU32 fpNormScale,
                              NVEvoUpdateState *updateState,
                              NvBool bypassComposition)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvBool enableLut = (pOutputLut->pLutSurfaceEvo != NULL);
    NvU64 offset = enableLut ? pOutputLut->offset : offsetof(NVEvoLutDataRec, output);
    NvU32 ctxdma = enableLut ?
        pOutputLut->pLutSurfaceEvo->planes[0].surfaceDesc.ctxDmaHandle : 0;

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    nvAssert((offset & 0xff) == 0);

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_CONTROL_OUTPUT_LUT(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_DEF(C37D, _HEAD_SET_CONTROL_OUTPUT_LUT, _SIZE, _SIZE_1025) |
        DRF_DEF(C37D, _HEAD_SET_CONTROL_OUTPUT_LUT, _RANGE, _UNITY) |
        DRF_DEF(C37D, _HEAD_SET_CONTROL_OUTPUT_LUT, _OUTPUT_MODE, _INTERPOLATE));

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_OFFSET_OUTPUT_LUT(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37D, _HEAD_SET_OFFSET_OUTPUT_LUT, _ORIGIN, offset >> 8));

    /* Set the ctxdma for the output LUT */

    if (!enableLut) {
        /* Class C37D has no separate enable flag. */
        ctxdma = 0;
    }
    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_CONTEXT_DMA_OUTPUT_LUT(head), 1);
    nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(C37D, _HEAD_SET_CONTEXT_DMA_OUTPUT_LUT, _HANDLE, ctxdma));
}

static void EvoSetupPQOetfOutputLutC5(NVEvoLutDataRec *pData,
                                      enum NvKmsLUTState *lutState,
                                      NvU32 *lutSize,
                                      NvBool *isLutModeVss)
{
    NvU32 lutDataStartingIndex = NV_LUT_VSS_HEADER_SIZE;
    NvU32 numOetfPQ512Entries = ARRAY_LEN(OetfPQ512Entries);
    NvU32 oetfTableIdx;
    NvU64 vssHead = 0;
    NvU32 lutEntryCounter = 0, i;

    // Skip LUT data init if already done
    if (*lutState == NvKmsLUTStatePQ) {
        goto skipInit;
    }

    // VSS Header
    for (lutEntryCounter = 0; lutEntryCounter < NV_LUT_VSS_HEADER_SIZE; lutEntryCounter++) {
        vssHead = 0;
        for (i = 0; ((i < 16) && (((lutEntryCounter * 16) + i) < ARRAY_LEN(OetfPQ512SegSizesLog2))); i++) {
            NvU64 temp = OetfPQ512SegSizesLog2[(lutEntryCounter * 16) + i];
            temp = temp << (i * 3);
            vssHead |= temp;
        }
        nvkms_memcpy(&(pData->output[lutEntryCounter]), &vssHead, sizeof(NVEvoLutEntryRec));
    }

    for (oetfTableIdx = 0; oetfTableIdx < numOetfPQ512Entries; oetfTableIdx++) {
        pData->output[oetfTableIdx + lutDataStartingIndex].Red =
        pData->output[oetfTableIdx + lutDataStartingIndex].Green =
        pData->output[oetfTableIdx + lutDataStartingIndex].Blue =
            OetfPQ512Entries[oetfTableIdx];
    }

    // Copy the last entry for interpolation
    pData->output[numOetfPQ512Entries + lutDataStartingIndex].Red =
        pData->output[numOetfPQ512Entries + lutDataStartingIndex - 1].Red;
    pData->output[numOetfPQ512Entries + lutDataStartingIndex].Green =
        pData->output[numOetfPQ512Entries + lutDataStartingIndex - 1].Green;
    pData->output[numOetfPQ512Entries + lutDataStartingIndex].Blue =
        pData->output[numOetfPQ512Entries + lutDataStartingIndex - 1].Blue;

skipInit:
    *lutState = NvKmsLUTStatePQ;
    *lutSize = numOetfPQ512Entries + 1;
    *isLutModeVss = TRUE;
}

static void EvoSetupIdentityOutputLutC5(NVEvoLutDataRec *pData,
                                        enum NvKmsLUTState *lutState,
                                        NvU32 *lutSize,
                                        NvBool *isLutModeVss)
{
    NvU32 i;

    // Skip LUT data init if already done
    if (*lutState == NvKmsLUTStateIdentity) {
        goto skipInit;
    }

    ct_assert(NV_NUM_EVO_LUT_ENTRIES == 1025);

    // nvdisplay 3 uses 16-bit fixed-point entries in the OLUT.
    for (i = 0; i < 1024; i++) {
        pData->output[NV_LUT_VSS_HEADER_SIZE + i].Red =
        pData->output[NV_LUT_VSS_HEADER_SIZE + i].Green =
        pData->output[NV_LUT_VSS_HEADER_SIZE + i].Blue = (i << (16 - 10));
    }
    pData->output[NV_LUT_VSS_HEADER_SIZE + 1024] =
        pData->output[NV_LUT_VSS_HEADER_SIZE + 1023];

skipInit:
    *lutState = NvKmsLUTStateIdentity;
    *lutSize = 1025;
    *isLutModeVss = FALSE;
}

static void SetupHDROutputLUT(NVDevEvoPtr pDevEvo,
                              const NVDispHeadStateEvoRec *pHeadState,
                              NvU32 sd,
                              enum NvKmsLUTState *lutState,
                              NvU32 *lutSize,
                              NvBool *isLutModeVss)
{
    NVSurfaceEvoPtr pLut = pDevEvo->lut.defaultLut;
    NVEvoLutDataRec *pData = pLut->cpuAddress[sd];

    // XXX HDR TODO: Support other transfer functions
    nvAssert(pHeadState->tf == NVKMS_OUTPUT_TF_PQ);

    EvoSetupPQOetfOutputLutC5(pData, lutState, lutSize, isLutModeVss);
}

void nvSetupOutputLUT5(NVDevEvoPtr pDevEvo,
                       const NVDispHeadStateEvoRec *pHeadState,
                       NvBool enableOutputLut,
                       NvBool bypassComposition,
                       NVSurfaceDescriptor **pSurfaceDesc,
                       NvU32 *lutSize,
                       NvU64 *offset,
                       NvBool *disableOcsc0,
                       NvU32 *fpNormScale,
                       NvBool *isLutModeVss)
{
    NvU32 sd;

    /* Set the ctxdma for the output LUT */

    if (bypassComposition) {
        *pSurfaceDesc = NULL;

        /* if we're not enabling the OLUT, OCSC0 also needs to be disabled */
        *disableOcsc0 = TRUE;
    } else if (!enableOutputLut) {
        /* Use the default OLUT if the client didn't provide one */
        *pSurfaceDesc = &pDevEvo->lut.defaultLut->planes[0].surfaceDesc;
        *offset = offsetof(NVEvoLutDataRec, output);

        // Setup default OLUT
        for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
            // XXX HDR TODO: Support other transfer functions
            if (pHeadState->tf == NVKMS_OUTPUT_TF_PQ) {
                SetupHDROutputLUT(pDevEvo, pHeadState, sd,
                                  &pDevEvo->lut.defaultOutputLUTState[sd],
                                  lutSize, isLutModeVss);

                *disableOcsc0 = TRUE;

                /*
                 * Scale from [0.0, 125.0] to [0.0, 1.0]
                 * XXX HDR TODO: Assumes input is in this range, SDR is not.
                 */
                *fpNormScale = NVKMS_OLUT_FP_NORM_SCALE_DEFAULT / 125;
            } else {
                NVSurfaceEvoPtr pLut = pDevEvo->lut.defaultLut;
                NVEvoLutDataRec *pData = pLut->cpuAddress[sd];

                EvoSetupIdentityOutputLutC5(
                    pData,
                    &pDevEvo->lut.defaultOutputLUTState[sd],
                    lutSize, isLutModeVss);
            }
        }
    }
}

static void SetOLUTSurfaceAddress(
    NVEvoChannelPtr pChannel,
    const NVSurfaceDescriptor *pSurfaceDesc,
    NvU32 offset,
    NvU32 head)
{
    NvU32 ctxDmaHandle = pSurfaceDesc ? pSurfaceDesc->ctxDmaHandle : 0;

    nvDmaSetStartEvoMethod(pChannel, NVC57D_HEAD_SET_CONTEXT_DMA_OLUT(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C57D, _HEAD_SET_CONTEXT_DMA_OLUT, _HANDLE, ctxDmaHandle));

    nvDmaSetStartEvoMethod(pChannel, NVC57D_HEAD_SET_OFFSET_OLUT(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C57D, _HEAD_SET_OFFSET_OLUT, _ORIGIN, offset >> 8));
}

static void EvoSetOutputLutC5(NVDevEvoPtr pDevEvo,
                              NvU32 sd,
                              NvU32 head,
                              const NVFlipLutHwState *pOutputLut,
                              NvU32 fpNormScale,
                              NVEvoUpdateState *updateState,
                              NvBool bypassComposition)
{
    const NVDispEvoRec *pDispEvo = pDevEvo->pDispEvo[sd];
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvBool enableOutputLut = (pOutputLut->pLutSurfaceEvo != NULL);
    NVSurfaceEvoPtr pLutSurfEvo = pOutputLut->pLutSurfaceEvo;
    NVSurfaceDescriptor *pSurfaceDesc =
        enableOutputLut ? &pLutSurfEvo->planes[0].surfaceDesc : NULL;
    NvU64 offset = enableOutputLut ? pOutputLut->offset : offsetof(NVEvoLutDataRec, output);
    NvBool isLutModeVss = enableOutputLut ? (pOutputLut->vssSegments != 0) : FALSE;
    NvU32 lutSize = enableOutputLut ? pOutputLut->lutEntries : NV_NUM_EVO_LUT_ENTRIES;
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    NvBool disableOcsc0 = FALSE;
    NvBool outputRoundingFix = nvkms_output_rounding_fix();

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    // XXX HDR TODO: Enable custom output LUTs with HDR
    // XXX HDR TODO: Support other transfer functions
    if (!pOutputLut->fromOverride &&
        (pHeadState->tf == NVKMS_OUTPUT_TF_PQ)) {
        enableOutputLut = FALSE;
    }

    nvSetupOutputLUT5(pDevEvo,
                      pHeadState,
                      enableOutputLut,
                      bypassComposition,
                      &pSurfaceDesc,
                      &lutSize,
                      &offset,
                      &disableOcsc0,
                      &fpNormScale,
                      &isLutModeVss);

    if (disableOcsc0) {
        nvDmaSetStartEvoMethod(pChannel, NVC57D_HEAD_SET_OCSC0CONTROL(head), 1);
        nvDmaSetEvoMethodData(pChannel, DRF_DEF(C57D, _HEAD_SET_OCSC0CONTROL, _ENABLE, _DISABLE));

        outputRoundingFix = FALSE;
    } else {
        /* Update status of output rounding fix. */
        EvoSetOCsc0C5(pDispEvo, head, &outputRoundingFix);
    }

    /* Program the output LUT */
    nvAssert((offset & 0xff) == 0);

    nvDmaSetStartEvoMethod(pChannel, NVC57D_HEAD_SET_OLUT_CONTROL(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        ((isLutModeVss || !outputRoundingFix) ?
            DRF_DEF(C57D, _HEAD_SET_OLUT_CONTROL, _INTERPOLATE, _ENABLE) :
            DRF_DEF(C57D, _HEAD_SET_OLUT_CONTROL, _INTERPOLATE, _DISABLE)) |
        DRF_DEF(C57D, _HEAD_SET_OLUT_CONTROL, _MIRROR, _DISABLE) |
        (isLutModeVss ? DRF_DEF(C57D, _HEAD_SET_OLUT_CONTROL, _MODE, _SEGMENTED) :
                        DRF_DEF(C57D, _HEAD_SET_OLUT_CONTROL, _MODE, _DIRECT10)) |
        DRF_NUM(C57D, _HEAD_SET_OLUT_CONTROL, _SIZE, NV_LUT_VSS_HEADER_SIZE + lutSize));

    SetOLUTSurfaceAddress(pChannel, pSurfaceDesc, offset, head);

    nvDmaSetStartEvoMethod(pChannel, NVC57D_HEAD_SET_OLUT_FP_NORM_SCALE(head), 1);
    nvDmaSetEvoMethodData(pChannel, fpNormScale);

    if (!disableOcsc0) {
        /* only enable OCSC0 after enabling the OLUT */
        nvDmaSetStartEvoMethod(pChannel, NVC57D_HEAD_SET_OCSC0CONTROL(head), 1);
        nvDmaSetEvoMethodData(pChannel, DRF_DEF(C57D, _HEAD_SET_OCSC0CONTROL, _ENABLE, _ENABLE));
    }
}

static NvBool QueryStereoPinC3(NVDevEvoPtr pDevEvo,
                               NVEvoSubDevPtr pEvoSubDev,
                               NvU32 *pStereoPin)
{
    NVC370_CTRL_GET_LOCKPINS_CAPS_PARAMS params = { };

    params.base.subdeviceIndex = pEvoSubDev->subDeviceInstance;

    if (nvRmApiControl(nvEvoGlobal.clientHandle,
                       pDevEvo->displayHandle,
                       NVC370_CTRL_CMD_GET_LOCKPINS_CAPS,
                       &params, sizeof(params)) != NVOS_STATUS_SUCCESS) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                         "Failed to query stereo pin");
        return FALSE;
    }

    if ((params.stereoPin >= NV_EVO_NUM_LOCK_PIN_CAPS) ||
        (params.stereoPin == NVC370_CTRL_GET_LOCKPINS_CAPS_STEREO_PIN_NONE)) {
        return FALSE;
    } else {
        *pStereoPin = params.stereoPin;
        return TRUE;
    }
}

static void EvoParseCapabilityNotifier3(NVDevEvoPtr pDevEvo,
                                        NVEvoSubDevPtr pEvoSubDev,
                                        volatile const NvU32 *pCaps)
{
    NVEvoCapabilitiesPtr pEvoCaps = &pEvoSubDev->capabilities;
    const NvU32 sysCap = nvEvoReadCapReg3(pCaps, NVC373_SYS_CAP);
    const NvU32 sysCapB = nvEvoReadCapReg3(pCaps, NVC373_SYS_CAPB);
    NvU32 i, stereoPin;
    NvU32 layer;

    pDevEvo->caps.cursorCompositionCaps =
        (struct NvKmsCompositionCapabilities) {
            .supportedColorKeySelects =
                NVBIT(NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE),

            .colorKeySelect = {
                [NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE] = {
                    .supportedBlendModes = {
                        [1] = NV_EVO3_SUPPORTED_CURSOR_COMP_BLEND_MODES,
                    },
                },
            }
        };

    for (layer = 0;
         layer < ARRAY_LEN(pDevEvo->caps.layerCaps); layer++) {
        pDevEvo->caps.layerCaps[layer].composition =
            (struct NvKmsCompositionCapabilities) {
                .supportedColorKeySelects =
                    NVBIT(NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE) |
                    NVBIT(NVKMS_COMPOSITION_COLOR_KEY_SELECT_SRC) |
                    NVBIT(NVKMS_COMPOSITION_COLOR_KEY_SELECT_DST),

                .colorKeySelect = {
                    [NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE] = {
                        .supportedBlendModes = {
                            [1] = NV_EVO3_SUPPORTED_WINDOW_COMP_BLEND_MODES,
                        },
                    },

                    [NVKMS_COMPOSITION_COLOR_KEY_SELECT_SRC] = {
                        .supportedBlendModes = {
                            [0] = NV_EVO3_SUPPORTED_WINDOW_COMP_BLEND_MODES,
                            [1] = NV_EVO3_SUPPORTED_WINDOW_COMP_BLEND_MODES,
                        },
                    },

                    [NVKMS_COMPOSITION_COLOR_KEY_SELECT_DST] = {
                        .supportedBlendModes = {
                            [0] = NV_EVO3_SUPPORTED_WINDOW_COMP_BLEND_MODES,
                            [1] = NV_EVO3_SUPPORTED_WINDOW_COMP_BLEND_MODES,
                        },
                    },
                },
            };
    }

    /*
     * Previous EVO display implementations exposed capabilities for lock pins,
     * detailing which pin(s) could be used for which functions. The idea was
     * that it didn't make sense to try to drive a stereo pin with a fliplock
     * signal (for example), so the pin associated with the stereo function was
     * marked as stereo-capable but not any other function; attempting to use a
     * non-stereo-capable pin for stereo or vice-versa would result in an error.
     *
     * With nvdisplay, the meaning of lock pins was changed such that they no
     * longer have a shared namespace.  So stereo lockpin 0 is not the same as
     * fliplock lockpin 0 and neither is the same as scanlock lockpin 0.  With
     * this scheme, there is no way to specify a pin that is incapable of a
     * given function, so the entire capabilities mechanism was removed.
     *
     * However, the pins chosen for HEAD_SET_CONTROL still need to match the
     * pins selected for each function in the VBIOS DCB.  Fliplock and scanlock
     * query this information through
     * NV5070_CTRL_CMD_GET_FRAMELOCK_HEADER_LOCKPINS.  Stereo is handled
     * here, using NVC370_CTRL_CMD_GET_LOCKPINS_CAPS.
     */

    for (i = 0; i < NV_EVO_NUM_LOCK_PIN_CAPS; i++) {
        pEvoCaps->pin[i].flipLock = TRUE;
        pEvoCaps->pin[i].scanLock = TRUE;
    }

    if (QueryStereoPinC3(pDevEvo, pEvoSubDev, &stereoPin)) {
        pEvoCaps->pin[stereoPin].stereo = TRUE;
    }

    // Miscellaneous capabilities
    // NVDisplay does not support interlaced modes.
    pEvoCaps->misc.supportsInterlaced = FALSE;

/* XXX temporary WAR; see bug 4028718 */
#if !defined(NVC373_HEAD_CLK_CAP)
#define NVC373_HEAD_CLK_CAP(i)                                     (0x5e8+(i)*4) /* RW-4A */
#define NVC373_HEAD_CLK_CAP__SIZE_1                                            8 /*       */
#define NVC373_HEAD_CLK_CAP_PCLK_MAX                                         7:0 /* RWIUF */
#define NVC373_HEAD_CLK_CAP_PCLK_MAX_INIT                             0x00000085 /* RWI-V */
#endif

    // Heads
    ct_assert(ARRAY_LEN(pEvoCaps->head) >= NVC373_HEAD_CAPA__SIZE_1);
    for (i = 0; i < NVC373_HEAD_CAPA__SIZE_1; i++) {
        NVEvoHeadCaps *pHeadCaps = &pEvoCaps->head[i];

        pHeadCaps->usable =
            FLD_IDX_TEST_DRF(C373, _SYS_CAP, _HEAD_EXISTS, i, _YES, sysCap);
        if (pHeadCaps->usable) {
            pHeadCaps->maxPClkKHz =
                DRF_VAL(C373, _HEAD_CLK_CAP, _PCLK_MAX,
                        nvEvoReadCapReg3(pCaps, NVC373_HEAD_CLK_CAP(i))) * 10000;
        }

    }

    // SORs
    ct_assert(ARRAY_LEN(pEvoCaps->sor) >= NVC373_SOR_CAP__SIZE_1);
    for (i = 0; i < NVC373_SOR_CAP__SIZE_1; i++) {
        NVEvoSorCaps *pSorCaps = &pEvoCaps->sor[i];

        NvBool sorUsable =
            FLD_IDX_TEST_DRF(C373, _SYS_CAP, _SOR_EXISTS, i, _YES, sysCap);

        /* XXXnvdisplay: add SOR caps: max DP clk, ... */
        if (sorUsable) {
            const NvU32 sorCap = nvEvoReadCapReg3(pCaps, NVC373_SOR_CAP(i));
            pSorCaps->dualTMDS =
                FLD_TEST_DRF(C373, _SOR_CAP, _DUAL_TMDS, _TRUE, sorCap);

            /*
             * Assume that all SORs are equally capable, and that all SORs
             * support HDMI FRL if the display class supports it.  (If this
             * assert fires, we may need to rework SOR assignment for such HDMI
             * sinks.)
             *
             * Although HDMI_FRL is only defined for class C6, classes C3 and
             * C5 don't use that bit in the SOR_CAP register so it should
             * always be 0 on those chips.
             */
            nvAssert(!!FLD_TEST_DRF(C673, _SOR_CAP, _HDMI_FRL, _TRUE, sorCap) ==
                     !!pDevEvo->hal->caps.supportsHDMIFRL);

            pSorCaps->maxTMDSClkKHz =
                DRF_VAL(C373, _SOR_CLK_CAP, _TMDS_MAX,
                        nvEvoReadCapReg3(pCaps, NVC373_SOR_CLK_CAP(i))) * 10000;
        }
    }

    // Don't need any PIOR caps currently.

    // Windows
    ct_assert(ARRAY_LEN(pEvoCaps->window) >= NVC373_SYS_CAPB_WINDOW_EXISTS__SIZE_1);
    for (i = 0; i < NVC373_SYS_CAPB_WINDOW_EXISTS__SIZE_1; i++) {
        NVEvoWindowCaps *pWinCaps = &pEvoCaps->window[i];

        pWinCaps->usable =
            FLD_IDX_TEST_DRF(C373, _SYS_CAPB, _WINDOW_EXISTS, i, _YES, sysCapB);
    }
}

static void EvoParseCapabilityNotifierC3(NVDevEvoPtr pDevEvo,
                                         NVEvoSubDevPtr pEvoSubDev,
                                         volatile const NvU32 *pCaps)
{
    NVEvoCapabilitiesPtr pEvoCaps = &pEvoSubDev->capabilities;
    NvU32 i;

    // Miscellaneous capabilities
    pEvoCaps->misc.supportsSemiPlanar = FALSE;
    pEvoCaps->misc.supportsPlanar = FALSE;
    pEvoCaps->misc.supportsDSI = FALSE;

    // Heads
    ct_assert(ARRAY_LEN(pEvoCaps->head) >= NVC373_HEAD_CAPA__SIZE_1);
    for (i = 0; i < NVC373_HEAD_CAPA__SIZE_1; i++) {
        NVEvoHeadCaps *pHeadCaps = &pEvoCaps->head[i];

        /* XXXnvdisplay: add caps for hsat, ocsc, lut */
        if (pHeadCaps->usable) {
            NVEvoScalerCaps *pScalerCaps = &pHeadCaps->scalerCaps;

            pScalerCaps->present =
                FLD_TEST_DRF(C373, _HEAD_CAPA, _SCALER, _TRUE,
                             nvEvoReadCapReg3(pCaps, NVC373_HEAD_CAPA(i)));
            if (pScalerCaps->present) {
                NVEvoScalerTapsCaps *pTapsCaps;
                NvU32 tmp;

                /*
                 * Note that some of these may be zero (e.g., only 2-tap 444
                 * mode is supported on GV100, so the rest are all zero.
                 *
                 * Downscaling by more than 2x in either direction is not
                 * allowed by state error check for both horizontal and
                 * vertical 2-tap scaling.
                 *
                 * Downscaling by more than 4x in either direction is not
                 * allowed by argument error check (and state error check) for
                 * 5-tap scaling.
                 *
                 * 5-tap scaling is not implemented on GV100, though, so we
                 * should never see numTaps == 5 on GV100, and we can just use a
                 * max of 2 here all the time.
                 */

                /* 2-tap capabilities */
                tmp = nvEvoReadCapReg3(pCaps, NVC373_HEAD_CAPD(i));
                pTapsCaps = &pScalerCaps->taps[NV_EVO_SCALER_2TAPS];
                pTapsCaps->maxVDownscaleFactor = NV_EVO_SCALE_FACTOR_2X;
                pTapsCaps->maxHDownscaleFactor = NV_EVO_SCALE_FACTOR_2X;
                pTapsCaps->maxPixelsVTaps =
                    NV_MAX(DRF_VAL(C373, _HEAD_CAPD, _MAX_PIXELS_2TAP422, tmp),
                           DRF_VAL(C373, _HEAD_CAPD, _MAX_PIXELS_2TAP444, tmp));

                /*
                 * Note that there is a capability register for 1TAP, but there
                 * doesn't appear to be a way to select 1-tap scaling in the
                 * channel methods, so don't bother reading it for now.
                 */
            }
        }
    }
}

static void EvoParsePrecompScalerCaps5(NVEvoCapabilitiesPtr pEvoCaps,
                                       volatile const NvU32 *pCaps)
{
    int i;

    for (i = 0; i < NVC573_SYS_CAPB_WINDOW_EXISTS__SIZE_1; i++) {
        NVEvoWindowCaps *pWinCaps = &pEvoCaps->window[i];
        NVEvoScalerCaps *pScalerCaps = &pWinCaps->scalerCaps;
        NVEvoScalerTapsCaps *pTapsCaps;
        NvU32 capA = nvEvoReadCapReg3(pCaps, NVC573_PRECOMP_WIN_PIPE_HDR_CAPA(i));
        NvU32 capD, capF;

        pScalerCaps->present =
            FLD_TEST_DRF(C573, _PRECOMP_WIN_PIPE_HDR_CAPA, _SCLR_PRESENT,
                         _TRUE, capA);
        if (pScalerCaps->present) {
            capD = nvEvoReadCapReg3(pCaps, NVC573_PRECOMP_WIN_PIPE_HDR_CAPD(i));
            capF = nvEvoReadCapReg3(pCaps, NVC573_PRECOMP_WIN_PIPE_HDR_CAPF(i));

            /* 5-tap capabilities */
            pTapsCaps = &pScalerCaps->taps[NV_EVO_SCALER_5TAPS];
            if (FLD_TEST_DRF(C573, _PRECOMP_WIN_PIPE_HDR_CAPD,
                             _SCLR_VS_MAX_SCALE_FACTOR, _4X, capD)) {
                pTapsCaps->maxVDownscaleFactor = NV_EVO_SCALE_FACTOR_4X;
            } else {
                pTapsCaps->maxVDownscaleFactor = NV_EVO_SCALE_FACTOR_2X;
            }

            if (FLD_TEST_DRF(C573, _PRECOMP_WIN_PIPE_HDR_CAPD,
                             _SCLR_HS_MAX_SCALE_FACTOR, _4X, capD)) {
                pTapsCaps->maxHDownscaleFactor = NV_EVO_SCALE_FACTOR_4X;
            } else {
                pTapsCaps->maxHDownscaleFactor = NV_EVO_SCALE_FACTOR_2X;
            }

            pTapsCaps->maxPixelsVTaps =
                DRF_VAL(C573, _PRECOMP_WIN_PIPE_HDR_CAPF,
                        _VSCLR_MAX_PIXELS_5TAP, capF);

            /* 2-tap capabilities */
            pTapsCaps = &pScalerCaps->taps[NV_EVO_SCALER_2TAPS];
            pTapsCaps->maxVDownscaleFactor = NV_EVO_SCALE_FACTOR_2X;
            pTapsCaps->maxHDownscaleFactor = NV_EVO_SCALE_FACTOR_2X;
            pTapsCaps->maxPixelsVTaps =
            DRF_VAL(C573, _PRECOMP_WIN_PIPE_HDR_CAPF, _VSCLR_MAX_PIXELS_2TAP,
                    capF);
        }
    }
}

static void EvoParseCapabilityNotifierC5C6Common(NVEvoCapabilitiesPtr pEvoCaps,
                                                 volatile const NvU32 *pCaps)
{
    NvU32 i;
    NvBool postcompScalingSupported = FALSE;

    // Heads
    ct_assert(ARRAY_LEN(pEvoCaps->head) >= NVC573_SYS_CAP_HEAD_EXISTS__SIZE_1);
    for (i = 0; i < NVC573_SYS_CAP_HEAD_EXISTS__SIZE_1; i++) {
        NVEvoHeadCaps *pHeadCaps = &pEvoCaps->head[i];

        if (pHeadCaps->usable) {
            NVEvoScalerCaps *pScalerCaps = &pHeadCaps->scalerCaps;
            NVEvoScalerTapsCaps *pTapsCaps;
            NvU32 capA = nvEvoReadCapReg3(pCaps, NVC573_POSTCOMP_HEAD_HDR_CAPA(i));
            NvU32 capC, capD;

            pScalerCaps->present =
                FLD_TEST_DRF(C573, _POSTCOMP_HEAD_HDR_CAPA, _SCLR_PRESENT,
                             _TRUE, capA);
            if (pScalerCaps->present) {
                postcompScalingSupported = TRUE;

                capC = nvEvoReadCapReg3(pCaps, NVC573_POSTCOMP_HEAD_HDR_CAPC(i));
                capD = nvEvoReadCapReg3(pCaps, NVC573_POSTCOMP_HEAD_HDR_CAPD(i));

                /*
                 * Note that some of these may be zero.
                 *
                 * XXXnvdisplay: what about POSTCOMP_HEAD_HDR_CAPC_SCLR_*?
                 */

                /* 5-tap capabilities */
                pTapsCaps = &pScalerCaps->taps[NV_EVO_SCALER_5TAPS];
                if (FLD_TEST_DRF(C573, _POSTCOMP_HEAD_HDR_CAPC,
                                 _SCLR_VS_MAX_SCALE_FACTOR, _4X, capC)) {
                    pTapsCaps->maxVDownscaleFactor = NV_EVO_SCALE_FACTOR_4X;
                } else {
                    pTapsCaps->maxVDownscaleFactor = NV_EVO_SCALE_FACTOR_2X;
                }

                if (FLD_TEST_DRF(C573, _POSTCOMP_HEAD_HDR_CAPC,
                                 _SCLR_HS_MAX_SCALE_FACTOR, _4X, capC)) {
                    pTapsCaps->maxHDownscaleFactor = NV_EVO_SCALE_FACTOR_4X;
                } else {
                    pTapsCaps->maxHDownscaleFactor = NV_EVO_SCALE_FACTOR_2X;
                }

                pTapsCaps->maxPixelsVTaps =
                    DRF_VAL(C573, _POSTCOMP_HEAD_HDR_CAPD,
                    _VSCLR_MAX_PIXELS_5TAP, capD);

                /* 2-tap capabilities */
                pTapsCaps = &pScalerCaps->taps[NV_EVO_SCALER_2TAPS];
                pTapsCaps->maxVDownscaleFactor = NV_EVO_SCALE_FACTOR_2X;
                pTapsCaps->maxHDownscaleFactor = NV_EVO_SCALE_FACTOR_2X;
                pTapsCaps->maxPixelsVTaps =
                    DRF_VAL(C573, _POSTCOMP_HEAD_HDR_CAPD,
                            _VSCLR_MAX_PIXELS_2TAP, capD);
            }

#if defined(NV_DEBUG)
            NvU32 capA = nvEvoReadCapReg3(pCaps, NVC573_POSTCOMP_HEAD_HDR_CAPA(i));
            NvU32 unitWidth = DRF_VAL(C573, _POSTCOMP_HEAD_HDR_CAPA, _UNIT_WIDTH, capA);

            // EvoInitChannelC5 assumes 16-bit fixed-point.
            nvAssert(unitWidth == 16);
#endif
        }
    }

    for (i = 0; i < NVC573_SYS_CAPB_WINDOW_EXISTS__SIZE_1; i++) {
        NVEvoWindowCaps *pWinCaps = &pEvoCaps->window[i];
        NvU32 capA = nvEvoReadCapReg3(pCaps, NVC573_PRECOMP_WIN_PIPE_HDR_CAPA(i));

        pWinCaps->tmoPresent = FLD_TEST_DRF(C573, _PRECOMP_WIN_PIPE_HDR_CAPA,
                                            _TMO_PRESENT, _TRUE, capA);

        pWinCaps->csc0MatricesPresent =
            FLD_TEST_DRF(C573, _PRECOMP_WIN_PIPE_HDR_CAPA,
                         _CSC00_PRESENT, _TRUE, capA) &&
            FLD_TEST_DRF(C573, _PRECOMP_WIN_PIPE_HDR_CAPA,
                         _CSC01_PRESENT, _TRUE, capA);

        pWinCaps->csc10MatrixPresent =
            FLD_TEST_DRF(C573, _PRECOMP_WIN_PIPE_HDR_CAPA,
                          _CSC10_PRESENT, _TRUE, capA);
        pWinCaps->csc11MatrixPresent =
            FLD_TEST_DRF(C573, _PRECOMP_WIN_PIPE_HDR_CAPA,
                         _CSC11_PRESENT, _TRUE, capA);

        pWinCaps->cscLUTsPresent =
            FLD_TEST_DRF(C573, _PRECOMP_WIN_PIPE_HDR_CAPA,
                         _CSC0LUT_PRESENT, _TRUE, capA) &&
            FLD_TEST_DRF(C573, _PRECOMP_WIN_PIPE_HDR_CAPA,
                         _CSC1LUT_PRESENT, _TRUE, capA);

        nvAssert(!pWinCaps->tmoPresent ||
                 (pWinCaps->csc0MatricesPresent &&
                  pWinCaps->csc10MatrixPresent &&
                  pWinCaps->csc11MatrixPresent &&
                  pWinCaps->cscLUTsPresent));
    }

    /*
     * To keep the design simple, NVKMS will expose support for precomp scaling
     * iff postcomp scaling isn't supported. This means that on chips which have
     * both precomp and postcomp scalers (e.g., Turing), NVKMS will only report
     * that postcomp scaling is supported.
     */
    if (!postcompScalingSupported) {
        EvoParsePrecompScalerCaps5(pEvoCaps, pCaps);
    }

    // XXXnvdisplay3: add SOR caps for DP over USB
}

static void EvoParseCapabilityNotifierC5(NVDevEvoPtr pDevEvo,
                                         NVEvoSubDevPtr pEvoSubDev,
                                         volatile const NvU32 *pCaps)
{
    NVEvoCapabilitiesPtr pEvoCaps = &pEvoSubDev->capabilities;

    // Miscellaneous capabilities

    /*
     * On Turing, the NVC573_IHUB_COMMON_CAPA_SUPPORT_PLANAR bit actually
     * reports whether IHUB supports YUV _semi-planar_ formats.
     */
    pEvoCaps->misc.supportsSemiPlanar =
        FLD_TEST_DRF(C573, _IHUB_COMMON_CAPA, _SUPPORT_PLANAR, _TRUE,
                     nvEvoReadCapReg3(pCaps, NVC573_IHUB_COMMON_CAPA));
    pEvoCaps->misc.supportsDSI = FALSE;

    EvoParseCapabilityNotifierC5C6Common(pEvoCaps, pCaps);
}

void nvEvoParseCapabilityNotifier6(NVDevEvoPtr pDevEvo,
                                   NVEvoSubDevPtr pEvoSubDev,
                                   volatile const NvU32 *pCaps)
{
    NVEvoCapabilitiesPtr pEvoCaps = &pEvoSubDev->capabilities;
    NvU32 capC = nvEvoReadCapReg3(pCaps, NVC673_IHUB_COMMON_CAPC);
    NvU32 i;

    // Miscellaneous capabilities

    pEvoCaps->misc.supportsPlanar =
        FLD_TEST_DRF(C673, _IHUB_COMMON_CAPA, _SUPPORT_PLANAR, _TRUE,
                     nvEvoReadCapReg3(pCaps, NVC673_IHUB_COMMON_CAPA));

    pEvoCaps->misc.supportsSemiPlanar =
        FLD_TEST_DRF(C673, _IHUB_COMMON_CAPC, _SUPPORT_SEMI_PLANAR, _TRUE, capC);

    pEvoCaps->misc.supportsHVFlip =
        FLD_TEST_DRF(C673, _IHUB_COMMON_CAPC, _SUPPORT_HOR_VER_FLIP, _TRUE, capC);

    ct_assert(ARRAY_LEN(pEvoCaps->head) >= NVC673_SYS_CAP_HEAD_EXISTS__SIZE_1);

    // DSI is currently supported on just Orin, which has only 1 DSI engine (DSI0).
    pEvoCaps->misc.supportsDSI = 
        FLD_TEST_DRF(C673, _SYS_CAP, _DSI0_EXISTS, _YES, 
                     nvEvoReadCapReg3(pCaps, NVC673_SYS_CAP));

    for (i = 0; i < NVC673_SYS_CAP_HEAD_EXISTS__SIZE_1; i++) {
        NVEvoHeadCaps *pHeadCaps = &pEvoCaps->head[i];

        if (pHeadCaps->usable) {
            NvU32 capA = nvEvoReadCapReg3(pCaps, NVC673_POSTCOMP_HEAD_HDR_CAPA(i));
            NvBool hclpfPresent =
                FLD_TEST_DRF(C673, _POSTCOMP_HEAD_HDR_CAPA, _HCLPF_PRESENT,
                             _TRUE, capA);
            NvBool vfilterPresent =
                FLD_TEST_DRF(C673, _POSTCOMP_HEAD_HDR_CAPA, _VFILTER_PRESENT,
                             _TRUE, capA);

            pHeadCaps->supportsHDMIYUV420HW = hclpfPresent && vfilterPresent;
        }
    }

    EvoParseCapabilityNotifierC5C6Common(pEvoCaps, pCaps);
}

static NvU32 UsableWindowCount(const NVEvoCapabilities *pEvoCaps)
{
    NvU32 i, count = 0;

    for (i = 0; i < ARRAY_LEN(pEvoCaps->window); i++) {
        if (pEvoCaps->window[i].usable) {
            count++;
            /*
             * We expect usable windows to be contiguous and start at 0.
             * Check that the number of usable windows matches the
             * number of loop iterations.
             */
             nvAssert(count == (i + 1));
        }
    }

    return count;
}

static void FillLUTCaps(struct NvKmsLUTCaps *pCaps,
                        NvBool supported,
                        enum NvKmsLUTVssSupport vssSupport,
                        enum NvKmsLUTVssType vssType,
                        NvU32 vssSegments,
                        NvU32 lutEntries,
                        enum NvKmsLUTFormat entryFormat)
{
    pCaps->supported   = supported;
    pCaps->vssSupport  = supported ? vssSupport  : NVKMS_LUT_VSS_NOT_SUPPORTED;
    pCaps->vssType     = supported ? vssType     : NVKMS_LUT_VSS_TYPE_NONE;
    pCaps->vssSegments = supported ? vssSegments : 0;
    pCaps->lutEntries  = supported ? lutEntries  : 0;
    pCaps->entryFormat = supported ? entryFormat : NVKMS_LUT_FORMAT_UNORM14_WAR_813188;
}

static void SetHDRLayerCaps(NVDevEvoPtr pDevEvo)
{
    NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[0];
    NVEvoCapabilitiesPtr pEvoCaps = &pEvoSubDev->capabilities;
    NvU32 win;
#if defined(DEBUG)
    NvBool hdrLayerCapSet[NVKMS_MAX_LAYERS_PER_HEAD] = {FALSE};
#endif
    NvU32 numLayers[NVKMS_MAX_HEADS_PER_DISP] = {0};

    /*
     * XXX HDR: This assumes the window => layer mapping from
     * nvAllocCoreChannelEvo().
     *
     * TODO: Rework API to explicitly factor in window => layer mapping.
     */
    for (win = 0; win < pDevEvo->numWindows; win++) {
        NVEvoWindowCaps *pWinCaps = &pEvoCaps->window[win];
        const NvU32 head = pDevEvo->headForWindow[win];

        if (head == NV_INVALID_HEAD) {
            continue;
        }

        /*
         * XXX HDR: layerCaps assumes that every head has layers with the
         * same capabilities.
         *
         * TODO: Rework API for per-head layerCaps if this assert trips.
         */
        nvAssert(!hdrLayerCapSet[numLayers[head]] ||
                 (pDevEvo->caps.layerCaps[numLayers[head]].supportsICtCp ==
                  pWinCaps->tmoPresent));

        pDevEvo->caps.layerCaps[numLayers[head]].supportsICtCp = pWinCaps->tmoPresent;

        if (pDevEvo->hal->caps.needDefaultLutSurface) {
            /* Turing+ uses an FP16, linear 64-segment VSS supported ILUT */
            FillLUTCaps(&pDevEvo->caps.layerCaps[numLayers[head]].ilut, TRUE,
                        NVKMS_LUT_VSS_SUPPORTED, NVKMS_LUT_VSS_TYPE_LINEAR,
                        64, 1025, NVKMS_LUT_FORMAT_FP16);
        } else {
            /* Volta uses a UNORM14_WAR_813188, non-VSS ILUT */
            FillLUTCaps(&pDevEvo->caps.layerCaps[numLayers[head]].ilut, TRUE,
                        NVKMS_LUT_VSS_NOT_SUPPORTED, NVKMS_LUT_VSS_TYPE_NONE,
                        0, 1025, NVKMS_LUT_FORMAT_UNORM14_WAR_813188);
        }

        if (pWinCaps->tmoPresent) {
            FillLUTCaps(&pDevEvo->caps.layerCaps[numLayers[head]].tmo, TRUE,
                        NVKMS_LUT_VSS_REQUIRED, NVKMS_LUT_VSS_TYPE_LINEAR,
                        64, 1025, NVKMS_LUT_FORMAT_UNORM16);

        } else {
            FillLUTCaps(&pDevEvo->caps.layerCaps[numLayers[head]].tmo, FALSE,
                        NVKMS_LUT_VSS_NOT_SUPPORTED, NVKMS_LUT_VSS_TYPE_NONE,
                        0, 0, NVKMS_LUT_FORMAT_UNORM14_WAR_813188);
        }

#if defined(DEBUG)
        hdrLayerCapSet[numLayers[head]] = TRUE;
#endif

        numLayers[head]++;
    }

    if (pDevEvo->hal->caps.hasUnorm16OLUT) {
        /* Turing+ uses a UNORM16, logarithmic 33-segment VSS supported OLUT */
        FillLUTCaps(&pDevEvo->caps.olut, TRUE,
                    NVKMS_LUT_VSS_SUPPORTED, NVKMS_LUT_VSS_TYPE_LOGARITHMIC,
                    33, 1025, NVKMS_LUT_FORMAT_UNORM16);
    } else {
        /* Volta uses a UNORM14_WAR_813188, non-VSS OLUT */
        FillLUTCaps(&pDevEvo->caps.olut, TRUE,
                    NVKMS_LUT_VSS_NOT_SUPPORTED, NVKMS_LUT_VSS_TYPE_NONE,
                    0, 1025, NVKMS_LUT_FORMAT_UNORM14_WAR_813188);
    }
}

NvBool nvEvoGetCapabilities3(NVDevEvoPtr pDevEvo,
                             NVEvoParseCapabilityNotifierFunc3 *pParse,
                             NVEvoHwFormatFromKmsFormatFunc3 *pGetHwFmt,
                             NvU32 hwclass,
                             size_t length)
{
    NvU32 capsHandle = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);
    NVDispEvoPtr pDispEvo;
    NvU32 sd;
    NvU32 status;
    NvBool ret = FALSE;
    NvBool first = TRUE;
    NvBool supportsSemiPlanar = TRUE;
    NvBool supportsPlanar = TRUE;
    NvBool supportsHVFlip = TRUE;
    unsigned int i;
    enum NvKmsRotation curRotation;
    NvBool reflectionX;
    NvBool reflectionY;
    NvU8 layer;

    /* With nvdisplay, capabilities are exposed in a separate object. */
    status = nvRmApiAlloc(nvEvoGlobal.clientHandle,
                          pDevEvo->displayHandle,
                          capsHandle,
                          hwclass, NULL);
    if (status != NVOS_STATUS_SUCCESS) {
        nvAssert(!"Failed to allocate caps object");
        goto free_handle;
    }

    for (layer = 0;
         layer < ARRAY_LEN(pDevEvo->caps.layerCaps);
         layer++) {
        pDevEvo->caps.layerCaps[layer].supportsWindowMode = TRUE;
    }

    FOR_ALL_EVO_DISPLAYS(pDispEvo, sd, pDevEvo) {

        NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[sd];
        void *ptr;

        status = nvRmApiMapMemory(nvEvoGlobal.clientHandle,
                                  pDevEvo->pSubDevices[sd]->handle,
                                  capsHandle,
                                  0,
                                  length,
                                  &ptr,
                                  0);
        if (status != NVOS_STATUS_SUCCESS) {
            nvAssert(!"Failed to map caps memory");
            goto free_object;
        }

        nvkms_memset(&pEvoSubDev->capabilities, 0,
                     sizeof(pEvoSubDev->capabilities));

        EvoParseCapabilityNotifier3(pDevEvo, pEvoSubDev, ptr);
        pParse(pDevEvo, pEvoSubDev, ptr);

        status = nvRmApiUnmapMemory(nvEvoGlobal.clientHandle,
                                    pDevEvo->pSubDevices[sd]->handle,
                                    capsHandle, ptr, 0);
        if (status != NVOS_STATUS_SUCCESS) {
            nvAssert(!"Failed to unmap caps memory");
        }

        if (first) {
            pDevEvo->numWindows =
                UsableWindowCount(&pEvoSubDev->capabilities);
            first = FALSE;
        } else {
            /* Assert that each subdevice has the same number of windows. */
            nvAssert(pDevEvo->numWindows ==
                     UsableWindowCount(&pEvoSubDev->capabilities));
        }

        /*
         * Expose YUV semi-planar iff all of the disps belonging to pDevEvo
         * support it.
         */
        supportsSemiPlanar &=
            pEvoSubDev->capabilities.misc.supportsSemiPlanar;

        /*
         * Expose YUV planar iff all of the disps belonging to pDevEvo
         * support it.
         */
        supportsPlanar &=
            pEvoSubDev->capabilities.misc.supportsPlanar;

        supportsHVFlip &=
            pEvoSubDev->capabilities.misc.supportsHVFlip;
    }

    SetHDRLayerCaps(pDevEvo);

    for (i = NvKmsSurfaceMemoryFormatMin;
         i <= NvKmsSurfaceMemoryFormatMax;
         i++) {
        const NvKmsSurfaceMemoryFormatInfo *pFormatInfo =
            nvKmsGetSurfaceMemoryFormatInfo(i);

        if ((pFormatInfo->numPlanes == 2 && !supportsSemiPlanar) ||
            (pFormatInfo->numPlanes == 3 && !supportsPlanar)) {
            continue;
        }

        if (pGetHwFmt(i) != 0) {
            NvU8 layer;

            for (layer = 0;
                 layer < ARRAY_LEN(pDevEvo->caps.layerCaps);
                 layer++) {
                pDevEvo->caps.layerCaps[layer].supportedSurfaceMemoryFormats |=
                    NVBIT64(i);
            }
        }
    }

    for (reflectionX = FALSE;
         reflectionX <= TRUE;
         reflectionX++) {

        for (reflectionY = FALSE;
             reflectionY <= TRUE;
             reflectionY++) {

            for (curRotation = NVKMS_ROTATION_MIN;
                 curRotation <= NVKMS_ROTATION_MAX;
                 curRotation++) {
                struct NvKmsRRParams rrParams = { curRotation,
                                                  reflectionX,
                                                  reflectionY };
                NvU8 bitPosition;

                if ((reflectionX || reflectionY) && !supportsHVFlip) {
                    continue;
                }

                if (curRotation == NVKMS_ROTATION_180 && !supportsHVFlip) {
                    continue;
                }

                /*
                 * Skipping over rotations by 90 and 270 degrees
                 * because these rotations require support for
                 * SCAN_COLUMN rotation, which hasn't been added
                 * to NVKMS yet.
                 */
                if (curRotation == NVKMS_ROTATION_90 ||
                    curRotation == NVKMS_ROTATION_270) {
                    continue;
                }

                bitPosition = NvKmsRRParamsToCapBit(&rrParams);
                pDevEvo->caps.validLayerRRTransforms |= NVBIT(bitPosition);
            }
        }
    }

    ret = TRUE;

free_object:
    status = nvRmApiFree(nvEvoGlobal.clientHandle,
                         pDevEvo->displayHandle,
                         capsHandle);
    if (status != NVOS_STATUS_SUCCESS) {
        nvAssert(!"Failed to free caps object");
    }

free_handle:
    nvFreeUnixRmHandle(&pDevEvo->handleAllocator, capsHandle);

    return ret;
}

static NvBool EvoGetCapabilitiesC3(NVDevEvoPtr pDevEvo)
{
    return nvEvoGetCapabilities3(pDevEvo, EvoParseCapabilityNotifierC3,
                                 nvHwFormatFromKmsFormatC3,
                                 NVC373_DISP_CAPABILITIES,
                                 sizeof(_NvC373DispCapabilities));
}

static NvBool EvoGetCapabilitiesC5(NVDevEvoPtr pDevEvo)
{
    return nvEvoGetCapabilities3(pDevEvo, EvoParseCapabilityNotifierC5,
                                 nvHwFormatFromKmsFormatC5,
                                 NVC573_DISP_CAPABILITIES,
                                 sizeof(_NvC573DispCapabilities));
}

NvBool nvEvoGetCapabilitiesC6(NVDevEvoPtr pDevEvo)
{
    return nvEvoGetCapabilities3(pDevEvo,
                                 nvEvoParseCapabilityNotifier6,
                                 nvHwFormatFromKmsFormatC6,
                                 NVC673_DISP_CAPABILITIES,
                                 sizeof(_NvC673DispCapabilities));
}

static void EvoSetViewportPointInC3(NVDevEvoPtr pDevEvo, const int head,
                                    NvU16 x, NvU16 y,
                                    NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;

    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    /* Set the input viewport point */
    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_VIEWPORT_POINT_IN(head), 1);
    nvDmaSetEvoMethodData(pChannel, DRF_NUM(C37D, _HEAD_SET_VIEWPORT_POINT_IN, _X, x) |
                             DRF_NUM(C37D, _HEAD_SET_VIEWPORT_POINT_IN, _Y, y));
    /* XXXnvdisplay set ViewportValidPointIn to configure overfetch */
}

static void EvoSetOutputScalerC3(const NVDispEvoRec *pDispEvo, const NvU32 head,
                                 const NvU32 imageSharpeningValue,
                                 NVEvoUpdateState *updateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoChannelPtr pChannel = pDevEvo->core;
    const NVDispHeadStateEvoRec *pHeadState = &pDispEvo->headState[head];
    const NVHwModeViewPortEvo *pViewPort = &pHeadState->timings.viewPort;

    /* These methods should only apply to a single pDpyEvo */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    NvU32 vTaps = pViewPort->vTaps > NV_EVO_SCALER_2TAPS ?
                    NVC37D_HEAD_SET_CONTROL_OUTPUT_SCALER_VERTICAL_TAPS_TAPS_5 :
                    NVC37D_HEAD_SET_CONTROL_OUTPUT_SCALER_VERTICAL_TAPS_TAPS_2;
    NvU32 hTaps = pViewPort->hTaps > NV_EVO_SCALER_2TAPS ?
                    NVC37D_HEAD_SET_CONTROL_OUTPUT_SCALER_HORIZONTAL_TAPS_TAPS_5 :
                    NVC37D_HEAD_SET_CONTROL_OUTPUT_SCALER_HORIZONTAL_TAPS_TAPS_2;

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_CONTROL_OUTPUT_SCALER(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37D, _HEAD_SET_CONTROL_OUTPUT_SCALER, _VERTICAL_TAPS, vTaps) |
        DRF_NUM(C37D, _HEAD_SET_CONTROL_OUTPUT_SCALER, _HORIZONTAL_TAPS, hTaps));
}

static NvBool EvoSetViewportInOut3(NVDevEvoPtr pDevEvo, const int head,
                                   const NVHwModeViewPortEvo *pViewPortMin,
                                   const NVHwModeViewPortEvo *pViewPort,
                                   const NVHwModeViewPortEvo *pViewPortMax,
                                   NVEvoUpdateState *updateState,
                                   NvU32 setWindowUsageBounds)
{
    const NVEvoCapabilitiesPtr pEvoCaps = &pDevEvo->gpus[0].capabilities;
    NVEvoChannelPtr pChannel = pDevEvo->core;
    struct NvKmsScalingUsageBounds scalingUsageBounds = { };
    NvU32 win;

    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    /* The input viewport shouldn't vary. */
    nvAssert(pViewPortMin->in.width == pViewPort->in.width);
    nvAssert(pViewPortMax->in.width == pViewPort->in.width);
    nvAssert(pViewPortMin->in.height == pViewPort->in.height);
    nvAssert(pViewPortMax->in.height == pViewPort->in.height);
    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_VIEWPORT_SIZE_IN(head), 1);
    nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(C37D, _HEAD_SET_VIEWPORT_SIZE_IN, _WIDTH, pViewPort->in.width) |
            DRF_NUM(C37D, _HEAD_SET_VIEWPORT_SIZE_IN, _HEIGHT, pViewPort->in.height));
    /* XXXnvdisplay set ViewportValidSizeIn to configure overfetch */

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_VIEWPORT_POINT_OUT_ADJUST(head), 1);
    nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(C37D, _HEAD_SET_VIEWPORT_POINT_OUT, _ADJUST_X, pViewPort->out.xAdjust) |
            DRF_NUM(C37D, _HEAD_SET_VIEWPORT_POINT_OUT, _ADJUST_Y, pViewPort->out.yAdjust));

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_VIEWPORT_SIZE_OUT(head), 1);
    nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(C37D, _HEAD_SET_VIEWPORT_SIZE_OUT, _WIDTH, pViewPort->out.width) |
            DRF_NUM(C37D, _HEAD_SET_VIEWPORT_SIZE_OUT, _HEIGHT, pViewPort->out.height));

    /* XXXnvdisplay deal with pViewPortMin, pViewPortMax */

    if (!nvComputeScalingUsageBounds(&pEvoCaps->head[head].scalerCaps,
                                   pViewPort->in.width, pViewPort->in.height,
                                   pViewPort->out.width, pViewPort->out.height,
                                   pViewPort->hTaps, pViewPort->vTaps,
                                   &scalingUsageBounds)) {
        /* Should have been rejected by validation */
        nvAssert(!"Attempt to program invalid viewport");
    }

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_MAX_OUTPUT_SCALE_FACTOR(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37D, _HEAD_SET_MAX_OUTPUT_SCALE_FACTOR, _HORIZONTAL,
                scalingUsageBounds.maxHDownscaleFactor) |
        DRF_NUM(C37D, _HEAD_SET_MAX_OUTPUT_SCALE_FACTOR, _VERTICAL,
                scalingUsageBounds.maxVDownscaleFactor));

    /*
     * Program MAX_PIXELS_FETCHED_PER_LINE window usage bounds
     * for each window that is attached to the head.
     *
     * Precomp will clip the post-scaled window to the input viewport, reverse-scale
     * this cropped size back to the input surface domain, and isohub will fetch
     * this cropped size. This function assumes that there's no window scaling yet,
     * so the MAX_PIXELS_FETCHED_PER_LINE will be bounded by the input viewport
     * width. SetScalingUsageBoundsOneWindow5() will take care of updating
     * MAX_PIXELS_FETCHED_PER_LINE, if window scaling is enabled later.
     *
     * Program MAX_PIXELS_FETCHED_PER_LINE for each window that is attached to
     * head. For Turing+, SetScalingUsageBoundsOneWindow5() will take care of
     * programming window usage bounds only for the layers/windows in use.
     */
    setWindowUsageBounds |=
        DRF_NUM(C37D, _WINDOW_SET_WINDOW_USAGE_BOUNDS, _MAX_PIXELS_FETCHED_PER_LINE,
                nvGetMaxPixelsFetchedPerLine(pViewPort->in.width,
                NV_EVO_SCALE_FACTOR_1X));

    for (win = 0; win < pDevEvo->numWindows; win++) {
        if (head != pDevEvo->headForWindow[win]) {
            continue;
        }

        nvDmaSetStartEvoMethod(pChannel, NVC37D_WINDOW_SET_WINDOW_USAGE_BOUNDS(win), 1);
        nvDmaSetEvoMethodData(pChannel, setWindowUsageBounds);
    }

    return scalingUsageBounds.vUpscalingAllowed;
}

static void EvoSetViewportInOutC3(NVDevEvoPtr pDevEvo, const int head,
                                  const NVHwModeViewPortEvo *pViewPortMin,
                                  const NVHwModeViewPortEvo *pViewPort,
                                  const NVHwModeViewPortEvo *pViewPortMax,
                                  NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvBool verticalUpscalingAllowed =
        EvoSetViewportInOut3(pDevEvo, head, pViewPortMin, pViewPort,
                             pViewPortMax, updateState,
                             NV_EVO3_DEFAULT_WINDOW_USAGE_BOUNDS_C3);

    nvDmaSetStartEvoMethod(pChannel,
        NVC37D_HEAD_SET_HEAD_USAGE_BOUNDS(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_DEF(C37D, _HEAD_SET_HEAD_USAGE_BOUNDS, _CURSOR, _USAGE_W256_H256) |
        DRF_DEF(C37D, _HEAD_SET_HEAD_USAGE_BOUNDS, _OUTPUT_LUT, _USAGE_1025) |
        (verticalUpscalingAllowed ?
            DRF_DEF(C37D, _HEAD_SET_HEAD_USAGE_BOUNDS, _UPSCALING_ALLOWED, _TRUE) :
            DRF_DEF(C37D, _HEAD_SET_HEAD_USAGE_BOUNDS, _UPSCALING_ALLOWED, _FALSE)));
}

static void EvoSetViewportInOutC5(NVDevEvoPtr pDevEvo, const int head,
                                  const NVHwModeViewPortEvo *pViewPortMin,
                                  const NVHwModeViewPortEvo *pViewPort,
                                  const NVHwModeViewPortEvo *pViewPortMax,
                                  NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvU32 setWindowUsageBounds =
        (NV_EVO3_DEFAULT_WINDOW_USAGE_BOUNDS_C5 |
         DRF_DEF(C57D, _WINDOW_SET_WINDOW_USAGE_BOUNDS, _INPUT_SCALER_TAPS, _TAPS_2) |
         DRF_DEF(C57D, _WINDOW_SET_WINDOW_USAGE_BOUNDS, _UPSCALING_ALLOWED, _FALSE));
    NvU32 verticalUpscalingAllowed =
        EvoSetViewportInOut3(pDevEvo, head, pViewPortMin, pViewPort,
                             pViewPortMax, updateState, setWindowUsageBounds);

    nvDmaSetStartEvoMethod(pChannel,
        NVC57D_HEAD_SET_HEAD_USAGE_BOUNDS(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_DEF(C57D, _HEAD_SET_HEAD_USAGE_BOUNDS, _CURSOR, _USAGE_W256_H256) |
        DRF_DEF(C57D, _HEAD_SET_HEAD_USAGE_BOUNDS, _OLUT_ALLOWED, _TRUE) |
        /* Despite the generic name of this field, it's specific to vertical taps. */
        (pViewPort->vTaps > NV_EVO_SCALER_2TAPS ?
            DRF_DEF(C57D, _HEAD_SET_HEAD_USAGE_BOUNDS, _OUTPUT_SCALER_TAPS, _TAPS_5) :
            DRF_DEF(C57D, _HEAD_SET_HEAD_USAGE_BOUNDS, _OUTPUT_SCALER_TAPS, _TAPS_2)) |
        (verticalUpscalingAllowed ?
            DRF_DEF(C57D, _HEAD_SET_HEAD_USAGE_BOUNDS, _UPSCALING_ALLOWED, _TRUE) :
            DRF_DEF(C57D, _HEAD_SET_HEAD_USAGE_BOUNDS, _UPSCALING_ALLOWED, _FALSE)));
}

/*!
 * Compute the C37D_HEAD_SET_CONTROL_CURSOR method value.
 *
 * This function also validates that the given NVSurfaceEvoRec can be
 * used as a cursor image.

 *
 * \param[in]  pDevEvo      The device on which the cursor will be programmed.
 * \param[in]  pSurfaceEvo  The surface to be used as the cursor image.
 * \param[out] pValue       The C37D_HEAD_SET_CONTROL_CURSOR method value.

 * \return  If TRUE, the surface can be used as a cursor image, and
 *          pValue contains the method value.  If FALSE, the surface
 *          cannot be used as a cursor image.
 */
NvBool nvEvoGetHeadSetControlCursorValueC3(const NVDevEvoRec *pDevEvo,
                                                const NVSurfaceEvoRec *pSurfaceEvo,
                                                NvU32 *pValue)
{
    NvU32 plane, numPlanes;
    NvU64 minRequiredSize = 0;
    NvU32 value = 0;

    if (pSurfaceEvo == NULL) {
        value |= DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR, _ENABLE, _DISABLE);
        value |= DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR, _FORMAT, _A8R8G8B8);
        goto done;
    } else {
        value |= DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR, _ENABLE, _ENABLE);
    }

    /* The cursor must always be pitch. */

    if (pSurfaceEvo->layout != NvKmsSurfaceMemoryLayoutPitch) {
        return FALSE;
    }

    /*
     * The only supported cursor image memory format is A8R8G8B8.
     */
    if (pSurfaceEvo->format == NvKmsSurfaceMemoryFormatA8R8G8B8) {
        value |= DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR, _FORMAT, _A8R8G8B8);
    } else {
        return FALSE;
    }

    numPlanes = nvKmsGetSurfaceMemoryFormatInfo(pSurfaceEvo->format)->numPlanes;

    /*
     * The cursor only supports a few image sizes.
     *
     * Compute minRequiredSize as widthInPixels x heightInPixels x 4 bytes per
     * pixel, except for 32x32: we require a minimum pitch of 256, so we use
     * that instead of widthInPixels x 4.
     */
    if ((pSurfaceEvo->widthInPixels == 32) &&
        (pSurfaceEvo->heightInPixels == 32)) {
        value |= DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR, _SIZE, _W32_H32);
        minRequiredSize = 256 * 32;
    } else if ((pSurfaceEvo->widthInPixels == 64) &&
               (pSurfaceEvo->heightInPixels == 64)) {
        value |= DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR, _SIZE, _W64_H64);
        minRequiredSize = 64 * 64 * 4;
    } else if ((pDevEvo->cursorHal->caps.maxSize >= 128) &&
               (pSurfaceEvo->widthInPixels == 128) &&
               (pSurfaceEvo->heightInPixels == 128)) {
        value |= DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR, _SIZE, _W128_H128);
        minRequiredSize = 128 * 128 * 4;
    } else if ((pDevEvo->cursorHal->caps.maxSize >= 256) &&
               (pSurfaceEvo->widthInPixels == 256) &&
               (pSurfaceEvo->heightInPixels == 256)) {
        value |= DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR, _SIZE, _W256_H256);
        minRequiredSize = 256 * 256 * 4;
    } else {
        return FALSE;
    }

    /* The surface size cannot be smaller than the required minimum. */

    for (plane = 0; plane < numPlanes; plane++) {
        if (pSurfaceEvo->planes[plane].rmObjectSizeInBytes < minRequiredSize) {
            return FALSE;
        }
    }

    /*
     * Hard code the cursor hotspot.
     */
    value |= DRF_NUM(C37D, _HEAD_SET_CONTROL_CURSOR, _HOT_SPOT_Y, 0);
    value |= DRF_NUM(C37D, _HEAD_SET_CONTROL_CURSOR, _HOT_SPOT_X, 0);

    // XXXnvdisplay: Add support for cursor de-gamma.

done:

    if (pValue != NULL) {
        *pValue = value;
    }

    return TRUE;
}

static void SetCursorSurfaceAddress(
    NVEvoChannelPtr pChannel,
    const NVSurfaceDescriptor *pSurfaceDesc,
    NvU32 offset,
    NvU32 head)
{
    NvU32 ctxDmaHandle = pSurfaceDesc ? pSurfaceDesc->ctxDmaHandle : 0;

    nvAssert(!pSurfaceDesc || ctxDmaHandle);

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_CONTEXT_DMA_CURSOR(head, 0), 4);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37D, _HEAD_SET_CONTEXT_DMA_CURSOR, _HANDLE, ctxDmaHandle));

    // Always set the right cursor context DMA.
    // HW will just ignore this if it is not in stereo cursor mode.
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37D, _HEAD_SET_CONTEXT_DMA_CURSOR, _HANDLE, ctxDmaHandle));
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37D, _HEAD_SET_OFFSET_CURSOR, _ORIGIN,
                nvCtxDmaOffsetFromBytes(offset)));
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37D, _HEAD_SET_OFFSET_CURSOR, _ORIGIN,
                nvCtxDmaOffsetFromBytes(offset)));
}

static void EvoSetCursorImageC3(NVDevEvoPtr pDevEvo, const int head,
                                const NVSurfaceEvoRec *pSurfaceEvo,
                                NVEvoUpdateState *updateState,
                                const struct NvKmsCompositionParams *pCursorCompParams)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;
    const NVSurfaceDescriptor *pSurfaceDesc =
        pSurfaceEvo ? &pSurfaceEvo->planes[0].surfaceDesc : NULL;
    const NvU64 offset = pSurfaceEvo ? pSurfaceEvo->planes[0].offset : 0;
    NvU32 headSetControlCursorValue = 0;
    NvBool ret;

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);
    nvAssert(pCursorCompParams->colorKeySelect ==
                NVKMS_COMPOSITION_COLOR_KEY_SELECT_DISABLE);
    nvAssert(NVBIT(pCursorCompParams->blendingMode[1]) &
                NV_EVO3_SUPPORTED_CURSOR_COMP_BLEND_MODES);
    /* These methods should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    ret = nvEvoGetHeadSetControlCursorValueC3(pDevEvo, pSurfaceEvo,
                                            &headSetControlCursorValue);
    /*
     * The caller should have already validated the surface, so there
     * shouldn't be a failure.
     */
    if (!ret) {
        nvAssert(!"Could not construct HEAD_SET_CONTROL_CURSOR value");
    }

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_PRESENT_CONTROL_CURSOR(head), 1);
    nvDmaSetEvoMethodData(pChannel,
            DRF_DEF(C37D, _HEAD_SET_PRESENT_CONTROL_CURSOR, _MODE, _MONO));

    SetCursorSurfaceAddress(pChannel, pSurfaceDesc, offset, head);

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_CONTROL_CURSOR(head), 1);
    nvDmaSetEvoMethodData(pChannel, headSetControlCursorValue);

    nvDmaSetStartEvoMethod(pChannel,
            NVC37D_HEAD_SET_CONTROL_CURSOR_COMPOSITION(head), 1);
    switch (pCursorCompParams->blendingMode[1]) {
    case NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE:
        nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(C37D, _HEAD_SET_CONTROL_CURSOR_COMPOSITION, _K1, 255) |
            DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR_COMPOSITION,
                    _CURSOR_COLOR_FACTOR_SELECT, _K1) |
            DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR_COMPOSITION,
                    _VIEWPORT_COLOR_FACTOR_SELECT, _ZERO) |
            DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR_COMPOSITION, _MODE, _BLEND));
        break;
    case NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_ALPHA:
        nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(C37D, _HEAD_SET_CONTROL_CURSOR_COMPOSITION, _K1, 255) |
            DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR_COMPOSITION,
                    _CURSOR_COLOR_FACTOR_SELECT, _K1_TIMES_SRC) |
            DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR_COMPOSITION,
                    _VIEWPORT_COLOR_FACTOR_SELECT, _NEG_K1_TIMES_SRC) |
            DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR_COMPOSITION, _MODE, _BLEND));
        break;
    case NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_ALPHA:
        nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(C37D, _HEAD_SET_CONTROL_CURSOR_COMPOSITION, _K1, 255) |
            DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR_COMPOSITION,
                    _CURSOR_COLOR_FACTOR_SELECT, _K1) |
            DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR_COMPOSITION,
                    _VIEWPORT_COLOR_FACTOR_SELECT, _NEG_K1_TIMES_SRC) |
            DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR_COMPOSITION, _MODE, _BLEND));
        break;
    case NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_SURFACE_ALPHA:
        nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(C37D, _HEAD_SET_CONTROL_CURSOR_COMPOSITION, _K1,
                    pCursorCompParams->surfaceAlpha) |
            DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR_COMPOSITION,
                    _CURSOR_COLOR_FACTOR_SELECT, _K1_TIMES_SRC) |
            DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR_COMPOSITION,
                    _VIEWPORT_COLOR_FACTOR_SELECT, _NEG_K1_TIMES_SRC) |
            DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR_COMPOSITION, _MODE, _BLEND));
        break;
    case NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_SURFACE_ALPHA:
        nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(C37D, _HEAD_SET_CONTROL_CURSOR_COMPOSITION, _K1,
                    pCursorCompParams->surfaceAlpha) |
            DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR_COMPOSITION,
                    _CURSOR_COLOR_FACTOR_SELECT, _K1) |
            DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR_COMPOSITION,
                    _VIEWPORT_COLOR_FACTOR_SELECT, _NEG_K1_TIMES_SRC) |
            DRF_DEF(C37D, _HEAD_SET_CONTROL_CURSOR_COMPOSITION, _MODE, _BLEND));
        break;
    default:
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
            "%s: composition mode %d not supported for cursor",
            __func__, pCursorCompParams->blendingMode[1]);
        break;
    }
}

NvBool nvEvoValidateCursorSurfaceC3(const NVDevEvoRec *pDevEvo,
                                         const NVSurfaceEvoRec *pSurfaceEvo)
{
    return nvEvoGetHeadSetControlCursorValueC3(pDevEvo, pSurfaceEvo, NULL);
}

static NvBool ValidateWindowFormatSourceRectC3(
    const struct NvKmsRect *sourceFetchRect,
    const enum NvKmsSurfaceMemoryFormat format)
{
    const NvKmsSurfaceMemoryFormatInfo *pFormatInfo =
        nvKmsGetSurfaceMemoryFormatInfo(format);

    /*
     * sourceFetchRect represents the dimensions of the source fetch rectangle.
     * If YUV crop and scaler overfetch are supported, it is up to the caller to
     * provide the correct dimensions (e.g., ValidSizeIn/ValidPointIn vs.
     * SizeIn/PointIn).
     *
     * For all YUV formats, the position and size of the fetch rectangle must be
     * even in the horizontal direction.
     *
     * For YUV420 formats, there is an additional restriction that the position
     * and size of the fetch rectangle must be even in the vertical direction as
     * well.
     */
    if (pFormatInfo->isYUV) {
        if (((sourceFetchRect->x & 1) != 0) ||
            (sourceFetchRect->width & 1) != 0) {
            return FALSE;
        }

        if (pFormatInfo->yuv.vertChromaDecimationFactor > 1) {
            if (((sourceFetchRect->y & 1) != 0) ||
                (sourceFetchRect->height & 1) != 0) {
                return FALSE;
            }
        }
    }

    return TRUE;
}

typedef typeof(ValidateWindowFormatSourceRectC3) val_src_rect_t;

static NvBool EvoValidateWindowFormatWrapper(
    const enum NvKmsSurfaceMemoryFormat format,
    NVEvoHwFormatFromKmsFormatFunc3 *pGetHwFmt,
    const struct NvKmsRect *sourceFetchRect,
    val_src_rect_t *pValSrcRect,
    NvU32 *hwFormatOut)
{
    const NvU32 hwFormat = pGetHwFmt(format);

    if (hwFormat == 0) {
        return FALSE;
    }

    if (hwFormatOut != NULL) {
        *hwFormatOut = hwFormat;
    }

    /*
     * If sourceFetchRect is NULL, this function is only responsible for
     * verifying whether the given NvKmsSurfaceMemoryFormat has a corresponding
     * HW format.
     */
    if (sourceFetchRect == NULL) {
        return TRUE;
    }

    return pValSrcRect(sourceFetchRect, format);
}

static NvBool EvoValidateWindowFormatC3(
    const enum NvKmsSurfaceMemoryFormat format,
    const struct NvKmsRect *sourceFetchRect,
    NvU32 *hwFormatOut)
{
    return EvoValidateWindowFormatWrapper(
            format,
            nvHwFormatFromKmsFormatC3,
            sourceFetchRect,
            ValidateWindowFormatSourceRectC3,
            hwFormatOut);
}

static NvBool EvoValidateWindowFormatC5(
    const enum NvKmsSurfaceMemoryFormat format,
    const struct NvKmsRect *sourceFetchRect,
    NvU32 *hwFormatOut)
{
    return EvoValidateWindowFormatWrapper(
            format,
            nvHwFormatFromKmsFormatC5,
            sourceFetchRect,
            ValidateWindowFormatSourceRectC3,
            hwFormatOut);
}

NvBool nvEvoValidateWindowFormatC6(
    const enum NvKmsSurfaceMemoryFormat format,
    const struct NvKmsRect *sourceFetchRect,
    NvU32 *hwFormatOut)
{
    return EvoValidateWindowFormatWrapper(
            format,
            nvHwFormatFromKmsFormatC6,
            sourceFetchRect,
            ValidateWindowFormatSourceRectC3,
            hwFormatOut);
}

static NvU32 OffsetForNotifier(int idx)
{
    /* NVDisplay notifiers are always the 16-byte variety.  We only care about
     * the NV_DISP_NOTIFIER__0 dword which contains the status. */
    NvU32 base = idx * (NV_DISP_NOTIFIER_SIZEOF / sizeof(NvU32));
    return base + NV_DISP_NOTIFIER__0;
}

void nvEvoInitCompNotifierC3(const NVDispEvoRec *pDispEvo, int idx)
{
    nvWriteEvoCoreNotifier(pDispEvo, OffsetForNotifier(idx),
                           DRF_DEF(_DISP, _NOTIFIER__0, _STATUS, _NOT_BEGUN));
}

NvBool nvEvoIsCompNotifierCompleteC3(NVDispEvoPtr pDispEvo, int idx) {
    return nvEvoIsCoreNotifierComplete(pDispEvo, OffsetForNotifier(idx),
                                       DRF_BASE(NV_DISP_NOTIFIER__0_STATUS),
                                       DRF_EXTENT(NV_DISP_NOTIFIER__0_STATUS),
                                       NV_DISP_NOTIFIER__0_STATUS_FINISHED);
}

void nvEvoWaitForCompNotifierC3(const NVDispEvoRec *pDispEvo, int idx)
{
    nvEvoWaitForCoreNotifier(pDispEvo, OffsetForNotifier(idx),
                             DRF_BASE(NV_DISP_NOTIFIER__0_STATUS),
                             DRF_EXTENT(NV_DISP_NOTIFIER__0_STATUS),
                             NV_DISP_NOTIFIER__0_STATUS_FINISHED);
}

static void EvoSetDitherC3(NVDispEvoPtr pDispEvo, const int head,
                           const NvBool enabled, const NvU32 type,
                           const NvU32 algo,
                           NVEvoUpdateState *updateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvU32 ditherControl;

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    if (enabled) {
        ditherControl = DRF_DEF(C37D, _HEAD_SET_DITHER_CONTROL, _ENABLE, _ENABLE);

        switch (type) {
        case NV0073_CTRL_SPECIFIC_OR_DITHER_TYPE_6_BITS:
            ditherControl |=
                DRF_DEF(C37D, _HEAD_SET_DITHER_CONTROL, _BITS, _TO_6_BITS);
            break;
        case NV0073_CTRL_SPECIFIC_OR_DITHER_TYPE_8_BITS:
            ditherControl |=
                DRF_DEF(C37D, _HEAD_SET_DITHER_CONTROL, _BITS, _TO_8_BITS);
            break;
        /* XXXnvdisplay: Support DITHER_TO_{10,12}_BITS (see also bug 1729668). */
        default:
            nvAssert(!"Unknown ditherType");
            // Fall through
        case NV0073_CTRL_SPECIFIC_OR_DITHER_TYPE_OFF:
            ditherControl = NVC37D_HEAD_SET_DITHER_CONTROL_ENABLE_DISABLE;
            break;
        }

    } else {
        ditherControl = DRF_DEF(C37D, _HEAD_SET_DITHER_CONTROL, _ENABLE, _DISABLE);
    }

    switch (algo) {
    case NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_STATIC_ERR_ACC:
        ditherControl |=
            DRF_DEF(C37D, _HEAD_SET_DITHER_CONTROL, _MODE, _STATIC_ERR_ACC);
        break;
    case NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_DYNAMIC_2X2:
        ditherControl |=
            DRF_DEF(C37D, _HEAD_SET_DITHER_CONTROL, _MODE, _DYNAMIC_2X2);
        break;
    case NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_STATIC_2X2:
        ditherControl |=
            DRF_DEF(C37D, _HEAD_SET_DITHER_CONTROL, _MODE, _STATIC_2X2);
        break;
    case NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_TEMPORAL:
        ditherControl |=
            DRF_DEF(C37D, _HEAD_SET_DITHER_CONTROL, _MODE, _TEMPORAL);
        break;
    default:
        nvAssert(!"Unknown DitherAlgo");
        // Fall through
    case NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_UNKNOWN:
    case NV0073_CTRL_SPECIFIC_OR_DITHER_ALGO_DYNAMIC_ERR_ACC:
        ditherControl |=
            DRF_DEF(C37D, _HEAD_SET_DITHER_CONTROL, _MODE, _DYNAMIC_ERR_ACC);
        break;
    }

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_DITHER_CONTROL(head), 1);
    nvDmaSetEvoMethodData(pChannel, ditherControl);
}

static void EvoSetDisplayRateC3(NVDispEvoPtr pDispEvo, const int head,
                                NvBool enable,
                                NVEvoUpdateState *updateState,
                                NvU32 timeoutMicroseconds)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoChannelPtr pChannel = pDevEvo->core;

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    if (enable) {
        timeoutMicroseconds =
            NV_MIN(timeoutMicroseconds,
                   DRF_MASK(NVC37D_HEAD_SET_DISPLAY_RATE_MIN_REFRESH_INTERVAL));

        nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_DISPLAY_RATE(head), 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_DEF(C37D, _HEAD_SET_DISPLAY_RATE, _RUN_MODE, _ONE_SHOT) |
            DRF_NUM(C37D, _HEAD_SET_DISPLAY_RATE, _MIN_REFRESH_INTERVAL,
                    timeoutMicroseconds) |
            (timeoutMicroseconds == 0 ?
                DRF_DEF(C37D, _HEAD_SET_DISPLAY_RATE, _MIN_REFRESH, _DISABLE) :
                DRF_DEF(C37D, _HEAD_SET_DISPLAY_RATE, _MIN_REFRESH, _ENABLE)));
    } else {
        nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_DISPLAY_RATE(head), 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_DEF(C37D, _HEAD_SET_DISPLAY_RATE, _RUN_MODE, _CONTINUOUS));
    }
}

static void EvoSetStallLockC3(NVDispEvoPtr pDispEvo, const int head,
                              NvBool enable, NVEvoUpdateState *updateState)
{
    NVDevEvoPtr pDevEvo = pDispEvo->pDevEvo;
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NVEvoSubDevPtr pEvoSubDev = &pDevEvo->gpus[pDispEvo->displayOwner];
    NVEvoHeadControlPtr pHC = &pEvoSubDev->headControl[head];
    NvU32 data = 0x0;

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    if (pHC->crashLockUnstallMode) {
        data |= DRF_DEF(C37D, _HEAD_SET_STALL_LOCK, _UNSTALL_MODE, _CRASH_LOCK);
    } else {
        data |= DRF_DEF(C37D, _HEAD_SET_STALL_LOCK, _UNSTALL_MODE, _LINE_LOCK);
    }

    if (enable) {
        data |= DRF_DEF(C37D, _HEAD_SET_STALL_LOCK, _ENABLE, _TRUE) |
                DRF_DEF(C37D, _HEAD_SET_STALL_LOCK, _MODE, _ONE_SHOT);

        if (!pHC->useStallLockPin) {
            data |= DRF_DEF(C37D, _HEAD_SET_STALL_LOCK, _LOCK_PIN, _LOCK_PIN_NONE);
        } else  if (NV_EVO_LOCK_PIN_IS_INTERNAL(pHC->stallLockPin)) {
            NvU32 pin = pHC->stallLockPin - NV_EVO_LOCK_PIN_INTERNAL_0;
            data |= DRF_NUM(C37D, _HEAD_SET_STALL_LOCK, _LOCK_PIN,
                            NVC37D_HEAD_SET_STALL_LOCK_LOCK_PIN_INTERNAL_SCAN_LOCK(pin));
        } else {
            NvU32 pin = pHC->stallLockPin - NV_EVO_LOCK_PIN_0;
            data |= DRF_NUM(C37D, _HEAD_SET_STALL_LOCK, _LOCK_PIN,
                            NVC37D_HEAD_SET_STALL_LOCK_LOCK_PIN_LOCK_PIN(pin));
        }
    } else {
        data |= DRF_DEF(C37D, _HEAD_SET_STALL_LOCK, _ENABLE, _FALSE);
    }

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_STALL_LOCK(head), 1);
    nvDmaSetEvoMethodData(pChannel, data);
}

static NvBool GetChannelState(NVDevEvoPtr pDevEvo,
                              NVEvoChannelPtr pChan,
                              NvU32 sd,
                              NvU32 *result)
{
    NVC370_CTRL_CMD_GET_CHANNEL_INFO_PARAMS info = { };
    NvU32 ret;

    info.base.subdeviceIndex = sd;
    info.channelClass = pChan->hwclass;
    info.channelInstance = pChan->instance;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayHandle,
                         NVC370_CTRL_CMD_GET_CHANNEL_INFO,
                         &info, sizeof(info));
    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDev(pDevEvo, EVO_LOG_ERROR,
                    "Failed to query display engine channel state: 0x%08x:%d:%d:0x%08x",
                    pChan->hwclass, pChan->instance, sd, ret);
        return FALSE;
    }

    *result = info.channelState;

    return TRUE;
}

NvBool nvEvoIsChannelIdleC3(NVDevEvoPtr pDevEvo,
                                 NVEvoChannelPtr pChan,
                                 NvU32 sd,
                                 NvBool *result)
{
    NvU32 channelState;

    if (!GetChannelState(pDevEvo, pChan, sd, &channelState)) {
        return FALSE;
    }

    *result = (channelState == NVC370_CTRL_GET_CHANNEL_INFO_STATE_IDLE);

    return TRUE;
}

NvBool nvEvoIsChannelMethodPendingC3(NVDevEvoPtr pDevEvo,
                                          NVEvoChannelPtr pChan,
                                          NvU32 sd,
                                          NvBool *result)
{
    NvBool tmpResult;

    /* With C370, Idle and NoMethodPending are equivalent. */
    ct_assert(NVC370_CTRL_GET_CHANNEL_INFO_STATE_IDLE ==
              NVC370_CTRL_GET_CHANNEL_INFO_STATE_NO_METHOD_PENDING);

    if (!nvEvoIsChannelIdleC3(pDevEvo, pChan, sd, &tmpResult)) {
        return FALSE;
    }

    *result = !tmpResult;

    return TRUE;
}

NvBool nvEvoAllocRmCtrlObjectC3(NVDevEvoPtr pDevEvo)
{
    const NvU32 handle = nvGenerateUnixRmHandle(&pDevEvo->handleAllocator);

    /* Note that this object is not at all related to the GF100_DISP_SW (9072)
     * or NV50_DISPLAY_SW (5072) objects, despite their similarity in name. */
    NvU32 status = nvRmApiAlloc(nvEvoGlobal.clientHandle,
                                pDevEvo->deviceHandle,
                                handle,
                                NVC372_DISPLAY_SW, NULL);
    if (status != NVOS_STATUS_SUCCESS) {
        nvAssert(!"Failed to allocate nvdisplay rmctrl object");
        goto fail;
    }

    pDevEvo->rmCtrlHandle = handle;

    return TRUE;

fail:
    nvFreeUnixRmHandle(&pDevEvo->handleAllocator, handle);
    return FALSE;
}

static NvU32 GetAccelerators(
    NVDevEvoPtr pDevEvo,
    NVEvoChannelPtr pChannel,
    NvU32 sd)
{
    NVC370_CTRL_GET_ACCL_PARAMS params = { };
    NvU32 ret;

    params.base.subdeviceIndex = sd;
    params.channelClass = pChannel->hwclass;
    nvAssert(pChannel->channelMask & NV_EVO_CHANNEL_MASK_WINDOW_ALL);
    params.channelInstance =
        NV_EVO_CHANNEL_MASK_WINDOW_NUMBER(pChannel->channelMask);

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayHandle,
                         NVC370_CTRL_CMD_GET_ACCL,
                         &params, sizeof(params));
    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                         "Failed to retrieve accelerators");
        return 0;
    }

    return params.accelerators;
}

static NvBool SetAccelerators(
    NVDevEvoPtr pDevEvo,
    NVEvoChannelPtr pChannel,
    NvU32 sd,
    NvU32 accelerators,
    NvU32 accelMask)
{
    NVC370_CTRL_SET_ACCL_PARAMS params = { };
    NvU32 ret;

    params.base.subdeviceIndex = sd;
    params.channelClass = pChannel->hwclass;
    nvAssert(pChannel->channelMask & NV_EVO_CHANNEL_MASK_WINDOW_ALL);
    params.channelInstance =
        NV_EVO_CHANNEL_MASK_WINDOW_NUMBER(pChannel->channelMask);
    params.accelerators = accelerators;
    params.accelMask = accelMask;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->displayHandle,
                         NVC370_CTRL_CMD_SET_ACCL,
                         &params, sizeof(params));
    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                         "Failed to set accelerators");
        return FALSE;
    }

    return TRUE;
}

void nvEvoAccelerateChannelC3(NVDevEvoPtr pDevEvo,
                                   NVEvoChannelPtr pChannel,
                                   const NvU32 sd,
                                   const NvBool trashPendingMethods,
                                   const NvBool unblockMethodsInExecutation,
                                   NvU32 *pOldAccelerators)
{
    NvU32 accelMask = 0x0;

    if (trashPendingMethods) {
        accelMask |= NVC370_CTRL_ACCL_TRASH_ONLY;
    }

    /* Start with a conservative set of accelerators; may need to add more
     * later. */
    if (unblockMethodsInExecutation) {
        accelMask |= NVC370_CTRL_ACCL_IGNORE_PI |
                     NVC370_CTRL_ACCL_SKIP_SEMA |
                     NVC370_CTRL_ACCL_IGNORE_FLIPLOCK;
    }

    if (accelMask == 0x0) {
        return;
    }

    *pOldAccelerators = GetAccelerators(pDevEvo, pChannel, sd);

    /* Accelerate window channel. */
    if (!SetAccelerators(pDevEvo, pChannel, sd, accelMask, accelMask)) {
        nvAssert(!"Failed to set accelerators");
    }
}

void nvEvoResetChannelAcceleratorsC3(NVDevEvoPtr pDevEvo,
                                          NVEvoChannelPtr pChannel,
                                          const NvU32 sd,
                                          const NvBool trashPendingMethods,
                                          const NvBool unblockMethodsInExecutation,
                                          NvU32 oldAccelerators)
{
    NvU32 accelMask = 0x0;

    if (trashPendingMethods) {
        accelMask |= NVC370_CTRL_ACCL_TRASH_ONLY;
    }

    /* Start with a conservative set of accelerators; may need to add more
     * later. */
    if (unblockMethodsInExecutation) {
        accelMask |= NVC370_CTRL_ACCL_IGNORE_PI |
                     NVC370_CTRL_ACCL_SKIP_SEMA |
                     NVC370_CTRL_ACCL_IGNORE_FLIPLOCK;
    }

    if (accelMask == 0x0) {
        return;
    }

    /* Accelerate window channel. */
    if (!SetAccelerators(pDevEvo, pChannel, sd, oldAccelerators, accelMask)) {
        nvAssert(!"Failed to set accelerators");
    }
}

static void ForceFlipToNull(
    NVDevEvoPtr pDevEvo,
    NVEvoChannelPtr pChannel,
    NvU32 sd,
    NVEvoUpdateState *updateState,
    const NVFlipChannelEvoHwState *pNullHwState)
{
    const NvU32 subDeviceMask = (1 << sd);

    nvPushEvoSubDevMask(pDevEvo, subDeviceMask);

    pDevEvo->hal->Flip(pDevEvo, pChannel, pNullHwState, updateState,
                       FALSE /* bypassComposition */);

    nvPopEvoSubDevMask(pDevEvo);
}

static NvBool PollForChannelIdle(
    NVDevEvoPtr pDevEvo,
    NVEvoChannelPtr pChannel,
    NvU32 sd)
{
    const NvU32 timeout = 2000000; // 2 seconds
    NvU64 startTime = 0;
    NvBool isMethodPending = TRUE;

    do {
        if (!nvEvoIsChannelMethodPendingC3(pDevEvo, pChannel, sd,
                                         &isMethodPending)) {
            break;
        }

        if (!isMethodPending) {
            break;
        }

        if (nvExceedsTimeoutUSec(pDevEvo, &startTime, timeout)) {
            return FALSE;
        }

        nvkms_yield();

    } while (TRUE);

    return TRUE;
}

/*!
 * This function emulates the behavior of the STOP_BASE/STOP_OVERLAY RM control
 * calls for pre-EVO hardware.
 *
 * STOP_BASE/STOP_OVERLAY will apply hardware channel accelerators, push
 * methods via the debug interface to NULL context DMAs, and wait for the
 * channel to go idle (which means the surface programmed into the core channel
 * will become visible).
 *
 * If we asked RM to do the same thing for the window channel that is emulating
 * the base channel on nvdisplay, the display would just go black: there's no
 * surface in the core channel, so NULLing the context DMA in the window
 * channel will disable both "core" and "base".
 *
 * So instead, similar functionality is implemented here: we apply
 * accelerators, push methods to flip to core, and wait for the channel to
 * idle.
 */
typedef struct {
    struct {
        NvU32 accelerators;
        NvBool overridden;
    } window[NVKMS_MAX_WINDOWS_PER_DISP];
    NVFlipChannelEvoHwState nullEvoHwState;
} EvoIdleChannelAcceleratorState;

static NvBool EvoForceIdleSatelliteChannelsWithAccel(
    NVDevEvoPtr pDevEvo,
    const NVEvoIdleChannelState *idleChannelState,
    const NvU32 accelMask)
{
    NvU32 sd, window;
    NVEvoUpdateState updateState = { };
    NvBool ret = FALSE;

    EvoIdleChannelAcceleratorState *pAcceleratorState = nvCalloc(
        pDevEvo->numSubDevices, sizeof(EvoIdleChannelAcceleratorState));

    if (!pAcceleratorState) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                         "Failed to alloc accelerator state");
        return FALSE;
    }

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        /*
         * Forcing a channel to be idle is currently only implemented for window
         * channels.
         */
        if ((idleChannelState->subdev[sd].channelMask &
             ~NV_EVO_CHANNEL_MASK_WINDOW_ALL) != 0) {

            nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                             "Forcing non-window channel idle not implemented");
            goto done;
        }

        for (window = 0; window < pDevEvo->numWindows; window++) {
            if (FLD_IDX_TEST_DRF64(_EVO, _CHANNEL_MASK,
                                   _WINDOW, window, _ENABLE,
                                   idleChannelState->subdev[sd].channelMask)) {
                NVEvoChannelPtr pChannel = pDevEvo->window[window];

                /* Save old window channel accelerators. */
                NvU32 oldAccel = GetAccelerators(pDevEvo, pChannel, sd);

                pAcceleratorState[sd].window[window].accelerators =
                    oldAccel;

                /* Accelerate window channel. */
                if (!SetAccelerators(pDevEvo, pChannel, sd, accelMask,
                                     accelMask)) {
                    nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                                     "Failed to set accelerators");
                    goto done;
                }
                pAcceleratorState[sd].window[window].overridden = TRUE;

                /*
                 * Push a flip to null in this channel.
                 *
                 * XXX nullEvoHwState isn't a valid state for NULL flip, for
                 * example 'csc' will be all 0s instead of the identity. This
                 * will also lead to the HW state being out of sync with the SW
                 * state.
                 */
                ForceFlipToNull(pDevEvo, pChannel, sd, &updateState,
                                &pAcceleratorState->nullEvoHwState);
            }
        }
    }

    /* Push one update for all of the flips programmed above. */
    nvEvoUpdateC3(pDevEvo, &updateState, TRUE /* releaseElv */);

    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        for (window = 0; window < pDevEvo->numWindows; window++) {
            if (FLD_IDX_TEST_DRF64(_EVO, _CHANNEL_MASK, _WINDOW, window, _ENABLE,
                                   idleChannelState->subdev[sd].channelMask)) {
                NVEvoChannelPtr pChannel = pDevEvo->window[window];

                /* Wait for the flips to complete. */
                if (!PollForChannelIdle(pDevEvo, pChannel, sd)) {
                    nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                                     "Timed out while idling base channel");
                    goto done;
                }
            }
        }
    }

    ret = TRUE;

done:
    for (sd = 0; sd < pDevEvo->numSubDevices; sd++) {
        for (window = 0; window < pDevEvo->numWindows; window++) {
            if (FLD_IDX_TEST_DRF64(_EVO, _CHANNEL_MASK, _WINDOW, window, _ENABLE,
                                   idleChannelState->subdev[sd].channelMask)) {
                NVEvoChannelPtr pChannel = pDevEvo->window[window];

                const NvU32 oldAccel =
                    pAcceleratorState[sd].window[window].accelerators;

                if (!pAcceleratorState[sd].window[window].overridden) {
                    continue;
                }

                /* Restore window channel accelerators. */
                if (!SetAccelerators(pDevEvo, pChannel, sd, oldAccel,
                                     accelMask)) {
                    nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                                     "Failed to restore accelerators");
                }
            }
        }
    }

    nvFree(pAcceleratorState);
    return ret;
}

NvBool nvEvoForceIdleSatelliteChannelC3(
    NVDevEvoPtr pDevEvo,
    const NVEvoIdleChannelState *idleChannelState)
{
    /* Start with a conservative set of accelerators; may need to add more
     * later. */
    const NvU32 accelMask =
        NVC370_CTRL_ACCL_IGNORE_PI |
        NVC370_CTRL_ACCL_SKIP_SEMA;

    return EvoForceIdleSatelliteChannelsWithAccel(pDevEvo,
                                                  idleChannelState,
                                                  accelMask);
}

NvBool nvEvoForceIdleSatelliteChannelIgnoreLockC3(
    NVDevEvoPtr pDevEvo,
    const NVEvoIdleChannelState *idleChannelState)
{
    const NvU32 accelMask =
        NVC370_CTRL_ACCL_IGNORE_PI |
        NVC370_CTRL_ACCL_SKIP_SEMA |
        NVC370_CTRL_ACCL_IGNORE_FLIPLOCK |
        NVC370_CTRL_ACCL_IGNORE_INTERLOCK;

    return EvoForceIdleSatelliteChannelsWithAccel(pDevEvo,
                                                  idleChannelState,
                                                  accelMask);
}

void nvEvoFreeRmCtrlObjectC3(NVDevEvoPtr pDevEvo)
{
    if (pDevEvo->rmCtrlHandle) {
        NvU32 status;

        status = nvRmApiFree(nvEvoGlobal.clientHandle,
                             pDevEvo->deviceHandle,
                             pDevEvo->rmCtrlHandle);

        if (status != NVOS_STATUS_SUCCESS) {
            nvAssert(!"Failed to free nvdisplay rmctrl object");
        }

        nvFreeUnixRmHandle(&pDevEvo->handleAllocator, pDevEvo->rmCtrlHandle);
        pDevEvo->rmCtrlHandle = 0;
    }
}

void nvEvoSetImmPointOutC3(NVDevEvoPtr pDevEvo,
                                NVEvoChannelPtr pChannel,
                                NvU32 sd,
                                NVEvoUpdateState *updateState,
                                NvU16 x, NvU16 y)
{
    NVEvoChannelPtr pImmChannel = pChannel->imm.u.dma;

    nvAssert((pChannel->channelMask & NV_EVO_CHANNEL_MASK_WINDOW_ALL) != 0);
    nvAssert(pChannel->imm.type == NV_EVO_IMM_CHANNEL_DMA);

    /* This should only be called for one GPU at a time, since the
     * pre-nvdisplay version uses PIO and cannot broadcast. */
    nvAssert(ONEBITSET(nvPeekEvoSubDevMask(pDevEvo)));

    nvDmaSetStartEvoMethod(pImmChannel,
        NVC37B_SET_POINT_OUT(0 /* Left eye */), 1);

    nvDmaSetEvoMethodData(pImmChannel,
        DRF_NUM(C37B, _SET_POINT_OUT, _X, x) |
        DRF_NUM(C37B, _SET_POINT_OUT, _Y, y));

    nvWinImmChannelUpdateState(pDevEvo, updateState, pChannel);
}

static void SetCrcSurfaceAddress(
    NVEvoChannelPtr pChannel,
    const NVSurfaceDescriptor *pSurfaceDesc,
    NvU32 head)
{
    NvU32 ctxDmaHandle = pSurfaceDesc ? pSurfaceDesc->ctxDmaHandle : 0;

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_CONTEXT_DMA_CRC(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37D, _HEAD_SET_CONTEXT_DMA_CRC, _HANDLE, ctxDmaHandle));
}

static void EvoStartHeadCRC32CaptureC3(NVDevEvoPtr pDevEvo,
                                       NVEvoDmaPtr pDma,
                                       NVConnectorEvoPtr pConnectorEvo,
                                       const enum nvKmsTimingsProtocol protocol,
                                       const NvU32 orIndex,
                                       NvU32 head,
                                       NvU32 sd,
                                       NVEvoUpdateState *updateState)
{
    const NvU32 winChannel = head << 1;
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvU32 orOutput = 0;

    /* These method should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    /* The window channel should fit in
     * NVC37D_HEAD_SET_CRC_CONTROL_CONTROLLING_CHANNEL */
    nvAssert(winChannel < DRF_MASK(NVC37D_HEAD_SET_CRC_CONTROL_CONTROLLING_CHANNEL));

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    switch (pConnectorEvo->or.type) {
    case NV0073_CTRL_SPECIFIC_OR_TYPE_SOR:
        if (protocol == NVKMS_PROTOCOL_SOR_DP_A ||
            protocol == NVKMS_PROTOCOL_SOR_DP_B) {
            orOutput = NVC37D_HEAD_SET_CRC_CONTROL_PRIMARY_CRC_SF;
        } else {
            orOutput =
                NVC37D_HEAD_SET_CRC_CONTROL_PRIMARY_CRC_SOR(orIndex);
        }
        break;
    case NV0073_CTRL_SPECIFIC_OR_TYPE_PIOR:
        orOutput =
            NVC37D_HEAD_SET_CRC_CONTROL_PRIMARY_CRC_PIOR(orIndex);
        break;
    case NV0073_CTRL_SPECIFIC_OR_TYPE_DAC:
        /* No DAC support on nvdisplay.  Fall through. */
    default:
        nvAssert(!"Invalid pConnectorEvo->or.type");
        break;
    }

    SetCrcSurfaceAddress(pChannel, &pDma->surfaceDesc, head);

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_CRC_CONTROL(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C37D, _HEAD_SET_CRC_CONTROL, _PRIMARY_CRC, orOutput) |
        DRF_DEF(C37D, _HEAD_SET_CRC_CONTROL, _SECONDARY_CRC, _NONE) |
        DRF_NUM(C37D, _HEAD_SET_CRC_CONTROL, _CONTROLLING_CHANNEL, winChannel) |
        DRF_DEF(C37D, _HEAD_SET_CRC_CONTROL, _EXPECT_BUFFER_COLLAPSE, _FALSE) |
        DRF_DEF(C37D, _HEAD_SET_CRC_CONTROL, _CRC_DURING_SNOOZE, _DISABLE));

    /* Reset the CRC notifier */
    nvEvoResetCRC32Notifier(pDma->subDeviceAddress[sd],
                            NVC37D_NOTIFIER_CRC_STATUS_0,
                            DRF_BASE(NVC37D_NOTIFIER_CRC_STATUS_0_DONE),
                            NVC37D_NOTIFIER_CRC_STATUS_0_DONE_FALSE);
}

static void EvoStopHeadCRC32CaptureC3(NVDevEvoPtr pDevEvo,
                                      NvU32 head,
                                      NVEvoUpdateState *updateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;

    /* These method should only apply to a single pDpy */
    nvAssert(pDevEvo->subDevMaskStackDepth > 0);

    nvUpdateUpdateState(pDevEvo, updateState, pChannel);

    SetCrcSurfaceAddress(pChannel, NULL /* pSurfaceDesc */, head);

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_CRC_CONTROL(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_DEF(C37D, _HEAD_SET_CRC_CONTROL, _PRIMARY_CRC, _NONE) |
        DRF_DEF(C37D, _HEAD_SET_CRC_CONTROL, _SECONDARY_CRC, _NONE) |
        DRF_NUM(C37D, _HEAD_SET_CRC_CONTROL, _CONTROLLING_CHANNEL, 0) |
        DRF_DEF(C37D, _HEAD_SET_CRC_CONTROL, _EXPECT_BUFFER_COLLAPSE, _FALSE) |
        DRF_DEF(C37D, _HEAD_SET_CRC_CONTROL, _CRC_DURING_SNOOZE, _DISABLE));
}

/*!
 * Queries the current head's CRC Notifier and returns values if successful
 *
 * First waits for hardware to finish writing to the CRC32Notifier,
 * and performs a read of the Compositor, SF/OR CRCs,
 * and the RG CRC in numCRC32 frames.
 * Crc fields in input array crc32 should be calloc'd to 0s.
 *
 * \param[in]  pDevEvo          NVKMS device pointer
 * \param[in]  pDma             Pointer to DMA-mapped memory
 * \param[in]  sd               Subdevice index
 * \param[in]  entry_count      Number of independent frames to read CRCs from
 * \param[out] crc32            Contains pointers to CRC output arrays
 * \param[out] numCRC32         Number of CRC frames successfully read from DMA
 *
 * \return  Returns TRUE if was able to successfully read CRCs from DMA,
 *          otherwise FALSE
 */
NvBool nvEvoQueryHeadCRC32_C3(NVDevEvoPtr pDevEvo,
                                   NVEvoDmaPtr pDma,
                                   NvU32 sd,
                                   NvU32 entry_count,
                                   CRC32NotifierCrcOut *crc32,
                                   NvU32 *numCRC32)
{
    volatile NvU32 *pCRC32Notifier = pDma->subDeviceAddress[sd];
    const NvU32 entry_stride =
          NVC37D_NOTIFIER_CRC_CRC_ENTRY1_21 - NVC37D_NOTIFIER_CRC_CRC_ENTRY0_13;
    // Define how many/which variables to read from each CRCNotifierEntry struct
    const CRC32NotifierEntryRec field_info[NV_EVO3_NUM_CRC_FIELDS] = {
        {
            .field_offset = NVC37D_NOTIFIER_CRC_CRC_ENTRY0_11,
            .field_base_bit =
             DRF_BASE(NVC37D_NOTIFIER_CRC_CRC_ENTRY0_11_COMPOSITOR_CRC),
            .field_extent_bit =
             DRF_EXTENT(NVC37D_NOTIFIER_CRC_CRC_ENTRY0_11_COMPOSITOR_CRC),
            .field_frame_values = crc32->compositorCrc32,
        },
        {
            .field_offset = NVC37D_NOTIFIER_CRC_CRC_ENTRY0_12,
            .field_base_bit =
             DRF_BASE(NVC37D_NOTIFIER_CRC_CRC_ENTRY0_12_RG_CRC),
            .field_extent_bit =
             DRF_EXTENT(NVC37D_NOTIFIER_CRC_CRC_ENTRY0_12_RG_CRC),
            .field_frame_values = crc32->rasterGeneratorCrc32,
        },
        {
            .field_offset = NVC37D_NOTIFIER_CRC_CRC_ENTRY0_13,
            .field_base_bit =
             DRF_BASE(NVC37D_NOTIFIER_CRC_CRC_ENTRY0_13_PRIMARY_OUTPUT_CRC),
            .field_extent_bit =
             DRF_EXTENT(NVC37D_NOTIFIER_CRC_CRC_ENTRY0_13_PRIMARY_OUTPUT_CRC),
            .field_frame_values = crc32->outputCrc32
        }
    };

    const CRC32NotifierEntryFlags flag_info[NV_EVO3_NUM_CRC_FLAGS] = {
        {
            .flag_base_bit =
             DRF_BASE(NVC37D_NOTIFIER_CRC_STATUS_0_COUNT),
            .flag_extent_bit =
             DRF_EXTENT(NVC37D_NOTIFIER_CRC_STATUS_0_COUNT),
            .flag_type = NVEvoCrc32NotifierFlagCount
        },
        {
            .flag_base_bit =
             DRF_BASE(NVC37D_NOTIFIER_CRC_STATUS_0_COMPOSITOR_OVERFLOW),
            .flag_extent_bit =
             DRF_EXTENT(NVC37D_NOTIFIER_CRC_STATUS_0_COMPOSITOR_OVERFLOW),
            .flag_type = NVEvoCrc32NotifierFlagCrcOverflow
        },
        {
            .flag_base_bit =
             DRF_BASE(NVC37D_NOTIFIER_CRC_STATUS_0_RG_OVERFLOW),
            .flag_extent_bit =
             DRF_EXTENT(NVC37D_NOTIFIER_CRC_STATUS_0_RG_OVERFLOW),
            .flag_type = NVEvoCrc32NotifierFlagCrcOverflow
        },
        {
            .flag_base_bit =
             DRF_BASE(NVC37D_NOTIFIER_CRC_STATUS_0_PRIMARY_OUTPUT_OVERFLOW),
            .flag_extent_bit =
             DRF_EXTENT(NVC37D_NOTIFIER_CRC_STATUS_0_PRIMARY_OUTPUT_OVERFLOW),
            .flag_type = NVEvoCrc32NotifierFlagCrcOverflow
        }
    };

    if (!nvEvoWaitForCRC32Notifier(pDevEvo,
                                   pCRC32Notifier,
                                   NVC37D_NOTIFIER_CRC_STATUS_0,
                                   DRF_BASE(NVC37D_NOTIFIER_CRC_STATUS_0_DONE),
                                   DRF_EXTENT(NVC37D_NOTIFIER_CRC_STATUS_0_DONE),
                                   NVC37D_NOTIFIER_CRC_STATUS_0_DONE_TRUE)) {
        return FALSE;
    }

    *numCRC32 = nvEvoReadCRC32Notifier(pCRC32Notifier,
                                       entry_stride,
                                       entry_count,
                                       NVC37D_NOTIFIER_CRC_STATUS_0, /* Status offset */
                                       NV_EVO3_NUM_CRC_FIELDS,
                                       NV_EVO3_NUM_CRC_FLAGS,
                                       field_info,
                                       flag_info);

    nvEvoResetCRC32Notifier(pCRC32Notifier,
                            NVC37D_NOTIFIER_CRC_STATUS_0,
                            DRF_BASE(NVC37D_NOTIFIER_CRC_STATUS_0_DONE),
                            NVC37D_NOTIFIER_CRC_STATUS_0_DONE_FALSE);

    return TRUE;
}

void nvEvoGetScanLineC3(const NVDispEvoRec *pDispEvo,
                             const NvU32 head,
                             NvU16 *pScanLine,
                             NvBool *pInBlankingPeriod)
{
    const NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    const NvU32 sd = pDispEvo->displayOwner;
    const NvU32 window = head << 1;
    void *pDma = pDevEvo->window[window]->pb.control[sd];
    const NvU32 scanLine = nvDmaLoadPioMethod(pDma, NVC37E_GET_LINE);

    if ((scanLine & NVBIT(15)) == 0) {
        *pInBlankingPeriod = FALSE;
        *pScanLine = scanLine & DRF_MASK(14:0);
    } else {
        *pInBlankingPeriod = TRUE;
    }
}

/*
 * This method configures and programs the RG Core Semaphores. Default behavior
 * is to continuously trigger on the specified rasterline when enabled.
 */
static void
EvoConfigureVblankSyncObjectC6(const NVDevEvoPtr pDevEvo,
                               const NvU16 rasterLine,
                               const NvU32 head,
                               const NvU32 semaphoreIndex,
                               const NVSurfaceDescriptor *pSurfaceDesc,
                               NVEvoUpdateState* pUpdateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;

    /*
     * Populate the NVEvoUpdateState for the caller. The Update State contains
     * a mask of which display channels need to be updated.
     */
    nvUpdateUpdateState(pDevEvo, pUpdateState, pChannel);

    /*
     * Tell HW what ctxdma entry to use to look up actual RG semaphore surface.
     * If ctxdma handle is 0, HW will disable the semaphore.
     */
    nvDmaSetStartEvoMethod(pChannel,
                           NVC67D_HEAD_SET_CONTEXT_DMA_RG_REL_SEMAPHORE(head, semaphoreIndex),
                           1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C67D, _HEAD_SET_CONTEXT_DMA_RG_REL_SEMAPHORE, _HANDLE,
                (pSurfaceDesc == NULL) ? 0 : pSurfaceDesc->ctxDmaHandle));

    if ((pSurfaceDesc == NULL) || (pSurfaceDesc->ctxDmaHandle == 0)) {
        /* Disabling semaphore so no configuration necessary. */
        return;
    }

    /*
     * Configure the semaphore with the following:
     * Set OFFSET to 0 (default).
     * Set PAYLOAD_SIZE to 32bits (default).
     * Set REL_MODE to WRITE (default).
     * Set RUN_MODE to CONTINUOUS.
     * Set RASTER_LINE to start of Vblank: Vsync + Vbp + Vactive.
     *
     * Note that all these options together fit in 32bits, and that all 32 bits
     * must be written each time any given option changes.
     *
     * The actual payload value doesn't currently matter since this RG
     * semaphore will be mapped to a syncpt for now. Each HW-issued payload
     * write is converted to a single syncpt increment irrespective of what the
     * actual semaphore payload value is.
     */
    nvDmaSetStartEvoMethod(pChannel,
                           NVC67D_HEAD_SET_RG_REL_SEMAPHORE_CONTROL(head, semaphoreIndex),
                           1);
    nvDmaSetEvoMethodData(pChannel,
            DRF_NUM(C67D, _HEAD_SET_RG_REL_SEMAPHORE_CONTROL, _OFFSET, 0) |
            DRF_DEF(C67D, _HEAD_SET_RG_REL_SEMAPHORE_CONTROL, _PAYLOAD_SIZE,
                    _PAYLOAD_32BIT) |
            DRF_DEF(C67D, _HEAD_SET_RG_REL_SEMAPHORE_CONTROL, _REL_MODE,
                    _WRITE) |
            DRF_DEF(C67D, _HEAD_SET_RG_REL_SEMAPHORE_CONTROL, _RUN_MODE,
                    _CONTINUOUS) |
            DRF_NUM(C67D, _HEAD_SET_RG_REL_SEMAPHORE_CONTROL, _RASTER_LINE,
                    rasterLine));
}

static void EvoSetHdmiDscParams(const NVDispEvoRec *pDispEvo,
                                   const NvU32 head,
                                   const NVDscInfoEvoRec *pDscInfo,
                                   const enum nvKmsPixelDepth pixelDepth)
{
    NVEvoChannelPtr pChannel = pDispEvo->pDevEvo->core;
    NvU32 bpc, flatnessDetThresh;
    NvU32 i;

    nvAssert(pDispEvo->pDevEvo->hal->caps.supportsHDMIFRL &&
             pDscInfo->type == NV_DSC_INFO_EVO_TYPE_HDMI);

    bpc = nvPixelDepthToBitsPerComponent(pixelDepth);
    if (bpc < 8) {
        nvAssert(bpc >= 8);
        bpc = 8;
    }
    flatnessDetThresh = (2 << (bpc - 8));

    nvDmaSetStartEvoMethod(pChannel, NVC67D_HEAD_SET_DSC_CONTROL(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_DEF(C67D, _HEAD_SET_DSC_CONTROL, _ENABLE, _TRUE) |
        ((pDscInfo->hdmi.dscMode == NV_DSC_EVO_MODE_DUAL) ?
             DRF_DEF(C57D, _HEAD_SET_DSC_CONTROL, _MODE, _DUAL) :
             DRF_DEF(C57D, _HEAD_SET_DSC_CONTROL, _MODE, _SINGLE)) |
        DRF_NUM(C67D, _HEAD_SET_DSC_CONTROL, _FLATNESS_DET_THRESH, flatnessDetThresh) |
        DRF_DEF(C67D, _HEAD_SET_DSC_CONTROL, _FULL_ICH_ERR_PRECISION, _ENABLE) |
        DRF_DEF(C67D, _HEAD_SET_DSC_CONTROL, _AUTO_RESET, _ENABLE) |
        DRF_DEF(C67D, _HEAD_SET_DSC_CONTROL, _FORCE_ICH_RESET, _FALSE));

    nvDmaSetStartEvoMethod(pChannel, NVC67D_HEAD_SET_DSC_PPS_CONTROL(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_DEF(C67D, _HEAD_SET_DSC_PPS_CONTROL, _ENABLE, _TRUE) |
        DRF_DEF(C67D, _HEAD_SET_DSC_PPS_CONTROL, _LOCATION, _VBLANK) |
        DRF_DEF(C67D, _HEAD_SET_DSC_PPS_CONTROL, _FREQUENCY, _EVERY_FRAME) |
        /* MFS says "For FRL DSC CVTEM, it should be 0x21 (136bytes)." */
        DRF_NUM(C67D, _HEAD_SET_DSC_PPS_CONTROL, _SIZE, 0x21));

    /* The loop below assumes the methods are tightly packed. */
    ct_assert(ARRAY_LEN(pDscInfo->hdmi.pps) == 32);
    ct_assert((NVC67D_HEAD_SET_DSC_PPS_DATA1(0) - NVC67D_HEAD_SET_DSC_PPS_DATA0(0)) == 4);
    ct_assert((NVC67D_HEAD_SET_DSC_PPS_DATA31(0) - NVC67D_HEAD_SET_DSC_PPS_DATA0(0)) == (31 * 4));
    for (i = 0; i < ARRAY_LEN(pDscInfo->hdmi.pps); i++) {
        nvDmaSetStartEvoMethod(pChannel, NVC67D_HEAD_SET_DSC_PPS_DATA0(head) + (i * 4), 1);
        nvDmaSetEvoMethodData(pChannel, pDscInfo->hdmi.pps[i]);
    }

    /* Byte 0 must be 0x7f, the rest are don't care (will be filled in by HW) */
    nvDmaSetStartEvoMethod(pChannel, NVC67D_HEAD_SET_DSC_PPS_HEAD(head), 1);
    nvDmaSetEvoMethodData(pChannel,
                          DRF_NUM(C67D, _HEAD_SET_DSC_PPS_HEAD, _BYTE0, 0x7f));

    nvDmaSetStartEvoMethod(pChannel, NVC67D_HEAD_SET_HDMI_DSC_HCACTIVE(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C67D, _HEAD_SET_HDMI_DSC_HCACTIVE, _BYTES, pDscInfo->hdmi.dscHActiveBytes) |
        DRF_NUM(C67D, _HEAD_SET_HDMI_DSC_HCACTIVE, _TRI_BYTES, pDscInfo->hdmi.dscHActiveTriBytes));
    nvDmaSetStartEvoMethod(pChannel, NVC67D_HEAD_SET_HDMI_DSC_HCBLANK(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_NUM(C67D, _HEAD_SET_HDMI_DSC_HCBLANK, _WIDTH, pDscInfo->hdmi.dscHBlankTriBytes));
}

static void EvoSetDpDscParams(const NVDispEvoRec *pDispEvo,
                              const NvU32 head,
                              const NVDscInfoEvoRec *pDscInfo)
{
    NVEvoChannelPtr pChannel = pDispEvo->pDevEvo->core;
    NvU32 flatnessDetThresh;
    NvU32 i;

    nvAssert(pDscInfo->type == NV_DSC_INFO_EVO_TYPE_DP);

    // XXX: I'm pretty sure that this is wrong.
    // BitsPerPixelx16 is something like (24 * 16) = 384, and 2 << (384 - 8) is
    // an insanely large number.
    flatnessDetThresh = (2 << (pDscInfo->dp.bitsPerPixelX16 - 8)); /* ??? */

    nvAssert((pDscInfo->dp.dscMode == NV_DSC_EVO_MODE_DUAL) ||
                (pDscInfo->dp.dscMode == NV_DSC_EVO_MODE_SINGLE));

    nvDmaSetStartEvoMethod(pChannel, NVC57D_HEAD_SET_DSC_CONTROL(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_DEF(C57D, _HEAD_SET_DSC_CONTROL, _ENABLE, _TRUE) |
        ((pDscInfo->dp.dscMode == NV_DSC_EVO_MODE_DUAL) ?
             DRF_DEF(C57D, _HEAD_SET_DSC_CONTROL, _MODE, _DUAL) :
             DRF_DEF(C57D, _HEAD_SET_DSC_CONTROL, _MODE, _SINGLE)) |
        DRF_NUM(C57D, _HEAD_SET_DSC_CONTROL, _FLATNESS_DET_THRESH, flatnessDetThresh) |
        DRF_DEF(C57D, _HEAD_SET_DSC_CONTROL, _FULL_ICH_ERR_PRECISION, _ENABLE) |
        DRF_DEF(C57D, _HEAD_SET_DSC_CONTROL, _AUTO_RESET, _DISABLE) |
        DRF_DEF(C57D, _HEAD_SET_DSC_CONTROL, _FORCE_ICH_RESET, _TRUE));

    nvDmaSetStartEvoMethod(pChannel, NVC57D_HEAD_SET_DSC_PPS_CONTROL(head), 1);
    nvDmaSetEvoMethodData(pChannel,
        DRF_DEF(C57D, _HEAD_SET_DSC_PPS_CONTROL, _ENABLE, _TRUE) |
        DRF_DEF(C57D, _HEAD_SET_DSC_PPS_CONTROL, _LOCATION, _VSYNC) |
        DRF_DEF(C57D, _HEAD_SET_DSC_PPS_CONTROL, _FREQUENCY, _EVERY_FRAME) |
        DRF_NUM(C57D, _HEAD_SET_DSC_PPS_CONTROL, _SIZE, 0x1F /* 32 PPS Dwords - 1 = 31 */));


#define NV_EVO5_NUM_HEAD_SET_DSC_PPS_DATA_DWORDS \
    (((NVC57D_HEAD_SET_DSC_PPS_DATA31(0) - NVC57D_HEAD_SET_DSC_PPS_DATA0(0)) / 4) + 1)

    ct_assert(NV_EVO5_NUM_HEAD_SET_DSC_PPS_DATA_DWORDS <= ARRAY_LEN(pDscInfo->dp.pps));

    for (i = 0; i < NV_EVO5_NUM_HEAD_SET_DSC_PPS_DATA_DWORDS; i++) {
        nvDmaSetStartEvoMethod(pChannel,(NVC57D_HEAD_SET_DSC_PPS_DATA0(head) + (i * 4)), 1);
        nvDmaSetEvoMethodData(pChannel, pDscInfo->dp.pps[i]);
    }

    /*
     * In case of DP, PPS is sent using the SDP over the Main-Link
     * during the vertical blanking interval. The PPS SDP header is defined
     * in DP 1.4 specification under section 2.2.5.9.1.
     */

    nvDmaSetStartEvoMethod(pChannel,
                           NVC57D_HEAD_SET_DSC_PPS_HEAD(head), 1);
    nvDmaSetEvoMethodData(pChannel,
                          DRF_NUM(C57D, _HEAD_SET_DSC_PPS_HEAD, _BYTE0, 0x00) | /* SDP ID = 0x0 */
                          DRF_NUM(C57D, _HEAD_SET_DSC_PPS_HEAD, _BYTE1, 0x10) | /* SDP Type = 0x10 */
                          DRF_NUM(C57D, _HEAD_SET_DSC_PPS_HEAD, _BYTE2, 0x7f) | /* Number of payload data bytes - 1 = 0x7F */
                          DRF_NUM(C57D, _HEAD_SET_DSC_PPS_HEAD, _BYTE3, 0x00)); /* Reserved */
}

static void EvoSetDscParamsC5(const NVDispEvoRec *pDispEvo,
                              const NvU32 head,
                              const NVDscInfoEvoRec *pDscInfo,
                              const enum nvKmsPixelDepth pixelDepth)
{
    if (pDscInfo->type == NV_DSC_INFO_EVO_TYPE_HDMI) {
        EvoSetHdmiDscParams(pDispEvo, head, pDscInfo, pixelDepth);
    } else if (pDscInfo->type == NV_DSC_INFO_EVO_TYPE_DP) {
        EvoSetDpDscParams(pDispEvo, head, pDscInfo);
    } else {
        NVEvoChannelPtr pChannel = pDispEvo->pDevEvo->core;

        nvAssert(pDscInfo->type == NV_DSC_INFO_EVO_TYPE_DISABLED);

        /* Disable DSC function */
        nvDmaSetStartEvoMethod(pChannel, NVC57D_HEAD_SET_DSC_CONTROL(head), 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_DEF(C57D, _HEAD_SET_DSC_CONTROL, _ENABLE, _FALSE));

        /* Disable PPS SDP (Secondary-Data Packet), DP won't send out PPS SDP */
        nvDmaSetStartEvoMethod(pChannel, NVC57D_HEAD_SET_DSC_PPS_CONTROL(head), 1);
        nvDmaSetEvoMethodData(pChannel,
            DRF_DEF(C57D, _HEAD_SET_DSC_PPS_CONTROL, _ENABLE, _FALSE));
    }
}

static void
EvoEnableMidFrameAndDWCFWatermarkC5(NVDevEvoPtr pDevEvo,
                                    NvU32 sd,
                                    NvU32 head,
                                    NvBool enable,
                                    NVEvoUpdateState *pUpdateState)
{
    NVEvoChannelPtr pChannel = pDevEvo->core;

    if (enable) {
        pDevEvo->gpus[sd].setSwSpareA[head] =
            FLD_SET_DRF(C37D,
                        _HEAD_SET_SW_SPARE_A,
                        _DISABLE_MID_FRAME_AND_DWCF_WATERMARK,
                        _FALSE,
                        pDevEvo->gpus[sd].setSwSpareA[head]);
    } else {
        pDevEvo->gpus[sd].setSwSpareA[head] =
            FLD_SET_DRF(C37D,
                        _HEAD_SET_SW_SPARE_A,
                        _DISABLE_MID_FRAME_AND_DWCF_WATERMARK,
                        _TRUE,
                        pDevEvo->gpus[sd].setSwSpareA[head]);
    }

    nvPushEvoSubDevMask(pDevEvo, NVBIT(sd));

    nvUpdateUpdateState(pDevEvo, pUpdateState, pChannel);

    nvDmaSetStartEvoMethod(pChannel, NVC37D_HEAD_SET_SW_SPARE_A(head), 1);
    nvDmaSetEvoMethodData(pChannel, pDevEvo->gpus[sd].setSwSpareA[head]);

    nvPopEvoSubDevMask(pDevEvo);
}

NvU32 nvEvoGetActiveViewportOffsetC3(NVDispEvoRec *pDispEvo, NvU32 head)
{
    NVC372_CTRL_CMD_GET_ACTIVE_VIEWPORT_POINT_IN_PARAMS params = { };
    NvU32 ret;
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;

    params.base.subdeviceIndex = pDispEvo->displayOwner;
    params.windowIndex = head << 1;

    ret = nvRmApiControl(nvEvoGlobal.clientHandle,
                         pDevEvo->rmCtrlHandle,
                         NVC372_CTRL_CMD_GET_ACTIVE_VIEWPORT_POINT_IN,
                         &params, sizeof(params));

    if (ret != NVOS_STATUS_SUCCESS) {
        nvEvoLogDevDebug(pDevEvo, EVO_LOG_ERROR,
                         "Failed to query active viewport offset");
    }

    return params.activeViewportPointIn.y;
}

static NvBool EvoComputeWindowScalingTapsC3(const NVDevEvoRec *pDevEvo,
                                            const NVEvoChannel *pChannel,
                                            NVFlipChannelEvoHwState *pHwState)
{
    NvU32 win = NV_EVO_CHANNEL_MASK_WINDOW_NUMBER(pChannel->channelMask);
    const NVEvoScalerCaps *pScalerCaps =
        &pDevEvo->gpus[0].capabilities.window[win].scalerCaps;

    if (!nvAssignScalerTaps(pDevEvo,
                            pScalerCaps,
                            pHwState->sizeIn.width, pHwState->sizeIn.height,
                            pHwState->sizeOut.width, pHwState->sizeOut.height,
                            FALSE /* doubleScan */,
                            &pHwState->hTaps, &pHwState->vTaps)) {
        return FALSE;
    }

    return TRUE;
}

NvBool nvEvoComputeWindowScalingTapsC5(const NVDevEvoRec *pDevEvo,
                                            const NVEvoChannel *pChannel,
                                            NVFlipChannelEvoHwState *pHwState)
{
    if (!EvoComputeWindowScalingTapsC3(pDevEvo, pChannel, pHwState)) {
        return FALSE;
    }

    /*
     * If scaling is enabled, CSC11 will be used by NVKMS to convert from
     * linear FP16 LMS to linear FP16 RGB. As such, the user-supplied precomp
     * CSC can't be programmed into CSC11 in this case.
     */
    if ((pHwState->sizeIn.width != pHwState->sizeOut.width) ||
        (pHwState->sizeIn.height != pHwState->sizeOut.height)) {
        if (!nvIsCscMatrixIdentity(&pHwState->cscMatrix)) {
            return FALSE;
        }
    }

    return TRUE;
}

static void EvoSetMergeModeC5(const NVDispEvoRec *pDispEvo,
                              const NvU32 head,
                              const NVEvoMergeMode mode,
                              NVEvoUpdateState* pUpdateState)
{
    NVDevEvoRec *pDevEvo = pDispEvo->pDevEvo;
    NVEvoChannelPtr pChannel = pDevEvo->core;
    NvU32 data = 0x0;

    nvPushEvoSubDevMask(pDevEvo, NVBIT(pDispEvo->displayOwner));

    nvUpdateUpdateState(pDevEvo, pUpdateState, pChannel);

    switch (mode) {
        case NV_EVO_MERGE_MODE_DISABLED:
            data = DRF_DEF(C57D, _HEAD_SET_RG_MERGE, _MODE, _DISABLE);
            break;
        case NV_EVO_MERGE_MODE_SETUP:
            data = DRF_DEF(C57D, _HEAD_SET_RG_MERGE, _MODE, _SETUP);
            break;
        case NV_EVO_MERGE_MODE_PRIMARY:
            data = DRF_DEF(C57D, _HEAD_SET_RG_MERGE, _MODE, _MASTER);
            break;
        case NV_EVO_MERGE_MODE_SECONDARY:
            data = DRF_DEF(C57D, _HEAD_SET_RG_MERGE, _MODE, _SLAVE);
            break;
    }

    nvDmaSetStartEvoMethod(pChannel, NVC57D_HEAD_SET_RG_MERGE(head), 1);
    nvDmaSetEvoMethodData(pChannel, data);

    nvPopEvoSubDevMask(pDevEvo);
}

static NvU32 EvoAllocSurfaceDescriptorC3(
    NVDevEvoPtr pDevEvo, NVSurfaceDescriptor *pSurfaceDesc,
    NvU32 memoryHandle, NvU32 localCtxDmaFlags,
    NvU64 limit)
{
    return nvCtxDmaAlloc(pDevEvo, &pSurfaceDesc->ctxDmaHandle,
                         memoryHandle,
                         localCtxDmaFlags, limit);
}

static void EvoFreeSurfaceDescriptorC3(
    NVDevEvoPtr pDevEvo,
    NvU32 deviceHandle,
    NVSurfaceDescriptor *pSurfaceDesc)
{
    nvCtxDmaFree(pDevEvo, deviceHandle, &pSurfaceDesc->ctxDmaHandle);
}

static NvU32 EvoBindSurfaceDescriptorC3(
    NVDevEvoPtr pDevEvo,
    NVEvoChannelPtr pChannel,
    NVSurfaceDescriptor *pSurfaceDesc)
{
    return nvCtxDmaBind(pDevEvo, pChannel, pSurfaceDesc->ctxDmaHandle);
}

NVEvoHAL nvEvoC3 = {
    EvoSetRasterParamsC3,                         /* SetRasterParams */
    EvoSetProcAmpC3,                              /* SetProcAmp */
    EvoSetHeadControlC3,                          /* SetHeadControl */
    EvoSetHeadRefClkC3,                           /* SetHeadRefClk */
    EvoHeadSetControlORC3,                        /* HeadSetControlOR */
    nvEvoORSetControlC3,                          /* ORSetControl */
    EvoHeadSetDisplayIdC3,                        /* HeadSetDisplayId */
    EvoSetUsageBoundsC3,                          /* SetUsageBounds */
    nvEvoUpdateC3,                                /* Update */
    nvEvoIsModePossibleC3,                        /* IsModePossible */
    nvEvoPrePostIMPC3,                            /* PrePostIMP */
    nvEvoSetNotifierC3,                           /* SetNotifier */
    EvoGetCapabilitiesC3,                         /* GetCapabilities */
    EvoFlipC3,                                    /* Flip */
    EvoFlipTransitionWARC3,                       /* FlipTransitionWAR */
    EvoFillLUTSurfaceC3,                          /* FillLUTSurface */
    EvoSetOutputLutC3,                            /* SetOutputLut */
    EvoSetOutputScalerC3,                         /* SetOutputScaler */
    EvoSetViewportPointInC3,                      /* SetViewportPointIn */
    EvoSetViewportInOutC3,                        /* SetViewportInOut */
    EvoSetCursorImageC3,                          /* SetCursorImage */
    nvEvoValidateCursorSurfaceC3,                 /* ValidateCursorSurface */
    EvoValidateWindowFormatC3,                    /* ValidateWindowFormat */
    nvEvoInitCompNotifierC3,                      /* InitCompNotifier */
    nvEvoIsCompNotifierCompleteC3,                /* IsCompNotifierComplete */
    nvEvoWaitForCompNotifierC3,                   /* WaitForCompNotifier */
    EvoSetDitherC3,                               /* SetDither */
    EvoSetStallLockC3,                            /* SetStallLock */
    EvoSetDisplayRateC3,                          /* SetDisplayRate */
    EvoInitChannelC3,                             /* InitChannel */
    NULL,                                         /* InitDefaultLut */
    EvoInitWindowMappingC3,                       /* InitWindowMapping */
    nvEvoIsChannelIdleC3,                         /* IsChannelIdle */
    nvEvoIsChannelMethodPendingC3,                /* IsChannelMethodPending */
    nvEvoForceIdleSatelliteChannelC3,             /* ForceIdleSatelliteChannel */
    nvEvoForceIdleSatelliteChannelIgnoreLockC3,   /* ForceIdleSatelliteChannelIgnoreLock */
    nvEvoAccelerateChannelC3,                     /* AccelerateChannel */
    nvEvoResetChannelAcceleratorsC3,              /* ResetChannelAccelerators */
    nvEvoAllocRmCtrlObjectC3,                     /* AllocRmCtrlObject */
    nvEvoFreeRmCtrlObjectC3,                      /* FreeRmCtrlObject */
    nvEvoSetImmPointOutC3,                        /* SetImmPointOut */
    EvoStartHeadCRC32CaptureC3,                   /* StartCRC32Capture */
    EvoStopHeadCRC32CaptureC3,                    /* StopCRC32Capture */
    nvEvoQueryHeadCRC32_C3,                       /* QueryCRC32 */
    nvEvoGetScanLineC3,                           /* GetScanLine */
    NULL,                                         /* ConfigureVblankSyncObject */
    nvEvo1SetDscParams,                           /* SetDscParams */
    NULL,                                         /* EnableMidFrameAndDWCFWatermark */
    nvEvoGetActiveViewportOffsetC3,               /* GetActiveViewportOffset */
    NULL,                                         /* ClearSurfaceUsage */
    EvoComputeWindowScalingTapsC3,                /* ComputeWindowScalingTaps */
    nvEvoGetWindowScalingCapsC3,                  /* GetWindowScalingCaps */
    NULL,                                         /* SetMergeMode */
    nvEvo1SendHdmiInfoFrame,                      /* SendHdmiInfoFrame */
    nvEvo1DisableHdmiInfoFrame,                   /* DisableHdmiInfoFrame */
    nvEvo1SendDpInfoFrameSdp,                     /* SendDpInfoFrameSdp */
    NULL,                                         /* SetDpVscSdp */
    NULL,                                         /* InitHwHeadMultiTileConfig */
    NULL,                                         /* SetMultiTileConfig */
    EvoAllocSurfaceDescriptorC3,                  /* AllocSurfaceDescriptor */
    EvoFreeSurfaceDescriptorC3,                   /* FreeSurfaceDescriptor */
    EvoBindSurfaceDescriptorC3,                   /* BindSurfaceDescriptor */
    NULL,                                         /* SetTmoLutSurfaceAddress */
    NULL,                                         /* SetILUTSurfaceAddress */
    EvoSetISOSurfaceAddressC3,                    /* SetISOSurfaceAddress */
    EvoSetCoreNotifierSurfaceAddressAndControlC3, /* SetCoreNotifierSurfaceAddressAndControl */
    EvoSetWinNotifierSurfaceAddressAndControlC3,  /* SetWinNotifierSurfaceAddressAndControl */
    NULL,                                         /* SetSemaphoreSurfaceAddressAndControl */
    NULL,                                         /* SetAcqSemaphoreSurfaceAddressAndControl */
    {                                             /* caps */
        TRUE,                                     /* supportsNonInterlockedUsageBoundsUpdate */
        TRUE,                                     /* supportsDisplayRate */
        FALSE,                                    /* supportsFlipLockRGStatus */
        FALSE,                                    /* needDefaultLutSurface */
        FALSE,                                    /* hasUnorm10OLUT */
        FALSE,                                    /* supportsImageSharpening */
        FALSE,                                    /* supportsHDMIVRR */
        FALSE,                                    /* supportsCoreChannelSurface */
        FALSE,                                    /* supportsHDMIFRL */
        TRUE,                                     /* supportsSetStorageMemoryLayout */
        FALSE,                                    /* supportsIndependentAcqRelSemaphore */
        FALSE,                                    /* supportsCoreLut */
        TRUE,                                     /* supportsSynchronizedOverlayPositionUpdate */
        FALSE,                                    /* supportsVblankSyncObjects */
        FALSE,                                    /* requiresScalingTapsInBothDimensions */
        FALSE,                                    /* supportsMergeMode */
        FALSE,                                    /* supportsHDMI10BPC */
        FALSE,                                    /* supportsDPAudio192KHz */
        FALSE,                                    /* supportsInputColorSpace */
        FALSE,                                    /* supportsInputColorRange */
        NV_EVO3_SUPPORTED_DITHERING_MODES,        /* supportedDitheringModes */
        sizeof(NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS), /* impStructSize */
        NV_EVO_SCALER_2TAPS,                      /* minScalerTaps */
        NV_EVO3_X_EMULATED_SURFACE_MEMORY_FORMATS_C3, /* xEmulatedSurfaceMemoryFormats */
    },
};

NVEvoHAL nvEvoC5 = {
    EvoSetRasterParamsC5,                         /* SetRasterParams */
    EvoSetProcAmpC5,                              /* SetProcAmp */
    EvoSetHeadControlC3,                          /* SetHeadControl */
    EvoSetHeadRefClkC3,                           /* SetHeadRefClk */
    EvoHeadSetControlORC5,                        /* HeadSetControlOR */
    nvEvoORSetControlC3,                          /* ORSetControl */
    EvoHeadSetDisplayIdC3,                        /* HeadSetDisplayId */
    nvEvoSetUsageBoundsC5,                        /* SetUsageBounds */
    nvEvoUpdateC3,                                /* Update */
    nvEvoIsModePossibleC3,                        /* IsModePossible */
    nvEvoPrePostIMPC3,                            /* PrePostIMP */
    nvEvoSetNotifierC3,                           /* SetNotifier */
    EvoGetCapabilitiesC5,                         /* GetCapabilities */
    EvoFlipC5,                                    /* Flip */
    EvoFlipTransitionWARC5,                       /* FlipTransitionWAR */
    nvEvoFillLUTSurfaceC5,                        /* FillLUTSurface */
    EvoSetOutputLutC5,                            /* SetOutputLut */
    EvoSetOutputScalerC3,                         /* SetOutputScaler */
    EvoSetViewportPointInC3,                      /* SetViewportPointIn */
    EvoSetViewportInOutC5,                        /* SetViewportInOut */
    EvoSetCursorImageC3,                          /* SetCursorImage */
    nvEvoValidateCursorSurfaceC3,                 /* ValidateCursorSurface */
    EvoValidateWindowFormatC5,                    /* ValidateWindowFormat */
    nvEvoInitCompNotifierC3,                      /* InitCompNotifier */
    nvEvoIsCompNotifierCompleteC3,                /* IsCompNotifierComplete */
    nvEvoWaitForCompNotifierC3,                   /* WaitForCompNotifier */
    EvoSetDitherC3,                               /* SetDither */
    EvoSetStallLockC3,                            /* SetStallLock */
    EvoSetDisplayRateC3,                          /* SetDisplayRate */
    EvoInitChannelC5,                             /* InitChannel */
    nvEvoInitDefaultLutC5,                        /* InitDefaultLut */
    nvEvoInitWindowMappingC5,                     /* InitWindowMapping */
    nvEvoIsChannelIdleC3,                         /* IsChannelIdle */
    nvEvoIsChannelMethodPendingC3,                /* IsChannelMethodPending */
    nvEvoForceIdleSatelliteChannelC3,             /* ForceIdleSatelliteChannel */
    nvEvoForceIdleSatelliteChannelIgnoreLockC3,   /* ForceIdleSatelliteChannelIgnoreLock */
    nvEvoAccelerateChannelC3,                     /* AccelerateChannel */
    nvEvoResetChannelAcceleratorsC3,              /* ResetChannelAccelerators */
    nvEvoAllocRmCtrlObjectC3,                     /* AllocRmCtrlObject */
    nvEvoFreeRmCtrlObjectC3,                      /* FreeRmCtrlObject */
    nvEvoSetImmPointOutC3,                        /* SetImmPointOut */
    EvoStartHeadCRC32CaptureC3,                   /* StartCRC32Capture */
    EvoStopHeadCRC32CaptureC3,                    /* StopCRC32Capture */
    nvEvoQueryHeadCRC32_C3,                       /* QueryCRC32 */
    nvEvoGetScanLineC3,                           /* GetScanLine */
    NULL,                                         /* ConfigureVblankSyncObject */
    EvoSetDscParamsC5,                            /* SetDscParams */
    EvoEnableMidFrameAndDWCFWatermarkC5,          /* EnableMidFrameAndDWCFWatermark */
    nvEvoGetActiveViewportOffsetC3,               /* GetActiveViewportOffset */
    NULL,                                         /* ClearSurfaceUsage */
    nvEvoComputeWindowScalingTapsC5,              /* ComputeWindowScalingTaps */
    nvEvoGetWindowScalingCapsC3,                  /* GetWindowScalingCaps */
    EvoSetMergeModeC5,                            /* SetMergeMode */
    nvEvo1SendHdmiInfoFrame,                      /* SendHdmiInfoFrame */
    nvEvo1DisableHdmiInfoFrame,                   /* DisableHdmiInfoFrame */
    nvEvo1SendDpInfoFrameSdp,                     /* SendDpInfoFrameSdp */
    NULL,                                         /* SetDpVscSdp */
    NULL,                                         /* InitHwHeadMultiTileConfig */
    NULL,                                         /* SetMultiTileConfig */
    EvoAllocSurfaceDescriptorC3,                  /* AllocSurfaceDescriptor */
    EvoFreeSurfaceDescriptorC3,                   /* FreeSurfaceDescriptor */
    EvoBindSurfaceDescriptorC3,                   /* BindSurfaceDescriptor */
    EvoSetTmoLutSurfaceAddressC5,                 /* SetTmoLutSurfaceAddress */
    EvoSetILUTSurfaceAddressC5,                   /* SetILUTSurfaceAddress */
    EvoSetISOSurfaceAddressC3,                    /* SetISOSurfaceAddress */
    EvoSetCoreNotifierSurfaceAddressAndControlC3, /* SetCoreNotifierSurfaceAddressAndControl */
    EvoSetWinNotifierSurfaceAddressAndControlC3,  /* SetWinNotifierSurfaceAddressAndControl */
    NULL,                                         /* SetSemaphoreSurfaceAddressAndControl */
    NULL,                                         /* SetAcqSemaphoreSurfaceAddressAndControl */
    {                                             /* caps */
        TRUE,                                     /* supportsNonInterlockedUsageBoundsUpdate */
        TRUE,                                     /* supportsDisplayRate */
        FALSE,                                    /* supportsFlipLockRGStatus */
        TRUE,                                     /* needDefaultLutSurface */
        TRUE,                                     /* hasUnorm10OLUT */
        FALSE,                                    /* supportsImageSharpening */
        TRUE,                                     /* supportsHDMIVRR */
        FALSE,                                    /* supportsCoreChannelSurface */
        FALSE,                                    /* supportsHDMIFRL */
        TRUE,                                     /* supportsSetStorageMemoryLayout */
        FALSE,                                    /* supportsIndependentAcqRelSemaphore */
        FALSE,                                    /* supportsCoreLut */
        TRUE,                                     /* supportsSynchronizedOverlayPositionUpdate */
        FALSE,                                    /* supportsVblankSyncObjects */
        FALSE,                                    /* requiresScalingTapsInBothDimensions */
        TRUE,                                     /* supportsMergeMode */
        FALSE,                                    /* supportsHDMI10BPC */
        FALSE,                                    /* supportsDPAudio192KHz */
        TRUE,                                     /* supportsInputColorSpace */
        TRUE,                                     /* supportsInputColorRange */
        NV_EVO3_SUPPORTED_DITHERING_MODES,        /* supportedDitheringModes */
        sizeof(NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS), /* impStructSize */
        NV_EVO_SCALER_2TAPS,                      /* minScalerTaps */
        NV_EVO3_X_EMULATED_SURFACE_MEMORY_FORMATS_C5, /* xEmulatedSurfaceMemoryFormats */
    },
};

NVEvoHAL nvEvoC6 = {
    EvoSetRasterParamsC6,                         /* SetRasterParams */
    EvoSetProcAmpC5,                              /* SetProcAmp */
    EvoSetHeadControlC3,                          /* SetHeadControl */
    EvoSetHeadRefClkC3,                           /* SetHeadRefClk */
    EvoHeadSetControlORC5,                        /* HeadSetControlOR */
    EvoORSetControlC6,                            /* ORSetControl */
    EvoHeadSetDisplayIdC3,                        /* HeadSetDisplayId */
    nvEvoSetUsageBoundsC5,                        /* SetUsageBounds */
    nvEvoUpdateC3,                                /* Update */
    nvEvoIsModePossibleC3,                        /* IsModePossible */
    nvEvoPrePostIMPC3,                            /* PrePostIMP */
    nvEvoSetNotifierC3,                           /* SetNotifier */
    nvEvoGetCapabilitiesC6,                       /* GetCapabilities */
    nvEvoFlipC6,                                  /* Flip */
    nvEvoFlipTransitionWARC6,                     /* FlipTransitionWAR */
    nvEvoFillLUTSurfaceC5,                        /* FillLUTSurface */
    EvoSetOutputLutC5,                            /* SetOutputLut */
    EvoSetOutputScalerC3,                         /* SetOutputScaler */
    EvoSetViewportPointInC3,                      /* SetViewportPointIn */
    EvoSetViewportInOutC5,                        /* SetViewportInOut */
    EvoSetCursorImageC3,                          /* SetCursorImage */
    nvEvoValidateCursorSurfaceC3,                 /* ValidateCursorSurface */
    nvEvoValidateWindowFormatC6,                  /* ValidateWindowFormat */
    nvEvoInitCompNotifierC3,                      /* InitCompNotifier */
    nvEvoIsCompNotifierCompleteC3,                /* IsCompNotifierComplete */
    nvEvoWaitForCompNotifierC3,                   /* WaitForCompNotifier */
    EvoSetDitherC3,                               /* SetDither */
    EvoSetStallLockC3,                            /* SetStallLock */
    EvoSetDisplayRateC3,                          /* SetDisplayRate */
    EvoInitChannelC5,                             /* InitChannel */
    nvEvoInitDefaultLutC5,                        /* InitDefaultLut */
    nvEvoInitWindowMappingC5,                     /* InitWindowMapping */
    nvEvoIsChannelIdleC3,                         /* IsChannelIdle */
    nvEvoIsChannelMethodPendingC3,                /* IsChannelMethodPending */
    nvEvoForceIdleSatelliteChannelC3,             /* ForceIdleSatelliteChannel */
    nvEvoForceIdleSatelliteChannelIgnoreLockC3,   /* ForceIdleSatelliteChannelIgnoreLock */
    nvEvoAccelerateChannelC3,                     /* AccelerateChannel */
    nvEvoResetChannelAcceleratorsC3,              /* ResetChannelAccelerators */
    nvEvoAllocRmCtrlObjectC3,                     /* AllocRmCtrlObject */
    nvEvoFreeRmCtrlObjectC3,                      /* FreeRmCtrlObject */
    nvEvoSetImmPointOutC3,                        /* SetImmPointOut */
    EvoStartHeadCRC32CaptureC3,                   /* StartCRC32Capture */
    EvoStopHeadCRC32CaptureC3,                    /* StopCRC32Capture */
    nvEvoQueryHeadCRC32_C3,                       /* QueryCRC32 */
    nvEvoGetScanLineC3,                           /* GetScanLine */
    EvoConfigureVblankSyncObjectC6,               /* ConfigureVblankSyncObject */
    EvoSetDscParamsC5,                            /* SetDscParams */
    NULL,                                         /* EnableMidFrameAndDWCFWatermark */
    nvEvoGetActiveViewportOffsetC3,               /* GetActiveViewportOffset */
    NULL,                                         /* ClearSurfaceUsage */
    nvEvoComputeWindowScalingTapsC5,              /* ComputeWindowScalingTaps */
    nvEvoGetWindowScalingCapsC3,                  /* GetWindowScalingCaps */
    EvoSetMergeModeC5,                            /* SetMergeMode */
    nvEvo1SendHdmiInfoFrame,                      /* SendHdmiInfoFrame */
    nvEvo1DisableHdmiInfoFrame,                   /* DisableHdmiInfoFrame */
    nvEvo1SendDpInfoFrameSdp,                     /* SendDpInfoFrameSdp */
    NULL,                                         /* SetDpVscSdp */
    NULL,                                         /* InitHwHeadMultiTileConfig */
    NULL,                                         /* SetMultiTileConfig */
    EvoAllocSurfaceDescriptorC3,                  /* AllocSurfaceDescriptor */
    EvoFreeSurfaceDescriptorC3,                   /* FreeSurfaceDescriptor */
    EvoBindSurfaceDescriptorC3,                   /* BindSurfaceDescriptor */
    EvoSetTmoLutSurfaceAddressC5,                 /* SetTmoLutSurfaceAddress */
    EvoSetILUTSurfaceAddressC5,                   /* SetILUTSurfaceAddress */
    EvoSetISOSurfaceAddressC3,                    /* SetISOSurfaceAddress */
    EvoSetCoreNotifierSurfaceAddressAndControlC3, /* SetCoreNotifierSurfaceAddressAndControl */
    EvoSetWinNotifierSurfaceAddressAndControlC3,  /* SetWinNotifierSurfaceAddressAndControl */
    EvoSetSemaphoreSurfaceAddressAndControlC6,    /* SetSemaphoreSurfaceAddressAndControl */
    EvoSetAcqSemaphoreSurfaceAddressAndControlC6, /* SetAcqSemaphoreSurfaceAddressAndControl */
    {                                             /* caps */
        TRUE,                                     /* supportsNonInterlockedUsageBoundsUpdate */
        TRUE,                                     /* supportsDisplayRate */
        FALSE,                                    /* supportsFlipLockRGStatus */
        TRUE,                                     /* needDefaultLutSurface */
        TRUE,                                     /* hasUnorm10OLUT */
        FALSE,                                    /* supportsImageSharpening */
        TRUE,                                     /* supportsHDMIVRR */
        FALSE,                                    /* supportsCoreChannelSurface */
        TRUE,                                     /* supportsHDMIFRL */
        FALSE,                                    /* supportsSetStorageMemoryLayout */
        TRUE,                                     /* supportsIndependentAcqRelSemaphore */
        FALSE,                                    /* supportsCoreLut */
        TRUE,                                     /* supportsSynchronizedOverlayPositionUpdate */
        TRUE,                                     /* supportsVblankSyncObjects */
        FALSE,                                    /* requiresScalingTapsInBothDimensions */
        TRUE,                                     /* supportsMergeMode */
        TRUE,                                     /* supportsHDMI10BPC */
        FALSE,                                    /* supportsDPAudio192KHz */
        TRUE,                                     /* supportsInputColorSpace */
        TRUE,                                     /* supportsInputColorRange */
        NV_EVO3_SUPPORTED_DITHERING_MODES,        /* supportedDitheringModes */
        sizeof(NVC372_CTRL_IS_MODE_POSSIBLE_PARAMS), /* impStructSize */
        NV_EVO_SCALER_2TAPS,                      /* minScalerTaps */
        NV_EVO3_X_EMULATED_SURFACE_MEMORY_FORMATS_C6, /* xEmulatedSurfaceMemoryFormats */
    },
};
