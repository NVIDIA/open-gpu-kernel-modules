dnl #
dnl # commit a4574f63edc6f76fb46dcd65d3eb4d5a8e23ba38
dnl # mm/memremap_pages: convert to 'struct range'
dnl #
AC_DEFUN([AC_AMDGPU_DEV_PAGEMAP_RANGE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/memremap.h>
		], [
			struct dev_pagemap *pm = NULL;
			pm->range.start = 0;
		], [
			AC_DEFINE(HAVE_DEV_PAGEMAP_RANGE, 1,
				[there is 'range' field within dev_pagemap structure])
		])
	])
])
AC_DEFUN([AC_AMDGPU_DEV_PAGEMAP], [
	AC_AMDGPU_DEV_PAGEMAP_RANGE
])

