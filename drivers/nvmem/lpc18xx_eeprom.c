// SPDX-License-Identifier: GPL-2.0-only
/*
 * NXP LPC18xx/LPC43xx EEPROM memory NVMEM driver
 *
 * Copyright (c) 2015 Ariel D'Alessandro <ariel@vanguardiasur.com>
 */

#include <linux/clk.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/io.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/nvmem-provider.h>
#include <linux/platform_device.h>
#include <linux/reset.h>

/* Registers */
#define LPC18XX_EEPROM_AUTOPROG			0x00c
#define LPC18XX_EEPROM_AUTOPROG_WORD		0x1

#define LPC18XX_EEPROM_CLKDIV			0x014

#define LPC18XX_EEPROM_PWRDWN			0x018
#define LPC18XX_EEPROM_PWRDWN_NO		0x0
#define LPC18XX_EEPROM_PWRDWN_YES		0x1

#define LPC18XX_EEPROM_INTSTAT			0xfe0
#define LPC18XX_EEPROM_INTSTAT_END_OF_PROG	BIT(2)

#define LPC18XX_EEPROM_INTSTATCLR		0xfe8
#define LPC18XX_EEPROM_INTSTATCLR_PROG_CLR_ST	BIT(2)

/* Fixed page size (bytes) */
#define LPC18XX_EEPROM_PAGE_SIZE		0x80

/* EEPROM device requires a ~1500 kHz clock (min 800 kHz, max 1600 kHz) */
#define LPC18XX_EEPROM_CLOCK_HZ			1500000

/* EEPROM requires 3 ms of erase/program time between each writing */
#define LPC18XX_EEPROM_PROGRAM_TIME		3

struct lpc18xx_eeprom_dev {
	struct clk *clk;
	void __iomem *reg_base;
	void __iomem *mem_base;
	struct nvmem_device *nvmem;
	unsigned reg_bytes;
	unsigned val_bytes;
	int size;
};

static inline void lpc18xx_eeprom_writel(struct lpc18xx_eeprom_dev *eeprom,
					 u32 reg, u32 val)
{
	writel(val, eeprom->reg_base + reg);
}

static inline u32 lpc18xx_eeprom_readl(struct lpc18xx_eeprom_dev *eeprom,
				       u32 reg)
{
	return readl(eeprom->reg_base + reg);
}

static int lpc18xx_eeprom_busywait_until_prog(struct lpc18xx_eeprom_dev *eeprom)
{
	unsigned long end;
	u32 val;

	/* Wait until EEPROM program operation has finished */
	end = jiffies + msecs_to_jiffies(LPC18XX_EEPROM_PROGRAM_TIME * 10);

	while (time_is_after_jiffies(end)) {
		val = lpc18xx_eeprom_readl(eeprom, LPC18XX_EEPROM_INTSTAT);

		if (val & LPC18XX_EEPROM_INTSTAT_END_OF_PROG) {
			lpc18xx_eeprom_writel(eeprom, LPC18XX_EEPROM_INTSTATCLR,
					LPC18XX_EEPROM_INTSTATCLR_PROG_CLR_ST);
			return 0;
		}

		usleep_range(LPC18XX_EEPROM_PROGRAM_TIME * USEC_PER_MSEC,
			     (LPC18XX_EEPROM_PROGRAM_TIME + 1) * USEC_PER_MSEC);
	}

	return -ETIMEDOUT;
}

static int lpc18xx_eeprom_gather_write(void *context, unsigned int reg,
				       void *val, size_t bytes)
{
	struct lpc18xx_eeprom_dev *eeprom = context;
	unsigned int offset = reg;
	int ret;

	/*
	 * The last page contains the EEPROM initialization data and is not
	 * writable.
	 */
	if ((reg > eeprom->size - LPC18XX_EEPROM_PAGE_SIZE) ||
			(reg + bytes > eeprom->size - LPC18XX_EEPROM_PAGE_SIZE))
		return -EINVAL;


	lpc18xx_eeprom_writel(eeprom, LPC18XX_EEPROM_PWRDWN,
			      LPC18XX_EEPROM_PWRDWN_NO);

	/* Wait 100 us while the EEPROM wakes up */
	usleep_range(100, 200);

	while (bytes) {
		writel(*(u32 *)val, eeprom->mem_base + offset);
		ret = lpc18xx_eeprom_busywait_until_prog(eeprom);
		if (ret < 0)
			return ret;

		bytes -= eeprom->val_bytes;
		val += eeprom->val_bytes;
		offset += eeprom->val_bytes;
	}

	lpc18xx_eeprom_writel(eeprom, LPC18XX_EEPROM_PWRDWN,
			      LPC18XX_EEPROM_PWRDWN_YES);

	return 0;
}

static int lpc18xx_eeprom_read(void *context, unsigned int offset,
			       void *val, size_t bytes)
{
	struct lpc18xx_eeprom_dev *eeprom = context;

	lpc18xx_eeprom_writel(eeprom, LPC18XX_EEPROM_PWRDWN,
			      LPC18XX_EEPROM_PWRDWN_NO);

	/* Wait 100 us while the EEPROM wakes up */
	usleep_range(100, 200);

	while (bytes) {
		*(u32 *)val = readl(eeprom->mem_base + offset);
		bytes -= eeprom->val_bytes;
		val += eeprom->val_bytes;
		offset += eeprom->val_bytes;
	}

	lpc18xx_eeprom_writel(eeprom, LPC18XX_EEPROM_PWRDWN,
			      LPC18XX_EEPROM_PWRDWN_YES);

	return 0;
}


static struct nvmem_config lpc18xx_nvmem_config = {
	.name = "lpc18xx-eeprom",
	.stride = 4,
	.word_size = 4,
	.reg_read = lpc18xx_eeprom_read,
	.reg_write = lpc18xx_eeprom_gather_write,
};

static int lpc18xx_eeprom_probe(struct platform_device *pdev)
{
	struct lpc18xx_eeprom_dev *eeprom;
	struct device *dev = &pdev->dev;
	struct reset_control *rst;
	unsigned long clk_rate;
	struct resource *res;
	int ret;

	eeprom = devm_kzalloc(dev, sizeof(*eeprom), GFP_KERNEL);
	if (!eeprom)
		return -ENOMEM;

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "reg");
	eeprom->reg_base = devm_ioremap_resource(dev, res);
	if (IS_ERR(eeprom->reg_base))
		return PTR_ERR(eeprom->reg_base);

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "mem");
	eeprom->mem_base = devm_ioremap_resource(dev, res);
	if (IS_ERR(eeprom->mem_base))
		return PTR_ERR(eeprom->mem_base);

	eeprom->clk = devm_clk_get(&pdev->dev, "eeprom");
	if (IS_ERR(eeprom->clk)) {
		dev_err(&pdev->dev, "failed to get eeprom clock\n");
		return PTR_ERR(eeprom->clk);
	}

	ret = clk_prepare_enable(eeprom->clk);
	if (ret < 0) {
		dev_err(dev, "failed to prepare/enable eeprom clk: %d\n", ret);
		return ret;
	}

	rst = devm_reset_control_get_exclusive(dev, NULL);
	if (IS_ERR(rst)) {
		dev_err(dev, "failed to get reset: %ld\n", PTR_ERR(rst));
		ret = PTR_ERR(rst);
		goto err_clk;
	}

	ret = reset_control_assert(rst);
	if (ret < 0) {
		dev_err(dev, "failed to assert reset: %d\n", ret);
		goto err_clk;
	}

	eeprom->val_bytes = 4;
	eeprom->reg_bytes = 4;

	/*
	 * Clock rate is generated by dividing the system bus clock by the
	 * division factor, contained in the divider register (minus 1 encoded).
	 */
	clk_rate = clk_get_rate(eeprom->clk);
	clk_rate = DIV_ROUND_UP(clk_rate, LPC18XX_EEPROM_CLOCK_HZ) - 1;
	lpc18xx_eeprom_writel(eeprom, LPC18XX_EEPROM_CLKDIV, clk_rate);

	/*
	 * Writing a single word to the page will start the erase/program cycle
	 * automatically
	 */
	lpc18xx_eeprom_writel(eeprom, LPC18XX_EEPROM_AUTOPROG,
			      LPC18XX_EEPROM_AUTOPROG_WORD);

	lpc18xx_eeprom_writel(eeprom, LPC18XX_EEPROM_PWRDWN,
			      LPC18XX_EEPROM_PWRDWN_YES);

	eeprom->size = resource_size(res);
	lpc18xx_nvmem_config.size = resource_size(res);
	lpc18xx_nvmem_config.dev = dev;
	lpc18xx_nvmem_config.priv = eeprom;

	eeprom->nvmem = devm_nvmem_register(dev, &lpc18xx_nvmem_config);
	if (IS_ERR(eeprom->nvmem)) {
		ret = PTR_ERR(eeprom->nvmem);
		goto err_clk;
	}

	platform_set_drvdata(pdev, eeprom);

	return 0;

err_clk:
	clk_disable_unprepare(eeprom->clk);

	return ret;
}

static int lpc18xx_eeprom_remove(struct platform_device *pdev)
{
	struct lpc18xx_eeprom_dev *eeprom = platform_get_drvdata(pdev);

	clk_disable_unprepare(eeprom->clk);

	return 0;
}

static const struct of_device_id lpc18xx_eeprom_of_match[] = {
	{ .compatible = "nxp,lpc1857-eeprom" },
	{ },
};
MODULE_DEVICE_TABLE(of, lpc18xx_eeprom_of_match);

static struct platform_driver lpc18xx_eeprom_driver = {
	.probe = lpc18xx_eeprom_probe,
	.remove = lpc18xx_eeprom_remove,
	.driver = {
		.name = "lpc18xx-eeprom",
		.of_match_table = lpc18xx_eeprom_of_match,
	},
};

module_platform_driver(lpc18xx_eeprom_driver);

MODULE_AUTHOR("Ariel D'Alessandro <ariel@vanguardiasur.com.ar>");
MODULE_DESCRIPTION("NXP LPC18xx EEPROM memory Driver");
MODULE_LICENSE("GPL v2");
