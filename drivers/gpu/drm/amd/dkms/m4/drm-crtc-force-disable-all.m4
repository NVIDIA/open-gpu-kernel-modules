dnl #
dnl # commit 6a0d95285035c43361c72776b4c618f60c0f4ab4
dnl # drm: Add helpers to turn off CRTCs
dnl #
AC_DEFUN([AC_AMDGPU_DRM_CRTC_FORCE_DISABLE_ALL], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <drm/drm_crtc.h>
		], [
			drm_crtc_force_disable_all(NULL);
		], [drm_crtc_force_disable_all], [drivers/gpu/drm/drm_crtc.c], [
			AC_DEFINE(HAVE_DRM_CRTC_FORCE_DISABLE_ALL, 1,
				[drm_crtc_force_disable_all() is available])
		])
	])
])
