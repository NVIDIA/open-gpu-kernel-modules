dnl #
dnl # v4.12-rc7-1335-gfef9df8b5945
dnl # drm/atomic: initial support for asynchronous plane update
dnl #
AC_DEFUN([AC_AMDGPU_STRUCT_DRM_PLANE_HELPER_FUNCS_ATOMIC_ASYNC_CHECK], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_modeset_helper_vtables.h>
		], [
			struct drm_plane_helper_funcs *funcs = NULL;
			funcs->atomic_async_check(NULL, NULL);
			funcs->atomic_async_update(NULL, NULL);
		], [
			AC_DEFINE(HAVE_STRUCT_DRM_PLANE_HELPER_FUNCS_ATOMIC_ASYNC_CHECK, 1,
				[drm_plane_helper_funcs->atomic_async_check() is available])
		])
	])
])

dnl #
dnl # v4.8-rc2-355-g1832040d010e drm: Allow drivers to modify plane_state in prepare_fb/cleanup_fb
dnl # v4.4-rc4-250-g092d01dae09a drm: Reorganize helper vtables and their docs
dnl # v4.2-rc8-1424-g844f9111f6f5 drm/atomic: Make prepare_fb/cleanup_fb only take state, v3.
dnl # v4.0-rc1-142-gd136dfeec84b drm: Pass in new and old plane state to prepare_fb and cleanup_fb
dnl # v3.18-rc2-124-gc2fcd274bce5 drm: Add atomic/plane helpers
dnl #
AC_DEFUN([AC_AMDGPU_STRUCT_DRM_PLANE_HELPER_FUNCS_PREPARE_FB], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/types.h>
			#include <drm/drm_plane_helper.h>
		], [
			struct drm_plane_helper_funcs *funcs = NULL;
			funcs->prepare_fb((struct drm_plane *)NULL, (struct drm_plane_state *) NULL);
		], [
			AC_DEFINE(HAVE_STRUCT_DRM_PLANE_HELPER_FUNCS_PREPARE_FB_PP, 1,
				[drm_plane_helper_funcs->prepare_fb() wants p,p arg])
		], [
			AC_KERNEL_TRY_COMPILE([
				#include <linux/types.h>
				#include <drm/drm_plane_helper.h>
			], [
				struct drm_plane_helper_funcs *funcs = NULL;
				funcs->prepare_fb((struct drm_plane *)NULL, (const struct drm_plane_state *) NULL);
			], [
				AC_DEFINE(HAVE_STRUCT_DRM_PLANE_HELPER_FUNCS_PREPARE_FB_CONST, 1,
					[drm_plane_helper_funcs->prepare_fb() wants const p arg])
			])
		])
	])
])

AC_DEFUN([AC_AMDGPU_STRUCT_DRM_PLANE_HELPER_FUNCS_ATOMIC_CHECK_DRM_ATOMIC_STATE_PARAMS], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_modeset_helper_vtables.h>
		], [
			struct drm_plane_helper_funcs *funcs = NULL;
			funcs->atomic_check(NULL, (struct drm_atomic_state *)NULL);
		], [
			AC_DEFINE(HAVE_STRUCT_DRM_PLANE_HELPER_FUNCS_ATOMIC_CHECK_DRM_ATOMIC_STATE_PARAMS, 1,
				[drm_plane_helper_funcs->atomic_check() second param wants drm_atomic_state arg])
		])
	])
])

AC_DEFUN([AC_AMDGPU_STRUCT_DRM_PLANE_HELPER_FUNCS], [
	AC_AMDGPU_STRUCT_DRM_PLANE_HELPER_FUNCS_ATOMIC_ASYNC_CHECK
	AC_AMDGPU_STRUCT_DRM_PLANE_HELPER_FUNCS_PREPARE_FB
	AC_AMDGPU_STRUCT_DRM_PLANE_HELPER_FUNCS_ATOMIC_CHECK_DRM_ATOMIC_STATE_PARAMS
])

