dnl #
dnl # v4.14-rc3-1071-g570e86963a51
dnl # drm: Widen vblank count to 64-bits [v3]
dnl #
AC_DEFUN([AC_AMDGPU_STRUCT_DRM_PENDING_VBLANK_EVENT_SEQUENCE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_vblank.h>
		],[
			struct drm_pending_vblank_event *event = NULL;
			event->sequence = 0;
		],[
			AC_DEFINE(HAVE_STRUCT_DRM_PENDING_VBLANK_EVENT_SEQUENCE, 1,
				[drm_pending_vblank_event->sequence is available])
		])
	])
])
