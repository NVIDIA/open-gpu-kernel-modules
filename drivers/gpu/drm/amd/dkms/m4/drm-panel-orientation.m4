dnl #
dnl # commit v4.15-rc2-376-g8d70f395e6cb
dnl #drm: Add support for a panel-orientation connector property, v6
dnl #
AC_DEFUN([AC_AMDGPU_DRM_PANEL_ORIENTATION], [
        AC_KERNEL_DO_BACKGROUND([
                AC_KERNEL_TRY_COMPILE([
                        #include <drm/drm_crtc.h>
			#include <drm/drm_connector.h>
                ],[
                        enum drm_panel_orientation panel = 0;
                ],[
                        AC_DEFINE(HAVE_DRM_PANEL_ORIENTATION_ENUM,
                                1,
                                [enum drm_panel_orientation is available])
                ])
        ])
])
