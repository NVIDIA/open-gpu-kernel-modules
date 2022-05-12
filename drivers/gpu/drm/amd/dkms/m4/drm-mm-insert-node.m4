dnl #
dnl # commit 4e64e5539d152e202ad6eea2b6f65f3ab58d9428
dnl # Author: Chris Wilson <chris@chris-wilson.co.uk>
dnl # Date:   Thu Feb 2 21:04:38 2017 +0000
dnl # drm: Improve drm_mm search (and fix topdown allocation) with rbtrees
dnl #
AC_DEFUN([AC_AMDGPU_DRM_MM_INSERT_NODE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_mm.h>
		], [
			drm_mm_insert_node(NULL, NULL, 0);
		], [
			AC_DEFINE(HAVE_DRM_MM_INSERT_NODE_THREE_PARAMETERS, 1,
				[drm_mm_insert_node has three parameters])
		])
	])
])
