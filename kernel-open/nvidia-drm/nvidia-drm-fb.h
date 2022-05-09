/*
 * Copyright (c) 2016, NVIDIA CORPORATION. All rights reserved.
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

#ifndef __NVIDIA_DRM_FB_H__
#define __NVIDIA_DRM_FB_H__

#include "nvidia-drm-conftest.h"

#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)

#if defined(NV_DRM_DRMP_H_PRESENT)
#include <drm/drmP.h>
#endif

#if defined(NV_DRM_DRM_FRAMEBUFFER_H_PRESENT)
#include <drm/drm_framebuffer.h>
#endif

#include "nvidia-drm-gem-nvkms-memory.h"
#include "nvkms-kapi.h"

struct nv_drm_framebuffer {
    struct NvKmsKapiSurface *pSurface;

    struct nv_drm_gem_object*
        nv_gem[NVKMS_MAX_PLANES_PER_SURFACE];

    struct drm_framebuffer base;
};

static inline struct nv_drm_framebuffer *to_nv_framebuffer(
    struct drm_framebuffer *fb)
{
    if (fb == NULL) {
        return NULL;
    }
    return container_of(fb, struct nv_drm_framebuffer, base);
}

struct drm_framebuffer *nv_drm_internal_framebuffer_create(
    struct drm_device *dev,
    struct drm_file *file,
    struct drm_mode_fb_cmd2 *cmd);

#endif /* NV_DRM_ATOMIC_MODESET_AVAILABLE */

#endif /* __NVIDIA_DRM_FB_H__ */
