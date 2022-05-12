/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2012-2020, The Linux Foundation. All rights reserved.
 */

#ifndef _DP_CTRL_H_
#define _DP_CTRL_H_

#include "dp_aux.h"
#include "dp_panel.h"
#include "dp_link.h"
#include "dp_parser.h"
#include "dp_power.h"
#include "dp_catalog.h"

struct dp_ctrl {
	bool orientation;
	atomic_t aborted;
	u32 pixel_rate;
};

int dp_ctrl_host_init(struct dp_ctrl *dp_ctrl, bool flip, bool reset);
void dp_ctrl_host_deinit(struct dp_ctrl *dp_ctrl);
int dp_ctrl_on_link(struct dp_ctrl *dp_ctrl);
int dp_ctrl_on_stream(struct dp_ctrl *dp_ctrl);
int dp_ctrl_off_link_stream(struct dp_ctrl *dp_ctrl);
void dp_ctrl_off_phy(struct dp_ctrl *dp_ctrl);
int dp_ctrl_off(struct dp_ctrl *dp_ctrl);
void dp_ctrl_push_idle(struct dp_ctrl *dp_ctrl);
void dp_ctrl_isr(struct dp_ctrl *dp_ctrl);
void dp_ctrl_handle_sink_request(struct dp_ctrl *dp_ctrl);
struct dp_ctrl *dp_ctrl_get(struct device *dev, struct dp_link *link,
			struct dp_panel *panel,	struct drm_dp_aux *aux,
			struct dp_power *power, struct dp_catalog *catalog,
			struct dp_parser *parser);

#endif /* _DP_CTRL_H_ */
