dnl #
dnl # commit 913b2cb727b7a47ccf8842d54c89f1b873c6deed
dnl # drm: change func to better detect wether swiotlb is needed
dnl #
AC_DEFUN([AC_AMDGPU_DRM_CACHE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_cache.h>
		], [
			drm_need_swiotlb(0);
		], [
			AC_DEFINE(HAVE_DRM_NEED_SWIOTLB, 1,
				[drm_need_swiotlb() is availablea])
		])
	])
])
