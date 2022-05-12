dnl #
dnl # v5.7-rc5-32-gd9d200bcebc1
dnl # dma-mapping: add generic helpers for mapping sgtable objects
dnl #
AC_DEFUN([AC_AMDGPU_DMA_MAP_SGTABLE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/dma-mapping.h>
		], [
			dma_map_sgtable(NULL, NULL, 0, 0);
		], [
			AC_DEFINE(HAVE_DMA_MAP_SGTABLE, 1,
				[dma_map_sgtable() is enabled])
		]
		dnl #
		dnl # v4.7-11546-g00085f1efa38
		dnl # dma-mapping: use unsigned long for dma_attrs
		dnl # leverage test for linux/dma-attrs.h
		)
	])
])
