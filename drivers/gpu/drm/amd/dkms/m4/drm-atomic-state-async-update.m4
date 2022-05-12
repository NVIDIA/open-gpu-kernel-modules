dnl #
dnl # commit fef9df8b594531a4257b6a3bf7e190570c17be29
dnl # drm/atomic: initial support for asynchronous plane update
dnl #
AC_DEFUN([AC_AMDGPU_DRM_ATOMIC_STATE_ASYNC_UPDATE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_atomic.h>
		], [
			struct drm_atomic_state *state = NULL;

			state->async_update = 0;
		], [
			AC_DEFINE(HAVE_DRM_ATOMIC_STATE_ASYNC_UPDATE, 1,
				[whether struct drm_atomic_state have async_update])
		])
	])
])
