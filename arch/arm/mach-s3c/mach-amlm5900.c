// SPDX-License-Identifier: GPL-2.0+
//
// Copyright (c) 2006 American Microsystems Limited
//	David Anders <danders@amltd.com>
//
// @History:
// derived from linux/arch/arm/mach-s3c2410/mach-bast.c, written by
// Ben Dooks <ben@simtec.co.uk>

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/gpio/machine.h>
#include <linux/gpio.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/serial_core.h>
#include <linux/serial_s3c.h>
#include <linux/io.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>
#include <asm/mach/flash.h>

#include <asm/irq.h>
#include <asm/mach-types.h>
#include <linux/platform_data/fb-s3c2410.h>

#include "regs-gpio.h"
#include "gpio-samsung.h"

#include <linux/platform_data/i2c-s3c2410.h>
#include "devs.h"
#include "cpu.h"
#include "gpio-cfg.h"

#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/map.h>
#include <linux/mtd/physmap.h>

#include "s3c24xx.h"

static struct resource amlm5900_nor_resource =
			DEFINE_RES_MEM(0x00000000, SZ_16M);

static struct mtd_partition amlm5900_mtd_partitions[] = {
	{
		.name		= "System",
		.size		= 0x240000,
		.offset		= 0,
		.mask_flags 	= MTD_WRITEABLE,  /* force read-only */
	}, {
		.name		= "Kernel",
		.size		= 0x100000,
		.offset		= MTDPART_OFS_APPEND,
	}, {
		.name		= "Ramdisk",
		.size		= 0x300000,
		.offset		= MTDPART_OFS_APPEND,
	}, {
		.name		= "JFFS2",
		.size		= 0x9A0000,
		.offset		= MTDPART_OFS_APPEND,
	}, {
		.name		= "Settings",
		.size		= MTDPART_SIZ_FULL,
		.offset		= MTDPART_OFS_APPEND,
	}
};

static struct physmap_flash_data amlm5900_flash_data = {
	.width		= 2,
	.parts		= amlm5900_mtd_partitions,
	.nr_parts	= ARRAY_SIZE(amlm5900_mtd_partitions),
};

static struct platform_device amlm5900_device_nor = {
	.name		= "physmap-flash",
	.id		= 0,
	.dev = {
			.platform_data = &amlm5900_flash_data,
		},
	.num_resources	= 1,
	.resource	= &amlm5900_nor_resource,
};

static struct map_desc amlm5900_iodesc[] __initdata = {
};

#define UCON S3C2410_UCON_DEFAULT
#define ULCON S3C2410_LCON_CS8 | S3C2410_LCON_PNONE | S3C2410_LCON_STOPB
#define UFCON S3C2410_UFCON_RXTRIG8 | S3C2410_UFCON_FIFOMODE

static struct s3c2410_uartcfg amlm5900_uartcfgs[] = {
	[0] = {
		.hwport	     = 0,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
	},
	[1] = {
		.hwport	     = 1,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
	},
	[2] = {
		.hwport	     = 2,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
	}
};

static struct gpiod_lookup_table amlm5900_mmc_gpio_table = {
	.dev_id = "s3c2410-sdi",
	.table = {
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

static struct platform_device *amlm5900_devices[] __initdata = {
#ifdef CONFIG_FB_S3C2410
	&s3c_device_lcd,
#endif
	&s3c_device_adc,
	&s3c_device_wdt,
	&s3c_device_i2c0,
	&s3c_device_ohci,
 	&s3c_device_rtc,
	&s3c_device_usbgadget,
        &s3c_device_sdi,
	&amlm5900_device_nor,
};

static void __init amlm5900_map_io(void)
{
	s3c24xx_init_io(amlm5900_iodesc, ARRAY_SIZE(amlm5900_iodesc));
	s3c24xx_init_uarts(amlm5900_uartcfgs, ARRAY_SIZE(amlm5900_uartcfgs));
	s3c24xx_set_timer_source(S3C24XX_PWM3, S3C24XX_PWM4);
}

static void __init amlm5900_init_time(void)
{
	s3c2410_init_clocks(12000000);
	s3c24xx_timer_init();
}

#ifdef CONFIG_FB_S3C2410
static struct s3c2410fb_display __initdata amlm5900_lcd_info = {
	.width		= 160,
	.height		= 160,

	.type		= S3C2410_LCDCON1_STN4,

	.pixclock	= 680000, /* HCLK = 100MHz */
	.xres		= 160,
	.yres		= 160,
	.bpp		= 4,
	.left_margin	= 1 << (4 + 3),
	.right_margin	= 8 << 3,
	.hsync_len	= 48,
	.upper_margin	= 0,
	.lower_margin	= 0,

	.lcdcon5	= 0x00000001,
};

static struct s3c2410fb_mach_info __initdata amlm5900_fb_info = {

	.displays = &amlm5900_lcd_info,
	.num_displays = 1,
	.default_display = 0,

	.gpccon =	0xaaaaaaaa,
	.gpccon_mask =	0xffffffff,
	.gpccon_reg =	S3C2410_GPCCON,
	.gpcup =	0x0000ffff,
	.gpcup_mask =	0xffffffff,
	.gpcup_reg =	S3C2410_GPCUP,

	.gpdcon =	0xaaaaaaaa,
	.gpdcon_mask =	0xffffffff,
	.gpdcon_reg =	S3C2410_GPDCON,
	.gpdup =	0x0000ffff,
	.gpdup_mask =	0xffffffff,
	.gpdup_reg =	S3C2410_GPDUP,
};
#endif

static irqreturn_t
amlm5900_wake_interrupt(int irq, void *ignored)
{
	return IRQ_HANDLED;
}

static void amlm5900_init_pm(void)
{
	int ret = 0;

	ret = request_irq(IRQ_EINT9, &amlm5900_wake_interrupt,
				IRQF_TRIGGER_RISING | IRQF_SHARED,
				"amlm5900_wakeup", &amlm5900_wake_interrupt);
	if (ret != 0) {
		printk(KERN_ERR "AML-M5900: no wakeup irq, %d?\n", ret);
	} else {
		enable_irq_wake(IRQ_EINT9);
		/* configure the suspend/resume status pin */
		s3c_gpio_cfgpin(S3C2410_GPF(2), S3C2410_GPIO_OUTPUT);
		s3c_gpio_setpull(S3C2410_GPF(2), S3C_GPIO_PULL_UP);
	}
}
static void __init amlm5900_init(void)
{
	amlm5900_init_pm();
#ifdef CONFIG_FB_S3C2410
	s3c24xx_fb_set_platdata(&amlm5900_fb_info);
#endif
	s3c_i2c0_set_platdata(NULL);
	gpiod_add_lookup_table(&amlm5900_mmc_gpio_table);
	platform_add_devices(amlm5900_devices, ARRAY_SIZE(amlm5900_devices));
}

MACHINE_START(AML_M5900, "AML_M5900")
	.atag_offset	= 0x100,
	.map_io		= amlm5900_map_io,
	.init_irq	= s3c2410_init_irq,
	.init_machine	= amlm5900_init,
	.init_time	= amlm5900_init_time,
MACHINE_END
