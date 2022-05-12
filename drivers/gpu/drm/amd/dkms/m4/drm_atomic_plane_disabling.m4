dnl #
dnl # commit v4.10-rc8-1379-g51ffa12d90ba
dnl # drm/atomic: Make drm_atomic_plane_disabling easier to understand.
dnl #
AC_DEFUN([AC_AMDGPU_DRM_ATOMIC_PLANE_DISABLING], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_atomic_helper.h>
		], [
			drm_atomic_plane_disabling((struct drm_plane_state *)NULL, NULL);
		], [
			AC_DEFINE(HAVE_DRM_ATOMIC_PLANE_DISABLING_DRM_PLANE_STATE, 1,
				[drm_atomic_plane_disabling() wants drm_plane_state * arg])
		])
	])
])
