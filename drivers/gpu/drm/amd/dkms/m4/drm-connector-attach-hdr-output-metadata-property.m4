dnl #
dnl # commit v5.12-rc7-1581-ge057b52c1d90
dnl # drm/connector: Create a helper to attach the hdr_output_metadata property
dnl #
AC_DEFUN([AC_AMDGPU_DRM_CONNECTOR_ATTACH_HDR_OUTPUT_METADATA_PROPERTY], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <drm/drm_connector.h>
		], [
			drm_connector_attach_hdr_output_metadata_property(NULL);
		], [drm_connector_attach_hdr_output_metadata_property], [drm/drm_connector.c], [
			AC_DEFINE(HAVE_DRM_CONNECTOR_ATTACH_HDR_OUTPUT_METADATA_PROPERTY, 1, 
				[drm_connector_attach_hdr_output_metadata_property() is available])
		])
	])
])
