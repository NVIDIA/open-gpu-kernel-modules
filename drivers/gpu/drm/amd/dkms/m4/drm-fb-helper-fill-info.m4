dnl #
dnl # commit ec8bf1942567bf0736314da9723e93bcc73c131f
dnl # drm/fb-helper: Fixup fill_info cleanup
dnl #
AC_DEFUN([AC_AMDGPU_DRM_FB_HELPER_FILL_INFO], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#ifdef HAVE_DRM_DRMP_H
			struct vm_area_struct;
			#include <drm/drmP.h>
			#endif
			#include <drm/drm_fb_helper.h>
		], [
			drm_fb_helper_fill_info(NULL, NULL, NULL);
		], [
			AC_DEFINE(HAVE_DRM_FB_HELPER_FILL_INFO, 1,
				[drm_fb_helper_fill_info() is available])
		])
	])
])
