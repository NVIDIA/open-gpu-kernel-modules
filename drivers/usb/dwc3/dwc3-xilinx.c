// SPDX-License-Identifier: GPL-2.0
/*
 * dwc3-xilinx.c - Xilinx DWC3 controller specific glue driver
 *
 * Authors: Manish Narani <manish.narani@xilinx.com>
 *          Anurag Kumar Vulisha <anurag.kumar.vulisha@xilinx.com>
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/clk.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/of_platform.h>
#include <linux/pm_runtime.h>
#include <linux/reset.h>
#include <linux/of_address.h>
#include <linux/delay.h>
#include <linux/firmware/xlnx-zynqmp.h>
#include <linux/io.h>

#include <linux/phy/phy.h>

/* USB phy reset mask register */
#define XLNX_USB_PHY_RST_EN			0x001C
#define XLNX_PHY_RST_MASK			0x1

/* Xilinx USB 3.0 IP Register */
#define XLNX_USB_TRAFFIC_ROUTE_CONFIG		0x005C
#define XLNX_USB_TRAFFIC_ROUTE_FPD		0x1

/* Versal USB Reset ID */
#define VERSAL_USB_RESET_ID			0xC104036

#define XLNX_USB_FPD_PIPE_CLK			0x7c
#define PIPE_CLK_DESELECT			1
#define PIPE_CLK_SELECT				0
#define XLNX_USB_FPD_POWER_PRSNT		0x80
#define FPD_POWER_PRSNT_OPTION			BIT(0)

struct dwc3_xlnx {
	int				num_clocks;
	struct clk_bulk_data		*clks;
	struct device			*dev;
	void __iomem			*regs;
	int				(*pltfm_init)(struct dwc3_xlnx *data);
};

static void dwc3_xlnx_mask_phy_rst(struct dwc3_xlnx *priv_data, bool mask)
{
	u32 reg;

	/*
	 * Enable or disable ULPI PHY reset from USB Controller.
	 * This does not actually reset the phy, but just controls
	 * whether USB controller can or cannot reset ULPI PHY.
	 */
	reg = readl(priv_data->regs + XLNX_USB_PHY_RST_EN);

	if (mask)
		reg &= ~XLNX_PHY_RST_MASK;
	else
		reg |= XLNX_PHY_RST_MASK;

	writel(reg, priv_data->regs + XLNX_USB_PHY_RST_EN);
}

static int dwc3_xlnx_init_versal(struct dwc3_xlnx *priv_data)
{
	struct device		*dev = priv_data->dev;
	int			ret;

	dwc3_xlnx_mask_phy_rst(priv_data, false);

	/* Assert and De-assert reset */
	ret = zynqmp_pm_reset_assert(VERSAL_USB_RESET_ID,
				     PM_RESET_ACTION_ASSERT);
	if (ret < 0) {
		dev_err_probe(dev, ret, "failed to assert Reset\n");
		return ret;
	}

	ret = zynqmp_pm_reset_assert(VERSAL_USB_RESET_ID,
				     PM_RESET_ACTION_RELEASE);
	if (ret < 0) {
		dev_err_probe(dev, ret, "failed to De-assert Reset\n");
		return ret;
	}

	dwc3_xlnx_mask_phy_rst(priv_data, true);

	return 0;
}

static int dwc3_xlnx_init_zynqmp(struct dwc3_xlnx *priv_data)
{
	struct device		*dev = priv_data->dev;
	struct reset_control	*crst, *hibrst, *apbrst;
	struct phy		*usb3_phy;
	int			ret;
	u32			reg;

	usb3_phy = devm_phy_get(dev, "usb3-phy");
	if (PTR_ERR(usb3_phy) == -EPROBE_DEFER) {
		ret = -EPROBE_DEFER;
		goto err;
	} else if (IS_ERR(usb3_phy)) {
		usb3_phy = NULL;
	}

	crst = devm_reset_control_get_exclusive(dev, "usb_crst");
	if (IS_ERR(crst)) {
		ret = PTR_ERR(crst);
		dev_err_probe(dev, ret,
			      "failed to get core reset signal\n");
		goto err;
	}

	hibrst = devm_reset_control_get_exclusive(dev, "usb_hibrst");
	if (IS_ERR(hibrst)) {
		ret = PTR_ERR(hibrst);
		dev_err_probe(dev, ret,
			      "failed to get hibernation reset signal\n");
		goto err;
	}

	apbrst = devm_reset_control_get_exclusive(dev, "usb_apbrst");
	if (IS_ERR(apbrst)) {
		ret = PTR_ERR(apbrst);
		dev_err_probe(dev, ret,
			      "failed to get APB reset signal\n");
		goto err;
	}

	ret = reset_control_assert(crst);
	if (ret < 0) {
		dev_err(dev, "Failed to assert core reset\n");
		goto err;
	}

	ret = reset_control_assert(hibrst);
	if (ret < 0) {
		dev_err(dev, "Failed to assert hibernation reset\n");
		goto err;
	}

	ret = reset_control_assert(apbrst);
	if (ret < 0) {
		dev_err(dev, "Failed to assert APB reset\n");
		goto err;
	}

	ret = phy_init(usb3_phy);
	if (ret < 0) {
		phy_exit(usb3_phy);
		goto err;
	}

	ret = reset_control_deassert(apbrst);
	if (ret < 0) {
		dev_err(dev, "Failed to release APB reset\n");
		goto err;
	}

	/* Set PIPE Power Present signal in FPD Power Present Register*/
	writel(FPD_POWER_PRSNT_OPTION, priv_data->regs + XLNX_USB_FPD_POWER_PRSNT);

	/* Set the PIPE Clock Select bit in FPD PIPE Clock register */
	writel(PIPE_CLK_SELECT, priv_data->regs + XLNX_USB_FPD_PIPE_CLK);

	ret = reset_control_deassert(crst);
	if (ret < 0) {
		dev_err(dev, "Failed to release core reset\n");
		goto err;
	}

	ret = reset_control_deassert(hibrst);
	if (ret < 0) {
		dev_err(dev, "Failed to release hibernation reset\n");
		goto err;
	}

	ret = phy_power_on(usb3_phy);
	if (ret < 0) {
		phy_exit(usb3_phy);
		goto err;
	}

	/*
	 * This routes the USB DMA traffic to go through FPD path instead
	 * of reaching DDR directly. This traffic routing is needed to
	 * make SMMU and CCI work with USB DMA.
	 */
	if (of_dma_is_coherent(dev->of_node) || device_iommu_mapped(dev)) {
		reg = readl(priv_data->regs + XLNX_USB_TRAFFIC_ROUTE_CONFIG);
		reg |= XLNX_USB_TRAFFIC_ROUTE_FPD;
		writel(reg, priv_data->regs + XLNX_USB_TRAFFIC_ROUTE_CONFIG);
	}

err:
	return ret;
}

static const struct of_device_id dwc3_xlnx_of_match[] = {
	{
		.compatible = "xlnx,zynqmp-dwc3",
		.data = &dwc3_xlnx_init_zynqmp,
	},
	{
		.compatible = "xlnx,versal-dwc3",
		.data = &dwc3_xlnx_init_versal,
	},
	{ /* Sentinel */ }
};
MODULE_DEVICE_TABLE(of, dwc3_xlnx_of_match);

static int dwc3_xlnx_probe(struct platform_device *pdev)
{
	struct dwc3_xlnx		*priv_data;
	struct device			*dev = &pdev->dev;
	struct device_node		*np = dev->of_node;
	const struct of_device_id	*match;
	void __iomem			*regs;
	int				ret;

	priv_data = devm_kzalloc(dev, sizeof(*priv_data), GFP_KERNEL);
	if (!priv_data)
		return -ENOMEM;

	regs = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(regs)) {
		ret = PTR_ERR(regs);
		dev_err_probe(dev, ret, "failed to map registers\n");
		return ret;
	}

	match = of_match_node(dwc3_xlnx_of_match, pdev->dev.of_node);

	priv_data->pltfm_init = match->data;
	priv_data->regs = regs;
	priv_data->dev = dev;

	platform_set_drvdata(pdev, priv_data);

	ret = devm_clk_bulk_get_all(priv_data->dev, &priv_data->clks);
	if (ret < 0)
		return ret;

	priv_data->num_clocks = ret;

	ret = clk_bulk_prepare_enable(priv_data->num_clocks, priv_data->clks);
	if (ret)
		return ret;

	ret = priv_data->pltfm_init(priv_data);
	if (ret)
		goto err_clk_put;

	ret = of_platform_populate(np, NULL, NULL, dev);
	if (ret)
		goto err_clk_put;

	pm_runtime_set_active(dev);
	pm_runtime_enable(dev);
	pm_suspend_ignore_children(dev, false);
	pm_runtime_get_sync(dev);

	return 0;

err_clk_put:
	clk_bulk_disable_unprepare(priv_data->num_clocks, priv_data->clks);

	return ret;
}

static int dwc3_xlnx_remove(struct platform_device *pdev)
{
	struct dwc3_xlnx	*priv_data = platform_get_drvdata(pdev);
	struct device		*dev = &pdev->dev;

	of_platform_depopulate(dev);

	clk_bulk_disable_unprepare(priv_data->num_clocks, priv_data->clks);
	priv_data->num_clocks = 0;

	pm_runtime_disable(dev);
	pm_runtime_put_noidle(dev);
	pm_runtime_set_suspended(dev);

	return 0;
}

static int __maybe_unused dwc3_xlnx_suspend_common(struct device *dev)
{
	struct dwc3_xlnx *priv_data = dev_get_drvdata(dev);

	clk_bulk_disable(priv_data->num_clocks, priv_data->clks);

	return 0;
}

static int __maybe_unused dwc3_xlnx_resume_common(struct device *dev)
{
	struct dwc3_xlnx *priv_data = dev_get_drvdata(dev);

	return clk_bulk_enable(priv_data->num_clocks, priv_data->clks);
}

static int __maybe_unused dwc3_xlnx_runtime_idle(struct device *dev)
{
	pm_runtime_mark_last_busy(dev);
	pm_runtime_autosuspend(dev);

	return 0;
}

static UNIVERSAL_DEV_PM_OPS(dwc3_xlnx_dev_pm_ops, dwc3_xlnx_suspend_common,
			    dwc3_xlnx_resume_common, dwc3_xlnx_runtime_idle);

static struct platform_driver dwc3_xlnx_driver = {
	.probe		= dwc3_xlnx_probe,
	.remove		= dwc3_xlnx_remove,
	.driver		= {
		.name		= "dwc3-xilinx",
		.of_match_table	= dwc3_xlnx_of_match,
		.pm		= &dwc3_xlnx_dev_pm_ops,
	},
};

module_platform_driver(dwc3_xlnx_driver);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Xilinx DWC3 controller specific glue driver");
MODULE_AUTHOR("Manish Narani <manish.narani@xilinx.com>");
MODULE_AUTHOR("Anurag Kumar Vulisha <anurag.kumar.vulisha@xilinx.com>");
