// SPDX-License-Identifier: GPL-2.0
/*
 * Power off through MediaTek PMIC
 *
 * Copyright (C) 2018 MediaTek Inc.
 *
 * Author: Sean Wang <sean.wang@mediatek.com>
 *
 */

#include <linux/err.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/mfd/mt6397/core.h>
#include <linux/mfd/mt6397/rtc.h>

struct mt6323_pwrc {
	struct device *dev;
	struct regmap *regmap;
	u32 base;
};

static struct mt6323_pwrc *mt_pwrc;

static void mt6323_do_pwroff(void)
{
	struct mt6323_pwrc *pwrc = mt_pwrc;
	unsigned int val;
	int ret;

	regmap_write(pwrc->regmap, pwrc->base + RTC_BBPU, RTC_BBPU_KEY);
	regmap_write(pwrc->regmap, pwrc->base + RTC_WRTGR_MT6323, 1);

	ret = regmap_read_poll_timeout(pwrc->regmap,
					pwrc->base + RTC_BBPU, val,
					!(val & RTC_BBPU_CBUSY),
					MTK_RTC_POLL_DELAY_US,
					MTK_RTC_POLL_TIMEOUT);
	if (ret)
		dev_err(pwrc->dev, "failed to write BBPU: %d\n", ret);

	/* Wait some time until system down, otherwise, notice with a warn */
	mdelay(1000);

	WARN_ONCE(1, "Unable to power off system\n");
}

static int mt6323_pwrc_probe(struct platform_device *pdev)
{
	struct mt6397_chip *mt6397_chip = dev_get_drvdata(pdev->dev.parent);
	struct mt6323_pwrc *pwrc;
	struct resource *res;

	pwrc = devm_kzalloc(&pdev->dev, sizeof(*pwrc), GFP_KERNEL);
	if (!pwrc)
		return -ENOMEM;

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	pwrc->base = res->start;
	pwrc->regmap = mt6397_chip->regmap;
	pwrc->dev = &pdev->dev;
	mt_pwrc = pwrc;

	pm_power_off = &mt6323_do_pwroff;

	return 0;
}

static int mt6323_pwrc_remove(struct platform_device *pdev)
{
	if (pm_power_off == &mt6323_do_pwroff)
		pm_power_off = NULL;

	return 0;
}

static const struct of_device_id mt6323_pwrc_dt_match[] = {
	{ .compatible = "mediatek,mt6323-pwrc" },
	{},
};
MODULE_DEVICE_TABLE(of, mt6323_pwrc_dt_match);

static struct platform_driver mt6323_pwrc_driver = {
	.probe          = mt6323_pwrc_probe,
	.remove         = mt6323_pwrc_remove,
	.driver         = {
		.name   = "mt6323-pwrc",
		.of_match_table = mt6323_pwrc_dt_match,
	},
};

module_platform_driver(mt6323_pwrc_driver);

MODULE_DESCRIPTION("Poweroff driver for MT6323 PMIC");
MODULE_AUTHOR("Sean Wang <sean.wang@mediatek.com>");
MODULE_LICENSE("GPL v2");
