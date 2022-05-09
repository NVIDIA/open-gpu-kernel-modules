/*
 * Copyright (c) 2020, NVIDIA CORPORATION. All rights reserved.
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

#ifndef __NVIDIA_DRM_GEM_DMA_BUF_H__
#define __NVIDIA_DRM_GEM_DMA_BUF_H__

#include "nvidia-drm-conftest.h"

#if defined(NV_DRM_AVAILABLE)

#include "nvidia-drm-gem.h"

struct nv_drm_gem_dma_buf {
    struct nv_drm_gem_object base;
    struct sg_table *sgt;
};

extern const struct nv_drm_gem_object_funcs __nv_gem_dma_buf_ops;

static inline struct nv_drm_gem_dma_buf *to_nv_dma_buf(
    struct nv_drm_gem_object *nv_gem)
{
    if (nv_gem != NULL) {
        return container_of(nv_gem, struct nv_drm_gem_dma_buf, base);
    }

    return NULL;
}

static inline
struct nv_drm_gem_dma_buf *nv_drm_gem_object_dma_buf_lookup(
    struct drm_device *dev,
    struct drm_file *filp,
    u32 handle)
{
    struct nv_drm_gem_object *nv_gem =
            nv_drm_gem_object_lookup(dev, filp, handle);

    if (nv_gem != NULL && nv_gem->ops != &__nv_gem_dma_buf_ops) {
        nv_drm_gem_object_unreference_unlocked(nv_gem);
        return NULL;
    }

    return to_nv_dma_buf(nv_gem);
}

struct drm_gem_object*
nv_drm_gem_prime_import_sg_table(struct drm_device *dev,
                                 struct dma_buf_attachment *attach,
                                 struct sg_table *sgt);

int nv_drm_gem_export_dmabuf_memory_ioctl(struct drm_device *dev,
                                          void *data, struct drm_file *filep);

#endif

#endif /* __NVIDIA_DRM_GEM_DMA_BUF_H__ */
