// SPDX-License-Identifier: GPL-2.0-only
/*
 * SPI controller driver for the Atheros AR71XX/AR724X/AR913X SoCs
 *
 * Copyright (C) 2009-2011 Gabor Juhos <juhosg@openwrt.org>
 *
 * This driver has been based on the spi-gpio.c:
 *	Copyright (C) 2006,2008 David Brownell
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/spi/spi.h>
#include <linux/spi/spi_bitbang.h>
#include <linux/bitops.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/platform_data/spi-ath79.h>

#define DRV_NAME	"ath79-spi"

#define ATH79_SPI_RRW_DELAY_FACTOR	12000
#define MHZ				(1000 * 1000)

#define AR71XX_SPI_REG_FS		0x00	/* Function Select */
#define AR71XX_SPI_REG_CTRL		0x04	/* SPI Control */
#define AR71XX_SPI_REG_IOC		0x08	/* SPI I/O Control */
#define AR71XX_SPI_REG_RDS		0x0c	/* Read Data Shift */

#define AR71XX_SPI_FS_GPIO		BIT(0)	/* Enable GPIO mode */

#define AR71XX_SPI_IOC_DO		BIT(0)	/* Data Out pin */
#define AR71XX_SPI_IOC_CLK		BIT(8)	/* CLK pin */
#define AR71XX_SPI_IOC_CS(n)		BIT(16 + (n))

struct ath79_spi {
	struct spi_bitbang	bitbang;
	u32			ioc_base;
	u32			reg_ctrl;
	void __iomem		*base;
	struct clk		*clk;
	unsigned int		rrw_delay;
};

static inline u32 ath79_spi_rr(struct ath79_spi *sp, unsigned int reg)
{
	return ioread32(sp->base + reg);
}

static inline void ath79_spi_wr(struct ath79_spi *sp, unsigned int reg, u32 val)
{
	iowrite32(val, sp->base + reg);
}

static inline struct ath79_spi *ath79_spidev_to_sp(struct spi_device *spi)
{
	return spi_master_get_devdata(spi->master);
}

static inline void ath79_spi_delay(struct ath79_spi *sp, unsigned int nsecs)
{
	if (nsecs > sp->rrw_delay)
		ndelay(nsecs - sp->rrw_delay);
}

static void ath79_spi_chipselect(struct spi_device *spi, int is_active)
{
	struct ath79_spi *sp = ath79_spidev_to_sp(spi);
	int cs_high = (spi->mode & SPI_CS_HIGH) ? is_active : !is_active;
	u32 cs_bit = AR71XX_SPI_IOC_CS(spi->chip_select);

	if (cs_high)
		sp->ioc_base |= cs_bit;
	else
		sp->ioc_base &= ~cs_bit;

	ath79_spi_wr(sp, AR71XX_SPI_REG_IOC, sp->ioc_base);
}

static void ath79_spi_enable(struct ath79_spi *sp)
{
	/* enable GPIO mode */
	ath79_spi_wr(sp, AR71XX_SPI_REG_FS, AR71XX_SPI_FS_GPIO);

	/* save CTRL register */
	sp->reg_ctrl = ath79_spi_rr(sp, AR71XX_SPI_REG_CTRL);
	sp->ioc_base = ath79_spi_rr(sp, AR71XX_SPI_REG_IOC);

	/* clear clk and mosi in the base state */
	sp->ioc_base &= ~(AR71XX_SPI_IOC_DO | AR71XX_SPI_IOC_CLK);

	/* TODO: setup speed? */
	ath79_spi_wr(sp, AR71XX_SPI_REG_CTRL, 0x43);
}

static void ath79_spi_disable(struct ath79_spi *sp)
{
	/* restore CTRL register */
	ath79_spi_wr(sp, AR71XX_SPI_REG_CTRL, sp->reg_ctrl);
	/* disable GPIO mode */
	ath79_spi_wr(sp, AR71XX_SPI_REG_FS, 0);
}

static u32 ath79_spi_txrx_mode0(struct spi_device *spi, unsigned int nsecs,
			       u32 word, u8 bits, unsigned flags)
{
	struct ath79_spi *sp = ath79_spidev_to_sp(spi);
	u32 ioc = sp->ioc_base;

	/* clock starts at inactive polarity */
	for (word <<= (32 - bits); likely(bits); bits--) {
		u32 out;

		if (word & (1 << 31))
			out = ioc | AR71XX_SPI_IOC_DO;
		else
			out = ioc & ~AR71XX_SPI_IOC_DO;

		/* setup MSB (to slave) on trailing edge */
		ath79_spi_wr(sp, AR71XX_SPI_REG_IOC, out);
		ath79_spi_delay(sp, nsecs);
		ath79_spi_wr(sp, AR71XX_SPI_REG_IOC, out | AR71XX_SPI_IOC_CLK);
		ath79_spi_delay(sp, nsecs);
		if (bits == 1)
			ath79_spi_wr(sp, AR71XX_SPI_REG_IOC, out);

		word <<= 1;
	}

	return ath79_spi_rr(sp, AR71XX_SPI_REG_RDS);
}

static int ath79_spi_probe(struct platform_device *pdev)
{
	struct spi_master *master;
	struct ath79_spi *sp;
	struct ath79_spi_platform_data *pdata;
	unsigned long rate;
	int ret;

	master = spi_alloc_master(&pdev->dev, sizeof(*sp));
	if (master == NULL) {
		dev_err(&pdev->dev, "failed to allocate spi master\n");
		return -ENOMEM;
	}

	sp = spi_master_get_devdata(master);
	master->dev.of_node = pdev->dev.of_node;
	platform_set_drvdata(pdev, sp);

	pdata = dev_get_platdata(&pdev->dev);

	master->use_gpio_descriptors = true;
	master->bits_per_word_mask = SPI_BPW_RANGE_MASK(1, 32);
	master->flags = SPI_MASTER_GPIO_SS;
	if (pdata) {
		master->bus_num = pdata->bus_num;
		master->num_chipselect = pdata->num_chipselect;
	}

	sp->bitbang.master = master;
	sp->bitbang.chipselect = ath79_spi_chipselect;
	sp->bitbang.txrx_word[SPI_MODE_0] = ath79_spi_txrx_mode0;
	sp->bitbang.flags = SPI_CS_HIGH;

	sp->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(sp->base)) {
		ret = PTR_ERR(sp->base);
		goto err_put_master;
	}

	sp->clk = devm_clk_get(&pdev->dev, "ahb");
	if (IS_ERR(sp->clk)) {
		ret = PTR_ERR(sp->clk);
		goto err_put_master;
	}

	ret = clk_prepare_enable(sp->clk);
	if (ret)
		goto err_put_master;

	rate = DIV_ROUND_UP(clk_get_rate(sp->clk), MHZ);
	if (!rate) {
		ret = -EINVAL;
		goto err_clk_disable;
	}

	sp->rrw_delay = ATH79_SPI_RRW_DELAY_FACTOR / rate;
	dev_dbg(&pdev->dev, "register read/write delay is %u nsecs\n",
		sp->rrw_delay);

	ath79_spi_enable(sp);
	ret = spi_bitbang_start(&sp->bitbang);
	if (ret)
		goto err_disable;

	return 0;

err_disable:
	ath79_spi_disable(sp);
err_clk_disable:
	clk_disable_unprepare(sp->clk);
err_put_master:
	spi_master_put(sp->bitbang.master);

	return ret;
}

static int ath79_spi_remove(struct platform_device *pdev)
{
	struct ath79_spi *sp = platform_get_drvdata(pdev);

	spi_bitbang_stop(&sp->bitbang);
	ath79_spi_disable(sp);
	clk_disable_unprepare(sp->clk);
	spi_master_put(sp->bitbang.master);

	return 0;
}

static void ath79_spi_shutdown(struct platform_device *pdev)
{
	ath79_spi_remove(pdev);
}

static const struct of_device_id ath79_spi_of_match[] = {
	{ .compatible = "qca,ar7100-spi", },
	{ },
};
MODULE_DEVICE_TABLE(of, ath79_spi_of_match);

static struct platform_driver ath79_spi_driver = {
	.probe		= ath79_spi_probe,
	.remove		= ath79_spi_remove,
	.shutdown	= ath79_spi_shutdown,
	.driver		= {
		.name	= DRV_NAME,
		.of_match_table = ath79_spi_of_match,
	},
};
module_platform_driver(ath79_spi_driver);

MODULE_DESCRIPTION("SPI controller driver for Atheros AR71XX/AR724X/AR913X");
MODULE_AUTHOR("Gabor Juhos <juhosg@openwrt.org>");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:" DRV_NAME);
