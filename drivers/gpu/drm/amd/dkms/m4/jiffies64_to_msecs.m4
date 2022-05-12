dnl #
dnl # v5.1-rc3-699-g3b15d09f7e6d
dnl # time: Introduce jiffies64_to_msecs()
dnl #
AC_DEFUN([AC_AMDGPU_JIFFIES64_TO_MSECS], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([jiffies64_to_msecs], [kernel/time/time.c], [
			AC_DEFINE(HAVE_JIFFIES64_TO_MSECS, 1, [jiffies64_to_msecs() is available])
		])
	])
])
