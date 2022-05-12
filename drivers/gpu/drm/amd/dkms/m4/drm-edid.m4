dnl #
dnl # 13d0add333afea7b2fef77473232b10dea3627dd
dnl # drm/edid: Pass connector to AVI infoframe functions
dnl #
AC_DEFUN([AC_AMDGPU_DRM_EDID], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <drm/drm_edid.h>
		], [
			struct hdmi_avi_infoframe *frame = NULL;
			struct drm_connector *connector = NULL;
			const struct drm_display_mode *mode = NULL;
			drm_hdmi_avi_infoframe_from_display_mode(frame, connector, mode);
		], [drm_hdmi_avi_infoframe_from_display_mode], [drivers/gpu/drm/drm_edid.c], [
			AC_DEFINE(HAVE_DRM_HDMI_AVI_INFOFRAME_FROM_DISPLAY_MODE_P_P_P, 1,
				[drm_hdmi_avi_infoframe_from_display_mode() has p,p,p interface])
		], [
		dnl #
		dnl # 10a8512008655d5ce62f8c56323a6b5bd221c920
		dnl # drm: Add HDMI infoframe helpers
		dnl #
			AC_KERNEL_TRY_COMPILE_SYMBOL([
				#include <drm/drm_edid.h>
			], [
				struct hdmi_avi_infoframe *frame = NULL;
				const struct drm_display_mode *mode = NULL;
				bool is_hdmi2_sink = false;
				drm_hdmi_avi_infoframe_from_display_mode(frame, mode, is_hdmi2_sink);
			], [drm_hdmi_avi_infoframe_from_display_mode], [drivers/gpu/drm/drm_edid.c], [
				AC_DEFINE(HAVE_DRM_HDMI_AVI_INFOFRAME_FROM_DISPLAY_MODE_P_P_B, 1,
					[drm_hdmi_avi_infoframe_from_display_mode() has p,p,b interface])
			])
		])
	])
])
