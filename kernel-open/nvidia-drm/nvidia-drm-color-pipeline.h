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

#ifndef __NVIDIA_DRM_COLOR_PIPELINE_H__
#define __NVIDIA_DRM_COLOR_PIPELINE_H__

#include "nvidia-drm-conftest.h"

#if defined(NV_DRM_AVAILABLE) && defined(NV_DRM_HAS_COLOROP)

#include <drm/drm_plane.h>
#include <drm/drm_colorop.h>

#define NV_DRM_PLANE_MAX_COLOR_PIPELINES 4

struct nv_drm_device;
struct nv_drm_plane;
struct nv_drm_plane_state;

#if defined(NV_DRM_COLOROP_HAS_FUNCS)
static const struct drm_colorop_funcs nv_drm_plane_colorop_funcs;
#endif

static int nv_drm_plane_colorop_curve_1d_lut_init(
    struct drm_device *dev, struct drm_colorop *colorop,
    struct drm_plane *plane, uint32_t lut_size,
    enum drm_colorop_lut1d_interpolation_type interpolation,
    uint32_t flags)
{
#if defined(NV_DRM_COLOROP_HAS_FUNCS)
    return drm_plane_colorop_curve_1d_lut_init(dev, colorop, plane,
                                               &nv_drm_plane_colorop_funcs,
                                               lut_size, interpolation, flags);
#else
    return drm_plane_colorop_curve_1d_lut_init(dev, colorop, plane,
                                               lut_size, interpolation, flags);
#endif
}

static int nv_drm_plane_colorop_curve_1d_init(
    struct drm_device *dev, struct drm_colorop *colorop,
    struct drm_plane *plane, u64 supported_tfs, uint32_t flags)
{
#if defined(NV_DRM_COLOROP_HAS_FUNCS)
    return drm_plane_colorop_curve_1d_init(dev, colorop, plane,
                                           &nv_drm_plane_colorop_funcs,
                                           supported_tfs, flags);
#else
    return drm_plane_colorop_curve_1d_init(dev, colorop, plane,
                                           supported_tfs, flags);
#endif
}

static int nv_drm_plane_colorop_ctm_3x4_init(
    struct drm_device *dev, struct drm_colorop *colorop,
    struct drm_plane *plane, uint32_t flags)
{
#if defined(NV_DRM_COLOROP_HAS_FUNCS)
    return drm_plane_colorop_ctm_3x4_init(dev, colorop, plane,
                                          &nv_drm_plane_colorop_funcs,
                                          flags);
#else
    return drm_plane_colorop_ctm_3x4_init(dev, colorop, plane,
                                          flags);
#endif
}

static int nv_drm_plane_colorop_mult_init(
    struct drm_device *dev, struct drm_colorop *colorop,
    struct drm_plane *plane, uint32_t flags)
{
#if defined(NV_DRM_COLOROP_HAS_FUNCS)
    return drm_plane_colorop_mult_init(dev, colorop, plane,
                                       &nv_drm_plane_colorop_funcs,
                                       flags);
#else
    return drm_plane_colorop_mult_init(dev, colorop, plane,
                                       flags);
#endif
}

struct nv_drm_colorop {
    /** @colorop: The DRM colorop */
    struct drm_colorop colorop;

    /** @pipeline: Back-pointer to the pipeline this colorop belongs to */
    struct nv_drm_color_pipeline *pipeline;
};

/**
 * struct nv_drm_color_pipeline - NVIDIA DRM color pipeline
 *
 * Represents a complete color pipeline for a plane, consisting of
 * chained colorops that map to NVKMS color operations.
 */
struct nv_drm_color_pipeline {
    /** @name: Pipeline name exposed to userspace */
    const char *name;

    /** @first: First colorop; its ID names the pipeline in COLOR_PIPELINE */
    struct nv_drm_colorop *first;

    /** @fmt_ctm: 3x4 matrix colorop for FMT (YUV to RGB) conversion (FMT) */
    struct nv_drm_colorop *fmt_ctm;

    /** @degamma_tf: 1D curve colorop for degamma transfer function (ILUT) */
    struct nv_drm_colorop *degamma_tf;

    /** @degamma_lut: 1D LUT colorop for custom degamma LUT (ILUT) */
    struct nv_drm_colorop *degamma_lut;

    /** @degamma_multiplier: Multiplier colorop for degamma gain (ILUT) */
    struct nv_drm_colorop *degamma_multiplier;

    /** @lms_ctm: 3x4 matrix colorop for LMS color transformation (CSC00) */
    struct nv_drm_colorop *lms_ctm;

    /** @pq_inv_eotf: Non-bypassable 1D curve for PQ inverse EOTF (linear to PQ, CSC0LUT) */
    struct nv_drm_colorop *pq_inv_eotf;

    /** @lms_to_itp_ctm: 3x4 matrix colorop for LMS to ICtCp conversion (CSC01) */
    struct nv_drm_colorop *lms_to_itp_ctm;

    /** @tmo_lut: 1D single-channel tone mapping LUT colorop - requires R=G=B, but only G is used (TMO) */
    struct nv_drm_colorop *tmo_lut;

    /** @pq_eotf: Non-bypassable 1D curve for PQ EOTF (PQ to linear, CSC1LUT) */
    struct nv_drm_colorop *pq_eotf;

    /** @itp_to_lms_ctm: 3x4 matrix colorop for ICtCp to LMS conversion (CSC10) */
    struct nv_drm_colorop *itp_to_lms_ctm;

    /** @blend_ctm: 3x4 matrix colorop for final blending transformation (CSC11) */
    struct nv_drm_colorop *blend_ctm;
};

static void nv_drm_colorop_destroy(struct drm_colorop *colorop)
{
    struct nv_drm_colorop *nv_colorop =
        container_of(colorop, struct nv_drm_colorop, colorop);
    struct nv_drm_color_pipeline *pipeline = nv_colorop->pipeline;

    /*
     * Set pipeline->field to NULL before freeing so that
     * plane_destroy_pipeline_colorops() does not double-free. Commit
     * fa15259eb659 ("drm: Clean up colorop objects during mode_config
     * cleanup", expected in v7.1) added a colorop destroy loop to
     * drm_mode_config_cleanup() that runs before plane destroy, calling this
     * function for each colorop. By the time plane_destroy_pipeline_colorops()
     * runs, pipeline->field is already NULL and the DESTROY_COLOROP call is a
     * no-op.
     */
#define CLEAR_IF_MATCH(field) \
    if (pipeline->field == nv_colorop) { pipeline->field = NULL; }

    CLEAR_IF_MATCH(fmt_ctm)
    CLEAR_IF_MATCH(degamma_tf)
    CLEAR_IF_MATCH(degamma_lut)
    CLEAR_IF_MATCH(degamma_multiplier)
    CLEAR_IF_MATCH(lms_ctm)
    CLEAR_IF_MATCH(pq_inv_eotf)
    CLEAR_IF_MATCH(lms_to_itp_ctm)
    CLEAR_IF_MATCH(tmo_lut)
    CLEAR_IF_MATCH(itp_to_lms_ctm)
    CLEAR_IF_MATCH(pq_eotf)
    CLEAR_IF_MATCH(blend_ctm)

#undef CLEAR_IF_MATCH

    drm_colorop_cleanup(colorop);
    nv_drm_free(nv_colorop);
}

#if defined(NV_DRM_COLOROP_HAS_FUNCS)
static const struct drm_colorop_funcs nv_drm_plane_colorop_funcs = {
    .destroy = nv_drm_colorop_destroy,
};
#endif

/**
 * nv_drm_plane_create_color_pipelines - Create color pipelines for a plane
 * @plane: DRM plane to create pipelines for
 * @supportsICtCp: Whether the plane supports ICtCp color space
 *
 * Returns:
 * 0 on success, negative error code on failure.
 */
int nv_drm_plane_create_color_pipelines(struct drm_plane *plane,
                                        NvBool supportsICtCp);

/**
 * nv_drm_plane_destroy_color_pipelines - Destroy plane's color pipelines
 * @plane: DRM plane whose pipelines to destroy
 */
void nv_drm_plane_destroy_color_pipelines(struct drm_plane *plane);

/**
 * nv_drm_plane_create_color_pipeline_property - Create COLOR_PIPELINE property
 * @plane: DRM plane to create property for
 *
 * Creates the COLOR_PIPELINE property on the plane, advertising the available
 * color pipelines.
 *
 * Returns:
 * 0 on success, negative error code on failure.
 */
int nv_drm_plane_create_color_pipeline_property(struct drm_plane *plane);

/**
 * nv_drm_plane_process_color_mgmt_state - Process plane color management state
 * @plane: DRM plane
 * @plane_state: DRM plane state
 *
 * Handles color management state for a plane, enforcing mutual exclusion between
 * vendor properties and the color pipeline API:
 *
 * 1. If DRM_CLIENT_CAP_PLANE_COLOR_PIPELINE is set but COLOR_PIPELINE=0:
 *    Clear all vendor color properties (cap requires ignoring them)
 * 2. If cap is not set but COLOR_PIPELINE is set:
 *    Clear DRM core's COLOR_PIPELINE and all colorop fields (stale from duplicate_state)
 * 3. If cap is set AND COLOR_PIPELINE is active:
 *    Translate colorop state to nv_plane_state fields
 *
 * Ensures only one color management API is active and prevents stale state.
 *
 * Reads colorop state from the atomic state and populates the corresponding
 * fields in nv_drm_plane_state. This allows the existing plane_req_config_update()
 * code to handle colorop values the same way it handles NV_PLANE_* properties.
 *
 * Returns:
 * 0 on success, negative error code on failure.
 */
int nv_drm_plane_process_color_mgmt_state(struct drm_plane *plane,
                                          struct drm_plane_state *plane_state);

#endif /* NV_DRM_AVAILABLE && NV_DRM_HAS_COLOROP */

#endif /* __NVIDIA_DRM_COLOR_PIPELINE_H__ */

