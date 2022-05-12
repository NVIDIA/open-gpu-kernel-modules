dnl #
dnl # commit aa30f47cf666111f6bbfd15f290a27e8a7b9d854
dnl # kobject: Add support for default attribute groups to kobj_type
dnl #

AC_DEFUN([AC_AMDGPU_STRUCT_KOBJ_TYPE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/kobject.h>
		],[
			struct kobj_type *k_type = NULL;
			k_type->default_groups = NULL;
		],[
			AC_DEFINE(HAVE_DEFAULT_GROUP_IN_KOBJ_TYPE, 1,
				[kobj_type->default_groups is available])
		],[
		])
	])
])
