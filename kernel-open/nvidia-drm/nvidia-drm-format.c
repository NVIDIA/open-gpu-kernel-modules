/*
 * Copyright (c) 2019-2022, NVIDIA CORPORATION. All rights reserved.
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "nvidia-drm-conftest.h" /* NV_DRM_ATOMIC_MODESET_AVAILABLE */

#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)

#if defined(NV_DRM_DRMP_H_PRESENT)
#include <drm/drmP.h>
#endif
#include <linux/kernel.h>
#include <linux/bitmap.h>

#include "nvidia-drm-format.h"
#include "nvidia-drm-os-interface.h"

static const u32  nvkms_to_drm_format[] = {
    /* RGB formats */
    [NvKmsSurfaceMemoryFormatA1R5G5B5]    = DRM_FORMAT_ARGB1555,
    [NvKmsSurfaceMemoryFormatX1R5G5B5]    = DRM_FORMAT_XRGB1555,
    [NvKmsSurfaceMemoryFormatR5G6B5]      = DRM_FORMAT_RGB565,
    [NvKmsSurfaceMemoryFormatA8R8G8B8]    = DRM_FORMAT_ARGB8888,
    [NvKmsSurfaceMemoryFormatX8R8G8B8]    = DRM_FORMAT_XRGB8888,
    [NvKmsSurfaceMemoryFormatX8B8G8R8]    = DRM_FORMAT_XBGR8888,
    [NvKmsSurfaceMemoryFormatA2B10G10R10] = DRM_FORMAT_ABGR2101010,
    [NvKmsSurfaceMemoryFormatX2B10G10R10] = DRM_FORMAT_XBGR2101010,
    [NvKmsSurfaceMemoryFormatA8B8G8R8]    = DRM_FORMAT_ABGR8888,
#if defined(DRM_FORMAT_ABGR16161616F)
    [NvKmsSurfaceMemoryFormatRF16GF16BF16AF16] = DRM_FORMAT_ABGR16161616F,
#endif
#if defined(DRM_FORMAT_XBGR16161616F)
    [NvKmsSurfaceMemoryFormatRF16GF16BF16XF16] = DRM_FORMAT_XBGR16161616F,
#endif

    [NvKmsSurfaceMemoryFormatY8_U8__Y8_V8_N422] = DRM_FORMAT_YUYV,
    [NvKmsSurfaceMemoryFormatU8_Y8__V8_Y8_N422] = DRM_FORMAT_UYVY,

    /* YUV semi-planar formats
     *
     * NVKMS YUV semi-planar formats are MSB aligned. Yx__UxVx means
     * that the UV components are packed like UUUUUVVVVV (MSB to LSB)
     * and Yx_VxUx means VVVVVUUUUU (MSB to LSB).
     */

    /*
     * 2 plane YCbCr
     * index 0 = Y plane, [7:0] Y
     * index 1 = Cr:Cb plane, [15:0] Cr:Cb little endian
     * or
     * index 1 = Cb:Cr plane, [15:0] Cb:Cr little endian
     */
    [NvKmsSurfaceMemoryFormatY8___V8U8_N444]    = DRM_FORMAT_NV24, /* non-subsampled Cr:Cb plane */
    [NvKmsSurfaceMemoryFormatY8___U8V8_N444]    = DRM_FORMAT_NV42, /* non-subsampled Cb:Cr plane */
    [NvKmsSurfaceMemoryFormatY8___V8U8_N422]    = DRM_FORMAT_NV16, /* 2x1 subsampled Cr:Cb plane */
    [NvKmsSurfaceMemoryFormatY8___U8V8_N422]    = DRM_FORMAT_NV61, /* 2x1 subsampled Cb:Cr plane */
    [NvKmsSurfaceMemoryFormatY8___V8U8_N420]    = DRM_FORMAT_NV12, /* 2x2 subsampled Cr:Cb plane */
    [NvKmsSurfaceMemoryFormatY8___U8V8_N420]    = DRM_FORMAT_NV21, /* 2x2 subsampled Cb:Cr plane */

#if defined(DRM_FORMAT_P210)
    /*
     * 2 plane YCbCr MSB aligned
     * index 0 = Y plane, [15:0] Y:x [10:6] little endian
     * index 1 = Cr:Cb plane, [31:0] Cr:x:Cb:x [10:6:10:6] little endian
     *
     * 2x1 subsampled Cr:Cb plane, 10 bit per channel
     */
    [NvKmsSurfaceMemoryFormatY10___V10U10_N422] = DRM_FORMAT_P210,
#endif

#if defined(DRM_FORMAT_P010)
    /*
     * 2 plane YCbCr MSB aligned
     * index 0 = Y plane, [15:0] Y:x [10:6] little endian
     * index 1 = Cr:Cb plane, [31:0] Cr:x:Cb:x [10:6:10:6] little endian
     *
     * 2x2 subsampled Cr:Cb plane 10 bits per channel
     */
    [NvKmsSurfaceMemoryFormatY10___V10U10_N420] = DRM_FORMAT_P010,
#endif

#if defined(DRM_FORMAT_P012)
    /*
     * 2 plane YCbCr MSB aligned
     * index 0 = Y plane, [15:0] Y:x [12:4] little endian
     * index 1 = Cr:Cb plane, [31:0] Cr:x:Cb:x [12:4:12:4] little endian
     *
     * 2x2 subsampled Cr:Cb plane 12 bits per channel
     */
    [NvKmsSurfaceMemoryFormatY12___V12U12_N420] = DRM_FORMAT_P012,
#endif
};

bool nv_drm_format_to_nvkms_format(u32 format,
                                   enum NvKmsSurfaceMemoryFormat *nvkms_format)
{
    enum NvKmsSurfaceMemoryFormat i;
    for (i = 0; i < ARRAY_SIZE(nvkms_to_drm_format); i++) {
        /*
         * Note nvkms_to_drm_format[] is sparsely populated: it doesn't
         * handle all NvKmsSurfaceMemoryFormat values, so be sure to skip 0
         * entries when iterating through it.
         */
        if (nvkms_to_drm_format[i] != 0 && nvkms_to_drm_format[i] == format) {
            *nvkms_format = i;
            return true;
        }
    }
    return false;
}

uint32_t *nv_drm_format_array_alloc(
    unsigned int *count,
    const long unsigned int nvkms_format_mask)
{
    enum NvKmsSurfaceMemoryFormat i;
    unsigned int max_count = hweight64(nvkms_format_mask);
    uint32_t *array = nv_drm_calloc(1, sizeof(uint32_t) * max_count);

    if (array == NULL) {
        return NULL;
    }

    *count = 0;
    for_each_set_bit(i, &nvkms_format_mask,
        sizeof(nvkms_format_mask) * BITS_PER_BYTE) {

        if (i >= ARRAY_SIZE(nvkms_to_drm_format)) {
            break;
        }

        /*
         * Note nvkms_to_drm_format[] is sparsely populated: it doesn't
         * handle all NvKmsSurfaceMemoryFormat values, so be sure to skip 0
         * entries when iterating through it.
         */
        if (nvkms_to_drm_format[i] == 0) {
            continue;
        }
        array[(*count)++] = nvkms_to_drm_format[i];
    }

    if (*count == 0) {
        nv_drm_free(array);
        return NULL;
    }

    return array;
}

bool nv_drm_format_is_yuv(u32 format)
{
#if defined(NV_DRM_FORMAT_INFO_HAS_IS_YUV)
    const struct drm_format_info *format_info = drm_format_info(format);
    return (format_info != NULL) && format_info->is_yuv;
#else
    switch (format) {
        case DRM_FORMAT_YUYV:
        case DRM_FORMAT_UYVY:

        case DRM_FORMAT_NV24:
        case DRM_FORMAT_NV42:
        case DRM_FORMAT_NV16:
        case DRM_FORMAT_NV61:
        case DRM_FORMAT_NV12:
        case DRM_FORMAT_NV21:

#if defined(DRM_FORMAT_P210)
        case DRM_FORMAT_P210:
#endif
#if defined(DRM_FORMAT_P010)
        case DRM_FORMAT_P010:
#endif
#if defined(DRM_FORMAT_P012)
        case DRM_FORMAT_P012:
#endif
            return true;
        default:
            return false;
    }
#endif
}

#endif
