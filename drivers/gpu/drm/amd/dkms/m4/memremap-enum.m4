dnl #
dnl # commit c907e0eb43a522de60fb651c011c553f87273222 
dnl # memremap: add MEMREMAP_WC flag
dnl #
AC_DEFUN([AC_AMDGPU_MEMREMAP_WC], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/io.h>
		], [
			int v = MEMREMAP_WC;
		], [
			AC_DEFINE(HAVE_MEMREMAP_WC, 1,
				[enum MAMREMAP_WC is availablea])
		])
	])
])
