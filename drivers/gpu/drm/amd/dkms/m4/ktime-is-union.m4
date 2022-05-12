dnl #
dnl # commit v4.18-rc1-35-ga8802d97e733
dnl # ktime: Get rid of the union
dnl #
AC_DEFUN([AC_AMDGPU_KTIME_IS_UNION], [
	AC_KERNEL_DO_BACKGROUND([
	AC_KERNEL_TRY_COMPILE([
		#include <linux/ktime.h>
	], [
		ktime_t t;
		t.tv64 = 0;
	], [
		AC_DEFINE(HAVE_KTIME_IS_UNION, 1,
				[ktime_t is union])
	])
	])
])
