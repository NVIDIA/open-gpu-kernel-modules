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

#ifndef __NVIDIA_DRM_GEM_H__
#define __NVIDIA_DRM_GEM_H__

#include "nvidia-drm-conftest.h"

#if defined(NV_DRM_AVAILABLE)

#include "nvidia-drm-priv.h"

#if defined(NV_DRM_DRMP_H_PRESENT)
#include <drm/drmP.h>
#endif

#if defined(NV_DRM_DRM_GEM_H_PRESENT)
#include <drm/drm_gem.h>
#endif

#include "nvkms-kapi.h"
#include "nv-mm.h"

#if defined(NV_DRM_FENCE_AVAILABLE)
#include "nvidia-dma-fence-helper.h"
#include "nvidia-dma-resv-helper.h"
#endif

struct nv_drm_gem_object;

struct nv_drm_gem_object_funcs {
    void (*free)(struct nv_drm_gem_object *nv_gem);
    struct sg_table *(*prime_get_sg_table)(struct nv_drm_gem_object *nv_gem);
    void *(*prime_vmap)(struct nv_drm_gem_object *nv_gem);
    void (*prime_vunmap)(struct nv_drm_gem_object *nv_gem, void *address);
    struct drm_gem_object *(*prime_dup)(struct drm_device *dev,
                                        const struct nv_drm_gem_object *nv_gem_src);
    int (*mmap)(struct nv_drm_gem_object *nv_gem, struct vm_area_struct *vma);
    vm_fault_t (*handle_vma_fault)(struct nv_drm_gem_object *nv_gem,
                                   struct vm_area_struct *vma,
                                   struct vm_fault *vmf);
    int (*create_mmap_offset)(struct nv_drm_device *nv_dev,
                              struct nv_drm_gem_object *nv_gem,
                              uint64_t *offset);
};

struct nv_drm_gem_object {
    struct drm_gem_object base;

    struct nv_drm_device *nv_dev;
    const struct nv_drm_gem_object_funcs *ops;

    struct NvKmsKapiMemory *pMemory;

#if defined(NV_DRM_FENCE_AVAILABLE)
    nv_dma_resv_t  resv;
#endif
};

static inline struct nv_drm_gem_object *to_nv_gem_object(
    struct drm_gem_object *gem)
{
    if (gem != NULL) {
        return container_of(gem, struct nv_drm_gem_object, base);
    }

    return NULL;
}

/*
 * drm_gem_object_{get/put}() added by commit
 * e6b62714e87c8811d5564b6a0738dcde63a51774 (2017-02-28) and
 * drm_gem_object_{reference/unreference}() removed by commit
 * 3e70fd160cf0b1945225eaa08dd2cb8544f21cb8 (2018-11-15).
 */

static inline void
nv_drm_gem_object_unreference_unlocked(struct nv_drm_gem_object *nv_gem)
{
#if defined(NV_DRM_GEM_OBJECT_GET_PRESENT)

#if defined(NV_DRM_GEM_OBJECT_PUT_UNLOCK_PRESENT)
    drm_gem_object_put_unlocked(&nv_gem->base);
#else
    drm_gem_object_put(&nv_gem->base);
#endif

#else
    drm_gem_object_unreference_unlocked(&nv_gem->base);
#endif
}

static inline void
nv_drm_gem_object_unreference(struct nv_drm_gem_object *nv_gem)
{
#if defined(NV_DRM_GEM_OBJECT_GET_PRESENT)
    drm_gem_object_put(&nv_gem->base);
#else
    drm_gem_object_unreference(&nv_gem->base);
#endif
}

static inline int nv_drm_gem_handle_create_drop_reference(
    struct drm_file *file_priv,
    struct nv_drm_gem_object *nv_gem,
    uint32_t *handle)
{
    int ret = drm_gem_handle_create(file_priv, &nv_gem->base, handle);

    /* drop reference from allocate - handle holds it now */

    nv_drm_gem_object_unreference_unlocked(nv_gem);

    return ret;
}

static inline int nv_drm_gem_create_mmap_offset(
    struct nv_drm_gem_object *nv_gem,
    uint64_t *offset)
{
    int ret;

    if ((ret = drm_gem_create_mmap_offset(&nv_gem->base)) < 0) {
        NV_DRM_DEV_LOG_ERR(
            nv_gem->nv_dev,
            "drm_gem_create_mmap_offset failed with error code %d",
            ret);
        goto done;
    }

    *offset = drm_vma_node_offset_addr(&nv_gem->base.vma_node);

done:

    return ret;
}

void nv_drm_gem_free(struct drm_gem_object *gem);

static inline struct nv_drm_gem_object *nv_drm_gem_object_lookup(
    struct drm_device *dev,
    struct drm_file *filp,
    u32 handle)
{
#if (NV_DRM_GEM_OBJECT_LOOKUP_ARGUMENT_COUNT == 3)
    return to_nv_gem_object(drm_gem_object_lookup(dev, filp, handle));
#elif (NV_DRM_GEM_OBJECT_LOOKUP_ARGUMENT_COUNT == 2)
    return to_nv_gem_object(drm_gem_object_lookup(filp, handle));
#else
    #error "Unknown argument count of drm_gem_object_lookup()"
#endif
}

static inline int nv_drm_gem_handle_create(struct drm_file *filp,
                                           struct nv_drm_gem_object *nv_gem,
                                           uint32_t *handle)
{
    return drm_gem_handle_create(filp, &nv_gem->base, handle);
}

void nv_drm_gem_object_init(struct nv_drm_device *nv_dev,
                            struct nv_drm_gem_object *nv_gem,
                            const struct nv_drm_gem_object_funcs * const ops,
                            size_t size,
                            struct NvKmsKapiMemory *pMemory);

struct drm_gem_object *nv_drm_gem_prime_import(struct drm_device *dev,
                                               struct dma_buf *dma_buf);

struct sg_table *nv_drm_gem_prime_get_sg_table(struct drm_gem_object *gem);

void *nv_drm_gem_prime_vmap(struct drm_gem_object *gem);

void nv_drm_gem_prime_vunmap(struct drm_gem_object *gem, void *address);

#if defined(NV_DRM_DRIVER_HAS_GEM_PRIME_RES_OBJ)
nv_dma_resv_t* nv_drm_gem_prime_res_obj(struct drm_gem_object *obj);
#endif

extern const struct vm_operations_struct nv_drm_gem_vma_ops;

int nv_drm_gem_map_offset_ioctl(struct drm_device *dev,
                                void *data, struct drm_file *filep);

int nv_drm_mmap(struct file *file, struct vm_area_struct *vma);

int nv_drm_gem_identify_object_ioctl(struct drm_device *dev,
                                     void *data, struct drm_file *filep);

#endif /* NV_DRM_AVAILABLE */

#endif /* __NVIDIA_DRM_GEM_H__ */
