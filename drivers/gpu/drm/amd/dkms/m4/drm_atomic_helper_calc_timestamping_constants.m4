dnl #
dnl # v5.9-rc5-1595-ge1ad957d45f7
dnl # Extract drm_atomic_helper_calc_timestamping_constants()
dnl #
AC_DEFUN([AC_AMDGPU_DRM_ATOMIC_HELPER_CALC_TIMESTAMPING_CONSTANTS], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([drm_atomic_helper_calc_timestamping_constants],
			[drivers/gpu/drm/drm_atomic_helper.c], [
			AC_DEFINE(HAVE_DRM_ATOMIC_HELPER_CALC_TIMESTAMPING_CONSTANTS, 1,
				[drm_atomic_helper_calc_timestamping_constants() is available])
		])
	])
])
