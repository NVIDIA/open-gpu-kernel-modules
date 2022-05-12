dnl #
dnl # commit v4.17-rc3-491-ga19741e5e5a9
dnl # dma_buf: remove device parameter from attach callback v2
dnl #
AC_DEFUN([AC_AMDGPU_DRM_GEM_MAP_ATTACH], [
	AC_KERNEL_TRY_COMPILE_SYMBOL([
		#include <drm/drm_prime.h>
	], [
		drm_gem_map_attach(NULL, NULL);
	], [drm_gem_map_attach], [drivers/gpu/drm/drm_prime.c], [
		AC_DEFINE(HAVE_DRM_GEM_MAP_ATTACH_2ARGS, 1,
			[drm_gem_map_attach() wants 2 arguments])
	])
])
