dnl #
dnl # v5.3-rc1-684-g141f6357f45c
dnl # drm: tweak drm_print_bits()
dnl #
AC_DEFUN([AC_AMDGPU_DRM_PRINT_BITS], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_print.h>
		], [
			drm_print_bits(NULL, 0, NULL, 0);
		], [
			AC_DEFINE(HAVE_DRM_PRINT_BITS_4ARGS, 1,
				[drm_print_bits() has 4 args])
			AC_DEFINE(HAVE_DRM_PRINT_BITS, 1,
				[drm_print_bits() is available])
		], [
		dnl # v5.3-rc1-622-g2dc5d44ccc5e
		dnl # drm: add drm_print_bits
			AC_KERNEL_CHECK_SYMBOL_EXPORT([drm_print_bits],
				[drivers/gpu/drm/drm_print.c], [
				AC_DEFINE(HAVE_DRM_PRINT_BITS, 1,
					[drm_print_bits() is available])
			])
		])
	])
])
