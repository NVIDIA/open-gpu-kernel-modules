// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * RNG driver for Freescale RNGA
 *
 * Copyright 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 * Author: Alan Carvalho de Assis <acassis@gmail.com>
 */

/*
 *
 * This driver is based on other RNG drivers.
 */

#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/hw_random.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>

/* RNGA Registers */
#define RNGA_CONTROL			0x00
#define RNGA_STATUS			0x04
#define RNGA_ENTROPY			0x08
#define RNGA_OUTPUT_FIFO		0x0c
#define RNGA_MODE			0x10
#define RNGA_VERIFICATION_CONTROL	0x14
#define RNGA_OSC_CONTROL_COUNTER	0x18
#define RNGA_OSC1_COUNTER		0x1c
#define RNGA_OSC2_COUNTER		0x20
#define RNGA_OSC_COUNTER_STATUS		0x24

/* RNGA Registers Range */
#define RNG_ADDR_RANGE			0x28

/* RNGA Control Register */
#define RNGA_CONTROL_SLEEP		0x00000010
#define RNGA_CONTROL_CLEAR_INT		0x00000008
#define RNGA_CONTROL_MASK_INTS		0x00000004
#define RNGA_CONTROL_HIGH_ASSURANCE	0x00000002
#define RNGA_CONTROL_GO			0x00000001

#define RNGA_STATUS_LEVEL_MASK		0x0000ff00

/* RNGA Status Register */
#define RNGA_STATUS_OSC_DEAD		0x80000000
#define RNGA_STATUS_SLEEP		0x00000010
#define RNGA_STATUS_ERROR_INT		0x00000008
#define RNGA_STATUS_FIFO_UNDERFLOW	0x00000004
#define RNGA_STATUS_LAST_READ_STATUS	0x00000002
#define RNGA_STATUS_SECURITY_VIOLATION	0x00000001

struct mxc_rng {
	struct device *dev;
	struct hwrng rng;
	void __iomem *mem;
	struct clk *clk;
};

static int mxc_rnga_data_present(struct hwrng *rng, int wait)
{
	int i;
	struct mxc_rng *mxc_rng = container_of(rng, struct mxc_rng, rng);

	for (i = 0; i < 20; i++) {
		/* how many random numbers are in FIFO? [0-16] */
		int level = (__raw_readl(mxc_rng->mem + RNGA_STATUS) &
				RNGA_STATUS_LEVEL_MASK) >> 8;
		if (level || !wait)
			return !!level;
		udelay(10);
	}
	return 0;
}

static int mxc_rnga_data_read(struct hwrng *rng, u32 * data)
{
	int err;
	u32 ctrl;
	struct mxc_rng *mxc_rng = container_of(rng, struct mxc_rng, rng);

	/* retrieve a random number from FIFO */
	*data = __raw_readl(mxc_rng->mem + RNGA_OUTPUT_FIFO);

	/* some error while reading this random number? */
	err = __raw_readl(mxc_rng->mem + RNGA_STATUS) & RNGA_STATUS_ERROR_INT;

	/* if error: clear error interrupt, but doesn't return random number */
	if (err) {
		dev_dbg(mxc_rng->dev, "Error while reading random number!\n");
		ctrl = __raw_readl(mxc_rng->mem + RNGA_CONTROL);
		__raw_writel(ctrl | RNGA_CONTROL_CLEAR_INT,
					mxc_rng->mem + RNGA_CONTROL);
		return 0;
	} else
		return 4;
}

static int mxc_rnga_init(struct hwrng *rng)
{
	u32 ctrl, osc;
	struct mxc_rng *mxc_rng = container_of(rng, struct mxc_rng, rng);

	/* wake up */
	ctrl = __raw_readl(mxc_rng->mem + RNGA_CONTROL);
	__raw_writel(ctrl & ~RNGA_CONTROL_SLEEP, mxc_rng->mem + RNGA_CONTROL);

	/* verify if oscillator is working */
	osc = __raw_readl(mxc_rng->mem + RNGA_STATUS);
	if (osc & RNGA_STATUS_OSC_DEAD) {
		dev_err(mxc_rng->dev, "RNGA Oscillator is dead!\n");
		return -ENODEV;
	}

	/* go running */
	ctrl = __raw_readl(mxc_rng->mem + RNGA_CONTROL);
	__raw_writel(ctrl | RNGA_CONTROL_GO, mxc_rng->mem + RNGA_CONTROL);

	return 0;
}

static void mxc_rnga_cleanup(struct hwrng *rng)
{
	u32 ctrl;
	struct mxc_rng *mxc_rng = container_of(rng, struct mxc_rng, rng);

	ctrl = __raw_readl(mxc_rng->mem + RNGA_CONTROL);

	/* stop rnga */
	__raw_writel(ctrl & ~RNGA_CONTROL_GO, mxc_rng->mem + RNGA_CONTROL);
}

static int __init mxc_rnga_probe(struct platform_device *pdev)
{
	int err;
	struct mxc_rng *mxc_rng;

	mxc_rng = devm_kzalloc(&pdev->dev, sizeof(*mxc_rng), GFP_KERNEL);
	if (!mxc_rng)
		return -ENOMEM;

	mxc_rng->dev = &pdev->dev;
	mxc_rng->rng.name = "mxc-rnga";
	mxc_rng->rng.init = mxc_rnga_init;
	mxc_rng->rng.cleanup = mxc_rnga_cleanup;
	mxc_rng->rng.data_present = mxc_rnga_data_present;
	mxc_rng->rng.data_read = mxc_rnga_data_read;

	mxc_rng->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(mxc_rng->clk)) {
		dev_err(&pdev->dev, "Could not get rng_clk!\n");
		return PTR_ERR(mxc_rng->clk);
	}

	err = clk_prepare_enable(mxc_rng->clk);
	if (err)
		return err;

	mxc_rng->mem = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(mxc_rng->mem)) {
		err = PTR_ERR(mxc_rng->mem);
		goto err_ioremap;
	}

	err = hwrng_register(&mxc_rng->rng);
	if (err) {
		dev_err(&pdev->dev, "MXC RNGA registering failed (%d)\n", err);
		goto err_ioremap;
	}

	return 0;

err_ioremap:
	clk_disable_unprepare(mxc_rng->clk);
	return err;
}

static int __exit mxc_rnga_remove(struct platform_device *pdev)
{
	struct mxc_rng *mxc_rng = platform_get_drvdata(pdev);

	hwrng_unregister(&mxc_rng->rng);

	clk_disable_unprepare(mxc_rng->clk);

	return 0;
}

static const struct of_device_id mxc_rnga_of_match[] = {
	{ .compatible = "fsl,imx21-rnga", },
	{ .compatible = "fsl,imx31-rnga", },
	{ /* sentinel */ },
};
MODULE_DEVICE_TABLE(of, mxc_rnga_of_match);

static struct platform_driver mxc_rnga_driver = {
	.driver = {
		.name = "mxc_rnga",
		.of_match_table = mxc_rnga_of_match,
	},
	.remove = __exit_p(mxc_rnga_remove),
};

module_platform_driver_probe(mxc_rnga_driver, mxc_rnga_probe);

MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_DESCRIPTION("H/W RNGA driver for i.MX");
MODULE_LICENSE("GPL");
