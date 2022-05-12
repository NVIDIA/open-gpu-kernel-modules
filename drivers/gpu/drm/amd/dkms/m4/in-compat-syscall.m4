dnl #
dnl # v4.5-11126-g5180e3e24fd3
dnl # compat: add in_compat_syscall to ask whether we're in a compat syscall
dnl #
AC_DEFUN([AC_AMDGPU_IN_COMPAT_SYSCALL], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/compat.h>
		], [
			in_compat_syscall();
		],[
			AC_DEFINE(HAVE_IN_COMPAT_SYSCALL, 1,
				[in_compat_syscall is defined])
		])
	])
])
