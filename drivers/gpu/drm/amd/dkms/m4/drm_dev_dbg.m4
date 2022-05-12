dnl #
dnl # v4.16-rc1-493-gdb8708649258
dnl # drm: Reduce object size of DRM_DEV_<LEVEL> uses
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DEV_DBG], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([drm_dev_dbg], [drivers/gpu/drm/drm_print.c], [
			AC_DEFINE(HAVE_DRM_DEV_DBG, 1, [drm_dev_dbg() is available])
		])
	])
])
