dnl #
dnl # commit v4.5-2572-gd02bd27bd33d
dnl # mm/page_alloc.c: calculate 'available' memory in a separate function
dnl #
AC_DEFUN([AC_AMDGPU_SI_MEM_AVAILABLE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <linux/mm.h>
		], [
			si_mem_available();
		], [si_mem_available], [mm/page_alloc.c], [
			AC_DEFINE(HAVE_SI_MEM_AVAILABLE, 1, [whether si_mem_available() is available])
		])
	])
])

