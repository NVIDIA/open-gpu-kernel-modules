dnl #
dnl # commit v4.11-rc7-1869-g3f3353b7e121
dnl # drm/dp: Introduce MST topology state to track available link bandwidth
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DP_MST_TOPOLOGY_STATE_TOTAL_AVAIL_SLOTS], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_dp_mst_helper.h>
		], [
			struct drm_dp_mst_topology_state * mst_state = NULL;
			mst_state->total_avail_slots = 0;
		], [
			AC_DEFINE(HAVE_DRM_DP_MST_TOPOLOGY_STATE_TOTAL_AVAIL_SLOTS, 1,
				[struct drm_dp_mst_topology_state has member total_avail_slots])
		])
	])
])

