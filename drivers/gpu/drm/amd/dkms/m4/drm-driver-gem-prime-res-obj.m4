dnl #
dnl # commit v5.3-rc1-325-g51c98747113e
dnl # drm/amdgpu: Fill out gem_object->resv
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DRIVER_GEM_PRIME_RES_OBJ], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#ifdef HAVE_DRM_DRMP_H
			struct vm_area_struct;
			#include <drm/drmP.h>
			#else
			#include <drm/drm_drv.h>
			#endif
		], [
			struct drm_driver *drv = NULL;
			drv->gem_prime_res_obj(NULL);
		], [
			AC_DEFINE(HAVE_DRM_DRIVER_GEM_PRIME_RES_OBJ, 1,
				[drm_driver->gem_prime_res_obj() is available])
		])
	])
])
