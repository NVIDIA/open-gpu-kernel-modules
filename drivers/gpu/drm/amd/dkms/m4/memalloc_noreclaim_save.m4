dnl #
dnl # commit 4e544bac8267f65a0bf06aed1bde9964da4812ed
dnl # PCI: Add pci_dev_id() helper
dnl #
AC_DEFUN([AC_AMDGPU_MEMALLOC_NORECLAIM_SAVE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/sched/mm.h>
		], [
			memalloc_noreclaim_save();
		], [
			AC_DEFINE(HAVE_MEMALLOC_NORECLAIM_SAVE, 1,
				[memalloc_noreclaim_save() is available])
		])
	])
])
