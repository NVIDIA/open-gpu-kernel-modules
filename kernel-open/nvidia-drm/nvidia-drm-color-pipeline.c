/*
 * Copyright (c) 2026, NVIDIA CORPORATION. All rights reserved.
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

#include "nvidia-drm-conftest.h"

#if defined(NV_DRM_AVAILABLE) && defined(NV_DRM_HAS_COLOROP)

#include "nvidia-drm-priv.h"
#include "nvidia-drm-crtc.h"
#include "nvidia-drm-color-pipeline.h"
#include "nvidia-drm-os-interface.h"

#include <drm/drm_colorop.h>
#include <drm/drm_plane.h>
#include <drm/drm_print.h>
#include <drm/drm_atomic_uapi.h>

/**
 * mul_s31_32_scalar - Multiply S31.32 by unsigned integer scalar
 * @a: S31.32 sign-magnitude value
 * @scalar: Unsigned integer multiplier
 * @result: Output pointer for result in S31.32 format
 *
 * Returns: 0 on success, -EINVAL if result would overflow S31.32 format
 */
static inline int mul_s31_32_scalar(NvU64 a, NvU32 scalar, NvU64 *result)
{
    if (scalar == 0) {
        *result = 0;
    } else if (scalar == 1) {
        *result = a;
    } else {
        NvU64 sign_a = a &  (1ULL << 63);
        NvU64 mag_a  = a & ~(1ULL << 63);

        /* mag_a * scalar must fit in 63 bits */
        if (mag_a > (((1ULL << 63) - 1) / scalar)) {
            return -EINVAL;
        }

        *result = sign_a | (mag_a * scalar);
    }
    return 0;
}

static enum drm_colorop_curve_1d_type
transfer_function_to_degamma_colorop_curve(enum nv_drm_transfer_function tf)
{
    _Static_assert(NV_DRM_TRANSFER_FUNCTION_MAX == NV_DRM_TRANSFER_FUNCTION_SRGB,
                   "NV_DRM_TRANSFER_FUNCTION_MAX changed, "
                   "update transfer_function_to_colorop_curve()");

    switch (tf) {
        case NV_DRM_TRANSFER_FUNCTION_PQ:
            return DRM_COLOROP_1D_CURVE_PQ_125_EOTF;
        case NV_DRM_TRANSFER_FUNCTION_DEFAULT:
        case NV_DRM_TRANSFER_FUNCTION_LINEAR:
        default:
            return DRM_COLOROP_1D_CURVE_COUNT;
    }
}

static enum nv_drm_transfer_function
colorop_curve_to_transfer_function(enum drm_colorop_curve_1d_type curve_type)
{
    _Static_assert(NV_DRM_TRANSFER_FUNCTION_MAX == NV_DRM_TRANSFER_FUNCTION_SRGB,
                   "NV_DRM_TRANSFER_FUNCTION_MAX changed, "
                   "update colorop_curve_to_transfer_function()");

    switch (curve_type) {
        case DRM_COLOROP_1D_CURVE_PQ_125_EOTF:
            return NV_DRM_TRANSFER_FUNCTION_PQ;
        default:
            WARN_ON_ONCE("Unsupported curve type");
            return NV_DRM_TRANSFER_FUNCTION_DEFAULT;
    }
}

static NvU64 plane_get_supported_degamma_colorop_curves(void)
{
    NvU64 supported = 0;
    enum nv_drm_transfer_function tf;

    for (tf = 0; tf < NV_DRM_TRANSFER_FUNCTION_MAX; tf++) {
        enum drm_colorop_curve_1d_type curve_type;

        curve_type = transfer_function_to_degamma_colorop_curve(tf);
        if (curve_type != DRM_COLOROP_1D_CURVE_COUNT) {
            supported |= BIT_ULL(curve_type);
        }
    }

    return supported;
}

static void plane_destroy_pipeline_colorops(struct nv_drm_color_pipeline *pipeline)
{
    if (!pipeline) {
        return;
    }

#define DESTROY_COLOROP(field) \
    if (pipeline->field) { \
        nv_drm_colorop_destroy(&pipeline->field->colorop); \
    }

    DESTROY_COLOROP(fmt_ctm);
    DESTROY_COLOROP(degamma_tf);
    DESTROY_COLOROP(degamma_lut);
    DESTROY_COLOROP(degamma_multiplier);
    DESTROY_COLOROP(lms_ctm);
    DESTROY_COLOROP(pq_inv_eotf);
    DESTROY_COLOROP(lms_to_itp_ctm);
    DESTROY_COLOROP(tmo_lut);
    DESTROY_COLOROP(itp_to_lms_ctm);
    DESTROY_COLOROP(pq_eotf);
    DESTROY_COLOROP(blend_ctm);

#undef DESTROY_COLOROP
}

/**
 * plane_build_single_pipeline - Helper to build one pipeline
 * @plane: DRM plane
 * @include_ictcp: Whether to include ICtCp colorops
 * @include_ilut: Whether to include ILUT colorops (DEGAMMA_TF, DEGAMMA_LUT, DEGAMMA_MULTIPLIER)
 * @name: Pipeline name
 *
 * Returns: Allocated pipeline on success, ERR_PTR on failure
 */
static struct nv_drm_color_pipeline *
plane_build_single_pipeline(struct drm_plane *plane,
                            NvBool include_ictcp,
                            NvBool include_ilut,
                            const char *name)
{
    struct nv_drm_plane *nv_plane = to_nv_plane(plane);
    struct drm_device *dev = plane->dev;
    struct nv_drm_color_pipeline *pipeline;
    struct drm_colorop *prev_colorop = NULL;
    int ret;

    WARN_ON_ONCE(include_ilut && !nv_plane->ilut_caps.supported);

    /* Allocate color pipeline structure */
    pipeline = nv_drm_calloc(1, sizeof(*pipeline));
    if (!pipeline) {
        return ERR_PTR(-ENOMEM);
    }

    /* Set pipeline name */
    pipeline->name = name;

    /* Build the color pipeline chain */

#define CREATE_COLOROP(field, init_call, error_name, condition) \
    if (condition) { \
        pipeline->field = nv_drm_calloc(1, sizeof(*pipeline->field)); \
        if (!pipeline->field) { \
            ret = -ENOMEM; \
            goto failed; \
        } \
        pipeline->field->pipeline = pipeline; \
        ret = init_call; \
        if (ret) { \
            NV_DRM_DEV_LOG_ERR(to_nv_device(dev), \
                              "Failed to create " error_name " colorop: %d", ret); \
            goto failed; \
        } \
        if (prev_colorop) { \
            drm_colorop_set_next_property(prev_colorop, &pipeline->field->colorop); \
        } \
        prev_colorop = &pipeline->field->colorop; \
    }

    /* 0. FMT_CTM: 3x4 matrix for YUV to RGB conversion (FMT) */
    CREATE_COLOROP(fmt_ctm,
                   nv_drm_plane_colorop_ctm_3x4_init(dev, &pipeline->fmt_ctm->colorop, plane,
                                                     DRM_COLOROP_FLAG_ALLOW_BYPASS),
                   "FMT_CTM",
                   NV_TRUE /* Always included */);

    /* 1. DEGAMMA_TF: 1D Curve for transfer function (ILUT) */
    CREATE_COLOROP(degamma_tf,
                   nv_drm_plane_colorop_curve_1d_init(dev, &pipeline->degamma_tf->colorop, plane,
                                                      plane_get_supported_degamma_colorop_curves(),
                                                      DRM_COLOROP_FLAG_ALLOW_BYPASS),
                   "DEGAMMA_TF",
                   include_ilut);

    /* 2. DEGAMMA_LUT: Custom 1D LUT (ILUT) */
    CREATE_COLOROP(degamma_lut,
                   nv_drm_plane_colorop_curve_1d_lut_init(dev, &pipeline->degamma_lut->colorop, plane,
                                                          NVKMS_LUT_ARRAY_SIZE,
                                                          DRM_COLOROP_LUT1D_INTERPOLATION_LINEAR,
                                                          DRM_COLOROP_FLAG_ALLOW_BYPASS),
                   "DEGAMMA_LUT",
                   include_ilut);

    /* 3. DEGAMMA_MULTIPLIER: Gain/multiplier (ILUT) */
    CREATE_COLOROP(degamma_multiplier,
                   nv_drm_plane_colorop_mult_init(dev, &pipeline->degamma_multiplier->colorop, plane,
                                                  DRM_COLOROP_FLAG_ALLOW_BYPASS),
                   "DEGAMMA_MULTIPLIER",
                   include_ilut);

    /* 4. LMS_CTM: 3x4 matrix (CSC00) */
    CREATE_COLOROP(lms_ctm,
                   nv_drm_plane_colorop_ctm_3x4_init(dev, &pipeline->lms_ctm->colorop, plane,
                                                     DRM_COLOROP_FLAG_ALLOW_BYPASS),
                   "LMS_CTM",
                   include_ictcp);

    /* 4.5. PQ_INV_EOTF: Non-bypassable 1D curve for linear to PQ conversion (CSC0LUT) */
    CREATE_COLOROP(pq_inv_eotf,
                   nv_drm_plane_colorop_curve_1d_init(dev, &pipeline->pq_inv_eotf->colorop, plane,
                                                      BIT_ULL(DRM_COLOROP_1D_CURVE_PQ_125_INV_EOTF),
                                                      0),
                   "PQ_INV_EOTF",
                   include_ictcp);

    /* 5. LMS_TO_ITP_CTM: 3x4 matrix (CSC01) */
    CREATE_COLOROP(lms_to_itp_ctm,
                   nv_drm_plane_colorop_ctm_3x4_init(dev, &pipeline->lms_to_itp_ctm->colorop, plane,
                                                     DRM_COLOROP_FLAG_ALLOW_BYPASS),
                   "LMS_TO_ITP_CTM",
                   include_ictcp);

    /* 6. TMO_LUT: Single-channel tone mapping LUT (TMO) */
    CREATE_COLOROP(tmo_lut,
                   nv_drm_plane_colorop_curve_1d_lut_init(dev, &pipeline->tmo_lut->colorop, plane,
                                                          NVKMS_LUT_ARRAY_SIZE,
                                                          DRM_COLOROP_LUT1D_INTERPOLATION_LINEAR,
                                                          DRM_COLOROP_FLAG_ALLOW_BYPASS),
                   "TMO_LUT",
                   include_ictcp && nv_plane->tmo_caps.supported);

    /* 7. ITP_TO_LMS_CTM: 3x4 matrix (CSC10) */
    CREATE_COLOROP(itp_to_lms_ctm,
                   nv_drm_plane_colorop_ctm_3x4_init(dev, &pipeline->itp_to_lms_ctm->colorop, plane,
                                                     DRM_COLOROP_FLAG_ALLOW_BYPASS),
                   "ITP_TO_LMS_CTM",
                   include_ictcp);

    /* 7.5. PQ_EOTF: Non-bypassable 1D curve for PQ to linear conversion (CSC1LUT) */
    CREATE_COLOROP(pq_eotf,
                   nv_drm_plane_colorop_curve_1d_init(dev, &pipeline->pq_eotf->colorop, plane,
                                                      BIT_ULL(DRM_COLOROP_1D_CURVE_PQ_125_EOTF),
                                                      0),
                   "PQ_EOTF",
                   include_ictcp);

    /* 8. BLEND_CTM: 3x4 matrix (CSC11) */
    CREATE_COLOROP(blend_ctm,
                   nv_drm_plane_colorop_ctm_3x4_init(dev, &pipeline->blend_ctm->colorop, plane,
                                                     DRM_COLOROP_FLAG_ALLOW_BYPASS),
                   "BLEND_CTM",
                   NV_TRUE /* Always included */);

#undef CREATE_COLOROP

    return pipeline;

failed:
    plane_destroy_pipeline_colorops(pipeline);
    nv_drm_free(pipeline);
    return ERR_PTR(ret);
}

static int plane_update_state_from_colorops(struct drm_plane *plane,
                                            struct drm_plane_state *plane_state)
{
    nv_drm_atomic_state_base_t *state = plane_state->state;
    struct nv_drm_plane_state *nv_plane_state = to_nv_drm_plane_state(plane_state);
    struct nv_drm_colorop *nv_colorop;
    struct nv_drm_color_pipeline *pipeline;
    struct drm_colorop_state *colorop_state;
    NvU32 pq_implicit_mult = 1;

    if (!plane_state->color_pipeline) {
        /* No pipeline selected */
        return 0;
    }

    /* Extract pipeline from colorop */
    nv_colorop = container_of(plane_state->color_pipeline, struct nv_drm_colorop, colorop);
    pipeline = nv_colorop->pipeline;

#define HANDLE_CTM(field, plane_state_field) \
    if (pipeline->field) { \
        NvBool replaced; \
        colorop_state = drm_atomic_get_colorop_state(state, &pipeline->field->colorop); \
        if (IS_ERR(colorop_state)) { \
            return PTR_ERR(colorop_state); \
        } \
        if (!colorop_state->bypass && colorop_state->data) { \
            nv_drm_atomic_replace_property_blob(&nv_plane_state->plane_state_field, \
                                               colorop_state->data, &replaced); \
        } else if (!colorop_state->bypass && !colorop_state->data) { \
            /* Not bypassed but didn't set a blob */ \
            return -EINVAL; \
        } else { \
            nv_drm_atomic_replace_property_blob(&nv_plane_state->plane_state_field, NULL, &replaced); \
        } \
    }

#define HANDLE_LUT(field, plane_state_field, changed_field) \
    if (pipeline->field) { \
        NvBool replaced; \
        colorop_state = drm_atomic_get_colorop_state(state, &pipeline->field->colorop); \
        if (IS_ERR(colorop_state)) { \
            return PTR_ERR(colorop_state); \
        } \
        if (!colorop_state->bypass && colorop_state->data) { \
            nv_drm_atomic_replace_property_blob(&nv_plane_state->plane_state_field, \
                                               colorop_state->data, &replaced); \
            if (replaced) { \
                nv_plane_state->changed_field = NV_TRUE; \
            } \
        } else if (!colorop_state->bypass && !colorop_state->data) { \
            /* Not bypassed but didn't set a blob */ \
            return -EINVAL; \
        } else { \
            nv_drm_atomic_replace_property_blob(&nv_plane_state->plane_state_field, NULL, &replaced); \
            if (replaced) { \
                nv_plane_state->changed_field = NV_TRUE; \
            } \
        } \
    }

    /* 0. Handle FMT_CTM (3x4 Matrix for YUV to RGB) */
    HANDLE_CTM(fmt_ctm, fmt_ctm);

    /* 1. Handle DEGAMMA_TF (1D Curve) */
    if (pipeline->degamma_tf) {
        colorop_state =
            drm_atomic_get_colorop_state(state, &pipeline->degamma_tf->colorop);
        if (IS_ERR(colorop_state)) {
            return PTR_ERR(colorop_state);
        }

        if (!colorop_state->bypass) {
            enum nv_drm_transfer_function new_tf =
                colorop_curve_to_transfer_function(colorop_state->curve_1d_type);

            if (nv_plane_state->degamma_tf != new_tf) {
                nv_plane_state->degamma_tf = new_tf;
                nv_plane_state->degamma_changed = NV_TRUE;
            }

            /*
             * DRM_COLOROP_1D_CURVE_PQ_125_EOTF expects 10,000 nits to map to 125.0,
             * but NV_DRM_TRANSFER_FUNCTION_PQ expects 10,000 nits to map to 1.0.
             * Apply implicit 125x scaling via degamma_multiplier.
             */
            pq_implicit_mult =
                (colorop_state->curve_1d_type == DRM_COLOROP_1D_CURVE_PQ_125_EOTF) ?
                125 : 1;
        } else {
            if (nv_plane_state->degamma_tf != NV_DRM_TRANSFER_FUNCTION_DEFAULT) {
                nv_plane_state->degamma_tf = NV_DRM_TRANSFER_FUNCTION_DEFAULT;
                nv_plane_state->degamma_changed = NV_TRUE;
            }
            pq_implicit_mult = 1;
        }
        WARN_ON_ONCE((pq_implicit_mult != 1) && !pipeline->degamma_multiplier);
    }

    /* 2. Handle DEGAMMA_LUT (1D LUT) */
    HANDLE_LUT(degamma_lut, degamma_lut, degamma_changed);

    /* 3. Handle DEGAMMA_MULTIPLIER */
    if (pipeline->degamma_multiplier) {
        int ret;
        NvU64 colorop_mult;
        NvU64 new_mult;

        colorop_state =
            drm_atomic_get_colorop_state(state, &pipeline->degamma_multiplier->colorop);
        if (IS_ERR(colorop_state)) {
            return PTR_ERR(colorop_state);
        }

        if (!colorop_state->bypass) {
            colorop_mult = colorop_state->multiplier;
        } else {
            /* Bypassed, but still apply implicit PQ multiplier */
            colorop_mult = NV_DRM_S31_32_ONE;
        }

        /*
         * Apply implicit PQ 125 multiplier on top of the explicit multiplier.
         * If DEGAMMA_TF is PQ_125_EOTF, pq_implicit_mult is 125.
         * Otherwise, pq_implicit_mult is 1 (no-op).
         */
        ret = mul_s31_32_scalar(colorop_mult, pq_implicit_mult, &new_mult);
        if (ret != 0) {
            return ret;
        }

        if (nv_plane_state->degamma_multiplier != new_mult) {
            nv_plane_state->degamma_multiplier = new_mult;
            nv_plane_state->degamma_changed = NV_TRUE;
        }
    }

    /* 4. Handle LMS_CTM (3x4 Matrix) */
    HANDLE_CTM(lms_ctm, lms_ctm);

    /* 5. Handle LMS_TO_ITP_CTM (3x4 Matrix) */
    HANDLE_CTM(lms_to_itp_ctm, lms_to_itp_ctm);

    /* 6. Handle TMO_LUT (1D LUT) */
    HANDLE_LUT(tmo_lut, tmo_lut, tmo_changed);

    /* 7. Handle ITP_TO_LMS_CTM (3x4 Matrix) */
    HANDLE_CTM(itp_to_lms_ctm, itp_to_lms_ctm);

    /* 8. Handle BLEND_CTM (3x4 Matrix) */
    HANDLE_CTM(blend_ctm, blend_ctm);

#undef HANDLE_CTM
#undef HANDLE_LUT

    return 0;
}

int nv_drm_plane_create_color_pipelines(struct drm_plane *plane,
                                        NvBool supportsICtCp)
{
    struct nv_drm_plane *nv_plane = to_nv_plane(plane);
    int pipeline_count = 0;
    int ret;

#define CREATE_PIPELINE(include_ictcp, include_ilut, name, condition) \
    if (condition) { \
        nv_plane->color_pipelines[pipeline_count] = \
            plane_build_single_pipeline(plane, include_ictcp, include_ilut, name); \
        if (IS_ERR(nv_plane->color_pipelines[pipeline_count])) { \
            ret = PTR_ERR(nv_plane->color_pipelines[pipeline_count]); \
            nv_plane->color_pipelines[pipeline_count] = NULL; \
            goto failed; \
        } \
        pipeline_count++; \
    }

    /* Create Full Pipeline if supported */
    CREATE_PIPELINE(NV_TRUE,  /* include_ictcp */
                    NV_TRUE,  /* include_ilut */
                    "NVIDIA Full",
                    supportsICtCp);

    /* Create Lite Pipeline (always created) */
    CREATE_PIPELINE(NV_FALSE, /* include_ictcp */
                    NV_TRUE,  /* include_ilut */
                    "NVIDIA Lite",
                    NV_TRUE);

    /* Create Floating Point FB Full Pipeline if supported */
    CREATE_PIPELINE(NV_TRUE,  /* include_ictcp */
                    NV_FALSE, /* include_ilut */
                    "NVIDIA FP Full",
                    supportsICtCp);

    /* Create Floating Point FB Lite Pipeline (always created) */
    CREATE_PIPELINE(NV_FALSE, /* include_ictcp */
                    NV_FALSE, /* include_ilut */
                    "NVIDIA FP Lite",
                    NV_TRUE);

#undef CREATE_PIPELINE

    nv_plane->num_color_pipelines = pipeline_count;
    return 0;

failed:
    nv_drm_plane_destroy_color_pipelines(plane);
    return ret;
}

void nv_drm_plane_destroy_color_pipelines(struct drm_plane *plane)
{
    struct nv_drm_plane *nv_plane = to_nv_plane(plane);
    int i;

    for (i = 0; i < NV_DRM_PLANE_MAX_COLOR_PIPELINES; i++) {
        struct nv_drm_color_pipeline *pipeline = nv_plane->color_pipelines[i];

        if (!pipeline) {
            continue;
        }

        nv_plane->color_pipelines[i] = NULL;
        plane_destroy_pipeline_colorops(pipeline);
        nv_drm_free(pipeline);
    }

    nv_plane->num_color_pipelines = 0;
}

int nv_drm_plane_create_color_pipeline_property(struct drm_plane *plane)
{
    struct nv_drm_plane *nv_plane = to_nv_plane(plane);
    struct drm_prop_enum_list *pipeline_enums;
    int i;
    int ret;

    if (nv_plane->num_color_pipelines == 0) {
        return -EINVAL;
    }

    pipeline_enums = nv_drm_calloc(nv_plane->num_color_pipelines,
                                   sizeof(*pipeline_enums));
    if (!pipeline_enums) {
        return -ENOMEM;
    }

    for (i = 0; i < nv_plane->num_color_pipelines; i++) {
        struct nv_drm_color_pipeline *pipeline = nv_plane->color_pipelines[i];

        if (!pipeline) {
            ret = -EINVAL;
            goto done;
        }

        /* All pipelines must start with FMT CTM */
        WARN_ON(pipeline->fmt_ctm == NULL);

        /* Create enum entry pointing to the first colorop, the FMT CTM */
        pipeline_enums[i].type = pipeline->fmt_ctm->colorop.base.id;
        pipeline_enums[i].name = pipeline->name;
    }

    /* Create the COLOR_PIPELINE property with all pipelines */
    ret = drm_plane_create_color_pipeline_property(plane, pipeline_enums,
                                                   nv_plane->num_color_pipelines);

done:
    nv_drm_free(pipeline_enums);
    return ret;
}

int nv_drm_plane_process_color_mgmt_state(struct drm_plane *plane,
                                          struct drm_plane_state *plane_state)
{
    struct nv_drm_plane *nv_plane = to_nv_plane(plane);
    struct nv_drm_plane_state *nv_plane_state = to_nv_drm_plane_state(plane_state);
    int ret;
    NvBool clear_state = false;

    if (nv_plane->num_color_pipelines == 0) {
        return 0;
    }

    /*
     * Enforce mutual exclusion: vendor properties and COLOR_PIPELINE cannot
     * both be active. Clear stale state when switching between APIs.
     */

    /* COLOR_PIPELINE is set but cap is disabled, clear COLOR_PIPELINE and plane state */
    if (plane_state->color_pipeline && !plane_state->state->plane_color_pipeline) {
        drm_atomic_set_colorop_for_plane(plane_state, NULL);
        clear_state = true;
    }

    /* COLOR_PIPELINE is not set but cap is enabled, clear plane state */
    if (!plane_state->color_pipeline && plane_state->state->plane_color_pipeline) {
        clear_state = true;
    }

    if (clear_state) {
        drm_property_blob_put(nv_plane_state->fmt_ctm);
        nv_plane_state->fmt_ctm = NULL;

        drm_property_blob_put(nv_plane_state->lms_ctm);
        nv_plane_state->lms_ctm = NULL;

        drm_property_blob_put(nv_plane_state->lms_to_itp_ctm);
        nv_plane_state->lms_to_itp_ctm = NULL;

        drm_property_blob_put(nv_plane_state->itp_to_lms_ctm);
        nv_plane_state->itp_to_lms_ctm = NULL;

        drm_property_blob_put(nv_plane_state->blend_ctm);
        nv_plane_state->blend_ctm = NULL;

        nv_plane_state->degamma_tf = NV_DRM_TRANSFER_FUNCTION_DEFAULT;
        drm_property_blob_put(nv_plane_state->degamma_lut);
        nv_plane_state->degamma_lut = NULL;
        nv_plane_state->degamma_multiplier = NV_DRM_S31_32_ONE;
        nv_plane_state->degamma_changed = true;

        drm_property_blob_put(nv_plane_state->tmo_lut);
        nv_plane_state->tmo_lut = NULL;
        nv_plane_state->tmo_changed = true;
        nv_plane_state->tmo_sat_corr_enable = NV_DRM_TMO_SAT_CORR_ENABLE_DEFAULT;
    }

    /* If color pipeline is active, translate colorop state to nv_plane_state */
    if (plane_state->color_pipeline && plane_state->state->plane_color_pipeline) {
        /*
         * SAT_CORR has no colorop equivalent today. Force vendor SAT_CORR state
         * to default so stale vendor state cannot leak into upstream mode.
         */
        if (nv_plane_state->tmo_sat_corr_enable != NV_DRM_TMO_SAT_CORR_ENABLE_DEFAULT) {
            nv_plane_state->tmo_sat_corr_enable = NV_DRM_TMO_SAT_CORR_ENABLE_DEFAULT;
            nv_plane_state->tmo_changed = true;
        }

        /*
         * To prevent NVKMS from implicitly configuring CSC matrices in ways the
         * client may not expect, force all CTMs to default to the identity if
         * they aren't otherwise specified.
         */
        nv_plane_state->ctms_default_to_identity = true;

        ret = plane_update_state_from_colorops(plane, plane_state);
        if (ret != 0) {
            return ret;
        }
    } else {
        nv_plane_state->ctms_default_to_identity = false;
    }

    return 0;
}

#endif /* NV_DRM_AVAILABLE && NV_DRM_HAS_COLOROP */

