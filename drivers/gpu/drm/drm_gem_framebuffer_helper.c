// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * drm gem framebuffer helper functions
 *
 * Copyright (C) 2017 Noralf Trønnes
 */

#include <linux/slab.h>

#include <drm/drm_damage_helper.h>
#include <drm/drm_fb_helper.h>
#include <drm/drm_fourcc.h>
#include <drm/drm_framebuffer.h>
#include <drm/drm_gem.h>
#include <drm/drm_gem_framebuffer_helper.h>
#include <drm/drm_modeset_helper.h>

#define AFBC_HEADER_SIZE		16
#define AFBC_TH_LAYOUT_ALIGNMENT	8
#define AFBC_HDR_ALIGN			64
#define AFBC_SUPERBLOCK_PIXELS		256
#define AFBC_SUPERBLOCK_ALIGNMENT	128
#define AFBC_TH_BODY_START_ALIGNMENT	4096

/**
 * DOC: overview
 *
 * This library provides helpers for drivers that don't subclass
 * &drm_framebuffer and use &drm_gem_object for their backing storage.
 *
 * Drivers without additional needs to validate framebuffers can simply use
 * drm_gem_fb_create() and everything is wired up automatically. Other drivers
 * can use all parts independently.
 */

/**
 * drm_gem_fb_get_obj() - Get GEM object backing the framebuffer
 * @fb: Framebuffer
 * @plane: Plane index
 *
 * No additional reference is taken beyond the one that the &drm_frambuffer
 * already holds.
 *
 * Returns:
 * Pointer to &drm_gem_object for the given framebuffer and plane index or NULL
 * if it does not exist.
 */
struct drm_gem_object *drm_gem_fb_get_obj(struct drm_framebuffer *fb,
					  unsigned int plane)
{
	if (plane >= 4)
		return NULL;

	return fb->obj[plane];
}
EXPORT_SYMBOL_GPL(drm_gem_fb_get_obj);

static int
drm_gem_fb_init(struct drm_device *dev,
		 struct drm_framebuffer *fb,
		 const struct drm_mode_fb_cmd2 *mode_cmd,
		 struct drm_gem_object **obj, unsigned int num_planes,
		 const struct drm_framebuffer_funcs *funcs)
{
	int ret, i;

	drm_helper_mode_fill_fb_struct(dev, fb, mode_cmd);

	for (i = 0; i < num_planes; i++)
		fb->obj[i] = obj[i];

	ret = drm_framebuffer_init(dev, fb, funcs);
	if (ret)
		drm_err(dev, "Failed to init framebuffer: %d\n", ret);

	return ret;
}

/**
 * drm_gem_fb_destroy - Free GEM backed framebuffer
 * @fb: Framebuffer
 *
 * Frees a GEM backed framebuffer with its backing buffer(s) and the structure
 * itself. Drivers can use this as their &drm_framebuffer_funcs->destroy
 * callback.
 */
void drm_gem_fb_destroy(struct drm_framebuffer *fb)
{
	int i;

	for (i = 0; i < 4; i++)
		drm_gem_object_put(fb->obj[i]);

	drm_framebuffer_cleanup(fb);
	kfree(fb);
}
EXPORT_SYMBOL(drm_gem_fb_destroy);

/**
 * drm_gem_fb_create_handle - Create handle for GEM backed framebuffer
 * @fb: Framebuffer
 * @file: DRM file to register the handle for
 * @handle: Pointer to return the created handle
 *
 * This function creates a handle for the GEM object backing the framebuffer.
 * Drivers can use this as their &drm_framebuffer_funcs->create_handle
 * callback. The GETFB IOCTL calls into this callback.
 *
 * Returns:
 * 0 on success or a negative error code on failure.
 */
int drm_gem_fb_create_handle(struct drm_framebuffer *fb, struct drm_file *file,
			     unsigned int *handle)
{
	return drm_gem_handle_create(file, fb->obj[0], handle);
}
EXPORT_SYMBOL(drm_gem_fb_create_handle);

/**
 * drm_gem_fb_init_with_funcs() - Helper function for implementing
 *				  &drm_mode_config_funcs.fb_create
 *				  callback in cases when the driver
 *				  allocates a subclass of
 *				  struct drm_framebuffer
 * @dev: DRM device
 * @fb: framebuffer object
 * @file: DRM file that holds the GEM handle(s) backing the framebuffer
 * @mode_cmd: Metadata from the userspace framebuffer creation request
 * @funcs: vtable to be used for the new framebuffer object
 *
 * This function can be used to set &drm_framebuffer_funcs for drivers that need
 * custom framebuffer callbacks. Use drm_gem_fb_create() if you don't need to
 * change &drm_framebuffer_funcs. The function does buffer size validation.
 * The buffer size validation is for a general case, though, so users should
 * pay attention to the checks being appropriate for them or, at least,
 * non-conflicting.
 *
 * Returns:
 * Zero or a negative error code.
 */
int drm_gem_fb_init_with_funcs(struct drm_device *dev,
			       struct drm_framebuffer *fb,
			       struct drm_file *file,
			       const struct drm_mode_fb_cmd2 *mode_cmd,
			       const struct drm_framebuffer_funcs *funcs)
{
	const struct drm_format_info *info;
	struct drm_gem_object *objs[4];
	int ret, i;

	info = drm_get_format_info(dev, mode_cmd);
	if (!info) {
		drm_dbg_kms(dev, "Failed to get FB format info\n");
		return -EINVAL;
	}

	for (i = 0; i < info->num_planes; i++) {
		unsigned int width = mode_cmd->width / (i ? info->hsub : 1);
		unsigned int height = mode_cmd->height / (i ? info->vsub : 1);
		unsigned int min_size;

		objs[i] = drm_gem_object_lookup(file, mode_cmd->handles[i]);
		if (!objs[i]) {
			drm_dbg_kms(dev, "Failed to lookup GEM object\n");
			ret = -ENOENT;
			goto err_gem_object_put;
		}

		min_size = (height - 1) * mode_cmd->pitches[i]
			 + drm_format_info_min_pitch(info, i, width)
			 + mode_cmd->offsets[i];

		if (objs[i]->size < min_size) {
			drm_dbg_kms(dev,
				    "GEM object size (%zu) smaller than minimum size (%u) for plane %d\n",
				    objs[i]->size, min_size, i);
			drm_gem_object_put(objs[i]);
			ret = -EINVAL;
			goto err_gem_object_put;
		}
	}

	ret = drm_gem_fb_init(dev, fb, mode_cmd, objs, i, funcs);
	if (ret)
		goto err_gem_object_put;

	return 0;

err_gem_object_put:
	for (i--; i >= 0; i--)
		drm_gem_object_put(objs[i]);

	return ret;
}
EXPORT_SYMBOL_GPL(drm_gem_fb_init_with_funcs);

/**
 * drm_gem_fb_create_with_funcs() - Helper function for the
 *                                  &drm_mode_config_funcs.fb_create
 *                                  callback
 * @dev: DRM device
 * @file: DRM file that holds the GEM handle(s) backing the framebuffer
 * @mode_cmd: Metadata from the userspace framebuffer creation request
 * @funcs: vtable to be used for the new framebuffer object
 *
 * This function can be used to set &drm_framebuffer_funcs for drivers that need
 * custom framebuffer callbacks. Use drm_gem_fb_create() if you don't need to
 * change &drm_framebuffer_funcs. The function does buffer size validation.
 *
 * Returns:
 * Pointer to a &drm_framebuffer on success or an error pointer on failure.
 */
struct drm_framebuffer *
drm_gem_fb_create_with_funcs(struct drm_device *dev, struct drm_file *file,
			     const struct drm_mode_fb_cmd2 *mode_cmd,
			     const struct drm_framebuffer_funcs *funcs)
{
	struct drm_framebuffer *fb;
	int ret;

	fb = kzalloc(sizeof(*fb), GFP_KERNEL);
	if (!fb)
		return ERR_PTR(-ENOMEM);

	ret = drm_gem_fb_init_with_funcs(dev, fb, file, mode_cmd, funcs);
	if (ret) {
		kfree(fb);
		return ERR_PTR(ret);
	}

	return fb;
}
EXPORT_SYMBOL_GPL(drm_gem_fb_create_with_funcs);

static const struct drm_framebuffer_funcs drm_gem_fb_funcs = {
	.destroy	= drm_gem_fb_destroy,
	.create_handle	= drm_gem_fb_create_handle,
};

/**
 * drm_gem_fb_create() - Helper function for the
 *                       &drm_mode_config_funcs.fb_create callback
 * @dev: DRM device
 * @file: DRM file that holds the GEM handle(s) backing the framebuffer
 * @mode_cmd: Metadata from the userspace framebuffer creation request
 *
 * This function creates a new framebuffer object described by
 * &drm_mode_fb_cmd2. This description includes handles for the buffer(s)
 * backing the framebuffer.
 *
 * If your hardware has special alignment or pitch requirements these should be
 * checked before calling this function. The function does buffer size
 * validation. Use drm_gem_fb_create_with_dirty() if you need framebuffer
 * flushing.
 *
 * Drivers can use this as their &drm_mode_config_funcs.fb_create callback.
 * The ADDFB2 IOCTL calls into this callback.
 *
 * Returns:
 * Pointer to a &drm_framebuffer on success or an error pointer on failure.
 */
struct drm_framebuffer *
drm_gem_fb_create(struct drm_device *dev, struct drm_file *file,
		  const struct drm_mode_fb_cmd2 *mode_cmd)
{
	return drm_gem_fb_create_with_funcs(dev, file, mode_cmd,
					    &drm_gem_fb_funcs);
}
EXPORT_SYMBOL_GPL(drm_gem_fb_create);

static const struct drm_framebuffer_funcs drm_gem_fb_funcs_dirtyfb = {
	.destroy	= drm_gem_fb_destroy,
	.create_handle	= drm_gem_fb_create_handle,
	.dirty		= drm_atomic_helper_dirtyfb,
};

/**
 * drm_gem_fb_create_with_dirty() - Helper function for the
 *                       &drm_mode_config_funcs.fb_create callback
 * @dev: DRM device
 * @file: DRM file that holds the GEM handle(s) backing the framebuffer
 * @mode_cmd: Metadata from the userspace framebuffer creation request
 *
 * This function creates a new framebuffer object described by
 * &drm_mode_fb_cmd2. This description includes handles for the buffer(s)
 * backing the framebuffer. drm_atomic_helper_dirtyfb() is used for the dirty
 * callback giving framebuffer flushing through the atomic machinery. Use
 * drm_gem_fb_create() if you don't need the dirty callback.
 * The function does buffer size validation.
 *
 * Drivers should also call drm_plane_enable_fb_damage_clips() on all planes
 * to enable userspace to use damage clips also with the ATOMIC IOCTL.
 *
 * Drivers can use this as their &drm_mode_config_funcs.fb_create callback.
 * The ADDFB2 IOCTL calls into this callback.
 *
 * Returns:
 * Pointer to a &drm_framebuffer on success or an error pointer on failure.
 */
struct drm_framebuffer *
drm_gem_fb_create_with_dirty(struct drm_device *dev, struct drm_file *file,
			     const struct drm_mode_fb_cmd2 *mode_cmd)
{
	return drm_gem_fb_create_with_funcs(dev, file, mode_cmd,
					    &drm_gem_fb_funcs_dirtyfb);
}
EXPORT_SYMBOL_GPL(drm_gem_fb_create_with_dirty);

static __u32 drm_gem_afbc_get_bpp(struct drm_device *dev,
				  const struct drm_mode_fb_cmd2 *mode_cmd)
{
	const struct drm_format_info *info;

	info = drm_get_format_info(dev, mode_cmd);

	/* use whatever a driver has set */
	if (info->cpp[0])
		return info->cpp[0] * 8;

	/* guess otherwise */
	switch (info->format) {
	case DRM_FORMAT_YUV420_8BIT:
		return 12;
	case DRM_FORMAT_YUV420_10BIT:
		return 15;
	case DRM_FORMAT_VUY101010:
		return 30;
	default:
		break;
	}

	/* all attempts failed */
	return 0;
}

static int drm_gem_afbc_min_size(struct drm_device *dev,
				 const struct drm_mode_fb_cmd2 *mode_cmd,
				 struct drm_afbc_framebuffer *afbc_fb)
{
	__u32 n_blocks, w_alignment, h_alignment, hdr_alignment;
	/* remove bpp when all users properly encode cpp in drm_format_info */
	__u32 bpp;

	switch (mode_cmd->modifier[0] & AFBC_FORMAT_MOD_BLOCK_SIZE_MASK) {
	case AFBC_FORMAT_MOD_BLOCK_SIZE_16x16:
		afbc_fb->block_width = 16;
		afbc_fb->block_height = 16;
		break;
	case AFBC_FORMAT_MOD_BLOCK_SIZE_32x8:
		afbc_fb->block_width = 32;
		afbc_fb->block_height = 8;
		break;
	/* no user exists yet - fall through */
	case AFBC_FORMAT_MOD_BLOCK_SIZE_64x4:
	case AFBC_FORMAT_MOD_BLOCK_SIZE_32x8_64x4:
	default:
		drm_dbg_kms(dev, "Invalid AFBC_FORMAT_MOD_BLOCK_SIZE: %lld.\n",
			    mode_cmd->modifier[0]
			    & AFBC_FORMAT_MOD_BLOCK_SIZE_MASK);
		return -EINVAL;
	}

	/* tiled header afbc */
	w_alignment = afbc_fb->block_width;
	h_alignment = afbc_fb->block_height;
	hdr_alignment = AFBC_HDR_ALIGN;
	if (mode_cmd->modifier[0] & AFBC_FORMAT_MOD_TILED) {
		w_alignment *= AFBC_TH_LAYOUT_ALIGNMENT;
		h_alignment *= AFBC_TH_LAYOUT_ALIGNMENT;
		hdr_alignment = AFBC_TH_BODY_START_ALIGNMENT;
	}

	afbc_fb->aligned_width = ALIGN(mode_cmd->width, w_alignment);
	afbc_fb->aligned_height = ALIGN(mode_cmd->height, h_alignment);
	afbc_fb->offset = mode_cmd->offsets[0];

	bpp = drm_gem_afbc_get_bpp(dev, mode_cmd);
	if (!bpp) {
		drm_dbg_kms(dev, "Invalid AFBC bpp value: %d\n", bpp);
		return -EINVAL;
	}

	n_blocks = (afbc_fb->aligned_width * afbc_fb->aligned_height)
		   / AFBC_SUPERBLOCK_PIXELS;
	afbc_fb->afbc_size = ALIGN(n_blocks * AFBC_HEADER_SIZE, hdr_alignment);
	afbc_fb->afbc_size += n_blocks * ALIGN(bpp * AFBC_SUPERBLOCK_PIXELS / 8,
					       AFBC_SUPERBLOCK_ALIGNMENT);

	return 0;
}

/**
 * drm_gem_fb_afbc_init() - Helper function for drivers using afbc to
 *			    fill and validate all the afbc-specific
 *			    struct drm_afbc_framebuffer members
 *
 * @dev: DRM device
 * @afbc_fb: afbc-specific framebuffer
 * @mode_cmd: Metadata from the userspace framebuffer creation request
 * @afbc_fb: afbc framebuffer
 *
 * This function can be used by drivers which support afbc to complete
 * the preparation of struct drm_afbc_framebuffer. It must be called after
 * allocating the said struct and calling drm_gem_fb_init_with_funcs().
 * It is caller's responsibility to put afbc_fb->base.obj objects in case
 * the call is unsuccessful.
 *
 * Returns:
 * Zero on success or a negative error value on failure.
 */
int drm_gem_fb_afbc_init(struct drm_device *dev,
			 const struct drm_mode_fb_cmd2 *mode_cmd,
			 struct drm_afbc_framebuffer *afbc_fb)
{
	const struct drm_format_info *info;
	struct drm_gem_object **objs;
	int ret;

	objs = afbc_fb->base.obj;
	info = drm_get_format_info(dev, mode_cmd);
	if (!info)
		return -EINVAL;

	ret = drm_gem_afbc_min_size(dev, mode_cmd, afbc_fb);
	if (ret < 0)
		return ret;

	if (objs[0]->size < afbc_fb->afbc_size)
		return -EINVAL;

	return 0;
}
EXPORT_SYMBOL_GPL(drm_gem_fb_afbc_init);
