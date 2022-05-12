dnl #
dnl # commit v4.9-rc8-1739-g6d1b81d8e25d
dnl # drm: add crtc helper drm_crtc_from_index()
dnl #
AC_DEFUN([AC_AMDGPU_DRM_CRTC_FROM_INDEX], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_crtc.h>
		],[
			drm_crtc_from_index(NULL, 0);
		],[
			AC_DEFINE(HAVE_DRM_CRTC_FROM_INDEX,
				1,
				[drm_crtc_from_index() is available])
		])
	])
])
