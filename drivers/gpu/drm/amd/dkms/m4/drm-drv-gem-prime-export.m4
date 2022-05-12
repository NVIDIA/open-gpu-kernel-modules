dnl #
dnl # commit v5.2-rc5-870-ge4fa8457b219
dnl # drm/prime: Align gem_prime_export with obj_funcs.export
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DRV_GEM_PRIME_EXPORT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_prime.h>
		],[
			drm_gem_prime_export(NULL, 0);
		],[
			AC_DEFINE(HAVE_DRM_DRV_GEM_PRIME_EXPORT_PI, 1,
				[drm_gem_prime_export() with p,i arg is available])
		])
	])
])
