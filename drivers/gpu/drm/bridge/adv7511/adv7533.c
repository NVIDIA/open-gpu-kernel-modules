// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2016, The Linux Foundation. All rights reserved.
 */

#include <linux/of_graph.h>

#include "adv7511.h"

static const struct reg_sequence adv7533_fixed_registers[] = {
	{ 0x16, 0x20 },
	{ 0x9a, 0xe0 },
	{ 0xba, 0x70 },
	{ 0xde, 0x82 },
	{ 0xe4, 0x40 },
	{ 0xe5, 0x80 },
};

static const struct reg_sequence adv7533_cec_fixed_registers[] = {
	{ 0x15, 0xd0 },
	{ 0x17, 0xd0 },
	{ 0x24, 0x20 },
	{ 0x57, 0x11 },
	{ 0x05, 0xc8 },
};

static void adv7511_dsi_config_timing_gen(struct adv7511 *adv)
{
	struct mipi_dsi_device *dsi = adv->dsi;
	struct drm_display_mode *mode = &adv->curr_mode;
	unsigned int hsw, hfp, hbp, vsw, vfp, vbp;
	u8 clock_div_by_lanes[] = { 6, 4, 3 };	/* 2, 3, 4 lanes */

	hsw = mode->hsync_end - mode->hsync_start;
	hfp = mode->hsync_start - mode->hdisplay;
	hbp = mode->htotal - mode->hsync_end;
	vsw = mode->vsync_end - mode->vsync_start;
	vfp = mode->vsync_start - mode->vdisplay;
	vbp = mode->vtotal - mode->vsync_end;

	/* set pixel clock divider mode */
	regmap_write(adv->regmap_cec, 0x16,
		     clock_div_by_lanes[dsi->lanes - 2] << 3);

	/* horizontal porch params */
	regmap_write(adv->regmap_cec, 0x28, mode->htotal >> 4);
	regmap_write(adv->regmap_cec, 0x29, (mode->htotal << 4) & 0xff);
	regmap_write(adv->regmap_cec, 0x2a, hsw >> 4);
	regmap_write(adv->regmap_cec, 0x2b, (hsw << 4) & 0xff);
	regmap_write(adv->regmap_cec, 0x2c, hfp >> 4);
	regmap_write(adv->regmap_cec, 0x2d, (hfp << 4) & 0xff);
	regmap_write(adv->regmap_cec, 0x2e, hbp >> 4);
	regmap_write(adv->regmap_cec, 0x2f, (hbp << 4) & 0xff);

	/* vertical porch params */
	regmap_write(adv->regmap_cec, 0x30, mode->vtotal >> 4);
	regmap_write(adv->regmap_cec, 0x31, (mode->vtotal << 4) & 0xff);
	regmap_write(adv->regmap_cec, 0x32, vsw >> 4);
	regmap_write(adv->regmap_cec, 0x33, (vsw << 4) & 0xff);
	regmap_write(adv->regmap_cec, 0x34, vfp >> 4);
	regmap_write(adv->regmap_cec, 0x35, (vfp << 4) & 0xff);
	regmap_write(adv->regmap_cec, 0x36, vbp >> 4);
	regmap_write(adv->regmap_cec, 0x37, (vbp << 4) & 0xff);
}

void adv7533_dsi_power_on(struct adv7511 *adv)
{
	struct mipi_dsi_device *dsi = adv->dsi;

	if (adv->use_timing_gen)
		adv7511_dsi_config_timing_gen(adv);

	/* set number of dsi lanes */
	regmap_write(adv->regmap_cec, 0x1c, dsi->lanes << 4);

	if (adv->use_timing_gen) {
		/* reset internal timing generator */
		regmap_write(adv->regmap_cec, 0x27, 0xcb);
		regmap_write(adv->regmap_cec, 0x27, 0x8b);
		regmap_write(adv->regmap_cec, 0x27, 0xcb);
	} else {
		/* disable internal timing generator */
		regmap_write(adv->regmap_cec, 0x27, 0x0b);
	}

	/* enable hdmi */
	regmap_write(adv->regmap_cec, 0x03, 0x89);
	/* disable test mode */
	regmap_write(adv->regmap_cec, 0x55, 0x00);

	regmap_register_patch(adv->regmap_cec, adv7533_cec_fixed_registers,
			      ARRAY_SIZE(adv7533_cec_fixed_registers));
}

void adv7533_dsi_power_off(struct adv7511 *adv)
{
	/* disable hdmi */
	regmap_write(adv->regmap_cec, 0x03, 0x0b);
	/* disable internal timing generator */
	regmap_write(adv->regmap_cec, 0x27, 0x0b);
}

void adv7533_mode_set(struct adv7511 *adv, const struct drm_display_mode *mode)
{
	struct mipi_dsi_device *dsi = adv->dsi;
	int lanes, ret;

	if (adv->num_dsi_lanes != 4)
		return;

	if (mode->clock > 80000)
		lanes = 4;
	else
		lanes = 3;

	if (lanes != dsi->lanes) {
		mipi_dsi_detach(dsi);
		dsi->lanes = lanes;
		ret = mipi_dsi_attach(dsi);
		if (ret)
			dev_err(&dsi->dev, "failed to change host lanes\n");
	}
}

int adv7533_patch_registers(struct adv7511 *adv)
{
	return regmap_register_patch(adv->regmap,
				     adv7533_fixed_registers,
				     ARRAY_SIZE(adv7533_fixed_registers));
}

int adv7533_patch_cec_registers(struct adv7511 *adv)
{
	return regmap_register_patch(adv->regmap_cec,
				    adv7533_cec_fixed_registers,
				    ARRAY_SIZE(adv7533_cec_fixed_registers));
}

int adv7533_attach_dsi(struct adv7511 *adv)
{
	struct device *dev = &adv->i2c_main->dev;
	struct mipi_dsi_host *host;
	struct mipi_dsi_device *dsi;
	int ret = 0;
	const struct mipi_dsi_device_info info = { .type = "adv7533",
						   .channel = 0,
						   .node = NULL,
						 };

	host = of_find_mipi_dsi_host_by_node(adv->host_node);
	if (!host) {
		dev_err(dev, "failed to find dsi host\n");
		return -EPROBE_DEFER;
	}

	dsi = mipi_dsi_device_register_full(host, &info);
	if (IS_ERR(dsi)) {
		dev_err(dev, "failed to create dsi device\n");
		ret = PTR_ERR(dsi);
		goto err_dsi_device;
	}

	adv->dsi = dsi;

	dsi->lanes = adv->num_dsi_lanes;
	dsi->format = MIPI_DSI_FMT_RGB888;
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_SYNC_PULSE |
			  MIPI_DSI_MODE_EOT_PACKET | MIPI_DSI_MODE_VIDEO_HSE;

	ret = mipi_dsi_attach(dsi);
	if (ret < 0) {
		dev_err(dev, "failed to attach dsi to host\n");
		goto err_dsi_attach;
	}

	return 0;

err_dsi_attach:
	mipi_dsi_device_unregister(dsi);
err_dsi_device:
	return ret;
}

void adv7533_detach_dsi(struct adv7511 *adv)
{
	mipi_dsi_detach(adv->dsi);
	mipi_dsi_device_unregister(adv->dsi);
}

int adv7533_parse_dt(struct device_node *np, struct adv7511 *adv)
{
	u32 num_lanes;

	of_property_read_u32(np, "adi,dsi-lanes", &num_lanes);

	if (num_lanes < 1 || num_lanes > 4)
		return -EINVAL;

	adv->num_dsi_lanes = num_lanes;

	adv->host_node = of_graph_get_remote_node(np, 0, 0);
	if (!adv->host_node)
		return -ENODEV;

	of_node_put(adv->host_node);

	adv->use_timing_gen = !of_property_read_bool(np,
						"adi,disable-timing-generator");

	/* TODO: Check if these need to be parsed by DT or not */
	adv->rgb = true;
	adv->embedded_sync = false;

	return 0;
}
