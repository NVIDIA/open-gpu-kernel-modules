dnl #
dnl # 5379e4dd3220 mm, drm/ttm: Fix vm page protection handling
dnl # 574c5b3d0e4c mm: Add a vmf_insert_mixed_prot() function
dnl #
AC_DEFUN([AC_AMDGPU_VMF_INSERT_MIXED_PROT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <linux/mm.h>
			#include <asm/page.h>
		],[
			pfn_t pfn;
			pgprot_t prot;
			vmf_insert_mixed_prot(NULL, 0, pfn, prot);
		],[vmf_insert_mixed_prot],[mm/memory.c],[
			AC_DEFINE(HAVE_VMF_INSERT_MIXED_PROT,
				1,
				[vmf_insert_mixed_prot() is available])
		])
	])
])
