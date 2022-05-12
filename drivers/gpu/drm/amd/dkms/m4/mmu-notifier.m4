dnl #
dnl # commit 4a83bfe916f3d2100df5bc8389bd182a537ced3e
dnl # mm/mmu_notifier: helper to test if a range invalidation is blockable
dnl #
AC_DEFUN([AC_AMDGPU_MMU_NOTIFIER], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/mmu_notifier.h>
		], [
			mmu_notifier_range_blockable(NULL);
		], [
			AC_DEFINE(HAVE_MMU_NOTIFIER_RANGE_BLOCKABLE, 1,
				[mmu_notifier_range_blockable() is available])
		])
	])
])
