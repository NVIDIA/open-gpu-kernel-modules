dnl #
dnl # commit df2fc43d09d3ee5ede82cab9299df5e78aa427b5
dnl # Author: Christian König <christian.koenig@amd.com>
dnl # Date:   Thu Sep 13 11:17:23 2018 +0200
dnl # list: introduce list_bulk_move_tail helper
dnl #
AC_DEFUN([AC_AMDGPU_LIST_BULK_MOVE_TAIL], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/list.h>
		], [
			list_bulk_move_tail(NULL, NULL, NULL);
		], [
			AC_DEFINE(HAVE_LIST_BULK_MOVE_TAIL, 1,
				[list_bulk_move_tail() is available])
		])
	])
])
