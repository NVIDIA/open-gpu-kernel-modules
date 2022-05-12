dnl #
dnl # commit 8760c909f54a82aaa6e76da19afe798a0c77c3c3
dnl # file: Rename __close_fd to close_fd and remove the files parameter
dnl #
AC_DEFUN([AC_AMDGPU_CLOSE_FD], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/fdtable.h>
		], [
			close_fd(0);
		], [
			AC_DEFINE(HAVE_KERNEL_CLOSE_FD, 1, [close_fd() is available])
		], [
				dnl #
				dnl # commit 16a78543a1d3537645de737934b9387c42bfb53b
				dnl # drm/amdkcl: fix for close_fd not defined
				dnl #
				AC_KERNEL_TRY_COMPILE([
					#include <linux/syscalls.h>
				], [
					ksys_close(0);
				], [
					AC_DEFINE(HAVE_KSYS_CLOSE_FD, 1, [ksys_fd() is available])
				])
		])
	])
])
