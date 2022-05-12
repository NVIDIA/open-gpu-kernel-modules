dnl #
dnl # v4.14-rc3-576-g304a4f6accac
dnl # drm/fb-helper: Add .last_close and .output_poll_changed helpers
dnl #
AC_DEFUN([AC_AMDGPU_DRM_FB_HELPER_LASTCLOSE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#ifdef HAVE_DRM_DRMP_H
			struct vm_area_struct;
			#include <drm/drmP.h>
			#endif
			#include <drm/drm_fb_helper.h>
		], [
			drm_fb_helper_lastclose(NULL);
		], [drm_fb_helper_lastclose], [drivers/gpu/drm/drm_fb_helper.c], [
			AC_DEFINE(HAVE_DRM_FB_HELPER_LASTCLOSE, 1,
				[whether drm_fb_helper_lastclose() is available])
		])
	])
])
