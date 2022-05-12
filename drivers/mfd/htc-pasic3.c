// SPDX-License-Identifier: GPL-2.0-only
/*
 * Core driver for HTC PASIC3 LED/DS1WM chip.
 *
 * Copyright (C) 2006 Philipp Zabel <philipp.zabel@gmail.com>
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include <linux/gpio.h>
#include <linux/io.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/mfd/core.h>
#include <linux/mfd/ds1wm.h>
#include <linux/mfd/htc-pasic3.h>
#include <linux/slab.h>

struct pasic3_data {
	void __iomem *mapping;
	unsigned int bus_shift;
};

#define REG_ADDR  5
#define REG_DATA  6

#define READ_MODE 0x80

/*
 * write to a secondary register on the PASIC3
 */
void pasic3_write_register(struct device *dev, u32 reg, u8 val)
{
	struct pasic3_data *asic = dev_get_drvdata(dev);
	int bus_shift = asic->bus_shift;
	void __iomem *addr = asic->mapping + (REG_ADDR << bus_shift);
	void __iomem *data = asic->mapping + (REG_DATA << bus_shift);

	__raw_writeb(~READ_MODE & reg, addr);
	__raw_writeb(val, data);
}
EXPORT_SYMBOL(pasic3_write_register); /* for leds-pasic3 */

/*
 * read from a secondary register on the PASIC3
 */
u8 pasic3_read_register(struct device *dev, u32 reg)
{
	struct pasic3_data *asic = dev_get_drvdata(dev);
	int bus_shift = asic->bus_shift;
	void __iomem *addr = asic->mapping + (REG_ADDR << bus_shift);
	void __iomem *data = asic->mapping + (REG_DATA << bus_shift);

	__raw_writeb(READ_MODE | reg, addr);
	return __raw_readb(data);
}
EXPORT_SYMBOL(pasic3_read_register); /* for leds-pasic3 */

/*
 * LEDs
 */

static struct mfd_cell led_cell __initdata = {
	.name = "leds-pasic3",
};

/*
 * DS1WM
 */

static int ds1wm_enable(struct platform_device *pdev)
{
	struct device *dev = pdev->dev.parent;
	int c;

	c = pasic3_read_register(dev, 0x28);
	pasic3_write_register(dev, 0x28, c & 0x7f);

	dev_dbg(dev, "DS1WM OWM_EN low (active) %02x\n", c & 0x7f);
	return 0;
}

static int ds1wm_disable(struct platform_device *pdev)
{
	struct device *dev = pdev->dev.parent;
	int c;

	c = pasic3_read_register(dev, 0x28);
	pasic3_write_register(dev, 0x28, c | 0x80);

	dev_dbg(dev, "DS1WM OWM_EN high (inactive) %02x\n", c | 0x80);
	return 0;
}

static struct ds1wm_driver_data ds1wm_pdata = {
	.active_high = 0,
	.reset_recover_delay = 1,
};

static struct resource ds1wm_resources[] __initdata = {
	[0] = {
		.start  = 0,
		.flags  = IORESOURCE_MEM,
	},
	[1] = {
		.start  = 0,
		.end    = 0,
		.flags  = IORESOURCE_IRQ,
	},
};

static const struct mfd_cell ds1wm_cell __initconst = {
	.name          = "ds1wm",
	.enable        = ds1wm_enable,
	.disable       = ds1wm_disable,
	.platform_data = &ds1wm_pdata,
	.pdata_size    = sizeof(ds1wm_pdata),
	.num_resources = 2,
	.resources     = ds1wm_resources,
};

static int __init pasic3_probe(struct platform_device *pdev)
{
	struct pasic3_platform_data *pdata = dev_get_platdata(&pdev->dev);
	struct device *dev = &pdev->dev;
	struct pasic3_data *asic;
	struct resource *r;
	int ret;
	int irq = 0;

	r = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (r) {
		ds1wm_resources[1].flags = IORESOURCE_IRQ | (r->flags &
			(IORESOURCE_IRQ_HIGHEDGE | IORESOURCE_IRQ_LOWEDGE));
		irq = r->start;
	}

	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!r)
		return -ENXIO;

	if (!request_mem_region(r->start, resource_size(r), "pasic3"))
		return -EBUSY;

	asic = devm_kzalloc(dev, sizeof(struct pasic3_data), GFP_KERNEL);
	if (!asic)
		return -ENOMEM;

	platform_set_drvdata(pdev, asic);

	asic->mapping = ioremap(r->start, resource_size(r));
	if (!asic->mapping) {
		dev_err(dev, "couldn't ioremap PASIC3\n");
		return -ENOMEM;
	}

	/* calculate bus shift from mem resource */
	asic->bus_shift = (resource_size(r) - 5) >> 3;

	if (pdata && pdata->clock_rate) {
		ds1wm_pdata.clock_rate = pdata->clock_rate;
		/* the first 5 PASIC3 registers control the DS1WM */
		ds1wm_resources[0].end = (5 << asic->bus_shift) - 1;
		ret = mfd_add_devices(&pdev->dev, pdev->id,
				      &ds1wm_cell, 1, r, irq, NULL);
		if (ret < 0)
			dev_warn(dev, "failed to register DS1WM\n");
	}

	if (pdata && pdata->led_pdata) {
		led_cell.platform_data = pdata->led_pdata;
		led_cell.pdata_size = sizeof(struct pasic3_leds_machinfo);
		ret = mfd_add_devices(&pdev->dev, pdev->id, &led_cell, 1, r,
				      0, NULL);
		if (ret < 0)
			dev_warn(dev, "failed to register LED device\n");
	}

	return 0;
}

static int pasic3_remove(struct platform_device *pdev)
{
	struct pasic3_data *asic = platform_get_drvdata(pdev);
	struct resource *r;

	mfd_remove_devices(&pdev->dev);

	iounmap(asic->mapping);
	r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	release_mem_region(r->start, resource_size(r));
	return 0;
}

MODULE_ALIAS("platform:pasic3");

static struct platform_driver pasic3_driver = {
	.driver		= {
		.name	= "pasic3",
	},
	.remove		= pasic3_remove,
};

module_platform_driver_probe(pasic3_driver, pasic3_probe);

MODULE_AUTHOR("Philipp Zabel <philipp.zabel@gmail.com>");
MODULE_DESCRIPTION("Core driver for HTC PASIC3");
MODULE_LICENSE("GPL");
