/* SPDX-License-Identifier: MIT */
#ifndef AMDGPU_BACKPORT_KCL_AMDGPU_H
#define AMDGPU_BACKPORT_KCL_AMDGPU_H

#include <drm/drm_vblank.h>
#include "amdgpu.h"
#include <drm/drm_drv.h>

#ifndef HAVE_STRUCT_DRM_CRTC_FUNCS_GET_VBLANK_TIMESTAMP
static inline u32 kcl_amdgpu_get_vblank_counter_kms(struct drm_device *dev, unsigned int crtc)
{
	struct drm_crtc *drm_crtc = drm_crtc_from_index(dev, crtc);

	return amdgpu_get_vblank_counter_kms(drm_crtc);
}

static inline int kcl_amdgpu_enable_vblank_kms(struct drm_device *dev, unsigned int crtc)
{
	struct drm_crtc *drm_crtc = drm_crtc_from_index(dev, crtc);

	return amdgpu_enable_vblank_kms(drm_crtc);
}

static inline void kcl_amdgpu_disable_vblank_kms(struct drm_device *dev, unsigned int crtc)
{
	struct drm_crtc *drm_crtc = drm_crtc_from_index(dev, crtc);

	return amdgpu_disable_vblank_kms(drm_crtc);
}

#if defined(HAVE_DRM_DRIVER_GET_SCANOUT_POSITION_RETURN_BOOL)
static inline bool kcl_amdgpu_get_crtc_scanout_position(struct drm_device *dev, unsigned int pipe,
				 bool in_vblank_irq, int *vpos, int *hpos,
				 ktime_t *stime, ktime_t *etime,
				 const struct drm_display_mode *mode)
{
	return !!amdgpu_display_get_crtc_scanoutpos(dev, pipe, in_vblank_irq, vpos, hpos, stime, etime, mode);
}
#else
static inline int kcl_amdgpu_get_crtc_scanout_position(struct drm_device *dev, unsigned int pipe,
				   unsigned int flags, int *vpos, int *hpos,
				   ktime_t *stime, ktime_t *etime,
				   const struct drm_display_mode *mode)
{
	return amdgpu_display_get_crtc_scanoutpos(dev, pipe, flags, vpos, hpos, stime, etime, mode);
}
#endif

#if defined(HAVE_DRM_DRIVER_GET_VBLANK_TIMESTAMP_USE_KTIMER_T_ARG)
static inline bool kcl_amdgpu_get_vblank_timestamp_kms(struct drm_device *dev, unsigned int pipe,
					int *max_error,	ktime_t *vblank_time,
					bool in_vblank_irq)
{
	return drm_calc_vbltimestamp_from_scanoutpos(dev, pipe, max_error, vblank_time, in_vblank_irq);
}
#elif defined(HAVE_DRM_DRIVER_GET_VBLANK_TIMESTAMP_RETURN_BOOL)
static inline bool kcl_amdgpu_get_vblank_timestamp_kms(struct drm_device *dev, unsigned int pipe,
						int *max_error, struct timeval *vblank_time,
						bool in_vblank_irq)
{
	return drm_calc_vbltimestamp_from_scanoutpos(dev, pipe, max_error, vblank_time, in_vblank_irq);
}
#else
static inline int kcl_amdgpu_get_vblank_timestamp_kms(struct drm_device *dev, unsigned int pipe,
					int *max_error,	struct timeval *vblank_time,
					unsigned flags)
{
	struct drm_crtc *crtc;
	struct amdgpu_device *adev = drm_to_adev(dev);

	if (pipe >= dev->num_crtcs) {
		DRM_ERROR("Invalid crtc %u\n", pipe);
		return -EINVAL;
	}

	/* Get associated drm_crtc: */
	crtc = &adev->mode_info.crtcs[pipe]->base;
	if (!crtc) {
		/* This can occur on driver load if some component fails to
		 * initialize completely and driver is unloaded */
		DRM_ERROR("Uninitialized crtc %d\n", pipe);
		return -EINVAL;
	}

	return drm_calc_vbltimestamp_from_scanoutpos(dev, pipe, max_error,
						vblank_time, flags,
						&crtc->hwmode);
}
#endif /* HAVE_DRM_DRIVER_GET_VBLANK_TIMESTAMP_USE_KTIMER_T_ARG */
#endif /* HAVE_STRUCT_DRM_CRTC_FUNCS_GET_VBLANK_TIMESTAMP */

#if defined(HAVE_DRM_VBLANK_USE_KTIME_T)
static inline ktime_t kcl_amdgpu_get_vblank_time_ns(struct drm_vblank_crtc *vblank)
{
	return vblank->time;
}
#elif defined(HAVE_DRM_VBLANK_CRTC_HAS_ARRAY_TIME_FIELD)
static inline ktime_t kcl_amdgpu_get_vblank_time_ns(struct drm_vblank_crtc *vblank)
{
	return timeval_to_ktime(vblank->time[(vblank->count) % DRM_VBLANKTIME_RBSIZE]);
}
#else
static inline ktime_t kcl_amdgpu_get_vblank_time_ns(struct drm_vblank_crtc *vblank)
{
	return timeval_to_ktime(vblank->time);
}
#endif /* HAVE_DRM_VBLANK_USE_KTIME_T */

#endif /* AMDGPU_BACKPORT_KCL_AMDGPU_H */
