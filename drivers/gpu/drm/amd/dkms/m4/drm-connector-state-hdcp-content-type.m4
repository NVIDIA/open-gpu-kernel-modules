dnl #
dnl # commit v5.3-rc1-377-g7672dbba85d3
dnl # drm: Add Content protection type property
dnl #
AC_DEFUN([AC_AMDGPU_DRM_CONNECTOR_STATE_HDCP_CONTENT_TYPE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_connector.h>
		],[
			struct drm_connector_state *state = NULL;
			state->hdcp_content_type = 0;
		],[
			AC_DEFINE(HAVE_DRM_CONNECTOR_STATE_HDCP_CONTENT_TYPE, 1,
				[struct drm_connector_state has hdcp_content_type member])
		])
	])
])
