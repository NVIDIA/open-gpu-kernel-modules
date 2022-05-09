/*
 * SPDX-FileCopyrightText: Copyright (c) 2019 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
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

#if !defined(NVKMS_FORMAT_H)
#define NVKMS_FORMAT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "nvtypes.h"

/*
 * In order to interpret these pixel format namings, please take note of these
 * conventions:
 * - The Y8_U8__Y8_V8_N422 and U8_Y8__V8_Y8_N422 formats are both packed formats
 *   that have an interleaved chroma component across every two pixels. The
 *   double-underscore is a separator between these two pixel groups.
 * - The triple-underscore is a separator between planes.
 * - The 'N' suffix is a delimiter for the chroma decimation factor.
 *
 * As examples of the above rules:
 * - The Y8_U8__Y8_V8_N422 format has one 8-bit luma component (Y8) and one
 *   8-bit chroma component (U8) in pixel N, and one 8-bit luma component (Y8)
 *   and one 8-bit chroma component (V8) in pixel (N + 1). This format is
 *   422-decimated since the U and V chroma samples are shared between each
 *   pair of adjacent pixels per line.
 * - The Y10___U10V10_N444 format has one plane of 10-bit luma (Y10) components,
 *   and another plane of 10-bit chroma components (U10V10). This format has no
 *   chroma decimation since the luma and chroma components are sampled at the
 *   same rate.
 */
enum NvKmsSurfaceMemoryFormat {
    NvKmsSurfaceMemoryFormatI8               =  0,
    NvKmsSurfaceMemoryFormatA1R5G5B5         =  1,
    NvKmsSurfaceMemoryFormatX1R5G5B5         =  2,
    NvKmsSurfaceMemoryFormatR5G6B5           =  3,
    NvKmsSurfaceMemoryFormatA8R8G8B8         =  4,
    NvKmsSurfaceMemoryFormatX8R8G8B8         =  5,
    NvKmsSurfaceMemoryFormatA2B10G10R10      =  6,
    NvKmsSurfaceMemoryFormatX2B10G10R10      =  7,
    NvKmsSurfaceMemoryFormatA8B8G8R8         =  8,
    NvKmsSurfaceMemoryFormatX8B8G8R8         =  9,
    NvKmsSurfaceMemoryFormatRF16GF16BF16AF16 = 10,
    NvKmsSurfaceMemoryFormatR16G16B16A16     = 11,
    NvKmsSurfaceMemoryFormatRF32GF32BF32AF32 = 12,
    NvKmsSurfaceMemoryFormatY8_U8__Y8_V8_N422 = 13,
    NvKmsSurfaceMemoryFormatU8_Y8__V8_Y8_N422 = 14,
    NvKmsSurfaceMemoryFormatY8___U8V8_N444    = 15,
    NvKmsSurfaceMemoryFormatY8___V8U8_N444    = 16,
    NvKmsSurfaceMemoryFormatY8___U8V8_N422    = 17,
    NvKmsSurfaceMemoryFormatY8___V8U8_N422    = 18,
    NvKmsSurfaceMemoryFormatY8___U8V8_N420    = 19,
    NvKmsSurfaceMemoryFormatY8___V8U8_N420    = 20,
    NvKmsSurfaceMemoryFormatY10___U10V10_N444 = 21,
    NvKmsSurfaceMemoryFormatY10___V10U10_N444 = 22,
    NvKmsSurfaceMemoryFormatY10___U10V10_N422 = 23,
    NvKmsSurfaceMemoryFormatY10___V10U10_N422 = 24,
    NvKmsSurfaceMemoryFormatY10___U10V10_N420 = 25,
    NvKmsSurfaceMemoryFormatY10___V10U10_N420 = 26,
    NvKmsSurfaceMemoryFormatY12___U12V12_N444 = 27,
    NvKmsSurfaceMemoryFormatY12___V12U12_N444 = 28,
    NvKmsSurfaceMemoryFormatY12___U12V12_N422 = 29,
    NvKmsSurfaceMemoryFormatY12___V12U12_N422 = 30,
    NvKmsSurfaceMemoryFormatY12___U12V12_N420 = 31,
    NvKmsSurfaceMemoryFormatY12___V12U12_N420 = 32,
    NvKmsSurfaceMemoryFormatY8___U8___V8_N444 = 33,
    NvKmsSurfaceMemoryFormatY8___U8___V8_N420 = 34,
    NvKmsSurfaceMemoryFormatMin = NvKmsSurfaceMemoryFormatI8,
    NvKmsSurfaceMemoryFormatMax = NvKmsSurfaceMemoryFormatY8___U8___V8_N420,
};

typedef struct NvKmsSurfaceMemoryFormatInfo {
    enum NvKmsSurfaceMemoryFormat format;
    const char *name;
    NvU8 depth;
    NvBool isYUV;
    NvU8 numPlanes;

    union {
        struct {
            NvU8 bytesPerPixel;
            NvU8 bitsPerPixel;
        } rgb;

        struct {
            NvU8 depthPerComponent;
            NvU8 storageBitsPerComponent;
            NvU8 horizChromaDecimationFactor;
            NvU8 vertChromaDecimationFactor;
        } yuv;
    };
} NvKmsSurfaceMemoryFormatInfo;

const NvKmsSurfaceMemoryFormatInfo *nvKmsGetSurfaceMemoryFormatInfo(
    const enum NvKmsSurfaceMemoryFormat format);

const char *nvKmsSurfaceMemoryFormatToString(
    const enum NvKmsSurfaceMemoryFormat format);

#ifdef __cplusplus
};
#endif

#endif /* NVKMS_FORMAT_H */
