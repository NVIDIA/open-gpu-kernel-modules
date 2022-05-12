dnl #
dnl # commit: v5.9-rc5-23-g2efc459d06f1
dnl # sysfs: Add sysfs_emit and sysfs_emit_at
dnl # to format sysfs output
AC_DEFUN([AC_AMDGPU_SYSFS_EMIT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([sysfs_emit sysfs_emit_at],
			[fs/sysfs/file.c], [
			AC_DEFINE(HAVE_SYSFS_EMIT, 1,
				[sysfs_emit() and sysfs_emit_at() are available])
		])
	])
])


)
