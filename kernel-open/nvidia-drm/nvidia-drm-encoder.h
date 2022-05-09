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

#ifndef __NVIDIA_DRM_ENCODER_H__
#define __NVIDIA_DRM_ENCODER_H__

#include "nvidia-drm-conftest.h"

#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)

#include "nvidia-drm-priv.h"

#if defined(NV_DRM_DRM_ENCODER_H_PRESENT)
#include <drm/drm_encoder.h>
#else
#include <drm/drmP.h>
#endif

#include "nvkms-kapi.h"

struct nv_drm_encoder {
    NvKmsKapiDisplay hDisplay;

    struct nv_drm_connector *nv_connector;

    struct drm_encoder base;
};

static inline struct nv_drm_encoder *to_nv_encoder(
    struct drm_encoder *encoder)
{
    if (encoder == NULL) {
        return NULL;
    }
    return container_of(encoder, struct nv_drm_encoder, base);
}

struct drm_encoder*
nv_drm_add_encoder(struct drm_device *dev, NvKmsKapiDisplay hDisplay);

void nv_drm_handle_display_change(struct nv_drm_device *nv_dev,
                                  NvKmsKapiDisplay hDisplay);

void nv_drm_handle_dynamic_display_connected(struct nv_drm_device *nv_dev,
                                             NvKmsKapiDisplay hDisplay);

#endif /* NV_DRM_ATOMIC_MODESET_AVAILABLE */

#endif /* __NVIDIA_DRM_ENCODER_H__ */
