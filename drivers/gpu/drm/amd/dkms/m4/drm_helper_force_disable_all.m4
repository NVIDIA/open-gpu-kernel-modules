dnl #
dnl # commit f453ba0460742ad027ae0c4c7d61e62817b3e7ef
dnl # DRM: add mode setting support
dnl #
dnl # commit v5.0-rc1-118-gc2d88e06bcb9
dnl # drm: Move the legacy kms disable_all helper to crtc helpers
dnl #
AC_DEFUN([AC_AMDGPU_DRM_HELPER_FORCE_DISABLE_ALL], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([drm_helper_force_disable_all], [drivers/gpu/drm/drm_crtc_helper.c],[
			AC_DEFINE(HAVE_DRM_HELPER_FORCE_DISABLE_ALL, 1,
				[drm_helper_force_disable_all() is available])
		])
	])
])

