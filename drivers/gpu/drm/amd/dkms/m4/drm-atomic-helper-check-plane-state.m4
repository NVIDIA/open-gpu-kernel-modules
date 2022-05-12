dnl #
dnl # commit v4.16-rc1-401-g81af63a4af82
dnl # drm: Don't pass clip to drm_atomic_helper_check_plane_state()
dnl #
dnl # commit v4.14-rc3-641-ga01cb8ba3f62
dnl # drm: Move drm_plane_helper_check_state() into drm_atomic_helper.c
dnl #
dnl # commit v4.14-rc3-640-g10b47ee02d1a
dnl # drm: Check crtc_state->enable rather than crtc->enabled in drm_plane_helper_check_state()
dnl #
AC_DEFUN([AC_AMDGPU_DRM_ATOMIC_HELPER_CHECK_PLANE_STATE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_CHECK_SYMBOL_EXPORT(
			[drm_atomic_helper_check_plane_state], [drivers/gpu/drm/drm_atomic_helper.c], [
				AC_DEFINE(HAVE_DRM_ATOMIC_HELPER_CHECK_PLANE_STATE, 1,
					[drm_atomic_helper_check_plane_state() is available])
			], [
				dnl #
				dnl # v4.8-rc1-13-gdf86af9133b4
				dnl # drm/plane-helper: Add drm_plane_helper_check_state()
				dnl #
				AC_KERNEL_CHECK_SYMBOL_EXPORT(
					[drm_plane_helper_check_state],
					[drivers/gpu/drm/drm_plane_helper.c],[
						AC_DEFINE(HAVE_DRM_PLANE_HELPER_CHECK_STATE, 1,
							[drm_plane_helper_check_state is available])
			])
		])
	])
])
