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

#ifndef __NVIDIA_DRM_DRV_H__
#define __NVIDIA_DRM_DRV_H__

#include "nvidia-drm-conftest.h"

#if defined(NV_DRM_AVAILABLE)

struct NvKmsKapiGpuInfo;

int nv_drm_probe_devices(void);

void nv_drm_remove_devices(void);

void nv_drm_suspend_resume(NvBool suspend);

void nv_drm_register_drm_device(const struct NvKmsKapiGpuInfo *);

void nv_drm_update_drm_driver_features(void);

#endif /* defined(NV_DRM_AVAILABLE) */

#endif /* __NVIDIA_DRM_DRV_H__ */
