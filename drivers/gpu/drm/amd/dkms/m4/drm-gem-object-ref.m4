dnl #
dnl # v5.7-rc1-518-gab15d56e27be drm: remove transient drm_gem_object_put_unlocked()
dnl # v5.7-rc1-491-geecd7fd8bf58 drm/gem: add _locked suffix to drm_gem_object_put
dnl # v5.7-rc1-490-gb5d250744ccc drm/gem: fold drm_gem_object_put_unlocked and __drm_gem_object_put()
dnl #
AC_DEFUN([AC_AMDGPU_DRM_GEM_OBJECT_PUT_UNLOCKED], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT(
		[drm_gem_object_put_locked], [drivers/gpu/drm/drm_gem.c],
		[
			AC_DEFINE(HAVE_DRM_GEM_OBJECT_PUT_LOCKED, 1,
					[drm_gem_object_put_locked() is available])
		], [
			dnl #
			dnl # commit v4.10-rc8-1302-ge6b62714e87c
			dnl # drm: Introduce drm_gem_object_{get,put}()
			dnl #
			AC_KERNEL_TRY_COMPILE_SYMBOL([
				#include <drm/drm_gem.h>
			], [
				drm_gem_object_put_unlocked(NULL);
			], [drm_gem_object_put_unlocked], [drivers/gpu/drm/drm_gem.c], [
				AC_DEFINE(HAVE_DRM_GEM_OBJECT_PUT_UNLOCKED, 1,
					[drm_gem_object_put_unlocked() is available])
			])
		])
	])
])
