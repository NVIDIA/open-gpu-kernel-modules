// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2019 Intel Corporation.
 *
 * Authors:
 * Ramalingam C <ramalingam.c@intel.com>
 */
#include <kcl/kcl_drm_hdcp.h>

#ifndef HAVE_DRM_HDCP_UPDATE_CONTENT_PROTECTION
/* Copied from v5.3-rc1-380-gbb5a45d40d50 drivers/gpu/drm/drm_hdcp.c */
void _kcl_drm_hdcp_update_content_protection(struct drm_connector *connector,
							      u64 val)
{
	struct drm_device *dev = connector->dev;
	struct drm_connector_state *state = connector->state;

	WARN_ON(!drm_modeset_is_locked(&dev->mode_config.connection_mutex));
	if (state->content_protection == val)
		return;

	state->content_protection = val;
}
EXPORT_SYMBOL(_kcl_drm_hdcp_update_content_protection);
#endif
