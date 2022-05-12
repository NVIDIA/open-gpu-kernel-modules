dnl #
dnl # commit aaf285e2e0ff490e924dbcdfd08e8274c3093354
dnl # drm: Add a callback from connector registering
dnl #
AC_DEFUN([AC_AMDGPU_DRM_CONNECTOR_FUNCS_REGISTER_MEMBER], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_crtc.h>
		], [
			struct drm_connector_funcs *funcs = NULL;
			funcs->early_unregister(NULL);
		], [
			AC_DEFINE(HAVE_DRM_CONNECTOR_FUNCS_REGISTER, 1,
				[struct drm_connector_funcs has register members])
		])
	])
])
