dnl #
dnl # commit e66f6e095486f0210fcf3c5eb3ecf13fa348be4c
dnl # io-mapping: Provide iomap_local variant
dnl #
AC_DEFUN([AC_AMDGPU_IO_MAPPING_UNMAP_LOCAL], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/io-mapping.h>
		], [
			io_mapping_unmap_local(NULL);
		], [
			AC_DEFINE(HAVE_IO_MAPPING_UNMAP_LOCAL, 1, [io_mapping_unmap_local() is available])
		])
	])
])
