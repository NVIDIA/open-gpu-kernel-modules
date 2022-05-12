dnl #
dnl # v5.3-rc1-374-ge7f0141a217f	drm/ttm: drop ttm_buffer_object->resv
dnl # v5.3-rc1-370-g5a5011a72489	drm/amdgpu: switch driver from bo->resv to bo->base.resv
dnl # v5.3-rc1-367-ge532a135d704	drm/ttm: switch ttm core from bo->resv to bo->base.resv
dnl # v5.3-rc1-365-gb96f3e7c8069	drm/ttm: use gem vma_node
dnl # v5.3-rc1-364-g1e053b10ba60	drm/ttm: use gem reservation object
dnl # v5.3-rc1-362-gc105de2828e1	drm/amdgpu: use embedded gem object
dnl # v5.3-rc1-358-g8eb8833e7ed3	drm/ttm: add gem base object
dnl # v5.0-rc1-1004-g1ba627148ef5	drm: Add reservation_object to drm_gem_object
dnl #
AC_DEFUN([AC_AMDGPU_TTM_BUFFER_OBJECT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_gem.h>
		], [
			struct drm_gem_object *gem_obj = NULL;
			gem_obj->resv = &gem_obj->_resv;
		], [
			AC_DEFINE(HAVE_DRM_GEM_OBJECT_RESV, 1,
				[ttm_buffer_object->base is available])
		])
	])
])
