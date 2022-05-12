dnl #
dnl # commit 0853695c3ba46f97dfc0b5885f7b7e640ca212dd
dnl # drm: Add reference counting to drm_atomic_state
dnl #
AC_DEFUN([AC_AMDGPU_DRM_ATOMIC_STATE_PUT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_atomic.h>
		], [
			drm_atomic_state_put(NULL);
		], [
			AC_DEFINE(HAVE_DRM_ATOMIC_STATE_PUT, 1,
				[drm_atomic_state_put() is available])
		])
	])
])
