// SPDX-License-Identifier: GPL-2.0-only
/*
 * dwmac-stm32.c - DWMAC Specific Glue layer for STM32 MCU
 *
 * Copyright (C) STMicroelectronics SA 2017
 * Author:  Alexandre Torgue <alexandre.torgue@st.com> for STMicroelectronics.
 */

#include <linux/clk.h>
#include <linux/kernel.h>
#include <linux/mfd/syscon.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_net.h>
#include <linux/phy.h>
#include <linux/platform_device.h>
#include <linux/pm_wakeirq.h>
#include <linux/regmap.h>
#include <linux/slab.h>
#include <linux/stmmac.h>

#include "stmmac_platform.h"

#define SYSCFG_MCU_ETH_MASK		BIT(23)
#define SYSCFG_MP1_ETH_MASK		GENMASK(23, 16)
#define SYSCFG_PMCCLRR_OFFSET		0x40

#define SYSCFG_PMCR_ETH_CLK_SEL		BIT(16)
#define SYSCFG_PMCR_ETH_REF_CLK_SEL	BIT(17)

/* CLOCK feed to PHY*/
#define ETH_CK_F_25M	25000000
#define ETH_CK_F_50M	50000000
#define ETH_CK_F_125M	125000000

/*  Ethernet PHY interface selection in register SYSCFG Configuration
 *------------------------------------------
 * src	 |BIT(23)| BIT(22)| BIT(21)|BIT(20)|
 *------------------------------------------
 * MII   |   0	 |   0	  |   0    |   1   |
 *------------------------------------------
 * GMII  |   0	 |   0	  |   0    |   0   |
 *------------------------------------------
 * RGMII |   0	 |   0	  |   1	   |  n/a  |
 *------------------------------------------
 * RMII  |   1	 |   0	  |   0	   |  n/a  |
 *------------------------------------------
 */
#define SYSCFG_PMCR_ETH_SEL_MII		BIT(20)
#define SYSCFG_PMCR_ETH_SEL_RGMII	BIT(21)
#define SYSCFG_PMCR_ETH_SEL_RMII	BIT(23)
#define SYSCFG_PMCR_ETH_SEL_GMII	0
#define SYSCFG_MCU_ETH_SEL_MII		0
#define SYSCFG_MCU_ETH_SEL_RMII		1

/* STM32MP1 register definitions
 *
 * Below table summarizes the clock requirement and clock sources for
 * supported phy interface modes.
 * __________________________________________________________________________
 *|PHY_MODE | Normal | PHY wo crystal|   PHY wo crystal   |No 125Mhz from PHY|
 *|         |        |      25MHz    |        50MHz       |                  |
 * ---------------------------------------------------------------------------
 *|  MII    |	 -   |     eth-ck    |	      n/a	  |	  n/a        |
 *|         |        | st,ext-phyclk |                    |		     |
 * ---------------------------------------------------------------------------
 *|  GMII   |	 -   |     eth-ck    |	      n/a	  |	  n/a        |
 *|         |        | st,ext-phyclk |                    |		     |
 * ---------------------------------------------------------------------------
 *| RGMII   |	 -   |     eth-ck    |	      n/a	  |      eth-ck      |
 *|         |        | st,ext-phyclk |                    | st,eth-clk-sel or|
 *|         |        |               |                    | st,ext-phyclk    |
 * ---------------------------------------------------------------------------
 *| RMII    |	 -   |     eth-ck    |	    eth-ck        |	  n/a        |
 *|         |        | st,ext-phyclk | st,eth-ref-clk-sel |		     |
 *|         |        |               | or st,ext-phyclk   |		     |
 * ---------------------------------------------------------------------------
 *
 */

struct stm32_dwmac {
	struct clk *clk_tx;
	struct clk *clk_rx;
	struct clk *clk_eth_ck;
	struct clk *clk_ethstp;
	struct clk *syscfg_clk;
	int ext_phyclk;
	int enable_eth_ck;
	int eth_clk_sel_reg;
	int eth_ref_clk_sel_reg;
	int irq_pwr_wakeup;
	u32 mode_reg;		 /* MAC glue-logic mode register */
	struct regmap *regmap;
	u32 speed;
	const struct stm32_ops *ops;
	struct device *dev;
};

struct stm32_ops {
	int (*set_mode)(struct plat_stmmacenet_data *plat_dat);
	int (*clk_prepare)(struct stm32_dwmac *dwmac, bool prepare);
	int (*suspend)(struct stm32_dwmac *dwmac);
	void (*resume)(struct stm32_dwmac *dwmac);
	int (*parse_data)(struct stm32_dwmac *dwmac,
			  struct device *dev);
	u32 syscfg_eth_mask;
};

static int stm32_dwmac_init(struct plat_stmmacenet_data *plat_dat)
{
	struct stm32_dwmac *dwmac = plat_dat->bsp_priv;
	int ret;

	if (dwmac->ops->set_mode) {
		ret = dwmac->ops->set_mode(plat_dat);
		if (ret)
			return ret;
	}

	ret = clk_prepare_enable(dwmac->clk_tx);
	if (ret)
		return ret;

	if (!dwmac->dev->power.is_suspended) {
		ret = clk_prepare_enable(dwmac->clk_rx);
		if (ret) {
			clk_disable_unprepare(dwmac->clk_tx);
			return ret;
		}
	}

	if (dwmac->ops->clk_prepare) {
		ret = dwmac->ops->clk_prepare(dwmac, true);
		if (ret) {
			clk_disable_unprepare(dwmac->clk_rx);
			clk_disable_unprepare(dwmac->clk_tx);
		}
	}

	return ret;
}

static int stm32mp1_clk_prepare(struct stm32_dwmac *dwmac, bool prepare)
{
	int ret = 0;

	if (prepare) {
		ret = clk_prepare_enable(dwmac->syscfg_clk);
		if (ret)
			return ret;
		if (dwmac->enable_eth_ck) {
			ret = clk_prepare_enable(dwmac->clk_eth_ck);
			if (ret) {
				clk_disable_unprepare(dwmac->syscfg_clk);
				return ret;
			}
		}
	} else {
		clk_disable_unprepare(dwmac->syscfg_clk);
		if (dwmac->enable_eth_ck)
			clk_disable_unprepare(dwmac->clk_eth_ck);
	}
	return ret;
}

static int stm32mp1_set_mode(struct plat_stmmacenet_data *plat_dat)
{
	struct stm32_dwmac *dwmac = plat_dat->bsp_priv;
	u32 reg = dwmac->mode_reg, clk_rate;
	int val;

	clk_rate = clk_get_rate(dwmac->clk_eth_ck);
	dwmac->enable_eth_ck = false;
	switch (plat_dat->interface) {
	case PHY_INTERFACE_MODE_MII:
		if (clk_rate == ETH_CK_F_25M && dwmac->ext_phyclk)
			dwmac->enable_eth_ck = true;
		val = SYSCFG_PMCR_ETH_SEL_MII;
		pr_debug("SYSCFG init : PHY_INTERFACE_MODE_MII\n");
		break;
	case PHY_INTERFACE_MODE_GMII:
		val = SYSCFG_PMCR_ETH_SEL_GMII;
		if (clk_rate == ETH_CK_F_25M &&
		    (dwmac->eth_clk_sel_reg || dwmac->ext_phyclk)) {
			dwmac->enable_eth_ck = true;
			val |= SYSCFG_PMCR_ETH_CLK_SEL;
		}
		pr_debug("SYSCFG init : PHY_INTERFACE_MODE_GMII\n");
		break;
	case PHY_INTERFACE_MODE_RMII:
		val = SYSCFG_PMCR_ETH_SEL_RMII;
		if ((clk_rate == ETH_CK_F_25M || clk_rate == ETH_CK_F_50M) &&
		    (dwmac->eth_ref_clk_sel_reg || dwmac->ext_phyclk)) {
			dwmac->enable_eth_ck = true;
			val |= SYSCFG_PMCR_ETH_REF_CLK_SEL;
		}
		pr_debug("SYSCFG init : PHY_INTERFACE_MODE_RMII\n");
		break;
	case PHY_INTERFACE_MODE_RGMII:
	case PHY_INTERFACE_MODE_RGMII_ID:
	case PHY_INTERFACE_MODE_RGMII_RXID:
	case PHY_INTERFACE_MODE_RGMII_TXID:
		val = SYSCFG_PMCR_ETH_SEL_RGMII;
		if ((clk_rate == ETH_CK_F_25M || clk_rate == ETH_CK_F_125M) &&
		    (dwmac->eth_clk_sel_reg || dwmac->ext_phyclk)) {
			dwmac->enable_eth_ck = true;
			val |= SYSCFG_PMCR_ETH_CLK_SEL;
		}
		pr_debug("SYSCFG init : PHY_INTERFACE_MODE_RGMII\n");
		break;
	default:
		pr_debug("SYSCFG init :  Do not manage %d interface\n",
			 plat_dat->interface);
		/* Do not manage others interfaces */
		return -EINVAL;
	}

	/* Need to update PMCCLRR (clear register) */
	regmap_write(dwmac->regmap, reg + SYSCFG_PMCCLRR_OFFSET,
		     dwmac->ops->syscfg_eth_mask);

	/* Update PMCSETR (set register) */
	return regmap_update_bits(dwmac->regmap, reg,
				 dwmac->ops->syscfg_eth_mask, val);
}

static int stm32mcu_set_mode(struct plat_stmmacenet_data *plat_dat)
{
	struct stm32_dwmac *dwmac = plat_dat->bsp_priv;
	u32 reg = dwmac->mode_reg;
	int val;

	switch (plat_dat->interface) {
	case PHY_INTERFACE_MODE_MII:
		val = SYSCFG_MCU_ETH_SEL_MII;
		pr_debug("SYSCFG init : PHY_INTERFACE_MODE_MII\n");
		break;
	case PHY_INTERFACE_MODE_RMII:
		val = SYSCFG_MCU_ETH_SEL_RMII;
		pr_debug("SYSCFG init : PHY_INTERFACE_MODE_RMII\n");
		break;
	default:
		pr_debug("SYSCFG init :  Do not manage %d interface\n",
			 plat_dat->interface);
		/* Do not manage others interfaces */
		return -EINVAL;
	}

	return regmap_update_bits(dwmac->regmap, reg,
				 dwmac->ops->syscfg_eth_mask, val << 23);
}

static void stm32_dwmac_clk_disable(struct stm32_dwmac *dwmac)
{
	clk_disable_unprepare(dwmac->clk_tx);
	clk_disable_unprepare(dwmac->clk_rx);

	if (dwmac->ops->clk_prepare)
		dwmac->ops->clk_prepare(dwmac, false);
}

static int stm32_dwmac_parse_data(struct stm32_dwmac *dwmac,
				  struct device *dev)
{
	struct device_node *np = dev->of_node;
	int err;

	/*  Get TX/RX clocks */
	dwmac->clk_tx = devm_clk_get(dev, "mac-clk-tx");
	if (IS_ERR(dwmac->clk_tx)) {
		dev_err(dev, "No ETH Tx clock provided...\n");
		return PTR_ERR(dwmac->clk_tx);
	}

	dwmac->clk_rx = devm_clk_get(dev, "mac-clk-rx");
	if (IS_ERR(dwmac->clk_rx)) {
		dev_err(dev, "No ETH Rx clock provided...\n");
		return PTR_ERR(dwmac->clk_rx);
	}

	if (dwmac->ops->parse_data) {
		err = dwmac->ops->parse_data(dwmac, dev);
		if (err)
			return err;
	}

	/* Get mode register */
	dwmac->regmap = syscon_regmap_lookup_by_phandle(np, "st,syscon");
	if (IS_ERR(dwmac->regmap))
		return PTR_ERR(dwmac->regmap);

	err = of_property_read_u32_index(np, "st,syscon", 1, &dwmac->mode_reg);
	if (err)
		dev_err(dev, "Can't get sysconfig mode offset (%d)\n", err);

	return err;
}

static int stm32mp1_parse_data(struct stm32_dwmac *dwmac,
			       struct device *dev)
{
	struct platform_device *pdev = to_platform_device(dev);
	struct device_node *np = dev->of_node;
	int err = 0;

	/* Ethernet PHY have no crystal */
	dwmac->ext_phyclk = of_property_read_bool(np, "st,ext-phyclk");

	/* Gigabit Ethernet 125MHz clock selection. */
	dwmac->eth_clk_sel_reg = of_property_read_bool(np, "st,eth-clk-sel");

	/* Ethernet 50Mhz RMII clock selection */
	dwmac->eth_ref_clk_sel_reg =
		of_property_read_bool(np, "st,eth-ref-clk-sel");

	/*  Get ETH_CLK clocks */
	dwmac->clk_eth_ck = devm_clk_get(dev, "eth-ck");
	if (IS_ERR(dwmac->clk_eth_ck)) {
		dev_info(dev, "No phy clock provided...\n");
		dwmac->clk_eth_ck = NULL;
	}

	/*  Clock used for low power mode */
	dwmac->clk_ethstp = devm_clk_get(dev, "ethstp");
	if (IS_ERR(dwmac->clk_ethstp)) {
		dev_err(dev,
			"No ETH peripheral clock provided for CStop mode ...\n");
		return PTR_ERR(dwmac->clk_ethstp);
	}

	/*  Optional Clock for sysconfig */
	dwmac->syscfg_clk = devm_clk_get(dev, "syscfg-clk");
	if (IS_ERR(dwmac->syscfg_clk))
		dwmac->syscfg_clk = NULL;

	/* Get IRQ information early to have an ability to ask for deferred
	 * probe if needed before we went too far with resource allocation.
	 */
	dwmac->irq_pwr_wakeup = platform_get_irq_byname_optional(pdev,
							"stm32_pwr_wakeup");
	if (dwmac->irq_pwr_wakeup == -EPROBE_DEFER)
		return -EPROBE_DEFER;

	if (!dwmac->clk_eth_ck && dwmac->irq_pwr_wakeup >= 0) {
		err = device_init_wakeup(&pdev->dev, true);
		if (err) {
			dev_err(&pdev->dev, "Failed to init wake up irq\n");
			return err;
		}
		err = dev_pm_set_dedicated_wake_irq(&pdev->dev,
						    dwmac->irq_pwr_wakeup);
		if (err) {
			dev_err(&pdev->dev, "Failed to set wake up irq\n");
			device_init_wakeup(&pdev->dev, false);
		}
		device_set_wakeup_enable(&pdev->dev, false);
	}
	return err;
}

static int stm32_dwmac_probe(struct platform_device *pdev)
{
	struct plat_stmmacenet_data *plat_dat;
	struct stmmac_resources stmmac_res;
	struct stm32_dwmac *dwmac;
	const struct stm32_ops *data;
	int ret;

	ret = stmmac_get_platform_resources(pdev, &stmmac_res);
	if (ret)
		return ret;

	plat_dat = stmmac_probe_config_dt(pdev, stmmac_res.mac);
	if (IS_ERR(plat_dat))
		return PTR_ERR(plat_dat);

	dwmac = devm_kzalloc(&pdev->dev, sizeof(*dwmac), GFP_KERNEL);
	if (!dwmac) {
		ret = -ENOMEM;
		goto err_remove_config_dt;
	}

	data = of_device_get_match_data(&pdev->dev);
	if (!data) {
		dev_err(&pdev->dev, "no of match data provided\n");
		ret = -EINVAL;
		goto err_remove_config_dt;
	}

	dwmac->ops = data;
	dwmac->dev = &pdev->dev;

	ret = stm32_dwmac_parse_data(dwmac, &pdev->dev);
	if (ret) {
		dev_err(&pdev->dev, "Unable to parse OF data\n");
		goto err_remove_config_dt;
	}

	plat_dat->bsp_priv = dwmac;

	ret = stm32_dwmac_init(plat_dat);
	if (ret)
		goto err_remove_config_dt;

	ret = stmmac_dvr_probe(&pdev->dev, plat_dat, &stmmac_res);
	if (ret)
		goto err_clk_disable;

	return 0;

err_clk_disable:
	stm32_dwmac_clk_disable(dwmac);
err_remove_config_dt:
	stmmac_remove_config_dt(pdev, plat_dat);

	return ret;
}

static int stm32_dwmac_remove(struct platform_device *pdev)
{
	struct net_device *ndev = platform_get_drvdata(pdev);
	struct stmmac_priv *priv = netdev_priv(ndev);
	int ret = stmmac_dvr_remove(&pdev->dev);
	struct stm32_dwmac *dwmac = priv->plat->bsp_priv;

	stm32_dwmac_clk_disable(priv->plat->bsp_priv);

	if (dwmac->irq_pwr_wakeup >= 0) {
		dev_pm_clear_wake_irq(&pdev->dev);
		device_init_wakeup(&pdev->dev, false);
	}

	return ret;
}

static int stm32mp1_suspend(struct stm32_dwmac *dwmac)
{
	int ret = 0;

	ret = clk_prepare_enable(dwmac->clk_ethstp);
	if (ret)
		return ret;

	clk_disable_unprepare(dwmac->clk_tx);
	clk_disable_unprepare(dwmac->syscfg_clk);
	if (dwmac->enable_eth_ck)
		clk_disable_unprepare(dwmac->clk_eth_ck);

	return ret;
}

static void stm32mp1_resume(struct stm32_dwmac *dwmac)
{
	clk_disable_unprepare(dwmac->clk_ethstp);
}

static int stm32mcu_suspend(struct stm32_dwmac *dwmac)
{
	clk_disable_unprepare(dwmac->clk_tx);
	clk_disable_unprepare(dwmac->clk_rx);

	return 0;
}

#ifdef CONFIG_PM_SLEEP
static int stm32_dwmac_suspend(struct device *dev)
{
	struct net_device *ndev = dev_get_drvdata(dev);
	struct stmmac_priv *priv = netdev_priv(ndev);
	struct stm32_dwmac *dwmac = priv->plat->bsp_priv;

	int ret;

	ret = stmmac_suspend(dev);

	if (dwmac->ops->suspend)
		ret = dwmac->ops->suspend(dwmac);

	return ret;
}

static int stm32_dwmac_resume(struct device *dev)
{
	struct net_device *ndev = dev_get_drvdata(dev);
	struct stmmac_priv *priv = netdev_priv(ndev);
	struct stm32_dwmac *dwmac = priv->plat->bsp_priv;
	int ret;

	if (dwmac->ops->resume)
		dwmac->ops->resume(dwmac);

	ret = stm32_dwmac_init(priv->plat);
	if (ret)
		return ret;

	ret = stmmac_resume(dev);

	return ret;
}
#endif /* CONFIG_PM_SLEEP */

static SIMPLE_DEV_PM_OPS(stm32_dwmac_pm_ops,
	stm32_dwmac_suspend, stm32_dwmac_resume);

static struct stm32_ops stm32mcu_dwmac_data = {
	.set_mode = stm32mcu_set_mode,
	.suspend = stm32mcu_suspend,
	.syscfg_eth_mask = SYSCFG_MCU_ETH_MASK
};

static struct stm32_ops stm32mp1_dwmac_data = {
	.set_mode = stm32mp1_set_mode,
	.clk_prepare = stm32mp1_clk_prepare,
	.suspend = stm32mp1_suspend,
	.resume = stm32mp1_resume,
	.parse_data = stm32mp1_parse_data,
	.syscfg_eth_mask = SYSCFG_MP1_ETH_MASK
};

static const struct of_device_id stm32_dwmac_match[] = {
	{ .compatible = "st,stm32-dwmac", .data = &stm32mcu_dwmac_data},
	{ .compatible = "st,stm32mp1-dwmac", .data = &stm32mp1_dwmac_data},
	{ }
};
MODULE_DEVICE_TABLE(of, stm32_dwmac_match);

static struct platform_driver stm32_dwmac_driver = {
	.probe  = stm32_dwmac_probe,
	.remove = stm32_dwmac_remove,
	.driver = {
		.name           = "stm32-dwmac",
		.pm		= &stm32_dwmac_pm_ops,
		.of_match_table = stm32_dwmac_match,
	},
};
module_platform_driver(stm32_dwmac_driver);

MODULE_AUTHOR("Alexandre Torgue <alexandre.torgue@gmail.com>");
MODULE_AUTHOR("Christophe Roullier <christophe.roullier@st.com>");
MODULE_DESCRIPTION("STMicroelectronics STM32 DWMAC Specific Glue layer");
MODULE_LICENSE("GPL v2");
