dnl #
dnl #
dnl # is_smca_umc_v2()
dnl #
AC_DEFUN([AC_AMDGPU_CHECK_SMCA_UMC_V2], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([is_smca_umc_v2],
		[arch/x86/kernel/cpu/mce/amd.c], [
			AC_DEFINE(HAVE_SMCA_UMC_V2, 1,
				[is_smca_umc_v2() is available])
		])
	])
])
