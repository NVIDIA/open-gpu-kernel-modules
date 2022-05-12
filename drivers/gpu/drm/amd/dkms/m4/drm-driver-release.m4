dnl #
dnl # commit v4.10-rc5-1045-gf30c92576af4
dnl # drm: Provide a driver hook for drm_dev_release()
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DRIVER_RELEASE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			struct vm_area_struct;
			#ifdef HAVE_DRM_DRM_DRV_H
			#include <drm/drm_drv.h>
			#else
			#include <drm/drmP.h>
			#endif
		],[
			struct drm_driver *ddrv = NULL;
			ddrv->release = NULL;
		],[
			AC_DEFINE(HAVE_DRM_DRIVER_RELEASE, 1,
				[drm_driver->release() is available])
		])
	])
])
