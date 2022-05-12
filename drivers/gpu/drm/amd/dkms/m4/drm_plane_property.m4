dnl #
dnl # v4.18-rc3-785-ga5ec8332d428 drm: Add per-plane pixel blend mode property
dnl # v4.16-rc7-1731-gae0e28265e21 drm/blend: Add a generic alpha property
dnl #
AC_DEFUN([AC_AMDGPU_DRM_PLANE_PROPERTY_ALPHA_BLEND_MODE], [
	AC_KERNEL_CHECK_SYMBOL_EXPORT(
		[drm_plane_create_alpha_property drm_plane_create_blend_mode_property],
		[drivers/gpu/drm/drm_blend.c],[
		AC_DEFINE(HAVE_DRM_PLANE_PROPERTY_ALPHA_BLEND_MODE, 1,
			[drm_plane_create_alpha_property, drm_plane_create_blend_mode_property are available])
	])
])

dnl #
dnl # v4.16-rc1-388-g80f690e9e3a6 drm: Add optional COLOR_ENCODING and COLOR_RANGE properties to drm_plane
dnl #
AC_DEFUN([AC_AMDGPU_DRM_PLANE_PROPERTY_COLOR_ENCODING_RANGE], [
	AC_KERNEL_CHECK_SYMBOL_EXPORT([drm_plane_create_color_properties],
		[drivers/gpu/drm/drm_color_mgmt.c],[
		AC_DEFINE(HAVE_DRM_PLANE_PROPERTY_COLOR_ENCODING_RANGE, 1,
			[drm_plane_create_color_properties is available])
	])
])

dnl #
dnl # v4.8-rc8-1454-d138dd3c0c70 drm: Add support for optional per-plane rotation property
dnl #
AC_DEFUN([AC_AMDGPU_DRM_PLANE_PROPERTY_ROTATION], [
	AC_KERNEL_CHECK_SYMBOL_EXPORT(
		[drm_plane_create_rotation_property],
		[drivers/gpu/drm/drm_blend.c],[
		AC_DEFINE(HAVE_DRM_PLANE_PROPERTY_ROTATION, 1,
			[drm_plane_create_rotation_property is available])
	])
])

AC_DEFUN([AC_AMDGPU_DRM_PLANE_PROPERTY], [
	AC_KERNEL_DO_BACKGROUND([
		AC_AMDGPU_DRM_PLANE_PROPERTY_ALPHA_BLEND_MODE
		AC_AMDGPU_DRM_PLANE_PROPERTY_COLOR_ENCODING_RANGE
		AC_AMDGPU_DRM_PLANE_PROPERTY_ROTATION
	])
])
