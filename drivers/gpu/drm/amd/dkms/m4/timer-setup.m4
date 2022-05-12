dnl #
dnl # timer_setup is available
dnl #
dnl #
AC_DEFUN([AC_AMDGPU_TIMER_SETUP], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/timer.h>
		],[
			timer_setup(NULL, NULL, 0);
		],[
			AC_DEFINE(HAVE_TIMER_SETUP, 1,
				[timer_setup() is available])
		])
	])
])
