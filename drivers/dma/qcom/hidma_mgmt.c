// SPDX-License-Identifier: GPL-2.0-only
/*
 * Qualcomm Technologies HIDMA DMA engine Management interface
 *
 * Copyright (c) 2015-2017, The Linux Foundation. All rights reserved.
 */

#include <linux/dmaengine.h>
#include <linux/acpi.h>
#include <linux/of.h>
#include <linux/property.h>
#include <linux/of_address.h>
#include <linux/of_irq.h>
#include <linux/of_platform.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/pm_runtime.h>
#include <linux/bitops.h>
#include <linux/dma-mapping.h>

#include "hidma_mgmt.h"

#define HIDMA_QOS_N_OFFSET		0x700
#define HIDMA_CFG_OFFSET		0x400
#define HIDMA_MAX_BUS_REQ_LEN_OFFSET	0x41C
#define HIDMA_MAX_XACTIONS_OFFSET	0x420
#define HIDMA_HW_VERSION_OFFSET	0x424
#define HIDMA_CHRESET_TIMEOUT_OFFSET	0x418

#define HIDMA_MAX_WR_XACTIONS_MASK	GENMASK(4, 0)
#define HIDMA_MAX_RD_XACTIONS_MASK	GENMASK(4, 0)
#define HIDMA_WEIGHT_MASK		GENMASK(6, 0)
#define HIDMA_MAX_BUS_REQ_LEN_MASK	GENMASK(15, 0)
#define HIDMA_CHRESET_TIMEOUT_MASK	GENMASK(19, 0)

#define HIDMA_MAX_WR_XACTIONS_BIT_POS	16
#define HIDMA_MAX_BUS_WR_REQ_BIT_POS	16
#define HIDMA_WRR_BIT_POS		8
#define HIDMA_PRIORITY_BIT_POS		15

#define HIDMA_AUTOSUSPEND_TIMEOUT	2000
#define HIDMA_MAX_CHANNEL_WEIGHT	15

static unsigned int max_write_request;
module_param(max_write_request, uint, 0644);
MODULE_PARM_DESC(max_write_request,
		"maximum write burst (default: ACPI/DT value)");

static unsigned int max_read_request;
module_param(max_read_request, uint, 0644);
MODULE_PARM_DESC(max_read_request,
		"maximum read burst (default: ACPI/DT value)");

static unsigned int max_wr_xactions;
module_param(max_wr_xactions, uint, 0644);
MODULE_PARM_DESC(max_wr_xactions,
	"maximum number of write transactions (default: ACPI/DT value)");

static unsigned int max_rd_xactions;
module_param(max_rd_xactions, uint, 0644);
MODULE_PARM_DESC(max_rd_xactions,
	"maximum number of read transactions (default: ACPI/DT value)");

int hidma_mgmt_setup(struct hidma_mgmt_dev *mgmtdev)
{
	unsigned int i;
	u32 val;

	if (!is_power_of_2(mgmtdev->max_write_request) ||
	    (mgmtdev->max_write_request < 128) ||
	    (mgmtdev->max_write_request > 1024)) {
		dev_err(&mgmtdev->pdev->dev, "invalid write request %d\n",
			mgmtdev->max_write_request);
		return -EINVAL;
	}

	if (!is_power_of_2(mgmtdev->max_read_request) ||
	    (mgmtdev->max_read_request < 128) ||
	    (mgmtdev->max_read_request > 1024)) {
		dev_err(&mgmtdev->pdev->dev, "invalid read request %d\n",
			mgmtdev->max_read_request);
		return -EINVAL;
	}

	if (mgmtdev->max_wr_xactions > HIDMA_MAX_WR_XACTIONS_MASK) {
		dev_err(&mgmtdev->pdev->dev,
			"max_wr_xactions cannot be bigger than %ld\n",
			HIDMA_MAX_WR_XACTIONS_MASK);
		return -EINVAL;
	}

	if (mgmtdev->max_rd_xactions > HIDMA_MAX_RD_XACTIONS_MASK) {
		dev_err(&mgmtdev->pdev->dev,
			"max_rd_xactions cannot be bigger than %ld\n",
			HIDMA_MAX_RD_XACTIONS_MASK);
		return -EINVAL;
	}

	for (i = 0; i < mgmtdev->dma_channels; i++) {
		if (mgmtdev->priority[i] > 1) {
			dev_err(&mgmtdev->pdev->dev,
				"priority can be 0 or 1\n");
			return -EINVAL;
		}

		if (mgmtdev->weight[i] > HIDMA_MAX_CHANNEL_WEIGHT) {
			dev_err(&mgmtdev->pdev->dev,
				"max value of weight can be %d.\n",
				HIDMA_MAX_CHANNEL_WEIGHT);
			return -EINVAL;
		}

		/* weight needs to be at least one */
		if (mgmtdev->weight[i] == 0)
			mgmtdev->weight[i] = 1;
	}

	pm_runtime_get_sync(&mgmtdev->pdev->dev);
	val = readl(mgmtdev->virtaddr + HIDMA_MAX_BUS_REQ_LEN_OFFSET);
	val &= ~(HIDMA_MAX_BUS_REQ_LEN_MASK << HIDMA_MAX_BUS_WR_REQ_BIT_POS);
	val |= mgmtdev->max_write_request << HIDMA_MAX_BUS_WR_REQ_BIT_POS;
	val &= ~HIDMA_MAX_BUS_REQ_LEN_MASK;
	val |= mgmtdev->max_read_request;
	writel(val, mgmtdev->virtaddr + HIDMA_MAX_BUS_REQ_LEN_OFFSET);

	val = readl(mgmtdev->virtaddr + HIDMA_MAX_XACTIONS_OFFSET);
	val &= ~(HIDMA_MAX_WR_XACTIONS_MASK << HIDMA_MAX_WR_XACTIONS_BIT_POS);
	val |= mgmtdev->max_wr_xactions << HIDMA_MAX_WR_XACTIONS_BIT_POS;
	val &= ~HIDMA_MAX_RD_XACTIONS_MASK;
	val |= mgmtdev->max_rd_xactions;
	writel(val, mgmtdev->virtaddr + HIDMA_MAX_XACTIONS_OFFSET);

	mgmtdev->hw_version =
	    readl(mgmtdev->virtaddr + HIDMA_HW_VERSION_OFFSET);
	mgmtdev->hw_version_major = (mgmtdev->hw_version >> 28) & 0xF;
	mgmtdev->hw_version_minor = (mgmtdev->hw_version >> 16) & 0xF;

	for (i = 0; i < mgmtdev->dma_channels; i++) {
		u32 weight = mgmtdev->weight[i];
		u32 priority = mgmtdev->priority[i];

		val = readl(mgmtdev->virtaddr + HIDMA_QOS_N_OFFSET + (4 * i));
		val &= ~(1 << HIDMA_PRIORITY_BIT_POS);
		val |= (priority & 0x1) << HIDMA_PRIORITY_BIT_POS;
		val &= ~(HIDMA_WEIGHT_MASK << HIDMA_WRR_BIT_POS);
		val |= (weight & HIDMA_WEIGHT_MASK) << HIDMA_WRR_BIT_POS;
		writel(val, mgmtdev->virtaddr + HIDMA_QOS_N_OFFSET + (4 * i));
	}

	val = readl(mgmtdev->virtaddr + HIDMA_CHRESET_TIMEOUT_OFFSET);
	val &= ~HIDMA_CHRESET_TIMEOUT_MASK;
	val |= mgmtdev->chreset_timeout_cycles & HIDMA_CHRESET_TIMEOUT_MASK;
	writel(val, mgmtdev->virtaddr + HIDMA_CHRESET_TIMEOUT_OFFSET);

	pm_runtime_mark_last_busy(&mgmtdev->pdev->dev);
	pm_runtime_put_autosuspend(&mgmtdev->pdev->dev);
	return 0;
}
EXPORT_SYMBOL_GPL(hidma_mgmt_setup);

static int hidma_mgmt_probe(struct platform_device *pdev)
{
	struct hidma_mgmt_dev *mgmtdev;
	struct resource *res;
	void __iomem *virtaddr;
	int irq;
	int rc;
	u32 val;

	pm_runtime_set_autosuspend_delay(&pdev->dev, HIDMA_AUTOSUSPEND_TIMEOUT);
	pm_runtime_use_autosuspend(&pdev->dev);
	pm_runtime_set_active(&pdev->dev);
	pm_runtime_enable(&pdev->dev);
	pm_runtime_get_sync(&pdev->dev);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	virtaddr = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(virtaddr)) {
		rc = -ENOMEM;
		goto out;
	}

	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		rc = irq;
		goto out;
	}

	mgmtdev = devm_kzalloc(&pdev->dev, sizeof(*mgmtdev), GFP_KERNEL);
	if (!mgmtdev) {
		rc = -ENOMEM;
		goto out;
	}

	mgmtdev->pdev = pdev;
	mgmtdev->addrsize = resource_size(res);
	mgmtdev->virtaddr = virtaddr;

	rc = device_property_read_u32(&pdev->dev, "dma-channels",
				      &mgmtdev->dma_channels);
	if (rc) {
		dev_err(&pdev->dev, "number of channels missing\n");
		goto out;
	}

	rc = device_property_read_u32(&pdev->dev,
				      "channel-reset-timeout-cycles",
				      &mgmtdev->chreset_timeout_cycles);
	if (rc) {
		dev_err(&pdev->dev, "channel reset timeout missing\n");
		goto out;
	}

	rc = device_property_read_u32(&pdev->dev, "max-write-burst-bytes",
				      &mgmtdev->max_write_request);
	if (rc) {
		dev_err(&pdev->dev, "max-write-burst-bytes missing\n");
		goto out;
	}

	if (max_write_request &&
			(max_write_request != mgmtdev->max_write_request)) {
		dev_info(&pdev->dev, "overriding max-write-burst-bytes: %d\n",
			max_write_request);
		mgmtdev->max_write_request = max_write_request;
	} else
		max_write_request = mgmtdev->max_write_request;

	rc = device_property_read_u32(&pdev->dev, "max-read-burst-bytes",
				      &mgmtdev->max_read_request);
	if (rc) {
		dev_err(&pdev->dev, "max-read-burst-bytes missing\n");
		goto out;
	}
	if (max_read_request &&
			(max_read_request != mgmtdev->max_read_request)) {
		dev_info(&pdev->dev, "overriding max-read-burst-bytes: %d\n",
			max_read_request);
		mgmtdev->max_read_request = max_read_request;
	} else
		max_read_request = mgmtdev->max_read_request;

	rc = device_property_read_u32(&pdev->dev, "max-write-transactions",
				      &mgmtdev->max_wr_xactions);
	if (rc) {
		dev_err(&pdev->dev, "max-write-transactions missing\n");
		goto out;
	}
	if (max_wr_xactions &&
			(max_wr_xactions != mgmtdev->max_wr_xactions)) {
		dev_info(&pdev->dev, "overriding max-write-transactions: %d\n",
			max_wr_xactions);
		mgmtdev->max_wr_xactions = max_wr_xactions;
	} else
		max_wr_xactions = mgmtdev->max_wr_xactions;

	rc = device_property_read_u32(&pdev->dev, "max-read-transactions",
				      &mgmtdev->max_rd_xactions);
	if (rc) {
		dev_err(&pdev->dev, "max-read-transactions missing\n");
		goto out;
	}
	if (max_rd_xactions &&
			(max_rd_xactions != mgmtdev->max_rd_xactions)) {
		dev_info(&pdev->dev, "overriding max-read-transactions: %d\n",
			max_rd_xactions);
		mgmtdev->max_rd_xactions = max_rd_xactions;
	} else
		max_rd_xactions = mgmtdev->max_rd_xactions;

	mgmtdev->priority = devm_kcalloc(&pdev->dev,
					 mgmtdev->dma_channels,
					 sizeof(*mgmtdev->priority),
					 GFP_KERNEL);
	if (!mgmtdev->priority) {
		rc = -ENOMEM;
		goto out;
	}

	mgmtdev->weight = devm_kcalloc(&pdev->dev,
				       mgmtdev->dma_channels,
				       sizeof(*mgmtdev->weight), GFP_KERNEL);
	if (!mgmtdev->weight) {
		rc = -ENOMEM;
		goto out;
	}

	rc = hidma_mgmt_setup(mgmtdev);
	if (rc) {
		dev_err(&pdev->dev, "setup failed\n");
		goto out;
	}

	/* start the HW */
	val = readl(mgmtdev->virtaddr + HIDMA_CFG_OFFSET);
	val |= 1;
	writel(val, mgmtdev->virtaddr + HIDMA_CFG_OFFSET);

	rc = hidma_mgmt_init_sys(mgmtdev);
	if (rc) {
		dev_err(&pdev->dev, "sysfs setup failed\n");
		goto out;
	}

	dev_info(&pdev->dev,
		 "HW rev: %d.%d @ %pa with %d physical channels\n",
		 mgmtdev->hw_version_major, mgmtdev->hw_version_minor,
		 &res->start, mgmtdev->dma_channels);

	platform_set_drvdata(pdev, mgmtdev);
	pm_runtime_mark_last_busy(&pdev->dev);
	pm_runtime_put_autosuspend(&pdev->dev);
	return 0;
out:
	pm_runtime_put_sync_suspend(&pdev->dev);
	pm_runtime_disable(&pdev->dev);
	return rc;
}

#if IS_ENABLED(CONFIG_ACPI)
static const struct acpi_device_id hidma_mgmt_acpi_ids[] = {
	{"QCOM8060"},
	{},
};
MODULE_DEVICE_TABLE(acpi, hidma_mgmt_acpi_ids);
#endif

static const struct of_device_id hidma_mgmt_match[] = {
	{.compatible = "qcom,hidma-mgmt-1.0",},
	{},
};
MODULE_DEVICE_TABLE(of, hidma_mgmt_match);

static struct platform_driver hidma_mgmt_driver = {
	.probe = hidma_mgmt_probe,
	.driver = {
		   .name = "hidma-mgmt",
		   .of_match_table = hidma_mgmt_match,
		   .acpi_match_table = ACPI_PTR(hidma_mgmt_acpi_ids),
	},
};

#if defined(CONFIG_OF) && defined(CONFIG_OF_IRQ)
static int object_counter;

static int __init hidma_mgmt_of_populate_channels(struct device_node *np)
{
	struct platform_device *pdev_parent = of_find_device_by_node(np);
	struct platform_device_info pdevinfo;
	struct device_node *child;
	struct resource *res;
	int ret = 0;

	/* allocate a resource array */
	res = kcalloc(3, sizeof(*res), GFP_KERNEL);
	if (!res)
		return -ENOMEM;

	for_each_available_child_of_node(np, child) {
		struct platform_device *new_pdev;

		ret = of_address_to_resource(child, 0, &res[0]);
		if (!ret)
			goto out;

		ret = of_address_to_resource(child, 1, &res[1]);
		if (!ret)
			goto out;

		ret = of_irq_to_resource(child, 0, &res[2]);
		if (ret <= 0)
			goto out;

		memset(&pdevinfo, 0, sizeof(pdevinfo));
		pdevinfo.fwnode = &child->fwnode;
		pdevinfo.parent = pdev_parent ? &pdev_parent->dev : NULL;
		pdevinfo.name = child->name;
		pdevinfo.id = object_counter++;
		pdevinfo.res = res;
		pdevinfo.num_res = 3;
		pdevinfo.data = NULL;
		pdevinfo.size_data = 0;
		pdevinfo.dma_mask = DMA_BIT_MASK(64);
		new_pdev = platform_device_register_full(&pdevinfo);
		if (IS_ERR(new_pdev)) {
			ret = PTR_ERR(new_pdev);
			goto out;
		}
		new_pdev->dev.of_node = child;
		of_dma_configure(&new_pdev->dev, child, true);
		/*
		 * It is assumed that calling of_msi_configure is safe on
		 * platforms with or without MSI support.
		 */
		of_msi_configure(&new_pdev->dev, child);
	}

	kfree(res);

	return ret;

out:
	of_node_put(child);
	kfree(res);

	return ret;
}
#endif

static int __init hidma_mgmt_init(void)
{
#if defined(CONFIG_OF) && defined(CONFIG_OF_IRQ)
	struct device_node *child;

	for_each_matching_node(child, hidma_mgmt_match) {
		/* device tree based firmware here */
		hidma_mgmt_of_populate_channels(child);
	}
#endif
	/*
	 * We do not check for return value here, as it is assumed that
	 * platform_driver_register must not fail. The reason for this is that
	 * the (potential) hidma_mgmt_of_populate_channels calls above are not
	 * cleaned up if it does fail, and to do this work is quite
	 * complicated. In particular, various calls of of_address_to_resource,
	 * of_irq_to_resource, platform_device_register_full, of_dma_configure,
	 * and of_msi_configure which then call other functions and so on, must
	 * be cleaned up - this is not a trivial exercise.
	 *
	 * Currently, this module is not intended to be unloaded, and there is
	 * no module_exit function defined which does the needed cleanup. For
	 * this reason, we have to assume success here.
	 */
	platform_driver_register(&hidma_mgmt_driver);

	return 0;
}
module_init(hidma_mgmt_init);
MODULE_LICENSE("GPL v2");
