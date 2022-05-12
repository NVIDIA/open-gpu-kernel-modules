dnl #
dnl # 4.14 API
dnl # commit e6fc3b68558e4c6d8d160b5daf2511b99afa8814
dnl # drm: Plumb modifiers through plane init
dnl #
AC_DEFUN([AC_AMDGPU_NUM_ARGS_DRM_UNIVERSAL_PLANE_INIT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <drm/drm_crtc.h>
		], [
			drm_universal_plane_init(NULL, NULL, 0, NULL, NULL, 0, NULL, 0, NULL);
		], [drm_universal_plane_init], [drivers/gpu/drm/drm_plane.c], [
			AC_DEFINE(HAVE_DRM_UNIVERSAL_PLANE_INIT_9ARGS, 1,
				[drm_universal_plane_init() wants 9 args])
		], [
			dnl #
			dnl # commit b0b3b7951114
			dnl # drm: Pass 'name' to drm_universal_plane_init()
			dnl #
			AC_KERNEL_TRY_COMPILE_SYMBOL([
				#include <drm/drm_crtc.h>
			], [
				drm_universal_plane_init(NULL, NULL, 0, NULL, NULL, 0, 0, NULL);
			], [drm_universal_plane_init], [drivers/gpu/drm/drm_plane.c drivers/gpu/drm/drm_crtc.c], [
				AC_DEFINE(HAVE_DRM_UNIVERSAL_PLANE_INIT_8ARGS, 1,
					[drm_universal_plane_init() wants 8 args])
			], [
				dnl #
				dnl # commit v3.14-rc7-684-gdc415ff97d6b
				dnl # drm: Add drm_universal_plane_init()
				dnl #
				AC_DEFINE(HAVE_DRM_UNIVERSAL_PLANE_INIT_7ARGS, 1,
					[drm_universal_plane_init() wants 7 args])
			])
		])
	])
])
