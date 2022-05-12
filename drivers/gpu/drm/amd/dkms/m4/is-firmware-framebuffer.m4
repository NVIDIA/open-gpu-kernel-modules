dnl #
dnl # commit a99952170b19db855b7b45fba8e263ddc5205a0c
dnl # drm/amdgpu: disable runpm if we are the primary adapter
dnl #

AC_DEFUN([AC_AMDGPU_IS_FIRMWARE_FRAMEBUFFER], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([is_firmware_framebuffer], [include/linux/fb.h], [
			AC_DEFINE(HAVE_IS_FIRMWARE_FRAMEBUFFER, 1, [is_firmware_framebuffer() is available])
		],[
		])
	])
])
