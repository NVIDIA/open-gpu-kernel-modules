// SPDX-License-Identifier: GPL-2.0
/*
 * phy-uniphier-ahci.c - PHY driver for UniPhier AHCI controller
 * Copyright 2016-2020, Socionext Inc.
 * Author: Kunihiko Hayashi <hayashi.kunihiko@socionext.com>
 */

#include <linux/bitfield.h>
#include <linux/bitops.h>
#include <linux/clk.h>
#include <linux/iopoll.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>
#include <linux/reset.h>

struct uniphier_ahciphy_priv {
	struct device *dev;
	void __iomem  *base;
	struct clk *clk, *clk_parent;
	struct reset_control *rst, *rst_parent;
	const struct uniphier_ahciphy_soc_data *data;
};

struct uniphier_ahciphy_soc_data {
	int (*init)(struct uniphier_ahciphy_priv *priv);
	int (*power_on)(struct uniphier_ahciphy_priv *priv);
	int (*power_off)(struct uniphier_ahciphy_priv *priv);
	bool is_ready_high;
	bool is_phy_clk;
};

/* for PXs2/PXs3 */
#define CKCTRL				0x0
#define CKCTRL_P0_READY			BIT(15)
#define CKCTRL_P0_RESET			BIT(10)
#define CKCTRL_REF_SSP_EN		BIT(9)
#define TXCTRL0				0x4
#define TXCTRL0_AMP_G3_MASK		GENMASK(22, 16)
#define TXCTRL0_AMP_G2_MASK		GENMASK(14, 8)
#define TXCTRL0_AMP_G1_MASK		GENMASK(6, 0)
#define TXCTRL1				0x8
#define TXCTRL1_DEEMPH_G3_MASK		GENMASK(21, 16)
#define TXCTRL1_DEEMPH_G2_MASK		GENMASK(13, 8)
#define TXCTRL1_DEEMPH_G1_MASK		GENMASK(5, 0)
#define RXCTRL				0xc
#define RXCTRL_LOS_LVL_MASK		GENMASK(20, 16)
#define RXCTRL_LOS_BIAS_MASK		GENMASK(10, 8)
#define RXCTRL_RX_EQ_MASK		GENMASK(2, 0)

static void uniphier_ahciphy_pxs2_enable(struct uniphier_ahciphy_priv *priv,
					 bool enable)
{
	u32 val;

	val = readl(priv->base + CKCTRL);

	if (enable) {
		val |= CKCTRL_REF_SSP_EN;
		writel(val, priv->base + CKCTRL);
		val &= ~CKCTRL_P0_RESET;
		writel(val, priv->base + CKCTRL);
	} else {
		val |= CKCTRL_P0_RESET;
		writel(val, priv->base + CKCTRL);
		val &= ~CKCTRL_REF_SSP_EN;
		writel(val, priv->base + CKCTRL);
	}
}

static int uniphier_ahciphy_pxs2_power_on(struct uniphier_ahciphy_priv *priv)
{
	int ret;
	u32 val;

	uniphier_ahciphy_pxs2_enable(priv, true);

	/* wait until PLL is ready */
	if (priv->data->is_ready_high)
		ret = readl_poll_timeout(priv->base + CKCTRL, val,
					 (val & CKCTRL_P0_READY), 200, 400);
	else
		ret = readl_poll_timeout(priv->base + CKCTRL, val,
					 !(val & CKCTRL_P0_READY), 200, 400);
	if (ret) {
		dev_err(priv->dev, "Failed to check whether PHY PLL is ready\n");
		uniphier_ahciphy_pxs2_enable(priv, false);
	}

	return ret;
}

static int uniphier_ahciphy_pxs2_power_off(struct uniphier_ahciphy_priv *priv)
{
	uniphier_ahciphy_pxs2_enable(priv, false);

	return 0;
}

static int uniphier_ahciphy_pxs3_init(struct uniphier_ahciphy_priv *priv)
{
	int i;
	u32 val;

	/* setup port parameter */
	val = readl(priv->base + TXCTRL0);
	val &= ~TXCTRL0_AMP_G3_MASK;
	val |= FIELD_PREP(TXCTRL0_AMP_G3_MASK, 0x73);
	val &= ~TXCTRL0_AMP_G2_MASK;
	val |= FIELD_PREP(TXCTRL0_AMP_G2_MASK, 0x46);
	val &= ~TXCTRL0_AMP_G1_MASK;
	val |= FIELD_PREP(TXCTRL0_AMP_G1_MASK, 0x42);
	writel(val, priv->base + TXCTRL0);

	val = readl(priv->base + TXCTRL1);
	val &= ~TXCTRL1_DEEMPH_G3_MASK;
	val |= FIELD_PREP(TXCTRL1_DEEMPH_G3_MASK, 0x23);
	val &= ~TXCTRL1_DEEMPH_G2_MASK;
	val |= FIELD_PREP(TXCTRL1_DEEMPH_G2_MASK, 0x05);
	val &= ~TXCTRL1_DEEMPH_G1_MASK;
	val |= FIELD_PREP(TXCTRL1_DEEMPH_G1_MASK, 0x05);

	val = readl(priv->base + RXCTRL);
	val &= ~RXCTRL_LOS_LVL_MASK;
	val |= FIELD_PREP(RXCTRL_LOS_LVL_MASK, 0x9);
	val &= ~RXCTRL_LOS_BIAS_MASK;
	val |= FIELD_PREP(RXCTRL_LOS_BIAS_MASK, 0x2);
	val &= ~RXCTRL_RX_EQ_MASK;
	val |= FIELD_PREP(RXCTRL_RX_EQ_MASK, 0x1);

	/* dummy read 25 times to make a wait time for the phy to stabilize */
	for (i = 0; i < 25; i++)
		readl(priv->base + CKCTRL);

	return 0;
}

static int uniphier_ahciphy_init(struct phy *phy)
{
	struct uniphier_ahciphy_priv *priv = phy_get_drvdata(phy);
	int ret;

	ret = clk_prepare_enable(priv->clk_parent);
	if (ret)
		return ret;

	ret = reset_control_deassert(priv->rst_parent);
	if (ret)
		goto out_clk_disable;

	if (priv->data->init) {
		ret = priv->data->init(priv);
		if (ret)
			goto out_rst_assert;
	}

	return 0;

out_rst_assert:
	reset_control_assert(priv->rst_parent);
out_clk_disable:
	clk_disable_unprepare(priv->clk_parent);

	return ret;
}

static int uniphier_ahciphy_exit(struct phy *phy)
{
	struct uniphier_ahciphy_priv *priv = phy_get_drvdata(phy);

	reset_control_assert(priv->rst_parent);
	clk_disable_unprepare(priv->clk_parent);

	return 0;
}

static int uniphier_ahciphy_power_on(struct phy *phy)
{
	struct uniphier_ahciphy_priv *priv = phy_get_drvdata(phy);
	int ret = 0;

	ret = clk_prepare_enable(priv->clk);
	if (ret)
		return ret;

	ret = reset_control_deassert(priv->rst);
	if (ret)
		goto out_clk_disable;

	if (priv->data->power_on) {
		ret = priv->data->power_on(priv);
		if (ret)
			goto out_reset_assert;
	}

	return 0;

out_reset_assert:
	reset_control_assert(priv->rst);
out_clk_disable:
	clk_disable_unprepare(priv->clk);

	return ret;
}

static int uniphier_ahciphy_power_off(struct phy *phy)
{
	struct uniphier_ahciphy_priv *priv = phy_get_drvdata(phy);
	int ret = 0;

	if (priv->data->power_off)
		ret = priv->data->power_off(priv);

	reset_control_assert(priv->rst);
	clk_disable_unprepare(priv->clk);

	return ret;
}

static const struct phy_ops uniphier_ahciphy_ops = {
	.init  = uniphier_ahciphy_init,
	.exit  = uniphier_ahciphy_exit,
	.power_on  = uniphier_ahciphy_power_on,
	.power_off = uniphier_ahciphy_power_off,
	.owner = THIS_MODULE,
};

static int uniphier_ahciphy_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct uniphier_ahciphy_priv *priv;
	struct phy *phy;
	struct phy_provider *phy_provider;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->dev = dev;
	priv->data = of_device_get_match_data(dev);
	if (WARN_ON(!priv->data))
		return -EINVAL;

	priv->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(priv->base))
		return PTR_ERR(priv->base);

	priv->clk_parent = devm_clk_get(dev, "link");
	if (IS_ERR(priv->clk_parent))
		return PTR_ERR(priv->clk_parent);

	if (priv->data->is_phy_clk) {
		priv->clk = devm_clk_get(dev, "phy");
		if (IS_ERR(priv->clk))
			return PTR_ERR(priv->clk);
	}

	priv->rst_parent = devm_reset_control_get_shared(dev, "link");
	if (IS_ERR(priv->rst_parent))
		return PTR_ERR(priv->rst_parent);

	priv->rst = devm_reset_control_get_shared(dev, "phy");
	if (IS_ERR(priv->rst))
		return PTR_ERR(priv->rst);

	phy = devm_phy_create(dev, dev->of_node, &uniphier_ahciphy_ops);
	if (IS_ERR(phy)) {
		dev_err(dev, "failed to create phy\n");
		return PTR_ERR(phy);
	}

	phy_set_drvdata(phy, priv);
	phy_provider = devm_of_phy_provider_register(dev, of_phy_simple_xlate);
	if (IS_ERR(phy_provider))
		return PTR_ERR(phy_provider);

	return 0;
}

static const struct uniphier_ahciphy_soc_data uniphier_pxs2_data = {
	.power_on  = uniphier_ahciphy_pxs2_power_on,
	.power_off = uniphier_ahciphy_pxs2_power_off,
	.is_ready_high = false,
	.is_phy_clk = false,
};

static const struct uniphier_ahciphy_soc_data uniphier_pxs3_data = {
	.init      = uniphier_ahciphy_pxs3_init,
	.power_on  = uniphier_ahciphy_pxs2_power_on,
	.power_off = uniphier_ahciphy_pxs2_power_off,
	.is_ready_high = true,
	.is_phy_clk = true,
};

static const struct of_device_id uniphier_ahciphy_match[] = {
	{
		.compatible = "socionext,uniphier-pxs2-ahci-phy",
		.data = &uniphier_pxs2_data,
	},
	{
		.compatible = "socionext,uniphier-pxs3-ahci-phy",
		.data = &uniphier_pxs3_data,
	},
	{ /* Sentinel */ },
};
MODULE_DEVICE_TABLE(of, uniphier_ahciphy_match);

static struct platform_driver uniphier_ahciphy_driver = {
	.probe = uniphier_ahciphy_probe,
	.driver = {
		.name = "uniphier-ahci-phy",
		.of_match_table = uniphier_ahciphy_match,
	},
};
module_platform_driver(uniphier_ahciphy_driver);

MODULE_AUTHOR("Kunihiko Hayashi <hayashi.kunihiko@socionext.com>");
MODULE_DESCRIPTION("UniPhier PHY driver for AHCI controller");
MODULE_LICENSE("GPL v2");
