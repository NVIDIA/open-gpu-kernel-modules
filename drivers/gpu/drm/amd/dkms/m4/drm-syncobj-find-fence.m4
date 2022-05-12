dnl #
dnl # commit 649fdce23cdf516e69aa8c18f4b44c62127f0e83
dnl # drm: add flags to drm_syncobj_find_fence
dnl #
AC_DEFUN([AC_AMDGPU_DRM_SYNCOBJ_FIND_FENCE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([drm_syncobj_find_fence], [drivers/gpu/drm/drm_syncobj.c], [
			AC_DEFINE(HAVE_DRM_SYNCOBJ_FIND_FENCE, 1,
				[drm_syncobj_find_fence() is available])
			AC_KERNEL_TRY_COMPILE([
				struct drm_file;
				#include <drm/drm_syncobj.h>
			], [
				drm_syncobj_find_fence(NULL, 0, 0, 0, NULL);
			], [
				AC_DEFINE(HAVE_DRM_SYNCOBJ_FIND_FENCE_5ARGS, 1,
					[whether drm_syncobj_find_fence() wants 5 args])
			], [
				dnl #
				dnl # commit 0a6730ea27b68c7ac4171c29a816c29d26a9637a
				dnl # drm: expand drm_syncobj_find_fence to support timeline point v2
				dnl #
				AC_KERNEL_TRY_COMPILE([
					struct drm_file;
					#include <drm/drm_syncobj.h>
				], [
					drm_syncobj_find_fence(NULL, 0, 0, NULL);
				], [
					AC_DEFINE(HAVE_DRM_SYNCOBJ_FIND_FENCE_4ARGS, 1,
						[whether drm_syncobj_find_fence() wants 4 args])
				], [
					dnl #
					dnl # commit afaf59237843bf89823c33143beca6b262dff0ca
					dnl # drm/syncobj: Rename fence_get to find_fence
					dnl #
					AC_KERNEL_TRY_COMPILE([
						struct drm_file;
						#include <drm/drm_syncobj.h>
					], [
						drm_syncobj_find_fence(NULL, 0, NULL);
					], [
						AC_DEFINE(HAVE_DRM_SYNCOBJ_FIND_FENCE_3ARGS, 1,
							[whether drm_syncobj_find_fence() wants 3 args])
					])
				])
			])
		], [
			dnl #
			dnl # commit e9083420bbacce27e43d418064d0d2dfb4b37aaa
			dnl # drm: introduce sync objects (v4)
			dnl #
			AC_KERNEL_TRY_COMPILE_SYMBOL([
				struct drm_file;
				#include <drm/drm_syncobj.h>
			], [
				drm_syncobj_fence_get(NULL, 0, NULL);
			], [drm_syncobj_fence_get], [drivers/gpu/drm/drm_syncobj.c], [
				AC_DEFINE(HAVE_DRM_SYNCOBJ_FENCE_GET, 1,
					[drm_syncobj_fence_get() is available])
			])
		])
	])
])
