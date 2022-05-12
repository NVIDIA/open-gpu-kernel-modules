dnl #
dnl # v5.8-rc1-23-g7318d4cc14c8
dnl # sched: Provide sched_set_fifo()
dnl #
AC_DEFUN([AC_AMDGPU_SCHED_SET_FIFO_LOW], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([sched_set_fifo_low],
		[kernel/sched/core.c], [
			AC_DEFINE(HAVE_SCHED_SET_FIFO_LOW, 1,
				[sched_set_fifo_low() is available])
		])
	])
])
