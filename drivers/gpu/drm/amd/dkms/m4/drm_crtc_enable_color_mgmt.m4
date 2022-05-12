dnl #
dnl # v4.8-rc2-802-gf1e2f66ce2d9 drm: Extract drm_color_mgmt.[hc]
dnl # v4.7-rc2-68-gf8ed34ac7b45 drm: drm_helper_crtc_enable_color_mgmt() => drm_crtc_enable_color_mgmt()
dnl #
AC_DEFUN([AC_AMDGPU_DRM_CRTC_ENABLE_COLOR_MGMT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([drm_crtc_enable_color_mgmt],
			[drivers/gpu/drm/drm_color_mgmt.c drivers/gpu/drm/drm_crtc.c], [
			AC_DEFINE(HAVE_DRM_CRTC_ENABLE_COLOR_MGMT, 1,
				[drm_crtc_enable_color_mgmt() is available])
		])
	])
])
