dnl #
dnl # commit f808c13fd3738948e10196496959871130612b61
dnl # lib/interval_tree: fast overlap detection
dnl #
AC_DEFUN([AC_AMDGPU_INTERVAL_TREE_INSERT_HAVE_RB_ROOT_CACHED], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/interval_tree.h>
		],[
			struct rb_root_cached *r = NULL;
			interval_tree_insert(NULL, r);
		],[
			AC_DEFINE(HAVE_TREE_INSERT_HAVE_RB_ROOT_CACHED, 1,
				[interval_tree_insert have struct rb_root_cached])
		])
	])
])

