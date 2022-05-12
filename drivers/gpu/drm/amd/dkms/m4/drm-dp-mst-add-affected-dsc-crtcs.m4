dnl #
dnl # commit 971fb192aaeb4b5086ac3f21d00943a5e1431176
dnl # drm/dp_mst: Add helper to trigger modeset on affected DSC MST CRTCs
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DP_MST_ADD_AFFECTED_DSC_CRTCS], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <drm/drm_dp_mst_helper.h>
		], [
			int ret;
			ret = drm_dp_mst_add_affected_dsc_crtcs(NULL, NULL);
		], [drm_dp_mst_dsc_aux_for_port], [drivers/gpu/drm/drm_dp_mst_topology.c], [
			AC_DEFINE(HAVE_DRM_DP_MST_ADD_AFFECTED_DSC_CRTCS, 1,
				[drm_dp_mst_add_affected_dsc_crtcs() is available])
		])
	])
])
