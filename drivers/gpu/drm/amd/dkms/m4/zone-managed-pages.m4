dnl #
dnl # commit v4.20-6505-g9705bea5f833
dnl # Author: Arun KS <arunks@codeaurora.org>
dnl # Date:   Fri Dec 28 00:34:24 2018 -0800
dnl # mm: convert zone->managed_pages to atomic variable
dnl #
AC_DEFUN([AC_AMDGPU_ZONE_MANAGED_PAGES], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/mmzone.h>
		],[
			zone_managed_pages(NULL);
		],[
			AC_DEFINE(HAVE_ZONE_MANAGED_PAGES, 1,
				[zone_managed_pages() is available])
		],[
			dnl #
			dnl # commit v3.7-4152-g9feedc9d831e
			dnl # mm: introduce new field "managed_pages" to struct zone
			dnl #
			AC_KERNEL_TRY_COMPILE([
				#include <linux/mmzone.h>
			], [
				struct zone *z = NULL;
				z->managed_pages = 0;
			], [
				AC_DEFINE(HAVE_STRUCT_ZONE_MANAGED_PAGES, 1,
					[zone->managed_pages is available])
			])
		])
	])
])
