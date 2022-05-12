dnl #
dnl # commit v5.4-rc4-759-g6f85f73821f6
dnl # drm/dp_mst: Add basic topology reprobing when resuming
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DP_MST_TOPOLOGY_MGR_RESUME], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_dp_mst_helper.h>
		], [
			int ret;
			ret = drm_dp_mst_topology_mgr_resume(NULL, 0);
		], [
			AC_DEFINE(HAVE_DRM_DP_MST_TOPOLOGY_MGR_RESUME_2ARGS, 1,
				[drm_dp_mst_topology_mgr_resume() wants 2 args])
		])
	])
])
