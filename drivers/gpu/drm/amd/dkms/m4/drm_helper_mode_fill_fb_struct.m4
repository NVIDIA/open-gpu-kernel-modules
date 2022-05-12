dnl #
dnl # v4.9-rc8-1647-g95bce7601581 drm: Populate fb->dev from drm_helper_mode_fill_fb_struct()
dnl # v4.9-rc8-1643-ga3f913ca9892 drm: Pass 'dev' to drm_helper_mode_fill_fb_struct()
dnl #
AC_DEFUN([AC_AMDGPU_DRM_HELPER_MODE_FILL_FB_STRUCT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_crtc_helper.h>
		], [
			drm_helper_mode_fill_fb_struct(NULL, NULL, NULL);
		], [
			AC_DEFINE(HAVE_DRM_HELPER_MODE_FILL_FB_STRUCT_DEV, 1,
				[drm_helper_mode_fill_fb_struct() wants dev arg])
		])
	])
])
