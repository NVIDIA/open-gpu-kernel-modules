dnl #
dnl # v4.10-rc3-517-g7b0a89a6db9a
dnl # drm/dp: Store drm_device in MST topology manager
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DP_MST_TOPOLOGY_MGR_INIT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_dp_mst_helper.h>
		], [
			drm_dp_mst_topology_mgr_init(NULL, (struct drm_device *)NULL, NULL, 0, 0, 0, 0, 0);
		], [
			AC_DEFINE(HAVE_DRM_DP_MST_TOPOLOGY_MGR_INIT_DRM_DEV, 1,
				[drm_dp_mst_topology_mgr_init() wants drm_device arg])
			AC_DEFINE(HAVE_DRM_DP_MST_TOPOLOGY_MGR_INIT_MAX_LANE_COUNT, 1,
				[drm_dp_mst_topology_mgr_init() has max_lane_count and max_link_rate])
		], [
			AC_KERNEL_TRY_COMPILE([
				#include <drm/drm_dp_mst_helper.h>
			], [
				drm_dp_mst_topology_mgr_init(NULL, (struct drm_device *)NULL, NULL, 0, 0, 0);
			], [
				AC_DEFINE(HAVE_DRM_DP_MST_TOPOLOGY_MGR_INIT_DRM_DEV, 1,
					[drm_dp_mst_topology_mgr_init() wants drm_device arg])
			])
		])
	])
])
