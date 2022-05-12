// SPDX-License-Identifier: GPL-2.0
//
// Copyright 2010 Darius Augulis <augulis.darius@gmail.com>
// Copyright 2008 Openmoko, Inc.
// Copyright 2008 Simtec Electronics
//	Ben Dooks <ben@simtec.co.uk>
//	http://armlinux.simtec.co.uk/

#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/fb.h>
#include <linux/gpio.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/dm9000.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/partitions.h>
#include <linux/serial_core.h>
#include <linux/serial_s3c.h>
#include <linux/types.h>

#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>

#include "map.h"
#include "regs-gpio.h"
#include "gpio-samsung.h"

#include <linux/soc/samsung/s3c-adc.h>
#include "cpu.h"
#include "devs.h"
#include "fb.h"
#include <linux/platform_data/mtd-nand-s3c2410.h>
#include <linux/platform_data/mmc-sdhci-s3c.h>
#include "sdhci.h"
#include <linux/platform_data/touchscreen-s3c2410.h>
#include <mach/irqs.h>

#include <video/platform_lcd.h>
#include <video/samsung_fimd.h>

#include "s3c64xx.h"
#include "regs-modem-s3c64xx.h"
#include "regs-srom-s3c64xx.h"

#define UCON S3C2410_UCON_DEFAULT
#define ULCON (S3C2410_LCON_CS8 | S3C2410_LCON_PNONE | S3C2410_LCON_STOPB)
#define UFCON (S3C2410_UFCON_RXTRIG8 | S3C2410_UFCON_FIFOMODE)

static struct s3c2410_uartcfg mini6410_uartcfgs[] __initdata = {
	[0] = {
		.hwport	= 0,
		.flags	= 0,
		.ucon	= UCON,
		.ulcon	= ULCON,
		.ufcon	= UFCON,
	},
	[1] = {
		.hwport	= 1,
		.flags	= 0,
		.ucon	= UCON,
		.ulcon	= ULCON,
		.ufcon	= UFCON,
	},
	[2] = {
		.hwport	= 2,
		.flags	= 0,
		.ucon	= UCON,
		.ulcon	= ULCON,
		.ufcon	= UFCON,
	},
	[3] = {
		.hwport	= 3,
		.flags	= 0,
		.ucon	= UCON,
		.ulcon	= ULCON,
		.ufcon	= UFCON,
	},
};

/* DM9000AEP 10/100 ethernet controller */

static struct resource mini6410_dm9k_resource[] = {
	[0] = DEFINE_RES_MEM(S3C64XX_PA_XM0CSN1, 2),
	[1] = DEFINE_RES_MEM(S3C64XX_PA_XM0CSN1 + 4, 2),
	[2] = DEFINE_RES_NAMED(S3C_EINT(7), 1, NULL, IORESOURCE_IRQ \
					| IORESOURCE_IRQ_HIGHLEVEL),
};

static struct dm9000_plat_data mini6410_dm9k_pdata = {
	.flags		= (DM9000_PLATF_16BITONLY | DM9000_PLATF_NO_EEPROM),
};

static struct platform_device mini6410_device_eth = {
	.name		= "dm9000",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(mini6410_dm9k_resource),
	.resource	= mini6410_dm9k_resource,
	.dev		= {
		.platform_data	= &mini6410_dm9k_pdata,
	},
};

static struct mtd_partition mini6410_nand_part[] = {
	[0] = {
		.name	= "uboot",
		.size	= SZ_1M,
		.offset	= 0,
	},
	[1] = {
		.name	= "kernel",
		.size	= SZ_2M,
		.offset	= SZ_1M,
	},
	[2] = {
		.name	= "rootfs",
		.size	= MTDPART_SIZ_FULL,
		.offset	= SZ_1M + SZ_2M,
	},
};

static struct s3c2410_nand_set mini6410_nand_sets[] = {
	[0] = {
		.name		= "nand",
		.nr_chips	= 1,
		.nr_partitions	= ARRAY_SIZE(mini6410_nand_part),
		.partitions	= mini6410_nand_part,
	},
};

static struct s3c2410_platform_nand mini6410_nand_info = {
	.tacls		= 25,
	.twrph0		= 55,
	.twrph1		= 40,
	.nr_sets	= ARRAY_SIZE(mini6410_nand_sets),
	.sets		= mini6410_nand_sets,
	.engine_type	= NAND_ECC_ENGINE_TYPE_SOFT,
};

static struct s3c_fb_pd_win mini6410_lcd_type0_fb_win = {
	.max_bpp	= 32,
	.default_bpp	= 16,
	.xres		= 480,
	.yres		= 272,
};

static struct fb_videomode mini6410_lcd_type0_timing = {
	/* 4.3" 480x272 */
	.left_margin	= 3,
	.right_margin	= 2,
	.upper_margin	= 1,
	.lower_margin	= 1,
	.hsync_len	= 40,
	.vsync_len	= 1,
	.xres		= 480,
	.yres		= 272,
};

static struct s3c_fb_pd_win mini6410_lcd_type1_fb_win = {
	.max_bpp	= 32,
	.default_bpp	= 16,
	.xres		= 800,
	.yres		= 480,
};

static struct fb_videomode mini6410_lcd_type1_timing = {
	/* 7.0" 800x480 */
	.left_margin	= 8,
	.right_margin	= 13,
	.upper_margin	= 7,
	.lower_margin	= 5,
	.hsync_len	= 3,
	.vsync_len	= 1,
	.xres		= 800,
	.yres		= 480,
};

static struct s3c_fb_platdata mini6410_lcd_pdata[] __initdata = {
	{
		.setup_gpio	= s3c64xx_fb_gpio_setup_24bpp,
		.vtiming	= &mini6410_lcd_type0_timing,
		.win[0]		= &mini6410_lcd_type0_fb_win,
		.vidcon0	= VIDCON0_VIDOUT_RGB | VIDCON0_PNRMODE_RGB,
		.vidcon1	= VIDCON1_INV_HSYNC | VIDCON1_INV_VSYNC,
	}, {
		.setup_gpio	= s3c64xx_fb_gpio_setup_24bpp,
		.vtiming	= &mini6410_lcd_type1_timing,
		.win[0]		= &mini6410_lcd_type1_fb_win,
		.vidcon0	= VIDCON0_VIDOUT_RGB | VIDCON0_PNRMODE_RGB,
		.vidcon1	= VIDCON1_INV_HSYNC | VIDCON1_INV_VSYNC,
	},
	{ },
};

static void mini6410_lcd_power_set(struct plat_lcd_data *pd,
				   unsigned int power)
{
	if (power)
		gpio_direction_output(S3C64XX_GPE(0), 1);
	else
		gpio_direction_output(S3C64XX_GPE(0), 0);
}

static struct plat_lcd_data mini6410_lcd_power_data = {
	.set_power	= mini6410_lcd_power_set,
};

static struct platform_device mini6410_lcd_powerdev = {
	.name			= "platform-lcd",
	.dev.parent		= &s3c_device_fb.dev,
	.dev.platform_data	= &mini6410_lcd_power_data,
};

static struct s3c_sdhci_platdata mini6410_hsmmc1_pdata = {
	.max_width		= 4,
	.cd_type		= S3C_SDHCI_CD_GPIO,
	.ext_cd_gpio		= S3C64XX_GPN(10),
	.ext_cd_gpio_invert	= true,
};

static struct platform_device *mini6410_devices[] __initdata = {
	&mini6410_device_eth,
	&s3c_device_hsmmc0,
	&s3c_device_hsmmc1,
	&s3c_device_ohci,
	&s3c_device_nand,
	&s3c_device_fb,
	&mini6410_lcd_powerdev,
	&s3c_device_adc,
};

static void __init mini6410_map_io(void)
{
	u32 tmp;

	s3c64xx_init_io(NULL, 0);
	s3c64xx_set_xtal_freq(12000000);
	s3c24xx_init_uarts(mini6410_uartcfgs, ARRAY_SIZE(mini6410_uartcfgs));
	s3c64xx_set_timer_source(S3C64XX_PWM3, S3C64XX_PWM4);

	/* set the LCD type */
	tmp = __raw_readl(S3C64XX_SPCON);
	tmp &= ~S3C64XX_SPCON_LCD_SEL_MASK;
	tmp |= S3C64XX_SPCON_LCD_SEL_RGB;
	__raw_writel(tmp, S3C64XX_SPCON);

	/* remove the LCD bypass */
	tmp = __raw_readl(S3C64XX_MODEM_MIFPCON);
	tmp &= ~MIFPCON_LCD_BYPASS;
	__raw_writel(tmp, S3C64XX_MODEM_MIFPCON);
}

/*
 * mini6410_features string
 *
 * 0-9 LCD configuration
 *
 */
static char mini6410_features_str[12] __initdata = "0";

static int __init mini6410_features_setup(char *str)
{
	if (str)
		strlcpy(mini6410_features_str, str,
			sizeof(mini6410_features_str));
	return 1;
}

__setup("mini6410=", mini6410_features_setup);

#define FEATURE_SCREEN (1 << 0)

struct mini6410_features_t {
	int done;
	int lcd_index;
};

static void mini6410_parse_features(
		struct mini6410_features_t *features,
		const char *features_str)
{
	const char *fp = features_str;

	features->done = 0;
	features->lcd_index = 0;

	while (*fp) {
		char f = *fp++;

		switch (f) {
		case '0'...'9':	/* tft screen */
			if (features->done & FEATURE_SCREEN) {
				printk(KERN_INFO "MINI6410: '%c' ignored, "
					"screen type already set\n", f);
			} else {
				int li = f - '0';
				if (li >= ARRAY_SIZE(mini6410_lcd_pdata))
					printk(KERN_INFO "MINI6410: '%c' out "
						"of range LCD mode\n", f);
				else {
					features->lcd_index = li;
				}
			}
			features->done |= FEATURE_SCREEN;
			break;
		}
	}
}

static void __init mini6410_machine_init(void)
{
	u32 cs1;
	struct mini6410_features_t features = { 0 };

	printk(KERN_INFO "MINI6410: Option string mini6410=%s\n",
			mini6410_features_str);

	/* Parse the feature string */
	mini6410_parse_features(&features, mini6410_features_str);

	printk(KERN_INFO "MINI6410: selected LCD display is %dx%d\n",
		mini6410_lcd_pdata[features.lcd_index].win[0]->xres,
		mini6410_lcd_pdata[features.lcd_index].win[0]->yres);

	s3c_nand_set_platdata(&mini6410_nand_info);
	s3c_fb_set_platdata(&mini6410_lcd_pdata[features.lcd_index]);
	s3c_sdhci1_set_platdata(&mini6410_hsmmc1_pdata);
	s3c64xx_ts_set_platdata(NULL);

	/* configure nCS1 width to 16 bits */

	cs1 = __raw_readl(S3C64XX_SROM_BW) &
		~(S3C64XX_SROM_BW__CS_MASK << S3C64XX_SROM_BW__NCS1__SHIFT);
	cs1 |= ((1 << S3C64XX_SROM_BW__DATAWIDTH__SHIFT) |
		(1 << S3C64XX_SROM_BW__WAITENABLE__SHIFT) |
		(1 << S3C64XX_SROM_BW__BYTEENABLE__SHIFT)) <<
			S3C64XX_SROM_BW__NCS1__SHIFT;
	__raw_writel(cs1, S3C64XX_SROM_BW);

	/* set timing for nCS1 suitable for ethernet chip */

	__raw_writel((0 << S3C64XX_SROM_BCX__PMC__SHIFT) |
		(6 << S3C64XX_SROM_BCX__TACP__SHIFT) |
		(4 << S3C64XX_SROM_BCX__TCAH__SHIFT) |
		(1 << S3C64XX_SROM_BCX__TCOH__SHIFT) |
		(13 << S3C64XX_SROM_BCX__TACC__SHIFT) |
		(4 << S3C64XX_SROM_BCX__TCOS__SHIFT) |
		(0 << S3C64XX_SROM_BCX__TACS__SHIFT), S3C64XX_SROM_BC1);

	gpio_request(S3C64XX_GPF(15), "LCD power");
	gpio_request(S3C64XX_GPE(0), "LCD power");

	platform_add_devices(mini6410_devices, ARRAY_SIZE(mini6410_devices));
}

MACHINE_START(MINI6410, "MINI6410")
	/* Maintainer: Darius Augulis <augulis.darius@gmail.com> */
	.atag_offset	= 0x100,
	.nr_irqs	= S3C64XX_NR_IRQS,
	.init_irq	= s3c6410_init_irq,
	.map_io		= mini6410_map_io,
	.init_machine	= mini6410_machine_init,
	.init_time	= s3c64xx_timer_init,
MACHINE_END
