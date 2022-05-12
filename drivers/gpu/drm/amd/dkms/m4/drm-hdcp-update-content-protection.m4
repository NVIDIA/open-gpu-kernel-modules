dnl #
dnl # commit v5.3-rc1-380-gbb5a45d40d50
dnl # drm/hdcp: update content protection property with uevent
dnl #
AC_DEFUN([AC_AMDGPU_DRM_HDCP_UPDATE_CONTENT_PROTECTION], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_hdcp.h>
		], [
			drm_hdcp_update_content_protection(NULL, 0);
		], [
			AC_DEFINE(HAVE_DRM_HDCP_UPDATE_CONTENT_PROTECTION, 1,
				[drm_hdcp_update_content_protection is available])
		])
	])
])
