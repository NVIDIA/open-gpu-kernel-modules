dnl #
dnl # v5.9-rc5-1367-g564b9f4c7cf9
dnl # drm/amd/display: Add formats for DCC with 2/3 planes
dnl #
AC_DEFUN([AC_AMDGPU_DRM_FORMAT_INFO], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_fourcc.h>
		], [
			struct drm_format_info format = {
			    .format = DRM_FORMAT_XRGB16161616F,
			    .block_w = {0},
			    .block_h = {0},
			};
		], [
			AC_DEFINE(HAVE_DRM_FORMAT_INFO_MODIFIER_SUPPORTED, 1,
				[drm_format_info.block_w and rm_format_info.block_h is available])
		])
	])
])
