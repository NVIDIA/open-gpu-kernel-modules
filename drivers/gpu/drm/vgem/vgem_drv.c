/*
 * Copyright 2011 Red Hat, Inc.
 * Copyright © 2014 The Chromium OS Authors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software")
 * to deal in the software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * them Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTIBILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES, OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT, OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 *	Adam Jackson <ajax@redhat.com>
 *	Ben Widawsky <ben@bwidawsk.net>
 */

/*
 * This is vgem, a (non-hardware-backed) GEM service.  This is used by Mesa's
 * software renderer and the X server for efficient buffer sharing.
 */

#include <linux/dma-buf.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/shmem_fs.h>
#include <linux/vmalloc.h>

#include <drm/drm_drv.h>
#include <drm/drm_file.h>
#include <drm/drm_ioctl.h>
#include <drm/drm_managed.h>
#include <drm/drm_prime.h>

#include "vgem_drv.h"

#define DRIVER_NAME	"vgem"
#define DRIVER_DESC	"Virtual GEM provider"
#define DRIVER_DATE	"20120112"
#define DRIVER_MAJOR	1
#define DRIVER_MINOR	0

static const struct drm_gem_object_funcs vgem_gem_object_funcs;

static struct vgem_device {
	struct drm_device drm;
	struct platform_device *platform;
} *vgem_device;

static void vgem_gem_free_object(struct drm_gem_object *obj)
{
	struct drm_vgem_gem_object *vgem_obj = to_vgem_bo(obj);

	kvfree(vgem_obj->pages);
	mutex_destroy(&vgem_obj->pages_lock);

	if (obj->import_attach)
		drm_prime_gem_destroy(obj, vgem_obj->table);

	drm_gem_object_release(obj);
	kfree(vgem_obj);
}

static vm_fault_t vgem_gem_fault(struct vm_fault *vmf)
{
	struct vm_area_struct *vma = vmf->vma;
	struct drm_vgem_gem_object *obj = vma->vm_private_data;
	/* We don't use vmf->pgoff since that has the fake offset */
	unsigned long vaddr = vmf->address;
	vm_fault_t ret = VM_FAULT_SIGBUS;
	loff_t num_pages;
	pgoff_t page_offset;
	page_offset = (vaddr - vma->vm_start) >> PAGE_SHIFT;

	num_pages = DIV_ROUND_UP(obj->base.size, PAGE_SIZE);

	if (page_offset >= num_pages)
		return VM_FAULT_SIGBUS;

	mutex_lock(&obj->pages_lock);
	if (obj->pages) {
		get_page(obj->pages[page_offset]);
		vmf->page = obj->pages[page_offset];
		ret = 0;
	}
	mutex_unlock(&obj->pages_lock);
	if (ret) {
		struct page *page;

		page = shmem_read_mapping_page(
					file_inode(obj->base.filp)->i_mapping,
					page_offset);
		if (!IS_ERR(page)) {
			vmf->page = page;
			ret = 0;
		} else switch (PTR_ERR(page)) {
			case -ENOSPC:
			case -ENOMEM:
				ret = VM_FAULT_OOM;
				break;
			case -EBUSY:
				ret = VM_FAULT_RETRY;
				break;
			case -EFAULT:
			case -EINVAL:
				ret = VM_FAULT_SIGBUS;
				break;
			default:
				WARN_ON(PTR_ERR(page));
				ret = VM_FAULT_SIGBUS;
				break;
		}

	}
	return ret;
}

static const struct vm_operations_struct vgem_gem_vm_ops = {
	.fault = vgem_gem_fault,
	.open = drm_gem_vm_open,
	.close = drm_gem_vm_close,
};

static int vgem_open(struct drm_device *dev, struct drm_file *file)
{
	struct vgem_file *vfile;
	int ret;

	vfile = kzalloc(sizeof(*vfile), GFP_KERNEL);
	if (!vfile)
		return -ENOMEM;

	file->driver_priv = vfile;

	ret = vgem_fence_open(vfile);
	if (ret) {
		kfree(vfile);
		return ret;
	}

	return 0;
}

static void vgem_postclose(struct drm_device *dev, struct drm_file *file)
{
	struct vgem_file *vfile = file->driver_priv;

	vgem_fence_close(vfile);
	kfree(vfile);
}

static struct drm_vgem_gem_object *__vgem_gem_create(struct drm_device *dev,
						unsigned long size)
{
	struct drm_vgem_gem_object *obj;
	int ret;

	obj = kzalloc(sizeof(*obj), GFP_KERNEL);
	if (!obj)
		return ERR_PTR(-ENOMEM);

	obj->base.funcs = &vgem_gem_object_funcs;

	ret = drm_gem_object_init(dev, &obj->base, roundup(size, PAGE_SIZE));
	if (ret) {
		kfree(obj);
		return ERR_PTR(ret);
	}

	mutex_init(&obj->pages_lock);

	return obj;
}

static void __vgem_gem_destroy(struct drm_vgem_gem_object *obj)
{
	drm_gem_object_release(&obj->base);
	kfree(obj);
}

static struct drm_gem_object *vgem_gem_create(struct drm_device *dev,
					      struct drm_file *file,
					      unsigned int *handle,
					      unsigned long size)
{
	struct drm_vgem_gem_object *obj;
	int ret;

	obj = __vgem_gem_create(dev, size);
	if (IS_ERR(obj))
		return ERR_CAST(obj);

	ret = drm_gem_handle_create(file, &obj->base, handle);
	if (ret) {
		drm_gem_object_put(&obj->base);
		return ERR_PTR(ret);
	}

	return &obj->base;
}

static int vgem_gem_dumb_create(struct drm_file *file, struct drm_device *dev,
				struct drm_mode_create_dumb *args)
{
	struct drm_gem_object *gem_object;
	u64 pitch, size;

	pitch = args->width * DIV_ROUND_UP(args->bpp, 8);
	size = args->height * pitch;
	if (size == 0)
		return -EINVAL;

	gem_object = vgem_gem_create(dev, file, &args->handle, size);
	if (IS_ERR(gem_object))
		return PTR_ERR(gem_object);

	args->size = gem_object->size;
	args->pitch = pitch;

	drm_gem_object_put(gem_object);

	DRM_DEBUG("Created object of size %llu\n", args->size);

	return 0;
}

static struct drm_ioctl_desc vgem_ioctls[] = {
	DRM_IOCTL_DEF_DRV(VGEM_FENCE_ATTACH, vgem_fence_attach_ioctl, DRM_RENDER_ALLOW),
	DRM_IOCTL_DEF_DRV(VGEM_FENCE_SIGNAL, vgem_fence_signal_ioctl, DRM_RENDER_ALLOW),
};

static int vgem_mmap(struct file *filp, struct vm_area_struct *vma)
{
	unsigned long flags = vma->vm_flags;
	int ret;

	ret = drm_gem_mmap(filp, vma);
	if (ret)
		return ret;

	/* Keep the WC mmaping set by drm_gem_mmap() but our pages
	 * are ordinary and not special.
	 */
	vma->vm_flags = flags | VM_DONTEXPAND | VM_DONTDUMP;
	return 0;
}

static const struct file_operations vgem_driver_fops = {
	.owner		= THIS_MODULE,
	.open		= drm_open,
	.mmap		= vgem_mmap,
	.poll		= drm_poll,
	.read		= drm_read,
	.unlocked_ioctl = drm_ioctl,
	.compat_ioctl	= drm_compat_ioctl,
	.release	= drm_release,
};

static struct page **vgem_pin_pages(struct drm_vgem_gem_object *bo)
{
	mutex_lock(&bo->pages_lock);
	if (bo->pages_pin_count++ == 0) {
		struct page **pages;

		pages = drm_gem_get_pages(&bo->base);
		if (IS_ERR(pages)) {
			bo->pages_pin_count--;
			mutex_unlock(&bo->pages_lock);
			return pages;
		}

		bo->pages = pages;
	}
	mutex_unlock(&bo->pages_lock);

	return bo->pages;
}

static void vgem_unpin_pages(struct drm_vgem_gem_object *bo)
{
	mutex_lock(&bo->pages_lock);
	if (--bo->pages_pin_count == 0) {
		drm_gem_put_pages(&bo->base, bo->pages, true, true);
		bo->pages = NULL;
	}
	mutex_unlock(&bo->pages_lock);
}

static int vgem_prime_pin(struct drm_gem_object *obj)
{
	struct drm_vgem_gem_object *bo = to_vgem_bo(obj);
	long n_pages = obj->size >> PAGE_SHIFT;
	struct page **pages;

	pages = vgem_pin_pages(bo);
	if (IS_ERR(pages))
		return PTR_ERR(pages);

	/* Flush the object from the CPU cache so that importers can rely
	 * on coherent indirect access via the exported dma-address.
	 */
	drm_clflush_pages(pages, n_pages);

	return 0;
}

static void vgem_prime_unpin(struct drm_gem_object *obj)
{
	struct drm_vgem_gem_object *bo = to_vgem_bo(obj);

	vgem_unpin_pages(bo);
}

static struct sg_table *vgem_prime_get_sg_table(struct drm_gem_object *obj)
{
	struct drm_vgem_gem_object *bo = to_vgem_bo(obj);

	return drm_prime_pages_to_sg(obj->dev, bo->pages, bo->base.size >> PAGE_SHIFT);
}

static struct drm_gem_object* vgem_prime_import(struct drm_device *dev,
						struct dma_buf *dma_buf)
{
	struct vgem_device *vgem = container_of(dev, typeof(*vgem), drm);

	return drm_gem_prime_import_dev(dev, dma_buf, &vgem->platform->dev);
}

static struct drm_gem_object *vgem_prime_import_sg_table(struct drm_device *dev,
			struct dma_buf_attachment *attach, struct sg_table *sg)
{
	struct drm_vgem_gem_object *obj;
	int npages;

	obj = __vgem_gem_create(dev, attach->dmabuf->size);
	if (IS_ERR(obj))
		return ERR_CAST(obj);

	npages = PAGE_ALIGN(attach->dmabuf->size) / PAGE_SIZE;

	obj->table = sg;
	obj->pages = kvmalloc_array(npages, sizeof(struct page *), GFP_KERNEL);
	if (!obj->pages) {
		__vgem_gem_destroy(obj);
		return ERR_PTR(-ENOMEM);
	}

	obj->pages_pin_count++; /* perma-pinned */
	drm_prime_sg_to_page_array(obj->table, obj->pages, npages);
	return &obj->base;
}

static int vgem_prime_vmap(struct drm_gem_object *obj, struct dma_buf_map *map)
{
	struct drm_vgem_gem_object *bo = to_vgem_bo(obj);
	long n_pages = obj->size >> PAGE_SHIFT;
	struct page **pages;
	void *vaddr;

	pages = vgem_pin_pages(bo);
	if (IS_ERR(pages))
		return PTR_ERR(pages);

	vaddr = vmap(pages, n_pages, 0, pgprot_writecombine(PAGE_KERNEL));
	if (!vaddr)
		return -ENOMEM;
	dma_buf_map_set_vaddr(map, vaddr);

	return 0;
}

static void vgem_prime_vunmap(struct drm_gem_object *obj, struct dma_buf_map *map)
{
	struct drm_vgem_gem_object *bo = to_vgem_bo(obj);

	vunmap(map->vaddr);
	vgem_unpin_pages(bo);
}

static int vgem_prime_mmap(struct drm_gem_object *obj,
			   struct vm_area_struct *vma)
{
	int ret;

	if (obj->size < vma->vm_end - vma->vm_start)
		return -EINVAL;

	if (!obj->filp)
		return -ENODEV;

	ret = call_mmap(obj->filp, vma);
	if (ret)
		return ret;

	vma_set_file(vma, obj->filp);
	vma->vm_flags |= VM_DONTEXPAND | VM_DONTDUMP;
	vma->vm_page_prot = pgprot_writecombine(vm_get_page_prot(vma->vm_flags));

	return 0;
}

static const struct drm_gem_object_funcs vgem_gem_object_funcs = {
	.free = vgem_gem_free_object,
	.pin = vgem_prime_pin,
	.unpin = vgem_prime_unpin,
	.get_sg_table = vgem_prime_get_sg_table,
	.vmap = vgem_prime_vmap,
	.vunmap = vgem_prime_vunmap,
	.vm_ops = &vgem_gem_vm_ops,
};

static const struct drm_driver vgem_driver = {
	.driver_features		= DRIVER_GEM | DRIVER_RENDER,
	.open				= vgem_open,
	.postclose			= vgem_postclose,
	.ioctls				= vgem_ioctls,
	.num_ioctls 			= ARRAY_SIZE(vgem_ioctls),
	.fops				= &vgem_driver_fops,

	.dumb_create			= vgem_gem_dumb_create,

	.prime_handle_to_fd = drm_gem_prime_handle_to_fd,
	.prime_fd_to_handle = drm_gem_prime_fd_to_handle,
	.gem_prime_import = vgem_prime_import,
	.gem_prime_import_sg_table = vgem_prime_import_sg_table,
	.gem_prime_mmap = vgem_prime_mmap,

	.name	= DRIVER_NAME,
	.desc	= DRIVER_DESC,
	.date	= DRIVER_DATE,
	.major	= DRIVER_MAJOR,
	.minor	= DRIVER_MINOR,
};

static int __init vgem_init(void)
{
	int ret;
	struct platform_device *pdev;

	pdev = platform_device_register_simple("vgem", -1, NULL, 0);
	if (IS_ERR(pdev))
		return PTR_ERR(pdev);

	if (!devres_open_group(&pdev->dev, NULL, GFP_KERNEL)) {
		ret = -ENOMEM;
		goto out_unregister;
	}

	dma_coerce_mask_and_coherent(&pdev->dev,
				     DMA_BIT_MASK(64));

	vgem_device = devm_drm_dev_alloc(&pdev->dev, &vgem_driver,
					 struct vgem_device, drm);
	if (IS_ERR(vgem_device)) {
		ret = PTR_ERR(vgem_device);
		goto out_devres;
	}
	vgem_device->platform = pdev;

	/* Final step: expose the device/driver to userspace */
	ret = drm_dev_register(&vgem_device->drm, 0);
	if (ret)
		goto out_devres;

	return 0;

out_devres:
	devres_release_group(&pdev->dev, NULL);
out_unregister:
	platform_device_unregister(pdev);
	return ret;
}

static void __exit vgem_exit(void)
{
	struct platform_device *pdev = vgem_device->platform;

	drm_dev_unregister(&vgem_device->drm);
	devres_release_group(&pdev->dev, NULL);
	platform_device_unregister(pdev);
}

module_init(vgem_init);
module_exit(vgem_exit);

MODULE_AUTHOR("Red Hat, Inc.");
MODULE_AUTHOR("Intel Corporation");
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL and additional rights");
