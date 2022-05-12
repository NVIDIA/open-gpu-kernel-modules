dnl #
dnl # commit 5d6527a784f7a6d247961e046e830de8d71b47d1
dnl # Author: Jérôme Glisse <jglisse@redhat.com>
dnl # Date:   Fri Dec 28 00:38:05 2018 -0800
dnl # mm/mmu_notifier: use structure for invalidate_range_start/end callback
dnl # Patch series "mmu notifier contextual informations", v2.
dnl #
AC_DEFUN([AC_AMDGPU_INVALIDATE_RANGE_START], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <linux/mmu_notifier.h>
		], [
			struct mmu_notifier_ops *ops = NULL;
			ops->invalidate_range_start(NULL, NULL);
		], [
			AC_DEFINE(HAVE_2ARGS_INVALIDATE_RANGE_START, 1,
				whether invalidate_range_start() wants 2 args)
		], [
			dnl #
			dnl # commit 93065ac753e4443840a057bfef4be71ec766fde9
			dnl # Author: Michal Hocko <mhocko@suse.com>
			dnl # Date:   Tue Aug 21 21:52:33 2018 -0700
			dnl # mm, oom: distinguish blockable mode for mmu notifiers
			dnl #
			AC_KERNEL_TRY_COMPILE([
				#include <linux/mmu_notifier.h>
			], [
				struct mmu_notifier_ops *ops = NULL;
				ops->invalidate_range_start(NULL, NULL, 1, 1, 1);
			], [
				AC_DEFINE(HAVE_5ARGS_INVALIDATE_RANGE_START, 1,
					whether invalidate_range_start() wants 5 args)
			])
		])
	])
])
