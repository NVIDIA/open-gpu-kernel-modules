dnl #
dnl # commit a7c3e901a46ff54c016d040847eda598a9e3e653
dnl # mm: introduce kv[mz]alloc helpers
dnl #
AC_DEFUN([AC_AMDGPU_KVZALLOC_KVMALLOC], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/mm.h>
		], [
			kvmalloc(0, 0);
			kvzalloc(0, 0);
		], [
			AC_DEFINE(HAVE_KVZALLOC_KVMALLOC, 1,
				[kv[mz]alloc() are available])
		])
	])
])
