/*
 * Copyright (c) 2015, NVIDIA CORPORATION. All rights reserved.
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

#ifndef __NVIDIA_DRM_OS_INTERFACE_H__
#define __NVIDIA_DRM_OS_INTERFACE_H__

#include "nvidia-drm-conftest.h" /* NV_DRM_AVAILABLE */

#include "nvtypes.h"

#if defined(NV_DRM_AVAILABLE)

struct page;

/* Set to true when the atomic modeset feature is enabled. */
extern bool nv_drm_modeset_module_param;

void *nv_drm_calloc(size_t nmemb, size_t size);

void nv_drm_free(void *ptr);

char *nv_drm_asprintf(const char *fmt, ...);

void nv_drm_write_combine_flush(void);

int nv_drm_lock_user_pages(unsigned long address,
                           unsigned long pages_count, struct page ***pages);

void nv_drm_unlock_user_pages(unsigned long  pages_count, struct page **pages);

void *nv_drm_vmap(struct page **pages, unsigned long pages_count);

void nv_drm_vunmap(void *address);

#endif

#endif /* __NVIDIA_DRM_OS_INTERFACE_H__ */
