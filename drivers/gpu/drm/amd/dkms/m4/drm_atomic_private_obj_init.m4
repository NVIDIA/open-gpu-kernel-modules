dnl #
dnl # commit v4.20-rc4-945-gb962a12050a3
dnl # drm/atomic: integrate modeset lock with private objects
dnl #
AC_DEFUN([AC_AMDGPU_DRM_ATOMIC_PRIVATE_OBJ_INIT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_atomic.h>
		], [
			drm_atomic_private_obj_init(NULL, NULL, NULL, NULL);
		], [
			AC_DEFINE(HAVE_DRM_ATOMIC_PRIVATE_OBJ_INIT_P_P_P_P, 1,
				[drm_atomic_private_obj_init() has p,p,p,p interface])
			AC_DEFINE(HAVE_DRM_ATOMIC_PRIVATE_OBJ_INIT, 1,
				[drm_atomic_private_obj_init() is available])
		], [
			dnl #
			dnl # commit v4.12-rc7-1381-ga4370c777406
			dnl # drm/atomic: Make private objs proper objects
			dnl #
			AC_KERNEL_TRY_COMPILE([
				#include <drm/drm_atomic.h>
			], [
				drm_atomic_private_obj_init(NULL, NULL, NULL);
			], [
				AC_DEFINE(HAVE_DRM_ATOMIC_PRIVATE_OBJ_INIT, 1,
					[drm_atomic_private_obj_init() is available])
			])
		])
	])
])
