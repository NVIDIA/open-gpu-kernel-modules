dnl #
dnl # v4.12-rc7-1394-gd85231530b07
dnl # drm: add helper to validate YCBCR420 modes
dnl #
AC_DEFUN([AC_AMDGPU_STRUCT_DRM_CONNECTOR], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_connector.h>
		], [
			struct drm_connector *connector = NULL;
			connector->ycbcr_420_allowed = false;
		], [
			AC_DEFINE(HAVE_STRUCT_DRM_CONNECTOR_YCBCR_420_ALLOWED, 1,
				[drm_connector->ycbcr_420_allowed is available])
		])
	])
])
