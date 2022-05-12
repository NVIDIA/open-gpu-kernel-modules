dnl #
dnl # v5.6-11591-g0bd476e6c671 kallsyms: unexport kallsyms_lookup_name() and kallsyms_on_each_symbol()
dnl # v2.6.32-rc4-272-gf60d24d2ad04 hw-breakpoints: Fix broken hw-breakpoint sample module
dnl #
AC_DEFUN([AC_AMDGPU_KALLSYMS_LOOKUP_NAME], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([kallsyms_lookup_name],
		[kernel/kallsyms.c],
		[
			AC_DEFINE(HAVE_KALLSYMS_LOOKUP_NAME, 1,
				[kallsyms_lookup_name is available])
		])
	])
])
