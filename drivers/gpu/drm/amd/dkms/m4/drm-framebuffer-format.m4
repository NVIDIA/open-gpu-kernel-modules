dnl #
dnl # commit e14c23c647abfc1fed96a55ba376cd9675a54098
dnl # drm: Store a pointer to drm_format_info under drm_framebuffer
dnl #
AC_DEFUN([AC_AMDGPU_DRM_FRAMEBUFFER_FORMAT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#ifdef HAVE_DRM_DRMP_H
			struct vm_area_struct;
			#include <drm/drmP.h>
			#endif
			#include <drm/drm_framebuffer.h>
		], [
			struct drm_framebuffer *foo = NULL;
			foo->format = NULL;
		], [
			AC_DEFINE(HAVE_DRM_FRAMEBUFFER_FORMAT, 1,
				[whether struct drm_framebuffer have format])
		])
	])
])
