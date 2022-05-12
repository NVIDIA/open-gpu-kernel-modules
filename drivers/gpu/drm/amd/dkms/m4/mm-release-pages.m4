dnl #
dnl # commit c6f92f9fbe7dbcc8903a67229aa88b4077ae4422
dnl # mm: remove cold parameter for release_pages
dnl #
AC_DEFUN([AC_AMDGPU_MM_RELEASE_PAGES], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <linux/pagemap.h>
		], [
			struct page **pages = NULL;
			int nr = 0;

			release_pages(pages, nr);
		], [release_pages], [mm/swap.c], [
			AC_DEFINE(HAVE_MM_RELEASE_PAGES_2ARGS, 1,
				[release_pages() wants 2 args])
		])
	])
])
