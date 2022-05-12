dnl #
dnl # commit v4.10-rc8-1300-gad09360750af
dnl # drm: Introduce drm_connector_{get,put}()
dnl #
AC_DEFUN([AC_AMDGPU_DRM_CONNECTOR_PUT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_crtc.h>
		],[
			drm_connector_put(NULL);
		],[
			AC_DEFINE(HAVE_DRM_CONNECTOR_PUT, 1,
				[drm_connector_put() is available])
			AC_DEFINE(HAVE_DRM_CONNECTOR_REFERENCE_COUNTING_SUPPORTED, 1,
				[connector reference counting is available])
		],[
			dnl #
			dnl # v4.8-rc2-342-g522171951761 drm: Extract drm_connector.[hc]
			dnl # v4.6-rc3-676-gb164d31f50b2 drm/modes: add connector reference counting. (v2)
			dnl #
			AC_KERNEL_TRY_COMPILE([
				#include <drm/drm_crtc.h>
			],[
				drm_connector_unreference(NULL);
			],[
				AC_DEFINE(HAVE_DRM_CONNECTOR_UNREFERENCE,1,
					[drm_connector_unreference() is available])
				AC_DEFINE(HAVE_DRM_CONNECTOR_REFERENCE_COUNTING_SUPPORTED, 1,
					[connector reference counting is available])
			], [
				dnl #
				dnl # commit v4.6-rc3-351-gd0f37cf62979
				dnl # drm/mode: move framebuffer reference into object
				dnl #
				AC_KERNEL_TRY_COMPILE([
					#include <drm/drm_crtc.h>
				],[
					struct drm_mode_object *obj = NULL;
					obj->free_cb = NULL;
				],[
					AC_DEFINE(HAVE_FREE_CB_IN_STRUCT_DRM_MODE_OBJECT,1,
						[drm_mode_object->free_cb is available])
					AC_DEFINE(HAVE_DRM_CONNECTOR_REFERENCE_COUNTING_SUPPORTED, 1,
						[connector reference counting is available])
				])
			])
		])
	])
])
