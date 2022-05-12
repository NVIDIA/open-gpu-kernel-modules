dnl #
dnl # commit d251c02a2b78245bb32d7909a66b06285f7922a2
dnl # drm/dp_mst: Add helpers for MST DSC and virtual DPCD aux
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DP_MST_DSC_AUX_FOR_PORT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <drm/drm_dp_mst_helper.h>
		], [
			drm_dp_mst_dsc_aux_for_port(NULL);
		], [drm_dp_mst_dsc_aux_for_port], [drivers/gpu/drm/drm_dp_mst_topology.c], [
			AC_DEFINE(HAVE_DRM_DP_MST_DSC_AUX_FOR_PORT, 1,
				[drm_dp_mst_dsc_aux_for_port() is available])
		])
	])
])
