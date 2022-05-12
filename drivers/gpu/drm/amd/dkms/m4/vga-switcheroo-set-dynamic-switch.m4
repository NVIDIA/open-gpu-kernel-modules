dnl #
dnl # commit 07f4f97d7b4bf325d9f558c5b58230387e4e57e0
dnl # Author: Lukas Wunner <lukas@wunner.de>
dnl # vga_switcheroo: Use device link for HDA controller
dnl #
AC_DEFUN([AC_AMDGPU_VGA_SWITCHEROO_SET_DYNAMIC_SWITCH], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/vga_switcheroo.h>
		], [
			vga_switcheroo_set_dynamic_switch(NULL, 0);
		],[
			AC_DEFINE(HAVE_VGA_SWITCHEROO_SET_DYNAMIC_SWITCH, 1,
				[vga_switcheroo_set_dynamic_switch() exist])
		])
	])
])
