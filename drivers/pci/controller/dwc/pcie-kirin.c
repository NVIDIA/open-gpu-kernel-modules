// SPDX-License-Identifier: GPL-2.0
/*
 * PCIe host controller driver for Kirin Phone SoCs
 *
 * Copyright (C) 2017 HiSilicon Electronics Co., Ltd.
 *		https://www.huawei.com
 *
 * Author: Xiaowei Song <songxiaowei@huawei.com>
 */

#include <linux/compiler.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/mfd/syscon.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/of_pci.h>
#include <linux/pci.h>
#include <linux/pci_regs.h>
#include <linux/platform_device.h>
#include <linux/regmap.h>
#include <linux/resource.h>
#include <linux/types.h>
#include "pcie-designware.h"

#define to_kirin_pcie(x) dev_get_drvdata((x)->dev)

#define REF_CLK_FREQ			100000000

/* PCIe ELBI registers */
#define SOC_PCIECTRL_CTRL0_ADDR		0x000
#define SOC_PCIECTRL_CTRL1_ADDR		0x004
#define SOC_PCIEPHY_CTRL2_ADDR		0x008
#define SOC_PCIEPHY_CTRL3_ADDR		0x00c
#define PCIE_ELBI_SLV_DBI_ENABLE	(0x1 << 21)

/* info located in APB */
#define PCIE_APP_LTSSM_ENABLE	0x01c
#define PCIE_APB_PHY_CTRL0	0x0
#define PCIE_APB_PHY_CTRL1	0x4
#define PCIE_APB_PHY_STATUS0	0x400
#define PCIE_LINKUP_ENABLE	(0x8020)
#define PCIE_LTSSM_ENABLE_BIT	(0x1 << 11)
#define PIPE_CLK_STABLE		(0x1 << 19)
#define PHY_REF_PAD_BIT		(0x1 << 8)
#define PHY_PWR_DOWN_BIT	(0x1 << 22)
#define PHY_RST_ACK_BIT		(0x1 << 16)

/* info located in sysctrl */
#define SCTRL_PCIE_CMOS_OFFSET	0x60
#define SCTRL_PCIE_CMOS_BIT	0x10
#define SCTRL_PCIE_ISO_OFFSET	0x44
#define SCTRL_PCIE_ISO_BIT	0x30
#define SCTRL_PCIE_HPCLK_OFFSET	0x190
#define SCTRL_PCIE_HPCLK_BIT	0x184000
#define SCTRL_PCIE_OE_OFFSET	0x14a
#define PCIE_DEBOUNCE_PARAM	0xF0F400
#define PCIE_OE_BYPASS		(0x3 << 28)

/* peri_crg ctrl */
#define CRGCTRL_PCIE_ASSERT_OFFSET	0x88
#define CRGCTRL_PCIE_ASSERT_BIT		0x8c000000

/* Time for delay */
#define REF_2_PERST_MIN		20000
#define REF_2_PERST_MAX		25000
#define PERST_2_ACCESS_MIN	10000
#define PERST_2_ACCESS_MAX	12000
#define LINK_WAIT_MIN		900
#define LINK_WAIT_MAX		1000
#define PIPE_CLK_WAIT_MIN	550
#define PIPE_CLK_WAIT_MAX	600
#define TIME_CMOS_MIN		100
#define TIME_CMOS_MAX		105
#define TIME_PHY_PD_MIN		10
#define TIME_PHY_PD_MAX		11

struct kirin_pcie {
	struct dw_pcie	*pci;
	void __iomem	*apb_base;
	void __iomem	*phy_base;
	struct regmap	*crgctrl;
	struct regmap	*sysctrl;
	struct clk	*apb_sys_clk;
	struct clk	*apb_phy_clk;
	struct clk	*phy_ref_clk;
	struct clk	*pcie_aclk;
	struct clk	*pcie_aux_clk;
	int		gpio_id_reset;
};

/* Registers in PCIeCTRL */
static inline void kirin_apb_ctrl_writel(struct kirin_pcie *kirin_pcie,
					 u32 val, u32 reg)
{
	writel(val, kirin_pcie->apb_base + reg);
}

static inline u32 kirin_apb_ctrl_readl(struct kirin_pcie *kirin_pcie, u32 reg)
{
	return readl(kirin_pcie->apb_base + reg);
}

/* Registers in PCIePHY */
static inline void kirin_apb_phy_writel(struct kirin_pcie *kirin_pcie,
					u32 val, u32 reg)
{
	writel(val, kirin_pcie->phy_base + reg);
}

static inline u32 kirin_apb_phy_readl(struct kirin_pcie *kirin_pcie, u32 reg)
{
	return readl(kirin_pcie->phy_base + reg);
}

static long kirin_pcie_get_clk(struct kirin_pcie *kirin_pcie,
			       struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

	kirin_pcie->phy_ref_clk = devm_clk_get(dev, "pcie_phy_ref");
	if (IS_ERR(kirin_pcie->phy_ref_clk))
		return PTR_ERR(kirin_pcie->phy_ref_clk);

	kirin_pcie->pcie_aux_clk = devm_clk_get(dev, "pcie_aux");
	if (IS_ERR(kirin_pcie->pcie_aux_clk))
		return PTR_ERR(kirin_pcie->pcie_aux_clk);

	kirin_pcie->apb_phy_clk = devm_clk_get(dev, "pcie_apb_phy");
	if (IS_ERR(kirin_pcie->apb_phy_clk))
		return PTR_ERR(kirin_pcie->apb_phy_clk);

	kirin_pcie->apb_sys_clk = devm_clk_get(dev, "pcie_apb_sys");
	if (IS_ERR(kirin_pcie->apb_sys_clk))
		return PTR_ERR(kirin_pcie->apb_sys_clk);

	kirin_pcie->pcie_aclk = devm_clk_get(dev, "pcie_aclk");
	if (IS_ERR(kirin_pcie->pcie_aclk))
		return PTR_ERR(kirin_pcie->pcie_aclk);

	return 0;
}

static long kirin_pcie_get_resource(struct kirin_pcie *kirin_pcie,
				    struct platform_device *pdev)
{
	kirin_pcie->apb_base =
		devm_platform_ioremap_resource_byname(pdev, "apb");
	if (IS_ERR(kirin_pcie->apb_base))
		return PTR_ERR(kirin_pcie->apb_base);

	kirin_pcie->phy_base =
		devm_platform_ioremap_resource_byname(pdev, "phy");
	if (IS_ERR(kirin_pcie->phy_base))
		return PTR_ERR(kirin_pcie->phy_base);

	kirin_pcie->crgctrl =
		syscon_regmap_lookup_by_compatible("hisilicon,hi3660-crgctrl");
	if (IS_ERR(kirin_pcie->crgctrl))
		return PTR_ERR(kirin_pcie->crgctrl);

	kirin_pcie->sysctrl =
		syscon_regmap_lookup_by_compatible("hisilicon,hi3660-sctrl");
	if (IS_ERR(kirin_pcie->sysctrl))
		return PTR_ERR(kirin_pcie->sysctrl);

	return 0;
}

static int kirin_pcie_phy_init(struct kirin_pcie *kirin_pcie)
{
	struct device *dev = kirin_pcie->pci->dev;
	u32 reg_val;

	reg_val = kirin_apb_phy_readl(kirin_pcie, PCIE_APB_PHY_CTRL1);
	reg_val &= ~PHY_REF_PAD_BIT;
	kirin_apb_phy_writel(kirin_pcie, reg_val, PCIE_APB_PHY_CTRL1);

	reg_val = kirin_apb_phy_readl(kirin_pcie, PCIE_APB_PHY_CTRL0);
	reg_val &= ~PHY_PWR_DOWN_BIT;
	kirin_apb_phy_writel(kirin_pcie, reg_val, PCIE_APB_PHY_CTRL0);
	usleep_range(TIME_PHY_PD_MIN, TIME_PHY_PD_MAX);

	reg_val = kirin_apb_phy_readl(kirin_pcie, PCIE_APB_PHY_CTRL1);
	reg_val &= ~PHY_RST_ACK_BIT;
	kirin_apb_phy_writel(kirin_pcie, reg_val, PCIE_APB_PHY_CTRL1);

	usleep_range(PIPE_CLK_WAIT_MIN, PIPE_CLK_WAIT_MAX);
	reg_val = kirin_apb_phy_readl(kirin_pcie, PCIE_APB_PHY_STATUS0);
	if (reg_val & PIPE_CLK_STABLE) {
		dev_err(dev, "PIPE clk is not stable\n");
		return -EINVAL;
	}

	return 0;
}

static void kirin_pcie_oe_enable(struct kirin_pcie *kirin_pcie)
{
	u32 val;

	regmap_read(kirin_pcie->sysctrl, SCTRL_PCIE_OE_OFFSET, &val);
	val |= PCIE_DEBOUNCE_PARAM;
	val &= ~PCIE_OE_BYPASS;
	regmap_write(kirin_pcie->sysctrl, SCTRL_PCIE_OE_OFFSET, val);
}

static int kirin_pcie_clk_ctrl(struct kirin_pcie *kirin_pcie, bool enable)
{
	int ret = 0;

	if (!enable)
		goto close_clk;

	ret = clk_set_rate(kirin_pcie->phy_ref_clk, REF_CLK_FREQ);
	if (ret)
		return ret;

	ret = clk_prepare_enable(kirin_pcie->phy_ref_clk);
	if (ret)
		return ret;

	ret = clk_prepare_enable(kirin_pcie->apb_sys_clk);
	if (ret)
		goto apb_sys_fail;

	ret = clk_prepare_enable(kirin_pcie->apb_phy_clk);
	if (ret)
		goto apb_phy_fail;

	ret = clk_prepare_enable(kirin_pcie->pcie_aclk);
	if (ret)
		goto aclk_fail;

	ret = clk_prepare_enable(kirin_pcie->pcie_aux_clk);
	if (ret)
		goto aux_clk_fail;

	return 0;

close_clk:
	clk_disable_unprepare(kirin_pcie->pcie_aux_clk);
aux_clk_fail:
	clk_disable_unprepare(kirin_pcie->pcie_aclk);
aclk_fail:
	clk_disable_unprepare(kirin_pcie->apb_phy_clk);
apb_phy_fail:
	clk_disable_unprepare(kirin_pcie->apb_sys_clk);
apb_sys_fail:
	clk_disable_unprepare(kirin_pcie->phy_ref_clk);

	return ret;
}

static int kirin_pcie_power_on(struct kirin_pcie *kirin_pcie)
{
	int ret;

	/* Power supply for Host */
	regmap_write(kirin_pcie->sysctrl,
		     SCTRL_PCIE_CMOS_OFFSET, SCTRL_PCIE_CMOS_BIT);
	usleep_range(TIME_CMOS_MIN, TIME_CMOS_MAX);
	kirin_pcie_oe_enable(kirin_pcie);

	ret = kirin_pcie_clk_ctrl(kirin_pcie, true);
	if (ret)
		return ret;

	/* ISO disable, PCIeCtrl, PHY assert and clk gate clear */
	regmap_write(kirin_pcie->sysctrl,
		     SCTRL_PCIE_ISO_OFFSET, SCTRL_PCIE_ISO_BIT);
	regmap_write(kirin_pcie->crgctrl,
		     CRGCTRL_PCIE_ASSERT_OFFSET, CRGCTRL_PCIE_ASSERT_BIT);
	regmap_write(kirin_pcie->sysctrl,
		     SCTRL_PCIE_HPCLK_OFFSET, SCTRL_PCIE_HPCLK_BIT);

	ret = kirin_pcie_phy_init(kirin_pcie);
	if (ret)
		goto close_clk;

	/* perst assert Endpoint */
	if (!gpio_request(kirin_pcie->gpio_id_reset, "pcie_perst")) {
		usleep_range(REF_2_PERST_MIN, REF_2_PERST_MAX);
		ret = gpio_direction_output(kirin_pcie->gpio_id_reset, 1);
		if (ret)
			goto close_clk;
		usleep_range(PERST_2_ACCESS_MIN, PERST_2_ACCESS_MAX);

		return 0;
	}

close_clk:
	kirin_pcie_clk_ctrl(kirin_pcie, false);
	return ret;
}

static void kirin_pcie_sideband_dbi_w_mode(struct kirin_pcie *kirin_pcie,
					   bool on)
{
	u32 val;

	val = kirin_apb_ctrl_readl(kirin_pcie, SOC_PCIECTRL_CTRL0_ADDR);
	if (on)
		val = val | PCIE_ELBI_SLV_DBI_ENABLE;
	else
		val = val & ~PCIE_ELBI_SLV_DBI_ENABLE;

	kirin_apb_ctrl_writel(kirin_pcie, val, SOC_PCIECTRL_CTRL0_ADDR);
}

static void kirin_pcie_sideband_dbi_r_mode(struct kirin_pcie *kirin_pcie,
					   bool on)
{
	u32 val;

	val = kirin_apb_ctrl_readl(kirin_pcie, SOC_PCIECTRL_CTRL1_ADDR);
	if (on)
		val = val | PCIE_ELBI_SLV_DBI_ENABLE;
	else
		val = val & ~PCIE_ELBI_SLV_DBI_ENABLE;

	kirin_apb_ctrl_writel(kirin_pcie, val, SOC_PCIECTRL_CTRL1_ADDR);
}

static int kirin_pcie_rd_own_conf(struct pci_bus *bus, unsigned int devfn,
				  int where, int size, u32 *val)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(bus->sysdata);

	if (PCI_SLOT(devfn)) {
		*val = ~0;
		return PCIBIOS_DEVICE_NOT_FOUND;
	}

	*val = dw_pcie_read_dbi(pci, where, size);
	return PCIBIOS_SUCCESSFUL;
}

static int kirin_pcie_wr_own_conf(struct pci_bus *bus, unsigned int devfn,
				  int where, int size, u32 val)
{
	struct dw_pcie *pci = to_dw_pcie_from_pp(bus->sysdata);

	if (PCI_SLOT(devfn))
		return PCIBIOS_DEVICE_NOT_FOUND;

	dw_pcie_write_dbi(pci, where, size, val);
	return PCIBIOS_SUCCESSFUL;
}

static struct pci_ops kirin_pci_ops = {
	.read = kirin_pcie_rd_own_conf,
	.write = kirin_pcie_wr_own_conf,
};

static u32 kirin_pcie_read_dbi(struct dw_pcie *pci, void __iomem *base,
			       u32 reg, size_t size)
{
	struct kirin_pcie *kirin_pcie = to_kirin_pcie(pci);
	u32 ret;

	kirin_pcie_sideband_dbi_r_mode(kirin_pcie, true);
	dw_pcie_read(base + reg, size, &ret);
	kirin_pcie_sideband_dbi_r_mode(kirin_pcie, false);

	return ret;
}

static void kirin_pcie_write_dbi(struct dw_pcie *pci, void __iomem *base,
				 u32 reg, size_t size, u32 val)
{
	struct kirin_pcie *kirin_pcie = to_kirin_pcie(pci);

	kirin_pcie_sideband_dbi_w_mode(kirin_pcie, true);
	dw_pcie_write(base + reg, size, val);
	kirin_pcie_sideband_dbi_w_mode(kirin_pcie, false);
}

static int kirin_pcie_link_up(struct dw_pcie *pci)
{
	struct kirin_pcie *kirin_pcie = to_kirin_pcie(pci);
	u32 val = kirin_apb_ctrl_readl(kirin_pcie, PCIE_APB_PHY_STATUS0);

	if ((val & PCIE_LINKUP_ENABLE) == PCIE_LINKUP_ENABLE)
		return 1;

	return 0;
}

static int kirin_pcie_start_link(struct dw_pcie *pci)
{
	struct kirin_pcie *kirin_pcie = to_kirin_pcie(pci);

	/* assert LTSSM enable */
	kirin_apb_ctrl_writel(kirin_pcie, PCIE_LTSSM_ENABLE_BIT,
			      PCIE_APP_LTSSM_ENABLE);

	return 0;
}

static int kirin_pcie_host_init(struct pcie_port *pp)
{
	pp->bridge->ops = &kirin_pci_ops;

	return 0;
}

static const struct dw_pcie_ops kirin_dw_pcie_ops = {
	.read_dbi = kirin_pcie_read_dbi,
	.write_dbi = kirin_pcie_write_dbi,
	.link_up = kirin_pcie_link_up,
	.start_link = kirin_pcie_start_link,
};

static const struct dw_pcie_host_ops kirin_pcie_host_ops = {
	.host_init = kirin_pcie_host_init,
};

static int kirin_pcie_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct kirin_pcie *kirin_pcie;
	struct dw_pcie *pci;
	int ret;

	if (!dev->of_node) {
		dev_err(dev, "NULL node\n");
		return -EINVAL;
	}

	kirin_pcie = devm_kzalloc(dev, sizeof(struct kirin_pcie), GFP_KERNEL);
	if (!kirin_pcie)
		return -ENOMEM;

	pci = devm_kzalloc(dev, sizeof(*pci), GFP_KERNEL);
	if (!pci)
		return -ENOMEM;

	pci->dev = dev;
	pci->ops = &kirin_dw_pcie_ops;
	pci->pp.ops = &kirin_pcie_host_ops;
	kirin_pcie->pci = pci;

	ret = kirin_pcie_get_clk(kirin_pcie, pdev);
	if (ret)
		return ret;

	ret = kirin_pcie_get_resource(kirin_pcie, pdev);
	if (ret)
		return ret;

	kirin_pcie->gpio_id_reset = of_get_named_gpio(dev->of_node,
						      "reset-gpios", 0);
	if (kirin_pcie->gpio_id_reset == -EPROBE_DEFER) {
		return -EPROBE_DEFER;
	} else if (!gpio_is_valid(kirin_pcie->gpio_id_reset)) {
		dev_err(dev, "unable to get a valid gpio pin\n");
		return -ENODEV;
	}

	ret = kirin_pcie_power_on(kirin_pcie);
	if (ret)
		return ret;

	platform_set_drvdata(pdev, kirin_pcie);

	return dw_pcie_host_init(&pci->pp);
}

static const struct of_device_id kirin_pcie_match[] = {
	{ .compatible = "hisilicon,kirin960-pcie" },
	{},
};

static struct platform_driver kirin_pcie_driver = {
	.probe			= kirin_pcie_probe,
	.driver			= {
		.name			= "kirin-pcie",
		.of_match_table = kirin_pcie_match,
		.suppress_bind_attrs = true,
	},
};
builtin_platform_driver(kirin_pcie_driver);
