dnl #
dnl # commit a16b53849913e742d086bb2b6f5e069ea2850c56
dnl # Author: Tobin C. Harding <tobin@kernel.org>
dnl # Date:   Mon May 13 17:15:59 2019 -0700
dnl # list: add function list_rotate_to_front()
dnl #
AC_DEFUN([AC_AMDGPU_LIST_ROTATE_TO_FRONT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/list.h>
		], [
			list_rotate_to_front(NULL, NULL);
		], [
			AC_DEFINE(HAVE_LIST_ROTATE_TO_FRONT, 1,
				[list_rotate_to_front() is available])
		])
	])
])
