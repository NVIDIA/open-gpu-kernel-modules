dnl #
dnl # commit v4.19-6927-gf5e6d1d5f8f3
dnl # mm: introduce vmf_insert_pfn_prot()
dnl #
AC_DEFUN([AC_AMDGPU_VMF_INSERT_PFN_PROT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/mm.h>
			#include <asm/page.h>
		],[
			pgprot_t prot;
			vmf_insert_pfn_prot(NULL, 0, 0, prot);
		],[
			AC_DEFINE(HAVE_VMF_INSERT_PFN_PROT,
				1,
				[vmf_insert_pfn_prot() is available])
		],[
		dnl #
		dnl # commit v4.4-528-g1745cbc5d0de
		dnl # mm: Add vm_insert_pfn_prot()
		dnl #
			AC_KERNEL_TRY_COMPILE([
				#include <linux/mm.h>
				#include <asm/page.h>
			],[
				pgprot_t prot;
				vm_insert_pfn_prot(NULL, 0, 0, prot);
			],[
				AC_DEFINE(HAVE_VM_INSERT_PFN_PROT,
					1,
					[vm_insert_pfn_prot() is available])
			])
		])
	])
])
