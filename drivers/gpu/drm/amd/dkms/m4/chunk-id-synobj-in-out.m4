dnl # commit 660e855813f78b7fe63ff1ebc4f2ca07d94add0b
dnl # amdgpu: use drm sync objects for shared semaphores (v6)
AC_DEFUN([AC_AMDGPU_CHUNK_ID_SYNOBJ_IN_OUT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/amdgpu_drm.h>
		], [
			#if !defined(AMDGPU_CHUNK_ID_SYNCOBJ_IN) ||\
					!defined(AMDGPU_CHUNK_ID_SYNCOBJ_OUT)
			#error CHUNK_ID_SYNOBJ_IN_OUT not #defined
			#endif
		], [
			AC_DEFINE(HAVE_CHUNK_ID_SYNOBJ_IN_OUT, 1,
				[whether CHUNK_ID_SYNOBJ_IN_OUT is defined])
		])
	])
])
