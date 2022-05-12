dnl #
dnl # commit aa475a59fff172ec858093fbc8471c0993081481
dnl # ACPICA: ACPI 6.3: SRAT: add Generic Affinity Structure subtable
dnl #
AC_DEFUN([AC_AMDGPU_ACPI_SRAT_GENERIC_AFFINITY], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/acpi.h>
		], [
			struct acpi_srat_generic_affinity *p = NULL;
			p->reserved = 0;
		], [
			AC_DEFINE(HAVE_ACPI_SRAT_GENERIC_AFFINITY, 1, [struct acpi_srat_generic_affinity is available])
		])
	])
])
