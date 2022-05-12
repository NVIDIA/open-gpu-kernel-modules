dnl #
dnl # commit b3c11ac267d461d3d597967164ff7278a919a39f
dnl # drm: move allocation out of drm_get_format_name()
dnl #
AC_DEFUN([AC_AMDGPU_DRM_GET_FORMAT_NAME], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <drm/drm_fourcc.h>
		], [
			struct drm_format_name_buf *buf = NULL;
			drm_get_format_name(0, buf);
		], [drm_get_format_name], [drivers/gpu/drm/drm_fourcc.c], [
			AC_DEFINE(HAVE_DRM_GET_FORMAT_NAME_I_P, 1,
				[drm_get_format_name() has i,p interface])
		])
	])
])
