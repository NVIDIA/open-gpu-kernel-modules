dnl # commit 2624dd154bcc53ac2de16ecae9746ba867b6ca70
dnl # drm/amdgpu: add timeline support in amdgpu CS v3
AC_DEFUN([AC_AMDGPU_CHUNK_ID_SYNCOBJ_TIMELINE_WAIT_SIGNAL], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/amdgpu_drm.h>
		], [
			#if !defined(AMDGPU_CHUNK_ID_SYNCOBJ_TIMELINE_WAIT) ||\
					!defined(AMDGPU_CHUNK_ID_SYNCOBJ_TIMELINE_SIGNAL)
			#error CHUNK_ID_SYNCOBJ_TIMELINE_WAIT_SIGNAL not #defined
			#endif
		], [
			AC_DEFINE(HAVE_CHUNK_ID_SYNCOBJ_TIMELINE_WAIT_SIGNAL, 1,
				[whether CHUNK_ID_SYNCOBJ_TIMELINE_WAIT_SIGNAL is defined])
		])
	])
])
