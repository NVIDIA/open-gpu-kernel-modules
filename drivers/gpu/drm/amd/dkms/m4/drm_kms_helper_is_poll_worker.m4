dnl #
dnl # commit v4.15-rc8-13-g25c058ccaf2e
dnl # drm: Allow determining if current task is output poll worker
dnl #
AC_DEFUN([AC_AMDGPU_DRM_KMS_HELPER_IS_POLL_WORKER], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([drm_kms_helper_is_poll_worker],
			[drivers/gpu/drm/drm_probe_helper.c], [
			AC_DEFINE(HAVE_DRM_KMS_HELPER_IS_POLL_WORKER, 1,
				[drm_kms_helper_is_poll_worker() is available])
		])
	])
])
