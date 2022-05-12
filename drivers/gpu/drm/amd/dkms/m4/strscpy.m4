dnl #
dnl # commit 30035e45753b708e7d47a98398500ca005e02b86
dnl # Author: Chris Metcalf <cmetcalf@ezchip.com>
dnl # Date:   Wed Apr 29 12:52:04 2015 -0400
dnl # string: provide strscpy()
dnl #
AC_DEFUN([AC_AMDGPU_STRSCPY], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <linux/string.h>
		], [
			strscpy(NULL, NULL, 8);
		], [strscpy], [lib/string.c], [
			AC_DEFINE(HAVE_STRSCPY, 1, [strscpy() is available])
		])
	])
])
