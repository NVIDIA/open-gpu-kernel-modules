dnl #
dnl # commit v5.12-rc7-1498-g0c4fada608c1
dnl # drm/dp: Pass drm_dp_aux to drm_dp*_link_train_channel_eq_delay()
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DP_LINK_TRAIN_CHANNEL_EQ_DELAY], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <drm/drm_dp_helper.h>
		], [
			struct drm_dp_aux *aux = NULL;
			const u8 dpcd[DP_RECEIVER_CAP_SIZE];
			drm_dp_link_train_channel_eq_delay(aux, dpcd);
		], [drm_dp_link_train_channel_eq_delay],[drm/drm_dp_helper.c],[
			AC_DEFINE(HAVE_DRM_DP_LINK_TRAIN_CHANNEL_EQ_DELAY_2ARGS, 1,
				[drm_dp_link_train_channel_eq_delay() has 2 args])
		])
	])
])
