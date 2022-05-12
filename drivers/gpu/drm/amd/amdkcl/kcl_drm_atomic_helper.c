/*
 * Copyright (C) 2018 Intel Corp.
 * Copyright (C) 2014 Red Hat
 * Copyright (C) 2014 Intel Corp.
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
#include <kcl/kcl_drm_atomic_helper.h>
#include <kcl/kcl_drm_crtc.h>
#include <drm/drm_vblank.h>

#ifdef AMDKCL__DRM_ATOMIC_HELPER_PLANE_RESET
/* Copied from drivers/gpu/drm/drm_atomic_state_helper.c and modified for KCL */
void _kcl__drm_atomic_helper_plane_reset(struct drm_plane *plane,
							struct drm_plane_state *state)
{
	state->plane = plane;
	state->rotation = DRM_MODE_ROTATE_0;

#ifdef DRM_BLEND_ALPHA_OPAQUE
	state->alpha = DRM_BLEND_ALPHA_OPAQUE;
#endif
#ifdef DRM_MODE_BLEND_PREMULTI
	state->pixel_blend_mode = DRM_MODE_BLEND_PREMULTI;
#endif

	plane->state = state;
}
EXPORT_SYMBOL(_kcl__drm_atomic_helper_plane_reset);
#endif

#ifndef HAVE___DRM_ATOMIC_HELPER_CRTC_RESET
/* Copied from drivers/gpu/drm/drm_atomic_state_helper.c */
void
__drm_atomic_helper_crtc_reset(struct drm_crtc *crtc,
                              struct drm_crtc_state *crtc_state)
{
       if (crtc_state)
               crtc_state->crtc = crtc;

       crtc->state = crtc_state;
}
EXPORT_SYMBOL(__drm_atomic_helper_crtc_reset);
#endif

#ifndef HAVE_DRM_ATOMIC_HELPER_CALC_TIMESTAMPING_CONSTANTS
/*
 * This implementation is duplicated from v5.9-rc5-1595-ge1ad957d45f7
 * "Extract drm_atomic_helper_calc_timestamping_constants()"
 *
 */
void drm_atomic_helper_calc_timestamping_constants(struct drm_atomic_state *state)
{
	struct drm_crtc_state *new_crtc_state;
	struct drm_crtc *crtc;

#if !defined(for_each_new_crtc_in_state)
	struct drm_device *dev = state->dev;
	list_for_each_entry(crtc, &dev->mode_config.crtc_list, head) {
		new_crtc_state = crtc->state;
#else
	int i;
	for_each_new_crtc_in_state(state, crtc, new_crtc_state, i) {
#endif
		if (new_crtc_state->enable)
			drm_calc_timestamping_constants(crtc,
							&new_crtc_state->adjusted_mode);
	}
}
EXPORT_SYMBOL(drm_atomic_helper_calc_timestamping_constants);
#endif
