// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright © 2015 Broadcom Corporation
 */

#include <linux/device.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#include "brcmnand.h"

struct iproc_nand_soc {
	struct brcmnand_soc soc;

	void __iomem *idm_base;
	void __iomem *ext_base;
	spinlock_t idm_lock;
};

#define IPROC_NAND_CTLR_READY_OFFSET       0x10
#define IPROC_NAND_CTLR_READY              BIT(0)

#define IPROC_NAND_IO_CTRL_OFFSET          0x00
#define IPROC_NAND_APB_LE_MODE             BIT(24)
#define IPROC_NAND_INT_CTRL_READ_ENABLE    BIT(6)

static bool iproc_nand_intc_ack(struct brcmnand_soc *soc)
{
	struct iproc_nand_soc *priv =
			container_of(soc, struct iproc_nand_soc, soc);
	void __iomem *mmio = priv->ext_base + IPROC_NAND_CTLR_READY_OFFSET;
	u32 val = brcmnand_readl(mmio);

	if (val & IPROC_NAND_CTLR_READY) {
		brcmnand_writel(IPROC_NAND_CTLR_READY, mmio);
		return true;
	}

	return false;
}

static void iproc_nand_intc_set(struct brcmnand_soc *soc, bool en)
{
	struct iproc_nand_soc *priv =
			container_of(soc, struct iproc_nand_soc, soc);
	void __iomem *mmio = priv->idm_base + IPROC_NAND_IO_CTRL_OFFSET;
	u32 val;
	unsigned long flags;

	spin_lock_irqsave(&priv->idm_lock, flags);

	val = brcmnand_readl(mmio);

	if (en)
		val |= IPROC_NAND_INT_CTRL_READ_ENABLE;
	else
		val &= ~IPROC_NAND_INT_CTRL_READ_ENABLE;

	brcmnand_writel(val, mmio);

	spin_unlock_irqrestore(&priv->idm_lock, flags);
}

static void iproc_nand_apb_access(struct brcmnand_soc *soc, bool prepare,
				  bool is_param)
{
	struct iproc_nand_soc *priv =
			container_of(soc, struct iproc_nand_soc, soc);
	void __iomem *mmio = priv->idm_base + IPROC_NAND_IO_CTRL_OFFSET;
	u32 val;
	unsigned long flags;

	spin_lock_irqsave(&priv->idm_lock, flags);

	val = brcmnand_readl(mmio);

	/*
	 * In the case of BE or when dealing with NAND data, alway configure
	 * the APB bus to LE mode before accessing the FIFO and back to BE mode
	 * after the access is done
	 */
	if (IS_ENABLED(CONFIG_CPU_BIG_ENDIAN) || !is_param) {
		if (prepare)
			val |= IPROC_NAND_APB_LE_MODE;
		else
			val &= ~IPROC_NAND_APB_LE_MODE;
	} else { /* when in LE accessing the parameter page, keep APB in BE */
		val &= ~IPROC_NAND_APB_LE_MODE;
	}

	brcmnand_writel(val, mmio);

	spin_unlock_irqrestore(&priv->idm_lock, flags);
}

static int iproc_nand_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct iproc_nand_soc *priv;
	struct brcmnand_soc *soc;
	struct resource *res;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;
	soc = &priv->soc;

	spin_lock_init(&priv->idm_lock);

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "iproc-idm");
	priv->idm_base = devm_ioremap_resource(dev, res);
	if (IS_ERR(priv->idm_base))
		return PTR_ERR(priv->idm_base);

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "iproc-ext");
	priv->ext_base = devm_ioremap_resource(dev, res);
	if (IS_ERR(priv->ext_base))
		return PTR_ERR(priv->ext_base);

	soc->ctlrdy_ack = iproc_nand_intc_ack;
	soc->ctlrdy_set_enabled = iproc_nand_intc_set;
	soc->prepare_data_bus = iproc_nand_apb_access;

	return brcmnand_probe(pdev, soc);
}

static const struct of_device_id iproc_nand_of_match[] = {
	{ .compatible = "brcm,nand-iproc" },
	{},
};
MODULE_DEVICE_TABLE(of, iproc_nand_of_match);

static struct platform_driver iproc_nand_driver = {
	.probe			= iproc_nand_probe,
	.remove			= brcmnand_remove,
	.driver = {
		.name		= "iproc_nand",
		.pm		= &brcmnand_pm_ops,
		.of_match_table	= iproc_nand_of_match,
	}
};
module_platform_driver(iproc_nand_driver);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Brian Norris");
MODULE_AUTHOR("Ray Jui");
MODULE_DESCRIPTION("NAND driver for Broadcom IPROC-based SoCs");
