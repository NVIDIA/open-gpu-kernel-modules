dnl #
dnl # commit v5.9-rc2-391-g95f29c0b5105
dnl # drm/amdgpu/dc: Require primary plane to be enabled whenever the
dnl # CRTC is
dnl #
AC_DEFUN([AC_AMDGPU_DRM_PLANE_MASK], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_crtc.h>
		], [
			drm_plane_mask(NULL);
		], [
			AC_DEFINE(HAVE_DRM_PLANE_MASK, 1,
				[drm_plane_mask is available])
		])
	])
])
