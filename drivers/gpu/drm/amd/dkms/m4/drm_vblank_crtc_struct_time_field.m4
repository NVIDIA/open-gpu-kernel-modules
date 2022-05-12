dnl #
dnl # commit v4.14-rc3-721-g67680d3c0464
dnl # drm: vblank: use ktime_t instead of timeval
dnl #
AC_DEFUN([AC_AMDGPU_DRM_VBLANK_CRTC_TIME_FIELD], [
        AC_KERNEL_DO_BACKGROUND([
                AC_KERNEL_TRY_COMPILE([
                        #ifdef HAVE_DRM_DRMP_H
                        #include <drm/drmP.h>
                        #else
                        #include <drm/drm_vblank.h>
                        #endif
                        #include <linux/ktime.h>
                ], [
                        struct drm_vblank_crtc *vblank = NULL;
                        vblank->time = ns_to_ktime(0);
                ], [
                        AC_DEFINE(HAVE_DRM_VBLANK_USE_KTIME_T, 1,
                          [drm_vblank->time uses ktime_t type])
                ],[
                        dnl #
                        dnl #  commit v4.6-rc7-1094-gd4055a9b2079
                        dnl # drm: use seqlock for vblank time/count
                        dnl #
                        AC_KERNEL_TRY_COMPILE([
                                #ifdef HAVE_DRM_DRMP_H
                                #include <drm/drmP.h>
                                #else
                                #include <drm/drm_vblank.h>
                                #endif
                        ], [
                                struct drm_vblank_crtc *vblank = NULL;
                                struct timeval tv;
                                vblank->time[0] = tv;
                        ], [
                                AC_DEFINE(HAVE_DRM_VBLANK_CRTC_HAS_ARRAY_TIME_FIELD, 1,
                                  [drm_vblank->time is array])
                        ])
               ])
        ])
])
