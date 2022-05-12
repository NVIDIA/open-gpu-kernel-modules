dnl #
dnl # commit 96d4f267e40f9509e8a66e2b39e8b95655617693
dnl # Author: Linus Torvalds <torvalds@linux-foundation.org>
dnl # Date:   Thu Jan 3 18:57:57 2019 -0800
dnl # Remove 'type' argument from access_ok() function
dnl #
AC_DEFUN([AC_AMDGPU_ACCESS_OK_WITH_TWO_ARGUMENTS], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/uaccess.h>
		],[
			access_ok(0, 0);
		],[
			AC_DEFINE(HAVE_ACCESS_OK_WITH_TWO_ARGUMENTS, 1,
				[whether access_ok(x, x) is available])
		])
	])
])
