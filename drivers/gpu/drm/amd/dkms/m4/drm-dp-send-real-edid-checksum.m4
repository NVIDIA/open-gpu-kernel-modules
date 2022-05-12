dnl #
dnl # commit e11f5bd8228fc3760c221f940b9f6365dbf3e7ed
dnl # drm: Add support for DP 1.4 Compliance edid corruption test
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DP_SEND_REAL_EDID_CHECKSUM], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <drm/drm_dp_helper.h>
		], [
			drm_dp_send_real_edid_checksum(NULL, 0);
		], [drm_dp_send_real_edid_checksum], [drivers/gpu/drm/drm_dp_helper.c], [
			AC_DEFINE(HAVE_DRM_DP_SEND_REAL_EDID_CHECKSUM, 1,
				[drm_dp_send_real_edid_checksum() is available])
		])
	])
])
