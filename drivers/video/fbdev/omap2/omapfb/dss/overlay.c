// SPDX-License-Identifier: GPL-2.0-only
/*
 * linux/drivers/video/omap2/dss/overlay.c
 *
 * Copyright (C) 2009 Nokia Corporation
 * Author: Tomi Valkeinen <tomi.valkeinen@nokia.com>
 *
 * Some code and ideas taken from drivers/video/omap/ driver
 * by Imre Deak.
 */

#define DSS_SUBSYS_NAME "OVERLAY"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/sysfs.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/slab.h>

#include <video/omapfb_dss.h>

#include "dss.h"
#include "dss_features.h"

static int num_overlays;
static struct omap_overlay *overlays;

int omap_dss_get_num_overlays(void)
{
	return num_overlays;
}
EXPORT_SYMBOL(omap_dss_get_num_overlays);

struct omap_overlay *omap_dss_get_overlay(int num)
{
	if (num >= num_overlays)
		return NULL;

	return &overlays[num];
}
EXPORT_SYMBOL(omap_dss_get_overlay);

void dss_init_overlays(struct platform_device *pdev)
{
	int i, r;

	num_overlays = dss_feat_get_num_ovls();

	overlays = kcalloc(num_overlays, sizeof(struct omap_overlay),
			   GFP_KERNEL);

	BUG_ON(overlays == NULL);

	for (i = 0; i < num_overlays; ++i) {
		struct omap_overlay *ovl = &overlays[i];

		switch (i) {
		case 0:
			ovl->name = "gfx";
			ovl->id = OMAP_DSS_GFX;
			break;
		case 1:
			ovl->name = "vid1";
			ovl->id = OMAP_DSS_VIDEO1;
			break;
		case 2:
			ovl->name = "vid2";
			ovl->id = OMAP_DSS_VIDEO2;
			break;
		case 3:
			ovl->name = "vid3";
			ovl->id = OMAP_DSS_VIDEO3;
			break;
		}

		ovl->caps = dss_feat_get_overlay_caps(ovl->id);
		ovl->supported_modes =
			dss_feat_get_supported_color_modes(ovl->id);

		r = dss_overlay_kobj_init(ovl, pdev);
		if (r)
			DSSERR("failed to create sysfs file\n");
	}
}

void dss_uninit_overlays(struct platform_device *pdev)
{
	int i;

	for (i = 0; i < num_overlays; ++i) {
		struct omap_overlay *ovl = &overlays[i];
		dss_overlay_kobj_uninit(ovl);
	}

	kfree(overlays);
	overlays = NULL;
	num_overlays = 0;
}

int dss_ovl_simple_check(struct omap_overlay *ovl,
		const struct omap_overlay_info *info)
{
	if ((ovl->caps & OMAP_DSS_OVL_CAP_SCALE) == 0) {
		if (info->out_width != 0 && info->width != info->out_width) {
			DSSERR("check_overlay: overlay %d doesn't support "
					"scaling\n", ovl->id);
			return -EINVAL;
		}

		if (info->out_height != 0 && info->height != info->out_height) {
			DSSERR("check_overlay: overlay %d doesn't support "
					"scaling\n", ovl->id);
			return -EINVAL;
		}
	}

	if ((ovl->supported_modes & info->color_mode) == 0) {
		DSSERR("check_overlay: overlay %d doesn't support mode %d\n",
				ovl->id, info->color_mode);
		return -EINVAL;
	}

	if (info->zorder >= omap_dss_get_num_overlays()) {
		DSSERR("check_overlay: zorder %d too high\n", info->zorder);
		return -EINVAL;
	}

	if (dss_feat_rotation_type_supported(info->rotation_type) == 0) {
		DSSERR("check_overlay: rotation type %d not supported\n",
				info->rotation_type);
		return -EINVAL;
	}

	return 0;
}

int dss_ovl_check(struct omap_overlay *ovl, struct omap_overlay_info *info,
		const struct omap_video_timings *mgr_timings)
{
	u16 outw, outh;
	u16 dw, dh;

	dw = mgr_timings->x_res;
	dh = mgr_timings->y_res;

	if ((ovl->caps & OMAP_DSS_OVL_CAP_SCALE) == 0) {
		outw = info->width;
		outh = info->height;
	} else {
		if (info->out_width == 0)
			outw = info->width;
		else
			outw = info->out_width;

		if (info->out_height == 0)
			outh = info->height;
		else
			outh = info->out_height;
	}

	if (dw < info->pos_x + outw) {
		DSSERR("overlay %d horizontally not inside the display area "
				"(%d + %d >= %d)\n",
				ovl->id, info->pos_x, outw, dw);
		return -EINVAL;
	}

	if (dh < info->pos_y + outh) {
		DSSERR("overlay %d vertically not inside the display area "
				"(%d + %d >= %d)\n",
				ovl->id, info->pos_y, outh, dh);
		return -EINVAL;
	}

	return 0;
}

/*
 * Checks if replication logic should be used. Only use when overlay is in
 * RGB12U or RGB16 mode, and video port width interface is 18bpp or 24bpp
 */
bool dss_ovl_use_replication(struct dss_lcd_mgr_config config,
		enum omap_color_mode mode)
{
	if (mode != OMAP_DSS_COLOR_RGB12U && mode != OMAP_DSS_COLOR_RGB16)
		return false;

	return config.video_port_width > 16;
}
