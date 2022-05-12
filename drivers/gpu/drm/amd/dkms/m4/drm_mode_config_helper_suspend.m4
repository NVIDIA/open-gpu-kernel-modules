dnl #
dnl # v4.14-rc7-1626-gca038cfb5cfa
dnl # drm/modeset-helper: Add simple modeset suspend/resume helpers
dnl #
AC_DEFUN([AC_AMDGPU_DRM_MODE_CONFIG_HELPER_SUSPEND], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([drm_mode_config_helper_suspend drm_mode_config_helper_resume],
		[drivers/gpu/drm/drm_modeset_helper.c],[
			AC_DEFINE(HAVE_DRM_MODE_CONFIG_HELPER_SUSPEND, 1,
				[drm_mode_config_helper_{suspend/resume}() is available])
		])
	])
])
