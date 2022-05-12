dnl #
dnl # commit v4.10-rc8-1384-g79c1da7c3bf7
dnl # drm/dp: add helpers for capture of frame CRCs
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DP_START_CRC], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT(
			[drm_dp_start_crc drm_dp_stop_crc],
			[drivers/gpu/drm/drm_dp_helper.c],
			[AC_DEFINE(HAVE_DRM_DP_START_CRC, 1,
				[drm_dp_start_crc() is available])]
		)
	])
])
