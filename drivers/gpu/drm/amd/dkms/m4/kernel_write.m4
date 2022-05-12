dnl #
dnl # commit v4.13-rc7-6-ge13ec939e96b
dnl # fs: fix kernel_write prototype
dnl #
AC_DEFUN([AC_AMDGPU_KERNEL_WRITE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/fs.h>
		], [
			kernel_write(NULL, NULL, 0, NULL);
		], [
			AC_DEFINE(HAVE_KERNEL_WRITE_PPOS, 1,
				[kernel_write() take arg type of position as pointer])
		])
	])
])
