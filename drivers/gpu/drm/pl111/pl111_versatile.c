// SPDX-License-Identifier: GPL-2.0-only

/*
 * Versatile family (ARM reference designs) handling for the PL11x.
 * This is based on code and know-how in the previous frame buffer
 * driver in drivers/video/fbdev/amba-clcd.c:
 * Copyright (C) 2001 ARM Limited, by David A Rusling
 * Updated to 2.5 by Deep Blue Solutions Ltd.
 * Major contributions and discoveries by Russell King.
 */

#include <linux/bitops.h>
#include <linux/device.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/regmap.h>
#include <linux/vexpress.h>

#include "pl111_versatile.h"
#include "pl111_drm.h"

static struct regmap *versatile_syscon_map;

/*
 * We detect the different syscon types from the compatible strings.
 */
enum versatile_clcd {
	INTEGRATOR_IMPD1,
	INTEGRATOR_CLCD_CM,
	VERSATILE_CLCD,
	REALVIEW_CLCD_EB,
	REALVIEW_CLCD_PB1176,
	REALVIEW_CLCD_PB11MP,
	REALVIEW_CLCD_PBA8,
	REALVIEW_CLCD_PBX,
	VEXPRESS_CLCD_V2M,
};

static const struct of_device_id versatile_clcd_of_match[] = {
	{
		.compatible = "arm,core-module-integrator",
		.data = (void *)INTEGRATOR_CLCD_CM,
	},
	{
		.compatible = "arm,versatile-sysreg",
		.data = (void *)VERSATILE_CLCD,
	},
	{
		.compatible = "arm,realview-eb-syscon",
		.data = (void *)REALVIEW_CLCD_EB,
	},
	{
		.compatible = "arm,realview-pb1176-syscon",
		.data = (void *)REALVIEW_CLCD_PB1176,
	},
	{
		.compatible = "arm,realview-pb11mp-syscon",
		.data = (void *)REALVIEW_CLCD_PB11MP,
	},
	{
		.compatible = "arm,realview-pba8-syscon",
		.data = (void *)REALVIEW_CLCD_PBA8,
	},
	{
		.compatible = "arm,realview-pbx-syscon",
		.data = (void *)REALVIEW_CLCD_PBX,
	},
	{
		.compatible = "arm,vexpress-muxfpga",
		.data = (void *)VEXPRESS_CLCD_V2M,
	},
	{},
};

static const struct of_device_id impd1_clcd_of_match[] = {
	{
		.compatible = "arm,im-pd1-syscon",
		.data = (void *)INTEGRATOR_IMPD1,
	},
	{},
};

/*
 * Core module CLCD control on the Integrator/CP, bits
 * 8 thru 19 of the CM_CONTROL register controls a bunch
 * of CLCD settings.
 */
#define INTEGRATOR_HDR_CTRL_OFFSET	0x0C
#define INTEGRATOR_CLCD_LCDBIASEN	BIT(8)
#define INTEGRATOR_CLCD_LCDBIASUP	BIT(9)
#define INTEGRATOR_CLCD_LCDBIASDN	BIT(10)
/* Bits 11,12,13 controls the LCD or VGA bridge type */
#define INTEGRATOR_CLCD_LCDMUX_LCD24	BIT(11)
#define INTEGRATOR_CLCD_LCDMUX_SHARP	(BIT(11)|BIT(12))
#define INTEGRATOR_CLCD_LCDMUX_VGA555	BIT(13)
#define INTEGRATOR_CLCD_LCDMUX_VGA24	(BIT(11)|BIT(12)|BIT(13))
#define INTEGRATOR_CLCD_LCD0_EN		BIT(14)
#define INTEGRATOR_CLCD_LCD1_EN		BIT(15)
/* R/L flip on Sharp */
#define INTEGRATOR_CLCD_LCD_STATIC1	BIT(16)
/* U/D flip on Sharp */
#define INTEGRATOR_CLCD_LCD_STATIC2	BIT(17)
/* No connection on Sharp */
#define INTEGRATOR_CLCD_LCD_STATIC	BIT(18)
/* 0 = 24bit VGA, 1 = 18bit VGA */
#define INTEGRATOR_CLCD_LCD_N24BITEN	BIT(19)

#define INTEGRATOR_CLCD_MASK		GENMASK(19, 8)

static void pl111_integrator_enable(struct drm_device *drm, u32 format)
{
	u32 val;

	dev_info(drm->dev, "enable Integrator CLCD connectors\n");

	/* FIXME: really needed? */
	val = INTEGRATOR_CLCD_LCD_STATIC1 | INTEGRATOR_CLCD_LCD_STATIC2 |
		INTEGRATOR_CLCD_LCD0_EN | INTEGRATOR_CLCD_LCD1_EN;

	switch (format) {
	case DRM_FORMAT_XBGR8888:
	case DRM_FORMAT_XRGB8888:
		/* 24bit formats */
		val |= INTEGRATOR_CLCD_LCDMUX_VGA24;
		break;
	case DRM_FORMAT_XBGR1555:
	case DRM_FORMAT_XRGB1555:
		/* Pseudocolor, RGB555, BGR555 */
		val |= INTEGRATOR_CLCD_LCDMUX_VGA555;
		break;
	default:
		dev_err(drm->dev, "unhandled format on Integrator 0x%08x\n",
			format);
		break;
	}

	regmap_update_bits(versatile_syscon_map,
			   INTEGRATOR_HDR_CTRL_OFFSET,
			   INTEGRATOR_CLCD_MASK,
			   val);
}

#define IMPD1_CTRL_OFFSET	0x18
#define IMPD1_CTRL_DISP_LCD	(0 << 0)
#define IMPD1_CTRL_DISP_VGA	(1 << 0)
#define IMPD1_CTRL_DISP_LCD1	(2 << 0)
#define IMPD1_CTRL_DISP_ENABLE	(1 << 2)
#define IMPD1_CTRL_DISP_MASK	(7 << 0)

static void pl111_impd1_enable(struct drm_device *drm, u32 format)
{
	u32 val;

	dev_info(drm->dev, "enable IM-PD1 CLCD connectors\n");
	val = IMPD1_CTRL_DISP_VGA | IMPD1_CTRL_DISP_ENABLE;

	regmap_update_bits(versatile_syscon_map,
			   IMPD1_CTRL_OFFSET,
			   IMPD1_CTRL_DISP_MASK,
			   val);
}

static void pl111_impd1_disable(struct drm_device *drm)
{
	dev_info(drm->dev, "disable IM-PD1 CLCD connectors\n");

	regmap_update_bits(versatile_syscon_map,
			   IMPD1_CTRL_OFFSET,
			   IMPD1_CTRL_DISP_MASK,
			   0);
}

/*
 * This configuration register in the Versatile and RealView
 * family is uniformly present but appears more and more
 * unutilized starting with the RealView series.
 */
#define SYS_CLCD			0x50
#define SYS_CLCD_MODE_MASK		(BIT(0)|BIT(1))
#define SYS_CLCD_MODE_888		0
#define SYS_CLCD_MODE_5551		BIT(0)
#define SYS_CLCD_MODE_565_R_LSB		BIT(1)
#define SYS_CLCD_MODE_565_B_LSB		(BIT(0)|BIT(1))
#define SYS_CLCD_CONNECTOR_MASK		(BIT(2)|BIT(3)|BIT(4)|BIT(5))
#define SYS_CLCD_NLCDIOON		BIT(2)
#define SYS_CLCD_VDDPOSSWITCH		BIT(3)
#define SYS_CLCD_PWR3V5SWITCH		BIT(4)
#define SYS_CLCD_VDDNEGSWITCH		BIT(5)

static void pl111_versatile_disable(struct drm_device *drm)
{
	dev_info(drm->dev, "disable Versatile CLCD connectors\n");
	regmap_update_bits(versatile_syscon_map,
			   SYS_CLCD,
			   SYS_CLCD_CONNECTOR_MASK,
			   0);
}

static void pl111_versatile_enable(struct drm_device *drm, u32 format)
{
	u32 val = 0;

	dev_info(drm->dev, "enable Versatile CLCD connectors\n");

	switch (format) {
	case DRM_FORMAT_ABGR8888:
	case DRM_FORMAT_XBGR8888:
	case DRM_FORMAT_ARGB8888:
	case DRM_FORMAT_XRGB8888:
		val |= SYS_CLCD_MODE_888;
		break;
	case DRM_FORMAT_BGR565:
		val |= SYS_CLCD_MODE_565_R_LSB;
		break;
	case DRM_FORMAT_RGB565:
		val |= SYS_CLCD_MODE_565_B_LSB;
		break;
	case DRM_FORMAT_ABGR1555:
	case DRM_FORMAT_XBGR1555:
	case DRM_FORMAT_ARGB1555:
	case DRM_FORMAT_XRGB1555:
		val |= SYS_CLCD_MODE_5551;
		break;
	default:
		dev_err(drm->dev, "unhandled format on Versatile 0x%08x\n",
			format);
		break;
	}

	/* Set up the MUX */
	regmap_update_bits(versatile_syscon_map,
			   SYS_CLCD,
			   SYS_CLCD_MODE_MASK,
			   val);

	/* Then enable the display */
	regmap_update_bits(versatile_syscon_map,
			   SYS_CLCD,
			   SYS_CLCD_CONNECTOR_MASK,
			   SYS_CLCD_NLCDIOON | SYS_CLCD_PWR3V5SWITCH);
}

static void pl111_realview_clcd_disable(struct drm_device *drm)
{
	dev_info(drm->dev, "disable RealView CLCD connectors\n");
	regmap_update_bits(versatile_syscon_map,
			   SYS_CLCD,
			   SYS_CLCD_CONNECTOR_MASK,
			   0);
}

static void pl111_realview_clcd_enable(struct drm_device *drm, u32 format)
{
	dev_info(drm->dev, "enable RealView CLCD connectors\n");
	regmap_update_bits(versatile_syscon_map,
			   SYS_CLCD,
			   SYS_CLCD_CONNECTOR_MASK,
			   SYS_CLCD_NLCDIOON | SYS_CLCD_PWR3V5SWITCH);
}

/* PL110 pixel formats for Integrator, vanilla PL110 */
static const u32 pl110_integrator_pixel_formats[] = {
	DRM_FORMAT_ABGR8888,
	DRM_FORMAT_XBGR8888,
	DRM_FORMAT_ARGB8888,
	DRM_FORMAT_XRGB8888,
	DRM_FORMAT_ABGR1555,
	DRM_FORMAT_XBGR1555,
	DRM_FORMAT_ARGB1555,
	DRM_FORMAT_XRGB1555,
};

/* Extended PL110 pixel formats for Integrator and Versatile */
static const u32 pl110_versatile_pixel_formats[] = {
	DRM_FORMAT_ABGR8888,
	DRM_FORMAT_XBGR8888,
	DRM_FORMAT_ARGB8888,
	DRM_FORMAT_XRGB8888,
	DRM_FORMAT_BGR565, /* Uses external PLD */
	DRM_FORMAT_RGB565, /* Uses external PLD */
	DRM_FORMAT_ABGR1555,
	DRM_FORMAT_XBGR1555,
	DRM_FORMAT_ARGB1555,
	DRM_FORMAT_XRGB1555,
};

static const u32 pl111_realview_pixel_formats[] = {
	DRM_FORMAT_ABGR8888,
	DRM_FORMAT_XBGR8888,
	DRM_FORMAT_ARGB8888,
	DRM_FORMAT_XRGB8888,
	DRM_FORMAT_BGR565,
	DRM_FORMAT_RGB565,
	DRM_FORMAT_ABGR1555,
	DRM_FORMAT_XBGR1555,
	DRM_FORMAT_ARGB1555,
	DRM_FORMAT_XRGB1555,
	DRM_FORMAT_ABGR4444,
	DRM_FORMAT_XBGR4444,
	DRM_FORMAT_ARGB4444,
	DRM_FORMAT_XRGB4444,
};

/*
 * The Integrator variant is a PL110 with a bunch of broken, or not
 * yet implemented features
 */
static const struct pl111_variant_data pl110_integrator = {
	.name = "PL110 Integrator",
	.is_pl110 = true,
	.broken_clockdivider = true,
	.broken_vblank = true,
	.formats = pl110_integrator_pixel_formats,
	.nformats = ARRAY_SIZE(pl110_integrator_pixel_formats),
	.fb_bpp = 16,
};

/*
 * The IM-PD1 variant is a PL110 with a bunch of broken, or not
 * yet implemented features
 */
static const struct pl111_variant_data pl110_impd1 = {
	.name = "PL110 IM-PD1",
	.is_pl110 = true,
	.broken_clockdivider = true,
	.broken_vblank = true,
	.formats = pl110_integrator_pixel_formats,
	.nformats = ARRAY_SIZE(pl110_integrator_pixel_formats),
	.fb_bpp = 16,
};

/*
 * This is the in-between PL110 variant found in the ARM Versatile,
 * supporting RGB565/BGR565
 */
static const struct pl111_variant_data pl110_versatile = {
	.name = "PL110 Versatile",
	.is_pl110 = true,
	.external_bgr = true,
	.formats = pl110_versatile_pixel_formats,
	.nformats = ARRAY_SIZE(pl110_versatile_pixel_formats),
	.fb_bpp = 16,
};

/*
 * RealView PL111 variant, the only real difference from the vanilla
 * PL111 is that we select 16bpp framebuffer by default to be able
 * to get 1024x768 without saturating the memory bus.
 */
static const struct pl111_variant_data pl111_realview = {
	.name = "PL111 RealView",
	.formats = pl111_realview_pixel_formats,
	.nformats = ARRAY_SIZE(pl111_realview_pixel_formats),
	.fb_bpp = 16,
};

/*
 * Versatile Express PL111 variant, again we just push the maximum
 * BPP to 16 to be able to get 1024x768 without saturating the memory
 * bus. The clockdivider also seems broken on the Versatile Express.
 */
static const struct pl111_variant_data pl111_vexpress = {
	.name = "PL111 Versatile Express",
	.formats = pl111_realview_pixel_formats,
	.nformats = ARRAY_SIZE(pl111_realview_pixel_formats),
	.fb_bpp = 16,
	.broken_clockdivider = true,
};

#define VEXPRESS_FPGAMUX_MOTHERBOARD		0x00
#define VEXPRESS_FPGAMUX_DAUGHTERBOARD_1	0x01
#define VEXPRESS_FPGAMUX_DAUGHTERBOARD_2	0x02

static int pl111_vexpress_clcd_init(struct device *dev, struct device_node *np,
				    struct pl111_drm_dev_private *priv)
{
	struct platform_device *pdev;
	struct device_node *root;
	struct device_node *child;
	struct device_node *ct_clcd = NULL;
	struct regmap *map;
	bool has_coretile_clcd = false;
	bool has_coretile_hdlcd = false;
	bool mux_motherboard = true;
	u32 val;
	int ret;

	if (!IS_ENABLED(CONFIG_VEXPRESS_CONFIG))
		return -ENODEV;

	/*
	 * Check if we have a CLCD or HDLCD on the core tile by checking if a
	 * CLCD or HDLCD is available in the root of the device tree.
	 */
	root = of_find_node_by_path("/");
	if (!root)
		return -EINVAL;

	for_each_available_child_of_node(root, child) {
		if (of_device_is_compatible(child, "arm,pl111")) {
			has_coretile_clcd = true;
			ct_clcd = child;
			break;
		}
		if (of_device_is_compatible(child, "arm,hdlcd")) {
			has_coretile_hdlcd = true;
			of_node_put(child);
			break;
		}
	}

	of_node_put(root);

	/*
	 * If there is a coretile HDLCD and it has a driver,
	 * do not mux the CLCD on the motherboard to the DVI.
	 */
	if (has_coretile_hdlcd && IS_ENABLED(CONFIG_DRM_HDLCD))
		mux_motherboard = false;

	/*
	 * On the Vexpress CA9 we let the CLCD on the coretile
	 * take precedence, so also in this case do not mux the
	 * motherboard to the DVI.
	 */
	if (has_coretile_clcd)
		mux_motherboard = false;

	if (mux_motherboard) {
		dev_info(dev, "DVI muxed to motherboard CLCD\n");
		val = VEXPRESS_FPGAMUX_MOTHERBOARD;
	} else if (ct_clcd == dev->of_node) {
		dev_info(dev,
			 "DVI muxed to daughterboard 1 (core tile) CLCD\n");
		val = VEXPRESS_FPGAMUX_DAUGHTERBOARD_1;
	} else {
		dev_info(dev, "core tile graphics present\n");
		dev_info(dev, "this device will be deactivated\n");
		return -ENODEV;
	}

	/* Call into deep Vexpress configuration API */
	pdev = of_find_device_by_node(np);
	if (!pdev) {
		dev_err(dev, "can't find the sysreg device, deferring\n");
		return -EPROBE_DEFER;
	}

	map = devm_regmap_init_vexpress_config(&pdev->dev);
	if (IS_ERR(map)) {
		platform_device_put(pdev);
		return PTR_ERR(map);
	}

	ret = regmap_write(map, 0, val);
	platform_device_put(pdev);
	if (ret) {
		dev_err(dev, "error setting DVI muxmode\n");
		return -ENODEV;
	}

	priv->variant = &pl111_vexpress;
	dev_info(dev, "initializing Versatile Express PL111\n");

	return 0;
}

int pl111_versatile_init(struct device *dev, struct pl111_drm_dev_private *priv)
{
	const struct of_device_id *clcd_id;
	enum versatile_clcd versatile_clcd_type;
	struct device_node *np;
	struct regmap *map;

	np = of_find_matching_node_and_match(NULL, versatile_clcd_of_match,
					     &clcd_id);
	if (!np) {
		/* Non-ARM reference designs, just bail out */
		return 0;
	}

	versatile_clcd_type = (enum versatile_clcd)clcd_id->data;

	/* Versatile Express special handling */
	if (versatile_clcd_type == VEXPRESS_CLCD_V2M) {
		int ret = pl111_vexpress_clcd_init(dev, np, priv);
		of_node_put(np);
		if (ret)
			dev_err(dev, "Versatile Express init failed - %d", ret);
		return ret;
	}

	/*
	 * On the Integrator, check if we should use the IM-PD1 instead,
	 * if we find it, it will take precedence. This is on the Integrator/AP
	 * which only has this option for PL110 graphics.
	 */
	 if (versatile_clcd_type == INTEGRATOR_CLCD_CM) {
		np = of_find_matching_node_and_match(NULL, impd1_clcd_of_match,
						     &clcd_id);
		if (np)
			versatile_clcd_type = (enum versatile_clcd)clcd_id->data;
	}

	map = syscon_node_to_regmap(np);
	of_node_put(np);
	if (IS_ERR(map)) {
		dev_err(dev, "no Versatile syscon regmap\n");
		return PTR_ERR(map);
	}

	switch (versatile_clcd_type) {
	case INTEGRATOR_CLCD_CM:
		versatile_syscon_map = map;
		priv->variant = &pl110_integrator;
		priv->variant_display_enable = pl111_integrator_enable;
		dev_info(dev, "set up callbacks for Integrator PL110\n");
		break;
	case INTEGRATOR_IMPD1:
		versatile_syscon_map = map;
		priv->variant = &pl110_impd1;
		priv->variant_display_enable = pl111_impd1_enable;
		priv->variant_display_disable = pl111_impd1_disable;
		dev_info(dev, "set up callbacks for IM-PD1 PL110\n");
		break;
	case VERSATILE_CLCD:
		versatile_syscon_map = map;
		/* This can do RGB565 with external PLD */
		priv->variant = &pl110_versatile;
		priv->variant_display_enable = pl111_versatile_enable;
		priv->variant_display_disable = pl111_versatile_disable;
		/*
		 * The Versatile has a variant halfway between PL110
		 * and PL111 where these two registers have already been
		 * swapped.
		 */
		priv->ienb = CLCD_PL111_IENB;
		priv->ctrl = CLCD_PL111_CNTL;
		dev_info(dev, "set up callbacks for Versatile PL110\n");
		break;
	case REALVIEW_CLCD_EB:
	case REALVIEW_CLCD_PB1176:
	case REALVIEW_CLCD_PB11MP:
	case REALVIEW_CLCD_PBA8:
	case REALVIEW_CLCD_PBX:
		versatile_syscon_map = map;
		priv->variant = &pl111_realview;
		priv->variant_display_enable = pl111_realview_clcd_enable;
		priv->variant_display_disable = pl111_realview_clcd_disable;
		dev_info(dev, "set up callbacks for RealView PL111\n");
		break;
	default:
		dev_info(dev, "unknown Versatile system controller\n");
		break;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(pl111_versatile_init);
