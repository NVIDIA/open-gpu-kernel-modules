dnl #
dnl # commit b7e32bef4ae5f9149276203564b7911fac466588
dnl # drm: Add a prefetching memcpy_from_wc
dnl #
AC_DEFUN([AC_AMDGPU_DRM_MEMCPY_FROM_WC], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_cache.h>
		], [
			drm_memcpy_from_wc(NULL, NULL, 0);
		], [
			AC_DEFINE(HAVE_DRM_MEMCPY_FROM_WC, 1,
				[drm_memcpy_from_wc() is availablea])
		])
	])
])
