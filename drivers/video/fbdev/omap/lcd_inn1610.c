// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * LCD panel support for the TI OMAP1610 Innovator board
 *
 * Copyright (C) 2004 Nokia Corporation
 * Author: Imre Deak <imre.deak@nokia.com>
 */

#include <linux/module.h>
#include <linux/platform_device.h>

#include <linux/gpio.h>
#include "omapfb.h"

#define MODULE_NAME	"omapfb-lcd_h3"

static int innovator1610_panel_init(struct lcd_panel *panel,
				    struct omapfb_device *fbdev)
{
	int r = 0;

	/* configure GPIO(14, 15) as outputs */
	if (gpio_request_one(14, GPIOF_OUT_INIT_LOW, "lcd_en0")) {
		pr_err(MODULE_NAME ": can't request GPIO 14\n");
		r = -1;
		goto exit;
	}
	if (gpio_request_one(15, GPIOF_OUT_INIT_LOW, "lcd_en1")) {
		pr_err(MODULE_NAME ": can't request GPIO 15\n");
		gpio_free(14);
		r = -1;
		goto exit;
	}
exit:
	return r;
}

static void innovator1610_panel_cleanup(struct lcd_panel *panel)
{
	gpio_free(15);
	gpio_free(14);
}

static int innovator1610_panel_enable(struct lcd_panel *panel)
{
	/* set GPIO14 and GPIO15 high */
	gpio_set_value(14, 1);
	gpio_set_value(15, 1);
	return 0;
}

static void innovator1610_panel_disable(struct lcd_panel *panel)
{
	/* set GPIO13, GPIO14 and GPIO15 low */
	gpio_set_value(14, 0);
	gpio_set_value(15, 0);
}

static struct lcd_panel innovator1610_panel = {
	.name		= "inn1610",
	.config		= OMAP_LCDC_PANEL_TFT,

	.bpp		= 16,
	.data_lines	= 16,
	.x_res		= 320,
	.y_res		= 240,
	.pixel_clock	= 12500,
	.hsw		= 40,
	.hfp		= 40,
	.hbp		= 72,
	.vsw		= 1,
	.vfp		= 1,
	.vbp		= 0,
	.pcd		= 12,

	.init		= innovator1610_panel_init,
	.cleanup	= innovator1610_panel_cleanup,
	.enable		= innovator1610_panel_enable,
	.disable	= innovator1610_panel_disable,
};

static int innovator1610_panel_probe(struct platform_device *pdev)
{
	omapfb_register_panel(&innovator1610_panel);
	return 0;
}

static struct platform_driver innovator1610_panel_driver = {
	.probe		= innovator1610_panel_probe,
	.driver		= {
		.name	= "lcd_inn1610",
	},
};

module_platform_driver(innovator1610_panel_driver);

MODULE_AUTHOR("Imre Deak");
MODULE_DESCRIPTION("LCD panel support for the TI OMAP1610 Innovator board");
MODULE_LICENSE("GPL");
