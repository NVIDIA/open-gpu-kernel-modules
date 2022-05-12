dnl #
dnl # commit v5.4-rc4-752-g3f9b3f02dda5
dnl # drm/dp_mst: Protect drm_dp_mst_port members with locking
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DP_MST_DETECT_PORT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_dp_mst_helper.h>
		], [
			int ret;
			ret = drm_dp_mst_detect_port(NULL, NULL, NULL, NULL);
		], [
			AC_DEFINE(HAVE_DRM_DP_MST_DETECT_PORT_PPPP, 1,
				[drm_dp_mst_detect_port() wants p,p,p,p args])
		])
	])
])
