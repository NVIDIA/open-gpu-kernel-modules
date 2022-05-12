// SPDX-License-Identifier: GPL-2.0+
/*
 * rcar-fcp.c  --  R-Car Frame Compression Processor Driver
 *
 * Copyright (C) 2016 Renesas Electronics Corporation
 *
 * Contact: Laurent Pinchart (laurent.pinchart@ideasonboard.com)
 */

#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/mod_devicetable.h>
#include <linux/mutex.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/slab.h>

#include <media/rcar-fcp.h>

struct rcar_fcp_device {
	struct list_head list;
	struct device *dev;
};

static LIST_HEAD(fcp_devices);
static DEFINE_MUTEX(fcp_lock);

/* -----------------------------------------------------------------------------
 * Public API
 */

/**
 * rcar_fcp_get - Find and acquire a reference to an FCP instance
 * @np: Device node of the FCP instance
 *
 * Search the list of registered FCP instances for the instance corresponding to
 * the given device node.
 *
 * Return a pointer to the FCP instance, or an ERR_PTR if the instance can't be
 * found.
 */
struct rcar_fcp_device *rcar_fcp_get(const struct device_node *np)
{
	struct rcar_fcp_device *fcp;

	mutex_lock(&fcp_lock);

	list_for_each_entry(fcp, &fcp_devices, list) {
		if (fcp->dev->of_node != np)
			continue;

		get_device(fcp->dev);
		goto done;
	}

	fcp = ERR_PTR(-EPROBE_DEFER);

done:
	mutex_unlock(&fcp_lock);
	return fcp;
}
EXPORT_SYMBOL_GPL(rcar_fcp_get);

/**
 * rcar_fcp_put - Release a reference to an FCP instance
 * @fcp: The FCP instance
 *
 * Release the FCP instance acquired by a call to rcar_fcp_get().
 */
void rcar_fcp_put(struct rcar_fcp_device *fcp)
{
	if (fcp)
		put_device(fcp->dev);
}
EXPORT_SYMBOL_GPL(rcar_fcp_put);

struct device *rcar_fcp_get_device(struct rcar_fcp_device *fcp)
{
	return fcp->dev;
}
EXPORT_SYMBOL_GPL(rcar_fcp_get_device);

/**
 * rcar_fcp_enable - Enable an FCP
 * @fcp: The FCP instance
 *
 * Before any memory access through an FCP is performed by a module, the FCP
 * must be enabled by a call to this function. The enable calls are reference
 * counted, each successful call must be followed by one rcar_fcp_disable()
 * call when no more memory transfer can occur through the FCP.
 *
 * Return 0 on success or a negative error code if an error occurs. The enable
 * reference count isn't increased when this function returns an error.
 */
int rcar_fcp_enable(struct rcar_fcp_device *fcp)
{
	int ret;

	if (!fcp)
		return 0;

	ret = pm_runtime_get_sync(fcp->dev);
	if (ret < 0) {
		pm_runtime_put_noidle(fcp->dev);
		return ret;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(rcar_fcp_enable);

/**
 * rcar_fcp_disable - Disable an FCP
 * @fcp: The FCP instance
 *
 * This function is the counterpart of rcar_fcp_enable(). As enable calls are
 * reference counted a disable call may not disable the FCP synchronously.
 */
void rcar_fcp_disable(struct rcar_fcp_device *fcp)
{
	if (fcp)
		pm_runtime_put(fcp->dev);
}
EXPORT_SYMBOL_GPL(rcar_fcp_disable);

/* -----------------------------------------------------------------------------
 * Platform Driver
 */

static int rcar_fcp_probe(struct platform_device *pdev)
{
	struct rcar_fcp_device *fcp;

	fcp = devm_kzalloc(&pdev->dev, sizeof(*fcp), GFP_KERNEL);
	if (fcp == NULL)
		return -ENOMEM;

	fcp->dev = &pdev->dev;

	dma_set_max_seg_size(fcp->dev, UINT_MAX);

	pm_runtime_enable(&pdev->dev);

	mutex_lock(&fcp_lock);
	list_add_tail(&fcp->list, &fcp_devices);
	mutex_unlock(&fcp_lock);

	platform_set_drvdata(pdev, fcp);

	return 0;
}

static int rcar_fcp_remove(struct platform_device *pdev)
{
	struct rcar_fcp_device *fcp = platform_get_drvdata(pdev);

	mutex_lock(&fcp_lock);
	list_del(&fcp->list);
	mutex_unlock(&fcp_lock);

	pm_runtime_disable(&pdev->dev);

	return 0;
}

static const struct of_device_id rcar_fcp_of_match[] = {
	{ .compatible = "renesas,fcpf" },
	{ .compatible = "renesas,fcpv" },
	{ },
};
MODULE_DEVICE_TABLE(of, rcar_fcp_of_match);

static struct platform_driver rcar_fcp_platform_driver = {
	.probe		= rcar_fcp_probe,
	.remove		= rcar_fcp_remove,
	.driver		= {
		.name	= "rcar-fcp",
		.of_match_table = rcar_fcp_of_match,
		.suppress_bind_attrs = true,
	},
};

module_platform_driver(rcar_fcp_platform_driver);

MODULE_ALIAS("rcar-fcp");
MODULE_AUTHOR("Laurent Pinchart <laurent.pinchart@ideasonboard.com>");
MODULE_DESCRIPTION("Renesas FCP Driver");
MODULE_LICENSE("GPL");
