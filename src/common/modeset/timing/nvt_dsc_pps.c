/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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


//=============================================================================
//
//  Provide function to calculate PPS(Picture Parameter Set)
//
//
//==============================================================================

/* ------------------------ Includes --------------------------------------- */
#include "nvt_dsc_pps.h"
#include "nvmisc.h"
#include "displayport/displayport.h"
#include "displayport/displayport2x.h"
#include "nvctassert.h"
#include <stddef.h>

/* ------------------------ Macros ----------------------------------------- */

#define MIN_CHECK(s,a,b)     { if((a)<(b)) { return (NVT_STATUS_ERR);} }
#define RANGE_CHECK(s,a,b,c) { if((((NvS32)(a))<(NvS32)(b))||(((NvS32)(a))>(NvS32)(c))) { return (NVT_STATUS_ERR);} }
#define ENUM2_CHECK(s,a,b,c) { if(((a)!=(b))&&((a)!=(c))) { return (NVT_STATUS_ERR);} }
#define ENUM3_CHECK(s,a,b,c,d) { if(((a)!=(b))&&((a)!=(c))&&((a)!=(d))) { return (NVT_STATUS_ERR);} }
#define MAX(a,b)    (((a)>=(b) || (b == 0xffffffff))?(a):(b))
#define MIN(a,b)    ((a)>=(b)?(b):(a))
#define CLAMP(a,b,c) ((a)<=(b)?(b):((a)>(c)?(c):(a)))
#define ADJUST_SLICE_NUM(n)  ((n)>4?8:((n)>2?4:(n)))
#define MSB(a) (((a)>>8)&0xFF)
#define LSB(a) ((a)&0xFF)

#define NUM_BUF_RANGES 15
#define BPP_UNIT 16
#define OFFSET_FRACTIONAL_BITS  11
#define PIXELS_PER_GROUP 3

//The max pclk frequency(in Mhz) per slice
//DP1.4 spec defines the number of slices needed per display line,
//based on the pixel rate. it's about 340Mhz per slice.
#define MAX_PCLK_PER_SLICE_KHZ       340000
//The max slice_width used in slice_width calculation
//this is not HW limitation(which is 5120 per head), just a recommendation
#define MAX_WIDTH_PER_SLICE          5120
//RC algorithm will get better performance if slice size is bigger.
//This requires slice size be much greater than rc_model_size(8K bits)
//but bigger slice will increase the error rate of DSC slices.
//256KB is a moderate value (about 1280x200 @8bpp)
#define MIN_SLICE_SIZE          (256*1024)
// Per DP 1.4 spec, sink should support slice width of up to at least 2560 (it is allowed to support more).
#define SINK_MAX_SLICE_WIDTH_DEFAULT 2560
// Min bits per pixel supported
#define MIN_BITS_PER_PIXEL 8
// Max bits per pixel supported
#define MAX_BITS_PER_PIXEL 32
// Max HBlank pixel count
#define MAX_HBLANK_PIXELS 7680
#define MHZ_TO_HZ 1000000

/* ------------------------ Datatypes -------------------------------------- */

//input parameters to the pps calculation
typedef struct
{
    NvU32  dsc_version_minor;     // DSC minor version (1-DSC1.1, 2-DSC 1.2)
    NvU32  bits_per_component;    // bits per component of input pixels (8,10,12)
    NvU32  linebuf_depth;         // bits per component of reconstructed line buffer (8 ~ 13)
    NvU32  block_pred_enable;     // block prediction enable (0, 1)
    NvU32  convert_rgb;           // input pixel format (0 YCbCr, 1 RGB)
    NvU32  bits_per_pixel;        // bits per pixel*BPP_UNIT (8.0*BPP_UNIT ~ 32.0*BPP_UNIT)
    NvU32  pic_height;            // picture height (8 ~ 8192)
    NvU32  pic_width;             // picture width  (single mode: 32 ~ 5120, dual mode: 64 ~ 8192)
    NvU32  slice_height;          // 0 - auto,   others (8 ~ 8192)  - must be (pic_height % slice_height == 0)
    NvU32  slice_width;           // maximum slice_width, 0-- default: 1280.
    NvU32  slice_num;             // 0 - auto,   others: 1,2,4,8
    NvU32  slice_count_mask;      // no of slices supported by sink
    NvU32  max_slice_num;         // slice number cap determined from GPU and sink caps
    NvU32  max_slice_width;       // slice width cap determined from GPU and sink caps
    NvU32  pixel_clkMHz;          // pixel clock frequency in MHz, used for slice_width calculation.
    NvU32  dual_mode;             // 0 - single mode, 1 - dual mode, only for checking pic_width
    NvU32  simple_422;            // 4:2:2 simple mode
    NvU32  native_420;            // 420 native mode
    NvU32  native_422;            // 422 native mode
    NvU32  drop_mode;             // 0 - normal mode, 1 - drop mode.
    NvU32  multi_tile;            // 1 = Multi-tile architecture, 0 = dsc single or dual mode without multi-tile
    NvU32  peak_throughput_mode0; // peak throughput supported by the sink for 444 and simple 422 modes. 
    NvU32  peak_throughput_mode1; // peak throughput supported by the sink for native 422 and 420 modes.
    NvU32  eDP;                   // 1 = connector type is eDP, 0 otherwise. 
} DSC_INPUT_PARAMS;

//output pps parameters after calculation
typedef struct
{
    NvU32  dsc_version_major;                // DSC major version, always 1
    NvU32  dsc_version_minor;                // DSC minor version
    NvU32  pps_identifier;                   // Application-specific identifier, always 0
    NvU32  bits_per_component;               // bits per component for input pixels
    NvU32  linebuf_depth;                    // line buffer bit depth
    NvU32  block_pred_enable;                // enable/disable block prediction
    NvU32  convert_rgb;                      // color space for input pixels
    NvU32  simple_422;                       // 4:2:2 simple mode
    NvU32  vbr_enable;                       // enable VBR mode
    NvU32  bits_per_pixel;                   // (bits per pixel * BPP_UNIT) after compression
    NvU32  pic_height;                       // picture height
    NvU32  pic_width;                        // picture width
    NvU32  slice_height;                     // slice height
    NvU32  slice_width;                      // slice width
    NvU32  chunk_size;                       // the size in bytes of the slice chunks
    NvU32  initial_xmit_delay;               // initial transmission delay
    NvU32  initial_dec_delay;                // initial decoding delay
    NvU32  initial_scale_value;              // initial xcXformScale factor value
    NvU32  scale_increment_interval;         // number of group times between incrementing the rcXformScale factor
    NvU32  scale_decrement_interval;         // number of group times between decrementing the rcXformScale factor
    NvU32  first_line_bpg_offset;            // number of additional bits allocated for each group on the first line in a slice
    NvU32  nfl_bpg_offset;                   // number of bits de-allocated for each group after the first line in a slice
    NvU32  slice_bpg_offset;                 // number of bits de-allocated for each group to enforce the slice constrain
    NvU32  initial_offset;                   // initial value for rcXformOffset
    NvU32  final_offset;                     // maximum end-of-slice value for rcXformOffset
    NvU32  flatness_min_qp;                  // minimum flatness QP
    NvU32  flatness_max_qp;                  // maximum flatness QP
    //rc_parameter_set
    NvU32  rc_model_size;                    // number of bits within the "RC model"
    NvU32  rc_edge_factor;                   // edge detection factor
    NvU32  rc_quant_incr_limit0;             // QP threshold for short-term RC
    NvU32  rc_quant_incr_limit1;             // QP threshold for short-term RC
    NvU32  rc_tgt_offset_hi;                 // upper end of the target bpg range for short-term RC
    NvU32  rc_tgt_offset_lo;                 // lower end of the target bpg range for short-term RC
    NvU32  rc_buf_thresh[NUM_BUF_RANGES-1];  // thresholds in "RC model"
    //rc_range_parameters
    NvU32  range_min_qp[NUM_BUF_RANGES];     // minimum QP for each of the RC ranges
    NvU32  range_max_qp[NUM_BUF_RANGES];     // maximum QP for each of the RC ranges
    NvU32  range_bpg_offset[NUM_BUF_RANGES]; // bpg adjustment for each of the RC ranges
    //420,422
    NvU32  native_420;                       // 420 native mode
    NvU32  native_422;                       // 422 native mode
    NvU32  second_line_bpg_offset;           // 2nd line bpg offset to use, native 420 only
    NvU32  nsl_bpg_offset;                   // non-2nd line bpg offset to use, native 420 only
    NvU32  second_line_offset_adj;           // adjustment to 2nd line bpg offset, native 420 only

    //additional params not in PPS
    NvU32 slice_num;
    NvU32 groups_per_line;
    NvU32 num_extra_mux_bits;
    NvU32 flatness_det_thresh;
} DSC_OUTPUT_PARAMS;

//
// Opaque scratch space is passed by client for DSC calculation usage.
// Use an internal struct to cast the input buffer
// into in/out params for DSC PPS calculation functions to work with
//
typedef struct _DSC_GENERATE_PPS_WORKAREA
{
    DSC_INPUT_PARAMS  in;
    DSC_OUTPUT_PARAMS out;
} DSC_GENERATE_PPS_WORKAREA;

// Compile time check to ensure Opaque workarea buffer size always covers required work area. 
ct_assert(sizeof(DSC_GENERATE_PPS_OPAQUE_WORKAREA) == sizeof(DSC_GENERATE_PPS_WORKAREA));

/* ------------------------ Global Variables ------------------------------- */

static const NvU8 minqp444_8b[15][37]={
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{ 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{ 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{ 3, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{ 5, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{ 5, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0}
       ,{ 5, 5, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0}
       ,{ 5, 5, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0}
       ,{ 5, 5, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 0, 0, 0}
       ,{ 6, 5, 5, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 1, 1, 1, 1, 1, 1, 0, 0, 0}
       ,{ 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 0}
       ,{ 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 0}
       ,{ 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 0}
       ,{ 9, 9, 9, 9, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 5, 5, 5, 5, 4, 4, 3, 3, 3, 3, 2, 2, 1, 1, 1}
       ,{14,14,13,13,12,12,12,12,11,11,10,10,10,10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 3, 3, 3, 3}
};

static const NvU8 maxqp444_8b[15][37]={
        { 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{ 6, 6, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0}
       ,{ 8, 7, 7, 6, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0}
       ,{ 8, 8, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 4, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0}
       ,{ 9, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 5, 4, 4, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 0}
       ,{ 9, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 5, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1}
       ,{ 9, 9, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 5, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1}
       ,{10,10, 9, 9, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 6, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1}
       ,{11,11,10,10, 9, 9, 9, 9, 9, 9, 8, 8, 8, 7, 7, 6, 6, 5, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 2, 2, 2, 2, 2, 2, 1, 1, 1}
       ,{12,11,11,10,10,10, 9, 9, 9, 9, 9, 9, 9, 8, 8, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 2, 2, 2, 2, 2, 2, 1, 1, 1}
       ,{12,12,11,11,10,10,10,10,10,10, 9, 9, 9, 8, 8, 7, 7, 6, 6, 6, 5, 5, 4, 4, 4, 4, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 1}
       ,{12,12,12,11,11,11,10,10,10,10, 9, 9, 9, 9, 8, 8, 8, 7, 7, 7, 6, 6, 5, 5, 5, 5, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1}
       ,{12,12,12,12,11,11,11,11,11,10,10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 6, 6, 5, 5, 5, 5, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1}
       ,{13,13,13,13,12,12,11,11,11,11,10,10,10,10, 9, 9, 8, 8, 8, 8, 7, 7, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 3, 3, 2, 2, 2}
       ,{15,15,14,14,13,13,13,13,12,12,11,11,11,11,10,10,10, 9, 9, 9, 8, 8, 8, 8, 7, 7, 6, 6, 6, 6, 5, 5, 5, 4, 4, 4, 4}
};

static const NvU8 minqp444_10b[15][49]={
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{ 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{ 7, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{ 7, 7, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{ 9, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{ 9, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0}
       ,{ 9, 9, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 5, 5, 5, 5, 5, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0}
       ,{ 9, 9, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 0, 0, 0, 0}
       ,{ 9, 9, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 0, 0}
       ,{10, 9, 9, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 0}
       ,{10,10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1}
       ,{10,10,10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 1, 1, 1}
       ,{10,10,10,10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1}
       ,{12,12,12,12,12,12,12,12,12,12,11,11,11,11,11,11,11,11,11,11,10,10, 9, 9, 9, 9, 8, 8, 7, 7, 7, 7, 6, 6, 5, 5, 5, 5, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 1}
       ,{18,18,17,17,16,16,16,16,15,15,14,14,14,14,13,13,13,12,12,12,11,11,11,11,10,10, 9, 9, 9, 9, 9, 8, 8, 7, 7, 7, 7, 7, 6, 6, 5, 5, 5, 5, 4, 4, 3, 3, 3}
};

static const NvU8 maxqp444_10b[15][49]={
        { 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 5, 5, 4, 4, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{10,10, 9, 9, 8, 8, 8, 8, 8, 8, 7, 7, 6, 6, 6, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{12,11,11,10, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 8, 7, 6, 6, 5, 5, 5, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{12,12,11,11,10,10,10,10,10,10,10,10, 9, 9, 9, 8, 7, 7, 6, 6, 6, 5, 5, 5, 5, 5, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0}
       ,{13,12,12,11,11,11,11,11,11,11,11,11,10,10, 9, 8, 8, 7, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0}
       ,{13,12,12,12,11,11,11,11,11,11,11,11,10,10,10, 9, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 0, 0}
       ,{13,13,12,12,11,11,11,11,11,11,11,11,11,10,10, 9, 8, 8, 7, 7, 7, 7, 7, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1}
       ,{14,14,13,13,12,12,12,12,12,12,12,12,12,11,11,10, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 5, 5, 5, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1}
       ,{15,15,14,14,13,13,13,13,13,13,12,12,12,11,11,10,10, 9, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 6, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 1, 1}
       ,{16,15,15,14,14,14,13,13,13,13,13,13,13,12,12,11,10,10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 1}
       ,{16,16,15,15,14,14,14,14,14,14,13,13,13,12,12,11,11,10,10,10, 9, 9, 8, 8, 8, 8, 7, 7, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2, 2}
       ,{16,16,16,15,15,15,14,14,14,14,13,13,13,13,12,12,12,11,11,11,10,10, 9, 9, 9, 9, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 2, 2, 2}
       ,{16,16,16,16,15,15,15,15,15,14,14,13,13,13,12,12,12,11,11,11,10,10, 9, 9, 9, 9, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2}
       ,{17,17,17,17,16,16,15,15,15,15,14,14,14,14,13,13,12,12,12,12,11,11,10,10,10,10, 9, 9, 8, 8, 8, 8, 7, 7, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2}
       ,{19,19,18,18,17,17,17,17,16,16,15,15,15,15,14,14,14,13,13,13,12,12,12,12,11,11,10,10,10,10,10, 9, 9, 8, 8, 8, 8, 8, 7, 7, 6, 6, 6, 6, 5, 5, 4, 4, 4}
};

static const NvU8 minqp444_12b[15][61]={
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{ 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{11,10,10, 9, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 5, 5, 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{11,11,10,10, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 8, 7, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{13,12,12,11,11,11,11,11,11,11,11,11,10,10, 9, 9, 9, 8, 7, 7, 7, 7, 5, 5, 5, 5, 5, 5, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{13,12,12,12,11,11,11,11,11,11,11,11,11,11,11,10, 9, 9, 8, 8, 8, 8, 6, 6, 6, 6, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{13,13,12,12,11,11,11,11,11,11,11,11,11,11,11,10, 9, 9, 9, 9, 9, 9, 9, 8, 8, 8, 7, 7, 7, 6, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0}
       ,{13,13,12,12,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10,10,10, 9, 9, 9, 9, 8, 8, 8, 7, 7, 7, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 3, 3, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0}
       ,{13,13,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10, 9, 9, 8, 7, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 5, 5, 5, 4, 4, 4, 4, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 0, 0}
       ,{14,13,13,12,12,12,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,10,10,10,10, 9, 9, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 6, 6, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 0}
       ,{14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,11,11,11,11,11,11,10,10, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 8, 7, 7, 7, 6, 6, 6, 5, 5, 5, 5, 4, 4, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1}
       ,{14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,11,11,11,11,11,11,10,10,10,10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 5, 5, 5, 4, 4, 4, 3, 3, 3, 3, 2, 2, 1, 1, 1}
       ,{14,14,14,14,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,12,12,12,12,12,12,11,11,10,10,10,10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 5, 4, 4, 4, 3, 3, 3, 3, 2, 2, 1, 1, 1}
       ,{17,17,17,17,16,16,15,15,15,15,15,15,15,15,15,15,15,15,15,15,14,14,13,13,13,13,12,12,11,11,11,11,10,10, 9, 9, 9, 9, 8, 8, 7, 7, 7, 7, 7, 6, 6, 6, 5, 5, 5, 5, 4, 4, 3, 3, 3, 3, 2, 2, 1}
       ,{22,22,21,21,20,20,20,20,19,19,18,18,18,18,17,17,17,16,16,16,15,15,15,15,14,14,13,13,13,13,13,12,12,11,11,11,11,11,10,10, 9, 9, 9, 9, 9, 8, 8, 7, 7, 7, 7, 7, 6, 6, 5, 5, 5, 5, 4, 4, 3}
};

static const NvU8 maxqp444_12b[15][61]={
        {12,12,12,12,12,12,11,11,11,10, 9, 9, 6, 6, 5, 5, 5, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{14,14,13,13,12,12,12,12,12,12,11,11, 9, 9, 9, 8, 8, 7, 7, 7, 7, 5, 5, 5, 5, 5, 4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{16,15,15,14,13,13,13,13,13,13,13,13,12,12,12,11,10,10, 9, 9, 9, 7, 7, 7, 7, 7, 6, 6, 5, 5, 5, 5, 4, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{16,16,15,15,14,14,14,14,14,14,14,14,13,13,13,12,11,11,10,10,10, 8, 8, 8, 8, 8, 7, 7, 6, 5, 5, 5, 5, 5, 5, 5, 4, 4, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
       ,{17,16,16,15,15,15,15,15,15,15,15,15,14,14,13,12,12,11,10,10,10,10, 8, 8, 8, 8, 8, 8, 7, 7, 7, 6, 6, 5, 5, 5, 4, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0}
       ,{17,16,16,16,15,15,15,15,15,15,15,15,14,14,14,13,12,12,11,11,11,11, 9, 9, 9, 9, 8, 8, 8, 8, 7, 6, 6, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 0}
       ,{17,17,16,16,15,15,15,15,15,15,15,15,15,14,14,13,12,12,11,11,11,11,11,10,10,10, 9, 9, 9, 8, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 3, 3, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 0}
       ,{18,18,17,17,16,16,16,16,16,16,16,16,16,15,15,14,13,13,12,12,12,12,11,11,11,11,10,10,10, 8, 8, 8, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6, 5, 5, 5, 4, 4, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 1}
       ,{19,19,18,18,17,17,17,17,17,17,16,16,16,15,15,14,14,13,13,13,13,13,12,12,12,12,11,11,10, 9, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 6, 6, 6, 5, 5, 5, 5, 4, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 1, 1}
       ,{20,19,19,18,18,18,17,17,17,17,17,17,17,16,16,15,14,14,13,13,13,13,12,12,12,12,11,11,10,10, 9, 9, 9, 9, 8, 8, 8, 8, 8, 7, 7, 6, 6, 6, 5, 5, 5, 5, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 2, 2, 1}
       ,{20,20,19,19,18,18,18,18,18,18,17,17,17,16,16,15,15,14,14,14,13,13,12,12,12,12,11,11,10,10,10,10,10,10,10,10, 9, 9, 9, 8, 8, 8, 7, 7, 7, 6, 6, 6, 6, 5, 5, 4, 4, 4, 3, 3, 3, 3, 2, 2, 2}
       ,{20,20,20,19,19,19,18,18,18,18,17,17,17,17,16,16,16,15,15,15,14,14,13,13,13,13,12,12,11,11,11,11,10,10,10,10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 6, 6, 6, 5, 5, 5, 4, 4, 4, 4, 3, 3, 2, 2, 2}
       ,{20,20,20,20,19,19,19,19,19,18,18,17,17,17,16,16,16,15,15,15,14,14,13,13,13,13,12,12,11,11,11,11,10,10,10,10, 9, 9, 9, 9, 8, 8, 8, 8, 7, 7, 7, 7, 6, 5, 5, 5, 4, 4, 4, 4, 3, 3, 2, 2, 2}
       ,{21,21,21,21,20,20,19,19,19,19,18,18,18,18,17,17,16,16,16,16,15,15,14,14,14,14,13,13,12,12,12,12,11,11,10,10,10,10, 9, 9, 8, 8, 8, 8, 8, 7, 7, 7, 6, 6, 6, 6, 5, 5, 4, 4, 4, 4, 3, 3, 2}
       ,{23,23,22,22,21,21,21,21,20,20,19,19,19,19,18,18,18,17,17,17,16,16,16,16,15,15,14,14,14,14,14,13,13,12,12,12,12,12,11,11,10,10,10,10,10, 9, 9, 8, 8, 8, 8, 8, 7, 7, 6, 6, 6, 6, 5, 5, 4}
};

static const NvU8 minqp422_8b[15][21] = {
        {0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{1 ,1 ,1 ,1 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{3 ,3 ,3 ,3 ,3 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{3 ,3 ,3 ,3 ,3 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,0 ,0 ,0 ,0}
       ,{3 ,3 ,3 ,3 ,3 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,0 ,0 ,0}
       ,{3 ,3 ,3 ,3 ,3 ,3 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,0 ,0}
       ,{3 ,3 ,3 ,3 ,3 ,3 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1}
       ,{3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,2 ,2 ,2 ,1 ,1 ,1}
       ,{5 ,5 ,5 ,5 ,5 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,3 ,3 ,3 ,2 ,2 ,1 ,1 ,1}
       ,{5 ,5 ,5 ,5 ,5 ,5 ,5 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,3 ,3 ,3 ,2 ,2 ,1 ,1}
       ,{5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,4 ,4 ,3 ,3 ,2 ,2 ,1 ,1}
       ,{8 ,8 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,6 ,6 ,5 ,5 ,4 ,4 ,3 ,3 ,2 ,2 ,2}
       ,{12,12,11,11,10,10,9 ,9 ,8 ,8 ,7 ,7 ,6 ,6 ,5 ,5 ,4 ,4 ,4 ,3 ,3}
};

static const NvU8 maxqp422_8b[15][21] = {
        {4 ,4 ,3 ,3 ,2 ,2 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{4 ,4 ,4 ,4 ,4 ,3 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{5 ,5 ,5 ,5 ,5 ,4 ,3 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,0 ,0 ,0 ,0}
       ,{6 ,6 ,6 ,6 ,6 ,5 ,4 ,3 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,0 ,0}
       ,{7 ,7 ,7 ,7 ,7 ,6 ,5 ,3 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1}
       ,{7 ,7 ,7 ,7 ,7 ,6 ,5 ,4 ,3 ,3 ,3 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1}
       ,{7 ,7 ,7 ,7 ,7 ,6 ,5 ,4 ,3 ,3 ,3 ,3 ,3 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1}
       ,{8 ,8 ,8 ,8 ,8 ,7 ,6 ,5 ,4 ,4 ,4 ,3 ,3 ,3 ,3 ,2 ,2 ,2 ,2 ,1 ,1}
       ,{9 ,9 ,9 ,8 ,8 ,7 ,6 ,6 ,5 ,5 ,5 ,4 ,4 ,3 ,3 ,3 ,3 ,2 ,2 ,2 ,2}
       ,{10,10,9 ,9 ,9 ,8 ,7 ,6 ,5 ,5 ,5 ,5 ,4 ,4 ,4 ,3 ,3 ,3 ,2 ,2 ,2}
       ,{10,10,10,9 ,9 ,8 ,7 ,7 ,6 ,6 ,6 ,5 ,5 ,4 ,4 ,4 ,3 ,3 ,2 ,2 ,2}
       ,{11,11,10,10,9 ,9 ,8 ,7 ,7 ,7 ,6 ,6 ,5 ,5 ,4 ,4 ,4 ,3 ,3 ,2 ,2}
       ,{11,11,11,10,9 ,9 ,8 ,8 ,7 ,7 ,7 ,6 ,6 ,5 ,5 ,4 ,4 ,3 ,3 ,2 ,2}
       ,{12,12,11,11,10,10,9 ,9 ,8 ,8 ,7 ,7 ,6 ,6 ,5 ,5 ,4 ,4 ,3 ,3 ,3}
       ,{13,13,12,12,11,11,10,10,9 ,9 ,8 ,8 ,7 ,7 ,6 ,6 ,5 ,5 ,5 ,4 ,4}
};

static const NvU8 minqp422_10b[15][29] = {
        {0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{4 ,4 ,4 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{5 ,5 ,5 ,4 ,3 ,3 ,3 ,3 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{6 ,6 ,6 ,6 ,5 ,4 ,4 ,4 ,3 ,3 ,3 ,3 ,3 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{6 ,6 ,6 ,6 ,5 ,5 ,5 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,3 ,3 ,3 ,2 ,2 ,2 ,1 ,1 ,1 ,0 ,0 ,0 ,0 ,0}
       ,{6 ,6 ,6 ,6 ,6 ,5 ,5 ,5 ,5 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,3 ,3 ,3 ,3 ,2 ,1 ,1 ,0 ,0 ,0 ,0 ,0}
       ,{6 ,6 ,6 ,6 ,6 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,4 ,4 ,4 ,4 ,4 ,3 ,3 ,3 ,2 ,2 ,1 ,1 ,1 ,0 ,0 ,0}
       ,{7 ,7 ,7 ,7 ,7 ,6 ,6 ,6 ,6 ,6 ,6 ,5 ,5 ,5 ,5 ,4 ,4 ,4 ,4 ,3 ,3 ,3 ,2 ,2 ,1 ,1 ,1 ,1 ,1}
       ,{7 ,7 ,7 ,7 ,7 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,5 ,5 ,5 ,4 ,4 ,4 ,4 ,4 ,3 ,2 ,2 ,1 ,1 ,1 ,1 ,1}
       ,{8 ,8 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,6 ,6 ,6 ,6 ,5 ,5 ,4 ,4 ,4 ,3 ,3 ,2 ,2 ,1 ,1 ,1 ,1}
       ,{9 ,9 ,9 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,7 ,7 ,6 ,6 ,6 ,5 ,5 ,5 ,5 ,5 ,3 ,3 ,2 ,2 ,2 ,1 ,1 ,1}
       ,{9 ,9 ,9 ,9 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,7 ,7 ,6 ,6 ,6 ,6 ,6 ,5 ,5 ,4 ,3 ,3 ,2 ,2 ,1 ,1 ,1}
       ,{9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,8 ,8 ,8 ,8 ,7 ,7 ,6 ,6 ,5 ,5 ,4 ,3 ,3 ,3 ,2 ,2 ,1 ,1}
       ,{12,12,11,11,11,11,11,11,11,11,10,10,9 ,9 ,8 ,8 ,7 ,7 ,6 ,6 ,5 ,5 ,4 ,4 ,3 ,3 ,2 ,2 ,1}
       ,{16,16,15,15,14,14,13,13,12,12,11,11,10,10,9 ,9 ,8 ,8 ,8 ,7 ,7 ,6 ,6 ,5 ,5 ,5 ,4 ,4 ,3}
};

static const NvU8 maxqp422_10b[15][29] = {
        {8 ,8 ,7 ,5 ,4 ,4 ,3 ,3 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{8 ,8 ,8 ,6 ,6 ,5 ,4 ,4 ,3 ,3 ,3 ,3 ,3 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{9 ,9 ,9 ,8 ,7 ,6 ,5 ,4 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,0 ,0 ,0 ,0 ,0}
       ,{10,10,10,10,9 ,8 ,7 ,6 ,5 ,5 ,5 ,5 ,5 ,4 ,4 ,3 ,3 ,3 ,3 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1}
       ,{11,11,11,11,10,9 ,8 ,6 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,4 ,4 ,4 ,3 ,3 ,3 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1}
       ,{11,11,11,11,11,10,9 ,8 ,7 ,6 ,6 ,5 ,5 ,5 ,5 ,5 ,5 ,4 ,4 ,4 ,4 ,3 ,2 ,2 ,1 ,1 ,1 ,1 ,1}
       ,{11,11,11,11,11,10,9 ,8 ,7 ,7 ,7 ,7 ,7 ,6 ,6 ,6 ,5 ,5 ,4 ,4 ,4 ,3 ,3 ,2 ,2 ,2 ,1 ,1 ,1}
       ,{12,12,12,12,12,11,10,9 ,8 ,8 ,8 ,7 ,7 ,7 ,7 ,6 ,5 ,5 ,5 ,4 ,4 ,4 ,3 ,3 ,2 ,2 ,2 ,2 ,2}
       ,{13,13,13,12,12,11,10,10,9 ,9 ,9 ,8 ,8 ,7 ,7 ,7 ,6 ,5 ,5 ,5 ,5 ,4 ,3 ,3 ,2 ,2 ,2 ,2 ,2}
       ,{14,14,13,13,13,12,11,10,9 ,9 ,9 ,9 ,8 ,8 ,8 ,7 ,6 ,6 ,5 ,5 ,5 ,4 ,4 ,3 ,3 ,2 ,2 ,2 ,2}
       ,{14,14,14,13,13,12,11,11,10,10,10,9 ,9 ,8 ,8 ,8 ,7 ,7 ,6 ,6 ,6 ,4 ,4 ,3 ,3 ,3 ,2 ,2 ,2}
       ,{15,15,14,14,13,13,12,11,11,11,10,10,9 ,9 ,8 ,8 ,8 ,7 ,7 ,6 ,6 ,5 ,4 ,4 ,3 ,3 ,2 ,2 ,2}
       ,{15,15,15,14,13,13,12,12,11,11,11,10,10,9 ,9 ,8 ,8 ,7 ,7 ,6 ,6 ,5 ,4 ,4 ,4 ,3 ,3 ,2 ,2}
       ,{16,16,15,15,14,14,13,13,12,12,11,11,10,10,9 ,9 ,8 ,8 ,7 ,7 ,6 ,6 ,5 ,5 ,4 ,4 ,3 ,3 ,2}
       ,{17,17,16,16,15,15,14,14,13,13,12,12,11,11,10,10,9 ,9 ,9 ,8 ,8 ,7 ,7 ,6 ,6 ,6 ,5 ,5 ,4}
};

static const NvU8 minqp422_12b[15][37] = {
        {0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{4 ,4 ,4 ,4 ,4 ,3 ,3 ,3 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{9 ,9 ,9 ,8 ,7 ,6 ,5 ,5 ,4 ,4 ,4 ,4 ,4 ,4 ,3 ,3 ,2 ,1 ,1 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{10,10,10,10,8 ,8 ,8 ,7 ,6 ,6 ,6 ,6 ,6 ,5 ,4 ,3 ,3 ,3 ,3 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{11,11,11,11,10,9 ,9 ,8 ,7 ,7 ,7 ,7 ,6 ,6 ,5 ,4 ,4 ,4 ,3 ,3 ,3 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{11,11,11,11,11,10,10,9 ,9 ,8 ,8 ,7 ,6 ,6 ,5 ,5 ,5 ,4 ,4 ,4 ,4 ,3 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{11,11,11,11,11,10,10,10,9 ,9 ,9 ,9 ,8 ,7 ,7 ,7 ,6 ,6 ,5 ,5 ,5 ,4 ,4 ,3 ,3 ,3 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,0 ,0 ,0 ,0}
       ,{11,11,11,11,11,11,10,10,10,10,10,9 ,8 ,8 ,8 ,7 ,6 ,6 ,6 ,5 ,5 ,5 ,4 ,4 ,3 ,3 ,3 ,3 ,3 ,2 ,2 ,2 ,1 ,1 ,0 ,0 ,0}
       ,{11,11,11,11,11,11,11,11,11,11,11,10,9 ,8 ,8 ,8 ,7 ,6 ,6 ,6 ,6 ,5 ,4 ,4 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,2 ,2 ,1 ,0 ,0 ,0}
       ,{11,11,11,11,11,11,11,11,11,11,11,11,9 ,9 ,9 ,8 ,7 ,7 ,6 ,6 ,6 ,5 ,5 ,4 ,4 ,3 ,3 ,3 ,3 ,3 ,3 ,2 ,2 ,1 ,1 ,0 ,0}
       ,{13,13,13,13,13,12,12,12,12,12,12,11,11,10,10,10,9 ,9 ,8 ,8 ,8 ,6 ,6 ,5 ,5 ,5 ,4 ,4 ,4 ,4 ,3 ,3 ,2 ,2 ,1 ,1 ,1}
       ,{13,13,13,13,13,13,13,13,13,13,12,12,11,11,10,10,10,9 ,9 ,8 ,8 ,7 ,6 ,6 ,5 ,5 ,4 ,4 ,4 ,4 ,4 ,3 ,3 ,2 ,2 ,1 ,1}
       ,{13,13,13,13,13,13,13,13,13,13,13,12,12,11,11,10,10,9 ,9 ,8 ,8 ,7 ,6 ,6 ,6 ,5 ,5 ,4 ,4 ,4 ,4 ,3 ,3 ,2 ,2 ,1 ,1}
       ,{16,16,15,15,15,15,15,15,15,15,14,14,13,13,12,12,11,11,10,10,9 ,9 ,8 ,8 ,7 ,7 ,6 ,6 ,5 ,5 ,4 ,4 ,4 ,3 ,3 ,2 ,2}
       ,{20,20,19,19,18,18,17,17,16,16,15,15,14,14,13,13,12,12,12,11,11,10,10,9 ,9 ,9 ,8 ,8 ,7 ,7 ,6 ,6 ,6 ,5 ,5 ,4 ,4}
};

static const NvU8 maxqp422_12b[15][37] = {
        {12,12,11,9 ,6 ,6 ,5 ,5 ,4 ,4 ,4 ,3 ,3 ,3 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{12,12,12,10,9 ,8 ,7 ,7 ,6 ,6 ,5 ,5 ,5 ,4 ,4 ,4 ,3 ,3 ,3 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{13,13,13,12,10,9 ,8 ,7 ,6 ,6 ,6 ,6 ,6 ,6 ,5 ,5 ,4 ,3 ,3 ,3 ,3 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{14,14,14,14,12,11,10,9 ,8 ,8 ,8 ,8 ,8 ,7 ,6 ,5 ,5 ,5 ,5 ,4 ,4 ,4 ,3 ,3 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,0 ,0 ,0 ,0 ,0 ,0}
       ,{15,15,15,15,14,13,12,10,9 ,9 ,9 ,9 ,8 ,8 ,7 ,6 ,6 ,6 ,5 ,5 ,5 ,4 ,4 ,4 ,3 ,3 ,3 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,0 ,0 ,0}
       ,{15,15,15,15,15,14,13,12,11,10,10,9 ,8 ,8 ,7 ,7 ,7 ,6 ,6 ,6 ,6 ,5 ,4 ,4 ,3 ,3 ,3 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1}
       ,{15,15,15,15,15,14,13,12,11,11,11,11,10,9 ,9 ,9 ,8 ,8 ,7 ,7 ,7 ,6 ,6 ,5 ,5 ,5 ,4 ,4 ,4 ,3 ,3 ,2 ,2 ,1 ,1 ,1 ,1}
       ,{16,16,16,16,16,15,14,13,12,12,12,11,10,10,10,9 ,8 ,8 ,8 ,7 ,7 ,7 ,6 ,6 ,5 ,5 ,5 ,5 ,5 ,3 ,3 ,3 ,2 ,2 ,1 ,1 ,1}
       ,{17,17,17,16,16,15,14,14,13,13,13,12,11,10,10,10,9 ,8 ,8 ,8 ,8 ,7 ,6 ,6 ,5 ,5 ,5 ,5 ,5 ,4 ,4 ,3 ,3 ,2 ,1 ,1 ,1}
       ,{18,18,17,17,17,16,15,14,13,13,13,13,11,11,11,10,9 ,9 ,8 ,8 ,8 ,7 ,7 ,6 ,6 ,5 ,5 ,5 ,5 ,4 ,4 ,3 ,3 ,2 ,2 ,1 ,1}
       ,{18,18,18,17,17,16,15,15,14,14,14,13,13,12,12,12,11,11,10,10,10,8 ,8 ,7 ,7 ,7 ,6 ,6 ,6 ,5 ,4 ,4 ,3 ,3 ,2 ,2 ,2}
       ,{19,19,18,18,17,17,16,15,15,15,14,14,13,13,12,12,12,11,11,10,10,9 ,8 ,8 ,7 ,7 ,6 ,6 ,6 ,5 ,5 ,4 ,4 ,3 ,3 ,2 ,2}
       ,{19,19,19,18,17,17,16,16,15,15,15,14,14,13,13,12,12,11,11,10,10,9 ,8 ,8 ,8 ,7 ,7 ,6 ,6 ,5 ,5 ,4 ,4 ,3 ,3 ,2 ,2}
       ,{20,20,19,19,18,18,17,17,16,16,15,15,14,14,13,13,12,12,11,11,10,10,9 ,9 ,8 ,8 ,7 ,7 ,6 ,6 ,5 ,5 ,5 ,4 ,4 ,3 ,3}
       ,{21,21,20,20,19,19,18,18,17,17,16,16,15,15,14,14,13,13,13,12,12,11,11,10,10,10,9 ,9 ,8 ,8 ,7 ,7 ,7 ,6 ,6 ,5 ,5}
};

static const NvU32 rcBufThresh[] = { 896, 1792, 2688, 3584, 4480, 5376, 6272, 6720, 7168, 7616, 7744, 7872, 8000, 8064 };

/* ------------------------ Static Variables ------------------------------- */
/* ------------------------ Private Functions Prototype--------------------- */
static NvU32
DSC_GetHigherSliceCount
(
    NvU32 common_slice_count_mask, 
    NvU32 desired_slice_num, 
    NvU32 *new_slice_num
);
static NvU32 DSC_AlignDownForBppPrecision(NvU32 bitsPerPixelX16, NvU32 bitsPerPixelPrecision);

static NvU32
DSC_GetPeakThroughputMps(NvU32 peak_throughput);

static NvU32 
DSC_SliceCountMaskforSliceNum (NvU32 slice_num);

static NvU32
DSC_GetSliceCountMask(NvU32 maxSliceNum, NvBool bInclusive);

static NVT_STATUS
DSC_GetMinSliceCountForMode
(
    NvU32  picWidth,
    NvU32  pixelClkMhz,
    NvU32  maxSliceWidth,
    NvU32  peakThroughPutMps,
    NvU32  maxSliceCount,
    NvU32  commonSliceCountMask,
    NvU32 *pMinSliceCount
);

/* ------------------------ Private Functions ------------------------------ */

/*
 * @brief Calculate Bits Per Pixel aligned down as per bitsPerPixelPrecision supported
 *        by Sink
 *
 * @param[in]   bitsPerPixelX16         Bits Per Pixel
 * @param[in]   bitsPerPixelPrecision   Bits Per Pixel Precision Supported by Panel
 *
 * @returns Aligned down Bits Per Pixel value
 */
static NvU32
DSC_AlignDownForBppPrecision
(
    NvU32 bitsPerPixelX16,
    NvU32 bitsPerPixelPrecision
)
{
    NvU32 allignDownForBppPrecision;

    switch (bitsPerPixelPrecision)
    {
        case DSC_BITS_PER_PIXEL_PRECISION_1_16:
        allignDownForBppPrecision = 1;
        break;

        case DSC_BITS_PER_PIXEL_PRECISION_1_8:
        allignDownForBppPrecision = 2;
        break;

        case DSC_BITS_PER_PIXEL_PRECISION_1_4:
        allignDownForBppPrecision = 4;
        break;

        case DSC_BITS_PER_PIXEL_PRECISION_1_2:
        allignDownForBppPrecision = 8;
        break;

        case DSC_BITS_PER_PIXEL_PRECISION_1:
        allignDownForBppPrecision = 16;
        break;

        default:
        allignDownForBppPrecision = 16;
    }

    return (bitsPerPixelX16 & ~(allignDownForBppPrecision - 1));
}

/*
 * @brief Calculate chunk size, num_extra_mux_bits
 *
 * @param[in/out]   out   DSC output parameter
 *
 * @returns NVT_STATUS_SUCCESS if successful;
 *          NVT_STATUS_ERR if unsuccessful;
 */
static NVT_STATUS
DSC_PpsCalcExtraBits
(
    DSC_OUTPUT_PARAMS *out
)
{
    NvU32 numSsps = out->native_422 ? 4 : 3;
    NvU32 sliceBits;
    NvU32 extra_bits;
    NvU32 bitsPerComponent = out->bits_per_component;
    NvU32 muxWordSize;

    muxWordSize = (bitsPerComponent >= 12) ? 64 : 48;
    if (out->convert_rgb)
    {
        extra_bits = (numSsps * (muxWordSize + (4 * bitsPerComponent + 4) - 2));
    }
    else if (!out->native_422) // YCbCr
    {
        extra_bits = (numSsps * muxWordSize + (4 * bitsPerComponent + 4) + 2 * (4 * bitsPerComponent) - 2);
    }
    else
    {
        extra_bits = (numSsps * muxWordSize + (4 * bitsPerComponent + 4) + 3 * (4 * bitsPerComponent) - 2);
    }

    sliceBits = 8 * out->chunk_size * out->slice_height;
    //while ((extra_bits>0) && ((sliceBits - extra_bits) % muxWordSize))
    //  extra_bits--;
    sliceBits = (sliceBits - extra_bits) % muxWordSize;
    if (sliceBits != 0)
    {
        extra_bits -= MIN(extra_bits, muxWordSize - sliceBits);
    }

    out->num_extra_mux_bits = extra_bits;
    return NVT_STATUS_SUCCESS;
}

/*
 * @brief Calculate RC initial value.
 *        Require: groups_per_line in Dsc_PpsCalcWidth()
 *
 * @param[in/out]   out   DSC output parameter
 *
 * @returns NVT_STATUS_SUCCESS if successful;
 *          NVT_STATUS_ERR if unsuccessful;
 */
static NVT_STATUS
DSC_PpsCalcRcInitValue
(
    DSC_OUTPUT_PARAMS *out
)
{
    NvU32 bitsPerPixel = out->bits_per_pixel;
    NvU32 xmit_delay;
    out->rc_model_size = 8192;

    if (out->native_422)
    {
        // =IF(CompressBpp >= 8, 2048, IF(CompressBpp <=  7, 5632, 5632 - ROUND((CompressBpp - 7) * (3584), 0)))
        if (bitsPerPixel >= 16 * BPP_UNIT)
            out->initial_offset = 2048;
        else if (bitsPerPixel >= 14 * BPP_UNIT)
            out->initial_offset = 5632 - ((bitsPerPixel - 14 * BPP_UNIT) * 1792 + BPP_UNIT / 2) / BPP_UNIT;
        else
            out->initial_offset = 5632;
    }
    else
    {
        if (bitsPerPixel >= 12 * BPP_UNIT)
            out->initial_offset = 2048;
        else if (bitsPerPixel >= 10 * BPP_UNIT)
            out->initial_offset = 5632 - ((bitsPerPixel - 10 * BPP_UNIT) * 1792 + BPP_UNIT / 2) / BPP_UNIT;
        else if (bitsPerPixel >= 8 * BPP_UNIT)
            out->initial_offset = 6144 - ((bitsPerPixel - 8 * BPP_UNIT) * 256 + BPP_UNIT / 2) / BPP_UNIT;
        else
            out->initial_offset = 6144;
    }
    RANGE_CHECK("initial_offset", out->initial_offset, 0, out->rc_model_size);

    out->initial_scale_value = 8 * out->rc_model_size / (out->rc_model_size - out->initial_offset);
    if (out->groups_per_line < out->initial_scale_value - 8)
    {
        out->initial_scale_value = out->groups_per_line + 8;
    }
    RANGE_CHECK("initial_scale_value", out->initial_scale_value, 0, 63);

    xmit_delay = (4096*BPP_UNIT + bitsPerPixel/2) / bitsPerPixel;

    if (out->native_420 || out->native_422)
    {
	    NvU32 slicew = (out->native_420 || out->native_422) ? out->slice_width / 2 : out->slice_width;
	    NvU32 padding_pixels = ((slicew % 3) ? (3 - (slicew % 3)) : 0) * (xmit_delay / slicew);
	    if (3 * bitsPerPixel >= ((xmit_delay + 2) / 3) * (out->native_422 ? 4 : 3) * BPP_UNIT &&
			    (((xmit_delay + padding_pixels) % 3) == 1))
        {
		    xmit_delay++;
	    }
    }
    out->initial_xmit_delay = xmit_delay;
    RANGE_CHECK("initial_xmit_delay", out->initial_xmit_delay, 0, 1023);

    return NVT_STATUS_SUCCESS;
}

static NvU32 DSC_PpsCalcComputeOffset(DSC_OUTPUT_PARAMS *out, NvU32 grpcnt)
{
	NvU32 offset = 0;
	NvU32 groupsPerLine = out->groups_per_line;
	NvU32 grpcnt_id = (out->initial_xmit_delay + PIXELS_PER_GROUP - 1) / PIXELS_PER_GROUP;

	if(grpcnt <= grpcnt_id)
		offset = (grpcnt * PIXELS_PER_GROUP * out->bits_per_pixel + BPP_UNIT - 1) / BPP_UNIT;
	else
		offset = (grpcnt_id * PIXELS_PER_GROUP * out->bits_per_pixel + BPP_UNIT - 1) / BPP_UNIT - (((grpcnt-grpcnt_id) * out->slice_bpg_offset)>>OFFSET_FRACTIONAL_BITS);

	if(grpcnt <= groupsPerLine)
		offset += grpcnt * out->first_line_bpg_offset;
	else
		offset += groupsPerLine * out->first_line_bpg_offset - (((grpcnt - groupsPerLine) * out->nfl_bpg_offset)>>OFFSET_FRACTIONAL_BITS);

	if(out->native_420)
	{
		if(grpcnt <= groupsPerLine)
			offset -= (grpcnt * out->nsl_bpg_offset) >> OFFSET_FRACTIONAL_BITS;
		else if(grpcnt <= 2*groupsPerLine)
			offset += (grpcnt - groupsPerLine) * out->second_line_bpg_offset - ((groupsPerLine * out->nsl_bpg_offset)>>OFFSET_FRACTIONAL_BITS);
		else
			offset += (grpcnt - groupsPerLine) * out->second_line_bpg_offset - (((grpcnt - groupsPerLine) * out->nsl_bpg_offset)>>OFFSET_FRACTIONAL_BITS);
	}
	return(offset);
}

/*
 * @brief Calculate bpg value except slice_bpg_offset
 *
 * @param[in/out]   out   DSC output parameter
 *
 * @returns NVT_STATUS_SUCCESS if successful;
 *          NVT_STATUS_ERR if unsuccessful;
 */
static
NvU32 DSC_PpsCalcBpg
(
    DSC_OUTPUT_PARAMS *out
)
{
    NvU32 uncompressedBpgRate;
    NvU32 ub_BpgOfs;
    NvU32 firstLineBpgOfs;
    NvU32 secondLineBpgOfs;
    NvU32 bitsPerPixel;
    NvU32 rbsMin;
    NvU32 hrdDelay;
    NvU32 groups_total;

    if (out->native_422)
        uncompressedBpgRate = PIXELS_PER_GROUP * out->bits_per_component * 4;
    else
        uncompressedBpgRate = (3 * out->bits_per_component + (out->convert_rgb ? 2 : 0)) * PIXELS_PER_GROUP;

    ub_BpgOfs = (uncompressedBpgRate*BPP_UNIT - PIXELS_PER_GROUP * out->bits_per_pixel) / BPP_UNIT;

    if (out->slice_height >= 8)
        firstLineBpgOfs = 12 + MIN(34, out->slice_height - 8) * 9 / 100;
    else
        firstLineBpgOfs = 2 * (out->slice_height - 1);

    firstLineBpgOfs = CLAMP(firstLineBpgOfs, 0, ub_BpgOfs);
    out->first_line_bpg_offset = firstLineBpgOfs;
    RANGE_CHECK("first_line_bpg_offset", out->first_line_bpg_offset, 0, 31);

    if (out->slice_height > 1)
        out->nfl_bpg_offset = ((out->first_line_bpg_offset << OFFSET_FRACTIONAL_BITS) + out->slice_height - 2) / (out->slice_height - 1);
    else
        out->nfl_bpg_offset = 0;

    RANGE_CHECK("nfl_bpg_offset", out->nfl_bpg_offset, 0, 65535);

    secondLineBpgOfs = out->native_420 ? 12 : 0;
    secondLineBpgOfs = CLAMP(secondLineBpgOfs, 0, ub_BpgOfs);
    out->second_line_bpg_offset = secondLineBpgOfs;
    RANGE_CHECK("second_line_bpg_offset", out->second_line_bpg_offset, 0, 31);

    if (out->slice_height > 2)
        out->nsl_bpg_offset = ((out->second_line_bpg_offset << OFFSET_FRACTIONAL_BITS) + out->slice_height - 2) / (out->slice_height - 1);
    else
        out->nsl_bpg_offset = 0;
    RANGE_CHECK("nsl_bpg_offset", out->nsl_bpg_offset, 0, 65535);

    out->second_line_offset_adj = out->native_420 ? 512 : 0;

    bitsPerPixel = out->bits_per_pixel;
    groups_total = out->groups_per_line * out->slice_height;
    out->slice_bpg_offset = (((out->rc_model_size - out->initial_offset + out->num_extra_mux_bits) << OFFSET_FRACTIONAL_BITS)
                + groups_total - 1) / groups_total;
    RANGE_CHECK("slice_bpg_offset", out->slice_bpg_offset, 0, 65535);

    if((PIXELS_PER_GROUP * bitsPerPixel << OFFSET_FRACTIONAL_BITS) - (out->slice_bpg_offset + out->nfl_bpg_offset) * BPP_UNIT
            < (1+5*PIXELS_PER_GROUP)*BPP_UNIT <<OFFSET_FRACTIONAL_BITS )
    {
        return NVT_STATUS_ERR;
    }

    if (((out->dsc_version_major > 1) || (out->dsc_version_major == 1 && out->dsc_version_minor >= 2)) &&
        (out->native_420 || out->native_422))
    {
        // OPTIMIZED computation of rbsMin:
        // Compute max by sampling offset at points of inflection
        // *MODEL NOTE* MN_RBS_MIN
        NvU32 maxOffset;
        maxOffset = DSC_PpsCalcComputeOffset(out, (out->initial_xmit_delay+PIXELS_PER_GROUP-1)/PIXELS_PER_GROUP );  // After initial delay
        maxOffset = MAX(maxOffset, DSC_PpsCalcComputeOffset(out, out->groups_per_line));   // After first line
        maxOffset = MAX(maxOffset, DSC_PpsCalcComputeOffset(out, 2*out->groups_per_line));
        rbsMin = out->rc_model_size - out->initial_offset + maxOffset;
    }
    else
    { // DSC 1.1 method
        rbsMin = out->rc_model_size - out->initial_offset
            + (out->initial_xmit_delay * bitsPerPixel + BPP_UNIT - 1) / BPP_UNIT
            + out->groups_per_line * out->first_line_bpg_offset;
    }
    hrdDelay = (rbsMin * BPP_UNIT + bitsPerPixel - 1) / bitsPerPixel;
    out->initial_dec_delay = hrdDelay - out->initial_xmit_delay;
    RANGE_CHECK("initial_dec_delay", out->initial_dec_delay, 0, 65535);

    return NVT_STATUS_SUCCESS;
}

/*
 * @brief Calculate final_offset and scale_increment_interval,
 *        scale_decrement_interval
 *
 * @param[in/out]   out   DSC output parameter
 *
 * @returns NVT_STATUS_SUCCESS if successful;
 *          NVT_STATUS_ERR if unsuccessful;
 */
static NvU32
DSC_PpsCalcScaleInterval
(
    DSC_OUTPUT_PARAMS *out
)
{
    NvU32 final_scale;

    out->final_offset = (out->rc_model_size - (out->initial_xmit_delay * out->bits_per_pixel + 8) /
                         BPP_UNIT + out->num_extra_mux_bits);
    RANGE_CHECK("final_offset", out->final_offset, 0, out->rc_model_size-1); //try increase initial_xmit_delay

    final_scale = 8 * out->rc_model_size / (out->rc_model_size - out->final_offset);
    RANGE_CHECK("final_scale", final_scale, 0, 63); //try increase initial_xmit_delay

    // BEGIN scale_increment_NvU32erval fix
    if(final_scale > 9)
    {
        //
        // Note: the following calculation assumes that the rcXformOffset crosses 0 at some point.  If the zero-crossing
        //   doesn't occur in a configuration, we recommend to reconfigure the rc_model_size and thresholds to be smaller
        //   for that configuration.
        //
        out->scale_increment_interval = (out->final_offset << OFFSET_FRACTIONAL_BITS) /
                                        ((final_scale - 9) * (out->nfl_bpg_offset +
                                        out->slice_bpg_offset + out->nsl_bpg_offset));
        RANGE_CHECK("scale_increment_interval", out->scale_increment_interval, 0, 65535);
    }
    else
    {
        out->scale_increment_interval = 0;
    }

    // END scale_increment_interval fix
    if (out->initial_scale_value > 8)
        out->scale_decrement_interval = out->groups_per_line / (out->initial_scale_value - 8);
    else
        out->scale_decrement_interval = 4095;
    RANGE_CHECK("scale_decrement_interval", out->scale_decrement_interval, 1, 4095);
    return NVT_STATUS_SUCCESS;
}

/*
 * @brief Calculate RC parameters
 *
 * @param[in/out]   out   DSC output parameter
 *
 * @returns NVT_STATUS_SUCCESS if successful;
 *          NVT_STATUS_ERR if unsuccessful;
 */
static NvU32
DSC_PpsCalcRcParam
(
    DSC_OUTPUT_PARAMS *out
)
{
    NvU32 i, idx;
    NvU32 bitsPerPixel = out->bits_per_pixel;
    NvU32 bpcm8 = out->bits_per_component - 8;
    NvU32 yuv_modifier = out->convert_rgb == 0 && out->dsc_version_minor == 1;
    NvU32 qp_bpc_modifier = bpcm8 * 2 - yuv_modifier;
    const int ofs_und6[] = { 2, 0, 0, -2, -4, -6, -8, -8, -8, -10, -10, -12, -12, -12, -12 };
    const int ofs_und7[] = { 2, 0, 0, -2, -4, -6, -8, -8, -8, -10, -10, -10, -12, -12, -12 };
    const int ofs_und10[] = { 10, 8, 6, 4, 2, 0, -2, -4, -6, -8, -10, -10, -12, -12, -12 };

    out->flatness_min_qp = 3 + qp_bpc_modifier;
    out->flatness_max_qp = 12 + qp_bpc_modifier;
    out->flatness_det_thresh = 2 << bpcm8;
    out->rc_edge_factor = 6;
    out->rc_quant_incr_limit0 = 11 + qp_bpc_modifier;
    out->rc_quant_incr_limit1 = 11 + qp_bpc_modifier;
    out->rc_tgt_offset_hi = 3;
    out->rc_tgt_offset_lo = 3;

    for (i = 0; i < NUM_BUF_RANGES - 1; i++)
        out->rc_buf_thresh[i] = rcBufThresh[i] & (0xFF << 6);

    if (out->native_422)
    {
        idx = bitsPerPixel/BPP_UNIT - 12;
        if (bpcm8 == 0)
        {
            for (i = 0; i < NUM_BUF_RANGES; ++i)
            {
                out->range_min_qp[i] = minqp422_8b[i][idx];
                out->range_max_qp[i] = maxqp422_8b[i][idx];
            }
        }
        else if (bpcm8 == 2)
        {
            for (i=0; i < NUM_BUF_RANGES; i++)
            {
                out->range_min_qp[i] = minqp422_10b[i][idx];
                out->range_max_qp[i] = maxqp422_10b[i][idx];
            }
        }
        else
        {
            for (i=0; i<NUM_BUF_RANGES; i++)
            {
                out->range_min_qp[i] = minqp422_12b[i][idx];
                out->range_max_qp[i] = maxqp422_12b[i][idx];
            }
        }

        for (i = 0; i < NUM_BUF_RANGES; ++i)
        {
            if (bitsPerPixel <= 12*BPP_UNIT)
            {
                out->range_bpg_offset[i] = ofs_und6[i];
            }
            else if (bitsPerPixel <= 14*BPP_UNIT)
            {
                out->range_bpg_offset[i] = ofs_und6[i] + ((bitsPerPixel - 12*BPP_UNIT) *
                                           (ofs_und7[i] - ofs_und6[i]) + BPP_UNIT) / (2*BPP_UNIT);
            }
            else if (bitsPerPixel <= 16*BPP_UNIT)
            {
                out->range_bpg_offset[i] = ofs_und7[i];
            }
            else if (bitsPerPixel <= 20*BPP_UNIT)
            {
                out->range_bpg_offset[i] = ofs_und7[i] + ((bitsPerPixel - 16*BPP_UNIT) *
                                           (ofs_und10[i] - ofs_und7[i]) + 2*BPP_UNIT) / (4*BPP_UNIT);
            }
            else
            {
                out->range_bpg_offset[i] = ofs_und10[i];
            }
        }
    }
    else
    {
        idx = (2 * (bitsPerPixel - 6 * BPP_UNIT) ) / BPP_UNIT;

        if (bpcm8 == 0)
        {
            for (i = 0; i < NUM_BUF_RANGES; i++)
            {
                const NvU32 min = minqp444_8b[i][idx];
                const NvU32 max = maxqp444_8b[i][idx];

                out->range_min_qp[i] = MAX(0, min - yuv_modifier);
                out->range_max_qp[i] = MAX(0, max - yuv_modifier);
            }
        }
        else if (bpcm8 == 2)
        {
            for (i = 0; i < NUM_BUF_RANGES; i++)
            {
                const NvU32 min = minqp444_10b[i][idx];
                const NvU32 max = maxqp444_10b[i][idx];

                out->range_min_qp[i] = MAX(0, min - yuv_modifier);
                out->range_max_qp[i] = MAX(0, max - yuv_modifier);
            }
        }
        else
        {
            for (i = 0; i < NUM_BUF_RANGES; i++)
            {
                const NvU32 min = minqp444_12b[i][idx];
                const NvU32 max = maxqp444_12b[i][idx];

                out->range_min_qp[i] = MAX(0, min - yuv_modifier);
                out->range_max_qp[i] = MAX(0, max - yuv_modifier);
            }
        }

        for (i = 0; i < NUM_BUF_RANGES; ++i)
        {
            //if (out->native_420)
            //{
            //  NvU32 ofs_und4[] = { 2, 0, 0, -2, -4, -6, -8, -8, -8, -10, -10, -12, -12, -12, -12 };
            //  NvU32 ofs_und5[] = { 2, 0, 0, -2, -4, -6, -8, -8, -8, -10, -10, -10, -12, -12, -12 };
            //  NvU32 ofs_und6[] = { 2, 0, 0, -2, -4, -6, -8, -8, -8, -10, -10, -10, -12, -12, -12 };
            //  NvU32 ofs_und8[] = { 10, 8, 6, 4, 2, 0, -2, -4, -6, -8, -10, -10, -12, -12, -12 };
            //  out->range_min_qp[i] = minqp_420[bpcm8 / 2][i][idx];
            //  out->range_max_qp[i] = maxqp_420[bpcm8 / 2][i][idx];
            //  if (bitsPerPixel <= 8*BPP_UNIT)
            //      out->range_bpg_offset[i] = ofs_und4[i];
            //  else if (bitsPerPixel <= 10*BPP_UNIT)
            //      out->range_bpg_offset[i] = ofs_und4[i] + (NvU32)(0.5 * (bitsPerPixel - 8.0) * (ofs_und5[i] - ofs_und4[i]) + 0.5);
            //  else if (bitsPerPixel <= 12*BPP_UNIT)
            //      out->range_bpg_offset[i] = ofs_und5[i] + (NvU32)(0.5 * (bitsPerPixel - 10.0) * (ofs_und6[i] - ofs_und5[i]) + 0.5);
            //  else if (bitsPerPixel <= 16*BPP_UNIT)
            //      out->range_bpg_offset[i] = ofs_und6[i] + (NvU32)(0.25 * (bitsPerPixel - 12.0) * (ofs_und8[i] - ofs_und6[i]) + 0.5);
            //  else
            //      out->range_bpg_offset[i] = ofs_und8[i];
            //}
            //else if (out->native_422)
            //{
            //  NvU32 ofs_und6[] = { 2, 0, 0, -2, -4, -6, -8, -8, -8, -10, -10, -12, -12, -12, -12 };
            //  NvU32 ofs_und7[] = { 2, 0, 0, -2, -4, -6, -8, -8, -8, -10, -10, -10, -12, -12, -12 };
            //  NvU32 ofs_und10[] = { 10, 8, 6, 4, 2, 0, -2, -4, -6, -8, -10, -10, -12, -12, -12 };
            //  out->range_min_qp[i] = minqp_422[bpcm8 / 2][i][idx];
            //  out->range_max_qp[i] = maxqp_422[bpcm8 / 2][i][idx];
            //  if (bitsPerPixel <= 12*BPP_UNIT)
            //      out->range_bpg_offset[i] = ofs_und6[i];
            //  else if(bitsPerPixel <= 14*BPP_UNIT)
            //      out->range_bpg_offset[i] = ofs_und6[i] + (NvU32)((bitsPerPixel - 12.0) * (ofs_und7[i] - ofs_und6[i]) / 2.0 + 0.5);
            //  else if(bitsPerPixel <= 16*BPP_UNIT)
            //      out->range_bpg_offset[i] = ofs_und7[i];
            //  else if(bitsPerPixel <= 20*BPP_UNIT)
            //      out->range_bpg_offset[i] = ofs_und7[i] + (NvU32)((bitsPerPixel - 16.0) * (ofs_und10[i] - ofs_und7[i]) / 4.0 + 0.5);
            //  else
            //      out->range_bpg_offset[i] = ofs_und10[i];
            //}
            //else
            {
                const NvU32 ofs_und6[] = { 0, -2, -2, -4, -6, -6, -8, -8, -8, -10, -10, -12, -12, -12, -12 };
                const NvU32 ofs_und8[] = { 2, 0, 0, -2, -4, -6, -8, -8, -8, -10, -10, -10, -12, -12, -12 };
                const NvU32 ofs_und12[] = { 2, 0, 0, -2, -4, -6, -8, -8, -8, -10, -10, -10, -12, -12, -12 };
                const NvU32 ofs_und15[] = { 10, 8, 6, 4, 2, 0, -2, -4, -6, -8, -10, -10, -12, -12, -12 };

                if (bitsPerPixel <= 6 * BPP_UNIT)
                {
                    out->range_bpg_offset[i] = ofs_und6[i];
                }
                else if (bitsPerPixel <= 8 * BPP_UNIT)
                {
                    out->range_bpg_offset[i] = ofs_und6[i] + ((bitsPerPixel - 6 * BPP_UNIT) *
                                               (ofs_und8[i] - ofs_und6[i]) + BPP_UNIT) / (2 * BPP_UNIT);
                }
                else if (bitsPerPixel <= 12 * BPP_UNIT)
                {
                    out->range_bpg_offset[i] = ofs_und8[i];
                }
                else if (bitsPerPixel <= 15 * BPP_UNIT)
                {
                    out->range_bpg_offset[i] = ofs_und12[i] + ((bitsPerPixel - 12 * BPP_UNIT) *
                                               (ofs_und15[i] - ofs_und12[i]) + 3 * BPP_UNIT / 2) / (3 * BPP_UNIT);
                }
                else
                {
                    out->range_bpg_offset[i] = ofs_und15[i];
                }
            }
        }
    }
    return NVT_STATUS_SUCCESS;
}

/*
 * @brief Initialize with basic PPS values based on passed down input params
 *
 * @param[in]   in   DSC input parameter
 * @param[out]  out  DSC output parameter
 *
 * @returns NVT_STATUS_SUCCESS if successful;
 *          NVT_STATUS_ERR if unsuccessful;
 */
static NvU32
DSC_PpsCalcBase
(
    const DSC_INPUT_PARAMS *in,
    DSC_OUTPUT_PARAMS *out
)
{
    out->dsc_version_major = 1;
    ENUM2_CHECK("dsc_version_minor", in->dsc_version_minor, 1, 2);
    out->dsc_version_minor = in->dsc_version_minor == 1 ? 1 : 2;
    out->pps_identifier = 0;
    ENUM3_CHECK("bits_per_component", in->bits_per_component, 8, 10, 12);
    out->bits_per_component = in->bits_per_component;
    out->bits_per_pixel = in->bits_per_pixel;
    RANGE_CHECK("linebuf_depth", in->linebuf_depth, DSC_DECODER_LINE_BUFFER_BIT_DEPTH_MIN, DSC_DECODER_LINE_BUFFER_BIT_DEPTH_MAX);
    out->linebuf_depth = in->linebuf_depth;
    ENUM2_CHECK("block_pred_enable", in->block_pred_enable, 0, 1);
    out->block_pred_enable = in->block_pred_enable ? 1 : 0;
    ENUM2_CHECK("convert_rgb", in->convert_rgb, 0, 1);
    out->convert_rgb = in->convert_rgb ? 1 : 0;

    if (in->multi_tile)
    {
        RANGE_CHECK("pic_width", in->pic_width, 64, 16384);
        RANGE_CHECK("pic_height", in->pic_height, 8, 16384);
    }
    else
    {
        RANGE_CHECK("pic_height", in->pic_height, 8, 8192);
        if (in->dual_mode)
        {
            RANGE_CHECK("pic_width", in->pic_width, 64, 8192);
        }
        else
        {
            RANGE_CHECK("pic_width", in->pic_width, 32, 5120);
        }
    }

    out->pic_height = in->pic_height;
    out->pic_width = in->pic_width;
    out->simple_422 = in->simple_422;
    out->vbr_enable = 0;
    out->native_420 = in->native_420;
    out->native_422 = in->native_422;
    out->slice_num  = in->slice_num;
    out->slice_width= in->slice_width;
    out->slice_height= in->slice_height;

    return NVT_STATUS_SUCCESS;
}

/*
 * @brief Generate 32bit data array from DSC_OUTPUT_PARAMS.
 *
 * @param[in]   in   DSC input parameter
 * @param[out]  out  DSC output parameter
 *                   NvU32[32] to return the pps data.
 *                   The data can be send to SetDscPpsData* methods directly.
 *
 * @returns NVT_STATUS_SUCCESS if successful;
 *          NVT_STATUS_ERR if unsuccessful;
 */
static void
DSC_PpsConstruct
(
    const DSC_OUTPUT_PARAMS *in,
    NvU32 data[DSC_MAX_PPS_SIZE_DWORD]
)
{
    NvU32 i;
    NvU32 pps[96];

    if (data == NULL)
    {
        return;
    }

    pps[0] = (in->dsc_version_major << 4) | (in->dsc_version_minor & 0xF);
    pps[1] = in->pps_identifier;
    pps[2] = 0;
    pps[3] = (in->bits_per_component << 4) | (in->linebuf_depth & 0xF);
    pps[4] = (in->block_pred_enable << 5) | (in->convert_rgb << 4) |
             (in->simple_422 << 3) | (in->vbr_enable << 2) |
             MSB(in->bits_per_pixel & 0x3FF);
    pps[5] = LSB(in->bits_per_pixel);
    pps[6] = MSB(in->pic_height);
    pps[7] = LSB(in->pic_height);
    pps[8] = MSB(in->pic_width);
    pps[9] = LSB(in->pic_width);
    pps[10] = MSB(in->slice_height);
    pps[11] = LSB(in->slice_height);
    pps[12] = MSB(in->slice_width);
    pps[13] = LSB(in->slice_width);
    pps[14] = MSB(in->chunk_size);
    pps[15] = LSB(in->chunk_size);
    pps[16] = MSB(in->initial_xmit_delay & 0x3FF);
    pps[17] = LSB(in->initial_xmit_delay);
    pps[18] = MSB(in->initial_dec_delay);
    pps[19] = LSB(in->initial_dec_delay);
    pps[20] = 0;
    pps[21] = in->initial_scale_value & 0x3F;
    pps[22] = MSB(in->scale_increment_interval);
    pps[23] = LSB(in->scale_increment_interval);
    pps[24] = MSB(in->scale_decrement_interval & 0xFFF);
    pps[25] = LSB(in->scale_decrement_interval);
    pps[26] = 0;
    pps[27] = in->first_line_bpg_offset & 0x1F;
    pps[28] = MSB(in->nfl_bpg_offset);
    pps[29] = LSB(in->nfl_bpg_offset);
    pps[30] = MSB(in->slice_bpg_offset);
    pps[31] = LSB(in->slice_bpg_offset);
    pps[32] = MSB(in->initial_offset);
    pps[33] = LSB(in->initial_offset);
    pps[34] = MSB(in->final_offset);
    pps[35] = LSB(in->final_offset);
    pps[36] = in->flatness_min_qp & 0x1F;
    pps[37] = in->flatness_max_qp & 0x1F;

    pps[38] = MSB(in->rc_model_size);
    pps[39] = LSB(in->rc_model_size);
    pps[40] = in->rc_edge_factor & 0xF;
    pps[41] = in->rc_quant_incr_limit0 & 0x1F;
    pps[42] = in->rc_quant_incr_limit1 & 0x1F;
    pps[43] = (in->rc_tgt_offset_hi << 4) | (in->rc_tgt_offset_lo & 0xF);
    for (i = 0; i < NUM_BUF_RANGES - 1; i++)
        pps[44 + i] = in->rc_buf_thresh[i] >> 6;

    for (i = 0; i < NUM_BUF_RANGES; i++)
    {
        NvU32 x = ((in->range_min_qp[i] & 0x1F) << 11) |
                ((in->range_max_qp[i] & 0x1F) << 6) |
                ((in->range_bpg_offset[i] & 0x3F)) ;
        pps[58 + i * 2] = MSB(x);
        pps[59 + i * 2] = LSB(x);
    }

    pps[88] = (in->native_420 << 1) | (in->native_422);
    pps[89] = in->second_line_bpg_offset & 0x1F;
    pps[90] = MSB(in->nsl_bpg_offset);
    pps[91] = LSB(in->nsl_bpg_offset);
    pps[92] = MSB(in->second_line_offset_adj);
    pps[93] = LSB(in->second_line_offset_adj);
    pps[94] = 0;
    pps[95] = 0;

    for (i = 0; i < 24; i++)
    {
        data[i] = ((pps[i * 4 + 0] << 0) |
            (pps[i * 4 + 1] << 8) |
            (pps[i * 4 + 2] << 16) |
            (pps[i * 4 + 3] << 24));
    }

    for(; i < 32; i++)
        data[i] = 0;
}

/*
 * @brief       Generate slice count supported mask with given slice num.
 *
 * @param[in]   slice_num             slice num for which mask needs to be  generated
 *
 * @returns     out_slice_count_mask  if successful
 *              0                     if not successful
 */
static NvU32 
DSC_SliceCountMaskforSliceNum (NvU32 slice_num)
{
    switch (slice_num)
    {
        case 1:
            return DSC_DECODER_SLICES_PER_SINK_1;
        case 2:
            return DSC_DECODER_SLICES_PER_SINK_2;
        case 4:
            return DSC_DECODER_SLICES_PER_SINK_4;
        case 6:
            return DSC_DECODER_SLICES_PER_SINK_6;
        case 8:
            return DSC_DECODER_SLICES_PER_SINK_8;
        case 10:
            return DSC_DECODER_SLICES_PER_SINK_10;
        case 12:
            return DSC_DECODER_SLICES_PER_SINK_12;
        case 16:
            return DSC_DECODER_SLICES_PER_SINK_16;
        case 20:
            return DSC_DECODER_SLICES_PER_SINK_20;
        case 24:
            return DSC_DECODER_SLICES_PER_SINK_24;
        default:
            return DSC_DECODER_SLICES_PER_SINK_INVALID;
    }
}

/*
 * @brief       Convert peak throughput placeholders into numeric values.
 *
 * @param[in]   peak_throughput_mode0   peak throughput sink cap placeholder.      
 *
 * @returns     peak_throughput_mps     actual throughput in MegaPixels/second. 
 */
static NvU32
DSC_GetPeakThroughputMps(NvU32 peak_throughput)
{
    NvU32 peak_throughput_mps;
    switch(peak_throughput)
    {
        case DSC_DECODER_PEAK_THROUGHPUT_MODE0_340:
            peak_throughput_mps = 340;
            break;
        case DSC_DECODER_PEAK_THROUGHPUT_MODE0_400:
            peak_throughput_mps = 400;
            break;
        case DSC_DECODER_PEAK_THROUGHPUT_MODE0_450:
            peak_throughput_mps = 450;
            break;
        case DSC_DECODER_PEAK_THROUGHPUT_MODE0_500:
            peak_throughput_mps = 500;
            break;
        case DSC_DECODER_PEAK_THROUGHPUT_MODE0_550:
            peak_throughput_mps = 550;
            break;
        case DSC_DECODER_PEAK_THROUGHPUT_MODE0_600:
            peak_throughput_mps = 600;
            break;
        case DSC_DECODER_PEAK_THROUGHPUT_MODE0_650:
            peak_throughput_mps = 650;
            break;
        case DSC_DECODER_PEAK_THROUGHPUT_MODE0_700:
            peak_throughput_mps = 700;
            break;
        case DSC_DECODER_PEAK_THROUGHPUT_MODE0_750:
            peak_throughput_mps = 750;
            break;
        case DSC_DECODER_PEAK_THROUGHPUT_MODE0_800:
            peak_throughput_mps = 800;
            break;
        case DSC_DECODER_PEAK_THROUGHPUT_MODE0_850:
            peak_throughput_mps = 850;
            break;
        case DSC_DECODER_PEAK_THROUGHPUT_MODE0_900:
            peak_throughput_mps = 900;
            break;
        case DSC_DECODER_PEAK_THROUGHPUT_MODE0_950:
            peak_throughput_mps = 950;
            break;
        case DSC_DECODER_PEAK_THROUGHPUT_MODE0_1000:
            peak_throughput_mps = 1000;
            break;
        case DSC_DECODER_PEAK_THROUGHPUT_MODE0_170:
            peak_throughput_mps = 170;
            break;
        default:
            peak_throughput_mps = 0;
    }
    return peak_throughput_mps;
}

/*
 * @brief       Get minimum slice count needed to support the mode.
 *
 * @param[in]   picWidth              active width of the mode. 
 * @param[in]   pixelClkMhz           pixel clock in Mhz of the mode. 
 * @param[in]   maxSliceWidth         Max slice with considering gpu and sink 
 * @param[in]   peakThroughPutMps     Max throughput supported by the sink dsc decoder. 
 * @param[in]   maxSliceCount         Max slice count considering gpu and sink
 * @param[in]   bInclusive            maximum slice count should be included in mask or not
   @param[in]   commonSliceCountMask  Slice count mask to be considered 
   @param[out]  minSliceCount         Minimum slice count to be used for the mode.
 *
 * @returns     minimum slice count to be used for the mode. 
 */
static NVT_STATUS
DSC_GetMinSliceCountForMode
(
    NvU32  picWidth,
    NvU32  pixelClkMhz,
    NvU32  maxSliceWidth,
    NvU32  peakThroughPutMps,
    NvU32  maxSliceCount,
    NvU32  commonSliceCountMask,
    NvU32 *pMinSliceCount
)
{
    NvU32 minSliceCountLocal = 0U;
    NvU32 minSliceCountPicWidth =  (picWidth + maxSliceWidth - 1) / maxSliceWidth;
    NvU32 minsliceCountThroughput = (pixelClkMhz + peakThroughPutMps - 1) / peakThroughPutMps;
    minSliceCountLocal = MAX(minSliceCountPicWidth, minsliceCountThroughput);
    if (maxSliceCount < minSliceCountLocal)
    {
        return NVT_STATUS_MIN_SLICE_COUNT_ERROR;
    }
    if ((DSC_SliceCountMaskforSliceNum(minSliceCountLocal) & commonSliceCountMask) == 0x0)
    {
        //
        // It is possible that the mininum slice count calculated from pic width and 
        // pixel clock criteria is not a valid slice count supported by both GPU and
        // sink. In those cases, we need to find next valid slice count for the 
        // combo.
        // 
        NvU32 newMinSliceCount = 0U;
        if (DSC_GetHigherSliceCount(commonSliceCountMask, minSliceCountLocal, &newMinSliceCount) != NVT_STATUS_SUCCESS)
        {
            return NVT_STATUS_MIN_SLICE_COUNT_ERROR;
        }
        minSliceCountLocal = newMinSliceCount;
    }
    *pMinSliceCount = minSliceCountLocal;
    return NVT_STATUS_SUCCESS;
}

/*
 * @brief       Get slice count mask upto max slice count.
 *
 * @param[in]   max_slice_num   max slice number to be considered while generating mask
 * @param[in]   bInclusive      maximum slice number should be included in mask or not
 *
 * @returns     slice count mask of all slice counts up to max slice count
 */
static NvU32
DSC_GetSliceCountMask
(
    NvU32  maxSliceNum,
    NvBool bInclusive
)
{
    // Below are the valid slice counts according to DP2.0 spec. 
    NvU32 validSliceNum[] = {1U,2U,4U,6U,8U,10U,12U,16U,20U,24U};
    NvU32 sliceCountMask = 0U;
    NvU32 sliceArrayCount;
    NvU32 i;

    sliceArrayCount = sizeof(validSliceNum)/sizeof(NvU32);

    if (maxSliceNum == 0U)
        return 0U;

    for(i = 0U; ((i < sliceArrayCount) && (validSliceNum[i] < maxSliceNum)); i++)
    {
        sliceCountMask |= DSC_SliceCountMaskforSliceNum(validSliceNum[i]);
    }

    if (bInclusive && (i < sliceArrayCount))
    {
        sliceCountMask |= DSC_SliceCountMaskforSliceNum(validSliceNum[i]);
    }

    return sliceCountMask;
}

/*
 * @brief       Get the next higher valid slice count.
 *
 * Note each bit position in the mask represents corresponding slice count as   
 * per validSliceNum. The function compares the bit position of the each set   
 * bits in the mask against the passed current slice count. If it finds a slice   
 * count that is more than the current slice count, that is returned as next  
 * higher slice count. 
 *
 * @param[in]   commonSliceCountMask   Includes slice counts supported by both
 *                                     GPU and sink                    
 * @param[in]   currentSliceCount      Current slice count
 * @param[in]   newSliceCount          Higher slice count if one was found. 
 *
 * @returns NVT_STATUS_SUCCESS if successful;
 *          NVT_STATUS_ERR if unsuccessful;
 */
static NvU32
DSC_GetHigherSliceCount
(
    NvU32 commonSliceCountMask, 
    NvU32 currentSliceCount,
    NvU32 *newSliceCount
)
{
    NvU32 i = 0U;
    NvU32 sliceMask = commonSliceCountMask;
    //
    // Below are the valid slice counts according to DP2.0 spec. 
    // Refer DPCD 64h & 6Dh. Note validSliceNum[2] is kept 0 to 
    // indicate DPCD 64[2] which is kept reserved according to spec.
    //
    NvU32 validSliceNum[] = {1U,2U,0U,4U,6U,8U,10U,12U,16U,20U,24U};
    NvU32 sliceArrayCount;

    sliceArrayCount = sizeof(validSliceNum)/sizeof(NvU32);

    //
    // We need to decode the slice count mask and find out if there is a slice  
    // count in the mask that is higher than the passed in currentSliceCount.
    // 
    while (sliceMask != 0U && i < sliceArrayCount)
    {
        if (sliceMask & 0x1)
        {
            if (validSliceNum[i] > currentSliceCount)
            {
                *newSliceCount = validSliceNum[i];
                return NVT_STATUS_SUCCESS;
            }
        }
        sliceMask = sliceMask >> 1;
        i++;
    }

    return NVT_STATUS_PPS_SLICE_COUNT_ERROR;
}

/*
 * @brief Function validates and calculates, if required, the slice parameters like
 * slice_width, slice_num for the DSC mode requested.
 * 
 * If slice width, slice num is not forced, fn calculates them by trying to minimize 
 * slice num used. 
 * 
 * If slice width/slice num is forced, it validates the forced parameter and calculates
 *  corresponding parameter and makes sure it can be supported.
 * 
 * If both slice num and slice width are forced, it validates both. 
 *
 * @param[in]   pixel_clkMHz       Pixel clock
 * @param[in]   dual_mode          Specify if Dual Mode is enabled or not
 * @param[in]   max_slice_num      max slice number supported by sink
 * @param[in]   max_slice_width    max slice width supported by sink
 * @param[in]   slice_count_mask   Mask of slice counts supported by sink
 * @param[in]   peak_throughput    Peak throughput supported by DSC sink 
 *                                     decoder in Mega Pixels Per Second 
 * @param[out]  out                DSC output parameter
 *
 * @returns NVT_STATUS_SUCCESS if successful;
 *          NVT_STATUS_ERR if unsuccessful;
 */
static NvU32
DSC_PpsCalcSliceParams
(
    NvU32 pixel_clkMHz,
    NvU32 dual_mode,
    NvU32 max_slice_num,
    NvU32 max_slice_width,
    NvU32 slice_count_mask,
    NvU32 peak_throughput,
    DSC_OUTPUT_PARAMS *out
)
{
    NvU32  min_slice_num;
    NvU32  slicew;
    NvU32  peak_throughput_mps;
    NvU32  common_slice_count_mask;
    NvU32  gpu_slice_count_mask;
    NVT_STATUS status;

    gpu_slice_count_mask = DSC_GetSliceCountMask(max_slice_num, NV_TRUE /*bInclusive*/);

    if (dual_mode)
    {
        //
        // Dual mode will be set until Ada which supports upto 8 slices with 2 heads
        // So minimum slice count to be used in this mode is 2 (1 slice on each head)
        // Also slice count 6 is not supported until Ada. So we need to remove both 
        // slice counts from the mask. 
        //
        gpu_slice_count_mask &= ~(DSC_SliceCountMaskforSliceNum(1) | 
                                  DSC_SliceCountMaskforSliceNum(6));
    }

    common_slice_count_mask = gpu_slice_count_mask & slice_count_mask;

    if (!common_slice_count_mask)
    {
        // DSC cannot be supported since no common supported slice count
        return NVT_STATUS_DSC_SLICE_ERROR;
    }

    peak_throughput_mps = DSC_GetPeakThroughputMps(peak_throughput);
    if (!peak_throughput_mps)
    {
        // Peak throughput cannot be zero
        return NVT_STATUS_INVALID_PEAK_THROUGHPUT;
    }

    if (out->slice_num == 0 && out->slice_width == 0)
    {
        status = DSC_GetMinSliceCountForMode(out->pic_width, pixel_clkMHz, 
                                             max_slice_width, peak_throughput_mps, 
                                             max_slice_num, 
                                             common_slice_count_mask,
                                             &min_slice_num);
        if (status != NVT_STATUS_SUCCESS)
        {
            return status;
        }

        out->slice_num = min_slice_num;
        out->slice_width = (out->pic_width + out->slice_num - 1) / out->slice_num;
    }
    else if (out->slice_num == 0)
    {
        if (out->slice_width > max_slice_width)
        {
            // Error! Calculated slice width exceeds max Supported Slice Width
            return NVT_STATUS_PPS_SLICE_WIDTH_ERROR;
        }

        out->slice_num = (out->pic_width + out->slice_width - 1) / out->slice_width;
        if (!(DSC_SliceCountMaskforSliceNum(out->slice_num) & common_slice_count_mask))
        {
            // Slice count corresponding to requested slice_width is not supported
            return NVT_STATUS_PPS_SLICE_COUNT_ERROR;
        }
    }
    else if (out->slice_width == 0)
    {
        if (!(DSC_SliceCountMaskforSliceNum(out->slice_num) & common_slice_count_mask))
        {
            // Slice count requested is not supported
            return NVT_STATUS_PPS_SLICE_COUNT_ERROR;
        }

        out->slice_width = (out->pic_width + out->slice_num - 1) / out->slice_num;

        if (out->native_420 || out->native_422)
        {
            out->slice_width = (out->slice_width+1)/2 * 2 ;
        }

        if (out->slice_width > max_slice_width)
        {
            // Slice width corresponding to the requested slice count is not supported
            return NVT_STATUS_PPS_SLICE_WIDTH_ERROR;
        }
    }
    else
    {
        if (!(DSC_SliceCountMaskforSliceNum(out->slice_num) & common_slice_count_mask))
        {
            // Requested slice count is not supported
            return NVT_STATUS_PPS_SLICE_COUNT_ERROR;
        }

        if (out->slice_width > max_slice_width)
        {
            // Requested slice width cannot be supported
            return NVT_STATUS_PPS_SLICE_WIDTH_ERROR;
        }

        if (out->slice_width != (out->pic_width + out->slice_num  - 1) / out->slice_num)
        {
            // slice_width must equal CEIL(pic_width/slice_num) 
            return NVT_STATUS_PPS_SLICE_WIDTH_ERROR;
        }
    }

    if((pixel_clkMHz / out->slice_num) > peak_throughput_mps)
    {
        // Sink DSC decoder does not support minimum throughout required for this DSC config 
        return NVT_STATUS_ERR;
    }

    if (max_slice_width < SINK_MAX_SLICE_WIDTH_DEFAULT)
    {
        // Sink has to support a max slice width of at least 2560 as per DP1.4 spec. Ignoring for now.
    }

    if (out->slice_width < 32)
    {
        // slice_width must >= 32
        return NVT_STATUS_PPS_SLICE_WIDTH_ERROR;
    }

    slicew = out->slice_width >> (out->native_420 || out->native_422);  // /2 in 4:2:0 mode
    out->groups_per_line = (slicew + PIXELS_PER_GROUP - 1) / PIXELS_PER_GROUP;
    out->chunk_size = (slicew * out->bits_per_pixel + 8 * BPP_UNIT - 1) / (8 * BPP_UNIT); // Number of bytes per chunk

    //
    // Below is not constraint of DSC module, this is RG limitation.
    // check total data packet per line from DSC to RG won't larger than pic_width
    //
    if ((out->chunk_size + 3) / 4 * out->slice_num > out->pic_width)
    {
        // Error! bpp too high, RG will overflow, normally, this error is also caused by padding
        // (pic_width<slice_width*slice_num or chunk_size%4!=0)
        return NVT_STATUS_ERR;
    }

    return NVT_STATUS_SUCCESS;
}

/*
 * @brief Check if Slice Height is valid or not
 *
 * @param[in/out]  out  DSC output parameter
 *
 * @returns NVT_STATUS_SUCCESS if successful;
 *          NVT_STATUS_ERR if unsuccessful;
 */
static NVT_STATUS
DSC_PpsCheckSliceHeight(DSC_OUTPUT_PARAMS *out)
{
    if (DSC_PpsCalcExtraBits(out) != NVT_STATUS_SUCCESS)
    {
        return NVT_STATUS_ERR;
    }

    if (DSC_PpsCalcBpg(out) != NVT_STATUS_SUCCESS) 
    {
        return NVT_STATUS_ERR;
    }
    return DSC_PpsCalcScaleInterval(out);
}

/*
 * @brief Calculate Slice Height
 *
 * @param[in/out]  out  DSC output parameter
 *
 * @returns NVT_STATUS_SUCCESS if successful;
 *          NVT_STATUS_ERR if unsuccessful;
 */
static NVT_STATUS
Dsc_PpsCalcHeight
(
const DSC_INPUT_PARAMS *in,
DSC_OUTPUT_PARAMS *out
)
{
    //
    // From Blackwell and later, eDP we will try to use smallest slice height 
    // if client has not asked for a specific slice height. Multi-tile is enabled
    // from Blackwell and so here we will use that condition for deciding on 
    // slice height calculation. Minimum slice height will help with power savings 
    // when eDP1.5 Selective Update is enabled.
    //
    if (in->multi_tile && in->eDP)
    {
        if (out->slice_height == 0U)
        {
            // Minimum area of slice should be 15000 as per VESA spec
            out->slice_height = (NvU32)NV_CEIL(15000U,(out->slice_width));
            while (out->pic_height > out->slice_height)
            {
                if (out->pic_height % out->slice_height == 0U)
                {
                    if (DSC_PpsCheckSliceHeight(out) == NVT_STATUS_SUCCESS)
                    {
                        return NVT_STATUS_SUCCESS;
                    }
                    else
                    {
                        out->slice_height++;
                    }
                }
                else
                {
                    out->slice_height++;
                }

                if (out->pic_height == out->slice_height)
                {
                    if(DSC_PpsCheckSliceHeight(out) == NVT_STATUS_SUCCESS)
                    {
                        return NVT_STATUS_SUCCESS;
                    }
                    else
                    {
                        return NVT_STATUS_PPS_SLICE_HEIGHT_ERROR;    
                    }
                }
            }
        }
    }
    else
    {
        if(out->slice_height == 0)
        {
            NvU32 i;
            for (i = 1 ; i <= 16; i++)
            {
                out->slice_height = out->pic_height / i;
                if (out->pic_height != out->slice_height * i )
                    continue;

                if (DSC_PpsCheckSliceHeight(out) == NVT_STATUS_SUCCESS)
                    return NVT_STATUS_SUCCESS;
            }
            // Error! can't find valid slice_height
            return NVT_STATUS_PPS_SLICE_HEIGHT_ERROR;
        }
    }

    RANGE_CHECK("slice_height", out->slice_height, 8, out->pic_height);

    if (out->pic_height % out->slice_height != 0)
    {
        // Error! pic_height % slice_height must be 0
        return NVT_STATUS_PPS_SLICE_HEIGHT_ERROR;
    }

    if(DSC_PpsCheckSliceHeight(out) != NVT_STATUS_SUCCESS)
    {
        // Error! slice_height not valid
        return NVT_STATUS_PPS_SLICE_HEIGHT_ERROR;
    }
    return NVT_STATUS_SUCCESS;
}

/*
 * @brief Calculate DSC_OUTPUT_PARAMS from DSC_INPUT_PARAMS.
 *
 * @param[in]   in   DSC input parameter
 * @param[out]  out  DSC output parameter
 *
 * @returns NVT_STATUS_SUCCESS if successful;
 *          NVT_STATUS_ERR if unsuccessful;
 */
static NVT_STATUS
DSC_PpsCalc
(
    const DSC_INPUT_PARAMS *in,
    DSC_OUTPUT_PARAMS *out
)
{
    NVT_STATUS ret;
    NvU32 peak_throughput = 0;

    ret = DSC_PpsCalcBase(in, out);
    if (ret != NVT_STATUS_SUCCESS)
        return ret;

    if (in->drop_mode)
    {
        // in drop mode, HW requires these params to simplify the design
        out->bits_per_pixel = 16 * BPP_UNIT;
        out->slice_num = 2;
    }

    if (out->native_420 || out->native_422)
    {
        peak_throughput = in->peak_throughput_mode1;
    }
    else
    {
        peak_throughput = in->peak_throughput_mode0;
    }

    ret = DSC_PpsCalcSliceParams(in->pixel_clkMHz, in->dual_mode, 
            in->max_slice_num, in->max_slice_width, in->slice_count_mask, 
            peak_throughput, out);
    if (ret != NVT_STATUS_SUCCESS) return ret;
    ret = DSC_PpsCalcRcInitValue(out);
    if (ret != NVT_STATUS_SUCCESS) return ret;
    ret = Dsc_PpsCalcHeight(in, out);
    if (ret != NVT_STATUS_SUCCESS) return ret;
    ret = DSC_PpsCalcRcParam(out);
    return ret;
}

/*
 * @brief Calculate DSC_OUTPUT_PARAMS from DSC_INPUT_PARAMS internally,
 *        then pack pps parameters into 32bit data array. 
 *
 * @param[in]   in   DSC input parameter
 * @param[in]   pPpsOut A preallocated work-area buffer for calculations
 * @param[out]  out  DSC output parameter
 *                   NvU32[32] to return the pps data.
 *                   The data can be send to SetDscPpsData* methods directly.
 *
 * @returns NVT_STATUS_SUCCESS if successful;
 *          NVT_STATUS_ERR if unsuccessful;
 */
static NVT_STATUS
DSC_PpsDataGen
(
    const DSC_INPUT_PARAMS *in,
    DSC_OUTPUT_PARAMS      *pPpsOut,
    NvU32 out[DSC_MAX_PPS_SIZE_DWORD]
)
{
    NVT_STATUS ret;

    NVMISC_MEMSET(pPpsOut, 0, sizeof(DSC_OUTPUT_PARAMS));
    ret = DSC_PpsCalc(in, pPpsOut);
    if (ret != NVT_STATUS_SUCCESS)
    {
        goto done;
    }

    DSC_PpsConstruct(pPpsOut, out);

    /* fall through */
done:

    return ret;
}

/*
 * @brief Validate input parameter we got from caller of this function
 *
 * @param[in]   pDscInfo       Includes Sink and GPU DSC capabilities
 * @param[in]   pModesetInfo   Modeset related information
 * @param[in]   pWARData       Data required for providing WAR for issues
 * @param[in]   availableBandwidthBitsPerSecond      Available bandwidth for video
 *                                                   transmission(After FEC/Downspread overhead consideration)
 *
 * @returns NVT_STATUS_SUCCESS if successful;
 *          NVT_STATUS_ERR if unsuccessful;
 */
static NVT_STATUS
_validateInput
(
    const DSC_INFO *pDscInfo,
    const MODESET_INFO *pModesetInfo,
    const WAR_DATA *pWARData,
    NvU64 availableBandwidthBitsPerSecond
)
{
    // Validate DSC Info
    if (pDscInfo->sinkCaps.decoderColorFormatMask == 0U)
    {
        // ERROR - At least one of the color format decoding needs to be supported by Sink.
        return NVT_STATUS_INVALID_PARAMETER;
    }

    if (!ONEBITSET(pDscInfo->sinkCaps.bitsPerPixelPrecision))
    {
        // ERROR - Only one of Bits Per Pixel Precision should be set
        return NVT_STATUS_INVALID_PARAMETER;
    }

    if ((pDscInfo->sinkCaps.bitsPerPixelPrecision != 1U) &&
        (pDscInfo->sinkCaps.bitsPerPixelPrecision != 2U) &&
        (pDscInfo->sinkCaps.bitsPerPixelPrecision != 4U) &&
        (pDscInfo->sinkCaps.bitsPerPixelPrecision != 8U) &&
        (pDscInfo->sinkCaps.bitsPerPixelPrecision != 16U))
    {
        // ERROR - Bits Per Pixel Precision should be 1/16, 1/8, 1/4, 1/2 or 1 bpp.
        return NVT_STATUS_INVALID_PARAMETER;
    }

    if (pDscInfo->sinkCaps.maxSliceWidth == 0U)
    {
        // ERROR - Invalid max slice width supported by sink.
        return NVT_STATUS_INVALID_PARAMETER;
    }

    if (pDscInfo->sinkCaps.maxNumHztSlices == 0U)
    {
        // ERROR - Invalid max number of horizontal slices supported by sink.
        return NVT_STATUS_INVALID_PARAMETER;
    }

    if (pDscInfo->sinkCaps.lineBufferBitDepth == 0U)
    {
        // ERROR - Invalid line buffer bit depth supported by sink.
        return NVT_STATUS_INVALID_PARAMETER;
    }

    if (pDscInfo->sinkCaps.algorithmRevision.versionMinor == 0U)
    {
        // ERROR - Invalid DSC algorithm revision supported by sink.
        return NVT_STATUS_INVALID_PARAMETER;
    }

    if (pDscInfo->gpuCaps.encoderColorFormatMask == 0U)
    {
        // ERROR - At least one of the color format encoding needs to be supported by GPU.
        return NVT_STATUS_INVALID_PARAMETER;
    }

    if (pDscInfo->gpuCaps.lineBufferSize == 0U)
    {
        // ERROR - Invalid Line buffer size supported by GPU.
        return NVT_STATUS_INVALID_PARAMETER;
    }

    if (pDscInfo->gpuCaps.maxNumHztSlices == 0U)
    {
        // ERROR - Invalid max number of horizontal slices supported by GPU.
        return NVT_STATUS_INVALID_PARAMETER;
    }

    if (pDscInfo->gpuCaps.lineBufferBitDepth == 0U)
    {
        // ERROR - Invalid line buffer bit depth supported by GPU.
        return NVT_STATUS_INVALID_PARAMETER;
    }

    if (pDscInfo->forcedDscParams.sliceCount > pDscInfo->sinkCaps.maxNumHztSlices)
    {
        // ERROR - Client can't specify forced slice count greater than what sink supports.
        return NVT_STATUS_DSC_SLICE_ERROR;
    }

    if ((pDscInfo->forcedDscParams.sliceCount / (pModesetInfo->bDualMode ? 2 : 1)) > pDscInfo->gpuCaps.maxNumHztSlices)
    {
        // ERROR - Client can't specify forced slice count greater than what GPU supports.
        return NVT_STATUS_DSC_SLICE_ERROR;
    }

    if (pDscInfo->forcedDscParams.sliceWidth > pDscInfo->sinkCaps.maxSliceWidth)
    {
        // ERROR - Client can't specify forced slice width greater than what sink supports.
        return NVT_STATUS_DSC_SLICE_ERROR;
    }

    if ((pDscInfo->forcedDscParams.sliceCount > 0U) &&
        (pDscInfo->forcedDscParams.sliceWidth != 0U))
    {
        // ERROR - Client can't specify both forced slice count and slice width.
        return NVT_STATUS_DSC_SLICE_ERROR;
    }

    if ((pDscInfo->forcedDscParams.sliceCount != 0U) &&
        (pDscInfo->forcedDscParams.sliceCount != 1U) &&
        (pDscInfo->forcedDscParams.sliceCount != 2U) &&
        (pDscInfo->forcedDscParams.sliceCount != 4U) &&
        (pDscInfo->forcedDscParams.sliceCount != 8U) && 
        (pDscInfo->forcedDscParams.sliceCount != 10U) &&
        (pDscInfo->forcedDscParams.sliceCount != 12U) &&
        (pDscInfo->forcedDscParams.sliceCount != 16U) &&
        (pDscInfo->forcedDscParams.sliceCount != 20U) &&
        (pDscInfo->forcedDscParams.sliceCount != 24U))
    {
        // ERROR - Forced Slice Count has to be 1/2/4/8/10/12/16/20/24.
        return NVT_STATUS_DSC_SLICE_ERROR;
    }

    if (pDscInfo->forcedDscParams.sliceWidth > pModesetInfo->activeWidth)
    {
        // ERROR - Forced Slice Width can't be more than Active Width.
        return NVT_STATUS_DSC_SLICE_ERROR;
    }

    if (pDscInfo->forcedDscParams.sliceHeight > pModesetInfo->activeHeight)
    {
        // ERROR - Forced Slice Height can't be more than Active Height.
        return NVT_STATUS_DSC_SLICE_ERROR;
    }

    if (pDscInfo->forcedDscParams.dscRevision.versionMinor > 
        pDscInfo->sinkCaps.algorithmRevision.versionMinor)
    {
        // ERROR - Forced DSC Algorithm Revision is greater than Sink Supported value.
        return NVT_STATUS_INVALID_PARAMETER;
    }

    if (pDscInfo->forcedDscParams.dscRevision.versionMinor > 2U)
    {
        // ERROR - Forced DSC Algorithm Revision is greater than 1.2
        return NVT_STATUS_INVALID_PARAMETER;
    }

    if (pModesetInfo->pixelClockHz == 0U)
    {
        // ERROR - Invalid pixel Clock for mode.
        return NVT_STATUS_INVALID_PARAMETER;
    }

    if ((pDscInfo->branchCaps.overallThroughputMode0 != 0U) && 
        (pModesetInfo->pixelClockHz > pDscInfo->branchCaps.overallThroughputMode0 * MHZ_TO_HZ))
    {
        // ERROR - Pixel clock cannot be greater than Branch DSC Overall Throughput Mode 0
        return NVT_STATUS_OVERALL_THROUGHPUT_ERROR;
    }

    if (pModesetInfo->activeWidth == 0U)
    {
        // ERROR - Invalid active width for mode.
        return NVT_STATUS_INVALID_PARAMETER;
    }

    if (pDscInfo->branchCaps.maxLineBufferWidth != 0U &&
        pModesetInfo->activeWidth > pDscInfo->branchCaps.maxLineBufferWidth)
    {
        // ERROR - Active width cannot be greater than DSC Decompressor max line buffer width
        return NVT_STATUS_MAX_LINE_BUFFER_ERROR;
    }

    if (pModesetInfo->activeHeight == 0U)
    {
        // ERROR - Invalid active height for mode.
        return NVT_STATUS_INVALID_PARAMETER;
    }

    if (pModesetInfo->bitsPerComponent == 0U)
    {
        // ERROR - Invalid bits per component for mode.
        return NVT_STATUS_INVALID_PARAMETER;
    }

    if (availableBandwidthBitsPerSecond == 0U)
    {
        // ERROR - Invalid available bandwidth in Bits Per Second.
        return NVT_STATUS_INVALID_PARAMETER;
    }

    if (pModesetInfo->colorFormat == NVT_COLOR_FORMAT_YCbCr422)
    {
        //
        // For using YCbCr422 with DSC, either of the following has to be true
        //      1> Sink supports Simple422
        //      2> GPU and Sink supports Native 422
        //
        if ((!(pDscInfo->sinkCaps.decoderColorFormatMask & DSC_DECODER_COLOR_FORMAT_Y_CB_CR_SIMPLE_422)) && 
            (!((pDscInfo->gpuCaps.encoderColorFormatMask & DSC_ENCODER_COLOR_FORMAT_Y_CB_CR_NATIVE_422) &&
                (pDscInfo->sinkCaps.decoderColorFormatMask & DSC_DECODER_COLOR_FORMAT_Y_CB_CR_NATIVE_422))))
        {
            // ERROR - Can't enable YCbCr422 with current GPU and Sink DSC config.
            return NVT_STATUS_COLOR_FORMAT_NOT_SUPPORTED;
        }
    }

    if (pModesetInfo->colorFormat == NVT_COLOR_FORMAT_YCbCr420)
    {
        //
        // For using YCbCr420 with DSC, GPU and Sink has to support Native 420
        //
        if (!((pDscInfo->gpuCaps.encoderColorFormatMask & DSC_ENCODER_COLOR_FORMAT_Y_CB_CR_NATIVE_420) &&
            (pDscInfo->sinkCaps.decoderColorFormatMask & DSC_DECODER_COLOR_FORMAT_Y_CB_CR_NATIVE_420)))
        {
            // ERROR - Can't enable YCbCr420 with current GPU and Sink DSC config.
            return NVT_STATUS_COLOR_FORMAT_NOT_SUPPORTED;
        }
    }

    if ((pDscInfo->sinkCaps.algorithmRevision.versionMajor == 1U) &&
        (pDscInfo->sinkCaps.algorithmRevision.versionMinor == 1U) &&
        (pModesetInfo->colorFormat == NVT_COLOR_FORMAT_YCbCr420))
    {
        // WARNING: DSC v1.2 or higher is recommended for using YUV444
        // Current version is 1.1
    }

    if (pDscInfo->sinkCaps.maxBitsPerPixelX16 > 1024U)
    {
            // ERROR - Max bits per pixel can't be greater than 1024
            return NVT_STATUS_INVALID_PARAMETER;
    }

    if (pDscInfo->sinkCaps.decoderColorDepthMask)
    {
        switch (pModesetInfo->bitsPerComponent)
        {
        case 12:
            if (!(pDscInfo->sinkCaps.decoderColorDepthMask & DSC_DECODER_COLOR_DEPTH_CAPS_12_BITS))
            {
                // ERROR - Sink DSC Decoder does not support 12 bpc
                return NVT_STATUS_INVALID_BPC;
            }
            break;
        case 10:
            if (!(pDscInfo->sinkCaps.decoderColorDepthMask & DSC_DECODER_COLOR_DEPTH_CAPS_10_BITS))
            {
                // ERROR - Sink DSC Decoder does not support 10 bpc
                return NVT_STATUS_INVALID_BPC;
            }
            break;
        case 8:
            if (!(pDscInfo->sinkCaps.decoderColorDepthMask & DSC_DECODER_COLOR_DEPTH_CAPS_8_BITS))
            {
                // ERROR - Sink DSC Decoder does not support 8 bpc
                return NVT_STATUS_INVALID_BPC;
            }
            break;

        default:
            // ERROR - Invalid bits per component specified
            return NVT_STATUS_INVALID_PARAMETER;
        }
    }
    else
    {
        // WARNING - Decoder Color Depth Mask was not provided. Assuming that decoder supports all depths.
    }

    // Validate WAR data
    if (pWARData)
    {
        if ((pWARData->connectorType != DSC_DP) && (pWARData->connectorType != DSC_HDMI))
        {
            // WARNING - Incorrect connector info sent with WAR data
            return NVT_STATUS_INVALID_PARAMETER;
        }

        if (pWARData->connectorType == DSC_DP)
        {
            if (!IS_VALID_LANECOUNT(pWARData->dpData.laneCount))
            {
                // ERROR - Incorrect DP Lane count info sent with WAR data
                return NVT_STATUS_INVALID_PARAMETER;
            }

            if (!IS_VALID_DP2_X_LINKBW(pWARData->dpData.linkRateHz))
            {
                // ERROR - Incorrect DP Link rate info sent with WAR data
                return NVT_STATUS_INVALID_PARAMETER;
            }

            if (pWARData->dpData.hBlank > MAX_HBLANK_PIXELS)
            {
                // ERROR - Incorrect DP HBlank info sent with WAR data
                return NVT_STATUS_INVALID_HBLANK;
            }

            if ((pWARData->dpData.dpMode != DSC_DP_SST) && (pWARData->dpData.dpMode != DSC_DP_MST))
            {
                // ERROR - Incorrect DP Stream mode sent with WAR data
                return NVT_STATUS_INVALID_PARAMETER;
            }
        }
    }

    return NVT_STATUS_SUCCESS;
}

/* ------------------------ Public Functions ------------------------------- */

/*
 * @brief       Calculate PPS parameters and slice count mask based on passed down 
 *              Sink, GPU capability and modeset info
 *
 *
 * @param[in]   pDscInfo       Includes Sink and GPU DSC capabilities
 * @param[in]   pModesetInfo   Modeset related information
 * @param[in]   pWARData       Data required for providing WAR for issues
 * @param[in]   availableBandwidthBitsPerSecond      Available bandwidth for video
 *                                                   transmission(After FEC/Downspread overhead consideration)
 * @param[out]  pps                 Calculated PPS parameter.
 *                                  The data can be sent to SetDscPpsData* methods directly.
 * @param[out]  pBitsPerPixelX16    Bits per pixel multiplied by 16
 * @param[out]  pSliceCountMask     Mask of all slice counts supported by the mode.
 *
 * @returns NVT_STATUS_SUCCESS if successful;
 *          NVT_STATUS_DSC_SLICE_ERROR if no common slice count could be found;
 *          NVT_STATUS_INVALID_PEAK_THROUGHPUT if peak through put is invalid;
 *          NVT_STATUS_PPS_SLICE_COUNT_ERROR if there is no slice count possible for the mode.
 *          In case this returns failure consider that PPS is not possible.
 */
NVT_STATUS
DSC_GeneratePPSWithSliceCountMask
(
    const DSC_INFO *pDscInfo,
    const MODESET_INFO *pModesetInfo,
    const WAR_DATA *pWARData,
    NvU64 availableBandwidthBitsPerSecond,
    NvU32 pps[DSC_MAX_PPS_SIZE_DWORD],
    NvU32 *pBitsPerPixelX16,
    NvU32 *pSliceCountMask
)
{
    NvU32 commonSliceCountMask;
    NvU32 gpuSliceCountMask;
    NvU32 rejectSliceCountMask;
    NvU32 possibleSliceCountMask;
    NvU32 validSliceCountMask = 0x0;
    NvU32 peakThroughPutIndex = 0U;
    NvU32 peakThroughPutMps = 0U;
    NvU32 maxSliceCount;
    NvU32 maxSliceWidth;
    NvU32 minSliceCount;
    NvU32 sliceArrayCount;
    NvU32 i;
    DSC_INFO localDscInfo;
    NVT_STATUS status;
    DSC_GENERATE_PPS_OPAQUE_WORKAREA scratchBuffer;

    // Below are the valid slice counts according to DP2.0 spec. 
    NvU32 validSliceNum[] = {1U,2U,4U,6U,8U,10U,12U,16U,20U,24U};

    // if any slice parameters are forced, just return PPS.
    if (pDscInfo->forcedDscParams.sliceWidth != 0U || 
        pDscInfo->forcedDscParams.sliceCount != 0U)
    {
        return DSC_GeneratePPS(pDscInfo, pModesetInfo, pWARData, 
                               availableBandwidthBitsPerSecond,  
                               &scratchBuffer, pps, pBitsPerPixelX16);
    }

    sliceArrayCount = sizeof(validSliceNum)/sizeof(NvU32);

    // For 2Head1OR mode, slice count supported by GPU is always 8. 
    maxSliceCount = MIN(pDscInfo->sinkCaps.maxNumHztSlices, 
                        pModesetInfo->bDualMode ? 8U : pDscInfo->gpuCaps.maxNumHztSlices);

    // lineBufferSize is reported in 1024 units by HW, so need to multiply by 1024 to get pixels.
    maxSliceWidth = MIN(pDscInfo->sinkCaps.maxSliceWidth, pDscInfo->gpuCaps.lineBufferSize * 1024);

    gpuSliceCountMask = DSC_GetSliceCountMask(maxSliceCount, NV_TRUE /*bInclusive*/);

    if (pModesetInfo->bDualMode)
    {
        // For DSC_DUAL, slice counts 1 and 6 are invalid. 
        gpuSliceCountMask &= ~(0x11);
    }

    commonSliceCountMask = gpuSliceCountMask & pDscInfo->sinkCaps.sliceCountSupportedMask;

    if (commonSliceCountMask == 0x0)
    {
        return NVT_STATUS_DSC_SLICE_ERROR;
    }

    if ((pModesetInfo->colorFormat == NVT_COLOR_FORMAT_YCbCr422 &&
        ((pDscInfo->gpuCaps.encoderColorFormatMask & DSC_ENCODER_COLOR_FORMAT_Y_CB_CR_NATIVE_422) &&
         (pDscInfo->sinkCaps.decoderColorFormatMask & DSC_DECODER_COLOR_FORMAT_Y_CB_CR_NATIVE_422))) ||
        (pModesetInfo->colorFormat == NVT_COLOR_FORMAT_YCbCr420 &&
        ((pDscInfo->gpuCaps.encoderColorFormatMask & DSC_ENCODER_COLOR_FORMAT_Y_CB_CR_NATIVE_420) &&
         (pDscInfo->sinkCaps.decoderColorFormatMask & DSC_DECODER_COLOR_FORMAT_Y_CB_CR_NATIVE_420))))
    {
        peakThroughPutIndex = pDscInfo->sinkCaps.peakThroughputMode1;
    }
    else
    {
        peakThroughPutIndex = pDscInfo->sinkCaps.peakThroughputMode0;
    }

    peakThroughPutMps = DSC_GetPeakThroughputMps(peakThroughPutIndex);
    if (peakThroughPutMps == 0U)
    {
        return NVT_STATUS_INVALID_PEAK_THROUGHPUT;
    }

    status = DSC_GetMinSliceCountForMode(pModesetInfo->activeWidth, 
                                         (NvU32)(pModesetInfo->pixelClockHz / 1000000L),
                                         maxSliceWidth,
                                         peakThroughPutMps,
                                         maxSliceCount,
                                         commonSliceCountMask,
                                         &minSliceCount);
    if (status != NVT_STATUS_SUCCESS)
        return status;

    // Find mask of slice counts which are less than min slice count 
    rejectSliceCountMask   = DSC_GetSliceCountMask(minSliceCount, NV_FALSE /*bInclusive*/);

    // Now find mask of slice counts that can be supported by the mode
    possibleSliceCountMask = commonSliceCountMask & (~rejectSliceCountMask);

    //
    // If we have mask of all possible slice counts, loop to generate PPS with 
    // each of those slice counts forced.
    //
    if (possibleSliceCountMask)
    {
        NvU32 minSliceCountOut = 0;
        localDscInfo = *pDscInfo;

        for(i = 0U ; i < sliceArrayCount; i++)
        {
            if (possibleSliceCountMask & DSC_SliceCountMaskforSliceNum(validSliceNum[i]))
            {
                // Use the forced bits per pixel, if any
                NvU32 bitsPerPixelX16Local = *pBitsPerPixelX16;
                localDscInfo.forcedDscParams.sliceCount = validSliceNum[i];
                status = DSC_GeneratePPS(&localDscInfo, pModesetInfo, pWARData, 
                                         availableBandwidthBitsPerSecond, &scratchBuffer,
                                         NULL, &bitsPerPixelX16Local);
                if (status == NVT_STATUS_SUCCESS)
                {
                    //
                    // DPlib and PPSlib follows DP spec to set slice count indices  
                    // in slice count mask. This mapping of index to slice count 
                    // is not 1:1. For eg. slice count 8 corresponds to bit
                    // index 5 as per spec. PPSLib clients are spec agnostic  
                    // and prefer indices to indicate corresponding slice count. 
                    // For eg. slice count = 8 should be set at bit index 7. 
                    // So while passing the mask back to clients, here we set
                    //  corresponding bit index.
                    // 
                    validSliceCountMask |= NVBIT32((validSliceNum[i]) - 1U);
                    if ((minSliceCountOut == 0) || (minSliceCountOut > validSliceNum[i]))
                    {
                        minSliceCountOut = validSliceNum[i];
                    }
                }
            }
        }

        if (minSliceCountOut != 0)
        {
            //
            // We need to return PPS with minimum slice count if client
            // has not forced any slice count even though we generate
            // pps with all other possible slice counts to validate them.
            //
            localDscInfo.forcedDscParams.sliceCount = minSliceCountOut;
            status = DSC_GeneratePPS(&localDscInfo, pModesetInfo, pWARData,
                                     availableBandwidthBitsPerSecond, &scratchBuffer,
                                     pps, pBitsPerPixelX16);
            if (status != NVT_STATUS_SUCCESS)
            {
                return status;
            }
        }
    }
    else
    {
        return NVT_STATUS_PPS_SLICE_COUNT_ERROR;
    }

    if (validSliceCountMask == 0U)
    {
        // Reason for failure with hightest possible slice count will be returned. 
        return status;
    }

    *pSliceCountMask = validSliceCountMask;

    return NVT_STATUS_SUCCESS;
}

static NvU32
_calculateEffectiveBppForDSC
(
    const DSC_INFO *pDscInfo,
    const MODESET_INFO *pModesetInfo,
    const WAR_DATA *pWARData,
    NvU32 bpp
)
{
    NvU32 LogicLaneCount, BytePerLogicLane;
    NvU32 BitPerSymbol;
    NvU32 slicewidth, chunkSize, sliceCount;
    NvU32 chunkSymbols, totalSymbolsPerLane;
    NvU32 totalSymbols;

    if (pWARData->dpData.bIs128b132bChannelCoding)
    {
        LogicLaneCount   = 4U;
        BytePerLogicLane = 4U;
        BitPerSymbol     = 32U;
    }
    else if (pWARData->dpData.dpMode == DSC_DP_MST)
    {
        LogicLaneCount   = 4U;
        BytePerLogicLane = 1U;
        BitPerSymbol     = 8U;
    }
    else
    {
        LogicLaneCount   = pWARData->dpData.laneCount;
        BytePerLogicLane = 1U;
        BitPerSymbol     = 8U;
    }

    //
    // Whenever this function is called, we either have forced slice
    // width or slice count.
    //
    if (pDscInfo->forcedDscParams.sliceWidth > 0U)
    {
        slicewidth = pDscInfo->forcedDscParams.sliceWidth;
        sliceCount = (NvU32)NV_CEIL(pModesetInfo->activeWidth, pDscInfo->forcedDscParams.sliceWidth);
    }
    else
    {
        slicewidth = (NvU32)NV_CEIL(pModesetInfo->activeWidth, pDscInfo->forcedDscParams.sliceCount);
        sliceCount = pDscInfo->forcedDscParams.sliceCount;
    }

    chunkSize           = (NvU32)NV_CEIL((bpp*slicewidth), (8U * BPP_UNIT));

    chunkSymbols        = (NvU32)NV_CEIL(chunkSize,(LogicLaneCount*BytePerLogicLane));
    totalSymbolsPerLane = (chunkSymbols+1)*(sliceCount);
    totalSymbols        = totalSymbolsPerLane*LogicLaneCount;
    bpp                 = (NvU32)NV_CEIL((totalSymbols*BitPerSymbol*BPP_UNIT),pModesetInfo->activeWidth);

    return bpp;
}

/*
 * @brief Calculate PPS parameters based on passed down Sink,
 *        GPU capability and modeset info
 *
 * @param[in]   pDscInfo       Includes Sink and GPU DSC capabilities
 * @param[in]   pModesetInfo   Modeset related information
 * @param[in]   pWARData       Data required for providing WAR for issues
 * @param[in]   availableBandwidthBitsPerSecond      Available bandwidth for video
 *                                                   transmission(After FEC/Downspread overhead consideration)
 * @param[in]   pOpaqueWorkarea  Scratch buffer of sufficient size pre-allocated
                                 by client for DSC PPS calculations internal use
 * @param[out]  pps                 Calculated PPS parameter.
 *                                  The data can be send to SetDscPpsData* methods directly.
 * @param[out]  pBitsPerPixelX16    Bits per pixel multiplied by 16
 *
 * @returns NVT_STATUS_SUCCESS if successful;
 *          NVT_STATUS_ERR if unsuccessful;
 *          In case this returns failure consider that PPS is not possible.
 */
NVT_STATUS
DSC_GeneratePPS
(
    const DSC_INFO *pDscInfo,
    const MODESET_INFO *pModesetInfo,
    const WAR_DATA *pWARData,
    NvU64 availableBandwidthBitsPerSecond,
    DSC_GENERATE_PPS_OPAQUE_WORKAREA *pOpaqueWorkarea,
    NvU32 pps[DSC_MAX_PPS_SIZE_DWORD],
    NvU32 *pBitsPerPixelX16
)
{
    DSC_INPUT_PARAMS  *in  = NULL;
    DSC_OUTPUT_PARAMS *out = NULL;
    DSC_GENERATE_PPS_WORKAREA *pWorkarea = NULL;
    NVT_STATUS ret = NVT_STATUS_ERR;
    NvU32 eff_bpp = 0U;

    if ((!pDscInfo) || (!pModesetInfo) || (!pOpaqueWorkarea) || (!pBitsPerPixelX16))
    {
        ret = NVT_STATUS_INVALID_PARAMETER;
        goto done;
    }

    pWorkarea = (DSC_GENERATE_PPS_WORKAREA*)(pOpaqueWorkarea);
    in  = &pWorkarea->in;
    out = &pWorkarea->out;

    ret = _validateInput(pDscInfo, pModesetInfo, pWARData, availableBandwidthBitsPerSecond);
    if (ret != NVT_STATUS_SUCCESS)
    {
        goto done;
    }

    NVMISC_MEMSET(in, 0, sizeof(DSC_INPUT_PARAMS));

    in->bits_per_component   = pModesetInfo->bitsPerComponent;
    in->linebuf_depth        = MIN((pDscInfo->sinkCaps.lineBufferBitDepth), (pDscInfo->gpuCaps.lineBufferBitDepth));
    in->block_pred_enable    = pDscInfo->sinkCaps.bBlockPrediction;
    in->multi_tile           = (pDscInfo->gpuCaps.maxNumHztSlices > 4U) ? 1 : 0;

    switch (pModesetInfo->colorFormat)
    {
    case NVT_COLOR_FORMAT_RGB:
        in->convert_rgb = 1;
        break;

    case NVT_COLOR_FORMAT_YCbCr444:
        in->convert_rgb = 0;
        break;
    case NVT_COLOR_FORMAT_YCbCr422:
        in->convert_rgb = 0;

        if ((pDscInfo->gpuCaps.encoderColorFormatMask & DSC_ENCODER_COLOR_FORMAT_Y_CB_CR_NATIVE_422) &&
            (pDscInfo->sinkCaps.decoderColorFormatMask & DSC_DECODER_COLOR_FORMAT_Y_CB_CR_NATIVE_422))
        {
            in->native_422 = 1;
        }
        else if (pDscInfo->sinkCaps.decoderColorFormatMask & DSC_DECODER_COLOR_FORMAT_Y_CB_CR_SIMPLE_422)
        {
            in->simple_422 = 1;
        }
        else
        {
            // ERROR - YCbCr422 is not possible with current config.
            ret = NVT_STATUS_COLOR_FORMAT_NOT_SUPPORTED;
            goto done;
        }
        break;
    case NVT_COLOR_FORMAT_YCbCr420:
        in->convert_rgb = 0;

        if ((pDscInfo->gpuCaps.encoderColorFormatMask & DSC_ENCODER_COLOR_FORMAT_Y_CB_CR_NATIVE_420) &&
            (pDscInfo->sinkCaps.decoderColorFormatMask & DSC_DECODER_COLOR_FORMAT_Y_CB_CR_NATIVE_420))
        {
            in->native_420 = 1;
        }
        else
        {
            // ERROR - YCbCr420 is not possible with current config.
            ret = NVT_STATUS_COLOR_FORMAT_NOT_SUPPORTED;
            goto done;
        }
        break;

    default:
        // ERROR - Invalid color Format specified.
        ret = NVT_STATUS_COLOR_FORMAT_NOT_SUPPORTED;
        goto done;
    }

    // calculate max possible bits per pixel allowed by the available bandwidth
    in->bits_per_pixel = (NvU32)((availableBandwidthBitsPerSecond * BPP_UNIT) / pModesetInfo->pixelClockHz);

    if (pWARData && (pWARData->connectorType == DSC_DP))
    {

        if(!in->multi_tile || 
           (in->multi_tile && pWARData->dpData.dpMode == DSC_DP_SST
           && !pWARData->dpData.bIs128b132bChannelCoding
           && pWARData->dpData.bDisableEffBppSST8b10b
          ))
        {
            //
            // In DP case, being too close to the available bandwidth caused HW to hang. 
            // 2 is subtracted based on issues seen in DP CTS testing. Refer to bug 200406501, comment 76
            // This limitation is only on DP, not needed for HDMI DSC HW
            //
            in->bits_per_pixel = (NvU32)((availableBandwidthBitsPerSecond * BPP_UNIT) / pModesetInfo->pixelClockHz) - (BPP_UNIT/8);

            if (pWARData->dpData.laneCount == 1U)
            {
                //
                // SOR lane fifo might get overflown when DP 1 lane, FEC enabled and pclk*bpp > 96%*linkclk*8 i.e.
                // DSC stream is consuming more than 96% of the total bandwidth. Use lower bits per pixel. Refer Bug 200561864.
                //
                in->bits_per_pixel = (NvU32)((96U * availableBandwidthBitsPerSecond * BPP_UNIT) / (100U * pModesetInfo->pixelClockHz)) -
                                     (BPP_UNIT / 8U);
            }
        }

        if ((pWARData->dpData.dpMode == DSC_DP_SST) && (pWARData->dpData.hBlank < 100U))
        {
            //
            // For short HBlank timing, using bits per pixel value which may have to add DSC padding for each chunk
            // may not be possible so use bits per pixel value which won't require DSC padding. Bug 200628516
            //

            NvU32 protocolOverhead;
            NvU32 dscOverhead;
            NvU32 minSliceCount = (NvU32)NV_CEIL(pModesetInfo->pixelClockHz, (MAX_PCLK_PER_SLICE_KHZ * 1000U)); 
            NvU32 sliceWidth;
            NvU32 i;
            NvU64 dataRate;

            if ((minSliceCount > 2U) &&(minSliceCount < 4U))
            {
                minSliceCount = 4U;
            }
            else if (minSliceCount > 4U)
            {
                minSliceCount = 8U;
            }
            
            sliceWidth = (NvU32)NV_CEIL(pModesetInfo->activeWidth, minSliceCount);

            if (pWARData->dpData.laneCount == 1U)
            {
                protocolOverhead = 42U;
            }
            else if (pWARData->dpData.laneCount == 2U)
            {
                protocolOverhead = 24U;
            }
            else
            {
                protocolOverhead = 21U;
            }

            dscOverhead = minSliceCount * 2U;

            if(pWARData->dpData.bIs128b132bChannelCoding)
            {
                dataRate = LINK_RATE_TO_DATA_RATE_128B_132B(pWARData->dpData.linkRateHz);
            }
            else
            {
                dataRate = LINK_RATE_TO_DATA_RATE_8B_10B(pWARData->dpData.linkRateHz);
            }
            if ((pWARData->dpData.hBlank * dataRate / pModesetInfo->pixelClockHz) <
                (protocolOverhead + dscOverhead + 3U))
            {
                //
                // For very short HBlank timing, find out bits per pixel value which will not require additional
                // DSC padding. 128 will be used as the lowest bits per pixel value.
                //
                for (i = in->bits_per_pixel; i >= MIN_BITS_PER_PIXEL * BPP_UNIT; i--)
                {
                    if (((i * sliceWidth) % ( 8U * minSliceCount * pWARData->dpData.laneCount * 16U)) == 0U)
                    {
                        break;
                    }
                }
                in->bits_per_pixel = i;
            }
        }
        in->eDP = (pWARData->dpData.bIsEdp == NV_TRUE) ? 1 : 0;
    }

    // 
    // bits per pixel upper limit is minimum of 3 times bits per component or 32
    //
    if (in->bits_per_pixel > MIN((3 * in->bits_per_component * BPP_UNIT), (MAX_BITS_PER_PIXEL * BPP_UNIT)))
    {
        in->bits_per_pixel = MIN((3 * in->bits_per_component * BPP_UNIT), (MAX_BITS_PER_PIXEL * BPP_UNIT));
    }

    in->bits_per_pixel =  DSC_AlignDownForBppPrecision(in->bits_per_pixel, pDscInfo->sinkCaps.bitsPerPixelPrecision);

    if (pWARData && (pWARData->connectorType == DSC_DP) && in->multi_tile)
    {
        //
        // EffectiveBpp should be used only from Blackwell, so keeping 
        // multi-tile check to restrict it from Blackwell.
        //
        if ((!pWARData->dpData.bDisableEffBppSST8b10b) || 
            (pWARData->dpData.bDisableEffBppSST8b10b && 
             ((pWARData->dpData.dpMode == DSC_DP_MST) || 
              pWARData->dpData.bIs128b132bChannelCoding)))
        {
            unsigned max_bpp = in->bits_per_pixel + 1;

            // Algorithm in bug 5004872
            do
            {
                max_bpp--;
                eff_bpp = _calculateEffectiveBppForDSC(pDscInfo, pModesetInfo, pWARData, max_bpp);

            } while ((eff_bpp * (pModesetInfo->pixelClockHz)) > (availableBandwidthBitsPerSecond*BPP_UNIT));

            in->bits_per_pixel = max_bpp;

            in->bits_per_pixel =  DSC_AlignDownForBppPrecision(in->bits_per_pixel, pDscInfo->sinkCaps.bitsPerPixelPrecision);            
        }
    }

    // If user specified bits_per_pixel value to be used check if it is valid one
    if (*pBitsPerPixelX16 != 0)
    {
        *pBitsPerPixelX16 = DSC_AlignDownForBppPrecision(*pBitsPerPixelX16, pDscInfo->sinkCaps.bitsPerPixelPrecision);

        //
        // The calculation of in->bits_per_pixel here in PPSlib, which is the maximum bpp that is allowed by available bandwidth, 
        // which is applicable to DP alone and not to HDMI FRL. 
        // Before calling PPS lib to generate PPS data, HDMI library has done calculation according to HDMI2.1 spec 
        // to determine if FRL rate is sufficient for the requested bpp. So restricting the condition to DP alone.
        //
        if ((pWARData && (pWARData->connectorType == DSC_DP)) &&
            (*pBitsPerPixelX16 > in->bits_per_pixel))
        {
            // ERROR - Invalid bits per pixel value specified.
            ret = NVT_STATUS_INVALID_BPP;
            goto done;
        }
        else
        {
            in->bits_per_pixel = *pBitsPerPixelX16;
        }

        if (pWARData && (pWARData->connectorType == DSC_DP) && in->multi_tile)
        {
            if ((!pWARData->dpData.bDisableEffBppSST8b10b) || 
                (pWARData->dpData.bDisableEffBppSST8b10b && 
                 ((pWARData->dpData.dpMode == DSC_DP_MST) || 
                  pWARData->dpData.bIs128b132bChannelCoding)))
            {
                eff_bpp = _calculateEffectiveBppForDSC(pDscInfo, pModesetInfo, pWARData, in->bits_per_pixel);                
            }
        }

        //
        // For DSC Dual Mode or Multi-tile configs (NVD 5.0 and later), 
        // because of architectural limitation we can't use bits_per_pixel 
        // more than 16.
        //
        if ((pModesetInfo->bDualMode ||
             (in->multi_tile && (!pWARData || (pWARData && !pWARData->dpData.bDisableDscMaxBppLimit))))
            && (in->bits_per_pixel > 256 /*bits_per_pixel = 16*/))
        {
            ret = NVT_STATUS_INVALID_BPP;
            goto done;
        }

        if ((pDscInfo->sinkCaps.maxBitsPerPixelX16 != 0) && (*pBitsPerPixelX16 > pDscInfo->sinkCaps.maxBitsPerPixelX16))
        {
            // ERROR - bits per pixel value specified by user is greater than what DSC decompressor can support.
            ret = NVT_STATUS_INVALID_BPP;
            goto done;
        }
    }
    else
    {
        //
        // For DSC Dual Mode or for multi-tile configs (NVD 5.0 and later), 
        // because of architectural limitation we can't use bits_per_pixel more 
        // than 16. So forcing it to 16.
        //
        if ((pModesetInfo->bDualMode ||
             (in->multi_tile && (!pWARData || (pWARData && !pWARData->dpData.bDisableDscMaxBppLimit))))
            && (in->bits_per_pixel > 256 /*bits_per_pixel = 16*/))
        {
            // ERROR - DSC Dual Mode, because of architectural limitation we can't use bits_per_pixel more than 16.
            // ERROR - Forcing it to 16.
            in->bits_per_pixel = 256;
        }

        // If calculated  bits_per_pixel is 126 or 127, we need to use 128 value. Bug 2686078
        if ((in->bits_per_pixel == 126) || (in->bits_per_pixel == 127))
        {
            // WARNING: bits_per_pixel is forced to 128 because calculated value was 126 or 127
            in->bits_per_pixel = 128;
        }

        if ((pDscInfo->sinkCaps.maxBitsPerPixelX16 != 0) && (in->bits_per_pixel > pDscInfo->sinkCaps.maxBitsPerPixelX16))
        {
            // WARNING - Optimal bits per pixel value calculated is greater than what DSC decompressor can support. Forcing it to max that decompressor can support
            in->bits_per_pixel = pDscInfo->sinkCaps.maxBitsPerPixelX16;
        }

        if (pWARData && (pWARData->connectorType == DSC_DP) && in->multi_tile)
        {
            if ((!pWARData->dpData.bDisableEffBppSST8b10b) || 
                (pWARData->dpData.bDisableEffBppSST8b10b && 
                 ((pWARData->dpData.dpMode == DSC_DP_MST) || 
                  pWARData->dpData.bIs128b132bChannelCoding)))
            {
                eff_bpp = _calculateEffectiveBppForDSC(pDscInfo, pModesetInfo, pWARData, in->bits_per_pixel);
            }
        }

    }

    if (pModesetInfo->bDualMode &&  (pDscInfo->gpuCaps.maxNumHztSlices > 4U))
    {
        // ERROR - Dual Mode should not be set when GPU can support more than 4 slices per head.
        ret = NVT_STATUS_INVALID_PARAMETER;
        goto done;
    }

    in->dsc_version_minor = pDscInfo->forcedDscParams.dscRevision.versionMinor ? pDscInfo->forcedDscParams.dscRevision.versionMinor :
                            pDscInfo->sinkCaps.algorithmRevision.versionMinor;
    in->pic_width = pModesetInfo->activeWidth;
    in->pic_height = pModesetInfo->activeHeight;
    in->slice_height = pDscInfo->forcedDscParams.sliceHeight;
    in->slice_width = pDscInfo->forcedDscParams.sliceWidth;
    in->slice_num = pDscInfo->forcedDscParams.sliceCount;
    in->max_slice_num = MIN(pDscInfo->sinkCaps.maxNumHztSlices,
                        pModesetInfo->bDualMode ? pDscInfo->gpuCaps.maxNumHztSlices * 2 : pDscInfo->gpuCaps.maxNumHztSlices);
    // lineBufferSize is reported in 1024 units by HW, so need to multiply by 1024 to get pixels.
    in->max_slice_width = MIN(pDscInfo->sinkCaps.maxSliceWidth, pDscInfo->gpuCaps.lineBufferSize * 1024);
    in->pixel_clkMHz = (NvU32)(pModesetInfo->pixelClockHz / 1000000L);
    in->dual_mode = pModesetInfo->bDualMode;
    in->drop_mode = pModesetInfo->bDropMode;
    in->slice_count_mask = pDscInfo->sinkCaps.sliceCountSupportedMask;
    in->peak_throughput_mode0 = pDscInfo->sinkCaps.peakThroughputMode0;
    in->peak_throughput_mode1 = pDscInfo->sinkCaps.peakThroughputMode1;
    
    if (in->native_422)
    {
        // bits_per_pixel in PPS is defined as 5 fractional bits in native422 mode
        in->bits_per_pixel *= 2;

        if (in->dsc_version_minor == 1)
        {
            // Error! DSC1.1 can't support native422!
            ret = NVT_STATUS_COLOR_FORMAT_NOT_SUPPORTED;
            goto done;
        }
        //the bpp in native 422 mode is doubled.
        if((((NvS32)(in->bits_per_pixel)) < (NvS32)(2*7*BPP_UNIT)) ||
           (((NvS32)(in->bits_per_pixel)) > (NvS32)(2*2*(in->bits_per_component)*BPP_UNIT-1)))
        {
            // ERROR - bits_per_pixelx16 outside valid range
            ret = NVT_STATUS_INVALID_BPP;
            goto done;
        }
    }
    else
    {
        if ((((NvS32)(in->bits_per_pixel)) < (NvS32)(8*BPP_UNIT)) ||
            (((NvS32)(in->bits_per_pixel)) > (NvS32)(32*BPP_UNIT)))
        {
            // ERROR - bits_per_pixelx16 outside valid range
            ret = NVT_STATUS_INVALID_BPP;
            goto done;
        }
    }

    ret = DSC_PpsDataGen(in, out, pps);

    if (in->multi_tile && eff_bpp)
    {
        *pBitsPerPixelX16 = eff_bpp;
    }
    else
    {
        *pBitsPerPixelX16 = in->bits_per_pixel;
    }

    /* fall through */
done:
    return ret;
}
