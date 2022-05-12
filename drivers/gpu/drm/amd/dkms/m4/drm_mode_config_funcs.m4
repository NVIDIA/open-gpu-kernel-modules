dnl #
dnl # v4.1-rc2-37-g036ef5733ba4
dnl # drm/atomic: Allow drivers to subclass drm_atomic_state, v3
dnl #
AC_DEFUN([AC_AMDGPU_DRM_MODE_CONFIG_FUNCS], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_mode_config.h>
			#include <drm/drm_crtc.h>
		], [
			struct drm_mode_config_funcs *funcs = NULL;
			funcs->atomic_state_alloc(NULL);
		], [
			AC_DEFINE(HAVE_DRM_MODE_CONFIG_FUNCS_ATOMIC_STATE_ALLOC, 1,
				[drm_mode_config_funcs->atomic_state_alloc() is available])
		])
	])
])
