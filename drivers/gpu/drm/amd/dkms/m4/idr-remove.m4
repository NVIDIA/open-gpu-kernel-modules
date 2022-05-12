dnl #
dnl # commit d3e709e63e97e5f3f129b639991cfe266da60bae
dnl # Author: Matthew Wilcox <mawilcox@microsoft.com>
dnl # Date:   Thu Dec 22 13:30:22 2016 -0500
dnl # idr: Return the deleted entry from idr_remove
dnl #
AC_DEFUN([AC_AMDGPU_IDR_REMOVE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/idr.h>
		], [
			void *i;
			i = idr_remove(NULL, 0);
		], [
			AC_DEFINE(HAVE_IDR_REMOVE_RETURN_VOID_POINTER, 1,
				[idr_remove return void pointer])
		])
	])
])
