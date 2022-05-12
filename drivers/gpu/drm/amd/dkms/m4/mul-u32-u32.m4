dnl #
dnl # commit v4.10-rc3-239-g9e3d6223d209
dnl # math64, timers: Fix 32bit mul_u64_u32_shr() and friends
dnl #
AC_DEFUN([AC_AMDGPU_MUL_U32_U32], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/math64.h>
		], [
			mul_u32_u32(0, 0);
		],[
			AC_DEFINE(HAVE_MUL_U32_U32, 1,
				[num_u32_u32 is available])
		])
	])
])
