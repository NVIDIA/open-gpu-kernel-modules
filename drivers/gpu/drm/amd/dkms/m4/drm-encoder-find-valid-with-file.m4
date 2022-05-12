dnl #
dnl # commit 418da17214aca5ef5f0b6f7588905ee7df92f98f
dnl # drm: Pass struct drm_file * to __drm_mode_object_find
dnl #
AC_DEFUN([AC_AMDGPU_DRM_ENCODER_FIND_VALID_WITH_FILE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_encoder.h>
		], [
			struct drm_device *dev = NULL;
			struct drm_file *file_priv = NULL;
			uint32_t id = 0;
			struct drm_encoder *encoder = NULL;

			encoder = drm_encoder_find(dev, file_priv, id);
		],[
			AC_DEFINE(HAVE_DRM_ENCODER_FIND_VALID_WITH_FILE, 1,
				[drm_encoder_find() wants file_priv])
		])
	])
])
