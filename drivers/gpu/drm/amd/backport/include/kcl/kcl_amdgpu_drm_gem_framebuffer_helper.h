#ifndef __AMDGPU_BACKPORT_KCL_AMDGPU_DRM_GEM_FRAMEBUFFER_HELPER_H__
#define __AMDGPU_BACKPORT_KCL_AMDGPU_DRM_GEM_FRAMEBUFFER_HELPER_H__

#include <drm/drm_gem_framebuffer_helper.h>
#include "amdgpu.h"

static inline
void kcl_drm_gem_fb_set_obj(struct drm_framebuffer *fb, int index, struct drm_gem_object *obj)
{
#ifdef HAVE_DRM_DRM_GEM_FRAMEBUFFER_HELPER_H
	if (fb)
		fb->obj[index] = obj;
#else
	struct amdgpu_framebuffer *afb = to_amdgpu_framebuffer(fb);
	(void)index; /* for compile un-used warning */
	if (afb)
		afb->obj = obj;
#endif
}

#ifndef HAVE_DRM_DRM_GEM_FRAMEBUFFER_HELPER_H
/* Copied from include/drm/drm_gem_framebuffer_helper.h */
struct drm_gem_object *drm_gem_fb_get_obj(struct drm_framebuffer *fb,
					  unsigned int plane);
void drm_gem_fb_destroy(struct drm_framebuffer *fb);
int drm_gem_fb_create_handle(struct drm_framebuffer *fb, struct drm_file *file,
			     unsigned int *handle);
#endif

#endif
