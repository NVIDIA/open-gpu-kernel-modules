// SPDX-License-Identifier: GPL-2.0
/*
 * Toshiba Visconti GPIO Support
 *
 * (C) Copyright 2020 Toshiba Electronic Devices & Storage Corporation
 * (C) Copyright 2020 TOSHIBA CORPORATION
 *
 * Nobuhiro Iwamatsu <nobuhiro1.iwamatsu@toshiba.co.jp>
 */

#include <linux/gpio/driver.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/of_irq.h>
#include <linux/platform_device.h>
#include <linux/bitops.h>

/* register offset */
#define GPIO_DIR	0x00
#define GPIO_IDATA	0x08
#define GPIO_ODATA	0x10
#define GPIO_OSET	0x18
#define GPIO_OCLR	0x20
#define GPIO_INTMODE	0x30

#define BASE_HW_IRQ 24

struct visconti_gpio {
	void __iomem *base;
	spinlock_t lock; /* protect gpio register */
	struct gpio_chip gpio_chip;
	struct irq_chip irq_chip;
};

static int visconti_gpio_irq_set_type(struct irq_data *d, unsigned int type)
{
	struct gpio_chip *gc = irq_data_get_irq_chip_data(d);
	struct visconti_gpio *priv = gpiochip_get_data(gc);
	u32 offset = irqd_to_hwirq(d);
	u32 bit = BIT(offset);
	u32 intc_type = IRQ_TYPE_EDGE_RISING;
	u32 intmode, odata;
	int ret = 0;
	unsigned long flags;

	spin_lock_irqsave(&priv->lock, flags);

	odata = readl(priv->base + GPIO_ODATA);
	intmode = readl(priv->base + GPIO_INTMODE);

	switch (type) {
	case IRQ_TYPE_EDGE_RISING:
		odata &= ~bit;
		intmode &= ~bit;
		break;
	case IRQ_TYPE_EDGE_FALLING:
		odata |= bit;
		intmode &= ~bit;
		break;
	case IRQ_TYPE_EDGE_BOTH:
		intmode |= bit;
		break;
	case IRQ_TYPE_LEVEL_HIGH:
		intc_type = IRQ_TYPE_LEVEL_HIGH;
		odata &= ~bit;
		intmode &= ~bit;
		break;
	case IRQ_TYPE_LEVEL_LOW:
		intc_type = IRQ_TYPE_LEVEL_HIGH;
		odata |= bit;
		intmode &= ~bit;
		break;
	default:
		ret = -EINVAL;
		goto err;
	}

	writel(odata, priv->base + GPIO_ODATA);
	writel(intmode, priv->base + GPIO_INTMODE);
	irq_set_irq_type(offset, intc_type);

	ret = irq_chip_set_type_parent(d, type);
err:
	spin_unlock_irqrestore(&priv->lock, flags);
	return ret;
}

static int visconti_gpio_child_to_parent_hwirq(struct gpio_chip *gc,
					       unsigned int child,
					       unsigned int child_type,
					       unsigned int *parent,
					       unsigned int *parent_type)
{
	/* Interrupts 0..15 mapped to interrupts 24..39 on the GIC */
	if (child < 16) {
		/* All these interrupts are level high in the CPU */
		*parent_type = IRQ_TYPE_LEVEL_HIGH;
		*parent = child + BASE_HW_IRQ;
		return 0;
	}
	return -EINVAL;
}

static void *visconti_gpio_populate_parent_fwspec(struct gpio_chip *chip,
						  unsigned int parent_hwirq,
						  unsigned int parent_type)
{
	struct irq_fwspec *fwspec;

	fwspec = kmalloc(sizeof(*fwspec), GFP_KERNEL);
	if (!fwspec)
		return NULL;

	fwspec->fwnode = chip->irq.parent_domain->fwnode;
	fwspec->param_count = 3;
	fwspec->param[0] = 0;
	fwspec->param[1] = parent_hwirq;
	fwspec->param[2] = parent_type;

	return fwspec;
}

static int visconti_gpio_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct visconti_gpio *priv;
	struct irq_chip *irq_chip;
	struct gpio_irq_chip *girq;
	struct irq_domain *parent;
	struct device_node *irq_parent;
	struct fwnode_handle *fwnode;
	int ret;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	spin_lock_init(&priv->lock);

	priv->base = devm_platform_ioremap_resource(pdev, 0);
	if (IS_ERR(priv->base))
		return PTR_ERR(priv->base);

	irq_parent = of_irq_find_parent(dev->of_node);
	if (!irq_parent) {
		dev_err(dev, "No IRQ parent node\n");
		return -ENODEV;
	}

	parent = irq_find_host(irq_parent);
	if (!parent) {
		dev_err(dev, "No IRQ parent domain\n");
		return -ENODEV;
	}

	fwnode = of_node_to_fwnode(irq_parent);
	of_node_put(irq_parent);

	ret = bgpio_init(&priv->gpio_chip, dev, 4,
			 priv->base + GPIO_IDATA,
			 priv->base + GPIO_OSET,
			 priv->base + GPIO_OCLR,
			 priv->base + GPIO_DIR,
			 NULL,
			 0);
	if (ret) {
		dev_err(dev, "unable to init generic GPIO\n");
		return ret;
	}

	irq_chip = &priv->irq_chip;
	irq_chip->name = dev_name(dev);
	irq_chip->irq_mask = irq_chip_mask_parent;
	irq_chip->irq_unmask = irq_chip_unmask_parent;
	irq_chip->irq_eoi = irq_chip_eoi_parent;
	irq_chip->irq_set_type = visconti_gpio_irq_set_type;
	irq_chip->flags = IRQCHIP_SET_TYPE_MASKED | IRQCHIP_MASK_ON_SUSPEND;

	girq = &priv->gpio_chip.irq;
	girq->chip = irq_chip;
	girq->fwnode = fwnode;
	girq->parent_domain = parent;
	girq->child_to_parent_hwirq = visconti_gpio_child_to_parent_hwirq;
	girq->populate_parent_alloc_arg = visconti_gpio_populate_parent_fwspec;
	girq->default_type = IRQ_TYPE_NONE;
	girq->handler = handle_level_irq;

	ret = devm_gpiochip_add_data(dev, &priv->gpio_chip, priv);
	if (ret) {
		dev_err(dev, "failed to add GPIO chip\n");
		return ret;
	}

	platform_set_drvdata(pdev, priv);

	return ret;
}

static const struct of_device_id visconti_gpio_of_match[] = {
	{ .compatible = "toshiba,gpio-tmpv7708", },
	{ /* end of table */ }
};
MODULE_DEVICE_TABLE(of, visconti_gpio_of_match);

static struct platform_driver visconti_gpio_driver = {
	.probe		= visconti_gpio_probe,
	.driver		= {
		.name	= "visconti_gpio",
		.of_match_table = of_match_ptr(visconti_gpio_of_match),
	}
};
module_platform_driver(visconti_gpio_driver);

MODULE_AUTHOR("Nobuhiro Iwamatsu <nobuhiro1.iwamatsu@toshiba.co.jp>");
MODULE_DESCRIPTION("Toshiba Visconti GPIO Driver");
MODULE_LICENSE("GPL v2");
