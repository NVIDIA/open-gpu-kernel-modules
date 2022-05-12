AC_DEFUN([AC_AMDGPU_CONFIG], [
	AC_ARG_ENABLE([linux-builtin],
		[AC_HELP_STRING([--enable-linux-builtin],
		[Configure for builtin kernel modules @<:@default=no@:>@])],
		[],
		[enable_linux_builtin=no])

	AC_CONFIG_KERNEL
])
