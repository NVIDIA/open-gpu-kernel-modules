dnl #
dnl # commit v4.6-rc2-221-g59f7c0fa325e
dnl # drm/edid: Add drm_edid_get_monitor_name()
dnl #
AC_DEFUN([AC_AMDGPU_DRM_EDID_GET_MONITOR_NAME], [
        AC_KERNEL_DO_BACKGROUND([
                AC_KERNEL_TRY_COMPILE_SYMBOL([
                        #include <drm/drm_edid.h>
                ], [
                        drm_edid_get_monitor_name(NULL, NULL, NULL);
                ], [drm_edid_get_monitor_name], [drivers/gpu/drm/drm_edid.c], [
                        AC_DEFINE(HAVE_DRM_EDID_GET_MONITOR_NAME, 1,
                                [drm_edid_get_monitor_name() are available])
                ])
        ])
])

