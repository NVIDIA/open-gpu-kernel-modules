// SPDX-License-Identifier: GPL-2.0
//
// Copyright (c) 2008 Ramax Lo <ramaxlo@gmail.com>
//      Based on mach-anubis.c by Ben Dooks <ben@simtec.co.uk>
//      and modifications by SBZ <sbz@spgui.org> and
//      Weibing <http://weibing.blogbus.com>
//
// For product information, visit http://www.arm.com/

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/gpio/machine.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/serial_core.h>
#include <linux/serial_s3c.h>
#include <linux/dm9000.h>
#include <linux/platform_device.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include <linux/platform_data/fb-s3c2410.h>
#include <asm/irq.h>
#include <asm/mach-types.h>

#include "regs-gpio.h"
#include "gpio-samsung.h"
#include <linux/platform_data/mtd-nand-s3c2410.h>
#include <linux/platform_data/i2c-s3c2410.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/rawnand.h>
#include <linux/mtd/nand-ecc-sw-hamming.h>
#include <linux/mtd/partitions.h>

#include "devs.h"
#include "cpu.h"
#include <linux/platform_data/mmc-s3cmci.h>

#include "s3c24xx.h"

static struct map_desc at2440evb_iodesc[] __initdata = {
	/* Nothing here */
};

#define UCON S3C2410_UCON_DEFAULT
#define ULCON (S3C2410_LCON_CS8 | S3C2410_LCON_PNONE)
#define UFCON (S3C2410_UFCON_RXTRIG8 | S3C2410_UFCON_FIFOMODE)

static struct s3c2410_uartcfg at2440evb_uartcfgs[] __initdata = {
	[0] = {
		.hwport	     = 0,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
		.clk_sel	= S3C2410_UCON_CLKSEL1 | S3C2410_UCON_CLKSEL2,
	},
	[1] = {
		.hwport	     = 1,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
		.clk_sel	= S3C2410_UCON_CLKSEL1 | S3C2410_UCON_CLKSEL2,
	},
};

/* NAND Flash on AT2440EVB board */

static struct mtd_partition __initdata at2440evb_default_nand_part[] = {
	[0] = {
		.name	= "Boot Agent",
		.size	= SZ_256K,
		.offset	= 0,
	},
	[1] = {
		.name	= "Kernel",
		.size	= SZ_2M,
		.offset	= SZ_256K,
	},
	[2] = {
		.name	= "Root",
		.offset	= SZ_256K + SZ_2M,
		.size	= MTDPART_SIZ_FULL,
	},
};

static struct s3c2410_nand_set __initdata at2440evb_nand_sets[] = {
	[0] = {
		.name		= "nand",
		.nr_chips	= 1,
		.nr_partitions	= ARRAY_SIZE(at2440evb_default_nand_part),
		.partitions	= at2440evb_default_nand_part,
	},
};

static struct s3c2410_platform_nand __initdata at2440evb_nand_info = {
	.tacls		= 25,
	.twrph0		= 55,
	.twrph1		= 40,
	.nr_sets	= ARRAY_SIZE(at2440evb_nand_sets),
	.sets		= at2440evb_nand_sets,
	.engine_type	= NAND_ECC_ENGINE_TYPE_SOFT,
};

/* DM9000AEP 10/100 ethernet controller */

static struct resource at2440evb_dm9k_resource[] = {
	[0] = DEFINE_RES_MEM(S3C2410_CS3, 4),
	[1] = DEFINE_RES_MEM(S3C2410_CS3 + 4, 4),
	[2] = DEFINE_RES_NAMED(IRQ_EINT7, 1, NULL, IORESOURCE_IRQ \
					| IORESOURCE_IRQ_HIGHEDGE),
};

static struct dm9000_plat_data at2440evb_dm9k_pdata = {
	.flags		= (DM9000_PLATF_16BITONLY | DM9000_PLATF_NO_EEPROM),
};

static struct platform_device at2440evb_device_eth = {
	.name		= "dm9000",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(at2440evb_dm9k_resource),
	.resource	= at2440evb_dm9k_resource,
	.dev		= {
		.platform_data	= &at2440evb_dm9k_pdata,
	},
};

static struct s3c24xx_mci_pdata at2440evb_mci_pdata __initdata = {
	.set_power	= s3c24xx_mci_def_set_power,
};

static struct gpiod_lookup_table at2440evb_mci_gpio_table = {
	.dev_id = "s3c2410-sdi",
	.table = {
		/* Card detect S3C2410_GPG(10) */
		GPIO_LOOKUP("GPIOG", 10, "cd", GPIO_ACTIVE_LOW),
		/* bus pins */
		GPIO_LOOKUP_IDX("GPIOE",  5, "bus", 0, GPIO_ACTIVE_HIGH),
		GPIO_LOOKUP_IDX("GPIOE",  6, "bus", 1, GPIO_ACTIVE_HIGH),
		GPIO_LOOKUP_IDX("GPIOE",  7, "bus", 2, GPIO_ACTIVE_HIGH),
		GPIO_LOOKUP_IDX("GPIOE",  8, "bus", 3, GPIO_ACTIVE_HIGH),
		GPIO_LOOKUP_IDX("GPIOE",  9, "bus", 4, GPIO_ACTIVE_HIGH),
		GPIO_LOOKUP_IDX("GPIOE", 10, "bus", 5, GPIO_ACTIVE_HIGH),
		{ },
	},
};


/* 7" LCD panel */

static struct s3c2410fb_display at2440evb_lcd_cfg __initdata = {

	.lcdcon5	= S3C2410_LCDCON5_FRM565 |
			  S3C2410_LCDCON5_INVVLINE |
			  S3C2410_LCDCON5_INVVFRAME |
			  S3C2410_LCDCON5_PWREN |
			  S3C2410_LCDCON5_HWSWP,

	.type		= S3C2410_LCDCON1_TFT,

	.width		= 800,
	.height		= 480,

	.pixclock	= 33333, /* HCLK 60 MHz, divisor 2 */
	.xres		= 800,
	.yres		= 480,
	.bpp		= 16,
	.left_margin	= 88,
	.right_margin	= 40,
	.hsync_len	= 128,
	.upper_margin	= 32,
	.lower_margin	= 11,
	.vsync_len	= 2,
};

static struct s3c2410fb_mach_info at2440evb_fb_info __initdata = {
	.displays	= &at2440evb_lcd_cfg,
	.num_displays	= 1,
	.default_display = 0,
};

static struct platform_device *at2440evb_devices[] __initdata = {
	&s3c_device_ohci,
	&s3c_device_wdt,
	&s3c_device_adc,
	&s3c_device_i2c0,
	&s3c_device_rtc,
	&s3c_device_nand,
	&s3c_device_sdi,
	&s3c_device_lcd,
	&at2440evb_device_eth,
};

static void __init at2440evb_map_io(void)
{
	s3c24xx_init_io(at2440evb_iodesc, ARRAY_SIZE(at2440evb_iodesc));
	s3c24xx_init_uarts(at2440evb_uartcfgs, ARRAY_SIZE(at2440evb_uartcfgs));
	s3c24xx_set_timer_source(S3C24XX_PWM3, S3C24XX_PWM4);
}

static void __init at2440evb_init_time(void)
{
	s3c2440_init_clocks(16934400);
	s3c24xx_timer_init();
}

static void __init at2440evb_init(void)
{
	s3c24xx_fb_set_platdata(&at2440evb_fb_info);
	gpiod_add_lookup_table(&at2440evb_mci_gpio_table);
	s3c24xx_mci_set_platdata(&at2440evb_mci_pdata);
	s3c_nand_set_platdata(&at2440evb_nand_info);
	s3c_i2c0_set_platdata(NULL);

	platform_add_devices(at2440evb_devices, ARRAY_SIZE(at2440evb_devices));
}


MACHINE_START(AT2440EVB, "AT2440EVB")
	.atag_offset	= 0x100,
	.map_io		= at2440evb_map_io,
	.init_machine	= at2440evb_init,
	.init_irq	= s3c2440_init_irq,
	.init_time	= at2440evb_init_time,
MACHINE_END
