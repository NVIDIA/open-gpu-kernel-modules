dnl #
dnl # commit v4.14-rc8-89-gd8aa7eea78a1
dnl # x86/mm: Add Secure Encrypted Virtualization (SEV) support
dnl #
AC_DEFUN([AC_AMDGPU_MEM_ENCRYPT_ACTIVE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/types.h>
			#include <linux/mem_encrypt.h>
		], [
			mem_encrypt_active();
		], [
			AC_DEFINE(HAVE_MEM_ENCRYPT_ACTIVE, 1,
				[mem_encrypt_active() is available])
		])
	])
])
