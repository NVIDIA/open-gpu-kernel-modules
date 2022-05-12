dnl #
dnl # v5.7-rc4-4-g2733ea144dcc mm/hmm: remove the customizable pfn format from hmm_range_fault
dnl # v5.7-rc4-3-g5c8f3c4cf18a mm/hmm: remove HMM_PFN_SPECIAL
dnl # v5.7-rc4-2-g4e2490843d55 drm/amdgpu: remove dead code after hmm_range_fault()
dnl # v5.7-rc4-1-gbe957c886d92 mm/hmm: make hmm_range_fault return 0 or -1
dnl #
AC_DEFUN([AC_AMDGPU_HMM_RANGE_FAULT], [
	AC_KERNEL_TRY_COMPILE([
		#include <linux/hmm.h>
	], [
		enum hmm_pfn_flags flag;
		flag = HMM_PFN_REQ_FAULT;
	], [
		AC_DEFINE(HAVE_HMM_DROP_CUSTOMIZABLE_PFN_FORMAT, 1,
			[hmm remove the customizable pfn format])
		AC_DEFINE(HAVE_HMM_RANGE_FAULT_1ARG, 1,
				[hmm_range_fault() wants 1 arg])
	], [
		dnl #
		dnl # v5.6-rc3-21-g6bfef2f91945
		dnl # mm/hmm: remove HMM_FAULT_SNAPSHOT
		dnl #
		AC_KERNEL_TRY_COMPILE([
			#include <linux/hmm.h>
		], [
			hmm_range_fault(NULL);;
		], [
			AC_DEFINE(HAVE_HMM_RANGE_FAULT_1ARG, 1,
				[hmm_range_fault() wants 1 arg])
		])
	])
])

dnl #
dnl # v5.1-10231-gbf198b2b34bf: mm/mmu_notifier: pass down vma and reasons why mmu notifier is happening
dnl # 93f4e735b6d9 - mm/hmm: remove hmm_range_dma_map and hmm_range_dma_unmap <Christoph Hellwig> 2019-11-23 19:56:45 -0400
dnl # d28c2c9a4877 - mm/hmm: make full use of walk_page_range() <Ralph Campbell> 2019-11-23 19:56:45 -0400
dnl # d3eeb1d77c5d - xen/gntdev: use mmu_interval_notifier_insert <Jason Gunthorpe> 2019-11-23 19:56:45 -0400
dnl # a22dd506400d - mm/hmm: remove hmm_mirror and related <Jason Gunthorpe> 2019-11-23 19:56:45 -0400
dnl # 81fa1af31b5d - drm/amdgpu: Use mmu_interval_notifier instead of hmm_mirror <Jason Gunthorpe> 2019-11-23 19:56:45 -0400
dnl # 62914a99dee5 - drm/amdgpu: Use mmu_interval_insert instead of hmm_mirror <Jason Gunthorpe> 2019-11-23 19:56:45 -0400
dnl # a9ae8731e6e5 - drm/amdgpu: Call find_vma under mmap_sem <Jason Gunthorpe> 2019-11-23 19:56:44 -0400
dnl # 20fef4ef84bf - nouveau: use mmu_interval_notifier instead of hmm_mirror <Jason Gunthorpe> 2019-11-23 19:56:44 -0400
dnl # c625c274ee00 - nouveau: use mmu_notifier directly for invalidate_range_start <Jason Gunthorpe> 2019-11-23 19:56:44 -0400
dnl # 3506ff69c3ec - drm/radeon: use mmu_interval_notifier_insert <Jason Gunthorpe> 2019-11-23 19:56:44 -0400
dnl # 3889551db212 - RDMA/hfi1: Use mmu_interval_notifier_insert for user_exp_rcv <Jason Gunthorpe> 2019-11-23 19:56:44 -0400
dnl # f25a546e6529 - RDMA/odp: Use mmu_interval_notifier_insert() <Jason Gunthorpe> 2019-11-23 19:56:44 -0400
dnl # 107e899874e9 - mm/hmm: define the pre-processor related parts of hmm.h even if disabled <Jason Gunthorpe> 2019-11-23 19:56:44 -0400
dnl # v5.4-rc5-20-g04ec32fbc2b2 - mm/hmm: allow hmm_range to be used with a mmu_interval_notifier or hmm_mirror <Jason Gunthorpe> 2019-11-23 19:56:44 -0400
dnl # 99cb252f5e68 - mm/mmu_notifier: add an interval tree notifier <Jason Gunthorpe> 2019-11-23 19:56:44 -0400
dnl # 56f434f40f05 - mm/mmu_notifier: define the header pre-processor parts even if disabled <Jason Gunthorpe> 2019-11-12 20:18:27 -0400
dnl #
AC_DEFUN([AC_AMDGPU_HMM], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/hmm.h>
			#include <linux/mmu_notifier.h>
		], [
			#ifdef CONFIG_HMM_MIRROR
			struct hmm_range *range = NULL;
			struct mmu_notifier_range *mmu_range = NULL;

			range->notifier = NULL;
			mmu_range->vma = NULL;
			#else
			#error CONFIG_HMM_MIRROR not enabled
			#endif
		], [
			AC_DEFINE(HAVE_AMDKCL_HMM_MIRROR_ENABLED, 1,
				[hmm support is enabled])
			AC_AMDGPU_HMM_RANGE_FAULT
			AC_AMDGPU_HSA_AMD_SVM
		])
	])
])
