dnl #
dnl # commit 18dddadc78c91a91b546acc48506c24f5f840c4f
dnl # drm/atomic: Introduce drm_atomic_helper_shutdown
dnl #
AC_DEFUN([AC_AMDGPU_DRM_ATOMIC_HELPER_SHUTDOWN], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE_SYMBOL([
			#include <drm/drm_atomic_helper.h>
		],[
			drm_atomic_helper_shutdown(NULL);
		],[drm_atomic_helper_shutdown],[drivers/gpu/drm/drm_atomic_helper.c],[
			AC_DEFINE(HAVE_DRM_ATOMIC_HELPER_SHUTDOWN, 1,
				[drm_atomic_helper_shutdown() is available])
		])
	])
])
