dnl #
dnl # commit v4.10-9602-g11bac8000449
dnl # mm, fs: reduce fault, page_mkwrite, and pfn_mkwrite to take only vmf
dnl #
AC_DEFUN([AC_AMDGPU_VM_OPERATIONS_STRUCT_FAULT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/mm.h>
		], [
			int (*fault)(struct vm_area_struct *vma, struct vm_fault *vmf) = 0;
			struct vm_operations_struct *vm_ops = NULL;
			vm_ops->fault(NULL);
		], [
			AC_DEFINE(HAVE_VM_OPERATIONS_STRUCT_FAULT_1ARG, 1,
				[vm_operations_struct->fault() wants 1 arg])
			AC_DEFINE(HAVE_VM_FAULT_ADDRESS_VMA, 1,
				[vm_fault->{address/vam} is available])
		], [
			dnl #
			dnl # commit v4.9-7746-g82b0f8c39a38
			dnl # mm: join struct fault_env and vm_fault
			dnl #
			AC_KERNEL_TRY_COMPILE([
				#include <linux/mm.h>
			], [
				struct vm_fault *ptest = NULL;
				ptest->address = 0;
				ptest->vma = NULL;
			], [
				AC_DEFINE(HAVE_VM_FAULT_ADDRESS_VMA, 1,
					[vm_fault->{address/vam} is available])
			])
		])
	])
])
