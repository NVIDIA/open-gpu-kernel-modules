/*
 * Copyright © 2008 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Authors:
 *    Eric Anholt <eric@anholt.net>
 *
 */

#include <drm/drm_gem.h>
#include "amdgpu_ttm.h"
#include "amdgpu_dma_buf.h"

#ifdef HAVE_STRUCT_DRM_DRV_GEM_OPEN_OBJECT_CALLBACK
int _kcl_drm_gem_mmap(struct file *filp, struct vm_area_struct *vma) {
	return amdgpu_mmap(filp, vma);
}

int _kcl_drm_gem_prime_mmap(struct drm_gem_object *obj, struct vm_area_struct *vma) {
	return amdgpu_gem_prime_mmap(obj, vma);
}

#else
static int _kcl_drm_gem_mmap_obj(struct drm_gem_object *obj, unsigned long obj_size,
                     struct vm_area_struct *vma)
{
        int ret;

        /* Check for valid size. */
        if (obj_size < vma->vm_end - vma->vm_start)
                return -EINVAL;

        /* Take a ref for this mapping of the object, so that the fault
         * handler can dereference the mmap offset's pointer to the object.
         * This reference is cleaned up by the corresponding vm_close
         * (which should happen whether the vma was created by this call, or
         * by a vm_open due to mremap or partial unmap or whatever).
         */
        drm_gem_object_get(obj);

        vma->vm_private_data = obj;
        vma->vm_ops = obj->funcs->vm_ops;

        if (obj->funcs->mmap) {
                ret = obj->funcs->mmap(obj, vma);
                if (ret)
                        goto err_drm_gem_object_put;
                WARN_ON(!(vma->vm_flags & VM_DONTEXPAND));
        } else {
                if (!vma->vm_ops) {
                        ret = -EINVAL;
                        goto err_drm_gem_object_put;
                }

                vma->vm_flags |= VM_IO | VM_PFNMAP | VM_DONTEXPAND | VM_DONTDUMP;
                vma->vm_page_prot = pgprot_writecombine(vm_get_page_prot(vma->vm_flags));
                vma->vm_page_prot = pgprot_decrypted(vma->vm_page_prot);
        }

        return 0;

err_drm_gem_object_put:
        drm_gem_object_put(obj);
        return ret;
}

int _kcl_drm_gem_mmap(struct file *filp, struct vm_area_struct *vma) {
        struct drm_file *priv = filp->private_data;
        struct drm_device *dev = priv->minor->dev;
        struct drm_gem_object *obj = NULL;
        struct drm_vma_offset_node *node;
        int ret;

        if (drm_dev_is_unplugged(dev))
                return -ENODEV;

        drm_vma_offset_lock_lookup(dev->vma_offset_manager);
        node = drm_vma_offset_exact_lookup_locked(dev->vma_offset_manager,
                                                  vma->vm_pgoff,
                                                  vma_pages(vma));
        if (likely(node)) {
                obj = container_of(node, struct drm_gem_object, vma_node);
                /*
                 * When the object is being freed, after it hits 0-refcnt it
                 * proceeds to tear down the object. In the process it will
                 * attempt to remove the VMA offset and so acquire this
                 * mgr->vm_lock.  Therefore if we find an object with a 0-refcnt
                 * that matches our range, we know it is in the process of being
                 * destroyed and will be freed as soon as we release the lock -
                 * so we have to check for the 0-refcnted object and treat it as
                 * invalid.
                 */
                if (!kref_get_unless_zero(&obj->refcount))
                        obj = NULL;
        }
        drm_vma_offset_unlock_lookup(dev->vma_offset_manager);

        if (!obj)
                return -EINVAL;

        if (!drm_vma_node_is_allowed(node, priv)) {
                drm_gem_object_put(obj);
                return -EACCES;
        }

        if (node->readonly) {
                if (vma->vm_flags & VM_WRITE) {
                        drm_gem_object_put(obj);
                        return -EINVAL;
                }

                vma->vm_flags &= ~VM_MAYWRITE;
        }

        ret = _kcl_drm_gem_mmap_obj(obj, drm_vma_node_size(node) << PAGE_SHIFT,
                               vma);

        drm_gem_object_put(obj);

        return ret;

}

int _kcl_drm_gem_prime_mmap(struct drm_gem_object *obj, struct vm_area_struct *vma) {
	if (obj->funcs && obj->funcs->mmap) {
		vma->vm_ops = obj->funcs->vm_ops;
	}
	return drm_gem_prime_mmap(obj, vma);
}

#endif
