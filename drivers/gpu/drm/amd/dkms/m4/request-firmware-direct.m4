dnl #
dnl # v3.13-rc2-51-gbba3a87e982a
dnl # firmware: Introduce request_firmware_direct()
dnl #
AC_DEFUN([AC_AMDGPU_REQUEST_FIRMWARE_DIRECT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/firmware.h>
		], [
			request_firmware_direct(NULL, NULL, NULL);
		], [
			AC_DEFINE(HAVE_REQUEST_FIRMWARE_DIRECT, 1,
				[request_firmware_direct() is available])
		])
	])
])
