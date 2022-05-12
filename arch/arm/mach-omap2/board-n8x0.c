// SPDX-License-Identifier: GPL-2.0-only
/*
 * linux/arch/arm/mach-omap2/board-n8x0.c
 *
 * Copyright (C) 2005-2009 Nokia Corporation
 * Author: Juha Yrjola <juha.yrjola@nokia.com>
 *
 * Modified from mach-omap2/board-generic.c
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/stddef.h>
#include <linux/i2c.h>
#include <linux/spi/spi.h>
#include <linux/usb/musb.h>
#include <linux/mmc/host.h>
#include <linux/platform_data/spi-omap2-mcspi.h>
#include <linux/platform_data/mmc-omap.h>
#include <linux/mfd/menelaus.h>
#include <sound/tlv320aic3x.h>

#include <asm/mach/arch.h>
#include <asm/mach-types.h>

#include "common.h"
#include "mmc.h"
#include "soc.h"
#include "common-board-devices.h"

#define TUSB6010_ASYNC_CS	1
#define TUSB6010_SYNC_CS	4
#define TUSB6010_GPIO_INT	58
#define TUSB6010_GPIO_ENABLE	0
#define TUSB6010_DMACHAN	0x3f

#define NOKIA_N810_WIMAX	(1 << 2)
#define NOKIA_N810		(1 << 1)
#define NOKIA_N800		(1 << 0)

static u32 board_caps;

#define board_is_n800()		(board_caps & NOKIA_N800)
#define board_is_n810()		(board_caps & NOKIA_N810)
#define board_is_n810_wimax()	(board_caps & NOKIA_N810_WIMAX)

static void board_check_revision(void)
{
	if (of_machine_is_compatible("nokia,n800"))
		board_caps = NOKIA_N800;
	else if (of_machine_is_compatible("nokia,n810"))
		board_caps = NOKIA_N810;
	else if (of_machine_is_compatible("nokia,n810-wimax"))
		board_caps = NOKIA_N810_WIMAX;

	if (!board_caps)
		pr_err("Unknown board\n");
}

#if IS_ENABLED(CONFIG_USB_MUSB_TUSB6010)
/*
 * Enable or disable power to TUSB6010. When enabling, turn on 3.3 V and
 * 1.5 V voltage regulators of PM companion chip. Companion chip will then
 * provide then PGOOD signal to TUSB6010 which will release it from reset.
 */
static int tusb_set_power(int state)
{
	int i, retval = 0;

	if (state) {
		gpio_set_value(TUSB6010_GPIO_ENABLE, 1);
		msleep(1);

		/* Wait until TUSB6010 pulls INT pin down */
		i = 100;
		while (i && gpio_get_value(TUSB6010_GPIO_INT)) {
			msleep(1);
			i--;
		}

		if (!i) {
			printk(KERN_ERR "tusb: powerup failed\n");
			retval = -ENODEV;
		}
	} else {
		gpio_set_value(TUSB6010_GPIO_ENABLE, 0);
		msleep(10);
	}

	return retval;
}

static struct musb_hdrc_config musb_config = {
	.multipoint	= 1,
	.dyn_fifo	= 1,
	.num_eps	= 16,
	.ram_bits	= 12,
};

static struct musb_hdrc_platform_data tusb_data = {
	.mode		= MUSB_OTG,
	.set_power	= tusb_set_power,
	.min_power	= 25,	/* x2 = 50 mA drawn from VBUS as peripheral */
	.power		= 100,	/* Max 100 mA VBUS for host mode */
	.config		= &musb_config,
};

static void __init n8x0_usb_init(void)
{
	int ret = 0;
	static const char announce[] __initconst = KERN_INFO "TUSB 6010\n";

	/* PM companion chip power control pin */
	ret = gpio_request_one(TUSB6010_GPIO_ENABLE, GPIOF_OUT_INIT_LOW,
			       "TUSB6010 enable");
	if (ret != 0) {
		printk(KERN_ERR "Could not get TUSB power GPIO%i\n",
		       TUSB6010_GPIO_ENABLE);
		return;
	}
	tusb_set_power(0);

	ret = tusb6010_setup_interface(&tusb_data, TUSB6010_REFCLK_19, 2,
					TUSB6010_ASYNC_CS, TUSB6010_SYNC_CS,
					TUSB6010_GPIO_INT, TUSB6010_DMACHAN);
	if (ret != 0)
		goto err;

	printk(announce);

	return;

err:
	gpio_free(TUSB6010_GPIO_ENABLE);
}
#else

static void __init n8x0_usb_init(void) {}

#endif /*CONFIG_USB_MUSB_TUSB6010 */


static struct omap2_mcspi_device_config p54spi_mcspi_config = {
	.turbo_mode	= 0,
};

static struct spi_board_info n800_spi_board_info[] __initdata = {
	{
		.modalias	= "p54spi",
		.bus_num	= 2,
		.chip_select	= 0,
		.max_speed_hz   = 48000000,
		.controller_data = &p54spi_mcspi_config,
	},
};

#if defined(CONFIG_MENELAUS) && IS_ENABLED(CONFIG_MMC_OMAP)

/*
 * On both N800 and N810, only the first of the two MMC controllers is in use.
 * The two MMC slots are multiplexed via Menelaus companion chip over I2C.
 * On N800, both slots are powered via Menelaus. On N810, only one of the
 * slots is powered via Menelaus. The N810 EMMC is powered via GPIO.
 *
 * VMMC				slot 1 on both N800 and N810
 * VDCDC3_APE and VMCS2_APE	slot 2 on N800
 * GPIO23 and GPIO9		slot 2 EMMC on N810
 *
 */
#define N8X0_SLOT_SWITCH_GPIO	96
#define N810_EMMC_VSD_GPIO	23
#define N810_EMMC_VIO_GPIO	9

static int slot1_cover_open;
static int slot2_cover_open;
static struct device *mmc_device;

static int n8x0_mmc_switch_slot(struct device *dev, int slot)
{
#ifdef CONFIG_MMC_DEBUG
	dev_dbg(dev, "Choose slot %d\n", slot + 1);
#endif
	gpio_set_value(N8X0_SLOT_SWITCH_GPIO, slot);
	return 0;
}

static int n8x0_mmc_set_power_menelaus(struct device *dev, int slot,
					int power_on, int vdd)
{
	int mV;

#ifdef CONFIG_MMC_DEBUG
	dev_dbg(dev, "Set slot %d power: %s (vdd %d)\n", slot + 1,
		power_on ? "on" : "off", vdd);
#endif
	if (slot == 0) {
		if (!power_on)
			return menelaus_set_vmmc(0);
		switch (1 << vdd) {
		case MMC_VDD_33_34:
		case MMC_VDD_32_33:
		case MMC_VDD_31_32:
			mV = 3100;
			break;
		case MMC_VDD_30_31:
			mV = 3000;
			break;
		case MMC_VDD_28_29:
			mV = 2800;
			break;
		case MMC_VDD_165_195:
			mV = 1850;
			break;
		default:
			BUG();
		}
		return menelaus_set_vmmc(mV);
	} else {
		if (!power_on)
			return menelaus_set_vdcdc(3, 0);
		switch (1 << vdd) {
		case MMC_VDD_33_34:
		case MMC_VDD_32_33:
			mV = 3300;
			break;
		case MMC_VDD_30_31:
		case MMC_VDD_29_30:
			mV = 3000;
			break;
		case MMC_VDD_28_29:
		case MMC_VDD_27_28:
			mV = 2800;
			break;
		case MMC_VDD_24_25:
		case MMC_VDD_23_24:
			mV = 2400;
			break;
		case MMC_VDD_22_23:
		case MMC_VDD_21_22:
			mV = 2200;
			break;
		case MMC_VDD_20_21:
			mV = 2000;
			break;
		case MMC_VDD_165_195:
			mV = 1800;
			break;
		default:
			BUG();
		}
		return menelaus_set_vdcdc(3, mV);
	}
	return 0;
}

static void n810_set_power_emmc(struct device *dev,
					 int power_on)
{
	dev_dbg(dev, "Set EMMC power %s\n", power_on ? "on" : "off");

	if (power_on) {
		gpio_set_value(N810_EMMC_VSD_GPIO, 1);
		msleep(1);
		gpio_set_value(N810_EMMC_VIO_GPIO, 1);
		msleep(1);
	} else {
		gpio_set_value(N810_EMMC_VIO_GPIO, 0);
		msleep(50);
		gpio_set_value(N810_EMMC_VSD_GPIO, 0);
		msleep(50);
	}
}

static int n8x0_mmc_set_power(struct device *dev, int slot, int power_on,
			      int vdd)
{
	if (board_is_n800() || slot == 0)
		return n8x0_mmc_set_power_menelaus(dev, slot, power_on, vdd);

	n810_set_power_emmc(dev, power_on);

	return 0;
}

static int n8x0_mmc_set_bus_mode(struct device *dev, int slot, int bus_mode)
{
	int r;

	dev_dbg(dev, "Set slot %d bus mode %s\n", slot + 1,
		bus_mode == MMC_BUSMODE_OPENDRAIN ? "open-drain" : "push-pull");
	BUG_ON(slot != 0 && slot != 1);
	slot++;
	switch (bus_mode) {
	case MMC_BUSMODE_OPENDRAIN:
		r = menelaus_set_mmc_opendrain(slot, 1);
		break;
	case MMC_BUSMODE_PUSHPULL:
		r = menelaus_set_mmc_opendrain(slot, 0);
		break;
	default:
		BUG();
	}
	if (r != 0 && printk_ratelimit())
		dev_err(dev, "MMC: unable to set bus mode for slot %d\n",
			slot);
	return r;
}

static int n8x0_mmc_get_cover_state(struct device *dev, int slot)
{
	slot++;
	BUG_ON(slot != 1 && slot != 2);
	if (slot == 1)
		return slot1_cover_open;
	else
		return slot2_cover_open;
}

static void n8x0_mmc_callback(void *data, u8 card_mask)
{
#ifdef CONFIG_MMC_OMAP
	int bit, *openp, index;

	if (board_is_n800()) {
		bit = 1 << 1;
		openp = &slot2_cover_open;
		index = 1;
	} else {
		bit = 1;
		openp = &slot1_cover_open;
		index = 0;
	}

	if (card_mask & bit)
		*openp = 1;
	else
		*openp = 0;

	omap_mmc_notify_cover_event(mmc_device, index, *openp);
#else
	pr_warn("MMC: notify cover event not available\n");
#endif
}

static int n8x0_mmc_late_init(struct device *dev)
{
	int r, bit, *openp;
	int vs2sel;

	mmc_device = dev;

	r = menelaus_set_slot_sel(1);
	if (r < 0)
		return r;

	if (board_is_n800())
		vs2sel = 0;
	else
		vs2sel = 2;

	r = menelaus_set_mmc_slot(2, 0, vs2sel, 1);
	if (r < 0)
		return r;

	n8x0_mmc_set_power(dev, 0, MMC_POWER_ON, 16); /* MMC_VDD_28_29 */
	n8x0_mmc_set_power(dev, 1, MMC_POWER_ON, 16);

	r = menelaus_set_mmc_slot(1, 1, 0, 1);
	if (r < 0)
		return r;
	r = menelaus_set_mmc_slot(2, 1, vs2sel, 1);
	if (r < 0)
		return r;

	r = menelaus_get_slot_pin_states();
	if (r < 0)
		return r;

	if (board_is_n800()) {
		bit = 1 << 1;
		openp = &slot2_cover_open;
	} else {
		bit = 1;
		openp = &slot1_cover_open;
		slot2_cover_open = 0;
	}

	/* All slot pin bits seem to be inversed until first switch change */
	if (r == 0xf || r == (0xf & ~bit))
		r = ~r;

	if (r & bit)
		*openp = 1;
	else
		*openp = 0;

	r = menelaus_register_mmc_callback(n8x0_mmc_callback, NULL);

	return r;
}

static void n8x0_mmc_shutdown(struct device *dev)
{
	int vs2sel;

	if (board_is_n800())
		vs2sel = 0;
	else
		vs2sel = 2;

	menelaus_set_mmc_slot(1, 0, 0, 0);
	menelaus_set_mmc_slot(2, 0, vs2sel, 0);
}

static void n8x0_mmc_cleanup(struct device *dev)
{
	menelaus_unregister_mmc_callback();

	gpio_free(N8X0_SLOT_SWITCH_GPIO);

	if (board_is_n810()) {
		gpio_free(N810_EMMC_VSD_GPIO);
		gpio_free(N810_EMMC_VIO_GPIO);
	}
}

/*
 * MMC controller1 has two slots that are multiplexed via I2C.
 * MMC controller2 is not in use.
 */
static struct omap_mmc_platform_data mmc1_data = {
	.nr_slots			= 0,
	.switch_slot			= n8x0_mmc_switch_slot,
	.init				= n8x0_mmc_late_init,
	.cleanup			= n8x0_mmc_cleanup,
	.shutdown			= n8x0_mmc_shutdown,
	.max_freq			= 24000000,
	.slots[0] = {
		.wires			= 4,
		.set_power		= n8x0_mmc_set_power,
		.set_bus_mode		= n8x0_mmc_set_bus_mode,
		.get_cover_state	= n8x0_mmc_get_cover_state,
		.ocr_mask		= MMC_VDD_165_195 | MMC_VDD_30_31 |
						MMC_VDD_32_33   | MMC_VDD_33_34,
		.name			= "internal",
	},
	.slots[1] = {
		.set_power		= n8x0_mmc_set_power,
		.set_bus_mode		= n8x0_mmc_set_bus_mode,
		.get_cover_state	= n8x0_mmc_get_cover_state,
		.ocr_mask		= MMC_VDD_165_195 | MMC_VDD_20_21 |
						MMC_VDD_21_22 | MMC_VDD_22_23 |
						MMC_VDD_23_24 | MMC_VDD_24_25 |
						MMC_VDD_27_28 | MMC_VDD_28_29 |
						MMC_VDD_29_30 | MMC_VDD_30_31 |
						MMC_VDD_32_33 | MMC_VDD_33_34,
		.name			= "external",
	},
};

static struct omap_mmc_platform_data *mmc_data[OMAP24XX_NR_MMC];

static struct gpio n810_emmc_gpios[] __initdata = {
	{ N810_EMMC_VSD_GPIO, GPIOF_OUT_INIT_LOW,  "MMC slot 2 Vddf" },
	{ N810_EMMC_VIO_GPIO, GPIOF_OUT_INIT_LOW,  "MMC slot 2 Vdd"  },
};

static void __init n8x0_mmc_init(void)
{
	int err;

	if (board_is_n810()) {
		mmc1_data.slots[0].name = "external";

		/*
		 * Some Samsung Movinand chips do not like open-ended
		 * multi-block reads and fall to braind-dead state
		 * while doing so. Reducing the number of blocks in
		 * the transfer or delays in clock disable do not help
		 */
		mmc1_data.slots[1].name = "internal";
		mmc1_data.slots[1].ban_openended = 1;
	}

	err = gpio_request_one(N8X0_SLOT_SWITCH_GPIO, GPIOF_OUT_INIT_LOW,
			       "MMC slot switch");
	if (err)
		return;

	if (board_is_n810()) {
		err = gpio_request_array(n810_emmc_gpios,
					 ARRAY_SIZE(n810_emmc_gpios));
		if (err) {
			gpio_free(N8X0_SLOT_SWITCH_GPIO);
			return;
		}
	}

	mmc1_data.nr_slots = 2;
	mmc_data[0] = &mmc1_data;
}
#else
static struct omap_mmc_platform_data mmc1_data;
void __init n8x0_mmc_init(void)
{
}
#endif	/* CONFIG_MMC_OMAP */

#ifdef CONFIG_MENELAUS

static int n8x0_auto_sleep_regulators(void)
{
	u32 val;
	int ret;

	val = EN_VPLL_SLEEP | EN_VMMC_SLEEP    \
		| EN_VAUX_SLEEP | EN_VIO_SLEEP \
		| EN_VMEM_SLEEP | EN_DC3_SLEEP \
		| EN_VC_SLEEP | EN_DC2_SLEEP;

	ret = menelaus_set_regulator_sleep(1, val);
	if (ret < 0) {
		pr_err("Could not set regulators to sleep on menelaus: %u\n",
		       ret);
		return ret;
	}
	return 0;
}

static int n8x0_auto_voltage_scale(void)
{
	int ret;

	ret = menelaus_set_vcore_hw(1400, 1050);
	if (ret < 0) {
		pr_err("Could not set VCORE voltage on menelaus: %u\n", ret);
		return ret;
	}
	return 0;
}

static int n8x0_menelaus_late_init(struct device *dev)
{
	int ret;

	ret = n8x0_auto_voltage_scale();
	if (ret < 0)
		return ret;
	ret = n8x0_auto_sleep_regulators();
	if (ret < 0)
		return ret;
	return 0;
}

#else
static int n8x0_menelaus_late_init(struct device *dev)
{
	return 0;
}
#endif

struct menelaus_platform_data n8x0_menelaus_platform_data = {
	.late_init = n8x0_menelaus_late_init,
};

struct aic3x_pdata n810_aic33_data = {
	.gpio_reset = 118,
};

static int __init n8x0_late_initcall(void)
{
	if (!board_caps)
		return -ENODEV;

	n8x0_mmc_init();
	n8x0_usb_init();

	return 0;
}
omap_late_initcall(n8x0_late_initcall);

/*
 * Legacy init pdata init for n8x0. Note that we want to follow the
 * I2C bus numbering starting at 0 for device tree like other omaps.
 */
void * __init n8x0_legacy_init(void)
{
	board_check_revision();
	spi_register_board_info(n800_spi_board_info,
				ARRAY_SIZE(n800_spi_board_info));
	return &mmc1_data;
}
