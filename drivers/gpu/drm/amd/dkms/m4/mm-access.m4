dnl #
dnl # commit 4766d6eb3c11d
dnl # Copy in non-KFD changes
dnl # mm_access() is exported in v5.0-rc1-569-g4766d6eb3c11
dnl #
AC_DEFUN([AC_AMDGPU_MM_ACCESS], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([mm_access], [kernel/fork.c], [
			AC_DEFINE(HAVE_MM_ACCESS, 1, [mm_access() is available])
		])
	])
])
