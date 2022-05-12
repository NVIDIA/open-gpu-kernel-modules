dnl #
dnl # v4.18-rc3-683-g97e14fbeb53f drm: drop _mode_ from remaining connector functions
dnl # v4.18-rc3-682-gcde4c44d8769 drm: drop _mode_ from drm_mode_connector_attach_encode
dnl # v4.18-rc3-681-gc555f02371c3 drm: drop _mode_ from update_edit_property()
dnl #
AC_DEFUN([AC_AMDGPU_DRM_CONNECTOR_XXX_DROP_MODE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT(
			[drm_connector_update_edid_property drm_connector_attach_encoder drm_connector_set_path_property],
			[drivers/gpu/drm/drm_connector.c], [
			AC_DEFINE(HAVE_DRM_CONNECTOR_XXX_DROP_MODE, 1,
					[drm_connector_xxx() drop _mode_])
		])
	])
])
