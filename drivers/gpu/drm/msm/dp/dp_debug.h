/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2017-2020, The Linux Foundation. All rights reserved.
 */

#ifndef _DP_DEBUG_H_
#define _DP_DEBUG_H_

#include "dp_panel.h"
#include "dp_link.h"

/**
 * struct dp_debug
 * @debug_en: specifies whether debug mode enabled
 * @vdisplay: used to filter out vdisplay value
 * @hdisplay: used to filter out hdisplay value
 * @vrefresh: used to filter out vrefresh value
 * @tpg_state: specifies whether tpg feature is enabled
 */
struct dp_debug {
	bool debug_en;
	int aspect_ratio;
	int vdisplay;
	int hdisplay;
	int vrefresh;
};

#if defined(CONFIG_DEBUG_FS)

/**
 * dp_debug_get() - configure and get the DisplayPlot debug module data
 *
 * @dev: device instance of the caller
 * @panel: instance of panel module
 * @usbpd: instance of usbpd module
 * @link: instance of link module
 * @connector: double pointer to display connector
 * @minor: pointer to drm minor number after device registration
 * return: pointer to allocated debug module data
 *
 * This function sets up the debug module and provides a way
 * for debugfs input to be communicated with existing modules
 */
struct dp_debug *dp_debug_get(struct device *dev, struct dp_panel *panel,
		struct dp_usbpd *usbpd, struct dp_link *link,
		struct drm_connector **connector,
		struct drm_minor *minor);

/**
 * dp_debug_put()
 *
 * Cleans up dp_debug instance
 *
 * @dp_debug: instance of dp_debug
 */
void dp_debug_put(struct dp_debug *dp_debug);

#else

static inline
struct dp_debug *dp_debug_get(struct device *dev, struct dp_panel *panel,
		struct dp_usbpd *usbpd, struct dp_link *link,
		struct drm_connector **connector, struct drm_minor *minor)
{
	return ERR_PTR(-EINVAL);
}

static inline void dp_debug_put(struct dp_debug *dp_debug)
{
}

#endif /* defined(CONFIG_DEBUG_FS) */

#endif /* _DP_DEBUG_H_ */
