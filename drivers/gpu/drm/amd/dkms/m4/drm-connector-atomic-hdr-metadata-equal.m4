dnl #
dnl # commit v5.12-rc7-1582-g72921cdf8ac2
dnl # drm/connector: Add helper to compare HDR metadata
dnl #
AC_DEFUN([AC_AMDGPU_DRM_CONNECTOR_ATOMIC_HDR_METADATA_EQUAL], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <drm/drm_connector.h>
		], [
			drm_connector_atomic_hdr_metadata_equal(NULL, NULL);
		], [drm_connector_atomic_hdr_metadata_equal], [drm/drm_connector.c], [
			AC_DEFINE(HAVE_DRM_CONNECTOR_ATOMIC_HDR_METADATA_EQUAL, 1, 
				[drm_connector_atomic_hdr_metadata_equal() is available])
		])
	])
])
