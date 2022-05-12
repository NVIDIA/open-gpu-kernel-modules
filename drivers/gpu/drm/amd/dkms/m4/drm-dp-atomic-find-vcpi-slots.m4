dnl #
dnl # commit edb1ed1ab7d314e114de84003f763da34c0f34c0
dnl # drm/dp: Add DP MST helpers to atomically find and release vcpi slots
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DP_ATOMIC_FIND_VCPI_SLOTS], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <drm/drm_dp_mst_helper.h>
		], [
			int retval;
			retval = drm_dp_atomic_find_vcpi_slots(NULL, NULL, NULL, 0);
		], [drm_dp_atomic_find_vcpi_slots], [drivers/gpu/drm/drm_dp_mst_topology.c], [
			AC_DEFINE(HAVE_DRM_DP_ATOMIC_FIND_VCPI_SLOTS, 1,
				[drm_dp_atomic_find_vcpi_slots() is available])
		], [
			dnl #
			dnl # commit dad1c2499a8f6d7ee01db8148f05ebba73cc41bd
			dnl # drm/dp_mst: Manually overwrite PBN divider for calculating timeslots
			dnl #
			AC_KERNEL_TRY_COMPILE([
				#include <drm/drm_dp_mst_helper.h>
			], [
				int retval;
				retval = drm_dp_atomic_find_vcpi_slots(NULL, NULL, NULL, 0, 0);
			], [
				AC_DEFINE(HAVE_DRM_DP_ATOMIC_FIND_VCPI_SLOTS_5ARGS, 1,
					[drm_dp_atomic_find_vcpi_slots() wants 5args])
				AC_DEFINE(HAVE_DRM_DP_ATOMIC_FIND_VCPI_SLOTS, 1,
					[drm_dp_atomic_find_vcpi_slots() is available])
			])
		])
	])
])
