AC_DEFUN([AC_AMDGPU_MMGRAB], [
	AC_KERNEL_DO_BACKGROUND([
		dnl #
		dnl # rhel 7.x wrapp mmgrab() in drm_backport.h
		dnl # v4.10-11141-g68e21be2916b sched/headers: Move task->mm handling methods to <linux/sched/mm.h>
		dnl # v4.10-10392-gf1f1007644ff mm: add new mmgrab() helper
		dnl #
		AC_KERNEL_TRY_COMPILE([
			#ifdef HAVE_DRM_DRM_BACKPORT_H
			#include <drm/drm_backport.h>
			#endif
			#ifdef HAVE_LINUX_SCHED_MM_H
			#include <linux/sched/mm.h>
			#endif
			#include <linux/sched.h>
		], [
			mmgrab(NULL);
		], [
			AC_DEFINE(HAVE_MMGRAB, 1, [mmgrab() is available])
		])
	])
])

AC_DEFUN([AC_AMDGPU_MMGET], [
        AC_KERNEL_DO_BACKGROUND([
                dnl #
                dnl # commit v4.10-10393-g3fce371bfac2
                dnl # mm: add new mmget() helper
                dnl #
                AC_KERNEL_TRY_COMPILE([
                        #ifdef HAVE_DRM_DRM_BACKPORT_H
                        #include <drm/drm_backport.h>
                        #endif
                        #ifdef HAVE_LINUX_SCHED_MM_H
                        #include <linux/sched/mm.h>
                        #endif
                        #include <linux/sched.h>
                ], [
                        mmget(NULL);
                ], [
                        AC_DEFINE(HAVE_MMGET, 1,
                        [mmget is available])
                ])
        ])
])
