dnl #
dnl # commit v5.10-rc3-1140-gc67e62790f5c
dnl # drm/prime: split array import functions v4
dnl #
AC_DEFUN([AC_AMDGPU_DRM_PRIME_SG_TO_DMA_ADDR_ARRAY], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([drm_prime_sg_to_dma_addr_array], [drivers/gpu/drm/drm_prime.c], [
			AC_DEFINE(HAVE_DRM_PRIME_SG_TO_DMA_ADDR_ARRAY, 1, [drm_prime_sg_to_dma_addr_array() is available])
		])
	])
])
