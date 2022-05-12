dnl #
dnl # commit v4.6-rc3-418-g9f0ba539d13a
dnl # drm/gem: support BO freeing without dev->struct_mutex
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DRIVER_GEM_FREE_OBJECT_UNLOCKED], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#ifdef HAVE_DRM_DRMP_H
			struct vm_area_struct;
			#include <drm/drmP.h>
			#else
			#include <drm/drm_drv.h>
			#endif
		],[
			struct drm_driver *ddrv = NULL;
			ddrv->gem_free_object_unlocked = NULL;
		],[
			AC_DEFINE(HAVE_GEM_FREE_OBJECT_UNLOCKED_IN_DRM_DRIVER, 1,
				[drm_driver->gem_free_object_unlocked() is available])
		])
	])
])
