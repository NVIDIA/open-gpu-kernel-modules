dnl #
dnl # v5.6-rc2-359-g63170ac6f2e8
dnl # drm/simple-kms: Add drm_simple_encoder_{init,create}()
dnl #
AC_DEFUN([AC_AMDGPU_DRM_SIMPLE_ENCODER_INIT], [
        AC_KERNEL_CHECK_SYMBOL_EXPORT(
                [drm_simple_encoder_init],
                [drivers/gpu/drm/drm_simple_kms_helper.c],[
                AC_DEFINE(HAVE_DRM_SIMPLE_ENCODER_INIT, 1,
                        [drm_simple_encoder is  available])
        ])
])
