dnl #
dnl # commit v4.4-6466-g34c0fd540e79
dnl # mm, dax, pmem: introduce pfn_t
dnl #
AC_DEFUN([AC_AMDGPU_VMF_INSERT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/mm.h>
		], [
			pfn_t pfn;
			pfn.val = 0;
		], [
			dnl #
			dnl # commit v4.16-7358-g1c8f422059ae
			dnl # mm: change return type to vm_fault_t
			dnl #
			AC_DEFINE(HAVE_PFN_T, 1, [pfn_t is defined])

			AC_KERNEL_TRY_COMPILE([
				#include <linux/mm.h>
			], [
				pfn_t pfn = {};
				vmf_insert_mixed(NULL, 0, pfn);
				vmf_insert_pfn(NULL, 0, 0);
			], [
				AC_DEFINE(HAVE_VMF_INSERT, 1,
					[vmf_insert_*() are available])
			], [
				dnl #
				dnl # commit v4.4-6475-g01c8f1c44b83
				dnl # mm, dax, gpu: convert vm_insert_mixed to pfn_t
				dnl #
				AC_KERNEL_TRY_COMPILE_SYMBOL([
					#include <linux/mm.h>
				], [
					pfn_t pfn = {};
					vm_insert_mixed(NULL, 0, pfn);
				], [vm_insert_mixed], [mm/memory.c], [
					AC_DEFINE(HAVE_PFN_T_VM_INSERT_MIXED, 1,
						[vm_insert_mixed() wants pfn_t arg])
				])
			])
		])
	])
])
