dnl #
dnl # commit v5.13-2803-gcf95d5c0c941
dnl # drm: Update MST First Link Slot Information Based on Encoding Format
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DP_UPDATE_PAYLOAD_PART1_START_SLOT_ARG], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_dp_mst_helper.h>
		], [
			drm_dp_update_payload_part1(NULL, 0);
		], [
			AC_DEFINE(HAVE_DRM_DP_UPDATE_PAYLOAD_PART1_START_SLOT_ARG, 1,
				[drm_dp_update_payload_part1() function has start_slot argument])
		])
	])
])
