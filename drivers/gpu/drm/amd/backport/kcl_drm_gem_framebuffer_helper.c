// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * drm gem framebuffer helper functions
 *
 * Copyright (C) 2017 Noralf Trønnes
 */

#include <kcl/kcl_amdgpu_drm_gem_framebuffer_helper.h>

/* Copied from drivers/gpu/drm/drm_gem_framebuffer_helper.c and modified for KCL */
#ifndef HAVE_DRM_DRM_GEM_FRAMEBUFFER_HELPER_H
struct drm_gem_object *drm_gem_fb_get_obj(struct drm_framebuffer *fb,
					  unsigned int plane)
{
	struct amdgpu_framebuffer *afb = to_amdgpu_framebuffer(fb);
	(void)plane; /* for compile un-used warning */
	if (afb)
		return afb->obj;
	else
		return NULL;
}

void drm_gem_fb_destroy(struct drm_framebuffer *fb)
{
	struct amdgpu_framebuffer *amdgpu_fb = to_amdgpu_framebuffer(fb);

	drm_gem_object_put(amdgpu_fb->obj);

	drm_framebuffer_cleanup(fb);
	kfree(fb);
}

int drm_gem_fb_create_handle(struct drm_framebuffer *fb, struct drm_file *file,
			     unsigned int *handle)
{
	struct amdgpu_framebuffer *amdgpu_fb = to_amdgpu_framebuffer(fb);

	return drm_gem_handle_create(file, amdgpu_fb->obj, handle);
}
#endif
