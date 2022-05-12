dnl #
dnl # commit d1ed7985b9a6b85ea38a330108c51ec83381c01b
dnl # Author: Peter Rosin <peda@axentia.se>
dnl # Date:   Thu Aug 25 23:07:01 2016 +0200
dnl # i2c: move locking operations to their own structure
dnl #
AC_DEFUN([AC_AMDGPU_I2C_LOCK_OPERATIONS_STRUCT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/i2c.h>
		], [
			struct i2c_lock_operations drm_dp_i2c_lock_ops;
			drm_dp_i2c_lock_ops.lock_bus = NULL;
		], [
			AC_DEFINE(HAVE_I2C_LOCK_OPERATIONS_STRUCT, 1,
				[struct i2c_lock_operations is defined])
		])
	])
])
