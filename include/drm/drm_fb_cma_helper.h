/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __DRM_FB_CMA_HELPER_H__
#define __DRM_FB_CMA_HELPER_H__

#include <linux/types.h>

struct drm_device;
struct drm_framebuffer;
struct drm_plane_state;

struct drm_gem_cma_object *drm_fb_cma_get_gem_obj(struct drm_framebuffer *fb,
	unsigned int plane);

dma_addr_t drm_fb_cma_get_gem_addr(struct drm_framebuffer *fb,
				   struct drm_plane_state *state,
				   unsigned int plane);

void drm_fb_cma_sync_non_coherent(struct drm_device *drm,
				  struct drm_plane_state *old_state,
				  struct drm_plane_state *state);

#endif

