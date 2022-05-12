AC_DEFUN([AC_AMDGPU_LINUX_HEADERS], [

	dnl #
	AC_KERNEL_CHECK_HEADERS([linux/overflow.h])

	dnl #
	AC_KERNEL_CHECK_HEADERS([linux/sched/mm.h])

	dnl #
	AC_KERNEL_CHECK_HEADERS([linux/sched/task.h])

	dnl #
	AC_KERNEL_CHECK_HEADERS([linux/sched/signal.h])

	dnl #
	dnl #  commit v4.15-28-gf3804203306e
	dnl #  array_index_nospec: Sanitize speculative array de-references
	dnl #
	AC_KERNEL_CHECK_HEADERS([linux/nospec.h])

	dnl #
	dnl # commit 8bd9cb51daac89337295b6f037b0486911e1b408
	dnl # locking/atomics, asm-generic: Move some macros from <linux/bitops.h>
	dnl # to a new <linux/bits.h> file
	dnl #
	AC_KERNEL_CHECK_HEADERS([linux/bits.h])

	dnl #
	dnl # commit v4.3-rc4-1-g2f8e2c877784
	dnl # move io-64-nonatomic*.h out of asm-generic
	dnl #
	AC_KERNEL_CHECK_HEADERS([linux/io-64-nonatomic-lo-hi.h])

	dnl #
	dnl # commit 299878bac326c890699c696ebba26f56fe93fc75
	dnl # treewide: move set_memory_* functions away from cacheflush.h
	dnl #
	AC_KERNEL_CHECK_HEADERS([asm/set_memory.h])

	dnl #
	dnl # commit df6b35f409af0a8ff1ef62f552b8402f3fef8665
	dnl # x86/fpu: Rename i387.h to fpu/api.h
	dnl #
	AC_KERNEL_CHECK_HEADERS([asm/fpu/api.h])

	dnl #
	dnl # commit 607ca46e97a1b6594b29647d98a32d545c24bdff
	dnl # UAPI: (Scripted) Disintegrate include/linux
	dnl #
	AC_KERNEL_CHECK_HEADERS([uapi/linux/sched/types.h])

	dnl #
	dnl # v4.19-rc6-7-ga3f8a30f3f00
	dnl # Compiler Attributes: use feature checks instead of version checks
	dnl #
	AC_KERNEL_CHECK_HEADERS([linux/compiler_attributes.h])

	dnl #
	dnl # v4.9-rc2-299-gf54d1867005c
	dnl # dma-buf: Rename struct fence to dma_fence
	dnl #
	AC_KERNEL_CHECK_HEADERS([linux/dma-fence.h])

	dnl #
	dnl # v5.3-rc1-449-g52791eeec1d9
	dnl $ dma-buf: rename reservation_object to dma_resv
	dnl #
	AC_KERNEL_CHECK_HEADERS([linux/dma-resv.h])

	dnl #
	dnl # v5.7-13149-g9740ca4e95b4
	dnl # mmap locking API: initial implementation as rwsem wrappers
	dnl #
	AC_KERNEL_CHECK_HEADERS([linux/mmap_lock.h])

	dnl #
	dnl # v4.19-rc4-1-g52916982af48
	dnl # PCI/P2PDMA: Support peer-to-peer memory
	dnl #
	AC_KERNEL_CHECK_HEADERS([linux/pci-p2pdma.h])

	dnl #
	dnl # v4.7-11546-g00085f1efa38
	dnl # dma-mapping: use unsigned long for dma_attrs
	dnl #
	AC_KERNEL_CHECK_HEADERS([linux/dma-attrs.h])

	dnl #
	dnl # v4.13-rc1-41-g7744ccdbc16f
	dnl # x86/mm: Add Secure Memory Encryption (SME) support
	dnl #
	AC_KERNEL_CHECK_HEADERS([linux/mem_encrypt.h])

	dnl #
	dnl # 01fd30da0474
	dnl # dma-buf: Add struct dma-buf-map for storing struct dma_buf.vaddr_ptr
	dnl #
	AC_KERNEL_CHECK_HEADERS([linux/dma-buf-map.h])

	dnl #
	dnl # commit fd851a3cdc196bfc1d229b5f22369069af532bf8
	dnl # spin loop primitives for busy waiting
	dnl #
	AC_KERNEL_CHECK_HEADERS([linux/processor.h])

])
