dnl #
dnl # commit cfe63423d9be3e7020296c3dfb512768a83cd099
dnl # drm/fb-helper: Add drm_fb_helper_set_suspend_unlocked()
dnl #
AC_DEFUN([AC_AMDGPU_DRM_FB_HELPER_SET_SUSPEND_UNLOCKED], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#ifdef HAVE_DRM_DRMP_H
			struct vm_area_struct;
			#include <drm/drmP.h>
			#endif
			#include <drm/drm_fb_helper.h>
		], [
			drm_fb_helper_set_suspend_unlocked(NULL,0);
		], [
			AC_DEFINE(HAVE_DRM_FB_HELPER_SET_SUSPEND_UNLOCKED, 1,
				[drm_fb_helper_set_suspend_unlocked() is available])
		])
	])
])
