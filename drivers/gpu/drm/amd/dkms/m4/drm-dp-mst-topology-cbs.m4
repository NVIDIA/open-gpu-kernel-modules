dnl #
dnl # commit v4.20-rc4-941-g16bff572cc66
dnl # drm/dp-mst-helper: Remove hotplug callback
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DP_MST_TOPOLOGY_CBS_HOTPLUG], [
	AC_KERNEL_TRY_COMPILE([
		#include <drm/drm_dp_mst_helper.h>
	], [
		struct drm_dp_mst_topology_cbs *dp_mst_cbs = NULL;
		dp_mst_cbs->hotplug(NULL);
	], [
		AC_DEFINE(HAVE_DRM_DP_MST_TOPOLOGY_CBS_HOTPLUG, 1,
			[struct drm_dp_mst_topology_cbs has hotplug member])
	])
])


dnl #
dnl # commit v5.6-rc2-1065-ga5c4dc165957
dnl # drm/dp_mst: Remove register_connector callback
dnl #
dnl # commit v4.3-rc3-39-gd9515c5ec1a2
dnl # drm/dp/mst: split connector registration into two parts (v2)
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DP_MST_TOPOLOGY_CBS_REGISTER_CONNECTOR], [
	AC_KERNEL_TRY_COMPILE([
		#include <drm/drm_dp_mst_helper.h>
	], [
		struct drm_dp_mst_topology_cbs *dp_mst_cbs = NULL;
		dp_mst_cbs->register_connector(NULL);
	], [
		AC_DEFINE(HAVE_DRM_DP_MST_TOPOLOGY_CBS_REGISTER_CONNECTOR, 1,
			[struct drm_dp_mst_topology_cbs->register_connector is available])
	])
])

dnl #
dnl # commit v5.6-rc5-1703-g72dc0f515913
dnl # drm/dp_mst: Remove drm_dp_mst_topology_cbs.destroy_connector
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DP_MST_TOPOLOGY_CBS_DESTROY_CONNECTOR], [
	AC_KERNEL_TRY_COMPILE([
		#include <drm/drm_dp_mst_helper.h>
	], [
		struct drm_dp_mst_topology_cbs *dp_mst_cbs = NULL;
		dp_mst_cbs->destroy_connector(NULL, NULL);
	], [
		AC_DEFINE(HAVE_DRM_DP_MST_TOPOLOGY_CBS_DESTROY_CONNECTOR, 1,
			[struct drm_dp_mst_topology_cbs->destroy_connector is available])
	])
])

AC_DEFUN([AC_AMDGPU_DRM_DP_MST_TOPOLOGY_CBS], [
	AC_KERNEL_DO_BACKGROUND([
		AC_AMDGPU_DRM_DP_MST_TOPOLOGY_CBS_HOTPLUG
		AC_AMDGPU_DRM_DP_MST_TOPOLOGY_CBS_REGISTER_CONNECTOR
		AC_AMDGPU_DRM_DP_MST_TOPOLOGY_CBS_DESTROY_CONNECTOR
	])
])
