/*
 * Copyright (c) 2015-2022, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "nvidia-drm-conftest.h" /* NV_DRM_ATOMIC_MODESET_AVAILABLE */

#if defined(NV_DRM_ATOMIC_MODESET_AVAILABLE)

#include "nvidia-drm-helper.h"
#include "nvidia-drm-priv.h"
#include "nvidia-drm-crtc.h"
#include "nvidia-drm-connector.h"
#include "nvidia-drm-encoder.h"
#include "nvidia-drm-utils.h"
#include "nvidia-drm-fb.h"
#include "nvidia-drm-ioctl.h"
#include "nvidia-drm-format.h"

#include "nvmisc.h"

#include <drm/drm_crtc_helper.h>
#include <drm/drm_plane_helper.h>

#include <drm/drm_atomic.h>
#include <drm/drm_atomic_helper.h>

#if defined(NV_DRM_DRM_COLOR_MGMT_H_PRESENT)
#include <drm/drm_color_mgmt.h>
#endif


#if defined(NV_DRM_HAS_HDR_OUTPUT_METADATA)
static int
nv_drm_atomic_replace_property_blob_from_id(struct drm_device *dev,
                                            struct drm_property_blob **blob,
                                            uint64_t blob_id,
                                            ssize_t expected_size)
{
    struct drm_property_blob *new_blob = NULL;

    if (blob_id != 0) {
        new_blob = drm_property_lookup_blob(dev, blob_id);
        if (new_blob == NULL) {
            return -EINVAL;
        }

        if ((expected_size > 0) &&
            (new_blob->length != expected_size)) {
            drm_property_blob_put(new_blob);
            return -EINVAL;
        }
    }

    drm_property_replace_blob(blob, new_blob);
    drm_property_blob_put(new_blob);

    return 0;
}
#endif

static void nv_drm_plane_destroy(struct drm_plane *plane)
{
    struct nv_drm_plane *nv_plane = to_nv_plane(plane);

    /* plane->state gets freed here */
    drm_plane_cleanup(plane);

    nv_drm_free(nv_plane);
}

static inline void
plane_config_clear(struct NvKmsKapiLayerConfig *layerConfig)
{
    if (layerConfig == NULL) {
        return;
    }

    memset(layerConfig, 0, sizeof(*layerConfig));
    layerConfig->csc = NVKMS_IDENTITY_CSC_MATRIX;
}

static inline void
plane_req_config_disable(struct NvKmsKapiLayerRequestedConfig *req_config)
{
    /* Clear layer config */
    plane_config_clear(&req_config->config);

    /* Set flags to get cleared layer config applied */
    req_config->flags.surfaceChanged = NV_TRUE;
    req_config->flags.srcXYChanged = NV_TRUE;
    req_config->flags.srcWHChanged = NV_TRUE;
    req_config->flags.dstXYChanged = NV_TRUE;
    req_config->flags.dstWHChanged = NV_TRUE;
}

static inline void
cursor_req_config_disable(struct NvKmsKapiCursorRequestedConfig *req_config)
{
    req_config->surface = NULL;
    req_config->flags.surfaceChanged = NV_TRUE;
}

#if defined(NV_DRM_COLOR_MGMT_AVAILABLE)
static void color_mgmt_config_ctm_to_csc(struct NvKmsCscMatrix *nvkms_csc,
                                         struct drm_color_ctm  *drm_ctm)
{
    int y;

    /* CTM is a 3x3 matrix while ours is 3x4. Zero out the last column. */
    nvkms_csc->m[0][3] = nvkms_csc->m[1][3] = nvkms_csc->m[2][3] = 0;

    for (y = 0; y < 3; y++) {
        int x;

        for (x = 0; x < 3; x++) {
            /*
             * Values in the CTM are encoded in S31.32 sign-magnitude fixed-
             * point format, while NvKms CSC values are signed 2's-complement
             * S15.16 (Ssign-extend12-3.16?) fixed-point format.
             */
            NvU64 ctmVal = drm_ctm->matrix[y*3 + x];
            NvU64 signBit = ctmVal & (1ULL << 63);
            NvU64 magnitude = ctmVal & ~signBit;

            /*
             * Drop the low 16 bits of the fractional part and the high 17 bits
             * of the integral part. Drop 17 bits to avoid corner cases where
             * the highest resulting bit is a 1, causing the `cscVal = -cscVal`
             * line to result in a positive number.
             */
            NvS32 cscVal = (magnitude >> 16) & ((1ULL << 31) - 1);
            if (signBit) {
                cscVal = -cscVal;
            }

            nvkms_csc->m[y][x] = cscVal;
        }
    }
}
#endif /* NV_DRM_COLOR_MGMT_AVAILABLE */

static void
cursor_plane_req_config_update(struct drm_plane *plane,
                               struct drm_plane_state *plane_state,
                               struct NvKmsKapiCursorRequestedConfig *req_config)
{
    struct nv_drm_plane *nv_plane = to_nv_plane(plane);
    struct NvKmsKapiCursorRequestedConfig old_config = *req_config;

    if (plane_state->fb == NULL) {
        cursor_req_config_disable(req_config);
        return;
    }

    memset(req_config, 0, sizeof(*req_config));
    req_config->surface = to_nv_framebuffer(plane_state->fb)->pSurface;
    req_config->dstX = plane_state->crtc_x;
    req_config->dstY = plane_state->crtc_y;

#if defined(NV_DRM_ALPHA_BLENDING_AVAILABLE)
    if (plane->blend_mode_property != NULL && plane->alpha_property != NULL) {

        switch (plane_state->pixel_blend_mode) {
            case DRM_MODE_BLEND_PREMULTI:
                req_config->compParams.compMode =
                    NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_SURFACE_ALPHA;
                break;
            case DRM_MODE_BLEND_COVERAGE:
                req_config->compParams.compMode =
                    NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_SURFACE_ALPHA;
                break;
            default:
                /*
                 * We should not hit this, because
                 * plane_state->pixel_blend_mode should only have values
                 * registered in
                 * __nv_drm_plane_create_alpha_blending_properties().
                 */
                WARN_ON("Unsupported blending mode");
                break;

        }

        req_config->compParams.surfaceAlpha =
            plane_state->alpha >> 8;

    } else if (plane->blend_mode_property != NULL) {

        switch (plane_state->pixel_blend_mode) {
            case DRM_MODE_BLEND_PREMULTI:
                req_config->compParams.compMode =
                    NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_ALPHA;
                break;
            case DRM_MODE_BLEND_COVERAGE:
                req_config->compParams.compMode =
                    NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_ALPHA;
                break;
            default:
                /*
                 * We should not hit this, because
                 * plane_state->pixel_blend_mode should only have values
                 * registered in
                 * __nv_drm_plane_create_alpha_blending_properties().
                 */
                WARN_ON("Unsupported blending mode");
                break;

        }

    } else {
        req_config->compParams.compMode =
            nv_plane->defaultCompositionMode;
    }
#else
    req_config->compParams.compMode = nv_plane->defaultCompositionMode;
#endif

    /*
     * Unconditionally mark the surface as changed, even if nothing changed,
     * so that we always get a flip event: a DRM client may flip with
     * the same surface and wait for a flip event.
     */
    req_config->flags.surfaceChanged = NV_TRUE;

    if (old_config.surface == NULL &&
        old_config.surface != req_config->surface) {
        req_config->flags.dstXYChanged = NV_TRUE;
        return;
    }

    req_config->flags.dstXYChanged =
        old_config.dstX != req_config->dstX ||
        old_config.dstY != req_config->dstY;
}

static int
plane_req_config_update(struct drm_plane *plane,
                        struct drm_plane_state *plane_state,
                        struct NvKmsKapiLayerRequestedConfig *req_config)
{
    struct nv_drm_plane *nv_plane = to_nv_plane(plane);
    struct NvKmsKapiLayerConfig old_config = req_config->config;
    struct nv_drm_plane_state *nv_drm_plane_state =
        to_nv_drm_plane_state(plane_state);

    if (plane_state->fb == NULL) {
        plane_req_config_disable(req_config);
        return 0;
    }

    memset(req_config, 0, sizeof(*req_config));

    req_config->config.surface = to_nv_framebuffer(plane_state->fb)->pSurface;

    /* Source values are 16.16 fixed point */
    req_config->config.srcX = plane_state->src_x >> 16;
    req_config->config.srcY = plane_state->src_y >> 16;
    req_config->config.srcWidth  = plane_state->src_w >> 16;
    req_config->config.srcHeight = plane_state->src_h >> 16;

    req_config->config.dstX = plane_state->crtc_x;
    req_config->config.dstY = plane_state->crtc_y;
    req_config->config.dstWidth  = plane_state->crtc_w;
    req_config->config.dstHeight = plane_state->crtc_h;

    req_config->config.csc = old_config.csc;

#if defined(NV_DRM_ROTATION_AVAILABLE)
    /*
     * plane_state->rotation is only valid when plane->rotation_property
     * is non-NULL.
     */
    if (plane->rotation_property != NULL) {
        if (plane_state->rotation & DRM_MODE_REFLECT_X) {
            req_config->config.rrParams.reflectionX = true;
        }

        if (plane_state->rotation & DRM_MODE_REFLECT_Y) {
            req_config->config.rrParams.reflectionY = true;
        }

        switch (plane_state->rotation & DRM_MODE_ROTATE_MASK) {
            case DRM_MODE_ROTATE_0:
                req_config->config.rrParams.rotation = NVKMS_ROTATION_0;
                break;
            case DRM_MODE_ROTATE_90:
                req_config->config.rrParams.rotation = NVKMS_ROTATION_90;
                break;
            case DRM_MODE_ROTATE_180:
                req_config->config.rrParams.rotation = NVKMS_ROTATION_180;
                break;
            case DRM_MODE_ROTATE_270:
                req_config->config.rrParams.rotation = NVKMS_ROTATION_270;
                break;
            default:
                /*
                 * We should not hit this, because
                 * plane_state->rotation should only have values
                 * registered in
                 * __nv_drm_plane_create_rotation_property().
                 */
                WARN_ON("Unsupported rotation");
                break;
        }
    }
#endif

#if defined(NV_DRM_ALPHA_BLENDING_AVAILABLE)
    if (plane->blend_mode_property != NULL && plane->alpha_property != NULL) {

        switch (plane_state->pixel_blend_mode) {
            case DRM_MODE_BLEND_PREMULTI:
                req_config->config.compParams.compMode =
                    NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_SURFACE_ALPHA;
                break;
            case DRM_MODE_BLEND_COVERAGE:
                req_config->config.compParams.compMode =
                    NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_SURFACE_ALPHA;
                break;
            default:
                /*
                 * We should not hit this, because
                 * plane_state->pixel_blend_mode should only have values
                 * registered in
                 * __nv_drm_plane_create_alpha_blending_properties().
                 */
                WARN_ON("Unsupported blending mode");
                break;

        }

        req_config->config.compParams.surfaceAlpha =
            plane_state->alpha >> 8;

    } else if (plane->blend_mode_property != NULL) {

        switch (plane_state->pixel_blend_mode) {
            case DRM_MODE_BLEND_PREMULTI:
                req_config->config.compParams.compMode =
                    NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_ALPHA;
                break;
            case DRM_MODE_BLEND_COVERAGE:
                req_config->config.compParams.compMode =
                    NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_ALPHA;
                break;
            default:
                /*
                 * We should not hit this, because
                 * plane_state->pixel_blend_mode should only have values
                 * registered in
                 * __nv_drm_plane_create_alpha_blending_properties().
                 */
                WARN_ON("Unsupported blending mode");
                break;

        }

    } else {
        req_config->config.compParams.compMode =
            nv_plane->defaultCompositionMode;
    }
#else
    req_config->config.compParams.compMode =
        nv_plane->defaultCompositionMode;
#endif

    req_config->config.inputColorSpace =
        nv_drm_plane_state->input_colorspace;

    req_config->config.syncParams.preSyncptSpecified = false;
    req_config->config.syncParams.postSyncptRequested = false;
    req_config->config.syncParams.semaphoreSpecified = false;

    if (nv_drm_plane_state->fd_user_ptr) {
        if (to_nv_device(plane->dev)->supportsSyncpts) {
            req_config->config.syncParams.postSyncptRequested = true;
        } else {
            return -1;
        }
    }

#if defined(NV_DRM_HAS_HDR_OUTPUT_METADATA)
    if (nv_drm_plane_state->hdr_output_metadata != NULL) {
        struct hdr_output_metadata *hdr_metadata =
            nv_drm_plane_state->hdr_output_metadata->data;
        struct hdr_metadata_infoframe *info_frame =
            &hdr_metadata->hdmi_metadata_type1;
        struct nv_drm_device *nv_dev = to_nv_device(plane->dev);
        uint32_t i;

        if (hdr_metadata->metadata_type != HDMI_STATIC_METADATA_TYPE1) {
            NV_DRM_DEV_LOG_ERR(nv_dev, "Unsupported Metadata Type");
            return -1;
        }

        for (i = 0; i < ARRAY_SIZE(info_frame->display_primaries); i ++) {
            req_config->config.hdrMetadata.val.displayPrimaries[i].x =
                info_frame->display_primaries[i].x;
            req_config->config.hdrMetadata.val.displayPrimaries[i].y =
                info_frame->display_primaries[i].y;
        }

        req_config->config.hdrMetadata.val.whitePoint.x =
            info_frame->white_point.x;
        req_config->config.hdrMetadata.val.whitePoint.y =
            info_frame->white_point.y;
        req_config->config.hdrMetadata.val.maxDisplayMasteringLuminance =
            info_frame->max_display_mastering_luminance;
        req_config->config.hdrMetadata.val.minDisplayMasteringLuminance =
            info_frame->min_display_mastering_luminance;
        req_config->config.hdrMetadata.val.maxCLL =
            info_frame->max_cll;
        req_config->config.hdrMetadata.val.maxFALL =
            info_frame->max_fall;

        switch (info_frame->eotf) {
            case HDMI_EOTF_SMPTE_ST2084:
                req_config->config.tf = NVKMS_OUTPUT_TF_PQ;
                break;
            case HDMI_EOTF_TRADITIONAL_GAMMA_SDR:
                req_config->config.tf =
                    NVKMS_OUTPUT_TF_TRADITIONAL_GAMMA_SDR;
                break;
            default:
                NV_DRM_DEV_LOG_ERR(nv_dev, "Unsupported EOTF");
                return -1;
        }

        req_config->config.hdrMetadata.enabled = true;
    } else {
        req_config->config.hdrMetadata.enabled = false;
        req_config->config.tf = NVKMS_OUTPUT_TF_NONE;
    }

    req_config->flags.hdrMetadataChanged =
        ((old_config.hdrMetadata.enabled !=
          req_config->config.hdrMetadata.enabled) ||
         memcmp(&old_config.hdrMetadata.val,
                &req_config->config.hdrMetadata.val,
                sizeof(struct NvKmsHDRStaticMetadata)));

    req_config->flags.tfChanged = (old_config.tf != req_config->config.tf);
#endif

    /*
     * Unconditionally mark the surface as changed, even if nothing changed,
     * so that we always get a flip event: a DRM client may flip with
     * the same surface and wait for a flip event.
     */
    req_config->flags.surfaceChanged = NV_TRUE;

    if (old_config.surface == NULL &&
        old_config.surface != req_config->config.surface) {
        req_config->flags.srcXYChanged = NV_TRUE;
        req_config->flags.srcWHChanged = NV_TRUE;
        req_config->flags.dstXYChanged = NV_TRUE;
        req_config->flags.dstWHChanged = NV_TRUE;
        return 0;
    }

    req_config->flags.srcXYChanged =
        old_config.srcX != req_config->config.srcX ||
        old_config.srcY != req_config->config.srcY;

    req_config->flags.srcWHChanged =
        old_config.srcWidth != req_config->config.srcWidth ||
        old_config.srcHeight != req_config->config.srcHeight;

    req_config->flags.dstXYChanged =
        old_config.dstX != req_config->config.dstX ||
        old_config.dstY != req_config->config.dstY;

    req_config->flags.dstWHChanged =
        old_config.dstWidth != req_config->config.dstWidth ||
        old_config.dstHeight != req_config->config.dstHeight;

    return 0;
}

static bool __is_async_flip_requested(const struct drm_plane *plane,
                                      const struct drm_crtc_state *crtc_state)
{
    if (plane->type == DRM_PLANE_TYPE_PRIMARY) {
#if defined(NV_DRM_CRTC_STATE_HAS_ASYNC_FLIP)
        return crtc_state->async_flip;
#elif defined(NV_DRM_CRTC_STATE_HAS_PAGEFLIP_FLAGS)
        return !!(crtc_state->pageflip_flags & DRM_MODE_PAGE_FLIP_ASYNC);
#endif
    }

    return false;
}

static int __nv_drm_cursor_atomic_check(struct drm_plane *plane,
                                        struct drm_plane_state *plane_state)
{
    struct nv_drm_plane *nv_plane = to_nv_plane(plane);
    int i;
    struct drm_crtc *crtc;
    struct drm_crtc_state *crtc_state;

    WARN_ON(nv_plane->layer_idx != NVKMS_KAPI_LAYER_INVALID_IDX);

    nv_drm_for_each_crtc_in_state(plane_state->state, crtc, crtc_state, i) {
        struct nv_drm_crtc_state *nv_crtc_state = to_nv_crtc_state(crtc_state);
        struct NvKmsKapiHeadRequestedConfig *head_req_config =
            &nv_crtc_state->req_config;
        struct NvKmsKapiCursorRequestedConfig *cursor_req_config =
            &head_req_config->cursorRequestedConfig;

        if (plane->state->crtc == crtc &&
            plane->state->crtc != plane_state->crtc) {
            cursor_req_config_disable(cursor_req_config);
            continue;
        }

        if (plane_state->crtc == crtc) {
            cursor_plane_req_config_update(plane, plane_state,
                                           cursor_req_config);
        }
    }

    return 0;
}

#if defined(NV_DRM_PLANE_ATOMIC_CHECK_HAS_ATOMIC_STATE_ARG)
static int nv_drm_plane_atomic_check(struct drm_plane *plane,
                                     struct drm_atomic_state *state)
#else
static int nv_drm_plane_atomic_check(struct drm_plane *plane,
                                     struct drm_plane_state *plane_state)
#endif
{
    struct nv_drm_plane *nv_plane = to_nv_plane(plane);
#if defined(NV_DRM_PLANE_ATOMIC_CHECK_HAS_ATOMIC_STATE_ARG)
    struct drm_plane_state *plane_state =
        drm_atomic_get_new_plane_state(state, plane);
#endif
    int i;
    struct drm_crtc *crtc;
    struct drm_crtc_state *crtc_state;
    int ret;

    if (plane->type == DRM_PLANE_TYPE_CURSOR) {
        return __nv_drm_cursor_atomic_check(plane, plane_state);
    }

    WARN_ON(nv_plane->layer_idx == NVKMS_KAPI_LAYER_INVALID_IDX);

    nv_drm_for_each_crtc_in_state(plane_state->state, crtc, crtc_state, i) {
        struct nv_drm_crtc_state *nv_crtc_state = to_nv_crtc_state(crtc_state);
        struct NvKmsKapiHeadRequestedConfig *head_req_config =
            &nv_crtc_state->req_config;
        struct NvKmsKapiLayerRequestedConfig *plane_requested_config =
            &head_req_config->layerRequestedConfig[nv_plane->layer_idx];

        if (plane->state->crtc == crtc &&
            plane->state->crtc != plane_state->crtc) {
            plane_req_config_disable(plane_requested_config);
            continue;
        }

        if (plane_state->crtc == crtc) {
            ret = plane_req_config_update(plane,
                                          plane_state,
                                          plane_requested_config);
            if (ret != 0) {
                return ret;
            }

#if defined(NV_DRM_COLOR_MGMT_AVAILABLE)
            if (crtc_state->color_mgmt_changed) {
                /*
                 * According to the comment in the Linux kernel's
                 * drivers/gpu/drm/drm_color_mgmt.c, if this property is NULL,
                 * the CTM needs to be changed to the identity matrix
                 */
                if (crtc_state->ctm) {
                    color_mgmt_config_ctm_to_csc(&plane_requested_config->config.csc,
                                                 (struct drm_color_ctm *)crtc_state->ctm->data);
                } else {
                    plane_requested_config->config.csc = NVKMS_IDENTITY_CSC_MATRIX;
                }
                plane_requested_config->config.cscUseMain = NV_FALSE;
                plane_requested_config->flags.cscChanged = NV_TRUE;
            }
#endif /* NV_DRM_COLOR_MGMT_AVAILABLE */

            if (__is_async_flip_requested(plane, crtc_state)) {
                /*
                 * Async flip requests that the flip happen 'as soon as
                 * possible', meaning that it not delay waiting for vblank.
                 * This may cause tearing on the screen.
                 */
                plane_requested_config->config.minPresentInterval = 0;
                plane_requested_config->config.tearing = NV_TRUE;
            } else {
                plane_requested_config->config.minPresentInterval = 1;
                plane_requested_config->config.tearing = NV_FALSE;
            }
        }
    }

    return 0;
}

#if defined(NV_DRM_UNIVERSAL_PLANE_INIT_HAS_FORMAT_MODIFIERS_ARG)
static bool nv_drm_plane_format_mod_supported(struct drm_plane *plane,
                                              uint32_t format,
                                              uint64_t modifier)
{
    /* All supported modifiers are compatible with all supported formats */
    return true;
}
#endif


static int nv_drm_plane_atomic_set_property(
    struct drm_plane *plane,
    struct drm_plane_state *state,
    struct drm_property *property,
    uint64_t val)
{
    struct nv_drm_device *nv_dev = to_nv_device(plane->dev);
    struct nv_drm_plane_state *nv_drm_plane_state =
        to_nv_drm_plane_state(state);

    if (property == nv_dev->nv_out_fence_property) {
        nv_drm_plane_state->fd_user_ptr = (void __user *)(uintptr_t)(val);
        return 0;
    } else if (property == nv_dev->nv_input_colorspace_property) {
        nv_drm_plane_state->input_colorspace = val;
        return 0;
    }
#if defined(NV_DRM_HAS_HDR_OUTPUT_METADATA)
    else if (property == nv_dev->nv_hdr_output_metadata_property) {
        return nv_drm_atomic_replace_property_blob_from_id(
                nv_dev->dev,
                &nv_drm_plane_state->hdr_output_metadata,
                val,
                sizeof(struct hdr_output_metadata));
    }
#endif

    return -EINVAL;
}

static int nv_drm_plane_atomic_get_property(
    struct drm_plane *plane,
    const struct drm_plane_state *state,
    struct drm_property *property,
    uint64_t *val)
{
    struct nv_drm_device *nv_dev = to_nv_device(plane->dev);
    const struct nv_drm_plane_state *nv_drm_plane_state =
        to_nv_drm_plane_state_const(state);

    if (property == nv_dev->nv_out_fence_property) {
        return 0;
    } else if (property == nv_dev->nv_input_colorspace_property) {
        *val = nv_drm_plane_state->input_colorspace;
        return 0;
    }
#if defined(NV_DRM_HAS_HDR_OUTPUT_METADATA)
    else if (property ==  nv_dev->nv_hdr_output_metadata_property) {
        const struct nv_drm_plane_state *nv_drm_plane_state =
            to_nv_drm_plane_state_const(state);
        *val = nv_drm_plane_state->hdr_output_metadata ?
            nv_drm_plane_state->hdr_output_metadata->base.id : 0;
        return 0;
    }
#endif

    return -EINVAL;
}

/**
 * nv_drm_plane_atomic_reset - plane state reset hook
 * @plane: DRM plane
 *
 * Allocate an empty DRM plane state.
 */
static void nv_drm_plane_atomic_reset(struct drm_plane *plane)
{
    struct nv_drm_plane_state *nv_plane_state =
        nv_drm_calloc(1, sizeof(*nv_plane_state));

    if (!nv_plane_state) {
        return;
    }

    drm_atomic_helper_plane_reset(plane);

    /*
     * The drm atomic helper function allocates a state object that is the wrong
     * size. Copy its contents into the one we allocated above and replace the
     * pointer.
     */
    if (plane->state) {
        nv_plane_state->base = *plane->state;
        kfree(plane->state);
        plane->state = &nv_plane_state->base;
    } else {
        kfree(nv_plane_state);
    }
}


static struct drm_plane_state *
nv_drm_plane_atomic_duplicate_state(struct drm_plane *plane)
{
    struct nv_drm_plane_state *nv_old_plane_state =
        to_nv_drm_plane_state(plane->state);
    struct nv_drm_plane_state *nv_plane_state =
        nv_drm_calloc(1, sizeof(*nv_plane_state));

    if (nv_plane_state == NULL) {
        return NULL;
    }

    __drm_atomic_helper_plane_duplicate_state(plane, &nv_plane_state->base);

    nv_plane_state->fd_user_ptr = nv_old_plane_state->fd_user_ptr;
    nv_plane_state->input_colorspace = nv_old_plane_state->input_colorspace;

#if defined(NV_DRM_HAS_HDR_OUTPUT_METADATA)
    nv_plane_state->hdr_output_metadata = nv_old_plane_state->hdr_output_metadata;
    if (nv_plane_state->hdr_output_metadata) {
        drm_property_blob_get(nv_plane_state->hdr_output_metadata);
    }
#endif

    return &nv_plane_state->base;
}

static inline void __nv_drm_plane_atomic_destroy_state(
    struct drm_plane *plane,
    struct drm_plane_state *state)
{
#if defined(NV_DRM_ATOMIC_HELPER_PLANE_DESTROY_STATE_HAS_PLANE_ARG)
    __drm_atomic_helper_plane_destroy_state(plane, state);
#else
    __drm_atomic_helper_plane_destroy_state(state);
#endif

#if defined(NV_DRM_HAS_HDR_OUTPUT_METADATA)
    {
        struct nv_drm_plane_state *nv_drm_plane_state =
            to_nv_drm_plane_state(state);
        drm_property_blob_put(nv_drm_plane_state->hdr_output_metadata);
    }
#endif
}

static void nv_drm_plane_atomic_destroy_state(
    struct drm_plane *plane,
    struct drm_plane_state *state)
{
    __nv_drm_plane_atomic_destroy_state(plane, state);

    nv_drm_free(to_nv_drm_plane_state(state));
}

static const struct drm_plane_funcs nv_plane_funcs = {
    .update_plane           = drm_atomic_helper_update_plane,
    .disable_plane          = drm_atomic_helper_disable_plane,
    .destroy                = nv_drm_plane_destroy,
    .reset                  = nv_drm_plane_atomic_reset,
    .atomic_get_property    = nv_drm_plane_atomic_get_property,
    .atomic_set_property    = nv_drm_plane_atomic_set_property,
    .atomic_duplicate_state = nv_drm_plane_atomic_duplicate_state,
    .atomic_destroy_state   = nv_drm_plane_atomic_destroy_state,
#if defined(NV_DRM_UNIVERSAL_PLANE_INIT_HAS_FORMAT_MODIFIERS_ARG)
    .format_mod_supported   = nv_drm_plane_format_mod_supported,
#endif
};

static const struct drm_plane_helper_funcs nv_plane_helper_funcs = {
    .atomic_check   = nv_drm_plane_atomic_check,
};

static void nv_drm_crtc_destroy(struct drm_crtc *crtc)
{
    struct nv_drm_crtc *nv_crtc = to_nv_crtc(crtc);

    drm_crtc_cleanup(crtc);

    nv_drm_free(nv_crtc);
}

static inline void
__nv_drm_atomic_helper_crtc_destroy_state(struct drm_crtc *crtc,
                                          struct drm_crtc_state *crtc_state)
{
#if defined(NV_DRM_ATOMIC_HELPER_CRTC_DESTROY_STATE_HAS_CRTC_ARG)
    __drm_atomic_helper_crtc_destroy_state(crtc, crtc_state);
#else
    __drm_atomic_helper_crtc_destroy_state(crtc_state);
#endif
}

static inline bool nv_drm_crtc_duplicate_req_head_modeset_config(
    const struct NvKmsKapiHeadRequestedConfig *old,
    struct NvKmsKapiHeadRequestedConfig *new)
{
    uint32_t i;

    /*
     * Do not duplicate fields like 'modeChanged' flags expressing delta changed
     * in new configuration with respect to previous/old configuration because
     * there is no change in new configuration yet with respect
     * to older one!
     */
    memset(new, 0, sizeof(*new));
    new->modeSetConfig = old->modeSetConfig;

    for (i = 0; i < ARRAY_SIZE(old->layerRequestedConfig); i++) {
        new->layerRequestedConfig[i].config =
            old->layerRequestedConfig[i].config;
    }

    if (old->modeSetConfig.lut.input.pRamps) {
        new->modeSetConfig.lut.input.pRamps =
            nv_drm_calloc(1, sizeof(*new->modeSetConfig.lut.input.pRamps));

        if (!new->modeSetConfig.lut.input.pRamps) {
            return false;
        }
        *new->modeSetConfig.lut.input.pRamps =
            *old->modeSetConfig.lut.input.pRamps;
    }
    if (old->modeSetConfig.lut.output.pRamps) {
        new->modeSetConfig.lut.output.pRamps =
            nv_drm_calloc(1, sizeof(*new->modeSetConfig.lut.output.pRamps));

        if (!new->modeSetConfig.lut.output.pRamps) {
            /*
             * new->modeSetConfig.lut.input.pRamps is either NULL or it was
             * just allocated
             */
            nv_drm_free(new->modeSetConfig.lut.input.pRamps);
            new->modeSetConfig.lut.input.pRamps = NULL;
            return false;
        }
        *new->modeSetConfig.lut.output.pRamps =
            *old->modeSetConfig.lut.output.pRamps;
    }
    return true;
}

static inline struct nv_drm_crtc_state *nv_drm_crtc_state_alloc(void)
{
    struct nv_drm_crtc_state *nv_state = nv_drm_calloc(1, sizeof(*nv_state));
    int i;

    if (nv_state == NULL) {
        return NULL;
    }

    for (i = 0; i < ARRAY_SIZE(nv_state->req_config.layerRequestedConfig); i++) {
        plane_config_clear(&nv_state->req_config.layerRequestedConfig[i].config);
    }
    return nv_state;
}


/**
 * nv_drm_atomic_crtc_reset - crtc state reset hook
 * @crtc: DRM crtc
 *
 * Allocate an empty DRM crtc state.
 */
static void nv_drm_atomic_crtc_reset(struct drm_crtc *crtc)
{
    struct nv_drm_crtc_state *nv_state = nv_drm_crtc_state_alloc();

    if (!nv_state) {
        return;
    }

    drm_atomic_helper_crtc_reset(crtc);

    /*
     * The drm atomic helper function allocates a state object that is the wrong
     * size. Copy its contents into the one we allocated above and replace the
     * pointer.
     */
    if (crtc->state) {
        nv_state->base = *crtc->state;
        kfree(crtc->state);
        crtc->state = &nv_state->base;
    } else {
        kfree(nv_state);
    }
}

/**
 * nv_drm_atomic_crtc_duplicate_state - crtc state duplicate hook
 * @crtc: DRM crtc
 *
 * Allocate and accosiate flip state with DRM crtc state, this flip state will
 * be getting consumed at the time of atomic update commit to hardware by
 * nv_drm_atomic_helper_commit_tail().
 */
static struct drm_crtc_state*
nv_drm_atomic_crtc_duplicate_state(struct drm_crtc *crtc)
{
    struct nv_drm_crtc_state *nv_state = nv_drm_crtc_state_alloc();

    if (nv_state == NULL) {
        return NULL;
    }

    if ((nv_state->nv_flip =
            nv_drm_calloc(1, sizeof(*(nv_state->nv_flip)))) == NULL) {
        nv_drm_free(nv_state);
        return NULL;
    }

    INIT_LIST_HEAD(&nv_state->nv_flip->list_entry);
    INIT_LIST_HEAD(&nv_state->nv_flip->deferred_flip_list);

    /*
     * nv_drm_crtc_duplicate_req_head_modeset_config potentially allocates
     * nv_state->req_config.modeSetConfig.lut.{in,out}put.pRamps, so they should
     * be freed in any following failure paths.
     */
    if (!nv_drm_crtc_duplicate_req_head_modeset_config(
             &(to_nv_crtc_state(crtc->state)->req_config),
             &nv_state->req_config)) {

        nv_drm_free(nv_state->nv_flip);
        nv_drm_free(nv_state);
        return NULL;
    }

    __drm_atomic_helper_crtc_duplicate_state(crtc, &nv_state->base);

    return &nv_state->base;
}

/**
 * nv_drm_atomic_crtc_destroy_state - crtc state destroy hook
 * @crtc: DRM crtc
 * @state: DRM crtc state object to destroy
 *
 * Destroy flip state associated with the given crtc state if it haven't get
 * consumed because failure of atomic commit.
 */
static void nv_drm_atomic_crtc_destroy_state(struct drm_crtc *crtc,
                                             struct drm_crtc_state *state)
{
    struct nv_drm_crtc_state *nv_state = to_nv_crtc_state(state);

    if (nv_state->nv_flip != NULL) {
        nv_drm_free(nv_state->nv_flip);
        nv_state->nv_flip = NULL;
    }

    __nv_drm_atomic_helper_crtc_destroy_state(crtc, &nv_state->base);

    nv_drm_free(nv_state->req_config.modeSetConfig.lut.input.pRamps);
    nv_drm_free(nv_state->req_config.modeSetConfig.lut.output.pRamps);

    nv_drm_free(nv_state);
}

static struct drm_crtc_funcs nv_crtc_funcs = {
    .set_config             = drm_atomic_helper_set_config,
    .page_flip              = drm_atomic_helper_page_flip,
    .reset                  = nv_drm_atomic_crtc_reset,
    .destroy                = nv_drm_crtc_destroy,
    .atomic_duplicate_state = nv_drm_atomic_crtc_duplicate_state,
    .atomic_destroy_state   = nv_drm_atomic_crtc_destroy_state,
#if defined(NV_DRM_ATOMIC_HELPER_LEGACY_GAMMA_SET_PRESENT)
    .gamma_set = drm_atomic_helper_legacy_gamma_set,
#endif
};

/*
 * In kernel versions before the addition of
 * drm_crtc_state::connectors_changed, connector changes were
 * reflected in drm_crtc_state::mode_changed.
 */
static inline bool
nv_drm_crtc_state_connectors_changed(struct drm_crtc_state *crtc_state)
{
#if defined(NV_DRM_CRTC_STATE_HAS_CONNECTORS_CHANGED)
    return crtc_state->connectors_changed;
#else
    return crtc_state->mode_changed;
#endif
}

static int head_modeset_config_attach_connector(
    struct nv_drm_connector *nv_connector,
    struct NvKmsKapiHeadModeSetConfig *head_modeset_config)
{
    struct nv_drm_encoder *nv_encoder = nv_connector->nv_detected_encoder;

    if (NV_DRM_WARN(nv_encoder == NULL ||
                    head_modeset_config->numDisplays >=
                        ARRAY_SIZE(head_modeset_config->displays))) {
        return -EINVAL;
    }
    head_modeset_config->displays[head_modeset_config->numDisplays++] =
        nv_encoder->hDisplay;
    return 0;
}

#if defined(NV_DRM_COLOR_MGMT_AVAILABLE)
static int color_mgmt_config_copy_lut(struct NvKmsLutRamps *nvkms_lut,
                                      struct drm_color_lut *drm_lut,
                                      uint64_t lut_len)
{
    uint64_t i = 0;
    if (lut_len != NVKMS_LUT_ARRAY_SIZE) {
        return -EINVAL;
    }

    /*
     * Both NvKms and drm LUT values are 16-bit linear values. NvKms LUT ramps
     * are in arrays in a single struct while drm LUT ramps are an array of
     * structs.
     */
    for (i = 0; i < lut_len; i++) {
        nvkms_lut->red[i]   = drm_lut[i].red;
        nvkms_lut->green[i] = drm_lut[i].green;
        nvkms_lut->blue[i]  = drm_lut[i].blue;
    }
    return 0;
}

static int color_mgmt_config_set_luts(struct nv_drm_crtc_state *nv_crtc_state,
                                      struct NvKmsKapiHeadRequestedConfig *req_config)
{
    struct NvKmsKapiHeadModeSetConfig *modeset_config =
        &req_config->modeSetConfig;
    struct drm_crtc_state *crtc_state = &nv_crtc_state->base;
    int ret = 0;

    /*
     * According to the comment in the Linux kernel's
     * drivers/gpu/drm/drm_color_mgmt.c, if either property is NULL, that LUT
     * needs to be changed to a linear LUT
     *
     * On failure, any LUT ramps allocated in this function are freed when the
     * subsequent atomic state cleanup calls nv_drm_atomic_crtc_destroy_state.
     */

    if (crtc_state->degamma_lut) {
        struct drm_color_lut *degamma_lut = NULL;
        uint64_t degamma_len = 0;

        if (!modeset_config->lut.input.pRamps) {
            modeset_config->lut.input.pRamps =
                nv_drm_calloc(1, sizeof(*modeset_config->lut.input.pRamps));
            if (!modeset_config->lut.input.pRamps) {
                return -ENOMEM;
            }
        }

        degamma_lut = (struct drm_color_lut *)crtc_state->degamma_lut->data;
        degamma_len = crtc_state->degamma_lut->length /
                      sizeof(struct drm_color_lut);

        if ((ret = color_mgmt_config_copy_lut(modeset_config->lut.input.pRamps,
                                              degamma_lut,
                                              degamma_len)) != 0) {
            return ret;
        }

        modeset_config->lut.input.depth     = 30; /* specify the full LUT */
        modeset_config->lut.input.start     = 0;
        modeset_config->lut.input.end       = degamma_len - 1;
    } else {
        /* setting input.end to 0 is equivalent to disabling the LUT, which
         * should be equivalent to a linear LUT */
        modeset_config->lut.input.depth     = 30; /* specify the full LUT */
        modeset_config->lut.input.start     = 0;
        modeset_config->lut.input.end       = 0;

        nv_drm_free(modeset_config->lut.input.pRamps);
        modeset_config->lut.input.pRamps    = NULL;
    }
    req_config->flags.ilutChanged = NV_TRUE;

    if (crtc_state->gamma_lut) {
        struct drm_color_lut *gamma_lut = NULL;
        uint64_t gamma_len = 0;

        if (!modeset_config->lut.output.pRamps) {
            modeset_config->lut.output.pRamps =
                nv_drm_calloc(1, sizeof(*modeset_config->lut.output.pRamps));
            if (!modeset_config->lut.output.pRamps) {
                return -ENOMEM;
            }
        }

        gamma_lut = (struct drm_color_lut *)crtc_state->gamma_lut->data;
        gamma_len = crtc_state->gamma_lut->length /
                    sizeof(struct drm_color_lut);

        if ((ret = color_mgmt_config_copy_lut(modeset_config->lut.output.pRamps,
                                              gamma_lut,
                                              gamma_len)) != 0) {
            return ret;
        }

        modeset_config->lut.output.enabled   = NV_TRUE;
    } else {
        /* disabling the output LUT should be equivalent to setting a linear
         * LUT */
        modeset_config->lut.output.enabled   = NV_FALSE;

        nv_drm_free(modeset_config->lut.output.pRamps);
        modeset_config->lut.output.pRamps    = NULL;
    }
    req_config->flags.olutChanged = NV_TRUE;

    return 0;
}
#endif /* NV_DRM_COLOR_MGMT_AVAILABLE */

/**
 * nv_drm_crtc_atomic_check() can fail after it has modified
 * the 'nv_drm_crtc_state::req_config', that is fine because 'nv_drm_crtc_state'
 * will be discarded if ->atomic_check() fails.
 */
#if defined(NV_DRM_CRTC_ATOMIC_CHECK_HAS_ATOMIC_STATE_ARG)
static int nv_drm_crtc_atomic_check(struct drm_crtc *crtc,
                                    struct drm_atomic_state *state)
#else
static int nv_drm_crtc_atomic_check(struct drm_crtc *crtc,
                                    struct drm_crtc_state *crtc_state)
#endif
{
#if defined(NV_DRM_CRTC_ATOMIC_CHECK_HAS_ATOMIC_STATE_ARG)
    struct drm_crtc_state *crtc_state =
        drm_atomic_get_new_crtc_state(state, crtc);
#endif
    struct nv_drm_crtc_state *nv_crtc_state = to_nv_crtc_state(crtc_state);
    struct NvKmsKapiHeadRequestedConfig *req_config =
        &nv_crtc_state->req_config;
    int ret = 0;

    if (crtc_state->mode_changed) {
        drm_mode_to_nvkms_display_mode(&crtc_state->mode,
                                       &req_config->modeSetConfig.mode);
        req_config->flags.modeChanged = NV_TRUE;
    }

    if (nv_drm_crtc_state_connectors_changed(crtc_state)) {
        struct NvKmsKapiHeadModeSetConfig *config = &req_config->modeSetConfig;
        struct drm_connector *connector;
        struct drm_connector_state *connector_state;
        int j;

        config->numDisplays = 0;

        memset(config->displays, 0, sizeof(config->displays));

        req_config->flags.displaysChanged = NV_TRUE;

        nv_drm_for_each_connector_in_state(crtc_state->state,
                                           connector, connector_state, j) {
            if (connector_state->crtc != crtc) {
                continue;
            }

            if ((ret = head_modeset_config_attach_connector(
                            to_nv_connector(connector),
                            config)) != 0) {
                return ret;
            }
        }
    }

    if (crtc_state->active_changed) {
        req_config->modeSetConfig.bActive = crtc_state->active;
        req_config->flags.activeChanged = NV_TRUE;
    }

#if defined(NV_DRM_CRTC_STATE_HAS_VRR_ENABLED)
    req_config->modeSetConfig.vrrEnabled = crtc_state->vrr_enabled;
#endif

#if defined(NV_DRM_COLOR_MGMT_AVAILABLE)
    if (crtc_state->color_mgmt_changed) {
        if ((ret = color_mgmt_config_set_luts(nv_crtc_state, req_config)) != 0) {
            return ret;
        }
    }
#endif

    return ret;
}

static bool
nv_drm_crtc_mode_fixup(struct drm_crtc *crtc,
                       const struct drm_display_mode *mode,
                       struct drm_display_mode *adjusted_mode)
{
    return true;
}

static const struct drm_crtc_helper_funcs nv_crtc_helper_funcs = {
    .atomic_check = nv_drm_crtc_atomic_check,
    .mode_fixup = nv_drm_crtc_mode_fixup,
};

static void nv_drm_plane_install_properties(
    struct drm_plane *plane,
    NvBool supportsICtCp)
{
    struct nv_drm_device *nv_dev = to_nv_device(plane->dev);

    if (nv_dev->nv_out_fence_property) {
        drm_object_attach_property(
            &plane->base, nv_dev->nv_out_fence_property, 0);
    }

    if (nv_dev->nv_input_colorspace_property) {
        drm_object_attach_property(
            &plane->base, nv_dev->nv_input_colorspace_property,
            NVKMS_INPUT_COLORSPACE_NONE);
    }

#if defined(NV_DRM_HAS_HDR_OUTPUT_METADATA)
    if (supportsICtCp && nv_dev->nv_hdr_output_metadata_property) {
        drm_object_attach_property(
            &plane->base, nv_dev->nv_hdr_output_metadata_property, 0);
    }
#endif
}

static void
__nv_drm_plane_create_alpha_blending_properties(struct drm_plane *plane,
                                                 NvU32 validCompModes)
{
#if defined(NV_DRM_ALPHA_BLENDING_AVAILABLE)
    if ((validCompModes &
         NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_SURFACE_ALPHA)) != 0x0 &&
        (validCompModes &
         NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_SURFACE_ALPHA)) != 0x0) {

        drm_plane_create_alpha_property(plane);
        drm_plane_create_blend_mode_property(plane,
                                             NVBIT(DRM_MODE_BLEND_PREMULTI) |
                                             NVBIT(DRM_MODE_BLEND_COVERAGE));
    } else if ((validCompModes &
                NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_ALPHA)) != 0x0 &&
               (validCompModes &
                NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_NON_PREMULT_ALPHA)) != 0x0) {

        drm_plane_create_blend_mode_property(plane,
                                             NVBIT(DRM_MODE_BLEND_PREMULTI) |
                                             NVBIT(DRM_MODE_BLEND_COVERAGE));
   }
#endif
}

static void
__nv_drm_plane_create_rotation_property(struct drm_plane *plane,
                                        NvU16 validLayerRRTransforms)
{
#if defined(NV_DRM_ROTATION_AVAILABLE)
    enum NvKmsRotation curRotation;
    NvU32 supported_rotations = 0;
    struct NvKmsRRParams rrParams = {
        .rotation = NVKMS_ROTATION_0,
        .reflectionX = true,
        .reflectionY = true,
    };

    if ((NVBIT(NvKmsRRParamsToCapBit(&rrParams)) &
        validLayerRRTransforms) != 0) {
        supported_rotations |= DRM_MODE_REFLECT_X;
        supported_rotations |= DRM_MODE_REFLECT_Y;
    }

    rrParams.reflectionX = false;
    rrParams.reflectionY = false;

    for (curRotation = NVKMS_ROTATION_MIN;
         curRotation <= NVKMS_ROTATION_MAX; curRotation++) {
        rrParams.rotation = curRotation;
        if ((NVBIT(NvKmsRRParamsToCapBit(&rrParams)) &
                    validLayerRRTransforms) == 0) {
            continue;
        }

        switch (curRotation) {
            case NVKMS_ROTATION_0:
                supported_rotations |= DRM_MODE_ROTATE_0;
                break;
            case NVKMS_ROTATION_90:
                supported_rotations |= DRM_MODE_ROTATE_90;
                break;
            case NVKMS_ROTATION_180:
                supported_rotations |= DRM_MODE_ROTATE_180;
                break;
            case NVKMS_ROTATION_270:
                supported_rotations |= DRM_MODE_ROTATE_270;
                break;
            default:
                break;
        }

    }

    if (supported_rotations != 0) {
        drm_plane_create_rotation_property(plane, DRM_MODE_ROTATE_0,
                                           supported_rotations);
    }
#endif
}

static struct drm_plane*
nv_drm_plane_create(struct drm_device *dev,
                    enum drm_plane_type plane_type,
                    uint32_t layer_idx,
                    NvU32 head,
                    const struct NvKmsKapiDeviceResourcesInfo *pResInfo)
{
#if defined(NV_DRM_UNIVERSAL_PLANE_INIT_HAS_FORMAT_MODIFIERS_ARG)
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    const NvU64 linear_modifiers[] = {
        DRM_FORMAT_MOD_LINEAR,
        DRM_FORMAT_MOD_INVALID,
    };
#endif
    enum NvKmsCompositionBlendingMode defaultCompositionMode;
    struct nv_drm_plane *nv_plane = NULL;
    struct nv_drm_plane_state *nv_plane_state = NULL;
    struct drm_plane *plane = NULL;
    int ret = -ENOMEM;
    uint32_t *formats = NULL;
    unsigned int formats_count = 0;
    const NvU32 validCompositionModes =
        (plane_type == DRM_PLANE_TYPE_CURSOR) ?
            pResInfo->caps.validCursorCompositionModes :
            pResInfo->caps.layer[layer_idx].validCompositionModes;
    const long unsigned int nvkms_formats_mask =
        (plane_type == DRM_PLANE_TYPE_CURSOR) ?
            pResInfo->caps.supportedCursorSurfaceMemoryFormats :
            pResInfo->supportedSurfaceMemoryFormats[layer_idx];
    const NvU16 validLayerRRTransforms =
        (plane_type == DRM_PLANE_TYPE_CURSOR) ?
            0x0 : pResInfo->caps.layer[layer_idx].validRRTransforms;

    if ((validCompositionModes &
         NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE)) != 0x0) {
        defaultCompositionMode = NVKMS_COMPOSITION_BLENDING_MODE_OPAQUE;
    } else if ((validCompositionModes &
                NVBIT(NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_ALPHA)) != 0x0) {
        defaultCompositionMode = NVKMS_COMPOSITION_BLENDING_MODE_PREMULT_ALPHA;
    } else {
        goto failed;
    }

    formats =
        nv_drm_format_array_alloc(&formats_count,
                                  nvkms_formats_mask);
    if (formats == NULL) {
        goto failed;
    }

    if ((nv_plane = nv_drm_calloc(1, sizeof(*nv_plane))) == NULL) {
        goto failed_plane_alloc;
    }
    plane = &nv_plane->base;

    nv_plane->defaultCompositionMode = defaultCompositionMode;
    nv_plane->layer_idx = layer_idx;

    if ((nv_plane_state =
            nv_drm_calloc(1, sizeof(*nv_plane_state))) == NULL) {
        goto failed_state_alloc;
    }

    plane->state = &nv_plane_state->base;
    plane->state->plane = plane;

    /*
     * Possible_crtcs for primary and cursor plane is zero because
     * drm_crtc_init_with_planes() will assign the plane's possible_crtcs
     * after the crtc is successfully initialized.
     */
    ret = drm_universal_plane_init(
        dev,
        plane,
        (plane_type == DRM_PLANE_TYPE_OVERLAY) ?
        (1 << head) : 0,
        &nv_plane_funcs,
        formats, formats_count,
#if defined(NV_DRM_UNIVERSAL_PLANE_INIT_HAS_FORMAT_MODIFIERS_ARG)
        (plane_type == DRM_PLANE_TYPE_CURSOR) ?
        linear_modifiers : nv_dev->modifiers,
#endif
        plane_type
#if defined(NV_DRM_UNIVERSAL_PLANE_INIT_HAS_NAME_ARG)
        , NULL
#endif
        );

    if (ret != 0) {
        goto failed_plane_init;
    }

    drm_plane_helper_add(plane, &nv_plane_helper_funcs);

    if (plane_type != DRM_PLANE_TYPE_CURSOR) {
        nv_drm_plane_install_properties(
                plane,
                pResInfo->supportsICtCp[layer_idx]);
    }

    __nv_drm_plane_create_alpha_blending_properties(
            plane,
            validCompositionModes);

    __nv_drm_plane_create_rotation_property(
            plane,
            validLayerRRTransforms);

    nv_drm_free(formats);

    return plane;

failed_plane_init:
    nv_drm_free(nv_plane_state);

failed_state_alloc:
    nv_drm_free(nv_plane);

failed_plane_alloc:
    nv_drm_free(formats);

failed:
    return ERR_PTR(ret);
}

/*
 * Add drm crtc for given head and supported enum NvKmsSurfaceMemoryFormats.
 */
static struct drm_crtc *__nv_drm_crtc_create(struct nv_drm_device *nv_dev,
                                             struct drm_plane *primary_plane,
                                             struct drm_plane *cursor_plane,
                                             unsigned int head)
{
    struct nv_drm_crtc *nv_crtc = NULL;
    struct nv_drm_crtc_state *nv_state = NULL;
    int ret = -ENOMEM;

    if ((nv_crtc = nv_drm_calloc(1, sizeof(*nv_crtc))) == NULL) {
        goto failed;
    }

    nv_state = nv_drm_crtc_state_alloc();
    if (nv_state == NULL) {
        goto failed_state_alloc;
    }

    nv_crtc->base.state = &nv_state->base;
    nv_crtc->base.state->crtc = &nv_crtc->base;

    nv_crtc->head = head;
    INIT_LIST_HEAD(&nv_crtc->flip_list);
    spin_lock_init(&nv_crtc->flip_list_lock);
    nv_crtc->modeset_permission_filep = NULL;

    ret = drm_crtc_init_with_planes(nv_dev->dev,
                                    &nv_crtc->base,
                                    primary_plane, cursor_plane,
                                    &nv_crtc_funcs
#if defined(NV_DRM_CRTC_INIT_WITH_PLANES_HAS_NAME_ARG)
                                    , NULL
#endif
                                    );

    if (ret != 0) {
        NV_DRM_DEV_LOG_ERR(
            nv_dev,
            "Failed to init crtc for head %u with planes", head);
        goto failed_init_crtc;
    }

    /* Add crtc to drm sub-system */

    drm_crtc_helper_add(&nv_crtc->base, &nv_crtc_helper_funcs);

#if defined(NV_DRM_COLOR_MGMT_AVAILABLE)
#if defined(NV_DRM_CRTC_ENABLE_COLOR_MGMT_PRESENT)
    drm_crtc_enable_color_mgmt(&nv_crtc->base, NVKMS_LUT_ARRAY_SIZE, true,
                               NVKMS_LUT_ARRAY_SIZE);
#else
    drm_helper_crtc_enable_color_mgmt(&nv_crtc->base, NVKMS_LUT_ARRAY_SIZE,
                                      NVKMS_LUT_ARRAY_SIZE);
#endif
    ret = drm_mode_crtc_set_gamma_size(&nv_crtc->base, NVKMS_LUT_ARRAY_SIZE);
    if (ret != 0) {
        NV_DRM_DEV_LOG_WARN(
            nv_dev,
            "Failed to initialize legacy gamma support for head %u", head);
    }
#endif

    return &nv_crtc->base;

failed_init_crtc:
    nv_drm_free(nv_state);

failed_state_alloc:
    nv_drm_free(nv_crtc);

failed:
    return ERR_PTR(ret);
}

void nv_drm_enumerate_crtcs_and_planes(
    struct nv_drm_device *nv_dev,
    const struct NvKmsKapiDeviceResourcesInfo *pResInfo)
{
    unsigned int i;

    for (i = 0; i < pResInfo->numHeads; i++) {
        struct drm_plane *primary_plane = NULL, *cursor_plane = NULL;
        NvU32 layer;

        if (pResInfo->numLayers[i] <= NVKMS_KAPI_LAYER_PRIMARY_IDX) {
            continue;
        }

        primary_plane =
            nv_drm_plane_create(nv_dev->dev,
                                DRM_PLANE_TYPE_PRIMARY,
                                NVKMS_KAPI_LAYER_PRIMARY_IDX,
                                i,
                                pResInfo);

        if (IS_ERR(primary_plane)) {
            NV_DRM_DEV_LOG_ERR(
                nv_dev,
                "Failed to create primary plane for head %u, error = %ld",
                i, PTR_ERR(primary_plane));
            continue;
        }

        cursor_plane =
            nv_drm_plane_create(nv_dev->dev,
                                DRM_PLANE_TYPE_CURSOR,
                                NVKMS_KAPI_LAYER_INVALID_IDX,
                                i,
                                pResInfo);
        if (IS_ERR(cursor_plane)) {
            NV_DRM_DEV_LOG_ERR(
                nv_dev,
                "Failed to create cursor plane for head %u, error = %ld",
                i, PTR_ERR(cursor_plane));
            cursor_plane = NULL;
        }

        /* Create crtc with the primary and cursor planes */
        {
            struct drm_crtc *crtc =
                __nv_drm_crtc_create(nv_dev,
                                     primary_plane, cursor_plane,
                                     i);
            if (IS_ERR(crtc)) {
                nv_drm_plane_destroy(primary_plane);

                if (cursor_plane != NULL) {
                    nv_drm_plane_destroy(cursor_plane);
                }

                NV_DRM_DEV_LOG_ERR(
                    nv_dev,
                    "Failed to add DRM CRTC for head %u, error = %ld",
                    i, PTR_ERR(crtc));
                continue;
            }
        }

        for (layer = 0; layer < pResInfo->numLayers[i]; layer++) {
            struct drm_plane *overlay_plane = NULL;

            if (layer == NVKMS_KAPI_LAYER_PRIMARY_IDX) {
                continue;
            }

            overlay_plane =
                nv_drm_plane_create(nv_dev->dev,
                                    DRM_PLANE_TYPE_OVERLAY,
                                    layer,
                                    i,
                                    pResInfo);

            if (IS_ERR(overlay_plane)) {
                NV_DRM_DEV_LOG_ERR(
                    nv_dev,
                    "Failed to create plane for layer-%u of head %u, error = %ld",
                    layer, i, PTR_ERR(overlay_plane));
            }
        }

    }
}
/*
 * Helper function to convert NvKmsKapiCrcs to drm_nvidia_crtc_crc32_out.
 */
static void NvKmsKapiCrcsToDrm(const struct NvKmsKapiCrcs *crcs,
                               struct drm_nvidia_crtc_crc32_v2_out *drmCrcs)
{
    drmCrcs->outputCrc32.value = crcs->outputCrc32.value;
    drmCrcs->outputCrc32.supported = crcs->outputCrc32.supported;
    drmCrcs->outputCrc32.__pad0 = 0;
    drmCrcs->outputCrc32.__pad1 = 0;
    drmCrcs->rasterGeneratorCrc32.value = crcs->rasterGeneratorCrc32.value;
    drmCrcs->rasterGeneratorCrc32.supported = crcs->rasterGeneratorCrc32.supported;
    drmCrcs->rasterGeneratorCrc32.__pad0 = 0;
    drmCrcs->rasterGeneratorCrc32.__pad1 = 0;
    drmCrcs->compositorCrc32.value = crcs->compositorCrc32.value;
    drmCrcs->compositorCrc32.supported = crcs->compositorCrc32.supported;
    drmCrcs->compositorCrc32.__pad0 = 0;
    drmCrcs->compositorCrc32.__pad1 = 0;
}

int nv_drm_get_crtc_crc32_v2_ioctl(struct drm_device *dev,
                                   void *data, struct drm_file *filep)
{
    struct drm_nvidia_get_crtc_crc32_v2_params *params = data;
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct drm_crtc *crtc = NULL;
    struct nv_drm_crtc *nv_crtc = NULL;
    struct NvKmsKapiCrcs crc32;

    if (!drm_core_check_feature(dev, DRIVER_MODESET)) {
        return -EOPNOTSUPP;
    }

    crtc = nv_drm_crtc_find(dev, filep, params->crtc_id);
    if (!crtc) {
        return -ENOENT;
    }

    nv_crtc = to_nv_crtc(crtc);

    if (!nvKms->getCRC32(nv_dev->pDevice, nv_crtc->head, &crc32)) {
        return -ENODEV;
    }
    NvKmsKapiCrcsToDrm(&crc32, &params->crc32);

    return 0;
}

int nv_drm_get_crtc_crc32_ioctl(struct drm_device *dev,
                                void *data, struct drm_file *filep)
{
    struct drm_nvidia_get_crtc_crc32_params *params = data;
    struct nv_drm_device *nv_dev = to_nv_device(dev);
    struct drm_crtc *crtc = NULL;
    struct nv_drm_crtc *nv_crtc = NULL;
    struct NvKmsKapiCrcs crc32;

    if (!drm_core_check_feature(dev, DRIVER_MODESET)) {
        return -EOPNOTSUPP;
    }

    crtc = nv_drm_crtc_find(dev, filep, params->crtc_id);
    if (!crtc) {
        return -ENOENT;
    }

    nv_crtc = to_nv_crtc(crtc);

    if (!nvKms->getCRC32(nv_dev->pDevice, nv_crtc->head, &crc32)) {
        return -ENODEV;
    }
    params->crc32 = crc32.outputCrc32.value;

    return 0;
}

#endif
