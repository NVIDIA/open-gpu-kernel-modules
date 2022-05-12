dnl #
dnl # commit v5.2-rc2-494-ge5852bee90d6
dnl # drm/fb-helper: Remove drm_fb_helper_connector
dnl #
AC_DEFUN([AC_AMDGPU_DRM_FB_HELPER_ADD_REMOVE_CONNECTOR], [
	AC_KERNEL_DO_BACKGROUND([
		dnl #
		dnl # commit v2.6.34-rc2-85-g0b4c0f3f0ece
		dnl # drm/kms/fb: separate fbdev connector list from core drm connectors
		dnl #
		dnl # commit v3.16-rc4-40-g65c2a89c30ed
		dnl # drm/fb_helper: allow adding/removing connectors later
		dnl #
		AC_KERNEL_CHECK_SYMBOL_EXPORT([drm_fb_helper_single_add_all_connectors drm_fb_helper_remove_one_connector],
			[drivers/gpu/drm/drm_fb_helper.c],[
			AC_DEFINE(HAVE_DRM_FB_HELPER_ADD_REMOVE_CONNECTORS, 1,
				[drm_fb_helper_single_add_all_connectors() && drm_fb_helper_remove_one_connector() are symbol])
		])
	])
])
