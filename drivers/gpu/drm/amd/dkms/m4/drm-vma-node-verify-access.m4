dnl #
dnl # commit v4.8-rc2-780-gd9a1f0b4eb60
dnl # drm: use drm_file to tag vm-bos
dnl #
AC_DEFUN([AC_AMDGPU_DRM_VMA_NODE_VERIFY_ACCESS], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_vma_manager.h>
		], [
			struct drm_vma_offset_node *node = NULL;
			struct drm_file *tag = NULL;
			drm_vma_node_verify_access(node, tag);
		], [
			AC_DEFINE(HAVE_DRM_VMA_NODE_VERIFY_ACCESS_HAS_DRM_FILE, 1, [drm_vma_node_verify_access() 2nd argument is drm_file])
		])
	])
])
