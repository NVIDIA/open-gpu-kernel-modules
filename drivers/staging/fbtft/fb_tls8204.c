// SPDX-License-Identifier: GPL-2.0+
/*
 * FB driver for the TLS8204 LCD Controller
 *
 * The display is monochrome and the video memory is RGB565.
 * Any pixel value except 0 turns the pixel on.
 *
 * Copyright (C) 2013 Noralf Tronnes
 * Copyright (C) 2014 Michael Hope (adapted for the TLS8204)
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/gpio/consumer.h>
#include <linux/spi/spi.h>
#include <linux/delay.h>

#include "fbtft.h"

#define DRVNAME		"fb_tls8204"
#define WIDTH		84
#define HEIGHT		48
#define TXBUFLEN	WIDTH

/* gamma is used to control contrast in this driver */
#define DEFAULT_GAMMA	"40"

static unsigned int bs = 4;
module_param(bs, uint, 0000);
MODULE_PARM_DESC(bs, "BS[2:0] Bias voltage level: 0-7 (default: 4)");

static int init_display(struct fbtft_par *par)
{
	par->fbtftops.reset(par);

	/* Enter extended command mode */
	write_reg(par, 0x21);	/* 5:1  1
				 * 2:0  PD - Powerdown control: chip is active
				 * 1:0  V  - Entry mode: horizontal addressing
				 * 0:1  H  - Extended instruction set control:
				 *	     extended
				 */

	/* H=1 Bias system */
	write_reg(par, 0x10 | (bs & 0x7));
				/* 4:1  1
				 * 3:0  0
				 * 2:x  BS2 - Bias System
				 * 1:x  BS1
				 * 0:x  BS0
				 */

	/* Set the address of the first display line. */
	write_reg(par, 0x04 | (64 >> 6));
	write_reg(par, 0x40 | (64 & 0x3F));

	/* Enter H=0 standard command mode */
	write_reg(par, 0x20);

	/* H=0 Display control */
	write_reg(par, 0x08 | 4);
				/* 3:1  1
				 * 2:1  D - DE: 10=normal mode
				 * 1:0  0
				 * 0:0  E
				 */

	return 0;
}

static void set_addr_win(struct fbtft_par *par, int xs, int ys, int xe, int ye)
{
	/* H=0 Set X address of RAM */
	write_reg(par, 0x80);	/* 7:1  1
				 * 6-0: X[6:0] - 0x00
				 */

	/* H=0 Set Y address of RAM */
	write_reg(par, 0x40);	/* 7:0  0
				 * 6:1  1
				 * 2-0: Y[2:0] - 0x0
				 */
}

static int write_vmem(struct fbtft_par *par, size_t offset, size_t len)
{
	u16 *vmem16 = (u16 *)par->info->screen_buffer;
	int x, y, i;
	int ret = 0;

	for (y = 0; y < HEIGHT / 8; y++) {
		u8 *buf = par->txbuf.buf;
		/* The display is 102x68 but the LCD is 84x48.
		 * Set the write pointer at the start of each row.
		 */
		gpiod_set_value(par->gpio.dc, 0);
		write_reg(par, 0x80 | 0);
		write_reg(par, 0x40 | y);

		for (x = 0; x < WIDTH; x++) {
			u8 ch = 0;

			for (i = 0; i < 8 * WIDTH; i += WIDTH) {
				ch >>= 1;
				if (vmem16[(y * 8 * WIDTH) + i + x])
					ch |= 0x80;
			}
			*buf++ = ch;
		}
		/* Write the row */
		gpiod_set_value(par->gpio.dc, 1);
		ret = par->fbtftops.write(par, par->txbuf.buf, WIDTH);
		if (ret < 0) {
			dev_err(par->info->device,
				"write failed and returned: %d\n", ret);
			break;
		}
	}

	return ret;
}

static int set_gamma(struct fbtft_par *par, u32 *curves)
{
	/* apply mask */
	curves[0] &= 0x7F;

	write_reg(par, 0x21); /* turn on extended instruction set */
	write_reg(par, 0x80 | curves[0]);
	write_reg(par, 0x20); /* turn off extended instruction set */

	return 0;
}

static struct fbtft_display display = {
	.regwidth = 8,
	.width = WIDTH,
	.height = HEIGHT,
	.txbuflen = TXBUFLEN,
	.gamma_num = 1,
	.gamma_len = 1,
	.gamma = DEFAULT_GAMMA,
	.fbtftops = {
		.init_display = init_display,
		.set_addr_win = set_addr_win,
		.write_vmem = write_vmem,
		.set_gamma = set_gamma,
	},
	.backlight = 1,
};

FBTFT_REGISTER_DRIVER(DRVNAME, "teralane,tls8204", &display);

MODULE_ALIAS("spi:" DRVNAME);
MODULE_ALIAS("spi:tls8204");

MODULE_DESCRIPTION("FB driver for the TLS8204 LCD Controller");
MODULE_AUTHOR("Michael Hope");
MODULE_LICENSE("GPL");
