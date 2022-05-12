dnl #
dnl # commit v5.2-rc5-8-g9285ec4c8b61
dnl # timekeeping: Use proper clock specifier names in functions
dnl #
AC_DEFUN([AC_AMDGPU_KTIME_GET_BOOTTIME_NS], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/ktime.h>
		], [
			ktime_get_boottime_ns();
		], [
			AC_DEFINE(HAVE_KTIME_GET_BOOTTIME_NS, 1,
				[ktime_get_boottime_ns() is available])
			AC_DEFINE(HAVE_KTIME_GET_NS, 1,
				[ktime_get_ns is available])
		],[
			dnl #
			dnl # commit v3.16-rc5-76-g897994e32b2b
			dnl # timekeeping: Provide ktime_get[*]_ns() helpers
			dnl #
			AC_KERNEL_TRY_COMPILE([
				#include <linux/ktime.h>
				#include <linux/timekeeping.h>
			], [
				ktime_get_ns();
			], [
				AC_DEFINE(HAVE_KTIME_GET_NS, 1,
					[ktime_get_ns is available])
			])
		])
	])
])
