dnl #
dnl # commit v4.14-rc3-594-g79436a1c9bcc
dnl # drm/edid: make drm_edid_to_eld() static
dnl #
dnl # commit v3.1-rc6-139-g76adaa34db40
dnl # drm: support routines for HDMI/DP ELD
dnl #
AC_DEFUN([AC_AMDGPU_DRM_EDID_TO_ELD], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <drm/drm_edid.h>
		], [
			drm_edid_to_eld(NULL, NULL);
		], [drm_edid_to_eld], [drivers/gpu/drm/drm_edid.c], [
			AC_DEFINE(HAVE_DRM_EDID_TO_ELD, 1,
				[drm_edid_to_eld() are available])
		])
	])
])
