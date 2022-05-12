// SPDX-License-Identifier: GPL-2.0
/*
 * PCIe endpoint controller driver for UniPhier SoCs
 * Copyright 2018 Socionext Inc.
 * Author: Kunihiko Hayashi <hayashi.kunihiko@socionext.com>
 */

#include <linux/bitops.h>
#include <linux/bitfield.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/of_device.h>
#include <linux/pci.h>
#include <linux/phy/phy.h>
#include <linux/platform_device.h>
#include <linux/reset.h>

#include "pcie-designware.h"

/* Link Glue registers */
#define PCL_RSTCTRL0			0x0010
#define PCL_RSTCTRL_AXI_REG		BIT(3)
#define PCL_RSTCTRL_AXI_SLAVE		BIT(2)
#define PCL_RSTCTRL_AXI_MASTER		BIT(1)
#define PCL_RSTCTRL_PIPE3		BIT(0)

#define PCL_RSTCTRL1			0x0020
#define PCL_RSTCTRL_PERST		BIT(0)

#define PCL_RSTCTRL2			0x0024
#define PCL_RSTCTRL_PHY_RESET		BIT(0)

#define PCL_MODE			0x8000
#define PCL_MODE_REGEN			BIT(8)
#define PCL_MODE_REGVAL			BIT(0)

#define PCL_APP_CLK_CTRL		0x8004
#define PCL_APP_CLK_REQ			BIT(0)

#define PCL_APP_READY_CTRL		0x8008
#define PCL_APP_LTSSM_ENABLE		BIT(0)

#define PCL_APP_MSI0			0x8040
#define PCL_APP_VEN_MSI_TC_MASK		GENMASK(10, 8)
#define PCL_APP_VEN_MSI_VECTOR_MASK	GENMASK(4, 0)

#define PCL_APP_MSI1			0x8044
#define PCL_APP_MSI_REQ			BIT(0)

#define PCL_APP_INTX			0x8074
#define PCL_APP_INTX_SYS_INT		BIT(0)

/* assertion time of INTx in usec */
#define PCL_INTX_WIDTH_USEC		30

struct uniphier_pcie_ep_priv {
	void __iomem *base;
	struct dw_pcie pci;
	struct clk *clk, *clk_gio;
	struct reset_control *rst, *rst_gio;
	struct phy *phy;
	const struct pci_epc_features *features;
};

#define to_uniphier_pcie(x)	dev_get_drvdata((x)->dev)

static void uniphier_pcie_ltssm_enable(struct uniphier_pcie_ep_priv *priv,
				       bool enable)
{
	u32 val;

	val = readl(priv->base + PCL_APP_READY_CTRL);
	if (enable)
		val |= PCL_APP_LTSSM_ENABLE;
	else
		val &= ~PCL_APP_LTSSM_ENABLE;
	writel(val, priv->base + PCL_APP_READY_CTRL);
}

static void uniphier_pcie_phy_reset(struct uniphier_pcie_ep_priv *priv,
				    bool assert)
{
	u32 val;

	val = readl(priv->base + PCL_RSTCTRL2);
	if (assert)
		val |= PCL_RSTCTRL_PHY_RESET;
	else
		val &= ~PCL_RSTCTRL_PHY_RESET;
	writel(val, priv->base + PCL_RSTCTRL2);
}

static void uniphier_pcie_init_ep(struct uniphier_pcie_ep_priv *priv)
{
	u32 val;

	/* set EP mode */
	val = readl(priv->base + PCL_MODE);
	val |= PCL_MODE_REGEN | PCL_MODE_REGVAL;
	writel(val, priv->base + PCL_MODE);

	/* clock request */
	val = readl(priv->base + PCL_APP_CLK_CTRL);
	val &= ~PCL_APP_CLK_REQ;
	writel(val, priv->base + PCL_APP_CLK_CTRL);

	/* deassert PIPE3 and AXI reset */
	val = readl(priv->base + PCL_RSTCTRL0);
	val |= PCL_RSTCTRL_AXI_REG | PCL_RSTCTRL_AXI_SLAVE
		| PCL_RSTCTRL_AXI_MASTER | PCL_RSTCTRL_PIPE3;
	writel(val, priv->base + PCL_RSTCTRL0);

	uniphier_pcie_ltssm_enable(priv, false);

	msleep(100);
}

static int uniphier_pcie_start_link(struct dw_pcie *pci)
{
	struct uniphier_pcie_ep_priv *priv = to_uniphier_pcie(pci);

	uniphier_pcie_ltssm_enable(priv, true);

	return 0;
}

static void uniphier_pcie_stop_link(struct dw_pcie *pci)
{
	struct uniphier_pcie_ep_priv *priv = to_uniphier_pcie(pci);

	uniphier_pcie_ltssm_enable(priv, false);
}

static void uniphier_pcie_ep_init(struct dw_pcie_ep *ep)
{
	struct dw_pcie *pci = to_dw_pcie_from_ep(ep);
	enum pci_barno bar;

	for (bar = BAR_0; bar <= BAR_5; bar++)
		dw_pcie_ep_reset_bar(pci, bar);
}

static int uniphier_pcie_ep_raise_legacy_irq(struct dw_pcie_ep *ep)
{
	struct dw_pcie *pci = to_dw_pcie_from_ep(ep);
	struct uniphier_pcie_ep_priv *priv = to_uniphier_pcie(pci);
	u32 val;

	/*
	 * This makes pulse signal to send INTx to the RC, so this should
	 * be cleared as soon as possible. This sequence is covered with
	 * mutex in pci_epc_raise_irq().
	 */
	/* assert INTx */
	val = readl(priv->base + PCL_APP_INTX);
	val |= PCL_APP_INTX_SYS_INT;
	writel(val, priv->base + PCL_APP_INTX);

	udelay(PCL_INTX_WIDTH_USEC);

	/* deassert INTx */
	val &= ~PCL_APP_INTX_SYS_INT;
	writel(val, priv->base + PCL_APP_INTX);

	return 0;
}

static int uniphier_pcie_ep_raise_msi_irq(struct dw_pcie_ep *ep,
					  u8 func_no, u16 interrupt_num)
{
	struct dw_pcie *pci = to_dw_pcie_from_ep(ep);
	struct uniphier_pcie_ep_priv *priv = to_uniphier_pcie(pci);
	u32 val;

	val = FIELD_PREP(PCL_APP_VEN_MSI_TC_MASK, func_no)
		| FIELD_PREP(PCL_APP_VEN_MSI_VECTOR_MASK, interrupt_num - 1);
	writel(val, priv->base + PCL_APP_MSI0);

	val = readl(priv->base + PCL_APP_MSI1);
	val |= PCL_APP_MSI_REQ;
	writel(val, priv->base + PCL_APP_MSI1);

	return 0;
}

static int uniphier_pcie_ep_raise_irq(struct dw_pcie_ep *ep, u8 func_no,
				      enum pci_epc_irq_type type,
				      u16 interrupt_num)
{
	struct dw_pcie *pci = to_dw_pcie_from_ep(ep);

	switch (type) {
	case PCI_EPC_IRQ_LEGACY:
		return uniphier_pcie_ep_raise_legacy_irq(ep);
	case PCI_EPC_IRQ_MSI:
		return uniphier_pcie_ep_raise_msi_irq(ep, func_no,
						      interrupt_num);
	default:
		dev_err(pci->dev, "UNKNOWN IRQ type (%d)\n", type);
	}

	return 0;
}

static const struct pci_epc_features*
uniphier_pcie_get_features(struct dw_pcie_ep *ep)
{
	struct dw_pcie *pci = to_dw_pcie_from_ep(ep);
	struct uniphier_pcie_ep_priv *priv = to_uniphier_pcie(pci);

	return priv->features;
}

static const struct dw_pcie_ep_ops uniphier_pcie_ep_ops = {
	.ep_init = uniphier_pcie_ep_init,
	.raise_irq = uniphier_pcie_ep_raise_irq,
	.get_features = uniphier_pcie_get_features,
};

static int uniphier_pcie_ep_enable(struct uniphier_pcie_ep_priv *priv)
{
	int ret;

	ret = clk_prepare_enable(priv->clk);
	if (ret)
		return ret;

	ret = clk_prepare_enable(priv->clk_gio);
	if (ret)
		goto out_clk_disable;

	ret = reset_control_deassert(priv->rst);
	if (ret)
		goto out_clk_gio_disable;

	ret = reset_control_deassert(priv->rst_gio);
	if (ret)
		goto out_rst_assert;

	uniphier_pcie_init_ep(priv);

	uniphier_pcie_phy_reset(priv, true);

	ret = phy_init(priv->phy);
	if (ret)
		goto out_rst_gio_assert;

	uniphier_pcie_phy_reset(priv, false);

	return 0;

out_rst_gio_assert:
	reset_control_assert(priv->rst_gio);
out_rst_assert:
	reset_control_assert(priv->rst);
out_clk_gio_disable:
	clk_disable_unprepare(priv->clk_gio);
out_clk_disable:
	clk_disable_unprepare(priv->clk);

	return ret;
}

static const struct dw_pcie_ops dw_pcie_ops = {
	.start_link = uniphier_pcie_start_link,
	.stop_link = uniphier_pcie_stop_link,
};

static int uniphier_pcie_ep_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct uniphier_pcie_ep_priv *priv;
	int ret;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->features = of_device_get_match_data(dev);
	if (WARN_ON(!priv->features))
		return -EINVAL;

	priv->pci.dev = dev;
	priv->pci.ops = &dw_pcie_ops;

	priv->base = devm_platform_ioremap_resource_byname(pdev, "link");
	if (IS_ERR(priv->base))
		return PTR_ERR(priv->base);

	priv->clk_gio = devm_clk_get(dev, "gio");
	if (IS_ERR(priv->clk_gio))
		return PTR_ERR(priv->clk_gio);

	priv->rst_gio = devm_reset_control_get_shared(dev, "gio");
	if (IS_ERR(priv->rst_gio))
		return PTR_ERR(priv->rst_gio);

	priv->clk = devm_clk_get(dev, "link");
	if (IS_ERR(priv->clk))
		return PTR_ERR(priv->clk);

	priv->rst = devm_reset_control_get_shared(dev, "link");
	if (IS_ERR(priv->rst))
		return PTR_ERR(priv->rst);

	priv->phy = devm_phy_optional_get(dev, "pcie-phy");
	if (IS_ERR(priv->phy)) {
		ret = PTR_ERR(priv->phy);
		dev_err(dev, "Failed to get phy (%d)\n", ret);
		return ret;
	}

	platform_set_drvdata(pdev, priv);

	ret = uniphier_pcie_ep_enable(priv);
	if (ret)
		return ret;

	priv->pci.ep.ops = &uniphier_pcie_ep_ops;
	return dw_pcie_ep_init(&priv->pci.ep);
}

static const struct pci_epc_features uniphier_pro5_data = {
	.linkup_notifier = false,
	.msi_capable = true,
	.msix_capable = false,
	.align = 1 << 16,
	.bar_fixed_64bit = BIT(BAR_0) | BIT(BAR_2) | BIT(BAR_4),
	.reserved_bar =  BIT(BAR_4),
};

static const struct of_device_id uniphier_pcie_ep_match[] = {
	{
		.compatible = "socionext,uniphier-pro5-pcie-ep",
		.data = &uniphier_pro5_data,
	},
	{ /* sentinel */ },
};

static struct platform_driver uniphier_pcie_ep_driver = {
	.probe  = uniphier_pcie_ep_probe,
	.driver = {
		.name = "uniphier-pcie-ep",
		.of_match_table = uniphier_pcie_ep_match,
		.suppress_bind_attrs = true,
	},
};
builtin_platform_driver(uniphier_pcie_ep_driver);
