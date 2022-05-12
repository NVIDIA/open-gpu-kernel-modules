dnl #
dnl # commit v4.6-rc3-372-g1d2ac403ae3b
dnl # drm: Protect dev->filelist with its own mutex
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DEVICE_FILELIST_MUTEX], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#ifdef HAVE_DRM_DRM_DEVICE_H
			#include <drm/drm_device.h>
			#else
			#include <drm/drmP.h>
			#endif
		],[
			struct drm_device *ddev;
			ddev->filelist_mutex = ddev->filelist_mutex;
		],[
			AC_DEFINE(HAVE_DRM_DEVICE_FILELIST_MUTEX, 1,
				[drm_device->filelist_mutex is available])
		])
	])
])
