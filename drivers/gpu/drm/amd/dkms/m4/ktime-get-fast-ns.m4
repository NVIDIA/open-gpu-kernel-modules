dnl #
dnl # commit v3.16-rc5-111-g4396e058c52e
dnl # timekeeping: Provide fast and NMI safe access to CLOCK_MONOTONIC
dnl #
AC_DEFUN([AC_AMDGPU_KTIME_GET_FAST_NS], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/ktime.h>
		], [
			ktime_get_mono_fast_ns();
		], [
			AC_DEFINE(HAVE_KTIME_GET_MONO_FAST_NS, 1,
				[ktime_get_mono_fast_ns is available])
		])
	])
])
