// SPDX-License-Identifier: GPL-2.0+
/*
 * FB driver for the ILI9340 LCD Controller
 *
 * Copyright (C) 2013 Noralf Tronnes
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio/consumer.h>
#include <linux/delay.h>
#include <video/mipi_display.h>

#include "fbtft.h"

#define DRVNAME		"fb_ili9340"
#define WIDTH		240
#define HEIGHT		320

/* Init sequence taken from: Arduino Library for the Adafruit 2.2" display */
static int init_display(struct fbtft_par *par)
{
	par->fbtftops.reset(par);

	write_reg(par, 0xEF, 0x03, 0x80, 0x02);
	write_reg(par, 0xCF, 0x00, 0XC1, 0X30);
	write_reg(par, 0xED, 0x64, 0x03, 0X12, 0X81);
	write_reg(par, 0xE8, 0x85, 0x00, 0x78);
	write_reg(par, 0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02);
	write_reg(par, 0xF7, 0x20);
	write_reg(par, 0xEA, 0x00, 0x00);

	/* Power Control 1 */
	write_reg(par, 0xC0, 0x23);

	/* Power Control 2 */
	write_reg(par, 0xC1, 0x10);

	/* VCOM Control 1 */
	write_reg(par, 0xC5, 0x3e, 0x28);

	/* VCOM Control 2 */
	write_reg(par, 0xC7, 0x86);

	/* COLMOD: Pixel Format Set */
	/* 16 bits/pixel */
	write_reg(par, MIPI_DCS_SET_PIXEL_FORMAT, 0x55);

	/* Frame Rate Control */
	/* Division ratio = fosc, Frame Rate = 79Hz */
	write_reg(par, 0xB1, 0x00, 0x18);

	/* Display Function Control */
	write_reg(par, 0xB6, 0x08, 0x82, 0x27);

	/* Gamma Function Disable */
	write_reg(par, 0xF2, 0x00);

	/* Gamma curve selection */
	write_reg(par, MIPI_DCS_SET_GAMMA_CURVE, 0x01);

	/* Positive Gamma Correction */
	write_reg(par, 0xE0,
		  0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1,
		  0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00);

	/* Negative Gamma Correction */
	write_reg(par, 0xE1,
		  0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1,
		  0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F);

	write_reg(par, MIPI_DCS_EXIT_SLEEP_MODE);

	mdelay(120);

	write_reg(par, MIPI_DCS_SET_DISPLAY_ON);

	return 0;
}

static void set_addr_win(struct fbtft_par *par, int xs, int ys, int xe, int ye)
{
	write_reg(par, MIPI_DCS_SET_COLUMN_ADDRESS,
		  xs >> 8, xs & 0xFF, xe >> 8, xe & 0xFF);

	write_reg(par, MIPI_DCS_SET_PAGE_ADDRESS,
		  ys >> 8, ys & 0xFF, ye >> 8, ye & 0xFF);

	write_reg(par, MIPI_DCS_WRITE_MEMORY_START);
}

#define ILI9340_MADCTL_MV  0x20
#define ILI9340_MADCTL_MX  0x40
#define ILI9340_MADCTL_MY  0x80
static int set_var(struct fbtft_par *par)
{
	u8 val;

	switch (par->info->var.rotate) {
	case 270:
		val = ILI9340_MADCTL_MV;
		break;
	case 180:
		val = ILI9340_MADCTL_MY;
		break;
	case 90:
		val = ILI9340_MADCTL_MV | ILI9340_MADCTL_MY | ILI9340_MADCTL_MX;
		break;
	default:
		val = ILI9340_MADCTL_MX;
		break;
	}
	/* Memory Access Control  */
	write_reg(par, MIPI_DCS_SET_ADDRESS_MODE, val | (par->bgr << 3));

	return 0;
}

static struct fbtft_display display = {
	.regwidth = 8,
	.width = WIDTH,
	.height = HEIGHT,
	.fbtftops = {
		.init_display = init_display,
		.set_addr_win = set_addr_win,
		.set_var = set_var,
	},
};

FBTFT_REGISTER_DRIVER(DRVNAME, "ilitek,ili9340", &display);

MODULE_ALIAS("spi:" DRVNAME);
MODULE_ALIAS("platform:" DRVNAME);
MODULE_ALIAS("spi:ili9340");
MODULE_ALIAS("platform:ili9340");

MODULE_DESCRIPTION("FB driver for the ILI9340 LCD Controller");
MODULE_AUTHOR("Noralf Tronnes");
MODULE_LICENSE("GPL");
