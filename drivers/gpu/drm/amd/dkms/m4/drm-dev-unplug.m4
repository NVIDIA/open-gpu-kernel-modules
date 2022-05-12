dnl #
dnl # commit v4.13-rc2-388-gc07dcd61a0e5
dnl # drm: Document device unplug infrastructure
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DEV_UNPLUG], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#ifdef HAVE_DRM_DRMP_H
			struct vm_area_struct;
			#include <drm/drmP.h>
			#endif
			#ifdef HAVE_DRM_DRM_DRV_H
			#include <drm/drm_drv.h>
			#endif
		], [
			drm_dev_unplug(NULL);
		], [drm_dev_unplug],[drivers/gpu/drm/drm_drv.c],[
			AC_DEFINE(HAVE_DRM_DEV_UNPLUG, 1,
				[drm_dev_unplug() is available])
		])
	])
])
