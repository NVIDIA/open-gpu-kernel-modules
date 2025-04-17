/*
 * Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.
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

#ifndef __NVIDIA_DRM_FORMAT_H__
#define __NVIDIA_DRM_FORMAT_H__

#include "nvidia-drm-conftest.h"

#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)

#include <drm/drm_fourcc.h>

#include "nvkms-format.h"

bool nv_drm_format_to_nvkms_format(u32 format,
                                   enum NvKmsSurfaceMemoryFormat *nvkms_format);

uint32_t *nv_drm_format_array_alloc(
    unsigned int *count,
    const long unsigned int nvkms_format_mask);

bool nv_drm_format_is_yuv(u32 format);

#endif /* NV_DRM_ATOMIC_MODESET_AVAILABLE */

#endif /* __NVIDIA_DRM_FORMAT_H__ */
