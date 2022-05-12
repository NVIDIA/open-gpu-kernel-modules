dnl #
dnl # commit v4.15-rc1-4-g8ced390c2b18
dnl # define __poll_t, annotate constants
dnl #
AC_DEFUN([AC_AMDGPU_TYPE__POLL_T], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <uapi/linux/types.h>
		],[
			__poll_t mask = 0;
		],[
			AC_DEFINE(HAVE_TYPE__POLL_T, 1, [__poll_t is available])
		])
	])
])

