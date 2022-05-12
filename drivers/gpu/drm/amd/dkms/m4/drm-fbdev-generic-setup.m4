dnl #
dnl # commit v4.18-rc3-614-g9060d7f49376
dnl # drm/fb-helper: Finish the generic fbdev emulation
dnl #
AC_DEFUN([AC_AMDGPU_DRM_FBDEV_GENERIC_SETUP], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <drm/drm_fb_helper.h>
		], [
			drm_fbdev_generic_setup(NULL, 0);
		], [drm_fbdev_generic_setup], [drivers/gpu/drm/drm_fb_helper.c],[
			AC_DEFINE(HAVE_DRM_FBDEV_GENERIC_SETUP, 1,
				[drm_fbdev_generic_setup() is available])
		])
	])
])
