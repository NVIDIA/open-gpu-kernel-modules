dnl #
dnl # commit v3.16-rc5-99-gf519b1a2e08c
dnl # timekeeping: Provide ktime_get_raw()
dnl # Provide a ktime_t based interface for raw monotonic time.
dnl #
AC_DEFUN([AC_AMDGPU_KTIME_GET_RAW_NS], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/ktime.h>
			#include <linux/timekeeping.h>
		], [
			ktime_get_raw_ns();
		], [
			AC_DEFINE(HAVE_KTIME_GET_RAW_NS, 1,
				[ktime_get_raw_ns is available])
		])
	])
])
