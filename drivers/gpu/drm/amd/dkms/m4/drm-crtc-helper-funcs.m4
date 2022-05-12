dnl #
dnl # v5.10-rc2-260-g29b77ad7b9ca
dnl # drm/atomic: Pass the full state to CRTC atomic_check
dnl
dnl # v5.10-rc2-261-gf6ebe9f9c923
dnl # drm/atomic: Pass the full state to CRTC atomic begin and flush
dnl #
AC_DEFUN([AC_AMDGPU_DRM_CRTC_HELPER_FUNCS_ATOMIC_CHECK], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_modeset_helper_vtables.h>
			#include <drm/drm_atomic.h>
		], [
			struct drm_crtc_helper_funcs *p = NULL;
			p->atomic_check(NULL, (struct drm_atomic_state*)NULL);
		], [
			AC_DEFINE(HAVE_DRM_CRTC_HELPER_FUNCS_ATOMIC_CHECK_ARG_DRM_ATOMIC_STATE, 1,
				[drm_crtc_helper_funcs->atomic_check()/atomic_flush()/atomic_begin() wants struct drm_atomic_state arg])
		])
	])
])

dnl #
dnl # v5.9-rc5-1161-g351f950db4ab
dnl # drm/atomic: Pass the full state to CRTC atomic enable/disable
dnl #
AC_DEFUN([AC_AMDGPU_DRM_CRTC_HELPER_FUNCS_ATOMIC_ENABLE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_modeset_helper_vtables.h>
			#include <drm/drm_atomic.h>
		], [
			struct drm_crtc_helper_funcs *p = NULL;
			p->atomic_enable(NULL, (struct drm_atomic_state*)NULL);
		], [
			AC_DEFINE(HAVE_DRM_CRTC_HELPER_FUNCS_ATOMIC_ENABLE_ARG_DRM_ATOMIC_STATE, 1,
				[drm_crtc_helper_funcs->atomic_enable()/atomic_disable() wants struct drm_atomic_state arg])
			AC_DEFINE(HAVE_DRM_CRTC_HELPER_FUNCS_HAVE_ATOMIC_ENABLE, 1,
				[have drm_crtc_helper_funcs->atomic_enable()])

		],[
			dnl #
			dnl # v4.12-rc7-1332-g0b20a0f8c3cb
			dnl # drm: Add old state pointer to CRTC .enable() helper function
			dnl #
			AC_KERNEL_TRY_COMPILE([
				#include <drm/drm_modeset_helper_vtables.h>
				#include <drm/drm_atomic.h>
			], [
				struct drm_crtc_helper_funcs *p = NULL;
				p->atomic_enable(NULL, NULL);
			], [
				AC_DEFINE(HAVE_DRM_CRTC_HELPER_FUNCS_HAVE_ATOMIC_ENABLE, 1,
					[have drm_crtc_helper_funcs->atomic_enable()])
			])

		])
	])
])

AC_DEFUN([AC_AMDGPU_DRM_CRTC_HELPER_FUNCS], [
                AC_AMDGPU_DRM_CRTC_HELPER_FUNCS_ATOMIC_CHECK
                AC_AMDGPU_DRM_CRTC_HELPER_FUNCS_ATOMIC_ENABLE
])
