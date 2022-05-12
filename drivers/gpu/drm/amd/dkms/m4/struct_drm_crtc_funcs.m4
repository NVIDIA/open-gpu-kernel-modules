dnl #
dnl # v5.5-rc2-1557-ge3eff4b5d91e drm/amdgpu: Convert to CRTC VBLANK callbacks
dnl # v5.5-rc2-1556-gea702333e567 drm/amdgpu: Convert to struct drm_crtc_helper_funcs.get_scanout_position()
dnl # v5.5-rc2-1555-g7fe3f0d15aac drm: Add get_vblank_timestamp() to struct drm_crtc_funcs
dnl # v5.5-rc2-1554-gf1e2b6371c12 drm: Add get_scanout_position() to struct drm_crtc_helper_funcs
dnl #
AC_DEFUN([AC_AMDGPU_STRUCT_DRM_CRTC_FUNCS_GET_VBLANK_TIMESTAMP], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_crtc.h>
		],[
			struct drm_crtc_funcs *ptr = NULL;
			ptr->get_vblank_timestamp(NULL, NULL, NULL, 0);
		],[
			AC_DEFINE(HAVE_STRUCT_DRM_CRTC_FUNCS_GET_VBLANK_TIMESTAMP,
				1,
				[struct drm_crtc_funcs->get_vblank_timestamp() is available])
		],[
			AC_AMDGPU_DRM_CALC_VBLTIMESTAMP_FROM_SCANOUTPOS
		])
	])
])

dnl #
dnl # v4.11-rc3-950-ga4eff9aa6db8
dnl # drm: Add acquire ctx parameter to ->set_config
dnl #
AC_DEFUN([AC_AMDGPU_STRUCT_DRM_CRTC_FUNCS_SET_CONFIG], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_crtc.h>
		], [
			struct drm_crtc_funcs *funcs = NULL;
			funcs->set_config(NULL, NULL);
		], [
			AC_DEFINE(HAVE_STRUCT_DRM_CRTC_FUNCS_SET_CONFIG_CTX, 1,
				[drm_crtc_funcs->set_config() wants ctx parameter])
		])
	])
])

dnl #
dnl # v4.11-rc3-945-g41292b1fa13a
dnl # drm: Add acquire ctx parameter to ->page_flip(_target)
dnl #
AC_DEFUN([AC_AMDGPU_STRUCT_DRM_CRTC_FUNCS_PAGE_FLIP_TARGET], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_crtc.h>
		], [
			struct drm_crtc_funcs *funcs = NULL;
			funcs->page_flip_target(NULL, NULL, NULL, 0, 0, NULL);
		], [
			AC_DEFINE(HAVE_STRUCT_DRM_CRTC_FUNCS_PAGE_FLIP_TARGET_CTX, 1,
				[drm_crtc_funcs->page_flip_target() wants ctx parameter])
			AC_DEFINE(HAVE_STRUCT_DRM_CRTC_FUNCS_PAGE_FLIP_TARGET, 1,
				[drm_crtc_funcs->page_flip_target() is available])
		], [
			dnl #
			dnl # v4.8-rc1-112-gc229bfbbd04a
			dnl # drm: Add page_flip_target CRTC hook v2
			dnl #
			AC_KERNEL_TRY_COMPILE([
				#include <drm/drm_crtc.h>
			], [
				struct drm_crtc_funcs *funcs = NULL;
				funcs->page_flip_target(NULL, NULL, NULL, 0, 0);
			], [
				AC_DEFINE(HAVE_STRUCT_DRM_CRTC_FUNCS_PAGE_FLIP_TARGET, 1,
					[drm_crtc_funcs->page_flip_target() is available])
			])
		])
	])
])

dnl #
dnl # commit v4.10-rc5-1070-g84e354839b15
dnl # drm: add vblank hooks to struct drm_crtc_funcs
dnl #
AC_DEFUN([AC_AMDGPU_STRUCT_DRM_CRTC_FUNCS_ENABLE_VBLANK], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_crtc.h>
		], [
			struct drm_crtc_funcs *crtc_funcs = NULL;
			crtc_funcs->enable_vblank(NULL);
		], [
			AC_DEFINE(HAVE_STRUCT_DRM_CRTC_FUNCS_ENABLE_VBLANK, 1, [
				drm_crtc_funcs->enable_vblank() is available])
		])
	])
])

dnl #
dnl # v5.2-rc5-2034-g8fb843d179a6 drm/amd/display: add functionality to get pipe CRC source.
dnl # v4.18-rc3-759-g3b3b8448ebd1 drm/amdgpu_dm/crc: Implement verify_crc_source callback
dnl # v4.18-rc3-757-g4396551e9cf3 drm: crc: Introduce get_crc_sources callback
dnl # v4.18-rc3-756-gd5cc15a0c66e drm: crc: Introduce verify_crc_source callback
dnl #
AC_DEFUN([AC_AMDGPU_STRUCT_DRM_CRTC_FUNCS_GET_VERIFY_CRC_SOURCES], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_crtc.h>
		], [
			struct drm_crtc_funcs *crtc_funcs = NULL;
			crtc_funcs->get_crc_sources(NULL, NULL);
			crtc_funcs->verify_crc_source(NULL, NULL, NULL);
		], [
			AC_DEFINE(HAVE_STRUCT_DRM_CRTC_FUNCS_GET_VERIFY_CRC_SOURCES, 1, [
				drm_crtc_funcs->{get,verify}_crc_sources() is available])
		])
	])
])

dnl #
dnl # v4.18-rc3-781-gc0811a7d5bef drm/crc: Cleanup crtc_crc_open function
dnl # v4.16-rc1-363-g31aec354f92c drm/amd/display: Implement interface for CRC on CRTC
dnl # v4.8-rc8-1429-g9edbf1fa600a drm: Add API for capturing frame CRCs
dnl #
AC_DEFUN([AC_AMDGPU_STRUCT_DRM_CRTC_FUNCS_SET_CRC_SOURCE], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_crtc.h>
		], [
			struct drm_crtc *crtc = NULL;
			int ret;

			ret = crtc->funcs->set_crc_source(NULL, NULL);
		], [
			AC_DEFINE(HAVE_STRUCT_DRM_CRTC_FUNCS_SET_CRC_SOURCE, 1,
				[crtc->funcs->set_crc_source() is available])
			AC_DEFINE(HAVE_STRUCT_DRM_CRTC_FUNCS_SET_CRC_SOURCE_2ARGS, 1,
				[crtc->funcs->set_crc_source() wants 2 args])
		], [
			AC_KERNEL_TRY_COMPILE([
				#include <drm/drm_crtc.h>
			], [
				struct drm_crtc *crtc = NULL;
				int ret;

				ret = crtc->funcs->set_crc_source(NULL, NULL, NULL);
			], [
				AC_DEFINE(HAVE_STRUCT_DRM_CRTC_FUNCS_SET_CRC_SOURCE, 1,
					[crtc->funcs->set_crc_source() is available])
			])
		])
	])
])

dnl #
dnl # v4.11-rc5-1392-g6d124ff84533 drm: Add acquire ctx to ->gamma_set hook
dnl # 		int (*gamma_set)(struct drm_crtc *crtc, u16 *r, u16 *g, u16 *b,
dnl # 	-                        uint32_t size);
dnl # 	+                        uint32_t size,
dnl # 	+                        struct drm_modeset_acquire_ctx *ctx);
dnl # v4.7-rc1-260-g7ea772838782 drm/core: Change declaration for gamma_set.
dnl # 	-       void (*gamma_set)(struct drm_crtc *crtc, u16 *r, u16 *g, u16 *b,
dnl # 	-                         uint32_t start, uint32_t size);
dnl # 	+       int (*gamma_set)(struct drm_crtc *crtc, u16 *r, u16 *g, u16 *b,
dnl # 	+                        uint32_t size);
dnl # v2.6.35-260-g7203425a943e drm: expand gamma_set
dnl # 		void (*gamma_set)(struct drm_crtc *crtc, u16 *r, u16 *g, u16 *b,
dnl # 	-                         uint32_t size);
dnl # 	+                         uint32_t start, uint32_t size);
dnl # v2.6.28-8-gf453ba046074 DRM: add mode setting support
dnl # 	+       void (*gamma_set)(struct drm_crtc *crtc, u16 *r, u16 *g, u16 *b,
dnl # 	+                         uint32_t size);
dnl #
AC_DEFUN([AC_AMDGPU_STRUCT_DRM_CRTC_FUNCS_GAMMA_SET], [
	AC_KERNEL_DO_BACKGROUND([
		AC_KERNEL_TRY_COMPILE([
			#include <drm/drm_crtc.h>
		], [
			struct drm_crtc *crtc = NULL;
			int ret;

			ret = crtc->funcs->gamma_set(NULL, NULL, NULL, NULL, 0, NULL);
		], [
			AC_DEFINE(HAVE_STRUCT_DRM_CRTC_FUNCS_GAMMA_SET_6ARGS, 1,
				[crtc->funcs->gamma_set() wants 6 args])
		], [
			AC_KERNEL_TRY_COMPILE([
				#include <drm/drm_crtc.h>
			], [
				struct drm_crtc *crtc = NULL;
				int ret;

				ret = crtc->funcs->gamma_set(NULL, NULL, NULL, NULL, 0);
			], [
				AC_DEFINE(HAVE_STRUCT_DRM_CRTC_FUNCS_GAMMA_SET_5ARGS, 1,
					[crtc->funcs->gamma_set() wants 5 args])
			])
		])
	])
])

dnl #
dnl # v5.10-1961-g6ca2ab8086af drm: automatic legacy gamma support
dnl #
AC_DEFUN([AC_AMDGPU_STRUCT_DRM_CRTC_FUNCS_GAMMA_SET_OPTIONAL], [
	AC_KERNEL_CHECK_SYMBOL_EXPORT(
		[drm_atomic_helper_legacy_gamma_set], [drivers/gpu/drm/drm_atomic_helper.c], [],
		[
			AC_DEFINE(HAVE_STRUCT_DRM_CRTC_FUNCS_GAMMA_SET_OPTIONAL, 1,
				[HAVE_STRUCT_DRM_CRTC_FUNCS_GAMMA_SET_OPTIONAL is available])
		])
])

AC_DEFUN([AC_AMDGPU_STRUCT_DRM_CRTC_FUNCS], [
	AC_AMDGPU_STRUCT_DRM_CRTC_FUNCS_GET_VBLANK_TIMESTAMP
	AC_AMDGPU_STRUCT_DRM_CRTC_FUNCS_ENABLE_VBLANK
	AC_AMDGPU_STRUCT_DRM_CRTC_FUNCS_GET_VERIFY_CRC_SOURCES
	AC_AMDGPU_STRUCT_DRM_CRTC_FUNCS_SET_CRC_SOURCE
	AC_AMDGPU_STRUCT_DRM_CRTC_FUNCS_SET_CONFIG
	AC_AMDGPU_STRUCT_DRM_CRTC_FUNCS_PAGE_FLIP_TARGET
	AC_AMDGPU_STRUCT_DRM_CRTC_FUNCS_GAMMA_SET
	AC_AMDGPU_STRUCT_DRM_CRTC_FUNCS_GAMMA_SET_OPTIONAL
])
