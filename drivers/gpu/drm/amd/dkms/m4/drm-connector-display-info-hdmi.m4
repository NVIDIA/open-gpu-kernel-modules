dnl #
dnl # dfbe9bf0 introduce this change
dnl # drm/amdgpu: replace drm_detect_hdmi_monitor() with drm_display_info.is_hdmi
dnl # v5.13-3121-gdfbe9bf067a2
dnl #
AC_DEFUN([AC_AMDGPU_DRM_DISPLAY_INFO_IS_HDMI], [
        AC_KERNEL_DO_BACKGROUND([
                AC_KERNEL_TRY_COMPILE([
                        #include <drm/drm_connector.h>
                ], [
                        struct drm_display_info *display_info = NULL;
                        display_info->is_hdmi = 0;
                ], [
                        AC_DEFINE(HAVE_DRM_DISPLAY_INFO_IS_HDMI, 1,
                                [display_info->is_hdmi is available])
                ])
        ])
])
