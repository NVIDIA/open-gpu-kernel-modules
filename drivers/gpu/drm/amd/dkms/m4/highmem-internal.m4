dnl #
dnl # commit f3ba3c710ac5a30cd058615a9eb62d2ad95bb782
dnl # mm/highmem: Provide kmap_local*
dnl #
AC_DEFUN([AC_AMDGPU_KMAP_LOCAL], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/highmem.h>
		], [
			pgprot_t prot;
			kmap_local_page_prot(NULL, prot);
		], [
			AC_DEFINE(HAVE_KMAP_LOCAL, 1, [kmap_local_* is available])
		])
	])
])
