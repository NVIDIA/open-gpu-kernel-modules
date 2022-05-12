dnl #
dnl # commit fbb5d0353c62d10c3699ec844d2d015a762952d7
dnl # drm: Add HDR source metadata property
dnl #

AC_DEFUN([AC_AMDGPU_DRM_CONNECTOR_HAVE_HDR_SINK_METADATA], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_connector.h>
		],[
			struct drm_connector *dc = NULL;
			struct hdr_sink_metadata *p = NULL;

			p = &dc->hdr_sink_metadata;
		],[
			AC_DEFINE(HAVE_HDR_SINK_METADATA, 1,
				[drm_connector_hdr_sink_metadata() is available])
		])
	])
])
