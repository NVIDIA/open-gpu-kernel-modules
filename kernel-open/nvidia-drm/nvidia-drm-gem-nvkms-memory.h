/*
 * Copyright (c) 2017, NVIDIA CORPORATION. All rights reserved.
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

#ifndef __NVIDIA_DRM_GEM_NVKMS_MEMORY_H__
#define __NVIDIA_DRM_GEM_NVKMS_MEMORY_H__

#include "nvidia-drm-conftest.h"

#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)

#include "nvidia-drm-gem.h"

struct nv_drm_gem_nvkms_memory {
    struct nv_drm_gem_object base;

    /*
     * Lock to protect concurrent writes to physically_mapped, pPhysicalAddress,
     * and pWriteCombinedIORemapAddress.
     *
     * __nv_drm_gem_nvkms_map(), the sole writer, is structured such that
     * readers are not required to hold the lock.
     */
    struct mutex map_lock;
    bool physically_mapped;
    void *pPhysicalAddress;
    void *pWriteCombinedIORemapAddress;

    struct page **pages;
    unsigned long pages_count;
};

extern const struct nv_drm_gem_object_funcs nv_gem_nvkms_memory_ops;

static inline struct nv_drm_gem_nvkms_memory *to_nv_nvkms_memory(
    struct nv_drm_gem_object *nv_gem)
{
    if (nv_gem != NULL) {
        return container_of(nv_gem, struct nv_drm_gem_nvkms_memory, base);
    }

    return NULL;
}

static inline struct nv_drm_gem_nvkms_memory *to_nv_nvkms_memory_const(
    const struct nv_drm_gem_object *nv_gem)
{
    if (nv_gem != NULL) {
        return container_of(nv_gem, struct nv_drm_gem_nvkms_memory, base);
    }

    return NULL;
}

static inline
struct nv_drm_gem_nvkms_memory *nv_drm_gem_object_nvkms_memory_lookup(
    struct drm_device *dev,
    struct drm_file *filp,
    u32 handle)
{
    struct nv_drm_gem_object *nv_gem =
            nv_drm_gem_object_lookup(dev, filp, handle);

    if (nv_gem != NULL && nv_gem->ops != &nv_gem_nvkms_memory_ops) {
        nv_drm_gem_object_unreference_unlocked(nv_gem);
        return NULL;
    }

    return to_nv_nvkms_memory(nv_gem);
}

int nv_drm_dumb_create(
    struct drm_file *file_priv,
    struct drm_device *dev, struct drm_mode_create_dumb *args);

int nv_drm_gem_import_nvkms_memory_ioctl(struct drm_device *dev,
                                         void *data, struct drm_file *filep);

int nv_drm_gem_export_nvkms_memory_ioctl(struct drm_device *dev,
                                         void *data, struct drm_file *filep);

int nv_drm_gem_alloc_nvkms_memory_ioctl(struct drm_device *dev,
                                        void *data, struct drm_file *filep);

int nv_drm_dumb_map_offset(struct drm_file *file,
                           struct drm_device *dev, uint32_t handle,
                           uint64_t *offset);

#if defined(NV_DRM_DRIVER_HAS_DUMB_DESTROY)
int nv_drm_dumb_destroy(struct drm_file *file,
                        struct drm_device *dev,
                        uint32_t handle);
#endif /* NV_DRM_DRIVER_HAS_DUMB_DESTROY */

struct drm_gem_object *nv_drm_gem_nvkms_prime_import(
    struct drm_device *dev,
    struct drm_gem_object *gem);

#endif

#endif /* __NVIDIA_DRM_GEM_NVKMS_MEMORY_H__ */
