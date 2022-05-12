/*
 * Copyright 2012-15 Advanced Micro Devices, Inc.
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
 * Authors: AMD
 *
 */

#ifndef __DAL_AMDGPU_DM_MST_TYPES_H__
#define __DAL_AMDGPU_DM_MST_TYPES_H__

#define DP_BRANCH_DEVICE_ID_90CC24 0x90CC24

#define SYNAPTICS_RC_COMMAND       0x4B2
#define SYNAPTICS_RC_RESULT        0x4B3
#define SYNAPTICS_RC_LENGTH        0x4B8
#define SYNAPTICS_RC_OFFSET        0x4BC
#define SYNAPTICS_RC_DATA          0x4C0

struct amdgpu_display_manager;
struct amdgpu_dm_connector;

int dm_mst_get_pbn_divider(struct dc_link *link);

void amdgpu_dm_initialize_dp_connector(struct amdgpu_display_manager *dm,
				       struct amdgpu_dm_connector *aconnector,
				       int link_index);

void
dm_dp_create_fake_mst_encoders(struct amdgpu_device *adev);

#ifdef CONFIG_DRM_AMD_DC_DSC_SUPPORT
#if defined(CONFIG_DRM_AMD_DC_DCN1_0)

struct dsc_mst_fairness_vars {
	int pbn;
	bool dsc_enabled;
	int bpp_x16;
	struct amdgpu_dm_connector *aconnector;
};

bool compute_mst_dsc_configs_for_state(struct drm_atomic_state *state,
				       struct dc_state *dc_state,
				       struct dsc_mst_fairness_vars *vars);

bool needs_dsc_aux_workaround(struct dc_link *link);

void pre_validate_dsc(struct drm_atomic_state *state,
		      struct dm_atomic_state **dm_state_ptr,
		      struct dsc_mst_fairness_vars *vars);
#endif
#endif

#endif
