// SPDX-License-Identifier: GPL-2.0
//
// Copyright (c) 2008 Ramax Lo <ramaxlo@gmail.com>
//      Based on mach-anubis.c by Ben Dooks <ben@simtec.co.uk>
//      and modifications by SBZ <sbz@spgui.org> and
//      Weibing <http://weibing.blogbus.com> and
//      Michel Pollet <buserror@gmail.com>
//
// For product information, visit https://code.google.com/p/mini2440/

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/gpio/machine.h>
#include <linux/input.h>
#include <linux/io.h>
#include <linux/serial_core.h>
#include <linux/serial_s3c.h>
#include <linux/dm9000.h>
#include <linux/property.h>
#include <linux/platform_device.h>
#include <linux/gpio_keys.h>
#include <linux/i2c.h>
#include <linux/mmc/host.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>

#include <linux/platform_data/fb-s3c2410.h>
#include <asm/mach-types.h>

#include "regs-gpio.h"
#include <linux/platform_data/leds-s3c24xx.h>
#include <mach/irqs.h>
#include "gpio-samsung.h"
#include <linux/platform_data/mtd-nand-s3c2410.h>
#include <linux/platform_data/i2c-s3c2410.h>
#include <linux/platform_data/mmc-s3cmci.h>
#include <linux/platform_data/usb-s3c2410_udc.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/rawnand.h>
#include <linux/mtd/nand-ecc-sw-hamming.h>
#include <linux/mtd/partitions.h>

#include "gpio-cfg.h"
#include "devs.h"
#include "cpu.h"

#include <sound/s3c24xx_uda134x.h>

#include "s3c24xx.h"

#define MACH_MINI2440_DM9K_BASE (S3C2410_CS4 + 0x300)

static struct map_desc mini2440_iodesc[] __initdata = {
	/* nothing to declare, move along */
};

#define UCON S3C2410_UCON_DEFAULT
#define ULCON (S3C2410_LCON_CS8 | S3C2410_LCON_PNONE | S3C2410_LCON_STOPB)
#define UFCON (S3C2410_UFCON_RXTRIG8 | S3C2410_UFCON_FIFOMODE)


static struct s3c2410_uartcfg mini2440_uartcfgs[] __initdata = {
	[0] = {
		.hwport		= 0,
		.flags		= 0,
		.ucon		= UCON,
		.ulcon		= ULCON,
		.ufcon		= UFCON,
	},
	[1] = {
		.hwport		= 1,
		.flags		= 0,
		.ucon		= UCON,
		.ulcon		= ULCON,
		.ufcon		= UFCON,
	},
	[2] = {
		.hwport		= 2,
		.flags		= 0,
		.ucon		= UCON,
		.ulcon		= ULCON,
		.ufcon		= UFCON,
	},
};

/* USB device UDC support */

static struct s3c2410_udc_mach_info mini2440_udc_cfg __initdata = {
	.pullup_pin = S3C2410_GPC(5),
};


/* LCD timing and setup */

/*
 * This macro simplifies the table bellow
 */
#define _LCD_DECLARE(_clock, _xres, margin_left, margin_right, hsync, \
			_yres, margin_top, margin_bottom, vsync, refresh) \
	.width = _xres, \
	.xres = _xres, \
	.height = _yres, \
	.yres = _yres, \
	.left_margin	= margin_left,	\
	.right_margin	= margin_right,	\
	.upper_margin	= margin_top,	\
	.lower_margin	= margin_bottom,	\
	.hsync_len	= hsync,	\
	.vsync_len	= vsync,	\
	.pixclock	= ((_clock*100000000000LL) /	\
			   ((refresh) * \
			   (hsync + margin_left + _xres + margin_right) * \
			   (vsync + margin_top + _yres + margin_bottom))), \
	.bpp		= 16,\
	.type		= (S3C2410_LCDCON1_TFT16BPP |\
			   S3C2410_LCDCON1_TFT)

static struct s3c2410fb_display mini2440_lcd_cfg[] __initdata = {
	[0] = {	/* mini2440 + 3.5" TFT + touchscreen */
		_LCD_DECLARE(
			7,			/* The 3.5 is quite fast */
			240, 21, 38, 6,		/* x timing */
			320, 4, 4, 2,		/* y timing */
			60),			/* refresh rate */
		.lcdcon5	= (S3C2410_LCDCON5_FRM565 |
				   S3C2410_LCDCON5_INVVLINE |
				   S3C2410_LCDCON5_INVVFRAME |
				   S3C2410_LCDCON5_INVVDEN |
				   S3C2410_LCDCON5_PWREN),
	},
	[1] = { /* mini2440 + 7" TFT + touchscreen */
		_LCD_DECLARE(
			10,			/* the 7" runs slower */
			800, 40, 40, 48,	/* x timing */
			480, 29, 3, 3,		/* y timing */
			50),			/* refresh rate */
		.lcdcon5	= (S3C2410_LCDCON5_FRM565 |
				   S3C2410_LCDCON5_INVVLINE |
				   S3C2410_LCDCON5_INVVFRAME |
				   S3C2410_LCDCON5_PWREN),
	},
	/* The VGA shield can outout at several resolutions. All share
	 * the same timings, however, anything smaller than 1024x768
	 * will only be displayed in the top left corner of a 1024x768
	 * XGA output unless you add optional dip switches to the shield.
	 * Therefore timings for other resolutions have been omitted here.
	 */
	[2] = {
		_LCD_DECLARE(
			10,
			1024, 1, 2, 2,		/* y timing */
			768, 200, 16, 16,	/* x timing */
			24),	/* refresh rate, maximum stable,
				 * tested with the FPGA shield
				 */
		.lcdcon5	= (S3C2410_LCDCON5_FRM565 |
				   S3C2410_LCDCON5_HWSWP),
	},
	/* mini2440 + 3.5" TFT (LCD-W35i, LQ035Q1DG06 type) + touchscreen*/
	[3] = {
		_LCD_DECLARE(
			/* clock */
			7,
			/* xres, margin_right, margin_left, hsync */
			320, 68, 66, 4,
			/* yres, margin_top, margin_bottom, vsync */
			240, 4, 4, 9,
			/* refresh rate */
			60),
		.lcdcon5	= (S3C2410_LCDCON5_FRM565 |
				   S3C2410_LCDCON5_INVVDEN |
				   S3C2410_LCDCON5_INVVFRAME |
				   S3C2410_LCDCON5_INVVLINE |
				   S3C2410_LCDCON5_INVVCLK |
				   S3C2410_LCDCON5_HWSWP),
	},
};

/* todo - put into gpio header */

#define S3C2410_GPCCON_MASK(x)	(3 << ((x) * 2))
#define S3C2410_GPDCON_MASK(x)	(3 << ((x) * 2))

static struct s3c2410fb_mach_info mini2440_fb_info __initdata = {
	.displays	 = &mini2440_lcd_cfg[0], /* not constant! see init */
	.num_displays	 = 1,
	.default_display = 0,

	/* Enable VD[2..7], VD[10..15], VD[18..23] and VCLK, syncs, VDEN
	 * and disable the pull down resistors on pins we are using for LCD
	 * data.
	 */

	.gpcup		= (0xf << 1) | (0x3f << 10),

	.gpccon		= (S3C2410_GPC1_VCLK   | S3C2410_GPC2_VLINE |
			   S3C2410_GPC3_VFRAME | S3C2410_GPC4_VM |
			   S3C2410_GPC10_VD2   | S3C2410_GPC11_VD3 |
			   S3C2410_GPC12_VD4   | S3C2410_GPC13_VD5 |
			   S3C2410_GPC14_VD6   | S3C2410_GPC15_VD7),

	.gpccon_mask	= (S3C2410_GPCCON_MASK(1)  | S3C2410_GPCCON_MASK(2)  |
			   S3C2410_GPCCON_MASK(3)  | S3C2410_GPCCON_MASK(4)  |
			   S3C2410_GPCCON_MASK(10) | S3C2410_GPCCON_MASK(11) |
			   S3C2410_GPCCON_MASK(12) | S3C2410_GPCCON_MASK(13) |
			   S3C2410_GPCCON_MASK(14) | S3C2410_GPCCON_MASK(15)),

	.gpccon_reg	= S3C2410_GPCCON,
	.gpcup_reg	= S3C2410_GPCUP,

	.gpdup		= (0x3f << 2) | (0x3f << 10),

	.gpdcon		= (S3C2410_GPD2_VD10  | S3C2410_GPD3_VD11 |
			   S3C2410_GPD4_VD12  | S3C2410_GPD5_VD13 |
			   S3C2410_GPD6_VD14  | S3C2410_GPD7_VD15 |
			   S3C2410_GPD10_VD18 | S3C2410_GPD11_VD19 |
			   S3C2410_GPD12_VD20 | S3C2410_GPD13_VD21 |
			   S3C2410_GPD14_VD22 | S3C2410_GPD15_VD23),

	.gpdcon_mask	= (S3C2410_GPDCON_MASK(2)  | S3C2410_GPDCON_MASK(3) |
			   S3C2410_GPDCON_MASK(4)  | S3C2410_GPDCON_MASK(5) |
			   S3C2410_GPDCON_MASK(6)  | S3C2410_GPDCON_MASK(7) |
			   S3C2410_GPDCON_MASK(10) | S3C2410_GPDCON_MASK(11)|
			   S3C2410_GPDCON_MASK(12) | S3C2410_GPDCON_MASK(13)|
			   S3C2410_GPDCON_MASK(14) | S3C2410_GPDCON_MASK(15)),

	.gpdcon_reg	= S3C2410_GPDCON,
	.gpdup_reg	= S3C2410_GPDUP,
};

/* MMC/SD  */

static struct s3c24xx_mci_pdata mini2440_mmc_cfg __initdata = {
	.wprotect_invert	= 1,
	.set_power		= s3c24xx_mci_def_set_power,
	.ocr_avail		= MMC_VDD_32_33|MMC_VDD_33_34,
};

static struct gpiod_lookup_table mini2440_mmc_gpio_table = {
	.dev_id = "s3c2410-sdi",
	.table = {
		/* Card detect S3C2410_GPG(8) */
		GPIO_LOOKUP("GPIOG", 8, "cd", GPIO_ACTIVE_LOW),
		/* Write protect S3C2410_GPH(8) */
		GPIO_LOOKUP("GPIOH", 8, "wp", GPIO_ACTIVE_HIGH),
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

/* NAND Flash on MINI2440 board */

static struct mtd_partition mini2440_default_nand_part[] __initdata = {
	[0] = {
		.name	= "u-boot",
		.size	= SZ_256K,
		.offset	= 0,
	},
	[1] = {
		.name	= "u-boot-env",
		.size	= SZ_128K,
		.offset	= SZ_256K,
	},
	[2] = {
		.name	= "kernel",
		/* 5 megabytes, for a kernel with no modules
		 * or a uImage with a ramdisk attached
		 */
		.size	= 0x00500000,
		.offset	= SZ_256K + SZ_128K,
	},
	[3] = {
		.name	= "root",
		.offset	= SZ_256K + SZ_128K + 0x00500000,
		.size	= MTDPART_SIZ_FULL,
	},
};

static struct s3c2410_nand_set mini2440_nand_sets[] __initdata = {
	[0] = {
		.name		= "nand",
		.nr_chips	= 1,
		.nr_partitions	= ARRAY_SIZE(mini2440_default_nand_part),
		.partitions	= mini2440_default_nand_part,
		.flash_bbt	= 1, /* we use u-boot to create a BBT */
	},
};

static struct s3c2410_platform_nand mini2440_nand_info __initdata = {
	.tacls		= 0,
	.twrph0		= 25,
	.twrph1		= 15,
	.nr_sets	= ARRAY_SIZE(mini2440_nand_sets),
	.sets		= mini2440_nand_sets,
	.ignore_unset_ecc = 1,
	.engine_type	= NAND_ECC_ENGINE_TYPE_ON_HOST,
};

/* DM9000AEP 10/100 ethernet controller */

static struct resource mini2440_dm9k_resource[] = {
	[0] = DEFINE_RES_MEM(MACH_MINI2440_DM9K_BASE, 4),
	[1] = DEFINE_RES_MEM(MACH_MINI2440_DM9K_BASE + 4, 4),
	[2] = DEFINE_RES_NAMED(IRQ_EINT7, 1, NULL, IORESOURCE_IRQ
						| IORESOURCE_IRQ_HIGHEDGE),
};

/*
 * The DM9000 has no eeprom, and it's MAC address is set by
 * the bootloader before starting the kernel.
 */
static struct dm9000_plat_data mini2440_dm9k_pdata = {
	.flags		= (DM9000_PLATF_16BITONLY | DM9000_PLATF_NO_EEPROM),
};

static struct platform_device mini2440_device_eth = {
	.name		= "dm9000",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(mini2440_dm9k_resource),
	.resource	= mini2440_dm9k_resource,
	.dev		= {
		.platform_data	= &mini2440_dm9k_pdata,
	},
};

/*  CON5
 *	+--+	 /-----\
 *	|  |    |	|
 *	|  |	|  BAT	|
 *	|  |	 \_____/
 *	|  |
 *	|  |  +----+  +----+
 *	|  |  | K5 |  | K1 |
 *	|  |  +----+  +----+
 *	|  |  +----+  +----+
 *	|  |  | K4 |  | K2 |
 *	|  |  +----+  +----+
 *	|  |  +----+  +----+
 *	|  |  | K6 |  | K3 |
 *	|  |  +----+  +----+
 *	  .....
 */
static struct gpio_keys_button mini2440_buttons[] = {
	{
		.gpio		= S3C2410_GPG(0),		/* K1 */
		.code		= KEY_F1,
		.desc		= "Button 1",
		.active_low	= 1,
	},
	{
		.gpio		= S3C2410_GPG(3),		/* K2 */
		.code		= KEY_F2,
		.desc		= "Button 2",
		.active_low	= 1,
	},
	{
		.gpio		= S3C2410_GPG(5),		/* K3 */
		.code		= KEY_F3,
		.desc		= "Button 3",
		.active_low	= 1,
	},
	{
		.gpio		= S3C2410_GPG(6),		/* K4 */
		.code		= KEY_POWER,
		.desc		= "Power",
		.active_low	= 1,
	},
	{
		.gpio		= S3C2410_GPG(7),		/* K5 */
		.code		= KEY_F5,
		.desc		= "Button 5",
		.active_low	= 1,
	},
#if 0
	/* this pin is also known as TCLK1 and seems to already
	 * marked as "in use" somehow in the kernel -- possibly wrongly
	 */
	{
		.gpio		= S3C2410_GPG(11),	/* K6 */
		.code		= KEY_F6,
		.desc		= "Button 6",
		.active_low	= 1,
	},
#endif
};

static struct gpio_keys_platform_data mini2440_button_data = {
	.buttons	= mini2440_buttons,
	.nbuttons	= ARRAY_SIZE(mini2440_buttons),
};

static struct platform_device mini2440_button_device = {
	.name		= "gpio-keys",
	.id		= -1,
	.dev		= {
		.platform_data	= &mini2440_button_data,
	}
};

/* LEDS */

static struct gpiod_lookup_table mini2440_led1_gpio_table = {
	.dev_id = "s3c24xx_led.1",
	.table = {
		GPIO_LOOKUP("GPB", 5, NULL, GPIO_ACTIVE_LOW | GPIO_OPEN_DRAIN),
		{ },
	},
};

static struct gpiod_lookup_table mini2440_led2_gpio_table = {
	.dev_id = "s3c24xx_led.2",
	.table = {
		GPIO_LOOKUP("GPB", 6, NULL, GPIO_ACTIVE_LOW | GPIO_OPEN_DRAIN),
		{ },
	},
};

static struct gpiod_lookup_table mini2440_led3_gpio_table = {
	.dev_id = "s3c24xx_led.3",
	.table = {
		GPIO_LOOKUP("GPB", 7, NULL, GPIO_ACTIVE_LOW | GPIO_OPEN_DRAIN),
		{ },
	},
};

static struct gpiod_lookup_table mini2440_led4_gpio_table = {
	.dev_id = "s3c24xx_led.4",
	.table = {
		GPIO_LOOKUP("GPB", 8, NULL, GPIO_ACTIVE_LOW | GPIO_OPEN_DRAIN),
		{ },
	},
};

static struct gpiod_lookup_table mini2440_backlight_gpio_table = {
	.dev_id = "s3c24xx_led.5",
	.table = {
		GPIO_LOOKUP("GPG", 4, NULL, GPIO_ACTIVE_HIGH),
		{ },
	},
};

static struct s3c24xx_led_platdata mini2440_led1_pdata = {
	.name		= "led1",
	.def_trigger	= "heartbeat",
};

static struct s3c24xx_led_platdata mini2440_led2_pdata = {
	.name		= "led2",
	.def_trigger	= "nand-disk",
};

static struct s3c24xx_led_platdata mini2440_led3_pdata = {
	.name		= "led3",
	.def_trigger	= "mmc0",
};

static struct s3c24xx_led_platdata mini2440_led4_pdata = {
	.name		= "led4",
	.def_trigger	= "",
};

static struct s3c24xx_led_platdata mini2440_led_backlight_pdata = {
	.name		= "backlight",
	.def_trigger	= "backlight",
};

static struct platform_device mini2440_led1 = {
	.name		= "s3c24xx_led",
	.id		= 1,
	.dev		= {
		.platform_data	= &mini2440_led1_pdata,
	},
};

static struct platform_device mini2440_led2 = {
	.name		= "s3c24xx_led",
	.id		= 2,
	.dev		= {
		.platform_data	= &mini2440_led2_pdata,
	},
};

static struct platform_device mini2440_led3 = {
	.name		= "s3c24xx_led",
	.id		= 3,
	.dev		= {
		.platform_data	= &mini2440_led3_pdata,
	},
};

static struct platform_device mini2440_led4 = {
	.name		= "s3c24xx_led",
	.id		= 4,
	.dev		= {
		.platform_data	= &mini2440_led4_pdata,
	},
};

static struct platform_device mini2440_led_backlight = {
	.name		= "s3c24xx_led",
	.id		= 5,
	.dev		= {
		.platform_data	= &mini2440_led_backlight_pdata,
	},
};

/* AUDIO */

static struct s3c24xx_uda134x_platform_data mini2440_audio_pins = {
	.l3_clk = S3C2410_GPB(4),
	.l3_mode = S3C2410_GPB(2),
	.l3_data = S3C2410_GPB(3),
	.model = UDA134X_UDA1341
};

static struct platform_device mini2440_audio = {
	.name		= "s3c24xx_uda134x",
	.id		= 0,
	.dev		= {
		.platform_data	= &mini2440_audio_pins,
	},
};

/*
 * I2C devices
 */
static const struct property_entry mini2440_at24_properties[] = {
	PROPERTY_ENTRY_U32("pagesize", 16),
	{ }
};

static const struct software_node mini2440_at24_node = {
	.properties = mini2440_at24_properties,
};

static struct i2c_board_info mini2440_i2c_devs[] __initdata = {
	{
		I2C_BOARD_INFO("24c08", 0x50),
		.swnode = &mini2440_at24_node,
	},
};

static struct uda134x_platform_data s3c24xx_uda134x = {
	.l3 = {
		.gpio_clk = S3C2410_GPB(4),
		.gpio_data = S3C2410_GPB(3),
		.gpio_mode = S3C2410_GPB(2),
		.use_gpios = 1,
		.data_hold = 1,
		.data_setup = 1,
		.clock_high = 1,
		.mode_hold = 1,
		.mode = 1,
		.mode_setup = 1,
	},
	.model = UDA134X_UDA1341,
};

static struct platform_device uda1340_codec = {
		.name = "uda134x-codec",
		.id = -1,
		.dev = {
			.platform_data	= &s3c24xx_uda134x,
		},
};

static struct platform_device *mini2440_devices[] __initdata = {
	&s3c_device_ohci,
	&s3c_device_wdt,
	&s3c_device_i2c0,
	&s3c_device_rtc,
	&s3c_device_usbgadget,
	&mini2440_device_eth,
	&mini2440_led1,
	&mini2440_led2,
	&mini2440_led3,
	&mini2440_led4,
	&mini2440_button_device,
	&s3c_device_nand,
	&s3c_device_sdi,
	&s3c2440_device_dma,
	&s3c_device_iis,
	&uda1340_codec,
	&mini2440_audio,
};

static void __init mini2440_map_io(void)
{
	s3c24xx_init_io(mini2440_iodesc, ARRAY_SIZE(mini2440_iodesc));
	s3c24xx_init_uarts(mini2440_uartcfgs, ARRAY_SIZE(mini2440_uartcfgs));
	s3c24xx_set_timer_source(S3C24XX_PWM3, S3C24XX_PWM4);
}

static void __init mini2440_init_time(void)
{
	s3c2440_init_clocks(12000000);
	s3c24xx_timer_init();
}

/*
 * mini2440_features string
 *
 * t = Touchscreen present
 * b = backlight control
 * c = camera [TODO]
 * 0-9 LCD configuration
 *
 */
static char mini2440_features_str[12] __initdata = "0tb";

static int __init mini2440_features_setup(char *str)
{
	if (str)
		strlcpy(mini2440_features_str, str,
			sizeof(mini2440_features_str));
	return 1;
}

__setup("mini2440=", mini2440_features_setup);

#define FEATURE_SCREEN (1 << 0)
#define FEATURE_BACKLIGHT (1 << 1)
#define FEATURE_TOUCH (1 << 2)
#define FEATURE_CAMERA (1 << 3)

struct mini2440_features_t {
	int count;
	int done;
	int lcd_index;
	struct platform_device *optional[8];
};

static void __init mini2440_parse_features(
		struct mini2440_features_t *features,
		const char *features_str)
{
	const char *fp = features_str;

	features->count = 0;
	features->done = 0;
	features->lcd_index = -1;

	while (*fp) {
		char f = *fp++;

		switch (f) {
		case '0'...'9':	/* tft screen */
			if (features->done & FEATURE_SCREEN) {
				pr_info("MINI2440: '%c' ignored, screen type already set\n",
					f);
			} else {
				int li = f - '0';

				if (li >= ARRAY_SIZE(mini2440_lcd_cfg))
					pr_info("MINI2440: '%c' out of range LCD mode\n",
						f);
				else {
					features->optional[features->count++] =
							&s3c_device_lcd;
					features->lcd_index = li;
				}
			}
			features->done |= FEATURE_SCREEN;
			break;
		case 'b':
			if (features->done & FEATURE_BACKLIGHT)
				pr_info("MINI2440: '%c' ignored, backlight already set\n",
					f);
			else {
				features->optional[features->count++] =
						&mini2440_led_backlight;
			}
			features->done |= FEATURE_BACKLIGHT;
			break;
		case 't':
			pr_info("MINI2440: '%c' ignored, touchscreen not compiled in\n",
				f);
			break;
		case 'c':
			if (features->done & FEATURE_CAMERA)
				pr_info("MINI2440: '%c' ignored, camera already registered\n",
					f);
			else
				features->optional[features->count++] =
					&s3c_device_camif;
			features->done |= FEATURE_CAMERA;
			break;
		}
	}
}

static void __init mini2440_init(void)
{
	struct mini2440_features_t features = { 0 };
	int i;

	pr_info("MINI2440: Option string mini2440=%s\n",
			mini2440_features_str);

	/* Parse the feature string */
	mini2440_parse_features(&features, mini2440_features_str);

	/* turn LCD on */
	s3c_gpio_cfgpin(S3C2410_GPC(0), S3C2410_GPC0_LEND);

	/* Turn the backlight early on */
	WARN_ON(gpio_request_one(S3C2410_GPG(4), GPIOF_OUT_INIT_HIGH, NULL));
	gpio_free(S3C2410_GPG(4));

	/* remove pullup on optional PWM backlight -- unused on 3.5 and 7"s */
	gpio_request_one(S3C2410_GPB(1), GPIOF_IN, NULL);
	s3c_gpio_setpull(S3C2410_GPB(1), S3C_GPIO_PULL_UP);
	gpio_free(S3C2410_GPB(1));

	/* mark the key as input, without pullups (there is one on the board) */
	for (i = 0; i < ARRAY_SIZE(mini2440_buttons); i++) {
		s3c_gpio_setpull(mini2440_buttons[i].gpio, S3C_GPIO_PULL_UP);
		s3c_gpio_cfgpin(mini2440_buttons[i].gpio, S3C2410_GPIO_INPUT);
	}

	/* Configure the I2S pins (GPE0...GPE4) in correct mode */
	s3c_gpio_cfgall_range(S3C2410_GPE(0), 5, S3C_GPIO_SFN(2),
			      S3C_GPIO_PULL_NONE);

	if (features.lcd_index != -1) {
		int li;

		mini2440_fb_info.displays =
			&mini2440_lcd_cfg[features.lcd_index];

		pr_info("MINI2440: LCD");
		for (li = 0; li < ARRAY_SIZE(mini2440_lcd_cfg); li++)
			if (li == features.lcd_index)
				pr_cont(" [%d:%dx%d]", li,
					mini2440_lcd_cfg[li].width,
					mini2440_lcd_cfg[li].height);
			else
				pr_cont(" %d:%dx%d", li,
					mini2440_lcd_cfg[li].width,
					mini2440_lcd_cfg[li].height);
		pr_cont("\n");
		s3c24xx_fb_set_platdata(&mini2440_fb_info);
	}

	s3c24xx_udc_set_platdata(&mini2440_udc_cfg);
	gpiod_add_lookup_table(&mini2440_mmc_gpio_table);
	s3c24xx_mci_set_platdata(&mini2440_mmc_cfg);
	s3c_nand_set_platdata(&mini2440_nand_info);
	s3c_i2c0_set_platdata(NULL);

	i2c_register_board_info(0, mini2440_i2c_devs,
				ARRAY_SIZE(mini2440_i2c_devs));

	/* Disable pull-up on the LED lines */
	s3c_gpio_setpull(S3C2410_GPB(5), S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(S3C2410_GPB(6), S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(S3C2410_GPB(7), S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(S3C2410_GPB(8), S3C_GPIO_PULL_NONE);
	s3c_gpio_setpull(S3C2410_GPG(4), S3C_GPIO_PULL_NONE);

	/* Add lookups for the lines */
	gpiod_add_lookup_table(&mini2440_led1_gpio_table);
	gpiod_add_lookup_table(&mini2440_led2_gpio_table);
	gpiod_add_lookup_table(&mini2440_led3_gpio_table);
	gpiod_add_lookup_table(&mini2440_led4_gpio_table);
	gpiod_add_lookup_table(&mini2440_backlight_gpio_table);

	platform_add_devices(mini2440_devices, ARRAY_SIZE(mini2440_devices));

	if (features.count)	/* the optional features */
		platform_add_devices(features.optional, features.count);

}


MACHINE_START(MINI2440, "MINI2440")
	/* Maintainer: Michel Pollet <buserror@gmail.com> */
	.atag_offset	= 0x100,
	.map_io		= mini2440_map_io,
	.init_machine	= mini2440_init,
	.init_irq	= s3c2440_init_irq,
	.init_time	= mini2440_init_time,
MACHINE_END
