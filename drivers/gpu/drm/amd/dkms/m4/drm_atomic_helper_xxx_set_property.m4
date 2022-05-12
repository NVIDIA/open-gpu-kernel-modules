dnl #
dnl # v4.13-rc2-369-g7d902c05b480 drm: Nuke drm_atomic_helper_connector_dpms
dnl # v4.13-rc2-368-g482b0e3c2fd7 drm: Nuke drm_atomic_helper_connector_set_property
dnl # v4.13-rc2-367-ge90271bc07ed drm: Nuke drm_atomic_helper_plane_set_property
dnl # v4.13-rc2-366-gb6715570c10d drm: Nuke drm_atomic_helper_crtc_set_property
dnl #
AC_DEFUN([AC_AMDGPU_DRM_ATOMIC_HELPER_XXX_SET_PROPERTY], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([
			drm_atomic_helper_crtc_set_property
			drm_atomic_helper_plane_set_property
			drm_atomic_helper_connector_set_property
			drm_atomic_helper_connector_dpms
		],[drivers/gpu/drm/drm_atomic_helper.c],[
			AC_DEFINE(HAVE_DRM_ATOMIC_HELPER_XXX_SET_PROPERTY, 1,
				[{drm_atomic_helper_crtc_set_property, drm_atomic_helper_plane_set_property, drm_atomic_helper_connector_set_property, drm_atomic_helper_connector_dpms} is available])
		])
	])
])
