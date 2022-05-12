dnl #
dnl # commit e66f6e095486f0210fcf3c5eb3ecf13fa348be4c
dnl # io-mapping: Provide iomap_local variant
dnl #
AC_DEFUN([AC_AMDGPU_IO_MAPPING_MAP_LOCAL_WC], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/io-mapping.h>
		], [
			io_mapping_map_local_wc(NULL, 0);
		], [
			AC_DEFINE(HAVE_IO_MAPPING_MAP_LOCAL_WC, 1, [io_mapping_map_local_wc() is available])
		])
	])
])
