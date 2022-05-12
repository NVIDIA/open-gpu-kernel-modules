dnl #
dnl # commit v5.3-rc1-708-gf0a8f533adc2
dnl # drm/print: add drm_debug_enabled()
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DEBUG_ENABLED], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_print.h>
		],[
			drm_debug_enabled(0);
		],[
			AC_DEFINE(HAVE_DRM_DEBUG_ENABLED,
				1,
				[drm_debug_enabled() is available])
		])
	])
])
