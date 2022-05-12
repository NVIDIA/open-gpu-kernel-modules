/*
 * Copyright (C) 2014 Red Hat
 * Copyright (C) 2014 Intel Corp.
 * Copyright (C) 2018 Intel Corp.
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
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Authors:
 * Rob Clark <robdclark@gmail.com>
 * Daniel Vetter <daniel.vetter@ffwll.ch>
 */
#ifndef AMDKCL_DRM_ATOMIC_HELPER_H
#define AMDKCL_DRM_ATOMIC_HELPER_H

#include <drm/drm_atomic_helper.h>
#include <drm/drm_atomic.h>
#include <drm/drm_plane_helper.h>
#include <kcl/kcl_drm_modes.h>
#include <kcl/kcl_drm_crtc.h>

static inline struct drm_crtc_state *
kcl_drm_atomic_get_old_crtc_state_before_commit(struct drm_atomic_state *state,
					    struct drm_crtc *crtc)
{
#if defined(HAVE_DRM_ATOMIC_GET_CRTC_STATE)
	return drm_atomic_get_old_crtc_state(state, crtc);
#elif defined(HAVE_DRM_CRTCS_STATE_MEMBER)
	return state->crtcs[drm_crtc_index(crtc)].ptr->state;
#else
	return state->crtcs[drm_crtc_index(crtc)]->state;
#endif
}

static inline struct drm_crtc_state *
kcl_drm_atomic_get_old_crtc_state_after_commit(struct drm_atomic_state *state,
				  struct drm_crtc *crtc)
{
#if defined(HAVE_DRM_ATOMIC_GET_CRTC_STATE)
	return drm_atomic_get_old_crtc_state(state, crtc);
#else
	return drm_atomic_get_existing_crtc_state(state, crtc);
#endif
}

static inline struct drm_crtc_state *
kcl_drm_atomic_get_new_crtc_state_before_commit(struct drm_atomic_state *state,
				  struct drm_crtc *crtc)
{
#if defined(HAVE_DRM_ATOMIC_GET_CRTC_STATE)
	return drm_atomic_get_new_crtc_state(state,crtc);
#else
	return drm_atomic_get_existing_crtc_state(state, crtc);
#endif
}

static inline struct drm_crtc_state *
kcl_drm_atomic_get_new_crtc_state_after_commit(struct drm_atomic_state *state,
					    struct drm_crtc *crtc)
{
#if defined(HAVE_DRM_ATOMIC_GET_CRTC_STATE)
	return drm_atomic_get_new_crtc_state(state,crtc);
#elif defined(HAVE_DRM_CRTCS_STATE_MEMBER)
	return state->crtcs[drm_crtc_index(crtc)].ptr->state;
#else
	return state->crtcs[drm_crtc_index(crtc)]->state;
#endif
}

static inline struct drm_plane_state *
kcl_drm_atomic_get_new_plane_state_before_commit(struct drm_atomic_state *state,
							struct drm_plane *plane)
{
#if defined(HAVE_DRM_ATOMIC_GET_NEW_PLANE_STATE)
	return drm_atomic_get_new_plane_state(state, plane);
#else
	return drm_atomic_get_existing_plane_state(state, plane);
#endif
}

#ifndef HAVE_DRM_ATOMIC_HELPER_CHECK_PLANE_STATE
static inline int
drm_atomic_helper_check_plane_state(struct drm_plane_state *plane_state,
					const struct drm_crtc_state *crtc_state,
					int min_scale,
					int max_scale,
					bool can_position,
					bool can_update_disabled)
{
#ifdef HAVE_DRM_PLANE_HELPER_CHECK_STATE
	struct drm_rect clip = {};
	struct drm_crtc *crtc = crtc_state->crtc;
	if (crtc->enabled)
		drm_mode_get_hv_timing(&crtc->mode, &clip.x2, &clip.y2);
	return drm_plane_helper_check_state(plane_state, &clip, min_scale,
				max_scale, can_position, can_update_disabled);
#else
	pr_warn_once("%s is not supported\n", __func__);
	return 0;
#endif
}
#endif

/*
 * v4.19-rc1-206-ge267364a6e1b
 * drm/atomic: Initialise planes with opaque alpha values
 */
#if DRM_VERSION_CODE < DRM_VERSION(4, 20, 0)
#define AMDKCL__DRM_ATOMIC_HELPER_PLANE_RESET
void _kcl__drm_atomic_helper_plane_reset(struct drm_plane *plane,
							struct drm_plane_state *state);
#endif

#ifndef HAVE___DRM_ATOMIC_HELPER_CRTC_RESET
void __drm_atomic_helper_crtc_reset(struct drm_crtc *crtc,
                              struct drm_crtc_state *crtc_state);
#endif

#ifndef HAVE_DRM_ATOMIC_HELPER_CALC_TIMESTAMPING_CONSTANTS
void drm_atomic_helper_calc_timestamping_constants(struct drm_atomic_state *state);
#endif

#endif
