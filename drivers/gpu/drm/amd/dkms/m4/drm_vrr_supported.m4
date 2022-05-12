dnl #
dnl # v4.20-rc3-428-gbb47de736661 drm/amdgpu: Set FreeSync state using drm VRR properties
dnl # v4.20-rc3-427-g520f08df45fb drm/amdgpu: Correct get_crtc_scanoutpos behavior when vpos >= vtotal
dnl # v4.20-rc3-426-gab7a664f7a2d drm: Document variable refresh properties
dnl # v4.20-rc3-425-g1398958cfd8d drm: Add vrr_enabled property to drm CRTC
dnl # v4.20-rc3-424-gba1b0f6c73d4 drm: Add vrr_capable property to the drm connector
dnl #
AC_DEFUN([AC_AMDGPU_DRM_VRR_SUPPORTED], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_connector.h>
			#include <drm/drm_mode_config.h>
			#include <drm/drm_crtc.h>
		], [
			struct drm_connector *connector = NULL;
			struct drm_mode_config *config = NULL;
			struct drm_crtc_state *base = NULL;

			connector->vrr_capable_property = NULL;
			config->prop_vrr_enabled = NULL;
			base->vrr_enabled = 1;
		], [
			AC_DEFINE(HAVE_DRM_VRR_SUPPORTED, 1,
				[Variable refresh rate(vrr) is supported])
		])
	])
])
