dnl #
dnl # commit v4.10-rc8-1380-g2107777c0249
dnl # drm/atomic: Add macros to access existing old/new state, v2.
dnl #
AC_DEFUN([AC_AMDGPU_DRM_ATOMIC_GET_CRTC_STATE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_atomic.h>
		], [
			struct drm_atomic_state *state = NULL;
			struct drm_crtc *crtc = NULL;
			struct drm_crtc_state *ret;

			ret = drm_atomic_get_old_crtc_state(state, crtc);
			ret = drm_atomic_get_new_crtc_state(state, crtc);
		], [
			AC_DEFINE(HAVE_DRM_ATOMIC_GET_CRTC_STATE, 1,
				[drm_atomic_get_old_crtc_state() and drm_atomic_get_new_crtc_state() are available])
		], [
			dnl #
			dnl # v4.7-rc1-237-g5d943aa6c0d4
			dnl # drm: Consolidate crtc arrays in drm_atomic_state
			dnl #
			AC_KERNEL_TRY_COMPILE([
				#include <drm/drm_atomic.h>
			], [
				struct drm_atomic_state *state = NULL;
				struct drm_crtc *crtc = NULL;

				state->crtcs[drm_crtc_index(crtc)].ptr = NULL;
			], [
				AC_DEFINE(HAVE_DRM_CRTCS_STATE_MEMBER, 1,
					[ddrm_atomic_stat has __drm_crtcs_state])
			])
		])
	])
])
