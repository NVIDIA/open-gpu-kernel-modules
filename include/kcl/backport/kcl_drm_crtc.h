/* SPDX-License-Identifier: MIT */
#ifndef KCL_BACKPORT_KCL_DRM_CRTC_H
#define KCL_BACKPORT_KCL_DRM_CRTC_H

#include <drm/drm_crtc.h>
#include <kcl/kcl_drm_crtc.h>

#if !defined(HAVE_DRM_CRTC_INIT_WITH_PLANES_VALID_WITH_NAME)
static inline
int _kcl_drm_crtc_init_with_planes(struct drm_device *dev, struct drm_crtc *crtc,
			      struct drm_plane *primary,
			      struct drm_plane *cursor,
			      const struct drm_crtc_funcs *funcs,
			      const char *name, ...)
{
	return drm_crtc_init_with_planes(dev, crtc, primary, cursor, funcs);
}
#define drm_crtc_init_with_planes _kcl_drm_crtc_init_with_planes
#endif

#if DRM_VERSION_CODE == DRM_VERSION(4, 10, 0) && defined(OS_NAME_RHEL_7_4)
#define AMDKCL_WORKAROUND_DRM_4_10_0_RHEL_7_4
#endif

#endif
