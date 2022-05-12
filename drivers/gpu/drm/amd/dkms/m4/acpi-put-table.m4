dnl #
dnl # commit: v4.9-rc5-17-g174cc7187e6f
dnl # ACPICA: Tables: Back port acpi_get_table_with_size() and 
dnl # early_acpi_os_unmap_memory() from Linux kernel
AC_DEFUN([AC_AMDGPU_ACPI_PUT_TABLE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([acpi_put_table],
			[drivers/acpi/acpica/tbxface.c], [
			AC_DEFINE(HAVE_ACPI_PUT_TABLE, 1,
				[acpi_put_table() is available])
		])
	])
])
