dnl #
dnl # commit v5.6-rc2-1021-g2dea2d118217
dnl # drm: Remove unused arg from drm_fb_helper_init
dnl #
AC_DEFUN([AC_AMDGPU_DRM_FB_HELPER_INIT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#ifdef HAVE_DRM_DRMP_H
			#include <drm/drmP.h>
			#endif
			#include <drm/drm_fb_helper.h>
		], [
			drm_fb_helper_init(NULL, NULL);
		], [drm_fb_helper_init], [drivers/gpu/drm/drm_fb_helper.c], [
			AC_DEFINE(HAVE_DRM_FB_HELPER_INIT_2ARGS, 1,
				[drm_fb_helper_init() has 2 args])
		], [
		dnl #
		dnl # commit v4.10-rc5-1046-ge4563f6ba717
		dnl # drm: Rely on mode_config data for fb_helper initialization
		dnl #
			AC_KERNEL_TRY_COMPILE_SYMBOL([
				#ifdef HAVE_DRM_DRMP_H
				#include <drm/drmP.h>
				#endif
				#include <drm/drm_fb_helper.h>
			], [
				drm_fb_helper_init(NULL, NULL, 0);
			], [drm_fb_helper_init], [drivers/gpu/drm/drm_fb_helper.c], [
				AC_DEFINE(HAVE_DRM_FB_HELPER_INIT_3ARGS, 1,
					[drm_fb_helper_init() has 3 args])
			])
		])
	])
])
