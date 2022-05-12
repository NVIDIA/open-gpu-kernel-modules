dnl #
dnl # commit d8dab00de9b767eaa11496a0eedf4798fc225803
dnl # io-mapping: Specify mapping size for io_mapping_map_wc()
dnl #
AC_DEFUN([AC_AMDGPU_IO_MAPPING_MAP_WC_HAS_SIZE_ARG], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/io-mapping.h>
		], [
			io_mapping_map_wc(NULL, 0, 0);
		], [
			AC_DEFINE(HAVE_IO_MAPPING_MAP_WC_HAS_SIZE_ARG, 1,
				[io_mapping_map_wc() has size argument])
		])
	])
])
