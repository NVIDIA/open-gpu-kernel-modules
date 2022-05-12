AC_DEFUN([AC_AMDGPU_KTIME_GET_REAL_SECONDS], [
	AC_KERNEL_DO_BACKGROUND([
		dnl #
		dnl # commit dbe7aa622db96b5cd601f59d09c4f00b98b76079
		dnl # timekeeping: Provide y2038 safe accessor to the seconds portion of CLOCK_REALTIME
		dnl #
		AC_KERNEL_TRY_COMPILE([
			#ifdef HAVE_DRM_DRM_BACKPORT_H
			#include <drm/drm_backport.h>
			#endif
			#include <linux/ktime.h>
			#include <linux/timekeeping.h>
		], [
			ktime_get_real_seconds();
		], [
			AC_DEFINE(HAVE_KTIME_GET_REAL_SECONDS, 1,
				[ktime_get_real_seconds() is available])
		])
	])
])
