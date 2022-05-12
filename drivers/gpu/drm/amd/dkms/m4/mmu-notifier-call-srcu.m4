dnl # commit b972216e27d1c853eced33f8638926636c606341
dnl # mmu_notifier: add call_srcu and sync function
dnl # for listener to delay call and sync
dnl #
dnl # commit v5.3-rc5-63-gc96245148c1e
dnl # mm/mmu_notifiers: remove unregister_no_release
dnl #
AC_DEFUN([AC_AMDGPU_MMU_NOTIFIER_CALL_SRCU], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/mmu_notifier.h>
		],[
			mmu_notifier_call_srcu(NULL, NULL);
		],[
			AC_DEFINE(HAVE_MMU_NOTIFIER_CALL_SRCU, 1, [mmu_notifier_call_srcu() is available])
		])
	])
])
