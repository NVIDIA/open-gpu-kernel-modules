/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __DRM_GEM_CMA_HELPER_H__
#define __DRM_GEM_CMA_HELPER_H__

#include <drm/drm_file.h>
#include <drm/drm_ioctl.h>
#include <drm/drm_gem.h>

struct drm_mode_create_dumb;

/**
 * struct drm_gem_cma_object - GEM object backed by CMA memory allocations
 * @base: base GEM object
 * @paddr: physical address of the backing memory
 * @sgt: scatter/gather table for imported PRIME buffers. The table can have
 *       more than one entry but they are guaranteed to have contiguous
 *       DMA addresses.
 * @vaddr: kernel virtual address of the backing memory
 * @map_noncoherent: if true, the GEM object is backed by non-coherent memory
 */
struct drm_gem_cma_object {
	struct drm_gem_object base;
	dma_addr_t paddr;
	struct sg_table *sgt;

	/* For objects with DMA memory allocated by GEM CMA */
	void *vaddr;

	bool map_noncoherent;
};

#define to_drm_gem_cma_obj(gem_obj) \
	container_of(gem_obj, struct drm_gem_cma_object, base)

#ifndef CONFIG_MMU
#define DRM_GEM_CMA_UNMAPPED_AREA_FOPS \
	.get_unmapped_area	= drm_gem_cma_get_unmapped_area,
#else
#define DRM_GEM_CMA_UNMAPPED_AREA_FOPS
#endif

/**
 * DEFINE_DRM_GEM_CMA_FOPS() - macro to generate file operations for CMA drivers
 * @name: name for the generated structure
 *
 * This macro autogenerates a suitable &struct file_operations for CMA based
 * drivers, which can be assigned to &drm_driver.fops. Note that this structure
 * cannot be shared between drivers, because it contains a reference to the
 * current module using THIS_MODULE.
 *
 * Note that the declaration is already marked as static - if you need a
 * non-static version of this you're probably doing it wrong and will break the
 * THIS_MODULE reference by accident.
 */
#define DEFINE_DRM_GEM_CMA_FOPS(name) \
	static const struct file_operations name = {\
		.owner		= THIS_MODULE,\
		.open		= drm_open,\
		.release	= drm_release,\
		.unlocked_ioctl	= drm_ioctl,\
		.compat_ioctl	= drm_compat_ioctl,\
		.poll		= drm_poll,\
		.read		= drm_read,\
		.llseek		= noop_llseek,\
		.mmap		= drm_gem_mmap,\
		DRM_GEM_CMA_UNMAPPED_AREA_FOPS \
	}

/* free GEM object */
void drm_gem_cma_free_object(struct drm_gem_object *gem_obj);

/* create memory region for DRM framebuffer */
int drm_gem_cma_dumb_create_internal(struct drm_file *file_priv,
				     struct drm_device *drm,
				     struct drm_mode_create_dumb *args);

/* create memory region for DRM framebuffer */
int drm_gem_cma_dumb_create(struct drm_file *file_priv,
			    struct drm_device *drm,
			    struct drm_mode_create_dumb *args);

/* allocate physical memory */
struct drm_gem_cma_object *drm_gem_cma_create(struct drm_device *drm,
					      size_t size);

extern const struct vm_operations_struct drm_gem_cma_vm_ops;

#ifndef CONFIG_MMU
unsigned long drm_gem_cma_get_unmapped_area(struct file *filp,
					    unsigned long addr,
					    unsigned long len,
					    unsigned long pgoff,
					    unsigned long flags);
#endif

void drm_gem_cma_print_info(struct drm_printer *p, unsigned int indent,
			    const struct drm_gem_object *obj);

struct sg_table *drm_gem_cma_get_sg_table(struct drm_gem_object *obj);
struct drm_gem_object *
drm_gem_cma_prime_import_sg_table(struct drm_device *dev,
				  struct dma_buf_attachment *attach,
				  struct sg_table *sgt);
int drm_gem_cma_vmap(struct drm_gem_object *obj, struct dma_buf_map *map);
int drm_gem_cma_mmap(struct drm_gem_object *obj, struct vm_area_struct *vma);

/**
 * DRM_GEM_CMA_DRIVER_OPS_WITH_DUMB_CREATE - CMA GEM driver operations
 * @dumb_create_func: callback function for .dumb_create
 *
 * This macro provides a shortcut for setting the default GEM operations in the
 * &drm_driver structure.
 *
 * This macro is a variant of DRM_GEM_CMA_DRIVER_OPS for drivers that
 * override the default implementation of &struct rm_driver.dumb_create. Use
 * DRM_GEM_CMA_DRIVER_OPS if possible. Drivers that require a virtual address
 * on imported buffers should use
 * DRM_GEM_CMA_DRIVER_OPS_VMAP_WITH_DUMB_CREATE() instead.
 */
#define DRM_GEM_CMA_DRIVER_OPS_WITH_DUMB_CREATE(dumb_create_func) \
	.dumb_create		= (dumb_create_func), \
	.prime_handle_to_fd	= drm_gem_prime_handle_to_fd, \
	.prime_fd_to_handle	= drm_gem_prime_fd_to_handle, \
	.gem_prime_import_sg_table = drm_gem_cma_prime_import_sg_table, \
	.gem_prime_mmap		= drm_gem_prime_mmap

/**
 * DRM_GEM_CMA_DRIVER_OPS - CMA GEM driver operations
 *
 * This macro provides a shortcut for setting the default GEM operations in the
 * &drm_driver structure.
 *
 * Drivers that come with their own implementation of
 * &struct drm_driver.dumb_create should use
 * DRM_GEM_CMA_DRIVER_OPS_WITH_DUMB_CREATE() instead. Use
 * DRM_GEM_CMA_DRIVER_OPS if possible. Drivers that require a virtual address
 * on imported buffers should use DRM_GEM_CMA_DRIVER_OPS_VMAP instead.
 */
#define DRM_GEM_CMA_DRIVER_OPS \
	DRM_GEM_CMA_DRIVER_OPS_WITH_DUMB_CREATE(drm_gem_cma_dumb_create)

/**
 * DRM_GEM_CMA_DRIVER_OPS_VMAP_WITH_DUMB_CREATE - CMA GEM driver operations
 *                                                ensuring a virtual address
 *                                                on the buffer
 * @dumb_create_func: callback function for .dumb_create
 *
 * This macro provides a shortcut for setting the default GEM operations in the
 * &drm_driver structure for drivers that need the virtual address also on
 * imported buffers.
 *
 * This macro is a variant of DRM_GEM_CMA_DRIVER_OPS_VMAP for drivers that
 * override the default implementation of &struct drm_driver.dumb_create. Use
 * DRM_GEM_CMA_DRIVER_OPS_VMAP if possible. Drivers that do not require a
 * virtual address on imported buffers should use
 * DRM_GEM_CMA_DRIVER_OPS_WITH_DUMB_CREATE() instead.
 */
#define DRM_GEM_CMA_DRIVER_OPS_VMAP_WITH_DUMB_CREATE(dumb_create_func) \
	.dumb_create		= dumb_create_func, \
	.prime_handle_to_fd	= drm_gem_prime_handle_to_fd, \
	.prime_fd_to_handle	= drm_gem_prime_fd_to_handle, \
	.gem_prime_import_sg_table = drm_gem_cma_prime_import_sg_table_vmap, \
	.gem_prime_mmap		= drm_gem_prime_mmap

/**
 * DRM_GEM_CMA_DRIVER_OPS_VMAP - CMA GEM driver operations ensuring a virtual
 *                               address on the buffer
 *
 * This macro provides a shortcut for setting the default GEM operations in the
 * &drm_driver structure for drivers that need the virtual address also on
 * imported buffers.
 *
 * Drivers that come with their own implementation of
 * &struct drm_driver.dumb_create should use
 * DRM_GEM_CMA_DRIVER_OPS_VMAP_WITH_DUMB_CREATE() instead. Use
 * DRM_GEM_CMA_DRIVER_OPS_VMAP if possible. Drivers that do not require a
 * virtual address on imported buffers should use DRM_GEM_CMA_DRIVER_OPS
 * instead.
 */
#define DRM_GEM_CMA_DRIVER_OPS_VMAP \
	DRM_GEM_CMA_DRIVER_OPS_VMAP_WITH_DUMB_CREATE(drm_gem_cma_dumb_create)

struct drm_gem_object *
drm_gem_cma_prime_import_sg_table_vmap(struct drm_device *drm,
				       struct dma_buf_attachment *attach,
				       struct sg_table *sgt);

#endif /* __DRM_GEM_CMA_HELPER_H__ */
