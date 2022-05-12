dnl #
dnl # commit v4.14-rc3-721-g67680d3c0464
dnl # drm: vblank: use ktime_t instead of timeval
dnl #
AC_DEFUN([AC_AMDGPU_DRM_CALC_VBLTIMESTAMP_FROM_SCANOUTPOS], [
	AC_KERNEL_DO_BACKGROUND([
	AC_KERNEL_TRY_COMPILE([
		#ifdef HAVE_DRM_DRMP_H
		struct vm_area_struct;
		#include <drm/drmP.h>
		#else
		#include <drm/drm_drv.h>
		#include <drm/drm_vblank.h>
		#endif
	], [
		struct drm_driver *kms_driver = NULL;
		bool (*get_vblank_timestamp) (struct drm_device *dev, unsigned int pipe,
							 int *max_error,
							 ktime_t *vblank_time,
							 bool in_vblank_irq);
		drm_calc_vbltimestamp_from_scanoutpos(NULL, 0, NULL, (ktime_t *)NULL, 0);
		kms_driver->get_vblank_timestamp = get_vblank_timestamp;
	], [
		AC_DEFINE(HAVE_DRM_DRIVER_GET_VBLANK_TIMESTAMP_USE_KTIMER_T_ARG, 1,
				[drm_calc_vbltimestamp_from_scanoutpos() use ktime_t arg])
		AC_DEFINE(HAVE_DRM_DRIVER_GET_SCANOUT_POSITION_RETURN_BOOL, 1,
					[drm_driver->get_scanout_position() return bool])
	], [
		dnl #
		dnl # v4.11-rc7-1902-g1bf6ad622b9b drm/vblank: drop the mode argument from drm_calc_vbltimestamp_from_scanoutpos
		dnl # v4.11-rc7-1900-g3fcdcb270936 drm/vblank: Switch to bool in_vblank_irq in get_vblank_timestamp
		dnl # v4.11-rc7-1899-gd673c02c4bdb drm/vblank: Switch drm_driver->get_vblank_timestamp to return a bool
		dnl #
		AC_KERNEL_TRY_COMPILE([
			struct vm_area_struct;
			#include <drm/drmP.h>
		], [
			struct drm_driver *kms_driver = NULL;
			bool (*get_scanout_position) (struct drm_device *dev, unsigned int pipe,
                              bool in_vblank_irq, int *vpos, int *hpos,
                              ktime_t *stime, ktime_t *etime,
                              const struct drm_display_mode *mode);
			bool (*get_vblank_timestamp) (struct drm_device *dev, unsigned int pipe,
                             int *max_error,
                             struct timeval *vblank_time,
                             bool in_vblank_irq);
			kms_driver->get_scanout_position = get_scanout_position;
			kms_driver->get_vblank_timestamp = get_vblank_timestamp;
			drm_calc_vbltimestamp_from_scanoutpos(NULL, 0, NULL, (struct timeval *)NULL, 0);
		], [
			AC_DEFINE(HAVE_DRM_DRIVER_GET_SCANOUT_POSITION_RETURN_BOOL, 1,
					[drm_driver->get_scanout_position() return bool])
			AC_DEFINE(HAVE_DRM_DRIVER_GET_VBLANK_TIMESTAMP_RETURN_BOOL, 1,
					[drm_driver->get_vblank_timestamp() return bool])
		])
	])
	])
])
