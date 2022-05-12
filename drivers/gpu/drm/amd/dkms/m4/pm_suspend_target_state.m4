dnl #
dnl # commit edf3ad32f18b0ea7d27ea9420f3bb9b2c850b48b
dnl # drm/amd: Warn users about potential s0ix problems
dnl #
AC_DEFUN([AC_AMDGPU_PM_SUSPEND_TARGET_STATE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/suspend.h>
		],[
			pm_suspend_target_state = PM_SUSPEND_TO_IDLE;
		],[
			AC_DEFINE(HAVE_PM_SUSPEND_TARGET_STATE,
				1,
				[pm_suspend_target_state is available])
		])
	])
])
