dnl #
dnl # commit v4.11-rc1-237-g6a0f9ebfc5e7
dnl # drm: Add mode_config .get_format_info() hook
dnl #
AC_DEFUN([AC_AMDGPU_DRM_GET_FORMAT_INFO], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([drm_get_format_info],
			[drivers/gpu/drm/drm_fourcc.c], [
			AC_DEFINE(HAVE_DRM_GET_FORMAT_INFO, 1,
				[drm_get_format_info() is available])
		])
	])
])
