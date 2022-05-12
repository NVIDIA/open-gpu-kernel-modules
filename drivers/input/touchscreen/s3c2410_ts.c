// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Samsung S3C24XX touchscreen driver
 *
 * Copyright 2004 Arnaud Patard <arnaud.patard@rtp-net.org>
 * Copyright 2008 Ben Dooks <ben-linux@fluff.org>
 * Copyright 2009 Simtec Electronics <linux@simtec.co.uk>
 *
 * Additional work by Herbert Pötzl <herbert@13thfloor.at> and
 * Harald Welte <laforge@openmoko.org>
 */

#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/io.h>

#include <linux/soc/samsung/s3c-adc.h>
#include <linux/platform_data/touchscreen-s3c2410.h>

#define	S3C2410_ADCCON			(0x00)
#define	S3C2410_ADCTSC			(0x04)
#define	S3C2410_ADCDLY			(0x08)
#define	S3C2410_ADCDAT0			(0x0C)
#define	S3C2410_ADCDAT1			(0x10)
#define	S3C64XX_ADCUPDN			(0x14)
#define	S3C2443_ADCMUX			(0x18)
#define	S3C64XX_ADCCLRINT		(0x18)
#define	S5P_ADCMUX			(0x1C)
#define	S3C64XX_ADCCLRINTPNDNUP		(0x20)

/* ADCTSC Register Bits */
#define S3C2443_ADCTSC_UD_SEN		(1 << 8)
#define S3C2410_ADCTSC_YM_SEN		(1<<7)
#define S3C2410_ADCTSC_YP_SEN		(1<<6)
#define S3C2410_ADCTSC_XM_SEN		(1<<5)
#define S3C2410_ADCTSC_XP_SEN		(1<<4)
#define S3C2410_ADCTSC_PULL_UP_DISABLE	(1<<3)
#define S3C2410_ADCTSC_AUTO_PST		(1<<2)
#define S3C2410_ADCTSC_XY_PST(x)	(((x)&0x3)<<0)

/* ADCDAT0 Bits */
#define S3C2410_ADCDAT0_UPDOWN		(1<<15)
#define S3C2410_ADCDAT0_AUTO_PST	(1<<14)
#define S3C2410_ADCDAT0_XY_PST		(0x3<<12)
#define S3C2410_ADCDAT0_XPDATA_MASK	(0x03FF)

/* ADCDAT1 Bits */
#define S3C2410_ADCDAT1_UPDOWN		(1<<15)
#define S3C2410_ADCDAT1_AUTO_PST	(1<<14)
#define S3C2410_ADCDAT1_XY_PST		(0x3<<12)
#define S3C2410_ADCDAT1_YPDATA_MASK	(0x03FF)


#define TSC_SLEEP  (S3C2410_ADCTSC_PULL_UP_DISABLE | S3C2410_ADCTSC_XY_PST(0))

#define INT_DOWN	(0)
#define INT_UP		(1 << 8)

#define WAIT4INT	(S3C2410_ADCTSC_YM_SEN | \
			 S3C2410_ADCTSC_YP_SEN | \
			 S3C2410_ADCTSC_XP_SEN | \
			 S3C2410_ADCTSC_XY_PST(3))

#define AUTOPST		(S3C2410_ADCTSC_YM_SEN | \
			 S3C2410_ADCTSC_YP_SEN | \
			 S3C2410_ADCTSC_XP_SEN | \
			 S3C2410_ADCTSC_AUTO_PST | \
			 S3C2410_ADCTSC_XY_PST(0))

#define FEAT_PEN_IRQ	(1 << 0)	/* HAS ADCCLRINTPNDNUP */

/* Per-touchscreen data. */

/**
 * struct s3c2410ts - driver touchscreen state.
 * @client: The ADC client we registered with the core driver.
 * @dev: The device we are bound to.
 * @input: The input device we registered with the input subsystem.
 * @clock: The clock for the adc.
 * @io: Pointer to the IO base.
 * @xp: The accumulated X position data.
 * @yp: The accumulated Y position data.
 * @irq_tc: The interrupt number for pen up/down interrupt
 * @count: The number of samples collected.
 * @shift: The log2 of the maximum count to read in one go.
 * @features: The features supported by the TSADC MOdule.
 */
struct s3c2410ts {
	struct s3c_adc_client *client;
	struct device *dev;
	struct input_dev *input;
	struct clk *clock;
	void __iomem *io;
	unsigned long xp;
	unsigned long yp;
	int irq_tc;
	int count;
	int shift;
	int features;
};

static struct s3c2410ts ts;

/**
 * get_down - return the down state of the pen
 * @data0: The data read from ADCDAT0 register.
 * @data1: The data read from ADCDAT1 register.
 *
 * Return non-zero if both readings show that the pen is down.
 */
static inline bool get_down(unsigned long data0, unsigned long data1)
{
	/* returns true if both data values show stylus down */
	return (!(data0 & S3C2410_ADCDAT0_UPDOWN) &&
		!(data1 & S3C2410_ADCDAT0_UPDOWN));
}

static void touch_timer_fire(struct timer_list *unused)
{
	unsigned long data0;
	unsigned long data1;
	bool down;

	data0 = readl(ts.io + S3C2410_ADCDAT0);
	data1 = readl(ts.io + S3C2410_ADCDAT1);

	down = get_down(data0, data1);

	if (down) {
		if (ts.count == (1 << ts.shift)) {
			ts.xp >>= ts.shift;
			ts.yp >>= ts.shift;

			dev_dbg(ts.dev, "%s: X=%lu, Y=%lu, count=%d\n",
				__func__, ts.xp, ts.yp, ts.count);

			input_report_abs(ts.input, ABS_X, ts.xp);
			input_report_abs(ts.input, ABS_Y, ts.yp);

			input_report_key(ts.input, BTN_TOUCH, 1);
			input_sync(ts.input);

			ts.xp = 0;
			ts.yp = 0;
			ts.count = 0;
		}

		s3c_adc_start(ts.client, 0, 1 << ts.shift);
	} else {
		ts.xp = 0;
		ts.yp = 0;
		ts.count = 0;

		input_report_key(ts.input, BTN_TOUCH, 0);
		input_sync(ts.input);

		writel(WAIT4INT | INT_DOWN, ts.io + S3C2410_ADCTSC);
	}
}

static DEFINE_TIMER(touch_timer, touch_timer_fire);

/**
 * stylus_irq - touchscreen stylus event interrupt
 * @irq: The interrupt number
 * @dev_id: The device ID.
 *
 * Called when the IRQ_TC is fired for a pen up or down event.
 */
static irqreturn_t stylus_irq(int irq, void *dev_id)
{
	unsigned long data0;
	unsigned long data1;
	bool down;

	data0 = readl(ts.io + S3C2410_ADCDAT0);
	data1 = readl(ts.io + S3C2410_ADCDAT1);

	down = get_down(data0, data1);

	/* TODO we should never get an interrupt with down set while
	 * the timer is running, but maybe we ought to verify that the
	 * timer isn't running anyways. */

	if (down)
		s3c_adc_start(ts.client, 0, 1 << ts.shift);
	else
		dev_dbg(ts.dev, "%s: count=%d\n", __func__, ts.count);

	if (ts.features & FEAT_PEN_IRQ) {
		/* Clear pen down/up interrupt */
		writel(0x0, ts.io + S3C64XX_ADCCLRINTPNDNUP);
	}

	return IRQ_HANDLED;
}

/**
 * s3c24xx_ts_conversion - ADC conversion callback
 * @client: The client that was registered with the ADC core.
 * @data0: The reading from ADCDAT0.
 * @data1: The reading from ADCDAT1.
 * @left: The number of samples left.
 *
 * Called when a conversion has finished.
 */
static void s3c24xx_ts_conversion(struct s3c_adc_client *client,
				  unsigned data0, unsigned data1,
				  unsigned *left)
{
	dev_dbg(ts.dev, "%s: %d,%d\n", __func__, data0, data1);

	ts.xp += data0;
	ts.yp += data1;

	ts.count++;

	/* From tests, it seems that it is unlikely to get a pen-up
	 * event during the conversion process which means we can
	 * ignore any pen-up events with less than the requisite
	 * count done.
	 *
	 * In several thousand conversions, no pen-ups where detected
	 * before count completed.
	 */
}

/**
 * s3c24xx_ts_select - ADC selection callback.
 * @client: The client that was registered with the ADC core.
 * @select: The reason for select.
 *
 * Called when the ADC core selects (or deslects) us as a client.
 */
static void s3c24xx_ts_select(struct s3c_adc_client *client, unsigned select)
{
	if (select) {
		writel(S3C2410_ADCTSC_PULL_UP_DISABLE | AUTOPST,
		       ts.io + S3C2410_ADCTSC);
	} else {
		mod_timer(&touch_timer, jiffies+1);
		writel(WAIT4INT | INT_UP, ts.io + S3C2410_ADCTSC);
	}
}

/**
 * s3c2410ts_probe - device core probe entry point
 * @pdev: The device we are being bound to.
 *
 * Initialise, find and allocate any resources we need to run and then
 * register with the ADC and input systems.
 */
static int s3c2410ts_probe(struct platform_device *pdev)
{
	struct s3c2410_ts_mach_info *info;
	struct device *dev = &pdev->dev;
	struct input_dev *input_dev;
	struct resource *res;
	int ret = -EINVAL;

	/* Initialise input stuff */
	memset(&ts, 0, sizeof(struct s3c2410ts));

	ts.dev = dev;

	info = dev_get_platdata(dev);
	if (!info) {
		dev_err(dev, "no platform data, cannot attach\n");
		return -EINVAL;
	}

	dev_dbg(dev, "initialising touchscreen\n");

	ts.clock = clk_get(dev, "adc");
	if (IS_ERR(ts.clock)) {
		dev_err(dev, "cannot get adc clock source\n");
		return -ENOENT;
	}

	ret = clk_prepare_enable(ts.clock);
	if (ret) {
		dev_err(dev, "Failed! to enabled clocks\n");
		goto err_clk_get;
	}
	dev_dbg(dev, "got and enabled clocks\n");

	ts.irq_tc = ret = platform_get_irq(pdev, 0);
	if (ret < 0) {
		dev_err(dev, "no resource for interrupt\n");
		goto err_clk;
	}

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(dev, "no resource for registers\n");
		ret = -ENOENT;
		goto err_clk;
	}

	ts.io = ioremap(res->start, resource_size(res));
	if (ts.io == NULL) {
		dev_err(dev, "cannot map registers\n");
		ret = -ENOMEM;
		goto err_clk;
	}

	/* inititalise the gpio */
	if (info->cfg_gpio)
		info->cfg_gpio(to_platform_device(ts.dev));

	ts.client = s3c_adc_register(pdev, s3c24xx_ts_select,
				     s3c24xx_ts_conversion, 1);
	if (IS_ERR(ts.client)) {
		dev_err(dev, "failed to register adc client\n");
		ret = PTR_ERR(ts.client);
		goto err_iomap;
	}

	/* Initialise registers */
	if ((info->delay & 0xffff) > 0)
		writel(info->delay & 0xffff, ts.io + S3C2410_ADCDLY);

	writel(WAIT4INT | INT_DOWN, ts.io + S3C2410_ADCTSC);

	input_dev = input_allocate_device();
	if (!input_dev) {
		dev_err(dev, "Unable to allocate the input device !!\n");
		ret = -ENOMEM;
		goto err_iomap;
	}

	ts.input = input_dev;
	ts.input->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
	ts.input->keybit[BIT_WORD(BTN_TOUCH)] = BIT_MASK(BTN_TOUCH);
	input_set_abs_params(ts.input, ABS_X, 0, 0x3FF, 0, 0);
	input_set_abs_params(ts.input, ABS_Y, 0, 0x3FF, 0, 0);

	ts.input->name = "S3C24XX TouchScreen";
	ts.input->id.bustype = BUS_HOST;
	ts.input->id.vendor = 0xDEAD;
	ts.input->id.product = 0xBEEF;
	ts.input->id.version = 0x0102;

	ts.shift = info->oversampling_shift;
	ts.features = platform_get_device_id(pdev)->driver_data;

	ret = request_irq(ts.irq_tc, stylus_irq, 0,
			  "s3c2410_ts_pen", ts.input);
	if (ret) {
		dev_err(dev, "cannot get TC interrupt\n");
		goto err_inputdev;
	}

	dev_info(dev, "driver attached, registering input device\n");

	/* All went ok, so register to the input system */
	ret = input_register_device(ts.input);
	if (ret < 0) {
		dev_err(dev, "failed to register input device\n");
		ret = -EIO;
		goto err_tcirq;
	}

	return 0;

 err_tcirq:
	free_irq(ts.irq_tc, ts.input);
 err_inputdev:
	input_free_device(ts.input);
 err_iomap:
	iounmap(ts.io);
 err_clk:
	clk_disable_unprepare(ts.clock);
	del_timer_sync(&touch_timer);
 err_clk_get:
	clk_put(ts.clock);
	return ret;
}

/**
 * s3c2410ts_remove - device core removal entry point
 * @pdev: The device we are being removed from.
 *
 * Free up our state ready to be removed.
 */
static int s3c2410ts_remove(struct platform_device *pdev)
{
	free_irq(ts.irq_tc, ts.input);
	del_timer_sync(&touch_timer);

	clk_disable_unprepare(ts.clock);
	clk_put(ts.clock);

	input_unregister_device(ts.input);
	iounmap(ts.io);

	return 0;
}

#ifdef CONFIG_PM
static int s3c2410ts_suspend(struct device *dev)
{
	writel(TSC_SLEEP, ts.io + S3C2410_ADCTSC);
	disable_irq(ts.irq_tc);
	clk_disable(ts.clock);

	return 0;
}

static int s3c2410ts_resume(struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct s3c2410_ts_mach_info *info = dev_get_platdata(&pdev->dev);

	clk_enable(ts.clock);
	enable_irq(ts.irq_tc);

	/* Initialise registers */
	if ((info->delay & 0xffff) > 0)
		writel(info->delay & 0xffff, ts.io + S3C2410_ADCDLY);

	writel(WAIT4INT | INT_DOWN, ts.io + S3C2410_ADCTSC);

	return 0;
}

static const struct dev_pm_ops s3c_ts_pmops = {
	.suspend	= s3c2410ts_suspend,
	.resume		= s3c2410ts_resume,
};
#endif

static const struct platform_device_id s3cts_driver_ids[] = {
	{ "s3c2410-ts", 0 },
	{ "s3c2440-ts", 0 },
	{ "s3c64xx-ts", FEAT_PEN_IRQ },
	{ }
};
MODULE_DEVICE_TABLE(platform, s3cts_driver_ids);

static struct platform_driver s3c_ts_driver = {
	.driver         = {
		.name   = "samsung-ts",
#ifdef CONFIG_PM
		.pm	= &s3c_ts_pmops,
#endif
	},
	.id_table	= s3cts_driver_ids,
	.probe		= s3c2410ts_probe,
	.remove		= s3c2410ts_remove,
};
module_platform_driver(s3c_ts_driver);

MODULE_AUTHOR("Arnaud Patard <arnaud.patard@rtp-net.org>, "
	      "Ben Dooks <ben@simtec.co.uk>, "
	      "Simtec Electronics <linux@simtec.co.uk>");
MODULE_DESCRIPTION("S3C24XX Touchscreen driver");
MODULE_LICENSE("GPL v2");
