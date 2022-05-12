dnl #
dnl # v5.0-rc1-415-g022debad063e
dnl # drm/atomic: Add drm_atomic_state->duplicated
dnl #
AC_DEFUN([AC_AMDGPU_STRUCT_DRM_ATOMIC_STATE_DUPLICATED], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_atomic.h>
		],[
			struct drm_atomic_state *state = NULL;
			state->duplicated = 0;
		],[
			AC_DEFINE(HAVE_STRUCT_DRM_ATOMIC_STATE_DUPLICATED, 1,
				[struct drm_connector_state->duplicated is available])
		])
	])
])

