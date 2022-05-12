dnl #
dnl # commit 2570fe2586254ff174c2ba5a20dabbde707dbb9b
dnl # drm: add helper functions for YCBCR420 handling
dnl #
AC_DEFUN([AC_AMDGPU_DRM_MODE_IS_420_XXX], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <drm/drm_modes.h>
		], [
			drm_mode_is_420_only(NULL, NULL);
			drm_mode_is_420_also(NULL, NULL);
		], [drm_mode_is_420_only drm_mode_is_420_also],[drivers/gpu/drm/drm_modes.c],[
			AC_DEFINE(HAVE_DRM_MODE_IS_420_XXX, 1,
				[drm_mode_is_420_xxx() is available])
		])
	])
])
