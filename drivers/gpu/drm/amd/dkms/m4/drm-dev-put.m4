dnl #
dnl # commit 9a96f55034e41b4e002b767e9218d55f03bdff7d
dnl # drm: introduce drm_dev_{get/put} functions
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DEV_SUPPORTED], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#ifdef HAVE_DRM_DRMP_H
			struct vm_area_struct;
			#include <drm/drmP.h>
			#else
			#include <drm/drm_drv.h>
			#endif
		], [
			drm_dev_put(NULL);
		], [drm_dev_put],[drivers/gpu/drm/drm_drv.c],[
			AC_DEFINE(HAVE_DRM_DEV_PUT, 1,
				[drm_dev_put() is available])
		])
	])
])
