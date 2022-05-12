/* SPDX-License-Identifier: MIT */
/*
 * Copyright © 2020 Intel Corporation
 */
#ifndef INTEL_SCALER_H
#define INTEL_SCALER_H

#include <linux/types.h>

enum drm_scaling_filter;
struct drm_i915_private;
struct intel_crtc_state;
struct intel_plane_state;
struct intel_plane;
enum pipe;

int skl_update_scaler_crtc(struct intel_crtc_state *crtc_state);

int skl_update_scaler_plane(struct intel_crtc_state *crtc_state,
			    struct intel_plane_state *plane_state);

void skl_pfit_enable(const struct intel_crtc_state *crtc_state);

void skl_program_plane_scaler(struct intel_plane *plane,
			      const struct intel_crtc_state *crtc_state,
			      const struct intel_plane_state *plane_state);
void skl_detach_scalers(const struct intel_crtc_state *crtc_state);
void skl_scaler_disable(const struct intel_crtc_state *old_crtc_state);
#endif
