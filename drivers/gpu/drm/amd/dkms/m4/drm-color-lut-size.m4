dnl #
dnl # v4.16-rc1-483-g41204dfeed93 drm: Introduce drm_color_lut_size()
dnl # v4.5-rc3-706-g5488dc16fde7 drm: introduce pipe color correction properties
dnl #
AC_DEFUN([AC_AMDGPU_DRM_COLOR_LUT_SIZE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_mode.h>
			#include <drm/drm_property.h>
			struct drm_crtc;
			#include <drm/drm_color_mgmt.h>
		], [
			struct drm_property_blob blob;
			drm_color_lut_size(&blob);
		], [
			AC_DEFINE(HAVE_DRM_COLOR_LUT_SIZE, 1,
				[drm_color_lut_size() is available])
		])
	])
])
