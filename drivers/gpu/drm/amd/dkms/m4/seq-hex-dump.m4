dnl #
dnl # commit 37607102c4426cf92aeb5da1b1d9a79ba6d95e3f
dnl # seq_file: provide an analogue of print_hex_dump()
dnl #
AC_DEFUN([AC_AMDGPU_SEQ_HEX_DUMP], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <linux/seq_file.h>
		], [
			seq_hex_dump(NULL,NULL,0,0,0,NULL,0,0);
		], [seq_hex_dump],[fs/seq_file.c], [
			AC_DEFINE(HAVE_SEQ_HEX_DUMP, 1,
				[seq_hex_dump() is available])
		])
	])
])
