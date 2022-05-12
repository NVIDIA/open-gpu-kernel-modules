dnl #
dnl # v2.6.39-6856-g3864601387cf mm: extract exe_file handling from procfs
dnl #
AC_DEFUN([AC_AMDGPU_GET_MM_EXE_FILE], [
       AC_KERNEL_DO_BACKGROUND([
               AC_KERNEL_CHECK_SYMBOL_EXPORT([get_mm_exe_file], [kernel/fork.c], [
                       AC_DEFINE(HAVE_GET_MM_EXE_FILE, 1, [get_mm_exe_file() is available])
               ])
       ])
])
