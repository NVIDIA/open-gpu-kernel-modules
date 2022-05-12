dnl #
dnl # v4.19-rc5-244-gf8d5d0cc145c
dnl # xarray: Add definition of struct xarray
dnl #
AC_DEFUN([AC_AMDGPU_STRUCT_XARRAY], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/xarray.h>
		], [
			struct xarray x;
			xa_init(&x);
		], [
			AC_DEFINE(HAVE_STRUCT_XARRAY, 1,
				[struct xarray is available])
		])
	])
])
