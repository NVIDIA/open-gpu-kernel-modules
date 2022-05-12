// SPDX-License-Identifier: GPL-2.0-only
/*
 * PIC32 RNG driver
 *
 * Joshua Henderson <joshua.henderson@microchip.com>
 * Copyright (C) 2016 Microchip Technology Inc.  All rights reserved.
 */

#include <linux/clk.h>
#include <linux/clkdev.h>
#include <linux/err.h>
#include <linux/hw_random.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#define RNGCON		0x04
#define  TRNGEN		BIT(8)
#define  PRNGEN		BIT(9)
#define  PRNGCONT	BIT(10)
#define  TRNGMOD	BIT(11)
#define  SEEDLOAD	BIT(12)
#define RNGPOLY1	0x08
#define RNGPOLY2	0x0C
#define RNGNUMGEN1	0x10
#define RNGNUMGEN2	0x14
#define RNGSEED1	0x18
#define RNGSEED2	0x1C
#define RNGRCNT		0x20
#define  RCNT_MASK	0x7F

struct pic32_rng {
	void __iomem	*base;
	struct hwrng	rng;
	struct clk	*clk;
};

/*
 * The TRNG can generate up to 24Mbps. This is a timeout that should be safe
 * enough given the instructions in the loop and that the TRNG may not always
 * be at maximum rate.
 */
#define RNG_TIMEOUT 500

static int pic32_rng_read(struct hwrng *rng, void *buf, size_t max,
			  bool wait)
{
	struct pic32_rng *priv = container_of(rng, struct pic32_rng, rng);
	u64 *data = buf;
	u32 t;
	unsigned int timeout = RNG_TIMEOUT;

	do {
		t = readl(priv->base + RNGRCNT) & RCNT_MASK;
		if (t == 64) {
			/* TRNG value comes through the seed registers */
			*data = ((u64)readl(priv->base + RNGSEED2) << 32) +
				readl(priv->base + RNGSEED1);
			return 8;
		}
	} while (wait && --timeout);

	return -EIO;
}

static int pic32_rng_probe(struct platform_device *pdev)
{
	struct pic32_rng *priv;
	u32 v;
	int ret;

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(priv->base))
		return PTR_ERR(priv->base);

	priv->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(priv->clk))
		return PTR_ERR(priv->clk);

	ret = clk_prepare_enable(priv->clk);
	if (ret)
		return ret;

	/* enable TRNG in enhanced mode */
	v = TRNGEN | TRNGMOD;
	writel(v, priv->base + RNGCON);

	priv->rng.name = pdev->name;
	priv->rng.read = pic32_rng_read;

	ret = devm_hwrng_register(&pdev->dev, &priv->rng);
	if (ret)
		goto err_register;

	platform_set_drvdata(pdev, priv);

	return 0;

err_register:
	clk_disable_unprepare(priv->clk);
	return ret;
}

static int pic32_rng_remove(struct platform_device *pdev)
{
	struct pic32_rng *rng = platform_get_drvdata(pdev);

	writel(0, rng->base + RNGCON);
	clk_disable_unprepare(rng->clk);
	return 0;
}

static const struct of_device_id pic32_rng_of_match[] __maybe_unused = {
	{ .compatible	= "microchip,pic32mzda-rng", },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, pic32_rng_of_match);

static struct platform_driver pic32_rng_driver = {
	.probe		= pic32_rng_probe,
	.remove		= pic32_rng_remove,
	.driver		= {
		.name	= "pic32-rng",
		.of_match_table = of_match_ptr(pic32_rng_of_match),
	},
};

module_platform_driver(pic32_rng_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Joshua Henderson <joshua.henderson@microchip.com>");
MODULE_DESCRIPTION("Microchip PIC32 RNG Driver");
