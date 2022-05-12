dnl #
dnl # v5.0-rc1-998-gc6b38fbbde91
dnl # drm: move i915_kick_out_vgacon to vgaarb
dnl #
AC_DEFUN([AC_AMDGPU_VGA_REMOVE_VGACON], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/vgaarb.h>
		], [
			vga_remove_vgacon(NULL);
		], [
			AC_DEFINE(HAVE_VGA_REMOVE_VGACON, 1,
				[vga_remove_vgacon() is available])
		])
	])
])
