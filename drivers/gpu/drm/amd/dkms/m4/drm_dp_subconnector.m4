dnl #
dnl # v5.8-rc2-671-ge5b92773287c drm: report dp downstream port type as a subconnector property
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DP_SUBCONNECTOR], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_mode_config.h>
			#include <drm/drm_crtc.h>
		], [
			struct drm_mode_config *mode_config = NULL;
			mode_config->dp_subconnector_property = NULL;
		], [
			AC_DEFINE(HAVE_DRM_MODE_CONFIG_DP_SUBCONNECTOR_PROPERTY, 1,
				[drm_mode_config->dp_subconnector_property is available])
		], [
			AC_KERNEL_TRY_COMPILE([
				#include <drm/drm_crtc.h>
			], [
				enum drm_mode_subconnector sub = 0;
			], [
				AC_DEFINE(HAVE_DRM_MODE_SUBCONNECTOR_ENUM, 1,
					[enum drm_mode_subconnector is available])
			])
		])
	])
])
