// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * RNG driver for Freescale RNGC
 *
 * Copyright (C) 2008-2012 Freescale Semiconductor, Inc.
 * Copyright (C) 2017 Martin Kaiser <martin@kaiser.cx>
 */

#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/hw_random.h>
#include <linux/completion.h>
#include <linux/io.h>

#define RNGC_VER_ID			0x0000
#define RNGC_COMMAND			0x0004
#define RNGC_CONTROL			0x0008
#define RNGC_STATUS			0x000C
#define RNGC_ERROR			0x0010
#define RNGC_FIFO			0x0014

/* the fields in the ver id register */
#define RNGC_TYPE_SHIFT		28
#define RNGC_VER_MAJ_SHIFT		8

/* the rng_type field */
#define RNGC_TYPE_RNGB			0x1
#define RNGC_TYPE_RNGC			0x2


#define RNGC_CMD_CLR_ERR		0x00000020
#define RNGC_CMD_CLR_INT		0x00000010
#define RNGC_CMD_SEED			0x00000002
#define RNGC_CMD_SELF_TEST		0x00000001

#define RNGC_CTRL_MASK_ERROR		0x00000040
#define RNGC_CTRL_MASK_DONE		0x00000020
#define RNGC_CTRL_AUTO_SEED		0x00000010

#define RNGC_STATUS_ERROR		0x00010000
#define RNGC_STATUS_FIFO_LEVEL_MASK	0x00000f00
#define RNGC_STATUS_FIFO_LEVEL_SHIFT	8
#define RNGC_STATUS_SEED_DONE		0x00000020
#define RNGC_STATUS_ST_DONE		0x00000010

#define RNGC_ERROR_STATUS_STAT_ERR	0x00000008

#define RNGC_TIMEOUT  3000 /* 3 sec */


static bool self_test = true;
module_param(self_test, bool, 0);

struct imx_rngc {
	struct device		*dev;
	struct clk		*clk;
	void __iomem		*base;
	struct hwrng		rng;
	struct completion	rng_op_done;
	/*
	 * err_reg is written only by the irq handler and read only
	 * when interrupts are masked, we need no spinlock
	 */
	u32			err_reg;
};


static inline void imx_rngc_irq_mask_clear(struct imx_rngc *rngc)
{
	u32 ctrl, cmd;

	/* mask interrupts */
	ctrl = readl(rngc->base + RNGC_CONTROL);
	ctrl |= RNGC_CTRL_MASK_DONE | RNGC_CTRL_MASK_ERROR;
	writel(ctrl, rngc->base + RNGC_CONTROL);

	/*
	 * CLR_INT clears the interrupt only if there's no error
	 * CLR_ERR clear the interrupt and the error register if there
	 * is an error
	 */
	cmd = readl(rngc->base + RNGC_COMMAND);
	cmd |= RNGC_CMD_CLR_INT | RNGC_CMD_CLR_ERR;
	writel(cmd, rngc->base + RNGC_COMMAND);
}

static inline void imx_rngc_irq_unmask(struct imx_rngc *rngc)
{
	u32 ctrl;

	ctrl = readl(rngc->base + RNGC_CONTROL);
	ctrl &= ~(RNGC_CTRL_MASK_DONE | RNGC_CTRL_MASK_ERROR);
	writel(ctrl, rngc->base + RNGC_CONTROL);
}

static int imx_rngc_self_test(struct imx_rngc *rngc)
{
	u32 cmd;
	int ret;

	imx_rngc_irq_unmask(rngc);

	/* run self test */
	cmd = readl(rngc->base + RNGC_COMMAND);
	writel(cmd | RNGC_CMD_SELF_TEST, rngc->base + RNGC_COMMAND);

	ret = wait_for_completion_timeout(&rngc->rng_op_done, RNGC_TIMEOUT);
	imx_rngc_irq_mask_clear(rngc);
	if (!ret)
		return -ETIMEDOUT;

	return rngc->err_reg ? -EIO : 0;
}

static int imx_rngc_read(struct hwrng *rng, void *data, size_t max, bool wait)
{
	struct imx_rngc *rngc = container_of(rng, struct imx_rngc, rng);
	unsigned int status;
	unsigned int level;
	int retval = 0;

	while (max >= sizeof(u32)) {
		status = readl(rngc->base + RNGC_STATUS);

		/* is there some error while reading this random number? */
		if (status & RNGC_STATUS_ERROR)
			break;

		/* how many random numbers are in FIFO? [0-16] */
		level = (status & RNGC_STATUS_FIFO_LEVEL_MASK) >>
			RNGC_STATUS_FIFO_LEVEL_SHIFT;

		if (level) {
			/* retrieve a random number from FIFO */
			*(u32 *)data = readl(rngc->base + RNGC_FIFO);

			retval += sizeof(u32);
			data += sizeof(u32);
			max -= sizeof(u32);
		}
	}

	return retval ? retval : -EIO;
}

static irqreturn_t imx_rngc_irq(int irq, void *priv)
{
	struct imx_rngc *rngc = (struct imx_rngc *)priv;
	u32 status;

	/*
	 * clearing the interrupt will also clear the error register
	 * read error and status before clearing
	 */
	status = readl(rngc->base + RNGC_STATUS);
	rngc->err_reg = readl(rngc->base + RNGC_ERROR);

	imx_rngc_irq_mask_clear(rngc);

	if (status & (RNGC_STATUS_SEED_DONE | RNGC_STATUS_ST_DONE))
		complete(&rngc->rng_op_done);

	return IRQ_HANDLED;
}

static int imx_rngc_init(struct hwrng *rng)
{
	struct imx_rngc *rngc = container_of(rng, struct imx_rngc, rng);
	u32 cmd, ctrl;
	int ret;

	/* clear error */
	cmd = readl(rngc->base + RNGC_COMMAND);
	writel(cmd | RNGC_CMD_CLR_ERR, rngc->base + RNGC_COMMAND);

	imx_rngc_irq_unmask(rngc);

	/* create seed, repeat while there is some statistical error */
	do {
		/* seed creation */
		cmd = readl(rngc->base + RNGC_COMMAND);
		writel(cmd | RNGC_CMD_SEED, rngc->base + RNGC_COMMAND);

		ret = wait_for_completion_timeout(&rngc->rng_op_done,
				RNGC_TIMEOUT);

		if (!ret) {
			ret = -ETIMEDOUT;
			goto err;
		}

	} while (rngc->err_reg == RNGC_ERROR_STATUS_STAT_ERR);

	if (rngc->err_reg) {
		ret = -EIO;
		goto err;
	}

	/*
	 * enable automatic seeding, the rngc creates a new seed automatically
	 * after serving 2^20 random 160-bit words
	 */
	ctrl = readl(rngc->base + RNGC_CONTROL);
	ctrl |= RNGC_CTRL_AUTO_SEED;
	writel(ctrl, rngc->base + RNGC_CONTROL);

	/*
	 * if initialisation was successful, we keep the interrupt
	 * unmasked until imx_rngc_cleanup is called
	 * we mask the interrupt ourselves if we return an error
	 */
	return 0;

err:
	imx_rngc_irq_mask_clear(rngc);
	return ret;
}

static void imx_rngc_cleanup(struct hwrng *rng)
{
	struct imx_rngc *rngc = container_of(rng, struct imx_rngc, rng);

	imx_rngc_irq_mask_clear(rngc);
}

static int imx_rngc_probe(struct platform_device *pdev)
{
	struct imx_rngc *rngc;
	int ret;
	int irq;
	u32 ver_id;
	u8  rng_type;

	rngc = devm_kzalloc(&pdev->dev, sizeof(*rngc), GFP_KERNEL);
	if (!rngc)
		return -ENOMEM;

	rngc->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(rngc->base))
		return PTR_ERR(rngc->base);

	rngc->clk = devm_clk_get(&pdev->dev, NULL);
	if (IS_ERR(rngc->clk)) {
		dev_err(&pdev->dev, "Can not get rng_clk\n");
		return PTR_ERR(rngc->clk);
	}

	irq = platform_get_irq(pdev, 0);
	if (irq < 0)
		return irq;

	ret = clk_prepare_enable(rngc->clk);
	if (ret)
		return ret;

	ver_id = readl(rngc->base + RNGC_VER_ID);
	rng_type = ver_id >> RNGC_TYPE_SHIFT;
	/*
	 * This driver supports only RNGC and RNGB. (There's a different
	 * driver for RNGA.)
	 */
	if (rng_type != RNGC_TYPE_RNGC && rng_type != RNGC_TYPE_RNGB) {
		ret = -ENODEV;
		goto err;
	}

	ret = devm_request_irq(&pdev->dev,
			irq, imx_rngc_irq, 0, pdev->name, (void *)rngc);
	if (ret) {
		dev_err(rngc->dev, "Can't get interrupt working.\n");
		goto err;
	}

	init_completion(&rngc->rng_op_done);

	rngc->rng.name = pdev->name;
	rngc->rng.init = imx_rngc_init;
	rngc->rng.read = imx_rngc_read;
	rngc->rng.cleanup = imx_rngc_cleanup;
	rngc->rng.quality = 19;

	rngc->dev = &pdev->dev;
	platform_set_drvdata(pdev, rngc);

	imx_rngc_irq_mask_clear(rngc);

	if (self_test) {
		ret = imx_rngc_self_test(rngc);
		if (ret) {
			dev_err(rngc->dev, "self test failed\n");
			goto err;
		}
	}

	ret = hwrng_register(&rngc->rng);
	if (ret) {
		dev_err(&pdev->dev, "hwrng registration failed\n");
		goto err;
	}

	dev_info(&pdev->dev,
		"Freescale RNG%c registered (HW revision %d.%02d)\n",
		rng_type == RNGC_TYPE_RNGB ? 'B' : 'C',
		(ver_id >> RNGC_VER_MAJ_SHIFT) & 0xff, ver_id & 0xff);
	return 0;

err:
	clk_disable_unprepare(rngc->clk);

	return ret;
}

static int __exit imx_rngc_remove(struct platform_device *pdev)
{
	struct imx_rngc *rngc = platform_get_drvdata(pdev);

	hwrng_unregister(&rngc->rng);

	clk_disable_unprepare(rngc->clk);

	return 0;
}

static int __maybe_unused imx_rngc_suspend(struct device *dev)
{
	struct imx_rngc *rngc = dev_get_drvdata(dev);

	clk_disable_unprepare(rngc->clk);

	return 0;
}

static int __maybe_unused imx_rngc_resume(struct device *dev)
{
	struct imx_rngc *rngc = dev_get_drvdata(dev);

	clk_prepare_enable(rngc->clk);

	return 0;
}

static SIMPLE_DEV_PM_OPS(imx_rngc_pm_ops, imx_rngc_suspend, imx_rngc_resume);

static const struct of_device_id imx_rngc_dt_ids[] = {
	{ .compatible = "fsl,imx25-rngb", .data = NULL, },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, imx_rngc_dt_ids);

static struct platform_driver imx_rngc_driver = {
	.driver = {
		.name = "imx_rngc",
		.pm = &imx_rngc_pm_ops,
		.of_match_table = imx_rngc_dt_ids,
	},
	.remove = __exit_p(imx_rngc_remove),
};

module_platform_driver_probe(imx_rngc_driver, imx_rngc_probe);

MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_DESCRIPTION("H/W RNGC driver for i.MX");
MODULE_LICENSE("GPL");
