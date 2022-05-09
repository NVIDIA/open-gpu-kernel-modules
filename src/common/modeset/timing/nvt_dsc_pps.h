/*
 * SPDX-FileCopyrightText: Copyright (c) 2017-2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

    dsc_pps.h

    Provide definition needed for DSC(Display Stream Compression) PPS(Picture Parameter Set)

================================================================================
*/

#ifndef __DSCPPS_H__
#define __DSCPPS_H__

/* ------------------------ Includes --------------------------------------- */
#include "nvtypes.h"
#include "nvtiming.h"

/* ------------------------ Macros ----------------------------------------- */
#define DSC_MAX_PPS_SIZE_DWORD 32

/* ------------------------ Datatypes -------------------------------------- */

#define DSC_CALLBACK_MODIFIED 1

#if defined(DSC_CALLBACK_MODIFIED)
typedef struct
{
    // DSC - Callbacks
    const void* clientHandle;    // ClientHandle is only used when calling into HDMI lib's mallocCb/freeCb
    void (*dscPrint) (const char* fmtstring, ...);
    void *(*dscMalloc)(const void *clientHandle, NvLength size);
    void (*dscFree) (const void *clientHandle, void * ptr);
} DSC_CALLBACK;
#else
typedef struct
{
    // DSC - Callbacks
    void (*dscPrint) (const char* fmtstring, ...);
    void *(*dscMalloc)(NvLength size);
    void (*dscFree) (void * ptr);
} DSC_CALLBACK;
#endif // DSC_CALLBACK_MODIFIED

typedef struct
{
    NvU32 versionMajor;
    NvU32 versionMinor;
} DSC_ALGORITHM_REV;

typedef struct 
{
    NvU64 pixelClockHz;                 // Requested pixel clock for the mode
    NvU32 activeWidth;                  // Active Width
    NvU32 activeHeight;                 // Active Height
    NvU32 bitsPerComponent;             // BPC value to be used
    NVT_COLOR_FORMAT colorFormat;       // Color format to be used for this modeset

    //
    // Whether to enable Dual mode for DSC.
    // Dual mode specifies that 2 heads would be generating
    // pixels for complete stream.
    //
    NvBool bDualMode;

    //
    // Whether to enable DROP mode for DSC.
    // DROP mode specifies that instead of compressing the pixels, OR will drop
    // the pixels of the right half frame to reduce the data rate by half.
    // This mode is added for testing 2head1OR solution without a DSC panel
    //
    NvBool bDropMode;
} MODESET_INFO;

typedef struct 
{
    struct SINK_DSC_CAPS 
    {
        // Mask of all color formats for which decoding supported by panel
        NvU32 decoderColorFormatMask;
#define DSC_DECODER_COLOR_FORMAT_RGB                    (0x00000001)
#define DSC_DECODER_COLOR_FORMAT_Y_CB_CR_444            (0x00000002)
#define DSC_DECODER_COLOR_FORMAT_Y_CB_CR_SIMPLE_422     (0x00000004)
#define DSC_DECODER_COLOR_FORMAT_Y_CB_CR_NATIVE_422     (0x00000008)
#define DSC_DECODER_COLOR_FORMAT_Y_CB_CR_NATIVE_420     (0x00000010)

        // e.g. 1/16, 1/8, 1/4, 1/2, 1bpp
        NvU32 bitsPerPixelPrecision;
#define DSC_BITS_PER_PIXEL_PRECISION_1_16               (0x00000001)
#define DSC_BITS_PER_PIXEL_PRECISION_1_8                (0x00000002)
#define DSC_BITS_PER_PIXEL_PRECISION_1_4                (0x00000004)
#define DSC_BITS_PER_PIXEL_PRECISION_1_2                (0x00000008)
#define DSC_BITS_PER_PIXEL_PRECISION_1                  (0x00000010)

        // Maximum slice width supported by panel
        NvU32 maxSliceWidth;     

        // Maximum number of horizontal slices supported 
        NvU32 maxNumHztSlices;

        // Slice counts supported by the sink
        NvU32 sliceCountSupportedMask;
#define DSC_DECODER_SLICES_PER_SINK_INVALID             (0x00000000)
#define DSC_DECODER_SLICES_PER_SINK_1                   (0x00000001)
#define DSC_DECODER_SLICES_PER_SINK_2                   (0x00000002)
#define DSC_DECODER_SLICES_PER_SINK_4                   (0x00000008)
#define DSC_DECODER_SLICES_PER_SINK_6                   (0x00000010)
#define DSC_DECODER_SLICES_PER_SINK_8                   (0x00000020)
#define DSC_DECODER_SLICES_PER_SINK_10                  (0x00000040)
#define DSC_DECODER_SLICES_PER_SINK_12                  (0x00000080)
#define DSC_DECODER_SLICES_PER_SINK_16                  (0x00000100)
#define DSC_DECODER_SLICES_PER_SINK_20                  (0x00000200)
#define DSC_DECODER_SLICES_PER_SINK_24                  (0x00000400)

        //
        // Bit depth used by the Sink device to store the
        // reconstructed pixels within the line buffer
        //
        NvU32 lineBufferBitDepth;
#define DSC_DECODER_LINE_BUFFER_BIT_DEPTH_MIN           (0x00000008)
#define DSC_DECODER_LINE_BUFFER_BIT_DEPTH_MAX           (0x0000000D)

        NvU32 decoderColorDepthCaps;            // Color depth supported by DSC decoder of panel
#define DSC_DECODER_COLOR_DEPTH_CAPS_8_BITS             (0x00000001)
#define DSC_DECODER_COLOR_DEPTH_CAPS_10_BITS            (0x00000002)
#define DSC_DECODER_COLOR_DEPTH_CAPS_12_BITS            (0x00000004)
#define DSC_DECODER_COLOR_DEPTH_CAPS_16_BITS            (0x00000008)

        NvU32 decoderColorDepthMask;
        
        DSC_ALGORITHM_REV algorithmRevision;    // DSC algorithm revision that sink supports
        
        NvBool bBlockPrediction;                // Whether block prediction is supported or not.

        // Peak throughput supported for 444 and simple 422 modes
        NvU32  peakThroughputMode0;
#define DSC_DECODER_PEAK_THROUGHPUT_MODE0_INVALID       (0x00000000)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE0_340           (0x00000001)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE0_400           (0x00000002)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE0_450           (0x00000003)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE0_500           (0x00000004)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE0_550           (0x00000005)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE0_600           (0x00000006)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE0_650           (0x00000007)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE0_700           (0x00000008)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE0_750           (0x00000009)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE0_800           (0x0000000A)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE0_850           (0x0000000B)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE0_900           (0x0000000C)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE0_950           (0x0000000D)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE0_1000          (0x0000000E)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE0_170           (0x0000000F)

        // Peak throughput supported for native 422 and 420 modes
        NvU32  peakThroughputMode1;
#define DSC_DECODER_PEAK_THROUGHPUT_MODE1_INVALID       (0x00000000)   
#define DSC_DECODER_PEAK_THROUGHPUT_MODE1_340           (0x00000001)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE1_400           (0x00000002)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE1_450           (0x00000003)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE1_500           (0x00000004)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE1_550           (0x00000005)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE1_600           (0x00000006)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE1_650           (0x00000007)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE1_700           (0x00000008)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE1_750           (0x00000009)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE1_800           (0x0000000A)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE1_850           (0x0000000B)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE1_900           (0x0000000C)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE1_950           (0x0000000D)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE1_1000          (0x0000000E)
#define DSC_DECODER_PEAK_THROUGHPUT_MODE1_170           (0x0000000F)

        // Maximum bits_per_pixel supported by the DSC decompressor multiplied by 16
        NvU32  maxBitsPerPixelX16;
    }sinkCaps;

    struct GPU_DSC_CAPS
    {
        // Mask of all color formats for which encoding supported by GPU
        NvU32 encoderColorFormatMask;
#define DSC_ENCODER_COLOR_FORMAT_RGB                    (0x00000001)
#define DSC_ENCODER_COLOR_FORMAT_Y_CB_CR_444            (0x00000002)
#define DSC_ENCODER_COLOR_FORMAT_Y_CB_CR_NATIVE_422     (0x00000004)
#define DSC_ENCODER_COLOR_FORMAT_Y_CB_CR_NATIVE_420     (0x00000008)

        //
        // Size of line buffer inside DSC. Should be in number of pixels.
        // this should be greater than or equal to active width
        //
        NvU32 lineBufferSize;

        // e.g. 1/16, 1/8, 1/4, 1/2, 1bpp
        NvU32 bitsPerPixelPrecision;

        // Maximum number of horizontal slices supported 
        NvU32 maxNumHztSlices;

        //
        // Bit depth used by the GPU to store the
        // reconstructed pixels within the line buffer
        //
        NvU32 lineBufferBitDepth;
    }gpuCaps;

    struct FORCED_DSC_PARAMS
    {
        // Forced Slice count
        NvU32 sliceCount;

        // Forced Slice width
        NvU32 sliceWidth;

        // Forced Slice height
        NvU32 sliceHeight;

        // Forced DSC Algorithm Revision
        DSC_ALGORITHM_REV dscRevision;
    }forcedDscParams;
} DSC_INFO;

typedef struct 
{
    NvU32 manufacturerID;
    NvU32 productID;
    NvU32 yearWeek;
} EDID_INFO;

typedef enum
{
    DSC_DP,
    DSC_HDMI
} DSC_CONNECTOR_TYPE;

typedef enum
{
    DSC_DP_SST,
    DSC_DP_MST
} DSC_DP_MODE;

typedef struct 
{
    DSC_CONNECTOR_TYPE connectorType;
    struct DP_DATA
    {
        NvU64 linkRateHz;
        NvU32 laneCount;
        DSC_DP_MODE dpMode;
        NvU32 hBlank;
    }dpData;
} WAR_DATA;

/*
 *  Windows testbed compiles are done with warnings as errors
 *  with the maximum warning level.  Here we turn off some
 *  of the problematic warnings.
 */

/* ------------------------ Global Variables ------------------------------- */
/* ------------------------ Static Variables ------------------------------- */
/* ------------------------ Private Functions ------------------------------ */
/* ------------------------ Public Functions ------------------------------- */

#ifdef __cplusplus
extern "C" {
#endif
/*
 * @brief Initializes callbacks for print and assert
 *
 * @param[in]   callback   DSC callbacks
 *
 * @returns NVT_STATUS_SUCCESS if successful;
 *          NVT_STATUS_ERR if unsuccessful;
 */
NVT_STATUS DSC_InitializeCallback(DSC_CALLBACK callback);

/*
 * @brief Calculate PPS parameters based on passed down Sink,
 *        GPU capability and modeset info
 *
 * @param[in]   pDscInfo       Includes Sink and GPU DSC capabilities
 * @param[in]   pModesetInfo   Modeset related information
 * @param[in]   pWARData       Data required for providing WAR for issues
 * @param[in]   availableBandwidthBitsPerSecond      Available bandwidth for video
 *                                                   transmission(After FEC/Downspread overhead consideration)
 * @param[out]  pps                 Calculated PPS parameter.
 *                                  The data can be send to SetDscPpsData* methods directly.
 * @param[out]  pBitsPerPixelX16    Bits per pixel multiplied by 16
 *
 * @returns NVT_STATUS_SUCCESS if successful;
 *          NVT_STATUS_ERR if unsuccessful;
 */
NVT_STATUS DSC_GeneratePPS(const DSC_INFO *pDscInfo,
                           const MODESET_INFO *pModesetInfo,
                           const WAR_DATA *pWARData,
                           NvU64 availableBandwidthBitsPerSecond,
                           NvU32 pps[DSC_MAX_PPS_SIZE_DWORD],
                           NvU32 *pBitsPerPixelX16);

#ifdef __cplusplus
}
#endif
#endif // __DSCPPS_H__
