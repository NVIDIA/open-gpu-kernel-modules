dnl #
dnl # commit v5.6-rc5-1191-gf2b816d78a94
dnl # drm: drm/core: Allow drivers allocate a subclass 
dnl # of struct drm_framebuffer
dnl #
AC_DEFUN([AC_AMDGPU_DRM_GEN_FB_INIT_WITH_FUNCS], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([drm_gem_fb_init_with_funcs],
			[drivers/gpu/drm/drm_gem_framebuffer_helper.c], [
			AC_DEFINE(HAVE_DRM_GEN_FB_INIT_WITH_FUNCS, 1,
				[drm_gen_fb_init_with_funcs() is available])
		])
	])
])
