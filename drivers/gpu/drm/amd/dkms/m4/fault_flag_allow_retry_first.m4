dnl #
dnl # commit v5.6-5709-g4064b9827063
dnl # mm: allow VM_FAULT_RETRY for multiple times
dnl #
AC_DEFUN([AC_AMDGPU_FAULT_FLAG_ALLOW_RETRY_FIRST], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/mm.h>
		], [
			fault_flag_allow_retry_first(0);
		], [
			AC_DEFINE(HAVE_FAULT_FLAG_ALLOW_RETRY_FIRST, 1,
				[fault_flag_allow_retry_first() is available])
		])
	])
])
