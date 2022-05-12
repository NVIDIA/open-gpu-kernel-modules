dnl #
dnl # commit v5.4-rc2-1-g2952db0fd51b
dnl # compat_ioctl: add compat_ptr_ioctl()
dnl #
AC_DEFUN([AC_AMDGPU_COMPAT_PTR_IOCTL], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <linux/fs.h>
		],[
			compat_ptr_ioctl(NULL, 0, 0);
		],[compat_ptr_ioctl],[fs/ioctl.c],[
			AC_DEFINE(HAVE_COMPAT_PTR_IOCTL,
				1,
				[compat_ptr_ioctl() is available])
		])
	])
])
