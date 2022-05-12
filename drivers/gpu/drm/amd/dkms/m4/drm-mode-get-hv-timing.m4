dnl #
dnl # commit v4.10-rc3-530-g196cd5d3758c
dnl # drm: s/drm_crtc_get_hv_timings/drm_mode_get_hv_timings/
dnl #
AC_DEFUN([AC_AMDGPU_DRM_MODE_GET_HV_TIMING], [
	AC_KERNEL_CHECK_SYMBOL_EXPORT(
		[drm_mode_get_hv_timing],
		[drivers/gpu/drm/drm_modes.c],[
		AC_DEFINE(HAVE_DRM_MODE_GET_HV_TIMING, 1,
			[drm_mode_get_hv_timing is available])
	])
])
