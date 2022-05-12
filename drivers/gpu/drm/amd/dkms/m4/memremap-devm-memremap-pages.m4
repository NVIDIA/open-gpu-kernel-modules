dnl #
dnl # e8d5134833006a46fcbefc5f4a84d0b62bd520e7
dnl # memremap: change devm_memremap_pages interface to use struct dev_pagemap
dnl #
AC_DEFUN([AC_AMDGPU_DEVM_MEMREMAP_PAGES], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/memremap.h>
		], [
			struct device *dev = NULL;
			struct dev_pagemap *pgmap = NULL;
			devm_memremap_pages(dev, pgmap);
		], [
			AC_DEFINE(HAVE_DEVM_MEMREMAP_PAGES_DEV_PAGEMAP, 1,
				[devm_memremap_pages() wants struct dev_pagemap])
		], [
			dnl #
			dnl # commit 5c2c2587b132
			dnl # mm, dax, pmem: introduce {get|put}_dev_pagemap()
			dnl #                for dax-gup
			dnl #
			AC_KERNEL_TRY_COMPILE([
				#include <linux/memremap.h>
			], [
				devm_memremap_pages(NULL, NULL, NULL, NULL);
			], [
				AC_DEFINE(HAVE_DEVM_MEMREMAP_PAGES_P_P_P_P, 1,
					[devm_memremap_pages() wants p,p,p,p interface])
			])
		])
	])
])
