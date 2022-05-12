dnl #
dnl # fade785374de drm/amdkcl: [5.0] fix drm_connector_state max_bpc build error
dnl # v5.2-rc1-162-g42ba01fc30e6 drm/amd/display: Use new connector state when getting color depth
dnl # v5.2-rc1-161-g1825fd34e8ed drm/amd/display: Switch the custom "max bpc" property to the DRM prop
dnl # v4.19-rc6-1783-g47e22ff1a9e0 drm: Add connector property to limit max bpc
dnl #
AC_DEFUN([AC_AMDGPU_DRM_CONNECTOR_PROPERTY_MAX_BPC], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_connector.h>
		], [
			struct drm_connector *connector = NULL;
			struct drm_connector_state *connector_state = NULL;

			connector->max_bpc_property = NULL;
			connector_state->max_requested_bpc = 0;
			connector_state->max_bpc = 0;
		], [
			AC_DEFINE(HAVE_DRM_CONNECTOR_PROPERTY_MAX_BPC, 1,
				[connector property "max bpc" is available])
		])
	])
])
