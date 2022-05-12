dnl #
dnl # commit v4.6-rc6-1-gf6a4790a5471
dnl # video / backlight: add two APIs for drivers to use
dnl #
AC_DEFUN([AC_AMDGPU_BACKLIGHT_DEVICE_SET_BRIGHTNESS], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <linux/backlight.h>
		], [
			backlight_device_set_brightness(NULL, 0);
		], [backlight_device_set_brightness], [drivers/video/backlight/backlight.c], [
			AC_DEFINE(HAVE_BACKLIGHT_DEVICE_SET_BRIGHTNESS, 1,
				[backlight_device_set_brightness() is available])
		])
	])
])
