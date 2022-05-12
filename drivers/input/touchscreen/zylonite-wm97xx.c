// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * zylonite-wm97xx.c  --  Zylonite Continuous Touch screen driver
 *
 * Copyright 2004, 2007, 2008 Wolfson Microelectronics PLC.
 * Author: Mark Brown <broonie@opensource.wolfsonmicro.com>
 * Parts Copyright : Ian Molton <spyro@f2s.com>
 *                   Andrew Zabolotny <zap@homelink.ru>
 *
 * Notes:
 *     This is a wm97xx extended touch driver supporting interrupt driven
 *     and continuous operation on Marvell Zylonite development systems
 *     (which have a WM9713 on board).
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/wm97xx.h>

#include <mach/hardware.h>
#include <mach/mfp.h>
#include <mach/regs-ac97.h>

struct continuous {
	u16 id;    /* codec id */
	u8 code;   /* continuous code */
	u8 reads;  /* number of coord reads per read cycle */
	u32 speed; /* number of coords per second */
};

#define WM_READS(sp) ((sp / HZ) + 1)

static const struct continuous cinfo[] = {
	{ WM9713_ID2, 0, WM_READS(94),  94  },
	{ WM9713_ID2, 1, WM_READS(120), 120 },
	{ WM9713_ID2, 2, WM_READS(154), 154 },
	{ WM9713_ID2, 3, WM_READS(188), 188 },
};

/* continuous speed index */
static int sp_idx;

/*
 * Pen sampling frequency (Hz) in continuous mode.
 */
static int cont_rate = 200;
module_param(cont_rate, int, 0);
MODULE_PARM_DESC(cont_rate, "Sampling rate in continuous mode (Hz)");

/*
 * Pressure readback.
 *
 * Set to 1 to read back pen down pressure
 */
static int pressure;
module_param(pressure, int, 0);
MODULE_PARM_DESC(pressure, "Pressure readback (1 = pressure, 0 = no pressure)");

/*
 * AC97 touch data slot.
 *
 * Touch screen readback data ac97 slot
 */
static int ac97_touch_slot = 5;
module_param(ac97_touch_slot, int, 0);
MODULE_PARM_DESC(ac97_touch_slot, "Touch screen data slot AC97 number");


/* flush AC97 slot 5 FIFO machines */
static void wm97xx_acc_pen_up(struct wm97xx *wm)
{
	int i;

	msleep(1);

	for (i = 0; i < 16; i++)
		MODR;
}

static int wm97xx_acc_pen_down(struct wm97xx *wm)
{
	u16 x, y, p = 0x100 | WM97XX_ADCSEL_PRES;
	int reads = 0;
	static u16 last, tries;

	/* When the AC97 queue has been drained we need to allow time
	 * to buffer up samples otherwise we end up spinning polling
	 * for samples.  The controller can't have a suitably low
	 * threshold set to use the notifications it gives.
	 */
	msleep(1);

	if (tries > 5) {
		tries = 0;
		return RC_PENUP;
	}

	x = MODR;
	if (x == last) {
		tries++;
		return RC_AGAIN;
	}
	last = x;
	do {
		if (reads)
			x = MODR;
		y = MODR;
		if (pressure)
			p = MODR;

		dev_dbg(wm->dev, "Raw coordinates: x=%x, y=%x, p=%x\n",
			x, y, p);

		/* are samples valid */
		if ((x & WM97XX_ADCSEL_MASK) != WM97XX_ADCSEL_X ||
		    (y & WM97XX_ADCSEL_MASK) != WM97XX_ADCSEL_Y ||
		    (p & WM97XX_ADCSEL_MASK) != WM97XX_ADCSEL_PRES)
			goto up;

		/* coordinate is good */
		tries = 0;
		input_report_abs(wm->input_dev, ABS_X, x & 0xfff);
		input_report_abs(wm->input_dev, ABS_Y, y & 0xfff);
		input_report_abs(wm->input_dev, ABS_PRESSURE, p & 0xfff);
		input_report_key(wm->input_dev, BTN_TOUCH, (p != 0));
		input_sync(wm->input_dev);
		reads++;
	} while (reads < cinfo[sp_idx].reads);
up:
	return RC_PENDOWN | RC_AGAIN;
}

static int wm97xx_acc_startup(struct wm97xx *wm)
{
	int idx;

	/* check we have a codec */
	if (wm->ac97 == NULL)
		return -ENODEV;

	/* Go you big red fire engine */
	for (idx = 0; idx < ARRAY_SIZE(cinfo); idx++) {
		if (wm->id != cinfo[idx].id)
			continue;
		sp_idx = idx;
		if (cont_rate <= cinfo[idx].speed)
			break;
	}
	wm->acc_rate = cinfo[sp_idx].code;
	wm->acc_slot = ac97_touch_slot;
	dev_info(wm->dev,
		 "zylonite accelerated touchscreen driver, %d samples/sec\n",
		 cinfo[sp_idx].speed);

	return 0;
}

static void wm97xx_irq_enable(struct wm97xx *wm, int enable)
{
	if (enable)
		enable_irq(wm->pen_irq);
	else
		disable_irq_nosync(wm->pen_irq);
}

static struct wm97xx_mach_ops zylonite_mach_ops = {
	.acc_enabled	= 1,
	.acc_pen_up	= wm97xx_acc_pen_up,
	.acc_pen_down	= wm97xx_acc_pen_down,
	.acc_startup	= wm97xx_acc_startup,
	.irq_enable	= wm97xx_irq_enable,
	.irq_gpio	= WM97XX_GPIO_2,
};

static int zylonite_wm97xx_probe(struct platform_device *pdev)
{
	struct wm97xx *wm = platform_get_drvdata(pdev);
	int gpio_touch_irq;

	if (cpu_is_pxa320())
		gpio_touch_irq = mfp_to_gpio(MFP_PIN_GPIO15);
	else
		gpio_touch_irq = mfp_to_gpio(MFP_PIN_GPIO26);

	wm->pen_irq = gpio_to_irq(gpio_touch_irq);
	irq_set_irq_type(wm->pen_irq, IRQ_TYPE_EDGE_BOTH);

	wm97xx_config_gpio(wm, WM97XX_GPIO_13, WM97XX_GPIO_IN,
			   WM97XX_GPIO_POL_HIGH,
			   WM97XX_GPIO_STICKY,
			   WM97XX_GPIO_WAKE);
	wm97xx_config_gpio(wm, WM97XX_GPIO_2, WM97XX_GPIO_OUT,
			   WM97XX_GPIO_POL_HIGH,
			   WM97XX_GPIO_NOTSTICKY,
			   WM97XX_GPIO_NOWAKE);

	return wm97xx_register_mach_ops(wm, &zylonite_mach_ops);
}

static int zylonite_wm97xx_remove(struct platform_device *pdev)
{
	struct wm97xx *wm = platform_get_drvdata(pdev);

	wm97xx_unregister_mach_ops(wm);

	return 0;
}

static struct platform_driver zylonite_wm97xx_driver = {
	.probe	= zylonite_wm97xx_probe,
	.remove	= zylonite_wm97xx_remove,
	.driver	= {
		.name	= "wm97xx-touch",
	},
};
module_platform_driver(zylonite_wm97xx_driver);

/* Module information */
MODULE_AUTHOR("Mark Brown <broonie@opensource.wolfsonmicro.com>");
MODULE_DESCRIPTION("wm97xx continuous touch driver for Zylonite");
MODULE_LICENSE("GPL");
