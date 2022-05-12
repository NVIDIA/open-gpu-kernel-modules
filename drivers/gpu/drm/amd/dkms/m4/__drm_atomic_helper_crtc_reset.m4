dnl #
dnl # v5.1-rc2-1163-g7d26097b4beb
dnl # drm/atomic: Create __drm_atomic_helper_crtc_reset() for subclassing crtc_state.
dnl #
AC_DEFUN([AC_AMDGPU___DRM_ATOMIC_HELPER_CRTC_RESET], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([__drm_atomic_helper_crtc_reset],
		[drivers/gpu/drm/drm_atomic_state_helper.c],
		[
			AC_DEFINE(HAVE___DRM_ATOMIC_HELPER_CRTC_RESET, 1,
				[__drm_atomic_helper_crtc_reset() is available])
		])
	])
])
