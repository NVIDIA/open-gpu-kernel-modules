// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2011-2015, The Linux Foundation. All rights reserved.
 *
 * Description: CoreSight Replicator driver
 */

#include <linux/acpi.h>
#include <linux/amba/bus.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/pm_runtime.h>
#include <linux/property.h>
#include <linux/clk.h>
#include <linux/of.h>
#include <linux/coresight.h>

#include "coresight-priv.h"

#define REPLICATOR_IDFILTER0		0x000
#define REPLICATOR_IDFILTER1		0x004

DEFINE_CORESIGHT_DEVLIST(replicator_devs, "replicator");

/**
 * struct replicator_drvdata - specifics associated to a replicator component
 * @base:	memory mapped base address for this component. Also indicates
 *		whether this one is programmable or not.
 * @atclk:	optional clock for the core parts of the replicator.
 * @csdev:	component vitals needed by the framework
 * @spinlock:	serialize enable/disable operations.
 * @check_idfilter_val: check if the context is lost upon clock removal.
 */
struct replicator_drvdata {
	void __iomem		*base;
	struct clk		*atclk;
	struct coresight_device	*csdev;
	spinlock_t		spinlock;
	bool			check_idfilter_val;
};

static void dynamic_replicator_reset(struct replicator_drvdata *drvdata)
{
	struct coresight_device *csdev = drvdata->csdev;

	CS_UNLOCK(drvdata->base);

	if (!coresight_claim_device_unlocked(csdev)) {
		writel_relaxed(0xff, drvdata->base + REPLICATOR_IDFILTER0);
		writel_relaxed(0xff, drvdata->base + REPLICATOR_IDFILTER1);
		coresight_disclaim_device_unlocked(csdev);
	}

	CS_LOCK(drvdata->base);
}

/*
 * replicator_reset : Reset the replicator configuration to sane values.
 */
static inline void replicator_reset(struct replicator_drvdata *drvdata)
{
	if (drvdata->base)
		dynamic_replicator_reset(drvdata);
}

static int dynamic_replicator_enable(struct replicator_drvdata *drvdata,
				     int inport, int outport)
{
	int rc = 0;
	u32 id0val, id1val;
	struct coresight_device *csdev = drvdata->csdev;

	CS_UNLOCK(drvdata->base);

	id0val = readl_relaxed(drvdata->base + REPLICATOR_IDFILTER0);
	id1val = readl_relaxed(drvdata->base + REPLICATOR_IDFILTER1);

	/*
	 * Some replicator designs lose context when AMBA clocks are removed,
	 * so have a check for this.
	 */
	if (drvdata->check_idfilter_val && id0val == 0x0 && id1val == 0x0)
		id0val = id1val = 0xff;

	if (id0val == 0xff && id1val == 0xff)
		rc = coresight_claim_device_unlocked(csdev);

	if (!rc) {
		switch (outport) {
		case 0:
			id0val = 0x0;
			break;
		case 1:
			id1val = 0x0;
			break;
		default:
			WARN_ON(1);
			rc = -EINVAL;
		}
	}

	/* Ensure that the outport is enabled. */
	if (!rc) {
		writel_relaxed(id0val, drvdata->base + REPLICATOR_IDFILTER0);
		writel_relaxed(id1val, drvdata->base + REPLICATOR_IDFILTER1);
	}

	CS_LOCK(drvdata->base);

	return rc;
}

static int replicator_enable(struct coresight_device *csdev, int inport,
			     int outport)
{
	int rc = 0;
	struct replicator_drvdata *drvdata = dev_get_drvdata(csdev->dev.parent);
	unsigned long flags;
	bool first_enable = false;

	spin_lock_irqsave(&drvdata->spinlock, flags);
	if (atomic_read(&csdev->refcnt[outport]) == 0) {
		if (drvdata->base)
			rc = dynamic_replicator_enable(drvdata, inport,
						       outport);
		if (!rc)
			first_enable = true;
	}
	if (!rc)
		atomic_inc(&csdev->refcnt[outport]);
	spin_unlock_irqrestore(&drvdata->spinlock, flags);

	if (first_enable)
		dev_dbg(&csdev->dev, "REPLICATOR enabled\n");
	return rc;
}

static void dynamic_replicator_disable(struct replicator_drvdata *drvdata,
				       int inport, int outport)
{
	u32 reg;
	struct coresight_device *csdev = drvdata->csdev;

	switch (outport) {
	case 0:
		reg = REPLICATOR_IDFILTER0;
		break;
	case 1:
		reg = REPLICATOR_IDFILTER1;
		break;
	default:
		WARN_ON(1);
		return;
	}

	CS_UNLOCK(drvdata->base);

	/* disable the flow of ATB data through port */
	writel_relaxed(0xff, drvdata->base + reg);

	if ((readl_relaxed(drvdata->base + REPLICATOR_IDFILTER0) == 0xff) &&
	    (readl_relaxed(drvdata->base + REPLICATOR_IDFILTER1) == 0xff))
		coresight_disclaim_device_unlocked(csdev);
	CS_LOCK(drvdata->base);
}

static void replicator_disable(struct coresight_device *csdev, int inport,
			       int outport)
{
	struct replicator_drvdata *drvdata = dev_get_drvdata(csdev->dev.parent);
	unsigned long flags;
	bool last_disable = false;

	spin_lock_irqsave(&drvdata->spinlock, flags);
	if (atomic_dec_return(&csdev->refcnt[outport]) == 0) {
		if (drvdata->base)
			dynamic_replicator_disable(drvdata, inport, outport);
		last_disable = true;
	}
	spin_unlock_irqrestore(&drvdata->spinlock, flags);

	if (last_disable)
		dev_dbg(&csdev->dev, "REPLICATOR disabled\n");
}

static const struct coresight_ops_link replicator_link_ops = {
	.enable		= replicator_enable,
	.disable	= replicator_disable,
};

static const struct coresight_ops replicator_cs_ops = {
	.link_ops	= &replicator_link_ops,
};

#define coresight_replicator_reg(name, offset) \
	coresight_simple_reg32(struct replicator_drvdata, name, offset)

coresight_replicator_reg(idfilter0, REPLICATOR_IDFILTER0);
coresight_replicator_reg(idfilter1, REPLICATOR_IDFILTER1);

static struct attribute *replicator_mgmt_attrs[] = {
	&dev_attr_idfilter0.attr,
	&dev_attr_idfilter1.attr,
	NULL,
};

static const struct attribute_group replicator_mgmt_group = {
	.attrs = replicator_mgmt_attrs,
	.name = "mgmt",
};

static const struct attribute_group *replicator_groups[] = {
	&replicator_mgmt_group,
	NULL,
};

static int replicator_probe(struct device *dev, struct resource *res)
{
	int ret = 0;
	struct coresight_platform_data *pdata = NULL;
	struct replicator_drvdata *drvdata;
	struct coresight_desc desc = { 0 };
	void __iomem *base;

	if (is_of_node(dev_fwnode(dev)) &&
	    of_device_is_compatible(dev->of_node, "arm,coresight-replicator"))
		dev_warn_once(dev,
			      "Uses OBSOLETE CoreSight replicator binding\n");

	desc.name = coresight_alloc_device_name(&replicator_devs, dev);
	if (!desc.name)
		return -ENOMEM;

	drvdata = devm_kzalloc(dev, sizeof(*drvdata), GFP_KERNEL);
	if (!drvdata)
		return -ENOMEM;

	drvdata->atclk = devm_clk_get(dev, "atclk"); /* optional */
	if (!IS_ERR(drvdata->atclk)) {
		ret = clk_prepare_enable(drvdata->atclk);
		if (ret)
			return ret;
	}

	/*
	 * Map the device base for dynamic-replicator, which has been
	 * validated by AMBA core
	 */
	if (res) {
		base = devm_ioremap_resource(dev, res);
		if (IS_ERR(base)) {
			ret = PTR_ERR(base);
			goto out_disable_clk;
		}
		drvdata->base = base;
		desc.groups = replicator_groups;
		desc.access = CSDEV_ACCESS_IOMEM(base);
	}

	if (fwnode_property_present(dev_fwnode(dev),
				    "qcom,replicator-loses-context"))
		drvdata->check_idfilter_val = true;

	dev_set_drvdata(dev, drvdata);

	pdata = coresight_get_platform_data(dev);
	if (IS_ERR(pdata)) {
		ret = PTR_ERR(pdata);
		goto out_disable_clk;
	}
	dev->platform_data = pdata;

	spin_lock_init(&drvdata->spinlock);
	desc.type = CORESIGHT_DEV_TYPE_LINK;
	desc.subtype.link_subtype = CORESIGHT_DEV_SUBTYPE_LINK_SPLIT;
	desc.ops = &replicator_cs_ops;
	desc.pdata = dev->platform_data;
	desc.dev = dev;

	drvdata->csdev = coresight_register(&desc);
	if (IS_ERR(drvdata->csdev)) {
		ret = PTR_ERR(drvdata->csdev);
		goto out_disable_clk;
	}

	replicator_reset(drvdata);
	pm_runtime_put(dev);

out_disable_clk:
	if (ret && !IS_ERR_OR_NULL(drvdata->atclk))
		clk_disable_unprepare(drvdata->atclk);
	return ret;
}

static int replicator_remove(struct device *dev)
{
	struct replicator_drvdata *drvdata = dev_get_drvdata(dev);

	coresight_unregister(drvdata->csdev);
	return 0;
}

static int static_replicator_probe(struct platform_device *pdev)
{
	int ret;

	pm_runtime_get_noresume(&pdev->dev);
	pm_runtime_set_active(&pdev->dev);
	pm_runtime_enable(&pdev->dev);

	/* Static replicators do not have programming base */
	ret = replicator_probe(&pdev->dev, NULL);

	if (ret) {
		pm_runtime_put_noidle(&pdev->dev);
		pm_runtime_disable(&pdev->dev);
	}

	return ret;
}

static int static_replicator_remove(struct platform_device *pdev)
{
	replicator_remove(&pdev->dev);
	pm_runtime_disable(&pdev->dev);
	return 0;
}

#ifdef CONFIG_PM
static int replicator_runtime_suspend(struct device *dev)
{
	struct replicator_drvdata *drvdata = dev_get_drvdata(dev);

	if (drvdata && !IS_ERR(drvdata->atclk))
		clk_disable_unprepare(drvdata->atclk);

	return 0;
}

static int replicator_runtime_resume(struct device *dev)
{
	struct replicator_drvdata *drvdata = dev_get_drvdata(dev);

	if (drvdata && !IS_ERR(drvdata->atclk))
		clk_prepare_enable(drvdata->atclk);

	return 0;
}
#endif

static const struct dev_pm_ops replicator_dev_pm_ops = {
	SET_RUNTIME_PM_OPS(replicator_runtime_suspend,
			   replicator_runtime_resume, NULL)
};

static const struct of_device_id static_replicator_match[] = {
	{.compatible = "arm,coresight-replicator"},
	{.compatible = "arm,coresight-static-replicator"},
	{}
};

MODULE_DEVICE_TABLE(of, static_replicator_match);

#ifdef CONFIG_ACPI
static const struct acpi_device_id static_replicator_acpi_ids[] = {
	{"ARMHC985", 0}, /* ARM CoreSight Static Replicator */
	{}
};

MODULE_DEVICE_TABLE(acpi, static_replicator_acpi_ids);
#endif

static struct platform_driver static_replicator_driver = {
	.probe          = static_replicator_probe,
	.remove         = static_replicator_remove,
	.driver         = {
		.name   = "coresight-static-replicator",
		/* THIS_MODULE is taken care of by platform_driver_register() */
		.of_match_table = of_match_ptr(static_replicator_match),
		.acpi_match_table = ACPI_PTR(static_replicator_acpi_ids),
		.pm	= &replicator_dev_pm_ops,
		.suppress_bind_attrs = true,
	},
};

static int dynamic_replicator_probe(struct amba_device *adev,
				    const struct amba_id *id)
{
	return replicator_probe(&adev->dev, &adev->res);
}

static void dynamic_replicator_remove(struct amba_device *adev)
{
	replicator_remove(&adev->dev);
}

static const struct amba_id dynamic_replicator_ids[] = {
	CS_AMBA_ID(0x000bb909),
	CS_AMBA_ID(0x000bb9ec),		/* Coresight SoC-600 */
	{},
};

MODULE_DEVICE_TABLE(amba, dynamic_replicator_ids);

static struct amba_driver dynamic_replicator_driver = {
	.drv = {
		.name	= "coresight-dynamic-replicator",
		.pm	= &replicator_dev_pm_ops,
		.owner	= THIS_MODULE,
		.suppress_bind_attrs = true,
	},
	.probe		= dynamic_replicator_probe,
	.remove         = dynamic_replicator_remove,
	.id_table	= dynamic_replicator_ids,
};

static int __init replicator_init(void)
{
	int ret;

	ret = platform_driver_register(&static_replicator_driver);
	if (ret) {
		pr_info("Error registering platform driver\n");
		return ret;
	}

	ret = amba_driver_register(&dynamic_replicator_driver);
	if (ret) {
		pr_info("Error registering amba driver\n");
		platform_driver_unregister(&static_replicator_driver);
	}

	return ret;
}

static void __exit replicator_exit(void)
{
	platform_driver_unregister(&static_replicator_driver);
	amba_driver_unregister(&dynamic_replicator_driver);
}

module_init(replicator_init);
module_exit(replicator_exit);

MODULE_AUTHOR("Pratik Patel <pratikp@codeaurora.org>");
MODULE_AUTHOR("Mathieu Poirier <mathieu.poirier@linaro.org>");
MODULE_DESCRIPTION("Arm CoreSight Replicator Driver");
MODULE_LICENSE("GPL v2");
