dnl #
dnl # v4.8-rc1-3-g6f3d87968f9c
dnl # dma-mapping: add dma_{map,unmap}_resource
dnl #
AC_DEFUN([AC_AMDGPU_DMA_MAP_RESOURCE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/dma-mapping.h>
		], [
			dma_map_resource(NULL, 0, 0, 0, 0);
		], [
			AC_DEFINE(HAVE_DMA_MAP_RESOURCE, 1,
				[dma_map_resource() is enabled])
		])
	])
])
