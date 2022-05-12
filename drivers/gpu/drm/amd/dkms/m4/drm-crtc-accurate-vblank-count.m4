dnl #
dnl # Function drm_accurate_vblank_count() was first added in drm_irq.c in:
dnl #   commit af61d5ce1532191213dce2404f9c45d32260a6cd
dnl #   drm/core: Add drm_accurate_vblank_count, v5.
dnl # Then, it was moved to drm_vblank.[hc] in:
dnl #   commit 3ed4351a83ca05d3cd886ade6900be1067aa7903
dnl #   drm: Extract drm_vblank.[hc]
dnl # Finally, it was renamed to drm_crtc_accurate_vblank_count() in:
dnl #   commit ca814b25538a5b2c0a8de6665191725f41608f2c
dnl #   drm/vblank: Consistent drm_crtc_ prefix
dnl #

AC_DEFUN([AC_AMDGPU_DRM_CRTC_ACCURATE_VBLANK_COUNT], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT([drm_crtc_accurate_vblank_count],
		[drivers/gpu/drm/drm_vblank.c], [
			AC_DEFINE(HAVE_DRM_CRTC_ACCURATE_VBLANK_COUNT, 1,
					[drm_crtc_accurate_vblank_count() is available])
		], [
			AC_KERNEL_CHECK_SYMBOL_EXPORT([drm_accurate_vblank_count],
			[drivers/gpu/drm/drm_vblank.c drivers/gpu/drm/drm_irq.c], [
				AC_DEFINE(HAVE_DRM_ACCURATE_VBLANK_COUNT, 1,
					[drm_accurate_vblank_count() is available])
			])
		])
	])
])
