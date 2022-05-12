dnl #
dnl # commit v4.3-rc5-6-gef25ba047601
dnl # PM / sleep: Add flags to indicate platform firmware involvement
dnl #
AC_DEFUN([AC_AMDGPU_PM_SUSPEND_VIA_FIRMWARE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/suspend.h>
		],[
			pm_suspend_via_firmware();
		],[
			AC_DEFINE(HAVE_PM_SUSPEND_VIA_FIRMWARE,
				1,
				[pm_suspend_via_firmware() is available])
		])
	])
])
