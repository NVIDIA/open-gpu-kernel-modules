dnl #
dnl # commit bee330f3d67273a68dcb99f59480d59553c008b2
dnl # drm: Use srcu to protect drm_device.unplugged
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DEV_ENTER], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
                        #ifdef HAVE_DRM_DRMP_H
                        #include <drm/drmP.h>
                        #else
                        #include <drm/drm_drv.h>
                        #endif
		], [
			drm_dev_enter(NULL, NULL);
		], [drm_dev_enter], [drivers/gpu/drm/drm_drv.c], [
			AC_DEFINE(HAVE_DRM_DEV_ENTER, 1, [drm_dev_enter() is available])
		])
	])
])
