// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2011-2012, The Linux Foundation. All rights reserved.
 *
 * Description: CoreSight Trace Port Interface Unit driver
 */

#include <linux/atomic.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/pm_runtime.h>
#include <linux/coresight.h>
#include <linux/amba/bus.h>
#include <linux/clk.h>

#include "coresight-priv.h"

#define TPIU_SUPP_PORTSZ	0x000
#define TPIU_CURR_PORTSZ	0x004
#define TPIU_SUPP_TRIGMODES	0x100
#define TPIU_TRIG_CNTRVAL	0x104
#define TPIU_TRIG_MULT		0x108
#define TPIU_SUPP_TESTPATM	0x200
#define TPIU_CURR_TESTPATM	0x204
#define TPIU_TEST_PATREPCNTR	0x208
#define TPIU_FFSR		0x300
#define TPIU_FFCR		0x304
#define TPIU_FSYNC_CNTR		0x308
#define TPIU_EXTCTL_INPORT	0x400
#define TPIU_EXTCTL_OUTPORT	0x404
#define TPIU_ITTRFLINACK	0xee4
#define TPIU_ITTRFLIN		0xee8
#define TPIU_ITATBDATA0		0xeec
#define TPIU_ITATBCTR2		0xef0
#define TPIU_ITATBCTR1		0xef4
#define TPIU_ITATBCTR0		0xef8

/** register definition **/
/* FFSR - 0x300 */
#define FFSR_FT_STOPPED_BIT	1
/* FFCR - 0x304 */
#define FFCR_FON_MAN_BIT	6
#define FFCR_FON_MAN		BIT(6)
#define FFCR_STOP_FI		BIT(12)

DEFINE_CORESIGHT_DEVLIST(tpiu_devs, "tpiu");

/*
 * @base:	memory mapped base address for this component.
 * @atclk:	optional clock for the core parts of the TPIU.
 * @csdev:	component vitals needed by the framework.
 */
struct tpiu_drvdata {
	void __iomem		*base;
	struct clk		*atclk;
	struct coresight_device	*csdev;
};

static void tpiu_enable_hw(struct csdev_access *csa)
{
	CS_UNLOCK(csa->base);

	/* TODO: fill this up */

	CS_LOCK(csa->base);
}

static int tpiu_enable(struct coresight_device *csdev, u32 mode, void *__unused)
{
	tpiu_enable_hw(&csdev->access);
	atomic_inc(csdev->refcnt);
	dev_dbg(&csdev->dev, "TPIU enabled\n");
	return 0;
}

static void tpiu_disable_hw(struct csdev_access *csa)
{
	CS_UNLOCK(csa->base);

	/* Clear formatter and stop on flush */
	csdev_access_relaxed_write32(csa, FFCR_STOP_FI, TPIU_FFCR);
	/* Generate manual flush */
	csdev_access_relaxed_write32(csa, FFCR_STOP_FI | FFCR_FON_MAN, TPIU_FFCR);
	/* Wait for flush to complete */
	coresight_timeout(csa, TPIU_FFCR, FFCR_FON_MAN_BIT, 0);
	/* Wait for formatter to stop */
	coresight_timeout(csa, TPIU_FFSR, FFSR_FT_STOPPED_BIT, 1);

	CS_LOCK(csa->base);
}

static int tpiu_disable(struct coresight_device *csdev)
{
	if (atomic_dec_return(csdev->refcnt))
		return -EBUSY;

	tpiu_disable_hw(&csdev->access);

	dev_dbg(&csdev->dev, "TPIU disabled\n");
	return 0;
}

static const struct coresight_ops_sink tpiu_sink_ops = {
	.enable		= tpiu_enable,
	.disable	= tpiu_disable,
};

static const struct coresight_ops tpiu_cs_ops = {
	.sink_ops	= &tpiu_sink_ops,
};

static int tpiu_probe(struct amba_device *adev, const struct amba_id *id)
{
	int ret;
	void __iomem *base;
	struct device *dev = &adev->dev;
	struct coresight_platform_data *pdata = NULL;
	struct tpiu_drvdata *drvdata;
	struct resource *res = &adev->res;
	struct coresight_desc desc = { 0 };

	desc.name = coresight_alloc_device_name(&tpiu_devs, dev);
	if (!desc.name)
		return -ENOMEM;

	drvdata = devm_kzalloc(dev, sizeof(*drvdata), GFP_KERNEL);
	if (!drvdata)
		return -ENOMEM;

	drvdata->atclk = devm_clk_get(&adev->dev, "atclk"); /* optional */
	if (!IS_ERR(drvdata->atclk)) {
		ret = clk_prepare_enable(drvdata->atclk);
		if (ret)
			return ret;
	}
	dev_set_drvdata(dev, drvdata);

	/* Validity for the resource is already checked by the AMBA core */
	base = devm_ioremap_resource(dev, res);
	if (IS_ERR(base))
		return PTR_ERR(base);

	drvdata->base = base;
	desc.access = CSDEV_ACCESS_IOMEM(base);

	/* Disable tpiu to support older devices */
	tpiu_disable_hw(&desc.access);

	pdata = coresight_get_platform_data(dev);
	if (IS_ERR(pdata))
		return PTR_ERR(pdata);
	dev->platform_data = pdata;

	desc.type = CORESIGHT_DEV_TYPE_SINK;
	desc.subtype.sink_subtype = CORESIGHT_DEV_SUBTYPE_SINK_PORT;
	desc.ops = &tpiu_cs_ops;
	desc.pdata = pdata;
	desc.dev = dev;
	drvdata->csdev = coresight_register(&desc);

	if (!IS_ERR(drvdata->csdev)) {
		pm_runtime_put(&adev->dev);
		return 0;
	}

	return PTR_ERR(drvdata->csdev);
}

static void tpiu_remove(struct amba_device *adev)
{
	struct tpiu_drvdata *drvdata = dev_get_drvdata(&adev->dev);

	coresight_unregister(drvdata->csdev);
}

#ifdef CONFIG_PM
static int tpiu_runtime_suspend(struct device *dev)
{
	struct tpiu_drvdata *drvdata = dev_get_drvdata(dev);

	if (drvdata && !IS_ERR(drvdata->atclk))
		clk_disable_unprepare(drvdata->atclk);

	return 0;
}

static int tpiu_runtime_resume(struct device *dev)
{
	struct tpiu_drvdata *drvdata = dev_get_drvdata(dev);

	if (drvdata && !IS_ERR(drvdata->atclk))
		clk_prepare_enable(drvdata->atclk);

	return 0;
}
#endif

static const struct dev_pm_ops tpiu_dev_pm_ops = {
	SET_RUNTIME_PM_OPS(tpiu_runtime_suspend, tpiu_runtime_resume, NULL)
};

static const struct amba_id tpiu_ids[] = {
	{
		.id	= 0x000bb912,
		.mask	= 0x000fffff,
	},
	{
		.id	= 0x0004b912,
		.mask	= 0x0007ffff,
	},
	{
		/* Coresight SoC-600 */
		.id	= 0x000bb9e7,
		.mask	= 0x000fffff,
	},
	{ 0, 0},
};

MODULE_DEVICE_TABLE(amba, tpiu_ids);

static struct amba_driver tpiu_driver = {
	.drv = {
		.name	= "coresight-tpiu",
		.owner	= THIS_MODULE,
		.pm	= &tpiu_dev_pm_ops,
		.suppress_bind_attrs = true,
	},
	.probe		= tpiu_probe,
	.remove         = tpiu_remove,
	.id_table	= tpiu_ids,
};

module_amba_driver(tpiu_driver);

MODULE_AUTHOR("Pratik Patel <pratikp@codeaurora.org>");
MODULE_AUTHOR("Mathieu Poirier <mathieu.poirier@linaro.org>");
MODULE_DESCRIPTION("Arm CoreSight TPIU (Trace Port Interface Unit) driver");
MODULE_LICENSE("GPL v2");
