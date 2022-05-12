dnl #
dnl # commit 707d561f77b5e2a6f90c9786bee44ee7a8dedc7e
dnl # drm: allow limiting the scatter list size.
dnl #
AC_DEFUN([AC_AMDGPU_DRM_PRIME_PAGES_TO_SG], [
	AC_KERNEL_TRY_COMPILE_SYMBOL([
		#include <drm/drm_prime.h>
	], [
		drm_prime_pages_to_sg(NULL, NULL, 0);
	], [drm_prime_pages_to_sg], [drivers/gpu/drm/drm_prime.c], [
		AC_DEFINE(HAVE_DRM_PRIME_PAGES_TO_SG_3ARGS, 1,
			[drm_prime_pages_to_sg() wants 3 arguments])
	])
])
