dnl #
dnl # commit 7ca24cf2d2269bde25e21c02a77fe81995a081ae
dnl # drm/amdgpu: add FENCE_TO_HANDLE ioctl that returns syncobj or sync_file
dnl #
AC_DEFUN([AC_AMDGPU_DRM_FENCE_TO_HANDLE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/amdgpu_drm.h>
		], [
			#if !defined(DRM_AMDGPU_FENCE_TO_HANDLE)
			#error DRM_AMDGPU_FENCE_TO_HANDLE not #defined
			#endif
		], [
			AC_DEFINE(HAVE_DRM_AMDGPU_FENCE_TO_HANDLE, 1,
				[DRM_AMDGPU_FENCE_TO_HANDLE is defined])
		])
	])
])
