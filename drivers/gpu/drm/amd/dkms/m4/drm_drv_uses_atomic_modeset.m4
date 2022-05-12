dnl #
dnl # commit v4.9-rc8-1697-ga743d7582db9
dnl # drm: Wrap the check for atomic_commit implementation
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DRV_USES_ATOMIC_MODESET], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#ifdef HAVE_DRM_DRMP_H
			#include <drm/drmP.h>
			#endif
			#ifdef HAVE_DRM_DRM_DRV_H
			#include <drm/drm_drv.h>
			#endif
		], [
			drm_drv_uses_atomic_modeset(0);
		],[
			AC_DEFINE(HAVE_DRM_DRV_USES_ATOMIC_MODESET, 1,
				[drm_drv_uses_atomic_modeset() is available])
		])
	])
])
