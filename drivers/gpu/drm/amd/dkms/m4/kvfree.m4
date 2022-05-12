dnl #
dnl # commit 39f1f78d53b9bcbca91967380c5f0f2305a5c55f
dnl # nick kvfree() from apparmor
dnl #
AC_DEFUN([AC_AMDGPU_KVFREE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <linux/mm.h>
		], [
			kvfree(NULL);
		], [kvfree], [mm/util.c], [
			AC_DEFINE(HAVE_KVFREE, 1, [kvfree() is available])
		])
	])
])
