dnl #
dnl # v4.19-rc5-1020-g9ef8a9dc4b21 drm: Extract drm_atomic_state_helper.[hc]
dnl # v4.6-rc7-993-gec2dc6a0fe38 drm: Drop crtc argument from __drm_atomic_helper_crtc_destroy_state
dnl #
AC_DEFUN([AC_AMDGPU___DRM_ATOMIC_HELPER_CRTC_DESTROY_STATE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_atomic_helper.h>
		], [
			__drm_atomic_helper_crtc_destroy_state(NULL);
		], [
			AC_DEFINE(HAVE___DRM_ATOMIC_HELPER_CRTC_DESTROY_STATE_P, 1,
				[__drm_atomic_helper_crtc_destroy_state() wants 1 arg])
		])
	])
])
