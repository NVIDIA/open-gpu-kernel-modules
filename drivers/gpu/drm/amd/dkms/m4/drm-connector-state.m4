dnl #
dnl # v4.20-rc3-425-g1398958cfd8d
dnl # drm: Add vrr_enabled property to drm CRTC
dnl #
AC_DEFUN([AC_AMDGPU_STRUCT_DRM_CONNECTOR_STATE_COLORSPACE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_connector.h>
		],[
			struct drm_connector_state *state = NULL;
			state->colorspace = 0;
		],[
			AC_DEFINE(HAVE_STRUCT_DRM_CONNECTOR_STATE_COLORSPACE, 1,
				[struct drm_connector_state->colorspace is available])
			AC_KERNEL_TRY_COMPILE([
				#include <drm/drm_connector.h>
			],[
				struct drm_connector_state *state = NULL;
				state->self_refresh_aware = 0;
			],[
				AC_DEFINE(HAVE_STRUCT_DRM_CONNECTOR_STATE_SELF_REFRESH_AWARE, 1,
					[struct drm_connector_state->self_refresh_aware is available])
			])
		])
	])
])
