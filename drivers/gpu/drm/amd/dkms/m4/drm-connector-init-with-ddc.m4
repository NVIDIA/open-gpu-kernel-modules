dnl #
dnl # commit v5.3-rc1-330-g100163df4203
dnl # drm: Add drm_connector_init() variant with ddc
dnl #
AC_DEFUN([AC_AMDGPU_DRM_CONNECTOR_INIT_WITH_DDC], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_connector.h>
		],[
			drm_connector_init_with_ddc(NULL, NULL, NULL, 0, NULL);
		],[
			AC_DEFINE(HAVE_DRM_CONNECTOR_INIT_WITH_DDC, 1,
				[drm_connector_init_with_ddc() is available])
		])
	])
])
