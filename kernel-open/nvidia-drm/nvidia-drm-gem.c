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

#include "nvidia-drm-conftest.h"

#if defined(NV_DRM_AVAILABLE)

#include "nvidia-drm-priv.h"
#include "nvidia-drm-ioctl.h"
#include "nvidia-drm-fence.h"
#include "nvidia-drm-gem.h"
#include "nvidia-drm-gem-nvkms-memory.h"
#include "nvidia-drm-gem-user-memory.h"
#include "nvidia-dma-resv-helper.h"
#include "nvidia-drm-helper.h"
#include "nvidia-drm-gem-dma-buf.h"
#include "nvidia-drm-gem-nvkms-memory.h"

#if defined(NV_DRM_DRM_DRV_H_PRESENT)
#include <drm/drm_drv.h>
#endif

#if defined(NV_DRM_DRM_PRIME_H_PRESENT)
#include <drm/drm_prime.h>
#endif

#if defined(NV_DRM_DRM_FILE_H_PRESENT)
#include <drm/drm_file.h>
#endif

#include "linux/dma-buf.h"

#include "nv-mm.h"

void nv_drm_gem_free(struct drm_gem_object *gem)
{
    struct nv_drm_gem_object *nv_gem = to_nv_gem_object(gem);

    /* Cleanup core gem object */
    drm_gem_object_release(&nv_gem->base);

#if defined(NV_DRM_FENCE_AVAILABLE) && !defined(NV_DRM_GEM_OBJECT_HAS_RESV)
    nv_dma_resv_fini(&nv_gem->resv);
#endif

    nv_gem->ops->free(nv_gem);
}

#if !defined(NV_DRM_DRIVER_HAS_GEM_PRIME_CALLBACKS) && \
    defined(NV_DRM_GEM_OBJECT_VMAP_HAS_MAP_ARG)

/*
 * The 'dma_buf_map' structure is renamed to 'iosys_map' by the commit
 * 7938f4218168 ("dma-buf-map: Rename to iosys-map").
 */
#if defined(NV_LINUX_IOSYS_MAP_H_PRESENT)
typedef struct iosys_map nv_sysio_map_t;
#else
typedef struct dma_buf_map nv_sysio_map_t;
#endif

static int nv_drm_gem_vmap(struct drm_gem_object *gem,
                           nv_sysio_map_t *map)
{
    void *vaddr = nv_drm_gem_prime_vmap(gem);
    if (vaddr == NULL) {
        return -ENOMEM;
    } else if (IS_ERR(vaddr)) {
        return PTR_ERR(vaddr);
    }
    map->vaddr = vaddr;
    map->is_iomem = true;
    return 0;
}

static void nv_drm_gem_vunmap(struct drm_gem_object *gem,
                              nv_sysio_map_t *map)
{
    nv_drm_gem_prime_vunmap(gem, map->vaddr);
    map->vaddr = NULL;
}
#endif

#if !defined(NV_DRM_DRIVER_HAS_GEM_FREE_OBJECT) || \
    !defined(NV_DRM_DRIVER_HAS_GEM_PRIME_CALLBACKS)
static struct drm_gem_object_funcs nv_drm_gem_funcs = {
    .free = nv_drm_gem_free,
    .get_sg_table = nv_drm_gem_prime_get_sg_table,

#if !defined(NV_DRM_DRIVER_HAS_GEM_PRIME_CALLBACKS)
    .export  = drm_gem_prime_export,
#if defined(NV_DRM_GEM_OBJECT_VMAP_HAS_MAP_ARG)
    .vmap    = nv_drm_gem_vmap,
    .vunmap  = nv_drm_gem_vunmap,
#else
    .vmap    = nv_drm_gem_prime_vmap,
    .vunmap  = nv_drm_gem_prime_vunmap,
#endif
    .vm_ops  = &nv_drm_gem_vma_ops,
#endif
};
#endif

void nv_drm_gem_object_init(struct nv_drm_device *nv_dev,
                            struct nv_drm_gem_object *nv_gem,
                            const struct nv_drm_gem_object_funcs * const ops,
                            size_t size,
                            struct NvKmsKapiMemory *pMemory)
{
    struct drm_device *dev = nv_dev->dev;

    nv_gem->nv_dev = nv_dev;
    nv_gem->ops = ops;

    nv_gem->pMemory = pMemory;

    /* Initialize the gem object */

#if defined(NV_DRM_FENCE_AVAILABLE) && !defined(NV_DRM_GEM_OBJECT_HAS_RESV)
    nv_dma_resv_init(&nv_gem->resv);
#endif

#if !defined(NV_DRM_DRIVER_HAS_GEM_FREE_OBJECT)
    nv_gem->base.funcs = &nv_drm_gem_funcs;
#endif

    drm_gem_private_object_init(dev, &nv_gem->base, size);
}

struct drm_gem_object *nv_drm_gem_prime_import(struct drm_device *dev,
                                               struct dma_buf *dma_buf)
{
#if defined(NV_DMA_BUF_OWNER_PRESENT)
    struct drm_gem_object *gem_dst;
    struct nv_drm_gem_object *nv_gem_src;

    if (dma_buf->owner == dev->driver->fops->owner) {
        nv_gem_src = to_nv_gem_object(dma_buf->priv);

        if (nv_gem_src->base.dev != dev &&
            nv_gem_src->ops->prime_dup != NULL) {
            /*
             * If we're importing from another NV device, try to handle the
             * import internally rather than attaching through the dma-buf
             * mechanisms.  Importing from the same device is even easier,
             * and drm_gem_prime_import() handles that just fine.
             */
            gem_dst = nv_gem_src->ops->prime_dup(dev, nv_gem_src);

            if (gem_dst)
                return gem_dst;
        }
    }
#endif /* NV_DMA_BUF_OWNER_PRESENT */

    return drm_gem_prime_import(dev, dma_buf);
}

struct sg_table *nv_drm_gem_prime_get_sg_table(struct drm_gem_object *gem)
{
    struct nv_drm_gem_object *nv_gem = to_nv_gem_object(gem);

    if (nv_gem->ops->prime_get_sg_table != NULL) {
        return nv_gem->ops->prime_get_sg_table(nv_gem);
    }

    return ERR_PTR(-ENOTSUPP);
}

void *nv_drm_gem_prime_vmap(struct drm_gem_object *gem)
{
    struct nv_drm_gem_object *nv_gem = to_nv_gem_object(gem);

    if (nv_gem->ops->prime_vmap != NULL) {
        return nv_gem->ops->prime_vmap(nv_gem);
    }

    return ERR_PTR(-ENOTSUPP);
}

void nv_drm_gem_prime_vunmap(struct drm_gem_object *gem, void *address)
{
    struct nv_drm_gem_object *nv_gem = to_nv_gem_object(gem);

    if (nv_gem->ops->prime_vunmap != NULL) {
        nv_gem->ops->prime_vunmap(nv_gem, address);
    }
}

#if defined(NV_DRM_DRIVER_HAS_GEM_PRIME_RES_OBJ)
nv_dma_resv_t* nv_drm_gem_prime_res_obj(struct drm_gem_object *obj)
{
    struct nv_drm_gem_object *nv_gem = to_nv_gem_object(obj);
    return nv_drm_gem_res_obj(nv_gem);
}
#endif

int nv_drm_gem_map_offset_ioctl(struct drm_device *dev,
                                void *data, struct drm_file *filep)
{
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct drm_nvidia_gem_map_offset_params *params = data;
    struct nv_drm_gem_object *nv_gem;
    int ret;

    if ((nv_gem = nv_drm_gem_object_lookup(dev,
                                           filep,
                                           params->handle)) == NULL) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to lookup gem object for map: 0x%08x",
            params->handle);
        return -EINVAL;
    }

    if (nv_gem->ops->create_mmap_offset) {
        ret = nv_gem->ops->create_mmap_offset(nv_dev, nv_gem, &params->offset);
    } else {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Gem object type does not support mapping: 0x%08x",
            params->handle);
        ret = -EINVAL;
    }

    nv_drm_gem_object_unreference_unlocked(nv_gem);

    return ret;
}

#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)
int nv_drm_mmap(struct file *file, struct vm_area_struct *vma)
{
    struct drm_file *priv = file->private_data;
    struct drm_device *dev = priv->minor->dev;
    struct drm_gem_object *obj = NULL;
    struct drm_vma_offset_node *node;
    int ret = 0;
    struct nv_drm_gem_object *nv_gem;

    drm_vma_offset_lock_lookup(dev->vma_offset_manager);
    node = nv_drm_vma_offset_exact_lookup_locked(dev->vma_offset_manager,
                                                 vma->vm_pgoff, vma_pages(vma));
    if (likely(node)) {
        obj = container_of(node, struct drm_gem_object, vma_node);
        /*
         * When the object is being freed, after it hits 0-refcnt it proceeds
         * to tear down the object. In the process it will attempt to remove
         * the VMA offset and so acquire this mgr->vm_lock.  Therefore if we
         * find an object with a 0-refcnt that matches our range, we know it is
         * in the process of being destroyed and will be freed as soon as we
         * release the lock - so we have to check for the 0-refcnted object and
         * treat it as invalid.
         */
        if (!kref_get_unless_zero(&obj->refcount))
            obj = NULL;
    }
    drm_vma_offset_unlock_lookup(dev->vma_offset_manager);

    if (!obj)
        return -EINVAL;

    nv_gem = to_nv_gem_object(obj);
    if (nv_gem->ops->mmap == NULL) {
        ret = -EINVAL;
        goto done;
    }

    if (!nv_drm_vma_node_is_allowed(node, file)) {
        ret = -EACCES;
        goto done;
    }

#if defined(NV_DRM_VMA_OFFSET_NODE_HAS_READONLY)
    if (node->readonly) {
        if (vma->vm_flags & VM_WRITE) {
            ret = -EINVAL;
            goto done;
        }
        nv_vm_flags_clear(vma, VM_MAYWRITE);
    }
#endif

    ret = nv_gem->ops->mmap(nv_gem, vma);

done:
    nv_drm_gem_object_unreference_unlocked(nv_gem);

    return ret;
}
#endif

int nv_drm_gem_identify_object_ioctl(struct drm_device *dev,
                                     void *data, struct drm_file *filep)
{
    struct drm_nvidia_gem_identify_object_params *p = data;
    struct nv_drm_gem_dma_buf *nv_dma_buf;
    struct nv_drm_gem_nvkms_memory *nv_nvkms_memory;
    struct nv_drm_gem_user_memory *nv_user_memory;
    struct nv_drm_gem_object *nv_gem = NULL;

    if (!drm_core_check_feature(dev, DRIVER_MODESET)) {
        return -EOPNOTSUPP;
    }

    nv_dma_buf = nv_drm_gem_object_dma_buf_lookup(dev, filep, p->handle);
    if (nv_dma_buf) {
        p->object_type = NV_GEM_OBJECT_DMABUF;
        nv_gem = &nv_dma_buf->base;
        goto done;
    }

#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)
    nv_nvkms_memory = nv_drm_gem_object_nvkms_memory_lookup(dev, filep, p->handle);
    if (nv_nvkms_memory) {
        p->object_type = NV_GEM_OBJECT_NVKMS;
        nv_gem = &nv_nvkms_memory->base;
        goto done;
    }
#endif

    nv_user_memory = nv_drm_gem_object_user_memory_lookup(dev, filep, p->handle);
    if (nv_user_memory) {
        p->object_type = NV_GEM_OBJECT_USERMEMORY;
        nv_gem = &nv_user_memory->base;
        goto done;
    }

    p->object_type = NV_GEM_OBJECT_UNKNOWN;

done:
    if (nv_gem) {
        nv_drm_gem_object_unreference_unlocked(nv_gem);
    }
    return 0;
}

/* XXX Move these vma operations to os layer */

static vm_fault_t __nv_drm_vma_fault(struct vm_area_struct *vma,
                              struct vm_fault *vmf)
{
    struct drm_gem_object *gem = vma->vm_private_data;
    struct nv_drm_gem_object *nv_gem = to_nv_gem_object(gem);

    if (!nv_gem) {
        return VM_FAULT_SIGBUS;
    }

    return nv_gem->ops->handle_vma_fault(nv_gem, vma, vmf);
}

/*
 * Note that nv_drm_vma_fault() can be called for different or same
 * ranges of the same drm_gem_object simultaneously.
 */

#if defined(NV_VM_OPS_FAULT_REMOVED_VMA_ARG)
static vm_fault_t nv_drm_vma_fault(struct vm_fault *vmf)
{
    return __nv_drm_vma_fault(vmf->vma, vmf);
}
#else
static vm_fault_t nv_drm_vma_fault(struct vm_area_struct *vma,
                                struct vm_fault *vmf)
{
    return __nv_drm_vma_fault(vma, vmf);
}
#endif

const struct vm_operations_struct nv_drm_gem_vma_ops = {
    .open  = drm_gem_vm_open,
    .fault = nv_drm_vma_fault,
    .close = drm_gem_vm_close,
};

#endif /* NV_DRM_AVAILABLE */
