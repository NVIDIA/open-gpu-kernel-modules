dnl #
dnl # f5678e7f2ac3 kernel: better document the use_mm/unuse_mm API contract
dnl # 9bf5b9eb232b kernel: move use_mm/unuse_mm to kthread.c
dnl #
AC_DEFUN([AC_AMDGPU_KTHREAD_USE_MM], [
	AC_KERNEL_DO_BACKGROUND([
		dnl #
		dnl # sle sp2 server distro inlines kthread_use_mm/kthread_unuse_mm
		dnl # in mmu_context.h
		dnl #
		AC_KERNEL_TRY_COMPILE([
			#include <linux/mm_types.h>
			#include <linux/kthread.h>
			#include <linux/mmu_context.h>
		], [
			kthread_use_mm(NULL);
			kthread_unuse_mm(NULL);
		], [
			AC_DEFINE(HAVE_KTHREAD_USE_MM, 1,
				[kthread_{use,unuse}_mm() is available])
		])
	])
])
