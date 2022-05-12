dnl #
dnl # v5.6-rc5-1663-g09606b5446c2
dnl # dma-buf: add peer2peer flag
dnl #
AC_DEFUN([AC_AMDGPU_DMA_BUF], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/dma-buf.h>
		],[
			struct dma_buf_attach_ops *ptr = NULL;
			ptr->allow_peer2peer = false;
		],[
			AC_DEFINE(HAVE_STRUCT_DMA_BUF_ATTACH_OPS_ALLOW_PEER2PEER,
				1,
				[struct dma_buf_attach_ops->allow_peer2peer is available])

			AC_DEFINE(HAVE_STRUCT_DMA_BUF_OPS_PIN,
					1,
					[struct dma_buf_ops->pin() is available])
		],[
			dnl #
			dnl # 4981cdb063e3 dma-buf: make move_notify mandatory if importer_ops are provided
			dnl # bd2275eeed5b dma-buf: drop dynamic_mapping flag
			dnl # a448cb003edc drm/amdgpu: implement amdgpu_gem_prime_move_notify v2
			dnl # 2d4dad2734e2 drm/amdgpu: add amdgpu_dma_buf_pin/unpin v2
			dnl # 4993ba02635f drm/amdgpu: use allowed_domains for exported DMA-bufs
			dnl # d2588d2ded0f drm/ttm: remove the backing store if no placement is given
			dnl # bb42df4662a4 dma-buf: add dynamic DMA-buf handling v15
			dnl #
			AC_KERNEL_TRY_COMPILE([
				#include <linux/dma-buf.h>
			],[
				struct dma_buf_ops *ptr = NULL;
				ptr->pin(NULL);
			],[
				AC_DEFINE(HAVE_STRUCT_DMA_BUF_OPS_PIN,
					1,
					[struct dma_buf_ops->pin() is available])
			], [
				dnl #
				dnl # commit v5.4-rc4-863-g15fd552d186c
				dnl # dma-buf: change DMA-buf locking convention v3
				dnl #
				AC_KERNEL_TRY_COMPILE([
					#include <linux/dma-buf.h>
				], [
					struct dma_buf_ops *dma_buf_ops = NULL;
					dma_buf_ops->dynamic_mapping = true;
				],[
					AC_DEFINE(HAVE_DMA_BUF_OPS_DYNAMIC_MAPPING, 1,
						[dma_buf->dynamic_mapping is available])
				],[
					AC_DEFINE(HAVE_DMA_BUF_OPS_LEGACY, 1,
						[dma_buf->dynamic_mapping is not available])

					AC_AMDGPU_DRM_GEM_MAP_ATTACH
				])
			])
		])
	])
])
