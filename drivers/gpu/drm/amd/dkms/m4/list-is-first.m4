dnl #
dnl # commit 70b44595eafe9c7c235f076d653a268ca1ab9fdb
dnl # Author: Mel Gorman <mgorman@techsingularity.net>
dnl # Date:   Tue Mar 5 15:44:54 2019 -0800
dnl # mm, compaction: use free lists to quickly locate a migration source
dnl #
AC_DEFUN([AC_AMDGPU_LIST_IS_FIRST], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/list.h>
		], [
			list_is_first(NULL, NULL);
		], [
			AC_DEFINE(HAVE_LIST_IS_FIRST, 1,
				[list_is_first() is available])
		])
	])
])
