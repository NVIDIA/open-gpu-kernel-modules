dnl #
dnl # commit v5.3-rc1-656-g62afb4ad425a
dnl # drm/connector: Allow max possible encoders to attach to a connector
dnl #
AC_DEFUN([AC_AMDGPU_DRM_CONNECTOR_FOR_EACH_POSSIBLE_ENCODER], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_encoder.h>
			#include <drm/drm_connector.h>
		],[
			struct drm_connector *connector = NULL;
			struct drm_encoder *encoder = NULL;
			drm_connector_for_each_possible_encoder(connector, encoder)
				return 0;
		],[
			AC_DEFINE(HAVE_DRM_CONNECTOR_FOR_EACH_POSSIBLE_ENCODER_2ARGS, 1,
				[drm_connector_for_each_possible_encoder() wants 2 arguments])
		])
	])
])
