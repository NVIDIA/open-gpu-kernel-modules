// SPDX-License-Identifier: GPL-2.0-only
/*
 * MEN Chameleon Bus.
 *
 * Copyright (C) 2014 MEN Mikroelektronik GmbH (www.men.de)
 * Author: Andreas Werner <andreas.werner@men.de>
 */

#include <linux/platform_device.h>
#include <linux/module.h>
#include <linux/dmi.h>
#include <linux/mcb.h>
#include <linux/io.h>
#include "mcb-internal.h"

struct priv {
	struct mcb_bus *bus;
	struct resource *mem;
	void __iomem *base;
};

static int mcb_lpc_probe(struct platform_device *pdev)
{
	struct resource *res;
	struct priv *priv;
	int ret = 0;

	priv = devm_kzalloc(&pdev->dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!priv->mem) {
		dev_err(&pdev->dev, "No Memory resource\n");
		return -ENODEV;
	}

	res = devm_request_mem_region(&pdev->dev, priv->mem->start,
				      resource_size(priv->mem),
				      KBUILD_MODNAME);
	if (!res) {
		dev_err(&pdev->dev, "Failed to request IO memory\n");
		return -EBUSY;
	}

	priv->base = devm_ioremap(&pdev->dev, priv->mem->start,
				  resource_size(priv->mem));
	if (!priv->base) {
		dev_err(&pdev->dev, "Cannot ioremap\n");
		return -ENOMEM;
	}

	platform_set_drvdata(pdev, priv);

	priv->bus = mcb_alloc_bus(&pdev->dev);
	if (IS_ERR(priv->bus))
		return PTR_ERR(priv->bus);

	ret = chameleon_parse_cells(priv->bus, priv->mem->start, priv->base);
	if (ret < 0) {
		mcb_release_bus(priv->bus);
		return ret;
	}

	dev_dbg(&pdev->dev, "Found %d cells\n", ret);

	mcb_bus_add_devices(priv->bus);

	return 0;

}

static int mcb_lpc_remove(struct platform_device *pdev)
{
	struct priv *priv = platform_get_drvdata(pdev);

	mcb_release_bus(priv->bus);

	return 0;
}

static struct platform_device *mcb_lpc_pdev;

static int mcb_lpc_create_platform_device(const struct dmi_system_id *id)
{
	struct resource *res = id->driver_data;
	int ret;

	mcb_lpc_pdev = platform_device_alloc("mcb-lpc", -1);
	if (!mcb_lpc_pdev)
		return -ENOMEM;

	ret = platform_device_add_resources(mcb_lpc_pdev, res, 1);
	if (ret)
		goto out_put;

	ret = platform_device_add(mcb_lpc_pdev);
	if (ret)
		goto out_put;

	return 0;

out_put:
	platform_device_put(mcb_lpc_pdev);
	return ret;
}

static struct resource sc24_fpga_resource = {
	.start = 0xe000e000,
	.end = 0xe000e000 + CHAM_HEADER_SIZE,
	.flags = IORESOURCE_MEM,
};

static struct resource sc31_fpga_resource = {
	.start = 0xf000e000,
	.end = 0xf000e000 + CHAM_HEADER_SIZE,
	.flags = IORESOURCE_MEM,
};

static struct platform_driver mcb_lpc_driver = {
	.driver		= {
		.name = "mcb-lpc",
	},
	.probe		= mcb_lpc_probe,
	.remove		= mcb_lpc_remove,
};

static const struct dmi_system_id mcb_lpc_dmi_table[] = {
	{
		.ident = "SC24",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "MEN"),
			DMI_MATCH(DMI_PRODUCT_VERSION, "14SC24"),
		},
		.driver_data = (void *)&sc24_fpga_resource,
		.callback = mcb_lpc_create_platform_device,
	},
	{
		.ident = "SC31",
		.matches = {
			DMI_MATCH(DMI_SYS_VENDOR, "MEN"),
			DMI_MATCH(DMI_PRODUCT_VERSION, "14SC31"),
		},
		.driver_data = (void *)&sc31_fpga_resource,
		.callback = mcb_lpc_create_platform_device,
	},
	{}
};
MODULE_DEVICE_TABLE(dmi, mcb_lpc_dmi_table);

static int __init mcb_lpc_init(void)
{
	if (!dmi_check_system(mcb_lpc_dmi_table))
		return -ENODEV;

	return platform_driver_register(&mcb_lpc_driver);
}

static void __exit mcb_lpc_exit(void)
{
	platform_device_unregister(mcb_lpc_pdev);
	platform_driver_unregister(&mcb_lpc_driver);
}

module_init(mcb_lpc_init);
module_exit(mcb_lpc_exit);

MODULE_AUTHOR("Andreas Werner <andreas.werner@men.de>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MCB over LPC support");
MODULE_IMPORT_NS(MCB);
