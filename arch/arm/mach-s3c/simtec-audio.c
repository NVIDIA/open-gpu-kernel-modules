// SPDX-License-Identifier: GPL-2.0
//
// Copyright (c) 2009 Simtec Electronics
//	http://armlinux.simtec.co.uk/
//	Ben Dooks <ben@simtec.co.uk>
//
// Audio setup for various Simtec S3C24XX implementations

#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/io.h>

#include "regs-gpio.h"
#include "gpio-samsung.h"
#include "gpio-cfg.h"

#include <linux/platform_data/asoc-s3c24xx_simtec.h>
#include "devs.h"

#include "bast.h"
#include "simtec.h"

/* platform ops for audio */

static void simtec_audio_startup_lrroute(void)
{
	unsigned int tmp;
	unsigned long flags;

	local_irq_save(flags);

	tmp = __raw_readb(BAST_VA_CTRL1);
	tmp &= ~BAST_CPLD_CTRL1_LRMASK;
	tmp |= BAST_CPLD_CTRL1_LRCDAC;
	__raw_writeb(tmp, BAST_VA_CTRL1);

	local_irq_restore(flags);
}

static struct s3c24xx_audio_simtec_pdata simtec_audio_platdata;
static char our_name[32];

static struct platform_device simtec_audio_dev = {
	.name	= our_name,
	.id	= -1,
	.dev	= {
		.parent		= &s3c_device_iis.dev,
		.platform_data	= &simtec_audio_platdata,
	},
};

int __init simtec_audio_add(const char *name, bool has_lr_routing,
			    struct s3c24xx_audio_simtec_pdata *spd)
{
	if (!name)
		name = "tlv320aic23";

	snprintf(our_name, sizeof(our_name)-1, "s3c24xx-simtec-%s", name);

	/* copy platform data so the source can be __initdata */
	if (spd)
		simtec_audio_platdata = *spd;

	if (has_lr_routing)
		simtec_audio_platdata.startup = simtec_audio_startup_lrroute;

	/* Configure the I2S pins (GPE0...GPE4) in correct mode */
	s3c_gpio_cfgall_range(S3C2410_GPE(0), 5, S3C_GPIO_SFN(2),
			      S3C_GPIO_PULL_NONE);

	platform_device_register(&s3c_device_iis);
	platform_device_register(&simtec_audio_dev);
	return 0;
}
