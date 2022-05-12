// SPDX-License-Identifier: GPL-2.0
//
// Copyright 2008 Simtec Electronics
//	Ben Dooks <ben@simtec.co.uk>
//	http://armlinux.simtec.co.uk/

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/serial_core.h>
#include <linux/serial_s3c.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/io.h>

#include <asm/mach-types.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include <mach/irqs.h>
#include "map.h"

#include "devs.h"
#include "cpu.h"
#include <linux/platform_data/i2c-s3c2410.h>
#include "gpio-samsung.h"

#include "s3c64xx.h"

#define UCON S3C2410_UCON_DEFAULT | S3C2410_UCON_UCLK
#define ULCON S3C2410_LCON_CS8 | S3C2410_LCON_PNONE | S3C2410_LCON_STOPB
#define UFCON S3C2410_UFCON_RXTRIG8 | S3C2410_UFCON_FIFOMODE

static struct s3c2410_uartcfg smdk6400_uartcfgs[] __initdata = {
	[0] = {
		.hwport	     = 0,
		.flags	     = 0,
		.ucon	     = 0x3c5,
		.ulcon	     = 0x03,
		.ufcon	     = 0x51,
	},
	[1] = {
		.hwport	     = 1,
		.flags	     = 0,
		.ucon	     = 0x3c5,
		.ulcon	     = 0x03,
		.ufcon	     = 0x51,
	},
};

static struct map_desc smdk6400_iodesc[] = {};

static void __init smdk6400_map_io(void)
{
	s3c64xx_init_io(smdk6400_iodesc, ARRAY_SIZE(smdk6400_iodesc));
	s3c64xx_set_xtal_freq(12000000);
	s3c24xx_init_uarts(smdk6400_uartcfgs, ARRAY_SIZE(smdk6400_uartcfgs));
	s3c64xx_set_timer_source(S3C64XX_PWM3, S3C64XX_PWM4);
}

static struct platform_device *smdk6400_devices[] __initdata = {
	&s3c_device_hsmmc1,
	&s3c_device_i2c0,
};

static struct i2c_board_info i2c_devs[] __initdata = {
	{ I2C_BOARD_INFO("wm8753", 0x1A), },
	{ I2C_BOARD_INFO("24c08", 0x50), },
};

static void __init smdk6400_machine_init(void)
{
	i2c_register_board_info(0, i2c_devs, ARRAY_SIZE(i2c_devs));
	platform_add_devices(smdk6400_devices, ARRAY_SIZE(smdk6400_devices));
}

MACHINE_START(SMDK6400, "SMDK6400")
	/* Maintainer: Ben Dooks <ben-linux@fluff.org> */
	.atag_offset	= 0x100,
	.nr_irqs	= S3C64XX_NR_IRQS,
	.init_irq	= s3c6400_init_irq,
	.map_io		= smdk6400_map_io,
	.init_machine	= smdk6400_machine_init,
	.init_time	= s3c64xx_timer_init,
MACHINE_END
