dnl #
dnl # commit 1c542f38ab8d30d9c852a16d49ac5a15267bbf1f
dnl # mm: Introduce kvcalloc()
dnl #
AC_DEFUN([AC_AMDGPU_KVCALLOC], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/mm.h>
		], [
			kvcalloc(0, 0, 0);
		], [
			AC_DEFINE(HAVE_KVCALLOC, 1, [kvcalloc() is available])
		])
	])
])
