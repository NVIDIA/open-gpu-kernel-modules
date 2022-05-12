dnl #
dnl # commit 2c935bc57221cc2edc787c72ea0e2d30cdcd3d5e
dnl # locking/atomic, kref: Add kref_read()
dnl #
AC_DEFUN([AC_AMDGPU_KREF_READ], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/kref.h>
		], [
			kref_read(NULL);
		], [
			AC_DEFINE(HAVE_KREF_READ, 1,
				[kref_read() function is available])
		])
	])
])
