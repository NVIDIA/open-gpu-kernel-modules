dnl #
dnl # commit v5.3-rc1-555-gae85b0df124f
dnl # drm_dp_cec: add connector info support.
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DP_CEC_CORRELATION_FUNCTIONS], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_dp_helper.h>
		], [
			drm_dp_cec_register_connector(NULL, NULL);
		], [
			AC_DEFINE(HAVE_DRM_DP_CEC_REGISTER_CONNECTOR_PP, 1,
				[drm_dp_cec_register_connector() wants p,p interface])
			AC_DEFINE(HAVE_DRM_DP_CEC_CORRELATION_FUNCTIONS, 1,
				[drm_dp_cec* correlation functions are available])
		], [
			AC_KERNEL_TRY_COMPILE([
				#include <drm/drm_dp_helper.h>
			], [
				drm_dp_cec_irq(NULL);
				drm_dp_cec_register_connector(NULL, NULL, NULL);
				drm_dp_cec_unregister_connector(NULL);
				drm_dp_cec_set_edid(NULL, NULL);
				drm_dp_cec_unset_edid(NULL);
			], [
				AC_DEFINE(HAVE_DRM_DP_CEC_CORRELATION_FUNCTIONS, 1,
					[drm_dp_cec* correlation functions are available])
			])
		])
	])
])
