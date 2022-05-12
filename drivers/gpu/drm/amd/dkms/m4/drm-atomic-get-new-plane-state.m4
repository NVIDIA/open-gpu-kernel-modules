dnl #
dnl # commit 2107777c0249e95f9493f3341dcb4fd89b965385
dnl # drm/atomic: Add macros to access existing old/new state, v2.
dnl #
AC_DEFUN([AC_AMDGPU_DRM_ATOMIC_GET_NEW_PLANE_STATE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_atomic.h>
		], [
			struct drm_atomic_state *state = NULL;
			struct drm_plane *plane = NULL;
			struct drm_plane_state *ret;

			ret = drm_atomic_get_new_plane_state(state, plane);
		], [
			AC_DEFINE(HAVE_DRM_ATOMIC_GET_NEW_PLANE_STATE, 1,
				[drm_atomic_get_new_plane_state() is available])
		])
	])
])
