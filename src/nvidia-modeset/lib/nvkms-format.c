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

#include "nvkms-format.h"
#include "nv_common_utils.h"
#include "nvctassert.h"

#include <stddef.h>

#define RGB_ENTRY(_format, _depth, _bytesPerPixel) \
    [NvKmsSurfaceMemoryFormat##_format] = { \
        .format = NvKmsSurfaceMemoryFormat##_format, \
        .name = #_format, \
        .depth = _depth, \
        .isYUV = NV_FALSE, \
        .numPlanes = 1, \
        { \
            .rgb = { \
                .bytesPerPixel = _bytesPerPixel, \
                .bitsPerPixel = _bytesPerPixel * 8, \
            }, \
        }, \
    }

#define YUV_ENTRY(_format, \
                _depth, \
                _numPlanes, \
                _depthPerComponent, \
                _storageBitsPerComponent, \
                _horizChromaDecimationFactor, \
                _vertChromaDecimationFactor) \
    [NvKmsSurfaceMemoryFormat##_format] = { \
        .format = NvKmsSurfaceMemoryFormat##_format, \
        .name = #_format, \
        .depth = _depth, \
        .isYUV = NV_TRUE, \
        .numPlanes = _numPlanes, \
        { \
            .yuv = { \
                .depthPerComponent = _depthPerComponent, \
                .storageBitsPerComponent = _storageBitsPerComponent, \
                .horizChromaDecimationFactor = _horizChromaDecimationFactor, \
                .vertChromaDecimationFactor = _vertChromaDecimationFactor, \
            }, \
        }, \
    }

static const NvKmsSurfaceMemoryFormatInfo nvKmsEmptyFormatInfo;

/*
 * For 10/12-bit YUV formats, each component is packed in a 16-bit container in
 * memory, and fetched by display HW as such.
 */
static const NvKmsSurfaceMemoryFormatInfo nvKmsSurfaceMemoryFormatInfo[] = {
    RGB_ENTRY(I8, 8, 1),
    RGB_ENTRY(A1R5G5B5, 16, 2),
    RGB_ENTRY(X1R5G5B5, 15, 2),
    RGB_ENTRY(R5G6B5, 16, 2),
    RGB_ENTRY(A8R8G8B8, 32, 4),
    RGB_ENTRY(X8R8G8B8, 24, 4),
    RGB_ENTRY(A2B10G10R10, 32, 4),
    RGB_ENTRY(X2B10G10R10, 30, 4),
    RGB_ENTRY(A8B8G8R8, 32, 4),
    RGB_ENTRY(X8B8G8R8, 24, 4),
    RGB_ENTRY(RF16GF16BF16AF16, 64, 8),
    RGB_ENTRY(RF16GF16BF16XF16, 64, 8),
    RGB_ENTRY(R16G16B16A16, 64, 8),
    RGB_ENTRY(RF32GF32BF32AF32, 128, 16),
    YUV_ENTRY(Y8_U8__Y8_V8_N422, 16, 1, 8, 8, 2, 1),
    YUV_ENTRY(U8_Y8__V8_Y8_N422, 16, 1, 8, 8, 2, 1),
    YUV_ENTRY(Y8___U8V8_N444, 24, 2, 8, 8, 1, 1),
    YUV_ENTRY(Y8___V8U8_N444, 24, 2, 8, 8, 1, 1),
    YUV_ENTRY(Y8___U8V8_N422, 16, 2, 8, 8, 2, 1),
    YUV_ENTRY(Y8___V8U8_N422, 16, 2, 8, 8, 2, 1),
    YUV_ENTRY(Y8___U8V8_N420, 12, 2, 8, 8, 2, 2),
    YUV_ENTRY(Y8___V8U8_N420, 12, 2, 8, 8, 2, 2),
    YUV_ENTRY(Y10___U10V10_N444, 30, 2, 10, 16, 1, 1),
    YUV_ENTRY(Y10___V10U10_N444, 30, 2, 10, 16, 1, 1),
    YUV_ENTRY(Y10___U10V10_N422, 20, 2, 10, 16, 2, 1),
    YUV_ENTRY(Y10___V10U10_N422, 20, 2, 10, 16, 2, 1),
    YUV_ENTRY(Y10___U10V10_N420, 15, 2, 10, 16, 2, 2),
    YUV_ENTRY(Y10___V10U10_N420, 15, 2, 10, 16, 2, 2),
    YUV_ENTRY(Y12___U12V12_N444, 36, 2, 12, 16, 1, 1),
    YUV_ENTRY(Y12___V12U12_N444, 36, 2, 12, 16, 1, 1),
    YUV_ENTRY(Y12___U12V12_N422, 24, 2, 12, 16, 2, 1),
    YUV_ENTRY(Y12___V12U12_N422, 24, 2, 12, 16, 2, 1),
    YUV_ENTRY(Y12___U12V12_N420, 18, 2, 12, 16, 2, 2),
    YUV_ENTRY(Y12___V12U12_N420, 18, 2, 12, 16, 2, 2),
    YUV_ENTRY(Y8___U8___V8_N444, 24, 3, 8, 8, 1, 1),
    YUV_ENTRY(Y8___U8___V8_N420, 12, 3, 8, 8, 2, 2),
};

ct_assert(ARRAY_LEN(nvKmsSurfaceMemoryFormatInfo) ==
          (NvKmsSurfaceMemoryFormatMax + 1));

const NvKmsSurfaceMemoryFormatInfo *nvKmsGetSurfaceMemoryFormatInfo(
    const enum NvKmsSurfaceMemoryFormat format)
{
    if (format >= ARRAY_LEN(nvKmsSurfaceMemoryFormatInfo)) {
        return &nvKmsEmptyFormatInfo;
    }

    return &nvKmsSurfaceMemoryFormatInfo[format];
}

const char *nvKmsSurfaceMemoryFormatToString(
    const enum NvKmsSurfaceMemoryFormat format)
{
    const NvKmsSurfaceMemoryFormatInfo *pFormatInfo =
        nvKmsGetSurfaceMemoryFormatInfo(format);

    return (pFormatInfo != NULL) ? pFormatInfo->name : NULL;
}
