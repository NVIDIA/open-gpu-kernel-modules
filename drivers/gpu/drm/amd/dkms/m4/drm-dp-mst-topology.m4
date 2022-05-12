dnl #
dnl # commit 1e797f556c616a42f1e039b1ff1d3c58f61b6104
dnl # drm/dp: Split drm_dp_mst_allocate_vcpi
dnl #
dnl # Note: This autoconf only works with compiler flag -Werror
dnl #       The interface types are specified in Hungarian notation
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DP_MST_TOPOLOGY], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_dp_mst_helper.h>
		], [
			drm_dp_mst_allocate_vcpi(NULL, NULL, 1, 1);
		], [
			AC_DEFINE(HAVE_DRM_DP_MST_ALLOCATE_VCPI_P_P_I_I, 1, [
				drm_dp_mst_allocate_vcpi() has p,p,i,i interface])
		])
	])
	dnl #
	dnl # commit d25689760b747287c6ca03cfe0729da63e0717f4
	dnl # drm/amdgpu/display: Keep malloc ref to MST port
	dnl #
	dnl # commit ebcc0e6b509108b4a67daa4c55809a05ab7f4b77
	dnl # drm/dp_mst: Introduce new refcounting scheme for mstbs and ports
	dnl #
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_dp_mst_helper.h>
		], [
			drm_dp_mst_get_port_malloc(NULL);
			drm_dp_mst_put_port_malloc(NULL);
		], [
			AC_DEFINE(HAVE_DRM_DP_MST_GET_PUT_PORT_MALLOC, 1, [
				drm_dp_mst_{get,put}_port_malloc() is available])
		])
	])
	dnl #
	dnl # commit aad0eab4e8dd76d1ba5248f9278633829cbcec38
	dnl # drm/dp_mst: Enable registration of AUX devices for MST ports
	dnl #
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_dp_mst_helper.h>
		], [
			drm_dp_mst_connector_early_unregister(NULL, NULL);
			drm_dp_mst_connector_late_register(NULL, NULL);
		], [
			AC_DEFINE(HAVE_DP_MST_CONNECTOR_EARLY_UNREGISTER, 1, [
				drm_dp_mst_connector_early_unregister() is available])
			AC_DEFINE(HAVE_DP_MST_CONNECTOR_LATE_REGISTER, 1, [
				drm_dp_mst_connector_late_register() is available])
		])
	])
])

