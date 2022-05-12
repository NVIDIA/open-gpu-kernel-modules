dnl f1781e9bb2dd2305d8d7ffbede1888ae22119557
dnl # drm/edid: Allow HDMI infoframe without VIC or S3D
dnl #
AC_DEFUN([AC_AMDGPU_DRM_HDMI_VENDOR_INFOFRAME_FROM_DISPLAY_MODE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <drm/drm_edid.h>
		], [
			drm_hdmi_vendor_infoframe_from_display_mode(NULL, NULL, NULL);
		], [drm_hdmi_vendor_infoframe_from_display_mode], [drivers/gpu/drm/drm_edid.c], [
			AC_DEFINE(HAVE_DRM_HDMI_VENDOR_INFOFRAME_FROM_DISPLAY_MODE_P_P_P, 1,
				[drm_hdmi_vendor_infoframe_from_display_mode() has p,p,p interface])
		])
	])
])
