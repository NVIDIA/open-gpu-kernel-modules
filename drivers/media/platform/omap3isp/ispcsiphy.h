/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * ispcsiphy.h
 *
 * TI OMAP3 ISP - CSI PHY module
 *
 * Copyright (C) 2010 Nokia Corporation
 * Copyright (C) 2009 Texas Instruments, Inc.
 *
 * Contacts: Laurent Pinchart <laurent.pinchart@ideasonboard.com>
 *	     Sakari Ailus <sakari.ailus@iki.fi>
 */

#ifndef OMAP3_ISP_CSI_PHY_H
#define OMAP3_ISP_CSI_PHY_H

#include "omap3isp.h"

struct isp_csi2_device;
struct regulator;

struct isp_csiphy {
	struct isp_device *isp;
	struct mutex mutex;	/* serialize csiphy configuration */
	struct isp_csi2_device *csi2;
	struct regulator *vdd;
	/* the entity that acquired the phy */
	struct media_entity *entity;

	/* mem resources - enums as defined in enum isp_mem_resources */
	unsigned int cfg_regs;
	unsigned int phy_regs;

	u8 num_data_lanes;	/* number of CSI2 Data Lanes supported */
};

int omap3isp_csiphy_acquire(struct isp_csiphy *phy,
			    struct media_entity *entity);
void omap3isp_csiphy_release(struct isp_csiphy *phy);
int omap3isp_csiphy_init(struct isp_device *isp);
void omap3isp_csiphy_cleanup(struct isp_device *isp);

#endif	/* OMAP3_ISP_CSI_PHY_H */
