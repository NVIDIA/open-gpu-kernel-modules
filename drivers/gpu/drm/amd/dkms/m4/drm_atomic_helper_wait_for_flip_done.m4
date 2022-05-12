dnl #
dnl # v4.12-rc1-218-g0108648749bf
dnl # drm: Add drm_atomic_helper_wait_for_flip_done()
dnl #
AC_DEFUN([AC_AMDGPU_DRM_ATOMIC_HELPER_WAIT_FOR_FLIP_DONE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([drm_atomic_helper_wait_for_flip_done],
			[drivers/gpu/drm/drm_atomic_helper.c], [
			AC_DEFINE(HAVE_DRM_ATOMIC_HELPER_WAIT_FOR_FLIP_DONE, 1,
				[drm_atomic_helper_wait_for_flip_done() is available])
		])
	])
])
