dnl #
dnl # commit v5.12-rc7-1497-g9e9866664456
dnl # drm/dp: Pass drm_dp_aux to drm_dp_link_train_clock_recovery_delay()
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DP_LINK_TRAIN_CLOCK_RECOVERY_DELAY], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <drm/drm_dp_helper.h>
		], [
			struct drm_dp_aux *aux = NULL;
			const u8 dpcd[DP_RECEIVER_CAP_SIZE];
			drm_dp_link_train_clock_recovery_delay(aux, dpcd);
		], [drm_dp_link_train_clock_recovery_delay],[drm/drm_dp_helper.c],[
			AC_DEFINE(HAVE_DRM_DP_LINK_TRAIN_CLOCK_RECOVERY_DELAY_2ARGS, 1,
				[drm_dp_link_train_clock_recovery_delay() has 2 args])
		])
	])
])
