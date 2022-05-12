dnl #
dnl # commit v5.1-rc5-1688-gfbb5d0353c62
dnl # drm: Add HDR source metadata property
dnl #
AC_DEFUN([AC_AMDGPU_DRM_CONNECTOR_STATE_HDR_OUTPUT_METADATA], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_connector.h>
		],[
			struct drm_connector_state *state = NULL;
			state->hdr_output_metadata = NULL;
		],[
			AC_DEFINE(HAVE_DRM_CONNECTOR_STATE_HDR_OUTPUT_METADATA, 1,
				[struct drm_connector_state has hdr_output_metadata member])
		])
	])
])
