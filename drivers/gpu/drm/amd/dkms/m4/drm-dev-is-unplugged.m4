dnl #
dnl # commit bee330f3d67273a68dcb99f59480d59553c008b2
dnl # drm: Use srcu to protect drm_device.unplugged
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DEV_IS_UNPLUGGED], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#ifdef HAVE_DRM_DRMP_H
			#include <drm/drmP.h>
			#endif
			#ifdef HAVE_DRM_DRM_DRV_H
			#include <drm/drm_drv.h>
			#endif
		], [
			drm_dev_is_unplugged(NULL);
		], [
			AC_DEFINE(HAVE_DRM_DEV_IS_UNPLUGGED, 1,
				[drm_dev_is_unplugged() is available])
		])
	])
])
