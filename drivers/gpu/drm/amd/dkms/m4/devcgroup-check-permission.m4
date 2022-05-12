dnl #
dnl # commit v5.3-rc3-2427-g4b7d4d453fc4
dnl # device_cgroup: Export devcgroup_check_permission
dnl #
AC_DEFUN([AC_AMDGPU_DEVCGROUP_CHECK_PERMISSION], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <linux/device_cgroup.h>
		], [
			devcgroup_check_permission(0, 0, 0, 0);
		], [devcgroup_check_permission], [security/device_cgroup.c], [
			AC_DEFINE(HAVE_DEVCGROUP_CHECK_PERMISSION, 1, [devcgroup_check_permission() is available])
		])
	])
])
