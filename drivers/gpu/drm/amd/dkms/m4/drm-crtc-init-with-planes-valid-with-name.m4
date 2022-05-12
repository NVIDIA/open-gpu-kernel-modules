dnl #
dnl # commit v4.4-rc4-276-gf98828769c88
dnl # drm: Pass 'name' to drm_crtc_init_with_planes()
dnl #
AC_DEFUN([AC_AMDGPU_DRM_CRTC_INIT_WITH_PLANES_VALID_WITH_NAME], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_crtc.h>
		], [
			int error;
			error = drm_crtc_init_with_planes(NULL, NULL, NULL, NULL, NULL, NULL);
		], [
			AC_DEFINE(HAVE_DRM_CRTC_INIT_WITH_PLANES_VALID_WITH_NAME, 1,
				[drm_crtc_init_with_planes() wants name])
		])
	])
])
