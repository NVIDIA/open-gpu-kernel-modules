dnl #
dnl # v4.13-rc4-164-gd92a8cfcb37e
dnl # locking/lockdep: Rework FS_RECLAIM annotation
dnl #
AC_DEFUN([AC_AMDGPU_FS_RECLAIM_ACQUIRE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/sched/mm.h>
		],[
			fs_reclaim_acquire(0);
		],[
			AC_DEFINE(HAVE_FS_RECLAIM_ACQUIRE, 1, [fs_reclaim_acquire() is available])
		])
	])
])
