dnl #
dnl # v4.9-rc2-477-gd8187177b0b1 drm: add helper for printing to log or seq_file
dnl #
AC_DEFUN([AC_AMDGPU_DRM_PRINTER], [
	AC_KERNEL_DO_BACKGROUND([
		AS_IF([test $HAVE_DRM_DRM_PRINT_H], [
			dnl #
			dnl # v4.9-rc8-1738-gb5c3714fe878 drm/mm: Convert to drm_printer
			dnl # v4.9-rc8-1737-g3d387d923c18 drm/printer: add debug printer
			dnl #
			AC_KERNEL_TRY_COMPILE([
				#include <drm/drm_print.h>
			], [
				struct drm_printer *p = NULL;
				p->prefix = NULL;
			], [
				AC_DEFINE(HAVE_DRM_PRINTER_PREFIX, 1, [drm_printer->prefix is available])
			])
		])
	])
])
