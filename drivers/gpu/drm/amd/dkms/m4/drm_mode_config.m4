dnl #
dnl # v4.10-rc5-1093-ga4b10ccead4d drm: Constify drm_mode_config atomic helper private pointer
dnl # v4.9-rc4-840-g28575f165d36 drm: Extract drm_mode_config.[hc]
dnl # v4.7-rc2-454-g9f2a7950e77a drm/atomic-helper: nonblocking commit support
dnl #
AC_DEFUN([AC_AMDGPU_DRM_MODE_CONFIG], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_mode_config.h>
			#include <drm/drm_crtc.h>
		], [
			struct drm_mode_config *mode_config = NULL;
			mode_config->helper_private = NULL;
		], [
			AC_DEFINE(HAVE_DRM_MODE_CONFIG_HELPER_PRIVATE, 1,
				[drm_mode_config->helper_private is available])
			AC_DEFINE(HAVE_DRM_NONBLOCKING_COMMIT_SUPPORT, 1,
				[drm atomic nonblocking commit support is available])
		])
	])
])
